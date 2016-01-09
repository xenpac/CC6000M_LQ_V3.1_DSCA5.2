/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_pause.h"
#include "ui_volume.h"
#include "ui_mute.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

static BOOL g_is_pause = FALSE;

BOOL ui_is_pause(void)
{
  return g_is_pause;
}


void ui_set_pause(BOOL is_pause)
{
  BOOL ret = FALSE;
  
  /* do something to set pause */
  UI_PRINTF("AP: set pause = %d\n", is_pause);

  if (is_pause)
    ret = avc_av_freeze(class_get_handle_by_id(AVC_CLASS_ID));
  else
    ret = avc_av_resume(class_get_handle_by_id(AVC_CLASS_ID));

  if (FALSE == ret)
    return;

  if(ui_is_mute())
  {
    avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID), TRUE, TRUE);
    OS_PRINTF("%s(Line: %d): pause set mute\n", __FUNCTION__, __LINE__);
  }

  g_is_pause = is_pause;
    
  if (g_is_pause)
  {
    u8 focus = fw_get_focus_id();
    
    if (ui_is_fullscreen_menu(focus))
    {
      open_pause(0, 0);
    }
    else
    {
      if (focus == ROOT_ID_PROG_LIST 
        || focus == ROOT_ID_EPG
        || focus == ROOT_ID_PVR_REC_BAR)
      {
        open_pause(0, 0);
      }
    }
  }
  else
  {
    close_pause();
  }

}


RET_CODE open_pause(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  u16 cont_x, cont_y, cont_w, cont_h;
  u16 icon_x, icon_y, icon_w, icon_h;
  u16 icon_id;
  u16 left, top, width, height;
  BOOL is_fullscr = TRUE;

  // check for close
  if(fw_find_root_by_id(ROOT_ID_PAUSE) != NULL)
  {
    close_pause();
  }

  if(manage_get_preview_rect(fw_get_focus_id(),
                             &left, &top,
                             &width, &height))
  {
    is_fullscr = FALSE;
  }

  if (is_fullscr) // fullscr mode
  {
    cont_x = PAUSE_CONT_FULL_X, cont_y = PAUSE_CONT_FULL_Y;
    cont_w = PAUSE_CONT_FULL_W, cont_h = PAUSE_CONT_FULL_H;   
    icon_x = PAUSE_ICON_FULL_X, icon_y = PAUSE_ICON_FULL_Y;
    icon_w = PAUSE_ICON_FULL_W, icon_h = PAUSE_ICON_FULL_H;    
    icon_id = IM_PAUSE;
  }
  else
  {
    cont_x = left + width - PAUSE_CONT_PREV_W - 60;
    cont_y = top + 10;
    cont_w = PAUSE_CONT_PREV_W;
    cont_h = PAUSE_CONT_PREV_H;
    
    icon_x = PAUSE_ICON_PREV_X, icon_y = PAUSE_ICON_PREV_Y;
    icon_w = PAUSE_ICON_PREV_W, icon_h = PAUSE_ICON_PREV_H;    
    icon_id = IM_PAUSE_S;
  }

  p_cont = fw_create_mainwin(ROOT_ID_PAUSE,
                                  cont_x, cont_y, cont_w,cont_h,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_INACTIVE, 0);
  if (p_cont == NULL)
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


void close_pause(void)
{
  //manage_close_menu(ROOT_ID_PAUSE, 0, 0);
  fw_destroy_mainwin_by_id(ROOT_ID_PAUSE);
}


