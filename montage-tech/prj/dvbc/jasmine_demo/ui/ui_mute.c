/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 
 ****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_volume.h"
#include "ui_volume_usb.h"
#include "ui_nvod_volume.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

static BOOL g_is_mute = FALSE;

BOOL ui_is_mute(void)
{
  return g_is_mute;
}


void ui_set_mute(BOOL is_mute)
{
  u8 focus;
  g_is_mute = is_mute;

  if(g_is_mute)
  {
    if(fw_find_root_by_id(ROOT_ID_VOLUME) != NULL)
    {
      close_volume();
    }
    
    if(fw_find_root_by_id(ROOT_ID_VOLUME_USB) != NULL)
    {
      close_volume_usb();
    }
  #ifdef NVOD_ENABLE
    if(fw_find_root_by_id(ROOT_ID_NVOD_VOLUME) != NULL)
    {
      close_nvod_volume();
    }
  #endif

    focus = fw_get_focus_id();
    if(ui_is_fullscreen_menu(focus)
        || ui_is_preview_menu(focus)
#ifdef ENABLE_MUSIC_PIC_CONFIG
        || ( focus == ROOT_ID_USB_MUSIC )
        || ( focus == ROOT_ID_USB_PICTURE )
#endif
#ifdef ENABLE_PVR_REC_CONFIG
    	 || ( focus == ROOT_ID_TIMESHIFT_BAR )
    	 || ( focus == ROOT_ID_RECORD_MANAGER )
#endif
#ifdef ENABLE_FILE_PLAY  				
    	 || ( focus == ROOT_ID_USB_FILEPLAY )
    	 || ( focus == ROOT_ID_FILEPLAY_BAR )
#endif
    	 || ( focus == ROOT_ID_PVR_PLAY_BAR )
        || ( focus == ROOT_ID_NVOD_VIDEO )
        || ( focus == ROOT_ID_SMALL_LIST )
        || ( focus == ROOT_ID_FAV_EDIT )
        || ( focus == ROOT_ID_CATEGORY )
        || ( focus == ROOT_ID_PROG_LIST )
        || ( focus == ROOT_ID_MAINMENU )
        || ( focus == ROOT_ID_CHANNEL_EDIT )
        || ( focus == ROOT_ID_PRO_INFO )
        || ( focus == ROOT_ID_PARENTAL_LOCK )
        || ( focus == ROOT_ID_FREQ_SET )
        || ( focus == ROOT_ID_EPG )
        || ( focus == ROOT_ID_NVOD )
        || ( focus == ROOT_ID_BOOK_LIST )
        || ( focus == ROOT_ID_FAV_LIST )
        || ( focus == ROOT_ID_TIME_SET )
        || ( focus == ROOT_ID_INFO )
        || ( focus == ROOT_ID_EPG_DETAIL )
        || ( focus == ROOT_ID_JUMP )
        || ( focus == ROOT_ID_HDD_INFO )
        || ( focus == ROOT_ID_STORAGE_FORMAT )
        || ( focus == ROOT_ID_DVR_CONFIG )
        || ( focus == ROOT_ID_GAME )
        || ( focus == ROOT_ID_USB_MUSIC_FULLSCREEN )
#ifdef ENABLE_CA 
        || ( focus == ROOT_ID_CONDITIONAL_ACCEPT )
#endif
      )
    {
      open_mute(0, 0);
    }
  }
  else
  {
    close_mute();
  }
  avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID), g_is_mute, TRUE);
}


