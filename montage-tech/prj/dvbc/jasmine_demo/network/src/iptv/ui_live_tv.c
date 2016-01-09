/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_live_tv.h"
#include "ui_live_tv_api.h"
#include "ui_video.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "file_play_api.h"
#include "ui_live_tv_desktop.h"


#define IS_LIST_HIDE() (ctrl_get_sts(g_pLivetv->hChanListCont) == OBJ_STS_HIDE \
                                    && ctrl_get_sts(g_pLivetv->hGroupListCont) == OBJ_STS_HIDE ? TRUE : FALSE)

enum live_tv_local_msg
{
    MSG_HIDE_ALL_LIST = MSG_LOCAL_BEGIN + 200,
    MSG_SHOW_INFO_BAR,
    MSG_HIDE_INFO_BAR,
    MSG_ADD_FAV_PG,
    MSG_RECALL,
    MSG_CACEL_EXIT_WINDOW,
    MSG_ADD_URL_SRC,
    MSG_REDUCE_URL_SRC,
    MSG_SELECT_GROUP,
    MSG_NUM_PLAY,
};

enum live_tv_ctrl_id
{
    IDC_LIVE_TV_LIST_CONT = 1,
    IDC_LIVE_TV_GROUP_CONT,
    IDC_LIVE_TV_GROUP_LEFT,
    IDC_LIVE_TV_GROUP_RIGHT,
    IDC_LIVE_TV_GROUP,
    IDC_LIVE_TV_LIST_BAR,
    IDC_LIVE_TV_LIST,
    IDC_LIVE_TV_NUM_FRM,
    IDC_LIVE_TV_NUM_TXT,
    IDC_LIVE_TV_CHANGE_URL_SRC_PROG_NAME = 10,
    IDC_LIVE_TV_CHANGE_URL_SRC_INDEX,
    IDC_LIVE_TV_HELP_BAR,
    IDC_LIVE_TV_HELP_BMP,
    IDC_LIVE_TV_HELP_TEXT,
    IDC_LIVE_TV_HELP_BMP2,
    IDC_LIVE_TV_HELP_TEXT2,
    IDC_LIVE_TV_EXIT_CONT,
    IDC_LIVE_TV_EXIT_TEXT = 18,
};

enum live_tv_epg_info_ctrl_id
{
    IDC_LIVE_TV_EPG_CONT = 20,
    IDC_EPG_CURR_P,
    IDC_EPG_CURR_P_NAME,
    IDC_EPG_CURR_TIME,
    IDC_EPG_CURR_TIME_TEXT,
    IDC_EPG_NET_CONN,
    IDC_EPG_NET_CONN_TEXT,
    IDC_EPG_CURR_PROG,
    IDC_EPG_CURR_PROG_NAME,
    IDC_EPG_URL_SRC,
    IDC_EPG_URL_SRC_NUM = 30,
    IDC_EPG_NEXT_PROG,
    IDC_EPG_NEXT_PROG_NAME,
    IDC_EPG_CHANGE_PROG = 33,
};

enum live_tv_group_list_ctrl_id
{
    IDC_LIVE_TV_GP_LIST_CONT = 34,
    IDC_LIVE_TV_GP_GROUP_TITLE_CONT,
    IDC_LIVE_TV_GP_GROUP_LEFT,
    IDC_LIVE_TV_GP_GROUP_RIGHT,
    IDC_LIVE_TV_GP_LIST_BAR,
    IDC_LIVE_TV_GP_LIST,
    IDC_LIVE_TV_GP_HELP_BAR = 40,
    IDC_LIVE_TV_GP_HELP_BMP,
    IDC_LIVE_TV_GP_HELP_TEXT,
    IDC_LIVE_TV_GP_HELP_BMP2,
    IDC_LIVE_TV_GP_HELP_TEXT2 = 44,
};

static list_xstyle_t live_tv_list_item_rstyle =
{
    RSI_PBACK,
    RSI_ITEM_2_SH,
    RSI_ITEM_2_HL,
    RSI_ITEM_8_BG,
    RSI_ITEM_2_HL,
};

static list_xstyle_t live_tv_list_field_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
};

static list_xstyle_t live_tv_list_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
};


static list_field_attr_t live_tv_play_list_attr[LIVE_TV_LIST_FIELD] =
{
    { LISTFIELD_TYPE_UNISTR,
    55, 15, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle},
    { LISTFIELD_TYPE_UNISTR|STL_LEFT,
    190, 75, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle},
    { LISTFIELD_TYPE_ICON | STL_LEFT,
    17, 270, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle}
};


typedef struct
{
    control_t *hMainWin;
    control_t *hChanListCont;
    control_t *hChanList;  //prog list
    control_t *hGroup; 
    control_t *hGroupListCont;
    control_t *hGroupList;  //prog list
    control_t *hGroupName; 
    control_t *hPanel;
    control_t *hAlertDlg;
    control_t *hChanNum;
    control_t *hChanListHelpBar;
    control_t *hGroupHelp;
    
    u8 numBit;
    u8 circleIdx;//for change url src one circle;
    u16 vcircleIdx;//for change prog one circle at a group;
    u16 totalChan;
    u16 playingFocus;
    u16 currGrpIdx;
    BOOL stopCfm;
    BOOL playStoped;
    BOOL isExitPlay;
    BOOL isLeft;//for manually change url src, record left right key
    BOOL isAutoChange; //if TRUE means auto change url, else means manually change url src
    u16 allGrp;
    u16 favGrp;

    ui_livetv_chan_info_t prevPg;
    ui_livetv_chan_info_t currPg;

    ui_livetv_entry_grp_t entry_grp;
}ui_livetv_t;

static ui_livetv_t *g_pLivetv;

u16 live_tv_mainwin_keymap(u16 key);
u16 live_tv_list_keymap(u16 key);
u16 live_tv_num_play_cont_keymap(u16 key);
u16 live_tv_group_list_keymap(u16 key);

RET_CODE live_tv_mainwin_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE live_tv_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE live_tv_group_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE live_tv_num_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


static void show_infobar_while_change_pg(void);
static RET_CODE on_live_tv_hide_all_list(control_t *p_list, u16 msg, u32 para1, u32 para2);
static RET_CODE on_live_tv_load_media_error(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static void ui_livetv_start_timer(u16 msg, u32 time_out)
{
    ui_start_timer(ROOT_ID_LIVE_TV, msg, time_out);
}

static void ui_livetv_stop_timer(u16 msg)
{
    ui_stop_timer(ROOT_ID_LIVE_TV, msg);
}

static void ui_livetv_reset_hide_all_list()
{
    ui_livetv_start_timer(MSG_HIDE_ALL_LIST, 10000);
}

void ui_live_tv_set_exit_state(BOOL is_exit)
{
    g_pLivetv->isExitPlay = is_exit;
}

void ui_live_tv_get_exit_state(BOOL *is_exit)
{
    *is_exit = g_pLivetv->isExitPlay;
}

static RET_CODE live_tv_group_update(control_t *p_cbox, u16 focus, u16 *p_str, u16 max_length)
{
    u16 *p_unistr = NULL;
    ui_livetv_group_info_t groupInfo = {{0}};

    if (focus == g_pLivetv->allGrp)
    {
        p_unistr = (u16 *)gui_get_string_addr(IDS_ALL);
    }
    else if (focus == g_pLivetv->favGrp)
    {
        p_unistr = (u16 *)gui_get_string_addr(IDS_FAV);
    }
    else //comm network group
    {
        ui_livetv_storage_get_group_info_by_group_index(&groupInfo, focus - 1);
        p_unistr = groupInfo.name;
    }

    uni_strcpy(p_str, p_unistr);

    return SUCCESS;
}


static BOOL ui_livetv_check_net()
{
    net_conn_stats_t eth_connt_stats = {0};

    eth_connt_stats = ui_get_net_connect_status();
    if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE) \
                 && (eth_connt_stats.is_3g_conn == FALSE))
    {
        return FALSE;
    }
    return TRUE;
}

static void live_tv_open_dlg(u16 str_id, u32 time_out)
{
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    };
    dlg_data.content = str_id;
    dlg_data.dlg_tmout = time_out;

    if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
    {
        ui_comm_dlg_open(&dlg_data);
    }
}

static BOOL ui_livetv_play_net_prog(ui_livetv_chan_info_t *chan_info)
{
    MT_ASSERT(chan_info != NULL);

    ui_comm_loading_hide();

    if ((chan_info->ppUrlList != NULL) && (chan_info->urlIdx < chan_info->urlCnt))
    {
        if (chan_info->ppUrlList[chan_info->urlIdx] != NULL)
        {
            ui_video_c_play_by_url(chan_info->ppUrlList[chan_info->urlIdx], 0, OTT_LIVE_MODE);

            return TRUE;
        }
    }

    ui_video_c_play_by_url("", 0, OTT_LIVE_MODE);

    return FALSE;
}

