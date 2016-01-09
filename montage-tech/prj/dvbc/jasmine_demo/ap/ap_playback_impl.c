/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "lib_util.h"

#include "lib_unicode.h"
#include "lib_char.h"
#include "string.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "hal_dma.h"


#include "common.h"
#include "drv_dev.h"
#include "lib_rect.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "avsync.h"
#include "drv_misc.h"

#include "uio.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "cat.h"
#include "sdt.h"
#include "nit.h"
#include "bat.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"

#include "subtitle_api.h"
#include "vbi_api.h"
#include "vbi_inserter.h"

#include "data_manager.h"
#include "data_base.h"
#include "db_dvbs.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "dmx.h"


#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_i.h"
//#include "ap_flounder_ota.h"
#include "sys_status.h"

#include "hal_misc.h"
#include "customer_config.h"
#include "ui_dbase_api.h"
#include "user_parse_table.h"
#ifdef QUICK_SEARCH_SUPPORT
#include "monitor_service.h"
#endif
#ifdef JHC_SIGNAL_INFO
#include "ui_signal.h"
#include "str_id.h"
#endif
#ifdef ENABLE_CA
#include "config_cas.h"
#endif

#ifdef NIT_SOFTWARE_UPDATE
#include "cas_ware.h"
update_t up_info = {0};
static u16 plat_id = 0;
#define revert_32(x)  \
           (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))
           
#define DVB_DESC_CDCAS_OTA        0xa1
#define ManufactureID_KINGVON     0X043F
#endif
#ifdef MIS_ADS
#define DVB_DESC_MIS_ADS_APP                 0xd4
#endif

typedef struct
{
  u32 cur_pg_id;
  u32 s_nit_ver;
} pb_data_t;

#if ENABLE_TTX_SUBTITLE
extern void stc_set_pg_id_1(handle_t handle, u16 pg_id);
#endif

static dvbs_prog_node_t  g_cur_pg = {0}; 

static void send_evt_to_ui(pb_evt_t evt_t, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = evt_t;
  evt.data1 = (u32)data1;
  evt.data2 = (u32)data2;
  ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
}

static void _play(void *p_data, play_param_t *p_play_param)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  p_pb_data->cur_pg_id = p_play_param->pg_info.context1;
#if ENABLE_TTX_SUBTITLE
   {
      class_handle_t handle = NULL;
      handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
      stc_set_pg_id_1(handle, (u16)p_play_param->pg_info.context1);
    }
#endif
  db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &g_cur_pg);
}


static void _check_hd_prog(void *p_data)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  //lint -e64
  vdec_info_t v_info = {0};
  //lint +e64
  void *p_video_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

  vdec_get_info(p_video_dev,&v_info);

  if((v_info.heigth > 720) && (v_info.width > 576))
  {
    ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
    MT_ASSERT(DB_DVBS_OK == ret);
    if(pg.service_type != 0x11)
    {
      if(pg.video_pid != 0)
      {
        pg.service_type = 0x11;
        db_dvbs_edit_program(&pg);
        db_dvbs_save_pg_edit(&pg);
      }
    }
  }
  //lint -e438 -e550
}
//lint +e438 +e550

static void _check(play_param_t play_param)
{
  BOOL is_display = FALSE;
  vdec_info_t v_info;
  static u8 cnt = 0;
  static u16 last_pmt_pid = 0;
  static u8 sup_cnt = 0;
  
  #ifdef JHC_SIGNAL_INFO
  static u8 jhc_show_msg_flag = 0;
  #endif

  void *p_disp_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  void *p_video_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
//  void *p_dmx_dev = dev_find_identifier(NULL,
//    DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  if(0 == play_param.pg_info.v_pid || 0x1FFF == play_param.pg_info.v_pid)
  {
    //radio return
    return;
  }

  vdec_get_info(p_video_dev,&v_info);


   if(last_pmt_pid != play_param.pg_info.pmt_pid)
  {
    last_pmt_pid = play_param.pg_info.pmt_pid;
    sup_cnt = 0;
  }
  if(3 == sup_cnt || 0 == sup_cnt)
  {
    if(v_info.is_format != TRUE)
    {
     // OS_PRINTF("------------->>applayback not support video!\n");
     // send_evt_to_ui(PB_EVT_NOT_SUPPORT_VIDEO, sup_cnt == 0?FALSE:TRUE , TRUE);
    }
    else
    {
      //send_evt_to_ui(PB_EVT_NOT_SUPPORT_VIDEO, sup_cnt == 0?FALSE:TRUE , FALSE);
    }
  }
  if(sup_cnt <= 3)
  {
    sup_cnt ++;
  }
  else
  {
    sup_cnt = 0;
  }


  is_display = disp_layer_is_show(p_disp_dev,DISP_LAYER_ID_VIDEO_SD);

  if(v_info.err == VDEC_ERROR_NONE)
  {
    if(!is_display)
    {
      if(TRUE == v_info.is_stable)
      {
        OS_PRINTF("------------->> open video!!!!!!!!!!!!!!! \n");
        disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
        disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, TRUE);
        send_evt_to_ui(PB_EVT_NOTIFY_VIDEO_FORMAT, 
          (u32)v_info.vid_format, (u32) play_param.pg_info.s_id);
      }

    }
    if(play_param.pg_info.is_scrambled &&  (++cnt > 1))
    {
      send_evt_to_ui(PB_DESCRAMBL_SUCCESS, 0, 0);
      cnt = 0;
    }
    #ifdef JHC_SIGNAL_INFO
    if (jhc_show_msg_flag == 1)
    {
      update_ca_message(0);
      jhc_show_msg_flag = 0;
    }
    #endif
  }
  else
  {
    cnt ++;

    if((cnt > 3 && v_info.err == VDEC_ERROR_THIRSTY) // no data input
      || (cnt > 4 && v_info.err == VDEC_ERROR_SIZE)) // error data input
    {
      cnt = 0;
      OS_PRINTF("------------->> close video\n");
      #ifdef JHC_SIGNAL_INFO
      update_ca_message(IDS_MSG_NO_SIGNAL_SIGLE);
      jhc_show_msg_flag = 1;
      #endif
      //dmx_av_reset(p_dmx_dev);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);
      if(play_param.pg_info.is_scrambled)
      {
        send_evt_to_ui(PB_DESCRAMBL_FAILED, 0, 0);
      }
    }
  }
  //OS_PRINTF("vdec info err %d \n",v_info.err);
}




