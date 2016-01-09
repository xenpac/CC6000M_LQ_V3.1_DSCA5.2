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
#include "pmt.h"
#include "err_check_def.h"

#include "iso_639_2.h"



typedef struct parse_desc_param_tag
{
  s16     es_info_len;
  s16     desc_len;
  s16     proc_len;
  u16     element_pid;
  pmt_t   *p_pmt;
  u8      *p_buf_data;
  service_t *p_svc;
} parse_desc_param_t;

typedef enum dump_pmt_desc_ret
{
  RET_NONE = 0,

  RET_VIDEO,

  RET_AUDIO,
}dump_pmt_desc_ret_t;


/*!
 parse tkgs component descriptor.. 
 */
u8 tkgs_parse_component(u8 *p_data)
{
  CHECK_FAIL_RET_ZERO(p_data != NULL);

  /*!
   *ASCII coding of "TKGS" string. 
   *The STB should ignore the descriptor if tkgs_identifier doesn't match this pattern.
   */
  if((p_data[2] == 'T') && (p_data[3] == 'K')
    && (p_data[4]) == 'G' && (p_data[5] == 'S'))
  {
   /*!
    *current_nex_indicator 0 : it indicates that the TKGS data sent is not yet applicable.  
    */
    if(((p_data[6] & 0x80) >> 7) == 1)
    {
      return 1;
    }
  }

  return 0;
}

static void dump_abs_upgrade_id_descriptior(service_t *p_svc, u16 pid,
  u8 *p_data, s32 data_len)
{
  os_msg_t msg = {0};
  u32 mem_size = 0;
  abs_software_upgrade_id_des_t des = {0};
  abs_software_upgrade_id_des_detail_t *p_loop = 
    (abs_software_upgrade_id_des_detail_t *)&p_data[3];
  des.pid = pid;

  //Check data length
  if(p_data[1] >= 4 * KBYTES)
  {
    return;
  }
  
  des.manufacture_id = p_data[2];
  des.count = (p_data[1]) / 7;
  mem_size = sizeof(abs_software_upgrade_id_des_detail_t) * des.count;

  if(mem_size < p_data[1] + 2) // less than descriptior length
  {
    u16 i = 0;
    des.p_detail = mtos_malloc(mem_size);
    CHECK_FAIL_RET_VOID(des.p_detail != NULL);

    for(; i < des.count; i++)
    {
      memcpy(des.p_detail + i, p_loop + i,
        sizeof(abs_software_upgrade_id_des_detail_t));
    }
    
    msg.content = DVB_ABS_SOFTWARE_UPGRADE_ID_DES_FOUND;
    msg.para1   = (u32)&des;
    msg.para2   = sizeof(abs_software_upgrade_id_des_t);

    if(p_svc != NULL && p_svc->notify != NULL)
    {
      p_svc->notify(p_svc, &msg);
    }
    mtos_free(des.p_detail);
  }
}

static inline u16 dump_subtitle_descriptor(u8 *p_data, s32 data_len, u8 *p_buf, 
s32 buf_len, u16 es_pid)
{
    subtitle_descr_t *p_subt_dr = (subtitle_descr_t *)p_buf;
    u16                 cnt = 0;


    for(; data_len >= 8 && buf_len >= sizeof(subtitle_descr_t)
        ; data_len -= 8, buf_len -= sizeof(subtitle_descr_t))
    {
        if(SYS_GET_LOW_HALF_BYTE(p_data[3]) <= 4)
        {
            p_subt_dr->pid = es_pid;

            p_subt_dr->language_code[0] = *p_data++;
            p_subt_dr->language_code[1] = *p_data++;
            p_subt_dr->language_code[2] = *p_data++;

            p_subt_dr->type        = *p_data++;
            p_subt_dr->cmps_page_id = MAKE_WORD(*(p_data + 1), *p_data);
            p_data += 2;
            p_subt_dr->ancl_page_id = MAKE_WORD(*(p_data + 1), *p_data);
            p_data += 2;

            p_subt_dr ++;
            cnt ++;
        }
    }

    return cnt;
}

