/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "drv_dev.h"
#include "../../drv/drvbase/drv_svc.h"
#include "dmx.h"
#include "smc_op.h"
#include "hal_misc.h"

#include "cas_ware.h"
#include "cas_adapter.h"

#define ATR_MAX_SIZE    33  /* Maximum size of ATR byte array */
#define ATR_MAX_HISTORICAL  15  /* Maximum number of historical bytes */
#define ATR_MAX_PROTOCOLS 7 /* Maximun number of protocols */
#define ATR_MAX_IB    4 /* Maximum number of interface bytes per protocol */
#define ATR_CONVENTION_DIRECT 0 /* Direct convention */
#define ATR_CONVENTION_INVERSE  1 /* Inverse convention */
#define ATR_PROTOCOL_TYPE_T0  0 /* Protocol type T=0 */
#define ATR_PROTOCOL_TYPE_T1  1 /* Protocol type T=1 */
#define ATR_PROTOCOL_TYPE_T2  2 /* Protocol type T=2 */
#define ATR_PROTOCOL_TYPE_T3  3 /* Protocol type T=3 */
#define ATR_PROTOCOL_TYPE_T14 14  /* Protocol type T=14 */
#define ATR_INTERFACE_BYTE_TA 0 /* Interface byte TAi */
#define ATR_INTERFACE_BYTE_TB 1 /* Interface byte TBi */
#define ATR_INTERFACE_BYTE_TC 2 /* Interface byte TCi */
#define ATR_INTERFACE_BYTE_TD 3 /* Interface byte TDi */
#define ATR_MAX_VENDER_LEN 10

#define FILTER_SINGLE_MODE (2)

/*!
  The error flag for smc atr parse
  */
typedef enum {
  /*!
    there is a card present in the slot,
    and the last operation was successfully completed
    */
  SMC_SUCCESS = 0,
  /*!
    no card is present in the slot
    */
  SMC_ERR_NOT_INSERTED = -1,
  /*!
    a card or something similar is present in the slot,
    but that is invalid or is not recognized
    */
  SMC_ERR_CARD_INVALID = -2,
  /*!
    the smart card driver found an error in the ATR recrived from the card
    */
  SMC_ERR_WRONG_TS_CHAR = -3,
  /*!
    there was a data overrun in communication between the
    smart card driver and the smart card interface hardware
    */
  SMC_ERR_OVERRUN_ERROR = -4,
  /*!
    there was a parity error in the data transferred
    between the smart card driver and the smart card interface hardware
    */
  SMC_ERR_PARITY_ERROR = -5,
  /*!
    there was a framing error in the data transferred
    between the smart card driver and the smart card interface
    */
  SMC_ERR_FRAME_ERROR = -6,
  /*!
    there was no answer from the smart card
    interface hardware (a time-out condition)
    */
  SMC_ERR_NO_ANSWER = -7,
  /*!
    the smart card was not reset
    */
  SMC_ERR_NOT_RESET = -8,
  /*!
    there is an error in the checksum for data transferred
    between the smart card driver and the smart card interface hardware
    */
  SMC_ERR_CHECKSUM_ERROR = -9,
  /*!
    The error flag for smc atr parse
    */
  SMC_ERR_FAILURE = -10,
} smc_error_t;

typedef struct
{
  u8 length;
  u8 ts;
  u8 t0;
  struct
  {
    u8 value;
    u8 present;
  }
  ib[ATR_MAX_PROTOCOLS][ATR_MAX_IB], tck;
  u8 pn;
  u8 hb[ATR_MAX_HISTORICAL];
  u8 hbn;
}atr_t;

typedef struct
{
  u8 cam_id;
  u8 name[ATR_MAX_VENDER_LEN];
  u8 length;
}cas_vender_str_t;

cas_adapter_priv_t g_cas_priv = {0};
static u8 atr_num_ib_table[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
static cas_vender_str_t cas_vender[] =
{
  {CAS_ID_CUSTOM1, "UNKNOWN", 7}, /* KingVon */
  {CAS_ID_CUSTOM2, "UNKNOWN", 7}, /* KingVon */
  {CAS_ID_CUSTOM3, "UNKNOWN", 7}, /* KingVon */
  {CAS_ID_CUSTOM4, "UNKNOWN", 7}, /* KingVon */
  {CAS_ID_CUSTOM5, "UNKNOWN", 7}, /* KingVon */
  {CAS_ID_ADT_MG, "ADTCA", 5}, /* ADT */
  {CAS_ID_TR, "TRC", 3}, /* Topreal */
  {CAS_ID_KV, "WD()", 4}, /* KingVon */
  {CAS_ID_TF, "NTIC", 4}, /* TongFang */
  {CAS_ID_SV, {0x1c,0x72}, 2}, /* Sumavision */
  {CAS_ID_SV, {0x1c,0x90}, 2}, /* Sumavision */
  {CAS_ID_SV, {0x09,0x80}, 2}, /* Sumavision */
  {CAS_ID_SV, {0x26,0x72}, 2}, /* Sumavision */
  {CAS_ID_SV, {0x21,0x90}, 2}, /* Sumavision */
  {CAS_ID_CONAXK, "0B00", 4}, /* Konka */
  {CAS_UNKNOWN, "DVN", 3}, /* DVN */
  {CAS_UNKNOWN, "cti", 3}, /* cti */
  {CAS_UNKNOWN, "SVM", 3}, /* YongXinShiBo */
  {CAS_UNKNOWN, "aSCCHC", 6}, /* ChangHong */
  {CAS_UNKNOWN, "dreamcrypt", 10}, /* Dreamcrypt */
  {CAS_ID_IRDETO, "IRDETO", 6}, /* IRDETO */
  {CAS_UNKNOWN, "WD)Z", 4}, /* ChengDuYingJi */
  {CAS_UNKNOWN, "CDM", 3}, /* CDM */
  {CAS_ID_BY, "tYq", 3}, /* BYCAS */
  {CAS_ID_GS, {0x0,0x32,0x0}, 3}, /* GS */
  {CAS_ID_DS, {0x43, 0x61}, 2}, /* DS */
  {CAS_ID_DS, {0x33, 0xf7}, 2}, /* DS */
  {CAS_ID_WF, "WF", 2}, /* WF */
  {CAS_ID_WF, "WG", 2}, /* WF */
  {CAS_ID_WF, "WH", 2}, /* WF */
  {CAS_ID_WF, "WI", 2}, /* WF */
  {CAS_ID_YJ, {0x50,0x53,0x29,0x5a}, 4}, /* ChengDuYingJi */
  {CAS_ID_YJ, {0x00,0x85,0x50,0x01}, 4}, /* ChengDuYingJi */
  {CAS_ID_YJ, {0x57,0x44,0x29,0x46}, 4}, /* ChengDuYingJi */
  {CAS_ID_ABV, {0x41,0x42}, 2}, /* ABV */
  {CAS_ID_ZHH, {0x57,0x44,0x33,0x31,0x41}, 5}, /* JieXiang*/
  {CAS_ID_KV40, "KV", 2}, /* KingVon */
};



#define MAX_NODE_NUM (256)
#ifdef ENABLE_CONAXK
#define MAIL_MAX_NODE_NUM (50)
#else
#define MAIL_MAX_NODE_NUM (100)
#endif
#define ANNOUNCE_MAX_NODE_NUM (50)
#define SERIAL_MAX_NODE_NUM (1)
#define CAS_DATA_TYPE_SIZE (4)
#define CAS_TEMP_DATA_SIZE (2 * (KBYTES))
/*!
  CAS save data type
  */
typedef enum
{
  /*!
    mail data
    */
  MAIL_DATA_TYPE = 1,
  /*!
    announce data
    */
  ANNOUNCE_DATA_TYPE,  
  /*!
    stb serial data
    */
  STB_SERIAL_DATA_TYPE,
  /*!
    prog level data
    */
  PROG_LEVEL_DATA_TYPE,  
  /*!
    max
    */
  DATA_TYPE_MAX
}cas_save_data_type_t;

/*!
  align to 4 bytes
  */
#define ALIGN4(x)    (((x) + 3) & (~3))

static cas_mail_headers_t mail_header;// = {0};
cas_announce_headers_t announce_header;
static u16 g_mail_node_id[MAIL_MAX_NODE_NUM] = {0};
u16 g_announce_node_id[ANNOUNCE_MAX_NODE_NUM] = {0};
static u16 g_stb_serial[SERIAL_MAX_NODE_NUM] = {0};
static u32 g_mail_max_index = 0;
static u32 g_announce_max_index = 0;

/*
**************the internal functions used by CAS adapter ***********
*/
static u16 _dump_cas_desc(u8 *p_data, cas_pmt_t *p_pmt, u16 es_pid)
{
  u16 cnt = 0;
  u16 desc_len = p_data[1]+2;
#if defined(CUSTOMER_JIZHONG_ANXIN)  
  u16 i;
#endif

  cnt ++;

  //skip desc len
  cnt ++;

  p_pmt->cas_descr[p_pmt->ecm_cnt].ca_sys_id = MAKE_WORD2(p_data[cnt],
        p_data[cnt + 1]);
  cnt += 2;

  p_pmt->cas_descr[p_pmt->ecm_cnt].ecm_pid
        = MAKE_WORD2(SYS_GET_LOW_FIVE_BITS(p_data[cnt]),
                                                    p_data[cnt + 1]);
  cnt += 2;

  p_pmt->cas_descr[p_pmt->ecm_cnt].es_pid = es_pid;

#if defined(CUSTOMER_JIZHONG_ANXIN)
  // Beginning of Modiciation
  // There are too many cas dscriptor in PMT in Anxin, only keep the last one
  for (i=0; i< p_pmt->ecm_cnt; i++)
  {
    if (p_pmt->cas_descr[i].ca_sys_id == p_pmt->cas_descr[p_pmt->ecm_cnt].ca_sys_id)
    {
      p_pmt->cas_descr[i].ecm_pid = p_pmt->cas_descr[p_pmt->ecm_cnt].ecm_pid;
      break;
    }
  }
  if (i == p_pmt->ecm_cnt)
  {
    p_pmt->ecm_cnt++;
  }
  // End Of Modification
#else
  p_pmt->ecm_cnt++;
#endif
  MT_ASSERT(p_pmt->ecm_cnt <= DVB_MAX_ECM_DESC_NUM);

  return desc_len;
}

static u16 _parse_program_info_desc(u8 *p_data, u32 len, cas_pmt_t *p_pmt)
{
  u32 cnt = 0; //cnt must be the same type as the variable len to avoid a bomb

  while(cnt < len)
  {
    switch(p_data[cnt])
    {
      case DVB_CA_DESC_ID:
        _dump_cas_desc(&p_data[cnt], p_pmt, 0);
        break;

      default:
        break;
    }
    cnt += (p_data[cnt + 1] + 2);
  }

  return 0;
}

static s32 _atr_parse(atr_t *p_atr, u8 *p_buf, u8 length)
{
  u8 td_i = 0;
  u8 pointer = 0, pn = 0;
  u8 i = 0;

  memset(p_atr, 0, sizeof(atr_t));

  /* Check size of buffer */
  if (length < 2)
  {
    return SMC_ERR_WRONG_TS_CHAR;
  }

  /* Store T0 and TS */
  p_atr->ts = p_buf[0];

  p_atr->t0 = td_i = p_buf[1];
  pointer = 1;

  /* Store number of historical bytes */
  p_atr->hbn = td_i & 0x0F;

  /* tck is not present by default */
  (p_atr->tck).present = FALSE;

  /* Extract interface bytes */
  while (pointer < length)
  {
    /* Check buffer is long enought */
    if (pointer + atr_num_ib_table[(0xF0 & td_i) >> 4] >= length)
    {
      return SMC_ERR_WRONG_TS_CHAR;
    }
    /* Check TAi is present */
    if ((td_i | 0xEF) == 0xFF)
    {
      pointer++;
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TA].value = p_buf[pointer];
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TA].present = TRUE;
    }
    else
    {
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TA].present = FALSE;
    }
    /* Check TBi is present */
    if ((td_i | 0xDF) == 0xFF)
    {
      pointer++;
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TB].value = p_buf[pointer];
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TB].present = TRUE;
    }
    else
    {
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TB].present = FALSE;
    }

    /* Check TCi is present */
    if ((td_i | 0xBF) == 0xFF)
    {
      pointer++;
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TC].value = p_buf[pointer];
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TC].present = TRUE;
    }
    else
    {
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TC].present = FALSE;
    }

    /* Read TDi if present */
    if ((td_i | 0x7F) == 0xFF)
    {
      pointer++;
      td_i = p_atr->ib[pn][ATR_INTERFACE_BYTE_TD].value = p_buf[pointer];
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TD].present = TRUE;
      (p_atr->tck).present = ((td_i & 0x0F) != ATR_PROTOCOL_TYPE_T0);
      if (pn >= ATR_MAX_PROTOCOLS)
      {
        return SMC_ERR_WRONG_TS_CHAR;
      }
      pn++;
    }
    else
    {
      p_atr->ib[pn][ATR_INTERFACE_BYTE_TD].present = FALSE;
      break;
    }
  }

  /* Store number of protocols */
  p_atr->pn = pn + 1;

  /* Store historical bytes */
  if (pointer + p_atr->hbn >= length)
  {
    return SMC_ERR_WRONG_TS_CHAR;
  }

