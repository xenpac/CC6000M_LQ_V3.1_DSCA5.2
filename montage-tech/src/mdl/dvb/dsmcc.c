/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include  "sys_types.h"
#include  "sys_define.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"

#include "dvb_protocol.h"
#include "dsmcc.h"
#include "err_check_def.h"


#ifdef PRINT_ON
#define MDL_DSMCC_DEBUG
#endif

#ifdef MDL_DSMCC_DEBUG
#define MDL_DBG_DSMCC(x) (OS_PRINTF x)
#else
#define MDL_DBG_DSMCC(x) do {} while(0)
#endif

/*!
   DSMCC BIOP TAG
  */
#define DSMCC_TAG_BIOP      0x49534F06
/*!
   DSMCC connection binder TAG
  */
#define DSMCC_TAG_CONN_BINDER   0x49534F40
/*!
   DSMCC lite options TAG
  */
#define DSMCC_TAG_LITE_OPTIONS    0x49534F05
/*!
   DSMCC object location TAG
  */
#define DSMCC_TAG_OBJECT_LOCATION   0x49534F50

/*!
   DSMCC DII GROUP ID
  */
#define DSMCC_DII_GROUP_ID      0x0080

dsmcc_module_info_t mods_info[DSMCC_MAX_MOD_NUM];

static void parse_dsmcc_bpb(u8 *p_bpb, u32 len)
{
  u8 comp_data_len = p_bpb[6];
  u8 object_key_len = p_bpb[15];
  u32 comp_id_tag = 0;
  u32 carousel_id = 0;
  u32 transaction_id = 0;
  u16 module_id = 0;
  u8 object_key[4];
  u8 taps_count = 0;

  comp_id_tag = make32(&p_bpb[2]);

  CHECK_FAIL_RET_VOID(comp_id_tag == DSMCC_TAG_OBJECT_LOCATION);

  carousel_id = make32(&p_bpb[7]);
  module_id = MAKE_WORD2(p_bpb[11], p_bpb[12]);

  CHECK_FAIL_RET_VOID(comp_data_len == (9 + object_key_len));
  memcpy(object_key, &p_bpb[16], object_key_len);

  comp_id_tag = make32(&p_bpb[16 + object_key_len]);
  CHECK_FAIL_RET_VOID(comp_id_tag == DSMCC_TAG_CONN_BINDER);

  comp_data_len = p_bpb[20 + object_key_len];
  taps_count = p_bpb[21 + object_key_len];
  transaction_id = make32(&p_bpb[31 + object_key_len]);
  MDL_DBG_DSMCC(("transaction id is 0x%x\n", transaction_id));
}


static void parse_dsmcc_gi(service_t *p_svc, u8 *p_gi, u16 len)
{
  //u16 i = 0;
  u16 j = 0;
  u16 offset = 0;
  u16 group_num = 6;
  u16 compa_desc_len = 0;
  u16 descript_cnt = 0;
  u16 descript_type = 0;
  u32 group_id = 0;
  //u32 group_size = 0;
  dsmcc_dsi_gi_t group_info;

  group_num = MAKE_WORD2(p_gi[0], p_gi[1]);
  offset = 2;
  /* currently only one group is handled */
  //for(i = 0; i < group_num; i++)
  {
    group_id = make32(&p_gi[offset]);
    group_info.file_total_size = make32(&p_gi[offset + 4]);
    compa_desc_len = MAKE_WORD2(p_gi[offset + 8], p_gi[offset + 9]);
    descript_cnt = MAKE_WORD2(p_gi[offset + 10], p_gi[offset + 11]);
    offset += 12;
    for(j = 0; j < descript_cnt; j++)
    {
      descript_type = p_gi[offset];
      if(SYS_HARDWARE_DESC == descript_type)
      {
        CHECK_FAIL_RET_VOID(p_gi[offset + 1] == 9);
        CHECK_FAIL_RET_VOID(p_gi[offset + 2] == 1);
        group_info.oui = p_gi[offset + 3] << 16 | p_gi[offset + 4] << 8
                         | p_gi[offset + 5];
        group_info.hw_mod_id = MAKE_WORD2(p_gi[offset + 6], p_gi[offset + 7]);
        group_info.hw_version = MAKE_WORD2(p_gi[offset + 8], p_gi[offset + 9]);
      }
      else if(SYS_SOFTWARE_DESC == descript_type)
      {
        CHECK_FAIL_RET_VOID(p_gi[offset + 1] == 9);
        CHECK_FAIL_RET_VOID(p_gi[offset + 2] == 1);
        group_info.oui = p_gi[offset + 3] << 16 | p_gi[offset + 4] << 8
                         | p_gi[offset + 5];
        group_info.sw_mod_id = MAKE_WORD2(p_gi[offset + 6], p_gi[offset + 7]);
        group_info.sw_version = MAKE_WORD2(p_gi[offset + 8], p_gi[offset + 9]);
      }
      else if(SYS_SOFTWARE_FORMAT_DESC == descript_type)
      {
        CHECK_FAIL_RET_VOID(p_gi[offset + 1] == 9);
        CHECK_FAIL_RET_VOID(p_gi[offset + 2] == 1);
        group_info.sw_block_id = p_gi[offset + 3];
        group_info.sw_is_zipped = p_gi[offset + 4];
        group_info.sw_is_spilt = p_gi[offset + 5];
      }
      CHECK_FAIL_RET_VOID(p_gi[offset + 10] == 0);
      offset += 11;
    }
  }

  {
    os_msg_t msg = {0};
    msg.is_ext = 0;
    msg.content = DVB_DSMCC_DSI_FOUND;
    msg.para1 = (u32) & group_info;
    msg.para2 = sizeof(dsmcc_dsi_gi_t);
    p_svc->notify(p_svc, &msg);
  }
}


