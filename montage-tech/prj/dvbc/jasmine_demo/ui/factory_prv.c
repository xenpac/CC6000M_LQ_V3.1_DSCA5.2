/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
//#include "hal_misc.h"
#include "ui_common.h"
#include "ui_mainmenu.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_prog_bar.h"
//#include "ui_ca_info.h"
#include "ui_nvod_api.h"

//#include "ui_finger_print.h"
//#include "ui_new_mail.h"
//#include "ui_ca_public.h"
#include "lib_char.h"
#include "lib_unicode.h"
#include "drv_dev.h"
#include "uio.h"
#include "class_factory.h"

enum factory_cont_ctrl_id
{
  IDC_FACTORY_INFO_START = 1,
  IDC_CHECK_START = 20,
  IDC_PASS_BTN = 40,
};

#define FACTORY_ITME_X   200
#define FACTORY_ITME_Y   70
#define FACTORY_ITME_H   36

#define FACTORY_COLUMN0_W    300
#define FACTORY_COLUMN1_W    260
#define FACTORY_COLUMN2_W    100

#define FACTORY_COLUMN_HGAP  6
#define FACTORY_ROW_VGAP     20

#define FACTORY_BTN_COUNT    7


BOOL factory_mode_flag = FALSE;
static BOOL check_state[FACTORY_BTN_COUNT];

static void restore_to_factory(void);
u16 ui_desktop_keymap_on_factory(u16 key);
RET_CODE ui_desktop_proc_on_factory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 factory_mode_cont_keymap(u16 key);
RET_CODE factory_mode_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

u16 factory_mode_state_check_keymap(u16 key);
RET_CODE factory_mode_state_check_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

RET_CODE open_factory_mode_ui(u32 para1, u32 para2);

extern BOOL ap_uio_translate_key(v_key_t *key);
extern void ap_uio_init_kep_map(void);

static void restore_to_factory(void)
{
  u16 view_id;
  utc_time_t  p_time;
  BOOL is_scart = FALSE;

  // set front panel
  ui_set_front_panel_by_str("----");
  ui_stop_play(STOP_PLAY_BLACK, TRUE);


  // disable epg at first
//  ui_epg_stop();
//  ui_epg_release();

  // kill tmr

  //set default use common buffer as ext buffer
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
  sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
  sys_status_load();

  if(is_scart)
 {
 	sys_status_set_status(BS_IS_SCART_OUT, TRUE);
 }
  sys_status_check_group();
  sys_status_save();
  
  // set environment according ss_data
  sys_status_reload_environment_setting(TRUE);

  sys_status_get_utc_time( &p_time);
  time_set(&p_time);

  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }
  if (ui_is_notify())
  {
    ui_set_notify(NULL, NOTIFY_TYPE_STRID, RSC_INVALID_ID, 0);
  }

  // send cmd to ap frm and wait for finish

  //create a new view after load default, and save the new view id.
  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id((u8)view_id);
}

#ifdef DALIAN_ZHUANGGUANG
void divi_restore_to_factory(void)
{
  restore_to_factory();
}
#endif

BOOL get_factory_flag(void)
{
  return factory_mode_flag;
  //return TRUE;
}

void check_factory_key(void)
{
  uio_device_t *p_dev = NULL;
  u8 cnk = 0;
  v_key_t p_key = {0, INVALID_KEY, INVALID_KEY, INVALID_KEY, INVALID_KEY, IRDA_KEY};
  BOOL factory_flag = FALSE;
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_UIO);

  ap_uio_init_kep_map();
#if 0
  factory_mode_flag = TRUE;
#else
  factory_mode_flag = FALSE;
