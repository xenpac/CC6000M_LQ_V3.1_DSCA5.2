/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_live_tv_api.h"
#include "ui_live_tv_desktop.h"
#include "SM_StateMachine.h"
#include "States_IPTV.h"

#if ENABLE_CUSTOMER_URL

static desk_sts_mch_t g_desktop = {0};
static void *file_live = NULL;
static vfs_file_info_t file_info;

/********************************************************************
                                    Destop Import livetv custom urls
                        
********************************************************************/

BOOL Iptv_Find_Cus_Url_Files(void)
{
    u16 g_ffilter_all[32] = {0};
    u32 g_partition_cnt = 0;
    file_list_t g_list = {0};
    flist_dir_t g_flist_dir = NULL;
    partition_t *p_partition = NULL;
    u32 count = 0;
    u16 file_name[MAX_FILE_PATH] = {0};

    str_asc2uni("|txt|TXT", g_ffilter_all);
    g_partition_cnt = 0;
    p_partition = NULL;
    g_partition_cnt = file_list_get_partition(&p_partition);  
    if(g_partition_cnt > 0)
    {
        g_flist_dir = file_list_enter_dir(g_ffilter_all, MAX_FILE_COUNT, p_partition[sys_status_get_usb_work_partition()].letter);
        if (g_flist_dir != NULL)
        {
            file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
            count = (u16)g_list.file_count;
            file_name[0] = p_partition[sys_status_get_usb_work_partition()].letter[0];
            str_asc2uni(":\\custom_url.txt", file_name + 1);

            file_live = vfs_open( file_name, VFS_READ);
            if (file_live == NULL)
            {
                return FALSE; 
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


void Iptv_Start_Load_Cus_Urls(void)
{
    u8 *p_buffer = NULL;
    u32 file_size = 0;

    if(file_live == NULL)
    {
        return ;
    }

    if(ui_get_usb_status() == FALSE)
    {
        if(file_live)
        {
            vfs_close(file_live);
            file_live = NULL;
        }
        return;
    }

    //----disable ir--------
    ui_evt_disable_ir();
    //--------------------
    vfs_get_file_info(file_live, &file_info);
    p_buffer = mtos_malloc(file_info.file_size + 2* KBYTES);
    MT_ASSERT(NULL != p_buffer);
    file_size = vfs_read(p_buffer, file_info.file_size, 1, file_live);
    MT_ASSERT(file_size == file_info.file_size);
    save_customer_iptv(p_buffer,file_info.file_size);
    
    //----enable ir------
    ui_evt_enable_ir();
    //-----------------

    mtos_free(p_buffer);
    vfs_close(file_live);
    file_live = NULL;
}

void Iptv_Cancel_Load_cus_Pg(void)
{
    if(file_live)
    {
        vfs_close(file_live);
        file_live = NULL;
    }
}

/********************************************************************
                                    Destop iptv state machine
                        
********************************************************************/
static void ui_desktop_dispatch_jump_status()
{
    Desktop_DispatchMsg(NULL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);   
}

static void ui_desktop_notify_msg_to_win(u8 root_id, u16 msg)
{
    control_t *p_root = NULL;
    p_root = fw_find_root_by_id(root_id);
    if(p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, msg, 0, 0);
    }
}

static void ui_iptv_entry_cus_grp_if_get_chan_failed(void)
{
    if(fw_get_focus_id() == ROOT_ID_SUBMENU_NETWORK)
    {
        ui_desktop_notify_msg_to_win(ROOT_ID_SUBMENU_NETWORK, MSG_OPEN_IPTV_CUS_MENU);
    }
}

BOOL ui_get_livetv_db_flag(void)
{
    return g_desktop.write_db_info_done;
}
void ui_set_livetv_db_flag(BOOL is_done)
{
    g_desktop.write_db_info_done = is_done;
}
u8 ui_get_init_livetv_status(void)
{
    return g_desktop.livetv_init;
}
void ui_set_init_livetv_status(u8 status)
{
    g_desktop.livetv_init = status;
}

static void SenDesktop_Disconnected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__); 
}
static void SexDesktop_Disconnected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__); 
}
static void SenDesktop_Connected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
}
static void SexDesktop_Connected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__); 
}

