/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CLASS_FACTORY_H_
#define __CLASS_FACTORY_H_
#ifdef __cplusplus
extern "C" {
#endif
/*!
  Class handle
  */
typedef void * class_handle_t;

/*!
  Complete id set supported in current system
  */
typedef enum 
{
 /*!
  Invalid 
  */
  IN_VALID_CLASS_ID = 0,
 /*!
  Class id for gb2312
  */
  GB2312_CLASS_ID = 1,
 /*!
  Class id for big5
  */
  BIG5_CLASS_ID = 2,
 /*!
  Class id for surface
  */
  SURFACE_CLASS_ID = 3,
 /*!
  Class id for mmi
  */
  MMI_CLASS_ID = 4,
 /*!
  Class id for gdi
  */
  GDI_CLASS_ID = 5,
 /*!
  Class id for gui control
  */
  GUI_CLASS_ID = 6,
 /*!
  Class id for gui paint
  */
  PAINT_CLASS_ID = 7,
 /*!
  Class id for gui scrip
  */
  SCRIPT_CLASS_ID = 8,
 /*!
  Class id for resource
  */
  RESOURCE_CLASS_ID = 9,
 /*!
  Class id for framework
  */
  UIFRM_CLASS_ID = 10,
 /*!
  Class id for AV control
  */
  AVC_CLASS_ID = 11,
 /*!
  Class id for database
  */
  DB_CLASS_ID = 12,
 /*!
  Class id for data manager
  */
  DM_CLASS_ID = 13,
 /*!
  Class id for DVB
  */
  DVB_CLASS_ID = 14,
 /*!
  Class id for NIM control
  */
  NC_CLASS_ID = 15,
 /*!
  Class id for system status control
  */
  SC_CLASS_ID = 16, 
 /*!
  Class id for time
  */
  TIME_CLASS_ID = 17,
 /*!
  Class id for epg
  */
  EPG_CLASS_ID = 18,
 /*!
  Class id for simple queue
  */
  SIMPLE_Q_CLASS_ID = 19,
 /*!
  Class id for app framework
  */
  AP_FRM_CLASS_ID = 20,
 /*!
  Class id for middleware utility
  */
  MDL_CLASS_ID = 21,
 /*!
  Class id for subtitle
  */
  SUBT_CLASS_ID = 22,
 /*!
  Class id VBI/Teletex
  */
  VBI_CLASS_ID = 23,
 /*!
  Class id VBI/Teletex ctrl
  */
  VBI_SUBT_CTRL_CLASS_ID = 24,  
 /*!
  Class id for AP EPG private
  */
  AP_EPG_PRV_CLASS_ID = 25,
 /*!
  CLASS ID for saving playback impl data
  */
  MAGIC_KEY_CLASS_ID = 26,
 /*!
  CLASS ID for smart card control
  */
  SMART_CARD_CLASS_ID = 27,
 /*!
  CLASS ID for scan  impl default
  */
  SCAN_IMPL_CLASS_ID = 28,
 /*!
  CLASS ID for file list
  */
  FILE_LIST_CLASS_ID = 29,
 /*!
  VFA class ID
  */
  VFS_CLASS_ID = 30,
 /*!
  NVOD class ID
  */
  NVOD_CLASS_ID = 31,   
 /*!
  Class id for lib char
  */
  LIB_CHAR_CLASS_ID = 32,
 /*!
  monitor service class ID
  */
  M_SVC_CLASS_ID = 33,    
 /*!
  pnp service class ID
  */
  PNP_SVC_CLASS_ID = 34,    
 /*!
  SOCKET class ID
  */
  SOCKET_CLASS_ID = 35,  
 /*!
  CLASS ID for saving playback impl data
  */
  PRESET_IMPL_CLASS_ID = 36,
 /*!
  Class id for NVOD mosaic
  */
  NVOD_MOSAIC_CLASS_ID = 37,
 /*!
  CLASS ID for saving playback impl data
  */
  MDX_CLASS_ID = 38,
 /*!
  Roll class id
  */
  ROLL_CLASS_ID = 39,  
 /*!
    ttx db class id
    */
  TTX_DB_CLASS_ID = 40,
  /*!
  dmx manager class id
  */
  DMX_MANAGER_CLASS_ID = 41,
  /*!
    vector font class id
    */
  VFONT_CLASS_ID = 42,    
  /*!
    subt database class id
    */
  SUBT_DB_CLASS_ID = 43,
  /*!
    dvb factory class id
    */
  DVB_FACTORY_CLASS_ID = 44,
  /*!
    pvr api class id
    */
  PVR_API_CLASS_ID = 45,
  /*! 
   Subtitle api class id 
  */ 
  SUBT_API_CLASS_ID = 46,
 
  /*! 
   Teletext api class id 
  */ 
  TTX_API_CLASS_ID = 47,

  /*! 
   File play api class id 
  */ 
  FILE_PLAY_API_CLASS_ID = 48,  
  /*! 
   subt ttx ctrl class id
  */ 
  SUBT_TTX_CTRL_CLASS_ID = 49,
  /*! 
   weather api class id
  */ 
  WEATHER_API_CLASS_ID = 50,
    /*! 
   weather api class id
  */ 
  NEWS_API_CLASS_ID = 51,
  /*! 
    vod api class id
    */ 
  VOD_API_CLASS_ID = 52,
  /*!
    albums api class id
    */
  ALBUMS_API_CLASS_ID = 53,
  /*!
    utils api class id
    */
  UTILS_API_CLASS_ID = 54,
  /*!
    rss api class id
    */
  RSS_API_CLASS_ID = 55,
  /*! 
    vod api class id
    */
  NET_SVC_CLASS_ID = 56,
   /*! 
   net upg api class id
  */ 
  NET_UPG_API_CLASS_ID = 57,
   /*! 
   weather api class id
  */ 
  NET_MUSIC_API_CLASS_ID = 58,
   /*! 
   net src svc class id
  */ 
  NET_SRC_SVC_CLASS_ID = 59,
  /*! 
    vod api class id
    */
  HTTPD_SVC_CLASS_ID = 60,
  /*! 
    fifo db class id
    */
  FIFO_DB_CLASS_ID = 61,

  /*!
    log manager class id
    */
  LOG_MGR_CLASS_ID = 62,
  /*!
    Class id for ota class
    */
  EPG_API_CLASS_ID = 63,
  /*!
    Class id for ota class
    */
  OTA_API_CLASS_ID = 64,
  /*!
    Class id for ota dm class
    */
  OTA_DM_CLASS_ID = 65,
  /*!
    Class id for nim virtual
    */
  NIM_VIRTUAL_CLASS_ID = 66,

  /*!
    Class id for nim virtual
    */
  OTA_DSMCC_CLASS_ID = 67,
  /*!
    gui xml class id.
    */
  GUI_XML_CLASS_ID = 68,    
  /*!
    mgui class id
    */
  MGUI_CLASS_ID = 69,
  /*!
    MDI class id
    */
  MDI_CLASS_ID = 70,
  /*!
    ctrl class id.
    */
  MCTRL_CLASS_ID = 71,
  /*! 
    net extern api class id
    */
  NET_EXTERN_API_CLASS_ID = 72,
  /*! 
    net extern api class id
    */
  MEDIA_IP_API_CLASS_ID = 73,
  /*!
    mp3 head analyse class id
    */
  MP3_FRAME_CLASS_ID = 74,
  /*!
    xml skin.
    */
  XML_SKIN_CLASS_ID = 75,  
  /*!
    xml logo.
    */
  XML_LOGO_CLASS_ID = 76,
  /*!
    music api
    */
  MUSIC_API_CLASS_ID = 77,  
  /*!
    Max item number supported in class factory
    */
  CLASS_ID_CNT
}class_id_t;


/*!
  Register class
  \param[in] class_id for all valid module in system
  \param[in] p_class_handle size of class in class factory
  */
void class_register(u8 class_id, class_handle_t p_class_handle);

/*!
  Get the proc function
  \param[in] class_id registered class id
  \param[out] class handle in function sets
  */
class_handle_t class_get_handle_by_id(u8 class_id);
#ifdef __cplusplus
}
#endif

#endif
