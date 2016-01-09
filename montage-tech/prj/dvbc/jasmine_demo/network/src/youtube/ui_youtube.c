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
#include "ui_youtube_api.h"

#include "ui_youtube_gprot.h"
#include "ui_youtube_prot.h"

#include "SM_StateMachine.h"
#include "States_Youtube.h"
#include "ui_video.h"
#include "ui_picture_api.h"
#include "ui_video_player_gprot.h"
#include "ui_keyboard_v2.h"

/*!
 Const value
 */
//plist list container
#define YOUTUBE_LIST_CONTX   120
#define YOUTUBE_LIST_CONTY   100
#define YOUTUBE_LIST_CONTW  200
#define YOUTUBE_LIST_CONTH   570

//bmp
#define YOUTUBE_BMP_X   0
#define YOUTUBE_BMP_Y   0
#define YOUTUBE_BMP_W  YOUTUBE_LIST_CONTW
#define YOUTUBE_BMP_H   90


//List
#define YOUTUBE_LIST_X  20
#define YOUTUBE_LIST_Y  90
#define YOUTUBE_LISTW  160
#define YOUTUBE_LISTH  400

#define YOUTUBE_LIST_MIDL  0//5
#define YOUTUBE_LIST_MIDT  0//5
#define YOUTUBE_LIST_MIDW  (YOUTUBE_LISTW - 2 * YOUTUBE_LIST_MIDL)
#define YOUTUBE_LIST_MIDH  (YOUTUBE_LISTH - 2 * YOUTUBE_LIST_MIDT)

#define YOUTUBE_LIST_ICON_VGAP 4
#define YOUTUBE_LIST_ITEM_NUM_ONE_PAGE  9//12
#define YOUTUBE_LIST_ITEM_COUNT     8

//page container
#define YOUTUBE_PAGE_CONTX 330
#define YOUTUBE_PAGE_CONTY 100
#define YOUTUBE_PAGE_CONTW 830
#define YOUTUBE_PAGE_CONTH 50

//page arrow left
#define YOUTUBE_PAGE_ARROWL_X       150
#define YOUTUBE_PAGE_ARROWL_Y       7
#define YOUTUBE_PAGE_ARROWL_W      36
#define YOUTUBE_PAGE_ARROWL_H       36
//page arrow right
#define YOUTUBE_PAGE_ARROWR_X       (YOUTUBE_PAGE_CONTW - YOUTUBE_PAGE_ARROWL_X - YOUTUBE_PAGE_ARROWR_W)
#define YOUTUBE_PAGE_ARROWR_Y       7
#define YOUTUBE_PAGE_ARROWR_W      36
#define YOUTUBE_PAGE_ARROWR_H       36
//page name
#define YOUTUBE_PAGE_NAME_X  250
#define YOUTUBE_PAGE_NAME_Y  7
#define YOUTUBE_PAGE_NAME_W 100
#define YOUTUBE_PAGE_NAME_H  36
//page value
#define YOUTUBE_PAGE_VALUE_X  450
#define YOUTUBE_PAGE_VALUE_Y  7
#define YOUTUBE_PAGE_VALUE_W 100
#define YOUTUBE_PAGE_VALUE_H  36

//client area
#define YOUTUBE_CLIENT_CONTX 330//(YOUTUBE_LIST_CONTX + YOUTUBE_LIST_CONTW)
#define YOUTUBE_CLIENT_CONTY 160//(YOUTUBE_PAGE_CONTY + YOUTUBE_PAGE_CONTH)
#define YOUTUBE_CLIENT_CONTW 830
#define YOUTUBE_CLIENT_CONTH 450//(500)

//movie list
#define YOUTUBE_MOVLIST_X 0
#define YOUTUBE_MOVLIST_Y 0
#define YOUTUBE_MOVLIST_W (YOUTUBE_CLIENT_CONTW  /*-YOUTUBE_MOVLIST_SBAR_W - 6*/)
#define YOUTUBE_MOVLIST_H YOUTUBE_CLIENT_CONTH
#define YOUTUBE_MOVLIST_ITEM_V_GAP      10//(2)

#define YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE  4

#if 0
#define YOUTUBE_MOVLIST_SBAR_X        (YOUTUBE_CLIENT_CONTW - YOUTUBE_MOVLIST_SBAR_W)
#define YOUTUBE_MOVLIST_SBAR_Y        (YOUTUBE_MOVLIST_Y)
#define YOUTUBE_MOVLIST_SBAR_W        (6)
#define YOUTUBE_MOVLIST_SBAR_H        (YOUTUBE_MOVLIST_H)
#endif

#define YOUTUBE_RATED_ICON_Y            (45)
#define YOUTUBE_RATED_ICON_W            (20)
#define YOUTUBE_RATED_ICON_H            (20)


//category list
#define YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE  6

//option list
#define YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE  6

//info
#define YOUTUBE_INFO_PIC_X  (215)//(220)//(230)
#define YOUTUBE_INFO_PIC_Y  (0)
#define YOUTUBE_INFO_PIC_W  (400)//(390)//(370)
#define YOUTUBE_INFO_PIC_H  (280)//(320)

#define YOUTUBE_INFO_DETAIL_CONTX       (0)
#define YOUTUBE_INFO_DETAIL_CONTY       (YOUTUBE_INFO_PIC_Y + YOUTUBE_INFO_PIC_H + 10)
#define YOUTUBE_INFO_DETAIL_CONTW       (YOUTUBE_CLIENT_CONTW)
#define YOUTUBE_INFO_DETAIL_CONTH       (YOUTUBE_CLIENT_CONTH - YOUTUBE_INFO_DETAIL_CONTY)

#define YOUTUBE_INFO_DETAIL_X       COMM_BOX1_BORDER
#define YOUTUBE_INFO_DETAIL_Y       COMM_BOX1_BORDER
#define YOUTUBE_INFO_DETAIL_W       (YOUTUBE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - COMM_BOX1_BORDER - YOUTUBE_INFO_DETAIL_SBAR_W - 10)
#define YOUTUBE_INFO_DETAIL_H       (YOUTUBE_INFO_DETAIL_CONTH - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

#define YOUTUBE_INFO_DETAIL_MIDX    (0)
#define YOUTUBE_INFO_DETAIL_MIDY    (0)
#define YOUTUBE_INFO_DETAIL_MIDW    (YOUTUBE_INFO_DETAIL_W)
#define YOUTUBE_INFO_DETAIL_MIDH    (YOUTUBE_INFO_DETAIL_H)

#define YOUTUBE_INFO_DETAIL_SBAR_X        (YOUTUBE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - YOUTUBE_INFO_DETAIL_SBAR_W)
#define YOUTUBE_INFO_DETAIL_SBAR_Y        (YOUTUBE_INFO_DETAIL_Y)
#define YOUTUBE_INFO_DETAIL_SBAR_W        (6)
#define YOUTUBE_INFO_DETAIL_SBAR_H        (YOUTUBE_INFO_DETAIL_H)

//bottom help container
#define YOUTUBE_BOTTOM_HELP_X  330//(YOUTUBE_LIST_CONTX + YOUTUBE_LIST_CONTW + 20)
#define YOUTUBE_BOTTOM_HELP_Y  620// (YOUTUBE_CLIENT_CONTY + YOUTUBE_CLIENT_CONTH + 10)
#define YOUTUBE_BOTTOM_HELP_W  830//(COMM_BG_W - YOUTUBE_BOTTOM_HELP_X - 20 - YOUTUBE_LIST_CONTX)
#define YOUTUBE_BOTTOM_HELP_H  50


/*!
 Macro
 */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define MAX_OPT_NAME_LEN    20

typedef struct
{ 
  u16 id;
  u16 name;
} youtube_optlist_item;

/*!
 * Type define
 */

/*!
 * Video view control id
 */
typedef enum
{
    IDC_YOUTUBE_LIST_CONT = 1,
    IDC_YOUTUBE_LEFT_BMP,
    IDC_YOUTUBE_LIST,
    IDC_YOUTUBE_PAGE_CONT,
    IDC_YOUTUBE_PAGE_NUM,

    IDC_YOUTUBE_CLIENT_CONT,
    IDC_YOUTUBE_MOVLIST,
  //  IDC_YOUTUBE_MOVLIST_SBAR,
    IDC_YOUTUBE_CATELIST,
    //IDC_YOUTUBE_CATELIST_SBAR,
    IDC_YOUTUBE_OPTLIST,
    //IDC_YOUTUBE_OPTLIST_SBAR,
    IDC_YOUTUBE_INFO_PIC,
    IDC_YOUTUBE_INFO_DETAIL_CONT,
    IDC_YOUTUBE_INFO_DETAIL,
    IDC_YOUTUBE_INFO_DETAIL_SBAR,

    IDC_YOUTUBE_BTM_HELP,
}ui_youtube_ctrl_id_t;

typedef enum
{
    VDOLIST_FIELD_ID_LOGO,
    VDOLIST_FIELD_ID_TITLE,
    VDOLIST_FIELD_ID_AUTHOR,
    VDOLIST_FIELD_ID_DURATION,
    VDOLIST_FIELD_ID_RATED_START,
    VDOLIST_FIELD_ID_RATED_END = VDOLIST_FIELD_ID_RATED_START + YOUTUBE_RATED_ICON_NUM - 1,
    VDOLIST_FIELD_ID_VIEW_COUNT
}ui_youtube_vdolist_field_id_t;

typedef enum
{
    YOUTUBE_RATED_MASK_EMPTY = 0x00,
    YOUTUBE_RATED_MASK_HALF = 0x02,
    YOUTUBE_RATED_MASK_FULL = 0x03,
}ui_youtube_rated_mask_t;

typedef struct
{ 
    u16 name;
} youtube_mainlist_item;

/*!
 * Function define
 */
u16 youtube_mainlist_keymap(u16 key);
RET_CODE youtube_mainlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 youtube_movlist_keymap(u16 key);
RET_CODE youtube_movlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 youtube_catelist_keymap(u16 key);
RET_CODE youtube_catelist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 youtube_optlist_keymap(u16 key);
RET_CODE youtube_optlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 youtube_info_detail_keymap(u16 key);
RET_CODE youtube_info_detail_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 youtube_cont_keymap(u16 key);
RET_CODE youtube_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;
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
  FSI_WHITE,
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
     5*YOUTUBE_RATED_ICON_W, 29, 
     YOUTUBE_MOVLIST_W - 5*YOUTUBE_RATED_ICON_W - 10, 12, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_START
    {LISTFIELD_TYPE_ICON,
     YOUTUBE_RATED_ICON_W, YOUTUBE_RATED_ICON_H,  
     YOUTUBE_MOVLIST_W - 5*YOUTUBE_RATED_ICON_W - 10, YOUTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     YOUTUBE_RATED_ICON_W, YOUTUBE_RATED_ICON_H,  
     YOUTUBE_MOVLIST_W - 4*YOUTUBE_RATED_ICON_W - 10, YOUTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     YOUTUBE_RATED_ICON_W, YOUTUBE_RATED_ICON_H,  
     YOUTUBE_MOVLIST_W - 3*YOUTUBE_RATED_ICON_W - 10, YOUTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     YOUTUBE_RATED_ICON_W, YOUTUBE_RATED_ICON_H,  
     YOUTUBE_MOVLIST_W - 2*YOUTUBE_RATED_ICON_W - 10, YOUTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_END
    {LISTFIELD_TYPE_ICON,
     YOUTUBE_RATED_ICON_W, YOUTUBE_RATED_ICON_H,  
     YOUTUBE_MOVLIST_W - YOUTUBE_RATED_ICON_W - 10, YOUTUBE_RATED_ICON_Y, 
     &movlist_field_rstyle, &movlist_field_fstyle_small},
    //VDOLIST_FIELD_ID_VIEW_COUNT
    {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
     190, 29, YOUTUBE_MOVLIST_W - 200, 70, &movlist_field_rstyle, &movlist_field_fstyle_small}
};

