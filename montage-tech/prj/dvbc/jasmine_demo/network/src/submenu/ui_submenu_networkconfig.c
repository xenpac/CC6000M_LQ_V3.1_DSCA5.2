/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_submenu_networkconfig.h"

//-------------------------
#ifdef DVBT2_DM6K
#define SUB_NETWORK_COUNT 2
#else
#define SUB_NETWORK_COUNT 4
#endif
#define MAX_OPT_CNT_PER_SUB (3)
#define NONE_LINK_ID (255)

//main window cordinate
#define NET_CONFIG_X (0)
#define NET_CONFIG_Y (0)
#define NET_CONFIG_W (SCREEN_WIDTH) //(1280)
#define NET_CONFIG_H (SCREEN_HEIGHT) //(720)

//pic list cordinate
#define CONFIG_LIST_ITEM_X (0)
#define CONFIG_LIST_ITEM_Y (0)
#define CONFIG_LIST_ITEM_W (175)
#define CONFIG_LIST_ITEM_H (135)

#define CONFIG_LIST_ITEM_PIC_X (3)
#define CONFIG_LIST_ITEM_PIC_Y (3)
#define CONFIG_LIST_ITEM_PIC_W (170)
#define CONFIG_LIST_ITEM_PIC_H (130)

#define CONFIG_LIST_ITEM_FLAG_X (20)
#define CONFIG_LIST_ITEM_FLAG_Y (20)
#define CONFIG_LIST_ITEM_FLAG_W (20)
#define CONFIG_LIST_ITEM_FLAG_H (20)

#define NET_CONFIG_MBOX_X   (150)
#define NET_CONFIG_MBOX_Y   (190) 
#define NET_CONFIG_MBOX_W   (1000)
#define NET_CONFIG_MBOX_H   (420)//(470)//500


#ifdef DVBT2_DM6K
#define CONFIG_LIST_COUNT (2)
#else
#define CONFIG_LIST_COUNT (4)
#endif

#define NET_CONFIG_LIST_VGAP (270)//(220)

//text list cordinate
#define NET_CONFIG_LIST_TEXT_X  (160)
#define NET_CONFIG_LIST_TEXT_Y  (400)
#define NET_CONFIG_LIST_TEXT_W   (170)
#define NET_CONFIG_LIST_TEXT_H   (200)//(300)

#define NET_CONFIG_PER_LIST_TEXT_W  (170)
#define NET_CONFIG_PER_LIST_TEXT_H  (120)

#define MAX_OPT_TEXT_PER_PAGE (4)

#define NET_CONFIG_WINCOL (4)
#define NET_CONFIG_WINROW (1)

//help bar cordinate
#define NET_HELP_X  190
#define NET_HELP_Y  600//570
#define NET_HELP_W  900
#define NET_HELP_H   80

//submenu networkconfig ctrl id
enum control_id
{
  IDC_INVALID = 0,  	
  IDC_NETWORK_CONFIG_TEXT_LIST,
  IDC_NETWORK_CONFIG_LIST,
  IDC_NETWORK_CONFIG_LIST_1,
  IDC_NETWORK_CONFIG_LIST_2,
  IDC_NETWORK_CONFIG_LIST_3,
  IDC_NETWORK_CONFIG_LIST_4,
  IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1,
  IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_2,
  IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_3,
  IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_4,
  IDC_NETWORK_CONFIG_TEXTLIST_1,
  IDC_NETWORK_CONFIG_TEXTLIST_2,
  IDC_NETWORK_CONFIG_TEXTLIST_3,
  IDC_NETWORK_CONFIG_TEXTLIST_4,
  IDC_NETWORK_CONDIF_BACK_PIC,
  IDC_NETWORK_CONDIF_FLAG_PIC,
  IDC_NETWORK_CONFIG_HELP_CONT
};


//the static data
typedef struct
{
  u8 link_root_id;
  u16 subconfig_item_title;
  BOOL (*check_item_accessible)(u8 type);  
}cate_subconfig_t;

