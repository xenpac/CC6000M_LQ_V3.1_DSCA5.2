/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_edit_usr_pwd.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"
#include "samba_fs.h"
#include "pnp_service.h"

#define USR_PWD_MAX_NAME_LENGTH 32

enum control_id
{
  IDC_EDIT_USR = 1,
  IDC_EDIT_PWD,
  IDC_TEXT,
  IDC_EDIT_BOX,
};

static comm_help_data_t help_data = //help bar data
{
  2,                                    //select  + scroll page + exit
  2,
  {IDS_KEYBOARD,
   IDS_CONNECT},
   {IM_HELP_RED,
   IM_HELP_BLUE}
};

enum local_msg
{
  MSG_CONNECT = MSG_LOCAL_BEGIN + 550,
  MSG_KEYBOARD,  
};

rename_param_t edit_usr_param = {0};
rename_param_t edit_pwd_param = {0};
static ip_address_t ip_address = {255,255,255,255};
static u16 ip_address_with_path[32] ={0};
static u16 is_with_path = 0;
static u16 g_index = 0;
static u16 ipaddress_temp[32] ={0};
static u16 service_temp[32] ={0};
static u16 mont_url[MAX_FILE_PATH] = {0,};
static u16 *p_url = NULL;

u16 edit_usr_pwd_cont_keymap(u16 key);
u16 ui_edit_usr_keymap(u16 key);
u16 ui_edit_pwd_keymap(u16 key);

RET_CODE edit_usr_pwd_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ui_edit_usr_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ui_edit_pwd_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


