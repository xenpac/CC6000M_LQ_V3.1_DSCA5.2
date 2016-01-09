/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "lib_unicode.h"

#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_mutex.h"

#include "class_factory.h"
#include "mp3_decode_frame.h"
#include "vfs.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

/*!
  packet sync byte
  */
#define SYNC_BYTE (0x47)

/*!
  parse stream len
  */
#define PARSE_STREAM_LEN (188 * 512)

/*!
  parse rec info
  */
#define PARSE_REC_INFO (188 * 10)

/*!
  ring stream pts offset min, ring stream should more than 10s
  */
#define RING_STREAM_PTS_OFFSET_MIN (45000 * 10)

/*!
  is pes header code
  */
#define PES_HEADER(data)\
  (((((char *)data)[0] == 0) && (((char *)data)[1] == 0) && (((char *)data)[2] == 0x01)) \
  && (((unsigned char *)data)[3] >= 0xBC))

/*!
  is audio stream
  */
#define IS_AUDIO_STREAM(stream_id)\
  ((stream_id >= 0xc0) && (stream_id <= 0xdf))

/*!
  is video stream
  */
#define IS_VIDEO_STREAM(stream_id)\
  ((stream_id >= 0xe0) && (stream_id <= 0xef))

static u8 *pes_packet_start(u8 *p_data, u32 *p_payload)
{
  u8 adaptation_field_control = 0;
  u8 adaptation_field_length = 0;

  adaptation_field_control = p_data[3] & 0x30;
  adaptation_field_control >>= 4;
  if (adaptation_field_control >= 2)
  {
    adaptation_field_length = p_data[4];
    adaptation_field_length += 5;
    if (adaptation_field_length > 184)
    {
      (*p_payload) = 0;
      return (u8 *)((u32)p_data + 4);
    }

    (*p_payload) = 188 - adaptation_field_length;
    return (u8 *)((u32)p_data + adaptation_field_length);
  }
  else
  {
    (*p_payload) = 184;
    return (u8 *)((u32)p_data + 4);
  }
}

static u8 *pes_data_start(u8 *p_data, u32 *p_payload)
{
  u8 pes_header_length = 0;

  pes_header_length = p_data[8];
  pes_header_length += 9;
  *p_payload -= pes_header_length;

  return (u8 *)(p_data + pes_header_length);
}

static BOOL get_pcr(u8 *p_data, unsigned long long *p_pcr)
{
  u64 pcr_base = 0;
  u16 pcr_ext = 0;
  u64 temp = 0;

  pcr_base = p_data[0];
  pcr_base <<= 25;
  temp = p_data[1];
  temp <<= 17;
  pcr_base |= temp;
  temp = p_data[2];
  temp <<= 9;
  pcr_base |= temp;
  temp = p_data[3];
  temp <<= 1;
  pcr_base |= temp;
  temp = p_data[4] & 0x80;
  temp >>= 7;
  pcr_base |= temp;

  pcr_ext = p_data[4] & 0x01;
  pcr_ext <<= 8;
  pcr_ext |= p_data[5];

  (*p_pcr) = pcr_base * 300 + pcr_ext;

  return TRUE;
}

static u8 pes_stream_type(u8 *p_data)
{
  u8 *p_pes_packet = NULL;
  u32 payload = 0;

  p_pes_packet = pes_packet_start(p_data, &payload);

  if (PES_HEADER(p_pes_packet))
  {
    if (IS_AUDIO_STREAM(p_pes_packet[3]))
    {
      return 1;//audio
    }

    if (IS_VIDEO_STREAM(p_pes_packet[3]))
    {
      return 2;//video
    }
  }

  return 0;
}

static u16 *parse_pat_table(u8 *p_data, u16 *p_svc_cnt)
{
  u8 *p_pes_packet = NULL;
  u32 payload = 0;
  u16 section_length = 0;
  u16 program_num = 0;
  u16 *p_svc_map_id = NULL;
  u16 loopi = 0;
  u16 svc_cnt = 0;

  p_pes_packet = pes_packet_start(p_data, &payload);

  p_pes_packet = p_pes_packet + p_pes_packet[0] + 1;

  section_length = p_pes_packet[1] & 0x0F;
  section_length <<= 8;
  section_length |= p_pes_packet[2];
  section_length -= 5;//pat data start pos
  section_length -= 4;//crc
  p_pes_packet = p_pes_packet + 8;
  p_svc_map_id = mtos_malloc(sizeof(u16) * (section_length / 4));
  svc_cnt = 0;
  for (loopi = 0; loopi < section_length; loopi += 4)
  {
    program_num = p_pes_packet[loopi];
    program_num <<= 8;
    program_num |= p_pes_packet[loopi + 1];

    if (program_num != 0)
    {
      p_svc_map_id[svc_cnt] = p_pes_packet[loopi + 2] & 0x1F;
      p_svc_map_id[svc_cnt] <<= 8;
      p_svc_map_id[svc_cnt] |= p_pes_packet[loopi + 3];

      svc_cnt ++;
    }
  }

  *p_svc_cnt = svc_cnt;
  return p_svc_map_id;
}

static void parse_pmt_table(u8 *p_data, u16 *p_a_pid, u16 *p_v_pid, u16 *p_pcr_pid)
{
  u8 *p_pes_packet = NULL;
  u32 payload = 0;
  u16 section_length = 0;
  u16 program_info_length = 0;
  u16 es_info_length = 0;
  u16 loopi = 0;
  u8 stream_type = 0;
  u16 elementary_pid = 0;
  u16 pcr_pid = 0;

  p_pes_packet = pes_packet_start(p_data, &payload);

  p_pes_packet = p_pes_packet + p_pes_packet[0] + 1;

  section_length = p_pes_packet[1] & 0x0F;
  section_length <<= 8;
  section_length |= p_pes_packet[2];
  section_length -= 12;
  pcr_pid = p_pes_packet[8] & 0x1F;
  pcr_pid <<= 8;
  pcr_pid |= p_pes_packet[9];
  *p_pcr_pid = pcr_pid;

  program_info_length = p_pes_packet[10] & 0x0F;
  program_info_length <<= 8;
  program_info_length |= p_pes_packet[11];
  section_length -= program_info_length;
  p_pes_packet += (12 + program_info_length);

  for (loopi = 0; loopi < section_length; loopi += (5 + es_info_length))
  {
    es_info_length = 0;
    stream_type = p_pes_packet[loopi];
    elementary_pid = p_pes_packet[loopi + 1] & 0x1F;
    elementary_pid <<= 8;
    elementary_pid |= p_pes_packet[loopi + 2];
    es_info_length = p_pes_packet[loopi + 3] & 0x0F;
    es_info_length |= p_pes_packet[loopi + 4];

    if (stream_type == 0x01 || stream_type == 0x02)
    {
      //video
      *p_v_pid = elementary_pid;
    }

    if (stream_type == 0x03 || stream_type == 0x04)
    {
      //audio
      *p_a_pid = elementary_pid;
    }
  }

}

static u8 match_svc_id(u16 *p_svc_map, u16 cnt, u16 pid)
{
  u16 loopi = 0;

  for (loopi = 0; loopi < cnt; loopi ++)
  {
    if (pid == p_svc_map[loopi])
    {
      return 1;
    }
  }
  return 0;
}

