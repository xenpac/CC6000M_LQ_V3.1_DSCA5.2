/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

// standard library
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_event.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_otp.h"

#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "hdmi.h"
#include "region.h"
#include "display.h"
#include "pdec.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nit.h"
#include "pat.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#include "dsmcc.h"
//#include "dsmcc_galaxy.h"
//#include "cas_ware.h"
#include "monitor_service.h"
#include "dmx.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "drv_svc.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"

#include "surface.h"


//eva
#include "interface.h"
#include "eva.h"
// ap
#include "ap_framework.h"

#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
//#include "ap_dvbs_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
//#include "ap_cas.h"
//#include "ap_ota.h"
//#include "ap_twin_port.h"

#include "ota_public_def.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "demux_filter.h"
#include "flash_sink_filter.h"
#include "ota_filter.h"
#include "lpower.h"
#include "ota_dm_api.h"

#include "db_dvbs.h"
#include "../../sysstatus/sys_status.h"
#include "spi.h"
#include "mem_cfg.h"
#include "../../includes/customer_config.h"
char *even_mem = 0;
char *odd_mem = 0;

hw_cfg_t hw_cfg = {0};
extern app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id);

extern u8 ui_get_com_num(void);

#define PTI_PARALLEL

u8 ui_get_com_num(void)
{
  return hw_cfg.led_num;
}

#ifdef WIN32
u32 get_flash_addr(void)
{
  extern u8 *p_flash_buf;
  return (u32)p_flash_buf;
}

 inline RET_CODE gpe_vsb_sim_attach_warriors(char *name)
{
    return gpe_soft_attach(name);
}
#endif

static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = SUB_BUFFER_ADDR;
  if(SUB_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_BUFFER_SIZE / 2;
  p_cfg->p_sub_cfg->even_mem_start = p_addr + SUB_BUFFER_SIZE / 2;
  p_cfg->p_sub_cfg->even_mem_end = p_addr + SUB_BUFFER_SIZE;
#endif

  // init osd0 frame buffer
  p_addr = OSD1_BUFFER_ADDR;
  if(OSD1_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + OSD1_BUFFER_SIZE;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + OSD1_BUFFER_SIZE / 2;
  p_cfg->p_osd1_cfg->even_mem_start = p_addr + OSD1_BUFFER_SIZE / 2;
  p_cfg->p_osd1_cfg->even_mem_end = p_addr + OSD1_BUFFER_SIZE;
#endif

  // init osd1 frame buffer
  p_addr = OSD0_BUFFER_ADDR;
  if(OSD0_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + OSD0_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + OSD0_BUFFER_SIZE / 2;
  p_cfg->p_osd0_cfg->even_mem_start = p_addr + OSD0_BUFFER_SIZE / 2;
  p_cfg->p_osd0_cfg->even_mem_end = p_addr + OSD0_BUFFER_SIZE;
#endif

  p_cfg->lock_type = OS_MUTEX_LOCK;
  return TRUE;
}


void uio_init()
{
  RET_CODE ret;
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
  u32 read_len = 0;
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif

  OS_PRINTF("\r\n[OTA]uio attach");
  //uio including irda and frontpanel dev....
   ret = uio_attach_concerto("concerto");
  MT_ASSERT(ret == SUCCESS);

 #if 0
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     HW_CFG_BLOCK_ID, 0, 0,
                     sizeof(hw_cfg_t),
                     (u8 *)&hw_cfg);
 #else
 read_len = mul_ota_dm_api_dm_read(HW_CFG_BLOCK_ID,
                        0,
                       sizeof(hw_cfg_t),
                     (u8 *)&hw_cfg);
 #endif
  MT_ASSERT(read_len == sizeof(hw_cfg_t));

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  fpcfg.p_info = &(hw_cfg.pan_info);
  //fpcfg.p_map = hw_cfg.led_bitmap;
  //fpcfg.map_size = hw_cfg.map_size;
  fpcfg.fp_type = hw_cfg.fp_type;

  if(fpcfg.fp_type != HAL_GPIO)
  {
    fpcfg.p_map = NULL; //650D/1635 use default LED bitmap
  }
  #ifndef WIN32
  // PINMUX
  switch (fpcfg.fp_type)
  {
   case HAL_CT1642:
   case HAL_LM8168:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0x7 << 27);
     val |= 0x5 << 27;
     *((volatile u32 *)reg) = val;

     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFFF << 16);
     val |= 0x444 << 16;
     *((volatile u32 *)reg) = val;
     break;

   case HAL_FD650:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val |= 0x3 << 27;
     *((volatile u32 *)reg) = val;

     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFF << 16);
     val |= 0x33 << 16;
     *((volatile u32 *)reg) = val;

     p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
     MT_ASSERT(NULL != p_dev_i2cfp);
     break;

   default:
     break;
  }
  if (NULL != p_dev_i2cfp)
  {
    fpcfg.p_bus_dev = p_dev_i2cfp;
  }
