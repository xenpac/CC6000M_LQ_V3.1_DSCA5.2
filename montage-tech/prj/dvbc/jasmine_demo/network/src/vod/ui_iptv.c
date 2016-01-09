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
//#include "lib_char.h"
#if ENABLE_NETWORK
#include "commonData.h"
#include "redxDataProvider.h"
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"

#include "ui_iptv_gprot.h"
#include "ui_iptv_prot.h"

#include "SM_StateMachine.h"
#include "States_VOD.h"
#include "ui_video.h"
#include "ui_picture_api.h"

/*!
 * Const value
 */
// Catlist container
#define IPTV_CATLIST_CONTX      (90)//(60)
#define IPTV_CATLIST_CONTY      (90)//(60)
#define IPTV_CATLIST_CONTW      (190)//(220)
#define IPTV_CATLIST_CONTH      (600)

// Title icon
//#define IPTV_TITLE_ICON_X         (20)//(17)//(27)
//#define IPTV_TITLE_ICON_Y         (18)//(16)
//#define IPTV_TITLE_ICON_W         (54)//(80)
//#define IPTV_TITLE_ICON_H         (54)//(80)

// Title name
#define IPTV_TITLE_NAME_X         (20)//(IPTV_TITLE_ICON_X + IPTV_TITLE_ICON_W + 4)//(118)
#define IPTV_TITLE_NAME_Y         (18)//(IPTV_TITLE_ICON_Y)
#define IPTV_TITLE_NAME_W         (IPTV_CATLIST_CONTW - IPTV_TITLE_NAME_X - IPTV_TITLE_NAME_X)//(IPTV_CATLIST_CONTW - IPTV_TITLE_NAME_X - IPTV_TITLE_ICON_X)//(100)
#define IPTV_TITLE_NAME_H         (54)//(IPTV_TITLE_ICON_H)

// Catlist
#define IPTV_CATLIST_X                ((SUB_DP_ID == 21337) ? 5 : 10)//(5)
#define IPTV_CATLIST_Y                (IPTV_TITLE_NAME_Y + IPTV_TITLE_NAME_H + 15)
#define IPTV_CATLIST_W                (IPTV_CATLIST_CONTW - IPTV_CATLIST_X - IPTV_CATLIST_X)
#define IPTV_CATLIST_H                (471)//(IPTV_CATLIST_CONTH - IPTV_CATLIST_Y - IPTV_TITLE_ICON_Y)

#define IPTV_CATLIST_MIDL           (0)
#define IPTV_CATLIST_MIDT           (0)
#define IPTV_CATLIST_MIDR           (IPTV_CATLIST_MIDL + IPTV_CATLIST_W)
#define IPTV_CATLIST_MIDB           (IPTV_CATLIST_MIDT + IPTV_CATLIST_H)

#define IPTV_CATLIST_ITEM_V_GAP (2)//(4)

#define IPTV_CATLIST_PAGE_SIZE    (11)//(13)

#define IPTV_CATLIST_FIELD_X                ((SUB_DP_ID == 21337) ? 10 : 25)
#define IPTV_CATLIST_FIELD_Y                (0)
#define IPTV_CATLIST_FIELD_W                (IPTV_CATLIST_W - IPTV_CATLIST_FIELD_X - IPTV_CATLIST_FIELD_X)

// Reslist container
#define IPTV_RESLIST_CONTX             (290)//(IPTV_RESLIST_CONTX)
#define IPTV_RESLIST_CONTY             (IPTV_CATLIST_CONTY)//(IPTV_RESLIST_CONTY + IPTV_RESLIST_CONTH)
#define IPTV_RESLIST_CONTW             (900)//(930)//(IPTV_RESLIST_CONTW)
#define IPTV_RESLIST_CONTH             (60)//(IPTV_CATLIST_CONTH - IPTV_RESLIST_CONTH)

#define IPTV_RESLIST_X              ((IPTV_RESLIST_CONTW - IPTV_RESLIST_W) >> 1)//(75)//(83)
#define IPTV_RESLIST_Y              ((IPTV_RESLIST_CONTH - IPTV_RESLIST_H) >> 1)//(0)
#define IPTV_RESLIST_W              (IPTV_RESLIST_PAGE_SIZE*IPTV_RESLIST_FIELD_W)// (776+56+48)//(776)
#define IPTV_RESLIST_H              (40)//(60)

#define IPTV_RESLIST_MIDL           (0)
#define IPTV_RESLIST_MIDT           (0)
#define IPTV_RESLIST_MIDR           (IPTV_RESLIST_MIDL + IPTV_RESLIST_W)
#define IPTV_RESLIST_MIDB           (IPTV_RESLIST_MIDT + IPTV_RESLIST_H)

//#define IPTV_RESLIST_ITEM_H_GAP (7)

#define IPTV_RESLIST_PAGE_SIZE  (7)

#define IPTV_RESLIST_FIELD_X                (0)
#define IPTV_RESLIST_FIELD_Y                (0)
#define IPTV_RESLIST_FIELD_W                (110)//(97)

// Client container
#define IPTV_CLIENT_CONTX             (IPTV_RESLIST_CONTX)//(IPTV_RESLIST_CONTX)
#define IPTV_CLIENT_CONTY             (IPTV_RESLIST_CONTY + IPTV_RESLIST_CONTH)//(IPTV_RESLIST_CONTY + IPTV_RESLIST_CONTH)
#define IPTV_CLIENT_CONTW             (IPTV_RESLIST_CONTW)//(930)//(IPTV_RESLIST_CONTW)
#define IPTV_CLIENT_CONTH             (IPTV_CATLIST_CONTH - IPTV_RESLIST_CONTH - IPTV_PAGE_CONTH)//(IPTV_CATLIST_CONTH - IPTV_RESLIST_CONTH)
// Video list
#define IPTV_VDOLIST_X                  ((IPTV_CLIENT_CONTW - IPTV_VDOLIST_W) >> 1)//(28)//(45)
#define IPTV_VDOLIST_Y                  (IPTV_VDOLIST_ITEM_V_GAP)//(27)
#define IPTV_VDOLIST_W                  (844)//(830)
#define IPTV_VDOLIST_H                  (IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_V_GAP)//(460)

#define IPTV_VDOLIST_ITEM_X         (0)
#define IPTV_VDOLIST_ITEM_Y         (0)
#define IPTV_VDOLIST_ITEM_W         (156)//(150)
#define IPTV_VDOLIST_ITEM_H         (220)

#define IPTV_VDOLIST_ITEM_H_GAP (16)//(20)
#define IPTV_VDOLIST_ITEM_V_GAP (15)

#define IPTV_VDOPIC_X                   (0)
#define IPTV_VDOPIC_Y                   (0)
#define IPTV_VDOPIC_W                   (IPTV_VDOLIST_ITEM_W)//(150)
#define IPTV_VDOPIC_H                   (180)

#define IPTV_VDOPIC_MIDL                   (5)
#define IPTV_VDOPIC_MIDT                   (5)
#define IPTV_VDOPIC_MIDR                   (IPTV_VDOPIC_W - IPTV_VDOPIC_MIDL)
#define IPTV_VDOPIC_MIDB                   (IPTV_VDOPIC_H - IPTV_VDOPIC_MIDT)

#define IPTV_VDONAME_X                (IPTV_VDOPIC_X)
#define IPTV_VDONAME_Y                (IPTV_VDOPIC_Y + IPTV_VDOPIC_H)
#define IPTV_VDONAME_W                (IPTV_VDOPIC_W)
#define IPTV_VDONAME_H                (40)

#define IPTV_VDONAME_MIDL                   (5)
#define IPTV_VDONAME_MIDT                   (5)
#define IPTV_VDONAME_MIDR                   (IPTV_VDONAME_W - IPTV_VDONAME_MIDL)
#define IPTV_VDONAME_MIDB                   (IPTV_VDONAME_H - IPTV_VDONAME_MIDT)

