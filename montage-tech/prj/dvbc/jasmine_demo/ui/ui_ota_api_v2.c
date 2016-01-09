/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include <math.h>
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "charsto.h"
#include "dvb_protocol.h"
#include "ui_common.h"
#include "ota_dm_api.h"
#include "ota_public_def.h"
#include "ota_api.h"
#include "ui_ota_api_v2.h"


#define OTA_API_DEBUG

#ifdef OTA_API_DEBUG
  #ifdef PRINT_ON
  #define OTA_API_DBG_PRINTF OS_PRINTF
  #else
  #define OTA_API_DBG_PRINTF NULL
  #endif 
#else
#define OTA_API_DBG_PRINTF DUMMY_PRINTF
#endif


static BOOL g_auto_mode = FALSE;
//static BOOL g_manual_force = FALSE;
static dm_dmh_info_t all_dmh_info;
static BOOL g_ota_in_maincode = TRUE;
static u8 *p_ota_api_buffer = NULL;
static BOOL g_ota_api_buf_free = FALSE;
//static BOOL g_factory_force = FALSE;  

extern void virtual_nim_filter_register_do_lock_result(void *function);
extern void virtual_nim_filter_lock_start(u8 lock_mode,u32 *p_para);
extern void virtual_nim_filter_lock_stop(void);
extern void virtual_nim_filter_start(void);
extern void virtual_nim_filter_stop(void);
u16 ui_ota_api_evtmap(u32 event);
void  ui_ota_api_save_do_ota_tp(nim_info_t *tp_info);

static void ui_ota_reset_block_data(u8 block_id)
{
  void *p_dev = NULL;
  u8 sec_cnt;
  u32 block_addr  = 0;
  u32 block_size = 0;
  u8 i = 0,j = 0 ;
  BOOL block_exist_flag = FALSE;
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);

  for(i = 0;i < all_dmh_info.header_num;i ++)
  {
     for(j = 0; j < all_dmh_info.header_group[i].block_num; j++)
      {
         if((all_dmh_info.header_group[i].dm_block[j].id == block_id)
            && ((all_dmh_info.header_group[i].dm_block[j].base_addr % CHARSTO_SECTOR_SIZE) == 0)
            && ((all_dmh_info.header_group[i].dm_block[j].size % CHARSTO_SECTOR_SIZE) == 0)
            && (all_dmh_info.header_group[i].dm_block[j].base_addr != 0)
            && (all_dmh_info.header_group[i].dm_block[j].size != 0))
          {
            block_addr = all_dmh_info.header_group[i].dm_block[j].base_addr;
            block_size = all_dmh_info.header_group[i].dm_block[j].size;
            block_exist_flag = TRUE;
            break;
          }
      }
     if(TRUE == block_exist_flag)
      {
        break;
      }
  }

  if(FALSE == block_exist_flag)
  {
    return;
  }

  if((0 != (block_size % CHARSTO_SECTOR_SIZE ))
  || (0 != (block_addr % CHARSTO_SECTOR_SIZE)))
  {
    return;
  }
  
  sec_cnt = (u8)(block_size / CHARSTO_SECTOR_SIZE);

  {
    charsto_prot_status_t st_old;
    charsto_prot_status_t st_set;
    memset(&st_old, 0, sizeof(charsto_prot_status_t));
    memset(&st_set, 0, sizeof(charsto_prot_status_t));
    dev_io_ctrl(p_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);

    //unprotect
    st_set.prt_t = PRT_UNPROT_ALL;
    dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

    charsto_erase(p_dev, block_addr, sec_cnt);

    //restore
    dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  }
  return;
}

void ui_ota_dm_api_init(void)
{
  ota_dm_config_t p_cfg = {0};
  dm_dmh_info_t *p_all_dmh_info = NULL;
  #if 0
  p_cfg.ota_dm_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_BLOCK_ID) - get_flash_addr();
  p_cfg.ota_dm_backup_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_RESERVE_BLOCK_ID) - get_flash_addr();

  OS_PRINTF("#### debug dm addr:0x%x,dm backup addr:0x%x\n",
                              p_cfg.ota_dm_addr,
                              p_cfg.ota_dm_backup_addr);
  #endif
  p_cfg.is_use_mutex = TRUE;
  p_cfg.mutex_prio = 1;
  p_cfg.is_protect = TRUE;
  p_cfg.disable_backup_block = FALSE;
  p_cfg.debug_level = OTA_DM_DEBUG_ALL;
  p_cfg.ota_dm_api_printf = NULL;
  p_cfg.align_size = PICE_MAX_ALIGN_SIZE;
  p_cfg.flash_size = CHARSTO_SIZE;
  p_cfg.pp_dm_info = &p_all_dmh_info;
  p_cfg.ota_dm_block_id = STATIC_SAVE_DATA_BLOCK_ID;
  p_cfg.ota_dm_backup_block_id =STATIC_SAVE_DATA_RESERVE_BLOCK_ID;
  
  
  OS_PRINTF("#####debug ui ota dm api  init\n");
  mul_ota_dm_api_init(&p_cfg);

  /****test use ,please don't use in project,but factory upg*****/
 //mul_ota_dm_api_reset_data_block();   
}
void ui_ota_api_bootload_info_init(void)
{
    ota_bl_info_t bl_info = {0};
    
    bl_info.ota_number = 2;
    bl_info.ota_curr_id = OTA_BLOCK_ID;
    bl_info.ota_backup_id = OTA1_BLOCK_ID;
    bl_info.load_block_id = MAINCODE_BLOCK_ID;

    bl_info.destroy_flag = FALSE;
    bl_info.ota_status = OTA_TRI_MODE_NONE;
    bl_info.fail_times = 0;
    bl_info.medium_type = OTA_MEDIUM_BY_TUNER;
    mul_ota_dm_api_init_bootload_info(&bl_info);
}

