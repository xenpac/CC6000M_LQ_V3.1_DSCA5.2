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

//#include "commonData.h"
#include "NetMediaDataProvider.h"
#include "al_netmedia.h"
#include "ui_utils.h"

#include "ui_nm_gprot.h"
#include "ui_nm_prot.h"
#include "ui_nm_simple.h"

#include "SM_StateMachine.h"
#include "states_simple.h"
#include "ui_video.h"
#include "ui_picture_api.h"
#include "ui_video_player_gprot.h"
#include "ui_keyboard_v2.h"


/*!
 * Function define
 */
u16 nm_simple_class_list_keymap(u16 key);
RET_CODE nm_simple_class_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_page_num_keymap(u16 key);
RET_CODE nm_simple_page_num_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_vdo_list_keymap(u16 key);
RET_CODE nm_simple_vdo_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_subclass_list_keymap(u16 key);
RET_CODE nm_simple_subclass_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_opt_list_keymap(u16 key);
RET_CODE nm_simple_opt_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_info_detail_keymap(u16 key);
RET_CODE nm_simple_info_detail_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_simple_cont_keymap(u16 key);
RET_CODE nm_simple_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

//class list ext data
nm_class_list_ext_item class_list_ext[1] = 
{
    {
        IDS_RESOLUTION
    }
};

nm_opt_list_item g_resolution_opt[2] = 
{
    {NETMEDIA_VIDEO_SD, IDS_NORMAL_RESOLUTION},
    {NETMEDIA_VIDEO_HD, IDS_HIGH_RESOLUTION}
};

static ui_nm_dp_type_rsc_item g_dp_type_rsc[] = 
{
    {NETMEDIA_DP_YOUTUBE, IM_PIC_YOUTUBE_LOGO, IDS_YOUTUBE},
    {NETMEDIA_DP_YOUPORN, IM_PIC_YUPOO_LOGO, IDS_YOUPORN},
    {NETMEDIA_DP_DAILYMOTION, IM_PIC_DAILYMOTION_LOGO, IDS_DAILYMOTION},
    {NETMEDIA_DP_ALJAZEER, IM_PIC_ALJA_LOGO, IDS_ALJAZEERA},
    {NETMEDIA_DP_VEOH, IM_PIC_VEOH_LOGO, IDS_VEOH},
    {NETMEDIA_DP_YAHOO_VIDEO, IM_PIC_YAHOO_LOGO, IDS_YAHOO_VIDEO},
    {NETMEDIA_DP_ADDANIME_VIDEO, IM_PIC_ADD_ANIME_LOGO, IDS_ADDANIME_VIDEO},
    {NETMEDIA_DP_KONTRTUBE, IM_PIC_KONTRTUBE_LOGO, IDS_KONTRTUBE},
    {NETMEDIA_DP_SPORTS_VIDEO, IM_PIC_ESPN_LOGO, IDS_SPORTS_VIDEO},
    {NETMEDIA_DP_ARABIC_FILM, IM_PIC_ARABICFILM_LOGO, IDS_ARABIC_FILM},
    {NETMEDIA_DP_MBCSHAHID, IM_PIC_SHAHID_LOGO, IDS_SHAHID},
	{NETMEDIA_DP_DUBAI_MEDIA, IM_PIC_DUBAI_LOGO, IDS_DUBAI_MEDIA},
};

/*!
 * Main list style
 */
static list_xstyle_t class_list_item_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_2_HL,
    RSI_ITEM_2_SH,
    RSI_ITEM_2_HL,
};

static list_xstyle_t class_list_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t class_list_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_BLACK,
    FSI_BLACK,
    FSI_BLACK,
};
static const list_field_attr_t class_list_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
        170 + 30,//162,
        0,
        0,
        &class_list_field_rstyle,
        &class_list_field_fstyle
    }
};

/*!
 * Video list style
 */
static list_xstyle_t vdo_list_item_rstyle =
{
    RSI_PBACK,
    RSI_MAIN_BG,//RSI_ITEM_2_SH,
    RSI_ITEM_13_HL,//RSI_ITEM_2_HL,
    RSI_MAIN_BG,//RSI_ITEM_2_SH,
    RSI_ITEM_13_HL//RSI_ITEM_2_HL,
};

static list_xstyle_t vdo_list_icon_rstyle =
{
    RSI_PBACK,
    RSI_TRANSPARENT,
    RSI_TRANSPARENT,
    RSI_TRANSPARENT,
    RSI_TRANSPARENT,
};
static list_xstyle_t vdo_list_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t vdo_list_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
};
static list_xstyle_t vdo_list_field_fstyle_small =
{
    FSI_GRAY,
    FSI_WHITE_20,
    FSI_WHITE_20,
    FSI_WHITE_20,
    FSI_WHITE_20,
};

static const list_field_attr_ext_t vdo_list_attr[] =
{
    //VDOLIST_FIELD_ID_LOGO
    {LISTFIELD_TYPE_ICON,
     168, 105 - 12 - 1,  6, 6, &vdo_list_icon_rstyle, &vdo_list_field_fstyle},
    //VDOLIST_FIELD_ID_TITLE
    {LISTFIELD_TYPE_EXTSTR | STL_LEFT | STL_TOP,
     530, 58, 174, 12, &vdo_list_field_rstyle, &vdo_list_field_fstyle},
    //VDOLIST_FIELD_ID_AUTHOR
    {LISTFIELD_TYPE_EXTSTR | STL_LEFT | STL_TOP,
     450, 29, 180, 70, &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    //VDOLIST_FIELD_ID_DURATION
    {LISTFIELD_TYPE_EXTSTR | STL_RIGHT | STL_TOP,
     5*NM_SIMPLE_RATED_ICON_W, 29, 
     NM_SIMPLE_VDO_LIST_W - 5*NM_SIMPLE_RATED_ICON_W - 10, 12, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_START
    {LISTFIELD_TYPE_ICON,
     NM_SIMPLE_RATED_ICON_W, NM_SIMPLE_RATED_ICON_H,  
     NM_SIMPLE_VDO_LIST_W - 5*NM_SIMPLE_RATED_ICON_W - 10, NM_SIMPLE_RATED_ICON_Y, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     NM_SIMPLE_RATED_ICON_W, NM_SIMPLE_RATED_ICON_H,  
     NM_SIMPLE_VDO_LIST_W - 4*NM_SIMPLE_RATED_ICON_W - 10, NM_SIMPLE_RATED_ICON_Y, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     NM_SIMPLE_RATED_ICON_W, NM_SIMPLE_RATED_ICON_H,  
     NM_SIMPLE_VDO_LIST_W - 3*NM_SIMPLE_RATED_ICON_W - 10, NM_SIMPLE_RATED_ICON_Y, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    {LISTFIELD_TYPE_ICON,
     NM_SIMPLE_RATED_ICON_W, NM_SIMPLE_RATED_ICON_H,  
     NM_SIMPLE_VDO_LIST_W - 2*NM_SIMPLE_RATED_ICON_W - 10, NM_SIMPLE_RATED_ICON_Y, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    //VDOLIST_FIELD_ID_RATED_END
    {LISTFIELD_TYPE_ICON,
     NM_SIMPLE_RATED_ICON_W, NM_SIMPLE_RATED_ICON_H,  
     NM_SIMPLE_VDO_LIST_W - NM_SIMPLE_RATED_ICON_W - 10, NM_SIMPLE_RATED_ICON_Y, 
     &vdo_list_field_rstyle, &vdo_list_field_fstyle_small},
    //VDOLIST_FIELD_ID_VIEW_COUNT
    {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
     190, 29, NM_SIMPLE_VDO_LIST_W - 200, 70, &vdo_list_field_rstyle, &vdo_list_field_fstyle_small}
};

static const list_field_attr_t subclass_list_attr[] =
{
    {LISTFIELD_TYPE_UNISTR | STL_LEFT,
     600, 10, 0, &vdo_list_field_rstyle, &vdo_list_field_fstyle}
};

static const list_field_attr_t opt_list_attr[] =
{
    {LISTFIELD_TYPE_STRID | STL_LEFT,
     600, 10, 0, &vdo_list_field_rstyle, &vdo_list_field_fstyle},
    {LISTFIELD_TYPE_ICON,
     28, NM_SIMPLE_VDO_LIST_W - 100, 0, &vdo_list_field_rstyle, &vdo_list_field_fstyle}
};

static comm_help_data_t search_help_data =
{
    1, 1,
    {IDS_SEARCH},
    {IM_HELP_RED}
};

static comm_help_data_t emp_help_data =
{
    1, 1,
    {RSC_INVALID_ID},
    {RSC_INVALID_ID}
};

static comm_help_data_t vdo_help_data =
{
    4, 4,
    {IDS_GOTO, IDS_PAGE, IDS_SELECT, IDS_RETURN},
    {IM_HELP_SHORTCUT, IM_HELP_PAGE, IM_HELP_OK, IM_HELP_MENU}
};

static comm_help_data_t comm_help_data =
{
    3, 3,
    {IDS_PAGE, IDS_SELECT, IDS_RETURN},
    {IM_HELP_PAGE, IM_HELP_OK, IM_HELP_MENU}
};

// App data
static ui_nm_simple_app_t *g_pNmSimple = NULL;

static void ui_release_nm_simple_data(void);
static void ui_release_classlist(void);
static void ui_release_subclasslist(void);
static void ui_clean_historylist(void);
static void ui_release_vdolist(void);
void ui_nm_simple_open_cfm_dlg(u8 root_id, u16 str_id);
void ui_nm_simple_open_dlg(u8 root_id, u16 str_id);

/*================================================================================================
                           nm simple internel function
 ================================================================================================*/

static BOOL ui_init_nm_simple_data(void)
{
    if (g_pNmSimple)
    {
        ui_release_nm_simple_data();
    }

    g_pNmSimple = (ui_nm_simple_app_t *)mtos_malloc(sizeof(ui_nm_simple_app_t));
    MT_ASSERT(g_pNmSimple != NULL);
    memset((void *)g_pNmSimple, 0, sizeof(ui_nm_simple_app_t));

    ui_link_list_create(&g_pNmSimple->hHisList, 0);

    return TRUE;
}

static void ui_release_nm_simple_data(void)
{
    if (g_pNmSimple)
    {
        ui_release_classlist();

        ui_release_subclasslist();
        ui_clean_historylist();
        ui_link_list_destory(&g_pNmSimple->hHisList);

        ui_release_vdolist();

        mtos_free(g_pNmSimple);
        g_pNmSimple = NULL;
    }
}

static void ui_release_classlist(void)
{
    u16 i;

    if (g_pNmSimple->classList)
    {
        for (i = 0; i < g_pNmSimple->total_class; i++)
        {
            if (g_pNmSimple->classList[i].name)
            {
                mtos_free(g_pNmSimple->classList[i].name);
                g_pNmSimple->classList[i].name = NULL;
            }
            if (g_pNmSimple->classList[i].key)
            {
                mtos_free(g_pNmSimple->classList[i].key);
                g_pNmSimple->classList[i].key = NULL;
            }
        }
        mtos_free(g_pNmSimple->classList);
        g_pNmSimple->classList = NULL;
    }
}

static void ui_release_subclasslist(void)
{
    u16 i;

    if (g_pNmSimple->subclaList != NULL)
    {
        for (i = 0; i < g_pNmSimple->total_subclass; i++)
        {
            if (g_pNmSimple->subclaList[i].name)
            {
                mtos_free(g_pNmSimple->subclaList[i].name);
                g_pNmSimple->subclaList[i].name = NULL;
            }
            if (g_pNmSimple->subclaList[i].key)
            {
                mtos_free(g_pNmSimple->subclaList[i].key);
                g_pNmSimple->subclaList[i].key = NULL;
            }
        }
        mtos_free(g_pNmSimple->subclaList);
        g_pNmSimple->subclaList = NULL;
    }
}

static void ui_clean_historylist(void)
{
    ui_subclass_context *p_context;
    u16 i;

    while (ui_link_list_delete(&g_pNmSimple->hHisList, 1, (void **)&p_context) == SUCCESS)
    {
        if (p_context != NULL)
        {
            if (p_context->subclaList != NULL)
            {
                for (i = 0; i < p_context->total_subclass; i++)
                {
                    if (p_context->subclaList[i].name)
                    {
                        mtos_free(p_context->subclaList[i].name);
                        p_context->subclaList[i].name = NULL;
                    }
                    if (p_context->subclaList[i].key)
                    {
                        mtos_free(p_context->subclaList[i].key);
                        p_context->subclaList[i].key = NULL;
                    }
                }
                mtos_free(p_context->subclaList);
                p_context->subclaList = NULL;
            }
            mtos_free(p_context);
        }
    }
}

static void ui_release_vdolist(void)
{
    u16 i;

    if (g_pNmSimple->vdoList)
    {
        for (i = 0; i < g_pNmSimple->vdo_cnt; i++)
        {
            if (g_pNmSimple->vdoList[i].id)
            {
              mtos_free(g_pNmSimple->vdoList[i].id);
              g_pNmSimple->vdoList[i].id = NULL;
            }
            if (g_pNmSimple->vdoList[i].title)
            {
                mtos_free(g_pNmSimple->vdoList[i].title);
                g_pNmSimple->vdoList[i].title = NULL;
            }
            if (g_pNmSimple->vdoList[i].key)
            {
                mtos_free(g_pNmSimple->vdoList[i].key);
                g_pNmSimple->vdoList[i].key = NULL;
            }
            if (g_pNmSimple->vdoList[i].img_url)
            {
                mtos_free(g_pNmSimple->vdoList[i].img_url);
            }
            if (g_pNmSimple->vdoList[i].date)
            {
                mtos_free(g_pNmSimple->vdoList[i].date);
                g_pNmSimple->vdoList[i].date = NULL;
            }
            if (g_pNmSimple->vdoList[i].duration)
            {
                mtos_free(g_pNmSimple->vdoList[i].duration);
                g_pNmSimple->vdoList[i].duration = NULL;
            }
            if (g_pNmSimple->vdoList[i].description)
            {
                mtos_free(g_pNmSimple->vdoList[i].description);
                g_pNmSimple->vdoList[i].description = NULL;
            }
         }
        mtos_free(g_pNmSimple->vdoList);
        g_pNmSimple->vdoList = NULL;
    }
}

RET_CODE ui_nm_simple_edit_cb(u16* p_unistr)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_NM_SIMPLE);

    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_INPUT_COMP, 0, 0);
    }
	  return SUCCESS;
}
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
  
    *p_w = ((p_ctrl->mrect.right - p_ctrl->mrect.left - NM_SIMPLE_CLASS_LIST_ICON_VGAP *
       (columns - 1)) / columns);
  
    *p_h = (p_ctrl->mrect.bottom - p_ctrl->mrect.top - NM_SIMPLE_CLASS_LIST_ICON_VGAP * (lines - 1)) / lines;
  
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
          (NM_SIMPLE_CLASS_LIST_ICON_VGAP + h));
    
        left =  w * ((index - current_pos) / lines) + p_ctrl->mrect.left;
    }
  
    set_rect(p_item_rect, left, top, (left + w), (s16)(top + h));
  
}

