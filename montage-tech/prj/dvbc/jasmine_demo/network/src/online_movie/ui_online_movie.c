/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
//#include "ui_rename.h"
//#include "lib_char.h"
#if ENABLE_NETWORK
#include "ui_online_movie.h"
//#include "ui_onmov_provider.h"
#include "ui_video.h"
#include "ui_picture_api.h"
#include "ui_onmov_api.h"
#include "netplay.h"

#include "iconv_ext.h"
#include "ui_keyboard_v2.h"

/*!
 Const value
 */
//plist list container
#define ONMOV_LIST_CONTX   120
#define ONMOV_LIST_CONTY   100
#define ONMOV_LIST_CONTW  170
#define ONMOV_LIST_CONTH   570

//bmp
#define ONMOV_BMP_X   0
#define ONMOV_BMP_Y   0
#define ONMOV_BMP_W  ONMOV_LIST_CONTW
#define ONMOV_BMP_H   130

//List
#define ONMOV_LIST_X  20
#define ONMOV_LIST_Y  130
#define ONMOV_LISTW  130
#define ONMOV_LISTH  400

#define ONMOV_LIST_MIDL  5////4
#define ONMOV_LIST_MIDT  5//4
#define ONMOV_LIST_MIDW  (ONMOV_LISTW - 2 * ONMOV_LIST_MIDL)
#define ONMOV_LIST_MIDH  (ONMOV_LISTH - 2 * ONMOV_LIST_MIDT)

#define ONMOV_LIST_ICON_VGAP 4
#define ONMOV_LIST_ITEM_NUM_ONE_PAGE  8


//page container
#define ONMOV_PAGE_CONTX 300
#define ONMOV_PAGE_CONTY 100
#define ONMOV_PAGE_CONTW 860
#define ONMOV_PAGE_CONTH 50

//page arrow left
#define ONMOV_PAGE_ARROWL_X       150
#define ONMOV_PAGE_ARROWL_Y       7
#define ONMOV_PAGE_ARROWL_W      36
#define ONMOV_PAGE_ARROWL_H       36
//page arrow right
#define ONMOV_PAGE_ARROWR_X       (ONMOV_PAGE_CONTW - ONMOV_PAGE_ARROWL_X - ONMOV_PAGE_ARROWR_W)
#define ONMOV_PAGE_ARROWR_Y       7
#define ONMOV_PAGE_ARROWR_W      36
#define ONMOV_PAGE_ARROWR_H       36
//page name
#define ONMOV_PAGE_NAME_X  250
#define ONMOV_PAGE_NAME_Y  7
#define ONMOV_PAGE_NAME_W 100
#define ONMOV_PAGE_NAME_H  36
//page value
#define ONMOV_PAGE_VALUE_X  450
#define ONMOV_PAGE_VALUE_Y  7
#define ONMOV_PAGE_VALUE_W 100
#define ONMOV_PAGE_VALUE_H  36

//movie list
#define ONMOV_MOVLIST_CONTX 300//(ONMOV_LIST_CONTX + ONMOV_LIST_CONTW)
#define ONMOV_MOVLIST_CONTY 160//(ONMOV_PAGE_CONTY + ONMOV_PAGE_CONTH)
#define ONMOV_MOVLIST_CONTW 860//(COMM_BG_W - ONMOV_MOVLIST_CONTX - ONMOV_LIST_CONTX)
#define ONMOV_MOVLIST_CONTH 450

#define ONMOV_MOVLIST_ITEM_X 15//0
#define ONMOV_MOVLIST_ITEM_Y 0
#define ONMOV_MOVLIST_ITEM_W 150//195//200
#define ONMOV_MOVLIST_ITEM_H 220

#define ONMOV_MOVLIST_ITEM_HGAP 20
#define ONMOV_MOVLIST_ITEM_VGAP 10

#define ONMOV_MOVPIC_X   5// 10
#define ONMOV_MOVPIC_Y  5//  10
#define ONMOV_MOVPIC_W    140
#define ONMOV_MOVPIC_H    170

#define ONMOV_MOVNAME_X (ONMOV_MOVPIC_X)
#define ONMOV_MOVNAME_Y (ONMOV_MOVPIC_Y + ONMOV_MOVPIC_H + 5)
#define ONMOV_MOVNAME_W (ONMOV_MOVPIC_W)
#define ONMOV_MOVNAME_H (35)

#define NUM_MOVIE_PER_PAGE  10

//bottom help container
#define ONMOV_BOTTOM_HELP_X  300
#define ONMOV_BOTTOM_HELP_Y  620
#define ONMOV_BOTTOM_HELP_W  860
#define ONMOV_BOTTOM_HELP_H  50
#define ONMOV_MAX_SEARCH_NAME_LEN     (32)

