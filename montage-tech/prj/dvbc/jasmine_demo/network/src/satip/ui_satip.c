/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_video.h"
#include "ui_satip.h"
#include "ui_dlna.h"
#include "ui_dlna_api.h"

//#include "mini_httpd.h"
#include "network/mini_httpd/mini_httpd.h"
#include "satip_play.h"
#include "cdlna.h"


#define UNS_LOW_LIMIT (11700)

enum control_id
{
  IDC_INVALID = 0,
  IDC_SATIP,
  IDC_SATIP_MESSAGE,
  IDC_SATIP_HELP_CONT
};


typedef struct
{
    control_t *p_root;
    control_t *p_satip;
    satip_sts satip_status;
    u16 client_pg_id;
    BOOL active_port;  //if false use prot1, else use port2
    BOOL is_push; //only push one time at satip menu
    BOOL is_disable; // disable left right key
}satip_t;

static satip_t g_satip = {NULL,NULL,SATIP_OFF, 0xFFFF, FALSE, FALSE, FALSE};

static u16 satip_cont_keymap(u16 key);
static RET_CODE satip_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE satip_change_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

void ui_set_satip_port_flag(BOOL flag)
{
    g_satip.active_port = flag;
}

BOOL ui_get_satip_port_flag()
{
    return g_satip.active_port;
}
void ui_set_satip_status(u8 status)
{
    g_satip.satip_status = (satip_sts)status;
}

u8 ui_get_satip_status()
{
    return g_satip.satip_status;
}

u16 ui_get_satip_client_pg_id()
{
    return g_satip.client_pg_id;
}

void ui_set_satip_client_pg_id(u16 pg_id)
{
    g_satip.client_pg_id = pg_id;
}

static control_t *create_satip_menu()
{
    control_t *p_cont = NULL;
    control_t *p_satip = NULL;
    control_t *p_name_help = NULL;
    control_t *p_msg = NULL;
    u16 opt_data[SAT_IP_ITEM] = {IDS_OFF, IDS_ON};
    comm_help_data_t satip_help_data = 
    {
        3, 3,
        { IDS_MOVE, 
        IDS_OK,
        IDS_EXIT},
        { IM_HELP_ARROW,
        IM_HELP_OK,
        IM_HELP_MENU}
    };
    u8 msg[312] = {"If open SATIP switch, will close DLNA, and open DLNA switch will close SATIP. If SATIP is on, STB will preferentially push favorite programs to SATIP clients. If there is no favorite programs, STB will push all programs to SATIP clients, and the maximum number of programs which pushed is 128!"};
    u8 i;

    /*create the main window*/
    p_cont = ui_comm_root_create_netmenu(ROOT_ID_SATIP, 0 , IM_INDEX_SATIP_BANNER, IDS_SATIP);
    ctrl_set_keymap(p_cont, satip_cont_keymap);
    ctrl_set_proc(p_cont, satip_select_proc);
    g_satip.p_root = p_cont;
    
    /*create name*/  
    p_satip = ui_comm_select_create(p_cont, (u8)(IDC_SATIP),
                                SATIP_ITEM_X, SATIP_ITEM_Y,
                                SATIP_ITEM_LW, SATIP_ITEM_RW);
    ui_comm_select_set_static_txt(p_satip, IDS_SATIP);
    ui_comm_select_set_param(p_satip, TRUE,
                           CBOX_WORKMODE_STATIC, 2,
                           CBOX_ITEM_STRTYPE_STRID, NULL);
    for (i = 0; i < SAT_IP_ITEM; i++)
    {
        ui_comm_select_set_content(p_satip, i, opt_data[i]);
    }
    ui_comm_select_set_focus(p_satip, ui_get_satip_status());
    ui_comm_ctrl_set_proc(p_satip, satip_change_proc);
    g_satip.p_satip = p_satip;

    //satip message
    p_msg = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_SATIP_MESSAGE), 
        SATIP_MSG_X, SATIP_MSG_Y, SATIP_MSG_W, SATIP_MSG_H, p_cont, 0);
    
    ctrl_set_rstyle(p_msg, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_msg, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_msg, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_msg, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_msg, msg);
    
    /*help bar */
    p_name_help = ctrl_create_ctrl(CTRL_CONT, IDC_SATIP_HELP_CONT,
                                  250, 520,
                                  900, 80,
                                  p_cont, 0);
    ctrl_set_rstyle(p_name_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ui_comm_help_create(&satip_help_data, p_name_help);

    /*set the name focus*/
    ctrl_default_proc(p_satip, MSG_GETFOCUS, 0, 0); /* focus on satip */
    return p_cont;
}