static RET_CODE ui_nm_simple_pic_play_start()
{
    rect_t rect;
    rect_t item_rect;

    if (g_pNmSimple->vdoList)
    {
        while (g_pNmSimple->curVdoListRendIdx < g_pNmSimple->vdo_cnt)
        {
            if(g_pNmSimple->vdoList[g_pNmSimple->curVdoListRendIdx].img_url)
            {
              if (strlen(g_pNmSimple->vdoList[g_pNmSimple->curVdoListRendIdx].img_url) > 0)
              {
  
                  ctrl_get_frame(g_pNmSimple->hVdoList, &rect);
                  //OS_PRINTF("@@@ctrl_get_frame=(%d, %d), (%d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
                  ctrl_client2screen(g_pNmSimple->hVdoList, &rect);
                  //OS_PRINTF("@@@ctrl_client2screen=(%d, %d), (%d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
                  _list_get_item_rect(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListRendIdx, &item_rect);
                  //OS_PRINTF("@@@_list_get_item_rect=(%d, %d), (%d, %d)\n", item_rect.left, item_rect.top, item_rect.right, item_rect.bottom);
      
                  item_rect.left += rect.left;
                  item_rect.top += rect.top;
                  item_rect.right += rect.left;
                  item_rect.bottom += rect.top;
                  item_rect.left += vdo_list_attr[VDOLIST_FIELD_ID_LOGO].left;
                  item_rect.top += vdo_list_attr[VDOLIST_FIELD_ID_LOGO].top;
                  item_rect.right = item_rect.left + vdo_list_attr[VDOLIST_FIELD_ID_LOGO].width;
                  item_rect.bottom -= vdo_list_attr[VDOLIST_FIELD_ID_LOGO].top;
                  //OS_PRINTF("@@@pic_rect=(%d, %d), (%d, %d)\n", item_rect.left, item_rect.top, item_rect.right, item_rect.bottom);
      
                  ui_pic_play_by_url(g_pNmSimple->vdoList[g_pNmSimple->curVdoListRendIdx].img_url, &item_rect, 0);
                  return SUCCESS;
              }
             }
            g_pNmSimple->curVdoListRendIdx++;
        }
    }

    return ERR_NOFEATURE;
}

static RET_CODE ui_nm_simple_pic_clear(void)
{
    rect_t rect;

    ctrl_get_frame(g_pNmSimple->hVdoList, &rect);
    ctrl_client2screen(g_pNmSimple->hVdoList, &rect);
    rect.left += vdo_list_attr[VDOLIST_FIELD_ID_LOGO].left;
    rect.right = rect.left + vdo_list_attr[VDOLIST_FIELD_ID_LOGO].width;
    ui_pic_clear_rect(&rect, 0);

    return SUCCESS;
}

/*!
 * class list update function
 */
static RET_CODE nm_simple_update_class_list(control_t* p_class_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    u16 *p_unistr;
    cnt = list_get_count(p_class_list);
    OS_PRINTF("@@@%s,%d  cnt = [%d]\n",__FUNCTION__,__LINE__,cnt);

    for (i = start; i < (start + size) && i < g_pNmSimple->total_class; i++)
    {
        list_set_field_content_by_unistr(p_class_list, i, 0, g_pNmSimple->classList[i].name);
    }

    for (i = g_pNmSimple->total_class; i < (start + size) && i < cnt; i++)
    {
        p_unistr = (u16 *)gui_get_string_addr(class_list_ext[i - g_pNmSimple->total_class].name);
        list_set_field_content_by_unistr(p_class_list, i, 0, p_unistr);
    }

    return SUCCESS;
}

/*!
 * video list update function
 */
static RET_CODE nm_simple_update_vdo_list(control_t *p_vdo_list, u16 start, u16 size, u32 context)
{
    al_netmedia_vdo_item_t *p_vdo;
    u16 cnt, i;
    
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    cnt = g_pNmSimple->vdo_cnt;
    p_vdo = g_pNmSimple->vdoList;

    if (p_vdo != NULL)
    {
        for (i = start; i < start + size && i < cnt; i++)
        {
            list_set_field_content_by_icon(p_vdo_list, i, VDOLIST_FIELD_ID_LOGO, IM_INDEX_NETWORK_YOUTUBE_PIC);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_TITLE, p_vdo[i].title);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_AUTHOR, p_vdo[i].date);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_DURATION, p_vdo[i].duration);
        }
    }
    else
    {
        for (i = start; i < start + size && i < cnt; i++)
        {
            list_set_field_content_by_icon(p_vdo_list, i, VDOLIST_FIELD_ID_LOGO, IM_INDEX_NETWORK_YOUTUBE_PIC);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_TITLE, NULL);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_AUTHOR, NULL);
            list_set_field_content_by_extstr(p_vdo_list, i, VDOLIST_FIELD_ID_DURATION, NULL);
        }
    }

    return SUCCESS;
}

/*!
 * subclass list update function
 */
static RET_CODE nm_simple_update_subclass_list(control_t *p_subclass_list, u16 start, u16 size, u32 context)
{
    al_netmedia_class_item_t *p_subclaList;
    u16 i, cnt;

    if (g_pNmSimple->subclaList != NULL)
    {
        p_subclaList = g_pNmSimple->subclaList;
        cnt = g_pNmSimple->total_subclass;
        OS_PRINTF("@@@%s, cnt=%d\n", __FUNCTION__, cnt);

        for (i = start; i < start + size && i < cnt; i++)
        {
            list_set_field_content_by_unistr(p_subclass_list, i, 0, p_subclaList[i].name);
        }
    }

    return SUCCESS;
}

