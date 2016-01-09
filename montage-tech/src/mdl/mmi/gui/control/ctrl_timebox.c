/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_numbox.c
   this file  implement the functions defined in  ctrl_numbox.h, also it
   implement some internal used   function. All these functions are about how to
   decribe, set and draw a numbox control.
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
#include "ctrl_timebox.h"

/*!
  time-box control.
  */
typedef struct
{
  /*!
    base control.
    */
  control_t base;    
  /*!
     Year time
    */
  utc_time_t ctrl_time;
  /*!
     Current bit
    */
  u8 curn_bit;
  /*!
     Current item.
    */
  tbox_item_t curn_item;
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
  u32 separator[TBOX_MAX_ITEM_NUM];
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
  /*!
  	time style
  	*/
	time_style_t style;	
}ctrl_tbox_t;

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
static NONINLINE u32 _tbox_get_base(u32 bits)
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

   \param[in] p_tbox			: private data of nbox.
   \return					: the number of current bit.
  */
static u8 _tbox_get_curn_bit(control_t *p_ctrl);

/*!
   set the number of current bit.

   \param[in] p_tbox			: private data of nbox.
   \param[in] num			: number to be set.
   \return					: the number of current bit.
  */
static void _tbox_set_curn_bit(control_t *p_ctrl, u8 num);

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

static void _tbox_draw(control_t *p_ctrl, hdc_t hdc);

static u16 _tbox_get_total_width(control_t *p_ctrl);

static u32 _tbox_get_fstyle(u8 attr, ctrl_tbox_t *p_tbox);

