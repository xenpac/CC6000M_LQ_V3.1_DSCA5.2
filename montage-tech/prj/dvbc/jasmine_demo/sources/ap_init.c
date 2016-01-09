/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
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
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_int.h"
// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
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
#include "avsync.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"
#include "nim.h"
#include "dmx.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "mtos_event.h"
#include "list.h"
#include "block.h"
#ifdef WIN32
//#include "fsioctl.h"
//#include "ufs.h"
#endif
#include "vfs.h"
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
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"
#include "vfs.h"
#include "avctrl1.h"
#include "vbi_api.h"
#include "hal_secure.h"
#ifndef WIN32
#include "sctrl.h"
#endif
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
#include "surface.h"
#include "db_dvbs.h"
#include "mem_cfg.h"
#include "pnp_service.h"
#include "media_data.h"
#include "cas_ware.h"
#include "dsmcc.h"
#include "monitor_service.h"
#include "ota_public_def.h"
#include "ota_dm_api.h"
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
#include "file_source_filter.h"
#include "ts_player_filter.h"
#include "av_render_filter.h"
#include "demux_filter.h"
#include "record_filter.h"
#include "file_sink_filter.h"
#include "pic_filter.h"
#include "pic_render_filter.h"
#include "mp3_player_filter.h"
#include "lrc_filter.h"
#include "str_filter.h"
#include "flash_sink_filter.h"
#ifdef ENABLE_FILE_PLAY
#include "file_playback_sequence.h"
#include "mplayer_aud_filter.h"
#endif
#include "ota_filter.h"
#include "file_list.h"
#include "dmx_manager.h"
#include "epg_type.h"
//#include "epg_filter.h"
// ap
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
//#include "ap_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#include "media_data.h"
#include "ap_usb_upgrade.h"
#include "pvr_api.h"
#include "LzmaIf.h"
#ifndef WIN32
#include "app_demux.h"
#endif
#include "ui_systatus_api.h"
//customer define
#include "customer_config.h"
#include "lpower.h"
#include "ui_util_api.h"
#include "ui_usb_api.h"
#include "sys_status.h"
#include "subt_station_filter.h"
#include "user_parse_table.h"
#include "sys_app.h"
#ifdef NVOD_ENABLE
#include "nvod_data.h"
#include "ap_nvod.h"
#endif

#ifdef BROWSER_APP
#include "browser_adapter.h"
#include "ap_browser.h"
#endif

#if defined DESAI_56_CA || defined DESAI_52_CA || defined DESAI_40_CA
#include "ap_nit.h"
#endif
#include "iconv_ext.h"

#include "spi.h"
#ifndef WIN32
//#include "common_api.h"
#endif

#ifdef ENABLE_CA
#include "config_cas.h"
#include "ui_ca_api.h"
#endif

#ifdef ENABLE_ADS
#include "config_ads.h"
#endif

#ifdef TENGRUI_ROLLTITLE
#include "ap_tr_subt.h"
#endif

#ifndef WIN32
#include "emon_cmd.h"
#endif
#ifdef UPDATE_PG_BACKGROUND
#include "ap_table_monitor.h"
#endif
#ifdef CHANNEL_FOLLOWING_TABLE
#include "ap_table_parse.h"
#endif
#ifdef ENABLE_NETWORK
#include "ethernet.h"
#include "wifi.h"
#include "modem.h"
#ifndef WIN32
#include "ppp.h"
#endif

#include "register_net_stream.h"
#include "network_monitor.h"
#include "rc_xml.h"
#include "ap_rc.h"
#include "httpd_svc.h"
#include "media_ip_filter.h"
#include "media_ip_api.h"
#include "ap_cst_m.h"

#include "vod_public.h"
#include "google_map_filter.h"
#include "weather_filter.h"
#include "net_source_filter.h"
#include "vod_filter.h"
#include "net_music_filter.h"
#include "news_filter.h"
#include "net_upg_filter.h"
#include "flash_sink_filter.h"
#include "albums_public.h"
#include "albums_filter.h"

#include "vod_api.h"
#include "albums_api.h"
#include "news_api.h"
#include "net_upg_api.h"
#include "livetv_db.h"
#include "net_svc.h"
#include "albums_public.h"
#include "albums_api.h"
#include "net_music_api.h"
#include "music_api_net.h"
#include "music_api.h"
#include "commonData.h"


extern int lwip_stack_init(ethernet_cfg_t *p_cfg);
extern  u8 mount_ramfs();
extern RET_CODE rtl8188eus_ethernet_attach(char *p_name);
extern int cmm_wifi_attach(char *devname);
extern RET_CODE  usb_tty_attach(char *p_name);
extern RET_CODE modem_usb_3g_dongle_attach(char *name);
extern RET_CODE usb_3g_cdc_ether_attach(char *p_name);
extern RET_CODE ethernet_concerto_attach(char *p_name);

#endif
/*lint -e64 -e746 for pc-lint*/
#ifndef WIN32
  #define FF_BSS __attribute__((section(".av_bss")))
  extern FF_BSS ipc_fw_fun_set_t g_ipcfw_f;
  extern RET_CODE usb_mass_stor_attach(char *p_name);
  extern u32 attach_ipcfw_fun_set_concerto(ipc_fw_fun_set_t * p_funset);
#endif

#define DEMOD_RST_PIN 3
#define PTI_PARALLEL

#define AV_SYNC_FLAG_ON
#define GPE_USE_HW (1)

#ifdef WIN32
extern RET_CODE disk_win32_attach(char *p_name);
#endif
extern u8 ufs_dev_init();
extern void load_ui_resource(void);
extern RET_CODE lpower_concerto_attach(char *p_name);
extern void epg_api_init(void);
extern ifilter_t *epg_filter_create(void *p_para);
extern ap_frm_policy_t *construct_ap_frm_policy(void);
#ifdef SONGXIA_RC
extern app_t *construct_ap_songxia_uio(void);
#else
extern ap_uio_policy_t *construct_ap_uio_policy(void);
#endif
extern sig_mon_policy_t *construct_sig_mon_policy(void);
extern pb_policy_t *construct_pb_policy(void);
extern cas_policy_t *construct_cas_policy(void);

//extern ota_policy_t *construct_dvbs_ota_policy(void);
extern time_policy_t *construct_time_policy(void);
extern usb_upg_policy_t *construct_usb_upg_policy(void);
//extern ota_policy_t *construct_ota_policy(void);
//extern ap_twin_port_policy_t *construct_twin_policy(void);
extern app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id);
extern void ui_init(void);

#ifdef BROWSER_APP
extern  browser_policy_t *construct_browser_policy(void);
#endif

#ifdef TENGRUI_ROLLTITLE
extern subt_policy_t* construct_subt_policy(void);
extern app_t *construct_ap_tr_subt(subt_policy_t *p_policy);
#endif

extern RET_CODE smc_attach_concerto(char *name);

#ifdef SMSX_CA
extern  void channel_init(u32 freq, u32 sym, u16  modulation,u16 pid, u16 table_id);
#endif
extern u32 g_video_fw_cfg_addr;
extern u32 g_sub_buffer_size;
extern u32 g_sub_buffer_addr;
extern u32 g_sub_vscaler_buffer_size;
extern u32 g_sub_vscaler_buffer_addr;
extern u32 g_osd0_buffer_size;
extern u32 g_osd0_buffer_addr;
extern u32 g_osd0_vscaler_buffer_size;
extern u32 g_osd0_vscaler_buffer_addr;
extern u32 g_osd1_buffer_size;
extern u32 g_osd1_buffer_addr;
extern u32 g_osd1_vscaler_buffer_size;
extern u32 g_osd1_vscaler_buffer_addr;
extern u32 g_vid_di_cfg_size;
extern u32 g_vid_di_cfg_addr;
extern u32 g_ap_av_share_mem_size;
extern u32 g_ap_av_share_mem_addr;
extern u32 g_vid_sd_wr_back_size;
extern u32 g_vid_sd_wr_back_addr;
extern u32 g_audio_fw_cfg_size;
extern u32 g_audio_fw_cfg_addr;
extern u32 g_vbi_buf_size;
extern u32 g_vbi_buf_addr;

static inline RET_CODE gpe_vsb_sim_attach_concerto(char *name)
{
    return gpe_soft_attach(name);
}

/*lint -e64 for pc-lint*/
static hw_cfg_t hw_cfg = {0};
#define NUM_ENCODINGS (3)
iconv_t g_cd_utf8_to_utf16le = NULL;
iconv_t g_cd_utf16le_to_utf8 = NULL;
iconv_t g_cd_gb2312_to_utf16le = NULL;



static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = g_sub_buffer_addr;
  if(g_sub_buffer_size == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + g_sub_buffer_size;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + g_sub_buffer_size / 2;
  p_cfg->p_sub_cfg->even_mem_start = p_addr + g_sub_buffer_size / 2;
  p_cfg->p_sub_cfg->even_mem_end = p_addr + g_sub_buffer_size;
#endif

  // init osd0 frame buffer
  p_addr = g_osd1_buffer_addr;
  if(g_osd1_buffer_size == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + g_osd1_buffer_size;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + g_osd1_buffer_size / 2;
  p_cfg->p_osd1_cfg->even_mem_start = p_addr + g_osd1_buffer_size / 2;
  p_cfg->p_osd1_cfg->even_mem_end = p_addr + g_osd1_buffer_size;
#endif

  // init osd1 frame buffer
  p_addr = g_osd0_buffer_addr;
  if(g_osd0_buffer_size == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + g_osd0_buffer_size;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + g_osd0_buffer_size / 2;
  p_cfg->p_osd0_cfg->even_mem_start = p_addr + g_osd0_buffer_size / 2;
  p_cfg->p_osd0_cfg->even_mem_end = p_addr + g_osd0_buffer_size;
#endif

  p_cfg->lock_type = OS_MUTEX_LOCK;
  return TRUE;
}

static void util_init(void)
{
#ifdef CORE_DUMP_DEBUG
  static debug_mem_t d_mem = {0};

  d_mem.start_addr = (u8 *)mtos_malloc(10 * KBYTES);
  MT_ASSERT(d_mem.start_addr != NULL);
  memset(d_mem.start_addr, 0, 10 * KBYTES);

  d_mem.common_print_size = 2 * KBYTES;
  d_mem.msgq_print_size = 2 * KBYTES;
  d_mem.os_info_size = 4 * KBYTES;
  d_mem.stack_info_size = 1 * KBYTES;
  d_mem.mem_map_size = 1 * KBYTES;

  os_debug_init(&d_mem);
#endif
  simple_queue_init();

  //initialize iconv
  MT_ASSERT(0 == iconv_init(NUM_ENCODINGS));
  MT_ASSERT(0 ==ICONV_ADD_ENC(utf8));  //utf-8
  MT_ASSERT(0 ==ICONV_ADD_ENC(ucs2le));  //utf-16 little endian
  MT_ASSERT(0 ==ICONV_ADD_ENC(euccn)); //gb2312

  g_cd_utf8_to_utf16le = iconv_open("ucs2le", "utf8");
  g_cd_utf16le_to_utf8 = iconv_open("utf8", "ucs2le");
  g_cd_gb2312_to_utf16le = iconv_open("ucs2le", "euccn");
}

#ifndef WIN32

typedef struct  bin_board_cfg_st
{
  BOOL bin_flag;
  hal_board_config_t  board_cfg;
}bin_board_cfg;
static bin_board_cfg bin_board_cfg_info;

void set_board_config(void)
{
  u8 cfg_buff[2 * KBYTES] = {0};
  u32 read_len = 0;
  u32 size = 0;

  memset(&bin_board_cfg_info,0,sizeof(bin_board_cfg));
  bin_board_cfg_info.bin_flag = 0;

  size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), BOARD_CONFIG_BLOCK_ID);
  OS_PRINTF("\nset_board_config size = %d\n",size);

  if(size >0)
  {

    if(size > 2 * KBYTES)
    {
      size = 2 * KBYTES;
    }
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
    BOARD_CONFIG_BLOCK_ID, 0, 0,
    size,cfg_buff);
    if(read_len >0)
    {
      bin_board_cfg_info.bin_flag = 1;
      memcpy(&bin_board_cfg_info.board_cfg,cfg_buff,sizeof(hal_board_config_t));

#if 0
      OS_PRINTK("board confg av_perf.changed_param_cnt :%d\n",bin_board_cfg_info.board_cfg.av_perf.changed_param_cnt);
      OS_PRINTK("board confg av_perf.pal_changed_param_cn :%d\n",bin_board_cfg_info.board_cfg.av_perf.pal_changed_param_cnt);
      OS_PRINTK("board confg av_perf.ntsc_changed_param_cnt :%d\n",bin_board_cfg_info.board_cfg.av_perf.ntsc_changed_param_cnt);

      OS_PRINTK("board confg smc_dete :%d\n",bin_board_cfg_info.board_cfg.misc_config.smc_detect_validity);
      OS_PRINTK("board confg smc_power :%d\n",bin_board_cfg_info.board_cfg.misc_config.smc_power_validity);
      OS_PRINTK("board confg tn_loopout :%d\n",bin_board_cfg_info.board_cfg.misc_config.is_tn_loopthrough);
#endif

      #ifdef USE_BOARD_CONFIG
      hal_board_config(cfg_buff);
      #else
      bin_board_cfg_info.bin_flag = 0;
      #endif
    }
  }
}
#endif

#ifdef WIN32
static void s32_block_int(void)
{
  drv_dev_t *p_dev = NULL;
  block_device_config_t config = {0};
  u8 part_tab[20] = {0};
  u8 value = 0;
  RET_CODE ret = SUCCESS;
  char *disk_win32_dev = "disk_win32";

  assign_drives(1, 5); //fix me andy.chen//what's happened
  disk_win32_attach(disk_win32_dev);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_WIN32_FAKE_DEV);
  OS_PRINTF("p_dev[0x%x]\n", p_dev);

  /*config  block device*/
  ufs_dev_init();
  config.cmd = 1;  /*support tr transport*/
  config.tr_submit_prio = BLOCK_SERV_PRIORITY;   /*tr task prio*/
  dev_open(p_dev, &config);

  regist_device(p_dev);
  regist_logic_parttion(part_tab, &value);
}
#endif

