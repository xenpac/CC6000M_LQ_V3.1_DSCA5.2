/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_info.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#ifdef ENABLE_CA
#include "config_cas.h"
#endif

enum info_ctrl_id
{
  IDC_INFO_LIST = 1,
};

static list_xstyle_t info_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_SH,
};

static list_xstyle_t info_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t info_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t info_list_attr[INFO_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    330, 5, 0, &info_list_field_rstyle,  &info_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    280,400, 0, &info_list_field_rstyle,  &info_list_field_fstyle},       
};
/*
static comm_help_data_t info_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
#ifdef ONLY1_CA_VER
u16 ui_info_root_keymap(u16 key);
RET_CODE ui_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#endif
RET_CODE info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE info_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{
  u8 i;
  #ifndef TEMP_SUPPORT_DS_AD
  #ifdef ONLY1_CA_VER
  u16 str_id[9] = 
  #else
  u16 str_id[INFO_LIST_CNT] = 
  #endif
    { 
    #ifndef CAS_CONFIG_ABV
    IDS_CUR_STBID,
    #endif
    IDS_MANUFACTURE_ID,
    IDS_HARDWARE_VER,
    IDS_SOFTWARE_VER,
    IDS_DOWNLOAD_DATE,
    IDS_LOADER_VER,
    #ifdef ONLY1_CA_VER
    IDS_HARDWARE_MOD,
    IDS_SOFTWARE_MOD,
    IDS_CA_CARD_VER,
    #endif
    };
   #else
    u16 str_id[7] =
    {    
    IDS_CUR_STBID, 
    IDS_MANUFACTURE_ID,
    IDS_HARDWARE_VER,
    IDS_SOFTWARE_VER,
    IDS_DOWNLOAD_DATE,
    IDS_LOADER_VER,
    IDS_ADS_VERSION,
    }; 
   #endif
  u32 oui = 0;
  misc_options_t misc;
  sw_info_t sw_info;
  u8 ansstr[64];
  #ifdef PLAAS_DEXIN_STB_ID_COMBINE
  stb_id_com_t sys_stb_id = {{0},{0},{0},0};
  u8 *p_stb_id = NULL;
  
  sys_status_get_stb_id_info(&sys_stb_id);
  
  if(sys_stb_id.stb_id_select == STB_ID_ORIGINAL)
    p_stb_id = sys_stb_id.original_stb_id;
  else if(sys_stb_id.stb_id_select == STB_ID_COMBINED1)
    p_stb_id = sys_stb_id.combined_stb_id1;
  else
    p_stb_id = sys_stb_id.combined_stb_id2;
#endif
  dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

  oui = misc.ota_tdi.oui;
  
  sys_status_get_sw_info(&sw_info);
  #ifndef TEMP_SUPPORT_DS_AD
  #ifdef ONLY1_CA_VER
  for(i = 0; i < 9; i++)
  #else
  for(i = 0; i < INFO_LIST_CNT; i++)
  #endif
  {
    list_set_field_content_by_strid(p_list, i, 0, str_id[i]);
  }
  #else
  for(i = 0; i < 7; i++)
  {
    list_set_field_content_by_strid(p_list, i, 0, str_id[i]);	
  }
  #endif
  #ifdef CAS_CONFIG_ABV
  {
    extern RET_CODE abv_software_version_get(u8 *p_buf, u32 size);
    extern RET_CODE abv_hardware_version_get(u8 *p_buf, u32 size);
    extern RET_CODE abv_maucode_get(u8 *p_buf, u32 size);
    u8 buf[4] = {0};

    abv_maucode_get(buf, 1);
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "0X%.2X", buf[0]);
    list_set_field_content_by_ascstr(p_list, 0, 1, ansstr);

    abv_hardware_version_get(buf, 4);
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "0X%.2X%.2X%.2X%.2X", buf[3], buf[2], buf[1], buf[0]);
    list_set_field_content_by_ascstr(p_list, 1, 1, ansstr);

    abv_software_version_get(buf, 4);
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "0X%.2X%.2X%.2X%.2X", buf[3], buf[2], buf[1], buf[0]);
    list_set_field_content_by_ascstr(p_list, 2, 1, ansstr);
      
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%s   %s",__DATE__,__TIME__);
    list_set_field_content_by_ascstr(p_list, 3, 1, ansstr);
    
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "1010");
    list_set_field_content_by_ascstr(p_list, 4, 1, ansstr);
  }
  #else
  {
    
    //sys_status_t *p_sys_status = sys_status_get();
    u8 local_changeset[MAX_CHANGESET_LEN];
    memset(local_changeset, 0, sizeof(local_changeset));
    dm_read(class_get_handle_by_id(DM_CLASS_ID), SS_DATA_BLOCK_ID, 0, 0, MAX_CHANGESET_LEN, local_changeset);	
    #ifndef CAS_CONFIG_CDCAS
    {
      memset(ansstr, 0, sizeof(ansstr));
      if(sys_get_serial_num(ansstr, sizeof(ansstr)))
      {
        #ifdef PLAAS_DEXIN_STB_ID_COMBINE
         list_set_field_content_by_ascstr(p_list, 0, 1, p_stb_id);
        #else
        list_set_field_content_by_ascstr(p_list, 0, 1, ansstr);
        #endif
      }
    }
    #endif

    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%lx", oui);
    list_set_field_content_by_ascstr(p_list, 1, 1, ansstr);
    
    memset(ansstr, 0, sizeof(ansstr));
    #ifdef ONLY1_CA_VER 	
    sprintf((char*)ansstr, "0x0001");//hw version
    #else
    sprintf((char*)ansstr, "%x", misc.ota_tdi.hw_version);
    #endif
    list_set_field_content_by_ascstr(p_list, 2, 1, ansstr);
    OS_PRINTF("\nhw version%d\n",misc.ota_tdi.hw_version);
	
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%lx%s", ui_ota_api_get_upg_check_version(), local_changeset);
    list_set_field_content_by_ascstr(p_list, 3, 1, ansstr);
    
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%s   %s",__DATE__,__TIME__);
    list_set_field_content_by_ascstr(p_list, 4, 1, ansstr);
	
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "1010");
    list_set_field_content_by_ascstr(p_list, 5, 1, ansstr);
    #ifdef ONLY1_CA_VER 
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "0x0201");// hw model
    list_set_field_content_by_ascstr(p_list, 6, 1, ansstr);
    OS_PRINTF("\nhw version%d\n",misc.ota_tdi.hw_mod_id);
	
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "0x0001");// sw model
    list_set_field_content_by_ascstr(p_list, 7, 1, ansstr);
    OS_PRINTF("\nhw version%d\n",misc.ota_tdi.sw_mod_id);
    #endif
    #ifdef TEMP_SUPPORT_DS_AD
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%s", ads_get_ad_version());	
    list_set_field_content_by_ascstr(p_list, 6, 1, ansstr);
    #endif
  }
  #endif
  return SUCCESS;
}

RET_CODE open_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list;
  u8 i;

  p_cont =
    ui_comm_root_create(ROOT_ID_INFO, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, IM_TITLEICON_TV, IDS_INFORMATION);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  #ifdef ONLY1_CA_VER
  ctrl_set_keymap(p_cont, ui_info_root_keymap);
  ctrl_set_proc(p_cont, ui_info_root_proc);
  #else
  ctrl_set_proc(p_cont, info_proc);
  #endif
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_INFO_LIST, 
    INFO_LIST_X, INFO_LIST_Y, INFO_LIST_W, INFO_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_INFO_LIST, RSI_INFO_LIST, RSI_INFO_LIST);
  ctrl_set_mrect(p_list, INFO_LIST_MID_L, INFO_LIST_MID_T,
    INFO_LIST_MID_L + INFO_LIST_MID_W, INFO_LIST_MID_T + INFO_LIST_MID_H);
  list_set_item_interval(p_list, INFO_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &info_list_item_rstyle);
  list_enable_select_mode(p_list, FALSE);
  list_set_focus_pos(p_list, 0);
  #ifndef TEMP_SUPPORT_DS_AD
  #ifdef ONLY1_CA_VER
  list_set_count(p_list, 9, 9);
  list_set_field_count(p_list, INFO_LIST_FIELD, 9);
  #else
  list_set_count(p_list, INFO_LIST_CNT, INFO_LIST_CNT);
  list_set_field_count(p_list, INFO_LIST_FIELD, INFO_LIST_CNT);
  #endif
  #else
  list_set_count(p_list, 7, 7);
  list_set_field_count(p_list, INFO_LIST_FIELD, 7);
  #endif
  list_set_update(p_list, info_list_update, 0); 
 
  for (i = 0; i < INFO_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(info_list_attr[i].attr), (u16)(info_list_attr[i].width),
                        (u16)(info_list_attr[i].left), (u8)(info_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, info_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, info_list_attr[i].fstyle);
  } 
  #ifndef TEMP_SUPPORT_DS_AD
  #ifdef ONLY1_CA_VER
  info_list_update(p_list, list_get_valid_pos(p_list), 9, 0);
  #else
  info_list_update(p_list, list_get_valid_pos(p_list), INFO_LIST_CNT, 0);
  #endif
  #else
  info_list_update(p_list, list_get_valid_pos(p_list), 7, 0);
  #endif
  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  #ifdef ENABLE_CA
  ui_ca_do_cmd(CAS_CMD_PLATFORM_ID_GET, 0 ,0);
  #endif
  #ifndef WIN32
  #ifdef ONLY1_CA_VER
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
  #endif
  #endif
  return SUCCESS;
}

static RET_CODE on_info_stb_id(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
 #ifdef CAS_CONFIG_CDCAS
  u8 buf[6] = {0};
  u8 ascstr[14] = {0};
  u16 *plat_id = (u16 *)para2;
  control_t *p_list = ctrl_get_child_by_id(p_cont, IDC_INFO_LIST);

  UI_PRINTF("on_conditional_accept_info_plat_id plat_id:0x%x\n", *plat_id);
  buf[0] = (*plat_id) >> 8;
  buf[1] = (*plat_id) & (0xff);
  
  if(machine_serial_get((u8 *)&buf[2], CDCAS_CA_STDID_SIZE/2) == SUCCESS)
  {
    sprintf((char*)ascstr, "0x%02x%02x%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
  }
  UI_PRINTF("0x%02x%02x%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
  list_set_field_content_by_ascstr(p_list, 0, 1, ascstr);
#endif
  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE ui_info_card_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list;
  u32 ca_ver = 0;
  u8 ansstr[32];
  cas_card_info_t *p_card_info = (cas_card_info_t *)para2;
  p_list = ctrl_get_child_by_id(p_cont, IDC_INFO_LIST);
  ca_ver = (p_card_info->cas_ver[3] << 24) | ( p_card_info->cas_ver[2] << 16) | (p_card_info->cas_ver[1] << 8)| p_card_info->cas_ver[0];
  UI_PRINTF("cas = %d %x\n",ca_ver,ca_ver);
  sprintf((char*)ansstr, "%ld", ca_ver);
  list_set_field_content_by_ascstr(p_list, 8, 1, ansstr);
  ctrl_paint_ctrl(p_list, TRUE);
  return SUCCESS;
}
BEGIN_KEYMAP(ui_info_root_keymap, ui_comm_root_keymap)
END_KEYMAP(ui_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ui_info_root_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, ui_info_card_info_update)
END_MSGPROC(ui_info_root_proc, ui_comm_root_proc)

BEGIN_MSGPROC(info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_PLATFORM_ID, on_info_stb_id)
END_MSGPROC(info_proc, ui_comm_root_proc)

