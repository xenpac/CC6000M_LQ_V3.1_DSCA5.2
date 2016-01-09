/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __NIM_CTRL_SVC_H_
#define __NIM_CTRL_SVC_H_

/*!
  NIM diseq port shift
  */
#define NIM_DISEQC_TYPE_SHIFT (5)
/*!
  The marcos function to generate parameter of positioner control command
  */
#define RS_PSTER_PARAM(mode, data) ((u32)((mode)<<16)|(u32)((data) & 0xFF))

/*!
  Enable dual tuner function or not
  */
#define ENABLE_DUAL_TUNER  1

/*!
  tuner id define
  */
typedef enum
{
/*!
  Define tuner 0
  */
  TUNER0 = 0,
  /*!
  Define tuner 1
  */
  TUNER1 = 1,
  /*!
  Define tuner number
  */
  TUNER_NUM
}nim_tuner_id_t;

/*!
  system signal
  */
typedef enum
{
  /*!
    DVBS
    */
  SYS_DVBS = 0,
  /*!
    DVBC
    */
  SYS_DVBC,
  /*!
    DVBT2-DVBT
    */
  SYS_DVBT2,
  /*!
    ABS-S
    */
  SYS_ABSS,
  /*!
    DTMB
    */
  SYS_DTMB,
  /*!
    support DVB-S,DVB-C,DVB-T all in one
    */
  SYS_DVB_3IN1
} sys_signal_t;

/*!
  NC service commands
  */
typedef enum
{
  /*!
    lock tuner 0
    */
  NC_LOCK_TUNER0,
  /*!
    lock tuner 1
    */  
  NC_LOCK_TUNER1
} nc_svc_cmd_t;

/*!
  \file nim_ctrl_svc.h
  
  nim_ctrl provides the interface to tunner control module
  And all the operation to tunner should be implemented in this api
  */
/*!
  Event definitions in NIM CONTROL
  */
typedef enum
{
  /*!
    Start NIM control
    */
  NC_EVT_BEGIN = (MDL_NIM_CTRL << 16),
  /*!
    Frequency is locked
    */  
  NC_EVT_LOCKED,
  /*!
    Frequency is unlocked
    */
  NC_EVT_UNLOCKED,
  /*!
    Turning position
    */
  NC_EVT_POS_TURNING,
  /*!
    Out of rangle
    */
  NC_EVT_POS_OUT_RANGE
} nc_evt_t;

/*!
  Switch control mode, applied in DiSEqC x.1
  */  
typedef enum
{
  /*!
    Send command once 
    */
  RSCMD_SWITCH_NO_REPEAT = 0,
  /*!
    Send command twice 
    */
  RSCMD_SWITCH_ONE_REPEAT,
  /*!
    Send command thrice 
    */
  RSCMD_SWITCH_TWO_REPEATS
}rscmd_switch_mode_t;

/*!
  Positioner control cammand, applied in DiSEqC x.2
  */
typedef enum
{
  /*!
    stop positioner movement
    */
  RSCMD_PSTER_STOP = 0x60,
  /*!
    Disable limits 
    */
  RSCMD_PSTER_LIMIT_OFF = 0x63,
  /*!
    set east limit
    */
  RSCMD_PSTER_LIMIT_E = 0x66,
  /*!
    set west limit
    */
  RSCMD_PSTER_LIMIT_W = 0x67,
  /*!
    drive motor east
    */
  RSCMD_PSTER_DRV_E = 0x68,
  /*!
    drive motor west
    */
  RSCMD_PSTER_DRV_W = 0x69,
  /*!
    store satellite position and enable limits
    */
  RSCMD_PSTER_STORE_NN = 0x6A,
  /*!
    Drive Motor to Satellite Position nn
    */
  RSCMD_PSTER_GOTO_NN = 0x6B,
  /*!
    Goto Angular Position
    */
  RSCMD_PSTER_GOTO_ANG = 0x6E,
  /*!
    Re-calculate
    */
  RSCMD_PSTER_RECALCULATE = 0x6F
}rscmd_positer_t;

/*!
  Positioner drive mode, it is set to recver_positioner_ctrl 
  3rd parameter's high 16 bits 
  */
typedef enum
{
  /*!
    drive positioner continuously
    */
  RSMOD_PSTER_DRV_CONTINUE = 0,
  /*!
    drive positioner with time out in s, 1 ~ 127
    */
  RSMOD_PSTER_DRV_TIMEOUT,
  /*!
    drive positioner with steps, 1 ~ 128
    */
  RSMOD_PSTER_DRV_STEPS
}rsmod_positer_drv_t;

/*!
  Positioner re-calculate mode, 
  it is set to recver_positioner_ctrl 3st parameter's high 16 bits 
  */
