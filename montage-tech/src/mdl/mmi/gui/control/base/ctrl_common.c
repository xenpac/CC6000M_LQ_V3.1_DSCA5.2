/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
   \file ctrl_common.c
   this file  implement the functions defined in  ctrl_common.h.
   These functions are mainly about child control, invalid region and constant
   transform.
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


void proc_check(control_t *p_ctrl, u32 proc)
{
  if((u32)(p_ctrl->p_oci->p_default_ctrl->p_proc) != proc)
  {
    MT_ASSERT(0);
  }    
}

void ctrl_add_child(control_t *p_parent, control_t *p_child)
{
  control_t *p_first_child = NULL, *p_last_child = NULL;

  MT_ASSERT(p_child != NULL);

  if(p_parent == NULL)
  {
    return;
  }

  p_first_child = p_parent->p_child;

  p_child->p_parent = p_parent;
  p_child->p_next = NULL;

  if(p_first_child == NULL)
  {
    p_parent->p_child = p_child;
    p_child->p_prev = NULL;
  }
  else
  {
    p_last_child = p_first_child;

    while(p_last_child->p_next != NULL)
    {
      p_last_child = p_last_child->p_next;
    }

    p_last_child->p_next = p_child;
    p_child->p_prev = p_last_child;
  }
}


BOOL ctrl_remove_child(control_t *p_parent, control_t *p_child)
{
  control_t *p_first_child = NULL;
  BOOL is_found = FALSE;

  MT_ASSERT(p_child != NULL);

  if(NULL == p_parent)
  {
    if(p_child->p_prev != NULL)
    {
      p_child->p_prev->p_next = p_child->p_next;
    }
    if(p_child->p_next != NULL)
    {
      p_child->p_next->p_prev = p_child->p_prev;
    }
    p_child->p_prev = p_child->p_next = NULL;
    is_found = TRUE;
  }
  else
  {
    p_first_child = p_parent->p_child;

    if(NULL == p_first_child)
    {
      return FALSE;
    }
    else
    {
      if(p_first_child == p_child)
      {
        p_parent->p_child = p_child->p_next;

        if(p_child->p_next != NULL)
        {
          p_child->p_next->p_prev = NULL;
        }

        is_found = TRUE;
      }
      else
      {
        while(p_first_child->p_next != NULL)
        {
          if(p_first_child->p_next == p_child)
          {
            p_first_child->p_next = p_child->p_next;

            if(p_first_child->p_next != NULL)
            {
              p_first_child->p_next->p_prev = p_child->p_prev;
            }
            is_found = TRUE;
            break;
          }

          p_first_child = p_first_child->p_next;
        }
      }
    }
  }

  if(is_found == TRUE)
  {
    p_child->p_parent = NULL;
  }

  return is_found;
}


void ctrl_destroy_children(control_t *p_ctrl)
{
  control_t *p_child = NULL, *p_temp = NULL;

  MT_ASSERT(p_ctrl != NULL);

  p_child = p_ctrl->p_child;
  while(NULL != p_child)
  {
    p_temp = p_child->p_next;
    ctrl_destroy_ctrl(p_child);
    p_child = p_temp;
  }
  ctrl_add_rect_to_invrgn(p_ctrl, NULL);
}


void ctrl_init_invrgn(control_t *p_ctrl)
{
  rect_t rc;

  MT_ASSERT(p_ctrl != NULL);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_lock(p_ctrl->invrgn_info.lock);
#endif
  gdi_init_cliprgn(&p_ctrl->invrgn_info.crgn, gdi_get_cliprc_heap());

  ctrl_get_client_rect(p_ctrl, &rc);
  gdi_set_cliprgn(&p_ctrl->invrgn_info.crgn, &rc);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_unlock(p_ctrl->invrgn_info.lock);
#endif
}


