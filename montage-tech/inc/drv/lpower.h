/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LPOWER_H__
#define __LPOWER_H__


#define   WAKE_UP_KEY_USERCODE

/*!
    low power manager device structure
  */
typedef struct lpower_device
{
  /*!
      Device base control
    */
  void *p_base;
  /*!
      Device private handle
    */
  void *p_priv;
}lpower_device_t;

/*!
    Power config
*/
typedef enum ctrl_cmd
{
  /*!
      key wake up
  */
  KEY_WAKE_UP = (0x1 << 0),
  /*!
      time wake up
  */
  TIME_WAKE_UP = (0x1 << 1),
  /*!
      The front panel don't display
    */
  FP_NO_DISPLAY = (0x1 << 2),
  /*!
      The front panel display standby time.
    */
  FP_DISPLAY_STANDBY_TIME = (0x1 << 3),
  /*!
      The front panel display local time.
    */
  FP_DISPLAY_LOCAL_TIME = (0x1 << 4),
  /*!
      Reserve
  */
  FP_DISPLAY_MISC = (0x1 << 5),
  /*!
      Into low power mode but can't display standby time.
  */
  IN_LOW_POWER = (0x1 << 30),
  /*!
      Don't Into low power mode can display standby time.
  */
  NO_IN_LOW_POWER = (0x1 << 31),
      /*!
      set standby time
  */
  SET_STANDBY_TIME = (0x1 <<10),
   /*!
      LED_DIS_CHAR
  */
  LED_DIS_CHAR = (0x1 <<12),
       /*!
      LED_DIS_CHAR
  */
  LED_DIS_NO = (0x1 <<14),
       /*!
      LED_DIS_TIME
  */
  LED_DIS_TIME = (0x1 <<13),
      /*!
      get standby time
  */
  GET_STANDBY_TIME = (0x1 <<11),
      /*!
      set standby parameters
  */
  SET_STANDBY_CONFIG = (0x1 <<15),
} ctrl_cmd_t;

/*!
  User gpio config
*/
typedef struct lpower_gpio_cfg
{
  /*!
      Pin mux:set used as gpio
  */
  u8 pin_mux;
  /*!
      Pin number
  */
  u8 gpio_num;
  /*!
      High active(1) or low active(0)
  */
  BOOL gp_active;
  /*!
      Open(1) or close(0)
  */
  BOOL on_off;
} lpower_gpio_cfg_t;

/*!
  User gpio led config
*/
typedef struct lpower_led_cfg
{
  /*!
      Pin number
  */
  u8 gpio_num;
  /*!
      High active(1) or low active(0)
  */
  BOOL gp_active;
} lpower_led_cfg_t;

/*!
  front panel type
  */
enum fp_type
{
  /*!
      CT1642
    */
  CT1642    = 0,
    /*!
      TM1635
    */
  TM1635    = 1,
      /*!
      FD620
    */
  FD620    = 2,
    /*!
      FD650
    */
  FD650    = 3 ,
      /*!
      FD650
    */
  FPGPIO    = 4 ,
      /*!
      FD650
    */
  FD650_FLOUNDER_TDX    = 5 ,
      /*!
        CT1642_flounder_ztd
      */
  CT1642_FLOUNDER_ZTD  = 6,
      /*!
        PT6315
      */
  PT6315_VFD16_0808FN  = 7,
      /*!
      FD650
    */
  FD650_FLOUNDER_TONGJIU = 8,
        /*!
      FD650_goby_dl
    */
  FD650_GOBY_DL = 9,
    /*!
      PT6311B HL_D336WA
    */
  PT6311B_HL_D336WA = 10,
  /*!
    FD650
    */
  FD650_FLOUNDER_SMT = 11,
  /*!
    CT1658
    */
  CT1658 = 12,
  /*!
      FD650
    */
  FD650_FLOUNDER_ZTD = 13,
    /*!
      FD650
    */
  FD650_FLOUNDER_ZHGX = 14,
  /*!
      FD650
    */
  FD650_BOOTES = 15,
  /*!
      CT1642_flounder_lotus
    */
  CT1642_FLOUNDER_LOTUS = 16,
  /*!
      FD650
    */
  FD650_CHART = 17,
    /*!
      type max
    */
  TYPE_MAX_CNT,
};

/*!
  front panel type
  */
enum fp_wakeup {
  /*!
    wake up by interrupt
  */
  WAKE_UP_INT    = 0,
  /*!
    wake up by normal
  */
  WAKE_UP_NOR    = 1,
    /*!
    type max
  */
  WAKE_MAX_CNT,
};

