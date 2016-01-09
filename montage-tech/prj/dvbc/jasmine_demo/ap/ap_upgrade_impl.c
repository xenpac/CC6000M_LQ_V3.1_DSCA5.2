/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include <string.h>
#include<stdio.h>
#include "sys_define.h"
#include "sys_types.h"



#include "class_factory.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "sys_regs_concerto.h"

#include "ap_framework.h"

#include "drv_dev.h"
#include "video.h"
#include "uio.h"
#include "sys_cfg.h"
#include "mem_manager.h"
#include "data_manager.h"
#include "ap_framework.h"

#include "hal_uart.h"

#include "ap_upgrade.h"
#include "..\..\..\..\src\ap\upg\ap_upg_priv.h"

#include "ap_uio.h"

#include "mem_cfg.h"
#include "lib_rect.h"
#include "lib_memp.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "lib_memf.h"
#include "flinger.h"
#include "gdi.h"
#include "ctrl_base.h"
#include "gui_resource.h"
#include "gui_paint.h"
#include "framework.h"
#include "mdl.h"
#include "uio_key_def.h"

#define BAUDRATE_NORMAL       115200

#define BAUDRATE_SYNC       115200

#ifdef DISABLE_FP_ON_UPG
#define BAUDRATE_UPG  115200/4
#else
#define BAUDRATE_UPG  115200
#endif


uio_device_t *p_uio_dev = NULL;

extern u8 get_key_value(u8 key_set, u8 vkey);

/*!
  find hardware code for the exit/power key
  */
u16 ap_upg_get_code(u32 index, u16 *code)
{
  u8 v_key = V_KEY_CANCEL;
  u16 cnt = 0;

  switch(index)
  {
    case UPGRD_CODE_EXIT:
      v_key = V_KEY_CANCEL;
      break;
    case UPGRD_CODE_POWER:
      v_key = V_KEY_POWER;
      break;
    case UPGRD_CODE_MENU:
      v_key = V_KEY_MENU;
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  //code[0] = get_key_value(0, v_key);
  code[0] = get_key_value(0, v_key);
  cnt++;
#ifndef WIN32
  code[1] = get_key_value(1, v_key);
  cnt++;
#endif

  return cnt;
}


/*!
  init upgrade basic information
  */
upg_ret_t ap_upg_init(upg_config_t *p_cfg)
{
  class_handle_t dm_hdl;

  p_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_uio_dev);

  dm_hdl = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(NULL != p_cfg);
  //lint -e613
  p_cfg->flash_size = CHARSTO_SIZE;
  p_cfg->uart_id = 0;
  p_cfg->zip_type = UPG_ZIP_NONE;

  p_cfg->upg_client_addr = (u32)(dm_get_block_addr(dm_hdl, UPG_TOOL_BLOCK_ID)) - get_flash_addr();

  p_cfg->upg_client_size = dm_get_block_size(dm_hdl, UPG_TOOL_BLOCK_ID);

  p_cfg->upg_baudrate = BAUDRATE_UPG;

  p_cfg->normal_baudrate = BAUDRATE_NORMAL;

  p_cfg->sync_baudrate = BAUDRATE_SYNC;

  p_cfg->upg_chip_type  =  UPG_CHIP_WARRIORS;

  //p_cfg->inflate_size = UPG_INFLATE_SIZE;

  //p_cfg->unzip_size = UPG_UNZIP_SIZE;

  p_cfg->unzip_size = p_cfg->upg_client_size;
  //lint +e613
#ifdef WIN32
  uart_init(1);
#endif

  return UPG_RET_SUCCESS;
}

upg_ret_t ap_upg_init_chip(upg_config_t *p_cfg)
{
  return p2p_ddr_init_chip();
}

/*!
  check if the block id exist in slave side
  */
upg_ret_t ap_upg_block_process(u32 block_num, upg_block_t *p_block, slave_info_t *p_info)
{
  u32 i = 0, j = 0;
  u32 flag = FALSE;
  for(j = 0; j < block_num; j++)
  {
    flag = FALSE;
    for(i = 0; i < p_info->block_num; i++)
    {
      //find if the block ID exists in slave side
      if(p_block->master_block_id == p_info->blocks[i].id)
      {
        p_block->slave_block_id = p_info->blocks[i].id;
        flag = TRUE;
        break;
      }
    }
    if(FALSE == flag)
      return UPG_RET_ERROR;
  }
  return UPG_RET_SUCCESS;
}

