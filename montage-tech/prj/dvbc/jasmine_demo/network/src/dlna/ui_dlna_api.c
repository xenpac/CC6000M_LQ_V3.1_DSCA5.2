/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
//#include "ui_dlna_info.h"
#include "ui_mainmenu.h"
#include "ui_video.h"
#include "cdlna.h"
#include "ui_dlna.h"
#include "ui_dlna_api.h"
#include "ui_menu_manager.h"
#include "ui_network_playbar.h"
//#include "ui_netplay_bar.h"
#include "ui_config.h"
#include "sys_cfg.h"
#include "ui_stb_name.h"

#include "mt_url.h"
#include "mt_http.h"
#include "http.h"
#include "pdec.h"
#include "network/mini_httpd/mini_httpd.h"

#ifndef STRCMPI
#ifdef WIN32
#define STRCMPI strcmpi
#else 
#define STRCMPI strcasecmp
#endif
#endif

#define MAX_IMAGE_LENGTH (5*1024*1024)

#define DLNA_VIDEO_MODE 0

#define DLNA_PIC_MODE  1

typedef enum
{
  /*!
    picture
    */
  DLNA_PICTURE,
  /*!
    video
    */
  DLNA_VIDEO,
  /*!
    audio
    */
  DLNA_AUDIO,
  /*!
  other
  */
  DLNA_OTHER,
  /*!
    unrecognize
  */
  DLNA_UNRECOGNIZE,
}dlna_data_t;

typedef struct
{ 
  u8 *name;
  u8 url[DLNA_URL_LEN];
  u8 play_url[DLNA_URL_LEN];
  dlna_data_t data_type;
  u8 mode;
  u8 *p_mete_data;
} dlna_api_status_t;


static BOOL b_inited = FALSE;
static BOOL dlna_enable = FALSE;
static dlna_api_status_t g_dlna_status = {0};

/*
  when set the dlna promotion TRUE, it can receive the vedio and picture;
  when set the dlna promotion FALSE, it can not receive the vedio and picture.
*/
 void set_dlna_promotion(BOOL enable)
{
  if(enable)
    {
      dlna_enable = TRUE;
    }
  else
    {
      dlna_enable = FALSE;
    }

}

pdec_image_format_t get_pic_type(unsigned char *p_buf, unsigned int data_size)
{
  unsigned char *p_data = p_buf;
  pdec_image_format_t type = IMAGE_FORMAT_UNKNOWN;
  switch(p_data[0])
  {
    case 0xff:
      if(p_data[1] == 0xd8)
      {
        type = IMAGE_FORMAT_JPEG;
      }
      break;
    case 0x47:
      if(p_data[1] == 0x49 && p_data[2] == 0x46 && p_data[3] == 0x38)
      {
       type = IMAGE_FORMAT_GIF;
      }
      break;
    case 0x89:
      if(p_data[1] == 0x50 && p_data[2] == 0x4E &&
         p_data[3] == 0x47 && p_data[4] == 0x0D &&
         p_data[5] == 0x0A && p_data[6] == 0x1A &&
         p_data[7] == 0x0A)
      {
       type = IMAGE_FORMAT_PNG;
      }
      break;
    case 0x42:
      if(p_data[1] == 0x4d)
      {
               type = IMAGE_FORMAT_BMP;
      }
      break;
    default:
      break;
  }
  return type;
}
/*
    get type by the url suffix
*/
  static dlna_data_t get_type_by_suffix(char *p_url)
  {
    u16 len = 0;
    if(p_url == NULL)
    {
      return DLNA_UNRECOGNIZE;
    }
    OS_PRINTF(" ui_dlna_api.c is_pic p_url=%s\n",p_url);
    len = (u16)strlen(p_url);
    if(STRCMPI(&p_url[len - 3], "jpg") == 0
      || STRCMPI(&p_url[len - 3], "png") == 0
      || STRCMPI(&p_url[len - 3], "gif") == 0
      || STRCMPI(&p_url[len - 3], "bmp") == 0    
      || STRCMPI(&p_url[len - 4], "jpeg") == 0)
    {
      return DLNA_PICTURE;
    }
    if(STRCMPI(&p_url[len - 3], "aac") == 0
      || STRCMPI(&p_url[len - 3], "mp3") == 0
      || STRCMPI(&p_url[len - 3], "wma") == 0
      )
    {
      return DLNA_AUDIO;
    }
    if(STRCMPI(&p_url[len - 4], "h264") == 0
      || STRCMPI(&p_url[len - 4], "mpeg") == 0
      || STRCMPI(&p_url[len - 5], "mpeg2") == 0
      || STRCMPI(&p_url[len - 3], "mp4") == 0
      || STRCMPI(&p_url[len - 3], "flv") == 0
      || STRCMPI(&p_url[len - 3], "mkv") == 0
      || STRCMPI(&p_url[len - 3], "avi") == 0
      || STRCMPI(&p_url[len - 2], "ts") == 0
      || STRCMPI(&p_url[len - 4], "m3u8") == 0
      || STRCMPI(&p_url[len - 3], "mov") == 0
      )
    {
      return DLNA_VIDEO;
    }
    return DLNA_UNRECOGNIZE;
  }

