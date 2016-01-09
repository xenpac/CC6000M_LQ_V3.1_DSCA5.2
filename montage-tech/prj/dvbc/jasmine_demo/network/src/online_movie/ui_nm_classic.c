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
#include "ui_nm_classic.h"

#include "SM_StateMachine.h"
#include "states_classic.h"
#include "ui_video.h"
#include "ui_picture_api.h"
#include "ui_video_player_gprot.h"
#include "ui_keyboard_v2.h"


/*!
 * Function define
 */
u16 nm_classic_class_list_keymap(u16 key);
RET_CODE nm_classic_class_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_classic_page_num_keymap(u16 key);
RET_CODE nm_classic_page_num_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_classic_vdo_list_keymap(u16 key);
RET_CODE nm_classic_vdo_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_classic_subclass_list_keymap(u16 key);
RET_CODE nm_classic_subclass_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_classic_opt_list_keymap(u16 key);
RET_CODE nm_classic_opt_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 nm_classic_cont_keymap(u16 key);
RET_CODE nm_classic_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static void ui_clean_historylist(void);

extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

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
     28, NM_CLASSIC_OPT_LIST_W - 100, 0, &vdo_list_field_rstyle, &vdo_list_field_fstyle}
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

static ui_nm_dp_type_rsc_item g_dp_type_rsc[] = 
{
    {NETMEDIA_DP_VIMEO, IM_PIC_VIMEO_LOGO, IDS_VIMEO},
    {NETMEDIA_DP_ONLAIN_FILM, IM_PIC_ONLAINFILM_LOGO, IDS_ONLAIN_FILM},
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

//search helpbar
static comm_help_data_t search_help_data =
{
    1, 1,
    {IDS_SEARCH},
    {IM_HELP_RED}
};

// App data
static ui_nm_classic_app_t *g_pNmClassic = NULL;

static void ui_release_nm_classic_data(void);
static void ui_release_classList(void);
static void ui_release_subclassList(void);
static void ui_release_vdolist(void);
void ui_nm_classic_open_cfm_dlg(u8 root_id, u16 str_id);
void ui_nm_classic_open_dlg(u8 root_id, u16 str_id);

/*================================================================================================
                           nm classic internel function
 ================================================================================================*/

static BOOL ui_init_nm_classic_data(void)
{
    if (g_pNmClassic)
    {
      ui_release_nm_classic_data();
    }

    g_pNmClassic = (ui_nm_classic_app_t *)mtos_malloc(sizeof(ui_nm_classic_app_t));
    MT_ASSERT(g_pNmClassic != NULL);
    memset((void *)g_pNmClassic, 0, sizeof(ui_nm_classic_app_t));

    ui_link_list_create(&g_pNmClassic->hHisList, 0);

    return TRUE;
}

static void ui_release_nm_classic_data(void)
{
    if (g_pNmClassic)
    {
        ui_release_classList();

        ui_release_subclassList();
        ui_clean_historylist();
        ui_link_list_destory(&g_pNmClassic->hHisList);

        ui_release_vdolist();

        mtos_free(g_pNmClassic);
        g_pNmClassic = NULL;
    }
}

static void ui_release_vdolist(void)
{
    u16 i;

    if (g_pNmClassic->vdoList)
    {
        for (i = 0; i < g_pNmClassic->vdo_cnt; i++)
        {
            if (g_pNmClassic->vdoList[i].id)
            {
              mtos_free(g_pNmClassic->vdoList[i].id);
              g_pNmClassic->vdoList[i].id = NULL;
            }
            if (g_pNmClassic->vdoList[i].title)
            {
                mtos_free(g_pNmClassic->vdoList[i].title);
                g_pNmClassic->vdoList[i].title = NULL;
            }
            if (g_pNmClassic->vdoList[i].key)
            {
                mtos_free(g_pNmClassic->vdoList[i].key);
                g_pNmClassic->vdoList[i].key = NULL;
            }
            if (g_pNmClassic->vdoList[i].img_url)
            {
                mtos_free(g_pNmClassic->vdoList[i].img_url);
            }
            if (g_pNmClassic->vdoList[i].date)
            {
                mtos_free(g_pNmClassic->vdoList[i].date);
                g_pNmClassic->vdoList[i].date = NULL;
            }
            if (g_pNmClassic->vdoList[i].duration)
            {
                mtos_free(g_pNmClassic->vdoList[i].duration);
                g_pNmClassic->vdoList[i].duration = NULL;
            }
            if (g_pNmClassic->vdoList[i].description)
            {
                mtos_free(g_pNmClassic->vdoList[i].description);
                g_pNmClassic->vdoList[i].description = NULL;
            }
         }
        mtos_free(g_pNmClassic->vdoList);
        g_pNmClassic->vdoList = NULL;
    }
}

static void ui_release_classList(void)
{
    u16 i;

    if (g_pNmClassic->classList)
    {
        for (i = 0; i < g_pNmClassic->total_class; i++)
        {
            if (g_pNmClassic->classList[i].name)
            {
                mtos_free(g_pNmClassic->classList[i].name);
                g_pNmClassic->classList[i].name = NULL;
            }
            if (g_pNmClassic->classList[i].key)
            {
                mtos_free(g_pNmClassic->classList[i].key);
                g_pNmClassic->classList[i].key = NULL;
            }
        }
        mtos_free(g_pNmClassic->classList);
        g_pNmClassic->classList = NULL;
    }
}

static void ui_release_subclassList(void)
{
    u16 i;

    if (g_pNmClassic->subclaList != NULL)
    {
        for (i = 0; i < g_pNmClassic->total_subclass; i++)
        {
            if (g_pNmClassic->subclaList[i].name)
            {
                mtos_free(g_pNmClassic->subclaList[i].name);
                g_pNmClassic->subclaList[i].name = NULL;
            }
            if (g_pNmClassic->subclaList[i].key)
            {
                mtos_free(g_pNmClassic->subclaList[i].key);
                g_pNmClassic->subclaList[i].key = NULL;
            }
        }
        mtos_free(g_pNmClassic->subclaList);
        g_pNmClassic->subclaList = NULL;
    }
}

static void ui_clean_historylist(void)
{
    ui_subclass_context *p_context;
    u16 i;

    while (ui_link_list_delete(&g_pNmClassic->hHisList, 1, (void **)&p_context) == SUCCESS)
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

RET_CODE ui_nm_classic_edit_cb(u16* p_unistr)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(ROOT_ID_NM_CLASSIC);

    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_INPUT_COMP, 0, 0);
    }
	  return SUCCESS;
}

static control_t *ui_nm_classic_creat_vdo_list(control_t *p_client_cont)
{
    control_t *p_list_cont = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    u16 i;
    u16 x, y;
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_VDO_LIST_CONT,
                                  NM_CLASSIC_VDO_LIST_CONTX, NM_CLASSIC_VDO_LIST_CONTY,
                                  NM_CLASSIC_VDO_LIST_CONTW, NM_CLASSIC_VDO_LIST_CONTH,
                                  p_client_cont, 0);
    ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    x = NM_CLASSIC_VDO_LIST_ITEM_X;
    y = NM_CLASSIC_VDO_LIST_ITEM_Y;
    for (i = 0; i < NM_CLASSIC_VDO_NUM_ONE_PAGE; i++)
    {
OS_PRINTF("@@@ X = [%d], Y = [%d]\n", x, y);        
        p_item = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_VDO_ITEM_START + i,
                                x, y,
                                NM_CLASSIC_VDO_LIST_ITEM_W, NM_CLASSIC_VDO_LIST_ITEM_H,
                                p_list_cont, 0);
        ctrl_set_rstyle(p_item, RSI_NM_CLASSIC_FRM_SH, RSI_NM_CLASSIC_FRM_HL, RSI_NM_CLASSIC_FRM_SH);
  
        p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_CLASSIC_VDO_ITEM_PIC,
                                NM_CLASSIC_VDO_PIC_X, NM_CLASSIC_VDO_PIC_Y,
                                NM_CLASSIC_VDO_PIC_W, NM_CLASSIC_VDO_PIC_H,
                                p_item, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  
        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_CLASSIC_VDO_ITEM_NAME,
                                NM_CLASSIC_VDO_NAME_X, NM_CLASSIC_VDO_NAME_Y,
                                NM_CLASSIC_VDO_NAME_W, NM_CLASSIC_VDO_NAME_H,
                                p_item, 0);
  //      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  
        text_set_font_style(p_ctrl, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);        
        x += NM_CLASSIC_VDO_LIST_ITEM_W + NM_CLASSIC_VDO_LIST_ITEM_HGAP;
        if (x + NM_CLASSIC_VDO_PIC_W > NM_CLASSIC_VDO_LIST_CONTW)
        {
            x = NM_CLASSIC_VDO_LIST_ITEM_X;
            y += NM_CLASSIC_VDO_LIST_ITEM_H + NM_CLASSIC_VDO_LIST_ITEM_VGAP;
        }
    }

    return p_list_cont;
}

