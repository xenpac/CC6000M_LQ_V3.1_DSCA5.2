/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_zoom.h"
enum control_id
{
  IDC_TEXT = 1,
  IDC_ZOOM,
  IDC_ARROW_L,
  IDC_ARROW_T,
  IDC_ARROW_R,
  IDC_ARROW_B,
};

enum local_msg
{
  MSG_ZOOM = MSG_LOCAL_BEGIN + 1100,
};

static u8 g_zoom_curn;
static u8 g_zoom_x[] =
{1, 2, 4, 6, 8, 12, 16};
static double g_zoom_scale[] =
{
  1.0,
  1.4142135623730951,
  2.0,
  2.4494897427831781,
  2.8284271247461901,
  3.4641016151377546,
  4.0
};
#define ZOOM_CNT    (sizeof(g_zoom_x) / sizeof(u8))
#define ZOOM_MOV_STEP 2

static point_t g_zoom_center;

static rect_t g_tvsrc_fullrc = {0};

static void *g_display_dev;

static RET_CODE close_zoom(u32 para1, u32 para2);

u16 zoom_cont_keymap(u16 key);
RET_CODE zoom_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

void get_scaled_rect(rect_t *rc_tvsrc, rect_t *rc_icon)
{
  double scale;
  s16 half_width, half_height;
  s16 full_width, full_height;
  s16 offset;
  s16 width, height;

  scale = g_zoom_scale[g_zoom_curn];

  full_width = (s16)(ZOOM_BOUND_W / scale);
  full_height = (s16)(ZOOM_BOUND_H / scale);
  half_width = full_width / 2;
  half_height = full_height / 2;

  if(g_zoom_center.x < half_width)
  {
    g_zoom_center.x = half_width;
  }
  if(g_zoom_center.x > ZOOM_BOUND_W - half_width)
  {
    g_zoom_center.x = ZOOM_BOUND_W - half_width;
  }
  if(g_zoom_center.y < half_height)
  {
    g_zoom_center.y = half_height;
  }
  if(g_zoom_center.y > ZOOM_BOUND_H - half_height)
  {
    g_zoom_center.y = ZOOM_BOUND_H - half_height;
  }

  rc_icon->left = ZOOM_BOUND_X + (g_zoom_center.x - half_width);
  rc_icon->top = ZOOM_BOUND_Y + (g_zoom_center.y - half_height);
  rc_icon->right = rc_icon->left + full_width;
  rc_icon->bottom = rc_icon->top + full_height;

  offset = (ZOOM_BOUND_X + ZOOM_BOUND_W) - rc_icon->right;
  if(offset < 0)
  {
    offset_rect(rc_icon, offset, 0);
  }
  offset = (ZOOM_BOUND_Y + ZOOM_BOUND_H) - rc_icon->bottom;
  if(offset < 0)
  {
    offset_rect(rc_icon, 0, offset);
  }
  width = g_tvsrc_fullrc.right - g_tvsrc_fullrc.left + 1;
  height = g_tvsrc_fullrc.bottom - g_tvsrc_fullrc.top + 1;
  rc_tvsrc->left = g_tvsrc_fullrc.left + (s16)((rc_icon->left - ZOOM_BOUND_X) * width / ZOOM_BOUND_W);
  rc_tvsrc->top = g_tvsrc_fullrc.top + (s16)((rc_icon->top - ZOOM_BOUND_Y) * height / ZOOM_BOUND_H);
  rc_tvsrc->right = (s16)((full_width * width / ZOOM_BOUND_W)) + rc_tvsrc->left - 1;
  rc_tvsrc->bottom =(s16)((full_height * height / ZOOM_BOUND_H)) + rc_tvsrc->top - 1;
}


