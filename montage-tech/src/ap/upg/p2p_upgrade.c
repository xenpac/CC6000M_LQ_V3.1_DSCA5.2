/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_warriors.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mem_manager.h"

#include "hal_uart.h"
#include "drv_dev.h"
#include "charsto.h"
#include "uio.h"
#include "fcrc.h"

#include "err_check_def.h"
#include "class_factory.h"
#include "data_manager.h"
#include "ap_framework.h"
#include "ap_upgrade.h"
#include "ap_upg_priv.h"
#include "unzip.h"
#include "lzmaif.h"

//#include "sys_cfg.h"

#define CRC_MODE                  CRC32_ARITHMETIC
#define CRC_INIT                  0xFFFFFFFF

#define BOOTROM_SYNC_BYTE         0xA5
#define BOOTROM_RESPONSE_BYTE     0x60
#define P2P_PACKET_SIZE           1024

#define P2P_START_INDICATOR1      0x23 //'#'
#define P2P_START_INDICATOR2      0x2A //'*'

#define P2P_CMD_RETYR_TIME        3
#define P2P_CMD_OK                0x4F // 'O', slave is OK
#define P2P_CMD_ERROR             0x45 //'E', slave is Error

#define P2P_CMD_MAX_LEN           100 //normal case, UPG client check if it <120
#define BL_SEC_OFFSET             8   //bootloader size store position
#define FLASH_SEC_SIZE            CHARSTO_SECTOR_SIZE

#define REG8(addr)                 (*((volatile u8 *)(addr)))

extern u32 get_flash_addr(void);

/*!
  Define the p2p commands
 */
typedef enum
{
  P2P_CMD_TEST = 0x50,
  P2P_CMD_COLLECT,
  P2P_CMD_INFORM,
  P2P_CMD_TRANSFER,
  P2P_CMD_BURN,
  P2P_CMD_REBOOT,
}p2p_cmd_id_t;

//Internal functions
#ifdef WIN32
#include "uart_win32.h"

static upg_ret_t p2p_get_data(u8 *p_buff, u32 len, u32 timeout)
{
  u32 i = 0;

  for(i = 0; i < len; i++)
  {
    if(0 != uart_read_byte(0, &p_buff[i], timeout))
    {
      return UPG_RET_ERROR;
    }
  }
  return UPG_RET_SUCCESS;
}


static u32 p2p_send_data(u8 *p_buff, u32 len)
{
  uart_write(0, p_buff, len);

  return len;
}


static void p2p_clear_data()
{
  uart_flush(0);
}

#else  //for actual board
#define FP_START_WORK 0
#define FP_STOP_WORK  1

static upg_ret_t p2p_get_data(u8 *p_buff, u32 len, u32 timeout)
{
  u32 i = 0;

  for(i = 0; i < len; i++)
  {
    if(0 != uart_read_byte_polling(UART_UPG_ID, &p_buff[i], timeout))
    {
      return UPG_RET_ERROR;
    }
  }
  return UPG_RET_SUCCESS;
}


static u32 p2p_send_data(u8 *p_buff, u32 len)
{
  u32 i = 0;
  s32 w_len = 0;

  for(i = 0; i < len; i++)
  {
    if (uart_write_byte(UART_UPG_ID, p_buff[i]) == SUCCESS)
    {
      w_len ++;
    }
  }
  
  #ifdef _P2P_DEBUG
  OS_PRINTF("\n##uart_write_byte [%d, %d]\n", w_len, len);
  #endif
  return w_len;
}


static void p2p_clear_data()
{
  uart_flush(UART_UPG_ID);
}
#endif  //end of #ifdef WIN32

/*!
  Malloc/free from video partition
 */
void *p2p_malloc(u32 size)
{
  void *mem_ptr = NULL;

  mem_ptr = mtos_malloc(size);
  CHECK_FAIL_RET_NULL(mem_ptr != NULL);

  return mem_ptr;
}


void p2p_free(void *addr)
{
  mtos_free(addr);
}

/*!
  read flash data, hide the device pointer for p2p function call
 */
static RET_CODE p2p_read_flash_data(u32 addr, u8 *p_buf, u32 len)
{
  charsto_device_t *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                                SYS_DEV_TYPE_CHARSTO);

  return charsto_read(p_dev, addr, p_buf, len);
}


/*!
  Send the upgclient to slave
 */
