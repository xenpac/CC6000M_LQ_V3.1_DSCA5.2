/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "hal_misc.h"
#include "mtos_msg.h"
#include "ui_common.h"
#include "nim.h"
#include "pti.h"
#include "dvb_svc.h"
//#include "ap_ota.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_api_v2.h"
#include "ota_dm_api.h"
#include "ota_public_def.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "../../includes/customer_config.h"

enum ota_upgrade_ctrl_id
{
  IDC_OTA_UPGRADE_STATE = 1,
  IDC_OTA_UPGRADE_NOTES,
  IDC_OTA_UPGRADE_PBAR,
  IDC_OTA_UPGRADE_PER,
  IDC_OTA_UPGRADE_TTL,
};

extern u8 serial_num[64];
extern u8 ui_get_com_num(void);

static BOOL is_burning_state = FALSE;

extern u32 get_flash_addr(void);
extern void ota_read_otai(ota_info_t *p_otai);

u16 ota_upgrade_keymap(u16 key);
RET_CODE ota_upgrade_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

s32 open_ota_upgrade(u32 para1, u32 para2)
{
  struct control *p_cont, *p_state, *p_notes, *p_pbar;
  struct control *p_per, *p_title;

  p_cont = fw_create_mainwin(ROOT_ID_OTA_UPGRADE,
    OTA_UPGRADE_CONTX, OTA_UPGRADE_CONTY,
    OTA_UPGRADE_CONTW, OTA_UPGRADE_CONTH,
    0, 0, OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  ctrl_set_proc(p_cont, ota_upgrade_proc);
  ctrl_set_keymap(p_cont, ota_upgrade_keymap);

  p_state = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_STATE,
    OTA_UPGRADE_STATEX, OTA_UPGRADE_STATEY,
    OTA_UPGRADE_STATEW, OTA_UPGRADE_STATEH,
    p_cont, 0);
  ctrl_set_rstyle(p_state, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_state, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_state, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_state, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_state, IDS_OTA_SEARCHING);

  p_notes = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_NOTES,
    OTA_UPGRADE_NOTESX, OTA_UPGRADE_NOTESY,
    OTA_UPGRADE_NOTESW, OTA_UPGRADE_NOTESH,
    p_cont, 0);
  ctrl_set_rstyle(p_notes, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_notes, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_notes, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_notes, TEXT_STRTYPE_STRID);

  p_pbar = ctrl_create_ctrl(CTRL_PBAR, IDC_OTA_UPGRADE_PBAR,
    OTA_UPGRADE_PBARX, OTA_UPGRADE_PBARY,
    OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH,
    p_cont, 0);
  ctrl_set_rstyle(p_pbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_pbar, 0, 0, OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH);
  pbar_set_rstyle(p_pbar, RSI_OTA_UPGRADE_PBAR_MIN,
    RSI_OTA_UPGRADE_PBAR_MAX, RSI_OTA_UPGRADE_PBAR_MIN);
  pbar_set_count(p_pbar, OTA_UPGRADE_PBAR_MIN,
    OTA_UPGRADE_PBAR_MAX, OTA_UPGRADE_PBAR_MAX);
  pbar_set_direction(p_pbar, 1);
  pbar_set_workmode(p_pbar, 1, 0);
  pbar_set_current(p_pbar, 0);

  p_per = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_PER,
    OTA_UPGRADE_PERX, OTA_UPGRADE_PERY,
    OTA_UPGRADE_PERW, OTA_UPGRADE_PERH,
    p_cont, 0);
  ctrl_set_rstyle(p_per, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_per, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_per, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_per, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_per, "0%");

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_TTL,
    OTA_UPGRADE_TTLX, OTA_UPGRADE_TTLY,
    OTA_UPGRADE_TTLW, OTA_UPGRADE_TTLH,
    p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_OTA_UPGRADE);

  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