static u32 ts_packet_head_offset(hfile_t file)
{
  u8 *p_heads = NULL;
  u32 loopi = 0;
  u32 read_size = 0;
  u32 head_size = 188 * 6;

  p_heads = mtos_malloc(head_size);
  MT_ASSERT(p_heads != NULL);

  vfs_seek(file, 0, VFS_SEEK_SET);
  read_size = vfs_read(p_heads, head_size, 1, file);
  if (read_size != head_size)
  {
    mtos_free(p_heads);
    p_heads = NULL;
    return 0;
  }

  for (loopi = 0; loopi < 3 * 188; loopi ++)
  {
    if (p_heads[loopi] == 0x47)
    {
      if (p_heads[loopi + 188] == 0x47
        && p_heads[loopi + 2 * 188] == 0x47
        && p_heads[loopi + 3 * 188] == 0x47)
      {
        break;
      }
    }
  }

  mtos_free(p_heads);
  p_heads = NULL;
  return loopi;
}
/*
static u32 ts_packet_end_offset(hfile_t file)
{
  u8 *p_heads = NULL;
  u32 loopi = 0;
  u32 read_size = 0;
  u32 head_size = 188 * 6;

  p_heads = mtos_align_malloc(head_size, 4);
  MT_ASSERT(p_heads != NULL);

  vfs_seek(file, head_size, VFS_SEEK_TAIL);
  read_size = vfs_read(p_heads, head_size, 1, file);
  if (read_size != head_size)
  {
    mtos_align_free(p_heads);
    p_heads = NULL;
    return 0;
  }

  for (loopi = head_size - 186; loopi > 3 * 188; loopi --)
  {
    if (p_heads[loopi] == 0x47)
    {
      if (p_heads[loopi - 188] == 0x47
        && p_heads[loopi - 2 * 188] == 0x47
        && p_heads[loopi - 3 * 188] == 0x47)
      {
        break;
      }
    }
  }
  mtos_align_free(p_heads);
  p_heads = NULL;

  return (loopi % 188);
}
*/
static BOOL is_pcr_packet(u8 *p_data)
{
  u32 adaptation_field_control = 0;
  u8 pcr_flag = 0;

  adaptation_field_control = p_data[3] & 0x30;

  if (p_data[4])
  {
    pcr_flag = p_data[5] & 0x10;
    if (pcr_flag)
    {
      return TRUE;
    }
  }

  return FALSE;
}

BOOL parse_pcr(u8 *p_data, unsigned long long *p_pcr)
{
  u8 adaptation_field_control = 0;
  u8 adaptation_field_length = 0;
  u8 pcr_flag = 0;

  adaptation_field_control = p_data[3] & 0x30;
  adaptation_field_control >>= 4;
  if (adaptation_field_control != 2 && adaptation_field_control != 3)
  {
    return FALSE;
  }

  adaptation_field_length = p_data[4];
  if (adaptation_field_length < 5)
  {
    return FALSE;
  }

  pcr_flag = p_data[5] & 0x10;

  if (pcr_flag > 0)
  {
    get_pcr(&p_data[6], p_pcr);

    return TRUE;
  }

  return FALSE;
}

u32 parse_bit_rate(u8 *p_data, u32 payload, u16 pcr_pid)
{
  u8 *p_packet = NULL;
  u32 loopi = 0;
  u64 pcr1 = 0;
  u64 pcr2 = 0;
  u8 pcr_cnt = 0;
  u32 bit_rate = 0;
  u64 temp = 0;
  u32 packet_cnt = 0;

  if (!pcr_pid || (payload < TS_PACKET_LEN * 10))
  {
    return 0;
  }

  for (loopi = 0; loopi < payload; loopi += TS_PACKET_LEN)
  {
    p_packet = p_data + loopi;
    if (p_packet[0] != SYNC_BYTE)
    {
      OS_PRINTF("\n##parse_bit_rate: ts packet error, head is not 0x47!");
      return 0;
    }
    if (TS_PID(p_packet) == pcr_pid)
    {
      if (parse_pcr(p_packet, &pcr1))
      {
        pcr_cnt ++;
        loopi += TS_PACKET_LEN;
        break;
      }
    }
  }

  for (; loopi < payload; loopi += TS_PACKET_LEN)
  {
    p_packet = p_data + loopi;
    packet_cnt ++;
    if (p_packet[0] != SYNC_BYTE)
    {
      OS_PRINTF("\n##parse_bit_rate: ts packet error, head is not 0x47!");
      return 0;
    }
    if (TS_PID(p_packet) == pcr_pid)
    {
      if (parse_pcr(p_packet, &pcr2))
      {
        pcr_cnt ++;
        break;
      }
    }
  }

  if ((pcr_cnt < 2) || !pcr1 || !pcr2)
  {
    return 0;
  }

  /*!
    188 * 8 * 27000000
    */
  temp = packet_cnt;
  temp = temp * 188 * 8 * 27000000;
  pcr2 = pcr2 - pcr1;
  if (pcr2)
  {
    bit_rate = temp / pcr2;
  }

  OS_PRINTF("\n##debug: parse_bit_rate [%ld]\n", bit_rate);

  return bit_rate;
}

u32 parse_pts(u8 *p_ts_packet, u32 payload, u16 v_pid)
{
  u8 *p_start = NULL;
  u32 loopi = 0;
  u32 v_packet_cnt = 0;
  u8 ps_packet[188 * 2] = {0};
  u8 *p_ps_start = NULL;
  u32 ps_payload = 0;
  u32 temp = 0;
  u64 tmp = 0;
  u8 PTS_DTS_flag = 0;
  u8 stream_id = 0;
  u64 pts = 0;

  while (loopi < payload)
  {
    v_packet_cnt = 0;
    ps_payload = 0;
    for (; loopi < payload; loopi += 188)
    {
      p_start = (u8 *)((u32)p_ts_packet + loopi);
      if (p_start[0] != SYNC_BYTE)
      {
        OS_PRINTF("\n##parse_pts: ts packet error, head is not 0x47!");
        return 0;
      }
      if (v_pid == TS_PID(p_start)
        && PAYLOAD_UNIT_START(p_start))
      {
        break;
      }
    }
    if (loopi >= payload)
    {
      break;
    }
    for (; loopi < payload; loopi += 188)
    {
      p_start = (u8 *)((u32)p_ts_packet + loopi);
      if (p_start[0] != SYNC_BYTE)
      {
        OS_PRINTF("\n##parse_pts: ts packet error, head is not 0x47!");
        return 0;
      }
      if (v_pid == TS_PID(p_start))
      {
        v_packet_cnt ++;
        p_ps_start = pes_packet_start(p_start, &temp);
        if (temp)
        {
          memcpy(ps_packet + ps_payload, p_ps_start, temp);
        }
        ps_payload += temp;
        if (v_packet_cnt == 2
          || ps_payload > 14)
        {
          break;
        }
      }
    }
    if (loopi >= payload)
    {
      break;
    }
    if (!PRE_START_CODE(ps_packet))
    {
      loopi += 188;
      continue;
    }

    if (v_packet_cnt < 1
      || ps_payload < 14)
    {
      loopi += 188;
      continue;
    }
    if (stream_id == 0xBC
      || stream_id == 0xBD
      || stream_id == 0xBE
      || stream_id == 0xF0
      || stream_id == 0xF1
      || stream_id == 0xFF)
    {
      loopi += 188;
      continue;
    }
    PTS_DTS_flag = ps_packet[7] & 0xC0;
    PTS_DTS_flag >>= 6;

    if (PTS_DTS_flag >= 2)
    {
      if (ps_packet[9] & 0xC0)
      {
        OS_PRINTF("\n##parse_pts: ts packet error, pts dts flag error!");
        return 0;
      }
      pts = ps_packet[9] & 0x0e;
      pts >>= 1;
      pts <<= 30;
      tmp = ps_packet[10];
      tmp <<= 22;
      pts |= tmp;
      tmp = ps_packet[11]&0xfe;
      tmp <<= 14;
      pts |= tmp;
      tmp = ps_packet[12];
      tmp <<= 7;
      pts |= tmp;
      tmp = ps_packet[13]&0xfe;
      tmp >>= 1;
      pts |= tmp;

      return (pts >> 1);
    }
    else
    {
      loopi += 188;
    }
  }
  return 0;
}

void get_idx_info_file(const u16 *p_ts_file_name, u16 *p_idx_file_name, u32 idx_name_len)
{
  u16 *p_str = NULL;
  u16 uni_idx[6] = {0};

  if (idx_name_len < uni_strlen(p_ts_file_name))
  {
    return ;
  }

  uni_strcpy(p_idx_file_name, p_ts_file_name);

  p_str = uni_strrchr(p_idx_file_name, 0x2e/*'.'*/);
  if (p_str)
  {
    p_str[0] = 0;
  }

  str_asc2uni(".ridx", uni_idx);
  uni_strcat(p_idx_file_name, uni_idx, idx_name_len);
}

