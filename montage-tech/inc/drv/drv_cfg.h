/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DRV_CFG_H__
#define __DRV_CFG_H__

/*!
 drv_attach_func
  */
typedef RET_CODE (*drv_attach_func)(char *p_name);

/*!
 video DAC output mode for board config
  */
typedef enum
{
  /*!
    VDAC power off
    */
  VDAC_POWER_OFF = 0,
  /*!
    used for CVBS output
    */
  VDAC_CVBS,
  /*!
    used for Y in YPbPr output
    */
  VDAC_Y,
  /*!
    used for Pb in YPbPr output
    */
  VDAC_PB,
  /*!
    used for Pr in YPbPr output
    */
  VDAC_PR,
  /*!
    used for Y in S-Video output
    */
  VDAC_SVIDEO_Y,
  /*!
    used for Component in S-Video output
    */
  VDAC_SVIDEO_C,
  /*!
    used for R in RGB output
    */
  VDAC_R,
  /*!
    used for G in RGB output
    */
  VDAC_G,
  /*!
    used for B in RGB output
    */
  VDAC_B,
}board_vdac_mode_t;

/*!
 video DAC output mode for board config
  */
typedef enum
{
  /*!
    no use
    */
  SCART_NO_USE = 0,
  /*!
    used for scart CVBS output
    */
  SCART_CVBS,
  /*!
    used for scart RGB output
    */
  SCART_RGB,
  /*!
    support scart RGB and CVBS output
    */
  SCART_RGB_AND_CVBS,
}board_scart_tv_mode_t;

/*!
 video DAC output mode for board config
  */
typedef enum
{
  /*!
    no use
    */
  SCART_16_9 = 0,
  /*!
    used for scart CVBS output
    */
  SCART_4_3,
  /*!
    used for scart RGB output
    */
  SCART_STB_OFF,
}board_scart_aspect_t;

/*!
 video DAC output mode for board config
  */
typedef enum
{
  /*!
    YPbPr output
    */
  VOUT_YPBPR = 0,
  /*!
    scart output
    */
  VOUT_SCART,
}board_vout_mode_t;

/*!
 board_gpio_switch_t
  */
typedef struct
{
  /*!
    gpio number used to function switch
    */
  u8 gpio_num;
  /*!
    gpio index, start from 0
    */
  u8 gpio[3];
  /*!
    if gpio_num is 1, cfg_tab[0], cfg_tab[1] and cfg_tab[2] are valid, cfg_tab[0] means the config when gpio[0] is low,
      cfg_tab[1] means the config when gpio[0] is high, cfg_tab[2] means the config when gpio[0] is hiz;
    if gpio_num is 2, cfg_tab[0], cfg_tab[1], cfg_tab[2] and cfg_tab[3] are valid,
      cfg_tab[0] means the config when gpio[0] and goio[1] both are low,
      cfg_tab[1] means the config when gpio[0] is low and goio[1] is high,
      cfg_tab[2] means the config when gpio[0] is high and goio[1] is low,
      cfg_tab[3] means the config when gpio[0] and goio[1] both are high,
    */
  u8 cfg_tab[8];
}board_gpio_switch_t;

/*!
 board_vout_scart_cfg_t
  */
typedef struct
{
  /*!
    board_scart_tv_mode_t
    */
  u8 scart_tv_mode;
  /*!
    scart dac index, start from 0
    */
  u8 scart_cvbs_dac;
  /*!
    scart dac index, start from 0
    */
  u8 scart_cvbs_r_dac;
  /*!
    scart dac index, start from 0
    */
  u8 scart_cvbs_g_dac;
  /*!
    scart dac index, start from 0
    */
  u8 scart_cvbs_b_dac;
  /*!
    scart tv mode gpio switch config
    */
  board_gpio_switch_t tv_mode_switch;
  /*!
    scart aspect mode gpio switch config
    */
  board_gpio_switch_t aspect_switch;
  /*!
    YCbCr and Scart auto detect switch
    */
  board_gpio_switch_t ycbcr_scart_auto_detect;
}board_vout_scart_cfg_t;

/*!
 board_video_cfg_t
  */
typedef struct
{
  /*!
    vdac mode, see board_vdac_mode_t
    */
  u8 vdac_mode[8];
  /*!
    scart config if support scart
    */
  board_vout_scart_cfg_t scart_cfg;
}board_video_cfg_t;

