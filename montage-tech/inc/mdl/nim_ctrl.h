/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_CTRL_H_
#define __NIM_CTRL_H_

/*!
  Nim control class declaration
  */
typedef struct
{
  /*!
    \see nc_set_tp
    */
  void (*set_tp)(class_handle_t handle, void *p_extend,
                  nc_channel_info_t *p_tp_info, u8 tuner_id);
  /*!
    \see nc_get_tp
    */
  void (*get_tp)(class_handle_t handle, nc_channel_info_t *p_tp_info, 
                  u8 tuner_id);                  
  /*!
    \see nc_get_lock_status
    */
  BOOL (*get_lock_status)(class_handle_t handle, u8 tuner_id);
  /*!
    \see nc_is_finish_locking
    */
  BOOL (*is_finish_locking)(class_handle_t handle, u8 tuner_id);
  /*!
    \see nc_perf_get
    */
  void (*perf_get)(class_handle_t handle, nim_channel_perf_t *p_perf, u8 tuner_id);
  /*!
    \see nc_set_blind_scan_mode
    */
  void (*set_blind_scan_mode)(class_handle_t handle, BOOL bs, u8 tuner_id);
  /*!
    \see nc_enable_monitor
    */
  void (*enable_monitor)(class_handle_t handle, BOOL enable, u8 tuner_id);
  /*!
    \see lnb_check_monitor
    */
  void (*lnb_check_enable)(class_handle_t handle, BOOL enable, u8 tuner_id);
  /*!
    \see lnb_check_monitor
    */
  BOOL (*is_lnb_check_enable)(class_handle_t handle, u8 tuner_id);  
  /*!
    \see nc_disable_set_diseqc
    */
  void (*disable_set_diseqc)(class_handle_t handle, BOOL flag, u8 tuner_id);
  /*!
    \see nc_set_polarization
    */
  void (*set_polarization)(class_handle_t handle, nim_lnb_porlar_t polar, u8 tuner_id);
  /*!
    \see nc_set_22k
    */
  void (*set_22k)(class_handle_t handle, u8 onoff22k, u8 tuner_id);
  /*!
    \see nc_set_12v
    */
  void (*set_12v)(class_handle_t handle, u8 onoff12v, u8 tuner_id);
  /*!
    \see nc_tone_burst
    */
  BOOL (*tone_burst)(class_handle_t handle, nim_diseqc_mode_t mode, u8 tuner_id);
  /*!
    \see nc_switch_ctrl
    */
  void (*switch_ctrl)(class_handle_t handle,
          nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port, u8 tuner_id);
  /*!
    \see nc_positioner_ctrl
    */
  void (*positioner_ctrl)(class_handle_t handle,
        rscmd_positer_t cmd, u32 param, u8 tuner_id);
  /*!
    \see nc_usals_ctrl
    */
  BOOL (*usals_ctrl)(class_handle_t handle, double sat_longitude, 
          double local_longitude, double local_latitude, u8 tuner_id);
  /*!
    \see nc_set_diseqc
    */
  BOOL (*set_diseqc)(class_handle_t handle, nc_diseqc_info_t *p_diseqc, u8 tuner_id);

  /*!
    \see nc_cpy_diseqc
    */
  void (*cpy_diseqc_info)(class_handle_t handle, nc_diseqc_info_t *p_diseqc, u8 tuner_id);

  /*!
    \see nc_set_diseqc
    */
  void (*clr_diseqc_info)(class_handle_t handle, u8 tuner_id);

  /*!
    \see nc_clr_tp_info
    */
  void (*clr_tp_info)(class_handle_t handle, u8 tuner_id);

  /*!
    \see nc_clr_tp_info
    */
  void (*enter_monitor_idle)(class_handle_t handle, u8 tuner_id);

  /*!
    \see nc_set_tuner_ts
    */
  void (*set_tuner_ts)(class_handle_t handle, u8 tuner_0, u8 ts_0, u8 tuner_1, u8 ts_1);

  /*!
    \see nc_get_ts_by_tuner
    */
  void (*get_ts_by_tuner)(class_handle_t handle, u8 tuner_id, u8 *p_ts_in);
  /*!
    get the main tuner index
    \param[in] handle for nc service 
    \param[out] tuner id
    */
  void (*get_main_tuner_ts)(class_handle_t handle, u8 *p_tuner_id, u8 *p_ts_in); 
  /*!
    get tuner count 
    \param[in] handle for nc service 
    \param[out] none
    */
  u32 (*get_tuner_count)(class_handle_t handle);   
  /*!
    get nim dev handle by tuner indext 
    \param[in] handle for nc service, tuner index
    \param[out] nim dev handle
    */
  u32 (*get_nim_handle_by_tuner)(class_handle_t handle, u8 tuner_id);     
  /*!
    \see nc_set_lock_mode
    */
  void (*set_lock_mode)(class_handle_t handle, u8 tuner_id, sys_signal_t lock_mode);
  /*!
    Nim ctrl private data
    */
  void *p_data;
} nim_ctrl_t;

/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_set_tp(class_handle_t handle, u8 tuner_id, void *p_extend,
                  nc_channel_info_t *p_tp_info);
