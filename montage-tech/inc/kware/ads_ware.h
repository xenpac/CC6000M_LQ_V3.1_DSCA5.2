/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ADS_WARE_H__
#define __ADS_WARE_H__

/*!
the ads max email len
  */
#define ADS_MAX_EMAIL_NUM (100)
/*!
  The ads mail subject text max length, in unit of char
  */
#define ADS_MAIL_SUBJECT_MAX_LEN (11*2)
/*!
  The ads mail sender text max length, in unit of char
  */
#define ADS_MAIL_SENDER_MAX_LEN (11*2)
/*!
  The ads mail body text max length, in unit of char
  */
#define ADS_MAIL_BODY_MAX_LEN (257*2) 

/*!
  Define a macro function to culculate array length in u32
  */
#define AD_EVT_U32_LEN (((ADS_EVT_ALL - 1) >> 4) + 1)

/*!
  The ads max slot number
  */
//#define ADS_MAX_SLOT_NUM 3

/*!
  The event callback structure for AD system module:ADS_S_ status, ADS_O_ operating
  */
typedef enum
{
/*!
    ad event begin
    */
  ADS_EVT_NONE = 0,
/*!
    ad search finished
    */
  ADS_S_ADSERACH_FINISHED = 1,
/*!
    ad show gif
    */
  ADS_O_SHOW_GIF = 2,
/*!
    ad show logo
    */
  ADS_O_SHOW_LOGO = 3,
/*!
    ad write rectangle
    */
  ADS_O_WRITE_RECTANGLE = 4,
/*!
    ad red rectangle
    */
  ADS_O_READ_RECTANGLE = 5,
  /*!
    ad get new pic
    */
  ADS_S_AD_NEW_LOG = 6,
  /*!
    ad get new pic
    */
  ADS_S_AD_NEW_LOG_VER = 7,
  /*!
    ad get new pic
    */
  ADS_S_AD_NEW_PIC = 8,
  /*!
    ad get new pic
    */
  ADS_S_AD_NEW_AD = 9,
/*!
    end,
    */
  ADS_EVT_ALL,
}ads_event_id_t;

/*!
  Define the AD system event call back function's prototype
  */
typedef RET_CODE (*ads_event_cb_t)(u32 id, u32 events[AD_EVT_U32_LEN], u32 param, u32 context);

/*!
  The event callback structure for AD system module
  */
typedef struct
{
/*!    
    The callback function pointer.
    */
  ads_event_cb_t evt_cb;
/*!    
    The context need to transfered by this callback function.
    */
  u32 context;
}ads_event_notify_t;


/*!
  ADS module id
  */
typedef enum
{
  /*!
    TopReal
    */
  ADS_ID_ADT_TR = 0,
  /*!
    iFaboo
    */
  ADS_ID_ADT_WFA,
    /*!
    quan zhi
    */
  ADS_ID_ADT_QZ,
  /*!
    shida
    */
  ADS_ID_ADT_SHIDA,
  /*!
    desai
    */
  ADS_ID_ADT_DESAI,  
  /*!
    xsm
    */
  ADS_ID_ADT_XSM,
/*!
    divi
    */
  ADS_ID_ADT_DIVI,
/*!
    divi
    */
  ADS_ID_ADT_MT,  
  /*!
    divi
    */
  ADS_ID_ADT_ABV,    
  /*!
    The maximal number  of CA system modules
    */
  ADS_ID_ADT_MAX_NUM,
    /*!
    Unkown module
    */
  ADS_UNKNOWN,
}ads_module_id_t;


/*!
  The event callback structure for AD system module
  */
typedef struct
{
  /*!    
      The events happened
    */
  u32 events[ADS_ID_ADT_MAX_NUM][AD_EVT_U32_LEN];
  /*!    
      The flag to indicate on which slot the events happened
    */
  u8 happened[ADS_ID_ADT_MAX_NUM];
}ads_event_info_t;

/*!
  The common AD types of ADS
  */
