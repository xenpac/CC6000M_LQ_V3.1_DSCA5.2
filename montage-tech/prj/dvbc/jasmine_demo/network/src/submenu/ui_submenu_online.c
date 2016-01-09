/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_submenu_online.h"
#include "fifo_db.h"
#include "NetMediaDataProvider.h"
#include "al_netmedia.h"
#include "ui_nm_gprot.h"
#include "ui_video.h"
#include "SM_StateMachine.h"
#include "states_website.h"

enum control_id
{
	IDC_INVALID = 0,
  IDC_DYN_WEBSITE_WEB_CONT,
	IDC_DYN_WEBSITE_WEB_ITEM_CONT_START,
  IDC_DYN_WEBSITE_WEB_ITEM_CONT_END = IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + DYN_WEBSITE_WEB_PAGE_SIZE - 1,
  IDC_DYN_WEBSITE_WEB_PIC,
  IDC_DYN_WEBSITE_WEB_NAME,
};

typedef struct
{  
    control_t *hMenu;
    control_t *hWebCont;

    u8 total_web;
    u8 cur_web;
	u8 total_page;
	u8 cur_page;
    u8 cur_pic;
    al_netmedia_website_item_t *website_list;
    
} ui_nm_dyn_website_t;


static ui_nm_dyn_website_t *g_pNmWeb = NULL;

static u16 dyn_online_movie_web_cont_keymap(u16 key);
static RET_CODE dyn_online_movie_web_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE submenu_network_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void ui_nm_dyn_website_pic_init(void)
{
    ui_pic_init(PIC_SOURCE_NET);
}

static void ui_nm_dyn_website_pic_deinit(void)
{
    pic_stop();
  
    ui_pic_release();
}

static void ui_release_website_list(void)
{
    u16 i;

    if (g_pNmWeb->website_list)
    {
        for (i = 0; i < g_pNmWeb->total_web; i++)
        {
            if (g_pNmWeb->website_list[i].title)
            {
                mtos_free(g_pNmWeb->website_list[i].title);
                g_pNmWeb->website_list[i].title = NULL;
            }
            if (g_pNmWeb->website_list[i].logo_url)
            {
                mtos_free(g_pNmWeb->website_list[i].logo_url);
                g_pNmWeb->website_list[i].logo_url = NULL;
            }
        }
        mtos_free(g_pNmWeb->website_list);
        g_pNmWeb->website_list = NULL;
    }
}

static void ui_release_nm_dyn_website_data(void)
{
    if (g_pNmWeb)
    {
        if (g_pNmWeb->website_list)
        {
          ui_release_website_list();
        }
        mtos_free(g_pNmWeb);
        g_pNmWeb = NULL;
    }
}

static BOOL ui_init_nm_dyn_website_data(void)
{
    if (g_pNmWeb)
    {
        ui_release_nm_dyn_website_data();
    }

    g_pNmWeb = (ui_nm_dyn_website_t *)mtos_malloc(sizeof(ui_nm_dyn_website_t));
    MT_ASSERT(g_pNmWeb != NULL);
    memset((void *)g_pNmWeb, 0, sizeof(ui_nm_dyn_website_t));

    return TRUE;
}