/*!
 * Video list style
 */
static list_xstyle_t cate_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_SH,
  RSI_ITEM_2_HL,
};

static list_xstyle_t cate_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_xstyle_t cate_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};
static const list_field_attr_t cate_list_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR,
        120,//162,
        0,
        0,
        &cate_list_field_rstyle,
        &cate_list_field_fstyle
    }
};

static comm_help_data_t movie_help_data =
{
    4,4,
    {IDS_SELECT, IDS_MOVE, IDS_MENU, IDS_SEARCH},
    {IM_HELP_OK, IM_HELP_ARROW, IM_HELP_MENU, IM_HELP_YELLOW}
};


/*!
 * Type define
 */

/*!
 * Video view message
 */
typedef enum
{
    MSG_MOVIE_SEARCH = MSG_LOCAL_BEGIN + 0x600,
}ui_onmov_msg_t;

/*!
 * Video view control id
 */
typedef enum
{
    IDC_ONMOV_LIST_CONT = 1,
    IDC_ONMOV_LEFT_BMP,  
    IDC_ONMOV_LIST,
    IDC_ONMOV_PAGE_CONT,
    IDC_ONMOV_PAGE_NUM,

    IDC_ONMOV_MOVLIST_CONT,
    IDC_ONMOV_MOVITEM_START,
    IDC_ONMOV_MOVITEM_END = IDC_ONMOV_MOVITEM_START + NUM_MOVIE_PER_PAGE - 1,
    IDC_ONMOV_MOVITEM_PIC,
    IDC_ONMOV_MOVITEM_NAME,

    IDC_ONMOV_BTM_HELP,
}ui_onmov_ctrl_id_t;

/*!
 Macro
 */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


static netplay_website_info_t*p_curWebsite = NULL;
static s16 g_curCatelistIndex = -1;
static s16 g_curMovlistIndex = 0;
static netplay_media_info_t  *onmove_media  = NULL;
static int sg_media_cnt = 0;
static u8 curWebsitesIndex = 0;
static s16 sg_cur_page_index = 0;
static u16 sg_uni_str_tab[10][64] = {{0}}; //preserve movie names of current page
static u8 sg_draw_end_flag[NUM_MOVIE_PER_PAGE] = {0};
static s16 sg_cur_draw_index = -1;
static BOOL sg_search_flag = FALSE;
static u16 g_searchKey[ONMOV_MAX_SEARCH_NAME_LEN+1] = {0};
static BOOL sg_b_wait_search_result = FALSE;

extern iconv_t g_cd_utf8_to_utf16le;

/*!
 * Function define
 */
u16 onmov_catelist_keymap(u16 key);
RET_CODE onmov_catelist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 onmov_movlist_keymap(u16 key);
RET_CODE onmov_movlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 onmov_cont_keymap(u16 key);
RET_CODE onmov_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern netplay_server_info_t* ui_get_server_info(void);


char* get_cur_web_name()
{
  return p_curWebsite->name;
}

/*================================================================================================
                           video play view internel function
 ================================================================================================*/
static void onmov_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void onmov_pic_deinit(void)
{
  pic_stop();
  ui_pic_release();
}

static void onmov_update_page_num(control_t* p_ctrl, u16 page_idx)
{
  char num[20] = {0};
  u16 page = (p_curWebsite->p_cate[g_curCatelistIndex].media_count
  	+ NUM_MOVIE_PER_PAGE - 1) / NUM_MOVIE_PER_PAGE;
  u16 cur = page_idx + 1;

  sprintf(num, "%d/%d", cur, page);
  text_set_content_by_ascstr(p_ctrl, num);
  ctrl_paint_ctrl(p_ctrl, TRUE);
}

static void onmov_set_page(control_t* p_ctrl, u16 page_idx, u16 page_cnt)
{
  char num[20] = {0};
  u16 cur = page_idx + 1;

  sprintf(num, "%d/%d", cur, page_cnt);
  text_set_content_by_ascstr(p_ctrl, num);
  ctrl_paint_ctrl(p_ctrl, TRUE);
}

/*
static void hex_dump(u8* addr, u32 len)
{
  u32 i;
  u8 col_cnt = 0;
  for( i = 0; i < len; i++ )
  {
    OS_PRINTF("0x%02x,", addr[i]);
    col_cnt++;
    if(16 == col_cnt)
    {
      col_cnt =0;
      OS_PRINTF("\n");
    }
  }
  OS_PRINTF("\n");
}
*/

