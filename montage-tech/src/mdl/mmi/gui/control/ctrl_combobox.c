/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_combobox.c
   this file  implement the funcs defined in  ctrl_combobox.h,and some internal
   used functions.
   These functions are about register,set feature and draw a combobox.
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

#include "ctrl_list.h"
#include "ctrl_scrollbar.h"
#include "ctrl_combobox.h"

/*!
   Common data of combobox, only for static&dynamic combobox
  */
typedef struct
{
  /*!
     Total item number
    */
  u32 total;
  /*!
     Current focus item number
    */
  u32 focus;
} cbox_comm_data_t;


/*!
   Static combobox private data
  */
typedef struct
{
  /*!
     Common data
    */
  cbox_comm_data_t common;
  /*!
     Static content buffer
    */
  u32 *p_content;
  /*!
     Reserved
    */
  u32 reserve1;
  /*!
     Reserved
    */
  u32 reserve2;
} cbox_static_data_t;

/*!
   Number combobox private data
  */
typedef struct
{
  /*!
     The maximal length of the number
    */
  s32 bit_length   : 4;
  /*!
     The maximal value
    */
  s32 max          : 14;
  /*!
     The minimal valule
    */
  s32 min          : 14;
  /*!
     The current value
    */
  s32 curn;
  /*!
     The step
    */
  s32 step;
  /*!
     The pretfix
    */
  u32 prefix;
  /*!
     The postfix
    */
  u32 postfix;
} cbox_num_data_t;

/*!
   Dynamic combobox private data
  */
typedef struct
{
  /*!
     Combobox common data
    */
  cbox_comm_data_t common;
  /*!
     Dynamic combobox update callback
    */
  cbox_dync_update_t update;
  /*!
     Dynamic combobox content buffer
    */
  u16 *p_str_buf;
  /*!
     Reserved
    */
  u32 reserved;
} cbox_dync_data_t;

/*!
   Private data of combobox union.
  */
typedef union
{
  /*!
     Static cbox data.
    */
  cbox_static_data_t static_data;
  /*!
     Number cbox data.
    */
  cbox_num_data_t num_data;
  /*!
     Dynamic cbox data
    */
  cbox_dync_data_t dync_data;
}ctrl_cbox_union_data_t;

/*!
  combobox control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     String on show
    */
  u32 str_char;
  /*!
     String normal color
    */
  u32 n_fstyle;
  /*!
     String highlight color
    */
  u32 h_fstyle;
  /*!
     String gray color
    */
  u32 g_fstyle;
  /*!
     String left interval
    */
  u16 str_left;
  /*!
     String top interval
    */
  u16 str_top;

  /*!
     Drop-down list
    */
  control_t *p_droplist;

  /*!
     Drop-down list is opened or not
    */
  BOOL is_droplist_opened;

  /*!
     Combobox private data, decided by combobox type: dynamic, static or number
    */
  ctrl_cbox_union_data_t union_data;
}ctrl_cbox_t;


/*!
   Max string length of dynamic combobox.
  */
#define CBOX_DYNC_STR_LENGTH    32

/*!
   The index of the droplist of a combobox.
  */
#define IDC_CBOX_DROPLIST       0

/*!
   The index of the scrollbar of a combobox.
  */
#define IDC_CBOX_DROPLIST_SBAR  0


static cbox_static_data_t *_cbox_get_static_data(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = (ctrl_cbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & CBOX_WORKMODE_MASK) != CBOX_WORKMODE_STATIC)
  {
    return NULL;
  }

  return (cbox_static_data_t *)&p_cbox->union_data.static_data;
}


static cbox_num_data_t *_cbox_get_num_data(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = (ctrl_cbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & CBOX_WORKMODE_MASK) != CBOX_WORKMODE_NUMBER)
  {
    return NULL;
  }

  return (cbox_num_data_t *)&p_cbox->union_data.num_data;
}


static cbox_dync_data_t *_cbox_get_dync_data(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = (ctrl_cbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & CBOX_WORKMODE_MASK) != CBOX_WORKMODE_DYNAMIC)
  {
    return NULL;
  }

  return (cbox_dync_data_t *)&p_cbox->union_data.dync_data;
}


static cbox_comm_data_t *_cbox_get_common_data(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = (ctrl_cbox_t *)p_ctrl;

  if((p_ctrl->priv_attr & CBOX_WORKMODE_MASK) == CBOX_WORKMODE_STATIC
    || (p_ctrl->priv_attr & CBOX_WORKMODE_MASK) == CBOX_WORKMODE_DYNAMIC)
  {
    return (cbox_comm_data_t *)&p_cbox->union_data.static_data;
  }

  return NULL;
}


static void _cbox_update_curn(control_t *p_ctrl, u32 new_curn)
{
  cbox_comm_data_t *p_common_data = NULL;
  cbox_num_data_t *p_num_data = NULL;
  u32 orig_curn = 0;

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
    case CBOX_WORKMODE_DYNAMIC:
      p_common_data = _cbox_get_common_data(p_ctrl);
      orig_curn = p_common_data->focus;

      /* notify pre change */
      ctrl_process_msg(p_ctrl, MSG_CHANGING, orig_curn, new_curn);

      p_common_data->focus = (u16)new_curn;

      /* notify post change*/
      ctrl_process_msg(p_ctrl, MSG_CHANGED, orig_curn, new_curn);

      break;

    case CBOX_WORKMODE_NUMBER:
      p_num_data = _cbox_get_num_data(p_ctrl);
      orig_curn = p_num_data->curn;

      /* notify pre change */
      ctrl_process_msg(p_ctrl, MSG_CHANGING, orig_curn, new_curn);

      p_num_data->curn = (s32)new_curn;

      /* notify post change*/
      ctrl_process_msg(p_ctrl, MSG_CHANGED, orig_curn, new_curn);

      break;

    default:
      MT_ASSERT(0);
  }
}


static void _cbox_update_content(control_t *p_ctrl)
{
  cbox_comm_data_t *p_comm_data = NULL;
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_comm_data = _cbox_get_common_data(p_ctrl);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
    case CBOX_WORKMODE_DYNAMIC:
      p_cbox->str_char = cbox_get_content(p_ctrl, p_comm_data->focus);
      break;

    case CBOX_WORKMODE_NUMBER:
      p_cbox->str_char = p_cbox->union_data.num_data.curn;
      break;

    default:
      MT_ASSERT(0);
  }
}


