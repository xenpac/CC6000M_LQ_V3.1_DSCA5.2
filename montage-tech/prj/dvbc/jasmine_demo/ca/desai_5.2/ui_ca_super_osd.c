/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_ca_super_osd.h"

#include "ui_ca_public.h"
//lint -e19
#include "math.h"
//lint +e19
#include "cas_ware.h"

extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
extern rsc_fstyle_t f_tab[FSTYLE_CNT];
enum control_id
{

  IDC_INVALID = 0,
  IDC_CA_CONT,
  IDC_CONTENT,
};
//static BOOL g_is_super_osd = FALSE;

u16 ca_super_osd_keymap(u16 key);
RET_CODE ca_super_osd_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE on_ca_super_osd_exit_updown(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  u16 prog_id;
  manage_close_menu(ROOT_ID_CA_SUPER_OSD, 0, 0);
  OS_PRINTF("on_ca_ipp_exit_updown\n");
  if(para1 == V_KEY_UP)
  {
    ui_shift_prog(1, TRUE, &prog_id);
    ui_play_prog(prog_id,FALSE);
  }
  else
  {
    ui_shift_prog(-1, TRUE, &prog_id);
    ui_play_prog(prog_id,FALSE);
  }
  
  return SUCCESS;
}

RET_CODE on_ca_super_osd_exit(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  fw_tmr_destroy(ROOT_ID_CA_SUPER_OSD, MSG_CANCEL);
  fw_destroy_mainwin_by_id(ROOT_ID_CA_SUPER_OSD);
  
  return SUCCESS;
}