void parse_dsmcc_ior(u8 *p_ior, u16 len)
{
  u32 type_id_len = 0;
  u32 tagged_profile_count = 0;
  u16 i = 0;
  u8 type_id[4];

  type_id_len = make32(&p_ior[0]);

  memcpy(type_id, &p_ior[4], type_id_len);
  MDL_DBG_DSMCC(("ior type id %s\n", type_id));

  tagged_profile_count = make32(&p_ior[8]);

  for(i = 0; i < tagged_profile_count; i++)
  {
    u32 profile_tag = 0;
    u32 profile_data_len = 0;

    profile_tag = make32(&p_ior[12]);

    profile_data_len = make32(&p_ior[16]);

    switch(profile_tag)
    {
      // BIOP profile body
      case DSMCC_TAG_BIOP:
        parse_dsmcc_bpb(&p_ior[20], profile_data_len);
        break;
      // Lite options profile body
      case DSMCC_TAG_LITE_OPTIONS:
        break;
      default:
        break;
    }
  }
}


static BOOL dsmcc_add_mod_info(dsmcc_module_info_t *p_mod_info)
{
  u32 i = 0;

  for(i = 0; i < DSMCC_MAX_MOD_NUM; i++)
  {
    if(mods_info[i].mod_size == 0)
    {
      break;
    }
    if(p_mod_info->mod_id == mods_info[i].mod_id)
    {
      MDL_DBG_DSMCC(("module info already exists\n"));
      return FALSE;
    }
  }
  MDL_DBG_DSMCC(("add module info, mod id: 0x%x\n", p_mod_info->mod_id));
  memcpy(&mods_info[i], p_mod_info, sizeof(dsmcc_module_info_t));

  return TRUE;
}


static void parse_dsmcc_dii(service_t *p_svc, u8 *p_dii, u16 len)
{
  u32 download_id = 0;
  u16 block_size = 0;
  u16 num_module = 0;
  u16 i = 0;
  u16 module_cul_len = 0;
  u8 *p_dii_mod = NULL;
  dsmcc_module_info_t temp_mod;
  static u8 *p_t_buf = NULL;

  if(p_t_buf == NULL)
  {
    p_t_buf = mtos_malloc(sizeof(dsmcc_module_info_t) * DSMCC_MAX_MOD_NUM);
    CHECK_FAIL_RET_VOID(p_t_buf != NULL);
  }
  download_id = make32(&p_dii[0]);

  block_size = MAKE_WORD2(p_dii[4], p_dii[5]);
  num_module = MAKE_WORD2(p_dii[18], p_dii[19]);

  module_cul_len = 0;
  p_dii_mod = &p_dii[20];

  // init the mods info before parse the dii module info
  memset(mods_info, 0, DSMCC_MAX_MOD_NUM * sizeof(dsmcc_module_info_t));

  for(i = 0; i < num_module; i++)
  {
    temp_mod.mod_id = MAKE_WORD2(p_dii_mod[0], p_dii_mod[1]);
    temp_mod.mod_size = make32(&p_dii_mod[2]);
    temp_mod.mod_ver = p_dii_mod[6];
    dsmcc_add_mod_info(&temp_mod);
    p_dii_mod += (8 + p_dii_mod[7]);
  }

  {
    os_msg_t msg = {0};
    memcpy(p_t_buf, mods_info,
           sizeof(dsmcc_module_info_t) * DSMCC_MAX_MOD_NUM);
    msg.is_ext = 0;
    msg.content = DVB_DSMCC_DII_FOUND;
    msg.para1 = (u32)p_t_buf;
    msg.para2 = sizeof(dsmcc_module_info_t) * DSMCC_MAX_MOD_NUM;
    p_svc->notify(p_svc, &msg);
  }
}