static upg_ret_t p2p_send_client(upg_priv_t *p_priv)
{
  u32 brom[20] = {0x4, 0x41000000, UART_UPG_BP, 0, 0, 0};
  u32 len = 0, l = 0, i = 0;
  u8 *p_buff = NULL;
  upg_config_t *p_cfg = &p_priv->upg_data.cfg;
  u32 out_size = 0;

  //malloc buffer for packet transfer
  p_buff = (u8 *)p2p_malloc(P2P_PACKET_SIZE);

  CHECK_FAIL_RET((NULL != p_buff), UPG_RET_ERROR);
  memset(p_buff, 0, P2P_PACKET_SIZE);

  CHECK_FAIL_RET((NULL != p_cfg->p_unzip_buffer), UPG_RET_ERROR);
  memset(p_cfg->p_unzip_buffer, 0, p_cfg->unzip_size);

  CHECK_FAIL_RET((NULL != p_cfg->p_zip_buffer), UPG_RET_ERROR);
  memset(p_cfg->p_zip_buffer, 0, p_cfg->zip_size);

  CHECK_FAIL_RET((NULL != p_cfg->p_inflate_buffer), UPG_RET_ERROR);
  memset(p_cfg->p_inflate_buffer, 0, p_cfg->inflate_size);

  if(p_cfg->zip_type == UPG_ZIP_LZMA)
  {
    if(SUCCESS != p2p_read_flash_data(
      p_cfg->upg_client_addr, p_cfg->p_zip_buffer, p_cfg->zip_size))
    {
      upg_update_status(p_priv, UPG_SM_MASETER_ERROR, ERR_SYNC_READ_FLASH_ERR,
                        "sending client: read flash error!");

      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }

      return UPG_RET_ERROR;
    }

    init_fake_mem_lzma(p_cfg->p_inflate_buffer, p_cfg->inflate_size);
    out_size = p_cfg->unzip_size;

    if(0 != lzma_decompress((void *)p_cfg->p_unzip_buffer, &out_size,
      (void *)p_cfg->p_zip_buffer, p_cfg->upg_client_size))
    {
      upg_update_status(p_priv, UPG_SM_MASETER_ERROR, ERR_SYNC_UNZIP_ERR,
                        "sending client: unzip data error!");
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }
      return UPG_RET_ERROR;
    }
  }
  else if(p_cfg->zip_type == UPG_ZIP_GZIP)
  {
    if(SUCCESS != p2p_read_flash_data(
      p_cfg->upg_client_addr, p_cfg->p_zip_buffer, p_cfg->zip_size))
    {
      upg_update_status(p_priv, UPG_SM_MASETER_ERROR, ERR_SYNC_READ_FLASH_ERR,
                        "sending client: read flash error!");

      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }

      return UPG_RET_ERROR;
    }

    init_fake_mem(p_cfg->p_inflate_buffer);
    if(0 != gunzip(p_cfg->p_zip_buffer, p_cfg->p_unzip_buffer,
      p_cfg->upg_client_size, &out_size, memcpy))
    {
      upg_update_status(p_priv, UPG_SM_MASETER_ERROR, ERR_SYNC_UNZIP_ERR,
                        "sending client: unzip data error!");
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }
      return UPG_RET_ERROR;
    }
  }
  else if(p_cfg->zip_type == UPG_ZIP_NONE)
  {
    //read upg client to RAM
    if(SUCCESS != p2p_read_flash_data(p_cfg->upg_client_addr,
      p_cfg->p_unzip_buffer, p_cfg->upg_client_size))
    {
      upg_update_status(p_priv, UPG_SM_MASETER_ERROR, ERR_SYNC_READ_FLASH_ERR,
                        "sending client: read flash error!");

      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }

      return UPG_RET_ERROR;
    }

    out_size = p_cfg->upg_client_size;
  }
  else
  {
    CHECK_FAIL_RET(0, UPG_RET_ERROR);
  }

  upg_update_status(p_priv, UPG_GET_STS(p_priv), 10, NULL);

  //set the baudrate
#ifndef WIN32
  brom[2] = ((mtos_cpu_freq_get() / (p_cfg->upg_baudrate) + 16) / 32 - 1);
#else
  brom[2] = 0x23;
