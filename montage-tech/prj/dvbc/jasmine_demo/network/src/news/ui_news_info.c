/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_news_title.h"
#include "ui_news_info.h"
#include "ui_picture_api.h"

#include "news_filter.h"
#include "news_api.h"
#include "ui_news_api.h"

typedef enum
{
  IDC_NEWS_INFO_PREV = 1,
  IDC_NEWS_INFO_BMP,
  IDC_NEWS_INFO_TEXT,
  IDC_NEWS_INFO_TEXT2,
  IDC_NEWS_INFO_SBAR,
  IDC_NEWS_INFO_PREV_T,
  IDC_NEWS_GET_MSG_TP,
}news_ideo_info_t;

news_up_callback cb;
BOOL redraw_limit = FALSE;
u8 curn_img_idx = 0;
static comm_help_data_t help_data =
{
  3,
  3,
  {
   IDS_PAGE,
   IDS_RETURN,
   IDS_EXIT,
  },
  {
   IM_HELP_ARROW,
   IM_HELP_MENU,
   IM_HELP_EXIT,
  },
};
#if 0
static comm_help_data_t help_data_pic =
{
  3,
  3,
  {
   IDS_CHANGE,
   IDS_RETURN,
   IDS_EXIT,
  },
  {
   IM_HELP_ARROW,
   IM_MENU,
   IM_EXIT,
  },
};
#endif
//static u16 pos = 0;
static u16 *unistr = NULL;

u16 news_info_keymap(u16 key);
RET_CODE news_info_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
//RET_CODE news_info_text_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 news_info_text_keymap(u16 key);

//static RET_CODE news_info_update(news_t *p_news, u16 show_curn );

static void news_info_update_page_num(control_t* p_ctrl, u16 page_num, u16
total_page)
{
  char num[20] = {0};

  sprintf(num, "%d/%d", page_num, total_page);
  text_set_content_by_ascstr(p_ctrl, (u8*)num);
}

RET_CODE open_news_info(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_prev = NULL, *p_bmp = NULL, *p_text = NULL, 
    *p_sbar = NULL, *p_temp = NULL;
  control_t *p_page_cont = NULL, *p_ctrl = NULL, *p_title = NULL, *p_msg =NULL, *p_text2 = NULL;
  news_t *p_news = NULL;
  RET_CODE ret = SUCCESS;
    
  cb = (news_up_callback)para2;

  p_news = ui_news_get_news();
  //pos = (u16)para1;
  redraw_limit = FALSE;
 // parent_root_id = (u8)para2;
  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_news->count == 0)
  {
    return ERR_FAILURE;
  }

  p_menu = fw_create_mainwin(ROOT_ID_NEWS_INFO,
                        NEWS_INFO_MENU_X, NEWS_INFO_MENU_Y, NEWS_INFO_MENU_W, NEWS_INFO_MENU_H,
                        ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }

#ifndef MODEST
 ctrl_set_rstyle(p_menu, RSI_MAIN_BG1, RSI_MAIN_BG1, RSI_MAIN_BG1);