static void parse_dsmcc_dsi(service_t *p_svc, u8 *p_dsi, u16 len, u32 context)
{
  u16 priv_data_len = 0;

  CHECK_FAIL_RET_VOID(p_dsi[20] == 0);
  CHECK_FAIL_RET_VOID(p_dsi[21] == 0);

  priv_data_len = MAKE_WORD2(p_dsi[22], p_dsi[23]);
  CHECK_FAIL_RET_VOID(priv_data_len + 24 == len);
  if(priv_data_len > 0)
  {
    /* for dsmcc sections, the private data is IOR */
    //parse_dsmcc_ior(&p_dsi[24], priv_data_len);
    //check our dsi info
    if ((MAKE_WORD2(p_dsi[24], p_dsi[25]))  != 0)
    {
      os_msg_t msg = {0};
      msg.content = DVB_TABLE_TIMED_OUT;
      msg.is_ext = FALSE;
      msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
      msg.context = context;
      p_svc->notify(p_svc, &msg);
      MDL_DBG_DSMCC(("Not our DSMCC msg\n"));
      return;
    }
    /* for dvbs ota sections, the private data is group info */
    parse_dsmcc_gi(p_svc, &p_dsi[24], priv_data_len);
  }

  /* it's only a notification to OTA AP to start filter DII */
  /*{
     os_msg_t msg = {0};
     msg.is_ext = 0;
     msg.content = DVB_ENGINE_DSMCC_DSI_FOUND;
     msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
     msg.para2 = 0;
     mdl_broadcast_msg(&msg);
     }*/
}


void parse_dsmcc_msg(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  u8 *p_buf = p_sec->p_buffer;
  u16 msg_type = 0;
  u16 msg_len = 0;
  u32 transaction_id = 0;

  CHECK_FAIL_RET_VOID(NULL != p_buf);
  if(p_buf[8] != 0x11)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not DSMCC msg\n"));
    return;
  }
  if(p_buf[9] != 0x03)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not U-N download msg\n"));
    return;
  }
  transaction_id = make32(&p_buf[12]);

  msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);
  msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);
  switch(msg_type)
  {
    case DSMCC_MSG_DSI:
      parse_dsmcc_dsi(p_svc, &p_buf[20], msg_len, p_sec->r_context);
      break;
    case DSMCC_MSG_DII:
      parse_dsmcc_dii(p_svc, &p_buf[20], msg_len);
      break;
    default:
      MDL_DBG_DSMCC(("dsm_msg: should not come here\n"));
      break;
  }
}