#endif

 if(p_cfg->upg_chip_type == UPG_CHIP_WARRIORS)
 {
   if (p_priv->p_upg_impl->p_set_brom != NULL)
    {
      p_priv->p_upg_impl->p_set_brom(p_cfg->upg_baudrate, out_size);
    }  
 }
 else
 {
    //write bootrom cmd to set slave
    if (12 != p2p_send_data((u8 *)brom, 12))
    {
      OS_PRINTF("\n##p2p_send_data fail!line[%lu]\n", __LINE__);
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }
      return UPG_RET_ERROR;
    }

  //reset the Baudrate and send the upgclient size
  brom[3] = out_size;

  uart_set_param(0, p_cfg->upg_baudrate, 0, 0, 0);

    mtos_task_delay_ms(10);
    if (12 != p2p_send_data((u8 *)(&brom[3]), 12))
    {
      OS_PRINTF("\n##p2p_send_data fail!line[%lu]\n", __LINE__);
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }
      return UPG_RET_ERROR;
    }
  }
  len = 0;

  while(len < out_size)
  {
    //to check exit key.
    if (p_priv->p_upg_impl->p_set_key != NULL)
    {
      p_priv->p_upg_impl->p_set_key(&p_priv->upg_data.key, &p_priv->upg_data.cfg);
    }
    if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_priv))
    {
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }

      return UPG_RET_EXIT;
    }

    //to check menu key.
    if(UPGRD_KEY_MENU == UPG_GET_KEY(p_priv))
    {
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }

      return UPG_RET_MENU;
    }

    l = ((len + P2P_PACKET_SIZE) <= out_size) ? P2P_PACKET_SIZE : (out_size - len);
    memcpy(p_buff, p_cfg->p_unzip_buffer + len, l);
    if (l != p2p_send_data(p_buff, l))
    {
      OS_PRINTF("\n##p2p_send_data fail!line[%lu]\n", __LINE__);
      if(p_buff != NULL)
      {
        mtos_free(p_buff);
      }
      return UPG_RET_ERROR;
    }
    len += l;

    if ((++i % 10) == 0)
    {
      upg_update_status(p_priv, UPG_GET_STS(p_priv), 10 + len * 90 / out_size, NULL);
    }
  }
  upg_update_status(p_priv, UPG_GET_STS(p_priv), 100, NULL);

  if(NULL != p_buff)
  {
    p2p_free(p_buff);
  }

  return UPG_RET_SUCCESS;
}


/*!
  Send command to upgclient,
  cmd format: '#' + id + length + data + crc
 */
static upg_ret_t p2p_send_cmd(upg_priv_t *p_priv, u8 id,u8 *p_param,
                        u8 param_len, u32 timeout)
{
  u8 reply[4] = {0, };
  u8 buff[P2P_CMD_MAX_LEN] = {'#', 0, 0, };
  u32 len = 0, crc = 0, i = 0;

  buff[1] = id;
  buff[2] = param_len;

  if(0 != param_len)
  {
    CHECK_FAIL_RET((sizeof(buff) > (param_len + 4)), UPG_RET_ERROR);
    memcpy(&buff[3], p_param, param_len);
  }

  len = 3 + param_len;
  // do not include '#'
  crc = crc_fast_calculate(CRC_MODE, CRC_INIT, &buff[1], len - 1);
  memcpy(&buff[len], (u8 *)&crc, sizeof(u32));
  len += sizeof(u32);

  for(i = 0; i < P2P_CMD_RETYR_TIME; i++)
  {
    if (p_priv->p_upg_impl->p_set_key != NULL)
    {
      p_priv->p_upg_impl->p_set_key(&p_priv->upg_data.key, &p_priv->upg_data.cfg);
    }
    //to check exit key.
    if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_priv))
    {
      return UPG_RET_EXIT;
    }

    //to check menu key.
    if(UPGRD_KEY_MENU == UPG_GET_KEY(p_priv))
    {
      return UPG_RET_MENU;
    }

    p2p_clear_data();
    if(len != p2p_send_data(buff, len))
    {
      continue;
    }

    reply[0] = 0;
    reply[1] = 0;
    if(UPG_RET_SUCCESS != p2p_get_data(reply, 1, timeout))   //cmd not response
    {
      continue;
    }

    if(P2P_CMD_OK == reply[0])                   //cmd format is OK
    {
      if(UPG_RET_SUCCESS != p2p_get_data(&reply[1], 1, timeout))
      {
        continue;
      }

      if(P2P_CMD_OK == reply[1])                //cmd param is OK
      {
        break;
      }
      else if((P2P_CMD_ERROR == reply[1])   //slave return wrong, get remote error type
             && (UPG_RET_SUCCESS == p2p_get_data(&reply[1], 2, timeout)))
      {
        upg_update_status(p_priv, UPG_SM_REMOTE_ERROR,
                  (reply[1] << 8) | reply[2], "slave response: error happend!");
        return UPG_RET_PARAM;
      }
    }
  }

  if(i >= P2P_CMD_RETYR_TIME)
  {
    return UPG_RET_ERROR;
  }
  else
  {
    return UPG_RET_SUCCESS;
  }
}