typedef enum
{
/*!    
    The welcome AD
  */
  ADS_AD_TYPE_WELCOME = 0,
/*!    
    The logo
  */
  ADS_AD_TYPE_LOGO,
/*!    
    The radio AD
  */
  ADS_AD_TYPE_RADIO,
/*!    
    The channel-bar AD
  */
  ADS_AD_TYPE_CHBAR,
/*!    
    The channel-list AD
  */
  ADS_AD_TYPE_CHLIST,
/*!    
    The upper part main menu AD
  */
  ADS_AD_TYPE_MAIN_MENU,
/*!    
    The upper part menu AD
  */
  ADS_AD_TYPE_MENU_UP,
/*!    
    The lower half menu AD
  */
  ADS_AD_TYPE_MENU_DOWN,
/*!    
    The volume-bar AD
  */
  ADS_AD_TYPE_VOLBAR,
/*!    
    The subtitle AD
  */
  ADS_AD_TYPE_SUBTITLE,
/*!    
    The vod AD
  */
  ADS_AD_TYPE_VOD,
/*!    
    The CA AD
  */
  ADS_AD_TYPE_CA,
/*!    
    The banner AD
  */
  ADS_AD_TYPE_BANNER,
  /*!    
    The banner AD TEXT
  */
  ADS_AD_TYPE_BANNER_TEXT,
/*!    
    The other AD
  */
  ADS_AD_TYPE_OTHER_GIF,
/*!    
    The channel change AD
  */
  ADS_AD_TYPE_CHANGE_CHANNEL,
/*!    
    The sub menu AD
  */
  ADS_AD_TYPE_SUB_MENU,
/*!    
    The favorite pro AD
  */
  ADS_AD_TYPE_FAVORITEPRO,
/*!    
    The channel freq AD
  */
  ADS_AD_TYPE_CHANGE_FREQ,
/*!    
    The searching AD
  */
  ADS_AD_TYPE_SEARCHING,
/*!    
    The recovery signal AD
  */
  ADS_AD_TYPE_SIGNAL,
/*!    
    The no signal AD
  */
  ADS_AD_TYPE_NOSIGNAL,
/*!    
    The unauthorized AD
  */
  ADS_AD_TYPE_NORIGHT,
/*!    
    The left channel AD
  */
  ADS_AD_TYPE_LEFTCHANNEL,
/*!    
    The right channel AD
  */
  ADS_AD_TYPE_RIGHTCHANNEL,
/*!    
    The stereo AD
  */
  ADS_AD_TYPE_STEREOPHONIC,
/*!    
    The OSD ad
  */
  ADS_AD_TYPE_OSD,
/*!    
    The PIC AD
  */
  ADS_AD_TYPE_PIC,
/*!    
    The AV AD
  */
  ADS_AD_TYPE_AV,
  /*!    
    The channel manage AD
  */
  ADS_AD_TYPE_MANAGE_CHANNEL,
  /*!    
    The broadcast-list AD
  */
  ADS_AD_TYPE_BROADCAST_LIST,
  /*!    
    The epg-list AD
  */
  ADS_AD_TYPE_EPG,
  /*!    
    The ungency notice AD
  */
  ADS_AD_TYPE_URGENCY_NOTICE,
  /*!    
    The menu AD 1
  */
  ADS_AD_MENU_ONE,
  /*!    
    The menu AD 1
  */
  ADS_AD_MENU_TWO,
  /*!    
    The menu AD 1
  */
  ADS_AD_MENU_THREE,
  /*!    
    The menu AD 1
  */
  ADS_AD_MENU_FOUR,
  /*!    
    The unauthed ad for desai
  */
  ADS_AD_TYPE_UNAUTH_PG,
  /*!    
    ad power off
  */
  ADS_AD_POWER_OFF,
  /*!    
    The reserved AD
  */
  ADS_AD_TYPE_NUM,
}ads_ad_type_t;

/*!
  The AD pic data save location
  */
typedef enum
{
  /*!
    In Flash
    */
  ADS_PIC_IN_FLASH = 1,
  /*!
    In SDRAM
    */
  ADS_PIC_IN_SDRAM,
}ads_pic_in_t;

/*!
  The ioctrl cmd for ADS system module
  */
typedef enum
{
/*!    
    Get ad pic exist info(for Topreal)
    */
  ADS_IOCMD_AD_PIC_INFO_GET = 0,
  /*!    
    get pic number
    */
  ADS_IOCMD_AD_GET_PIC_NUM,
  /*!    
    get logo info
    */
  ADS_IOCMD_AD_GET_LOGO_INFO,
   /*!    
     logo updata finish to free ram
    */
  ADS_IOCMD_AD_FINISH_LOGO,
/*!    
    Set search AD(for Topreal)
    */
  ADS_IOCMD_AD_SEARCH_SET,
/*!    
    Set search AD(for Topreal)
    */
  ADS_IOCMD_AD_KEY_GET,
 /*!    
    show ads without singal
    */ 
  ADS_IOCMD_AD_GET_FLASH,
  /*!    
    Exit when timeout 
  */ 
  ADS_IOCMD_AD_EXIT_TIMEOUT,
  /*!    
    Get AD non-association program
  */ 
  ADS_IOCMD_AD_NO_PROGRAM,
  /*!    
    Get AD association program
  */ 
  ADS_IOCMD_AD_PROGRAM,
  /*!    
    Get AD av pid
  */ 
  ADS_IOCMD_AD_AV_PID,
  /*!    
    Get osd AD
  */ 
  ADS_IOCMD_AD_OSD,
  /*!    
  Get version(for desai)
  */ 
  ADS_IOCMD_AD_VERSION_GET,
  /*!    
  Get pic data(for MTADS)
  */ 
  ADS_IOCMD_AD_RES_GET,
  /*!    
  Change channel
  */ 
  ADS_IOCMD_AD_CHANGE_CHANNEL,
  /*!    
  Change freq
  */ 
  ADS_IOCMD_AD_CHANGE_FREQ
}ads_ioctrl_cmd_t;