#endif
  sys_status_get_status(BS_FACTORY_ENTERED, &factory_flag);
  OS_PRINTF("factory_flag = %d\n",factory_flag);
  if(factory_flag == TRUE)
  {
    factory_mode_flag = TRUE;
  }
  else
  {
   //OS_PRINTF("#####cgf debug into ckeck factory \n");
    for(cnk = 0;cnk < 100;cnk ++)
    {
      p_key.type = IRDA_KEY;

      if(uio_get_code(p_dev, &p_key.v_key) == SUCCESS)
      {
          OS_PRINTF("#####cgf debug get uio code \n");
          if((p_key.v_key & 0x0100) == 0)
            {
                p_key.usr = (p_key.v_key >> (UIO_USR_CODE_SHIFT + 8)) & 0x3;
                p_key.v_key &= ((p_key.v_key) & 0xFF);
                //OS_PRINTF("#####cgf debug get uio code 0x%d \n",p_key.v_key);
                ap_uio_translate_key(&p_key);
                //OS_PRINTF("#####cgf debug get uio code 0x%d key=%d \n",p_key.v_key,V_KEY_BLUE);
                if(p_key.v_key == V_KEY_BLUE)
                  {
                      factory_mode_flag = TRUE;
                      OS_PRINTF("check factory key is push\n");
                      break;
                  }
            }
      }
      mtos_task_delay_ms(10);
    }
  }
}
//lint -e732
void db_factory_program(void)
{
  u8 vv_id_tp = 0 ,vv_id_pro = 0;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  u32 cur_tp_id = 0;
  u8 pgname[32] = {0};

  vv_id_pro = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  vv_id_tp = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);

  tp.freq = 315000;
  tp.sym = 6875;
  tp.nim_modulate = NIM_MODULA_QAM64;

  sprintf((char *)pgname,"%s",(u8 *)"test");

  db_dvbs_add_tp(vv_id_tp,&tp) ;
  cur_tp_id = tp.id;
  pg.s_id = 1;
  pg.ts_id = 2241;
  pg.orig_net_id = 1999;
  pg.pcr_pid = 0x1ff;
  pg.video_pid = 0x1ff;
  pg.is_scrambled = 0;
  pg.ca_system_id = 0;
  pg.pmt_pid = 0x80;
  pg.audio_ch_num = 1;
  pg.audio[0].p_id = 0x200;
  pg.audio[0].type = 2;
  pg.tp_id = cur_tp_id;
  //pg.volume = 0x10;
  pg.volume = 31;
  pg.service_type = SVC_TYPE_TV;
  pg.audio_track = 1;
  pg.tv_flag = (pg.video_pid != 0) ? 1 : 0;
  pg.skp_flag = 0;
  pg.mosaic_flag = 0;

  dvb_to_unicode(pgname, sizeof(pgname),pg.name, DB_DVBS_MAX_NAME_LENGTH+1);
  db_dvbs_add_program(vv_id_pro, &pg) ;
  db_dvbs_save(vv_id_tp);
  db_dvbs_save(vv_id_pro);

}
//lint +e732

void ui_factory_init(void)
{
  u8 curn_mode, vid;
  u16 type;
  u32 context;
  ss_public_t *ss_public;

  OS_PRINTF("into factory mode\n");
  //ui_desktop_init();

  // restore the setting
  sys_status_reload_environment_setting(TRUE);
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
  db_factory_program();

  //set sw version.
  ss_public = ss_ctrl_get_public(class_get_handle_by_id(SC_CLASS_ID));
  OS_PRINTF("get ver %d\n", sys_status_get_sw_version());
  ss_public->otai.orig_software_version = (u16)sys_status_get_sw_version();
  ss_ctrl_update_public(ss_public);

  // try to play
  sys_status_check_group();
  curn_mode = sys_status_get_curn_prog_mode();
  if(curn_mode != CURN_MODE_NONE)
  {
    sys_status_get_curn_view_info(&type, &context);
    vid = ui_dbase_create_view((dvbs_view_t)type, context, NULL);
    ui_dbase_set_pg_view_id(vid);
  }

  db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), 31);

  open_factory_mode_ui(0, 0);

  // goto loop
  ui_desktop_main();
}

static RET_CODE on_check_signal_factory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_signal_check(para1, para2);
  return SUCCESS;
}

