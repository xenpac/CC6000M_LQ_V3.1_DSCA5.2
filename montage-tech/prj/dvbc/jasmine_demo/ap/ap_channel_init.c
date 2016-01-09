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
#ifdef LCN_SWITCH_DS_JHC
#include "nit_logic_num_parse.h"
#endif
#if 1
#define CH_DBG  OS_PRINTF
#else
#define CH_DBG DUMMY_PRINTF
#endif

#define MAX_NAME_LEN  (16)  // name len
#define MAX_COMPONENT_NUM  (16) // component per pg
#define MAX_NVOD_NUM  (10)    // nvod per pg
#define MAX_BOUQUET_NUM  (16)  // bou per tp
#define MAX_SVC_NUM     (25)  // pg per tp
#define MAX_NODE_NUM  (100) // tp

#define REQUEST_CHANNEL_SEC_TIMEOUT 4000
#define REQUEST_NIT_STB_START_SEC_TIMEOUT 10000

typedef struct
{
  u32 freq;
  u32 modulation;
  u32 sym;
}cable_desc_t;

typedef struct
{
  u16 ca_sys_id;
  u16 ca_pid;
}channel_cas_desc_t;

typedef struct
{
  u16 id;
  u8 name[MAX_NAME_LEN + 1];
}bouquet_desc_t;

typedef struct
{
  u8 type;
  u8 provider_name[MAX_NAME_LEN + 1];
  u8 svc_name[MAX_NAME_LEN + 1];
}service_desc_t;

typedef struct
{
  u16 ts_id;
  u16 net_id;
  u16 s_id;
}nvod_desc_t;

typedef struct
{
  u16 s_id;
  u16 event_id;
}timeshift_desc_t;


typedef struct
{
  u16 ch_no;
  u16 sound_channel;
  u8 vol_offset;
}priv_desc_t;

typedef struct
{
  u16 bouquet_id;
}sevice_content_desc_t;


typedef struct
{

  u8 type;
  u16 pid;
  channel_cas_desc_t cas;
}component_desc_t;


typedef struct
{
  u16 s_id;
  u16 pcr_pid;
  service_desc_t service;
  nvod_desc_t nvod[MAX_NVOD_NUM];
  timeshift_desc_t timeshift;
  priv_desc_t setting;
  sevice_content_desc_t s_content;
  component_desc_t component[MAX_COMPONENT_NUM];
  u8  total_component_num;
  u8 total_nvod_num;
}channel_service_t;


typedef struct
{
  u16 ts_id;
  u16 net_id;
  cable_desc_t cable;
  channel_cas_desc_t cas;
  channel_service_t svc[MAX_SVC_NUM];
  u8  total_svc_num;
}channel_node_t;

typedef struct
{
  u32 table_id :8;
  u32 section_syntax_indicator :1;
  u32 reserved :3;
  u32 section_len :12;
  u32 table_id_ext  :16;
  u32 version  :5;
  u32 section_num :8;
  u32 last_section  :8;

  channel_node_t node[MAX_NODE_NUM];
  bouquet_desc_t bouquet[MAX_BOUQUET_NUM];
  u8  total_node_num;
  u8  total_bouquet_num;
}channel_update_section_t;

typedef struct
{
  u32 frq;
  u32 sym;
  u16 idx;
} tp_node_t;


typedef enum
{
  INIT_IDLE,

  INIT_REQUEST_TABLE,

  INIT_WAIT_TABLE,

  INIT_DONE

}channel_init_state_t;


typedef struct
{

  service_t *p_dvb_svc;
  /*!
  NC service handle
  */
  service_t *p_nc_svc;

  channel_init_state_t status;

  u8 cur_sec_num;
  u16 pid;
  u16 table_id;

  u32 msg_id;
  //tp_node_t tp_node[255];
  u8 vv_id_tp;
  u8 vv_id_pro;
  u8 pending_job;
  BOOL table_found_bit;
}channel_init_priv_t;