static STATEID StaDesktop_desktop_on_connect_network(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_CONNECTED;   
}
static STATEID StaDesktop_desktop_on_disconnect_network(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
    return SID_DESKTOP_DISCONNECTED;
}
static void SenDesktop_Initialize(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SexDesktop_Initialize(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SexDesktop_LivetvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvDpInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(ui_get_init_livetv_status() != LIVETV_INIT_SUCCESS && 
       ui_get_init_livetv_status() != LIVETV_INITING)
    {
        al_livetv_dp_init();
        ui_set_init_livetv_status(LIVETV_INITING);
    }
    else
    {
        ui_desktop_dispatch_jump_status();
    }
}
static STATEID StaDesktop_livetv_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_SUCCESS);
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static STATEID StaDesktop_livetv_init_on_init_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    ui_iptv_entry_cus_grp_if_get_chan_failed();
    return SID_DESKTOP_DISCONNECTED;
}
static STATEID StaDesktop_jump_to_desktop_livetv_categry_init(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static void SexDesktop_LivetvDpInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvCatgryInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(g_desktop.category_init == FALSE)
    {
        ui_live_tv_get_categorylist();
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            ui_live_tv_get_categorylist();
        }
        else
        {
            ui_desktop_dispatch_jump_status();
        }
    }
}
static STATEID StaDesktop_livetv_init_on_catgry_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    LIVE_TV_CATEGORY_LIST_T *p_group  = (LIVE_TV_CATEGORY_LIST_T *)para1;
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(p_group && p_group->status == TVLIVE_DATA_SUCCESS)
    {
        ui_live_tv_api_group_arrived(para1);
        g_desktop.category_init = TRUE;
        return SID_DESKTOP_LIVETV_CHAN_ZIP_INIT;
    }
    else
    {
        ui_set_init_livetv_status(LIVETV_INIT_FAILED);
        g_desktop.category_init = FALSE;
        g_desktop.up_livetv_timing = FALSE;
        ui_iptv_entry_cus_grp_if_get_chan_failed();
        return SID_DESKTOP_DISCONNECTED;
    }
}
static STATEID StaDesktop_jump_to_desktop_chan_zip_init(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CHAN_ZIP_INIT;
}
static void SexDesktop_LivetvCatgryInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvChanZipInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(g_desktop.chan_zip_init == FALSE)
    {
        ui_live_tv_api_all_chan_zip_load();
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            ui_live_tv_api_all_chan_zip_load();
        }
        else
        {
            ui_desktop_dispatch_jump_status();
        }
    }
}
static STATEID StaDesktop_livetv_init_on_chan_zip_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    g_desktop.p_chan_zip = (LIVE_TV_CHANNEL_ZIP_T *)para1;
    if(g_desktop.p_chan_zip && ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->status == TVLIVE_DATA_SUCCESS)
    {
        g_desktop.chan_zip_init = TRUE;
        return SID_DESKTOP_LIVETV_CHAN_LIST_INIT;
    }
    else
    {
        ui_set_init_livetv_status(LIVETV_INIT_FAILED);
        g_desktop.chan_zip_init = FALSE;
        g_desktop.up_livetv_timing = FALSE;
        ui_iptv_entry_cus_grp_if_get_chan_failed();
        return SID_DESKTOP_DISCONNECTED;
    }
}
static STATEID StaDesktop_jump_to_desktop_chan_list_init(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CHAN_LIST_INIT; 
}
static void SexDesktop_LivetvChanZipInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvChanListInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(ui_get_livetv_db_flag() == FALSE)
    {
    #ifndef WIN32
        LiveTV_getChannelList(get_live_tv_handle(),\
            ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_start_addr,\
                ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_length);
    #endif
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            #ifndef WIN32
            LiveTV_getChannelList(get_live_tv_handle(),\
                ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_start_addr,\
                    ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_length);
            #endif
        }
        else
        {
            ui_desktop_dispatch_jump_status();
        }
    }
}
static STATEID StaDesktop_livetv_init_on_livetv_all_chan_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    LIVE_TV_CHANNEL_LIST_T *p_all_chan = NULL;
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    p_all_chan = (LIVE_TV_CHANNEL_LIST_T *)para1;
    if(p_all_chan && p_all_chan->status == TVLIVE_DATA_SUCCESS)
    {
        OS_PRINTF("###p_all_chan->num == %d##\n",p_all_chan->num);
        ui_live_tv_api_all_chan_arrived(p_all_chan);
        ui_set_livetv_db_flag(TRUE);
        ui_comm_dlg_close();

        if(fw_get_focus_id() == ROOT_ID_SUBMENU_NETWORK)
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_SUBMENU_NETWORK, MSG_OPEN_IPTV_MENU);
        }
        else if((fw_get_focus_id() == ROOT_ID_BACKGROUND)||(fw_get_focus_id() == ROOT_ID_PROG_BAR))
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_BACKGROUND, MSG_OPEN_IPTV_MENU);
	 }
	 
        if(g_desktop.up_livetv_timing == FALSE)
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_LIVE_TV, MSG_GET_LIVE_TV_GET_ZIP);
        }
        else
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_LIVE_TV, MSG_LIVETV_UPDATE_TIMING);
        }
    }
    else
    {
        ui_set_livetv_db_flag(FALSE);
        g_desktop.up_livetv_timing = FALSE;
        ui_set_init_livetv_status(LIVETV_INIT_FAILED);
        ui_iptv_entry_cus_grp_if_get_chan_failed();
        return SID_DESKTOP_DISCONNECTED;
    }
    g_desktop.up_livetv_timing = FALSE;
    return SID_DESKTOP_IPTV_INIT;
}
static STATEID StaDesktop_jump_to_desktop_iptv_init(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IPTV_INIT; 
}
static void SexDesktop_LivetvChanListInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_IptvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    #ifndef WIN32
    {
        ui_desktop_dispatch_jump_status();
    }
    #endif
}
static void SexDesktop_IptvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
/*
static STATEID StaDesktop_iptv_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IDLE;
}
static STATEID StaDesktop_iptv_init_on_init_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IDLE;
}*/
static STATEID StaDesktop_jump_to_desktop_idle_status(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IDLE;
}
static void SenDesktop_Idle(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static STATEID StaDesktop_livetv_init_on_init_success_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_SUCCESS);
    if(ui_get_livetv_db_flag() == TRUE)
    {
        g_desktop.up_livetv_timing = TRUE;
    }
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static STATEID StaDesktop_livetv_init_on_init_failed_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    return SID_DESKTOP_IDLE;
}
/*
static STATEID StaDesktop_iptv_init_on_init_success_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IDLE;
}*/
static void SexDesktop_Idle(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}


