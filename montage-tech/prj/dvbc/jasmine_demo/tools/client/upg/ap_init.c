/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_devs.h"
#include "sys_define.h"
//#include "sys_regs_warriors.h"
#include "mtos_mem.h"
#include "mtos_printk.h" //hu
#include "mtos_task.h" //hu
#include "mtos_misc.h"
#include "upg_sys_cfg.h" //hu
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

#define STB_MSG_BLOCK_ID    0x8F

enum upg_user_t
{
  UPG_NORMAL_USER,
  UPG_SUPER_USER = 0xEE,
  UPG_END_USER,
};

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
  P2P_CMD_COLLECT,
  P2P_CMD_INFORM,
  P2P_CMD_TRANSFER,
  P2P_CMD_BURN,
  P2P_CMD_REBOOT,
  P2P_CMD_READ,
  P2P_CMD_GETPROPERTY,
  P2P_CMD_DUMP = 0x60,
  P2P_CMD_LIMIT = 0x65,
  P2P_CMD_END,
};
enum cmd_err_type
{
  CMD_OK = 0,
  CMD_ERROR,
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

struct upgclient_info
{
  u32 unzip_buff_start;  //start addr of unzip used buffer
  u32 flash_map_start;   //start addr of flash mapping
  u32 data_bk_start;     //start addr of zipped data backup
  u32 start_burn_addr;   //start addr to write flash data
  u32 upg_all_size;      //for upgrade all only, the whole data size
  u8 upg_all_flag;       //if need to upgrade all flash
  u8 upg_use_type;     //0: normal,  0xEE: super user
};

struct blk_info
{
  u8 block_id;
  u32 size;
  u32 dmh_start_offset;
};
typedef struct _upg_slave_info
{
  u32 dmh_start_addr;         //data manager header start addr
  u32 bootloader_size;        //the size of bootloader section
  u32 sdram_size;             //below is the same with data manager.
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
  dmh_block_info_t blocks[P2P_MAX_BLOCKS];
}upg_slave_info;

static u8 cmd_buff[P2P_CMD_MAX_LEN + 1] = {0, };
static struct upgclient_info upg_cfg = {0, 0, 0, 0, 0, 0};  //upg mem map info
static upg_slave_info slave_info = {0, 0, 0, 0, 0, 0, }; //mainly for slave block info.
static struct charsto_device *p_charsto = NULL;

struct blk_info blk_info_tab[P2P_MAX_BLOCKS];


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


#ifdef GET_FLASH_DATA_BY_DRIVER
static u32 get_word(u8 *buff)
{
  return buff[0] | (buff[1] << 8);
}


#endif
static u32 get_dword(u8 *buff)
{
  return buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24);
}


static u32 find_id(struct blk_info *info, u32 num, u8 block_id)
{
  u32 i;

  for(i = 0; i < num; i++)
  {
    if(block_id == info[i].block_id)
    {
      return info[i].size;
    }
  }

  return 0;
}


static s32 flash_mapping_type_test(void)
{
  u32 i = 0, j = 0;
  u32 ret;
  u8 buff[1024] = {0, };
  u8 dmh_indicator[12] = "*^_^*DM(^o^)";
  u32 dmh_man_blocks_size = 0;
  u32 h_start_offset_0 = 0;

  if(SUCCESS != charsto_read(p_charsto, 0, buff, sizeof(buff)))
  {
    return -3;
  }
  for(i = 0; i < sizeof(buff); i++)
  {
    ret = memcmp(buff + i, dmh_indicator, 12);
    if(ret == 0)
    {
      flash_mapping_type = 2;
      dmh_man_blocks_size = get_dword(&buff[DMH_MAN_BLOCKS_SIZE_OFFSET + i]);
      break;
    }
  }

  h_start_offset_0 = 256 * 1024;

  if(i == sizeof(buff))
  {
    flash_mapping_type = 1;
    return 0;
  }

  if(flash_mapping_type == 2)
  {
    //The second DMH address is k allign case.
    for(i = 0; i < 20; i++)
    {
      if(SUCCESS !=
         charsto_read(p_charsto,
                      (i *
                       1024) + dmh_start_offset_0 + dmh_man_blocks_size + 0xC + h_start_offset_0,
                      buff, sizeof(buff)))
      {
        return -3;
      }
      ret = memcmp(buff, dmh_indicator, 12);
      if(ret == 0)
      {
        dmh_start_offset =
          (i * 1024) + dmh_start_offset_0 + dmh_man_blocks_size + 0xC + h_start_offset_0;
        return SUCCESS;
      }
    }
    //The second DMH address is not K allign case.
    for(i = 0; i < 20; i++)
    {
      if(SUCCESS !=
         charsto_read(p_charsto,
                      (i * 1024) + dmh_start_offset_0 + dmh_man_blocks_size + h_start_offset_0,
                      buff, sizeof(buff)))
      {
        return -3;
      }
      for(j = 0; j < sizeof(buff); j++)
      {
        ret = memcmp(buff + j, dmh_indicator, 12);
        if(ret == 0)
        {
          dmh_start_offset =
            (i * 1024) + j + dmh_start_offset_0 + dmh_man_blocks_size + h_start_offset_0;
          return SUCCESS;
        }
      }
    }
  }

  return ERR_FAILURE;
}


