/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "albums_filter.h"
#include "albums_public.h"
#include "ui_albums.h"
#include "ui_albums_api.h"
#include "ui_picture_api.h"
#include "ui_edit_city.h"
#include "ui_keyboard_v2.h"
#include "ui_gui_defines.h"

typedef enum
{
  IDC_ALBUMS_TTL = 1,
  IDC_ALBUMS_CONT,
  IDC_ALBUMS_PAGE,
  IDC_ALBUMS_TYPE_LIST_CONT,
  IDC_ALBUMS_TYPE,
  IDC_ALBUMS_PREW_CONT,
  IDC_ALBUMS_FRIEND_LIST,
  IDC_ALBUMS_BTM_HELP,
}albums_id_t;
typedef enum
{
  IDC_ALBUMS_ITEM1 = 1,
  IDC_ALBUMS_ITEM2,
  IDC_ALBUMS_ITEM3,
  IDC_ALBUMS_ITEM4,
  IDC_ALBUMS_ITEM5,
  IDC_ALBUMS_ITEM6,
  IDC_ALBUMS_ITEM7,
  IDC_ALBUMS_ITEM8,
  IDC_ALBUMS_ITEM9,
  IDC_ALBUMS_ITEM10,
  IDC_ALBUMS_ITEM11,
  IDC_ALBUMS_ITEM12,  
}albums_item_cont_t;
typedef enum
{
  IDC_ALBUMS_ITEM_PREW = 1,
  IDC_ALBUMS_ITEM_TEXT,
}albums_item_t;
enum local_msg
{
  MSG_GREEN= MSG_LOCAL_BEGIN + 300,
  MSG_BLUE,
  MSG_RED,
  MSG_YELLOW,
};
#define ALBUMS_CONT_X  0//40
#define ALBUMS_CONT_Y  0//10
#define ALBUMS_CONT_W  SCREEN_WIDTH //960
#define ALBUMS_CONT_H  SCREEN_HEIGHT //600
#define ALBUMS_TTL_X  40
#define ALBUMS_TTL_Y  10
#define ALBUMS_TTL_W  160
#define ALBUMS_TTL_H  80
#define ALBUMS_PAGE_CONT_X  330//((ALBUMS_CONT_W - ALBUMS_TTL_W - ALBUMS_PAGE_CONT_W) / 2 + 170 + 40)
#define ALBUMS_PAGE_CONT_Y  100//((ALBUMS_TTL_H - ALBUMS_PAGE_CONT_H) / 2 + 15)
#define ALBUMS_PAGE_CONT_W 830//500
#define ALBUMS_PAGE_CONT_H 50//32
#define ALBUMS_PAGE_ARROWL_X       150
#define ALBUMS_PAGE_ARROWL_Y       7
#define ALBUMS_PAGE_ARROWL_W      36
#define ALBUMS_PAGE_ARROWL_H       36
#define ALBUMS_PAGE_ARROWR_X       (ALBUMS_PAGE_CONT_W - ALBUMS_PAGE_ARROWL_X - ALBUMS_PAGE_ARROWR_W)
#define ALBUMS_PAGE_ARROWR_Y       7
#define ALBUMS_PAGE_ARROWR_W      36
#define ALBUMS_PAGE_ARROWR_H       36
#define ALBUMS_PAGE_X  450
#define ALBUMS_PAGE_Y 7
#define ALBUMS_PAGE_W 100
#define ALBUMS_PAGE_H 36
#define ALBUMS_PAGE1_X  250
#define ALBUMS_PAGE1_Y 7
#define ALBUMS_PAGE1_W 100
#define ALBUMS_PAGE1_H 36
#define ALBUMS_TYPE_LIST_CONT_X   120
#define ALBUMS_TYPE_LIST_CONT_Y   100
#define ALBUMS_TYPE_LIST_CONT_W  200
#define ALBUMS_TYPE_LIST_CONT_H   570
#define ALBUMS_TYPE_PIC_X   0
#define ALBUMS_TYPE_PIC_Y   0
#define ALBUMS_TYPE_PIC_W  200
#define ALBUMS_TYPE_PIC_H   90
#define ALBUMS_TYPE_X   20
#define ALBUMS_TYPE_Y   90
#define ALBUMS_TYPE_W   160
#define ALBUMS_TYPE_H   200
#define ALBUMS_PREW_CONTX  360//(ALBUMS_TTL_X + ALBUMS_TTL_W)
#define ALBUMS_PREW_CONTY  160//(ALBUMS_TTL_Y + ALBUMS_TTL_H)
#define ALBUMS_PREW_CONTW  830//(ALBUMS_CONT_W - ALBUMS_PREW_CONTX)
#define ALBUMS_PREW_CONTH  (ALBUMS_ITEM_H*3+ALBUMS_ITEM_VGAP * 2) //500//480 + ALBUMS_ITEM_VGAP * 2
#define ALBUMS_ITEM_X  0//22
#define ALBUMS_ITEM_Y  0
#define ALBUMS_ITEM_W  180
#define ALBUMS_ITEM_H  150
#define ALBUMS_HELP_X ((SCREEN_WIDTH - ALBUMS_HELP_W)/2)//260
#define ALBUMS_HELP_TOP_OFFSET (-80)
#define ALBUMS_HELP_W  750//800
#define ALBUMS_HELP_BOTTOM_OFFSET 50
#define ALBUMS_HELP_STR_OFFSET 42
#define ALBUMS_PREW_X  0
#define ALBUMS_PREW_Y  0
#define ALBUMS_PREW_W  ALBUMS_ITEM_W
#define ALBUMS_PREW_H  120
#define ALBUMS_TEXT_X  0
#define ALBUMS_TEXT_Y  (ALBUMS_PREW_Y + ALBUMS_PREW_H)
#define ALBUMS_TEXT_W  ALBUMS_ITEM_W
#define ALBUMS_TEXT_H  (ALBUMS_ITEM_H - ALBUMS_TEXT_Y)
#define ALBUMS_BOTTOM_HELP_X  330
#define ALBUMS_BOTTOM_HELP_Y  620
#define ALBUMS_BOTTOM_HELP_W  830
#define ALBUMS_BOTTOM_HELP_H  50
#define ALBUMS_ITEM_VGAP 0
#define ALBUMS_ITEM_HGAP 15
#define ALBUMS_PAGE     (ALBUMS_COL * ALBUMS_ROW)
#define ALBUMS_COL 4
#define ALBUMS_ROW 3
#define ALBUMS_TYPE_TOT				ALBUMS_TYPE_PAGE
#define ALBUMS_TYPE_PAGE       4
#define ALBUMS_TYPE_FCNT       1
#define ALBUMS_TYPE_VGAP		10
#define ALBUMS_FRIEND_PAGE       4
#define ALBUMS_FRIEND_FCNT       3
#define ALBUMS_FRIEND_VGAP 6
typedef enum
{
  ALBUMS_T_FEATURE = 0,
  ALBUMS_T_ALBUMS,
  ALBUMS_T_SEARCH,
  ALBUMS_T_FRIENDS,
  ALBUMS_T_ALBUMS_PHOTOS,
  ALBUMS_T_FRIENDS_ALBUMS,
  ALBUMS_T_FRIENDS_PHOTOS,
}albums_type_t;
typedef struct
{
    control_t *p_menu;
    control_t *p_list;
    control_t *p_icon_title;
    control_t *p_page_cont;
    control_t *p_prev_cont;
    control_t *p_help_cont;
    control_t *p_friend_list;
}albums_ctrl_t;
static list_xstyle_t type_irstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_2_HL,
    RSI_ITEM_2_SH,
    RSI_ITEM_2_HL,
  };
  static list_xstyle_t type_frstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  static list_xstyle_t type_ffstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
  };
  static list_xstyle_t friend_irstyle =
  {
	  RSI_IGNORE,
	  RSI_IGNORE,
	  RSI_ITEM_12_HL_TRANS,
	  RSI_IGNORE,
	  RSI_IGNORE,
  };
  static list_xstyle_t friend_frstyle =
  {
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };
  static list_xstyle_t friend_ffstyle =
  {
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
  };
  static list_field_attr2_t friend_attr[ALBUMS_FRIEND_FCNT] =
  {
    {LISTFIELD_TYPE_UNISTR, 
      100, 100, 0, 0, &friend_irstyle, &friend_ffstyle},
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
     620, 50, 110, 0, &friend_frstyle, &friend_ffstyle},
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
     620, 50, 110, 50, &friend_frstyle, &friend_ffstyle},
  };  
extern void pic_play_update_slide_show(BOOL is_net);

#define MAX_SEARCH_PICTURE_NAME_LEN     (16)
#define MAX_PICTURE_NAME_LEN     (32)

static albums_friends_t g_albums_friends = {0};
static albums_ctrl_t g_ctrls = {NULL};

static BOOL add_friend(u8 *p_user_id, albums_site_t site)
{
  u32 i = 0;
  u16 str_len = 0;
  comm_dlg_data_t dlg_data_full =
  {
    ROOT_ID_ALBUMS,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    300, 190,
    400, 160,
    IDS_FRIEND_LIST_FULL,
    500,
    FALSE,
    0,
    {0},
  };
  comm_dlg_data_t dlg_add_friend =
  {
    ROOT_ID_ALBUMS,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    300, 190,
    400, 160,
    IDS_ADD_FRIEND_SUCCESSED,
    500,
    FALSE,
    0,
    {0},
  };
  comm_dlg_data_t dlg_friend_exist =
  {
    ROOT_ID_ALBUMS,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    300, 190,
    400, 160,
    IDS_FRIEND_EXISTS,
    500,
    FALSE,
    0,
    {0},
  };
  MT_ASSERT(p_user_id != NULL);

  if(g_albums_friends.friends_num == MAX_FRIENDS_NUM)
  {
    ui_comm_dlg_open(&dlg_data_full);
    return FALSE;
  }

  //check duplicate.
  for(i = 0; i < g_albums_friends.friends_num; i++)
  {
    if(g_albums_friends.albums_friend[i].is_used &&
      (g_albums_friends.albums_friend[i].site == site) &&
      (strlen((s8 *)p_user_id) == strlen((s8 *)g_albums_friends.albums_friend[i].user_id)) &&
      (memcmp(p_user_id, g_albums_friends.albums_friend[i].user_id, strlen((s8 *)p_user_id)) == 0))
    {
      ui_comm_dlg_open(&dlg_friend_exist);
      return FALSE;
    }
  }  

  //add new friend.
  g_albums_friends.albums_friend[g_albums_friends.friends_num].is_used = TRUE;
  g_albums_friends.albums_friend[g_albums_friends.friends_num].site = site;
    if(p_user_id != NULL)
    {
        str_len = (u16)strlen((s8 *)p_user_id);
        str_len = (str_len >= ALBUMS_ID_LENHGT) ? (ALBUMS_ID_LENHGT - 1) : str_len;
        memcpy(g_albums_friends.albums_friend[g_albums_friends.friends_num].user_id, p_user_id, str_len);
        g_albums_friends.albums_friend[g_albums_friends.friends_num].user_id[str_len]  = '\0';
    }

  g_albums_friends.friends_num++;
  
  sys_status_set_albums_friends_info(&g_albums_friends);
  ui_comm_dlg_open(&dlg_add_friend);
  return TRUE;
}