/********************************************************
                    external APIs
 ********************************************************/
/*!
  Sync with the slave and send the upgclient to slave
 */
upg_ret_t p2p_cmd_sync(upg_priv_t *p_upg_priv)
{
  u8 data = 0;
  u32 tm = 100; //100 ms
  u16 code = 0xFFFF;
  u8 key_usr = 0;
  u8 sync_data = BOOTROM_SYNC_BYTE;
  u8 res_data = BOOTROM_RESPONSE_BYTE;
  uio_device_t *p_dev = NULL;
  upg_config_t *p_cfg = &p_upg_priv->upg_data.cfg;

  p_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  p_upg_priv->p_upg_impl->p_get_code(UPGRD_CODE_EXIT, p_cfg->exit_key);
  p_upg_priv->p_upg_impl->p_get_code(UPGRD_CODE_MENU, p_cfg->menu_key);


  p2p_clear_data(); //clear uart buff to

  uart_set_param(0, p_cfg->sync_baudrate, 0, 0, 0);

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0, "Master is ready for upgrade...");
    //clear key at first
  uio_clear_key(p_dev);

  if (p_cfg->sync_byte)
  {
    sync_data = p_cfg->sync_byte;
  }

  if (p_cfg->res_byte)
  {
    res_data = p_cfg->res_byte;
  }

  mtos_task_lock();
  while(1)
  {
    if((UPG_RET_SUCCESS == p2p_get_data(&data, 1, tm)) && (sync_data == data))
    {
      p2p_send_data(&res_data, 1);
      break;
    }

    if(SUCCESS == uio_get_code(p_dev, &code))
    {
      key_usr = (code >> (UIO_USR_CODE_SHIFT + 8)) & 0x7;
      if(p_cfg->exit_key[key_usr] == (code & 0xff))
        {
          mtos_task_unlock();
          return UPG_RET_EXIT;
        }
      if(p_cfg->menu_key[key_usr] == (code & 0xff))
        {
          mtos_task_unlock();
          return UPG_RET_MENU;
      }
    }
  }
  mtos_task_unlock();
  crc_setup_fast_lut(CRC32_ARITHMETIC);

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 5, "Start to init slave...");
  if(p_cfg->upg_chip_type == UPG_CHIP_WARRIORS)
  {
    if (p_upg_priv->p_upg_impl->p_init_chip != NULL)
    {
      if (UPG_RET_SUCCESS != p_upg_priv->p_upg_impl->p_init_chip(p_cfg))
      {
        upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 5, "init slave failed!");
        return UPG_RET_ERROR ;
      }
    } 
   upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 5, "init slave success!");
  }
  return p2p_send_client(p_upg_priv);
}


/*!
  send cmd to test if the upgclient is alive
 */
upg_ret_t p2p_cmd_test(upg_priv_t *p_upg_priv)
{
  u8 *p_test = (u8 *)"STB TEST P2P UPG";
  u32 l = strlen((char *)p_test), retry = 3;
  u8 cmd[20] = {0, }, i = 0;

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0,
                    "Start to check serial...");

  for(i = 0; i < retry; i++)
  {
    if(UPG_RET_SUCCESS != p2p_send_cmd(p_upg_priv, P2P_CMD_TEST, p_test, l, 1000))
    {
      continue;
    }

    if(UPG_RET_SUCCESS != p2p_get_data(cmd, l, 2000))
    {
      continue;
    }
    if(0 == memcmp(cmd, p_test, l))
    {
      break;
    }
  }

  if(i >= retry)
  {
    upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR, ERR_TEST_FAIL,
                      "Check serial fail!");
    return UPG_RET_ERROR;
  }
  else
  {
    upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0,
                      "Check serial port OK!");
    return UPG_RET_SUCCESS;
  }
}