static control_t *create_mainwindow(void)
{
	control_t *p_mainwin = NULL;  
         
	p_mainwin = fw_create_mainwin(ROOT_ID_LIVE_TV,
		LIVE_TV_CONT_X, LIVE_TV_CONT_Y,
		LIVE_TV_CONT_W, LIVE_TV_CONT_H,
		ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
	MT_ASSERT(p_mainwin != NULL);

	ctrl_set_rstyle(p_mainwin, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_keymap(p_mainwin, live_tv_mainwin_keymap);
	ctrl_set_proc(p_mainwin, live_tv_mainwin_proc);
	g_pLivetv->hMainWin = p_mainwin;
    
	return p_mainwin;
}

/*=================================================================
                                        create livetv program group list
===================================================================*/

static control_t *create_live_tv_prog_list_cont(control_t *p_mainwin)
{
	control_t *p_list_cont = NULL;

	p_list_cont = ctrl_create_ctrl(CTRL_CONT,
		IDC_LIVE_TV_LIST_CONT, LIVE_TV_LIST_CONT_X,
		LIVE_TV_LIST_CONT_Y, LIVE_TV_LIST_CONT_W,
		LIVE_TV_LIST_CONT_H, p_mainwin, 0);

	ctrl_set_rstyle(p_list_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ui_livetv_reset_hide_all_list();
	g_pLivetv->hChanListCont = p_list_cont;
	
	return p_list_cont;
}

static void create_live_tv_prog_list_group(control_t *p_list_cont)
{
	control_t *p_group_cont = NULL,  *p_group = NULL;
	control_t *p_ctrl = NULL;

	//group container
	p_group_cont =
		ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GROUP_CONT, LIVE_TV_GROUP_CONT_X,
		LIVE_TV_GROUP_CONT_Y, LIVE_TV_GROUP_CONT_W,LIVE_TV_GROUP_CONT_H, p_list_cont, 0);

	ctrl_set_rstyle(p_group_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	
	//left arrow
	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GROUP_LEFT,
		LIVE_TV_GROUPL_X, LIVE_TV_GROUPL_Y,
		LIVE_TV_GROUPL_W, LIVE_TV_GROUPL_H,
		p_group_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_ARROW_L);

	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GROUP_RIGHT,
		LIVE_TV_GROUPR_X, LIVE_TV_GROUPR_Y,
		LIVE_TV_GROUPR_W, LIVE_TV_GROUPR_H,
		p_group_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_ARROW_R);

	//group
	p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_LIVE_TV_GROUP, LIVE_TV_GROUP_X,
		LIVE_TV_GROUP_Y, LIVE_TV_GROUP_W,
		LIVE_TV_GROUP_H, p_group_cont, 0);
	ctrl_set_rstyle(p_group, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	cbox_set_font_style(p_group, FSI_BLACK, FSI_BLACK, FSI_BLACK);
	cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
	cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
	cbox_enable_cycle_mode(p_group, TRUE);

	cbox_dync_set_count(p_group, ui_livetv_storage_get_group_cnt()+2);//all grp cnt
	cbox_dync_set_update(p_group, live_tv_group_update);
	g_pLivetv->hGroup = p_group;

      if(!g_pLivetv->entry_grp)
      {
      	  cbox_dync_set_focus(p_group, g_pLivetv->allGrp);  
     	}
       else
	{
	  cbox_dync_set_focus(p_group,g_pLivetv->currGrpIdx);  
       }
	OS_PRINTF("##%s, group index = %d##\n", __FUNCTION__,g_pLivetv->currGrpIdx);
}


static RET_CODE live_tv_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 *p_unistr;
    u8 asc_str[32] = {0};
    live_tv_fav_info fav_livetv_info;
    u16 i, j, cnt, group_focus;
    u32 fav_num = 0;
    ui_livetv_chan_info_t p_pg;

    cnt = list_get_count(p_list);
    group_focus = cbox_dync_get_focus(g_pLivetv->hGroup);

    for (i = start; i < start + size && i < cnt; i++)
    {
        /* icon. */
        list_set_field_content_by_icon(p_list, i, 2, 0);
        p_unistr = NULL;
        if (group_focus < g_pLivetv->favGrp)
        {
            if (group_focus == g_pLivetv->allGrp)
            {
                ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, LIVE_TV_INVALID_INDEX, i);
            }
            else
            {
                ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, group_focus - 1, i);
            }
            sys_status_get_fav_livetv_total_num(&fav_num);

            if(fav_num > 0)
            {
                for(j = 0; j < fav_num;j++)
                {
                    sys_status_get_fav_livetv_info(j, &fav_livetv_info);
                    if(fav_livetv_info.live_fav_id == p_pg.id)
                    {
                        list_set_field_content_by_icon(p_list, i, 2, IM_TV_FAV);
                    }
                }
            }
            /* NO. */
            sprintf(asc_str, "%04d", p_pg.id);
            list_set_field_content_by_ascstr(p_list, i, 0, asc_str);
            p_unistr = p_pg.name;
        }
        else if(group_focus == g_pLivetv->favGrp)//fav group
        {
            list_set_field_content_by_icon(p_list, i, 2, IM_TV_FAV);
            sys_status_get_fav_livetv_info(i, &fav_livetv_info);
            sprintf(asc_str, "%04d", (u16)fav_livetv_info.live_fav_id);
            list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

            str_asc2uni(fav_livetv_info.live_fav_name, p_unistr);
            //p_unistr = fav_livetv_info.live_fav_name;
        }

        /* Name. */
        if (p_unistr != NULL)
        {
            list_set_field_content_by_unistr(p_list, i, 1, p_unistr); 
        }
    }

    return SUCCESS;
}

static void create_live_tv_prog_list(control_t *p_list_cont)
{
    control_t  *p_list = NULL;
    control_t *p_list_bar = NULL;
    u16 i;  

    //list
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_LIVE_TV_LIST, LIVE_TV_LIST_X,
    	LIVE_TV_LIST_Y, LIVE_TV_LIST_W, LIVE_TV_LIST_H, p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, live_tv_list_keymap);
    ctrl_set_proc(p_list, live_tv_list_proc);

    ctrl_set_mrect(p_list, LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDT,
    LIVE_TV_LIST_MIDW+LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDH+LIVE_TV_LIST_MIDT);
    list_set_item_interval(p_list,LIVE_TV_LCONT_LIST_VGAP);
    list_set_item_rstyle(p_list, &live_tv_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);
    list_enable_page_mode(p_list, TRUE);
    g_pLivetv->hChanList = p_list;

    if(g_pLivetv->entry_grp )
    {
       list_set_count(p_list, ui_livetv_storage_get_chan_cnt_by_group_index( g_pLivetv->currGrpIdx -1), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    }
    else
    {
      list_set_count(p_list, g_pLivetv->totalChan, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    }
    list_set_field_count(p_list, LIVE_TV_LIST_FIELD, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    g_pLivetv->playingFocus = 0;
	
    list_set_focus_pos(p_list, g_pLivetv->playingFocus);
    list_select_item(p_list, g_pLivetv->playingFocus);
    list_set_update(p_list, live_tv_list_update, 0);

    for (i = 0; i < LIVE_TV_LIST_FIELD; i++)
    {
        list_set_field_attr(p_list, (u8)i, (u32)(live_tv_play_list_attr[i].attr), (u16)(live_tv_play_list_attr[i].width),
        	(u16)(live_tv_play_list_attr[i].left), (u8)(live_tv_play_list_attr[i].top));
        list_set_field_font_style(p_list, (u8)i, live_tv_play_list_attr[i].fstyle);
    }

    //scrollbar
    p_list_bar = ctrl_create_ctrl(CTRL_SBAR, IDC_LIVE_TV_LIST_BAR, LIVE_TV_LIST_BAR_X,
    LIVE_TV_LIST_BAR_Y, LIVE_TV_LIST_BAR_W, LIVE_TV_LIST_BAR_H, p_list_cont, 0);
    ctrl_set_rstyle(p_list_bar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_list_bar, 1);
    sbar_set_direction(p_list_bar, 0);
    sbar_set_mid_rstyle(p_list_bar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    list_set_scrollbar(p_list, p_list_bar);

    live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 

}

static void create_live_tv_prog_list_helpbar(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_bmp = NULL;
	control_t *p_text = NULL;
      u8 asc_str1[32]= {"[Fav]:Add fav program"};
      u8 asc_str2[32]= {"[Yellow]:Select group"};
      
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_HELP_BAR,
		LIVE_TV_HELP_FRM_X, LIVE_TV_HELP_FRM_Y,
		LIVE_TV_HELP_FRM_W, LIVE_TV_HELP_FRM_H,
		p_mainwin, 0);

	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	g_pLivetv->hChanListHelpBar = p_cont;
	ctrl_set_sts(p_cont, OBJ_STS_SHOW);

      //fav bmp
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_HELP_BMP, 20, 0, 
		26, LIVE_TV_HELP_FRM_H/2, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);
	bmap_set_content_by_id(p_bmp, IM_TV_FAV);

        //fav text
	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_HELP_TEXT,
		56, 0,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H/2,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
	text_set_content_by_ascstr(p_text, asc_str1);

        //select group bmp(yellow key)
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_HELP_BMP2, 20, 
	LIVE_TV_HELP_FRM_H/2+5, 
		25, 25, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	bmap_set_content_by_id(p_bmp, IM_OK);
      bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);

        //select group text
	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_HELP_TEXT2,
		56, LIVE_TV_HELP_FRM_H/2,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H/2,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
	text_set_content_by_ascstr(p_text, asc_str2);
    
}


static void ui_livetv_create_prog_list(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;
    
    //create list cont
    p_list_cont = create_live_tv_prog_list_cont(p_mainwin);

    //create live_tv group
    create_live_tv_prog_list_group(p_list_cont);

    //create live_tv list
    create_live_tv_prog_list(p_list_cont);

    //create help bar
    create_live_tv_prog_list_helpbar(p_mainwin);
}



/*=================================================================
                                        create livetv jump group list
===================================================================*/
static control_t *create_live_tv_group_list_cont(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;

    p_list_cont = ctrl_create_ctrl(CTRL_CONT,
        IDC_LIVE_TV_GP_LIST_CONT, LIVE_TV_LIST_CONT_X,
        LIVE_TV_LIST_CONT_Y, LIVE_TV_LIST_CONT_W,
        LIVE_TV_LIST_CONT_H, p_mainwin, 0);

    ctrl_set_rstyle(p_list_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    g_pLivetv->hGroupListCont = p_list_cont;
    ctrl_set_sts(p_list_cont, OBJ_STS_HIDE);
    
    return p_list_cont;
}

static void create_live_tv_group_list_curr_group_title(control_t *p_list_cont)
{
    control_t *p_title_cont = NULL;
    control_t *p_left = NULL;
    control_t *p_right = NULL;
    u8 asc_str[32] = {"Current Group:"};
    
    //group container
    p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GP_GROUP_TITLE_CONT, LIVE_TV_GROUP_CONT_X,
        LIVE_TV_GROUP_CONT_Y, LIVE_TV_GROUP_CONT_W,LIVE_TV_GROUP_CONT_H, p_list_cont, 0);
    ctrl_set_rstyle(p_title_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    
    //left text
    p_left = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_GROUP_LEFT, 5,
        0, LIVE_TV_GROUP_CONT_W/2+25,LIVE_TV_GROUP_CONT_H, p_title_cont, 0);
    ctrl_set_rstyle(p_left, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_left, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_left, FSI_BLACK, FSI_BLACK, FSI_BLACK);
    text_set_content_type(p_left, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_left, asc_str);

    //right curr group name
    p_right = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_GROUP_RIGHT, 
    LIVE_TV_GROUP_CONT_W/2+30,
        0, LIVE_TV_GROUP_CONT_W/2-30,LIVE_TV_GROUP_CONT_H, p_title_cont, 0);
    ctrl_set_rstyle(p_right, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    
    text_set_align_type(p_right, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_right, FSI_BLACK, FSI_BLACK, FSI_BLACK);
    text_set_content_type(p_right, TEXT_STRTYPE_UNICODE);
    g_pLivetv->hGroupName = p_right;
}