static BOOL _cbox_common_set_focus(control_t *p_ctrl, u16 focus)
{
  cbox_comm_data_t *p_common_data = NULL;

  if((p_common_data = _cbox_get_common_data(p_ctrl)) == NULL)
  {
    return FALSE;
  }

  if((p_ctrl->priv_attr & CBOX_WORKMODE_MASK) == CBOX_WORKMODE_DYNAMIC)
  {
    if(focus >= p_common_data->total && p_common_data->total > 0)
    {
      return FALSE;
    }
  }
  else
  {
    if(focus >= p_common_data->total)
    {
      return FALSE;
    }
  }

//  if(focus == p_common_data->focus && focus != 0)
//  {
//    return TRUE;
//  }

  /* update */
  _cbox_update_curn(p_ctrl, focus);
  _cbox_update_content(p_ctrl);
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);

  return TRUE;
}


static u16 _cbox_common_get_focus(control_t *p_ctrl)
{
  cbox_comm_data_t *p_common_data = NULL;

  if((p_common_data = _cbox_get_common_data(p_ctrl)) == NULL)
  {
    return 0;
  }

  return (u16)p_common_data->focus;
}


static u16 _cbox_common_get_count(control_t *p_ctrl)
{
  cbox_comm_data_t *p_common_data = NULL;

  if((p_common_data = _cbox_get_common_data(p_ctrl)) == NULL)
  {
    return 0;
  }

  return (u16)p_common_data->total;
}


static u32 *_cbox_get_static_buf(control_t *p_ctrl, u16 pos, u32 type)
{
  cbox_static_data_t *p_static_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_static_data = _cbox_get_static_data(p_ctrl)) == NULL)
  {
    return NULL;
  }

  if((p_ctrl->priv_attr & CBOX_ITEM_STRTYPE_MASK) != type)
  {
    return NULL;
  }

  if(pos >= p_static_data->common.total)
  {
    return NULL;
  }

  return p_static_data->p_content + pos;
}


static void _cbox_set_static_content(control_t *p_ctrl,
                                     u16 pos,
                                     u32 type,
                                     u32 val)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _cbox_get_static_buf(p_ctrl, pos, type);
  if(p_temp == NULL)
  {
    return;
  }
  *p_temp = (u32)val;
}


static void _cbox_free_static_buf(control_t *p_ctrl)
{
  u16 i = 0;
  u32 *p_temp = NULL;
  cbox_static_data_t *p_static_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_static_data = _cbox_get_static_data(p_ctrl)) == NULL)
  {
    return;
  }
  for(i = 0; i < p_static_data->common.total; i++)
  {
    if((p_ctrl->priv_attr & CBOX_ITEM_STRTYPE_MASK) !=
       CBOX_ITEM_STRTYPE_UNICODE)
    {
      continue;
    }

    p_temp = p_static_data->p_content + i;
    ctrl_unistr_free((void *)*p_temp);
    *p_temp = 0;
  }
}


static BOOL _cbox_num_set_fix_by_strid(control_t *p_ctrl,
                                       BOOL is_prefix,
                                       u16 strid)
{
  cbox_num_data_t *p_num_data = NULL;

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return FALSE;
  }

  if(is_prefix)
  {
    if((p_ctrl->priv_attr & CBOX_ITEM_PREFIX_TYPE_MASK) !=
       CBOX_ITEM_PREFIX_TYPE_STRID)
    {
      return FALSE;
    }
    p_num_data->prefix = strid;
  }
  else
  {
    if((p_ctrl->priv_attr & CBOX_ITEM_POSTFIX_TYPE_MASK) !=
       CBOX_ITEM_POSTFIX_TYPE_STRID)
    {
      return FALSE;
    }
    p_num_data->postfix = strid;
  }
  return TRUE;
}


static BOOL _cbox_num_set_fix_by_extstr(control_t *p_ctrl,
                                        BOOL is_prefix,
                                        u16 *p_extstr)
{
  cbox_num_data_t *p_num_data = NULL;

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return FALSE;
  }

  if(is_prefix)
  {
    if((p_ctrl->priv_attr & CBOX_ITEM_PREFIX_TYPE_MASK) !=
       CBOX_ITEM_PREFIX_TYPE_EXTSTR)
    {
      return FALSE;
    }
    p_num_data->prefix = (u32)p_extstr;
  }
  else
  {
    if((p_ctrl->priv_attr & CBOX_ITEM_POSTFIX_TYPE_MASK) !=
       CBOX_ITEM_POSTFIX_TYPE_EXTSTR)
    {
      return FALSE;
    }
    p_num_data->postfix = (u32)p_extstr;
  }
  return TRUE;
}