//  memcpy (atr->hb, buffer + pointer + 1, atr->hbn);
  for(i = 0; i < p_atr->hbn; i++)
  {
    *(p_atr->hb + i) = *(p_buf + pointer + 1 + i);
  }
  pointer += (p_atr->hbn);

  /* Store tck  */
  if ((p_atr->tck).present)
  {
    if (pointer + 1 >= length)
    {
      return SMC_ERR_WRONG_TS_CHAR;
    }

    pointer++;

    (p_atr->tck).value = p_buf[pointer];
  }

  p_atr->length = pointer + 1;
  return SMC_SUCCESS;
}

#if 0
static void _atr_dump(atr_t *p_atr)
{
  u32 i = 0;

  OS_PRINTF("atr: \n");
  for(i = 0; i < p_atr->pn; i++)
  {
    OS_PRINTF("TA%d, present[%d], val[0x%x]\n",
      i + 1, p_atr->ib[i][0].present, p_atr->ib[i][0].value);
    OS_PRINTF("TB%d, present[%d], val[0x%x]\n",
      i + 1, p_atr->ib[i][1].present, p_atr->ib[i][1].value);
    OS_PRINTF("TC%d, present[%d], val[0x%x]\n",
      i + 1, p_atr->ib[i][2].present, p_atr->ib[i][2].value);
    OS_PRINTF("TD%d, present[%d], val[0x%x]\n",
      i + 1, p_atr->ib[i][3].present, p_atr->ib[i][3].value);
  }

  OS_PRINTF("\n");

  OS_PRINTF("p_atr->hb[0]=0x%x,p_atr->hb[1]=0x%x\n",p_atr->hb[0],p_atr->hb[1]);
}
#endif
static u32 _cam_identify(u8 *p_atr_hb, u32 size)
{
  u32 i = 0, j = 0;
  u32 vender_num = 0;

  for(i = 0; i < size; i++)
  {
    vender_num = sizeof(cas_vender) / sizeof(cas_vender_str_t);
    for(j = 0; j < vender_num; j++)
    {
      if(0 == memcmp(&p_atr_hb[i], cas_vender[j].name,
        cas_vender[j].length))
      {
        return (u32)cas_vender[j].cam_id;
      }
    }
  }

  return CAS_UNKNOWN;
}

static void _slot_notify(scard_notify_t *p_event)
{
  u32 event = 0;

  event = 1 << (p_event->slot * 2);
  if(p_event->card_stat == SMC_CARD_REMOVE)
  {
    event <<= 1;
    //cas_send_event(p_event->slot, CAS_UNKNOWN, CAS_S_ADPT_CARD_REMOVE, CAS_UNKNOWN);
  }
  OS_PRINTF("### %s %d \n", __FUNCTION__, __LINE__);
  drvsvc_event_set(g_cas_priv.p_svc, event);
}

static BOOL _is_cam_actived(u32 cam_id)
{
  u32 i = 0;

  for(i = 0; i < CAS_MAX_SLOT_NUM; i++)
  {
    if(g_cas_priv.cam_id[i] == cam_id)
    {
      return g_cas_priv.slot_active[i];
    }
  }

  return FALSE;
}

static RET_CODE _init_target_cam(u32 slot, cas_module_id_t cam_id)
{
  u32 i = 0;
  cas_op_t *p_op = NULL;
  u16 cam = CAS_UNKNOWN;

  if(g_cas_priv.slot_num == 1)
  {
    /* single-slot solution */
    for(i = 0; i < CAS_ID_MAX_NUM; i++)
    {
      p_op = &g_cas_priv.cam_op[i];
      if((p_op->inited == 1) && (i != cam_id))
      {
        /* deinit the different cam */
        cas_module_deinit(i);
      }
    }
  }
  else
  {
    /* multi-slot solution */
    for(i = 0; i < CAS_MAX_SLOT_NUM; i++)
    {
      cam = g_cas_priv.cam_id[i];
      if(cam == CAS_UNKNOWN)
      {
        continue;
      }
      if((cam != cam_id) && (g_cas_priv.cam_op[cam].inited == 1))
      {
        if(i == slot)
        {
          /* deinit the different cam inited on the same slot */
          cas_module_deinit(cam);
          break;
        }
      }
    }
  }

  return cas_module_init(cam_id);
}

