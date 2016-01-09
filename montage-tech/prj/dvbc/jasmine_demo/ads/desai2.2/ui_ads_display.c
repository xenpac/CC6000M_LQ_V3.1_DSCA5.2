/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"

#include "ads_ware.h"
#include "ads_api_desai.h"
#include "ui_ads_display.h"
#include "ui_config.h"
#include "pdec.h"
#include "fcrc.h"
#include "ui_signal.h"
#include "ui_util_api.h"
#include "ui_ad_api.h"


static BOOL is_ads_logo_stored = FALSE;
static BOOL is_full_screen_ad_enable = FALSE;
static BOOL is_unauth_ad_enable = FALSE;

static BOOL is_ads_osd_ready = FALSE;
static BOOL is_ads_osd_roll = FALSE;

static u8 uc_current_osd_index = 0;
static ST_DSAD_OSD_PROGRAM_SHOW_INFO osd_prom_show_info;
static ST_ADS_OSD_INFO osd_display_info[MAX_ROLL_OSD_NUM];

static BOOL is_full_screen_ad_on_showing = FALSE;
static BOOL is_full_screen_ad_need_display = FALSE;

static ST_DSAD_PICTURE_SHOW_INFO * pic_full_scr_info = NULL;
static ST_DSAD_PICTURE_SHOW_INFO * pic_unauth_ad_info = NULL;
static BOOL is_unauth_ad_on_showing = FALSE;

static void nvm_ad_block_crc_init(void)
{
  RET_CODE ret;
  
   /* create crc32 table */
  ret = (RET_CODE)crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  MT_ASSERT(SUCCESS == ret); 
}

static u32 nvm_ad_block_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, (int)len);
  
}

static RET_CODE nvram_ad_logo_write(ST_DSAD_AV_SHOW_INFO * av_show_info)
{
  RET_CODE ret = SUCCESS;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_CHARSTO);
  u32 base_addr = dm_get_block_addr(dm_handle, START_LOGO_BLOCK_ID) - get_flash_addr();
  u32 block_size = 0;
  u32 wr_addr = 0;
  u8 *p_data = NULL;
  u32 size = 0;

  //lint -save -e64
  charsto_prot_status_t st_old={0};
  charsto_prot_status_t st_set={0};
  dmh_block_info_t real_dm_info={0};
  //lint -restore
  u8 logo_head[4] = {0}; // logo data size; without head size.
  
  u32 head_size = sizeof(logo_head);
  block_size = dm_get_block_size(dm_handle, START_LOGO_BLOCK_ID);

  size = av_show_info->uiDataLen;
  p_data = av_show_info->pucAvData;
  if(base_addr == 0 || block_size == 0 || (size + head_size) > block_size)
  {
    OS_PRINTF("Error!,blocksize:%d,size:%d\n",block_size, size);
    return ERR_FAILURE;
  }
    
  wr_addr = base_addr + block_size - sizeof(dmh_block_info_t);

  ret = charsto_read(p_charsto_dev,wr_addr,(u8 *)&real_dm_info,sizeof(dmh_block_info_t));

  if(ret == SUCCESS)
  {
    real_dm_info.size = size;
  }
  else
  {
    OS_PRINTF("Error!\n");
    return ERR_FAILURE;
  }

  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  ret = charsto_erase(p_charsto_dev, base_addr, block_size/CHARSTO_SECTOR_SIZE);
  MT_ASSERT(ret == SUCCESS);

  logo_head[0] = SYS_GET_BYTE3(size);
  logo_head[1] = SYS_GET_BYTE2(size); 
  logo_head[2] = SYS_GET_BYTE1(size); 
  logo_head[3] = SYS_GET_BYTE0(size); 
  
  ret = charsto_writeonly(p_charsto_dev, base_addr, logo_head, head_size);
  ret = charsto_writeonly(p_charsto_dev, base_addr+head_size, p_data, size);
  MT_ASSERT(ret == SUCCESS);
  //write dmh_block_info_t infomation for OTA upgrade, the end of block.
  ret = charsto_writeonly(p_charsto_dev, wr_addr, (u8 *)&real_dm_info, sizeof(dmh_block_info_t));
  OS_PRINTF("!!!!!!!!!!wr_addr=0x%x,sizeof(dmh_block_info_t)=0x%x\n",wr_addr,sizeof(dmh_block_info_t));
  MT_ASSERT(ret == SUCCESS);
  //restore
  ret = dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  MT_ASSERT(ret == SUCCESS);
  OS_PRINTF("AD nvram_write success !! \n");
  
  return SUCCESS;