static channel_init_priv_t g_priv = {0};
static u8 startup_nit_version = 0xff;
static u16 startup_nit_network_id = 0xffff;
static BOOL startup_nit_channel_status = FALSE;

u8 get_channel_init_nit_ver(void)
{
    return startup_nit_version;
}

void set_channel_init_nit_ver(u8 nit_version)
{
    startup_nit_version = nit_version;
}

u16 get_channel_init_nit_network_id(void)
{
    return startup_nit_network_id;
}

void set_channel_init_nit_network_id(u16 network_id)
{
    startup_nit_network_id = network_id;
}

BOOL get_channel_init_channel_status(void)
{
    return startup_nit_channel_status;
}

/*
static BOOL is_new_tp(u32 freq, u32 sym, u32 *p_index)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(g_priv.vv_id_tp);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(g_priv.vv_id_tp, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if((old_tp.freq == freq)
      && (old_tp.sym == sym))
    {
      *p_index = pos;
      return FALSE;
    }
  }
  return TRUE;
}
*/
/*
static void dump_info(channel_update_section_t *p_ch)
{
  u8 i = 0, j = 0;//, k = 0;

  CH_DBG("---------------total node %d---total bouquet %d------------------\n",
    p_ch->total_node_num, p_ch->total_bouquet_num);

  for(j = 0; j < p_ch->total_bouquet_num; j ++)
  {
    CH_DBG("bouquet_id %d\t", p_ch->bouquet[j].id);
    CH_DBG("name %s \t",p_ch->bouquet[j].name);
    CH_DBG("\n");
  }
  CH_DBG("\n");

  for(i = 0; i < p_ch->total_node_num; i ++)
  {
    CH_DBG("freq %d\t", p_ch->node[i].cable.freq);
    CH_DBG("modu %d\t",p_ch->node[i].cable.modulation);
    CH_DBG("sym %d\t",p_ch->node[i].cable.sym);
    CH_DBG("ts_id %d\t",p_ch->node[i].ts_id);
    CH_DBG("net id %d\t",p_ch->node[i].net_id);
    CH_DBG("ca sys id %d\t",p_ch->node[i].cas.ca_sys_id);
    CH_DBG("ca pid %d\t",p_ch->node[i].cas.ca_pid);
    CH_DBG("total svc %d\t",p_ch->node[i].total_svc_num);
    CH_DBG("\n");

    for(j = 0; j < p_ch->node[i].total_svc_num; j ++)
    {
      CH_DBG("s_id %d\t", p_ch->node[i].svc[j].s_id);
      CH_DBG("service type %d\t",p_ch->node[i].svc[j].service.type);
      CH_DBG("type %d  pid %d ca_id %d %d \t",
        p_ch->node[i].svc[j].component[0].type,
        p_ch->node[i].svc[j].component[0].pid,
        p_ch->node[i].svc[j].component[0].cas.ca_sys_id,
        p_ch->node[i].svc[j].component[0].cas.ca_pid);
      CH_DBG("type %d  pid %d ca_id %d %d \t",
        p_ch->node[i].svc[j].component[1].type,
        p_ch->node[i].svc[j].component[1].pid,
        p_ch->node[i].svc[j].component[1].cas.ca_sys_id,
        p_ch->node[i].svc[j].component[1].cas.ca_pid);
      CH_DBG("pcr %d\t",p_ch->node[i].svc[j].pcr_pid);
      CH_DBG("bouquet_id  %d\t",p_ch->node[i].svc[j].s_content.bouquet_id);
      CH_DBG("name  %s\t",p_ch->node[i].svc[j].service.svc_name);
      CH_DBG("\n");
    }
    CH_DBG("\n");
    CH_DBG("\n");
  }

  CH_DBG("-----------------------------------------\n");
 }
*/
void request_channel_sec(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  u16 table_id = para2 >> 16 & 0xffff;
  u16 sec_num = para2 & 0xffff;
  /*!
    para1 and para2 should be used and one of them should be table id
    */
  MT_ASSERT(p_dvb_handle != NULL);
  CH_DBG("sec num in request %d\n", sec_num);
  p_sec->pid = (u16)para1;
  p_sec->timeout  = REQUEST_CHANNEL_SEC_TIMEOUT;//4000;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  p_sec->filter_code[0] = (table_id & 0xFF);
  p_sec->filter_mask[0] = 0xFF;
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

static channel_update_section_t  *p_channel = NULL;

static void dum_bouquet_list(channel_update_section_t *p_ch, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u16 name_len = 0;
  u8 id = 0;
  u8 num = 0;
  while(cnt < len)
  {
      id = MAKE_WORD(p_data[cnt + 1], p_data[cnt]);
      cnt += 2;
      name_len = p_data[cnt];
      cnt += 1;
      p_ch->bouquet[num].id = id;
      name_len = name_len > MAX_NAME_LEN ? MAX_NAME_LEN : name_len;
      memcpy(p_ch->bouquet[num].name, &p_data[cnt], name_len);
      CH_DBG("id %d len %d name %s\n", id, name_len, p_ch->bouquet[num].name);
      cnt += name_len;
      num ++;
  }
  p_ch->total_bouquet_num = num;
}

static void dump_transport_desc(channel_node_t *p_node, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u16 desc_len = 0;
  while(cnt < len)
  {
    switch(p_data[cnt])
    {
      case CA_DESC_ID:
        CH_DBG("CA_DESC_ID \n");
        desc_len = p_data[cnt + 1];
        p_node->cas.ca_sys_id = MAKE_WORD(p_data[cnt + 3], p_data[cnt + 2]);
        p_node->cas.ca_pid = MAKE_WORD(p_data[cnt + 5], p_data[cnt + 4] & 0x1f);
        break;
      case DVB_DESC_CABLE_DELIVERY:
        CH_DBG("DVB_DESC_CABLE_DELIVERY \n");
        desc_len = p_data[cnt + 1];
        p_node->cable.freq = MT_MAKE_DWORD(MAKE_WORD(p_data[cnt + 5], p_data[cnt + 4]), \
      MAKE_WORD(p_data[cnt + 3], p_data[cnt + 2]));
        p_node->cable.modulation = p_data[cnt + 8];
        p_node->cable.sym = MT_MAKE_DWORD(MAKE_WORD(p_data[cnt + 12], p_data[cnt + 11]), \
      MAKE_WORD(p_data[cnt + 10], p_data[cnt + 9] & 0xf));
        CH_DBG("freq %d sym %d mod %d\n",
          p_node->cable.freq, p_node->cable.sym, p_node->cable.modulation);
        break;
       case 0x90: //bouquet_list_descriptor
        {
          CH_DBG("bouquet_list_descriptor \n");
          desc_len = p_data[cnt + 1];
          if(desc_len > 0)
          {
            dum_bouquet_list(p_channel, &p_data[cnt + 2], desc_len);
          }
        }
        break;

      default:
        break;
    }
    cnt += (p_data[cnt + 1] + 2);

  }

}


static void dump_service_desc(channel_service_t *p_svc, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u16 desc_len = 0;
  u8 name_len = 0;
  u8 content_offset = 0;
  u8 bou_id = 0;
  u8 nvod_num = 0;
  while(cnt < len)
  {
    switch(p_data[cnt])
    {
      case DVB_DESC_SERVICE:
        CH_DBG("DVB_DESC_SERVICE \n");
        content_offset = (u8)cnt;
        content_offset += 1; // skip tag
        desc_len = p_data[content_offset];
        content_offset += 1;
        p_svc->service.type = p_data[content_offset];
        content_offset += 1;
        name_len = p_data[content_offset];
        content_offset += 1;
        memcpy(p_svc->service.provider_name, &p_data[content_offset],
                      name_len > MAX_NAME_LEN ? MAX_NAME_LEN : name_len);
        content_offset += name_len;
        name_len = p_data[content_offset];
        content_offset += 1;
        memcpy(p_svc->service.svc_name, &p_data[content_offset],
                      name_len > MAX_NAME_LEN ? MAX_NAME_LEN : name_len);
        CH_DBG("type :%d, %s \t %s \n",
          p_svc->service.type, p_svc->service.provider_name, p_svc->service.svc_name);
        break;
      case DVB_DESC_NVOD_REFERENCE:
        CH_DBG("DVB_DESC_NVOD_REFERENCE \n");
        desc_len = p_data[cnt + 1];
        //dump_nvod_desc(p_svc, &p_data[cnt + 2], desc_len);
        content_offset = 0;
        nvod_num = 0;
        while(content_offset < desc_len)
        {
          p_svc->nvod[nvod_num].ts_id =
            MAKE_WORD(p_data[cnt + content_offset + 3] , p_data[cnt + content_offset + 2]);
          p_svc->nvod[nvod_num].net_id =
            MAKE_WORD(p_data[cnt + content_offset + 5] , p_data[cnt + content_offset + 4]);
          p_svc->nvod[nvod_num].s_id =
            MAKE_WORD(p_data[cnt + content_offset + 7] , p_data[cnt + content_offset + 6]);
          content_offset += 6;
          CH_DBG("ts_id %d net id %d  s_id %d \n",
            p_svc->nvod[nvod_num].ts_id, p_svc->nvod[nvod_num].net_id, p_svc->nvod[nvod_num].s_id);
        }
        break;
       case DVB_DESC_TIME_SHIFTED_SERVICE:
        desc_len = p_data[cnt + 1];
        p_svc->timeshift.s_id = MAKE_WORD(p_data[cnt + 3] , p_data[cnt + 2]);
        p_svc->timeshift.event_id = MAKE_WORD(p_data[cnt + 5] , p_data[cnt + 4]);
        CH_DBG("DVB_DESC_TIME_SHIFTED_SERVICE  sid %d  evt_id\n",
          p_svc->timeshift.s_id, p_svc->timeshift.event_id);
        break;
      case 0x80:
        desc_len = p_data[cnt + 1];
        p_svc->setting.ch_no = MAKE_WORD(p_data[cnt + 3] , p_data[cnt + 2]);
        p_svc->setting.sound_channel = p_data[cnt + 4];
        p_svc->setting.vol_offset = p_data[cnt + 5];
        CH_DBG("	PrivateSetting  ch: %d  sound %d  offset %d\n",
          p_svc->setting.ch_no, p_svc->setting.sound_channel, p_svc->setting.vol_offset);
        break;
      case 0x91:
        CH_DBG("service_content_descriptor \n");
        desc_len = p_data[cnt + 1];
        content_offset = 0;
        while(content_offset < desc_len)
        {
          bou_id = MAKE_WORD(p_data[cnt + content_offset + 3] , p_data[cnt + content_offset + 2]);
          p_svc->s_content.bouquet_id = bou_id;
          content_offset += 2;
          CH_DBG("bou_id %d\n", bou_id);
        }
        break;

      case 0x51:
        OS_PRINTF("mosaic found\n");
        break;
      default:
        break;
    }
    cnt += (p_data[cnt + 1] + 2);
  }
}

static void dump_component_desc(component_desc_t *p_com, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u16 desc_len = 0;

  while(cnt < len)
  {
    switch(p_data[cnt])
    {
      case CA_DESC_ID:

        desc_len = p_data[cnt + 1];
        p_com->cas.ca_sys_id = MAKE_WORD(p_data[cnt + 3], p_data[cnt + 2]);
        p_com->cas.ca_pid = MAKE_WORD(p_data[cnt + 5], p_data[cnt + 4] & 0x1f);
        CH_DBG("CA_DESC_ID 2  sys_id %d  pid %d\n",
          p_com->cas.ca_sys_id, p_com->cas.ca_pid);
        break;
      default:
        break;
    }
    cnt += (p_data[cnt + 1] + 2);
  }

}

static void dump_component_loop(channel_service_t *p_svc, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u16 desc_len = 0;
  u8 num = 0;
  while(cnt < len)
  {
    CH_DBG("tag %x, type %x pid %x \n",
      p_data[cnt], p_data[cnt + 1], MAKE_WORD(p_data[cnt + 3] , p_data[cnt + 2]));
    switch(p_data[cnt ])// tag
    {
      case 0: //video
        p_svc->component[num].pid = MAKE_WORD(p_data[cnt + 3] , p_data[cnt + 2]);
        p_svc->component[num].type = p_data[cnt + 1];
        break;
      case 1: // audio
        p_svc->component[num].pid = MAKE_WORD(p_data[cnt + 3] , p_data[cnt + 2]);
        p_svc->component[num].type = p_data[cnt + 1];
        break;
      default:
        break;
    }
    cnt += 4;
    desc_len = MAKE_WORD(p_data[cnt + 1] , p_data[cnt ] & 0xf);
    cnt += 2;
    if(desc_len > 0)
    {
      // component desc
      dump_component_desc(&p_svc->component[num], &p_data[cnt], desc_len);
      cnt += desc_len;
    }
    num ++;
  }
  p_svc->total_component_num = num;
}

static void dump_service_loop(channel_node_t *p_node, u8 *p_data, u16 len)
{
  u16 cnt = 0;
  u8 loop = 0;
  u16 desc_len = 0;

  while(cnt < len)
  {
    p_node->svc[loop].s_id = MAKE_WORD(p_data[cnt + 1], p_data[cnt]);
    cnt += 2;
    p_node->svc[loop].pcr_pid = MAKE_WORD(p_data[cnt + 1], p_data[cnt]);
    cnt += 2;
    desc_len = MAKE_WORD(p_data[cnt + 1] , p_data[cnt] & 0xf);
    cnt += 2;
    if(desc_len > 0)
    {
      // dump service desc
      dump_service_desc(&p_node->svc[loop], &p_data[cnt], desc_len);
      cnt += desc_len;
    }
    desc_len = MAKE_WORD(p_data[cnt + 1] , p_data[cnt] & 0xf);
    cnt += 2;
    if(desc_len > 0)
    {
      // componet_loop
      dump_component_loop(&p_node->svc[loop], &p_data[cnt], desc_len);
      cnt += desc_len;
    }
    loop ++;
  }
  p_node->total_svc_num = loop;
}

void parse_channel_sec(handle_t handle, dvb_section_t *p_sec)
{
  u8          *p_buf   = p_sec->p_buffer;
  u8          *p_data  = NULL;

  service_t *p_svc = handle;
  os_msg_t    msg      = {0};
  u16 des_len = 0;
  s32 data_len = 0;
  u8 node_cnt = p_channel->total_node_num;
  u16 offset = 0;




  p_channel->table_id = p_buf[0];
  p_channel->section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0x0f);
  p_channel->version = (p_buf[5] & 0x3e) >> 1;
  p_channel->section_num = p_buf[6];
  p_channel->last_section = p_buf[7];

  CH_DBG("parsing channel list section  last sec %d\n", p_channel->last_section);

  p_data = p_buf + 10;
  data_len = p_channel->section_len - 10;

  offset = 0;
  while(offset < data_len)
  {
    p_channel->node[node_cnt].ts_id = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    p_channel->node[node_cnt].net_id = MAKE_WORD(p_data[offset + 1], p_data[offset]);
    offset += 2;
    des_len = MAKE_WORD(p_data[offset + 1] , p_data[offset] & 0xf);
    offset += 2;

    if(des_len > 0)
    {
      // dump transport desc
      dump_transport_desc(&(p_channel->node[node_cnt]), &p_data[offset], des_len);
      offset += des_len;
    }

    des_len = MAKE_WORD(p_data[offset + 1], p_data[offset] & 0xf);
    offset += 2;
    if(des_len > 0)
    {
      // service loop
      dump_service_loop(&(p_channel->node[node_cnt]), &p_data[offset], des_len);
      offset += des_len;
    }
   // offset += 4; //crc
    node_cnt ++;
   if(data_len - offset < 6)
   {
     break;
   }
//    data_len -= offset;
  }
  p_channel->total_node_num = node_cnt;

//  dump_info(&channel);

  msg.content = DVB_DTM_FOUND;
  msg.para1   = (u32)p_channel;
  msg.para2   = sizeof(channel_update_section_t);
  msg.context = p_sec->r_context;

  p_svc->notify(p_svc, &msg);

}




