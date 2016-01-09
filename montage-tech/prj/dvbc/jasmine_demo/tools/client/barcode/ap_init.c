/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_mem.h"
#include "mtos_printk.h" //hu
#include "mtos_task.h" //hu
#include "mtos_misc.h"
#include "sys_cfg.h" //hu
#include "drv_dev.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "charsto.h"
#include "data_manager.h"
#include "fcrc.h"
#include "unzip.h"
#include "mtos_misc.h"
#include "hal_base.h"
#include "spi.h"
#include "driver.h"

#define GET_FLASH_DATA_BY_DRIVER      //need flash driver to get data manager info

//same macro with offsetof in "stddef.h"
#define fieldoffset(st, m) ((u32)((char *)&((st *)(0))->m - (char *)0))

#define CRC_MODE      CRC32_ARITHMETIC
#define CRC_INIT      0xFFFFFFFF
#define NO_CRC        0x4352434E // "NCRC"

#ifdef WIN32
#define BOOTROM_SYNC_BYTE         0xA5
#define BOOTROM_RESPONSE_BYTE     0x60
#endif

#define P2P_MAX_BLOCKS            20
#define P2P_PACKET_SIZE           1024
#define P2P_START_INDICATOR       0x23 //'#'
#define P2P_START_INDICATOR2      0x2A //'*'
#define P2P_CMD_RESPONSE          0x4F // 'O'
#define P2P_CMD_MAX_LEN           120  //at most 100B command, enlarge it if needed
#define P2P_CMD_UPG_ALL           0xFF

//flash info
#define FLASH_SECTION_SIZE                (64 * 1024)
//#define FLASHSTART_ADDR                   NORF_BASE_ADDR
#define FLASHSTART_ADDR                   0
#define DMH_START_OFFSET                  (8) //offset to flash base address

//block structure
#define DMH_SDRAM_SIZE_OFFSET             (0) //offset to dmh start address
#define DMH_FLASH_SIZE_OFFSET             (DMH_SDRAM_SIZE_OFFSET + 4)
#define DMH_BLOCKNUM_OFFSET               (DMH_FLASH_SIZE_OFFSET + 4)
#define DMH_BHSIZE_OFFSET                 (DMH_BLOCKNUM_OFFSET + 2)
#define BLOCK_ID_OFFSET                   (0) //offset to block header info start addr
#define BLOCK_BASEADDR_OFFSET             (BLOCK_ID_OFFSET + 4)
#define BLOCK_SIZE_OFFSET                 (BLOCK_BASEADDR_OFFSET + 4)
#define BLOCK_CRC_OFFSET                  (BLOCK_SIZE_OFFSET + 4)

#define DMH_MAN_BLOCKS_SIZE_OFFSET        (12)

//sdram config
#define SDRAM_UNZIP_BUFF_SIZE             (600 * 1024)
#define SDRAM_2M_THRESHOLD                (64 * MBYTES)



u8 flash_mapping_type = 1;
u32 dmh_start_offset_0 = 0;
u32 dmh_start_offset = 0;
u8 dmh_tag_2[12] = {0, };
u32 dmh_man_blocks_size_2[4] = {0, };
u8 test_tmp = -1;

//SDRAM MAPPING

/*********************************************/
//    zipped data
/**********************data_bk_start**********/
//    flash map
/**********************flash_map_start********/
//  unzip inflate buffer
/*********************unzip_buff_start********/
//    upg client tool
/*********addr=0******************************/

enum p2p_cmd_id
{
  P2P_CMD_TEST = 0x50,
  P2P_CMD_SN_BURN = 0x70,
  P2P_CMD_SN_DIRECT,
  P2P_CMD_CHECKBURNED = 0x80,
  P2P_CMD_END,
};
enum cmd_err_type
{
  CMD_OK = 0,
  CMD_ERROR,
  CMD_FIND,
  CMD_ERASE,
  CMD_READ,
  CMD_WRITE,
  CMD_BURN_PROGRESS,
  CMD_FINISHED,
  CMD_UNKNOWN,
  CMD_ACK,
};