static void parse_dsmcc_biop(service_t *p_svc, u8 *p_biop, u16 len,
  dsmcc_ddb_t *p_dsmcc)
{
  u32 magic = 0;
  u32 object_data = 0;
  u32 content_size = 0;
  u32 content_len = 0;
  u32 msg_size = 0;
  u16 object_len = 0;
  u16 i = 0;
  u8 object_key_len = 0;
  u8 context_list_count = 0;
  u8 object_key[4];

  if(p_dsmcc->ddb_hdr.block_num == 0)
  {
    magic = make32(&p_biop[0]);
    CHECK_FAIL_RET_VOID(magic == 0x42494F50);

    msg_size = make32(&p_biop[8]);

    // in this case object key len is always 4
    object_key_len = p_biop[12];
    memcpy(object_key, &p_biop[13], object_key_len);

    object_data = make32(&p_biop[21]);
    if(object_data != 0x66696C00)
    {
      /* currently we only parse file message */
      /* though not dir type exactly, use it for temp */
      p_dsmcc->content_type = DSMCC_CONTENT_DIR;
      MDL_DBG_DSMCC(("object data not file\n"));
    }
    else
    {
      u8 *p_contextList = NULL;
      u32 context_len = 0;

      object_len = MAKE_WORD2(p_biop[25], p_biop[26]);
      content_size = make32(&p_biop[31]);
      MDL_DBG_DSMCC(("file size is %d\n", content_size));

      context_list_count = p_biop[35 + object_len - 8];
      p_contextList = &p_biop[35 + object_len - 8 + 1];
      for(i = 0; i < context_list_count; i++)
      {
        context_len += 6 + MAKE_WORD2(p_contextList[context_len + 4],
                                      p_contextList[context_len + 5]);
      }
      content_len = make32(&p_biop[35 + object_len - 8 + 1 + context_len + 4]);

      p_dsmcc->content_len = len - (36 + object_len + context_len);
      p_dsmcc->content_type = DSMCC_CONTENT_FILE;
      memcpy(p_dsmcc->content,
             &p_biop[35 + object_len - 8 + 1 + context_len + 8],
             p_dsmcc->content_len);
    }
  }
  else
  {
    p_dsmcc->content_len = len;
    p_dsmcc->content_type = DSMCC_CONTENT_FILE;
    memcpy(p_dsmcc->content, &p_biop[0], len);
  }

  {
    os_msg_t msg = {0};
    msg.is_ext = 0;
    msg.content = DVB_DSMCC_DDM_FOUND;
    msg.para1 = (u32)p_dsmcc;
    msg.para2 = sizeof(dsmcc_ddb_t);
    p_svc->notify(p_svc, &msg);
  }
}


static void parse_dsmcc_ddb(service_t *p_svc, u8 *p_ddb, u16 len,
  dsmcc_ddb_t *p_dsmcc)
{
  p_dsmcc->ddb_hdr.mod_id = MAKE_WORD2(p_ddb[0], p_ddb[1]);
  p_dsmcc->ddb_hdr.mod_ver = p_ddb[2];
  p_dsmcc->ddb_hdr.block_num = MAKE_WORD2(p_ddb[4], p_ddb[5]);

  parse_dsmcc_biop(p_svc, &p_ddb[6], (u16)(len - 6), p_dsmcc);
}



void parse_dsmcc_ddm(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  static dsmcc_ddb_t *p_dsmcc = NULL;
  u8 *p_buf = p_sec->p_buffer;
  u16 msg_type = 0;
  u16 msg_len = 0;
  u32 transaction_id = 0;

  CHECK_FAIL_RET_VOID(NULL != p_buf);

  if(p_dsmcc == NULL)
  {
    p_dsmcc = mtos_malloc(sizeof(dsmcc_ddb_t));
    CHECK_FAIL_RET_VOID(p_dsmcc != NULL);
  }

  if(p_buf[8] != 0x11)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_DDM;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not DSMCC msg\n"));
    return;
  }
  if(p_buf[9] != 0x03)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_DDM;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not U-N download msg\n"));
    return;
  }

  p_dsmcc->sec_num = p_buf[6];
  p_dsmcc->last_sec_num = p_buf[7];

  transaction_id = make32(&p_buf[12]);

  msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);
  msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);

  switch(msg_type)
  {
    case DSMCC_MSG_DDB:
      parse_dsmcc_ddb(p_svc, &p_buf[20], msg_len, p_dsmcc);
      break;
    default:
      MDL_DBG_DSMCC(("dsmcc_ddm: should not come here\n"));
      break;
  }
}

void request_dsmcc_ddm_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  ddm_param_t *p_ddm_param = (ddm_param_t *)para1;
  u16 dsmcc_pid = p_ddm_param->es_id;
  u16 sec_number = p_ddm_param->section_num;
  u16 mod_id = p_ddm_param->mod_id;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  MDL_DBG_DSMCC(("MDL: requesting a dsmcc ddb section\n"));

  p_sec->pid = dsmcc_pid;
  p_sec->timeout = (u32)DSMCC_DDB_TIMEOUT;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_DDM;
  p_sec->filter_mode = FILTER_TYPE_SECTION;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
         sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_DDM;
  p_sec->filter_mask[0] = 0xFF;

  /* msg type should be DDB */