RET_CODE tbox_register_class(u16 max_cnt)
{
  control_class_register_info_t register_info = {0};
  control_t *p_default_ctrl = NULL;

  register_info.data_size = sizeof(ctrl_tbox_t);
  register_info.max_cnt = max_cnt;

  // alloc buff
  ctrl_link_ctrl_class_buf(&register_info);
  p_default_ctrl = register_info.p_default_ctrl;

  // initialize the default control of timebox class
  p_default_ctrl->priv_attr = STL_CENTER | STL_VCENTER |
                              TBOX_YEAR | TBOX_MONTH | TBOX_DAY;
  p_default_ctrl->p_proc = tbox_class_proc;
  p_default_ctrl->p_paint = _tbox_draw;

  // initalize the default data of timebox class

  if(ctrl_register_ctrl_class(CTRL_TBOX, &register_info) != SUCCESS)
  {
    ctrl_unlink_ctrl_class_buf(&register_info);
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static void _tbox_pos2item(control_t *p_ctrl, u8 pos, tbox_item_t *p_index, u8 *p_sep_index)
{
  ctrl_tbox_t *p_tbox = NULL;
  
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_index != NULL);
  MT_ASSERT(p_sep_index != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  switch(p_tbox->style)
  {
    case TIME_STYLE_YMD_HMS:
      switch(pos)
      {
        case 0:
          *p_index = 0;
          *p_sep_index = 0;
          break;
          
        case 1:
          *p_index = 1;
          *p_sep_index = 1;        
          break;
          
        case 2:
          *p_index = 2;
          *p_sep_index = 0xFF;        
          break;
          
        case 3:
          *p_index = 3;
          *p_sep_index = 3;        
          break;
          
        case 4:
          *p_index = 4;
          *p_sep_index = 4;        
          break;
          
        case 5:
          *p_index = 5;
          *p_sep_index = 0xFF;   
          break;
          
        default:
          MT_ASSERT(0);
          break;
      }
      break;

    case TIME_STYLE_DMY_HMS:
      switch(pos)
      {
        case 0:
          *p_index = 2;
          *p_sep_index = 1;
          break;
          
        case 1:
          *p_index = 1;
          *p_sep_index = 0;        
          break;
          
        case 2:
          *p_index = 0;
          *p_sep_index = 0xFF;        
          break;
          
        case 3:
          *p_index = 3;
          *p_sep_index = 3;        
          break;
          
        case 4:
          *p_index = 4;
          *p_sep_index = 4;        
          break;
          
        case 5:
          *p_index = 5;
          *p_sep_index = 0xFF;   
          break;
          
        default:
          MT_ASSERT(0);
          break;
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return;
}


static void _tbox_item2pos(control_t *p_ctrl, u8 index, u8 *p_pos)
{
  ctrl_tbox_t *p_data = NULL;
  
  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_pos != NULL);

  p_data = (ctrl_tbox_t *)p_ctrl;

  switch(p_data->style)
  {
    case TIME_STYLE_YMD_HMS:
      switch(index)
      {
        case 0:
          *p_pos = 0;
          break;
          
        case 1:
          *p_pos = 1;
          break;
          
        case 2:
          *p_pos = 2;
          break;
          
        case 3:
          *p_pos = 3;
          break;
          
        case 4:
          *p_pos = 4;
          break;
          
        case 5:
          *p_pos = 5;
          break;
          
        default:
          MT_ASSERT(0);
          break;
      }
      break;

    case TIME_STYLE_DMY_HMS:
      switch(index)
      {
        case 0:
          *p_pos = 2;
          break;
          
        case 1:
          *p_pos = 1;
          break;
          
        case 2:
          *p_pos = 0;
          break;
          
        case 3:
          *p_pos = 3;
          break;
          
        case 4:
          *p_pos = 4;
          break;
          
        case 5:
          *p_pos = 5;
          break;
          
        default:
          MT_ASSERT(0);
          break;
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return;
}
static u32 _get_draw_style(ctrl_tbox_t *p_tbox, tbox_item_t item)
{
  if(item == p_tbox->curn_item)
  {
    return MAKE_DRAW_STYLE(STRDRAW_WITH_UNDERLINE,
                           p_tbox->curn_bit);
  }
  else
  {
    return MAKE_DRAW_STYLE(STRDRAW_NORMAL,
                           0);
  }
}


static u8 _tbox_get_item_bit(control_t *p_ctrl, tbox_item_t item)
{
  u32 tbox_attr[TBOX_MAX_ITEM_NUM] =
  {
    TBOX_YEAR, TBOX_MONTH, TBOX_DAY,
    TBOX_HOUR, TBOX_MIN, TBOX_SECOND
  };
  u8 item_bit = 0;

  if(p_ctrl->priv_attr & tbox_attr[item])
  {
    item_bit = ((item == TBOX_ITEM_YEAR) ? 4 : 2);
  }

  return item_bit;
}


static s32 _tbox_shift_offset(control_t *p_ctrl, s32 offset)
{
  ctrl_tbox_t *p_tbox = NULL;
  u8 pos = 0, sep_index = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  _tbox_item2pos(p_ctrl, p_tbox->curn_item, &pos);
  if(offset > 0) //move left
  {
    if(offset > p_tbox->curn_bit) //out of curn item
    {
      offset -= p_tbox->curn_bit;

      do
      {
        if(pos == 0)
        {
          pos = TBOX_MAX_ITEM_NUM - 1;
        }
        else
        {
          pos--;
        }
        _tbox_pos2item(p_ctrl, pos, &p_tbox->curn_item, &sep_index);
      }
      while(_tbox_get_item_bit(p_ctrl, p_tbox->curn_item) == 0);


      p_tbox->curn_bit = (_tbox_get_item_bit(p_ctrl, p_tbox->curn_item) - 1);

      (offset--);

      return offset;
    }
    else //move in curn item
    {
      p_tbox->curn_bit -= offset;

      return 0;
    }
  }
  else if(offset < 0)
  {
    offset = (0 - offset);

    if(offset > (_tbox_get_item_bit(p_ctrl, p_tbox->curn_item)
                 - (p_tbox->curn_bit + 1))) //out of current item.
    {
      offset -= (_tbox_get_item_bit(p_ctrl, p_tbox->curn_item)
                 - (p_tbox->curn_bit + 1));

      do
      {
        if(pos == (TBOX_MAX_ITEM_NUM - 1))
        {
          pos = 0;
        }
        else
        {
          pos++;
        }

        _tbox_pos2item(p_ctrl, pos, &p_tbox->curn_item, &sep_index);
      }
      while(_tbox_get_item_bit(p_ctrl, p_tbox->curn_item) == 0);

      p_tbox->curn_bit = 0;

      (offset--);

      return 0 - offset;
    }
    else
    {
      p_tbox->curn_bit += offset;

      return 0;
    }
  }
  return 0;
}


static u8 _tbox_get_curn_bit(control_t *p_ctrl)
{
  ctrl_tbox_t *p_tbox = NULL;
  u16 data = 0;

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  switch(p_tbox->curn_item)
  {
    case TBOX_ITEM_YEAR: //year
      data = p_tbox->ctrl_time.year;
      break;
    case TBOX_ITEM_MONTH: //month
      data = p_tbox->ctrl_time.month;
      break;
    case TBOX_ITEM_DAY: //day
      data = p_tbox->ctrl_time.day;
      break;
    case TBOX_ITEM_HOUR: //hour
      data = p_tbox->ctrl_time.hour;
      break;
    case TBOX_ITEM_MIN: //min
      data = p_tbox->ctrl_time.minute;
      break;
    case TBOX_ITEM_SECOND: //sec
      data = p_tbox->ctrl_time.second;
      break;
    default:
      MT_ASSERT(0);
      break;
  }

  return (u8)(data %
              _tbox_get_base(_tbox_get_item_bit(p_ctrl, p_tbox->curn_item)
                             - p_tbox->curn_bit) /
              _tbox_get_base(_tbox_get_item_bit(p_ctrl, p_tbox->curn_item) -
                             (p_tbox->curn_bit + 1)));
}


static void _tbox_set_curn_bit(control_t *p_ctrl, u8 num)
{
  ctrl_tbox_t *p_tbox = (ctrl_tbox_t *)p_ctrl;
  u32 base = _tbox_get_base(
    _tbox_get_item_bit(p_ctrl, p_tbox->curn_item) - (p_tbox->curn_bit + 1));

  switch(p_tbox->curn_item)
  {
    case TBOX_ITEM_YEAR: //year
      p_tbox->ctrl_time.year -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.year += num * base;
      break;
    case TBOX_ITEM_MONTH: //month
      p_tbox->ctrl_time.month -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.month += num * base;
      break;
    case TBOX_ITEM_DAY: //day
      p_tbox->ctrl_time.day -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.day += num * base;
      break;
    case TBOX_ITEM_HOUR: //hour
      p_tbox->ctrl_time.hour -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.hour += num * base;
      break;
    case TBOX_ITEM_MIN: //min
      p_tbox->ctrl_time.minute -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.minute += num * base;
      break;
    case TBOX_ITEM_SECOND: //sec
      p_tbox->ctrl_time.second -= _tbox_get_curn_bit(p_ctrl) * base;
      p_tbox->ctrl_time.second += num * base;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
}


static u16 _tbox_get_total_width(control_t *p_ctrl)
{
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  ctrl_tbox_t *p_tbox = NULL;
  u16 num[TBOX_MAX_ITEM_NUM] = {0};
  u16 total_width = 0, width = 0, height = 0;
  rsc_bitmap_t hdr_bmp = {{0}};
  u8 *p_bmp_data = NULL;
  u8 i = 0, sep_index = 0, item_bit = 0;
  tbox_item_t item = TBOX_ITEM_YEAR;
  handle_t h_rsc = gui_get_rsc_handle();  
  rsc_fstyle_t *p_fstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);

  //draw string
  p_tbox = (ctrl_tbox_t *)p_ctrl;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _tbox_get_fstyle(attr, p_tbox);

  num[0] = p_tbox->ctrl_time.year;
  num[1] = p_tbox->ctrl_time.month;
  num[2] = p_tbox->ctrl_time.day;
  num[3] = p_tbox->ctrl_time.hour;
  num[4] = p_tbox->ctrl_time.minute;
  num[5] = p_tbox->ctrl_time.second;

  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    _tbox_pos2item(p_ctrl, i, &item, &sep_index);
    item_bit = _tbox_get_item_bit(p_ctrl, item);
    if(item_bit != 0)
    {
      total_width +=
        (p_tbox->max_num_width * item_bit);

      switch(p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK)
      {
        case TBOX_SEPARATOR_TYPE_STRID:
          total_width += rsc_get_strid_width(h_rsc, (u16)p_tbox->separator[sep_index], font);
          break;
        case TBOX_SEPARATOR_TYPE_EXTSTR:
          total_width +=
            rsc_get_unistr_width(h_rsc, (u16 *)(p_tbox->separator[sep_index]), font);
          break;
        case TBOX_SEPARATOR_TYPE_BMPID:
          rsc_get_bmp(h_rsc, (u16)p_tbox->separator[sep_index], &hdr_bmp, &p_bmp_data);
          total_width += hdr_bmp.width;
          break;
        case TBOX_SEPARATOR_TYPE_UNICODE:
          p_fstyle = rsc_get_fstyle(h_rsc, font);
          
          rsc_get_char_attr(h_rsc, (u16)p_tbox->separator[sep_index], p_fstyle, &width, &height);

          total_width += width;
          break;

      }
    }
  }

  return total_width;
}


void tbox_set_time_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_TIME_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TBOX_TIME_MASK);    //set new type
}


void tbox_set_separator_type(control_t *p_ctrl, u32 type)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_SEPARATOR_TYPE_MASK);          //clean old type
  p_ctrl->priv_attr |= (type & TBOX_SEPARATOR_TYPE_MASK);    //set new type
}