static RET_CODE live_tv_group_list_update(control_t *p_list, u16 start, u16 size, u32 context)
{
    u16 *p_unistr;
    u16 i, cnt;
    u8  asc_str[32];
    ui_livetv_group_info_t groupInfo;

    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        /* NO. */
        sprintf(asc_str, "%03d", i + 1);
        list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

        /* NAME */
        p_unistr = NULL;
        if(i == 0)
        {
            p_unistr = (u16 *)gui_get_string_addr(IDS_FAV);
        }
        else if(i == 1)
        {
            p_unistr = (u16 *)gui_get_string_addr(IDS_ALL);
        }
        else
        {
            ui_livetv_storage_get_group_info_by_group_index(&groupInfo, i - 2);
            p_unistr = groupInfo.name;
        }
        list_set_field_content_by_unistr(p_list, i, 1, p_unistr);
    }

    return SUCCESS;
}


static void create_live_tv_group_list(control_t *p_list_cont)
{
    control_t  *p_list = NULL;
    control_t *p_list_bar = NULL;
    u16 i;  

    //list
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_LIVE_TV_GP_LIST, LIVE_TV_LIST_X,
    	LIVE_TV_LIST_Y, LIVE_TV_LIST_W, LIVE_TV_LIST_H, p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, live_tv_group_list_keymap);
    ctrl_set_proc(p_list, live_tv_group_list_proc);

    ctrl_set_mrect(p_list, LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDT,
    	LIVE_TV_LIST_MIDW+LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDH+LIVE_TV_LIST_MIDT);
    list_set_item_interval(p_list,LIVE_TV_LCONT_LIST_VGAP);
    list_set_item_rstyle(p_list, &live_tv_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);
    list_enable_page_mode(p_list, TRUE);
    g_pLivetv->hGroupList = p_list;

    list_set_count(p_list, ui_livetv_storage_get_group_cnt()+2, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, LIVE_TV_LIST_FIELD, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);

    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, live_tv_group_list_update, 0);

    for (i = 0; i < LIVE_TV_LIST_FIELD; i++)
    {
        list_set_field_attr(p_list, (u8)i, (u32)(live_tv_play_list_attr[i].attr), (u16)(live_tv_play_list_attr[i].width),
        	(u16)(live_tv_play_list_attr[i].left), (u8)(live_tv_play_list_attr[i].top));
        list_set_field_font_style(p_list, (u8)i, live_tv_play_list_attr[i].fstyle);
    }

    //scrollbar
    p_list_bar = ctrl_create_ctrl(CTRL_SBAR, IDC_LIVE_TV_GP_LIST_BAR, LIVE_TV_LIST_BAR_X,
    	LIVE_TV_LIST_BAR_Y, LIVE_TV_LIST_BAR_W, LIVE_TV_LIST_BAR_H, p_list_cont, 0);
    ctrl_set_rstyle(p_list_bar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_list_bar, 1);
    sbar_set_direction(p_list_bar, 0);
    sbar_set_mid_rstyle(p_list_bar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
    list_set_scrollbar(p_list, p_list_bar);

    live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);

}

static void create_live_tv_group_list_helpbar(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_bmp = NULL;
	control_t *p_text = NULL;
      u8 asc_str[32] = {"[OK]:Entry group"};
      
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GP_HELP_BAR,
		LIVE_TV_HELP_FRM_X, LIVE_TV_HELP_FRM_Y,
		LIVE_TV_HELP_FRM_W, LIVE_TV_HELP_FRM_H,
		p_mainwin, 0);

	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	g_pLivetv->hGroupHelp = p_cont;
      ctrl_set_sts(p_cont, OBJ_STS_HIDE);
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GP_HELP_BMP, 20, 0, 
		38, LIVE_TV_HELP_FRM_H, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
       bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);
	bmap_set_content_by_id(p_bmp, IM_HELP_OK);

	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_HELP_TEXT,
		56, 0,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_CENTER| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
	text_set_content_by_ascstr(p_text, asc_str);
    
}

static void ui_livetv_create_select_group_list(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;
    
    //create list cont
    p_list_cont = create_live_tv_group_list_cont(p_mainwin);

    //create live_tv group
    create_live_tv_group_list_curr_group_title(p_list_cont);

    //create live_tv list
    create_live_tv_group_list(p_list_cont);

    //create help bar
    create_live_tv_group_list_helpbar(p_mainwin);
}


static void create_num_play(control_t *p_mainwin)
{
    control_t *p_frm = NULL;
    control_t *p_subctrl = NULL;

    // frm
    p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_NUM_FRM,
    	LIVE_TV_NUM_PLAY_FRM_X, LIVE_TV_NUM_PLAY_FRM_Y,
    	LIVE_TV_NUM_PLAY_FRM_W, LIVE_TV_NUM_PLAY_FRM_H,
    	p_mainwin, 0);
    ctrl_set_rstyle(p_frm, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    ctrl_set_keymap(p_frm, live_tv_num_play_cont_keymap);
    ctrl_set_proc(p_frm, live_tv_num_play_cont_proc);
    ctrl_set_sts(p_frm, OBJ_STS_HIDE);
    g_pLivetv->hChanNum = p_frm;

    p_subctrl = ctrl_create_ctrl(CTRL_NBOX, IDC_LIVE_TV_NUM_TXT,
    	LIVE_TV_NUM_PLAY_TXT_X, LIVE_TV_NUM_PLAY_TXT_Y,
    	LIVE_TV_NUM_PLAY_TXT_W, LIVE_TV_NUM_PLAY_TXT_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    nbox_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
    nbox_set_font_style(p_subctrl, FSI_BLACK, FSI_BLACK, FSI_BLACK);
    nbox_set_num_type(p_subctrl, NBOX_NUMTYPE_DEC);
    nbox_set_range(p_subctrl, 0, 9999, LIVE_TV_NUM_PLAY_CNT);

}

static void create_live_tv_epg_bar(control_t *p_mainwin)
{
    control_t *p_frm = NULL;
    control_t *p_subctrl = NULL;

    p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_EPG_CONT,
    	LIVE_TV_EPG_FRM_X, LIVE_TV_EPG_FRM_Y,
    	LIVE_TV_EPG_FRM_W, LIVE_TV_EPG_FRM_H,
    	p_mainwin, 0);
    ctrl_set_rstyle(p_frm, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    ctrl_set_sts(p_frm, OBJ_STS_HIDE);
    g_pLivetv->hPanel = p_frm;

    //first line µ±Ç°ÆµµÀ
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_P,
    	LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y,
    	LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_PG);


    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_P_NAME,
    	LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y,
    	2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    //time controls
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_TIME,
    	LIVE_TV_EPG_CURR_P_X+3*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y,
    	LIVE_TV_EPG_CURR_P_W+30, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_TIME);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_TIME_TEXT,
    	LIVE_TV_EPG_CURR_P_X+4*LIVE_TV_EPG_CURR_P_W+30+50, LIVE_TV_EPG_CURR_P_Y,
    	2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NET_CONN,
    	LIVE_TV_EPG_CURR_P_X+6*LIVE_TV_EPG_CURR_P_W+30+50, LIVE_TV_EPG_CURR_P_Y,
    	1*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_by_strid(p_subctrl, IDS_CON_STATUS);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NET_CONN_TEXT,
    	LIVE_TV_EPG_CURR_P_X+7*LIVE_TV_EPG_CURR_P_W+30+50+50, LIVE_TV_EPG_CURR_P_Y,
    	LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    //second line current program
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_PROG,
    	LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
    	LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_PROG);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_PROG_NAME,
    	LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
    	2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    //change url src info 
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_URL_SRC,
    	4*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
    	2*LIVE_TV_EPG_CURR_P_W + 50, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_CHANGE_SRC);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_URL_SRC_NUM,
    	6*LIVE_TV_EPG_CURR_P_W + 50+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
    	2 *LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    //third line next pg
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NEXT_PROG,
    	LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
    	LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_NEXT_PROG);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NEXT_PROG_NAME,
    	LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
    	2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CHANGE_PROG,
    	4*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
    	3*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
    	p_frm, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_subctrl, IDS_LIVETV_CHANGE_PG);
	
}

static void create_live_tv_exit_info(control_t *p_mainwin)
{
    control_t *p_cont = NULL;
    control_t *p_txt = NULL;

    p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_EXIT_CONT,
    	COMM_DLG_X, COMM_DLG_Y,COMM_DLG_W, COMM_DLG_H,
    	p_mainwin, 0);
    ctrl_set_rstyle(p_cont,RSI_POPUP_BG,RSI_POPUP_BG,RSI_POPUP_BG);
    ctrl_set_sts(p_cont, OBJ_STS_HIDE);
    g_pLivetv->hAlertDlg = p_cont;
    p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_EXIT_TEXT,
    	DLG_CONTENT_GAP, DLG_CONTENT_GAP,
    	(u16)(COMM_DLG_X - 2 * DLG_CONTENT_GAP),
    	(u16)(COMM_DLG_H - 2 * DLG_CONTENT_GAP),
    	p_cont, 0);

    text_set_font_style(p_txt, FSI_BLACK, FSI_BLACK, FSI_BLACK);
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt,IDS_EXIT_PLAY_AFTER_EXIT );
    ui_live_tv_set_exit_state(FALSE);
}


