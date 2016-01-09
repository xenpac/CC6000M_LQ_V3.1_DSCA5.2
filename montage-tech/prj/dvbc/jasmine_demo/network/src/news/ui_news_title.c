/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_news_title.h"
#include "ui_picture_api.h"

#include "news_filter.h"
#include "news_api.h"
#include "ui_news_api.h"

typedef enum
{
  MSG_INFO = MSG_LOCAL_BEGIN + 0x100,
}ui_news_title_msg_t;

//#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef enum
{
  IDC_NEWS_TITLE_CONT = 1,  
  IDC_NEWS_TITLE,
  IDC_NEWS_ICONT,
  IDC_NEWS_PAGE_NUM,
}news_id_t;

typedef enum
{
  IDC_NEWS_ITEM1 = 1,
  IDC_NEWS_ITEM2,
  IDC_NEWS_ITEM3,
  IDC_NEWS_ITEM4,
  IDC_NEWS_ITEM5,  
}news_icont_t;

typedef enum
{
  IDC_NEWS_ITEM_PREW = 1,
  IDC_NEWS_ITEM_BMP,
  IDC_NEWS_ITEM_TEXT,
}news_item_ctrl_t;

typedef enum
{
  IDC_NEWS_ITEM_L1 = 1,
  IDC_NEWS_ITEM_L2,
  IDC_NEWS_ITEM_R1,
  IDC_NEWS_ITEM_R2,
  IDC_NEWS_ITEM_R3,
}news_item_child_t;

static comm_help_data_t help_data = 
{
  4,                                   
  4,
  {
   IDS_DETAIL,
   IDS_RETURN,
   IDS_EXIT,
   IDS_PAGE,
  },
  {
   IM_HELP_OK,
   IM_HELP_MENU,
   IM_HELP_EXIT,  
   IM_HELP_PAGE,
  },
};

//current loading preview(1 - page).
static u16 g_news_load_curn = 0;
static u16 g_show_curn = 0; 
static u16 g_request_type = 0;

//current show. (0 - max count)


u16 news_cont_keymap(u16 key);
RET_CODE news_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 news_item_keymap(u16 key);
RET_CODE news_item_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

static BOOL news_update_pic(void);


void news_clear_bmp(control_t *p_menu)
{
  control_t *p_icont = NULL, *p_item = NULL, *p_bmp = NULL;
  u16 i = 0;

  p_icont = ctrl_get_child_by_id(p_menu, IDC_NEWS_ICONT);
  
  for(i = IDC_NEWS_ITEM1; i <= IDC_NEWS_ITEM4; i++)
  {
    p_item = ctrl_get_child_by_id(p_icont, i);
    p_bmp = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_BMP);
    
    bmap_set_content_by_id(p_bmp, IM_INDEX_NETWORK_YOUTUBE_PIC);
  }
}

static void news_clear_all(control_t *p_menu)
{
  control_t *p_icont = NULL, *p_item = NULL, *p_bmp = NULL;
  control_t *p_tcont = NULL;
  u16 i = 0;

  p_icont = ctrl_get_child_by_id(p_menu, IDC_NEWS_ICONT);
  
  for(i = IDC_NEWS_ITEM1; i <= IDC_NEWS_ITEM4; i++)
  {
    p_item = ctrl_get_child_by_id(p_icont, i);
    p_bmp = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_BMP);
    p_tcont = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_TEXT);
    
    text_set_content_by_ascstr(p_tcont, (u8*)" ");
    bmap_set_content_by_id(p_bmp, IM_INDEX_NETWORK_YOUTUBE_PIC);
    ctrl_paint_ctrl(p_item, TRUE);
  }
  ui_pic_clear_all(0);
}

static void news_update_page_num(control_t* p_ctrl, u16 page_num, u16 
total_page)
{
  char num[20] = {0};

  sprintf(num, "%d/%d", page_num, total_page);

  text_set_content_by_ascstr(p_ctrl, (u8*)num);
}

