/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
  \file avctrl.c

  This file defines interfaces to control video and audio during playing
  program.
  */
#include <assert.h>
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "class_factory.h"

#include "common.h"
#include "lib_rect.h"
#include "drv_dev.h"
#include "vdec.h"
#include "display.h"
//#include "video.h"
//#include "audio.h"
#include "aud_vsb.h"
#include "rf.h"
#include "dmx.h"
#include "scart.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"

#include "class_factory.h"

//#include "osd.h"
//#include "gpe.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

#include "mdl.h"
#include "mmi.h"
#include "gdi.h"
#include "avctrl1.h"
//#include "osd.h"
#include "drv_misc.h"
#include "hal_misc.h"
#include "subtitle_api.h"
#include "vbi_api.h"
#include "hdmi.h"
#include "err_check_def.h"

/*!
  AVC global variables
  */
typedef struct 
{
  /*!
    AVC screen position callback
    */
  avc_callback_screen_pos_1_t p_notify_screen_proc;
  /*!
    AVC mute status
    */
  BOOL is_avc_mute;
  /*!
    Audio device
    */
  void *p_audio_dev;
  /*!
    Video device
    */
  void *p_video_dev;
  /*!
    display device
    */
  void *p_disp_dev;
  /*!
    Rf device
    */
  rf_device_t *p_rf_dev;
  /*!
    Scart device
    */
  scart_device_t *p_scart_dev;
  /*!
    disp channel
    */
  disp_channel_t disp_chan;

} avc_var_t;

void avc_init_1(avc_cfg_t *p_avc_cfg)
{
  avc_var_t *p_avc_var = mtos_malloc(sizeof(avc_var_t));
  CHECK_FAIL_RET_VOID(p_avc_var != NULL);

  memset(p_avc_var,0,sizeof(avc_var_t));
  class_register(AVC_CLASS_ID, p_avc_var);

  p_avc_var->disp_chan = p_avc_cfg->disp_chan;

  p_avc_var->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_AUDIO);
  CHECK_FAIL_RET_VOID(p_avc_var->p_audio_dev != NULL);
  
  p_avc_var->p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);
  CHECK_FAIL_RET_VOID(p_avc_var->p_video_dev != NULL); 

  p_avc_var->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_DISPLAY);

  // modify by li yang , in ui init ,we will reset TV mode based on ss_data
  //disp_set_tv_sys(p_avc_var->p_disp_dev, p_avc_cfg->disp_chan, p_avc_cfg->disp_fmt);

  p_avc_var->p_rf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_RF);

  p_avc_var->p_scart_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_SCART);
}

void avc_setvolume_1(void *p_data, u8 volume)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  
  //dev_io_ctrl(p_avc_data->p_audio_dev, AUDIO_CMD_SET_VOLUME, volume);
  aud_set_volume_vsb(p_avc_data->p_audio_dev, volume, volume);
}

BOOL avc_is_mute_1(void *p_data)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);

  return p_avc_data->is_avc_mute;
}

void avc_set_mute_1(void *p_data, BOOL is_mute, BOOL is_update)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  //dev_io_ctrl(p_avc_data->p_audio_dev, AUDIO_CMD_SET_MUTE, is_mute);
  aud_mute_onoff_vsb(p_avc_data->p_audio_dev,is_mute);
  if(is_update && (is_mute != p_avc_data->is_avc_mute))
  {
    p_avc_data->is_avc_mute = is_mute;
  }
}

void avc_set_audio_mode_1(void *p_data, avc_audio_mode_1_t mode)
{
  aud_mode_vsb_t drv_mode = AUDIO_MODE_MONO;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  switch(mode)
  {
    case AVC_AUDIO_STEREO_1:
      drv_mode = AUDIO_MODE_STEREO;
      break;
    case AVC_AUDIO_LEFT_1:
      drv_mode = AUDIO_MODE_LEFT;
      break;
    case AVC_AUDIO_RIGHT_1:
      drv_mode = AUDIO_MODE_RIGHT;
      break;
    case AVC_AUDIO_MONO_1:
      drv_mode = AUDIO_MODE_MONO;
      break;

    default:
      return;
  }
  //dev_io_ctrl(p_avc_data->p_audio_dev, AUDIO_CMD_SET_CHANNEL, drv_mode);
  aud_set_play_mode_vsb(p_avc_data->p_audio_dev, drv_mode);
}
      
/*!
   Install notify callback functions.
 \param[in]  proc Function address.
 \return Return TRUE if succeed. FALSE for failed.
 */
BOOL avc_install_screen_notification_1(void *p_data, 
  avc_callback_screen_pos_1_t p_proc)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);
  
  p_avc_data->p_notify_screen_proc = p_proc;
  return TRUE;
}

/*!
   PAL/NTSC switch for OSD layer. This fuction modify UI move osd region position
   according to the video PAL/NTSC status,

   \param[in] is_ntsc TRUE is NTSC; FALSE is PAL.
 */
void avc_reset_screen_pos_1(void *p_data, BOOL is_ntsc)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  
  if(NULL != p_avc_data->p_notify_screen_proc)
  {
    p_avc_data->p_notify_screen_proc(is_ntsc);
  }
}

/*!
  Play a program in an window

  \param[in] left Left position of window in pixel
  \param[in] top Top position of window in pixel
  \param[in] width Width of window in pixel
  \param[in] height Height of window in pixel
  \param[out] : confirm video format size is correct ;
  */
