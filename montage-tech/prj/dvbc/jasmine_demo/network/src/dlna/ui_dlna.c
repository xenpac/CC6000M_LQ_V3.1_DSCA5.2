/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_dlna_info.h"
#include "ui_mainmenu.h"
#include "ui_video.h"
#include "ui_dlna.h"
#include "ui_dlna_api.h"
#include "ui_menu_manager.h"
#include "ui_network_playbar.h"
#include "ui_desktop.h"
#include "ui_config.h"
#include "ui_satip.h"


enum dlna_info_id
{
  IDC_DLNA_TEXT_START_CONT = 1,
  IDC_DLNA_TEXT_START,
  IDC_DLNA_MBOX_ICON,
  IDC_DLNA_BG_1,
  IDC_DLNA_BG_2,
  IDC_DLNA_ICON_INFO,
  IDC_DLNA_ICON_INFO_2,
  IDC_DLNA_ICON_INFO_3,
  //page off text info
  IDC_DLNA_TEXT_INFO_1,
  IDC_DLNA_TEXT_INFO_2,
  IDC_DLNA_TEXT_INFO_3,
  IDC_DLNA_TEXT_INFO_4,
  IDC_DLNA_TEXT_INFO_5,
  IDC_DLNA_TEXT_INFO_6,
  IDC_DLNA_TEXT_INFO_7,
  IDC_DLNA_TEXT_INFO_8,
  IDC_DLNA_TEXT_INFO_9,
  IDC_DLNA_TEXT_INFO_10,

  //page on text info
  IDC_DLNA_TEXT_INFO_11,
  IDC_DLNA_TEXT_INFO_12,
  IDC_DLNA_TEXT_INFO_13,
  IDC_DLNA_TEXT_INFO_14,
  //icon
  IDC_DLNA_BG_V_LINE,
  IDC_DLNA_BG_P_ICON,
  IDC_DLNA_BG_P_ICON_2,
};

#define DLNA_Y_OFFSET 60

static item_xstyle_t dlna_text_item_rstyle = 
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static item_xstyle_t dlna_text_item_fstyle = 
{
  FSI_WHITE_22,
  FSI_WHITE_22,
  FSI_WHITE_22,
};

static item_xstyle_t dlna_text_item_small_fstyle = 
{
  FSI_WHITE_20,
  FSI_WHITE_20,
  FSI_WHITE_20,
};

static item_xstyle_t dlna_text_item_big_fstyle = 
{
  FSI_YELLOW_26,
  FSI_YELLOW_26,
  FSI_YELLOW_26,
};
static dlna_item_attr_t dlna_text_item_attr[] =
{
  { IDS_DLNA_INTRO_INFO1, TEXT_STRTYPE_STRID, STL_LEFT| STL_VCENTER,
    80, 20 + DLNA_Y_OFFSET, 600, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO2, TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    80, 160 + DLNA_Y_OFFSET, 600,40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO3,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    80, 300 + DLNA_Y_OFFSET, 600, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO4,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    80, 340 + DLNA_Y_OFFSET, 600, 40, &dlna_text_item_rstyle,  &dlna_text_item_small_fstyle},
  {IDS_DLNA_INTRO_INFO5,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    80, 380 + DLNA_Y_OFFSET, 600, 40, &dlna_text_item_rstyle,  &dlna_text_item_small_fstyle},
  {IDS_DLNA_INTRO_INFO6,  TEXT_STRTYPE_STRID, STL_LEFT| STL_VCENTER,
    720, 50 + DLNA_Y_OFFSET, 160, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO7,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    880, 50 + DLNA_Y_OFFSET, 80, 40, &dlna_text_item_rstyle,  &dlna_text_item_big_fstyle},
  {IDS_DLNA_INTRO_INFO8,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    960, 50 + DLNA_Y_OFFSET, 70, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO9,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    760, 90 + DLNA_Y_OFFSET, 80, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO10,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    840, 90 + DLNA_Y_OFFSET, 180, 40, &dlna_text_item_rstyle,  &dlna_text_item_big_fstyle},
};

