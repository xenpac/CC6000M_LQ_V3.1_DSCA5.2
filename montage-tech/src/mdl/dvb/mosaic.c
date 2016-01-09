/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include  "sys_types.h"
#include  "sys_define.h"

#include "lib_util.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "common.h"
#include "audio.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "vdec.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "err_check_def.h"

/*!
  conver bcd
  */
#define BCD_CONVER(hight, low) \
  (((((u8)hight) & 0xF0) >> 4) * 1000 + (((u8)hight) & 0x0F) * 100 +\
  ((((u8)low) & 0xF0) >> 4) * 10 + (((u8)low) & 0x0F)) 


/*!
    For mosaic/nvod parse.  
  */
typedef struct tag_nvod_mosaic_buf
{
  /*!
    buffer addr
    */
  u32 buf_addr;
  /*!
    total Size of buffer
    */
  u32 total_size;
  /*!
    used buffer size
    */
  u32 used_size;
}nvod_mosaic_buf_t;

/*!
    mosaic list.  
  */
typedef struct tag_mosaic_list
{
  /*!
    mosaic des svc id
    */
  u16 svc_id;
  /*!
    dummy
    */
  u16 rev;
  /*!
    mosaic
    */
  mosaic_t *p_mosaic;
  /*!
    used buffer size
    */
  struct tag_mosaic_list *p_next;
}mosaic_list_t;

/*!
  nvod_mosaic_priv
  */
typedef struct tag_nvod_mosaic_priv
{
  /*!
    buffer addr
    */
  u32 buf_addr;
  /*!
    total Size of buffer
    */
  u32 total_size;
  /*!
    used buffer size
    */
  u32 used_size;
  /*!
    mosaic
    */
  mosaic_list_t *p_mosaic_list;
}nvod_mosaic_priv_t;

void parse_mosaic_des(u8 *p_buf, mosaic_t *p_des)
{
  u8 loopi = 0;
  u8 loopj = 0;
  u8 descriptor_len = p_buf[loopi ++];
  //u8 *p_data = NULL;
  u8 e_cell_cnt = 0;
  u8 e_cell_id = 0;
  u8 top_left_e_cell_id = 0;
  u8 bottom_right_e_cell_id = 0;
  u8 idx = 0;
 
  p_des->mosaic_entry_point = ((p_buf[loopi] & 0x80) >> 7);
  p_des->h_e_cells = ((p_buf[loopi] & 0x70) >> 4) + 1;
  p_des->v_e_cells = (p_buf[loopi ++] & 0x07) + 1;
  p_des->lg_cell_cnt = 0;

  while (loopi < descriptor_len)
  {
    idx = ((p_buf[loopi ++] & 0xFC) >> 2);
    CHECK_FAIL_RET_VOID(idx < (p_des->h_e_cells * p_des->v_e_cells));
    p_des->lg_cell[idx].logic_cell_id = idx;
    p_des->lg_cell[idx].logical_cell_info = (p_buf[loopi ++] & 0x07);
    e_cell_cnt = p_buf[loopi ++];
    
    top_left_e_cell_id = (p_buf[loopi] & 0x3F);
    bottom_right_e_cell_id = (p_buf[loopi] & 0x3F);
    
    for (loopj = 1; loopj < e_cell_cnt; loopj++)
    {
      e_cell_id = (p_buf[loopj + loopi] & 0x3F);
      if ((((e_cell_id % p_des->h_e_cells) <= (top_left_e_cell_id % p_des->h_e_cells))
        && ((e_cell_id / p_des->h_e_cells) < (top_left_e_cell_id / p_des->h_e_cells)))
        || (((e_cell_id % p_des->h_e_cells) < (top_left_e_cell_id % p_des->h_e_cells))
        && ((e_cell_id / p_des->h_e_cells) <= (top_left_e_cell_id / p_des->h_e_cells))))
      {
        top_left_e_cell_id = e_cell_id;
      }
      
      if ((((e_cell_id % p_des->h_e_cells) >= (bottom_right_e_cell_id % p_des->h_e_cells))
        && ((e_cell_id / p_des->h_e_cells) > (bottom_right_e_cell_id / p_des->h_e_cells)))
        || (((e_cell_id % p_des->h_e_cells) > (bottom_right_e_cell_id % p_des->h_e_cells))
        && ((e_cell_id / p_des->h_e_cells) >= (bottom_right_e_cell_id / p_des->h_e_cells))))
      {
        bottom_right_e_cell_id = e_cell_id;
      }
    }
    p_des->lg_cell[idx].top_left_e_cell_id = top_left_e_cell_id;
    p_des->lg_cell[idx].bottom_right_e_cell_id = bottom_right_e_cell_id;
    loopi += e_cell_cnt;
    p_des->lg_cell[idx].cell_link_info = p_buf[loopi ++];

    switch (p_des->lg_cell[idx].cell_link_info)
    {
    case MOSAIC_LINK_BOUQUET:
      p_des->lg_cell[idx].lnk.bqt.bouquet_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      break;
    case MOSAIC_LINK_SVC:
      p_des->lg_cell[idx].lnk.svc.orig_network_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.svc.stream_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.svc.svc_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      break;
    case MOSAIC_LINK_MOSAIC:
      p_des->lg_cell[idx].lnk.msc.orig_network_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.msc.stream_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.msc.svc_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      break;
    case MOSAIC_LINK_EVENT:
      p_des->lg_cell[idx].lnk.evt.orig_network_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.evt.stream_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.evt.svc_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      p_des->lg_cell[idx].lnk.evt.evt_id = MAKE_WORD2(p_buf[loopi], p_buf[loopi + 1]);
      loopi += 2;
      break;
      
    default:
      break;
    }
    p_des->lg_cell_cnt ++;
  }
}