enum net_source_type_id
{
  NET_SOURCE_LAN = 1,
  NET_SOURCE_WIFI,
  NET_SOURCE_GPRS,
  NET_SOURCE_3GDONGLE
};

typedef struct
{
  /*the cate index*/
  u8 cate_idx;
  /*the sub index in the cate*/
  u8 sub_idx;
}active_config_t;

static active_config_t config_active_opt;
//help data
static comm_help_data_t help_data = 
{
  3, 3,
  { IDS_MOVE, 
    IDS_CONFIRM,
    IDS_EXIT},
  { IM_HELP_ARROW,
    IM_HELP_OK,
    IM_HELP_MENU}
};

/*return the network connect type*/
static int get_netconnect_type()
{	
	net_config_t net_config = {0};
	sys_status_get_net_config_info(&net_config);
	return net_config.link_type;
}

/*check the paticipate connection is available*/
static BOOL check_network_available(u8 type)
{
	int net_type = get_netconnect_type();
  net_conn_stats_t connt_stats = ui_get_net_connect_status();	
	if(type == net_type)
	{
    switch(type)
      {
        case LINK_TYPE_LAN:
          if(connt_stats.is_eth_insert)
            return TRUE;
          break;
        case LINK_TYPE_WIFI:
          if(connt_stats.is_wifi_insert)
            return TRUE;
          break;
        case LINK_TYPE_GPRS:        
          return TRUE;
        case LINK_TYPE_3G:
          if(connt_stats.is_3g_insert)
            return TRUE;
          break;
        default:
            break;
      }
	}
	return FALSE;
}

#ifdef DVBT2_DM6K
static cate_subconfig_t sg_mmenu_options[SUB_NETWORK_COUNT][MAX_OPT_CNT_PER_SUB] = 
  {
    //&&&&&&&&&&&&&&Wifi&&&&&&&&&&&&&
    {
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_WIFI,
	IDS_WIFI_MANAGER,
	//check_wifidevice_available,
	check_network_available,
      },
      {
	ROOT_ID_NETWORK_CONFIG_WIFI,
	IDS_SETUP,
	check_network_available,
      },
    },
	
    //&&&&&&&&&&&&&&3G Dongle&&&&&&&&&&&&&
    {
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_NETWORK_CONFIG_3G,
	IDS_SETUP,
	check_network_available,
      },
    },
  };

/*the pic id for the network*/
static u16 g_list_pic[SUB_NETWORK_COUNT] = 
		{IM_CONFIG_WIFI, IM_CONFIG_3G};

/*the acount items for the each category*/
static u16 g_option_count[SUB_NETWORK_COUNT] = 
{0,0};
#else
static cate_subconfig_t sg_mmenu_options[SUB_NETWORK_COUNT][MAX_OPT_CNT_PER_SUB] = 
  {
    {
      //&&&&&&&&&&&&&&Lan&&&&&&&&&&&&&
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_NETWORK_CONFIG_LAN,
	IDS_SETUP,
	check_network_available,
      },
    },

    //&&&&&&&&&&&&&&Wifi&&&&&&&&&&&&&
    {
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_WIFI,
	IDS_WIFI_MANAGER,
	//check_wifidevice_available,
	check_network_available,
      },
      {
	ROOT_ID_NETWORK_CONFIG_WIFI,
	IDS_SETUP,
	check_network_available,
      },
    },
	
    //&&&&&&&&&&&&&&Gprs&&&&&&&&&&&&&
    {
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_NETWORK_CONFIG_GPRS,
	IDS_SETUP,
	check_network_available,
      },
    },
	
    //&&&&&&&&&&&&&&3G Dongle&&&&&&&&&&&&&
    {
      {
	NONE_LINK_ID,
	IDS_ACTIVE,
	NULL,
      },
      {
	ROOT_ID_NETWORK_CONFIG_3G,
	IDS_SETUP,
	check_network_available,
      },
    },
  };

/*the pic id for the network*/
static u16 g_list_pic[SUB_NETWORK_COUNT] = 
		{IM_CONFIG_LAN, IM_CONFIG_WIFI, IM_CONFIG_GPRS, IM_CONFIG_3G};