enum cmd_err_code
{
  CMD_TEST_OK = 0,
  CMD_CMD_ID_ERR = 1,
  CMD_CMD_LENGTH_ERR = 2,
  CMD_CMD_TIMEOUT = 3,
  CMD_CMD_CRC_ERR = 4,
  CMD_COLLECT_DMH_ERR = 5,
  CMD_COLLECT_CRC_ERR = 6,
  CMD_COLLECT_FALSH_READ_ERR = 7,
  CMD_INFORM_NO_INFO = 8,
  CMD_INFORM_READ_FLASH_ERR = 9,
  CMD_TRANS_UPGALL_ID_ERR = 0x0a,
  CMD_TRANS_UPGALL_LEN_ERR = 0x0b,
  CMD_TRANS_ZIPPED_FOR_BLOCK = 0x0b,    //do not support for zipped block
  CMD_TRANS_BLOCK_LEN_ERR = 0x0c,
  CMD_TRANS_ID_NOT_EXIST = 0x0d,
  CMD_TRANS_DATA_TIMEOUT = 0x0e,
  CMD_TRANS_CRC_ERR = 0x0f,
  CMD_TRANS_TOTAL_CRC_ERR = 0x10,
  CMD_TRANS_UNZIP_ERR = 0x11,
  CMD_BURN_ERASE_ERR = 0x12,
  CMD_BURN_FLASH_WRITE_ERR = 0x13,
  CMD_BURN_SIZE_NOT_64K_ALIGN = 0x14,
  CMD_REBOOT_ERROR = 0x15,
  CMD_GETPROPERTY_LEN_ERROR = 0x16,
  CMD_GETPROPERTY_FLASH_READ_ERROR = 0x17,
};


struct blk_info
{
  u8 block_id;
  u32 size;
  u32 dmh_start_offset;
};

static u8 cmd_buff[P2P_CMD_MAX_LEN + 1] = {0, };
static struct charsto_device *p_charsto = NULL;

struct blk_info blk_info_tab[P2P_MAX_BLOCKS];

extern void spi_flash_init();

#ifdef WIN32

static u8 ramsim[8 * 1024 * 1024] = {0, };
#include "uart_win32.h"
static void send_data(u8 *buff, u32 len)
{
  uartwin_write(buff, len);
}


static s32 read_data(u8 *buff, u32 len, u32 timeout)
{
  u32 i;

  MT_ASSERT(NULL != buff);
  for(i = 0; i < len; i++)
  {
    if(0 != uartwin_read_byte(&buff[i], timeout))
    {
      return ERR_FAILURE;
    }
  }
  return SUCCESS;
}


static void flush_data()
{
  uartwin_flush();
}


#else
static void send_data(u8 *buff, u32 len)
{
  u32 i;
  u32 sr = 0;

  MT_ASSERT(NULL != buff);

  mtos_critical_enter(&sr);
  for(i = 0; i < len; i++)
  {
    uart_write_byte(0, buff[i]);
  }
  mtos_critical_exit(sr);
}


static s32 read_data(u8 *buff, u32 len, u32 timeout)
{
  u32 i;
  u32 sr = 0;

  MT_ASSERT(NULL != buff);

  mtos_critical_enter(&sr);
  for(i = 0; i < len; i++)
  {
    if(SUCCESS != uart_read_byte_polling(0, &buff[i], timeout))
    {
      mtos_critical_exit(sr);
      return ERR_FAILURE;
    }
  }
  mtos_critical_exit(sr);

  return SUCCESS;
}


static void flush_data()
{
  uart_flush(0);
}


#endif

//type: see enum cmd_err_type
static void cmd_response(u8 type, u8 id, u8 errcode)
{
  u8 len = 0;
  u8 tmp[4] = {0, };

  switch(type)
  {
    case CMD_OK:
      tmp[0] = 'O'; //OK
      len = 1;
      break;
    case CMD_ERROR:
      tmp[0] = 'E'; //Error
      tmp[1] = id;
      tmp[2] = errcode;
      len = 3;
      break;
    case CMD_BURN_PROGRESS:
      tmp[0] = 'B';
      tmp[1] = errcode;         //here is the burn progress
      len = 2;
      break;
    case CMD_FINISHED:
      tmp[0] = 'F';            //burn Finished
      len = 1;
      break;
    case CMD_ACK:
      tmp[0] = 'O';
      tmp[1] = id;
      tmp[2] = errcode;
      len = 3;
      break;
    default:
      tmp[0] = 'U';           //unknown error
      len = 1;
      break;
  }

  send_data(tmp, len);
}


static u32 get_dword(u8 *buff)
{
  return buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24);
}




static s32 cmd_handle_test()
{
  u32 len;

  cmd_response(CMD_OK, 0, 0);
  len = cmd_buff[1];
  send_data(&cmd_buff[2], len);
  return SUCCESS;
}