RET_CODE avc_enter_preview_1(void *p_data, u16 left, u16 top, u16 right, 
u16 bottom)
{
  rect_t rect;
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_CODE(p_avc_data != NULL);
  
  OS_PRINTF("avc-------x = %d\n-------y = %d\n-------w = %d\n-------h = %d\n",
      left, top, right, bottom);
  rect.left = left;
  rect.top = top;
  rect.right = right;
  rect.bottom = bottom;
    OS_PRINTF("avc_enter_preview_1 chan %d\n",p_avc_data->disp_chan);
  while((ret = disp_vid_zoom_in(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &rect)) != SUCCESS)
  {
    mtos_task_delay_ms(10);
    OS_PRINTF("disp_vid_zoom_in is failed, try again\n");
  }
  
  return ret;
}

/*!
  Config a program in an window, it works after pb_enter_preview

  \param[in] left Left position of window in pixel
  \param[in] top Top position of window in pixel
  \param[in] width Width of window in pixel
  \param[in] height Height of window in pixel
  */
void avc_config_preview_1(void *p_data, u16 left, u16 top, u16 right, 
u16 bottom)
{
  avc_enter_preview_1(p_data, left, top, right, bottom);
}

/*!
  Leave preview mode
  */
void avc_leave_preview_1(void *p_data)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  
  while(disp_vid_zoom_exit(p_avc_data->p_disp_dev, p_avc_data->disp_chan) != SUCCESS)
  {
    mtos_task_delay_ms(10);
    OS_PRINTF("disp_vid_zoom_exit is failed, try again\n");
  }
}

static void get_sd_tv_mode_from_hd(disp_sys_t *p_sd, disp_sys_t hd)
{
  switch(hd)
  {
    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
    case VID_SYS_PAL_M:
    case VID_SYS_576P_50HZ:
    case VID_SYS_720P_50HZ:
    case VID_SYS_1080I_50HZ:
    case VID_SYS_1080P_50HZ:
      *p_sd = VID_SYS_PAL;
      break;
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_480P:
    case VID_SYS_720P:
    case VID_SYS_1080I:
    case VID_SYS_1080P:
      *p_sd = VID_SYS_NTSC_M;
      break;
    default:
      *p_sd = VID_SYS_PAL;
      break;
  }
}

static void get_hd_tv_mode_from_sd(disp_sys_t sd, disp_sys_t hd_old, disp_sys_t *p_hd_new)
{
    if((sd == VID_SYS_PAL) ||(sd == VID_SYS_PAL_N)
      ||(sd == VID_SYS_PAL_NC))
    {
      switch(hd_old)
      {
         case VID_SYS_1080P_50HZ:
         case VID_SYS_1080P:
          *p_hd_new = VID_SYS_1080P_50HZ;
           break;
         case VID_SYS_1080I_50HZ:
         case VID_SYS_1080I:
          *p_hd_new = VID_SYS_1080I_50HZ;
           break;
         case VID_SYS_720P_50HZ:
         case VID_SYS_720P:
           *p_hd_new = VID_SYS_720P_50HZ;
           break;
         case VID_SYS_576P_50HZ:
          case VID_SYS_480P:
            *p_hd_new = VID_SYS_576P_50HZ;
            break;
         case VID_SYS_PAL:
         case VID_SYS_PAL_N:
         case VID_SYS_PAL_NC:
         case VID_SYS_PAL_M:
         case VID_SYS_NTSC_J:
         case VID_SYS_NTSC_M:
         case VID_SYS_NTSC_443:
          *p_hd_new = VID_SYS_PAL;
            break;
        default:
          *p_hd_new = VID_SYS_1080I_50HZ;
           break;
      }
    }
    else
    {
      switch(hd_old)
      {
         case VID_SYS_1080P_50HZ:
         case VID_SYS_1080P:
          *p_hd_new = VID_SYS_1080P;
           break;
         case VID_SYS_1080I_50HZ:
         case VID_SYS_1080I:
          *p_hd_new = VID_SYS_1080I;
           break;
         case VID_SYS_720P_50HZ:
         case VID_SYS_720P:
           *p_hd_new = VID_SYS_720P;
           break;
         case VID_SYS_576P_50HZ:
          case VID_SYS_480P:
            *p_hd_new = VID_SYS_480P;
            break;
         case VID_SYS_PAL:
         case VID_SYS_PAL_N:
         case VID_SYS_PAL_NC:
         case VID_SYS_PAL_M:
         case VID_SYS_NTSC_J:
         case VID_SYS_NTSC_M:
         case VID_SYS_NTSC_443:
          *p_hd_new = VID_SYS_NTSC_M;
            break;
        default:
          *p_hd_new = VID_SYS_1080I;
           break;
      }
    }
}



/*!
  set video mode
  */