static s32 check_crc(void)
{
#ifndef GET_FLASH_DATA_BY_DRIVER
  u32 dmh_start_addr = 0, block_base_addr = 0;
  u32 flash_size = 0, block_num = 0, bh_size = 0;
  u32 i, crc = 0, bcrc = 0, bid = 0, baddr = 0, bsize = 0;

  dmh_start_addr = *(u32 *)(FLASHSTART_ADDR + DMH_START_OFFSET) + FLASHSTART_ADDR;
  flash_size = *(u32 *)(dmh_start_addr + DMH_FLASH_SIZE_OFFSET); //flash size
  block_num = *(u16 *)(dmh_start_addr + DMH_BLOCKNUM_OFFSET); //total block count
  bh_size = *(u16 *)(dmh_start_addr + DMH_BHSIZE_OFFSET); //each block header size

  if((!flash_size)
    || (!block_num)
    || (!bh_size)
    || (block_num * bh_size > flash_size))
  {
    return -1; //dmh info error
  }
  block_base_addr = dmh_start_addr + 12; //block header start addr
  for(i = 0; i < block_num; i++)
  {
    bcrc = *(u32 *)(block_base_addr + i * bh_size + BLOCK_CRC_OFFSET);
    if(bcrc == NO_CRC)  // NCRC
    {
      continue;
    }

    bid = *(u8 *)(block_base_addr + i * bh_size + BLOCK_ID_OFFSET);
    baddr = *(u32 *)(block_base_addr + i * bh_size + BLOCK_BASEADDR_OFFSET);
    bsize = *(u32 *)(block_base_addr + i * bh_size + BLOCK_SIZE_OFFSET);

    if((!baddr)
      || (!bsize)
      || (baddr > flash_size)
      || (bsize > flash_size))
    {
      return -1;  //dmh info error
    }

    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, (u8 *)(baddr + FLASHSTART_ADDR), bsize);
    if(crc != bcrc)
    {
      return -2; //crc error
    }
  }
  return 0;
#else
  u32 dmh_start_addr = 0, block_base_addr = 0;
  u32 flash_size = 0, sdram_size = 0, block_num = 0, bh_size = 0;
  u32 i = 0, crc = 0, bcrc = 0, bid = 0, baddr = 0, bsize = 0;
  u8 buff[1024] = {0, };
  u8 *bk = NULL;

  /*get dmh_start_add*/
  if(flash_mapping_type == 2)
  {
    dmh_start_addr = dmh_start_offset;
    if(SUCCESS != charsto_read(p_charsto, dmh_start_addr + 16, buff, 12))
    {
      return -3;
    }
  }
  else
  {
    if(SUCCESS != charsto_read(p_charsto, DMH_START_OFFSET, buff, 4))
    {
      return -3;
    }
    dmh_start_addr = get_dword(buff);
    if(SUCCESS != charsto_read(p_charsto, dmh_start_addr, buff, 12))
    {
      return -3;
    }
    if(0 != dmh_start_addr % FLASH_SECTION_SIZE)
    {
      return -1;
    }
  }

  flash_size = get_dword(&buff[DMH_FLASH_SIZE_OFFSET]);
  sdram_size = get_dword(&buff[DMH_SDRAM_SIZE_OFFSET]);
  block_num = get_word(&buff[DMH_BLOCKNUM_OFFSET]);
  bh_size = get_word(&buff[DMH_BHSIZE_OFFSET]);

  if((!sdram_size)
    || (!flash_size)
    || (!block_num)
    || (!bh_size)
    || (block_num * bh_size > flash_size))
  {
    return -1; //dmh info error
  }
  if(flash_mapping_type == 2)
  {
    block_base_addr = dmh_start_addr + 12 + 16;
  }
  else
  {
    block_base_addr = dmh_start_addr + 12;
  }
#ifndef WIN32
  // if (sdram_size > SDRAM_2M_THRESHOLD) // if sdram > 2M,
  //   upg_cfg.flash_map_start = SDRAM_2M_THRESHOLD;
#endif
  upg_cfg.data_bk_start = upg_cfg.flash_map_start + flash_size + 100 * 1024;
  bk = (u8 *)upg_cfg.data_bk_start;

  if(SUCCESS != charsto_read(p_charsto, block_base_addr, buff, block_num * bh_size))
  {
    return -3;
  }

  for(i = 0; i < block_num; i++)
  {
    bcrc = get_dword(buff + i * bh_size + BLOCK_CRC_OFFSET);
    if(bcrc == NO_CRC)  // NCRC
    {
      continue;
    }

    bid = get_dword(buff + i * bh_size + BLOCK_ID_OFFSET);

    baddr = get_dword(buff + i * bh_size + BLOCK_BASEADDR_OFFSET);
    if(flash_mapping_type == 2)
    {
      baddr += dmh_start_addr;
    }
    bsize = get_dword(buff + i * bh_size + BLOCK_SIZE_OFFSET);

    if((!baddr)
      || (!bsize)
      || (baddr > flash_size)
      || (bsize > flash_size))
    {
      return -1;  //dmh info error
    }
    if(SUCCESS != charsto_read(p_charsto, baddr, bk, bsize))
    {
      return -3;
    }
    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, bk, bsize);
    if(crc != bcrc)
    {
      test_tmp = i;
      return -2;
    }
  }
  return 0;
#endif
}