RET_CODE open_edit_usr_pwd(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl[USR_PWD_ITEM_CNT] = {NULL};
  control_t *p_text = NULL, *p_edit_box = NULL;
  u16 usr_pwd_str[USR_PWD_ITEM_CNT] = {IDS_ACCOUNT,IDS_PWD};
  u16 i = 0;
  //u8 *p_char = NULL;
  u8 ip_account[32] = {0};
  u8 ip_passwd[32] = {0};
  
  p_url = mont_url;
  
  //memcpy((void *)&edit_usr_pwd_param, (void *)para2, sizeof(edit_usr_pwd_param_t));
   g_index = (u16)para1;
   is_with_path = 1; //para1;
   if(!is_with_path)
   {
    memcpy((void *)&ip_address, (void *)para2, sizeof(ip_address_t));
   }
   else
   {
    memcpy((void *)ip_address_with_path, (void *)para2, sizeof(ip_address_with_path));
   }
   

  p_cont = fw_create_mainwin(ROOT_ID_EDIT_USR_PWD,
                             EDIT_USR_PWD_CONT_X, EDIT_USR_PWD_CONT_Y,
                             EDIT_USR_PWD_CONT_W, EDIT_USR_PWD_CONT_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  sys_status_get_ip_account((u8)g_index, ip_account);
  sys_status_get_ip_passwd((u8)g_index, ip_passwd);
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_keymap(p_cont, edit_usr_pwd_cont_keymap);
  ctrl_set_proc(p_cont, edit_usr_pwd_cont_proc);
  //create usr and pwd edit box
  for(i = 0;i < USR_PWD_ITEM_CNT;i++)
  {
    switch(i)
      {
      case 0:
        p_ctrl[i] = ui_comm_t9edit_create(p_cont, (u8)(IDC_EDIT_USR+i),
                                    EDIT_USR_PWD_EBOX_X,
                                    (u16)(EDIT_USR_PWD_EBOX_Y+i*(EDIT_USR_PWD_EBOX_H+EDIT_USR_PWD_ITEM_VGAP)),
                                    EDIT_USR_PWD_EBOX_LW, EDIT_USR_PWD_EBOX_RW,
                                    ROOT_ID_EDIT_USR_PWD);
        ui_comm_t9edit_set_static_txt(p_ctrl[i], usr_pwd_str[i]);
        ui_comm_ctrl_set_keymap(p_ctrl[i], ui_edit_usr_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], ui_edit_usr_proc);
		    ui_comm_t9edit_set_content_by_ascstr(p_ctrl[i], ip_account);
        edit_usr_param.max_len = USR_PWD_MAX_NAME_LENGTH;
        edit_usr_param.type = KB_INPUT_TYPE_SENTENCE;
        edit_usr_param.uni_str = ui_comm_t9edit_get_content(p_ctrl[i]);
        break;
        case 1:

        p_ctrl[i] = ctrl_create_ctrl(CTRL_CONT, IDC_EDIT_USR+i,
                                    EDIT_USR_PWD_EBOX_X,
                                    (u16)(EDIT_USR_PWD_EBOX_Y+i*(EDIT_USR_PWD_EBOX_H+EDIT_USR_PWD_ITEM_VGAP)),
                                    EDIT_USR_PWD_EBOX_LW+EDIT_USR_PWD_EBOX_RW,
                                    COMM_CTRL_H,p_cont,0);

        ctrl_set_style (p_ctrl[i], STL_EX_WHOLE_HL);
        ctrl_set_rstyle(p_ctrl[i],RSI_ITEM_1_SH,RSI_ITEM_1_HL,RSI_ITEM_3_SH);
        //left text part
        p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_TEXT,0, 0, EDIT_USR_PWD_EBOX_LW, COMM_CTRL_H,p_ctrl[i], 0);
        ctrl_set_rstyle(p_text,RSI_PBACK,RSI_PBACK,RSI_PBACK);
        text_set_content_type(p_text, TEXT_STRTYPE_STRID);
        text_set_font_style(p_text,FSI_COMM_TXT_SH,FSI_COMM_TXT_HL,FSI_COMM_TXT_GRAY);
        text_set_align_type(p_text, STL_LEFT | STL_VCENTER);
        text_set_offset(p_text, COMM_CTRL_OX, 0);
        text_set_content_by_strid(p_text,usr_pwd_str[i]);
        //right ebox part
        p_edit_box = ctrl_create_ctrl(CTRL_EBOX, IDC_EDIT_BOX,EDIT_USR_PWD_EBOX_LW , 0, EDIT_USR_PWD_EBOX_RW - 12, COMM_CTRL_H,p_ctrl[i], 0);
        ctrl_set_rstyle(p_edit_box, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        ctrl_set_keymap(p_edit_box, ui_edit_pwd_keymap);
        ctrl_set_proc(p_edit_box, ui_edit_pwd_proc);
        ebox_set_align_type(p_edit_box, STL_CENTER | STL_VCENTER);
        ebox_set_font_style(p_edit_box,FSI_WHITE,FSI_BLACK,FSI_BLACK);
   //     ebox_set_hide_mask(p_edit_box,  '-', '*');
        ctrl_set_active_ctrl(p_ctrl[i], p_edit_box);
        ebox_set_content_by_ascstr(p_edit_box, (char*)ip_passwd);

        ebox_set_worktype(p_edit_box, EBOX_WORKTYPE_HIDE);

        edit_pwd_param.max_len = USR_PWD_MAX_NAME_LENGTH;
        edit_pwd_param.type = KB_INPUT_TYPE_SENTENCE;
        edit_pwd_param.uni_str = ebox_get_content(p_edit_box);
        break;
      default:
        break;
      }

  ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + USR_PWD_ITEM_CNT)
                             % USR_PWD_ITEM_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % USR_PWD_ITEM_CNT + 1));    /* down */
 }
  // add help bar
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);


  /* set focus according to current pg */

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_edit_cancel(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;
  manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
  if((p_root = fw_find_root_by_id(ROOT_ID_NETWORK_PLACES))!= NULL)
  {
    ctrl_paint_ctrl(p_root,TRUE);
  }
    
  return SUCCESS;
}

  BOOL  ui_get_ip_service_with_path(char *ipaddress,char *service)
  {

    u16 ip_address_with_path_temp[32] ={0};
    u16 count = 0;

    memcpy(ip_address_with_path_temp,ip_address_with_path,64);
      count = 2;
      while(count < 32)
      {
        if((ip_address_with_path_temp[count] != '\\') )
        {
        count ++;
        }
        else
        {
          ip_address_with_path_temp[count] ='\0';
          break;
        }
      }
      if(count !=32)
      {
      memcpy(service_temp,&ip_address_with_path_temp[count+1],(32 - count - 1)*2);
      memcpy(ipaddress_temp,&ip_address_with_path_temp[2],(count-1) *2);
      str_uni2asc((u8*) ipaddress, ipaddress_temp);
       str_uni2asc((u8*) service, service_temp);
       return TRUE;
     }
     else
     {
       ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 0);
       return FALSE;
     }

    //  OS_PRINTF("lou 3333333333  ipaddress ==%s\n ",ipaddress);
    //  OS_PRINTF("lou 444444444444  service ==%s\n ",service);
   }

