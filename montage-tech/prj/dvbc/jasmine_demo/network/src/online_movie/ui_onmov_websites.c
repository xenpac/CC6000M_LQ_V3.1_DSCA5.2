/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_onmov_websites.h"
#include "ui_onmov_api.h"
#include "netplay.h"
#include "ui_video.h"

/*!
 * Onmov websites message
 */
 /*
typedef enum
{
    MSG_ONLINE_MOVIE = MSG_LOCAL_BEGIN + 0x100,
} ui_onmov_msg_t;
*/

enum control_id
{
    IDC_INVALID = 0,
    IDC_OMW_WLIST_CONT,
    IDC_OMW_WITEM_START,
    IDC_OMW_WITEM_END = IDC_OMW_WITEM_START + OMW_WLIST_ITEM_NUM_ONE_PAGE,
    IDC_OMW_WITEM_PIC,
    
    IDC_OMW_BTM_LINE,
    IDC_HELP
};

comm_dlg_data_t net_web_loading_dlg_data =
{
	ROOT_ID_INVALID,
	DLG_FOR_ASK | DLG_STR_MODE_STATIC,
	COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
	IDS_LOADING_WITH_WAIT,
	0,
};

static netplay_website_info_t* sg_p_webs = NULL;
static u16 sg_web_cnt = 0;
static s16 sg_web_focus = 0;
static u8 sg_draw_end_flag[OMW_WLIST_ITEM_NUM_ONE_PAGE] = {0};
static u8 sg_cur_draw_idx = 0;
//static NETPLAY_SERVR_INFO *onmov_server_info = NULL;
//static BOOL sg_network_win_opened = FALSE;

static RET_CODE omw_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static u16 omw_wlist_keymap(u16 key);
static RET_CODE omw_wlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static void omw_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);

  //ui_init_play_timer(ROOT_ID_ONMOV_WEBSITES, MSG_MEDIA_PLAY_TMROUT, 100);
}

static void omw_pic_deinit(void)
{
  pic_stop();

  ui_pic_release();

  //ui_deinit_play_timer();
}

static control_t *omw_creat_wlist(control_t *p_menu)
{
    control_t *p_list_cont = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    u16 i;
    u16 x, y;

    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_OMW_WLIST_CONT,
                                  OMW_WLIST_CONTX, OMW_WLIST_CONTY,
                                  OMW_WLIST_CONTW, OMW_WLIST_CONTH,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    x = OMW_WLIST_ITEM_X;
    y = OMW_WLIST_ITEM_Y;
    for (i = 0; i < OMW_WLIST_ITEM_NUM_ONE_PAGE; i++)
    {
      p_item = ctrl_create_ctrl(CTRL_CONT, IDC_OMW_WITEM_START + i,
                              x, y,
                              OMW_WLIST_ITEM_W, OMW_WLIST_ITEM_H,
                              p_list_cont, 0);
      ctrl_set_rstyle(p_item, RSI_PBACK, RSI_MV_POSTER_FRM_HL, RSI_PBACK);

      p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_OMW_WITEM_PIC,
                              OMW_WLIST_PIC_X, OMW_WLIST_PIC_Y,
                              OMW_WLIST_PIC_W, OMW_WLIST_PIC_H,
                              p_item, 0);
      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

      x += OMW_WLIST_ITEM_W;
      if (x + OMW_WLIST_PIC_W > OMW_WLIST_CONTW)
      {
        x = OMW_WLIST_ITEM_X;
        y += OMW_WLIST_ITEM_H;
      }
    }

    return p_list_cont;
}

static BOOL omw_wlist_set_focus_pos(control_t *p_list_cont, u16 focus)
{
  control_t *p_active = NULL, *p_next_ctrl = NULL;
  u16 index;

  p_active = p_list_cont->p_active_child;
  if(p_active != NULL)
  {
    ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
  }

  index = focus % OMW_WLIST_ITEM_NUM_ONE_PAGE;
  p_next_ctrl = ctrl_get_child_by_id(p_list_cont, IDC_OMW_WITEM_START + index);
  if (p_next_ctrl != NULL)
  {
    //ctrl_set_attr(p_next_ctrl, OBJ_ATTR_ACTIVE);
    //ctrl_set_sts(p_next_ctrl, OBJ_STS_SHOW);
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  }

  ctrl_paint_ctrl(p_list_cont, TRUE);

  return TRUE;
}