static control_t *news_create_item(control_t *p_parent, u8 id, u16 x, u16 y)
{
  control_t *p_cont = NULL, *p_prew = NULL, *p_bmp = NULL, *p_text = NULL;
  
  p_cont = ctrl_create_ctrl(CTRL_CONT, id, x, y, NEWS_ITEM_W, NEWS_ITEM_H, p_parent, 0);
  ctrl_set_keymap(p_cont, news_item_keymap);
  ctrl_set_proc(p_cont, news_item_proc);
  ctrl_set_style(p_cont, STL_EX_WHOLE_HL);

  p_prew = ctrl_create_ctrl(CTRL_CONT, IDC_NEWS_ITEM_PREW,
    NEWS_ITEM_PREWX, NEWS_ITEM_PREWY, NEWS_ITEM_PREWW, NEWS_ITEM_PREWH, p_cont, 0);
  ctrl_set_rstyle(p_prew, RSI_ITEM_12_SH_TRANS, RSI_ITEM_12_SH_TRANS, RSI_ITEM_12_SH_TRANS);

  p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_NEWS_ITEM_BMP,
    NEWS_ITEM_PREWX, NEWS_ITEM_PREWY, NEWS_ITEM_PREWW, NEWS_ITEM_PREWH, p_cont, 0);
  ctrl_set_rstyle(p_bmp, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  bmap_set_content_by_id(p_bmp, IM_INDEX_NETWORK_YOUTUBE_PIC);

  p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_NEWS_ITEM_TEXT,
    NEWS_ITEM_TEXTX, NEWS_ITEM_TEXTY, NEWS_ITEM_TEXTW, NEWS_ITEM_TEXTH, p_cont, 0);
  ctrl_set_rstyle(p_text, RSI_ITEM_11_SH, RSI_POPUP_BG, RSI_ITEM_12_SH);
  ctrl_set_mrect(p_text, 8, 6, NEWS_ITEM_TEXTW - 16, NEWS_ITEM_TEXTH - 12);
  text_set_align_type(p_text, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
  
  return p_cont;
}