/*!
  filenamesize
  */
#define FILENAMESIZE      160

/*!
  fileversioninfo
  */
#define FILEVERSIONINFO   28

/*!
    Stream_FileHead
  */ 
typedef struct
{
/*!    
    AD
    */
  u8  sFileName[FILENAMESIZE];
/*!    
    AD
    */
  u8  sFileDes[FILEVERSIONINFO];
/*!    
    AD
    */
  u8  dwFileVer;
/*!    
    AD
    */
  u32  dwFileSize;
/*!    
    AD
    */
  u8  dwFileType;
}ads_file_head_t;
/*!
    The position info of ad pic
  */ 
typedef struct
{
/*!    
    AD res id
    */
  u8  id;
/*!    
    AD pic reserve1
    */
  ads_file_head_t  *p_head;
/*!    
    AD pic reserve2
    */
  u8  *p_data;
}ads_res_t;

/*!
    The position info of ad pic
  */ 
typedef struct
{
/*!    
    AD pic type
    */
  u8  type;
/*!    
    AD pic reserve1
    */
  u8  reserve1;
/*!    
    AD pic reserve2
    */
  u16  reserve2;
/*!    
    AD pic x position
    */
  s16 x;
/*!    
    AD pic y position
    */
  s16 y;
}ads_pic_position_t;

/*!
    The info of change channel
  */ 
typedef struct
{
/*!    
    Service id
    */
  u16 service_id;
/*!    
    Ts id
    */
  u16 ts_id;
/*!    
    Network id
    */
  u16 network_id;
}ads_service_t;

/*!
    The info of av file
  */ 
typedef struct
{
/*!    
    video pid
    */
  u16 v_pid;
/*!    
    audio pid
    */
  u16 a_pid;
/*!    
    pcr pid
    */
  u16 pcr_pid;
}ads_av_info_t;

/*!
    The info of ad pic to show
  */ 
typedef struct
{
/*!
    The pic type
  */
  u8  pic_type;
/*!
    The pic index
  */
  u8 pic_index;
/*!    
    The pic x position
    */
  s16 pic_x;
/*!    
    The pic y position
    */
  s16 pic_y;
/*!    
    The src address point of pic data
    */
  u8 *p_pic_src_addr;
/*!    
    The pic data length
    */
  u32 pic_length;
/*!    
    The pic refresh or not
    */
  u8 pic_refresh;
}ads_pic_info_t;

/*!
    The info of Rectangle to drow
  */ 
typedef struct
{
/*!
    The Rectangle x position
  */
  u32 rec_x;
/*!
    The Rectangle y position
  */
  u32 rec_y;
/*!
    The Rectangle width
  */
  u32 rec_width;
/*!
    The Rectangle height
  */
  u32 rec_height;
/*!    
    The direction address point of rect data
    */
  u8 *p_rec_addr;
}ads_rec_info_t;

/*!
    The info of WANFA ads key definition
  */
typedef enum
{
  ADS_KEY_TYPE_POWER = 0,

  ADS_KEY_TYPE_MUTE,

  ADS_KEY_TYPE_NUM0,

  ADS_KEY_TYPE_NUM1,

  ADS_KEY_TYPE_NUM2,

  ADS_KEY_TYPE_NUM3,

  ADS_KEY_TYPE_NUM4,

  ADS_KEY_TYPE_NUM5,

  ADS_KEY_TYPE_NUM6,

  ADS_KEY_TYPE_NUM7,

  ADS_KEY_TYPE_NUM8,

  ADS_KEY_TYPE_NUM9,

  ADS_KEY_TYPE_UP,

  ADS_KEY_TYPE_DOWN,

  ADS_KEY_TYPE_LEFT,

  ADS_KEY_TYPE_RIGHT,

  ADS_KEY_TYPE_SELECT,

  ADS_KEY_TYPE_MENU,

  ADS_KEY_TYPE_EXIT,

  ADS_KEY_TYPE_BACK,

}ads_key_type_t;

