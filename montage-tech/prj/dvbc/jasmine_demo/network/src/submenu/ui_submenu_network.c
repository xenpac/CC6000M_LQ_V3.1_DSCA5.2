/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_submenu_network.h"
#include "commonData.h"
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"


enum control_id
{
    IDC_INVALID = 0,
    IDC_NETWORK_ICON_BOX,
    IDC_NETWORK_BTM_LINE,
    IDC_SUB_BTM_HELP,
};

enum network_local_msg
{
    MSG_NETWORK_LEFT = MSG_LOCAL_BEGIN + 1050,
    MSG_NETWORK_RIGHT,
    MSG_NETWORK_UP,
    MSG_NETWORK_DOWN,
};

enum network_app_id
{
  APP_NETWORK_CONFIG_ID = 0,
#if ENABLE_NETMEDIA
  APP_NETMEDIA_ID,
#endif
  APP_WEATHER_ID,
  APP_MAP_ID,
  APP_YAHOO_ID,
#if ENABLE_JAMENDO
  APP_MUSIC_ID,
#endif
  APP_PHOTO_ID,
  APP_DLNA_ID,
  
  APP_SATIP_ID,
#if ENABLE_IPTV
  APP_IPTV_ID,
#endif
#if ENABLE_VOD
  APP_VOD_ID,
#endif
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

u16 icon_mbox_keymap(u16 key);
RET_CODE icon_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE submenu_network_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_submenu_network(u32 para1, u32 para2)
{
  control_t *p_menu = NULL,*p_mbox_icon = NULL, *p_bottom_help = NULL;
  u16 i = 0;
  u16 total;
  
  u16 net_image_hl[] = 
    {
	  IM_INDEX_BG,
	#if ENABLE_NETMEDIA
      IM_APP_NETWORKPLAY_2,
    #endif
	  IM_APP_WEATHER_2,
	  IM_APP_MAP_2,
	  IM_APP_YAHOO_2,
#if ENABLE_JAMENDO
	  IM_APP_MUSIC_2,
#endif
	  IM_APP_PHOTO_2, 
	  IM_APP_DLNA_2, 
	  IM_APP_SATIP_2,
	#if ENABLE_IPTV
	  IM_INDEX_BG,
	#endif
	#if ENABLE_VOD
	  IM_INDEX_BG
	#endif
    };

  u16 net_image_sh[] = 
    {
      IM_INDEX_BG,  
    #if ENABLE_NETMEDIA
  	  IM_APP_NETWORKPLAY_1, 
    #endif
  	  IM_APP_WEATHER_1,
  	  IM_APP_MAP_1,
  	  IM_APP_YAHOO_1,
#if ENABLE_JAMENDO
  	  IM_APP_MUSIC_1,
#endif
   	  IM_APP_PHOTO_1,
  	  IM_APP_DLNA_1,
  	  IM_APP_SATIP_1,
    #if ENABLE_IPTV
	  IM_INDEX_BG,
    #endif
    #if ENABLE_VOD
	  IM_INDEX_BG
    #endif
    };
  u16 text_str[] =
    {
	  IDS_NETWORK_CONFIG,
	#if ENABLE_NETMEDIA
	  IDS_YOUTUBE,
	#endif
	  IDS_WEATHER_FORECAST,
	  IDS_GOOGLE_MAP, 
	  IDS_NETWORK_YAHOO_NEWS,
#if ENABLE_JAMENDO
	  IDS_JAMENDO,
#endif
	  IDS_PICTURE,
	  IDS_DLNA, 
	  IDS_SATIP,
	#if ENABLE_IPTV
	  IDS_IPTV,
	#endif
	#if ENABLE_VOD
	  IDS_VOD
	#endif
    };


  comm_help_data_t help_data = 
    {
      3,3,
      {IDS_MOVE,IDS_OK,IDS_MENU},
      {IM_HELP_ARROW,IM_HELP_OK,IM_HELP_MENU}
    };
  total = ARRAY_SIZE(net_image_hl);
   //main menu
  p_menu = ui_comm_root_create(ROOT_ID_SUBMENU_NETWORK,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_INDEX_NETWORK_BANNER, IDS_NETWORK);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_menu, submenu_network_proc);
  ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);

 //icon box
  p_mbox_icon = ctrl_create_ctrl(CTRL_MBOX, IDC_NETWORK_ICON_BOX,
                                 UI_SUBMENU_NET_CTRL_X, UI_SUBMENU_NET_CTRL_Y,
                                 UI_SUBMENU_NET_CTRL_W , UI_SUBMENU_NET_CTRL_H ,
                                 p_menu, 0);
  ctrl_set_keymap(p_mbox_icon, icon_mbox_keymap);
  ctrl_set_proc(p_mbox_icon,icon_mbox_proc);
  ctrl_set_mrect(p_mbox_icon, NET_MBOX_L, NET_MBOX_T, NET_MBOX_R, NET_MBOX_B);
  mbox_enable_icon_mode(p_mbox_icon, TRUE);
  mbox_enable_string_mode(p_mbox_icon, TRUE);
  mbox_set_count(p_mbox_icon, total, NETWORK_SUBMENU_COL, NETWORK_SUBMENU_ROW);
  mbox_set_win(p_mbox_icon, NETWORK_SUBMENU_WINCOL, NETWORK_SUBMENU_WINROW);
  mbox_set_item_interval(p_mbox_icon, 0, 0);
  mbox_set_item_rstyle(p_mbox_icon, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
  mbox_set_string_fstyle(p_mbox_icon, FSI_RED, FSI_WHITE, FSI_WHITE);
  mbox_set_content_strtype(p_mbox_icon, MBOX_STRTYPE_STRID);
  mbox_set_icon_align_type(p_mbox_icon, STL_CENTER | STL_TOP);
  mbox_set_string_offset(p_mbox_icon, 0, 85);
  mbox_set_string_align_type(p_mbox_icon, STL_CENTER | STL_TOP);
  for(i = 0; i < total; i++)
  {
    mbox_set_content_by_icon(p_mbox_icon, i, net_image_hl[i], net_image_sh[i]);
    mbox_set_content_by_strid(p_mbox_icon, i, text_str[i]);
  }
  
  mbox_set_focus(p_mbox_icon, 0);
  
  ctrl_set_style(p_mbox_icon, STL_EX_ALWAYS_HL);

  //help container
  p_bottom_help = ctrl_create_ctrl(CTRL_CONT, IDC_SUB_BTM_HELP,
                                      SUBMENU_BOTTOM_HELP_X, SUBMENU_BOTTOM_HELP_Y,
                                      SUBMENU_BOTTOM_HELP_W, SUBMENU_BOTTOM_HELP_H,
                                      p_menu, 0);
  ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  ui_comm_help_create_ext(200,0,SUBMENU_BOTTOM_HELP_W - 200 * 2,SUBMENU_BOTTOM_HELP_H ,&help_data, p_bottom_help);

  //
  ctrl_default_proc(p_mbox_icon, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

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

static BOOL ui_check_satip_lan_wifi_conn_sts(void)
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32  
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE))
  {
     ret = ERR_FAILURE;  
     ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
  }