static void _cbox_get_num_string(control_t *p_ctrl, u16 *p_buffer)
{
  u16 str_num[NUM_STRING_LEN + 1];
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return;
  }

  p_buffer[0] = '\0';

  switch(p_ctrl->priv_attr & CBOX_ITEM_NUMTYPE_MASK)
  {
    case CBOX_ITEM_NUMTYPE_DEC:
      if(p_num_data->bit_length > 0)
      {
        convert_i_to_dec_str_ex(str_num, p_num_data->curn,
                                (u8)p_num_data->bit_length);
      }
      else
      {
        convert_i_to_dec_str(str_num, p_num_data->curn);
      }
      break;
    case CBOX_ITEM_NUMTYPE_HEX:
      if(p_num_data->bit_length > 0)
      {
        convert_i_to_hex_str_ex(str_num, p_num_data->curn,
                                (u8)p_num_data->bit_length);
      }
      else
      {
        convert_i_to_hex_str(str_num, p_num_data->curn);
      }
      break;
    default:
      MT_ASSERT(0);
  }

  if(p_num_data->prefix != 0)
  {
    switch(p_ctrl->priv_attr & CBOX_ITEM_PREFIX_TYPE_MASK)
    {
      case CBOX_ITEM_PREFIX_TYPE_STRID:
        gui_get_string((u16)p_num_data->prefix, p_buffer,
                       CBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      case CBOX_ITEM_PREFIX_TYPE_EXTSTR:
        uni_strcat(p_buffer, (u16 *)p_num_data->prefix,
                   CBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      default:
        MT_ASSERT(0);
    }
  }

  uni_strcat(&p_buffer[uni_strlen(
                         p_buffer)], str_num, NUM_STRING_LEN +
             CBOX_ITEM_NUM_XFIX_LENGTH);

  if(p_num_data->postfix != 0)
  {
    switch(p_ctrl->priv_attr & CBOX_ITEM_POSTFIX_TYPE_MASK)
    {
      case CBOX_ITEM_POSTFIX_TYPE_STRID:
        gui_get_string((u16)p_num_data->postfix, &p_buffer[uni_strlen(
                                                             p_buffer)],
                       CBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      case CBOX_ITEM_POSTFIX_TYPE_EXTSTR:
        uni_strcat(&p_buffer[uni_strlen(
                               p_buffer)], (u16 *)p_num_data->postfix,
                   CBOX_ITEM_NUM_XFIX_LENGTH);
        break;
      default:
        MT_ASSERT(0);
    }
  }
}


static u32 _cbox_get_fstyle(u8 attr, ctrl_cbox_t *p_cbox)
{
  u32 font = 0;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_cbox->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_cbox->g_fstyle;
      break;

    default:
      font = p_cbox->n_fstyle;
  }

  return font;
}


static void _cbox_draw_content(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_cbox_t *p_cbox = NULL;
  rect_t temp_rect = {0};
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  u16 str_num[NUM_STRING_LEN + 1] = {0};
  u16 *p_num_str = NULL;

  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if(p_cbox->str_char == 0)
  {
    _cbox_update_content(p_ctrl);
  }

  //draw title string
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _cbox_get_fstyle(attr, p_cbox);

  ctrl_get_draw_rect(p_ctrl, &temp_rect);
  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      switch(p_ctrl->priv_attr & CBOX_ITEM_STRTYPE_MASK)
      {
        case CBOX_ITEM_STRTYPE_STRID:
          gui_draw_strid(hdc, &temp_rect, p_ctrl->priv_attr,
                         p_cbox->str_left, p_cbox->str_top,
                         0, (u16)p_cbox->str_char, font, STRDRAW_NORMAL);
          break;
        case CBOX_ITEM_STRTYPE_EXTSTR:
        case CBOX_ITEM_STRTYPE_UNICODE:
          gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                          p_cbox->str_left, p_cbox->str_top,
                          0, (u16 *)p_cbox->str_char, font, STRDRAW_NORMAL);
          break;
        case CBOX_ITEM_STRTYPE_DEC:
          convert_i_to_dec_str(str_num, p_cbox->str_char);
          gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                          p_cbox->str_left, p_cbox->str_top,
                          0, str_num, font, STRDRAW_NORMAL);
          break;
        case CBOX_ITEM_STRTYPE_HEX:
          convert_i_to_hex_str(str_num, p_cbox->str_char);
          gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                          p_cbox->str_left, p_cbox->str_top,
                          0, str_num, font, STRDRAW_NORMAL);
          break;
        default:
          ;
      }
      break;
    case CBOX_WORKMODE_NUMBER:
      p_num_str = ctrl_unistr_alloc(
        NUM_STRING_LEN + CBOX_ITEM_NUM_XFIX_LENGTH * 2);
      _cbox_get_num_string(p_ctrl, p_num_str);
      gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                      p_cbox->str_left, p_cbox->str_top,
                      0, p_num_str, font, STRDRAW_NORMAL);
      ctrl_unistr_free(p_num_str);
      p_num_str = NULL;
      break;
    case CBOX_WORKMODE_DYNAMIC:
      gui_draw_unistr(hdc, &temp_rect, p_ctrl->priv_attr,
                      p_cbox->str_left, p_cbox->str_top, 0,
                      (u16 *)(p_cbox->str_char), font, STRDRAW_NORMAL);
      break;
    default:
      MT_ASSERT(0);
  }
}


static control_t *_cbox_get_droplist(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cbox = (ctrl_cbox_t *)p_ctrl;
  
  return p_cbox->p_droplist;
}


static control_t *_cbox_get_scrollbar(control_t *p_cbox)
{
  control_t *p_list = _cbox_get_droplist(p_cbox);

  if(p_list != NULL)
  {
    return list_get_scrollbar(p_list);
  }

  return NULL;
}


static void _cbox_draw_droplist(control_t *p_ctrl)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    // chk, if list attach on cbox
    if(ctrl_get_parent(p_list) != p_ctrl)
    {
      ctrl_paint_ctrl(p_list, TRUE);
    }
  }
}


static void _cbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  MT_ASSERT(p_ctrl != NULL);

  gui_paint_frame(hdc, p_ctrl);
  //draw title
  _cbox_draw_content(p_ctrl, hdc);
}


static BOOL _cbox_calc_curn(control_t *p_ctrl,
                            BOOL is_increase,
                            u32 *p_new_curn)
{
  cbox_comm_data_t *p_common_data = NULL;
  cbox_num_data_t *p_num_data = NULL;
  u32 focus = 0;
  s32 num = 0;

  MT_ASSERT(p_ctrl != NULL);

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
    case CBOX_WORKMODE_DYNAMIC:
      p_common_data = _cbox_get_common_data(p_ctrl);
      if(p_common_data->total < 2)
      {
        return FALSE;
      }

      if(is_increase)
      {
        if(p_common_data->focus + 1 < p_common_data->total)
        {
          focus = p_common_data->focus + 1;
        }
        else if(p_ctrl->priv_attr & CBOX_CYCLE_MODE)
        {
          focus = 0;
        }
        else
        {
          return FALSE;
        }
      }
      else
      {
        if(p_common_data->focus > 0)
        {
          focus = p_common_data->focus - 1;
        }
        else if(p_ctrl->priv_attr & CBOX_CYCLE_MODE)
        {
          focus = p_common_data->total - 1;
        }
        else
        {
          return FALSE;
        }
      }
      
      *p_new_curn = focus;
      break;

    case CBOX_WORKMODE_NUMBER:
      p_num_data = _cbox_get_num_data(p_ctrl);

      if(is_increase)
      {
        num = p_num_data->curn + p_num_data->step;
        if(num > p_num_data->max)
        {
          if(p_ctrl->priv_attr & CBOX_CYCLE_MODE)
          {
            num = p_num_data->min;
          }
          else
          {
            num = p_num_data->max;
          }
        }
      }
      else
      {
        num = p_num_data->curn - p_num_data->step;
        if(num < p_num_data->min)
        {
          if(p_ctrl->priv_attr & CBOX_CYCLE_MODE)
          {
            num = p_num_data->max;
          }
          else
          {
            num = p_num_data->min;
          }
        }
      }
      *p_new_curn = (u32)num;
      break;

    default:
      MT_ASSERT(0);
  }

  return TRUE;
}