static RET_CODE nm_simple_update_opt_list(control_t *p_opt_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    nm_simple_config_t config;
    
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    cnt = list_get_count(g_pNmSimple->hOptList);
    sys_status_get_nm_simple_config_info(&config);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(g_pNmSimple->hOptList, i, 0, g_resolution_opt[i].name);
        list_set_field_content_by_icon(g_pNmSimple->hOptList, i, 1, 
                                (g_resolution_opt[i].id == config.resolution) ? IM_TV_SELECT : RSC_INVALID_ID);
    }

    return SUCCESS;
}

static void nm_simple_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[21 + 1];

    sprintf(num, "%ld / %ld", page_num, total_page);
    text_set_content_by_ascstr(p_ctrl, num);
}

static void nm_simple_update_vdo_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[21 + 1];

    if (IS_MASKED(g_pNmSimple->attr, NETMEDIA_UI_ATTR_FASTPAGE))
    {
        sprintf(num, "%ld / %ld", page_num, total_page);
        text_set_content_by_ascstr(p_ctrl, num);
    }
    else
    {
        sprintf(num, "%ld", page_num);
        text_set_content_by_ascstr(p_ctrl, num);
    }
}

static BOOL ui_load_new_page_subclass(al_netmedia_class_list_t *p_data)
{
    al_netmedia_class_item_t *p_subclaList;
    u16 i;
    u16 str_len;

    if (p_data->class_cnt > 0 && p_data->classList != NULL)
    {
        p_subclaList = (al_netmedia_class_item_t *)mtos_malloc(p_data->class_cnt * sizeof(al_netmedia_class_item_t));
        MT_ASSERT(p_subclaList != NULL);
        memset(p_subclaList, 0, p_data->class_cnt * sizeof(al_netmedia_class_item_t));

        for (i = 0; i < p_data->class_cnt; i++)
        {
            if (p_data->classList[i].name != NULL)
            {
                str_len = uni_strlen(p_data->classList[i].name);
                p_subclaList[i].name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_subclaList[i].name != NULL);
                uni_strcpy(p_subclaList[i].name, p_data->classList[i].name);
            }

            if (p_data->classList[i].key != NULL)
            {
                str_len = strlen(p_data->classList[i].key);
                p_subclaList[i].key = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_subclaList[i].key != NULL);
                strcpy(p_subclaList[i].key, p_data->classList[i].key);
                OS_PRINTF("@@@%s,%d,p_subclaList[i].key=%s\n",__FUNCTION__,__LINE__,p_subclaList[i].key);
            }

            p_subclaList[i].attr = p_data->classList[i].attr;
            p_subclaList[i].id = p_data->classList[i].id;
        }
        g_pNmSimple->subclaList = p_subclaList;
        g_pNmSimple->total_subclass = p_data->class_cnt;
        g_pNmSimple->curSubClaListIdx = 0;

        return TRUE;
    }

    return FALSE;
}

static BOOL ui_load_new_page_vdo(al_netmedia_vdo_list_t *p_data)
{
    al_netmedia_vdo_item_t *p_vdoList;
    u8 *p_id,*p_key,*p_url;
    u16 *p_title,*p_date,*p_duration,*p_description;
    u16 vdo_cnt;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@%s, vdo_cnt=%d, vdoList=0x%08x\n", __FUNCTION__, p_data->vdo_cnt, p_data->vdoList);
    if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
    {
        vdo_cnt = MIN(p_data->vdo_cnt, g_pNmSimple->vdo_page_size);
        p_vdoList = (al_netmedia_vdo_item_t *)mtos_malloc(vdo_cnt * sizeof(al_netmedia_vdo_item_t));
        MT_ASSERT(p_vdoList != NULL);
        memset(p_vdoList, 0, vdo_cnt * sizeof(al_netmedia_vdo_item_t));

        for (i = 0; i < vdo_cnt; i++)
        {
            OS_PRINTF("i=%d, @@@vdo_id=%s, img_url=%s\n", i, p_data->vdoList[i].id, p_data->vdoList[i].img_url);
            p_vdoList[i].score = p_data->vdoList[i].score;

            if (p_data->vdoList[i].id != NULL)
            {
                str_len = strlen(p_data->vdoList[i].id);
                p_id = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_id != NULL);
                strcpy(p_id, p_data->vdoList[i].id);
                p_vdoList[i].id = p_id;
            }

            if (p_data->vdoList[i].title != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].title);
                p_title = (u16 *)mtos_malloc((str_len + 1)* sizeof(u16));
                MT_ASSERT(p_title != NULL);
                uni_strcpy(p_title, p_data->vdoList[i].title);
                p_vdoList[i].title = p_title;
            }

            if (p_data->vdoList[i].key != NULL)
            {
                str_len = strlen(p_data->vdoList[i].key);
                p_key = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_key != NULL);
                strcpy(p_key, p_data->vdoList[i].key);
                p_vdoList[i].key = p_key;
                OS_PRINTF("p_vdoList[i].key = [%s]\n", p_vdoList[i].key);
            }

            if (p_data->vdoList[i].img_url != NULL)
            {
                str_len = strlen(p_data->vdoList[i].img_url);
                p_url = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_url != NULL);
                strcpy(p_url, p_data->vdoList[i].img_url);
                p_vdoList[i].img_url = p_url;
            }

            if (p_data->vdoList[i].date != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].date);
                p_date = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_date != NULL);
                uni_strcpy(p_date, p_data->vdoList[i].date);
                p_vdoList[i].date = p_date;
            }

            if (p_data->vdoList[i].duration != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].duration);
                p_duration = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_duration != NULL);
                uni_strcpy(p_duration, p_data->vdoList[i].duration);
                p_vdoList[i].duration = p_duration;
            }

            if (p_data->vdoList[i].description != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].description);
                p_description = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_description != NULL);
                uni_strcpy(p_description, p_data->vdoList[i].description);
                p_vdoList[i].description = p_description;
            }
        }
        g_pNmSimple->vdoList = p_vdoList;

        ///////////////////////////////////////////////////////////////////////
        if (g_pNmSimple->vdo_cnt != vdo_cnt)
        {
            OS_PRINTF("@@@calced vdo_cnt=%d, gived vdo_cnt=%d\n", g_pNmSimple->vdo_cnt, vdo_cnt);
            g_pNmSimple->vdo_cnt = vdo_cnt;
            list_set_count(g_pNmSimple->hVdoList, g_pNmSimple->vdo_cnt, g_pNmSimple->vdo_page_size);
        }
        if (g_pNmSimple->curVdoPageNum == 1)
        {
            OS_PRINTF("@@@curVdoPageNum=%d\n", g_pNmSimple->curVdoPageNum);
            list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
        }
        else if (g_pNmSimple->curVdoListIdx >= vdo_cnt)
        {
            OS_PRINTF("@@@curVdoListIdx=%d, vdo_cnt=%d\n", g_pNmSimple->curVdoListIdx, vdo_cnt);
            g_pNmSimple->curVdoListIdx = vdo_cnt - 1;
            list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
        }
        if (g_pNmSimple->total_vdo != p_data->total_vdo)
        {
            OS_PRINTF("@@@g_pNmSimple->total_vdo=%d, p_data->total_vdo=%d\n", g_pNmSimple->total_vdo, p_data->total_vdo);
            g_pNmSimple->total_vdo =  p_data->total_vdo;
            g_pNmSimple->totalVdoPage = (g_pNmSimple->total_vdo + g_pNmSimple->vdo_cnt - 1) / g_pNmSimple->vdo_cnt;
            nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curVdoPageNum, g_pNmSimple->totalVdoPage);
        }

        return TRUE;
    }

    return FALSE;
}

static void ui_nm_simple_pic_init(void)
{
    ui_pic_init(PIC_SOURCE_NET);
}

static void ui_nm_simple_pic_deinit(void)
{
    pic_stop();
  
    ui_pic_release();
}


/*================================================================================================
                           nm simple sm function
 ================================================================================================*/

static BOOL StcSimple_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_CLASS_LIST) ? TRUE : FALSE;
}

static BOOL StcSimple_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_PAGE_NUM) ? TRUE : FALSE;
}

static BOOL StcSimple_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_SUBCLASS_LIST) ? TRUE : FALSE;
}

static BOOL StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_VDO_LIST) ? TRUE : FALSE;
}

static BOOL StcSimple_is_on_info_detail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_INFO_DETAIL) ? TRUE : FALSE;
}

static BOOL StcSimple_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_SIMPLE_OPT_LIST) ? TRUE : FALSE;
}

static STATEID StaSimple_vdo_info_on_infodetail_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_NULL;
}

static STATEID StaSimple_vdo_info_on_infodetail_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_NULL;
} 

static STATEID StaSimple_resolution_on_optlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);
    OS_PRINTF("@@@%s\n", __FUNCTION__);

    switch(msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(ctrl, msg, para1, para2);
            break;

        case MSG_FOCUS_LEFT:
            ctrl_change_focus(g_pNmSimple->hOptList, g_pNmSimple->hClaList);
            break;
    }

    return SID_NULL;
}   

static STATEID StaSimple_resolution_on_optlist_select(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    nm_simple_config_t config;
    u16 i, cnt, focus;

    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    cnt = list_get_count(ctrl);
    sys_status_get_nm_simple_config_info(&config);

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
        list_draw_item_ext(ctrl, i, TRUE);
    }

    focus = list_get_focus_pos(ctrl);
    if (focus < cnt)
    {
        list_set_field_content_by_icon(ctrl, focus, 1, IM_TV_SELECT);
        list_draw_item_ext(ctrl, focus, TRUE);

        config.resolution = (u8)g_resolution_opt[focus].id;
        sys_status_set_nm_simple_config_info(&config);
    }
    return SID_NULL;
}     

static STATEID StaSimple_resolution_on_optlist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pNmSimple->hOptList, g_pNmSimple->hClaList);

    return SID_NULL;
}

static void SenSimple_Inactive(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static STATEID StaSimple_inactive_on_open_simple_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    return SID_SIMPLE_INIT;
}

static void SexSimple_Inactive(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenSimple_Active(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static STATEID StaSimple_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Simple_IsStateActive(SID_SIMPLE_DEINIT))
    {
        al_netmedia_website_deinit();
    }

    return SID_NULL;
}

static STATEID StaSimple_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Simple_IsStateActive(SID_SIMPLE_DEINIT))
    {
        return SID_SIMPLE_DEINIT;
    }
    else
    {
        return SID_NULL;
    }
}

static void SexSimple_Active(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

/* --------------------- Initialize:  ---------------------*/
static void SenSimple_Init(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_LOADING_WITH_WAIT);
    al_netmedia_website_init(g_pNmSimple->nm_type);
}


