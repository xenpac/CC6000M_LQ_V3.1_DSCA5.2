/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_api.h"
#include "cas_manager.h"

#ifdef ENABLE_CA
u16 ui_ca_evtmap(u32 event);


char * g_cardtype_str[] = 
{
  "AutoDetect",
  "Irdeto Card",
  "Viaccess Card",
  "Nagra Card",
  "Seca Card",
  "Conax Card",
  "Cryptowork Card",
  "Shl Card",
  "Dreamcrypt Card",
  "Xcrypto Card",
  "NDS Card",
  "Nagra2 Card",
  "Tricolor Card",
  "Tongfang Card",
  "Sumavision Card",
  "Tianbai Card",
  "Unknown Card",
};

#define SUPPORT_CARDTYPE_MAX_CNT (sizeof(g_cardtype_str)/sizeof(char*))
static  cmd_t cmd = {0};
#if 0
int snprintf(char *string, size_t count, const char *format, ...) 
{
  char temp[1024] = {0};
  int ret = 0;
  
  ret = sprintf(temp, format);

  memcpy(string, temp, count);
  string[count] = 0;

  return ret;
}
#endif

static void ui_add_rootid_to_ca_api_register_msg(void)
{
    u32 i = 0;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if(NULL == handle)
    {
        return;
    }
    for(i = 0;i < handle->attr_number;i ++)
    {
      fw_register_ap_msghost(APP_CA, handle->ca_menu_attr[i].root_id);
    }
}

static void ui_add_rootid_to_ca_api_unregister_msg(void)
{
    u32 i = 0;
    cas_manager_policy_t *handle = NULL;
    handle = get_cas_manager_policy_handle();
    if(NULL == handle)
    {
        return;
    }
    for(i = 0;i < handle->attr_number;i ++)
    {
      fw_unregister_ap_msghost(APP_CA, handle->ca_menu_attr[i].root_id);
    }
}
#endif
void ui_init_ca(void)
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
      return;
  }
  #ifdef ENABLE_CA
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_CA;
  #ifndef WIN32
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  #endif
  fw_register_ap_evtmap(APP_CA, ui_ca_evtmap);
  fw_register_ap_msghost(APP_CA, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_CA, ROOT_ID_PASSWORD);
  fw_register_ap_msghost(APP_CA, ROOT_ID_INFO);
  ui_add_rootid_to_ca_api_register_msg();
 #endif
}


void ui_release_ca(void)
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
      return;
  }
  #ifdef ENABLE_CA
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_CA;
  #ifndef WIN32
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  #endif
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_BACKGROUND);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_PASSWORD);
  fw_unregister_ap_msghost(APP_CA, ROOT_ID_INFO);
  ui_add_rootid_to_ca_api_unregister_msg();
  fw_unregister_ap_evtmap(APP_CA);
  #endif
}

void ui_ca_do_cmd(u32 cmd_id, u32 para1,u32 para2)
{
  cas_manager_policy_t *handle = NULL;
  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
      return;
  }
  #ifdef ENABLE_CA
  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | handle->ap_cas_id);
  #ifndef WIN32
  ap_frm_do_command(APP_CA, &cmd);
  #endif
  #endif
}

void ui_ca_set_language(language_set_t lang_set)
{
  switch(lang_set.osd_text)
  {
    case 0:
      ui_ca_do_cmd(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_ENGLISH, 0);
      break;
    case 1:
      ui_ca_do_cmd(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_CHINESE, 0);
      break;
    default:
      ui_ca_do_cmd(CAS_CMD_SET_CA_LANGUAGE, CAS_LANGUAGE_CHINESE, 0);
      break;
  }
}