void ui_ota_api_ota_param_init(void)
{
   ota_param_t ota_param;
   //dm_dmh_info_t all_dmh_info;
   BOOL ret = FALSE;
   u8 i = 0, j = 0, k = 0;
   misc_options_t misc = {0};

   ret = (BOOL)dm_read(class_get_handle_by_id(DM_CLASS_ID), MISC_OPTION_BLOCK_ID, 0, 
                           0, sizeof(misc_options_t), (u8*)&misc);
   MT_ASSERT(ret != ERR_FAILURE);

   memset(&ota_param, 0, sizeof(ota_param_t));
   sys_status_get_ota_preset(&ota_param.ota_tp);
   if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
   {
     if(ota_param.ota_tp.lock_mode != SYS_DTMB || ota_param.ota_tp.lockt.tp_freq == 0)
     {
       ota_param.ota_tp.lock_mode = SYS_DTMB;
       ota_param.ota_tp.lockt.tp_freq = SEARCH_FREQ_NIT;
       ota_param.ota_tp.lockt.nim_type = NIM_DTMB;
       ota_param.ota_tp.lockt.band_width = DTMB_HN_SYMBOL_DEFAULT;
       ota_param.ota_tp.data_pid = 0x1b58; /*default 7000 */
     }
   }
   /****init in ota block,the data is do ota tp data,but in app,it from system status init data*/

   memcpy(&ota_param.ota_set_tp,&ota_param.ota_tp,sizeof(nim_para_t));
   memcpy(&ota_param.ota_set_back_tp,&ota_param.ota_tp,sizeof(nim_para_t));

   memset(&all_dmh_info,0,sizeof(dm_dmh_info_t));

   /**virtual all flash or maincode block***/
   ota_param.block_table[0].id = OTA_ALL_MAIN_CODE_BLOCK_ID; 
   ota_param.block_table[0].type = 0;
   ota_param.block_table[0].base_addr = 0;
   ota_param.block_table[0].size = CHARSTO_SIZE;
   ota_param.block_table[0].ota_ver = 0;
   ota_param.block_table[0].exist_bit = TRUE;
   ota_param.reserved1 = 1;
   
   ret = mul_ota_dm_api_find_dmh_info_from_flash(0,CHARSTO_SIZE,&all_dmh_info);
   if(ret == TRUE)
    {
       for(i = 0;i < all_dmh_info.header_num;i ++)
        {
           for(j = 0; j < all_dmh_info.header_group[i].block_num; j++)
            {
               if((all_dmh_info.header_group[i].dm_block[j].id != OTA_ALL_MAIN_CODE_BLOCK_ID)
                  && ((all_dmh_info.header_group[i].dm_block[j].base_addr % CHARSTO_SECTOR_SIZE) == 0)
                  && ((all_dmh_info.header_group[i].dm_block[j].size % CHARSTO_SECTOR_SIZE) == 0)
                  && (all_dmh_info.header_group[i].dm_block[j].base_addr != 0)
                  && (all_dmh_info.header_group[i].dm_block[j].size != 0))
                {
                        for(k= 1;k < OTA_DM_MAX_BLOCK_NUMBER;k++)
                        {
                          if(ota_param.block_table[k].exist_bit != TRUE)
                            {
                              ota_param.block_table[k].id = all_dmh_info.header_group[i].dm_block[j].id;
                              ota_param.block_table[k].type = all_dmh_info.header_group[i].dm_block[j].type;
                              ota_param.block_table[k].base_addr = all_dmh_info.header_group[i].dm_block[j].base_addr;
                              ota_param.block_table[k].size = all_dmh_info.header_group[i].dm_block[j].size;
                              ota_param.block_table[k].ota_ver = 0;
                              ota_param.block_table[k].exist_bit = TRUE;
                              break;
                            }
                        }
                }
            }
        }
    }
   OS_PRINTF(" ota_param.upg_version sw_version%d\n", misc.ota_tdi.sw_version);

   ota_param.upg_version = misc.ota_tdi.sw_version;
   mul_ota_dm_api_init_ota_param(&ota_param);
}