static BOOL del_friend(u8 index)
{
  u8 i = 0;
  
  if(index >= MAX_FRIENDS_NUM)
  {
    return FALSE;
  }

  if(g_albums_friends.albums_friend[index].is_used)
  {
    g_albums_friends.albums_friend[index].is_used = FALSE;
    
    g_albums_friends.friends_num--;

    for( i = index; i < MAX_FRIENDS_NUM-1; i++)
    {
      g_albums_friends.albums_friend[i].is_used = g_albums_friends.albums_friend[i + 1].is_used;
      g_albums_friends.albums_friend[i].site = g_albums_friends.albums_friend[i + 1].site;
      memset(g_albums_friends.albums_friend[i].user_id, 0, sizeof(g_albums_friends.albums_friend[i].user_id));
      strcpy((s8 *)g_albums_friends.albums_friend[i].user_id, (s8 *)g_albums_friends.albums_friend[i + 1].user_id);
    }

    return TRUE;
  }

  return FALSE;
}





//cont







//ui_comm_help_move_pos(p_menu, 260, -80, 800, 50, 42);






//#define ALBUMS_COL 4







static albums_site_t g_albums_site = ALBUMS_YUPOO;
static albums_type_t g_albums_type = ALBUMS_T_FEATURE;
static u32 g_albums_show = 0;
static u8 g_albums_community[ALBUMS_ID_LEN] = {0};
static u8 g_friend_focus = 0;
static u8 g_focus_album = 0;
u16 unistr[MAX_SEARCH_PICTURE_NAME_LEN] = {0};

static comm_help_data_t help_data= //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_SWITCH_WEBSITE,
   IDS_SETUP},
  {IM_EPG_COLORBUTTON_RED,
   IM_EPG_COLORBUTTON_BLUE},
};

static comm_help_data_t help_data1= //help bar data
{
  4,                                    //select  + scroll page + exit
  4,
  {IDS_SWITCH_WEBSITE,
   IDS_ADD_FRIEND_PICTURE,
   IDS_INFO,
   IDS_SETUP},
  {IM_EPG_COLORBUTTON_RED,
   IM_EPG_COLORBUTTON_GREEN,
   IM_EPG_COLORBUTTON_YELLOW,
   IM_EPG_COLORBUTTON_BLUE},
};
static comm_help_data_t help_data2= //help bar data
{
  4,                                    //select  + scroll page + exit
  4,
  {IDS_SWITCH_WEBSITE,
   IDS_DELETE,
   IDS_DELETE_ALL,
   IDS_SETUP},
  {IM_EPG_COLORBUTTON_RED,
   IM_EPG_COLORBUTTON_GREEN,
   IM_EPG_COLORBUTTON_YELLOW,
   IM_EPG_COLORBUTTON_BLUE},
};

u16 albums_menu_keymap(u16 key);
RET_CODE albums_menu_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 albums_type_keymap(u16 key);
RET_CODE albums_type_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 albums_prew_cont_keymap(u16 key);
RET_CODE albums_prew_cont_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 albums_friend_keymap(u16 key);
RET_CODE albums_friend_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


extern iconv_t g_cd_utf8_to_utf16le;

static void albums_help_update(control_t *p_parent, comm_help_data_t *p_data)
{
  control_t *p_cont = NULL, *p_mbox = NULL;
  u8 i, row;

  
  if((p_cont = ctrl_get_child_by_id(g_ctrls.p_help_cont, IDC_COMM_HELP_CONT)) == NULL)
  {
    return;
  }
  if((p_mbox = ctrl_get_child_by_id(p_cont, IDC_COMM_HELP_MBOX)) == NULL)
  {
    return;
  }
  row = (p_data->item_cnt / p_data->col_cnt) + ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
}

static void exit_album(void)
{  
    ctrl_process_msg(g_ctrls.p_menu, MSG_EXIT, 0, 0);
}

static void albums_set_page(control_t *p_menu)
{
  control_t *p_page = NULL, *p_friend = NULL;
  albums_t *p_albums = NULL;
  photos_t *p_photos = NULL;
  u8 asc_str[16]= {0};
  u16 focus = 0;
  p_page = ctrl_get_child_by_id(g_ctrls.p_page_cont, IDC_ALBUMS_PAGE);
  p_friend = g_ctrls.p_friend_list;

  switch(g_albums_type)
  {
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_PHOTOS:
     p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return;
      }
      sprintf((s8 *)asc_str, "%d/%d",
        p_photos->page, (int)((p_photos->photo_total + ALBUMS_PAGE - 1) / ALBUMS_PAGE));

      text_set_content_by_ascstr(p_page, asc_str);
      
      ctrl_paint_ctrl(p_page, TRUE);
      break;

    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return;
      }
      sprintf((s8 *)asc_str, "%d/%d", (int)(g_albums_show / ALBUMS_PAGE + 1), (int)(p_albums->album_total / ALBUMS_PAGE + 1));

      text_set_content_by_ascstr(p_page, asc_str);
      ctrl_paint_ctrl(p_page, TRUE);      
      break;

    case ALBUMS_T_FRIENDS:
      focus = list_get_focus_pos(p_friend);
    
      if(g_albums_friends.friends_num == 0)
      {
        sprintf((s8 *)asc_str, "%d/%d", 0, 0);
      }
      else
      {
        sprintf((s8 *)asc_str, "%d/%d",
          (int)(focus / ALBUMS_FRIEND_PAGE + 1),
          (int)((g_albums_friends.friends_num + ALBUMS_FRIEND_PAGE - 1) / ALBUMS_FRIEND_PAGE));
      }
      text_set_content_by_ascstr(p_page, asc_str);
      ctrl_paint_ctrl(p_page, TRUE);         
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

static void albums_set_photos_data(control_t *p_item, photo_info_t *p_info, u16 msg)
{
  control_t *p_text = NULL, *p_prew = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u16 uni_str[MAX_PICTURE_NAME_LEN];
  
  if(p_info == NULL)
  {
    return;
  }

  ctrl_set_sts(p_item, OBJ_STS_SHOW);

  p_text = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_TEXT);
  
  inbuf = p_info->p_photo_name;
  outbuf = (char*) uni_str;
  src_len = strlen(inbuf) + 1;
  dest_len = sizeof(uni_str);
  iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

  text_set_content_by_unistr(p_text, uni_str);

  p_prew = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_PREW);
  switch(msg)
  {
    case MSG_PIC_EVT_TOO_LARGE:
      text_set_content_by_ascstr(p_prew, (u8 *)"too large");
      break;

    case MSG_PIC_EVT_UNSUPPORT:
      text_set_content_by_ascstr(p_prew, (u8 *)"unsupport");
      break;

    case MSG_PIC_EVT_DATA_ERR:
      text_set_content_by_ascstr(p_prew, (u8 *)"data error");
      break;

    default:
      break;
  }

  ctrl_paint_ctrl(p_item, TRUE);
}

static void albums_set_albums_data(control_t *p_item, album_info_t *p_info)
{
  control_t *p_text = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u16 uni_str[MAX_PICTURE_NAME_LEN];
  if(p_info == NULL)
  {
    return;
  }

  ctrl_set_sts(p_item, OBJ_STS_SHOW);
  p_text = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_TEXT);
 // p_text = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_TEXT);
  inbuf = p_info->p_album_name;
  outbuf = (char*) uni_str;
  src_len = strlen(inbuf) + 1;
  dest_len = sizeof(uni_str);
  iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
  text_set_content_by_unistr(p_text, uni_str);
  ctrl_paint_ctrl(p_item, TRUE);
}

static void albums_clear_all(control_t *p_menu)
{
  control_t *p_item = NULL, *p_page = NULL, *p_prew = NULL;
  control_t *p_text = NULL;
  u16 i = 0;

  
  for(i = IDC_ALBUMS_ITEM1; i <= IDC_ALBUMS_ITEM12; i++)
  {
    p_item = ctrl_get_child_by_id(g_ctrls.p_prev_cont, (u8)i);

    ctrl_set_sts(p_item, OBJ_STS_HIDE);
    
    p_text = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_TEXT);

    text_set_content_by_ascstr(p_text, (u8 *)" ");

    p_prew = ctrl_get_child_by_id(p_item, IDC_ALBUMS_ITEM_PREW);

    text_set_content_by_ascstr(p_prew, (u8 *)" ");
  }
  ctrl_paint_ctrl(g_ctrls.p_prev_cont, TRUE);    

  ui_pic_clear_all(0);
  p_page = ctrl_get_child_by_id(g_ctrls.p_page_cont, IDC_ALBUMS_PAGE);

  text_set_content_by_ascstr(p_page, (u8 *)" ");

  ctrl_paint_ctrl(p_page, TRUE);
}

static RET_CODE albums_type_list_update(control_t *p_list, u16 start, u16 size, 
                                      u32 context)
{
  u16 i = 0, pos = 0, cnt = list_get_count(p_list);
  u8 asc_str[ALBUMS_TYPE_PAGE][16] = 
  {
    "Featured",
    "View Albums",
    "Search",
    "Friends",
  };

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str[start + i]); 
    }
  }
  return SUCCESS;
}

static RET_CODE albums_friend_list_update(control_t *p_list, u16 start, u16 size, 
                                      u32 context)
{
  u16 i = 0, pos = 0, cnt = list_get_count(p_list);
  u8 number[16] = {0};
  u8 asc_str[3][16] = 
  {
    "Picasa",
    "Flickr",
    "Yupoo",
  };  
  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      sprintf((s8 *)number, "%d ", pos +1);
      list_set_field_content_by_ascstr(p_list, pos, 0, number); 
      list_set_field_content_by_ascstr(p_list,
        pos, 1, g_albums_friends.albums_friend[pos].user_id); 
      list_set_field_content_by_ascstr(p_list,
        pos, 2, asc_str[g_albums_friends.albums_friend[pos].site]); 
    }
  }
  return SUCCESS;
}

RET_CODE albums_ext_prev_page(void)
{ 
  albums_req_photos_t photos_req;
  albums_t *p_albums = NULL;
  u16 str_len = 0;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_FEATURE:
      pic_stop();
      //recaculate current show.
      g_albums_show -= ALBUMS_PAGE;
      photos_req.request_type = PHOTO_BY_FEATURE;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      ui_albums_get_photos(&photos_req);
      return SUCCESS; 
	  
    case ALBUMS_T_SEARCH:
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }
      pic_stop();
      //recaculate current show.
      g_albums_show -= ALBUMS_PAGE;
      photos_req.request_type = PHOTO_BY_SEARCH;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      memset(photos_req.community, 0, sizeof(photos_req.community));
      strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);
      ui_albums_get_photos(&photos_req);
      return SUCCESS;
	  
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }

      pic_stop();
   
      //recaculate current show.
      g_albums_show -= ALBUMS_PAGE;

      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      if(p_albums->p_album[g_focus_album].p_album_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
         photos_req.album_id[str_len] = '\0';
      }
      if(p_albums->p_album[g_focus_album].p_user_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
         photos_req.user_id[str_len] = '\0';
      }
      ui_albums_get_photos(&photos_req);
      return SUCCESS;
    default:
      break;
  }
  
  return ERR_NOFEATURE;
}

