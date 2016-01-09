/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NET_SERV_H__
#define __NET_SERV_H__


/*!
  wifi ssid name length
  */
#define MAX_SSID_LEN (32)
/*!
  password length
  */
#define MAX_PASS_WORD_LEN (64)
/*!
  ping test url length
  */
#define MAX_PING_URL_LEN (32)
/*!
  connect para len
  */
#define MAX_CONN_PARA_LEN   (64)


/*!
  network service commands
  */
typedef enum
{
  /*!
    config network ip address
    */
  NET_DO_CONFIG_TCPIP,
  /*!
    network ping test
    */
  NET_DO_PING_TEST,
  /*!
    network stop ping test
    */
  NET_DO_STOP_PING_TEST,
  /*!
    network ping test
    */
  NET_DO_WIFI_SERACH,
  /*!
    network ping test
    */
  NET_DO_WIFI_CONNECT,
  /*!
    network gprs connect
    */
  NET_DO_GPRS_CONNECT,
  /*!
    network gprs disconnect
    */
  NET_DO_GPRS_DISCONNECT, 
   /*!
    network 3G disconnect
    */
  NET_DO_3G_CONNECT,
  /*!
    network 3G disconnect
    */  
  NET_DO_3G_DISCONNECT,
  /*!
    network get 3G info
    */  
  NET_DO_GET_3G_INFO,
  /*!
    network samba fs mount
    */
  NET_DO_MOUNT,
  /*!
    network pppoe connect
    */
  NET_DO_PPPOE_CONNECT,
  /*!
    network pppoe disconnect
    */
  NET_DO_PPPOE_DISCONNECT, 
} net_svc_cmd_t;

/*!
  net service events
  */
typedef enum tag_net_svc_evt
{
  /*!
    Flag of evt start
    */
  NET_SVC_EVT_START = (MDL_NET_SVC << 16),
  /*!
    Invoke this event when config ipaddr in 0x01
    */
  NET_SVC_CONFIG_IP,
  /*!
    Invoke this event when do ping test in 0x02
    */
  NET_SVC_PING_TEST,
  /*!
    Invoke this event when do ping test in 0x02
    */
  NET_SVC_WIFI_AP_CNT,
  
  /*!
    Invoke this event when network wire plug in in 0x03
    */
  NET_SVC_CHAIN_CONNECTED,
  /*!
    Invoke this event when network wire plug out in 0x03
    */
  NET_SVC_CHAIN_UNCONNECTED,
  /*!
    Invoke this event when get 3g info
    */
  NET_SVC_3G_INFO_GET,
  /*!
    pppoe connect success
    */
  NET_SVC_PPPOE_CONNECT_SUCCESS,
  /*!
    pppoe connect fail
    */
  NET_SVC_PPPOE_CONNECT_FAIL,
  /*!
    Flag of evt end
    */
  NET_SVC_EVT_END
}net_svc_event_t;


  /*!
    ping url
    */
  typedef struct tag_net_svc_ping
  {
  /*!
    ping url
    */  
    char ping_url_1[32];
  /*!
    ping url
    */  
    char ping_url_2[32];
  /*!
    ping url
    */  
    u8 ip[8];
  }net_svc_ping_t;

/*!
  init parameter net service.
  */
typedef struct tag_net_svc_init_para
{
  /*!
    service's priority.
    */
  u32 service_prio;
  /*!
    service's stack size.
    */
  u32 nstksize;
  /*!
    monitor interval. ms
    */
  u32 monitor_interval;
  /*!
    ping url
    */  
  BOOL enable_auto_ping;
  /*!
    ping url
    */  
  BOOL is_ping_url;
  /*!
    ping url
    */  
  net_svc_ping_t param;
}net_svc_init_para_t;

/*!
  network service ping test param
  */
typedef struct tag_net_svc_ping_para
{
  /*!
    ip addr
  */ 
  u8 ipaddr[4];
  /*!
    send bytes
  */ 
  u16 size;
  /*!
    test counts
  */ 
  u16 count;
  /*!
    ping url 
  */ 
  u8 url[MAX_PING_URL_LEN + 1];
  
}net_svc_ping_para_t;

/*!
  wifi config param
  */
typedef struct tag_net_svc_wifi_para
{
  /*!
    network service wifi key code
    */
  u8 key[MAX_PASS_WORD_LEN + 1];
  /*!
    network service wifi ssid
    */
  u8 ssid[MAX_SSID_LEN + 1];
   /*!
    indicate whether encrypt is enabled
    */ 
  int is_enable_encrypt;
  /*!
    wifi encrypt type
    */ 
  u8 encrypt_type;
}net_svc_wifi_para_t;