void avc_switch_video_mode_1(void *p_data, avc_video_mode_1_t new_std)
{
  disp_sys_t video_mode;
  vdec_info_t v_info;
  aspect_ratio_t video_aspect;
  BOOL b_switch = FALSE;
  disp_channel_t disp_chan = DISP_CHANNEL_SD;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  BOOL is_ntsc = FALSE;
  disp_sys_t sd_mode = VID_SYS_PAL;
  disp_sys_t hd_mode = VID_SYS_720P;
  disp_sys_t hd_old = VID_SYS_720P;
  disp_common_info_t disp_info = {0};
  
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(new_std >= AVC_VIDEO_MODE_480P)
  {
    disp_chan = DISP_CHANNEL_HD;
  }
  disp_get_tv_sys(p_avc_data->p_disp_dev, disp_chan, &video_mode);
  disp_get_aspect_ratio(p_avc_data->p_disp_dev, disp_chan, &video_aspect);

  switch(new_std)
  {
    case AVC_VIDEO_MODE_NTSC_1:
      if(video_mode != VID_SYS_NTSC_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_1:
      if(video_mode != VID_SYS_PAL)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL;
      }
      break;
    case AVC_VIDEO_MODE_PAL_M_1:
      if(video_mode != VID_SYS_PAL_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_N_1:
      if(video_mode != VID_SYS_PAL_N)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL_N;
      }
      break;
    case AVC_VIDEO_MODE_480P:
      if(video_mode != VID_SYS_480P)
      {
        video_mode = VID_SYS_480P;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_576P:
      if(video_mode != VID_SYS_576P_50HZ)
      {
        video_mode = VID_SYS_576P_50HZ;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_720P_60HZ:
      if(video_mode != VID_SYS_720P)
      {
        video_mode = VID_SYS_720P;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_720P_50HZ:
      if(video_mode != VID_SYS_720P_50HZ)
      {
        video_mode = VID_SYS_720P_50HZ;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_1080I_50HZ:
      if(video_mode != VID_SYS_1080I_50HZ)
      {
        video_mode = VID_SYS_1080I_50HZ;
        b_switch = TRUE;
      }
      break;
     case AVC_VIDEO_MODE_1080I_60HZ:
      if(video_mode != VID_SYS_1080I)
      {
        video_mode = VID_SYS_1080I;
        b_switch = TRUE;
      }
      break;  
     case AVC_VIDEO_MODE_1080P_60HZ:
      if(video_mode != VID_SYS_1080P)
      {
        video_mode = VID_SYS_1080P;
        b_switch = TRUE;
      }
      break;   
     case AVC_VIDEO_MODE_1080P_50HZ:
      if(video_mode != VID_SYS_1080P_50HZ)
      {
        video_mode = VID_SYS_1080P_50HZ;
        b_switch = TRUE;
      }
      break;   
     case AVC_VIDEO_MODE_480I:
      if(video_mode != VID_SYS_NTSC_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_576I:
      if(video_mode != VID_SYS_PAL)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL;
      }
      break;
    case AVC_VIDEO_MODE_AUTO_1:
      vdec_get_info(p_avc_data->p_video_dev, &v_info);
      if(v_info.vid_format == VID_SYS_MAX || v_info.err == VDEC_ERROR_THIRSTY
        || v_info.is_format != TRUE)
      {
        break;
      }
      #if 0
      if(v_info.vid_format >= VID_SYS_1080I)
      {
        disp_chan = DISP_CHANNEL_HD;
        disp_get_tv_sys(p_avc_data->p_disp_dev, disp_chan, &video_mode);
      }

      if((video_mode != v_info.vid_format))
      {
        b_switch = TRUE;
        video_mode = v_info.vid_format;
      }
      #else
   //   OS_PRINTF("========v_mode %d", v_info.vid_format);
      if(v_info.vid_format >= VID_SYS_1080I)
      {
        get_sd_tv_mode_from_hd(&video_mode, v_info.vid_format);
      //  OS_PRINTF("========sd  v_mode %d", video_mode);
      }
      else
      {
        video_mode = v_info.vid_format;
      }
      disp_get_tv_sys(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, &sd_mode);
      if((video_mode != sd_mode))
      {
        b_switch = TRUE;
      }
      #endif
      break;
    default:
      CHECK_FAIL_RET_VOID(0);
      break;
  }



  if(b_switch == TRUE)
  {

    disp_get_common_info(p_avc_data->p_disp_dev, &disp_info);

    if(disp_info.ch_num > 1)//HD IC, warriors/sonata
    {

      if(video_mode >= VID_SYS_1080I)
      {
        get_sd_tv_mode_from_hd(&sd_mode, video_mode);
        hd_mode = video_mode;
      }
      else
      {
        if((new_std == AVC_VIDEO_MODE_480I)
          ||(new_std == AVC_VIDEO_MODE_576I))
        {
          hd_mode = video_mode;
        }
        else
        {
          disp_get_tv_sys(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, &hd_old);
          get_hd_tv_mode_from_sd(video_mode, hd_old, &hd_mode);
        }
        
        sd_mode = video_mode;
      }
      {
  #ifndef WIN32
          drv_dev_t *p_hdmi_dev = dev_find_identifier(NULL,
                                  DEV_IDT_TYPE,
                                  SYS_DEV_TYPE_HDMI);
       hdmi_av_mute(p_hdmi_dev, TRUE);
       mtos_task_delay_ms(100);
  #endif
       disp_set_tv_sys(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, hd_mode);
  #ifndef WIN32
       hdmi_av_mute(p_hdmi_dev, FALSE);
  #endif

       
      }

    }
    else
    {
        sd_mode = video_mode;
    }
    disp_set_tv_sys(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, sd_mode);

    OS_PRINTF("set video std sd: %d  hd:  %d======================\n", sd_mode, hd_mode);


  ///todo: confirm aspect ratio
    disp_set_aspect_ratio(p_avc_data->p_disp_dev, disp_chan, video_aspect);

    disp_get_tv_sys(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, &video_mode);

    if((VID_SYS_NTSC_M == video_mode)
      ||(VID_SYS_PAL_M == video_mode)
      ||(VID_SYS_NTSC_J == video_mode)
      ||(VID_SYS_NTSC_443 == video_mode))
    {
      subt_set_video_std_vsb(VID_STD_NTSC);
      vbi_set_video_std_vsb(VID_STD_NTSC);
      is_ntsc = TRUE;
    }
    else if(VID_SYS_PAL == video_mode 
      || VID_SYS_PAL_N == video_mode)
    {
      subt_set_video_std_vsb(VID_STD_PAL);
      vbi_set_video_std_vsb(VID_STD_PAL);
      is_ntsc = FALSE;
    }

    avc_reset_screen_pos_1(p_data, is_ntsc);
  }
}

disp_sys_t avc_get_video_mode_1(void *p_data)
{
  disp_sys_t video_mode = VID_SYS_PAL;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET((p_avc_data != NULL), video_mode);

  disp_get_tv_sys(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &video_mode);
  return video_mode;
}
/*!
  set video mode
  */
void avc_set_video_aspect_mode_1(void *p_data, avc_video_aspect_1_t new_aspect)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  aspect_ratio_t aspect_ratio = AR_SQUARE;
  disp_vid_aspect_mode_t aspect_mode = VID_ASPECT_MODE_ORIG;
  vdec_info_t v_info = {0};
  chip_ic_t chip_ver = IC_WARRIORS;
  
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  disp_get_aspect_ratio(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &aspect_ratio);
  disp_vid_get_aspect_mode(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &aspect_mode);
  switch(new_aspect)
  {
    case AVC_VIDEO_ASPECT_43_PANSCAN_1:
      if (aspect_ratio != AR_43)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_43);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_43);
      }
      if (aspect_mode != VID_ASPECT_MODE_PANSCAN)
      {
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_HD, VID_ASPECT_MODE_PANSCAN);
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_SD, VID_ASPECT_MODE_PANSCAN);
      }
      break;
    case AVC_VIDEO_ASPECT_43_LETTERBOX_1:
      if (aspect_ratio != AR_43)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_43);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_43);
      }
      if (aspect_mode != VID_ASPECT_MODE_LETTERBOX)
      {
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_HD, VID_ASPECT_MODE_LETTERBOX);
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_SD, VID_ASPECT_MODE_LETTERBOX);
      }
      break;
    case AVC_VIDEO_ASPECT_169_1:
      if (aspect_ratio != AR_169)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_169);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_169);
      }
      if (aspect_mode != VID_ASPECT_MODE_ORIG)
      {
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_HD, VID_ASPECT_MODE_ORIG);
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_SD, VID_ASPECT_MODE_ORIG);
      }
      break;
    case AVC_VIDEO_ASPECT_169_PANSCAN_1:
      if (aspect_ratio != AR_169)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_169);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_169);
      }
      if (aspect_mode != VID_ASPECT_MODE_PANSCAN)
      {
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_HD, VID_ASPECT_MODE_PANSCAN);
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_SD, VID_ASPECT_MODE_PANSCAN);
      }
      break;
    case AVC_VIDEO_ASPECT_169_LETTERBOX_1:
      if (aspect_ratio != AR_169)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_169);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_169);
      }
      if (aspect_mode != VID_ASPECT_MODE_LETTERBOX)
      {
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_HD, VID_ASPECT_MODE_LETTERBOX);
        disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
          DISP_CHANNEL_SD, VID_ASPECT_MODE_LETTERBOX);
      }
      break;
    case AVC_VIDEO_ASPECT_AUTO_1:
      vdec_get_info(p_avc_data->p_video_dev, &v_info);

      if (aspect_ratio != v_info.ar)
      {
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, v_info.ar);
        disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, v_info.ar);
      }
      //get chip id
      chip_ver = hal_get_chip_ic_id();
      if(IC_WARRIORS == chip_ver || IC_SONATA == chip_ver || IC_CONCERTO == chip_ver)
      {
        if (aspect_mode != VID_ASPECT_MODE_AUTO)
        {
          disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
            DISP_CHANNEL_HD, VID_ASPECT_MODE_AUTO);
          disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
            DISP_CHANNEL_SD, VID_ASPECT_MODE_AUTO);
        }
      }else{
        if (aspect_mode != VID_ASPECT_MODE_PANSCAN)
        {
          disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
            DISP_CHANNEL_HD, VID_ASPECT_MODE_PANSCAN);
          disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
            DISP_CHANNEL_SD, VID_ASPECT_MODE_PANSCAN);
        }
      }
      break;
    default:
      OS_PRINTF("warning---->>error aspect %d\n", new_aspect);
      disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_HD, AR_169);
      disp_set_aspect_ratio(p_avc_data->p_disp_dev, DISP_CHANNEL_SD, AR_169);
      disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
        DISP_CHANNEL_HD, VID_ASPECT_MODE_PANSCAN);
      disp_vid_set_aspect_mode(p_avc_data->p_disp_dev,
        DISP_CHANNEL_SD, VID_ASPECT_MODE_PANSCAN);
      break;
  }

}