RET_CODE Iptv_On_State_Process_Event(u32 event, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;
    u16 msg = ui_live_tv_evtmap(event);
    ret = Desktop_DispatchMsg(NULL, msg, para1, para2);    
    if (ret != SUCCESS)
    {
        OS_PRINTF("####desktop unaccepted msg, id=0x%04x####\n", msg);
    }
    return ret;
}

RET_CODE Iptv_On_State_Process_Msg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;
    ret = Desktop_DispatchMsg(ctrl, msg, para1, para2);    
    if (ret != SUCCESS)
    {
        OS_PRINTF("####desktop unaccepted msg, id=0x%04x####\n", msg);
    }
    return ret;
}

void Iptv_Set_Desktop()
{
    g_desktop.p_desktop = fw_find_root_by_id(ROOT_ID_BACKGROUND); 
}

//start paser iptv data
void Iptv_Open_State()
{
    Desktop_OpenStateTree();
}

extern void ui_set_enter_iptv_status(BOOL flag);

void Iptv_Get_Pg_Info(u8 type, BOOL is_from_desktop)
{
    u8 Ret_Sts = ui_get_init_livetv_status();

    ui_set_enter_iptv_status(is_from_desktop);
    
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    };
    
    dlg_data.content = IDS_DOWNLOAD_TO_WAIT;
    dlg_data.dlg_tmout = 2000;
  
    if(ui_get_livetv_db_flag() == FALSE)
    {
        if(LIVETV_INIT_FAILED == Ret_Sts || LIVETV_UNINIT == Ret_Sts)
        {
          Desktop_DispatchMsg(NULL, MSG_DESKTOP_NET_CONNTECED, 0, 0); 
        }
        ui_comm_dlg_open(&dlg_data);
    }
    else
    {
        manage_open_menu(ROOT_ID_LIVE_TV, type, 0);
    }
}
#endif