static void _cbox_increase(control_t *p_ctrl)
{
  u32 new_curn = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!_cbox_calc_curn(p_ctrl, TRUE, &new_curn))
  {
    // no change
    return;
  }

  /* update */
  _cbox_update_curn(p_ctrl, new_curn);
  _cbox_update_content(p_ctrl);

  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
}


static void _cbox_decrease(control_t *p_ctrl)
{
  u32 new_curn = 0;

  MT_ASSERT(p_ctrl != NULL);

  if(!_cbox_calc_curn(p_ctrl, FALSE, &new_curn))
  {
    // no change
    return;
  }

  /* update */
  _cbox_update_curn(p_ctrl, new_curn);
  _cbox_update_content(p_ctrl);

  ctrl_process_msg(p_ctrl, MSG_PAINT, TRUE, 0);
}


static void _cbox_free_data(control_t *p_ctrl)
{
  cbox_static_data_t *p_static_data = NULL;
  cbox_dync_data_t *p_dync_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      p_static_data = _cbox_get_static_data(p_ctrl);

      _cbox_free_static_buf(p_ctrl);
      if(p_static_data->p_content != NULL)
      {
        mmi_free_buf((void *)p_static_data->p_content);
        p_static_data->p_content = 0;
      }
      break;
    case CBOX_WORKMODE_NUMBER:
      /* do nothing */
      break;

    case CBOX_WORKMODE_DYNAMIC:
      p_dync_data = _cbox_get_dync_data(p_ctrl);
      if(p_dync_data->p_str_buf != NULL)
      {
        ctrl_unistr_free(p_dync_data->p_str_buf);
        p_dync_data->p_str_buf = NULL;
      }
      break;

    default:
      ;
  }
}


static void _cbox_update_cbox_focus(control_t *p_ctrl)
{
  u16 new_focus = 0;
  u32 new_num = 0;

  control_t *p_list = _cbox_get_droplist(p_ctrl);

  MT_ASSERT(p_list != NULL);

  new_focus = list_get_focus_pos(p_list);
  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      cbox_static_set_focus(p_ctrl, new_focus);
      break;
    case CBOX_WORKMODE_NUMBER:
      new_num = cbox_get_content(p_ctrl, new_focus);
      cbox_num_set_curn(p_ctrl, new_num);
      break;
    case CBOX_WORKMODE_DYNAMIC:
      cbox_dync_set_focus(p_ctrl, new_focus);
      break;
    default:
      MT_ASSERT(0);
  }
}


static u16 _cbox_calc_droplist_count(control_t *p_cbox)
{
  u16 count = 0;
  cbox_comm_data_t *p_comm_data = NULL;
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_cbox != NULL);

  switch(p_cbox->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
    case CBOX_WORKMODE_DYNAMIC:
      p_comm_data = _cbox_get_common_data(p_cbox);
      count = p_comm_data->total;
      break;

    case CBOX_WORKMODE_NUMBER:
      p_num_data = _cbox_get_num_data(p_cbox);
      count =
        (u16)((p_num_data->max - p_num_data->min +
               p_num_data->step) / p_num_data->step);
      break;

    default:
      MT_ASSERT(0);
      return 0;
  }

  return count;
}


static void _cbox_update_droplist_focus(control_t *p_ctrl)
{
  cbox_comm_data_t *p_comm_data = NULL;
  cbox_num_data_t *p_num_data = NULL;
  u32 focus = 0;

  control_t *p_list = _cbox_get_droplist(p_ctrl);

  MT_ASSERT(p_list != NULL);

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
    case CBOX_WORKMODE_DYNAMIC:
      p_comm_data = _cbox_get_common_data(p_ctrl);
      focus = p_comm_data->focus;
      break;

    case CBOX_WORKMODE_NUMBER:
      p_num_data = _cbox_get_num_data(p_ctrl);
      focus = (p_num_data->curn - p_num_data->min) / p_num_data->step;
      break;

    default:
      MT_ASSERT(0);
      return;
  }

  list_set_focus_pos(p_list, focus);
}


static RET_CODE _cbox_droplist_update(control_t *p_list,
                                      u16 start,
                                      u16 size,
                                      u32 context)
{
  control_t *p_cbox = (control_t *)context;
  u32 item_content = 0;
  u16 i = 0, pos = 0, count = 0;

  MT_ASSERT(p_cbox != NULL);

  count = list_get_count(p_list);
  for(i = 0; i < size; i++)
  {
    pos = i + start;
    if(pos < count)
    {
      item_content = cbox_get_content(p_cbox, pos);
      switch(p_cbox->priv_attr & CBOX_WORKMODE_MASK)
      {
        case CBOX_WORKMODE_STATIC:
          switch(p_cbox->priv_attr & CBOX_ITEM_STRTYPE_MASK)
          {
            case CBOX_ITEM_STRTYPE_STRID:
              list_set_field_content_by_strid(p_list, pos, 0, 
                  (u16)item_content);
              break;
            case CBOX_ITEM_STRTYPE_EXTSTR:
            case CBOX_ITEM_STRTYPE_UNICODE:
              list_set_field_content_by_unistr(p_list,
                                               pos,
                                               0,
                                               (u16 *)item_content);
              break;
            case CBOX_ITEM_STRTYPE_DEC:
              list_set_field_content_by_dec(p_list, pos, 0, item_content);
              break;
            case CBOX_ITEM_STRTYPE_HEX:
              list_set_field_content_by_hex(p_list, pos, 0, item_content);
              break;
            default:
              MT_ASSERT(0);
          }
          break;
        case CBOX_WORKMODE_NUMBER:
          list_set_field_content_by_dec(p_list, pos, 0, item_content);
          break;
        case CBOX_WORKMODE_DYNAMIC:
          list_set_field_content_by_unistr(p_list, pos, 0, (u16 *)item_content);
          break;
        default:
          MT_ASSERT(0);
      }
    }
  }
  return SUCCESS;
}