static const list_field_attr_t catelist_attr[] =
{
    {LISTFIELD_TYPE_UNISTR | STL_LEFT,
     600, 10, 0, &movlist_field_rstyle, &movlist_field_fstyle}
};

static const list_field_attr_t optlist_attr[] =
{
    {LISTFIELD_TYPE_STRID | STL_LEFT,
     600, 10, 0, &movlist_field_rstyle, &movlist_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, YOUTUBE_MOVLIST_W - 100, 0, &movlist_field_rstyle, &movlist_field_fstyle}
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

static comm_help_data_t info_help_data =
{
    3, 3,
    {IDS_MOVE, IDS_PLAY, IDS_RETURN},
    {IM_HELP_MOVE, IM_HELP_OK, IM_HELP_MENU}
};

static comm_help_data_t comm_help_data =
{
    4, 4,
    {IDS_MOVE, IDS_PAGE, IDS_SELECT, IDS_RETURN},
    {IM_HELP_MOVE, IM_HELP_PAGE, IM_HELP_OK, IM_HELP_MENU}
};

//main list priv data
static youtube_mainlist_item mainlist[YOUTUBE_LIST_ITEM_COUNT] = 
{
    {
        IDS_MOST_POPULATE
    },
    {
        IDS_MOST_VIEWED
    },
    {
        IDS_TOP_RATED
    },
    {
        IDS_CATEGORY
    },
    {
        IDS_SEARCH
    },
    {
        IDS_COUNTRY
    },
    {
        IDS_RESOLUTION
    },
    {
        IDS_TIME_MODE
    },
};
static s16 g_mainlist_index = -1;

//list content
static ui_youtube_vdo_page_t g_curVdoPage = {0};
static u8  g_vdolist_index = 0;
static u32 g_totalVdoPage = 0;
static u32 g_curVdoPageNum = 0;
static u32 g_savVdoPageNum = 0;

static ui_youtube_cate_page_t g_cateList = {0};
static u8  g_catelist_index = 0;
static u8  g_totalCatePage = 0;
static u8  g_curCatePageNum = 0;

static u8 g_totalOptPage = 0;
static u8  g_curOptPageNum = 0;

static u16 g_movlist_rend_idx = 0;
static u8  g_curFeedType = INVALD_FEED_TYPE;

static STATEID g_reqUrlFrom = SID_NULL;

static u16 g_exitMsg;

static youtube_optlist_item g_country_opt[] = 
{
    {ALL_COUNTRY, IDS_ALL_COUNTRY},
    {AR, IDS_ARGENTINA},
    {AU, IDS_AUSTRALIA},
    {AT, IDS_AUSTRIA},
    {BE, IDS_BELGIUM},
    {BR, IDS_BRAZIL},
    {CA, IDS_CANADA},
    {CL, IDS_CHILE},
    {CO, IDS_COLOMBIA},
    {CZ, IDS_CZECH_REPUBLIC},
    {EG, IDS_EGYPT},
    {FR, IDS_FRANCE},
    {DE, IDS_GERMANY},
    {GB, IDS_GREAT_BRITAIN},
    {HK, IDS_HONG_KONG},
    {HU, IDS_HUNGARY},
    {IN, IDS_INDIA},
    {IE, IDS_IRELAND},
    {IL, IDS_ISRAEL},
    {IT, IDS_ITALY},
    {JP, IDS_JAPAN},
    {JO, IDS_JORDAN},
    {MY, IDS_MALAYSIA},
    {MX, IDS_MEXICO},
    {MA, IDS_MOROCCO},
    {NL, IDS_NETHERLANDS},
    {NZ, IDS_NEW_ZEALAND},
    {PE, IDS_PERU},
    {PH, IDS_PHILIPPINES},
    {PL, IDS_POLAND},
    {RU, IDS_RUSSIAN_FEDERATION},
    {SA, IDS_SAUDI_ARABIA},
    {SG, IDS_SINGAPORE},
    {ZA, IDS_SOUTH_AFRICA},
    {KR, IDS_REPUBLIC_OF_KOREA},
    {ES, IDS_SPAIN},
    {SE, IDS_SWEDEN},
    {CH, IDS_CHINA},
    {TW, IDS_TAIWAN},
    {AE, IDS_UNITED_ARAB_EMIRATES},
    {US, IDS_UNITED_STATES},
};

static youtube_optlist_item g_resolution_opt[] = 
{
    {YOUTUBE_RESOLUTION_NORMAL, IDS_NORMAL_RESOLUTION},
    {YOUTUBE_RESOLUTION_HIGH, IDS_HIGH_RESOLUTION}
};

static youtube_optlist_item g_timemode_opt[] = 
{
    {TODAY, IDS_TODAY},
    {THIS_WEEK, IDS_THIS_WEEK},
    {THIS_MONTH, IDS_THIS_MONTH},
    {ALL_TIME, IDS_ALL_TIME},
};

//STATEID g_baseStateId = SID_NULL;


/*================================================================================================
                           video play view internel function
 ================================================================================================*/
static void _list_get_item_size(control_t *p_ctrl, u16 *p_w, u16 *p_h)
{
  //ctrl_list_data_t *p_data = NULL;
  u8 lines = 0;

  
  u8 columns = list_get_columns(p_ctrl);
  u16 page = list_get_page(p_ctrl);
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(page >= columns);
  MT_ASSERT(page % columns == 0);

  lines = page / columns;

  *p_w = ((p_ctrl->mrect.right - p_ctrl->mrect.left - YOUTUBE_LIST_ICON_VGAP *
     (columns - 1)) / columns);

  *p_h = (p_ctrl->mrect.bottom - p_ctrl->mrect.top - YOUTUBE_LIST_ICON_VGAP * (lines - 1)) / lines;

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
      (YOUTUBE_LIST_ICON_VGAP + h));

    left =  w * ((index - current_pos) / lines) + p_ctrl->mrect.left;
  }

  set_rect(p_item_rect, left, top, (left + w), (s16)(top + h));

}

static void youtube_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void youtube_pic_deinit(void)
{
  pic_stop();

  ui_pic_release();
}

static RET_CODE youtube_pic_play_start()
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    rect_t rect;
    rect_t item_rect;

    OS_PRINTF("@@@youtube_pic_play_start g_movlist_rend_idx=%d\n", g_movlist_rend_idx);
    while (g_movlist_rend_idx < g_curVdoPage.count)
    {
        if (strlen(g_curVdoPage.vdoList[g_movlist_rend_idx].thumnail_url) > 0)
        {
            p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
            p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
            p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
            MT_ASSERT(p_movlist != NULL);

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

static void youtube_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
  char num[22] = {0};

  sprintf(num, "%ld/%ld", page_num, total_page);

  text_set_content_by_ascstr(p_ctrl, num);
}

/*!
 * list update function
 */
static RET_CODE youtube_update_mainlist(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(p_list, i, 0, mainlist[i].name);
    }

    return SUCCESS;
}

static RET_CODE youtube_update_vdolist(control_t *p_movlist, u16 start, u16 size, u32 context)
{
    youtube_vdo_item_t *p_vdo;
    u16 *p_unistr;
    u32 view_count;
    u16 uni_str[MAX_TITLE_LEN+1];
    u16 value_tab[4];
    u16 cnt, rated_mask;
    u16 icon_id = RSC_INVALID_ID;
    u16 i;
    u8  num_str[4];
    u8  asc_str[16];
    u8  j;
    u8 flag;
    BOOL b_first_do;

    OS_PRINTF("@@@youtube_update_vdolist\n");

    cnt = g_curVdoPage.count;
    p_vdo = g_curVdoPage.vdoList;
    p_unistr = (u16 *)gui_get_string_addr(IDS_VIEW);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_icon(p_movlist, i, VDOLIST_FIELD_ID_LOGO, IM_INDEX_NETWORK_YOUTUBE_PIC);

        list_set_field_content_by_unistr(p_movlist, i, VDOLIST_FIELD_ID_TITLE, p_vdo[i].title);

        list_set_field_content_by_unistr(p_movlist, i, VDOLIST_FIELD_ID_AUTHOR, p_vdo[i].author);

        list_set_field_content_by_ascstr(p_movlist, i, VDOLIST_FIELD_ID_DURATION, p_vdo[i].duration);

        rated_mask = p_vdo[i].rated_mask;
        for (j = VDOLIST_FIELD_ID_RATED_END; j >= VDOLIST_FIELD_ID_RATED_START; j--)
        {
            flag = (u8) (rated_mask & 0x0003);
            switch (flag)
            {
                case YOUTUBE_RATED_MASK_FULL:
                    icon_id = IM_ICON_YOUTUBE_FLAG1;
                    break;
                case YOUTUBE_RATED_MASK_HALF:
                    icon_id = IM_ICON_YOUTUBE_FLAG2;
                    break;
                case YOUTUBE_RATED_MASK_EMPTY:
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
        uni_strcat(uni_str, p_unistr, MAX_TITLE_LEN);
        list_set_field_content_by_unistr(p_movlist, i, VDOLIST_FIELD_ID_VIEW_COUNT, uni_str);
    }

    return SUCCESS;
}

static RET_CODE youtube_update_catelist(control_t *p_movlist, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    youtube_cate_item_t *p_cate;

    OS_PRINTF("@@@youtube_update_catelist\n");

    cnt = g_cateList.count;
    p_cate = g_cateList.cateList;

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_unistr(p_movlist, i, 0, p_cate[i].cate_name);
    }

    return SUCCESS;
}

static RET_CODE youtube_update_optlist(control_t *p_optlist, u16 start, u16 size, u32 context)
{
    youtube_optlist_item *p_item = NULL;
    youtube_config_t config;
    u16 i, cnt, selected_id = 0;

    OS_PRINTF("@@@youtube_update_optlist\n");

    cnt = list_get_count(p_optlist);
    sys_status_get_youtube_config_info(&config);

    switch(mainlist[g_mainlist_index].name)
    {
        case IDS_COUNTRY:
            p_item = g_country_opt;
            selected_id = config.region_id;
            break;
        case IDS_RESOLUTION:
            p_item = g_resolution_opt;
            selected_id = config.resolution;
            break;
        case IDS_TIME_MODE:
            p_item = g_timemode_opt;
            selected_id = config.time_mode;
            break;
        default:
            MT_ASSERT(0);
            break;
    }

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(p_optlist, i, 0, p_item[i].name);
        list_set_field_content_by_icon(p_optlist, i, 1, 
                                (p_item[i].id == selected_id) ? IM_TV_SELECT : RSC_INVALID_ID);
    }

    return SUCCESS;
}