#define IPTV_VDOLIST_PAGE_SIZE    (10)
#define IPTV_VDOLIST_ITEM_ROW     (2)
#define IPTV_VDOLIST_ITEM_COL     (5)


//Page container
#define IPTV_PAGE_CONTX    (IPTV_RESLIST_CONTX)
#define IPTV_PAGE_CONTY    (IPTV_CLIENT_CONTY + IPTV_CLIENT_CONTH)
#define IPTV_PAGE_CONTW    (IPTV_RESLIST_CONTW)
#define IPTV_PAGE_CONTH    (60)

//Page number
#define IPTV_PAGE_NUMBER_X    ((IPTV_PAGE_CONTW - IPTV_PAGE_NUMBER_W) >> 1)
#define IPTV_PAGE_NUMBER_Y    ((IPTV_PAGE_CONTH - IPTV_PAGE_NUMBER_H) >> 1)
#define IPTV_PAGE_NUMBER_W    (150)
#define IPTV_PAGE_NUMBER_H    (40)

/*!
 * Macro
 */

/*!
 * Type define
 */
// Video view control id
typedef enum
{
    IDC_IPTV_RESLIST_CONT = 1,
    IDC_IPTV_RESLIST,

    IDC_IPTV_CATLIST_CONT,
    IDC_IPTV_TITLE_NAME,
    IDC_IPTV_CATLIST,

    IDC_IPTV_CLIENT_CONT,
    IDC_IPTV_VDOLIST,
    IDC_IPTV_VDOITEM_START,
    IDC_IPTV_VDOITEM_END = IDC_IPTV_VDOITEM_START + IPTV_VDOLIST_PAGE_SIZE - 1,
    IDC_IPTV_VDOITEM_PIC,
    IDC_IPTV_VDOITEM_NAME,

    IDC_IPTV_PAGE_CONT,
    IDC_IPTV_PAGE_NUM
} ui_iptv_ctrl_id_t;

typedef enum 
{
    PAGER_KEY_TYPE_NONE,
    PAGER_KEY_TYPE_PREV_PAGE,  //page navigation
    PAGER_KEY_TYPE_NEXT_PAGE,
    PAGER_KEY_TYPE_PAGE_NUM,

    PAGER_KEY_TYPE_MAX_COUNT
} ui_pager_key_type_t;

typedef struct
{
    u32 res_id;
    u16 *name;

    u16 total_cat;
    al_iptv_cat_item_t *catList;
} ui_iptv_res_item_t;

typedef struct
{
    control_t *hResList;
    control_t *hCatList;
    control_t *hVdoList;
    control_t *hTitleName;
    control_t *hPageNum;

    ui_iptv_res_item_t *resList;
    u16 total_res;
    u16 curResListIdx;

    u16 **mainMenuName;
    u16 totalMainMenu;

    al_iptv_cat_item_t *catList;
    u16 total_cat;
    u16 curCatListIdx;

    al_iptv_vdo_item_t *vdoList;
    u32 total_vdo;//
    u16 vdo_cnt;
    u16 curVdoListIdx;
    u16 curVdoListRendIdx;

    u16 vdo_page_size;

    u32 total_page;     // total video page
    u32 curPageNum;   // video page num
    u32 jumpPageNum;
    BOOL b_totalPageChanged;

} ui_iptv_app_t;

/*!
 * Function define
 */
u16 iptv_reslist_keymap(u16 key);
RET_CODE iptv_reslist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_catlist_keymap(u16 key);
RET_CODE iptv_catlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_vdolist_keymap(u16 key);
RET_CODE iptv_vdolist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_page_num_keymap(u16 key);
RET_CODE iptv_page_num_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_cont_keymap(u16 key);
RET_CODE iptv_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static BOOL ui_release_app_data(void);

/*!
 * Priv data
 */
// Category list style
static list_xstyle_t catlist_item_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_OTT_BUTTON_HL,
    RSI_OTT_BUTTON_SH,
    RSI_OTT_BUTTON_HL,
};

static list_xstyle_t catlist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t catlist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
};
static const list_field_attr_t catlist_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
        IPTV_CATLIST_FIELD_W,
        IPTV_CATLIST_FIELD_X,
        IPTV_CATLIST_FIELD_Y,
        &catlist_field_rstyle,
        &catlist_field_fstyle
    }
};

// Resource list style
static list_xstyle_t reslist_item_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_OTT_BUTTON_HL,
    RSI_OTT_BUTTON_SH,
    RSI_OTT_BUTTON_HL,
};

static list_xstyle_t reslist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t reslist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
};
static const list_field_attr_t reslist_attr[] =
{
    {
        LISTFIELD_TYPE_EXTSTR | STL_CENTER | STL_VCENTER,
        IPTV_RESLIST_FIELD_W,
        IPTV_RESLIST_FIELD_X,
        IPTV_RESLIST_FIELD_Y,
        &reslist_field_rstyle,
        &reslist_field_fstyle
    }
};

// App data
static ui_iptv_app_t *this = NULL;


/*================================================================================================
                           iptv internel function
 ================================================================================================*/

static BOOL ui_init_app_data(void)
{
    if (this)
    {
        ui_release_app_data();
    }

    this = (ui_iptv_app_t *)mtos_malloc(sizeof(ui_iptv_app_t));
    MT_ASSERT(this != NULL);
    memset((void *)this, 0, sizeof(ui_iptv_app_t));

    return TRUE;
}

static void ui_release_vdolist(void)
{
    u16 i;

    if (this->vdoList)
    {
        for (i = 0; i < this->vdo_cnt; i++)
        {
            if (this->vdoList[i].name)
            {
                mtos_free(this->vdoList[i].name);
            }
            if (this->vdoList[i].img_url)
            {
                mtos_free(this->vdoList[i].img_url);
            }
        }
        mtos_free(this->vdoList);
        this->vdoList = NULL;
    }
}

static BOOL ui_release_app_data(void)
{
    u16 i, j;

    if (this)
    {
        if (this->resList)
        {
            for (i = 0; i < this->total_res; i++)
            {
                if (this->resList[i].name)
                {
                    mtos_free(this->resList[i].name);
                }
                if (this->resList[i].catList)
                {
                    for (j = 0; j < this->resList[i].total_cat; j++)
                    {
                        if (this->resList[i].catList[j].name)
                        {
                            mtos_free(this->resList[i].catList[j].name);
                        }
                        if (this->resList[i].catList[j].key)
                        {
                            mtos_free(this->resList[i].catList[j].key);
                        }
                    }
                    mtos_free(this->resList[i].catList);
                }
            }
            mtos_free(this->resList);
        }

        if (this->mainMenuName)
        {
            mtos_free(this->mainMenuName);
        }

        ui_release_vdolist();

        mtos_free(this);
        this = NULL;
    }
    
    return TRUE;
}

static void ui_iptv_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void ui_iptv_pic_deinit(void)
{
  pic_stop();

  ui_pic_release();
}

static RET_CODE ui_iptv_pic_play_start()
{
    control_t *p_item = NULL, *p_pic = NULL;
    rect_t rect;

    OS_PRINTF("@@@ui_iptv_pic_play_start curVdoListRendIdx=%d, vdo_cnt=%d\n", this->curVdoListRendIdx, this->vdo_cnt);
    if (this->vdoList)
    {
        while (this->curVdoListRendIdx < this->vdo_cnt)
        {
            if (this->vdoList[this->curVdoListRendIdx].img_url 
                && strlen(this->vdoList[this->curVdoListRendIdx].img_url) > 0)
            {
                p_item = ctrl_get_child_by_id(this->hVdoList, IDC_IPTV_VDOITEM_START + this->curVdoListRendIdx);
                p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);

                ctrl_get_frame(p_pic, &rect);
                ctrl_client2screen(p_pic, &rect);

                ui_pic_play_by_url(this->vdoList[this->curVdoListRendIdx].img_url, &rect, 0);
                return SUCCESS;
            }
            this->curVdoListRendIdx++;
        }
    }

    return ERR_NOFEATURE;
}