s32 cbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_cbox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;
  
  // initialize the default control of combobox class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER | CBOX_STATUS_NORMAL;
  p_default_ctrl->p_proc = cbox_class_proc;
  p_default_ctrl->p_paint = _cbox_draw;

  // initalize the default data of combobox class

  if(ctrl_register_ctrl_class(CTRL_CBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


void cbox_set_font_style(control_t *p_ctrl,
                         u32 n_fstyle,
                         u32 h_fstyle,
                         u32 g_fstyle)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cbox = (ctrl_cbox_t *)p_ctrl;
  p_cbox->n_fstyle = n_fstyle;
  p_cbox->h_fstyle = h_fstyle;
  p_cbox->g_fstyle = g_fstyle;
}

void cbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);

  p_cbox = (ctrl_cbox_t *)p_ctrl;

  *p_factive = p_cbox->n_fstyle;
  *p_fhlight = p_cbox->h_fstyle;
  *p_finactive = p_cbox->g_fstyle;
}

void cbox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cbox = (ctrl_cbox_t *)p_ctrl;
  p_cbox->str_left = left;
  p_cbox->str_top = top;
}


void cbox_set_align_style(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_STRING_ALIGN_MASK);         //clean old style
  p_ctrl->priv_attr |= (style & CBOX_STRING_ALIGN_MASK);  //set new style
}

u32 cbox_get_align_style(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & CBOX_STRING_ALIGN_MASK);
}


void cbox_enable_cycle_mode(control_t *p_ctrl, BOOL is_enable)
{
  MT_ASSERT(p_ctrl != NULL);

  if(is_enable)
  {
    p_ctrl->priv_attr |= CBOX_CYCLE_MODE;
  }
  else
  {
    p_ctrl->priv_attr &= ~CBOX_CYCLE_MODE;
  }
}

BOOL cbox_is_cycle_mode(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (BOOL)(p_ctrl->priv_attr & CBOX_CYCLE_MODE);
}

void cbox_set_work_mode(control_t *p_ctrl, u32 mode)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_WORKMODE_MASK);         //clean old style
  p_ctrl->priv_attr |= (mode & CBOX_WORKMODE_MASK);   //set new style
}


u32 cbox_get_work_mode(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (u32)(p_ctrl->priv_attr & CBOX_WORKMODE_MASK);
}


control_t *cbox_create_droplist(control_t *p_ctrl,
                                u16 page,
                                u16 sbar_width,
                                u16 sbar_gap)
{
  ctrl_cbox_t *p_cbox = NULL;
  control_t *p_sbar = NULL;
  control_t *p_root = NULL;
  rect_t rc_cbox, rc_root;
  u16 list_width = 0, list_height = 0;
  u16 sbar_left = 0, sbar_top = 0;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  ctrl_get_frame(p_ctrl, &rc_cbox);
  // check coordinate
  MT_ASSERT((RECTW(rc_cbox) > sbar_gap + sbar_width)
           && (RECTH(rc_cbox) * page > 2 * sbar_gap));

  list_width = RECTW(rc_cbox);
  list_height = RECTH(rc_cbox) * page;

  p_root = ctrl_get_root(p_ctrl);
  ctrl_get_frame(p_root, &rc_root);
  ctrl_client2screen(p_root, &rc_root);
  ctrl_client2screen(p_ctrl, &rc_cbox);
  
  p_cbox->p_droplist = ctrl_create_ctrl(CTRL_LIST, IDC_CBOX_DROPLIST,
    rc_cbox.left - rc_root.left, rc_cbox.top - rc_root.top,
    list_width, list_height, p_root, 0);
  MT_ASSERT(p_cbox->p_droplist != NULL);

  if(p_cbox->p_droplist != NULL)
  {
    // set attr
    list_set_count(p_cbox->p_droplist, _cbox_calc_droplist_count(p_ctrl), page);
    list_set_field_count(p_cbox->p_droplist, 1, page);
    list_set_update(p_cbox->p_droplist, _cbox_droplist_update, (u32)p_ctrl);

    ctrl_set_sts(p_cbox->p_droplist, OBJ_STS_HIDE);

    // add scrollbar
    if(sbar_width > 0)
    {
      sbar_left = list_width - 2 * sbar_gap
        - sbar_width + rc_cbox.left - rc_root.left;
      sbar_top = sbar_gap + rc_cbox.top - rc_root.top;
      p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_CBOX_DROPLIST_SBAR,
                                sbar_left, sbar_top,
                                sbar_width, list_height - 2 * sbar_gap,
                                p_root, 0);
      MT_ASSERT(p_sbar != NULL);

      sbar_set_autosize_mode(p_sbar, TRUE);
      sbar_set_direction(p_sbar, FALSE);
      ctrl_set_sts(p_sbar, OBJ_STS_HIDE);

      list_set_scrollbar(p_cbox->p_droplist, p_sbar);
    }
  }

  return p_cbox->p_droplist;
}


void cbox_droplist_set_rstyle(control_t *p_ctrl,
                              u8 sh_style,
                              u8 hl_style,
                              u8 gr_style)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    ctrl_set_rstyle(p_list, sh_style, hl_style, gr_style);
  }
}

void cbox_droplist_set_cycle_mode(control_t *p_ctrl, BOOL is_enable)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_enable_cycle_mode(p_list, is_enable);
  }
}

void cbox_droplist_set_page_mode(control_t *p_ctrl, BOOL is_enable)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_enable_page_mode(p_list, is_enable);
  }
}

void cbox_droplist_set_mid_rect(control_t *p_ctrl,
                                s16 left,
                                s16 top,
                                u16 width,
                                u16 height,
                                u8 interval)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);
  
  if(p_list != NULL)
  {
    ctrl_set_mrect(p_list, left, top, left + width, top + height);
    list_set_item_interval(p_list, interval);
  }
}


void cbox_droplist_set_item_rstyle(control_t *p_ctrl, list_xstyle_t *p_style)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_set_item_rstyle(p_list, p_style);
  }
}