static STATEID StaSimple_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    al_netmedia_set_page_size(g_pNmSimple->vdo_page_size);
    al_netmedia_get_class_list(); 
    return SID_NULL;
}

static STATEID StaSimple_init_on_newclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_class_list_t *p_data = (al_netmedia_class_list_t *)para2;
    al_netmedia_class_item_t *p_classList;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    if (p_data->class_cnt > 0 && p_data->classList != NULL)
    {
        g_pNmSimple->total_class = p_data->class_cnt;
        p_classList = (al_netmedia_class_item_t *)mtos_malloc(p_data->class_cnt * sizeof(al_netmedia_class_item_t));
        MT_ASSERT(p_classList != NULL);
        memset((void *)p_classList, 0, p_data->class_cnt * sizeof(al_netmedia_class_item_t));

        for (i = 0; i < g_pNmSimple->total_class; i++)
        {
            if (p_data->classList[i].name != NULL)
            {
                str_len = uni_strlen(p_data->classList[i].name);
                p_classList[i].name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_classList[i].name != NULL);
                uni_strcpy(p_classList[i].name, p_data->classList[i].name);
            }

            if (p_data->classList[i].key != NULL)
            {
                str_len = strlen(p_data->classList[i].key);
                p_classList[i].key = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_classList[i].key != NULL);
                strcpy(p_classList[i].key, p_data->classList[i].key);
                OS_PRINTF("@@@%s,%dp_classList[%d].key==%s\n",__FUNCTION__,__LINE__,i,p_classList[i].key);
            }

            p_classList[i].attr = p_data->classList[i].attr;
            p_classList[i].id = p_data->classList[i].id;
  
        }
      
        g_pNmSimple->classList = p_classList;
    }
    else
    {
        OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
        return SID_SIMPLE_INIT_FAILED;
    }
  
    g_pNmSimple->curClassListIdx = 0;

    //update class list
    list_set_count(g_pNmSimple->hClaList, g_pNmSimple->total_class + g_pNmSimple->ext_class, NM_SIMPLE_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(g_pNmSimple->hClaList, g_pNmSimple->curClassListIdx);
    list_select_item(g_pNmSimple->hClaList, g_pNmSimple->curClassListIdx);

    nm_simple_update_class_list(g_pNmSimple->hClaList, list_get_valid_pos(g_pNmSimple->hClaList), NM_SIMPLE_CLASS_LIST_ITEM_NUM_ONE_PAGE, 0);

    ctrl_paint_ctrl(g_pNmSimple->hClaList, TRUE);
  
    if(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].attr == NM_SUB_ATTR_CLASS)
    {
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_CLASS\n",__FUNCTION__,__LINE__);
        return SID_SIMPLE_SUBCLASS_REQ;
    }
    else if(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].attr == NM_SUB_ATTR_ITEM)
    {
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_ITEM\n",__FUNCTION__,__LINE__);
        return SID_SIMPLE_VDO_REQ;
    }
    
    return SID_NULL;
}

static void SexSimple_Init(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_comm_dlg_close();
}


static void SenSimple_Resource()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}


static STATEID StaSimple_resource_on_classlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);
    OS_PRINTF("@@@%s\n", __FUNCTION__);

    switch (msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(g_pNmSimple->hClaList, msg, para1, para2);
            break;

        case MSG_FOCUS_RIGHT:
            if (ctrl_get_sts(g_pNmSimple->hVdoList) == OBJ_STS_SHOW)
            {
                if (list_get_count(g_pNmSimple->hVdoList) > 0)
                {
                    ctrl_change_focus(g_pNmSimple->hClaList, g_pNmSimple->hVdoList);
                }
            }
            else if (ctrl_get_sts(g_pNmSimple->hSubClaList) == OBJ_STS_SHOW)
            {
                if (list_get_count(g_pNmSimple->hSubClaList) > 0)
                {
                    ctrl_change_focus(g_pNmSimple->hClaList, g_pNmSimple->hSubClaList);
                }
            }
            else if (ctrl_get_sts(g_pNmSimple->hOptList) == OBJ_STS_SHOW)
            {
                ctrl_change_focus(g_pNmSimple->hClaList, g_pNmSimple->hOptList);
            }
            break;

        default:
            break;
    }

    return SID_NULL;
}

static STATEID StaSimple_resource_on_classlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    ui_release_subclasslist();
    g_pNmSimple->total_subclass = 0;
    g_pNmSimple->curSubClaListIdx = 0;
    ui_clean_historylist();

    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    OS_PRINTF("@@@StaSimple_resource_on_classlist_selected focus=%d\n", focus);

    g_pNmSimple->curClassListIdx= focus;
    ctrl_paint_ctrl(ctrl, TRUE);
    
    if (focus == (g_pNmSimple->total_class + 1))
    {
        return  SID_SIMPLE_SEARCH;
    }
    if (focus == g_pNmSimple->total_class)
    {
        return  SID_SIMPLE_RESOLUTION;
    }
    
    if (g_pNmSimple->classList[g_pNmSimple->curClassListIdx].attr == NM_SUB_ATTR_ITEM)
    {
        return SID_SIMPLE_VDO;
    }
    else if (g_pNmSimple->classList[g_pNmSimple->curClassListIdx].attr == NM_SUB_ATTR_CLASS)
    {
        return SID_SIMPLE_SUBCLASS;
    }

    return SID_NULL;
}

static void SexSimple_Resource()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenSimple_Subclass()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    //ctrl_set_attr(g_pNmSimple->hSubClaList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmSimple->hSubClaList, OBJ_STS_SHOW);

    ui_comm_help_set_data(&comm_help_data, g_pNmSimple->hHelpCont);
    ctrl_paint_ctrl(g_pNmSimple->hHelpCont, TRUE);
}

static void SexSimple_Subclass(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ctrl_set_sts(g_pNmSimple->hSubClaList, OBJ_STS_HIDE);
}

static void SenSimple_SubclassReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (g_pNmSimple->total_subclass == 0)
    {
        list_set_count(g_pNmSimple->hSubClaList, g_pNmSimple->total_subclass, NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE);
        ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    }

    if (g_pNmSimple->subclaList == NULL)
    {
        al_netmedia_get_subclass_list(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].id,
                                                       g_pNmSimple->classList[g_pNmSimple->curClassListIdx].key);
    }
    else
    {
        al_netmedia_get_subclass_list(g_pNmSimple->subclaList[g_pNmSimple->curSubClaListIdx].id,
                                                       g_pNmSimple->subclaList[g_pNmSimple->curSubClaListIdx].key);
    }
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_LOADING_WITH_WAIT);
}

static STATEID StaSimple_subclass_on_newsubclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_subclass_context *p_new_ctx;

    OS_PRINTF("@@@%s, %d\n", __FUNCTION__, __LINE__);
    if (g_pNmSimple->subclaList != NULL)
    {
        p_new_ctx = (ui_subclass_context *)mtos_malloc(sizeof(ui_subclass_context));
        MT_ASSERT(p_new_ctx != NULL);
        p_new_ctx->subclaList = g_pNmSimple->subclaList;
        p_new_ctx->total_subclass = g_pNmSimple->total_subclass;
        p_new_ctx->curSubClaListIdx = g_pNmSimple->curSubClaListIdx;

        ui_link_list_insert(&g_pNmSimple->hHisList, 1, p_new_ctx);
    }

    if (ui_load_new_page_subclass((al_netmedia_class_list_t *)para2))
    {
        return SID_SIMPLE_SUBCLASS_LIST;
    }

    g_pNmSimple->subclaList = NULL;
    g_pNmSimple->total_subclass = 0;
    g_pNmSimple->curSubClaListIdx = 0;
    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_NO_DATA);
    return SID_NULL;
}

static STATEID StaSimple_subclass_on_get_subclasslist_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);
    return SID_NULL;
}

static void SexSimple_SubclassReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenSimple_SubclassList(void)
{
    OS_PRINTF("@@@%s, %d\n", __FUNCTION__, __LINE__);
    list_set_count(g_pNmSimple->hSubClaList, g_pNmSimple->total_subclass, NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE);
    list_set_focus_pos(g_pNmSimple->hSubClaList, g_pNmSimple->curSubClaListIdx);
    nm_simple_update_subclass_list(g_pNmSimple->hSubClaList, list_get_valid_pos(g_pNmSimple->hSubClaList), NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE, 0);

    g_pNmSimple->totalSubClaPage = (g_pNmSimple->total_subclass > 0) 
                                                  ? (g_pNmSimple->total_subclass + NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE - 1) / NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE 
                                                  : 1;
    g_pNmSimple->curSubClaPageNum = g_pNmSimple->curSubClaListIdx / NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE + 1;
    nm_simple_update_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curSubClaPageNum, g_pNmSimple->totalSubClaPage);

    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
}

static STATEID StaSimple_subclass_on_subclasslist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 page_num;

    FOCUS_KEY_MAP(para1, msg);
    OS_PRINTF("@@@%s curSubClassListIdx=%d\n", __FUNCTION__, g_pNmSimple->curSubClaListIdx);

    switch(msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(ctrl, msg, para1, para2);
            g_pNmSimple->curSubClaListIdx = list_get_focus_pos(ctrl);
            page_num = g_pNmSimple->curSubClaListIdx / NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE + 1;
            if (g_pNmSimple->curSubClaPageNum != page_num)
            {
                g_pNmSimple->curSubClaPageNum = page_num;
                nm_simple_update_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curSubClaPageNum, g_pNmSimple->totalSubClaPage);
                ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
            }
            OS_PRINTF("@@@curSubClassListIdx=%d\n", g_pNmSimple->curSubClaListIdx);
            break;

        case MSG_FOCUS_LEFT:
            if (list_get_count(g_pNmSimple->hSubClaList) > 0)
            {
                ctrl_change_focus(g_pNmSimple->hSubClaList, g_pNmSimple->hClaList);
            }
            break;
    }

    return SID_NULL;
}

static STATEID StaSimple_subclass_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s curSubClaListIdx=%d\n", __FUNCTION__, g_pNmSimple->curSubClaListIdx);

    PAGE_KEY_MAP(para1,msg);
    
    list_class_proc(ctrl, msg, para1, para2);
    g_pNmSimple->curSubClaListIdx = list_get_focus_pos(ctrl);

    g_pNmSimple->curSubClaPageNum = g_pNmSimple->curSubClaListIdx / NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE + 1;
    nm_simple_update_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curSubClaPageNum, g_pNmSimple->totalSubClaPage);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
    OS_PRINTF("@@@curSubClaListIdx=%d\n", g_pNmSimple->curSubClaListIdx);

    return SID_NULL;
}