static RET_CODE omw_update_wlist(control_t* p_list_cont, u16 start, u16 size, u32 context)
{
  control_t *p_item = NULL;
  control_t *p_pic;
  u16 i;

  for (i = 0; i < sg_web_cnt; i++)
  {
    p_item = ctrl_get_child_by_id(p_list_cont, IDC_OMW_WITEM_START + i);
    p_pic = ctrl_get_child_by_id(p_item, IDC_OMW_WITEM_PIC);

    //if (start + i < sg_web_cnt)
    OS_PRINTF("xuhd: paint IM_NOSIGNAL %d\n", i);
      bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
    //else
      //bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
  }

  //omw_wlist_set_focus_pos(p_list_cont, sg_web_focus);
  omw_wlist_set_focus_pos(p_list_cont, 0);
  
  ctrl_paint_ctrl(p_list_cont, TRUE);
#if 0
    {
        void *p_disp = NULL;
        void *p_gpe = NULL;
        void *p_rgn = NULL;
        rect_t rect;

        p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
        MT_ASSERT(NULL != p_disp);
        p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
        MT_ASSERT(NULL != p_gpe);
        p_rgn = ui_get_pic_region();
        MT_ASSERT(NULL != p_rgn);

        ctrl_get_frame(p_list_cont, &rect);
        ctrl_client2screen(p_list_cont, &rect);

        gpe_draw_rectangle_vsb(p_gpe, p_rgn, &rect, 0);
        disp_layer_update_region(p_disp, p_rgn, NULL);
    }
    OS_PRINTF("@@@@@@onmov_update_wlist\n");
    g_curWebsitesRendIdx = g_curWebsitesIndex - (g_curWebsitesIndex % OMW_WLIST_ITEM_NUM_ONE_PAGE);
    ui_play_timer_start();
#endif
  return SUCCESS;
}



static RET_CODE omw_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *root = NULL;

  sg_web_focus = 0;  
  omw_pic_deinit();
  ui_onmov_deinit();

  root = fw_find_root_by_id(ROOT_ID_POPUP);
  if(root)
  {
    ui_comm_dlg_close();
  }

  ui_video_c_destroy();
  return ERR_NOFEATURE;
}

/*
static RET_CODE omw_on_pic_play_start(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_movlist_cont = NULL;
  control_t *p_item = NULL;
  control_t *p_pic = NULL;
  rect_t rect;

  p_movlist_cont = ctrl_get_child_by_id(p_cont, IDC_OMW_WLIST_CONT);
  p_item = ctrl_get_child_by_id(p_movlist_cont, IDC_OMW_WITEM_START + g_curWebsitesRendIdx % OMW_WLIST_ITEM_NUM_ONE_PAGE);
  p_pic = ctrl_get_child_by_id(p_item, IDC_OMW_WITEM_PIC);

  ctrl_get_frame(p_pic, &rect);
  ctrl_client2screen(p_pic, &rect);

  OS_PRINTF("@@@@@@omw_on_pic_play_start\n");
  if (strlen(g_pWebsites[g_curWebsitesRendIdx].logo_url) > 0)
  {
    ui_pic_play_by_url(g_pWebsites[g_curWebsitesRendIdx].logo_url, &rect);

    bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_pic, TRUE);
  }

  g_curWebsitesRendIdx++;

  ui_play_timer_set_state(0);
  return SUCCESS;
}
*/