/*!
 * list update function
 */
static RET_CODE ui_iptv_update_catlist(control_t *p_catlist, u16 start, u16 size, u32 context)
{
    u16 *p_unistr;
    u16 i, cnt;

    cnt = list_get_count(p_catlist);

    OS_PRINTF("@@@ui_iptv_update_catlist start=%d\n", start);
    if (start == 0)
    {
        p_unistr = (u16 *)gui_get_string_addr(IDS_SEARCH);
        list_set_field_content_by_unistr(p_catlist, 0, 0, p_unistr);

        i = 1;
    }
    else
    {
        i = start;
    }

    for (; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_unistr(p_catlist, i, 0, this->catList[i - 1].name);
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_update_reslist(control_t *p_reslist, u16 start, u16 size, u32 context)
{
    u16 i, cnt;

    OS_PRINTF("@@@ui_iptv_update_reslist start=%d\n", start);
    cnt = list_get_count(p_reslist);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_extstr(p_reslist, (u16)i, 0, this->mainMenuName[i]);
    }

    return SUCCESS;
}

static control_t *ui_iptv_create_vdolist(control_t *p_parent)
{
    control_t *p_vdolist = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    u16 i;
    u16 x, y;

    OS_PRINTF("@@@ui_iptv_create_vdolist\n");
    p_vdolist = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_VDOLIST,
                                  IPTV_VDOLIST_X, IPTV_VDOLIST_Y,
                                  IPTV_VDOLIST_W, IPTV_VDOLIST_H,
                                  p_parent, 0);
    ctrl_set_rstyle(p_vdolist, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    this->vdo_page_size = IPTV_VDOLIST_PAGE_SIZE;

    x = IPTV_VDOLIST_ITEM_X;
    y = IPTV_VDOLIST_ITEM_Y;
    for (i = 0; i < this->vdo_page_size; i++)
    {
        p_item = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_VDOITEM_START + i,
                                x, y,
                                IPTV_VDOLIST_ITEM_W, IPTV_VDOLIST_ITEM_H,
                                p_vdolist, 0);
        ctrl_set_rstyle(p_item, RSI_PBACK, RSI_PBACK, RSI_PBACK);

        p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_VDOITEM_PIC,
                                IPTV_VDOPIC_X, IPTV_VDOPIC_Y,
                                IPTV_VDOPIC_W, IPTV_VDOPIC_H,
                                p_item, 0);
        //ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_FRM_1_SH, RSI_OTT_FRM_1_HL, RSI_OTT_FRM_1_SH);
        ctrl_set_mrect(p_ctrl, 
                        IPTV_VDOPIC_MIDL, IPTV_VDOPIC_MIDT,
                        IPTV_VDOPIC_MIDR, IPTV_VDOPIC_MIDB);

        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_VDOITEM_NAME,
                                IPTV_VDONAME_X, IPTV_VDONAME_Y,
                                IPTV_VDONAME_W, IPTV_VDONAME_H,
                                p_item, 0);
        //ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH);
        ctrl_set_mrect(p_ctrl, 
                        IPTV_VDONAME_MIDL, IPTV_VDONAME_MIDT,
                        IPTV_VDONAME_MIDR, IPTV_VDONAME_MIDB);

        text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);

        x += IPTV_VDOLIST_ITEM_W + IPTV_VDOLIST_ITEM_H_GAP;
        if (x + IPTV_VDOPIC_W > IPTV_VDOLIST_W)
        {
            x = IPTV_VDOLIST_ITEM_X;
            y += IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_V_GAP;
        }
    }

    return p_vdolist;
}

/* to do show/hide child ctrl
    vdolist pre update
*/
static RET_CODE ui_iptv_vdolist_set_count(control_t *p_vdolist, u16 count)
{
    control_t *p_item = NULL, *p_pic = NULL, *p_text = NULL;
    rect_t rect;
    u16 i;

    OS_PRINTF("@@@ui_iptv_vdolist_set_count count=%d\n", count);
    MT_ASSERT(count <= this->vdo_page_size);

    for (i = 0; i < count; i++)
    {
        p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);
        p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_NAME);

        ctrl_set_sts(p_item, OBJ_STS_SHOW);
        bmap_set_content_by_id(p_pic, IM_PIC_LOADING_1);
        text_set_content_by_extstr(p_text, NULL);
    }
    for (i = count; i < this->vdo_page_size; i++)
    {
        p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);

        ctrl_set_sts(p_item, OBJ_STS_HIDE);

        //clear pic layer
        pic_stop();
        ctrl_get_frame(p_pic, &rect);
        ctrl_client2screen(p_pic, &rect);
        //OS_PRINTF("@@@left=%d, top=%d, right=%d, bottom=%d\n", rect.left, rect.top, rect.right, rect.bottom);
        ui_pic_clear_rect(&rect, 0);
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_update_vdolist(control_t *p_vdolist, u16 count)
{
    control_t *p_item = NULL, *p_text = NULL;
    u16 i;

    OS_PRINTF("@@@ui_iptv_update_vdolist count=%d\n", count);
    MT_ASSERT(count <= this->vdo_page_size);

    if (this->vdoList)
    {
        for (i = 0; i < count && i < this->vdo_page_size; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
            p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_NAME);
            text_set_content_by_extstr(p_text, this->vdoList[i].name);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_vdolist_set_focus_pos(control_t *p_vdolist, u16 focus, BOOL is_paint)
{
    control_t *p_active_item = NULL, *p_active_pic = NULL;
    control_t *p_next_item = NULL, *p_next_pic = NULL;

    OS_PRINTF("@@@ui_iptv_vdolist_set_focus_pos focus=%d\n", focus);
    MT_ASSERT(focus < this->vdo_page_size);

    p_active_item = p_vdolist->p_active_child;
    if (p_active_item != NULL)
    {
        p_active_pic = ctrl_get_child_by_id(p_active_item, IDC_IPTV_VDOITEM_PIC);
        MT_ASSERT(p_active_pic != NULL);
        ctrl_process_msg(p_active_pic, MSG_LOSTFOCUS, 0, 0);
    }

    p_next_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + focus);
    p_next_pic = ctrl_get_child_by_id(p_next_item, IDC_IPTV_VDOITEM_PIC);
    MT_ASSERT(p_next_pic != NULL);
    ctrl_set_attr(p_next_pic, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(p_next_pic, MSG_GETFOCUS, 0, 0);

    if (is_paint)
    {
        if (p_active_pic != NULL)
        {
            ctrl_paint_ctrl(p_active_pic, TRUE);
        }
        if (p_next_pic != NULL)
        {
            ctrl_paint_ctrl(p_next_pic, TRUE);
        }
    }

    return SUCCESS;
}

static void ui_iptv_update_curVdoPage(void)
{
    OS_PRINTF("@@@ui_iptv_update_curVdoPage\n");
    if (ctrl_is_onfocus(this->hVdoList))
    {
        ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, FALSE);
    }

    ui_iptv_update_vdolist(this->hVdoList, this->vdo_cnt);
}

static void ui_iptv_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[23 + 1];
  
    sprintf(num, "%ld / %ld", page_num, total_page);
    text_set_content_by_ascstr(p_ctrl, num);
}

/*================================================================================================
                           iptv sm function
 ================================================================================================*/

static BOOL    StcIptv_is_on_reslist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_RESLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_catlist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_CATLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_VDOLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_PAGE_NUM) ? TRUE : FALSE;
}

static void SenIptv_Inactive(void)
{
    OS_PRINTF("@@@SenIptv_Inactive\n");
}

static STATEID StaIptv_inactive_on_open_iptv_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_inactive_on_open_iptv_req\n");
    return SID_IPTV_INITIALIZE;
}

static void SexIptv_Inactive(void)
{
    OS_PRINTF("@@@SexIptv_Inactive\n");
}