static RET_CODE ui_livetv_play_by_grp_idx_and_chan_pos(u16 group_focus, u16 list_focus)
{
    live_tv_fav_info fav_livetv_info;
    ui_livetv_chan_info_t p_pg;
    BOOL eRet = FALSE;
    s32 chan_pos;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (group_focus < g_pLivetv->favGrp) //comm network prog
    {
        if (group_focus == g_pLivetv->allGrp)
        {
            eRet = ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, LIVE_TV_INVALID_INDEX, list_focus);
        }
        else
        {
            eRet = ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, group_focus - 1, list_focus);
        }
    }
    else if (group_focus == g_pLivetv->favGrp)//fav group
    {
        sys_status_get_fav_livetv_info(list_focus, &fav_livetv_info);
       
        OS_PRINTF("@@@live_fav_id=%d\n", fav_livetv_info.live_fav_id);
        chan_pos = ui_livetv_storage_get_chan_info_by_id(&p_pg, fav_livetv_info.live_fav_id);
        if (chan_pos < 0)
        {
            eRet = FALSE;
        }
    }

    //check curr pg is playing, if is playing, will return
    if (MUL_PLAY_STATE_PLAY == ui_video_c_get_play_state() && (g_pLivetv->currPg.id == p_pg.id))
    {
        return ERR_FAILURE;
    }

    g_pLivetv->currGrpIdx = group_focus;
    g_pLivetv->playingFocus = list_focus;

    memcpy(&g_pLivetv->prevPg, &g_pLivetv->currPg, sizeof(ui_livetv_chan_info_t));
    memcpy(&g_pLivetv->currPg, &p_pg, sizeof(ui_livetv_chan_info_t));
    g_pLivetv->isLeft = FALSE;
    g_pLivetv->circleIdx = g_pLivetv->currPg.urlIdx;

    if (IS_LIST_HIDE())
    {
        show_infobar_while_change_pg();
    }

    ui_livetv_play_net_prog(&g_pLivetv->currPg);

    return eRet;
}


static void ui_livetv_play_first_prog(void)
{

    if(g_pLivetv->entry_grp == ENTRY_ALL_GRP)
    {
    	ui_livetv_play_by_grp_idx_and_chan_pos(g_pLivetv->allGrp, 0);
    }
    else if(g_pLivetv->entry_grp == ENTRY_CUS_GRP)
    {
	ui_livetv_play_by_grp_idx_and_chan_pos(ui_livetv_storage_get_group_cnt(), 0);
    }
	
}

static void init_livetv_static_data(void)
{
    u16 group_cnt;

    group_cnt = ui_livetv_storage_get_group_cnt();
    g_pLivetv->totalChan = ui_livetv_storage_get_chan_cnt_by_group_index(LIVE_TV_INVALID_INDEX);
    g_pLivetv->allGrp = 0;
    g_pLivetv->favGrp = group_cnt + 1;
    g_pLivetv->vcircleIdx = INVALIDID;
    g_pLivetv->currGrpIdx = g_pLivetv->allGrp;
}

static BOOL ui_livetv_init_app_data(void)
{
    if (g_pLivetv == NULL)
    {
        g_pLivetv = (ui_livetv_t *)mtos_malloc(sizeof(ui_livetv_t));
        MT_ASSERT(g_pLivetv != NULL);
        memset((void *)g_pLivetv, 0, sizeof(ui_livetv_t));
    }
    
    init_livetv_static_data();

    return TRUE;
}

extern BOOL ui_is_init_pLivetvData();
extern  void al_livetv_create_livetv_data(void);

RET_CODE open_live_tv(u32 para1, u32 para2)
{
    control_t *p_mainwin = NULL;


    if(para1 == 1)
    {
       if(ui_is_init_pLivetvData() == FALSE)
       {
          al_livetv_create_livetv_data();
	 }
    }
	
    //init iptv data
    ui_livetv_init_app_data();

     g_pLivetv->entry_grp = para1;

     if(g_pLivetv->entry_grp )
     {
        g_pLivetv->currGrpIdx = ui_livetv_storage_get_group_cnt();
     }
    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    //init customer url
    ui_livetv_init_custom_chan_list();

    //create mainwindow
    p_mainwin = create_mainwindow();

    //create iptv playing loading
    ui_comm_loading_create(p_mainwin);   

    //create livetv prog list
    ui_livetv_create_prog_list(p_mainwin);

    //create livetv group list
    ui_livetv_create_select_group_list(p_mainwin);

    //create number play
    create_num_play(p_mainwin);

    //create epg bar
    create_live_tv_epg_bar(p_mainwin);

    //create exit text
    create_live_tv_exit_info(p_mainwin);

    ctrl_paint_ctrl(ctrl_get_root(p_mainwin), FALSE);

    //play the last prog once entry livetv
    ui_livetv_play_first_prog();

    return SUCCESS;
}

static void close_infobar_while_show_pg_list()
{
    if(ctrl_get_sts(g_pLivetv->hPanel) == OBJ_STS_SHOW)
    {
        ui_livetv_stop_timer(MSG_HIDE_INFO_BAR);
        ctrl_set_sts(g_pLivetv->hPanel, OBJ_STS_HIDE);
        ctrl_paint_ctrl(g_pLivetv->hPanel, TRUE);
    }
}

static RET_CODE ui_livetv_change_prog(u16 msg)
{
    s32 list_focus;
    u8 group_index;
    u32 cur_grp_cnt;
	
    group_index = g_pLivetv->currGrpIdx;
    if (group_index < g_pLivetv->favGrp)
    {
        if (group_index == g_pLivetv->allGrp)
        {
            cur_grp_cnt = g_pLivetv->totalChan;
        }  
        else
        {
            cur_grp_cnt = ui_livetv_storage_get_chan_cnt_by_group_index(group_index - 1);
        }
    }
    else if (group_index == g_pLivetv->favGrp)//fav
    {
        sys_status_get_fav_livetv_total_num(&cur_grp_cnt);
        if (cur_grp_cnt == 0)
        {
            g_pLivetv->currGrpIdx = group_index = g_pLivetv->allGrp;
            g_pLivetv->playingFocus = ui_livetv_storage_get_chan_pos_by_id(g_pLivetv->currPg.id);
            cur_grp_cnt = g_pLivetv->totalChan;
        }
    }

    list_focus = g_pLivetv->playingFocus;
    switch (msg)
    {
        case MSG_FOCUS_UP:
            list_focus++;
            if (list_focus >= cur_grp_cnt)
            {
                list_focus = cur_grp_cnt - 1;
            }
            break;

        case MSG_FOCUS_DOWN:
            list_focus--;
            if (list_focus < 0)
            {
                list_focus = 0;
            }
            break;

        case MSG_PAGE_UP:
            list_focus += LIVE_TV_LIST_ITEM_NUM_ONE_PAGE;
            if (list_focus >= cur_grp_cnt)
            {
                list_focus = cur_grp_cnt - 1;
            }
            break;

        case MSG_PAGE_DOWN:
            list_focus -= LIVE_TV_LIST_ITEM_NUM_ONE_PAGE;
            if (list_focus < 0)
            {
                list_focus = 0;
            }
            break;

        default:
            break;
    }

    if (g_pLivetv->isAutoChange == FALSE)
    {
        g_pLivetv->vcircleIdx = list_focus;
    }
    else
    {
        if (list_focus == g_pLivetv->vcircleIdx)
        {
            if (ctrl_get_sts(g_pLivetv->hChanListCont) == OBJ_STS_SHOW)
            {
                ctrl_set_sts(g_pLivetv->hChanListCont, OBJ_STS_HIDE);
                ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);
            }
            g_pLivetv->vcircleIdx = INVALIDID;

            return SUCCESS;
        }
        else
        {
            if (ctrl_get_sts(g_pLivetv->hChanListCont) == OBJ_STS_SHOW)
            {
                if (g_pLivetv->currGrpIdx == cbox_dync_get_focus(g_pLivetv->hGroup))
                {
                    list_set_focus_pos(g_pLivetv->hChanList, list_focus);
                    list_select_item(g_pLivetv->hChanList, list_focus);
                    ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);
                }
            }
        }
    }

    ui_livetv_play_by_grp_idx_and_chan_pos(group_index, list_focus);

    return SUCCESS;
}


static RET_CODE on_livetv_select_to_show_proglist(BOOL isJump)
{
    control_t *p_list = NULL;
    u8 group_index;
    u32 count;

    p_list = g_pLivetv->hChanList;
    close_infobar_while_show_pg_list();
    ui_livetv_reset_hide_all_list();

    ctrl_set_sts(g_pLivetv->hChanListCont, OBJ_STS_SHOW);
    ctrl_set_sts(g_pLivetv->hChanListHelpBar, OBJ_STS_SHOW);
    OS_PRINTF("###show list prog id == %d##\n", g_pLivetv->currPg.id);
    OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
    if (!isJump)
    {
        group_index = g_pLivetv->currGrpIdx;
    }
    else
    {
        group_index = cbox_dync_get_focus(g_pLivetv->hGroup);
    }

    if (group_index < g_pLivetv->favGrp) //comm network prog
    {
        if (group_index == g_pLivetv->allGrp)
        {
            count = g_pLivetv->totalChan;
        }
        else
        {
            count = ui_livetv_storage_get_chan_cnt_by_group_index(group_index - 1);
            if(group_index == ui_livetv_storage_get_group_cnt()) //customer group
            {
                ctrl_set_sts(g_pLivetv->hChanListHelpBar, OBJ_STS_HIDE);
            }
        }
    }
    else if (group_index == g_pLivetv->favGrp)//fav
    {
        sys_status_get_fav_livetv_total_num(&count);
        if (!isJump)
        {
            if (count == 0)
            {
                count = g_pLivetv->totalChan;
                group_index = g_pLivetv->allGrp;
                g_pLivetv->currGrpIdx = group_index;
                g_pLivetv->playingFocus = ui_livetv_storage_get_chan_pos_by_id(g_pLivetv->currPg.id);
                OS_PRINTF("##playing fav id ==[%d], playing focus = [%d]##\n", g_pLivetv->currPg.id, g_pLivetv->playingFocus);
            }
        }
    }
    OS_PRINTF("##%s, this group count == %d##\n", __FUNCTION__, count);
    list_set_count(p_list, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    if (!isJump)
    {
        list_set_focus_pos(p_list, g_pLivetv->playingFocus);
        list_select_item(p_list, g_pLivetv->playingFocus);
    }
    else
    {
        list_set_focus_pos(p_list, 0);
    }
    cbox_dync_set_focus(g_pLivetv->hGroup, group_index);
    cbox_dync_set_update(g_pLivetv->hGroup, live_tv_group_update);
    live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
    ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);
    OS_PRINTF("##live tv list hide, should show list##\n");

    return SUCCESS;
}