/*
static RET_CODE omw_on_enter_onmov(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_NETWORK_PLAY, (u32)&sg_p_webs[sg_web_focus], (u32)sg_web_focus);
  return SUCCESS;
}
*/
/*
static RET_CODE omw_on_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
OS_PRINTF("XXXXXXXXXXXXXXXXXXX  enter  %s\n", __FUNCTION__);
  omw_pic_init();
  OS_PRINTF("XXXXXXXXXXXXXXXXXXX  exit  %s\n", __FUNCTION__);
  //g_curWebsitesRendIdx = g_curWebsitesIndex - (g_curWebsitesIndex % OMW_WLIST_ITEM_NUM_ONE_PAGE);
  //ui_play_timer_start();

  return SUCCESS;
}
*/

static void clear_draw_end_flag(void)
{
  int i;
  
  for(i = 0; i < OMW_WLIST_ITEM_NUM_ONE_PAGE; i++)
    sg_draw_end_flag[i] = 0;
  sg_cur_draw_idx = 0;
}

static RET_CODE omw_on_wlist_exit(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  //ui_release_onmov_data();
  //ui_onmov_deinit();
  clear_draw_end_flag();
//  sg_network_win_opened = FALSE;
  return ERR_NOFEATURE;
}

static RET_CODE omw_on_wlist_leftright(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  if (msg == MSG_FOCUS_LEFT)
  {
    sg_web_focus--;
    if (sg_web_focus < 0)
      sg_web_focus = sg_web_cnt - 1;
  }
  else if (msg == MSG_FOCUS_RIGHT)
  {
    sg_web_focus++;
    if (sg_web_focus == sg_web_cnt)
      sg_web_focus = 0;
  }

  omw_wlist_set_focus_pos(p_movlist_cont, sg_web_focus);
  ctrl_paint_ctrl(p_movlist_cont, TRUE);

  return SUCCESS;
}

static RET_CODE omw_on_wlist_select(control_t *p_movlist_cont, u16 msg, u32 para1, u32 para2)
{
  omw_pic_deinit();
  clear_draw_end_flag();
  //sg_network_win_opened = TRUE;
  manage_open_menu(ROOT_ID_NETWORK_PLAY, (u32)&sg_p_webs[sg_web_focus], (u32)sg_web_focus);

  return SUCCESS;
}