RET_CODE open_mute(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  u16 cont_x, cont_y, cont_w, cont_h;
  u16 icon_x, icon_y, icon_w, icon_h;
  u16 icon_id;
  u16 left, top, width, height;
  u8 focus = fw_get_focus_id();
  BOOL is_fullscr = TRUE;
  BOOL is_music = FALSE;
  BOOL is_video_fullscr = FALSE;
  BOOL is_mutemenu = FALSE;
  // check for close
  if(fw_find_root_by_id(ROOT_ID_MUTE) != NULL)
  {
    close_mute();
  }
#ifdef ENABLE_PVR_REC_CONFIG
  if(ROOT_ID_USB_MUSIC == focus || ROOT_ID_USB_PICTURE== focus)
  {
    is_music = TRUE;
  }
#endif
#ifdef ENABLE_FILE_PLAY  
  if(ROOT_ID_FILEPLAY_BAR== focus)
  {
    is_video_fullscr = TRUE;
  }
#endif
 
  if(manage_get_preview_rect(fw_get_focus_id(),
                             &left, &top,
                             &width, &height))
  {
    is_fullscr = FALSE;
  }

  if( ( focus == ROOT_ID_NVOD_VIDEO )
    || ( focus == ROOT_ID_SMALL_LIST )
    || ( focus == ROOT_ID_FAV_EDIT )
    || ( focus == ROOT_ID_CATEGORY )
    || ( focus == ROOT_ID_PROG_LIST )
    || ( focus == ROOT_ID_MAINMENU )
    || ( focus == ROOT_ID_CHANNEL_EDIT )
    || ( focus == ROOT_ID_PRO_INFO )
    || ( focus == ROOT_ID_PARENTAL_LOCK )
    || ( focus == ROOT_ID_FREQ_SET )
    || ( focus == ROOT_ID_EPG )
    || ( focus == ROOT_ID_NVOD )
    || ( focus == ROOT_ID_BOOK_LIST )
    || ( focus == ROOT_ID_FAV_LIST )
    || ( focus == ROOT_ID_TIME_SET )
    || ( focus == ROOT_ID_INFO )
    || ( focus == ROOT_ID_EPG_DETAIL )
    || ( focus == ROOT_ID_JUMP )
    || ( focus == ROOT_ID_HDD_INFO )
    || ( focus == ROOT_ID_STORAGE_FORMAT )
    || ( focus == ROOT_ID_DVR_CONFIG )
    || ( focus == ROOT_ID_GAME )
    || ( focus == ROOT_ID_RECORD_MANAGER )
    || ( focus == ROOT_ID_USB_FILEPLAY )
    || ( focus == ROOT_ID_USB_MUSIC_FULLSCREEN )
#ifdef ENABLE_CA
    || ( focus == ROOT_ID_CONDITIONAL_ACCEPT )
#endif
    )
  {
      is_mutemenu = TRUE;
  }
  
  if(is_music)
  {
    cont_x = MUTE_CONT_IN_MUSIC_X;
    cont_y = MUTE_CONT_IN_MUSIC_Y;
    cont_w = MUTE_CONT_PREV_W;
    cont_h = MUTE_CONT_PREV_H;
    
    icon_x = MUTE_ICON_PREV_X, icon_y = MUTE_ICON_PREV_Y;
    icon_w = MUTE_ICON_PREV_W, icon_h = MUTE_ICON_PREV_H;
    icon_id = IM_MUTE_S;
  }
  else if( is_mutemenu )
  {
    cont_x = MUTE_CONT_IN_MENU_X;
    cont_y = MUTE_CONT_IN_MENU_Y;
    cont_w = MUTE_CONT_PREV_W;
    cont_h = MUTE_CONT_PREV_H;
    
    icon_x = MUTE_ICON_PREV_X, icon_y = MUTE_ICON_PREV_Y;
    icon_w = MUTE_ICON_PREV_W, icon_h = MUTE_ICON_PREV_H;
    icon_id = IM_MUTE_S;
  }
  else if((is_fullscr)||(is_video_fullscr))  // list mode
  {
    cont_x = MUTE_CONT_FULL_X - MUTE_ICON_FULL_W - 20, cont_y = MUTE_CONT_FULL_Y ;//set icon mute,if move, please change cont_x ,not change cont_y
    cont_w = MUTE_CONT_FULL_W, cont_h = MUTE_CONT_FULL_H;
    icon_x = MUTE_ICON_FULL_X, icon_y = MUTE_ICON_FULL_Y;
    icon_w = MUTE_ICON_FULL_W, icon_h = MUTE_ICON_FULL_H;
    icon_id = IM_MUTE;
  }
  else
  {
    cont_x = left + width - MUTE_CONT_PREV_W - 10;
    cont_y = top + 10;
    cont_w = MUTE_CONT_PREV_W;
    cont_h = MUTE_CONT_PREV_H;
    
    icon_x = MUTE_ICON_PREV_X, icon_y = MUTE_ICON_PREV_Y;
    icon_w = MUTE_ICON_PREV_W, icon_h = MUTE_ICON_PREV_H;
    icon_id = IM_MUTE_S;
  }

  p_cont = fw_create_mainwin(ROOT_ID_MUTE,
                             cont_x, cont_y, cont_w, cont_h,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                            icon_x, icon_y, icon_w, icon_h,
                            p_cont, 0);
  bmap_set_content_by_id(p_icon, icon_id);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  #ifdef ENABLE_CA
  #ifndef ONLY1_CA_VER
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
  #endif

  return SUCCESS;
}


void close_mute(void)
{
  //manage_close_menu(ROOT_ID_MUTE, 0, 0);
  fw_destroy_mainwin_by_id(ROOT_ID_MUTE);
}
