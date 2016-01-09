/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
//#include "ui_rename.h"
//#include "lib_char.h"
#if ENABLE_NETWORK
#include "commonData.h"
#include "redxDataProvider.h"
#include "ui_redtube_api.h"

#include "ui_redtube_gprot.h"
#include "ui_redtube_prot.h"

#include "SM_StateMachine.h"
#include "States_Redtube.h"
#include "ui_video.h"
#include "ui_picture_api.h"
#include "ui_video_player_gprot.h"
#include "ui_keyboard_v2.h"

/*!
 Const value
 */
//plist list container
#define REDTUBE_LIST_CONTX   120
#define REDTUBE_LIST_CONTY   100
#define REDTUBE_LIST_CONTW  200
#define REDTUBE_LIST_CONTH   570

//bmp
#define REDTUBE_BMP_X   0
#define REDTUBE_BMP_Y   0
#define REDTUBE_BMP_W  REDTUBE_LIST_CONTW
#define REDTUBE_BMP_H   90


//List
#define REDTUBE_LIST_X  20
#define REDTUBE_LIST_Y  90
#define REDTUBE_LISTW  160
#define REDTUBE_LISTH  400

#define REDTUBE_LIST_MIDL  0//5
#define REDTUBE_LIST_MIDT  0//5
#define REDTUBE_LIST_MIDW  (REDTUBE_LISTW - 2 * REDTUBE_LIST_MIDL)
#define REDTUBE_LIST_MIDH  (REDTUBE_LISTH - 2 * REDTUBE_LIST_MIDT)

#define REDTUBE_LIST_ICON_VGAP 4
#define REDTUBE_LIST_ITEM_NUM_ONE_PAGE  9//12
#define REDTUBE_LIST_ITEM_COUNT     8

//page container
#define REDTUBE_PAGE_CONTX 330
#define REDTUBE_PAGE_CONTY 100
#define REDTUBE_PAGE_CONTW 830
#define REDTUBE_PAGE_CONTH 50

//page arrow left
#define REDTUBE_PAGE_ARROWL_X       150
#define REDTUBE_PAGE_ARROWL_Y       7
#define REDTUBE_PAGE_ARROWL_W      36
#define REDTUBE_PAGE_ARROWL_H       36
//page arrow right
#define REDTUBE_PAGE_ARROWR_X       (REDTUBE_PAGE_CONTW - REDTUBE_PAGE_ARROWL_X - REDTUBE_PAGE_ARROWR_W)
#define REDTUBE_PAGE_ARROWR_Y       7
#define REDTUBE_PAGE_ARROWR_W      36
#define REDTUBE_PAGE_ARROWR_H       36
//page name
#define REDTUBE_PAGE_NAME_X  250
#define REDTUBE_PAGE_NAME_Y  7
#define REDTUBE_PAGE_NAME_W 100
#define REDTUBE_PAGE_NAME_H  36
//page value
#define REDTUBE_PAGE_VALUE_X  450
#define REDTUBE_PAGE_VALUE_Y  7
#define REDTUBE_PAGE_VALUE_W 100
#define REDTUBE_PAGE_VALUE_H  36

//client area
#define REDTUBE_CLIENT_CONTX 330//(REDTUBE_LIST_CONTX + REDTUBE_LIST_CONTW)
#define REDTUBE_CLIENT_CONTY 160//(REDTUBE_PAGE_CONTY + REDTUBE_PAGE_CONTH)
#define REDTUBE_CLIENT_CONTW 830
#define REDTUBE_CLIENT_CONTH 450//(500)

//movie list
#define REDTUBE_MOVLIST_X 0
#define REDTUBE_MOVLIST_Y 0
#define REDTUBE_MOVLIST_W (REDTUBE_CLIENT_CONTW  /*-REDTUBE_MOVLIST_SBAR_W - 6*/)
#define REDTUBE_MOVLIST_H REDTUBE_CLIENT_CONTH
#define REDTUBE_MOVLIST_ITEM_V_GAP      10//(2)

#define REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE  4

#if 0
#define REDTUBE_MOVLIST_SBAR_X        (REDTUBE_CLIENT_CONTW - REDTUBE_MOVLIST_SBAR_W)
#define REDTUBE_MOVLIST_SBAR_Y        (REDTUBE_MOVLIST_Y)
#define REDTUBE_MOVLIST_SBAR_W        (6)
#define REDTUBE_MOVLIST_SBAR_H        (REDTUBE_MOVLIST_H)
#endif

#define REDTUBE_RATED_ICON_Y            (45)
#define REDTUBE_RATED_ICON_W            (20)
#define REDTUBE_RATED_ICON_H            (20)


//category list
#define REDTUBE_CATELIST_ITEM_NUM_ONE_PAGE  6

//info
#define REDTUBE_INFO_PIC_X  (215)//(220)//(230)
#define REDTUBE_INFO_PIC_Y  (0)
#define REDTUBE_INFO_PIC_W  (400)//(390)//(370)
#define REDTUBE_INFO_PIC_H  (280)//(320)

#define REDTUBE_INFO_DETAIL_CONTX       (0)
#define REDTUBE_INFO_DETAIL_CONTY       (REDTUBE_INFO_PIC_Y + REDTUBE_INFO_PIC_H + 10)
#define REDTUBE_INFO_DETAIL_CONTW       (REDTUBE_CLIENT_CONTW)
#define REDTUBE_INFO_DETAIL_CONTH       (REDTUBE_CLIENT_CONTH - REDTUBE_INFO_DETAIL_CONTY)

#define REDTUBE_INFO_DETAIL_X       COMM_BOX1_BORDER
#define REDTUBE_INFO_DETAIL_Y       COMM_BOX1_BORDER
#define REDTUBE_INFO_DETAIL_W       (REDTUBE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - COMM_BOX1_BORDER - REDTUBE_INFO_DETAIL_SBAR_W - 10)
#define REDTUBE_INFO_DETAIL_H       (REDTUBE_INFO_DETAIL_CONTH - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

#define REDTUBE_INFO_DETAIL_MIDX    (0)
#define REDTUBE_INFO_DETAIL_MIDY    (0)
#define REDTUBE_INFO_DETAIL_MIDW    (REDTUBE_INFO_DETAIL_W)
#define REDTUBE_INFO_DETAIL_MIDH    (REDTUBE_INFO_DETAIL_H)

#define REDTUBE_INFO_DETAIL_SBAR_X        (REDTUBE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - REDTUBE_INFO_DETAIL_SBAR_W)
#define REDTUBE_INFO_DETAIL_SBAR_Y        (REDTUBE_INFO_DETAIL_Y)
#define REDTUBE_INFO_DETAIL_SBAR_W        (6)
#define REDTUBE_INFO_DETAIL_SBAR_H        (REDTUBE_INFO_DETAIL_H)

//bottom help container
#define REDTUBE_BOTTOM_HELP_X  330//(REDTUBE_LIST_CONTX + REDTUBE_LIST_CONTW + 20)
#define REDTUBE_BOTTOM_HELP_Y  620// (REDTUBE_CLIENT_CONTY + REDTUBE_CLIENT_CONTH + 10)
#define REDTUBE_BOTTOM_HELP_W  830//(COMM_BG_W - REDTUBE_BOTTOM_HELP_X - 20 - REDTUBE_LIST_CONTX)
#define REDTUBE_BOTTOM_HELP_H  50


/*!
 Macro
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*!
 * Type define
 */

/*!
 * Video view control id
 */
