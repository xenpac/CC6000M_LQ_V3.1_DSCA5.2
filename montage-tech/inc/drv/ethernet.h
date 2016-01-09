/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ETHERNET_H_
#define __ETHERNET_H_
#ifdef __cplusplus
extern "C" {
#endif
/*!
  define invalid socket fd value
  */
#define INVALID_SOCKET (-1)

/*!
    CHIP VERSION
  */
typedef enum ethernet_phy_version 
{
    /*!
      veasion a
      */
    CHIPID_CONCERTO_A,
    /*!
      ersionb0
      */
    CHIPID_CONCERTO_B0,   
        /*!
      version unknow
      */
    CHIPID_CONCERTO,
}ethernet_phy_version;

/*!
    IO control command
  */
typedef enum ethernet_ioctrl_cmd 
{
    /*!
      get ethernet link stats
      */
    ETH_IOCTRL_LINK_STATS_CHECK,
    /*!
      identify if this is the right device
      */
    ETH_IOCTRL_IDENTIFY,   
 /*!
      get ethernet hw mac address
      */
    GET_ETH_HW_MAC_ADDRESS,
 /*!
      SET ethernet phy version A or B
      */
    SET_ETH_PHY_VERSION,
        /*!
      wifi link info
      */
    ETH_WIFI_LINK_INFO,
        /*!
      set ethernet hw mac address
      */
    SET_ETH_HW_MAC_ADDRESS,
}ethernet_ioctrl_cmd;
/*!
    IO control type
  */
typedef enum eth_ioctl_link_stats 
{
    /*!
      net cable unlink status
      */
    NET_CABLE_UNLINK,
    /*!
      cable linked
      */
    NET_CABLE_LINK,
    /*!
    for usb dongle: no device
      */
    NO_USB_NET_DEVICE,
        /*!
    for usb dongle: sw probe
      */
    USB_NET_DEVICE_PROBE,
     /*!
      wifi_device dis connect a AP
      */
    DEVICE_DIS_CONNECT_AP,
     /*!
      wifi connected a AP
      */
     DEVICE_CONNECT_AP,
}eth_ioctl_link_stats_t;
/*!
  define sockaddr_eth_t
  */
typedef struct
{
  /*!
    sa len
    */ 
  u8 sa_len;
  /*!
    sa family
    */ 
  u8 sa_family;
  /*!
    sa data
    */ 
  char sa_data[14];
}sockaddr_eth_t;

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
}ethernet_cfg_t;

/*!
  ethernet configuration
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
}ethernet_device_t;

/*!
  ethernet init function
  \param[in] dev handle
  \param[in] config
 */
int lwip_init_tcpip(ethernet_device_t *p_dev, ethernet_cfg_t *p_cfg);
/*!
  if ip address 192.168.1.4 fun will set ipaddr[0]=192,ipaddr[1]=168,ipaddr[2]=1,ipaddr[3]=4
  \param[in] dev handle
  \param[in] ip netmask gw pdns adns address 
 */
void  get_net_device_addr_info(ethernet_device_t *p_dev,
                                                            char *ipaddr,
                                                            char  *netmask,
                                                            char *gw,
                                                            char *primarydns,
                                                            char *alternatedns);

/*!
  if ip conflict, return 1, hwaddr is the mac address of conflicting machine.
  if no conflict, return 0.
  
  \param[out] return conflicting mac address, 6 bytes. 
  return 
 */
int check_ip_conflict(u8 hwaddr[6]);
 /*!
   close lwip net if
   now all prj only support one netif ,need todo
   
   \param[in] dev handle 
   return 
  */
void lwip_netif_link_down(ethernet_device_t *p_dev);

#ifdef __cplusplus
}
#endif
#endif //__ETHERNET_H_