/*!
 board_video_cfg_t
  */
typedef struct
{
    /*!
    display driver config
    */
  disp_cfg_t disp_cfg;
  /*!
    board video config
    */
  board_video_cfg_t board_cfg;
   /*!
    video attach function pointer
    */
  drv_attach_func drv_attach;
}_vedio_cfg_t;

/*!
 _audio_cfg_t
  */
typedef struct
{
  /*!
    if support spdif out
    */
  u8 if_spdif_out;
  /*!
    audio attach function pointer
    */
  drv_attach_func drv_attach;
}_audio_cfg_t;

/*!
 usb power status
  */
typedef enum
{
  /*!
    usb power no use
    */
  USB_POWER_NO_USE = 0,
  /*!
    usb power ctrl
    */
  USB_POWER_CTRL,
}board_usb_pwr_t;

/*!
 usb over current control
  */
typedef enum
{
  /*!
    usb over current on use on this pin
    */
  USB_OC_NO_USE = 0,
  /*!
    usb over current detect on this pin
    */
  USB_OC_DETECT,
}board_usb_oc_t;

/*!
  _usb_cfg_t
  */
typedef struct
{
  /*!
    usb host count
    */
  u8 host_cnt;
  /*!
    if support hub
    */
  u8 if_support_hub;
  /*!
    usb bus 0 task priority
    */
  u8 bus0_task_prio;
  /*!
    usb bus 1 task priority
    */
  u8 bus1_task_prio;
  /*!
    usb storage 0 task priority
    */
  u8 us0_task_prio;
  /*!
    usb storage 1 task priority
    */
  u8 us1_task_prio;
  /*!
    pinmux on usb0 power control, see board_usb_pwr_t
    */
  u8 pinmux_pwr0;
  /*!
    pinmux on usb0 over current, see board_usb_oc_t
    */
  u8 pinmux_oc0;
  /*!
    pinmux on usb1 power control, board_usb_pwr_t
    */
  u8 pinmux_pwr1;
  /*!
    pinmux on usb1 over current, board_usb_oc_t
    */
  u8 pinmux_oc1;
  /*!
    usb ethernet adapter device support count
    */
  u8 usb_eth_support_cnt;
  /*!
    usb ethernet adapter driver attach functions
    */
  drv_attach_func usb_eth_drv_attach[4];
    /*!
    usb camera adapter driver attach functions
    */
  drv_attach_func usb_camera_drv_attach[1];
  /*!
    usb ethernet adapter driver attach name
    */
  char *p_usb_eth_drv_name[4];
  /*!
    usb ethernet task priority
    */
  u8 usb_eth_task_prio[4];
   /*!
    usb hub tt task priority
   */
  u8 usb_hub_tt_task_prio;
   /*!
    usb port select number
   */
   u8 select_usb_port_number;
   /*!
    usb attach function pointer
    */
   drv_attach_func drv_attach;
}_usb_cfg_t;

/*!
 uio control mode
  */
typedef enum
{
  /*!
    uio hardware control mode
    */
  UIO_HW_CTRL = 0,
  /*!
    uio soft gpio control mode
    */
  UIO_SOFT_GPIO,
}uio_mode_t;

/*!
  uio config
  */
typedef struct
{
  /*!
    uio mode, see uio_mode_t
    */
  u8 mode;
  /*!
    uio soft config
    */
  uio_cfg_t soft_cfg;
}board_uio_cfg_t;

/*!
  smart card config
  */
typedef struct
{
  /*!
    if support 3.3V/5V voltage selection
    */
  u8 if_support_vol_sel;
  /*!
    card detect pin level, 0: low level, 1: high level
    */
  u8 card_detect_pin_level;
  /*!
    3.3V/5V voltage select gpio
    */
  u8 vol_sel_gpio;
  /*!
    pin level when select 5V, 0: low level, 1: high level
    */
  u8 pin_level_5V;
}board_smc_cfg_t;


/*!
 tuner access type
  */
typedef enum
{
  /*!
    defult, by demod access repeater, if use this mode, tn_i2c_cfg is invalid
    */
  TN_ACCESS_DEF = 0,
  /*!
    by hardware i2c, you need to config tn_i2c_cfg
    */
  TN_ACCESS_HW_I2C,
  /*!
    by software i2c, you need to config tn_i2c_cfg
    */
  TN_ACCESS_SW_I2C,
}board_tn_access_type_t;