/*!
  set video dac
  */
void avc_cfg_scart_format_1(void *p_data, scart_v_format_t mode)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->p_scart_dev == NULL)
  {
    return;
  }

  dev_io_ctrl(p_avc_data->p_scart_dev, SCART_CMD_FORMAT_SET, (u32)mode);

  switch(mode)
  {
    case SCART_VID_CVBS:
      disp_cvbs_onoff(p_avc_data->p_disp_dev, CVBS_GRP0,  TRUE);
      //disp_component_onoff(p_avc_data->p_disp_dev, COMPONENT_GRP0,TRUE);
      //video_dac_set(p_avc_data->p_video_dev, VDAC_0, VIDEO_DAC_CVBS);
    break;
    case SCART_VID_RGB:
      disp_component_onoff(p_avc_data->p_disp_dev, COMPONENT_GRP0,TRUE);
      disp_component_set_type(p_avc_data->p_disp_dev, COMPONENT_GRP0,COLOR_RGB);
   //   video_dac_set(p_avc_data->p_video_dev, VDAC_1, VIDEO_DAC_R);
    //  video_dac_set(p_avc_data->p_video_dev, VDAC_2, VIDEO_DAC_G);
     // video_dac_set(p_avc_data->p_video_dev, VDAC_3, VIDEO_DAC_B);
    break;
    case SCART_VID_YUV:
     disp_component_onoff(p_avc_data->p_disp_dev, COMPONENT_GRP0,TRUE);
     disp_component_set_type(p_avc_data->p_disp_dev, COMPONENT_GRP0,COLOR_YUV); 
    //TODO
    break;
    default:
      CHECK_FAIL_RET_VOID(0);
  }
}

