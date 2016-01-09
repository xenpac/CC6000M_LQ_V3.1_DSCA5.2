/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#if ENABLE_NETWORK
#include "ui_video.h"
#include "ui_picture_api.h"
#include "commonData.h"
#include "youtubeDataProvider.h"
#include "ui_youporn_api.h"

/*!
 Const value
 */
//plist list container
#define YOUPORN_LIST_CONTX   120
#define YOUPORN_LIST_CONTY   100
#define YOUPORN_LIST_CONTW  170
#define YOUPORN_LIST_CONTH   570

//bmp
#define YOUPORN_BMP_X   0
#define YOUPORN_BMP_Y   0
#define YOUPORN_BMP_W  YOUPORN_LIST_CONTW
#define YOUPORN_BMP_H   130

//List
#define YOUPORN_LIST_X  20
#define YOUPORN_LIST_Y  130
#define YOUPORN_LISTW  130
#define YOUPORN_LISTH  400

#define YOUPORN_LIST_MIDL  5////4
#define YOUPORN_LIST_MIDT  5//4
#define YOUPORN_LIST_MIDW  (YOUPORN_LISTW - 2 * YOUPORN_LIST_MIDL)
#define YOUPORN_LIST_MIDH  (YOUPORN_LISTH - 2 * YOUPORN_LIST_MIDT)

#define YOUPORN_LIST_ICON_VGAP 4
#define YOUPORN_LIST_ITEM_NUM_ONE_PAGE  8


//page container
#define YOUPORN_PAGE_CONTX 300
#define YOUPORN_PAGE_CONTY 100
#define YOUPORN_PAGE_CONTW 860
#define YOUPORN_PAGE_CONTH 50

//page arrow left
#define YOUPORN_PAGE_ARROWL_X       150
#define YOUPORN_PAGE_ARROWL_Y       7
#define YOUPORN_PAGE_ARROWL_W      36
#define YOUPORN_PAGE_ARROWL_H       36
//page arrow right
#define YOUPORN_PAGE_ARROWR_X       (YOUPORN_PAGE_CONTW - YOUPORN_PAGE_ARROWL_X - YOUPORN_PAGE_ARROWR_W)
#define YOUPORN_PAGE_ARROWR_Y       7
#define YOUPORN_PAGE_ARROWR_W      36
#define YOUPORN_PAGE_ARROWR_H       36
//page name
#define YOUPORN_PAGE_NAME_X  250
#define YOUPORN_PAGE_NAME_Y  7
#define YOUPORN_PAGE_NAME_W 100
#define YOUPORN_PAGE_NAME_H  36
//page value
#define YOUPORN_PAGE_VALUE_X  450
#define YOUPORN_PAGE_VALUE_Y  7
#define YOUPORN_PAGE_VALUE_W 100
#define YOUPORN_PAGE_VALUE_H  36

//movie list
#define YOUPORN_MOVLIST_CONTX 300//(YOUPORN_LIST_CONTX + YOUPORN_LIST_CONTW)
#define YOUPORN_MOVLIST_CONTY 160//(YOUPORN_PAGE_CONTY + YOUPORN_PAGE_CONTH)
#define YOUPORN_MOVLIST_CONTW 860//(COMM_BG_W - YOUPORN_MOVLIST_CONTX - YOUPORN_LIST_CONTX)
#define YOUPORN_MOVLIST_CONTH 450

#define YOUPORN_MOVLIST_ITEM_X 15//0
#define YOUPORN_MOVLIST_ITEM_Y 0
#define YOUPORN_MOVLIST_ITEM_W 150//195//200
#define YOUPORN_MOVLIST_ITEM_H 220

#define YOUPORN_MOVLIST_ITEM_HGAP 20
#define YOUPORN_MOVLIST_ITEM_VGAP 10

#define YOUPORN_MOVPIC_X   5// 10
#define YOUPORN_MOVPIC_Y  5//  10
#define YOUPORN_MOVPIC_W    140
#define YOUPORN_MOVPIC_H    170