void cbox_droplist_set_field_attr(control_t *p_ctrl,
                                  u32 align_type,
                                  u16 width,
                                  u16 left,
                                  u16 top)
{
  u32 field_attr = 0;
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
    {
      case CBOX_WORKMODE_STATIC:
        switch(p_ctrl->priv_attr & CBOX_ITEM_STRTYPE_MASK)
        {
          case CBOX_ITEM_STRTYPE_STRID:
            field_attr = LISTFIELD_TYPE_STRID;
            break;
          case CBOX_ITEM_STRTYPE_EXTSTR:
          case CBOX_ITEM_STRTYPE_UNICODE:
            field_attr = LISTFIELD_TYPE_UNISTR;
            break;
          case CBOX_ITEM_STRTYPE_DEC:
            field_attr = LISTFIELD_TYPE_DEC;
            break;
          case CBOX_ITEM_STRTYPE_HEX:
            field_attr = LISTFIELD_TYPE_HEX;
            break;
          default:
            ;
        }
        break;
      case CBOX_WORKMODE_NUMBER:
        field_attr = LISTFIELD_TYPE_DEC;
        break;
      case CBOX_WORKMODE_DYNAMIC:
        field_attr = LISTFIELD_TYPE_UNISTR;
        break;
      default:
        MT_ASSERT(0);
    }

    field_attr |= align_type;
    list_set_field_attr(p_list, 0, field_attr, width, left, top);
  }
}


void cbox_droplist_set_field_rect_style(control_t *p_ctrl,
                                        list_xstyle_t *p_style)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_set_field_rect_style(p_list, 0, p_style);
  }
}


void cbox_droplist_set_field_font_style(control_t *p_ctrl,
                                        list_xstyle_t *p_style)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_set_field_font_style(p_list, 0, p_style);
  }
}


void cbox_attach_droplist(control_t *p_ctrl, control_t *p_droplist)
{
  ctrl_cbox_t *p_cbox = NULL;
  u16 page = 0;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  p_cbox->p_droplist = p_droplist;
  if(p_cbox->p_droplist != NULL)
  {
    page = list_get_page(p_cbox->p_droplist);
    list_set_count(p_cbox->p_droplist, _cbox_calc_droplist_count(p_ctrl), page);
    ctrl_set_sts(p_cbox->p_droplist, OBJ_STS_HIDE);
  }
}


void cbox_droplist_set_update(control_t *p_ctrl,
                              list_update_t p_cb,
                              u32 context)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if(p_list != NULL)
  {
    list_set_update(p_list, p_cb, context);
  }
}


void cbox_droplist_set_sbar_rstyle(control_t *p_ctrl,
                                   u8 sh_style,
                                   u8 hl_style,
                                   u8 gr_style)
{
  control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);

  if(p_sbar != NULL)
  {
    ctrl_set_rstyle(p_sbar, sh_style, hl_style, gr_style);
  }
}


void cbox_droplist_set_sbar_mid_rect(control_t *p_ctrl,
                                     s16 left,
                                     s16 top,
                                     s16 right,
                                     s16 bottom)
{
  control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);
  
  if(p_sbar != NULL)
  {
    ctrl_set_mrect(p_sbar, left, top, right, bottom);
  }
}


void cbox_droplist_set_sbar_mid_rstyle(control_t *p_ctrl,
                                       u8 sh_style,
                                       u8 hl_style,
                                       u8 gr_style)
{
  control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);

  if(p_sbar != NULL)
  {
    sbar_set_mid_rstyle(p_sbar, sh_style, hl_style, gr_style);
  }
}


BOOL cbox_droplist_is_opened(control_t *p_ctrl)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if (p_cbox->p_droplist != NULL)
  {
    return p_cbox->is_droplist_opened;
  }

  return FALSE;
}


BOOL cbox_open_droplist(control_t *p_ctrl)
{
  list_update_t p_update = NULL;
  u32 context = 0;
  control_t *p_list = _cbox_get_droplist(p_ctrl);
  control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);
  u16 page = 0, count = 0;
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if(p_list == NULL)
  {
    return FALSE;
  }

  ctrl_process_msg(p_ctrl, MSG_OPEN_LIST, 0, 0);

  p_ctrl->priv_attr &= (~CBOX_STATUS_MASK);
  p_ctrl->priv_attr |= CBOX_STATUS_LIST;

  ctrl_set_attr(p_list, OBJ_ATTR_HL);
  ctrl_set_sts(p_list, OBJ_STS_SHOW);

  // update list with cbox
  page = list_get_page(p_list);
  count = _cbox_calc_droplist_count(p_ctrl);

  list_set_count(p_list, count, page);
  _cbox_update_droplist_focus(p_ctrl);

  // update list content
  p_update = list_get_update(p_list, &context);
  if(p_update != NULL)
  {
    p_update(p_list,
             list_get_valid_pos(p_list),
             list_get_page(p_list),
             context);
  }

  ctrl_add_rect_to_invrgn(p_list, NULL);

  if(p_sbar != NULL)
  {
    ctrl_set_attr(p_sbar, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_sbar, OBJ_STS_SHOW);
    ctrl_add_rect_to_invrgn(p_sbar, NULL);
  }

  p_cbox->is_droplist_opened = TRUE;

  return TRUE;
}


BOOL cbox_close_droplist(control_t *p_ctrl, BOOL is_update)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);
  control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);
  control_t *p_root = NULL;

  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if(p_list == NULL)
  {
    return FALSE;
  }

  p_ctrl->priv_attr &= (~CBOX_STATUS_MASK);
  p_ctrl->priv_attr |= CBOX_STATUS_NORMAL;

  // hidden list
  ctrl_set_sts(p_list, OBJ_STS_HIDE);
  ctrl_hide_ctrl(p_list);

  if(p_sbar != NULL)
  {
    ctrl_set_sts(p_sbar, OBJ_STS_HIDE);
    ctrl_hide_ctrl(p_sbar);
  }

  p_root = ctrl_get_root(p_ctrl);
  
  ctrl_process_msg(p_root, MSG_PAINT, 0, 0);
  

  ctrl_process_msg(p_ctrl, MSG_CLOSE_LIST, 0, 0);

  // update focus
  if(is_update)
  {
    _cbox_update_cbox_focus(p_ctrl);
  }
  else
  {
    _cbox_update_content(p_ctrl);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  p_cbox->is_droplist_opened = FALSE;
  
  return TRUE;
}


BOOL cbox_update_droplist(control_t *p_ctrl)
{
  list_update_t p_update = NULL;
  u32 context = 0;
  control_t *p_list = _cbox_get_droplist(p_ctrl);
  //control_t *p_sbar = _cbox_get_scrollbar(p_ctrl);
  u16 page = 0, count = 0;
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if(p_list == NULL)
  {
    return FALSE;
  }

  if(!(p_ctrl->priv_attr & CBOX_STATUS_LIST))
  {
    return FALSE;
  }

  // update list with cbox
  page = list_get_page(p_list);
  count = _cbox_calc_droplist_count(p_ctrl);

  list_set_count(p_list, count, page);
  _cbox_update_droplist_focus(p_ctrl);

  // update list content
  p_update = list_get_update(p_list, &context);
  if(p_update != NULL)
  {
    p_update(p_list,
             list_get_valid_pos(p_list),
             list_get_page(p_list),
             context);
  }

  return TRUE;
}