#else
ctrl_set_rstyle(p_menu, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
#endif
  ctrl_set_proc(p_menu, news_info_proc);
  ctrl_set_keymap(p_menu, news_info_keymap);

  p_title = ctrl_create_ctrl(CTRL_TEXT, (u8)0,
                             NEWS_INFO_TITLE_X, NEWS_INFO_TITLE_Y,
                             NEWS_INFO_TITLE_W, NEWS_INFO_TITLE_H,
                             p_menu, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_by_ascstr(p_title,(u8*)p_news->p_item[(u16)para1].p_title);

  p_msg = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_NEWS_GET_MSG_TP,
                             NEWS_INFO_MSG_X, NEWS_INFO_MSG_Y,
                             NEWS_INFO_MSG_W, NEWS_INFO_MSG_H,
                             p_menu, 0);
  ctrl_set_rstyle(p_msg, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  p_temp = ctrl_create_ctrl(CTRL_TEXT, (u8)1,
                             0, 0,
                             NEWS_INFO_MSG_W, NEWS_INFO_MSG_H,
                             p_msg, 0);
  ctrl_set_rstyle(p_temp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_temp, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_temp, TEXT_STRTYPE_UNICODE);
  text_set_align_type(p_temp, STL_CENTER | STL_VCENTER);

 p_page_cont = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_NEWS_INFO_PREV_T,
    NEWS_INFO_PAGE_CONT_X, NEWS_INFO_PAGE_CONT_Y, NEWS_INFO_PAGE_CONT_W, NEWS_INFO_PAGE_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_page_cont, RSI_SMALL_LIST_ARROW,
RSI_SMALL_LIST_ARROW_SELECT, RSI_SMALL_LIST_ARROW_SELECT);

   ctrl_set_sts(p_page_cont, OBJ_STS_HIDE);

   p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 1,
                                 NEWS_INFO_NAME_X, NEWS_INFO_NAME_Y,
                                 NEWS_INFO_NAME_W, NEWS_INFO_NAME_H,
                                 p_page_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_by_strid(p_ctrl, IDS_PAGE1);

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 2,
                                 NEWS_INFO_PAGE_X,  NEWS_INFO_PAGE_Y,
                                 NEWS_INFO_PAGE_W,  NEWS_INFO_PAGE_H,
                                 p_page_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  news_info_update_page_num(p_ctrl,0 ,0);

  p_prev = ctrl_create_ctrl(CTRL_CONT, IDC_NEWS_INFO_PREV,
    NEWS_INFO_PREV_X, NEWS_INFO_PREV_Y, NEWS_INFO_PREV_W, NEWS_INFO_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_prev, RSI_ITEM_12_SH_TRANS, RSI_ITEM_12_SH_TRANS, RSI_ITEM_12_SH_TRANS);
  ctrl_set_sts(p_prev, OBJ_STS_HIDE);

  p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_NEWS_INFO_BMP,
    NEWS_INFO_PREV_X, NEWS_INFO_PREV_Y, NEWS_INFO_PREV_W, NEWS_INFO_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_bmp, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  bmap_set_content_by_id(p_bmp, IM_PIC_LOADING_1);
  ctrl_set_sts(p_bmp, OBJ_STS_HIDE);
  

  p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_NEWS_INFO_TEXT,
    NEWS_INFO_TEXT_X, NEWS_INFO_TEXT_Y, NEWS_INFO_TEXT_W, NEWS_INFO_TEXT_H, p_menu, 0);
  ctrl_set_keymap(p_text, news_info_text_keymap);
  ctrl_set_rstyle(p_text, RSI_GRAY_FRM, RSI_GRAY_FRM, RSI_GRAY_FRM);
  text_set_font_style(p_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_text, TEXT_STRTYPE_EXTSTR);
  text_set_align_type(p_text, STL_LEFT | STL_TOP);
  text_enable_page(p_text, TRUE);
 // text_set_offset(p_text, 5 ,5);
 text_set_offset(p_text, 15 ,25);
  ctrl_set_sts(p_text, OBJ_STS_HIDE);

  p_text2 = ctrl_create_ctrl(CTRL_TEXT, IDC_NEWS_INFO_TEXT2,
    NEWS_INFO_TEXT2_X, NEWS_INFO_TEXT_Y, NEWS_INFO_TEXT2_W, NEWS_INFO_TEXT_H, p_menu, 0);
  ctrl_set_keymap(p_text2, news_info_text_keymap);
  ctrl_set_rstyle(p_text2, RSI_WHITE_FRM, RSI_WHITE_FRM, RSI_WHITE_FRM);
  text_set_font_style(p_text2, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_text2, TEXT_STRTYPE_EXTSTR);
  text_set_align_type(p_text2, STL_LEFT | STL_TOP);
  text_enable_page(p_text2, TRUE);
  text_set_offset(p_text2, 15 ,25);
  ctrl_set_sts(p_text2, OBJ_STS_HIDE);

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_NEWS_INFO_SBAR,
    NEWS_INFO_SBAR_X, NEWS_INFO_SBAR_Y, NEWS_INFO_SBAR_W, NEWS_INFO_SBAR_H, p_menu, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  text_set_scrollbar(p_text, p_sbar);
  ctrl_set_sts(p_sbar, OBJ_STS_HIDE);

  ctrl_default_proc(p_text, MSG_GETFOCUS, 0, 0);

  pic_stop();
  ui_pic_clear_all(0);
  ret = ui_news_detail_create_filter(p_news,( u16) para1);
  if(ERR_FAILURE == ret)
  {
    text_set_content_by_ascstr(p_temp,(u8*)"Failed to get news detail."); 
  }
  else
  {
    text_set_content_by_ascstr(p_temp,(u8*)"Getting news detail, Please wait.");
  }

  ctrl_paint_ctrl(p_menu, FALSE);
    
  return SUCCESS;
}

