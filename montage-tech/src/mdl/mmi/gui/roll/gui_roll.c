/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
 \file gui_resource.c
   this file  implement the functions defined in  gui_resource.h, also it implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar control.
 */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"
#include "charsto.h"

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
#include "common.h"
#include "osd.h"
#include "gpe.h"
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
#include "class_factory.h"

/*!
  roll separate mode
  */
typedef struct
{
  /*!
    string
    */
  u16 *p_str;
  /*!
    string drawed
    */
  u16 str_drawed;  
  /*!
    string x offset
    */
  s16 str_off;  
  /*!
    last string drawed.
    */
  u16 last_drawed;
  /*!
    last string offset.
    */
  s16 last_off;    
  /*!
    string length
    */
  u16 str_width; 
  /*!
    string height.
    */
  u16 str_height;    
  /*!
    font style id
    */
  u16 fstyle_id;  
  /*!
    draw style
    */
  u32 draw_style;
  /*!
    is picture or not
    */
  BOOL is_pic;
  /*!
    is picture rawdata whether picture ID
    */
  BOOL is_pic_rawdata;
  /*!
    draw style
    */
  u32 pic_data;
}roll_sept_t;

/*!
  roll list
  */
typedef struct roll_list
{
  /*!
    content.
    */
  u32 content;
  /*!
    type.
    */
  roll_data_t type;
  /*!
    roll param
    */
  roll_param_t param;    
  /*!
    next.
    */
  struct roll_list *p_next;
}roll_list_t;

/*!
  roll node
  */
typedef struct roll_node
{
  /*!
    blt x
    */
  s16 blt_x;
  /*!
    blt y
    */
  s16 blt_y;
  /*!
    rgn width
    */
  u16 rgn_w;
  /*!
    rgn height
    */
  u16 rgn_h;
  /*!
    control.
    */
  control_t *p_ctrl;
  /*!
    context
    */
  u32 context;
  /*!
    curn counts, for caculate pace.
    */
  u16 curn;
  /*!
    curn repeat times, for caculate repeat times.
    */
  u16 curn_times;
  /*!
    is seprate mode
    */
  BOOL is_sept;   
  /*!
    is pause
    */
  BOOL is_pause;    
  /*!
    dst rectangle
    */
  rect_t dst_rc;
  /*!
    surface handle
    */
  handle_t handle;
  /*!
    surface address
    */
  u32 addr;
  /*!
    roll parameter
    */
  roll_param_t param;
  /*!
    roll list
    */
  roll_list_t *p_list;   
  /*!
    roll seprate
    */
  roll_sept_t sept;
  /*!
    next roll node
    */
  struct roll_node *p_next;
}roll_node_t;

typedef struct
{
  /*!
    blt x
    */
  s16 blt_x;
  /*!
    blt y
    */
  s16 blt_y;
  /*!
    curn counts, for caculate pace.
    */
  u16 curn;
  /*!
    curn repeat times, for caculate repeat times.
    */
  u16 curn_times;
  /*!
    is pause
    */
  BOOL is_pause;    
  /*!
    string drawed
    */
  u16 str_drawed;  
  /*!
    string x offset
    */
  s16 str_off;
  /*!
    roll param
    */
  roll_param_t param;    
  /*!
    roll list
    */
  roll_list_t *p_list;   
}roll_saved_t;

/*!
   gui roll info.
  */
typedef struct
{
  /*!
    first node
    */
  roll_node_t *p_first;
  /*!
    node heap
    */
  lib_memf_t node_heap;
  /*!
    node heap addr
    */
  void *p_node_heap_addr;
  /*!
    pixel per step
    */
  u8 pps;    
  /*!
    saved roll param, for refresh.
    */
  roll_saved_t *p_saved;    
}gui_roll_t;

/*!
  roll module initial.
  */
void gui_roll_init(u8 max_cnt, u8 pps)
{
  class_handle_t p_handle = NULL;
  gui_roll_t *p_info = NULL;

  MT_ASSERT(max_cnt != 0);
  MT_ASSERT(pps != 0);
  MT_ASSERT(pps <= 10);

  p_handle = (class_handle_t)mmi_alloc_buf(sizeof(gui_roll_t));
  MT_ASSERT(p_handle != NULL);

  memset((void *)p_handle, 0, sizeof(gui_roll_t));
  class_register(ROLL_CLASS_ID, p_handle);

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->p_node_heap_addr =
    mmi_create_memf(&p_info->node_heap, max_cnt, sizeof(roll_node_t));
  MT_ASSERT(p_info->p_node_heap_addr != NULL);

  p_info->pps = pps;
}

/*!
  roll module release.
  */
void gui_roll_release(void)
{
  gui_roll_t *p_info = NULL;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  mmi_destroy_memf(&p_info->node_heap, p_info->p_node_heap_addr);
}

/*!
  allocate new roll node & add it to the rolling list.
  */
