/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_pro_info.h"

enum prog_info_cont_ctrl_id
{
  IDC_STRE_BAR = 1,
  IDC_BER_BAR,
  IDC_SNR_BAR,  

  IDC_CHECK_VIDE,
};
/*
static comm_help_data_t pro_info_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/

u16 pro_info_root_keymap(u16 key);
RET_CODE pro_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_pro_info(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl,*p_bar;
  u16 str[]=
    {
      IDS_TYPE,       IDS_LCN,       IDS_NAME2,    IDS_SERVICE_ID2, 
      IDS_FREQUENCE2, IDS_TS_ID,     IDS_SYMBOL2, IDS_QAM,
      IDS_VIDEO_PID,  IDS_AUDIO_PID, IDS_PCR_PID2, IDS_TEL_PID,
    };
  u16 x=0, y=0, w=0, h=0, i=0, pg_id;
  u32 text_type = 0, align_type = 0;
  u8 view_id;
  u16 pos = 0;
  u16 uni_str[32],uni_num[32],uni_pro[64];
  u8 ansstr[16];
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp = {0};

  if ((pg_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
  {
    return ERR_FAILURE;
  }

  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }
  
  view_id = ui_dbase_get_pg_view_id();
  pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);

  sprintf((char*)ansstr, "%03d ",pos + 1);
  str_asc2uni(ansstr, uni_pro);
  
  memcpy(uni_str, pg.name, 32);
  uni_strcat(uni_pro, uni_str, uni_strlen(uni_str) + uni_strlen(uni_pro));

  if(db_dvbs_get_tp_by_id((u16)(pg.tp_id), &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  p_cont = ui_comm_root_create(ROOT_ID_PRO_INFO, 0,
                             COMM_BG_X,
                             COMM_BG_Y, COMM_BG_W,
                             COMM_BG_H, IM_TITLEICON_TV,
                             IDS_CHANNEL_INFORMATION);
  
  ctrl_set_keymap(p_cont, pro_info_root_keymap);
  ctrl_set_proc(p_cont, pro_info_root_proc);

  x = PRO_INFO_CHANNEL_TYPE_X;
  y = PRO_INFO_CHANNEL_TYPE_Y;
  h = PRO_INFO_CHANNEL_TYPE_H;
  for(i=0; i<24; i++)
  {
    switch(i%4)
    {      
      case 0:
        x = PRO_INFO_CHANNEL_TYPE_X;
        y = (u16)(PRO_INFO_CHANNEL_TYPE_Y+i/4*(PRO_INFO_CHANNEL_TYPE_H+PRO_INFO_ROW_VGAP));
        w = PRO_INFO_COLUMN0_W;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      case 1:
        x = PRO_INFO_CHANNEL_TYPE_X+PRO_INFO_COLUMN0_W+PRO_INFO_COLUMN_HGAP;
        w = PRO_INFO_COLUMN1_W;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT| STL_VCENTER;
        break;

      case 2:
        x = PRO_INFO_CHANNEL_TYPE_X+PRO_INFO_COLUMN0_W+PRO_INFO_COLUMN1_W+2*PRO_INFO_COLUMN_HGAP;
        w = PRO_INFO_COLUMN2_W;
        text_type = TEXT_STRTYPE_STRID;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      case 3:
        x = PRO_INFO_CHANNEL_TYPE_X+PRO_INFO_COLUMN0_W+PRO_INFO_COLUMN1_W+PRO_INFO_COLUMN2_W+3*PRO_INFO_COLUMN_HGAP;
        w = PRO_INFO_COLUMN3_W;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT| STL_VCENTER;
        break;
      default:
        break;
    }

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CHECK_VIDE+i),
                              x, y, w, h, p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i%2==0)
    {
      text_set_content_by_strid(p_ctrl, str[i/2]);
    }
    else
    {
      switch(i)
      {
        case 1://Type
          text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
          if(pg.tv_flag)
          {
            text_set_content_by_strid(p_ctrl, IDS_TV_CHANNEL);
          }
          else
          {
            text_set_content_by_strid(p_ctrl, IDS_RADIO_CHANNEL);
          }
          break;

        case 3://LCN
          text_set_content_type(p_ctrl, TEXT_STRTYPE_DEC);
          text_set_content_by_dec(p_ctrl, pg.logical_num);
          break;

        case 5://Name
          text_set_content_by_unistr(p_ctrl, pg.name);
          break;
          
        case 7://SERVICE ID
          sprintf((char*)ansstr, "%d ",pg.s_id);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
        
        case 9://Freq
          sprintf((char*)ansstr, "%4d KHz",(int)tp.freq);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
          
        case 11://TS ID
          sprintf((char*)ansstr, "%d ",(int)pg.ts_id);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
          
        case 13://Symbol
          sprintf((char*)ansstr, "%d KBaud",(int)tp.sym);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 15://QAM
          switch(tp.nim_modulate)
          {
            case 0:
              sprintf((char*)ansstr, "AUTO");
              break;
            case 1:
              sprintf((char*)ansstr, "BPSK");
              break;
            case 2:
              sprintf((char*)ansstr, "QPSK");
              break;
            case 3:
              sprintf((char*)ansstr, "8PSK");
              break;
            case 4:
              sprintf((char*)ansstr, "QAM16");
              break;
            case 5:
              sprintf((char*)ansstr, "QAM32");
              break;
            case 6:
              sprintf((char*)ansstr, "QAM64");
              break;
            case 7:
              sprintf((char*)ansstr, "QAM128");
              break;
            case 8:
              sprintf((char*)ansstr, "QAM256");  
              break;
            default:
              break;
          }
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 17://Video PID
          sprintf((char*)ansstr, "%d ",pg.video_pid);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 19://Audio PID
          sprintf((char*)ansstr, "%d ",pg.audio[pg.audio_channel].p_id);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
          
        case 21://PCR PID
          sprintf((char*)ansstr, "%d ",pg.pcr_pid);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
        
        case 23://Tel PID
          sprintf((char*)ansstr, "%d ",pg.pcr_pid);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;
                    
        default:
          break;
      }
    }
  }

  // signal strength pbar1  
  p_bar = ui_comm_bar_create(p_cont, IDC_STRE_BAR,
                               SIG_STRENGTH_PBAR_X,
                               SIG_STRENGTH_PBAR_Y,
                               SIG_STRENGTH_PBAR_W,
                               SIG_STRENGTH_PBAR_H,
                               SIG_STRENGTH_NAME_X, 
                               SIG_STRENGTH_NAME_Y, 
                               SIG_STRENGTH_NAME_W, 
                               SIG_STRENGTH_NAME_H,
                               SIG_STRENGTH_PERCENT_X,
                               SIG_STRENGTH_PERCENT_Y,
                               SIG_STRENGTH_PERCENT_W,
                               SIG_STRENGTH_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_RFLEVEL, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_ORANGE,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  // signal strength pbar2  
  p_bar = ui_comm_bar_create(p_cont, IDC_BER_BAR,
                               SIG_SET_PBAR_X,
                               SIG_SET_PBAR_Y,
                               SIG_SET_PBAR_W,
                               SIG_SET_PBAR_H,
                               SIG_SET_NAME_X, 
                               SIG_SET_NAME_Y, 
                               SIG_SET_NAME_W, 
                               SIG_SET_NAME_H,
                               SIG_SET_PERCENT_X,
                               SIG_SET_PERCENT_Y,
                               SIG_SET_PERCENT_W,
                               SIG_SET_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_BER, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_ORANGE,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
  
  // signal strength pbar3  
  p_bar = ui_comm_bar_create(p_cont, IDC_SNR_BAR,
                               SIG_SNR_PBAR_X,
                               SIG_SNR_PBAR_Y,
                               SIG_SNR_PBAR_W,
                               SIG_SNR_PBAR_H,
                               SIG_SNR_NAME_X, 
                               SIG_SNR_NAME_Y, 
                               SIG_SNR_NAME_W, 
                               SIG_SNR_NAME_H,
                               SIG_SNR_PERCENT_X,
                               SIG_SNR_PERCENT_Y,
                               SIG_SNR_PERCENT_W,
                               SIG_SNR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, IDS_CN, 0, 100, 100);
  ui_comm_bar_set_style(p_bar,
                          RSI_PROGRESS_BAR_BG, RSI_PROGRESS_BAR_MID_ORANGE,
                          RSI_IGNORE, FSI_WHITE,
                          RSI_PBACK, FSI_WHITE);
    
  //ui_comm_timedate_create(p_cont, (u16*)"", RSI_TITLE_TIME_BG);
  //ui_comm_timedate_update(p_cont, FALSE);

  //ui_comm_help_create(&pro_info_help_data, p_cont);
  
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  ui_enable_signal_monitor(TRUE);
  return SUCCESS;
}

static RET_CODE on_signal_info_exit(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_enable_signal_monitor(FALSE);
  return ERR_NOFEATURE;
}

static RET_CODE on_signal_info_update(control_t *p_cont,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  control_t *p_bar1, *p_bar2, *p_bar3;
  struct signal_data *data = (struct signal_data *)(para1);

  p_bar1 = ctrl_get_child_by_id(p_cont, IDC_STRE_BAR);
  ui_comm_tp_bar_update(p_bar1, data->intensity, TRUE, (u8*)"dBuv");
  p_bar2 = ctrl_get_child_by_id(p_cont, IDC_SNR_BAR);
  ui_comm_tp_bar_update(p_bar2, data->quality, TRUE, (u8*)"dB");
  p_bar3 = ctrl_get_child_by_id(p_cont, IDC_BER_BAR);
  ui_comm_tp_bar_update(p_bar3, data->ber, TRUE, (u8*)"E-6");
  
  ctrl_paint_ctrl(p_bar1, TRUE);
  ctrl_paint_ctrl(p_bar2, TRUE);
  ctrl_paint_ctrl(p_bar3, TRUE);
  return SUCCESS;
}

BEGIN_KEYMAP(pro_info_root_keymap, ui_comm_root_keymap)
  //ON_EVENT(V_KEY_BACK, MSG_EXIT)
  //ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(pro_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(pro_info_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SIGNAL_UPDATE, on_signal_info_update)
  ON_COMMAND(MSG_EXIT, on_signal_info_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_signal_info_exit)
  //ON_COMMAND(MSG_EXIT, on_exit_pro_info)
END_MSGPROC(pro_info_root_proc, ui_comm_root_proc)