static RET_CODE ui_nm_classic_pic_play_start()
{
    control_t *p_item = NULL, *p_pic = NULL;
    rect_t rect;

    OS_PRINTF("@@@ui_nm_classic_pic_play_start curVdoListRendIdx=%d, vdo_cnt=%d\n", g_pNmClassic->curVdoListRendIdx, g_pNmClassic->vdo_cnt);
    if (g_pNmClassic->vdoList)
    {
        while (g_pNmClassic->curVdoListRendIdx < g_pNmClassic->vdo_cnt)
        {
            if(g_pNmClassic->vdoList[g_pNmClassic->curVdoListRendIdx].img_url)
            {
                if (strlen(g_pNmClassic->vdoList[g_pNmClassic->curVdoListRendIdx].img_url) > 0)
                {
                    p_item = ctrl_get_child_by_id(g_pNmClassic->hVdoList, IDC_NM_CLASSIC_VDO_ITEM_START + g_pNmClassic->curVdoListRendIdx);
                    p_pic = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);
    
                    ctrl_get_frame(p_pic, &rect);
                    ctrl_client2screen(p_pic, &rect);
    
                    ui_pic_play_by_url(g_pNmClassic->vdoList[g_pNmClassic->curVdoListRendIdx].img_url, &rect, 0);
                    return SUCCESS;
                }
            }
            g_pNmClassic->curVdoListRendIdx++;
        }
    }

    return ERR_NOFEATURE;
}

static RET_CODE ui_nm_classic_vdo_list_set_focus_pos(control_t *p_vdolist, u16 focus, BOOL is_paint)
{

  
    control_t *p_active_item = NULL, *p_active_pic = NULL;
    control_t *p_next_item = NULL, *p_next_pic = NULL;

    OS_PRINTF("@@@ui_iptv_vdolist_set_focus_pos focus=%d\n", focus);
    MT_ASSERT(focus < g_pNmClassic->vdo_page_size);

    p_active_item = p_vdolist->p_active_child;
    if (p_active_item != NULL)
    {
        p_active_pic = ctrl_get_child_by_id(p_active_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);
        MT_ASSERT(p_active_pic != NULL);
        ctrl_process_msg(p_active_item, MSG_LOSTFOCUS, 0, 0);
    }

    p_next_item = ctrl_get_child_by_id(p_vdolist, IDC_NM_CLASSIC_VDO_ITEM_START + focus);
    p_next_pic = ctrl_get_child_by_id(p_next_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);
    //MT_ASSERT(p_next_pic != NULL);
    ctrl_set_active_ctrl(p_next_item, p_next_pic);
    ctrl_set_attr(p_next_pic, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(p_next_pic, MSG_GETFOCUS, 0, 0);

    if (is_paint)
    {
        if (p_active_pic != NULL)
        {
            ctrl_paint_ctrl(p_active_item, TRUE);
        }
        if (p_next_pic != NULL)
        {
            ctrl_paint_ctrl(p_next_item, TRUE);
        }
    }

    return SUCCESS;
}

/*!
 * class list update function
 */
static RET_CODE nm_classic_update_class_list(control_t* p_class_list, u16 start, u16 size, u32 context)
{
    u16 i, j, cnt;
    u16 *p_unistr;
    cnt = list_get_count(p_class_list);
    OS_PRINTF("@@@%s,%d  cnt = [%d]\n",__FUNCTION__,__LINE__,cnt);

    for (i = start; i < (start + size) && i < (cnt - g_pNmClassic->ext_class); i++)
    {
        list_set_field_content_by_unistr(p_class_list, i, 0, g_pNmClassic->classList[i].name);
    }

    for (j = 0; j < g_pNmClassic->ext_class; j++)
    {
        p_unistr = (u16 *)gui_get_string_addr(class_list_ext[j].name);
        list_set_field_content_by_unistr(p_class_list, (i + j), 0, p_unistr);
    }

    return SUCCESS;
}

/*!
 * video list update function
 */

static RET_CODE nm_classic_update_vdo_list(control_t *p_vdolist, u16 count)
{
    control_t *p_item = NULL, *p_text = NULL;
    u16 i;

    OS_PRINTF("@@@nm_classic_update_vdolist count=%d\n", count);
    MT_ASSERT(count <= g_pNmClassic->vdo_page_size);

    if (g_pNmClassic->vdoList)
    {
        for (i = 0; i < count && i < g_pNmClassic->vdo_page_size; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_NM_CLASSIC_VDO_ITEM_START + i);
            p_text = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_NAME);
            text_set_content_by_extstr(p_text, g_pNmClassic->vdoList[i].title);
        }
    }

    return SUCCESS;
}

/*!
 * subclass list update function
 */
static RET_CODE nm_classic_update_subclass_list(control_t *p_subclass_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;

    cnt = g_pNmClassic->total_subclass;
    
    OS_PRINTF("@@@%s,%d  cnt = [%d]\n",__FUNCTION__,__LINE__,cnt);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_unistr(p_subclass_list, i, 0, g_pNmClassic->subclaList[i].name);
    }

    return SUCCESS;
}

/*!
 * option list update function
 */

static RET_CODE nm_classic_update_opt_list(control_t *p_opt_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    nm_simple_config_t config;
    
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    cnt = list_get_count(g_pNmClassic->hOptList);
    sys_status_get_nm_simple_config_info(&config);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(g_pNmClassic->hOptList, i, 0, g_resolution_opt[i].name);
        list_set_field_content_by_icon(g_pNmClassic->hOptList, i, 1, 
                                (g_resolution_opt[i].id == config.resolution) ? IM_TV_SELECT : RSC_INVALID_ID);
    }

    return SUCCESS;
}

static void nm_classic_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[21 + 1];

    sprintf(num, "%ld / %ld", page_num, total_page);
    text_set_content_by_ascstr(p_ctrl, num);
}

static void nm_classic_update_vdo_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[21 + 1];

    if (IS_MASKED(g_pNmClassic->attr, NETMEDIA_UI_ATTR_FASTPAGE))
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

static void nm_classic_update_curVdoPage(void)
{
    OS_PRINTF("@@@nm_classic_update_curVdoPage\n");
    if (ctrl_is_onfocus(g_pNmClassic->hVdoList))
    {
        ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, FALSE);
    }

    nm_classic_update_vdo_list(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);
}

/* to do show/hide child ctrl
    vdolist pre update
*/
static RET_CODE ui_nm_classic_vdo_list_set_count(control_t *p_vdo_list, u16 count)
{
    control_t *p_item = NULL, *p_pic = NULL, *p_text = NULL;
    rect_t rect;
    u16 i;

    OS_PRINTF("@@@ui_nm_classic_vdolist_set_count count=%d\n", count);
    MT_ASSERT(count <= g_pNmClassic->vdo_page_size);

    for (i = 0; i < count; i++)
    {
        p_item = ctrl_get_child_by_id(p_vdo_list, IDC_NM_CLASSIC_VDO_ITEM_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);
        p_text = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_NAME);

        ctrl_set_sts(p_item, OBJ_STS_SHOW);
        bmap_set_content_by_id(p_pic, IM_PIC_LOADING_1);
        text_set_content_by_extstr(p_text, NULL);
    }
    for (i = count; i < g_pNmClassic->vdo_page_size; i++)
    {
        p_item = ctrl_get_child_by_id(p_vdo_list, IDC_NM_CLASSIC_VDO_ITEM_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);

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
        g_pNmClassic->subclaList = p_subclaList;
        g_pNmClassic->total_subclass = p_data->class_cnt;
        g_pNmClassic->curSubClaListIdx = 0;

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
    
    OS_PRINTF("@@@%s,%d, is_vdolist == NULL[%d]\n\n\n",__FUNCTION__,__LINE__, (p_data->vdoList == NULL));
    
    if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
    {
        OS_PRINTF("@@@%s,%d, p_data->vdo_cnt[%d]\n\n\n",__FUNCTION__,__LINE__, p_data->vdo_cnt);
        vdo_cnt = MIN(p_data->vdo_cnt, g_pNmClassic->vdo_page_size);
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
        g_pNmClassic->vdoList = p_vdoList;
        
        if (g_pNmClassic->vdo_cnt != vdo_cnt)
        {
            OS_PRINTF("@@@calced vdo_cnt=%d, gived vdo_cnt=%d\n", g_pNmClassic->vdo_cnt, vdo_cnt);
            g_pNmClassic->vdo_cnt = vdo_cnt;
            //al_netmedia_set_page_size(g_pNmClassic->vdo_cnt);
            ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);
        }
        if (g_pNmClassic->curVdoPageNum == 1)
        {
            OS_PRINTF("@@@curVdoPageNum=%d\n", g_pNmClassic->curVdoPageNum);
            list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx);
        }
        else if (g_pNmClassic->curVdoListIdx >= vdo_cnt)
        {
            OS_PRINTF("@@@curVdoListIdx=%d, vdo_cnt=%d\n", g_pNmClassic->curVdoListIdx, vdo_cnt);
            g_pNmClassic->curVdoListIdx = vdo_cnt - 1;
            list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx);
        }
        if (g_pNmClassic->total_vdo != p_data->total_vdo)
        {
            OS_PRINTF("@@@g_pNmClassic->total_vdo=%d, p_data->total_vdo=%d\n", g_pNmClassic->total_vdo, p_data->total_vdo);
            OS_PRINTF("@@@g_pNmClassic->vdo_cnt=%d", g_pNmClassic->vdo_cnt);
            g_pNmClassic->total_vdo =  p_data->total_vdo;
            g_pNmClassic->totalVdoPage = (g_pNmClassic->total_vdo + g_pNmClassic->vdo_cnt - 1) / g_pNmClassic->vdo_cnt;
            nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);  
            ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
        }
        nm_classic_update_curVdoPage();
        ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
        
        return TRUE;
    }
    return FALSE;
}