/*!
    Platform type
  */
typedef enum
{
/*!
    SD Platform 
  */
  ADS_PLATFORM_SD = 1,
/*!
    HD Platform 
  */
  ADS_PLATFORM_HD = 2,
/*!
    Reserved Platform 
  */
  ADS_PLATFORM_UNKNOWN
}ads_platform_type_t;

/*!
  The ADS module configuation parameters
  */
typedef struct
{
/*!
    Platform type
  */
  ads_platform_type_t platform_type;
/*!
    AD type
  */
  ads_ad_type_t ad_type;
/*!
    The info of change channel
  */ 
  ads_service_t service_info;
/*!
    The info of av
  */ 
  ads_av_info_t av_info;
/*!
    The position info of ad pic
  */ 
  ads_pic_position_t pic_pos;
/*!
   The pic data save in
  */
  ads_pic_in_t pic_in;
/*!
   The key type
  */
  ads_key_type_t key_type;
/*!    
    task priority start, if is 0, means no task needed
  */
  u8 task_prio_start;
/*!    
    task priorityend, if is 0, means no task needed
  */
  u8 task_prio_end;
/*!    
    task stack pointer
  */
  u32 *p_task_stack;  
/*!    
    task stack size
  */
  u32 stack_size;   
/*!    
   flash_start_adr
  */
  u32 flash_start_adr;  
/*!    
   flash_size
  */
  u32 flash_size;
/*!    
    The handle of demux device
  */
  void *p_dmx_dev;
/*!    
   Channel_frequency
  */
  u32 channel_frequency;  
/*!    
   Channel_symbolrate
  */
  u32 channel_symbolrate;
/*!    
   Qam
  */
  u16 channel_qam;
/*!    
   pid 
  */
  u16 pid;
/*!    
   dii_timeout
  */
  s32 dii_timeout;
/*!    
   ad_timeout
  */
  s32 ad_timeout;
/*!    
   ads prviate data for ads extent
  */
  u16 *ads_prv;
/*!    
    The callback of nvram reading function
  */
  RET_CODE (*nvram_read)(u32 offset, 
              u8 *p_buf, u32 *size);
/*!    
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(u32 offset, 
              u8 *p_buf, u32 size);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase)(u32 length);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase2)(u32 offset, u32 length);
/*!    
    The callback of read gif data function
    */
  RET_CODE (*read_gif)(u32 length, u8 *p_data);
/*!    
    The callback of read float data function
    */
  RET_CODE (*read_float)(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data);
/*!    
    The callback of read rec function
    */
  RET_CODE (*read_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);
/*!    
    The callback of write rec function
    */
  RET_CODE (*write_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);
/*!    
    The callback of display osd, picture,av
    */
  RET_CODE (*display_ad)(ads_ad_type_t ad_type, u8 *p_data);
/*!    
    The callback of hide current display ad
    */
  RET_CODE (*hide_ad)(ads_ad_type_t ad_type, u8 *p_data);
/*!    
    The callback of get adv version function
    */
  u32 (*get_shida_adv_version)(u8 shida_type);
/*!    
    The callback of set adv version function
    */
  void (*set_shida_adv_version)(u32 shida_version,u8 shida_type);
/*!    
    The callback of get current data
    */
  RET_CODE (*get_date)(u32 *date);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_time)(u32 *time);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_channel_info)(u16 *tsid, u16 *sid, u16 *nid);
  /*!    
    The callback of read ads(pic | text) data function
    */
  RET_CODE (*read_data)(void *priv);

}ads_module_cfg_t;

/*!
  The ADS module priv data
  */
typedef struct
{
/*!    
    module id
    */
  u8 id;
/*!    
    module version
    */
  u8 *version;
/*!    
    AD URL (for VC type is u8*)
    */
  u8 *p_ad_url;
/*!    
    welcome ad showtime
    */
  u32 show_time;
/*!    
    task priority start, if is 0, means no task needed
  */
  u8 task_prio_start;
/*!    
    task priorityend, if is 0, means no task needed
  */
  u8 task_prio_end;
/*!    
   flash_start_adr
    */
  u32 flash_start_adr;  
/*!    
   flash_size
    */
  u32 flash_size;
/*!    
   Channel_frequency
  */
  u32 channel_frequency;  
/*!    
   Channel_symbolrate
  */
  u32 channel_symbolrate;
/*!    
   Qam
  */
  u16 channel_qam;
/*!    
    The handle of demux device
    */
  void *p_dmx_dev;
/*!    
    The callback of nvram reading function
    */
  RET_CODE (*nv_read)(u32 offset, u8 *p_buf, u32 *size);
/*!    
    The callback of nvram writing function
    */
  RET_CODE (*nv_write)(u32 offset, u8 *p_buf, u32 size);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nv_erase)(u32 length);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nv_erase2)(u32 offset, u32 length);