BOOL tbox_set_separator_by_strid(control_t *p_ctrl, u8 index, u16 strid)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_STRID)
  {
    return FALSE;
  }

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->separator[index] = strid;

  return TRUE;
}


BOOL tbox_set_separator_by_extstr(control_t *p_ctrl, u8 index, u16 *p_str)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_EXTSTR)
  {
    return FALSE;
  }
  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->separator[index] = (u32)p_str;

  return TRUE;
}


BOOL tbox_set_separator_by_ascchar(control_t *p_ctrl, u8 index, u8 asc_char)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_tbox = (ctrl_tbox_t *)p_ctrl;

  p_tbox->separator[index] = (u32)asc_char;

  return TRUE;
}


BOOL tbox_set_separator_by_unichar(control_t *p_ctrl, u8 index, u16 uni_char)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_UNICODE)
  {
    return FALSE;
  }
  p_tbox = (ctrl_tbox_t *)p_ctrl;

  p_tbox->separator[index] = (u32)uni_char;

  return TRUE;
}


BOOL tbox_set_separator_by_bmpid(control_t *p_ctrl, u8 index, u16 bmpid)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if((p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK) !=
     TBOX_SEPARATOR_TYPE_BMPID)
  {
    return FALSE;
  }

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->separator[index] = bmpid;

  return TRUE;
}


