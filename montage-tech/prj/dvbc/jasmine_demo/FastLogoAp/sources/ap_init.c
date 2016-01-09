/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "../../includes/sys_devs.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "sys_cfg.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_otp.h"
#include "hal.h"
#include "ipc.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "drv_svc.h"
#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "class_factory.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "fcrc.h"
#include "mtos_misc.h"
#include "hal_base.h"
#include "mem_cfg.h"
#include "mem_manager.h"
#include "lib_rect.h"
#include "common.h"
#include "display.h"
#include "gpe_vsb.h"
#include "hdmi.h"
#include "driver.h"
#include "region.h"
#include "lib_memp.h"
#include "hal_gpio.h"
#include "uio.h"
#include "pdec.h"
#include "mdl.h"
#include "nim.h"
#include "dmx.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "lib_util.h"
#include "ss_ctrl.h"
#include "../../../../src/drv/drvbase/drv_svc.h"
//#include "../../ap/ap_flounder_ota.h"
#include "../../sysstatus/sys_status.h"
#include "../../includes/customer_config.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "lpower.h"
#include "showlogo.h"
#include "fcrc.h"

#include "ota_public_def.h"
#include "ota_dm_api.h"
#include "spi.h"
#include "LzmaIf.h"
#include "hal_watchdog.h"
#include "showlogo.h"

#define LOGO_SIZE           (1280 * 720 * 4 + 0x100000)
#define STILL_BUFF_ADDR     (GUI_RESOURCE_BUFFER_ADDR -  LOGO_SIZE)
#define PDEC_OUT_ADDR       (0x80000000 | STILL_BUFF_ADDR)
#define HW_STILL_BUFF_ADDR  (STILL_BUFF_ADDR/8)

#define FP_TABLE_SIZE sizeof(fp_bitmap)/sizeof(led_bitmap_t)
#define DM_ADDRESS_OFFSET   (0x10000)
#define MAINCODE_SIZE	(0x500000)
#define MAINCODE_OUT_ZIE (0xA00000)
#define MAINCODE_RUN_ADDR	 (0x80008000)
#define UBOOT_ENV_ADDR  (0xa0000)
#define UBOOT_ENV_SIZE   (0x10000)
#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest));  __asm__("nop");}

static dm_dmh_info_t dmh_info;
typedef unsigned long (*p_entry)(int, char * const[]);
extern unsigned long _do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);

void jump_app(u8 block_id)
{
  u8 *p_data_buffer = NULL;
  dmh_block_info_t block_dm_info = {0};
  u8 *p_data_buffer2 = NULL;
  u8 *p_zip_sys = NULL;
  u32 out_size = 0;


  dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) ;
  OS_PRINTF("block_id %d size 0x%x \n",block_id,block_dm_info.size);

  //dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),0x86,&block_dm_info) ;
  //OS_PRINTF("%s size 0x%x \n",__FUNCTION__,block_dm_info.size);

  p_data_buffer = (u8 *)mtos_malloc(MAINCODE_SIZE);
  if(p_data_buffer == NULL)
  {
  	OS_PRINTF("p_data_buffer malloc fail \n");
	return;
  }
  memset(p_data_buffer,0,MAINCODE_SIZE);
  p_zip_sys = (u8 *)mtos_malloc(320 * KBYTES);
  if(p_zip_sys == NULL)
  {
  	OS_PRINTF("p_zip_sys malloc fail \n");
	return;
  }
  memset(p_zip_sys,0,320 * KBYTES);
  p_data_buffer2 = (u8 *)(MAINCODE_RUN_ADDR);
  memset(p_data_buffer2,0,MAINCODE_SIZE);
  out_size = MAINCODE_OUT_ZIE;

  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, block_dm_info.size, p_data_buffer);

  OS_PRINTF("\nblock_id :%d data :\n",block_id);
  OS_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer[0],p_data_buffer[1],p_data_buffer[2],p_data_buffer[3],p_data_buffer[4]);

  if((p_data_buffer[0] == 0x5d) && (p_data_buffer[1] == 0x00) && (p_data_buffer[2] == 0x00)  && (p_data_buffer[3] == 0x80) )
  {
    init_fake_mem_lzma(p_zip_sys, 320 * KBYTES);
    lzma_decompress((void *)p_data_buffer2, &out_size, p_data_buffer, block_dm_info.size);
    OS_PRINTF("\nblock_id  decompres data :\n");
    OS_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer2[0],p_data_buffer2[1],p_data_buffer2[2],p_data_buffer2[3],p_data_buffer2[4]);
  }
  else
  {
    memcpy((void *)p_data_buffer2,(void *)p_data_buffer,block_dm_info.size);
  }

  hal_dcache_flush((void *)p_data_buffer2, out_size);

  //mtos_task_sleep(10000);
  _do_go_exec((p_entry)MAINCODE_RUN_ADDR,0,0);
  //JUMP(MAINCODE_RUN_ADDR);

}
void spiflash_cfg( )
{
    RET_CODE ret;
    void *p_dev = NULL;
    void *p_spi = NULL;
    charsto_cfg_t charsto_cfg = {0};
    spi_cfg_t spiCfg;

    spiCfg.bus_clk_mhz   = 50;//10;
    spiCfg.bus_clk_delay = 12;//8;
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
    OS_PRINTF("drv --charsto 1\n");

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    OS_PRINTF("drv --charsto 2\n");

    charsto_cfg.rd_mode = SPI_FR_MODE;
    charsto_cfg.p_bus_handle = p_spi;
    charsto_cfg.size = CHARSTO_SIZE;
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
}
void drv_init_concerto(void)
{

  dm_v2_init_para_t dm_para = {0};
  u32 dm_app_addr = 0;

  spiflash_cfg( );

  //init data manager....
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);
  dm_para.use_mutex = TRUE;
  dm_para.mutex_prio = 1;
  dm_para.test_mode = FALSE;

  dm_init_v2(&dm_para);
  // g_dm_head_start = find_dm_head_start();

  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_BOOTER_START_ADDR);


  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_START_ADDR_BAK);

  dm_app_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), DM_MAINCODE_BLOCK_ID)
                   - get_flash_addr();

  dm_set_header(class_get_handle_by_id(DM_CLASS_ID),dm_app_addr);
}