#ifndef WIN32
  p_sec->filter_code[1] = (u8)(mod_id >> 8);
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = (u8)(mod_id & 0xFF);
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[4] = (u8)sec_number;
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[8] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[9] = 0xFF;
  p_sec->filter_mask_size = 10;
#else
  p_sec->filter_code[3] = (u8)(mod_id >> 8);
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = (u8)(mod_id & 0xFF);
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[6] = (u8)sec_number;
  p_sec->filter_mask[6] = 0xFF;
  p_sec->filter_code[10] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[11] = 0xFF;
  p_sec->filter_mask_size = 12;
#endif
  p_sec->direct_data = TRUE;
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void request_dsmcc_msg_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  u16 dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  /* msg type could be DSI or DII */
  u16 dsmcc_msg_type = (u16)(para1 & 0xFFFF);
  u16 group_id = 0;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  MDL_DBG_DSMCC(("MDL: requesting a dsmcc ddb section\n"));
  switch(dsmcc_msg_type)
  {
    case DSMCC_MSG_DII:
      group_id = DSMCC_DII_GROUP_ID;
      break;
    default:
      group_id = 0x00;
      break;

  }
  p_sec->pid = dsmcc_pid;
  p_sec->timeout = (u32)DSMCC_MSG_TIMEOUT;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = TRUE;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
         sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = (u8)(group_id >> 8);
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[8] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[9] = 0xFF;
  p_sec->filter_mask_size = 10;
#else
  p_sec->filter_code[3] = (u8)(group_id >> 8);
  p_sec->filter_mask[3] = 0xFF;
    p_sec->filter_code[4] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[10] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[11] = 0xFF;
  p_sec->filter_mask_size = 12;
#endif

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}


void request_dsmcc_ddm_multi_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  u16 data_pid = para1 & 0x1FFF;
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);

  p_sec->direct_data = TRUE;

  //Set PID information
  p_sec->pid = data_pid;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_DDM;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_DDM;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = 0x00;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_mask_size = 2;
#else
  p_sec->filter_code[3] = 0x00;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_mask_size = 4;
#endif
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->timeout = (u32)DSMCC_DDB_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = 1;

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

void request_dsmcc_msg_multi_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  u16 dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  u16 dsmcc_msg_type = (u16)(para1 & 0xFFFF);  
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  
  MDL_DBG_DSMCC(("MDL: requesting a dsmcc ddb section\n"));

  p_sec->pid = dsmcc_pid;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_MSG;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[8] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[9] = 0xFF;
  p_sec->filter_mask_size = 10;
#else
  p_sec->filter_code[10] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[11] = 0xFF;
  p_sec->filter_mask_size = 12;
#endif

  p_sec->filter_mode = FILTER_TYPE_SECTION;
  //p_sec->filter_mask_size = 1;  
  p_sec->timeout = (u32)DSMCC_MSG_TIMEOUT;  

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = 1;
  
  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}