RET_CODE open_onmov_websites(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_line = NULL, *p_help_bar = NULL;
    control_t *p_wlist_cont = NULL;


    comm_help_data_t help_data = 
    {
        3,3,
        {IDS_MOVE, IDS_OK, IDS_MENU},
        {IM_HELP_ARROW, IM_HELP_OK, IM_HELP_MENU}
    };

    sg_web_focus = 0;  

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    ui_onmov_init();

    omw_pic_init();
#ifndef WIN32
    netplay_request_server_info();
#endif
        //main menu
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_ONMOV_WEBSITES, 0, 
                                IM_INDEX_NETWORKPLAY_BANNER, IDS_NETWORK);
    MT_ASSERT(p_menu != NULL);
    #ifdef MODEST
  //  ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
    #else 
     ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
    #endif
    ctrl_set_proc(p_menu, omw_cont_proc);
    
        //bottom line
    p_line = ctrl_create_ctrl(CTRL_CONT, IDC_OMW_BTM_LINE, 
                                OMW_BOTTOM_LINE_X, OMW_BOTTOM_LINE_Y, 
                                OMW_BOTTOM_LINE_W, OMW_BOTTOM_LINE_H,
                                p_menu, 0);
     #ifdef MODEST
   // ctrl_set_rstyle(p_line, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_HL);
     #else
     ctrl_set_rstyle(p_line, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_HL, RSI_MV_POSTER_FRM_HL);
     #endif
     
    //list
     p_wlist_cont = omw_creat_wlist(p_menu);
     ctrl_set_keymap(p_wlist_cont, omw_wlist_keymap);
     ctrl_set_proc(p_wlist_cont, omw_wlist_proc);

    //bottom help bar
    p_help_bar = ctrl_create_ctrl(CTRL_TEXT, IDC_HELP,
                                OMW_BOTTOM_HELP_X, OMW_BOTTOM_HELP_Y, 
                                OMW_BOTTOM_HELP_W, OMW_BOTTOM_HELP_H, 
                                p_menu, 0);
    text_set_font_style(p_help_bar, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_help_bar, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_help_bar, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(300, 0, 
                                OMW_BOTTOM_HELP_W-300, OMW_BOTTOM_HELP_H,  
                                &help_data,  p_help_bar);
    ctrl_default_proc(p_menu, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

    fw_tmr_create(ROOT_ID_ONMOV_WEBSITES, MSG_SLEEP_TMROUT, 3000, FALSE);
  

    return SUCCESS;
}

/*
static RET_CODE omw_get_server(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
   control_t *p_menu = NULL;
   control_t *p_wlist_cont = NULL;
    onmov_server_info = (NETPLAY_SERVR_INFO*) para1;

    g_websites_cnt = onmov_server_info->website_count;
    g_pWebsites =onmov_server_info->p_web;
    g_curWebsitesIndex = 0;


   p_menu =  ui_comm_root_get_root(ROOT_ID_ONMOV_WEBSITES);
    p_wlist_cont = ctrl_get_child_by_id(p_menu,IDC_OMW_WLIST_CONT);
    omw_update_wlist(p_wlist_cont,
                                g_curWebsitesIndex - (g_curWebsitesIndex % OMW_WLIST_ITEM_NUM_ONE_PAGE),
                                OMW_WLIST_ITEM_NUM_ONE_PAGE,
                                0);
    omw_wlist_set_focus_pos(p_wlist_cont, g_curWebsitesIndex);


    ctrl_process_msg(p_menu, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_wlist_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  return SUCCESS;
}
*/
static void draw_web_logo(control_t* p_wlist, u8 index)
{
  control_t* p_item = NULL;
  control_t* p_pic = NULL;
  rect_t rc;

  p_item = ctrl_get_child_by_id(p_wlist, IDC_OMW_WITEM_START + index);
  p_pic = ctrl_get_child_by_id(p_item, IDC_OMW_WITEM_PIC);

  ctrl_get_frame(p_pic, &rc);
  ctrl_client2screen(p_pic, &rc);

  if( sg_p_webs[index].p_logo_url )
  {
    sg_cur_draw_idx = index;
    ui_pic_play_by_url(sg_p_webs[index].p_logo_url, &rc, 0);
    //bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    //ctrl_paint_ctrl(p_pic, TRUE);
  }
}

static RET_CODE omw_on_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i = 0;
  control_t* p_menu =  ui_comm_root_get_root(ROOT_ID_ONMOV_WEBSITES);
  control_t* p_wlist = ctrl_get_child_by_id(p_menu, IDC_OMW_WLIST_CONT);
  static u8 err_cnt;

  if( ROOT_ID_ONMOV_WEBSITES != fw_get_focus_id() )
    return SUCCESS;

OS_PRINTF("enter %s\n", __FUNCTION__);
  //if( TRUE == sg_network_win_opened )
    //return SUCCESS;
  if( MSG_PIC_EVT_DRAW_END == msg
    ||(MSG_PIC_EVT_DATA_ERROR == msg && MAX_RETRY_CNT == err_cnt ))
  {
    if( MSG_PIC_EVT_DRAW_END == msg )
    {
      control_t* p_item = ctrl_get_child_by_id(p_wlist, IDC_OMW_WITEM_START + sg_cur_draw_idx);
      control_t* p_pic = ctrl_get_child_by_id(p_item, IDC_OMW_WITEM_PIC);
  bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
  ctrl_paint_ctrl(p_pic, TRUE);
    }
    err_cnt = 0;
  sg_draw_end_flag[sg_cur_draw_idx] = 1;
  pic_stop();

  for(i = 0; i < sg_web_cnt; i++) 
    if( 0 == sg_draw_end_flag[i] )
      break;

  if( i < sg_web_cnt)
  {
    sg_cur_draw_idx = i;
    draw_web_logo(p_wlist, i);
    }
  }
  else if( MSG_PIC_EVT_DATA_ERROR == msg )
  {
    err_cnt++;
    pic_stop();
    draw_web_logo(p_wlist, sg_cur_draw_idx);
  }

  return SUCCESS;
}