typedef enum
{
    IDC_REDTUBE_LIST_CONT = 1,
    IDC_REDTUBE_LEFT_BMP,
    IDC_REDTUBE_LIST,
    IDC_REDTUBE_PAGE_CONT,
    IDC_REDTUBE_PAGE_NUM,

    IDC_REDTUBE_CLIENT_CONT,
    IDC_REDTUBE_MOVLIST,
    //IDC_REDTUBE_MOVLIST_SBAR,
    IDC_REDTUBE_CATELIST,
    //IDC_REDTUBE_CATELIST_SBAR,

    IDC_REDTUBE_BTM_HELP,
}ui_redtube_ctrl_id_t;

typedef enum
{
    VDOLIST_FIELD_ID_LOGO,
    VDOLIST_FIELD_ID_TITLE,
    VDOLIST_FIELD_ID_AUTHOR,
    VDOLIST_FIELD_ID_DURATION,
    VDOLIST_FIELD_ID_RATED_START,
    VDOLIST_FIELD_ID_RATED_END = VDOLIST_FIELD_ID_RATED_START + REDTUBE_RATED_ICON_NUM - 1,
    VDOLIST_FIELD_ID_VIEW_COUNT
}ui_redtube_vdolist_field_id_t;

typedef enum
{
    REDTUBE_RATED_MASK_EMPTY = 0x00,
    REDTUBE_RATED_MASK_HALF = 0x02,
    REDTUBE_RATED_MASK_FULL = 0x03,
}ui_redtube_rated_mask_t;

typedef struct
{ 
    u16 name;
} redtube_mainlist_item;

/*!
 * Function define
 */
u16 redtube_mainlist_keymap(u16 key);
RET_CODE redtube_mainlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 redtube_movlist_keymap(u16 key);
RET_CODE redtube_movlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 redtube_catelist_keymap(u16 key);
RET_CODE redtube_catelist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 redtube_cont_keymap(u16 key);
RET_CODE redtube_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);




/*!
 * Main list style
 */
static list_xstyle_t mainlist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_SH,
  RSI_ITEM_2_HL,
};

static list_xstyle_t mainlist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_xstyle_t mainlist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};
static const list_field_attr_t mainlist_attr[] =
{
    {
        LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
        160,//162,
        0,
        0,
        &mainlist_field_rstyle,
        &mainlist_field_fstyle
    }
};

/*!
 * Movie list style
 */
static list_xstyle_t movlist_item_rstyle =
{
  RSI_PBACK,
  RSI_MAIN_BG,//RSI_ITEM_2_SH,
  RSI_ITEM_13_HL,//RSI_ITEM_2_HL,
  RSI_MAIN_BG,//RSI_ITEM_2_SH,
  RSI_ITEM_13_HL//RSI_ITEM_2_HL,
};

static list_xstyle_t movlist_icon_rstyle =
{
  RSI_PBACK,
  RSI_TRANSPARENT,
  RSI_TRANSPARENT,
  RSI_TRANSPARENT,
  RSI_TRANSPARENT,
};
static list_xstyle_t movlist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_xstyle_t movlist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};
static list_xstyle_t movlist_field_fstyle_small =
{
  FSI_GRAY,
  FSI_WHITE_20,
  FSI_WHITE_20,
  FSI_WHITE_20,
  FSI_WHITE_20,
};

static const list_field_attr_ext_t movlist_attr[] =
{
    //VDOLIST_FIELD_ID_LOGO
    {LISTFIELD_TYPE_ICON,
     168, 105 - 12 - 1,  6, 6, &movlist_icon_rstyle, &movlist_field_fstyle},
    //VDOLIST_FIELD_ID_TITLE
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_TOP,
     530, 58, 174, 12, &movlist_field_rstyle, &movlist_field_fstyle},
    //VDOLIST_FIELD_ID_AUTHOR
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_TOP,
     450, 29, 180, 70, &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_DURATION
    {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
     5*REDTUBE_RATED_ICON_W, 29, 
     REDTUBE_MOVLIST_W - 5*REDTUBE_RATED_ICON_W - 10, 12, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_START
    {LISTFIELD_TYPE_ICON,
     REDTUBE_RATED_ICON_W, REDTUBE_RATED_ICON_H,  
     REDTUBE_MOVLIST_W - 5*REDTUBE_RATED_ICON_W - 10, REDTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     REDTUBE_RATED_ICON_W, REDTUBE_RATED_ICON_H,  
     REDTUBE_MOVLIST_W - 4*REDTUBE_RATED_ICON_W - 10, REDTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     REDTUBE_RATED_ICON_W, REDTUBE_RATED_ICON_H,  
     REDTUBE_MOVLIST_W - 3*REDTUBE_RATED_ICON_W - 10, REDTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     REDTUBE_RATED_ICON_W, REDTUBE_RATED_ICON_H,  
     REDTUBE_MOVLIST_W - 2*REDTUBE_RATED_ICON_W - 10, REDTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_END
    {LISTFIELD_TYPE_ICON,
     REDTUBE_RATED_ICON_W, REDTUBE_RATED_ICON_H,  
     REDTUBE_MOVLIST_W - REDTUBE_RATED_ICON_W - 10, REDTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_VIEW_COUNT
    {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
     190, 29, REDTUBE_MOVLIST_W - 200, 70, &movlist_field_rstyle, &movlist_field_fstyle_small}
};

static const list_field_attr_t catelist_attr[] =
{
    {LISTFIELD_TYPE_UNISTR | STL_LEFT,
     600, 10, 0, &movlist_field_rstyle, &movlist_field_fstyle}
};

static comm_help_data_t emp_help_data =
{
    1, 1,
    {RSC_INVALID_ID},
    {RSC_INVALID_ID}
};

static comm_help_data_t vdo_help_data =
{
    5, 5,
    {IDS_INFO, IDS_MOVE, IDS_PAGE, IDS_PLAY, IDS_RETURN},
    {IM_HELP_INFOR, IM_HELP_MOVE, IM_HELP_PAGE, IM_HELP_OK, IM_HELP_MENU}
};

static comm_help_data_t comm_help_data =
{
    3, 3,
    {IDS_MOVE, IDS_SELECT, IDS_RETURN},
    {IM_HELP_MOVE, IM_HELP_OK, IM_HELP_MENU}
};

//main list priv data
static redtube_mainlist_item mainlist[REDTUBE_LIST_ITEM_COUNT] = 
{
    {
        IDS_MOST_POPULATE
    },
    {
        IDS_CATEGORY
    },
    {
        IDS_SEARCH
    }
};
static s16 g_mainlist_index = -1;

//list content
static ui_redtube_vdo_page_t g_curVdoPage = {0};
static u8  g_vdolist_index = 0;
static u32 g_totalVdoPage = 0;
static u32 g_curVdoPageIdx = 0;
static u32 g_savVdoPageIdx = 0;

static ui_redtube_cate_page_t g_cateList = {0};
static u16 g_catelist_index = 0;
static u8  g_totalCatePage = 0;
static u8  g_curCatePageIdx = 0;

static u16 g_movlist_rend_idx = 0;

static STATEID g_reqUrlFrom = SID_NULL;

static u16 g_exitMsg;


/*================================================================================================
                           video play view internel function
 ================================================================================================*/
static void _list_get_item_size(control_t *p_ctrl, u16 *p_w, u16 *p_h)
{
  u8 lines = 0;

  
  u8 columns = list_get_columns(p_ctrl);
  u16 page = list_get_page(p_ctrl);
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(page >= columns);
  MT_ASSERT(page % columns == 0);

  lines = page / columns;

  *p_w = ((p_ctrl->mrect.right - p_ctrl->mrect.left - REDTUBE_LIST_ICON_VGAP *
     (columns - 1)) / columns);

  *p_h = (p_ctrl->mrect.bottom - p_ctrl->mrect.top - REDTUBE_LIST_ICON_VGAP * (lines - 1)) / lines;
}

static void _list_get_item_rect(control_t *p_ctrl,
                                u16 index,
                                rect_t *p_item_rect)
{
  s16 top = 0, h = 0, left = 0, w = 0;
  u8 lines = 0;
  u16 current_pos = list_get_current_pos(p_ctrl);
  u8 columns = list_get_columns(p_ctrl);
  u16 page = list_get_page(p_ctrl);

  MT_ASSERT(p_ctrl != NULL);

 // p_data = (ctrl_list_data_t *)p_ctrl->priv_data;

/*  if(index < p_data->cur_pos || index > (p_data->cur_pos + p_data->page - 1))
  {
    empty_rect(p_item_rect);
    return;
  }
*/
  MT_ASSERT(page >= columns);
  MT_ASSERT(page % columns == 0);

  lines = page / columns;

  _list_get_item_size(p_ctrl, &w, &h);

/*  if(p_data->is_hori)
  {
    top = (p_ctrl->mrect.top + ((index - p_data->cur_pos) / p_data->columns) *
      (p_data->interval + h));

    left =  w * ((index - p_data->cur_pos) % p_data->columns) + p_ctrl->mrect.left;
  }
  else*/
  {
    top = (p_ctrl->mrect.top + ((index - current_pos) % lines) *
      (REDTUBE_LIST_ICON_VGAP + h));

    left =  w * ((index - current_pos) / lines) + p_ctrl->mrect.left;
  }

  set_rect(p_item_rect, left, top, (left + w), (s16)(top + h));
}

static void redtube_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void redtube_pic_deinit(void)
{
  pic_stop();

  ui_pic_release();
}

static RET_CODE redtube_pic_play_start()
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    rect_t rect;
    rect_t item_rect;

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);

    OS_PRINTF("@@@redtube_pic_play_start\n");
    ctrl_get_frame(p_movlist, &rect);
    //OS_PRINTF("@@@ctrl_get_frame=(%d, %d), (%d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
    ctrl_client2screen(p_movlist, &rect);
    //OS_PRINTF("@@@ctrl_client2screen=(%d, %d), (%d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
    _list_get_item_rect(p_movlist, g_movlist_rend_idx, &item_rect);
    //OS_PRINTF("@@@_list_get_item_rect=(%d, %d), (%d, %d)\n", item_rect.left, item_rect.top, item_rect.right, item_rect.bottom);

    item_rect.left += rect.left;
    item_rect.top += rect.top;
    item_rect.right += rect.left;
    item_rect.bottom += rect.top;
    item_rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
    item_rect.top += movlist_attr[VDOLIST_FIELD_ID_LOGO].top;
    item_rect.right = item_rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
    item_rect.bottom -= movlist_attr[VDOLIST_FIELD_ID_LOGO].top;
    //OS_PRINTF("@@@pic_rect=(%d, %d), (%d, %d)\n", item_rect.left, item_rect.top, item_rect.right, item_rect.bottom);

    while (g_movlist_rend_idx < g_curVdoPage.count)
    {
        if (strlen(g_curVdoPage.vdoList[g_movlist_rend_idx].thumnail_url) > 0)
        {
            ui_pic_play_by_url(g_curVdoPage.vdoList[g_movlist_rend_idx].thumnail_url, &item_rect, 0);
            return SUCCESS;
        }

        g_movlist_rend_idx++;
    }

    return ERR_NOFEATURE;
}