static s32 get_info(upg_slave_info *info)
{
#ifndef GET_FLASH_DATA_BY_DRIVER
  u32 dmh_start = 0, dmh_start_addr = 0, block_base_addr = 0;
  u32 flash_size = 0, sdram_size = 0, block_num = 0, bh_size = 0;

  dmh_start = *(u32 *)(FLASHSTART_ADDR + DMH_START_OFFSET); //dmh start offset in flash

  dmh_start_addr = dmh_start + FLASHSTART_ADDR; //absolute DMH start address
  sdram_size = *(u32 *)(dmh_start_addr + DMH_SDRAM_SIZE_OFFSET);
  flash_size = *(u32 *)(dmh_start_addr + DMH_FLASH_SIZE_OFFSET);
  block_num = *(u16 *)(dmh_start_addr + DMH_BLOCKNUM_OFFSET); //total block count
  bh_size = *(u16 *)(dmh_start_addr + DMH_BHSIZE_OFFSET); //each block header size

  block_base_addr = dmh_start_addr + 12; //absolute addr of block header start

  if((!flash_size)
    || (!block_num)
    || (!bh_size)
    || (flash_size % FLASH_SECTION_SIZE)
    || (block_num * bh_size > flash_size)
    || (block_num > P2P_MAX_BLOCKS))
  {
    return -1; //dmh info error
  }

  info->sdram_size = sdram_size;
  info->flash_size = flash_size;
  info->bootloader_size = dmh_start;
  info->dmh_start_addr = dmh_start;
  info->block_num = block_num;
  info->bh_size = bh_size;
  memcpy((u8 *)(info->blocks), (u8 *)block_base_addr, block_num * bh_size);
  //if (sdram_size > SDRAM_2M_THRESHOLD)
  //  upg_cfg.flash_map_start = SDRAM_2M_THRESHOLD;

  upg_cfg.data_bk_start = upg_cfg.flash_map_start + flash_size + 100 * 1024;
  return 0;
#else
  u32 dmh_start_addr = 0, block_base_addr = 0;
  u32 flash_size = 0, sdram_size = 0, block_num = 0, bh_size = 0;
  u8 buff[1024] = {0, };

  if(flash_mapping_type == 2)
  {
    if(SUCCESS != charsto_read(p_charsto, 8, buff, 4))
    {
      return -3;
    }
    info->bootloader_size = get_dword(&buff[0]);

    memset(buff, 0x00, sizeof(buff));

    dmh_start_addr = dmh_start_offset;
    if(0 != dmh_start_addr % FLASH_SECTION_SIZE)
    {
      return -1;
    }

    if(SUCCESS != charsto_read(p_charsto, dmh_start_addr, buff, 16))
    {
      return -3;
    }

    memcpy(dmh_tag_2, buff, 12);
    memcpy(dmh_man_blocks_size_2, buff + 12, 4);

    memset(buff, 0x00, sizeof(buff));
    if(SUCCESS != charsto_read(p_charsto, dmh_start_addr + 16, buff, 12))
    {
      return -3;
    }
    block_base_addr = dmh_start_addr + 12 + 16; //offset in flash
  }
  else
  {
    if(SUCCESS != charsto_read(p_charsto, DMH_START_OFFSET, buff, 4))
    {
      return -3;
    }
    dmh_start_addr = get_dword(buff);

    if(0 != dmh_start_addr % FLASH_SECTION_SIZE)
    {
      return -1;
    }
    if(SUCCESS != charsto_read(p_charsto, dmh_start_addr, buff, 12))
    {
      return -3;
    }
    block_base_addr = dmh_start_addr + 12; //offset in flash
    info->bootloader_size = dmh_start_addr;
  }

  sdram_size = get_dword(&buff[DMH_SDRAM_SIZE_OFFSET]);
  flash_size = get_dword(&buff[DMH_FLASH_SIZE_OFFSET]);
  block_num = get_word(&buff[DMH_BLOCKNUM_OFFSET]);
  bh_size = get_word(&buff[DMH_BHSIZE_OFFSET]);

  if((!flash_size)
    || (!block_num)
    || (!bh_size)
    || (flash_size % FLASH_SECTION_SIZE)
    || (block_num * bh_size > flash_size)
    || (block_base_addr > flash_size)
    || (block_num > P2P_MAX_BLOCKS))
  {
    return -1; //dmh info error
  }
  info->dmh_start_addr = dmh_start_addr;
  info->sdram_size = sdram_size;
  info->flash_size = flash_size;
  info->block_num = block_num;
  info->bh_size = bh_size;

  if(SUCCESS != charsto_read(p_charsto, block_base_addr,
                             (u8 *)(info->blocks), block_num * bh_size))
  {
    return -3;
  }
#ifndef WIN32
  if(sdram_size > SDRAM_2M_THRESHOLD)
  {
    upg_cfg.flash_map_start = SDRAM_2M_THRESHOLD;
  }
#endif
  upg_cfg.data_bk_start = upg_cfg.flash_map_start + flash_size + 100 * 1024;

  return 0;
#endif
}


static s32 cmd_handle_test()
{
  u32 len;

  cmd_response(CMD_OK, 0, 0);
  len = cmd_buff[1];
  send_data(&cmd_buff[2], len);


  return SUCCESS;
}


//format: ID+len+flag('c'=check crc, else ignore crc)
static void cmd_handle_collect()
{
  u32 len, crc = 0;
  s32 ret = -1;

  ret = flash_mapping_type_test();
  if(-1 == ret)
  {
    cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_DMH_ERR);
    return;
  }
  else if(-3 == ret)
  {
    cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_FALSH_READ_ERR);
    return;
  }

  if('C' == cmd_buff[2])
  {
    ret = check_crc();

    if(-1 == ret)
    {
      cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_DMH_ERR);
      return;
    }
    else if(-2 == ret)
    {
      cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_CRC_ERR);
      return;
    }
    else if(-3 == ret)
    {
      cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_FALSH_READ_ERR);
      return;
    }
  }

  ret = get_info(&slave_info);

  if(-1 == ret)
  {
    cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_DMH_ERR);
    return;
  }
  else if(-3 == ret)
  {
    cmd_response(CMD_ERROR, P2P_CMD_COLLECT, CMD_COLLECT_FALSH_READ_ERR);
    return;
  }

  cmd_response(CMD_OK, 0, 0);

  len = slave_info.bh_size * slave_info.block_num + fieldoffset(upg_slave_info, blocks);
  crc = crc_fast_calculate(CRC_MODE, CRC_INIT, (u8 *)&slave_info, len);

  send_data((u8 *)&len, 4);            //send length
  send_data((u8 *)&slave_info, len);   //send info
  send_data((u8 *)&crc, 4);            //send crc
}