u32 tbox_get_separator(control_t *p_ctrl, u8 index)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  // only available on dec mode
  p_tbox = (ctrl_tbox_t *)p_ctrl;
  return p_tbox->separator[index];
}


void tbox_set_style(control_t *p_ctrl, time_style_t style)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_tbox = (ctrl_tbox_t *)p_ctrl;

  p_tbox->style = style;
}
void tbox_set_time(control_t *p_ctrl, utc_time_t *p_time)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  /* TODO */
  p_tbox = (ctrl_tbox_t *)p_ctrl;
  //p_tbox->year = time.y;
  //p_tbox->month = time.mon;
  //p_tbox->day = time.d;
  //p_tbox->hour = time.h;
  //p_tbox->minute = time.min;
  //p_tbox->second = time.sec;
  memcpy(&(p_tbox->ctrl_time), p_time, sizeof(utc_time_t));
}


void tbox_get_time(control_t *p_ctrl, utc_time_t *p_time)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  //get year
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) != 0)
  {
    p_time->year = p_tbox->ctrl_time.year;
  }

  //get month
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MONTH) != 0)
  {
    p_time->month = p_tbox->ctrl_time.month;
  }

  //get day
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_DAY) != 0)
  {
    p_time->day = p_tbox->ctrl_time.day;
  }

  //get hour
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_HOUR) != 0)
  {
    p_time->hour = p_tbox->ctrl_time.hour;
  }

  //get minute
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MIN) != 0)
  {
    p_time->minute = p_tbox->ctrl_time.minute;
  }

  //get second
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_SECOND) != 0)
  {
    p_time->second = p_tbox->ctrl_time.second;
  }
}


void tbox_set_font_style(control_t *p_ctrl, u32 n_fstyle, u32 h_fstyle, u32 g_fstyle)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->n_fstyle = n_fstyle;
  p_tbox->h_fstyle = h_fstyle;
  p_tbox->g_fstyle = g_fstyle;
}