void * upg_mem_alloc(u32 size)
{
  void *p_buf = NULL;
  mem_mgr_alloc_param_t  alloc_t = {0};

  alloc_t.id      =  MEM_SYS_PARTITION;
  alloc_t.size    =  size ;
  alloc_t.user_id =  SYS_MODULE_UPG ;

  p_buf = MEM_ALLOC(&alloc_t);

  MT_ASSERT(p_buf != NULL);
  return p_buf;
}

void upg_mem_free(void *p_buf)
{
  mem_mgr_free_param_t free_t = {0};

  free_t.id     = MEM_SYS_PARTITION;
  free_t.p_addr = p_buf;
  free_t.user_id= SYS_MODULE_UPG;

  //Memory allocation
  FREE_MEM(&free_t);
}

/*!
  clean the enviroment
  */
void ap_upg_pre_start(upg_config_t *p_cfg)
{
  u32 total_size = 0;

  total_size = p_cfg->inflate_size + p_cfg->unzip_size + p_cfg->upg_client_size;
  //alloc client used buffer
  p_cfg->p_inflate_buffer = (u8 *)mtos_malloc(total_size);
  MT_ASSERT(p_cfg->p_inflate_buffer != NULL);
  memset(p_cfg->p_inflate_buffer, 0, total_size);

  p_cfg->p_unzip_buffer =p_cfg->p_inflate_buffer + p_cfg->inflate_size;
  p_cfg->p_zip_buffer = p_cfg->p_unzip_buffer + p_cfg->unzip_size;
  //alloc info buffer
  p_cfg->p_slave_info = (slave_info_t*)mtos_malloc(sizeof(slave_info_t));
  MT_ASSERT(p_cfg->p_slave_info != NULL);
  memset(p_cfg->p_slave_info, 0, sizeof(slave_info_t));


  ap_upg_get_code(UPGRD_CODE_EXIT, p_cfg->exit_key);
  ap_upg_get_code(UPGRD_CODE_MENU, p_cfg->menu_key);
}

/*!
  post exit handle
  */
void ap_upg_post_exit(upg_config_t *p_cfg)
{
  uart_set_param(0, p_cfg->normal_baudrate, 0, 0, 0);

  if(p_cfg->p_inflate_buffer != NULL)
  {
    mtos_free(p_cfg->p_inflate_buffer);
    p_cfg->p_inflate_buffer = NULL;
  }
  if (p_cfg->p_slave_info != NULL)
  {
    mtos_free(p_cfg->p_slave_info);
    p_cfg->p_slave_info = NULL;
  }
}

static void ap_upg_set_key(s8 *p_key_code, upg_config_t *p_cfg)
{
  u16 code = 0xFFFF;
  u8 key_usr = 0;

  if(SUCCESS == uio_get_code(p_uio_dev, &code))
  {
    key_usr = (code >> (UIO_USR_CODE_SHIFT + 8)) & 0x7;
    if(p_cfg->exit_key[key_usr] == (code & 0xff))
    {
      *p_key_code = UPGRD_KEY_EXIT;
    }
    if(p_cfg->menu_key[key_usr] == (code & 0xff))
    {
      *p_key_code = UPGRD_KEY_MENU;
    }
  }
}

typedef enum
{
 CHIP_DEVICE_LOW = 0, // low 16 bit
 CHIP_DEVICE_HIGH,    //high 16 bit
 CHIP_DEVICE_ALL     //32 bit
}CAL_DEVIC_ID;

#define CONFIG_NAME_MAX_LEN  64
#define SECTION_COUNT 3
#define CONFIG_SECTION_COUNT 30

typedef struct
{
char name[CONFIG_NAME_MAX_LEN];
u32 value;
}name_value;

typedef struct
{
char *section[SECTION_COUNT];
name_value config[SECTION_COUNT][CONFIG_SECTION_COUNT];
}ddr_config;

 #define RDCLK_DLY_MAX   (64)
 typedef struct chip_ddrcal_outcome
{
u8 start_ok;
u8 end_ok;
u8 rang_count;

u8 rdclk_rise_latch_ok;
u8 digclk_neglatch_ok;
u8 rl_add_ok[RDCLK_DLY_MAX];
u8 dly_clk;
}chip_ddrcal_outcome_t;

