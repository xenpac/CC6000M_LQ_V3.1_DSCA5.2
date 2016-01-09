/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "ads_ware.h"

// ap headers
#include "ap_framework.h"
//#include "ap_cas.h"
#include "sys_cfg.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "mtos_sem.h"

#include "data_manager.h"

#include "subt_pic_ware.h"
#include "ap_tr_subt.h"

#if 1
//subt_pic_module_cfg_t subt_pic_module_config = {0};
//u32 subt_pic_adm_id = SUBT_PIC_ID_ADT_TR;
static BOOL g_b_opened = FALSE;

BOOL is_tr_title_started()
{
  return g_b_opened;
}

RET_CODE  subt_pic_module_open(u32 m_id)
{
  cmd_t cmd;
//  cmd.id = SUBT_PIC_CMD_START_SYNC;
  cmd.id = SUBT_PIC_CMD_START;

  cmd.data1 = m_id;
  cmd.data2 = 0;

  if(!g_b_opened)
  {
    //OS_PRINTF("%s\n",__FUNCTION__);
    ap_frm_do_command(APP_RESERVED2, &cmd);

    g_b_opened = TRUE;
  }

  return SUCCESS;
}

RET_CODE  subt_pic_module_close(u32 m_id)
{
  cmd_t cmd;
  cmd.id = SUBT_PIC_CMD_CLOSE;
  cmd.data1 = m_id;
  cmd.data2 = 0;
  //if(g_b_opened)
  //{
    OS_PRINTF("%s\n",__FUNCTION__);
    ap_frm_do_command(APP_RESERVED2, &cmd);

    //g_b_opened = FALSE;
  //}
  return SUCCESS;
}


RET_CODE  subt_pic_module_stop(u32 m_id)
{
  cmd_t cmd;
  cmd.id = SUBT_PIC_CMD_STOP_SYNC;
  cmd.data1 = m_id;
  cmd.data2 = 0;

  if(g_b_opened)
  {
    //OS_PRINTF("%s\n",__FUNCTION__);
    ap_frm_do_command(APP_RESERVED2, &cmd);

    g_b_opened = FALSE;
  }
  return SUCCESS;
}
#endif
#ifdef TR_HANGAD
extern RET_CODE ui_trhangad_set_pic(u16 pos_x,u16 pos_y,u32 size, u8 *pic_add);
#endif
void subt_pic_module_init(subt_pic_module_cfg_t cfg)
{
  RET_CODE ret;
  subt_pic_module_cfg_t subt_pic_module_config;
  u32 adm_id = SUBT_PIC_ID_ADT_TR;

  subt_pic_module_config.task_prio_start = cfg.task_prio_start;
  subt_pic_module_config.task_prio_end = cfg.task_prio_end;
  subt_pic_module_config.subt_is_scroll = cfg.subt_is_scroll;
  subt_pic_module_config.osd_display_is_ok = cfg.osd_display_is_ok;
  subt_pic_module_config.channel_info_set = cfg.channel_info_set;
  subt_pic_module_config.scroll_step_pixel_set = cfg.scroll_step_pixel_set;
  subt_pic_module_config.draw_rectangle = cfg.draw_rectangle;
  subt_pic_module_config.draw_picture = cfg.draw_picture;
  #ifdef TR_HANGAD
  subt_pic_module_config.read_gif = ui_trhangad_set_pic;
  #endif
  #ifndef WIN32
  ret = subt_pic_adt_tr_attach(&subt_pic_module_config,&adm_id);
  #endif
  MT_ASSERT(SUCCESS == ret);
  //subt_pic_module_open(0);
  //subt_pic_init(SUBT_PIC_ID_ADT_TR,&subt_pic_module_config);
  //mtos_task_sleep(500);
  //subt_pic_open(SUBT_PIC_ID_ADT_TR);
}

subt_policy_t* construct_subt_policy(void)
{
  //subt_pic_module_cfg_t cfg;
  subt_policy_t *p_policy = mtos_malloc(sizeof(subt_policy_t));

  return p_policy;
}


