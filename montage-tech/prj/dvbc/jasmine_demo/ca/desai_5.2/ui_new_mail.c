/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ui_new_mail.h"
#include "cas_manager.h"
#include "ui_ca_public.h"

static BOOL g_is_new_mail = FALSE;
static BOOL g_is_full_mail = FALSE;
static BOOL is_pic_flash = FALSE;

enum control_id
{
  IDC_EMAIL_ICON = 1,
};

static RET_CODE email_pic_cont_proc(control_t *cont, u16 msg, 
                             u32 para1, u32 para2);

BOOL ui_is_new_mail(void)
{
  return g_is_new_mail;
}

void ui_set_new_mail(BOOL is_new_mail)
{
  g_is_new_mail = is_new_mail;
}

BOOL ui_is_full_mail(void)
{
  return g_is_full_mail;
}

void ui_set_full_mail(BOOL is_full_mail)
{
  g_is_full_mail = is_full_mail;
}


RET_CODE open_ui_new_mail(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  
  // check for close
  if(fw_find_root_by_id(ROOT_ID_NEW_MAIL) != NULL)
  {
    close_new_mail();
  }
  
  p_cont = fw_create_mainwin(ROOT_ID_NEW_MAIL,
                             NEW_MAIL_CONT_FULL_X, 
                             NEW_MAIL_CONT_FULL_Y, 
                             NEW_MAIL_CONT_FULL_W, 
                             NEW_MAIL_CONT_FULL_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_proc(p_cont, email_pic_cont_proc);
  
  p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_EMAIL_ICON,
                            NEW_MAIL_ICON_FULL_X, 
                            NEW_MAIL_ICON_FULL_Y,
                            NEW_MAIL_ICON_FULL_W, 
                            NEW_MAIL_ICON_FULL_H,
                            p_cont, 0);
  bmap_set_content_by_id(p_icon, IM_ICON_EMAIL);
  
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

void close_new_mail(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_NEW_MAIL);
}

void update_email_picture(email_pic_show_mode_t e_show_mode)
{
  control_t *p_cont = NULL;
  //RET_CODE ret= SUCCESS;

  if((is_pic_flash)&&(EMAIL_PIC_FLASH  != e_show_mode))
  {
    is_pic_flash = FALSE;
    fw_tmr_destroy(ROOT_ID_NEW_MAIL, MSG_PIC_FLASH);
    close_new_mail();
  }

  switch(e_show_mode)
  {
      case EMAIL_PIC_SHOW:
        open_ui_new_mail(0, 0);
        break;
      case EMAIL_PIC_HIDE:
        close_new_mail();
        break;
      case EMAIL_PIC_FLASH:
        if(!is_pic_flash)
        {
          p_cont = fw_find_root_by_id(ROOT_ID_NEW_MAIL);
          if(p_cont == NULL)
          {
              open_ui_new_mail(0, 0);
          }
          //ret = fw_tmr_create(ROOT_ID_NEW_MAIL, MSG_PIC_FLASH, 2000, TRUE);
          fw_tmr_create(ROOT_ID_NEW_MAIL, MSG_PIC_FLASH, 2000, TRUE);
          is_pic_flash = TRUE;
        }
        break;
      default:
      break;
        //return;
  }
  
  //return;
}

// RET_CODE on_email_pic_flash_check(void)
static RET_CODE on_email_pic_flash(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_bmp_ctrl = NULL;
  static u16 us_flash_count = 0;
  
  if(!ui_is_fullscreen_menu(fw_get_focus_id()))
    return ERR_FAILURE;
  
  if(!is_pic_flash)
  {
      return ERR_FAILURE;
  }

  p_bmp_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_EMAIL_ICON);
  if(NULL == p_bmp_ctrl)
  {
      return ERR_FAILURE;
  }

  us_flash_count ++;
  us_flash_count = us_flash_count%2;
  
  if(0 == us_flash_count)
  {
    bmap_set_content_by_id(p_bmp_ctrl, 0);
  }
  else if(1 == us_flash_count)
  {
    bmap_set_content_by_id(p_bmp_ctrl, IM_ICON_EMAIL);
  }

  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}

BEGIN_MSGPROC(email_pic_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PIC_FLASH, on_email_pic_flash)
END_MSGPROC(email_pic_cont_proc, ui_comm_root_proc)

