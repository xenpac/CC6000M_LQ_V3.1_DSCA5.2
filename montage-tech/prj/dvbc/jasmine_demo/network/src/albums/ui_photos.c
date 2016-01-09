/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"

#include "albums_filter.h"

#include "albums_public.h"

#include "ui_photos.h"

#include "ui_albums_api.h"

#include "ui_picture_api.h"

typedef enum
{
  IDC_PHOTOS_TTL = 1,
  IDC_PHOTOS_PREW_CONT,
}photos_id_t;

typedef enum
{
  IDC_PHOTOS_ITEM1= 1,
  IDC_PHOTOS_ITEM2,
  IDC_PHOTOS_ITEM3,
  IDC_PHOTOS_ITEM4,
  IDC_PHOTOS_ITEM5,
  IDC_PHOTOS_ITEM6,
  IDC_PHOTOS_ITEM7,
  IDC_PHOTOS_ITEM8,
  IDC_PHOTOS_ITEM9,
  IDC_PHOTOS_ITEM10,
  IDC_PHOTOS_ITEM11,
  IDC_PHOTOS_ITEM12,  
}photos_item_cont_t;

typedef enum
{
  IDC_PHOTOS_ITEM_PREW = 1,
  IDC_PHOTOS_ITEM_TEXT,
}photos_item_t;

typedef enum 
{
  MSG_INFO = MSG_LOCAL_BEGIN + 250,
}photo_msg_t;

//cont
#define PHOTOS_CONT_X  0
#define PHOTOS_CONT_Y  0
#define PHOTOS_CONT_W  SCREEN_WIDTH //960
#define PHOTOS_CONT_H  SCREEN_HEIGHT //600

#define PHOTOS_TTL_X  0
#define PHOTOS_TTL_Y  0
#define PHOTOS_TTL_W  PHOTOS_CONT_W
#define PHOTOS_TTL_H  60

#define PHOTOS_PREW_CONTX  0
#define PHOTOS_PREW_CONTY  (PHOTOS_TTL_Y + PHOTOS_TTL_H)
#define PHOTOS_PREW_CONTW  PHOTOS_CONT_W
#define PHOTOS_PREW_CONTH  (PHOTOS_CONT_H - PHOTOS_PREW_CONTY)

#define PHOTOS_ITEM_X  60
#define PHOTOS_ITEM_Y  10
#define PHOTOS_ITEM_W  180
#define PHOTOS_ITEM_H  160

#define PHOTOS_PREW_X  0
#define PHOTOS_PREW_Y  0
#define PHOTOS_PREW_W  PHOTOS_ITEM_W
#define PHOTOS_PREW_H  120

#define PHOTOS_TEXT_X  0
#define PHOTOS_TEXT_Y  (PHOTOS_PREW_Y + PHOTOS_PREW_H)
#define PHOTOS_TEXT_W  PHOTOS_ITEM_W
#define PHOTOS_TEXT_H  (PHOTOS_ITEM_H - PHOTOS_TEXT_Y)

#define PHOTOS_ITEM_VGAP 20
#define PHOTOS_ITEM_HGAP 40

#define PHOTOS_PAGE     (PHOTOS_COL * PHOTOS_ROW)
#define PHOTOS_COL 4
#define PHOTOS_ROW 3

u16 photos_menu_keymap(u16 key);
RET_CODE photos_menu_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 photos_prew_cont_keymap(u16 key);
RET_CODE photos_prew_cont_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

static u32 g_photos_show = 0;
static u8 g_album_id[ALBUMS_ID_LEN] = {0};
static u8 g_user_id[ALBUMS_ID_LEN] = {0};

static void photos_set_data(control_t *p_item, photo_info_t *p_info)
{
  control_t *p_text = NULL;

  if(p_info == NULL)
  {
    return;
  }

  ctrl_set_sts(p_item, OBJ_STS_SHOW);

  p_text = ctrl_get_child_by_id(p_item, IDC_PHOTOS_ITEM_TEXT);

  text_set_content_by_ascstr(p_text, (u8 *)p_info->p_photo_name);

  ctrl_paint_ctrl(p_item, TRUE);
}