/*!
  set video dac
  */
void avc_cfg_scart_aspect_1(void *p_data, scart_v_aspect_t mode)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->p_scart_dev == NULL)
  {
    return;
  }

  dev_io_ctrl(p_avc_data->p_scart_dev, SCART_CMD_ASPECT_SET, (u32)mode);
}


/*!
  set video dac
  */
void avc_cfg_scart_select_tv_master_1(void *p_data, scart_terminal_t terminal)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->p_scart_dev == NULL)
  {
    return;
  }

  OS_PRINTF("AVC: TV MASTER SELECT [%s]\n",
    terminal == SCART_TERM_STB ? "STB" : "VCR");
  
  dev_io_ctrl(p_avc_data->p_scart_dev, SCART_CMD_TV_MASTER_SEL, (u32)terminal);
}

/*!
  set video dac
  */
void avc_cfg_scart_vcr_input_1(void *p_data, scart_terminal_t terminal)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->p_scart_dev == NULL)
  {
    return;
  }

  OS_PRINTF("AVC: VCR REC SELECT [%s]\n",
    terminal == SCART_TERM_TV ? "TV" : "STB/VCR");
  dev_io_ctrl(p_avc_data->p_scart_dev, SCART_CMD_VCR_REC_SEL, (u32)terminal);
}


BOOL avc_detect_scart_vcr_1(void *p_data)
{
  BOOL is_detected = FALSE;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);

  if(p_avc_data->p_scart_dev != NULL)
  {
    dev_io_ctrl(p_avc_data->p_scart_dev, 
      SCART_CMD_VCR_DETECT, 
      (u32)(&is_detected));
  }
  return is_detected;
}

BOOL avc_set_rf_system_1(void *p_data, rf_sys_t sys)
{
  RET_CODE ret = SUCCESS;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);

  if(p_avc_data->p_rf_dev == NULL)
  {
    return FALSE;
  }
  
  ret = dev_io_ctrl(p_avc_data->p_rf_dev, RF_CMD_SET_SYSTEM, sys);  
  return ret == SUCCESS ? TRUE : FALSE;
}


BOOL avc_set_rf_channel_1(void *p_data, u16 channel)
{
  RET_CODE ret = SUCCESS;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);

  if(p_avc_data->p_rf_dev == NULL)
  {
    return FALSE;
  }
  
  ret = dev_io_ctrl(p_avc_data->p_rf_dev, RF_CMD_SET_CHANNEL, channel);  
  return ret == SUCCESS ? TRUE : FALSE;
}


BOOL avc_video_freeze_1(void *p_data)
{
  RET_CODE ret = SUCCESS; 
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);
  ret = vdec_freeze(p_avc_data->p_video_dev);
