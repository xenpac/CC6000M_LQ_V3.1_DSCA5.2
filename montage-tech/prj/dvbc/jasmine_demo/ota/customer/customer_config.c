/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

// standard library
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_warriors.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "hdmi.h"
#include "region.h"
#include "display.h"
#include "pdec.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nit.h"
#include "pat.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#include "dsmcc.h"
//#include "dsmcc_galaxy.h"
//#include "cas_ware.h"
#include "monitor_service.h"
#include "dmx.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"
#include "surface.h"
//eva
#include "interface.h"
#include "eva.h"
// ap
#include "ap_framework.h"

#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
//#include "ap_dvbs_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
//#include "ap_cas.h"
//#include "ap_ota.h"
//#include "ap_twin_port.h"

#include "ota_public_def.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "demux_filter.h"
#include "flash_sink_filter.h"
#include "ota_filter.h"
#include "ota_dm_api.h"
#include "db_dvbs.h"
#include "../../sysstatus/sys_status.h"
#include "mem_cfg.h"
#include "../../includes/customer_config.h"

static EXT_CUS_SYS_CFG sys_cfg;
static hw_cfg_t hw_cfg;

void dm_load_customer_sys_cfg(void)
{
  int read_len = 0;

  #if 0
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     CUSTOMER_BLOCK_ID, 0, 0,
                     sizeof(EXT_CUS_SYS_CFG),
                     (u8 *)&sys_cfg);
  #else
   read_len = mul_ota_dm_api_dm_read(CUSTOMER_BLOCK_ID,
                        0,
                        sizeof(EXT_CUS_SYS_CFG),
                     (u8 *)&sys_cfg);
  #endif
  MT_ASSERT(read_len == sizeof(EXT_CUS_SYS_CFG));
  return;
}

u8 handle_dm_get_customer_module_cfg(EXT_CUS_MODULE_CFG ext_customer_cfg)
{
   return ((sys_cfg.customer_module_cfg >> ext_customer_cfg) & 1);
}

void dm_load_customer_hw_cfg(void)
{
  int read_len = 0;

  #if 0
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     HW_CFG_BLOCK_ID, 0, 0,
                     sizeof(hw_cfg_t),
                     (u8 *)&hw_cfg);
  #else
  read_len = mul_ota_dm_api_dm_read(HW_CFG_BLOCK_ID,
                        0,
                        sizeof(hw_cfg_t),
                        (u8 *)&hw_cfg);
  #endif
  MT_ASSERT(read_len == sizeof(hw_cfg_t));
  return;
}
hw_cfg_t dm_get_hw_cfg_info(void)
{
  return hw_cfg;
}