static u16 cas_find_free_id(cas_module_id_t module_id, u8 type)
{
  u16 i = 0xFFFF;

  switch(type)
  {
    case MAIL_DATA_TYPE:
      for (i = 0; i < MAIL_MAX_NODE_NUM; i++)
      {
        if (g_mail_node_id[i] == 0xFFFF)
        {
          g_mail_node_id[i] = i + 1;
          i += 1;
          break;
        }
      }
      break;
    case ANNOUNCE_DATA_TYPE:
      for (i = 0; i < ANNOUNCE_MAX_NODE_NUM; i++)
      {
        if (g_announce_node_id[i] == 0xFFFF)
        {
          g_announce_node_id[i] = i + 1 + MAIL_MAX_NODE_NUM;
          i = i + MAIL_MAX_NODE_NUM + 1;
          break;
        }
      }
      break;
    case STB_SERIAL_DATA_TYPE:
      g_stb_serial[0] = MAIL_MAX_NODE_NUM + ANNOUNCE_MAX_NODE_NUM + 1;
      i = MAIL_MAX_NODE_NUM + ANNOUNCE_MAX_NODE_NUM + 1;
      break;
    default:
      break;
  }
  return i;
}

static u16 find_min_index_position(cas_module_id_t module_id, u8 type)
{
  u16 i = 0;
  u32 index = 0;
  u16 position = 0;
  if(type == MAIL_DATA_TYPE)
  {
    index = mail_header.p_mail_head[0].index;

    for (i = 1; i < mail_header.max_num; i++)
    {
      if (index > mail_header.p_mail_head[i].index)
      {
        index = mail_header.p_mail_head[i].index;
        position = i;
      }
    }
  }
  else   if(type == ANNOUNCE_DATA_TYPE)
  {
    index = announce_header.p_mail_head[0].index;

    for (i = 1; i < announce_header.max_num; i++)
    {
      if (index > announce_header.p_mail_head[i].index)
      {
        index = announce_header.p_mail_head[i].index;
        position = i;
      }
    }
  }
  return position;
}

static RET_CODE _data_proc_handle(void *p_param, u32 event_status)
{
  u8 i = 0;
  cas_module_id_t module_id = CAS_UNKNOWN;
  cas_mail_header_t *p_mail_header = NULL;
  cas_mail_header_t *p_announce_header = NULL;
  u8 *p_body = NULL;
  u8 mail_max = 20;
  u8 mail_policy = 0;
  u8 announce_max = 20;
  u8 announce_policy = 0;  
  u8 type = 0;
  u8 slot_id = 0;
  u16 id = 0xFFFF;
  u16 mail_position = 0;
  u16 announce_position = 0;
  #ifdef ENABLE_CONAXK
  u8 ret =0;
  #endif

  for (i = 0; i < CAS_MAX_SLOT_NUM; i++)
  {
    if (g_cas_priv.adp_evt_info.slot_evt[i] != CAS_UNKNOWN)
    {
      module_id = g_cas_priv.adp_evt_info.slot_evt[i];
      slot_id = i;
      break;
    }
  }
  if (i >= CAS_MAX_SLOT_NUM)
  {
    return SUCCESS;
  }

  g_cas_priv.adp_evt_info.slot_evt[i] = CAS_UNKNOWN;

  if (event_status & CAS_EVT_MAIL_MASK)
  {
    type = MAIL_DATA_TYPE;
    g_cas_priv.p_buf[0] = MAIL_DATA_TYPE;
    p_mail_header = (cas_mail_header_t *)(&g_cas_priv.p_buf[CAS_DATA_TYPE_SIZE]);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_CURRENT_MAIL, p_mail_header);
    p_mail_header->new_email = 1;
    p_body = (u8 *)p_mail_header + sizeof(cas_mail_header_t);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_MAIL_MAX, &mail_max);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_MAIL_POLICY, &mail_policy);

    #ifdef ENABLE_CONAXK
    //?D??車D??車D?角赤?米?車那?t
    ret = cas_compare_mail((u8 *)(&(p_mail_header->special_id)), MAIL_COMPARE_ID);
    if(ret == 0)
    {
    	OS_PRINTF("mail special_id:%x Already exists \n",p_mail_header->special_id);
	return SUCCESS;
     }
     #endif	
	
    OS_PRINTF("mail_header.max_num:%d, mail_max:%d\n", mail_header.max_num, mail_max);
    if (mail_header.max_num >= mail_max)
    {
      switch(mail_policy)
      {
        default:
        case POLICY_BY_ID:
          mail_position = find_min_index_position(module_id, MAIL_DATA_TYPE);
          id = mail_header.p_mail_head[mail_position].m_id;
          //memmove(&mail_header.p_mail_head[0], &mail_header.p_mail_head[1],
                          //(mail_header.max_num - 1) * sizeof(cas_mail_header_t));
          p_mail_header->m_id = id;
          g_mail_max_index++;
          p_mail_header->index = g_mail_max_index;
          memcpy(&mail_header.p_mail_head[mail_position], p_mail_header,
                              sizeof(cas_mail_header_t));
          g_cas_priv.nvram_write(module_id, &id, g_cas_priv.p_buf,
                sizeof(cas_mail_header_t) + CAS_DATA_TYPE_SIZE + p_mail_header->body_len);
          break;
        case POLICY_BY_SEND_TIME:
          break;
        case POLICY_BY_SAVE_TIME:
          break;
      }
    }
    else
    {
      id = cas_find_free_id(module_id, type);
      if (id >= MAX_NODE_NUM)
      {
        return SUCCESS;
      }
      p_mail_header->m_id = id;
      g_mail_max_index++;
      p_mail_header->index = g_mail_max_index;
      memcpy(&mail_header.p_mail_head[mail_header.max_num], p_mail_header,
                  sizeof(cas_mail_header_t));
      g_cas_priv.nvram_write(module_id, &id, g_cas_priv.p_buf,
               sizeof(cas_mail_header_t) + CAS_DATA_TYPE_SIZE + p_mail_header->body_len);
      mail_header.max_num++;
    }
    cas_send_event(slot_id, module_id, CAS_C_SHOW_NEW_EMAIL, 0);
  }
  if (event_status & CAS_EVT_ANNOUNCE_MASK)
  {
    type = ANNOUNCE_DATA_TYPE;
    g_cas_priv.p_buf[0] = ANNOUNCE_DATA_TYPE;
    p_announce_header = (cas_announce_header_t *)(&g_cas_priv.p_buf[CAS_DATA_TYPE_SIZE]);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_CURRENT_ANNOUNCE, p_announce_header);
    p_announce_header->new_email = 1;
    p_body = (u8 *)p_announce_header + sizeof(cas_announce_header_t);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_ANNOUNCE_MAX, &announce_max);
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_ANNOUNCE_POLICY, &announce_policy);

    if (announce_header.max_num >= announce_max)
    {
      switch(announce_policy)
      {
        default:
        case POLICY_BY_ID:
          announce_position = find_min_index_position(module_id, ANNOUNCE_DATA_TYPE);
          id = announce_header.p_mail_head[announce_position].m_id;
          //memmove(&mail_header.p_mail_head[0], &mail_header.p_mail_head[1],
                          //(mail_header.max_num - 1) * sizeof(cas_mail_header_t));
          p_announce_header->m_id = id;
          g_announce_max_index++;
          p_announce_header->index = g_announce_max_index;
          memcpy(&announce_header.p_mail_head[mail_position], p_announce_header,
                              sizeof(cas_announce_header_t));
          g_cas_priv.nvram_write(module_id, &id, g_cas_priv.p_buf,
                sizeof(cas_announce_header_t) + CAS_DATA_TYPE_SIZE + p_announce_header->body_len);
          break;
        case POLICY_BY_SEND_TIME:
          break;
        case POLICY_BY_SAVE_TIME:
          break;
      }
    }
    else
    {
      id = cas_find_free_id(module_id, type);
      if (id >= MAX_NODE_NUM)
      {
        return SUCCESS;
      }
      p_announce_header->m_id = id;
      g_announce_max_index++;
      p_announce_header->index = g_announce_max_index;
      memcpy(&announce_header.p_mail_head[announce_header.max_num], p_announce_header,
                  sizeof(cas_announce_header_t));
      g_cas_priv.nvram_write(module_id, &id, g_cas_priv.p_buf,
               sizeof(cas_announce_header_t) + CAS_DATA_TYPE_SIZE + p_announce_header->body_len);
      announce_header.max_num++;
    }
    cas_send_event(slot_id, module_id, CAS_C_SHOW_NEW_ANNOUNCE, id);
  }  
  else if (event_status & CAS_EVT_SERIAL_MASK)
  {
    type = STB_SERIAL_DATA_TYPE;
    id = cas_find_free_id(module_id, type);
    if (id >= MAX_NODE_NUM)
    {
      return SUCCESS;
    }
    g_cas_priv.p_buf[0] = type;
    g_cas_priv.cam_op[module_id].func.io_ctrl(CAS_IOCMD_GET_SERIAL_DATA,
                                      &g_cas_priv.p_buf[CAS_DATA_TYPE_SIZE]);
    g_cas_priv.nvram_write(module_id, &id, g_cas_priv.p_buf,
                                CAS_DATA_TYPE_SIZE + CAS_CARD_SN_MAX_LEN);
  }
  else
  {
    return SUCCESS;
  }

  return SUCCESS;
}