//lint -save -e550
}
//lint -restore

//lint -save -e438 -e550
//read the ad data from flash
RET_CODE nvram_ad_logo_read(u32 offset, u8 *p_buf, u32 size)
{
 // RET_CODE ret = SUCCESS;
  void *p_dm_handle = NULL;
  u32 nvm_read =0;
  //u8 *p_block_buffer = NULL;
  u32 crc_32_read = 0;
  u32 crc_32_check = 0;
  u8 *p_read_crc_buffer = NULL;
  ST_AD_DS_LOG_SAVE_INFO *r_logo_info = NULL;
  
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  if(p_buf == NULL)
  {
     return ERR_FAILURE; 
  }
  memset(p_buf,0,size);
  
#if 0
  p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
  if(p_block_buffer == NULL)
  {
     return ERR_FAILURE; 
  }
#endif
 
  nvm_read = dm_direct_read(p_dm_handle,RW_ADS_LOGO_BLOCK_ID, 0, size, p_buf);
  UI_PRINTF("ad nvm_read[0x%x]\n", nvm_read);
  
  r_logo_info= (ST_AD_DS_LOG_SAVE_INFO *)p_buf;
  if(r_logo_info->uiLogoSize > 100 * KBYTES || r_logo_info->ucLogType != DSAD_AV_VIDEO_M2V)
  {
    UI_PRINTF("adv_logo:read logo data length too long or data type not right \n",__FUNCTION__);
    //mtos_free(p_block_buffer);
    return ERR_FAILURE;
  }
  p_read_crc_buffer = (u8*)(p_buf + sizeof(ST_AD_DS_LOG_SAVE_INFO) + r_logo_info->uiLogoSize);
  crc_32_read = ((*p_read_crc_buffer) << 24)+ ((*(p_read_crc_buffer + 1)) << 16)
                         +((*(p_read_crc_buffer + 2)) << 8) + (*(p_read_crc_buffer + 3));

  /****check crc and read back block*****/
 nvm_ad_block_crc_init();
 crc_32_check = nvm_ad_block_generate(p_buf + sizeof(ST_AD_DS_LOG_SAVE_INFO), r_logo_info->uiLogoSize);

 UI_PRINTF("### %s read crc =0x%x,check crc =0x%x\n",
                                         __FUNCTION__, crc_32_read,crc_32_check);
 
 if(crc_32_read != crc_32_check)
 {
	//mtos_free(p_block_buffer);
	return ERR_FAILURE;
 }
 //memcpy(p_buf,p_block_buffer,size);
 //mtos_free(p_block_buffer);
 return SUCCESS;
}
//lint -restore

RET_CODE ui_adv_display_osd_msg(u16 pg_id)
{
  ads_info_t ads_info = {0};
  dvbs_prog_node_t pg;
  RET_CODE ret = SUCCESS;

  if(pg_id != INVALIDID)
  {
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      OS_PRINTF("ADV ui_adv_set_pic_by_type SID[%d] TS_ID[%d] NET_ID[%d]\n",pg.s_id, pg.ts_id,pg.orig_net_id);
      ads_info.service_id = (u16)pg.s_id;
      ads_info.ts_id = (u16)pg.ts_id;
      ads_info.network_id= (u16)pg.orig_net_id;
    }  
  }
  if(is_ads_osd_roll)
  {
    ui_ads_osd_roll_stop();
  }
  ads_info.ads_media_type = ADS_AD_TYPE_OSD;
  ads_info.ads_type = ADS_AD_TYPE_OSD;
  ret = ads_display(ADS_ID_ADT_DESAI, &ads_info);
  UI_PRINTF("ui_adv_get ads osd osd msg ret = %d\n", ret);
  return ret;
}