static BOOL _process_pmt_info(void *p_data, pmt_t *p_pmt,
              u16 *p_v_pid, u16 *p_pcr_pid, u16 *p_a_pid, u16 *p_a_type)
{
  //pb_data_t *p_pb_data = (pb_data_t *)p_data;
  //db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_prog_node_t pg_temp = {0};
  BOOL data_changed = FALSE;
  BOOL audio_pid_changed = FALSE;
  u16 audio_count = 0;
  u16 i = 0;
  u16 old_a_pid = 0;
  u16 old_a_type = 0;
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 view_count = db_dvbs_get_count(view_id);
  u16 pg_id;
  BOOL find_pg = FALSE;
#ifdef QUICK_SEARCH_SUPPORT
  BOOL no_pid = FALSE;
#endif

  for(i=0; i<view_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg_temp);
#ifdef QUICK_SEARCH_SUPPORT
    if(pg_temp.s_id == p_pmt->prog_num && (pg_temp.pmt_pid == p_pmt->pmt_pid ||pg_temp.pmt_pid == 0))
  #else
    if(pg_temp.s_id == p_pmt->prog_num && pg_temp.pmt_pid == p_pmt->pmt_pid)
  #endif
    {
      memcpy(&pg, &pg_temp, sizeof(dvbs_prog_node_t));
      find_pg = TRUE;
      break;
    }
  }
  if(find_pg == FALSE)
  {

    return FALSE;
  }
#if 0
  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    return FALSE;
  }
#endif
  //param check
  if(p_pmt->audio_count > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    audio_count = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  else
  {
    audio_count = p_pmt->audio_count;
  }

#ifdef QUICK_SEARCH_SUPPORT 
  if(pg.video_pid==0
    && pg.video_pid != p_pmt->video_pid
    && pg.audio_ch_num == 0
    && pg.audio_ch_num != audio_count
    && pg.pmt_pid == 0)
  {
    no_pid =TRUE;
  }
#endif
  //video pid
  if(p_pmt->video_pid)
  {
    *p_v_pid = p_pmt->video_pid;
    if(pg.video_pid != p_pmt->video_pid
      || pg.video_type != p_pmt->video_type)
    {
      OS_PRINTF(" old video pid %d type  %d\n",
        pg.video_pid, pg.video_type);
      pg.video_pid = p_pmt->video_pid;
      pg.video_type = p_pmt->video_type;
      OS_PRINTF(" process pmt info  save new video pid %d type  %d\n",
        pg.video_pid, pg.video_type);
      data_changed = TRUE;
    }
  }

  //prc pid
  *p_pcr_pid = p_pmt->pcr_pid;
  if(pg.pcr_pid != p_pmt->pcr_pid)
  {
    pg.pcr_pid = p_pmt->pcr_pid;
    data_changed = TRUE;
  }

  //audio pid
  //lint -e571
  old_a_pid = (u16)pg.audio[pg.audio_channel].p_id;
  old_a_type = (u16)pg.audio[pg.audio_channel].type;
  //lint +e571
  
  for(i = 0; i < audio_count; i++)
  {
    if((pg.audio[i].p_id != p_pmt->audio[i].p_id)
      || (pg.audio[i].type != p_pmt->audio[i].type))
    {
      audio_pid_changed = TRUE;
      OS_PRINTF(" audio_pid_changed to %d\n",p_pmt->audio[i].p_id);
      break;
    }
  }

  if(pg.audio_ch_num != audio_count || audio_pid_changed)
  {
    for(i = 0; i < audio_count; i++)
    {
      pg.audio[i] = p_pmt->audio[i];
      OS_PRINTF("audio[%d] = [%d,%d,%d]\n",i,pg.audio[i].p_id,
        pg.audio[i].type,pg.audio[i].language_index);
    }
    pg.audio_ch_num = audio_count;
    pg.audio_channel = 0;
    audio_pid_changed = TRUE;
    data_changed = TRUE;
  }

  if(audio_pid_changed)
  {
    BOOL has_sim_pid = FALSE;

    //find cur audio pid
    for(i = 0; i < audio_count; i++)
    {
      if((old_a_pid == pg.audio[i].p_id)
        && (old_a_type == pg.audio[i].type))
      {
        has_sim_pid = TRUE;
        pg.audio_channel = i;
        break;
      }
    }

    //find a new pid
    if(!has_sim_pid)
    {
      pg.audio_channel = get_audio_channel(&pg);
      OS_PRINTF("find a new pid,audio_channel=%d,pg.audio_ch_num=%d\n",
        pg.audio_channel,pg.audio_ch_num);
      if(pg.audio_ch_num)
        MT_ASSERT(pg.audio_channel < pg.audio_ch_num);
    }
  }

  //lint -e571
  *p_a_pid = (u16)pg.audio[pg.audio_channel].p_id;
  *p_a_type = (u16)pg.audio[pg.audio_channel].type;
  //lint +e571

  //save new db info
  if(data_changed)
  {
  #ifdef QUICK_SEARCH_SUPPORT
    if(no_pid == TRUE)
    {
      pg.pmt_pid = p_pmt->pmt_pid;
    }
  #endif
    OS_PRINTF("data changed,PID:%d/%d/%d\n",
      pg.video_pid, pg.audio[pg.audio_channel].p_id,pg.pcr_pid);
    db_dvbs_edit_program(&pg);
    
  #ifdef QUICK_SEARCH_SUPPORT
    if(no_pid == TRUE)
    {
      //start monitor service
      {
        m_svc_cmd_p_t param = {0};
        param.s_id = pg.s_id;
        param.pmt_pid = pg.pmt_pid;

        dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_RESET_CMD, &param);
      }
    }
#endif
    return TRUE;
  }

  return FALSE;
}
#ifdef NIT_SOFTWARE_UPDATE
static s32 sn_ncmp(const u8 *p_src, const u8 *p_dst, u32 count)
{
  s32 ret = 0;

  MT_ASSERT(p_dst != NULL && p_src != NULL);

  while((count != 0) 
    && (*p_dst != 0)
    && (*p_src != 0)
    && !(ret = *(u8 *)p_src - *(u8 *)p_dst)
    )
  {
    ++p_src, ++p_dst;

    count--;
  }

  if(ret < 0)
  {
    ret = -1;
  }
  else if(ret > 0)
  {
    ret = 1;
  }

  return ret;
}