void file_sink_split_file_name(u16 *p_file_name, u32 index)
{
  u16 temp[MAX_FILE_PATH] = {0};
  u16 *p_ch = NULL;
  u16 split[32] = {0};
  u8 asc_str[32] = {0};

  if (index <= 1)
  {
    return ;
  }
  
  p_ch = uni_strrchr(p_file_name, 0x2e/*'.'*/);
  if (p_ch != NULL)
  {
    uni_strcpy(temp, p_ch);
    p_ch[0] = 0/*'\0'*/;
  }

  if (index > 2)
  {
    p_ch = uni_strrchr(p_file_name, 0x5f/*'_'*/);
    if (p_ch != NULL)
    {
      p_ch[0] = 0/*'\0'*/;
    }
  }
  sprintf(asc_str, "_%d", (unsigned int)(index));
  str_asc2uni(asc_str, split);

  uni_strcpy(p_file_name + uni_strlen(p_file_name), split);
  uni_strcpy(p_file_name + uni_strlen(p_file_name), temp);  
}

void parse_pes_data(u8 *p_ts_packet, u8 **pp_pes_data, u32 *p_payload)
{
  u8 *p_pes_data = NULL;

  p_pes_data = pes_packet_start(p_ts_packet, p_payload);

  if (*p_payload && PES_HEADER(p_pes_data))
  {
    p_pes_data = pes_data_start(p_pes_data, p_payload);
  }
  else
  {
    *p_payload = 0;
  }

  *pp_pes_data = p_pes_data;
}

rec_info_type_t is_rec_info_packet(u8 *p_packet, u8 *p_total_cnt, u8 *p_cur_index)
{
  if (p_packet[0] != SYNC_BYTE)
  {
    return REC_INFO_UNKONOWN;
  }

  if (TS_PID(p_packet) != REC_INFO_PID)
  {
    return REC_INFO_UNKONOWN;
  }

  if (!p_packet[3] || p_packet[6] != 'r' || !p_packet[7])
  {
    return REC_INFO_UNKONOWN;
  }

  if (p_total_cnt)
  {
    *p_total_cnt = p_packet[3];
  }
  if (p_cur_index)
  {
    *p_cur_index = p_packet[4];
  }

  return p_packet[5];
}

u32 split_packet_num(u32 info_len)
{
  return ((info_len - 1) / 180) + 1;
}


u32 calc_split_packet_length(u32 info_len)
{
  return split_packet_num(info_len) * 188;
}

u32 packet_dummy_info(u8 *p_packet, u32 info_len)
{
  u16 pid = REC_INFO_PID;
  u8 *p_start = p_packet;
  u32 total_cnt = info_len / 188;
  u32 loopi = 0;

  memset(p_packet, 0, info_len);

  for (loopi = 0; loopi < total_cnt; loopi ++)
  {
    p_start[0] = SYNC_BYTE;
    p_start[1] = (pid >> 8);
    p_start[2] = pid;
    p_start[3] = total_cnt;
    p_start[4] = loopi;
    p_start[5] = REC_INFO_DUMMY;
    p_start[6] = 'r';
    p_start[7] = 0;

    p_start += 188;
  }
  return total_cnt * 188;
}

u32 packet_rec_info(u8 *p_packet, rec_info_type_t rec_info_type, u8 *p_info, u32 info_len)
{
  u16 pid = REC_INFO_PID;
  u8 *p_start = p_packet;
  u8 payload = 0;
  u8 total_cnt = 0;
  u8 loopi = 0;

  total_cnt = split_packet_num(info_len);
  payload = info_len % 180;

  if (rec_info_type == REC_INFO_DYNAMIC)
  {
    MT_ASSERT(info_len <= 180);
  }

  for (loopi = 1; loopi <= total_cnt; loopi ++)
  {
    memset(p_start, 0x00, 188);

    p_start[0] = SYNC_BYTE;
    p_start[1] = (pid >> 8);
    p_start[2] = pid;
    p_start[3] = total_cnt;
    p_start[4] = loopi;
    p_start[5] = rec_info_type;
    p_start[6] = 'r';
    p_start[7] = ((loopi == total_cnt) ? payload : 180);

    memcpy(p_start + 8, p_info + (loopi - 1) * 180, p_start[7]);
    p_start += 188;
  }

  return total_cnt * 188;
}

BOOL unpacket_rec_info(u8 *p_packet, u8 *p_rec_info, u32 *p_payload)
{
  u8 payload = 0;

  payload = p_packet[7];

  memcpy(p_rec_info, p_packet + 8, payload);

  if (p_payload)
  {
    *p_payload = payload;
  }
  return (p_packet[3] == p_packet[4]);
}

u32 parse_stream(u16 *p_file_name, u16 *p_a_pid, u16 *p_v_pid, u16 *p_pcr_pid)
{
  hfile_t file;
  u32 total_read = 0;
  u32 read_size = 0;
  u8 *p_data = NULL;
  u8 *p_packet = NULL;
  u16 pid = 0;
  u8 stream_type = 0;
  u16 *p_svc_id = 0;
  u16 svc_cnt = 0;
  u32 read_max = 3;
  u32 offset = 0;
  u16 v_pid = 0;
  u16 a_pid = 0;
  u16 pcr_pid = 0;
  u32 loopi = 0;

  file = vfs_open(p_file_name, VFS_READ);
  if (file == NULL)
  {
    return 0;
  }

  p_data = mtos_malloc(PARSE_STREAM_LEN);
  MT_ASSERT(p_data != NULL);

  offset = ts_packet_head_offset(file);
  vfs_seek(file, offset, VFS_SEEK_SET);

  while (total_read < read_max)
  {
    memset(p_data, 0, PARSE_STREAM_LEN);
    read_size = vfs_read(p_data, PARSE_STREAM_LEN, 1, file);
    for (loopi = 0; loopi < (read_size / 188); loopi ++)
    {
      p_packet = p_data + loopi * 188;
      pid = TS_PID(p_packet);

      if (pid == 0 && svc_cnt == 0)
      {
        p_svc_id = parse_pat_table(p_packet, &svc_cnt);
        read_max = 6;
      }

      stream_type = pes_stream_type(p_packet);
      switch (stream_type)
      {
      case 1:
        if (!a_pid)
        {
          a_pid = pid;
        }
        break;

      case 2:
        if (!v_pid)
        {
          v_pid = pid;
        }
        if (!pcr_pid && is_pcr_packet(p_packet))
        {
          pcr_pid = pid;
        }
        break;

      default:
        if (!pcr_pid && is_pcr_packet(p_packet))
        {
          pcr_pid = pid;
        }
        break;
      }

      if (v_pid && a_pid && pcr_pid)
      {
        break;
      }

      if (p_svc_id != NULL && match_svc_id(p_svc_id, svc_cnt, pid) == 1)
      {
        *p_a_pid = 0;
        *p_v_pid = 0;
        parse_pmt_table(p_packet, p_a_pid, p_v_pid, p_pcr_pid);
        if (*p_v_pid != 0)
        {
          break;
        }
      }
    }

    if (loopi < (read_size / 188))
    {
      break;
    }
    total_read ++;
  }

  if (p_data != NULL)
  {
    mtos_free(p_data);
    p_data = NULL;
  }

  if (p_svc_id != NULL)
  {
    mtos_free(p_svc_id);
    p_svc_id = NULL;
  }

  vfs_seek(file, 0, VFS_SEEK_SET);
  vfs_close(file);

  if (*p_v_pid == 0)
  {
    *p_v_pid = v_pid;
    *p_a_pid = a_pid;
    *p_pcr_pid = pcr_pid;
  }

  OS_PRINTF("\n##debug: a[0x%x] v[0x%x] pcr[0x%x]\n", *p_a_pid, *p_v_pid, *p_pcr_pid);
  return (*p_a_pid + *p_v_pid + *p_pcr_pid);
}

BOOL parse_record_info(rec_info_t *p_rec_info, u16 *p_name)
{
  u16 idx_file_name[MAX_FILE_PATH] = {0};

  get_idx_info_file(p_name, idx_file_name, MAX_FILE_PATH);
  
  return parse_record_info_idx(p_rec_info, idx_file_name);
}