static void photos_clear_all(control_t *p_menu)
{
  control_t *p_pcont = NULL, *p_item = NULL;
  control_t *p_text = NULL;
  u16 i = 0;

  p_pcont = ctrl_get_child_by_id(p_menu, IDC_PHOTOS_PREW_CONT);
  
  for(i = IDC_PHOTOS_ITEM1; i <= IDC_PHOTOS_ITEM12; i++)
  {
    p_item = ctrl_get_child_by_id(p_pcont, i);

    ctrl_set_sts(p_item, OBJ_STS_HIDE);
    
    p_text = ctrl_get_child_by_id(p_item, IDC_PHOTOS_ITEM_TEXT);

    text_set_content_by_ascstr(p_text, (u8 *)" ");
  }
  
  ctrl_paint_ctrl(p_pcont, TRUE);

  ui_pic_clear_all(0);
}

RET_CODE open_photos(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_ttl = NULL, *p_prew_cont = NULL;
  control_t *p_item[PHOTOS_PAGE], *p_prew = NULL, *p_text = NULL;
  u16 i = 0;
  albums_req_photos_t photos_req;
  albums_t *p_albums = NULL;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  p_albums = ui_albums_get_albums_list();

  if(p_albums == NULL)
  {
    return ERR_FAILURE;
  }
  
  if(para1 > p_albums->album_total)
  {
    return ERR_FAILURE;
  }
  
  if(p_albums->p_album == NULL)
  {
    return ERR_FAILURE;
  }

  memcpy(g_album_id, p_albums->p_album[para1].p_album_id, strlen(p_albums->p_album[para1].p_album_id));
  memcpy(g_user_id, p_albums->p_album[para1].p_user_id, strlen(p_albums->p_album[para1].p_user_id));
  
  //create menu.
  p_menu = fw_create_mainwin(ROOT_ID_PHOTOS,
    PHOTOS_CONT_X, PHOTOS_CONT_Y, PHOTOS_CONT_W, PHOTOS_CONT_H, 0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_menu != NULL);
  ctrl_set_keymap(p_menu, photos_menu_keymap);
  ctrl_set_proc(p_menu, photos_menu_proc);
  ctrl_set_rstyle(p_menu, RSI_WINDOW_1, RSI_WINDOW_1, RSI_WINDOW_1);

  p_ttl = ctrl_create_ctrl(CTRL_TEXT, IDC_PHOTOS_TTL,
    PHOTOS_TTL_X, PHOTOS_TTL_Y, PHOTOS_TTL_W, PHOTOS_TTL_H, p_menu, 0);
  ctrl_set_rstyle(p_ttl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_ttl, FSI_WHITE_S, FSI_WHITE_S, FSI_WHITE_S);
  text_set_content_type(p_ttl, TEXT_STRTYPE_UNICODE);  
  text_set_content_by_ascstr(p_ttl, (u8 *)"Picasa");

  p_prew_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PHOTOS_PREW_CONT,
    PHOTOS_PREW_CONTX, PHOTOS_PREW_CONTY, PHOTOS_PREW_CONTW, PHOTOS_PREW_CONTH, p_menu, 0);
  ctrl_set_rstyle(p_prew_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_prew_cont, photos_prew_cont_keymap);
  ctrl_set_proc(p_prew_cont, photos_prew_cont_proc);

  for(i = 0; i < PHOTOS_PAGE; i++)
  {
    p_item[i] = ctrl_create_ctrl(CTRL_CONT, IDC_PHOTOS_ITEM1 + i,
      PHOTOS_ITEM_X + i % PHOTOS_COL * (PHOTOS_ITEM_W + PHOTOS_ITEM_HGAP),
      (u16)(PHOTOS_ITEM_Y + i / PHOTOS_COL * (PHOTOS_ITEM_H + PHOTOS_ITEM_VGAP)),
      PHOTOS_ITEM_W,
      PHOTOS_ITEM_H,
      p_prew_cont, 0);
    //ctrl_set_rstyle(p_item[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_style(p_item[i], STL_EX_WHOLE_HL);
    ctrl_set_sts(p_item[i], OBJ_STS_HIDE);
  
    p_prew = ctrl_create_ctrl(CTRL_TEXT, IDC_PHOTOS_ITEM_PREW,
      PHOTOS_PREW_X, PHOTOS_PREW_Y, PHOTOS_PREW_W, PHOTOS_PREW_H, p_item[i], 0);
    ctrl_set_rstyle(p_prew, RSI_ITEM_12_SH_TRANS, RSI_ITEM_12_HL_TRANS, RSI_ITEM_12_HL_TRANS);
    text_set_align_type(p_prew, STL_LEFT |STL_VCENTER);
    text_set_font_style(p_prew, FSI_WHITE_S, FSI_WHITE_S, FSI_WHITE_S);
    text_set_content_type(p_prew, TEXT_STRTYPE_UNICODE); 

    p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_PHOTOS_ITEM_TEXT,
      PHOTOS_TEXT_X, PHOTOS_TEXT_Y, PHOTOS_TEXT_W, PHOTOS_TEXT_H, p_item[i], 0);
    //ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_text, FSI_WHITE_S, FSI_WHITE_S, FSI_WHITE_S);
    text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);  

    ctrl_set_related_id(p_item[i],
      (i % PHOTOS_COL == 0) ? (i + PHOTOS_COL): (i),
      (i < PHOTOS_COL) ? (i + PHOTOS_PAGE - PHOTOS_COL + 1) : (i - PHOTOS_COL + 1),
      (u16)(((i + 1) % PHOTOS_COL == 0) ? (i / PHOTOS_COL * PHOTOS_COL + 1) : (i + 2)),
      (i >= (PHOTOS_PAGE - PHOTOS_COL)) ? (i % PHOTOS_COL + 1): (i + PHOTOS_COL + 1));
  }  

  ctrl_default_proc(p_item[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  photos_req.request_type = PHOTO_BY_ALBUM;
  photos_req.page = 1;
  photos_req.per_page = PHOTOS_PAGE;
  
  memcpy(photos_req.user_id, g_user_id, ALBUMS_ID_LEN - 1);
  memcpy(photos_req.album_id, g_album_id, ALBUMS_ID_LEN - 1);
  memcpy(photos_req.community, "milan",6);// ALBUMS_ID_LEN - 1);
  
  ui_albums_get_photos(&photos_req);

  return SUCCESS;
}

static RET_CODE photos_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  pic_stop();

  ui_pic_clear_all(0);
  
  if(fw_find_root_by_id(ROOT_ID_ALBUMS))
  {
  fw_notify_root(fw_find_root_by_id(ROOT_ID_ALBUMS), NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  return ERR_NOFEATURE;
}

static RET_CODE photos_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  mul_pic_param_t pic_param = {{0},};
  photos_t *p_photos = NULL;
  u16 thumb_idx = 0;

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }
  MT_ASSERT(g_photos_show < p_photos->photo_total);

  //request picture.
  thumb_idx = p_photos->p_photo[g_photos_show].thumb_num - 1;
  
  ui_pic_set_url((u8 *)p_photos->p_photo[g_photos_show].p_thumb[thumb_idx].p_thumb_url);

  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = PHOTOS_PREW_X;
  pic_param.win_rect.top = PHOTOS_PREW_Y;
  pic_param.win_rect.right = (pic_param.win_rect.left + PHOTOS_PREW_W);
  pic_param.win_rect.bottom = (pic_param.win_rect.top + PHOTOS_PREW_H);
  pic_param.file_size = 0;
  pic_param.handle = 0;

  offset_rect(&pic_param.win_rect, PHOTOS_ITEM_X, PHOTOS_ITEM_Y);
  offset_rect(&pic_param.win_rect, PHOTOS_PREW_CONTX, PHOTOS_PREW_CONTY);
  offset_rect(&pic_param.win_rect, PHOTOS_CONT_X, PHOTOS_CONT_Y);

  pic_start(&pic_param);  
  
  return SUCCESS;
}