static void youtube_update_curVdoPage(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;
    rect_t rect;

    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);
    //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST_SBAR);
    //MT_ASSERT(p_movlist_sbar != NULL);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);

    list_set_count(p_movlist, g_curVdoPage.count, YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_movlist, g_vdolist_index);

    ctrl_set_attr(p_movlist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_movlist, OBJ_STS_SHOW);
    //ctrl_set_attr(p_movlist_sbar, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_movlist_sbar, OBJ_STS_SHOW);

    if (!ctrl_is_onfocus(p_mainlist))
    {
        ctrl_process_msg(p_movlist, MSG_GETFOCUS, 0, 0);
    }

    youtube_update_page_num(p_ctrl, g_savVdoPageNum, g_totalVdoPage);
    youtube_update_vdolist(p_movlist, 0, YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE, 0);

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

static void youtube_open_dlg(u16 str_id)
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

static void youtube_open_cfm_dlg(u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////

static void SenYoutube_Inactive(void)
{
    OS_PRINTF("@@@SenYoutube_Inactive\n");
}

static STATEID  ui_youtube_get_cur_entry(void)
{
    STATEID stateID = SID_NULL;

    switch(mainlist[g_mainlist_index].name)
    {
        case IDS_MOST_POPULATE:
            g_curFeedType = STAND_FEED_TOP_POPULAR;
            stateID = SID_YOUTUBE_MOST_POPU;
            break;
        case IDS_MOST_VIEWED:
            g_curFeedType = STAND_FEED_TOP_FAVORITE;
            stateID = SID_YOUTUBE_MOST_POPU;
            break;
        case IDS_TOP_RATED:
            g_curFeedType = STAND_FEED_TOP_RATED;
            stateID = SID_YOUTUBE_MOST_POPU;
            break;
        case IDS_CATEGORY:
            g_curFeedType = VIDEO_FEED;
            stateID = SID_YOUTUBE_CATEGORY;
            break;
        case IDS_SEARCH:
            stateID = SID_YOUTUBE_SEARCH;
            break;
        case IDS_COUNTRY:
            //g_curFeedType = INVALD_FEED_TYPE;
            stateID = SID_YOUTUBE_COUNTRY;
            break;
        case IDS_RESOLUTION:
            //g_curFeedType = INVALD_FEED_TYPE;
            stateID = SID_YOUTUBE_RESOLUTION;
            break;
        case IDS_TIME_MODE:
            //g_curFeedType = INVALD_FEED_TYPE;
            stateID = SID_YOUTUBE_TIME_MODE;
            break;
        default:
            break;
    }

    return stateID;
}

static STATEID StaYoutube_on_open_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_mainlist_cont, *p_mainlist;

    OS_PRINTF("@@@StaYoutube_on_open_req\n");

    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);

    list_set_focus_pos(p_mainlist, g_mainlist_index);
    list_select_item(p_mainlist, g_mainlist_index);

    ctrl_paint_ctrl(p_mainlist, TRUE);

    ui_youtube_dp_init();

    return ui_youtube_get_cur_entry();
}

static void SexYoutube_Inactive(void)
{
    OS_PRINTF("@@@SexYoutube_Inactive\n");
}

static void SenYoutube_Active(void)
{
    youtube_config_t config;

    OS_PRINTF("@@@SenYoutube_Active\n");
    youtube_pic_init();

    sys_status_get_youtube_config_info(&config);
    ui_youtube_set_country((YT_REGION_ID_T) config.region_id);
    ui_youtube_set_time_mode((YT_TIME_T) config.time_mode);
}

static BOOL StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_YOUTUBE_LIST) ? TRUE : FALSE;
}

static STATEID StaYoutube_on_mainlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;

    OS_PRINTF("@@@StaYoutube_on_mainlist_selected\n");

    focus = list_get_focus_pos(ctrl);
    g_mainlist_index = focus;

    list_select_item(ctrl, focus);
    ctrl_paint_ctrl(ctrl, TRUE);

    return ui_youtube_get_cur_entry();
}

static STATEID StaYoutube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_on_active_exit\n");

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

static void SexYoutube_Active(void)
{
    control_t *p_root = NULL;

    OS_PRINTF("@@@SexYoutube_Active\n");

    ui_youtube_dp_deinit();
 
    youtube_pic_deinit();

    if (g_exitMsg != MSG_SAVE)
    {
        p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
        if(p_root)
        {
          fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, g_exitMsg, 0, 0);
        }
    }
}


/* --------------------- Video list: Most Popu, Most Viewed, Top Rated ---------------------*/

static void SenYoutube_MostPopu(void)
{
    OS_PRINTF("@@@SenYoutube_MostPopu g_curFeedType=%d\n", g_curFeedType);
    ui_youtube_set_max_item_cnt(YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    ui_youtube_request_open(g_curFeedType);
}

static void SexYoutube_MostPopu(void)
{
    OS_PRINTF("@@@SexYoutube_MostPopu\n");
    ui_youtube_request_close();
}

static void SenYoutube_MpReqOpen(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL;

    OS_PRINTF("@@@SenYoutube_MpReqOpen\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    MT_ASSERT(p_client_cont != NULL);

    ctrl_paint_ctrl(p_client_cont, TRUE);
}

static BOOL StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (FEED_TYPE(para2) == g_curFeedType) ? TRUE : FALSE;
}

static STATEID StaYoutube_most_popu_on_req_open_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_most_popu_on_req_open_succ\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    g_vdolist_index = 0;
    g_totalVdoPage = (p_data->total_count + YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE - 1) / YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE;
    g_savVdoPageNum = g_curVdoPageNum = 1;

    return SID_YOUTUBE_MP_VDOLIST;
}

static STATEID StaYoutube_most_popu_on_req_open_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_req_open_fail\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = 0;
    g_vdolist_index = 0;
    g_totalVdoPage = 1;
    g_savVdoPageNum = g_curVdoPageNum = 1;

    youtube_open_cfm_dlg(IDS_DATA_ERROR);
    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_reqopenfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_MP_VDOLIST;
}

static void SexYoutube_MpReqOpen(void)
{
    OS_PRINTF("@@@SexYoutube_MpReqOpen\n");
}

static void SenYoutube_MpVdolist(void)
{
    OS_PRINTF("@@@SenYoutube_MpVdolist\n");
    youtube_update_curVdoPage();
}

static void SexYoutube_MpVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    rect_t rect;

    OS_PRINTF("@@@SexYoutube_MpVdolist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);

        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        //ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);

        //ctrl_paint_ctrl(p_client_cont, TRUE);
        
        //clear pic layer
        pic_stop();

        ctrl_get_frame(p_movlist, &rect);
        ctrl_client2screen(p_movlist, &rect);
        rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
        rect.right = rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
        ui_pic_clear_rect(&rect, 0);
        
    }
}

static void SenYoutube_MpVdolistNormal(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_MpVdolistNormal\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);

    g_movlist_rend_idx = 0;
    youtube_pic_play_start();
}

static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL;

    OS_PRINTF("@@@StaYoutube_most_popu_on_change_focus\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
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

static STATEID StaYoutube_most_popu_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_updown g_vdolist_index=%d\n", g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageNum > 1)
            {
                ui_youtube_get_prev_page();
                g_curVdoPageNum--;
                return SID_YOUTUBE_MP_REQUEST;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageNum < g_totalVdoPage)
            {
                ui_youtube_get_next_page();
                g_curVdoPageNum++;
                return SID_YOUTUBE_MP_REQUEST;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageNum > 1)
        {
            ui_youtube_get_prev_page();
            g_curVdoPageNum--;
            return SID_YOUTUBE_MP_REQUEST;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageNum < g_totalVdoPage)
        {
            ui_youtube_get_next_page();
            g_curVdoPageNum++;
            return SID_YOUTUBE_MP_REQUEST;
        }
    }

    return SID_NULL;
}

static STATEID StaYoutube_most_popu_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;

    OS_PRINTF("@@@StaYoutube_most_popu_pic_draw_end\n");

    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);

    list_set_field_content_by_icon(p_movlist, g_movlist_rend_idx, VDOLIST_FIELD_ID_LOGO, RSC_INVALID_ID);
    //ctrl_paint_ctrl(p_movlist, TRUE);
    list_draw_field_ext(p_movlist, g_movlist_rend_idx, VDOLIST_FIELD_ID_LOGO, TRUE);

    pic_stop();
    g_movlist_rend_idx++;
    youtube_pic_play_start();
    return SID_NULL;
}

static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_pic_draw_fail\n");
    pic_stop();
    g_movlist_rend_idx++;
    youtube_pic_play_start();
    return SID_NULL;
}

static BOOL StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_YOUTUBE_MOVLIST) ? TRUE : FALSE;
}

static STATEID StaYoutube_most_popu_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_vdolist_selected\n");
    g_reqUrlFrom = SID_YOUTUBE_MP_VDOLIST_NORMAL;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_client_cont, *p_movlist/*, *p_movlist_sbar*/;
    control_t *p_preview, *p_detail_cont, *p_detail/*, *p_detail_sbar*/;
    rect_t rect;

    OS_PRINTF("@@@StaYoutube_most_popu_on_vdolist_info_pressed\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
    MT_ASSERT(p_movlist != NULL);
  //  p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST_SBAR);
  //  MT_ASSERT(p_movlist_sbar != NULL);

    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_detail_cont = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_DETAIL_CONT);
    MT_ASSERT(p_detail_cont != NULL);
    p_detail = ctrl_get_child_by_id(p_detail_cont, IDC_YOUTUBE_INFO_DETAIL);
    MT_ASSERT(p_detail != NULL);
    //p_detail_sbar = ctrl_get_child_by_id(p_detail_cont, IDC_YOUTUBE_INFO_DETAIL_SBAR);
    //MT_ASSERT(p_detail_sbar != NULL);

    text_set_content_by_extstr(p_detail, g_curVdoPage.vdoList[g_vdolist_index].info_description);
    text_reset_param(p_detail);
    text_reset_param(p_detail);
    
    ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
//    ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
    ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);

    ctrl_set_attr(p_preview, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_preview, OBJ_STS_SHOW);
    ctrl_set_attr(p_detail_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_detail_cont, OBJ_STS_SHOW);
    //ctrl_set_attr(p_detail, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_detail, OBJ_STS_SHOW);
    //ctrl_set_attr(p_detail_sbar, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_detail_sbar, OBJ_STS_SHOW);
    ctrl_process_msg(p_detail, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(p_client_cont, TRUE);

    //clear pic layer
    pic_stop();

    ctrl_get_frame(p_movlist, &rect);
    ctrl_client2screen(p_movlist, &rect);
    rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
    rect.right = rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
    ui_pic_clear_rect(&rect, 0);

    return SID_NULL;
}

