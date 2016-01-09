/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "stdlib.h"
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"

#include "dmx.h"

#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "rc_xml.h"
#include "httpd_svc.h"
#include "ap_framework.h"
#include "ap_rc.h"

#include "mosaic.h"
#include "dvb_svc.h"
#include "pmt.h"

#include "data_base.h"
#include "db_dvbs.h"

#include "common.h"
#include "drv_dev.h"
#include "lib_rect.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "ui_systatus_api.h"

#include "iconv_ext.h"

#include "mem_cfg.h"
#include "mem_manager.h"

extern iconv_t g_cd_utf16le_to_utf8;


void *ext_buf_list;
#define PTI_BUF_SIZE (64 * 1024)
#define MAX_TABLE_REQ (4)


void _init(rc_init_param_t *p_init_param)
{
 // u8 *p_block_addr = NULL;
//  u32 block_size = 0;
 // filter_ext_buf_t *p_filter_ext_buf = NULL;
//  u8 buf_cnt = 0;

  
  p_init_param->max_connect_num = 1;
  p_init_param->enable_epg = 0;
  #if 0
  ext_buf_list = mtos_malloc(sizeof(filter_ext_buf_t) * MAX_TABLE_REQ);
  


  MT_ASSERT(ext_buf_list != NULL);
  memset(ext_buf_list, 0, sizeof(filter_ext_buf_t) * MAX_TABLE_REQ);
  
  p_block_addr = (u8*)mem_mgr_require_block(BLOCK_EPG_BUFFER,  SYS_MODULE_EPG);
  block_size = mem_mgr_get_block_size(BLOCK_EPG_BUFFER);
  memset(p_block_addr, 0, block_size);

  
  p_filter_ext_buf = (filter_ext_buf_t *)(ext_buf_list);

  //Add PTI buffer into FIFO
 for(buf_cnt = 0; buf_cnt < MAX_TABLE_REQ; buf_cnt ++)
  {    
    p_filter_ext_buf[buf_cnt].p_buf = p_block_addr;
    p_filter_ext_buf[buf_cnt].p_next = NULL;
    p_filter_ext_buf[buf_cnt].size = PTI_BUF_SIZE;    
    p_block_addr += PTI_BUF_SIZE;
  }
 #endif
}