static RET_CODE onmov_update_catelist(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    u16 uni_str[128];
    cnt = list_get_count(p_list);
    OS_PRINTF("%s\n", __FUNCTION__);
    for (i = start; i < start + size && i < cnt; i++)
    {
        char* inbuf = p_curWebsite->p_cate[i].name;
	 char* outbuf = (char*)uni_str;
        int src_len =  strlen(inbuf) + 1;
	 int dest_len = sizeof(sg_uni_str_tab[i]);

        iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
        list_set_field_content_by_unistr(p_list, i, 0, uni_str);
    }

    return SUCCESS;
}

static control_t *ui_creat_movie_list(control_t *p_menu)
{
    control_t *p_list_cont = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    u16 i;
    u16 x, y;

    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_MOVLIST_CONT,
                                  ONMOV_MOVLIST_CONTX, ONMOV_MOVLIST_CONTY,
                                  ONMOV_MOVLIST_CONTW, ONMOV_MOVLIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    x = ONMOV_MOVLIST_ITEM_X;
    y = ONMOV_MOVLIST_ITEM_Y;
    for (i = 0; i < NUM_MOVIE_PER_PAGE; i++)
    {
      p_item = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_MOVITEM_START + i,
                              x, y,
                              ONMOV_MOVLIST_ITEM_W, ONMOV_MOVLIST_ITEM_H,
                              p_list_cont, 0);
      ctrl_set_rstyle(p_item, RSI_MV_POSTER_FRM_SH, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_SH);

      p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ONMOV_MOVITEM_PIC,
                              ONMOV_MOVPIC_X, ONMOV_MOVPIC_Y,
                              ONMOV_MOVPIC_W, ONMOV_MOVPIC_H,
                              p_item, 0);
      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

      p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_MOVITEM_NAME,
                              ONMOV_MOVNAME_X, ONMOV_MOVNAME_Y,
                              ONMOV_MOVNAME_W, ONMOV_MOVNAME_H,
                              p_item, 0);
//      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
      ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

      text_set_font_style(p_ctrl, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
      text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
      x += ONMOV_MOVLIST_ITEM_W + ONMOV_MOVLIST_ITEM_HGAP;
      if (x + ONMOV_MOVPIC_W > ONMOV_MOVLIST_CONTW)
      {
        x = ONMOV_MOVLIST_ITEM_X;
        y += ONMOV_MOVLIST_ITEM_H + ONMOV_MOVLIST_ITEM_VGAP;
      }
    }

    return p_list_cont;
}

static RET_CODE onmov_update_movlist(control_t* p_list_cont, u16 start, u16 size, u32 context)
{
    control_t *p_item = NULL;
    control_t *p_pic, *p_text;
    u16 i;//, cnt;
    netplay_media_info_t *p_movie;
    //u16 uni_str[128] = {0};
    p_movie =  onmove_media;
    memset(sg_uni_str_tab, 0, sizeof(sg_uni_str_tab));
    for (i = 0; i < size; i++)
    {
      p_item = ctrl_get_child_by_id(p_list_cont, IDC_ONMOV_MOVITEM_START + i);
      p_pic = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_PIC);
      p_text = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_NAME);
      if (start + i < sg_media_cnt)
      {
        char* inbuf = p_movie[start + i].name;
	 char* outbuf = (char*)(sg_uni_str_tab + i);
        int src_len =  strlen(inbuf);
	 int dest_len = sizeof(sg_uni_str_tab[i]);
	 
        iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);		
        text_set_content_by_extstr(p_text, sg_uni_str_tab[i]);
        bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
      }
      else
      {
        bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
        text_set_content_by_extstr(p_text, NULL);
      }
      ctrl_paint_ctrl(p_text, TRUE);
      ctrl_paint_ctrl(p_pic, TRUE);
    }


if(1)
{
  {
    void *p_disp = NULL;
    void *p_gpe = NULL;
    void *p_rgn = NULL;
    rect_t rect;
    
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);
    p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_gpe);
    p_rgn = ui_get_pic_region();
    MT_ASSERT(NULL != p_rgn);

    ctrl_get_frame(p_list_cont, &rect);
    ctrl_client2screen(p_list_cont, &rect);

    gpe_draw_rectangle_vsb(p_gpe, p_rgn, &rect, 0);
    disp_layer_update_region(p_disp, p_rgn, NULL);
  }
}
    return SUCCESS;
}

static RET_CODE onmov_clear_movlist(control_t* p_list_cont,  u16 size)
{
    control_t *p_item = NULL;
    control_t *p_pic, *p_text;
    u16 i;

    for (i = 0; i < size; i++)
    {
      p_item = ctrl_get_child_by_id(p_list_cont, IDC_ONMOV_MOVITEM_START + i);
      p_pic = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_PIC);
      p_text = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_NAME);

      bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
      text_set_content_by_extstr(p_text, NULL);
    }
    
    return SUCCESS;
}