static BOOL ui_release_app_data(void)
{
    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    if (g_cateList.cateList)
    {
        mtos_free(g_cateList.cateList);
        g_cateList.cateList = NULL;
    }
    
    return TRUE;
}

static void redtube_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
  char num[22] = {0};
  sprintf(num, "%ld/%ld", page_num, total_page);

  text_set_content_by_ascstr(p_ctrl, num);
}

/*!
 * list update function
 */
static RET_CODE redtube_update_mainlist(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(p_list, i, 0, mainlist[i].name);
    }

    return SUCCESS;
}

static RET_CODE redtube_update_vdolist(control_t *p_movlist, u16 start, u16 size, u32 context)
{
    redtube_vdo_item_t *p_vdo;
    u16 *p_unistr;
    u32 view_count;
    u16 uni_str[32];
    u16 value_tab[4];
    u16 cnt, rated_mask;
    u16 icon_id = RSC_INVALID_ID;
    u16 i;
    u8  num_str[4];
    u8  asc_str[16];
    u8  j;
    u8 flag;
    BOOL b_first_do;

    OS_PRINTF("@@@redtube_update_vdolist\n");

    cnt = g_curVdoPage.count;
    p_vdo = g_curVdoPage.vdoList;
    p_unistr = (u16 *)gui_get_string_addr(IDS_VIEW);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_icon(p_movlist, i, VDOLIST_FIELD_ID_LOGO, IM_INDEX_NETWORK_YOUTUBE_PIC);
        
        list_set_field_content_by_unistr(p_movlist, i, VDOLIST_FIELD_ID_TITLE, p_vdo[i].title);

        list_set_field_content_by_ascstr(p_movlist, i, VDOLIST_FIELD_ID_AUTHOR, p_vdo[i].upload_time);

        list_set_field_content_by_ascstr(p_movlist, i, VDOLIST_FIELD_ID_DURATION, p_vdo[i].duration);

        rated_mask = p_vdo[i].rated_mask;
        for (j = VDOLIST_FIELD_ID_RATED_END; j >= VDOLIST_FIELD_ID_RATED_START; j--)
        {
            flag = (u8) (rated_mask & 0x0003);
            switch (flag)
            {
                case REDTUBE_RATED_MASK_FULL:
                    icon_id = IM_ICON_YOUTUBE_FLAG1;
                    break;
                case REDTUBE_RATED_MASK_HALF:
                    icon_id = IM_ICON_YOUTUBE_FLAG2;
                    break;
                case REDTUBE_RATED_MASK_EMPTY:
                    icon_id = IM_ICON_YOUTUBE_FLAG3;
                    break;
                default:
                    icon_id = IM_ICON_YOUTUBE_FLAG3;
                    break;
            }

            list_set_field_content_by_icon(p_movlist, i, j, icon_id);

            rated_mask = (rated_mask >> 2);
        }

        memset(value_tab, 0, sizeof(value_tab));
        memset(asc_str, 0, sizeof(asc_str));

        j = 0;
        view_count = p_vdo[i].view_count;
        do
        {
            value_tab[j] = view_count % 1000;
            view_count /= 1000;
            j++;
        } while (view_count > 0 && j < 4);

        b_first_do = TRUE;
        do
        {
            j--;
            memset(num_str, 0, sizeof(num_str));
            if (b_first_do)
            {
                sprintf(num_str, "%d", value_tab[j]);
            }
            else
            {
                sprintf(num_str, "%03d", value_tab[j]);
            }
            strcat(asc_str, num_str);
            if (j <= 0)
            {
                break;
            }
            strcat(asc_str, ".");
            if (b_first_do)
            {
                b_first_do = FALSE;
            }
        } while (TRUE);

        strcat(asc_str, " ");
        str_asc2uni(asc_str, uni_str);
        uni_strcat(uni_str, p_unistr, 32);
        list_set_field_content_by_unistr(p_movlist, i, VDOLIST_FIELD_ID_VIEW_COUNT, uni_str);
    }

    return SUCCESS;
}

static RET_CODE redtube_update_catelist(control_t *p_movlist, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    redtube_cate_item_t *p_cate;

    OS_PRINTF("@@@redtube_update_catelist\n");

    cnt = g_cateList.count;
    p_cate = g_cateList.cateList;

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_unistr(p_movlist, i, 0, p_cate[i].cate_name);
    }

    return SUCCESS;
}