//allen
static void parse_dsi_group_info(service_t *p_svc, u8 *p_gi)
{
  u16 i = 0;
  u16 j = 0;
  u16 compa_desc_len = 0;
  u16 descript_cnt = 0;
  u16 len  = 0;
  u16 descript_type = 0;
  u16 step = 0;
  u8 *p_tem = NULL;
  dsmcc_dsi_info_t dsi_info;
  
  //dsi_info.group_num = MAKE_WORD2(p_gi[0], p_gi[1]);
  dsi_info.is_force = p_gi[0];
  dsi_info.group_num = p_gi[1];  
   
  step = 2;

  for(i = 0; i < dsi_info.group_num; i++)
  {
    dsi_info.group_info[i].group_id = make32(&p_gi[step]);

    dsi_info.group_info[i].file_total_size = make32(&p_gi[step + 4]);

    compa_desc_len = MAKE_WORD2(p_gi[step + 8], p_gi[step + 9]);

    descript_cnt = MAKE_WORD2(p_gi[step + 10], p_gi[step + 11]);

    step += 12;
    len = 0;
    for(j = 0; j < descript_cnt; j++)
    {
       //descrip_len = step;
       p_tem = &p_gi[step];
       descript_type = p_tem[len];
        if(SYS_HARDWARE_DESC == descript_type)
        {
            CHECK_FAIL_RET_VOID(p_tem[len + 1] == 0x9);
            CHECK_FAIL_RET_VOID(p_tem[len + 2] == 1);
            
            dsi_info.group_info[i].oui =
              p_tem[len + 3] << 16 | p_tem[len + 4] << 8 | p_tem[len + 5];
            dsi_info.group_info[i].hw_mod_id = MAKE_WORD2(p_tem[len + 6], p_tem[len + 7]);
            dsi_info.group_info[i].hw_version = MAKE_WORD2(p_tem[len + 8], p_tem[len + 9]);
            //len  += 11;

        }
        else if(SYS_SOFTWARE_DESC == descript_type)
        {
            CHECK_FAIL_RET_VOID(p_tem[len + 1] == 0x9);
            CHECK_FAIL_RET_VOID(p_tem[len + 2] == 1);
            dsi_info.group_info[i].oui =
              p_tem[len + 3] << 16 | p_tem[len + 4] << 8 | p_tem[len + 5];
            dsi_info.group_info[i].sw_mod_id = MAKE_WORD2(p_tem[len + 6], p_tem[len + 7]);
            dsi_info.group_info[i].sw_version = MAKE_WORD2(p_tem[len + 8], p_tem[len + 9]);
            //len  += 11;
        }
        else if(SYS_SOFTWARE_FORMAT_DESC == descript_type)
        {
            CHECK_FAIL_RET_VOID(p_tem[len + 1] == 0x9);
            CHECK_FAIL_RET_VOID(p_tem[len + 2] == 1);
            dsi_info.group_info[i].sw_block_id = p_tem[len + 3];
            dsi_info.group_info[i].sw_is_zipped = p_tem[len + 4];
            dsi_info.group_info[i].sw_is_spilt = p_tem[len + 5];
            //len  += 6;
        }
        len  += 11;
        CHECK_FAIL_RET_VOID(p_tem[len - 1] == 0);
    }
    step += len + 4;//4bytes gap between groups
  }

  {
      os_msg_t msg = {0};
      msg.is_ext = 0;
      msg.content = DVB_DSMCC_DSI_FOUND;
      msg.para1 = (u32)&dsi_info;
      msg.para2 = sizeof(dsmcc_dsi_info_t);
      p_svc->notify(p_svc, &msg);
  }
}

static void parse_dsmcc_dsi_info(service_t *p_svc, u8 *p_dsi, u16 len)
{
  u16 priv_data_len = 0;

  CHECK_FAIL_RET_VOID(p_dsi[20] == 0);
  CHECK_FAIL_RET_VOID(p_dsi[21] == 0);

  priv_data_len = MAKE_WORD2(p_dsi[22], p_dsi[23]);
  CHECK_FAIL_RET_VOID(priv_data_len + 24 == len);
  if(priv_data_len > 0)
  {
    parse_dsi_group_info(p_svc,&p_dsi[24]);
  }

}
static void parse_dsmcc_dii_info(service_t *p_svc, u8 *p_dii, u16 len)
{
  u32 download_id = 0;
  u16 block_size = 0;
  u16 num_module = 0;
  u16 i = 0;
  u16 module_cul_len = 0;
  u8 *p_dii_mod = NULL;
  dsmcc_dii_info_t dii_info = {0};
  u8 module_info_len = 0;
  u16 next_module_id = 0;
  u8 position = 0;
  u16 offset = 0;
  u16 step = 0;
  u8 gap = 0;
  u8 *p_tem = NULL;
  u32 num  = 0;

  download_id = make32(&p_dii[0]);

  block_size = MAKE_WORD2(p_dii[4], p_dii[5]);
  num_module = MAKE_WORD2(p_dii[18], p_dii[19]);

  module_cul_len = 0;
  p_dii_mod = &p_dii[20];

  for(i = 0; i < num_module; i++)
  { 
    num = dii_info.dii_type_info[dii_info.type_num].module_num;
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_id = make16(&p_dii[20 + step]);
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_size = make32(&p_dii[22 + step]);
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_ver =  p_dii[26 + step];
    dii_info.dii_type_info[dii_info.type_num].module_num++;
    module_info_len = p_dii[27 + step];
    offset = 0;
    p_tem = &p_dii[28 + step];
  
    while(module_info_len)
    {
      gap = 0;
      
      if(p_tem[offset] == MODULE_LINK_TAG)
      {
        offset++;
        gap = p_tem[offset++] + 2;
        position = p_tem[offset++];
        next_module_id = make16(&p_tem[offset]);
        offset += 2;
      }
      else if (p_tem[offset] == MODULE_INFO_TAG)
      {
        offset++;
        gap = p_tem[offset++] + 2;
        dii_info.dii_type_info[dii_info.type_num].mod_type = p_tem[offset++];
        dii_info.dii_type_info[dii_info.type_num].is_data_valid = p_tem[offset++];
        dii_info.dii_type_info[dii_info.type_num].start_addr = make32(&p_tem[offset]);
        offset += 4;
      }
      module_info_len -= gap;
    }

    if((position == 0x02) ||(next_module_id == 0))
    {
      dii_info.type_num++;
    }
    
    step += offset + 8;

  }


  {
    os_msg_t msg = {0};
    msg.is_ext = 0;
    msg.content = DVB_DSMCC_DII_FOUND;
    msg.para1 = (u32)&dii_info;
    msg.para2 = sizeof(dsmcc_dii_info_t);
    p_svc->notify(p_svc, &msg);
  }
}