static control_t *ui_nm_dyn_website_creat_web_cont(control_t *p_parent)
{
    control_t *p_web_cont = NULL;
    control_t *p_item = NULL;
    control_t *p_pic = NULL;
    control_t *p_title = NULL;
    u16 i;
    u16 x, y;

    OS_PRINTF("@@@ui_nm_dyn_website_creat_web_cont\n");
    
    p_web_cont = ctrl_create_ctrl(CTRL_CONT, IDC_DYN_WEBSITE_WEB_CONT,
                                  UI_SUBMENU_WEB_CTRL_X, UI_SUBMENU_WEB_CTRL_Y,
                                  UI_SUBMENU_WEB_CTRL_W, UI_SUBMENU_WEB_CTRL_H,
                                  p_parent, 0);
    ctrl_set_rstyle(p_web_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    x = DYN_WEBSITE_WEB_ITEM_X;
    y = DYN_WEBSITE_WEB_ITEM_Y;
    for (i = 0; i < DYN_WEBSITE_WEB_PAGE_SIZE; i++)
    {
        p_item = ctrl_create_ctrl(CTRL_CONT, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + i,
                                  x, y,
                                  DYN_WEBSITE_WEB_ITEM_W, DYN_WEBSITE_WEB_ITEM_H,
                                  p_web_cont, 0);
        ctrl_set_rstyle(p_item, RSI_PBACK, RSI_PBACK, RSI_PBACK);

        p_pic = ctrl_create_ctrl(CTRL_BMAP, IDC_DYN_WEBSITE_WEB_PIC,
                                  DYN_WEBSITE_WEB_PIC_X, DYN_WEBSITE_WEB_PIC_Y,
                                  DYN_WEBSITE_WEB_PIC_W, DYN_WEBSITE_WEB_PIC_H,
                                  p_item, 0);
        ctrl_set_rstyle(p_pic, RSI_TRANSPARENT, RSI_OTT_FRM_1_RED_HL, RSI_TRANSPARENT);
        ctrl_set_mrect(p_pic, 
                        DYN_WEBSITE_WEB_PIC_MIDL, DYN_WEBSITE_WEB_PIC_MIDT,
                        DYN_WEBSITE_WEB_PIC_MIDR, DYN_WEBSITE_WEB_PIC_MIDB);

        p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_DYN_WEBSITE_WEB_NAME,
                                  DYN_WEBSITE_WEB_NAME_X, DYN_WEBSITE_WEB_NAME_Y,
                                  DYN_WEBSITE_WEB_NAME_W, DYN_WEBSITE_WEB_NAME_H,
                                  p_item, 0);
        
        ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        ctrl_set_mrect(p_title, 
                        DYN_WEBSITE_WEB_NAME_MIDL, DYN_WEBSITE_WEB_NAME_MIDT,
                        DYN_WEBSITE_WEB_NAME_MIDR, DYN_WEBSITE_WEB_NAME_MIDB);

        text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_title, TEXT_STRTYPE_EXTSTR);

        x += (DYN_WEBSITE_WEB_ITEM_W + DYN_WEBSITE_WEB_ITEM_H_GAP);
        OS_PRINTF("\n x = [%d], y = [%d]\n", x, y);
        if ((x + DYN_WEBSITE_WEB_PIC_W) > UI_SUBMENU_WEB_CTRL_W)
        {
            x = DYN_WEBSITE_WEB_ITEM_X;
            y += DYN_WEBSITE_WEB_ITEM_H + DYN_WEBSITE_WEB_ITEM_V_GAP;
            OS_PRINTF("\n x = [%d], y = [%d]\n", x, y);
        }
    }

    return p_web_cont;
}

static BOOL ui_nm_dyn_website_set_focus_pos(control_t *p_cont, u16 focus)
{
    control_t *p_active = NULL, *p_next_ctrl = NULL,*p_pic = NULL;
    u16 index;
  
    p_active = p_cont->p_active_child;
    if (p_active != NULL)
    {
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
    }

    OS_PRINTF("\nfocus = [%d]\n", focus);
    index = focus % DYN_WEBSITE_WEB_PAGE_SIZE;
    
    p_next_ctrl = ctrl_get_child_by_id(p_cont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + index);
    p_pic = ctrl_get_child_by_id(p_next_ctrl, IDC_DYN_WEBSITE_WEB_PIC);
    if (p_next_ctrl != NULL)
    {   
        OS_PRINTF("\nindex = [%d]\n", index);
        ctrl_set_active_ctrl(p_next_ctrl,p_pic);
        ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
    }
  
    ctrl_paint_ctrl(p_cont, TRUE);
  
    return TRUE;
}

