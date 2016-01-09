/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_ipbox.c
   this file  implement the functions defined in  ctrl_ipbox.h, also it
   implement some internal used   function. All these functions are about how to
   decribe, set and draw a ip control.
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
#include "lib_util.h"
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
#include "mt_time.h"
#include "ctrl_ipbox.h"

/*!
  ipbox control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;
  /*!
     ip address 
    */
  ip_address_t ctrl_address;
  /*!
     Current bit
    */
  u8 curn_bit;
  /*!
     Current item.
    */
  u8 curn_item;
  /*!
     String normal color
    */
  u32 n_fstyle;
  /*!
     String highlight color
    */
  u32 h_fstyle;
  /*!
     String color in edit state
    */
  u32 e_fstyle;
  /*!
     String gray color
    */
  u32 g_fstyle;
  /*!
     Separator
    */
  u32 separator[IPBOX_MAX_ITEM_NUM];
  /*!
     Distance from left side of textfield to left side of string.
    */
  u16 str_left;
  /*!
     Distance from top side of textfield to top side of string.
    */
  u16 str_top;
  /*!
     Max number width.
    */
  u8 max_num_width;  
}ctrl_ipbox_t;

/*!
  Macro of NOT
  */
#define NOT(x) ((x) == 0)
/*!
  Macro of noninline
  */
#if defined(NXP)
/*!
  Macro for noninline function:no need in NXP
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#elif defined(WIN32)
/*!
  Macro for noninline function:no need in x86
  */
#define NONINLINE
/*!
  Macro of noninline
  */
#else
/*!
  Macro for noninline function
  */
#define NONINLINE __attribute__ ((noinline))
#endif


/*!
   get base of a certain bit.

   \param[in] bits			: to specified a bit.
   \return					: base of the specified bit.
  */
static NONINLINE u32 _ipbox_get_base(u32 bits)
{
  u32 i = 0, base = 1;

  for(i = 0; i < bits; i++)
  {
    base = base * 10;
  }
  return base;
}


/*!
   get the number of current bit.

   \param[in] p_ipbox			: private data of nbox.
   \return					: the number of current bit.
  */
static u8 _ipbox_get_curn_bit(control_t *p_ctrl);

/*!
   set the number of current bit.

   \param[in] p_ipbox			: private data of nbox.
   \param[in] num			: number to be set.
   \return					: the number of current bit.
  */
static void _ipbox_set_curn_bit(control_t *p_ctrl, u8 num);

/*!
   increase the number of current bit.

   \param[in] ctrl				: nbox control.
   \return					: TRUE for success, else return FALSE.
  */
static BOOL _num_increase(control_t *p_ctrl);

/*!
   decrease the number of current bit.

   \param[in] ctrl				: nbox control.
   \return					: TRUE for success, else return FALSE.
  */
static BOOL _num_decrease(control_t *p_ctrl);

static void _ipbox_draw(control_t *p_ctrl, hdc_t hdc);

static u16 _ipbox_get_total_width(control_t *p_ctrl);

static u32 _ipbox_get_fstyle(u8 attr, ctrl_ipbox_t *p_ipbox);