/*the exit function*/
static RET_CODE on_satip_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        OS_PRINTF("##%s, busying##\n", __FUNCTION__);
        return SUCCESS;
    }
    OS_PRINTF("##%s##\n", __FUNCTION__);
    manage_close_menu(ROOT_ID_SATIP, 0, 0);
    return SUCCESS;
}

static RET_CODE on_satip_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("##%s##\n", __FUNCTION__);
    g_satip.is_push = FALSE;
    g_satip.is_disable = FALSE;
    return ERR_NOFEATURE;
}


static RET_CODE on_satip_switch_on_off_get_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####%s####\n", __FUNCTION__);

    if(msg == MSG_ON_OFF_SATIP_SUCCESS)
    {
        if(ui_get_satip_status() == SATIP_OPENING)
        {
            OS_PRINTF("####open satip success####\n");
            ui_set_satip_status(SATIP_ON);
        }
        else if(ui_get_satip_status() == SATIP_CLOSING)
        {
            OS_PRINTF("####close satip success####\n");
            ui_set_satip_status(SATIP_OFF);
        }
        ui_comm_dlg_close();
    }
    else if(msg == MSG_ON_OFF_SATIP_FAILED)
    {
        if(ui_get_satip_status() == SATIP_OPENING)
        {
            OS_PRINTF("####open satip failed####\n");
            ui_set_satip_status(SATIP_OFF);
            ui_comm_select_set_focus(g_satip.p_satip, 0);
            ui_comm_cfmdlg_open(NULL, IDS_OPEN_SATIP_FAILED, NULL, 1000);
            
        }
        else if(ui_get_satip_status() == SATIP_CLOSING)
        {
            OS_PRINTF("####close satip failed####\n");
            ui_set_satip_status(SATIP_ON);
            ui_comm_select_set_focus(g_satip.p_satip, 1);
            ui_comm_cfmdlg_open(NULL, IDS_CLOSE_SATIP_FAILED, NULL, 1000);
        }
        ui_comm_dlg_close();
       
    }
    g_satip.is_disable = FALSE;
    ctrl_paint_ctrl(g_satip.p_satip, FALSE);

    return SUCCESS;
}

RET_CODE open_satip(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;

    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        OS_PRINTF("##%s, dlg is exist##\n", __FUNCTION__);
        ui_comm_dlg_close();
    }
    
    OS_PRINTF("############%s,###############\n", __FUNCTION__);
    p_menu = create_satip_menu();
    MT_ASSERT(p_menu != NULL);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

    return SUCCESS;
}

void satip_http_init(void)
{
    mini_httpd_t mini_httpd_cfg;

    memset(&mini_httpd_cfg, 0, sizeof(mini_httpd_cfg));
    mini_httpd_cfg.task_prio_satip[0] = SAT_IP_TASK_PRIORITY;
    mini_httpd_cfg.task_prio_satip[1] = SAT_IP_TASK2_PRIORITY;
    mini_httpd_cfg.task_prio_server = MINI_HTPPD_REQUEST_HANDLE_TASK_PRIORITY;
    mini_httpd_cfg.task_prio_handle = MINI_HTTPD_WEB_SERVER_TASK_PRIORITY;
    mini_httpd_cfg.stack_size_satip[0] = 8192;
    mini_httpd_cfg.stack_size_satip[1] = 8192;
    mini_httpd_cfg.stack_size_server = 8192;
    mini_httpd_cfg.stack_size_handle = 8192;
	#ifndef WIN32
    mini_httpd_config(&mini_httpd_cfg);
	#endif
}