extern RET_CODE dvb_to_unicode(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen);
/*
static void save_prog(channel_update_section_t *p_ch)
{
  u16 i = 0, j = 0, k = 0;
  channel_node_t *p_node = NULL;
  channel_service_t *p_svc = NULL;
  dvbs_tp_node_t tp = {0};
  dvbs_prog_node_t pg = {0};
  u32 tp_id = 0;
  component_desc_t *p_com = NULL;
  u8 aud_cnt = 0;

  g_priv.vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  g_priv.vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);

  for(i = 0; i < p_ch->total_node_num; i ++)
  {
    p_node = &p_ch->node[i];
    if(is_new_tp(p_node->cable.freq, p_node->cable.sym, &tp_id))
    {
      //add tp
      tp.freq = p_node->cable.freq;
      tp.sym = p_node->cable.sym;
    //  tp.is_scramble = p_node->.is_scrable;
      switch(p_node->cable.modulation)
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
        default:
          tp.nim_modulate = NIM_MODULA_QAM64;
          break;
      }
      db_dvbs_add_tp(g_priv.vv_id_tp,&tp);

      tp_id = tp.id;
    }

    for(j = 0; j < p_node->total_svc_num; j ++)
    {
      // add pg
      p_svc = &p_node->svc[j];
      pg.tp_id = tp_id;
      pg.ts_id = p_node->ts_id;
      pg.orig_net_id = p_node->net_id;
      pg.s_id = p_svc->s_id;
//      pg.logical_num = p_svc->setting.ch_no;

      if(p_svc->service.type == 0x6
        || p_svc->service.type == 0xB)
      {
        pg.mosaic_flag = TRUE;
      }

      pg.service_type = p_svc->service.type;
      if(p_node->cas.ca_sys_id != 0)
        pg.is_scrambled = 1;
   //   pg.bouquet_id = p_svc->s_content.bouquet_id;
      pg.volume = 10; //DEFAULT
      pg.pmt_pid = 0x1fff;
	  aud_cnt = 0;
      for(k = 0; k < p_svc->total_component_num; k ++)
      {
        p_com = &p_svc->component[k];

        if(p_com->type == 2)
        {//video
          pg.video_pid = p_com->pid;
        }
        else if(p_com->type == 4)
        {
          pg.audio[aud_cnt].p_id = p_com->pid;
          //pg.audio[aud_cnt].type = 0;
          pg.audio[aud_cnt].type = AUDIO_MP2;
          aud_cnt ++;
        }
        else
        {

        }
      }
      pg.pcr_pid = p_svc->pcr_pid;
      pg.audio_ch_num = aud_cnt;
      pg.ca_system_id = p_node->cas.ca_sys_id;
      pg.cas_ecm[0].ca_sys_id = p_node->cas.ca_sys_id;
      pg.cas_ecm[0].ecm_pid = p_node->cas.ca_pid;
      dvb_to_unicode((u8*)p_svc->service.svc_name,
        sizeof(p_svc->service.svc_name), (u16*)pg.name, DB_DVBS_MAX_NAME_LENGTH);

      db_dvbs_add_program(g_priv.vv_id_pro, &pg);
    }
  }

  db_dvbs_save(g_priv.vv_id_tp);
  db_dvbs_save(g_priv.vv_id_pro);

}
*/
extern BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play);
static void nit_request_start(void)
{
  dvb_request_t dvb_req = {0};
  dvb_req.req_mode  = DATA_SINGLE;
  dvb_req.table_id  = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para1     = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2     = 0xFFFF;

  g_priv.p_dvb_svc->do_cmd(g_priv.p_dvb_svc, DVB_REQUEST,
    (u32)&dvb_req, sizeof(dvb_request_t));

  g_priv.pending_job ++;
}