static void SenIptv_Active(void)
{
    OS_PRINTF("@@@SenIptv_Active\n");
}

static STATEID StaIptv_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Iptv_IsStateActive(SID_IPTV_DEINIT))
    {
        ui_iptv_dp_deinit();
    }

    return SID_NULL;
}

static STATEID StaIptv_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Iptv_IsStateActive(SID_IPTV_DEINIT))
    {
        return SID_IPTV_DEINIT;
    }

    return SID_NULL;
}

static void SexIptv_Active(void)
{
    OS_PRINTF("@@@SexIptv_Active\n");
}

/* --------------------- Initialize:  ---------------------*/
static void SenIptv_Initialize(void)
{
    OS_PRINTF("@@@SenIptv_Initialize\n");
    ui_iptv_open_dlg(ROOT_ID_IPTV, IDS_LOADING_WITH_WAIT);

    ui_iptv_dp_init();
    ui_iptv_set_page_size(this->vdo_page_size);
}

static STATEID StaIptv_initialize_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_iptv_set_dp_state(IPTV_DP_STATE_INIT_SUCC);
    ui_iptv_get_res_list();
    return SID_NULL;
}

static STATEID StaIptv_initialize_on_newresname_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_name_res_list_t *p_data = (al_iptv_name_res_list_t *)para1;
    u16 str_len;
    u16 i;

    OS_PRINTF("@@@StaIptv_initialize_on_newresname_arrive\n");
    if (p_data->total_res > 0 && p_data->resList != NULL)
    {
        if (this->resList)
        {
            mtos_free(this->resList);
        }

        this->resList = (ui_iptv_res_item_t *)mtos_malloc(p_data->total_res * sizeof(ui_iptv_res_item_t));
        MT_ASSERT(this->resList != NULL);
        memset(this->resList, 0, p_data->total_res * sizeof(ui_iptv_res_item_t));

        this->total_res = p_data->total_res;
        this->totalMainMenu = p_data->total_res
#if ENABLE_IPTV_FAV
                                             + 1
#endif
#if ENABLE_PLAY_HIST
                                             + 1
#endif
        ;
        this->mainMenuName = (u16 **)mtos_malloc(this->totalMainMenu * sizeof(u16 *));
        this->curResListIdx = 0;

        for (i = 0; i < p_data->total_res; i++)
        {
            this->resList[i].res_id = p_data->resList[i].res_id;
            str_len = (u16)uni_strlen(p_data->resList[i].name);
            this->resList[i].name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
            MT_ASSERT(this->resList[i].name != NULL);
            uni_strcpy(this->resList[i].name, p_data->resList[i].name);

            this->mainMenuName[i] = this->resList[i].name;
        }
#if ENABLE_IPTV_FAV
        this->mainMenuName[i] = (u16 *)gui_get_string_addr(IDS_FAVORITE);
        i++;
#endif
#if ENABLE_PLAY_HIST
        this->mainMenuName[i] = (u16 *)gui_get_string_addr(IDS_PLAY_HISTORY);
#endif

        // update ui ctrl
        OS_PRINTF("@@@hTitleName attr=%d\n", ctrl_get_attr(this->hTitleName));
        text_set_content_by_extstr(this->hTitleName, this->mainMenuName[this->curResListIdx]);
        ctrl_set_sts(this->hTitleName, OBJ_STS_SHOW);

        list_set_count(this->hResList, this->totalMainMenu, IPTV_RESLIST_PAGE_SIZE);
        list_set_focus_pos(this->hResList, this->curResListIdx);
        list_select_item(this->hResList, this->curResListIdx);
        ui_iptv_update_reslist(this->hResList, list_get_valid_pos(this->hResList), IPTV_RESLIST_PAGE_SIZE, 0);

        ctrl_paint_ctrl(this->hTitleName, TRUE);
        ctrl_set_sts(this->hResList, OBJ_STS_SHOW);
        ctrl_change_focus(NULL, this->hResList);

        ui_iptv_get_catgry_list();
        return SID_NULL;
    }

    return SID_IPTV_INIT_FAILED;
}