static inline u16  dump_teletext_descriptor(u8 *p_data, s32 data_len, 
u8 *p_buf, s32 buf_len, u16 es_pid)
{
    teletext_descr_t *p_ttx_dr = (teletext_descr_t *)p_buf;
    u16                 cnt = 0;


    for(; data_len >= 5 && buf_len >= sizeof(teletext_descr_t)
        ; data_len -= 5, buf_len -= sizeof(teletext_descr_t))
    {
        if(SYS_GET_HIGH_FIVE_BITS(p_data[3]) >= (1 << 3) 
          && (SYS_GET_HIGH_FIVE_BITS(p_data[3]) <= (5 << 3)))
        {
            p_ttx_dr->pid = es_pid;

            p_ttx_dr->language_code[0] = *p_data++;
            p_ttx_dr->language_code[1] = *p_data++;
            p_ttx_dr->language_code[2] = *p_data++;

            p_ttx_dr->type     = ((*p_data) & 0xf8) >> 3;
            p_ttx_dr->magazien = ((*p_data++) & 0x7);
            p_ttx_dr->page     = *p_data++;


            p_ttx_dr ++;
            cnt ++;
        }
    }

    return cnt;
}

static inline u16  dump_cas_descriptor(u8 *p_data, pmt_t *p_pmt, u16 es_pid)
{
  u16  cnt = 0;
  u16 desc_len = p_data[1]+2;

  if(p_pmt->ecm_cnt > MAX_ECM_DESC_NUM)
    return 0;

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
  
  p_pmt->ecm_cnt++;
  //CHECK_FAIL_RET_ZERO(p_pmt->ecm_cnt <= MAX_ECM_DESC_NUM);

  return desc_len;
}

static inline u16  dump_program_info_desc(u8 *p_data, u16 info_len, pmt_t *p_pmt, BOOL dvbc)
{
  u32 desc_info_len = info_len;
  u32 cnt = 0;
  void *p_nvod_mosaic = class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID);
  
  while(cnt < desc_info_len)
  {
    switch(p_data[cnt])
    {
      case CA_DESC_ID:
        dump_cas_descriptor(&p_data[cnt], p_pmt, 0);
        break;
      case DVB_DESC_MOSAIC_DESC:
        {
          if (!dvbc)
          {
            break;
          }
          
          if (!p_pmt->mosaic_des_found
            && !p_pmt->logic_screen_des_found
            && !p_pmt->stream_identifier_des_found)
          {
            p_pmt->p_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_pmt->prog_num);
          }
          p_pmt->mosaic_des_found = 1;
          if (p_pmt->p_mosaic_des == NULL)
          {
            break;
          }
          parse_mosaic_des(&p_data[cnt + 1], p_pmt->p_mosaic_des);
        }
        break;
      case DVB_DESC_LOGIC_SCREEN:
        {
          if (!dvbc)
          {
            break;
          }
          
          if (!p_pmt->mosaic_des_found
            && !p_pmt->logic_screen_des_found
            && !p_pmt->stream_identifier_des_found)
          {
            p_pmt->p_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_pmt->prog_num);
          }
          p_pmt->logic_screen_des_found = 1;
          if (p_pmt->p_mosaic_des == NULL)
          {
            break;
          }
          parse_mosaic_logic_screen_des(&p_data[cnt + 1], p_pmt->p_mosaic_des);
        }
        break;
        
      case DVB_DESC_LINKAGE:
        break;

      case DVB_PRV_DESC_AD :
        {
          if (!dvbc || (p_data[cnt + 1] != 3))
          {
            break;
          }
          
          p_pmt->priv_ad_dec.program_id = p_data[cnt + 2];
          p_pmt->priv_ad_dec.ad_pid = MAKE_WORD2(p_data[cnt + 3], p_data[cnt + 4]);
          OS_PRINTF("\n##debug: private ad desciptor program_id[%d]ad_pid[%d]!!\n",
            p_pmt->priv_ad_dec.program_id, p_pmt->priv_ad_dec.ad_pid);
        }
        break;
      case DVB_DESC_TELETEXT:
        if(p_pmt->ttx_dr_cnt < DVB_MAX_TELETEXT_DESC)
        {
            p_pmt->ttx_dr_cnt += dump_teletext_descriptor(p_data + 2
                , p_data[cnt + 1]
                , (u8 *)(p_pmt->ttx_descr + p_pmt->ttx_dr_cnt)
                , sizeof(teletext_descr_t) * 
                      (DVB_MAX_TELETEXT_DESC - p_pmt->ttx_dr_cnt)
                , 0);
        }
        break;
      default:
        break;
   }
    cnt += (p_data[cnt + 1] + 2);
     
  }
    return 0;
}