RET_CODE albums_ext_next_page(void)
{
  albums_req_photos_t photos_req;
  albums_t *p_albums = NULL;  
  photos_t *p_photos = NULL;
  u16 str_len = 0;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_FEATURE:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
      {
        return ERR_FAILURE;
      }
      
      pic_stop();

      //recaculate current show.
      g_albums_show += ALBUMS_PAGE;

      photos_req.request_type = PHOTO_BY_FEATURE;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;

      ui_albums_get_photos(&photos_req);

      return SUCCESS;     
    case ALBUMS_T_SEARCH:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
    
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }

      if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
      {
        return ERR_FAILURE;
      }
      
      pic_stop();

      albums_clear_all(g_ctrls.p_menu);
    
      //recaculate current show.
      g_albums_show += ALBUMS_PAGE;

      photos_req.request_type = PHOTO_BY_SEARCH;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      memset(photos_req.community, 0, sizeof(photos_req.community));
      strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);

      ui_albums_get_photos(&photos_req);

      return SUCCESS;
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }
      
      if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
      {
        return ERR_FAILURE;
      }
      
      pic_stop();

      albums_clear_all(g_ctrls.p_menu);
    
      //recaculate current show.
      g_albums_show += ALBUMS_PAGE;

      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      if(p_albums->p_album[g_focus_album].p_album_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
         photos_req.album_id[str_len] = '\0';
      }
      if(p_albums->p_album[g_focus_album].p_user_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
         photos_req.user_id[str_len] = '\0';
      }

      ui_albums_get_photos(&photos_req);

      return SUCCESS;

    default:
      break;
  }  

  return SUCCESS;
}


RET_CODE albums_ext_first_page(void)
{
  
    albums_req_photos_t photos_req;
    photos_t *p_photos = NULL;
    memset(&photos_req,0,sizeof(albums_req_photos_t)); 
    if(ALBUMS_T_FEATURE == g_albums_type)
    {
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      pic_stop();

      //switch the first page
      g_albums_show = 0;

      photos_req.request_type = PHOTO_BY_FEATURE;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      ui_albums_get_photos(&photos_req);
    }
    else if(ALBUMS_T_ALBUMS_PHOTOS == g_albums_type)
    {
      albums_t* p_albums = ui_albums_get_albums_list();
      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      pic_stop();

      g_albums_show = 0;
      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = 1;
      photos_req.per_page = ALBUMS_PAGE;

    if(p_albums->p_album[g_focus_album].p_album_id != NULL)
      strncpy((s8 *)photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, ALBUMS_ID_LEN - 1);

    if(p_albums->p_album[g_focus_album].p_user_id != NULL)
      strncpy((s8 *)photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, ALBUMS_ID_LEN-1);

    ui_albums_get_photos(&photos_req);
  }
	
  return SUCCESS;
}

RET_CODE open_albums(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_list_cont = NULL, *p_page_cont = NULL,*p_page = NULL;
  control_t *p_ttl = NULL,  *p_type = NULL, *p_icon = NULL;
  control_t *p_prew_cont = NULL, *p_friend_list = NULL, *p_bottom_help = NULL;
  control_t *p_item[ALBUMS_PAGE], *p_prew = NULL, *p_text = NULL;
 // control_t *p_picture_back = NULL;
  //control_t *p_picture_back1 = NULL;  
  //control_t *p_picture_back2 = NULL;
  albums_req_photos_t photos_req;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
	  //RSI_ITEM_12_SH,
	  //RSI_ITEM_12_SH,

  u16 i = 0;
  
  ui_pic_init(PIC_SOURCE_NET);
  ui_albums_init(ALBUMS_YUPOO);
  sys_status_get_albums_friends_info(&g_albums_friends);
  
  g_albums_site = ALBUMS_YUPOO;
  g_albums_type = ALBUMS_T_FEATURE;

  //create menu.
 
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_ALBUMS, 0,
                                    IM_INDEX_PICTURE_BANNER, IDS_PICTURE);
  MT_ASSERT(p_menu != NULL);
  ctrl_set_keymap(p_menu, albums_menu_keymap);
  ctrl_set_proc(p_menu, albums_menu_proc);
    g_ctrls.p_menu = p_menu;

 /* p_picture_back = ctrl_create_ctrl(CTRL_CONT, IDC_OMW_WITEM_START1,
						  ALBUMS_TTL_X, ALBUMS_TTL_Y,
						  ALBUMS_TTL_W, ALBUMS_TTL_H-10,
						  p_menu, 0);
  ctrl_set_rstyle(p_picture_back, RSI_WHITE_FRM, RSI_WHITE_FRM, RSI_WHITE_FRM);
*/



  p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ALBUMS_CONT,
  ALBUMS_PAGE_CONT_X, ALBUMS_PAGE_CONT_Y, ALBUMS_PAGE_CONT_W, ALBUMS_PAGE_CONT_H, p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    g_ctrls.p_page_cont = p_page_cont;
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               ALBUMS_PAGE_ARROWL_X, ALBUMS_PAGE_ARROWL_Y,
                               ALBUMS_PAGE_ARROWL_W, ALBUMS_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               ALBUMS_PAGE_ARROWR_X, ALBUMS_PAGE_ARROWR_Y,
                               ALBUMS_PAGE_ARROWR_W, ALBUMS_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);
  p_page = ctrl_create_ctrl(CTRL_TEXT, IDC_ALBUMS_PAGE,
    ALBUMS_PAGE_X, ALBUMS_PAGE_Y, ALBUMS_PAGE_W, ALBUMS_PAGE_H, p_page_cont, 0);
  
  ctrl_set_rstyle(p_page, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_page, FSI_RED, FSI_RED, FSI_RED);
  text_set_align_type(p_page, STL_LEFT|STL_VCENTER);
  text_set_content_type(p_page, TEXT_STRTYPE_UNICODE);  

  p_page = ctrl_create_ctrl(CTRL_TEXT, 0,
    ALBUMS_PAGE1_X, ALBUMS_PAGE1_Y, ALBUMS_PAGE1_W, ALBUMS_PAGE1_H, p_page_cont, 0);
  ctrl_set_rstyle(p_page, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_page, FSI_RED, FSI_RED, FSI_RED);
  text_set_align_type(p_page, STL_CENTER|STL_VCENTER);
  text_set_content_type(p_page, TEXT_STRTYPE_STRID);  
  text_set_content_by_strid(p_page, IDS_PAGE1);

    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ALBUMS_TYPE_LIST_CONT,
                                  ALBUMS_TYPE_LIST_CONT_X, 
                                  ALBUMS_TYPE_LIST_CONT_Y,
                                  ALBUMS_TYPE_LIST_CONT_W, 
                                  ALBUMS_TYPE_LIST_CONT_H,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    p_ttl = ctrl_create_ctrl(CTRL_BMAP, IDC_ALBUMS_TTL, 
                                        ALBUMS_TYPE_PIC_X, ALBUMS_TYPE_PIC_Y, 
                                        ALBUMS_TYPE_PIC_W, ALBUMS_TYPE_PIC_H, p_list_cont, 0);
    ctrl_set_rstyle(p_ttl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_ttl, IM_PIC_YUPOO_LOGO);
    g_ctrls.p_icon_title = p_ttl;
  p_type = ctrl_create_ctrl(CTRL_LIST, IDC_ALBUMS_TYPE,
    ALBUMS_TYPE_X, ALBUMS_TYPE_Y, ALBUMS_TYPE_W, ALBUMS_TYPE_H, p_list_cont, 0);
  ctrl_set_keymap(p_type, albums_type_keymap);
  ctrl_set_proc(p_type, albums_type_proc);
  g_ctrls.p_list = p_type;
  ctrl_set_rstyle(p_type, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_mrect(p_type, 0, 0, ALBUMS_TYPE_W, ALBUMS_TYPE_H);
  list_set_item_interval(p_type, ALBUMS_TYPE_VGAP);
  list_set_item_rstyle(p_type, &type_irstyle);
  list_enable_select_mode(p_type, TRUE);
  list_set_select_mode(p_type, LIST_SINGLE_SELECT);  
  list_set_count(p_type, ALBUMS_TYPE_TOT, ALBUMS_TYPE_PAGE);
  list_set_field_count(p_type, ALBUMS_TYPE_FCNT, ALBUMS_TYPE_PAGE);
  list_set_focus_pos(p_type, 0);
  list_select(p_type);
  list_set_update(p_type, albums_type_list_update, 0);
  albums_type_list_update(p_type, list_get_valid_pos(p_type), ALBUMS_TYPE_PAGE, 0);
  list_set_field_attr(p_type, 0, STL_CENTER/*STL_LEFT*/, ALBUMS_TYPE_W, 0, 0);
  list_set_field_rect_style(p_type, 0, &type_frstyle);
  list_set_field_font_style(p_type, 0, &type_ffstyle);

  p_prew_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ALBUMS_PREW_CONT,
    ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY, ALBUMS_PREW_CONTW, ALBUMS_PREW_CONTH, p_menu, 0);
  ctrl_set_keymap(p_prew_cont, albums_prew_cont_keymap);
  ctrl_set_proc(p_prew_cont, albums_prew_cont_proc);
  g_ctrls.p_prev_cont = p_prew_cont;
  ctrl_set_rstyle(p_prew_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);      

  for(i = 0; i < ALBUMS_PAGE; i++)
  {
    p_item[i] = ctrl_create_ctrl(CTRL_CONT, IDC_ALBUMS_ITEM1 + i,
      ALBUMS_ITEM_X + i % ALBUMS_COL * (ALBUMS_ITEM_W + ALBUMS_ITEM_HGAP),
      (u16)(ALBUMS_ITEM_Y + i / ALBUMS_COL * (ALBUMS_ITEM_H + ALBUMS_ITEM_VGAP)),
      ALBUMS_ITEM_W,
      ALBUMS_ITEM_H,
      p_prew_cont, 0);
    ctrl_set_rstyle(p_item[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);      
    ctrl_set_style(p_item[i], STL_EX_WHOLE_HL);
    ctrl_set_sts(p_item[i], OBJ_STS_HIDE);
  
    p_prew = ctrl_create_ctrl(CTRL_TEXT, IDC_ALBUMS_ITEM_PREW,
      ALBUMS_PREW_X, ALBUMS_PREW_Y, ALBUMS_PREW_W, ALBUMS_PREW_H, p_item[i], 0);
    //ctrl_set_rstyle(p_prew, RSI_ITEM_11_SH, RSI_ITEM_11_HL, RSI_ITEM_11_SH);
    ctrl_set_rstyle(p_prew, RSI_OTT_FRM_1_SH, RSI_OTT_FRM_1_HL, RSI_OTT_FRM_1_SH);
    text_set_font_style(p_prew, FSI_WHITE_S, FSI_WHITE_S, FSI_WHITE_S);
    text_set_content_type(p_prew, TEXT_STRTYPE_UNICODE); 

    p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_ALBUMS_ITEM_TEXT,
      ALBUMS_TEXT_X, ALBUMS_TEXT_Y, ALBUMS_TEXT_W, ALBUMS_TEXT_H, p_item[i], 0);
    ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_text, FSI_WHITE_S, FSI_WHITE_S, FSI_WHITE_S);
    text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);  

    ctrl_set_related_id(p_item[i],
      (i % ALBUMS_COL == 0) ? (i + ALBUMS_COL): (i),
      (i < ALBUMS_COL) ? (i + ALBUMS_PAGE - ALBUMS_COL + 1) : (i - ALBUMS_COL + 1),
      (u16)(((i + 1) % ALBUMS_COL == 0) ? (i / ALBUMS_COL * ALBUMS_COL + 1) : (i + 2)),
      (i >= (ALBUMS_PAGE - ALBUMS_COL)) ? (i % ALBUMS_COL + 1): (i + ALBUMS_COL + 1));
  }  

  p_friend_list = ctrl_create_ctrl(CTRL_LIST, IDC_ALBUMS_FRIEND_LIST,
    ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY, ALBUMS_PREW_CONTW, ALBUMS_PREW_CONTH, p_menu, 0);
  ctrl_set_keymap(p_friend_list, albums_friend_keymap);
  ctrl_set_proc(p_friend_list, albums_friend_proc);
  g_ctrls.p_friend_list = p_friend_list;
  ctrl_set_rstyle(p_friend_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_mrect(p_friend_list, 0, 0, 780, 434);
  list_set_item_interval(p_friend_list, ALBUMS_FRIEND_VGAP);
  list_set_item_rstyle(p_friend_list, &friend_irstyle);
  list_set_count(p_friend_list,g_albums_friends.friends_num, ALBUMS_FRIEND_PAGE);
  list_set_field_count(p_friend_list, ALBUMS_FRIEND_FCNT, ALBUMS_FRIEND_PAGE);
  list_set_focus_pos(p_friend_list, 0);
  list_enable_page_mode(p_friend_list, TRUE);
  list_set_update(p_friend_list, albums_friend_list_update, 0);

  for(i = 0; i < ALBUMS_FRIEND_FCNT; i++)
  {
    list_set_field_attr2(p_friend_list, (u8)i, friend_attr[i].attr,
      friend_attr[i].width, friend_attr[i].height, friend_attr[i].left, friend_attr[i].top);
    list_set_field_rect_style(p_friend_list, (u8)i, friend_attr[i].p_rstyle);
    list_set_field_font_style(p_friend_list, (u8)i, friend_attr[i].p_fstyle);
  }
  albums_friend_list_update(p_friend_list, list_get_valid_pos(p_friend_list), ALBUMS_FRIEND_PAGE, 0);  
  ctrl_set_sts(p_friend_list, OBJ_STS_HIDE);
    p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_ALBUMS_BTM_HELP,
                                      ALBUMS_BOTTOM_HELP_X, ALBUMS_BOTTOM_HELP_Y,
                                      ALBUMS_BOTTOM_HELP_W, ALBUMS_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    ui_comm_help_create_ext(60, 0, 
  //ui_comm_help_move_pos(p_menu, 260, -80, 800, 50, 42);
                                ALBUMS_BOTTOM_HELP_W-60, ALBUMS_BOTTOM_HELP_H,  
                                &help_data,  p_bottom_help);
    g_ctrls.p_help_cont = p_bottom_help;
  ctrl_default_proc(p_type, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  g_albums_show = 0;
  
  photos_req.request_type = PHOTO_BY_FEATURE;
  photos_req.page = 1;
  photos_req.per_page = ALBUMS_PAGE;
  ui_albums_get_photos(&photos_req);
  pic_play_update_slide_show(TRUE);//pic from net
  return SUCCESS;
}

static RET_CODE albums_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  pic_stop();

  ui_pic_clear_all(0);
  
  ui_pic_release();

  ui_albums_release();

  return ERR_NOFEATURE;
}