static RET_CODE on_ota_upgarde_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ota_bl_info_t bl_info = {0};
    
  mul_ota_dm_api_read_bootload_info(&bl_info);
  
  OS_PRINTF("\r\n[OTA]%s:upg ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);
  
  bl_info.ota_status = OTA_TRI_MODE_NONE;
  bl_info.load_block_id = MAINCODE_BLOCK_ID;
  mul_ota_dm_api_save_bootload_info(&bl_info);
   
#ifndef WIN32
  mtos_task_delay_ms(1000);

  //hal_watchdog_enable();
  hal_watchdog_init(0);
  hal_watchdog_enable();

  //hal_pm_reset();
#endif

  return SUCCESS;
}

static RET_CODE on_ota_upgrade_step(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_pbar, *p_per;
  u8 ascstr[8];
  u8 string_pro[8];
  void *p_dev = NULL;
  u8 num = 0;
   
	p_pbar = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PBAR);
	p_per = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PER);

	pbar_set_current(p_pbar, (u16)para1);
	ctrl_paint_ctrl(p_pbar, TRUE);

	//OS_PRINTF("percent [%d]", para1);
    memset(string_pro, 0, sizeof(string_pro));
    num = ui_get_com_num();
    if(num == 3)
    {
      if(para2 == 0)
      {
        if(para1 == 100)
          sprintf((char*)string_pro, "%03d", (int)para1);
        else
          sprintf((char*)string_pro, "D%02d", (int)para1);
      }
      else if(para2 == 1)
      {
        if(para1 == 100)
          sprintf((char*)string_pro, "%03d", (int)para1);
        else
          sprintf((char*)string_pro, "B%02d", (int)para1);
      }
    }
    else
    {
      if(para2 == 0)
      {
        if(para1 == 100)
          sprintf((char*)string_pro, " %03d", (int)para1);
        else
          sprintf((char*)string_pro, " D%02d", (int)para1);
      }
      else if(para2 == 1)
      {
        if(para1 == 100)
          sprintf((char*)string_pro, " %03d", (int)para1);
        else
          sprintf((char*)string_pro, " B%02d", (int)para1);
      }
    }
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

      //OS_PRINTF("%s\n", string_pro);
    //OS_PRINTF("1111111111111111num=%d,string_pro=%s\n",num,string_pro);
    uio_display(p_dev, string_pro, num);
      
    sprintf(ascstr, "%d%%", (int)para1);
	text_set_content_by_ascstr(p_per, ascstr);
	ctrl_paint_ctrl(p_per, TRUE);

	return SUCCESS;
}

static RET_CODE on_ota_unzip_ok(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

 // ui_ota_burn_flash(TRUE);

  /*comm_dlg_data_t dlg_data =
    {
      ROOT_ID_OTA_UPGRADE,
      DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
      0,
      0,
    };
  u16 uni_str[32];
  ota_info_t ota_info = {0};

  str_asc2uni("Uzip Ok, Burn Flash Now?", uni_str);

  dlg_data.content = (u32)uni_str;

  ota_read_otai(&ota_info);
  if((OTA_TRI_MON_IN_LOADER == ota_info.ota_tri)
      ||(OTA_TRI_FORC == ota_info.ota_tri))
  {
    ui_ota_burn_flash(TRUE);
    return SUCCESS;
  }

  if(DLG_RET_YES == ui_comm_dlg_open(&dlg_data))
  {
    ui_ota_burn_flash(TRUE);
  }
  else
  {
    ui_ota_burn_flash(FALSE);
  }*/

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_start_download(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_OTA_DOWNLOADING);
  ctrl_paint_ctrl(p_states, TRUE);
#if 1//#ifndef CONFIG_CUSTOMER
  text_set_content_by_strid(p_notes, IDS_PRESS_EXIT_TO_EXIT_UPGRADE_AND_RESTART);
#endif
  ctrl_paint_ctrl(p_notes, TRUE);


	return SUCCESS;
}

static RET_CODE on_ota_upgrade_checking(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_CHECKING_UPGRADE_FILE);
  ctrl_paint_ctrl(p_states, TRUE);

  //mtos_task_sleep(3000);

	return SUCCESS;
}