static STATEID StaIptv_initialize_on_newrescatgry_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_cat_res_list_t *p_data = (al_iptv_cat_res_list_t *)para1;
    al_iptv_cat_item_t *p_catList;
    u16 i, j, k;
    u16 str_len;

    OS_PRINTF("@@@StaIptv_initialize_on_newrescatgry_arrive\n");
    if (p_data->total_res > 0 && p_data->resList != NULL)
    {
        for (i = 0; i < this->total_res; i++)
        {
            for (j = 0; j < p_data->total_res; j++)
            {
                if (this->resList[i].res_id == p_data->resList[j].res_id)
                {
                    if (p_data->resList[j].total_cat > 0)
                    {
                        p_catList = (al_iptv_cat_item_t *)mtos_malloc(p_data->resList[j].total_cat * sizeof(al_iptv_cat_item_t));
                        MT_ASSERT(p_catList != NULL);

                        for (k = 0; k < p_data->resList[i].total_cat; k++)
                        {
                            if (p_data->resList[j].catList[k].name)
                            {
                                str_len = uni_strlen(p_data->resList[j].catList[k].name);
                                p_catList[k].name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                                MT_ASSERT(p_catList[k].name != NULL);
                                uni_strcpy(p_catList[k].name, p_data->resList[j].catList[k].name);
                            }
                            else
                            {
                                p_catList[k].name = NULL;
                            }

                            if (p_data->resList[j].catList[k].key)
                            {
                                str_len = strlen(p_data->resList[j].catList[k].key);
                                p_catList[k].key = (u8 *)mtos_malloc((str_len + 1));
                                MT_ASSERT(p_catList[k].key != NULL);
                                strcpy(p_catList[k].key, p_data->resList[j].catList[k].key);
                            }
                            else
                            {
                                p_catList[k].key = NULL;
                            }

                            p_catList[k].total_vdo = p_data->resList[j].catList[k].total_vdo;
                        }
                    }
                    else
                    {
                        p_catList = NULL;
                    }
                    this->resList[i].total_cat = p_data->resList[j].total_cat;
                    this->resList[i].catList = p_catList;
                }
            }
        }
        this->catList = this->resList[this->curResListIdx].catList;
        this->total_cat = this->resList[this->curResListIdx].total_cat;
        this->curCatListIdx = 0;

        list_set_count(this->hCatList, this->total_cat + 1, IPTV_CATLIST_PAGE_SIZE);
        if (this->total_cat > 0)
        {
            list_set_focus_pos(this->hCatList, this->curCatListIdx + 1);
            list_select_item(this->hCatList, this->curCatListIdx + 1);
        }
        else
        {
            list_set_focus_pos(this->hCatList, 0);
            list_select_item(this->hCatList, 0);
        }

        ui_iptv_update_catlist(this->hCatList, list_get_valid_pos(this->hCatList), IPTV_CATLIST_PAGE_SIZE, 0);
#if 0
        ctrl_process_msg(this->hResList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_sts(this->hCatList, OBJ_STS_SHOW);
        ctrl_set_attr(this->hCatList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(this->hCatList, MSG_GETFOCUS, 0, 0);

        ctrl_paint_ctrl(this->hResList, TRUE);
        ctrl_paint_ctrl(this->hCatlistCont, TRUE);
#else
        ctrl_set_sts(this->hCatList, OBJ_STS_SHOW);
        ctrl_change_focus(this->hResList, this->hCatList);
#endif
        return SID_IPTV_RESOURCE;
    }

    return SID_IPTV_INIT_FAILED;
}

static void SexIptv_Initialize(void)
{
    OS_PRINTF("@@@SexIptv_Initialize\n");
    ui_comm_dlg_close();
}

/* --------------------- Video list: Resource ---------------------*/
static void SenIptv_Resource(void)
{
    OS_PRINTF("@@@SenIptv_Resource\n");
    this->total_vdo = (this->catList != NULL) ? this->catList[this->curCatListIdx].total_vdo : 0;
    this->total_page = (this->total_vdo > 0) ? ((this->total_vdo + this->vdo_page_size - 1) / this->vdo_page_size) : 1;
    this->b_totalPageChanged = TRUE;
    this->curPageNum = 1;
    this->curVdoListIdx = 0;

    ctrl_set_attr(this->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(this->hVdoList, OBJ_STS_SHOW);
}

static STATEID StaIptv_resource_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;
    u8 ctrl_id = ctrl_get_ctrl_id(ctrl);

    OS_PRINTF("@@@StaIptv_resource_on_focus_key_hldr\n");
    if (ctrl_id == IDC_IPTV_VDOLIST)
    {
        OS_PRINTF("@@@IDC_IPTV_VDOLIST curVdoListIdx=%d\n", this->curVdoListIdx);
        switch(para1)
        {
            case MSG_FOCUS_UP:
                if (this->curVdoListIdx >= IPTV_VDOLIST_ITEM_COL)
                {
                    this->curVdoListIdx -= IPTV_VDOLIST_ITEM_COL;
                    ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                }
                else
                {
                    if (this->curPageNum > 1)
                    {
                        this->curPageNum--;
                        return SID_IPTV_RES_VDO_REQ;
                    }
                    else
                    {
                        ctrl_change_focus(this->hVdoList, this->hResList);
                    }
                }
                break;

            case MSG_FOCUS_DOWN:
                if (this->curVdoListIdx < IPTV_VDOLIST_ITEM_COL
                  && this->curVdoListIdx + IPTV_VDOLIST_ITEM_COL < this->vdo_cnt)
                {
                    this->curVdoListIdx += IPTV_VDOLIST_ITEM_COL;
                    ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                }
                else
                {
                    if (this->curPageNum < this->total_page)
                    {
                        this->curPageNum++;
                        return SID_IPTV_RES_VDO_REQ;
                    }
                }
                break;

            case MSG_FOCUS_LEFT:
                if (this->curVdoListIdx % IPTV_VDOLIST_ITEM_COL != 0)
                {
                    this->curVdoListIdx--;
                    ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                }
                else
                {
                    ctrl_change_focus(this->hVdoList, this->hCatList);
                }
                break;

            case MSG_FOCUS_RIGHT:
                if (this->curVdoListIdx + 1 < this->vdo_cnt)
                {
                    this->curVdoListIdx++;
                    ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                }
                else
                {
                    if (this->curPageNum < this->total_page)
                    {
                        this->curPageNum++;
                        return SID_IPTV_RES_VDO_REQ;
                    }
                }
                break;
        }
        OS_PRINTF("@@@curVdoListIdx=%d\n", this->curVdoListIdx);
    }
    else if (ctrl_id == IDC_IPTV_CATLIST)
    {
        OS_PRINTF("@@@IDC_IPTV_CATLIST\n");
        switch(para1)
        {
            case MSG_FOCUS_UP:
                focus = list_get_focus_pos(this->hCatList);
                if (focus != 0)
                {
                    list_class_proc(this->hCatList, MSG_FOCUS_UP, 0, 0);
                }
                else
                {
                    ctrl_change_focus(this->hCatList, this->hResList);
                }
                break;

            case MSG_FOCUS_DOWN:
                list_class_proc(this->hCatList, MSG_FOCUS_DOWN, 0, 0);
                break;

            case MSG_FOCUS_RIGHT:
                focus = list_get_focus_pos(this->hCatList);
                if (focus != 0)
                {
                    ctrl_process_msg(this->hCatList, MSG_LOSTFOCUS, 0, 0);
                    ctrl_paint_ctrl(this->hCatList, TRUE);

                    ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                    ctrl_paint_ctrl(this->hVdoList, TRUE);
                }
                else
                {
                    ctrl_change_focus(this->hCatList, this->hResList);
                }
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_RESLIST)
    {
        OS_PRINTF("@@@IDC_IPTV_RESLIST\n");
        switch (para1)
        {
            case MSG_FOCUS_LEFT:
                focus = list_get_focus_pos(this->hResList);
                if (focus != 0)
                {
                    list_class_proc(this->hResList, MSG_FOCUS_LEFT, 0, 0);
                }
                else
                {
                    ctrl_change_focus(this->hResList, this->hCatList);
                }
                break;

            case MSG_FOCUS_RIGHT:
                list_class_proc(this->hResList, MSG_FOCUS_RIGHT, 0, 0);
                break;

            case MSG_FOCUS_DOWN:
                ctrl_process_msg(this->hResList, MSG_LOSTFOCUS, 0, 0);
                ctrl_paint_ctrl(this->hResList, TRUE);

                ui_iptv_vdolist_set_focus_pos(this->hVdoList, this->curVdoListIdx, TRUE);
                break;
        }
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_resource_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (this->curPageNum > 1)
        {
            this->curPageNum--;
            return SID_IPTV_RES_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (this->curPageNum < this->total_page)
        {
            this->curPageNum++;
            return SID_IPTV_RES_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_catlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    OS_PRINTF("@@@StaIptv_resource_on_catlist_selected focus=%d\n", focus);
    if (focus != 0)
    {
        this->curCatListIdx = focus - 1;
        ctrl_paint_ctrl(ctrl, TRUE);

        return SID_IPTV_RESOURCE;
    }
    else
    {//search
        ui_iptv_pic_deinit();
        ui_iptv_search_set_res_id(this->resList[this->curResListIdx].res_id);
        manage_open_menu(ROOT_ID_IPTV_SEARCH, 0, 0);
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_reslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    OS_PRINTF("@@@StaIptv_resource_on_reslist_selected\n");
    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    if (focus < this->total_res)
    {
        this->curResListIdx = focus;
        this->catList = this->resList[this->curResListIdx].catList;
        this->total_cat = this->resList[this->curResListIdx].total_cat;
        this->curCatListIdx = 0;

        text_set_content_by_extstr(this->hTitleName, this->mainMenuName[this->curResListIdx]);

        list_set_count(this->hCatList, this->total_cat+1, IPTV_CATLIST_PAGE_SIZE);
        if (this->total_cat > 0)
        {
            list_set_focus_pos(this->hCatList, this->curCatListIdx+1);
            list_select_item(this->hCatList, this->curCatListIdx+1);
        }
        else
        {
            list_set_focus_pos(this->hCatList, 0);
            list_select_item(this->hCatList, 0);
        }

        ui_iptv_update_catlist(this->hCatList, list_get_valid_pos(this->hCatList), IPTV_CATLIST_PAGE_SIZE, 0);
#if 0
        ctrl_process_msg(this->hResList, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(this->hCatList, MSG_GETFOCUS, 0, 0);

        ctrl_paint_ctrl(this->hResList, TRUE);
        ctrl_paint_ctrl(this->hCatlistCont, TRUE);
#else
        ctrl_paint_ctrl(this->hTitleName, TRUE);
        ctrl_change_focus(this->hResList, this->hCatList);
#endif
        return SID_IPTV_RESOURCE;
    }
#if ENABLE_IPTV_FAV
    else if (focus == this->total_res)
    {
        ui_iptv_pic_deinit();
        manage_open_menu(ROOT_ID_VDO_FAVORITE, 1, 0);
    }
#endif
#if ENABLE_PLAY_HIST
    else if (focus == this->total_res + 1)
    {
        ui_iptv_pic_deinit();
        manage_open_menu(ROOT_ID_PLAY_HIST, 0, 0);
    }
#endif

    return SID_NULL;
}

static STATEID StaIptv_resource_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_resource_on_subwindow_exit\n");
    ui_iptv_pic_init();
    ui_iptv_set_page_size(this->vdo_page_size);

    if (Iptv_IsStateActive(SID_IPTV_RES_VDOLIST_NORMAL))
    {
        ui_iptv_vdolist_set_count(this->hVdoList, this->vdo_cnt);
        ui_iptv_update_curVdoPage();
        ctrl_paint_ctrl(this->hVdoList, TRUE);

        this->curVdoListRendIdx = 0;
        ui_iptv_pic_play_start();
    }

    return SID_NULL;
}

static void SexIptv_Resource(void)
{
    OS_PRINTF("@@@SexIptv_Resource\n");
    ctrl_set_sts(this->hVdoList, OBJ_STS_HIDE);
}

static void SenIptv_ResVdoReq(void)
{
    OS_PRINTF("@@@SenIptv_ResVdoReq() curResListIdx=%d, curCatListIdx=%d\n", this->curResListIdx, this->curCatListIdx);
    ui_release_vdolist();

    this->vdo_cnt = (u16)((this->curPageNum < this->total_page) ? this->vdo_page_size : (this->total_vdo - (this->curPageNum - 1) * this->vdo_page_size));
    ui_iptv_vdolist_set_count(this->hVdoList, this->vdo_cnt);
    ui_iptv_update_page_num(this->hPageNum, this->curPageNum, this->total_page);

    ctrl_paint_ctrl(this->hVdoList, TRUE);
    ctrl_paint_ctrl(this->hPageNum, TRUE);

    if (this->catList)
    {
        ui_iptv_get_video_list(this->resList[this->curResListIdx].res_id, 
                          this->catList[this->curCatListIdx].name, 
                          this->catList[this->curCatListIdx].key, 
                          this->curPageNum);
    }
}

static STATEID StaIptv_resource_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)para1;
    al_iptv_vdo_item_t *p_vdoList;
    u16 *p_name;
    u8  *p_url;
    u16 vdo_cnt;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@StaIptv_resource_on_newpagevdo_arrive\n");
    ui_release_vdolist();

    if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
    {
        vdo_cnt = MIN(p_data->vdo_cnt, this->vdo_page_size);
        p_vdoList = (al_iptv_vdo_item_t *)mtos_malloc(vdo_cnt * sizeof(al_iptv_vdo_item_t));
        MT_ASSERT(p_vdoList != NULL);
        memset(p_vdoList, 0, vdo_cnt * sizeof(al_iptv_vdo_item_t));

        for (i = 0; i < vdo_cnt; i++)
        {
            OS_PRINTF("i=%d, @@@vdo_id=%d, res_id=%d\n", i, p_data->vdoList[i].vdo_id, p_data->vdoList[i].res_id);
            p_vdoList[i].vdo_id = p_data->vdoList[i].vdo_id;
            p_vdoList[i].res_id = p_data->vdoList[i].res_id;
            p_vdoList[i].b_single_page = p_data->vdoList[i].b_single_page;

            if (p_data->vdoList[i].name != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].name);
                p_name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_name != NULL);
                uni_strcpy(p_name, p_data->vdoList[i].name);
                p_vdoList[i].name = p_name;
            }

            if (p_data->vdoList[i].img_url != NULL)
            {
                str_len = strlen(p_data->vdoList[i].img_url);
                p_url = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_url != NULL);
                strcpy(p_url, p_data->vdoList[i].img_url);
                p_vdoList[i].img_url = p_url;
            }
        }
        this->vdoList = p_vdoList;
        if (this->curVdoListIdx >= vdo_cnt)
        {
            this->curVdoListIdx = vdo_cnt - 1;
        }
    }
    else
    {
        vdo_cnt = 0;
        this->vdoList = NULL;
        this->curVdoListIdx = 0;
    }

    if (this->vdo_cnt != vdo_cnt)
    {
        OS_PRINTF("@@@calced vdo_cnt=%d, gived vdo_cnt=%d\n", this->vdo_cnt, vdo_cnt);
        this->vdo_cnt = vdo_cnt;
        ui_iptv_vdolist_set_count(this->hVdoList, this->vdo_cnt);
    }
    if (this->total_vdo != p_data->total_vdo)
    {
        OS_PRINTF("@@@this->total_vdo=%d, p_data->total_vdo=%d\n", this->total_vdo, p_data->total_vdo);
        this->total_vdo = this->resList[this->curResListIdx].catList[this->curCatListIdx].total_vdo = p_data->total_vdo;
    }
    if (p_data->total_page == 0)
    {
        p_data->total_page = 1;
    }
    if (this->total_page != p_data->total_page)
    {
        OS_PRINTF("@@@this->total_page=%d, p_data->total_page=%d\n", this->total_page, p_data->total_page);
        this->total_page = p_data->total_page;
        this->b_totalPageChanged = TRUE;
        ui_iptv_update_page_num(this->hPageNum, this->curPageNum, this->total_page);
        ctrl_paint_ctrl(this->hPageNum, TRUE);
    }

    ui_iptv_update_curVdoPage();
    ctrl_paint_ctrl(this->hVdoList, TRUE);
    return SID_IPTV_RES_VDOLIST_NORMAL;
}

