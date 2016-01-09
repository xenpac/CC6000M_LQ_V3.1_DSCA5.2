/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __WIFI_H_
#define __WIFI_H_

/*!
  wpa config string length
  */
#define WPA_CONFIG_MAX_STRING_LENGTH (64)
/*!
  max wpa config dimension
  */
#define WPA_CONFIG_MAX_DIMENSION (10)
/*!
  wifi encrypt type 
  */
typedef enum wifi_encrypt_type 
{
  /*!
    none
    */ 
    IW_ENC_NONE = 1,
  /*!
    wifi dis connect 
    */ 
    IW_ENC_WEP,
  /*!
    wifi wpa1 encrypt
    */ 
    IW_ENC_WPA1,
  /*!
    wifi wpa2 encrypt
    */ 
    IW_ENC_WPA2,
      /*!
    error 
    */ 
    IW_ENC_ERROR,
}wifi_encrypt_type_t;
/*!
  wifi ap info 
  */
typedef struct wifi_ap_info
{
  /*!
    essid
    */ 
signed char essid[32+1];
    /*!
    net work type
    */ 
unsigned char NetworkType;
  /*!
 is encryption
    */ 
unsigned char IsEncryption;
  /*!
    qual level
    */ 
unsigned char qual_level;
  /*!
    auth mode
    */ 
unsigned char AuthMode;
    /*!
    encryptype
    */ 
wifi_encrypt_type_t EncrypType;
  /*!
    ap mac address
    */ 
unsigned char ap_mac[14];
}wifi_ap_info_t; 
/*!
  wifi connect status 
  */
typedef enum wifi_connect_status 
{
  /*!
   no device
    */ 
    NO_WIFI_DEVICE,
  /*!
    wifi dis connect 
    */ 
    WIFI_DIS_CONNECT,
  /*!
    wifi connecting
    */ 
    WIFI_CONNECTING,
  /*!
    wifi connected
    */ 
    WIFI_CONNECTED,
  /*!
    wifi  connect failed
    */ 
    WIFI_CONNECT_FAILED,
}wifi_connect_stats_t;
/*!
  wifi link info 
  */
typedef struct wifi_status_info
{
  /*!
    essid
    */ 
  signed char essid[32+1];
      /*!
    encryptype
    */ 
  wifi_encrypt_type_t encrypt_type;
    /*!
    indicate wifi connect status
    */ 
 wifi_connect_stats_t connect_sts;
}wifi_status_info_t; 
/*!
  config wifi info 
  */
typedef struct config_wifi_info
{
  /*!
    ethernet cfg 
    */ 
ethernet_cfg_t eth_cfg;
  /*!
    essid
    */ 
signed char essid[32+1];
    /*!
    indicate whether encrypt is enabled
    */ 
  int is_enable_encrypt;
   /*!
    encryptype
    */ 
  wifi_encrypt_type_t encrypt_type;
  /*!
  essid key 
  */
  unsigned char key[64+1];
}config_wifi_info_t; 

/*!
 wifi_search_ap function
  \param[in] dev handle
  \param[in] config
 */
void wifi_ap_list_search(ethernet_device_t *p_dev);
/*!
 set_wifi_config
  \param[in] dev handle
  \param[in] config
 */
RET_CODE  set_wifi_config(ethernet_device_t *p_dev, config_wifi_info_t *cfg);
/*!
 wifi_ap_cnt_get function
  \param[in] dev handle
  \param[in] config
 */
void wifi_ap_cnt_get(ethernet_device_t *p_dev, signed long *ap_cnt);
/*!
 wifi_ap_info_get function
  \param[in] dev handle
  \param[in] config
  \ret error -1
 */
s32 wifi_ap_info_get(ethernet_device_t *p_dev, u32 index, wifi_ap_info_t *apInfo);
/*!
set_wifi_connect
  \param[in] dev handle
  \ret error -1
 */
void set_wifi_connect(ethernet_device_t *p_dev);
/*!
set_wifi_disconnect
  \param[in] dev handle
  \ret error -1
 */
void set_wifi_disconnect(ethernet_device_t *p_dev);
/*!
get_wifi_handle
  \param[in] dev handle
  \ret error -1
 */
void *get_wifi_handle();
/*!
  wifi_config
  */
typedef struct wifi_task_config
{
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
}wifi_task_config_t;
/*!
  wifi device info 
  */
typedef enum wifi_device_type 
{
  /*!
   no device
    */ 
    DUMMY_DEVICE,
  /*!
    Rtl8188eus
    */ 
    WIFI_RTL_8188EUS,
  /*!
    RT5370
    */ 
    WIFI_RT_5370,
  /*!
    RT7601
    */ 
    WIFI_RT_7601,
  /*!
   UN-know
    */ 
    WIFI_TODO,
}wifi_device_type_t;
/*!
get plug in wifi device
  \param[in] dev handle
  \ret wifi device type
 */
wifi_device_type_t get_wifi_plugin_device(void *handle);

#endif //__WIFI_H_