ddr_config ddr_g_config =
{
  {"#CONFIG_SETTING#","#INIT_REGS#","#CONFIG_REGS#"},
  {{
     {"CALIBRATION_METHOD",0},{"SCAN_MODE",0},
     {"NO_CALIBRATION",0},{"DEVICE_SELECT",2},
     {"CLOCK_OFFSET",(u32)-0x11},{"DELAY_90_OFFSET",0},
     {"DELAY_9090_OFFSET",0},{"DQS_IN_90_OFFSET",0},
     {"RDCLK_DLY_MAX",0x28},{"RANGE_SELECT",2},
    },
    { {"0xbfd40010",0x20189374},
       {"0xbff70088",0xfe380d},{"0xbfedc050",2},
       {"sleep",1},{"0xbfedc050",0},
       {"0xbfd70024",0x1051206},{"0xbfd70028",0x30020504},
       {"0xbfd7002c",0x18020618},{"0xbfd70038",0x72120001},
       {"0xbfd700a4",0x9000},{"0xbfd70048",0x10000010},
       {"0xbfd700a0",0},
    },
    {
      {"0xbfd70050",0x0910},{"0xbfd700ac",0x04040101},
      {"0xbfd700b0",0x00000101},{"0xbfd700b4",0x0f000000},
      {"0xbfd700a0",0x86000000},{"0xbfd700b8",0x06060101},
      {"0xbfd700a8",0x22110222},{"0xbfd700bc",0x74017284},
      {"0xbfd70048",0x13010110},{"sleep",1},
      {"0xbfd70018",0},{"sleep",2},
      {"0xbfd70000",1},{"0xbfd70020",0x8000},
      {"0xbfd70004",1},{"0xbfd70020",0xC000},
      {"0xbfd70004",1},{"0xbfd70020",0x4000},
      {"0xbfd70004",1},{"sleep",1},
      {"0xbfd70000",1},{"0xbfd70008",1},
      {"0xbfd70008",1},{"0xbfd70020",0xa62},
      {"0xbfd70004",1},{"0xbfd70020",0x4384},
      {"0xbfd70004",1},{"0xbfd70020",0x4004},
      {"0xbfd70004",1},{"0xbfd70018",1},
    }
  },
};



#define AVCPU_REG_BASE  0xBFD00000
#define AV_DMA_SRC_ADDR (AVCPU_REG_BASE+0x54)
#define AV_DMA_GOAL_ADDR (AVCPU_REG_BASE+0x58)
#define AV_DMA_HW (AVCPU_REG_BASE+0x5C)
#define AV_DMA_ALU (AVCPU_REG_BASE+0x60)
#define AV_DMA_CMD (AVCPU_REG_BASE+0x64)

 static u8 writed;
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
  uart_write(UART_UPG_ID, p_buff, len);

  return len;
}


static void p2p_clear_data()
{
  uart_flush(UART_UPG_ID);
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

  for(i = 0; i < len; i++)
  {
    uart_write_byte(UART_UPG_ID, p_buff[i]);
  }

  return i;
}

#endif  //end of #ifdef WIN32


BOOL send_instruct(u32 opcode, u32 addr, u32 val, u32 mode)
{
    u8 instruct_buf[16];
    u32 send_size = 10;
    char code;

    memset(instruct_buf, 0, sizeof(instruct_buf));
    code = (char)opcode;
    memcpy(instruct_buf, "#", 1);
    memcpy(instruct_buf+1, &code,1);

    if(mode != 0)
   {
        memcpy(instruct_buf+2, &mode, 1);
        memcpy(instruct_buf+3, &addr, 4);
    }
    else
    {
        memcpy(instruct_buf+2, &addr, 4);
    }

    if(opcode == 1)
    {
        memcpy(instruct_buf+6, &val, 4);
        send_size = 10;
    }

    if(opcode == 2)
    {
        memcpy(instruct_buf+6, &val, 2);
        send_size = 8;
    }

    if(opcode == 3)
    {
        memcpy(instruct_buf+6, &val, 1);
        send_size = 7;
    }

    if(opcode == 4)
    {
        send_size = 6;
    }

    if(opcode == 5)
    {
        if(mode == 1)
        {
            memcpy(instruct_buf+7, &val, 4);
            send_size = 11;
        }

        if(mode == 2)
        {
            memcpy(instruct_buf+7, &val, 2);
            send_size = 9;
        }

        if(mode == 3)
        {
            memcpy(instruct_buf+7, &val, 1);
            send_size = 8;
        }
    }

    if(send_size == p2p_send_data(instruct_buf, send_size))
        return TRUE;

    return FALSE;
}