#endif
  return ret;
}


static u16 ui_mmbox_get_next_item(control_t *ctrl,
                               u16 curn,
                               s16 offset,
                               BOOL is_hori)
{
  u16 z = 0, o = 0;
  u16 total = 0, col = 0, row = 0;
  u16 item = 0;
  u8 left_cnt = 0, i = 0;
  u16 m = 0;

  MT_ASSERT(ctrl != NULL);

  mbox_get_count(ctrl, &total, &col, &row);
  z = curn / col, o = curn % col;

  if(is_hori)
  {
    o = (o + offset + col) % col;
  }
  else
  {
    z = (z + offset + row) % row;
  }

  item = z * col + o;

  //  
  if(total < col * row)
  {
     left_cnt = col * row - total;
     for(i = 0; i < left_cnt; i ++)
	 {
		 m = total + i;
		 if(item == m)
		 {
			 if(is_hori)
			 {
				 z = curn / (col - left_cnt), o = curn % (col - left_cnt);
			     o = (o + offset + left_cnt) % left_cnt;
				 item = z * left_cnt + o;   

				 break;
			 }
			 else
			 {
				 z = curn / col, o = curn % col;
			     z = (z + offset + row - 1) % (row - 1);
				 item = z * col + o;

				 break;
			 }
		 }
	 }
  }


  return item;
}


