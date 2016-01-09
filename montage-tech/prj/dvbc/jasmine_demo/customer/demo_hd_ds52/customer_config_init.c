/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//#include "lpower.h"
#include "ui_common.h"

#include "customer_config.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#endif

customer_cfg_t g_customer;
extern u8 g_led_index;
extern u8 g_led_buffer[10];
extern u8 com_num;


void config_ota_main_tp(void)
{
  dvbc_lock_t upg_tp;
  nim_para_t set_tp;

  memset(&upg_tp,0,sizeof(dvbc_lock_t));
  sys_status_get_upgrade_tp(&upg_tp);
  memset(&set_tp,0,sizeof(nim_para_t));
  set_tp.lock_mode = SYS_DVBC;
  set_tp.lockc.tp_freq = upg_tp.tp_freq;
  set_tp.lockc.tp_sym = upg_tp.tp_sym;
  set_tp.lockc.nim_modulate = (nim_modulation_t)(upg_tp.nim_modulate);
  set_tp.data_pid = 0x1b58;

  /****init in ota block,the data is do ota tp data,but in app,it from system status init data*/
  mul_ota_dm_api_save_set_ota_tp(&set_tp);

}

void customer_config_init(void)
{
  memset(&g_customer, 0, sizeof(customer_cfg_t));
  g_customer.customer_id = CUSTOMER_DEFAULT;
  g_customer.cas_id = CONFIG_CAS_ID_DS;
  g_customer.country = COUNTRY_INDIA;
  g_customer.language_num = 2;// english curn and chinese curn
  g_customer.cus_config_ota_main_tp = config_ota_main_tp;
}

u8 customer_config_language_num_get()
{
  return g_customer.language_num;
}

void lock_led_set(BOOL locked)
{
  uio_device_t *p_uio_dev = NULL;
  
  p_uio_dev = dev_find_identifier(NULL,DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  
  u8 g_get_buffer[10] = {0};
  g_led_index = 1;//3;

  if(locked == 0)
  {
    if(g_led_buffer[com_num] != 0)
    memcpy(&(g_led_buffer[g_led_index]),&(g_led_buffer[g_led_index + 1]),com_num - g_led_index);
    g_led_buffer[com_num] = 0;
    uio_display(p_uio_dev, g_led_buffer, com_num);
  }
  else
  {
    if(g_led_buffer[com_num] == 0)
    {
	  memcpy(g_get_buffer, g_led_buffer , sizeof(g_led_buffer));
      memcpy(&(g_led_buffer[g_led_index + 1]),&(g_get_buffer[g_led_index]),com_num - g_led_index);
      g_led_buffer[g_led_index] = '.';
    }
    uio_display(p_uio_dev, g_led_buffer, com_num + 1);
  }
}

#ifdef ENABLE_CA
BEGIN_MSGPROC(ui_desktop_proc_customer, cas_manage_proc_on_normal_cas)
#else
BEGIN_MSGPROC(ui_desktop_proc_customer, cont_class_proc)
#endif


#ifdef ENABLE_CA
END_MSGPROC(ui_desktop_proc_customer, cas_manage_proc_on_normal_cas)
#else
END_MSGPROC(ui_desktop_proc_customer, cont_class_proc)
#endif

#ifdef ENABLE_CA
BEGIN_KEYMAP(ui_desktop_keymap_customer, cas_manage_keymap_on_normal_cas)
#else
BEGIN_KEYMAP(ui_desktop_keymap_customer, NULL)
#endif

#ifdef ENABLE_CA
END_KEYMAP(ui_desktop_keymap_customer, cas_manage_keymap_on_normal_cas)
#else
END_KEYMAP(ui_desktop_keymap_customer, NULL)
#endif