static roll_node_t *gui_roll_add(control_t *p_ctrl, roll_param_t *p_param,
  handle_t handle, u32 addr, u16 width, u16 height,
  rect_t *p_dst_rc, u32 context, roll_sept_t *p_sept)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_last = NULL;
  roll_node_t *p_tmp = NULL;

  MT_ASSERT(p_dst_rc != NULL);
  MT_ASSERT(p_param != NULL);
  MT_ASSERT(p_ctrl != NULL);  

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  //to check if the node is already rolling.
  p_last = p_info->p_first;
  
  while(p_last != NULL)
  {
    if((p_ctrl == p_last->p_ctrl) && (p_last->context == context))
    {
      OS_PRINTF("node already rolling\n");

      return p_last;
    }
    else
    {
      p_last = p_last->p_next;
    }
  }  
  

  //try to find the last node.
  p_last = p_info->p_first;

  while(p_last != NULL)
  {
    p_tmp = p_last;
    p_last = p_last->p_next;
  }

  p_last = p_tmp;

  //create a new node
  p_tmp = (roll_node_t *)lib_memf_alloc(&p_info->node_heap);
  MT_ASSERT(p_tmp != NULL);

  memset(p_tmp, 0, sizeof(roll_node_t));

  p_tmp->p_ctrl = p_ctrl;
  p_tmp->blt_x = 0;
  p_tmp->blt_y = 0;
  p_tmp->context = context;
  memcpy(&p_tmp->param, p_param, sizeof(roll_param_t));

  p_tmp->rgn_w = width;
  p_tmp->rgn_h = height;
  p_tmp->dst_rc = *p_dst_rc;
  p_tmp->curn = 0;
  p_tmp->curn_times = 0;
  p_tmp->blt_x = 0;
  p_tmp->handle = handle;
  p_tmp->addr = addr;

  if(p_sept != NULL)
  {
    p_tmp->is_sept = TRUE;
    memcpy(&p_tmp->sept, p_sept, sizeof(roll_sept_t));
  }
  
  //add new node.
  if(p_last == NULL)
  {
    p_info->p_first = p_tmp;
  }
  else
  {
    p_last->p_next = p_tmp;
  }

  return p_tmp;
}

/*!
  free the memory allocated by a certain wait list content.
  */
static void gui_roll_free_list(roll_list_t *p_list)
{
  MT_ASSERT(p_list != NULL);
  
  switch(p_list->type)
  {
    case ROLL_ASCSTR:
    case ROLL_UNISTR:      
      if(p_list->content != 0)
      {
        mtos_free((void *)p_list->content);
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }  

  mtos_free(p_list);
}

/*!
  free memory allocated by node.
  */
static void gui_roll_release_node(roll_node_t *p_node)
{
  gui_roll_t *p_info = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  MT_ASSERT(p_node != NULL);

  gdi_delete_rsurf(p_node->handle, p_node->addr);
  
  //release new node.
  lib_memf_free(&p_info->node_heap, p_node);

  
  return;
}

/*!
  stripped node from node list & release it.
  */
static void gui_remove_node(roll_node_t *p_node)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  roll_node_t *p_next = NULL;
  roll_node_t *p_prev = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_temp = p_info->p_first;
  p_prev = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_node == p_temp)
    {
      if(p_temp == p_info->p_first)
      {
        p_next = p_info->p_first->p_next;
        p_info->p_first = p_next;
      }
      else
      {
        p_prev->p_next = p_temp->p_next;
      }

      //remove the node.
      gui_roll_release_node(p_temp);

      return;

    }
    else
    {
      p_prev = p_temp;
    }

    p_temp = p_temp->p_next;
  }  

  return;  
}

/*!
  free all wait list memory under a certain node.
  */
static void gui_roll_free_node_list(roll_node_t *p_node)
{
  roll_list_t *p_list = NULL;
  roll_list_t *p_temp = NULL;
  
  //release content list, if exist
  p_list = p_node->p_list;
  while(p_list != NULL)
  {
    p_temp = p_list;

    p_list = p_list->p_next;

    gui_roll_free_list(p_temp);
  }
}