static RET_CODE on_connect_ip(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  char ipaddress[128] = {0};
  control_t *p_ebox;
  control_t *p_ctrl;
  u16 *usr_account,*usr_passwd;
  char *username = mtos_malloc(sizeof(u16)*128);
  char *password = mtos_malloc(sizeof(char)*128);
  char service[64] = {0,};
  int ret = ERR_FAILURE;
  BOOL is_ip = FALSE;
  char mount_path[128]={0};
   
  p_ebox = ctrl_get_child_by_id(p_cont, IDC_EDIT_USR);
  usr_account =  ui_comm_t9edit_get_content(p_ebox);
  str_uni2asc((u8*) username, usr_account);

  p_ctrl = ctrl_get_child_by_id(ctrl_get_child_by_id(p_cont, IDC_EDIT_PWD),IDC_EDIT_BOX);
  usr_passwd = ebox_get_content(p_ctrl);
  str_uni2asc((u8*) password, usr_passwd);

  if(!is_with_path)
  {
    sprintf(ipaddress, "%d.%d.%d.%d",ip_address.s_b1,ip_address.s_b2,ip_address.s_b3,ip_address.s_b4);
    
    #ifndef WIN32
    ret = mount_samba_fs(ipaddress,service,username, password);
    #endif
    
    if(SUCCESS == ret)
    {
      OS_PRINTF("lou it  connect ip success\n");
      manage_open_menu(ROOT_ID_USB_MUSIC,1 ,(u32)(ipaddress));
    }
    else
    {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 0);
    }
  }
  else
  {
    is_ip =  ui_get_ip_service_with_path(ipaddress,service);

    if(is_ip == TRUE)  
    {
      #ifndef WIN32
      sprintf(mount_path, "smb://%s:%s@%s/%s", username, password, ipaddress, service);

      str_asc2uni((u8*) mount_path, p_url);
      
      pnp_service_vfs_mount(p_url);
      #endif
    }
  }

 mtos_free(password);
  mtos_free(username);
  return SUCCESS;
}


//void edit_usr_box_update(void)
RET_CODE edit_usr_box_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
 u8 test[32];
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_EDIT_USR_PWD, IDC_EDIT_USR);
  ctrl_paint_ctrl(p_edit_cont, TRUE); 
  str_uni2asc(test, p_unistr);
  sys_status_set_ip_account((u8)g_index, test);
  return SUCCESS;
}

static RET_CODE on_edit_usr_box(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;
  control_t *p_ebox;

  p_ebox = ctrl_get_parent(p_ctrl);
  param.uni_str = ui_comm_t9edit_get_content(p_ebox);
  param.type = edit_usr_param.type;
  param.max_len = USR_PWD_MAX_NAME_LENGTH;
  param.cb = edit_usr_box_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

//void edit_pwd_box_update(void)
RET_CODE edit_pwd_box_update(u16 *p_unistr)
{
  control_t *p_edit_cont;
  u8 test[32];
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_EDIT_USR_PWD, IDC_EDIT_PWD);
  ctrl_paint_ctrl(p_edit_cont, TRUE);
  str_uni2asc(test, p_unistr);
  sys_status_set_ip_passwd((u8)g_index, test);
  return SUCCESS;
}

static RET_CODE on_edit_pwd_box(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = edit_pwd_param.type;
  param.max_len = USR_PWD_MAX_NAME_LENGTH;
  param.cb = edit_pwd_box_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}

RET_CODE ui_get_ip(char *ipaddr)
{
  char ipaddress[128] = {0};
  char service[128] = {0};
  
  ui_get_ip_service_with_path(ipaddress, service);
  
  strcpy(ipaddr, ipaddress);
  
  return SUCCESS;
}

RET_CODE ui_get_ip_path_mount(u16 **pp_ip_path)
{
  *pp_ip_path = p_url;
  return SUCCESS;
}

RET_CODE ui_get_ip_path(u16 *ip_path)
{
  memcpy(ip_path,ip_address_with_path,sizeof(ip_address_with_path));
  return SUCCESS;
}

BEGIN_KEYMAP(edit_usr_pwd_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_BLUE, MSG_CONNECT)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(edit_usr_pwd_cont_keymap, NULL)

BEGIN_MSGPROC(edit_usr_pwd_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CANCEL, on_edit_cancel)
  ON_COMMAND(MSG_CONNECT, on_connect_ip)
END_MSGPROC(edit_usr_pwd_cont_proc, cont_class_proc)

BEGIN_KEYMAP(ui_edit_usr_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_RED,MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ui_edit_usr_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(ui_edit_usr_proc, ui_comm_t9_proc)
  ON_COMMAND(MSG_SELECT, on_edit_usr_box)
  ON_COMMAND(MSG_NUMBER, on_edit_usr_box)
END_MSGPROC(ui_edit_usr_proc, ui_comm_t9_proc)

BEGIN_KEYMAP(ui_edit_pwd_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_RED,MSG_SELECT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ui_edit_pwd_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(ui_edit_pwd_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_pwd_box)
  ON_COMMAND(MSG_NUMBER, on_edit_pwd_box)
END_MSGPROC(ui_edit_pwd_proc, ui_comm_edit_proc)

