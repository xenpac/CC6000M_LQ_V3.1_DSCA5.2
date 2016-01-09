/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_editbox.c
   this file  implement the functions defined in  ctrl_editbox.h .
  */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_char.h"

//#include "osd.h"
//#include "gpe.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_string.h"
#include "ctrl_base.h"
#include "ctrl_common.h"

#include "gui_resource.h"
#include "gui_paint.h"

#include "ctrl_editbox.h"

/*!
  editbox control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     String in unicode
    */
  u32 str_char;
  /*!
     Normal color of string
    */
  u32 n_fstyle;
  /*!
     Highlight color of string
    */
  u32 h_fstyle;
  /*!
     Gray color of string
    */
  u32 g_fstyle;
  /*!
     Distance from left side of textfield to left side of string.
    */
  u16 str_left;
  /*!
     Distance from top side of textfield to top side of string.
    */
  u16 str_top;
  /*!
     Maximal text length. zero means no limit.
    */
  u16 str_maxtext;
  /*!
     The hide mask before edit
    */
  u8 bef_hmask;
  /*!
     The hide mask before edit
    */
  u8 aft_hmask;
  /*!
     String in arabic base code
    */
  u32 str_arabic;  
  /*!
     Current bit
    */
  u8 curn_bit;
  /*!
     Current bit length
    */
  u8 bit_length;
  /*!
     Distance between lines
    */
  u8 str_l_space;
  /*!
    draw curn
    */  
  u8 draw_curn;      
}ctrl_ebox_t;

static void _ebox_alloc_buf(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_str = (u16 *)ctrl_unistr_alloc(EBOX_MAX_STRLEN + 2);
  p_str[0] = '\0';

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ebox->str_char = (u32)p_str;
}


static void _ebox_free_buf(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  if(p_ebox->str_char != 0)
  {
    ctrl_unistr_free((void *)p_ebox->str_char);
    p_ebox->str_char = 0;
  }

  if((p_ctrl->priv_attr & EBOX_ARABIC_MASK) && (p_ebox->str_arabic != 0))
  {
    ctrl_unistr_free((void *)p_ebox->str_arabic);
    p_ebox->str_arabic = 0;
  }
}


static u32 _ebox_get_fstyle(u8 attr, ctrl_ebox_t *p_ebox)
{
  u32 font = 0;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_ebox->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_ebox->g_fstyle;
      break;

    default:
      font = p_ebox->n_fstyle;
  }

  return font;
}

static u8 ebox_get_underline_pos(ctrl_ebox_t *p_ebox, u16 content[])
{
  u16 post[2];
  u8 pos = 0;
  
  post[0] = 0x0020;
  post[1] = '\0';

  if(p_ebox->bit_length == 0)
  {
    p_ebox->curn_bit = 1;
  }

  uni_strcat(content, post, EBOX_MAX_STRLEN + 1);
  
  pos = p_ebox->curn_bit - 1;

  return pos;
}

static void ebox_recal_draw_curn(ctrl_ebox_t *p_ebox, u32 fstyle_idx,
  u32 style, rect_t *p_rc, u16 x_off, u32 draw_style, u16 *p_str, u16 focus)
{
  u16 *p_temp = NULL;
  u16 i = 0, str_len = 0;
  
  if(focus <= p_ebox->draw_curn)
  {
    p_ebox->draw_curn = focus;

    return;
  }

  str_len = uni_strlen(p_str);
  
  if(str_len == 0)
  {
    p_ebox->draw_curn = 0;

    return;
  }

  MT_ASSERT(focus < str_len);

  p_temp = p_str + p_ebox->draw_curn;

  while(1)
  {
    p_temp = gui_get_next_line(fstyle_idx, style, p_rc, p_temp + i, x_off, draw_style);

    if(p_temp == NULL)
    {
      break;
    }

    if(p_str + focus < p_temp)
    {
      break;
    }

    i++;
  }

  p_ebox->draw_curn += i;

  return;
}