BOOL parse_record_info_idx(rec_info_t *p_rec_info, u16 *p_idx_name)
{
  hfile_t file;
  vfs_file_info_t file_info = {0};
  dynamic_rec_info_t dy_rec_info = {0};
#ifndef WIN32
  __attribute__ ((aligned (16)))u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#else
  u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#endif
  idx_extern_info_t *p_extern_info = (idx_extern_info_t *)extern_info;
  static_rec_info_t st_rec_info;
  static_rec_info_t *p_st_rec_info = NULL;
  u32 start_offset = IDX_INFO_OFFSET_NEW;
  u8 unit_size = 0;
  u32 read_size = 0;
  u16 loopi = 0;
  BOOL b_get_pid = FALSE;

  MT_ASSERT(p_idx_name != NULL);
  MT_ASSERT(p_rec_info != NULL);

  memset(p_rec_info, 0, sizeof(rec_info_t));

  MT_ASSERT(sizeof(idx_extern_info_t) == IDX_INFO_OFFSET_NEW);

  file = vfs_open(p_idx_name, VFS_READ);
  if (file == NULL)
  {
    return FALSE;
  }

  vfs_get_file_info(file, &file_info);
  p_rec_info->total_size = file_info.file_size;
  if (!p_rec_info->total_size)
  {
    vfs_close(file);
    return FALSE;
  }
  
  read_size = vfs_read(extern_info, IDX_INFO_OFFSET_NEW, 1, file);
  
  if (read_size < IDX_INFO_OFFSET)
  {
    vfs_close(file);
    return FALSE;
  }
  OS_PRINTF("\n##parse_record_info_idx version [%lu]\n", p_extern_info->version_number);
  if (p_extern_info->version_number == IDX_INFO_VERSION)
  {
    p_st_rec_info = (static_rec_info_t *)&p_extern_info->st_rec_info;
    p_rec_info->lock_flag = p_extern_info->st_rec_info.lock_flag;
    p_rec_info->ext_pid_cnt = p_extern_info->extern_num;
    for (loopi = 0; loopi < p_rec_info->ext_pid_cnt; loopi ++)
    {
      if (loopi >= REC_MAX_EXTERN_PID)
      {
        break;
      }
      p_rec_info->ext_pid[loopi].pid = p_extern_info->extern_pid[loopi].pid;
      p_rec_info->ext_pid[loopi].type = p_extern_info->extern_pid[loopi].type;
      p_rec_info->ext_pid[loopi].fmt = p_extern_info->extern_pid[loopi].fmt;
    }

    if (p_extern_info->v_pid
      && p_extern_info->a_pid
      && p_extern_info->pcr_pid)
    {
      p_rec_info->v_pid = p_extern_info->v_pid;
      p_rec_info->a_pid = p_extern_info->a_pid;
      p_rec_info->pcr_pid = p_extern_info->pcr_pid;
      p_rec_info->video_fmt = p_extern_info->video_fmt;
      p_rec_info->audio_fmt = p_extern_info->audio_fmt;
      b_get_pid = TRUE;
    }
  }
  else
  {
    start_offset = IDX_INFO_OFFSET;
    memcpy(&st_rec_info, &extern_info[1], sizeof(static_rec_info_t));
    p_st_rec_info = &st_rec_info;
    p_rec_info->lock_flag = extern_info[IDX_INFO_LOCK_OFFSET];
  }
  memcpy(p_rec_info->user_data, p_extern_info->user_data, IDX_INFO_USER_DATA_LENGTH);
  
  vfs_seek(file, start_offset, VFS_SEEK_SET);
  vfs_read(&unit_size, 1, 1, file);
  
  memcpy(p_rec_info->program, p_st_rec_info->program, sizeof(u16) * PROGRAM_NAME_MAX);
  memcpy(&p_rec_info->start, &p_st_rec_info->start, sizeof(utc_time_t));
  p_rec_info->encrypt_flag = p_st_rec_info->encrypt_flag;
  p_rec_info->pmt_pid = p_st_rec_info->pmt_pid;
  p_rec_info->total_time = 0;
  
  vfs_seek(file, p_rec_info->total_size - unit_size, VFS_SEEK_SET);
  read_size = vfs_read(&dy_rec_info, unit_size, 1, file);
  if (read_size != unit_size)
  {
    vfs_close(file);
    return b_get_pid;
  }

  p_rec_info->total_time = dy_rec_info.cur_time / 1000;
  if (!b_get_pid)
  {
    p_rec_info->v_pid = dy_rec_info.v_pid;
    p_rec_info->a_pid = dy_rec_info.a_pid;
    p_rec_info->pcr_pid = dy_rec_info.pcr_pid;
    p_rec_info->video_fmt = dy_rec_info.video_fmt;
    p_rec_info->audio_fmt = dy_rec_info.audio_fmt;
    b_get_pid = TRUE;
  }

  vfs_close(file);
  return b_get_pid;
}

u8 record_info_idx_get_lock_flag(u16 *p_idx_name)
{
  hfile_t file;
  u8 lock_flag = 0;
  u32 read_size = 0;
#ifndef WIN32
  __attribute__ ((aligned (16)))u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#else
  u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#endif
  idx_extern_info_t *p_extern_info = (idx_extern_info_t *)extern_info;

  MT_ASSERT(p_idx_name != NULL);

  file = vfs_open(p_idx_name, VFS_READ);
  if (file == NULL)
  {
    return 0;
  }

  read_size = vfs_read(extern_info, IDX_INFO_OFFSET_NEW, 1, file);
  
  if (read_size < IDX_INFO_OFFSET)
  {
    vfs_close(file);
    return 0;
  }

  if (p_extern_info->version_number != IDX_INFO_VERSION)
  {
    lock_flag = extern_info[IDX_INFO_LOCK_OFFSET];
  }
  else
  {
    lock_flag = p_extern_info->st_rec_info.lock_flag;
  }

  vfs_close(file);
  return lock_flag;
}

void record_info_idx_change_lock_flag(u16 *p_idx_name)
{
  hfile_t file;
  u32 read_size = 0;
#ifndef WIN32
  __attribute__ ((aligned (16)))u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#else
  u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#endif
  idx_extern_info_t *p_extern_info = (idx_extern_info_t *)extern_info;

  MT_ASSERT(p_idx_name != NULL);

  file = vfs_open(p_idx_name, VFS_READ | VFS_WRITE);
  if (file == NULL)
  {
    return ;
  }

  read_size = vfs_read(extern_info, IDX_INFO_OFFSET_NEW, 1, file);
  
  if (read_size < IDX_INFO_OFFSET)
  {
    vfs_close(file);
    return ;
  }
  
  if (p_extern_info->version_number != IDX_INFO_VERSION)
  {
    extern_info[IDX_INFO_LOCK_OFFSET] = !extern_info[IDX_INFO_LOCK_OFFSET];
  }
  else
  {
    p_extern_info->st_rec_info.lock_flag = !p_extern_info->st_rec_info.lock_flag;
  }
  
  vfs_seek(file, 0, VFS_SEEK_SET);
  vfs_write(extern_info, 1, IDX_INFO_OFFSET_NEW, file);
  vfs_close(file);
}

u32 parse_mp3_time_long(u16 *p_name)
{
  u32 time_long = 0;
  mpegaudioframeinfo_t *p_mp3_info = NULL;

  p_mp3_info = mtos_malloc(sizeof(mpegaudioframeinfo_t));
  MT_ASSERT(p_mp3_info != NULL);

  memset(p_mp3_info,0,sizeof(mpegaudioframeinfo_t));

  mp3_parse_info(p_name, p_mp3_info);
  if (p_mp3_info->is_mp3file)
  {
    OS_PRINTF("\n##debug: mp3[%s] time long[%d]\n", p_name, p_mp3_info->mDuration);
    time_long = p_mp3_info->mDuration;
  }
  else
    OS_PRINTF("this file is not an mp3 file!!!\n");

  if(p_mp3_info->p_pic_buf != NULL)
  {
     mp3_frame_free(p_mp3_info->p_pic_buf);
     p_mp3_info->p_pic_buf = NULL;
  }

  mtos_free(p_mp3_info);
  p_mp3_info = NULL;
  return time_long;
}