#if 0
static RET_CODE ui_nm_dyn_website_update_web_cont(control_t *p_cont, u16 start, u16 size, u32 context)
{
    control_t *p_item = NULL;
    control_t *p_pic = NULL;
    control_t *p_name = NULL;   
    u16 i;
	u16 max_web;

	max_web = g_pNmWeb->total_web - (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
    for (i = 0; ((i < max_web) && (i < DYN_WEBSITE_WEB_PAGE_SIZE) ); i++)
    {
        p_item = ctrl_get_child_by_id(p_cont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);
        p_name = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_NAME);
  
        bmap_set_content_by_id(p_pic, IM_APP_LOADING);
        text_set_content_by_extstr(p_name, g_pNmWeb->website_list[ ( g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE + i].title);
        ctrl_paint_ctrl(p_item, TRUE);
    }

    g_pNmWeb->cur_web = (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
    ui_nm_dyn_website_set_focus_pos(p_cont, g_pNmWeb->cur_web);

    return SUCCESS;
}
#endif

RET_CODE open_submenu_online_movie(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_wlist_cont = NULL;
    
    comm_help_data_t help_data = 
    {
        3, 3,
        {IDS_MOVE, IDS_OK, IDS_MENU},
        {IM_CHANGE, IM_HELP_OK, IM_HELP_MENU}
    };

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    //init
    ui_init_nm_dyn_website_data();

    ui_nm_dyn_website_pic_init();
    
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_SUBMENU_NM, 0, 
                                                 IM_INDEX_NETWORK_BANNER, 
                                                 IDS_ONLINE_MOVIE_APP);
    if(p_menu == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_proc(p_menu, submenu_network_proc);
    ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);

    //website cont
    p_wlist_cont = ui_nm_dyn_website_creat_web_cont(p_menu);
    ctrl_set_keymap(p_wlist_cont, dyn_online_movie_web_cont_keymap);
    ctrl_set_proc(p_wlist_cont, dyn_online_movie_web_cont_proc);

    //bottom help bar
    ui_comm_help_create(&help_data, p_menu);
    ui_comm_help_move_pos(p_menu, UI_NETWORK_BOTTOM_HELP_X, -50, UI_NETWORK_BOTTOM_HELP_W, 50, 42);

    ctrl_default_proc(p_wlist_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

    //ui_comm_cfmdlg_open(NULL, IDS_LOADING_WITH_WAIT, NULL, 0);

    g_pNmWeb->hMenu = p_menu;
    g_pNmWeb->hWebCont = p_wlist_cont;

	al_netmedia_register_msg();
    Website_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_SUBMENU_NM), NOTIFY_T_MSG, FALSE, MSG_OPEN_WEBSITE_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE check_network_conn_stats(void)
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32  
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
     && (eth_connt_stats.is_3g_conn == FALSE) && (eth_connt_stats.is_gprs_conn == FALSE))
  {
     ret = ERR_FAILURE;  
     ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
  }
#endif
  return ret;
}

static RET_CODE on_submenu_network_destroy(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
    OS_PRINTF("@@@on_submenu_network_destroy\n");
    //ui_comm_dlg_close();
	Website_CloseStateTree();
    al_netmedia_unregister_msg();
    ui_nm_dyn_website_pic_deinit();
    ui_release_nm_dyn_website_data();
    ui_video_c_destroy();

    return ERR_NOFEATURE;
}

static RET_CODE ui_nm_dyn_website_pic_play_start()
{
    control_t *p_item = NULL, *p_pic = NULL;
    rect_t rect;

    OS_PRINTF("@@@ui_nm_dyn_website_pic_play_start cur Idx=%d,\n", g_pNmWeb->cur_pic);
    if (g_pNmWeb->website_list)
    {
        while ((g_pNmWeb->cur_pic < g_pNmWeb->total_web) && (g_pNmWeb->cur_pic < (DYN_WEBSITE_WEB_PAGE_SIZE * g_pNmWeb->cur_page)))
        {
            if (g_pNmWeb->website_list[g_pNmWeb->cur_pic].logo_url 
                && strlen(g_pNmWeb->website_list[g_pNmWeb->cur_pic].logo_url) > 0)
            {
                p_item = ctrl_get_child_by_id(g_pNmWeb->hWebCont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + g_pNmWeb->cur_pic - (DYN_WEBSITE_WEB_PAGE_SIZE * (g_pNmWeb->cur_page - 1)));
                p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);

                ctrl_get_frame(p_pic, &rect);
                ctrl_client2screen(p_pic, &rect);

                ui_pic_play_by_url(g_pNmWeb->website_list[g_pNmWeb->cur_pic].logo_url, &rect, 0);
                return SUCCESS;
            }
            g_pNmWeb->cur_pic++;
        }
    }

    return SUCCESS;
}