static dlna_item_attr_t dlna_text2_item_attr[] =
{
  { IDS_DLNA_INTRO_INFO11, TEXT_STRTYPE_STRID, STL_LEFT| STL_VCENTER,
    80, 20 + DLNA_Y_OFFSET, 600, 40, &dlna_text_item_rstyle,  &dlna_text_item_small_fstyle},
  {IDS_DLNA_INTRO_INFO12, TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    80, 60 + DLNA_Y_OFFSET, 600,40, &dlna_text_item_rstyle,  &dlna_text_item_small_fstyle},
  {IDS_DLNA_INTRO_INFO13,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    710, 50 + DLNA_Y_OFFSET, 320, 40, &dlna_text_item_rstyle,  &dlna_text_item_fstyle},
  {IDS_DLNA_INTRO_INFO14,  TEXT_STRTYPE_STRID,STL_LEFT| STL_VCENTER,
    760, 90 + DLNA_Y_OFFSET, 260, 40, &dlna_text_item_rstyle,  &dlna_text_item_big_fstyle},
};

typedef struct
{
    control_t *p_root;
    control_t *p_bg1;
    control_t *p_bg2;
    dlna_sts g_dlna_sts;
    BOOL is_exit;
    BOOL is_closing_satip;
}ui_dlna_t;

static ui_dlna_t g_dlna = {NULL, NULL, NULL, DLNA_OFF, FALSE, FALSE};
u16 dlna_keymap(u16 key);
RET_CODE dlna_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

void ui_set_closing_satip_at_dlna(BOOL is_closing)
{
    g_dlna.is_closing_satip = is_closing;
}

BOOL ui_get_closing_satip_at_dlna(void)
{
    return g_dlna.is_closing_satip;
}

static void ui_set_dlna_exit_flag(BOOL is_exit)
{
    g_dlna.is_exit = is_exit;
}

static BOOL ui_get_dlna_exit_flag()
{
    return g_dlna.is_exit;
}

static void on_dlna_mode_change(u32 dlna_st)
{
    u32 mode;

    if(dlna_st == DLNA_ON)
    {
        mode = MODE_DLNA;
    }
    else
    {
        mode = MODE_NORMAL;
    }

    OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
    if(fw_find_root_by_id(ROOT_ID_MAINMENU))
    {
      fw_notify_root(fw_find_root_by_id(ROOT_ID_MAINMENU), NOTIFY_T_MSG, FALSE, MSG_UPDATE_MODE, mode, 0);
    }
}

static void on_dlna_frame_refresh(control_t *p_list, u16 msg, u32 para1, u32 para2)
{    
    u32 dlna_status = para2;
    OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
    
    switch(dlna_status)
    {
        case DLNA_OFF:
            ctrl_set_sts(g_dlna.p_bg1, OBJ_STS_SHOW);
            ctrl_set_sts(g_dlna.p_bg2, OBJ_STS_HIDE);
            break;
        case DLNA_ON:
            ctrl_set_sts(g_dlna.p_bg1, OBJ_STS_HIDE);
            ctrl_set_sts(g_dlna.p_bg2, OBJ_STS_SHOW);
            break;
        default:
            break;
    }
    ctrl_paint_ctrl(g_dlna.p_root, TRUE);
    return;
}



