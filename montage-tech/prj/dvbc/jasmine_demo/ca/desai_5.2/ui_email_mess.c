/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#include "ui_email_mess.h"
#include "ui_ca_public.h"



static cas_mail_headers_t *p_mail_data_buf = NULL;
static u8 g_del_mail = 0; 
enum control_id
{
  IDC_INVALID = 0,
  IDC_EMAIL_ITEMS,
  IDC_EMAIL_PRO,
  IDC_EMAIL_BAR,
  IDC_EMAIL_NOTIC,
  IDC_EMAIL_BODY_CONT,
  IDC_EMAIL_TEXT_BAR,
  IDC_EMAIL_TEXT,
  IDC_EMAIL_RECEIVED_HEAD,
  IDC_EMAIL_RECEIVED,
  IDC_EMAIL_RESET_HEAD,
  IDC_EMAIL_RESET,
};

enum nvod_email_mess_local_msg
{
  MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
  MSG_DELETE_ALL,
};

enum read_email_id
{
  READY_TO_READ_MAIL = 0,
};

#ifdef WIN32
static cas_mail_headers_t test_cas_mail_head = 
{
      {
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
        {0, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x11, 0x58}, {0}, 4, 5, 6},
        {1, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x12, 0x58}, {0}, 4, 5, 6},
        {2, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x13, 0x58}, {0}, 4, 5, 6},
        {3, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x14, 0x58}, {0}, 4, 5, 6},
        {4, 1, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0},  {20, 12, 7, 12, 0x15, 0x58}, {0}, 4, 5, 6},
        {5, 0, 3, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4}, {0xD3, 0xCA, 0xBC, 0xFE, 0xB2, 0xE2, 0xCA, 0xD4},  {20, 12, 7, 12, 0x16, 0x58}, {0}, 4, 5, 6},
      },
      18,
      9,
      18,
      8,
};
#endif

u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

comm_help_data_t2 email_help_data2 = //help bar data
{
  8, 20, {40, 160,40, 160,40,160,40,160},
  {
    HELP_RSC_BMP   | IM_UPDOWN,
    HELP_RSC_STRID | IDS_MOVE,
    HELP_RSC_BMP   | IM_OK,
    HELP_RSC_STRID | IDS_SELECT,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
    HELP_RSC_STRID | IDS_DELETE,
    HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
    HELP_RSC_STRID | IDS_DELETE_ALL,
  },
};

RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len)
{
  u32 i = 0;
  u32 ascStrLen = 0;
  u32 tmp_value = 0;
  
  if((NULL == p_ascstr) || (NULL == p_dec_number) || (max_len > 8))
  {
      return ERR_FAILURE;
  }
  
  ascStrLen = strlen((char *)p_ascstr);
  *p_dec_number = 0;

  if((0 == ascStrLen) || (ascStrLen > max_len))
  {
      return ERR_FAILURE;
  }
  
  for(i = 0; i < ascStrLen; i++)
  {
    if((p_ascstr[i] >= '0') && (p_ascstr[i] <= '9'))
    {
        tmp_value = p_ascstr[i] - '0';
        *p_dec_number = (*p_dec_number) * 10 + tmp_value;
    }
    else
    {
        break;
    }
  }

  return SUCCESS;
}