RET_CODE ipbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_ipbox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;

  // initialize the default control of ipbox class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER |
                              IPBOX_S_B1| IPBOX_S_B2 | IPBOX_S_B3 |IPBOX_S_B4;
  p_default_ctrl->p_proc = ipbox_class_proc;
  p_default_ctrl->p_paint = _ipbox_draw;

  // initalize the default data of ipbox class

  if(ctrl_register_ctrl_class(CTRL_IPBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static u32 _get_draw_style(ctrl_ipbox_t *p_ipbox, u8 item_index)
{
  if(item_index == p_ipbox->curn_item)
  {
    return MAKE_DRAW_STYLE(STRDRAW_WITH_UNDERLINE,
                           p_ipbox->curn_bit);
  }
  else
  {
    return MAKE_DRAW_STYLE(STRDRAW_NORMAL,
                           0);
  }
}


static u8 _ipbox_get_item_bit(control_t *p_ctrl, u8 index)
{
  u32 ipbox_attr[IPBOX_MAX_ITEM_NUM] =
  {
    IPBOX_S_B1, IPBOX_S_B2, IPBOX_S_B3,
    IPBOX_S_B4
  };
  u8 item_bit = 0;

  if(p_ctrl->priv_attr & ipbox_attr[index])
  {
    item_bit =  3;
  }

  return item_bit;
}


static s32 _ipbox_shift_offset(control_t *p_ctrl, s32 offset)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  if(offset > 0) //move left
  {
    if(offset > p_ipbox->curn_bit) //out of curn item
    {
      offset -= p_ipbox->curn_bit;

      do
      {
        if(p_ipbox->curn_item == 0)
        {
          p_ipbox->curn_item = (IPBOX_MAX_ITEM_NUM - 1);
        }
        else
        {
          p_ipbox->curn_item--;
        }
      }
      while(_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item) == 0);


      p_ipbox->curn_bit = (_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item) - 1);

      (offset--);

      return offset;
    }
    else //move in curn item
    {
      p_ipbox->curn_bit -= offset;

      return 0;
    }
  }
  else if(offset < 0)
  {
    offset = (0 - offset);

    if(offset > (_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item)
                 - (p_ipbox->curn_bit + 1))) //out of current item.
    {
      offset -= (_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item)
                 - (p_ipbox->curn_bit + 1));

      do
      {
        if(p_ipbox->curn_item == (IPBOX_MAX_ITEM_NUM - 1))
        {
          p_ipbox->curn_item = 0;
        }
        else
        {
          p_ipbox->curn_item++;
        }
      }
      while(_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item) == 0);

      p_ipbox->curn_bit = 0;

      (offset--);

      return 0 - offset;
    }
    else
    {
      p_ipbox->curn_bit += offset;

      return 0;
    }
  }
  return 0;
}


static u8 _ipbox_get_curn_bit(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;
  u16 data = 0;

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  switch(p_ipbox->curn_item)
  {
    case 0: //s_b1
      data = p_ipbox->ctrl_address.s_b1;
      break;
    case 1: //s_b2
      data = p_ipbox->ctrl_address.s_b2;
      break;
    case 2: //s_b3
      data = p_ipbox->ctrl_address.s_b3;
      break;
    case 3: //s_b4
      data = p_ipbox->ctrl_address.s_b4;
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  return (u8)(data %
              _ipbox_get_base(_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item)
                             - p_ipbox->curn_bit) /
              _ipbox_get_base(_ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item) -
                             (p_ipbox->curn_bit + 1)));
}


static void _ipbox_set_curn_bit(control_t *p_ctrl, u8 num)
{
  ctrl_ipbox_t *p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  u32 base = _ipbox_get_base(
    _ipbox_get_item_bit(p_ctrl, p_ipbox->curn_item) - (p_ipbox->curn_bit + 1));

  switch(p_ipbox->curn_item)
  {
    case 0: //s_b1
      p_ipbox->ctrl_address.s_b1 -= _ipbox_get_curn_bit(p_ctrl) * base;
      p_ipbox->ctrl_address.s_b1 += num * base;
      break;
    case 1: //s_b2
      p_ipbox->ctrl_address.s_b2 -= _ipbox_get_curn_bit(p_ctrl) * base;
      p_ipbox->ctrl_address.s_b2 += num * base;
      break;
    case 2: //s_b3
      p_ipbox->ctrl_address.s_b3 -= _ipbox_get_curn_bit(p_ctrl) * base;
      p_ipbox->ctrl_address.s_b3 += num * base;
      break;
    case 3: //s_b4
      p_ipbox->ctrl_address.s_b4 -= _ipbox_get_curn_bit(p_ctrl) * base;
      p_ipbox->ctrl_address.s_b4 += num * base;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}


static u16 _ipbox_get_total_width(control_t *p_ctrl)
{
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  ctrl_ipbox_t *p_ipbox = NULL;
  u16 num[IPBOX_MAX_ITEM_NUM] = {0};
  u16 total_width = 0;
  rsc_bitmap_t hdr_bmp = {{0}};
  u8 *p_bmp_data = NULL;
  u8 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  //draw string
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _ipbox_get_fstyle(attr, p_ipbox);

  num[0] = p_ipbox->ctrl_address.s_b1;
  num[1] = p_ipbox->ctrl_address.s_b2;
  num[2] = p_ipbox->ctrl_address.s_b3;
  num[3] = p_ipbox->ctrl_address.s_b4;

  for(i = 0; i < IPBOX_MAX_ITEM_NUM; i++)
  {
    if(_ipbox_get_item_bit(p_ctrl, i) != 0)
    {
      total_width +=
        (p_ipbox->max_num_width * _ipbox_get_item_bit(p_ctrl, i));

      switch(p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK)
      {
        case IPBOX_SEPARATOR_TYPE_STRID:
          total_width += rsc_get_strid_width(
            gui_get_rsc_handle(), (u16)p_ipbox->separator[i], font);
          break;
        case IPBOX_SEPARATOR_TYPE_EXTSTR:
          total_width +=
            rsc_get_unistr_width(gui_get_rsc_handle(), (u16 *)(p_ipbox->separator[i]), font);
          break;
        case IPBOX_SEPARATOR_TYPE_BMPID:
          rsc_get_bmp(gui_get_rsc_handle(), (u16)p_ipbox->separator[i], &hdr_bmp, &p_bmp_data);
          total_width += hdr_bmp.width;
          break;
      }
    }
  }

  return total_width;
}


void ipbox_set_address_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~IPBOX_ADDRESS_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & IPBOX_ADDRESS_MASK);    //set new type
}