//u8 fp_irq = 0;
/*!
  collect the data manager information from slave
 */
upg_ret_t p2p_cmd_collect(upg_priv_t *p_upg_priv, u8 check_slave, u8 *p_info)
{
  u32 crc = 0, size = 0;
  u8 check = 0, retry = 10, i = 0;
  upg_ret_t ret = UPG_RET_SUCCESS;

 // u8 error_temp_irq = 0;
  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0,
                    "Start to collect slave info...");

  check = check_slave ? 'C' : 'N';  //if slave need to check crc

  mtos_task_lock();
  for(i = 0; i < retry; i++)
 {
     // stop fp irq
    #ifndef WIN32
      uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                              DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
      uio_tmp_ctl(p_dev, FP_STOP_WORK);
    #endif
    p2p_clear_data();
    ret = p2p_send_cmd(p_upg_priv, P2P_CMD_COLLECT, &check, 1, 3000);
    //if(fp_irq == 1)
    //  error_temp_irq++;
   // fp_irq = 0;

    if(UPG_RET_PARAM == ret)
    {
      mtos_task_unlock();
      return UPG_RET_ERROR;
    }
    else if(UPG_RET_ERROR == ret)
    {
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "COLLECT: send cmd failed!");
    }
    else if(UPG_RET_SUCCESS == ret)  //slave response OK
    {
      if(UPG_RET_SUCCESS != p2p_get_data((u8 *)&size, 4, 2000))    // get info size
      {
        continue;
      }

      if(UPG_RET_SUCCESS != p2p_get_data((u8 *)p_info, size, 1000))  //get info
      {
      //start FP irq
      #ifndef WIN32
        uio_tmp_ctl(p_dev, FP_START_WORK);
      #endif

        continue;
      }

      //start FP irq
      #ifndef WIN32
        uio_tmp_ctl(p_dev, FP_START_WORK);
      #endif

      if(UPG_RET_SUCCESS != p2p_get_data((u8 *)&crc, 4, 1000))  //get crc
      {
        continue;
      }

      if(crc == crc_fast_calculate(CRC_MODE, CRC_INIT, (u8 *)p_info, size))
      {
        break;
      }
      else
      {
        upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_CRC_ERROR, "COLLECT: receive info crc error!");
      }
    }
  }
  mtos_task_unlock();
  /*switch(error_temp_irq)
  {
      case 0:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 0");
		  break;
      case 1:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 1");
		  break;
      case 2:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 2");
		  break;
      case 3:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 3");
		  break;
      case 4:
     upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 4");
		  break;
      case 5:
     upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 5");
		  break;
      case 6:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 6");
		  break;
      case 7:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 7");
		  break;
      case 8:
      upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 8");
		  break;
       case 9:
       upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 9");
		  break;
	ddefault :
		  upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_COLLECT_FAIL, "irq: send cmd failed 10");
		  break;

  }*/
  if(i >= retry)
  {
    upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR, ERR_COLLECT_FAIL,
                      "COLLECT: send command failed!");
    return UPG_RET_ERROR;
  }
  else
  {
    upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 50,
                      "Collect info OK!");
    return UPG_RET_SUCCESS;
  }
}


/*!
  Inform the upgclient the upgrade info, data format:
  for upg all: block num(must be 1)+ 0xff + size
  for upgrade blocks: ID+len+block num +{id, size}+{id, size}....
 */