static void redtube_update_curVdoPage(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;
    rect_t rect;

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_REDTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);
    //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST_SBAR);
    //MT_ASSERT(p_movlist_sbar != NULL);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_REDTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);

    list_set_count(p_movlist, g_curVdoPage.count, REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_movlist, g_vdolist_index);

    ctrl_set_attr(p_movlist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_movlist, OBJ_STS_SHOW);
    //ctrl_set_attr(p_movlist_sbar, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_movlist_sbar, OBJ_STS_SHOW);

    if (!ctrl_is_onfocus(p_mainlist))
    {
        ctrl_process_msg(p_movlist, MSG_GETFOCUS, 0, 0);
    }
    redtube_update_page_num(p_ctrl, g_savVdoPageIdx, g_totalVdoPage);
    redtube_update_vdolist(p_movlist, 0, REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE, 0);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);

    //clear pic layer
    pic_stop();

    ctrl_get_frame(p_movlist, &rect);
    ctrl_client2screen(p_movlist, &rect);
    rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
    rect.right = rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
    ui_pic_clear_rect(&rect, 0);
}

static void redtube_update_curCatePage(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_catelist = NULL;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_REDTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_CATELIST);
    MT_ASSERT(p_catelist != NULL);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_REDTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    list_set_count(p_catelist, g_cateList.count, REDTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_catelist, 0);

    ctrl_set_attr(p_catelist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_catelist, OBJ_STS_SHOW);

    if (!ctrl_is_onfocus(p_mainlist))
    {
        ctrl_process_msg(p_catelist, MSG_GETFOCUS, 0, 0);
    }
    redtube_update_page_num(p_ctrl, 1, 1);
    redtube_update_catelist(p_catelist, list_get_valid_pos(p_catelist), REDTUBE_CATELIST_ITEM_NUM_ONE_PAGE, 0);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);
}

static void redtube_open_dlg(u16 str_id)
{
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };
    dlg_data.content = str_id;
    ui_comm_dlg_open(&dlg_data);
}

static void redtube_open_cfm_dlg(u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_REDTUBE);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

static STATEID  redtube_get_cur_entry(void)
{
    STATEID stateID = SID_NULL;

    switch(mainlist[g_mainlist_index].name)
    {
        case IDS_MOST_POPULATE:
            stateID = SID_REDTUBE_DEFAULT;
            break;
        case IDS_CATEGORY:
            stateID = SID_REDTUBE_CATEGORY;
            break;
        case IDS_SEARCH:
            stateID = SID_REDTUBE_SEARCH;
            break;
        default:
            break;
    }

    return stateID;
}

////////////////////////////////////////////////////////////////////////////////////

static void SenRedtube_Inactive(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static STATEID StaRedtube_on_open_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_mainlist_cont, *p_mainlist;

    OS_PRINTF("@@@%s\n", __FUNCTION__);

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_REDTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);

    list_set_focus_pos(p_mainlist, g_mainlist_index);
    list_select_item(p_mainlist, g_mainlist_index);

    ctrl_paint_ctrl(p_mainlist, TRUE);

    return redtube_get_cur_entry();
}

static void SexRedtube_Inactive(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_Active(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_dp_init();
    ui_redtube_set_page_size(REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
}

static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_REDTUBE_MOVLIST) ? TRUE : FALSE;
}

static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_REDTUBE_LIST) ? TRUE : FALSE;
}

static STATEID StaRedtube_on_mainlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    focus = list_get_focus_pos(ctrl);
    g_mainlist_index = focus;

    list_select_item(ctrl, focus);
    ctrl_paint_ctrl(ctrl, TRUE);

    return redtube_get_cur_entry();
}

static STATEID StaRedtube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (msg == MSG_INTERNET_PLUG_OUT)
    {
        g_exitMsg = MSG_EXIT;
    }
    else
    {
        g_exitMsg = msg;
    }

    return SID_NULL;
}

static void SexRedtube_Active(void)
{
    control_t *p_root = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_dp_deinit();

    p_root = fw_find_root_by_id(ROOT_ID_REDTUBE);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, g_exitMsg, 0, 0);
    }
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

/* --------------------- Video list: Default ---------------------*/

static void SenRedtube_Default(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }
    g_curVdoPage.count = 0;
    g_vdolist_index = 0;
    g_totalVdoPage = 1;
    g_savVdoPageIdx = g_curVdoPageIdx = 1;
}

static void SexRedtube_Default(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_DefVdolist(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    redtube_update_curVdoPage();

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_DefVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);

        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        //ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);

        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

static void SenRedtube_DefVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_default_video(g_curVdoPageIdx);
}

static STATEID StaRedtube_default_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {//next req
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;

        g_curVdoPage.count = p_data->count;
        if (p_data->count > 0 && p_data->vdoList != NULL)
        {
            g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
            MT_ASSERT(g_curVdoPage.vdoList != NULL);
            memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
        }
        if (g_curVdoPageIdx > g_savVdoPageIdx)
        {
            g_vdolist_index = 0;
        }
        else
        {
            g_vdolist_index = g_curVdoPage.count - 1;
        }
        g_savVdoPageIdx = g_curVdoPageIdx;
    }
    else
    {// first req
        g_curVdoPage.count = p_data->count;
        if (p_data->count > 0 && p_data->vdoList != NULL)
        {
            g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
            MT_ASSERT(g_curVdoPage.vdoList != NULL);
            memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
        }
        g_vdolist_index = 0;
        g_totalVdoPage = p_data->total_page;
        g_savVdoPageIdx = g_curVdoPageIdx = 1;
    }

    redtube_update_curVdoPage();
    return SID_REDTUBE_DEF_VDOLIST_NORMAL;
}

static STATEID StaRedtube_default_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageIdx = g_savVdoPageIdx;

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_default_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_DEF_VDOLIST_NORMAL;
}

static void SexRedtube_DefVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_DefVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_movlist_rend_idx = 0;
    redtube_pic_play_start();
}

static STATEID StaRedtube_default_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageIdx > 1)
            {
                g_curVdoPageIdx--;
                return SID_REDTUBE_DEF_VDO_REQ;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageIdx < g_totalVdoPage )
            {
                g_curVdoPageIdx++;
                return SID_REDTUBE_DEF_VDO_REQ;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaRedtube_default_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageIdx > 1)
        {
            g_curVdoPageIdx--;
            return SID_REDTUBE_DEF_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageIdx < g_totalVdoPage)
        {
            g_curVdoPageIdx++;
            return SID_REDTUBE_DEF_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaRedtube_default_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);

    list_set_field_content_by_icon(p_movlist, g_movlist_rend_idx, VDOLIST_FIELD_ID_LOGO, RSC_INVALID_ID);
    //ctrl_paint_ctrl(p_movlist, TRUE);
    list_draw_field_ext(p_movlist, g_movlist_rend_idx, VDOLIST_FIELD_ID_LOGO, TRUE);

    pic_stop();
    g_movlist_rend_idx++;
    redtube_pic_play_start();
    return SID_NULL;
}

static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
    g_movlist_rend_idx++;
    redtube_pic_play_start();
    return SID_NULL;
}

static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_REDTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);

    switch (msg)
    {
        case MSG_FOCUS_RIGHT:
            if (g_curVdoPage.count > 0)
            {
                ctrl_process_msg(p_mainlist, MSG_LOSTFOCUS, 0, 0);

                ctrl_set_attr(p_movlist, OBJ_ATTR_ACTIVE);
                ctrl_set_sts(p_movlist, OBJ_STS_SHOW);
                ctrl_process_msg(p_movlist, MSG_GETFOCUS, 0, 0);

                ctrl_paint_ctrl(p_mainlist_cont, TRUE);
                ctrl_paint_ctrl(p_client_cont, TRUE);
            }
            break;
        case MSG_BACK:
        case MSG_MAIN:
            ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);

            ctrl_set_attr(p_mainlist, OBJ_ATTR_ACTIVE);
            ctrl_set_sts(p_mainlist, OBJ_STS_SHOW);
            ctrl_process_msg(p_mainlist, MSG_GETFOCUS, 0, 0);

            ctrl_paint_ctrl(p_mainlist_cont, TRUE);
            ctrl_paint_ctrl(p_client_cont, TRUE);
            break;
        default:
            break;
    }

    return SID_NULL;
}