#if 0
static void nim_driver_init(void)
{
#ifdef CHIP
  extern RET_CODE nim_m88ds3000_attach(char *name);
  RET_CODE ret;
  nim_config_t nim_cfg;
  void *p_nim_dev = NULL;
  chip_rev_t chip = hal_get_chip_rev();

  /* reset demod chip before attach driver */
  gpio_ioctl(GPIO_CMD_IO_ENABLE, DEMOD_RST_PIN, TRUE);
  gpio_set_dir(DEMOD_RST_PIN, GPIO_DIR_OUTPUT);
  gpio_set_value(DEMOD_RST_PIN, 0);
  mtos_task_delay_ms(20);
  gpio_set_value(DEMOD_RST_PIN, 1);
  mtos_task_delay_ms(10);

  ret = nim_m88ds3000_attach("ds3k");
  MT_ASSERT(ret == SUCCESS);

  p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_nim_dev);

  memset(&nim_cfg, 0, sizeof(nim_config_t));
  nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_ctrller_master0");
  MT_ASSERT(NULL != nim_cfg.p_dem_bus);

  nim_cfg.bs_times = 1;
  nim_cfg.pin_config.vsel_when_13v = NIM_PIN_LEVEL_LOW;
  nim_cfg.pin_config.vsel_when_lnb_off = NIM_PIN_LEVEL_HIGH;
  nim_cfg.pin_config.diseqc_out_when_lnb_off = NIM_PIN_LEVEL_LOW;
  nim_cfg.pin_config.lnb_enable_by_mcu = 0;
  nim_cfg.pin_config.lnb_enable = NIM_PIN_LEVEL_HIGH;
  nim_cfg.pin_config.lnb_prot_by_mcu = 0;
  nim_cfg.lock_mode = OS_MUTEX_LOCK;
  nim_cfg.freq_offset_limit = 5000;


  if(CHIP_WARRIORS_A1 <= chip)
  {
    nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
  }
  else if(CHIP_WARRIORS_A0 == chip)
  {
    nim_cfg.ts_mode = NIM_TS_INTF_DEF;
  }

  nim_cfg.tun_support = TS2022;
  nim_cfg.tun_crystal_khz = 27000;
  nim_cfg.task_prio = NIM_NOTIFY_TASK_PRIORITY;
  nim_cfg.stack_size = NIM_NOTIFY_TASK_STK_SIZE;
  nim_cfg.dem_addr = 0xd0; //support 2 tuners

  if(hw_cfg.tuner_clock == 1)
  {
    OS_PRINTF("crystal tuner CLK_OUT solution\n");
    nim_cfg.tun_clk_out_by_tn = 1;
    nim_cfg.tun_clk_out_div = 1;
  }
  else
  {
    nim_cfg.tun_clk_out_by_xtal = 1;
    nim_cfg.tun_clk_out_by_tn = 0;
    OS_PRINTF("crystal tuner CLK_XTAL solution\n");
  }
  ret = dev_open(p_nim_dev, &nim_cfg);
  MT_ASSERT(SUCCESS == ret);
#else
  extern RET_CODE nim_m88rs2k_attach(char *name);
  RET_CODE ret;
  nim_config_t nim_cfg;
  void *p_nim_dev = NULL;

  ret = nim_m88rs2k_attach("rs2k");
  MT_ASSERT(ret == SUCCESS);

  p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_nim_dev);

  memset(&nim_cfg, 0, sizeof(nim_config_t));
  nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_ctrller_master0");
  MT_ASSERT(NULL != nim_cfg.p_dem_bus);

  nim_cfg.bs_times = 1;
  nim_cfg.pin_config.lnb_enable = NIM_PIN_LEVEL_LOW;
  nim_cfg.pin_config.vsel_when_13v = NIM_PIN_LEVEL_HIGH;
  nim_cfg.pin_config.vsel_when_lnb_off = NIM_PIN_LEVEL_HIGH;
  nim_cfg.pin_config.diseqc_out_when_lnb_off = NIM_PIN_LEVEL_LOW;
  nim_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_nim_dev, &nim_cfg);
  MT_ASSERT(SUCCESS == ret);
#endif
}
#endif

#ifndef WIN32
#ifdef ENABLE_USB_CONFIG
static void block_driver_attach(void)
{
  RET_CODE ret = ERR_FAILURE;
  drv_dev_t *p_gUsb_dev = NULL;
  block_device_config_t mass_config;

  if(CUSTOMER_ID == CUSTOMER_JIZHONGMEX)
  {
    ret = usb_mass_stor_attach(DEV_NAME_BLOCK_USB0);
    MT_ASSERT(ret == SUCCESS);
    p_gUsb_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB0);
  }
  else
  {
    ret = usb_mass_stor_attach(DEV_NAME_BLOCK_USB1);
    MT_ASSERT(ret == SUCCESS);
    p_gUsb_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB1);
  }
  MT_ASSERT(NULL != p_gUsb_dev);
  //  mass_config.cmd = 1;
  //  mass_config.tr_submit_prio = 10;
  mass_config.cmd = 0;

  mass_config.bus_prio = USB_BUS_TASK_PRIO;
  mass_config.bus_thread_stack_size = DRV_USB_BUS_STKSIZE;

  mass_config.block_mass_stor_prio = USB_MASS_STOR_TASK_PRIO;
  mass_config.block_mass_stor_thread_stack_size = DRV_USB_BUS_STKSIZE;
  mass_config.lock_mode = OS_MUTEX_LOCK;
  mass_config.hub_tt_prio = USB_HUB_TT_TASK_PRIO;
  mass_config.hub_tt_stack_size = USB_HUB_TT_TASK_SIZE;
  ret = dev_open(p_gUsb_dev, (void *)&mass_config);
  MT_ASSERT(SUCCESS == ret);

#ifdef DTMB_PROJECT
  {
    memset(&mass_config,0,sizeof(block_device_config_t));
    ret = usb_mass_stor_attach(DEV_NAME_BLOCK_USB0);
    MT_ASSERT(ret == SUCCESS);
    p_gUsb_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB0);
    MT_ASSERT(NULL != p_gUsb_dev);
    //  mass_config.cmd = 1;
    //  mass_config.tr_submit_prio = 10;
    mass_config.cmd = 0;

    mass_config.bus_prio = USB_BUS_TASK_PRI1;
    mass_config.bus_thread_stack_size = DRV_USB_BUS_STKSIZE;

    mass_config.block_mass_stor_prio = USB_MASS_STOR_TASK_PRI1;
    mass_config.block_mass_stor_thread_stack_size = DRV_USB_BUS_STKSIZE;
    mass_config.lock_mode = OS_MUTEX_LOCK;
    mass_config.hub_tt_prio = USB_HUB_TT_TASK_PRI1;
    mass_config.hub_tt_stack_size = USB_HUB_TT_TASK_SIZE;
    ret = dev_open(p_gUsb_dev, (void *)&mass_config);
    MT_ASSERT(SUCCESS == ret);
  }
#endif
}
#endif
#endif

//-----------------------------------------------------------
   /*usb dump code  begin*/
//-----------------------------------------------------------
#ifndef WIN32

#if ENABLE_USB_DUMP_BREAK_LOG
#define USB_DUMP_LOG_TOTAL_NUM 10
#define  USB_DUMP_LOG_CMP_HEADER  "C:\\usb_dump_log_version"   // used for modify txt name
#define USB_DUMP_LOG_PATH_SUFFIX1 "%s"                                     //add or not
#define USB_DUMP_LOG_PATH_SUFFIX2 "(%d)"
#define USB_DUMP_LOG_PATH_SUFFIX3  ".txt"

#define USB_DUMP_LOG_ASSRT_HEADER "AP!# exception!"

extern void os_set_out_buff(void *p_buf);
/*!
   Use the warriors int function
  */
 BOOL usb_dump_log_set_buf()
{
   BOOL ret = FALSE;
   u8 *p_epg_addr = NULL;

   mem_cfg(MEMCFG_T_NORMAL);
   p_epg_addr =  (u8 *)mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
   mem_mgr_release_block(BLOCK_REC_BUFFER);
   os_set_out_buff(p_epg_addr);
    if(!memcmp(p_epg_addr + 4,USB_DUMP_LOG_ASSRT_HEADER,
                 strlen( USB_DUMP_LOG_ASSRT_HEADER)))
    {
    ret = TRUE;
    memcpy(p_epg_addr + 4,"cmp ok!",strlen("cmp ok!"));
    hal_dcache_flush_all();
    }
    return ret;
 }

/*!
   dump log to usb function
  */
  BOOL dump_log_to_usb()
  {
    u8 i = 0,uni_len = 0,sort_num = 0,sort_order = 0,usb_dump_num = 0,dump_txt_count = 0,
    dump_file_path[50] = USB_DUMP_LOG_CMP_HEADER;
    u16 FileCount = 0,block_num = 0,block_size = 0,cmp_len = 0,
    file_name[50] = {0},ffilter_all[32] = {0},
    data[USB_DUMP_LOG_TOTAL_NUM] = {0};
    u32 ret = 0,partition_cnt = 0,flash_base_addr = NORF_BASE_ADDR_NON_CACHE,
    dm_header_addr = DM_HDR_START_ADDR,dm_block_addr = 0,
    dm_ss_addr = 0,dm_ss_flash_addr = 0;
    char file_name_char[50] = {0};

    hfile_t file = NULL;
    partition_t *p_partition = NULL;
    flist_dir_t flist_dir = NULL;
    u8 *buffer_block = NULL;
    dmh_block_info_t *p_block_info = NULL;
    dmh_block_info_t *dmh_block_info_addr = NULL;

    file_list_t file_list = {0};
    upg_file_ver_t upg_file_version = {{0},{0},{0}, 0};
    utc_time_t sys_time = {0};

    str_asc2uni(dump_file_path,file_name);

    cmp_len = strlen(USB_DUMP_LOG_CMP_HEADER);
    partition_cnt = file_list_get_partition(&p_partition);

    block_num = *(u16 *)(flash_base_addr + dm_header_addr + 24);
    block_size = *(u16 *)(flash_base_addr + dm_header_addr + 26);
    dm_block_addr = flash_base_addr + dm_header_addr + 28;
    dmh_block_info_addr = (dmh_block_info_t *)dm_block_addr;
	if(partition_cnt > 0)
	{
		for(i = 0; i < block_num; i++)
		{
			p_block_info = (dmh_block_info_t *)((u8 *)dmh_block_info_addr + i * block_size);
			if(SS_DATA_BLOCK_ID == p_block_info->id)
			{
			dm_ss_addr = *(u32 *)((u8 *)p_block_info + 4);
			dm_ss_flash_addr = dm_ss_addr + flash_base_addr + dm_header_addr;
		       memcpy(upg_file_version.changeset,(u32 *)dm_ss_flash_addr,19);
		       memcpy(&sys_time,(u32 *)(dm_ss_flash_addr + \
			       (u8) ( (u8 *)&((sys_status_t *)(0))->time_set-(u8 *)0 )),
			        sizeof(utc_time_t));
			break;
			}
		}
		if(upg_file_version.changeset[0] == 0)
		upg_file_version.changeset[0] = (u8)('0');

		str_asc2uni("|txt|nodir", ffilter_all);
		flist_dir = file_list_enter_dir(ffilter_all, 100, p_partition[0].letter);
		file_list_get(flist_dir, FLIST_UNIT_FIRST, &file_list);
	       FileCount = (u16)file_list.file_count;

		if(FileCount > 0)
		{
			for(i=0; i<FileCount; i++)
			{
			  uni_len = uni_strlen(file_list.p_file[i].name);
			  if(!memcmp(file_list.p_file[i].name,file_name,
			          cmp_len * 2))
			  {
			     if((u8)file_list.p_file[i].name[uni_len - 5] != (u8)(')'))
			     {
			        if(!memcmp((file_list.p_file[i].name + cmp_len),
				              (u16 *)upg_file_version.changeset,(uni_len - 4 - cmp_len) * 2))
				 data[i] = 1;
			     }
			     else if((u8)file_list.p_file[i].name[uni_len - 5] == (u8)(')'))
			     {
			        if(!memcmp((file_list.p_file[i].name + cmp_len),
				           (u16 *)upg_file_version.changeset,(uni_len - 7 - cmp_len) *2))
				 data[i] = file_list.p_file[i].name[uni_len - 6] - '0';
			     }
				 dump_txt_count ++;
			  }
			}
			if(data[0] == 0)
			dump_txt_count = 0;
		}

		//sort the num when it has same version
		for(sort_num = 0;sort_num < dump_txt_count;sort_num ++)
		{
                   for(sort_order = 0;sort_order < dump_txt_count - sort_num;sort_order ++)
		     {
		       if(sort_order + 1 == dump_txt_count - sort_num || dump_txt_count - sort_num == 1)
			 break;
		       if(data[sort_order] > data[sort_order + 1])
		       {
		           data[sort_order]  = data[sort_order]  + data[sort_order + 1];
		           data[sort_order + 1] = data[sort_order] - data[sort_order + 1];
		           data[sort_order] = data[sort_order] - data[sort_order + 1];
		       }
		     }
		}

		if(sort_num == 0)
		{
		        strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX1);  //cutomer choose which suffix
		        strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX3);

	               sprintf(file_name_char,dump_file_path,
		                  upg_file_version.changeset);
		}
		else
		{
			if(data[sort_num - 1] == 1)
			usb_dump_num = 2;
			else
			{
			usb_dump_num = (data[sort_num - 1] + 1) % USB_DUMP_LOG_TOTAL_NUM;
			}
			if(usb_dump_num == 0)
			{
		       strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX1);   //cutomer choose which suffix
		       strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX3);
			sprintf(file_name_char,dump_file_path,
			upg_file_version.changeset);
			}
			else
			{
			strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX1);   //cutomer choose which suffix when it has same version
			strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX2);   //if dont need ,you can delete this else
			strcat(dump_file_path,USB_DUMP_LOG_PATH_SUFFIX3);
			sprintf(file_name_char,dump_file_path,
			upg_file_version.changeset,usb_dump_num);
			}
		}
      str_asc2uni((u8 *)file_name_char,(u16 *)file_name);
      time_init(&sys_time);
      file = vfs_open(file_name,  VFS_NEW);
		if(file == NULL)
		{
		   return FALSE;
		}
		vfs_seek( file,0, VFS_SEEK_SET);
		buffer_block =  (u8 *)mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
       memcpy(buffer_block + 4,USB_DUMP_LOG_ASSRT_HEADER,
                 strlen(USB_DUMP_LOG_ASSRT_HEADER));
       block_size = *(u32 *)buffer_block;
       if(block_size == 0)
       {
         ret = vfs_write("ap exection dont write the data size",
                        sizeof("ap exection dont write the data size"), 1, file);
       }
		ret = vfs_write(buffer_block + 4,*(u32 *)buffer_block, 1, file);
		if(ret == 0)
		{
		   return FALSE;
		}
		vfs_close(file);
		memset(buffer_block,0,*(u32 *)buffer_block + 4);
       hal_dcache_flush_all();
		mem_mgr_release_block(BLOCK_REC_BUFFER);
		return TRUE;
	}
       return FALSE;
  }