u32 cbox_get_content(control_t *p_ctrl, u16 pos)
{
  cbox_static_data_t *p_static_data = NULL;
  cbox_num_data_t *p_num_data = NULL;
  cbox_dync_data_t *p_dync_data = NULL;

  u32 content = 0;
  s32 range = 0;

  MT_ASSERT(p_ctrl != NULL);

  switch(p_ctrl->priv_attr & CBOX_WORKMODE_MASK)
  {
    case CBOX_WORKMODE_STATIC:
      p_static_data = _cbox_get_static_data(p_ctrl);
      if(p_static_data->p_content != 0
        && pos < p_static_data->common.total)
      {
        content = *(p_static_data->p_content + pos);
      }
      break;

    case CBOX_WORKMODE_NUMBER:
      p_num_data = _cbox_get_num_data(p_ctrl);
      range =
        (p_num_data->max - p_num_data->min +
         p_num_data->step) / p_num_data->step;
      if(pos < range)
      {
        content = p_num_data->min + pos * p_num_data->step;
      }
      break;

    case CBOX_WORKMODE_DYNAMIC:
      p_dync_data = _cbox_get_dync_data(p_ctrl);
      if(p_dync_data->p_str_buf == NULL
        || p_dync_data->update == NULL
        || (pos >= p_dync_data->common.total && p_dync_data->common.total > 0))
      {
        break;
      }
      p_dync_data->update(p_ctrl, pos,
                          p_dync_data->p_str_buf,
                          CBOX_DYNC_STR_LENGTH);
      content = (u32)p_dync_data->p_str_buf;
      break;

    default:
      MT_ASSERT(0);
  }

  return content;
}


BOOL cbox_static_set_count(control_t *p_ctrl, u16 total)
{
  cbox_static_data_t *p_static_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_static_data = _cbox_get_static_data(p_ctrl)) == NULL)
  {
    return FALSE;
  }

//  if(p_static_data->content) //count cannot be init 2 times
//  {
//    return FALSE;
//  }
// free at first
  _cbox_free_data(p_ctrl);

  p_static_data->common.total = total;
  p_static_data->p_content =
    mmi_realloc_buf(p_static_data->p_content, sizeof(u32) * total);
  MT_ASSERT(p_static_data->p_content != NULL);

  if(p_static_data->p_content != NULL)
  {
    memset(p_static_data->p_content, 0, sizeof(u32) * total);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


u16 cbox_static_get_count(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_get_count(p_ctrl);
}


BOOL cbox_static_set_focus(control_t *p_ctrl, u16 focus)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_set_focus(p_ctrl, focus);
}


u16 cbox_static_get_focus(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_get_focus(p_ctrl);
}


void cbox_static_set_content_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_ITEM_STRTYPE_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & CBOX_ITEM_STRTYPE_MASK);   //set new type
}


BOOL cbox_static_set_content_by_unistr(control_t *p_ctrl,
                                       u16 pos,
                                       u16 *p_unistr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _cbox_get_static_buf(p_ctrl, pos, CBOX_ITEM_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, uni_strlen(p_unistr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  uni_strcpy((u16 *)(*p_temp), p_unistr);

  return TRUE;
}


BOOL cbox_static_set_content_by_ascstr(control_t *p_ctrl, u16 pos, u8 *p_ascstr)
{
  u32 *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_temp = _cbox_get_static_buf(p_ctrl, pos, CBOX_ITEM_STRTYPE_UNICODE);
  if(p_temp == NULL)
  {
    return FALSE;
  }

  *p_temp = (u32)ctrl_unistr_realloc((void *)*p_temp, strlen((void *)p_ascstr));
  if((*p_temp) == 0)
  {
    return FALSE;
  }
  str_asc2uni(p_ascstr, (u16 *)(*p_temp));

  return TRUE;
}


void cbox_static_set_content_by_extstr(control_t *p_ctrl,
                                       u16 pos,
                                       u16 *p_extstr)
{
  _cbox_set_static_content(p_ctrl,
                           pos, CBOX_ITEM_STRTYPE_EXTSTR, (u32)p_extstr);
}


void cbox_static_set_content_by_strid(control_t *p_ctrl, u16 pos, u16 strid)
{
  _cbox_set_static_content(p_ctrl, pos, CBOX_ITEM_STRTYPE_STRID, (u32)strid);
}


void cbox_static_set_content_by_hex(control_t *p_ctrl, u16 pos, s32 num)
{
  _cbox_set_static_content(p_ctrl, pos, CBOX_ITEM_STRTYPE_HEX, (u32)num);
}


void cbox_static_set_content_by_dec(control_t *p_ctrl, u16 pos, s32 num)
{
  _cbox_set_static_content(p_ctrl, pos, CBOX_ITEM_STRTYPE_DEC, (u32)num);
}


void cbox_num_set_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_ITEM_NUMTYPE_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & CBOX_ITEM_NUMTYPE_MASK);   //set new type
}


void cbox_num_set_range(control_t *p_ctrl,
                        s32 min,
                        s32 max,
                        u32 step,
                        u8 bit_length)
{
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return;
  }

  p_num_data->min = min;
  p_num_data->max = max;
  p_num_data->step = step;
  p_num_data->bit_length = bit_length;
}


void cbox_num_get_range(control_t *p_ctrl, s32 *p_min, s32 *p_max, u32 *p_inc)
{
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return;
  }

  *p_min = p_num_data->min;
  *p_max = p_num_data->max;
  *p_inc = p_num_data->step;
}


void cbox_num_set_curn(control_t *p_ctrl, s32 value)
{
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return;
  }

  if(p_num_data->curn == value && value != 0)
  {
    return;
  }

  /* update */
  _cbox_update_curn(p_ctrl, (u32)value);
  _cbox_update_content(p_ctrl);
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
}


s32 cbox_num_get_curn(control_t *p_ctrl)
{
  cbox_num_data_t *p_num_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_num_data = _cbox_get_num_data(p_ctrl)) == NULL)
  {
    return -1;
  }

  return p_num_data->curn;
}


