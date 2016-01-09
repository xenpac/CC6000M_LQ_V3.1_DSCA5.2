/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "class_factory.h"
#include "mdl.h"
#include "lib_char.h"
#include "lib_util.h"

#include "LzmaIf.h"


#include "drv_dev.h"
#include "nim.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pmt.h"
#include "cat.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#include "db_dvbs.h"
#include "db_preset.h"
#include "data_manager.h"
#include "data_base.h"

#include "lib_memp.h"
#include "mem_manager.h"
#ifdef PRESET_DEBUG
/*!
   Macro for debug
  */
#define PRESET_PRINTF    OS_PRINTF
#else
/*!
   Macro for debug
  */
#define PRESET_PRINTF DUMMY_PRINTF
#endif


#define DEFAULT_VOLUME_STRONG (10)


typedef struct
{
  u32 diseqc_port;
  u32 votage;
  u32 k_option;

  u32 lnb_power;
  u32 lnb_type;

  u32 lnb_high;
  u32 lnb_low;
  u16 sat_name[MAX_NAME_LENGTH + 1];
  u16 longitude;
  u32 is_select;
  u8 is_abs;
  u8 reserved;
} preset_sat_node_t;

typedef struct
{
  u32 pol;
  u32 frq;
  u32 sym;
  u16 tp_id;
  u16 sat_id;
} preset_tp_node_t;

typedef struct
{
  u32 audio_pid1;
  u32 audio_type1;
  u32 audio_track;
  u32 audio_pid2;
  u32 audio_type2;
  u32 video_pid;
  u32 audio_pid3;
  u32 audio_type3;
  u32 audio_pid4;
  u32 audio_type4;
  u32 pcr_pid;
  u32 s_id;
  u16 is_domestic;
  u8 is_9b;
  u8 reserved;
  u32 net_id;
  u32 ts_id;
  u32 ca_id;
  u16 service_name[MAX_NAME_LENGTH + 1];
  u32 is_scrambled;
  u8 video_type;
} preset_pro_node_t;

typedef struct
{
  preset_sat_node_t *p_sat;
  preset_tp_node_t *p_tp;
  preset_pro_node_t *p_pro;
  preset_type_t node_type;
}preset_node_t;


static u32 _uni_len(const u16 *p_str) // a trick for byte alignment
{
  const u8 *p_eos = (u8 *)p_str;
  const u8 *pp_str = (u8 *)p_str;

  while(*p_eos != 0)
  {
    p_eos += 2;
  }
  //lint -e946
  return (u32)((p_eos - pp_str) / 2);
  //lint +e946
}


static u32 _uni_2_asc(u8 *p_ascstr, const u16 *p_unistr)
{
  u32 i = 0, j = 0;
  u8 *pp_unistr = (u8 *)p_unistr;

  MT_ASSERT(p_ascstr != NULL
           && p_unistr != NULL);

  while(pp_unistr[j] != 0)
  {
    p_ascstr[i] = (u8)pp_unistr[j];
    i++;
    j += 2;
  }
  p_ascstr[i] = 0;

  return i;
}


void _uni_strncpy(u16 *p_dst, const u16 *p_src, u32 count)
{
  u8 *pp_src = (u8 *)p_src;
  u8 *pp_dst = (u8 *)p_dst;

  if((NULL == p_dst)
    || (NULL == p_src))
  {
    return;
  }
  while(count != 0)
  {
    *pp_dst++ = *pp_src++;
    *pp_dst++ = *pp_src++;
    count--;
  }

  *pp_dst = 0;
}


void _uni_strcpy(u16 *p_dst, const u16 *p_src)
{
  //MT_ASSERT(p_dst != NULL && p_src != NULL);
  u8 *pp_src = (u8 *)p_src;
  u8 *pp_dst = (u8 *)p_dst;

  if((NULL == p_dst)
    || (NULL == p_src))
  {
    return;
  }
  while(*pp_src != 0)
  {
    *pp_dst++ = *pp_src++;
    *pp_dst++ = *pp_src++;
    /* copy p_src over p_dst */
  }
}


static int uni_2_int(const u16 *p_unistr)
{
  u8 conv_buf[255] = {0};
  int value = 0;

  _uni_2_asc(conv_buf, p_unistr);
  value = atoi((const char *)conv_buf);

  return value;
}