#endif
  #ifdef SONGXIA_RC
  irdacfg.protocol = 4;
  #else
  irdacfg.protocol = IRDA_NEC;
  #endif

  irdacfg.code_mode = OUR_DEF_MODE;
  irdacfg.irda_repeat_time = 300;

{
  u8 i;
  u8 power_keys[3];
  u8 tmp;
  u16 user_code;
  u8 block_ids[3] = {IRKEY_BLOCK_ID, IRKEY1_BLOCK_ID, IRKEY2_BLOCK_ID};


  irdacfg.irda_wfilt_channel = 3;

  for (i=0; i<3; i++)
  {
    mul_ota_dm_api_dm_read(block_ids[i],
                        1, 1,
                     (u8 *)&tmp);
    user_code = tmp;

    mul_ota_dm_api_dm_read(block_ids[i],
                      0, 1,
                   (u8 *)&tmp);
    user_code |= ((u16)tmp) << 8;

    mul_ota_dm_api_dm_read(block_ids[i],
                  2, 1,
               (u8 *)&power_keys[i]);

    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= ((u32)user_code) << 16;
    irdacfg.irda_wfilt_channel_cfg[i].protocol = IRDA_NEC;
    irdacfg.irda_wfilt_channel_cfg[i].addr_len = 32;
    tmp = ~power_keys[i];
    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= tmp;
    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= ((u16)power_keys[i]) << 8;

    OS_PRINTF("wavefilter code 0x%x\n", irdacfg.irda_wfilt_channel_cfg[i].wfilt_code);
  }
}
 #ifdef SONGXIA_RC
 irdacfg.irda_wfilt_channel_cfg[0].protocol = 4;
 irdacfg.irda_wfilt_channel_cfg[0].addr_len = 16;
 irdacfg.irda_wfilt_channel_cfg[0].wfilt_code = 0x5750;
#endif

  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);

  if(hw_cfg.led_num == 4)
  {
    uio_display(p_dev, " ON ", 4);
  }
  else if(hw_cfg.led_num == 3)
  {
    uio_display(p_dev, "ON ", 3);
  }
  else if(hw_cfg.led_num == 2)
  {
    uio_display(p_dev, "ON ", 2);
  }
  else if(hw_cfg.led_num == 1)
  {
    uio_display(p_dev, "O ", 1);
  }

  dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);

    switch (fpcfg.fp_type)
  {
    case HAL_CT1642:
      standby_cpu_attach_fw_ct1642_concerto();
      break;
    case HAL_FD650:
      standby_cpu_attach_fw_fd650_concerto();
      break;
    default :
      standby_cpu_attach_fw_fd650_concerto();
      break;
  }

}