static RET_CODE photos_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_prew_cont = NULL;
  control_t *p_item = NULL;
  photos_t *p_photos = NULL;
  mul_pic_param_t pic_param = {{0},};
  u16 thumb_idx = 0;

  if(fw_get_focus_id() != ROOT_ID_PHOTOS)
  {
    return ERR_FAILURE;
  }

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(para1 > p_photos->cnt)
  {
    return ERR_FAILURE;
  }

  p_prew_cont = ctrl_get_child_by_id(p_cont, IDC_PHOTOS_PREW_CONT);

  p_item = ctrl_get_child_by_id(p_prew_cont, (u16)(IDC_PHOTOS_ITEM1 + para1));

  photos_set_data(p_item, &p_photos->p_photo[para1]);

  if((para1 + 1) >= p_photos->cnt)
  {
    return ERR_FAILURE;
  }

  thumb_idx = p_photos->p_photo[para1 + 1].thumb_num - 1;
  
  ui_pic_set_url((u8 *)p_photos->p_photo[para1 + 1].p_thumb[thumb_idx].p_thumb_url);

  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = PHOTOS_PREW_X;
  pic_param.win_rect.top = PHOTOS_PREW_Y;
  pic_param.win_rect.right = (pic_param.win_rect.left + PHOTOS_PREW_W);
  pic_param.win_rect.bottom = (pic_param.win_rect.top + PHOTOS_PREW_H);
  pic_param.file_size = 0;
  pic_param.handle = para1 + 1;

  offset_rect(&pic_param.win_rect,
    PHOTOS_ITEM_X + (para1 + 1) % PHOTOS_COL * (PHOTOS_ITEM_W + PHOTOS_ITEM_HGAP),
    (s16)(PHOTOS_ITEM_Y + (para1 + 1) / PHOTOS_COL * (PHOTOS_ITEM_H + PHOTOS_ITEM_VGAP)));
  offset_rect(&pic_param.win_rect, PHOTOS_PREW_CONTX, PHOTOS_PREW_CONTY);
  offset_rect(&pic_param.win_rect, PHOTOS_CONT_X, PHOTOS_CONT_Y);
  
  pic_start(&pic_param);  

  return SUCCESS;
}