//desai ads get type func
void ads_get_type(ads_ad_type_t type, u32 * cmd, ads_info_t * ads_info)
{
  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
    case ADS_AD_TYPE_MENU_DOWN:
    case ADS_AD_TYPE_MAIN_MENU:
    case ADS_AD_TYPE_SUB_MENU:
    case ADS_AD_TYPE_OTHER_GIF:
    case ADS_AD_TYPE_SEARCHING:
    case ADS_AD_TYPE_SIGNAL:
      ads_info->ads_type = ADS_AD_TYPE_MAIN_MENU;
      *cmd = ADS_IOCMD_AD_NO_PROGRAM;
      break;
    case ADS_AD_TYPE_CHLIST:
    case ADS_AD_TYPE_CHBAR:
      ads_info->ads_type =  ADS_AD_TYPE_CHLIST;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
    case ADS_AD_TYPE_VOLBAR:
      ads_info->ads_type =  ADS_AD_TYPE_VOLBAR;
      *cmd = ADS_IOCMD_AD_PROGRAM;
      break;
    case ADS_AD_TYPE_UNAUTH_PG:
      ads_info->ads_type =  ADS_AD_TYPE_UNAUTH_PG;
      *cmd = ADS_IOCMD_AD_NO_PROGRAM;
      break;
    case  ADS_AD_TYPE_OSD:
      ads_info->ads_type = ADS_AD_TYPE_OSD;
      *cmd = ADS_IOCMD_AD_OSD;
      break;
    default:
      break;           
  }
  return;
}

/*fullscreen advertisement display*/
static RET_CODE ui_pic_fullscr_play(void)
{
  RET_CODE ret = 0;
  
  if(FALSE == get_full_scr_ad_status() || (is_full_screen_ad_need_display == FALSE))
  {   
    UI_PRINTF("@@DS_AD: Full scr status not ready!\n");
    return ret;
  }
  if((pic_full_scr_info->uiDataLen == 0) || (pic_full_scr_info->pucPicData == NULL))
  {
    UI_PRINTF("@@DS_AD: Full scr ad data error!\n");
    return ret;
  }
  //fullscreen ad
  UI_PRINTF("[UI_ADS_DIS] fw_get_focus_id()= %d \n",fw_get_focus_id());
 UI_PRINTF("[show full screen ad] pic_full_scr_info = 0x%x, pic_show_info->uiDataLen = %d\n",pic_full_scr_info,
                          pic_full_scr_info->uiDataLen);
  if((ROOT_ID_BACKGROUND == fw_get_focus_id()))
  {
    pic_adv_stop();
    UI_PRINTF("[ui_pic_fullscr_play] show fullscreen ad!!!!\n");
    ui_adv_set_fullscr_rect(pic_full_scr_info->usStartX, pic_full_scr_info->usStartY, 
                                            pic_full_scr_info->usWidth, pic_full_scr_info->usHeigth);     
    ret = ui_adv_set_pic(pic_full_scr_info->uiDataLen, pic_full_scr_info->pucPicData);

    is_full_screen_ad_on_showing = TRUE;
  }
  return ret;
}

/*unauth adv display*/
static RET_CODE ui_unauth_ad_play(void)
{
  RET_CODE ret = 0;

  if((pic_unauth_ad_info->uiDataLen == 0) ||
      (pic_unauth_ad_info->pucPicData == NULL) ||
      (DSAD_PIC_GIF != pic_unauth_ad_info->enPicType))
  {
    UI_PRINTF("pic_unauth_ad_info is null, will show full screen ad \n");
    ret =ui_pic_fullscr_play();
    return ret;
  }

  if(ROOT_ID_BACKGROUND == fw_get_focus_id())
  {
    pic_adv_stop();
    //ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, MSG_CA_DS_CLEAR_UNAUTH);
    UI_PRINTF("ui_unauth_ad_play show pic_unauth_ad_info!! \n");
    ui_adv_set_fullscr_rect(pic_unauth_ad_info->usStartX, pic_unauth_ad_info->usStartY, 
                                          pic_unauth_ad_info->usWidth, pic_unauth_ad_info->usHeigth);     
    ret = ui_adv_set_pic(pic_unauth_ad_info->uiDataLen, pic_unauth_ad_info->pucPicData);
    is_unauth_ad_on_showing = TRUE;
  }
  return ret;
}


RET_CODE ui_check_fullscr_ad_play(void)
{
  RET_CODE ret = FALSE;
 is_unauth_ad_on_showing =FALSE;
  
  if(TRUE == is_unauth_ad_enable)
  {
    if(ROOT_ID_BACKGROUND == fw_get_focus_id())
    {
      ret = ui_adv_pic_play(ADS_AD_TYPE_UNAUTH_PG, ROOT_ID_BACKGROUND);  
      UI_PRINTF("get ADS_AD_TYPE_UNAUTH ad ret value = %d \n",ret);
    }
  }  
  if(!ret)
  {
    UI_PRINTF("[ui_check_fullscr_ad_play] show full screen ad \n",ret);
    ret = ui_pic_fullscr_play();
  }
  return ret;
}