/*!
   wait for the usb connect  function
  */
 void task_usb_monitor()
{
	while(1)
	{
		vfs_dev_event_t  p_event = {0};
		if(vfs_process(&p_event))
		{
			if(p_event.evt == VFS_PLUG_IN_EVENT)
			{
			   dump_log_to_usb();
			}
		}
	}
}

/*!
   init usb for dump function
  */
void usb_dump_log_init()
{
	ufs_dev_init();   //register callback for block layer
   block_driver_attach();
   vfs_mt_init();
	file_list_init();
	{
		pnp_svc_init_para_t pnp_param = {0};
		pnp_param.nstksize = MDL_PNP_SVC_TASK_STKSIZE;
		pnp_param.service_prio = MDL_PNP_SVC_TASK_PRIORITY;
		pnp_service_init(&pnp_param);
	}
	mul_pvr_init();
	{
		BOOL ret = FALSE;
		u32 *pstack_usb = (u32*)mtos_malloc(USB_DUMP_LOG_STKSIZE);
		MT_ASSERT(pstack_usb != NULL);

		ret = mtos_task_create((u8 *)"usb monitor",
		task_usb_monitor,
		(void *)0,
		USB_TASK_DUMP_PRIO,
		pstack_usb,
		USB_DUMP_LOG_STKSIZE);
		MT_ASSERT(FALSE != ret);
	}
}
#endif
#endif
//-----------------------------------------------------------
   /*usb dump code function end*/
//-----------------------------------------------------------
#ifndef WIN32
#if ENABLE_WATCH_DOG
#ifdef DIVI_CA
extern u8 g_divi_start_finish;
extern u8 DIVI_ENTITLE_INFO_GET;
extern RET_CODE divi_ca_entitle_remind_info(void);
#endif
static void  _feed_dog(void *p_param)
{
    hal_watchdog_feed();
    while(1)
    {
    	hal_watchdog_feed();

      #ifdef DIVI_CA
      if (g_divi_start_finish == 1)
      {
        #ifdef DIVI_GET_TIME_FROM_CA
        {
          OS_PRINTF("[DIVI]Request system time from CA!\n");
          ui_ca_do_cmd(CAS_CMD_SYSTEM_TIME_GET, 0, 0);
        }
        #endif
        if (DIVI_ENTITLE_INFO_GET == 1)
        {
          divi_ca_entitle_remind_info();
        }
      }
      #endif
    	mtos_task_sleep(3000);
    }
}

void watchdog_init(void)
{
  BOOL ret = FALSE;
  u32 *pstack_usb = (u32*)mtos_malloc(16 * KBYTES);
  MT_ASSERT(pstack_usb != NULL);

  if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))// only support 7S max in B, 14S max in A
    hal_watchdog_init(7000);
  else
    hal_watchdog_init(14000);

  hal_watchdog_enable();

  ret = mtos_task_create((u8 *)"feed watchdog",
      	_feed_dog,
	(void *)0,
	WATCHDOG_FEED_PRIO,
	pstack_usb,
	16 * KBYTES);
	MT_ASSERT(FALSE != ret);
 }
#endif
#endif

BOOL uio_get_lock_pol(void)
{
  return hw_cfg.pan_info.lbd[0].polar;
}


BOOL uio_get_power_pol(void)
{
  return hw_cfg.pan_info.lbd[1].polar;
}

u8 *find_led_map(u8 *LED_MAP2)
{
  u8 bitmap_led[8] = {'.', '1', '3', '6', '7', '9', '-', '_'};
  u8 i = 0, j = 0, l_num = 0;

  l_num = 0;
  while(l_num < hw_cfg.map_size)
  {
    if(hw_cfg.led_bitmap[l_num].ch == bitmap_led[i])
    {
      bitmap_led[i] = hw_cfg.led_bitmap[l_num].bitmap;
      i++;
    }
    l_num++;
  }
  LED_MAP2[0] = bitmap_led[4] ^ bitmap_led[1];
  LED_MAP2[1] = ~(bitmap_led[0] | bitmap_led[3]);
  LED_MAP2[2] = bitmap_led[1] ^ LED_MAP2[1];
  LED_MAP2[3] = bitmap_led[7];
  LED_MAP2[4] = ~(bitmap_led[0] | bitmap_led[5]);
  LED_MAP2[5] = bitmap_led[5] ^ bitmap_led[2];
  LED_MAP2[6] = bitmap_led[6];
  LED_MAP2[7] = bitmap_led[0];

  while(j < 8)
  {
    for(i = 0; i < 8; i++)
    {
      if(LED_MAP2[j] >> (i + 1) == 0)
      {
        LED_MAP2[j] = i;
        break;
      }
    }
    j++;
  }
  return LED_MAP2;
}
static void uio_init(void)
{
  RET_CODE ret;
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};


  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  fpcfg.p_info = &(hw_cfg.pan_info);
  //fpcfg.p_map = hw_cfg.led_bitmap;
  //fpcfg.map_size = hw_cfg.map_size;


  app_printf(PRINTF_DEBUG_LEVEL,"===fpcfg.fp_type:%d, map_size %d ,com0:%d, com7:%d,reserve7:%d\n", fpcfg.fp_type, fpcfg.map_size,
    fpcfg.p_info->com[0].pos, fpcfg.p_info->com[7].pos,fpcfg.p_info->com[7].reserve);

  if(g_customer.customer_id == CUSTOMER_NEWSTART ||
     g_customer.customer_id == CUSTOMER_JIZHONG ||
     g_customer.customer_id == CUSTOMER_JIZHONGMEX)
  {
      fpcfg.fp_type = HAL_GPIO;
      uiocfg.p_fpcfg = &fpcfg;
      ret = dev_open(p_dev, &uiocfg);
      MT_ASSERT(SUCCESS == ret);
      dev_close(p_dev);
      app_printf(PRINTF_DEBUG_LEVEL,"standby key for newstart FP\n");
  }

   fpcfg.fp_type = hw_cfg.fp_type;

  if(g_customer.customer_id == CUSTOMER_ORRISDIGITAL)
  {
      fpcfg.fp_type = HAL_ORRIS;
      fpcfg.fp_op_pri = FP_TASK_PRIORITY;
      fpcfg.fp_op_stksize = 4*1024;
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
  irdacfg.irda_repeat_time = 500;

{
  u8 i;
  u8 power_keys[3];
  u8 tmp;
  u16 user_code;
  u8 block_ids[3] = {IRKEY_BLOCK_ID, IRKEY1_BLOCK_ID, IRKEY2_BLOCK_ID};


  irdacfg.irda_wfilt_channel = 3;

  for (i=0; i<3; i++)
  {
    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 1,
          1,
          (u8 *)&tmp);
    user_code = tmp;
    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 0,
          1,
          (u8 *)&tmp);
    user_code |= ((u16)tmp) << 8;

    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 2,
          1,
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

  {
     ui_set_com_num((u8)(hw_cfg.led_num));
  }
  if(hw_cfg.led_num == 4)
  {
    if(CUSTOMER_ID == CUSTOMER_JIULIAN)
    {
      uio_display(p_dev, (u8 *)" ON. ", 5);
    }
    else
    {
    uio_display(p_dev, (u8 *)" ON ", 4);
  }
  }
  else if(hw_cfg.led_num == 3)
  {
    uio_display(p_dev, (u8 *)"ON ", 3);
  }
  else if(hw_cfg.led_num == 2)
  {
    uio_display(p_dev, (u8 *)"ON ", 2);
  }
  else if(hw_cfg.led_num == 1)
  {
    uio_display(p_dev, (u8 *)"O ", 1);
  }
  if(g_customer.customer_id == CUSTOMER_JIZHONG
    || g_customer.customer_id == CUSTOMER_JIESAI_MINI)
  {
  	dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, FALSE);
  }
  else if(g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
  #ifndef WIN32
    //init lock led AGPIO2
    gpio_io_enable(66, TRUE);
    gpio_set_dir(66, GPIO_DIR_OUTPUT);
    gpio_set_value(66, 0);
  #endif
  }
  else if(g_customer.customer_id == CUSTOMER_KONKA)
  {
  	dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, FALSE);
  	dev_io_ctrl(p_dev, UIO_FP_SET_COLON, TRUE);
  }
  else if(g_customer.customer_id == CUSTOMER_JIZHONGMEX)
  {
    dev_io_ctrl(p_dev, UIO_FP_SET_LOCK_LBD, 1);
    dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, 0);
  }
  else
  {
	dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);
  }
#ifndef WIN32
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
#endif
  //gpio_output(0, 1);
  }