void parse_dsmcc_dsi_dii_section(handle_t handle, dvb_section_t *p_sec)
{
    service_t *p_svc = handle;
    u8 *p_buf = p_sec->p_buffer;
    u16 msg_type = 0;
    u16 msg_len = 0;
    u32 transaction_id = 0;
    
    CHECK_FAIL_RET_VOID(NULL != p_buf);
    if(p_buf[8] != 0x11)
    {
        os_msg_t msg = {0};
        msg.content = DVB_TABLE_TIMED_OUT;
        msg.is_ext = FALSE;
        msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
        msg.context = p_sec->r_context;
        p_svc->notify(p_svc, &msg);
        MDL_DBG_DSMCC(("Not DSMCC msg\n"));
        return;
    }
    if(p_buf[9] != 0x03)
    {
        os_msg_t msg = {0};
        msg.content = DVB_TABLE_TIMED_OUT;
        msg.is_ext = FALSE;
        msg.para1 = DVB_TABLE_ID_DSMCC_MSG;
        msg.context = p_sec->r_context;
        p_svc->notify(p_svc, &msg);
        MDL_DBG_DSMCC(("Not U-N download msg\n"));
        return;
    }
    transaction_id = make32(&p_buf[12]);

    msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);
    msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);
    switch(msg_type)
    {
        case DSMCC_MSG_DSI:
          parse_dsmcc_dsi_info(p_svc, &p_buf[20], msg_len);
          break;
        case DSMCC_MSG_DII:
          parse_dsmcc_dii_info(p_svc, &p_buf[20], msg_len);
          break;
        default:
          MDL_DBG_DSMCC(("dsm_msg: should not come here\n"));
          break;
    }
}

static void parse_dsmcc_db_data(service_t *p_svc, u8 *p_ddb, u16 len,
  dsmcc_ddb_t *p_dsmcc)
{
  p_dsmcc->ddb_hdr.mod_id = MAKE_WORD2(p_ddb[0], p_ddb[1]);
  p_dsmcc->ddb_hdr.mod_ver = p_ddb[2];
  p_dsmcc->ddb_hdr.block_num = MAKE_WORD2(p_ddb[4], p_ddb[5]);
  p_dsmcc->content_len = len - 6;
  memcpy(p_dsmcc->content,&p_ddb[6], p_dsmcc->content_len);

  {
    os_msg_t msg = {0};
    msg.is_ext = 0;
    msg.content = DVB_DSMCC_DDM_FOUND;
    msg.para1 = (u32)p_dsmcc;
    msg.para2 = sizeof(dsmcc_ddb_t);
    p_svc->notify(p_svc, &msg);
  }

}