//lint -e438 -e550 -e830
void ui_ota_api_evt_process(u32 content, u32 para1, u32 para2,u32 context)
{
    event_t evt = {0};
    BOOL  send_flag = FALSE;
    static u16 process = 0;
    static u32 upg_new_version = 0;
    
    evt.id = OTA_EVT_UNKOWN;
    evt.data1 = para1;
    evt.data2 = para2;
   
    switch(content)
    {
      case OTA_API_EVENT_LOCK:
      OS_PRINTF("###debug [%s]:locked\n",__FUNCTION__);
      evt.id = OTA_EVT_LOCKED;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_UNLOCK:
      OS_PRINTF("###debug [%s]:un lock\n",__FUNCTION__);
      evt.id = OTA_EVT_UNLOCKED;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_CHECK_FAIL:
      OS_PRINTF("###debug [%s]:check fail\n",__FUNCTION__);
      evt.id = OTA_EVT_CHECK_FAIL;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_CHECK_SUCCESS:
      {
        upg_check_info_t *p_upg_info = NULL;
        nim_info_t *p_do_ota_tp = NULL;
        p_upg_info = (upg_check_info_t *)para1;
        p_do_ota_tp = (nim_info_t *)para2;
        OS_PRINTF("###debug [%s]:check secccs new version%d\n",
                                  __FUNCTION__,
                                  p_upg_info->upg_new_version);
        ui_ota_api_save_do_ota_tp(p_do_ota_tp);
        if((g_auto_mode == TRUE) &&  (OTA_WORK_SELECT_CHECK == context))
        {
          ota_bl_info_t bl_info = {0};
          mul_ota_dm_api_read_bootload_info(&bl_info);
          if(bl_info.ota_status != OTA_TRI_MODE_FORC)
          {
          bl_info.ota_status = OTA_TRI_MODE_AUTO;
          bl_info.fail_times = 0;
          bl_info.destroy_flag = FALSE;
          }
          mul_ota_dm_api_save_bootload_info(&bl_info);
        }
        evt.id = OTA_EVT_CHECK_SUCESS;
      }
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_DOWNLOAD_FAIL: 
      OS_PRINTF("###debug [%s]:download fail\n",__FUNCTION__);
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_DOWNLOAD_START:
      OS_PRINTF("###debug [%s]:download start\n",__FUNCTION__);
      evt.id =OTA_EVT_START_DL;
      send_flag = TRUE;
      process = 0;
      break;
      case OTA_API_EVENT_DOWNLOAD_ING:
      evt.id = OTA_EVT_PROGRESS;
      if((para1 != process) && (para1 <= 100))
      {
        send_flag = TRUE;
        process = (u16)para1;
        OS_PRINTF("###debug [%s]:download progress %d\n",__FUNCTION__,(u8)para1);   
       }
      break;
      case OTA_API_EVENT_DOWNLOAD_SUCCESS:
      OS_PRINTF("###debug [%s]:download success\n",__FUNCTION__);
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_BURNFLASH_START: 
      OS_PRINTF("###debug [%s]:burn start\n",__FUNCTION__);
      evt.id = OTA_EVT_BURN_START;
      send_flag = TRUE;
      process = 0;
      break;
      case OTA_API_EVENT_BURNFLASH_ING:
      evt.id = OTA_EVT_PROGRESS;
      if((para1 != process) && (para1 <= 100))
      {
        send_flag = TRUE;
        process = (u16)para1;
		OS_PRINTF("###debug [%s]:burning progress %d\n",__FUNCTION__,(u8)para1);
      }
      break;
      case OTA_API_EVENT_BURNFLASH_BLOCK:
      OS_PRINTF("###debug [%s]:burn block id: 0x%x\n",__FUNCTION__,(u8)para1);
      if(para1 == OTA_ALL_MAIN_CODE_BLOCK_ID)
      {
         /***all flash force will don't save block!but only use dm api****/
         if(para2 == TRUE)  
          {
             //g_factory_force = TRUE;
             mul_ota_dm_api_forbid();
          }
      }
      send_flag = TRUE;
      break;
      
      case OTA_API_EVENT_BURNFLASH_SAVE_OTA_FAIL_TIME:
      OS_PRINTF("###debug [%s]:ota fail time: %d\n",__FUNCTION__,(u8)para1);
      mul_ota_dm_api_save_ota_fail_times(para1);
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_OTA_STATUS:
      OS_PRINTF("###debug [%s]:ota trigger mode: %d\n",__FUNCTION__,(u8)para1);
      mul_ota_dm_api_save_ota_status(para1);
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_OTA_DSTROY_FLAGE:
      OS_PRINTF("###debug [%s]:ota destroy flage: %d,block:0x%x\n",
                            __FUNCTION__,
                            (u8)para1,para2);
      #if 0
      mul_ota_dm_api_save_flash_destroy_flag((BOOL)para1);
      mul_ota_dm_api_save_flash_destroy_block_id(para2);
      #else
        {
           ota_bl_info_t bl_info = {0};
           mul_ota_dm_api_read_bootload_info(&bl_info);
           bl_info.destroy_block_id = para2;
           bl_info.destroy_flag = (BOOL)para1;
           mul_ota_dm_api_save_bootload_info(&bl_info);
        }
      #endif
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_CURR_OTA_ID:
      OS_PRINTF("###debug [%s]:ota current id : 0x%x, back up id:0x%x\n",
                              __FUNCTION__,
                              para1,para2);
      mul_ota_dm_api_save_ota_block_id(para1,para2);
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_LOAD_BLOCK_ID:
      OS_PRINTF("###debug [%s]:save load block id : 0x%x\n",__FUNCTION__,para1);
      mul_ota_dm_api_save_load_block_id(para1);  
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_BLOCK_VERSION_INFO:
      { 
        ota_block_info_t evt_save_block = {0};
        block_ota_info_t dm_save_block = {0};
        memcpy(&evt_save_block,(ota_block_info_t *)para1,sizeof(ota_block_info_t));
        mul_ota_dm_api_read_one_block_info(evt_save_block.id,&dm_save_block);
        dm_save_block.id = evt_save_block.id;
        dm_save_block.type= evt_save_block.type;
        dm_save_block.ota_ver = evt_save_block.ota_new_ver;
        dm_save_block.base_addr = evt_save_block.base_addr;
        dm_save_block.size = evt_save_block.size;
        dm_save_block.crc = evt_save_block.crc;
        dm_save_block.exist_bit = TRUE;
        mul_ota_dm_api_save_one_block_info(evt_save_block.id,&dm_save_block);
      }
      break;
      case OTA_API_EVENT_BURNFLASH_SAVE_UPG_VERSION:
      {
        OS_PRINTF("###debug [%s]:ota upg version : %d\n",__FUNCTION__,para1);
        mul_ota_dm_api_save_ota_param_upg_version(para1);
        upg_new_version = para1;
      }
      break;
      case  OTA_API_EVENT_BURNFLASH_FACTORY:
      /****if need save ota param,but if no save same param*****/
      
      OS_PRINTF("###debug [%s]:factory ota secessful\n",__FUNCTION__);
      /****if need restart,need save version,if not restart on line,don't do it*****/
      mul_ota_dm_api_destroy();
      ui_ota_dm_api_init();
      ui_ota_api_bootload_info_init();
      ui_ota_api_ota_param_init();
      /******save other param,eg:upg version,block version******/
      if(upg_new_version)
        mul_ota_dm_api_save_ota_param_upg_version(upg_new_version);
      evt.id = OTA_EVT_UPG_COMPLETE;
      //evt.data2 = OTA_TRI_MODE_FORC;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_NO_BLOCK_UPG:
      {
        ota_bl_info_t bl_info = {0};
        mul_ota_dm_api_read_bootload_info(&bl_info);
        if(bl_info.ota_status != OTA_TRI_MODE_FORC)
        {
          bl_info.ota_status = OTA_TRI_MODE_NONE;
          bl_info.fail_times = 0;
          bl_info.destroy_flag = FALSE;
        }
        mul_ota_dm_api_save_bootload_info(&bl_info);
      }
      evt.id = OTA_EVT_UPG_COMPLETE;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_BURNFLASH_RESET_SYS_DATA_BLOCK:
      OS_PRINTF("###debug [%s]:reset sys data block\n",__FUNCTION__);
      ui_ota_reset_block_data(IW_VIEW_BLOCK_ID);
      break;
      case OTA_API_EVENT_BURNFLASH_RESET_DB_DATA_BLOCK:
      OS_PRINTF("###debug [%s]:reset db data block\n",__FUNCTION__);
      ui_ota_reset_block_data(IW_TABLE_BLOCK_ID);
      break;
      case OTA_API_EVENT_BURNFLASH_RESET_CA_DATA_BLOCK:
      OS_PRINTF("###debug [%s]:reset CA data block\n",__FUNCTION__);
      ui_ota_reset_block_data(CADATA_BLOCK_ID);
      break;
       case OTA_API_EVENT_BURNFLASH_RESET_ADS_DATA_BLOCK:
      OS_PRINTF("###debug [%s]:reset ads data block\n",__FUNCTION__);
      break;
      case OTA_API_EVENT_BURNFLASH_FAIL:
      OS_PRINTF("###debug [%s]:burn fail\n",__FUNCTION__);
      evt.id = OTA_EVT_BURN_FAILED;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_BURNFLASH_SUCCESS:
      OS_PRINTF("###debug [%s]:burn success\n",__FUNCTION__);
      break;
      case OTA_API_EVENT_FINISH_REBOOT:
      evt.id = OTA_EVT_UPG_COMPLETE;
      send_flag = TRUE;
      OS_PRINTF("###debug [%s]:burn reboot\n",__FUNCTION__);
      break;

      case OTA_API_REQUEST_TIMEOUT:
      OS_PRINTF("###debug [%s]:time out\n",__FUNCTION__);
      evt.id = OTA_EVT_TABLE_TIMEOUT;
      send_flag = TRUE;
      break;
      case OTA_API_EVENT_STOP_RESULT:
      OS_PRINTF("###debug [%s]:stop states:%d\n",__FUNCTION__,para1);
      break;
      default :
      OS_PRINTF("###debug [%s]:unkown msg\n",__FUNCTION__);
      send_flag = FALSE;    
      break;
    }

    /***send_flag = TRUE:set ui root ,else it process self****/
    if(TRUE == send_flag)
    {
      ap_frm_send_evt_to_ui(APP_OTA, &evt);
    }
}
//lint +e438 +e550 +e830