/*the acount items for the each category*/
static u16 g_option_count[SUB_NETWORK_COUNT] = 
{0,0,0,0};
#endif

u16 subnetwork_config_cont_keymap(u16 key);
u16 subconfig_mbox_keymap(u16 key);
static u16 subconfig_pic_list_keymap(u16 key);
static u16 subconfig_text_list_keymap(u16 key);

static RET_CODE subconfig_pic_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE subconfig_text_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE subconfig_mbox_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE subnetwork_config_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE text_list_update(control_t* p_list, u16 start, u16 size, u32 context);


static void init_option_count(void)
{
	u16 i,j;
	for( i = 0; i < SUB_NETWORK_COUNT; i++ )
	{	
	  for(j = 0; j < MAX_OPT_CNT_PER_SUB; j++)
	  	{
		if((j != 0) && (( 255 == sg_mmenu_options[i][j].link_root_id )||( 0 == sg_mmenu_options[i][j].link_root_id )))
		  break;
	  	}
	  g_option_count[i] = j;
	}
}

/*the function is to change the network connect type */
static int switch_connect_type(int type)
{
	net_config_t g_net_config;
	net_conn_stats_t connt_stats;
	connt_stats = ui_get_net_connect_status();	
	sys_status_get_net_config_info(&g_net_config);	
	if((!connt_stats.is_wifi_insert) && (type == LINK_TYPE_WIFI))
	{
		ui_comm_cfmdlg_open(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);		
		return -1;
	}
  /*
	if(type == LINK_TYPE_GPRS)
	{
		ui_comm_cfmdlg_open(NULL, IDS_GPRS_DEV_NOT_EXIST, NULL, 2000);		
		return -1;
	}*/
  if((!connt_stats.is_3g_insert) && (type == LINK_TYPE_3G))
	{
		ui_comm_cfmdlg_open(NULL, IDS_3G_DEV_NOT_EXIST, NULL, 2000);		
		return -1;
	}
  if((!connt_stats.is_eth_insert) && (type == LINK_TYPE_LAN))
	{
		ui_comm_cfmdlg_open(NULL, IDS_LAN_DEV_NOT_EXIST, NULL, 2000);		
		return -1;
	}
	g_net_config.link_type = type;
	sys_status_set_net_config_info(&g_net_config);
	sys_status_save();	
	if(type == LINK_TYPE_LAN)
	{
		ui_comm_cfmdlg_open(NULL, IDS_LAN_CONN_SUC, NULL, 2000);
		
		return 0;
	}
	if(type == LINK_TYPE_GPRS)
  {
		ui_comm_cfmdlg_open(NULL, IDS_GPRS_CONN_SUC, NULL, 2000);
		
		return 0;
  }
	if(type == LINK_TYPE_WIFI)
	{
		ui_comm_cfmdlg_open(NULL, IDS_WIFI_CONN_SUC, NULL, 2000);
		return 0;
	}
  if(type == LINK_TYPE_3G)
   {
		ui_comm_cfmdlg_open(NULL, IDS_3G_CONN_SUC, NULL, 2000);
		return 0;
   }
	return -1;
}

/*the function is to update the list */
static RET_CODE text_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
	u16 i;
	u8 cate_idx = ctrl_get_ctrl_id(p_list) - IDC_NETWORK_CONFIG_TEXTLIST_1;//config_active_opt.cate_idx;
	for(i = 0; i < size; i++)
	{
	  if(i >= g_option_count[cate_idx])
	  {
			continue;
	  }
	  list_set_field_content_by_strid(p_list, i, 0, 
		sg_mmenu_options[cate_idx][i].subconfig_item_title);
	  if( (NULL != sg_mmenu_options[cate_idx][i].check_item_accessible)
		  && (FALSE == sg_mmenu_options[cate_idx][i].check_item_accessible(cate_idx)))
	  {
		list_set_item_status(p_list, i, LIST_ITEM_DISABLED);
	  }
	  else
	  {
		list_set_item_status(p_list, i, LIST_ITEM_NORMAL);
		if(( i == config_active_opt.sub_idx ) && (config_active_opt.cate_idx == cate_idx))
		{
		  list_set_focus_pos(p_list, config_active_opt.sub_idx);
		  list_select_item(p_list, config_active_opt.sub_idx);
		}
	  }
	}
 // ctrl_paint_ctrl(p_list, TRUE);
	return SUCCESS;
}