//check if satip client is exist
void ui_check_satip_client_is_exist_while_change_pg(u16 curr_pg_id)
{
    dvbs_prog_node_t curr_pg = {0};
    dvbs_prog_node_t client_pg = {0};
    u16 client_pg_id;

    OS_PRINTF("##%s, line == %d##\n", __FUNCTION__, __LINE__);
    if(ui_get_satip_status() == SATIP_ON && ui_get_play_prg_type() == SAT_PRG_TYPE 
    && ui_is_playing() && CURN_MODE_TV == sys_status_get_curn_prog_mode())
    {
        client_pg_id = ui_get_satip_client_pg_id();
        OS_PRINTF("##%s, client_pg_id == %d##\n", __FUNCTION__, client_pg_id);
        if(client_pg_id != 0xFFFF)
        {
            OS_PRINTF("##%s, line == %d##\n", __FUNCTION__, __LINE__);
            db_dvbs_get_pg_by_id(curr_pg_id, &curr_pg);
            db_dvbs_get_pg_by_id(client_pg_id, &client_pg);
            if(client_pg.tp_id != curr_pg.tp_id)
            {
                OS_PRINTF("##%s, line == %d, set curr pg id is invilad##\n", __FUNCTION__, __LINE__);
                //ui_comm_cfmdlg_open(NULL, IDS_STB_CHANGE_PG_CHECK_CLINET, NULL, 2000);
                ui_set_satip_client_pg_id(0xFFFF);
            }
        }
        else
        {
            OS_PRINTF("##%s, invalid line == %d##\n", __FUNCTION__, __LINE__);
        }
    }
}

static void satip_change_prog_callback(program_param_t *p_program_param)
{
    control_t *p_root = NULL;
    
    OS_PRINTF("####%s, start####\n", __FUNCTION__);

    if(p_program_param == NULL)
    {
        OS_PRINTF("####%s, p_program_param is null####\n", __FUNCTION__);
        return ;
    }

    if(ui_get_closing_satip_at_dlna())
    {
        OS_PRINTF("####%s, dlna ui is closing satip####\n", __FUNCTION__);
        return ;
    }
    
    ui_set_satip_client_pg_id(p_program_param->program_id);
    p_root = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(ui_get_ttx_flag() == TRUE)
    {
        if(p_root)    
        {
            OS_PRINTF("####%s, ttx is start, will stop it firstly####\n", __FUNCTION__);
            fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_STOP_TTX, 0, 0);
        }
    }
    if(p_root)    
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_SATIP_CLIENT_REQ_CHANGE_PG, (u32)g_satip.client_pg_id, 0);
    }
    
    OS_PRINTF("####%s, freq=[%d],sym=[%d],polarity=[%d]####\n", __FUNCTION__, \
            p_program_param->freq,p_program_param->symb_rate,p_program_param->polarization);
    OS_PRINTF("####%s, end####\n", __FUNCTION__);
    
}


extern int start_mini_httpd(dmx_changeprogram_t p_changeprogram, u16 port_num);

static void ui_open_satip()
{
    int result = 0;
    comm_dlg_data_t dlg_data_on =
    {
        ROOT_ID_SATIP,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_OPENING_SATIP,
        0,
    };

    if(g_satip.is_push == FALSE)
    {
        ui_set_satip_pg_info();
        g_satip.is_push = TRUE;
    }
    OS_PRINTF("####%s####\n", __FUNCTION__);
    ui_comm_dlg_open(&dlg_data_on);
    satip_http_init();
    if(ui_get_satip_port_flag() == FALSE)
    {
        #ifndef WIN32
        result = start_mini_httpd(satip_change_prog_callback, HTTP_SERVER_PORT1);
	#endif
        ui_set_satip_port_flag(TRUE);
    }
    else
    {
    	#ifndef WIN32
        result = start_mini_httpd(satip_change_prog_callback, HTTP_SERVER_PORT2);
	#endif
        ui_set_satip_port_flag(FALSE);
    }
    OS_PRINTF("####%s, open satip return[%d]####\n", __FUNCTION__, result);
    dlna_init();
	#ifndef WIN32
    cg_dlna_server_start();
	#endif
    ui_set_satip_status(SATIP_OPENING);
  
}