static RET_CODE news_info_destroy(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
   
  redraw_limit = FALSE;
  curn_img_idx = 0;
  pic_stop();
  ui_pic_clear_all(0);

  return ERR_NOFEATURE;
}
#if 0
static RET_CODE news_info_update(news_t *p_news, u16 show_curn )
{
  control_t *p_text = NULL;
  mul_pic_param_t pic_param = {{0},};
  u32 total_vod = 0;

  total_vod = p_news->count;
  if(p_news == NULL)
  {
    return ERR_FAILURE;

  }

  if(p_news->count == 0)
  {
    return ERR_FAILURE;
  }

  //request picture.
  ui_pic_set_url(p_news->p_item[show_curn].media_content.p_url);

  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = NEWS_INFO_PREV_X;
  pic_param.win_rect.top = NEWS_INFO_PREV_Y;
  pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_INFO_PREV_W);
  pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_INFO_PREV_H);
  pic_param.file_size = 0;
  pic_param.handle = 0;

  offset_rect(&pic_param.win_rect, NEWS_INFO_MENU_X, NEWS_INFO_MENU_Y);

  pic_start(&pic_param);

  p_text = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_TEXT);

  text_set_content_by_ascstr(p_text,  p_news->p_item[show_curn].p_description);
  text_reset_param(p_text);

  ctrl_paint_ctrl(p_text, TRUE);

  return SUCCESS;
}
#endif
static RET_CODE news_content_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(MSG_NEWS_EVT_FAILED == msg)
  {
    control_t *p_msg = NULL;
    
    p_msg  = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_GET_MSG_TP);
    text_set_content_by_ascstr(ctrl_get_child_by_id(p_msg, 1),  (u8*)"Failed to get news detail.");

    ctrl_paint_ctrl(p_msg, TRUE);
    return SUCCESS;
  }
  if(NEWS_EVT_GET_DETAIL == para1)
  {
    news_html_t *new_content = (news_html_t *)para2;

    control_t *p_text = NULL;
    control_t *p_sbar = NULL;
    control_t *p_img = NULL;
    //control_t *p_imgT = NULL;
    control_t *p_msg = NULL;
    control_t *temp = NULL;
    control_t *p_bmp = NULL;

    mul_pic_param_t pic_param = {{0},}; 
    BOOL IsHasImg = FALSE;

    p_sbar = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_SBAR);
    p_img = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_PREV);
    //p_imgT = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_PREV_T);
    p_msg  = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_GET_MSG_TP);
    p_bmp = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_BMP);

    if(new_content->img_cnt == 0 && new_content->seg_len == 0)
    {
      text_set_content_by_ascstr(ctrl_get_child_by_id(p_msg, 1),  (u8*)"Failed to get news detail.");
      ctrl_paint_ctrl(p_msg, TRUE);
      return ERR_FAILURE;
    }
    else
    {
      ctrl_set_sts(ctrl_get_child_by_id(p_msg, 1), OBJ_STS_HIDE);
    }

    if(new_content->img_cnt != 0 
      && new_content->p_img[0] != NULL
      && strlen(new_content->p_img[0]) > 2)
    {
      IsHasImg = TRUE;
      p_text = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_TEXT);
  //    ui_comm_help_create(&help_data_pic, p_cont);
  //    ui_comm_help_move_pos(p_cont, 200, 20, 700, 20, 42);
    }
    else
    {
      temp = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_TEXT);
      p_text = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_TEXT2);
      ctrl_default_proc(temp, MSG_LOSTFOCUS, 0, 0);
      ctrl_default_proc(p_text, MSG_GETFOCUS, 0, 0);
      text_set_scrollbar(p_text, p_sbar);
   //   ui_comm_help_create(&help_data, p_cont);
   //   ui_comm_help_move_pos(p_cont, 200, 20, 700, 20, 42);
    }

    ui_comm_help_create_net(&help_data, p_cont);
    //ui_comm_help_move_pos(p_cont, 200, 20, 700, 20, 42);
    ui_comm_help_move_pos(p_cont, 200, (u16)-30, 700, 20, 42);
    if(unistr != NULL)
    {
      mtos_free(unistr);
    }
    if((strlen(new_content->p_segment) != 0))
    {
      ctrl_set_sts(p_text, OBJ_STS_SHOW);
      ctrl_set_sts(p_sbar, OBJ_STS_SHOW);
	  
      unistr = (u16 *)mtos_malloc((strlen(new_content->p_segment) + 1) * 2);
      str_asc2uni((const u8*)new_content->p_segment, unistr);	  
      text_set_content_by_extstr(p_text,  unistr);
      text_reset_param(p_text);
	  ctrl_paint_ctrl(p_msg, TRUE);
      ctrl_paint_ctrl(p_text, TRUE);
    }

    if(IsHasImg)
    {
      redraw_limit = TRUE;
      curn_img_idx = 0;
      //ctrl_set_sts(p_imgT, OBJ_STS_SHOW);
      ctrl_set_sts(p_img, OBJ_STS_SHOW);
      ctrl_set_sts(p_bmp, OBJ_STS_SHOW);

    //  news_info_update_page_num(ctrl_get_child_by_id(p_imgT, 2),1 ,new_content->img_cnt);
    //  ctrl_paint_ctrl(p_imgT, TRUE);
      ctrl_paint_ctrl(p_img, TRUE);
      ctrl_paint_ctrl(p_bmp, TRUE);
      ui_pic_set_url((u8*)new_content->p_img[0]);

      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = NEWS_INFO_PREV_X;
      pic_param.win_rect.top = NEWS_INFO_PREV_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_INFO_PREV_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_INFO_PREV_H);
      pic_param.file_size = 0;
      pic_param.handle = 0;

      offset_rect(&pic_param.win_rect, NEWS_INFO_MENU_X, NEWS_INFO_MENU_Y);
      pic_start(&pic_param);
    }
  }

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE news_info_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bmp = NULL;

  if(fw_get_focus_id() != ROOT_ID_NEWS_INFO)
  {
    return ERR_FAILURE;
  }  

  if((msg == MSG_PIC_EVT_DRAW_END) || (msg == MSG_PIC_EVT_DRAW_END))
  {
    p_bmp = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_BMP);
    ctrl_set_sts(p_bmp, OBJ_STS_HIDE);
    bmap_set_content_by_id(p_bmp, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_cont, TRUE);
  }
  return SUCCESS;
}