static control_t *create_submenu_textlist(control_t *p_menu, u16 list_x, u16 list_y, u16 list_w, u16 list_h, int index)
{
	control_t *p_list;
	u8 i = 0;
		list_xstyle_t list_item_style = 
			{
			  RSI_ITEM_8_SH,
			  RSI_PBACK,
			  RSI_ITEM_2_HL,
			  RSI_ITEM_2_HL,
			  RSI_ITEM_2_HL,
			};
		list_xstyle_t list_field_style = 
			{
			  RSI_IGNORE,
			  RSI_IGNORE,
			  RSI_IGNORE,
			  RSI_IGNORE,
			  RSI_IGNORE,
			};
		list_xstyle_t list_field_fstyle =
			{
			  FSI_GRAY,
			  FSI_WHITE,
			  FSI_WHITE,
			  FSI_BLACK,
			  FSI_WHITE,
			};
		list_field_attr_t list_attr = 
			{LISTFIELD_TYPE_STRID| STL_VCENTER,
				   160,5,5,
				   &list_field_style,&list_field_fstyle};
		
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_NETWORK_CONFIG_TEXTLIST_1 + index,
										 list_x, list_y,
										 list_w, list_h,
										 p_menu, 0);
	
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_mrect(p_list, 5, 5, NET_CONFIG_PER_LIST_TEXT_W, NET_CONFIG_PER_LIST_TEXT_H);
    list_set_item_interval(p_list, 5);
    list_set_item_rstyle(p_list, &list_item_style);
    list_enable_select_mode(p_list, TRUE);
    list_enable_cycle_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);
    list_set_count(p_list, MAX_OPT_TEXT_PER_PAGE, 3);//MAX_OPT_TEXT_PER_PAGE);
    list_set_field_count(p_list, 1, MAX_OPT_TEXT_PER_PAGE);
    list_set_update(p_list, text_list_update, 0);

	list_set_field_attr(p_list, (u8)i, (u32)(list_attr.attr), (u16)(list_attr.width),
									  (u16)(list_attr.left), (u8)(list_attr.top));
	list_set_field_rect_style(p_list, (u8)i, list_attr.rstyle);
	list_set_field_font_style(p_list, (u8)i, list_attr.fstyle);
    text_list_update(p_list, list_get_valid_pos(p_list), MAX_OPT_TEXT_PER_PAGE, 0);

	return p_list;
}


