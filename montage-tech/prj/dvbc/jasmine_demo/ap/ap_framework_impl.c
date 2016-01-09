/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "nit.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "db_dvbs.h"
#include "mt_time.h"
#include "audio.h"
#include "video.h"

#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_flounder_ota.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "sys_status.h"
#include "i2c.h"
#include "hal_base.h"

#include "lib_util.h"
#include "lpower.h"

#include "customer_config.h"

static void ap_proc(void)
{
}


static void ap_restore_to_factory(void)
{
 #if ((!defined WIN32) )
#endif

}


BOOL ap_get_standby(void)
{
#if ((!defined WIN32) )
return FALSE;
#endif

}


void ap_set_standby(u32 flag)
{
  #if ((!defined WIN32) )
 #endif

}

#define R_FPGPIO_OUT_EN_REG 0xbfedc028
#define R_FPGPIO_OUT_REG 0xbfedc02c
#define R_FPGPIO_MASK_REG 0xbfedc034
#define R_FPGPIO_RESET_REG 0xbfedc038

void ap_enter_tkgs_standby()
{
    audio_device_t *p_audio_dev = NULL;
    video_device_t *p_video_dev = NULL;
    void * pDev = NULL;

#ifdef SCART_ENABLE   
      avc_cfg_scart_select_tv_master(class_get_handle_by_id(AVC_CLASS_ID), SCART_TERM_VCR);
      avc_cfg_scart_vcr_input(class_get_handle_by_id(AVC_CLASS_ID), SCART_TERM_TV);
#endif

    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    MT_ASSERT(NULL != p_video_dev);
    vdec_stop(p_video_dev);

    p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    MT_ASSERT(NULL != p_audio_dev);
    aud_mute_onoff_vsb(p_audio_dev,TRUE);

    pDev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != pDev);
    disp_cvbs_onoff(pDev, CVBS_GRP0,  FALSE);

    if(disp_layer_is_show(pDev, DISP_LAYER_ID_OSD0))
    disp_layer_show(pDev, DISP_LAYER_ID_OSD0, FALSE);
    if(disp_layer_is_show(pDev, DISP_LAYER_ID_OSD1))
    disp_layer_show(pDev, DISP_LAYER_ID_OSD1, FALSE);
        
    pDev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_POW);
    MT_ASSERT(NULL != pDev);
    dev_close(pDev);
    
    mtos_close_printk();
    mtos_task_delay_ms(100);
      
}