static STATEID StaRedtube_default_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);
    g_reqUrlFrom = SID_REDTUBE_DEF_VDOLIST_NORMAL;
    redtube_open_dlg(IDS_MSG_GET_URL);

    return SID_NULL;
}

static void SexRedtube_DefVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_DefVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_play_url(g_vdolist_index);
}

static STATEID StaRedtube_default_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_play_url_t *p_data = (redtube_vdo_play_url_t *)para1;
    ui_video_player_param vdoPlayerParam;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }
    ui_comm_dlg_close();

    redtube_pic_deinit();

    memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));
    vdoPlayerParam.name = g_curVdoPage.vdoList[g_vdolist_index].title;
    vdoPlayerParam.url_cnt = 1;
    vdoPlayerParam.pp_urlList = (u8 **)mtos_malloc(sizeof(u8 *));
    MT_ASSERT(vdoPlayerParam.pp_urlList != NULL);
    vdoPlayerParam.pp_urlList[0] = p_data->url;

    manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_REDTUBE, (u32)&vdoPlayerParam);

    if (vdoPlayerParam.pp_urlList != NULL)
    {
        mtos_free(vdoPlayerParam.pp_urlList);
    }
    return SID_NULL;
}

static STATEID StaRedtube_default_on_get_playurl_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }
    ui_comm_dlg_close();

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_default_on_urlreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    switch (g_reqUrlFrom)
    {
        case SID_REDTUBE_DEF_VDOLIST_NORMAL:
        case SID_REDTUBE_CATE_VDOLIST_NORMAL:
        case SID_REDTUBE_SEARCH_VDOLIST_NORMAL:
            redtube_update_curVdoPage();
            break;
        default:
            break;
    }

    return g_reqUrlFrom;
}

static STATEID StaRedtube_default_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    redtube_pic_init();

    switch (g_reqUrlFrom)
    {
        case SID_REDTUBE_DEF_VDOLIST_NORMAL:
        case SID_REDTUBE_CATE_VDOLIST_NORMAL:
        case SID_REDTUBE_SEARCH_VDOLIST_NORMAL:
            redtube_update_curVdoPage();
            break;
        default:
            break;
    }

    return g_reqUrlFrom;
}

static void SexRedtube_DefVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/* --------------------- Cate list: Category --------------------- */

static void SenRedtube_Category(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_cateList.cateList)
    {
        mtos_free(g_cateList.cateList);
        g_cateList.cateList = NULL;
    }
    g_cateList.count = 0;
    g_catelist_index = 0;
    g_totalCatePage = 1;
    g_curCatePageIdx = 0;
}

static void SexRedtube_Category(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_CateList(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    redtube_update_curCatePage();

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&comm_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_CateList(void)
{
    control_t *p_menu;
    control_t *p_client_cont, *p_catelist;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
        p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_CATELIST);
        MT_ASSERT(p_catelist != NULL);
        
        ctrl_set_sts(p_catelist, OBJ_STS_HIDE);
        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

static void SenRedtube_CateListReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_cate_list();
}

static STATEID StaRedtube_category_on_newcatelist_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_cate_page_t *p_data = (redtube_cate_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_cateList.cateList)
    {
        mtos_free(g_cateList.cateList);
        g_cateList.cateList = NULL;
    }

    g_cateList.count = p_data->count;
    if (p_data->count > 0 && p_data->cateList != NULL)
    {
        g_cateList.cateList = (redtube_cate_item_t *)mtos_malloc(p_data->count*sizeof(redtube_cate_item_t));
        MT_ASSERT(g_cateList.cateList != NULL);
        memcpy(g_cateList.cateList, p_data->cateList, p_data->count*sizeof(redtube_cate_item_t));
    }
    g_catelist_index = 0;
    g_totalCatePage = 1;
    g_curCatePageIdx = 0;

    redtube_update_curCatePage();
    return SID_REDTUBE_CATE_LIST_NORMAL;
}

static STATEID StaRedtube_category_on_get_catelist_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_category_on_catereq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_CATE_LIST_NORMAL;
}

static void SexRedtube_CateListReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_CateListNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static STATEID StaRedtube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_mainlist_cont, *p_mainlist;
    control_t *p_client_cont, *p_catelist;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_REDTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_CATELIST);
    MT_ASSERT(p_catelist != NULL);

    switch (msg)
    {
        case MSG_FOCUS_RIGHT:
            if (g_cateList.count > 0)
            {
                ctrl_process_msg(p_mainlist, MSG_LOSTFOCUS, 0, 0);

                ctrl_set_attr(p_catelist, OBJ_ATTR_ACTIVE);
                ctrl_set_sts(p_catelist, OBJ_STS_SHOW);
                ctrl_process_msg(p_catelist, MSG_GETFOCUS, 0, 0);

                ctrl_paint_ctrl(p_mainlist_cont, TRUE);
                ctrl_paint_ctrl(p_client_cont, TRUE);
            }
            break;
        case MSG_BACK:
        case MSG_MAIN:
            ctrl_process_msg(p_catelist, MSG_LOSTFOCUS, 0, 0);

            ctrl_set_attr(p_mainlist, OBJ_ATTR_ACTIVE);
            ctrl_set_sts(p_mainlist, OBJ_STS_SHOW);
            ctrl_process_msg(p_mainlist, MSG_GETFOCUS, 0, 0);

            ctrl_paint_ctrl(p_mainlist_cont, TRUE);
            ctrl_paint_ctrl(p_client_cont, TRUE);
            break;
    }

    return SID_NULL;
}

static BOOL    StcRedtube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_REDTUBE_CATELIST) ? TRUE : FALSE;
}

static STATEID StaRedtube_category_on_catelist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_catelist_index = list_get_focus_pos(ctrl);

    return SID_REDTUBE_CATE_1ST_VDO_REQ;
}

static void SexRedtube_CateListNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_Cate1stVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_cate_video(g_catelist_index, 0);
}

static STATEID StaRedtube_category_on_1stpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = p_data->count;
    if (p_data->count > 0 && p_data->vdoList != NULL)
    {
        g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
    }
    g_vdolist_index = 0;
    g_totalVdoPage = p_data->total_page;
    g_savVdoPageIdx = g_curVdoPageIdx = 1;

    return SID_REDTUBE_CATE_VDOLIST_NORMAL;
}

static STATEID StaRedtube_category_on_get_1stpagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_category_on_1stvdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_CATE_LIST_NORMAL;
}

static void SexRedtube_Cate1stVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_CateVdolist(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    redtube_update_curVdoPage();

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_CateVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);
        
        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);
        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

static void SenRedtube_CateVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_movlist_rend_idx = 0;
    redtube_pic_play_start();
}

static STATEID StaRedtube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageIdx > 1)
            {
                g_curVdoPageIdx--;
                return SID_REDTUBE_CATE_VDO_REQ;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageIdx < g_totalVdoPage )
            {
                g_curVdoPageIdx++;
                return SID_REDTUBE_CATE_VDO_REQ;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaRedtube_category_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageIdx > 1)
        {
            g_curVdoPageIdx--;
            return SID_REDTUBE_CATE_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageIdx < g_totalVdoPage )
        {
            g_curVdoPageIdx++;
            return SID_REDTUBE_CATE_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaRedtube_category_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);
    g_reqUrlFrom = SID_REDTUBE_CATE_VDOLIST_NORMAL;
    redtube_open_dlg(IDS_MSG_GET_URL);

    return SID_NULL;
}