static RET_CODE news_pic_redraw(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(redraw_limit)
  {
    news_html_t *news_detail = NULL;
    mul_pic_param_t pic_param = {{0},};

    OS_PRINTF("#* ui_news_info draw pic error\n");

    pic_stop();
    ui_pic_clear_all(0);

    news_detail = ui_news_get_detail();

    ui_pic_set_url((u8*)news_detail->p_img[curn_img_idx]);

    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = NEWS_INFO_PREV_X;
    pic_param.win_rect.top = NEWS_INFO_PREV_Y;
    pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_INFO_PREV_W);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_INFO_PREV_H);
    pic_param.file_size = 0;
    pic_param.handle = 0;

    offset_rect(&pic_param.win_rect, NEWS_INFO_MENU_X, NEWS_INFO_MENU_Y);
    pic_start(&pic_param);
  }
  return SUCCESS;
}
#if 0
static RET_CODE news_pic_change(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(para1 == V_KEY_LEFT || para1 == V_KEY_RIGHT)
  {
    mul_pic_param_t pic_param = {{0},};
    control_t *p_imgT = NULL;
    u8 img_cnt = 0;
    news_html_t *news_detail = NULL;

    news_detail = ui_news_get_detail();

    if(news_detail->img_cnt == 0 || news_detail->img_cnt == 1)
    {
      return SUCCESS;
    }

    img_cnt = (news_detail->img_cnt > MAX_NEWS_IMG_CNT)?MAX_NEWS_IMG_CNT:news_detail->img_cnt;

    if(MSG_PAGE_UP == msg)
    {
      if(curn_img_idx == 0)
      {
        curn_img_idx = img_cnt - 1;
      }
      else
      {
        curn_img_idx --;
      }
    }
    else
    {
      if(curn_img_idx == img_cnt - 1)
      {
        curn_img_idx = 0;
      }
      else
      {
        curn_img_idx ++ ;
      }
    }

    pic_stop();
    ui_pic_clear_all(0);
    if(news_detail->p_img[curn_img_idx] != NULL
       && strlen(news_detail->p_img[curn_img_idx]) > 2)
    {
      ui_pic_set_url(news_detail->p_img[curn_img_idx]);
    }
    else
    {
      OS_PRINTF("#* news_pic_change url is error\n");
      return SUCCESS;
    }

    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = NEWS_INFO_PREV_X;
    pic_param.win_rect.top = NEWS_INFO_PREV_Y;
    pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_INFO_PREV_W);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_INFO_PREV_H);
    pic_param.file_size = 0;
    pic_param.handle = 0;

    offset_rect(&pic_param.win_rect, NEWS_INFO_MENU_X, NEWS_INFO_MENU_Y);
    pic_start(&pic_param);
    p_imgT = ui_comm_root_get_ctrl(ROOT_ID_NEWS_INFO, IDC_NEWS_INFO_PREV_T);
    news_info_update_page_num(ctrl_get_child_by_id(p_imgT, 2),curn_img_idx + 1, img_cnt);

    ctrl_paint_ctrl(p_imgT, TRUE);

    return SUCCESS;
  }
  return ERR_NOFEATURE;
}
#endif

