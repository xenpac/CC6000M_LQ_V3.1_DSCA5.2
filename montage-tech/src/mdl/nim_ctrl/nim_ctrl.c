/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_usals.h"
#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_set_tp(class_handle_t handle, u8 tuner_id, void *p_extend,
                  nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_tp != NULL);
  p_this->set_tp(handle, p_extend, p_tp_info, tuner_id);
}

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_get_tp(class_handle_t handle, u8 tuner_id, nc_channel_info_t *p_tp_info)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->get_tp != NULL);
  p_this->get_tp(handle, p_tp_info, tuner_id);
}

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  */
BOOL nc_get_lock_status(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->get_lock_status != NULL);
  return p_this->get_lock_status(handle, tuner_id);
}

/*!
  Determine the locking is finished or not
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  */
BOOL nc_is_finish_locking(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->is_finish_locking != NULL);
  return p_this->is_finish_locking(handle, tuner_id);
}


/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[out] p_perf: channel performance
  */
void nc_perf_get(class_handle_t handle, u8 tuner_id, 
  nim_channel_perf_t *p_perf)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->perf_get != NULL);
  p_this->perf_get(handle, p_perf, tuner_id);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] bs TRUE is blind scan mode and false is not blind scan mode
  */
void nc_set_blind_scan_mode(class_handle_t handle, u8 tuner_id, BOOL bs)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_blind_scan_mode != NULL);
  p_this->set_blind_scan_mode(handle, bs, tuner_id);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_monitor(class_handle_t handle, u8 tuner_id, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->enable_monitor != NULL);
  p_this->enable_monitor(handle, enable, tuner_id);
}


/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_lnb_check(class_handle_t handle, u8 tuner_id, BOOL enable)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->lnb_check_enable != NULL);
  p_this->lnb_check_enable(handle, enable, tuner_id);
}

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
BOOL nc_is_lnb_check_enable(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->is_lnb_check_enable != NULL);
  return p_this->is_lnb_check_enable(handle, tuner_id);
}

/*!
  disable set diseqc when set tp on full scan
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] flag TRUE is disable diseqc
  */
void nc_disable_set_diseqc(class_handle_t handle, u8 tuner_id, BOOL flag)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->disable_set_diseqc!= NULL);
  p_this->disable_set_diseqc(handle, flag, tuner_id);
}
  
/*!
  Set nim_lnb_porlar_t mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] polar nim_lnb_porlar_t mode
  */
void nc_set_polarization(class_handle_t handle, u8 tuner_id, 
  nim_lnb_porlar_t polar)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_polarization != NULL);
  p_this->set_polarization(handle, polar, tuner_id);
}

/*!
  Set 22k onoff
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] onoff22k 1 is on and 0 is off
  */
void nc_set_22k(class_handle_t handle, u8 tuner_id, u8 onoff22k)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_22k != NULL);
  p_this->set_22k(handle, onoff22k, tuner_id);
}

/*!
  Set 12v onoff
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] onoff12v 1 is on and 0 is off
  */
void nc_set_12v(class_handle_t handle, u8 tuner_id, u8 onoff12v)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_12v != NULL);
  p_this->set_12v(handle, onoff12v, tuner_id);
}


/*!
  Get the performance of certain channel
  
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
void nc_switch_ctrl(class_handle_t handle, u8 tuner_id, 
  nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->switch_ctrl != NULL);
  p_this->switch_ctrl(handle, diseqc_level, mode, port, tuner_id);
}

/*!
  set position
  
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] cmd: the cmd to disqec
  \param[in] param: cmd with paramter
  */
void nc_positioner_ctrl(class_handle_t handle, u8 tuner_id, 
  rscmd_positer_t cmd, u32 param)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->positioner_ctrl != NULL);
  p_this->positioner_ctrl(handle, cmd, param, tuner_id);
}

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] sat_longitude: sat longitude
  \param[in] local_longitude: local longitude
  \param[in] local_latitude: local latitude
  */
void nc_usals_ctrl(class_handle_t handle, u8 tuner_id, double sat_longitude,
  double local_longitude, double local_latitude)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->usals_ctrl != NULL);
  p_this->usals_ctrl(handle, sat_longitude,
    local_longitude, local_latitude, tuner_id);
}

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  \param[in] polar nim_lnb_porlar_t mode
  \return TRUE: need wait time when moving dish FALSE: no need waiting
  */
BOOL nc_set_diseqc(class_handle_t handle, u8 tuner_id,
  nc_diseqc_info_t *p_diseqc)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_diseqc != NULL);
  return p_this->set_diseqc(handle, p_diseqc, tuner_id);
}

/*!
  copy diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_cpy_diseqc_info(class_handle_t handle, u8 tuner_id, 
          nc_diseqc_info_t *p_diseqc)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->clr_diseqc_info != NULL);
  p_this->cpy_diseqc_info(handle, p_diseqc, tuner_id);
}

/*!
  Clear diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_clr_diseqc_info(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->clr_diseqc_info != NULL);
  p_this->clr_diseqc_info(handle, tuner_id);
}

/*!
  Clear blind scan tp info
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_clr_tp_info(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->clr_tp_info != NULL);
  p_this->clr_tp_info(handle, tuner_id);
}

/*!
  monitor enter idle
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_enter_monitor_idle(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->enter_monitor_idle != NULL);
  p_this->enter_monitor_idle(handle, tuner_id);
}

/*!
  config the relation between TUNER & TS IN
  \param[in] handle for nc service 
  \param[in] tuner id
    \param[in] TS ID
  \param[in] tuner id
    \param[in] TS ID 
  */
void nc_set_tuner_ts(class_handle_t handle, u8 tuner_0, u8 ts_0, u8 tuner_1, u8 ts_1)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  //MT_ASSERT(p_this->enter_monitor_idle != NULL);
  p_this->set_tuner_ts(handle, tuner_0, ts_0, tuner_1, ts_1);
}

/*!
  get the ts index by tuner id
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_get_ts_by_tuner(class_handle_t handle, u8 tuner_id, u8 *p_ts_in)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  //MT_ASSERT(p_this->enter_monitor_idle != NULL);
  p_this->get_ts_by_tuner(handle, tuner_id, p_ts_in);
}

/*!
  get the main tuner index
  \param[in] handle for nc service 
  \param[out] tuner id
  */
void nc_get_main_tuner_ts(class_handle_t handle, u8 *p_tuner_id, u8 *p_ts_in)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  //MT_ASSERT(p_this->enter_monitor_idle != NULL);
  p_this->get_main_tuner_ts(handle, p_tuner_id, p_ts_in);
}  

/*!
  get tuner count 
  \param[in] handle for nc service 
  \param[out] none
  */
u32 nc_get_tuner_count(class_handle_t handle)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);

  return p_this->get_tuner_count(handle);
}

/*!
  get nim dev handle by tuner indext 
  \param[in] handle for nc service, tuner index
  \param[out] nim dev handle
  */
u32 nc_get_nim_handle_by_tuner(class_handle_t handle, u8 tuner_id)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);

  return p_this->get_nim_handle_by_tuner(handle, tuner_id);
}

/*!
  Set lock mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] lock_mode dvbs,dvbt,dvbc
  */
void nc_set_lock_mode(class_handle_t handle, u8 tuner_id, sys_signal_t lock_mode)
{
  nim_ctrl_t *p_this = handle;
  MT_ASSERT(p_this != NULL);
  MT_ASSERT(p_this->set_lock_mode != NULL);
  p_this->set_lock_mode(handle, tuner_id, lock_mode);
}