static u32 _card_reset_by_cam()
{
  u32 i = 0;

  for(i = 0; i < CAS_ID_MAX_NUM; i++)
  {
    if((g_cas_priv.cam_op[i].attached == 1)
      && (g_cas_priv.cam_op[i].card_reset_by_cam == 1))
    {
      return i;
    }
  }

  return CAS_UNKNOWN;
}

static u32 _card_id_by_attach()
{
  u32 i = 0;

  for(i = 0; i < CAS_ID_MAX_NUM; i++)
  {
    if(g_cas_priv.cam_op[i].attached == 1)
    {
      return i;
    }
  }

  return CAS_UNKNOWN;
}

static RET_CODE _card_reset(u32 slot, scard_device_t *p_smc, scard_atr_desc_t *p_atr)
{
  scard_config_t config = {0};
  scard_slot_config_t slot_config = {0};
  u32 param[4][2] = {{372, 3571200}, {625, 6000000}, {607, 3750000}, {372, 3750000}};
  u32 i = 0;
  u32 vol_sel_ena = 0;
  RET_CODE ret;
  
#ifdef ONLY1_CA_VER
  mtos_task_delay_ms(10);
#endif
  p_atr->p_buf[0] = 0;

  scard_get_config(p_smc, &config);
  slot_config.slot = slot;
  dev_io_ctrl((void *)p_smc, SMC_IO_SLOT_CONFIG_GET, (u32)&slot_config);
  config.bit_d_rate = 1;
  for(i = 0; i < 4; i++)
  {
    config.clk_f_rate = param[i][0];
    config.clk_ref = slot_config.base_clk / param[i][1];
    scard_set_config(p_smc, &config);
    ret = scard_active(p_smc, p_atr);
    if((p_atr->p_buf[0] == 0x3B) || (p_atr->p_buf[0] == 0x3F))
    {
      return SUCCESS;
    }
  }

  /* atr invalid, change VCC voltage and try again */
  dev_io_ctrl(p_smc, SMC_IO_CHK_VCC_VOL_SEL, (u32)&vol_sel_ena);
  if(!vol_sel_ena)
  {
    OS_PRINTF("card reset failed\n");
    return ERR_FAILURE;
  }
  dev_io_ctrl(p_smc, SMC_IO_VCC_VOL_CHANGE, 0);
  for(i = 0; i < 4; i++)
  {
    config.clk_f_rate = param[i][0];
    config.clk_ref = slot_config.base_clk / param[i][1];
    scard_set_config(p_smc, &config);
    scard_active(p_smc, p_atr);
    if((p_atr->p_buf[0] == 0x3B) || (p_atr->p_buf[0] == 0x3F))
    {
      return SUCCESS;
    }
  }

  return ERR_FAILURE;
}

static RET_CODE _hotplug_handle(void *p_param, u32 event_status)
{

  void *p_smc_drv = NULL;
  scard_atr_desc_t atr = {0};
  u8 atr_buf[32] = {0};
  //u32 i = 0;
  RET_CODE ret = ERR_FAILURE;
  atr_t atr_parsed = {0};
  u8 slot = 0;
  u32 cam_id = CAS_UNKNOWN;
  card_reset_info_t card_rst_info = {0};
  u32 system_time_tick = 0x00;

  atr.p_buf = atr_buf;
  atr.atr_len = 0;
  card_rst_info.p_atr = &atr;
  if(event_status & CAS_EVT_SLOT0_MASK)
  {
    p_smc_drv = g_cas_priv.p_smc_drv[0];
    slot = 0;
  }
  else if(event_status & CAS_EVT_SLOT1_MASK)
  {
    /* reset the card and get ATR to identify the card vender */
    p_smc_drv = g_cas_priv.p_smc_drv[1];
    event_status >>= 2;
    slot = 1;
  }
  else if(event_status & CAS_EVT_SLOT2_MASK)
  {
    /* reset the card and get ATR to identify the card vender */
    p_smc_drv = g_cas_priv.p_smc_drv[2];
    event_status >>= 4;
    slot = 2;
  }
  else if(event_status & CAS_EVT_SLOT3_MASK)
  {
    /* reset the card and get ATR to identify the card vender */
    p_smc_drv = g_cas_priv.p_smc_drv[3];
    event_status >>= 6;
    slot = 3;
  }
  else
  {
    return SUCCESS;
  }

  if(event_status & CAS_EVT_SLOT0_PLUG_IN)
  {
    OS_PRINTF("------>SMC IN \n");
    /* plug in, send event */
    cas_send_event(slot, CAS_UNKNOWN, CAS_S_ADPT_CARD_INSERT, CAS_UNKNOWN);

    /* check if need to reset card by only the ca module itself */
    cam_id = _card_reset_by_cam();
    if(cam_id == CAS_UNKNOWN)
    {
      system_time_tick = mtos_ticks_get();
      OS_PRINTF(" SMC IN system_time: 0x%8x\n", system_time_tick);
      /* reset the card and get ATR to identify the card vender */
      ret = _card_reset(slot, p_smc_drv, &atr);
      system_time_tick = mtos_ticks_get();
      //OS_PRINTF("SMC Active system_time: 0x%8x retry cnt: %d\n", system_time_tick, i);

      if(SUCCESS == ret)
      {
        _atr_parse(&atr_parsed, atr.p_buf, atr.atr_len);
        //_atr_dump(&atr_parsed);
        cam_id = _cam_identify(atr_parsed.hb, atr_parsed.hbn);
        OS_PRINTF("%s %d cam_id=%d\n",__FUNCTION__,__LINE__,cam_id);
        if(CAS_UNKNOWN == cam_id)
        {
          cas_send_event(slot, CAS_UNKNOWN, CAS_E_ADPT_ILLEGAL_CARD, CAS_UNKNOWN);
          return SUCCESS;
        }
      }
      else
      {
	  	OS_PRINTF("cas adt: card reset failed\n");
        cas_send_event(slot, CAS_UNKNOWN, CAS_E_ADPT_ILLEGAL_CARD, CAS_UNKNOWN);
        return SUCCESS;
      }
    }


    if(g_cas_priv.cam_op[cam_id].attached == 0)
    {
	  OS_PRINTF("cas adt: CAS not attached\n");
      cas_send_event(slot, cam_id, CAS_E_ILLEGAL_CARD, cam_id);
      return SUCCESS;
    }

    g_cas_priv.cam_id[slot] = cam_id;
    ret = _init_target_cam(slot, cam_id);
    if(SUCCESS != ret)
    {
      cas_send_event(slot, cam_id, CAS_E_ADPT_CARD_INIT_FAIL, cam_id);
      return SUCCESS;
    }
    if(g_cas_priv.cam_op[cam_id].func.card_reset != NULL)
    {
      card_rst_info.p_smc = p_smc_drv;
      ret = g_cas_priv.cam_op[cam_id].func.card_reset(slot, &card_rst_info);
      if(SUCCESS != ret)
      {
        cas_send_event(slot, cam_id, CAS_E_ADPT_CARD_RST_FAIL, cam_id);
        return SUCCESS;
      }
    }
    g_cas_priv.slot_active[slot] = 1;

    cas_send_event(slot, cam_id, CAS_S_ADPT_CARD_RST_SUCCESS, cam_id);
    if(g_cas_priv.cam_op[cam_id].func.table_resent)
    {
      g_cas_priv.cam_op[cam_id].func.table_resent();
    }
  }
  else
  {
    /* plug out */
    OS_PRINTF("<------SMC OUT \n");
    cam_id = g_cas_priv.cam_id[slot];
    if(CAS_UNKNOWN == cam_id)
    {
      //cam_id = _card_reset_by_cam();
      cam_id = _card_id_by_attach();
    }
    OS_PRINTF("<------SMC OUT 111\n");
    g_cas_priv.cam_id[slot] = cam_id;
    if(cam_id == CAS_UNKNOWN)
    {
      OS_PRINTF("<------SMC OUT 33333\n");
      g_cas_priv.slot_active[slot] = 0;
      cas_send_event(slot, CAS_UNKNOWN, CAS_S_ADPT_CARD_REMOVE, CAS_UNKNOWN);
      /* the module was not attached yet, reset the event until the module init done */
      mtos_task_sleep(100);
      drvsvc_event_set(g_cas_priv.p_svc, event_status);      
      return SUCCESS;
    }
    OS_PRINTF("<------SMC OUT 22222\n");
    if(g_cas_priv.cam_op[cam_id].attached == 1)
    {
      OS_PRINTF("<------SMC OUT 44444\n");
      if(g_cas_priv.cam_op[cam_id].inited == 1)
      {
        OS_PRINTF("<------SMC OUT 55555\n");
        if(g_cas_priv.cam_op[cam_id].func.card_remove != NULL)
        {
          OS_PRINTF("<------SMC OUT 777777\n");
          ret = g_cas_priv.cam_op[cam_id].func.card_remove();
        }
        OS_PRINTF("<------SMC OUT 666666\n");
        cas_module_deinit(cam_id);
      }
      else
      {
        /* the module was not inited yet, reset the event until the module init done */
        mtos_task_sleep(100);
        drvsvc_event_set(g_cas_priv.p_svc, event_status);
      }
      OS_PRINTF("<------SMC OUT 88888\n");
      cas_send_event(slot, cam_id, CAS_S_ADPT_CARD_REMOVE, cam_id);
    }
    else
    {
      OS_PRINTF("<------SMC OUT 9999999\n");
      cas_send_event(slot, CAS_UNKNOWN, CAS_S_ADPT_CARD_REMOVE, cam_id);
      /* the module was not attached yet, reset the event until the module init done */
      mtos_task_sleep(100);
      drvsvc_event_set(g_cas_priv.p_svc, event_status);
    }
    g_cas_priv.slot_active[slot] = 0;
  }

  return SUCCESS;
}