#if 0
static void disp_set_hdmi(hdmi_video_config_t *p_hdmi_vcfg, avc_video_mode_1_t new_std)
{

  disp_sys_t video_mode = VID_SYS_1080I_50HZ;

  switch(new_std)
  {
    case AVC_VIDEO_MODE_NTSC_1:
      if(video_mode != VID_SYS_NTSC_M)
      {
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_1:
      if(video_mode != VID_SYS_PAL)
      {
        video_mode = VID_SYS_PAL;
      }
      break;
    case AVC_VIDEO_MODE_PAL_M_1:
      if(video_mode != VID_SYS_PAL_M)
      {
        video_mode = VID_SYS_PAL_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_N_1:
      if(video_mode != VID_SYS_PAL_N)
      {
        video_mode = VID_SYS_PAL_N;
      }
      break;
    case AVC_VIDEO_MODE_480P:
      if(video_mode != VID_SYS_480P)
      {
        video_mode = VID_SYS_480P;
      }
      break;
    case AVC_VIDEO_MODE_576P:
      if(video_mode != VID_SYS_576P_50HZ)
      {
        video_mode = VID_SYS_576P_50HZ;
      }
      break;
    case AVC_VIDEO_MODE_720P_60HZ:
      if(video_mode != VID_SYS_720P)
      {
        video_mode = VID_SYS_720P;
      }
      break;
    case AVC_VIDEO_MODE_720P_50HZ:
      if(video_mode != VID_SYS_720P_50HZ)
      {
        video_mode = VID_SYS_720P_50HZ;
      }
      break;
    case AVC_VIDEO_MODE_1080I_50HZ:
      if(video_mode != VID_SYS_1080I_50HZ)
      {
        video_mode = VID_SYS_1080I_50HZ;
      }
      break;
     case AVC_VIDEO_MODE_1080I_60HZ:
      if(video_mode != VID_SYS_1080I)
      {
        video_mode = VID_SYS_1080I;
      }
      break;
     case AVC_VIDEO_MODE_1080P_60HZ:
      if(video_mode != VID_SYS_1080P)
      {
        video_mode = VID_SYS_1080P;
      }
      break;
     case AVC_VIDEO_MODE_1080P_50HZ:
      if(video_mode != VID_SYS_1080P_50HZ)
      {
        video_mode = VID_SYS_1080P_50HZ;
      }
      break;
     case AVC_VIDEO_MODE_480I:
      if(video_mode != VID_SYS_NTSC_M)
      {
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_576I:
      if(video_mode != VID_SYS_PAL)
      {
        video_mode = VID_SYS_PAL;
      }
      break;
    default:
        video_mode = VID_SYS_1080I_50HZ;
      break;
    }
    switch(video_mode)
    {
      case VID_SYS_PAL:
      case VID_SYS_PAL_N:
      case VID_SYS_PAL_NC:
      case VID_SYS_PAL_M:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_2_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_576I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_NTSC_J:
      case VID_SYS_NTSC_M:
      case VID_SYS_NTSC_443:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_2_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_480I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_576P_50HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_576P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_480P:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_480P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_720P:
      case VID_SYS_720P_30HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_720P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_720P_50HZ:
      case VID_SYS_720P_24HZ:
      case VID_SYS_720P_25HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_720P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080I:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080I_50HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080P:
      case VID_SYS_1080P_30HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080P_50HZ:
      case VID_SYS_1080P_25HZ:
      case VID_SYS_1080P_24HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      default: //1080i50
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
    }
}
#endif
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

  ret = dev_open(p_hdmi_dev, &hdmi_cfg);
  MT_ASSERT(SUCCESS == ret);
}
void spiflash_cfg( )
{
    RET_CODE ret;
    void *p_dev = NULL;
    void *p_spi = NULL;
    charsto_cfg_t charsto_cfg = {0};
#ifndef WIN32
    spi_cfg_t spiCfg;
    ret = spi_concerto_attach("spi_concerto_0");
    MT_ASSERT(SUCCESS == ret);
    ret = spiflash_jazz_attach("charsto_concerto");
    MT_ASSERT(SUCCESS == ret);
    OS_PRINTF("drv --charsto 1\n");
#endif

#ifndef WIN32
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

    p_spi = dev_find_identifier(NULL,DEV_IDT_NAME, (u32)"spi_concerto_0");
    spiCfg.spi_id = 0;
    ret = dev_open(p_spi, &spiCfg);
    MT_ASSERT(SUCCESS == ret);
#endif


    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    OS_PRINTF("drv --charsto 2\n");

    charsto_cfg.rd_mode = SPI_FR_MODE;
    charsto_cfg.p_bus_handle = p_spi;
    charsto_cfg.size = CHARSTO_SIZE;
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
}

static void get_sd_tv_mode_from_hd(disp_sys_t *p_sd, disp_sys_t hd)
{
  switch(hd)
  {
    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
    case VID_SYS_PAL_M:
    case VID_SYS_576P_50HZ:
    case VID_SYS_720P_50HZ:
    case VID_SYS_1080I_50HZ:
    case VID_SYS_1080P_50HZ:
      *p_sd = VID_SYS_PAL;
      break;
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_480P:
    case VID_SYS_720P:
    case VID_SYS_1080I:
    case VID_SYS_1080P:
      *p_sd = VID_SYS_NTSC_M;
      break;
    default:
      *p_sd = VID_SYS_PAL;
      break;
  }
}
static void get_hd_tv_mode_from_sd(disp_sys_t sd, disp_sys_t hd_old, disp_sys_t *p_hd_new)
{
    if((sd == VID_SYS_PAL) ||(sd == VID_SYS_PAL_N)
      ||(sd == VID_SYS_PAL_NC))
    {
      switch(hd_old)
      {
         case VID_SYS_1080P_50HZ:
         case VID_SYS_1080P:
          *p_hd_new = VID_SYS_1080P_50HZ;
           break;
         case VID_SYS_1080I_50HZ:
         case VID_SYS_1080I:
          *p_hd_new = VID_SYS_1080I_50HZ;
           break;
         case VID_SYS_720P_50HZ:
         case VID_SYS_720P:
           *p_hd_new = VID_SYS_720P_50HZ;
           break;
         case VID_SYS_576P_50HZ:
         case VID_SYS_480P:
            *p_hd_new = VID_SYS_576P_50HZ;
            break;
         case VID_SYS_PAL:
         case VID_SYS_PAL_N:
         case VID_SYS_PAL_NC:
         case VID_SYS_PAL_M:
         case VID_SYS_NTSC_J:
         case VID_SYS_NTSC_M:
         case VID_SYS_NTSC_443:
          *p_hd_new = VID_SYS_PAL;
            break;
        default:
          *p_hd_new = VID_SYS_1080I_50HZ;
           break;
      }
    }
    else
    {
      switch(hd_old)
      {
         case VID_SYS_1080P_50HZ:
         case VID_SYS_1080P:
          *p_hd_new = VID_SYS_1080P;
           break;
         case VID_SYS_1080I_50HZ:
         case VID_SYS_1080I:
          *p_hd_new = VID_SYS_1080I;
           break;
         case VID_SYS_720P_50HZ:
         case VID_SYS_720P:
           *p_hd_new = VID_SYS_720P;
           break;
         case VID_SYS_576P_50HZ:
          case VID_SYS_480P:
            *p_hd_new = VID_SYS_480P;
            break;
         case VID_SYS_PAL:
         case VID_SYS_PAL_N:
         case VID_SYS_PAL_NC:
         case VID_SYS_PAL_M:
         case VID_SYS_NTSC_J:
         case VID_SYS_NTSC_M:
         case VID_SYS_NTSC_443:
          *p_hd_new = VID_SYS_NTSC_M;
            break;
        default:
          *p_hd_new = VID_SYS_1080I;
           break;
      }
    }
}

static void disp_switch_video_mode(void *p_disp, avc_video_mode_1_t new_std)
{
  disp_sys_t video_mode;
  aspect_ratio_t video_aspect;
  BOOL b_switch = FALSE;
  disp_channel_t disp_chan = DISP_CHANNEL_SD;
  disp_sys_t sd_mode = VID_SYS_PAL;
  disp_sys_t hd_mode = VID_SYS_720P;
  disp_sys_t hd_old = VID_SYS_720P;
  disp_common_info_t disp_info = {0};

  if(new_std >= AVC_VIDEO_MODE_480P)
  {
    disp_chan = DISP_CHANNEL_HD;
  }
  disp_get_tv_sys(p_disp, disp_chan, &video_mode);
  disp_get_aspect_ratio(p_disp, disp_chan, &video_aspect);

  switch(new_std)
  {
    case AVC_VIDEO_MODE_NTSC_1:
      if(video_mode != VID_SYS_NTSC_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_1:
      if(video_mode != VID_SYS_PAL)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL;
      }
      break;
    case AVC_VIDEO_MODE_PAL_M_1:
      if(video_mode != VID_SYS_PAL_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_N_1:
      if(video_mode != VID_SYS_PAL_N)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL_N;
      }
      break;
    case AVC_VIDEO_MODE_480P:
      if(video_mode != VID_SYS_480P)
      {
        video_mode = VID_SYS_480P;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_576P:
      if(video_mode != VID_SYS_576P_50HZ)
      {
        video_mode = VID_SYS_576P_50HZ;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_720P_60HZ:
      if(video_mode != VID_SYS_720P)
      {
        video_mode = VID_SYS_720P;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_720P_50HZ:
      if(video_mode != VID_SYS_720P_50HZ)
      {
        video_mode = VID_SYS_720P_50HZ;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_1080I_50HZ:
      if(video_mode != VID_SYS_1080I_50HZ)
      {
        video_mode = VID_SYS_1080I_50HZ;
        b_switch = TRUE;
      }
      break;
     case AVC_VIDEO_MODE_1080I_60HZ:
      if(video_mode != VID_SYS_1080I)
      {
        video_mode = VID_SYS_1080I;
        b_switch = TRUE;
      }
      break;
     case AVC_VIDEO_MODE_1080P_60HZ:
      if(video_mode != VID_SYS_1080P)
      {
        video_mode = VID_SYS_1080P;
        b_switch = TRUE;
      }
      break;
     case AVC_VIDEO_MODE_1080P_50HZ:
      if(video_mode != VID_SYS_1080P_50HZ)
      {
        video_mode = VID_SYS_1080P_50HZ;
        b_switch = TRUE;
      }
      break;
     case AVC_VIDEO_MODE_480I:
      if(video_mode != VID_SYS_NTSC_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_576I:
      if(video_mode != VID_SYS_PAL)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL;
      }
      break;
    default:
      break;
  }

  if(b_switch == TRUE)
  {

    disp_get_common_info(p_disp, &disp_info);

    if(disp_info.ch_num > 1)//HD IC, warriors/sonata
    {

      if(video_mode >= VID_SYS_1080I)
      {
        get_sd_tv_mode_from_hd(&sd_mode, video_mode);
        hd_mode = video_mode;
      }
      else
      {
        if((new_std == AVC_VIDEO_MODE_480I)
          ||(new_std == AVC_VIDEO_MODE_576I))
        {
          hd_mode = video_mode;
        }
        else
        {
          disp_get_tv_sys(p_disp, DISP_CHANNEL_HD, &hd_old);
          get_hd_tv_mode_from_sd(video_mode, hd_old, &hd_mode);
        }

        sd_mode = video_mode;
      }
      {
       disp_set_tv_sys(p_disp, DISP_CHANNEL_HD, hd_mode);
      }
    }
    else
    {
        sd_mode = video_mode;
    }

    disp_set_tv_sys(p_disp, DISP_CHANNEL_SD, sd_mode);

    OS_PRINTF("set video std sd: %d  hd:  %d======================\n", sd_mode, hd_mode);
  }
}

static void disp_mode_config(void *p_disp)
{
  av_set_t av_set;
  drv_dev_t *p_hdmi_dev = NULL;
  RET_CODE ret = SUCCESS;

  sys_status_get_av_set(&av_set);

   if(av_set.tv_mode != 0)
      disp_switch_video_mode(p_disp, sys_status_get_sd_mode(av_set.tv_mode));
   disp_switch_video_mode(p_disp, sys_status_get_hd_mode_1(av_set.tv_resolution,p_disp));

  p_hdmi_dev = dev_find_identifier(NULL,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_HDMI);
  if(p_hdmi_dev != NULL)
  {
    //ret = dev_io_ctrl(p_hdmi_dev, HDMI_START_MAIN_SERVICE, 0);
  }
  if(ret != SUCCESS)
  {
    APPLOGE("Can not start main service ret %d\n",ret);
  }
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

static const drvsvc_handle_t* drv_public_svr_init(void)
{
  drvsvc_handle_t *p_public_drvsvc = NULL;
  u32 *p_buf = NULL;

  //this service will be shared by HDMI, Audio and Display driver
  p_buf = (u32 *)mtos_malloc(DRV_HDMI_TASK_STKSIZE);
  MT_ASSERT(NULL != p_buf);
  p_public_drvsvc = drvsvc_create(DRV_HDMI_TASK_PRIORITY,
    p_buf, DRV_HDMI_TASK_STKSIZE, 8);
  MT_ASSERT(NULL != p_public_drvsvc);

  return p_public_drvsvc;
}

static int drv_i2c_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  i2c_cfg_t i2c_cfg = {0};
  void *p_dev_i2c0 = NULL;
  void *p_dev_i2c1 = NULL;
  void *p_dev_i2cfp = NULL;
  void *p_dev_i2chdmi = NULL;
  void *p_dev_i2cqam = NULL;

  ret = i2c_concerto_attach("i2c0");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2c0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
  MT_ASSERT(NULL != p_dev_i2c0);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 0;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2c0, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C 1 */
  ret = i2c_concerto_attach("i2c1");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2c1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c1");
  MT_ASSERT(NULL != p_dev_i2c1);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 1;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2c1, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C FP */
  ret = i2c_concerto_attach("i2c_FP");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
  MT_ASSERT(NULL != p_dev_i2cfp);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 2;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2cfp, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C HDMI */
  ret = i2c_concerto_attach("i2c_HDMI");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2chdmi = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
  MT_ASSERT(NULL != p_dev_i2chdmi);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 3;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2chdmi, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C QAM */
  ret = i2c_concerto_attach("i2c_QAM");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2cqam = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");
  MT_ASSERT(NULL != p_dev_i2cqam);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 4;
  i2c_cfg.bus_clk_khz = 400;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2cqam, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);

  return SUCCESS;
}

#ifndef WIN32
static int drv_vbi_init(void)
{
  // vbi
  void *p_dev = NULL;
  vbi_cfg_t vbi_cfg = {0};
  RET_CODE ret = ERR_FAILURE;

  /*lint -e123 */
  ATTACH_DRIVER(VBI_INSERTER, concerto, default, default);
  /*lint +e123 */
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VBI_INSERTER);
  MT_ASSERT(NULL != p_dev);
  app_printf(PRINTF_DEBUG_LEVEL,"find vbi\n");
  vbi_cfg.vbi_pes_buf_addr = g_vbi_buf_addr;
  vbi_cfg.vbi_pes_buf_size = g_vbi_buf_size;
  ret = dev_open(p_dev, &vbi_cfg);
  MT_ASSERT(SUCCESS == ret);
  app_printf(PRINTF_DEBUG_LEVEL,"vbi ATTACH_DRIVER\n");

  return ret;
}
#endif
static int drv_gpe_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_gpe_c = NULL;

#if GPE_USE_HW
  /* GPE */
  app_printf(PRINTF_DEBUG_LEVEL,"GPE_USE_HW\n");
  /*lint -e123 */
  ret = ATTACH_DRIVER(GPE_VSB, concerto, default, default);
  MT_ASSERT(SUCCESS == ret);
  /*lint +e123 */

  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
#ifndef WIN32
  MT_ASSERT(TRUE == ret);
#else
  MT_ASSERT(SUCCESS == ret);
#endif
#else // soft
  /* GPE */
  void *p_gpe_c = NULL;
  ret = gpe_soft_attach("gpe_soft");
  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(SUCCESS == ret);
#endif

  return SUCCESS;
}

static int drv_jpeg_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;

  /*JPEG*/
  /*lint -e123 */
  ret = ATTACH_DRIVER(PDEC, concerto, default, default);
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(NULL != p_dev);

  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);

  return ret;
}
#ifdef MXL241
extern RET_CODE nim_mxl241_attach(char *name);
#endif

#ifdef TDA10028
RET_CODE nim_tda10028_attach(char *name);
#endif

static int drv_nim_init(void)
{
  //if(g_customer.customer_id == CUSTOMER_HUANGSHI)

#ifdef DTMB_PROJECT
  {
    void *p_dev = NULL;
    s32 ret = 0;
    /*Reset Demod*/
    /*
    s32 ret = gpio_io_enable(64, TRUE);
    gpio_set_dir(64, GPIO_DIR_OUTPUT);
    gpio_set_value(64, GPIO_LEVEL_LOW);
    mtos_task_delay_ms(100);
    gpio_set_value(64, GPIO_LEVEL_HIGH);
    mtos_task_delay_ms(300);
    */
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
    return ret;
  }
#elif defined(MXL241)
    /* NIM */
    RET_CODE ret = ERR_FAILURE;
    void *p_dev_i2c0 = NULL;
    void *p_dev = NULL;
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
  return ret;
#elif defined(TDA10028)
    RET_CODE ret = ERR_FAILURE;
    void *p_dev_i2c0 = NULL;
    void *p_dev = NULL;
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
 
    ret = nim_tda10028_attach("nim");
    MT_ASSERT(ret == SUCCESS);
    
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);
 
     nim_cfg.p_dem_bus = p_dev_i2c0;
     nim_cfg.tuner_bandwidth = 8;
     nim_cfg.lock_mode = OS_MUTEX_LOCK;
    OS_PRINTF("nim1=0x%x\n", (u32)p_dev);
    ret = dev_open(p_dev, &nim_cfg);
    MT_ASSERT(SUCCESS == ret);
    return ret;
#else
  {
    RET_CODE ret = ERR_FAILURE;
    void *p_dev = NULL;
    nim_config_t nim_cfg = {0};
    void *p_dem_bus_0 = NULL;
    void *p_dem_bus_qam = NULL;
    //u32 dmd_version = 0;
    //u32 tn_version = 0;

    p_dem_bus_0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
    p_dem_bus_qam = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");

     if(g_customer.customer_id == CUSTOMER_KONKA)
     {
#ifdef WIN32
        /*lint -e123 */
        ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
        /*lint +e123 */
#else
        ret = nim_m88dc2800_attach("nim");
#endif
        MT_ASSERT(ret == SUCCESS);
        {
#ifndef WIN32
          extern void tuner_attach_tc2800(void);
          tuner_attach_tc2800();
#endif
        }


        p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
        MT_ASSERT(NULL != p_dev);
        nim_cfg.p_dem_bus = p_dem_bus_0;
        nim_cfg.lock_mode = OS_LOCK_DEF;
        //nim_cfg.lock_mode = OS_LOCK_DEF;
#ifndef PTI_PARALLEL
        nim_cfg.dem_addr = 0x3a;
        nim_cfg.tun_addr = 0xc2;
        nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
#else
        nim_cfg.dem_addr = 0x38;
        nim_cfg.tun_addr = 0xc2;
        //nim_cfg.ts_mode = NIM_TS_INTF_PARALLEL;
        nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
#endif

        nim_cfg.dem_ver = DEM_VER_0;
        //nim_cfg.tuner_loopthrough = 1;
        ret = dev_open(p_dev, &nim_cfg);

        MT_ASSERT(SUCCESS == ret);

      //  dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_SET_TUNER_WAKEUP, 0);
     }
     else if(g_customer.customer_id == CUSTOMER_JIULIAN)
     {
        /*lint -e123 */
        ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
        /*lint +e123 */
        MT_ASSERT(ret == SUCCESS);
#ifndef WIN32
        {
            extern void tuner_attach_R836(void);
            ret = nim_m88dc2800_attach("nim");
            MT_ASSERT(ret == SUCCESS);
            tuner_attach_R836();
        }
#endif

        p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
        MT_ASSERT(NULL != p_dev);
        nim_cfg.p_dem_bus = p_dem_bus_qam;
        nim_cfg.lock_mode = OS_LOCK_DEF;
#ifndef PTI_PARALLEL
    nim_cfg.dem_addr = 0x3a;
    nim_cfg.tun_addr = 0x74;
    nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
#else
    nim_cfg.dem_addr = 0x38;
    nim_cfg.tun_addr = 0x74;
    nim_cfg.ts_mode = NIM_TS_INTF_PARALLEL;
#endif

        nim_cfg.dem_ver = DEM_VER_3;

        nim_cfg.tuner_loopthrough = 1;

        ret = dev_open(p_dev, &nim_cfg);

        MT_ASSERT(SUCCESS == ret);

        dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_SET_TUNER_WAKEUP, 0);
     }
     else

     {
        /*lint -e123 */
        ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
        /*lint +e123 */
        MT_ASSERT(ret == SUCCESS);
#ifndef WIN32
        {
          extern void tuner_attach_tc2800(void);
          tuner_attach_tc2800();
        }
#endif

        p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
        MT_ASSERT(NULL != p_dev);
        nim_cfg.p_dem_bus = p_dem_bus_qam;
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

        nim_cfg.tuner_loopthrough = 1;

        ret = dev_open(p_dev, &nim_cfg);

        MT_ASSERT(SUCCESS == ret);

        dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_SET_TUNER_WAKEUP, 0);

     }

    //dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_GET_DMD_VERSION, (u32)(&dmd_version));
    //dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_GET_TN_VERSION, (u32)(&tn_version));
    //APPLOGA("Current DMD version is V%d Tuner version is V%d\n",dmd_version,tn_version);

    return ret;
  }