static void hdmi_driver_attach(void* p_drvsvc)
{
  RET_CODE ret;
  u32 reg= 0, val = 0;
  hdmi_cfg_t hdmi_cfg = {0};
  void * p_i2c_master = NULL;
  drv_dev_t *p_hdmi_dev = NULL;

  ret = hdmi_anx8560_attach("hdmi");
  MT_ASSERT(ret == SUCCESS);

  p_hdmi_dev = dev_find_identifier(NULL,
                              DEV_IDT_TYPE,
                              SYS_DEV_TYPE_HDMI);
  MT_ASSERT(p_hdmi_dev != NULL);

  // PINMUX
  reg = R_PIN3_SEL;
  val = *((volatile u32 *)reg);
  val &= ~(0xFF << 16);
  *((volatile u32 *)reg) = val;

  if (SUCCESS != hal_module_clk_get(HAL_HD_VIDEO, &val))
  {
    OS_PRINTF("HAL_HD_VIDEO mclk: 74.25MHz\n");
    hal_module_clk_set(HAL_HD_VIDEO, 74250000);
  }

  p_i2c_master = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
  MT_ASSERT(NULL != p_i2c_master);

  memset(&hdmi_cfg, 0x0, sizeof(hdmi_cfg_t));
  if(NULL == p_drvsvc)
  {
  hdmi_cfg.task_prio = DRV_HDMI_TASK_PRIORITY;
  hdmi_cfg.stack_size = (8 * KBYTES);
  }
  else
  {
    hdmi_cfg.p_drvsvc = p_drvsvc;
  }
  hdmi_cfg.i2c_master = p_i2c_master;
  hdmi_cfg.is_initialized = 0;
  hdmi_cfg.is_fastlogo_mode = 0;
//#ifdef CHIP
  ret = dev_open(p_hdmi_dev, &hdmi_cfg);
  MT_ASSERT(SUCCESS == ret);
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
    charsto_cfg.size = 8*1024*1024;
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);

}