#ifdef ENABLE_CA
BEGIN_AP_EVTMAP(ui_ca_evtmap)
  CONVERT_EVENT(CAS_EVT_RST_SUCCESS,   MSG_CA_INIT_OK)
  CONVERT_EVENT(CAS_EVT_OPERATOR_INFO, MSG_CA_OPE_INFO)
  CONVERT_EVENT(CAS_EVT_ENTITLE_INFO, MSG_CA_ENT_INFO)
  CONVERT_EVENT(CAS_EVT_IPPV_INFO, MSG_CA_IPV_INFO)
  CONVERT_EVENT(CAS_EVT_BURSE_INFO, MSG_CA_BUR_INFO)
  CONVERT_EVENT(CAS_EVT_CARD_INFO, MSG_CA_CARD_INFO)
  CONVERT_EVENT(CAS_EVT_PAIRED_STA, MSG_CA_PAIRE_INFO)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_INFO, MSG_CA_MON_CHILD_INFO)
  CONVERT_EVENT(CAS_EVT_MAIL_HEADER_INFO, MSG_CA_MAIL_HEADER_INFO)
  CONVERT_EVENT(CAS_EVT_MAIL_BODY_INFO, MSG_CA_MAIL_BODY_INFO)
  CONVERT_EVENT(CAS_EVT_WORK_TIME_INFO, MSG_CA_WORK_TIME_INFO)
  CONVERT_EVENT(CAS_EVT_WORK_TIME_SET, MSG_CA_WORK_TIME_SET)
 // CONVERT_EVENT(CAS_EVT_SYSTEM_TIME_INFO, MSG_CA_SYSTEM_TIME_INFO)
  
  CONVERT_EVENT(CAS_EVT_ANNOUNCE_HEADER_INFO, MSG_CA_ANNOUNCE_HEADER_INFO)
  CONVERT_EVENT(CAS_EVT_ANNOUNCE_BODY_INFO, MSG_CA_ANNOUNCE_BODY_INFO)
  CONVERT_EVENT(CAS_EVT_PIN_SET, MSG_CA_PIN_SET_INFO)
  CONVERT_EVENT(CAS_EVT_NOTIFY, MSG_CA_EVT_NOTIFY)
  CONVERT_EVENT(CAS_EVT_RATING_SET, MSG_CA_RATING_SET)

  CONVERT_EVENT(CAS_EVT_IPP_REC_INFO, MSG_CA_IPP_REC_INFO)
  CONVERT_EVENT(CAS_EVT_IPP_BUY_RESULT, MSG_CA_IPP_BUY_OK)
  CONVERT_EVENT(CAS_EVT_PIN_VERIFY, MSG_CA_PIN_VERIFY_INFO)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_FEED, MSG_CA_MON_CHILD_FEED)
  CONVERT_EVENT(CAS_EVT_SHOW_FINGER, MSG_CA_FINGER_INFO)
  CONVERT_EVENT(CAS_EVT_SHOW_ECM_FINGER, MSG_CA_ECM_FINGER_INFO)
  CONVERT_EVENT(CAS_EVT_HIDE_FINGER, MSG_CA_HIDE_FINGER)

  CONVERT_EVENT(CAS_EVT_PPV_SYNC_INFO,MSG_CA_OPEN_IPPV_PPT)
  CONVERT_EVENT(CAS_EVT_PPV_BUY_INFO, MSG_CA_IPPV_BUY_INFO)
   #ifdef SMSX_CA
  CONVERT_EVENT(CAS_EVT_SHOW_OSD_DOWN, MSG_CA_SHOW_OSD)
#endif
#ifdef TENGRUI_CA
  CONVERT_EVENT(CAS_EVT_SHOW_ECM_FINGER, MSG_CA_FINGER_INFO)
  CONVERT_EVENT(CAS_EVT_SHOW_IPP_BUY_INFO, MSG_CA_IPP_BUY_INFO)
  CONVERT_EVENT(CAS_EVT_HIDE_PPV_BUY_INFO, MSG_CA_IPP_HIDE_BUY_INFO)
  CONVERT_EVENT(CAS_EVT_SENIOR_PREVIEW_SHOW,MSG_CA_STB_NOTIFICATION)
#elif defined(SZXC_423_CA)
  CONVERT_EVENT(CAS_EVT_SHOW_IPP_BUY_INFO, MSG_CA_OPEN_IPPV_PPT)
  CONVERT_EVENT(CAS_EVT_HIDE_PPV_BUY_INFO,MSG_CA_HIDE_IPPV_PPT)
//  CONVERT_EVENT(CAS_EVT_IPP_PROG_NOTIFY,MSG_CA_IPP_PROG_NOTIFY)
#else
  CONVERT_EVENT(CAS_EVT_SHOW_IPP_BUY_INFO, MSG_CA_IPP_BUY_INFO)
  CONVERT_EVENT(CAS_EVT_HIDE_PPV_BUY_INFO, MSG_CA_IPP_HIDE_BUY_INFO)
#endif


  CONVERT_EVENT(CAS_EVT_FORCE_CHANNEL_INFO, MSG_CA_FORCE_CHANNEL_INFO)
 CONVERT_EVENT(CAS_EVT_UNFORCE_CHANNEL_INFO, MSG_CA_UNFORCE_CHANNEL_INFO)
 CONVERT_EVENT(CAS_EVT_MAIL_DEL_RESULT, MSG_CA_MAIL_DEL_RESULT) 
  CONVERT_EVENT(CAS_EVT_ANNOUNCE_DEL_RESULT, MSG_CA_ANNOUNCE_DEL_RESULT)
  CONVERT_EVENT(CAS_EVT_SHOW_OSD_MSG, MSG_CA_SHOW_OSD)