static void SexRedtube_CateVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_CateVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_cate_video(g_catelist_index, g_curVdoPageIdx);
}

static STATEID StaRedtube_category_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = p_data->count;
    if (p_data->count > 0 && p_data->vdoList != NULL)
    {
        g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
    }
    if (g_curVdoPageIdx > g_savVdoPageIdx)
    {
        g_vdolist_index = 0;
    }
    else
    {
        g_vdolist_index = g_curVdoPage.count - 1;
    }
    g_savVdoPageIdx = g_curVdoPageIdx;

    redtube_update_curVdoPage();
    return SID_REDTUBE_CATE_VDOLIST_NORMAL;
}

static STATEID StaRedtube_category_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageIdx = g_savVdoPageIdx;

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_category_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_CATE_VDOLIST_NORMAL;
}

static void SexRedtube_CateVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_CateVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_play_url(g_vdolist_index);
}

static void SexRedtube_CateVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/* --------------------- Video list: Search --------------------- */

static void SenRedtube_Search(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }
    g_curVdoPage.count = 0;
    g_vdolist_index = 0;
    g_totalVdoPage = 1;
    g_savVdoPageIdx = g_curVdoPageIdx = 1;
}

static void SexRedtube_Search(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_SearchVdolist(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    redtube_update_curVdoPage();

    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_SearchVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);

        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        //ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);
        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

#define MAX_SEARCH_NAME_LEN     (16)
static u16 g_searchKey[MAX_SEARCH_NAME_LEN+1] = {0};

RET_CODE ui_redtube_edit_cb(u16* p_unistr)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_REDTUBE);

    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_INPUT_COMP, 0, 0);
    }
    return SUCCESS;
}

static void SenRedtube_Input(void)
{
    kb_param_t param;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    param.uni_str = g_searchKey;
    param.max_len = MAX_SEARCH_NAME_LEN;
    param.type = KB_INPUT_TYPE_SENTENCE;
    param.cb = ui_redtube_edit_cb;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
}

static void SexRedtube_Input(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_Searching(void)
{
    u8 key[MAX_SEARCH_NAME_LEN+1] = {0};

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    str_uni2asc(key, g_searchKey);
    //ui_redtube_set_max_item_cnt(REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    ui_redtube_search(key);
}

static STATEID StaRedtube_search_on_search_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = p_data->count;
    if (p_data->count > 0 && p_data->vdoList != NULL)
    {
        g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
    }
    g_vdolist_index = 0;
    g_totalVdoPage = p_data->total_page;
    g_savVdoPageIdx = g_curVdoPageIdx = 1;

    redtube_update_curVdoPage();
    return SID_REDTUBE_SEARCH_VDOLIST_NORMAL;
}

static STATEID StaRedtube_search_on_search_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_search_on_searchfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_SEARCH_VDOLIST_NORMAL;
}

static void SexRedtube_Searching(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_SearchVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_movlist_rend_idx = 0;
    redtube_pic_play_start();
}

static STATEID StaRedtube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageIdx > 1)
            {
                g_curVdoPageIdx--;
                return SID_REDTUBE_SEARCH_VDO_REQ;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageIdx < g_totalVdoPage )
            {
                g_curVdoPageIdx++;
                return SID_REDTUBE_SEARCH_VDO_REQ;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaRedtube_search_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageIdx > 1)
        {
            g_curVdoPageIdx--;
            return SID_REDTUBE_SEARCH_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageIdx < g_totalVdoPage )
        {
            g_curVdoPageIdx++;
            return SID_REDTUBE_SEARCH_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaRedtube_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s g_vdolist_index=%d\n", __FUNCTION__, g_vdolist_index);
    g_reqUrlFrom = SID_REDTUBE_SEARCH_VDOLIST_NORMAL;
    redtube_open_dlg(IDS_MSG_GET_URL);

    return SID_NULL;
}

static void SexRedtube_SearchVdolistNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_search_video(g_curVdoPageIdx);
}

static STATEID StaRedtube_search_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    redtube_vdo_page_t *p_data = (redtube_vdo_page_t *)para1;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = p_data->count;
    if (p_data->count > 0 && p_data->vdoList != NULL)
    {
        g_curVdoPage.vdoList = (redtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(redtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(redtube_vdo_item_t));
    }
    if (g_curVdoPageIdx > g_savVdoPageIdx)
    {
        g_vdolist_index = 0;
    }
    else
    {
        g_vdolist_index = g_curVdoPage.count - 1;
    }
    g_savVdoPageIdx = g_curVdoPageIdx;

    redtube_update_curVdoPage();
    return SID_REDTUBE_SEARCH_VDOLIST_NORMAL;
}

static STATEID StaRedtube_search_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_get_focus_id() != ROOT_ID_REDTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageIdx = g_savVdoPageIdx;

    redtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaRedtube_search_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    return SID_REDTUBE_SEARCH_VDOLIST_NORMAL;
}

static void SexRedtube_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenRedtube_SearchVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_redtube_get_play_url(g_vdolist_index);
}

static void SexRedtube_SearchVdolistUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/////////////////////////////////////////////////////////////////////////////

static RET_CODE redtube_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = Redtube_DispatchMsg(p_ctrl, msg, para1, para2);

    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@unaccepted msg, id=0x%04x\n", msg);
    }

    ui_redtube_free_msg_data(msg, para1, para2);

    return ret;
}

/*!
 * Video view entry
 */