static RET_CODE on_change_state_check(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 index = (u8)(p_ctrl->id-IDC_CHECK_START);
  u8 i = 0;
  BOOL b_show_pass = TRUE;

  if(p_ctrl->id == IDC_CHECK_START+FACTORY_BTN_COUNT-1)
  {
    restore_to_factory();
    #ifndef WIN32
    hal_pm_reset();
    #endif
  }
  else
  {
    check_state[index] = !(check_state[index]);

    if(check_state[index])
    {
      text_set_content_by_ascstr(p_ctrl, (u8*)"pass");
    }
    else
    {
      text_set_content_by_ascstr(p_ctrl, (u8*)"fail");
    }

    for(i=0; i<FACTORY_BTN_COUNT-1; i++)
    {
      if(!check_state[i])
      {
        b_show_pass = FALSE;
        break;
      }
    }

    if(b_show_pass)
    {
      ctrl_set_attr(ctrl_get_child_by_id(p_ctrl->p_parent,IDC_CHECK_START+FACTORY_BTN_COUNT-1), OBJ_ATTR_ACTIVE);
      ctrl_set_sts(ctrl_get_child_by_id(p_ctrl->p_parent,IDC_CHECK_START+FACTORY_BTN_COUNT-1), OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
    }
    else
    {
      ctrl_set_sts(ctrl_get_child_by_id(p_ctrl->p_parent,IDC_CHECK_START+FACTORY_BTN_COUNT-1), OBJ_STS_HIDE);
      ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
    }
  }
  return SUCCESS;
}


RET_CODE open_factory_mode_ui(u32 para1, u32 para2)
{
  control_t *p_cont, *p_btn[FACTORY_BTN_COUNT];
  control_t *p_ctrl;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  u8 str[][32]=
    {
      "Hardware ver:",
      "Software ver1:",
      "Software ver2:",
      "SN:",
      "Card number:",
      "Output:",
    };

  u16 x, y, w=0, h, i;
  u32 text_type = 0, align_type = 0;
  u16 uni_num[256];
  u8  ansstr[64];

  misc_options_t misc;
  sw_info_t sw_info;

  dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

  sys_status_get_sw_info(&sw_info);

  /* create */
  p_cont = fw_create_mainwin(ROOT_ID_FACTORY_MODE,100,100,(1280-2*100),(720-2*100),
                            ROOT_ID_INVALID,0,OBJ_ATTR_ACTIVE,0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_BLACK, RSI_BLACK,RSI_BLACK);
  ctrl_set_keymap(p_cont, factory_mode_cont_keymap);
  ctrl_set_proc(p_cont, factory_mode_cont_proc);

  x = FACTORY_ITME_X;
  y = FACTORY_ITME_Y;
  h = FACTORY_ITME_H;

  for(i=0; i<12; i++)
  {
    switch(i%2)
    {
      case 0:
        x = FACTORY_ITME_X;
        y = (u16)(FACTORY_ITME_Y+i/2*(FACTORY_ITME_H+FACTORY_ROW_VGAP));
        w = FACTORY_COLUMN0_W;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      case 1:
        x = FACTORY_ITME_X+FACTORY_COLUMN0_W+FACTORY_COLUMN_HGAP;
        w = FACTORY_COLUMN1_W;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      default:
        break;
    }

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_FACTORY_INFO_START+i),
                              x, y, w, h, p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i%2==0)
    {
      text_set_content_by_ascstr(p_ctrl, str[i/2]);
    }
    else
    {
      switch(i)
      {
        case 1://Hardware Version
          //memcpy(ansstr, misc.str_hw_version, sizeof(misc.str_hw_version));
          sprintf((char*)ansstr, "%s","V1");
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 3://Software Version
          sprintf((char*)ansstr, "%s",misc.str_sw_version);
          str_asc2uni(ansstr, uni_num);
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 5://Sofeware
          sprintf((char*)ansstr, "v%04ld",sys_status_get_sw_version());
          text_set_content_by_ascstr(p_ctrl, ansstr);
          break;

        case 7://SN
          //if(sys_get_serial_num(ansstr, (u8)sizeof(ansstr)))
          {
            //str_asc2uni((u8 *)ansstr, uni_num);
          }
          //else
          {
            str_asc2uni((u8 *)"NULL", uni_num);
          }
          text_set_content_by_unistr(p_ctrl, uni_num);
          break;

        case 9://card number
          text_set_content_by_ascstr(p_ctrl, (u8*)" ");
          break;

        default:
          break;
      }
    }
  }


  x = FACTORY_ITME_X+FACTORY_COLUMN0_W+FACTORY_COLUMN1_W;
  h = FACTORY_ITME_H;
  w = FACTORY_COLUMN2_W;
  text_type = TEXT_STRTYPE_UNICODE;
  align_type = STL_CENTER | STL_VCENTER;

  for(i=0; i<FACTORY_BTN_COUNT; i++)
  {
    y = (u16)(FACTORY_ITME_Y+i*(FACTORY_ITME_H+FACTORY_ROW_VGAP));
    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CHECK_START+i),
                              x, y, w, h, p_cont, 0);
    ctrl_set_keymap(p_btn[i], factory_mode_state_check_keymap);
    ctrl_set_proc(p_btn[i], factory_mode_state_check_proc);
    ctrl_set_rstyle(p_btn[i], RSI_GRAY, RSI_BLUE, RSI_GRAY);
    text_set_font_style(p_btn[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_btn[i], align_type);
    text_set_content_type(p_btn[i], text_type);

    ctrl_set_related_id(p_btn[i],
                        (u8)((i - 1 + FACTORY_BTN_COUNT)%FACTORY_BTN_COUNT+IDC_CHECK_START),
                        (u8)((i - 1 + FACTORY_BTN_COUNT)%FACTORY_BTN_COUNT+IDC_CHECK_START),
                        (u8)((i + 1) % FACTORY_BTN_COUNT)+IDC_CHECK_START,
                        (u8)((i + 1) % FACTORY_BTN_COUNT)+IDC_CHECK_START);

    switch(i)
    {
      case 0:
      case 1:
      case 2:
        ctrl_set_sts(p_btn[i], OBJ_STS_HIDE);
        check_state[i] = TRUE;
        text_set_content_by_ascstr(p_btn[i], (u8*)"pass");
        break;

      case (FACTORY_BTN_COUNT-1)://PASS BTN
        ctrl_set_sts(p_btn[i], OBJ_STS_HIDE);
        check_state[i] = FALSE;
        text_set_content_by_ascstr(p_btn[i], (u8*)"PASS");
        break;

      default:
        check_state[i] = FALSE;
        text_set_content_by_ascstr(p_btn[i], (u8*)"null");
        break;
    }

  }

  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CHECK_START+FACTORY_BTN_COUNT-4), MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, FALSE);
  
  ui_play_prog(prog_id, FALSE);
  
  //ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);

  return SUCCESS;
}