#endif
}

#ifndef WIN32
static dm_dmh_info_t dmh_info;

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

static void ota_dm_api_init(void)
{
  ota_dm_config_t p_cfg = {0};
  dm_dmh_info_t *p_all_dmh_info = NULL;
  #if 0
  p_cfg.ota_dm_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_BLOCK_ID) - get_flash_addr();
  p_cfg.ota_dm_backup_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_RESERVE_BLOCK_ID) - get_flash_addr();

  APPLOGA("#### debug dm addr:0x%x,dm backup addr:0x%x\n",p_cfg.ota_dm_addr,p_cfg.ota_dm_backup_addr);
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

  APPLOGA("#####debug ui ota dm api  init\n");
  mul_ota_dm_api_init(&p_cfg);

  /****test use ,please don't use in project,but factory upg*****/
 //mul_ota_dm_api_reset_data_block();
}
static void ota_dm_api_bootload_info_init(void)
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
static void ota_dm_init(void)
{
  ota_dm_api_init();
  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
    ota_dm_api_bootload_info_init();
  }
  memset(&dmh_info,0,sizeof(dm_dmh_info_t));
  mul_ota_dm_api_find_dmh_info_from_flash(0,4 * MBYTES,&dmh_info);
}

static void load_jump_policy(void)
{
  u8 jump_block_id = MAINCODE_BLOCK_ID;
  ota_bl_info_t bl_info = {0};

  mul_ota_dm_api_read_bootload_info(&bl_info);
  APPLOGA("**************load info***************\n ");
  APPLOGA("ota_number:0x%x,  curr_ota_id:0x%x,  back_ota_id:0x%x,\n",bl_info.ota_number,bl_info.ota_curr_id,bl_info.ota_backup_id);
  APPLOGA("load_block_id:0x%x,ota_tri:%d\n", bl_info.load_block_id,bl_info.ota_status);
  APPLOGA("destroy flag:0x%x,   destroy_block:0x%x\n",bl_info.destroy_flag,bl_info.destroy_block_id);
  APPLOGA("**************load info end***************\n");
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

    APPLOGA("it will forc ota by dstroy flag\n");
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
        jump_block_id = OTA1_BLOCK_ID;
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

  if((jump_block_id == OTA_BLOCK_ID) || (jump_block_id == OTA1_BLOCK_ID))
  {
    APPLOGA("ota check reset stb !\n");
    hal_pm_reset();
    mtos_task_delay_ms(1000);
  }

}

static void ap_ota_check()
{
  ota_dm_init();
  load_jump_policy();
}
#endif
static int drv_dm_init(void)
{
  dm_v2_init_para_t dm_para = {0};
  u32 dm_app_addr = 0;
  RET_CODE ret;

  /*lint -e123 */
  ret = ATTACH_DRIVER(CHARSTO, concerto, default, default);
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);

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
  app_printf(PRINTF_INFO_LEVEL,"dm_para.flash_base_addr  :0x%x \n",dm_para.flash_base_addr );

  dm_init_v2(&dm_para);
  app_printf(PRINTF_INFO_LEVEL,"set header [0x%08x]\n",get_bootload_off_addr());
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), get_bootload_off_addr());
  app_printf(PRINTF_INFO_LEVEL,"set header [0x%08x]\n", get_maincode_off_addr());
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), get_maincode_off_addr());
  dm_app_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), DM_APP_BLOCK_ID)
                           - get_flash_addr();
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID),dm_app_addr);

  //load customer hw config
  dm_load_customer_hw_cfg();
  hw_cfg =dm_get_hw_cfg_info();
  return SUCCESS;
}
static void drv_cfg_init(void)
{
  //load ui resource
  load_ui_resource();

  //load customer sys config
  dm_load_customer_sys_cfg();
}

static int drv_uio_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  /*lint -e123 */
#ifndef WIN32
  ret = ATTACH_DRIVER(UIO, concerto, default, concerto);//ATTACH_DRIVER(UIO, concerto, default, concerto);
#else
  ret = ATTACH_DRIVER(UIO, magic, default, gpio);
#endif
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);
  uio_init();

  return ret;
}
static int drv_smc_init(const drvsvc_handle_t* p_public_drvsvc)
{
  #ifdef ENABLE_CA
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  scard_open_t smc_open_params ={0};
  u32 reg = 0, val = 0;

  /* SMC */
  // PINMUX
  reg = R_IE1_SEL;
  val = *((volatile u32 *)reg);
  val |= 0x1D << 10;
  *((volatile u32 *)reg) = val;

  reg = R_PIN5_SEL;
  val = *((volatile u32 *)reg);
  val &= ~0x3FF;
  *((volatile u32 *)reg) = val;

  // SMC device 0
  ret = smc_attach_concerto("concerto_smc0");
  MT_ASSERT(ret == SUCCESS);
  p_dev = (scard_device_t *) dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_smc0");
  MT_ASSERT(NULL != p_dev);
  smc_open_params.p_drvsvc = (void *)p_public_drvsvc;
  smc_open_params.smc_id = SMC0_ID;
  smc_open_params.smc_op_pri = DRV_SMC_TASK_PRIORITY;
  smc_open_params.phyclk = 0;
  smc_open_params.convention = 0;
  smc_open_params.convt_value = 0;

  smc_open_params.detect_pin_pol = 0;
  smc_open_params.vcc_enable_pol = 0;

  smc_open_params.iopin_mode = 1; // external pull-up 5V
  smc_open_params.rstpin_mode = 1; // external pull-up 5V
  smc_open_params.clkpin_mode = 1; // external pull-up 5V

  smc_open_params.read_timeout = 2500;

  smc_open_params.smc_op_stksize = 4096;
  ret = dev_open(p_dev, &smc_open_params);
  MT_ASSERT(SUCCESS == ret);

  app_printf(PRINTF_DEBUG_LEVEL,"\nSmart Card 0 Dev Open\n");
  #endif

  return SUCCESS;
}

static int drv_hdmi_init(const drvsvc_handle_t* p_public_drvsvc)
{
  hdmi_driver_attach((void *)p_public_drvsvc);

  return SUCCESS;
}

static int drv_audio_init(const drvsvc_handle_t* p_public_drvsvc)
{
  /* AUDIO */
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  aud_cfg_t aud_cfg = {0};
  u32 size = 0;
  u32 align = 0;

  /*lint -e123 */
  ret = ATTACH_DRIVER(AUDIO_VSB, concerto, default, default);
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  MT_ASSERT(NULL != p_dev);

  //aud_cfg.p_drvsvc = p_public_drvsvc;
  aud_cfg.stack_size = 4 * KBYTES;
  aud_cfg.task_prio = DRV_HDMI_TASK_PRIORITY;
  aud_cfg.lock_mode = OS_MUTEX_LOCK;

  aud_cfg.p_drvsvc = (void *)p_public_drvsvc;
  ret = dev_open(p_dev, &aud_cfg);
  MT_ASSERT(SUCCESS == ret);

  aud_stop_vsb(p_dev);
  aud_get_buf_requirement_vsb(p_dev, &size, &align);
  MT_ASSERT(g_audio_fw_cfg_size >= size);
  aud_set_buf_vsb(p_dev, g_audio_fw_cfg_addr, size);

  return SUCCESS;
}
static int drv_video_init(void)
{
  RET_CODE ret;
  vdec_cfg_t vdec_cfg = {0};
  void *p_video = NULL;
  //u32 size = 0;
  //u32 align = 0;
  vdec_buf_policy_t policy = VDEC_OPENDI_64M;

  /*lint -e123 */
  ret = ATTACH_DRIVER(VDEC, concerto, default, default);
  /*lint +e123 */
  MT_ASSERT(ret == SUCCESS);

  p_video = (void *)dev_find_identifier(NULL
   , DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);

  vdec_cfg.is_autotest = FALSE;
  ret = dev_open(p_video, &vdec_cfg);
  MT_ASSERT(SUCCESS == ret);
  app_printf(PRINTF_DEBUG_LEVEL,"Init Vdec success\n");

  vdec_stop(p_video);

#ifdef MIN_AV_64M
  #ifdef MIN_AV_SDONLY
    policy = VDEC_SDINPUT_ONLY;
  #else
    if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))
      policy = VDEC_OPENDI_64M;
    else
      policy = VDEC_BUFFER_AD_UNUSEPRESCALE;
  //policy = VDEC_OPENDI_64M;
  #endif
#else
  #ifdef VDEC_11DPB
    policy = VDEC_QAULITY_AD_128M_UNUSEPRESCALE;
  #else
    policy = VDEC_OPENDI_64M;
  #endif
#endif
  //vdec_get_buf_requirement(p_video, policy, &size, &align);
  //APPLOGA("policy %d vdec buffer size is : 0x%x\n",policy,size);
  //MT_ASSERT(g_video_fw_cfg_size >= size);
  //OS_PRINTF("g_video_fw_cfg_addr :0x%x\n",g_video_fw_cfg_addr);
  vdec_set_buf(p_video, policy, g_video_fw_cfg_addr);

#ifndef WIN32
  vdec_do_avsync_cmd(p_video,AVSYNC_NO_PAUSE_SYNC_CMD,0);