static void gui_rolling_compt(roll_node_t *p_node, hdc_t hdc)
{
  RET_CODE ret = SUCCESS;
  hdc_t sdc = 0;
  roll_list_t *p_list = NULL;
  control_t *p_ctrl = NULL;
  rect_t src_rect = {0};
  u16 tot_w = 0, tot_h = 0;
  u16 src_x = 0, src_y = 0;
  u16 src_w = 0, src_h = 0;
  u16 dst_x = 0, dst_y = 0;
  u16 dst_w = 0, dst_h = 0;
  gui_roll_t *p_info = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_node != NULL);
  
  dst_w = RECTW(p_node->dst_rc);
  dst_h = RECTH(p_node->dst_rc);  
  
  tot_w = p_node->rgn_w + dst_w;
  tot_h = p_node->rgn_h + dst_h;
      
  switch(p_node->param.style)
  {
    case ROLL_LR:
      p_node->blt_x += p_info->pps;
      if(p_node->blt_x > (tot_w - 1))
      {
        p_node->blt_x = 0;
        p_node->curn_times++;
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  if((p_node->param.repeats != 0) && (p_node->curn_times == p_node->param.repeats))
  {
    if(p_node->p_list != NULL)
    {
      p_ctrl = p_node->p_ctrl;
      
      ret = ctrl_process_msg(p_node->p_ctrl,
        MSG_ROLL_NEXT, p_node->p_list->content, (u32)p_node->p_list->type);
      if(ret == SUCCESS)
      {
        //list control won't support this function, so don't warry about that.
        p_list = p_node->p_list;

        gui_remove_node(p_node);
        gui_start_roll(p_ctrl, &p_list->param);   

        //free current list
        p_node = p_info->p_first;

        //reset p_list for the new node.
        while(p_node != NULL)
        {
          if(p_ctrl == p_node->p_ctrl)
          {
            p_node->p_list = p_list->p_next;
            break;
          }
          else
          {
            p_node = p_node->p_next;
          }
        }

        //release current roll content.
        gui_roll_free_list(p_list);

      }
      else
      {
        gui_remove_node(p_node);
        ctrl_process_msg(p_ctrl, MSG_ROLL_STOPPED, 0, 0);        
      }
    }
    else
    {
      gui_remove_node(p_node);      
      ctrl_process_msg(p_node->p_ctrl, MSG_ROLL_STOPPED, 0, 0);
    }
    return;
  }

  src_rect.right = p_node->rgn_w;
  src_rect.bottom = p_node->rgn_h;


  if(p_node->rgn_w <= dst_w)
  {
    if(p_node->blt_x <= p_node->rgn_w)
    {
      src_x = 0;
      src_y = p_node->blt_y;
      src_w = p_node->blt_x;
      src_h = dst_h - p_node->blt_y;
      dst_x = dst_w - p_node->blt_x;
      dst_y = 0;
    }
    else if(p_node->blt_x <= dst_w)
    {
      src_x = 0;
      src_y = p_node->blt_y;        
      src_w = p_node->rgn_w;
      src_h = dst_h - p_node->blt_y;
      dst_x = dst_w - p_node->blt_x;
      dst_y = 0;
    }
    else
    {
      src_x = (p_node->blt_x - dst_w);
      src_y = p_node->blt_y;        
      src_w = tot_w - p_node->blt_x;
      src_h = dst_h - p_node->blt_y;
      dst_x = 0;
      dst_y = 0;
    }
  }
  else
  {
    if(p_node->blt_x <= dst_w)
    {
      src_x = 0;
      src_y = p_node->blt_y;        
      src_w = p_node->blt_x;
      src_h = dst_h - p_node->blt_y;
      dst_x = dst_w - p_node->blt_x;
      dst_y = 0;      
    }
    else if(p_node->blt_x <= p_node->rgn_w)
    {
      src_x = p_node->blt_x - dst_w;
      src_y = p_node->blt_y;        
      src_w = dst_w;
      src_h = dst_h - p_node->blt_y;
      dst_x = 0;
      dst_y = 0;      
    }
    else
    {
      src_x = p_node->blt_x - dst_w;
      src_y = p_node->blt_y;        
      src_w = tot_w - p_node->blt_x;
      src_h = dst_h - p_node->blt_y;
      dst_x = 0;
      dst_y = 0;        
    }    
  }
  
  if((src_w != 0) && (src_h != 0))
  {
    dst_x += p_node->dst_rc.left;
    dst_y += p_node->dst_rc.top;
    
    sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
    gdi_bitblt(sdc, src_x, src_y,
      src_w, src_h, hdc, dst_x, dst_y, 0, 0);
    gdi_release_roll_dc(sdc);
  }

  return;
}

static void gui_rolling_sept(roll_node_t *p_node, hdc_t hdc)
{
  RET_CODE ret = SUCCESS;
  hdc_t sdc = 0, str_dc = 0;
  roll_list_t *p_list = NULL;
  control_t *p_ctrl = NULL;
  rect_t src_rect = {0};
  u16 tot_w = 0;
  u16 dst_x = 0, dst_y = 0;
  u16 dst_w = 0, dst_h = 0;
  gui_roll_t *p_info = NULL;
  rect_t str_rect = {0};
  rsc_fstyle_t *p_fstyle = NULL;
  u32 color_key = 0;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  MT_ASSERT(p_node != NULL);
  
  dst_w = RECTW(p_node->dst_rc);
  dst_h = RECTH(p_node->dst_rc);  
  
  tot_w = dst_w + p_node->sept.str_width;

  if(p_node->blt_x > (tot_w - 1))
  {
    p_node->blt_x = 0;
    p_node->curn_times++;

    //reload whole string.
    str_rect.left = 0;
    str_rect.top = 0;
    str_rect.right = dst_w;
    str_rect.bottom = dst_h;
    if((p_node->sept.is_pic) && (p_node->sept.str_width > dst_w))
    {
      str_rect.right = p_node->sept.str_width;
    }

    p_node->sept.str_drawed = 0;
    p_node->sept.str_off = 0;

    if(p_node->sept.is_pic)
    {
      color_key = 0;
    }
    else
    {
      p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), p_node->sept.fstyle_id);

      color_key = (p_fstyle->color ? 0 : 1);
    }

    if(p_node->handle != NULL)
    {
      if(p_node->param.use_bg)
      {
        flinger_fill_rect((void *)p_node->handle, &str_rect, p_node->param.bg_color);
      }
      else
      {
        flinger_fill_rect((void *)p_node->handle, &str_rect, color_key);
      }
      
      str_dc = gdi_get_roll_dc(&str_rect, p_node->handle);
      MT_ASSERT(hdc != HDC_INVALID);

      //save last draw param, for refresh.
      p_node->sept.last_drawed = p_node->sept.str_drawed;
      p_node->sept.last_off = p_node->sept.str_off;

      if(p_node->sept.is_pic)
      {
        p_node->sept.str_drawed = gui_draw_picture_offset(str_dc, &str_rect, 
          p_node->sept.pic_data, p_node->sept.is_pic_rawdata, p_node->sept.str_drawed, 
          p_node->sept.str_width, p_node->sept.str_height, &p_node->sept.str_off);
      }
      else
      {
        p_node->sept.str_drawed = gui_draw_unistr_offset(str_dc, &str_rect, 
          p_node->sept.p_str, p_node->sept.fstyle_id, p_node->sept.draw_style,
          p_node->sept.str_drawed, p_node->sept.str_width,
          p_node->sept.str_height, &p_node->sept.str_off);
      }

      gdi_release_roll_dc(str_dc);   
    }
  }

  if((p_node->param.repeats != 0) && (p_node->curn_times == p_node->param.repeats))
  {
    if(p_node->p_list != NULL)
    {
      p_ctrl = p_node->p_ctrl;
      
      ret = ctrl_process_msg(p_node->p_ctrl,
        MSG_ROLL_NEXT, p_node->p_list->content, (u32)p_node->p_list->type);
      if(ret == SUCCESS)
      {
        //list control won't support this function, so don't warry about that.
        p_list = p_node->p_list;

        gui_remove_node(p_node);
        gui_start_roll(p_ctrl, &p_list->param);   

        //free current list
        p_node = p_info->p_first;

        //reset p_list for the new node.
        while(p_node != NULL)
        {
          if(p_ctrl == p_node->p_ctrl)
          {
            p_node->p_list = p_list->p_next;
            break;
          }
          else
          {
            p_node = p_node->p_next;
          }
        }

        //release current roll content.
        gui_roll_free_list(p_list);

      }
      else
      {
        gui_remove_node(p_node);
        ctrl_process_msg(p_ctrl, MSG_ROLL_STOPPED, 0, 0);        
      }
    }
    else
    {
      gui_remove_node(p_node);      
      ctrl_process_msg(p_node->p_ctrl, MSG_ROLL_STOPPED, 0, 0);
    }
    return;
  }

  //draw rolling step.
  switch(p_node->param.style)
  {
    case ROLL_LR:
    case ROLL_LB:
      src_rect.top = 0;
      src_rect.bottom = dst_h;

      if(p_node->blt_x <= dst_w)
      {
        src_rect.left = 0;
        src_rect.right = p_node->blt_x;
        dst_x = dst_w - p_node->blt_x;
        dst_y = 0;

        dst_x += p_node->dst_rc.left;
        dst_y += p_node->dst_rc.top;     
        
        sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
        gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
        gdi_release_roll_dc(sdc);
      }
      else if(p_node->blt_x <= p_node->sept.str_width)
      {
        if(p_node->sept.is_pic)
        {
          src_rect.left = p_node->blt_x - dst_w;
          src_rect.right = p_node->blt_x;
          dst_x = 0;
          dst_y = 0;

          if(!is_invalid_rect(&src_rect))
          {
            dst_x = 0;
            dst_y += p_node->dst_rc.top;     

            sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
            gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
            gdi_release_roll_dc(sdc);
          }
        }
        else
        {
          //blt first part.
          src_rect.left = p_node->blt_x % dst_w;
          src_rect.right = dst_w;
          
          dst_x = 0;
          dst_y = 0;
          
          if(!is_invalid_rect(&src_rect))
          {
            dst_x += p_node->dst_rc.left;
            dst_y += p_node->dst_rc.top;        
            sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
            gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
            gdi_release_roll_dc(sdc);  
          }

          //blt second part.
          src_rect.left = 0;
          src_rect.right = p_node->blt_x % dst_w;

          dst_x = dst_w - p_node->blt_x % dst_w;
          dst_y = 0;
          if(!is_invalid_rect(&src_rect))
          {          
            dst_x += p_node->dst_rc.left;
            dst_y += p_node->dst_rc.top;        
            sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
            gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
            gdi_release_roll_dc(sdc);  
          }
        }
      }
      else
      {
        src_rect.left = p_node->blt_x % dst_w;
        src_rect.right = dst_w;

        dst_x = 0;
        dst_y = 0;
        
        if(!is_invalid_rect(&src_rect))
        {
          dst_x += p_node->dst_rc.left;
          dst_y += p_node->dst_rc.top;          
          sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
          gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
          gdi_release_roll_dc(sdc);  
        }

        if((tot_w - p_node->blt_x) >= RECTW(src_rect))
        {
          dst_x = RECTW(src_rect);
          dst_y = 0;
          
          src_rect.left = 0;
          src_rect.right = (tot_w - p_node->blt_x - dst_x);

          if(!is_invalid_rect(&src_rect))
          {
            dst_x += p_node->dst_rc.left;
            dst_y += p_node->dst_rc.top;          
            sdc = gdi_get_roll_dc(&src_rect, (void *)p_node->handle);
            gdi_bitblt(sdc, 0, 0, RECTW(src_rect), RECTH(src_rect), hdc, dst_x, dst_y, 0, 0);
            gdi_release_roll_dc(sdc); 
          }
        }
      }
      break;
  
    default:
      break;
  }

  //reload string on previous sept buffer, if needed.
  if(!p_node->sept.is_pic)
  {
    switch(p_node->param.style)
    {
      case ROLL_LR:
      case ROLL_LB:
        if(p_node->blt_x + p_info->pps <= dst_w)
        {
          str_rect.left = 0;
          str_rect.right = 0;
        }
        else if(p_node->blt_x <= dst_w)
        {
          str_rect.left = 0;
          str_rect.right  = p_node->blt_x + p_info->pps - dst_w;
        }
        else
        {
          str_rect.left = p_node->blt_x % dst_w;
          str_rect.right  = str_rect.left + p_info->pps;
        }
      
        //reload more chars.
        str_rect.top = 0;
        str_rect.bottom = dst_h;

        if(str_rect.right > dst_w)
        {
          str_rect.right = dst_w;
        }
        
        if(!is_invalid_rect(&str_rect))
        {
          p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), p_node->sept.fstyle_id);

          color_key = (p_fstyle->color ? 0 : 1);

          if(p_node->param.use_bg)
          {
            flinger_fill_rect(
              (void *)p_node->handle, &str_rect, p_node->param.bg_color);
          }
          else
          {
            flinger_fill_rect((void *)p_node->handle, &str_rect, color_key);
          }
          
          str_dc = gdi_get_roll_dc(&str_rect, p_node->handle);
          MT_ASSERT(hdc != HDC_INVALID);

          //save last draw param, for refresh.
          p_node->sept.last_drawed = p_node->sept.str_drawed;
          p_node->sept.last_off = p_node->sept.str_off;

          p_node->sept.str_drawed = gui_draw_unistr_offset(str_dc, &str_rect, 
            p_node->sept.p_str, p_node->sept.fstyle_id, p_node->sept.draw_style,
            p_node->sept.str_drawed, p_node->sept.str_width,
            p_node->sept.str_height, &p_node->sept.str_off);

          OS_PRINTF("@# draw %d ---> %d-----%d\n", str_rect.left, str_rect.right, p_node->blt_x);
    
          gdi_release_roll_dc(str_dc);
        }  

        if((p_node->blt_x > dst_w) && ((str_rect.left + p_info->pps) > dst_w))
        {
          str_rect.right = ((str_rect.left + p_info->pps) - dst_w);
          str_rect.left = 0;
          
          if(!is_invalid_rect(&str_rect))
          {
            p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), p_node->sept.fstyle_id);

            color_key = (p_fstyle->color ? 0 : 1);

            if(p_node->param.use_bg)
            {
              flinger_fill_rect(
                (void *)p_node->handle, &str_rect, p_node->param.bg_color);
            }
            else
            {
              flinger_fill_rect((void *)p_node->handle, &str_rect, color_key);
            }
            
            str_dc = gdi_get_roll_dc(&str_rect, p_node->handle);
            MT_ASSERT(hdc != HDC_INVALID);

            //save last draw param, for refresh.
            p_node->sept.last_drawed = p_node->sept.str_drawed;
            p_node->sept.last_off = p_node->sept.str_off;

            p_node->sept.str_drawed = gui_draw_unistr_offset(str_dc, &str_rect, 
              p_node->sept.p_str, p_node->sept.fstyle_id, p_node->sept.draw_style,
              p_node->sept.str_drawed, p_node->sept.str_width,
              p_node->sept.str_height, &p_node->sept.str_off);

            OS_PRINTF("@# draw %d ---> %d-----%d\n", str_rect.left, str_rect.right, p_node->blt_x);
      
            gdi_release_roll_dc(str_dc);
          } 
        }
        break;

      default:
        break;
    }
  }

  p_node->blt_x += p_info->pps;

  return;
}