static RET_CODE on_ota_upgrade_burning(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;

  is_burning_state = TRUE;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_BURNING_FLASH);
  ctrl_paint_ctrl(p_states, TRUE);

  text_set_content_by_strid(p_notes, IDS_DONT_POWER_OFF);
  ctrl_paint_ctrl(p_notes, TRUE);

	return SUCCESS;
}

static void _keep_serial_num()
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  //dm_v2_init_para_t dm_para = { 0 };
  u32 indentity_block_addr;
  u8 *p_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
  charsto_prot_status_t st_set = {0};
  charsto_prot_status_t st_old = {0};

  memset(p_buffer, 0, CHARSTO_SECTOR_SIZE);
#if 0
  /* init data manager */
  dm_destory_v2();
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.test_mode = FALSE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);

  //dm_init_v2(&dm_para);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_BOOTER_START_ADDR);
#endif
  //unprotect
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  //indentity_block_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID) - get_flash_addr();
  indentity_block_addr = mul_ota_dm_api_get_block_addr(IDENTITY_BLOCK_ID);
  if(charsto_read(p_charsto_dev, (indentity_block_addr / 0x10000 * 0x10000), p_buffer, CHARSTO_SECTOR_SIZE) == SUCCESS)
  {
    memcpy(p_buffer + (indentity_block_addr % 0x10000), serial_num, sizeof(serial_num));
    if(charsto_erase(p_charsto_dev, (indentity_block_addr / 0x10000 * 0x10000), 1) == SUCCESS)
    {
      charsto_writeonly(p_charsto_dev, (indentity_block_addr / 0x10000 * 0x10000), p_buffer, CHARSTO_SECTOR_SIZE);
    }
  }
  //restore
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  mtos_free(p_buffer);
}

static RET_CODE on_ota_upgrade_finish(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_notes, *p_states;
  ota_bl_info_t bl_info = {0};

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_OTA_UPGRADE_COMPLETE);
  ctrl_paint_ctrl(p_states, TRUE);

  _keep_serial_num();

    
  mul_ota_dm_api_read_bootload_info(&bl_info);
  
  OS_PRINTF("\r\n[OTA]%s:upg ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);
  
  bl_info.ota_status = OTA_TRI_MODE_NONE;
  bl_info.load_block_id = MAINCODE_BLOCK_ID;
  mul_ota_dm_api_save_bootload_info(&bl_info);

  if(para2 == OTA_TRI_MODE_FORC)
  {
    text_set_content_by_strid(p_notes, IDS_STB_FACTORY_COMPLETE);
    ctrl_paint_ctrl(p_notes, TRUE);
  }
  else
  {
    text_set_content_by_strid(p_notes, IDS_STB_WILL_AUTO_RESET_THEN);
    ctrl_paint_ctrl(p_notes, TRUE);
    mtos_task_sleep(1000);

  	#ifndef WIN32

    //hal_watchdog_enable();
	hal_watchdog_init(0);
	hal_watchdog_enable();

    //hal_pm_reset();
  	#else
    while(1)
    {
      mtos_task_sleep(1000);
    }
  	#endif
  }
	return SUCCESS;
}

BEGIN_KEYMAP(ota_upgrade_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
END_KEYMAP(ota_upgrade_keymap, NULL)

BEGIN_MSGPROC(ota_upgrade_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ota_upgarde_exit)
  ON_COMMAND(MSG_OTA_PROGRESS, on_ota_upgrade_step)
  ON_COMMAND(MSG_OTA_UNZIP_OK, on_ota_unzip_ok)
  ON_COMMAND(MSG_OTA_START_DL, on_ota_upgrade_start_download)
  ON_COMMAND(MSG_OTA_CHECKING, on_ota_upgrade_checking)
  ON_COMMAND(MSG_OTA_BURNING, on_ota_upgrade_burning)
  ON_COMMAND(MSG_OTA_FINISH, on_ota_upgrade_finish)
END_MSGPROC(ota_upgrade_proc, cont_class_proc);

