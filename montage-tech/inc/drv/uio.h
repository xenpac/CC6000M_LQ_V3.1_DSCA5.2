/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UIO_H__
#define __UIO_H__
#ifdef __cplusplus
extern "C" {
#endif
    /*!
    UIO fifo size for keys(irda key and panel key)
    */
#define UIO_KEY_FIFO_SIZE            32
    /*!
    Max repeat key count for IrDA
    */
#define UIO_IRDA_MAX_PRTKEY          32
    /*!
    Max repeat key count for Frontpanel
    */
#define UIO_FP_MAX_PRTKEY            8
    /*!
    the user code start pos in UIO code, for IRDA only, for 16bit UIO code, bit 0~7
    is the pure code, bit 8~9 is the indicator if it is FP or IR, bit 10~15 is the
    user id
    */
#define UIO_USR_CODE_SHIFT           2
    /*!
    Max user code to support
    */
#define IRDA_MAX_USER                16
    /*!
    Max wfilt channel to support
    */
#define IRDA_MAX_CHANNEL             4
    /*!
    Max led map
    */
#define LED_MAP_MAX                  8
    /*!
    Module types supported by UIO
    */
  typedef enum
  {
    /*!
      IRDA module
      */
    UIO_IRDA = 0,
    /*!
      frontpanel module
      */
    UIO_FRONTPANEL
  }uio_type_t;

  /*!
    IRDA protocol
    */
  typedef enum
  {
    /*!
    NEC protocol, available now
    */
    IRDA_NEC = 0
  }irda_protocol_t;

  /*!
    fp scan protocol
    */
  typedef enum
  {
    /*!
      scan by task
      */
    FP_SCAN_TASK = 0,
    /*!
      scan by irq
      */
    FP_SCAN_IR
  }fp_scan_t;


  /*!
    UIO commands
    */
  typedef enum
  {
    /*!
      CT1642
      */
    FP_TYPE_DEFAULT,
    /*!
      gpio key: such as changhong jazz
      */
    FP_TYPE_GPIOKEY,
    /*!
      TS3189
      */
    FP_TYPE_TS3189,
    /*!
      74HC164
      */
    FP_TYPE_74HC164
  }frontpanel_type_t;

  /*!
    UIO commands
    */
  typedef enum
  {
    /*!
    set hardware user code, only this remote is acceptable
    */
    UIO_IR_SET_HW_USERCODE = DEV_IOCTRL_TYPE_UNLOCK + 0,
    /*!
    set hardware key code filter, only this key is acceptable
    */
    UIO_IR_SET_HW_KEYCODE,
    /*!
    Get IRDA user code
    */
    UIO_IR_GET_USERCODE,
    /*!
    Disable IRDA software user code
    */
    UIO_IR_DISABLE_SW_USERCODE,
    /*!
    Print IRDA software user code
    */
    UIO_IR_PRINT_SW_USERCODE,
    /*!
    Set irda wave filt
    */
    UIO_IR_SET_WAVEFILT,
    /*!
    Check if the irda key has been pressed
    */
    UIO_IR_CHECK_KEY,
    /*!
    Set the key flag 0 when no key pressed
    */
    UIO_IR_CLEAR_KEY_FLAG,
    /*!
    regist callback for IR key
    */
    UIO_IR_CALLBACK_REGIST,
    /*!
    Set Power LED status, only available in GPIO panel driver
    */
    UIO_FP_SET_POWER_LBD,
    /*!
    Set Power LED status, red or green for vfd16-0808fn
    */
    UIO_FP_SET_POWER_LED_COLOR,
    /*!
    Set Lock LED status, only available in GPIO panel driver
    */
    UIO_FP_SET_LOCK_LBD,
    /*!
    Set colon
    */
    UIO_FP_SET_COLON,
    /*!
    Set colon 2
    */
    UIO_FP_SET_COLON2,
    /*!
    Set colon 3
    */
    UIO_FP_SET_COLON3,
    /*!
    Set TXT led for HL_D336WA
    */
    UIO_FP_SET_TXT_LED,
    /*!
    Set power led for HL_D336WA
    */
    UIO_FP_SET_POWER_LED,
    /*!
    Set cicle1 led for HL_D336WA
    */
    UIO_FP_SET_CICLE1_LED,
    /*!
    Set cicle2 led for HL_D336WA
    */
    UIO_FP_SET_CICLE2_LED,
    /*!
    Set play led for HL_D336WA
    */
    UIO_FP_SET_PLAY_LED,
    /*!
    Set pause led for HL_D336WA
    */
    UIO_FP_SET_PAUSE_LED,
    /*!
    Set hdtype led for HL_D336WA
    */
    UIO_FP_SET_HDTYPE_LED,
    /*!
    Set hd led for HL_D336WA
    */
    UIO_FP_SET_HD_LED,
    /*!
    Set epg led for HL_D336WA
    */
    UIO_FP_SET_EPG_LED,
    /*!
    Set sub led for HL_D336WA
    */
    UIO_FP_SET_SUB_LED,
    /*!
    Set Dp1 led for VFD16_0808FN
    */
    UIO_FP_SET_DP1,
    /*!
    Set Dp2 led for VFD16_0808FN
    */
    UIO_FP_SET_DP2,
    /*!
    Set Dp3 led for HL_D336WA
    */
    UIO_FP_SET_DP3,
    /*!
    Set Dp4 led for HL_D336WA
    */
    UIO_FP_SET_DP4,
    /*!
    Set Dp5 led for HL_D336WA
    */
    UIO_FP_SET_DP5,
    /*!
    Set Dp6 led for HL_D336WA
    */
    UIO_FP_SET_DP6,
    /*!
    Set Dp7 led for HL_D336WA
    */
    UIO_FP_SET_DP7,
    /*!
    Set usb led for HL_D336WA
    */
    UIO_FP_SET_USB_LED,
    /*!
    Set Lock LED status, only available in GPIO panel driver
    */
    UIO_FP_GET_CH_PLUS_LBD,
    /*!
    Set Lock LED status, only available in GPIO panel driver
    */
    UIO_FP_GET_CH_SUB_LBD,
    /*!
    enable/disable gpio pan scan input, only available on GPIO panel
    */
    UIO_FP_KEY_ENABLE,
    /*!
    set panel scan interval,only available on GPIO panel
    */
    UIO_FP_SET_SCAN_FREQ, //set the scan interval, in ms
    /*!
    Get panel scan interval,only available on GPIO panel
    */
    UIO_FP_GET_SCAN_FREQ,
    /*!
    set panel signal strength if has
    */
    UIO_FP_SET_SIGNAL,
    /*!
     set power key
      */
    UIO_FP_POWER_KEY_PRESS,
    /*!
    set ir key 1
    */
    UIO_FP_IR_KEY_ONE,
    /*!
    set ir key 2
    */
    UIO_FP_IR_KEY_TWO,
    /*!
    set time out
    */
    UIO_FP_TIME_OUTO,
    /*!
    call this function in a loop when we don't use interrupt
    */
    UIO_FP_NO_INT_RUN,
    /*!
    set frontpanel type
    */
    UIO_FP_SET_FW_TYPE,
    /*!
    set frontpanel email led
    */
    UIO_FP_SET_EMAIL_LBD,
    /*!
    set frontpanel IR led
    */
    UIO_FP_SET_IR_LBD,
    /*!
    get curr disp led map data
    */
    UIO_FP_GET_DISP_BITMAP,
    /*!
    regist callback for FP key
    */
    UIO_FP_CALLBACK_REGIST,
    /*!
    set special led
    */
    UIO_FP_SET_SPEC_LED,
    /*!
    set TV led
    */
    UIO_FP_SET_TV_LED,
    /*!
    set RADIO led
    */
    UIO_FP_SET_RADIO_LED,
    /*!
    set dollar($) led
    */
    UIO_FP_SET_DOLLAR_LED,
    /*!
    set clock led
    */
    UIO_FP_SET_CLOCK_LED,
    /*!
    set level led(10 levels maxminum, use 2 Bytes to mark)
    */
    UIO_FP_SET_LEVEL_LED,
    /*!
    display data in designated grid,
    param(
         bit[14:14]:whether include special led when clear all data,
         bit[13:13]:whether clear all data,
         bit[12:12]:whether special led light,
         bit[11:8]:designated grid(0-9))
    */
    UIO_FP_DISPLAY_DESIGNATED_GRID,
  /*!
    set all segments of vfd power on to test vfd screen quality
   */
  UIO_FP_SET_ALL_LED_LIGHT,
  }uio_cmd_t;

  /*!
    Panel scan type, only for GPIO panel
    */
  typedef enum
  {
    /*!
      COM GPIO are reused with key scan
      */
    PAN_SCAN_SLOT = 0,
    /*!
      Shifter data are reused with key scan
      */
    PAN_SCAN_SHADOW
  }pan_scan_type_t;

  /*!
    Panel led lock, for jinya
    */
  typedef enum
  {
    /*!
      power led
      */
    FP_POWER_INDEX = 0,
    /*!
      email led
      */
    FP_EMAIL_INDEX,
    /*!
      lock led
      */
    FP_LOCK_INDEX,
    /*!
      ir led
      */
    FP_IR_INDEX,
  }pan_lock_type_t;
  /*!
    repeat code mode
    */
  typedef enum
  {
    /*!
    our default mode
    */
    OUR_DEF_MODE = 0,
    /*!
    put every repeat key, and add flag to ap
    */
    REPEAT_CODE_FLAG = 1,

  }irda_code_mode_t;
  /*!
    GPIO pin information, 1 Byte
    */
  typedef struct
  {
    /*!
      Polarity of GPIO, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH active(light)
      */
    u8 polar:1;
    /*!
      GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
      */
    u8 io   :1;
    /*!
      GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
      */
    u8 reserve   :6;	
    /*!
      GPIO index, upto 64 GPIO, 63 means invalid
      */
    u8 pos  :8;
  }pan_gpio_info_t;

  /*!
    GPIO pannel configuration
    */
  typedef struct        // Total 28 bytes
  {
    /*!
      refer to "enum pan_scan_type", start of offset 0
      */
    u8  type_scan:4;
    /*!
      0: Stand-alone LBD, 1: LBD in shifter.
      */
    u8  type_lbd :1;
    /*!
      0: Stand-alone COM, 1: COM in shifter.
      */
    u8  type_com: 1;
    /*!
      Number of scan PIN, MUST <= 2, 0: no scan;
      */
    u8  num_scan: 2;

    /*!
      Number of COM PIN, MUST <= 8, 0: no com; start of offset 1
      */
    u8  num_com : 4;
    /*!
      Position of colon flag, 0 to 7, 8 no colon
      */
    u8  pos_colon:4;

    /*!
 Attach MCU Type:
     FP type   bit1:FD620; bit2:PT6315 HL_886A; bit3:FD650;bit5: PT6311B HL_D336WA;
     bit6:PT6315 VFD16_0808FN
     bit 0: specify leds sequence, bit 4: 1, 3-line mcu, 0:2-line mcu;
Attache GPIO Type:
    bit0:1 CT1642 send data sequence, left -> right and 0 revert sequence, right -> left
    bit2:1:default com number(max number) ,0 specify com number
    bit4,5:use general gpio number (mux flatch,fclock, fdata...)
  */
    u8  status;

    /*!
      Shifter latch PIN, offset 3
      */
    pan_gpio_info_t flatch;
    /*!
      Shifter clock PIN
      */
    pan_gpio_info_t fclock;
    /*!
      Shifter data PIN
      */
    pan_gpio_info_t fdata;
    /*!
      Panel scan PIN
      */
    pan_gpio_info_t scan[2];
    /*!
      COM PIN, offset 8
      COM[4-7] is leds position, just for warriors now!!!
      */
    pan_gpio_info_t  com[8];
    /*!
      LBD PIN,IN sequence: power,lock ..., offset 16
      */
    pan_gpio_info_t  lbd[4];
    /*!
      COM scan interval in ms, offset 20
      */
    u32 com_scan_intv;
    /*!
      Repeat interval in ms, for rpt key only
      */
    u32 repeat_intv;
  }pan_hw_info_t;


  /*!
    IrDA wavefilt configuratin
    */
  typedef struct
  {
    /*!
      IrDA transport protocol
      */
    u8 protocol;
    /*!
      IrDA idle
      */
    u8 idle;
    /*!
      IRDA XTAL
      */
    u8 irda_xtal;
    /*!
      IrDA wave_len
     */
    u8 addr_len;
    /*!
      IrDA wfilt code
     */
    u32 wfilt_code;
    /*!
      IrDA wfilt mask
     */
    u32 wfilt_mask;
  }irda_wfilt_cfg_t;

  /*!
    IrDA configuratin
    */
  typedef struct
  {
    /*!
      IrDA transport protocol
      */
    u8 protocol;
    /*!
      IRDA XTAL
      */
    u8 irda_xtal;
    /*!
      IrDA uplayer specified power key simpling
     */
    u16 *p_irda_power_cfg;
    /*!
      user_code_set
     */
    u8 user_code_set;
    /*!
      sw_user_code
     */
    u16 sw_user_code[IRDA_MAX_USER];
    /*!
      set repeat time
     */
    u32 irda_repeat_time;
    /*!
      sw decoding give interrupt every N received samples
     */
    u8 irda_sw_recv_decimate;
    /*!
      IRDA wfilt channel number, max is 4
     */
     u8 irda_wfilt_channel;
    /*!
      IRDA wfilt cfg
     */
     irda_wfilt_cfg_t irda_wfilt_channel_cfg[IRDA_MAX_CHANNEL];
    /*!
      IRDA repeat code mode
     */
     irda_code_mode_t code_mode;
    /*!
      IRDA call back
     */
    void (*calback)(u32 callback_key);
  }irda_cfg_t;

  /*!
    LEB bitmap
    */
  typedef struct
  {
    /*!
      ascii character to display
      */
    u8 ch;
    /*!
      bitmap
      */
    u8 bitmap;
  }led_bitmap_t;

  /*!
    led disply mode
    */
  typedef enum
  {
    /*!
      default
      */
    LED_DISPLAY_DEFAULT = 0,
    /*!
      reverse display
      */
    LED_DISPLAY_REVERSE
  }led_display_type_t;


  /*!
    Frontpanel configuratin
    */
  typedef struct
  {
    /*!
    pointer to panel info config
    */
    pan_hw_info_t *p_info;
    /*!
    mcu panel usually is contorlled by bus which is spi or i2c
    */
    void *p_bus_dev;
    /*!
    the ledmap table size
    */
    u32 map_size;
    /*!
    the ledmap
    */
    const led_bitmap_t *p_map;
    /*!
    fp_scan_mode, not 0,scan in task, 0 scan in interrrupt
    */
    u8 fp_scan_mode;
    /*!
    fp type: 0 CT1642, 1 gpio key(chang hong), for jazz
    */
    u8 fp_type;
    /*!
    set led lum
    */
    u32 fp_led_lum;
    /*!
     fp op priority
     */
    u32 fp_op_pri;
    /*!
     Statck size
     */
    u32 fp_op_stksize;
    /*!
     led code(0-7)
     */
    u8 *p_led_map;
    /*!
     led display mode
     */
    u8 led_display_mode;
    /*!
     fp  pt6315 whether display on
     */
    u8 display_off;
  }fp_cfg_t;

  /*!
    UIO configuration
    */
  typedef struct
  {
    /*!
      irda config
      */
    irda_cfg_t *p_ircfg;
    /*!
      frontpanel config
      */
    fp_cfg_t *p_fpcfg;

    /*!
      The resource lock type
      */
    u32 lock_type;

  }uio_cfg_t;

  /*!
    UIO configuration
    */
  typedef struct
  {
    /*!
      device base class
      */
    void *p_base;
    /*!
      Pointer to private data
      */
    void *p_priv;
  }uio_device_t;

  /*!
    Get UIO code

    \param[in] p_dev device pointer
    \param[out] p_code pointer to return UIO code

    \return SUCCESS if OK, else fail
    */
  RET_CODE uio_get_code(uio_device_t *p_dev, u16 *p_code);

  /*!
    display front panel data

    \param[in] p_dev device pointer
    \param[in] p_data data pointer to display
    \param[in] len length of the data

    \return SUCCESS if OK, else fail
    */
  RET_CODE uio_display(uio_device_t *p_dev, u8 *p_data, u32 len);

  /*!
    Set UIO repeat key, it supports several IR remote(right now: 2) at the same
    time, and different remote can set different repeat key lists.

    \param[in] p_dev device pointer
    \param[in] uio device type, frontpanel or IrDA
    \param[in] p_keycodes pointer to the repeat key list
    \param[in] len the number of repeat key
    \param[in] ir_index the supported IrDA remote index

    \return SUCCESS if OK, else fail
    */
  void uio_tmp_ctl(uio_device_t *p_dev, u8 cmd);

  /*!
    Stop or start the fp-timer

    \param[in] p_dev device pointer
    \param[in] cmd, 0--start fp-timer, 1--stop-fp-timer;

    \return
    */
  RET_CODE uio_set_rpt_key(uio_device_t *p_dev, uio_type_t uio,
                            u8 *p_keycodes, u32 len, u8 ir_index);

  /*!
    Clear repeat key for UIO device

    \param[in] p_dev device pointer
    \param[in] uio device type, frontpanel or IrDA
    */
  void uio_clr_rpt_key(uio_device_t *p_dev, uio_type_t uio);

  /*!
    Clear keys in current UIO fifo

    \param[in] p_dev device pointer
    */
  void uio_clear_key(uio_device_t *p_dev);


  /*!
    Set software user code to support multiple remote control

    \param[in] p_dev device pointer
    \param[in] num the max number of IrDA to support, <= IRDA_MAX_USER
    \param[in] p_array pointer to user code array

    \return SUCCESS if OK, else fail
    */
  RET_CODE uio_set_user_code(uio_device_t *p_dev, u8 num, u16 *p_array);

  /*!
    Set the bimmap of front panel

    \param[in] p_dev device pointer
    \param[in] p_map bitmap pointer
    \param[in] size bitmap size
    */
  void uio_set_bitmap(uio_device_t *p_dev, led_bitmap_t *p_map, u32 size);
#ifdef __cplusplus
}
#endif
#endif //__UIO_H__