#define YOUPORN_MOVNAME_X (YOUPORN_MOVPIC_X)
#define YOUPORN_MOVNAME_Y (YOUPORN_MOVPIC_Y + YOUPORN_MOVPIC_H + 5)
#define YOUPORN_MOVNAME_W (YOUPORN_MOVPIC_W)
#define YOUPORN_MOVNAME_H (35)

#define NUM_MOVIE_PER_PAGE  10

//bottom help container
#define YOUPORN_BOTTOM_HELP_X  300
#define YOUPORN_BOTTOM_HELP_Y  620
#define YOUPORN_BOTTOM_HELP_W  860
#define YOUPORN_BOTTOM_HELP_H  50

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
    3,3,
    {IDS_SELECT, IDS_MOVE, IDS_MENU},
    {IM_HELP_OK, IM_HELP_ARROW, IM_HELP_MENU}
};


/*!
 * Video view control id
 */
typedef enum
{
    IDC_YOUPORN_LIST_CONT = 1,
    IDC_YOUPORN_LEFT_BMP,  
    IDC_YOUPORN_LIST,
    IDC_YOUPORN_PAGE_CONT,
    IDC_YOUPORN_PAGE_NUM,

    IDC_YOUPORN_MOVLIST_CONT,
    IDC_YOUPORN_MOVITEM_START,
    IDC_YOUPORN_MOVITEM_END = IDC_YOUPORN_MOVITEM_START + NUM_MOVIE_PER_PAGE - 1,
    IDC_YOUPORN_MOVITEM_PIC,
    IDC_YOUPORN_MOVITEM_NAME,

    IDC_YOUPORN_BTM_HELP,
}ui_yp_ctrl_id_t;

/*!
 Macro
 */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))



static youporn_website_t* p_curWebsite = NULL;
static s16 g_curCatelistIndex = -1;
static s16 g_curMovlistIndex = 0;
static youx_item* sg_p_yp_video  = NULL;
static int sg_video_cnt = 0;
static u8 curWebsitesIndex = 0;
static s16 sg_cur_page_index = 0;
static u16 sg_uni_str_tab[10][64] = {{0}}; //preserve movie names of current page
static u8 sg_draw_end_flag[NUM_MOVIE_PER_PAGE] = {0};
static s16 sg_cur_draw_index = -1;
static BOOL sg_search_flag = FALSE;

extern iconv_t g_cd_utf8_to_utf16le;
/*!
 * Function define
 */
u16 yp_catelist_keymap(u16 key);
RET_CODE yp_catelist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 yp_movlist_keymap(u16 key);
RET_CODE yp_movlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 yp_cont_keymap(u16 key);
RET_CODE yp_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 yp_pwd_keymap(u16 key);
RET_CODE yp_pwd_proc(control_t *p_list, u16 msg,  u32 para1, u32 para2);

extern netplay_server_info_t* ui_get_server_info(void);

/*================================================================================================
                           video play view internel function
 ================================================================================================*/
static void yp_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void yp_pic_deinit(void)
{
  pic_stop();
  ui_pic_release();
}