static RET_CODE albums_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_prew_cont = NULL;
  control_t *p_item = NULL;
  albums_t *p_albums = NULL;
  photos_t *p_photos = NULL;
  mul_pic_param_t pic_param = {{0},};
  u16 thumb_idx = 0, cnt = 0;
  albums_site_t site = (albums_site_t)((para1 >> 16) & 0xFFFF);
  u16 photo_idx = (para1 & 0xFFFF);
  u8 focus_root = 0;
  if(site != g_albums_site)
  {
    return ERR_FAILURE;
  }
  focus_root = fw_get_focus_id();
  
  if(focus_root != ROOT_ID_ALBUMS &&
      focus_root != ROOT_ID_POPUP &&
      focus_root != ROOT_ID_PHOTO_INFO &&
      focus_root != ROOT_ID_PIC_PLAY_MODE_SET)
  {
    return ERR_FAILURE;
  }  
  p_prew_cont = g_ctrls.p_prev_cont;
  p_item = ctrl_get_child_by_id(p_prew_cont, (u8)(IDC_ALBUMS_ITEM1 + photo_idx));
  switch(g_albums_type)
  {
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
    case ALBUMS_T_ALBUMS_PHOTOS:  
    case ALBUMS_T_FRIENDS_PHOTOS:
		
      p_photos = ui_albums_get_photos_list();
      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      if(photo_idx > p_photos->cnt)
      {
        return ERR_FAILURE;
      }   
      albums_set_photos_data(p_item, &p_photos->p_photo[photo_idx], msg);
      if((photo_idx + 1) >= p_photos->cnt)
      {
        return ERR_FAILURE;
      }      
      albums_set_page(p_cont);
      pic_stop();
      thumb_idx = p_photos->p_photo[0].thumb_num - 1;
      if(thumb_idx > (p_photos->p_photo[photo_idx + 1].thumb_num - 1))        
      {
          return ERR_FAILURE;
      }
      ui_pic_set_url((u8 *)p_photos->p_photo[photo_idx + 1].p_thumb[thumb_idx].p_thumb_url);
      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = ALBUMS_PREW_X;
      pic_param.win_rect.top = ALBUMS_PREW_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
      pic_param.file_size = 0;
      pic_param.handle = ((g_albums_site << 16) | (photo_idx  + 1));
      pic_param.is_thumb = TRUE;
      offset_rect(&pic_param.win_rect,
        ALBUMS_ITEM_X + (photo_idx + 1) % ALBUMS_COL * (ALBUMS_ITEM_W + ALBUMS_ITEM_HGAP),
        (s16)(ALBUMS_ITEM_Y + (photo_idx + 1) / ALBUMS_COL * (ALBUMS_ITEM_H + ALBUMS_ITEM_VGAP)));
      offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
      offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);
      pic_start(&pic_param);           
      break;

    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();
      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }   
     // if(p_albums->album_total > ALBUMS_PAGE)
     if((p_albums->album_total - g_albums_show) > ALBUMS_PAGE )
      {
        cnt = ALBUMS_PAGE;
      }
      else
      {
        cnt = (u16)(p_albums->album_total - g_albums_show);
      }
      if(photo_idx  >= p_albums->album_total)
      {
        return ERR_FAILURE;
      }      
      if(photo_idx > cnt)
      {
        return ERR_FAILURE;
      }
      albums_set_albums_data(p_item, &p_albums->p_album[g_albums_show + photo_idx]);
     
      if((photo_idx + 1) >= cnt)
      {
        return ERR_FAILURE;
      }
      pic_stop();
      albums_set_page(p_cont);
      ui_pic_set_url((u8 *)p_albums->p_album[g_albums_show + photo_idx +1].thumb.p_thumb_url);
      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = ALBUMS_PREW_X;
      pic_param.win_rect.top = ALBUMS_PREW_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
      pic_param.file_size = 0;
      pic_param.handle = ((g_albums_site << 16) | (photo_idx  + 1));
      pic_param.is_thumb = TRUE;
      offset_rect(&pic_param.win_rect,
        ALBUMS_ITEM_X + (photo_idx + 1) % ALBUMS_COL * (ALBUMS_ITEM_W + ALBUMS_ITEM_HGAP),
        (s16)(ALBUMS_ITEM_Y + (photo_idx + 1) / ALBUMS_COL * (ALBUMS_ITEM_H + ALBUMS_ITEM_VGAP)));
      offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
      offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);
      pic_start(&pic_param);        
      break;

    default:

      break;
  }

  return SUCCESS;
}