void gui_rolling_node(void *p_roll_node, hdc_t hdc)
{
  roll_node_t *p_node = NULL;

  p_node = (roll_node_t *)p_roll_node;

  if(p_node == NULL)
  {
    return;
  }
  
  if(p_node->is_sept)
  {
    gui_rolling_sept(p_node, hdc);
  }
  else
  {
    gui_rolling_compt(p_node, hdc);
  }

  return;
}

void gui_rolling(void)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);  

  p_temp = p_info->p_first;

  while(p_temp != NULL)
  {
    MT_ASSERT(p_temp->curn < p_temp->param.pace);
    if(p_temp->is_pause == FALSE)
    {
      p_temp->curn++;
      
      if(p_temp->curn == p_temp->param.pace)
      {
      
        if((p_temp->addr != 0) && (p_temp->handle != 0))
        {
          ctrl_process_msg(p_temp->p_ctrl, MSG_ROLLING, (u32)p_temp, 0);
        }

        p_temp->curn = 0;
      }
    }
    p_temp = p_temp->p_next;
  }
}

BOOL ctrl_is_rolling(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_node = NULL;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  if(p_info == NULL)
  {
    return FALSE;
  }

  p_node = p_info->p_first;

  while(p_node != NULL)
  {
    if(p_ctrl == p_node->p_ctrl)
    {
      return TRUE;
    }
    else
    {
      p_node = p_node->p_next;
    }
  }  

  return FALSE;
}