static void ui_nm_classic_pic_init(void)
{
    ui_pic_init(PIC_SOURCE_NET);
}

static void ui_nm_classic_pic_deinit(void)
{
    pic_stop();
  
    ui_pic_release();
}


/*================================================================================================
                           nm classic sm function
 ================================================================================================*/

static BOOL StcClassic_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_CLASSIC_CLASS_LIST) ? TRUE : FALSE;
}

static BOOL StcClassic_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_CLASSIC_PAGE_NUM) ? TRUE : FALSE;
}

static BOOL StcClassic_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_CLASSIC_SUBCLASS_LIST) ? TRUE : FALSE;
}
static BOOL StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_CLASSIC_VDO_LIST_CONT) ? TRUE : FALSE;
}

static BOOL StcClassic_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_NM_CLASSIC_OPT_LIST) ? TRUE : FALSE;
}

static STATEID StaClassic_resolution_on_optlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    FOCUS_KEY_MAP(para1, msg);

    switch(msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(ctrl, msg, para1, para2);
            break;
        case MSG_FOCUS_LEFT:
            ctrl_change_focus(g_pNmClassic->hOptList, g_pNmClassic->hClaList);
            break;
    }

    return SID_NULL;
}   

static STATEID StaClassic_resolution_on_optlist_select(control_t *ctrl, u16 msg, u32 para1, u32 para2)
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

static STATEID StaClassic_resolution_on_optlist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);

    ctrl_process_msg(g_pNmClassic->hOptList, MSG_LOSTFOCUS, 0, 0);

    ctrl_set_attr(g_pNmClassic->hClaList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmClassic->hClaList, OBJ_STS_SHOW);
    ctrl_process_msg(g_pNmClassic->hClaList, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(g_pNmClassic->hClaList, TRUE);
    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    return SID_NULL;
}

static void SenClassic_Inactive(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static STATEID StaClassic_inactive_on_open_classic_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    return SID_CLASSIC_INIT;
}

static void SexClassic_Inactive(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenClassic_Active(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static STATEID StaClassic_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Classic_IsStateActive(SID_CLASSIC_DEINIT))
    {
        al_netmedia_website_deinit();
    }

    return SID_NULL;
}

static STATEID StaClassic_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Classic_IsStateActive(SID_CLASSIC_DEINIT))
    {
        return SID_CLASSIC_DEINIT;
    }
    else
    {
        return SID_NULL;
    }
}

static void SexClassic_Active(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

/* --------------------- Initialize:  ---------------------*/
static void SenClassic_Init(void)
{
    OS_PRINTF("@@@%s,%d,g_pNmClassic->nm_type==%d\n",__FUNCTION__,__LINE__,g_pNmClassic->nm_type);
    ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_LOADING_WITH_WAIT);
    al_netmedia_website_init(g_pNmClassic->nm_type);
}


static STATEID StaClassic_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    al_netmedia_set_page_size(g_pNmClassic->vdo_page_size);
    al_netmedia_get_class_list(); 
    return SID_NULL;
}

static STATEID StaClassic_init_on_newclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_class_list_t *p_data = (al_netmedia_class_list_t *)para2;
    al_netmedia_class_item_t *p_classList;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    if (p_data->class_cnt > 0 && p_data->classList != NULL)
    {
        g_pNmClassic->total_class = p_data->class_cnt;
        p_classList = (al_netmedia_class_item_t *)mtos_malloc(p_data->class_cnt * sizeof(al_netmedia_class_item_t));
        MT_ASSERT(p_classList != NULL);
        memset((void *)p_classList, 0, p_data->class_cnt * sizeof(al_netmedia_class_item_t));

        for (i = 0; i < g_pNmClassic->total_class; i++)
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
      
        g_pNmClassic->classList = p_classList;
    }
    else
    {
        OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
        return SID_CLASSIC_INIT_FAILED;
    }
  
    g_pNmClassic->curClassListIdx = 0;
    g_pNmClassic->curVdoPageNum = 1;

    //update class list
    list_set_count(g_pNmClassic->hClaList, g_pNmClassic->total_class + g_pNmClassic->ext_class, NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(g_pNmClassic->hClaList, g_pNmClassic->curClassListIdx);
    list_select_item(g_pNmClassic->hClaList, g_pNmClassic->curClassListIdx);

    nm_classic_update_class_list(g_pNmClassic->hClaList, list_get_valid_pos(g_pNmClassic->hClaList), NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE, 0);

    ctrl_paint_ctrl(g_pNmClassic->hClaList, TRUE);
  
    if(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].attr == NM_SUB_ATTR_CLASS)
    {
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_CLASS\n",__FUNCTION__,__LINE__);
        return SID_CLASSIC_SUBCLASS_REQ;
    }
    else if(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].attr == NM_SUB_ATTR_ITEM)
    {
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_ITEM\n",__FUNCTION__,__LINE__);
        return SID_CLASSIC_VDO_REQ;
    }
    
    return SID_NULL;
}

static void SexClassic_Init(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_comm_dlg_close();
}


static void SenClassic_Resource()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}


static STATEID StaClassic_resource_on_classlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{  
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    FOCUS_KEY_MAP(para1, msg);

    switch (msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(g_pNmClassic->hClaList, msg, para1, para2);
            break;

        case MSG_FOCUS_RIGHT:
            if (ctrl_get_sts(g_pNmClassic->hVdoList) == OBJ_STS_SHOW)
            {
              ctrl_change_focus(g_pNmClassic->hClaList, g_pNmClassic->hVdoList);
              ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else if (ctrl_get_sts(g_pNmClassic->hSubClaList) == OBJ_STS_SHOW)
            {
              ctrl_change_focus(g_pNmClassic->hClaList, g_pNmClassic->hSubClaList);
            }
            else if (ctrl_get_sts(g_pNmClassic->hOptList) == OBJ_STS_SHOW)
            {
              ctrl_change_focus(g_pNmClassic->hClaList, g_pNmClassic->hOptList);
            }
            break;

        default:
            break;
    }
  
    return SID_NULL;
}

static STATEID StaClassic_resource_on_classlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    ui_release_subclassList();
    g_pNmClassic->total_subclass = 0;
    g_pNmClassic->curSubClaListIdx = 0;
    ui_clean_historylist();
    
    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    OS_PRINTF("@@@StaClassic_resource_on_classlist_selected focus=%d\n", focus);

    g_pNmClassic->curClassListIdx= focus;
    ctrl_paint_ctrl(ctrl, TRUE);
    
    if (focus == g_pNmClassic->total_class)
    {
        return  SID_CLASSIC_RESOLUTION;
    }
    
    if (g_pNmClassic->classList[g_pNmClassic->curClassListIdx].attr == NM_SUB_ATTR_ITEM)
    {
        return SID_CLASSIC_VDO;
    }
    else if (g_pNmClassic->classList[g_pNmClassic->curClassListIdx].attr == NM_SUB_ATTR_CLASS)
    {
        return SID_CLASSIC_SUBCLASS;
    }

    return SID_NULL;
}

static STATEID StaClassic_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
  
    ui_nm_classic_pic_init();
    
    g_pNmClassic->curVdoListRendIdx = 0;
    ui_nm_classic_pic_play_start();
    
    return SID_NULL;
}