static void _ebox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_ebox_t *p_ebox = NULL;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  u32 len = 0;
  rect_t str_rect = {0};
  u16 content[EBOX_MAX_STRLEN + 1] = {0};
  u16 underline_pos = 0;
  u32 draw_style = 0;

  MT_ASSERT(p_ctrl != NULL);

  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  //draw title string
  if((ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl)))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _ebox_get_fstyle(attr, p_ebox);

  ctrl_get_draw_rect(p_ctrl, &str_rect);

  // prepare string buffer for drawing
  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_HIDE)
  {
    // on hide mode
    len = uni_strlen((u16 *)p_ebox->str_char);
    uni_strnset(&content[0], char_asc2uni(p_ebox->aft_hmask), len);

    if(p_ebox->str_maxtext > 0)
    {
      len = uni_strlen(content);
      if(len < p_ebox->str_maxtext)
      {
        uni_strnset(&content[len], char_asc2uni(p_ebox->bef_hmask),
                    p_ebox->str_maxtext - len);
      }
    }

    draw_style = MAKE_DRAW_STYLE(STRDRAW_NORMAL | STRDRAW_BREAK_WORD, 0);

    ebox_recal_draw_curn(p_ebox, font, p_ctrl->priv_attr,
      &str_rect, p_ebox->str_left, draw_style, content, uni_strlen(content) - 1);
    
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr, p_ebox->str_left,
                  p_ebox->str_top, p_ebox->str_l_space,
                  content + p_ebox->draw_curn, font, draw_style);
  }
  else if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_SHIFT)
  {
    uni_strncpy(content, (u16 *)p_ebox->str_char, EBOX_MAX_STRLEN);

    underline_pos = ebox_get_underline_pos(p_ebox, content);

    draw_style = MAKE_DRAW_STYLE(STRDRAW_NORMAL | STRDRAW_BREAK_WORD, 0);

    ebox_recal_draw_curn(p_ebox, font, p_ctrl->priv_attr,
      &str_rect, p_ebox->str_left, draw_style, content, underline_pos);

    underline_pos -= p_ebox->draw_curn;

    draw_style = MAKE_DRAW_STYLE(STRDRAW_WITH_UNDERLINE | STRDRAW_BREAK_WORD, underline_pos);
    
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr, p_ebox->str_left, p_ebox->str_top,
      p_ebox->str_l_space, content + p_ebox->draw_curn, font, draw_style);
  }
  else if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_NO_CURSOR)
  {
    uni_strncpy(content, (u16 *)p_ebox->str_char, EBOX_MAX_STRLEN);

    draw_style = MAKE_DRAW_STYLE(STRDRAW_NORMAL | STRDRAW_BREAK_WORD, 0);

    ebox_recal_draw_curn(p_ebox, font, p_ctrl->priv_attr,
      &str_rect, p_ebox->str_left, draw_style, content, uni_strlen(content) - 1);
    
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr, 
      p_ebox->str_left, p_ebox->str_top, p_ebox->str_l_space, 
      content + p_ebox->draw_curn, font, draw_style);
  }
  else
  {
    uni_strncpy(content, (u16 *)p_ebox->str_char, EBOX_MAX_STRLEN);

    if((p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
    {
      // on edit stage
      u16 post[2];

      post[0] = char_asc2uni('_');
      post[1] = '\0';

      uni_strcat(content, post, EBOX_MAX_STRLEN);
    }

    draw_style = MAKE_DRAW_STYLE(STRDRAW_NORMAL | STRDRAW_BREAK_WORD, 0);

    ebox_recal_draw_curn(p_ebox, font, p_ctrl->priv_attr,
      &str_rect, p_ebox->str_left, draw_style, content, uni_strlen(content) - 1);
    
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr, p_ebox->str_left,
      p_ebox->str_top, p_ebox->str_l_space, content + p_ebox->draw_curn, font, draw_style);
  }
}