void ipbox_set_separator_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~IPBOX_SEPARATOR_TYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & IPBOX_SEPARATOR_TYPE_MASK);    //set new type
}


BOOL ipbox_set_separator_by_strid(control_t *p_ctrl, u8 index, u16 strid)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK) !=
     IPBOX_SEPARATOR_TYPE_STRID)
  {
    return FALSE;
  }

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->separator[index] = strid;

  return TRUE;
}


BOOL ipbox_set_separator_by_extstr(control_t *p_ctrl, u8 index, u16 *p_str)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK) !=
     IPBOX_SEPARATOR_TYPE_EXTSTR)
  {
    return FALSE;
  }
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->separator[index] = (u32)p_str;

  return TRUE;
}


BOOL ipbox_set_separator_by_ascchar(control_t *p_ctrl, u8 index, u8 asc_char)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK) !=
     IPBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  p_ipbox->separator[index] = (u32)asc_char;

  return TRUE;
}


BOOL ipbox_set_separator_by_unichar(control_t *p_ctrl, u8 index, u16 uni_char)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK) !=
     IPBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  p_ipbox->separator[index] = (u32)uni_char;

  return TRUE;
}


BOOL ipbox_set_separator_by_bmpid(control_t *p_ctrl, u8 index, u16 bmpid)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK) !=
     IPBOX_SEPARATOR_TYPE_BMPID)
  {
    return FALSE;
  }

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->separator[index] = bmpid;

  return TRUE;
}


u32 ipbox_get_separator(control_t *p_ctrl, u8 index)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  return p_ipbox->separator[index];
}


void ipbox_set_address(control_t *p_ctrl, ip_address_t *p_address)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  memcpy(&(p_ipbox->ctrl_address), p_address, sizeof(ip_address_t));
}


void ipbox_get_address(control_t *p_ctrl, ip_address_t *p_address)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  //get s_b1
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) != 0)
  {
    p_address->s_b1 = p_ipbox->ctrl_address.s_b1;
  }

  //get s_b2
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B2) != 0)
  {
    p_address->s_b2 = p_ipbox->ctrl_address.s_b2;
  }

  //get s_b3
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B3) != 0)
  {
    p_address->s_b3 = p_ipbox->ctrl_address.s_b3;
  }

  //get s_b4
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B4) != 0)
  {
    p_address->s_b4 = p_ipbox->ctrl_address.s_b4;
  }

}


void ipbox_set_font_style(control_t *p_ctrl, u32 n_fstyle, u32 h_fstyle, u32 g_fstyle)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->n_fstyle = n_fstyle;
  p_ipbox->h_fstyle = h_fstyle;
  p_ipbox->g_fstyle = g_fstyle;
}

void ipbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  
  *p_factive = p_ipbox->n_fstyle;
  *p_fhlight = p_ipbox->h_fstyle;
  *p_finactive = p_ipbox->g_fstyle;
}


void ipbox_set_max_num_width(control_t *p_ctrl, u8 width)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->max_num_width = width;
}


u8 ipbox_get_max_num_width(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  return p_ipbox->max_num_width;
}


void ipbox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ipbox->str_left = left;
  p_ipbox->str_top = top;
}

void ipbox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  
  *p_left = p_ipbox->str_left;
  *p_top = p_ipbox->str_top;
}