BOOL addr_read(u32 addr, u8 *data)
{
    if(TRUE != send_instruct(4, addr, 0, 0))
    {
        return FALSE;
    }
    if(UPG_RET_SUCCESS == p2p_get_data(data, 4, 300))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL addr_write(u32 addr,u32 data, u32 cnt, BOOL verify)
{
    u32 opcode = 0;
    u32 mode = 0;
    u8 response = 0;
    
    if(verify)
     {
        if(cnt == 4)
        {
            mode = 1;
         }
        else if(cnt == 2)
        {
            mode = 2;
        }
        else
        {
            mode = 3;
        }

        if(TRUE != send_instruct(5, addr, data, mode))
        {
            return FALSE;
         }
        if(UPG_RET_SUCCESS == p2p_get_data(&response,1, 2000))
        {
            if(response == 'S')
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        if(cnt == 4)
        {
            opcode = 1;
        }
        else if(cnt == 2)
        {
            opcode = 2;
        }
        else
        {
            opcode = 3;
        }

        if(TRUE != send_instruct(opcode, addr, data, 0))
        {
            return FALSE;
        }
    }

    return FALSE;
}

int get_value_from_config(ddr_config *section, char *section_name,
                char *name, u32 *value)
{
u8 i=0,j=0;
for(; i<SECTION_COUNT; i++)
{
if(!strcmp(section->section[i], section_name))
{
    for(;j<CONFIG_SECTION_COUNT;j++)
    {
       if(!strcmp(section->config[i][j].name,name))
       {
         *value = section->config[i][j].value;
         return 1;
       }
    }
}
}
return -1;
}


u32 set_regs_from_config(ddr_config *section,u8 *config_name)
{
u32 address = 0;
u8 i=0,j=0;
for(; i<SECTION_COUNT; i++)
{
  if(!strcmp(section->section[i], (char*)config_name))
  {
   for(;j<CONFIG_SECTION_COUNT;j++)
  {
       if(!strcmp(section->config[i][j].name, "sleep"))
      {
        mtos_task_sleep(section->config[i][j].value);
      }
       else
       {
        sscanf(section->config[i][j].name, "%x", (unsigned int *)&address);
         addr_write(address, section->config[i][j].value, 4, 0);
       }
   }
  }
}
return 0;
}


 void dma_transfer(unsigned int dst, unsigned int src, unsigned int len)
{
    unsigned int temp;
    unsigned int cn = mtos_ticks_get();

    addr_write(AV_DMA_SRC_ADDR, (src & 0x0fffffff), 4, 0);
    addr_write(AV_DMA_GOAL_ADDR, (dst & 0x0fffffff), 4, 0);
    addr_write(AV_DMA_HW, len | 0x10000, 4, 0);
    addr_write(AV_DMA_CMD,0x2, 4, 0); //cp
    addr_read(AV_DMA_CMD, (u8 *)&temp);

    while(0x80000000 & temp)
    {
        addr_read(AV_DMA_CMD, (u8 *)&temp);
        //mt_console_printf("dma_transfer failed!\n");
        if (mtos_ticks_get() - cn >100)
        {
            return;
        }
    }
 }


s32 chip_ddr_test_rw(CAL_DEVIC_ID dev_id, unsigned int addr)
{
    //int count = 1;
    u8 i = 0;
    unsigned int tmp = 0;
    unsigned int mask = 0;

    if(dev_id == CHIP_DEVICE_LOW)
    {
        mask = 0xFFFF;
    }
    else if(dev_id == CHIP_DEVICE_HIGH)
    {
        mask = 0xFFFF0000;
    }
    else if(dev_id == CHIP_DEVICE_ALL)
    {
        mask = 0xFFFFFFFF;
    }

    /* write source memory, if success, no need to do it again. */
    if(!writed)
    {
        addr = 0xaaaaaaa0;
        for(i=0; i<8; i++)
        {
            addr_write(addr+8*i, 0x5555aaaa, 4, 0);
            addr_write(addr+8*i+4, 0xaaaa5555, 4, 0);
        }
        addr = 0xaaaaaae0;
        for(i=0; i<8; i++)
        {
            addr_write(addr+8*i, 0xaaaa5555, 4, 0);
            addr_write(addr+8*i+4, 0x5555aaaa, 4, 0);
        }

        addr = 0xa0000000;
        for(i=0; i<8; i++)
        {
            addr_write(addr+8*i, 0, 4, 0);
            addr_write(addr+8*i+4, 0xffffffff, 4, 0);
        }

        addr = 0xa0000040;
        for(i=0; i<8; i++)
        {
            addr_write(addr+8*i, 0xffffffff, 4, 0);
            addr_write(addr+8*i+4, 0, 4, 0);
        }
        writed = 1;
    }

    /* erase target memory */
    addr = 0xa5555550;
    for(i=0; i<8; i++)
    {
        addr_write(addr+8*i, 0xaaaa5555, 4, 0);
        addr_write(addr+8*i+4, 0x5555aaaa, 4, 0);
    }
    addr = 0xa5555590;
    for(i=0; i<8; i++)
    {
        addr_write(addr+8*i, 0x5555aaaa, 4, 0);
        addr_write(addr+8*i+4, 0xaaaa5555, 4, 0);
     }
    addr = 0xafffff80;
    for(i=0; i<8; i++)
    {
        addr_write(addr+8*i, 0xffffffff, 4, 0);
        addr_write(addr+8*i+4, 0, 4, 0);
    }
    addr = 0xa00000e0;
    for(i=0; i<8; i++)
    {
        addr_write(addr+8*i, 0, 4, 0);
        addr_write(addr+8*i+4, 0xffffffff, 4, 0);
    }

    /* DMA source to target */
    dma_transfer(0xa5555550, 0xaaaaaaa0, 128);
    dma_transfer(0xafffff80, 0xa0000000, 128);

    /* check */
    addr = 0xa5555550;
    for(i=0; i<8; i++)
    {
        addr_read(addr+8*i, (u8 *)&tmp);
        if((tmp & mask) != (0x5555aaaa&mask))
        {
            return -1;
        }
        addr_read(addr+8*i+4, (u8 *)&tmp);
        if((tmp & mask) != (0xaaaa5555&mask))
        {
            return -1;
        }
    }

    addr = 0xa5555590;
    for(i=0; i<8; i++)
    {
        addr_read(addr+8*i,  (u8 *)&tmp);
        if((tmp & mask) != (0xaaaa5555&mask))
        {
            return -1;
        }

        addr_read(addr+8*i+4,  (u8 *)&tmp);
        if((tmp & mask) != (0x5555aaaa&mask))
        {
            return -1;
        }
    }

    addr = 0xafffff80;
    for(i=0; i<8; i++)
    {
        addr_read(addr+8*i,  (u8 *)&tmp);
        if((tmp & mask) != (0&mask))
        {
            return -1;
        }

        addr_read(addr+8*i+4,  (u8 *)&tmp);
        if((tmp & mask) != (0xffffffff&mask))
        {
            return -1;
        }
    }

    addr = 0xafffffc0;
    for(i=0; i<8; i++)
    {
        addr_read(addr+8*i,  (u8 *)&tmp);
        if((tmp & mask) != (0xffffffff&mask))
        {
            return -1;
        }

        addr_read(addr+8*i+4,  (u8 *)&tmp);
        if((tmp & mask) != (0&mask))
        {
            return -1;
        }
    }

    return 0;
}

u32 set_register_bits(u32 addr, u8 bit_offset,u8 bit_num,u32 value)
{
    u32 data = 0;
    
    addr_read(addr, (u8 *)&data);
    data = (data & (~((((u32)1 << bit_num) - 1) << bit_offset))) | (value << bit_offset);
    addr_write(addr, data, 4, 0);

    return data;
}


static void ap_upg_set_brom(u32 index,u32 size)
{
   u32 brom[20] = {0x4, 0x41000000, UART_UPG_BP, 0, 0, 0};
   uart_set_param(0, index, 0, 0, 0);

  mtos_task_delay_ms(10);
 // p2p_send_data((u8 *)(&brom[3]), 12);
  brom[0] = size;
  brom[1] = 0x80008000;
  brom[2] = 0x80008000;
  p2p_send_data((u8 *)(&brom[0]), 12);  //send data out
}


//calculate clock delay and dqs delay

 upg_ret_t  p2p_ddr_init_chip(void)
{
    return UPG_RET_SUCCESS;
}

/*!
  construct the upg policy
  */
upg_policy_t *construct_upg_policy(void)
{
  upg_policy_t *p_upg_policy = mtos_malloc(sizeof(upg_policy_t));
  MT_ASSERT(p_upg_policy != NULL);

  //lint -e613
  p_upg_policy->p_init = ap_upg_init;
  p_upg_policy->p_block_process = ap_upg_block_process;
  p_upg_policy->p_pre_start = ap_upg_pre_start;
  p_upg_policy->p_post_exit = ap_upg_post_exit;
  p_upg_policy->p_get_code = ap_upg_get_code;
  p_upg_policy->p_set_key = ap_upg_set_key;
  p_upg_policy->p_set_brom= ap_upg_set_brom; 
  p_upg_policy->p_init_chip= ap_upg_init_chip; 
  //lint +e613
  
  return p_upg_policy;
}