static void enable_icon(control_t *icon, BOOL is_enable)
{
  if(is_enable)
  {
    ctrl_set_attr(icon, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(icon, OBJ_STS_SHOW);
    ctrl_paint_ctrl(icon, TRUE);
  }
  else
  {
    ctrl_set_sts(icon, OBJ_STS_HIDE);
    ctrl_erase_ctrl(icon);
  }
}


static void fill_zoom_info(control_t *cont)
{
  control_t *ctrl;
  rect_t rc_icon;
  rect_t rc_tvsrc;

  u8 buf[10] = {0};

  get_scaled_rect(&rc_tvsrc, &rc_icon);

  ctrl = ctrl_get_child_by_id(cont, IDC_TEXT);
  sprintf((char *)buf, "ZOOM %dX", g_zoom_x[g_zoom_curn]);
  text_set_content_by_ascstr(ctrl, buf);
  ctrl_paint_ctrl(ctrl, TRUE);

  ctrl = ctrl_get_child_by_id(cont, IDC_ARROW_L);
  enable_icon(ctrl, (BOOL)(rc_icon.left > ZOOM_BOUND_X));

  ctrl = ctrl_get_child_by_id(cont, IDC_ARROW_T);
  enable_icon(ctrl, (BOOL)(rc_icon.top > ZOOM_BOUND_Y));

  ctrl = ctrl_get_child_by_id(cont, IDC_ARROW_R);
  enable_icon(ctrl, (BOOL)(rc_icon.right < (ZOOM_BOUND_X + ZOOM_BOUND_W)));

  ctrl = ctrl_get_child_by_id(cont, IDC_ARROW_B);
  enable_icon(ctrl, (BOOL)(rc_icon.bottom <( ZOOM_BOUND_Y + ZOOM_BOUND_H)));

  ctrl = ctrl_get_child_by_id(cont, IDC_ZOOM);
  enable_icon(ctrl, FALSE);
  ctrl_set_frame(ctrl, &rc_icon);
  enable_icon(ctrl, TRUE);

  // do zoom
  OS_PRINTF("zoom %dX \n tv src rect (%d, %d, %d, %d)\n", g_zoom_x[g_zoom_curn],
            rc_tvsrc.left, rc_tvsrc.top, rc_tvsrc.right, rc_tvsrc.bottom);

  //disp_vid_zoom_in(g_display_dev, DISP_CHANNEL_SD, &rc_tvsrc);
  disp_vid_zoom_out(g_display_dev, DISP_CHANNEL_HD, &rc_tvsrc);
}


static RET_CODE on_zoom_focus_up(control_t *p_cont, u16 msg, u32 para1,
                                 u32 para2)
{
  g_zoom_center.y -= ZOOM_MOV_STEP;

  fill_zoom_info(p_cont);

  return SUCCESS;
}


static RET_CODE on_zoom_focus_down(control_t *p_cont, u16 msg, u32 para1,
                                   u32 para2)
{
  g_zoom_center.y += ZOOM_MOV_STEP;

  fill_zoom_info(p_cont);

  return SUCCESS;
}


static RET_CODE on_zoom_focus_left(control_t *p_cont, u16 msg, u32 para1,
                                   u32 para2)
{
  g_zoom_center.x -= ZOOM_MOV_STEP;

  fill_zoom_info(p_cont);

  return SUCCESS;
}


static RET_CODE on_zoom_focus_right(control_t *p_cont, u16 msg, u32 para1,
                                    u32 para2)
{
  g_zoom_center.x += ZOOM_MOV_STEP;

  fill_zoom_info(p_cont);

  return SUCCESS;
}


static RET_CODE on_zoom_exit(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  //disp_vid_zoom_exit(g_display_dev, DISP_CHANNEL_SD);
  disp_vid_zoom_exit(g_display_dev, DISP_CHANNEL_HD); //fix bug 16241
  return close_zoom(0, 0);
}


static RET_CODE on_zoom(control_t *p_cont, u16 msg, u32 para1,
                        u32 para2)
{
  void *p_vdec_dev = NULL;
  vdec_info_t v_info = {VDEC_ERROR_NONE,};

  p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_vdec_dev);

  vdec_get_info(p_vdec_dev, &v_info);

  if(VDEC_ERROR_THIRSTY == v_info.err)  //fix bug 16956
  	{
	  	OS_PRINTF("Decoder exist error ,can't zoom now!\n");
	  	return VDEC_ERROR_NONE;
  	}
    
  g_zoom_curn = (g_zoom_curn + 1) % ZOOM_CNT;
  fill_zoom_info(p_cont);

  return SUCCESS;
}


static RET_CODE on_zoom_save(control_t *p_cont, u16 msg, u32 para1,
                             u32 para2)
{
  disp_vid_zoom_exit(g_display_dev, DISP_CHANNEL_SD);

  return SUCCESS;
}


RET_CODE open_zoom(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl;
  disp_sys_t tv_format;
  void *p_vdec_dev = NULL;
  vdec_info_t v_info = {VDEC_ERROR_NONE,};

  p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_vdec_dev);
  vdec_get_info(p_vdec_dev, &v_info);

  if(CURN_MODE_TV != sys_status_get_curn_prog_mode())  //fix bug 17008
  {
  	OS_PRINTF("Only support this function in TV mode!\n");
  	return ERR_FAILURE;
  }

  if(VDEC_ERROR_THIRSTY == v_info.err)  //fix bug 16956
  	{
	  	OS_PRINTF("Decoder exist error ,can't zoom now!\n");
	  	return VDEC_ERROR_NONE;
  	}
  
  g_display_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                      SYS_DEV_TYPE_DISPLAY);
#if 0
  video_frame_size_query(g_video_dev, &tvsrc_w, &tvsrc_h);

  if(tvsrc_w == 0
    || tvsrc_h == 0)
  {
    return ERR_FAILURE;
  }

  g_tvsrc_fullrc.width = (u16)tvsrc_w;
  g_tvsrc_fullrc.height = (u16)tvsrc_h;