void ui_set_full_scr_ad_status(BOOL status)
{
  is_full_screen_ad_on_showing = status;
}

BOOL ui_get_full_scr_ad_status(void)
{
  return is_full_screen_ad_on_showing;
}
#if 0
BOOL ui_set_ads_osd_status(BOOL status)
{
  is_ads_osd_roll = status;
}
#endif

BOOL ui_get_ads_osd_status(void)
{
  return is_ads_osd_roll;
}

BOOL ui_is_adv_osd_ready(void)
{
  return is_ads_osd_ready;
}
void on_desktop_ad_start_roll(u16 *p_uni_str, u32 roll_mode, ST_DSAD_OSD_SHOW_INFO *osd_show_info)
{
  u8 font_size = 0;
  if(get_force_channel_status())
  {
    return;
  }
  OS_PRINTF("ADS OSD font_size: %d\n",osd_show_info->ucFontSize);

  switch(osd_show_info->ucFontSize)
  {
    case 8:
      font_size = 1;
      break;
    case 12:
      font_size = 2;
      break;
    case 16:
      font_size = 3;
      break;
    case 18:
      font_size = 4;
      break;
    case 20:
      font_size = 5;
      break;
    case 22:
      font_size = 6;
      break;
    case 24:
      font_size = 7;
      break;
    case 26:
      font_size = 8;
      break;
    case 28:
      font_size = 9;
      break;
    case 36:
      font_size = 10;
      break;
    case 48:
      font_size = 11;
      break;
    case 72:
      font_size = 12;
      break;
    default:
      font_size = 3;
      break;
  }
  on_desktop_start_roll_position(p_uni_str, osd_show_info->aucBackgroundColor, 
                              osd_show_info->aucDisplayColor, &font_size, (u8 *)(&osd_show_info->enDisplayPos));
  
  ui_ca_set_roll_status(TRUE);
}

void ui_ads_osd_roll(void)
{
  ST_DSAD_OSD_SHOW_INFO * osd_show_info = NULL;
  u16 uni_str[260] = {0};
  u8 index = 0;
  
  osd_show_info = (ST_DSAD_OSD_SHOW_INFO *)(osd_prom_show_info.stTextShow + uc_current_osd_index);
  
  OS_PRINTF("[AD_OSD] **** osd_show_info->usContentSize = %d \n",osd_show_info->usContentSize);
  OS_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer = %s \n",osd_show_info->pucDisplayBuffer);
  OS_PRINTF("[AD_OSD] **** osd_show_info->pucDisplayBuffer[%d] = %s \n",uc_current_osd_index, osd_display_info[uc_current_osd_index].osd_display_buffer);
  
  if(osd_display_info[uc_current_osd_index].b_SaveFlag == TRUE)
  {  
    /******************************************************************************
    //不判断CA滚动字幕状态，直接显示广告滚动，
    ******************************************************************************/
  
    gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
                                                                osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);
    
    on_desktop_ad_start_roll(uni_str, 0, osd_show_info);
    //set_ca_msg_rolling_status(FALSE);
    
    is_ads_osd_roll = TRUE;
  }
  else
  { 
    for(index = uc_current_osd_index + 1; index <= osd_prom_show_info.ucOsdNum; index++)
    {
      if(index == osd_prom_show_info.ucOsdNum)
      {
        //ui_ads_osd_roll_stop();
        uc_current_osd_index = 0;
        break;
      }
      else
      {
        if(osd_display_info[index].b_SaveFlag == TRUE)
         { 
           osd_show_info = (ST_DSAD_OSD_SHOW_INFO *)(osd_prom_show_info.stTextShow + index);
           gb2312_to_unicode(osd_display_info[uc_current_osd_index].osd_display_buffer, 
                                                                osd_show_info->usContentSize, uni_str, MAX_ROLL_OSD_LENGTH);
    
           on_desktop_ad_start_roll(uni_str, 0, osd_show_info);
            is_ads_osd_roll = TRUE;
            //set_ca_msg_rolling_status(FALSE);
            uc_current_osd_index = ++index;
           break;
          }
        }
    }
  }

}