static RET_CODE ui_nm_website_update_page(void)
{
	control_t *p_item = NULL;
    control_t *p_pic = NULL;
    control_t *p_name = NULL;   
    u8 i;
	u8 max_web;
	rect_t rect;

    if (g_pNmWeb->cur_page < g_pNmWeb->total_page)
	{
		max_web = DYN_WEBSITE_WEB_PAGE_SIZE;
	}
	else
	{
		max_web = g_pNmWeb->total_web - (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
	}

    OS_PRINTF("@@@ui_nm_website_update_page  max_web = [%d] \n", max_web);
	
    for (i = 0; i < max_web; i++)
    {
        p_item = ctrl_get_child_by_id(g_pNmWeb->hWebCont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);
        p_name = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_NAME);
		
  	 	ctrl_set_sts(p_item, OBJ_STS_SHOW);
        bmap_set_content_by_id(p_pic, IM_APP_LOADING);
        text_set_content_by_extstr(p_name, g_pNmWeb->website_list[ ( g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE + i].title);
        ctrl_paint_ctrl(p_item, TRUE);
    }
	
    for (i = max_web; i < DYN_WEBSITE_WEB_PAGE_SIZE; i++)
    {
        p_item = ctrl_get_child_by_id(g_pNmWeb->hWebCont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);

        ctrl_set_sts(p_item, OBJ_STS_HIDE);

        //clear pic layer
        pic_stop();
        ctrl_get_frame(p_pic, &rect);
        ctrl_client2screen(p_pic, &rect);
        //OS_PRINTF("@@@left=%d, top=%d, right=%d, bottom=%d\n", rect.left, rect.top, rect.right, rect.bottom);
        ui_pic_clear_rect(&rect, 0);
    }

    //g_pNmWeb->cur_web = g_pNmWeb->cur_page * DYN_WEBSITE_WEB_PAGE_SIZE;
    //ui_nm_dyn_website_set_focus_pos(g_pNmWeb->hWebCont, g_pNmWeb->cur_web);

	g_pNmWeb->cur_pic = (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
    //ui_nm_dyn_website_pic_play_start();

    return SUCCESS;
}

void ui_nm_website_open_cfm_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

void ui_nm_website_open_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_DLG_REQ, str_id, 0);
    }
}

static RET_CODE ui_nm_website_on_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_SUBMENU_NM,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_website_on_open_dlg\n");

    if (Website_IsStateActive(SID_WEBSITE_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);
    }

    return SUCCESS;
}

static RET_CODE ui_nm_website_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_SUBMENU_NM,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_nm_website_on_open_cfm_dlg\n");

    if (Website_IsStateActive(SID_WEBSITE_ACTIVE))
    {
      dlg_data.content = para1;

      ui_comm_dlg_open(&dlg_data);

      p_root = fw_find_root_by_id(ROOT_ID_SUBMENU_NM);
      if(p_root)
      {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
      }
    }

    return SUCCESS;
}

static void SenWebsite_Inactive(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static STATEID StaWebsite_inactive_on_open_website_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
	OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    return SID_WEBSITE_INIT;
}

static void SexWebsite_Inactive(void)
{
	OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenWebsite_Active(void)
{
	OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);		
}

static STATEID StaWebsite_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Website_IsStateActive(SID_WEBSITE_DEINIT))
    {
        al_netmedia_dp_deinit();
    }

    return SID_NULL;	
}

static STATEID StaWebsite_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    if (!Website_IsStateActive(SID_WEBSITE_DEINIT))
    {
        return SID_WEBSITE_DEINIT;
    }
    else
    {
        return SID_NULL;
    }
}

static void SexWebsite_Active(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
}