#endif
avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
tv_format = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));

 if((VID_SYS_NTSC_M == tv_format) ||
     (VID_SYS_NTSC_J == tv_format) ||
     (VID_SYS_NTSC_443 == tv_format) ||
      //hd
     (VID_SYS_480P == tv_format))
  {
    g_tvsrc_fullrc.right = 720;
    g_tvsrc_fullrc.bottom = 480;
  }
  else if((VID_SYS_PAL == tv_format) ||
          (VID_SYS_PAL_N == tv_format) ||
          (VID_SYS_PAL_NC == tv_format) ||
          (VID_SYS_PAL_M == tv_format) ||
          //hd
          (VID_SYS_576P_50HZ == tv_format))
  {
    g_tvsrc_fullrc.right = 720;
    g_tvsrc_fullrc.bottom = 576;
  }
  else if((tv_format >= VID_SYS_1080I) &&
          (tv_format <= VID_SYS_1080P_50HZ))
  {
    g_tvsrc_fullrc.right = 1920;
    g_tvsrc_fullrc.bottom = 1080;
  }
  else if((tv_format >= VID_SYS_720P) &&
          (tv_format <= VID_SYS_720P_50HZ))
  {
   g_tvsrc_fullrc.right = 1280;
   g_tvsrc_fullrc.bottom = 720;
  }
  else
  {
    MT_ASSERT(0);
  }

#if 1//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_ZOOM,
                             ZOOM_CONT_X, ZOOM_CONT_Y,
                             ZOOM_CONT_W, ZOOM_CONT_H,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_ZOOM_FRM, RSI_ZOOM_FRM, RSI_ZOOM_FRM);
  ctrl_set_keymap(p_cont, zoom_cont_keymap);
  ctrl_set_proc(p_cont, zoom_cont_proc);
  ctrl_set_style(p_cont, STL_EX_WHOLE_HL);

  // text
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TEXT,
                            ZOOM_STXT_X, ZOOM_STXT_Y,
                            ZOOM_STXT_W, ZOOM_STXT_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ZOOM_TXT, RSI_ZOOM_TXT, RSI_ZOOM_TXT);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_font_style(p_ctrl, FSI_ZOOM_TXT,
                      FSI_ZOOM_TXT, FSI_ZOOM_TXT);
  text_set_content_by_ascstr(p_ctrl, (u8 *)"ZOOM 1X");

  p_ctrl = ctrl_create_ctrl(CTRL_CONT, 0,
                            ZOOM_BOUND_X, ZOOM_BOUND_Y,
                            ZOOM_BOUND_W, ZOOM_BOUND_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ZOOM_BOUND,
                  RSI_ZOOM_BOUND, RSI_ZOOM_BOUND);

  // icon
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ZOOM,
                            ZOOM_ICON_X, ZOOM_ICON_Y,
                            ZOOM_ICON_W, ZOOM_ICON_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_ZOOM_BOUND,
                  RSI_ZOOM_BOUND, RSI_ZOOM_BOUND);
  bmap_set_content_by_id(p_ctrl, IM_ZOOM_IN);

  // arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ARROW_T,
                            ZOOM_ARROW_T_X, ZOOM_ARROW_T_Y,
                            ZOOM_ARROW_T_W, ZOOM_ARROW_T_H,
                            p_cont, 0);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_ctrl, IM_ZOOM_UP);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ARROW_R,
                            ZOOM_ARROW_R_X, ZOOM_ARROW_R_Y,
                            ZOOM_ARROW_R_W, ZOOM_ARROW_R_H,
                            p_cont, 0);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_ctrl, IM_ZOOM_RIGHT);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ARROW_B,
                            ZOOM_ARROW_B_X, ZOOM_ARROW_B_Y,
                            ZOOM_ARROW_B_W, ZOOM_ARROW_B_H,
                            p_cont, 0);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_ctrl, IM_ZOOM_DOWN);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ARROW_L,
                            ZOOM_ARROW_L_X, ZOOM_ARROW_L_Y,
                            ZOOM_ARROW_L_W, ZOOM_ARROW_L_H,
                            p_cont, 0);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  bmap_set_content_by_id(p_ctrl, IM_ZOOM_LEFT);

#else
#endif

  g_zoom_curn = 0;

  g_zoom_center.x = ZOOM_BOUND_W / 2;
  g_zoom_center.y = ZOOM_BOUND_H / 2;

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE close_zoom(u32 para1, u32 para2)
{
  return manage_close_menu(ROOT_ID_ZOOM, 0, 0);
}


BEGIN_KEYMAP(zoom_cont_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_ZOOM, MSG_ZOOM)
END_KEYMAP(zoom_cont_keymap, NULL)

BEGIN_MSGPROC(zoom_cont_proc, cont_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_zoom_focus_up)
ON_COMMAND(MSG_FOCUS_DOWN, on_zoom_focus_down)
ON_COMMAND(MSG_FOCUS_LEFT, on_zoom_focus_left)
ON_COMMAND(MSG_FOCUS_RIGHT, on_zoom_focus_right)
ON_COMMAND(MSG_EXIT, on_zoom_exit)
ON_COMMAND(MSG_ZOOM, on_zoom)
ON_COMMAND(MSG_SAVE, on_zoom_save)
END_MSGPROC(zoom_cont_proc, cont_class_proc)