static RET_CODE photos_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  return SUCCESS;
}

static RET_CODE photos_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  photos_t *p_photos = NULL;
  mul_pic_param_t pic_param = {{0},};
  u16 thumb_idx = 0;

  if(fw_get_focus_id() != ROOT_ID_PHOTOS)
  {
    return ERR_FAILURE;
  }  

  if(para1 != ALBUMS_EVT_PHOTOS_LIST)
  {
    return ERR_FAILURE;
  }
  
  p_photos = (photos_t *)para2;

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_photos->cnt == 0)
  {
    return ERR_FAILURE;
  }
  
  //request picture.
  thumb_idx = p_photos->p_photo[0].thumb_num - 1;
  
  ui_pic_set_url((u8 *)p_photos->p_photo[0].p_thumb[thumb_idx].p_thumb_url);
  
  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = PHOTOS_PREW_X;
  pic_param.win_rect.top = PHOTOS_PREW_Y;
  pic_param.win_rect.right = (pic_param.win_rect.left + PHOTOS_PREW_W);
  pic_param.win_rect.bottom = (pic_param.win_rect.top + PHOTOS_PREW_H);
  pic_param.file_size = 0;
  pic_param.handle = 0;

  offset_rect(&pic_param.win_rect, PHOTOS_ITEM_X, PHOTOS_ITEM_Y);
  offset_rect(&pic_param.win_rect, PHOTOS_PREW_CONTX, PHOTOS_PREW_CONTY);
  offset_rect(&pic_param.win_rect, PHOTOS_CONT_X, PHOTOS_CONT_Y);

  pic_start(&pic_param);    

  return SUCCESS;
}

static RET_CODE photos_cont_focus_up(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  albums_req_photos_t photos_req;
  control_t *p_active = NULL;
  photos_t *p_photos = NULL;
  u8 active_id = 0;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  p_active = ctrl_get_active_ctrl(p_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }  

  if((active_id <= PHOTOS_COL) && (g_photos_show >= PHOTOS_PAGE))
  {
    pic_stop();

    photos_clear_all(ctrl_get_parent(p_cont));
  
    //recaculate current show.
    g_photos_show -= PHOTOS_PAGE;

    photos_req.request_type = PHOTO_BY_ALBUM;
    photos_req.page = (u16)((g_photos_show / PHOTOS_PAGE) + 1);
    photos_req.per_page = PHOTOS_PAGE;
    
    memcpy(photos_req.user_id, "zhengwu1008", 12);//ALBUMS_ID_LEN - 1);
    
    ui_albums_get_photos(&photos_req);
    
    return SUCCESS;
  }
  
  return ERR_NOFEATURE;
}