/*!
  int_edge for gpio
  */
enum int_edge {
    /*!
      rising up edge trigger
    */
  RISING_UP_INT    = 0,
    /*!
      falling down edge trigger
    */
  FALLING_DOWN_INT    = 1,
      /*!
      type max
    */
 EDGE_MAX_CNT,
};

/*!
  int_edge for gpio
  */
enum int_gpio {
  /*!
    pin 0
  */
  GPIO_INT_0    = 0,
  /*!
    pin 1
  */
  GPIO_INT_1    = 1,
    /*!
    type max
  */
  INT_MAX_CNT,
};

/*!
  standby cpu time struct
  */
typedef struct standby_time
{
  /*!
      get standby current  hour
    */
  u8 cur_hour;
    /*!
      get standby current  min
    */
  u8 cur_min;
      /*!
      get standby current  sec
    */
  u8 cur_sec;
  /*!
      standby run pass day
    */
  u8 pass_day;
}standby_time_t;


/*!
  standby cpu moudle
*/
typedef struct lpower_standby_cfg
{
     /*!
      fp type   //default is TM1642 type = 0 ;
    */
  enum fp_type type;
     /*!
      wake up mode,interrupt or normal
    */
  enum fp_wakeup wake_type;
      /*!
      interrupt mode for gpio;
    */
  enum int_edge int_edge_type;
      /*!
      interrupt pin for gpio ;
    */
  enum int_gpio int_gpio_pin;
     /*!
      led src bitmap
    */
  u8 *p_raw_map;
       /*!
      power down key   if the standby key is zero you must set pd_key0
    */
  u8 pd_key0;
    /*!
      power down key
    */
  u8 pd_key1;
      /*!
      power down key for front panel
    */
  u8 pd_key_fp;
       /*!
      used for gpio power pin
    */
  u8 gpio_power_pin;
       /*!
      used for fp clk pin
    */
  u8 gpio_clk_pin;
       /*!
      used for fp data pin
    */
  u8 gpio_data_pin;
       /*!
      time to wake up
    */
  u32 time_wake_up;
    /*!
      power down key
    */
  u8 pd_key2;
    /*!
      power down key
    */
  u8 pd_key3;
    /*!
      power down key
    */
  u8 pd_key4;
    /*!
      power down key
    */
  u8 pd_key5;
    /*!
      power down key
    */
  u8 pd_key6;
    /*!
      power down key
    */
  u8 pd_key7;


#ifdef   WAKE_UP_KEY_USERCODE

    /*!
    pd_key0_usercode
    */ 
    u16 pd_key0_usercode;
     /*!
    pd_key1_usercode
    */ 
    u16 pd_key1_usercode;
    /*!
    pd_key2_usercode
    */ 
    u16 pd_key2_usercode;
     /*!
    pd_key3_usercode
    */ 
    u16 pd_key3_usercode;
     /*!
    pd_key4_usercode
    */ 
    u16 pd_key4_usercode;
     /*!
    pd_key5_usercode
    */ 
    u16 pd_key5_usercode;
     /*!
    pd_key6_usercode
    */ 
    u16 pd_key6_usercode;
    /*!
    pd_key7_usercode
    */ 
    u16 pd_key7_usercode;

#endif

} lpower_standby_cfg_t;
/*!
    User config
  */
typedef struct lpower_cfg
{
    /*!
        Set if into low power mode.
      */
  u32 low_power_flag;
  /*!
      enter standby param
  */
  utc_time_t *p_tim;
  /*!
      Key map, irda key, keyboard key and reserve
  */
  u16 key_map[3];
  /*!
      gpio cfg
  */
  lpower_gpio_cfg_t gpio_cfg[3];
  /*!
  frontpanel led num
  */
  u8 led_num;
  /*!
  power polar
  */
  BOOL polar;
  /*!
  customer:oldman_biss....
  */
  u8 customer;
  /*!
  fp_type
  */
  u8 fp_type;
  /*!
    lp power code addr
    */
  u8 *p_code_addr;
   /*!
    lp power code size
    */
  u32 code_size;
   /*!
  the logo to display
  */
  u8 display_char[4];
  /*!
      lpower led num
  */
    u8 power_led_num;
    /*!
      lpower led cfg
  */
    lpower_led_cfg_t led_gpio_cfg[3];
   /*!
      lpower in sram debug
  */
  u8 debug_flag;
  /*!
      lpower_standby_cfg
  */
  lpower_standby_cfg_t standby_cfg;
} lpower_cfg_t;

/*!
  * concerto standby ir configure
  */