void ipbox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~IPBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & IPBOX_ALIGN_MASK); //set new style
}

u32 ipbox_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & IPBOX_ALIGN_MASK);
}


static u32 _ipbox_get_fstyle(u8 attr, ctrl_ipbox_t *p_ipbox)
{
  u32 font = 0;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_ipbox->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_ipbox->g_fstyle;
      break;

    default:
      font = p_ipbox->n_fstyle;
  }

  return font;
}


static void _ipbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_ipbox_t *p_ipbox = NULL;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  u16 str_num[NUM_STRING_LEN + 1] = {0};
  rect_t str_rect = {0};
  u16 total_width = 0, width = 0, height = 0;
  rsc_bitmap_t hdr_bmp = {{0}};
  u8 *p_bmp_data = NULL;
  u8 i = 0;
  u16 num[IPBOX_MAX_ITEM_NUM] = {0};
  rsc_fstyle_t *p_fstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);


  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _ipbox_get_fstyle(attr, p_ipbox);

  ctrl_get_draw_rect(p_ctrl, &str_rect);

  num[0] = p_ipbox->ctrl_address.s_b1;
  num[1] = p_ipbox->ctrl_address.s_b2;
  num[2] = p_ipbox->ctrl_address.s_b3;
  num[3] = p_ipbox->ctrl_address.s_b4;

  total_width = _ipbox_get_total_width(p_ctrl);

  if(p_ctrl->priv_attr & STL_RIGHT)
  {
    str_rect.left = str_rect.right - p_ipbox->str_left - total_width;
  }
  else if(p_ctrl->priv_attr & STL_LEFT)
  {
    str_rect.left = str_rect.left + p_ipbox->str_left;
  }
  else
  {
    str_rect.left =
      str_rect.left + ((str_rect.right - str_rect.left - total_width) >> 1);
  }

  for(i = 0; i < IPBOX_MAX_ITEM_NUM; i++)
  {
    if(_ipbox_get_item_bit(p_ctrl, i) != 0)
    {
      convert_i_to_dec_str_ex(str_num, num[i], 3);

      width = (p_ipbox->max_num_width * _ipbox_get_item_bit(p_ctrl, i));
      str_rect.right = str_rect.left + width;

      //draw number
      if(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK)
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_ipbox->str_top, 0, str_num, font,
                        _get_draw_style(p_ipbox, i));
      }
      else
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_ipbox->str_top, 0, str_num, font,
                        MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
      }
      str_rect.left += width; //width of number.

      //draw separator
      switch(p_ctrl->priv_attr & IPBOX_SEPARATOR_TYPE_MASK)
      {
        case IPBOX_SEPARATOR_TYPE_STRID:
          width = rsc_get_strid_width(gui_get_rsc_handle(), (u16)p_ipbox->separator[i], font);
          str_rect.right = str_rect.left + width;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_ipbox->str_top, 0,
                          (u16 *)gui_get_string_addr((u16)p_ipbox->separator[i]),
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
          str_rect.left += width;
          break;
        case IPBOX_SEPARATOR_TYPE_EXTSTR:
          width = rsc_get_unistr_width(gui_get_rsc_handle(), (u16 *)(p_ipbox->separator[i]), font);
          str_rect.right = str_rect.left + width;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_ipbox->str_top, 0, (u16 *)(p_ipbox->separator[i]),
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
          str_rect.left += width;
          break;
        case IPBOX_SEPARATOR_TYPE_BMPID:
          rsc_get_bmp(gui_get_rsc_handle(), (u16)p_ipbox->separator[i], &hdr_bmp, &p_bmp_data);
          width = hdr_bmp.width;
          str_rect.right = str_rect.left + width;
          gui_draw_picture(hdc, &str_rect, p_ctrl->priv_attr, 0,
                           p_ipbox->str_top, (u16)p_ipbox->separator[i]);
          str_rect.left += width;
          break;
        case IPBOX_SEPARATOR_TYPE_UNICODE:
          p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), font);
          rsc_get_char_attr(gui_get_rsc_handle(),
            (u16)p_ipbox->separator[i], p_fstyle, &width, &height);

          //gui_draw_char(hdc, p_fstyle,
          //              (u16)p_ipbox->separator[i], str_rect.left, str_rect.top);

          str_rect.right = str_rect.left + width;
          gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                          0, p_ipbox->str_top, 0,
                          (u16 *)(&(p_ipbox->separator[i])),
                          font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));

          str_rect.left += width;
          break;
      }
    }
  }
}


