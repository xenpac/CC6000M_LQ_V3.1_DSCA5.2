/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifdef ENABLE_TIMESHIFT_CONFIG
#include "ui_common.h"
#include "pvr_api.h"
#include "ui_timeshift_bar.h"
#include "ui_timeshift_api.h"

u16 ui_timeshift_evtmap(u32 event);
static mul_pvr_play_status_t g_play_status = {MUL_PVR_PLAY_STATE_INVALID, };
static mul_pvr_rec_status_t g_rec_status = {MUL_PVR_REC_STATE_INVALID, };
typedef enum
{
  API_TIMESHIFT_EVT_PLAY_UPDATED = ((APP_TIMESHIFT << 16) + 0),
  API_TIMESHIFT_EVT_REC_UPDARED,
  API_TIMESHIFT_EVT_PLAY_SOF,
  API_TIMESHIFT_EVT_PLAY_REACH_REC,
}tshift_api_evt_t;

void ui_tshift_callback(handle_t tshift_handle, mul_pvr_event_t content, u32 para1, u32 para2)
{
  event_t evt = {0};

  switch(content)
  {
    case MUL_PVR_EVENT_PLAY_TIME_UPDATE:
			mul_pvr_play_get_status ((u32)tshift_handle, &g_play_status);
      evt.id = API_TIMESHIFT_EVT_PLAY_UPDATED;
      evt.data1 = para1;
      evt.data2 = (u32)&g_play_status;
      
      ap_frm_send_evt_to_ui(APP_TIMESHIFT, &evt);
      break;

    case MUL_PVR_EVENT_REC_TIME_UPDATE:
			mul_pvr_rec_get_status ((u32)tshift_handle, &g_rec_status);
      evt.id = API_TIMESHIFT_EVT_REC_UPDARED;
      evt.data1 = para1;
      evt.data2 = (u32)&g_rec_status;
      
      ap_frm_send_evt_to_ui(APP_TIMESHIFT, &evt);    
      break;

    case MUL_PVR_EVENT_PLAY_SOF:
      evt.id = API_TIMESHIFT_EVT_PLAY_SOF;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_TIMESHIFT, &evt);        
      break;

    case MUL_PVR_EVENT_PLAY_REACH_REC:
      evt.id = API_TIMESHIFT_EVT_PLAY_REACH_REC;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_TIMESHIFT, &evt);        
      break;          
      
    default:
      break;
  }
}


void ui_register_pvr_cb(u32 chanid, event_call_back callBack)
{
	if(SUCCESS != mul_pvr_register_event(chanid, callBack, NULL))
  {
    MT_ASSERT(0);
  }
}

void ui_unregister_pvr_cb(u32 chanid)
{
	if(SUCCESS != mul_pvr_unregister_event (chanid))
	{
	  MT_ASSERT(0);
	}
}

void ui_rec_destroy_chn(u32 chanid)
{
  if(SUCCESS != mul_pvr_rec_destroy_chn (chanid))
  {
    MT_ASSERT(0);
  }
}

void ui_play_destroy_chn(u32 chanid)
{
  if(SUCCESS != mul_pvr_play_destroy_chn (chanid))
  {
    MT_ASSERT(0);
  }
}
/*
void ui_tshift_init(void)
{
	fw_register_ap_evtmap(APP_TIMESHIFT, ui_timeshift_evtmap);
	fw_register_ap_msghost(APP_TIMESHIFT, ROOT_ID_TIMESHIFT_BAR);
}*/

void ui_tshift_keymap_init(void)
{
	fw_register_ap_evtmap(APP_TIMESHIFT, ui_timeshift_evtmap);
  fw_register_ap_msghost(APP_TIMESHIFT, ROOT_ID_TIMESHIFT_BAR);
}

void ui_tshift_deinit(void)
{
	fw_unregister_ap_evtmap(APP_TIMESHIFT);
	fw_unregister_ap_msghost(APP_TIMESHIFT, ROOT_ID_TIMESHIFT_BAR);
}

void ui_tshift_calc_rec_addr_size(u8 **p_addr, u32 *size)
{
  u16 pg_id = 0;
  dvbs_prog_node_t pg = {0};
  u32 _p_addr = 0;

  _p_addr = mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
  mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  mem_mgr_release_block(BLOCK_REC_BUFFER);
  OS_PRINTF("get addr in ui_tshift_rec_cfg 0x%x\n", _p_addr);
  MT_ASSERT(_p_addr != 0);

  pg_id = sys_status_get_curn_group_curn_prog_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);

  if(pg.video_type == VIDEO_MPEG)  //mpg
  {
	  *p_addr = (u8 *)(_p_addr - VIDEO_FW_MPEG_LESS_H264);
	  *size = mem_mgr_get_block_size(BLOCK_REC_BUFFER) + VIDEO_FW_MPEG_LESS_H264;
  }
  else
  {
	  *p_addr = (u8 *)_p_addr;
	  *size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  }
  (*size) = (*size) - 10 * KBYTES;
}