typedef struct _concerto_standby_ir_config
{
    /*!
     * wakeup_enable
     */
    u8 wakeup_enable;
} concerto_standby_ir_config_t;

/*!
  * concerto standby panel configure
  */
typedef struct _concerto_standby_panel_config
{
    /*!
     * wakeup_key
     */
    u16 wakeup_key;
    /*!
     * panel_type, see enum fp_type
     */
    u16 panel_type;
    /*!
     * wakeup_enable
     */
    u8 wakeup_enable;
    /*!
     * led_chars_enable
     */
    u8 led_chars_enable;
    /*!
     * led_chars
     * if you wants to display "1234", you should set led_chars[0] = '1',
     * led_chars[1] = '2', led_chars[2] = '3', led_chars[3] = '4'
     */
    u8 led_chars[4];
    /*!
     * led_dot_mask
     * if bit[0] = 1,then 1st led's dot is light.
     * if bit[1] = 1,then 2nd led's dot is light.
     * if bit[2] = 1,then 3rd led's dot is light.
     * if bit[3] = 1,then 4th led's dot is light.
     */
    u8 led_dot_mask;
    /*!
     * time_enable, if you wants to display time,set it to 1
     */
    u8 time_enable;
    /*!
     * time_hh, hours of time
     */
    u8 time_hh;
    /*!
     * time_mm, minutes of time
     */
    u8 time_mm;
    /*!
     * ao_gpio_led0_enable, if a led linked by ao_gpio0, set it to 1
     */
    u8 ao_gpio_led0_enable;
    /*!
     * ao_gpio_led0_state, ao_gpio0 's level
     */
    u8 ao_gpio_led0_state;
    /*!
     * ao_gpio_led1_enable, if a led linked by ao_gpio1, set it to 1
     */
    u8 ao_gpio_led1_enable;
    /*!
     * ao_gpio_led1_state, ao_gpio1 's level
     */
    u8 ao_gpio_led1_state;
    /*!
     * led_intens     
     */
    u8 led_intens;
} concerto_standby_panel_config_t;

/*!
  * concerto standby timer configure
  */
typedef struct _concerto_standby_timer_config
{
    /*!
     * wakeup_enable
     */
    u8 wakeup_enable;
    /*!
     * howmany milliseconds(ms) later, wake up.
     */
    u32 wakeup_duration;
} concerto_standby_timer_config_t;

/*!
  * concerto standby gpio configure
  */
typedef struct _concerto_standby_gpio_config
{
    /*!
     * if wakeup by ao_gpio0, set it to 1.
     */
    u8 wakeup_source0_enable;
    /*!
     * ao_gpio0 's edge for trigger wakeup
     */
    u8 source0_edge;
    /*!
     * if wakeup by ao_gpio1, set it to 1.
     */
    u8 wakeup_source1_enable;
    /*!
     * ao_gpio1 's edge for trigger wakeup
     */
    u8 source1_edge;    
    /*!
     * if wakeup by normal gpio.
     */
    u8 wakeup_nomal_enable;
    /*!
     * normal gpio's num
     */
    u8 normal_gpio_num;     	
} concerto_standby_gpio_config_t;
/*!
  * concerto standby configure
  */
typedef struct _concerto_standby_config
{
    /*!
     * standby ir config
     */
    concerto_standby_ir_config_t ir_config;
    /*!
     * standby panel config
     */
    concerto_standby_panel_config_t panel_config;
    /*!
     * standby timer config
     */    
    concerto_standby_timer_config_t timer_config;
    /*!
     * standby gpio config
     */    
    concerto_standby_gpio_config_t gpio_config;
} concerto_standby_config_t;

/*!
   \return 0 for success and others for failure.
  */
RET_CODE ap_lpower_ioctl(lpower_device_t *p_dev, u32 cmd, u32 param);
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_tm1635_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_goby_dl_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_flounder_tdx_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_flounder_tongjiu_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd620_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fpgpio_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_flounder_ztd_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_pt6315_vfd16_0808fn_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_pt6311b_hl_d336wa_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_flounder_smt_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1658_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_flounder_ztd_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_flounder_zhgx_war();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_bootes_war();

/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_son();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_flounder_ztd_son();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_flounder_lotus_son();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_son();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fpgpio_son();
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_fd650_concerto(void);
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_ct1642_concerto(void);
/*!
   \return 0 for success and others for failure.
  */
RET_CODE standby_cpu_attach_fw_gpio_concerto(void);


/*!
   \enter into standby.
  */
void ap_lpower_enter(lpower_device_t *p_dev);

#endif
