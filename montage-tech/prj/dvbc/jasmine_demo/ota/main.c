/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
// system
// std
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
//#include "sys_regs_magic.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
#include "common.h"
// os
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
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
#include "Ss_ctrl.h"
#include "LzmaIf.h"
#include "fcrc.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "uio.h"
#include "charsto.h"

#include "ota_public_def.h"
#include "ota_dm_api.h"
#include "hal_watchdog.h"
#include "spi.h"


//static BOOL crc_result = FALSE;
char TaskStartStk[SYS_INIT_TASK_STKSIZE] __attribute__ ((aligned (8)));

s32 hal_uart_warriors_b0_write_byte(u8 id, u8 chval);

extern void ota_read_otai(ota_info_t *p_otai);
extern void ota_write_otai(ota_info_t *p_otai);
extern BOOL is_burn_finish(void);
extern void sys_status_init(void);
extern void hal_warriors_attach(hal_config_t *p_config);
extern void  OSStart (void);
extern void Task_SysTimer(void *p_data);
extern void ap_init(void);
extern void uio_init(void);
extern void set_flash_error(BOOL b_error);

//boot code start
#define PARA_ADDRESS   (0xbfef0180)
#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest));  __asm__("nop");}
#define REBACK_BOOT_FLAG  (0x1234)


char buildSTR[]=__DATE__"--"__TIME__;

u32 boot_jump_ID = 0x0;
u32 back_addr = 0x0;
volatile u32* boot_para1 = NULL;//jump to which code
volatile u32* boot_para2 = NULL;//jump address
volatile u32* boot_para3 = NULL;//HDMI flag

void reback_to_boot(void)
{
  //hal_watchdog_enable();
  hal_watchdog_init(0);
  hal_watchdog_enable();
  //hal_pm_reset();
}
//boot code end

#if 0
static void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  //MT_ASSERT(p_addr != NULL);
  memset(p_addr, 0, size);

  return p_addr;
}

static void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}
#endif

u32 get_mem_addr(void)
{
  return 0xa0000000;
}

u32 get_flash_addr(void)
{
  return 0x9e000000;
}


BOOL is_force_key_press()
{
  void *p_dev = NULL;
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  u32 i = 5;

  p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);

  //get second hkey from fp.bin.
   #if 0
    dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 1, sizeof(u8), &hkey);
  #else
    mul_ota_dm_api_dm_read(FPKEY_BLOCK_ID,
                        1,
                       sizeof(u8),
                       &hkey);
  #endif
  while(i > 1)
  {
    uio_get_code(p_dev, &code);
    OS_PRINTF("is_force_key_press hkey %x , code %x \n", hkey, code);
  //make sure it's fp key.
    if((code & 0x100) && (hkey == (code & 0xFF)))
    {
      return TRUE;
    }
    i --;
    mtos_task_delay_ms(200);
  }

  return FALSE;
}
u8 serial_num[64] = {0xFF};

extern void ui_ota_dm_api_init(void);
extern void ui_ota_api_bootload_info_init();
ota_trigger_t ota_check(void)
{
  ota_bl_info_t bl_info = {0};

  //ui_ota_dm_api_init();
  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
    ui_ota_api_bootload_info_init();
  }

  mul_ota_dm_api_read_bootload_info(&bl_info);
  OS_PRINTF("\r\n[OTA]%s:ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);

  {
    #if 0
      dm_read(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID, 0, 0, sizeof(serial_num), serial_num);
    #else
      mul_ota_dm_api_dm_read(IDENTITY_BLOCK_ID,
                        0,
                       sizeof(serial_num),
                       serial_num);
  #endif
  }

  if(bl_info.ota_status == OTA_TRI_MODE_FORC)
  {
    OS_PRINTF("OTA it will forc ota\n");
    return TRUE;
  }
  /* flash burning is not finished, force ota*/
  if( bl_info.destroy_flag == TRUE)
  {
    bl_info.ota_status = OTA_TRI_MODE_FORC;
    mul_ota_dm_api_save_bootload_info(&bl_info);
    OS_PRINTF("OTA it will forc ota by dstroy flag\n");
    return TRUE;
  }

  if(is_force_key_press())
  {
    bl_info.ota_status = OTA_TRI_MODE_AUTO;
    mul_ota_dm_api_save_bootload_info(&bl_info);
    OS_PRINTF("OTA it will auto ota by key tri\n");
    return TRUE;
  }

  if((bl_info.ota_status == OTA_TRI_MODE_AUTO)
  ||(bl_info.ota_status == OTA_TRI_MODE_FORC))
  {
    OS_PRINTF("OTA it will auto ota or forc\n");
    return TRUE;
  }

  bl_info.load_block_id = MAINCODE_BLOCK_ID;
  mul_ota_dm_api_save_bootload_info(&bl_info);

  return FALSE;
  }