static void parse_cable_delivery_system_descriptor( u8* pBuf, cable_tp_info_t* pDesc )
{
  u8* data = pBuf;    
  if( data != NULL && pDesc != NULL )
  {
    pDesc->frequency = make32(data);
    pDesc->frequency = bcd_number_to_dec(pDesc->frequency)/10;

    pDesc->modulation = data[6] + 3;
    pDesc->symbol_rate = MT_MAKE_DWORD(
                      MAKE_WORD((data[10] & 0xf0), data[9]),
                      MAKE_WORD(data[8], data[7]));
    //pDesc->symbol_rate >>= 8;
    pDesc->symbol_rate = bcd_number_to_dec(pDesc->symbol_rate)/100;
  }
}

static BOOL parse_tfcas_codedownload_descriptor( u8* pBuf, u8  DescLength,
                                                update_t *p_up_info )
{
  BOOL    bErr = FALSE;
  u8*     pData = pBuf;
  s32     Length = DescLength;
  u16     ManufactureID = 0;
  u8      private_data_len = 0;
  cable_tp_info_t cable_tp_info = {0,};
  
#ifndef WIN32
#ifdef CAS_CONFIG_CDCAS
    extern RET_CODE cas_get_platformid(cas_module_id_t cam_id, u16 *platformid);
#endif
#endif

  if( pData != NULL && p_up_info != NULL )
  {
    ManufactureID = ( ( ( u16 ) pData[0] ) << 8 ) | pData[1];
#ifndef WIN32
#ifdef CAS_CONFIG_CDCAS
      cas_get_platformid(CAS_ID_TF, &plat_id);
#endif
#endif
    OS_PRINTF("[Update] ManufactureID=0x%x;ManufactureID=0x%x, PlatID=0x%x\n",ManufactureID, ManufactureID_KINGVON, plat_id);
    if(ManufactureID_KINGVON == ManufactureID)
    {
      //skip manufacture id
      Length -= 2;
      pData += 2;

      while( Length > 0 )
      {
        //skip cable delivery tag and len
        pData +=2;
        Length -=2;
        
        parse_cable_delivery_system_descriptor( pData, &cable_tp_info );
        OS_PRINTF("[Update] Down freq=%d,Qam=%d,Sym=%d\n", 
                      cable_tp_info.frequency,
                      cable_tp_info.modulation,
                      cable_tp_info.symbol_rate );
        
        //p_up_info->oui = ManufactureID;
        p_up_info->symbol = cable_tp_info.symbol_rate;
        p_up_info->freq = cable_tp_info.frequency;
        p_up_info->qam_mode = cable_tp_info.modulation;
        //skip cable delivery body
        pData += 11;
        Length -= 11;

        p_up_info->data_pid = (u16)(((pData[0]<<8)|(pData[1]&0xF8))>>3);
        OS_PRINTF("[Update] data_pid=0x%x\n",p_up_info->data_pid);
        p_up_info->ota_type = pData[1] & 0x7;
        OS_PRINTF("[Update] ota_type=0x%x\n",p_up_info->ota_type);
        private_data_len = pData[2];
        /*skip the Private_data_Len and pid */
        pData += 3;
        Length -= 3;

        /*Next is private data*/
        //Hardware_version
        p_up_info->hwVersion = make32(&pData[0]);
        OS_PRINTF("[Update] hwVersion=0x%x\n",p_up_info->hwVersion);
        Length -= 4;
        pData += 4;

        //Software_version
        p_up_info->swVersion = make32(&pData[0]);
        OS_PRINTF("[Update] swVersion=0x%x\n",p_up_info->swVersion);
        Length -= 4;
        pData += 4;
        
        //platform id
        p_up_info->plat_id = make32(&pData[0]);
        OS_PRINTF("[Update] plat_id=0x%x\n",p_up_info->plat_id);
        Length -= 4;
        pData += 4;

        return TRUE;
      }
    }
  }  
  return bErr;
}