void parse_dsmcc_db_section(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  static dsmcc_ddb_t *p_dsmcc = NULL;
  u8 *p_buf = p_sec->p_buffer;
  u16 msg_type = 0;
  u16 msg_len = 0;
  u32 transaction_id = 0;

  CHECK_FAIL_RET_VOID(NULL != p_buf);

  if(p_dsmcc == NULL)
  {
    p_dsmcc = mtos_malloc(sizeof(dsmcc_ddb_t));
    CHECK_FAIL_RET_VOID(p_dsmcc != NULL);
  }

  if(p_buf[8] != 0x11)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_DDM;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not DSMCC msg\n"));
    return;
  }
  if(p_buf[9] != 0x03)
  {
    os_msg_t msg = {0};
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_DSMCC_DDM;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    MDL_DBG_DSMCC(("Not U-N download msg\n"));
    return;
  }

  p_dsmcc->sec_num = p_buf[6];
  p_dsmcc->last_sec_num = p_buf[7];

  transaction_id = make32(&p_buf[12]);

  msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);
  msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);

  switch(msg_type)
  {
    case DSMCC_MSG_DDB:
      parse_dsmcc_db_data(p_svc, &p_buf[20], msg_len, p_dsmcc);
      break;
    default:
      MDL_DBG_DSMCC(("dsmcc_ddm: should not come here\n"));
      break;
  }
}
void request_dsmcc_dsi_dii_section(dvb_section_t *p_sec,u32 para1, u32 para2)
{
  u16 dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  /* msg type could be DSI or DII */
  u16 dsmcc_msg_type = (u16)(para1 & 0xFFFF);
  u16 group_id = 0;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  MDL_DBG_DSMCC(("MDL: requesting a dsmcc ddb section\n"));
  switch(dsmcc_msg_type)
  {
    case DSMCC_MSG_DII:
      group_id = (u16)(para2 & 0xFFFF);
      break;
    default:
      group_id = 0x00;
      break;

  }
  p_sec->pid = dsmcc_pid;
  p_sec->timeout = (u32)DSMCC_MSG_TIMEOUT;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = TRUE;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
         sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = (u8)(group_id >> 8);
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[8] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[9] = 0xFF;
  p_sec->filter_mask_size = 10;
#else
  p_sec->filter_code[3] = (u8)(group_id >> 8);
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = (u8)(group_id & 0xFF);
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[10] = (u8)(dsmcc_msg_type >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_sec->filter_mask[11] = 0xFF;
  p_sec->filter_mask_size = 12;
#endif

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);

}

void request_dsmcc_db_multi_section(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  //dsmcc_db_req_t *p_ddm_param = (dsmcc_db_req_t *)para1;
  filter_ext_buf_t *p_ext_buf_list = (filter_ext_buf_t *)para2;
  u16 dsmcc_pid = (u16)((para1 >> 16) & 0xFFFF);
  u8 group_id = (u8)((para1 >> 8) & 0xFF);
  u8 type = (u8)(para1 & 0xFF);
  
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  MDL_DBG_DSMCC(("MDL: requesting a dsmcc ddb section\n"));

  p_sec->direct_data = TRUE;

  //Set PID information
  p_sec->pid = dsmcc_pid;
  p_sec->table_id = DVB_TABLE_ID_DSMCC_DDM;

  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);

  //Set filter parameter
  p_sec->filter_code[0] = DVB_TABLE_ID_DSMCC_DDM;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = group_id ;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = type;
  p_sec->filter_mask[2] = 0xFF;

  p_sec->filter_code[8] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[8] = 0xFF;
  p_sec->filter_code[9] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[9] = 0xFF;

  p_sec->filter_mask_size = 8;
#else
  p_sec->filter_code[3] = (u8)group_id ;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = (u8)type;
  p_sec->filter_mask[4] = 0xFF;
 

  p_sec->filter_code[10] = (u8)(DSMCC_MSG_DDB >> 8);
  p_sec->filter_mask[10] = 0xFF;
  p_sec->filter_code[11] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_sec->filter_mask[11] = 0xFF;

  p_sec->filter_mask_size = 10;
#endif
  p_sec->filter_mode = FILTER_TYPE_SECTION;
  p_sec->timeout = (u32)DSMCC_DDB_TIMEOUT;

  //Attach buffer list
  p_sec->p_ext_buf_list = p_ext_buf_list;
  p_sec->crc_enable = TRUE;

  //Allocate section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}