void ctrl_empty_invrgn(control_t *p_ctrl)
{
  MT_ASSERT(p_ctrl != NULL);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_lock(p_ctrl->invrgn_info.lock);
#endif
  gdi_empty_cliprgn(&p_ctrl->invrgn_info.crgn);
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_unlock(p_ctrl->invrgn_info.lock);
#endif
}


static void update_invrgn_by_rect(crgn_info_t *p_invrgn,
                                  BOOL is_add,
                                  const rect_t *p_rc,
                                  const rect_t *p_client_rect)
{
  rect_t temp_rect;

  MT_ASSERT(p_invrgn != NULL && p_client_rect != NULL);

#ifdef MUTI_THREAD_SUPPORT
  os_mutex_lock(p_invrgn->lock);
#endif
  if(p_rc != NULL)
  {
    temp_rect = *p_rc;
    normalize_rect(&temp_rect);
    if(is_rect_covered(p_client_rect, &temp_rect))
    {
      if(is_add)
      {
        gdi_set_cliprgn(&p_invrgn->crgn, p_client_rect);
      }
      else
      {
        gdi_empty_cliprgn(&p_invrgn->crgn);
      }
    }
    else if(intersect_rect(&temp_rect, &temp_rect, p_client_rect))
    {
      if(is_add)
      {
        gdi_add_cliprect(&p_invrgn->crgn, &temp_rect);
      }
      else
      {
        gdi_subtract_cliprect(&p_invrgn->crgn, &temp_rect);
      }
    }
  }
  else
  {
    if(is_add)
    {
      gdi_set_cliprgn(&p_invrgn->crgn, p_client_rect);
    }
    else
    {
      gdi_empty_cliprgn(&p_invrgn->crgn);
    }
  }
#ifdef MUTI_THREAD_SUPPORT
  os_mutex_unlock(p_invrgn->lock);
#endif
}


static void recalc_children_invrgn(control_t *p_ctrl,
                                   BOOL is_add,
                                   const rect_t *p_rc_in_parent)
{
  rect_t rc_client, rc_frame, rc_parent;

  while(p_ctrl != NULL)
  {
    if(p_ctrl->sts != OBJ_STS_HIDE)
    {
      rc_parent = *p_rc_in_parent;
      ctrl_get_frame(p_ctrl, &rc_frame);

      if(intersect_rect(&rc_parent, &rc_parent, &rc_frame))
      {
        offset_rect(&rc_parent, (s16)(0 - p_ctrl->frame.left),
                    (s16)(0 - p_ctrl->frame.top));
        ctrl_get_client_rect(p_ctrl, &rc_client);
        update_invrgn_by_rect(&p_ctrl->invrgn_info,
                              is_add,
                              &rc_parent,
                              &rc_client);

        if(p_ctrl->p_child != NULL)
        {
          recalc_children_invrgn(p_ctrl->p_child, is_add, &rc_parent);
        }
      }
    }

    p_ctrl = p_ctrl->p_next;
  }
}


static BOOL recalc_invrgn_by_rect(control_t *p_ctrl,
                                  BOOL is_add,
                                  BOOL is_recalc_children,
                                  const rect_t *p_rc)
{
  rect_t rc_invalid, rc_client;

  MT_ASSERT(p_ctrl != NULL);

  if(OBJ_STS_HIDE == p_ctrl->sts)
  {
    return FALSE;
  }

  ctrl_get_client_rect(p_ctrl, &rc_client);
  if(p_rc != NULL)
  {
    copy_rect(&rc_invalid, p_rc);
    update_invrgn_by_rect(&p_ctrl->invrgn_info, 
                          is_add, 
                          &rc_invalid, 
                          &rc_client);
  }
  else
  {
    copy_rect(&rc_invalid, &rc_client);
    update_invrgn_by_rect(&p_ctrl->invrgn_info, is_add, NULL, &rc_client);
  }

  if(is_recalc_children
    && p_ctrl->p_child != NULL)
  {
    recalc_children_invrgn(p_ctrl->p_child, is_add, &rc_invalid);
  }

  return TRUE;
}