static void SexYoutube_MpVdolistNormal(void)
{
    OS_PRINTF("@@@SexYoutube_MpVdolistNormal\n");
    pic_stop();
}

static void SenYoutube_MpInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@SenYoutube_MpInfo\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
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

static STATEID StaYoutube_most_popu_on_info_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_client_cont, *p_preview, *p_detail_cont, *p_detail/*, *p_detail_sbar*/;
    rect_t rect;

    OS_PRINTF("@@@StaYoutube_most_popu_on_info_exit\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_detail_cont = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_DETAIL_CONT);
    MT_ASSERT(p_detail_cont != NULL);
    p_detail = ctrl_get_child_by_id(p_detail_cont, IDC_YOUTUBE_INFO_DETAIL);
    MT_ASSERT(p_detail != NULL);
    //p_detail_sbar = ctrl_get_child_by_id(p_detail_cont, IDC_YOUTUBE_INFO_DETAIL_SBAR);
    //MT_ASSERT(p_detail_sbar != NULL);

    ctrl_set_sts(p_preview, OBJ_STS_HIDE);
    ctrl_set_sts(p_detail_cont, OBJ_STS_HIDE);
    ctrl_set_sts(p_detail, OBJ_STS_HIDE);
    //ctrl_set_sts(p_detail_sbar, OBJ_STS_HIDE);
    ctrl_process_msg(p_detail, MSG_LOSTFOCUS, 0, 0);

    pic_stop();

    ctrl_get_frame(p_preview, &rect);
    ctrl_client2screen(p_preview, &rect);
    ui_pic_clear_rect(&rect, 0);

    youtube_update_curVdoPage();
    return SID_NULL;
}

static BOOL StcYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_YOUTUBE_INFO_DETAIL) ? TRUE : FALSE;
}

static STATEID StaYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_infotext_selected\n");
    g_reqUrlFrom = SID_YOUTUBE_MP_INFO;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static void SexYoutube_MpInfo(void)
{
    OS_PRINTF("@@@SexYoutube_MpInfo\n");
    pic_stop();
}

static void SenYoutube_MpRequest(void)
{
    OS_PRINTF("@@@SenYoutube_MpRequest\n");
}

static STATEID StaYoutube_most_popu_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_most_popu_on_newpageitem_arrive\n");

    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    if (g_curVdoPageNum > g_savVdoPageNum)
    {
        g_vdolist_index = 0;
    }
    else
    {
        g_vdolist_index = g_curVdoPage.count - 1;
    }
    g_savVdoPageNum = g_curVdoPageNum;

    youtube_update_curVdoPage();
    return SID_YOUTUBE_MP_VDOLIST_NORMAL;
}

static STATEID StaYoutube_most_popu_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_get_page_fail\n");

    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageNum = g_savVdoPageNum;
    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);

    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_MP_VDOLIST_NORMAL;
}

static void SexYoutube_MpRequest(void)
{
    OS_PRINTF("@@@SexYoutube_MpRequest\n");
}

static void SenYoutube_MpReqUrl(void)
{
    OS_PRINTF("@@@SenYoutube_MpReqUrl\n");
}

static STATEID StaYoutube_most_popu_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_play_url_t *p_data = (youtube_vdo_play_url_t *)para1;
    ui_video_player_param vdoPlayerParam;

    OS_PRINTF("@@@StaYoutube_most_popu_on_newplayurl_arrive\n");
    ui_comm_dlg_close();

    youtube_pic_deinit();

    memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));
    vdoPlayerParam.name = g_curVdoPage.vdoList[g_vdolist_index].title;
    vdoPlayerParam.url_cnt = 1;
    vdoPlayerParam.pp_urlList = (u8 **)mtos_malloc(sizeof(u8 *));
    MT_ASSERT(vdoPlayerParam.pp_urlList != NULL);
    vdoPlayerParam.pp_urlList[0] = p_data->url;

    manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_YOUTUBE, (u32)&vdoPlayerParam);

    if (vdoPlayerParam.pp_urlList != NULL)
    {
        mtos_free(vdoPlayerParam.pp_urlList);
    }
    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_get_url_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_get_url_fail\n");
    ui_comm_dlg_close();
    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);

    return SID_NULL;
}

static STATEID StaYoutube_most_popu_on_geturlfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    switch (g_reqUrlFrom)
    {
        case SID_YOUTUBE_MP_VDOLIST_NORMAL:
        case SID_YOUTUBE_CATE_VDOLIST_NORMAL:
        case SID_YOUTUBE_SEARCH_VDOLIST_NORMAL:
            youtube_update_curVdoPage();
            break;
        default:
            break;
    }

    return g_reqUrlFrom;
}

static STATEID StaYoutube_most_popu_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_pic_init();

    switch (g_reqUrlFrom)
    {
        case SID_YOUTUBE_MP_VDOLIST_NORMAL:
        case SID_YOUTUBE_CATE_VDOLIST_NORMAL:
        case SID_YOUTUBE_SEARCH_VDOLIST_NORMAL:
            youtube_update_curVdoPage();
            break;
        default:
            break;
    }

    return g_reqUrlFrom;
}

static void SexYoutube_MpReqUrl(void)
{
    OS_PRINTF("@@@SexYoutube_MpReqUrl\n");
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/* --------------------- Cate list: Category --------------------- */

static void SenYoutube_Category(void)
{
    OS_PRINTF("@@@SenYoutube_Category\n");
    ui_youtube_set_max_item_cnt(YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    ui_youtube_request_open(g_curFeedType);
}

static void SexYoutube_Category(void)
{
    OS_PRINTF("@@@SexYoutube_Category\n");
    ui_youtube_request_close();
}

static void SenYoutube_CateReqOpen(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL;

    OS_PRINTF("@@@SenYoutube_CateReqOpen\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    MT_ASSERT(p_client_cont != NULL);

    ctrl_paint_ctrl(p_client_cont, TRUE);
}

static STATEID StaYoutube_category_on_req_open_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_cate_page_t *p_data = (youtube_cate_page_t *)para1;

    OS_PRINTF("@@@StaYoutube_category_on_req_open_succ\n");

    if (g_cateList.cateList)
    {
        mtos_free(g_cateList.cateList);
        g_cateList.cateList = NULL;
    }

    g_cateList.count = p_data->count;
    if (p_data->count > 0 && p_data->cateList != NULL)
    {
        g_cateList.cateList = (youtube_cate_item_t *)mtos_malloc(p_data->count*sizeof(youtube_cate_item_t));
        MT_ASSERT(g_cateList.cateList != NULL);
        memcpy(g_cateList.cateList, p_data->cateList, p_data->count*sizeof(youtube_cate_item_t));
    }
    g_catelist_index = 0;
    g_totalCatePage = (p_data->count + YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE - 1) / YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE;
    g_curCatePageNum = 1;

    return SID_YOUTUBE_CATE_LIST;
}

static STATEID StaYoutube_category_on_req_open_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_req_open_fail\n");

    if (g_cateList.cateList)
    {
        mtos_free(g_cateList.cateList);
        g_cateList.cateList = NULL;
    }

    g_cateList.count = 0;
    g_catelist_index = 0;
    g_totalCatePage = 1;
    g_curCatePageNum = 1;

    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaYoutube_category_on_reqopenfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_CATE_LIST;
}

static void SexYoutube_CateReqOpen(void)
{
    OS_PRINTF("@@@SexYoutube_CateReqOpen\n");
}

static void SenYoutube_CateList(void)
{
    control_t *p_menu;
    control_t *p_page_cont, *p_ctrl;
    control_t *p_client_cont, *p_catelist;
    control_t *p_mainlist_cont = NULL, *p_mainlist = NULL;

    OS_PRINTF("@@@SenYoutube_CateList\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_CATELIST);
    MT_ASSERT(p_catelist != NULL);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);

    list_set_count(p_catelist, g_cateList.count, YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_catelist, g_catelist_index);

    ctrl_set_attr(p_catelist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_catelist, OBJ_STS_SHOW);

    if (!ctrl_is_onfocus(p_mainlist))
    {
        ctrl_process_msg(p_catelist, MSG_GETFOCUS, 0, 0);
    }

    youtube_update_page_num(p_ctrl, g_curCatePageNum, g_totalCatePage);
    youtube_update_catelist(p_catelist, list_get_valid_pos(p_catelist), YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE, 0);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);
}

static void SexYoutube_CateList(void)
{
    control_t *p_menu;
    control_t *p_client_cont, *p_catelist;

    OS_PRINTF("@@@SexYoutube_Catelist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_CATELIST);
        MT_ASSERT(p_catelist != NULL);
        
        ctrl_set_sts(p_catelist, OBJ_STS_HIDE);
        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

static void SenYoutube_CateListNormal(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_CateListNormal\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&comm_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static BOOL    StcYoutube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_YOUTUBE_CATELIST) ? TRUE : FALSE;
}

static STATEID StaYoutube_category_on_catelist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    u8 page_num;

    OS_PRINTF("@@@StaYoutube_category_on_catelist_updown\n");
    list_class_proc(ctrl, msg, para1, para2);

    g_catelist_index = (u8)list_get_focus_pos(ctrl);
    page_num = g_catelist_index / YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE + 1;
    if (page_num != g_curCatePageNum)
    {
        p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
        p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
        p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
        MT_ASSERT(p_ctrl != NULL);

        g_curCatePageNum = page_num;
        youtube_update_page_num(p_ctrl, g_curCatePageNum, g_totalCatePage);
        ctrl_paint_ctrl(p_ctrl, TRUE);
    }

    return SID_NULL;
}

static STATEID StaYoutube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_mainlist_cont, *p_mainlist;
    control_t *p_client_cont, *p_catelist;

    OS_PRINTF("@@@StaYoutube_category_on_change_focus\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_catelist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_CATELIST);
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

static STATEID StaYoutube_category_on_catelist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;

    if (ctrl_get_ctrl_id(ctrl) != IDC_YOUTUBE_CATELIST)
    {
        return SID_INTERNAL;
    }

    focus = list_get_focus_pos(ctrl);
    OS_PRINTF("@@@StaYoutube_category_on_catelist_selected focus=%d\n", focus);
    ui_youtube_set_category_index(focus);
    ui_youtube_set_page_index(0);
    ui_youtube_get_cur_page();
    return SID_YOUTUBE_CATE_REQ_VDO;
}

static void SexYoutube_CateListNormal(void)
{
    OS_PRINTF("@@@SexYoutube_CateListNormal\n");
}

static void SenYoutube_CateReqVdo(void)
{
    OS_PRINTF("@@@SenYoutube_CateReqVdo\n");
}