/*
  get the type by the http header
*/
static dlna_data_t get_type_by_httphead(char* url)
{
  HTTP_header_t *p_hdr = NULL;
  char *type = NULL;
  int fd = 0;
  int length = 0;
  int format_length = 10;
  dlna_data_t ret = DLNA_UNRECOGNIZE;  
  long long  image_length = 0;
  char p_buff[10];
  pdec_image_format_t format = IMAGE_FORMAT_UNKNOWN;
  if((NULL == url)||(strstr(url, "http://") == NULL))
  {
      return ret;
  }
  fd = chunkhttp_download_start(url, &p_hdr);
  if(fd >= 0)
  {
      type = strstr(p_hdr->first_field->field_name, "Content-Type: ");
      if(NULL != type)
       {
        if(strstr(type, "image/"))
           {
               image_length = p_hdr->content_length;
               /*if the picture size is more than 5M,the stb doesn't proceed it*/
               if(image_length > MAX_IMAGE_LENGTH)
                {                  
                  mthttp_http_free(p_hdr);
                  chunkhttp_close(fd);
                  return DLNA_OTHER;
                }               
               ret = DLNA_PICTURE;
           }
           else if(strstr(type, "video/"))
           {
               ret = DLNA_VIDEO;
           }
           else if(strstr(type, "audio/"))
           {
               ret = DLNA_AUDIO;
           }
       }
  }
  if(fd >= 0)
  {
    if(DLNA_UNRECOGNIZE == ret)
      {
         length =  chunkhttp_recv(fd, p_buff, (unsigned int)format_length, p_hdr->isChunked);
         if(length >= format_length)
          {
             format = get_pic_type((unsigned char *)p_buff, (unsigned int)format_length);
             if(format !=0 )
              {
                ret = DLNA_PICTURE;
              }
             else
              {
                ret = DLNA_VIDEO;
              }
          }
      }
    mthttp_http_free(p_hdr);
    chunkhttp_close(fd);
  }
  return ret;
}

static dlna_data_t get_data_type(char* url)
{
    dlna_data_t ret = DLNA_UNRECOGNIZE;  
    ret = get_type_by_httphead(url);
    if(ret <= DLNA_OTHER)
    {
          return ret;
    }
    else
    {
        ret = get_type_by_suffix(url);
        return ret;
    }
}

static dlna_data_t get_metedata_type(char* url)
{
    dlna_data_t ret = DLNA_UNRECOGNIZE;  

    if(strstr(url, "image/"))
    {
        ret = DLNA_PICTURE;
    }
    else if(strstr(url, "video/"))
    {
        ret = DLNA_VIDEO;
    }
    else if(strstr(url, "audio/"))
    {
        ret = DLNA_AUDIO;
    }                
    
    return ret;
}