//format:for blocks: ID+len+block num +{id, size}+{id, size}....
//       for upg all:ID+len+block num(=1) +P2P_CMD_UPG_ALL+size
static void cmd_handle_inform()
{
  u32 i, size, num = 0, tmp = 0;
  u32 dmh_start = 0, blk_start = 0, tmp_addr = 0;
  u8 *p = NULL;
  u8 *map_base = NULL;

  struct blk_info newblk[P2P_MAX_BLOCKS];

  if((1 == cmd_buff[2])
    && (P2P_CMD_UPG_ALL == cmd_buff[3]))                      //upgrade all
  {
    upg_cfg.upg_all_flag = TRUE;
    upg_cfg.upg_all_size = get_dword(&cmd_buff[4]);
    cmd_response(CMD_OK, 0, 0);
#ifndef WIN32
    //if (upg_cfg.upg_all_size > (512*KBYTES)) //for >512K size, sdram must >2M
    //  upg_cfg.flash_map_start = SDRAM_2M_THRESHOLD;
#endif
    return;
  }

  upg_cfg.upg_all_flag = FALSE;

  // Must call collect info first
  if(0 == slave_info.flash_size)
  {
    cmd_response(CMD_ERROR, P2P_CMD_INFORM, CMD_INFORM_NO_INFO);
    return;
  }
  num = cmd_buff[2];     //block number to upgrade
  p = &cmd_buff[3];      //start: blk id + new blk size

  for(i = 0; i < num; i++)
  {
    newblk[i].block_id = p[i * 5];
    newblk[i].size = get_dword(&p[i * 5 + 1]);
  }


  map_base = (u8 *)upg_cfg.flash_map_start;

  //check if bootloader is needed to upg
  size = find_id(newblk, num, BOOTLOADER_BLOCK_ID);

  if(0 != size)  //bootloader need upg
  {
    upg_cfg.start_burn_addr = 0;
    dmh_start = size + 256; //reserve space in ram
    slave_info.dmh_start_addr = size + 256;
  }
  else  //just record where start to burn flash, save some time if bootloader is not need to upg
  {
    upg_cfg.start_burn_addr = slave_info.bootloader_size + 256 * 1024;

    if(flash_mapping_type == 2)
    {
      dmh_start = dmh_start_offset;
    }
    else
    {
      dmh_start = slave_info.bootloader_size;
    }
  }

  if(flash_mapping_type == 2)
  {
    blk_start = dmh_start + (28 + slave_info.block_num * slave_info.bh_size);
    // 1k alignment
    blk_start = ((blk_start + (1024) - 1) / 1024) * 1024;
  }
  else
  {
    //record the real block data start address
    blk_start = dmh_start + (12 + slave_info.block_num * slave_info.bh_size);
  }

  for(i = 0; i < slave_info.block_num; i++)
  {
    size = find_id(newblk, num, slave_info.blocks[i].id);

    if(0 != size)     //block to upgrade, update its size, update
    {
      slave_info.blocks[i].size = size;
    }
    else  //block not to upgrade, record its flash base addr, not update
    {
      if(flash_mapping_type == 2)
      {
        tmp_addr = slave_info.blocks[i].base_addr + dmh_start_offset;
      }
      else
      {
        tmp_addr = slave_info.blocks[i].base_addr;
      }
    }
    if((BLOCK_TYPE_IW == slave_info.blocks[i].type)
      || (BLOCK_TYPE_PIECE == slave_info.blocks[i].type))
    {
      //align IW to 64K position
      tmp = (blk_start + (FLASH_SECTION_SIZE - 1)) / FLASH_SECTION_SIZE * FLASH_SECTION_SIZE;
      if(tmp > blk_start)
      {
        memset(map_base + blk_start, 0xFF, tmp - blk_start);
      }
      blk_start = tmp;
    }
    #if 0
    if((i != 0)
      && (BLOCK_TYPE_IW != slave_info.blocks[i].type))
    {
      slave_info.blocks[i].base_addr =
        slave_info.blocks[i - 1].base_addr + slave_info.blocks[i - 1].size;
    }
    #endif
    slave_info.blocks[i].base_addr = blk_start;
    blk_start += slave_info.blocks[i].size;

    if(0 == size)  //block do not upgrade, copy from flash to construct new flash map
    {
      if(SUCCESS != charsto_read(p_charsto, tmp_addr,
                                 map_base + blk_start - slave_info.blocks[i].size,
                                 slave_info.blocks[i].size))
      {
        cmd_response(CMD_ERROR, P2P_CMD_INFORM, CMD_INFORM_READ_FLASH_ERR);
        return;
      }
    }

    if(flash_mapping_type == 2)
     slave_info.blocks[i].base_addr -= dmh_start;
  }

  //MAP Construct: fill the dmh to ram at last
  p = (u8 *)&slave_info;
  p += fieldoffset(upg_slave_info, sdram_size);
  if(flash_mapping_type == 2)
  {
    memcpy(map_base + dmh_start, dmh_tag_2, 12);
    memcpy(map_base + dmh_start + 12, dmh_man_blocks_size_2, 4);
    memcpy(map_base + dmh_start + 16, p, (12 + slave_info.block_num * slave_info.bh_size));
  }
  else
  {
    memcpy(map_base + dmh_start, p, (12 + slave_info.block_num * slave_info.bh_size));
  }
  cmd_response(CMD_OK, 0, 0);

  return;
}