static void disp_vdac_config(void * p_disp)
{
  vdac_info_t v_dac_info = hw_cfg.vdac_info;

  OS_PRINTF("[disp_vdac_config] v_dav_info = %d \n",v_dac_info);

  switch(v_dac_info)
  {
    case VDAC_CVBS_RGB:
    {
      OS_PRINTF("\nVDAC_CVBS_RGB\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_RGB);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, FALSE);
      //disp_component_set_type(p_disp, COMPONENT_GRP0,COLOR_CVBS_RGB);
      //disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;

    case VDAC_SIGN_CVBS:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS\n");
      disp_set_dacmode(p_disp,DISP_DAC_SING_CVBS);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
      //disp_dac_onoff(p_disp, DAC_1,  FALSE);
      //disp_dac_onoff(p_disp, DAC_2,  FALSE);

    }
    break;

    case VDAC_SIGN_CVBS_LOW_POWER:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS_LOW_POWER\n");
      disp_set_dacmode(p_disp,DISP_DAC_SING_CVBS_LOW_POWER);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
      //disp_dac_onoff(p_disp, DAC_0,  FALSE);
      //disp_dac_onoff(p_disp, DAC_1,  FALSE);
      //disp_dac_onoff(p_disp, DAC_2,  FALSE);
    }
    break;

    case VDAC_DUAL_CVBS:
    {
      OS_PRINTF("\nVDAC_DUAL_CVBS\n");
      disp_set_dacmode(p_disp,DISP_DAC_DULE_CVBS);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP1, FALSE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP2, FALSE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;

    case VDAC_CVBS_SVIDEO:
    {
      OS_PRINTF("\nVDAC_CVBS_SVIDEO\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_SVIDEO);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_svideo_onoff(p_disp, SVIDEO_GRP0, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP2, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;

    case VDAC_CVBS_YPBPR_SD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_SD\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_SD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP0, COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;

    case VDAC_CVBS_YPBPR_HD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_HD\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_HD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;

    default:
    {
      OS_PRINTF("\nVDAC setting default, CVBS + HD COMPONENT \n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_HD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;
  }
}


static int drv_display_init(void)
{
  void *p_disp = NULL;
  disp_cfg_t disp_cfg = {0};
  layer_cfg_t osd0_vs_cfg = {0};
  layer_cfg_t osd1_vs_cfg = {0};
  layer_cfg_t sub_vs_cfg = {0};
  layer_cfg_t osd0_cfg = {0};
  layer_cfg_t osd1_cfg = {0};
  layer_cfg_t sub_cfg = {0};
  rect_size_t disp_rect;
  RET_CODE ret;

  //display driver attach
  /*lint -e123 */
  ret = ATTACH_DRIVER(DISP, concerto, default, default);
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);
  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);
  //adjust these in future

  OS_PRINTF("\ndisp_concerto_attach ok\n");
  disp_cfg.p_sub_cfg = &sub_cfg;
  disp_cfg.p_osd0_cfg = &osd0_cfg;
  disp_cfg.p_osd1_cfg = &osd1_cfg;
  disp_cfg.p_still_sd_cfg = NULL;

  disp_cfg.b_osd_vscle = TRUE;
  disp_cfg.b_osd_hscle = TRUE;
  disp_cfg.b_di_en = TRUE;
  disp_cfg.b_vscale = TRUE;
  disp_cfg.b_vscale_osd0 = TRUE;
  disp_cfg.b_vscale_osd1 = TRUE;
  disp_cfg.b_vscale_sub = TRUE;
  disp_cfg.b_wrback_422 = TRUE;
  disp_cfg.b_unuse_prescale = FALSE;
  disp_cfg.misc_buf_cfg.sd_wb_addr = VID_SD_WR_BACK_ADDR;
  disp_cfg.misc_buf_cfg.sd_wb_size = VID_SD_WR_BACK_SIZE;
  disp_cfg.misc_buf_cfg.sd_wb_field_no = VID_SD_WR_BACK_FIELD_NO;
  //memset((void *)(VID_SD_WR_BACK_ADDR | 0xa0000000), 0, VID_SD_WR_BACK_SIZE);
  //disp_cfg.av_ap_shared_mem = AP_AV_SHARE_MEM_ADDR;
  //disp_cfg.shared_mem_size = AP_AV_SHARE_MEM_SIZE;

  if(TRUE == disp_cfg.b_vscale)
  {
    if(disp_cfg.b_vscale_osd1 == TRUE)
    {
      osd1_vs_cfg.odd_mem_start = OSD1_VSCALER_BUF_ADDR;
      osd1_vs_cfg.odd_mem_end = OSD1_VSCALER_BUF_ADDR + OSD1_VSCALER_BUF_SIZE;
    }
    if(disp_cfg.b_vscale_sub == TRUE)
    {
      sub_vs_cfg.odd_mem_start = SUB_VSCALER_BUF_ADDR;
      sub_vs_cfg.odd_mem_end = SUB_VSCALER_BUF_ADDR + SUB_VSCALER_BUF_SIZE;
    }
    disp_cfg.p_osd0_vscale_cfg = &osd0_vs_cfg;
    disp_cfg.p_osd1_vscale_cfg = &osd1_vs_cfg;
    disp_cfg.p_sub_vscale_cfg = &sub_vs_cfg;
  }
  if(TRUE == disp_cfg.b_di_en)
  {
    disp_cfg.misc_buf_cfg.di_addr = VID_DI_CFG_ADDR;
    disp_cfg.misc_buf_cfg.di_size = VID_DI_CFG_SIZE;
  }
  disp_cfg.stack_size = DISP_HDMI_NOTIFY_TASK_STK_SIZE;
  disp_cfg.task_prio = DISP_HDMI_NOTIFY_TASK_PRIORITY;
  disp_cfg.lock_type = OS_MUTEX_LOCK;

  mtos_printk("\ndisp_open begin\n");
  disp_cfg.b_uboot_uninit = TRUE;
  if(init_display_cfg(&disp_cfg))
  {
    ret = dev_open(p_disp, &disp_cfg);
  }
  else
  {
    ret = dev_open(p_disp, NULL);
  }
  MT_ASSERT(SUCCESS == ret);

  mtos_printk("\ndisp_open ok\n");
  //minnan add cfg vdac

#ifndef WIN32

  disp_vdac_config(p_disp);

  disp_rect.w = 1280;
  disp_rect.h = 720;
  disp_set_graphic_size(p_disp,&disp_rect);
  mtos_printk("disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);
#endif

  return ret;
}


extern u32 get_flash_addr(void);
extern void ui_ota_dm_api_init(void);
#ifdef MXL241
extern RET_CODE nim_mxl241_attach(char *name);
#endif
static void drv_init_concerto(void)
{
  RET_CODE ret;
  void *p_dev = NULL;
  void *p_dev_i2c0 = NULL;
  void *p_dev_i2c1 = NULL;
  void *p_dev_i2cfp = NULL;
  void *p_dev_i2chdmi = NULL;
  void *p_dev_i2cqam = NULL;
  drvsvc_handle_t *p_public_drvsvc = NULL;
  u32 *p_buf = NULL;

  //this service will be shared by HDMI, Audio and Display driver
  p_buf = (u32 *)mtos_malloc(DRV_HDMI_TASK_STKSIZE);
  MT_ASSERT(NULL != p_buf);
  p_public_drvsvc = drvsvc_create(DRV_HDMI_TASK_PRIORITY,
    p_buf, DRV_HDMI_TASK_STKSIZE, 8);
  MT_ASSERT(NULL != p_public_drvsvc);

  #ifndef WIN32
  hal_otp_init();
#endif

  /* DMA */
  ret = hal_dma_init();
  MT_ASSERT(ret == SUCCESS);

#if 1
  /* GPE */
  OS_PRINTF("-------------- GPE_USE_HW  \n");
  void *p_gpe_c = NULL;
  ret = cct_gpe_attach("cct_gpe");
  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(TRUE == ret);
#else // soft
  /* GPE */
  void *p_gpe_c = NULL;
  ret = gpe_soft_attach("gpe_soft");
  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(SUCCESS == ret);
#endif

  {
    /* I2C 0 */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c0");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2c0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
    MT_ASSERT(NULL != p_dev_i2c0);
    i2c_cfg.i2c_id = 0;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2c0, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }
  {
    /* I2C 1 */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c1");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2c1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c1");
    MT_ASSERT(NULL != p_dev_i2c1);
    i2c_cfg.i2c_id = 1;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2c1, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  {
    /* I2C FP */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_FP");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
    MT_ASSERT(NULL != p_dev_i2cfp);
    i2c_cfg.i2c_id = 2;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2cfp, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  {
    /* I2C HDMI */
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_HDMI");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2chdmi = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
    MT_ASSERT(NULL != p_dev_i2chdmi);
    i2c_cfg.i2c_id = 3;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2chdmi, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
  }

  {
    /* I2C QAM */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_QAM");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2cqam = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");
    MT_ASSERT(NULL != p_dev_i2cqam);
    i2c_cfg.i2c_id = 4;
    i2c_cfg.bus_clk_khz = 400;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2cqam, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }
#ifdef DTMB_PROJECT
  {
    /*Reset Demod*/
    s32 ret = gpio_io_enable(64, TRUE);
    gpio_set_dir(64, GPIO_DIR_OUTPUT);
    gpio_set_value(64, GPIO_LEVEL_LOW);
    mtos_task_delay_ms(100);
    gpio_set_value(64, GPIO_LEVEL_HIGH);
    /*Init Demod & Tuner*/
    extern RET_CODE nim_m88dd3k_attach(char *name);
    nim_config_t nim_cfg = {0};
    ret = nim_m88dd3k_attach("nim_dd3k");
    MT_ASSERT(ret == SUCCESS);
    p_dev = dev_find_identifier(p_dev, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);
    nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
    MT_ASSERT(NULL != nim_cfg.p_dem_bus);
    nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
    nim_cfg.tn_version = TC3800;
    ret = dev_open(p_dev, &nim_cfg);
    MT_ASSERT(SUCCESS == ret);
  }
#elif defined(MXL241)
  {
    /* NIM */
    nim_config_t nim_cfg = {0};
    u32 temp = 0;
    p_dev_i2c0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");

    temp = hal_get_u32((volatile unsigned long *)0xbf156014);
    temp &= ~(7 << 12);
    temp &= ~(7 << 8);
    hal_put_u32((volatile unsigned long *)0xbf156014, temp);

    temp = hal_get_u32((volatile unsigned long *)0xbf156304);
    temp |= 3 <<  18;
    hal_put_u32((volatile unsigned long *)0xbf156304, temp);

    temp = hal_get_u32((volatile unsigned long *)0xbf15601c);
    temp &= ~(0xff);
    hal_put_u32((volatile unsigned long *)0xbf15601c, temp);

    temp = hal_get_u32((volatile unsigned long *)0xbf156308);
    temp |= (0xf << 24);
    hal_put_u32((volatile unsigned long *)0xbf156308, temp);

    ret = nim_mxl241_attach("nim");
    MT_ASSERT(ret == SUCCESS);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);

     nim_cfg.p_dem_bus = p_dev_i2c0;
     nim_cfg.tuner_bandwidth = 6;
     nim_cfg.lock_mode = OS_MUTEX_LOCK;
    OS_PRINTF("nim1=0x%x\n", (u32)p_dev);
    ret = dev_open(p_dev, &nim_cfg);
    MT_ASSERT(SUCCESS == ret);
#ifdef J83A
    ret = dev_io_ctrl(p_dev, NIM_IOCTRL_SWITCH_J83B_J83A, 1);
    MT_ASSERT(SUCCESS == ret);
#endif
  }
#else
  extern void tuner_attach_tc2800();

  nim_config_t nim_cfg = {0};
  ret = nim_attach_concerto("nim");
  MT_ASSERT(ret == SUCCESS);
  tuner_attach_tc2800();

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_dev);
  nim_cfg.p_dem_bus = p_dev_i2cqam;
  nim_cfg.lock_mode = OS_LOCK_DEF;
#ifndef PTI_PARALLEL
  nim_cfg.dem_addr = 0x3a;
  nim_cfg.tun_addr = 0xc2;
  nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
#else
  nim_cfg.dem_addr = 0x38;
  nim_cfg.tun_addr = 0xc2;
  nim_cfg.ts_mode = NIM_TS_INTF_PARALLEL;
#endif

  nim_cfg.dem_ver = DEM_VER_3;

  ret = dev_open(p_dev, &nim_cfg);

  MT_ASSERT(SUCCESS == ret);

  dev_io_ctrl(p_dev, NIM_IOCTRL_SET_TUNER_WAKEUP, 0);

#endif


  spiflash_cfg( );

  ui_ota_dm_api_init();

  /* open uio */
  uio_init();

    /* HDMI */
  hdmi_driver_attach((void *)p_public_drvsvc);

{
    /*DMX PTI*/
#if 1
#ifndef WIN32
#ifdef DTMB_PROJECT
    *(volatile unsigned long *)0xbf156014 = 0xa0000;
    *(volatile unsigned long *)0xbf15601c = 0x111100;
    *(volatile unsigned long *)0xbf156308 = 0xfff80000;
#elif defined(MXL241)
  *(volatile unsigned long *)0xbf156014 = 0xa0000;
#else
    *(volatile unsigned long *)0xbf156014 = 0xa0000;
    *(volatile unsigned long *)0xbf15601c = 0x1111a5;
    *(volatile unsigned long *)0xbf156308 = 0xfcf80000;
#endif
#endif
    void *p_pti0 = NULL;
    void *p_pti1 = NULL;
    dmx_config_t dmx_cfg0 = {0};
    dmx_config_t dmx_cfg1 = {0};
    /*************PTI0 attatch************/
    dmx_concerto_attach("concerto_pti0");
    p_pti0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti0");
    MT_ASSERT(NULL != p_pti0);

#ifdef DTMB_PROJECT
    dmx_cfg0.input_port_used[1] = TRUE;
    dmx_cfg0.ts_input_cfg[1].input_way = 0;
    dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
    dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
    dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
#elif defined(MXL241)
    dmx_cfg0.input_port_used[1] = TRUE;
    dmx_cfg0.ts_input_cfg[1].input_way = 0;
    dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
    dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
    dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
#else
    dmx_cfg0.input_port_used[0] = TRUE;
    dmx_cfg0.ts_input_cfg[0].input_way = 1;
    dmx_cfg0.ts_input_cfg[0].local_sel_edge = 1;
    dmx_cfg0.ts_input_cfg[0].error_indicator = 0;
    dmx_cfg0.ts_input_cfg[0].start_byte_mask = 0;
#endif

#ifdef AV_SYNC_FLAG_ON
	 dmx_cfg0.av_sync = TRUE;
#else
	 dmx_cfg0.av_sync = FALSE;
#endif
      dmx_cfg0.pool_size = 512*188;
      dmx_cfg0.pool_mode = 0;
      dmx_cfg0.pti_id = 0;
      ret = dev_open(p_pti0, &dmx_cfg0);
      MT_ASSERT(SUCCESS == ret);
      /*************PTI1 attatch************/
      dmx_concerto_attach("concerto_pti1");
      p_pti1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
      MT_ASSERT(NULL != p_pti1);

      dmx_cfg1.input_port_used[0] = TRUE;
      dmx_cfg1.ts_input_cfg[0].input_way = 1;
      dmx_cfg1.ts_input_cfg[0].local_sel_edge = 1;
      dmx_cfg1.ts_input_cfg[0].error_indicator = 0;
      dmx_cfg1.ts_input_cfg[0].start_byte_mask = 0;

#ifdef AV_SYNC_FLAG_ON
     dmx_cfg1.av_sync = TRUE;
#else
     dmx_cfg1.av_sync = FALSE;
#endif

    dmx_cfg1.pool_size = 512*188;
    dmx_cfg1.pool_mode = 0;
    dmx_cfg1.pti_id = 1;
    ret = dev_open(p_pti1, &dmx_cfg1);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  drv_display_init();
  mtos_task_sleep(1500);
}

 void middleware_init(void)
{
  eva_init_para_t eva_para = {0};
  mdl_init();
  eva_para.debug_level = E_PRINT_ERROR;
  //eva_para.eva_sys_task_prio = EVA_SYS_TASK_PRIORITY;
  eva_para.eva_sys_task_prio_start = EVA_SYS_TASK_PRIORITY_START;
  eva_para.eva_sys_task_prio_end = EVA_SYS_TASK_PRIORITY_END;
  eva_init(&eva_para);

  eva_filter_factory_init();
  eva_register_filter(DEMUX_FILTER, dmx_filter_create);
  eva_register_filter(OTA_FILTER, ota_filter_create);
  //eva_register_filter(FLASH_SINK_FILTER,flash_sink_filter_create);

#if 0
  {
    nc_svc_cfg_t nc_cfg = {0};
    nc_cfg.b_sync_lock = FALSE;
    nc_cfg.lock_mode = SYS_DVBC;
    nc_cfg.priority = MDL_NIM_CTRL_TASK_PRIORITY;
    nc_cfg.stack_size = MDL_NIM_CTRL_TASK_STKSIZE;
    nc_cfg.auto_iq = TRUE;
    nc_svc_init(&nc_cfg, NULL);

  }
#endif
}

#define INIT_APP_INFO(info, id, name, instance, task_prio, task_stack) \
  { \
    info.app_id = id; \
    info.p_name = name; \
    info.p_instance = instance; \
    info.priority = task_prio; \
    info.stack_size = task_stack; \
  }

extern ap_frm_policy_t *construct_ap_frm_policy(void);
#ifdef SONGXIA_RC
extern app_t *construct_ap_songxia_uio(void);
#else
extern ap_uio_policy_t *construct_ap_uio_policy(void);
#endif


//extern ota_policy_t *construct_ota_policy(void);

 void app_init(void)
{
  ap_frm_init_param_t param = {0};
  handle_t ap_frm_handle = NULL;
  u32 ap_frm_msgq_id = 0;

  app_info_t app_array[] =
  {
  //  {APP_OTA, construct_ap_ota(construct_ota_policy()), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_OTA, construct_virtual_nim_filter(APP_OTA,0), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
     #ifdef SONGXIA_RC
      {APP_UIO, construct_ap_songxia_uio(), "app uio", AP_UIO_TASK_PRIORITY, AP_UIO_TASK_STKSIZE},
     #else
      {APP_UIO, construct_ap_uio(construct_ap_uio_policy()), "app uio", AP_UIO_TASK_PRIORITY,AP_UIO_TASK_STKSIZE},
    #endif
  };

  param.p_implement_policy = construct_ap_frm_policy();
  param.p_app_array = app_array;
  param.num_of_apps = sizeof(app_array)/ sizeof(app_info_t);
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;
  //param.print_level = AP_FRM_PRINT_ALL;
  ap_frm_handle = ap_frm_init(&param, &ap_frm_msgq_id);

  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
}

 void util_init(void)
{
  simple_queue_init();
}

extern void ui_init(void);

void ap_init(void)
{

  mem_cfg(MEMCFG_T_NORMAL);
  OS_PRINTF("\r\n[OTA] mem cfg ok ");

  util_init();

  drv_init_concerto();
  middleware_init();

  app_init();
  OS_PRINTF("\r\n[OTA] app init ok ");

  ui_init();
}

