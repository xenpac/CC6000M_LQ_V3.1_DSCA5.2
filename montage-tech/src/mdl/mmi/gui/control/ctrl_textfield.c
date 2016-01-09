/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_textfield.c
   this file  implement the functions defined in  ctrl_textfield.h, also it
   implement some internal used   function. All these functions are about how to
   decribe, set and draw a pbar control.
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
#include "gui_roll.h"

#include "ctrl_textfield.h"
#include "ctrl_scrollbar.h"

/*!
  text-field control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;    
  /*!
     String content of textfield control.
    */
  u32 str_char;
  /*!
     R-style of string in normal state.
    */
  u32 n_fstyle;
  /*!
     R-style of string in highlight color.
    */
  u32 h_fstyle;
  /*!
     R-style of string in gray color.
    */
  u32 g_fstyle;
  /*!
     Distance between lines.
    */
  u8 str_l_space;
  /*!
     Distance from left side of textfield to left side of string
    */
  u16 str_left;
  /*!
     Distance from top side of textfield to top side of string
    */
  u16 str_top;
  /*!
     String is eof | indicate how many chars has been drawed
    */
  u32 status;
  /*!
     Point to the curn line.
    */
  u32 curn;
  /*!
     Specified the drawing style
    */
  u32 draw_style;
  /*!
    Specified the total lines of the textfield.
    */
  u32 total;
  /*!
    Specified the lines per page of the textfield.
    */
  u32 page;
  /*!
    Specified the scroll bar for textfield in page mode.
    */
  control_t *p_scroll_bar;
  /*!
     Line start address.(only used in page mode.)
    */
  u32 *p_line_addr;  
}ctrl_text_t;

/*!
   page infor
  */
typedef struct page_info
{
  /*!
     the address of string in current page
    */
  u32 str_addr;
  /*!
     the previous page infor
    */
  struct page_info *p_prev;
  /*!
     the next page infor
    */
  struct page_info *p_next;
} page_info_t;

static u32 *_text_get_content_addr(control_t *p_ctrl, u32 type)
{
  ctrl_text_t *p_text = (ctrl_text_t *)p_ctrl;

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  return (u32 *)&p_text->str_char;
}


static void _text_set_static_content(control_t *p_ctrl, u32 type, u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _text_get_content_addr(p_ctrl, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = (u32)val;
}


static u32 _text_get_fstyle(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;
  u32 font = 0;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }
  
  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_text->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_text->g_fstyle;
      break;

    default:
      font = p_text->n_fstyle;
  }

  return font;
}

static u16 *_text_get_str_buf(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;
  u16 *p_str_buf = NULL;
  static u16 str_num[NUM_STRING_LEN + 1];
  
  p_text = (ctrl_text_t *)p_ctrl;

  switch(p_ctrl->priv_attr & TEXT_STRTYPE_MASK)
  {
    case TEXT_STRTYPE_STRID:
      p_str_buf = (u16 *)gui_get_string_addr((u16)p_text->str_char);
      break;
      
    case TEXT_STRTYPE_UNICODE:
      p_str_buf = (u16 *)p_text->str_char;
      break;
      
    case TEXT_STRTYPE_EXTSTR:
      p_str_buf = (u16 *)p_text->str_char;
      
      if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
      {
        if(p_text->p_line_addr != NULL)
        {
          p_str_buf = (u16 *)*(p_text->p_line_addr + p_text->curn);
        }
      }
      break;
      
    case TEXT_STRTYPE_DEC:
      convert_i_to_dec_str(str_num, p_text->str_char);
      p_str_buf = str_num;
      break;
      
    case TEXT_STRTYPE_HEX:
      convert_i_to_hex_str(str_num, p_text->str_char);
      p_str_buf = str_num;
      break;
      
    default:
      p_str_buf = NULL;
      break;
  }

  return p_str_buf;
}

static void _text_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_text_t *p_text = NULL;
  u32 font = 0;
  rect_t str_rect;
  u16 *p_str_buf = NULL;

  if(ctrl_is_rolling(p_ctrl))
  {
    return;
  }

  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_text = (ctrl_text_t *)p_ctrl;

  font = _text_get_fstyle(p_ctrl);
  
  ctrl_get_draw_rect(p_ctrl, &str_rect);

  p_str_buf = _text_get_str_buf(p_ctrl);

  p_text->status =
    gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                    p_text->str_left, p_text->str_top, p_text->str_l_space,
                    p_str_buf, font, MAKE_DRAW_STYLE(p_text->draw_style, 0));
}