static RET_CODE email_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
  u16 i, cnt = list_get_count(ctrl);
  u8 asc_str[32];
  u16 uni_str[64];
  control_t *p_ctrl= NULL;
  //utc_time_t  s_ca_time = {0};
  //utc_time_t  s_local_time = {0};
  //u32 tmp_value = 0;
  
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%d ", i + start + 1);   //p_mail_data_buf->p_mail_head[i + start].m_id);
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

      /* NAME */
      gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].subject, 80, 
                      uni_str, 41);
      list_set_field_content_by_unistr(ctrl, (u16)(start + i), 1, (u16 *)uni_str);

      /* Is new */
      if(p_mail_data_buf->p_mail_head[i + start].new_email)
      {
          list_set_field_content_by_strid(ctrl, (u16)(start + i), 2, IDS_YES);
      }
      else
      {
          list_set_field_content_by_strid(ctrl, (u16)(start + i), 2, IDS_NO);
      }
      
      //date
      #if 0
      s_ca_time.year = p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 100 
                                  + p_mail_data_buf->p_mail_head[i + start].creat_date[1];
      s_ca_time.month =  p_mail_data_buf->p_mail_head[i + start].creat_date[2];
      s_ca_time.day = p_mail_data_buf->p_mail_head[i + start].creat_date[3];
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[4]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.hour = (u8)tmp_value;
      sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[5]);
      asc_to_dec(asc_str, &tmp_value, 2);
      s_ca_time.minute = (u8)tmp_value;

      time_to_local(&s_ca_time, &s_local_time);
      sprintf((char *)asc_str,"%2d/%2d/%2d %2d:%2d", 
                s_local_time.year, s_local_time.month, 
                s_local_time.day, s_local_time.hour, s_local_time.minute);
      str_asc2uni(asc_str, uni_str);
      #endif
      #if 1
      sprintf((char *)asc_str, "%04d/%02d/%02d %02d:%02d:%02d",
              p_mail_data_buf->p_mail_head[i + start].creat_date[0] * 256 
                + p_mail_data_buf->p_mail_head[i + start].creat_date[1],
              p_mail_data_buf->p_mail_head[i + start].creat_date[2],
              p_mail_data_buf->p_mail_head[i + start].creat_date[3],
              p_mail_data_buf->p_mail_head[i + start].creat_date[4],
              p_mail_data_buf->p_mail_head[i + start].creat_date[5],
              p_mail_data_buf->p_mail_head[i + start].creat_date[6]);
      #endif
      list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);
    }
  }
  if((0 != p_mail_data_buf->max_num) && (0xFFFF != p_mail_data_buf->max_num))
  {
    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RECEIVED);
    sprintf((char*)asc_str,"%d",p_mail_data_buf->max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
    p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RESET);
    sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB - p_mail_data_buf->max_num);
    text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
  }
  return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  u16 size = EMAIL_LIST_PAGE;
  p_mail_data_buf = (cas_mail_headers_t*)para2;
  #ifdef WIN32
  p_mail_data_buf = &test_cas_mail_head;
  #endif

  list_set_count(p_ctrl, p_mail_data_buf->max_num, EMAIL_LIST_PAGE);  
  list_set_focus_pos(p_ctrl, 0);
  email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);

  ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
 return SUCCESS;

}
#if 0
static RET_CODE on_ca_mail_preread(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 ret = para1;
  cas_mail_body_t *mail_body = NULL;
  u16 index = 0;
  u16 i;
  index = list_get_focus_pos(p_ctrl);

  mail_body = (cas_mail_body_t *)para2;
  OS_PRINTF("MMMMMMMMMMMp_mail_data_buf->total_email_num = %d, index = %d\n",p_mail_data_buf->total_email_num,index);
  
  OS_PRINTF("total_email_num %d\n",mail_body->data_len);
  OS_PRINTF("mail boy data start!!!!!!\n");
  for(i = 0;i < mail_body->data_len;i ++)
  {
    OS_PRINTF("%s",mail_body->data[i]);
    if(i % 16 == 0)
      OS_PRINTF("\n");
  }
  switch(ret)
  {
	case READY_TO_READ_MAIL:
    if((0 != p_mail_data_buf->max_num) && (0xFFFF != p_mail_data_buf->max_num))
      manage_open_menu(ROOT_ID_EMAIL_MESS_CONTENT, (u32)&p_mail_data_buf->p_mail_head[index],(u32)mail_body);
    break;
  	case CAS_E_MAIL_CAN_NOT_READ://CAS_E_MAIL_CAN_NOT_READ
  	  ui_comm_cfmdlg_open(NULL,IDS_CA,NULL,0);//IDS_CARD_EMAIL_ERR
     OS_PRINTF("ret  = CAS_E_MAIL_CAN_NOT_READ\n");
  	  break;
  	default:
      ui_comm_cfmdlg_open(NULL,IDS_CA2, NULL,0);//IDS_RECEIVED_DATA_FAIL
  	  break;
  }
	return SUCCESS;
}
#endif