RET_CODE open_dlna_start(u32 para1, u32 para2)
{
  control_t *p_cont,*p_text, *p_mbox, *p_icon_1, *p_icon_2, *p_background1,*p_background2;
  control_t * p_vert_line,*p_dlna_icon;
  u16 i = 0;
  
  u16 icon_n[5] =
  {
    IM_LOGO_PPS,
    IM_LOGO_PPTV,
    IM_LOGO_SOHU,
    IM_LOGO_TX,
    IM_LOGO_YOUKU,
  };

  u16 x,y,w,h;
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        OS_PRINTF("##%s, dlg is exist##\n", __FUNCTION__);
        ui_comm_dlg_close();
    }
    
 p_cont = ui_comm_root_create_netmenu(ROOT_ID_DLNA_START, 0,
                                                 IM_INDEX_DLNA_BANNER, IDS_DLNA);
  
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont,dlna_keymap);
  ctrl_set_proc(p_cont, dlna_proc);
  g_dlna.p_root = p_cont;
  
  p_icon_1 = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_ICON_INFO,
                                 40, 20 + DLNA_Y_OFFSET,
                                 20, 40,
                                 p_cont, 0);
  bmap_set_align_type(p_icon_1, STL_LEFT| STL_VCENTER);
  bmap_set_content_by_id(p_icon_1, IM_DLNA_BG_ICON);
  
  //dlna off background
  p_background1 = ctrl_create_ctrl(CTRL_CONT, IDC_DLNA_BG_1,
                                 2, 0,
                                 1034, 480+DLNA_Y_OFFSET,
                                 p_cont, 0);
  ctrl_set_rstyle(p_background1, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);
  g_dlna.p_bg1 = p_background1;
  p_mbox= ctrl_create_ctrl(CTRL_MBOX, IDC_DLNA_MBOX_ICON,
                                 80, 70 + DLNA_Y_OFFSET,
                                 500, 80,
                                 p_background1, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, FALSE);
  mbox_set_count(p_mbox, 5, 5, 1);
  mbox_set_item_interval(p_mbox, 0, 40);
  mbox_set_icon_align_type(p_mbox, STL_TOP | STL_VCENTER);
  //mbox_set_item_status(p_mbox_icon, 2, MBOX_ITEM_DISABLED);
  for(i = 0; i < 5; i++)
  {
    mbox_set_content_by_icon(p_mbox, i, icon_n[i], icon_n[i]);
    ctrl_set_style(p_mbox, STL_EX_ALWAYS_HL);
  }

  p_icon_2 = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_ICON_INFO_2,
  	                                  80, 210 + DLNA_Y_OFFSET,
  	                                  500, 80,
  	                                  p_background1, 0);
  bmap_set_align_type(p_icon_2, STL_CENTER | STL_VCENTER);
  bmap_set_content_by_id(p_icon_2, IM_DLNA_BACK);
  
  for(i = 0; i < MAX_DLNA_TEXT_CTRL; i++)
{
  x = dlna_text_item_attr[i].left;
  y = dlna_text_item_attr[i].top;
  w = dlna_text_item_attr[i].width;
  h = dlna_text_item_attr[i].heigt;
  
  p_text = ctrl_create_ctrl(CTRL_TEXT, (IDC_DLNA_TEXT_INFO_1 + i),
                                           x, y, w, h, p_background1, 0);
  
  ctrl_set_rstyle(p_text, dlna_text_item_attr[i].rstyle->n_xstyle, 
                                        dlna_text_item_attr[i].rstyle->f_xstyle, 
                                         dlna_text_item_attr[i].rstyle->g_xstyle);
  
  text_set_font_style(p_text, dlna_text_item_attr[i].fstyle->n_xstyle,
                                                dlna_text_item_attr[i].fstyle->f_xstyle, 
                                                dlna_text_item_attr[i].fstyle->g_xstyle);
  
  text_set_align_type(p_text, dlna_text_item_attr[i].align_attr);
  text_set_content_type(p_text, dlna_text_item_attr[i].type_attr);
  
  //gb2312_to_unicode(text_str[i], strlen(text_str[i]), uni_str, 64);
  text_set_content_by_strid(p_text, (u16)dlna_text_item_attr[i].str_id);
}

  //dlna ready icon
  p_dlna_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_BG_P_ICON,\
  	                                  790, 190 + DLNA_Y_OFFSET,\
  	                                  160, 160,\
  	                                  p_background1, 0);
  bmap_set_content_by_id(p_dlna_icon, IM_DLNA_POWER_OFF);

  //dlna off background
  p_background2 = ctrl_create_ctrl(CTRL_CONT, IDC_DLNA_BG_2,
                                 2, 0,
                                 1034, 426,
                                 p_cont, 0);
  ctrl_set_rstyle(p_background2, RSI_IGNORE,RSI_IGNORE,RSI_IGNORE);
  g_dlna.p_bg2 = p_background2;
  for(i = 0; i < 4; i++)
{
  x = dlna_text2_item_attr[i].left;
  y = dlna_text2_item_attr[i].top;
  w = dlna_text2_item_attr[i].width;
  h = dlna_text2_item_attr[i].heigt;
  
  p_text = ctrl_create_ctrl(CTRL_TEXT, (IDC_DLNA_TEXT_INFO_11 + i),
                                           x, y, w, h, p_background2, 0);
  
  ctrl_set_rstyle(p_text, dlna_text2_item_attr[i].rstyle->n_xstyle, 
                                        dlna_text2_item_attr[i].rstyle->f_xstyle, 
                                         dlna_text2_item_attr[i].rstyle->g_xstyle);
  
  text_set_font_style(p_text, dlna_text2_item_attr[i].fstyle->n_xstyle,
                                                dlna_text2_item_attr[i].fstyle->f_xstyle, 
                                                dlna_text2_item_attr[i].fstyle->g_xstyle);
  
  text_set_align_type(p_text, dlna_text2_item_attr[i].align_attr);
  text_set_content_type(p_text, dlna_text2_item_attr[i].type_attr);
  
  //gb2312_to_unicode(text_str2[i], strlen(text_str2[i]), uni_str, 64);
  text_set_content_by_strid(p_text, (u16)dlna_text2_item_attr[i].str_id);
}
  
  p_icon_2 = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_ICON_INFO_3,\
  	                                  80, 160,\
  	                                  200, 80,\
  	                                  p_background2, 0);
  bmap_set_align_type(p_icon_2, STL_CENTER | STL_VCENTER);
  bmap_set_content_by_id(p_icon_2, IM_LOGO_TX);
  
  //dlna on icon
  p_dlna_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_BG_P_ICON_2,
  	                                  790, 250,//190,
  	                                  160, 160,
  	                                  p_background2, 0);
  bmap_set_content_by_id(p_dlna_icon, IM_DLNA_POWER_ON);
  
 // ctrl_set_sts(p_background2, OBJ_STS_HIDE);
 if(DLNA_OFF == ui_get_dlna_status())
 {
    ctrl_set_sts(p_background1, OBJ_STS_SHOW);
    ctrl_set_sts(p_background2, OBJ_STS_HIDE);
 }else if(DLNA_ON == ui_get_dlna_status())
 {
    ctrl_set_sts(p_background1, OBJ_STS_HIDE);
    ctrl_set_sts(p_background2, OBJ_STS_SHOW);
 }else
 {
   ctrl_set_sts(p_background1, OBJ_STS_SHOW);
   ctrl_set_sts(p_background2, OBJ_STS_HIDE);
 }
  //vert line
  p_vert_line = ctrl_create_ctrl(CTRL_BMAP, IDC_DLNA_BG_V_LINE,\
  	                                  690, 85,\
  	                                  22, 382,\
  	                                  p_cont, 0);
  bmap_set_content_by_id(p_vert_line, IM_DLNA_BG_LINE);
   
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  set_dlna_promotion(TRUE);
  
  ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
  
  return SUCCESS;

}