static STATEID StaYoutube_category_on_firstpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_category_on_firstpageitem_arrive\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    g_totalVdoPage = (p_data->total_count + YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE - 1) / YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE;
    g_savVdoPageNum = g_curVdoPageNum = 1;
    g_vdolist_index = 0;

    return SID_YOUTUBE_CATE_VDOLIST;
}

static STATEID StaYoutube_category_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_get_page_fail\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = 0;
    g_totalVdoPage = 1;
    g_savVdoPageNum = g_curVdoPageNum = 1;
    g_vdolist_index = 0;

    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaYoutube_category_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_CATE_LIST_NORMAL;
}

static void SexYoutube_CateReqVdo(void)
{
    OS_PRINTF("@@@SexYoutube_CateReqVdo\n");
}

static void SenYoutube_CateVdolist(void)
{
    OS_PRINTF("@@@SenYoutube_CateVdolist\n");
    youtube_update_curVdoPage();
}

static void SexYoutube_CateVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    rect_t rect;

    OS_PRINTF("@@@SexYoutube_CateVdolist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);
        
        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);
        //ctrl_paint_ctrl(p_client_cont, TRUE);

        //clear pic layer
        pic_stop();

        ctrl_get_frame(p_movlist, &rect);
        ctrl_client2screen(p_movlist, &rect);
        rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
        rect.right = rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
        ui_pic_clear_rect(&rect, 0);
    }
}

static void SenYoutube_CateVdolistNormal(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_CateVdolistNormal\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);

    g_movlist_rend_idx = 0;
    youtube_pic_play_start();
}

static STATEID StaYoutube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_vdolist_updown g_vdolist_index=%d\n", g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageNum > 1)
            {
                ui_youtube_get_prev_page();
                g_curVdoPageNum--;
                return SID_YOUTUBE_CATE_VDO_REQ;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageNum < g_totalVdoPage)
            {
                ui_youtube_get_next_page();
                g_curVdoPageNum++;
                return SID_YOUTUBE_CATE_VDO_REQ;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaYoutube_category_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_vdolist_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageNum > 1)
        {
            ui_youtube_get_prev_page();
            g_curVdoPageNum--;
            return SID_YOUTUBE_CATE_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageNum < g_totalVdoPage)
        {
            ui_youtube_get_next_page();
            g_curVdoPageNum++;
            return SID_YOUTUBE_CATE_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaYoutube_category_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_vdolist_selected\n");
    g_reqUrlFrom = SID_YOUTUBE_CATE_VDOLIST_NORMAL;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static void SexYoutube_CateVdolistNormal(void)
{
    OS_PRINTF("@@@SexYoutube_CateVdolistNormal\n");
    pic_stop();
}

static void SenYoutube_CateVdoInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@SenYoutube_CateVdoInfo\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
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

static STATEID StaYoutube_category_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_infotext_selected\n");
    g_reqUrlFrom = SID_YOUTUBE_CATE_VDO_INFO;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static void SexYoutube_CateVdoInfo(void)
{
    OS_PRINTF("@@@SexYoutube_CateVdoInfo\n");
    pic_stop();
}

static void SenYoutube_CateVdoReq(void)
{
    OS_PRINTF("@@@SenYoutube_CateVdoReq\n");
}

static STATEID StaYoutube_category_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_category_on_newpageitem_arrive\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    if (g_curVdoPageNum > g_savVdoPageNum)
    {
        g_vdolist_index = 0;
    }
    else
    {
        g_vdolist_index = g_curVdoPage.count - 1;
    }
    g_savVdoPageNum = g_curVdoPageNum;

    youtube_update_curVdoPage();
    return SID_YOUTUBE_CATE_VDOLIST_NORMAL;
}

static STATEID StaYoutube_category_on_vdolist_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_category_on_vdolist_get_page_fail\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageNum = g_savVdoPageNum;
    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaYoutube_category_on_vdolist_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_CATE_VDOLIST_NORMAL;
}

static void SexYoutube_CateVdoReq(void)
{
    OS_PRINTF("@@@SexYoutube_CateVdoReq\n");
}

static void SenYoutube_CateVdoReqUrl(void)
{
    OS_PRINTF("@@@SenYoutube_CateVdoReqUrl\n");
}

static void SexYoutube_CateVdoReqUrl(void)
{
    OS_PRINTF("@@@SexYoutube_CateVdoReqUrl\n");
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/* --------------------- Video list: Search --------------------- */

static void SenYoutube_Search(void)
{
    OS_PRINTF("@@@SenYoutube_Search\n");
}

static void SexYoutube_Search(void)
{
    OS_PRINTF("@@@SexYoutube_Search\n");
    if (g_curFeedType == SEARCH_FEED_TYPE)
    {
        ui_youtube_request_close();
    }
}

#define MAX_SEARCH_NAME_LEN     (16)
static u16 g_searchKey[MAX_SEARCH_NAME_LEN+1] = {0};
//static void ui_youtube_edit_cb(void)
RET_CODE ui_youtube_edit_cb(u16* p_unistr)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);

    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_INPUT_COMP, 0, 0);
    }
	return SUCCESS;
}

static void SenYoutube_Input(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL;
    kb_param_t param;

    OS_PRINTF("@@@SenYoutube_Input\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    MT_ASSERT(p_client_cont != NULL);
    ctrl_paint_ctrl(p_client_cont, TRUE);

    param.uni_str = g_searchKey;
    param.type = KB_INPUT_TYPE_SENTENCE;
    param.max_len = MAX_SEARCH_NAME_LEN;
    param.cb = ui_youtube_edit_cb;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
}

static void SexYoutube_Input(void)
{
    OS_PRINTF("@@@SexYoutube_Input\n");
    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }
}

static void SenYoutube_Searching(void)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u8 key[MAX_SEARCH_NAME_LEN*3+1] = {0};

    OS_PRINTF("@@@SenYoutube_Searching\n");
    inbuf = (char *)g_searchKey;
    outbuf = (char *)key;
    src_len = sizeof(g_searchKey);
    dest_len = sizeof(key);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    ui_youtube_set_max_item_cnt(YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    ui_youtube_search(key);
    g_curFeedType = SEARCH_FEED_TYPE;
}

static STATEID StaYoutube_search_on_search_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_search_on_search_succ\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    g_totalVdoPage = (p_data->total_count + YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE - 1) / YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE;
    g_savVdoPageNum = g_curVdoPageNum = 1;
    g_vdolist_index = 0;

    return SID_YOUTUBE_SEARCH_VDOLIST;
}

static STATEID StaYoutube_search_on_search_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_search_fail\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    if (g_curVdoPage.vdoList)
    {
        mtos_free(g_curVdoPage.vdoList);
        g_curVdoPage.vdoList = NULL;
    }

    g_curVdoPage.count = 0;
    g_totalVdoPage = 1;
    g_savVdoPageNum = g_curVdoPageNum = 1;
    g_vdolist_index = 0;

    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_NO_DATA);
    return SID_NULL;
}

static STATEID StaYoutube_search_on_searchfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_YOUTUBE_SEARCH_VDOLIST;
}

static void SexYoutube_Searching(void)
{
    OS_PRINTF("@@@SexYoutube_Searching\n");
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenYoutube_SearchVdolist(void)
{
    OS_PRINTF("@@@SenYoutube_SearchVdolist\n");
    youtube_update_curVdoPage();
}

static void SexYoutube_SearchVdolist(void)
{
    control_t *p_menu = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    rect_t rect;

    OS_PRINTF("@@@SexYoutube_MpVdolist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_movlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST);
        MT_ASSERT(p_movlist != NULL);
        //p_movlist_sbar = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_MOVLIST_SBAR);
        //MT_ASSERT(p_movlist_sbar != NULL);

        ctrl_set_sts(p_movlist, OBJ_STS_HIDE);
        //ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);
        //ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);
        //ctrl_paint_ctrl(p_client_cont, TRUE);

        //clear pic layer
        pic_stop();

        ctrl_get_frame(p_movlist, &rect);
        ctrl_client2screen(p_movlist, &rect);
        rect.left += movlist_attr[VDOLIST_FIELD_ID_LOGO].left;
        rect.right = rect.left + movlist_attr[VDOLIST_FIELD_ID_LOGO].width;
        ui_pic_clear_rect(&rect, 0);
    }
}

static void SenYoutube_SearchVdolistNormal(void)
{
    control_t *p_menu = NULL, *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_SearchVdolistNormal\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    ui_comm_help_set_data(&vdo_help_data, p_bottom_help);
    ctrl_paint_ctrl(p_bottom_help, TRUE);

    g_movlist_rend_idx = 0;
    youtube_pic_play_start();
}

static STATEID StaYoutube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_most_popu_on_updown g_vdolist_index=%d\n", g_vdolist_index);

    if (msg == MSG_FOCUS_UP)
    {
        if (g_vdolist_index == 0)
        {
            if (g_curVdoPageNum > 1)
            {
                ui_youtube_get_prev_page();
                g_curVdoPageNum--;
                return SID_YOUTUBE_SEARCH_REQUEST;
            }
            return SID_NULL;
        }
    }
    else if (msg == MSG_FOCUS_DOWN)
    {
        if (g_vdolist_index + 1 >= g_curVdoPage.count)
        {
            if (g_curVdoPageNum < g_totalVdoPage)
            {
                ui_youtube_get_next_page();
                g_curVdoPageNum++;
                return SID_YOUTUBE_SEARCH_REQUEST;
            }
            return SID_NULL;
        }
    }

    list_class_proc(ctrl, msg, para1, para2);
    g_vdolist_index = (u8)list_get_focus_pos(ctrl);
    return SID_NULL;
}

static STATEID StaYoutube_search_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_vdolist_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_curVdoPageNum > 1)
        {
            ui_youtube_get_prev_page();
            g_curVdoPageNum--;
            return SID_YOUTUBE_SEARCH_REQUEST;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_curVdoPageNum < g_totalVdoPage)
        {
            ui_youtube_get_next_page();
            g_curVdoPageNum++;
            return SID_YOUTUBE_SEARCH_REQUEST;
        }
    }

    return SID_NULL;
}

static STATEID StaYoutube_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_vdolist_selected g_vdolist_index=%d\n", g_vdolist_index);
    g_reqUrlFrom = SID_YOUTUBE_SEARCH_VDOLIST_NORMAL;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static void SexYoutube_SearchVdolistNormal(void)
{
    OS_PRINTF("@@@SexYoutube_SearchVdolistNormal\n");
    pic_stop();
}