/*!
  Set channel tp for tunner scan operation
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] p_extend nim ctrl extern point
  \param[in] p_tp_info: channel information for NIM module to set
  */
void nc_get_tp(class_handle_t handle, u8 tuner_id, 
                nc_channel_info_t *p_tp_info);

/*!
  Determine the locking is finished or not
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  */
BOOL nc_is_finish_locking(class_handle_t handle, u8 tuner_id);

/*!
  Get the lock status of the tunner
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  */
BOOL nc_get_lock_status(class_handle_t handle, u8 tuner_id);

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[out] p_perf: channel performance
  */
void nc_perf_get(class_handle_t handle, u8 tuner_id,
                  nim_channel_perf_t *p_perf);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] bs TRUE is blind scan mode and false is not blind scan mode
  */
void nc_set_blind_scan_mode(class_handle_t handle, u8 tuner_id, BOOL bs);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_monitor(class_handle_t handle, u8 tuner_id, BOOL enable);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
void nc_enable_lnb_check(class_handle_t handle, u8 tuner_id, BOOL enable);

/*!
  Set blind scan mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is enable nc monitor
  */
BOOL nc_is_lnb_check_enable(class_handle_t handle, u8 tuner_id);

/*!
  Don't set diseqc when full tp scan.
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] enable TRUE is disable diseqc
  */
void nc_disable_set_diseqc(class_handle_t handle, u8 tuner_id, BOOL enable);

/*!
  Set nim_lnb_porlar_t mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] polar nim_lnb_porlar_t mode
  */
void nc_set_polarization(class_handle_t handle, u8 tuner_id, 
                          nim_lnb_porlar_t polar);

/*!
  Set 22k onoff
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] onoff22k 1 is on and 0 is off
  */
void nc_set_22k(class_handle_t handle, u8 tuner_id, u8 onoff22k);

/*!
  Set 12v onoff
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] onoff12v 1 is on and 0 is off
  */
void nc_set_12v(class_handle_t handle, u8 tuner_id, u8 onoff12v);

/*!
  Get the performance of certain channel
  
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
void nc_switch_ctrl(class_handle_t handle, u8 tuner_id, 
        nim_diseqc_level_t diseqc_level, rscmd_switch_mode_t mode, u8 port);

/*!
  set position
  
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] cmd: the cmd to disqec
  \param[in] param: cmd with paramter
  */
void nc_positioner_ctrl(class_handle_t handle, u8 tuner_id, 
      rscmd_positer_t cmd, u32 param);

/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] tuner id
  \param[in] sat_longitude: sat longitude
  \param[in] local_longitude: local longitude
  \param[in] local_latitude: local latitude
  */
void nc_usals_ctrl(class_handle_t handle, u8 tuner_id, double sat_longitude, 
                    double local_longitude, double local_latitude);

/*!
  Set  diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  \param[in] polar nim_lnb_porlar_t mode
  */
BOOL nc_set_diseqc(class_handle_t handle, u8 tuner_id, 
          nc_diseqc_info_t *p_diseqc);

/*!
  copy  diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  \param[in] polar nim_lnb_porlar_t mode
  */
void nc_cpy_diseqc_info(class_handle_t handle, u8 tuner_id, 
          nc_diseqc_info_t *p_diseqc);

/*!
  Clear diseqc info
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_clr_diseqc_info(class_handle_t handle, u8 tuner_id);

/*!
  Clear blind scan tp info
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_clr_tp_info(class_handle_t handle, u8 tuner_id);

/*!
  monitor enter idle
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_enter_monitor_idle(class_handle_t handle, u8 tuner_id);

/*!
  config the relation between TUNER & TS IN
  \param[in] handle for nc service 
  \param[in] tuner id
    \param[in] TS ID
  \param[in] tuner id
    \param[in] TS ID 
  */
void nc_set_tuner_ts(class_handle_t handle, u8 tuner_0, u8 ts_0, u8 tuner_1, u8 ts_1);

/*!
  get the ts index by tuner id
  \param[in] handle for nc service 
  \param[in] tuner id
  */
void nc_get_ts_by_tuner(class_handle_t handle, u8 tuner_id, u8 *p_ts_in);

/*!
  get the main tuner and ts index
  \param[in] handle for nc service 
  \param[in] tuner id
  \param[in] ts id  
  */
void nc_get_main_tuner_ts(class_handle_t handle, u8 *p_tuner_id, u8 *p_ts_in);

/*!
  get tuner count 
  \param[in] handle for nc service 
  \param[out] none
  */
u32 nc_get_tuner_count(class_handle_t handle);

/*!
  get nim dev handle by tuner indext 
  \param[in] handle for nc service, tuner index
  \param[out] nim dev handle
  */
u32 nc_get_nim_handle_by_tuner(class_handle_t handle, u8 tuner_id);

/*!
  Set lock mode
  \param[in] handle for nc 
  \param[in] tuner id
  \param[in] lock_mode dvbs,dvbt,dvbc
  */
void nc_set_lock_mode(class_handle_t handle, u8 tuner_id, sys_signal_t lock_mode);

#endif // End for __NIM_CTRL_H_