void ui_ads_osd_roll_next(void)
{
  uc_current_osd_index++;
 if(uc_current_osd_index == osd_prom_show_info.ucOsdNum)
  {
    uc_current_osd_index = 0;
  }
  ui_ads_osd_roll();
}

static void clear_ads_osd_info(void)
{
  memset(osd_display_info, 0, MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
}

extern RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
void ui_ads_osd_roll_stop(void)
{
  is_ads_osd_roll = FALSE;
  uc_current_osd_index = 0;
  clear_ads_osd_info();
  //fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, FALSE, MSG_EXIT_AD_ROLLING, 0, 0);
  on_ca_rolling_hide(fw_find_root_by_id(ROOT_ID_BACKGROUND), 0, 0, 0);
  return;
}

static void ui_save_desai_osd(ST_DSAD_OSD_PROGRAM_SHOW_INFO * pst_osd_ads_roll)
{
  u8 uc_ads_osd_num = 0;
  u32 index = 0;
  u16 content_size = 0;
  
  memset((void *)&osd_prom_show_info, 0, sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));
  memcpy((void *)&osd_prom_show_info, (void *)pst_osd_ads_roll, sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));
  UI_PRINTF("@@@@@pst_osd_ads_roll->ucOsdNum = %d \n",pst_osd_ads_roll->ucOsdNum);
  uc_ads_osd_num = osd_prom_show_info.ucOsdNum;

  if(uc_ads_osd_num == 0)
  {
    UI_PRINTF("[ad_osd] ads osd num is 0, return \n");
    return;
  }

  memset((void *)osd_display_info, 0, MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));
  UI_PRINTF("[ad_osd] osd_display_info mem clr num = %d \n", MAX_ROLL_OSD_NUM * sizeof(ST_ADS_OSD_INFO));

  for(index = 0; index < uc_ads_osd_num; index++)
  {
    content_size = pst_osd_ads_roll->stTextShow[index].usContentSize;

    if(content_size> MAX_ROLL_OSD_LENGTH)
    {
      content_size = MAX_ROLL_OSD_LENGTH;
    }

    if(pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer != NULL)
    {
      memcpy(osd_display_info[index].osd_display_buffer, pst_osd_ads_roll->stTextShow[index].pucDisplayBuffer, content_size);
      *(osd_display_info[index].osd_display_buffer + content_size) = '\0';
      osd_display_info[index].b_SaveFlag = TRUE;
    }
    else
    {
      osd_display_info[index].b_SaveFlag = FALSE;
    }
  }
}

static RET_CODE ui_show_desai_osd_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;

  ST_DSAD_OSD_PROGRAM_SHOW_INFO * pst_osd_ads_roll = NULL;
  
  pst_osd_ads_roll = (ST_DSAD_OSD_PROGRAM_SHOW_INFO*)p_data;
  if(pst_osd_ads_roll == NULL)
  {
    UI_PRINTF("[AD_OSD] ads osd null!! \n");
    return ERR_FAILURE;
  }
#endif
  //adv roll osd ready to show.
  is_ads_osd_ready = TRUE;
  
  //stop current rolling
  ui_ads_osd_roll_stop();

  ui_save_desai_osd(pst_osd_ads_roll);

  //set current roll index to 1st osd msg
  uc_current_osd_index = 0;

  //ads osd display
  //if(!is_ads_osd_roll)
  {
    //ui_ads_osd_roll_stop();
  }
  ui_ads_osd_roll();

  return ret;
}