static RET_CODE on_live_tv_list_select_to_play(void)
{
    control_t *p_list;
    u16 group_focus;
    u16 list_focus;
    u16 list_cnt;

    OS_PRINTF("@@@%s\n", __FUNCTION__);

    p_list = g_pLivetv->hChanList;
    list_cnt = list_get_count(p_list);
    if (list_cnt == 0)
    {
        return ERR_FAILURE;
    }

    list_focus = list_get_focus_pos(p_list);
    group_focus = cbox_dync_get_focus(g_pLivetv->hGroup);

    list_select_item(p_list, list_focus);
    ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);

    g_pLivetv->isAutoChange = FALSE;
    g_pLivetv->vcircleIdx = list_focus;
    ui_livetv_play_by_grp_idx_and_chan_pos(group_focus, list_focus);

    return SUCCESS;
}


static RET_CODE on_livetv_proglist_select_to_play_or_show_list(control_t *p_list, u16 msg,
															u32 para1, u32 para2)
{
    if(OBJ_STS_HIDE == ctrl_get_sts(g_pLivetv->hChanListCont))
    {
        on_livetv_select_to_show_proglist(FALSE);
    }
    else
    {
        on_live_tv_list_select_to_play();
    }

    return SUCCESS;
}


static u8 ui_livetv_get_next_url_idx(void)
{
    s16 urlIdx;

    urlIdx = g_pLivetv->currPg.urlIdx;

    if (g_pLivetv->isLeft == FALSE)
    {
        urlIdx++;
        if (urlIdx >= g_pLivetv->currPg.urlCnt)
        {
            urlIdx = 0;
        }
    }
    else
    {
        urlIdx--;
        if (urlIdx < 0)
        {
            urlIdx = g_pLivetv->currPg.urlCnt - 1;
        }
    }

    return urlIdx;
}

static RET_CODE auto_change_url_src()
{
    s16 urlIdx;

    g_pLivetv->isAutoChange = TRUE;

    if (!ui_livetv_check_net())
    {
        return SUCCESS;
    }

    urlIdx = ui_livetv_get_next_url_idx();
    g_pLivetv->currPg.urlIdx = urlIdx;
    ui_livetv_storage_set_pref_chan_url(g_pLivetv->currPg.id, urlIdx);

    // play next channel
    if (urlIdx == g_pLivetv->circleIdx)
    {
        live_tv_open_dlg(IDS_PLAY_ERROR, 2000);
        ui_livetv_change_prog(MSG_FOCUS_UP);

        return SUCCESS;
    }
    
    live_tv_open_dlg(IDS_STARTING_CHANGE_URL, 0);
    if (IS_LIST_HIDE())
    {
        show_infobar_while_change_pg();
    }

    ui_livetv_play_net_prog(&g_pLivetv->currPg);

	return SUCCESS;
}

/************************************************
change url src functions

************************************************/

static RET_CODE on_live_tv_change_url_src(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	s16 urlIdx;

    if(!ui_livetv_check_net())
    {
        return SUCCESS;
    }

    /*********don't change url src index if url cnt == 1********/
    if (g_pLivetv->currPg.urlCnt == 1)
    {
        if (ctrl_get_sts(g_pLivetv->hPanel) == OBJ_STS_HIDE)
        {
            show_infobar_while_change_pg();
        }
        return SUCCESS;
    }

    g_pLivetv->isAutoChange = FALSE;

    if (MSG_ADD_URL_SRC == msg)
    {
        g_pLivetv->isLeft = FALSE;
    }
    else
    {
        g_pLivetv->isLeft = TRUE;
    }
    urlIdx = ui_livetv_get_next_url_idx();

    g_pLivetv->currPg.urlIdx = urlIdx;
    g_pLivetv->circleIdx = g_pLivetv->currPg.urlIdx;
    ui_livetv_storage_set_pref_chan_url(g_pLivetv->currPg.id, g_pLivetv->currPg.urlIdx);

    show_infobar_while_change_pg();

    ui_livetv_play_net_prog(&g_pLivetv->currPg);

	return SUCCESS;
}


static void live_tv_all_list_hide(void)
{
	control_t *p_active = NULL;

    ctrl_set_sts(g_pLivetv->hChanListHelpBar, OBJ_STS_HIDE);
    ctrl_set_sts(g_pLivetv->hChanListCont, OBJ_STS_HIDE);
    ctrl_set_sts(g_pLivetv->hGroupHelp, OBJ_STS_HIDE);
    ctrl_set_sts(g_pLivetv->hGroupListCont, OBJ_STS_HIDE);
    p_active = ctrl_get_active_ctrl(g_pLivetv->hMainWin);
    OS_PRINTF("##%s, active id:%d##\n", __FUNCTION__, p_active->id);
    if(p_active->id != IDC_LIVE_TV_NUM_FRM)
    {
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(g_pLivetv->hChanList, MSG_GETFOCUS, 0, 0);
    }

    ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);
}

static RET_CODE on_live_tv_hide_all_list(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    live_tv_all_list_hide();               
    ui_livetv_stop_timer(MSG_HIDE_ALL_LIST);

    return SUCCESS;
}

static void live_tv_exit_info_show()
{
    if(ctrl_get_sts(g_pLivetv->hAlertDlg) == OBJ_STS_HIDE)
    {
        ctrl_set_sts(g_pLivetv->hAlertDlg, OBJ_STS_SHOW);
    }

    ctrl_paint_ctrl(g_pLivetv->hAlertDlg, TRUE);
}

static void live_tv_exit_info_hide()
{
    if(ctrl_get_sts(g_pLivetv->hAlertDlg) == OBJ_STS_SHOW)
    {
        ctrl_set_sts(g_pLivetv->hAlertDlg, OBJ_STS_HIDE);
    }

    ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);
}

static void stop_all_timer_before_exit_livetv()
{
    ui_livetv_stop_timer(MSG_CACEL_EXIT_WINDOW);
    ui_livetv_stop_timer(MSG_HIDE_INFO_BAR);
    ui_livetv_stop_timer(MSG_HIDE_ALL_LIST);
    ui_livetv_stop_timer(MSG_NUM_PLAY);
}

static RET_CODE on_live_tv_list_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    BOOL is_exit = FALSE;
    mul_fp_play_state_t _state;

    _state = ui_video_c_get_play_state();
    if(OBJ_STS_HIDE == ctrl_get_sts(g_pLivetv->hChanListCont))
    {
        ui_live_tv_get_exit_state(&is_exit);
        if(!is_exit)
        {
            ui_comm_dlg_close();
            live_tv_exit_info_show();
            ui_livetv_start_timer(MSG_CACEL_EXIT_WINDOW, 2000);
            ui_live_tv_set_exit_state(TRUE);
        }
        else
        {
            g_pLivetv->stopCfm = TRUE;
            ui_comm_dlg_close();
            ui_live_tv_set_exit_state(FALSE);
            if(_state == MUL_PLAY_STATE_NONE || _state == MUL_PLAY_STATE_STOP)
            {
                stop_all_timer_before_exit_livetv();
                ui_comm_dlg_close();
                manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
            }
            else
            {
                ui_video_c_stop();
            }
        }
    }
    else
    {
        live_tv_all_list_hide();
    }

    return SUCCESS;
}

 static RET_CODE on_live_tv_list_driect_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    mul_fp_play_state_t _state;
    _state = ui_video_c_get_play_state();
    g_pLivetv->stopCfm = TRUE;
    ui_comm_dlg_close();
    ui_live_tv_set_exit_state(FALSE);
    if(_state == MUL_PLAY_STATE_NONE || _state == MUL_PLAY_STATE_STOP)
    {
        stop_all_timer_before_exit_livetv();
        ui_comm_dlg_close();
        manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
    }
    else
    {
        ui_video_c_stop();
    }

    return SUCCESS;
}

static RET_CODE on_live_tv_exit_window(control_t *p_list, u16 msg,
									   u32 para1, u32 para2)
{
    ui_livetv_stop_timer(MSG_CACEL_EXIT_WINDOW);
    ui_live_tv_set_exit_state(FALSE);
    live_tv_exit_info_hide();

    return SUCCESS;
}

static RET_CODE on_live_tv_list_change_group(u16 msg)
{
    u16 group_focus = 0;
    u32 count = 0;
    u16 list_focus;
    u8 group_index;

    OS_PRINTF("@@@%s\n", __FUNCTION__);

    cbox_class_proc(g_pLivetv->hGroup, msg, 0, 0);

    group_focus = cbox_dync_get_focus(g_pLivetv->hGroup);
    OS_PRINTF("###%s,%d,  change group focus == %d##\n",__FUNCTION__,__LINE__,group_focus);
    ctrl_set_sts(g_pLivetv->hChanListHelpBar, OBJ_STS_SHOW);

    if (group_focus < g_pLivetv->favGrp)
    {
        if (group_focus == g_pLivetv->allGrp)
        {
            count = g_pLivetv->totalChan;
        }
        else
        {
            count = ui_livetv_storage_get_chan_cnt_by_group_index(group_focus - 1);
        }
    }
    else if (group_focus == g_pLivetv->favGrp)//fav
    {
        sys_status_get_fav_livetv_total_num(&count);
        OS_PRINTF("##%s, line[%d], fav count = %d##\n", __FUNCTION__, __LINE__, count);
    }

    OS_PRINTF("##%s, this group count == %d##\n", __FUNCTION__, count);
    list_set_count(g_pLivetv->hChanList, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
    group_index = g_pLivetv->currGrpIdx;
    if (group_focus == group_index)
    {
        list_set_focus_pos(g_pLivetv->hChanList, g_pLivetv->playingFocus);
        list_select_item(g_pLivetv->hChanList, g_pLivetv->playingFocus);
    }
    else
    {
        if (group_focus == g_pLivetv->allGrp)
        {
            list_focus = ui_livetv_storage_get_chan_pos_by_id(g_pLivetv->currPg.id);
            list_set_focus_pos(g_pLivetv->hChanList, list_focus);
            list_select_item(g_pLivetv->hChanList, list_focus);
        }
        else
        {
            if (count > 0)
            {
                list_set_focus_pos(g_pLivetv->hChanList, 0);
            }
            else
            {
                list_set_focus_pos(g_pLivetv->hChanList, INVALID);
            }
        }
    }

    list_set_update(g_pLivetv->hChanList, live_tv_list_update, 0);
    live_tv_list_update(g_pLivetv->hChanList, list_get_valid_pos(g_pLivetv->hChanList), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
    ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);

    return SUCCESS;
}


static RET_CODE on_live_tv_list_change_focus_or_program(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    if (OBJ_STS_SHOW == ctrl_get_sts(g_pLivetv->hChanListCont))
    {
        ui_livetv_reset_hide_all_list();
        list_class_proc(g_pLivetv->hChanList, msg, para1, para2);
    }
    else
    {
        g_pLivetv->isAutoChange = FALSE;
        ui_livetv_change_prog(msg);
    }

    return SUCCESS;

}

static BOOL ui_livetv_check_focus_root()
{
    u8 focus_root;

    focus_root = fw_get_focus_id();

    if(ROOT_ID_LIVE_TV != focus_root && ROOT_ID_VOLUME_USB != focus_root)
    {
        return FALSE;
    }

    return TRUE;
}

static RET_CODE on_live_tv_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    ui_comm_loading_hide();
    ui_comm_dlg_close();
    if(!ui_livetv_check_focus_root())
    {
        return SUCCESS;
    }
    OS_PRINTF("###########on_live_tv_load_media_success#########\n");
    g_pLivetv->playStoped = FALSE;

    return SUCCESS;
}