typedef enum
{
  /*!
    re-calculate default function by positioner device
    */
  RSMOD_PSTER_RECAL_DEF = 0,
  /*!
    re-calculate by Satellite Number 
    */
  RSMOD_PSTER_RECAL_SAT_NUM,
  /*!
    re-calculate by Site Longitude
    */
  RSMOD_PSTER_RECAL_X_VALUE,
  /*!
    re-calculate by Site Latitude
    */
  RSMOD_PSTER_RECAL_Y_VALUE
}rsmod_positer_recal_t;

/*!
  nim ctrl disepc info
  */
typedef struct
{
  /*! 
    the montor is fixed
    */
  BOOL is_fixed;
  /*!
    If not zero means support position
    */
  u32 position_type;
  /*!
    Enabled by sat_position_type
    */
  u32 position;
  /*!
    If not zero means support diseqc 1.0
    */
  u32 diseqc_type_1_0;
  /*!
    Diseqc port value
    */
  u32 diseqc_port_1_0;
  /*!
    If not zero means support diseqc 1.1
    */
  u32 diseqc_type_1_1;
  /*!
    Diseqc port value
    */
  u32 diseqc_port_1_1;
  /*! 
    use diseqc1.2
    */
  BOOL used_DiSEqC12;
  /*!
    sat longitude
    */
  double d_sat_longitude;
  /*!
    local longitude
    */
  double d_local_longitude;
  /*!
    local latitude
    */
  double d_local_latitude;
  /*!
    diseqc x.1 switch's mode, 0: no feature, 1: Mode1, 2:Mode2, 3:Mode3
    */
  u8 diseqc_1_1_mode;

  /*!
    the position location is change
    */
  BOOL position_change;

} nc_diseqc_info_t;

/*!
  nim ctrl parameter
  */
typedef struct
{
  /*!
    tuner polarization. /see nim_lnb_porlar_t
    */
  nim_lnb_porlar_t polarization;
  /*!
    nim 22KHz tone signal .0: off; 1: on
    */
  u8 onoff22k;
  /*!
    nim channel information
    */
  nim_channel_info_t channel_info;
}nc_channel_info_t;

/*!
  NIM ctrl scan parameter
  */
typedef struct
{
  /*!
    Channel list for nim scan operation
    */
  nim_channel_info_t *p_ch_info;
  /*!
    Maximum channel number
    */
  u16 max_ch_num;
  /*!
    start frequency (KHZ) for current nim scan operation
    */
  u32 start_freq_khz;
  /*!
    End frequency (KHZ) for current nim scan operation
    */
  u32 end_freq_khz;
  /*!
    Next frequency for NIM scan operation
    */
  u32 nxt_frq_khz;
}nc_scan_info_t;


/*!
  NIM ctrl svc config
  */
typedef struct
{
  /*!
    task priority.
    */
  u32 priority;
  /*!
    task stak size.
    */
  u32 stack_size;
  /*!
    sync lock or not.
    */
  BOOL b_sync_lock;
  /*!
    Lock mode
    */
  sys_signal_t lock_mode;
  /*!
    auto IQ
    */
  BOOL auto_iq;    
   /*!
     wait_ticks
     */
  u32 wait_ticks;
   /*!
     maxticks_cnk
     */   
  u32 maxticks_cnk;
  /*!
    mutex lock mode
    */ 
  u32 mutex_type;
  /*!
    if dvbs-2 support
    */
  BOOL dvbs_2_support;    
  /*!
    tuner count: only support 1 or 2, the tuner 1 is the master, and the tuner 2 is slave
    */
  u32 tuner_count;  
  /*!
    nim dev used to specified function such as LNB_EN, LNB_VSEL, DISEQC_OUT, 22K.
    */
  BOOL b_used_spec;   
  /*!
    dvbt auto lock mode
    */ 
  u32 dvbt_lock_mode;    
}nc_svc_cfg_t;
/*!
  Get  nc service instance
  \param[in] handle for nc service 
  \param[in] context user
  */
service_t * nc_get_svc_instance(class_handle_t handle, u32 context);

/*!
  Remove nc service instance
  \param[in] handle for nc service 
  \param[in] p_svc server
  */
void nc_remove_svc_instance(class_handle_t handle, service_t *p_svc);

/*!
  Initialize NIM control module  
  
  \param[in] p_cfg nc config
  
  \return nim ctrl service's handle
  */
handle_t nc_svc_init(nc_svc_cfg_t *p_cfg_0, nc_svc_cfg_t *p_cfg_1);

/*!
  Initialize NIM control module  
  
  \param[in] p_cfg nc config
  
  \return nim ctrl service's handle
  */
handle_t nc_svc_init_without_diseqc12(nc_svc_cfg_t *p_cfg_0, nc_svc_cfg_t *p_cfg_1);
#endif // End for __NIM_CTRL_SVC_H_

