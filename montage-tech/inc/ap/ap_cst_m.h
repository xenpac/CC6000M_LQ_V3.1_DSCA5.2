/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_CST_M_H_
#define __AP_CST_M_H_
/*!
  Cluster Machine Pg Name Lenght
  */
#define NAME_MAX_LENGTH 32
/*!
  Broadcast port
  */
#define RC_BROADCAST_PORT 20020

/*!
  type id list
  cluster machine xml cmd type
  */
typedef enum
{
  /*!
  cluster machine control type
  */
  CST_M_CONTROL = 0,
  /*!
  cluster machine input type
  */
  CST_M_INPUT,
  /*!
  cluster machine ark type
  */
  CST_M_ACK,
  /*!
  cluster machine program_list type
  */
  CST_M_PROGRAM_LIST,
  /*!
  cluster machine ch_list_param type
  */
  CST_M_CH_LIST_PARAM,
  /*!
  cluster machine dvb play type
  */
  CST_M_DVB_PLAY,
  /*!
  cluster machine dvb playing type 
  */
  CST_M_DVB_PLAYING,
  /*!
  cluster machine send ip type
  */
  CST_M_SEND_IP,
} cst_m_type_t;


/*!
  cmd id list
  the event notify ui frame work, something done
  */
typedef enum 
{
  /*!
  cluster machine start server mode
  */
  CST_M_CMD_EMPTY = 0,
  /*!
  cluster machine start server mode
  */
  CST_M_CMD_START_SERVER,
  /*!
  cluster machine start server mode
  */
  CST_M_CMD_SERVER_DISCONNECT,
  /*!
  cluster machine start client mode
  */
  CST_M_CMD_START_CLIENT,
  /*!
  cluster machine config param
  */
  CST_M_CMD_CLIENT_DISCONNECT,
  /*!
  cluster machine get ch list cmd
  */
  CST_M_CMD_CLINET_GET_SERVER_INFO,//5
  /*!
  cluster machine get ch list cmd
  */
  CST_M_CMD_CLIENT_GET_CH_LIST,
  /*!
  cluster machine start play program
  */
  CST_M_CMD_CLIENT_PLAY,
  /*!
  cluster machine start play program
  */
  CST_M_CMD_CLIENT_PLAY_STOP,
  /*!
  cluster machine start play program
  */
  CST_M_CMD_SERVER_CHANGE_CH,
  /*!
  cluster machine start play program
  */
  CST_M_CMD_SERVER_GET_CLINET_INFO,
  
} cst_m_cmd_t;

/*!
  sub cmd id list
  get ch list mode.
  */
typedef enum 
{
  /*!
  cluster machine start server mode
  */
  SUBCMD_EMPTY = CST_M_CMD_CLIENT_GET_CH_LIST + 0x0010,
  /*!
  cluster machine start server mode
  */
  SUBCMD_GET_TP_CH_LIST_BY_ID,
  /*!
  cluster machine start client mode
  */
  SUBCMD_GET_CH_LIST_BY_ID,
  /*!
  cluster machine config param
  */
  SUBCMD_GET_TP_CH_LIST_BY_PAGE,
  /*!
  cluster machine get ch list cmd
  */
  SUBCMD_GET_CH_LIST_BY_PAGE,
  
} ch_list_sub_cmd_t;
/*!
  event id list
  the event notify ui frame work, something done
  */
typedef enum 
{
  /*!
    cluster machine ch list update
    */
  CST_M_EVT_CH_LIST_UPDATE = (APP_CLUSTER_MACHINE << 16),
  /*!
    cluster machine get server ip
    */
  CST_M_EVT_CH_LIST_UPDATE_FAIL,
  /*!
    cluster machine get server ip
    */
  CST_M_EVT_GET_SERVER_IP,
  /*!
    cluster machine server not connected
    */
  CST_M_EVT_GET_SERVER_PLAY_INFO,
  /*!
    cluster machine server not connected
    */
  CST_M_EVT_GET_SERVER_PLAY_INFO_FAILED,
  /*!
    cluster machine server not connected
    */
  CST_M_EVT_CONNECT,
  /*!
    cluster machine server not connected
    */
  CST_M_EVT_DISCONNECTED,
  /*!
    cluster machine no text input
    */
  CST_M_EVT_ON_TEXT_INPUT,
  /*!
    cluster machine exit text input
    */
  CST_M_EVT_EXIT_TEXT_INPUT,
  /*!
    cluster machine ch list get failed
    */
  CST_M_EVT_CH_LIST_GET_FAILED,
  

} cst_m_evt_t;

/*!
  result id list
  */
typedef enum 
{
  /*!
    cluster machine return success
    */
  CST_M_SUCCESS = 0,
  /*!
    cluster machine return failed
    */
  CST_M_FAILED,  
  /*!
    cluster machine return timeout
    */
  CST_M_TIMEOUT,
  /*!
     cluster machine return error
    */
  CST_M_PARAM_ERROR,
  /*!
    cluster machine return on playing pg
    */
  CST_M_NO_PLAYING_PG,
  /*!
    cluster machine not in input mode
    */
  CST_M_NOT_IN_INPUT_MODE,
}cst_m_result_t;


/*!
  UPG status machine
  */