/*
********** the common function used by CAS modules *******
*/

u8 cas_modify_vender(u8 *atr, u32 size)
{
  u32 cam_id = CAS_UNKNOWN;
  u8 i = 0;
  
  cam_id = _cam_identify(atr, size);
  
  if(cam_id == CAS_UNKNOWN)
  {
    for(i=0; i<5; i++)
    {      
      if(0 == memcmp("UNKNOWN", cas_vender[i].name,cas_vender[i].length))
      {
        if(size > ATR_MAX_VENDER_LEN)
        {
          size = ATR_MAX_VENDER_LEN;
        }
        
        memcpy(cas_vender[i].name,atr,size);
        cas_vender[i].length = size;
        OS_PRINTF("cam_id = %d, name = %s, size = %d\n",
                   cas_vender[i].cam_id,cas_vender[i].name,cas_vender[i].length);
        
        return cas_vender[i].cam_id;
      }
    }
  }
  
  return cam_id;
}


cas_adapter_priv_t *cas_get_private_data()
{
  return &g_cas_priv;
}

void cas_get_mail_headers(cas_module_id_t module_id, u8 *p_buffer)
{
  //u16 node_id;
  //u8 type = MAIL_DATA_TYPE;

  memcpy(p_buffer, &mail_header, sizeof(cas_mail_headers_t));
}

/*
********** the common function used by CAS modules *******
*/
void cas_get_announce_headers(cas_module_id_t module_id, u8 *p_buffer)
{
  memcpy(p_buffer, &announce_header, sizeof(cas_announce_headers_t));
}
/*!
  compare mail headers

return:
  0:old mail
  1:new mail
  */
u8 cas_compare_mail(u8 *p_data, cas_mail_compare_type_t type)
{
  u8 num = 0;
  for(num = 0; num < mail_header.max_num; num ++)
  {
    switch(type)
    {
      case MAIL_COMPARE_ID:
        if(mail_header.p_mail_head[num].special_id == *(u32 *)p_data)
        {
          return 0;
        }
        break;
      case MAIL_COMPARE_SEND_DATE:

        /*			  
        if(memncmp(p_data, mail_header.p_mail_head[num].send_date, CAS_MAIL_DATE_MAX_LEN) == 0)
        {
          return 0;
        }
        */
        break;
      case MAIL_COMPARE_CREATE_DATE:
        /*
        if(memncmp(p_data, mail_header.p_mail_head[num].creat_date, CAS_MAIL_DATE_MAX_LEN) == 0)
        {
          return 0;
        }
        */
        break;
      default:
        break;
    }
  }
  return 1;
}

/*!
  compare announce headers

return:
  0:old mail
  1:new mail
  */
u8 cas_compare_announce(u8 *p_data, cas_mail_compare_type_t type)
{
  u8 num = 0;
  for(num = 0; num < announce_header.max_num; num ++)
  {
    switch(type)
    {
      case MAIL_COMPARE_ID:
        if(announce_header.p_mail_head[num].special_id == *(u32 *)p_data)
        {
          return 0;
        }
        break;
      case MAIL_COMPARE_SEND_DATE:
        /*
        if(memncmp(p_data, mail_header.p_mail_head[num].send_date, CAS_MAIL_DATE_MAX_LEN) == 0)
        {
          return 0;
        }
        */
        break;
      case MAIL_COMPARE_CREATE_DATE:
        /*
        if(memncmp(p_data, mail_header.p_mail_head[num].creat_date, CAS_MAIL_DATE_MAX_LEN) == 0)
        {
          return 0;
        }
        */
        break;
      default:
        break;
    }
  }
  return 1;
}

void cas_get_mail_body(cas_module_id_t module_id, u8 *p_buffer, u32 mail_id)
{
  u8 i = 0;
  u16 node_id = (u16)mail_id;
  u8 temp[CAS_TEMP_DATA_SIZE] = {0};
  u16 len = 0;
  u16 offset = CAS_DATA_TYPE_SIZE + sizeof(cas_mail_header_t);
  cas_mail_header_t *p_mail_header = NULL;

  for (i = 0; i < mail_header.max_num; i++)
  {
    if (mail_header.p_mail_head[i].m_id == mail_id)
    {
      if (mail_header.p_mail_head[i].new_email)
      {
        mail_header.p_mail_head[i].new_email = 0;
        len = offset + mail_header.p_mail_head[i].body_len;
        g_cas_priv.nvram_read(module_id, node_id, 0, len, (u8 *)&temp);
        p_mail_header = (cas_mail_header_t *)(temp + CAS_DATA_TYPE_SIZE);
        p_mail_header->new_email = 0;
        memcpy(p_buffer, temp + offset, mail_header.p_mail_head[i].body_len);
        g_cas_priv.nvram_write(module_id, (u16 *)&node_id, (u8 *)&temp, len);
      }
      else
      {
        len = mail_header.p_mail_head[i].body_len;
        g_cas_priv.nvram_read(module_id, node_id, offset, len, p_buffer);
      }
      break;
    }
  }
}

void cas_get_announce_body(cas_module_id_t module_id, u8 *p_buffer, u32 mail_id)
{
  u8 i = 0;
  u16 node_id = (u16)mail_id;
  u8 temp[CAS_TEMP_DATA_SIZE] = {0};
  u16 len = 0;
  u16 offset = CAS_DATA_TYPE_SIZE + sizeof(cas_announce_header_t);
  cas_announce_header_t *p_announce_header = NULL;

  for (i = 0; i < announce_header.max_num; i++)
  {
    if (announce_header.p_mail_head[i].m_id == mail_id)
    {
      if (announce_header.p_mail_head[i].new_email)
      {
        announce_header.p_mail_head[i].new_email = 0;
        len = offset + announce_header.p_mail_head[i].body_len;
        g_cas_priv.nvram_read(module_id, node_id, 0, len, (u8 *)&temp);
        p_announce_header = (cas_announce_header_t *)(temp + CAS_DATA_TYPE_SIZE);
        p_announce_header->new_email = 0;
        memcpy(p_buffer, temp + offset, announce_header.p_mail_head[i].body_len);
        g_cas_priv.nvram_write(module_id, (u16 *)&node_id, (u8 *)&temp, len);
      }
      else
      {
        len = announce_header.p_mail_head[i].body_len;
        g_cas_priv.nvram_read(module_id, node_id, offset, len, p_buffer);
      }
      break;
    }
  }
}

void cas_del_mail_data(cas_module_id_t module_id, u32 mail_id)
{
  u8 i = 0;
  u16 node_id = (u16)mail_id;

  g_cas_priv.nvram_del(module_id, node_id);

  g_mail_node_id[mail_id - 1] = 0xFFFF;
  for (i = 0; i < mail_header.max_num - 1; i++)
  {
    if (mail_header.p_mail_head[i].m_id == mail_id)
    {
      memmove(&mail_header.p_mail_head[i], &mail_header.p_mail_head[i + 1],
                    (mail_header.max_num - 1 - i) * sizeof(cas_mail_header_t));
      mail_header.max_num--;
      break;
    }
  }
  if (mail_header.p_mail_head[mail_header.max_num - 1].m_id == mail_id)
  {
    mail_header.max_num--;
  }
}

void cas_del_announce_data(cas_module_id_t module_id, u32 mail_id)
{
  u8 i = 0;
  u16 node_id = (u16)mail_id;

  g_cas_priv.nvram_del(module_id, node_id);

  g_announce_node_id[mail_id - 1] = 0xFFFF;
  for (i = 0; i < announce_header.max_num - 1; i++)
  {
    if (announce_header.p_mail_head[i].m_id == mail_id)
    {
      memmove(&announce_header.p_mail_head[i], &announce_header.p_mail_head[i + 1],
                    (announce_header.max_num - 1 - i) * sizeof(cas_announce_header_t));
      announce_header.max_num--;
      break;
    }
  }
  if (announce_header.p_mail_head[announce_header.max_num - 1].m_id == mail_id)
  {
    announce_header.max_num--;
  }
}