upg_ret_t p2p_cmd_inform(upg_priv_t *p_upg_priv,
  u8 block_number, upg_block_t *p_blocks)
{
  u8 len = 0, i = 0;
  u8 cmd[5 * UPG_MAX_BLOCK_NUM + 10] = {0, };
  dmh_block_info_t head = {0, };
  upg_ret_t ret = UPG_RET_SUCCESS;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  p_upg_priv->upg_data.out_size = 0;
  p_upg_priv->upg_data.total_size = 0;

  //upgrade all flash
  if((P2P_UPGRADE_ALL == p_blocks->master_block_id))
  {
    p_upg_priv->upg_data.upg_all_flag = TRUE;
    p_upg_priv->upg_data.total_size = p_blocks->size;
    cmd[0] = block_number;
    cmd[1] = P2P_UPGRADE_ALL;
    memcpy(&cmd[2], (u8 *)&p_blocks->size, 4);
    len = 6;
  }
  else                                   //upgrade specific blocks
  {
    p_upg_priv->upg_data.upg_all_flag = FALSE;
    CHECK_FAIL_RET((block_number <= UPG_MAX_BLOCK_NUM), UPG_RET_ERROR);
    cmd[0] = block_number;
    for(i = 0; i < block_number; i++)
    {
      if(UPG_BOOTLOADER_BLOCK_ID == p_blocks[i].master_block_id)
      {
        //if upgrade bootloader, finde the bootloader automatically
        if(SUCCESS != p2p_read_flash_data(BL_SEC_OFFSET, (u8 *)&head.size, 4))
        {
          upg_update_status(p_upg_priv,UPG_SM_MASETER_ERROR,
                    ERR_INFORM_FLASH_READ_ERR, "INFORM: read flash data err!");
          return UPG_RET_ERROR;
        }
        else if(head.size % FLASH_SEC_SIZE)  //check if it is 64K alignde
        {
          upg_update_status(p_upg_priv,UPG_SM_MASETER_ERROR,
                  ERR_INFORM_BL_SIZE_ERR,"INFORM: bootloader not 64K aligned!");
          return UPG_RET_ERROR;
        }
      }
      else if(DM_SUC != dm_get_block_header(p_dm_handle,
                                            p_blocks[i].master_block_id, &head))
      { //find block by data manager
        upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                             ERR_INFORM_ID_NOT_EXIST, "INFORM: get dmh error!");
        return UPG_RET_ERROR;
      }
      p_upg_priv->upg_data.total_size += head.size;
      cmd[i * 5 + 1] = p_blocks[i].slave_block_id;
      memcpy(&cmd[i * 5 + 2], (u8 *)&(head.size), 4);
    }
    len = 5 * block_number + 1;
  }

  for(i = 0; i < 3; i++)
  {
    ret = p2p_send_cmd(p_upg_priv, P2P_CMD_INFORM, cmd, len, 5000);
    if(UPG_RET_PARAM == ret)
    {
      return UPG_RET_ERROR;
    }
    else if(UPG_RET_SUCCESS == ret)
    {
      break;
    }
  }

  if(i >= 3)
  {
    upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR, ERR_INFORM_FAIL,
                      "INFORM: failed!");
    return UPG_RET_ERROR;
  }
  else
  {
    upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 100,
                      "Inform slave OK!");
    return UPG_RET_SUCCESS;
  }
}


/*!
  Transfer the data to upgclient,

  cmd format: block_id + zipflag + size
    zipflag = 'Z': zipped data(only available for upg all, block_id = 0xFF),
    zipfleg = other char, normal data

  Packet format: '#' + '*' + packet sequence + 1K data + crc
 */