void ui_tshift_rec_cfg(mul_pvr_rec_attr_t *p_tshift_para, ui_pvr_private_info_t *ui_tshift_para)
{
  u16 pg_id, total;
  dvbs_prog_node_t prog;
  dvbs_tp_node_t tp;
  av_set_t av_set = {0};
  
  total = db_dvbs_get_count(ui_dbase_get_pg_view_id());

  pg_id = sys_status_get_curn_group_curn_prog_id();
  if((pg_id == INVALIDID) || (total == 0))
  {
    return ;
  }
  db_dvbs_get_pg_by_id(pg_id, &prog);
  db_dvbs_get_tp_by_id((u16)prog.tp_id, &tp);
  sys_status_get_av_set(&av_set);
  
  memset(p_tshift_para, 0, sizeof(mul_pvr_rec_attr_t));
	p_tshift_para->b_clear_stream = !ui_tshift_para->b_scramble;
	p_tshift_para->b_rewind = TRUE;
	p_tshift_para->demux_index = 0;
	p_tshift_para->media_info.v_pid = (u16)ui_tshift_para->video_pid;
	p_tshift_para->media_info.a_pid = (u16)ui_tshift_para->audio_pid;
	p_tshift_para->media_info.pcr_pid = (u16)ui_tshift_para->pcr_pid;
	p_tshift_para->media_info.audio_type = (adec_src_fmt_vsb_t)ui_tshift_para->audio_type;

  //1 means bs out
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == p_tshift_para->media_info.audio_type)
    ||(AUDIO_EAC3 == p_tshift_para->media_info.audio_type)))
  {
    p_tshift_para->media_info.audio_type = AUDIO_SPDIF;
  }
  p_tshift_para->media_info.video_type = (vdec_src_fmt_t)ui_tshift_para->video_type;
  p_tshift_para->media_info.extern_num = 0;
	//p_tshift_para->media_info.extern_pid[1] = 0;
  if (ui_tshift_para->b_scramble)
  {
    p_tshift_para->key_cfg.is_biss_key = 0;
    if(prog.ca_system_id == 0x2600)
    {
    }
  }
  else
  {
    p_tshift_para->key_cfg.is_biss_key = 0;
  }
  ui_tshift_calc_rec_addr_size(&p_tshift_para->p_rec_buffer, &p_tshift_para->rec_buffer_size);
	p_tshift_para->stk_size = RECORD_CHAIN_TASK_STK_SIZE;
	p_tshift_para->stream_type = MUL_PVR_STREAM_TYPE_TS;
	p_tshift_para->user_data_size = 0;
	uni_strncpy(p_tshift_para->file_name, ui_tshift_para->file_name, PVR_MAX_FILENAME_LEN * sizeof(u16));
	p_tshift_para->file_name_len = ui_tshift_para->file_name_len;
	p_tshift_para->max_file_size = ui_tshift_para->data_size;


}

void ui_tshift_play_cfg(mul_pvr_play_attr_t *p_tshift_para, ui_pvr_private_info_t *ui_tshift_para)
{
  memset(p_tshift_para, 0, sizeof(mul_pvr_play_attr_t));
  p_tshift_para->v_pid = (u16)ui_tshift_para->video_pid;
  p_tshift_para->v_type = ui_tshift_para->video_type;
  p_tshift_para->a_pid = (u16)ui_tshift_para->audio_pid;
  p_tshift_para->a_type = ui_tshift_para->audio_type;
  p_tshift_para->pcr_pid= (u16)ui_tshift_para->pcr_pid;
  //p_tshift_para->decrypt_cfg.bDoCipher = ui_tshift_para->b_scramble;
  uni_strcpy(p_tshift_para->file_name, ui_tshift_para->file_name);
  p_tshift_para->file_name_len = ui_tshift_para->file_name_len;
  p_tshift_para->is_clear_stream = TRUE;

  p_tshift_para->p_play_buffer = (u8 *)mem_mgr_require_block(BLOCK_PLAY_BUFFER, 0);
  mem_mgr_release_block(BLOCK_PLAY_BUFFER);
  p_tshift_para->play_buffer_size = mem_mgr_get_block_size(BLOCK_PLAY_BUFFER) - TS_PLAY_BUFFER_LEN;

  p_tshift_para->p_parse_buf = (u8 *)((u32)p_tshift_para->p_play_buffer + p_tshift_para->play_buffer_size);
  p_tshift_para->parse_buf_size = TS_PLAY_BUFFER_LEN;
  
	p_tshift_para->stk_size = PLAY_CHAIN_TASK_STK_SIZE;
	p_tshift_para->stream_type = MUL_PVR_STREAM_TYPE_TS;
}