#if defined DESAI_56_CA || defined DESAI_40_CA
  CONVERT_EVENT(CAS_EVT_HIDE_FORCE_OSD, MSG_CA_HIDE_FORCE_OSD)
 #endif

#ifdef YXSB_CA
  CONVERT_EVENT(CAS_EVT_HIDE_OSD, MSG_CA_HIDE_OSD)
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_UP, MSG_CA_HIDE_OSD_UP)
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_DOWN, MSG_CA_HIDE_OSD_DOWN)
#else
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_UP, MSG_CA_HIDE_OSD)
  CONVERT_EVENT(CAS_EVT_HIDE_OSD_DOWN, MSG_CA_HIDE_OSD)
#endif
#ifndef WIN32
  CONVERT_EVENT(CAS_EVT_ECM_UPDATE, MSG_CA_ECM_UPDATE)
#endif
  CONVERT_EVENT(CAS_EVT_RATING_INFO, MSG_CA_RATING_INFO)
  CONVERT_EVENT(CAS_EVT_UNLOCK_PARENTAL_RESULT, MSG_CA_UNLOCK_PARENTAL)
  CONVERT_EVENT(CAS_EVT_ACLIST_INFO, MSG_CA_ACLIST_INFO)
  CONVERT_EVENT(CAS_EVT_PLATFORM_ID, MSG_CA_PLATFORM_ID)
  CONVERT_EVENT(CAS_EVT_READ_FEED_DATA, MSG_CA_READ_FEED_DATA)

  CONVERT_EVENT(CAS_EVT_IEXIGENT_PROG_STOP, MSG_CA_STOP_CUR_PG)
  CONVERT_EVENT(CAS_EVT_CARD_NEED_UPDATE, MSG_CA_CARD_NEED_UPDATE)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_BEGIN, MSG_CA_CARD_UPDATE_BEGIN)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_PROGRESS, MSG_CA_CARD_UPDATE_PROGRESS)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_ERR, MSG_CA_CARD_UPDATE_ERR)
  CONVERT_EVENT(CAS_EVT_CARD_UPDATE_END, MSG_CA_CARD_UPDATE_END)
  //CONVERT_EVENT(CAS_EVT_SOFTWARE_UPDATE_PROGRESS, MSG_SOFTWARE_UPDATE)
  //CONVERT_EVENT(CAS_EVT_SHOW_ECM_FINGER, MSG_CA_ECM_FINGER_INFO)
  CONVERT_EVENT(CAS_EVT_SHOW_AUTHEN_EXPIRE_DAY, MSG_CA_AUTHEN_EXPIRE_DAY)
  CONVERT_EVENT(CAS_CONDITION_SEARCH, MSG_CA_CONDITION_SEARCH)
  CONVERT_EVENT(CAS_EVT_SHOW_BURSE_CHARGE, MSG_CA_SHOW_BURSE_CHARGE)
  CONVERT_EVENT(CAS_EVT_SHOW_ERROR_PIN_CODE, MSG_CA_SHOW_ERROR_PIN_CODE)
  CONVERT_EVENT(CAS_EVT_MON_CHILD_IDENTIFY,MSG_CA_JUDGE_MOTHER_CARD)
  CONVERT_EVENT(CAS_EVT_DEBUG_EN,MSG_CA_DEBUG)
  CONVERT_EVENT(CAS_EVT_SOFTWARE_UPDATE_PROGRESS, MSG_CA_OTA_UPDATE_ABV)
  CONVERT_EVENT(CAS_EVT_CONTINUE_WATCH_LIMIT, MSG_CA_CONTINUE_WATCH_LIMIT)
  CONVERT_EVENT(CAS_EVT_STB_NOTIFICATION, MSG_CA_STB_NOTIFICATION)
  
#ifndef WIN32
  CONVERT_EVENT(CAS_EVT_SHOW_URGENT_MSG, MSG_CA_SHOW_URGENT_MSG)
  CONVERT_EVENT(CAS_EVT_HIDE_URGENT_MSG, MSG_CA_HIDE_URGENT_MSG)  
#endif
  
  CONVERT_EVENT(CAS_EVT_FORCE_MSG,MSG_CA_FORCE_MSG)
  CONVERT_EVENT(CAS_EVT_HIDE_FORCE_MSG,MSG_CA_HIDE_FORCE_MSG)
END_AP_EVTMAP(ui_ca_evtmap);
#endif