RET_CODE ebox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;
  ctrl_ebox_t *p_default_data = NULL;

  register_info.data_size = sizeof(ctrl_ebox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  p_default_data = (ctrl_ebox_t *)register_info.p_default_ctrl;
  
  // initialize the default control of editbox class
  p_default_ctrl->priv_attr = EBOX_WORKTYPE_EDIT | STL_CENTER | STL_VCENTER;
  p_default_ctrl->p_proc = ebox_class_proc;
  p_default_ctrl->p_paint = _ebox_draw;

  // initalize the default data of editbox class
  p_default_data->bef_hmask = char_asc2uni('-');
  p_default_data->aft_hmask = char_asc2uni('*');
  
  if(ctrl_register_ctrl_class(CTRL_EBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void ebox_set_content_by_ascstr(control_t *p_ctrl, char *p_ascstr)
{
  u32 len = strlen(p_ascstr);
  u16 *p_content = ebox_get_content(p_ctrl);
  ctrl_ebox_t *p_ebox = NULL;
  u16 max = 0;
  
  MT_ASSERT(p_ctrl != NULL && p_ascstr != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  if(p_content == NULL)
  {
    return;
  }

  if(p_ebox->str_maxtext == 0)
  {
    max = EBOX_MAX_STRLEN;
  }
  else
  {
    max = p_ebox->str_maxtext;
  }

  if(len > max)
  {
    len = max;
  }

  str_nasc2uni((u8 *)p_ascstr, p_content, len);

  if(len == 0)
  {
    p_ebox->curn_bit = 1;
    p_ebox->bit_length = 0;
  }
  else
  {
    p_ebox->bit_length = len;
    p_ebox->curn_bit = len + 1;
  }
}


void ebox_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr)
{
  u32 len = uni_strlen(p_unistr);
  u16 *p_content = ebox_get_content(p_ctrl);
  ctrl_ebox_t *p_ebox = NULL;
  u16 max = 0;

  MT_ASSERT(p_ctrl != NULL && p_unistr != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  if(p_content == NULL)
  {
    return;
  }

  if(p_ebox->str_maxtext == 0)
  {
    max = EBOX_MAX_STRLEN;
  }
  else
  {
    max = p_ebox->str_maxtext;
  }

  if(len > max)
  {
    len = max;
  }  

  uni_strncpy(p_content, p_unistr, len);

  if(len == 0)
  {
    p_ebox->curn_bit = 1;
    p_ebox->bit_length = 0;
  }
  else
  {
    p_ebox->bit_length = len;
    p_ebox->curn_bit = len + 1;
  }
}

void ebox_set_keyboard_content_by_unistr(control_t *p_ctrl, u16 *p_unistr)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  
  ebox_set_content_by_unistr(p_ctrl, p_unistr);
  if ((p_ctrl->priv_attr & EBOX_ARABIC_MASK) && (p_ebox->str_arabic != 0))
  {
    uni_strcpy((u16 *) p_ebox->str_arabic, p_unistr);
  }
}

u16 *ebox_get_content(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  return (u16 *)p_ebox->str_char;
}


void ebox_set_line_gap(control_t *p_ctrl, u8 l_space)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ebox->str_l_space = l_space;
}


void ebox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ebox->n_fstyle = n_fstyle;
  p_ebox->h_fstyle = h_fstyle;
  p_ebox->g_fstyle = g_fstyle;
}


void ebox_get_fstyle(control_t *p_ctrl, u32 *p_active, u32 *p_hlight, u32 *p_inactive)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_active != NULL);
  MT_ASSERT(p_hlight != NULL);
  MT_ASSERT(p_inactive != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  *p_active = p_ebox->n_fstyle;
  *p_hlight = p_ebox->h_fstyle;
  *p_inactive = p_ebox->g_fstyle;
  
}

void ebox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ebox->str_left = left;
  p_ebox->str_top = top;
}

void ebox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);
  
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  
  *p_left = p_ebox->str_left;
  *p_top = p_ebox->str_top;
}


void ebox_set_maxtext(control_t *p_ctrl, u16 max)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  if(max > EBOX_MAX_STRLEN)
  {
    max = EBOX_MAX_STRLEN;
  }
  p_ebox->str_maxtext = max;
}


u16 ebox_get_maxtext(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  return p_ebox->str_maxtext;
}


void ebox_set_worktype(control_t *p_ctrl, u32 worktype)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~EBOX_WORKTYPE_MASK);
  p_ctrl->priv_attr |= (worktype & EBOX_WORKTYPE_MASK);
}