void ui_tshift_rec_create(u32 *p_chnid, mul_pvr_rec_attr_t *tshift_para)
{
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    {
      ui_pvr_extern_open(tshift_para->file_name, UI_PVR_EXTERN_APPEND_RW);
    }
#endif
	if(SUCCESS != mul_pvr_rec_create_chn(p_chnid, tshift_para))
	{
		MT_ASSERT(0);
	}
	if(SUCCESS != mul_pvr_rec_start_chn(*p_chnid))
	{
		MT_ASSERT(0);
	}

}

void ui_tshift_rec_set(u32 h_chnid, mul_pvr_rec_attr_t *tshift_para)
{
	if(SUCCESS != mul_pvr_rec_set_chn (h_chnid, tshift_para))
	{
		MT_ASSERT(0);
	}
	if(SUCCESS != mul_pvr_rec_start_chn (h_chnid))
	{
		MT_ASSERT(0);
	}
}

void ui_tshift_play_start(u32 *p_play_id, u32 rec_id, mul_pvr_play_attr_t *p_play_attr)
{
	if(SUCCESS != mul_pvr_play_start_timeshift(p_play_id, rec_id, p_play_attr))
	{
		MT_ASSERT(0);
	}

}

void ui_tshift_pause(u32 h_chnid)
{
	if(SUCCESS != mul_pvr_play_pause_chn (h_chnid))
	{
		MT_ASSERT(0);
	}
}

void ui_tshift_resume(u32 h_chnid)
{
	if(SUCCESS != mul_pvr_play_resume_chn (h_chnid))
	{
		MT_ASSERT(0);
	}
}

void ui_tshift_stop(u32 h_chnid_play, u32 h_chnid_rec, mul_av_play_stop_opt_t * p_stop_para)
{

  if(h_chnid_rec != INVALIDID)
  {
    mul_pvr_rec_stop_chn(h_chnid_rec);
  }

  if((h_chnid_play != INVALIDID) && (p_stop_para != NULL))
  {
    mul_pvr_play_stop_timeshift (h_chnid_play, p_stop_para);
  }

  if(h_chnid_rec != INVALIDID)
  {
    mul_pvr_rec_destroy_chn(h_chnid_rec);
  }
#ifdef EXTERN_CA_PVR
  if(((CUSTOMER_ID == CUSTOMER_KINGVON) && (CAS_ID == CONFIG_CAS_ID_ABV))
    || ((CUSTOMER_ID == CUSTOMER_AISAT_DEMO) && (CAS_ID == CONFIG_CAS_ID_ONLY_1)))
    {
      ui_pvr_extern_close();
    }
#endif
}

void ui_tshift_play_mode(u32 chanid, mul_pvr_play_mode_t *p_play_mode)
{
	if(SUCCESS != mul_pvr_play_trick_play (chanid, p_play_mode))
	{
		MT_ASSERT(0);
	}
}

void ui_tshift_seek(u32 chanid, mul_pvr_play_position_t *p_position)
{
 // return;
  if(SUCCESS != mul_pvr_play_seek (chanid, p_position))
	{
		MT_ASSERT(0);
	}
}


BEGIN_AP_EVTMAP(ui_timeshift_evtmap)
CONVERT_EVENT(API_TIMESHIFT_EVT_PLAY_UPDATED, MSG_TIMESHIFT_EVT_PLAY_UPDATED)
CONVERT_EVENT(API_TIMESHIFT_EVT_REC_UPDARED, MSG_TIMESHIFT_EVT_REC_UPDARED)
CONVERT_EVENT(API_TIMESHIFT_EVT_PLAY_SOF, MSG_TIMESHIFT_EVT_PLAY_SOF)
CONVERT_EVENT(API_TIMESHIFT_EVT_PLAY_REACH_REC, MSG_TIMESHIFT_EVT_PLAY_REACH_REC)
END_AP_EVTMAP(ui_timeshift_evtmap)

//end of file

#endif