static void SenWebsite_Init(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
	OS_PRINTF("@@@dp_init\n");
	
    ui_nm_website_open_dlg(ROOT_ID_SUBMENU_NM, IDS_LOADING_WITH_WAIT);
    al_netmedia_dp_init();
}

static STATEID StaWebsite_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
	al_netmedia_get_website_info();
    return SID_NULL;
}

static STATEID StaWebsite_init_on_newweblist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_netmedia_website_info_t *p_data = (al_netmedia_website_info_t *)para2;
    al_netmedia_website_item_t *p_website_info_list;
    u16 i;
    u16 str_len;
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    if (p_data->web_count > 0 && p_data->website_list != NULL)
    {
        OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
        p_website_info_list = (al_netmedia_website_item_t *)mtos_malloc(p_data->web_count * sizeof(al_netmedia_website_item_t));
        MT_ASSERT(p_website_info_list != NULL);
        memset((void *)p_website_info_list, 0, p_data->web_count * sizeof(al_netmedia_website_item_t));

        for (i = 0; i < p_data->web_count; i++)
        {
            if (p_data->website_list[i].title != NULL)
            {
                str_len = uni_strlen(p_data->website_list[i].title);
                p_website_info_list[i].title = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_website_info_list[i].title != NULL);
                uni_strcpy(p_website_info_list[i].title, p_data->website_list[i].title);
                OS_PRINTF("@@@%s,%d p_website_info_list[%d].title==%s\n",__FUNCTION__,__LINE__,i,p_website_info_list[i].title);
            }

            if (p_data->website_list[i].logo_url != NULL)
            {
                str_len = strlen(p_data->website_list[i].logo_url);
                p_website_info_list[i].logo_url = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_website_info_list[i].logo_url != NULL);
                strcpy(p_website_info_list[i].logo_url, p_data->website_list[i].logo_url);
                OS_PRINTF("@@@%s,%d p_website_info_list[%d].logo_url==%s\n",__FUNCTION__,__LINE__,i,p_website_info_list[i].logo_url);
            }

            p_website_info_list[i].ui_style = p_data->website_list[i].ui_style;
            p_website_info_list[i].ui_attr = p_data->website_list[i].ui_attr;
            p_website_info_list[i].dp_type = p_data->website_list[i].dp_type;
  
        }
      
        g_pNmWeb->website_list = p_website_info_list;

        g_pNmWeb->total_web = p_data->web_count;
		
		if((g_pNmWeb->total_web % DYN_WEBSITE_WEB_PAGE_SIZE) == 0)
		{
			g_pNmWeb->total_page = p_data->web_count / DYN_WEBSITE_WEB_PAGE_SIZE;	
		}
		else
		{
			g_pNmWeb->total_page = (p_data->web_count / DYN_WEBSITE_WEB_PAGE_SIZE + 1);			
		}
		g_pNmWeb->cur_page = 1;
		
        //update icon and str
        ui_comm_dlg_close();
        //ui_nm_dyn_website_update_web_cont(g_pNmWeb->hWebCont, 0,  DYN_WEBSITE_WEB_PAGE_SIZE, 0);
        g_pNmWeb->cur_pic = 0;
        //ui_nm_dyn_website_pic_play_start();
    }
    else
    {
		return SID_WEBSITE_INIT_FAILED;
    }

    return SID_WEBSITE_WEBSITE;
}

static void SexWebsite_Init(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_comm_dlg_close();
}

static void SenWebsite_Website(void)
{
	OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

	ui_nm_website_update_page();
	ui_nm_dyn_website_set_focus_pos(g_pNmWeb->hWebCont, g_pNmWeb->cur_web);
	ui_nm_dyn_website_pic_play_start();
}