static void _flush_idx_file(media_idx_t *p_array)
{
  u32 offset = 0;
  
  if((p_array == NULL) || (p_array->p_buffer == NULL)
    ||(p_array->file == NULL) || (p_array->flush_cnt >= p_array->get_cnt))
  {
    return;
  }
  
  offset = p_array->flush_cnt % p_array->unit_total;
  OS_PRINTF("\n##_flush_idx_file[%lu, %lu]!\n",
    offset, p_array->get_cnt - p_array->flush_cnt);
  vfs_write((void *)((u32)p_array->p_buffer + offset * p_array->unit_size),
    p_array->unit_size, p_array->get_cnt - p_array->flush_cnt, p_array->file);
  vfs_flush(p_array->file);
  p_array->flush_cnt = p_array->get_cnt;
}

static BOOL _load_idx_file(media_idx_t *p_array, u32 start, BOOL first_load)
{
  u8 *p_file_buf = NULL;
  //fd == file data, and the new version must > older version's
  u32 fd_read_len = p_array->unit_total * p_array->unit_size;
  u32 fd_len = 0;
  u32 fd_unit_size = 0;
  u32 p = 0;
  u32 load_cnt = 0;
  u32 unit_total = p_array->unit_total;

  if(p_array->file == NULL) //no file opend
  {
    return FALSE;
  }

  p_file_buf = mtos_malloc(fd_read_len);
  if(p_file_buf == NULL)
  {
    return FALSE;
  }

  if(!first_load)
  {
    vfs_seek(p_array->file,
      p_array->start_offset + start * p_array->unit_size, VFS_SEEK_SET);
    //OS_PRINTF("%s seek %d\n",
    //  __FUNCTION__, p_array->start_offset + start * p_array->unit_size);
  }

  //load data
  fd_len = vfs_read(p_file_buf, 1, fd_read_len, p_array->file);

  if(fd_len == 0)
  {
    OS_PRINTF("%s no data \n", __FUNCTION__);
    mtos_free(p_file_buf);
    p_array->file_end_index = p_array->file_data_last_read_index + load_cnt - 1;
    return FALSE;
  }

  if(fd_len > 0)  //the firset read is valid, do it
  {
    fd_unit_size = p_file_buf[0];  //unit_len addr
    if(first_load)
    {
      //p_array->file_data_unit_size = fd_unit_size;
    }

    while(fd_len >= fd_unit_size) //more than one item
    {
      if(load_cnt >= unit_total)  //fill full
      {
        break;
      }
      //must makesure the new version data size > older version's
      memcpy(p_array->p_buffer + load_cnt, p_file_buf + p , fd_unit_size);
      load_cnt++;
      fd_len -= fd_unit_size;
      p += fd_unit_size;
    }
  }

  p_array->get_cnt = load_cnt;
  p_array->file_data_last_read_index = start;

  if (p_array->get_cnt < p_array->unit_total)
  {
    p_array->file_end_index = p_array->file_data_last_read_index + load_cnt - 1;
  }
  mtos_free(p_file_buf);

  return TRUE;
}

static BOOL _media_idx_init(media_idx_t **pp_array,
  u32 unit_total, u16 *p_name, u32 open_mode, u8 *p_ridx_buf, u32 ridx_buf_size)
{
  media_idx_t *p_array = NULL;
  u8 unit_size = sizeof(dynamic_rec_info_t);
  u32 array_len = 0;
  idx_extern_info_t extern_info;
  u32 read_size = 0;
  u16 start_offset = IDX_INFO_OFFSET_NEW;
  void *p_file = NULL;
  u8 is_malloc = 0;

  if(unit_total == 0)
  {
    return FALSE;
  }

  MT_ASSERT(sizeof(idx_extern_info_t) == IDX_INFO_OFFSET_NEW);
  
  if(p_name != NULL)
  {
    p_file = vfs_open(p_name,  open_mode);
    if(p_file == NULL)
    {
      return FALSE;
    }
  }

  memset(&extern_info, 0, sizeof(idx_extern_info_t));
  if (open_mode == VFS_READ)
  {
    read_size = vfs_read((void *)&extern_info, IDX_INFO_OFFSET_NEW, 1, p_file);
    if (read_size == IDX_INFO_OFFSET_NEW
      && extern_info.version_number == IDX_INFO_VERSION)
    {
      vfs_seek(p_file, IDX_INFO_OFFSET_NEW, VFS_SEEK_SET);
      OS_PRINTF("\n##debug: new idx info!!\n");
    }
    else
    {
      start_offset = IDX_INFO_OFFSET;
      vfs_seek(p_file, IDX_INFO_OFFSET, VFS_SEEK_SET);
      OS_PRINTF("\n##debug: old idx info!!\n");
    }
    vfs_read((void *)&unit_size, 1, 1, p_file);
    OS_PRINTF("\n##media_idx unit_size [%d]\n", unit_size);
    vfs_seek(p_file, start_offset, VFS_SEEK_SET);
  }
  
  array_len = sizeof(dynamic_rec_info_t) * unit_total + sizeof(media_idx_t);

  if ((ridx_buf_size >= array_len) && p_ridx_buf)
  {
    p_array = (media_idx_t *)p_ridx_buf;
    is_malloc = 0;
    OS_PRINTF("\n##ridx info use cfg!");
  }
  else
  {
    p_array = (media_idx_t *)mtos_malloc(array_len);
    is_malloc = 1;
    OS_PRINTF("\n##ridx info use malloc [%x]!", p_array);
  }
  
  if(p_array == NULL)
  {
    OS_PRINTF("\n##_media_idx_init malloc[%lu] fail!!!\n", array_len);
    vfs_close(p_file);
    return FALSE;
  }
  memset(p_array, 0, array_len);

  p_array->is_malloc = is_malloc;
  p_array->file = p_file;
  p_array->start_offset = start_offset;
  if (p_array->file)
  {
    if (open_mode == VFS_READ)
    {
      p_array->open_mode = MEDIA_IDX_READ;
    }
    else
    {
      p_array->open_mode = MEDIA_IDX_WRITE;
    }
  }
  else
  {
    p_array->open_mode = MEDIA_IDX_TS;
  }

  if (!mtos_sem_create(&p_array->sem, 1))
  {
    mtos_free(p_array);
    vfs_close(p_file);
    return FALSE;
  }
  p_array->p_buffer = (dynamic_rec_info_t *)((u32)p_array + sizeof(media_idx_t));
  p_array->unit_size = unit_size;
  p_array->unit_total = unit_total;
  p_array->get_cnt = 0;
  p_array->file_end_index = 0;
  p_array->file_data_last_read_index = 0;
  memcpy(&(p_array->extern_info), &extern_info, sizeof(idx_extern_info_t));
  (*pp_array) = p_array;
  
  return TRUE;
}

BOOL media_idx_create(media_idx_t **pp_array,
  u32 unit_total, u16 *p_name, u8 *p_ridx_buf, u32 ridx_buf_size)
{
  return _media_idx_init(pp_array,
    unit_total, p_name, VFS_NEW, p_ridx_buf, ridx_buf_size);
}

BOOL media_idx_load(media_idx_t **pp_array,
  u32 unit_total, u16 *p_name, u8 *p_ridx_buf, u32 ridx_buf_size)
{
  BOOL ret = FALSE;

  if(p_name == NULL)  //must have file
  {
    return FALSE;
  }

  ret = _media_idx_init(pp_array,
    unit_total, p_name, VFS_READ, p_ridx_buf, ridx_buf_size);

  if(ret) //init success
  {
    //load file data
    _load_idx_file(*pp_array, 0, TRUE);
  }

  return ret;
}

void media_idx_destory(media_idx_t *p_array)
{
  if (p_array == NULL)
  {
    return ;
  }
  
  if(p_array->file != NULL)
  {
    //flush it
    if (p_array->open_mode == MEDIA_IDX_WRITE)
    {
      _flush_idx_file(p_array);
    }
    vfs_close(p_array->file);
    p_array->file = NULL;
  }
  if(p_array->sem)
  {
    mtos_sem_destroy(&p_array->sem, 0);
    p_array->sem = 0;
  }

  if(p_array != NULL && p_array->is_malloc)
  {
    mtos_free(p_array);
    p_array = NULL;
  }
}