static void ui_livetv_print_load_media_error_msg(u16 msg)
{
    if(msg == MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT)
    {
        UI_PRINTF("##load media error msg is[MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT]##\n");
    }
    else if(msg == MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR)
    {
        UI_PRINTF("##load media error msg is[MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR]##\n");
    }
    else if(msg == MSG_VIDEO_EVENT_SET_PATH_FAIL)
    {
        UI_PRINTF("##load media error msg is[MSG_VIDEO_EVENT_SET_PATH_FAIL]##\n");
    }
    else
    {
        UI_PRINTF("##load media error msg is[UNKNOWN]##\n");
    }
}

static RET_CODE on_live_tv_load_media_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u8 group_index;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    ui_comm_loading_hide();
    ui_comm_dlg_close();
    ui_livetv_print_load_media_error_msg(msg);
    if (!ui_livetv_check_focus_root())
    {
        return SUCCESS;
    }

    group_index = g_pLivetv->currGrpIdx;

    if (!ui_livetv_check_net())
    {
        return SUCCESS;
    }

    auto_change_url_src();

    return SUCCESS;
}

static RET_CODE on_live_tv_request_to_change_url_src(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("##%s##\n", __FUNCTION__);

    if(!ui_livetv_check_focus_root())
    {
        return SUCCESS;
    }

    if(g_pLivetv->currPg.urlCnt > 1)
    {
        live_tv_open_dlg(IDS_IF_CHANGE_URL, 2000);
    }
    else
    {
        live_tv_open_dlg(IDS_IF_CHANGE_CHAL, 2000);
    }

    return SUCCESS;
}


static RET_CODE on_live_tv_net_play_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
    OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);

    if(TRUE == g_pLivetv->stopCfm)
    {
        ui_comm_dlg_close();
        stop_all_timer_before_exit_livetv();
        manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
    }

    g_pLivetv->stopCfm = FALSE;
    return SUCCESS;
}

static RET_CODE on_live_tv_net_play_load_media_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
    OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);

    if(TRUE == g_pLivetv->stopCfm)
    {
        OS_PRINTF("##########load media stop cfm, will exit livetv####\n");
        ui_comm_dlg_close();
        stop_all_timer_before_exit_livetv();
        manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
        g_pLivetv->stopCfm = FALSE;
    }
    else
    {
        OS_PRINTF("##########load media stop cfm, load media error now####\n");
        on_live_tv_load_media_error(NULL, msg, 0, 0);
    }

    return SUCCESS;
	
}


static livetv_fav_sts ui_livetv_get_fav_status(ui_livetv_chan_info_t *p_pg)
{
    live_tv_fav_info fav_info;
    live_tv_fav_info fav_livetv_info;
    u16 i;
    u32 g_fav_prog_num;
    u16 uni[MAX_LIVETV_NAME_LEN + 1] = {0};

    sys_status_get_fav_livetv_total_num(&g_fav_prog_num);

    for (i = 0; i < g_fav_prog_num; i++)
    {
        sys_status_get_fav_livetv_info(i, &fav_livetv_info);
        if(fav_livetv_info.live_fav_id == p_pg->id)
        {
            sys_status_del_fav_livetv_info(i);
            sys_status_save();
            OS_PRINTF("###this is fav prog, will delete fav##\n");
            return DEL_FAV;
        }
    }

    if (g_fav_prog_num >= FAV_LIVETV_NUM)
    {
        OS_PRINTF("#####fav num is max number##\n");
        return FULL_FAV;
    } 

    fav_info.live_fav_id = p_pg->id;
    str_asc2uni(fav_info.live_fav_name, uni);
    uni_strcpy(uni, p_pg->name);
    sys_status_set_fav_livetv_info(g_fav_prog_num, &fav_info);
    g_fav_prog_num++;
    sys_status_set_fav_livetv_total_num(g_fav_prog_num);
    sys_status_save();

    return ADD_FAV;
}


static RET_CODE on_live_tv_add_fav_prog(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_livetv_chan_info_t p_pg;
    livetv_fav_sts fav_status;
    u8 group_focus;
    u16 list_focus;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    group_focus = cbox_dync_get_focus(g_pLivetv->hGroup);

    ui_livetv_reset_hide_all_list();
    if (OBJ_STS_HIDE == ctrl_get_sts(g_pLivetv->hChanListCont))
    {
        fav_status = ui_livetv_get_fav_status(&g_pLivetv->currPg);
        if (fav_status == ADD_FAV)
        {
            live_tv_open_dlg(IDS_LIVETV_ADD_ONE_FAV, 500);
        }
        else if (fav_status == DEL_FAV)
        {
            live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
        }
        else if (fav_status == FULL_FAV)
        {
            live_tv_open_dlg(IDS_FAV_PROG_IS_FULL, 500);
        }
        return SUCCESS;
    }

    list_focus = list_get_focus_pos(ctrl);
    if (group_focus < g_pLivetv->favGrp)
    {
        if (group_focus == 0)
        {
            ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, LIVE_TV_INVALID_INDEX, list_focus);
        }
        else
        {
            ui_livetv_storage_get_chan_info_by_grp_idx_and_chan_pos(&p_pg, group_focus - 1, list_focus);
        }
        fav_status = ui_livetv_get_fav_status(&p_pg);
        if (fav_status == ADD_FAV)
        {
            list_set_field_content_by_icon(ctrl,list_focus, 2, IM_TV_FAV);
            live_tv_open_dlg(IDS_LIVETV_ADD_ONE_FAV, 500);
        }
        else if (fav_status == DEL_FAV)
        {
            list_set_field_content_by_icon(ctrl,list_focus, 2, 0);
            live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
        }
        else if (fav_status == FULL_FAV)
        {
            live_tv_open_dlg(IDS_FAV_PROG_IS_FULL, 500);
            return SUCCESS;
        }
        live_tv_list_update(ctrl, list_get_valid_pos(ctrl), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
        list_class_proc(ctrl, MSG_FOCUS_DOWN, 0, 0);

        ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);
    }
    else if (group_focus == g_pLivetv->favGrp)//fav grp
    {
        u32 cur_fav_cnt;
        sys_status_get_fav_livetv_total_num(&cur_fav_cnt);
        if (cur_fav_cnt == 0)
        {
            list_set_count(ctrl, cur_fav_cnt, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
            live_tv_list_update(ctrl, list_get_valid_pos(ctrl), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
            if (g_pLivetv->currGrpIdx == g_pLivetv->favGrp)
            {
                g_pLivetv->currGrpIdx = g_pLivetv->allGrp;
                g_pLivetv->playingFocus = ui_livetv_storage_get_chan_pos_by_id(g_pLivetv->currPg.id);
            }
            return SUCCESS;
        }
        sys_status_del_fav_livetv_info(list_focus);
        sys_status_save();
        list_set_field_content_by_icon(ctrl,list_focus, 2, 0);
        live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
        sys_status_get_fav_livetv_total_num(&cur_fav_cnt);
        if (cur_fav_cnt != 0)
        {
            list_set_focus_pos(ctrl, 0);
        }
        list_set_count(ctrl, cur_fav_cnt, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
        live_tv_list_update(ctrl, list_get_valid_pos(ctrl), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
        ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);
    }
	return SUCCESS;
}



static RET_CODE on_live_tv_list_change_group_or_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    if(OBJ_STS_HIDE == ctrl_get_sts(g_pLivetv->hChanListCont))
    {
        if(OBJ_STS_HIDE == ctrl_get_sts(g_pLivetv->hGroupListCont))
        {   
            manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_LIVE_TV, para1);
            OS_PRINTF("##live tv change volume! %s##\n", __FUNCTION__);
        }
        else
        {
            live_tv_all_list_hide();                
        }
    }
    else
    {
        ui_livetv_reset_hide_all_list();
        on_live_tv_list_change_group(msg);
    }

    return SUCCESS;
}

/************************************************
number play functions

************************************************/
static BOOL live_tv_input_num(u8 num, BOOL is_update)
{
    control_t *p_subctrl;
    u32 curn;

    if (g_pLivetv->numBit < LIVE_TV_NUM_PLAY_CNT)
    {
        p_subctrl = ctrl_get_child_by_id(g_pLivetv->hChanNum, IDC_LIVE_TV_NUM_TXT);

        curn = nbox_get_num(p_subctrl);
        curn = curn * 10 + num;
        nbox_set_num_by_dec(p_subctrl, curn);

        g_pLivetv->numBit++;
        if (is_update)
        {
            ctrl_paint_ctrl(p_subctrl, TRUE);
        }

        return TRUE;
    }

    return FALSE;
}

static RET_CODE on_live_tv_show_number_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_active = NULL;
    u8 group_focus;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_active = ctrl_get_active_ctrl(g_pLivetv->hMainWin);

    group_focus = cbox_dync_get_focus(g_pLivetv->hGroup);
    ctrl_set_sts(g_pLivetv->hChanNum, OBJ_STS_SHOW);

    g_pLivetv->numBit = 0;
    live_tv_input_num((u8)(para1 - V_KEY_0), FALSE);

    ctrl_change_focus(p_active, g_pLivetv->hChanNum);
    ui_livetv_start_timer(MSG_NUM_PLAY, 3000);

    return SUCCESS;
}

static RET_CODE on_live_tv_input_num(control_t *p_ctrl, u16 msg, 
									 u32 para1, u32 para2)
{
    u8 num = (u8)(MSG_DATA_MASK & msg);
    live_tv_input_num(num, TRUE);

    return SUCCESS;
}