/*!    
    The callback of read gif data function
    */
  RET_CODE (*read_gif)(u32 length, u8 *p_data);
/*!    
    The callback of read float data function
    */
  RET_CODE (*read_float)(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data);
/*!    
    The callback of read rec function
    */
  RET_CODE (*read_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);
/*!    
    The callback of write rec function
    */
  RET_CODE (*write_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);
/*!    
    The callback of display osd, picture,av
    */
  RET_CODE (*display_ad)(ads_ad_type_t ad_type, u8 *p_data);
/*!    
    The callback of hide current display ad
    */
  RET_CODE (*hide_ad)(ads_ad_type_t ad_type, u8 *p_data);
/*!    
    The callback of get adv version function
    */
  u32 (*get_adv_version)(u8 type);
/*!    
    The callback of set adv version function
    */
  void (*set_adv_version)(u32 version,u8 type);

/*!    
    The callback of get current data
    */
  RET_CODE (*get_date)(u32 *date);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_time)(u32 *time);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_channel_info)(u16 *tsid, u16 *sid, u16 *nid);
/*!    
    The callback of read ads(pic | text) data function
    */
  RET_CODE (*read_data)(void *priv);
}ads_module_priv_t;

/*!
  Attach the ADS module: Topreal
  \param[out] p_cam_id The attached ADS handle
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_adt_tr_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);
/*!
  Attach the ADS module: quanzhi
  \param[out] p_cam_id The attached ADS handle
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_adt_qz_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);
/*!
  Attach the ADS module: shida
  \param[out] p_cam_id The attached ADS handle
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_adt_shida_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);
/*!
  Attach the ADS module: desai
  \param[out] p_cam_id The attached ADS handle
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_adt_desai_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);

/*!
  Detach the ADS module
  \param[in] adm_id ADS module id  
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_detach(u32 adm_id);

/*!
  Initialize a specific ADS module
  \param[in] adm_id ADS module id
  \param[in] p_cfg The configuation of ADS adapter
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_init(u32 adm_id, ads_module_cfg_t *p_cfg);

/*!
  De-Initialize a specific ADS module
  \param[in] adm_id ADS module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_deinit(u32 adm_id);

/*!
  Open a specific ADS module
  \param[in] adm_id ADS module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_open(u32 adm_id);

/*!
  Close a specific ADS module
  \param[in] adm_id ADS module id
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_close(u32 adm_id);

/*!
  Display a specific AD
  \param[in] adm_id ADS module id
  \param[in] p_cfg The configuation of ADS module
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_display(u32 adm_id, ads_module_cfg_t *p_cfg);

/*!
  Hide a specific AD
  \param[in] adm_id ADS module id
  \param[in] p_cfg The configuation of ADS module
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_hide(u32 adm_id, ads_module_cfg_t *p_cfg);

/*!
  IO control function for this ADS module
  \param[in] adm_id ADS module id
  \param[in] cmd The IO command, see ads_ioctrl_cmd_t 
  \param[in] p_param The parameter of this command  
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_io_ctrl(u32 adm_id, u32 cmd, void *p_param);

/*!
  Register event notify function for CAS module
  \param[in] p_notify The callback function information                                      
  
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_event_notify_register(ads_event_notify_t *p_notify);

/*!
  Polling the current event of this ADS module 
  \param[out] events All the events happened in type of bit mask
  
  \return Return 0 for success and others for failure.
  */
extern RET_CODE ads_event_polling(ads_event_info_t *p_event_info);

/*!
  Parse the event from bit mask to the value defined in ads_event_t
  when events[2] filled in multi event bits, 
  the function can be called multi-times until return 0 means no event can be parsed.
  \param[in] events[2] The event bit mask, maybe filled in multi event bits. 
  
  \return Return the value defined in cas_event_t, if return 0, means no event can be parsed
  */
extern ads_event_id_t ads_event_parse(u32 events[AD_EVT_U32_LEN]);

#endif //__ADS_WARE_H__