static void get_field_by_column(preset_node_t *p_node, u8 *p_buf, u16 length, u16 filedcnt)
{
  u8 *p_temp = p_buf;
  u8 len = 0;
  u8 conv_buf[255] = {0};


	MT_ASSERT(p_node != NULL && p_buf != NULL);

	switch(filedcnt+1)
	{
	case 1:
		p_node->node_type = ((*(p_temp) == 0x2A) ? PRESET_SAT: PRESET_PRO);
		if(p_node->node_type == PRESET_SAT)
		{
			_uni_strncpy((u16 *)p_node->p_sat->sat_name, (u16*)(p_temp+2),
				length>MAX_NAME_LENGTH?MAX_NAME_LENGTH:length);
			p_node->p_sat->sat_name[MAX_NAME_LENGTH] = 0;
		}

		if(p_node->node_type == PRESET_PRO)
		{
			_uni_strncpy((u16 *)p_node->p_pro->service_name, (u16*)p_temp,
				length>MAX_NAME_LENGTH?MAX_NAME_LENGTH:length);
			p_node->p_pro->service_name[MAX_NAME_LENGTH] = 0;
		}
		break;
	case 2:
		if(p_node->node_type == PRESET_SAT)
		{
			p_node->p_sat->lnb_low = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 3:
		if(p_node->node_type == PRESET_SAT)
		{
			p_node->p_sat->lnb_high = (u32)uni_2_int((u16 *)p_temp);
		}
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->video_pid = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 4:
		if(p_node->node_type == PRESET_SAT)
		{
			_uni_2_asc(conv_buf, (u16 *)p_temp);
			len = (u8)strlen((char*)conv_buf);

			if(conv_buf[len-1] == 'W' ||conv_buf[len-1] == 'w')
			{
				conv_buf[len-1] = '\0';
				p_node->p_sat->longitude = 0x8000 |(u16)(mt_atof((char*)conv_buf)*100);
			}
			else
			{
				conv_buf[len-1] = '\0';
				p_node->p_sat->longitude = (u16)(mt_atof((char*)conv_buf)*100);
			}
		}
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_pid1 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 5:
		if(p_node->node_type == PRESET_SAT)
		{
			p_node->p_sat->diseqc_port = (u32)uni_2_int((u16 *)p_temp);
		}
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->pcr_pid = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 6:
		if(p_node->node_type == PRESET_SAT)
		{
			p_node->p_sat->k_option = (u32)uni_2_int((u16 *)p_temp);
		}
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_tp->frq = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 7:
		if(p_node->node_type == PRESET_SAT)
		{
			p_node->p_sat->is_select = (u32)uni_2_int((u16 *)p_temp);
		}
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_tp->sym = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 8:
		if(p_node->node_type == PRESET_PRO)
		{
			_uni_2_asc(conv_buf, (u16 *)p_temp);
			if(conv_buf[0] == 'H')
			{
				p_node->p_tp->pol = 0;
			}
			else if(conv_buf[0] == 'V')
			{
				p_node->p_tp->pol = 1;
			}
			else
			{
				p_node->p_tp->pol = 0;
			}
		}
		break;
	case 9:
		if(p_node->node_type == PRESET_PRO)
		{
			_uni_2_asc(conv_buf, (u16 *)p_temp);
			if(conv_buf[0] == 'L')
			{
				p_node->p_pro->audio_track = 1;
			}
			else if(conv_buf[0] == 'R')
			{
				p_node->p_pro->audio_track = 2;
			}
			else if(conv_buf[0] == 'S')
			{
				p_node->p_pro->audio_track = 0;
			}
			else if(conv_buf[0] == 'M')
			{
				p_node->p_pro->audio_track = 3;
			}
			else
			{
				/*!
				Bug 8908 :  AUDIO_CHANNEL_STEREO to AUDIO_CHANNEL_LEFT
				*/
				p_node->p_pro->audio_track = 1;
			}
		}
		break;
	case 10:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->s_id = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 11:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->is_scrambled = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 12:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->net_id= (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 13:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->ts_id= (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 14:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->ca_id = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		break;
	case 25:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->video_type= (u8)uni_2_int((u16 *)p_temp);
		}
		break;
	case 26:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_type1 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 27:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_pid2 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 28:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_type2 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 29:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_pid3 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 30:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_type3 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 31:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_pid4 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	case 32:
		if(p_node->node_type == PRESET_PRO)
		{
			p_node->p_pro->audio_type4 = (u32)uni_2_int((u16 *)p_temp);
		}
		break;
	default:
		break;
	}
}


static u32 ignore_header(u32 *p_addr, u8 *p_line, u8 *p_buf, u32 size)
{
  u16 i = 0, offset = 0, cur_pos = 0;
  u8 pos_info = 0;

  cur_pos = 14;

  //while(cur_pos < size)
  while((cur_pos < size) && (i < 3))
  {
    pos_info = p_buf[cur_pos];
    cur_pos++;
    if(pos_info & 0x80)
    {
      if(p_buf[cur_pos] == 0x2A)
      {
        i++;
        offset = cur_pos - 1;
      }
      else
      {
        break;
      }
    }
  }
  *p_line = (u8)(i - 1);
  *p_addr = offset;
  return offset;
}


static void *preset_mem_alloc(u32 size)
{
  void *p_buf = NULL;

  p_buf = mtos_malloc(size);
  MT_ASSERT(p_buf != NULL);
  memset(p_buf, 0, size);

  return p_buf;
}


static void preset_mem_free(void *p_buf)
{
  mtos_free(p_buf);
}

static u32 db_strlen(const u16 *p_str)
{
  const u16 *p_eos = p_str;

  while(*p_eos++ != 0)
  {
    ;
  }
//lint -e946
  return (u32)(p_eos - p_str - 1);
//lint +e946
}


static BOOL is_new_pre_sat(u16 *p_sat_id, u8 sat_vv_id, preset_sat_node_t *p_rawsat)
{
  u16 sat_cnt = 0;
  u16 i = 0;
  u16 sat_id = 0, len_new = 0, len_exist = 0;
  sat_node_t sat = {0};

  sat_cnt = db_dvbs_get_count(sat_vv_id);
  len_new = (u16)db_strlen(p_rawsat->sat_name);

  for(i = 0; i < sat_cnt; i++)
  {
    sat_id = db_dvbs_get_id_by_view_pos(sat_vv_id, i);
    db_dvbs_get_sat_by_id(sat_id, &sat);
    len_exist = (u16)db_strlen(sat.name);
    if(len_exist ==  len_new)
    {
        if(0 == memcmp(p_rawsat->sat_name, sat.name, 2 * _uni_len(p_rawsat->sat_name)))
        {
            *p_sat_id = sat_id;
            return FALSE;
        }
    }
  }
  return TRUE;
}


void db_dvbs_load_preset(u8 blockid, preset_pg_type_t pg_type, preset_pg_area_t pg_area)
{
  u32 blocksize = 0;
  u8 *p_buf = NULL, *p_in_buf = NULL;
  u16 max_rows = 0;
  u16 i = 0, length = 0;
  u32 cur_sat_id = 0, cur_tp_id = 0;
  u8 vv_id_sat = 0, vv_id_tp = 0, vv_id_pro = 0;
  u8 head_rows = 0;
  u8 *p_zip_sys = NULL;
  // ur preset data size must less than 384 * KBYTES
  u32 out_size = 384 * KBYTES;
  u32 zip_sys_size = 64 * KBYTES;
  u32 offset = 0;
  //u32 total = 0;
  u8 pos_info = 0;
  u16 view_id = 0;
  preset_tp_node_t *tp_node = NULL, *p_cur_node = NULL;
  u16 tp_num = 0;
  BOOL tp_flag = FALSE;

  preset_pro_node_t rawpro = {0};
  preset_sat_node_t rawsat = {0};
  preset_tp_node_t rawtp = {0};
  preset_node_t prenode = {0};

  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  sat_node_t sat = {0};
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  MT_ASSERT(p_dm_handle != NULL);
  // init ext buf, at first
  prenode.p_pro = &rawpro;
  prenode.p_tp = &rawtp;
  prenode.p_sat = &rawsat;

  vv_id_sat = db_dvbs_create_view(DB_DVBS_SAT, 0, NULL);
  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);

  blocksize = dm_get_block_size(p_dm_handle, blockid);
  p_in_buf = preset_mem_alloc(blocksize);
  MT_ASSERT(p_in_buf != NULL);
  dm_read(p_dm_handle, blockid, 0, 0, blocksize, p_in_buf);

  p_buf = preset_mem_alloc(out_size);
  MT_ASSERT(p_buf != NULL);
  p_zip_sys = preset_mem_alloc(zip_sys_size);
  MT_ASSERT(p_zip_sys != NULL);
#if 0
  ret = gzip_uncompress(p_buf, &out_size, p_in_buf, blocksize, p_zip_sys, zip_sys_size);
  MT_ASSERT(ret == 0);
#else
  init_fake_mem_lzma(p_zip_sys, (int)zip_sys_size);
  lzma_decompress(p_buf, &out_size, p_in_buf, blocksize);
#endif
  preset_mem_free(p_zip_sys);
  preset_mem_free(p_in_buf);
  if(0 != memcmp(p_buf, "presetdata", 10))
  {
  //  MT_ASSERT(0);  if unzip fail(preset data size more than 384K), always run here ,it can't start even  reboot.
	OS_PRINTF("unzip fail\n");
	preset_mem_free(p_buf);
	return;
  }
  max_rows = (p_buf[10] << 8) | p_buf[11];

  if(max_rows == 2)
  {
    OS_PRINTF("No preset data\n");
    preset_mem_free(p_buf);
    return;
  }

  tp_node = preset_mem_alloc((max_rows > DB_DVBS_MAX_TP ? DB_DVBS_MAX_TP : max_rows)
                             * sizeof(preset_tp_node_t));
  MT_ASSERT(tp_node != NULL);
  ignore_header(&offset, &head_rows, p_buf, out_size);
  pos_info = p_buf[offset];
  offset++;
  //lint -e732 -e737 -e734
  while(offset < out_size)
  {
    if((pos_info & 0x80)
      && ((pos_info & 0x7f) != 0x00))
    {
      prenode.node_type = PRESET_PRO;
    }
    length = (u16)(2 * (u16)(_uni_len((u16 *)&p_buf[offset]) + 1));
    get_field_by_column(&prenode, &p_buf[offset], length, pos_info & 0x7f);
    offset += length;
    pos_info = p_buf[offset];
    offset++;
    if(pos_info & 0x80
      || offset >= out_size)                //find next row start flag, save current row info
    {
      switch(prenode.node_type)
      {
        case PRESET_SAT:
          if(((1 == rawsat.is_abs)
             && (PRESET_PG_TYPE_DVBS != pg_type))
            || ((0 == rawsat.is_abs)
               && (PRESET_PG_TYPE_ABS != pg_type)))
          {
            if(is_new_pre_sat(&view_id, vv_id_sat, &rawsat))
            {
              memset((u8 *)&sat, 0, sizeof(sat_node_t));
              sat.diseqc_port = rawsat.diseqc_port;
              if(sat.diseqc_port != 0)
              {
                sat.diseqc_type = 1;
                sat.diseqc_port = sat.diseqc_port - 1;
              }
              sat.k22 = rawsat.k_option;
              sat.lnb_high = rawsat.lnb_high;
              sat.lnb_low = rawsat.lnb_low;

              sat.user_band = 0; // not use
              sat.band_freq = 1210;
              if(C_BAND == dvbs_detect_lnb_freq(sat.lnb_low))
              {
                if(sat.lnb_low == sat.lnb_high)
                {
                  sat.lnb_type = 0;
                }
                else
                {
                  sat.lnb_type = 1;
                }
              }
              else
              {
                if(sat.lnb_low == sat.lnb_high)
                {
                  sat.lnb_type = 0;
                }
                else
                {
                  sat.lnb_type = 2;
                }
              }
              sat.id = cur_sat_id;
              sat.lnb_power = 0;
              sat.longitude = rawsat.longitude;

              _uni_strcpy(sat.name, rawsat.sat_name);
              db_dvbs_add_satellite(vv_id_sat, &sat);
              db_dvbs_save(vv_id_sat);
              if(rawsat.is_select == 1)
              {
                db_dvbs_change_mark_status(vv_id_sat, db_dvbs_get_count(vv_id_sat) - 1, DB_DVBS_SEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
              }
              else
              {
                db_dvbs_change_mark_status(vv_id_sat, db_dvbs_get_count(vv_id_sat) - 1, DB_DVBS_SEL_FLAG, 0);
              }
              cur_sat_id = sat.id;
            }
            else
            {
              cur_sat_id = view_id;
            }
          }
          break;
        case  PRESET_PRO:
          if((1 == rawpro.is_9b
             && PRESET_PG_TYPE_DVBS != pg_type)
            || (0 == rawpro.is_9b
               && PRESET_PG_TYPE_ABS != pg_type
               && ((PRESET_PG_AREA_DOMESTIC == pg_area
                   && 1 == rawpro.is_domestic)
                  || (PRESET_PG_AREA_OVERSEA == pg_area
                     && 0 == rawpro.is_domestic)
                  || PRESET_PG_AREA_ALL == pg_area)))
          {
            memset(&tp, 0, sizeof(dvbs_tp_node_t));
            tp.freq = rawtp.frq;
            tp.sym = rawtp.sym;
            tp.polarity = rawtp.pol;
            tp.sat_id = cur_sat_id;

            for(i = 0; i < tp_num; i++)
            {
              p_cur_node = tp_node + i;

              //   OS_PRINTF("freq:%d sym %d pol %d id %d\n",tp.freq,tp.sym,tp.polarity,tp.sat_id);
              //   OS_PRINTF("Freq:%d sym %d pol %d id
              // %d\n",p_cur_node->frq,p_cur_node->sym,p_cur_node->pol,p_cur_node->sat_id);
              if(tp.sat_id == p_cur_node->sat_id
                && tp.freq == p_cur_node->frq
                && tp.sym == p_cur_node->sym
                && tp.polarity == p_cur_node->pol)
              {
                //find duplicate tp
                tp_flag = TRUE;
                cur_tp_id = p_cur_node->tp_id;
                break;
              }
            }

            if(!tp_flag)
            {
              p_cur_node = tp_node + tp_num;
              memcpy(tp_node + tp_num, prenode.p_tp, sizeof(preset_tp_node_t));
              db_dvbs_add_tp(vv_id_tp, &tp);
              cur_tp_id = tp.id;
              p_cur_node->tp_id = tp.id;
              p_cur_node->sat_id = cur_sat_id;
              tp_num++;
              MT_ASSERT(tp_num < DB_DVBS_MAX_TP);
            }
            tp_flag = FALSE;

            // add pg
            if((rawpro.audio_pid1 != 0 && rawpro.audio_pid1 != 8191)
              || (rawpro.video_pid != 0 && rawpro.video_pid != 8191))
            {
	              memset(&pg, 0, sizeof(dvbs_prog_node_t));
	              pg.audio_track = rawpro.audio_track;

	              if(rawpro.audio_pid2 != 8191 && rawpro.audio_pid2 != 0)
	              {
	                	pg.audio[1].p_id = (u16)rawpro.audio_pid2;
	                	pg.audio[1].type = (u16)rawpro.audio_type2;
	                	pg.audio_ch_num++;

	                	if(rawpro.audio_pid3 != 8191 && rawpro.audio_pid3 != 0)
	                	{
	                	  pg.audio[2].p_id = (u16)rawpro.audio_pid3;
	                	  pg.audio[2].type = (u16)rawpro.audio_type3;
	                	  pg.audio_ch_num++;

	                	  if(rawpro.audio_pid4 != 8191 && rawpro.audio_pid4 != 0)
	                	  {
	                	    pg.audio[3].p_id = (u16)rawpro.audio_pid4;
	                	    pg.audio[3].type = (u16)rawpro.audio_type4;
	                	    pg.audio_ch_num++;
	                	  }
	                	}
	              }

	              pg.pcr_pid = rawpro.pcr_pid;
	              pg.sat_id = cur_sat_id;
	              pg.tp_id = cur_tp_id;
	              pg.video_pid = rawpro.video_pid;
	              pg.s_id = rawpro.s_id;
	              pg.is_scrambled = rawpro.is_scrambled;
	              pg.tv_flag = (rawpro.video_pid != 0) ? 1 : 0;
	              //pg.orig_net_id = rawpro.net_id;
	              //pg.ts_id = rawpro.ts_id;
	              //pg.ca_system_id = rawpro.ca_id;
	              pg.volume = DEFAULT_VOLUME_STRONG;
	              //  pg.name_is_unicode = 1;
	              pg.tv_flag = (rawpro.video_pid != 0) ? 1 : 0;
	              _uni_strcpy(pg.name, rawpro.service_name);
	              db_dvbs_add_program(vv_id_pro, &pg);
            }
          }
          break;
        default:
          break;
      }
      memset(prenode.p_pro, 0, sizeof(preset_pro_node_t));
      memset(prenode.p_sat, 0, sizeof(preset_sat_node_t));
    }
  }
  //lint +e732 +e737 +e734
  preset_mem_free(p_buf);
  // make all sat on selected status
  #if 0
  total = db_dvbs_get_count(vv_id_sat);
  for(i = 0; i < total; i++)
  {
    db_dvbs_change_mark_status(vv_id_sat, i,
                               DB_DVBS_SEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
  }
  #endif
//  MT_ASSERT(0);
  db_dvbs_save(vv_id_sat);
  db_dvbs_save(vv_id_tp);
  db_dvbs_save(vv_id_pro);
  // release ext buf
  preset_mem_free(tp_node);
}