static int check_block_exist_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return TRUE;
			   	   	}
			   	}
			}
	}
  return FALSE;
}

void ota_dm_api_init(void)
{
  ota_dm_config_t p_cfg = {0};
  dm_dmh_info_t *p_all_dmh_info = NULL;
  #if 0
  p_cfg.ota_dm_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_BLOCK_ID) - get_flash_addr();
  p_cfg.ota_dm_backup_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_RESERVE_BLOCK_ID) - get_flash_addr();

  OS_PRINTF("#### debug dm addr:0x%x,dm backup addr:0x%x\n",p_cfg.ota_dm_addr,p_cfg.ota_dm_backup_addr);
  #endif
  p_cfg.is_use_mutex = TRUE;
  p_cfg.mutex_prio = 1;
  p_cfg.is_protect = TRUE;
  p_cfg.disable_backup_block = FALSE;
  p_cfg.debug_level = OTA_DM_DEBUG_ALL;
  p_cfg.ota_dm_api_printf = NULL;
  p_cfg.align_size = PICE_MAX_ALIGN_SIZE;
  p_cfg.flash_size = CHARSTO_SIZE;
  p_cfg.pp_dm_info = &p_all_dmh_info;
  p_cfg.ota_dm_block_id = STATIC_SAVE_DATA_BLOCK_ID;
  p_cfg.ota_dm_backup_block_id =STATIC_SAVE_DATA_RESERVE_BLOCK_ID;

  OS_PRINTF("#####debug ui ota dm api  init\n");
  mul_ota_dm_api_init(&p_cfg);

  /****test use ,please don't use in project,but factory upg*****/
 //mul_ota_dm_api_reset_data_block();
}
void ota_dm_api_bootload_info_init(void)
{
    ota_bl_info_t bl_info = {0};

    bl_info.ota_number = 2;
    bl_info.ota_curr_id = OTA_BLOCK_ID;
    bl_info.ota_backup_id = OTA1_BLOCK_ID;

    bl_info.load_block_id = MAINCODE_BLOCK_ID;

    bl_info.destroy_flag = FALSE;
    bl_info.ota_status = OTA_TRI_MODE_NONE;
    bl_info.fail_times = 0;
    bl_info.medium_type = OTA_MEDIUM_BY_TUNER;
    mul_ota_dm_api_init_bootload_info(&bl_info);
}
void ota_dm_init(void)
{
  ota_dm_api_init();
  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
    ota_dm_api_bootload_info_init();
  }
  memset(&dmh_info,0,sizeof(dm_dmh_info_t));
  mul_ota_dm_api_find_dmh_info_from_flash(0,4 * MBYTES,&dmh_info);
}