BOOL enter_factory_mode(void)
{
  BOOL ret = FALSE;
  if(get_factory_flag())
  {
    //ui_set_desktop_key_and_proc(ui_desktop_keymap_on_factory, ui_desktop_proc_on_factory);
    // restore keymap
    fw_set_keymap(ui_desktop_keymap_on_factory);
    fw_set_proc(ui_desktop_proc_on_factory);

    ui_factory_init();
    ret = TRUE;
  }

  return ret;
}
/*
static void factory_ca_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_card_num = ctrl_get_child_by_id(p_cont, IDC_FACTORY_INFO_START+9);

  u8 asc_str[32];
  u16 uni_num[64];
  
  if(p_card_info == NULL)
  {
    text_set_content_by_unistr(p_card_num, (u16*)" ");
  }
  else
  {
    //cars sn
    sprintf((char*)asc_str, "%s", p_card_info->sn);
    
    str_asc2uni(asc_str, uni_num);
    text_set_content_by_unistr(p_card_num, uni_num);
  }
}

static RET_CODE on_factory_ca_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  factory_ca_info_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_factory_ca_info_accept_notify(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u32 event_id = 0;
  
  switch(msg)
  {
    case MSG_CA_INIT_OK:
      ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0, 0);
      break;

    case MSG_CA_EVT_NOTIFY:
      event_id = para2;
      if(event_id == CAS_S_ADPT_CARD_REMOVE || event_id == CAS_C_DETITLE_ALL_READED)
      {
        ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0, 0);
      }
      break;
  }

  return SUCCESS;
}
*/
// start key mapping on normal
BEGIN_KEYMAP(factory_mode_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(factory_mode_cont_keymap, NULL)

// start message mapping
BEGIN_MSGPROC(factory_mode_cont_proc, cont_class_proc)
  //ON_COMMAND(MSG_CA_CARD_INFO, on_factory_ca_info_update)
  //ON_COMMAND(MSG_CA_EVT_NOTIFY, on_factory_ca_info_accept_notify)
  //ON_COMMAND(MSG_CA_INIT_OK, on_factory_ca_info_accept_notify)
END_MSGPROC(factory_mode_cont_proc, cont_class_proc)

// start key mapping on normal
BEGIN_KEYMAP(factory_mode_state_check_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(factory_mode_state_check_keymap, NULL)

// start message mapping
BEGIN_MSGPROC(factory_mode_state_check_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_change_state_check)
END_MSGPROC(factory_mode_state_check_proc, text_class_proc)


// start key mapping on normal
BEGIN_KEYMAP(ui_desktop_keymap_on_factory, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ui_desktop_keymap_on_factory, NULL)

// start message mapping
BEGIN_MSGPROC(ui_desktop_proc_on_factory, cont_class_proc)
  ON_COMMAND(MSG_SIGNAL_CHECK, on_check_signal_factory)
END_MSGPROC(ui_desktop_proc_on_factory, cont_class_proc)