static STATEID StaSimple_subclass_on_subclasslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_pNmSimple->subclaList != NULL)
    {
        switch (g_pNmSimple->subclaList[g_pNmSimple->curSubClaListIdx].attr)
        {
            case NM_SUB_ATTR_CLASS:
                OS_PRINTF("@@@NM_SUB_ATTR_CLASS\n");
                return SID_SIMPLE_SUBCLASS_REQ;

            case NM_SUB_ATTR_ITEM:
                OS_PRINTF("@@@NM_SUB_ATTR_ITEM\n");
                ctrl_process_msg(g_pNmSimple->hSubClaList, MSG_LOSTFOCUS, 0, 0);
                ctrl_set_attr(g_pNmSimple->hVdoList, OBJ_ATTR_ACTIVE);
                ctrl_process_msg(g_pNmSimple->hVdoList, MSG_GETFOCUS, 0, 0);
                return SID_SIMPLE_VDO_REQ;

            default:
                break;
        }
    }

    return SID_NULL;
}

static STATEID StaSimple_subclass_on_subclasslist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_subclass_context *p_ctx;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_link_list_get_size(&g_pNmSimple->hHisList) <= 1)
    {
        ctrl_change_focus(g_pNmSimple->hSubClaList, g_pNmSimple->hClaList);

        return SID_NULL;
    }
    else
    {
        ui_release_subclasslist();
        g_pNmSimple->total_subclass = 0;

        ui_link_list_delete(&g_pNmSimple->hHisList, 1, (void **)&p_ctx);
        g_pNmSimple->subclaList = p_ctx->subclaList;
        g_pNmSimple->total_subclass = p_ctx->total_subclass;
        g_pNmSimple->curSubClaListIdx = p_ctx->curSubClaListIdx;

        return SID_SIMPLE_SUBCLASS_LIST;
    }
}

static STATEID StaSimple_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    kb_param_t param;

    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (IS_MASKED(g_pNmSimple->attr, NETMEDIA_UI_ATTR_SEARCH))
    {
        param.uni_str = g_pNmSimple->searchKey;
        param.type = KB_INPUT_TYPE_SENTENCE;
        param.max_len = MAX_SEARCH_NAME_LEN;
        param.cb = ui_nm_simple_edit_cb;
        manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    }

    return SID_NULL;
}

static STATEID StaSimple_subclass_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (ctrl_is_onfocus(g_pNmSimple->hSubClaList))
    {
        ctrl_process_msg(g_pNmSimple->hSubClaList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_attr(g_pNmSimple->hVdoList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(g_pNmSimple->hVdoList, MSG_GETFOCUS, 0, 0);
    }

    al_netmedia_set_search_keyword(g_pNmSimple->searchKey);

    return SID_NULL;
}

static void SexSimple_SubclassList(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }
}

static void SenSimple_Vdo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_pNmSimple->totalVdoPage = 1;
    g_pNmSimple->curVdoPageNum = 1;
    g_pNmSimple->total_vdo = 0;
    g_pNmSimple->curVdoListIdx = 0;

    //ctrl_set_attr(g_pNmSimple->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmSimple->hVdoList, OBJ_STS_SHOW);

    ui_comm_help_set_data(&vdo_help_data, g_pNmSimple->hHelpCont);
    ctrl_paint_ctrl(g_pNmSimple->hHelpCont, TRUE);
}

static STATEID StaSimple_vdo_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_pNmSimple->subclaList == NULL)
    {
        ctrl_change_focus(g_pNmSimple->hVdoList, g_pNmSimple->hClaList);
        return SID_NULL;
    }
    else
    {
        ctrl_process_msg(g_pNmSimple->hVdoList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_attr(g_pNmSimple->hSubClaList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(g_pNmSimple->hSubClaList, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(g_pNmSimple->hVdoList,TRUE);
        ctrl_paint_ctrl(g_pNmSimple->hSubClaList,TRUE);
        return SID_SIMPLE_SUBCLASS_LIST;
    }
}

static void SexSimple_Vdo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_set_sts(g_pNmSimple->hVdoList, OBJ_STS_HIDE);
    ui_nm_simple_pic_clear();
}

static void SenSimple_VdoReq(void)
{
    OS_PRINTF("@@@%s, curVdoPageNum=%d\n", __FUNCTION__, g_pNmSimple->curVdoPageNum);
    // Show ui
    ui_release_vdolist();

    g_pNmSimple->vdo_cnt = (u16)((g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage) 
                                  ? g_pNmSimple->vdo_page_size 
                                  : (g_pNmSimple->total_vdo-(g_pNmSimple->totalVdoPage-1)*g_pNmSimple->vdo_page_size));
    list_set_count(g_pNmSimple->hVdoList, g_pNmSimple->vdo_cnt, g_pNmSimple->vdo_page_size);
    if (g_pNmSimple->vdo_cnt < g_pNmSimple->vdo_page_size)
    {
        ui_nm_simple_pic_clear();
    }
    if (g_pNmSimple->curVdoPageNum == 1)
    {
        list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
    }
    else if (g_pNmSimple->curVdoListIdx >= g_pNmSimple->vdo_cnt)
    {
        g_pNmSimple->curVdoListIdx = g_pNmSimple->vdo_cnt - 1;
        list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
    }
    nm_simple_update_vdo_list(g_pNmSimple->hVdoList, 0, g_pNmSimple->vdo_page_size, 0);
    nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curVdoPageNum, g_pNmSimple->totalVdoPage);

    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);

    // Send request
    if (g_pNmSimple->subclaList == NULL)
    {
        al_netmedia_get_video_list(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].id,
                                                 g_pNmSimple->classList[g_pNmSimple->curClassListIdx].key,
                                                 0,
                                                 NULL, 
                                                 g_pNmSimple->curVdoPageNum);
    }
    else
    {
        al_netmedia_get_video_list(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].id,
                                                 g_pNmSimple->classList[g_pNmSimple->curClassListIdx].key,
                                                 g_pNmSimple->subclaList[g_pNmSimple->curSubClaListIdx].id,
                                                 g_pNmSimple->subclaList[g_pNmSimple->curSubClaListIdx].key, 
                                                 g_pNmSimple->curVdoPageNum);
    }
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_LOADING_WITH_WAIT);
}

static STATEID StaSimple_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);
    OS_PRINTF("@@@%s curVdoListIdx=%d\n", __FUNCTION__, g_pNmSimple->curVdoListIdx);

    switch(msg)
    {
        case MSG_FOCUS_UP:
            if (g_pNmSimple->curVdoListIdx == 0)
            {
                if (g_pNmSimple->curVdoPageNum > 1)
                {
                    g_pNmSimple->curVdoPageNum--;
                    return SID_SIMPLE_VDO_REQ;
                }
                else
                {
                    return SID_NULL;
                }
            }
            list_class_proc(ctrl, msg, para1, para2);
            g_pNmSimple->curVdoListIdx = list_get_focus_pos(ctrl);
            OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmSimple->curVdoListIdx);
            break;

        case MSG_FOCUS_DOWN:
            if (g_pNmSimple->curVdoListIdx + 1 >= g_pNmSimple->vdo_cnt)
            {
                if (g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage)
                {
                    g_pNmSimple->curVdoPageNum++;
                    return SID_SIMPLE_VDO_REQ;
                }
                else
                {
                    return SID_NULL;
                }
            }
            list_class_proc(ctrl, msg, para1, para2);
            g_pNmSimple->curVdoListIdx = list_get_focus_pos(ctrl);
            OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmSimple->curVdoListIdx);
            break;

        case MSG_FOCUS_LEFT:
            if (list_get_count(g_pNmSimple->hVdoList) > 0)
            {
                ctrl_change_focus(g_pNmSimple->hVdoList, g_pNmSimple->hClaList);
            }
            break;
    }

    return SID_NULL;
}

static STATEID  StaSimple_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    PAGE_KEY_MAP(para1, msg);

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (para1 == V_KEY_PAGE_UP)
    {
        if (g_pNmSimple->curVdoPageNum > 1)
        {
            g_pNmSimple->curVdoPageNum--;
            return SID_SIMPLE_VDO_REQ;
        }
    }
    else if (para1 == V_KEY_PAGE_DOWN)
    {
        if (g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage)
        {
            g_pNmSimple->curVdoPageNum++;
            return SID_SIMPLE_VDO_REQ;
        }
    }
    return SID_NULL;
}

static STATEID StaSimple_vdo_on_newpagevdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_comm_dlg_close();

    if (ui_load_new_page_vdo((al_netmedia_vdo_list_t *)para2))
    {
        return SID_SIMPLE_VDO_LIST;
    }

    if (g_pNmSimple->curVdoPageNum == 1)
    {
        ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_NO_DATA);
    }
    else
    {
        ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);
    }

    return SID_NULL;
}

static STATEID StaSimple_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_comm_dlg_close();
    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);

    return SID_NULL;
}

static void SexSimple_VdoReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenSimple_VdoList(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    nm_simple_update_vdo_list(g_pNmSimple->hVdoList, 0, g_pNmSimple->vdo_page_size, 0);
    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);

    g_pNmSimple->curVdoListRendIdx = 0;
    ui_nm_simple_pic_play_start();
}

static STATEID  StaSimple_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_NM_SIMPLE)
    {
        list_set_field_content_by_icon(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListRendIdx, VDOLIST_FIELD_ID_LOGO, RSC_INVALID_ID);
        list_draw_field_ext(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListRendIdx, VDOLIST_FIELD_ID_LOGO, TRUE);

        pic_stop();
        g_pNmSimple->curVdoListRendIdx++;
        ui_nm_simple_pic_play_start();
    }

    return SID_NULL;
}

static STATEID  StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_NM_SIMPLE)
    {
        pic_stop();
        g_pNmSimple->curVdoListRendIdx++;
        ui_nm_simple_pic_play_start();
    }

    return SID_NULL;
}

static STATEID  StaSimple_vdo_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_pNmSimple->vdoList != NULL)
    {
        if ((g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].id != NULL) && (g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].key != NULL))
        {
            return SID_SIMPLE_VDO_URL_REQ;
        }
    }

    return SID_NULL;
}

static STATEID  StaSimple_vdo_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SID_NULL;
}