void ui_ota_api_msg_register(void)
{
    fw_register_ap_evtmap(APP_OTA, ui_ota_api_evtmap);
    fw_register_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
    //fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_USER_INPUT);
    fw_register_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
}
void ui_ota_api_msg_unregister(void)
{
    fw_unregister_ap_evtmap(APP_OTA);
    fw_unregister_ap_msghost(APP_OTA, ROOT_ID_BACKGROUND);
    //fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_USER_INPUT);
    fw_unregister_ap_msghost(APP_OTA, ROOT_ID_OTA_SEARCH);
}


static void ui_ota_api_do_lock_result(BOOL is_lock)
{
   mul_ota_api_process_nim_lock_status(is_lock);
}
static void ui_ota_api_do_lock(BOOL is_lock,u8 lock_mode,u32 *param)
{
  if(is_lock == TRUE)
  {
    virtual_nim_filter_lock_start(lock_mode,param);
  }
  else
  {
    virtual_nim_filter_lock_stop();
  }
}

static BOOL ui_ota_api_do_version_check(u16 new_version,u16 old_version,u8 stbid_start[128],u8 stbid_end[128])
{
    OS_PRINTF("#####@@@@@debug check:new_ver:0x%x;old_ver:0x%x\n",
                        new_version,old_version);
  //if((new_version > old_version)
	  //|| (g_manual_force == TRUE))
	if(new_version != old_version)
  	{
  	  return TRUE;
  	}
  return FALSE;	
}