static void parse_stream_identifier_des(u8 *p_buf, u16 audio_id, pmt_t *p_pmt)
{
  void *p_nvod_mosaic = class_get_handle_by_id(NVOD_MOSAIC_CLASS_ID);
  mosaic_logic_cell_t *p_lg_cell = NULL;
  u8 idx = 0;
  
  if (p_buf[1] >= MOSAIC_CELL_MAX)
  {
    return ;
  }
  
  if (!p_pmt->mosaic_des_found
    && !p_pmt->logic_screen_des_found
    && !p_pmt->stream_identifier_des_found)
  {
    p_pmt->p_mosaic_des = alloc_mosaic_buf(p_nvod_mosaic, p_pmt->prog_num);
  }

  if (p_pmt->p_mosaic_des == NULL)
  {
    return ;
  }
  p_lg_cell = (mosaic_logic_cell_t *)(p_pmt->p_mosaic_des->lg_cell);
  
  idx = p_buf[1];

  p_lg_cell[idx].logic_cell_id = idx;
  p_lg_cell[idx].audio_stream = audio_id;
  p_pmt->stream_identifier_des_found ++;
}

static BOOL dump_pmt_desc(parse_desc_param_t *p_parse_info)
{
  s16     es_info_len = p_parse_info->es_info_len;
  s16     desc_len    = p_parse_info->desc_len;
  s16     proc_len    = p_parse_info->proc_len;
  u16     element_pid = p_parse_info->element_pid;
  pmt_t   *p_pmt      = p_parse_info->p_pmt;
  u8      *p_data     = p_parse_info->p_buf_data;
  BOOL  ret   = FALSE;

  for(; es_info_len > 0; 
        es_info_len -= desc_len, proc_len += desc_len, p_data += desc_len)
  {
    desc_len = p_data[1] + 2;

    switch(p_data[0])
    {
    case DVB_DESC_DATA_BRODCAST_ID:
      if(((p_data[2] << 8) | p_data[3]) == 0x0B)
      {
          p_pmt->data_broadcast.manufacture_id = p_data[6];
          p_pmt->data_broadcast.hardware_id    = 
                                     (p_data[7] << 8) | p_data[8];
          p_pmt->data_broadcast.model_id       = p_data[9];
          p_pmt->data_broadcast.wait_timeout   = 
                (p_data[12] & 0xFC) >> 2;
      }
      break;
    case DVB_DESC_SUBTITLE:
      if(p_pmt->subt_dr_cnt < DVB_MAX_SUBTILTE_DESC)
      {
          p_pmt->subt_dr_cnt += dump_subtitle_descriptor(p_data + 2
              , desc_len - 2
              , (u8 *)(p_pmt->subt_descr + p_pmt->subt_dr_cnt)
              , sizeof(subtitle_descr_t) * 
                    (DVB_MAX_SUBTILTE_DESC - p_pmt->subt_dr_cnt)
              , element_pid);
      }
      break;
    case DVB_DESC_VBI_TELETEXT:
    case DVB_DESC_TELETEXT:
      if(p_pmt->ttx_dr_cnt < DVB_MAX_TELETEXT_DESC)
      {
          p_pmt->ttx_dr_cnt += dump_teletext_descriptor(p_data + 2
              , desc_len - 2
              , (u8 *)(p_pmt->ttx_descr + p_pmt->ttx_dr_cnt)
              , sizeof(teletext_descr_t) * 
                    (DVB_MAX_TELETEXT_DESC - p_pmt->ttx_dr_cnt)
              , element_pid);
      }
      break;
     case DVB_DESC_AC3:
      if(p_pmt->audio_count < DVB_MAX_AUDIO_PID_NUM)
      {
          p_pmt->audio[p_pmt->audio_count].p_id = element_pid;
          p_pmt->audio[p_pmt->audio_count].type = AUDIO_AC3;
          
          p_pmt->audio_count++;
      }
      break;
    case DVB_DESC_ISO_639_LANGUAGE:
      if(p_pmt->audio_count > 0)
      {
        p_pmt->audio[p_pmt->audio_count - 1].language_index = 
          iso_639_2_desc_to_idx((const char *)(p_data + 2));
        //memcpy(p_pmt->audio[p_pmt->audio_count - 1].language,
        //  p_data + 2, LANGUAGE_LEN);
      }
      break;
    case DVB_DESC_COMPONENT_DESC:
      {
        u8 stream_content = p_data[2] &0xF;
        u8 component_type = p_data[3];

        if(stream_content == 1 
        && component_type >= 1
         && component_type <= 0x10)
        {
          ret = TRUE;
        }
      }
      break;
      
    case DVB_DESC_MAXIMUM_BITRATE:
      break;
    case 0xE3://ABS_DESC_SOFTEWARE_UPGRADE:
      dump_abs_upgrade_id_descriptior(p_parse_info->p_svc,
        p_parse_info->element_pid,
        p_data, desc_len - 2);
      break;

    case CA_DESC_ID:
      dump_cas_descriptor(p_data, p_pmt, p_parse_info->element_pid);
      break;

    case DVB_DESC_TKGS_COMPONENT:
      p_pmt->tkgs_component_found = tkgs_parse_component(p_data);
      break;
    
    default:
      break;
    }
  }
  return ret;
}