void gui_pause_roll(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_temp = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_ctrl == p_temp->p_ctrl)
    {
      p_temp->is_pause = TRUE;
    }

    p_temp = p_temp->p_next;
  }  

  return;  
}

void gui_resume_roll(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_temp = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_ctrl == p_temp->p_ctrl)
    {
      p_temp->is_pause = FALSE;
    }

    p_temp = p_temp->p_next;
  }  
}


void gui_start_roll(control_t *p_ctrl, roll_param_t *p_param)
{
  ctrl_process_msg(p_ctrl, MSG_START_ROLL, (u32)p_param, 0);  
}


void gui_stop_roll(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_temp = NULL;
  roll_node_t *p_next = NULL;
  roll_node_t *p_prev = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_temp = p_info->p_first;
  p_prev = p_info->p_first;

  while(p_temp != NULL)
  {
    if(p_ctrl == p_temp->p_ctrl)
    {
      if(p_temp == p_info->p_first)
      {
        p_next = p_info->p_first->p_next;
        p_info->p_first = p_next;
      }
      else
      {
        p_prev->p_next = p_temp->p_next;
      }

      //free all the list under this node.
      gui_roll_free_node_list(p_temp);

      //remove the node.
      gui_roll_release_node(p_temp);

    }
    else
    {
      p_prev = p_temp;
    }

    p_temp = p_temp->p_next;
  }  

  return;  
}

