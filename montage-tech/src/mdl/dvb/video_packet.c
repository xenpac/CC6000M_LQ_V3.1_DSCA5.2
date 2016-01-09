/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
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
//#include "lib_util.h"
#include "video_packet.h"

#include "service.h"

#define TS_SYNC_FLAG  (0x47)

#define ADPTION_NO_PAYLOAD_FLAG   (0) 
#define ADPTION_FIELD_FLAG_EXISTING_FLAG  (1)


#define SECTION_HEAD_LENGTH   (3)
#define PACKET_STARTING_IN_PAYLOAD      (1)


#define ADP_FLAG_RESEERVED              (0)
#define NO_ADP_PAYLOAD_ONLY             (1)
#define ADP_ONLY_NO_PAYLOAD             (2)
#define ADP_AND_PAYLOAD                 (3)
/*!
  TS packet length
  */
#define MAX_TS_PACKET_LEN     (188)
#define MAX_SEC_LENGTH        (4096)
#define TS_PACKET_HEADER_LEN  (4)

void parse_ts_packet(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  u8 *p_ts = NULL;
  u8 *p_sec_buf = p_sec->p_buffer;

  p_ts = p_sec_buf;

  if(p_sec->table_id == MPEG_TS_PACKET)
  {
    BOOL is_scramble = FALSE;
    os_msg_t msg = {0};
    msg.content = DVB_SCRAMBLE_FLAG_FOUND;
    msg.is_ext = 0;
    OS_PRINTF("parse_ts_packet,sid[%x], pid[%x]\n",p_sec->sid, p_sec->pid);

    is_scramble = ((p_ts[3] >> 6) != 0);
    msg.para1 = p_sec->pid | (p_sec->sid << 16);
    msg.para2 = is_scramble;
    p_svc->notify(p_svc, &msg);
    return;
  }
  
}


void request_ts_packet(dvb_section_t * p_sec, u32 para1, u32 para2)
{
  u16 sid = (para2 >> 16) & 0xFFFF;
  u16 pid = para2 & 0xFFFF;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);

  OS_PRINTF("!!!request ts packet sid [%x], pid [%x], para2[%x]\n", 
    sid, pid, para2);
  p_sec->pid      = pid;
  p_sec->table_id = MPEG_TS_PACKET;
  p_sec->timeout  = MPEG_TS_PACKET_TIMEOUT;
  p_sec->sid      = sid;
  
  p_sec->filter_mode = FILTER_TYPE_TSPKT; 
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,
    sizeof(u8) * MAX_FILTER_MASK_BYTES);
  p_sec->filter_mask_size = 1;
  //Disable CRC check
  p_sec->crc_enable = 0;
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