static RET_CODE omw_get_server(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_wlist_cont = NULL;
  netplay_server_info_t* p_serv = NULL;

  fw_tmr_destroy(ROOT_ID_ONMOV_WEBSITES, MSG_SLEEP_TMROUT);
  ui_comm_dlg_close();

  p_serv = (netplay_server_info_t*)para1;
  sg_web_cnt = p_serv->website_count;
  sg_p_webs = p_serv->p_web;
OS_PRINTF("xuhd: %s,%d\n", __FUNCTION__, __LINE__);  
  p_wlist_cont = ctrl_get_child_by_id(p_cont, IDC_OMW_WLIST_CONT);

  omw_update_wlist(p_wlist_cont, 0, OMW_WLIST_ITEM_NUM_ONE_PAGE, 0);

do
{
  void *p_disp = NULL;
  void *p_gpe = NULL;
  void *p_rgn = NULL;
  rect_t rect;

  p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);
  p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe);
  p_rgn = ui_get_pic_region();
  MT_ASSERT(NULL != p_rgn);

  ctrl_get_frame(p_wlist_cont, &rect);
  ctrl_client2screen(p_wlist_cont, &rect);

  gpe_draw_rectangle_vsb(p_gpe, p_rgn, &rect, 0);
  disp_layer_update_region(p_disp, p_rgn, NULL);
}while(0);
OS_PRINTF("xuhd: %s,%d\n", __FUNCTION__, __LINE__);  
//  omw_pic_init();
  draw_web_logo(p_wlist_cont, 0);
  return SUCCESS;
}
static RET_CODE omw_on_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u8 i;
  control_t* p_wlist = ctrl_get_child_by_id(p_cont, IDC_OMW_WLIST_CONT);
  control_t* p_item = NULL;
  control_t* p_pic = NULL;


  for( i = 0; i < sg_web_cnt; i++ )
  {
    p_item = ctrl_get_child_by_id(p_wlist, IDC_OMW_WITEM_START + i);
    p_pic = ctrl_get_child_by_id(p_item, IDC_OMW_WITEM_PIC);
    bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
  }
  ctrl_paint_ctrl(p_wlist, TRUE);

  omw_pic_init();
  draw_web_logo(p_wlist, 0);

  return SUCCESS;
}

static RET_CODE omw_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_LOADING_WITH_WAIT, NULL, 0);

  return SUCCESS;
}

BEGIN_MSGPROC(omw_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, omw_on_destory)
  //ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, omw_on_pic_play_start)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, omw_on_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, omw_on_pic_draw_end)
  //ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_picture_too_large)
  //ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_picture_unsupport)  
  //ON_COMMAND(MSG_ONLINE_MOVIE, omw_on_enter_onmov)  
  ON_COMMAND(MSG_UPDATE, omw_on_update)  
  ON_COMMAND(MSG_ONMOV_EVT_SERVER, omw_get_server)  
  ON_COMMAND(MSG_SLEEP_TMROUT,omw_open_dlg)
END_MSGPROC(omw_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(omw_wlist_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(omw_wlist_keymap, NULL)

BEGIN_MSGPROC(omw_wlist_proc, cont_class_proc)
    ON_COMMAND(MSG_EXIT, omw_on_wlist_exit)
    ON_COMMAND(MSG_EXIT_ALL, omw_on_wlist_exit)
    ON_COMMAND(MSG_FOCUS_LEFT, omw_on_wlist_leftright)
    ON_COMMAND(MSG_FOCUS_RIGHT, omw_on_wlist_leftright)
    ON_COMMAND(MSG_SELECT, omw_on_wlist_select)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, omw_on_pic_draw_end)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, omw_on_pic_draw_end)
END_MSGPROC(omw_wlist_proc, cont_class_proc)

#endif
