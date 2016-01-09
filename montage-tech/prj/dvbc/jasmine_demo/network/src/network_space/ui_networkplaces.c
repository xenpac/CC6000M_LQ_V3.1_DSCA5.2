/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_networkplaces.h"
#include "lib_char.h"
#include "ui_edit_ipaddress.h"
#include "ui_keyboard_v2.h"



enum local_msg
{
  MSG_ADD_IP = MSG_LOCAL_BEGIN + 850,
  MSG_EDIT_IP,
  MSG_DELETE_IP,
};

enum favorite_ctrl_id
{
  IDC_NETWORK_LIST = 1,
  IDC_NETWORK_SBAR,
  IDC_SMALL_LIST_HELP,
};

static list_xstyle_t network_list_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
};

static list_xstyle_t network_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t network_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t network_list_attr[NETWORK_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    160, 20, 0, &network_list_field_rstyle,  &network_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    300, 180, 0, &network_list_field_rstyle,  &network_list_field_fstyle},
};

typedef struct
{
  u16 *uni_str;
  u16 max_len;
  u16   type;
  u16     add_type;
} edit_ip_path_param_t;

//static partition_t *p_partition = NULL;
//static u32 g_partition_cnt = 0;
static u16 is_add = 0;
static u32 ip_count = 0;
//static u16 ip_address[8][32] ={{0},};

comm_dlg_data_t neteork_delete_dlg_data =
{
	ROOT_ID_INVALID,
	DLG_FOR_ASK | DLG_STR_MODE_STATIC,
	COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
	IDS_MSG_ASK_FOR_DEL,
	0,
};



static u16 network_list_keymap(u16 key);

static RET_CODE network_list_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);


static RET_CODE network_list_update(control_t* p_list, u16 start, u16 size,
                                     u32 context)
{
// u8 ipaddress[128] = "";
 u16 cnt = list_get_count(p_list);
  u16 i;
  u8 asc_str[8];
  u16 ip_name[32 + 1]={0};

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {

      /* NO. */
      sprintf((char*)asc_str, "%.4d ", (u16)(start + i+1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      sys_status_get_ip_path_set((u8)(i + start), ip_name);

     // dvb_to_unicode(p_partition[i + start].name,(MAX_FILE_PATH-1), file_uniname, MAX_FILE_PATH);
   //    sprintf(ipaddress, "%d.%d.%d.%d",ip_address[i + start].s_b1,ip_address[i + start].s_b2,ip_address[i + start].s_b3,ip_address[i + start].s_b4);

       list_set_field_content_by_unistr(p_list, (u16)(start + i), 1,ip_name);
    }
  }

  return SUCCESS;
}

RET_CODE open_networkplaces(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar,*p_help;
  u8 i;
  u8 count;

  ip_count = sys_status_get_ip_path_cnt();

  count = (u8)ip_count;

#ifndef SPT_SUPPORT
  p_cont =
    ui_comm_root_create_netmenu(ROOT_ID_NETWORK_PLACES, 0, 
      IM_INDEX_SYSTEMDETAIL_BANNER, IDS_NETWORK_PLACE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_NETWORK_LIST,
    NETWORK_LIST_X, NETWORK_LIST_Y,
    NETWORK_LIST_W, NETWORK_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_NETWORK_LIST, RSI_NETWORK_LIST, RSI_NETWORK_LIST);
  ctrl_set_keymap(p_list, network_list_keymap);
  ctrl_set_proc(p_list, network_list_proc);

  ctrl_set_mrect(p_list, NETWORK_LIST_MID_L, NETWORK_LIST_MID_T,
                    NETWORK_LIST_MID_W+NETWORK_LIST_MID_L, NETWORK_LIST_MID_H+NETWORK_LIST_MID_T);
  list_set_item_interval(p_list, NETWORK_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &network_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, count, NETWORK_LIST_PAGE);
  list_set_field_count(p_list, NETWORK_LIST_FIELD, NETWORK_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list,network_list_update, 0);

  for (i = 0; i < NETWORK_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(network_list_attr[i].attr),
      (u16)(network_list_attr[i].width),
      (u16)(network_list_attr[i].left),
      (u8)(network_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, network_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, network_list_attr[i].fstyle);
  }

  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_NETWORK_SBAR,
    NETWORK_SBAR_X, NETWORK_SBAR_Y,
    NETWORK_SBAR_WIDTH, NETWORK_SBAR_HEIGHT,
    p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_NETWORK_SBAR_BG,
                  RSI_NETWORK_SBAR_BG, RSI_NETWORK_SBAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_NETWORK_SBAR_MID,
                     RSI_NETWORK_SBAR_MID, RSI_NETWORK_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, NETWORK_SBAR_VERTEX_GAP,
                    NETWORK_SBAR_WIDTH,
                    NETWORK_SBAR_HEIGHT - NETWORK_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);

  network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);

  //create help bar
    p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_SMALL_LIST_HELP,
     NETWORK_LIST_X, NETWORK_LIST_Y+NETWORK_LIST_H+20,
    NETWORK_LIST_W, 50,
                            p_cont, 0);
  ctrl_set_rstyle(p_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, 4, 4, 1);
  ctrl_set_mrect(p_help, 10, 0, NETWORK_LIST_W - 20 +10, 50);
  mbox_set_item_interval(p_help,0, 0);
  mbox_set_string_fstyle(p_help, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_help, 0, 0);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_help, 40, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);

     mbox_set_content_by_icon(p_help, 0, IM_HELP_RED, IM_HELP_RED);
  mbox_set_content_by_strid(p_help, 0, IDS_ADD);
  //mbox_set_content_by_icon(p_help, 1, IM_OK, IM_OK);
   mbox_set_content_by_icon(p_help, 1, IM_HELP_GREEN, IM_HELP_GREEN);
  mbox_set_content_by_strid(p_help, 1, IDS_EDIT);
  mbox_set_content_by_icon(p_help, 2, IM_HELP_YELLOW, IM_HELP_YELLOW);
  mbox_set_content_by_strid(p_help, 2, IDS_DELETE);
  mbox_set_content_by_icon(p_help, 3, IM_HELP_BLUE, IM_HELP_BLUE);
  mbox_set_content_by_strid(p_help, 3, IDS_CONNECT);