void ebox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~EBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & EBOX_ALIGN_MASK); //set new style
}

u32 ebox_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & EBOX_ALIGN_MASK);        //clean old style
}

void ebox_set_arabic_type(control_t *p_ctrl, u32 style)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str_ara = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_ebox = (ctrl_ebox_t *)p_ctrl;

  //alloc buffer to save arabic base code
  if (((p_ctrl->priv_attr & EBOX_ARABIC_MASK) == 0) && (p_ebox->str_arabic == 0))
  {
    p_ctrl->priv_attr |= EBOX_ARABIC_MASK; //set new style
    p_str_ara = (u16 *)ctrl_unistr_alloc(EBOX_MAX_STRLEN * 2 + 1);
    p_str_ara[0] = '\0';
    p_ebox->str_arabic = (u32)p_str_ara;
  }
}

BOOL ebox_set_hide_mask(control_t *p_ctrl, u8 bef_mask, u8 aft_mask)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_HIDE)
  {
    // on hide mode
    return FALSE;
  }

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  p_ebox->bef_hmask = bef_mask;
  p_ebox->aft_hmask = aft_mask;

  return TRUE;
}


BOOL ebox_enter_edit(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ctrl->priv_attr |= EBOX_HL_STATUS_MASK;

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  return TRUE;
}


BOOL ebox_exit_edit(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_ctrl->priv_attr &= (~EBOX_HL_STATUS_MASK);

  // notify leave edit
  ctrl_process_msg(p_ctrl, MSG_CHANGED, p_ebox->str_char, 0);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  return TRUE;
}


BOOL ebox_back_space(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 len = 0;
  u8 temp_bit = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  if(p_ctrl->priv_attr & EBOX_ARABIC_MASK)
  {
    p_str = (u16 *)p_ebox->str_arabic;
    len = uni_strlen(p_str);
    if(len > 0)
    {
      *(p_str + len - 1) = '\0';
      arabic_str_convert((u16 *)p_ebox->str_arabic, (u16 *)p_ebox->str_char, EBOX_MAX_STRLEN);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      return TRUE;
    }
  }
  else
  {
    p_str = (u16 *)p_ebox->str_char;
    len = uni_strlen(p_str);
    if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_SHIFT)
    {
      if(len > 0)
      {
        *(p_str + len - 1) = '\0';
        ctrl_add_rect_to_invrgn(p_ctrl, NULL);
        return TRUE;
      }
    }
    else
    {
      temp_bit = p_ebox->curn_bit - 1;
      if((len > 0) && (*(p_str + temp_bit) != '\0') && (p_ebox->curn_bit > 1))
      {
        if(p_ebox->curn_bit > 1)
        {
          //delete the char before focus char.
          while(*(p_str + temp_bit) != '\0')
          {
            *(p_str + temp_bit - 1) = *(p_str + temp_bit);    
            temp_bit ++;
          }
          *(p_str + temp_bit - 1) = '\0';
          p_ebox->bit_length --;
          p_ebox->curn_bit --;
          ctrl_add_rect_to_invrgn(p_ctrl, NULL);
          return TRUE;
        }
        else
        {
          //focus on first bit, no previous char.
          return FALSE;
        }
      }
      else if(len > 0 && *(p_str + temp_bit) == '\0')
      {
        p_ebox->curn_bit --;
        p_ebox->bit_length --;
        *(p_str + len - 1) = '\0';
        ctrl_add_rect_to_invrgn(p_ctrl, NULL);
        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOL ebox_delete(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 len = 0;
  u8 temp_bit = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  if(p_ctrl->priv_attr & EBOX_ARABIC_MASK)
  {
    p_str = (u16 *)p_ebox->str_arabic;
    len = uni_strlen(p_str);
    if(len > 0)
    {
      *(p_str + len - 1) = '\0';
      arabic_str_convert((u16 *)p_ebox->str_arabic, (u16 *)p_ebox->str_char, EBOX_MAX_STRLEN);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      return TRUE;
    }
  }
  else
  {
    if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_SHIFT)
    {
      //none shift mode, always foucs on last bit('0'), can't delete.
      return FALSE;
    }
    else
    {
      p_str = (u16 *)p_ebox->str_char;
      len = uni_strlen(p_str);

      temp_bit = p_ebox->curn_bit - 1;
      if(len > 0 && *(p_str + temp_bit) != '\0')
      {
        while(*(p_str + temp_bit) != '\0')
        {
          temp_bit ++;
          *(p_str + temp_bit - 1) = *(p_str + temp_bit);    
        }
        p_ebox->bit_length --;
        ctrl_add_rect_to_invrgn(p_ctrl, NULL);
        return TRUE;
      }
      else if(len > 0 && *(p_str + temp_bit - 1) == '\0')
      {
        p_ebox->curn_bit --;
        p_ebox->bit_length --;
        *(p_str + len - 1) = '\0';
        ctrl_add_rect_to_invrgn(p_ctrl, NULL);
        return TRUE;
      }
    }
  }

  return FALSE;
}


BOOL ebox_empty_content(control_t *p_ctrl)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 length = 0;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_SHIFT)
  {
    p_ebox->bit_length = 0, p_ebox->curn_bit = 1;
  }
  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_str = (u16 *)p_ebox->str_char;
  length = uni_strlen(p_str);
  if(length > 0)
  {
    p_str[0] = '\0';
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    return TRUE;
  }
    

  return FALSE;
}