/*!
 demod access type
  */
typedef enum
{
  /*!
    demod default access mode
    */
  DMD_ACCESS_DEF = 0,
  /*!
    by hardware i2c, you need to config dmd_i2c_cfg
    */
  DMD_ACCESS_HW,
  /*!
    by software i2c, you need to config dmd_i2c_cfg
    */
  DMD_ACCESS_SW,
}board_dmd_access_type_t;


/*!
  i2c config
  */
typedef struct
{
  /*!
    the i2c address, 0 means default, need not set
    */
  u8 i2c_addr;
  /*!
    the number of gpio act as SCL, is valid if use software i2c
    */
  u8 sw_scl;
  /*!
    the number of gpio act as SDA, is valid if use software i2c
    */
  u8 sw_sda;
   /*!
    hardware i2c id, is valid if use hardware i2c host, start from 0
    */
  u8 hw_i2c_id;
}_i2c_config_t;

/*!
  tuner config
  */
typedef struct
{
  /*!
    see nim_tuner_ver_t
    */
  u8 chip_id;
  /*!
    tuner access type, see board_tn_access_type_t.
    */
  u8 access_type;
  /*!
    if support RF loop through.
    */
  u8 if_support_loop_through : 1;
  /*!
    clock out ctrl: 0:xtal out, 1:clock out
    */
  u8 clk_out_ctrl : 1;
  /*!
    clock out divider, only valid if clk_out_ctrl is 1
    */
  u8 clk_out_div : 5;
  /*!
    tuner i2c config
    */
  _i2c_config_t i2c_cfg;
  /*!
    tuner crystal clock, 0 means default
    */
  u32 crystal_clk;
}_tn_config_t;

/*!
  dmd config
  */
typedef struct
{
  /*!
    dmd access type, see board_dmd_access_type_t.
    */
  u8 access_type;
  /*!
    if needs software to reset chip
    */
  u8 if_reset_by_sw;
  /*!
    reset pin number, is valid if if_reset_by_sw is 1
    */
  u8 reset_pin;
  /*!
    demod band width, for C and T demod
    */
  u8 band_width;
   /*!
    number of set limit  lock ferquency
    */
  u32 freq_offset_limit;
  /*!
    demod i2c config
    */
  _i2c_config_t i2c_cfg;
  /*!
    demod crystal clock, 0 means default
    */
  u32 crystal_clk;
  /*!
    demod default working mode
    */
  u32 init_mode;
  /*!
    The times of blind scan
    */
  u8 bs_times;
}_dmd_config_t;

/*!
  nim config
  */
typedef struct
{
  /*!
    nim attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
    nim device name string called by attach function
    */
  char *p_dev_name;
  /*!
    if not use the public sharing driver service, set this, the notify task priority
    */
  u32 task_prio;
  /*!
    tuner config
    */
  _tn_config_t tn_cfg;
  /*!
    demod config
    */
  _dmd_config_t dmd_cfg;
  /*!
    pin config
    */
  nim_pin_config_t pin_cfg;
}_board_nim_cfg_t;


/*!
  smart card config
  */
typedef struct
{
  /*!
    if dual nim solution
    */
  u8 if_dual_nim;
  /*!
   if select nim solution
  */
  u8 if_sel_nim;
  /*!
    nim config
    */
  _board_nim_cfg_t dev_cfg[2];
}board_nim_cfg_t;

/*!
  hdmi config
  */
typedef struct
{
  /*!
    if use sw i2c, default is 0
    */
  u8 if_use_sw_i2c;
  /*!
    if use ece function
    */
  u8 if_use_ece;
  /*!
    if use hdcp function
    */
  u8 if_use_hdcp;
  /*!
    hdcp key
    */
  u8 *hdcp_key;
  /*!
    if running in fastlogo
    */
  u32 is_fastlogo_mode;
  /*!
    if initialized in fastlogo
    */
  u32 is_initialized;
  /*!
    i2c config
    */
  _i2c_config_t i2c_cfg;
}board_hdmi_cfg_t;

/*!
  spi config
  */