void cas_del_all_mail_data(cas_module_id_t module_id)
{
  u8 i = 0;

  for (i = 0; i < mail_header.max_num; i ++)
  {
    g_cas_priv.nvram_del(module_id, mail_header.p_mail_head[i].m_id);
  }

  memset(&mail_header, 0, sizeof(cas_mail_headers_t));

  for (i = 0; i < MAIL_MAX_NODE_NUM; i ++)
  {
    g_mail_node_id[i] = 0xFFFF;
  }

}

void cas_del_all_announce_data(cas_module_id_t module_id)
{
  u8 i = 0;

  for (i = 0; i < announce_header.max_num; i ++)
  {
    g_cas_priv.nvram_del(module_id, announce_header.p_mail_head[i].m_id);
  }

  memset(&announce_header, 0, sizeof(cas_announce_headers_t));

  for (i = 0; i < ANNOUNCE_MAX_NODE_NUM; i ++)
  {
    g_announce_node_id[i] = 0xFFFF;
  }

}

void cas_get_stb_serial(cas_module_id_t module_id, u8 *p_buffer)
{
  if (g_stb_serial[0] == 0)
  {
    return;
  }

  g_cas_priv.nvram_read(module_id, g_stb_serial[0], CAS_DATA_TYPE_SIZE,
                    CAS_CARD_SN_MAX_LEN, p_buffer);
}

void cas_init_nvram_data(cas_module_id_t module_id)
{
  u16 node_id[MAX_NODE_NUM] = {0};
  u16 total_num = MAX_NODE_NUM;
  u16 i = 0;
  u32 read_len = 0;
  u8 type = 0;
  BOOL new_mail_flag = FALSE;

  g_cas_priv.nvram_node_list(module_id, (u16 *)&node_id, (u16 *)&total_num);

  for (i = 0; i < total_num; i++)
  {
    read_len = g_cas_priv.nvram_read(module_id, node_id[i], 0, 1, &type);
    if (read_len == 0)
    {
      continue;
    }

    switch(type)
    {
      case MAIL_DATA_TYPE:
        g_mail_node_id[node_id[i] - 1] = node_id[i];
        read_len = g_cas_priv.nvram_read(module_id, node_id[i], CAS_DATA_TYPE_SIZE,
                    sizeof(cas_mail_header_t), (u8 *)&mail_header.p_mail_head[mail_header.max_num]);
        if (read_len == 0)
        {
          break;
        }
        if (g_mail_max_index < mail_header.p_mail_head[mail_header.max_num].index)
        {
          g_mail_max_index = mail_header.p_mail_head[mail_header.max_num].index;
        }

        if ((!new_mail_flag) && (mail_header.p_mail_head[mail_header.max_num].new_email))
        {
          OS_PRINTF("db has new mail\n");
          cas_send_event(0, module_id, CAS_C_SHOW_NEW_EMAIL, 0);//slot id::temp solution
          new_mail_flag = TRUE;
        }
        mail_header.max_num++;
        break;
      case ANNOUNCE_DATA_TYPE:
        g_announce_node_id[node_id[i] - 1] = node_id[i];
        read_len = g_cas_priv.nvram_read(module_id, node_id[i], CAS_DATA_TYPE_SIZE,
                    sizeof(cas_mail_header_t), (u8 *)&announce_header.p_mail_head[announce_header.max_num]);
        if (read_len == 0)
        {
          break;
        }
        if (g_announce_max_index < announce_header.p_mail_head[announce_header.max_num].index)
        {
          g_announce_max_index = announce_header.p_mail_head[announce_header.max_num].index;
        }
/*
        if ((!new_mail_flag) && (announce_header.p_mail_head[announce_header.max_num].new_email))
        {
          OS_PRINTF("db has new mail\n");
          cas_send_event(0, module_id, CAS_C_SHOW_NEW_EMAIL, 0);//slot id::temp solution
          new_mail_flag = TRUE;
        }
*/        
        announce_header.max_num++;
        break;
      case STB_SERIAL_DATA_TYPE:
        g_stb_serial[0] = node_id[i];
        break;
      default:
        break;
    }
  }
}

void cas_send_evt_to_adapter(u32 slot, cas_module_id_t cam_id, u32 event)
{
  if(event == 0)
  {
    return;
  }

  g_cas_priv.adp_evt_info.slot_evt[slot] = cam_id;

  drvsvc_event_set(g_cas_priv.p_data_svc, event);
}

void cas_send_event(u32 slot, cas_module_id_t cam_id, u32 event, u32 param)
{
 // u32 events[EVT_U32_LEN];

  if(event == 0)
  {
    return;
  }

  if(g_cas_priv.notify.evt_cb != NULL)
  {
    //memset(events, 0x00, EVT_U32_LEN * 4);
    //set_bit(u32 num, u32 * addr)(event - 1, events);
    g_cas_priv.notify.evt_cb(slot, event, param, cam_id, g_cas_priv.notify.context);
  }

  mtos_sem_take(&g_cas_priv.evt_mutex, 0);
  g_cas_priv.evt_info.slot_evt[slot].cam_id = cam_id;
  set_bit(event - 1, g_cas_priv.evt_info.slot_evt[slot].events);
  g_cas_priv.evt_info.slot_evt[slot].event_param = param;
  g_cas_priv.evt_info.happened[slot] = 1;
  mtos_sem_give(&g_cas_priv.evt_mutex);
}

void cas_parse_pmt(u8 *p_buffer, cas_pmt_t *p_pmt)
{
  u8 *p_buf   = p_buffer;
  u8 *p_data  = NULL;
  s16 proc_len = 0;
  u16 tmp_prog_info_len = 0;
  s16 sec_len       = 0;
  s16 prog_info_len = 0;
  s16 es_info_len   = 0;
  u16 element_pid   = 0;

  if(MASK_FIRST_BIT_OF_BYTE(p_buf[5]) == 0 || p_buf[0] != DVB_TID_PMT)
  {
    return;
  }

  //All section length including header but not including CRC bytes
  sec_len = MAKE_WORD(p_buf[2],SYS_GET_LOW_HALF_BYTE(p_buf[1])) + 3 - CAS_CRC_LEN;
  p_pmt->prog_num = MAKE_WORD(p_buf[4], p_buf[3]);
  p_pmt->pcr_pid = MAKE_WORD(p_buf[9], SYS_GET_LOW_FIVE_BITS(p_buf[8]));
  prog_info_len = MAKE_WORD(p_buf[11], SYS_GET_LOW_HALF_BYTE(p_buf[10]));
  p_data = p_buf + (proc_len = CAS_DEF_PMT_SEC_LEN);


  p_pmt->audio_count = 0;

  //Record program information length
  tmp_prog_info_len = prog_info_len;
  proc_len += prog_info_len;

  if(prog_info_len > 0)
  {
    //Parse description
    _parse_program_info_desc(p_data, prog_info_len, p_pmt);
    p_data += prog_info_len;
  }

  while(proc_len < sec_len)
  {
    element_pid = MAKE_WORD(p_data[2], SYS_GET_LOW_FIVE_BITS(p_data[1]));
    es_info_len = MAKE_WORD(p_data[4], SYS_GET_LOW_HALF_BYTE(p_data[3]));

    //OS_PRINTF("stream type 0x%x, es_id %d\n", p_data[0], element_pid);
    switch(p_data[0])
    {
      case DVB_STREAM_MG2_VIDEO:
      case DVB_STREAM_MG1_VIDEO:
      case DVB_STREAM_264_VIDEO:
      case DVB_STREAM_AVS_VIDEO:
        p_pmt->video_pid = element_pid;
        break;

      case DVB_STREAM_MG1_AUDIO:
      case DVB_STREAM_MG2_AUDIO:
      case DVB_STREAM_AAC_AUDIO:
        if(p_pmt->audio_count < DVB_MAX_AUDIO_PID_NUM)
        {
          p_pmt->audio[p_pmt->audio_count].p_id = element_pid;
          p_pmt->audio_count++;
        }
        break;

      default:
        break;
    }

    proc_len += CAS_PMT_DESC_HEAD_LEN;
    p_data += CAS_PMT_DESC_HEAD_LEN;

    if(p_data[0] == DVB_DESC_COMPONENT_DESC)
    {
      u8 stream_content = p_data[2] &0xF;
      u8 component_type = p_data[3];

      if(stream_content == 1
      && component_type >= 1
      && component_type <= 0x10)
      {
        if(p_pmt->video_pid == 0)
        {
          p_pmt->video_pid = element_pid;
        }
      }
    }

    //Move pointer
    p_data   += es_info_len;
    proc_len += es_info_len;
  }
}