static RET_CODE ui_show_desai_pic_ad(u8 *p_data)
{
  RET_CODE ret = ERR_FAILURE;
  ST_DSAD_PICTURE_SHOW_INFO * pic_show_info = NULL;
  pic_show_info = (ST_DSAD_PICTURE_SHOW_INFO*)p_data;
  
  UI_PRINTF("[UI]: pic_show_info-->enPicType = %d \n",pic_show_info->enPicType);
  
  if(pic_show_info == NULL || pic_show_info->pucPicData == NULL || pic_show_info->uiDataLen == 0)
    return ERR_FAILURE;

#ifdef MIN_AV_64M
      if((pic_show_info->usWidth * pic_show_info->usHeigth)> 300*330)
      {
            OS_PRINTF("\n\n picture too large to unsupport only support w<=300 h<=330\n\n");
            return ERR_FAILURE;
      }
#endif

  switch(pic_show_info->enPosType)
  {
    case DSAD_STB_UNAUTHORIZED:
    {
      UI_PRINTF("[DSAD_STB_UNAUTHORIZED] p_data = 0x%x, pic_show_info->uiDataLen = %d\n",
                    p_data, pic_show_info->uiDataLen);
      UI_PRINTF("[DSAD_STB_UNAUTHORIZED] x= %d, y=%d, w= %d, h = %d \n",
                  pic_show_info->usStartX, pic_show_info->usStartY,
                 pic_show_info->usWidth, pic_show_info->usHeigth);

      pic_unauth_ad_info = (ST_DSAD_PICTURE_SHOW_INFO*)p_data;
      ret = ui_unauth_ad_play();
      break;
    }
    
    case DSAD_STB_FULLSCREEN:
      {
        UI_PRINTF("[show full screen ad] get_full_scr_ad_status= %d \n",get_full_scr_ad_status());
        UI_PRINTF("[show full screen ad] p_data = 0x%x, pic_show_info->uiDataLen = %d\n",
                        p_data, pic_show_info->uiDataLen);
        UI_PRINTF("[full screen ad rect] x= %d, y=%d, w= %d, h = %d \n",
                      pic_show_info->usStartX, pic_show_info->usStartY,
                     pic_show_info->usWidth, pic_show_info->usHeigth);
        pic_full_scr_info = (ST_DSAD_PICTURE_SHOW_INFO*)p_data;
        is_full_screen_ad_need_display = TRUE;
        
        ret = ui_check_fullscr_ad_play();        
        break;
      }
    
    case DSAD_STB_EPG:
    case DSAD_STB_VOLUME:
    case DSAD_STB_MENU:
      {
        ret = ui_adv_set_pic(pic_show_info->uiDataLen, pic_show_info->pucPicData);
        is_full_screen_ad_on_showing = FALSE;
        break;
      }
    
    default:
          break;
  }
  return ret;
}
static RET_CODE ui_save_logo_data_to_nvm(ST_DSAD_AV_SHOW_INFO * av_show_info)
{
  RET_CODE ret = ERR_FAILURE;
  if(av_show_info->enAvType != DSAD_AV_VIDEO_M2V || av_show_info->uiDataLen > 100 * KBYTES)
  {
    UI_PRINTF("[ADS_DIS] Desai ads show:av_show_info->uiDataLen = %d  \n",av_show_info->uiDataLen);
    return ret;
  }
  //ret = nvram_ad_logo_write(offset, av_show_info->pucAvData,av_show_info->uiDataLen,&logo_info);
  ret = nvram_ad_logo_write(av_show_info);
  
  if(ret == SUCCESS)
    is_ads_logo_stored = TRUE;
  return ret;
}
static RET_CODE ui_show_desai_vid_ad(u8 *p_data)
{
  RET_CODE ret = SUCCESS;
  ST_DSAD_AV_SHOW_INFO * av_show_info = NULL;
  av_show_info = (ST_DSAD_AV_SHOW_INFO*)p_data;

  //ads osd display
  switch(av_show_info->enPosType)
  {
    case DSAD_STB_LOG:
    {
      //save logo info
      ui_save_logo_data_to_nvm(av_show_info);
      break;
    }
    case DSAD_STB_FULLSCREEN:
    case DSAD_STB_EPG:
    case DSAD_STB_MENU:
    case DSAD_STB_VOLUME:
    case DSAD_STB_POS_UNKNOWN:
      break;
    default:
      break;
  }
  
  return ret;
}

BOOL get_ads_logo_status(void)
{
  return  is_ads_logo_stored;
 }

BOOL get_full_scr_ad_status(void)
{
  return  is_full_screen_ad_enable;
 }

void set_full_scr_ad_status(BOOL is_show)
{
  is_full_screen_ad_enable = is_show;
 }