BOOL ctrl_update_invrgn_by_rect(control_t *p_ctrl, BOOL is_add, const rect_t *p_rc)
{
  return recalc_invrgn_by_rect(p_ctrl, is_add, FALSE, p_rc);
}

BOOL ctrl_add_rect_to_invrgn(control_t *p_ctrl, const rect_t *p_rc)
{
  return recalc_invrgn_by_rect(p_ctrl, TRUE, TRUE, p_rc);
}


BOOL ctrl_subtract_rect_from_invrgn(control_t *p_ctrl, const rect_t *p_rc)
{
  return recalc_invrgn_by_rect(p_ctrl, FALSE, TRUE, p_rc);
}


void convert_i_to_dec_str(u16 *p_str, s32 n)
{
  u32 abs = 0;
  /*the sprintf will auto add '\0' in the end,so the buf should be "NUM_STRING_LEN+1" */
  char buf[NUM_STRING_LEN + 1] = {0};  
  char *p_start = buf;

  MT_ASSERT(p_str != NULL);

  abs = ABS(n);

  if(n < 0)
  {
    buf[0] = '-';
    p_start++;
  }

//  itoa(p_start, abs);
  sprintf(p_start, "%d", (int)abs);

  str_asc2uni((u8 *)buf, p_str);
}


void convert_i_to_dec_str_ex(u16 *p_str, s32 n, u8 len)
{
  char buf[10], format[10];
  u32 i = 0, base = 1;
  s32 num = 0;
  
  MT_ASSERT(p_str != NULL);

  if(len > 0)
  {
    for(i = 0; i < len; i++)
    {
      base = base * 10;
    }
    num = n % base;
    sprintf(format, "%%.%dd", len);
    sprintf(buf, format, (int)num);
    str_asc2uni((u8 *)buf, p_str);
  }
  else
  {
    convert_i_to_dec_str(p_str, n);  
  }

}


s32 convert_dec_str_to_i(u16 *p_str)
{
  s32 n = 0;
  char buf[10], *p_start = buf;

  MT_ASSERT(p_str != NULL);

  str_uni2asc((u8 *)buf, p_str);

  if(buf[0] == '-')
  {
    p_start++;
  }

  n = atoi(p_start);

  if(buf[0] == '-')
  {
    n = 0 - n;
  }

  return n;
}


void convert_i_to_hex_str(u16 *p_str, s32 n)
{
  char buf[10];

  MT_ASSERT(p_str != NULL);

  sprintf(buf, "%x", (u16)n);
  str_asc2uni((u8 *)buf, p_str);
}


void convert_i_to_hex_str_ex(u16 *p_str, s32 n, u8 len)
{
  char buf[10], format[10];
  u32 i = 0, base = 1;
  s32 num = 0;
  
  MT_ASSERT(p_str != NULL);

  if(len > 0)
  {
    for(i = 0; i < len; i++)
    {
      base = base * 16;
    }
    
    num = n % base;
    sprintf(format, "%%.%dx", len);
    sprintf(buf, format, (int)num);
    str_asc2uni((u8 *)buf, p_str);
  }
  else
  {
    sprintf(format, "%%.%dx", len);
    sprintf(buf, format, (u32)n);
  }
}


s32 convert_hex_str_to_i(u16 *p_str)
{
  s32 n = 0;
  char buf[10];

  MT_ASSERT(p_str != NULL);

  str_uni2asc((u8 *)buf, p_str);

  n = atoi(buf);

  return n;
}


void insert_aschar_into_unistr(u16 *p_unistr, u8 aschar, u32 pos, u32 len)
{
  u32 i = 0, actual_len = uni_strlen(p_unistr);

  if(actual_len < len && pos < actual_len)
  {
    for(i = actual_len; i > (actual_len - pos - 1); (i--))
    {
      p_unistr[i + 1] = p_unistr[i];
    }
    p_unistr[i + 1] = char_asc2uni(aschar);
  }
}