RET_CODE ui_open_redtube(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_bmp = NULL;
    control_t *p_list_cont = NULL, *p_list = NULL;
    control_t *p_page_cont = NULL, *p_icon = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    control_t *p_catelist = NULL/*, *p_catelist_sbar = NULL*/;
    control_t *p_bottom_help = NULL;
    u8 i = 0;

    OS_PRINTF("@@@ui_open_redtube\n");

    g_mainlist_index = 0;
    g_vdolist_index = 0;
    g_catelist_index = 0;
    g_reqUrlFrom = SID_NULL;
    g_totalVdoPage = 0;

    redtube_pic_init();

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_REDTUBE, 0,
                                    IM_INDEX_REDTUBE_BANNER, IDS_REDTUBE);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, redtube_cont_keymap);
    ctrl_set_proc(p_menu, redtube_cont_proc);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_REDTUBE_BTM_HELP,
                                      REDTUBE_BOTTOM_HELP_X, REDTUBE_BOTTOM_HELP_Y,
                                      REDTUBE_BOTTOM_HELP_W, REDTUBE_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    //text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    //text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
    //text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(60, 0, 
                                REDTUBE_BOTTOM_HELP_W-60, REDTUBE_BOTTOM_HELP_H,  
                                &emp_help_data,  p_bottom_help);

    /*!
     * Create list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_REDTUBE_LIST_CONT,
                                  REDTUBE_LIST_CONTX, REDTUBE_LIST_CONTY,
                                  REDTUBE_LIST_CONTW, REDTUBE_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_REDTUBE_LEFT_BMP, 
                                        REDTUBE_BMP_X, REDTUBE_BMP_Y, 
                                        REDTUBE_BMP_W, REDTUBE_BMP_H, p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, IM_PIC_REDTUBE_LOGO);
    
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_REDTUBE_LIST,
                          REDTUBE_LIST_X, REDTUBE_LIST_Y,
                          REDTUBE_LISTW, REDTUBE_LISTH,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, redtube_mainlist_keymap);
    ctrl_set_proc(p_list, redtube_mainlist_proc);

    ctrl_set_mrect(p_list, REDTUBE_LIST_MIDL, REDTUBE_LIST_MIDT,
                  REDTUBE_LIST_MIDW + REDTUBE_LIST_MIDL, REDTUBE_LIST_MIDH + REDTUBE_LIST_MIDT);
    list_set_item_interval(p_list, REDTUBE_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &mainlist_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, REDTUBE_LIST_ITEM_COUNT, REDTUBE_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(mainlist_attr), REDTUBE_LIST_ITEM_NUM_ONE_PAGE);
    //list_set_focus_pos(p_list, g_mainlist_index);
    //list_select_item(p_list, g_mainlist_index);
    list_set_update(p_list, redtube_update_mainlist, 0);
    //ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

    for (i = 0; i < ARRAY_SIZE(mainlist_attr); i++)
    {
        list_set_field_attr(p_list, i, mainlist_attr[i].attr,
                            mainlist_attr[i].width, mainlist_attr[i].left, mainlist_attr[i].top);
        list_set_field_rect_style(p_list, i, mainlist_attr[i].rstyle);
        list_set_field_font_style(p_list, i, mainlist_attr[i].fstyle);
    }

    redtube_update_mainlist(p_list, list_get_valid_pos(p_list), REDTUBE_LIST_ITEM_NUM_ONE_PAGE, 0);

    //Page
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_REDTUBE_PAGE_CONT,
                                    REDTUBE_PAGE_CONTX, REDTUBE_PAGE_CONTY,
                                    REDTUBE_PAGE_CONTW, REDTUBE_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               REDTUBE_PAGE_ARROWL_X, REDTUBE_PAGE_ARROWL_Y,
                               REDTUBE_PAGE_ARROWL_W, REDTUBE_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               REDTUBE_PAGE_ARROWR_X, REDTUBE_PAGE_ARROWR_Y,
                               REDTUBE_PAGE_ARROWR_W, REDTUBE_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 REDTUBE_PAGE_NAME_X, REDTUBE_PAGE_NAME_Y,
                                 REDTUBE_PAGE_NAME_W, REDTUBE_PAGE_NAME_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_by_strid(p_ctrl, IDS_PAGE1);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_REDTUBE_PAGE_NUM,
                                 REDTUBE_PAGE_VALUE_X, REDTUBE_PAGE_VALUE_Y,
                                 REDTUBE_PAGE_VALUE_W, REDTUBE_PAGE_VALUE_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    //redtube_update_page_num(p_ctrl);

    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_REDTUBE_CLIENT_CONT,
                                  REDTUBE_CLIENT_CONTX, REDTUBE_CLIENT_CONTY,
                                  REDTUBE_CLIENT_CONTW, REDTUBE_CLIENT_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //Movie list
    p_movlist = ctrl_create_ctrl(CTRL_LIST, IDC_REDTUBE_MOVLIST, 
                                        REDTUBE_MOVLIST_X, REDTUBE_MOVLIST_Y,
                                        REDTUBE_MOVLIST_W, REDTUBE_MOVLIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_movlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_movlist, redtube_movlist_keymap);
    ctrl_set_proc(p_movlist, redtube_movlist_proc);

    ctrl_set_mrect(p_movlist, 
                    REDTUBE_MOVLIST_X, REDTUBE_MOVLIST_Y,
                    REDTUBE_MOVLIST_W, REDTUBE_MOVLIST_H);
    list_set_item_interval(p_movlist, REDTUBE_MOVLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_movlist, &movlist_item_rstyle);

    list_set_count(p_movlist, 0, REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_movlist, ARRAY_SIZE(movlist_attr), REDTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    //list_set_focus_pos(p_movlist, 0);
    list_set_update(p_movlist, redtube_update_vdolist, 0);

    for(i = 0; i < ARRAY_SIZE(movlist_attr); i++)
    {
        list_set_field_attr2(p_movlist, (u8)i, movlist_attr[i].attr,
                            movlist_attr[i].width, movlist_attr[i].height, movlist_attr[i].left, movlist_attr[i].top);
        list_set_field_rect_style(p_movlist, (u8)i, movlist_attr[i].rstyle);
        list_set_field_font_style(p_movlist, (u8)i, movlist_attr[i].fstyle);
    }
    ctrl_set_sts(p_movlist, OBJ_STS_HIDE);

    //create scroll bar
  /*  p_movlist_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_REDTUBE_MOVLIST_SBAR,
                            REDTUBE_MOVLIST_SBAR_X, REDTUBE_MOVLIST_SBAR_Y,
                            REDTUBE_MOVLIST_SBAR_W, REDTUBE_MOVLIST_SBAR_H,
                            p_client_cont, 0);
    ctrl_set_rstyle(p_movlist_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_movlist_sbar, 1);
    sbar_set_direction(p_movlist_sbar, 0);
    sbar_set_mid_rstyle(p_movlist_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_movlist_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    REDTUBE_MOVLIST_SBAR_W, REDTUBE_MOVLIST_SBAR_H - COMM_SBAR_VERTEX_GAP);
    list_set_scrollbar(p_movlist, p_movlist_sbar);
    ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);*/

    //Category list
    p_catelist = ctrl_create_ctrl(CTRL_LIST, IDC_REDTUBE_CATELIST, 
                                        REDTUBE_MOVLIST_X, REDTUBE_MOVLIST_Y,
                                        REDTUBE_MOVLIST_W, REDTUBE_MOVLIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_catelist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_catelist, redtube_catelist_keymap);
    ctrl_set_proc(p_catelist, redtube_catelist_proc);

    ctrl_set_mrect(p_catelist, 
                    REDTUBE_MOVLIST_X, REDTUBE_MOVLIST_Y,
                    REDTUBE_MOVLIST_W, REDTUBE_MOVLIST_H);
    list_set_item_interval(p_catelist, REDTUBE_MOVLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_catelist, &movlist_item_rstyle);

    list_set_count(p_catelist, 0, REDTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_catelist, ARRAY_SIZE(catelist_attr), REDTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_catelist, redtube_update_catelist, 0);

    for(i = 0; i < ARRAY_SIZE(catelist_attr); i++)
    {
        list_set_field_attr(p_catelist, (u8)i, catelist_attr[i].attr,
                            catelist_attr[i].width, catelist_attr[i].left, catelist_attr[i].top);
        list_set_field_rect_style(p_catelist, (u8)i, catelist_attr[i].rstyle);
        list_set_field_font_style(p_catelist, (u8)i, catelist_attr[i].fstyle);
    }
    ctrl_set_sts(p_catelist, OBJ_STS_HIDE);

    //create scroll bar
    #if 0
    p_catelist_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_REDTUBE_CATELIST_SBAR,
                            REDTUBE_MOVLIST_SBAR_X, REDTUBE_MOVLIST_SBAR_Y,
                            REDTUBE_MOVLIST_SBAR_W, REDTUBE_MOVLIST_SBAR_H,
                            p_client_cont, 0);
    ctrl_set_rstyle(p_catelist_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_catelist_sbar, 1);
    sbar_set_direction(p_catelist_sbar, 0);
    sbar_set_mid_rstyle(p_catelist_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_catelist_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    REDTUBE_MOVLIST_SBAR_W, REDTUBE_MOVLIST_SBAR_H - COMM_SBAR_VERTEX_GAP);
    list_set_scrollbar(p_catelist, p_catelist_sbar);
    ctrl_set_sts(p_catelist_sbar, OBJ_STS_HIDE);
    #endif

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

    ui_redtube_register_msg();
    Redtube_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_REDTUBE), NOTIFY_T_MSG, FALSE, MSG_OPEN_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE redtube_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@redtube_on_destory\n");

    Redtube_CloseStateTree();
    ui_redtube_unregister_msg();

    ui_video_c_destroy();

    redtube_pic_deinit();

    ui_release_app_data();

    return ERR_NOFEATURE;
}