BOOL ipbox_enter_edit(control_t *p_ctrl)
{
  u32 ipbox_attr[IPBOX_MAX_ITEM_NUM] =
  {
    IPBOX_S_B1, IPBOX_S_B2,
    IPBOX_S_B3, IPBOX_S_B4
  };
  ctrl_ipbox_t *p_ipbox = NULL;
  u8 i = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;
  p_ctrl->priv_attr |= IPBOX_HL_STATUS_MASK;

  // goto the first bit
  for(i = 0; i < IPBOX_MAX_ITEM_NUM; i++)
  {
    if(p_ctrl->priv_attr & ipbox_attr[i])
    {
      p_ipbox->curn_item = i;
      break;
    }
  }
  p_ipbox->curn_bit = 0; //first bit of year item

  /* DO SOMETHING */
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL ipbox_is_invalid_address(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  //check s_b1
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) != 0)
  {
    if(p_ipbox->ctrl_address.s_b1 > 255)
    {
      return TRUE;
    }
  }

  //check s_b2
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B2) != 0)
  {
    if(p_ipbox->ctrl_address.s_b2 > 255)
    {
      return TRUE;
    }
  }

  //check s_b3
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B3) != 0)
  {
    if(p_ipbox->ctrl_address.s_b3 > 255)
    {
      return TRUE;
    }
  }

  //check s_b4
  if(_ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B4) != 0)
  {
    if(p_ipbox->ctrl_address.s_b4 > 255)
    {
      return TRUE;
    }
  }

  return FALSE;
}


void ipbox_exit_edit(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;
  ip_address_t address = {0};

  MT_ASSERT(p_ctrl != NULL);
  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  if(ipbox_is_invalid_address(p_ctrl))
  {
    // set num to border
      address.s_b1 = 255;
      address.s_b2 = 255;
      address.s_b3 = 255;
      address.s_b4 = 255;
    ipbox_set_address(p_ctrl, &address);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    // notify out of range
    //p_ctrl->p_proc(p_ctrl, MSG_OUTRANGE, p_ipbox->num, border);
  }
  else
  {
    p_ctrl->priv_attr &= (~IPBOX_HL_STATUS_MASK);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    ipbox_get_address(p_ctrl, &address);

    // notify leave edit
    p_ctrl->p_proc(p_ctrl, MSG_CHANGED, (u32)(&address), 0);
  }
}


BOOL ipbox_input_number(control_t *p_ctrl, u16 msg)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  _ipbox_set_curn_bit(p_ctrl, (u8)(msg & MSG_DATA_MASK));

  // move to next bit
  //p_ipbox->curn_bit =
  //  (p_ipbox->curn_bit + TBOX_BITLEN - 1) % p_ipbox->bit_length;
  ipbox_shift_focus(p_ctrl, -1);

  return TRUE;
}


static BOOL _num_increase(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  switch(p_ipbox->curn_item)
  {
    case IPBOX_ITEM_S_B1:
     OS_PRINTF("@@@before s_b1[%d]@@@\n", p_ipbox->ctrl_address.s_b1);
      if(_ipbox_get_curn_bit(p_ctrl) == 9)
      {
        p_ipbox->ctrl_address.s_b1 -= 9 * _ipbox_get_base(
          _ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b1 += _ipbox_get_base(
          _ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) - (p_ipbox->curn_bit + 1));
      }
     OS_PRINTF("@@@after s_b1[%d]@@@\n", p_ipbox->ctrl_address.s_b1);
      break;
    case IPBOX_ITEM_S_B2:
      if(_ipbox_get_curn_bit(p_ctrl) == 9)
      {
        p_ipbox->ctrl_address.s_b2 -= 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B2) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b2 += _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B2) - (p_ipbox->curn_bit + 1));
      }
      break;
    case IPBOX_ITEM_S_B3:
      if(_ipbox_get_curn_bit(p_ctrl) == 9)
      {
        p_ipbox->ctrl_address.s_b3 -= 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B3) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b3 += _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B3) - (p_ipbox->curn_bit + 1));
      }
      break;
    case IPBOX_ITEM_S_B4:
      if(_ipbox_get_curn_bit(p_ctrl) == 9)
      {
        p_ipbox->ctrl_address.s_b4 -= 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B4) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b4 += _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B4) - (p_ipbox->curn_bit + 1));
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  return TRUE;
}