static STATEID StaSimple_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ctrl_is_onfocus(g_pNmSimple->hVdoList))
    {
        g_pNmSimple->jumpVdoPageNum = para1;
        if (g_pNmSimple->jumpVdoPageNum > g_pNmSimple->totalVdoPage)
        {
            g_pNmSimple->jumpVdoPageNum = g_pNmSimple->totalVdoPage;
        }
        nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, 
                                                           g_pNmSimple->jumpVdoPageNum, 
                                                            g_pNmSimple->totalVdoPage);
        ctrl_change_focus(g_pNmSimple->hVdoList, g_pNmSimple->hPageNum);
    }
    else
    {
        if (g_pNmSimple->jumpVdoPageNum < g_pNmSimple->totalVdoPage)
        {  
            g_pNmSimple->jumpVdoPageNum = g_pNmSimple->jumpVdoPageNum * 10 + para1;
        }
        else
        {
            g_pNmSimple->jumpVdoPageNum = para1;
        }
        if (g_pNmSimple->jumpVdoPageNum > g_pNmSimple->totalVdoPage)
        {
            g_pNmSimple->jumpVdoPageNum = g_pNmSimple->totalVdoPage;
        }
        nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, 
                                                            g_pNmSimple->jumpVdoPageNum, 
                                                            g_pNmSimple->totalVdoPage);
        ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
    }

    return SID_NULL;
}

static STATEID StaSimple_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pNmSimple->hPageNum, g_pNmSimple->hVdoList);

    if(g_pNmSimple->jumpVdoPageNum == 0)
    {
       g_pNmSimple->jumpVdoPageNum = 1;
       nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, 
                                                   g_pNmSimple->jumpVdoPageNum, 
                                                   g_pNmSimple->totalVdoPage);
       ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
    }
    g_pNmSimple->curVdoPageNum = g_pNmSimple->jumpVdoPageNum;

    return SID_NULL;
}

static STATEID StaSimple_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_pNmSimple->curVdoPageNum != g_pNmSimple->jumpVdoPageNum)
    {
        nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, 
                                                            g_pNmSimple->curVdoPageNum, 
                                                            g_pNmSimple->totalVdoPage);
    }

    ctrl_change_focus(g_pNmSimple->hPageNum, g_pNmSimple->hVdoList);

    return SID_NULL;
}

static STATEID StaSimple_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    al_netmedia_set_search_keyword(g_pNmSimple->searchKey);

    return SID_NULL;
}

static void SexSimple_VdoList(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }

    pic_stop();
}

static void SenSimple_VdoUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_MSG_GET_URL);
    //get url
    al_netmedia_get_play_urls(g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].id,g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].key);
}

static STATEID StaSimple_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_play_url_list_t *p_data = (al_netmedia_play_url_list_t *)para2;
    al_netmedia_play_url_item_t *p_url = NULL;
    ui_video_player_param vdoPlayerParam;
    nm_simple_config_t config;
    u16 i;

    OS_PRINTF("@@@%s,%d,p_data->url_cnt==%d\n",__FUNCTION__,__LINE__,p_data->play_url_cnt);
    ui_comm_dlg_close();

    sys_status_get_nm_simple_config_info(&config);

    if (p_data->play_url_cnt > 0 && p_data->playUrlList != NULL)
    {
        for (i = 0; i < p_data->play_url_cnt; i++)
        {
            if (p_data->playUrlList[i].resolution == config.resolution)
            {
                if (p_data->playUrlList[i].pp_fragmntUrlList != NULL)
                {
                    p_url = p_data->playUrlList + i;
                    break;
                } 
            }
        }
        if (i >= p_data->play_url_cnt)
        { 
            for (i = 0; i < p_data->play_url_cnt; i++)
            {
                if (p_data->playUrlList[i].pp_fragmntUrlList != NULL)
                {
                    p_url = p_data->playUrlList + i;
                    break;
                }
            }
        }
    }

    if ((p_url != NULL) && (p_url->fragment_url_cnt > 0) && (p_url->pp_fragmntUrlList != NULL))
    {
        for (i = 0; i < p_url->fragment_url_cnt; i++)
        {
            if (p_url->pp_fragmntUrlList[i] == NULL)
            {
                break;
            }
        }

        if (i >= p_url->fragment_url_cnt)
        {
            ui_nm_simple_pic_deinit();

            memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));
            vdoPlayerParam.name = g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].title;
            vdoPlayerParam.url_cnt = p_url->fragment_url_cnt;
            vdoPlayerParam.pp_urlList = p_url->pp_fragmntUrlList;
            manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_NM_SIMPLE, (u32)&vdoPlayerParam);

            return SID_NULL;
        }
    }

    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);

    return SID_NULL;
}

static STATEID StaSimple_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_comm_dlg_close();

    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaSimple_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_pic_init();

    return SID_NULL;
}

static void SexSimple_VdoUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenSimple_Search(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_pNmSimple->totalVdoPage = 1;
    g_pNmSimple->curVdoPageNum = 1;
    g_pNmSimple->total_vdo = 0;
    g_pNmSimple->curVdoListIdx = 0;

    //ctrl_set_attr(g_pNmSimple->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmSimple->hVdoList, OBJ_STS_SHOW);

    ui_comm_help_set_data(&vdo_help_data, g_pNmSimple->hHelpCont);
    ctrl_paint_ctrl(g_pNmSimple->hHelpCont, TRUE);
}

static STATEID StaSimple_search_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pNmSimple->hVdoList, g_pNmSimple->hClaList);
    return SID_NULL;
}

static void SexSimple_Search(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_set_sts(g_pNmSimple->hVdoList, OBJ_STS_HIDE);
    ui_nm_simple_pic_clear();
}

static void SenSimple_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s, curVdoPageNum=%d\n", __FUNCTION__, g_pNmSimple->curVdoPageNum);
    // Show ui
    ui_release_vdolist();

    g_pNmSimple->vdo_cnt = (u16)((g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage) 
                                  ? g_pNmSimple->vdo_page_size 
                                  : (g_pNmSimple->total_vdo-(g_pNmSimple->totalVdoPage-1)*g_pNmSimple->vdo_page_size));
    list_set_count(g_pNmSimple->hVdoList, g_pNmSimple->vdo_cnt, g_pNmSimple->vdo_page_size);
    if (g_pNmSimple->vdo_cnt < g_pNmSimple->vdo_page_size)
    {
        ui_nm_simple_pic_clear();
    }
    if (g_pNmSimple->curVdoPageNum == 1)
    {
        list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
    }
    else if (g_pNmSimple->curVdoListIdx >= g_pNmSimple->vdo_cnt)
    {
        g_pNmSimple->curVdoListIdx = g_pNmSimple->vdo_cnt - 1;
        list_set_focus_pos(g_pNmSimple->hVdoList, g_pNmSimple->curVdoListIdx);
    }
    nm_simple_update_vdo_list(g_pNmSimple->hVdoList, 0, g_pNmSimple->vdo_page_size, 0);
    nm_simple_update_vdo_page_num(g_pNmSimple->hPageNum, g_pNmSimple->curVdoPageNum, g_pNmSimple->totalVdoPage);

    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);

    // Send request
    al_netmedia_search(g_pNmSimple->classList[g_pNmSimple->curClassListIdx].id, g_pNmSimple->curVdoPageNum);
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_LOADING_WITH_WAIT);
}

static STATEID StaSimple_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);
    OS_PRINTF("@@@%s curVdoListIdx=%d\n", __FUNCTION__, g_pNmSimple->curVdoListIdx);

    switch(msg)
    {
        case MSG_FOCUS_UP:
            if (g_pNmSimple->curVdoListIdx == 0)
            {
                if (g_pNmSimple->curVdoPageNum > 1)
                {
                    g_pNmSimple->curVdoPageNum--;
                    return SID_SIMPLE_SEARCH_VDO_REQ;
                }
                else
                {
                    return SID_NULL;
                }
            }
            list_class_proc(ctrl, msg, para1, para2);
            g_pNmSimple->curVdoListIdx = list_get_focus_pos(ctrl);
            OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmSimple->curVdoListIdx);
            break;

        case MSG_FOCUS_DOWN:
            if (g_pNmSimple->curVdoListIdx + 1 >= g_pNmSimple->vdo_cnt)
            {
                if (g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage)
                {
                    g_pNmSimple->curVdoPageNum++;
                    return SID_SIMPLE_SEARCH_VDO_REQ;
                }
                else
                {
                    return SID_NULL;
                }
            }
            list_class_proc(ctrl, msg, para1, para2);
            g_pNmSimple->curVdoListIdx = list_get_focus_pos(ctrl);
            OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmSimple->curVdoListIdx);
            break;
            
        case MSG_FOCUS_LEFT:
            if (list_get_count(g_pNmSimple->hVdoList) > 0)
            {
                ctrl_change_focus(g_pNmSimple->hVdoList, g_pNmSimple->hClaList);
            }
            break;
    }

    return SID_NULL;
}

static STATEID  StaSimple_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    PAGE_KEY_MAP(para1, msg);

    OS_PRINTF("@@@StaSimple_search_vdo_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_pNmSimple->curVdoPageNum > 1)
        {
            g_pNmSimple->curVdoPageNum--;
            return SID_SIMPLE_SEARCH_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_pNmSimple->curVdoPageNum < g_pNmSimple->totalVdoPage)
        {
            g_pNmSimple->curVdoPageNum++;
            return SID_SIMPLE_SEARCH_VDO_REQ;
        }
    }
    return SID_NULL;
}

static STATEID StaSimple_search_on_newsearchvdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_comm_dlg_close();

    if (ui_load_new_page_vdo((al_netmedia_vdo_list_t *)para2))
    {
        return SID_SIMPLE_SEARCH_VDO_LIST;
    }

    if (g_pNmSimple->curVdoPageNum == 1)
    {
        ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_NO_DATA);
    }
    else
    {
        ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);
    }

    return SID_NULL;
}

static void SexSimple_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenSimple_SearchVdoList(void)
{
    OS_PRINTF("@@@%s, %d\n", __FUNCTION__, __LINE__);
    nm_simple_update_vdo_list(g_pNmSimple->hVdoList, 0, g_pNmSimple->vdo_page_size, 0);
    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);

    g_pNmSimple->curVdoListRendIdx = 0;
    ui_nm_simple_pic_play_start();
}

static STATEID StaSimple_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if ((g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].id != NULL) && (g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].key != NULL))
    {
        return SID_SIMPLE_SEARCH_URL_REQ;
    }

    return SID_NULL;
}