static void onmov_set_focus(control_t *p_cont, s16 last_focus, s16 focus)
{
  control_t* p_item = NULL;
  control_t* p_text = NULL;

  if( last_focus < 0 )
    ;
  else
  {
    p_item = ctrl_get_child_by_id(p_cont, IDC_ONMOV_MOVITEM_START + last_focus);
    p_text = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_NAME);
    text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
    ctrl_process_msg(p_item, MSG_LOSTFOCUS, 0, 0);
    
  }

  p_item = ctrl_get_child_by_id(p_cont, IDC_ONMOV_MOVITEM_START + focus);
  p_text = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_NAME);
  text_set_font_style(p_text, FSI_BLACK_16, FSI_BLACK_16, FSI_BLACK_16);
  
  ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);
}

#if 1
static BOOL onmov_movlist_set_focus_pos(control_t *p_list_cont, u16 focus)
{
  control_t *p_active = NULL, *p_next_ctrl = NULL;
  u16 index;

  p_active = p_list_cont->p_active_child;
  if(p_active != NULL)
  {
    ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
  }

  index = focus % NUM_MOVIE_PER_PAGE;
  p_next_ctrl = ctrl_get_child_by_id(p_list_cont, IDC_ONMOV_MOVITEM_START + index);
  if (p_next_ctrl != NULL)
  {
    //ctrl_set_attr(p_next_ctrl, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_next_ctrl, OBJ_STS_SHOW);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  }

  ctrl_paint_ctrl(p_next_ctrl, TRUE);

  return TRUE;
}
#endif
static RET_CODE onmov_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;

  p_root = fw_find_root_by_id(ROOT_ID_ONMOV_WEBSITES);

  if (p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  return ERR_NOFEATURE;
}

//static RET_CODE onmov_on_pic_play_start(control_t *p_cont, u16 msg, u32 para1, u32 para2)
static RET_CODE draw_movie_icon(control_t *p_cont, u16 movie_index)
{
  control_t *p_item = NULL;
  control_t *p_pic = NULL;
  rect_t rect;
  netplay_media_info_t *p_movie = onmove_media;
OS_PRINTF("eeeeeeeeeeeeeeenter%s\n", __FUNCTION__);
OS_PRINTF("movie_index = %d\n", movie_index);
  p_item = ctrl_get_child_by_id(p_cont, IDC_ONMOV_MOVITEM_START + movie_index);
  p_pic = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_PIC);

  ctrl_get_frame(p_pic, &rect);
  ctrl_client2screen(p_pic, &rect);

  if( p_movie[movie_index].p_logo_url )
  {
    u32 identity = 0;
    sg_cur_draw_index = movie_index;
    identity = (curWebsitesIndex<<24) 
  	           | ((g_curCatelistIndex&0XFF)<<16)
  	           | sg_cur_draw_index;
	
    ui_pic_play_by_url(p_movie[movie_index].p_logo_url, &rect, identity);
    //bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    //ctrl_paint_ctrl(p_pic, TRUE);
  }
OS_PRINTF("eeeeeeeeeeeeeeeexit%s\n", __FUNCTION__);

  return SUCCESS;
}

static RET_CODE onmov_on_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i = 0;
  u32 identity = 0;
  control_t* p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
  control_t* p_mlist = ctrl_get_child_by_id(p_menu, IDC_ONMOV_MOVLIST_CONT);
  control_t* p_item = NULL;
  control_t* p_pic = NULL;
  static u8 err_cnt;
OS_PRINTF("xxxxxxxxx pic %d GOT MSG_PIC_EVT_DRAW_END msg\n", sg_cur_draw_index);
  if( ROOT_ID_NETWORK_PLAY != fw_get_focus_id() )
    return SUCCESS;

  identity = (curWebsitesIndex<<24) 
  	         | ((g_curCatelistIndex&0XFF)<<16)
  	         | sg_cur_draw_index;

  if( para1 != identity && FALSE == sg_search_flag )
  {
    return SUCCESS;
  }
  if( MSG_PIC_EVT_DRAW_END == msg 
    ||(MSG_PIC_EVT_DATA_ERROR == msg && MAX_RETRY_CNT == err_cnt ))
  {
    if( MSG_PIC_EVT_DRAW_END == msg )
    {
  p_item = ctrl_get_child_by_id(p_mlist, IDC_ONMOV_MOVITEM_START + sg_cur_draw_index);
  p_pic = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_PIC);

  bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
  ctrl_paint_ctrl(p_pic, TRUE);
    }
    err_cnt = 0;
  sg_draw_end_flag[sg_cur_draw_index] = 1;
  pic_stop();
  
  for( i = 0; i < sg_media_cnt; i++ )
    if(0 == sg_draw_end_flag[i])
      break;
  
  if( i < sg_media_cnt )
  {
    sg_cur_draw_index = i;
    draw_movie_icon(p_mlist, i);
  }
  }
  else if( MSG_PIC_EVT_DATA_ERROR == msg )
  {
    OS_PRINTF("%s(), RETRY!!\n", __FUNCTION__);
    err_cnt++;
    pic_stop();
    draw_movie_icon(p_mlist, sg_cur_draw_index);
  }
  return SUCCESS;
}