RET_CODE open_email_mess(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_mbox = NULL;
  control_t *p_sbar = NULL;
  control_t *p_list = NULL;
  control_t *p_ctrl = NULL;
  u16 i = 0;
  u8 asc_str[8];
  
  u16 itemmenu_btn_str[EMAIL_ITME_COUNT] =
  {
    IDS_CA_NUMBER, IDS_CA_TITLE, IDS_CA_NEW_NOT, IDS_DATE,  
  };
  list_xstyle_t email_item_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_ITEM_1_HL,
    RSI_ITEM_1_SH,
    RSI_ITEM_1_HL,
  };
  
  list_xstyle_t emial_plist_field_fstyle =
  {
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
  };
  list_xstyle_t email_plist_field_rstyle =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
  };

  list_field_attr_t email_plist_attr[EMAIL_LIST_FIELD] =
  {
    { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
        40, 10, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

    { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
        390, 50, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

    { LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
        60, 440, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

    { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER ,
        370, 500, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},
  };
  
  p_cont = ui_comm_root_create(ROOT_ID_EMAIL_MESS,0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,IM_TITLEICON_TV,
                               IDS_CA_EMAIL);

  //mbox item title
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_EMAIL_ITEMS,
                            EMAIL_MBOX_X, EMAIL_MBOX_Y, 
                            EMAIL_MBOX_W, EMAIL_MBOX_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_ITEM_1_HL,RSI_ITEM_1_HL, RSI_ITEM_1_HL);
  ctrl_set_attr(p_mbox, OBJ_ATTR_ACTIVE);
  mbox_set_focus(p_mbox, 1);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);
  mbox_set_count(p_mbox, EMAIL_ITME_COUNT, EMAIL_ITME_COUNT, 1);
  //mbox_set_item_rect(p_mbox, CA_MBOX_MID_X, 0, CA_MBOX_MID_W, CA_MBOX_MID_H, 0, 0);
  mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  mbox_set_string_offset(p_mbox, EMAIL_ITEM_OFFSET, 0);
  mbox_set_string_align_type(p_mbox, STL_LEFT| STL_VCENTER);
  for (i = 0; i < EMAIL_ITME_COUNT; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, itemmenu_btn_str[i]);
  }
  
  //email list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_EMAIL_PRO,
                           EMAIL_LIST_X, EMAIL_LIST_Y, EMAIL_LIST_W,EMAIL_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
  ctrl_set_keymap(p_list, email_plist_list_keymap);
  ctrl_set_proc(p_list, email_plist_list_proc);

  ctrl_set_mrect(p_list, EMAIL_LIST_MIDL, EMAIL_LIST_MIDT,
           EMAIL_LIST_MIDL + EMAIL_LIST_MIDW, EMAIL_LIST_MIDT + EMAIL_LIST_MIDH);
  list_set_item_rstyle(p_list, &email_item_rstyle);
  list_set_count(p_list, 0, EMAIL_LIST_PAGE);

  list_set_field_count(p_list, EMAIL_LIST_FIELD, EMAIL_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, email_plist_update, 0);
 
  for (i = 0; i < EMAIL_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(email_plist_attr[i].attr), (u16)(email_plist_attr[i].width),
                        (u16)(email_plist_attr[i].left), (u8)(email_plist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, email_plist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, email_plist_attr[i].fstyle);
  }
  
  //list scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EMAIL_BAR, 
                              EMAIL_SBAR_X, EMAIL_SBAR_Y, EMAIL_SBAR_W, EMAIL_SBAR_H, 
                              p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
  ctrl_set_mrect(p_sbar, 0, 0, EMAIL_SBAR_W,  EMAIL_SBAR_H);
  list_set_scrollbar(p_list, p_sbar);
  //email_plist_update(p_list, list_get_valid_pos(p_list), EMAIL_LIST_PAGE, 0);
  
  //received head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RECEIVED_HEAD,
                           EMAIL_RECEIVED_HEAD_X, EMAIL_RECEIVED_HEAD_Y,
                           EMAIL_RECEIVED_HEAD_W,EMAIL_RECEIVED_HEAD_H,
                           p_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_BLACK, RSI_BLACK, RSI_BLACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_RECEIVED);

  //received
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RECEIVED,
                           EMAIL_RECEIVED_X, EMAIL_RECEIVED_Y,
                           EMAIL_RECEIVED_W,EMAIL_RECEIVED_H,
                           p_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_BLACK, RSI_BLACK, RSI_BLACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_ctrl, (u8*)"0");

  //rest space head
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RESET_HEAD,
                           EMAIL_RESET_HEAD_X, EMAIL_RESET_HEAD_Y,
                           EMAIL_RESET_HEAD_W, EMAIL_RESET_HEAD_H,
                           p_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_BLACK, RSI_BLACK, RSI_BLACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_REST_SPACE);

  //rest space
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RESET,
                           EMAIL_RESET_X, EMAIL_RESET_Y,
                           EMAIL_RESET_W, EMAIL_RESET_H,
                           p_cont, 0);
  //ctrl_set_rstyle(p_ctrl, RSI_BLACK, RSI_BLACK, RSI_BLACK);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB);
  text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

  ui_comm_help_create2(&email_help_data2, p_cont, 0);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);
  #ifdef WIN32 
  plist_update_email(p_list, 0, 0, 0);
  #else
  ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  #endif
  return SUCCESS;
}