//format:id + len + block_id + zipflag + size + crc (for all data)
//    zipflag = 'Z': zipped data(only available for upg all, block_id = 0xFF),
//    zipfleg = other char, normal data
static void cmd_handle_transfer()
{
  u32 i = 0, id, index = 0, crc = CRC_INIT, zip = 0, tcrc = 0, seq = 0, seqtmp = 0;
  u32 total = 0, got = 0, len = 0;
  u8 *buff = (u8 *)(upg_cfg.flash_map_start);
  u8 *p = NULL;
  u8 start[10] = {0, };

  if(TRUE == upg_cfg.upg_all_flag)   //update all flash data
  {
    if(P2P_CMD_UPG_ALL != cmd_buff[2])
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_UPGALL_ID_ERR);
      return;
    }

    total = get_dword(&cmd_buff[4]);
    if(total > upg_cfg.upg_all_size)
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_UPGALL_LEN_ERR);
      return;
    }

    zip = ('Z' == cmd_buff[3]) ? 1 : 0; //check if zipped data
    if(zip)                        //if zipped data, store to bk place first.
    {
      upg_cfg.data_bk_start = upg_cfg.flash_map_start + upg_cfg.upg_all_size + 100 * 1024;
      buff = (u8 *)upg_cfg.data_bk_start;
    }
  }
  else   //update blocks
  {
    id = cmd_buff[2];
    if('Z' == cmd_buff[3])        //not support zipped block data
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_ZIPPED_FOR_BLOCK);
      return;
    }
    total = get_dword(&cmd_buff[4]);
    //OS_PRINTK("total =%x\n",total);
    //check if size right with prev stored info
    if(BOOTLOADER_BLOCK_ID == id)
    {
      index = 0xFF;  //do not check total crc below for bootloader
    }
    else
    {
      for(i = 0; i < slave_info.block_num; i++)
      {
        if(slave_info.blocks[i].id == id)
        {
          if(slave_info.blocks[i].size != total) //check if the size is the same as informed bofore
          {
            cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_BLOCK_LEN_ERR);
            return;
          }
          else //record the block index for total crc check later
          {
            if(flash_mapping_type == 2)
            {
              //find the block start addr
              buff += (slave_info.blocks[i].base_addr + slave_info.dmh_start_addr);
            }
            else
            {
              buff += slave_info.blocks[i].base_addr; //find the block start addr
            }
            index = i;
            break;
          }
        }
      }
    }
    if(i >= slave_info.block_num)
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_ID_NOT_EXIST);
      return;
    }
  }

  cmd_response(CMD_OK, 0, 0);        // respond command
  p = buff;
  got = 0;                          //actual got bytes
  flush_data();

  seq = 0;         //record data packet sequence
  while(got < total)
  {
    len = (total - got > P2P_PACKET_SIZE) ? P2P_PACKET_SIZE : total - got;

    //get packet start indicator
    start[0] = 0;      //start indecator 1
    start[1] = 0;      //start indecator 2
    start[2] = 0;      //sequence high byte
    start[3] = 0;      //sequence low byte

    if((SUCCESS != read_data(start, 4, 1000))
      || (P2P_START_INDICATOR != start[0])
      || (P2P_START_INDICATOR2 != start[1]))
    {
      continue;
    }

    //seq in slave should always >= seqs in master,
    //becasue master never trans a new packet before acked!
    seqtmp = (start[2] << 8 | start[3]);
    if(seqtmp != seq)
    {
      if(seqtmp == (seq - 1)) //the ack is lost
      {
        read_data(p, len + 4, 1000);
        cmd_response(CMD_ACK, (seq >> 8) & 0xFF, seq & 0xff); //ack the sequence
        continue;
      }
      else                  //not the start indicator, but the data
      {
        read_data(p, len + 4, 1000);
        continue;
      }
    }


    if(SUCCESS != read_data(p, len, 1000))   //get data
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_DATA_TIMEOUT);
      flush_data();
      continue;
    }

    if(SUCCESS != read_data((u8 *)&tcrc, 4, 1000))  //get crc
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_DATA_TIMEOUT);
      flush_data();
      continue;
    }
    //check crc
    if(tcrc != crc_fast_calculate(CRC_MODE, CRC_INIT, p, len))
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_CRC_ERR);
      flush_data();
      continue;
    }
    got += len;
    p += len;
    cmd_response(CMD_ACK, (seq >> 8) & 0xFF, seq & 0xff); //ack the sequence
    seq++;
    seq %= 0xFFFF;
  }

  if((TRUE != upg_cfg.upg_all_flag)              // do not check upg all
    && (NO_CRC != slave_info.blocks[index].crc)  // do not check NCRC
    && (0xFF != index))                          // do not check bootloader
  {
    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, buff, total);
    slave_info.blocks[index].crc = crc;

    if(flash_mapping_type == 2)
    {
      p = (u8 *)(upg_cfg.flash_map_start + slave_info.dmh_start_addr + 12 + 16
                 + index * sizeof(dmh_block_info_t) + BLOCK_CRC_OFFSET),
      memcpy(p, &slave_info.blocks[index].crc, 4);  //update CRC in dmh
    }
    else
    {
      p = (u8 *)(upg_cfg.flash_map_start + slave_info.dmh_start_addr + 12
                 + index * sizeof(dmh_block_info_t) + BLOCK_CRC_OFFSET),
      memcpy(p, &slave_info.blocks[index].crc, 4);  //update CRC in dmh
    }
  }
#ifndef WIN32
  //unzip data for upgrade all case
  if(zip)
  {
    init_fake_mem((u8 *)(upg_cfg.unzip_buff_start));
    if(0 != gunzip(buff, (u8 *)upg_cfg.flash_map_start, total, &len, memcpy))
    {
      cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_UNZIP_ERR);
      return;
    }
  }
#else //just for windows simulator debug
  if(TRUE == upg_cfg.upg_all_flag)
  {
    memcpy((u8 *)upg_cfg.flash_map_start, buff, total);
  }
#endif
  cmd_response(CMD_OK, 0, 0); //response all
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

  return 1;

}
/*
u32 find_dm_head_start(u32 start_address, u8 *buff)
{

  u32 i = 0;
  u32 dm_head_start = 0;
  u8 data[64] = {0,};

  u32 init_start = start_address;
  int err = 0;

  for (i = 0; i < 36; i++)
  {
   if(NULL == buff)
     err = charsto_read(p_charsto, init_start, (u8 *)data, 8);
   else
   {
     memcpy(data, (u8 *)(buff + init_start), 8);
   }

   if (err)
   {
     MT_ASSERT(0);
     return 1;
   }
   u32 start_data1 = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
   u32 start_data2 = (data[7] << 24) + (data[6] << 16) + (data[5] << 8) + data[4];

   if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
   {
      dm_head_start = init_start;
      return (dm_head_start - start_address);
   }
    init_start += 0x10000;
  }
 
  return 1;

}
*/