static RET_CODE onmov_on_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i;
  control_t* p_mlist = ctrl_get_child_by_id(p_cont, IDC_ONMOV_MOVLIST_CONT);
  control_t* p_item = NULL;
  control_t* p_pic = NULL;

  for( i = 0; i < NUM_MOVIE_PER_PAGE; i++ )
  {
    p_item = ctrl_get_child_by_id(p_mlist, IDC_ONMOV_MOVITEM_START + i);
    p_pic = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_PIC);
    bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
  }
  ctrl_paint_ctrl(p_mlist, TRUE);

  onmov_pic_init();

  OS_PRINTF("*******ENTER %s\n", __FUNCTION__);
  draw_movie_icon(p_mlist, 0);
  OS_PRINTF("*******EXIT %s\n", __FUNCTION__);
  return SUCCESS;
}
/*!
 * Video view entry
 */
RET_CODE ui_open_online_movie(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_bmp = NULL;
    control_t *p_list_cont = NULL, *p_list = NULL;
    control_t *p_page_cont = NULL;
    control_t *p_icon = NULL, *p_ctrl = NULL;
    control_t *p_movlist_cont = NULL;
    control_t *p_bottom_help = NULL;
    u8 i = 0;
    onmov_pic_init();


    p_curWebsite = (netplay_website_info_t*) para1;
    g_curCatelistIndex = 0;
    g_curMovlistIndex = 0;
    sg_cur_page_index = 0;
    curWebsitesIndex = para2;
  
    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_NETWORK_PLAY, 0,
                                    IM_INDEX_NETWORKPLAY_BANNER, IDS_NETWORK_PLAY);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, onmov_cont_keymap);
    ctrl_set_proc(p_menu, onmov_cont_proc);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_BTM_HELP,
                                      ONMOV_BOTTOM_HELP_X, ONMOV_BOTTOM_HELP_Y,
                                      ONMOV_BOTTOM_HELP_W, ONMOV_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(120, 0, ONMOV_BOTTOM_HELP_W-120, ONMOV_BOTTOM_HELP_H,  &movie_help_data,  p_bottom_help);

    /*!
     * Create list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_LIST_CONT,
                                  ONMOV_LIST_CONTX, ONMOV_LIST_CONTY,
                                  ONMOV_LIST_CONTW, ONMOV_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_ONMOV_LEFT_BMP, ONMOV_BMP_X, ONMOV_BMP_Y, 
      ONMOV_BMP_W, ONMOV_BMP_H, p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, IM_ICON_NETWORK_MOVIE);
    
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_ONMOV_LIST,
                          ONMOV_LIST_X, ONMOV_LIST_Y,
                          ONMOV_LISTW, ONMOV_LISTH,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, onmov_catelist_keymap);
    ctrl_set_proc(p_list, onmov_catelist_proc);

    ctrl_set_mrect(p_list, ONMOV_LIST_MIDL, ONMOV_LIST_MIDT,
                  ONMOV_LIST_MIDW + ONMOV_LIST_MIDL, ONMOV_LIST_MIDH + ONMOV_LIST_MIDT);

    list_set_item_interval(p_list, ONMOV_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &cate_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, p_curWebsite->cate_count, ONMOV_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(cate_list_attr), ONMOV_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, onmov_update_catelist, 0);
    //ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

    for (i = 0; i < ARRAY_SIZE(cate_list_attr); i++)
    {
        list_set_field_attr(p_list,
                            i,
                            cate_list_attr[i].attr,
                            cate_list_attr[i].width,
                            cate_list_attr[i].left,
                            cate_list_attr[i].top);

        list_set_field_rect_style(p_list, i, cate_list_attr[i].rstyle);
        list_set_field_font_style(p_list, i, cate_list_attr[i].fstyle);
    }

    onmov_update_catelist(p_list, list_get_valid_pos(p_list), ONMOV_LIST_ITEM_NUM_ONE_PAGE, 0);

    //Page
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_PAGE_CONT,
                                    ONMOV_PAGE_CONTX, ONMOV_PAGE_CONTY,
                                    ONMOV_PAGE_CONTW, ONMOV_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               ONMOV_PAGE_ARROWL_X, ONMOV_PAGE_ARROWL_Y,
                               ONMOV_PAGE_ARROWL_W, ONMOV_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               ONMOV_PAGE_ARROWR_X, ONMOV_PAGE_ARROWR_Y,
                               ONMOV_PAGE_ARROWR_W, ONMOV_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 ONMOV_PAGE_NAME_X, ONMOV_PAGE_NAME_Y,
                                 ONMOV_PAGE_NAME_W, ONMOV_PAGE_NAME_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_ctrl, IDS_PAGE1);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_ONMOV_PAGE_NUM,
                                 ONMOV_PAGE_VALUE_X, ONMOV_PAGE_VALUE_Y,
                                 ONMOV_PAGE_VALUE_W, ONMOV_PAGE_VALUE_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    onmov_update_page_num(p_ctrl, 0);

    //Movie list
    p_movlist_cont = ui_creat_movie_list(p_menu);
    ctrl_set_keymap(p_movlist_cont, onmov_movlist_keymap);
    ctrl_set_proc(p_movlist_cont, onmov_movlist_proc);

    ctrl_process_msg(p_movlist_cont, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
  do
  { //req media info
    int identify = 0;
    netplay_website_info_t* p_web = (netplay_website_info_t*)para1;
    int web_idx = para2;
    identify = web_idx<<24 | 0;
    OS_PRINTF("xuhd, request(%d,%d,%d)\n", web_idx, 0, 0);

    sg_search_flag = FALSE;
    ui_onmov_media_request(p_web->name, p_web->p_cate[0].name,0, identify);
  }while(0);

  return SUCCESS;
}

static void clear_draw_end_flag(void)
{
  u8 i;
  sg_cur_draw_index = 0;
  for( i = 0; i < NUM_MOVIE_PER_PAGE; i++ )
  {
    sg_draw_end_flag[i] = 0;
  }
}

static RET_CODE onmov_on_catelist_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  //control_t* p_root = fw_find_root_by_id(ROOT_ID_ONMOV_WEBSITES);
  sg_media_cnt  = 0;

  clear_draw_end_flag();
  onmov_pic_deinit();
/*
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
*/  
  return ERR_NOFEATURE;
}