static STATEID StaIptv_resource_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_resource_on_get_pagevdo_fail\n");
    ui_iptv_open_cfm_dlg(ROOT_ID_IPTV, IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaIptv_resource_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_resource_on_vdoreq_dlg_closed\n");
    return SID_IPTV_RES_VDOLIST_NORMAL;
}

static void SexIptv_ResVdoReq(void)
{
    OS_PRINTF("@@@SexIptv_ResVdoReq\n");
    ui_comm_dlg_close();
}

static void SenIptv_ResVdolistNormal(void)
{
    OS_PRINTF("@@@SenIptv_ResVdolistNormal\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_IPTV)
    {
        this->curVdoListRendIdx = 0;
        ui_iptv_pic_play_start();
    }
}

static STATEID StaIptv_resource_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_item = NULL, *p_pic = NULL;

    OS_PRINTF("@@@StaIptv_resource_pic_draw_end\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_IPTV)
    {
        p_item = ctrl_get_child_by_id(this->hVdoList, IDC_IPTV_VDOITEM_START + this->curVdoListRendIdx);
        p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);
        bmap_set_content_by_id(p_pic, RSC_INVALID_ID);

        ctrl_paint_ctrl(p_pic, TRUE);

        pic_stop();
        this->curVdoListRendIdx++;
        ui_iptv_pic_play_start();
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptv_resource_pic_draw_fail\n");
    pic_stop();
    this->curVdoListRendIdx++;
    ui_iptv_pic_play_start();
    return SID_NULL;
}

static STATEID StaIptv_resource_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_iptv_description_param_t p_param;

    OS_PRINTF("@@@StaIptv_resource_on_vdolist_selected curVdoListIdx=%d\n", this->curVdoListIdx);
    if (this->vdoList)
    {
        ui_iptv_pic_deinit();

        p_param.vdo_id = this->vdoList[this->curVdoListIdx].vdo_id;
        p_param.res_id = this->vdoList[this->curVdoListIdx].res_id;
        p_param.b_single_page = this->vdoList[this->curVdoListIdx].b_single_page;
        OS_PRINTF("@@@vdo_id=%d, res_id=%d, b_single_page=%d\n", p_param.vdo_id, p_param.res_id, p_param.b_single_page);
        manage_open_menu(ROOT_ID_IPTV_DESCRIPTION, (u32)&p_param, (u32)ROOT_ID_IPTV);
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ctrl_is_onfocus(this->hVdoList))
    {
        this->jumpPageNum = para1;
        if (this->jumpPageNum > this->total_page)
        {
            this->jumpPageNum = this->total_page;
        }
        ui_iptv_update_page_num(this->hPageNum, 
                                                            this->jumpPageNum, 
                                                            this->total_page);
        ctrl_change_focus(this->hVdoList, this->hPageNum);
    }
    else
    {
        if (this->jumpPageNum < this->total_page)
        {
            this->jumpPageNum = this->jumpPageNum * 10 + para1;
        }
        else
        {
            this->jumpPageNum = para1;
        }
        if (this->jumpPageNum > this->total_page)
        {
            this->jumpPageNum = this->total_page;
        }
        ui_iptv_update_page_num(this->hPageNum, 
                                                            this->jumpPageNum, 
                                                            this->total_page);
        ctrl_paint_ctrl(this->hPageNum, TRUE);
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(this->hPageNum, this->hVdoList);

    if (this->jumpPageNum == 0)
    {
       this->jumpPageNum = 1;
       ui_iptv_update_page_num(this->hPageNum, 
                                                   this->jumpPageNum, 
                                                   this->total_page);
       ctrl_paint_ctrl(this->hPageNum, TRUE);
    }
    this->curPageNum = this->jumpPageNum;

    return SID_NULL;
}

static STATEID StaIptv_resource_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (this->curPageNum != this->jumpPageNum)
    {
        ui_iptv_update_page_num(this->hPageNum, 
                                                            this->curPageNum, 
                                                            this->total_page);
    }

    ctrl_change_focus(this->hPageNum, this->hVdoList);

    return SID_NULL;
}