#ifdef CHECK_STB_SERIAL_OTA
static BOOL ui_ota_stb_serial_check(ota_dsi_info_t *p_info)
{
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
#if defined(CUSTOMER_JIESAI_WUNING) || defined(CUSTOMER_JIZHONG_ANXIN)
	int i= 0, j = 0;
	u8 serial_num[15] = {0};
	u32 serial_num_start =0, serial_num_end =0, serialnum =0;
    dm_read(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID, 0, 0, sizeof(serial_num), serial_num);

	for(i=0;i<15;i++)
    {
      OS_PRINTF("%d,",p_info->stbid_start[i]-48);
    }
	OS_PRINTF("\n");

	for(i=0;i<15;i++)
    {
      OS_PRINTF("%d,",p_info->stbid_end[i]-48);
    }
	OS_PRINTF("\n");

	for(i=0;i<15;i++)
    {
      OS_PRINTF("%d,",serial_num[i]-48);
    }
	OS_PRINTF("\n");
	
	for(i=0; i<3; i++) //area code
	{
		if( (p_info->stbid_start[i] != serial_num[i]) || (p_info->stbid_end[i] != serial_num[i]) )
			return FALSE;
	}

	for(; i<6; i++) //product number
	{
		if( (p_info->stbid_start[i] != serial_num[i]) || (p_info->stbid_end[i] != serial_num[i]) )
			return FALSE;
	}

	for(; i<8; i++) //hardware version
	{
		if( (p_info->stbid_start[i] != serial_num[i]) || (p_info->stbid_end[i] != serial_num[i]) )
			return FALSE;
	}

	j=6;
	for(; i<15; i++) //stb serial
	{
		serial_num_start += (p_info->stbid_start[i]-48) * (int)pow(10, j);
		serial_num_end += (p_info->stbid_end[i]-48) * (int)pow(10, j);
		serialnum += (serial_num[i]-48) * (int)pow(10, j);
		j--;
	}

	OS_PRINTF("serial_num_start= %d, serial_num_end=%d, serialnum:%d\n",serial_num_start,serial_num_end,serialnum);
	if( (serial_num_start > serialnum) || (serial_num_end < serialnum) )
		return FALSE;
	
	OS_PRINTF("%s %d, i= %d, j=%d\n",__FUNCTION__, __LINE__,i,j);
	return TRUE;
#endif
   return FALSE;
}
#endif

static BOOL ui_ota_api_do_dsi_check(ota_dsi_info_t *p_info)
{
   misc_options_t misc;
   ota_param_t ota_param;
   memset(&misc,0,sizeof(misc_options_t));
   dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

   memset(&ota_param,0,sizeof(ota_param_t));
   mul_ota_dm_api_read_ota_param(&ota_param);
   
  OS_PRINTF("#####@@@@@debug check dsi info:oui:0x%x;sw_ver:0x%x\n",
                        p_info->oui ,p_info->sw_version);
  
  if((p_info->oui == misc.ota_tdi.oui)
        && (p_info->hw_mod_id == misc.ota_tdi.hw_mod_id)
        && (p_info->hw_version == misc.ota_tdi.hw_version)
        && (p_info->sw_mod_id == misc.ota_tdi.sw_mod_id)
        && (p_info->is_force == FALSE))
  {
#if !defined(CUSTOMER_JIESAI_WUNING) && !defined(CUSTOMER_JIZHONG_ANXIN)
    if(ota_param.reserved1 && (p_info->sw_version != ota_param.upg_version))
    {
      return TRUE;
    }
#endif

    if(p_info->sw_version > ota_param.upg_version)
    {
#ifdef CHECK_STB_SERIAL_OTA
		if (TRUE == ui_ota_stb_serial_check(p_info))
			return TRUE;
		else
			return FALSE;
#else      	
		return TRUE;
#endif
    }
  }
  else if((p_info->is_force == TRUE)
        && (p_info->hw_mod_id == misc.ota_tdi.hw_mod_id)
        && (p_info->hw_version == misc.ota_tdi.hw_version)
        && (p_info->sw_mod_id == misc.ota_tdi.sw_mod_id))
    {
      if(ota_param.reserved1 && (p_info->sw_version != ota_param.upg_version))
      {
  	    return TRUE;
      }
      
      if(p_info->sw_version > ota_param.upg_version)
      {
  	    return TRUE;
      }
    }
  
  return FALSE;	
}

static u8 ui_ota_api_find_ota_param_index(block_ota_info_t *block_table,u8 block_id)
{
   u8 i = 0;
   for(i= 0; i < OTA_DM_MAX_BLOCK_NUMBER; i ++)
  {
    if((block_table[i].exist_bit == TRUE)
        && (block_table[i].id == block_id))
      { 
        return i;
      }
  }
  return 0xff;
}


void ui_ota_api_set_ota_tp(void)
{
   nim_para_t set_tp;
   memset(&set_tp,0,sizeof(nim_para_t));
  #if 0
   ota_info_t ota_info;
   sys_status_get_ota_info(&ota_info);
   set_tp.lock_mode = = ota_info.sys_mode;
   set_tp.data_pid = ota_info.download_data_pid;
   memcpy(&set_tp.lockc, &ota_info.lockc,sizeof(dvbc_lock_info_t));
   #else
/***save it when system init****/
   memset(&set_tp,0,sizeof(nim_para_t));
   if(g_customer.customer_id == CUSTOMER_HUANGSHI || g_customer.customer_id == CUSTOMER_JIZHONG_DTMB)
   {
     set_tp.lock_mode = SYS_DTMB;
     set_tp.lockt.tp_freq = 602000;
     set_tp.lockt.nim_type = NIM_DTMB;
     set_tp.lockt.band_width = DTMB_HN_SYMBOL_DEFAULT;
     set_tp.data_pid = 0x1b58;
   }
   else
   {
    set_tp.lock_mode = SYS_DVBC;
    set_tp.lockc.tp_freq = 307000;
    set_tp.lockc.tp_sym = 6875;
    set_tp.lockc.nim_modulate = NIM_MODULA_QAM64;
   }
#endif
   /****init in ota block,the data is do ota tp data,but in app,it from system status init data*/
   mul_ota_dm_api_save_set_ota_tp(&set_tp);
}

void  ui_ota_api_save_do_ota_tp(nim_info_t *tp_info)
{
    /***save it when system init****/
   nim_para_t set_tp;
   memset(&set_tp,0,sizeof(nim_para_t));
   set_tp.lock_mode = tp_info->lock_mode;
   set_tp.data_pid = tp_info->data_pid;
   memcpy(&set_tp.lockc,&tp_info->lockc,sizeof(dvbc_lock_info_t));
   memcpy(&set_tp.locks,&tp_info->locks,sizeof(dvbs_lock_info_t)); 
   memcpy(&set_tp.lockt,&tp_info->lockt,sizeof(dvbt_lock_info_t));
   /****init in ota block,the data is do ota tp data,but in app,it from system status init data*/
   mul_ota_dm_api_save_ota_param_tp(&set_tp);
}