static dump_pmt_desc_ret_t dump_pmt_desc_1(parse_desc_param_t *p_parse_info, BOOL dvbc)
{
  s16     es_info_len = p_parse_info->es_info_len;
  s16     desc_len    = p_parse_info->desc_len;
  s16     proc_len    = p_parse_info->proc_len;
  u16     element_pid = p_parse_info->element_pid;
  pmt_t   *p_pmt      = p_parse_info->p_pmt;
  u8      *p_data     = p_parse_info->p_buf_data;
  dump_pmt_desc_ret_t  ret   = RET_NONE;
  u16 loopi = 0;

  for(; es_info_len > 0; 
        es_info_len -= desc_len, proc_len += desc_len, p_data += desc_len)
  {
    desc_len = p_data[1] + 2;

    switch(p_data[0])
    {
    case DVB_DESC_DATA_BRODCAST_ID:
      if(((p_data[2] << 8) | p_data[3]) == 0x0B)
      {
          p_pmt->data_broadcast.manufacture_id = p_data[6];
          p_pmt->data_broadcast.hardware_id    = 
                                     (p_data[7] << 8) | p_data[8];
          p_pmt->data_broadcast.model_id       = p_data[9];
          p_pmt->data_broadcast.wait_timeout   = 
                (p_data[12] & 0xFC) >> 2;
      }
      break;
    case DVB_DESC_SUBTITLE:
      if(p_pmt->subt_dr_cnt < DVB_MAX_SUBTILTE_DESC)
      {
          p_pmt->subt_dr_cnt += dump_subtitle_descriptor(p_data + 2
              , desc_len - 2
              , (u8 *)(p_pmt->subt_descr + p_pmt->subt_dr_cnt)
              , sizeof(subtitle_descr_t) * 
                    (DVB_MAX_SUBTILTE_DESC - p_pmt->subt_dr_cnt)
              , element_pid);
      }
      break;
    case DVB_DESC_VBI_TELETEXT:
    case DVB_DESC_TELETEXT:
      if(p_pmt->ttx_dr_cnt < DVB_MAX_TELETEXT_DESC)
      {
          if ((desc_len - 2) > 0)
          {
            p_pmt->ttx_dr_cnt += dump_teletext_descriptor(p_data + 2
                , desc_len - 2
                , (u8 *)(p_pmt->ttx_descr + p_pmt->ttx_dr_cnt)
                , sizeof(teletext_descr_t) * 
                      (DVB_MAX_TELETEXT_DESC - p_pmt->ttx_dr_cnt)
                , element_pid);
          }
          else
          {
            for (loopi = 0; loopi < p_pmt->ttx_dr_cnt; loopi ++)
            {
              if (p_pmt->ttx_descr[loopi].pid == 0)
              {
                p_pmt->ttx_descr[loopi].pid = element_pid;
              }
            }
			if(p_pmt->ttx_dr_cnt == 0)
			{
				p_pmt->ttx_descr[0].pid = element_pid;
				p_pmt->ttx_descr[0].type = 1;
				p_pmt->ttx_dr_cnt ++;
			}
          }
      }
      break;
     case DVB_DESC_AC3:
      if(p_pmt->audio_count < DVB_MAX_AUDIO_PID_NUM)
      {
          p_pmt->audio[p_pmt->audio_count].p_id = element_pid;
          p_pmt->audio[p_pmt->audio_count].type = AUDIO_AC3_VSB;
          
          ret = RET_AUDIO;
      }
      break;
      case DVB_DESC_EAC3:
      if(p_pmt->audio_count < DVB_MAX_AUDIO_PID_NUM)
      {
          p_pmt->audio[p_pmt->audio_count].p_id = element_pid;
          p_pmt->audio[p_pmt->audio_count].type = AUDIO_EAC3;
          
          ret = RET_AUDIO;
      }
      break;
    case DVB_DESC_ISO_639_LANGUAGE:
      if(p_pmt->audio_count < DVB_MAX_AUDIO_PID_NUM)
      {
        p_pmt->audio[p_pmt->audio_count].language_index = 
          iso_639_2_desc_to_idx((const char *)(p_data + 2));

        //ENABLE_AUDIO_DESCRIPTION
        p_pmt->audio[p_pmt->audio_count].audio_type =  *(p_data + 5);
        
        //memcpy(p_pmt->audio[p_pmt->audio_count - 1].language,
        //  p_data + 2, LANGUAGE_LEN);
      }
      break;
    case DVB_DESC_COMPONENT_DESC:
      {
        u8 stream_content = p_data[2] &0xF;
        u8 component_type = p_data[3];

        if(stream_content == 1 
        && component_type >= 1
         && component_type <= 0x10)
        {
          ret = RET_VIDEO;
        }
      }
      break;
    case DVB_DESC_STREAM_IDENTIFIER:
      {
        if (dvbc)
        {
          parse_stream_identifier_des(p_data + 1, element_pid, p_pmt);
        }
      }
      break;
    case DVB_DESC_MAXIMUM_BITRATE:
      break;
    case 0xE3://ABS_DESC_SOFTEWARE_UPGRADE:
      dump_abs_upgrade_id_descriptior(p_parse_info->p_svc,
        p_parse_info->element_pid,
        p_data, desc_len - 2);
      break;
    case CA_DESC_ID:
     dump_cas_descriptor(p_data, p_pmt, p_parse_info->element_pid);
     break;
    default:
      break;
    }
  }
  return ret;
}