void cas_parse_cat(u8 *p_buffer, cas_cat_t *p_cat)
{
  u16 cnt = 0 ;
  s32 sec_length = 0;
  u8 *p_inbuf = NULL;
  s32 desc_lenth = 0;
  u16 ca_desc_cnt = 0;
#if defined(CUSTOMER_JIZHONG_ANXIN)  
  u16 i = 0;
#endif  
  p_inbuf = p_buffer ;
  p_cat->tab_id = p_inbuf[cnt];
  cnt ++;

  //Get section length
  sec_length = MAKE_WORD2(SYS_GET_LOW_HALF_BYTE(p_inbuf[cnt]), p_inbuf[cnt + 1]);
  cnt += 2;

  //Skip reserved
  cnt += 2;
  sec_length --;

  //Get version number
  p_cat->ver_num = (p_inbuf[cnt] & 0x3e) >> 1;
  cnt ++;
  sec_length --;

  //Get section number
  p_cat->sec_num = p_inbuf[cnt];

  cnt ++;
  sec_length --;

  //Get last section number
  p_cat->last_sec_num = p_inbuf[cnt];
  cnt ++;
  sec_length --;

  while(sec_length > 0)
  {
    desc_lenth = 0;
    switch(p_inbuf[cnt])
    {
      case DVB_CA_DESC_ID:
        //skip tag
        cnt ++;
        sec_length --;

        //Get section length
        desc_lenth = p_inbuf[cnt];
        cnt ++;
        sec_length --;

        //Delete whole descriptor
        sec_length -= desc_lenth;

        p_cat->ca_desc[ca_desc_cnt].ca_sys_id = MAKE_WORD2(p_inbuf[cnt], p_inbuf[cnt + 1]);
        cnt += 2;
        desc_lenth -= 2;

        p_cat->ca_desc[ca_desc_cnt].emm_pid
                = MAKE_WORD2(SYS_GET_LOW_FIVE_BITS(p_inbuf[cnt]), p_inbuf[cnt + 1]);
        cnt += 2;
        desc_lenth -= 2;
#if defined(CUSTOMER_JIZHONG_ANXIN)
        // Beginning of Modiciation
        // There are too many cas dscriptor in PMT in Anxin, only keep the last one
        for(i=0; i<ca_desc_cnt; i++)
        {
          if (p_cat->ca_desc[ca_desc_cnt].ca_sys_id == p_cat->ca_desc[i].ca_sys_id)
          {
            p_cat->ca_desc[i].emm_pid = p_cat->ca_desc[ca_desc_cnt].emm_pid;
            break;
          }
        }
        if (i == ca_desc_cnt)
        {
          ca_desc_cnt ++;
        }
        //End of Modification
#else
        ca_desc_cnt ++;
#endif
        break;

      default:
        //skip tag
        cnt ++;
        sec_length --;

        //Get section length
        desc_lenth = p_inbuf[cnt];
        cnt ++;
        sec_length --;

        //Delete whole descriptor
        sec_length -= desc_lenth;
        break;
    }

  }

  p_cat->emm_cnt = ca_desc_cnt;
}

RET_CODE cas_table_request(void *p_dmx, cas_dmx_chan_t *p_chan,
       u16 pid,u8 *p_match,u8 *p_mask, u8 filter_mode)
{
  RET_CODE dmx_ret = ERR_FAILURE;
  dmx_chanid_t  si_channel_id = 0;
  dmx_slot_setting_t slot_t = {0};
  dmx_filter_setting_t  filter_param_t = {0};
  u8 *p_buf = NULL;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  slot_t.in     = DMX_INPUT_EXTERN0;
  slot_t.pid   = pid;
  slot_t.type = DMX_CH_TYPE_SECTION;
  if(chip_ic_id == IC_CONCERTO)
  {
    slot_t.muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;//for bug 63129
  }

  dmx_ret = dmx_si_chan_open(p_dmx, &slot_t, &si_channel_id);
  if(dmx_ret != SUCCESS)
  {
    OS_PRINTF("open si channel failed!\n");
    return ERR_FAILURE;
  }

  chip_ic_id = hal_get_chip_ic_id();
  if((chip_ic_id == IC_MAGIC) || (chip_ic_id == IC_WIZARDS))
  {
    p_chan->p_buf = p_buf = (u8 *)mtos_dma_malloc(CAS_DVB_SECTION_SIZE);
  }
  else
  {
    p_chan->p_buf = p_buf = (u8 *)mtos_malloc(CAS_DVB_SECTION_SIZE);
  }

  MT_ASSERT(p_buf != NULL);

  dmx_ret = dmx_si_chan_set_buffer(p_dmx, si_channel_id,
    p_buf, CAS_DVB_SECTION_SIZE);
  if(dmx_ret != SUCCESS)
  {
    OS_PRINTF("set filter buffer failed!\n");
    return ERR_FAILURE;
  }
  if(filter_mode & (1 << FILTER_CONTINOUS_ENABLED))
  {
    filter_param_t.continuous = TRUE;
  }
  else
  {
    filter_param_t.continuous = FALSE;
  }
  filter_param_t.en_crc = TRUE;
  filter_param_t.ts_packet_mode = DMX_THREE_MODE;

  if(p_match != NULL)
  {
    memcpy(filter_param_t.value, p_match, DMX_SECTION_FILTER_SIZE);
  }
  else
  {
    memset(filter_param_t.value, 0x00, DMX_SECTION_FILTER_SIZE);
  }

  if(p_mask != NULL)
  {
    memcpy(filter_param_t.mask, p_mask, DMX_SECTION_FILTER_SIZE);
  }
  else
  {
    memset(filter_param_t.mask, 0x00, DMX_SECTION_FILTER_SIZE);
  }

  dmx_ret = dmx_si_chan_set_filter(p_dmx, si_channel_id, &filter_param_t);
  if(dmx_ret != SUCCESS)
  {
    OS_PRINTF("set filter failed!\n");
    return ERR_FAILURE;
  }

  dmx_ret = dmx_chan_start(p_dmx, si_channel_id);
  if(dmx_ret != SUCCESS)
  {
    OS_PRINTF("start channel  failed!\n");
    return ERR_FAILURE;
  }

  p_chan->pid = pid;
  p_chan->handle = si_channel_id;

  OS_PRINTF("cas_table_request_ p_chan_pid:%d  p_chan_hand:%d  \n", pid, si_channel_id);

  return SUCCESS;
}

void cas_table_derequest(void *p_dmx, cas_dmx_chan_t *p_chan)
{
  RET_CODE ret = ERR_FAILURE;
  u32 ic_id = IC_MAGIC;

  ret = dmx_chan_stop(p_dmx, p_chan->handle);
  MT_ASSERT(ret == SUCCESS);

  ret = dmx_chan_close(p_dmx, p_chan->handle);
  MT_ASSERT(ret == SUCCESS);

  ic_id = hal_get_chip_ic_id();
  if((ic_id == IC_MAGIC) || (ic_id == IC_WIZARDS))
  {
    mtos_dma_free(p_chan->p_buf);
  }
  else
  {
    mtos_free(p_chan->p_buf);
  }
  //memset(p_chan, 0, sizeof(cas_dmx_chan_t));
  p_chan->handle = DMX_INVALID_CHANNEL_ID;
  p_chan->pid = 0x1FFF;
}

/*
************** the APIs ***********
*/
RET_CODE cas_detach(cas_module_id_t cam_id)
{
  cas_op_t *p_op = NULL;

  p_op = &g_cas_priv.cam_op[cam_id];
  if(p_op->inited == 1)
  {
    p_op->func.deinit();
  }
  p_op->inited = 0;


  if(p_op->p_priv != NULL)
  {
    mtos_free(p_op->p_priv);
    p_op->p_priv = NULL;
  }
  p_op->attached = 0;

  return SUCCESS;
}