void gui_roll_reset(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_param_t roll_param = {0};
  roll_node_t *p_node = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  if(p_info == NULL)
  {
    return;
  }

  p_node = p_info->p_first;

  while(p_node != NULL)
  {
    if(p_ctrl == p_node->p_ctrl)
    {
      //reset this node.
      memcpy(&roll_param, &p_node->param, sizeof(roll_param_t));
      break;
    }
    else
    {
      p_node = p_node->p_next;
    }    
  }  

  if(p_node != NULL)
  {
    gui_stop_roll(p_ctrl);

    gui_start_roll(p_ctrl, &roll_param);
  }
  return;  
}

void gui_create_rsurf(rsurf_ceate_t *p_rsurf, roll_param_t *p_param)
{
  rsc_fstyle_t *p_fstyle = NULL;
  u16 width = 0, height = 0, data_width = 0;
  s16 str_offset = 0;
  handle_t handle = 0;
  rect_t str_rect = {0};
  hdc_t hdc = 0;
  u32 color_key = 0, addr = 0;
  u16 str_drawed = 0;
  roll_sept_t sept = {0};
  u32 draw_style = 0;
  roll_param_t saved_param = {0};
  roll_param_t *p_roll_param = NULL;
  gui_roll_t *p_info = NULL;
  roll_node_t *p_node = NULL;

  p_roll_param = p_param;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  if(p_roll_param == NULL)
  {
    if(p_info->p_saved == NULL)
    {
      //nothing saved, so just return.
      return;
    }

    //use the saved parameters to create roll surface.
    memcpy(&saved_param, &(p_info->p_saved->param), sizeof(roll_param_t));
  
    str_drawed = p_info->p_saved->str_drawed;
    str_offset = p_info->p_saved->str_off;

    p_roll_param = &saved_param;
  }
  
  //if((p_rsurf == NULL) || (p_param == NULL))
  if(p_rsurf == NULL)
  {
    return;
  }
  
  if(p_rsurf->is_pic)
  {
    color_key = 0;

    //if(!p_rsurf->is_pic_rawdata)
    {
      gui_get_bmp_attr(p_rsurf->pic_data, p_rsurf->is_pic_rawdata, &width, &height);
    }
  }
  else
  {
    if(p_rsurf->p_str == NULL)
    {
      p_node = gui_roll_add(p_rsurf->p_ctrl, p_roll_param,
        handle, addr, 0, RECTH(p_rsurf->rect), &p_rsurf->rect, p_rsurf->context, NULL);      

      MT_ASSERT(p_node != NULL);

      //reset the node by saved parameters.
      if(p_info->p_saved != NULL)
      {
        p_node->blt_x = p_info->p_saved->blt_x;
        p_node->blt_y = p_info->p_saved->blt_y;
        p_node->curn = p_info->p_saved->curn;
        p_node->curn_times = p_info->p_saved->curn_times;
        p_node->is_pause = p_info->p_saved->is_pause;
        p_node->p_list = p_info->p_saved->p_list;
      }
      
      return;
    }

    p_fstyle = rsc_get_fstyle(gui_get_rsc_handle(), p_rsurf->fstyle_idx);

    color_key = (p_fstyle->color ? 0 : 1);

    rsc_get_string_attr(gui_get_rsc_handle(), p_rsurf->p_str, p_fstyle, &width, &height);
  }

  if((width <= RECTW(p_rsurf->rect)) && (p_roll_param->is_force == FALSE))
  {
    p_node = gui_roll_add(p_rsurf->p_ctrl, p_roll_param,
      handle, addr, width, RECTH(p_rsurf->rect), &p_rsurf->rect, p_rsurf->context, NULL);  

    MT_ASSERT(p_node != NULL);
    p_node->sept.is_pic = p_rsurf->is_pic;
    p_node->sept.is_pic_rawdata = p_rsurf->is_pic_rawdata;

    //reset the node by saved parameters.
    if(p_info->p_saved != NULL)
    {
      p_node->blt_x = p_info->p_saved->blt_x;
      p_node->blt_y = p_info->p_saved->blt_y;
      p_node->curn = p_info->p_saved->curn;
      p_node->curn_times = p_info->p_saved->curn_times;
      p_node->is_pause = p_info->p_saved->is_pause;
      p_node->p_list = p_info->p_saved->p_list;
    }
      
    return;
  }

  data_width = width;

  if(p_rsurf->is_pic)
  {
    if(data_width > RECTW(p_rsurf->rect))
    {
      p_rsurf->rect.right = p_rsurf->rect.left + data_width;
    }
  }
  
  width = RECTW(p_rsurf->rect);

  //create roll surface.
  handle = gdi_create_rsurf(p_rsurf->is_topmost,
    RECTW(p_rsurf->rect), RECTH(p_rsurf->rect), &addr);

  MT_ASSERT(handle != 0);
  MT_ASSERT(addr != 0);

  flinger_set_colorkey((void *)handle, color_key);

  //fill roll surface.
  set_rect(&str_rect, 0, 0, RECTW(p_rsurf->rect), RECTH(p_rsurf->rect));

  if(p_roll_param->use_bg)
  {
    flinger_fill_rect((void *)handle, &str_rect, p_roll_param->bg_color);
  }
  else
  {
    flinger_fill_rect((void *)handle, &str_rect, color_key);
  }
  
  hdc = gdi_get_roll_dc(&str_rect, handle);
  MT_ASSERT(hdc != HDC_INVALID);
 
  if(p_rsurf->is_pic)
  {
    str_drawed = gui_draw_picture_offset(hdc, &str_rect, 
    p_rsurf->pic_data, p_rsurf->is_pic_rawdata, str_drawed, data_width, height, (s16 *)&str_offset);
  }
  else
  {  
    draw_style = (p_rsurf->draw_style | MAKE_DRAW_STYLE(STRDRAW_IGNORE_NLINE, 0));

    str_drawed = gui_draw_unistr_offset(hdc, &str_rect, 
      p_rsurf->p_str, p_rsurf->fstyle_idx,
      draw_style, str_drawed, data_width, height, (s16 *)&str_offset);
  }
  gdi_release_roll_dc(hdc);    

  sept.p_str = p_rsurf->p_str;
  sept.str_drawed = str_drawed;
  sept.str_off = str_offset;
  sept.str_width = data_width;
  sept.str_height = height;
  sept.fstyle_id = p_rsurf->fstyle_idx;
  sept.draw_style = draw_style;
  sept.is_pic = p_rsurf->is_pic;
  sept.is_pic_rawdata = p_rsurf->is_pic_rawdata;
  sept.pic_data = p_rsurf->pic_data;
  sept.is_pic = p_rsurf->is_pic;
  sept.is_pic_rawdata = p_rsurf->is_pic_rawdata;
  
  p_node = gui_roll_add(p_rsurf->p_ctrl, p_roll_param, 
    handle, addr, width, RECTH(p_rsurf->rect), &p_rsurf->rect, p_rsurf->context, &sept);

  MT_ASSERT(p_node != NULL);

  if(p_param->style == ROLL_LB)
  {
    p_node->blt_x = RECTW(p_rsurf->rect);
  }

  //reset the node by saved parameters.
  if(p_info->p_saved != NULL)
  {
    p_node->blt_x = p_info->p_saved->blt_x;
    p_node->blt_y = p_info->p_saved->blt_y;
    p_node->curn = p_info->p_saved->curn;
    p_node->curn_times = p_info->p_saved->curn_times;
    p_node->is_pause = p_info->p_saved->is_pause;
    p_node->p_list = p_info->p_saved->p_list;
  }

  return;
}