static int DMR_Callback_Play(char *playSpeed, char *cur_media_duration,   
                                          char *cur_track_duration, int *track_num, int *cur_track)
{ 
int ret = ERR_FAILURE;  
dmr_play_para_t st_play_para = {0};
netplay_dlna_url url_menu = {0};
control_t *p_root = NULL;
OS_PRINTF("\nCALL THE PLAY\n");
if(!dlna_enable)
{
  return DMR_ERROR_REJECTMETADATA;
}
if(0 == memcmp(g_dlna_status.play_url, g_dlna_status.url, DLNA_URL_LEN))
{
  return DMR_ERROR_BADURI;
}
memcpy(g_dlna_status.play_url, g_dlna_status.url, DLNA_URL_LEN);
if((strstr((s8 *)g_dlna_status.play_url, "http://") == NULL))
{
    return DMR_ERROR_BADURI;
}
OS_PRINTF("\nCALL THE PLAY1\n");
if((DLNA_VIDEO == g_dlna_status.data_type) || (DLNA_AUDIO == g_dlna_status.data_type))
  {
    st_play_para.playSpeed = playSpeed;
    st_play_para.cur_media_duration = cur_media_duration;
    st_play_para.cur_track_duration = cur_track_duration;
    st_play_para.track_num = track_num;
    st_play_para.cur_track = cur_track;
	OS_PRINTF("\nCALL THE PLAY2\n");
    memset(&url_menu, 0, sizeof(netplay_dlna_url));
    url_menu.name = g_dlna_status.name;
    url_menu.url = g_dlna_status.play_url;
     p_root = fw_find_root_by_id(ROOT_ID_BACKGROUND);
     fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_DLNA_START_VIDEO, (u32)&url_menu,(u32)&st_play_para);
     g_dlna_status.mode = DLNA_VIDEO_MODE;
  
  }
  else if(DLNA_PICTURE== g_dlna_status.data_type)
  {
   
    p_root = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_DLNA_START_PICTURE, (u32)g_dlna_status.play_url, (u32)(g_dlna_status.url)); 
  }
  return ret;
}

static int DMR_Callback_Stop(void)
{  
  control_t * root  = NULL;
  memset(g_dlna_status.play_url, 0, DLNA_URL_LEN);
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
 if((DLNA_VIDEO == g_dlna_status.data_type) || (DLNA_AUDIO == g_dlna_status.data_type))
  {
    root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR);
    if(root)
    {
     fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_NP_DLNA_PLAY_STOP, 0, 0);
    }
  }
  else if(DLNA_PICTURE == g_dlna_status.data_type)
  {
   pic_stop();
  }
  return 0;
}

static int DMR_Callback_Pause(void)
{  
  control_t * root  = NULL;
  
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_NP_DLNA_PLAY_PAUSE, 0, 0);
  }
  return 0;
}

static int DMR_Callback_Resume(void)
{  
  control_t * root  = NULL;
  
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_NP_DLNA_PLAY_PAUSE, 0, 0);
  }
  return 0;
}

static int DMR_Callback_SeekTrackPosition(long position)
{ 
  control_t * root  = NULL;
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  //ui_video_c_seek(position);  
  root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAYBAR);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_NP_DLNA_PLAY_SEEK, (u32)position, 0);
  }
  return 0;
}

static int DMR_Callback_SetVolume(unsigned char volume)
{  
  u16 act_volume = 0;
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  act_volume = (u16) (((u8)volume * AP_VOLUME_MAX)/100);
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), (u8)act_volume);
  return 0;
}

static int DMR_Callback_SetMute(BOOL mute)
{  
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID), mute, TRUE);
  return 0;
}
static int DMR_Callback_Get_SatIP_Switch_Status(DLNA_OPERATE_CONFIRM_STATUS OpStatus)
{
    control_t * root  = NULL;
    root = fw_find_root_by_id(ROOT_ID_SATIP);
    if(root)
    {
        if(OpStatus == DLNA_OPERATE_OK)
        {
            fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_ON_OFF_SATIP_SUCCESS, 0, 0);
        }
        else
        {
            fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_ON_OFF_SATIP_FAILED, 0, 0);
        }
    }
    return 0;
}
static int DMR_Callback_Get_DLNA_Switch_Status(DLNA_OPERATE_CONFIRM_STATUS OpStatus)
{
    control_t * root  = NULL;
    root = fw_find_root_by_id(ROOT_ID_DLNA_START);
    if(root)
    {
        if(OpStatus == DLNA_OPERATE_OK)
        {
            fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_ON_OFF_DLNA_SUCCESS, 0, 0);
        }
        else
        {
            fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_ON_OFF_DLNA_FAILED, 0, 0);
        }
    }
    return 0;
}