void tbox_get_fstyle(control_t *p_ctrl, u32 *p_factive, u32 *p_fhlight, u32 *p_finactive)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_factive != NULL);
  MT_ASSERT(p_fhlight != NULL);
  MT_ASSERT(p_finactive != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  *p_factive = p_tbox->n_fstyle;
  *p_fhlight = p_tbox->h_fstyle;
  *p_finactive = p_tbox->g_fstyle;
}


void tbox_set_max_num_width(control_t *p_ctrl, u8 width)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->max_num_width = width;
}


u8 tbox_get_max_num_width(control_t *p_ctrl)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  return p_tbox->max_num_width;
}


void tbox_set_offset(control_t *p_ctrl, u16 left, u16 top)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_tbox->str_left = left;
  p_tbox->str_top = top;
}

void tbox_get_offset(control_t *p_ctrl, u16 *p_left, u16 *p_top)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);
  MT_ASSERT(p_left != NULL);
  MT_ASSERT(p_top != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  
  *p_left = p_tbox->str_left;
  *p_top = p_tbox->str_top;
}


void tbox_set_align_type(control_t *p_ctrl, u32 style)
{
  MT_ASSERT(p_ctrl != NULL);

  p_ctrl->priv_attr &= (~TBOX_ALIGN_MASK);        //clean old style
  p_ctrl->priv_attr |= (style & TBOX_ALIGN_MASK); //set new style
}

u32 tbox_get_align_type(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);

  return (p_ctrl->priv_attr & TBOX_ALIGN_MASK);
}


static u32 _tbox_get_fstyle(u8 attr, ctrl_tbox_t *p_tbox)
{
  u32 font = 0;

  switch(attr)
  {
    case OBJ_ATTR_HL:
      font = p_tbox->h_fstyle;
      break;

    case OBJ_ATTR_INACTIVE:
      font = p_tbox->g_fstyle;
      break;

    default:
      font = p_tbox->n_fstyle;
  }

  return font;
}