static control_t *create_submenu_configlist(control_t *p_menu)
{
    control_t *p_list_cont = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;	
    control_t *p_flag = NULL;	
    control_t *p_item1 = NULL;
	control_t *p_textlist = NULL;
    u16 i;
    u16 x, y;

	int connect_type = get_netconnect_type();
	
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NETWORK_CONFIG_LIST,
                                  NET_CONFIG_MBOX_X, NET_CONFIG_MBOX_Y,
                                  NET_CONFIG_MBOX_W, NET_CONFIG_MBOX_H,
                                  p_menu, 0);
    ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    x = CONFIG_LIST_ITEM_X;
    y = CONFIG_LIST_ITEM_Y;
    for (i = 0; i < CONFIG_LIST_COUNT; i++)
    {
    
      p_item = ctrl_create_ctrl(CTRL_CONT, IDC_NETWORK_CONFIG_LIST_1 + i,
                              x, y,
                              CONFIG_LIST_ITEM_W, CONFIG_LIST_ITEM_H,
                              p_list_cont, 0);
      ctrl_set_rstyle(p_item, RSI_PBACK, RSI_ITEM_11_HL, RSI_PBACK);
	  if(i == 0)
	      ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);

      p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETWORK_CONDIF_BACK_PIC,
                              CONFIG_LIST_ITEM_PIC_X, CONFIG_LIST_ITEM_PIC_Y,
                              CONFIG_LIST_ITEM_PIC_W, CONFIG_LIST_ITEM_PIC_H,
                              p_item, 0);
      ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);	  
      bmap_set_content_by_id(p_ctrl, g_list_pic[i]);

      p_flag = ctrl_create_ctrl(CTRL_BMAP, IDC_NETWORK_CONDIF_FLAG_PIC,
                              CONFIG_LIST_ITEM_FLAG_X, CONFIG_LIST_ITEM_FLAG_Y,
                              CONFIG_LIST_ITEM_FLAG_W, CONFIG_LIST_ITEM_FLAG_H,
                              p_ctrl, 0);
      ctrl_set_rstyle(p_flag, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);	
	  if(connect_type == i){
      		bmap_set_content_by_id(p_flag, IM_CONFIGCONNECT_2);
	  }else{
		    bmap_set_content_by_id(p_flag, IM_CONFIGCONNECT_1);
	  }
	  
      p_item1 = ctrl_create_ctrl(CTRL_CONT, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1 + i,
                              x, y + CONFIG_LIST_ITEM_H + 20,
                              NET_CONFIG_LIST_TEXT_W+5, 200,
                              p_list_cont, 0);
      ctrl_set_rstyle(p_item1, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	  p_textlist = create_submenu_textlist(p_item1, 
	  								0, 0,
	  								NET_CONFIG_LIST_TEXT_W, NET_CONFIG_LIST_TEXT_H, i);
	  
	  ctrl_set_keymap(p_textlist, subconfig_text_list_keymap);
	  ctrl_set_proc(p_textlist, subconfig_text_list_proc);
      x += NET_CONFIG_LIST_VGAP;
    }

    return p_list_cont;
}