static RET_CODE on_news_info_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;
  manage_close_menu(ROOT_ID_NEWS_INFO, 0, 0);
  p_root = fw_find_root_by_id(ROOT_ID_NEWS_TITLE);
  if(p_root != NULL)
  {
    news_clear_bmp(p_root);
    ctrl_paint_ctrl(p_root, TRUE);
  }
  cb();
  return SUCCESS;
}
static RET_CODE on_news_info_exit_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();
  return SUCCESS;
}
BEGIN_KEYMAP(news_info_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(news_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(news_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, news_info_destroy)
  ON_COMMAND(MSG_NEWS_EVT_SUCCESS, news_content_update)
  ON_COMMAND(MSG_NEWS_EVT_FAILED, news_content_update)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, news_pic_redraw)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, news_info_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, news_info_draw_end)
  ON_COMMAND(MSG_EXIT, on_news_info_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_news_info_exit_all)
END_MSGPROC(news_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(news_info_text_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
  ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
END_KEYMAP(news_info_text_keymap, NULL)

#if 0
BEGIN_MSGPROC(news_info_text_proc, text_class_proc)
  ON_COMMAND(MSG_PAGE_UP, news_pic_change)
  ON_COMMAND(MSG_PAGE_DOWN, news_pic_change)
END_MSGPROC(news_info_text_proc, text_class_proc)
#endif