static void ui_livetv_play_by_chan_num(u16 num)
{
    ui_livetv_chan_info_t p_pg = {0};
    s32 chan_pos;

    chan_pos = ui_livetv_storage_get_chan_info_by_id(&p_pg, num);
    if (chan_pos < 0)
    {
        live_tv_open_dlg(ROOT_ID_LIVE_TV, IDS_MSG_INVALID_NUMBER);
        return;
    }

    g_pLivetv->currGrpIdx = g_pLivetv->allGrp;
    g_pLivetv->playingFocus = chan_pos;

    memcpy(&g_pLivetv->prevPg, &g_pLivetv->currPg, sizeof(ui_livetv_chan_info_t));
    memcpy(&g_pLivetv->currPg, &p_pg, sizeof(ui_livetv_chan_info_t));
    g_pLivetv->isLeft = FALSE;
    g_pLivetv->circleIdx = g_pLivetv->currPg.urlIdx;

    if (IS_LIST_HIDE())
    {
        show_infobar_while_change_pg();
    }
    else
    {
        list_set_focus_pos(g_pLivetv->hChanList, g_pLivetv->playingFocus);
        list_select_item(g_pLivetv->hChanList, g_pLivetv->playingFocus);
        ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);
    }

    ui_livetv_play_net_prog(&g_pLivetv->currPg);
}


static RET_CODE on_live_tv_exit_num_play(control_t *p_ctrl, u16 msg, 
										 u32 para1, u32 para2)
{
    control_t *p_num_txt = NULL;
    control_t *p_active = NULL;

    if(fw_find_root_by_id(ROOT_ID_LIVE_TV) == NULL)
    {
        return SUCCESS; //fixed Bug 58442
    }

    p_active = ctrl_get_active_ctrl(g_pLivetv->hMainWin);
    p_num_txt = ctrl_get_child_by_id(p_ctrl, IDC_LIVE_TV_NUM_TXT);
    ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
    ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
    if(ctrl_get_sts(g_pLivetv->hGroupListCont) == OBJ_STS_HIDE)
    {
        ctrl_process_msg(g_pLivetv->hChanList, MSG_GETFOCUS, 0, 0);
    }
    else
    {
        ctrl_process_msg(g_pLivetv->hGroupList, MSG_GETFOCUS, 0, 0);
    }

    ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
    nbox_set_num_by_dec(p_num_txt, 0);
    ui_livetv_stop_timer(MSG_NUM_PLAY);

    return ERR_NOFEATURE;
}

static RET_CODE on_live_tv_num_select(control_t *p_ctrl, u16 msg, 
									  u32 para1, u32 para2)
{
    control_t *p_subctrl = NULL;
    u16 curn;
    u8 group_index;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_pLivetv->vcircleIdx = INVALIDID;

    group_index = g_pLivetv->currGrpIdx;

    g_pLivetv->isAutoChange = FALSE;
    p_subctrl = ctrl_get_child_by_id(g_pLivetv->hChanNum, IDC_LIVE_TV_NUM_TXT);
    curn = (u16)nbox_get_num(p_subctrl);

    /* jump to prog */
    if(ui_livetv_check_net())
    {
        ui_livetv_play_by_chan_num(curn);
    }
    on_live_tv_exit_num_play(g_pLivetv->hChanNum, msg, para1, para2);

    return SUCCESS;

}


/******************************update epg  details***********************/

static void show_conn_net_type()
{
    u16 net_sts[4] = 
    {
        IDS_ETHERNET,
        IDS_WIFI,
        IDS_3G,
        IDS_NONE
    };
    control_t *p_net = NULL;
    net_conn_stats_t sts = {0};

    sts = ui_get_net_connect_status();
    p_net = ctrl_get_child_by_id(g_pLivetv->hPanel, IDC_EPG_NET_CONN_TEXT);
    if(sts.is_eth_conn)
    {
        text_set_content_by_strid(p_net, net_sts[0]);
    }
    else if(sts.is_wifi_conn)
    {
        text_set_content_by_strid(p_net, net_sts[1]);
    }
    else if(sts.is_3g_conn)
    {
        text_set_content_by_strid(p_net, net_sts[2]);
    }
    else
    {
        text_set_content_by_strid(p_net, net_sts[3]);
    }
}

static void ui_livetv_update_infobar(control_t *hPanel, BOOL is_show)
{
    control_t *p_sub = NULL;
    u16 uni_str[MAX_LIVETV_NAME_LEN+1+LIVE_TV_NUM_PLAY_CNT+1];
    u8 asc_str[MAX_LIVETV_NAME_LEN+1+LIVE_TV_NUM_PLAY_CNT+1];
    u8 time_str[32] = {0};
    u8 temp_asc[16] = "None";
    utc_time_t time = {0};
    utc_time_t gmt_time = {0};
    time_set_t p_set={{0}};

    //program name
    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_CURR_P_NAME);

    sprintf(asc_str, "%d ", g_pLivetv->currPg.id);
    str_asc2uni(asc_str, uni_str);
    uni_strcat(uni_str, g_pLivetv->currPg.name, MAX_LIVETV_NAME_LEN+1+LIVE_TV_NUM_PLAY_CNT);
    text_set_content_by_unistr(p_sub, uni_str);

    //current time
    sys_status_get_time(&p_set);
    time_get(&time, p_set.gmt_usage);
    if( p_set.gmt_usage)
    {
        time_to_local(&time, &gmt_time);
        memcpy(&time,&gmt_time,sizeof(gmt_time));
    }

    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_CURR_TIME_TEXT);
    sprintf(time_str, "%.4d-%.2d-%.2d  %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
    text_set_content_by_ascstr(p_sub, time_str);

    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_URL_SRC);
    text_set_content_by_strid(p_sub, IDS_LIVETV_CHANGE_SRC);

    //current network connect status
    show_conn_net_type();

    //src url number
    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_URL_SRC_NUM);
    sprintf(asc_str,"(%d/%d)", g_pLivetv->currPg.urlIdx + 1, g_pLivetv->currPg.urlCnt);
    text_set_content_by_ascstr(p_sub, asc_str);

    //current prog name 
    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_CURR_PROG_NAME);
    convert_gb2312_chinese_asc2unistr(temp_asc, uni_str, sizeof(temp_asc));
    text_set_content_by_unistr(p_sub, uni_str);

    //next prog name 
    p_sub = ctrl_get_child_by_id(hPanel, IDC_EPG_NEXT_PROG_NAME);
    text_set_content_by_unistr(p_sub, uni_str);
}

static void show_infobar_while_change_pg(void)
{
    if(ctrl_get_sts(g_pLivetv->hPanel) == OBJ_STS_SHOW)
    {
        ui_livetv_stop_timer(MSG_HIDE_INFO_BAR);
        ctrl_set_sts(g_pLivetv->hPanel, OBJ_STS_HIDE);
    }
    fw_notify_root(g_pLivetv->hMainWin, NOTIFY_T_MSG, TRUE, MSG_SHOW_INFO_BAR, 0, 0);
}

static RET_CODE on_live_tv_show_infobar(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_cont = NULL;

    p_cont = g_pLivetv->hMainWin;
    if(ctrl_get_sts(g_pLivetv->hChanListCont) == OBJ_STS_SHOW)
    {
        ctrl_set_sts(g_pLivetv->hChanListHelpBar, OBJ_STS_HIDE);
        ctrl_set_sts(g_pLivetv->hChanListCont, OBJ_STS_HIDE);
    }
    
    if(ctrl_get_sts(g_pLivetv->hGroupListCont) == OBJ_STS_SHOW)
    {
        ctrl_set_sts(g_pLivetv->hGroupHelp, OBJ_STS_HIDE);
        ctrl_set_sts(g_pLivetv->hGroupListCont, OBJ_STS_HIDE);
    }

    ctrl_set_sts(g_pLivetv->hPanel, OBJ_STS_SHOW);
    ui_livetv_start_timer(MSG_HIDE_INFO_BAR, 2000);

    ui_livetv_update_infobar(g_pLivetv->hPanel, TRUE);

    ctrl_paint_ctrl(p_cont, TRUE);

    return SUCCESS;
}

static RET_CODE on_live_tv_hide_infobar(control_t *p_ctrl, u16 msg,
											 u32 para1, u32 para2)
{
    ctrl_set_sts(g_pLivetv->hPanel, OBJ_STS_HIDE);
    ui_livetv_stop_timer(MSG_HIDE_INFO_BAR);

    ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);

    return SUCCESS;
}

static RET_CODE on_live_tv_recall(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
    ui_livetv_chan_info_t temp_pg;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    g_pLivetv->vcircleIdx = INVALIDID;
    g_pLivetv->isAutoChange = FALSE;

    ctrl_set_sts(g_pLivetv->hChanListCont, OBJ_STS_HIDE);
    ctrl_paint_ctrl(g_pLivetv->hChanListCont, TRUE);
    if(g_pLivetv->prevPg.name != NULL)
    {
        if(uni_strlen(g_pLivetv->prevPg.name) > 0)
        {
            memcpy(&temp_pg, &g_pLivetv->currPg, sizeof(ui_livetv_chan_info_t));
            memcpy(&g_pLivetv->currPg, &g_pLivetv->prevPg, sizeof(ui_livetv_chan_info_t));
            memcpy(&g_pLivetv->prevPg, &temp_pg, sizeof(ui_livetv_chan_info_t));

            g_pLivetv->currGrpIdx = g_pLivetv->allGrp;
            g_pLivetv->circleIdx = g_pLivetv->currPg.urlIdx;
            g_pLivetv->playingFocus = ui_livetv_storage_get_chan_pos_by_id(g_pLivetv->currPg.id);
            OS_PRINTF("##%s, line[%d], playing focus = %d##\n", __FUNCTION__, __LINE__, g_pLivetv->playingFocus);

            if(IS_LIST_HIDE())
            {
                show_infobar_while_change_pg();
            }

            ui_livetv_play_net_prog(&g_pLivetv->currPg);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_livetv_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    al_livetv_release_custom_chan_list();

    ui_video_c_destroy();

    return ERR_NOFEATURE;
}

static RET_CODE on_live_tv_mute(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("##%s##\n", __FUNCTION__);
    ui_set_mute(!ui_is_mute());

    return SUCCESS;
}


/************************************************
show bps and downloading percent

************************************************/
void ui_livetv_get_bps_infos(u32 param, u16 *percent, u16 *bps)
{
    *percent = param >> 16;  //high 16bit saved percent
    *bps = param & 0xffff;  //low 16 bit saved bps
}

static RET_CODE on_live_tv_show_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u16 net_bps = 0;
    u16 percent = 0;

    ui_livetv_get_bps_infos(para1, &percent, &net_bps);
    OS_PRINTF("##%s, para1 = %d, net_bps = %d, load percent = %d##\n", __FUNCTION__, para1, net_bps, percent);
    ui_comm_loading_show(LOAD_BPS, 0, net_bps, percent);

    return SUCCESS;
	
}


static RET_CODE on_live_tv_cacel_bps(control_t *p_cont, u16 msg,
									 u32 para1, u32 para2)
{
    ui_comm_dlg_close();
    ui_comm_loading_hide();

    return SUCCESS;
}

/************************************************
show time while loading media

************************************************/
static RET_CODE on_live_tv_show_loading_media_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    if(para1 <= LIVETV_LOAD_MEDIA_MAX_TIME)
    {
        ui_comm_loading_show(LOAD_TIME, para1, 0, 0);
    }
    else
    {
        ui_comm_loading_show(LOAD_TIME, para1, 0, 0);
        if(g_pLivetv->isAutoChange == FALSE)// manaully change url
        {
            if(g_pLivetv->currPg.urlCnt > 1)
            {
                live_tv_open_dlg(IDS_IF_CHANGE_URL, 0);
            }
            else
            {
                live_tv_open_dlg(IDS_IF_CHANGE_CHAL, 0);
            }
        }
        else
        {
            UI_PRINTF("####%s, auto change url over 8 sec, will change next url or prog###\n", __FUNCTION__);
            auto_change_url_src();
        }
    }
	
    return SUCCESS;
    
}