#endif
  return ret;
}
static int drv_dmx_init(void)
{
#ifdef MXL241
  RET_CODE ret;
  void *p_pti0 = NULL;
  void *p_pti1 = NULL;
  dmx_config_t dmx_cfg0 = {0};
  dmx_config_t dmx_cfg1 = {0};
  *(volatile unsigned long *)0xbf156014 = 0xa0000;
  /*************PTI0 attatch************/
  dmx_concerto_attach("concerto_pti0");
  p_pti0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti0");
  MT_ASSERT(NULL != p_pti0);

  dmx_cfg0.input_port_used[1] = TRUE;
  dmx_cfg0.ts_input_cfg[1].input_way = 0;
  dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
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
#elif defined(TDA10028)
  RET_CODE ret;
  void *p_pti0 = NULL;
  void *p_pti1 = NULL;
  dmx_config_t dmx_cfg0 = {0};
  dmx_config_t dmx_cfg1 = {0};
  *(volatile unsigned long *)0xbf156014 = 0xa0000;
  /*************PTI0 attatch************/
  dmx_concerto_attach("concerto_pti0");
  p_pti0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti0");
  MT_ASSERT(NULL != p_pti0);
  mtos_printk("concerto_pti0 connect\n");
  dmx_cfg0.input_port_used[1] = TRUE;
  dmx_cfg0.ts_input_cfg[1].input_way = 0;
  dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
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
#else
  RET_CODE ret;
  void *p_pti0 = NULL;
  void *p_pti1 = NULL;
  dmx_config_t dmx_cfg0 = {0};
  dmx_config_t dmx_cfg1 = {0};

#ifndef WIN32
  if(g_customer.customer_id == CUSTOMER_KONKA
    || g_customer.customer_id == CUSTOMER_HUANGSHI
    || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
  {
    *(volatile unsigned long *)0xbf156014 = 0xa0000;
    *(volatile unsigned long *)0xbf15601c = 0x111100;
    *(volatile unsigned long *)0xbf156308 = 0xfff80000;
  }
  else
  {
    *(volatile unsigned long *)0xbf156014 = 0xa0000;
    *(volatile unsigned long *)0xbf15601c = 0x1111a5;
    *(volatile unsigned long *)0xbf156308 = 0xfcf80000;
  }
#endif
  /*************PTI0 attatch************/
#ifdef WIN32
  ret = ATTACH_DRIVER(DMX, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);
  p_pti0 = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
#else
  ret = dmx_concerto_attach("concerto_pti0");
  MT_ASSERT(ret == SUCCESS);
  p_pti0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti0");
#endif
  MT_ASSERT(NULL != p_pti0);
#ifndef DTMB_PROJECT
  if(g_customer.customer_id == CUSTOMER_KONKA)
  {
  dmx_cfg0.input_port_used[1] = TRUE;
  dmx_cfg0.ts_input_cfg[1].input_way = 0;
  dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
  }
  else
  {
  dmx_cfg0.input_port_used[0] = TRUE;
  dmx_cfg0.ts_input_cfg[0].input_way = 1;
  dmx_cfg0.ts_input_cfg[0].local_sel_edge = 1;
  dmx_cfg0.ts_input_cfg[0].error_indicator = 0;
  dmx_cfg0.ts_input_cfg[0].start_byte_mask = 0;
  }
#else
  {
  dmx_cfg0.input_port_used[1] = TRUE;
  dmx_cfg0.ts_input_cfg[1].input_way = 0;
  dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
  }
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
#endif
  /*************PTI1 attatch************/
#ifndef WIN32
  dmx_concerto_attach("concerto_pti1");
  p_pti1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
  MT_ASSERT(NULL != p_pti1);

  dmx_cfg1.input_port_used[1] = TRUE;
  dmx_cfg1.ts_input_cfg[1].input_way = 0;
  dmx_cfg1.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg1.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg1.ts_input_cfg[1].start_byte_mask = 0;

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

  return SUCCESS;
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
#ifdef MIN_AV_64M
#ifdef MIN_AV_SDONLY
  disp_cfg.b_wrback_422 = FALSE;
#else
  disp_cfg.b_wrback_422 = TRUE;
#endif
#else
  disp_cfg.b_wrback_422 = FALSE;
#endif
#ifdef MIN_AV_64M
#ifdef MIN_AV_SDONLY
  disp_cfg.b_unuse_prescale = FALSE;
#else
  disp_cfg.b_unuse_prescale = TRUE;
#endif
#else
  disp_cfg.b_unuse_prescale = FALSE;
#endif
  disp_cfg.misc_buf_cfg.sd_wb_addr = g_vid_sd_wr_back_addr;
  disp_cfg.misc_buf_cfg.sd_wb_size = g_vid_sd_wr_back_size;
  disp_cfg.misc_buf_cfg.sd_wb_field_no = VID_SD_WR_BACK_FIELD_NO;
  disp_cfg.av_ap_shared_mem = g_ap_av_share_mem_addr;
  disp_cfg.shared_mem_size = g_ap_av_share_mem_size;

  if(TRUE == disp_cfg.b_vscale)
  {
    if(disp_cfg.b_vscale_osd1 == TRUE)
    {
      osd1_vs_cfg.odd_mem_start = g_osd1_vscaler_buffer_addr;
      osd1_vs_cfg.odd_mem_end = g_osd1_vscaler_buffer_addr + g_osd1_vscaler_buffer_size;
    }
    if(disp_cfg.b_vscale_sub == TRUE)
    {
      sub_vs_cfg.odd_mem_start = g_sub_vscaler_buffer_addr;
      sub_vs_cfg.odd_mem_end = g_sub_vscaler_buffer_addr + g_sub_vscaler_buffer_size;
    }
    disp_cfg.p_osd0_vscale_cfg = &osd0_vs_cfg;
    disp_cfg.p_osd1_vscale_cfg = &osd1_vs_cfg;
    disp_cfg.p_sub_vscale_cfg = &sub_vs_cfg;
  }
  if(TRUE == disp_cfg.b_di_en)
  {
    disp_cfg.misc_buf_cfg.di_addr = g_vid_di_cfg_addr;
    disp_cfg.misc_buf_cfg.di_size = g_vid_di_cfg_size;
  }
  disp_cfg.stack_size = DISP_HDMI_NOTIFY_TASK_STK_SIZE;
  disp_cfg.task_prio = DISP_HDMI_NOTIFY_TASK_PRIORITY;
  disp_cfg.lock_type = OS_MUTEX_LOCK;

  app_printf(PRINTF_DEBUG_LEVEL,"\ndisp_open begin\n");
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

  app_printf(PRINTF_DEBUG_LEVEL,"\ndisp_open ok\n");
  //sys_status_reload_environment_setting(FALSE);
  disp_mode_config(p_disp);
  //minnan add cfg vdac

#ifndef WIN32

  disp_vdac_config(p_disp);

  disp_rect.w = 1280;
  disp_rect.h = 720;
  disp_set_graphic_size(p_disp,&disp_rect);
  if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
    disp_set_bright(p_disp, DISP_CHANNEL_SD, 60);
  app_printf(PRINTF_DEBUG_LEVEL,"disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);
#endif

  return ret;
}
static int drv_low_power_manager_init(void)
{
  void *p_dev_lpm = NULL;
  lpower_cfg_t lpm_cfg = {0};
  RET_CODE ret;

  ret = lpower_concerto_attach("standby");
  p_dev_lpm = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"standby");
  MT_ASSERT(ret == SUCCESS);
  ret = dev_open(p_dev_lpm, &lpm_cfg);
  MT_ASSERT(SUCCESS == ret);

  return ret;
}

static void drv_mute_contrl()
{
  //静音电路的gpio置高，cvbs输出有声?
  gpio_io_enable(62, TRUE);
  gpio_set_dir(62, GPIO_DIR_OUTPUT);
  gpio_set_value(62, GPIO_LEVEL_HIGH);
}

#ifdef ENABLE_NETWORK
static int drv_networt_init()
{
  RET_CODE ret;
  void *p_dev = NULL;
  modem_cfg_t cfg = {0};
  ethernet_cfg_t ethcfg_cdc = {0};
  ethernet_cfg_t ethcfg_1 = {0};


  {
    BOX_INFO_T box_info={0};
    box_info.custom_id = 0x8c736eb8;
    set_the_box_hw_info(&box_info) ;
  }


  {
    /*config ethernet: use mt mac controller*/
    ethernet_cfg_t ethcfg = {0};
    void *p_dev_eth_mt = NULL;
    ethernet_concerto_attach("mt_eth");
    p_dev_eth_mt = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
    MT_ASSERT(p_dev_eth_mt != NULL);
    ret = dev_open(p_dev_eth_mt, (void *)&ethcfg);
    MT_ASSERT(SUCCESS == ret);
  }
#if 1
  {
    /*config WIFI: use usb wifi adapter: rtl8188eus*/
    ethernet_cfg_t ethcfg_wifi = {0};
    wifi_task_config_t cfg_8188;

    cfg_8188.Wifi_Prio_H = WIFI_1_TASK_H_THREAD;
    cfg_8188.Wifi_Prio_M1 = WIFI_1_TASK_M1_THREAD;
    cfg_8188.Wifi_Prio_M2 = WIFI_1_TASK_M2_THREAD;
    cfg_8188.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
    cfg_8188.wifi_net_link_task_prio = NET_LINK_TASK;
    ethcfg_wifi.p_priv_confg = (u8*)&cfg_8188;

    void *p_dev = NULL;
    rtl8188eus_ethernet_attach(RTL_REALTEK_WIFI_NAME);
    p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)RTL_REALTEK_WIFI_NAME);
    MT_ASSERT(p_dev != NULL);
    ret = dev_open(p_dev, (void *)&ethcfg_wifi);
    MT_ASSERT(SUCCESS == ret);
  }
#endif

#ifdef WIFI_MTK5370_SUPPORT
{

    /*config WIFI: use usb wifi adapter: mtk5370*/
    ethernet_cfg_t ethcfg_wifi_5370 = {0};
    wifi_task_config_t cmm_cfg_5370;

    cmm_cfg_5370.Wifi_Prio_H = WIFI_TASK_H_THREAD;
    cmm_cfg_5370.Wifi_Prio_M1 = WIFI_TASK_M1_THREAD;
    cmm_cfg_5370.Wifi_Prio_M2 = WIFI_TASK_M2_THREAD;
    cmm_cfg_5370.Wifi_Prio_M3 = WIFI_TASK_M3_THREAD;
    cmm_cfg_5370.Wifi_Prio_L1 = WIFI_TASK_L1_THREAD;
    cmm_cfg_5370.Wifi_Prio_L2 = WIFI_TASK_L2_THREAD;
    cmm_cfg_5370.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
    cmm_cfg_5370.wifi_net_link_task_prio= NET_LINK_TASK;

    ethcfg_wifi_5370.p_priv_confg = (u8*)&cmm_cfg_5370;

    void *p_dev = NULL;
    cmm_wifi_attach(MTK_RALINK_WIFI_NAME);
    p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)MTK_RALINK_WIFI_NAME);
    MT_ASSERT(p_dev != NULL);
    ret = dev_open(p_dev, (void *)&ethcfg_wifi_5370);
    MT_ASSERT(SUCCESS == ret);

}
#else
{
    /*config WIFI: use usb wifi adapter: mtk7601*/
    ethernet_cfg_t ethcfg_wifi_7601 = {0};
    wifi_task_config_t cmm_cfg;
    extern s32 cmm_wifi_attach_rt7601(char *devname);

    cmm_cfg.Wifi_Prio_H = WIFI_TASK_H_THREAD;
    cmm_cfg.Wifi_Prio_M1 = WIFI_TASK_M1_THREAD;
    cmm_cfg.Wifi_Prio_M2 = WIFI_TASK_M2_THREAD;
    cmm_cfg.Wifi_Prio_M3 = WIFI_TASK_M3_THREAD;
    cmm_cfg.Wifi_Prio_L1 = WIFI_TASK_L1_THREAD;
    cmm_cfg.Wifi_Prio_L2 = WIFI_TASK_L2_THREAD;
    cmm_cfg.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
    cmm_cfg.wifi_net_link_task_prio= NET_LINK_TASK;

    ethcfg_wifi_7601.p_priv_confg = (u8*)&cmm_cfg;

    void *p_dev = NULL;
    cmm_wifi_attach_rt7601(MTK_RALINK7601_WIFI_NAME);
    p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)MTK_RALINK7601_WIFI_NAME);
    MT_ASSERT(p_dev != NULL);
    ret = dev_open(p_dev, (void *)&ethcfg_wifi_7601);
    MT_ASSERT(SUCCESS == ret);
}
#endif

#if 1
  app_printf(PRINTF_DEBUG_LEVEL,"ttyUSB attaching...n");
  if(SUCCESS == usb_tty_attach("ttyUSB"))
  {
    p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"ttyUSB");
    MT_ASSERT(p_dev != NULL);
    dev_open(p_dev, (void *)NULL);
    app_printf(PRINTF_DEBUG_LEVEL,"ttyUSB done \n");
  }
#endif

#if 1
  app_printf(PRINTF_DEBUG_LEVEL,"usb_serial_modem attatching...\n");
  ret = modem_usb_3g_dongle_attach("usb_serial_modem");
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(NULL != p_dev);

  cfg.ppp_task_prio = PPP_TASK_PRIORITY;
  cfg.modem_task_prio = MODEM_TASK_PRIORITY_3G;
  ret = dev_open(p_dev, &cfg);
  MT_ASSERT(SUCCESS == ret);
  app_printf(PRINTF_DEBUG_LEVEL,"usb_serial_modem\n");
#endif

#if 1
  app_printf(PRINTF_DEBUG_LEVEL,"3g-cdc-ether attatching...\n");
  usb_3g_cdc_ether_attach("3g-cdc-ether");
  memset(&ethcfg_cdc, 0, sizeof(ethernet_cfg_t));
  ethcfg_cdc.task_prio = CDC_ETHER_RECV_TASK_PRIORITY;
  ethcfg_cdc.stack_size = 4096;
  p_dev = (ethernet_device_t *)dev_find_identifier(p_dev, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_3G_ETHER);
  MT_ASSERT(p_dev != NULL);

  ret = dev_open(p_dev, (void *)&ethcfg_cdc);
  MT_ASSERT(ret == SUCCESS);
  app_printf(PRINTF_DEBUG_LEVEL,"3g-cdc-ether done\n");