typedef struct
{
  /*!
    spi nor-flash io mode, 1 or 2 or 4 line mode
    */
  u8 spi_norf_io_mode;
  /*!
    cs pin gpio
    */
  u8 cs_pin;
}board_spi_cfg_t;

/*!
 demod access type
  */
typedef enum
{
  /*!
    spi ethernet phy
    */
  ETHERNET_PHY_SPI = 0,
  /*!
    RMII ethernet phy
    */
  ETHERNET_PHY_RMII,
  /*!
    MII ethernet phy
    */
  ETHERNET_PHY_MII,
}_ethernet_phy_intf_t;

/*!
  ethernet config
  */
typedef struct
{
  /*!
    ethernet driver attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
    device name
    */
  char *p_dev_name;
  /*!
    phy interface, see _ethernet_phy_intf_t
    */
  u8 phy_intf;
  /*!
    if phy reset by soft
    */
  u8 if_reset_by_sw;
  /*!
    reset pin, is valid if phy reset by soft
    */
  u8 reset_pin;
  /*!
    interrupt pin
    */
  u8 intrrupt_pin;
}board_ethernet_cfg_t;

/*!
  wifi config
  */
typedef struct
{
  /*!
    wifi driver attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
    device name
    */
  char *p_dev_name;
    /*!
  wifi H prio task
    */
u8 Wifi_Prio_H;
      /*!
  wifi median1 prio task
    */
u8 Wifi_Prio_M1;
        /*!
  wifi median2 prio task
    */
u8 Wifi_Prio_M2;
          /*!
  wifi median3 prio task
    */
u8 Wifi_Prio_M3;
          /*!
  wifi Low prio task
    */
u8 Wifi_Prio_L1;
                    /*!
  wifi Low2 prio task
    */
u8 Wifi_Prio_L2;
                            /*!
    wpa supplicant task prio
    */
u8 wifi_wpa_supplicant_task_prio;
                                                 /*!
    net link task prio
    */
u8 wifi_net_link_task_prio;
}board_wifi_cfg_t;
/*!
  wifi config
  */
typedef struct
{
  /*!
    spi driver attach function pointer
    */
  drv_attach_func drv_attach;
    /*!
    device name
    */
  char *p_dev_name;
      /*!
  wifi send task
    */
u8 task_let_0;
        /*!
  wifi recv task
    */
u8 task_let_1;
            /*!
    dev cmd config
    */
u8 task_let_2;
                        /*!
    dev cmd config
    */
u8 task_let_3;
                        /*!
    wpa supplicant task prio
    */
u8 wifi_wpa_supplicant_task_prio;

                         /*!
    net link task prio
    */
u8 wifi_net_link_task_prio;
}board_ra0_wifi_cfg_t;

/*!
  wifi config
  */
typedef struct
{
  /*!
    spi driver attach function pointer
    */
  drv_attach_func drv_attach;
    /*!
    device name
    */
  char *p_dev_name;

}board_spi_eth_cfg_t;

/*!
  sd card on-board config
  */
typedef struct
{
  /*!
    card detect pin level, 0:low, 1:high
    */
  u8 card_detect_level;
  /*!
    card write protect pin level, 0:low, 1:high
    */
  u8 wr_protect_level;
  /*!
    if power enable by soft
    */
  u8 if_power_ena_by_sw;
  /*!
    power enable pin, is valid if power enable by soft
    */
  u8 power_ena_pin;
  /*!
    power enable pin level, 0:low, 1:high
    */
  u8 power_ena_level;
}board_sdcard_cfg_t;

/*!
  sd card config
  */
typedef struct
{
  /*!
    soft config
    */
  block_device_config_t soft_cfg;
  /*!
    on-board hardware config
    */
  board_sdcard_cfg_t hw_cfg;
}sdcard_cfg_t;

/*!
 ts source
  */
typedef enum
{
  /*!
    spi ethernet phy
    */
  TS_SOURCE_DMD_0 = 0,
  /*!
    RMII ethernet phy
    */
  TS_SOURCE_DMD_1,
  /*!
    MII ethernet phy
    */
  TS_SOURCE_INTERNAL_SOC,
}ts_source_t;

/*!
  ts config
  */