static void _text_free_ctrl(control_t *p_ctrl)
{
  ctrl_text_t *p_text = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_UNICODE)
  {
    if(p_text->str_char != 0)
    {
      ctrl_unistr_free((void *)p_text->str_char);
      p_text->str_char = 0;
    }
  }

  //page mode, should free line addr buffer.
  if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
  {
    if(p_text->p_line_addr != NULL)
    {
      mmi_free_buf(p_text->p_line_addr);
      p_text->p_line_addr = NULL;
    }
  }
}

static void _text_draw_scroll_bar(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {  
      if(p_text->p_scroll_bar != NULL)
      {
        sbar_set_count(p_text->p_scroll_bar, p_text->page, p_text->total);
        ctrl_paint_ctrl(p_text->p_scroll_bar, TRUE);
      }
    }
  }
}

RET_CODE text_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_text_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of textfield class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER |
                              TEXT_STRTYPE_STRID;
  p_default_ctrl->p_proc = text_class_proc;
  p_default_ctrl->p_paint = _text_draw;

  // initalize the default data of textfield class
  
  if(ctrl_register_ctrl_class(CTRL_TEXT, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void text_set_content_by_ascstr(control_t *p_ctrl, u8 *p_ascstr)
{
  u16 **p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = (u16 **)_text_get_content_addr(p_ctrl, TEXT_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp = (u16 *)ctrl_unistr_realloc((void *)(*p_temp), strlen((char *)p_ascstr));
  if(*p_temp == NULL)
  {
    return;
  }
  str_asc2uni(p_ascstr, (u16 *)(*p_temp));
}


void text_set_content_by_unistr(control_t *p_ctrl, u16 *p_unistr)
{
  u16 **p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = (u16 **)_text_get_content_addr(p_ctrl, TEXT_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return;
  }

  *p_temp =
    (u16 *)ctrl_unistr_realloc((void *)(*p_temp), uni_strlen(p_unistr));
  if(*p_temp == NULL)
  {
    return;
  }
  uni_strcpy((u16 *)(*p_temp), p_unistr);
}


void text_set_content_by_strid(control_t *p_ctrl, u16 str_id)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_STRID, str_id);
}


void text_set_content_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_EXTSTR, (u32)p_extstr);
}


void text_set_content_by_dec(control_t *p_ctrl, s32 dec)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_DEC, dec);
}


void text_set_content_by_hex(control_t *p_ctrl, s32 hex)
{
  _text_set_static_content(p_ctrl, TEXT_STRTYPE_HEX, hex);
}


u32 text_get_content(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  return p_text->str_char;
}


u32 *text_get_content_buf(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  return &p_text->str_char;
}


u32 text_get_status(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  return p_text->status;
}


void text_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  p_text->n_fstyle = n_fstyle;
  p_text->h_fstyle = h_fstyle;
  p_text->g_fstyle = g_fstyle;
}

void text_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  *p_factive = p_text->n_fstyle;
  *p_fhlight = p_text->h_fstyle;
  *p_finactive = p_text->g_fstyle;
}

void text_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  p_text->str_left = left;
  p_text->str_top = top;
}

void text_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  
  *p_left = p_text->str_left;
  *p_top = p_text->str_top;
}

void text_set_line_gap(control_t *p_ctrl, u8 l_space)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  p_text->str_l_space = l_space;
}

u8 text_get_line_gap(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  
  return p_text->str_l_space;
}

void text_enable_page(control_t *p_ctrl, BOOL enable)
{
  MT_ASSERT(p_ctrl != NULL);

  //only support page mode when string type is external string.
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != TEXT_STRTYPE_EXTSTR)
  {
    return;
  }

  if(enable)
  {
    p_ctrl->priv_attr |= TEXT_PAGE_MASK;
  }
  else
  {
    p_ctrl->priv_attr &= (~TEXT_PAGE_MASK);
  }
}