static int __get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 dm_head_start, u32 start_address, u8 *buff)
{
  u32 blk_num = 0;
  u32 first_block_addr = 0;
  s32 result = 0;
  int i = 0;
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
      err = charsto_read(p_charsto, first_block_addr, (u8 *)dest, sizeof(dmh_block_info_t));
    else
      memcpy(dest, (buff + first_block_addr), sizeof(dmh_block_info_t));
    if (err)
    {
      MT_ASSERT(0);
    }

    if(dest->id == block_id)
    {
      result = 1;
      if (dm_head_start == 0xc)
        dest->base_addr += 0xc + start_address;
      else
        dest->base_addr += dm_head_start + start_address;
      break;
    }

    first_block_addr += sizeof(dmh_block_info_t);
  }
  return result;

}


int get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 start_address, u8 *buff)
{
  s32 result = 0;

  unsigned int dm_head_start_main = 0;

  result = __get_dm_block_head_info(block_id, dest, 0x0, start_address, buff);

  if (result)
    return result;

  dm_head_start_main = find_dm_head_start(start_address, buff);

  result = __get_dm_block_head_info(block_id, dest, dm_head_start_main, start_address, buff);

  if (result)
    return result;

  return result;
}


static void cmd_handle_burn()
{
    u32 secnum = 0, size = 0, i, ret = 0, ret2 = 0;
    u8 *buff = (u8 *)upg_cfg.flash_map_start;
    dmh_block_info_t flash_block_info = {0};
    dmh_block_info_t mem_block_info = {0};
    u32 ota_blcok_addr = 0x40000; //app_block_addr = 0x80000;
    cmd_response(CMD_OK, 0, 0);

    ota_blcok_addr = find_dm_head_start(0, NULL);    

/*
    app_block_addr  = find_dm_head_start(ota_blcok_addr + 0x10000, NULL);

    if(app_block_addr == 1)
        app_block_addr = 0;
    else
        app_block_addr = app_block_addr + ota_blcok_addr + 0x10000;
*/

    if(TRUE == upg_cfg.upg_all_flag)
    {        
        size = upg_cfg.upg_all_size;
        secnum = size/FLASH_SECTION_SIZE;
        //update boot so need make sure do not chang SN        
        if(ota_blcok_addr != 0)
        {
            ret = get_dm_block_head_info(STB_MSG_BLOCK_ID, &flash_block_info, ota_blcok_addr, NULL);
            
            if (0 != ret)
            {
                ret2 = get_dm_block_head_info(STB_MSG_BLOCK_ID, &mem_block_info, ota_blcok_addr,  buff);
                

                if (0 == ret2 || (flash_block_info.size != mem_block_info.size))
                {
                    //if super use can use , otherwise error
                    if(UPG_SUPER_USER != upg_cfg.upg_use_type)
                    {
                        cmd_response(CMD_ERROR, P2P_CMD_BURN, CMD_TRANS_ID_NOT_EXIST);
                        return;
                    }
                }
                else
                {
                    if(UPG_SUPER_USER != upg_cfg.upg_use_type)
                    {
                        ret = charsto_read(p_charsto, flash_block_info.base_addr, (u8 *)(buff + mem_block_info.base_addr),
                        mem_block_info.size);
                        if(ret)
                        {
                            cmd_response(CMD_ERROR, P2P_CMD_BURN, CMD_INFORM_READ_FLASH_ERR);
                            return;
                        }
                    }

                }
            }
       }
    }
    else
    {
        size = slave_info.flash_size - upg_cfg.start_burn_addr;
        secnum = size / FLASH_SECTION_SIZE;
    }

  if(size % FLASH_SECTION_SIZE) //should be 64K aligned!!
  {
    cmd_response(CMD_ERROR, P2P_CMD_BURN, CMD_BURN_SIZE_NOT_64K_ALIGN);
    return;
  }

  for(i = 0; i < secnum; i++)
  {
    if(SUCCESS != charsto_erase(p_charsto,
                                upg_cfg.start_burn_addr + i * FLASH_SECTION_SIZE, 1))
    {
      cmd_response(CMD_ERROR, P2P_CMD_BURN, CMD_BURN_ERASE_ERR);
      return;
    }
    cmd_response(CMD_BURN_PROGRESS, 0, (i + 1) * 50 / secnum);
  }

  for(i = 0; i < secnum; i++)
  {
    if(SUCCESS != charsto_writeonly(p_charsto, upg_cfg.start_burn_addr + i * FLASH_SECTION_SIZE,
                                    buff + upg_cfg.start_burn_addr + i * FLASH_SECTION_SIZE,
                                    FLASH_SECTION_SIZE))
    {
      cmd_response(CMD_ERROR, P2P_CMD_BURN, CMD_BURN_FLASH_WRITE_ERR);
      return;
    }
    cmd_response(CMD_BURN_PROGRESS, 0, (i + 1) * 50 / secnum + 50);
  }

  cmd_response(CMD_FINISHED, 0, 0);
}


static void cmd_handle_reboot()
{
  cmd_response(CMD_OK, 0, 0);
  mtos_task_delay_ms(100);
#ifndef WIN32
//  hal_put_u32((u32 *)0xbff70004, 0x0);
  hal_put_u32((u32 *)0xbff70004, 0x0);
#else
  exit();
#endif
}