static RET_CODE photos_cont_focus_down(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  albums_req_photos_t photos_req;
  control_t *p_active = NULL;
  u8 active_id = 0;
  photos_t *p_photos = NULL;  
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  
  p_active = ctrl_get_active_ctrl(p_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(active_id > (PHOTOS_PAGE - PHOTOS_COL))
  {
    if((g_photos_show + PHOTOS_PAGE) >= p_photos->photo_total)
    {
      return ERR_FAILURE;
    }  
    
    pic_stop();

    photos_clear_all(ctrl_get_parent(p_cont));
    
    //recaculate current show.
    g_photos_show += PHOTOS_PAGE;

    photos_req.request_type = PHOTO_BY_ALBUM;
    photos_req.page = (u16)((g_photos_show / PHOTOS_PAGE) + 1);
    photos_req.per_page = PHOTOS_PAGE;
    
    memcpy(photos_req.user_id, g_user_id, ALBUMS_ID_LEN - 1);
    memcpy(photos_req.album_id, g_album_id, ALBUMS_ID_LEN - 1);   
    
    ui_albums_get_photos(&photos_req);  

    return SUCCESS;
  }

  return ERR_NOFEATURE;
}

static RET_CODE photos_select(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  control_t *p_active = NULL;
  u8 active_id = 0;
  photos_t *p_photos = NULL;  
  
  p_active = ctrl_get_active_ctrl(p_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(active_id + g_photos_show > p_photos->photo_total)
  {
    return ERR_FAILURE;
  }

  pic_stop();
  
  ui_pic_clear_all(0);

  return manage_open_menu(ROOT_ID_PHOTO_SHOW, (active_id - IDC_PHOTOS_ITEM1), FALSE);
}

static RET_CODE photos_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  control_t *p_active = NULL;
  u8 active_id = 0;
  photos_t *p_photos = NULL;
  albums_req_comments_t comments_req;
  albums_t *p_albums = NULL;

  memset(&comments_req,0,sizeof(albums_req_comments_t));
  
  p_active = ctrl_get_active_ctrl(p_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);

  p_photos = ui_albums_get_photos_list();

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(active_id + g_photos_show > p_photos->photo_total)
  {
    return ERR_FAILURE;
  }

  if(p_photos->p_photo == NULL)
  {
    return ERR_FAILURE;
  }

  if((active_id - IDC_PHOTOS_ITEM1 + 1) > p_photos->cnt)
  {
    return ERR_FAILURE;
  }

  p_albums = ui_albums_get_albums_list();

  if(p_albums == NULL)
  {
    return ERR_FAILURE;
  }  

  comments_req.request_type = COMMENT_BY_ALBUM;
	memcpy(comments_req.user_id,
	  p_albums->p_album[0].p_user_id,
	  strlen(p_albums->p_album[0].p_user_id));
	memcpy(comments_req.album_id,
	  p_photos->p_photo[active_id - IDC_PHOTOS_ITEM1].p_album_id,
	  strlen(p_photos->p_photo[active_id - IDC_PHOTOS_ITEM1].p_album_id));
	memcpy(comments_req.photo_id, 
	  p_photos->p_photo[active_id - IDC_PHOTOS_ITEM1].p_photo_id,
	  strlen(p_photos->p_photo[active_id - IDC_PHOTOS_ITEM1].p_photo_id));
	  
  ui_albums_get_comments(&comments_req);

  return SUCCESS;
}

BEGIN_KEYMAP(photos_menu_keymap, ui_comm_root_keymap)

END_KEYMAP(photos_menu_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(photos_menu_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, photos_on_destory)
  ON_COMMAND(MSG_UPDATE, photos_update)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, photos_draw_end)
  ON_COMMAND(MSG_ALBUMS_EVT_FAILED, photos_failed)
  ON_COMMAND(MSG_ALBUMS_EVT_SUCCESS, photos_success)
END_MSGPROC(photos_menu_proc, ui_comm_root_proc)

BEGIN_KEYMAP(photos_prew_cont_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(photos_prew_cont_keymap, NULL)

BEGIN_MSGPROC(photos_prew_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, photos_cont_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, photos_cont_focus_down)
  ON_COMMAND(MSG_SELECT, photos_select)
  ON_COMMAND(MSG_INFO, photos_info)
END_MSGPROC(photos_prew_cont_proc, cont_class_proc)