#endif

  {
    mount_ramfs();
    ethcfg_1.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
    lwip_stack_init(&ethcfg_1);
  }
  pppInit();

  register_rtmp_stream();
  register_rtmp_protocol(0);
  register_rtsp_stream();

  return ret;
}
#endif

#ifndef WIN32
static void drv_init_sctrl()
{
  RET_CODE ret = SUCCESS;
  void *p_dev_sctrl0 = NULL;
  void *p_dev_sctrl1 = NULL;
  void *p_dev_sctrl2 = NULL;

  /*!
  security
  */
  /*    sctrl   */
  sctrl_cfg_t sctrl_open_params = {0};
  sctrl_open_params.lock_mode = OS_LOCK_DEF;
  ret = sctrl_attach("sctrl_subch2");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch2");
  ret = dev_open(p_dev_sctrl0, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  ret = sctrl_attach("sctrl_subch3");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch3");
  ret = dev_open(p_dev_sctrl1, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  ret = sctrl_attach("sctrl_subch4");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl2 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch4");
  ret = dev_open(p_dev_sctrl2, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  /* secure moudles init */
  ret = hal_secure_ModuleInit();
  MT_ASSERT(ret == SUCCESS);

}
#endif
static void drv_flash_protect()
{
  /*protect flash*/
  void *p_dev = NULL;
  charsto_prot_status_t st_set;

  memset(&st_set,0,sizeof(charsto_prot_status_t));
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);
  st_set.prt_t = PRT_LOWER_1_8;
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
}
static void show_startup_iFram()
{
  RET_CODE ret = SUCCESS;
  u8 *addr = NULL;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_VDEC_VSB);
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);

  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),START_LOGO_BLOCK_ID);

  addr = (u8*)mtos_malloc(size);
  MT_ASSERT(addr != NULL);
  dm_read(class_get_handle_by_id(DM_CLASS_ID),START_LOGO_BLOCK_ID, 0, 0, size, addr);

  dmx_av_reset(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI));

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);

  vdec_set_data_input(p_video_dev,1);
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_USER);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)addr, size);
  MT_ASSERT(SUCCESS == ret);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, TRUE);
  vdec_set_data_input(p_video_dev,0);

  if(addr != NULL)
    mtos_free(addr);
}

#ifndef WIN32
typedef unsigned long (*p_entry)(int, char * const[]);
extern unsigned long _do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);
#define BARCODE_RUN_ADDR	 (0x81000000)
#define BARCODE_SIZE	(0x80000)
#define BARCODE_OUT_ZIE (0x80000)

static void jump_app(u8 block_id)
{
  u8 *p_data_buffer = NULL;
  dmh_block_info_t block_dm_info = {0};
  u8 *p_data_buffer2 = NULL;
  u8 *p_zip_sys = NULL;
  u32 out_size = 0;


  dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) ;
  APPLOGA("block_id %d size 0x%x \n",block_id,block_dm_info.size);

  //dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),0x86,&block_dm_info) ;
  //OS_PRINTF("%s size 0x%x \n",__FUNCTION__,block_dm_info.size);

  p_data_buffer = (u8 *)mtos_malloc(BARCODE_SIZE);
  if(p_data_buffer == NULL)
  {
  	APPLOGA("p_data_buffer malloc fail \n");
	return;
  }
  memset(p_data_buffer,0,BARCODE_SIZE);
  p_zip_sys = (u8 *)mtos_malloc(320 * KBYTES);
  if(p_zip_sys == NULL)
  {
  	APPLOGA("p_zip_sys malloc fail \n");
	return;
  }
  memset(p_zip_sys,0,320 * KBYTES);
  p_data_buffer2 = (u8 *)(BARCODE_RUN_ADDR);
  memset(p_data_buffer2,0,BARCODE_SIZE);
  out_size = BARCODE_OUT_ZIE;

  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, block_dm_info.size, p_data_buffer);

  APPLOGA("\nblock_id :%d data :\n",block_id);

  init_fake_mem_lzma(p_zip_sys, 320 * KBYTES);
  lzma_decompress((void *)p_data_buffer2, &out_size, p_data_buffer, block_dm_info.size);
  APPLOGA("\nblock_id  decompres data :\n");
  APPLOGA("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer2[0],p_data_buffer2[1],p_data_buffer2[2],p_data_buffer2[3],p_data_buffer2[4]);

  hal_dcache_flush((void *)p_data_buffer2, out_size);

  _do_go_exec((p_entry)BARCODE_RUN_ADDR,0,0);

}
#define   SN_CLIRNT_SIG   "JASNJASNJASNJASN"
#define   SN_CLIRNT_RES   0xA5
#define   SN_TIMEOUT      3
static RET_CODE sn_client_process(void)
{
  u32 ret = 0, i = 0;
  u8 ch = 0;
  #if 0
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  void * p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 5, sizeof(u8), &hkey);
  #endif

  uart_flush(0);
  APPLOGA(SN_CLIRNT_SIG);

  for(i = 0; i < SN_TIMEOUT; i++)
  {
    #if 0
    uio_get_code(p_dev, &code);
    if(hkey != 0xFF)
    {
      if((code & 0x100) && (hkey == (code & 0xFF)))
      {
        jump_to_usbupgrade();
        return ERR_FAILURE;
      }
    }
    #endif
    ret = uart_read_byte(0, &ch, 100);
    if((ret == 0) && (ch == SN_CLIRNT_RES))
    {
      jump_app(UPG_TOOL_BLOCK_ID);
      return SUCCESS;
    }
  }
  return ERR_FAILURE;
}
#endif
void drv_init_concerto(void)
{
  RET_CODE ret;
  const drvsvc_handle_t *p_public_drvsvc;

#ifndef WIN32
  hal_otp_init();
#endif

  /* DMA */
  ret = hal_dma_init();
  MT_ASSERT(ret == SUCCESS);

  p_public_drvsvc = drv_public_svr_init();
  MT_ASSERT(p_public_drvsvc != NULL);

  ret = drv_dm_init();
  MT_ASSERT(ret == SUCCESS);

#ifndef WIN32
  ap_ota_check();

  ret = drv_i2c_init();
  MT_ASSERT(ret == SUCCESS);
  ret = drv_hdmi_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);
  set_board_config();
#endif
  ret = drv_uio_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_dmx_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_video_init();
  MT_ASSERT(ret == SUCCESS);
  sys_status_init();
  ret = drv_display_init();
  MT_ASSERT(ret == SUCCESS);

  mtos_task_sleep(1500);
  /*show iFrame*/
  show_startup_iFram( );

  ret = drv_gpe_init();
  MT_ASSERT(ret == SUCCESS);
  ret = drv_jpeg_init();
  MT_ASSERT(ret == SUCCESS);

  /*show jpeg*/

  drv_cfg_init();

#ifndef WIN32
  /*check barcode*/
  sn_client_process();
  APPLOGA("\n");
#endif


  ret = drv_audio_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);

  ret = drv_nim_init();
  MT_ASSERT(ret == SUCCESS);


#ifndef WIN32
  ret = drv_smc_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);
  ret = drv_vbi_init();
  MT_ASSERT(ret == SUCCESS);
  /* secure moudles init */
  drv_init_sctrl();
#endif

#ifndef WIN32
  ret = drv_low_power_manager_init();
  MT_ASSERT(ret == SUCCESS);
#endif

#ifndef WIN32
#ifdef ENABLE_NETWORK
  drv_networt_init();
#endif
  /*board config */
  drv_mute_contrl();
  drv_flash_protect();
#endif
}

void register_dvb_tables(void)
{
  dvb_register_t reg_param = {0};
  dvb_t *p_dvb = NULL;

  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ecm;
  reg_param.request_proc = request_ecm;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_ECM;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pat;
  reg_param.request_proc = request_pat;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PAT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_sdt_dvbc;
  reg_param.request_proc = request_sdt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_SDT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

#ifdef QUICK_SEARCH_SUPPORT
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_sdt_dvbc;
  reg_param.request_proc = request_sdt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_SDT_OTHER;
  p_dvb->register_table(p_dvb, &reg_param);
#endif

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_nit_india;
  #ifdef REQUEST_NIT_EXTEND_FILTER
  reg_param.request_proc = request_nit_extend_filter;
  #else
   reg_param.request_proc = request_nit;
  #endif
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pmt_1;
  reg_param.request_proc = request_pmt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PMT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ts_packet;
  reg_param.request_proc = request_ts_packet;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = MPEG_TS_PACKET;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT multi-sec mode request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode_user;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode_user;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode_user;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL_51;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_bat_india;
  #if 1
  reg_param.request_proc = request_bat_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  #else
  reg_param.request_proc = request_bat_section_india;
  reg_param.req_mode = DATA_SINGLE;
  #endif
  //reg_param.request_proc = request_bat_section;
  //reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_BAT;
  p_dvb->register_table(p_dvb, &reg_param);

  //dsmcc msg section:single
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_dsmcc_dsi_dii_section;
  reg_param.request_proc = request_dsmcc_dsi_dii_section;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_dvb->register_table(p_dvb, &reg_param);
#ifdef CHANNEL_FOLLOWING_TABLE
  //dsmcc msg section:single
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_channel_sec;
  reg_param.request_proc = request_channel_sec;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_RUN_STATUS;
  p_dvb->register_table(p_dvb, &reg_param);
#endif
}

#ifdef ENABLE_CA
extern void cas_manager_policy_init(void);
#endif

u8 *p_dvb2_mem = NULL;

void middleware_init(void)
{
  handle_t dm_handle = NULL;
  dvb_t *p_dvb = NULL;
  u32 size = 0;
  u8 *p_buf = NULL;
  eva_init_para_t eva_para = {0};
  pnp_svc_init_para_t pnp_param = {0};

  mdl_init();

  eva_para.debug_level = E_PRINT_ERROR;
  eva_para.eva_sys_task_prio_start = EVA_SYS_TASK_PRIORITY_START;
  eva_para.eva_sys_task_prio_end = EVA_SYS_TASK_PRIORITY_END;
  eva_init(&eva_para);

  eva_filter_factory_init();
#if ENABLE_MUSIC_PICTURE
  eva_register_filter(FILE_SOURCE_FILTER, fsrc_filter_create);
  eva_register_filter(SUBT_STATION_FILTER, subt_station_filter_create);
  eva_register_filter(JPEG_FILTER, pic_filter_create);
  eva_register_filter(PIC_RENDER_FILTER, pic_render_filter_create);
  eva_register_filter(MP3_TRANSFER_FILTER, mp3_transfer_filter_create);
  eva_register_filter(MP3_DECODE_FILTER, mp3_decode_filter_create);
  eva_register_filter(FILE_SINK_FILTER, fsink_filter_create);
  eva_register_filter(TS_PLAYER_FILTER, ts_player_filter_create);
  eva_register_filter(AV_RENDER_FILTER, av_render_filter_create);
  eva_register_filter(RECORD_FILTER, record_filter_create);
  eva_register_filter(LRC_PARSE_FILTER, lrc_filter_create);
  eva_register_filter(STR_FILTER, str_filter_create);
#ifdef ENABLE_FILE_PLAY
  eva_register_filter(MPLAYER_AUD_FILTER, mplayer_aud_filter_create);
#endif

#endif

#ifdef ENABLE_NETWORK
  eva_register_filter(WEATHER_FILTER, weather_filter_create);
  eva_register_filter(NET_SOURCE_FILTER, net_src_filter_create);
  eva_register_filter(GOOGLE_MAP_FILTER, google_map_filter_create);
  eva_register_filter(VOD_FILTER, vod_filter_create);
  eva_register_filter(ALBUMS_FILTER, albums_filter_create);
  eva_register_filter(NEWS_FILTER, news_filter_create);
  eva_register_filter(NET_UPG_FILTER, net_upg_filter_create);
  eva_register_filter(FLASH_SINK_FILTER, flash_sink_filter_create);
  eva_register_filter(NET_MUSIC_FILTER, net_music_filter_create);
#endif

   eva_register_filter(EPG_FILTER, epg_filter_create);
  eva_register_filter(DEMUX_FILTER, dmx_filter_create);
  eva_register_filter(OTA_FILTER, ota_filter_create);
  eva_register_filter(FLASH_SINK_FILTER,flash_sink_filter_create);

  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(dm_handle != NULL);

  // Whether cache is enabled, it is up to the configuration from UI
  OS_PRINTF("dm set cache\n");
  dm_set_cache(dm_handle, IW_TABLE_BLOCK_ID, MAX_TABLE_CACHE_SIZE);
  dm_set_cache(dm_handle, IW_VIEW_BLOCK_ID, MAX_VIEW_CACHE_SIZE);
  OS_PRINTF("dm set cache is done\n");
  {
    nc_svc_cfg_t nc_cfg = {0};
    nc_cfg.b_sync_lock = FALSE;
#ifdef DTMB_PROJECT
    nc_cfg.lock_mode = SYS_DTMB;
#else
    nc_cfg.lock_mode = SYS_DVBC;
#endif
    nc_cfg.priority = MDL_NIM_CTRL_TASK_PRIORITY;
    nc_cfg.stack_size = MDL_NIM_CTRL_TASK_STKSIZE;
    nc_cfg.auto_iq = TRUE;
    nc_cfg.mutex_type = OS_MUTEX_LOCK;
    nc_cfg.wait_ticks = 100;
    nc_cfg.maxticks_cnk=500;
    nc_svc_init(&nc_cfg, NULL);
  }

{
    u8 tuner_id = 0, ts_in = 0;
    class_handle_t nc_handle = NULL;
    nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);
    dvb_init_1(MDL_DVB_TASK_PRIORITY, MDL_DVB_TASK_STKSIZE,ts_in);
}

  nvod_mosaic_buf_init(30 * KBYTES);
  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  register_dvb_tables();
  size = p_dvb->get_mem_size(p_dvb, TRUE, MAX_FILTER_NUM, 128);


  p_dvb2_mem = mtos_malloc(10);
  MT_ASSERT(p_dvb2_mem != NULL);
  memset(p_dvb2_mem, 0x3b, 10);
  mem_show();


  p_buf = mtos_align_malloc(size, 8);
  p_dvb->start(p_dvb, p_buf, size, TRUE, MAX_FILTER_NUM, 128);

  {
      m_svc_init_para_t m_init_param = {0};

  //m_svc_t * p_svc = NULL;
  m_init_param.nstksize = MDL_MONITOR_TASK_STKSIZE;
  m_init_param.service_prio = MDL_MONITOR_TASK_PRIORITY;
  dvb_monitor_service_init(&m_init_param);
  //p_svc = class_get_handle_by_id(M_SVC_CLASS_ID);

  //p_svc->get_svc_instance(class_get_handle_by_id(M_SVC_CLASS_ID), APP_FRAMEWORK);
}
#if ENABLE_MUSIC_PICTURE
  {
    dmm_init_para_t dmm_para = {0};
    dmm_para.channel_num = 128;
    dmm_init(&dmm_para);
  }