static void _tbox_draw(control_t *p_ctrl, hdc_t hdc)
{
  ctrl_tbox_t *p_tbox = NULL;
  obj_attr_t attr = OBJ_ATTR_ACTIVE;
  u32 font = 0;
  u16 str_num[NUM_STRING_LEN + 1] = {0};
  rect_t str_rect;
  u16 total_width = 0, width = 0, height = 0;
  rsc_bitmap_t hdr_bmp = {{0}};
  u8 *p_bmp_data = NULL;
  u8 pos = 0, sep_index = 0, item_bit = 0;
  tbox_item_t item = TBOX_ITEM_YEAR;
  u16 num[TBOX_MAX_ITEM_NUM] = {0};
  rsc_fstyle_t *p_fstyle = NULL;

  MT_ASSERT(p_ctrl != NULL);


  gui_paint_frame(hdc, p_ctrl);

  //draw string
  p_tbox = (ctrl_tbox_t *)p_ctrl;
  if(ctrl_is_whole_hl(p_ctrl) || ctrl_is_always_hl(p_ctrl))
  {
    attr = OBJ_ATTR_HL;
  }
  else
  {
    attr = ctrl_get_attr(p_ctrl);
  }

  font = _tbox_get_fstyle(attr, p_tbox);

  ctrl_get_draw_rect(p_ctrl, &str_rect);

  num[0] = p_tbox->ctrl_time.year;
  num[1] = p_tbox->ctrl_time.month;
  num[2] = p_tbox->ctrl_time.day;
  num[3] = p_tbox->ctrl_time.hour;
  num[4] = p_tbox->ctrl_time.minute;
  num[5] = p_tbox->ctrl_time.second;

  total_width = _tbox_get_total_width(p_ctrl);

  if(p_ctrl->priv_attr & STL_RIGHT)
  {
    str_rect.left = str_rect.right - p_tbox->str_left - total_width;
  }
  else if(p_ctrl->priv_attr & STL_LEFT)
  {
    str_rect.left = str_rect.left + p_tbox->str_left;
  }
  else
  {
    str_rect.left =
      str_rect.left + ((str_rect.right - str_rect.left - total_width) >> 1);
  }

  for(pos = 0; pos < TBOX_MAX_ITEM_NUM; pos++)
  {
    _tbox_pos2item(p_ctrl, pos, &item, &sep_index);
    item_bit = _tbox_get_item_bit(p_ctrl, item);
    if(item_bit != 0)
    {
      convert_i_to_dec_str_ex(str_num, num[item], item_bit);

      width = (p_tbox->max_num_width * item_bit);
      str_rect.right = str_rect.left + width;

      //draw number
      if(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK)
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_tbox->str_top, 0, str_num, font,
                        _get_draw_style(p_tbox, item));
      }
      else
      {
        gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                        0, p_tbox->str_top, 0, str_num, font,
                        MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
      }
      str_rect.left += width; //width of number.

      if(sep_index != 0xFF)
      {
        //draw separator
        switch(p_ctrl->priv_attr & TBOX_SEPARATOR_TYPE_MASK)
        {
          case TBOX_SEPARATOR_TYPE_STRID:
            width = rsc_get_strid_width(gui_get_rsc_handle(), (u16)p_tbox->separator[sep_index], font);
            str_rect.right = str_rect.left + width;
            gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                            0, p_tbox->str_top, 0,
                            (u16 *)gui_get_string_addr((u16)p_tbox->separator[sep_index]),
                            font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
            str_rect.left += width;
            break;
          case TBOX_SEPARATOR_TYPE_EXTSTR:
            width = rsc_get_unistr_width(gui_get_rsc_handle(), (u16 *)(p_tbox->separator[sep_index]), font);
            str_rect.right = str_rect.left + width;
            gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                            0, p_tbox->str_top, 0, (u16 *)(p_tbox->separator[sep_index]),
                            font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));
            str_rect.left += width;
            break;
          case TBOX_SEPARATOR_TYPE_BMPID:
            rsc_get_bmp(gui_get_rsc_handle(), (u16)p_tbox->separator[sep_index], &hdr_bmp, &p_bmp_data);
            width = hdr_bmp.width;
            str_rect.right = str_rect.left + width;
            gui_draw_picture(hdc, &str_rect, p_ctrl->priv_attr, 0,
                             p_tbox->str_top, (u16)p_tbox->separator[sep_index]);
            str_rect.left += width;
            break;
          case TBOX_SEPARATOR_TYPE_UNICODE:
            p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), font);
            rsc_get_char_attr(gui_get_rsc_handle(),
              (u16)p_tbox->separator[sep_index], p_fstyle, &width, &height);

            //gui_draw_char(hdc, p_fstyle,
            //              (u16)p_data->separator[i], str_rect.left, str_rect.top);

            str_rect.right = str_rect.left + width;
            gui_draw_unistr(hdc, &str_rect, p_ctrl->priv_attr,
                            0, p_tbox->str_top, 0,
                            (u16 *)(&(p_tbox->separator[sep_index])),
                            font, MAKE_DRAW_STYLE(STRDRAW_NORMAL, 0));

            str_rect.left += width;
            break;
        }
      }
    }
  }
}


BOOL tbox_enter_edit(control_t *p_ctrl)
{
  u32 tbox_attr[TBOX_MAX_ITEM_NUM] =
  {
    TBOX_YEAR, TBOX_MONTH, TBOX_DAY,
    TBOX_HOUR, TBOX_MIN, TBOX_SECOND
  };
  ctrl_tbox_t *p_tbox = NULL;
  u8 i = 0, sep_index = 0;
  tbox_item_t item = TBOX_ITEM_YEAR;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;
  p_ctrl->priv_attr |= TBOX_HL_STATUS_MASK;

  // goto the first bit
  for(i = 0; i < TBOX_MAX_ITEM_NUM; i++)
  {
    if(p_ctrl->priv_attr & tbox_attr[i])
    {
      _tbox_pos2item(p_ctrl, i, &item, &sep_index);
      p_tbox->curn_item = item;
      break;
    }
  }
  p_tbox->curn_bit = 0; //first bit of year item

  /* DO SOMETHING */
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL tbox_is_invalid_time(control_t *p_ctrl)
{
  ctrl_tbox_t *p_tbox = NULL;
  BOOL is_leap = FALSE;
  u16 year = 0;
  u8 days = 0;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  //check year
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) != 0)
  {
    //check year
    if((p_tbox->ctrl_time.year < 2000) || (p_tbox->ctrl_time.year > 2100))
    {
      return TRUE;
    }
  }

  //check month
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MONTH) != 0)
  {
    //check month
    if((p_tbox->ctrl_time.month > 12) || (p_tbox->ctrl_time.month <= 0))
    {
      return TRUE;
    }
  }

  //check day
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_DAY) != 0)
  {
    year = p_tbox->ctrl_time.year;

    is_leap = NOT(year % 400) || ((year % 100) && NOT(year % 4));

    switch(p_tbox->ctrl_time.month)
    {
      case 11:
      case 9:
      case 6:
      case 4:
        days = 30;
        break;
      case 12:
      case 10:
      case 8:
      case 7:
      case 5:
      case 3:
      case 1:
        days = 31;
        break;
      case 2:
        days = is_leap ? 29 : 28;
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    //check day
    if((p_tbox->ctrl_time.day > days) || (p_tbox->ctrl_time.day <= 0))
    {
      return TRUE;
    }
  }

  //check hour
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_HOUR) != 0)
  {
    if(p_tbox->ctrl_time.hour >= 24) //check hour
    {
      return TRUE;
    }
  }

  //check minute
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_MIN) != 0)
  {
    if(p_tbox->ctrl_time.minute >= 60) //check minute
    {
      return TRUE;
    }
  }

  //check second
  if(_tbox_get_item_bit(p_ctrl, TBOX_ITEM_SECOND) != 0)
  {
    if(p_tbox->ctrl_time.second >= 60) //check second
    {
      return TRUE;
    }
  }

  return FALSE;
}