RET_CODE cas_init(cas_adapter_cfg_t *p_cfg)
{
  u32 i = 0;

  RET_CODE ret = ERR_FAILURE;

  if(g_cas_priv.inited == 1)
  {
    return SUCCESS;
  }
  g_cas_priv.slot_num = p_cfg->slot_num;
  for(i = 0; i < p_cfg->slot_num; i++)
  {
    g_cas_priv.cam_id[i] = CAS_UNKNOWN;
    g_cas_priv.slot_active[i] = FALSE;
    g_cas_priv.p_smc_drv[i] = p_cfg->p_smc_drv[i];
    scard_register_notify(p_cfg->p_smc_drv[i], _slot_notify);
  }
  /* create task to handle the card inserting & removing */
  g_cas_priv.p_svc = drvsvc_create(p_cfg->task_prio, p_cfg->p_task_stack, p_cfg->stack_size, 1);
  MT_ASSERT(NULL != g_cas_priv.p_svc);

  if(p_cfg->cas_type == 0)
  {
    ret = drvsvc_nod_insert(g_cas_priv.p_svc, _hotplug_handle,
        NULL, CAS_EVT_SLOT_MASK, MTOS_WAIT_FOREVER);
    MT_ASSERT(SUCCESS == ret);
  }

  /* create task to handle the data process */

  if (p_cfg->data_task_prio != 0)
  {
    g_cas_priv.p_data_svc = drvsvc_create(p_cfg->data_task_prio, p_cfg->p_data_task_stack,
                              p_cfg->data_stack_size, 1);
    MT_ASSERT(NULL != g_cas_priv.p_data_svc);
    ret = drvsvc_nod_insert(g_cas_priv.p_data_svc, _data_proc_handle,
      NULL, CAS_EVT_DATA_MASK, MTOS_WAIT_FOREVER);
    MT_ASSERT(SUCCESS == ret);
  }

  for(i = 0; i < CAS_MAX_SLOT_NUM; i++)
  {
    g_cas_priv.adp_evt_info.slot_evt[i] = CAS_UNKNOWN;
  }

  MT_ASSERT(TRUE == mtos_sem_create(&g_cas_priv.evt_mutex, TRUE));

  MT_ASSERT(TRUE == mtos_sem_create(&g_cas_priv.proc_mutex, TRUE));

  memset(&mail_header, 0, sizeof(cas_mail_headers_t));

  g_cas_priv.p_buf = mtos_malloc(CAS_TEMP_DATA_SIZE);
  MT_ASSERT(NULL != g_cas_priv.p_buf);

  for (i = 0; i < MAIL_MAX_NODE_NUM; i++)
  {
    g_mail_node_id[i] = 0xFFFF;
  }
  for (i = 0; i < ANNOUNCE_MAX_NODE_NUM; i++)
  {
    g_announce_node_id[i] = 0xFFFF;
  }  
  //register data process handle
  g_cas_priv.nvram_del = p_cfg->nvram_del;
  g_cas_priv.nvram_write = p_cfg->nvram_write;
  g_cas_priv.nvram_read = p_cfg->nvram_read;
  g_cas_priv.nvram_node_list = p_cfg->nvram_node_list;
  g_cas_priv.inited = 1;
  return SUCCESS;
}

RET_CODE cas_deinit()
{
  u32 i = 0;

  if(g_cas_priv.inited == 0)
  {
    return SUCCESS;
  }

  for(i = 0; i < g_cas_priv.slot_num; i++)
  {
    g_cas_priv.cam_id[i] = CAS_UNKNOWN;
    g_cas_priv.slot_active[i] = FALSE;
    g_cas_priv.p_smc_drv[i] = NULL;
  }
  MT_ASSERT(TRUE == mtos_sem_destroy(&g_cas_priv.evt_mutex, 0));

  drvsvc_destroy(g_cas_priv.p_svc);

  for(i = 0; i < g_cas_priv.slot_num; i++)
  {
    g_cas_priv.p_smc_drv[i] = NULL;
    scard_register_notify(g_cas_priv.p_smc_drv[i], NULL);
  }
  g_cas_priv.inited = 0;

  return SUCCESS;
}

RET_CODE cas_module_init(cas_module_id_t cam_id)
{
  cas_op_t *p_op = NULL;

  p_op = &g_cas_priv.cam_op[cam_id];
  if(p_op->inited == 1)
  {
    return SUCCESS;
  }

  p_op->inited = 1;
  if(p_op->func.init == NULL)
  {
    return SUCCESS;
  }
  return p_op->func.init();
}

RET_CODE cas_module_deinit(cas_module_id_t cam_id)
{
  cas_op_t *p_op = NULL;

  p_op = &g_cas_priv.cam_op[cam_id];
  if(p_op->inited == 0)
  {
    return SUCCESS;
  }

  p_op->inited = 0;
  if(p_op->func.deinit == NULL)
  {
    return SUCCESS;
  }
  return p_op->func.deinit();
}

RET_CODE cas_event_notify_register(cas_event_notify_t *p_notify)
{
  memcpy(&g_cas_priv.notify, p_notify, sizeof(cas_event_notify_t));

  return SUCCESS;
}

RET_CODE cas_identify(u16 ca_sys_id, u32 *p_cam_id, BOOL *p_is_actived)
{
  u32 i = 0;
  cas_op_t *p_op = NULL;
  RET_CODE ret = ERR_FAILURE;

  for(i = 0; i < CAS_ID_MAX_NUM; i++)
  {
    p_op = &g_cas_priv.cam_op[i];
    if(p_op->attached == 1)
    {
      ret = p_op->func.identify(ca_sys_id);
      if(ret == SUCCESS)
      {
        *p_cam_id = i;
        break;
      }
    }
  }
  if(CAS_ID_MAX_NUM == i)
  {
    *p_cam_id = CAS_UNKNOWN;
    *p_is_actived = FALSE;
    return SUCCESS;
  }

  *p_is_actived = _is_cam_actived(i);
  return SUCCESS;
}


RET_CODE cas_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{
  u32 i = 0;
  cas_op_t *p_op = NULL;

  for(i = 0; i < CAS_ID_MAX_NUM; i++)
  {
    p_op = &g_cas_priv.cam_op[i];
    if(p_op->attached == 1)
    {
      p_op->func.table_process(t_id, p_buf, p_result);
    }
  }

  return SUCCESS;
}

RET_CODE cas_event_polling(cas_event_info_t *p_event_info)
{
  mtos_sem_take(&g_cas_priv.evt_mutex, 0);
  memcpy(p_event_info, &g_cas_priv.evt_info, sizeof(cas_event_info_t));
  memset(&g_cas_priv.evt_info, 0x00, sizeof(cas_event_info_t));
  mtos_sem_give(&g_cas_priv.evt_mutex);

  return SUCCESS;
}

cas_event_id_t cas_event_parse(u32 events[EVT_U32_LEN])
{
  u32 i = 0;

  for(i = 0; i < (EVT_U32_LEN << 5); i++)
  {
    if(test_and_clear_bit(i, events) == 1)
    {
      return (i + 1);
    }
  }

  return CAS_EVT_NONE;
}

RET_CODE cas_get_platformid(cas_module_id_t cam_id, u16 *platformid)
{
  return g_cas_priv.cam_op[cam_id].func.get_platformid(platformid);
}


RET_CODE cas_io_ctrl(u32 slot, u32 cmd, cas_io_param_t *p_param)
{
  #ifdef WIN32
  return SUCCESS;

  #else
  u32 cam_id = 0;
  cas_hotplug_info_t *p_hp_info = NULL;
  OS_PRINTF("\nslot=%d\n",slot);


  if(CAS_INVALID_SLOT == slot)
  {
    cam_id = p_param->cam_id;
    return g_cas_priv.cam_op[cam_id].func.io_ctrl(cmd, p_param->p_param);
  }
  
  cam_id = g_cas_priv.cam_id[slot];
  OS_PRINTF("\ncam_id=%d\n",cam_id);
  if(CAS_UNKNOWN == cam_id)
  {
    return ERR_FAILURE;
  }
  if(g_cas_priv.cam_op[cam_id].attached != 1)
  {
    return ERR_STATUS;
  }

  if(CAS_IOCMD_HOTPLUG_INFO_GET == cmd)
  {
    p_hp_info = (cas_hotplug_info_t *)p_param->p_param;
    memcpy(p_hp_info->cam_id, g_cas_priv.cam_id, CAS_MAX_SLOT_NUM * 2);
    memcpy(p_hp_info->slot_actived, g_cas_priv.slot_active, CAS_MAX_SLOT_NUM);
    return SUCCESS;
  }
  OS_PRINTF("\n22222cam_id=%d\n",cam_id);
  return g_cas_priv.cam_op[cam_id].func.io_ctrl(cmd, p_param->p_param);
  #endif
}

RET_CODE cas_descrambler_ctrl(u16 ca_sys_id, u32 cmd, void *p_param)
{
  u32 cam_id = CAS_UNKNOWN;
  u32 i = 0;
  cas_op_t *p_op = NULL;
  RET_CODE ret = ERR_FAILURE;

  if(ca_sys_id == 0x1FFF)
  {
    /* does not specify the cas module, called by all attached modules */
    for(i = 0; i < CAS_ID_MAX_NUM; i++)
    {
      p_op = &g_cas_priv.cam_op[i];
      if(p_op->attached == 1)
      {
        p_op->func.io_ctrl(cmd, p_param);
      }
    }
    return SUCCESS;
  }
  else
  {
    for(i = 0; i < CAS_ID_MAX_NUM; i++)
    {
      p_op = &g_cas_priv.cam_op[i];
      if(p_op->attached == 1)
      {
        ret = p_op->func.identify(ca_sys_id);
        if(ret == SUCCESS)
        {
          cam_id = i;
          break;
        }
      }
    }
    if(cam_id == CAS_UNKNOWN)
    {
      return ERR_NOFEATURE;
    }
  }

  return g_cas_priv.cam_op[cam_id].func.io_ctrl(cmd, p_param);
}