static u16 text_reset_page(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;
  rect_t str_rect = {0};
  u32 font = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  if(p_text->curn >= p_text->total)
  {
    return 0;
  }
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      font = _text_get_fstyle(p_ctrl);
      
      ctrl_get_draw_rect(p_ctrl, &str_rect);

      if((u16 *)(p_text->p_line_addr + p_text->curn) == NULL)
      {
        return 0;
      }      

      return gui_get_lines_per_page(font, p_ctrl->priv_attr, &str_rect,
        (u16 *)(*(p_text->p_line_addr + p_text->curn)), p_text->str_left, p_text->str_top,
        p_text->str_l_space, MAKE_DRAW_STYLE(p_text->draw_style, 0));      
    }
  }

  return 0;
}

void text_reset_param(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;
  rect_t str_rect = {0};
  u16 *p_str_buf = NULL;
  u16 i = 0;
  u32 font = 0;
  
  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      font = _text_get_fstyle(p_ctrl);
      
      ctrl_get_draw_rect(p_ctrl, &str_rect);

      //caculate total line.
      p_text->total = gui_get_unistr_total_lines(font, p_ctrl->priv_attr, &str_rect,
        (u16 *)p_text->str_char, p_text->str_left, MAKE_DRAW_STYLE(p_text->draw_style, 0));

      if(p_text->total != 0)
      {
        p_text->p_line_addr = mmi_realloc_buf(p_text->p_line_addr, sizeof(u32) * p_text->total);
      }
      else
      {
        if(p_text->p_line_addr != NULL)
        {
          mmi_free_buf(p_text->p_line_addr);
          p_text->p_line_addr = NULL;
        }
      }

      //caculate each line start.
      p_str_buf = (u16 *)p_text->str_char;

      for(i = 0; i < p_text->total; i++)
      {
        *(p_text->p_line_addr + i) = (u32)p_str_buf;
        p_str_buf = gui_get_next_line(font, p_ctrl->priv_attr, &str_rect,
          p_str_buf, p_text->str_left, MAKE_DRAW_STYLE(p_text->draw_style, 0));
      }

      p_text->curn = 0;
      if(p_text->p_scroll_bar != NULL)
      {
        sbar_set_count(p_text->p_scroll_bar, p_text->page, p_text->total);
        sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
      }
      //caculate page line.
      p_text->page = text_reset_page(p_ctrl);
    }
  }
}

void text_set_scrollbar(control_t *p_ctrl, control_t *p_ctrl_bar)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {  
      p_text = (ctrl_text_t *)p_ctrl;
      p_text->p_scroll_bar = p_ctrl_bar;

      if(p_ctrl_bar != NULL)
      {
        sbar_set_count(p_ctrl_bar, p_text->page, p_text->total);
        sbar_set_current_pos(p_ctrl_bar, p_text->curn);
      }
    }
  }
}


void text_set_breakword(control_t *p_ctrl, BOOL enable)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;
  p_text->draw_style = enable ? STRDRAW_BREAK_WORD : STRDRAW_NORMAL;
}


u32 text_get_breakword(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  return p_text->draw_style;
}

BOOL text_is_breakword(control_t *p_ctrl)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_text = (ctrl_text_t *)p_ctrl;

  return (BOOL)(p_text->draw_style == STRDRAW_BREAK_WORD);
}


void text_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TEXT_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & TEXT_ALIGN_MASK); //set new style
}

u32 text_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & TEXT_ALIGN_MASK);
}

void text_set_content_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TEXT_STRTYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TEXT_STRTYPE_MASK);    //set new type
}

static RET_CODE on_text_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t parent_dc = (hdc_t)(para2);

  ctrl_default_proc(p_ctrl, msg, para1, para2);

  //this bar isn't text's child, when parent_dc isn't 0,
  //parent control will draw bar
  if(parent_dc == HDC_INVALID)
  {
    _text_draw_scroll_bar(p_ctrl);
  }

  return SUCCESS;
}

