/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_news.h"
#include "news_filter.h"
#include "news_api.h"
#include "ui_news_api.h"
#include "news_filter.h"


typedef enum
{
  FIRST_PAGE = 0,
  SECOND_PAGE,
}network_page_t;

enum control_id
{
	IDC_INVALID = 0,
	IDC_NEWS_ICON_BOX,
	IDC_NEWS_LOGO,
	IDC_NEWS_BTM_LINE,

};


u16 news_mbox_keymap(u16 key);
RET_CODE news_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE news_menu_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static u16 pos = 0;

RET_CODE open_news(u32 para1, u32 para2)
{
  control_t *p_menu = NULL,*p_mbox_icon = NULL, *p_logo_icon = NULL;
  u16 i = 0;
  u16 news_image_hl[NEWS_ITEM] = 
    {
      IM_ICON_YAHOO_TOPSTORIES_2, IM_ICON_YAHOO_US_2, IM_ICON_YAHOO_WORLD_2, IM_ICON_YAHOO_POLITICS_2,
      IM_ICON_YAHOO_BUSINESS_2,IM_ICON_YAHOO_SPORTS_2,IM_ICON_YAHOO_ENTERTAINMENT_2,IM_ICON_YAHOO_MOSTVIEWED_2,
    };

  u16 news_image_sh[NEWS_ITEM] = 
    {
      IM_ICON_YAHOO_TOPSTORIES_1, IM_ICON_YAHOO_US_1, IM_ICON_YAHOO_WORLD_1, IM_ICON_YAHOO_POLITICS_1,
      IM_ICON_YAHOO_BUSINESS_1,IM_ICON_YAHOO_SPORTS_1,IM_ICON_YAHOO_ENTERTAINMENT_1,IM_ICON_YAHOO_MOSTVIEWED_1,
    };
  u16 text_str[NEWS_ITEM] =
    {
      IDS_NETWORK_TOPSTOR,IDS_NETWORK_US,IDS_NETWORK_WORLD,IDS_NETWORK_POLITICS,IDS_NETWORK_BUSINESS,IDS_SPORTS,
        IDS_NETWORK_ENTERTAINMENT,IDS_NETWORK_MOST_VIEWED
      //{"TOPSTORIES"},{"US"},{"WORLD"},{"POLITICS"},{"BUSINESS"},{"SPORTS"},{"ENTERTAINMENT"},{"MOST_VIEWED"}
    };

   //main menu
  p_menu = ui_comm_root_create_netmenu(ROOT_ID_YAHOO_NEWS, 0, 0, 0);//IDS_NETWORK);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_menu, news_menu_proc);

	//icon logo
	p_logo_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_NEWS_LOGO,
																	UI_NEWS_LOGO_CTRL_X, UI_NEWS_LOGO_CTRL_Y,
																	UI_NEWS_LOGO_CTRL_W, UI_NEWS_LOGO_CTRL_H,
																	p_menu, 0);
	ctrl_set_rstyle(p_logo_icon, RSI_PBACK, RSI_PBACK, RSI_PBACK);

	bmap_set_content_by_id(p_logo_icon, IM_ICON_NETWORK_YAHOO_LOGO);
 //icon box
  p_mbox_icon = ctrl_create_ctrl(CTRL_MBOX, IDC_NEWS_ICON_BOX,
                                 UI_NEWS_CTRL_X, UI_NEWS_CTRL_Y,
                                 UI_NEWS_CTRL_W, UI_NEWS_CTRL_H ,
                                 p_menu, 0);
  //ctrl_set_rstyle(p_mbox_icon, RSI_NETWORK_SELECT_HL, RSI_NETWORK_SELECT_HL, RSI_NETWORK_SELECT_HL);
  ctrl_set_keymap(p_mbox_icon, news_mbox_keymap);
  ctrl_set_proc(p_mbox_icon,news_mbox_proc);
  ctrl_set_mrect(p_mbox_icon, NEWS_MBOX_L, NEWS_MBOX_T, NEWS_MBOX_R, NEWS_MBOX_B);
  mbox_enable_icon_mode(p_mbox_icon, TRUE);
  mbox_enable_string_mode(p_mbox_icon, TRUE);
  mbox_set_count(p_mbox_icon, NEWS_ITEM, NEWS_COL, NEWS_ROW);
  mbox_set_win(p_mbox_icon, NEWS_WINCOL, NEWS_WINROW);
  mbox_set_item_interval(p_mbox_icon, 20, 20);
  mbox_set_item_rstyle(p_mbox_icon, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_mbox_icon, FSI_BLUE_NEWS, FSI_WHITE_INFO, FSI_WHITE_INFO);
  mbox_set_content_strtype(p_mbox_icon, MBOX_STRTYPE_STRID);
  mbox_set_icon_align_type(p_mbox_icon, STL_CENTER| STL_BOTTOM);
  mbox_set_string_offset(p_mbox_icon, 0, 0);
  mbox_set_string_align_type(p_mbox_icon, STL_CENTER | STL_BOTTOM);
  for(i = 0; i < NEWS_ITEM; i++)
  {
    mbox_set_content_by_icon(p_mbox_icon, i, news_image_hl[i], news_image_sh[i]);
    mbox_set_content_by_strid(p_mbox_icon, i, text_str[i]);
  }

  mbox_set_focus(p_mbox_icon, pos);

  ctrl_set_style(p_mbox_icon, STL_EX_ALWAYS_HL);
  
  ctrl_default_proc(p_mbox_icon, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  ui_pic_init(PIC_SOURCE_NET);

  ui_news_init(NEWS_YAHOO);

  return SUCCESS;

}

RET_CODE on_news_box_select(control_t *p_mbox, u16 msg, u32 para1, u32 para2)
{
  u16 nfocus = 0;

  nfocus = mbox_get_focus(p_mbox);

  return manage_open_menu(ROOT_ID_NEWS_TITLE, nfocus, 0);
}

static RET_CODE news_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mbox = NULL;

  p_mbox = ctrl_get_child_by_id(p_cont, IDC_NEWS_ICON_BOX);

  pos =  mbox_get_focus(p_mbox);
  
  pic_stop();

  ui_pic_clear_all(0);

  ui_pic_release();

  ui_news_release();

  return ERR_NOFEATURE;
}

BEGIN_MSGPROC(news_menu_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, news_on_destory)
END_MSGPROC(news_menu_proc, ui_comm_root_proc)

BEGIN_KEYMAP(news_mbox_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(news_mbox_keymap, NULL)

BEGIN_MSGPROC(news_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_news_box_select)
END_MSGPROC(news_mbox_proc, mbox_class_proc)