u32 gui_roll_get_context(void *p_roll_node)
{
  roll_node_t *p_node = (roll_node_t *)p_roll_node;

  MT_ASSERT(p_node != NULL);

  return p_node->context;
}

void gui_roll_add_list(control_t *p_ctrl, u32 content,
  roll_data_t type, roll_param_t *p_param)
{
  gui_roll_t *p_info = NULL;  
  roll_node_t *p_node = NULL;
  roll_list_t *p_list = NULL;
  roll_list_t *p_temp = NULL;
  u32 size = 0;

  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);
  
  p_node = p_info->p_first;
  
  while(p_node != NULL)
  {
    if(p_ctrl == p_node->p_ctrl)
    {
      break;
    }
    else
    {
      p_node = p_node->p_next;
    }
  }  

  if(p_node == NULL)
  {
    return;
  }

  //find latest content node & malloc a new content node.
  p_list = p_node->p_list;
  p_temp = p_node->p_list;
  
  while(p_list != NULL)
  {
    p_temp = p_list;
    p_list = p_list->p_next;
  }

  p_list = mtos_malloc(sizeof(roll_list_t));
  MT_ASSERT(p_list != NULL);

  if(p_temp != NULL)
  {
    p_temp->p_next = p_list;
  }
  else
  {
    p_node->p_list = p_list;
  }

  p_list->p_next = NULL;
  p_list->type = type;

  
  //save roll param
  if(p_param != NULL)
  {
    memcpy(&p_list->param, p_param, sizeof(roll_param_t));
  }
  else
  {
    memcpy(&p_list->param, &p_node->param, sizeof(roll_param_t));
  }

  //save conteng type.
  switch(type)
  {
    case ROLL_STRID:
    case ROLL_HEX:
    case ROLL_DEC:   
      p_list->content = content;
      break;

    case ROLL_ASCSTR:
      size = strlen((char *)content);
      p_list->content = (u32)mtos_malloc(size);
      MT_ASSERT(p_list->content != 0);
      
      memcpy((u8 *)p_list->content, (u8 *)content, size);
      break;

    case ROLL_UNISTR:
      size = uni_strlen((u16 *)content);
      p_list->content = (u32)mtos_malloc(size * 2);
      MT_ASSERT(p_list->content != 0);
      
      uni_strncpy((u16 *)p_list->content, (u16 *)content, size);
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return;
}