#endif
  db_dvbs_init();

  #ifdef ENABLE_CA
  cas_manager_policy_init();
  #endif

  //sys_status_init();
  //time initial
  {
    BOOL p_status = FALSE;
    utc_time_t p_time;

    sys_status_get_status(BS_UNFIRST_UNPOWER_DOWN, &p_status);
    sys_status_get_utc_time(&p_time);
    time_init(&p_time);
    sys_status_set_time_zone();
    sys_status_set_status(BS_UNFIRST_UNPOWER_DOWN, FALSE);
    sys_status_save();
    //set time zone(should be called after time initial.)
  }
  {
    avc_cfg_t avc_cfg = {0};
#ifdef WIN32
    avc_cfg.disp_fmt = VID_SYS_PAL;
    avc_cfg.disp_chan = DISP_CHANNEL_SD;
#else
    avc_cfg.disp_fmt = VID_SYS_1080I_50HZ;
    avc_cfg.disp_chan = DISP_CHANNEL_HD;
#endif
    avc_init_1(&avc_cfg);
  }

#ifdef NVOD_ENABLE
  nvod_data_init();
#endif
  link_gb2312_maptab(NULL, 0);
#ifdef WIN32
  s32_block_int();
#else
#ifdef ENABLE_USB_CONFIG
  block_driver_attach();
  ufs_dev_init();   //register callback for block layer
#endif
#endif
#ifdef WIN32
  vfs_win32_init();
#else
#ifdef ENABLE_NETWORK
  vfs_uri_init();
#else
#ifdef ENABLE_USB_CONFIG
  vfs_mt_init();
#endif
#endif
#endif

  file_list_init2(FILE_LIST_MAX_SUPPORT_CNT);
  {
    //extern void   usb_monitor_attach(void);

    pnp_param.nstksize = MDL_PNP_SVC_TASK_STKSIZE;
    pnp_param.service_prio = MDL_PNP_SVC_TASK_PRIORITY;
    pnp_service_init(&pnp_param);

    //usb_monitor_attach();
  }
#ifdef ENABLE_PVR_REC_CONFIG
  mul_pvr_init();
#endif
#ifdef WIN32
  {
    vfs_mount('C');
#ifdef ENABLE_NETWORK
    socket_init();
#endif
  }
#endif


#ifdef ENABLE_NETWORK
#ifndef WIN32
  {
    net_svc_init_para_t net_svc_cfg;
    net_svc_t *p_net_svc = NULL;
    service_t *p_net = NULL;


    net_svc_cfg.service_prio = MDL_NET_SVC_TASK_PRIORITY;
    net_svc_cfg.nstksize = MDL_NET_SVC_TASK_STKSIZE;

    net_svc_cfg.is_ping_url = TRUE;
    net_svc_cfg.enable_auto_ping = TRUE;
    net_svc_cfg.monitor_interval= 2000;
    strcpy(net_svc_cfg.param.ping_url_1, "www.baidu.com");

    net_service_init(&net_svc_cfg);

    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    p_net = p_net_svc->get_svc_instance(p_net_svc, APP_FRAMEWORK);

    ui_set_net_svc_instance(p_net);
  }
#endif

  net_source_svc_init(NET_SOURCE_SVC_PRIO, 32 * KBYTES);

  mul_vod_init();
  mul_albums_init();

  {
    httpd_svc_init_para_t  para = {0};
    para.service_prio = MDL_HTTPD_SVC_TASK_PRIORITY;
    para.nstksize = 500 * 1024;
    httpd_service_init(&para);
  }

  mul_news_init();
  mul_net_upg_init();
#ifndef WIN32
  mul_net_music_init();
#endif
#endif
}

#ifdef ENABLE_NETWORK
static void live_tv_db_init()
{
  livetv_db_init_t p_param;

  u32 size = livetv_db_size_get();

  OS_PRINTF("init live db size %d\n", size);

  p_param.p_block_buf = mtos_malloc(size);
  p_param.buf_size = size;
  p_param.max_group = MAX_LIVE_GROUP;
  p_param.max_prg = MAX_LIVE_PRG;
  p_param.db_block_id = 0xAA;
  livetv_db_init(&p_param);
}
#endif

void app_init(void)
{
  RET_CODE ret;
  ap_frm_init_param_t param = {0};
  u32 ap_frm_msgq_id = 0;

/*lint -e446 for pc-lint*/
  app_info_t app_array[] =
  {
    {APP_PLAYBACK, construct_ap_playback_1(construct_pb_policy()), "app playback",
     AP_PLAYBACK_TASK_PRIORITY, AP_PLAYBACK_TASK_STKSIZE},
     #ifdef SONGXIA_RC
      {APP_UIO, construct_ap_songxia_uio(), "app uio", AP_UIO_TASK_PRIORITY, AP_UIO_TASK_STKSIZE},
     #else
      {APP_UIO, construct_ap_uio(construct_ap_uio_policy()), "app uio", AP_UIO_TASK_PRIORITY,AP_UIO_TASK_STKSIZE},
    #endif

    #ifndef WIN32
    #ifdef ENABLE_CA
    {APP_CA, construct_ap_cas(construct_cas_policy()), "app ca", AP_CA_TASK_PRIORITY, AP_CA_TASK_STKSIZE},
    #endif
    #endif
    //{APP_OTA, construct_ap_ota(construct_ota_policy()), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_OTA, construct_virtual_nim_filter(APP_OTA,0), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_SIGNAL_MONITOR, construct_ap_signal_monintor(construct_sig_mon_policy()),
     "app singal monitor", AP_SIGN_MON_TASK_PRIORITY, AP_SIGN_MON_TASK_STKSIZE},
    {APP_TIME, construct_ap_time(construct_time_policy()), "app time", AP_TIME_TASK_PRIORITY,
     AP_TIME_TASK_STKSIZE},
    {APP_SCAN, construct_ap_scan(), "app scan", AP_SCAN_TASK_PRIORITY, AP_SCAN_TASK_STKSIZE},
    {APP_USB_UPG, construct_ap_usb_upg(construct_usb_upg_policy()), "app usb upgrade",
     AP_USB_UPG_TASK_PRIORITY, AP_USB_UPGRADE_TASK_STKSIZE},
    {APP_UPG, construct_ap_upg(construct_upg_policy()), "app upg", AP_UPGRADE_TASK_PRIORITY,
     AP_UPGRADE_TASK_STKSIZE},
  #ifdef NVOD_ENABLE
    {APP_NVOD, construct_ap_nvod(), (u8 *)"app nvod", AP_NVOD_TASK_PRIORITY, AP_NVOD_TASK_STKSIZE},
  #endif

    #ifdef TENGRUI_ROLLTITLE
    {APP_RESERVED2, construct_ap_tr_subt(construct_subt_policy()), "app trsubt", AP_TR_TITLE_TASK_PRIORITY,
     AP_TR_TITLE_TASK_STKSIZE},
  #endif
  #ifdef BROWSER_APP
    {APP_SI_MON, construct_ap_browser(construct_browser_policy()), "app browser", AP_BROWSER_TASK_PRIORITY,

    AP_BROWSER_TASK_STKSIZE}
  #endif
#if defined DESAI_56_CA || defined DESAI_52_CA || defined DESAI_40_CA 
    {APP_NIT, construct_ap_nit(), (u8 *)"app nit", AP_NIT_TASK_PRIORITY, AP_NIT_TASK_STKSIZE},
  #endif
  #ifdef UPDATE_PG_BACKGROUND
    {APP_RESERVED2,  construct_ap_table_monitor(), (u8 *)"app table monitor", AP_TABLE_MONITOR_PRIORITY, AP_TABLE_MONITOR_TASK_STKSIZE},
#endif
  };

#ifndef WIN32
  ret = app_dmx_init(AP_DMX_TASK_PRIORITY);
  MT_ASSERT(ret == SUCCESS);
#endif

/*lint +e446 for pc-lint*/
  param.p_implement_policy = construct_ap_frm_policy();
  param.p_app_array = app_array;
  param.num_of_apps = sizeof(app_array) / sizeof(app_info_t);
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;
  //param.print_level = AP_FRM_PRINT_ALL;
  ap_frm_init(&param, &ap_frm_msgq_id);
//  ap_frm_preformance_test(0x11);
  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
  epg_api_init();

#ifdef TENGRUI_ROLLTITLE
  extern void subt_pic_ap_init(void);
  subt_pic_ap_init();
#endif

#ifndef WIN32
#ifndef ABV_CA
EMON_Initialise();//because of abv uart test
#endif
#endif

#ifdef ENABLE_NETWORK
  live_tv_db_init();
#endif
}

#ifdef AUTO_FACTORY_DEFAULT
extern void check_factory_key(void);
#endif

#ifndef WIN32
static inline void __delay(unsigned int loops)
{
  __asm__ __volatile__ (
  "       .set    noreorder                               \n"
  "       .align  3                                       \n"
  "1:     bnez    %0, 1b                                  \n"
  "       subu    %0, 1                                   \n"
  "       .set    reorder                                 \n"
  : "=r" (loops)
  : "0" (loops));
}
#endif

void ap_init(void)
{
  u32 interval = 0;
  dvbc_lock_t main_tp = {0};


  #ifndef WIN32
  hal_dcache_invalidate((void *)&g_ipcfw_f,sizeof(ipc_fw_fun_set_t));
  attach_ipcfw_fun_set_concerto(&g_ipcfw_f);

  ap_ipc_init(32);   // ap ipc fifo create
#endif

#ifndef WIN32
  #if ENABLE_USB_DUMP_BREAK_LOG
  if(usb_dump_log_set_buf())
  {
   usb_dump_log_init();
  }
  else
  #endif
#endif
  {
    //customer config
    mem_cfg(MEMCFG_T_NORMAL);
    customer_config_init();
    app_printf(PRINTF_DEBUG_LEVEL,"mem_cfg\n");

    interval = mtos_ticks_get();
    drv_init_concerto();

    util_init();
    app_printf(PRINTF_DEBUG_LEVEL,"util_init\n");

    //close spdif
    if(g_customer.customer_id == CUSTOMER_KONKA)
    {
#ifndef WIN32
      u32 dtmp = 0;
      dtmp = hal_get_u32((volatile unsigned long *) 0xbf15601c);
         //OS_PRINTK("\n reg1 value:%x",dtmp);
      dtmp |= (1 << 25) ;
      dtmp &= ~(1 << 24);
      hal_put_u32((volatile unsigned long *)0xbf15601c, dtmp);
#endif
    }



    interval = mtos_ticks_get() - interval;
    app_printf(PRINTF_DEBUG_LEVEL,"driver init time = %ld ms\n", interval * 10);
    app_printf(PRINTF_DEBUG_LEVEL,"drv_init\n");

    interval = mtos_ticks_get();
    middleware_init();
    interval = mtos_ticks_get() - interval;
    app_printf(PRINTF_DEBUG_LEVEL,"mdl init time = %ld ms\n", interval * 10);
    app_printf(PRINTF_DEBUG_LEVEL,"mid_init\n");

#ifdef AUTO_FACTORY_DEFAULT
    check_factory_key();
    app_printf(PRINTF_DEBUG_LEVEL,"check_factory_key\n");
#endif

    app_init();
    app_printf(PRINTF_DEBUG_LEVEL,"app_init\n");

    sys_status_get_main_tp1(&main_tp);
    #ifdef SMSX_CA
    channel_init(main_tp.tp_freq, main_tp.tp_sym,main_tp.nim_modulate, 0, 0);
   #endif
    #ifdef TENGRUI_CA
    extern void zone_check(u32 Freq);
    OS_PRINTF("main_tp.tp_freq = %d\n", main_tp.tp_freq);
    zone_check(main_tp.tp_freq);
    #endif
    sys_status_get_main_tp1(&main_tp);
    #ifdef STARTUP_NIT_VERSION_CHECK
    extern void channel_init(u32 freq, u32 sym, u16  modulation,u16 pid, u16 table_id);
    channel_init(main_tp.tp_freq, main_tp.tp_sym,main_tp.nim_modulate, 0, 0);
    #endif
    interval = mtos_ticks_get();
    ui_init();
        //init watchdog
   #ifndef WIN32
    #if ENABLE_WATCH_DOG
     watchdog_init();
    #endif
   #endif
    interval = mtos_ticks_get() - interval;
    app_printf(PRINTF_DEBUG_LEVEL,"ui init time = %ld ms\n", interval * 10);
    app_printf(PRINTF_DEBUG_LEVEL,"ui_init\n");
  }
}
/*lint +e64 +e746*/
/*lint -e528 for pc-lint*/

