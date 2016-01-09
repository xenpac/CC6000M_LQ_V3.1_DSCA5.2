/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_photo_info.h"

#include "albums_public.h"
enum control_id
{
  IDC_UER_NAME = 1,
  IDC_RESOLUTION,
  IDC_UPLOAD_TIMER,
  IDC_DESCRIPTION,
  IDC_PICTURE_NAME,
  IDC_DESCRIPTION_T,
};

extern iconv_t g_cd_utf8_to_utf16le;

static u16 photo_info_cont_keymap(u16 key);
RET_CODE photo_info_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);


RET_CODE open_photo_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[PHOTO_INFO_ITEM_CNT];
  control_t *p_text = NULL;
  photos_t *p_photos = (photos_t *)para1;
  u8 photo_idx = (u8)para2;
  u16 y = 0, i = 0;
  u8 asc_str[64] = {0};
  u16 stxt[PHOTO_INFO_ITEM_CNT] =
  {
    IDS_USER_NAME, IDS_PICTRUE_RESOLUTION, IDS_UPLOAD_TIMER,IDS_DESCRIPTION
  };
  char *p_inbuf, *p_outbuf;
  size_t src_len, dest_len;
  u16 uni_str[256];
    

  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_photos->p_photo == NULL)
  {
    return ERR_FAILURE;
  }

  if(photo_idx >= p_photos->cnt)
  {
    return ERR_FAILURE;
  }

  p_cont = fw_create_mainwin(ROOT_ID_PHOTO_INFO,
                                  PHOTO_INFO_CONT_X, PHOTO_INFO_CONT_Y,
                                  PHOTO_INFO_CONT_W, PHOTO_INFO_CONT_H,
                                  0, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  //ctrl_set_rstyle(p_cont, PHOTO_INFO_SET_FRM, PHOTO_INFO_SET_FRM, PHOTO_INFO_SET_FRM);
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_keymap(p_cont, photo_info_cont_keymap);
  ctrl_set_proc(p_cont, photo_info_cont_proc);
  
  y = PHOTO_INFO_ITEM_Y;
  for(i = 0; i < PHOTO_INFO_ITEM_CNT; i++)
  {

    p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_UER_NAME + i),
                    PHOTO_INFO_ITEM_X, y, PHOTO_INFO_ITEM_LW, PHOTO_INFO_ITEM_RW);
    ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    
    y += PHOTO_INFO_ITEM_H + PHOTO_INFO_ITEM_V_GAP;
  }

  if(p_photos->p_photo[photo_idx].p_user_id != NULL)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[0], (u8 *)p_photos->p_photo[photo_idx].p_user_id);
  }
  else
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[0], (u8 *)" ");
  }
  
  sprintf((s8 *)asc_str,"%d * %d", p_photos->p_photo[photo_idx].width, p_photos->p_photo[photo_idx].height);
  ui_comm_static_set_content_by_ascstr(p_ctrl[1], asc_str);

  {
    utc_time_t t = {0,};
    memcpy(&t, &p_photos->p_photo[photo_idx].create_time, sizeof(utc_time_t));
    sprintf((s8 *)asc_str,"%.4d/%.2d/%.2d %.2d:%.2d", t.year, t.month, t.day, t.hour, t.minute);
    ui_comm_static_set_content_by_ascstr(p_ctrl[2], asc_str);
  }
  
  p_text = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_PICTURE_NAME),
                             PHOTO_INFO_NAME_X, PHOTO_INFO_NAME_Y, 
                             PHOTO_INFO_NAME_W, PHOTO_INFO_NAME_H, p_cont, 0);
  ctrl_set_rstyle(p_text,  RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_text,  FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);

  if(p_photos->p_photo[photo_idx].p_photo_name != NULL)
  {
    p_inbuf = p_photos->p_photo[photo_idx].p_photo_name;
    p_outbuf = (char*) uni_str;
    src_len = strlen(p_inbuf) + 1;
    dest_len = sizeof(uni_str);
    iconv(g_cd_utf8_to_utf16le, (char**) &p_inbuf, &src_len, (char**) &p_outbuf, &dest_len);    
    
    text_set_content_by_unistr(p_text, uni_str);
  }
  else
  {
    text_set_content_by_ascstr(p_text, (u8 *)" ");
  }
  
  p_text = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_DESCRIPTION_T),
                             PHOTO_INFO_DESCR_X, PHOTO_INFO_DESCR_Y, 
                             PHOTO_INFO_DESCR_W, PHOTO_INFO_DESCR_H, p_cont, 0);
  ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);

  if(p_photos->p_photo[photo_idx].p_photo_description != NULL)
  {
    p_inbuf = p_photos->p_photo[photo_idx].p_photo_description;
    p_outbuf = (char*) uni_str;
    src_len = strlen(p_inbuf) + 1;
    dest_len = sizeof(uni_str);
    iconv(g_cd_utf8_to_utf16le, (char**) &p_inbuf, &src_len, (char**) &p_outbuf, &dest_len);   
    
    text_set_content_by_unistr(p_text, uni_str);

  }
  else
  {
    text_set_content_by_ascstr(p_text, (u8 *)"Null");
  }
 
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_photo_info_exit(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{

  manage_close_menu(ROOT_ID_PHOTO_INFO, 0, 0);
  
  return SUCCESS;
}

BEGIN_KEYMAP(photo_info_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(photo_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(photo_info_cont_proc, ui_comm_root_proc)
 ON_COMMAND(MSG_EXIT_ALL, on_photo_info_exit)
END_MSGPROC(photo_info_cont_proc, ui_comm_root_proc)