typedef struct
{
  /*!
    ts input or output port
    */
  u8 port;
  /*!
    ts mode, see nim_ts_mode
    */
  u8 mode;
  /*!
    ts edge, 0:default, 1:rise, 2:fall
    */
  u8 edge;
  /*!
    ts source, see ts_source_t
    */
  u8 source;
}board_ts_cfg_t;

/*!
 register setting used in board config
  */
typedef struct
{
  /*!
    register address
    */
  u32 reg;
  /*!
    the value need to set
    */
  u32 val;
}_reg_setting_t;

/*!
  _av_perf_t
  */
typedef struct
{
  /*!
    0 means use default setting, 1 means need to set as the optimized value
    */
  u8 if_optimized;
  /*!
    the count of registers to be set in register file list
    */
  u8 reg_cnt;
  /*!
    register file list to be set
    */
  _reg_setting_t *p_reg_file;
}_reg_file_t;


/*!
 demod access type
  */
typedef enum
{
  /*!
    SD_PAL_576I
    */
  SD_PAL_576I = 0,
  /*!
    SD_NTSC_480I
    */
  SD_NTSC_480I,
  /*!
    SD_RGB_576I
    */
  SD_RGB_576I,
  /*!
    SD_RGB_480I
    */
  SD_RGB_480I,
  /*!
    HD_YPBPR_1080I_50
    */
  HD_YPBPR_1080I_50,
  /*!
    TV_STD_MAX
    */
  TV_STD_MAX,
}_tv_standard_t;

/*!
 platform type
  */
typedef enum
{
  /*!
    chip
    */
  PLATFORM_CHIP = 0,
  /*!
    FPGA
    */
  PLATFORM_FPGA,
  /*!
    simulator
    */
  PLATFORM_SIMU,
}_platform_t;

/*!
  _av_perf_t
  */
typedef struct
{
  /*!
    tv encoder performace
    */
  _reg_file_t tv_encoder[TV_STD_MAX];
}_av_perf_t;


/*!
  board performance config
  */
typedef struct
{
  /*!
    A/V performace
    */
  _av_perf_t av_perf;
  /*!
    hmdi performance
    */
  _reg_file_t hdmi_perf;
}board_perf_cfg_t;

/*!
 _storage_medium_t
  */
typedef enum
{
  /*!
    nor-flash
    */
  NOR_FLASH = 0,
  /*!
    nand-flash
    */
  NAND_FLASH,
  /*!
    E2PROM
    */
  E2PROM,
}storage_medium_t;

/*!
 _storage_usage_t
  */
typedef enum
{
  /*!
    char storage
    */
  CHAR_STOR = 0,
  /*!
    block storage
    */
  BLOCK_STOR,
}storage_type_t;

/*!
 _storage_usage_t
  */
typedef enum
{
  /*!
    spi bus
    */
  BUS_SPI = 0,
  /*!
    iic bus
    */
  BUS_I2C,
  /*!
    sdio bus
    */
  BUS_SDIO,
}storage_bus_t;

/*!
  board storage device config
  */
typedef struct
{
  /*!
    storage type, see storage_medium_t
    */
  u8 medium;
  /*!
    storage usage, see storage_usage_t
    */
  u8 type;
  /*!
    storage usage, see storage_bus_t
    */
  u8 bus;
  /*!
    storage size in KByte
    */
  u32 size;
}stor_dev_t;

/*!
  board storage device config
  */
typedef struct
{
  /*!
    device number
    */
  u8 dev_num;
  /*!
    device info
    */
  stor_dev_t dev_info[2];
}board_storage_cfg_t;

/*!
 test_mode_t
  */
typedef enum
{
  /*!
    manual test mode
    */
  TEST_MANUAL_MODE = 0,
  /*!
    auto test
    */
  TEST_AUTO_MODE,
}test_mode_t;

/*!
  demux config
  */
typedef struct
{
  /*!
    if disable av sync function
    */
  u8 if_disable_av_sync;
}_dmx_cfg_t;

/*!
  gprs config
  */
typedef struct
{
  /*!
    gprs modem driver attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
    device name
    */
  char *p_dev_name;
  /*!
    ppp thread task prio
    */
  u8 ppp_thread_task_prio;
  /*!
    modem thread task prio
    */
  u8 modem_thread_task_prio;
  /*!
     Communicate device, if use UART, 0 for UART0, 1 for UART;
      */
  u32 comm_device;
}board_gprs_cfg_t;
/*!
  usb-3g-modem-dongle config
  */