static void SexSimple_SearchVdoList(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }

    pic_stop();
}

static void SenSimple_SearchUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_open_dlg(ROOT_ID_NM_SIMPLE, IDS_MSG_GET_URL);
    //get url
    al_netmedia_get_play_urls(g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].id,g_pNmSimple->vdoList[g_pNmSimple->curVdoListIdx].key);
}

static void SexSimple_SearchUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenSimple_VdoInfo(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SexSimple_VdoInfo(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenSimple_Resolution(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    list_set_count(g_pNmSimple->hOptList, ARRAY_SIZE(g_resolution_opt), NM_SIMPLE_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(g_pNmSimple->hOptList, 0);

    //ctrl_set_attr(g_pNmSimple->hOptList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmSimple->hOptList, OBJ_STS_SHOW);

    nm_simple_update_page_num(g_pNmSimple->hPageNum, 1, 1);
    nm_simple_update_opt_list(g_pNmSimple->hOptList, list_get_valid_pos(g_pNmSimple->hOptList), NM_SIMPLE_OPT_LIST_ITEM_NUM_ONE_PAGE, 0);

    ui_comm_help_set_data(&comm_help_data, g_pNmSimple->hHelpCont);

    ctrl_paint_ctrl(g_pNmSimple->hPageNum, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmSimple->hHelpCont, TRUE);
}

static void SexSimple_Resolution(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ctrl_set_sts(g_pNmSimple->hOptList, OBJ_STS_HIDE);
}

static void SenSimple_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_simple_open_cfm_dlg(ROOT_ID_NM_SIMPLE, IDS_DATA_ERROR);
}

static void SexSimple_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenSimple_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    al_netmedia_website_deinit();
}

static STATEID StaSimple_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_NM_SIMPLE);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    return SID_NULL;
}  

static void SexSimple_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

/*================================================================================================
                           nm simple public works function
 ================================================================================================*/

RET_CODE ui_open_nm_simple(u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_bmp;
    control_t *p_list_cont, *p_list;
    control_t *p_page_cont, *p_icon, *p_page_num;
    control_t *p_client_cont, *p_vdoList;
    control_t *p_subclass_list;
    control_t *p_optlist;
    control_t *p_preview, *p_detail_cont, *p_detail, *p_detail_sbar;
    control_t *p_bottom_help;
    u8 i = 0;

    OS_PRINTF("@@@ui_open_nm_simple\n");

    ui_init_nm_simple_data();

    ui_nm_simple_pic_init();

    g_pNmSimple->nm_type = (NETMEDIA_DP_TYPE)para1;
    g_pNmSimple->attr = (NETMEDIA_UI_ATTR_E)para2;
    g_pNmSimple->ext_class = ARRAY_SIZE(class_list_ext);

    g_pNmSimple->vdo_page_size = NM_SIMPLE_VDO_LIST_ITEM_NUM_ONE_PAGE;

    for (i = 0; i < ARRAY_SIZE(g_dp_type_rsc); i++)
    {
        if (g_pNmSimple->nm_type == g_dp_type_rsc[i].nm_type)
        {
            break;
        }
    }

    if (i >= ARRAY_SIZE(g_dp_type_rsc))
    {
        i = 0;
    }
    
    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_NM_SIMPLE, 0,
                                  IM_INDEX_DEFAULT_BANNER, g_dp_type_rsc[i].str_id);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, nm_simple_cont_keymap);
    ctrl_set_proc(p_menu, nm_simple_cont_proc);

    /*!
     * Create Help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_BTM_HELP,
                                      NM_SIMPLE_BOTTOM_HELP_X, NM_SIMPLE_BOTTOM_HELP_Y,
                                      NM_SIMPLE_BOTTOM_HELP_W, NM_SIMPLE_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    ui_comm_help_create_ext(60, 0, 
                                NM_SIMPLE_BOTTOM_HELP_W-60, NM_SIMPLE_BOTTOM_HELP_H,  
                                &emp_help_data,  p_bottom_help);
    /*!
     * Create Class list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_LIST_CONT,
                                  NM_SIMPLE_CLASS_LIST_CONTX, NM_SIMPLE_CLASS_LIST_CONTY,
                                  NM_SIMPLE_CLASS_LIST_CONTW, NM_SIMPLE_CLASS_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_SIMPLE_LEFT_BMP, 
                                        NM_SIMPLE_LOGO_X, NM_SIMPLE_LOGO_Y, 
                                        NM_SIMPLE_LOGO_W, NM_SIMPLE_LOGO_H, 
                                        p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, g_dp_type_rsc[i].bmp_id);
 
    //class list
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_SIMPLE_CLASS_LIST,
                          NM_SIMPLE_CLASS_LIST_X, NM_SIMPLE_CLASS_LIST_Y,
                          NM_SIMPLE_CLASS_LIST_W, NM_SIMPLE_CLASS_LIST_H,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, nm_simple_class_list_keymap);
    ctrl_set_proc(p_list, nm_simple_class_list_proc);

    ctrl_set_mrect(p_list, NM_SIMPLE_CLASS_LIST_MIDL, NM_SIMPLE_CLASS_LIST_MIDT,
                           NM_SIMPLE_CLASS_LIST_MIDW + NM_SIMPLE_CLASS_LIST_MIDL, 
                           NM_SIMPLE_CLASS_LIST_MIDH + NM_SIMPLE_CLASS_LIST_MIDT);
    list_set_item_interval(p_list, NM_SIMPLE_CLASS_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &class_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, 0, NM_SIMPLE_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(class_list_attr), NM_SIMPLE_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_list, nm_simple_update_class_list, 0);

    for (i = 0; i < ARRAY_SIZE(class_list_attr); i++)
    {
        list_set_field_attr(p_list, i, class_list_attr[i].attr,
                            class_list_attr[i].width, class_list_attr[i].left, class_list_attr[i].top);
        list_set_field_rect_style(p_list, i, class_list_attr[i].rstyle);
        list_set_field_font_style(p_list, i, class_list_attr[i].fstyle);
    }

    //Search help bar
    if (IS_MASKED(g_pNmSimple->attr, NETMEDIA_UI_ATTR_SEARCH))
    {
        ui_comm_help_create_ext(NM_SIMPLE_SEARCH_HELP_X, NM_SIMPLE_SEARCH_HELP_Y,
                                        NM_SIMPLE_SEARCH_HELP_W, NM_SIMPLE_SEARCH_HELP_H,
                                        &search_help_data, p_list_cont);
    }

    /*!
     * Create Page num
     */
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_PAGE_CONT,
                                    NM_SIMPLE_PAGE_CONTX, NM_SIMPLE_PAGE_CONTY,
                                    NM_SIMPLE_PAGE_CONTW, NM_SIMPLE_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               NM_SIMPLE_PAGE_ARROWL_X, NM_SIMPLE_PAGE_ARROWL_Y,
                               NM_SIMPLE_PAGE_ARROWL_W, NM_SIMPLE_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               NM_SIMPLE_PAGE_ARROWR_X, NM_SIMPLE_PAGE_ARROWR_Y,
                               NM_SIMPLE_PAGE_ARROWR_W, NM_SIMPLE_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_page_num = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_NM_SIMPLE_PAGE_NUM,
                                 NM_SIMPLE_PAGE_NUMBER_X, NM_SIMPLE_PAGE_NUMBER_Y,
                                 NM_SIMPLE_PAGE_NUMBER_W, NM_SIMPLE_PAGE_NUMBER_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_page_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_page_num, nm_simple_page_num_keymap);
    ctrl_set_proc(p_page_num, nm_simple_page_num_proc);

    text_set_align_type(p_page_num, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_page_num, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_page_num, TEXT_STRTYPE_UNICODE);

    /*!
     * Create Client rect
     */
    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_CLIENT_CONT,
                                  NM_SIMPLE_CLIENT_CONTX, NM_SIMPLE_CLIENT_CONTY,
                                  NM_SIMPLE_CLIENT_CONTW, NM_SIMPLE_CLIENT_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //Video list
    p_vdoList = ctrl_create_ctrl(CTRL_LIST, IDC_NM_SIMPLE_VDO_LIST, 
                                        NM_SIMPLE_VDO_LIST_X, NM_SIMPLE_VDO_LIST_Y,
                                        NM_SIMPLE_VDO_LIST_W, NM_SIMPLE_VDO_LIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_vdoList, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_vdoList, nm_simple_vdo_list_keymap);
    ctrl_set_proc(p_vdoList, nm_simple_vdo_list_proc);

    list_set_item_interval(p_vdoList, NM_SIMPLE_VDO_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_vdoList, &vdo_list_item_rstyle);

    list_set_count(p_vdoList, 0, NM_SIMPLE_VDO_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_vdoList, ARRAY_SIZE(vdo_list_attr), NM_SIMPLE_VDO_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_vdoList, nm_simple_update_vdo_list, 0);

    for(i = 0; i < ARRAY_SIZE(vdo_list_attr); i++)
    {
        list_set_field_attr2(p_vdoList, (u8)i, vdo_list_attr[i].attr,
                            vdo_list_attr[i].width, vdo_list_attr[i].height, vdo_list_attr[i].left, vdo_list_attr[i].top);
        list_set_field_rect_style(p_vdoList, (u8)i, vdo_list_attr[i].rstyle);
        list_set_field_font_style(p_vdoList, (u8)i, vdo_list_attr[i].fstyle);
    }
    ctrl_set_sts(p_vdoList, OBJ_STS_HIDE);

    //Subclass list
    p_subclass_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_SIMPLE_SUBCLASS_LIST, 
                                        NM_SIMPLE_VDO_LIST_X, NM_SIMPLE_VDO_LIST_Y,
                                        NM_SIMPLE_VDO_LIST_W, NM_SIMPLE_VDO_LIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_subclass_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_subclass_list, nm_simple_subclass_list_keymap);
    ctrl_set_proc(p_subclass_list, nm_simple_subclass_list_proc);

    list_set_item_interval(p_subclass_list, NM_SIMPLE_VDO_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_subclass_list, &vdo_list_item_rstyle);

    list_set_count(p_subclass_list, 0, NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE);
    list_set_field_count(p_subclass_list, ARRAY_SIZE(subclass_list_attr), NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE);
    list_set_update(p_subclass_list, nm_simple_update_subclass_list, 0);
    list_enable_page_mode(p_subclass_list, TRUE);

    for(i = 0; i < ARRAY_SIZE(subclass_list_attr); i++)
    {
        list_set_field_attr(p_subclass_list, (u8)i, subclass_list_attr[i].attr,
                            subclass_list_attr[i].width, subclass_list_attr[i].left, subclass_list_attr[i].top);
        list_set_field_rect_style(p_subclass_list, (u8)i, subclass_list_attr[i].rstyle);
        list_set_field_font_style(p_subclass_list, (u8)i, subclass_list_attr[i].fstyle);
    }
    ctrl_set_sts(p_subclass_list, OBJ_STS_HIDE);

    //Option list
    p_optlist = ctrl_create_ctrl(CTRL_LIST, IDC_NM_SIMPLE_OPT_LIST, 
                                        NM_SIMPLE_VDO_LIST_X, NM_SIMPLE_VDO_LIST_Y,
                                        NM_SIMPLE_VDO_LIST_W, NM_SIMPLE_VDO_LIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_optlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_optlist, nm_simple_opt_list_keymap);
    ctrl_set_proc(p_optlist, nm_simple_opt_list_proc);

    list_set_item_interval(p_optlist, NM_SIMPLE_VDO_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_optlist, &vdo_list_item_rstyle);

    list_set_count(p_optlist, 0, NM_SIMPLE_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_optlist, ARRAY_SIZE(opt_list_attr), NM_SIMPLE_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_optlist, nm_simple_update_opt_list, 0);
    list_enable_page_mode(p_optlist, TRUE);

    for(i = 0; i < ARRAY_SIZE(opt_list_attr); i++)
    {
        list_set_field_attr(p_optlist, (u8)i, opt_list_attr[i].attr,
                            opt_list_attr[i].width, opt_list_attr[i].left, opt_list_attr[i].top);
        list_set_field_rect_style(p_optlist, (u8)i, opt_list_attr[i].rstyle);
        list_set_field_font_style(p_optlist, (u8)i, opt_list_attr[i].fstyle);
    }
    ctrl_set_sts(p_optlist, OBJ_STS_HIDE);

    //info
    p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_INFO_PIC, 
                                NM_SIMPLE_INFO_PIC_X, NM_SIMPLE_INFO_PIC_Y, 
                                NM_SIMPLE_INFO_PIC_W, NM_SIMPLE_INFO_PIC_H, 
                                p_client_cont, 0);
    ctrl_set_rstyle(p_preview, RSI_TV, RSI_TV, RSI_TV);
    ctrl_set_sts(p_preview, OBJ_STS_HIDE);

    p_detail_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_SIMPLE_INFO_DETAIL_CONT,
                                      NM_SIMPLE_INFO_DETAIL_CONTX, NM_SIMPLE_INFO_DETAIL_CONTY, 
                                      NM_SIMPLE_INFO_DETAIL_CONTW, NM_SIMPLE_INFO_DETAIL_CONTH, 
                                      p_client_cont, 0);
    ctrl_set_rstyle(p_detail_cont, RSI_BOX_1, RSI_BOX_1, RSI_BOX_1);

    p_detail = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_SIMPLE_INFO_DETAIL,
                                NM_SIMPLE_INFO_DETAIL_X, NM_SIMPLE_INFO_DETAIL_Y, 
                                NM_SIMPLE_INFO_DETAIL_W, NM_SIMPLE_INFO_DETAIL_H,
                                p_detail_cont, 0);
    ctrl_set_keymap(p_detail, nm_simple_info_detail_keymap);
    ctrl_set_proc(p_detail, nm_simple_info_detail_proc);
    ctrl_set_rstyle(p_detail, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_detail, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_detail, TEXT_STRTYPE_EXTSTR);
    text_set_align_type(p_detail, STL_LEFT | STL_TOP);
    text_set_offset(p_detail, 0, 0);
    ctrl_set_mrect(p_detail, 
                    NM_SIMPLE_INFO_DETAIL_MIDX, NM_SIMPLE_INFO_DETAIL_MIDY,
                    NM_SIMPLE_INFO_DETAIL_MIDX + NM_SIMPLE_INFO_DETAIL_MIDW, NM_SIMPLE_INFO_DETAIL_MIDY + NM_SIMPLE_INFO_DETAIL_MIDH);
    text_enable_page(p_detail, TRUE);

    p_detail_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_NM_SIMPLE_INFO_DETAIL_SBAR, 
                                NM_SIMPLE_INFO_DETAIL_SBAR_X, NM_SIMPLE_INFO_DETAIL_SBAR_Y, 
                                NM_SIMPLE_INFO_DETAIL_SBAR_W, NM_SIMPLE_INFO_DETAIL_SBAR_H,
                                p_detail_cont, 0);
    ctrl_set_rstyle(p_detail_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_detail_sbar, 1);
    sbar_set_direction(p_detail_sbar, 0);
    sbar_set_mid_rstyle(p_detail_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    ctrl_set_mrect(p_detail_sbar, 0, COMM_SBAR_VERTEX_GAP,
                    NM_SIMPLE_INFO_DETAIL_SBAR_W, NM_SIMPLE_INFO_DETAIL_SBAR_H - COMM_SBAR_VERTEX_GAP);
    text_set_scrollbar(p_detail, p_detail_sbar);
    ctrl_set_sts(p_detail_cont, OBJ_STS_HIDE);

    g_pNmSimple->hClaList = p_list;
    g_pNmSimple->hSubClaList = p_subclass_list;
    g_pNmSimple->hVdoList = p_vdoList;
    //g_pNmSimple->hTitleName = p_title_name;
    g_pNmSimple->hPageNum = p_page_num;
    g_pNmSimple->hOptList = p_optlist;
    //g_pNmSimple->hCatlistCont = p_catlist_cont;
    g_pNmSimple->hClientCont = p_client_cont;
    g_pNmSimple->hHelpCont = p_bottom_help;

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
    
    //al_netmedia_register_msg();
    Simple_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_NM_SIMPLE), NOTIFY_T_MSG, FALSE, MSG_OPEN_SIMPLE_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_nm_simple_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    Simple_CloseStateTree();

    ui_nm_simple_pic_deinit();

    ui_release_nm_simple_data();

    if (fw_find_root_by_id(ROOT_ID_SUBMENU_NM))
    {
        fw_notify_root(fw_find_root_by_id(ROOT_ID_SUBMENU_NM), NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    return ERR_NOFEATURE;
}

//dispatch msg
static RET_CODE ui_nm_simple_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = Simple_DispatchMsg(p_ctrl, msg, para1, para2);
    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@nm_simple unaccepted msg, id=0x%04x\n", msg);
    }

    return ret;
}