/****main code block id = 0xff****/
u16 ui_ota_api_get_block_version(u8 block_id)
{
  block_ota_info_t block_info = {0};
  mul_ota_dm_api_read_one_block_info(block_id,&block_info);
  return block_info.ota_ver ;
}
u32 ui_ota_api_get_upg_check_version(void)
{
  u32 upg_version = 0;
  upg_version = mul_ota_dm_api_read_ota_param_upg_version();
  return upg_version; 
}

void ui_ota_api_config(BOOL g_new_mem)
{
   ota_config_t ota_config;
   ota_bl_info_t bl_info = {0};
   ota_param_t ota_param;
   u8 stb_id[64];
   misc_options_t misc;
   u8 block_index = 0;
   u8 i = 0,j = 0,k = 0;
   
   u32 read_buffer_size = 0;

   memset(&ota_config,0,sizeof(ota_config_t));
   /***read boot load info*****/
   memset(&bl_info,0,sizeof(ota_bl_info_t));
   mul_ota_dm_api_read_bootload_info(&bl_info);
   memset(&ota_param,0,sizeof(ota_param_t));
   mul_ota_dm_api_read_ota_param(&ota_param);

   memset(&misc,0,sizeof(misc_options_t));
  dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);
   memset(ota_config.sys_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
   ota_config.upg_version = ota_param.upg_version;
    /**virtual all flash or maincode block***/
   ota_config.sys_block[0].id = OTA_ALL_MAIN_CODE_BLOCK_ID; 
   ota_config.sys_block[0].type = 0;
   ota_config.sys_block[0].base_addr = 0;
   ota_config.sys_block[0].size = CHARSTO_SIZE;
   ota_config.sys_block[0].dm_offset = 0;
   block_index = ui_ota_api_find_ota_param_index(ota_param.block_table,ota_config.sys_block[0].id);
   if(block_index != 0xFF)
    {
      ota_config.sys_block[0].ota_old_ver  = ota_param.block_table[block_index].ota_ver;
    }
   else
    {
      ota_config.sys_block[0].ota_old_ver  = 0;
    }
   ota_config.sys_block[0].crc = 0;
   ota_config.sys_block[0].exist_bit = TRUE;
   
   for(i = 0;i < all_dmh_info.header_num;i ++)
    {
       for(j = 0; j < all_dmh_info.header_group[i].block_num; j++)
        {
           if((all_dmh_info.header_group[i].dm_block[j].id != OTA_ALL_MAIN_CODE_BLOCK_ID)
              && ((all_dmh_info.header_group[i].dm_block[j].base_addr % CHARSTO_SECTOR_SIZE) == 0)
              && ((all_dmh_info.header_group[i].dm_block[j].size % CHARSTO_SECTOR_SIZE) == 0)
              && (all_dmh_info.header_group[i].dm_block[j].base_addr != 0)
              && (all_dmh_info.header_group[i].dm_block[j].size != 0))
            {
                    for(k= 1;k < OTA_DM_MAX_BLOCK_NUMBER;k++)
                    {
                      if(ota_config.sys_block[k].exist_bit != TRUE)
                        {
                           
                          ota_config.sys_block[k].id = all_dmh_info.header_group[i].dm_block[j].id;
                          ota_config.sys_block[k].type = all_dmh_info.header_group[i].dm_block[j].type;
                          ota_config.sys_block[k].base_addr = all_dmh_info.header_group[i].dm_block[j].base_addr;
                          ota_config.sys_block[k].size = all_dmh_info.header_group[i].dm_block[j].size;
                          ota_config.sys_block[k].dm_offset = all_dmh_info.header_group[i].dm_offset;
                          block_index = ui_ota_api_find_ota_param_index(ota_param.block_table,ota_config.sys_block[k].id);
                          if(block_index != 0xFF)
                          {
                            ota_config.sys_block[k].ota_old_ver  = ota_param.block_table[block_index].ota_ver;
                          }
                          else
                          {
                            ota_config.sys_block[k].ota_old_ver  = 0;
                          }
                          ota_config.sys_block[k].crc = all_dmh_info.header_group[i].dm_block[j].crc;
                          ota_config.sys_block[k].exist_bit = TRUE;
                          OS_PRINTF("ota api add sys block [0x%x]\n",ota_config.sys_block[k].id);
                          break;
                        }
                    }
            }
        }
    }
   {
      ota_config.hw_tdi.oui = misc.ota_tdi.oui;
    }
    ota_config.hw_tdi.manufacture_id = misc.ota_tdi.manufacture_id;
    ota_config.hw_tdi.hw_version = misc.ota_tdi.hw_version;
    ota_config.hw_tdi.hw_mod_id = misc.ota_tdi.hw_mod_id;
    ota_config.hw_tdi.sw_mod_id = misc.ota_tdi.sw_mod_id;

    OS_PRINTF("ota api config misc info oui :0x%x,hw_version:0x%x\n",
                      ota_config.hw_tdi.oui,ota_config.hw_tdi.hw_version);
    
    memset(stb_id,0,64);
    //get_sys_stbid(64,stb_id,64);
    memcpy(ota_config.hw_tdi.stbid,stb_id,64);

  
   ota_config.tri_mode = (ota_tri_mode_t)bl_info.ota_status;
   ota_config.destroy_flag = bl_info.destroy_flag;
   ota_config.destroy_block_id = (u8)bl_info.destroy_block_id;
   ota_config.fail_times = (u8)bl_info.fail_times;
  /****it be from read save data*****/
   ota_config.ota_number = (u8)bl_info.ota_number;
   ota_config.ota_curr_block_id = (u8)bl_info.ota_curr_id;
   ota_config.ota_back_block_id = (u8)bl_info.ota_backup_id;
   ota_config.load_block_id = (u8)bl_info.load_block_id;
   ota_config.maincode_block_id = MAINCODE_BLOCK_ID;

   if((ota_config.tri_mode == OTA_TRI_MODE_FORC) 
      ||(TRUE == ota_config.destroy_flag)) 
  {
    //g_manual_force = TRUE;
  }

   
   ota_config.ota_in_maincode_bit = FALSE;
   if(ota_config.ota_in_maincode_bit == FALSE)
    {
       g_ota_in_maincode = FALSE;
    }
   else
    {
       g_ota_in_maincode = TRUE;
    }
   
   ota_config.debug_level = OTA_DEBUG_ALL;
   //ota_config.max_fail_times = 0;
   ota_config.max_fail_times = 3;
   ota_config.ota_debug_printf = OTA_API_DBG_PRINTF;
   ota_config.ota_do_lock = ui_ota_api_do_lock;
   ota_config.ota_version_check = ui_ota_api_do_version_check;
   ota_config.ota_dsi_info_check = ui_ota_api_do_dsi_check;
  
   #ifdef WIN32
   ota_config.dsmcc_msg_timeout = DSMCC_MSG_TIMEOUT;
   #else
   if(CUSTOMER_ID == CUSTOMER_AISAT_DEMO)
   	ota_config.dsmcc_msg_timeout = 2000;
   else
   	ota_config.dsmcc_msg_timeout = 5000;
   #endif
   if(g_new_mem == FALSE)
    {
       p_ota_api_buffer = (u8 *)mem_mgr_require_block(BLOCK_PLAY_BUFFER, SYS_MODULE_GDI);
       read_buffer_size = mem_mgr_get_block_size(BLOCK_PLAY_BUFFER);
       mem_mgr_release_block(BLOCK_PLAY_BUFFER);
       g_ota_api_buf_free = FALSE;
    }
   else
    {
       read_buffer_size = 128 * KBYTES;
       p_ota_api_buffer = mtos_malloc(read_buffer_size);
       g_ota_api_buf_free = TRUE;
    }
   
   #ifndef WIN32
   memset(p_ota_api_buffer,0,read_buffer_size);
	 #endif
	 ota_config.bootload_size = DM_HDR_START_ADDR;
   ota_config.flash_size =  CHARSTO_SIZE;
   
   #ifdef WIN32
   ota_config.dmx_buf_size = 512 * KBYTES;
   #else
   ota_config.dmx_buf_size = 96 * KBYTES;
   #endif
   ota_config.buffer_size = read_buffer_size;
   ota_config.p_attach_buffer = p_ota_api_buffer;
   if(ota_config.p_attach_buffer == NULL)
    {
      MT_ASSERT(ota_config.p_attach_buffer  != NULL);
    return;
    }
   mul_ota_api_config(&ota_config);
}
void ui_ota_api_init(void)
{

   ota_para_t para;
   RET_CODE ret = ERR_FAILURE;

  OS_PRINTF("#####ui_ota_api_init\n");

  //g_manual_force = FALSE;

   ui_ota_api_msg_register();
   virtual_nim_filter_register_do_lock_result(ui_ota_api_do_lock_result);
   
   memset(&para,0,sizeof(ota_para_t));
   memset(&all_dmh_info,0,sizeof(dm_dmh_info_t ));
   mul_ota_dm_api_find_dmh_info_from_flash(0,CHARSTO_SIZE,&all_dmh_info);
  
   para.medium = OTA_MEDIUM_BY_TUNER;
   para.protocol_type = OTA_STREAM_PROTOCOL_TYPE_DSMCC;
   para.evt_process = ui_ota_api_evt_process;
   para.task_stk_size = 128 * KBYTES;

   ret =  mul_ota_api_init(&para);
   if(ret != SUCCESS)
    {
      OS_PRINTF("ota api test fail!\n");
      return;
    }
   
}