RET_CODE open_news_title(u32 para1, u32 para2)
{
  control_t *p_menu = NULL,  *p_page_cont = NULL, *p_item[NEWS_ITEM_PAGE] = {0};
  control_t *p_icont = NULL, *p_ctrl = NULL;
  control_t *p_title_cont = NULL;
  u16 i = 0, y = 0;
  u8 up = 0, down = 0;
  //lint -e64
  news_req_t news_req = {0};
  //lint +e64

  g_news_load_curn = 0;
  g_show_curn = 0;

  //create menu.
  p_menu = fw_create_mainwin(ROOT_ID_NEWS_TITLE,
                       NEWS_CONT_X, NEWS_CONT_Y, NEWS_CONT_W, NEWS_CONT_H,
                       0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_menu != NULL);
  ctrl_set_keymap(p_menu, news_cont_keymap);
  ctrl_set_proc(p_menu, news_cont_proc);
  #ifndef MODEST
  ctrl_set_rstyle(p_menu, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  #else
  ctrl_set_rstyle(p_menu, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
  #endif

// title cont
  p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NEWS_TITLE_CONT,
                            0, 0, TITLE_W - NEWS_OFFSET, TITLE_H, p_menu, 0);
#ifndef MODEST
  ctrl_set_rstyle(p_title_cont, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
#else
  ctrl_set_rstyle(p_title_cont, RSI_COMMON_TITLE, RSI_COMMON_TITLE, RSI_COMMON_TITLE);
#endif

  //page
  p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NEWS_TITLE,
    NEWS_TTL_X, NEWS_TTL_Y, NEWS_TTL_W, NEWS_TTL_H, p_title_cont, 0);
  ctrl_set_rstyle(p_page_cont, RSI_PAGE_ARROW, RSI_PAGE_ARROW, RSI_PAGE_ARROW);
    //RSI_SMALL_LIST_ARROW, RSI_SMALL_LIST_ARROW, RSI_SMALL_LIST_ARROW);
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 NEWS_TTL_NAME_X, NEWS_TTL_NAME_Y,
                                 NEWS_TTL_NAME_W, NEWS_TTL_NAME_H,
                                 p_page_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_by_strid(p_ctrl, IDS_PAGE1);

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_NEWS_PAGE_NUM,
                                 NEWS_TTL_PAGE_X,  NEWS_TTL_PAGE_Y,
                                 NEWS_TTL_PAGE_W,  NEWS_TTL_PAGE_H,
                                 p_page_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
 
  //create video preview item container.
  p_icont = ctrl_create_ctrl(CTRL_CONT, IDC_NEWS_ICONT,
    NEWS_ICONT_X, NEWS_ICONT_Y, NEWS_ICONT_W, NEWS_ICONT_H, p_menu, 0);
  ctrl_set_rstyle(p_icont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  //create item
  y = NEWS_ITEM_Y;
  
  for(i = 0; i < NEWS_ITEM_PAGE; i++)
  {
    p_item[i] = news_create_item(p_icont, (u8)(IDC_NEWS_ITEM1 + i), NEWS_ITEM_X, y);

    up = (u8)(IDC_NEWS_ITEM1 + i - 1);
    if(up < IDC_NEWS_ITEM1)
    {
      up = IDC_NEWS_ITEM4;
    }
    
    down = (u8)(IDC_NEWS_ITEM1 + i + 1);
    if(down > IDC_NEWS_ITEM4)
    {
      down = IDC_NEWS_ITEM1;
    }
    
    ctrl_set_related_id(p_item[i], 0, up, 0, down);
    
    y += (NEWS_ITEM_H + NEWS_VGAP);
  }

  //help  bar
  ui_comm_help_create_net(&help_data, p_menu);
 // ui_comm_help_move_pos(p_menu, 100, 40, 880, 0, 40);
 ui_comm_help_move_pos(p_menu, 100, (u16)-60, 880, 0, 40);

  list_get_focus_pos(p_item[0]);
  ctrl_default_proc(p_item[0], MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  news_req.request_type = (news_type_t)para1;
  g_request_type = news_req.request_type;
  news_req.count= NEWS_ITEM_PAGE;
  news_req.start = 0;
  ui_news_get(&news_req);
 
  return SUCCESS;
}

static RET_CODE news_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  g_news_load_curn = 0;
  g_show_curn = 0;
  
  pic_stop();
  news_clear_all(p_cont);
  ui_pic_clear_all(0);

  free_news_html();
  free_news_mem();

  return ERR_NOFEATURE;
}

static RET_CODE news_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  return ERR_NOFEATURE;
}

static RET_CODE news_exit_all(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  return ERR_NOFEATURE;
}

static RET_CODE news_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  control_t *p_icont = NULL, *p_item = NULL, *p_text = NULL, *p_page_cont = NULL, *p_page_title = NULL, *p_page = NULL;
  news_t *p_news = NULL;
  mul_pic_param_t pic_param = {{0},};
  u16 curn_page = 0, total_page = 0;
  u32 total_vod = 0;
  u16 i = 0;

  OS_PRINTF("@# news_success\n");

  if(fw_get_focus_id() != ROOT_ID_NEWS_TITLE)
  {
    return ERR_FAILURE;
  }  

  if(para1 != NEWS_EVT_GET_NEWS)
  {
    return ERR_FAILURE;
  }
  
  p_news =(news_t *)para2;

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
  for(g_news_load_curn = 0; 
      g_news_load_curn < NEWS_ITEM_PAGE && g_show_curn + g_news_load_curn < p_news->count;
      g_news_load_curn ++)
  {
    if(p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url == NULL)
    {
      continue;
    }
    //OS_PRINTF("pic url = [%s]\n",p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url);
    ui_pic_set_url((u8*)p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url);

    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = NEWS_ITEM_PREWX;
    pic_param.win_rect.top = NEWS_ITEM_PREWY;
    pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_ITEM_PREWW);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_ITEM_PREWH);
    pic_param.file_size = 0;
    pic_param.handle = 0;

    offset_rect(&pic_param.win_rect, NEWS_ITEM_X, (s16)(NEWS_ITEM_Y + g_news_load_curn * (NEWS_ITEM_H + NEWS_VGAP)));
    offset_rect(&pic_param.win_rect, NEWS_ICONT_X, NEWS_ICONT_Y);
    offset_rect(&pic_param.win_rect, NEWS_CONT_X, NEWS_CONT_Y);

    pic_start(&pic_param); 
    break;
  }
  for(i = 0; 
      i < NEWS_ITEM_PAGE && g_show_curn + i < p_news->count; 
      i ++ )
  {
    //set text
    p_icont = ctrl_get_child_by_id(p_cont, IDC_NEWS_ICONT);
    p_item = ctrl_get_child_by_id(p_icont, IDC_NEWS_ITEM1 + i);
    p_text = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_TEXT);
    if(p_news->p_item[g_show_curn + i].p_title != NULL)
    {
      text_set_content_by_ascstr(p_text, (u8*)p_news->p_item[g_show_curn + i].p_title);
    }
    else
    {
      text_set_content_by_ascstr(p_text, (u8*)"No content.");
    }
  }

    //update item text.
  p_page_cont = ctrl_get_child_by_id(p_cont, IDC_NEWS_TITLE_CONT);  
  p_page_title = ctrl_get_child_by_id(p_page_cont, IDC_NEWS_TITLE);
  p_page = ctrl_get_child_by_id(p_page_title, IDC_NEWS_PAGE_NUM);

  curn_page = g_show_curn / NEWS_ITEM_PAGE + 1; 
  if(total_vod != 0&&total_vod % NEWS_ITEM_PAGE == 0)
  {
    total_page = (u16)(total_vod / NEWS_ITEM_PAGE);
  }
  else
  {
    total_page = (u16)(total_vod / NEWS_ITEM_PAGE + 1);
  }

  news_update_page_num(p_page, curn_page, total_page);

  ctrl_paint_ctrl(ctrl_get_parent(p_item), TRUE);
  ctrl_paint_ctrl(p_page_title, TRUE);
  g_news_load_curn ++;
  
  return SUCCESS;
}

