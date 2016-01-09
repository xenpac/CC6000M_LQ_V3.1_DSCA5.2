/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_ca_finger.h"
#include "ui_ca_public.h"
#include "gdi.h"
enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
};
#define CA_FINGER_PRINT_TMOUT 20
//extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
//extern rsc_fstyle_t f_tab[FSTYLE_CNT];
RET_CODE finger_print_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void ui_ca_fix_finger_attr(finger_msg_t *p_ca_finger, finger_rect_t *
finger_rect)
{
  u16 static_w = 200;
  u16 static_h = 30;
  double ratio = 1;
  color_t color = {0};
   u32 p_colors = 0;
   
#ifdef MULTI_FINGER
  s8 i  = 0;

  UI_PRINTF("@@@@@ disp_bg:%d  back_color 0x%x %x %x %x\n",p_ca_finger->
disp_bg, p_ca_finger->show_color_back[3], p_ca_finger->show_color_back[2], 
  	                                                                            
      p_ca_finger->show_color_back[1], p_ca_finger->show_color_back[0]);
  UI_PRINTF("@@@@@ font color 0x%x %x %x %x\n", p_ca_finger->show_color[3], 
p_ca_finger->show_color[2], 
  	                                                                            
      p_ca_finger->show_color[1], p_ca_finger->show_color[0]);
  //set background color
  if(p_ca_finger->disp_bg)
  {
   color.r = p_ca_finger->show_color_back[3] & 0xff;    //a
   color.g = p_ca_finger->show_color_back[2] & 0xff;   //r
   color.b = p_ca_finger->show_color_back[1] & 0xff;   //g
   color.a = p_ca_finger->show_color_back[0] & 0xff;   //b
   gdi_set_palette(0, (u16)C_FINGER_BACKGROUND, 1, &color);
 	}

  //set font color
  color.r = p_ca_finger->show_color[3] & 0xff;    //a
  color.g = p_ca_finger->show_color[2] & 0xff;   //r
  color.b = p_ca_finger->show_color[1] & 0xff;   //g
  color.a = p_ca_finger->show_color[0] & 0xff;   //b
  gdi_set_palette(0, (u16)C_FINGER_FONT, 1, &color);

    //w & h
	for(i = 0; i < 11 - p_ca_finger->show_front_size; i++)
	{
	  ratio = ratio * 0.87;
	}
#else
  //set bg color
  
  color.a = 0xb2;   //a
  color.r = 0x04;    //r
  color.g = 0x14;   //g
  color.b = 0x33;   //b

  p_colors = ((u32)color.a << 24) 
                     | ((u32)color.r << 16) 
                     | ((u32)color.g << 8) 
                     | (color.b);//a r g b

  g_rsc_config.p_rstyle_tab[RSI_FINGER_BACKGROUND].bg.value = p_colors;
#endif

  finger_rect->w = (s16)(static_w * ratio);
  finger_rect->h = (s16)(static_h * ratio); 
  //x & y
   if(p_ca_finger->show_postion == 0)
  {
    finger_rect->x = 20;
    finger_rect->y= 80;
  }
  else if(p_ca_finger->show_postion == 1)
  {
    finger_rect->x = 1045;
    finger_rect->y = 80;
  }
  else if(p_ca_finger->show_postion == 2)
  {
    finger_rect->x = 20;
    finger_rect->y = 620;
  }
  else if(p_ca_finger->show_postion == 3)
  {
    finger_rect->x = 1045;
    finger_rect->y = 620;
  }
  else
  {
    finger_rect->x = (s16)(p_ca_finger->show_postion_x);
    finger_rect->y = (s16)(p_ca_finger->show_postion_y);
  } 
  
  if(finger_rect->x < 0)
    finger_rect->x = 20;
    
  if(finger_rect->x > (SCREEN_WIDTH - 200))
   finger_rect->x = SCREEN_WIDTH - 200;

  //roll_title y pos + roll title width 
  if(finger_rect->y < 60)
    finger_rect->y = 60;
  
  //roll_title + finger_width = 30
  if(finger_rect->y > (SCREEN_HEIGHT - 80))
    finger_rect->y = SCREEN_HEIGHT - 80;
}

void open_ca_finger_menu(finger_msg_t *p_ca_finger)
{
  control_t *p_txt, *p_cont = NULL;
  finger_rect_t finger_rect = {0};
  
  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
  {
    return;
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_FINGER_PRINT))
  {
      close_ca_finger_menu();
  }
  
  //fix finger attr
  ui_ca_fix_finger_attr(p_ca_finger, &finger_rect);
  
  p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
                             (u16)(finger_rect.x), (u16)(finger_rect.y),
                             (u16)(finger_rect.w), (u16)(finger_rect.h),
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return;
  }
  ctrl_set_rstyle(p_cont, RSI_FINGER_BACKGROUND, RSI_FINGER_BACKGROUND, 
RSI_FINGER_BACKGROUND);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           0, 0, 200, 30,
                           p_cont, 0);
  ctrl_set_rstyle(p_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_CA_FINGER_TXT, FSI_CA_FINGER_TXT, FSI_CA_FINGER_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_txt, p_ca_finger->data);
  
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ctrl_paint_ctrl(p_cont, TRUE);
}
void close_ca_finger_menu(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
}

BEGIN_MSGPROC(finger_print_root_proc, ui_comm_root_proc)
END_MSGPROC(finger_print_root_proc, ui_comm_root_proc)