void set_unauth_ad_status(BOOL status)
{
  is_unauth_ad_enable = status;
  ui_check_fullscr_ad_play();
}
RET_CODE ui_adv_desai_display(ads_info_t *p_ads_info)
{
  RET_CODE ret = ERR_FAILURE, adv_ret = DSAD_FUN_ERR_PARA;
  u32 cmd = 0;
  //lint -save -e64
  ST_DSAD_OSD_PROGRAM_SHOW_INFO stOsdShowInfo;
  ST_DSAD_PICTURE_SHOW_INFO stPicShowInfo = {0}; 
  ST_DSAD_AV_SHOW_INFO stAvShowInfo = {0};
  //lint -restore

  if(p_ads_info == NULL)
  {
    UI_PRINTF("ADS: ads desai p_ads_info null \n");
    return ret;
  }

  ads_get_type((ads_ad_type_t)p_ads_info->ads_type, &cmd, p_ads_info);

  switch(p_ads_info->ads_media_type)
  {
    case ADS_AD_TYPE_OSD:
    {
      UI_PRINTF("ADS: Desai ad type [OSD_TYPE] \n");
      memset(&stOsdShowInfo, 0, sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));
      p_ads_info->p_priv = (void *)&stOsdShowInfo;
      adv_ret = ads_io_ctrl(ADS_ID_ADT_DESAI, cmd, p_ads_info);
      if(adv_ret == DSAD_FUN_OK)
      {
        ret = ui_show_desai_osd_ad((u8 *)p_ads_info->p_priv);
      }
      else
        {
          UI_PRINTF("ADS: Desai ad type [OSD_TYPE] \n");
          is_ads_osd_ready = FALSE;
          //ui_ads_osd_roll_stop();   
        }
      break;
    }
    case ADS_AD_TYPE_PIC:
    {
      UI_PRINTF("ADS: Desai ad type [PIC_TYPE] \n");
      p_ads_info->p_priv = (void *)&stPicShowInfo;
      adv_ret = ads_io_ctrl(ADS_ID_ADT_DESAI, cmd, p_ads_info);
      if(adv_ret == DSAD_FUN_OK)
      {
        ret = ui_show_desai_pic_ad((u8 *)p_ads_info->p_priv);
      }
      break;
    }
    case ADS_AD_TYPE_AV:
    {
      UI_PRINTF("ADS: Desai ad type [AV_TYPE] \n");
      p_ads_info->p_priv = (void *)&stAvShowInfo;
      adv_ret = ads_io_ctrl(ADS_ID_ADT_DESAI, cmd, p_ads_info);
      if(adv_ret == DSAD_FUN_OK)
      {
        ret = ui_show_desai_vid_ad((u8 *)p_ads_info->p_priv);
      }
      break;
    }
    default:
    break;
    }
  return ret;
}

RET_CODE ui_adv_desai_hide(ads_ad_type_t ad_type, u8 *p_data)
{
//hide fullscreen ad
  if(is_full_screen_ad_on_showing)
  {
    UI_PRINTF("[UI_ADS_DIS] hide fullscreen ad \n");
    pic_adv_stop();
    is_full_screen_ad_on_showing = FALSE;
  }
  is_full_screen_ad_need_display = FALSE;
  return SUCCESS;
}

RET_CODE ui_update_desai_osd_ad(u8 *p_data)
{
  RET_CODE ret = SUCCESS;
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;
  
  OS_PRINTF("@@ui_adv_update_osd MSG !! \n");
  
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if(curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if(curn_mode == CURN_MODE_NONE)
    {
      return FALSE;
    }
  }
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);

  ret = ui_adv_display_osd_msg(prog_id);
  OS_PRINTF("@@ get prog success, update osd info via adv io_ctrl!ret = %d \n",ret);
  return ret;
}

RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ads_ad_type_t ad_type = (ads_ad_type_t)para1;
  u8 *p_data = (u8 *)para2;
  RET_CODE ret = ERR_FAILURE;

  if(NULL != p_data)
  {
    switch(ad_type)
    {
        case ADS_AD_TYPE_PIC:
        {
          UI_PRINTF("ADS: on_ads_process Desai ad type [PIC_TYPE] \n");
          ret = ui_show_desai_pic_ad(p_data);
          break;
        }
        case ADS_AD_TYPE_AV:
        {
          UI_PRINTF("ADS:on_ads_process  Desai ad type [AV_TYPE] \n");
          ret = ui_show_desai_vid_ad(p_data);
          break;
        }
        default:
          break;
        }
  }
  else
  {
    if(ADS_AD_TYPE_OSD == ad_type)
    {
      UI_PRINTF("ADS:on_ads_process  Desai ad type [OSD_UPDATE] \n");
      ret = ui_update_desai_osd_ad(p_data);
    }
    else
    {
      ret = ui_adv_desai_hide(ad_type, p_data);
    }
  }

  return ret;
}