/*!
  gprs config param
  */
typedef struct tag_net_svc_gprs_para
{
   /*!
     tcpip_task_prio
     */ 
  u32 tcpip_task_prio;
  /*!
    network service gprs apn
    */
  u8 apn[MAX_CONN_PARA_LEN];
  /*!
    network service dial number
    */
  u8 dial_num[MAX_CONN_PARA_LEN];
  /*!
    fix me
    */
  u8 usrname[MAX_CONN_PARA_LEN];
  /*!
    fix me
    */
  u8 passwd[MAX_CONN_PARA_LEN];
  
}net_svc_gprs_para_t;

/*!
  gprs config param
  */
typedef struct tag_net_svc_3g_para
{
   /*!
     tcpip_task_prio
     */ 
  u32 tcpip_task_prio;
  /*!
    network service gprs apn
    */
  u8 apn[MAX_CONN_PARA_LEN];
  /*!
    network service dial number
    */
  u8 dial_num[MAX_CONN_PARA_LEN];
  /*!
    fix me
    */
  u8 usrname[MAX_CONN_PARA_LEN];
  /*!
    fix me
    */
  u8 passwd[MAX_CONN_PARA_LEN];
  
}net_svc_3g_para_t;

/*!
  pppoe config param
  */
typedef struct tag_net_svc_pppoe_para
{
  /*!
    fix me
    */
  u8 usrname[MAX_CONN_PARA_LEN];
  /*!
    fix me
    */
  u8 passwd[MAX_CONN_PARA_LEN];
}net_svc_pppoe_para_t;

/*!
  ethernet configuration
  */
typedef struct
{
  /*!
    indicate whether dhcp is enabled
    */ 
  int is_enabledhcp;
  /*!
    link level hardware address of this interface
    */
  unsigned char hwaddr[6];
  /*!
    ip addr
    */ 
  unsigned char ipaddr[4];
  /*!
    gateway
    */ 
  unsigned char gateway[4];
  /*!
    netmask
    */ 
  unsigned char netmask[4];
  /*!
    primary DNS
    */ 
  unsigned char primaryDNS[4];
  /*!
    secondary DNS
    */ 
  unsigned char alternateDNS[4];
  /*!
    tcp/ip stack prio
   */ 
   u32 tcp_ip_task_prio;
  /*!
    if use the public sharing driver service, set the handle here 
    */
  void *p_drvsvc;     
  /*!
    if not use the public sharing driver service, set this, the notify task priority
    */
  u32 task_prio;
  /*!
    if not use the public sharing driver service, set this, the notify task size
    */  
  u32 stack_size;
    /*!
    dev priv config 
    */  
  u8 *p_priv_confg;
  /*!
    network service wifi param, see net_svc_wifi_para_t
    */
  net_svc_wifi_para_t  wifi_para;
  /*!
    network service gprs param, see net_svc_wifi_para_t
    */
  net_svc_gprs_para_t  gprs_para;
  /*!
    network service 3g param, see net_svc_wifi_para_t
    */
  net_svc_3g_para_t  g3_para;
  /*!
    network service pppoe param, see net_svc_pppoe_para_t
    */
  net_svc_pppoe_para_t  pppoe_para;
}net_svc_net_cfg_t;


/*!
  network service do cmd param
  */
typedef struct tag_net_svc_cmd_para 
{

  /*!
    network service ethernet device handle
    */
  void *p_eth_dev;
  /*!
    UNION
    */
  union
  {
  /*!
    network service config tcpip param, see ethernet_cfg_t
    */
  net_svc_net_cfg_t net_cfg;
  /*!
    network service ping test param, see net_svc_ping_para_t
    */
  net_svc_ping_para_t  ping_para;
  
  };
} net_svc_cmd_para_t;

/*!
  net svc class declaration
  */
typedef struct tag_net_svc
{
  /*!
    get a service handle from net svc module
    */
  service_t * (*get_svc_instance)(class_handle_t handle, u32 context);
  /*!
    check net service busy or not
    */
  BOOL (*net_svc_is_busy)(class_handle_t handle);
  /*!
    clear net svc msg q
    */
  void (*net_svc_clear_msg)(class_handle_t handle);
  /*!
    net svc class privated data
    */
  void *p_data;
}net_svc_t;

/*!
  init net work service.

  \param[in] service_prio   service task priority.

  \return TRUE or FALSE.
  */
handle_t net_service_init(net_svc_init_para_t *p_para);

#endif