RET_CODE open_submenu_networkconfig(u32 para1, u32 para2)
{
  control_t *p_cont, *p_help_cont;
  control_t *p_list_pic;
  //create container
  init_option_count();
  p_cont = ui_comm_root_create_netmenu(ROOT_ID_SUBNETWORK_CONFIG, 0 ,
			       IM_INDEX_NETWORK_BANNER, IDS_NETWORK_CONFIG
			       );
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, subnetwork_config_cont_proc);
  //crate pic list
  p_list_pic = create_submenu_configlist(p_cont);
  ctrl_set_keymap(p_list_pic, subconfig_pic_list_keymap);
  ctrl_set_proc(p_list_pic, subconfig_pic_list_proc);
  //help bar
  p_help_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NETWORK_CONFIG_HELP_CONT,
				 NET_HELP_X, NET_HELP_Y,
				 NET_HELP_W, NET_HELP_H,
				 p_cont, 0);
  ctrl_set_rstyle(p_help_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  //help
  ui_comm_help_create(&help_data, p_help_cont);

  ctrl_default_proc(p_list_pic, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  return SUCCESS;
}


static RET_CODE on_exit_sub_networkconfig(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
	config_active_opt.cate_idx = 0;
	config_active_opt.sub_idx = 0;
	manage_close_menu(ROOT_ID_SUBNETWORK_CONFIG, 0, 0);
	return SUCCESS;
}


static BOOL subconfig_pic_list_set_focus_pos(control_t *p_list_cont, u16 focus)
{
  control_t *p_active = NULL, *p_next_ctrl = NULL;

  p_active = p_list_cont->p_active_child;
  if(p_active != NULL)
  {
    ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
  }

  p_next_ctrl = ctrl_get_child_by_id(p_list_cont, IDC_NETWORK_CONFIG_LIST_1 + focus);
  if (p_next_ctrl != NULL)
  {
    ctrl_process_msg(p_next_ctrl, MSG_GETFOCUS, 0, 0);
  }

  ctrl_paint_ctrl(p_list_cont, TRUE);

  return TRUE;
}

static u16 get_next_focus(u8 cate_idx, u8 cur_opt, u16 msg)
{
	u8 current = cur_opt;
	u8 m =0;
	if(MSG_FOCUS_UP == msg)
	{
		if(LINK_TYPE_WIFI != cate_idx){
			if(current ==0){				
				current = 1;
				if((NULL != sg_mmenu_options[cate_idx][current].check_item_accessible)
						&& (FALSE == sg_mmenu_options[cate_idx][current].check_item_accessible(cate_idx)))
					{
						current = 0;
					}
			}else{
				current = 0;
			}
		}else{
			if(current == 0){
				current = 2;
				if((NULL != sg_mmenu_options[cate_idx][current].check_item_accessible)
						&& (FALSE == sg_mmenu_options[cate_idx][current].check_item_accessible(cate_idx)))
				{
					current = 0;
				}
			}else{
				for(m = cur_opt -1; m > 0; m--)
					{						
						if(NULL == sg_mmenu_options[cate_idx][m].check_item_accessible
						   || TRUE == sg_mmenu_options[cate_idx][m].check_item_accessible(cate_idx))
						{
							return m;
						}
					}
				current = 0;
			}
		}
	}
	else if(MSG_FOCUS_DOWN == msg)
	{
		if(LINK_TYPE_WIFI != cate_idx){
			if(current ==1){
				current = 0;
			}else{
				current = 1;				
				if((NULL != sg_mmenu_options[cate_idx][current].check_item_accessible)
						&& (FALSE == sg_mmenu_options[cate_idx][current].check_item_accessible(cate_idx)))
					{
						current = 0;
					}
			}
		}else{
			if(current == 2){
				current = 0;
			}else{
				for(m = cur_opt +1; m < 3; m++)
					{						
						if(NULL == sg_mmenu_options[cate_idx][m].check_item_accessible
						   || (TRUE == sg_mmenu_options[cate_idx][m].check_item_accessible(cate_idx)))
						{
							return m;
						}
					}
				current = 0;
			}
		}
	}
	return current;
}

static RET_CODE subconfig_pic_list_focusmove(control_t *p_piclist_cont, u16 msg, u32 para1, u32 para2)
{
  control_t* p_menu =  ctrl_get_parent(p_piclist_cont);
  control_t* p_container = ctrl_get_child_by_id(p_menu, IDC_NETWORK_CONFIG_LIST);
  control_t* p_contrainer1 = ctrl_get_child_by_id(p_container, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1+config_active_opt.cate_idx);
  control_t* p_text_list_current = ctrl_get_child_by_id(p_contrainer1, config_active_opt.cate_idx + IDC_NETWORK_CONFIG_TEXTLIST_1);
  control_t* p_text_list_next = NULL;
  control_t* p_text_container_next = NULL;  
  
  u16 new_focus;
 if (msg == MSG_FOCUS_LEFT)
  {
    config_active_opt.cate_idx--;
    if (config_active_opt.cate_idx == 255)
      config_active_opt.cate_idx = CONFIG_LIST_COUNT - 1;	
	config_active_opt.sub_idx= 0;
	p_text_container_next = ctrl_get_child_by_id(p_container, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1+config_active_opt.cate_idx);
	p_text_list_next = ctrl_get_child_by_id(p_text_container_next, config_active_opt.cate_idx + IDC_NETWORK_CONFIG_TEXTLIST_1);
	text_list_update(p_text_list_current, list_get_valid_pos(p_text_list_current), MAX_OPT_TEXT_PER_PAGE, 0);
	text_list_update(p_text_list_next, list_get_valid_pos(p_text_list_next), MAX_OPT_TEXT_PER_PAGE, 0);
 }
  else if (msg == MSG_FOCUS_RIGHT)
  {
    config_active_opt.cate_idx++;
    if (config_active_opt.cate_idx == CONFIG_LIST_COUNT)
      config_active_opt.cate_idx = 0;	
	config_active_opt.sub_idx= 0;
	p_text_container_next = ctrl_get_child_by_id(p_container, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1+config_active_opt.cate_idx);
	p_text_list_next = ctrl_get_child_by_id(p_text_container_next, config_active_opt.cate_idx + IDC_NETWORK_CONFIG_TEXTLIST_1);	
	text_list_update(p_text_list_current, list_get_valid_pos(p_text_list_current), MAX_OPT_TEXT_PER_PAGE, 0);
	text_list_update(p_text_list_next, list_get_valid_pos(p_text_list_next), MAX_OPT_TEXT_PER_PAGE, 0);
  }
  else if(msg == MSG_FOCUS_UP)
  {
	new_focus = get_next_focus(config_active_opt.cate_idx, config_active_opt.sub_idx, msg);
	config_active_opt.sub_idx = new_focus;	
	text_list_update(p_text_list_current, list_get_valid_pos(p_text_list_current), MAX_OPT_TEXT_PER_PAGE, 0);
  }
  else if(msg == MSG_FOCUS_DOWN)
  {
	 new_focus = get_next_focus(config_active_opt.cate_idx, config_active_opt.sub_idx, msg);	 
	 config_active_opt.sub_idx = new_focus;	 
	 text_list_update(p_text_list_current, list_get_valid_pos(p_text_list_current), MAX_OPT_TEXT_PER_PAGE, 0);
  }

  subconfig_pic_list_set_focus_pos(p_piclist_cont, config_active_opt.cate_idx);
  ctrl_paint_ctrl(p_piclist_cont, TRUE);

  return SUCCESS;
}

static void refresh_net_config_ui(control_t* p_ctrl, int ori_type, int new_type, int flag)
{
	control_t *p_menu = NULL , *p_ori_contain_par = NULL, *p_ori_contain_par_pic = NULL, *p_ori_contain_par_pic_flag = NULL;
	control_t *p_new_contain_par = NULL, *p_new_contain_par_pic = NULL, *p_new_contain_par_pic_flag = NULL;
	control_t *p_ori_list_contrainer = NULL, *p_ori_list_contrainer_list = NULL, *p_new_list_contrainer= NULL, *p_new_list_contrainer_list = NULL;
    control_t *p_ori_contain;

	p_menu = ctrl_get_parent(p_ctrl);
	if(-1 == flag)
	{
		ctrl_paint_ctrl(p_menu, TRUE);
		return;
	}
	p_ori_contain = ctrl_get_child_by_id(p_menu, IDC_NETWORK_CONFIG_LIST);

	p_ori_contain_par = ctrl_get_child_by_id(p_ori_contain, IDC_NETWORK_CONFIG_LIST_1 + ori_type);
	p_ori_contain_par_pic = ctrl_get_child_by_id(p_ori_contain_par, IDC_NETWORK_CONDIF_BACK_PIC);
	p_ori_contain_par_pic_flag = ctrl_get_child_by_id(p_ori_contain_par_pic, IDC_NETWORK_CONDIF_FLAG_PIC);

	
	p_new_contain_par = ctrl_get_child_by_id(p_ori_contain, IDC_NETWORK_CONFIG_LIST_1 + new_type);
	p_new_contain_par_pic = ctrl_get_child_by_id(p_new_contain_par, IDC_NETWORK_CONDIF_BACK_PIC);
	p_new_contain_par_pic_flag = ctrl_get_child_by_id(p_new_contain_par_pic, IDC_NETWORK_CONDIF_FLAG_PIC);

	p_ori_list_contrainer = ctrl_get_child_by_id(p_ori_contain, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1+ori_type);
	p_ori_list_contrainer_list = ctrl_get_child_by_id(p_ori_list_contrainer, ori_type + IDC_NETWORK_CONFIG_TEXTLIST_1);

	p_new_list_contrainer = ctrl_get_child_by_id(p_ori_contain, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1+new_type);
	p_new_list_contrainer_list = ctrl_get_child_by_id(p_new_list_contrainer, new_type + IDC_NETWORK_CONFIG_TEXTLIST_1);


	bmap_set_content_by_id(p_ori_contain_par_pic_flag, IM_CONFIGCONNECT_1);
	bmap_set_content_by_id(p_new_contain_par_pic_flag, IM_CONFIGCONNECT_2);
	
	text_list_update(p_ori_list_contrainer_list, list_get_valid_pos(p_ori_list_contrainer_list), MAX_OPT_TEXT_PER_PAGE, 0);
	text_list_update(p_new_list_contrainer_list, list_get_valid_pos(p_new_list_contrainer_list), MAX_OPT_TEXT_PER_PAGE, 0);

	ctrl_paint_ctrl(p_menu, TRUE);
}

static RET_CODE subconfig_list_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 cate_index = config_active_opt.cate_idx;
	u8 sub_index = config_active_opt.sub_idx;
	u8	entry;
	int ret = 0;	
	net_config_t g_net_config;
	cate_subconfig_t *p_cate = &sg_mmenu_options[cate_index][sub_index];	
	sys_status_get_net_config_info(&g_net_config);	
	if(sub_index == 0)
	{
 		ret = switch_connect_type(cate_index);
		if(0 ==  ret)
		{
			refresh_net_config_ui(p_ctrl, g_net_config.link_type, cate_index, 0);
		}
		else
		{
			refresh_net_config_ui(p_ctrl, g_net_config.link_type, cate_index, -1);
		}
		return SUCCESS;
	}

	if(check_network_available(cate_index))
	{
	  entry = p_cate->link_root_id;		
	  manage_open_menu(entry, 0, 0);
	}
	return SUCCESS;
}