static RET_CODE on_live_tv_open_select_group_list(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    control_t *p_active = NULL;
    u16 *p_uni_str = NULL;
    u8 group_index;
    u16 list_focus;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (ctrl_get_sts(g_pLivetv->hGroupListCont) == OBJ_STS_HIDE)
    {
        group_index = g_pLivetv->currGrpIdx;
        live_tv_all_list_hide();
        close_infobar_while_show_pg_list();

        ctrl_set_sts(g_pLivetv->hGroupListCont, OBJ_STS_SHOW);

        if(group_index == g_pLivetv->favGrp)
        {
            list_focus = 0;
        }
        else
        {
            list_focus = group_index + 1;
        }

        list_set_focus_pos(g_pLivetv->hGroupList, list_focus);        
        live_tv_group_list_update(g_pLivetv->hGroupList, list_get_valid_pos(g_pLivetv->hGroupList), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
        p_active = ctrl_get_active_ctrl(g_pLivetv->hMainWin);
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(g_pLivetv->hGroupList, MSG_GETFOCUS, 0, 0);
        p_uni_str = (u16 *)list_get_field_content(g_pLivetv->hGroupList, list_focus, 1);
        text_set_content_by_unistr(g_pLivetv->hGroupName, p_uni_str);
        ctrl_paint_ctrl(g_pLivetv->hMainWin, TRUE);
    }
    ui_livetv_reset_hide_all_list();

    return SUCCESS;
}

static RET_CODE on_live_tv_group_list_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    OS_PRINTF("########%s#####\n", __FUNCTION__);
    live_tv_all_list_hide();
    return SUCCESS;
}

static RET_CODE on_live_tv_list_select_group(control_t *p_list, u16 msg,
															u32 para1, u32 para2)
{
    u8 group_index;
    u16 list_focus;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    list_focus = list_get_focus_pos(g_pLivetv->hGroupList);

    if(list_focus == 0)
    {
        group_index = g_pLivetv->favGrp;
    }
    else
    {
        group_index = list_focus - 1;
    }

    cbox_dync_set_focus(g_pLivetv->hGroup, group_index);  

    live_tv_all_list_hide();

    on_livetv_select_to_show_proglist(TRUE);
    return SUCCESS;
}

static RET_CODE on_live_tv_group_list_change_focus(control_t *p_ctrl, u16 msg,
														u32 para1, u32 para2)
{
    OS_PRINTF("#####%s######\n", __FUNCTION__);
    ui_livetv_reset_hide_all_list();
    list_class_proc(g_pLivetv->hGroupList, msg, para1, para2);

    return SUCCESS;
}


/************************************************
All keymap and proc

************************************************/
BEGIN_KEYMAP(live_tv_mainwin_keymap, NULL)
    ON_EVENT(V_KEY_INFO, MSG_SHOW_INFO_BAR)
    ON_EVENT(V_KEY_0, MSG_NUMBER)
    ON_EVENT(V_KEY_1, MSG_NUMBER)
    ON_EVENT(V_KEY_2, MSG_NUMBER)
    ON_EVENT(V_KEY_3, MSG_NUMBER)
    ON_EVENT(V_KEY_4, MSG_NUMBER)
    ON_EVENT(V_KEY_5, MSG_NUMBER)
    ON_EVENT(V_KEY_6, MSG_NUMBER)
    ON_EVENT(V_KEY_7, MSG_NUMBER)
    ON_EVENT(V_KEY_8, MSG_NUMBER)
    ON_EVENT(V_KEY_9, MSG_NUMBER)
    ON_EVENT(V_KEY_RECALL, MSG_RECALL)
    ON_EVENT(V_KEY_RED, MSG_REDUCE_URL_SRC)
    ON_EVENT(V_KEY_GREEN, MSG_ADD_URL_SRC)
    ON_EVENT(V_KEY_YELLOW, MSG_SELECT_GROUP)
    ON_EVENT(V_KEY_MUTE, MSG_MUTE)
END_KEYMAP(live_tv_mainwin_keymap, NULL)

BEGIN_MSGPROC(live_tv_mainwin_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_NUMBER, on_live_tv_show_number_play)
    ON_COMMAND(MSG_SHOW_INFO_BAR, on_live_tv_show_infobar)
    ON_COMMAND(MSG_HIDE_INFO_BAR, on_live_tv_hide_infobar)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_live_tv_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, on_live_tv_net_play_stop_cfm)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, on_live_tv_net_play_load_media_stop_cfm)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_live_tv_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC, on_live_tv_request_to_change_url_src)
    ON_COMMAND(MSG_VIDEO_EVENT_SET_PATH_FAIL, on_live_tv_load_media_error)
    ON_COMMAND(MSG_DESTROY, ui_livetv_on_destory)
    ON_COMMAND(MSG_EXIT_ALL, on_live_tv_list_driect_exit)
    ON_COMMAND(MSG_RECALL, on_live_tv_recall)
    ON_COMMAND(MSG_CACEL_EXIT_WINDOW, on_live_tv_exit_window)
    ON_COMMAND(MSG_ADD_URL_SRC, on_live_tv_change_url_src)
    ON_COMMAND(MSG_REDUCE_URL_SRC, on_live_tv_change_url_src)
    ON_COMMAND(MSG_MUTE, on_live_tv_mute)
    ON_COMMAND(MSG_HIDE_ALL_LIST, on_live_tv_hide_all_list)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, on_live_tv_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_UPDATE_BPS, on_live_tv_show_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_TIME, on_live_tv_show_loading_media_time)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_BUFFERING, on_live_tv_cacel_bps)
    ON_COMMAND(MSG_VIDEO_EVENT_FINISH_UPDATE_BPS, on_live_tv_cacel_bps)
    ON_COMMAND(MSG_SELECT_GROUP, on_live_tv_open_select_group_list)
END_MSGPROC(live_tv_mainwin_proc, ui_comm_root_proc)

BEGIN_KEYMAP(live_tv_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
    ON_EVENT(V_KEY_LEFT,MSG_DECREASE)
    ON_EVENT(V_KEY_RIGHT,MSG_INCREASE)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_FAV, MSG_ADD_FAV_PG)
END_KEYMAP(live_tv_list_keymap, NULL)

BEGIN_MSGPROC(live_tv_list_proc, list_class_proc)
    ON_COMMAND(MSG_DECREASE, on_live_tv_list_change_group_or_volume)
    ON_COMMAND(MSG_INCREASE, on_live_tv_list_change_group_or_volume)
    ON_COMMAND(MSG_FOCUS_UP, on_live_tv_list_change_focus_or_program)
    ON_COMMAND(MSG_FOCUS_DOWN, on_live_tv_list_change_focus_or_program)
    ON_COMMAND(MSG_PAGE_UP, on_live_tv_list_change_focus_or_program)
    ON_COMMAND(MSG_PAGE_DOWN, on_live_tv_list_change_focus_or_program)
    ON_COMMAND(MSG_EXIT, on_live_tv_list_exit)
    ON_COMMAND(MSG_SELECT, on_livetv_proglist_select_to_play_or_show_list)
    ON_COMMAND(MSG_ADD_FAV_PG, on_live_tv_add_fav_prog)
END_MSGPROC(live_tv_list_proc, list_class_proc)

BEGIN_KEYMAP(live_tv_num_play_cont_keymap, NULL)
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
    ON_EVENT(V_KEY_OK, MSG_NUM_PLAY)
END_KEYMAP(live_tv_num_play_cont_keymap, NULL)

BEGIN_MSGPROC(live_tv_num_play_cont_proc, cont_class_proc)
    ON_COMMAND(MSG_NUMBER, on_live_tv_input_num)
    ON_COMMAND(MSG_NUM_PLAY, on_live_tv_num_select)
    ON_COMMAND(MSG_EXIT, on_live_tv_exit_num_play)
END_MSGPROC(live_tv_num_play_cont_proc, cont_class_proc)


BEGIN_KEYMAP(live_tv_group_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(live_tv_group_list_keymap, NULL)

BEGIN_MSGPROC(live_tv_group_list_proc, list_class_proc)
    ON_COMMAND(MSG_EXIT, on_live_tv_group_list_exit)
    ON_COMMAND(MSG_SELECT, on_live_tv_list_select_group)
    ON_COMMAND(MSG_FOCUS_UP, on_live_tv_group_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, on_live_tv_group_list_change_focus)
    ON_COMMAND(MSG_PAGE_UP, on_live_tv_group_list_change_focus)
    ON_COMMAND(MSG_PAGE_DOWN, on_live_tv_group_list_change_focus)
END_MSGPROC(live_tv_group_list_proc, list_class_proc)