extern void hal_concerto_attach(void);
void TaskStart(void* p_data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
  ota_trigger_t ota_trigger = 0;
  //uio_device_t *p_dev = NULL;
  u32* pstack_pnt = NULL;
//  u8 content[5] = {' ', ' ', 'O', 'N', 0};
  u32 heap_start = 0;
  u32 cpuclk = 0;
  u32 sys_partition_size = 0;

  heap_start = ROUNDUP(bss_end,4);
  mtos_irq_init();

  hal_concerto_attach( );
  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);

  hal_module_clk_get(HAL_CPU0, &cpuclk);
  mtos_ticks_init(cpuclk);

  uart_init(0);
  uart_set_param(0, 115200, 8, 1, 0);

  OS_PRINTF("\n OTA TaskStart\n");
  OS_PRINTF("\n Built at %s \n", buildSTR);

  OS_PRINTF("\n 1. Heap_start[0x%x]\n", heap_start);
  OS_PRINTF("\n 2. Whole memory size[0x%x]\n", (WHOLE_MEM_END & 0xFFFFFFF));
  sys_partition_size = (((OSD1_VSCALER_BUF_ADDR & (~0xa0000000)) - (heap_start & (~0xa0000000))) / 0x400) * 0x400;
  OS_PRINTF("\n 3. Sys_partition_size[0x%x]\n", sys_partition_size);

  mem_mgr_init((u8 *)heap_start,
  sys_partition_size);

  OS_PRINTF("\n 4. Mem_mgr_start[0x%x],size=0x%x\n", heap_start,
  	sys_partition_size);
  dlmem_init((void *)heap_start,
  	sys_partition_size);
  mtos_mem_init(dl_malloc, dl_free);
  OS_PRINTF("Init mem ok!\n");


//enable interrupt
  mtos_irq_enable(TRUE);

   //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);


#ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
#endif

  //create timer task
  pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);

  ret = mtos_task_create((u8 *)"Timer",
         Task_SysTimer,
         (void *)0,
         SYS_TIMER_TASK_PRIORITY,
         pstack_pnt,
         SYS_TIMER_TASK_STKSIZE);

  MT_ASSERT(FALSE != ret);

  mtos_task_sleep(25);


  ap_init();
  OS_PRINTF("==========OTA check djd================\n");
  ret  = ota_check();
  OS_PRINTF("OTA check %d\n", ota_trigger);
  //{
  //  ota_info_t otai = {0};
  //  ota_read_otai(&otai);
  //  otai.ota_tri = OTA_TRI_FORC;
  //  otai.sys_mode = SYS_DVBS;
  //  ota_write_otai(&otai);
  //}
   //p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
   //MT_ASSERT(NULL != p_dev);

  if(ret == FALSE)
  {
    //clear frontpanel.
    //uio_display(p_dev, "    ", 4);
    reback_to_boot();
  }
  else
  {
    //uio_display(p_dev, content, 4);
  }

  mtos_task_exit( );

}
extern void EXCEP_vInstallIntInRam(void);
int main(int p1, char **p2, char **p3)
{
  mtos_cfg_t os_cfg = {0};

  EXCEP_vInstallIntInRam();

  /* Initialize uC/OS-II                                      */

  mtos_os_init(&os_cfg);

  mtos_task_create((u8 *)"initT1",
                 TaskStart,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)TaskStartStk,
                 SYS_INIT_TASK_STKSIZE);

  OSStart();                              /* Start multitasking    */

  return 0;
}