static RET_CODE on_ui_refresh(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_contain = NULL, *p_list_contrainer = NULL, *p_list_contrainer_list = NULL;
  net_config_t net_config;
  sys_status_get_net_config_info(&net_config);	

  p_contain = ctrl_get_child_by_id(p_cont, IDC_NETWORK_CONFIG_LIST);
  
  p_list_contrainer = ctrl_get_child_by_id(p_contain, IDC_NETWORK_CONFIG_TEXTLIST_CONTAIN_1 + net_config.link_type);
  p_list_contrainer_list = ctrl_get_child_by_id(p_list_contrainer, net_config.link_type + IDC_NETWORK_CONFIG_TEXTLIST_1);

  text_list_update(p_list_contrainer_list, list_get_valid_pos(p_list_contrainer_list), MAX_OPT_TEXT_PER_PAGE, 0);

  if((config_active_opt.cate_idx == net_config.link_type) &&
      (FALSE == check_network_available(net_config.link_type)))
  {
    config_active_opt.sub_idx = 0;
    list_set_focus_pos(p_list_contrainer_list, config_active_opt.sub_idx);
    list_select_item(p_list_contrainer_list, config_active_opt.sub_idx);
  }

  ctrl_paint_ctrl(p_list_contrainer, TRUE);

  return SUCCESS;
}