static void yp_update_page_num(control_t* p_ctrl, u16 page_idx)
{
  char num[20] = {0};
  u16 page = 0;
  u16 cur = 0;

  if( NULL == p_curWebsite )
  {
    cur = 0;
    page = 0;
  }
  else
  {
    cur = page_idx + 1;
    page = (p_curWebsite->p_cate[g_curCatelistIndex].video_cnt\
               + NUM_MOVIE_PER_PAGE - 1) / NUM_MOVIE_PER_PAGE;
  }
  OS_PRINTF("page count = %d\n", page);
  

  sprintf(num, "%d/%d", cur, page);
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

static RET_CODE yp_update_catelist(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
	u16 uni_str[128];
    cnt = list_get_count(p_list);
    OS_PRINTF("%s\n", __FUNCTION__);
    for (i = start; i < start + size && i < cnt; i++)
    {
        char* inbuf = "Video";
        char* outbuf = (char*)uni_str;
        int src_len = strlen(inbuf) + 1;
        int dest_len = sizeof(uni_str);

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

    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUPORN_MOVLIST_CONT,
                                  YOUPORN_MOVLIST_CONTX, YOUPORN_MOVLIST_CONTY,
                                  YOUPORN_MOVLIST_CONTW, YOUPORN_MOVLIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    x = YOUPORN_MOVLIST_ITEM_X;
    y = YOUPORN_MOVLIST_ITEM_Y;
    for (i = 0; i < NUM_MOVIE_PER_PAGE; i++)
    {
      p_item = ctrl_create_ctrl(CTRL_CONT, IDC_YOUPORN_MOVITEM_START + i,
                              x, y,
                              YOUPORN_MOVLIST_ITEM_W, YOUPORN_MOVLIST_ITEM_H,
                              p_list_cont, 0);
      ctrl_set_rstyle(p_item, RSI_MV_POSTER_FRM_SH, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_SH);

      p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_YOUPORN_MOVITEM_PIC,
                              YOUPORN_MOVPIC_X, YOUPORN_MOVPIC_Y,
                              YOUPORN_MOVPIC_W, YOUPORN_MOVPIC_H,
                              p_item, 0);
      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

      p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_YOUPORN_MOVITEM_NAME,
                              YOUPORN_MOVNAME_X, YOUPORN_MOVNAME_Y,
                              YOUPORN_MOVNAME_W, YOUPORN_MOVNAME_H,
                              p_item, 0);
//      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
      ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

      text_set_font_style(p_ctrl, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
      text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
      x += YOUPORN_MOVLIST_ITEM_W + YOUPORN_MOVLIST_ITEM_HGAP;
      if (x + YOUPORN_MOVPIC_W > YOUPORN_MOVLIST_CONTW)
      {
        x = YOUPORN_MOVLIST_ITEM_X;
        y += YOUPORN_MOVLIST_ITEM_H + YOUPORN_MOVLIST_ITEM_VGAP;
      }
    }

    return p_list_cont;
}

static RET_CODE yp_update_movlist(control_t* p_list_cont, u16 start, u16 size, u32 context)
{
    control_t *p_item = NULL;
    control_t *p_pic, *p_text;
    u16 i;//, cnt;
    youx_item* p_movie;
    //u16 uni_str[128] = {0};
    p_movie =  sg_p_yp_video;

OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
    memset(sg_uni_str_tab, 0, sizeof(sg_uni_str_tab));
    for (i = 0; i < size; i++)
    {
      p_item = ctrl_get_child_by_id(p_list_cont, IDC_YOUPORN_MOVITEM_START + i);
      p_pic = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_PIC);
      p_text = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_NAME);
//OS_PRINTF("%s,%d, i = %d\n", __FUNCTION__, __LINE__, i);
      if (start + i < sg_video_cnt)
      {
        char* inbuf = p_movie[start + i].title;
        char* outbuf = (char*)(sg_uni_str_tab + i);
        int src_len = strlen(inbuf) + 1;
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
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);


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
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
    return SUCCESS;
}

static RET_CODE yp_clear_movlist(control_t* p_list_cont,  u16 size)
{
    control_t *p_item = NULL;
    control_t *p_pic, *p_text;
    u16 i;

    for (i = 0; i < size; i++)
    {
      p_item = ctrl_get_child_by_id(p_list_cont, IDC_YOUPORN_MOVITEM_START + i);
      p_pic = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_PIC);
      p_text = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_NAME);

      bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
      text_set_content_by_extstr(p_text, NULL);
    }
    
    return SUCCESS;
}


static void yp_set_focus(control_t *p_cont, s16 last_focus, s16 focus)
{
  control_t* p_item = NULL;
  control_t* p_text = NULL;

  if( last_focus < 0 )
    ;
  else
  {
    p_item = ctrl_get_child_by_id(p_cont, IDC_YOUPORN_MOVITEM_START + last_focus);
    p_text = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_NAME);
    text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
    ctrl_process_msg(p_item, MSG_LOSTFOCUS, 0, 0);
    
  }

  p_item = ctrl_get_child_by_id(p_cont, IDC_YOUPORN_MOVITEM_START + focus);
  p_text = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_NAME);
  text_set_font_style(p_text, FSI_BLACK_16, FSI_BLACK_16, FSI_BLACK_16);
  
  ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);
}