static void cmd_handle_unknow()
{
  cmd_response(CMD_UNKNOWN, 0, 0);
}


#if 0
static void dump_cmd()
{
  u8 i = 0;

  P2PRINT("\nCMD ID[0x%0x] len[%d]\n data:", cmd_buff[0], cmd_buff[1]);
  for(i = 0; i < cmd_buff[1]; i++)
  {
    P2PRINT("%02x ", cmd_buff[i + 2]);
  }
  P2PRINT("\n");
}


#endif
//cmd format: #(1) + ID(1) + LEN(1) + data(LEN) + crc(4)
void get_cmd(void)
{
  u8 tmp = 0, cmd_id = 0, cmd_len = 0;
  u8 t[4] = {0, };
  u32 i = 0, crc = 0, rcrc;

  while(1)
  {
    for(i = 0; i < sizeof(cmd_buff); i++)
    {
      cmd_buff[i] = 0;
    }
    flush_data();
    //wait for new command
    //mtos_printk("-->1");
    //mtos_task_sleep(10);

    while(1)
    {
      tmp = 0;
      read_data(&tmp, 1, 0xFFFFFFFF);
      if(P2P_START_INDICATOR == tmp)
      {
        break;
      }
    }

    i = 0;
    if((SUCCESS != read_data(&cmd_id, 1, 1000))
      || ((cmd_id < P2P_CMD_TEST)
         || (cmd_id >= P2P_CMD_END)))                     //get cmd id
    {
      continue;
    }
    cmd_buff[i++] = cmd_id;

    if((SUCCESS != read_data(&cmd_len, 1, 1000))
      || (cmd_len >= P2P_CMD_MAX_LEN))                    //get cmd length
    {
      continue;
    }
    cmd_buff[i++] = cmd_len;

    if(SUCCESS != read_data(&cmd_buff[i], cmd_len, 1000))  //get data
    {
      continue;
    }

    if(SUCCESS != read_data(t, 4, 1000))  //get crc
    {
      continue;
    }

    rcrc = get_dword(t);

    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, cmd_buff, cmd_len + 2);
    if(crc != rcrc)
    {
      continue;
    }
    else
    {
      cmd_response(CMD_OK, 0, 0);
    }

    break;
  }
}



u32 find_dm_head_start(u32 start_address, u8 *buff)
{

  u32 i = 0;
  u32 j = 0;
  u32 dm_head_start = 0;
  u8 data[64] = {0,};

  u32 init_start = start_address;

  for (i = 0; i < 128; i++)
  {
    if( buff == NULL )
    {
      if( charsto_read(p_charsto, init_start, (u8 *)data, 64) )
      {
        MT_ASSERT(0);
        return 1;
      }
    }
    else
    {
      memcpy(data, (u8 *)(buff + init_start), 64);
    }

    for(j = 0; (j + 8) < 64; j++)
    {
      if( (data[j] == 0x2a ) &&
          (data[j+1] == 0x5e) &&
          (data[j+2] == 0x5f) &&
          (data[j+3] == 0x5e) &&
          (data[j+4] == 0x2a) &&
          (data[j+5] == 0x44) &&
          (data[j+6] == 0x4d) &&
          (data[j+7] == 0x28) )
      {
        dm_head_start = init_start + j;
        return (dm_head_start - start_address);
      }

//u32 start_data1 = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
//u32 start_data2 = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];

//if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
//{

//}
    }
    init_start += 0x10000;
  }

//mtos_printk("error: do not find dm head, start_address=0x%x! \n", start_address);
  return 1;

}