BOOL channel_init_loop(os_msg_t *p_msg)
{
 // customer_cfg_t cfg = {0};
//  get_customer_config(&cfg);

  if(p_msg != NULL)
  {
    switch(p_msg->content & (~SYS_EVT_TO_UI_MSG_MASK))
    {
    case NC_EVT_LOCKED:
      g_priv.status = INIT_REQUEST_TABLE; // to request channel table
      CH_DBG("locked in channel init\n");
      break;
    case NC_EVT_UNLOCKED:
      CH_DBG("unlock  in channel init\n");
      g_priv.status = INIT_DONE;
      break;
    case DVB_DTM_FOUND:
      // process nit
      {
        channel_update_section_t *p_channel = NULL;
        p_channel = (channel_update_section_t *)(p_msg->para1);
        CH_DBG("DVB_DTM_FOUND FOUND\n");
        g_priv.table_found_bit = TRUE;
      g_priv.pending_job --;
	//  if(p_channel->version == sys_status_get_channel_version())
     // {
     //   g_priv.status = INIT_DONE;
        //break;
     // }
    //
        if(p_channel->section_num != p_channel->last_section)
        {
          g_priv.status = INIT_REQUEST_TABLE;//request again
          g_priv.cur_sec_num = (u8 )(p_channel->section_num + 1);
          CH_DBG("request next sec %d\n", g_priv.cur_sec_num);
        }
        else
        {
          g_priv.status = INIT_DONE;
        }
      }
      break;
    case DVB_NIT_FOUND:
      {
        nit_t *p_nit = NULL;
        p_nit = (nit_t *)p_msg->para1;
        CH_DBG("DVB_NIT_FOUND FOUND\n");
        g_priv.pending_job --;
        startup_nit_version = p_nit->version_num;
        startup_nit_network_id = p_nit->network_id;
	      CH_DBG("startup_nit_network_id = %x \n ", startup_nit_network_id);
        g_priv.status = INIT_DONE;
        #ifdef LCN_SWITCH_DS_JHC
        {
          nit_logic_num_parse(p_nit->p_origion_data);
          extern void jhc_ds_cas_parse_nit(u8 *p_buf);
          jhc_ds_cas_parse_nit(p_nit->p_origion_data);
        }
        #endif
        break;
      }
      break;
    case DVB_TABLE_TIMED_OUT:
      CH_DBG("table 0x%x timeout\n",p_msg->para1);
      g_priv.pending_job --;
	  /*
      if((cfg.customer == CUSTOMER_TAIHUI) ||
                     (cfg.customer == CUSTOMER_JINGHUICHENG_QZ))
        {
          if(p_msg->para1 != DVB_TABLE_ID_NIT_ACTUAL)
          {
            g_priv.status = INIT_DONE;
          }
        }
      else
        {*/
          g_priv.status = INIT_DONE;
      //  }
      break;
    }
  }
  return FALSE;
}