BOOL ebox_input_keyboard_uni(control_t *p_ctrl, u16 uni_code)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_str = (u16 *)p_ebox->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if((len >= p_ebox->str_maxtext) && (p_ebox->str_maxtext != 0)) /*zero means no limit*/
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
    return FALSE;
  }
  input_char_convert((u16 *)p_ebox->str_arabic, p_str, uni_code);

  len = uni_strlen(p_str);
  *(p_str + len) = '\0';

  // check for maxtext
  if((len >= p_ebox->str_maxtext) && (p_ebox->str_maxtext != 0)) /*zero means no limit*/
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}

BOOL ebox_input_uchar(control_t *p_ctrl, u16 uchar)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_str = (u16 *)p_ebox->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if(((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_SHIFT)
    {
      ctrl_process_msg(p_ctrl,
                       MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
      return FALSE;
    }  
  }
  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_SHIFT)
  {
    if(p_ebox->bit_length < p_ebox->curn_bit)
    {
      if((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
      {
        ctrl_process_msg(p_ctrl,
                       MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
        return FALSE;
      }
      *(p_str + len) = uchar;
      *(p_str + len + 1) = '\0';
      len = uni_strlen(p_str);
      p_ebox->bit_length ++;
      p_ebox->curn_bit ++;
    }
    else
    {
      *(p_str + p_ebox->curn_bit - 1) = uchar;
    }
  }
  else
  {
    *(p_str + len) = uchar;
    *(p_str + len + 1) = '\0';

    // check for maxtext
    len = uni_strlen(p_str);
  }
  if(((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}

BOOL ebox_input_char(control_t *p_ctrl, u16 msg)
{
  ctrl_ebox_t *p_ebox = NULL;
  u16 *p_str = NULL;
  u32 len = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_ebox = (ctrl_ebox_t *)p_ctrl;
  p_str = (u16 *)p_ebox->str_char;
  len = uni_strlen(p_str);
  if(len >= EBOX_MAX_STRLEN)
  {
    return FALSE;
  }

  if(((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_SHIFT)
    {
      ctrl_process_msg(p_ctrl,
                       MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
      return FALSE;
    } 
  }

  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) == EBOX_WORKTYPE_SHIFT)
  {
    if(p_ebox->bit_length < p_ebox->curn_bit)
    {
      if((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
      {
        ctrl_process_msg(p_ctrl,
                         MSG_MAXTEXT, 
                         p_ebox->str_char, p_ebox->str_maxtext);
        return FALSE;
      }
      *(p_str + len) = char_asc2uni((u8)(msg & MSG_DATA_MASK));
      *(p_str + len + 1) = '\0';
      len = uni_strlen(p_str);
      p_ebox->bit_length ++;
      p_ebox->curn_bit ++;
    }
    else
    {
      *(p_str + p_ebox->curn_bit - 1) = char_asc2uni((u8)(msg & MSG_DATA_MASK));
    }
  }
  else
  {
    *(p_str + len) = char_asc2uni((u8)(msg & MSG_DATA_MASK));
    *(p_str + len + 1) = '\0';
  }

  // check for maxtext
  len = uni_strlen(p_str);
  if(((len >= p_ebox->str_maxtext) && p_ebox->str_maxtext)
     != 0 /*zero means no limit*/)
  {
    ctrl_process_msg(p_ctrl,
                     MSG_MAXTEXT, p_ebox->str_char, p_ebox->str_maxtext);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}


static RET_CODE on_ebox_create(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_ctrl != NULL);

  ret = ctrl_default_proc(p_ctrl, msg, para1, para2);
  MT_ASSERT(ret == SUCCESS);

  _ebox_alloc_buf(p_ctrl);
  return SUCCESS;
}


static RET_CODE on_ebox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _ebox_free_buf(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_ebox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(!(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK))
  {
    if(ebox_enter_edit(p_ctrl))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  else
  {
    if(ebox_exit_edit(p_ctrl))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}


static RET_CODE on_ebox_backspace(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(ebox_back_space(p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}

static RET_CODE on_ebox_delete(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(ebox_delete(p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}

static RET_CODE on_ebox_empty(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(ebox_empty_content(p_ctrl))
  {
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_ebox_char(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if(p_ctrl->priv_attr & EBOX_HL_STATUS_MASK)
  {
    if(ebox_input_char(p_ctrl, msg))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}

static RET_CODE on_ebox_keyboard_char(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);
  if((p_ctrl->priv_attr & EBOX_HL_STATUS_MASK) && (p_ctrl->priv_attr & EBOX_ARABIC_MASK))
  {
    if(ebox_input_keyboard_uni(p_ctrl, para1))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}

BOOL ebox_shift_focus(control_t *p_ctrl, s8 offset)
{
  ctrl_ebox_t *p_ebox = NULL;
  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & EBOX_WORKTYPE_MASK) != EBOX_WORKTYPE_SHIFT)
  {
    return FALSE;
  }

  p_ebox = (ctrl_ebox_t *)p_ctrl;

  if(p_ebox->bit_length == 0)
  {
    return FALSE;
  }
  /*
  if(p_ebox->curn_bit == p_ebox->bit_length && p_ebox->bit_length == p_data->str_maxtext
     && offset == 1)
  {
    return FALSE;
  }
  */
  if(p_ebox->curn_bit == 1 && offset == -1)
  {
    return FALSE;
  }

  if(p_ebox->curn_bit == (p_ebox->bit_length + 1) && offset == 1)
  {
    return FALSE;
  }

  p_ebox->curn_bit = p_ebox->curn_bit + offset;

  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
  
  return TRUE;
}

static RET_CODE on_ebox_change_focus(control_t *p_ctrl,
                                     u16 msg,
                                     u32 para1,
                                     u32 para2)
{
  s8 offset = (s8)(msg == MSG_FOCUS_LEFT ? -1 : 1);

  MT_ASSERT(p_ctrl != NULL);

  ebox_shift_focus(p_ctrl, offset);
  return SUCCESS;
}

// define the default msgmap of class
BEGIN_CTRLPROC(ebox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_CHAR, on_ebox_char)
ON_COMMAND(MSG_KEYBOARD_CHAR, on_ebox_keyboard_char)
ON_COMMAND(MSG_SELECT, on_ebox_select)
ON_COMMAND(MSG_BACKSPACE, on_ebox_backspace)
ON_COMMAND(MSG_DELETE, on_ebox_delete)
ON_COMMAND(MSG_EMPTY, on_ebox_empty)
ON_COMMAND(MSG_CREATE, on_ebox_create)
ON_COMMAND(MSG_DESTROY, on_ebox_destroy)
ON_COMMAND(MSG_FOCUS_LEFT, on_ebox_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_ebox_change_focus)
END_CTRLPROC(ebox_class_proc, ctrl_default_proc)