static RET_CODE reset_focus_on_pic(u8 pos)
{
  control_t *pre_cont = NULL;
  control_t *goal_act = NULL;
  control_t *old_act  = NULL;
   control_t *p_active = NULL;
  u8 current = 0;
  u8 active = 0;

  if(pos > ALBUMS_PAGE)
  {
    OS_PRINTF("pos is invaild!\n");
    return ERR_FAILURE;
  }
  pre_cont = g_ctrls.p_prev_cont;//pre cont
  

  p_active = ctrl_get_active_ctrl(pre_cont);  
  current = (u8)ctrl_get_ctrl_id(p_active);  
  if(pos == 0)
    {
      active = current;
    }
  else
    {
      active = pos;
    }
  goal_act = ctrl_get_child_by_id(pre_cont, active);

  old_act = ctrl_get_active_ctrl(pre_cont);

  ctrl_process_msg(old_act, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(goal_act, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(old_act, TRUE);
  ctrl_paint_ctrl(goal_act, TRUE);
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);

  //ctrl_set_active_ctrl(pre_cont, goal_act);

  return SUCCESS;
}
static RET_CODE reset_focus_on_pic_without_paint(u8 pos)
{
  control_t *pre_cont = NULL;
  control_t *goal_act = NULL;
  control_t *old_act  = NULL;
   control_t *p_active = NULL;
  u8 current = 0;
  u8 active = 0;
  if(pos > ALBUMS_PAGE)
  {
    OS_PRINTF("pos is invaild!\n");
    return ERR_FAILURE;
  }
  pre_cont = g_ctrls.p_prev_cont;//pre cont
  p_active = ctrl_get_active_ctrl(pre_cont);  
  current = (u8)ctrl_get_ctrl_id(p_active);  
  if(pos == 0)
    {
      active = current;
    }
  else
    {
      active = pos;
    }
  goal_act = ctrl_get_child_by_id(pre_cont, active);
  old_act = ctrl_get_active_ctrl(pre_cont);
  ctrl_process_msg(old_act, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(goal_act, MSG_GETFOCUS, 0, 0);
  return SUCCESS;
}

static RET_CODE albums_update(control_t *p_menu, u16 msg, u32 para1, u32 para2)
{ 
  mul_pic_param_t pic_param = {{0},};
  photos_t *p_photos = NULL;
  u16 thumb_idx = 0;
  net_conn_stats_t net_state;
  net_state = ui_get_net_connect_status();
  if((FALSE == net_state.is_3g_conn) 
    && (FALSE == net_state.is_eth_conn) 
    && (FALSE == net_state.is_gprs_conn)
    && (FALSE == net_state.is_wifi_conn))
    {
        exit_album();
        return SUCCESS;
    }
  if((BOOL)para1 == TRUE)
    reset_focus_on_pic((u8)para2);

  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
      p_photos = (photos_t *)ui_albums_get_photos_list();
      if(p_photos == NULL || p_photos->cnt == 0)
      {
        return ERR_FAILURE;
      }
//      MT_ASSERT(p_photos != NULL);      
//      MT_ASSERT(p_photos->cnt != 0);

      thumb_idx = p_photos->p_photo[0].thumb_num - 1;

      pic_stop();
      
      ui_pic_set_url((u8 *)p_photos->p_photo[0].p_thumb[thumb_idx].p_thumb_url);      

      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = ALBUMS_PREW_X;
      pic_param.win_rect.top = ALBUMS_PREW_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
      pic_param.file_size = 0;
      pic_param.handle = ((g_albums_site << 16)| 0);
      pic_param.is_thumb = TRUE;
      
      offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
      offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
      offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

      pic_start(&pic_param);        
      break;

      case ALBUMS_T_FRIENDS:
      if( g_ctrls.p_friend_list)
      {
        ctrl_paint_ctrl(g_ctrls.p_friend_list, TRUE);
      }
      break;
      
    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE albums_change_picture_type(control_t *p_menu, u16 msg, u32 para1, u32 para2)
{ 
  control_t *p_type = NULL;
  control_t *p_title = NULL;
  control_t *p_friend = NULL;
  control_t *p_cont = NULL;
  control_t *p_active = NULL;
  albums_req_photos_t photos_req;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  pic_stop();
  albums_clear_all(p_menu);
  ui_pic_release();
  ui_albums_release();
  
  p_type = g_ctrls.p_list;
  p_title = g_ctrls.p_icon_title;
  p_cont = g_ctrls.p_prev_cont;
  p_friend = g_ctrls.p_friend_list;

  switch(g_albums_type)
  {
    case ALBUMS_T_FRIENDS:
      ctrl_set_sts(p_friend, OBJ_STS_HIDE);
      ctrl_process_msg(p_friend, MSG_LOSTFOCUS, 0, 0);
      ctrl_hide_ctrl(p_friend);
      ctrl_paint_ctrl(p_friend, TRUE);    
      break;
    default:    
      p_active = ctrl_get_active_ctrl(p_cont);
      if(p_active != NULL)
      {
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_paint_ctrl(p_active, TRUE);  
      }
      break;
    
  }
  list_set_focus_pos(p_type, 0);
  ctrl_process_msg(p_type, MSG_GETFOCUS, 0, 0);
  list_select(p_type);
  ctrl_paint_ctrl(p_type, TRUE);
  
  switch(msg)
  {
    case MSG_RED:
      g_albums_type = ALBUMS_T_FEATURE;
    
      ui_pic_init(PIC_SOURCE_NET);
      if(g_albums_site == ALBUMS_YUPOO)
      {
        g_albums_site = ALBUMS_PICASA;
        bmap_set_content_by_id(p_title, IM_PIC_PICASA_LOGO);
        ui_albums_init(ALBUMS_PICASA);
      }
      else if(g_albums_site == ALBUMS_PICASA)
      {
        g_albums_site = ALBUMS_FLICKR;
        bmap_set_content_by_id(p_title, IM_PIC_FLICKR_LOGO);

		ui_albums_init(ALBUMS_FLICKR);
      }
      else if(g_albums_site == ALBUMS_FLICKR)
      {
        g_albums_site = ALBUMS_YUPOO;
        bmap_set_content_by_id(p_title, IM_PIC_YUPOO_LOGO);

        ui_albums_init(ALBUMS_YUPOO);
      }
      else
      {
        MT_ASSERT(0);
      }

      ctrl_paint_ctrl(p_title, TRUE);
      break;
      
    default:
      MT_ASSERT(0);
      break;
  }

  g_albums_show = 0;
  
  photos_req.request_type = PHOTO_BY_FEATURE;
  photos_req.page = 1;
  photos_req.per_page = ALBUMS_PAGE;
  
  ui_albums_get_photos(&photos_req);
  
  albums_help_update(p_menu, &help_data);
  
  return SUCCESS;
}

void ui_albums_retry(void)
{ 
  albums_req_photos_t photos_req;
  albums_req_albums_t albums_req;
  albums_t *p_albums = NULL;
  u16 str_len = 0;

  memset(&photos_req,0,sizeof(albums_req_photos_t));
  memset(&albums_req,0,sizeof(albums_req_albums_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return;
      }
      
      memset(albums_req.user_name, 0, sizeof(albums_req.user_name));
      strcpy((s8 *)albums_req.user_name, (s8 *)g_albums_community);
      albums_req.request_type = ALBUM_BY_USER_NAME;
      ui_albums_get_albums(&albums_req);    
      break;
      
    case ALBUMS_T_FRIENDS_ALBUMS:
      albums_req.request_type = ALBUM_BY_USER_ID;
      memset(albums_req.user_id, 0, sizeof(albums_req.user_id));
      strcpy((s8 *)albums_req.user_id, (s8 *)g_albums_friends.albums_friend[g_friend_focus].user_id);
      
      ui_albums_get_albums(&albums_req);  
      break;

    case ALBUMS_T_FEATURE:
      pic_stop();

      albums_clear_all(g_ctrls.p_menu);
    
      //recaculate current show.
      photos_req.request_type = PHOTO_BY_FEATURE;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;

      ui_albums_get_photos(&photos_req);

      return;
	  
      
    case ALBUMS_T_SEARCH:
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return;
      }

      pic_stop();

      albums_clear_all(g_ctrls.p_menu);
    
      //recaculate current show.
      photos_req.request_type = PHOTO_BY_SEARCH;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      memset(photos_req.community, 0, sizeof(photos_req.community));
      strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);

      ui_albums_get_photos(&photos_req);

      return;
  
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return;
      }

      if(p_albums->p_album == NULL)
      {
        return;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return;
      }

      pic_stop();

      albums_clear_all(g_ctrls.p_menu);
    
      //recaculate current show.
      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
      photos_req.per_page = ALBUMS_PAGE;
      if(p_albums->p_album[g_focus_album].p_album_id != NULL)
      { 
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
         photos_req.album_id[str_len] = '\0';
      }
      if(p_albums->p_album[g_focus_album].p_user_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
         photos_req.user_id[str_len] = '\0';
      }
      ui_albums_get_photos(&photos_req);

      return;
   

    default:
      break;
  }
  
  return;
}

static RET_CODE albums_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{   
  ui_comm_ask_for_dodlg_open(NULL, IDS_NET_RETRY, ui_albums_retry, NULL, 0);

  return SUCCESS;
}

static RET_CODE albums_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  albums_t *p_albums = NULL;
  photos_t *p_photos = NULL;
  mul_pic_param_t pic_param = {{0},};
  u16 thumb_idx = 0;

  if (para1 == ALBUMS_EVT_ALBUMS_LIST)
  {
    p_albums = (albums_t *)para2;
    if (ui_albums_context_cmp(p_albums->context))
    {
      return ERR_FAILURE;
    }
  }
  else if (para1 == ALBUMS_EVT_PHOTOS_LIST)
  {
    p_photos = (photos_t *)para2;
    if (ui_albums_context_cmp(p_photos->context))
    {
      return ERR_FAILURE;
    }
  }
  else
  {
    return ERR_FAILURE;
  }

  if(fw_get_focus_id() != ROOT_ID_ALBUMS)
  {
    return ERR_FAILURE;
  }  

  if(para2 == 0)
  {
    return ERR_FAILURE;
  }
  switch(g_albums_type)
  {
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_PHOTOS:
      p_photos = (photos_t *)para2;
	  
      if(p_photos->cnt == 0)
      {
        return ERR_FAILURE;
      }

      thumb_idx = p_photos->p_photo[0].thumb_num - 1;

      pic_stop();
      
      ui_pic_set_url((u8 *)p_photos->p_photo[0].p_thumb[thumb_idx].p_thumb_url);

      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = ALBUMS_PREW_X;
      pic_param.win_rect.top = ALBUMS_PREW_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
      pic_param.file_size = 0;
      pic_param.handle = ((g_albums_site << 16)| 0);
      pic_param.is_thumb = TRUE;
      
      offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
      offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
      offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);
      pic_start(&pic_param);        
      break;

    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:    
      p_albums = (albums_t *)para2;
	  
      if(p_albums->album_total == 0)
      {
        return ERR_FAILURE;
      }      

      ui_pic_set_url((u8 *)p_albums->p_album[0].thumb.p_thumb_url);

      pic_param.anim = REND_ANIM_NONE;
      pic_param.style = REND_STYLE_CENTER;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.win_rect.left = ALBUMS_PREW_X;
      pic_param.win_rect.top = ALBUMS_PREW_Y;
      pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
      pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
      pic_param.file_size = 0;
      pic_param.handle = ((g_albums_site << 16)| 0);

      offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
      offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
      offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

      pic_start(&pic_param);        
      break;

    default:

      break;
  }

  

  return SUCCESS;
}


static RET_CODE albums_cont_page_up(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  albums_req_photos_t photos_req;
  mul_pic_param_t pic_param = {{0},};
  //control_t *p_active = NULL;
  albums_t *p_albums = NULL;
 // u8 active_id = 0;
 u16 str_len = 0;
 // p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);
  reset_focus_on_pic_without_paint(1);
  //active_id = (u8)ctrl_get_ctrl_id(p_active);
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }  

      if(g_albums_show >= ALBUMS_PAGE)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        //request picture.
        ui_pic_set_url((u8 *)p_albums->p_album[g_albums_show].thumb.p_thumb_url);

        pic_param.anim = REND_ANIM_NONE;
        pic_param.style = REND_STYLE_CENTER;
        pic_param.flip = PIC_NO_F_NO_R;
        pic_param.win_rect.left = ALBUMS_PREW_X;
        pic_param.win_rect.top = ALBUMS_PREW_Y;
        pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
        pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
        pic_param.file_size = 0;
        pic_param.handle = ((g_albums_site << 16)| 0);
        pic_param.is_thumb = TRUE;
        
        offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
        offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
        offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

        pic_start(&pic_param);   
        
        return SUCCESS;
      }      
      break;

    case ALBUMS_T_FEATURE:
      if(g_albums_show >= ALBUMS_PAGE)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_FEATURE;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }       
      break;
      
    case ALBUMS_T_SEARCH:
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }

      if(g_albums_show >= ALBUMS_PAGE)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_SEARCH;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        memset(photos_req.community, 0, sizeof(photos_req.community));
        strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(g_albums_show >= ALBUMS_PAGE)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_ALBUM;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        if(p_albums->p_album[g_focus_album].p_album_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
            photos_req.album_id[str_len] = '\0';
        }
        if(p_albums->p_album[g_focus_album].p_user_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
            photos_req.user_id[str_len] = '\0';
        }
        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;

    default:
      break;
  }
  
  return SUCCESS;
}

static RET_CODE albums_cont_focus_up(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  albums_req_photos_t photos_req;
  mul_pic_param_t pic_param = {{0},};
  control_t *p_active = NULL;
  albums_t *p_albums = NULL;
  u8 active_id = 0;
  u16 str_len = 0;

  p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }  

      if((active_id <= ALBUMS_COL) && (g_albums_show >= ALBUMS_PAGE))
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        //request picture.
        ui_pic_set_url((u8 *)p_albums->p_album[g_albums_show].thumb.p_thumb_url);

        pic_param.anim = REND_ANIM_NONE;
        pic_param.style = REND_STYLE_CENTER;
        pic_param.flip = PIC_NO_F_NO_R;
        pic_param.win_rect.left = ALBUMS_PREW_X;
        pic_param.win_rect.top = ALBUMS_PREW_Y;
        pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
        pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
        pic_param.file_size = 0;
        pic_param.handle = ((g_albums_site << 16)| 0);
        pic_param.is_thumb = TRUE;
        
        offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
        offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
        offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

        pic_start(&pic_param);   
        
        return SUCCESS;
      }      
      break;

    case ALBUMS_T_FEATURE:
      if((active_id <= ALBUMS_COL) && (g_albums_show >= ALBUMS_PAGE))
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
        reset_focus_on_pic_without_paint(1);
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_FEATURE;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }       
      break;
      
    case ALBUMS_T_SEARCH:
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }

      if((active_id <= ALBUMS_COL) && (g_albums_show >= ALBUMS_PAGE))
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_SEARCH;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        memset(photos_req.community, 0, sizeof(photos_req.community));
        strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }

      if((active_id <= ALBUMS_COL) && (g_albums_show >= ALBUMS_PAGE))
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show -= ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_ALBUM;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        if(p_albums->p_album[g_focus_album].p_album_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
            photos_req.album_id[str_len] = '\0';
        }
        if(p_albums->p_album[g_focus_album].p_user_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
            photos_req.user_id[str_len] = '\0';
        }
        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;

    default:
      break;
  }
  
  return ERR_NOFEATURE;
}