static int DMR_Callback_SetAVTransportURI(char *url)
{ 
  u16 len = 0;
  dlna_data_t type;
  control_t *p_root = NULL;
  if(!dlna_enable)
  {
    return DMR_ERROR_REJECTMETADATA;
  }
  if(NULL == url)
  {
    return DMR_ERROR_BADURI;
  }
  if(0 == strcmp(url, (s8 *)g_dlna_status.url))
  {
    return DMR_ERROR_BADURI;
  }
  type = get_data_type(url);
  
 // OS_PRINTF("#####%s, line[%d], type=[%d], url=[%s]####\n", __FUNCTION__,__LINE__,type,url);
  
    if(g_dlna_status.p_mete_data)
    {
        if(DLNA_UNRECOGNIZE == type)
        {
      //      OS_PRINTF("#####%s, line[%d], metedata=[%s]####\n", __FUNCTION__,__LINE__,g_dlna_status.p_mete_data);
            type = get_data_type((s8 *)g_dlna_status.p_mete_data);
            if(DLNA_UNRECOGNIZE == type)
            {
                type = get_metedata_type((s8 *)g_dlna_status.p_mete_data);
            }   
    //        OS_PRINTF("#####%s, line[%d], type=[%d]####\n", __FUNCTION__,__LINE__,type);
        }
        mtos_free(g_dlna_status.p_mete_data);
        g_dlna_status.p_mete_data = NULL;
    }
  
  g_dlna_status.data_type = type;
 if(DLNA_UNRECOGNIZE == type)
  {
       ui_comm_dlg_close();
       if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
       {
        ui_comm_showdlg_open(NULL, IDS_DATA_UNRECOGNIZE, NULL,2000);
       }
       return DMR_ERROR_BADMETADATA;
  }
 if(DLNA_OTHER == type)
 {
     ui_comm_dlg_close();
     if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
     {
       ui_comm_showdlg_open(NULL, IDS_DATA_LARGE, NULL,2000);
     }

     return DMR_ERROR_BADMETADATA;
 }
 len = (u16)strlen(url);
 if(len == 0)
 {
   return DMR_ERROR_BADURI;
 }  
 if(len > DLNA_URL_LEN - 1)
 {
    len = DLNA_URL_LEN - 1;
 }
 memset(g_dlna_status.url, 0, DLNA_URL_LEN);
 memcpy((void *)g_dlna_status.url, (void *)url, len);
 if((DLNA_AUDIO== g_dlna_status.data_type)||(DLNA_VIDEO== g_dlna_status.data_type))
 {
   if(g_dlna_status.mode == DLNA_PIC_MODE)
   {
     // manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
     p_root = fw_find_root_by_id(ROOT_ID_BACKGROUND);
     fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_DLNA_CLOSE_PICTURE, 0, 0);
   }
   else if(g_dlna_status.mode == DLNA_VIDEO_MODE)
   {
     ui_video_c_stop();
    }
 }
 else if(DLNA_PICTURE== g_dlna_status.data_type)
 {
     if(DLNA_VIDEO_MODE == g_dlna_status.mode)
     {
        ui_video_c_stop();
     }

     g_dlna_status.mode = DLNA_PIC_MODE;
 }
 return DMR_ERROR_OK;
 }