static void ui_close_satip()
{
    comm_dlg_data_t dlg_data_off =
    {
        ROOT_ID_SATIP,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_CLOSING_SATIP,
        0,
    };

    OS_PRINTF("####%s####\n", __FUNCTION__);
    ui_comm_dlg_open(&dlg_data_off);
    ui_cg_dlna_server_stop();
    ui_stop_mini_httpd();
    ui_set_satip_status(SATIP_CLOSING);
}

static RET_CODE on_satip_select_change(control_t *ctrl, u16 msg, u32 para1,
                                    u32 para2)
{
    u16 focus = (u16)(para2);

    // check for initialize
    if(!ctrl_is_onfocus(ctrl))
    {
        return SUCCESS;
    }
  
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        OS_PRINTF("##%s, busying##\n", __FUNCTION__);
        return SUCCESS;
    }
    
    OS_PRINTF("##%s, focus == %d##\n", __FUNCTION__, focus);
        
    if(focus == 1)
    {
        ui_open_satip();
    }
    else
    {
        ui_close_satip();
    }

    g_satip.is_disable = TRUE;
    OS_PRINTF("####%s, line[%d]####\n", __FUNCTION__, __LINE__);
    
    return SUCCESS;
}

static RET_CODE on_satip_select_pre_change(control_t *ctrl, u16 msg, u32 para1,
                                    u32 para2)
{
    if(g_satip.is_disable == FALSE)
    {
        OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
        return ERR_NOFEATURE;
    }
    else
    {
        OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }
}

void ui_force_close_satip_switch()
{
    if(ui_get_satip_status() == SATIP_ON)
    {
        manage_close_menu(ROOT_ID_SATIP, 0, 0);  
        ui_comm_cfmdlg_open(NULL, IDS_CLOSING_SATIP, NULL, 2000);
        ui_cg_dlna_server_stop();
        ui_stop_mini_httpd();
        ui_set_satip_status(SATIP_OFF);
    }
}