static RET_CODE albums_cont_page_down(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  albums_req_photos_t photos_req;
  mul_pic_param_t pic_param = {{0},};
  //control_t *p_active = NULL;
  //u8 active_id = 0;
  albums_t *p_albums = NULL;  
  photos_t *p_photos = NULL;
  u16 str_len = 0;
  //p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  //active_id = (u8)ctrl_get_ctrl_id(p_active);
  reset_focus_on_pic_without_paint(1);
  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }    
      
      if((g_albums_show + ALBUMS_PAGE) < p_albums->album_total)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        //request picture.
        ui_pic_set_url((u8 *)p_albums->p_album[g_albums_show].thumb.p_thumb_url);

        pic_param.anim = REND_ANIM_NONE;
        pic_param.style = REND_STYLE_CENTER;
        pic_param.flip = PIC_NO_F_NO_R;
        pic_param.win_rect.left = ALBUMS_PREW_X;
        pic_param.win_rect.top = ALBUMS_PREW_Y;
        pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
        pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
        pic_param.file_size = 0;
        pic_param.handle = ((g_albums_site << 16)| 0);
        pic_param.is_thumb = TRUE;
        
        offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
        offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
        offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

        pic_start(&pic_param);   
        
        return SUCCESS;
      }      
      break;

    case ALBUMS_T_FEATURE:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if((g_albums_show + ALBUMS_PAGE) < p_photos->photo_total)
      {
        
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_FEATURE;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }       
      break;
      
    case ALBUMS_T_SEARCH:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
    
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }

      if((g_albums_show + ALBUMS_PAGE) < p_photos->photo_total)
      {
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_SEARCH;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        memset(photos_req.community, 0, sizeof(photos_req.community));
        strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }

      if((g_albums_show + ALBUMS_PAGE) < p_photos->photo_total)
      {
        
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_ALBUM;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        if(p_albums->p_album[g_focus_album].p_album_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
            photos_req.album_id[str_len] = '\0';
        }

        if(p_albums->p_album[g_focus_album].p_user_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
            photos_req.user_id[str_len] = '\0';
        }

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;

    default:
      break;
  }  

  return SUCCESS;
}

static RET_CODE albums_cont_focus_down(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  albums_req_photos_t photos_req;
  mul_pic_param_t pic_param = {{0},};
  control_t *p_active = NULL;
  u8 active_id = 0;
  albums_t *p_albums = NULL;  
  photos_t *p_photos = NULL;
  u16 str_len = 0;
  p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);
  active_id = (u8)ctrl_get_ctrl_id(p_active);
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }    
      
      if(active_id > (ALBUMS_PAGE - ALBUMS_COL))
      {
        if((g_albums_show + ALBUMS_PAGE) >= p_albums->album_total)
        {
          return ERR_FAILURE;
        }
        
        reset_focus_on_pic_without_paint(1);
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        //request picture.
        ui_pic_set_url((u8 *)p_albums->p_album[g_albums_show].thumb.p_thumb_url);

        pic_param.anim = REND_ANIM_NONE;
        pic_param.style = REND_STYLE_CENTER;
        pic_param.flip = PIC_NO_F_NO_R;
        pic_param.win_rect.left = ALBUMS_PREW_X;
        pic_param.win_rect.top = ALBUMS_PREW_Y;
        pic_param.win_rect.right = (pic_param.win_rect.left + ALBUMS_PREW_W);
        pic_param.win_rect.bottom = (pic_param.win_rect.top + ALBUMS_PREW_H);
        pic_param.file_size = 0;
        pic_param.handle = ((g_albums_site << 16)| 0);
        pic_param.is_thumb = TRUE;
        
        offset_rect(&pic_param.win_rect, ALBUMS_ITEM_X, ALBUMS_ITEM_Y);
        offset_rect(&pic_param.win_rect, ALBUMS_PREW_CONTX, ALBUMS_PREW_CONTY);
        offset_rect(&pic_param.win_rect, ALBUMS_CONT_X, ALBUMS_CONT_Y);

        pic_start(&pic_param);   
        
        return SUCCESS;
      }      
      break;

    case ALBUMS_T_FEATURE:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(active_id > (ALBUMS_PAGE - ALBUMS_COL))
      {
        if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
        {
          return ERR_FAILURE;
        }
        reset_focus_on_pic_without_paint(1);
        pic_stop();
        albums_clear_all(g_ctrls.p_menu);
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;
        photos_req.request_type = PHOTO_BY_FEATURE;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        ui_albums_get_photos(&photos_req);
        return SUCCESS;
      }       
      break;
      
    case ALBUMS_T_SEARCH:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
    
      if(strlen((s8 *)g_albums_community) == 0)
      {
        return ERR_FAILURE;
      }

      if(active_id > (ALBUMS_PAGE - ALBUMS_COL))
      {
        if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
        {
          return ERR_FAILURE;
        }
        
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_SEARCH;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;
        memset(photos_req.community, 0, sizeof(photos_req.community));
        strcpy((s8 *)photos_req.community,(s8 *)g_albums_community);

        ui_albums_get_photos(&photos_req);

        return SUCCESS;
      }
      break;
      
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_ALBUMS_PHOTOS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album == NULL)
      {
        return ERR_FAILURE;
      }
      
      if(p_albums->p_album[g_focus_album].p_album_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(p_albums->p_album[g_focus_album].p_user_id == NULL)
      {
        return ERR_FAILURE;
      }

      if(active_id > (ALBUMS_PAGE - ALBUMS_COL))
      {
        if((g_albums_show + ALBUMS_PAGE) >= p_photos->photo_total)
        {
          return ERR_FAILURE;
        }
        
        pic_stop();

        albums_clear_all(g_ctrls.p_menu);
      
        //recaculate current show.
        g_albums_show += ALBUMS_PAGE;

        photos_req.request_type = PHOTO_BY_ALBUM;
        photos_req.page = (u16)(g_albums_show / ALBUMS_PAGE + 1);
        photos_req.per_page = ALBUMS_PAGE;

        if(p_albums->p_album[g_focus_album].p_album_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_album_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.album_id, p_albums->p_album[g_focus_album].p_album_id, str_len);
            photos_req.album_id[str_len] = '\0';
        }

        if(p_albums->p_album[g_focus_album].p_user_id != NULL)
        {
            str_len = (u16)strlen(p_albums->p_album[g_focus_album].p_user_id);
            str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
            memcpy(photos_req.user_id, p_albums->p_album[g_focus_album].p_user_id, str_len);
            photos_req.user_id[str_len] = '\0';
        }
        ui_albums_get_photos(&photos_req);
        return SUCCESS;
      }
      break;

    default:
      break;
  }  

  return ERR_NOFEATURE;
}

static RET_CODE albums_cont_focus_left(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_active = NULL, *p_type = NULL;
  control_t *p_friend = NULL;
  albums_req_albums_t albums_req;
  u8 active_id = 0;
  p_friend = g_ctrls.p_friend_list;
  p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);
  active_id = (u8)ctrl_get_ctrl_id(p_active);
  memset(&albums_req,0,sizeof(albums_req_albums_t));
 if(msg == MSG_FOCUS_LEFT)
 {
   if((active_id % ALBUMS_COL) == 1)
    {
      p_type = g_ctrls.p_list;
      albums_help_update(g_ctrls.p_menu, &help_data);
      ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_type, MSG_GETFOCUS, 0, 0);

      ctrl_paint_ctrl(p_active, TRUE);
      ctrl_paint_ctrl(p_type, TRUE);
     // ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
      return SUCCESS;
    }
 }
 else if(msg == MSG_EXIT)
 {
   switch(g_albums_type)
    {
      case ALBUMS_T_FRIENDS_ALBUMS:
        pic_stop();
        albums_clear_all(g_ctrls.p_menu);
        ctrl_set_sts(p_friend, OBJ_STS_SHOW);

        list_set_count(p_friend, g_albums_friends.friends_num, ALBUMS_FRIEND_PAGE);
        list_set_focus_pos(p_friend, 0);
        
        albums_friend_list_update(p_friend, list_get_valid_pos(p_friend), ALBUMS_FRIEND_PAGE, 0);

        ctrl_paint_ctrl(p_friend, TRUE);
        
        g_albums_type = ALBUMS_T_FRIENDS;
        albums_set_page(g_ctrls.p_menu);
        albums_help_update(g_ctrls.p_menu, &help_data2);
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(p_friend, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_active, TRUE);
        ctrl_paint_ctrl(p_friend, TRUE);
      //  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return SUCCESS;
     
      case ALBUMS_T_FRIENDS_PHOTOS: 
        pic_stop();
        albums_clear_all(g_ctrls.p_menu);
        
        reset_focus_on_pic_without_paint(1);
        albums_req.request_type = ALBUM_BY_USER_ID;
        memset(albums_req.user_id, 0, sizeof(albums_req.user_id));
        strcpy((s8 *)albums_req.user_id, (s8 *)g_albums_friends.albums_friend[g_friend_focus].user_id);
        ui_albums_get_albums(&albums_req);  

        g_albums_type = ALBUMS_T_FRIENDS_ALBUMS;  
        g_albums_show = 0; 
        albums_help_update(g_ctrls.p_menu, &help_data);
        ctrl_paint_ctrl(p_active, TRUE);
      //  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return SUCCESS;
      case ALBUMS_T_ALBUMS_PHOTOS: 
        pic_stop();
        reset_focus_on_pic_without_paint(1);
        albums_clear_all(g_ctrls.p_menu);
        if(strlen((s8 *)g_albums_community) == 0)
        {
          return ERR_FAILURE;
        }
        memset(albums_req.user_name, 0, sizeof(albums_req.user_name));
        strcpy((s8 *)albums_req.user_name, (s8 *)g_albums_community);
        albums_req.request_type = ALBUM_BY_USER_NAME;
        ui_albums_get_albums(&albums_req);
        g_albums_type = ALBUMS_T_ALBUMS;  
        g_albums_show = 0; 
       // ctrl_process_msg(p_active, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_active, TRUE);
        //ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return SUCCESS;
      case ALBUMS_T_FEATURE:     
      case ALBUMS_T_SEARCH:   
      case ALBUMS_T_ALBUMS:  
        p_type = g_ctrls.p_list;
        albums_help_update(g_ctrls.p_menu, &help_data);
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(p_type, MSG_GETFOCUS, 0, 0);

        ctrl_paint_ctrl(p_active, TRUE);
        ctrl_paint_ctrl(p_type, TRUE);
       // ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return SUCCESS;
      default:
       break;
     }
  }
 return ERR_NOFEATURE;
}