void tbox_exit_edit(control_t *p_ctrl)
{
  ctrl_tbox_t *p_tbox = NULL;
  utc_time_t tm = {0};

  MT_ASSERT(p_ctrl != NULL);
  p_tbox = (ctrl_tbox_t *)p_ctrl;

  if(tbox_is_invalid_time(p_ctrl))
  {
    // set num to border
    //time_get(&curn_time, TRUE);
      tm.year = 2012;
      tm.month = 1;
      tm.day = 1;
      //tm.hour = 24;
      //tm.minute = 59;
      //tm.second = 59;
     if(p_tbox->ctrl_time.hour >= 24) //check hour
      {
        tm.hour = 23;
        tm.minute = 59;
        tm.second = 59;
       }
     else if(p_tbox->ctrl_time.hour < 24 && p_tbox->ctrl_time.minute >= 60)
     {
       tm.hour = p_tbox->ctrl_time.hour;
       tm.minute = 59;
       tm.second  = p_tbox->ctrl_time.second;
     }
    else
    {
      tm.hour = p_tbox->ctrl_time.hour;
      tm.minute = p_tbox->ctrl_time.minute;
      tm.second = 59;
     }
    tbox_set_time(p_ctrl, &tm);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    // notify out of range
    //p_ctrl->p_proc(p_ctrl, MSG_OUTRANGE, p_tbox->num, border);
  }
  else
  {
    p_ctrl->priv_attr &= (~TBOX_HL_STATUS_MASK);
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

    tbox_get_time(p_ctrl, &tm);

    // notify leave edit
    p_ctrl->p_proc(p_ctrl, MSG_CHANGED, (u32)(&tm), 0);
  }
}


BOOL tbox_input_number(control_t *p_ctrl, u16 msg)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  _tbox_set_curn_bit(p_ctrl, (u8)(msg & MSG_DATA_MASK));

  // move to next bit
  //p_tbox->curn_bit =
  //  (p_tbox->curn_bit + TBOX_BITLEN - 1) % p_tbox->bit_length;
  tbox_shift_focus(p_ctrl, -1);

  return TRUE;
}