static void SenYoutube_SearchInfo(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_preview = NULL, *p_bottom_help = NULL;
    rect_t rect;

    OS_PRINTF("@@@SenYoutube_SearchInfo\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_preview = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_INFO_PIC);
    MT_ASSERT(p_preview != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
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

static STATEID StaYoutube_search_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_infotext_selected\n");
    g_reqUrlFrom = SID_YOUTUBE_SEARCH_INFO;
    youtube_open_dlg(IDS_MSG_GET_URL);

    ui_youtube_set_item_index(g_vdolist_index);
    ui_youtube_get_video_play_url();

    return SID_NULL;
}

static void SexYoutube_SearchInfo(void)
{
    OS_PRINTF("@@@SexYoutube_SearchInfo\n");
    pic_stop();
}

static void SenYoutube_SearchRequest(void)
{
    OS_PRINTF("@@@SenYoutube_SearchRequest\n");
}

static STATEID StaYoutube_search_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_vdo_req_t *p_data = (youtube_vdo_req_t *)para1;

    OS_PRINTF("@@@StaYoutube_search_on_newpageitem_arrive\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
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
        g_curVdoPage.vdoList = (youtube_vdo_item_t *)mtos_malloc(p_data->count*sizeof(youtube_vdo_item_t));
        MT_ASSERT(g_curVdoPage.vdoList != NULL);
        memcpy(g_curVdoPage.vdoList, p_data->vdoList, p_data->count*sizeof(youtube_vdo_item_t));
    }
    if (g_curVdoPageNum > g_savVdoPageNum)
    {
        g_vdolist_index = 0;
    }
    else
    {
        g_vdolist_index = g_curVdoPage.count - 1;
    }
    g_savVdoPageNum = g_curVdoPageNum;

    youtube_update_curVdoPage();
    return SID_YOUTUBE_SEARCH_VDOLIST_NORMAL;
}

static STATEID StaYoutube_search_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_get_page_fail\n");
    if (fw_get_focus_id() != ROOT_ID_YOUTUBE)
    {
        return SID_NULL;
    }

    g_curVdoPageNum = g_savVdoPageNum;
    //ui_comm_cfmdlg_open(NULL, IDS_DATA_ERROR, NULL, 0);
    youtube_open_cfm_dlg(IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaYoutube_search_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaYoutube_search_on_getpagefail_dlg_closed\n");
    return SID_YOUTUBE_SEARCH_VDOLIST_NORMAL;
}

static void SexYoutube_SearchRequest(void)
{
    OS_PRINTF("@@@SexYoutube_SearchRequest\n");
}

static void SenYoutube_SearchReqUrl(void)
{
    OS_PRINTF("@@@SenYoutube_SearchReqUrl\n");
}

static void SexYoutube_SearchReqUrl(void)
{
    OS_PRINTF("@@@SexYoutube_SearchReqUrl\n");
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

/* --------------------- Option list: Country Resolution & TimeMode --------------------- */

static void SenYoutube_Country(void)
{
    OS_PRINTF("@@@SenYoutube_Country\n");
}

static void SexYoutube_Country(void)
{
    OS_PRINTF("@@@SexYoutube_Country\n");
}

static void SenYoutube_CountryOptlist(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_optlist = NULL;
    control_t *p_bottom_help = NULL;
    u16 total_count;

    OS_PRINTF("@@@SenYoutube_CountryOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
    MT_ASSERT(p_optlist != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    total_count = ARRAY_SIZE(g_country_opt);
    list_set_count(p_optlist, total_count, YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_optlist, 0);

    ctrl_set_attr(p_optlist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_optlist, OBJ_STS_SHOW);

    g_totalOptPage = (total_count + YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE - 1) / YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE;
    g_curOptPageNum = 1;
    youtube_update_page_num(p_ctrl, g_curOptPageNum, g_totalOptPage);
    youtube_update_optlist(p_optlist, list_get_valid_pos(p_optlist), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE, 0);

    ui_comm_help_set_data(&comm_help_data, p_bottom_help);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_YOUTUBE_OPTLIST) ? TRUE : FALSE;
}

static STATEID StaYoutube_country_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    u16 focus;
    u8 page_num;

    OS_PRINTF("@@@StaYoutube_country_on_vdolist_updown\n");
    list_class_proc(ctrl, msg, para1, para2);

    focus = (u8)list_get_focus_pos(ctrl);
    page_num = focus / YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE + 1;
    if (page_num != g_curOptPageNum)
    {
        p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
        p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
        p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
        MT_ASSERT(p_ctrl != NULL);

        g_curOptPageNum = page_num;
        youtube_update_page_num(p_ctrl, g_curOptPageNum, g_totalOptPage);
        ctrl_paint_ctrl(p_ctrl, TRUE);
    }

    return SID_NULL;
}

static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_mainlist_cont, *p_mainlist;
    control_t *p_client_cont, *p_optlist;

    OS_PRINTF("@@@StaYoutube_country_on_change_focus\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_mainlist_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_LIST_CONT);
    p_mainlist = ctrl_get_child_by_id(p_mainlist_cont, IDC_YOUTUBE_LIST);
    MT_ASSERT(p_mainlist != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
    MT_ASSERT(p_optlist != NULL);

    if (msg == MSG_FOCUS_RIGHT)
    {
        ctrl_process_msg(p_mainlist, MSG_LOSTFOCUS, 0, 0);

        ctrl_set_attr(p_optlist, OBJ_ATTR_ACTIVE);
        ctrl_set_sts(p_optlist, OBJ_STS_SHOW);
        ctrl_process_msg(p_optlist, MSG_GETFOCUS, 0, 0);

        ctrl_paint_ctrl(p_mainlist_cont, TRUE);
        ctrl_paint_ctrl(p_client_cont, TRUE);
    }
    else if (msg == MSG_BACK || msg == MSG_MAIN)
    {
        ctrl_process_msg(p_optlist, MSG_LOSTFOCUS, 0, 0);

        ctrl_set_attr(p_mainlist, OBJ_ATTR_ACTIVE);
        ctrl_set_sts(p_mainlist, OBJ_STS_SHOW);
        ctrl_process_msg(p_mainlist, MSG_GETFOCUS, 0, 0);

        ctrl_paint_ctrl(p_mainlist_cont, TRUE);
        ctrl_paint_ctrl(p_client_cont, TRUE);
    }

    return SID_NULL;
}

static STATEID StaYoutube_country_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_config_t config;
    u16 i, cnt, focus;

    cnt = list_get_count(ctrl);
    sys_status_get_youtube_config_info(&config);

    for (i = 0; i < cnt; i++)
    {
        if (g_country_opt[i].id == config.region_id)
        {
            break;
        }
    }
    if (i < cnt)
    {
        list_set_field_content_by_icon(ctrl, i, 1, RSC_INVALID_ID);
        //list_draw_field_ext(ctrl, i, 1, TRUE);
        list_draw_item_ext(ctrl, i, TRUE);
    }

    focus = list_get_focus_pos(ctrl);
    if (focus < cnt)
    {
        list_set_field_content_by_icon(ctrl, focus, 1, IM_TV_SELECT);
        //list_draw_field_ext(ctrl, focus, 1, TRUE);
        list_draw_item_ext(ctrl, focus, TRUE);

        config.region_id = g_country_opt[focus].id;
        sys_status_set_youtube_config_info(&config);
        ui_youtube_set_country((YT_REGION_ID_T) config.region_id);
    }
    return SID_NULL;
}

static void SexYoutube_CountryOptlist(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_optlist = NULL;

    OS_PRINTF("@@@SexYoutube_CountryOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
        MT_ASSERT(p_optlist != NULL);

        ctrl_set_sts(p_optlist, OBJ_STS_HIDE);
        //ctrl_paint_ctrl(p_client_cont, TRUE);
    }
}

static void SenYoutube_Resolution(void)
{
    OS_PRINTF("@@@SenYoutube_Resolution\n");
}

static void SexYoutube_Resolution(void)
{
    OS_PRINTF("@@@SexYoutube_Resolution\n");
}

static void SenYoutube_ResolutionOptlist(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_optlist = NULL;
    control_t *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_ResolutionOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
    MT_ASSERT(p_optlist != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    list_set_count(p_optlist, ARRAY_SIZE(g_resolution_opt), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_optlist, 0);

    ctrl_set_attr(p_optlist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_optlist, OBJ_STS_SHOW);

    youtube_update_page_num(p_ctrl, 1, 1);
    youtube_update_optlist(p_optlist, list_get_valid_pos(p_optlist), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE, 0);

    ui_comm_help_set_data(&comm_help_data, p_bottom_help);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static STATEID StaYoutube_resolution_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_config_t config;
    u16 i, cnt, focus;

    cnt = list_get_count(ctrl);
    sys_status_get_youtube_config_info(&config);

    for (i = 0; i < cnt; i++)
    {
        if (g_resolution_opt[i].id == config.resolution)
        {
            break;
        }
    }
    if (i < cnt)
    {
        list_set_field_content_by_icon(ctrl, i, 1, RSC_INVALID_ID);
        //list_draw_field_ext(ctrl, i, 1, TRUE);
        list_draw_item_ext(ctrl, i, TRUE);
    }

    focus = list_get_focus_pos(ctrl);
    if (focus < cnt)
    {
        list_set_field_content_by_icon(ctrl, focus, 1, IM_TV_SELECT);
        //list_draw_field_ext(ctrl, focus, 1, TRUE);
        list_draw_item_ext(ctrl, focus, TRUE);

        config.resolution = (u8)g_resolution_opt[focus].id;
        sys_status_set_youtube_config_info(&config);
    }
    return SID_NULL;
}

static void SexYoutube_ResolutionOptlist(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_optlist = NULL;

    OS_PRINTF("@@@SexYoutube_ResolutionOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
        MT_ASSERT(p_optlist != NULL);

        ctrl_set_sts(p_optlist, OBJ_STS_HIDE);
        //ctrl_paint_ctrl(p_optlist, TRUE);
    }
}

static void SenYoutube_TimeMode(void)
{
    OS_PRINTF("@@@SenYoutube_TimeMode\n");
}

static void SexYoutube_TimeMode(void)
{
    OS_PRINTF("@@@SexYoutube_TimeMode\n");
}

static void SenYoutube_TimeModeOptlist(void)
{
    control_t *p_menu = NULL;
    control_t *p_page_cont = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_optlist = NULL;
    control_t *p_bottom_help = NULL;

    OS_PRINTF("@@@SenYoutube_TimeModeOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    p_page_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_PAGE_CONT);
    p_ctrl = ctrl_get_child_by_id(p_page_cont, IDC_YOUTUBE_PAGE_NUM);
    MT_ASSERT(p_ctrl != NULL);
    p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
    p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
    MT_ASSERT(p_optlist != NULL);
    p_bottom_help = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_BTM_HELP);
    MT_ASSERT(p_bottom_help != NULL);

    list_set_count(p_optlist, ARRAY_SIZE(g_timemode_opt), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_optlist, 0);

    ctrl_set_attr(p_optlist, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_optlist, OBJ_STS_SHOW);

    youtube_update_page_num(p_ctrl, 1, 1);
    youtube_update_optlist(p_optlist, list_get_valid_pos(p_optlist), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE, 0);

    ui_comm_help_set_data(&comm_help_data, p_bottom_help);

    ctrl_paint_ctrl(p_ctrl, TRUE);
    ctrl_paint_ctrl(p_client_cont, TRUE);
    ctrl_paint_ctrl(p_bottom_help, TRUE);
}

static STATEID StaYoutube_time_mode_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    youtube_config_t config;
    u16 i, cnt, focus;

    cnt = list_get_count(ctrl);
    sys_status_get_youtube_config_info(&config);

    for (i = 0; i < cnt; i++)
    {
        if (g_timemode_opt[i].id == config.time_mode)
        {
            break;
        }
    }
    if (i < cnt)
    {
        list_set_field_content_by_icon(ctrl, i, 1, RSC_INVALID_ID);
        //list_draw_field_ext(ctrl, i, 1, TRUE);
        list_draw_item_ext(ctrl, i, TRUE);
    }

    focus = list_get_focus_pos(ctrl);
    if (focus < cnt)
    {
        list_set_field_content_by_icon(ctrl, focus, 1, IM_TV_SELECT);
        //list_draw_field_ext(ctrl, focus, 1, TRUE);
        list_draw_item_ext(ctrl, focus, TRUE);

        config.time_mode = (u8)g_timemode_opt[focus].id;
        sys_status_set_youtube_config_info(&config);
        ui_youtube_set_time_mode((YT_TIME_T) config.time_mode);
    }
    return SID_NULL;
}

static void SexYoutube_TimeModeOptlist(void)
{
    control_t *p_menu = NULL, *p_client_cont = NULL, *p_optlist = NULL;

    OS_PRINTF("@@@SexYoutube_TimeModeOptlist\n");
    p_menu = ui_comm_root_get_root(ROOT_ID_YOUTUBE);
    if (p_menu != NULL)
    {
        p_client_cont = ctrl_get_child_by_id(p_menu, IDC_YOUTUBE_CLIENT_CONT);
        p_optlist = ctrl_get_child_by_id(p_client_cont, IDC_YOUTUBE_OPTLIST);
        MT_ASSERT(p_optlist != NULL);

        ctrl_set_sts(p_optlist, OBJ_STS_HIDE);
        //ctrl_paint_ctrl(p_optlist, TRUE);
    }
}

static RET_CODE youtube_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = Youtube_DispatchMsg(p_ctrl, msg, para1, para2);

    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@unaccepted msgid=0x%04x\n", msg);
    }

    ui_youtube_free_msg_data(msg, para1, para2);

    return ret;
}