static int __get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 dm_head_start, u32 start_address, u8 *buff)
{
  u32 blk_num = 0;
  u32 first_block_addr = 0;
  s32 result = 0;
  int i = 0, j = 0;
  u8 data[100] = {0,};
  int err = 0;

  first_block_addr = ((dm_head_start) + 16 + 8 + 4 + start_address);

  if(NULL == buff)
     err = charsto_read(p_charsto, ((dm_head_start) + 16 + 8 + start_address), (u8 *)data, 8);
  else
    memcpy(data, (buff + ((dm_head_start) + 16 + 8 + start_address)), 8);

  if (err)
  {
    MT_ASSERT(0);
  }

  blk_num = (data[1] << 8) + data[0];

  for (i = 0; i < blk_num; i++)
  {
    if(NULL == buff)
      err = charsto_read(p_charsto, first_block_addr, (u8 *)data, sizeof(dmh_block_info_t));
    else
      memcpy(data, (buff + first_block_addr), sizeof(dmh_block_info_t));
    if (err)
    {
      MT_ASSERT(0);
    }
    //lttle to big, trouble!!!
      j = 0;
      dest->id = data[j++];
      dest->type = data[j++];
      dest->node_num = (data[j + 1] << 8) + data[j];
      j = j + 2;
      dest->base_addr = (data[j + 3] << 24) + (data[j + 2] << 16) + (data[j + 1] << 8) + data[j];
      j = j + 4;
      dest->size = (data[j + 3] << 24) + (data[j + 2] << 16) + (data[j + 1] << 8) + data[j];
      j = j + 4;
      dest->crc = (data[j + 3] << 24) + (data[j + 2] << 16) + (data[j + 1] << 8) + data[j];
      j = j + 4;

       j = j + 8;//dest version,  u8 version[8]
       j = j + 8;//dest->name   u8 name[8];  Name of this block
       j = j + 12;//time[12];
       j = j + 4; //reserve
   // PBL("===dest->id=0x%x======i=%d==,dm_head_start=0x%x=dest->base_addr=0x%x====\n",dest->id, i,dm_head_start,dest->base_addr);
    if(dest->id == block_id)
    {
      result = 1;



      if (dm_head_start == 0xc)
        dest->base_addr += 0xc + start_address;
      else
        dest->base_addr += dm_head_start + start_address;
      break;
    }

    first_block_addr += 48;
  }
  return result;

}

int get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 start_address, u8 *buff)
{
  s32 result = 0;

  unsigned int dm_head_start_main = find_dm_head_start(start_address, buff);

  result = __get_dm_block_head_info(block_id, dest, 0x0, start_address, buff);

  if (result)
    return result;

  result = __get_dm_block_head_info(block_id, dest, dm_head_start_main, start_address, buff);

  if (result)
    return result;

  return result;
}

static void cmd_burn_SN()
{
  u32 start_addr = 0, total_size = 0, i = 0, offset = 0, crc = 0;
  u8 *p_buff = (u8 *)mtos_malloc(FLASH_SECTION_SIZE + 10); //burn one sector
  u8 ret = 0, len = 0;
  u32 sn_offset = 0;
  dmh_block_info_t sn_block_info;
  u32 ota_blcok_addr = 0x0;
  int SN_BLOCK_ID = 0x8F;
  int SN_BLOCK_LEN = 1024;
 // u8 temp[16] = {'#','#', '#', '#', 0 };

  len = cmd_buff[2]; //SN size
  sn_offset = cmd_buff[3]+ (cmd_buff[4] << 8);
  //find SN ID, if do not find, error!
   ota_blcok_addr = find_dm_head_start(0, NULL);
  if(ota_blcok_addr == 1)
    ota_blcok_addr = 0x130000;

  ret = get_dm_block_head_info(SN_BLOCK_ID, &sn_block_info, ota_blcok_addr, NULL);
  if (!ret)
  {
   cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_FIND);
   return;
  }

  start_addr =(sn_block_info.base_addr + sn_offset) / FLASH_SECTION_SIZE * FLASH_SECTION_SIZE;
  total_size = sn_block_info.size;

  //mtos_printk("----222--start_addr=0x%x--total_size=0x%x--sn_offset=0x%x\n", start_addr, total_size, sn_offset);

  if(total_size > SN_BLOCK_LEN)
  {
	 cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_ERROR);
	 return;
  }
  ret = charsto_read(p_charsto, (start_addr), p_buff, FLASH_SECTION_SIZE);
  if (ret)
  {

	 cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_READ);
     return;
  }

  offset = sn_block_info.base_addr + sn_offset - start_addr;
#if 0
len = 10 + 16;
  for(i = 0; i < len; i++)
  {
	  if(i %0x10 == 0)
		  mtos_printk("\n");
	  mtos_printk("%x ", *(p_buff + offset - 8 + i));
  }
  mtos_printk("----222--OK---\n");
  return;