static void SexClassic_Resource()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenClassic_Subclass()
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    //ctrl_set_attr(g_pNmClassic->hSubClaList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmClassic->hSubClaList, OBJ_STS_SHOW);

    ui_comm_help_set_data(&comm_help_data, g_pNmClassic->hHelpCont);
    ctrl_paint_ctrl(g_pNmClassic->hHelpCont, TRUE);
}

static STATEID StaClassic_subclass_on_subclasslist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 page_num;

    FOCUS_KEY_MAP(para1, msg);

    OS_PRINTF("@@@%s,%d,msg==%d\n", __FUNCTION__,__LINE__,msg);

    switch(msg)
    {
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
            list_class_proc(ctrl, msg, para1, para2);
            ctrl_paint_ctrl(g_pNmClassic->hSubClaList, TRUE);
            g_pNmClassic->curSubClaListIdx = list_get_focus_pos(ctrl);
            page_num = g_pNmClassic->curSubClaListIdx / NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE + 1;
            if (g_pNmClassic->curSubClaPageNum != page_num)
            {
                g_pNmClassic->curSubClaPageNum = page_num;
                nm_classic_update_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curSubClaPageNum, g_pNmClassic->totalSubClaPage);
                ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
            }
            break;
        case MSG_FOCUS_LEFT:
            if (list_get_count(g_pNmClassic->hSubClaList) > 0)
            {
              ctrl_change_focus(g_pNmClassic->hSubClaList, g_pNmClassic->hClaList);
            }
            break;
    }
  
    return SID_NULL;
}

static STATEID StaClassic_subclass_on_subclasslist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_subclass_context *p_ctx;

   OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_link_list_get_size(&g_pNmClassic->hHisList) <= 1)
    {
        ctrl_change_focus(g_pNmClassic->hSubClaList, g_pNmClassic->hClaList);
    }
    else
    {
        ui_release_subclassList();
        g_pNmClassic->total_subclass = 0;

        ui_link_list_delete(&g_pNmClassic->hHisList, 1, (void **)&p_ctx);
        g_pNmClassic->subclaList = p_ctx->subclaList;
        g_pNmClassic->total_subclass = p_ctx->total_subclass;
        g_pNmClassic->curSubClaListIdx = p_ctx->curSubClaListIdx;
    }

    return SID_NULL;
}

static void SexClassic_Subclass(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ctrl_set_sts(g_pNmClassic->hSubClaList, OBJ_STS_HIDE);
}

static void SenClassic_SubclassReq(void)
{ 
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (g_pNmClassic->total_subclass == 0)
    {
        list_set_count(g_pNmClassic->hSubClaList, g_pNmClassic->total_subclass, NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE);
        ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    }

    if(g_pNmClassic->subclaList == NULL)
    {
        OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
        al_netmedia_get_subclass_list(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].id,
                                                       g_pNmClassic->classList[g_pNmClassic->curClassListIdx].key);
    }
    else
    {
        al_netmedia_get_subclass_list(g_pNmClassic->subclaList[g_pNmClassic->curSubClaListIdx].id,
                                                       g_pNmClassic->subclaList[g_pNmClassic->curSubClaListIdx].key);
    }
    ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_LOADING_WITH_WAIT);
}

static STATEID StaClassic_subclass_on_newsubclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_subclass_context *p_new_ctx;

    OS_PRINTF("@@@%s, %d\n", __FUNCTION__, __LINE__);
    if (g_pNmClassic->subclaList != NULL)
    {
        p_new_ctx = (ui_subclass_context *)mtos_malloc(sizeof(ui_subclass_context));
        MT_ASSERT(p_new_ctx != NULL);
        p_new_ctx->subclaList = g_pNmClassic->subclaList;
        p_new_ctx->total_subclass = g_pNmClassic->total_subclass;
        p_new_ctx->curSubClaListIdx = g_pNmClassic->curSubClaListIdx;

        ui_link_list_insert(&g_pNmClassic->hHisList, 1, p_new_ctx);
    }

    if (ui_load_new_page_subclass((al_netmedia_class_list_t *)para2))
    {
        return SID_CLASSIC_SUBCLASS_LIST;
    }

    g_pNmClassic->subclaList = NULL;
    g_pNmClassic->total_subclass = 0;
    g_pNmClassic->curSubClaListIdx = 0;
    ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_NO_DATA);
    return SID_NULL;
}

static STATEID StaClassic_subclass_on_get_subclasslist_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);
    return SID_NULL;
}

static void SexClassic_SubclassReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenClassic_SubclassList(void)
{
    OS_PRINTF("@@@%s, %d, g_pNmClassic->total_subclass==%d,g_pNmClassic->curSubClaListIdx==%d\n", __FUNCTION__, __LINE__, g_pNmClassic->total_subclass,g_pNmClassic->curSubClaListIdx);
    list_set_count(g_pNmClassic->hSubClaList, g_pNmClassic->total_subclass, NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE);
    list_set_focus_pos(g_pNmClassic->hSubClaList, g_pNmClassic->curSubClaListIdx);
    nm_classic_update_subclass_list(g_pNmClassic->hSubClaList, list_get_valid_pos(g_pNmClassic->hSubClaList), NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE, 0);
    ctrl_paint_ctrl(g_pNmClassic->hSubClaList, TRUE);

    g_pNmClassic->totalSubClaPage = (g_pNmClassic->total_subclass > 0) 
                                                  ? (g_pNmClassic->total_subclass + NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE - 1) / NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE 
                                                  : 1;
    g_pNmClassic->curSubClaPageNum = g_pNmClassic->curSubClaListIdx / NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE + 1;
    nm_classic_update_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curSubClaPageNum, g_pNmClassic->totalSubClaPage);

    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
}

static STATEID StaClassic_subclass_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s curSubClaListIdx=%d\n", __FUNCTION__, g_pNmClassic->curSubClaListIdx);

    PAGE_KEY_MAP(para1,msg);

    list_class_proc(ctrl, msg, para1, para2);
    g_pNmClassic->curSubClaListIdx = list_get_focus_pos(ctrl);

    g_pNmClassic->curSubClaPageNum = g_pNmClassic->curSubClaListIdx / NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE + 1;
    nm_classic_update_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curSubClaPageNum, g_pNmClassic->totalSubClaPage);
    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);

    return SID_NULL;
}

static STATEID StaClassic_subclass_on_subclasslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
   OS_PRINTF("@@@StaClassic_subclass_on_subclasslist_selected\n");
    if (g_pNmClassic->subclaList[g_pNmClassic->curSubClaListIdx].attr == NM_SUB_ATTR_CLASS)
    {
        g_pNmClassic->curCatePageNum = 1;
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_CLASS\n",__FUNCTION__,__LINE__);
        return SID_CLASSIC_SUBCLASS_REQ;
    }
    else if(g_pNmClassic->subclaList[g_pNmClassic->curSubClaListIdx].attr == NM_SUB_ATTR_ITEM)
    {
        g_pNmClassic->curVdoPageNum = 1;
        ctrl_process_msg(g_pNmClassic->hSubClaList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_attr(g_pNmClassic->hVdoList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(g_pNmClassic->hVdoList, MSG_GETFOCUS, 0, 0);
                
        OS_PRINTF("@@@%s,%d,NM_SUB_ATTR_ITEM\n",__FUNCTION__,__LINE__);
        return SID_CLASSIC_VDO_REQ;
    }

    return SID_NULL;  
}

static STATEID StaClassic_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    kb_param_t param;

    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (IS_MASKED(g_pNmClassic->attr, NETMEDIA_UI_ATTR_SEARCH))
    {
        param.uni_str = g_pNmClassic->g_searchKey;
        param.type = KB_INPUT_TYPE_SENTENCE;
        param.max_len = MAX_SEARCH_NAME_LEN;
        param.cb = ui_nm_classic_edit_cb;
        manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    }

    return SID_NULL;
}

static STATEID StaClassic_subclass_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (ctrl_is_onfocus(g_pNmClassic->hSubClaList))
    {
        ctrl_process_msg(g_pNmClassic->hSubClaList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_attr(g_pNmClassic->hVdoList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(g_pNmClassic->hVdoList, MSG_GETFOCUS, 0, 0);
    }

    al_netmedia_set_search_keyword(g_pNmClassic->g_searchKey);

    return SID_NULL;
}

static void SexClassic_SubclassList(void)
{
     if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }
}

static void SenClassic_Vdo(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    
    g_pNmClassic->total_vdo = 0;
    g_pNmClassic->totalVdoPage = 1;
    g_pNmClassic->curVdoPageNum = 1;
    g_pNmClassic->curVdoListIdx = 0;

    //ctrl_set_attr(g_pNmClassic->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmClassic->hVdoList, OBJ_STS_SHOW);
    
    ui_comm_help_set_data(&vdo_help_data, g_pNmClassic->hHelpCont);
    ctrl_paint_ctrl(g_pNmClassic->hHelpCont, TRUE);
}