extern u32 ui_ota_api_get_upg_check_version(void);
static BOOL check_update_param(update_t *ts_info)
{
  s32 ret;
  u32 stb_sw_ver = ui_ota_api_get_upg_check_version();
  char misc_info[200];
  u8 sn_STB[64];
  BOOL b_sn_pass = TRUE;
  misc_options_t *misc;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  
  ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)misc_info);
  MT_ASSERT(ret != ERR_FAILURE);
  misc = (misc_options_t *)misc_info;
  misc->ota_tdi.oui = revert_32(misc->ota_tdi.oui);
  //memcpy((ota_tdi_t *)p_tdi, (u8 *)&misc->ota_tdi, sizeof(ota_tdi_t));

  #if 1
  OS_PRINTF("misc->ota_tdi.oui=%d\n", misc->ota_tdi.oui);
  OS_PRINTF("misc->ota_tdi.hw_mod_id=%d\n", misc->ota_tdi.hw_mod_id);
  OS_PRINTF("misc->ota_tdi.hw_version=%d\n", misc->ota_tdi.hw_version);
  OS_PRINTF("stb_sw_ver=%d\n", stb_sw_ver);
  #endif

  if(0)//sys_get_serial_num(sn_STB, sizeof(sn_STB)))
  {
    sn_STB[48] = '0';
    sn_STB[49] = '0';
    sn_STB[50] = '1';
    sn_STB[51] = '1';
    sn_STB[52] = '1';
    sn_STB[53] = '1';
    sn_STB[54] = '1';
    sn_STB[55] = '1';
    sn_STB[56] = '1';
    sn_STB[57] = '1';
    sn_STB[58] = '1';
    sn_STB[59] = '1';
    sn_STB[60] = '1';
    sn_STB[61] = '1';
    sn_STB[62] = '0';
    sn_STB[63] = '1';

    ts_info->Serial_number_start[0] = '0';
    ts_info->Serial_number_start[1] = '0';
    ts_info->Serial_number_start[2] = '1';
    ts_info->Serial_number_start[3] = '1';
    ts_info->Serial_number_start[4] = '1';
    ts_info->Serial_number_start[5] = '1';
    ts_info->Serial_number_start[6] = '1';
    ts_info->Serial_number_start[7] = '1';
    ts_info->Serial_number_start[8] = '1';
    ts_info->Serial_number_start[9] = '1';
    ts_info->Serial_number_start[10] = '1';
    ts_info->Serial_number_start[11] = '1';
    ts_info->Serial_number_start[12] = '1';
    ts_info->Serial_number_start[13] = '1';
    ts_info->Serial_number_start[14] = '0';
    ts_info->Serial_number_start[15] = '0';

    ts_info->Serial_number_end[0] = '0';
    ts_info->Serial_number_end[1] = '0';
    ts_info->Serial_number_end[2] = '1';
    ts_info->Serial_number_end[3] = '1';
    ts_info->Serial_number_end[4] = '1';
    ts_info->Serial_number_end[5] = '1';
    ts_info->Serial_number_end[6] = '1';
    ts_info->Serial_number_end[7] = '1';
    ts_info->Serial_number_end[8] = '1';
    ts_info->Serial_number_end[9] = '1';
    ts_info->Serial_number_end[10] = '1';
    ts_info->Serial_number_end[11] = '1';
    ts_info->Serial_number_end[12] = '1';
    ts_info->Serial_number_end[13] = '1';
    ts_info->Serial_number_end[14] = '0';
    ts_info->Serial_number_end[15] = '0';

    if(sn_ncmp(&sn_STB[48], ts_info->Serial_number_start, 16)>=0
      && sn_ncmp(&sn_STB[48], ts_info->Serial_number_end, 16)<=0)
    {
      
    }
    else
    {
      b_sn_pass = FALSE;
    }
  }
  if(misc->ota_tdi.hw_version == ts_info->hwVersion
    && stb_sw_ver != ts_info->swVersion
    && plat_id == ts_info->plat_id
    && b_sn_pass)
  {
    send_evt_to_ui(PB_EVT_NIT_VERSION_SWITCH, (u32)(ts_info), 0);
    OS_PRINTF("send_evt_to_ui ota\n");
    return TRUE;
  }
  return FALSE;
}