#endif

  for(i = 0; i < 1024; i++) //Assume that the barcode length is smaller than 1024
  {
    if(i < len )
	  *(p_buff + offset + i) = cmd_buff[5 + i];
    else
      *(p_buff + offset + i) = 0xFF;
  }

  if (SUCCESS != charsto_erase(p_charsto, start_addr, 1))
  {
	cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_ERASE);
	return;
  }
  if (SUCCESS != charsto_writeonly(p_charsto, start_addr, p_buff, FLASH_SECTION_SIZE))
  {
	cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_WRITE);
	return;
  }
  cmd_response(CMD_OK, 0, 0);

  i = 0;
  p_buff[i++] = P2P_START_INDICATOR;
  p_buff[i++] = P2P_START_INDICATOR2;
  p_buff[i++] = P2P_CMD_SN_BURN;

  p_buff[i++] = len;
  p_buff[i++] = sn_offset & 0xFF;
  p_buff[i++] = (sn_offset >> 8)& 0xFF;

  //read and send data back for check
  if (SUCCESS != charsto_read(p_charsto, sn_block_info.base_addr + sn_offset, &p_buff[i], len))
  {
  	cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_WRITE);
  	return;
   }
  send_data(p_buff, len + i);
  crc = crc_fast_calculate(CRC_MODE, CRC_INIT, &p_buff[i], len);
  crc = get_dword((u8*)&crc);
  send_data((u8 *)&crc, 4);            //send crc

}


static void cmd_burn_SN_direct()
{
  u32 start_addr = 0, i = 0, crc = 0;
  u8 *p_buff = (u8 *)mtos_malloc(FLASH_SECTION_SIZE); //burn one sector
  u8 len = 0;
  u32 sn_offset = 0;

  len = cmd_buff[2]; //SN size
  sn_offset = cmd_buff[3]+ (cmd_buff[4] << 8);

  start_addr = 0xa0000;


  for(i = 0; i < 1024; i++) //Assume that the barcode length is smaller than 1024
  {
    if(i < len ) {
      p_buff[i]= cmd_buff[5 + i];
    } else {
      p_buff[i] = 0xFF;
    }
  }

  if (SUCCESS != charsto_erase(p_charsto, start_addr, 1))
  {
    cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_ERASE);
    return;
  }
  if (SUCCESS != charsto_writeonly(p_charsto, start_addr, p_buff, FLASH_SECTION_SIZE))
  {
    cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_WRITE);
    return;
  }
  cmd_response(CMD_OK, 0, 0);

  i = 0;
  p_buff[i++] = P2P_START_INDICATOR;
  p_buff[i++] = P2P_START_INDICATOR2;
  p_buff[i++] = P2P_CMD_SN_DIRECT;

  p_buff[i++] = len;
  p_buff[i++] = sn_offset & 0xFF;
  p_buff[i++] = (sn_offset >> 8)& 0xFF;

  //read and send data back for check
  if (SUCCESS != charsto_read(p_charsto, 0xa0000, &p_buff[i], len))
  {
    cmd_response(CMD_ERROR, P2P_CMD_SN_BURN, CMD_WRITE);
    return;
  }
  send_data(p_buff, len + i);
  crc = crc_fast_calculate(CRC_MODE, CRC_INIT, &p_buff[i], len);
  crc = get_dword((u8*)&crc);
  send_data((u8 *)&crc, 4);            //send crc

}

static void cmd_check_burned()
{
  u32 start_addr = 0, i = 0, offset = 0;
  u8 *p_buff = (u8 *)mtos_malloc(FLASH_SECTION_SIZE + 10); //burn one sector
  u8 ret = 0, len = 0;
  u32 sn_offset = 0;
  dmh_block_info_t sn_block_info;
  u32 ota_blcok_addr = 0x0;
  int SN_BLOCK_ID = 0x8F;
  u8 is_burned = 0;

  len = cmd_buff[2]; //SN size
  sn_offset = cmd_buff[3]+ (cmd_buff[4] << 8);
  //find SN ID, if do not find, error!
   ota_blcok_addr = find_dm_head_start(0, NULL);
  if(ota_blcok_addr == 1)
    ota_blcok_addr = 0x130000;

  ret = get_dm_block_head_info(SN_BLOCK_ID, &sn_block_info, ota_blcok_addr, NULL);
  if (!ret)
  {
   cmd_response(CMD_ERROR, P2P_CMD_CHECKBURNED, CMD_FIND);
   mtos_free(p_buff);
   return;
  }

  start_addr =(sn_block_info.base_addr + sn_offset) / FLASH_SECTION_SIZE * FLASH_SECTION_SIZE;

  if(len == 0)
  {
	 cmd_response(CMD_ERROR, P2P_CMD_CHECKBURNED, CMD_ERROR);
   mtos_free(p_buff);
	 return;
  }

  offset = sn_block_info.base_addr + sn_offset - start_addr;
  ret = charsto_read(p_charsto, (start_addr), p_buff, FLASH_SECTION_SIZE);
  if (ret)
  {
	 cmd_response(CMD_ERROR, P2P_CMD_CHECKBURNED, CMD_READ);
   mtos_free(p_buff);
     return;
  }
  for(i = 0; i < len; i++)
  {
    if (p_buff[offset+i] != 0xFF)
    {
      is_burned = 1;
      break;
    }
  }
   
  cmd_response(CMD_ACK, P2P_CMD_CHECKBURNED, is_burned);

  mtos_free(p_buff);
}