static STATEID StaClassic_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);

    OS_PRINTF("@@@%s curVdoListIdx=%d\n", __FUNCTION__, g_pNmClassic->curVdoListIdx);
    switch(msg)
    {
        case MSG_FOCUS_UP:
            if (g_pNmClassic->curVdoListIdx >= NM_CLASSIC_VDO_ITEM_COL)
            {
                g_pNmClassic->curVdoListIdx -= NM_CLASSIC_VDO_ITEM_COL;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum > 1)
                {
                    g_pNmClassic->curVdoPageNum--;
                    return SID_CLASSIC_VDO_REQ;
                }
            }
            break;

        case MSG_FOCUS_DOWN:
            if (g_pNmClassic->curVdoListIdx < NM_CLASSIC_VDO_ITEM_COL
              && g_pNmClassic->curVdoListIdx + NM_CLASSIC_VDO_ITEM_COL < g_pNmClassic->vdo_cnt)
            {
                g_pNmClassic->curVdoListIdx += NM_CLASSIC_VDO_ITEM_COL;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
                {
                    g_pNmClassic->curVdoPageNum++;
                    return SID_CLASSIC_VDO_REQ;
                }
            }
            break;

        case MSG_FOCUS_LEFT:
            if (g_pNmClassic->curVdoListIdx % NM_CLASSIC_VDO_ITEM_COL != 0)
            {
                g_pNmClassic->curVdoListIdx--;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                ctrl_change_focus(g_pNmClassic->hVdoList, g_pNmClassic->hClaList);
            }
            break;

        case MSG_FOCUS_RIGHT:
            if (g_pNmClassic->curVdoListIdx + 1 < g_pNmClassic->vdo_cnt)
            {
                g_pNmClassic->curVdoListIdx++;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
                {
                    g_pNmClassic->curVdoPageNum++;
                    return SID_CLASSIC_VDO_REQ;
                }
            }
            break;
    }
    
    OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmClassic->curVdoListIdx);
    return SID_NULL;
}

static STATEID  StaClassic_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    PAGE_KEY_MAP(para1, msg);

    OS_PRINTF("@@@StaClassic_vdo_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_pNmClassic->curVdoPageNum > 1)
        {
            g_pNmClassic->curVdoPageNum--;
            return SID_CLASSIC_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
        {
            g_pNmClassic->curVdoPageNum++;
            return SID_CLASSIC_VDO_REQ;
        }
    }
    return SID_NULL;
}

static STATEID StaClassic_vdo_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);

    if(g_pNmClassic->subclaList == NULL)
    {
        ctrl_change_focus(g_pNmClassic->hVdoList, g_pNmClassic->hClaList);
        return SID_NULL;
    }
    else
    {
        ctrl_process_msg(g_pNmClassic->hVdoList, MSG_LOSTFOCUS, 0, 0);
        ctrl_set_attr(g_pNmClassic->hSubClaList, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(g_pNmClassic->hSubClaList, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(g_pNmClassic->hVdoList,TRUE);
        ctrl_paint_ctrl(g_pNmClassic->hSubClaList,TRUE);
        return SID_CLASSIC_SUBCLASS_LIST;
    }
}

static void SexClassic_Vdo(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_set_sts(g_pNmClassic->hVdoList, OBJ_STS_HIDE);
}

static void SenClassic_VdoReq(void)
{      
    OS_PRINTF("@@@%s,%d,g_pNmClassic->curVdoPageNum=%d\n",__FUNCTION__,__LINE__,g_pNmClassic->curVdoPageNum);
    
    ui_release_vdolist();

    if (g_pNmClassic->total_vdo == 0)
    {
        g_pNmClassic->vdo_cnt = 0;
    }
    else
    {
        g_pNmClassic->vdo_cnt = (u16)((g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage) 
                                      ? g_pNmClassic->vdo_page_size 
                                      : (g_pNmClassic->total_vdo - (g_pNmClassic->totalVdoPage - 1) * g_pNmClassic->vdo_page_size));
    }
    
    ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);

    nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);

    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);

    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    
   if(g_pNmClassic->subclaList == NULL)
    {
        OS_PRINTF("\n\n@@@@al_netmedia_get_video_list@@@@\n\n");
        al_netmedia_get_video_list(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].id,
                                                 g_pNmClassic->classList[g_pNmClassic->curClassListIdx].key,
                                                 0,
                                                 NULL, 
                                                 g_pNmClassic->curVdoPageNum);
   }
    else
    {
       al_netmedia_get_video_list(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].id,
                                                 g_pNmClassic->classList[g_pNmClassic->curClassListIdx].key,
                                                 g_pNmClassic->subclaList[g_pNmClassic->curSubClaListIdx].id,
                                                 g_pNmClassic->subclaList [g_pNmClassic->curSubClaListIdx].key, 
                                                 g_pNmClassic->curVdoPageNum);
    }
     ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_LOADING_WITH_WAIT);
}

static STATEID StaClassic_vdo_on_newpagevdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_vdo_list_t *p_data = (al_netmedia_vdo_list_t *)para2;
    al_netmedia_vdo_item_t *p_vdoList;
    u8 *p_id,*p_key,*p_url;
    u16 *p_title,*p_date,*p_duration,*p_description;
    u16 vdo_cnt;
    u16 i;
    u16 str_len;
    
    OS_PRINTF("@@@%s,%d, is_vdolist == NULL[%d]\n\n\n",__FUNCTION__,__LINE__, (p_data->vdoList == NULL));
    
    if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
    {
        OS_PRINTF("@@@%s,%d, p_data->vdo_cnt[%d]\n\n\n",__FUNCTION__,__LINE__, p_data->vdo_cnt);
        vdo_cnt = MIN(p_data->vdo_cnt, g_pNmClassic->vdo_page_size);
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
        g_pNmClassic->vdoList = p_vdoList;
        g_pNmClassic->curVdoListIdx = 0;
    }
    else
    {
        vdo_cnt = 0;
        g_pNmClassic->vdoList = NULL;
        g_pNmClassic->curVdoListIdx = 0;
    }

    if (g_pNmClassic->vdo_cnt != vdo_cnt)
    {
        OS_PRINTF("@@@calced vdo_cnt=%d, gived vdo_cnt=%d\n", g_pNmClassic->vdo_cnt, vdo_cnt);
        g_pNmClassic->vdo_cnt = vdo_cnt;
        //al_netmedia_set_page_size(g_pNmClassic->vdo_cnt);
        ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);
    }
    if (g_pNmClassic->total_vdo != p_data->total_vdo)
    {
        OS_PRINTF("@@@g_pNmClassic->total_vdo=%d, p_data->total_vdo=%d\n", g_pNmClassic->total_vdo, p_data->total_vdo);
        OS_PRINTF("@@@g_pNmClassic->vdo_cnt=%d", g_pNmClassic->vdo_cnt);
        g_pNmClassic->total_vdo =  p_data->total_vdo;
        g_pNmClassic->totalVdoPage = (g_pNmClassic->total_vdo + g_pNmClassic->vdo_cnt - 1) / g_pNmClassic->vdo_cnt;
        nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);  
        ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    }
    nm_classic_update_curVdoPage();
    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    
    return SID_NULL;
}

static STATEID StaClassic_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (g_pNmClassic->curVdoPageNum == 1)
    {
        g_pNmClassic->vdo_cnt = 0;
        g_pNmClassic->totalVdoPage = 1;
        
        nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);
        ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);
        ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);
    }
    else
    {
        ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);
    }
    return SID_NULL;
}

static void SexClassic_VdoReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenClassic_VdoList(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    nm_classic_update_curVdoPage();
    ctrl_paint_ctrl(g_pNmClassic->hVdoList, TRUE);

    g_pNmClassic->curVdoListRendIdx = 0;
    ui_nm_classic_pic_play_start();
}

static STATEID  StaClassic_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_item = NULL, *p_pic = NULL;

    OS_PRINTF("@@@%s\n",__FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_NM_CLASSIC)
    {
        p_item = ctrl_get_child_by_id(g_pNmClassic->hVdoList, IDC_NM_CLASSIC_VDO_ITEM_START + g_pNmClassic->curVdoListRendIdx);
        p_pic = ctrl_get_child_by_id(p_item, IDC_NM_CLASSIC_VDO_ITEM_PIC);
        bmap_set_content_by_id(p_pic, RSC_INVALID_ID);

        ctrl_paint_ctrl(p_pic, TRUE);

        pic_stop();
        g_pNmClassic->curVdoListRendIdx++;
        ui_nm_classic_pic_play_start();
    }

    return SID_NULL;
}

static STATEID  StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_NM_CLASSIC)
    {
        pic_stop();
        g_pNmClassic->curVdoListRendIdx++;
        ui_nm_classic_pic_play_start();
    }

    return SID_NULL;
}