static RET_CODE news_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{   
  OS_PRINTF("ytbl_failed\n");
   if(fw_get_focus_id() != ROOT_ID_NEWS_TITLE)
  {
    return ERR_FAILURE;
  }  
  
  ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 3000);
  return SUCCESS;
}

static RET_CODE news_item_focus_up(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  u16 ctrl_id = 0;
  //news_t *p_news = NULL;
  control_t *p_menu = NULL;
  ctrl_id = ctrl_get_ctrl_id(p_cont);
  p_menu = ctrl_get_parent(ctrl_get_parent(p_cont));

  if(ctrl_id != IDC_NEWS_ITEM1)
  {
    return ctrl_process_msg(ctrl_get_parent(p_cont), MSG_FOCUS_UP, 0, 0);
  }
  //try to page down
  if(g_show_curn < NEWS_ITEM_PAGE)
  {
    return SUCCESS;
  }    

  //p_news = ui_news_get_news();

  //caculate start.
  g_show_curn -= NEWS_ITEM_PAGE;

  pic_stop();
  news_clear_all(p_menu);

  news_update_pic();  

  return SUCCESS;
}

static RET_CODE news_item_focus_down(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  u16 ctrl_id = 0;
  news_t *p_news = NULL;
  control_t *p_menu = NULL;

  ctrl_id = ctrl_get_ctrl_id(p_cont);
  p_menu = ctrl_get_parent(ctrl_get_parent(p_cont));

  if(ctrl_id != IDC_NEWS_ITEM4)
  {
    return ctrl_process_msg(ctrl_get_parent(p_cont), MSG_FOCUS_DOWN, 0, 0);
  }

  p_news = ui_news_get_news();
  if((g_show_curn + NEWS_ITEM_PAGE) >= p_news->count)
  {
     return SUCCESS;
  } 
  
  //caculate start.
  g_show_curn += NEWS_ITEM_PAGE;

  pic_stop();
  news_clear_all(p_menu);
  
  news_update_pic();
  
  return SUCCESS;
}