void ui_ota_api_auto_mode_start(ota_work_t  mode)
{

   ota_api_policy_t policy;
   ota_param_t ota_param;
   g_auto_mode = TRUE;
   memset(&ota_param,0,sizeof(ota_param_t));
   mul_ota_dm_api_read_ota_param(&ota_param);
   memset(&policy,0,sizeof(ota_api_policy_t));
   policy.run_select = mode;
   if(policy.run_select == OTA_WORK_SELECT_AUTO)
    {
       if(g_ota_in_maincode == FALSE)
        {
          MT_ASSERT(0);/**don't run ota mode***/
        }
       
       policy.run_select = OTA_WORK_SELECT_AUTO;
       policy.tp_info.do_ota_tp.lock_mode = ota_param.ota_tp.lock_mode;
       policy.tp_info.do_ota_tp.data_pid = ota_param.ota_tp.data_pid;
       memcpy(&policy.tp_info.do_ota_tp.lockc,&ota_param.ota_tp.lockc,sizeof(dvbc_lock_info_t));
       memcpy(&policy.tp_info.do_ota_tp.locks,&ota_param.ota_tp.locks,sizeof(dvbs_lock_info_t)); 
       memcpy(&policy.tp_info.do_ota_tp.lockt,&ota_param.ota_tp.lockt,sizeof(dvbt_lock_info_t));
       
       policy.tp_info.ota_set_tp.lock_mode = ota_param.ota_set_tp.lock_mode;
       policy.tp_info.ota_set_tp.data_pid = ota_param.ota_set_tp.data_pid;
       memcpy(&policy.tp_info.ota_set_tp.lockc,&ota_param.ota_set_tp.lockc,sizeof(dvbc_lock_info_t));
       memcpy(&policy.tp_info.ota_set_tp.locks,&ota_param.ota_set_tp.locks,sizeof(dvbs_lock_info_t)); 
       memcpy(&policy.tp_info.ota_set_tp.lockt,&ota_param.ota_set_tp.lockt,sizeof(dvbt_lock_info_t));

       policy.tp_info.ota_set_back_tp.lock_mode = ota_param.ota_set_back_tp.lock_mode;
       policy.tp_info.ota_set_back_tp.data_pid = ota_param.ota_set_back_tp.data_pid;
       memcpy(&policy.tp_info.ota_set_back_tp.lockc,
                        &ota_param.ota_set_back_tp.lockc,
                        sizeof(dvbc_lock_info_t));
       memcpy(&policy.tp_info.ota_set_back_tp.locks,
                        &ota_param.ota_set_back_tp.locks,
                        sizeof(dvbs_lock_info_t)); 
       memcpy(&policy.tp_info.ota_set_back_tp.lockt,
                          &ota_param.ota_set_back_tp.lockt,
                          sizeof(dvbt_lock_info_t));
    }
   else if(policy.run_select == OTA_WORK_SELECT_CHECK)
    {
       policy.run_select = OTA_WORK_SELECT_CHECK;
       policy.tp_info.do_ota_tp.lock_mode = ota_param.ota_set_tp.lock_mode;
       policy.tp_info.do_ota_tp.data_pid = ota_param.ota_set_tp.data_pid;
       memcpy(&policy.tp_info.do_ota_tp.lockc,&ota_param.ota_set_tp.lockc,sizeof(dvbc_lock_info_t));
       memcpy(&policy.tp_info.do_ota_tp.locks,&ota_param.ota_set_tp.locks,sizeof(dvbs_lock_info_t)); 
       memcpy(&policy.tp_info.do_ota_tp.lockt,&ota_param.ota_set_tp.lockt,sizeof(dvbt_lock_info_t));
    }
   else
    {
       return;
    }

   OS_PRINTF("#####ui_ota_api_init:lock mode:%d\n",policy.tp_info.do_ota_tp.lock_mode);
    OS_PRINTF("#####ui_ota_api_init:lock freq:%d\n",policy.tp_info.do_ota_tp.lockc.tp_freq);
    OS_PRINTF("#####ui_ota_api_init:lock ysm:%d\n",policy.tp_info.do_ota_tp.lockc.tp_sym);

   virtual_nim_filter_start();
   ui_ota_api_config(TRUE);
   mul_ota_api_start();
   mul_ota_api_select_policy(&policy);
}