void parse_pmt_1(handle_t handle, dvb_section_t *p_sec)
{
    service_t *p_svc = handle;
    u8          *p_buf   = p_sec->p_buffer;
    u8          *p_data  = NULL;
    s16         proc_len = 0;
    os_msg_t    msg      = {0};
    pmt_t       pmt      = {0};

    u16         tmp_prog_info_len = 0;
    s16         sec_len       = 0;
    s16         prog_info_len = 0;
    s16         es_info_len   = 0;
    s16         desc_len      = 0;
    u16         element_pid   = 0;
    u8           stream_type = 0;
    parse_desc_param_t  parse_info = {0};
    dump_pmt_desc_ret_t dump_ret = FALSE;
    dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);

    pmt.p_origion_data = p_sec->p_buffer;
    
    if(MASK_FIRST_BIT_OF_BYTE(p_buf[5]) == 0 || p_buf[0] != DVB_TABLE_ID_PMT)
    {
        p_dvb->filter_data_error(p_svc, p_sec);
        return;
    }

    //All section length including header but not including CRC bytes
    sec_len = MAKE_WORD(p_buf[2],SYS_GET_LOW_HALF_BYTE(p_buf[1])) + 3 - CRC_LEN;
    pmt.crc_32 = p_buf[sec_len] << 24 | p_buf[sec_len + 1] << 16 
                      |p_buf[sec_len + 2] << 8 | p_buf[sec_len + 3];    

    pmt.prog_num  = MAKE_WORD(p_buf[4], p_buf[3]);
    pmt.pcr_pid   = MAKE_WORD(p_buf[9], SYS_GET_LOW_FIVE_BITS(p_buf[8]));
    pmt.version = (p_buf[5] & 0x3E) >> 1;
    prog_info_len = MAKE_WORD(p_buf[11], SYS_GET_LOW_HALF_BYTE(p_buf[10]));
    p_data        = p_buf + (proc_len = DEFAULT_PMT_SEC_LEN);
    pmt.pmt_pid   = p_sec->pid;

    pmt.audio_count = 0;
    pmt.subt_dr_cnt = 0;
    pmt.ttx_dr_cnt  = 0;

    //Record program information length
    tmp_prog_info_len = prog_info_len;
    proc_len += prog_info_len;

    if(prog_info_len > 0)
    {
      //  desc_len = p_data[1] + 2;

        // TODO: parse description

        //Parse description
      dump_program_info_desc(p_data,prog_info_len,&pmt, FALSE);
      p_data += prog_info_len;
    }

    
    //CHECK_FAIL_RET_VOID(prog_info_len == 0);


    while(proc_len < sec_len)
    {
        element_pid = MAKE_WORD(p_data[2], SYS_GET_LOW_FIVE_BITS(p_data[1]));
        es_info_len = MAKE_WORD(p_data[4], SYS_GET_LOW_HALF_BYTE(p_data[3]));
        stream_type = p_data[0];
        switch(stream_type)
        {
            case DVB_STREAM_MG2_VIDEO:
            case DVB_STREAM_MG1_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_MPEG;
                break;
            }
            case DVB_STREAM_264_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_H264;
                break;
            }
            case DVB_STREAM_AVS_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_AVS;
                break;
            }
            case DVB_STREAM_MPEG4_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_MPEG4;
                break;
            }
            case DVB_STREAM_MG1_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                  pmt.audio[pmt.audio_count].p_id = element_pid;
                  pmt.audio[pmt.audio_count].type = AUDIO_MP1;
               //   pmt.audio_count++;
                }
                break;
            }
            case DVB_STREAM_MG2_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_MP2;
                 //   pmt.audio_count++;
                }
                break;
            }
            case DVB_STREAM_AAC_AUDIO   :
            case DVB_STREAM_AAC_AUDIO_1 :
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_AAC;
               //     pmt.audio_count++;
                }
                break;
            }
            case DVB_STREAM_AC3_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_AC3_VSB;
                //    pmt.audio_count++;
                }
                break;
            }  
           case DVB_STREAM_OTA_SEC:
            {
              // ABS sec, ref GD/J 027-2009 4.3.3.2.1
              pmt.es_id = element_pid;
              break;
            }
            case 0xE0:
            {
              // ABS sec, ref GD/J 027-2009 4.3.3.2.1
              pmt.es_id = element_pid;
              break;
            }
            default:
              break;
        }

        proc_len += PMT_DESC_HEAD_LEN;
        p_data   += PMT_DESC_HEAD_LEN;

        //Parse description
        parse_info.desc_len    = desc_len;
        parse_info.element_pid = element_pid;
        parse_info.es_info_len = es_info_len;
        parse_info.proc_len    = proc_len;
        parse_info.p_buf_data  = p_data;
        parse_info.p_pmt       = &pmt;
        parse_info.p_svc = p_svc;

        //parse descriptor
        dump_ret = dump_pmt_desc_1(&parse_info, FALSE);

        if(dump_ret == RET_VIDEO//Video type 
          && pmt.video_pid == 0)   //Fix HD bugs
        {
          pmt.video_pid = element_pid;
        }

        //add the counts
        if(dump_ret == RET_AUDIO
          || stream_type == DVB_STREAM_MG1_AUDIO
          || stream_type == DVB_STREAM_MG2_AUDIO
          || stream_type == DVB_STREAM_AAC_AUDIO
          || stream_type == DVB_STREAM_AAC_AUDIO_1
          || stream_type == DVB_STREAM_AC3_AUDIO)
        {
            pmt.audio_count++;

        }


        //Move pointer
        p_data   += es_info_len;
        proc_len += es_info_len;
        
    }

        
    //All the tasks receiving this message must have higher task priority 
    //than DVB task
    msg.content = DVB_PMT_FOUND;
    msg.para1   = (u32)&pmt;
    msg.para2   = sizeof(pmt_t);
    msg.context = p_sec->r_context;