static BOOL _num_increase(control_t *p_ctrl)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  switch(p_tbox->curn_item)
  {
    case TBOX_ITEM_YEAR:
      //OS_PRINTF("before year[%d]\n", p_tbox->year);
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.year -= 9 * _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.year += _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_tbox->curn_bit + 1));
      }
      //OS_PRINTF("after year[%d]\n", p_tbox->year);
      break;
    case TBOX_ITEM_MONTH:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.month -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.month += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_DAY:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.day -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.day += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_HOUR:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.hour -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.hour += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MIN:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.minute -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.minute += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_SECOND:
      if(_tbox_get_curn_bit(p_ctrl) == 9)
      {
        p_tbox->ctrl_time.second -= 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.second += _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_tbox->curn_bit + 1));
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
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  switch(p_tbox->curn_item)
  {
    case TBOX_ITEM_YEAR:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.year += 9 * _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.year -= _tbox_get_base(
          _tbox_get_item_bit(p_ctrl, TBOX_ITEM_YEAR) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MONTH:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.month += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.month -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MONTH) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_DAY:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.day += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.day -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_DAY) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_HOUR:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.hour += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.hour -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_HOUR) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_MIN:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.minute += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.minute -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_MIN) - (p_tbox->curn_bit + 1));
      }
      break;
    case TBOX_ITEM_SECOND:
      if(_tbox_get_curn_bit(p_ctrl) == 0)
      {
        p_tbox->ctrl_time.second += 9 * _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_tbox->curn_bit + 1));
      }
      else
      {
        p_tbox->ctrl_time.second -= _tbox_get_base(
          _tbox_get_item_bit(
            p_ctrl, TBOX_ITEM_SECOND) - (p_tbox->curn_bit + 1));
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


void tbox_set_focus(control_t *p_ctrl, u8 bit, tbox_item_t item)
{
  ctrl_tbox_t *p_tbox = NULL;
  u32 tbox_attr[TBOX_MAX_ITEM_NUM] =
  {
    TBOX_YEAR, TBOX_MONTH, TBOX_DAY,
    TBOX_HOUR, TBOX_MIN, TBOX_SECOND
  };

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  MT_ASSERT((p_ctrl->priv_attr & tbox_attr[item]) != 0);
  MT_ASSERT(bit < _tbox_get_item_bit(p_ctrl, item));
  
  p_tbox->curn_bit = bit;
  p_tbox->curn_item = item;

  return;
}


void tbox_get_focus(control_t *p_ctrl, u8 *p_bit, tbox_item_t *p_item)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_tbox = (ctrl_tbox_t *)p_ctrl;

  *p_bit = p_tbox->curn_bit;
  *p_item = p_tbox->curn_item;
}


BOOL tbox_shift_focus(control_t *p_ctrl, s8 offset)
{
  ctrl_tbox_t *p_tbox = NULL;

  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    return FALSE;
  }

  p_tbox = (ctrl_tbox_t *)p_ctrl;

//  p_tbox->curn_bit =
//    (p_tbox->curn_bit + TBOX_BITLEN + offset) % TBOX_BITLEN;

  while(offset != 0)
  {
    offset = _tbox_shift_offset(p_ctrl, offset);
  }

  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
  ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

  return TRUE;
}


BOOL tbox_is_on_edit(control_t *p_ctrl)
{
  return (BOOL)(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK);
}


static RET_CODE on_tbox_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    tbox_enter_edit(p_ctrl);
  }

  if(tbox_input_number(p_ctrl, msg))
  {
    ctrl_add_rect_to_invrgn(p_ctrl, NULL);
    ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
  }

  return SUCCESS;
}


static RET_CODE on_tbox_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    tbox_enter_edit(p_ctrl);
  }
  else
  {
    tbox_exit_edit(p_ctrl);
  }

  return SUCCESS;
}


static RET_CODE on_tbox_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  s8 offset = (s8)(msg == MSG_FOCUS_LEFT ? 1 : -1);

  MT_ASSERT(p_ctrl != NULL);

  tbox_shift_focus(p_ctrl, offset);

  return SUCCESS;
}


static RET_CODE on_tbox_increase(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_increase(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


static RET_CODE on_tbox_decrease(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  MT_ASSERT(p_ctrl != NULL);

  if(!_num_decrease(p_ctrl))
  {
    return ERR_NOFEATURE;
  }

  return SUCCESS;
}


BEGIN_CTRLPROC(tbox_class_proc, ctrl_default_proc)
ON_COMMAND(MSG_NUMBER, on_tbox_number)
ON_COMMAND(MSG_SELECT, on_tbox_select)
ON_COMMAND(MSG_FOCUS_LEFT, on_tbox_focus_change)
ON_COMMAND(MSG_FOCUS_RIGHT, on_tbox_focus_change)
ON_COMMAND(MSG_INCREASE, on_tbox_increase)
ON_COMMAND(MSG_DECREASE, on_tbox_decrease)
END_CTRLPROC(tbox_class_proc, ctrl_default_proc)