dynamic_rec_info_t *media_idx_read_by_index(media_idx_t *p_array, u32 index)
{
  dynamic_rec_info_t *p_info = NULL;

  if(p_array == NULL)
  {
    return NULL;
  }
  else if(p_array->file == NULL)  //timeshift mode
  {
    return p_array->p_buffer + (index % p_array->unit_total);
  }
  else
  {
    p_info = p_array->p_buffer;
    //check out of range
    if((index < p_info[0].indx) || (index > p_info[p_array->get_cnt - 1].indx))
    {
      if (p_array->file_end_index
        && (index > p_array->file_end_index))//load end 
      {
        return p_info + (p_array->get_cnt - 1);
      }
      
      if(media_idx_reload(p_array, index))
      {
        if (index < p_info[0].indx)
        {
          return NULL;
        }
        return p_info + (index - p_info[0].indx);
      }
      else
      {
        return NULL;
      }
    }
    else
    {
      if (index < p_info[0].indx)
      {
        return NULL;
      }
      return p_info + (index - p_info[0].indx);
    }
  }
}

//return -2;  //less than min
//return -1;  //equal min
//return 0;  //between min and max
//return 1;  //equal max
//return 2;  //more than max
static int _media_idx_check_range(dynamic_rec_info_t *p_min,
              dynamic_rec_info_t *p_max, BOOL by_ms, u32 v)
{
  u64 min_v = by_ms ? p_min->cur_time : p_min->pts;
  u64 max_v = by_ms ? p_max->cur_time : p_max->pts;
  u64 c_v = v;
  int ret = 0;

  if((p_max->indx > p_min->indx)
    && (max_v < min_v)
    && ((min_v - max_v) > 0x8FFFFFFF))  //the data was ring
  {
    /*OS_PRINTF("%s overwrite : max_v[%lld], min_v[%lld], v[%lld]\n",
              __FUNCTION__, max_v, min_v, c_v);*/
    max_v += 0xFFFFFFFF;  //max of u32

    if(c_v < min_v) //if the max don't ring ,the v can't ring
    {
      c_v += 0xFFFFFFFF;
    }
  }

  if ((c_v > min_v) && (c_v < max_v))
  {
    if ((c_v - min_v) > (max_v - c_v))
    {
      ret = 1;
    }
    else
    {
      ret = -1;
    }
  }
  else if((c_v == min_v)
    || (!by_ms && (c_v > min_v) && (min_v > max_v)
      && ((min_v - max_v) > RING_STREAM_PTS_OFFSET_MIN))
    || (!by_ms && (c_v < min_v) && (min_v < max_v) && ((min_v - c_v) < 4500)))
  {
    ret = -1;
  }
  else if((c_v == max_v)
    || (!by_ms && (c_v > max_v) && (c_v > min_v) && ((c_v - max_v) < 4500))
    || (!by_ms && (c_v < min_v) && (min_v > max_v) && (c_v < max_v)
      && ((min_v - max_v) > RING_STREAM_PTS_OFFSET_MIN)))
  {
    ret = 1;
  }
  else if (c_v < min_v)
  {
    ret = -2;
  }
  else
  {
    ret = 2;
  }
  
  return ret;
}

