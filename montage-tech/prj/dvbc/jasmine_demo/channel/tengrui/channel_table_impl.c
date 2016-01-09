/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "sys_regs_jazz.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"

#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "pmt.h"
#include "nit.h"
#include "cat.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "db_dvbs.h"
#include "mt_time.h"

#include "ss_ctrl.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_epg.h"
//#include "ap_satcodx.h"
//#include "ap_ota.h"
#include "sys_status.h"
#include "hal_base.h"
#include "smc_op.h"
#include "lpower.h"
//#include "customer_def.h"
//#include "config_customer.h"
//#include "config_prj.h"
#include "db_dvbs.h"
#include "ui_dbase_api.h"
#include "ap_table_parse.h"
#include "ui_systatus_api.h"
#include "user_parse_table.h"

typedef  struct
{
  u8 stream_type;
  u16 pid;
}stream_desc_t;


typedef struct
{
  u32 freq;
  u8 modulation;
  u32 sym;
  u16 ts_id;
  u16 net_id;
  u16 s_id;
  u16 name_len;
  char name[25];
  u16 logic_num;
  u8 service_type;
  u8 is_scrable;
  u16 bouguet_id; 
  u8  babance;
  u8 volume;
  u16 stream_type_len;
  stream_desc_t stream[12];

}channel_service_t;

typedef struct
{
  u32 frq;
  u32 sym;
} tp_node_t;

typedef struct
{
  u32 version;
  channel_service_t svc[256];
  u8  total_svc_num;
  tp_node_t tp_node[255];
  u16 offset;
  u8 *p_buf;
}channel_table_t;

#define SVC_TYPE_TV                    (0x01)
#define SVC_TYPE_RADIO                 (0x02)

static channel_table_t table =  {0};
static table_section_t section = {0};
#if 0
static void dump_info(channel_service_t *p_svc)
{
  OS_PRINTF("------------------------------------\n");

  OS_PRINTF("freq %d\t",p_svc->freq);
  OS_PRINTF("modu %d\t",p_svc->modulation);
  OS_PRINTF("sym %d\t",p_svc->sym);
  OS_PRINTF("ts_id %d\t",p_svc->ts_id);
  OS_PRINTF("net id %d\t",p_svc->net_id);
  OS_PRINTF("sid %d\t",p_svc->s_id);
  OS_PRINTF("name %s\t",p_svc->name);
  OS_PRINTF("logic %d\t",p_svc->logic_num);
  OS_PRINTF("s type %d\t",p_svc->service_type);
  OS_PRINTF("scramble %d\t",p_svc->is_scrable);
  OS_PRINTF("boq %d\t",p_svc->bouguet_id);
  OS_PRINTF("vol %d\t",p_svc->volume);
  OS_PRINTF("vpid  %d\t",p_svc->stream[0].pid);
  OS_PRINTF("apid  %d\t",p_svc->stream[1].pid);
  OS_PRINTF("pcr %d\t",p_svc->stream[9].pid);
  OS_PRINTF("-----------------------------------------\n");
  

}

#endif
void request_channel_sec(dvb_section_t *p_sec, u32 para1, u32 para2)
{  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  u16 table_id = para2 >> 16 & 0xffff;
  u16 sec_num = para2 & 0xffff;
  /*!
    para1 and para2 should be used and one of them should be table id
    */
  MT_ASSERT(p_dvb_handle != NULL);
//  OS_PRINTF("sec num in request %d\n", sec_num);
  p_sec->pid = (u16)para1;
  p_sec->timeout  = REQUEST_CHANNEL_SEC_TIMEOUT;
  
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);  

  p_sec->filter_code[0] = (table_id & 0xF0);
  p_sec->filter_mask[0] = 0xF0;
#ifdef WIN32  
  p_sec->filter_code[6] = (u8 )sec_num;
  p_sec->filter_mask[6] = 0xFF; 
#else
  p_sec->filter_code[4] = (u8 )sec_num;
  p_sec->filter_mask[4] = 0xFF; 