static STATEID StaWebsite_web_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_item = NULL, *p_pic = NULL;
	u16 index;
	
	index = g_pNmWeb->cur_pic % DYN_WEBSITE_WEB_PAGE_SIZE;

    OS_PRINTF("@@@StaWebsite_web_pic_draw_end\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_SUBMENU_NM)
    {
        p_item = ctrl_get_child_by_id(g_pNmWeb->hWebCont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + index);
        p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);
        bmap_set_content_by_id(p_pic, RSC_INVALID_ID);

        ctrl_paint_ctrl(p_pic, TRUE);

        pic_stop();
        g_pNmWeb->cur_pic++;
        ui_nm_dyn_website_pic_play_start();
    }

    return SID_NULL;
}

static STATEID StaWebsite_web_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaWebsite_web_pic_draw_fail\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_SUBMENU_NM)
    {
        pic_stop();
        g_pNmWeb->cur_pic++;
        ui_nm_dyn_website_pic_play_start();
    }
    
    return SID_NULL;
}

static BOOL StcWebsite_is_on_website_list(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_DYN_WEBSITE_WEB_CONT) ? TRUE : FALSE;
}

static STATEID StaWebsite_web_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
	OS_PRINTF("@@@StaWebsite_web_on_change_page\n");
	
    if (para1 == V_KEY_PAGE_UP)
    {
        if (g_pNmWeb->cur_page > 1)
        {
        	g_pNmWeb->cur_page--;
            g_pNmWeb->cur_web = (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
			//update page
			return SID_WEBSITE_WEBSITE;
        }
    }
    else if (para1 == V_KEY_PAGE_DOWN)
    {
        if (g_pNmWeb->cur_page < g_pNmWeb->total_page)
        {
            g_pNmWeb->cur_page++;
			g_pNmWeb->cur_web = (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
            //update page
			return SID_WEBSITE_WEBSITE;
        }
    }

	ui_nm_dyn_website_set_focus_pos(g_pNmWeb->hWebCont, g_pNmWeb->cur_web);
    return SID_NULL;
}

static STATEID StaWebsite_web_on_weblist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    s16 cur_pos;
    s16 new_pos;
	s16 max_pos;

    OS_PRINTF("@@@StaWebsite_web_on_weblist_focus_key_hldr  cur_page = [%d], total_page = [%d]\n", g_pNmWeb->cur_page, g_pNmWeb->total_page);
	
    cur_pos = (g_pNmWeb->cur_web % DYN_WEBSITE_WEB_PAGE_SIZE);
	
	if ((g_pNmWeb->cur_page < g_pNmWeb->total_page) || (( g_pNmWeb->total_web % DYN_WEBSITE_WEB_PAGE_SIZE) == 0))
	{
		max_pos = DYN_WEBSITE_WEB_PAGE_SIZE;
	}
	else
	{
		max_pos = g_pNmWeb->total_web % DYN_WEBSITE_WEB_PAGE_SIZE;
    }	

    switch(para1)
    {
        case V_KEY_RIGHT: 
            if (cur_pos == (max_pos - 1))
            {
            	if (g_pNmWeb->cur_page < g_pNmWeb->total_page)
			    {
			    	g_pNmWeb->cur_web++;
					g_pNmWeb->cur_page++;
					//update page
					return SID_WEBSITE_WEBSITE;
			    }
                break;
            }
            else
            {
                g_pNmWeb->cur_web++;
            }
            break;
    
        case V_KEY_LEFT:
            if (cur_pos == 0)
            {
            	if (g_pNmWeb->cur_page > 1)
			    {
			    	g_pNmWeb->cur_web--;
					g_pNmWeb->cur_page--;
					//update page
					return SID_WEBSITE_WEBSITE;
			    }
                break;
            }
            else
            {
                g_pNmWeb->cur_web--;
            }
            break;
    
        case V_KEY_DOWN:
            new_pos = cur_pos + DYN_WEBSITE_WEB_COL;
            if(new_pos >= max_pos)
            {
            	if (g_pNmWeb->cur_page < g_pNmWeb->total_page)
			    {
			    	g_pNmWeb->cur_web = g_pNmWeb->cur_page * DYN_WEBSITE_WEB_PAGE_SIZE;
					g_pNmWeb->cur_page++;
					//update page
					return SID_WEBSITE_WEBSITE;
			    }
                break;            
            }
            else
            {
                g_pNmWeb->cur_web += DYN_WEBSITE_WEB_COL;
            }
            break;
    
        case V_KEY_UP:
            new_pos = cur_pos - DYN_WEBSITE_WEB_COL;
            if (new_pos < 0)
            {
            	if (g_pNmWeb->cur_page > 1)
			    {
			    	g_pNmWeb->cur_page--;
			    	g_pNmWeb->cur_web = g_pNmWeb->cur_page * DYN_WEBSITE_WEB_PAGE_SIZE - 1;
					//update page
					return SID_WEBSITE_WEBSITE;
			    }
                break;
            }
            else
            {
                g_pNmWeb->cur_web -= DYN_WEBSITE_WEB_COL;
            }
            break;
    
        default:
          break;
    }
  
    ui_nm_dyn_website_set_focus_pos(g_pNmWeb->hWebCont, g_pNmWeb->cur_web);
    return SID_NULL;
}