static void cmd_handle_read()
{
  u32 block_id = 0, total_size = 0, index = 0, l = 0, len = 0, crc = CRC_INIT, i = 0;
  u16 seq = 0;
  u8 *buff = (u8 *)(upg_cfg.flash_map_start);
  u8 *p_buff = NULL;
  u8 cmd[10] = {0, }, err[4] = {0, };

  // read all flash
  if(cmd_buff[2] == 0xff)
  {
    buff = (u8 *)(upg_cfg.flash_map_start);
    total_size = get_dword(&cmd_buff[3]);
    if(total_size > slave_info.flash_size)
    {
      cmd_response(CMD_ERROR, P2P_CMD_READ, CMD_TRANS_UPGALL_LEN_ERR);
      return;
    }
  }
  else
  {
    block_id = cmd_buff[2];
    total_size = get_dword(&cmd_buff[3]);
    //check if size right with prev stored info
    if(BOOTLOADER_BLOCK_ID == block_id)
    {
      index = 0xFF;  //do not check total crc below for bootloader
    }
    else
    {
      for(i = 0; i < slave_info.block_num; i++)
      {
        if(slave_info.blocks[i].id == block_id)
        {
          if(slave_info.blocks[i].size != total_size) //check if the size is the same as informed
          {
            // bofore
            cmd_response(CMD_ERROR, P2P_CMD_READ, CMD_TRANS_BLOCK_LEN_ERR);
            return;
          }
          else //record the block index for total crc check later
          {
            if(flash_mapping_type == 2)
            {
              //find the block start addr
              buff += (slave_info.blocks[i].base_addr + slave_info.dmh_start_addr);
            }
            else
            {
              buff += slave_info.blocks[i].base_addr; //find the block start addr
            }
            index = i;
            break;
          }
        }
      }
      if(i >= slave_info.block_num)
      {
        cmd_response(CMD_ERROR, P2P_CMD_READ, CMD_TRANS_ID_NOT_EXIST);
        return;
      }
    }
  }
  cmd_response(CMD_OK, 0, 0);


  flush_data();
  cmd[0] = P2P_START_INDICATOR;           //data packet start indicator1: '#'
  cmd[1] = P2P_START_INDICATOR2;           //data packet start indicator1: '*'
  mtos_task_delay_ms(20);

  p_buff = buff;

  while(len < total_size)
  {
    // according to wu xiaoming's requirement, send 6k bytes data each time
    l = ((len + (6 * P2P_PACKET_SIZE)) <= total_size) ? (6 * P2P_PACKET_SIZE) : (total_size - len);

    if(SUCCESS != charsto_read(p_charsto, (u32)(p_buff - upg_cfg.flash_map_start), p_buff, l))
    {
      cmd_response(CMD_ERROR, P2P_CMD_INFORM, CMD_INFORM_READ_FLASH_ERR);
      return;
    }

    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, p_buff, l);
    while(1)
    {
      cmd[2] = (seq >> 8) & 0xFF;  //packet index, to avoid duplicat rcv in
                                   // slave
      cmd[3] = seq & 0xFF;

      send_data(cmd, 4);       //send indicator+index
      send_data(p_buff, l);      //send data

      send_data((u8 *)&crc, 4);  //send crc

      if(SUCCESS != read_data(err, 3, 3000))    //get packet response
      {
        flush_data();
        continue;
      }

      if(CMD_ACK == err[0])
      {
        if(((err[1] << 8) | err[2]) >= seq)
        {
          break;
        }
        else //in case the random error
        {
        }
        flush_data();
      }
      else if(CMD_ERROR == err[0])
      {
        flush_data();
      }
    }
    len += l;
    p_buff += l;
    seq++;
    seq %= 0xFFFF;
  }
}


//data format: block index(1 byte) + offset in block(4 bytes) + len of property(1 byte)
static void cmd_handle_getproperty()
{
  u32 block_id = 0;
  u32 offset = 0;
  u32 datalen = 0;

  u8 *databuf = NULL;

  u8 blockinfo[4] = {0, };
  int block_num = 0;
  int block_size = 0;
  u32 flash_offset = 0;
  int i = 0;
  u32 crc = CRC_INIT;

  block_id = cmd_buff[2];
  offset = cmd_buff[3] << 24 |
           cmd_buff[4] << 16 |
           cmd_buff[5] << 8 |
           cmd_buff[6];
  datalen = cmd_buff[7];

  if(datalen == 0)
  {
    cmd_response(CMD_ERROR, P2P_CMD_GETPROPERTY, CMD_GETPROPERTY_LEN_ERROR);
    return;
  }

  if(SUCCESS != charsto_read(p_charsto, 36, blockinfo, 4))
  {
    cmd_response(CMD_ERROR, P2P_CMD_GETPROPERTY, CMD_GETPROPERTY_FLASH_READ_ERROR);
    return;
  }

  block_num = (blockinfo[1] << 8) | blockinfo[0];
  block_size = (blockinfo[3] << 8) | blockinfo[2];

  flash_offset = 36 + 4;

  for(i = 0; i < block_num; i++)
  {
    if(SUCCESS != charsto_read(p_charsto, flash_offset, blockinfo, 4))
    {
      cmd_response(CMD_ERROR, P2P_CMD_GETPROPERTY, CMD_GETPROPERTY_FLASH_READ_ERROR);
      return;
    }

    if(blockinfo[0] == block_id)
    {
      if(SUCCESS != charsto_read(p_charsto, (flash_offset + 4), blockinfo, 4))
      {
        cmd_response(CMD_ERROR, P2P_CMD_GETPROPERTY, CMD_GETPROPERTY_FLASH_READ_ERROR);
        return;
      }

      flash_offset = ((blockinfo[3] << 24) |
                      (blockinfo[2] << 16) |
                      (blockinfo[1] << 8) |
                      (blockinfo[0]));

      flash_offset += 12; //dmh address + dmh header address

      break;
    }
    flash_offset += block_size;
  }

  if(i >= block_num) // can't find block of misc_option
  {
    return;
  }

  flash_offset += offset;

  databuf = mtos_malloc(datalen);

  if(SUCCESS != charsto_read(p_charsto, flash_offset, databuf, datalen))
  {
    cmd_response(CMD_ERROR, P2P_CMD_TRANSFER, CMD_TRANS_UPGALL_ID_ERR);
    return;
  }

  crc = crc_fast_calculate(CRC_MODE, CRC_INIT, databuf, datalen);

  send_data(databuf, datalen);
  send_data((u8 *)&crc, 4);
}