typedef struct
{
  /*!
   driver attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
   driver attach function pointer
    */
  drv_attach_func drv_ttyUSB_attach;
  /*!
    device name of ttyUSB
    */
  char *p_dev_ttyUSB_name;
  /*!
    ppp thread task prio
    */
  u8 ppp_thread_task_prio;
  /*!
    modem thread task prio
    */
  u8 modem_thread_task_prio;
  /*!
     Communicate device, if use UART, 0 for UART0, 1 for UART; need todo
      */
  u32 comm_device;
  /*!
    device name
    */
  char *p_dev_name;
}board_usb_modem_cfg_t;

/*!
  usb_3g_cdc_ether config
  */
typedef struct
{
  /*!
    cdc ether driver attach function pointer
    */
  drv_attach_func drv_attach;
  /*!
    device name
    */
  char *p_dev_name;
  /*!
    cdc ether recv task
    */
  u8 recv_task_prio;
}usb_3g_cdc_ether_cfg_t;

/*!
  spi-nand config
  */
typedef struct
{
  /*!
    spi bus config
    */
  spi_cfg_t spi_cfg;
  /*!
    ppp thread task prio
    */
  block_nand_config_t snand_cfg;
   /*!
    spi_bus attach function pointer
    */
  drv_attach_func drv_attach_spi_bus;
   /*!
    nand attach function pointer
    */
  drv_attach_func drv_attach_nand;
}snand_cfg_t;

/*!
  hid config
  */
typedef struct{
    /*!
    device name
    */
    char *p_dev_name;
    /*!
    usb hid attach function pointer
    */
    drv_attach_func drv_attach;
    /*!
    hid cfg
    */
    hid_cfg_t  hid_cfg;

}board_hid_cfg_t;

/*!
  support two terminal connection
  */
#define CONNECTION_NUM 2
/*!
  mini_httpd config
  */
typedef struct
{
  /*!
    mini_httpd server task prio
    */
  u8 task_prio_satip[CONNECTION_NUM];
  /*!
    mini_httpd server task prio
    */
  u8 task_prio_server;
  /*!
    mini_httpd handle task prio
    */
  u8 task_prio_handle;
}mini_httpd_cfg_t;

/*!
  ccid config
  */
typedef struct{
    /*!
    device name
    */
    char *p_dev_name;
    /*!
    usb ccid attach function pointer
    */
    drv_attach_func drv_attach;
    /*!
    ccid cfg
    */
    //ToDo
}usb_ccid_cfg_t;

/*!
  usb camera config
  */
typedef struct{
    /*!
    device name
    */
    char *p_dev_name;
    /*!
    usb camera attach function pointer
    */
    drv_attach_func drv_attach;
    /*!
      image quality
    */ 
    u32 img_quality;
    /*!
     frame rate
    */ 
    u32 frame_rate;
}usb_camera_cfg_t;

/*!
  the application mode you want to init
  */
typedef enum{
  /*!
    Main App
    */
  APP_MAIN = 0,
  /*!
    fast logo
    */
  APP_FASTLOGO,
  /*!
    ota
    */
  APP_MODE_OTA,
}drv_app_mode_t;

/*!
  over current control by gpio
  */