static STATEID StaWebsite_web_on_weblist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    if(ERR_FAILURE == check_network_conn_stats())
	{
	    return SID_NULL;
	}

    if ((g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style > NETMEDIA_UI_STYLE_INVALID)
                                                 &&(g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style < NETMEDIA_UI_STYLE_MAX))
    {
        ui_nm_dyn_website_pic_deinit();
    }
    else
    {
        return SID_NULL;
    }
    
    OS_PRINTF("@@@g_pNmWeb->website_list[g_pNmWeb->cur_web].dp_type = [%d]\n", g_pNmWeb->website_list[g_pNmWeb->cur_web].dp_type);
    OS_PRINTF("@@@g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style = [%d]\n", g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style);
    if (g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style == NETMEDIA_UI_STYLE_SIMPLE)
    {     
        ui_open_nm_simple(g_pNmWeb->website_list[g_pNmWeb->cur_web].dp_type, g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_attr);
    }  
    else if (g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_style == NETMEDIA_UI_STYLE_CLASSIC)
    {     
        ui_open_nm_classic(g_pNmWeb->website_list[g_pNmWeb->cur_web].dp_type, g_pNmWeb->website_list[g_pNmWeb->cur_web].ui_attr);
    } 
    
    return SID_NULL;
}

static STATEID StaWebsite_web_on_weblist_update(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_item = NULL, *p_pic = NULL;
    u16 i;
	u16 max_web;

	max_web = g_pNmWeb->total_web - (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
    for (i = 0; ((i < max_web) && (i < DYN_WEBSITE_WEB_PAGE_SIZE) ); i++)
    {
        p_item = ctrl_get_child_by_id(g_pNmWeb->hWebCont, IDC_DYN_WEBSITE_WEB_ITEM_CONT_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_DYN_WEBSITE_WEB_PIC);
        bmap_set_content_by_id(p_pic, IM_APP_LOADING);
    }
    ctrl_paint_ctrl(g_pNmWeb->hWebCont, TRUE);
  
    ui_nm_dyn_website_pic_init();

    g_pNmWeb->cur_pic = (g_pNmWeb->cur_page - 1) * DYN_WEBSITE_WEB_PAGE_SIZE;
    ui_nm_dyn_website_pic_play_start();
    
    return SUCCESS;
}

static void SexWebsite_Website(void)
{
    //rect_t rect;
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);

    pic_stop();
	/*
    ctrl_get_frame(g_pNmSimple->hVdoList, &rect);
    ctrl_client2screen(g_pNmSimple->hVdoList, &rect);
    rect.left += vdo_list_attr[VDOLIST_FIELD_ID_LOGO].left;
    rect.right = rect.left + vdo_list_attr[VDOLIST_FIELD_ID_LOGO].width;
    ui_pic_clear_rect(&rect, 0);
    */
}

static void SenWebsite_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
    ui_nm_website_open_cfm_dlg(ROOT_ID_SUBMENU_NM, IDS_DATA_ERROR);
}

static void SexWebsite_InitFailed(void)
{
    OS_PRINTF("@@@%s,%d\n",__FUNCTION__,__LINE__);
	ui_comm_dlg_close();
}