static RET_CODE on_text_increase(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);
  
  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_text = (ctrl_text_t *)p_ctrl;

      if(p_text->curn > 0)
      {
        p_text->curn--;

        p_text->page = text_reset_page(p_ctrl);
        
        if(p_text->p_scroll_bar != NULL)
        {
          sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
        }

        ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
      }  
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_decrease(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_text = (ctrl_text_t *)p_ctrl;

      if(p_text->total > p_text->page)
      {
        if(p_text->curn < (p_text->total - p_text->page))
        {
          p_text->curn++;

          p_text->page = text_reset_page(p_ctrl);

          if(p_text->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
          }

          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_page_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_text = (ctrl_text_t *)p_ctrl;

      if(p_text->curn >= p_text->page)
      {
        p_text->curn -= p_text->page;

        p_text->page = text_reset_page(p_ctrl);

        if(p_text->p_scroll_bar != NULL)
        {
          sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
        }

        ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
      }
      else 
      {
        if(p_text->curn != 0)
        {
          p_text->curn = 0;

          p_text->page = text_reset_page(p_ctrl);

          if(p_text->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
          }
          
          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}

static RET_CODE on_text_page_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_t *p_text = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) == TEXT_STRTYPE_EXTSTR)
  {
    if(p_ctrl->priv_attr & TEXT_PAGE_MASK)
    {
      p_text = (ctrl_text_t *)p_ctrl;

      if(p_text->total > p_text->page)
      {
        if((p_text->curn + p_text->page) < p_text->total)
        {
          p_text->curn += p_text->page;

          p_text->page = text_reset_page(p_ctrl);

          if(p_text->p_scroll_bar != NULL)
          {
            sbar_set_current_pos(p_text->p_scroll_bar, p_text->curn);
          }

          ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
        }
      }
    }
  }
  
  return SUCCESS;
}


static RET_CODE on_text_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(ctrl_is_rolling(p_ctrl))
  {
    gui_stop_roll(p_ctrl);
  }

  _text_free_ctrl(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}

static RET_CODE on_text_roll_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_text_t *p_text = NULL;
  rsurf_ceate_t rsurf = {0};
    
  p_text = (ctrl_text_t *)p_ctrl;

  ctrl_get_draw_rect(p_ctrl, &rsurf.rect);

  rsurf.p_str = (u16 *)_text_get_str_buf(p_ctrl);

  rsurf.fstyle_idx =  _text_get_fstyle(p_ctrl);;

  rsurf.p_ctrl = p_ctrl;

  rsurf.context = 0;

  rsurf.draw_style = MAKE_DRAW_STYLE(p_text->draw_style, 0);  

  rsurf.is_topmost = ctrl_is_topmost(p_ctrl);
  
  gui_create_rsurf(&rsurf, (roll_param_t *)para1);
  
  return SUCCESS;
}

static RET_CODE on_text_rolling(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  hdc_t hdc = 0;  

  MT_ASSERT(p_ctrl != NULL);

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  hdc = gui_begin_paint(p_ctrl, 0);
  gui_paint_frame(hdc, p_ctrl);  

  gui_rolling_node((void *)para1, hdc);

  gui_end_paint(p_ctrl, hdc);  
  
  return SUCCESS;
}

static RET_CODE on_text_roll_next(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  roll_data_t type = (roll_data_t)para2;
  u32 content = para1;

  switch(type)
  {
    case ROLL_STRID:
      if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != TEXT_STRTYPE_STRID)
      {
        return ERR_FAILURE;
      }

      text_set_content_by_strid(p_ctrl, (u16)content);      
      break;

    case ROLL_ASCSTR:
      if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != TEXT_STRTYPE_UNICODE)
      {
        return ERR_FAILURE;
      }

      text_set_content_by_ascstr(p_ctrl, (u8 *)content);  
      break;

    case ROLL_UNISTR:
      if((p_ctrl->priv_attr & TEXT_STRTYPE_MASK) != TEXT_STRTYPE_UNICODE)
      {
        return ERR_FAILURE;
      }

      text_set_content_by_unistr(p_ctrl, (u16 *)content);  
      break;

    default:
      MT_ASSERT(0);
      break;
  }
  
  return SUCCESS;
}

// define the default msgmap of class
BEGIN_CTRLPROC(text_class_proc, ctrl_default_proc)
  ON_COMMAND(MSG_PAINT, on_text_paint)
  ON_COMMAND(MSG_INCREASE, on_text_increase)
  ON_COMMAND(MSG_DECREASE, on_text_decrease)
  ON_COMMAND(MSG_DESTROY, on_text_destroy)
  ON_COMMAND(MSG_PAGE_DOWN, on_text_page_down)
  ON_COMMAND(MSG_PAGE_UP, on_text_page_up)
  ON_COMMAND(MSG_START_ROLL, on_text_roll_start)
  ON_COMMAND(MSG_ROLLING, on_text_rolling)
  ON_COMMAND(MSG_ROLL_NEXT, on_text_roll_next)  
END_CTRLPROC(text_class_proc, ctrl_default_proc)