/*!
 * Video view entry
 */
RET_CODE ui_open_youtube(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_bmp = NULL;
    control_t *p_list_cont = NULL, *p_list = NULL;
    control_t *p_page_cont = NULL, *p_icon = NULL, *p_ctrl = NULL;
    control_t *p_client_cont = NULL, *p_movlist = NULL/*, *p_movlist_sbar = NULL*/;
    control_t *p_catelist = NULL/*, *p_catelist_sbar = NULL*/;
    control_t *p_optlist = NULL/*, *p_optlist_sbar = NULL*/;
    control_t *p_preview = NULL, *p_detail_cont = NULL, *p_detail = NULL, *p_detail_sbar = NULL;
    control_t *p_bottom_help = NULL;
    u8 i = 0;

    OS_PRINTF("@@@ui_open_youtube\n");

    g_mainlist_index = 0;
    g_vdolist_index = 0;
    g_totalVdoPage = 1;
    g_curVdoPageNum = g_savVdoPageNum = 1;
    g_catelist_index = 0;
    g_totalCatePage = 1;
    g_curCatePageNum = 1;
    g_totalOptPage = 1;
    g_curOptPageNum = 1;

    g_curFeedType = STAND_FEED_TOP_POPULAR;
    g_reqUrlFrom = SID_NULL;

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_YOUTUBE, 0,
                                    IM_INDEX_YOUTUBE_BANNER, IDS_YOUTUBE);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, youtube_cont_keymap);
    ctrl_set_proc(p_menu, youtube_cont_proc);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_BTM_HELP,
                                      YOUTUBE_BOTTOM_HELP_X, YOUTUBE_BOTTOM_HELP_Y,
                                      YOUTUBE_BOTTOM_HELP_W, YOUTUBE_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    //text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    //text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
    //text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(60, 0, 
                                YOUTUBE_BOTTOM_HELP_W-60, YOUTUBE_BOTTOM_HELP_H,  
                                &emp_help_data,  p_bottom_help);

    /*!
     * Create list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_LIST_CONT,
                                  YOUTUBE_LIST_CONTX, YOUTUBE_LIST_CONTY,
                                  YOUTUBE_LIST_CONTW, YOUTUBE_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_YOUTUBE_LEFT_BMP, 
                                        YOUTUBE_BMP_X, YOUTUBE_BMP_Y, 
                                        YOUTUBE_BMP_W, YOUTUBE_BMP_H, p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, IM_PIC_YOUTUBE_LOGO);
    
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_YOUTUBE_LIST,
                          YOUTUBE_LIST_X, YOUTUBE_LIST_Y,
                          YOUTUBE_LISTW, YOUTUBE_LISTH,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, youtube_mainlist_keymap);
    ctrl_set_proc(p_list, youtube_mainlist_proc);

    ctrl_set_mrect(p_list, YOUTUBE_LIST_MIDL, YOUTUBE_LIST_MIDT,
                  YOUTUBE_LIST_MIDW + YOUTUBE_LIST_MIDL, YOUTUBE_LIST_MIDH + YOUTUBE_LIST_MIDT);
    list_set_item_interval(p_list, YOUTUBE_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &mainlist_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, YOUTUBE_LIST_ITEM_COUNT, YOUTUBE_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(mainlist_attr), YOUTUBE_LIST_ITEM_NUM_ONE_PAGE);
    //list_set_focus_pos(p_list, g_mainlist_index);
    //list_select_item(p_list, g_mainlist_index);
    list_set_update(p_list, youtube_update_mainlist, 0);
    //ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

    for (i = 0; i < ARRAY_SIZE(mainlist_attr); i++)
    {
        list_set_field_attr(p_list, i, mainlist_attr[i].attr,
                            mainlist_attr[i].width, mainlist_attr[i].left, mainlist_attr[i].top);
        list_set_field_rect_style(p_list, i, mainlist_attr[i].rstyle);
        list_set_field_font_style(p_list, i, mainlist_attr[i].fstyle);
    }

    youtube_update_mainlist(p_list, list_get_valid_pos(p_list), YOUTUBE_LIST_ITEM_NUM_ONE_PAGE, 0);

    //Page
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_PAGE_CONT,
                                    YOUTUBE_PAGE_CONTX, YOUTUBE_PAGE_CONTY,
                                    YOUTUBE_PAGE_CONTW, YOUTUBE_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               YOUTUBE_PAGE_ARROWL_X, YOUTUBE_PAGE_ARROWL_Y,
                               YOUTUBE_PAGE_ARROWL_W, YOUTUBE_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               YOUTUBE_PAGE_ARROWR_X, YOUTUBE_PAGE_ARROWR_Y,
                               YOUTUBE_PAGE_ARROWR_W, YOUTUBE_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 YOUTUBE_PAGE_NAME_X, YOUTUBE_PAGE_NAME_Y,
                                 YOUTUBE_PAGE_NAME_W, YOUTUBE_PAGE_NAME_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_by_strid(p_ctrl, IDS_PAGE1);

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_YOUTUBE_PAGE_NUM,
                                 YOUTUBE_PAGE_VALUE_X, YOUTUBE_PAGE_VALUE_Y,
                                 YOUTUBE_PAGE_VALUE_W, YOUTUBE_PAGE_VALUE_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    //youtube_update_page_num(p_ctrl);

    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_CLIENT_CONT,
                                  YOUTUBE_CLIENT_CONTX, YOUTUBE_CLIENT_CONTY,
                                  YOUTUBE_CLIENT_CONTW, YOUTUBE_CLIENT_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //Movie list
    p_movlist = ctrl_create_ctrl(CTRL_LIST, IDC_YOUTUBE_MOVLIST, 
                                        YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                                        YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_movlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_movlist, youtube_movlist_keymap);
    ctrl_set_proc(p_movlist, youtube_movlist_proc);

    ctrl_set_mrect(p_movlist, 
                    YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                    YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H);
    list_set_item_interval(p_movlist, YOUTUBE_MOVLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_movlist, &movlist_item_rstyle);

    list_set_count(p_movlist, 0, YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_movlist, ARRAY_SIZE(movlist_attr), YOUTUBE_MOVLIST_ITEM_NUM_ONE_PAGE);
    //list_set_focus_pos(p_movlist, 0);
    list_set_update(p_movlist, youtube_update_vdolist, 0);

    for(i = 0; i < ARRAY_SIZE(movlist_attr); i++)
    {
        list_set_field_attr2(p_movlist, (u8)i, movlist_attr[i].attr,
                            movlist_attr[i].width, movlist_attr[i].height, movlist_attr[i].left, movlist_attr[i].top);
        list_set_field_rect_style(p_movlist, (u8)i, movlist_attr[i].rstyle);
        list_set_field_font_style(p_movlist, (u8)i, movlist_attr[i].fstyle);
    }
    ctrl_set_sts(p_movlist, OBJ_STS_HIDE);

    //create scroll bar
  /*  p_movlist_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_YOUTUBE_MOVLIST_SBAR,
                            YOUTUBE_MOVLIST_SBAR_X, YOUTUBE_MOVLIST_SBAR_Y,
                            YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H,
                            p_client_cont, 0);
    ctrl_set_rstyle(p_movlist_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_movlist_sbar, 1);
    sbar_set_direction(p_movlist_sbar, 0);
    sbar_set_mid_rstyle(p_movlist_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_movlist_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H - COMM_SBAR_VERTEX_GAP);
    list_set_scrollbar(p_movlist, p_movlist_sbar);
    ctrl_set_sts(p_movlist_sbar, OBJ_STS_HIDE);*/

    //Category list
    p_catelist = ctrl_create_ctrl(CTRL_LIST, IDC_YOUTUBE_CATELIST, 
                                        YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                                        YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_catelist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_catelist, youtube_catelist_keymap);
    ctrl_set_proc(p_catelist, youtube_catelist_proc);

    ctrl_set_mrect(p_catelist, 
                    YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                    YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H);
    list_set_item_interval(p_catelist, YOUTUBE_MOVLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_catelist, &movlist_item_rstyle);

    list_set_count(p_catelist, 0, YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_catelist, ARRAY_SIZE(catelist_attr), YOUTUBE_CATELIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_catelist, youtube_update_catelist, 0);
    list_enable_page_mode(p_catelist, TRUE);

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
    p_catelist_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_YOUTUBE_CATELIST_SBAR,
                            YOUTUBE_MOVLIST_SBAR_X, YOUTUBE_MOVLIST_SBAR_Y,
                            YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H,
                            p_client_cont, 0);
    ctrl_set_rstyle(p_catelist_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_catelist_sbar, 1);
    sbar_set_direction(p_catelist_sbar, 0);
    sbar_set_mid_rstyle(p_catelist_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_catelist_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H - COMM_SBAR_VERTEX_GAP);
    list_set_scrollbar(p_catelist, p_catelist_sbar);
    ctrl_set_sts(p_catelist_sbar, OBJ_STS_HIDE);
    #endif

    //Option list
    p_optlist = ctrl_create_ctrl(CTRL_LIST, IDC_YOUTUBE_OPTLIST, 
                                        YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                                        YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_optlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_optlist, youtube_optlist_keymap);
    ctrl_set_proc(p_optlist, youtube_optlist_proc);

    ctrl_set_mrect(p_optlist, 
                    YOUTUBE_MOVLIST_X, YOUTUBE_MOVLIST_Y,
                    YOUTUBE_MOVLIST_W, YOUTUBE_MOVLIST_H);
    list_set_item_interval(p_optlist, YOUTUBE_MOVLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_optlist, &movlist_item_rstyle);

    list_set_count(p_optlist, 0, YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_optlist, ARRAY_SIZE(optlist_attr), YOUTUBE_OPTLIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_optlist, youtube_update_optlist, 0);
    list_enable_page_mode(p_optlist, TRUE);

    for(i = 0; i < ARRAY_SIZE(optlist_attr); i++)
    {
        list_set_field_attr(p_optlist, (u8)i, optlist_attr[i].attr,
                            optlist_attr[i].width, optlist_attr[i].left, optlist_attr[i].top);
        list_set_field_rect_style(p_optlist, (u8)i, optlist_attr[i].rstyle);
        list_set_field_font_style(p_optlist, (u8)i, optlist_attr[i].fstyle);
    }
    ctrl_set_sts(p_optlist, OBJ_STS_HIDE);

    //create scroll bar
    #if 0
    p_optlist_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_YOUTUBE_OPTLIST_SBAR,
                            YOUTUBE_MOVLIST_SBAR_X, YOUTUBE_MOVLIST_SBAR_Y,
                            YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H,
                            p_client_cont, 0);
    ctrl_set_rstyle(p_optlist_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_optlist_sbar, 1);
    sbar_set_direction(p_optlist_sbar, 0);
    sbar_set_mid_rstyle(p_optlist_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_optlist_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    YOUTUBE_MOVLIST_SBAR_W, YOUTUBE_MOVLIST_SBAR_H - COMM_SBAR_VERTEX_GAP);
    list_set_scrollbar(p_optlist, p_optlist_sbar);
    ctrl_set_sts(p_optlist_sbar, OBJ_STS_HIDE);
    #endif

    //info
    p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_INFO_PIC, 
                                YOUTUBE_INFO_PIC_X, YOUTUBE_INFO_PIC_Y, 
                                YOUTUBE_INFO_PIC_W, YOUTUBE_INFO_PIC_H, 
                                p_client_cont, 0);
    ctrl_set_rstyle(p_preview, RSI_TV, RSI_TV, RSI_TV);
    ctrl_set_sts(p_preview, OBJ_STS_HIDE);

    p_detail_cont = ctrl_create_ctrl(CTRL_CONT, IDC_YOUTUBE_INFO_DETAIL_CONT,
                                      YOUTUBE_INFO_DETAIL_CONTX, YOUTUBE_INFO_DETAIL_CONTY, 
                                      YOUTUBE_INFO_DETAIL_CONTW, YOUTUBE_INFO_DETAIL_CONTH, 
                                      p_client_cont, 0);
    ctrl_set_rstyle(p_detail_cont, RSI_BOX_1, RSI_BOX_1, RSI_BOX_1);

    p_detail = ctrl_create_ctrl(CTRL_TEXT, IDC_YOUTUBE_INFO_DETAIL,
                                YOUTUBE_INFO_DETAIL_X, YOUTUBE_INFO_DETAIL_Y, 
                                YOUTUBE_INFO_DETAIL_W, YOUTUBE_INFO_DETAIL_H,
                                p_detail_cont, 0);
    ctrl_set_keymap(p_detail, youtube_info_detail_keymap);
    ctrl_set_proc(p_detail, youtube_info_detail_proc);
    ctrl_set_rstyle(p_detail, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_detail, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_detail, TEXT_STRTYPE_EXTSTR);
    text_set_align_type(p_detail, STL_LEFT | STL_TOP);
    text_set_offset(p_detail, 0, 0);
    ctrl_set_mrect(p_detail, 
                    YOUTUBE_INFO_DETAIL_MIDX, YOUTUBE_INFO_DETAIL_MIDY,
                    YOUTUBE_INFO_DETAIL_MIDX + YOUTUBE_INFO_DETAIL_MIDW, YOUTUBE_INFO_DETAIL_MIDY + YOUTUBE_INFO_DETAIL_MIDH);
    text_enable_page(p_detail, TRUE);
    //ctrl_set_sts(p_detail, OBJ_STS_HIDE);

    p_detail_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_YOUTUBE_INFO_DETAIL_SBAR, 
                                YOUTUBE_INFO_DETAIL_SBAR_X, YOUTUBE_INFO_DETAIL_SBAR_Y, 
                                YOUTUBE_INFO_DETAIL_SBAR_W, YOUTUBE_INFO_DETAIL_SBAR_H,
                                p_detail_cont, 0);
    ctrl_set_rstyle(p_detail_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_detail_sbar, 1);
    sbar_set_direction(p_detail_sbar, 0);
    sbar_set_mid_rstyle(p_detail_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_detail_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    YOUTUBE_INFO_DETAIL_SBAR_W, YOUTUBE_INFO_DETAIL_SBAR_H - COMM_SBAR_VERTEX_GAP);
    text_set_scrollbar(p_detail, p_detail_sbar);
    //text_reset_param(p_detail);
    //ctrl_set_sts(p_detail_sbar, OBJ_STS_HIDE);
    ctrl_set_sts(p_detail_cont, OBJ_STS_HIDE);

    //ctrl_process_msg(p_movlist, MSG_LOSTFOCUS, 0, 0);
    //ctrl_process_msg(p_catelist, MSG_LOSTFOCUS, 0, 0);
    //ctrl_process_msg(p_optlist, MSG_LOSTFOCUS, 0, 0);
    
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

    ui_youtube_register_msg();
    Youtube_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_YOUTUBE), NOTIFY_T_MSG, FALSE, MSG_OPEN_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE youtube_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@youtube_on_destory\n");

    Youtube_CloseStateTree();
    ui_youtube_unregister_msg();

    ui_video_c_destroy();

    ui_release_app_data();

    return ERR_NOFEATURE;
}