static void SexIptv_ResVdolistNormal(void)
{
    OS_PRINTF("@@@SexIptv_ResVdolistNormal\n");
    pic_stop();
}

static void SenIptv_InitFailed(void)
{
    OS_PRINTF("@@@SenIptv_InitFailed\n");
    ui_iptv_open_cfm_dlg(ROOT_ID_IPTV, IDS_NETWORK_UNAVAILABLE);
}

static void SexIptv_InitFailed(void)
{
    OS_PRINTF("@@@SexIptv_InitFailed\n");
    ui_comm_dlg_close();
}

static void SenIptv_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_iptv_dp_deinit();
}

static STATEID StaIptv_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_IPTV);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    return SID_NULL;
}

static void SexIptv_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

/*================================================================================================
                           iptv public works function
 ================================================================================================*/

static RET_CODE ui_iptv_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    OS_PRINTF("@@@%s msg=%d\n", __FUNCTION__, msg);
    ret = Iptv_DispatchMsg(p_ctrl, msg, para1, para2);

    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@iptv unaccepted msg, id=0x%04x\n", msg);
    }

    return ret;
}

static RET_CODE ui_iptv_on_focus_key_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    Iptv_DispatchMsg(p_ctrl, MSG_FOCUS_KEY, msg, para2);

    return SUCCESS;
}

/*!
 * Video view entry
 */
RET_CODE ui_open_iptv(u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_catlist_cont, *p_title_name, *p_catlist;
    control_t *p_res_cont, *p_reslist;
    control_t *p_client_cont, *p_vdolist;
    control_t *p_page_cont, *p_page_num;
    u16 i = 0;

    OS_PRINTF("@@@ui_open_iptv\n");
    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
    
    ui_init_app_data();
    ui_iptv_register_msg();
    ui_iptv_pic_init();

    /*!
     * Create Menu
     */
    p_cont = ui_comm_root_create_netmenu(ROOT_ID_IPTV, 0,
                                    IM_INDEX_NETWORK_BANNER, IDS_VOD);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_keymap(p_cont, iptv_cont_keymap);
    ctrl_set_proc(p_cont, iptv_cont_proc);

    /*!
     * Create catlist container
     */
    p_catlist_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_CATLIST_CONT,
                                  IPTV_CATLIST_CONTX, IPTV_CATLIST_CONTY,
                                  IPTV_CATLIST_CONTW, IPTV_CATLIST_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_catlist_cont, RSI_BOX3, RSI_BOX3, RSI_BOX3);

    // title
    p_title_name = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_IPTV_TITLE_NAME,
                                 IPTV_TITLE_NAME_X, IPTV_TITLE_NAME_Y,
                                 IPTV_TITLE_NAME_W, IPTV_TITLE_NAME_H,
                                 p_catlist_cont, 0);
    ctrl_set_rstyle(p_title_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_title_name, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_title_name, FSI_WHITE_30, FSI_WHITE_30, FSI_WHITE_30);
    text_set_content_type(p_title_name, TEXT_STRTYPE_EXTSTR);
    ctrl_set_sts(p_title_name, OBJ_STS_HIDE);
    this->hTitleName = p_title_name;

    // catlist
    p_catlist = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_CATLIST,
                          IPTV_CATLIST_X, IPTV_CATLIST_Y,
                          IPTV_CATLIST_W, IPTV_CATLIST_H,
                          p_catlist_cont, 0);
    ctrl_set_rstyle(p_catlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_catlist, iptv_catlist_keymap);
    ctrl_set_proc(p_catlist, iptv_catlist_proc);

    ctrl_set_mrect(p_catlist,
                  IPTV_CATLIST_MIDL, IPTV_CATLIST_MIDT,
                  IPTV_CATLIST_MIDR, IPTV_CATLIST_MIDB);
    list_set_item_interval(p_catlist, IPTV_CATLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_catlist, &catlist_item_rstyle);

    list_enable_cycle_mode(p_catlist, FALSE);
    list_enable_select_mode(p_catlist, TRUE);
    list_set_select_mode(p_catlist, LIST_SINGLE_SELECT);

    list_set_count(p_catlist, 0, IPTV_CATLIST_PAGE_SIZE);//should not be removed
    list_set_field_count(p_catlist, ARRAY_SIZE(catlist_attr), IPTV_CATLIST_PAGE_SIZE);
    list_set_update(p_catlist, ui_iptv_update_catlist, 0);

    for (i = 0; i < ARRAY_SIZE(catlist_attr); i++)
    {
        list_set_field_attr(p_catlist, (u8)i, catlist_attr[i].attr,
                            catlist_attr[i].width, catlist_attr[i].left, catlist_attr[i].top);
        list_set_field_rect_style(p_catlist, (u8)i, catlist_attr[i].rstyle);
        list_set_field_font_style(p_catlist, (u8)i, catlist_attr[i].fstyle);
    }
    ctrl_set_sts(p_catlist, OBJ_STS_HIDE);
    this->hCatList = p_catlist;

#if 0
    list_set_count(p_catlist, IPTV_LIST_ITEM_COUNT, IPTV_CATLIST_ITEM_PAGE_SIZE);
    ////list_set_focus_pos(p_catlist, this->curCatListIdx);
    ////list_select_item(p_catlist, this->curCatListIdx);

    iptv_update_catlist(p_catlist, list_get_valid_pos(p_catlist), IPTV_LIST_ITEM_NUM_ONE_PAGE, 0);