static RET_CODE on_ca_super_osd_exit_all(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_close_all_mennus();
  
  return SUCCESS;
}
void close_super_osd(void)
{
  //g_is_super_osd = FALSE;
  fw_destroy_mainwin_by_id(ROOT_ID_CA_SUPER_OSD);
}
RET_CODE open_super_osd(u32 para1, u32 para2)
{
  control_t *p_cont, *p_mask, *p_txt;
  msg_info_t *osd_msg_info = (msg_info_t *)para2;
  u8 background_ratio = 0;
  //u8 display_front_size = 0;
  u8 asc_str[2048] = {0};
  u16 uni_str[2048] = {0};
  u16 width = 0;
  u16 height = 0;
  u16 txt_width = 0;
  u16 txt_height = 0;
  u16 x = 0;
  u16 y = 0;  
  u32 color = 0;
  
  if(osd_msg_info == NULL)
  {
    return ERR_FAILURE;
  }

  {
    extern void on_ca_roll_osd_hide(void);
    on_ca_roll_osd_hide();
 }
  if(NULL != fw_find_root_by_id(ROOT_ID_CA_SUPER_OSD))
  {
      close_super_osd();
  }
  //g_is_super_osd = TRUE;
  //set background color
  //color.r = osd_msg_info->display_back_color[3] & 0xff;    //a
  //color.g = osd_msg_info->display_back_color[2] & 0xff;   //r
  //color.b = osd_msg_info->display_back_color[1] & 0xff;   //g
  //color.a = osd_msg_info->display_back_color[0] & 0xff;   //b
  //gdi_set_palette(C_OSD_BACKGROUND, 1, &color);

  color = ((u32)(osd_msg_info->display_back_color[3]) <<24) + ((u32)(osd_msg_info->display_back_color[2]) << 16)
              + ((u32)(osd_msg_info->display_back_color[1]) << 8) + (osd_msg_info->display_back_color[0]);
   
  c_tab[RSI_OSD].bg.value = color;
   
  //set font color
  //color.r = osd_msg_info->display_color[3] & 0xff;    //a
  //color.g = osd_msg_info->display_color[2] & 0xff;   //r
  //color.b = osd_msg_info->display_color[1] & 0xff;   //g
  //color.a = osd_msg_info->display_color[0] & 0xff;   //b
  //gdi_set_palette(C_OSD_FONT, 1, &color);

  color = ((u32)(osd_msg_info->display_color[3]) <<24) + (u32)((osd_msg_info->display_color[2]) << 16)
              + ((u32)(osd_msg_info->display_color[1]) << 8) + (osd_msg_info->display_color[0]);
  f_tab[FSI_OSD1].color = color;
  
  
  if(osd_msg_info)
  {
    background_ratio = osd_msg_info->screen_ratio;
  }
  else
  {
     background_ratio = 80;
  }
  
  UI_PRINTF("@@@@@ super osd background_ratio:%d, display_front_size:%d\n", background_ratio, osd_msg_info->display_front_size);
  
  width = (u16)(sqrt((background_ratio * 1.0)/100) * SCREEN_WIDTH);
  height = (u16)(sqrt((background_ratio * 1.0)/100) * SCREEN_HEIGHT);
  x = (SCREEN_WIDTH - width)/2;
  y = (SCREEN_HEIGHT - height)/2;  
  
  p_mask = fw_create_mainwin(ROOT_ID_CA_SUPER_OSD,
                             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                             0, 0, OBJ_ATTR_ACTIVE, 0);
  
  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);



  
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_CA_CONT,
                            x, y, width, height, p_mask, 0);
  
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_OSD, RSI_OSD, RSI_OSD);
  //ctrl_set_keymap(p_cont, ca_super_osd_keymap);
  //ctrl_set_proc(p_cont, ca_super_osd_proc);

  if(width >= SCREEN_WIDTH -40)
  {
    txt_width = SCREEN_WIDTH -40;
  }
  else
  {
    txt_width = width;
  }

  if(height >= SCREEN_HEIGHT -40)
  {
    txt_height= SCREEN_HEIGHT -40;
  }
  else
  {
    txt_height = height;
  }
  
  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           (width - txt_width)/2, (height - txt_height)/2,
                           txt_width,txt_height,
                           p_cont, 0);

  ctrl_set_rstyle(p_txt, RSI_OSD, RSI_OSD, RSI_OSD);
  text_set_align_type(p_txt, STL_VCENTER | STL_CENTER);

  switch(osd_msg_info->display_front_size)
  {
    case 1:
		  f_tab[FSI_OSD1].width = 14;//18;
		  f_tab[FSI_OSD1].height = 14;//18;
      break;
    case 2:
		  f_tab[FSI_OSD1].width = 16;//22;
		  f_tab[FSI_OSD1].height = 16;//22;
      break;
    case 3:
		  f_tab[FSI_OSD1].width = 18;//26;
		  f_tab[FSI_OSD1].height = 18;//26;
      break;
    case 4:
		  f_tab[FSI_OSD1].width = 22;//30;
		  f_tab[FSI_OSD1].height = 22;//30;
      break;
    case 5:
		  f_tab[FSI_OSD1].width = 26;//34;
		  f_tab[FSI_OSD1].height =26;//34;
      break;
    case 6:
		  f_tab[FSI_OSD1].width = 30;//38;
		  f_tab[FSI_OSD1].height = 30;//38;
      break;
    case 7:
		  f_tab[FSI_OSD1].width = 34;//42;
		  f_tab[FSI_OSD1].height = 34;//42;
      break;
    case 8:
		  f_tab[FSI_OSD1].width = 38;//46;
		  f_tab[FSI_OSD1].height = 38;//46;
      break;
    case 9:
		  f_tab[FSI_OSD1].width = 42;//50;
		  f_tab[FSI_OSD1].height = 42;//50;
      break;
    case 10:
		  f_tab[FSI_OSD1].width = 46;//60;
		  f_tab[FSI_OSD1].height = 46;//60;
      break;
    case 11:
		  f_tab[FSI_OSD1].width = 50;//70;
		  f_tab[FSI_OSD1].height =50;// 70;
        break;
    case 12:
		  f_tab[FSI_OSD1].width =54;//80;
		  f_tab[FSI_OSD1].height = 54;//80;
      break;
    default:
		  f_tab[FSI_OSD1].width = 24;
		  f_tab[FSI_OSD1].height = 24;
      break;
  }
  
  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  text_set_font_style(p_txt, FSI_OSD1, FSI_OSD1, FSI_OSD1);
  memcpy((void *)asc_str, (void *)osd_msg_info->data, strlen((char *)osd_msg_info->data));
  gb2312_to_unicode(asc_str,
                               2047, uni_str, 2047);
  text_set_content_by_unistr(p_txt, uni_str);
  //text_set_content_by_strid(p_txt, IDS_CARD_BASE_INFO_HELP);
  ctrl_default_proc(p_txt, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_parent(p_cont), FALSE);
  return SUCCESS;
}




BEGIN_KEYMAP(ca_super_osd_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(ca_super_osd_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_super_osd_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_ca_super_osd_exit_updown)
  ON_COMMAND(MSG_FOCUS_DOWN, on_ca_super_osd_exit_updown)
  ON_COMMAND(MSG_EXIT, on_ca_super_osd_exit)
  ON_COMMAND(MSG_CANCEL, on_ca_super_osd_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_ca_super_osd_exit_all)
END_MSGPROC(ca_super_osd_proc, ui_comm_root_proc)
