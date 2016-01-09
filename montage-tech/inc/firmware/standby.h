/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __STANDBY_H__
#define __STANDBY_H__
/*!
  standby_cpu struct
  */
typedef struct standby_cpu
{
  /*!
      power down key   if the standby key is zero you must set pd_key0
    */   
  u8 pd_key0;
    /*!
      power down key
    */   
  u8 pd_key1;
    /*!
      power down key
    */   
  u8 pd_key_fp;   //fp 
  /*!
      fp type   //default is TM1642 type = 0 ;
    */     
  enum fp_type type;   
  /*!
      current time
    */   
  u8 hour;
   /*!
      current time
    */  
  u8 minute;
     /*!
      current time
    */ 
  u8 second ;
    /*!
     en_led_disp_time
    */     
  u32 en_led_disp_time;
    /*!
     en_led_disp_char
    */     
  u32 en_led_disp_char;    
    /*!
     led raw_map
    */
  u8 p_raw_map[8];
       /*!
      enable auto wake up 
    */
   u32 auto_time_wake_up;
   /*!
      set time to reset 
    */
   u32 auto_power_up_system_minute;
   /*!
      led0 disp 
    */  
  u8 led0_char;
      /*!
      led1 disp
    */  
  u8 led1_char;
         /*!
      led2 disp
    */  
  u8 led2_char;
            /*!
      led3 disp
    */  
  u8 led3_char;
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
    irda_usercode
    */ 
    u16 pd_key0_usercode;
     /*!
    irda1_usercode
    */ 
    u16 pd_key1_usercode;
    /*!
    irda2_usercode
    */ 
    u16 pd_key2_usercode;
     /*!
    irda3_usercode
    */ 
    u16 pd_key3_usercode;
     /*!
    irda4_usercode
    */ 
    u16 pd_key4_usercode;
     /*!
    irda5_usercode
    */ 
    u16 pd_key5_usercode;
     /*!
    irda6_usercode
    */ 
    u16 pd_key6_usercode;
    /*!
    irda7_usercode
    */ 
    u16 pd_key7_usercode;
#endif

}standby_cpu_t;
/*!
  get_wake_up_time_from_standby
  */
typedef struct cur_time
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
  
}cur_time_t;
/*!
  data register between AP and standby cpu
  */
typedef struct
{
      /*!
      raw map for led display
    */   
    u8 bitmap[4];  
      /*!
      current hour
    */   
    u8 cur_hour;  
      /*!
     current  min
    */   
    u8 cur_min;  
      /*!
    wake up hour
    */   
    u8 wake_hour;  
      /*!
    wake up min
    */   
    u8 wake_min;  
      /*!
      wake up irda
    */   
    u8 wake_up_irda;  
      /*!
     wake up key
    */   
    u8 wake_up_key;  
      /*!
     wake_up_irda1 
    */   
    u8 wake_up_irda1;
     /*!
     bit 7: time_wake; bit 6:time_disp; bit5 char_disp;bit4 repeat;bit[3:0]debug  
    */   
    u8 standby_config_info;
    /*!
      led0_disp
    */   
    u8 led0_disp;  
      /*!
      led1_disp
    */   
    u8 led1_disp;  
      /*!
      led1_disp
    */   
    u8 led2_disp;    
      /*!
      led1_disp
    */   
    u8 led3_disp;  
      /*!
     wake_up_irda2 
    */   
    u8 wake_up_irda2;
     /*!
     wake_up_irda3 
    */   
    u8 wake_up_irda3;
      /*!
     wake_up_irda4 
    */   
    u8 wake_up_irda4;
      /*!
     wake_up_irda5 
    */   
    u8 wake_up_irda5;
      /*!
     wake_up_irda6 
    */   
    u8 wake_up_irda6;
      /*!
     wake_up_irda7
    */   
    u8 wake_up_irda7;

#ifdef   WAKE_UP_KEY_USERCODE
    /*!
    wake_up_irda_usercode
    */ 
    u16 irda_usercode;
     /*!
    wake_up_irda1_usercode
    */ 
    u16 irda1_usercode;
    /*!
    wake_up_irda2_usercode
    */ 
    u16 irda2_usercode;
     /*!
    wake_up_irda3_usercode
    */ 
    u16 irda3_usercode;
     /*!
    wake_up_irda4_usercode
    */ 
    u16 irda4_usercode;
     /*!
    wake_up_irda5_usercode
    */ 
    u16 irda5_usercode;
     /*!
   wake_up_irda6_usercode
    */ 
    u16 irda6_usercode;
    /*!
    wake_up_irda7_usercode
    */ 
    u16 irda7_usercode;
#endif
}trans_info_standby_t;
/*!
  call standby cpu
  */
u32 call_standbycpu(struct standby_cpu *cmd);
#endif //__STANDBY_H__