#ifdef __LINUX__
    msg.is_ext  = 1;
#endif 
    p_svc->notify(p_svc, &msg);
    
    return;
}

void parse_pmt_dvbc(handle_t handle, dvb_section_t *p_sec)
{
    service_t *p_svc = handle;
    u8          *p_buf   = p_sec->p_buffer;
    u8          *p_data  = NULL;
    s16         proc_len = 0;
    os_msg_t    msg      = {0};
    pmt_t       pmt      = {0};

    u16         tmp_prog_info_len = 0;
    s16         sec_len       = 0;
    s16         prog_info_len = 0;
    s16         es_info_len   = 0;
    s16         desc_len      = 0;
    u16         element_pid   = 0;
    parse_desc_param_t  parse_info = {0};
    BOOL  is_video = FALSE;

    pmt.p_origion_data = p_sec->p_buffer;
    
    if(MASK_FIRST_BIT_OF_BYTE(p_buf[5]) == 0 || p_buf[0] != DVB_TABLE_ID_PMT)
    {
        //Fix me!!!
        //filter_data_error(p_sec);
        return;
    }

    //All section length including header but not including CRC bytes
    sec_len = MAKE_WORD(p_buf[2],SYS_GET_LOW_HALF_BYTE(p_buf[1])) + 3 - CRC_LEN;
    pmt.prog_num  = MAKE_WORD(p_buf[4], p_buf[3]);
    pmt.version = (p_buf[5] & 0x3E) >> 1;
    pmt.pcr_pid   = MAKE_WORD(p_buf[9], SYS_GET_LOW_FIVE_BITS(p_buf[8]));
    prog_info_len = MAKE_WORD(p_buf[11], SYS_GET_LOW_HALF_BYTE(p_buf[10]));
    p_data        = p_buf + (proc_len = DEFAULT_PMT_SEC_LEN);
    pmt.pmt_pid = p_sec->pid;

    pmt.audio_count = 0;
    pmt.subt_dr_cnt = 0;
    pmt.ttx_dr_cnt  = 0;

    //Record program information length
    tmp_prog_info_len = prog_info_len;
    proc_len += prog_info_len;

    if (prog_info_len > 0)
    {
        //Parse description
      dump_program_info_desc(p_data,prog_info_len,&pmt, TRUE);
      p_data += prog_info_len;
    }

    while(proc_len < sec_len)
    {
        element_pid = MAKE_WORD(p_data[2], SYS_GET_LOW_FIVE_BITS(p_data[1]));
        es_info_len = MAKE_WORD(p_data[4], SYS_GET_LOW_HALF_BYTE(p_data[3]));

        switch(p_data[0])
        {
            case DVB_STREAM_MG2_VIDEO:
            case DVB_STREAM_MG1_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_MPEG;
                break;
            }
            case DVB_STREAM_264_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_H264;
                break;
            }
            case DVB_STREAM_AVS_VIDEO:
            {
                pmt.video_pid = element_pid;
                pmt.video_type = VIDEO_AVS;
                break;
            }
            case DVB_STREAM_MG1_AUDIO:
            case DVB_STREAM_MG2_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_MP2;
                    pmt.audio_count++;
                }
                break;
            }
            case DVB_STREAM_AAC_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_AAC;
                    pmt.audio_count++;
                }
                break;
            }
            case DVB_STREAM_AC3_AUDIO:
            {
                if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
                {
                    pmt.audio[pmt.audio_count].p_id = element_pid;
                    pmt.audio[pmt.audio_count].type = AUDIO_AC3_VSB;
                    pmt.audio_count++;
                }
                break;
            }  
           case DVB_STREAM_OTA_SEC:
            {
              // ABS sec, ref GD/J 027-2009 4.3.3.2.1
              pmt.es_id = element_pid;
              break;
            }
            case 0xE0:
            {
              // ABS sec, ref GD/J 027-2009 4.3.3.2.1
              pmt.es_id = element_pid;
              break;
            }
            default:
              break;
        }

        proc_len += PMT_DESC_HEAD_LEN;
        p_data   += PMT_DESC_HEAD_LEN;

        //Parse description
        parse_info.desc_len    = desc_len;
        parse_info.element_pid = element_pid;
        parse_info.es_info_len = es_info_len;
        parse_info.proc_len    = proc_len;
        parse_info.p_buf_data  = p_data;
        parse_info.p_pmt       = &pmt;
        parse_info.p_svc = p_svc;

        //parse descriptor
        is_video = dump_pmt_desc_1(&parse_info, TRUE);

        if(is_video == TRUE//Video type 
          && pmt.video_pid == 0)   //Fix HD bugs
        {
          pmt.video_pid = element_pid;
        }
        //Move pointer
        p_data   += es_info_len;
        proc_len += es_info_len;
        
    }

        
    //All the tasks receiving this message must have higher task priority 
    //than DVB task
    msg.content = DVB_PMT_FOUND;
    msg.para1   = (u32)&pmt;
    msg.para2   = sizeof(pmt_t);
    msg.context = p_sec->r_context;
