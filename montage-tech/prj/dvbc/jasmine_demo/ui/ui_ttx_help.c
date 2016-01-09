/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_ttx_help.h"

RET_CODE open_ttx_help(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_ctrl;
  u16 x = 0;

  OS_PRINTF("\n\nopen ttx help\n");

  p_cont = fw_create_mainwin(ROOT_ID_TTX_HELP,
                             0, 0, SCREEN_WIDTH, TTX_HELP_H,
                             0, 0, OBJ_ATTR_INACTIVE, 0);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_rstyle(p_cont, RSI_ITEM_1_SH, RSI_ITEM_1_SH, RSI_ITEM_1_SH);
  
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, 1, x, TTX_HELP_ITEMY, 
    200, TTX_HELP_ITEMH, p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TTX_HELP_EXIT);

  x += 200;

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, 2, x, TTX_HELP_ITEMY, 
    38, TTX_HELP_ITEMH, p_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_TV_MOVE);

  x += 38;

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, 3, x, TTX_HELP_ITEMY, 
    200, TTX_HELP_ITEMH, p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TTX_HELP_PAGE);

  x += 200;

  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, 4, x, TTX_HELP_ITEMY, 
    160, TTX_HELP_ITEMH, p_cont, 0);
  text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);  
  text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_ctrl, IDS_TTX_HELP_TRANSPARENCE);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