void ui_ota_api_manual_check_mode(nim_info_t *tp_info)
{
   ota_api_policy_t policy;
   g_auto_mode = FALSE;
   //g_manual_force = FALSE;
   policy.run_select = OTA_WORK_SELECT_CHECK;
   policy.tp_info.do_ota_tp.lock_mode = tp_info->lock_mode;
   policy.tp_info.do_ota_tp.data_pid = tp_info->data_pid;
   memcpy(&policy.tp_info.do_ota_tp.lockc,&tp_info->lockc,sizeof(dvbc_lock_info_t));
   memcpy(&policy.tp_info.do_ota_tp.locks,&tp_info->locks,sizeof(dvbs_lock_info_t)); 
   memcpy(&policy.tp_info.do_ota_tp.lockt,&tp_info->lockt,sizeof(dvbt_lock_info_t));
   virtual_nim_filter_start();
   ui_ota_api_config(TRUE);
   mul_ota_api_start();
   mul_ota_api_select_policy(&policy); 
}

void ui_ota_api_manual_save_ota_info(void)
{
   ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  
  if(bl_info.ota_status != (u32)OTA_TRI_MODE_FORC)
  {
    bl_info.ota_status = OTA_TRI_MODE_AUTO;
    bl_info.fail_times = 0;
    bl_info.destroy_flag = FALSE;
  }
  mul_ota_dm_api_save_bootload_info(&bl_info);
}

void ui_ota_api_stop(void)
{
   virtual_nim_filter_stop();
   mul_ota_api_stop();
   /****here need to free ota_config.p_attach_buffer******/
   if(g_ota_api_buf_free == TRUE)
    {
      mtos_free(p_ota_api_buffer);
      p_ota_api_buffer = NULL;
      g_ota_api_buf_free = FALSE;
      
    }
}

void ui_ota_api_auto_check_cancel(void)
{
   control_t *p_ctrl = NULL;
	p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	if(NULL != p_ctrl)
	{
	  ctrl_process_msg(p_ctrl,MSG_OTA_TMOUT, 0, 0);
	}
}
BOOL  ui_ota_api_get_maincode_ota_flag(void)
{
   return g_ota_in_maincode;
}

void ui_ota_api_maincode_ota_start(void)
{
   OS_PRINTF("###into maincode ota:auto mode\n");
   ui_ota_api_stop();
   ui_ota_api_auto_mode_start(OTA_WORK_SELECT_AUTO);
}

BEGIN_AP_EVTMAP(ui_ota_api_evtmap)
  CONVERT_EVENT(OTA_EVT_LOCKED, MSG_OTA_LOCK) 
  //CONVERT_EVENT(OTA_EVT_UNLOCKED, MSG_OTA_TMOUT) 
  CONVERT_EVENT(OTA_EVT_TABLE_TIMEOUT, MSG_OTA_TMOUT) 
  CONVERT_EVENT(OTA_EVT_CHECK_FAIL, MSG_OTA_TMOUT) 
  CONVERT_EVENT(OTA_EVT_CHECK_SUCESS, MSG_OTA_TRIGGER_RESET) 
  CONVERT_EVENT(OTA_EVT_UPG_COMPLETE,   MSG_OTA_FINISH)
  //CONVERT_EVENT(OTA_EVT_UNLOCKED,   MSG_OTA_UNLOCK)
END_AP_EVTMAP(ui_ota_api_evtmap)