void ui_set_dlna_status(u32 status)
{
    g_dlna.g_dlna_sts = (dlna_sts)status;
}

u8 ui_get_dlna_status()
{
    return g_dlna.g_dlna_sts;
}

static void ui_open_dlna()
{
    comm_dlg_data_t dlg_data_on =
    {
        ROOT_ID_DLNA_START,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_OPENING_DLNA,
        0,
    };

    OS_PRINTF("####%s####\n", __FUNCTION__);
    ui_comm_dlg_open(&dlg_data_on);
    dlna_init();
    ui_dlna_start();
    ui_set_dlna_status(DLNA_OPENING);
}

static void ui_close_dlna()
{
    comm_dlg_data_t dlg_data_off =
    {
        ROOT_ID_DLNA_START,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_CLOSING_DLNA,
        0,
    };

    OS_PRINTF("####%s####\n", __FUNCTION__);
    ui_comm_dlg_open(&dlg_data_off);
    ui_dlna_stop();
    ui_set_dlna_status(DLNA_CLOSING);
}

static RET_CODE on_dlna_select(control_t *p_list, 
								u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
    net_conn_stats_t eth_connt_stats = {0};
  
    eth_connt_stats = ui_get_net_connect_status();
    if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE))
    {
        ui_comm_cfmdlg_open(NULL, IDS_DLNA_WARNING_INFO1, NULL, 0);
        return ERR_FAILURE;
    }
    
    //start dlna
    if(ui_get_satip_status() == SATIP_ON)
    {
        ui_comm_cfmdlg_open(NULL, IDS_TO_CLOSE_SATIP, NULL, 2000);
        ui_set_closing_satip_at_dlna(TRUE);
        ui_cg_dlna_server_stop();
        ui_stop_mini_httpd();
        ui_set_satip_status(SATIP_OFF);
    }
    
    if(ui_get_dlna_status() == DLNA_OFF)
    {
        ui_open_dlna();
    }
    else if(ui_get_dlna_status() == DLNA_ON)
    {
        ui_close_dlna();
    }    
    
    return ret;
}