static RET_CODE albums_cont_focus_right(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

static RET_CODE albums_select(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  albums_req_photos_t photos_req;
  albums_t *p_albums = NULL;  
  photos_t *p_photos = NULL;
  control_t *p_active = NULL;
  u8 active_id = 0;
  u16 str_len = 0;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);

  active_id = (u8)ctrl_get_ctrl_id(p_active);  

  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      if(active_id + g_albums_show > p_albums->album_total)
      {
        return ERR_FAILURE;
      }      
      reset_focus_on_pic_without_paint(1);
      pic_stop();
      
      albums_clear_all(g_ctrls.p_menu);
      g_albums_show = 0;
      g_albums_type = ALBUMS_T_ALBUMS_PHOTOS;
      g_focus_album = active_id - 1;
      
      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = 1;
      photos_req.per_page = ALBUMS_PAGE;

      if(p_albums->p_album[active_id - 1].p_album_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[active_id - 1].p_album_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.album_id, p_albums->p_album[active_id - 1].p_album_id, str_len);
         photos_req.album_id[str_len] = '\0';
      }

      if(p_albums->p_album[active_id - 1].p_user_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[active_id - 1].p_user_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.user_id, p_albums->p_album[active_id - 1].p_user_id, str_len);
         photos_req.user_id[str_len] = '\0';
      }  
          
      ui_albums_get_photos(&photos_req);      
      break;
      
    case ALBUMS_T_FRIENDS_ALBUMS:
      p_albums = ui_albums_get_albums_list();

      if(p_albums == NULL)
      {
        return ERR_FAILURE;
      }

      if(active_id + g_albums_show > p_albums->album_total)
      {
        return ERR_FAILURE;
      }      
      pic_stop();
      
      albums_clear_all(g_ctrls.p_menu);
      g_albums_show = 0;
      g_albums_type = ALBUMS_T_FRIENDS_PHOTOS;
      g_focus_album = active_id - 1;
      
      photos_req.request_type = PHOTO_BY_ALBUM;
      photos_req.page = 1;
      photos_req.per_page = ALBUMS_PAGE;

      if(p_albums->p_album[active_id - 1].p_album_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[active_id - 1].p_album_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.album_id, p_albums->p_album[active_id - 1].p_album_id, str_len);
         photos_req.album_id[str_len] = '\0';
      }

      if(p_albums->p_album[active_id - 1].p_user_id != NULL)
      {
         str_len = (u16)strlen(p_albums->p_album[active_id - 1].p_user_id);
         str_len = (str_len >= ALBUMS_ID_LEN) ? (ALBUMS_ID_LEN - 1) : str_len;
         memcpy(photos_req.user_id, p_albums->p_album[active_id - 1].p_user_id, str_len);
         photos_req.user_id[str_len] = '\0';
      }  
          
      ui_albums_get_photos(&photos_req);         
      albums_help_update(g_ctrls.p_menu, &help_data1);
      break;

    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_PHOTOS:
      p_photos = ui_albums_get_photos_list();

      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }

      if(active_id > p_photos->cnt)
      {
        return ERR_FAILURE;
      }

      pic_stop();
      
      albums_clear_all(g_ctrls.p_menu);

      manage_open_menu(ROOT_ID_PHOTO_SHOW,
      (u32)(active_id - 1),
      (u32)p_photos->p_photo[active_id - 1].p_photo_name);    
      break;
      
    default:
      MT_ASSERT(0);
      break;
  }
 // ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  return SUCCESS;
}

static RET_CODE albums_get_photo_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  photos_t *p_photos = NULL;
  control_t *p_active = NULL;
  u8 active_id = 0;


  p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);
   
  active_id = (u8)ctrl_get_ctrl_id(p_active);  
  
  p_photos = ui_albums_get_photos_list();
  ctrl_paint_ctrl(g_ctrls.p_prev_cont, TRUE);
  manage_open_menu(ROOT_ID_PHOTO_INFO, (u32)p_photos, (active_id - 1));
  
  return SUCCESS;
}
static RET_CODE albums_add_friend(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  photos_t *p_photos = NULL;
  control_t *p_active = NULL;
  u8 active_id = 0;

  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_SEARCH:
    case ALBUMS_T_ALBUMS:
      p_active = ctrl_get_active_ctrl(g_ctrls.p_prev_cont);

      active_id = (u8)ctrl_get_ctrl_id(p_active);  

      p_photos = ui_albums_get_photos_list();
      ctrl_paint_ctrl(g_ctrls.p_prev_cont, TRUE);
      if(p_photos == NULL)
      {
        return ERR_FAILURE;
      }

      if(active_id > p_photos->cnt)
      {
        return ERR_FAILURE;
      }

      add_friend((u8 *)p_photos->p_photo[active_id - 1].p_user_id, g_albums_site);
      break;

    default:
      break;
  }
 // ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  return SUCCESS;
}

static RET_CODE ui_albums_update_view_albums(u16 *p_unistr)
{
  albums_req_albums_t albums_req;

  str_uni2asc((u8*)(g_albums_community), p_unistr);
  if(strlen((s8 *)g_albums_community) == 0)
  {
    return ERR_FAILURE;
  }
  memset(&albums_req,0,sizeof(albums_req_albums_t));
  g_albums_show = 0;
  memset(albums_req.user_name, 0, sizeof(albums_req.user_name));
  strcpy((s8 *)albums_req.user_name, (s8 *)g_albums_community);
  albums_req.request_type = ALBUM_BY_USER_NAME;
  ui_albums_get_albums(&albums_req);

    return SUCCESS;
}
static RET_CODE ui_albums_search_picture(u16 *p_unistr)
//static void ui_albums_search_picture()
{
    albums_req_photos_t photos_req;
    
    str_uni2asc((u8*)(g_albums_community), p_unistr);
    if(strlen((s8 *)g_albums_community) == 0)
    {
      return ERR_FAILURE;
    }
    memset(&photos_req,0,sizeof(albums_req_photos_t));
    g_albums_show = 0;
    memset(photos_req.community, 0, sizeof(photos_req.community));
    strcpy((s8 *)photos_req.community, (s8 *)g_albums_community);
    photos_req.request_type = PHOTO_BY_SEARCH;
    photos_req.page = 1;
    photos_req.per_page = ALBUMS_PAGE;
    
    ui_albums_get_photos(&photos_req);

    return SUCCESS;
}

static void albums_search_picture(void)
{ 
    kb_param_t param;
    
    str_asc2uni(g_albums_community, unistr);
    param.uni_str = unistr;
    param.type = KB_INPUT_TYPE_ENGLISH;
    param.max_len = MAX_SEARCH_PICTURE_NAME_LEN;
    param.cb = ui_albums_search_picture;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
}
static void albums_input_user_name(void)
{   
    kb_param_t param;

    str_asc2uni(g_albums_community, unistr);
    param.uni_str = unistr;
    param.type = KB_INPUT_TYPE_ENGLISH;
    param.max_len = ALBUMS_ID_LEN;
    param.cb = ui_albums_update_view_albums;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  
}
static RET_CODE albums_type_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{   
  albums_req_photos_t photos_req;
  control_t *p_friend = NULL;
  u16 focus = 0;
  memset(&photos_req,0,sizeof(albums_req_photos_t));
  focus = list_get_focus_pos(g_ctrls.p_list);
  if(focus == g_albums_type)
  {
     if((g_albums_type != ALBUMS_T_ALBUMS) && (g_albums_type != ALBUMS_T_SEARCH))
    {
        ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
      return SUCCESS;
    }
    
  }

  p_friend = g_ctrls.p_friend_list;

  switch(g_albums_type)
  {
    case ALBUMS_T_FRIENDS:
     // ctrl_set_sts(p_friend, OBJ_STS_HIDE);
     // ctrl_hide_ctrl(p_friend);      
      ctrl_set_sts(p_friend, OBJ_STS_HIDE);
      ctrl_paint_ctrl(p_friend, TRUE);
      break;

    default:    
      break;
  }

  pic_stop();

  albums_clear_all(g_ctrls.p_menu);

  switch(focus)
  {
    case ALBUMS_T_FEATURE:
      photos_req.request_type = PHOTO_BY_FEATURE;
      photos_req.page = 1;
      photos_req.per_page = ALBUMS_PAGE;
      
      ui_albums_get_photos(&photos_req); 

      list_select(g_ctrls.p_list);

      g_albums_show = 0;
      
      g_albums_type = ALBUMS_T_FEATURE;
      break;

    case ALBUMS_T_ALBUMS:

      list_select(p_list);
      ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_list, TRUE);
      albums_input_user_name();

      g_albums_show = 0;
      g_albums_type = ALBUMS_T_ALBUMS;
      break;

    case ALBUMS_T_SEARCH:
      ui_albums_free_photos();
      list_select(p_list);
      ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_list, TRUE);
      albums_search_picture();
      
	  g_albums_show = 0;
      g_albums_type = ALBUMS_T_SEARCH;
      break;

    case ALBUMS_T_FRIENDS:
      ctrl_set_sts(p_friend, OBJ_STS_SHOW);

      list_set_count(p_friend, g_albums_friends.friends_num, ALBUMS_FRIEND_PAGE);
      list_set_focus_pos(p_friend, 0);
      
      albums_friend_list_update(p_friend, list_get_valid_pos(p_friend), ALBUMS_FRIEND_PAGE, 0);

      ctrl_paint_ctrl(p_friend, TRUE);

      list_select(p_list);

      g_albums_show = 0;
      
      g_albums_type = ALBUMS_T_FRIENDS;

      albums_set_page(g_ctrls.p_menu);
      break;

    default:
      MT_ASSERT(0);
      break;        
  }

  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  return SUCCESS;
}

static RET_CODE albums_type_lost_focus(control_t *p_type, u16 msg, u32 para1, u32 para2)
{    
  control_t *p_prew_cont = NULL, *p_item = NULL, *p_friend = NULL;
  u8 sts = OBJ_STS_SHOW;
  

  p_prew_cont = g_ctrls.p_prev_cont;
  p_item = ctrl_get_child_by_id(p_prew_cont, IDC_ALBUMS_ITEM1);
  p_friend = g_ctrls.p_friend_list;

  if(sts != OBJ_STS_SHOW)
  {
    ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
    return ERR_FAILURE;
  }

  switch(g_albums_type)
  {
    case ALBUMS_T_ALBUMS:
    case ALBUMS_T_FEATURE:
    case ALBUMS_T_ALBUMS_PHOTOS:
    case ALBUMS_T_FRIENDS_ALBUMS:
    case ALBUMS_T_FRIENDS_PHOTOS:
    case ALBUMS_T_SEARCH:
      sts = ctrl_get_sts(p_item);
    
      if(sts != OBJ_STS_SHOW)
      {
        ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return ERR_FAILURE;
      }

      if((g_albums_type != ALBUMS_T_FRIENDS_ALBUMS) &&  (g_albums_type != ALBUMS_T_FRIENDS_PHOTOS))
      {
        albums_help_update(g_ctrls.p_menu, &help_data1);
      }
      ctrl_process_msg(g_ctrls.p_list, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);
      
      ctrl_paint_ctrl(g_ctrls.p_list, TRUE);
      ctrl_paint_ctrl(p_item, TRUE);
      break;
    
    case ALBUMS_T_FRIENDS:
      sts = ctrl_get_sts(p_friend);
      if(sts != OBJ_STS_SHOW)
      {
        ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return ERR_FAILURE;
      }

      if(g_albums_friends.friends_num == 0)
      {
        ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
        return ERR_FAILURE;
      }
      albums_help_update(g_ctrls.p_menu, &help_data2);
      ctrl_process_msg(g_ctrls.p_list, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_friend, MSG_GETFOCUS, 0, 0);

      ctrl_paint_ctrl(g_ctrls.p_list, TRUE);
      ctrl_paint_ctrl(p_friend, TRUE);
      
      break;

    default:
      break;
 }
    ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
    
  return SUCCESS;
}