void dispatch_cmd()
{
  switch(cmd_buff[0])
  {
    case P2P_CMD_TEST:
      cmd_handle_test();
      break;
	case P2P_CMD_SN_BURN:
	  cmd_burn_SN();
    break;
    case P2P_CMD_SN_DIRECT:
      cmd_burn_SN_direct();
      break;
  case P2P_CMD_CHECKBURNED:
    cmd_check_burned();
    break;
    default:
      cmd_handle_unknow();
      break;
  }
}


#define CONCERTO_144_B
void spiflash_cfg( )
{
    RET_CODE ret;
    void *p_spi = NULL; 
    charsto_cfg_t charsto_cfg = {0};
    spi_cfg_t spiCfg;



   //spi pinmux
   *(volatile unsigned int*)(0xbf156008) = (0x11<<16)|(0x11);
   
   *(volatile unsigned int*)(0xbf15601c) = 0;
   
   *(volatile unsigned int*)(0xbf156004) = (0x5<<12)|(0x5<<6);
   
   *(volatile unsigned int*)(0xbf156300) = (0x3<<21)|(1<<24);
   
   *(volatile unsigned int*)(0xbf156400) = (0x3<<21)|(1<<24);




#if defined (CONCERTO_144_B) || defined (CONCERTO_144_A)
    spiCfg.bus_clk_mhz   = 50;//10;
    spiCfg.bus_clk_delay = 12;//8;
#else
    spiCfg.bus_clk_mhz   = 10;
    spiCfg.bus_clk_delay = 8;
#endif	
    spiCfg.io_num        = 1;
    spiCfg.lock_mode     = OS_MUTEX_LOCK;
    spiCfg.op_mode       = 0;
    spiCfg.pins_cfg[0].miso1_src  = 0;
    spiCfg.pins_cfg[0].miso0_src  = 1;
    spiCfg.pins_cfg[0].spiio1_src = 0;
    spiCfg.pins_cfg[0].spiio0_src = 0;
    spiCfg.pins_dir[0].spiio1_dir = 3;
    spiCfg.pins_dir[0].spiio0_dir = 3;
    spiCfg.spi_id = 0;
    
    ret = spi_concerto_attach("spi_concerto_0");
    MT_ASSERT(SUCCESS == ret);
    p_spi = dev_find_identifier(NULL,DEV_IDT_NAME, (u32)"spi_concerto_0");
    spiCfg.spi_id = 0;
    ret = dev_open(p_spi, &spiCfg);
    MT_ASSERT(SUCCESS == ret);

    spiflash_jazz_attach("charsto_concerto");
    //mtos_printk("drv --charsto 1\n");

    p_charsto = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_charsto);
    //mtos_printk("drv --charsto 2\n");
 
    charsto_cfg.rd_mode = SPI_FR_MODE;
    charsto_cfg.p_bus_handle = p_spi;
    charsto_cfg.size = 4*1024*1024;
    ret = dev_open(p_charsto, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
}

void drv_init()
{
  spiflash_cfg();
}


#define GAP_SIZE (100 * 1024)
int ap_init(void)
{
  drv_init();
  spi_flash_init();
  crc_setup_fast_lut(CRC_MODE);
  //set_uart_high_speed();
#ifdef WIN32
  uartwin_init(1);
#else
  uart_ioctl(UART_CMD_EN_INTERRUPT, FALSE, 0); //close interupt to use polling mode
#endif

//  mtos_close_printk();

#ifdef WIN32
  data = BOOTROM_SYNC_BYTE;
  send_data(&data, 1);
  while(1)
  {
    if((SUCCESS == read_data(&data, 1, 100))
      && (BOOTROM_RESPONSE_BYTE == data))
    {
      break;
    }
  }
#endif
  while(1)
  {
    get_cmd();
    dispatch_cmd();
  }

  return 0;
}