static void SenWebsite_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    al_netmedia_dp_deinit();
}

static STATEID StaWebsite_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_SUBMENU_NM);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    return SID_NULL;
}  

static void SexWebsite_Deinit(void)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
}

//dispatch website msg
static RET_CODE ui_nm_website_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = Website_DispatchMsg(p_ctrl, msg, para1, para2);
    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@nm_website unaccepted msg, id=0x%04x\n", msg);
    }

    return ret;
}

//dispatch nm msg
static RET_CODE ui_dispatch_netmedia_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    switch (msg)
    {
        case MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED:
        case MSG_NETMEDIA_GET_PAGE_VDO_FAILED:
        case MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED:
        case MSG_NETMEDIA_GET_SEARCH_VDO_FAILED:
            if (al_netmedia_vdo_idntfy_cmp(para1) == 0)
            {
                p_root = fw_find_root_by_id(ROOT_ID_NETMEDIA);
            }
            break;

        case MSG_NETMEDIA_NEW_PROGRAM_INFO_ARRIVED:
        case MSG_NETMEDIA_GET_PROGRAM_INFO_FAILED:
        case MSG_NETMEDIA_NEW_COLLECTION_LIST_ARRIVED:
        case MSG_NETMEDIA_GET_COLLECTION_LIST_FAILED:
        case MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED:
        case MSG_NETMEDIA_GET_PLAY_URLS_FAILED:
        case MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED:
        case MSG_NETMEDIA_GET_CLASS_LIST_FAILED:
        case MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED:
        case MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED:
        case MSG_NETMEDIA_WEBSITE_INIT_SUCCESS:
        case MSG_NETMEDIA_WEBSITE_INIT_FAILED:
        case MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS:
        case MSG_NETMEDIA_WEBSITE_DEINIT_FAILED:
            p_root = fw_find_root_by_id(ROOT_ID_NETMEDIA);
            break;

       case MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED:
       case MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED:
	   case MSG_NETMEDIA_INIT_SUCCESS:
	   case MSG_NETMEDIA_INIT_FAILED:
	   case MSG_NETMEDIA_DEINIT_SUCCESS:
	   case MSG_NETMEDIA_DEINIT_FAILED:
             ui_nm_website_on_state_process(p_ctrl, msg, para1, para2);
             break;
        default:
            break;
    }

    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, msg, para1, para2);
    }

    al_netmedia_free_msg_data(msg, para1, para2);
    
    return SUCCESS;
}



BEGIN_MSGPROC(submenu_network_proc, ui_comm_root_proc)
  //dispatch nm msg
    ON_COMMAND(MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_PAGE_VDO_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_SEARCH_VDO_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_PLAY_URLS_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_CLASS_LIST_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_SUCCESS, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_INIT_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_WEBSITE_DEINIT_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED, ui_dispatch_netmedia_msg)

    ON_COMMAND(MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_INIT_SUCCESS, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_INIT_FAILED, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_DEINIT_SUCCESS, ui_dispatch_netmedia_msg)
    ON_COMMAND(MSG_NETMEDIA_DEINIT_FAILED, ui_dispatch_netmedia_msg)
    
    //draw msg
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, ui_nm_website_on_state_process)
   
    //common msg
    ON_COMMAND(MSG_OPEN_WEBSITE_REQ, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_EXIT_ALL, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_DESTROY, on_submenu_network_destroy)
    ON_COMMAND(MSG_UPDATE, ui_nm_website_on_state_process)

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_nm_website_on_open_cfm_dlg)
    ON_COMMAND(MSG_OPEN_DLG_REQ, ui_nm_website_on_open_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_nm_website_on_state_process)  
  
END_MSGPROC(submenu_network_proc, ui_comm_root_proc)

BEGIN_KEYMAP(dyn_online_movie_web_cont_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(dyn_online_movie_web_cont_keymap, NULL)

BEGIN_MSGPROC(dyn_online_movie_web_cont_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_nm_website_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_nm_website_on_state_process)
END_MSGPROC(dyn_online_movie_web_cont_proc, cont_class_proc)