void cbox_num_set_prefix_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_ITEM_PREFIX_TYPE_MASK);         //clean old type
  p_ctrl->priv_attr |= (type & CBOX_ITEM_PREFIX_TYPE_MASK);   //set new type
}


BOOL cbox_num_set_prefix_by_strid(control_t *p_ctrl, u16 strid)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_num_set_fix_by_strid(p_ctrl, TRUE, strid);
}


BOOL cbox_num_set_postfix_by_strid(control_t *p_ctrl, u16 strid)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_num_set_fix_by_strid(p_ctrl, FALSE, strid);
}


void cbox_num_set_postfix_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~CBOX_ITEM_POSTFIX_TYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & CBOX_ITEM_POSTFIX_TYPE_MASK);    //set new type
}


BOOL cbox_num_set_prefix_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_num_set_fix_by_extstr(p_ctrl, TRUE, p_extstr);
}


BOOL cbox_num_set_postfix_by_extstr(control_t *p_ctrl, u16 *p_extstr)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_num_set_fix_by_extstr(p_ctrl, FALSE, p_extstr);
}


void cbox_dync_set_update(control_t *p_ctrl, cbox_dync_update_t p_callback)
{
  cbox_dync_data_t *p_dync_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_dync_data = _cbox_get_dync_data(p_ctrl)) == NULL)
  {
    return;
  }

  p_dync_data->update = p_callback;

  /*alloc buf for string*/
  p_dync_data->p_str_buf = ctrl_unistr_realloc(p_dync_data->p_str_buf,
                                               CBOX_DYNC_STR_LENGTH);
}


BOOL cbox_dync_set_count(control_t *p_ctrl, u16 total)
{
  cbox_dync_data_t *p_dync_data = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_dync_data = _cbox_get_dync_data(p_ctrl)) == NULL)
  {
    return FALSE;
  }

  p_dync_data->common.total = total;

  return TRUE;
}


u16 cbox_dync_get_count(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_get_count(p_ctrl);
}


BOOL cbox_dync_set_focus(control_t *p_ctrl, u16 focus)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_set_focus(p_ctrl, focus);
}


u16 cbox_dync_get_focus(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
  return _cbox_common_get_focus(p_ctrl);
}


control_t *cbox_droplist_get(control_t *p_ctrl)
{
  control_t *p_list = NULL;

  p_list = _cbox_get_droplist(p_ctrl);
  
  return p_list;
}


static RET_CODE on_cbox_destroy(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  _cbox_free_data(p_ctrl);

  // return ERR_NOFEATURE and process MSG_DESTROY by ctrl_default_proc
  return ERR_NOFEATURE;
}


static RET_CODE on_cbox_increase(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & CBOX_STATUS_MASK) == CBOX_STATUS_NORMAL)
  {
    _cbox_increase(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_cbox_decrease(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & CBOX_STATUS_MASK) == CBOX_STATUS_NORMAL)
  {
    _cbox_decrease(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_cbox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ctrl_cbox_t *p_cbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  p_cbox = (ctrl_cbox_t *)p_ctrl;

  if(!(p_ctrl->priv_attr & CBOX_STATUS_LIST))
  {
    if(cbox_open_droplist(p_ctrl))
    {
      ctrl_process_msg(p_cbox->p_droplist, MSG_PAINT, FALSE, 0);
    }
  }
  else
  {
    if(cbox_close_droplist(p_ctrl, TRUE))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
  }
  return SUCCESS;
}


static RET_CODE on_cbox_paint(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t rc_client, rc_list, rc_affected;
  hdc_t parent_dc = (hdc_t)(para2);

  control_t *p_list = _cbox_get_droplist(p_ctrl);

  if((p_ctrl->priv_attr & CBOX_STATUS_MASK) == CBOX_STATUS_LIST)
  {
    if(p_list != NULL)
    {
      ctrl_get_frame(p_ctrl, &rc_client);
      ctrl_client2screen(p_ctrl, &rc_client);

      ctrl_get_frame(p_list, &rc_list);
      ctrl_client2screen(p_list, &rc_list);

      if(intersect_rect(&rc_affected, &rc_client, &rc_list))
      {
        if(ctrl_screen2client(p_list, &rc_affected))
        {
          ctrl_subtract_rect_from_invrgn(p_ctrl, &rc_affected);
        }
      }
    }
  }

  //reset content.
  cbox_get_content(p_ctrl, _cbox_common_get_focus(p_ctrl));

  ctrl_default_proc(p_ctrl, msg, para1, para2);

  //this bar isn't list's child, when parent_dc isn't 0,
  //parent control will draw bar
  if(parent_dc == HDC_INVALID)
  {
    _cbox_draw_droplist(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_cbox_droplist(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *p_list = _cbox_get_droplist(p_ctrl);
  msgproc_t p_proc = NULL;

  if((p_ctrl->priv_attr & CBOX_STATUS_MASK) == CBOX_STATUS_LIST)
  {
    if(p_list != NULL)
    {
      p_proc = ctrl_get_proc(p_list);
      if(p_proc != NULL)
      {
        return p_proc(p_list, msg, para1, para2);
      }
    }
  }

  return ERR_NOFEATURE;
}


static RET_CODE on_cbox_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & CBOX_STATUS_MASK) == CBOX_STATUS_LIST)
  {
    if(cbox_close_droplist(p_ctrl, FALSE))
    {
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }

    return SUCCESS;
  }

  return ERR_NOFEATURE;
}

// define the default msgmap of class
BEGIN_CTRLPROC(cbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_PAINT, on_cbox_paint)
ON_COMMAND(MSG_INCREASE, on_cbox_increase)
ON_COMMAND(MSG_DECREASE, on_cbox_decrease)
ON_COMMAND(MSG_SELECT, on_cbox_select)
ON_COMMAND(MSG_CANCEL, on_cbox_cancel)
ON_COMMAND(MSG_FOCUS_UP, on_cbox_droplist)
ON_COMMAND(MSG_FOCUS_DOWN, on_cbox_droplist)
ON_COMMAND(MSG_PAGE_UP, on_cbox_droplist)
ON_COMMAND(MSG_PAGE_DOWN, on_cbox_droplist)
ON_COMMAND(MSG_DESTROY, on_cbox_destroy)
END_CTRLPROC(cbox_class_proc, ctrl_default_proc)