static STATEID  StaClassic_vdo_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if ((g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].id != NULL)
                      && (g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].key != NULL))
    {
        OS_PRINTF("@@@SID_CLASSIC_VDO_URL_REQ\n");
        return SID_CLASSIC_VDO_URL_REQ;
    }

    return SID_NULL;
}

static STATEID StaClassic_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ctrl_is_onfocus(g_pNmClassic->hVdoList))
    {
        g_pNmClassic->jumpVdoPageNum = para1;
        if (g_pNmClassic->jumpVdoPageNum > g_pNmClassic->totalVdoPage)
        {
            g_pNmClassic->jumpVdoPageNum = g_pNmClassic->totalVdoPage;
        }
        nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, 
                                                            g_pNmClassic->jumpVdoPageNum, 
                                                            g_pNmClassic->totalVdoPage);
        ctrl_change_focus(g_pNmClassic->hVdoList, g_pNmClassic->hPageNum);
    }
    else
    {
        if (g_pNmClassic->jumpVdoPageNum < g_pNmClassic->totalVdoPage)
        {
            g_pNmClassic->jumpVdoPageNum = g_pNmClassic->jumpVdoPageNum * 10 + para1;
        }
        else
        {
            g_pNmClassic->jumpVdoPageNum = para1;
        }
        if (g_pNmClassic->jumpVdoPageNum > g_pNmClassic->totalVdoPage)
        {
            g_pNmClassic->jumpVdoPageNum = g_pNmClassic->totalVdoPage;
        }
        nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, 
                                                            g_pNmClassic->jumpVdoPageNum, 
                                                            g_pNmClassic->totalVdoPage);
        ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    }

    return SID_NULL;
}

static STATEID StaClassic_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pNmClassic->hPageNum, g_pNmClassic->hVdoList);

    if(g_pNmClassic->jumpVdoPageNum == 0)
    {
       g_pNmClassic->jumpVdoPageNum = 1;
       nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, 
                                                   g_pNmClassic->jumpVdoPageNum, 
                                                   g_pNmClassic->totalVdoPage);
       ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    }
    g_pNmClassic->curVdoPageNum = g_pNmClassic->jumpVdoPageNum;

    return SID_NULL;
}

static STATEID StaClassic_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (g_pNmClassic->curVdoPageNum != g_pNmClassic->jumpVdoPageNum)
    {
        nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, 
                                                            g_pNmClassic->curVdoPageNum, 
                                                            g_pNmClassic->totalVdoPage);
    }

    ctrl_change_focus(g_pNmClassic->hPageNum, g_pNmClassic->hVdoList);

    return SID_NULL;
}

static STATEID StaClassic_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    al_netmedia_set_search_keyword(g_pNmClassic->g_searchKey);

    return SID_NULL;
}

static void SexClassic_VdoList(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }
    pic_stop();
}

static void SenClassic_VdoUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_MSG_GET_URL);
    //get url
    al_netmedia_get_play_urls(g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].id,g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].key);
}

static STATEID StaClassic_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_play_url_list_t *p_data = (al_netmedia_play_url_list_t *)para2;
    //al_netmedia_play_url_item_t *p_playurlList;
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
            ui_nm_classic_pic_deinit();

            memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));
            vdoPlayerParam.name = g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].title;
            vdoPlayerParam.url_cnt = p_url->fragment_url_cnt;
            vdoPlayerParam.pp_urlList = p_url->pp_fragmntUrlList;
            manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_NM_CLASSIC, (u32)&vdoPlayerParam);

            return SID_NULL;
        }
    }

    ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);

    return SID_NULL;
}

static STATEID StaClassic_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    return SID_NULL;
}

static void SexClassic_VdoUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenClassic_Search(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    
    g_pNmClassic->total_vdo = 0;
    g_pNmClassic->vdo_cnt = 0;
    g_pNmClassic->totalVdoPage = 1;
    g_pNmClassic->curVdoPageNum = 1;
    g_pNmClassic->curVdoListIdx = 0;
    ui_release_vdolist();

    //ctrl_set_attr(g_pNmClassic->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmClassic->hVdoList, OBJ_STS_SHOW);
    ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);
    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);
    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    
    ui_comm_help_set_data(&vdo_help_data, g_pNmClassic->hHelpCont);
    ctrl_paint_ctrl(g_pNmClassic->hHelpCont, TRUE);
}

static STATEID StaClassic_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);

    OS_PRINTF("@@@%s curVdoListIdx=%d\n", __FUNCTION__, g_pNmClassic->curVdoListIdx);
    switch(msg)
    {
        case MSG_FOCUS_UP:
            if (g_pNmClassic->curVdoListIdx >= NM_CLASSIC_VDO_ITEM_COL)
            {
                g_pNmClassic->curVdoListIdx -= NM_CLASSIC_VDO_ITEM_COL;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum > 1)
                {
                    g_pNmClassic->curVdoPageNum--;
                    return SID_CLASSIC_SEARCH_VDO_REQ;
                }
            }
            break;

        case MSG_FOCUS_DOWN:
            if (g_pNmClassic->curVdoListIdx < NM_CLASSIC_VDO_ITEM_COL
              && g_pNmClassic->curVdoListIdx + NM_CLASSIC_VDO_ITEM_COL < g_pNmClassic->vdo_cnt)
            {
                g_pNmClassic->curVdoListIdx += NM_CLASSIC_VDO_ITEM_COL;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
                {
                    g_pNmClassic->curVdoPageNum++;
                    return SID_CLASSIC_SEARCH_VDO_REQ;
                }
            }
            break;

        case MSG_FOCUS_LEFT:
            if (g_pNmClassic->curVdoListIdx % NM_CLASSIC_VDO_ITEM_COL != 0)
            {
                g_pNmClassic->curVdoListIdx--;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                ctrl_change_focus(g_pNmClassic->hVdoList, g_pNmClassic->hClaList);
            }
            break;

        case MSG_FOCUS_RIGHT:
            if (g_pNmClassic->curVdoListIdx + 1 < g_pNmClassic->vdo_cnt)
            {
                g_pNmClassic->curVdoListIdx++;
                ui_nm_classic_vdo_list_set_focus_pos(g_pNmClassic->hVdoList, g_pNmClassic->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
                {
                    g_pNmClassic->curVdoPageNum++;
                    return SID_CLASSIC_SEARCH_VDO_REQ;
                }
            }
            break;
    }
    
    OS_PRINTF("@@@curVdoListIdx=%d\n", g_pNmClassic->curVdoListIdx);
    return SID_NULL;
}

static STATEID  StaClassic_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{

    PAGE_KEY_MAP(para1, msg);

    OS_PRINTF("@@@StaClassic_search_vdo_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_pNmClassic->curVdoPageNum > 1)
        {
            g_pNmClassic->curVdoPageNum--;
            return SID_CLASSIC_SEARCH_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage)
        {
            g_pNmClassic->curVdoPageNum++;
            return SID_CLASSIC_SEARCH_VDO_REQ;
        }
    }
    return SID_NULL;
}

static STATEID StaClassic_search_on_newsearchvdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ui_load_new_page_vdo((al_netmedia_vdo_list_t *)para2))
    {
        return SID_CLASSIC_SEARCH_VDO_LIST;
    }
    if (g_pNmClassic->curVdoPageNum == 1)
    {
        ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_NO_DATA);
    }
    else
    {
        ui_nm_classic_open_cfm_dlg(ROOT_ID_NM_CLASSIC, IDS_DATA_ERROR);
    }
    return SID_NULL;
}

static STATEID StaClassic_search_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pNmClassic->hVdoList, g_pNmClassic->hClaList);
    return SID_NULL;
}

static void SexClassic_Search(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ctrl_set_sts(g_pNmClassic->hVdoList, OBJ_STS_HIDE);
}

static void SenClassic_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    ui_release_vdolist();

    if (g_pNmClassic->total_vdo == 0)
    {
        g_pNmClassic->vdo_cnt = 0;
    }
    else
    {
        g_pNmClassic->vdo_cnt = (u16)((g_pNmClassic->curVdoPageNum < g_pNmClassic->totalVdoPage) 
                                      ? g_pNmClassic->vdo_page_size 
                                      : (g_pNmClassic->total_vdo - (g_pNmClassic->totalVdoPage - 1) * g_pNmClassic->vdo_page_size));
    }
    
    ui_nm_classic_vdo_list_set_count(g_pNmClassic->hVdoList, g_pNmClassic->vdo_cnt);

    nm_classic_update_vdo_page_num(g_pNmClassic->hPageNum, g_pNmClassic->curVdoPageNum, g_pNmClassic->totalVdoPage);

    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);

    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    
    al_netmedia_search(g_pNmClassic->classList[g_pNmClassic->curClassListIdx].id, g_pNmClassic->curVdoPageNum);

    ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_LOADING_WITH_WAIT);
}