static void cmd_handle_admin()
{
  u8 len = cmd_buff[1];

  cmd_response(CMD_OK, 0, 0);
  if(cmd_buff[2] == UPG_SUPER_USER)
  {
    upg_cfg.upg_use_type = UPG_SUPER_USER;
  }
  else
  {
    upg_cfg.upg_use_type = UPG_NORMAL_USER;
  }

  send_data(&cmd_buff[2], len);
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


#define DUMP_RETRY_TIMES    3
static void cmd_dump_data()
{
  u32 tem_addr = 0, total_size = 0, crc = CRC_INIT, len = 0, l = 0;
  u8 *p_buff = (u8 *)(upg_cfg.flash_map_start);
  u16 seq = 0;
  u8 cmd[10] = {0, }, err[4] = {0, };
  u8 retry_time = DUMP_RETRY_TIMES;
  u32 si = 0;

  total_size = cmd_buff[2] + (cmd_buff[3] << 8) + (cmd_buff[4] << 16) + (cmd_buff[5] << 24);

  mtos_critical_enter(&si);
  while(len < total_size)
  {
    // according to wu xiaoming's requirement, send 6k bytes data each time
    l = ((len + (3 * P2P_PACKET_SIZE)) <= total_size) ? (3 * P2P_PACKET_SIZE) : (total_size - len);

    if(SUCCESS != charsto_read(p_charsto, tem_addr, p_buff, l))
    {
      cmd_response(CMD_ERROR, P2P_CMD_DUMP, CMD_INFORM_READ_FLASH_ERR);
      break;
    }

    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, p_buff, l);
    crc = get_dword((u8 *)&crc);

    retry_time = DUMP_RETRY_TIMES;
    while(retry_time)
    {
      cmd[0] = P2P_START_INDICATOR;                 //data packet start indicator1: '#'
      cmd[1] = P2P_START_INDICATOR2;                 //data packet start indicator1: '*'

      cmd[2] = (seq >> 8) & 0xFF;        //packet index, to avoid duplicat rcv in
      // slave
      cmd[3] = seq & 0xFF;

      send_data(cmd, 4);             //send indicator+index
      if(SUCCESS != read_data(err, 3, 0xFFFFFFFF))          //get packet response
      {
        flush_data();
        retry_time--;
        continue;
      }
      else
      {
        if(err[0] == 0x01)
        {
          retry_time--;
          continue;
        }
      }

      send_data(p_buff, l);            //send data
      if(SUCCESS != read_data(err, 3, 0xFFFFFFFF))          //get packet response
      {
        flush_data();
        retry_time--;
        continue;
      }
      else
      {
        if(err[0] == 0x01)
        {
          retry_time--;
          continue;
        }
      }

      send_data((u8 *)&crc, 4);        //send crc

      if(SUCCESS != read_data(err, 3, 0xFFFFFFFF))          //get packet response
      {
        flush_data();
        retry_time--;
        continue;
      }
      else
      {
        if(err[0] == 0x01)              // CMD_RETRANSFER
        {
          retry_time--;
          continue;
        }
        else
        {
          if(0x00 == err[0])
          {
            if(((err[1] << 8) | err[2]) != seq)
            {
              retry_time--;
              continue;
            }
            else
            {
              break;
            }
          }
        }
      }
    }
    if(!retry_time)
    {
      cmd_response(CMD_ERROR, P2P_CMD_DUMP, 0);      
      break;
    }
    tem_addr += l;
    len += l;
    p_buff += l;
    seq++;
    seq %= 0xFFFF;
  }
  mtos_critical_exit(si);
}


void dispatch_cmd()
{
  switch(cmd_buff[0])
  {
    case P2P_CMD_TEST:
      cmd_handle_test();
      break;
    case P2P_CMD_COLLECT:
      cmd_handle_collect();
      break;
    case P2P_CMD_INFORM:
      cmd_handle_inform();
      break;
    case P2P_CMD_TRANSFER:
      cmd_handle_transfer();
      break;
    case P2P_CMD_BURN:
      cmd_handle_burn();
      break;
    case P2P_CMD_REBOOT:
      cmd_handle_reboot();
      break;
    case P2P_CMD_READ:
      cmd_handle_read();
      break;
    case P2P_CMD_DUMP:
      cmd_dump_data();
      break;
    case P2P_CMD_GETPROPERTY:
      cmd_handle_getproperty();
    case P2P_CMD_LIMIT:
     cmd_handle_admin();
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
    
    p_charsto = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_charsto);
     
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
  extern u32 _end;
  volatile u32 *bss_end = &_end;

  upg_cfg.upg_all_flag = FALSE;
  upg_cfg.unzip_buff_start = (u32)bss_end + GAP_SIZE; //the inflate buff

  //default flash map start address starts within 2M
  upg_cfg.flash_map_start = upg_cfg.unzip_buff_start + SDRAM_UNZIP_BUFF_SIZE;
  upg_cfg.start_burn_addr = 0;
  upg_cfg.upg_all_size = 0;

  drv_init();
  spi_flash_init();
  crc_setup_fast_lut(CRC_MODE);
  //set_uart_high_speed();
#ifdef WIN32
  uartwin_init(1);
#else
  uart_ioctl(UART_CMD_EN_INTERRUPT, FALSE, 0); //close interupt to use polling mode
#endif

  //mtos_close_printk();

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
    //mtos_task_sleep(200);
    get_cmd();
    dispatch_cmd();
  }

  return 0;
}