#if 1
static BOOL yp_movlist_set_focus_pos(control_t *p_list_cont, u16 focus)
{
  control_t *p_active = NULL, *p_next_ctrl = NULL;
  u16 index;

  p_active = p_list_cont->p_active_child;
  if(p_active != NULL)
  {
    ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
  }

  index = focus % NUM_MOVIE_PER_PAGE;
  p_next_ctrl = ctrl_get_child_by_id(p_list_cont, IDC_YOUPORN_MOVITEM_START + index);
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
static RET_CODE yp_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;

  p_root = fw_find_root_by_id(ROOT_ID_YOUPORN);

  if (p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  
  ui_video_c_destroy();

  fw_tmr_destroy(ROOT_ID_YOUPORN, MSG_YP_TIMEOUT);
  
  return ERR_NOFEATURE;
}

//static RET_CODE yp_on_pic_play_start(control_t *p_cont, u16 msg, u32 para1, u32 para2)
static RET_CODE draw_video_icon(control_t *p_cont, u16 movie_index)
{
  control_t *p_item = NULL;
  control_t *p_pic = NULL;
  rect_t rect;
  youx_item* p_movie = sg_p_yp_video;
OS_PRINTF("eeeeeeeeeeeeeeenter%s\n", __FUNCTION__);
OS_PRINTF("movie_index = %d\n", movie_index);
  p_item = ctrl_get_child_by_id(p_cont, IDC_YOUPORN_MOVITEM_START + movie_index);
  p_pic = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_PIC);

  ctrl_get_frame(p_pic, &rect);
  ctrl_client2screen(p_pic, &rect);

  if( p_movie[movie_index].image )
  {
    u32 identity = 0;
    sg_cur_draw_index = movie_index;
    identity = (curWebsitesIndex<<24) 
  	           | ((g_curCatelistIndex&0XFF)<<16)
  	           | sg_cur_draw_index;
	
    ui_pic_play_by_url(p_movie[movie_index].image, &rect, identity);
    //bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    //ctrl_paint_ctrl(p_pic, TRUE);
  }
OS_PRINTF("eeeeeeeeeeeeeeeexit%s\n", __FUNCTION__);

  return SUCCESS;
}

static RET_CODE yp_on_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i = 0;
  control_t* p_menu =  ui_comm_root_get_root(ROOT_ID_YOUPORN);
  control_t* p_mlist = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_MOVLIST_CONT);
  control_t* p_item = NULL;
  control_t* p_pic = NULL;
  u32 identity = 0;
  static u8 err_cnt;

  if( ROOT_ID_YOUPORN != fw_get_focus_id() )
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
  p_item = ctrl_get_child_by_id(p_mlist, IDC_YOUPORN_MOVITEM_START + sg_cur_draw_index);
  p_pic = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_PIC);


  bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
  ctrl_paint_ctrl(p_pic, TRUE);
    }
    err_cnt = 0;
  sg_draw_end_flag[sg_cur_draw_index] = 1;
  pic_stop();
  
  for( i = 0; i < sg_video_cnt; i++ )
    if(0 == sg_draw_end_flag[i])
      break;
  
  if( i < sg_video_cnt )
  {
    sg_cur_draw_index = i;
    draw_video_icon(p_mlist, i);
  }
  }
  else if( MSG_PIC_EVT_DATA_ERROR == msg )
  {
    err_cnt++;
    pic_stop();
    draw_video_icon(p_mlist, sg_cur_draw_index);
  }
  return SUCCESS;
}


static RET_CODE yp_on_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i;
  control_t* p_mlist = ctrl_get_child_by_id(p_cont, IDC_YOUPORN_MOVLIST_CONT);
  control_t* p_item = NULL;
  control_t* p_pic = NULL;

  for( i = 0; i < NUM_MOVIE_PER_PAGE; i++ )
  {
    p_item = ctrl_get_child_by_id(p_mlist, IDC_YOUPORN_MOVITEM_START + i);
    p_pic = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_PIC);
    bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
  }
  ctrl_paint_ctrl(p_mlist, TRUE);

  yp_pic_init();

  OS_PRINTF("*******ENTER %s\n", __FUNCTION__);
  draw_video_icon(p_mlist, 0);
  OS_PRINTF("*******EXIT %s\n", __FUNCTION__);
  return SUCCESS;
}