static RET_CODE youtube_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_YOUTUBE,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@youtube_on_open_cfm_dlg\n");

    if (Youtube_IsStateActive(SID_YOUTUBE_ACTIVE))
    {
      dlg_data.content = para1;

      ui_comm_dlg_open(&dlg_data);

      p_root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
      if (p_root)
      {
          fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
      }
    }

    return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(youtube_mainlist_keymap, NULL)
    //ON_EVENT(V_KEY_MENU, MSG_EXIT)
    //ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    //ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    //ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(youtube_mainlist_keymap, NULL)

BEGIN_MSGPROC(youtube_mainlist_proc, list_class_proc)
    //ON_COMMAND(MSG_EXIT, youtube_on_mainlist_exit)
    //ON_COMMAND(MSG_EXIT_ALL, youtube_on_mainlist_exit_all)
    ON_COMMAND(MSG_FOCUS_UP, youtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, youtube_on_state_process)
    ON_COMMAND(MSG_SELECT, youtube_on_state_process)
END_MSGPROC(youtube_mainlist_proc, list_class_proc)

BEGIN_KEYMAP(youtube_movlist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(youtube_movlist_keymap, NULL)

BEGIN_MSGPROC(youtube_movlist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, youtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_UP, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_BACK, youtube_on_state_process)
    ON_COMMAND(MSG_MAIN, youtube_on_state_process)
    ON_COMMAND(MSG_INFO, youtube_on_state_process)
    ON_COMMAND(MSG_SELECT, youtube_on_state_process)
END_MSGPROC(youtube_movlist_proc, list_class_proc)

BEGIN_KEYMAP(youtube_catelist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(youtube_catelist_keymap, NULL)

BEGIN_MSGPROC(youtube_catelist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, youtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_UP, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_BACK, youtube_on_state_process)
    ON_COMMAND(MSG_MAIN, youtube_on_state_process)
    ON_COMMAND(MSG_SELECT, youtube_on_state_process)
END_MSGPROC(youtube_catelist_proc, list_class_proc)

BEGIN_KEYMAP(youtube_optlist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(youtube_optlist_keymap, NULL)

BEGIN_MSGPROC(youtube_optlist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, youtube_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_UP, youtube_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, youtube_on_state_process)
    ON_COMMAND(MSG_BACK, youtube_on_state_process)
    ON_COMMAND(MSG_MAIN, youtube_on_state_process)
    ON_COMMAND(MSG_SELECT, youtube_on_state_process)
END_MSGPROC(youtube_optlist_proc, list_class_proc)

BEGIN_KEYMAP(youtube_info_detail_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_BACK)
    ON_EVENT(V_KEY_CANCEL, MSG_MAIN)
    ON_EVENT(V_KEY_UP, MSG_INCREASE)
    ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(youtube_info_detail_keymap, NULL)

BEGIN_MSGPROC(youtube_info_detail_proc, text_class_proc)
    ON_COMMAND(MSG_BACK, youtube_on_state_process)
    ON_COMMAND(MSG_MAIN, youtube_on_state_process)
    ON_COMMAND(MSG_SELECT, youtube_on_state_process)
END_MSGPROC(youtube_info_detail_proc, text_class_proc)

BEGIN_KEYMAP(youtube_cont_keymap, ui_comm_root_keymap)

END_KEYMAP(youtube_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(youtube_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, youtube_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, youtube_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, youtube_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, youtube_on_state_process)

    ON_COMMAND(MSG_YT_EVT_VDO_REQ_OPEN_SUCC, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_VDO_REQ_OPEN_FAIL, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_CATE_REQ_OPEN_SUCC, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_CATE_REQ_OPEN_FAIL, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_SEARCH_SUCC, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_SEARCH_FAIL, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_GET_PAGE_FAIL, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_NEW_PLAY_URL_ARRIVE, youtube_on_state_process)
    ON_COMMAND(MSG_YT_EVT_GET_URL_FAIL, youtube_on_state_process)

    ON_COMMAND(MSG_OPEN_REQ, youtube_on_state_process)
    ON_COMMAND(MSG_SAVE, youtube_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, youtube_on_state_process)
    ON_COMMAND(MSG_EXIT, youtube_on_state_process)
    ON_COMMAND(MSG_EXIT_ALL, youtube_on_state_process)
    ON_COMMAND(MSG_DESTROY, youtube_on_destory)
    ON_COMMAND(MSG_INPUT_COMP, youtube_on_state_process)  

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, youtube_on_open_cfm_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, youtube_on_state_process)
    ON_COMMAND(MSG_UPDATE, youtube_on_state_process)
END_MSGPROC(youtube_cont_proc, ui_comm_root_proc)
#endif