void parse_mosaic_logic_screen_des(u8 *p_buf, mosaic_t *p_des)
{
  if (p_buf[0] == 8)
  {
    p_des->screen_left = BCD_CONVER(p_buf[1], p_buf[2]);
    p_des->screen_top = BCD_CONVER(p_buf[3], p_buf[4]);
    p_des->screen_right = BCD_CONVER(p_buf[5], p_buf[6]);
    p_des->screen_bottom = BCD_CONVER(p_buf[7], p_buf[8]);
  }
}

void parse_mosaic_linkage_des(u8 *p_buf, mosaic_t *p_des)
{
  switch (p_buf[7])
  {
  case MOSAIC_LINK_SVC_UP:
    //p_des->pre_orig_network_id = MAKE_WORD2(p_buf[1], p_buf[2]);
    //p_des->pre_stream_id = MAKE_WORD2(p_buf[3], p_buf[4]);
    p_des->pre_svc_id = MAKE_WORD2(p_buf[5], p_buf[6]);
    break;
  case MOSAIC_LINK_SVC_DOWN:
    //p_des->nxt_orig_network_id = MAKE_WORD2(p_buf[1], p_buf[2]);
    //p_des->nxt_stream_id = MAKE_WORD2(p_buf[3], p_buf[4]);
    p_des->nxt_svc_id = MAKE_WORD2(p_buf[5], p_buf[6]);
    break;
    
  default:
    break;
  }
}

mosaic_t * alloc_mosaic_buf(class_handle_t handle, u16 svc_id)
{
  nvod_mosaic_priv_t *p_this = handle;
  mosaic_list_t *p_list = NULL;
  mosaic_list_t *p_prev_list = NULL;
  u32 list_size = 0;

  if (p_this->p_mosaic_list)
  {
    p_list = p_this->p_mosaic_list;
    while (p_list)
    {
      if (p_list->svc_id == svc_id)
      {
        OS_PRINTF("\n##debug: alloc_mosaic_buf same svc!!!\n");
        return p_list->p_mosaic;
      }
      p_prev_list = p_list;
      p_list = p_list->p_next;
    }
  }
  
  list_size = sizeof(mosaic_list_t) + sizeof(mosaic_t);
  if ((p_this->total_size - p_this->used_size) >= list_size)
  {
    p_list = (mosaic_list_t *)(p_this->buf_addr + p_this->used_size);
    p_list->p_mosaic = (mosaic_t *)((u8 *)p_list + sizeof(mosaic_list_t));
    memset(p_list->p_mosaic, 0, sizeof(mosaic_t));
    p_list->p_next = NULL;
    p_list->svc_id = svc_id;
    p_this->used_size += list_size;
  }
  else
  {
    OS_PRINTF("\n##debug: alloc_mosaic_buf fail!!!\n");
    return NULL;
  }
  
  if (p_prev_list)
  {
    p_prev_list->p_next = p_list;
  }
  else
  {
    p_this->p_mosaic_list = p_list;
  }
  
  return p_list->p_mosaic;
}

nvod_reference_descr_t *alloc_nvod_buf(class_handle_t handle, u8 cnt)
{
  nvod_mosaic_priv_t *p_this = handle;
  nvod_reference_descr_t *p_nvod_des = NULL;

  if ((p_this->total_size - p_this->used_size) >= (cnt * sizeof(nvod_reference_descr_t)))
  {
    p_nvod_des = (nvod_reference_descr_t *)(p_this->buf_addr + p_this->used_size);
    p_this->used_size += (cnt * sizeof(nvod_reference_descr_t));
  }
  else
  {
    OS_PRINTF("\n##debug: alloc_nvod_buf fail!!!\n");
  }

  return p_nvod_des;
}

void nvod_mosaic_buf_reset(class_handle_t handle)
{
  nvod_mosaic_priv_t *p_this = handle;

  if (p_this && p_this->total_size)
  {
    p_this->used_size = 0;
    memset((void *)p_this->buf_addr, 0, p_this->total_size);
    p_this->p_mosaic_list = NULL;
  }
}

void nvod_mosaic_buf_init(u32 buf_size)
{
  nvod_mosaic_priv_t *p_this = mtos_malloc(sizeof(nvod_mosaic_priv_t));

  CHECK_FAIL_RET_VOID(p_this != NULL);
  p_this->p_mosaic_list = NULL;
  if (buf_size)
  {
    p_this->buf_addr = (u32)mtos_malloc(buf_size);
    CHECK_FAIL_RET_VOID(p_this->buf_addr != 0);
    p_this->total_size = buf_size;
    p_this->used_size = 0;
  }
  else
  {
    p_this->buf_addr = 0;
    p_this->total_size = 0;
    p_this->used_size = 0;
  }
  class_register(NVOD_MOSAIC_CLASS_ID, p_this);
}
