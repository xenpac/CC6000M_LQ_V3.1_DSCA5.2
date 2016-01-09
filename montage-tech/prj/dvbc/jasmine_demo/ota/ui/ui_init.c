/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
/**
 * \file ui_init.c
 *
 * This file implemented the entry function of shadow project which is the
 * default project of dvbs products
 */

#include "ui_common.h"
#include "data_manager.h"
#include "rsc.h"
#include "video.h"
#include "pti.h"
#include "dvb_svc.h"
//#include "ap_galaxy_ota.h"

#include "ota_public_def.h"
#include "ota_dm_api.h"
#include "ui_ota_api_v2.h"
#include "common.h"
#include "mem_cfg.h"
#include "display.h"
#include "../../includes/customer_config.h"
#include "af.h"

static RET_CODE mmi_init(void)
{
  handle_t rsc_handle = 0;
  rect_t flinger_rect = 
  {
    0,
    0,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
  };
  flinger_cfg_t bot = 
  {
    SURFACE_OSD1,
    1,
    &flinger_rect,
  };
  rect_t screen_rect =
  {
    SCREEN_POS_PAL_L,
    SCREEN_POS_PAL_T,
    SCREEN_POS_PAL_L + SCREEN_WIDTH,
    SCREEN_POS_PAL_T + SCREEN_HEIGHT,
  };
  gdi_cfg_t gdi_info =
  {
    &screen_rect,
    NULL,
    &bot,
    COLORFORMAT_ARGB8888,   //osd format.
    NULL,                   //palette
    C_TRANS,                //transparent color.
    C_KEY,                  //color key.
    0,                      //virtual buffer address.
    0,                      //virtual buffer size.
    MAX_CLIPRECT_CNT,       //clip rect count.
    MAX_DC_CNT,             //dc count.
    0,                      //animatio buffer address.
    0,                      //animation buffer size.
  };
  
  paint_param_t paint_param = {0};
  af_cfg_t af_cfg = {0};


  mmi_init_heap(GUI_PARTITION_SIZE);

  g_rsc_config.rsc_data_addr = (u32)ota_rsc_data;

  rsc_handle = rsc_init(&g_rsc_config);
  UI_PRINTF("\r\n ota rsc_init is ok ");

  af_cfg.bpp = 32;
  af_cfg.cache_cnt = 10;
  af_cfg.cache_size = 36 * 36 * 4;
  af_attach(rsc_handle, &af_cfg);
  
#if 0
    if(handle_dm_get_customer_module_cfg(EXTERNAL_CHINESE_OTA))
      rsc_set_curn_language(rsc_handle, LANGUAGE_CHINESE);
    else
   #endif 

    rsc_set_curn_language(rsc_handle, LANGUAGE_CHINESE);

  gdi_info.vsurf_buf_addr = 0;
  //MT_ASSERT(gdi_info.vsurf_buf_addr != 0);

  gdi_info.vsurf_buf_size = 0;

  //mem_mgr_release_block(BLOCK_OSD1_32BIT_BUFFER);

  gdi_init(&gdi_info);

  gdi_set_global_alpha(255);

  ctrl_init_lib(MAX_CLASS_CNT, MAX_OBJ_CNT);
  ctrl_str_init(MAX_CTRL_STRING_BUF_SIZE);

  cont_register_class(MAX_OBJECT_NUMBER_CNT);
  cbox_register_class(MAX_OBJECT_NUMBER_CBB);
  text_register_class(MAX_OBJECT_NUMBER_TXT);
  pbar_register_class(MAX_OBJECT_NUMBER_PGB);
  nbox_register_class(MAX_OBJECT_NUMBER_NUM);

  paint_param.max_str_len = MAX_PAINT_STRING_LENGTH;
  paint_param.max_str_lines = MAX_PAINT_STRING_LINES;
  paint_param.rsc_handle = rsc_handle;

  gui_paint_init(&paint_param);

  UI_PRINTF("\r\n ota all classes is ok\n");
  return SUCCESS;
}


void close_fast_logo(void)
{
  static BOOL closed = FALSE;

  if(!closed)
  {
    void  *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_DISPLAY);

    disp_layer_show(p_dev, DISP_LAYER_ID_STILL_HD, FALSE);
    closed = TRUE;
  }
}


extern void ui_ota_api_auto_mode_start(ota_work_t  mode);
static void ui_main_proc(void *p_param)
{
  close_fast_logo();

  mmi_init();

  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
    ui_ota_api_bootload_info_init();
  }
  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID) == FALSE)
    {
     ui_ota_api_ota_param_init();
    }
  
  {
   u32 upg_version = 0;
   u16 block_version = 0;
  OS_PRINTF("************ota veriosn info**************\n");
  upg_version = ui_ota_api_get_upg_check_version();
  OS_PRINTF("upg check version:%d\n",upg_version);
  block_version = ui_ota_api_get_block_version(0xFF);
  OS_PRINTF("all_app      version:%d\n",block_version);
  block_version = ui_ota_api_get_block_version(DM_MAINCODE_BLOCK_ID);
  OS_PRINTF("maincode  version:%d\n",block_version);
   block_version = ui_ota_api_get_block_version(DM_LOGO_PRESET_BLOCK_ID);
  OS_PRINTF("logo preset version:%d\n",block_version);
  block_version = ui_ota_api_get_block_version(OTA_BLOCK_ID);
  OS_PRINTF("ota            version:%d\n",block_version);
  OS_PRINTF("**************************************\n");
  }

  ui_desktop_init();
  
  ui_ota_api_init();
  ui_ota_api_auto_mode_start(OTA_WORK_SELECT_AUTO);
  OS_PRINTF("####DEBUG do finish auto mode\n");
  //manage_open_menu(ROOT_ID_OTA_USER_INPUT, 0, 0); /*add for test*/

  ui_desktop_main();
}


void ui_init(void)
{
  u32 *pstack_pnt = (u32 *)mtos_malloc(UI_FRM_TASK_STKSIZE);

  MT_ASSERT(pstack_pnt != NULL);
  memset(pstack_pnt, 0, UI_FRM_TASK_STKSIZE);

  /* start app task */
  mtos_task_create((u8 *)"ui_frm",
                   ui_main_proc,
                   NULL,
                   UI_FRM_TASK_PRIORITY,
                   pstack_pnt,
                   UI_FRM_TASK_STKSIZE);
}