BEGIN_KEYMAP(subconfig_text_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(subconfig_text_list_keymap, NULL)

BEGIN_MSGPROC(subconfig_text_list_proc, list_class_proc)
END_MSGPROC(subconfig_text_list_proc, list_class_proc)


BEGIN_KEYMAP(subconfig_pic_list_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)    
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(subconfig_pic_list_keymap, NULL)

BEGIN_MSGPROC(subconfig_pic_list_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, subconfig_pic_list_focusmove)
    ON_COMMAND(MSG_FOCUS_RIGHT, subconfig_pic_list_focusmove)    
   ON_COMMAND(MSG_FOCUS_UP, subconfig_pic_list_focusmove)
   ON_COMMAND(MSG_FOCUS_DOWN, subconfig_pic_list_focusmove)
   ON_COMMAND(MSG_SELECT, subconfig_list_select)
END_MSGPROC(subconfig_pic_list_proc, cont_class_proc)



BEGIN_KEYMAP(subnetwork_config_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(subnetwork_config_cont_keymap, NULL)


BEGIN_MSGPROC(subnetwork_config_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_sub_networkconfig)
  ON_COMMAND(MSG_EXIT, on_exit_sub_networkconfig)
  ON_COMMAND(MSG_UI_REFRESH, on_ui_refresh)
END_MSGPROC(subnetwork_config_cont_proc, ui_comm_root_proc)