upg_ret_t p2p_cmd_transfer(upg_priv_t *p_upg_priv, u8 zipped, u8 block_id)
{
  u32 start_addr = 0, size = 0, len = 0, l = 0, crc = CRC_INIT, seq = 0, i = 0;
  u8 *p_buff = NULL;
  u8 cmd[10] = {0, }, err[4] = {0, }, data = 0;
  dmh_block_info_t info;
  upg_data_t *p_data = &p_upg_priv->upg_data;
  u32 curn_percent = 0, total_percent = 0;
  upg_ret_t ret = UPG_RET_SUCCESS;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u32 transfer_time = 0;
  //p_data->out_size = 0;
  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv),
                        p_data->out_size * 100 / p_data->total_size, "Prepare to download...");

  if(TRUE == p_data->upg_all_flag)
  {
        cmd[0] = P2P_UPGRADE_ALL;
        start_addr = p_data->divides_ready * p_data->total_size;
        size = p_data->total_size;
  }
  else
  {
        CHECK_FAIL_RET((0 == zipped), UPG_RET_ERROR);// only support zipped all, not zipped block
        if(UPG_BOOTLOADER_BLOCK_ID == block_id)
        {
            if(SUCCESS != p2p_read_flash_data(BL_SEC_OFFSET, (u8 *)&info.size, 4))
            {
                upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_TRANS_FLASH_READ_ERR, "TRANSFER:read bootloader error!");
                return UPG_RET_ERROR;
            }
            info.base_addr = 0;
        }
        else if(DM_SUC != dm_get_block_header(p_dm_handle, block_id, &info))
        {
            CHECK_FAIL_RET(0, UPG_RET_ERROR);
        }
        cmd[0] = block_id;

        //In the new flash mapping ,info.base_addr is relative address
        //start_addr = info.base_addr;

        start_addr = dm_get_block_addr(p_dm_handle, block_id) - get_flash_addr();
        size = info.size;
  }

  cmd[1] = zipped ? 'Z' : 'N';
  p_buff = (u8 *)p2p_malloc(P2P_PACKET_SIZE);
  CHECK_FAIL_RET((NULL != p_buff), UPG_RET_ERROR);
  memset(p_buff, 0, P2P_PACKET_SIZE);
  memcpy(&cmd[2], (u8 *)&size, 4);

  for(i = 0; i < 3; i++)
  {
        ret = p2p_send_cmd(p_upg_priv, P2P_CMD_TRANSFER, cmd, 6, 3000);
        if(UPG_RET_PARAM == ret)
        {
            p2p_free(p_buff);
            return UPG_RET_ERROR;
        }
        else if(UPG_RET_SUCCESS == ret)
        {
            break;
        }
  }
  if(i >= 3)
  {
        upg_update_status(p_upg_priv,UPG_SM_MASETER_ERROR, ERR_TRANS_SEND_CMD_ERR,
            "TRANSFER: send command fail!");
        p2p_free(p_buff);
        return UPG_RET_ERROR;
  }

  p2p_clear_data();
  cmd[0] = P2P_START_INDICATOR1;           //data packet start indicator1: '#'
  cmd[1] = P2P_START_INDICATOR2;           //data packet start indicator1: '*'
  mtos_task_delay_ms(10);
  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv),
    p_data->out_size * 100 / p_data->total_size, "Start downloading data...");

  while(len < size)
  {
        l = ((len + P2P_PACKET_SIZE) <= size) ? P2P_PACKET_SIZE : size - len;
        if(SUCCESS != p2p_read_flash_data(start_addr + len, p_buff, l))
        {
            upg_update_status(p_upg_priv,UPG_SM_MASETER_ERROR,
                ERR_TRANS_FLASH_READ_ERR,"TRANSFER: read flash error!");
            p2p_free(p_buff);
            return UPG_RET_ERROR;
        }

        crc = crc_fast_calculate(CRC_MODE, CRC_INIT, p_buff, l);
        while(1)
        {
            if (p_upg_priv->p_upg_impl->p_set_key != NULL)
            {
                p_upg_priv->p_upg_impl->p_set_key(&p_upg_priv->upg_data.key, 
                    &p_upg_priv->upg_data.cfg);
            }
            if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_EXIT;
            }

            if(UPGRD_KEY_MENU == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_MENU;
            }

            cmd[2] = (seq >> 8) & 0xFF;  //packet index, to avoid duplicat rcv in // slave
            cmd[3] = seq & 0xFF;

            if(4 != p2p_send_data(cmd, 4))       //send indicator+index
            {
                continue;
            }
            if(l != p2p_send_data(p_buff, l))      //send data
            {
                continue;
            }

            if(4 != p2p_send_data((u8 *)&crc, 4))  //send crc
            {
                continue;
            }
            if (p_upg_priv->p_upg_impl->p_set_key != NULL)
            {
                p_upg_priv->p_upg_impl->p_set_key(&p_upg_priv->upg_data.key, 
                    &p_upg_priv->upg_data.cfg);
            }

            if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_EXIT;
            }

            if(UPGRD_KEY_MENU == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_MENU;
            }

            if(UPG_RET_SUCCESS != p2p_get_data(err, 3, 3000))    //get packet response
            {
                transfer_time++;
                if(transfer_time >= 10)
                    return UPG_RET_ERROR;
                    
                upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR,
                    ERR_TRANS_UART_TIMEOUT, "Connection time out, retry...");
                p2p_clear_data();
                continue;
            }

            if(P2P_CMD_OK == err[0])
            {
                if(((err[1] << 8) | err[2]) >= seq)
                {
                    break;
                }
                else //in case the random error
                {
                    upg_update_status(p_upg_priv,UPG_SM_MASETER_ERROR, ERR_TRANS_DISORDER,
                        "packet sequence error, retry...");
                }
                p2p_clear_data();
            }
            else if(P2P_CMD_ERROR == err[0])
            {
                 transfer_time++;
                 if(transfer_time >= 10)
                    return UPG_RET_ERROR;
                 
                upg_update_status(p_upg_priv,UPG_SM_REMOTE_ERROR,(err[1] << 8) | err[2],
                    "Slave error, retransfer...!");
                p2p_clear_data();
                continue;
            }

            if (p_upg_priv->p_upg_impl->p_set_key != NULL)
            {
                p_upg_priv->p_upg_impl->p_set_key(&p_upg_priv->upg_data.key, 
                    &p_upg_priv->upg_data.cfg);
            }
            if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_EXIT;
            }

            if(UPGRD_KEY_MENU == UPG_GET_KEY(p_upg_priv))
            {
                p2p_free(p_buff);
                return UPG_RET_MENU;
            }

        }

        len += l;
        p_data->out_size += l;
        seq++;
        seq %= 0xFFFF;

        curn_percent = p_data->out_size * 100 / p_data->total_size;
        if (curn_percent != total_percent)
        {
            total_percent = curn_percent;
            upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv),total_percent, NULL);
        }

  }

  if(NULL != p_buff)
  {
        p2p_free(p_buff);
  }

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv),
                    p_data->out_size * 100 / p_data->total_size,
                    "End downloading!");

  if(UPG_RET_SUCCESS == p2p_get_data(&data, 1, 3000))  //wait CRC or unzip
  {
        if(P2P_CMD_OK == data)
        {
            upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv),
                        p_data->out_size * 100 / p_data->total_size,
                        "Slave respose OK!");
            return UPG_RET_SUCCESS;
        }
  }

  upg_update_status(p_upg_priv, UPG_SM_REMOTE_ERROR, data,
                    "Slave respose Failed!");

  return UPG_RET_ERROR;

}