//  ret = dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_PAUSE, 0);  
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_video_resume_1(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_FALSE(p_avc_data != NULL);
  ret = vdec_resume(p_avc_data->p_video_dev);
  //dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_RESUME, 0);
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_av_start(void *p_data, video_param_t *p_video, 
        audio_param_t *p_audio, av_start_type_t type) 
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;

  if(type == AV_AUDIO_START || type == AV_START_ALL)
  {    
    ret = aud_start_vsb(p_avc_data->p_audio_dev, p_audio->type, p_audio->file_type);
    CHECK_FAIL_RET_FALSE(ret == SUCCESS);
  }

  OS_PRINTF("---------->>avc av start %d\n",p_video->format);
  if(type == AV_VIDEO_START || type == AV_START_ALL)
  {
    ret |= vdec_set_data_input(p_avc_data->p_video_dev, p_video->file_play);
    CHECK_FAIL_RET_FALSE(ret == SUCCESS);
    ret |=  vdec_start(p_avc_data->p_video_dev, p_video->format, p_video->mode);
    CHECK_FAIL_RET_FALSE(ret == SUCCESS);

  }
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_av_stop(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  OS_PRINTF("--____________________--------->>avc av stop\n");
  ret =  vdec_stop(p_avc_data->p_video_dev);
  ret |= aud_stop_vsb(p_avc_data->p_audio_dev);
  return ret == SUCCESS ? TRUE : FALSE;

}

BOOL avc_av_freeze(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  OS_PRINTF("---------->>avc_av_freeze\n");
  ret =  vdec_freeze(p_avc_data->p_video_dev);
  //ret |= aud_pause_vsb(p_avc_data->p_audio_dev);
  ret |= aud_mute_onoff_vsb(p_avc_data->p_audio_dev, TRUE);
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_av_resume(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  OS_PRINTF("---------->>avc_av_resume\n");
  ret =  vdec_resume(p_avc_data->p_video_dev);
  //ret |= aud_resume_vsb(p_avc_data->p_audio_dev);
  ret |= aud_mute_onoff_vsb(p_avc_data->p_audio_dev, FALSE);
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_audio_pause(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;

  ret = aud_pause_vsb(p_avc_data->p_audio_dev);
  //ret |= aud_mute_onoff_vsb(p_avc_data->p_audio_dev, TRUE);
  return ret == SUCCESS ? TRUE : FALSE;
}

BOOL avc_audio_resume(void *p_data)
{
  RET_CODE ret = SUCCESS;
  avc_var_t *p_avc_data = (avc_var_t *)p_data;

  ret = aud_resume_vsb(p_avc_data->p_audio_dev);
  //ret |= aud_mute_onoff_vsb(p_avc_data->p_audio_dev, FALSE);
  return ret == SUCCESS ? TRUE : FALSE;
}

#if 0
void avc_enter_compress_preview_1(void *p_data, rect_t *p_rect, 
u32 buf_addr)
{

  //dmx_device_t *p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    //SYS_DEV_TYPE_PTI);
  //todo:AUD_VSB
  /*aud_device_vsb_t *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_AUDIO);*/
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  //Disable video ISR to avoid garbage by quick channel switch. for bug##2042
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_ISR_EN, FALSE);
  
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_VIDEO_DISPLAY, FALSE);
  //To avoid garbage on full screen (Cause by stop).
  mtos_task_delay_ms(30);
  //dev_io_ctrl(pti_dev, PTI_CMD_PID_SLOT_EN, FALSE);
  
  //pti_dev_av_enable(p_dmx_dev, FALSE);
  
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_STOP, FALSE);
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_COMPRESSION_SET, TRUE);
  video_frame_buffer_set(p_avc_data->p_video_dev, buf_addr, 
    VIDEO_FRAME_SIZE_CMP_KB * 4, 4, TRUE, FALSE);
    
  video_tag_buffer_set(p_avc_data->p_video_dev, buf_addr, 
    VIDEO_TAG_SIZE * 4, 4);
    
  mtos_task_delay_ms(50);
  //pti_dev_av_reset(p_pti_dev);
  //todo:AUD_VSB
  //dev_io_ctrl(p_audio_dev, AUDIO_CMD_AUDIO_RESET, 0);
  //pti_dev_av_enable(p_pti_dev, TRUE);

  //Open video ISR here.
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_START_WAIT_FRAME, 0);
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_PLAY, 0);
  
  avc_enter_preview_1(p_data, p_rect->left, p_rect->top, p_rect->right, 
  p_rect->bottom);

}

void avc_leave_compress_preview_1(void *p_data, BOOL b_replay)
{
  //dmx_device_t *p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    //SYS_DEV_TYPE_PTI);
  //todo:AUD_VSB
  /*aud_device_vsb_t *p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_AUDIO);*/
  //RET_CODE video_ret = 0;

  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  //Disable video ISR to avoid garbage by quick channel switch. for bug##2042
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_ISR_EN, FALSE);
  
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_VIDEO_DISPLAY, FALSE);
  //To avoid garbage on preview screen (Cause by stop).
  mtos_task_delay_ms(30); 
  //pti_dev_av_enable(p_dmx_dev, FALSE);
  
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_STOP, FALSE);
  dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_COMPRESSION_SET, FALSE);

  // Todo: fix me
  /*video_ret = video_frame_buffer_set(p_avc_data->p_video_dev, buf_addr, 
    VIDEO_FRAME_SIZE * 4, 4, TRUE, FALSE);*/
  //CHECK_FAIL_RET_VOID(video_ret == SUCCESS);
  
  //pti_dev_av_reset(p_pti_dev);
  //todo:AUD_VSB
  //dev_io_ctrl(p_audio_dev, AUDIO_CMD_AUDIO_RESET, 0);
  //pti_dev_av_enable(p_pti_dev, TRUE);
  if(b_replay == TRUE)
  {
    //Open video ISR here.
    dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_START_WAIT_FRAME, 0);
    dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_PLAY, 0);
  }
  
  avc_leave_preview_1(p_data);

}
#endif