static RET_CODE onmov_on_catelist_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  sg_media_cnt  = 0;

  clear_draw_end_flag();
  onmov_pic_deinit();

  return ERR_NOFEATURE;
}

static RET_CODE onmov_on_catelist_updown(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 focus = 0;
  int identify = 0;
  control_t *p_menu = NULL;
  control_t *p_movlist_cont = NULL;
  control_t *p_page_cont, *p_ctrl;

  if( 1 == p_curWebsite->cate_count )
    return SUCCESS;

  list_class_proc(p_list, msg, para1, para2);
  focus = list_get_focus_pos(p_list);
  g_curCatelistIndex = focus;
  g_curMovlistIndex = 0;
  clear_draw_end_flag();
  
   p_menu =  ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
   p_page_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_PAGE_CONT);
   p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_ONMOV_PAGE_NUM);
   p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_MOVLIST_CONT);
   onmov_update_page_num(p_ctrl, 0);
   onmov_clear_movlist(p_movlist_cont,NUM_MOVIE_PER_PAGE);
   ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  identify = ((curWebsitesIndex&0xFF)<<24) 
  	         | ((g_curCatelistIndex&0XFF)<<16)
  	         | 0;
  OS_PRINTF("xuhd, request(%d,%d,%d)\n", curWebsitesIndex, g_curCatelistIndex, 0);

  sg_search_flag = FALSE;
  ui_onmov_media_request(p_curWebsite->name,
      p_curWebsite->p_cate[g_curCatelistIndex].name,
      0, identify);

  return SUCCESS;
}

static RET_CODE onmov_on_catelist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu, *p_movlist_cont;

  if( 0 == sg_media_cnt )
    return SUCCESS;

  p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
  p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_MOVLIST_CONT);
  MT_ASSERT(p_movlist_cont != NULL);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);

#if 0
  ctrl_set_attr(p_movlist_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_movlist_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_movlist_cont, MSG_GETFOCUS, 0, 0);
#endif
  onmov_movlist_set_focus_pos(p_movlist_cont, g_curMovlistIndex);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  return SUCCESS;
}