typedef enum
{
  /*!
    idle status, ready to upg
    */
  CST_M_KEY_0 = 0,
  /*!
    compare software version
    */
  CST_M_KEY_1,
  /*!
    client flash burning status
    */
  CST_M_KEY_2,
  /*!
    client reboot status
    */
  CST_M_KEY_3,
  /*!
    read flash
    */
  CST_M_KEY_4,
  /*!
    create usb file
    */
  CST_M_KEY_5,
  /*!
      write usb file
      */
  CST_M_KEY_6,
  /*!
    pre-exit upg
    */
  CST_M_KEY_7,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_8,
  /*!
    client error
    */
  CST_M_KEY_9,
    /*!
    client reboot status
    */
  CST_M_KEY_UP,
  /*!
    read flash
    */
  CST_M_KEY_DOWN,
  /*!
    create usb file
    */
  CST_M_KEY_LEFT,
  /*!
      write usb file
      */
  CST_M_KEY_RIGHT,
  /*!
    pre-exit upg
    */
  CST_M_KEY_OK,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_MENU,
  /*!
    client error
    */
  CST_M_KEY_RETURN,
    /*!
    client reboot status
    */
  CST_M_KEY_RED,
  /*!
    read flash
    */
  CST_M_KEY_BLUE,
  /*!
    create usb file
    */
  CST_M_KEY_YELLOW,
  /*!
      write usb file
      */
  CST_M_KEY_GREEN,
  /*!
    pre-exit upg
    */
  CST_M_KEY_AUDIO,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_FIND,
  /*!
    client error
    */
  CST_M_KEY_FAV,
    /*!
      write usb file
      */
  CST_M_KEY_SAT,
  /*!
    pre-exit upg
    */
  CST_M_KEY_EPG,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_SUB,
  /*!
    client error
    */
  CST_M_KEY_PAGE_UP,
    /*!
    client reboot status
    */
  CST_M_KEY_PAGE_DOWN,
  /*!
    read flash
    */
  CST_M_KEY_VOL_UP,
  /*!
    create usb file
    */
  CST_M_KEY_VOL_DOWN,
  /*!
      write usb file
      */
  CST_M_KEY_SLEEP,
  /*!
    pre-exit upg
    */
  CST_M_KEY_ASPECT,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_TTX,
  /*!
    client error
    */
  CST_M_KEY_REC,
  /*!
    create usb file
    */
  CST_M_KEY_V_FORMAT,
  /*!
    write usb file
    */
  CST_M_KEY_ZOOM,
  /*!
    pre-exit upg
    */
  CST_M_KEY_PREY,    
  /*!
    the end of State machine, below is for error status
    */
  CST_M_KEY_NEXT,
  /*!
    client error
    */
  CST_M_KEY_CANCEL,
} cst_m_key_t;

/*!
  UIO implementation policy
  */
typedef enum
{
    /*!
    Power all and both 13 and 18V are supported
    */
  POL_VERTICAL  = 0,
  /*!
    13V, vertical 
    */
  POL_HORIZANTAL,
  /*! 
    18V, horizantal
    */
  POL_LEFT,
    /*! 
    18V, horizantal
    */
  POL_RIGHT,
}pol_t;

/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
  UIO implementation policy
  */
  u8 pg_name[NAME_MAX_LENGTH + 1];
  /*!
  UIO implementation policy
  */
  u16 id;
  /*!
  UIO implementation policy
  */
  u32 sid;
  /*!
  UIO implementation policy
  */
  BOOL b_tv;
  /*!
  UIO implementation policy
  */
  BOOL b_scramble;
  /*!
  UIO implementation policy
  */
  u8 st_name[NAME_MAX_LENGTH + 1];
  /*!
  UIO implementation policy
  */
  u16 sat_id;
  /*!
  UIO implementation policy
  */
  u16 tp_id;
  /*!
  UIO implementation policy
  */
  u32 freq;
  /*!
  UIO implementation policy
  */
  u32 sym;
  /*!
  UIO implementation policy
  */
  pol_t pol; 
}pg_detail_t;

/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
  UIO implementation policy
  */
  u8 ver_id[5];
  /*!
  UIO implementation policy
  */
  u8 type[30];
  /*!
  UIO implementation policy
  */
  u32 pg_start_id;
  /*!
  UIO implementation policy
  */
  u32 cnt;
  /*!
  UIO implementation policy
  */
  u32 cur_page;
  /*!
  UIO implementation policy
  */
  u32 total_page;
  /*!
  UIO implementation policy
  */
  u32 total_pg;
  /*!
  UIO implementation policy
  */
  pg_detail_t pg[20];
}pg_list_t;

/*!
  UIO implementation policy
  */
typedef struct
{
  /*!
  UIO implementation policy
  */
  char ver_id[5];
  /*!
  UIO implementation policy
  */
  char play_type[20];
  /*!
  UIO implementation policy
  */
  pg_detail_t ser_pg;
}ser_play_info_t;


/*!
  UIO implementation policy
  */
typedef struct
{
  pg_list_t list_data;

  ser_play_info_t ser_play_info;
  /*!
    Initialize the upg ap, and add config info
    \param[out] p_cfg the configuration to fill
    */
  void (*on_init)(void);

  RET_CODE (*save_ch_list)(pg_list_t *p_pg_list);
  
  void (*restart_rtsp)(void);
}cst_m_policy_t;
/*!
  Get UPG instance and return upg application instance
  \param[in] p_usb_upg_policy UPG application implementation policy
  */
void get_server_ip(u8 *p_data);
/*!
  Get UPG instance and return upg application instance
  \param[in] p_usb_upg_policy UPG application implementation policy
  */
app_t * construct_ap_cst_m(cst_m_policy_t *p_cst_m_policy);

#endif // End for __AP_USB_UPGRADE_H_