#else
#endif
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_network_list_yes(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 focus;

  u16 ip_name[32]={0};
  focus = list_get_focus_pos(p_ctrl);
  list_select_item(p_ctrl, focus);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  // manage_open_menu(ROOT_ID_USB_MUSIC,1 ,(u32)(& ip_address[ip_count - 1]));
  if(ip_count > 0)
  {
    sys_status_get_ip_path_set((u8)focus, ip_name);
    manage_open_menu(ROOT_ID_EDIT_USR_PWD, focus,(u32)(ip_name));
  }
  return SUCCESS;
}

static RET_CODE on_network_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 focus;

  focus = list_get_focus_pos(p_ctrl);
  if(list_get_item_status(p_ctrl, focus) == LIST_ITEM_SELECTED)
  {
   list_unselect_item(p_ctrl, focus);
  }
  return ERR_NOFEATURE;
}



static void do_edit_ip(u16 focus,u16 *name)
{
  sys_status_set_ip_path_set((u8)focus,name);
  sys_status_save();
}

/*
RET_CODE edit_ip_update(BOOL is_add, void *name)
{
  u16 focus, total;
  control_t  *p_list;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_PLACES, IDC_NETWORK_LIST);
  MT_ASSERT(p_list != NULL);

  if(is_add)
  {
    //add tp
    do_add_ip(name);

    total = ip_count;
    focus = ip_count - 1;

    list_set_count(p_list, total, NETWORK_LIST_PAGE);
    list_set_focus_pos(p_list, focus);
    network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);

  }
  else
  {
    //edit sat
    focus = list_get_focus_pos(p_list);

    do_edit_ip(focus,name);
    network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);
  }


  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  return SUCCESS;
}
*/
static void add_ip(void)
{
  edit_ip_path_param_t param;
  char *name = "\0";

  is_add = 1;
  param.uni_str = (u16*) name;
  param.max_len = 32;
//  param.cb = edit_ip_update;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.add_type = 1;

  manage_open_menu(ROOT_ID_EDIT_IP_PATH, (u32)&param, ROOT_ID_NETWORK_PLACES);
}

static void edit_ip(u16 *name)
{
 edit_ip_path_param_t param;

  is_add = 0;

  param.max_len = 32;
  param.uni_str = (u16 *)name;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.add_type = 0;

  manage_open_menu(ROOT_ID_EDIT_IP_PATH, (u32)&param, ROOT_ID_NETWORK_PLACES);
}

static void delete_ip(u16 focus,void *name)
{
  u8 i = 0;
  u16 ip_name[32]={0};
  u8 ip_account[32]={0}, ip_passwd[32]={0};

  for(i=(u8)focus;i<ip_count - 1;i++)
  {
     sys_status_get_ip_path_set(i+1, ip_name);
     sys_status_set_ip_path_set(i, ip_name);
     sys_status_get_ip_account(i+1, ip_account);
     sys_status_set_ip_account(i, ip_account);
     sys_status_get_ip_passwd(i+1, ip_passwd);
     sys_status_set_ip_passwd(i, ip_passwd);
    //uni_strncpy((u16*)(ip_address[i]), (u16*)(ip_address[i+1]), 32);
  }
   // sys_status_set_ip_path_set(ip_count - 1, 0);

//   memset((void* )(ip_address[ip_count - 1]),0, 16);
  memset(ip_name, 0, sizeof(ip_name));  
  memset(ip_account, 0, sizeof(ip_account));  
  memset(ip_passwd, 0, sizeof(ip_passwd));  
  sys_status_set_ip_path_set(i, ip_name);
  sys_status_set_ip_account(i, ip_account);
  sys_status_set_ip_passwd(i, ip_passwd);
   ip_count --;
   sys_status_set_ip_path_cnt((u8)ip_count);
   sys_status_save();
}