void ap_enter_standby(u32 tm_out)
{
#ifndef WIN32    
    hw_cfg_t hw_cfg = dm_get_hw_cfg_info();
    static  void *p_dev_sty = NULL;
    void *p_dev_uio = NULL;
    void *p_dev_nim = NULL;
    RET_CODE ret = SUCCESS;
    utc_time_t  curn_time,p_time;
    u32 cmd = 0;
    concerto_standby_config_t info;

    p_dev_sty = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_POW);
    MT_ASSERT(NULL != p_dev_sty);
    p_dev_uio = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev_uio);
    p_dev_nim = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev_nim);


    memset(&info, 0, sizeof(info));
    /*        set standby time       */
    time_get(&curn_time, FALSE);

    /* select the correct FP */
    switch ((hal_fp_type_t)hw_cfg.fp_type)
    {
      case HAL_FD650:
        info.panel_config.panel_type = FD650;
         break;
      case HAL_CT1642:
      default:
        info.panel_config.panel_type = CT1642;
        break;
    }

    if(DISP_TIME == hw_cfg.fp_disp && hw_cfg.led_num >= 4)
    {
      info.panel_config.time_enable = 1;
      info.panel_config.time_hh = curn_time.hour;
      info.panel_config.time_mm = curn_time.minute;
      OS_PRINTF("Standby time %d:%d\n", curn_time.hour, curn_time.minute);
      #ifdef TONGJIU
      info.panel_config.led_dot_mask = 0x1;
      #else
      if(g_customer.customer_id == CUSTOMER_KINGVON) 
      {
        info.panel_config.led_dot_mask = 0x1;
      }
      #endif
    }
    else
    {
      info.panel_config.led_chars_enable = 1;
      if(g_customer.customer_id != CUSTOMER_KINGVON && g_customer.customer_id != CUSTOMER_PLAAS ) 
      {
        info.panel_config.led_dot_mask = 0;
        info.panel_config.led_chars[3] = ' '; 
        info.panel_config.led_chars[2] = ' ';
        info.panel_config.led_chars[1] = ' ';
        info.panel_config.led_chars[0] = ' ';
      }
      else
      {
        info.panel_config.led_dot_mask = 0x1;
        info.panel_config.led_chars[3] = ' '; 
        info.panel_config.led_chars[2] = 'F';
        info.panel_config.led_chars[1] = 'F';
        info.panel_config.led_chars[0] = 'O';
      }
    }
		
  if(g_customer.customer_id == CUSTOMER_JIULIAN)
 {
    info.panel_config.led_dot_mask = 3;
    info.panel_config.led_chars[3] = ' '; 
    info.panel_config.led_chars[2] = ' ';
    info.panel_config.led_chars[1] = ' ';
    info.panel_config.led_chars[0] = ' ';
  }

    sys_status_set_status(BS_UNFIRST_UNPOWER_DOWN,TRUE);
    time_to_gmt(&curn_time, &p_time);
    p_time.second = 0;
    sys_status_set_utc_time(&p_time);
    
    info.panel_config.wakeup_enable = 1;
    info.ir_config.wakeup_enable = 1;
    
    if(g_customer.customer_id == CUSTOMER_NEWSTART)
    {
      //OS_PRINTF("Entry real standby !\n");
      uio_display(p_dev_uio, (u8*)"   ", 3);
      info.panel_config.time_enable = 0;
      info.panel_config.led_chars_enable = 0;
      info.panel_config.wakeup_enable = 0;
    }

     /* get FP POWER key from DM */ 
    dm_read(class_get_handle_by_id(DM_CLASS_ID), 
            FPKEY_BLOCK_ID, 0, 0, 
            sizeof(u8), 
            (u8*)&info.panel_config.wakeup_key);
    if(g_customer.customer_id != CUSTOMER_AISAT ||
	g_customer.customer_id != CUSTOMER_AISAT_HOTEL ||
	g_customer.customer_id != CUSTOMER_AISAT_DIVI ||
	g_customer.customer_id != CUSTOMER_AISAT_DEMO) 
    {
      info.panel_config.wakeup_key |= 0x100;
    }
    if(g_customer.customer_id == CUSTOMER_NEWSTART ||
      g_customer.customer_id == CUSTOMER_KONKA)  //for newstart second board need debug ?
    {
      info.gpio_config.wakeup_source0_enable = 1;
      info.gpio_config.source0_edge = 1;
    }
	
    if(g_customer.customer_id == CUSTOMER_ORRISDIGITAL ||
	g_customer.customer_id == CUSTOMER_JIZHONG)
    {
   	info.gpio_config.wakeup_source1_enable = 1;
   	info.gpio_config.source1_edge = 0;
    }
    
    if(g_customer.customer_id == CUSTOMER_KONKA)
    {
     	info.gpio_config.wakeup_source0_enable = 1;
     	info.gpio_config.source0_edge = 1;
    }

    if(g_customer.customer_id == CUSTOMER_NEXTBITNETWORK)
    {
     	info.gpio_config.wakeup_nomal_enable = 1;
     	info.gpio_config.normal_gpio_num = 31;
    }
    
    /*        time wake up AP       */
    if(tm_out > 0)
    {
      info.timer_config.wakeup_enable = 1;
      info.timer_config.wakeup_duration = tm_out;
    }
    else
    {
      //Nothing to do yet.
    }

    /*set nim to sleep*/
    dev_io_ctrl(p_dev_nim, (u32)NIM_IOCTRL_SET_TUNER_SLEEP, 0);

    /* To avoid noise */
    gpio_io_enable(62, TRUE);
    gpio_set_dir(62, GPIO_DIR_OUTPUT);
    #ifdef TONGJIU
    gpio_set_value(62, GPIO_LEVEL_HIGH);
    #else
    gpio_set_value(62, GPIO_LEVEL_LOW);
    #endif

    cmd = SET_STANDBY_CONFIG;
    ret = ap_lpower_ioctl(p_dev_sty, cmd, (u32)&info);
    MT_ASSERT(ret == SUCCESS);

    /* Set wavefilter */
    dev_io_ctrl(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO), 
                (u32)UIO_IR_SET_WAVEFILT, 
                0);
    //set power led
    if(g_customer.cus_power_led_set)
    {
      g_customer.cus_power_led_set();
    }

    //disable watchdog before enter standby
    hal_watchdog_disable();

    /*        enter standby       */
    ap_lpower_enter(p_dev_sty);
#endif
   //lint -e438 -e550
}
//lint +e438 +e550

void ap_test_uart(void)
{
}


static BOOL ap_consume_ui_event(u32 ui_state, os_msg_t *p_msg)
{
  return FALSE;
}


/*!
   Construct APP. framework policy
  */
ap_frm_policy_t *construct_ap_frm_policy(void)
{
  ap_frm_policy_t *p_policy = mtos_malloc(sizeof(ap_frm_policy_t));
  MT_ASSERT(p_policy != NULL);
  //lint -e668 to avoid pclint check error
  memset(p_policy, 0, sizeof(ap_frm_policy_t));
  //lint +e668
  
  p_policy->enter_standby = ap_enter_standby;
  p_policy->extand_proc = ap_proc;
  p_policy->is_standby = ap_get_standby;
  p_policy->resotre_to_factory = ap_restore_to_factory;
  p_policy->set_standby = ap_set_standby;
  p_policy->test_uart = ap_test_uart;
  p_policy->consume_ui_event = ap_consume_ui_event;


  return p_policy;
}