static void parse_nit_for_upgrade(u8 *p_buf)
{
  s16 network_desc_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  u16 offset = 0;

  u16 nxt_ts_start = 0;

  u8  link_desc_len = 0;
  u8  linkage_type = 0;
  u8  OUI_data_length = 0;
  u8  OUI_data_offset = 0;
  u8  selector_length = 0;
  u8  selector_offset = 0;
  u32 OUI = 0;
  u32 i = 0;
  u8 *p_buf_temp = NULL;
  u32 temp = 0;
  u8 *pdata = NULL;
  BOOL ret = FALSE; 

  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    OS_PRINTF("MDL: not nit\n");
    return;
  }

  if((p_buf[5] & 0x01) == 0)
  {
    OS_PRINTF("MDL: this nit is not usable\n");
    return;
  }
  
  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  OS_PRINTF("network_desc_len : %d\n", network_desc_len);

  offset = 10;
  //OS_PRINTF("p_buf[11]:%d, p_buf[12]:%d, p_buf[13]:%d\n", p_buf[11], p_buf[12], p_buf[13]);

  while(network_desc_len > 0)
  {
    //net_name_des_len = p_buf[offset + 1];
    if(p_buf[offset] == DVB_DESC_CDCAS_OTA)
    {
      //Skip tag
      offset ++;
      network_desc_len --;
      
      //Fetch desc length
      desc_len = p_buf[offset];

      //Skip desc length
      offset ++;
      network_desc_len --;
      
      pdata = p_buf + offset;
#if 1
    OS_PRINTF("\n[NIT] DVB_DESC_CDCAS_OTA:len=%d\n", desc_len);
    for( i=0; i<desc_len; i++ )
    {
      OS_PRINTF("%02x ", pdata[i]);
    }
    OS_PRINTF("\n");
#endif
      ret = parse_tfcas_codedownload_descriptor(pdata, desc_len, &up_info);
      if(ret == TRUE)
      {
        check_update_param(&up_info);
        return;
      }
      offset += desc_len;
      network_desc_len -= desc_len;
    }
#ifdef MIS_ADS
    else if(p_buf[offset] == DVB_DESC_MIS_ADS_APP)
    {
        u16 dsi_pid = 0;
        u8  have_open_picture = 0;
        dvbc_lock_t mis_ads_tp_info = {0,};
        u16 openApp_Dsi_pid = 0x1fff;
        u16 main_etv_tsId = 0x1fff;
        u32 tp_freq = 0;
        u32 tp_sym = 0;
		u8 modulation = 0;
        //skip desc_tag
        offset ++;
        
        //skip desc_len
        offset ++;
        
        //skip reserved
        offset += 2;
        
        //dsi_pid
        dsi_pid = (((u16)p_buf[offset]<<8)|p_buf[offset + 1]);
        offset += 2;
        sys_status_set_MIS_ads_dis_pid(dsi_pid);
        OS_PRINTF("<parse nit<<<<<<MIS_ADS dsi_pid:%d\n",dsi_pid);

        //have_open_picture
        have_open_picture = p_buf[offset];
        offset ++;
        
        //infomation
        if(1 == have_open_picture)
        {
          tp_freq = (u32)(p_buf[offset]<<24)
                      |(p_buf[offset + 1]<<16)
                      |(p_buf[offset + 2]<<8)
                      |p_buf[offset + 3];
          mis_ads_tp_info.tp_freq = bcd_number_to_dec(tp_freq) / 10;
          
          modulation = p_buf[offset + 4];
          switch(modulation)
          {
            case 1:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM16;
              break;
            case 2:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM32;
              break;
            case 3:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM64;
              break;
            case 4:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM128;
              break;
            case 5:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM256;
              break;
            default:
              mis_ads_tp_info.nim_modulate = NIM_MODULA_QAM64;
              break;
          }
          tp_sym = (u32)(p_buf[offset + 5]<<24)
                      |(p_buf[offset + 6]<<16)
                      |(p_buf[offset + 7]<<8)
                      |p_buf[offset + 8];
          mis_ads_tp_info.tp_sym = bcd_number_to_dec(tp_sym) / 100;
          
          openApp_Dsi_pid = (((u16)p_buf[offset + 9]<<8)|p_buf[offset+ 10]);

          //此处还需要保存频点信息和dsi_pid到FLASH里，下次开机使用
          sys_status_set_MIS_ads_tp_info(&mis_ads_tp_info);
          sys_status_set_MIS_ads_openApp_dis_pid(openApp_Dsi_pid);
          //skip reserved
          offset += 13;
          main_etv_tsId = (((u16)p_buf[offset]<<8)|p_buf[offset + 1]);
          OS_PRINTF("<parse nit<<<<<<MIS_ADS tp_freq:%d\n",mis_ads_tp_info.tp_freq);
          OS_PRINTF("<parse nit<<<<<<MIS_ADS tp_sym:%d\n",mis_ads_tp_info.tp_sym);
          OS_PRINTF("<parse nit<<<<<<MIS_ADS nim_modulate:%d\n",mis_ads_tp_info.nim_modulate);
          OS_PRINTF("<parse nit<<<<<<MIS_ADS openApp_Dsi_pid:%d\n",openApp_Dsi_pid);
        }
        break;
      }
#endif

    else
    {
    //OS_PRINTF("%s,%d, desc_len:%d, network_desc_len:%d,\n",__func__,__LINE__,desc_len,network_desc_len);
#ifdef MIS_ADS 
      offset ++;
      network_desc_len --;
#else
      desc_len = p_buf[offset + 1];
      network_desc_len -= 2 + desc_len;
      offset += 2 + desc_len;
#endif
    }
  }
  //offset += network_desc_len;

  ts_loop_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
  
  offset += 2;

  while(ts_loop_len > 0)
  {
    //OS_PRINTF("ts_loop_len[%d]\n", ts_loop_len);

    /*! ts id*/

    offset += 4;

    desc_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
    /*! Skip offset length*/
    offset += 2;
    
    ts_loop_len -= (desc_len + 6);
    /*! Saving next start position for next ts id*/
    nxt_ts_start = offset + desc_len; 

    while(desc_len > 0)
    {
      //OS_PRINTF("desc_len[%d]\n", desc_len);
      switch(p_buf[offset])
      {
          case DVB_DESC_LINKAGE:
            //Skip tag
            offset ++;
            desc_len --;

            link_desc_len = (u8)p_buf[offset];
            if(link_desc_len>=7)
            {
              linkage_type = p_buf[offset+7];
              switch(linkage_type)
              {
                case 0x09:
                  p_buf_temp = &p_buf[offset+8];
                  OUI_data_length = p_buf_temp[0];
                  p_buf_temp++;
                  while(OUI_data_length>0)
                  {                                        
                    OUI = (p_buf_temp[0]<<16 | p_buf_temp[1]<<8 | p_buf_temp[2]);
                    up_info.oui = OUI;

                    OUI_data_length -= 3;
                    OUI_data_offset += 3;

                    p_buf_temp += 3;

                    selector_length = p_buf_temp[0];
                    OUI_data_length--;
                    OUI_data_offset++;

                    p_buf_temp++;

                    OUI_data_length -= selector_length;
                    OUI_data_offset += selector_length;

                    while(selector_length>0)
                    {
                      //Cable_delivery_desc
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;
                      
                      //DSysDes_Length
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Freq_desc
                      up_info.freq= make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Fec_outer
                      selector_offset++;
                      p_buf_temp++;

                      //QAM_Mode
                      up_info.qam_mode = (u8)p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Symbolrate_desc
                      up_info.symbol= make16(&p_buf_temp[0]);
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Fec_inner
                      selector_offset++;
                      p_buf_temp++;

                      //Download_Pid
                      up_info.data_pid = make16(&p_buf_temp[0]);
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Update_type
                      up_info.ota_type = (u8)p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Private_data_length
                      selector_offset++;
                      p_buf_temp++;

                      //Hardware_version
                      up_info.hwVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Software_type
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Software_version
                      up_info.swVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Serial_number_start
                      memcpy(up_info.Serial_number_start, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Serial_number_end
                      memcpy(up_info.Serial_number_end, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Private_data
                      selector_offset += 4;
                      p_buf_temp += 4;

                      selector_length -= selector_offset;
                      #if 1
                      OS_PRINTF("up_info.oui=%d\n", up_info.oui);
                      OS_PRINTF("up_info.freq=%d\n", up_info.freq);
                      OS_PRINTF("up_info.symbol=%d\n", up_info.symbol);
                      OS_PRINTF("up_info.qam_mode=%d\n", up_info.qam_mode);
                      OS_PRINTF("up_info.data_pid=%d\n", up_info.data_pid);
                      OS_PRINTF("up_info.ota_type=%d\n", up_info.ota_type);
                      
                      OS_PRINTF("up_info.Serial_number_start=0x");
                      for(i=0; i<sizeof(up_info.Serial_number_start); i++)
                      {
                        OS_PRINTF("%02x", up_info.Serial_number_start[i]);
                      }
                      OS_PRINTF("\n");
                      
                      OS_PRINTF("up_info.Serial_number_end=0x");
                      for(i=0; i<sizeof(up_info.Serial_number_end); i++)
                      {
                        OS_PRINTF("%02x", up_info.Serial_number_end[i]);
                      }
                      OS_PRINTF("\n");
                      
                      OS_PRINTF("up_info.swVersion=%d\n", up_info.swVersion);
                      OS_PRINTF("up_info.hwVersion=%d\n", up_info.hwVersion);
                      #endif

                      switch(up_info.ota_type)
                      {
                        case 0:
                          up_info.ota_type = UI_OTA_FORCE;
                          break;
                          
                        case 1:
                          up_info.ota_type = UI_OTA_NORMAL;
                          break;
                          
                        case 2:
                          up_info.ota_type = UI_OTA_NORMAL;
                          break;

                        default:
                          up_info.ota_type = UI_OTA_NORMAL;
                          break;
                      }
                      
                      {
                        s32 ret;
                        u32 stb_sw_ver = ui_ota_api_get_upg_check_version();
                        
                      #ifdef CHECK_STB_SERIAL_OTA
                        u8 sn_STB[64];
                      #endif
                      
                        BOOL b_sn_pass = TRUE;
                        misc_options_t misc;
                        void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
                        
                        ret = dm_read(p_dm_handle, MISC_OPTION_BLOCK_ID, 0, 0, sizeof(misc_options_t), (u8*)&misc);
                        MT_ASSERT(ret != ERR_FAILURE);
      
                        #if 1
                        OS_PRINTF("misc->ota_tdi.oui=%d\n", misc.ota_tdi.oui);
                        OS_PRINTF("misc->ota_tdi.hw_mod_id=%d\n", misc.ota_tdi.hw_mod_id);
                        OS_PRINTF("misc->ota_tdi.hw_version=%d\n", misc.ota_tdi.hw_version);
                        OS_PRINTF("stb_sw_ver=%d\n", stb_sw_ver);
                        #endif

                        #ifdef CHECK_STB_SERIAL_OTA
                         for(i=1;i<16;i++)
                         {
                           up_info.Serial_number_start[i] +=48;
                           up_info.Serial_number_end[i] +=48;
                         }
                         #if 0
                         for(i=1;i<16;i++)
                         {
                           OS_PRINTF("%d",up_info.Serial_number_start[i] );
                         }

                         for(i=1;i<16;i++)
                         {
                           OS_PRINTF("%d",up_info.Serial_number_end[i] );
                         }
            			    #endif
                         if(sys_get_serial_num(sn_STB, sizeof(sn_STB)))
                         {
                            for(i=0;i<15;i++)
                            {
                              OS_PRINTF("%d",sn_STB[i]);
                            }
                            if(sn_ncmp(sn_STB, &up_info.Serial_number_start[1], 15)>=0
                            && sn_ncmp(sn_STB, &up_info.Serial_number_end[1], 15)<=0)
                            {
                               OS_PRINTF("\n compare ok");
                            }
                            else
                            {
                              b_sn_pass = FALSE;
                            } 
                         }
                        #endif

                        if(misc.ota_tdi.oui == up_info.oui
                          && misc.ota_tdi.hw_version == up_info.hwVersion
                          && stb_sw_ver != up_info.swVersion
                          && b_sn_pass)
                        {
                          send_evt_to_ui(PB_EVT_NIT_VERSION_SWITCH, (u32)(&up_info), 0);
                        }
                      }
                    }
                  }
                  break;

                default:
                  break;
              }
            }

            desc_len -= link_desc_len;
            offset   += link_desc_len;   

            offset ++;
            desc_len -- ;
            break;
          
          default:
          /*! 
            Jump to unknown desc length
           */
          offset ++;
          desc_len --;

          //OS_PRINTF("MDL: parse nit tmp_desc_length %d\n",p_buf[offset]);
          //OS_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          //Skip content of unkown descriptor
          //if(desc_len > tmp_desc_length)
          {
            desc_len -=  (s16)p_buf[offset];
            offset   +=  p_buf[offset];   
          }

          //OS_PRINTF("MDL: parse nit offset %d\n",  offset);
          //OS_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          offset ++;
          desc_len -- ;
          break;
        }

    }
    
    offset = nxt_ts_start ;
  }

  return;  
}
#endif
static BOOL _process_nit_info(void *p_data, nit_t *p_nit )
{
    BOOL ret = TRUE;
    u8 nit_ver_num = p_nit->version_num;
    u32 old_nit_ver_num = 0;
    BOOL nit_recv;
    nc_channel_info_t tp_info = {0};
    class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    dvbc_lock_t main_tp = {0};

    sys_status_get_nit_version(&old_nit_ver_num);
    sys_status_get_status(BS_NIT_RECIEVED, &nit_recv);
    OS_PRINTF("_process_nit_info old_ver_num:%d, nit_ver_num:%d\n",old_nit_ver_num, nit_ver_num);
    if(nit_recv == FALSE)
    {
      if (CUSTOMER_ID == CUSTOMER_DEFAULT)
        {
           nc_get_tp(nc_handle, 0, &tp_info);
           sys_status_get_main_tp1(&main_tp);
           if((main_tp.tp_freq == tp_info.channel_info.frequency) &&
               (main_tp.tp_sym == tp_info.channel_info.param.dvbc.symbol_rate) &&
               (main_tp.nim_modulate == tp_info.channel_info.param.dvbc.modulation))
           {
               sys_status_set_status(BS_NIT_RECIEVED, TRUE);
#ifndef STARTUP_NIT_VERSION_CHECK
  	        sys_status_set_nit_version(nit_ver_num);    
#endif
            }
         }
     }
     else if(old_nit_ver_num != nit_ver_num)
     { 
	 //sys_status_set_nit_version(p_nit->version_num);
	 OS_PRINTF("_process_nit_info old_ver_num:%d, nit_ver_num:%d\n",old_nit_ver_num, nit_ver_num);
#ifndef STARTUP_NIT_VERSION_CHECK
        if(CUSTOMER_ID == CUSTOMER_DEFAULT)
        {
           nc_get_tp(nc_handle, 0, &tp_info);
           sys_status_get_main_tp1(&main_tp);
           if((main_tp.tp_freq == tp_info.channel_info.frequency) &&
               (main_tp.tp_sym == tp_info.channel_info.param.dvbc.symbol_rate) &&
               (main_tp.nim_modulate == tp_info.channel_info.param.dvbc.modulation))
           {
               send_evt_to_ui(PB_EVT_UPDATE_VERSION, (u32)nit_ver_num, 0);
               OS_PRINTF(" @@ _process_nit_info do search  line =%d\n",__LINE__);
            }
         }
#endif
		
       }
	  /*
   if(p_pb_data->s_nit_ver != p_nit->version_num)
   {
     p_pb_data->s_nit_ver = p_nit->version_num;
     OS_PRINTF("PB DVB_NIT_FOUND version:%d\n",p_pb_data->s_nit_ver);
     send_evt_to_ui(PB_EVT_UPDATE_VERSION, p_nit->network_id, 0);
   }
   */
#ifdef NIT_SOFTWARE_UPDATE
    if(p_data != NULL)
    {
      parse_nit_for_upgrade((u8*)p_nit->p_origion_data);
    }
#endif    
    return ret;
}

static BOOL _process_sdt_info(void *p_data, sdt_t *p_sdt)
{
  BOOL bRet = FALSE;
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp_node = {0};
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
  u16 i = 0;
  
  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    bRet =  FALSE;
  }
  else
    {
    ret = db_dvbs_get_tp_by_id(pg.tp_id, &tp_node);
    if(DB_DVBS_OK == ret)
    {
      for(i=0; i<p_sdt->svc_count; i++)
      {
        if(p_sdt->svc_des[i].svc_id == pg.s_id)// && p_sdt->version_num == tp_node.sdt_version) //jiaguoteng: sdt update need improved
        {
          dvb_to_unicode(p_sdt->svc_des[i].name, sizeof(p_sdt->svc_des[i].name), name, DB_DVBS_MAX_NAME_LENGTH+1);

          if(uni_strcmp(pg.name, name) != 0)
          {
            uni_strcpy(pg.name, name);
            OS_PRINTF("program name changed\n");

            //prevent pg namne changed by sdt update
            #ifdef DALIAN_ZHUANGGUANG
            return TRUE;
            #endif
            
            db_dvbs_edit_program(&pg);
            db_dvbs_save_pg_edit(&pg);
            send_evt_to_ui(PB_EVT_UPDATE_PG_NAME, (u32)pg.id, 0);

            bRet = TRUE;
          }

          break;
        }
      }
    }
  }
  
  return bRet;
}