/*
RET_CODE ui_open_yp_video_check_pass(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_YOUPORN,
    ((SCREEN_WIDTH-PWDLG_W)/2),
    ((SCREEN_HEIGHT-PWDLG_H)/2),
    IDS_MSG_INPUT_PASSWORD,
    yp_pwd_keymap,
    yp_pwd_proc,
    PWDLG_T_COMMON
  };

	

  ui_comm_pwdlg_open(&pwdlg_data);

}
*/
/*!
 * Video view entry
 */
RET_CODE ui_open_yp_video(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_bmp = NULL;
    control_t *p_list_cont = NULL, *p_list = NULL;
    control_t *p_page_cont = NULL;
    control_t *p_icon = NULL, *p_ctrl = NULL;
    control_t *p_movlist_cont = NULL;
    control_t *p_bottom_help = NULL;
    u8 i = 0;

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
  
#ifndef WIN32
  ui_youporn_init();
#endif

  // create timer for 20s timeout
  fw_tmr_create(ROOT_ID_YOUPORN, MSG_YP_TIMEOUT, 20*1000, FALSE);

    yp_pic_init();

    g_curCatelistIndex = 0;
    g_curMovlistIndex = 0;
    sg_cur_page_index = 0;
    curWebsitesIndex = 0;  //only youporn here
  
    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_YOUPORN, 0,
                                    IM_INDEX_YOUPORN_BANNER, IDS_YOUPORN);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, yp_cont_keymap);
    ctrl_set_proc(p_menu, yp_cont_proc);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_YOUPORN_BTM_HELP,
                                      YOUPORN_BOTTOM_HELP_X, YOUPORN_BOTTOM_HELP_Y,
                                      YOUPORN_BOTTOM_HELP_W, YOUPORN_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(120, 0, YOUPORN_BOTTOM_HELP_W-120, YOUPORN_BOTTOM_HELP_H,  &movie_help_data,  p_bottom_help);

    /*!
     * Create list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUPORN_LIST_CONT,
                                  YOUPORN_LIST_CONTX, YOUPORN_LIST_CONTY,
                                  YOUPORN_LIST_CONTW, YOUPORN_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_YOUPORN_LEFT_BMP, YOUPORN_BMP_X, YOUPORN_BMP_Y, 
      YOUPORN_BMP_W, YOUPORN_BMP_H, p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, IM_ICON_NETWORK_MOVIE);
    
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_YOUPORN_LIST,
                          YOUPORN_LIST_X, YOUPORN_LIST_Y,
                          YOUPORN_LISTW, YOUPORN_LISTH,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, yp_catelist_keymap);
    ctrl_set_proc(p_list, yp_catelist_proc);

    ctrl_set_mrect(p_list, YOUPORN_LIST_MIDL, YOUPORN_LIST_MIDT,
                  YOUPORN_LIST_MIDW + YOUPORN_LIST_MIDL, YOUPORN_LIST_MIDH + YOUPORN_LIST_MIDT);

    list_set_item_interval(p_list, YOUPORN_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &cate_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, 1, YOUPORN_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(cate_list_attr), YOUPORN_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, yp_update_catelist, 0);
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

OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);
    yp_update_catelist(p_list, list_get_valid_pos(p_list), YOUPORN_LIST_ITEM_NUM_ONE_PAGE, 0);
OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);
    //Page
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUPORN_PAGE_CONT,
                                    YOUPORN_PAGE_CONTX, YOUPORN_PAGE_CONTY,
                                    YOUPORN_PAGE_CONTW, YOUPORN_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               YOUPORN_PAGE_ARROWL_X, YOUPORN_PAGE_ARROWL_Y,
                               YOUPORN_PAGE_ARROWL_W, YOUPORN_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               YOUPORN_PAGE_ARROWR_X, YOUPORN_PAGE_ARROWR_Y,
                               YOUPORN_PAGE_ARROWR_W, YOUPORN_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 YOUPORN_PAGE_NAME_X, YOUPORN_PAGE_NAME_Y,
                                 YOUPORN_PAGE_NAME_W, YOUPORN_PAGE_NAME_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_ctrl, IDS_PAGE1);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_YOUPORN_PAGE_NUM,
                                 YOUPORN_PAGE_VALUE_X, YOUPORN_PAGE_VALUE_Y,
                                 YOUPORN_PAGE_VALUE_W, YOUPORN_PAGE_VALUE_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);

    yp_update_page_num(p_ctrl, 0);
OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);

    //Movie list
    p_movlist_cont = ui_creat_movie_list(p_menu);
    ctrl_set_keymap(p_movlist_cont, yp_movlist_keymap);
    ctrl_set_proc(p_movlist_cont, yp_movlist_proc);

    ctrl_process_msg(p_movlist_cont, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
OS_PRINTF("%s, %d\n", __FUNCTION__, __LINE__);
/*
  do
  { //req media info
    int identify = 0;
    netplay_website_info_t* p_web = (netplay_website_info_t*)para1;
    int web_idx = para2;
    identify = web_idx<<24 | 0;
    OS_PRINTF("xuhd, request(%d,%d,%d)\n", web_idx, 0, 0);

    sg_search_flag = FALSE;
    ui_youporn_video_request(p_web->name, p_web->p_cate[0].name,0, identify);
  }while(0);
*/
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