#ifdef __LINUX__
    msg.is_ext  = 1;
#endif
    p_svc->notify(p_svc, &msg);
    
    return;
}

void parse_pmt_tkgs(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  u8          *p_buf   = p_sec->p_buffer;
  u8          *p_data  = NULL;
  s16         proc_len = 0;
  os_msg_t    msg      = {0};
  pmt_t       pmt      = {0};

  u16         tmp_prog_info_len = 0;
  s16         sec_len       = 0;
  s16         prog_info_len = 0;
  s16         es_info_len   = 0;
  s16         desc_len      = 0;
  u16         element_pid   = 0;
  parse_desc_param_t  parse_info = {0};
  BOOL  is_video = FALSE;

  pmt.p_origion_data = p_sec->p_buffer;

  if(MASK_FIRST_BIT_OF_BYTE(p_buf[5]) == 0 || p_buf[0] != DVB_TABLE_ID_PMT)
  {
      //Fix me!!!
      //filter_data_error(p_sec);
      return;
  }

  //All section length including header but not including CRC bytes
  sec_len = MAKE_WORD(p_buf[2],SYS_GET_LOW_HALF_BYTE(p_buf[1])) + 3 - CRC_LEN;
  pmt.prog_num  = MAKE_WORD(p_buf[4], p_buf[3]);
  pmt.pcr_pid   = MAKE_WORD(p_buf[9], SYS_GET_LOW_FIVE_BITS(p_buf[8]));
  prog_info_len = MAKE_WORD(p_buf[11], SYS_GET_LOW_HALF_BYTE(p_buf[10]));
  p_data        = p_buf + (proc_len = DEFAULT_PMT_SEC_LEN);


  pmt.audio_count = 0;
  pmt.subt_dr_cnt = 0;
  pmt.ttx_dr_cnt  = 0;

  //Record program information length
  tmp_prog_info_len = prog_info_len;
  proc_len += prog_info_len;

//  for(;prog_info_len > 0; prog_info_len -= desc_len, p_data += desc_len)
  if(prog_info_len > 0)
  {
    //  desc_len = p_data[1] + 2;

      // TODO: parse description

      //Parse description
    dump_program_info_desc(p_data,prog_info_len,&pmt, FALSE);
    p_data += prog_info_len;
  }
  
  //CHECK_FAIL_RET_VOID(prog_info_len == 0);


  while(proc_len < sec_len)
  {
      element_pid = MAKE_WORD(p_data[2], SYS_GET_LOW_FIVE_BITS(p_data[1]));
      es_info_len = MAKE_WORD(p_data[4], SYS_GET_LOW_HALF_BYTE(p_data[3]));

      switch(p_data[0])
      {
          case DVB_STREAM_MG2_VIDEO:
          case DVB_STREAM_MG1_VIDEO:
          case DVB_STREAM_264_VIDEO:
          case DVB_STREAM_AVS_VIDEO:
          {
              pmt.video_pid = element_pid;
              break;
          }
          case DVB_STREAM_MG1_AUDIO:
          case DVB_STREAM_MG2_AUDIO:
          case DVB_STREAM_AAC_AUDIO:

          {
              if(pmt.audio_count < DVB_MAX_AUDIO_PID_NUM)
              {
                  pmt.audio[pmt.audio_count].p_id = element_pid;
                  pmt.audio[pmt.audio_count].type = AUDIO_MPEG;
                  pmt.audio_count++;
              }
              break;
          }
          case DVB_STREAM_OTA_SEC:
          {
            // ABS sec, ref GD/J 027-2009 4.3.3.2.1
            pmt.es_id = element_pid;
            break;
          }
          case 0xE0:
          {
            // ABS sec, ref GD/J 027-2009 4.3.3.2.1
            pmt.es_id = element_pid;
            break;
          }
          default:
            break;
      }

      proc_len += PMT_DESC_HEAD_LEN;
      p_data   += PMT_DESC_HEAD_LEN;

      //Parse description
      parse_info.desc_len    = desc_len;
      parse_info.element_pid = element_pid;
      parse_info.es_info_len = es_info_len;
      parse_info.proc_len    = proc_len;
      parse_info.p_buf_data  = p_data;
      parse_info.p_pmt       = &pmt;
      parse_info.p_svc = p_svc;

      //parse descriptor
      is_video = dump_pmt_desc(&parse_info);

      if(is_video == TRUE//Video type 
        && pmt.video_pid == 0)   //Fix HD bugs
      {
        pmt.video_pid = element_pid;
      }
      //Move pointer
      p_data   += es_info_len;
      proc_len += es_info_len;
      
  }

      
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.content = DVB_PMT_FOUND;
  msg.para1   = (u32)&pmt;
  msg.para2   = sizeof(pmt_t);
  msg.context = p_sec->r_context;

  p_svc->notify(p_svc, &msg);
  
  return;
}


void request_pmt(dvb_section_t *p_sec, u32 para1, u32 para2)
{
  u16 lbn     = para2 & (0xFFFF);
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  CHECK_FAIL_RET_VOID(p_dvb_handle != NULL);
  
  p_sec->pid       = (para2 >> 16) &(0xFFFF);
  p_sec->sid       = lbn;//Temp solution, will be fixed later. 
  //p_sec->module_id = (para2 >> 16) &(0xFFFF);
  p_sec->timeout  = (u32)PMT_TIMEOUT;
  p_sec->table_id  = DVB_TABLE_ID_PMT;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_code[0] = DVB_TABLE_ID_PMT;
  p_sec->filter_mask[0] = 0xFF;
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->crc_enable = 1;
  //p_sec->use_soft_filter = TRUE;
  if(p_sec->sid != 0)
  {
#ifndef WIN32
  p_sec->filter_code[1] = lbn >> 8;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = lbn & 0xFF;
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_mask_size = 3;
#else
  p_sec->filter_code[3] = lbn >> 8;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = lbn & 0xFF;
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_mask_size = 5;
#endif
  }

  
  //Allocate dvb section
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
  
}