static void ui_mmbox_switch_focus(control_t *ctrl,
                               u16 new_focus)
{
  u16 old_focus = 0;

  old_focus = mbox_get_focus(ctrl);
  mbox_set_focus(ctrl, new_focus);

  MT_ASSERT(ctrl != NULL);

//if(!_mmbox_recl_original(ctrl))
//{
    mbox_draw_item_ext(ctrl, old_focus, TRUE);
    mbox_draw_item_ext(ctrl, new_focus, TRUE);
//}
  //else
  //{
  //  ctrl_paint_ctrl(ctrl, TRUE);
  //}
}


static  BOOL ui_mmbox_change_focus(control_t *ctrl, s16 offset, BOOL is_hori)
{
  u16 z = 0;
  BOOL find_ret = FALSE;
  u16 focus = 0;
  u16 total = 0, col = 0, row = 0;

  MT_ASSERT(ctrl != NULL);
  focus = mbox_get_focus(ctrl);
  
  mbox_get_count(ctrl, &total, &col, &row);
  if((is_hori ? col : row) <= 1)
  {
    return FALSE;
  }

  z = focus;
  find_ret = FALSE;
  
  do
  {
    z = ui_mmbox_get_next_item(ctrl, z, offset, is_hori);
    
    if(mbox_get_item_status(ctrl, z) == 0)
    {
      if(z != focus)
      {
        find_ret = TRUE;
      }
      break;
    }
  }
  while(z != focus);

  if(find_ret)
  {
    ui_mmbox_switch_focus(ctrl, z);
    
    return TRUE;
  }
  return FALSE;
}


static RET_CODE on_icon_box_change_focus(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  s16 offset = 1;
  BOOL is_hori = FALSE;
    
  if(msg == MSG_NETWORK_LEFT
    || msg == MSG_NETWORK_RIGHT)
  {
    is_hori = TRUE;
  }

  if(msg == MSG_NETWORK_LEFT
    || msg == MSG_NETWORK_UP)
  {
    offset = -1;
  }

  ui_mmbox_change_focus(p_ctrl, offset, is_hori);

return SUCCESS;
}

static RET_CODE on_icon_box_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  
#if ENABLE_IPTV
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_SUBMENU_NETWORK,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_GET_LIVETV_SUCCESS,
        2000,
    };