static int DMR_Callback_SetAVTransportMETADATA(char *url)
{ 
  OS_PRINTF("#########[%s], line[%d]############\n", __FUNCTION__, __LINE__);

  if(url != NULL)
  {
 //   OS_PRINTF("#########[%s], line[%d], url=[%s]############\n", __FUNCTION__, __LINE__,url);
    if(g_dlna_status.p_mete_data)
    {
        mtos_free(g_dlna_status.p_mete_data);
    }
    g_dlna_status.p_mete_data = (u8 *)mtos_malloc(strlen(url)+1);
    memset(g_dlna_status.p_mete_data, 0, strlen(url)+1);
    strcpy((s8 *)g_dlna_status.p_mete_data, url);
  }
 
  return 0;
}

static int DMR_Callback_DlnaGetPostionInfo(unsigned int *vpts,  unsigned int *vp_total)
{ 
  u32 play_time;
  u32 total_time;

  play_time = dlna_video_goto_get_net_play_time();
  total_time = dlna_video_get_total_time();
  *vpts = play_time*1000;
  *vp_total = total_time*1000;
  OS_PRINTF("####[%s], line[%d], play time=[%d],total time=[%d]##\n", __FUNCTION__, 
  __LINE__,play_time,total_time);
  
  return 0;
}


int ui_dlna_start(void)
{
  #ifndef WIN32
    return cg_dlna_render_start();
  #else
    return 0;
  #endif
}

int ui_dlna_stop(void)
{
  control_t *p_root = NULL;

  memset(g_dlna_status.play_url, 0, DLNA_URL_LEN);
  memset(g_dlna_status.url, 0, DLNA_URL_LEN);
  if(g_dlna_status.mode == DLNA_PIC_MODE)
  {
    //manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
     p_root = fw_find_root_by_id(ROOT_ID_BACKGROUND);
     fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_DLNA_CLOSE_PICTURE, 0, 0);
  }
  
  #ifndef WIN32
  cg_dlna_user_stop();
  return cg_dlna_render_stop();
  #else
    return 0;
  #endif
}

BOOL ui_get_dlna_init_status(void)
{
  return b_inited;
}

void dlna_init(void)
{
  cg_dlna_config_t config;  
  int ret = DLNA_N_ERROR;
  
  char stb_name[MAX_DEVICE_NAME_LENGTH];
  if(TRUE == b_inited)  
  {    
    return; 
  } 
  
  memset(&config, 0, sizeof(cg_dlna_config_t));    
  #ifndef WIN32
  config.cg_dlna_thread_pool_start = DLNA_STACK_PRIO_START; 
  config.cg_dlna_thread_pool_end = DLNA_STACK_PRIO_END;  
 // config.stack_size = 32*1024;  
 config.stack_size = 32*1024;  
  //config.task_prio = 14;  
  config.task_prio = NETWORK_DLNA_PRIORITY;  
  config.callback.Event_SetAVTransportURI = DMR_Callback_SetAVTransportURI;  
  config.callback.Event_Play = DMR_Callback_Play;  
  config.callback.Event_Stop = DMR_Callback_Stop;  
  config.callback.Event_Pause = DMR_Callback_Pause;  
  config.callback.Event_Resume = DMR_Callback_Resume;  
  config.callback.Event_SeekTrackPosition = DMR_Callback_SeekTrackPosition;  
  config.callback.Event_SetVolume = DMR_Callback_SetVolume;  
  config.callback.Event_SetMute = DMR_Callback_SetMute;
  config.callback.Event_SatIpOperateConfirm = DMR_Callback_Get_SatIP_Switch_Status;
  config.callback.Event_DlnaOperateConfirm = DMR_Callback_Get_DLNA_Switch_Status;
  config.callback.Event_SetAVTransportMETADATA = DMR_Callback_SetAVTransportMETADATA;
  config.callback.Event_DlnaGetPostionInfo = DMR_Callback_DlnaGetPostionInfo;
  sys_status_get_dlna_device_name(stb_name);
  strcpy(config.name, stb_name);
  ret = cg_dlna_init(&config);
  
  #endif
  if(DLNA_N_ERROR == ret)
  {
    b_inited = TRUE;  
  }
}
void ui_cg_dlna_server_stop(void)
{
    #ifndef WIN32
    cg_dlna_server_stop();
    #endif
}
void ui_stop_mini_httpd(void)
{
    #ifndef WIN32
    stop_mini_httpd();
    #endif
}