static RET_CODE on_email_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 old_focus, new_focus;
  old_focus = list_get_focus_pos(p_list);
  //ret = list_class_proc(p_list, msg, para1, para2);
  list_class_proc(p_list, msg, para1, para2);
  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("m_id %d\n",p_mail_data_buf->p_mail_head[new_focus].m_id);
  OS_PRINTF("sender %d\n",p_mail_data_buf->p_mail_head[new_focus].sender);
  OS_PRINTF("subject %d\n",p_mail_data_buf->p_mail_head[new_focus].subject);
  
  OS_PRINTF("on_email_list_change_focus : old focus : %d,  new focus : %d\n",old_focus, new_focus);
  return SUCCESS;
}

//extern u16 get_message_strid();
static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
  //RET_CODE ret = SUCCESS;
  u16 new_focus;
  control_t *p_ctrl = NULL;
  p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);

  new_focus = list_get_focus_pos(p_list);
  OS_PRINTF("--------on_email_del_mail   :focus :%d\n",new_focus);
  switch(msg)
  {
      case MSG_DELETE_ONE:
        ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[new_focus].m_id,0);  
        break;
      case MSG_DELETE_ALL:
        ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ALL, 0 ,0);  
        break;
      default:
        return ERR_FAILURE;
  }
  if(1)//if((get_message_strid() == IDS_CA) && (NULL != p_ctrl))//IDS_CA_EMAIL_NO_SPACE//_____need modify IDS________
  {
    ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
  }
  ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  return SUCCESS;
}

void on_select_del(void)
{
  g_del_mail = 1;
}

void on_cancel_select_del(void)
{
  g_del_mail = 0;
}

static RET_CODE on_dlg_email_del_one_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(p_mail_data_buf->max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_CURRENT_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    
  if(g_del_mail == 1)
    on_email_del_mail(p_list,msg,para1,para2);
  }
  return SUCCESS;
}

static RET_CODE on_dlg_email_del_all_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(p_mail_data_buf->max_num)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_ALL_MAIL, 
                                 on_select_del, on_cancel_select_del, 0);
    if(g_del_mail == 1)
      on_email_del_mail(p_list,msg,para1,para2);
  }
  return SUCCESS;
}

static RET_CODE on_email_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u32 ret = READY_TO_READ_MAIL;
  //cas_mail_body_t *mail_body = NULL;
  u16 index = 0;
  prompt_type_t prompt_type;
  index = list_get_focus_pos(p_list);

  prompt_type.index = index;
  prompt_type.message_type = 0;
  
  OS_PRINTF("@@@@p_mail_data_buf.total_email_num = %d, index = %d\n",p_mail_data_buf->total_email_num,index);
  
  switch(ret)
  {
	case READY_TO_READ_MAIL:
    if((0 != p_mail_data_buf->max_num) && (0xFFFF != p_mail_data_buf->max_num))
    {
      #ifndef WIN32
      manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)p_mail_data_buf);
      #else
      manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)&test_cas_mail_head);
      #endif
    }
    break;
  	case CAS_E_MAIL_CAN_NOT_READ://CAS_E_MAIL_CAN_NOT_READ
  	  ui_comm_cfmdlg_open(NULL,IDS_CA,NULL,0);//IDS_CARD_EMAIL_ERR
     OS_PRINTF("ret  = CAS_E_MAIL_CAN_NOT_READ\n");
  	  break;
  	default:
      ui_comm_cfmdlg_open(NULL,IDS_CA2, NULL,0);//IDS_RECEIVED_DATA_FAIL
  	  break;
  }
	return SUCCESS;
}

static RET_CODE on_email_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
   fw_destroy_mainwin_by_id(ROOT_ID_EMAIL_MESS);
   ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
   return SUCCESS;
}

static RET_CODE on_email_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

  return SUCCESS;
}

BEGIN_KEYMAP(email_plist_list_keymap, NULL)
  ON_EVENT(V_KEY_F1, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_RED, MSG_DELETE_ONE)
  ON_EVENT(V_KEY_F2, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_YELLOW, MSG_DELETE_ALL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MAIL, MSG_EXIT_ALL)
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_email_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_email_list_change_focus)
  ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
  //ON_COMMAND(MSG_CA_MAIL_BODY_INFO, on_ca_mail_preread)
  ON_COMMAND(MSG_SELECT, on_email_list_select)
  ON_COMMAND(MSG_DELETE_ONE, on_dlg_email_del_one_mail)
  ON_COMMAND(MSG_DELETE_ALL, on_dlg_email_del_all_mail)
  ON_COMMAND(MSG_EXIT, on_email_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_email_exit_all)
END_MSGPROC(email_plist_list_proc, list_class_proc)