//return -1;  //less than min
//return 0;  //between min and max
//return 1;  //more than max
static int _media_idx_check_full_buffer_range(media_idx_t *p_array, BOOL by_ms, u32 v,
    dynamic_rec_info_t **pp_min,  dynamic_rec_info_t **pp_max)
{
  dynamic_rec_info_t *p_info = NULL;
  int total = 0;
  int min = 0;
  int max = 0;
  BOOL is_timeshift_mode = FALSE;
  int cmp = 0;

  if(p_array == NULL)
  {
    return -1;
  }

  p_info = p_array->p_buffer;
  is_timeshift_mode = (p_array->file == NULL);

  if(!is_timeshift_mode) //file mode
  {
    total = (int)p_array->get_cnt;
    min = 0;
    max = total - 1;
  }
  else  //timeshift mode
  {
    if(p_array->get_cnt > p_array->unit_total)  //the fifo overwrite
    {
      total = (int)p_array->unit_total;
      max = (p_array->get_cnt - 1) % total;
      min = (max + 1) % total;  //the min is follow the max
    }
    else
    {
      total = (int)p_array->get_cnt;
      min = 0;
      max = total - 1;
    }
  }

  if(pp_min != NULL)
  {
    *pp_min = p_info + min;
  }

  if(pp_max != NULL)
  {
    *pp_max = p_info + max;
  }

  cmp = _media_idx_check_range(p_info + min, p_info + max, by_ms, v);

  if(cmp == -2) //less than min
  {
    return -1;
  }
  else if(cmp == 2) //more than max
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static dynamic_rec_info_t *_media_idx_found_by_spec(media_idx_t *p_array, BOOL by_ms, u32 v)
{
  //#define _MI_FOUND_DBG
  dynamic_rec_info_t *p_info = NULL;
  int i = 0;
  int total = 0;
  int dir_step = 1; //default to end
  int cur = 0;
  int next = 0;
  int min = 0;
  int max = 0;
  dynamic_rec_info_t *p_found = NULL;
  BOOL is_timeshift_mode = FALSE;
  int cmp_ret = 0;
  int cmp_ret2 = 0;
  dynamic_rec_info_t *p_min = NULL;
  dynamic_rec_info_t *p_max = NULL;

  if(p_array == NULL)
  {
    return NULL;
  }

  p_info = p_array->p_buffer;
  cur = (int)p_array->last_query;
  is_timeshift_mode = (p_array->file == NULL);

  if(!is_timeshift_mode) //file mode
  {
    total = (int)p_array->get_cnt;
    min = 0;
    max = total - 1;
  }
  else  //timeshift mode
  {
    if(p_array->get_cnt > p_array->unit_total)  //the fifo overwrite
    {
      total = (int)p_array->unit_total;
      max = (p_array->get_cnt - 1) % total;
      min = (max + 1) % total;  //the min is follow the max
    }
    else
    {
      total = (int)p_array->get_cnt;
      min = 0;
      max = total - 1;
    }
  }

#ifdef _MI_FOUND_DBG //for debug
  OS_PRINTF("%s show: ts_mode[%d], total[%d], v[0x%x], ",
              __FUNCTION__, is_timeshift_mode, total, v);
  if(by_ms)
  {
    OS_PRINTF("min[%d - 0x%x], max[%d - 0x%x], cur[%d - 0x%x]--ms\n",
      min, p_info[min].cur_time, max, p_info[max].cur_time, cur, p_info[cur].cur_time);
  }
  else
  {
    OS_PRINTF("min[%d - 0x%x], max[%d - 0x%x], cur[%d - 0x%x]--pts\n",
      min, p_info[min].pts, max, p_info[max].pts, cur, p_info[cur].pts);
  }
#endif

  cmp_ret = _media_idx_check_full_buffer_range(p_array, by_ms, v, &p_min, &p_max);
  if(cmp_ret != 0)  //out of range
  {
    if(is_timeshift_mode)
    {
      if(cmp_ret == -1) //less than min
      {
        #ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("%s %d: out of range get min \n", __FUNCTION__, __LINE__);
        #endif
        return p_info + min;
      }
      else  //more than max
      {
        #ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("%s %d: out of range get max \n", __FUNCTION__, __LINE__);
        #endif
        return p_info + max;
      }
    }
    else
    {
      #ifdef _MI_FOUND_DBG //for debug
      OS_PRINTF("%s %d: out of range no found \n", __FUNCTION__, __LINE__);
      #endif
      return NULL;
    }
  }

  /*
  //check direction
  if(_media_idx_check_range(p_info + cur, p_info + max, by_ms, v) == -2) //less than cur
  {
    dir_step = -1;
  }
  */

  //go back 50 time
  if(cur > 50)
  {
    cur -= 50;
  }
  else
  {
    cur = 0;
  }
  
  while(i < total)  //must found total time
  {
    //get next
    next = cur + dir_step;
    if(next < 0)
    {
      if(1)//is_timeshift_mode)
      {
        next = (total - 1);//around to end
      }
      else
      {
        #ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("%s %d: found head \n", __FUNCTION__, __LINE__);
        #endif
        p_array->last_query = 0;
        p_found = p_info;
        break;
      }
    }
    else if(next >= total)
    {
      if(1)//is_timeshift_mode)
      {
        next = 0; //around to head
      }
      else
      {
        #ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("%s %d: found end \n", __FUNCTION__, __LINE__);
        #endif
        p_array->last_query = total - 1;
        p_found = p_info + total - 1;
        break;
      }
    }

    if(dir_step == 1)  //next > cur
    {
      cmp_ret2 = _media_idx_check_range(p_info + cur, p_info + next, by_ms, v);

      if((cmp_ret2 == -1) || (cmp_ret2 == 0))
      {
        p_found = p_info + cur;
      }
      else if(cmp_ret2 == 1)
      {
        p_found = p_info + next;
      }
    }
    else
    {
      cmp_ret2 = _media_idx_check_range(p_info + next, p_info + cur, by_ms, v);
      if((cmp_ret2 == 1) || (cmp_ret2 == 0))
      {
        p_found = p_info + cur;
      }
      else if(cmp_ret2 == -1)
      {
        p_found = p_info + next;
      }
    }

    if(p_found != NULL)
    {
      #ifdef _MI_FOUND_DBG //for debug
      OS_PRINTF("%s %d: found time %d, pts 0x%x, between[%d %d]\n",
        __FUNCTION__, __LINE__, p_found->cur_time, p_found->pts, cur, next);
      #endif
      p_array->last_query = p_found->indx % p_array->unit_total;
      break;
    }

    cur = next;
    i++;
  }

#ifdef _MI_FOUND_DBG //for debug
  OS_PRINTF("%s %d: end found [0%x]\n", __FUNCTION__, __LINE__, (u32)p_found);
#endif
  return p_found;
}

static BOOL _media_idx_reload(media_idx_t *p_array, u32 start_index)
{
  BOOL ret = FALSE;
  u32 _start = 0;
  u32 _end = 0;
  u32 new_set = 0;

  if((p_array == NULL) || (p_array->file == NULL)) //no file opend
  {
    return FALSE;
  }

  _start = p_array->file_data_last_read_index;
  _end = _start + p_array->get_cnt;

  if((_start <= start_index) && (start_index < _end))  //it's exsit
  {
    return TRUE;
  }
  else  //set new start to middle
  {
    if(start_index > (p_array->unit_total / 2))
    {
      new_set = start_index - (p_array->unit_total / 2);
    }
    else
    {
      new_set = 0;
    }
  }

  ret = _load_idx_file(p_array, new_set, FALSE);
  if(ret)
  {
    p_array->last_query = start_index;
  }
  return TRUE;
}

static dynamic_rec_info_t *_media_idx_found_by_spec_ex(media_idx_t *p_array,
  BOOL by_ms, u32 v, u32 start_index, BOOL b_forward)
{
  //#define _MI_FOUND_DBG
  dynamic_rec_info_t *p_info = NULL;
  int i = 0;
  int total = 0;
  int dir_step = 1; //default to end
  int cur = 0;
  int next = 0;
  int min = 0;
  int max = 0;
  dynamic_rec_info_t *p_found = NULL;
  BOOL is_timeshift_mode = FALSE;
  int cmp_ret2 = 0;

  if(p_array == NULL)
  {
    return NULL;
  }
  
  p_info = p_array->p_buffer;
  cur = (int)((start_index - p_array->file_data_last_read_index) % p_array->unit_total);
  is_timeshift_mode = (p_array->file == NULL);

  if(!is_timeshift_mode) //file mode
  {
    total = (int)p_array->get_cnt;
    min = 0;
    max = total - 1;
  }
  else  //timeshift mode
  {
    if(p_array->get_cnt > p_array->unit_total)  //the fifo overwrite
    {
      total = (int)p_array->unit_total;
      max = (p_array->get_cnt - 1) % total;
      min = (max + 1) % total;  //the min is follow the max
    }
    else
    {
      total = (int)p_array->get_cnt;
      min = 0;
      max = total - 1;
    }
    
    if ((cur >= total) && !b_forward)
    {
      cur = max;
    }
  }

  if (!b_forward)
  {
    dir_step = -1;
  }

  while(i < total)  //must found total time
  {
    //get next
    next = cur + dir_step;
    if(next < 0)
    {
      if(is_timeshift_mode)
      {
        next = (total - 1);//around to end
      }
      else
      {
        /*#ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("\n##%s %d: found head \n", __FUNCTION__, __LINE__);
        #endif
        p_array->last_query = 0;
        p_found = p_info;
        break;*/
        return NULL;
      }
    }
    else if(next >= total)
    {
      if(is_timeshift_mode)
      {
        next = 0; //around to head
      }
      else
      {
        /*#ifdef _MI_FOUND_DBG //for debug
        OS_PRINTF("\n##%s %d: found end \n", __FUNCTION__, __LINE__);
        #endif
        p_array->last_query = total - 1;
        p_found = p_info + total - 1;
        break;*/
        return NULL;
      }
    }

    if(dir_step == 1)  //next > cur
    {
      cmp_ret2 = _media_idx_check_range(p_info + cur, p_info + next, by_ms, v);

      if((cmp_ret2 == -1) || (cmp_ret2 == 0))
      {
        p_found = p_info + cur;
      }
      else if(cmp_ret2 == 1)
      {
        p_found = p_info + next;
      }
    }
    else
    {
      cmp_ret2 = _media_idx_check_range(p_info + next, p_info + cur, by_ms, v);
      if((cmp_ret2 == 1) || (cmp_ret2 == 0))
      {
        p_found = p_info + cur;
      }
      else if(cmp_ret2 == -1)
      {
        p_found = p_info + next;
      }
      else if (cmp_ret2 == -2 && next == min && !by_ms)
      {
        p_found = p_info + next;
      }
    }

    if(p_found != NULL)
    {
      #ifdef _MI_FOUND_DBG //for debug
      OS_PRINTF("%s %d: found time %d, pts 0x%x, between[%d %d]\n",
        __FUNCTION__, __LINE__, p_found->cur_time, p_found->pts, cur, next);
      #endif
      p_array->last_query = p_found->indx % p_array->unit_total;
      break;
    }

    cur = next;
    i++;
  }

#ifdef _MI_FOUND_DBG //for debug
  OS_PRINTF("%s %d: end found [0%x]\n", __FUNCTION__, __LINE__, (u32)p_found);
#endif
  return p_found;
}

dynamic_rec_info_t *media_idx_found_by_pts_ex(media_idx_t *p_array,
  u32 pts, u32 start_index, BOOL b_forward)
{
  dynamic_rec_info_t *p_info = NULL;
  u32 serach_start = 0;
  int cur = 0;
  int total = 0;
  int min = 0;
  int max = 0;
  
  if (p_array == NULL)
  {
    return NULL;
  }
  
  mtos_sem_take(&p_array->sem, 0);

  if (p_array->file == NULL)
  {
    if(p_array->get_cnt > p_array->unit_total)  //the fifo overwrite
    {
      total = (int)p_array->unit_total;
      max = (p_array->get_cnt - 1) % total;
      min = (max + 1) % total;  //the min is follow the max
    }
    else
    {
      total = (int)p_array->get_cnt;
      min = 0;
      max = total - 1;
    }
    
    if (b_forward)
    {
      if (start_index >= 1)
      {
        if (start_index >= min
          && ((start_index - 1) < min) && (((max + 1) % p_array->unit_total) == min))
        {
          serach_start = min;
        }
        else
        {
          serach_start = start_index - 1;
        }
      }
      else
      {
        if (start_index >= min)
        {
          serach_start = min;
        }
        else
        {
          serach_start = 0;
        }
      }
    }
    else
    {
      if (start_index <= max && ((start_index + 1) > max))
      {
        serach_start = max;
      }
      else
      {
        serach_start = start_index + 1;
      }
    }
  }
  else
  {
    if (b_forward)
    {
      if (start_index > 1)
      {
        serach_start = start_index - 1;
      }
      else
      {
        serach_start = 0;
      }
    }
    else
    {
      serach_start = start_index + 1;
    }
  }

  _media_idx_reload(p_array, serach_start);

  if (!b_forward
    && (serach_start >= (p_array->file_data_last_read_index + p_array->get_cnt)))
  {
    serach_start = (p_array->file_data_last_read_index + p_array->get_cnt) - 1;
  }
  
  p_info = _media_idx_found_by_spec_ex(p_array,
    FALSE, pts, serach_start, b_forward);

  if (p_info == NULL)
  {
    cur = (int)((start_index - p_array->file_data_last_read_index) % p_array->unit_total);
    p_info = (dynamic_rec_info_t *)&(p_array->p_buffer[cur]);
  }
  mtos_sem_give(&p_array->sem);
  
  return p_info;
}

dynamic_rec_info_t *media_idx_found_by_pts(media_idx_t *p_array, u32 pts)
{
  dynamic_rec_info_t *p_info = NULL;

  if (p_array == NULL)
  {
    return NULL;
  }
  
  mtos_sem_take(&p_array->sem, 0);
  p_info = _media_idx_found_by_spec(p_array, FALSE, pts);
  mtos_sem_give(&p_array->sem);
  return p_info;
}

dynamic_rec_info_t *_media_idx_found_by_ms(media_idx_t *p_array, u32 ms)
{
  dynamic_rec_info_t *p_min = NULL;
  dynamic_rec_info_t *p_max = NULL;
  u32 start = 0;
  int i = 0;
  
  if (p_array == NULL)
  {
    return NULL;
  }
  
  do
  {
    i = _media_idx_check_full_buffer_range(p_array, TRUE, ms, &p_min, &p_max);
    if(i == 0)
    {
      return _media_idx_found_by_spec(p_array, TRUE, ms);
    }
    else if(i < 0)  //outof range
    {
      if(p_array->file == NULL) //timeshift mode
      {
        return p_min;
      }
      else
      {
        if(p_array->file_data_last_read_index == 0) //the file's first item
        {
          return p_min;
        }
        else if(p_array->file_data_last_read_index < p_array->unit_total)
        {
          start = 0;
        }
        else
        {
          start = p_array->file_data_last_read_index - p_array->unit_total;
        }
        _load_idx_file(p_array, start, FALSE);
      }
    }
    else
    {
      if(p_array->file == NULL) //timeshift mode
      {
        return p_max;
      }
      else
      {
        start = p_array->file_data_last_read_index + p_array->unit_total;
        if(!_load_idx_file(p_array, start, FALSE))
        {
          OS_PRINTF("%s no data\n", __FUNCTION__);
          return p_max;
        }
      }
    }
  }while(1);
  
  //return NULL;
}

dynamic_rec_info_t *media_idx_found_by_ms(media_idx_t *p_array, u32 ms)
{
  dynamic_rec_info_t *p_info = NULL;

  if (p_array == NULL)
  {
    return NULL;
  }
  
  mtos_sem_take(&p_array->sem, 0);
  p_info = _media_idx_found_by_ms(p_array, ms);

  //it's hack
  if(p_info == NULL)  //no found
  {
    OS_PRINTF("%s error, no found\n", __FUNCTION__);
    p_info = p_array->p_buffer + p_array->last_query;
  }
  //end
  mtos_sem_give(&p_array->sem);
  return p_info;

}

s32 media_idx_check_index_range(media_idx_t *p_array, u32 index)
{
  s32 ret = 0;
  int total = 0;
  int min = 0;
  int max = 0;

  if (p_array == NULL)
  {
    return 0;
  }
  
  mtos_sem_take(&p_array->sem, 0);

  if (p_array->file == NULL)
  {
    if(p_array->get_cnt > p_array->unit_total)  //the fifo overwrite
    {
      total = (int)p_array->unit_total;
      max = (p_array->get_cnt - 1) % total;
      min = (max + 1) % total;  //the min is follow the max
      if (index == max)
      {
        ret = 1;
      }
      else if (index == min)
      {
        ret = -1;
      }
      else
      {
        ret = 0;
      }
    }
    else
    {
      total = (int)p_array->get_cnt;
      min = 0;
      max = total - 1;
      if (index >= max)
      {
        ret = 1 + (index - max);
      }
      else if (index == min)
      {
        ret = -1;
      }
      else
      {
        ret = 0;
      }
    }
  }
  else
  {
    if (p_array->file_end_index
      && index >= p_array->file_end_index)
    {
      ret = 1 + (index - p_array->file_end_index);
    }
    else
    {
      ret = 0;
    }
  }
  mtos_sem_give(&p_array->sem);

  return ret;
}

void media_idx_write_extern_info(media_idx_t *p_array, idx_extern_info_t *p_extern_info)
{
#ifndef WIN32
  __attribute__ ((aligned (16)))u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#else
  u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#endif

  if (p_array == NULL)
  {
    return ;
  }
  
  MT_ASSERT(p_extern_info != NULL);
  MT_ASSERT(sizeof(idx_extern_info_t) == IDX_INFO_OFFSET_NEW);

  p_extern_info->version_number = IDX_INFO_VERSION;
  if (p_array->file)
  {
    memcpy(&(p_array->extern_info), p_extern_info, IDX_INFO_OFFSET_NEW);
    memcpy(extern_info, p_extern_info, IDX_INFO_OFFSET_NEW);
    vfs_write(extern_info, IDX_INFO_OFFSET_NEW, 1, p_array->file);
    vfs_flush(p_array->file);
  }
}

u32 media_idx_write(media_idx_t *p_array, dynamic_rec_info_t *p_item)
{
  u32 p_info = 0;
  dynamic_rec_info_t *p_dy = NULL;

  if (p_array == NULL)
  {
    return 0;
  }
  
  mtos_sem_take(&p_array->sem, 0);

  if(p_array->file)
  {
    p_info = p_array->get_cnt % p_array->unit_total;
    p_dy = p_array->p_buffer + p_info;
    memcpy(p_dy, p_item, p_array->unit_size);
    p_dy->unit_len = sizeof(dynamic_rec_info_t);
    p_dy->indx = p_array->get_cnt;

    p_array->get_cnt++;

    if((p_array->get_cnt % p_array->unit_total) == 0) //full, flush it
    {
      _flush_idx_file(p_array);
    }

    mtos_sem_give(&p_array->sem);
    return p_array->get_cnt;
  }
  else
  {
    p_info = p_array->get_cnt % p_array->unit_total;
    p_dy = p_array->p_buffer + p_info;
    memcpy(p_array->p_buffer + p_info, p_item, p_array->unit_size);
    p_dy->unit_len = sizeof(dynamic_rec_info_t);
    p_dy->indx = p_array->get_cnt;

    p_array->get_cnt++;

    mtos_sem_give(&p_array->sem);
    return p_info;
  }
}

BOOL media_idx_reload(media_idx_t *p_array, u32 start_index)
{
  BOOL ret = FALSE;
  
  if (p_array == NULL)
  {
    return ret;
  }
  
  mtos_sem_take(&p_array->sem, 0);
  ret = _media_idx_reload(p_array, start_index);
  mtos_sem_give(&p_array->sem);
  
  return ret;
}

u32 media_idx_split_file(media_idx_t *p_array, u16 *p_name)
{
  u32 file_size = 0;
#ifndef WIN32
  __attribute__ ((aligned (16)))u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#else
  u8 extern_info[IDX_INFO_OFFSET_NEW] = {0};
#endif

  if (p_array == NULL)
  {
    return 0;
  }
  mtos_sem_take(&p_array->sem, 0);
    
  //close file
  if(p_array->file != NULL)
  {
    //flush it
    _flush_idx_file(p_array);
    file_size = vfs_tell(p_array->file);
    memcpy(extern_info, &(p_array->extern_info), IDX_INFO_OFFSET_NEW);
    vfs_close(p_array->file);
    p_array->file = NULL;
  }
  else
  {
    MT_ASSERT(0);
  }

  //new file
  if(p_name != NULL)
  {
    p_array->file = vfs_open(p_name,  VFS_NEW);
    if(p_array->file)
    {
      vfs_write((void *)extern_info, IDX_INFO_OFFSET_NEW, 1, p_array->file);
    }
    else
    {
      MT_ASSERT(0);
    }
  }

  //reset
  p_array->get_cnt = 0;
  p_array->flush_cnt = 0;
  p_array->last_query = 0;
  //p_array->file_data_unit_size = 0;
  p_array->file_data_last_read_index = 0;

  mtos_sem_give(&p_array->sem);
  
  return file_size;
}

void media_idx_flush(media_idx_t *p_array)
{
  if (p_array == NULL)
  {
    return ;
  }
  mtos_sem_take(&p_array->sem, 0);
  _flush_idx_file(p_array);
  mtos_sem_give(&p_array->sem);
}
//END OF FILE