#endif
  p_sec->filter_mask_size = 2;
  p_sec->filter_mode    = FILTER_TYPE_SECTION;

  p_sec->interval_time = 0;
  p_sec->crc_enable = 1;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void parse_channel_sec(handle_t handle, dvb_section_t *p_sec)
{
  u8          *p_buf   = p_sec->p_buffer;
  u8          *p_data  = NULL;
  
  service_t *p_svc = handle;
  os_msg_t    msg      = {0};
//  u32 cls_des_len = 0;
  s32 sec_len = 0;
//  u8 cnt = 0, i = 0;
//  u16 offset = 0;
  
  section.table_id = p_buf[0];
  section.section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0x0f);
  section.version = p_buf[5];
  section.section_num = p_buf[6];
  section.last_section = p_buf[7];
  
//  OS_PRINTF("parsing channel list section  last sec %d\n", section.last_section);
  
  p_data = p_buf + 8;
  sec_len = section.section_len;

  //if(channel.section_num != channel.last_section)
  {
    // copy data
    memcpy(table.p_buf + table.offset, p_data, section.section_len - 9);
    table.offset += (u16)section.section_len;
    table.offset -= 9;
  }

  table.version = section.version;
    
  msg.content = DVB_DTM_FOUND;
  msg.para1   = (u32)&section;
  msg.para2   = sizeof(table_section_t);
  msg.context = p_sec->r_context;

  p_svc->notify(p_svc, &msg);

}


static void parse_channel(void )
{
  u16 sec_len = table.offset;
  u16 offset = 0;
  u8          *p_data  = table.p_buf;
  u8 cnt = 0, i = 0;

  while(sec_len >= 90)
  { 
    offset = 0;
 //   cls_des_len = MAKE_WORD(p_data[offset + 1],p_data[offset]);
    offset +=2;
    table.svc[cnt].freq = MT_MAKE_DWORD(MAKE_WORD(p_data[offset + 3], p_data[offset + 2]), \
      MAKE_WORD(p_data[offset + 1], p_data[offset]));
    offset += 4;
    table.svc[cnt].modulation = p_data[offset];
    offset += 1;
    table.svc[cnt].sym = MT_MAKE_DWORD(MAKE_WORD(p_data[offset + 3], p_data[offset + 2]), \
      MAKE_WORD(p_data[offset + 1], p_data[offset]));
    offset += 4;
    table.svc[cnt].ts_id = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    table.svc[cnt].net_id = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    table.svc[cnt].s_id = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    table.svc[cnt].name_len = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;

 //   MT_ASSERT(channel.svc[cnt].name_len <= 25);
    memcpy(table.svc[cnt].name, &p_data[offset], 25);
    offset += table.svc[cnt].name_len;
    table.svc[cnt].logic_num = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    table.svc[cnt].service_type = p_data[offset];
    offset += 1;
    table.svc[cnt].is_scrable  = p_data[offset];
    offset += 1;
    table.svc[cnt].bouguet_id  = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    table.svc[cnt].babance  = p_data[offset];
    offset += 1;
    table.svc[cnt].volume   = p_data[offset];
    offset += 1;
    table.svc[cnt].stream_type_len  = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;

    for(i = 0; i < 12; i ++)
    {
      table.svc[cnt].stream[i].stream_type = p_data[offset];
      offset += 1;
      table.svc[cnt].stream[i].pid = MAKE_WORD(p_data[offset + 1], p_data[offset]);
      offset += 2;
    }
    offset += 4;
    p_data += 90;//(cls_des_len + 1);
 //   break;
 //   dump_info(&table.svc[cnt]);
    cnt ++;
    
    sec_len -= 90;

    if(sec_len < 90)
    {
      break;
    }
  }

  table.total_svc_num = cnt;

}

static BOOL get_tp_pos(u8 view_id, u16 freq, u16 sym, u16 *p_sim_idx)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(view_id);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(view_id, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if((old_tp.freq == freq*10)
      && (old_tp.sym == sym))
    {
      *p_sim_idx = pos;
      return FALSE;
    }
  }
  return TRUE;
}