static RET_CODE yp_on_catelist_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  //control_t* p_root = fw_find_root_by_id(ROOT_ID_YOUPORN_WEBSITES);

  sg_video_cnt = 0;

  clear_draw_end_flag();
  #ifndef WIN32
  ui_youporn_deinit();
  #endif
  p_curWebsite = NULL;
  yp_pic_deinit();
/*
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
*/  
  return ERR_NOFEATURE;
}

static RET_CODE yp_on_catelist_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  sg_video_cnt = 0;

  clear_draw_end_flag();
  #ifndef WIN32
  ui_youporn_deinit();
  #endif
  p_curWebsite = NULL;
  yp_pic_deinit();

  return ERR_NOFEATURE;
}

static RET_CODE yp_on_catelist_updown(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 focus = 0;
  int identify = 0;
  control_t *p_menu = NULL;
  control_t *p_movlist_cont = NULL;
  control_t *p_page_cont, *p_ctrl;

  if( NULL == p_curWebsite )
    return SUCCESS;

  if( 1 == p_curWebsite->cate_count )
    return SUCCESS;

  list_class_proc(p_list, msg, para1, para2);
  focus = list_get_focus_pos(p_list);
  g_curCatelistIndex = focus;
  g_curMovlistIndex = 0;
  clear_draw_end_flag();
  
   p_menu =  ui_comm_root_get_root(ROOT_ID_YOUPORN);
   p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_PAGE_CONT);
   p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUPORN_PAGE_NUM);
   p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_MOVLIST_CONT);
   yp_update_page_num(p_ctrl, 0);
   yp_clear_movlist(p_movlist_cont,NUM_MOVIE_PER_PAGE);
   ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  identify = ((curWebsitesIndex&0xFF)<<24) 
  	         | ((g_curCatelistIndex&0XFF)<<16)
  	         | 0;
  OS_PRINTF("xuhd, request(%d,%d,%d)\n", curWebsitesIndex, g_curCatelistIndex, 0);

  sg_search_flag = FALSE;
  #ifndef WIN32
  ui_youporn_video_request(p_curWebsite->name,
      p_curWebsite->p_cate[g_curCatelistIndex].name,
      0, identify);
  #endif
  return SUCCESS;
}

static RET_CODE yp_on_catelist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu, *p_movlist_cont;

  if( 0 == sg_video_cnt )
    return SUCCESS;

  p_menu = ui_comm_root_get_root(ROOT_ID_YOUPORN);
  p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_MOVLIST_CONT);
  MT_ASSERT(p_movlist_cont != NULL);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);

#if 0
  ctrl_set_attr(p_movlist_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_movlist_cont, OBJ_STS_SHOW);
  ctrl_process_msg(p_movlist_cont, MSG_GETFOCUS, 0, 0);
#endif
  yp_movlist_set_focus_pos(p_movlist_cont, g_curMovlistIndex);

  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  return SUCCESS;
}