static void SexClassic_SearchVdoReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenClassic_SearchVdoList(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    nm_classic_update_curVdoPage();
    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);

    g_pNmClassic->curVdoListRendIdx = 0;
    ui_nm_classic_pic_play_start();
}

static void SexClassic_SearchVdoList(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    if (fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
    {
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
    }

    pic_stop();
}

static STATEID StaClassic_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if ((g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].id != NULL)
                      && (g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].key != NULL))
    {
        OS_PRINTF("@@@SID_CLASSIC_SEARCH_URL_REQ\n");
        return SID_CLASSIC_SEARCH_URL_REQ;
    }

    return SID_NULL;
}
static void SenClassic_SearchUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_classic_open_dlg(ROOT_ID_NM_CLASSIC, IDS_MSG_GET_URL);
    //get url
    al_netmedia_get_play_urls(g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].id,g_pNmClassic->vdoList[g_pNmClassic->curVdoListIdx].key);
}

static void SexClassic_SearchUrlReq(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    if (fw_find_root_by_id(ROOT_ID_POPUP))
    {
        ui_comm_dlg_close();
    }
}

static void SenClassic_Resolution(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    list_set_count(g_pNmClassic->hOptList, ARRAY_SIZE(g_resolution_opt), NM_CLASSIC_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(g_pNmClassic->hOptList, 0);

   // ctrl_set_attr(g_pNmClassic->hOptList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pNmClassic->hOptList, OBJ_STS_SHOW);

    nm_classic_update_page_num(g_pNmClassic->hPageNum, 1, 1);
    nm_classic_update_opt_list(g_pNmClassic->hOptList, list_get_valid_pos(g_pNmClassic->hOptList), NM_CLASSIC_OPT_LIST_ITEM_NUM_ONE_PAGE, 0);

    ui_comm_help_set_data(&comm_help_data, g_pNmClassic->hHelpCont);

    ctrl_paint_ctrl(g_pNmClassic->hPageNum, TRUE);
    ctrl_paint_ctrl(g_pNmClassic->hClientCont, TRUE);
    ctrl_paint_ctrl(g_pNmClassic->hHelpCont, TRUE);
}

static void SexClassic_Resolution(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ctrl_set_sts(g_pNmClassic->hOptList, OBJ_STS_HIDE);
}

static void SenClassic_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SexClassic_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenClassic_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    al_netmedia_website_deinit();
}

static STATEID StaClassic_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_NM_CLASSIC);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    return SID_NULL;
}  

static void SexClassic_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

/*================================================================================================
                           nm classic public works function
 ================================================================================================*/

RET_CODE ui_open_nm_classic(u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_bmp;
    control_t *p_list_cont, *p_list;
    control_t *p_page_cont, *p_icon, *p_page_num;
    control_t *p_client_cont, *p_vdoList;
    control_t *p_subclass_list;
    control_t *p_optlist;
    control_t *p_bottom_help;
    u8 i = 0;

    OS_PRINTF("@@@ui_open_nm_classic\n");

    ui_nm_classic_pic_init();

    ui_init_nm_classic_data();
    g_pNmClassic->attr = (NETMEDIA_UI_ATTR_E)para2;

    g_pNmClassic->nm_type = (NETMEDIA_DP_TYPE)para1;
    if (IS_MASKED(para2, NETMEDIA_UI_ATTR_SEARCH))
    {
        g_pNmClassic->ext_class = ARRAY_SIZE(class_list_ext);
    }
    else
    {
        g_pNmClassic->ext_class = ARRAY_SIZE(class_list_ext) - 1;
    }

    memset(g_pNmClassic->g_searchKey,0,sizeof(g_pNmClassic->g_searchKey));
    
    g_pNmClassic->vdo_page_size = NM_CLASSIC_VDO_NUM_ONE_PAGE;

    for (i = 0; i < ARRAY_SIZE(g_dp_type_rsc); i++)
    {
        if (g_pNmClassic->nm_type == g_dp_type_rsc[i].nm_type)
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
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_NM_CLASSIC, 0,
                                      IM_INDEX_DEFAULT_BANNER, g_dp_type_rsc[i].str_id);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, nm_classic_cont_keymap);
    ctrl_set_proc(p_menu, nm_classic_cont_proc);