static BOOL is_new_tp(u8 vv_id_tp, u16 freq, u16 sym, u16 *p_index)
{
  u16 i = 0;

  for(i = 0; i < 255; i ++)
  {
    if(freq == table.tp_node[i].frq
      && sym == table.tp_node[i].sym)
    {
      get_tp_pos(vv_id_tp, freq, sym, p_index);
      return FALSE;
    }
    if( table.tp_node[i].frq == 0)
    {
       table.tp_node[i].frq = freq;
       table.tp_node[i].sym = sym;
      return TRUE;
    }
  }

  return FALSE;
}

static u16 get_tp_id_by_freq(  u32 freq,  u8 modulation,  u32 sym)
{
  u8 vv_id_tp = 0;
  u16 cnt=0, i =0, tp_id =0;
  dvbs_tp_node_t tp = {0};
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  cnt = db_dvbs_get_count(vv_id_tp);  
 // OS_PRINTF("\n total tp cnt :%d",cnt);
  
  for(i=0;i<cnt;i++)
  {
    tp_id = db_dvbs_get_id_by_view_pos(vv_id_tp,i);
    db_dvbs_get_tp_by_id(tp_id,&tp);
    
    if((tp.freq == freq*10) && (tp.sym == sym) )
    {
     //   OS_PRINTF("\n Get the tp id \n ");
        return (u16)tp.id;
    }
  }
  return 0;
}

static void ui_set_curnet_status_pg(u8 view_id)
{
      u16 pg_id = 0xFFFF, pg_pos = 0;
      dvbs_prog_node_t prog = {0};
      u8 curn_mode = CURN_MODE_NONE;

      ui_dbase_set_pg_view_id(view_id);
      if(db_dvbs_get_count(view_id)>0)
      {
      pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
      pg_pos = 0;

      db_dvbs_get_pg_by_id(pg_id, &prog);

      if((BOOL)prog.tv_flag)
      {
      curn_mode = CURN_MODE_TV;
      }
      else
      {
      curn_mode = CURN_MODE_RADIO;
      }

      sys_status_set_curn_prog_mode(curn_mode);
      sys_status_set_curn_group_info(pg_id, pg_pos);
  }
}