static RET_CODE albums_friend_lost_focus(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{
  control_t *p_type = NULL;
  
  p_type = g_ctrls.p_list;
  ctrl_process_msg(g_ctrls.p_friend_list, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_type, MSG_GETFOCUS, 0, 0);
  albums_help_update(g_ctrls.p_menu, &help_data);
  ctrl_paint_ctrl(g_ctrls.p_friend_list, TRUE);
  ctrl_paint_ctrl(p_type, TRUE);
  ctrl_paint_ctrl(p_type, TRUE);
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE albums_friend_change_focus(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = list_class_proc(g_ctrls.p_friend_list, msg, 0, 0);

  albums_set_page(g_ctrls.p_menu);
  
  return ret;
}

static RET_CODE albums_friend_change_page(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  ret = list_class_proc(g_ctrls.p_friend_list, msg, 0, 0);

  albums_set_page(g_ctrls.p_menu);
  
  return ret;
}

static void albums_del_all_friend(void)
{
  control_t *p_type = NULL;
  control_t *p_friend = NULL;
  //u16 focus = 0;
  u8 i = 0;
  
  p_friend = g_ctrls.p_friend_list;
  //focus = list_get_focus_pos(p_friend);
  for(i =0; i<= g_albums_friends.friends_num; i++)
  {
    del_friend(i);
  }  
  g_albums_friends.friends_num = 0;
  list_set_focus_pos(p_friend, 0);
  list_set_count(p_friend, 0, ALBUMS_FRIEND_PAGE);
  albums_friend_list_update(p_friend, list_get_valid_pos(p_friend), ALBUMS_FRIEND_PAGE, 0);

  ctrl_paint_ctrl(p_friend, TRUE);
  albums_set_page(g_ctrls.p_menu);
  sys_status_set_albums_friends_info(&g_albums_friends);

  p_type = g_ctrls.p_list;
  albums_help_update(g_ctrls.p_menu, &help_data);
  ctrl_process_msg(p_friend, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_type, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_friend, TRUE);
  ctrl_paint_ctrl(p_type, TRUE);
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);

}

static void albums_del_one_friend(void)
{
  control_t *p_friend = NULL;
  
  u16 focus = 0;
  
  p_friend = g_ctrls.p_friend_list;
  focus = list_get_focus_pos(p_friend);

  if(del_friend((u8)focus))
  {
    list_set_count(p_friend, g_albums_friends.friends_num, ALBUMS_FRIEND_PAGE);

    if(focus >= g_albums_friends.friends_num)
    {
      list_set_focus_pos(p_friend, (g_albums_friends.friends_num - 1));
    }
    albums_friend_list_update(p_friend, list_get_valid_pos(p_friend), ALBUMS_FRIEND_PAGE, 0);
    
    ctrl_paint_ctrl(p_friend, TRUE);

    albums_set_page(g_ctrls.p_menu);
    sys_status_set_albums_friends_info(&g_albums_friends);
  }  
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
}

static RET_CODE albums_ask_for_delete_all_friend(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{

   if (g_albums_friends.friends_num > 0)
  {
    ui_comm_ask_for_savdlg_open(NULL, IDS_MSG_ASK_FOR_DEL_ALL,
                                albums_del_all_friend, NULL, 0);
  }
  return SUCCESS;
}

static RET_CODE albums_ask_for_delete_one_friend(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{

   if (g_albums_friends.friends_num > 0)
  {
    ui_comm_ask_for_savdlg_open(NULL, IDS_MSG_ASK_FOR_DEL,
                                albums_del_one_friend, NULL, 0);
  }
  return SUCCESS;
}
static RET_CODE albums_friend_select(control_t *p_friend, u16 msg, u32 para1, u32 para2)
{
  control_t*p_pcont = NULL, *p_item = NULL, *p_title = NULL;
  albums_req_albums_t albums_req;
  
  MT_ASSERT(g_albums_type == ALBUMS_T_FRIENDS);
  memset(&albums_req,0,sizeof(albums_req_albums_t));
  p_title = g_ctrls.p_icon_title;
  p_pcont = g_ctrls.p_prev_cont;
  p_item = ctrl_get_child_by_id(p_pcont, IDC_ALBUMS_ITEM1);

  g_friend_focus = (u8)list_get_focus_pos(p_friend);

  ctrl_set_sts(p_friend, OBJ_STS_HIDE);
  ctrl_process_msg(p_friend, MSG_LOSTFOCUS, 0, 0);
  ctrl_hide_ctrl(p_friend);
  ctrl_erase_ctrl(p_friend);
  ctrl_paint_ctrl(p_friend, TRUE);

  if(g_albums_site != g_albums_friends.albums_friend[g_friend_focus].site)
  {
    pic_stop();
    albums_clear_all(g_ctrls.p_menu);
    ui_pic_release();
    ui_albums_release();

    ui_pic_init(PIC_SOURCE_NET);
    
    if(g_albums_friends.albums_friend[g_friend_focus].site == ALBUMS_PICASA)
    {

	  bmap_set_content_by_id(p_title, IM_PIC_PICASA_LOGO);


      ui_albums_init(ALBUMS_PICASA);
    }
    else if(g_albums_friends.albums_friend[g_friend_focus].site == ALBUMS_YUPOO)
    {
      bmap_set_content_by_id(p_title, IM_PIC_YUPOO_LOGO);

      ui_albums_init(ALBUMS_YUPOO);
    }
    else if(g_albums_friends.albums_friend[g_friend_focus].site == ALBUMS_FLICKR)
    {
      bmap_set_content_by_id(p_title, IM_PIC_FLICKR_LOGO);

      ui_albums_init(ALBUMS_FLICKR);
    }
    else
    {
      MT_ASSERT(0);
    }

    ctrl_paint_ctrl(p_title, TRUE);
    g_albums_site = (albums_site_t)g_albums_friends.albums_friend[g_friend_focus].site;
  }

  g_albums_show = 0;

  albums_req.request_type = ALBUM_BY_USER_ID;
  memset(albums_req.user_id, 0, sizeof(albums_req.user_id));
  strcpy((s8 *)albums_req.user_id, (s8 *)g_albums_friends.albums_friend[g_friend_focus].user_id);
  
  ui_albums_get_albums(&albums_req);  

  g_albums_type = ALBUMS_T_FRIENDS_ALBUMS;  
  albums_help_update(g_ctrls.p_menu, &help_data);
  
  ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(g_ctrls.p_help_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_albums_set_play_mode(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  if((g_albums_type != ALBUMS_T_FRIENDS) && (g_ctrls.p_prev_cont != NULL))
  {
    ctrl_paint_ctrl(g_ctrls.p_prev_cont, TRUE);
  }

  manage_open_menu(ROOT_ID_PIC_PLAY_MODE_SET, ROOT_ID_ALBUMS, 0);

  return SUCCESS;
}

static RET_CODE check_net_conn_sts(void)
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32  
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
     && (eth_connt_stats.is_3g_conn == FALSE) && (eth_connt_stats.is_gprs_conn == FALSE))
  {
     ret = ERR_FAILURE;  
  }
#endif
  return ret;
}
static RET_CODE on_albums_network_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u16 stringid = 0;
  if(ROOT_ID_ALBUMS != fw_get_focus_id())
  {
    return SUCCESS;
  }
  if(check_net_conn_sts() == SUCCESS)
  {
    return SUCCESS;
  }
  switch(msg)
  {
    case MSG_PLUG_OUT:
        if(para2 == HP_WIFI)
  {
            OS_PRINTF("####%s, wifi plug out####\n", __FUNCTION__);
            stringid = IDS_WIFI_DEV_NOT_EXIST;  
            ui_comm_cfmdlg_open(NULL, stringid, exit_album, 2000);
        }
        break;
    case MSG_INTERNET_PLUG_OUT:
        OS_PRINTF("####%s, ethernet plug out####\n", __FUNCTION__);
        stringid = IDS_LAN_DEV_NOT_EXIST;  
    ui_comm_cfmdlg_open(NULL, stringid, exit_album, 2000);
        break;
  }
  return SUCCESS;
}

static RET_CODE albums_exit_menu(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_ALBUMS, 0, 0);
  return SUCCESS;
}

static RET_CODE albums_exit_all(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  ui_close_all_mennus();
  return SUCCESS;
}
BEGIN_KEYMAP(albums_menu_keymap, NULL)
   ON_EVENT(V_KEY_RED, MSG_RED)
   ON_EVENT(V_KEY_BLUE, MSG_BLUE)
   ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(albums_menu_keymap, NULL)

BEGIN_MSGPROC(albums_menu_proc, cont_class_proc)
  ON_COMMAND(MSG_DESTROY, albums_on_destory)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, albums_draw_end)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, albums_draw_end)
  ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, albums_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, albums_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, albums_draw_end)  
  ON_COMMAND(MSG_UPDATE, albums_update)  
  ON_COMMAND(MSG_ALBUMS_EVT_FAILED, albums_failed)
  ON_COMMAND(MSG_ALBUMS_EVT_SUCCESS, albums_success)
  ON_COMMAND(MSG_RED, albums_change_picture_type)  
  ON_COMMAND(MSG_BLUE, on_albums_set_play_mode)  
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_albums_network_plug_out)
  ON_COMMAND(MSG_PLUG_OUT, on_albums_network_plug_out)
  ON_COMMAND(MSG_EXIT, albums_exit_menu)
  ON_COMMAND(MSG_EXIT_ALL, albums_exit_all)
END_MSGPROC(albums_menu_proc, cont_class_proc)

BEGIN_KEYMAP(albums_type_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(albums_type_keymap, NULL)

BEGIN_MSGPROC(albums_type_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, albums_type_lost_focus)
  ON_COMMAND(MSG_SELECT, albums_type_select)
END_MSGPROC(albums_type_proc, list_class_proc)

BEGIN_KEYMAP(albums_friend_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(albums_friend_keymap, NULL)

BEGIN_MSGPROC(albums_friend_proc, list_class_proc)
  ON_COMMAND(MSG_PAGE_UP, albums_friend_change_page)
  ON_COMMAND(MSG_PAGE_DOWN, albums_friend_change_page)
  ON_COMMAND(MSG_FOCUS_UP, albums_friend_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, albums_friend_change_focus)  
  ON_COMMAND(MSG_FOCUS_LEFT, albums_friend_lost_focus)
  ON_COMMAND(MSG_SELECT, albums_friend_select)
  ON_COMMAND(MSG_GREEN, albums_ask_for_delete_one_friend)
  ON_COMMAND(MSG_YELLOW, albums_ask_for_delete_all_friend)
  ON_COMMAND(MSG_EXIT, albums_friend_lost_focus)
END_MSGPROC(albums_friend_proc, list_class_proc)

BEGIN_KEYMAP(albums_prew_cont_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(albums_prew_cont_keymap, NULL)

BEGIN_MSGPROC(albums_prew_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, albums_cont_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, albums_cont_focus_down)
  ON_COMMAND(MSG_FOCUS_LEFT, albums_cont_focus_left)
  ON_COMMAND(MSG_FOCUS_RIGHT, albums_cont_focus_right)
  ON_COMMAND(MSG_SELECT, albums_select)
  ON_COMMAND(MSG_GREEN, albums_add_friend)
  ON_COMMAND(MSG_YELLOW, albums_get_photo_info)
  ON_COMMAND(MSG_EXIT, albums_cont_focus_left)
  ON_COMMAND(MSG_PAGE_DOWN, albums_cont_page_up)
  ON_COMMAND(MSG_PAGE_UP, albums_cont_page_down)
END_MSGPROC(albums_prew_cont_proc, cont_class_proc)