void _init_play(play_param_t *p_play_param)
{
  p_play_param->is_do_sdt = TRUE;
  p_play_param->is_do_nit = TRUE;
  p_play_param->is_do_dynamic_pid = TRUE;
  p_play_param->start_mode = PB_START_STABLE;
}

pb_policy_t *construct_pb_policy(void)
{
#if ENABLE_TTX_SUBTITLE
  extern const u8 wstfont2_bits_pal_vsb[];
  extern const u8 wstfont2_bits_ntsc_vsb[];
  extern const u8 wstfont2_bits_small_vsb[];
  extern const u8 wstfont2_bits_hd_vsb[];

  ttx_font_src_t ttx_font;

#endif
  pb_policy_t *p_pb_pol = mtos_malloc(sizeof(pb_policy_t));
  MT_ASSERT(p_pb_pol != NULL);
  //lint -e668
  memset(p_pb_pol, 0, sizeof(pb_policy_t));
  //lint +e668
  
  //Alloc private data
  p_pb_pol->p_data = mtos_malloc(sizeof(pb_data_t));
  MT_ASSERT(p_pb_pol->p_data != NULL);
  memset(p_pb_pol->p_data, 0, sizeof(pb_data_t));

  p_pb_pol->init_play = _init_play;
  p_pb_pol->on_play = _play;
  p_pb_pol->process_pmt_info = _process_pmt_info;
  p_pb_pol->check_video = _check;
  p_pb_pol->process_sdt_info = _process_sdt_info;
  p_pb_pol->process_nit_info = _process_nit_info;
  p_pb_pol->check_hd_prog = _check_hd_prog;
  p_pb_pol->is_subtitle_on = FALSE;

#if ENABLE_TTX_SUBTITLE
  ttx_font.p_ntsl_font = (u8 *)wstfont2_bits_ntsc_vsb;
  ttx_font.p_pal_font = (u8 *)wstfont2_bits_pal_vsb;
  ttx_font.p_small_font = (u8 *)wstfont2_bits_small_vsb;
  ttx_font.p_hd_font = (u8 *)wstfont2_bits_hd_vsb;

  {
    RET_CODE ret = ERR_FAILURE;
    ret = subt_init_vsb(MDL_SUBT_TASK_PRIORITY, MDL_SUBT_TASK_STKSIZE,
                                  DISP_LAYER_ID_SUBTITL,mdl_broadcast_msg);
    MT_ASSERT(ret == SUCCESS);
    ret = vbi_init_vsb(MDL_VBI_TASK_PRIORITY, MDL_VBI_TASK_STKSIZE,mdl_broadcast_msg);
    MT_ASSERT(ret == SUCCESS);
    vbi_inserter_set_insertion(VBI_INSERTION_TTX);
  }
  vbi_set_font_src(&ttx_font);
  
 #if 0
 init_sub_ttx_1();
  nc_handle = class_get_handle_by_id(NC_CLASS_ID);
  nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in);       
  vbi_set_ts_in(ts_in);
  subt_handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
  start_subt_dec_1(subt_handle);
 #else
   p_pb_pol->is_subtitle_on = TRUE;
#endif

  //vbi_ttx_start(TELTEXT_MAX_PAGE_NUM);
#endif

  return p_pb_pol;
}


void destruct_pb_policy(pb_policy_t *p_pb_policy)
{
  //Free private data
  mtos_free(p_pb_policy->p_data);

  //Free playback policy
  mtos_free(p_pb_policy);
}