void avc_switch_video_mode_jazz(void *p_data, avc_video_mode_1_t new_std)
{
  disp_sys_t video_mode;
 // video_standard_t stream_mode = VIDEO_STANDARD_PAL;
  //RET_CODE ret = SUCCESS;
  vdec_info_t v_info;
 aspect_ratio_t ratio;

  BOOL b_switch = FALSE;
  
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->disp_chan != DISP_CHANNEL_SD)
  {
    //TODO:  only support SD switch now!!!
    return;
  }
  disp_get_tv_sys(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &video_mode);
  switch(new_std)
  {
    case AVC_VIDEO_MODE_NTSC_1:
      if(video_mode != VID_SYS_NTSC_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_NTSC_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_1:
      if(video_mode != VID_SYS_PAL)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL;
      }
      break;
    case AVC_VIDEO_MODE_PAL_M_1:
      if(video_mode != VID_SYS_PAL_M)
      {
        b_switch = TRUE;
        video_mode = VID_SYS_PAL_M;
      }
      break;
    case AVC_VIDEO_MODE_PAL_N_1:
      if(video_mode != VID_SYS_PAL_N)
      {
        video_mode = VID_SYS_PAL_N;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_480P:
      if(video_mode != VID_SYS_480P)
      {
        video_mode = VID_SYS_480P;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_576P:
      if(video_mode != VID_SYS_576P_50HZ)
      {
        video_mode = VID_SYS_576P_50HZ;
        b_switch = TRUE;
      }
      break;
    case AVC_VIDEO_MODE_AUTO_1:
      vdec_get_info(p_avc_data->p_video_dev, &v_info);
 //     ret = dev_io_ctrl(p_avc_data->p_video_dev, VIDEO_CMD_GET_STREAM_STANDARD, 
    //    (u32)&stream_mode);
      if(v_info.vid_format == VID_SYS_MAX)
      {
        break;
      }
      
      if((video_mode != v_info.vid_format))
      {
        b_switch = TRUE;
        video_mode = v_info.vid_format;
      }
      break;
    default:
      CHECK_FAIL_RET_VOID(0);
      break;
  }

  if(b_switch == TRUE)
  {
#if 0
    BOOL is_osd_display = TRUE;
    BOOL is_video_layer_open = TRUE;

    if(VID_SYS_NTSC_M == video_mode)
    {        
        subt_set_video_std_vsb(VID_STD_NTSC);
    }
    else
    {        
        subt_set_video_std_vsb(VID_STD_PAL);
    }     


    is_video_layer_open = disp_layer_is_show(p_avc_data->p_disp_dev,DISP_LAYER_ID_VIDEO_SD);
    is_osd_display = disp_layer_is_show(p_avc_data->p_disp_dev, DISP_LAYER_ID_OSD0);
    
    disp_layer_show(p_avc_data->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, is_video_layer_open);
    disp_layer_show(p_avc_data->p_disp_dev, DISP_LAYER_ID_OSD0, is_osd_display);
    #endif
    //OS_PRINTF("set video std %d\n", video_mode);
    disp_set_tv_sys(p_avc_data->p_disp_dev, p_avc_data->disp_chan, video_mode);
    
  ///todo: confirm aspect ratio
    disp_get_aspect_ratio(p_avc_data->p_disp_dev, p_avc_data->disp_chan, &ratio);
    disp_set_aspect_ratio(p_avc_data->p_disp_dev, p_avc_data->disp_chan, ratio);
#if 0
        if((VID_SYS_NTSC_M == video_mode)
      ||(VID_SYS_PAL_M == video_mode)
      ||(VID_SYS_NTSC_J == video_mode)
      ||(VID_SYS_NTSC_443 == video_mode))
    {
      avc_reset_screen_pos_1(p_data, TRUE);
      //OS_PRINTF("PAL -> NTSC\n");
    }
    else
    {
      avc_reset_screen_pos_1(p_data, FALSE);
      //OS_PRINTF("NTSC -> PAL\n");
    }
    
    if(is_video_layer_open == TRUE)
    {
       disp_layer_show(p_avc_data->p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
    }
    if(is_osd_display == TRUE)
    {
      disp_layer_show(p_avc_data->p_disp_dev, DISP_LAYER_ID_OSD0, TRUE);
    }
#endif
  }
}

void avc_set_video_bright(void *p_data, u8 percent)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->disp_chan != DISP_CHANNEL_SD)
  {
    //TODO:  only support SD switch now!!!
    return;
  }
disp_set_bright(p_avc_data->p_disp_dev, p_avc_data->disp_chan,percent);
}

void avc_set_video_contrast(void *p_data, u8 percent)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->disp_chan != DISP_CHANNEL_SD)
  {
    //TODO:  only support SD switch now!!!
    return;
  }
disp_set_contrast(p_avc_data->p_disp_dev, p_avc_data->disp_chan,percent);
}

void avc_set_video_sature(void *p_data, u8 percent)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  if(p_avc_data->disp_chan != DISP_CHANNEL_SD)
  {
    //TODO:  only support SD switch now!!!
    return;
  }
disp_set_sature(p_avc_data->p_disp_dev, p_avc_data->disp_chan,percent);
}

void avc_cfg_scart_format_jazz(void *p_data, scart_v_format_t mode)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
#if 0
  if(p_avc_data->p_scart_dev == NULL)
  {
    return;
  }
  dev_io_ctrl(p_avc_data->p_scart_dev, SCART_CMD_FORMAT_SET, (u32)mode);