extern BOOL sys_status_check_group(void);

BOOL channel_sm(void)
{
  //customer_cfg_t cfg = {0};
//  get_customer_config(&cfg);
  switch(g_priv.status)
  {
    case INIT_IDLE: //idle
      break;
    case INIT_REQUEST_TABLE:// request
     nit_request_start();
	 /*
      if((cfg.customer == CUSTOMER_TAIHUI) ||
                 (cfg.customer == CUSTOMER_JINGHUICHENG_QZ))
      {
        dvb_request_t param = {0};
        CH_DBG("request table sec %d, pid %x\n", g_priv.cur_sec_num, g_priv.pid);
        param.req_mode  = DATA_SINGLE;
        param.table_id  = DVB_TABLE_ID_RUN_STATUS; //just for dvb register
        param.para1     = g_priv.pid; // pid
        param.para2     = g_priv.table_id << 16 | g_priv.cur_sec_num; //table_id
        g_priv.p_dvb_svc->do_cmd(g_priv.p_dvb_svc, DVB_REQUEST,(u32)&param, sizeof(dvb_request_t));
        g_priv.pending_job ++;
      }
*/
      g_priv.status = INIT_WAIT_TABLE;
      break;
    case INIT_WAIT_TABLE: //running
      break;
    case INIT_DONE: //done
      //release resource
      if(g_priv.pending_job == 0)
      {
      #if 0
            if((cfg.customer == CUSTOMER_TAIHUI) ||
                     (cfg.customer == CUSTOMER_JINGHUICHENG_QZ))
          {
            dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
            p_dvb->remove_svc_instance(p_dvb, g_priv.p_dvb_svc);
            nc_remove_svc_instance(class_get_handle_by_id(NC_CLASS_ID), g_priv.p_nc_svc);

            if((p_channel->version != sys_status_get_channel_version())&&
              (p_channel->total_node_num > 0))
            {
              sys_status_reset_channel_and_nit_version();/***make sure next start up can auto scan*****/
               ui_dbase_delete_all_pg();
              db_dvbs_restore_to_factory(PRESET_BLOCK_ID);
              //sys_status_load();
              sys_group_reset();
              sys_status_check_group();
              sys_status_save();
              dump_info(p_channel);
              save_prog(p_channel);
    //          ui_dbase_pg_sort(DB_DVBS_ALL_RADIO);
    //          ui_dbase_pg_sort(DB_DVBS_ALL_TV);
    //          reset_last_prog();
              sys_status_set_channel_version(p_channel->version);
    //          ui_dvbc_change_view(DB_DVBS_ALL_RADIO, FALSE);
    //          ui_dvbc_change_view(DB_DVBS_ALL_TV, FALSE);
            }
            CH_DBG("INIT done\n");
            {
              u8 vid = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
              if((g_priv.table_found_bit == TRUE) && (db_dvbs_get_count(vid) > 0))
                {
                  startup_nit_channel_status = TRUE;
                }
            }
          }
          else
          {
          #endif
            CH_DBG("INIT done no channel data\n");
            startup_nit_channel_status = FALSE;
          }
        return TRUE;
      break;
    default:
      break;
  }
  return FALSE;
}

 void channel_init(u32 freq, u32 sym, u16  modulation,u16 pid, u16 table_id)
{
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  nc_channel_info_t search_info = {0};
  os_msg_t  msg = {0};
  u32 ret = 0;

  p_channel = mtos_malloc(sizeof(channel_update_section_t));
  MT_ASSERT(NULL != p_channel);
  memset(p_channel, 0x00, sizeof(channel_update_section_t));

  ap_frm_run();

  g_priv.p_nc_svc = nc_get_svc_instance(nc_handle, APP_FRAMEWORK);
  g_priv.p_dvb_svc = p_dvb->get_svc_instance(p_dvb, APP_FRAMEWORK);
  g_priv.pid = pid;
  g_priv.cur_sec_num = 0;
  g_priv.table_id = table_id;


  CH_DBG("lock tp %d  %d\n", freq, sym);

  search_info.polarization = 0;
  search_info.channel_info.frequency = freq;
  search_info.channel_info.param.dvbc.symbol_rate = sym;
  search_info.channel_info.param.dvbc.modulation = modulation; //NIM_MODULA_QAM64

  g_priv.msg_id = mtos_messageq_create(40, (u8 *)"chan init");

  ap_frm_set_ui_msgq_id(g_priv.msg_id);
  nc_set_tp(nc_handle, 0, g_priv.p_nc_svc, &search_info);

  while(1)
  {
    ret = mtos_messageq_receive(g_priv.msg_id, &msg, 10);
    if(ret)
    {
      channel_init_loop(&msg);
    }

    if(channel_sm())
    {
      break;
    }
  }
  mtos_free(p_channel);
  p_channel = NULL;
}