u32 ui_dvbs_calc_mid_freq(dvbs_tp_node_t *p_tp, sat_node_t *p_sat)
{
    //u16 mid_freq = 0;
    //u8 polarity = 0;
    u8 k22 = 0;
    u32 temp = 0;

    MT_ASSERT(NULL != p_sat);
    MT_ASSERT(NULL != p_tp);

    //mid_freq = (u16)p_tp->freq;
    //polarity = (u8)p_tp->polarity;

    //OS_PRINTF("##%s, lnb_type == %d##\n", __FUNCTION__, p_sat->lnb_type);
    switch(p_sat->lnb_type)
    {
        case DVBS_LNB_STANDARD:
            if(C_BAND == dvbs_detect_lnb_freq((u32)(int)p_sat->lnb_low))
            {
                temp = (u32)(int)p_sat->lnb_low - p_tp->freq;
            }
            else
            {
                temp = p_tp->freq - (u32)(int)p_sat->lnb_low;
            }
            break;
        case DVBS_LNB_USER:
            if(0 == p_tp->polarity)
            {
                temp = (u32)(int)p_sat->lnb_low - p_tp->freq;
            }
            else if(1 == p_tp->polarity)
            {
                temp = (u32)(int)p_sat->lnb_high - p_tp->freq;
            }
            else
            {
                MT_ASSERT(0);
            }
            break;
         case DVBS_LNB_UNIVERSAL:
            k22 = (p_tp->freq > UNS_LOW_LIMIT) ? 1 : 0;
            p_tp->is_on22k = k22;

            if(0 == k22)
            {
                temp = p_tp->freq - (u32)(int)p_sat->lnb_low;
            }
            else if(1 == k22)
            {
                temp = p_tp->freq - (u32)(int)p_sat->lnb_high;
            }
            else
            {
                MT_ASSERT(0);
            }
            break;
        default:
            OS_PRINTF("Error: bad lnb type\n");
            MT_ASSERT(0);
            break;
            
    }
    
    return temp * KHZ;
    
}

 void  ui_set_satip_pg_info(void)
{
    u8 pg_view = 0;
    u32 pg_cnt = 0;
    u32 all_pg_cnt = 0;
    u32 i = 0;
    u16 pg_id =0;
    u16 fav_cnt = 0;
    dvbs_prog_node_t pg_node = {0,};
    dvbs_tp_node_t tp = {0};
    sat_node_t p_sat = {0};
    satip_pg_info_t *pg_info = NULL;

    pg_view = db_dvbs_create_view(DB_DVBS_FAV_ALL_TV, 0, NULL);
    fav_cnt = db_dvbs_get_count(pg_view);

    if(fav_cnt)
    {
        pg_cnt = MIN(fav_cnt, MAX_SATIP_PG_CNT);
    }
    else
    {
        pg_view = db_dvbs_create_view(DB_DVBS_ALL_TV, 0, NULL);
        all_pg_cnt = db_dvbs_get_count(pg_view);
        pg_cnt = MIN(all_pg_cnt, MAX_SATIP_PG_CNT);
    }
    
    pg_info = (satip_pg_info_t *)mtos_malloc(pg_cnt * sizeof(satip_pg_info_t));
    memset(pg_info, 0, pg_cnt * sizeof(satip_pg_info_t));
    OS_PRINTF("##will push all group##\n", __FUNCTION__);
    for(i = 0; i < pg_cnt; i++)
    {
        pg_id = db_dvbs_get_id_by_view_pos(pg_view, (u16)i);
        db_dvbs_get_pg_by_id(pg_id, &pg_node);
        db_dvbs_get_tp_by_id((u16)pg_node.tp_id, &tp);
        //db_dvbs_get_sat_by_id((u16)tp.sat_id, &p_sat);
        memcpy(pg_info[i].pg_name,pg_node.name,sizeof(pg_info[i].pg_name));
        //pg_info[i].freq = ui_dvbs_calc_mid_freq(&tp,&p_sat);
        //pg_info[i].symb_rate = tp.sym;
        //pg_info[i].onoff_22k = tp.is_on22k;
        //pg_info[i].polar = tp.polarity;
        pg_info[i].v_pid =(u16)(int) pg_node.video_pid;
        pg_info[i].a_pid = (u16)(int)pg_node.audio[0].p_id;
        pg_info[i].pcr_pid = (u16)(int)pg_node.pcr_pid;
        pg_info[i].pmt_pid = (u16)(int)pg_node.pmt_pid;
        pg_info[i].pg_id = (u16)(int)pg_node.id;
        pg_info[i].type = TV_TYPE;
        //pg_info[i].is_des = pg_node.is_scrambled;
        
        //lint -e701
        if(p_sat.tuner_index == 1)
        {
            pg_info[i].is_des = (pg_node.is_scrambled << 4) | DMX_INPUT_EXTERN0;
        }
        else
        {
          pg_info[i].is_des = (pg_node.is_scrambled << 4) | DMX_INPUT_EXTERN_CI;
        }
        //lint +e701

    }
            
#ifndef WIN32    
    put_satip_pg_list(pg_info, pg_cnt);
#endif
    mtos_free(pg_info);
    
}


BEGIN_KEYMAP(satip_cont_keymap, NULL)
    ON_EVENT(V_KEY_MENU,MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL,MSG_CANCEL)
END_KEYMAP(satip_cont_keymap, NULL)

BEGIN_MSGPROC(satip_select_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_CANCEL,on_satip_exit)
    ON_COMMAND(MSG_EXIT,on_satip_exit)
    ON_COMMAND(MSG_DESTROY,on_satip_destroy)
    ON_COMMAND(MSG_ON_OFF_SATIP_SUCCESS,on_satip_switch_on_off_get_msg)
    ON_COMMAND(MSG_ON_OFF_SATIP_FAILED,on_satip_switch_on_off_get_msg)
END_MSGPROC(satip_select_proc, ui_comm_root_proc)


BEGIN_MSGPROC(satip_change_proc, cbox_class_proc)
    ON_COMMAND(MSG_CHANGED, on_satip_select_change)
    ON_COMMAND(MSG_INCREASE, on_satip_select_pre_change)
    ON_COMMAND(MSG_DECREASE, on_satip_select_pre_change)
END_MSGPROC(satip_change_proc, cbox_class_proc)       




