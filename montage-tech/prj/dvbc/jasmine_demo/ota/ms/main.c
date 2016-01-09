/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
/*!
  Win32 main file.
  */

// standard library
#include <malloc.h>
#include <assert.h>

#include "string.h"

#include "sys_types.h"
#include "sys_define.h"

#include "sys_regs_warriors.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
#include "common.h"
// os
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_int.h"
#include "mem_manager.h"
#include "hal.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "mem_cfg.h"

#include "hal_misc.h"

#include "drv_dev.h"
#include "service.h"
#include "class_factory.h"
//#include "dsmcc.h"
#include "Mdl.h"
#include "nim.h"
#include "Nim_ctrl_svc.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "LzmaIf.h"
#include "fcrc.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "uio.h"
#include "charsto.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "ota_public_def.h"
#include "ota_dm_api.h"

#define M_DM_HEADER_ADDR  (0xC)
#define M_CODE_MASK       (0xFFFF)
#define M_CODE_LOW_MASK   (0xFF)
#define M_CODE_HIGH_MASK  (0x100)
#define M_CHARSTO_SIZE    (4)
#define M_PARTITION_SIZE  (9)
#define M_TASK_SLEEP_TIME (25)
#define M_TASK_DELAY_TIME (200)

static BOOL g_crc_result = FALSE;

u32 get_mem_addr(void)
{
  static u8 g_sys_mem[SYS_MEMORY_END] = {0};
  return (u32)&g_sys_mem[0];
}

extern u32 get_flash_addr(void);

u32 get_bootload_off_addr(void)
{
  return M_DM_HEADER_ADDR;
}
BOOL is_force_key_press()
{
  void *p_dev = NULL;
  u16 code = M_CODE_MASK;
  u8 hkey = M_CODE_LOW_MASK;

  p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);
  #if 0
  //get second hkey from fp.bin.
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 1, sizeof(u8), &hkey);
  #else
   mul_ota_dm_api_dm_read(FPKEY_BLOCK_ID,
                        1,
                       sizeof(u8),
                       &hkey);
  #endif
  uio_get_code(p_dev, &code);
  OS_PRINTF("is_force_key_press hkey %x , code %x \n", hkey, code);
  //make sure it's fp key.
  if((code & M_CODE_HIGH_MASK) && (hkey == (code & M_CODE_LOW_MASK)))  
  {
    return TRUE;
  }

  return FALSE;
}
extern void ui_ota_dm_api_init(void);
extern void ui_ota_api_bootload_info_init(void);
extern void uio_init();
BOOL ota_check(void)
{
  ota_bl_info_t bl_info = {0};
  charsto_cfg_t charsto_cfg = {0};
  void  *p_dev = NULL;
  RET_CODE ret = ERR_FAILURE;

  /******char storage init******/
  ret = ATTACH_DRIVER(CHARSTO, warriors, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);

  charsto_cfg.size = CHARSTO_SIZE; //4
  charsto_cfg.spi_clk = FLASH_C_CLK_P6;
  charsto_cfg.rd_mode = SPI_FR_MODE;
  //charsto_cfg.multi_io_rd = 1;
  //charsto_cfg.multi_io_wd = 1;
  ret = dev_open(p_dev, &charsto_cfg);
  MT_ASSERT(ret == SUCCESS);

  /* open uio */
  uio_init();

  mtos_task_delay_ms(M_TASK_DELAY_TIME);

  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
      ui_ota_api_bootload_info_init();
  }

  mul_ota_dm_api_read_bootload_info(&bl_info);
  OS_PRINTF("\r\n[OTA]%s:ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);



  /* flash burning is not finished, force ota*/
  if( bl_info.destroy_flag == TRUE) 
  {
      bl_info.ota_status = OTA_TRI_MODE_FORC;
      mul_ota_dm_api_save_bootload_info(&bl_info);
      return TRUE;
  }

  if(is_force_key_press())
  {
    bl_info.ota_status = OTA_TRI_MODE_AUTO;
    mul_ota_dm_api_save_bootload_info(&bl_info);
    return TRUE;
  }

  if((bl_info.ota_status == OTA_TRI_MODE_AUTO)
  ||(bl_info.ota_status == OTA_TRI_MODE_FORC))
  {
      return TRUE;
  }

  return FALSE;
}

u32 get_otaback_off_addr(void)
{
  return DM_HDR_START_ADDR_BAK;
}

u32 get_maincode_off_addr(void)
{
  return DM_HDR_START_ADDR;
}

void *my_malloc(u32 size)
{
  mem_mgr_alloc_param_t para = {0};
  para.id = MEM_SYS_PARTITION;
  para.size = size;
  return MEM_ALLOC(&para);
  //return malloc(size);
}

void my_free(void *p_addr)
{
  mem_mgr_free_param_t para = {0};
  para.id = MEM_SYS_PARTITION;
  para.p_addr = p_addr;
  FREE_MEM(&para);
  //free(p_addr);
}

void task_sys_init(void *p_data)
{
  BOOL ret = FALSE;
  u32 *p_pstack_pnt = NULL;
  mem_mgr_partition_param_t partition_param = { 0 };
  extern void Task_SysTimer(void *p_data);
  extern void ap_init(void);
  hal_config_t hal_config = {0};

  hal_win32_attach(&hal_config);

  //init memory manager, to the memory end
  mem_mgr_init((u8 *)get_mem_addr(), SYS_MEMORY_END);

  /* create SYS partition */
  partition_param.id   = MEM_SYS_PARTITION;
  partition_param.size = SYS_PARTITION_SIZE;
  partition_param.p_addr = (u8 *)get_mem_addr();
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_SYSTEM;
  partition_param.method_id = MEM_METHOD_NORMAL;
  ret = MEM_CREATE_PARTITION(&partition_param);
  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create patition ok!\n");

  //register malloc/free function, using system partition
  mtos_mem_init(my_malloc, my_free);
  OS_PRINTF("init mem ok!\n");

  mtos_ticks_init(SYS_CPU_CLOCK);

  //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);

  #ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
  #endif

  //create timer task
  p_pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
  MT_ASSERT(p_pstack_pnt != NULL);

  ret = mtos_task_create((u8 *)"Timer",
    Task_SysTimer,
    (void *)0,
    SYS_TIMER_TASK_PRIORITY,
    p_pstack_pnt,
    SYS_TIMER_TASK_STKSIZE);

  MT_ASSERT(FALSE != ret);
  OS_PRINTF("create timer task ok!\n");

  mtos_task_sleep(M_TASK_SLEEP_TIME);//25
  OS_PRINTF("start ap init!\n");

  ret = ota_check();
  if(ret == TRUE)
  	{
	  ap_init();
	  OS_PRINTF("ap init ok!\n");
  	}
	else
	{
	  MT_ASSERT(0);
	}

  mtos_task_exit();
}

int main_entry(void)
{
  u8 *p_stack = malloc(SYS_INIT_TASK_STKSIZE);
  mtos_cfg_t os_cfg = {0};

  os_cfg.enable_bhr = FALSE;
  mtos_os_init(&os_cfg);

  mtos_task_create((u8 *)"initT",
    task_sys_init,
    NULL, 
    SYS_INIT_TASK_PRIORITY,
    (u32 *)p_stack,
    SYS_INIT_TASK_STKSIZE);

  /* begin to run multi-task */
  mtos_start();
  return 0;
}