static RET_CODE yp_on_movlist_exit(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu, *p_catelist_cont, *p_catelist, *p_item, *p_text;

  p_menu = ui_comm_root_get_root(ROOT_ID_YOUPORN);
  p_catelist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_LIST_CONT);
  p_catelist = ctrl_get_child_by_id(p_catelist_cont, IDC_YOUPORN_LIST);
  p_item = ctrl_get_child_by_id(p_movlist_cont, IDC_YOUPORN_MOVITEM_START+g_curMovlistIndex);
  p_text = ctrl_get_child_by_id(p_item, IDC_YOUPORN_MOVITEM_NAME);
  text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
  
  ctrl_process_msg(p_movlist_cont, MSG_LOSTFOCUS, 0, 0);

  ctrl_set_attr(p_catelist, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_catelist, OBJ_STS_SHOW);
  ctrl_process_msg(p_catelist, MSG_GETFOCUS, 0, 0);

  clear_draw_end_flag();

  ctrl_paint_ctrl(p_menu, TRUE);

  return SUCCESS;
}


static RET_CODE yp_change_page(control_t* p_cont, u16 msg, u32 para1, u32 para2)
{
  int identify = 0;
  int page_cnt = 0;

  if( NULL == p_curWebsite )
    return SUCCESS;
  
  page_cnt = (p_curWebsite->p_cate[g_curCatelistIndex].video_cnt 
  	+ NUM_MOVIE_PER_PAGE - 1) / NUM_MOVIE_PER_PAGE;

  yp_pic_deinit();
  yp_pic_init();
  
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
  #ifndef WIN32
  ui_youporn_video_request(p_curWebsite->name,
  	p_curWebsite->p_cate[g_curCatelistIndex].name,
  	sg_cur_page_index, identify);
  #endif
  sg_cur_draw_index = 0;

  return SUCCESS;
}

static RET_CODE yp_on_focus_move(control_t* p_cont, u16 msg, u32 para1, u32 para2)
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
  yp_set_focus(p_cont, old_focus, g_curMovlistIndex);
  
  return SUCCESS;
}


//enter movlist details
static RET_CODE yp_on_movlist_select(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  u16 video_idx = 0;
  if( NULL == p_curWebsite )
    return SUCCESS;

  if( g_curMovlistIndex >= sg_video_cnt )
    return SUCCESS; //focus on a cell with no movie
  
  clear_draw_end_flag();
  yp_pic_deinit();

  video_idx = sg_cur_page_index*NUM_MOVIE_PER_PAGE + g_curMovlistIndex;
  #ifndef WIN32
  request_play_video_url(video_idx);
  #endif
  manage_open_menu(ROOT_ID_ONMOV_DESCRIPTION, (u32)&(sg_p_yp_video[g_curMovlistIndex]), 1);

  return SUCCESS;
}


static RET_CODE yp_get_webinfo(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  control_t *p_page_cont, *p_ctrl;

  fw_tmr_destroy(ROOT_ID_YOUPORN, MSG_YP_TIMEOUT);

  p_curWebsite = (youporn_website_t*)para1;
  
  p_menu =  ui_comm_root_get_root(ROOT_ID_YOUPORN);
  p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_PAGE_CONT);
  p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUPORN_PAGE_NUM);

  yp_update_page_num(p_ctrl, 0);
  
  return SUCCESS;
}

static RET_CODE yp_get_videos(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  control_t *p_movlist_cont = NULL;
  control_t *p_page_cont = NULL;
  control_t *p_ctrl = NULL;
  u16 page_idx = 0;
  sg_p_yp_video = (youx_item*) para1;
  sg_video_cnt = para2&0xffff;
  page_idx = para2>>16;

OS_PRINTF(">>>>>>>>>>>>>>>>>  %s()\n", __FUNCTION__);
OS_PRINTF("%s, sg_video_cnt = %d, page_idx =%d\n",
	__FUNCTION__, sg_video_cnt, page_idx);
  p_menu =  ui_comm_root_get_root(ROOT_ID_YOUPORN);
  p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_PAGE_CONT);
  p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUPORN_PAGE_NUM);
  p_movlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUPORN_MOVLIST_CONT);
  yp_update_page_num(p_ctrl, page_idx);
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  yp_update_movlist(p_movlist_cont,
                                       0,
                                       NUM_MOVIE_PER_PAGE,
                                       0);
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);
  draw_video_icon(p_movlist_cont, 0);