static void exit_dlna_menu()
{
    ui_video_c_destroy();
    manage_close_menu(ROOT_ID_DLNA_START, 0, 0);
    set_dlna_promotion(FALSE);
}

static RET_CODE on_pre_exit_dlna_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
        OS_PRINTF("##%s, busying##\n", __FUNCTION__);
        return SUCCESS;
    }

    if(ui_get_dlna_exit_flag() == FALSE)
    {
        ui_set_dlna_exit_flag(TRUE);
    }
    
    if(DLNA_ON == ui_get_dlna_status())
    {
        OS_PRINTF("##%s, will close dlna##\n", __FUNCTION__);
        ui_close_dlna();
    }
    else if(DLNA_OFF == ui_get_dlna_status())
    {
        ui_set_dlna_exit_flag(FALSE);
        exit_dlna_menu();
    }
    
    
    return SUCCESS;  
}

static RET_CODE on_dlna_switch_on_off_get_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####%s####\n", __FUNCTION__);

    if(msg == MSG_ON_OFF_DLNA_SUCCESS)
    {
        if(ui_get_dlna_status() == DLNA_OPENING)
        {
            OS_PRINTF("####open dlna success####\n");
            ui_set_dlna_status(DLNA_ON);
        }
        else if(ui_get_dlna_status() == DLNA_CLOSING)
        {
            OS_PRINTF("####close dlna success####\n");
            ui_set_dlna_status(DLNA_OFF);
            if(ui_get_dlna_exit_flag() == TRUE)
            {
                OS_PRINTF("####before exit DLNA, first close DLNA####\n");
                ui_comm_dlg_close();
                ui_set_dlna_exit_flag(FALSE);
                exit_dlna_menu();
                on_dlna_mode_change(g_dlna.g_dlna_sts);
                return SUCCESS;
            }
        }
        ui_comm_dlg_close();
    }
    else if(msg == MSG_ON_OFF_DLNA_FAILED)
    {
        if(ui_get_dlna_status() == DLNA_OPENING)
        {
            OS_PRINTF("####open dlna failed####\n");
            ui_set_dlna_status(DLNA_OFF);
            ui_comm_cfmdlg_open(NULL, IDS_OPEN_DLNA_FAILED, NULL, 1000);
        }
        else if(ui_get_dlna_status() == DLNA_CLOSING)
        {
            OS_PRINTF("####close dlna failed####\n");
            ui_set_dlna_status(DLNA_ON);
            ui_comm_cfmdlg_open(NULL, IDS_CLOSE_DLNA_FAILED, NULL, 1000);
        }
        ui_comm_dlg_close();
       
    }
    on_dlna_mode_change(g_dlna.g_dlna_sts);
    on_dlna_frame_refresh(NULL, msg, para1, g_dlna.g_dlna_sts);

    return SUCCESS;
}


static RET_CODE on_ui_dlna_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####%s####\n", __FUNCTION__);
    if(ui_get_dlna_status() == DLNA_ON)
    {
        OS_PRINTF("####%s, destory dlna but switch is on, will close switch####\n", __FUNCTION__);
        ui_comm_cfmdlg_open(NULL, IDS_CLOSING_DLNA, NULL, 1000);
        ui_dlna_stop();
        ui_set_dlna_status(DLNA_OFF);
        ui_set_closing_satip_at_dlna(FALSE);
    }
    return ERR_NOFEATURE;
}

BEGIN_KEYMAP(dlna_keymap, NULL)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(dlna_keymap, NULL)

BEGIN_MSGPROC(dlna_proc,ui_comm_root_proc)
    ON_COMMAND(MSG_SELECT, on_dlna_select)
    ON_COMMAND(MSG_EXIT, on_pre_exit_dlna_menu)
    ON_COMMAND(MSG_ON_OFF_DLNA_SUCCESS, on_dlna_switch_on_off_get_msg)
    ON_COMMAND(MSG_ON_OFF_DLNA_FAILED, on_dlna_switch_on_off_get_msg)
    ON_COMMAND(MSG_DESTROY, on_ui_dlna_destroy)
END_MSGPROC(dlna_proc, ui_comm_root_proc)