/*!
  Get the slave burn progress
 */
upg_ret_t p2p_cmd_burn(upg_priv_t *p_upg_priv)
{
  u8 cmd[10] = {0, };
  u32 i = 0;

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0,
                    "Verify burning flash...");

  for(i = 0; i < 3; i++)
  {
    if(UPG_RET_SUCCESS == p2p_send_cmd(p_upg_priv, P2P_CMD_BURN, NULL, 0, 1000))
    {
      break;
    }
  }
  if(i >= 3)
  {
    upg_update_status(p_upg_priv, UPG_SM_MASETER_ERROR, ERR_TRANS_SEND_CMD_ERR,
                      "burn: send command fail!");
    return UPG_RET_ERROR;
  }

  upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 0,
                    "Start burning flash, DONOT POWOFF!");
  while(1)
  {
    if (p_upg_priv->p_upg_impl->p_set_key != NULL)
    {
      p_upg_priv->p_upg_impl->p_set_key(&p_upg_priv->upg_data.key, &p_upg_priv->upg_data.cfg);
    }
    if(UPGRD_KEY_EXIT == UPG_GET_KEY(p_upg_priv))
    {
      return UPG_RET_EXIT;
    }

    if(UPGRD_KEY_MENU == UPG_GET_KEY(p_upg_priv))
    {
      return UPG_RET_MENU;
    }

    if(UPG_RET_SUCCESS != p2p_get_data(cmd, 1, 20000))
    {
      upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 100,
                        "Receive burn info timeout!");
      return UPG_RET_ERROR;
    }

    if('B' == cmd[0])
    {
      if(UPG_RET_SUCCESS != p2p_get_data(cmd, 1, 2000))
      {
        continue;
      }
      else
      {
        upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), cmd[0], NULL);
        continue;
      }
    }
    if('F' == cmd[0])
    {
      upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 100,
                        "Burning flash OK!");
      break;
    }
    if('E' == cmd[0])
    {
      upg_update_status(p_upg_priv, UPG_GET_STS(p_upg_priv), 100,
                        "burning flash error!");
      return UPG_RET_ERROR;
    }
  }

  return UPG_RET_SUCCESS;
}


/*!
  Send command to reboot slave
 */
upg_ret_t p2p_cmd_reboot(upg_priv_t *p_upg_priv)
{
  p2p_send_cmd(p_upg_priv, P2P_CMD_REBOOT, NULL, 0, 1000);

  return UPG_RET_SUCCESS;
}