typedef struct
{
  /*!
    platform
    */
  _platform_t platform;
  /*!
    package
    */
  chip_package_id_t chip_package;
  /*!
    sharing driver service task priority
    */
  u32 sharing_drvsvc_prio;
  /*!
    config app mode, see drv_app_mode_t
    */
  u32 app_mode : 3;
  /*!
    if config pinmux&gpio by uplayer
    */
  u32 if_cfg_pinmux_by_prj : 1;
  /*!
    if for driver test
    */
  u32 if_for_test : 1;
  /*!
    test mode, is valid if if_for_test is 1, see test_mode_t
    */
  u32 test_mode : 2;
  /*!
    if use smc
    */
  u32 if_use_smc : 1;
  /*!
    if use ethernet
    */
  u32 if_use_ethernet : 1;
    /*!
    if use spi ethernet
    */
  u32 if_use_spi_ethernet : 1;
  /*!
    if use wifi 8188
    */
  u32 if_use_wifi_rtl8188 : 1;
    /*!
    if use wifi 5370
    */
  u32 if_use_wifi_rt5370 : 1;
        /*!
    if use wifi  rt 7601
    */
  u32 if_use_wifi_rt7601 : 1;
  /*!
    if use usb-ethernet
    */
  u32 if_use_usb_ethernet : 1;
  /*!
    if use 2nd uart
    */
  u32 if_use_2nd_uart : 1;
  /*!
    if use sd card
    */
  u32 if_use_sd : 1;
  /*!
    if use soft gpe
    */
  u32 if_use_soft_gpe : 1;
  /*!
    if use gprs modem
    */
  u32 if_use_gprs : 1;
    /*!
    if use usb camera
    */
  u32 if_use_usb_camera : 1;
    /*!
    if use usb tty
    */
  u32 if_use_usb_serial : 1;
    /*!
    if use usb modem
    */
  u32 if_use_usb_serial_modem : 1;
    /*!
    if use usb modem
    */
  u32 if_use_nim : 1;

    /*!
    if use usb modem
    */
  u32 if_use_nand : 1;
    /*!
    if use usb modem
    */
  u32 if_use_vid : 1;
    /*!
    if use usb modem
    */
  u32 if_use_adu : 1;

  /*!   if use usb hid
  */
  u32 if_use_hid_scanner : 1;
    /*!
    ts input ways
    */
  u32 ts_input_port_num : 2;
    /*!
    cmm_wifi_cfg_used
    */
  u32 cmm_wifi_cfg_used;
  /*!
    if use cdc ethernet
    */
  u32 if_use_3g_cdc_ether : 1;
  /*!
    if start mini_httpd web server
    */
  u32 if_use_mini_httpd : 1;
   /*!
    if start mini_httpd web server
    */
  u32 if_use_ccid : 1;
  /*!
    storage config
    */
  board_storage_cfg_t stor_cfg;
  /*!
    nim config
    */
  board_uio_cfg_t uio_cfg;
  /*!
    uio config
    */
  board_nim_cfg_t nim_cfg;
  /*!
    video out config
    */
  _vedio_cfg_t video_cfg;
  /*!
    audio out config
    */
  _audio_cfg_t audio_cfg;
  /*!
    usb config
    */
  _usb_cfg_t usb_cfg;
  /*!
    smc config
    */
  board_smc_cfg_t smc_cfg;
  /*!
    standby config
    */
  lpower_cfg_t lp_cfg;
  /*!
    hdmi config
    */
  board_hdmi_cfg_t hdmi_cfg;
  /*!
    ethernet config
    */
  board_ethernet_cfg_t eth_cfg;
  /*!
    wifi config rtl8188
    */
  board_wifi_cfg_t wifi_cfg_rtl8188;
    /*!
    wifi config rt5370
    */
  board_wifi_cfg_t wifi_cfg_rt5370;
        /*!
    wifi config rt7601
    */
  board_wifi_cfg_t wifi_cfg_rt7601;
    /*!
    spi eth config
    */
  board_spi_eth_cfg_t spi_eth_cfg;
  /*!
    sd card config
    */
  sdcard_cfg_t sdcard_cfg;
  /*!
    ts input config
    */
  board_ts_cfg_t ts_inut_cfg[4];
  /*!
    demux config
    */
  _dmx_cfg_t dmx_cfg;
  /*!
    performance config
    */
  board_perf_cfg_t perf_cfg;
  /*!
    gprs modem config
    */
  board_gprs_cfg_t gprs_cfg;
    /*!
    usb modem 3g dongle config
    */
  board_usb_modem_cfg_t usb_modem_cfg;
    /*!
    usb cdc ethernet config
    */
  usb_3g_cdc_ether_cfg_t usb_3g_cdc_ether_cfg;
    /*!
    spi nand config
    */
  snand_cfg_t spi_nand_cfg;
     /*!
     usb hid config
     */
      board_hid_cfg_t   usb_hid_cfg;
    /*!
    mini_httpd config
    */
  mini_httpd_cfg_t mini_httpd_cfg;
  /*!
     usb ccid config
     */
  usb_ccid_cfg_t   usb_ccid_cfg;
  /*!
    usb camera config
    */
  usb_camera_cfg_t usb_camera_cfg;
}drv_config_t;

#endif //__DRV_CFG_H__