static RET_CODE onmov_on_movlist_exit(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu, *p_catelist_cont, *p_catelist, *p_item, *p_text;

  p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
  p_catelist_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_LIST_CONT);
  p_catelist = ctrl_get_child_by_id(p_catelist_cont, IDC_ONMOV_LIST);
  p_item = ctrl_get_child_by_id(p_movlist_cont, IDC_ONMOV_MOVITEM_START+g_curMovlistIndex);
  p_text = ctrl_get_child_by_id(p_item, IDC_ONMOV_MOVITEM_NAME);
  text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
  
  ctrl_process_msg(p_movlist_cont, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_catelist, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_catelist, OBJ_STS_SHOW);
  ctrl_process_msg(p_catelist, MSG_GETFOCUS, 0, 0);

  //clear_draw_end_flag();

  ctrl_paint_ctrl(p_menu, TRUE);

  return SUCCESS;
}


static RET_CODE onmov_change_page(control_t* p_cont, u16 msg, u32 para1, u32 para2)
{
  int identify = 0;
  int page_cnt = (p_curWebsite->p_cate[g_curCatelistIndex].media_count 
  	+ NUM_MOVIE_PER_PAGE - 1) / NUM_MOVIE_PER_PAGE;

  if( NULL == p_curWebsite )
    return SUCCESS;

  onmov_pic_deinit();
  onmov_pic_init();
  
  clear_draw_end_flag();
  switch( msg )
  {
    case MSG_PAGE_UP:
      sg_cur_page_index ++;
    break;
    case MSG_PAGE_DOWN:
      sg_cur_page_index --;
    break;
  }

  sg_cur_page_index = (sg_cur_page_index+page_cnt)%page_cnt;

  identify = ((curWebsitesIndex&0xFF)<<24) 
  	         | ((g_curCatelistIndex&0XFF)<<16) 
  	         | (sg_cur_page_index&0XFFFF);

  OS_PRINTF("xuhd, request(%d,%d,%d)\n", curWebsitesIndex, g_curCatelistIndex, sg_cur_page_index);

  sg_search_flag = FALSE;
  ui_onmov_media_request(p_curWebsite->name,
  	p_curWebsite->p_cate[g_curCatelistIndex].name,
  	sg_cur_page_index, identify);

  sg_cur_draw_index = 0;

  return SUCCESS;
}

static RET_CODE onmov_on_focus_move(control_t* p_cont, u16 msg, u32 para1, u32 para2)
{
  s8 row = g_curMovlistIndex/5;
  s8 col = g_curMovlistIndex%5;
  s16 old_focus = -1;

  switch(msg)
  {
    case MSG_FOCUS_LEFT:
      col--;
    break;
    case MSG_FOCUS_RIGHT:
      col++;
    break;
    case MSG_FOCUS_UP:
      row--;
    break;
    case MSG_FOCUS_DOWN:
      row++;
    break;
  }
  if(5==col)
    row++;
  if( -1 == col )
    row--;

  col = (col+5)%5;
  old_focus = g_curMovlistIndex;
  if( 2 == row && MSG_FOCUS_RIGHT == msg )
    ctrl_process_msg(p_cont, MSG_PAGE_UP, 0, 0);
  if( -1 == row && MSG_FOCUS_LEFT == msg )
    ctrl_process_msg(p_cont, MSG_PAGE_DOWN, 0, 0);

  row = (row+2)%2;
  g_curMovlistIndex = 5*row + col;
  onmov_set_focus(p_cont, old_focus, g_curMovlistIndex);
  
  return SUCCESS;
}


//enter movlist details
static RET_CODE onmov_on_movlist_select(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  if( NULL == p_curWebsite )
    return SUCCESS;
  if( g_curMovlistIndex >= sg_media_cnt )
    return SUCCESS; //focus on a cell with no movie

  clear_draw_end_flag();
  onmov_pic_deinit();

  manage_open_menu(ROOT_ID_ONMOV_DESCRIPTION, (u32)&(onmove_media[g_curMovlistIndex]), 0);

  return SUCCESS;
}


static RET_CODE onmov_get_media(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  control_t *p_movlist_cont = NULL;
  control_t *p_page_cont, *p_ctrl;
  u16 page_idx = 0;
  onmove_media = (netplay_media_info_t*) para1;
  sg_media_cnt = para2&0xffff;
  page_idx = para2>>16;

  if(sg_media_cnt == 0)
  {
    memset(onmove_media,0,sizeof(netplay_media_info_t));
  }

  p_menu =  ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
  p_page_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_PAGE_CONT);
  p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_ONMOV_PAGE_NUM);


  p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_ONMOV_MOVLIST_CONT);

  if( sg_b_wait_search_result == TRUE )
  {
    sg_b_wait_search_result = FALSE;
    onmov_set_page(p_ctrl, 0, 1);
  }
  else
  {
  onmov_update_page_num(p_ctrl, page_idx);
  }
  
  onmov_update_movlist(p_movlist_cont,
                                       0,
                                       NUM_MOVIE_PER_PAGE,
                                       0);
  draw_movie_icon(p_movlist_cont, 0);

  return SUCCESS;
}