#endif
  
  focus = mbox_get_focus(p_ctrl);
  if(para2 == 1) //used for open livetv after get livetv data
  {
  #if ENABLE_IPTV
    if(focus == APP_IPTV_ID)
    {
        ui_comm_dlg_open(&dlg_data);
        manage_open_menu(ROOT_ID_IPTV, 0, 0);
    }
  #endif
    return SUCCESS;
  }
  
  switch(focus)
  {
    case APP_NETWORK_CONFIG_ID:
      manage_open_menu(ROOT_ID_NETWORK_CONFIG, 0, 0);
      break;
#if ENABLE_NETMEDIA   
    case APP_NETMEDIA_ID:
	  if(ERR_FAILURE == check_network_conn_stats())
	  {
	    return ERR_FAILURE;
	  }
	  manage_open_menu(ROOT_ID_SUBMENU_NM, 0, 0);
	  break;
#endif
   
	  case APP_WEATHER_ID:
		   if(ERR_FAILURE == check_network_conn_stats())
		  {
			return ERR_FAILURE;
		  }
		   manage_open_menu(ROOT_ID_WEATHER_FORECAST, 0, 0);
	  	break;
      
	  case APP_MAP_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_GOOGLE_MAP, 0, 0);
	  	break;
      
	  case APP_YAHOO_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_YAHOO_NEWS, 0, 0);
	    break;

      #if ENABLE_JAMENDO
	  case APP_MUSIC_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_NETWORK_MUSIC, 0, 0);
		break;
    #endif
	  case APP_PHOTO_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_ALBUMS, 0, 0);
		break;
    
	  case APP_DLNA_ID:	  	
		 if(ERR_FAILURE == ui_check_satip_lan_wifi_conn_sts())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_DLNA_DMR, 0, 0);
		break;
        
        case APP_SATIP_ID:	  	
		 if(ERR_FAILURE == ui_check_satip_lan_wifi_conn_sts())
		{
		  return ERR_FAILURE;
		}
		 manage_open_menu(ROOT_ID_SATIP, 0, 0);
		break;
    #if ENABLE_IPTV
	  case APP_IPTV_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}

         Iptv_Get_Pg_Info(0, 0);

		 break;
    #endif
	#if ENABLE_VOD
         case APP_VOD_ID:	  	
		 if(ERR_FAILURE == check_network_conn_stats())
		{
		  return ERR_FAILURE;
		}
         manage_open_menu(ROOT_ID_IPTV, 0, 0);
		 break;
	#endif
    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE on_submenu_network_destroy(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  //fifo_db_destroy();
 // mem_mgr_release_block(BLOCK_FIFO_DB_BUFFER);
  
  return ERR_NOFEATURE;
}


#if ENABLE_IPTV
static RET_CODE on_open_iptv_menu_after_get_iptv_data(control_t *p_ctrl, 
u16 msg, u32 para1, u32 para2)
{
    control_t *p_mbox = NULL;
    UI_PRINTF("######%s, line[%d]#########\n", __FUNCTION__, __LINE__);

    if(msg == MSG_OPEN_IPTV_MENU)
    {
        p_mbox = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU_NETWORK, IDC_NETWORK_ICON_BOX);
        on_icon_box_select(p_mbox, msg, para1, 1);
    }
    else
    {
        extern  u16 ui_get_customer_iptv_pg_cnt(void);
        ui_comm_dlg_close();
        if(ui_get_customer_iptv_pg_cnt() > 0)
        {
            manage_open_menu(ROOT_ID_IPTV, ENTRY_CUS_GRP, 0);
        }
    }
    
    return SUCCESS;
}
#endif

BEGIN_MSGPROC(submenu_network_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_submenu_network_destroy)
  #if ENABLE_IPTV
  ON_COMMAND(MSG_OPEN_IPTV_MENU, on_open_iptv_menu_after_get_iptv_data)
  ON_COMMAND(MSG_OPEN_IPTV_CUS_MENU, on_open_iptv_menu_after_get_iptv_data)
  #endif
END_MSGPROC(submenu_network_proc, ui_comm_root_proc)

BEGIN_KEYMAP(icon_mbox_keymap, NULL)
/*
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
*/
  ON_EVENT(V_KEY_LEFT, MSG_NETWORK_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_NETWORK_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_NETWORK_UP)
  ON_EVENT(V_KEY_DOWN, MSG_NETWORK_DOWN)

  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(icon_mbox_keymap, NULL)

BEGIN_MSGPROC(icon_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_icon_box_select)
  ON_COMMAND(MSG_NETWORK_LEFT, on_icon_box_change_focus)
  ON_COMMAND(MSG_NETWORK_RIGHT, on_icon_box_change_focus)
  ON_COMMAND(MSG_NETWORK_UP, on_icon_box_change_focus)
  ON_COMMAND(MSG_NETWORK_DOWN, on_icon_box_change_focus)
  
END_MSGPROC(icon_mbox_proc, mbox_class_proc)