OS_PRINTF("@@@ line = [%d]\n",__LINE__);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_BTM_HELP,
                                      NM_CLASSIC_BOTTOM_HELP_X, NM_CLASSIC_BOTTOM_HELP_Y,
                                      NM_CLASSIC_BOTTOM_HELP_W, NM_CLASSIC_BOTTOM_HELP_H,
                                      p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    ui_comm_help_create_ext(60, 0, 
                                NM_CLASSIC_BOTTOM_HELP_W-60, NM_CLASSIC_BOTTOM_HELP_H,  
                                &emp_help_data,  p_bottom_help);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    /*!
     * Create list
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_LIST_CONT,
                                  NM_CLASSIC_CLASS_LIST_CONTX, NM_CLASSIC_CLASS_LIST_CONTY,
                                  NM_CLASSIC_CLASS_LIST_CONTW, NM_CLASSIC_CLASS_LIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //create bmp
    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_CLASSIC_LEFT_BMP, 
                                        NM_CLASSIC_LOGO_X, NM_CLASSIC_LOGO_Y, 
                                        NM_CLASSIC_LOGO_W, NM_CLASSIC_LOGO_H, 
                                        p_list_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, g_dp_type_rsc[i].bmp_id);

 
    //class list
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_CLASSIC_CLASS_LIST,
                          NM_CLASSIC_CLASS_LIST_X, NM_CLASSIC_CLASS_LIST_Y,
                          NM_CLASSIC_CLASS_LIST_W, NM_CLASSIC_CLASS_LIST_H,
                          p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, nm_classic_class_list_keymap);
    ctrl_set_proc(p_list, nm_classic_class_list_proc);

    ctrl_set_mrect(p_list, NM_CLASSIC_CLASS_LIST_MIDL, NM_CLASSIC_CLASS_LIST_MIDT,
                           NM_CLASSIC_CLASS_LIST_MIDW + NM_CLASSIC_CLASS_LIST_MIDL, 
                           NM_CLASSIC_CLASS_LIST_MIDH + NM_CLASSIC_CLASS_LIST_MIDT);
    list_set_item_interval(p_list, NM_CLASSIC_CLASS_LIST_ICON_VGAP);
    list_set_item_rstyle(p_list, &class_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, 0, NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    //list_set_count(p_list, NM_CLASSIC_CLASS_LIST_ITEM_COUNT, NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(class_list_attr), NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_list, nm_classic_update_class_list, 0);

    for (i = 0; i < ARRAY_SIZE(class_list_attr); i++)
    {
        list_set_field_attr(p_list, i, class_list_attr[i].attr,
                            class_list_attr[i].width, class_list_attr[i].left, class_list_attr[i].top);
        list_set_field_rect_style(p_list, i, class_list_attr[i].rstyle);
        list_set_field_font_style(p_list, i, class_list_attr[i].fstyle);
    }

        //Search help bar
    if (IS_MASKED(g_pNmClassic->attr, NETMEDIA_UI_ATTR_SEARCH))
    {
        ui_comm_help_create_ext(NM_CLASSIC_SEARCH_HELP_X, NM_CLASSIC_SEARCH_HELP_Y,
                                        NM_CLASSIC_SEARCH_HELP_W, NM_CLASSIC_SEARCH_HELP_H,
                                        &search_help_data, p_list_cont);
    }

    //nm_classic_update_class_list(p_list, list_get_valid_pos(p_list), NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE, 0);

    //Page
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_PAGE_CONT,
                                    NM_CLASSIC_PAGE_CONTX, NM_CLASSIC_PAGE_CONTY,
                                    NM_CLASSIC_PAGE_CONTW, NM_CLASSIC_PAGE_CONTH,
                                    p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               NM_CLASSIC_PAGE_ARROWL_X, NM_CLASSIC_PAGE_ARROWL_Y,
                               NM_CLASSIC_PAGE_ARROWL_W, NM_CLASSIC_PAGE_ARROWL_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_L);
    p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                               NM_CLASSIC_PAGE_ARROWR_X, NM_CLASSIC_PAGE_ARROWR_Y,
                               NM_CLASSIC_PAGE_ARROWR_W, NM_CLASSIC_PAGE_ARROWR_H,
                               p_page_cont, 0);
    bmap_set_content_by_id(p_icon, IM_ARROW1_R);

    p_page_num = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_NM_CLASSIC_PAGE_NUM,
                                 NM_CLASSIC_PAGE_NUMBER_X, NM_CLASSIC_PAGE_NUMBER_Y,
                                 NM_CLASSIC_PAGE_NUMBER_W, NM_CLASSIC_PAGE_NUMBER_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_page_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_page_num, nm_classic_page_num_keymap);
    ctrl_set_proc(p_page_num, nm_classic_page_num_proc);

    text_set_align_type(p_page_num, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_page_num, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_page_num, TEXT_STRTYPE_UNICODE);

    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CLASSIC_CLIENT_CONT,
                                  NM_CLASSIC_CLIENT_CONTX, NM_CLASSIC_CLIENT_CONTY,
                                  NM_CLASSIC_CLIENT_CONTW, NM_CLASSIC_CLIENT_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    //Video list
    p_vdoList = ui_nm_classic_creat_vdo_list(p_client_cont);
    ctrl_set_keymap(p_vdoList, nm_classic_vdo_list_keymap);
    ctrl_set_proc(p_vdoList, nm_classic_vdo_list_proc);
    ctrl_set_sts(p_vdoList, OBJ_STS_HIDE);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    //Subclass list
    p_subclass_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_CLASSIC_SUBCLASS_LIST, 
                                        NM_CLASSIC_OPT_LIST_X, NM_CLASSIC_OPT_LIST_Y,
                                        NM_CLASSIC_OPT_LIST_W, NM_CLASSIC_OPT_LIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_subclass_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_subclass_list, nm_classic_subclass_list_keymap);
    ctrl_set_proc(p_subclass_list, nm_classic_subclass_list_proc);

    list_set_item_interval(p_subclass_list, NM_CLASSIC_OPT_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_subclass_list, &vdo_list_item_rstyle);

    list_set_count(p_subclass_list, 0, NM_CLASSIC_SUBCLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_subclass_list, ARRAY_SIZE(subclass_list_attr), NM_CLASSIC_SUBCLASS_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_subclass_list, nm_classic_update_subclass_list, 0);
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
    p_optlist = ctrl_create_ctrl(CTRL_LIST, IDC_NM_CLASSIC_OPT_LIST, 
                                        NM_CLASSIC_OPT_LIST_X, NM_CLASSIC_OPT_LIST_Y,
                                        NM_CLASSIC_OPT_LIST_W, NM_CLASSIC_OPT_LIST_H, 
                                        p_client_cont, 0);
    ctrl_set_rstyle(p_optlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_optlist, nm_classic_opt_list_keymap);
    ctrl_set_proc(p_optlist, nm_classic_opt_list_proc);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    list_set_item_interval(p_optlist, NM_CLASSIC_OPT_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_optlist, &vdo_list_item_rstyle);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    list_set_count(p_optlist, 0, NM_CLASSIC_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_optlist, ARRAY_SIZE(opt_list_attr), NM_CLASSIC_OPT_LIST_ITEM_NUM_ONE_PAGE);
    list_set_update(p_optlist, nm_classic_update_opt_list, 0);
    list_enable_page_mode(p_optlist, TRUE);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    for(i = 0; i < ARRAY_SIZE(opt_list_attr); i++)
    {
        list_set_field_attr(p_optlist, (u8)i, opt_list_attr[i].attr,
                            opt_list_attr[i].width, opt_list_attr[i].left, opt_list_attr[i].top);
        list_set_field_rect_style(p_optlist, (u8)i, opt_list_attr[i].rstyle);
        list_set_field_font_style(p_optlist, (u8)i, opt_list_attr[i].fstyle);
    }
    ctrl_set_sts(p_optlist, OBJ_STS_HIDE);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    g_pNmClassic->hClaList = p_list;
    g_pNmClassic->hVdoList = p_vdoList;
    g_pNmClassic->hSubClaList = p_subclass_list;
    g_pNmClassic->hPageNum = p_page_num;
    g_pNmClassic->hOptList = p_optlist;
    g_pNmClassic->hClientCont = p_client_cont;
    g_pNmClassic->hHelpCont = p_bottom_help;
OS_PRINTF("@@@ line = [%d]\n",__LINE__); 
    ctrl_process_msg(p_vdoList, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
   
OS_PRINTF("@@@ line = [%d]\n",__LINE__);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
OS_PRINTF("@@@ line = [%d]\n",__LINE__);    
    //al_netmedia_register_msg();
    Classic_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_NM_CLASSIC), NOTIFY_T_MSG, FALSE, MSG_OPEN_CLASSIC_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_nm_classic_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    Classic_CloseStateTree();

    ui_nm_classic_pic_deinit();

    ui_release_nm_classic_data();

    if (fw_find_root_by_id(ROOT_ID_SUBMENU_NM))
    {
        fw_notify_root(fw_find_root_by_id(ROOT_ID_SUBMENU_NM), NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    return ERR_NOFEATURE;
}

//dispatch msg
static RET_CODE ui_nm_classic_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = Classic_DispatchMsg(p_ctrl, msg, para1, para2);

    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@nm_classic unaccepted msg, id=0x%04x\n", msg);
    }

    return ret;
}

static RET_CODE ui_nm_classic_on_input_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)	
{
    para1 = (u32)(msg & MSG_DATA_MASK);
    msg = (msg & MSG_TYPE_MASK);
    ui_nm_classic_on_state_process(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

void ui_nm_classic_open_cfm_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
}

void ui_nm_classic_open_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_DLG_REQ, str_id, 0);
    }
}

static RET_CODE ui_nm_classic_on_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_NM_CLASSIC,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_classic_on_open_dlg\n");

    if (Classic_IsStateActive(SID_CLASSIC_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);
    }

    return SUCCESS;
}

static RET_CODE ui_nm_classic_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_NM_CLASSIC,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_classic_on_open_cfm_dlg\n");

    if (Classic_IsStateActive(SID_CLASSIC_ACTIVE))
    {
      dlg_data.content = para1;

      ui_comm_dlg_open(&dlg_data);

      p_root = fw_find_root_by_id(ROOT_ID_NM_CLASSIC);
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
    }

    return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(nm_classic_class_list_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
END_KEYMAP(nm_classic_class_list_keymap, NULL)

BEGIN_MSGPROC(nm_classic_class_list_proc, list_class_proc)
    ON_COMMAND(MSG_SELECT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_classic_on_state_process)
END_MSGPROC(nm_classic_class_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_classic_page_num_keymap, NULL)
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
END_KEYMAP(nm_classic_page_num_keymap, NULL)

BEGIN_MSGPROC(nm_classic_page_num_proc, text_class_proc)
    ON_COMMAND(MSG_NUMBER, ui_nm_classic_on_input_number)
    ON_COMMAND(MSG_SELECT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_classic_on_state_process)
END_MSGPROC(nm_classic_page_num_proc, text_class_proc)

BEGIN_KEYMAP(nm_classic_vdo_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
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
END_KEYMAP(nm_classic_vdo_list_keymap, NULL)

BEGIN_MSGPROC(nm_classic_vdo_list_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_INFO, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NUMBER, ui_nm_classic_on_input_number)
END_MSGPROC(nm_classic_vdo_list_proc, cont_class_proc)

BEGIN_KEYMAP(nm_classic_subclass_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(nm_classic_subclass_list_keymap, NULL)

BEGIN_MSGPROC(nm_classic_subclass_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_classic_on_state_process)
END_MSGPROC(nm_classic_subclass_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_classic_opt_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(nm_classic_opt_list_keymap, NULL)

BEGIN_MSGPROC(nm_classic_opt_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_classic_on_state_process)
END_MSGPROC(nm_classic_opt_list_proc, list_class_proc)

BEGIN_KEYMAP(nm_classic_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_RED, MSG_SEARCH)
END_KEYMAP(nm_classic_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(nm_classic_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, ui_nm_classic_on_state_process)

    ON_COMMAND(MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_PAGE_VDO_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_SEARCH_VDO_FAILED, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_GET_PLAY_URLS_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_GET_CLASS_LIST_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_INIT_SUCCESS, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_INIT_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_SUCCESS, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_DEINIT_SUCCESS, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_DEINIT_FAILED, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS, ui_nm_classic_on_state_process)   
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_FAILED, ui_nm_classic_on_state_process)

    ON_COMMAND(MSG_OPEN_CLASSIC_REQ, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_INPUT_COMP, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_EXIT_ALL, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_DESTROY, ui_nm_classic_on_destory)
    ON_COMMAND(MSG_UPDATE, ui_nm_classic_on_state_process)
    ON_COMMAND(MSG_SEARCH, ui_nm_classic_on_state_process)

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_nm_classic_on_open_cfm_dlg)
    ON_COMMAND(MSG_OPEN_DLG_REQ, ui_nm_classic_on_open_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_nm_classic_on_state_process)
END_MSGPROC(nm_classic_cont_proc, ui_comm_root_proc)