RET_CODE onmov_on_search_input_completed(u16 *p_unistr)
{
  control_t *p_root = NULL;
  u32 identity;
  u8  asc_str[ONMOV_MAX_SEARCH_NAME_LEN*3+1];

  if (uni_strlen(g_searchKey) > 0)
  {
    str_uni2asc(asc_str, g_searchKey);
    identity = (curWebsitesIndex<<24) | ((g_curCatelistIndex&0XFF)<<16) | 0;
    ui_onmov_media_search(get_cur_web_name(), "", asc_str, identity);

    p_root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAY);
    if (p_root != NULL)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_MOVIE_SEARCH_UPDATE, 0, 0);
    }
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
  }
  return SUCCESS;
}
static RET_CODE on_search_button(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  kb_param_t param;

  clear_draw_end_flag();
  sg_search_flag = TRUE;
  sg_cur_draw_index = 0;

  param.uni_str = g_searchKey;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = ONMOV_MAX_SEARCH_NAME_LEN;
  param.cb = onmov_on_search_input_completed;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, ROOT_ID_NETWORK_PLAY, (u32)&param);
  return SUCCESS;
}

static RET_CODE onmov_search_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t* p_mlist = ctrl_get_child_by_id(p_cont, IDC_ONMOV_MOVLIST_CONT);

  sg_b_wait_search_result = TRUE;
  onmov_clear_movlist(p_mlist, NUM_MOVIE_PER_PAGE);
  return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(onmov_catelist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_YELLOW, MSG_MOVIE_SEARCH)
END_KEYMAP(onmov_catelist_keymap, NULL)

BEGIN_MSGPROC(onmov_catelist_proc, list_class_proc)
    ON_COMMAND(MSG_EXIT, onmov_on_catelist_exit)
    ON_COMMAND(MSG_EXIT_ALL, onmov_on_catelist_exit_all)
    ON_COMMAND(MSG_FOCUS_UP, onmov_on_catelist_updown)
    ON_COMMAND(MSG_FOCUS_DOWN, onmov_on_catelist_updown)
    ON_COMMAND(MSG_FOCUS_RIGHT, onmov_on_catelist_change_focus)
    ON_COMMAND(MSG_ONMOV_EVT_MEDIA, onmov_get_media)  
    ON_COMMAND(MSG_MOVIE_SEARCH, on_search_button)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, onmov_on_pic_draw_end)
END_MSGPROC(onmov_catelist_proc, list_class_proc)

BEGIN_KEYMAP(onmov_movlist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP,MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN,MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_YELLOW, MSG_MOVIE_SEARCH)
END_KEYMAP(onmov_movlist_keymap, NULL)

BEGIN_MSGPROC(onmov_movlist_proc, cont_class_proc)
    ON_COMMAND(MSG_EXIT, onmov_on_movlist_exit)
    ON_COMMAND(MSG_EXIT_ALL, onmov_on_movlist_exit)
    ON_COMMAND(MSG_FOCUS_LEFT, onmov_on_focus_move)
    ON_COMMAND(MSG_FOCUS_RIGHT, onmov_on_focus_move)
    ON_COMMAND(MSG_FOCUS_UP, onmov_on_focus_move)
    ON_COMMAND(MSG_FOCUS_DOWN, onmov_on_focus_move)
    ON_COMMAND(MSG_SELECT, onmov_on_movlist_select)
    ON_COMMAND(MSG_PAGE_UP, onmov_change_page)
    ON_COMMAND(MSG_PAGE_DOWN, onmov_change_page)
    ON_COMMAND(MSG_ONMOV_EVT_MEDIA, onmov_get_media)  
//    ON_COMMAND(MSG_PIC_EVT_DRAW_END, onmov_on_pic_draw_end)
    ON_COMMAND(MSG_MOVIE_SEARCH, on_search_button)
END_MSGPROC(onmov_movlist_proc, cont_class_proc)

BEGIN_KEYMAP(onmov_cont_keymap, ui_comm_root_keymap)

END_KEYMAP(onmov_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(onmov_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, onmov_on_destory)
  //ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, onmov_on_pic_play_start)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, onmov_on_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, onmov_on_pic_draw_end)
  //ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_picture_too_large)
  //ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_picture_unsupport)  
  ON_COMMAND(MSG_UPDATE, onmov_on_update)  
  ON_COMMAND(MSG_ONMOV_EVT_MEDIA, onmov_get_media)
  ON_COMMAND(MSG_MOVIE_SEARCH_UPDATE, onmov_search_update) 
END_MSGPROC(onmov_cont_proc, ui_comm_root_proc)
#endif