static RET_CODE on_network_add_ip(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  u16 focus;
  focus = list_get_focus_pos(ctrl);
  list_select_item(ctrl, focus);
  ctrl_paint_ctrl(ctrl, TRUE);

  if(ip_count > 7)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, NULL, 0);
  }
  else
  {
    add_ip();
  }
  return SUCCESS;
}

static RET_CODE on_network_edit_ip(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  u16 focus;
  u16 ip_name[32]={0};
  if(ip_count > 0)
  {
    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);
    ctrl_paint_ctrl(ctrl, TRUE);
    sys_status_get_ip_path_set((u8)focus, ip_name);
    edit_ip( (u16 *)(ip_name));
  }

  return SUCCESS;
}


static RET_CODE on_network_delete_ip(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  u16 focus;
  control_t  *p_list;
  dlg_ret_t dlg_ret;
  u16 ip_name[32]={0};


  if(ip_count > 0)
  {
    dlg_ret = ui_comm_dlg_open2(&neteork_delete_dlg_data);

    if(dlg_ret == DLG_RET_YES)
    {
      focus = list_get_focus_pos(ctrl);
      sys_status_get_ip_path_set((u8)focus, ip_name);
      delete_ip(focus, (u16 *)(ip_name));

      p_list = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_PLACES, IDC_NETWORK_LIST);
      MT_ASSERT(p_list != NULL);
      list_set_count(p_list, (u16)ip_count, NETWORK_LIST_PAGE);
      if(focus < ip_count)
      {
        list_set_focus_pos(p_list, focus);
      }
      else
      {
        list_set_focus_pos(p_list, (u16)(ip_count - 1));
      }
      network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);

      ctrl_paint_ctrl(p_list, TRUE);
    }
  }

  return SUCCESS;
}

static RET_CODE on_network_name_check(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{

  return SUCCESS;
}

static void do_add_ip(u16 *name)
{

 //uni_strncpy((u16*)(ip_address[ip_count]), name, 32);

 sys_status_set_ip_path_set((u8)ip_count,name);

// memcpy((u16*)(ip_address[ip_count]),name,32);

  ip_count ++;
  sys_status_set_ip_path_cnt((u8)ip_count);
  sys_status_save();
}

static RET_CODE on_network_name_update(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u16 *name = (u16 *)para1;
  u16 focus, total;
  control_t  *p_list;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_PLACES, IDC_NETWORK_LIST);
  MT_ASSERT(p_list != NULL);

  if(is_add)
  {
    //add tp
    do_add_ip(name);

    total = (u16)ip_count;
    focus = (u16)(ip_count - 1);

    list_set_count(p_list, total, NETWORK_LIST_PAGE);
    list_set_focus_pos(p_list, focus);
    network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);

  }
  else
  {
    //edit sat
    focus = list_get_focus_pos(p_list);

    do_edit_ip(focus,name);
    network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);
  }


  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  return SUCCESS;

}


BEGIN_KEYMAP(network_list_keymap, NULL)
   ON_EVENT(V_KEY_BLUE, MSG_YES)
   ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
   ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
   ON_EVENT(V_KEY_RED, MSG_ADD_IP)
   ON_EVENT(V_KEY_GREEN, MSG_EDIT_IP)
   ON_EVENT(V_KEY_YELLOW, MSG_DELETE_IP)
   ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(network_list_keymap, NULL)

BEGIN_MSGPROC(network_list_proc, list_class_proc)
   ON_COMMAND(MSG_FOCUS_UP, on_network_focus_change)
   ON_COMMAND(MSG_FOCUS_DOWN, on_network_focus_change)
	ON_COMMAND(MSG_YES, on_network_list_yes)
   ON_COMMAND(MSG_ADD_IP, on_network_add_ip)
   ON_COMMAND(MSG_EDIT_IP_PATH_CHECK, on_network_name_check)
   ON_COMMAND(MSG_EDIT_IP_PATH_UPDATE, on_network_name_update)
   ON_COMMAND(MSG_EDIT_IP, on_network_edit_ip)
   ON_COMMAND(MSG_DELETE_IP, on_network_delete_ip)
END_MSGPROC(network_list_proc, list_class_proc)