#endif
  switch(mode)
  {
    case SCART_VID_CVBS:
      //disp_cvbs_onoff(p_avc_data->p_disp_dev, CVBS_GRP0,  TRUE);
      //video_dac_set(p_avc_data->p_video_dev, VDAC_0, VIDEO_DAC_CVBS);
      disp_cvbs_onoff(p_avc_data->p_disp_dev,CVBS_GRP0,TRUE);
    // disp_jazz_cvbs_onoff(p_avc_data->p_disp_dev,CVBS_GRP1,TRUE);
     //disp_jazz_cvbs_onoff(p_avc_data->p_disp_dev,CVBS_GRP2,TRUE);  
    break;
    //case SCART_VID_SVDIEO:
   // disp_component_set_type(p_avc_data->p_disp_dev,COMPONENT_GRP0,COLOR_SVID_CVBS);
    //disp_component_onoff(p_avc_data->p_disp_dev,COMPONENT_GRP0,TRUE);
   // break;
    case SCART_VID_RGB:
      //disp_component_onoff(p_avc_data->p_disp_dev, COMPONENT_GRP0,TRUE);
      //disp_component_set_type(p_avc_data->p_disp_dev, COMPONENT_GRP0,COLOR_RGB);
      disp_component_set_type(p_avc_data->p_disp_dev,COMPONENT_GRP0,COLOR_CVBS_RGB);
      disp_component_onoff(p_avc_data->p_disp_dev,COMPONENT_GRP0,TRUE);
     break;
    case SCART_VID_YUV:
     //disp_component_onoff(p_avc_data->p_disp_dev, COMPONENT_GRP0,TRUE);
     //disp_component_set_type(p_avc_data->p_disp_dev, COMPONENT_GRP0,COLOR_YUV); 
    //TODO
       disp_component_set_type(p_avc_data->p_disp_dev,COMPONENT_GRP0,COLOR_CVBS_YUV);
       disp_component_onoff(p_avc_data->p_disp_dev,COMPONENT_GRP0,TRUE);
       break;
    default:
      CHECK_FAIL_RET_VOID(0);
  }
}

/***********jazz dac config**********************
DAC      mode 0             mode1                       mode3
DAC 0    CVBS                CVBS                         S-Y
DAC 1    S-Y                   Y                               CVBS
DAC 2    S-Chrom           Cb                             CVBS
DAC 3    null                    Cr                              S-Chrom
**********************************************/
void avc_cfg_video_out_format_jazz(void *p_data, video_out_format_t mode)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;
  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  switch(mode)
  {
    case VIDEO_OUT_VID_CVBS:
    disp_component_set_type (p_avc_data->p_disp_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
    disp_component_onoff (p_avc_data->p_disp_dev, COMPONENT_GRP0, FALSE);
    disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0,TRUE);
     break;
    case VIDEO_OUT_VID_SVDIEO:
    disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0, FALSE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP1, TRUE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP2, TRUE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP3, FALSE);
   break;
  case VIDEO_OUT_VID_RGB:
    disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0,FALSE);
    disp_component_set_type(p_avc_data->p_disp_dev,COMPONENT_GRP0,COLOR_CVBS_RGB);
    disp_component_onoff(p_avc_data->p_disp_dev,COMPONENT_GRP0,TRUE);
   break;
  case VIDEO_OUT_VID_YUV:
    disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0,FALSE);
    disp_component_set_type (p_avc_data->p_disp_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
    disp_component_onoff (p_avc_data->p_disp_dev, COMPONENT_GRP0, TRUE);     
  break;
  case VIDEO_OUT_VID_CVBS_YUV:
    disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0,TRUE);
    disp_component_set_type (p_avc_data->p_disp_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
    disp_component_onoff (p_avc_data->p_disp_dev, COMPONENT_GRP0, TRUE);
  break;
  case VIDEO_OUT_VID_CVBS_SVDIEO:
    disp_component_set_type (p_avc_data->p_disp_dev, COMPONENT_GRP0, COLOR_SVID_CVBS);
    disp_component_onoff (p_avc_data->p_disp_dev, COMPONENT_GRP0, TRUE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP1, TRUE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP2, TRUE);
    disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP3, FALSE);
  break;
  case VIDEO_OUT_VID_NULL:
  disp_component_set_type (p_avc_data->p_disp_dev, COMPONENT_GRP0, COLOR_CVBS_YUV);
  disp_component_onoff (p_avc_data->p_disp_dev, COMPONENT_GRP0, FALSE);
  disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP0,FALSE); 
  break;
  case VIDEO_OUT_VID_CVBS_CVBS:
  disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP2,TRUE);
  disp_cvbs_onoff (p_avc_data->p_disp_dev, CVBS_GRP1, TRUE);
  disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP0,FALSE);
  disp_svideo_onoff (p_avc_data->p_disp_dev, SVIDEO_GRP3,FALSE);
  break;
    default: 
      CHECK_FAIL_RET_VOID(0);
  }
}

void avc_video_switch_chann(void *p_data, disp_channel_t disp_chann)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;

  CHECK_FAIL_RET_VOID(p_avc_data != NULL);

  p_avc_data->disp_chan = disp_chann;
}

void avc_update_region(void *p_data, void *p_region, rect_t *p_rect)
{
  avc_var_t *p_avc_data = (avc_var_t *)p_data;

  CHECK_FAIL_RET_VOID(p_avc_data != NULL);
  CHECK_FAIL_RET_VOID(p_region != NULL);
  
  disp_layer_update_region(p_avc_data->p_disp_dev, p_region, p_rect);    
}