static BOOL _num_decrease(control_t *p_ctrl)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  switch(p_ipbox->curn_item)
  {
    case IPBOX_ITEM_S_B1:
      if(_ipbox_get_curn_bit(p_ctrl) == 0)
      {
        p_ipbox->ctrl_address.s_b1 += 9 * _ipbox_get_base(
          _ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b1 -= _ipbox_get_base(
          _ipbox_get_item_bit(p_ctrl, IPBOX_ITEM_S_B1) - (p_ipbox->curn_bit + 1));
      }
      break;
    case IPBOX_ITEM_S_B2:
      if(_ipbox_get_curn_bit(p_ctrl) == 0)
      {
        p_ipbox->ctrl_address.s_b2 += 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B2) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b2 -= _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B2) - (p_ipbox->curn_bit + 1));
      }
      break;
    case IPBOX_ITEM_S_B3:
      if(_ipbox_get_curn_bit(p_ctrl) == 0)
      {
        p_ipbox->ctrl_address.s_b3 += 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B3) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b3 -= _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B3) - (p_ipbox->curn_bit + 1));
      }
      break;
    case IPBOX_ITEM_S_B4:
      if(_ipbox_get_curn_bit(p_ctrl) == 0)
      {
        p_ipbox->ctrl_address.s_b4 += 9 * _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B4) - (p_ipbox->curn_bit + 1));
      }
      else
      {
        p_ipbox->ctrl_address.s_b4 -= _ipbox_get_base(
          _ipbox_get_item_bit(
            p_ctrl, IPBOX_ITEM_S_B4) - (p_ipbox->curn_bit + 1));
      }
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  return TRUE;
}


BOOL ipbox_set_focus(control_t *p_ctrl, u8 bit, u8 item)
{
  ctrl_ipbox_t *p_ipbox = NULL;
  BOOL ret = FALSE;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  if(bit < _ipbox_get_item_bit(p_ctrl, item))
  {
    p_ipbox->curn_bit = bit;
    p_ipbox->curn_item = item;
    ret = TRUE;
  }

  return ret;
}


void ipbox_get_focus(control_t *p_ctrl, u8 *p_bit, u8 *p_item)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

  *p_bit = p_ipbox->curn_bit;
  *p_item = p_ipbox->curn_item;
}


BOOL ipbox_shift_focus(control_t *p_ctrl, s8 offset)
{
  ctrl_ipbox_t *p_ipbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_ipbox = (ctrl_ipbox_t *)p_ctrl;

//  p_ipbox->curn_bit =
//    (p_ipbox->curn_bit + TBOX_BITLEN + offset) % TBOX_BITLEN;

  while(offset != 0)
  {
    offset = _ipbox_shift_offset(p_ctrl, offset);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL ipbox_is_on_edit(control_t *p_ctrl)
{
  return (BOOL)(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK);
}


static RET_CODE on_ipbox_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK))
  {
    ipbox_enter_edit(p_ctrl);
  }

  if(ipbox_input_number(p_ctrl, msg))
  {
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_ipbox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & IPBOX_HL_STATUS_MASK))
  {
    ipbox_enter_edit(p_ctrl);
  }
  else
  {
    ipbox_exit_edit(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_ipbox_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  s8 offset = (s8)(msg == MSG_FOCUS_LEFT ? 1 : -1);

  MT_ASSERT(p_ctrl != NULL);

  ipbox_shift_focus(p_ctrl, offset);

  return SUCCESS;
}


static RET_CODE on_ipbox_increase(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_increase(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_ipbox_decrease(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_decrease(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


BEGIN_CTRLPROC(ipbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_NUMBER, on_ipbox_number)
ON_COMMAND(MSG_SELECT, on_ipbox_select)
ON_COMMAND(MSG_FOCUS_LEFT, on_ipbox_focus_change)
ON_COMMAND(MSG_FOCUS_RIGHT, on_ipbox_focus_change)
ON_COMMAND(MSG_INCREASE, on_ipbox_increase)
ON_COMMAND(MSG_DECREASE, on_ipbox_decrease)
END_CTRLPROC(ipbox_class_proc, ctrl_default_proc)