OS_PRINTF("%s,%d\n", __FUNCTION__, __LINE__);

  return SUCCESS;
}


 
static void exit_youporn(void)
{
  control_t *p_menu = NULL;
  p_menu =  ui_comm_root_get_root(ROOT_ID_YOUPORN);

  ctrl_process_msg(p_menu, MSG_EXIT_ALL, 0, 0);
}

static RET_CODE on_yp_timeout(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_TIMEOUT, exit_youporn, 0);

  return SUCCESS;
}



BEGIN_KEYMAP(yp_catelist_keymap, NULL)
   // ON_EVENT(V_KEY_MENU, MSG_EXIT)
 //   ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_FOCUS_RIGHT)
END_KEYMAP(yp_catelist_keymap, NULL)

BEGIN_MSGPROC(yp_catelist_proc, list_class_proc)
//    ON_COMMAND(MSG_EXIT, yp_on_catelist_exit)
//    ON_COMMAND(MSG_EXIT_ALL, yp_on_catelist_exit_all)
    ON_COMMAND(MSG_FOCUS_UP, yp_on_catelist_updown)
    ON_COMMAND(MSG_FOCUS_DOWN, yp_on_catelist_updown)
    ON_COMMAND(MSG_FOCUS_RIGHT, yp_on_catelist_change_focus)
    ON_COMMAND(MSG_YP_VIDEO_INFO, yp_get_videos)  
    ON_COMMAND(MSG_YP_WEB_INFO, yp_get_webinfo)  
END_MSGPROC(yp_catelist_proc, list_class_proc)

BEGIN_KEYMAP(yp_movlist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP,MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN,MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(yp_movlist_keymap, NULL)

BEGIN_MSGPROC(yp_movlist_proc, cont_class_proc)
    ON_COMMAND(MSG_EXIT, yp_on_movlist_exit)
    ON_COMMAND(MSG_EXIT_ALL, yp_on_movlist_exit)
    ON_COMMAND(MSG_FOCUS_LEFT, yp_on_focus_move)
    ON_COMMAND(MSG_FOCUS_RIGHT, yp_on_focus_move)
    ON_COMMAND(MSG_FOCUS_UP, yp_on_focus_move)
    ON_COMMAND(MSG_FOCUS_DOWN, yp_on_focus_move)
    ON_COMMAND(MSG_SELECT, yp_on_movlist_select)
    ON_COMMAND(MSG_PAGE_UP, yp_change_page)
    ON_COMMAND(MSG_PAGE_DOWN, yp_change_page)
    ON_COMMAND(MSG_YP_VIDEO_INFO, yp_get_videos)  
    ON_COMMAND(MSG_YP_WEB_INFO, yp_get_webinfo)  
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, yp_on_pic_draw_end)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, yp_on_pic_draw_end)
END_MSGPROC(yp_movlist_proc, cont_class_proc)

BEGIN_KEYMAP(yp_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(yp_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(yp_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, yp_on_destory)
  //ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, yp_on_pic_play_start)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, yp_on_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, yp_on_pic_draw_end)
  //ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_picture_too_large)
  //ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_picture_unsupport)  
  ON_COMMAND(MSG_UPDATE, yp_on_update)  
  ON_COMMAND(MSG_YP_VIDEO_INFO, yp_get_videos)
    ON_COMMAND(MSG_YP_WEB_INFO, yp_get_webinfo)  
    ON_COMMAND(MSG_EXIT, yp_on_catelist_exit)
    ON_COMMAND(MSG_EXIT_ALL, yp_on_catelist_exit_all)
    ON_COMMAND(MSG_YP_TIMEOUT, on_yp_timeout)
END_MSGPROC(yp_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(yp_pwd_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)   
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)   
  ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)   
  ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)   
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(yp_pwd_keymap, NULL)

BEGIN_MSGPROC(yp_pwd_proc, cont_class_proc)
END_MSGPROC(yp_pwd_proc, cont_class_proc)

#endif