extern RET_CODE dvb_to_unicode(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen);
static void save_prog(channel_table_t *p_channel)
{
  u8 vv_id_tp = 0 ,vv_id_pro = 0;
  u16 i = 0;
  dvbs_tp_node_t tp = {0};
  dvbs_prog_node_t pg = {0};
  u16 tp_id = 0;

  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);

  for(i = 0; i < p_channel->total_svc_num; i ++)
  {
    memset(&pg, 0, sizeof(dvbs_prog_node_t));
    
    if(is_new_tp(vv_id_tp, (u16)p_channel->svc[i].freq, (u16)p_channel->svc[i].sym, &tp_id))
    {
      // add tp
      tp.freq = p_channel->svc[i].freq*10;
      tp.sym = p_channel->svc[i].sym;
      tp.is_scramble = p_channel->svc[i].is_scrable;

      switch(p_channel->svc[i].modulation)
      {
        case 1:
          tp.nim_modulate = NIM_MODULA_QAM16;
          break;

        case 2:
          tp.nim_modulate = NIM_MODULA_QAM32;
          break;

        case 3:
          tp.nim_modulate = NIM_MODULA_QAM64;
          break;

        case 4:
          tp.nim_modulate = NIM_MODULA_QAM128;
          break;

        case 5:
          tp.nim_modulate = NIM_MODULA_QAM256;
          break;

        default:
          tp.nim_modulate = NIM_MODULA_QAM64;
          break;
      }
      db_dvbs_add_tp(vv_id_tp,&tp);
      db_dvbs_save(vv_id_tp);
    }
    // add prog
    tp_id = get_tp_id_by_freq(p_channel->svc[i].freq,p_channel->svc[i].modulation,
                p_channel->svc[i].sym);
    pg.tp_id = tp_id;
    pg.ts_id = p_channel->svc[i].ts_id;
    pg.orig_net_id = p_channel->svc[i].net_id;
    pg.s_id = p_channel->svc[i].s_id;
  //  pg.logical_num = p_channel->svc[i].logic_num;
    pg.service_type = p_channel->svc[i].service_type;
  if(pg.service_type == SVC_TYPE_TV)
    pg.tv_flag = 1;
    else
      pg.tv_flag = 0;
    pg.is_scrambled = p_channel->svc[i].is_scrable;
//    pg.bouquet_id = p_channel->svc[i].bouguet_id;

    if(p_channel->svc[i].volume>31)
    {
      pg.volume = 31;
    }
    else
    {
      pg.volume = p_channel->svc[i].volume;
    }
    pg.pmt_pid = 0x1fff;

    if(p_channel->svc[i].babance == 0)
      pg.audio_track = 1;
    else if(p_channel->svc[i].babance == 1)
    {
      pg.audio_track = 0;
    }
    else
    {
      pg.audio_track = p_channel->svc[i].babance;
    }

    //memcpy(pg.name, p_channel->svc[i].name, DB_DVBS_MAX_NAME_LENGTH);
    //Transfer name to unicode
    dvb_to_unicode((u8*)p_channel->svc[i].name,
        sizeof(p_channel->svc[i].name), (u16*)pg.name, DB_DVBS_MAX_NAME_LENGTH);
//    pg.name = p_channel->svc[i].name;
    pg.pcr_pid = p_channel->svc[i].stream[9].pid;

    pg.video_pid = p_channel->svc[i].stream[0].pid;
    pg.audio_ch_num = 1;
    pg.audio[0].p_id = p_channel->svc[i].stream[1].pid;
    pg.audio[0].type  = AUDIO_MP2;
    pg.audio[1].p_id = p_channel->svc[i].stream[11].pid;
    pg.audio[1].type = 1; // ac3

    #ifdef DALIAN_ZHUANGGUANG
    if (strcmp(p_channel->svc[i].name, "ÈËÎïÆµµÀ") == 0)
    {
      pg.video_pid = 40;
      pg.audio[0].p_id = 49;
      pg.pcr_pid = 39;
      OS_PRINTF("[TR_CH]Modify pg info!\n");
    }
    #endif
    
    db_dvbs_add_program(vv_id_pro, &pg);
    
    db_dvbs_save(vv_id_pro);
    
  }
  
//  ui_restore_view();
    ui_cache_view();
    create_categories();
    ui_restore_view();
    ui_dbase_pg_sort(DB_DVBS_ALL_PG);
    sys_status_set_curn_group(GROUP_T_ALL);
    ui_set_curnet_status_pg(vv_id_pro);
 
}

void channel_parse_init(void)
{
      table.offset = 0;
      table.version = 0xff;
      table.p_buf = mtos_malloc(20* KBYTES);
}
extern BOOL sys_status_check_group(void);
void channel_table_parse_process(void)
  {
        if(table.version != sys_status_get_channel_version())
        {          
          parse_channel();
          if(table.total_svc_num > 0)
          {
          db_reset_prog_before_copy();
           save_prog(&table);
          sys_status_set_channel_version((u8)table.version);
          }
        }
    }
void channel_parse_release(void)
{
      if(table.p_buf !=NULL)
        {
          mtos_free(table.p_buf );
        }
}

channel_table_parse_policy_t *construct_channel_table_parse_policy(void)
{
  channel_table_parse_policy_t *p_table_parse_policy = mtos_malloc(sizeof(channel_table_parse_policy_t));
  MT_ASSERT(p_table_parse_policy != NULL);
  memset(p_table_parse_policy, 0, sizeof(channel_table_parse_policy_t));
  p_table_parse_policy->channel_bit = 1;
  p_table_parse_policy->pid = 0x1964;
  p_table_parse_policy->table_id = 0;
  p_table_parse_policy->p_channel_init = channel_parse_init;
  p_table_parse_policy->p_channel_process= channel_table_parse_process;
  p_table_parse_policy->p_channel_release = channel_parse_release;
  return p_table_parse_policy;
}


void destruct_channel_table_parse_policy(channel_table_parse_policy_t *p_table_parse_policy)
{
  mtos_free(p_table_parse_policy);
}