static RET_CODE ui_nm_simple_on_key_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    Simple_DispatchMsg(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

static RET_CODE ui_nm_simple_on_input_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)	
{
    para1 = (u32)(msg & MSG_DATA_MASK);
    msg = (msg & MSG_TYPE_MASK);
    Simple_DispatchMsg(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

void ui_nm_simple_open_cfm_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

void ui_nm_simple_open_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_DLG_REQ, str_id, 0);
    }
}

static RET_CODE ui_nm_simple_on_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_NM_SIMPLE,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_simple_on_open_dlg\n");

    if (Simple_IsStateActive(SID_SIMPLE_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);
    }

    return SUCCESS;
}

static RET_CODE ui_nm_simple_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_NM_SIMPLE,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_simple_on_open_cfm_dlg\n");

    if (Simple_IsStateActive(SID_SIMPLE_ACTIVE))
    {
      dlg_data.content = para1;

      ui_comm_dlg_open(&dlg_data);

      p_root = fw_find_root_by_id(ROOT_ID_NM_SIMPLE);
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
BEGIN_KEYMAP(nm_simple_class_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(nm_simple_class_list_keymap, NULL)

BEGIN_MSGPROC(nm_simple_class_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_nm_simple_on_key_process)
END_MSGPROC(nm_simple_class_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_simple_page_num_keymap, NULL)
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
    ON_EVENT(V_KEY_DOWN, MSG_EXIT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(nm_simple_page_num_keymap, NULL)

BEGIN_MSGPROC(nm_simple_page_num_proc, text_class_proc)
    ON_COMMAND(MSG_NUMBER, ui_nm_simple_on_input_number)
    ON_COMMAND(MSG_SELECT, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_nm_simple_on_state_process)
END_MSGPROC(nm_simple_page_num_proc, text_class_proc)

BEGIN_KEYMAP(nm_simple_vdo_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(nm_simple_vdo_list_keymap, NULL)

BEGIN_MSGPROC(nm_simple_vdo_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_INFO, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NUMBER, ui_nm_simple_on_input_number)
END_MSGPROC(nm_simple_vdo_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_simple_subclass_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(nm_simple_subclass_list_keymap, NULL)

BEGIN_MSGPROC(nm_simple_subclass_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_nm_simple_on_state_process)
END_MSGPROC(nm_simple_subclass_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_simple_opt_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(nm_simple_opt_list_keymap, NULL)

BEGIN_MSGPROC(nm_simple_opt_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_nm_simple_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_nm_simple_on_state_process)
END_MSGPROC(nm_simple_opt_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_simple_info_detail_keymap, NULL)

END_KEYMAP(nm_simple_info_detail_keymap, NULL)

BEGIN_MSGPROC(nm_simple_info_detail_proc, text_class_proc)

END_MSGPROC(nm_simple_info_detail_proc, text_class_proc)

BEGIN_KEYMAP(nm_simple_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_RED, MSG_SEARCH)
END_KEYMAP(nm_simple_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(nm_simple_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, ui_nm_simple_on_state_process)

    ON_COMMAND(MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_PAGE_VDO_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_SEARCH_VDO_FAILED, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_GET_PLAY_URLS_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_CLASS_LIST_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_INIT_SUCCESS, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_INIT_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_SUCCESS, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_DEINIT_SUCCESS, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_DEINIT_FAILED, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS, ui_nm_simple_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_FAILED, ui_nm_simple_on_state_process)

    ON_COMMAND(MSG_OPEN_SIMPLE_REQ, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_SEARCH, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_INPUT_COMP, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_EXIT_ALL, ui_nm_simple_on_state_process)
    ON_COMMAND(MSG_DESTROY, ui_nm_simple_on_destory)
    ON_COMMAND(MSG_UPDATE, ui_nm_simple_on_state_process)

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_nm_simple_on_open_cfm_dlg)
    ON_COMMAND(MSG_OPEN_DLG_REQ, ui_nm_simple_on_open_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_nm_simple_on_state_process)
END_MSGPROC(nm_simple_cont_proc, ui_comm_root_proc)