static RET_CODE redtube_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_REDTUBE,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@redtube_on_open_cfm_dlg\n");

    if (Redtube_IsStateActive(SID_REDTUBE_ACTIVE))
    {
      dlg_data.content = para1;

      ui_comm_dlg_open(&dlg_data);

      p_root = fw_find_root_by_id(ROOT_ID_REDTUBE);
      if(p_root)
      {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
      }
    }

    return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(redtube_mainlist_keymap, NULL)
    //ON_EVENT(V_KEY_MENU, MSG_EXIT)
    //ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    //ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
    //ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(redtube_mainlist_keymap, NULL)

BEGIN_MSGPROC(redtube_mainlist_proc, list_class_proc)
    //ON_COMMAND(MSG_EXIT, redtube_on_mainlist_exit)
    //ON_COMMAND(MSG_EXIT_ALL, redtube_on_mainlist_exit_all)
    ON_COMMAND(MSG_FOCUS_UP, redtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, redtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, redtube_on_state_process)
    ON_COMMAND(MSG_SELECT, redtube_on_state_process)
END_MSGPROC(redtube_mainlist_proc, list_class_proc)

BEGIN_KEYMAP(redtube_movlist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(redtube_movlist_keymap, NULL)

BEGIN_MSGPROC(redtube_movlist_proc, list_class_proc)
    ON_COMMAND(MSG_BACK, redtube_on_state_process)
    ON_COMMAND(MSG_MAIN, redtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_UP, redtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, redtube_on_state_process)
    ON_COMMAND(MSG_PAGE_UP, redtube_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, redtube_on_state_process)
    ON_COMMAND(MSG_INFO, redtube_on_state_process)
    ON_COMMAND(MSG_SELECT, redtube_on_state_process)
END_MSGPROC(redtube_movlist_proc, list_class_proc)

BEGIN_KEYMAP(redtube_catelist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(redtube_catelist_keymap, NULL)

BEGIN_MSGPROC(redtube_catelist_proc, list_class_proc)
    ON_COMMAND(MSG_BACK, redtube_on_state_process)
    ON_COMMAND(MSG_MAIN, redtube_on_state_process)
    ON_COMMAND(MSG_SELECT, redtube_on_state_process)
END_MSGPROC(redtube_catelist_proc, list_class_proc)

BEGIN_KEYMAP(redtube_cont_keymap, ui_comm_root_keymap)

END_KEYMAP(redtube_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(redtube_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, redtube_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, redtube_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, redtube_on_state_process)

    ON_COMMAND(MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, redtube_on_state_process)
    ON_COMMAND(MSG_RT_EVT_GET_PAGE_VDO_FAIL, redtube_on_state_process)
    ON_COMMAND(MSG_RT_EVT_NEW_CATE_LIST_ARRIVE, redtube_on_state_process)
    ON_COMMAND(MSG_RT_EVT_GET_CATE_LIST_FAIL, redtube_on_state_process)
    ON_COMMAND(MSG_RT_EVT_NEW_PLAY_URL_ARRIVE, redtube_on_state_process)
    ON_COMMAND(MSG_RT_EVT_GET_PLAY_URL_FAIL, redtube_on_state_process)

    ON_COMMAND(MSG_OPEN_REQ, redtube_on_state_process)
    ON_COMMAND(MSG_EXIT, redtube_on_state_process)
    ON_COMMAND(MSG_EXIT_ALL, redtube_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, redtube_on_state_process)
    ON_COMMAND(MSG_DESTROY, redtube_on_destory)
    ON_COMMAND(MSG_INPUT_COMP, redtube_on_state_process)  

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, redtube_on_open_cfm_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, redtube_on_state_process)
    ON_COMMAND(MSG_UPDATE, redtube_on_state_process)
END_MSGPROC(redtube_cont_proc, ui_comm_root_proc)
#endif

#if 0
static void SenRedtube_DefInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    pic_stop();
    ctrl_get_frame(p_preview, &rect);
    ctrl_client2screen(p_preview, &rect);
    ui_pic_clear_rect(&rect, 0);
    if (strlen(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url) > 0)
    {
        ui_pic_play_by_url(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url, &rect, 0);
    }

    ui_comm_help_set_data(&info_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_DefInfo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_DefInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static STATEID StaRedtube_default_on_info_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_client_cont, *p_preview, *p_detail_cont, *p_detail/*, *p_detail_sbar*/;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_detail_cont = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_INFO_DETAIL_CONT);
    MT_ASSERT(p_detail_cont != NULL);
    p_detail = ctrl_get_child_by_id(p_detail_cont, IDC_REDTUBE_INFO_DETAIL);
    MT_ASSERT(p_detail != NULL);
    //p_detail_sbar = ctrl_get_child_by_id(p_detail_cont, IDC_REDTUBE_INFO_DETAIL_SBAR);
    //MT_ASSERT(p_detail_sbar != NULL);

    ctrl_set_sts(p_preview, OBJ_STS_HIDE);
    ctrl_set_sts(p_detail_cont, OBJ_STS_HIDE);
    ctrl_set_sts(p_detail, OBJ_STS_HIDE);
    //ctrl_set_sts(p_detail_sbar, OBJ_STS_HIDE);
    ctrl_process_msg(p_detail, MSG_LOSTFOCUS, 0, 0);

    redtube_update_curVdoPage();
    return SID_NULL;
}

static BOOL    StcRedtube_is_on_infotext(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_REDTUBE_INFO_DETAIL) ? TRUE : FALSE;
}

static STATEID StaRedtube_default_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_reqUrlFrom = SID_REDTUBE_DEF_INFO_NORMAL;
    redtube_open_dlg(IDS_MSG_GET_URL);

    ui_redtube_set_item_index(g_vdolist_index);
    ui_redtube_get_video_play_url();

    return SID_NULL;
}

static void SexRedtube_DefInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_DefInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SexRedtube_DefInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_CateInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    pic_stop();
    ctrl_get_frame(p_preview, &rect);
    ctrl_client2screen(p_preview, &rect);
    ui_pic_clear_rect(&rect, 0);
    if (strlen(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url) > 0)
    {
        ui_pic_play_by_url(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url, &rect, 0);
    }

    ui_comm_help_set_data(&info_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_CateInfo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_CateInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static STATEID StaRedtube_category_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_reqUrlFrom = SID_REDTUBE_CATE_INFO_NORMAL;
    redtube_open_dlg(IDS_MSG_GET_URL);

    ui_redtube_set_item_index(g_vdolist_index);
    ui_redtube_get_video_play_url();

    return SID_NULL;
}

static void SexRedtube_CateInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_CateInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SexRedtube_CateInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_SearchInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_menu = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_REDTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_REDTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    pic_stop();
    ctrl_get_frame(p_preview, &rect);
    ctrl_client2screen(p_preview, &rect);
    ui_pic_clear_rect(&rect, 0);
    if (strlen(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url) > 0)
    {
        ui_pic_play_by_url(g_curVdoPage.vdoList[g_vdolist_index].info_thumnail_url, &rect, 0);
    }

    ui_comm_help_set_data(&info_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static void SexRedtube_SearchInfo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    pic_stop();
}

static void SenRedtube_SearchInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static STATEID StaRedtube_search_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_reqUrlFrom = SID_REDTUBE_SEARCH_INFO;
    redtube_open_dlg(IDS_MSG_GET_URL);

    ui_redtube_set_item_index(g_vdolist_index);
    ui_redtube_get_video_play_url();

    return SID_NULL;
}

static void SexRedtube_SearchInfoNormal(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SenRedtube_SearchInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

static void SexRedtube_SearchInfoUrlReq(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}


#endif