static BOOL news_update_pic()
{ 
  news_t *p_news = NULL;
  mul_pic_param_t pic_param = {{0},};
  u8 i;
  control_t *p_icont = NULL, *p_item = NULL, *p_page_cont = NULL, *p_page_title = NULL, 
            *p_text = NULL, *p_cont = NULL, *p_page = NULL, *p_help = NULL;
  u16 curn_page = 0, total_page = 0;
  u32 total_vod = 0;
  //u8 i = 0;
  
  p_news = ui_news_get_news();

  total_vod = p_news->count;
  
  p_cont = fw_find_root_by_id(ROOT_ID_NEWS_TITLE);
  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  g_news_load_curn = 0;
  
  for(; g_news_load_curn < NEWS_ITEM_PAGE && g_show_curn + g_news_load_curn < p_news->count; 
      g_news_load_curn ++)
  {
    if(p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url == NULL)
    {
      continue;
    }

    ui_pic_set_url((u8*)p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url);

    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = NEWS_ITEM_PREWX;
    pic_param.win_rect.top = NEWS_ITEM_PREWY;
    pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_ITEM_PREWW);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_ITEM_PREWH);
    pic_param.file_size = 0;
    pic_param.handle = 0;

    offset_rect(&pic_param.win_rect, NEWS_ITEM_X, (s16)(NEWS_ITEM_Y + g_news_load_curn * (NEWS_ITEM_H + NEWS_VGAP)));
    offset_rect(&pic_param.win_rect, NEWS_ICONT_X, NEWS_ICONT_Y);
    offset_rect(&pic_param.win_rect, NEWS_CONT_X, NEWS_CONT_Y);
    
    pic_start(&pic_param); 
    break;
  }
  for(i = 0; 
      i < NEWS_ITEM_PAGE && g_show_curn + i < p_news->count; 
      i ++ )
  {
    //set text
    p_icont = ctrl_get_child_by_id(p_cont, IDC_NEWS_ICONT);
    p_item = ctrl_get_child_by_id(p_icont, IDC_NEWS_ITEM1 + i);
    p_text = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_TEXT);
    if(p_news->p_item[g_show_curn + i].p_title != NULL)
    {
      text_set_content_by_ascstr(p_text, (u8*)p_news->p_item[g_show_curn + i].p_title);
    }
    else
    {
      text_set_content_by_ascstr(p_text, (u8*)"No content.");
    }

    ctrl_paint_ctrl(p_icont, TRUE);
  }
      //update item text.
  p_page_cont = ctrl_get_child_by_id(p_cont, IDC_NEWS_TITLE_CONT);  
  p_page_title = ctrl_get_child_by_id(p_page_cont, IDC_NEWS_TITLE);
  p_page = ctrl_get_child_by_id(p_page_title, IDC_NEWS_PAGE_NUM);
  curn_page = g_show_curn / NEWS_ITEM_PAGE + 1; 
  if(total_vod != 0&&total_vod % NEWS_ITEM_PAGE == 0)
  {
    total_page = (u16)(total_vod / NEWS_ITEM_PAGE);
  }
  else
  {
    total_page = (u16)(total_vod / NEWS_ITEM_PAGE + 1);
  }

  news_update_page_num(p_page, curn_page, total_page);

  ctrl_paint_ctrl(p_page, TRUE);

  p_help = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_CONT);
  ctrl_destroy_ctrl(p_help);

  ui_comm_help_create_net(&help_data, p_cont);
  //ui_comm_help_move_pos(p_cont, 100, 40, 880, 0, 40);
  ui_comm_help_move_pos(p_cont, 100, (u16)-60, 880, 0, 40);
  //ctrl_add_rect_to_invrgn(p_help, NULL);
  g_news_load_curn ++;
  return SUCCESS;
}


static RET_CODE news_item_info(control_t *p_item, u16 msg, u32 para1, u32 para2)
{ 
  news_t *p_news = NULL;
  u32 total_vod = 0;
  u16 ctrl_id = 0;

  news_up_callback cb;
  
  ctrl_id = ctrl_get_ctrl_id(p_item);
  p_news =  ui_news_get_news();

  total_vod = p_news->count;

  if(total_vod <= (u32)(g_show_curn + ctrl_id -1))
  {
    return ERR_FAILURE;
  }

  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_news->count == 0)
  {
    return ERR_FAILURE;
  }
  cb = news_update_pic;
  return manage_open_menu(ROOT_ID_NEWS_INFO,  g_show_curn + ctrl_id -1, (u32)cb);
}