void load_jump_policy(void)
{
  u8 jump_block_id = MAINCODE_BLOCK_ID;
  ota_bl_info_t bl_info = {0};

  mul_ota_dm_api_read_bootload_info(&bl_info);
  OS_PRINTF("**************load info***************\n ");
  OS_PRINTF("ota_number:0x%x,  curr_ota_id:0x%x,  back_ota_id:0x%x,\n",bl_info.ota_number,bl_info.ota_curr_id,bl_info.ota_backup_id);
  OS_PRINTF("load_block_id:0x%x,ota_tri:%d\n", bl_info.load_block_id,bl_info.ota_status);
  OS_PRINTF("destroy flag:0x%x,   destroy_block:0x%x\n",bl_info.destroy_flag,bl_info.destroy_block_id);
  OS_PRINTF("**************load info end***************\n");
  /* flash burning is not finished, force ota*/
  if( bl_info.destroy_flag == TRUE)
  {
    if(bl_info.ota_status != OTA_TRI_MODE_FORC)
    {
     bl_info.ota_status = OTA_TRI_MODE_FORC;
      mul_ota_dm_api_save_bootload_info(&bl_info);
    }
   if(((bl_info.destroy_block_id != OTA_BLOCK_ID)
         || (bl_info.destroy_block_id != OTA1_BLOCK_ID))
       && (bl_info.load_block_id == MAINCODE_BLOCK_ID))
      {
         bl_info.load_block_id = bl_info.ota_curr_id;
         mul_ota_dm_api_save_bootload_info(&bl_info);
      }
    else if(((bl_info.destroy_block_id == OTA_BLOCK_ID))
         && (bl_info.load_block_id != MAINCODE_BLOCK_ID)
         && (bl_info.ota_number < 2))
      {
         bl_info.load_block_id = MAINCODE_BLOCK_ID;
         mul_ota_dm_api_save_bootload_info(&bl_info);
      }
    else if((bl_info.ota_number >= 2)
               && (bl_info.destroy_block_id == bl_info.load_block_id))
      {
        if(bl_info.load_block_id == OTA_BLOCK_ID)
          {
            bl_info.load_block_id = OTA1_BLOCK_ID;
          }
        else
          {
            bl_info.load_block_id = OTA_BLOCK_ID;
          }
          mul_ota_dm_api_save_bootload_info(&bl_info);
      }

    OS_PRINTF("it will forc ota by dstroy flag\n");
  }

  if((bl_info.load_block_id == MAINCODE_BLOCK_ID)
     && (check_block_exist_from_file_dm(&dmh_info,MAINCODE_BLOCK_ID) == TRUE))
  {
     jump_block_id = MAINCODE_BLOCK_ID;
  }
  else
  {
     if((bl_info.load_block_id == OTA_BLOCK_ID)
     && (check_block_exist_from_file_dm(&dmh_info,OTA_BLOCK_ID) == TRUE))
      {
        jump_block_id = OTA_BLOCK_ID;
      }
     else if((bl_info.ota_number >=2)
                && (bl_info.load_block_id == OTA1_BLOCK_ID)
                && (check_block_exist_from_file_dm(&dmh_info,OTA1_BLOCK_ID) == TRUE))
      {
         jump_block_id = OTA1_BLOCK_ID;
      }
     else
      {
        jump_block_id = USB_TOOL_BLOCK_ID;
      }
  }

  if(bl_info.load_block_id != jump_block_id)
  {
     if((jump_block_id == OTA_BLOCK_ID)
        ||(jump_block_id == OTA1_BLOCK_ID))
      {
         if(bl_info.ota_status != OTA_TRI_MODE_FORC)
        {
          mul_ota_dm_api_read_bootload_info(&bl_info);
          bl_info.ota_status = OTA_TRI_MODE_FORC;
          bl_info.load_block_id = jump_block_id;
          if(bl_info.ota_number >=2)
            {
              if(bl_info.load_block_id == OTA_BLOCK_ID)
                {
                 bl_info.ota_curr_id = OTA_BLOCK_ID;
                 bl_info.ota_backup_id = OTA1_BLOCK_ID;
                }
              else
                {
                 bl_info.ota_curr_id = OTA1_BLOCK_ID;
                 bl_info.ota_backup_id = OTA_BLOCK_ID;
                }
            }
          else
            {
              bl_info.ota_curr_id = OTA_BLOCK_ID;
              bl_info.ota_backup_id = OTA_BLOCK_ID;
            }

          mul_ota_dm_api_save_bootload_info(&bl_info);
        }
      }
  }

  OS_PRINTF("fast load block id:0x%x\n",jump_block_id);

  jump_app(jump_block_id);
}


int ap_init(void)
{
  mem_cfg(MEMCFG_T_NORMAL);
  drv_init_concerto();

  ota_dm_init();
  load_jump_policy();

  return 0;
}