BOOL gui_save_roll(control_t *p_ctrl)
{
  gui_roll_t *p_info = NULL;
  roll_node_t *p_node = NULL;
  BOOL is_rolling = FALSE;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_info->p_saved == NULL);

  p_node = p_info->p_first;

  while(p_node != NULL)
  {
    if(p_ctrl == p_node->p_ctrl)
    {
      is_rolling = TRUE;
      break;
    }
    else
    {
      p_node = p_node->p_next;
    }
  }    

  if(is_rolling == FALSE)
  {
    return FALSE;
  }

  p_info->p_saved = mtos_malloc(sizeof(roll_saved_t));
  MT_ASSERT(p_info->p_saved != NULL);

  memset(p_info->p_saved, 0, sizeof(roll_saved_t));

  p_info->p_saved->blt_x = p_node->blt_x;
  p_info->p_saved->blt_y = p_node->blt_y;
  p_info->p_saved->curn = p_node->curn;
  p_info->p_saved->curn_times = p_node->curn_times;
  p_info->p_saved->is_pause = p_node->is_pause;
  p_info->p_saved->p_list = p_node->p_list;
  p_info->p_saved->str_drawed = p_node->sept.last_drawed;
  p_info->p_saved->str_off = p_node->sept.last_off;
  memcpy(&(p_info->p_saved->param), &(p_node->param), sizeof(roll_param_t));  

  return TRUE;
}

void gui_free_roll(void)
{
  gui_roll_t *p_info = NULL;
  
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  MT_ASSERT(p_info->p_saved != NULL);

  mtos_free(p_info->p_saved);

  p_info->p_saved = NULL;
}

void gui_roll_set_pps(u8 pps)
{
  gui_roll_t *p_info = NULL;


  MT_ASSERT(pps != 0);
  p_info = (gui_roll_t *)class_get_handle_by_id(ROLL_CLASS_ID);
  MT_ASSERT(p_info != NULL);

  p_info->pps = pps;
}