static RET_CODE news_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  news_t *p_news = NULL;
  mul_pic_param_t pic_param = {{0},};
  control_t *p_icont = NULL, *p_item = NULL, *p_bmp = NULL;

  p_icont = ctrl_get_child_by_id(p_cont, IDC_NEWS_ICONT);
  p_item = ctrl_get_child_by_id(p_icont, (u16)(IDC_NEWS_ITEM1 + para1));
  p_bmp = ctrl_get_child_by_id(p_item, IDC_NEWS_ITEM_BMP);

  if(fw_get_focus_id() != ROOT_ID_NEWS_TITLE)
  {
    return ERR_FAILURE;
  }  

  p_news = ui_news_get_news();

  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  if(para1 > p_news->count)
  {
    return ERR_FAILURE;
  }

  if((para1 + 1) >= p_news->count)
  {
    return ERR_FAILURE;
  }

  if(g_news_load_curn >= NEWS_ITEM_PAGE)
  {
    return ERR_FAILURE;
  }

  if(g_news_load_curn >= p_news->count)
  {
    return ERR_FAILURE;
  } 
  
  for(; g_news_load_curn < NEWS_ITEM_PAGE && g_show_curn + g_news_load_curn < p_news->count; 
      g_news_load_curn ++)
  {
    if(p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url == NULL)
    {
      continue;
    }

    pic_stop();
    
    ui_pic_set_url((u8*)p_news->p_item[g_news_load_curn +g_show_curn ].media_content.p_url);
    OS_PRINTF("pic url = [%s]\n",p_news->p_item[g_show_curn + g_news_load_curn].media_content.p_url);

    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = NEWS_ITEM_PREWX;
    pic_param.win_rect.top = NEWS_ITEM_PREWY;
    pic_param.win_rect.right = (pic_param.win_rect.left + NEWS_ITEM_PREWW);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + NEWS_ITEM_PREWH);
    pic_param.file_size = 0;
    pic_param.handle = para1 + 1;

    offset_rect(&pic_param.win_rect, NEWS_ITEM_X, (u16)(NEWS_ITEM_Y + g_news_load_curn * (NEWS_ITEM_H + NEWS_VGAP)));
    offset_rect(&pic_param.win_rect, NEWS_ICONT_X, NEWS_ICONT_Y);
    offset_rect(&pic_param.win_rect, NEWS_CONT_X, NEWS_CONT_Y);
    
    bmap_set_content_by_id(p_bmp, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_item, TRUE);
    
    pic_start(&pic_param);  
    break;
  }

  g_news_load_curn ++;

  return SUCCESS;
}
static RET_CODE news_item_change_page(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  news_t *p_news = NULL;
  control_t *p_menu = NULL;

  p_menu = ctrl_get_parent(ctrl_get_parent(p_cont));

  p_news = ui_news_get_news();
  if(msg == MSG_PAGE_DOWN)
  {
    if((g_show_curn + NEWS_ITEM_PAGE) >= p_news->count)
    {
       return SUCCESS;
    } 
  
    g_show_curn += NEWS_ITEM_PAGE;
  }
  else
  {
    if(g_show_curn < NEWS_ITEM_PAGE)
    {
      return SUCCESS;
    }    

    g_show_curn -= NEWS_ITEM_PAGE;
  }
  
  pic_stop();
  news_clear_all(p_menu);

  news_update_pic();
  
  return SUCCESS;
}


BEGIN_KEYMAP(news_cont_keymap, ui_comm_root_keymap)

END_KEYMAP(news_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(news_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, news_on_destory)
  ON_COMMAND(MSG_EXIT, news_exit)
  ON_COMMAND(MSG_EXIT_ALL, news_exit_all)
  ON_COMMAND(MSG_NEWS_EVT_SUCCESS, news_success)
  ON_COMMAND(MSG_NEWS_EVT_FAILED, news_failed)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, news_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, news_draw_end)
END_MSGPROC(news_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(news_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN) 
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(news_item_keymap, NULL)

BEGIN_MSGPROC(news_item_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, news_item_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, news_item_focus_down) 
  ON_COMMAND(MSG_PAGE_UP, news_item_change_page)
  ON_COMMAND(MSG_PAGE_DOWN, news_item_change_page) 
  ON_COMMAND(MSG_SELECT, news_item_info)
END_MSGPROC(news_item_proc, cont_class_proc)