#endif

    /*!
     * Create res container
     */
    p_res_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_RESLIST_CONT,
                                  IPTV_RESLIST_CONTX, IPTV_RESLIST_CONTY,
                                  IPTV_RESLIST_CONTW, IPTV_RESLIST_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_res_cont, RSI_BOX3, RSI_BOX3, RSI_BOX3);

    // Reslist
    p_reslist = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_RESLIST,
                              IPTV_RESLIST_X, IPTV_RESLIST_Y,
                              IPTV_RESLIST_W, IPTV_RESLIST_H, 
                              p_res_cont, 0);
    ctrl_set_rstyle(p_reslist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_reslist, iptv_reslist_keymap);
    ctrl_set_proc(p_reslist, iptv_reslist_proc);

    ctrl_set_mrect(p_reslist,
                  IPTV_RESLIST_MIDL, IPTV_RESLIST_MIDT,
                  IPTV_RESLIST_MIDR, IPTV_RESLIST_MIDB);
    //list_set_item_interval(p_reslist, IPTV_RESLIST_ITEM_H_GAP);
    list_set_item_rstyle(p_reslist, &reslist_item_rstyle);

    list_enable_cycle_mode(p_reslist, FALSE);
    list_enable_select_mode(p_reslist, TRUE);
    list_set_select_mode(p_reslist, LIST_SINGLE_SELECT);

    list_set_count(p_reslist, 0, IPTV_RESLIST_PAGE_SIZE);//should not be removed
    list_set_field_count(p_reslist, ARRAY_SIZE(reslist_attr), IPTV_RESLIST_PAGE_SIZE);
    list_set_columns(p_reslist, IPTV_RESLIST_PAGE_SIZE, TRUE);
    list_set_update(p_reslist, ui_iptv_update_reslist, 0);

    for (i = 0; i < ARRAY_SIZE(reslist_attr); i++)
    {
        list_set_field_attr(p_reslist, (u8)i, reslist_attr[i].attr,
                            reslist_attr[i].width, reslist_attr[i].left, reslist_attr[i].top);
        list_set_field_rect_style(p_reslist, (u8)i, reslist_attr[i].rstyle);
        list_set_field_font_style(p_reslist, (u8)i, reslist_attr[i].fstyle);
    }
    ctrl_set_sts(p_reslist, OBJ_STS_HIDE);
    this->hResList = p_reslist;

    /*!
     * Create client container
     */
    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_CLIENT_CONT,
                                  IPTV_CLIENT_CONTX, IPTV_CLIENT_CONTY,
                                  IPTV_CLIENT_CONTW, IPTV_CLIENT_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //Video list
    p_vdolist = ui_iptv_create_vdolist(p_client_cont);
    ctrl_set_keymap(p_vdolist, iptv_vdolist_keymap);
    ctrl_set_proc(p_vdolist, iptv_vdolist_proc);
    ctrl_set_sts(p_vdolist, OBJ_STS_HIDE);
    this->hVdoList = p_vdolist;

    /*!
     * Create page container
     */
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PAGE_CONT,
                                    IPTV_PAGE_CONTX, IPTV_PAGE_CONTY,
                                    IPTV_PAGE_CONTW, IPTV_PAGE_CONTH,
                                    p_cont, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX3, RSI_BOX3, RSI_BOX3);

    p_page_num = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_IPTV_PAGE_NUM,
                                 IPTV_PAGE_NUMBER_X, IPTV_PAGE_NUMBER_Y,
                                 IPTV_PAGE_NUMBER_W, IPTV_PAGE_NUMBER_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_page_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_page_num, iptv_page_num_keymap);
    ctrl_set_proc(p_page_num, iptv_page_num_proc);

    text_set_align_type(p_page_num, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_page_num, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_page_num, TEXT_STRTYPE_UNICODE);
    this->hPageNum = p_page_num;

    ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

///////////////////////////////////////////////////////////////////
    Iptv_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_IPTV), NOTIFY_T_MSG, FALSE, MSG_OPEN_IPTV_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_iptv_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_iptv_on_destory\n");

    Iptv_CloseStateTree();

    ui_iptv_pic_deinit();
    ui_iptv_unregister_msg();
    ui_release_app_data();

    ui_video_c_destroy();
    
    ui_stop_play(STOP_PLAY_BLACK, TRUE);
    ui_play_curn_pg();
    return ERR_NOFEATURE;
}

static RET_CODE ui_iptv_on_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_iptv_on_open_dlg\n");

    if (Iptv_IsStateActive(SID_IPTV_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_iptv_on_open_cfm_dlg\n");

    if (Iptv_IsStateActive(SID_IPTV_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);

        p_root = fw_find_root_by_id(ROOT_ID_IPTV);
        if (p_root)
        {
            fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
        }
    }

    return SUCCESS;
}

void ui_iptv_open_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_DLG_REQ, str_id, 0);
    }
}

void ui_iptv_open_cfm_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

static RET_CODE ui_dispatch_iptv_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    switch (msg)
    {
        case MSG_IPTV_EVT_INIT_SUCCESS:
        case MSG_IPTV_EVT_INIT_FAIL:
        case MSG_IPTV_EVT_DEINIT_SUCCESS:
        case MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE:
        case MSG_IPTV_EVT_GET_RES_NAME_FAIL:
        case MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE:
        case MSG_IPTV_EVT_GET_RES_CATGRY_FAIL:
            ui_iptv_on_state_process(p_ctrl, msg, para1, para2);
            break;

        case MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE:
        case MSG_IPTV_EVT_GET_PAGE_VDO_FAIL:
            if (ui_iptv_vdo_idntfy_cmp(para2) == 0)
            {
                ui_iptv_on_state_process(p_ctrl, msg, para1, para2);
            }
            break;

        case MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE:
        case MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL:
            if (ui_iptv_vdo_idntfy_cmp(para2) == 0)
            {
                p_root = fw_find_root_by_id(ROOT_ID_IPTV_SEARCH);
            }
            break;

        case MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE:
        case MSG_IPTV_EVT_GET_VDO_INFO_FAIL:
        case MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE:
        case MSG_IPTV_EVT_GET_INFO_URL_FAIL:
        case MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE:
        case MSG_IPTV_EVT_GET_RECMND_INFO_FAIL:
            p_root = fw_find_root_by_id(ROOT_ID_IPTV_DESCRIPTION);
            break;

        case MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE:
        case MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL:
        case MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE:
        case MSG_IPTV_EVT_GET_PLAY_URL_FAIL:
            p_root = fw_find_root_by_id(ROOT_ID_IPTV_PLAYER);
            break;

        default:
            break;
    }

    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, msg, para1, para2);
    }

    ui_iptv_free_msg_data(msg, para1, para2);
    
    return SUCCESS;
}

static RET_CODE ui_iptv_on_key_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s msg=%d\n", __FUNCTION__, msg);
    Iptv_DispatchMsg(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

static RET_CODE ui_iptv_on_input_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)	
{
    OS_PRINTF("@@@%s msg=%d\n", __FUNCTION__, msg);
    para1 = (u32)(msg & MSG_DATA_MASK);
    msg = (msg & MSG_TYPE_MASK);
    Iptv_DispatchMsg(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(iptv_reslist_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_reslist_keymap, NULL)

BEGIN_MSGPROC(iptv_reslist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_state_process)
END_MSGPROC(iptv_reslist_proc, list_class_proc)

BEGIN_KEYMAP(iptv_catlist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_catlist_keymap, NULL)

BEGIN_MSGPROC(iptv_catlist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_state_process)
END_MSGPROC(iptv_catlist_proc, list_class_proc)

BEGIN_KEYMAP(iptv_vdolist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(iptv_vdolist_keymap, NULL)

BEGIN_MSGPROC(iptv_vdolist_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptv_on_focus_key_process)
    ON_COMMAND(MSG_PAGE_UP, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, ui_iptv_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_NUMBER, ui_iptv_on_input_number)
END_MSGPROC(iptv_vdolist_proc, cont_class_proc)

BEGIN_KEYMAP(iptv_page_num_keymap, NULL)
    ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_UP, MSG_EXIT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(iptv_page_num_keymap, NULL)

BEGIN_MSGPROC(iptv_page_num_proc, text_class_proc)
    ON_COMMAND(MSG_NUMBER, ui_iptv_on_input_number)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_iptv_on_key_process)
END_MSGPROC(iptv_page_num_proc, text_class_proc)

BEGIN_KEYMAP(iptv_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(iptv_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(iptv_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, ui_iptv_on_state_process)


    ON_COMMAND(MSG_IPTV_EVT_INIT_SUCCESS, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_INIT_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_DEINIT_SUCCESS, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RES_NAME_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RES_CATGRY_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_PAGE_VDO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_VDO_INFO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_INFO_URL_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_PLAY_URL_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RECMND_INFO_FAIL, ui_dispatch_iptv_msg)

    ON_COMMAND(MSG_OPEN_IPTV_REQ, ui_iptv_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_iptv_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_INFO, ui_iptv_on_state_process)
    ON_COMMAND(MSG_DESTROY, ui_iptv_on_destory)

    ON_COMMAND(MSG_OPEN_DLG_REQ, ui_iptv_on_open_dlg)
    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_iptv_on_open_cfm_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_iptv_on_state_process)
    ON_COMMAND(MSG_UPDATE, ui_iptv_on_state_process)
END_MSGPROC(iptv_cont_proc, ui_comm_root_proc)
#endif