void _get_channel_list(u16 page_idx, rc_xml_pg_root_t *p_root)
{
  u8 *p_array = NULL;
  u8 view_id = 0;
  u16 count = 0;
  u16 start = 0;
  u16 i = 0;
  u16 res = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t  pg = {0};
  rc_xml_pg_t *p_pg = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u8 out[32] = {0};
  sat_node_t  sat = {0};
  dvbs_tp_node_t tp = {0};

  
  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(u16));

  start = page_idx * 10;
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TV, 0, (u8 *)p_array);
  count = db_dvbs_get_count(view_id);

  
  p_root->cur_page = page_idx;
  p_root->total_page = (count + 9) / 10;

  res = count - start;
  if(res > 10)
  {
    res = 10;
  }
  p_root->pg_num = (u8)res;
  for(i = 0; i < res; i ++)
  {
    p_pg = p_root->p_list+ i;
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i + start);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    db_dvbs_get_sat_by_id(pg.sat_id , &sat);
    db_dvbs_get_tp_by_id(pg.tp_id, &tp);
    
    p_pg->id = pg.id;
    p_pg->scramble = pg.is_scrambled;
    p_pg->sid = pg.s_id;
    p_pg->org_nw_id = pg.orig_net_id;
    p_pg->stream_id = pg.ts_id;
    p_pg->sat_id = pg.sat_id;
    p_pg->tp_id = pg.tp_id;
    
    if(pg.video_pid != 0)
    {
       p_pg->tv = 1;
    }
    else 
    {
      p_pg->tv = 0;
    }

    inbuf = (char *)pg.name;
    outbuf = (char *)out;
    src_len = sizeof(pg.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

    strcpy(p_pg->name, out);
    p_pg->freq = tp.freq;
    p_pg->sym = tp.sym;
    p_pg->pol = tp.polarity;
    
    inbuf = (char *)sat.name;
    outbuf = (char *)out;
    src_len = sizeof(sat.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    
    strcpy(p_pg->sat_name, out);
    
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  
}

void _get_channel_list_by_id(u16 pg_start, u8 cnt, rc_xml_pg_root_by_id_t *p_root)
{
  u8 *p_array = NULL;
  u8 view_id = 0;
  u16 total = 0;
  u16 start = 0;
  u16 count = 0;
  u16 i = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t  pg = {0};
  rc_xml_pg_t *p_pg = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u8 out[32] = {0};
  sat_node_t  sat = {0};
  dvbs_tp_node_t tp = {0};
  
  
  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(u16));

  start = pg_start;
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TV, 0, (u8 *)p_array);
  total = db_dvbs_get_count(view_id);
  count = cnt > MAX_PG_PER_PACKET ? MAX_PG_PER_PACKET : cnt;
  count =  count > total ? total : count;

  p_root->pg_id_start = pg_start;
  p_root->cnt = count;
  p_root->total_program = total;
  
  for(i = 0; i < count; i ++)
  {
    p_pg = p_root->p_list+ i;
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i + start);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    db_dvbs_get_sat_by_id(pg.sat_id , &sat);
    db_dvbs_get_tp_by_id(pg.tp_id, &tp);
    
    p_pg->id = pg.id;
    p_pg->scramble = pg.is_scrambled;
    p_pg->sid = pg.s_id;
    p_pg->org_nw_id = pg.orig_net_id;
    p_pg->stream_id = pg.ts_id;
    p_pg->sat_id = pg.sat_id;
    p_pg->tp_id = pg.tp_id;

    if(pg.video_pid != 0)
    {
       p_pg->tv = 1;
    }
    else 
    {
      p_pg->tv = 0;
    }

    inbuf = (char *)pg.name;
    outbuf = (char *)out;
    src_len = sizeof(pg.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

    strcpy(p_pg->name, out);
    p_pg->freq = tp.freq;
    p_pg->sym = tp.sym;
    p_pg->pol = tp.polarity;
    
    inbuf = (char *)sat.name;
    outbuf = (char *)out;
    src_len = sizeof(sat.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    
    strcpy(p_pg->sat_name, out);
    
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  
}

void _get_tp_channel_list(u16 page_idx, u16 tp_id, rc_xml_pg_root_t *p_root)
{
  u8 *p_array = NULL;
  u8 view_id = 0;
  u16 count = 0;
  u16 start = 0;
  u16 i = 0;
  u16 res = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t  pg = {0};
  rc_xml_pg_t *p_pg = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u8 out[32] = {0};
  sat_node_t  sat = {0};
  dvbs_tp_node_t tp = {0};

  
  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(u16));

  start = page_idx * 10;
  view_id = db_dvbs_create_view(DB_DVBS_TP_PG, tp_id, (u8 *)p_array);
  count = db_dvbs_get_count(view_id);

  p_root->cur_page = page_idx;
  p_root->total_page = (count + 9) / 10;

  res = count - start;
  if(res > 10)
  {
    res = 10;
  }
  p_root->pg_num = res;
  for(i = 0; i < res; i ++)
  {
    p_pg = p_root->p_list+ i;
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i + start);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    db_dvbs_get_sat_by_id(pg.sat_id , &sat);
    db_dvbs_get_tp_by_id(pg.tp_id, &tp);
    
    p_pg->id = pg.id;
    p_pg->scramble = pg.is_scrambled;
    p_pg->sid = pg.s_id;
    p_pg->org_nw_id = pg.orig_net_id;
    p_pg->stream_id = pg.ts_id;
    p_pg->sat_id = pg.sat_id;
    p_pg->tp_id = tp_id;

    if(pg.video_pid != 0)
    {
       p_pg->tv = 1;
    }
    else 
    {
      p_pg->tv = 0;
    }

    inbuf = (char *)pg.name;
    outbuf = (char *)out;
    src_len = sizeof(pg.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

    strcpy(p_pg->name, out);
    p_pg->freq = tp.freq;
    p_pg->sym = tp.sym;
    p_pg->pol = tp.polarity;
    
    inbuf = (char *)sat.name;
    outbuf = (char *)out;
    src_len = sizeof(sat.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    
    strcpy(p_pg->sat_name, out);
    
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  
}


void _get_tp_channel_list_by_id(u16 pg_start, u8 cnt, u16 tp_id, rc_xml_pg_root_by_id_t *p_root)
{
  u8 *p_array = NULL;
  u8 view_id = 0;
  u16 total = 0;
  u16 start = 0;
  u16 count = 0;
  u16 i = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t  pg = {0};
  rc_xml_pg_t *p_pg = NULL;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u8 out[32] = {0};
  sat_node_t  sat = {0};
  dvbs_tp_node_t tp = {0};
  
  
  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(u16));

  start = pg_start;
  view_id = db_dvbs_create_view(DB_DVBS_TP_PG, tp_id, (u8 *)p_array);
  total = db_dvbs_get_count(view_id);
  count = cnt > MAX_PG_PER_PACKET ? MAX_PG_PER_PACKET : cnt;
  count =  count > total ? total : count;

  p_root->pg_id_start = pg_start;
  p_root->cnt = count;
  p_root->total_program = total;
  
  for(i = 0; i < count; i ++)
  {
    p_pg = p_root->p_list+ i;
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i + start);
    db_dvbs_get_pg_by_id(pg_id, &pg);
    db_dvbs_get_sat_by_id(pg.sat_id , &sat);
    db_dvbs_get_tp_by_id(pg.tp_id, &tp);
    
    p_pg->id = pg.id;
    p_pg->scramble = pg.is_scrambled;
    p_pg->sid = pg.s_id;
    p_pg->org_nw_id = pg.orig_net_id;
    p_pg->stream_id = pg.ts_id;
    p_pg->sat_id = pg.sat_id;
    p_pg->tp_id = tp_id;

    if(pg.video_pid != 0)
    {
       p_pg->tv = 1;
    }
    else 
    {
      p_pg->tv = 0;
    }

    inbuf = (char *)pg.name;
    outbuf = (char *)out;
    src_len = sizeof(pg.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

    strcpy(p_pg->name, out);
    p_pg->freq = tp.freq;
    p_pg->sym = tp.sym;
    p_pg->pol = tp.polarity;
    
    inbuf = (char *)sat.name;
    outbuf = (char *)out;
    src_len = sizeof(sat.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    
    strcpy(p_pg->sat_name, out);
    
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  
}


BOOL _get_cur_play_info(rc_xml_pg_t *p_pg)
{
  dvbs_prog_node_t pg_node = {0};
  sat_node_t  sat = {0};
  dvbs_tp_node_t tp = {0};
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u8 out[32] = {0};
  
  u16 pg_id = sys_status_get_curn_group_curn_prog_id();
  
  if(pg_id != INVALIDID)
  {
    db_dvbs_get_pg_by_id(pg_id, &pg_node);
    db_dvbs_get_sat_by_id(pg_node.sat_id , &sat);
    db_dvbs_get_tp_by_id(pg_node.tp_id, &tp);
    p_pg->id = pg_id;
    p_pg->scramble = pg_node.is_scrambled;
    p_pg->sid = pg_node.s_id;
    p_pg->org_nw_id = pg_node.orig_net_id;
    p_pg->stream_id = pg_node.ts_id;
    p_pg->sat_id = pg_node.sat_id;
    p_pg->tp_id = pg_node.tp_id;

    if(pg_node.video_pid != 0)
    {
       p_pg->tv = 1;
    }
    else 
    {
      p_pg->tv = 0;
    }

    inbuf = (char *)pg_node.name;
    outbuf = (char *)out;
    src_len = sizeof(pg_node.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

    
    strcpy(p_pg->name, out);
    p_pg->freq = tp.freq;
    p_pg->sym = tp.sym;
    p_pg->pol = tp.polarity;
    
    inbuf = (char *)sat.name;
    outbuf = (char *)out;
    src_len = sizeof(sat.name);
    dest_len = sizeof(out);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    
    strcpy(p_pg->sat_name, out);
    return TRUE;
  }
  return FALSE;
}

u32 _get_ext_buf(u8 idx)
{
  filter_ext_buf_t *p_filter_ext_buf = (filter_ext_buf_t *)(ext_buf_list);

  return (u32)&p_filter_ext_buf[idx];
}

rc_policy_t *construct_rc_policy(void)
{

  rc_policy_t *p_rc_pol = mtos_malloc(sizeof(rc_policy_t));
  MT_ASSERT(p_rc_pol != NULL);
  memset(p_rc_pol, 0, sizeof(rc_policy_t));


  p_rc_pol->init = _init;
  p_rc_pol->get_channel_list= _get_channel_list;
  p_rc_pol->get_channel_list_by_id = _get_channel_list_by_id;
  p_rc_pol->get_tp_channel_list = _get_tp_channel_list;
  p_rc_pol->get_tp_channel_list_by_id = _get_tp_channel_list_by_id;
  p_rc_pol->get_cur_play_info = _get_cur_play_info;
  p_rc_pol->get_ext_buf_addr = _get_ext_buf;
  return p_rc_pol;
}
