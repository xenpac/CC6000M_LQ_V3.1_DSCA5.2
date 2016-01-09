/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "mem_manager.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"
#include "mdl.h"
#include "lib_util.h"
#include "lib_usals.h"
#include "lzmaif.h"
#include "hal_misc.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "dvbt_util.h"
#include "drv_dev.h"
#include "ss_ctrl.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "data_manager.h"
#include "dvb_protocol.h"

#include "ap_framework.h"

/*!
  DVB OTA virtual NIM nimfilter command definition
  */
typedef enum
{
  /*!
    OTA start locking tp:
    for UI to start OTA with locking tp, used when user input done.
    */
  NIM_FILTER_NO,
  /*!
    OTA start locking tp:
    for UI to start OTA with locking tp, used when user input done.
    */
  NIM_FILTER_OTA,
  /*!
    Exit OTA:
    for UI to exit OTA, and reboot to maincode.
    */  
  NIM_FILTER_OTHER,
}nim_filter_modu_em_t;

/*!
  DVB OTA virtual NIM nimfilter command definition
  */
typedef enum
{
  /*!
    OTA start locking tp:
    for UI to start OTA with locking tp, used when user input done.
    */
  NIM_FILTER_CMD_START_LOCK = ASYNC_CMD,
  /*!
    filter stop:
    */  
  NIM_FILTER_CMD_STOP,
    /*!
    set tuner id:
    */  
  NIM_FILTER_CMD_SET_TUNER,
}nim_filter_cmd_em_t;

/*!
   NIM_FILTER virtual api State Machine definition
  */
typedef enum
{
  /*!
    NIM_FILTER idle state.
    */
  NIM_FILTER_SM_IDLE = 0,
  /*!
    NIM_FILTER try to locking.
    */
  NIM_FILTER_SM_LOCKING,
  /*!
    NIM_FILTER wait lock.
    */
  NIM_FILTER_SM_WAIT_LOCK,
    /*!
    NIM_FILTER wait lock.
    */
  NIM_FILTER_SM_LOCK_MONITOR,
}nim_filter_sm_t;

/*!
  nim info
  */
typedef struct tg_lock_info_t
{
/*!
  Lock mode
  */
  sys_signal_t lock_mode;
  /*!
    dvbs lock info.
    */
  dvbs_lock_info_t locks;
  /*!
    dvbc lock info.
    */
  dvbc_lock_info_t lockc;
  /*!
    dvbt lock info
    */
  dvbt_lock_info_t lockt;
}lock_info_t;

/*!
   otaprivate data
  */
typedef struct
{
/*!
  ota state machine state
  */
  nim_filter_sm_t sm;
/*!
  DVB service handle
  */
  service_t *p_nim_svc;
/*!
  nc channel info
  */
  nc_channel_info_t nc_info;
/*!
  lock info
  */
  lock_info_t lock_info;
/*!
  nim filter lock status message process
  */
 void(*do_lock_result)(BOOL is_lock);
/*!
  virtual nim filter app;
  */
  app_id_t app_id;
/*!
  virtual nim filter start bit;
  */
  BOOL g_start_bit;
  /*!
    virtual nim filter start bit;
    */
  nim_device_t *p_nim_dev;
/*!
  virtual nim filter monitor start ticks;
  */
  u32 start_ticks;
/*!
  virtual nim filter moniter duration ticks;
  */
  u32 duration_ticks;
/*!
  virtual nim filter lock result bit;
  */
  BOOL g_locked;
/*!
  spectral I/Q porlarity, see nim_spectral_polar_t
  */    
  u8 spectral_polar_c;
/*!
  dvbt2 nim type
  */
  u32 nim_type;
/*!
  nim filter do set dvbs sat ctrl
  */
  void(*do_set_dvbs_sat_ctrl)(dvbs_lock_info_t *p_lock_info);
} nim_filter_priv_data_t;

/*!
   dvbs ota handle
  */
typedef struct
{
  /*!
     ota instance
    */
  app_t instance;
  /*!
     private data
    */
  nim_filter_priv_data_t data;
} nim_filter_handle_t;

/*!
   Initialization function.
   \param[in] handle ota handle
  */
static void init(class_handle_t p_handler)
{
  nim_filter_handle_t *p_handle = (nim_filter_handle_t *)p_handler;
  nim_filter_priv_data_t *p_data = &p_handle->data;
  p_data->duration_ticks = 200;  //200 ticks = 2s
  p_data->start_ticks = mtos_ticks_get();
}
/*!
   get nim filter handle.
  */
static nim_filter_handle_t *nim_filter_get_handle(void)
{
  return  (nim_filter_handle_t *)class_get_handle_by_id(NIM_VIRTUAL_CLASS_ID);
}

/*!
  diseqc ctrl.
  */
static void vitual_nim_filter_diseqc_ctrl(nim_diseqc_cmd_t *p_diseqc_cmd,
  BOOL lnb_check)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
  u8 param = 0;

  if(lnb_check == FALSE)
  {
    nim_diseqc_ctrl(p_data->p_nim_dev, p_diseqc_cmd);
  }
  else
  {
    //to detect
    dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_CHECK_LNB_SC_PROT, (u32)&param);

    if(param == NIM_LNB_SC_NO_PROTING)
    {
      //remove protect.
      dev_io_ctrl(p_data->p_nim_dev, NIM_REMOVE_PROTECT, 0);
      //nim diseqc control
      nim_diseqc_ctrl(p_data->p_nim_dev, p_diseqc_cmd);
      mtos_task_delay_ms(100);
      //reset
      dev_io_ctrl(p_data->p_nim_dev, NIM_ENABLE_CHECK_PROTECT, 0);
    }
  }
}
/*!
  Get the performance of certain channel
  \param[in] handle: nim ctrl handle
  \param[in] diseqc_level: the level of DiSEqC protocol this switch used
  \param[in] mode: switch control mode, applied in DiSEqC x.1
  \param[in] port: switch port to be selected, start form 0
  */
static void vitual_nim_filter_switch_ctrl(
        nim_diseqc_level_t diseqc_level, 
        rscmd_switch_mode_t mode, 
        u8 port,u8 is_on22k,u8 polarity)
{
  nim_diseqc_cmd_t diseqc_cmd = {0};
  u8 buf[4] = {0};
  u8 i = 0;
  u8 repeat_times = 1;
  u8 is_22k_on = 0;
  u8 pol_status = 0;

  diseqc_cmd.mode = NIM_DISEQC_BYTES;
  diseqc_cmd.tx_len = 4;
  diseqc_cmd.p_tx_buf = buf;

  buf[0] = 0xE0;
  buf[1] = 0x10;

  is_22k_on = is_on22k;
  if(is_22k_on != 1)
  {
    is_22k_on = 0;
  }

  if(polarity == NIM_PORLAR_HORIZONTAL)
  {
    pol_status = 1;
  }
  else
  {
    pol_status = 0;
  }

  switch(diseqc_level)
  {
    case NIM_DISEQC_LEVEL_X0:
      MT_ASSERT(port < 4);
      /* X.0, only support committed switch */
      //OS_PRINTF("port [%d], pol_status [%d], is_22k_on[%d]\n",
       // port, pol_status, is_22k_on);
      buf[2] = 0x38;
      buf[3] = 0xF0 + port * 4 + pol_status * 2 + is_22k_on;
      //OS_PRINTF("Command content %x\n", buf[3]);
      break;

    default:
      /* level above X.0, support uncommitted switch */
      MT_ASSERT(port < 16);
      /* uncommitted switch */
      buf[2] = 0x39;
      buf[3] = 0xF0 + port;
      if(mode != RSCMD_SWITCH_TWO_REPEATS)
      {
        repeat_times = 2;
      }
      else
      {
        repeat_times = 3;
      }
      break;
  }

  for(i = 0; i < repeat_times; i++)
  {
    //delay 20ms
    mtos_task_delay_ms(20);
    vitual_nim_filter_diseqc_ctrl(&diseqc_cmd,TRUE);
    //delay 20ms
    mtos_task_delay_ms(20);
  }
}
/*!
  DiSEqC control for positioner
  \param[in] handle: nim ctrl handle
  \param[in] cmd: the positioner cmd to DiSEqC
  \param[in] param: cmd with paramter
  */
static void vitual_nim_filter_positioner_ctrl(
      rscmd_positer_t cmd, u32 param)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
  nim_diseqc_cmd_t msg = {0};
  u8 tx_buf[8] = {0};
  u16 mode = param >> 16;
  u8 data = (u8)param;


   dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, 0);
   mtos_task_delay_ms(20);
  

  dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_DISEQC1X, 0);

  msg.mode = NIM_DISEQC_BYTES;
  msg.p_tx_buf = tx_buf;
  msg.tx_len = 4;

  msg.p_tx_buf[0] = 0xE0;
  msg.p_tx_buf[1] = 0x31;
  msg.p_tx_buf[2] = cmd;

  switch(cmd)
  {
    case RSCMD_PSTER_STOP:
    case RSCMD_PSTER_LIMIT_OFF:
    case RSCMD_PSTER_LIMIT_E:
    case RSCMD_PSTER_LIMIT_W:
      msg.tx_len = 3;
      break;

    case RSCMD_PSTER_DRV_E:
    case RSCMD_PSTER_DRV_W:
      if(mode == RSMOD_PSTER_DRV_CONTINUE)
      {
        msg.p_tx_buf[3] = 0;
      }
      else if(mode == RSMOD_PSTER_DRV_TIMEOUT)
      {
        /* time out is 1 ~ 127 s */
        msg.p_tx_buf[3] = data & 0x7F;
      }
      else if(mode == RSMOD_PSTER_DRV_STEPS)
      {
        /* step is 1 ~ 128, data_send is 0x80 ~ 0xFF */
        msg.p_tx_buf[3] = (~data) + 1;
      }
      else
      {
        /* default use continuesly mode */
        msg.p_tx_buf[3] = 0;
      }
      break;

    case RSCMD_PSTER_RECALCULATE:
       msg.p_tx_buf[3] = data;
      //msg.p_tx_buf[3] = 0xFF; //to invalid
      break;

    case RSCMD_PSTER_GOTO_NN:
      msg.p_tx_buf[3] = data;
      break;
case RSCMD_PSTER_STORE_NN:
      msg.p_tx_buf[3] = data;
      break;
    case RSCMD_PSTER_GOTO_ANG:
      msg.tx_len = 5;
      if(param == 0)
      {
        msg.p_tx_buf[3] = 0xE0;
        msg.p_tx_buf[4] = 0x00;
      }
      else
      {
        msg.p_tx_buf[3] = (param >> 8) & 0xFF;
        msg.p_tx_buf[4] = (param & 0xFF);
      }

      break;

    default:
      msg.p_tx_buf[3] = data;
  }

  vitual_nim_filter_diseqc_ctrl(&msg,TRUE);

  mtos_task_delay_ms(20);
}

static BOOL vitual_nim_filter_usals_ctrl(double sat_longitude,
        double local_longitude, double local_latitude)
{
  u16 param = 0;
  double degree = 0;

  degree = usals_calc_degree(sat_longitude,
                             local_longitude, local_latitude);
  if(degree > 90)
  {
    // notify driving positioner
    return TRUE;
  }
  else
  {
    param = usals_convert_degree(degree);
    vitual_nim_filter_positioner_ctrl(RSCMD_PSTER_GOTO_ANG,param);
    return FALSE;
  }
}

/*!
  Set  diseqc info
  \param[in] handle for nc service
  \param[in] polar nim_lnb_porlar_t mode
  */
static BOOL vitual_nim_filter_set_diseqc(nc_diseqc_info_t *p_diseqc,
          u8 is_on22k,u8 polarity)
{
  rscmd_switch_mode_t mode = 0;
  u8 port = 0;
  
  if(p_diseqc->is_fixed)
  {
    if(p_diseqc->diseqc_type_1_1 != 0)
    {
      //Record disq 1.1 info
      
      port = p_diseqc->diseqc_port_1_1;
      switch (p_diseqc->diseqc_type_1_1)
      {
        case 1: // no repeat
          mode = RSCMD_SWITCH_NO_REPEAT;
          break;
        case 2: // one repeat
          mode = RSCMD_SWITCH_ONE_REPEAT;
          break;
        default: // two repeat
          mode = RSCMD_SWITCH_TWO_REPEATS;
      }
      if(p_diseqc->diseqc_1_1_mode != 0)
      {
        port &= 0x03;
        port = port << (p_diseqc->diseqc_1_1_mode - 1);
      }

      vitual_nim_filter_switch_ctrl(NIM_DISEQC_LEVEL_X1, mode,
              port,is_on22k,polarity);
    }
   

    if(p_diseqc->diseqc_type_1_0 != 0)
    {
      vitual_nim_filter_switch_ctrl(NIM_DISEQC_LEVEL_X0, 0,
        (u8)p_diseqc->diseqc_port_1_0,is_on22k,polarity);
    }
  }
  else
  {
    if(p_diseqc->used_DiSEqC12) // DiSEqC 1.2
    {
      if(p_diseqc->position_type != 0)
      {
        if(p_diseqc ->position_change)
        {
            vitual_nim_filter_positioner_ctrl(RSCMD_PSTER_GOTO_NN, p_diseqc->position);
          //clean info for USALS
          return TRUE;
        }
      }
    }
    else // USALS
    {
      {
        //_usals_ctrl(handle,p_diseqc->d_sat_longitude,
        //  p_diseqc->d_local_longitude, p_diseqc->d_local_latitude);
        vitual_nim_filter_usals_ctrl(
            p_diseqc->d_sat_longitude,
            p_diseqc->d_local_longitude, 
            p_diseqc->d_local_latitude);
        return TRUE;
      }
    }
  }
  return FALSE;
}
/*!
  set 22k.
  */
static void vitual_nim_filter_set_22k(u8 onoff22k)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
  dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_SET_22K_ONOFF, onoff22k);
}
/*!
  set_polarization
  */
static void vitual_nim_filter_set_polarization(nim_lnb_porlar_t polar)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
 dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_SET_PORLAR, polar);
}
/*!
  set tp
  */
static BOOL vitual_nim_filter_set_tp(BOOL g_connect)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
  nim_channel_info_t channel_info = {0};
  nim_channel_set_info_t set_info = {0};
  
  switch(p_data->lock_info.lock_mode)
  {
    case SYS_DVBS:
      p_data->nc_info.polarization = p_data->lock_info.locks.tp_rcv.polarity;
      p_data->nc_info.channel_info.frequency =
        dvbs_calc_mid_freq(&(p_data->lock_info.locks.tp_rcv),
                &(p_data->lock_info.locks.sat_rcv));

      p_data->nc_info.channel_info.param.dvbs.symbol_rate
        = p_data->lock_info.locks.tp_rcv.sym;

      p_data->nc_info.channel_info.param.dvbs.fec_inner
        = NIM_CR_AUTO;

      p_data->nc_info.onoff22k = (u8)p_data->lock_info.locks.tp_rcv.is_on22k;
      p_data->nc_info.channel_info.param.dvbs.nim_type = p_data->nim_type;
      p_data->nc_info.channel_info.nim_type = p_data->nim_type;
      p_data->nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;
      break;

    case SYS_DVBC:
      p_data->nc_info.channel_info.frequency
        = p_data->lock_info.lockc.tp_freq;

      p_data->nc_info.channel_info.param.dvbc.symbol_rate
        = p_data->lock_info.lockc.tp_sym;

      p_data->nc_info.channel_info.param.dvbc.modulation
        = p_data->lock_info.lockc.nim_modulate;

      p_data->nc_info.polarization = NIM_PORLAR_HORIZONTAL;
      p_data->nc_info.channel_info.spectral_polar = p_data->spectral_polar_c;
      p_data->nc_info.channel_info.nim_type = p_data->nim_type;
      p_data->nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;
      break;

    case SYS_DTMB:
    case SYS_DVBT2:
      p_data->nc_info.channel_info.frequency
              = p_data->lock_info.lockt.tp_freq ;
      p_data->nc_info.channel_info.param.dvbt.band_width 
        = p_data->lock_info.lockt.band_width;
      p_data->nc_info.channel_info.param.dvbt.nim_type 
        = p_data->nim_type;
      p_data->nc_info.channel_info.nim_type = p_data->nim_type;
      p_data->nc_info.channel_info.lock = NIM_CHANNEL_UNLOCK;
    break;
      default:
      break;
    }

    memcpy(&channel_info,&(p_data->nc_info.channel_info),sizeof(nim_channel_info_t));
    if(g_connect == TRUE)
    {
         nim_channel_connect(
          p_data->p_nim_dev,
          &channel_info,
          FALSE);

         p_data->duration_ticks = 200;  //200 ticks = 2s
         p_data->start_ticks = mtos_ticks_get();
         /****first locked****/
         if(channel_info.lock == NIM_CHANNEL_LOCK) 
          {
            if(SYS_DVBC == p_data->lock_info.lock_mode)
              {
                p_data->spectral_polar_c = channel_info.spectral_polar;
                p_data->nim_type = NIM_DVBC;
              }
            if((SYS_DVBT2 == p_data->lock_info.lock_mode)
               || (SYS_DTMB == p_data->lock_info.lock_mode))
              {
                p_data->nim_type = channel_info.param.dvbt.nim_type;
              }
             if(SYS_DVBS == p_data->lock_info.lock_mode)
              {
                p_data->nim_type = p_data->nc_info.channel_info.param.dvbs.nim_type;
              }
            
            return TRUE;
          }

         /****second locked****/
         if(p_data->lock_info.lock_mode == SYS_DVBT2)
          {
              if(p_data->nc_info.channel_info.param.dvbt.nim_type == NIM_DVBT)
                {
                  p_data->nc_info.channel_info.param.dvbt.nim_type = NIM_DVBT2;
                  p_data->nc_info.channel_info.nim_type = 
                                     p_data->nc_info.channel_info.param.dvbt.nim_type;
                }
               else if(p_data->nc_info.channel_info.param.dvbt.nim_type == NIM_DVBT2)
                {
                  p_data->nc_info.channel_info.param.dvbt.nim_type = NIM_DVBT;
                  p_data->nc_info.channel_info.nim_type = 
                                     p_data->nc_info.channel_info.param.dvbt.nim_type;
                }
               else /****NIM DVBT_AUTO******/
                {
                  return FALSE;
                }

          }
          if(SYS_DVBS == p_data->lock_info.lock_mode)
          {
               if(p_data->nc_info.channel_info.param.dvbs.nim_type == NIM_DVBS)
                {
                  p_data->nc_info.channel_info.param.dvbs.nim_type = NIM_DVBS2;
                  p_data->nc_info.channel_info.nim_type = 
                                         p_data->nc_info.channel_info.param.dvbs.nim_type;
                }
               else if(p_data->nc_info.channel_info.param.dvbs.nim_type == NIM_DVBS2)
                {
                  p_data->nc_info.channel_info.param.dvbs.nim_type = NIM_DVBS;
                  p_data->nc_info.channel_info.nim_type = 
                                         p_data->nc_info.channel_info.param.dvbs.nim_type;
                }
               else/****NIM DVBS_AUTO******/
                {
                  return FALSE;
                }

          }
          if(SYS_DVBC== p_data->lock_info.lock_mode)
          {
               return FALSE;
          }
         memcpy(&channel_info,&(p_data->nc_info.channel_info),sizeof(nim_channel_info_t));
         nim_channel_connect(
                                              p_data->p_nim_dev,
                                              &channel_info,
                                              FALSE);

         p_data->duration_ticks = 200;  //200 ticks = 2s
         p_data->start_ticks = mtos_ticks_get();

         if(channel_info.lock == NIM_CHANNEL_LOCK)
          {
            if(SYS_DVBC == p_data->lock_info.lock_mode)
              {
                p_data->spectral_polar_c = channel_info.spectral_polar;
                p_data->nim_type = NIM_DVBC;
              }
            if((SYS_DVBT2 == p_data->lock_info.lock_mode)
               || (SYS_DTMB == p_data->lock_info.lock_mode))
              {
                p_data->nim_type = channel_info.param.dvbt.nim_type;
              }
             if(SYS_DVBS == p_data->lock_info.lock_mode)
              {
                p_data->nim_type = p_data->nc_info.channel_info.param.dvbs.nim_type;
              }
            return TRUE;
          }
               

          return FALSE;
    }
    else
    {
      nim_channel_set(p_data->p_nim_dev, &channel_info, &set_info);
      p_data->duration_ticks = set_info.lock_time / 10;  //200 ticks = 2s
      p_data->start_ticks = mtos_ticks_get();
    }
    return FALSE;
}
/*!
   process the message methdd, it will be called by ota task.

   \param[in] handle ota handle
   \param[in] p_msg new message
  */
static void vitual_nim_filter_single_step(class_handle_t p_handler, os_msg_t *p_msg)
{
  nim_filter_handle_t *p_handle = (nim_filter_handle_t *)p_handler;
  nim_filter_priv_data_t *p_data = &p_handle->data;
  nim_device_t *p_nim_dev =  NULL;
  u8 tuner_id  =0;
  u8 is_lock = 0;
  if(p_msg != NULL)
  {
    switch(p_msg->content)
    {
      case NIM_FILTER_CMD_SET_TUNER:
        tuner_id = (u8)p_msg->para1;
        if(tuner_id == 0)
        {
          p_handle->data.p_nim_dev = (nim_device_t *)dev_find_identifier(
                        NULL,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_NIM);
        }
      else
        {
          p_nim_dev = (nim_device_t *)dev_find_identifier(
                        NULL,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_NIM);
         p_handle->data.p_nim_dev = (nim_device_t *)dev_find_identifier(
                        p_nim_dev,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_NIM);
        }
       p_data->sm = NIM_FILTER_SM_IDLE;
       break;
        
      case NIM_FILTER_CMD_STOP:
        p_data->sm = NIM_FILTER_SM_IDLE;
        break;
        
      case NIM_FILTER_CMD_START_LOCK:
      p_data->lock_info.lock_mode = p_msg->para1;
      switch(p_data->lock_info.lock_mode)
      {
          case SYS_DVBC:
          memcpy(&p_data->lock_info.lockc,
                          (dvbc_lock_info_t *)p_msg->para2,
                          sizeof(dvbc_lock_info_t));
          p_data->spectral_polar_c = NIM_IQ_AUTO;
          p_data->nim_type = NIM_DVBC;
          break;
          case SYS_DVBS:
          memcpy(&p_data->lock_info.locks,
                          (dvbs_lock_info_t *)p_msg->para2,
                          sizeof(dvbs_lock_info_t));
          if(p_handle->data.do_set_dvbs_sat_ctrl  != NULL)
          {
            p_handle->data.do_set_dvbs_sat_ctrl(&p_data->lock_info.locks);
          }
          if(p_data->lock_info.locks.tp_rcv.nim_type == NIM_UNDEF)
          {
            p_data->lock_info.locks.tp_rcv.nim_type = NIM_DVBS;
          }
            p_data->nim_type = p_data->lock_info.locks.tp_rcv.nim_type;
          break;
          case SYS_DTMB:
          case SYS_DVBT2:
          memcpy(&p_data->lock_info.lockt,
                          (dvbt_lock_info_t *)p_msg->para2,
                          sizeof(dvbt_lock_info_t));
            if(p_data->lock_info.lockt.nim_type == NIM_UNDEF)
            {
              p_data->lock_info.lockt.nim_type = NIM_DVBT;
            }
            p_data->nim_type = p_data->lock_info.lockt.nim_type;
          break;
          default:
          break;
        }

         p_data->sm = NIM_FILTER_SM_LOCKING;
        break;

      #if 0
      case NC_EVT_LOCKED:
        OS_PRINTF("\r\n[OTA] locked");
        if(p_data->sm == NIM_FILTER_SM_WAIT_LOCK)
        {
          if(p_data->do_lock_result != NULL)
          {
            p_data->do_lock_result(TRUE);
          }
          p_data->sm = NIM_FILTER_SM_IDLE;
        }
        break;

      case NC_EVT_UNLOCKED:
        if(p_data->sm == NIM_FILTER_SM_WAIT_LOCK)
        {
           if(p_data->do_lock_result != NULL)
          {
            p_data->do_lock_result(FALSE);
          }
            p_data->sm = NIM_FILTER_SM_IDLE;
        }
        break;
        #endif
      default:
        break;
    }
  }
  else
  {
    switch(p_data->sm)
    {
      case NIM_FILTER_SM_IDLE:
        break;
      case NIM_FILTER_SM_LOCKING:
        p_data->g_locked = vitual_nim_filter_set_tp(TRUE);
        p_data->sm = NIM_FILTER_SM_WAIT_LOCK;
        break;
      case NIM_FILTER_SM_WAIT_LOCK:
         if(p_data->do_lock_result != NULL)
        {
          if(p_data->g_locked == TRUE)
          {
            p_data->do_lock_result(TRUE);
            p_data->duration_ticks = 200;  //200 ticks = 2s
            p_data->start_ticks = mtos_ticks_get();
            p_data->sm = NIM_FILTER_SM_LOCK_MONITOR;
          }
          else
          {
            p_data->do_lock_result(FALSE);
            p_data->sm = NIM_FILTER_SM_IDLE;
          }
        }
        
        break;
        case NIM_FILTER_SM_LOCK_MONITOR:
          if((mtos_ticks_get() - p_data->start_ticks) 
            > p_data->duration_ticks)
            {
              dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_CHECK_LOCK,
                  (u32)&is_lock);
              if(is_lock == 0)
              {
                p_data->duration_ticks = 200;  //200 ticks = 2s
                p_data->start_ticks = mtos_ticks_get();
                vitual_nim_filter_set_tp(FALSE);
              }
              else
              {
                p_data->duration_ticks = 200;  //200 ticks = 2s
                p_data->start_ticks = mtos_ticks_get();
              }
              p_data->sm = NIM_FILTER_SM_LOCK_MONITOR;
            }
        break;
      default:
        break;
    }
  }
}

/*!
   Call this method, load ota instance by the policy.

   \param[in] p_policy The policy of application ota
   \return Return app_t instance address
  */
app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id)
{
  nim_filter_handle_t *p_handle = NULL;
  nim_device_t *p_nim_dev =  NULL;
  p_handle = (nim_filter_handle_t *)mtos_malloc(sizeof(nim_filter_handle_t));
  MT_ASSERT(p_handle != NULL);
  memset(&(p_handle->data), 0, sizeof(nim_filter_priv_data_t));
  memset(&(p_handle->instance), 0, sizeof(app_t));
  p_handle->instance.init = init;
  p_handle->instance.task_single_step = vitual_nim_filter_single_step;
  p_handle->instance.get_msgq_timeout = NULL;
  p_handle->instance.p_data = (void *)p_handle;
  p_handle->data.app_id = app_id;

  if(tuner_id == 0)
    {
      p_handle->data.p_nim_dev = (nim_device_t *)dev_find_identifier(
                    NULL,
                    DEV_IDT_TYPE,
                    SYS_DEV_TYPE_NIM);
    }
  else
    {
      p_nim_dev = (nim_device_t *)dev_find_identifier(
                    NULL,
                    DEV_IDT_TYPE,
                    SYS_DEV_TYPE_NIM);
     p_handle->data.p_nim_dev = (nim_device_t *)dev_find_identifier(
                    p_nim_dev,
                    DEV_IDT_TYPE,
                    SYS_DEV_TYPE_NIM);
    }
  
  class_register(NIM_VIRTUAL_CLASS_ID,(void *)p_handle);

  p_handle->data.duration_ticks = 200;  //200 ticks = 2s
  p_handle->data.start_ticks = mtos_ticks_get();

  return &(p_handle->instance);
}


/*!
  register do lock result function
  */
void virtual_nim_filter_register_do_lock_result(void *function)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  p_handle->data.do_lock_result = function;
}

/*!
  register do set dvbs sat ctrl
  */
static void virtual_nim_filter_do_set_dvbs_sat_ctrl(dvbs_lock_info_t *p_lock_info)
{
    nim_filter_handle_t * p_handle = nim_filter_get_handle();
    nim_filter_priv_data_t *p_data = &p_handle->data;
    dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_SET_LNB_ONOFF,1);
    vitual_nim_filter_set_diseqc(&(p_lock_info->disepc_rcv),
      p_lock_info->tp_rcv.is_on22k,p_lock_info->tp_rcv.polarity);
    vitual_nim_filter_set_polarization(p_lock_info->tp_rcv.polarity);
    vitual_nim_filter_set_22k((u8)p_lock_info->tp_rcv.is_on22k);
}

/*!
  register do set dvbs sat ctrl
  */
void virtual_nim_filter_regest_do_set_sat_ctrl(void *function)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  if(function != NULL)
    {
      p_handle->data.do_set_dvbs_sat_ctrl = function;
    }
    else
    {
      p_handle->data.do_set_dvbs_sat_ctrl = 
        virtual_nim_filter_do_set_dvbs_sat_ctrl;
    }
}


/*!
  do lock command
  */
void virtual_nim_filter_lock_start(u8 lock_mode,u32 *p_para)
{
  cmd_t cmd = {0};
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  OS_PRINTF("####debug :%s [lock start]\n",__FUNCTION__);
  cmd.id = NIM_FILTER_CMD_START_LOCK;
  cmd.data1 = (u32)lock_mode;
  cmd.data2 = (u32)p_para;
  ap_frm_do_command(p_handle->data.app_id, &cmd);   
}

/*!
  do lock command
  */
void virtual_nim_filter_set_tuner(u8 tuner_id)
{
  cmd_t cmd = {0};
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  OS_PRINTF("####debug :%s [lock start]\n",__FUNCTION__);
  cmd.id = NIM_FILTER_CMD_SET_TUNER;
  cmd.data1 = (u32)tuner_id;
  ap_frm_do_command(p_handle->data.app_id, &cmd);   
}
/*!
  do stop command
  */
void virtual_nim_filter_lock_stop(void)
{
  cmd_t cmd = {0};
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  cmd.id = NIM_FILTER_CMD_STOP;
  ap_frm_do_command(p_handle->data.app_id, &cmd);   
}

void virtual_nim_filter_start(void)
{
  cmd_t cmd = {0};
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  if(p_handle->data.g_start_bit == TRUE)
  {
    return;
  }
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = p_handle->data.app_id;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  p_handle->data.g_start_bit = TRUE;
}

void virtual_nim_filter_stop(void)
{
  cmd_t cmd = {0};
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  if(NULL == p_handle)
  {
    return;
  }
  if(p_handle->data.g_start_bit == FALSE)
  {
    return;
  }
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = p_handle->data.app_id;
  cmd.data2 = 0;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  p_handle->data.g_start_bit = FALSE;
}

BOOL vitual_nim_filter_get_lock_status(void)
{
  nim_filter_handle_t * p_handle = nim_filter_get_handle();
  nim_filter_priv_data_t *p_data = &p_handle->data;
  u8 is_lock = 0;
  dev_io_ctrl(p_data->p_nim_dev, NIM_IOCTRL_CHECK_LOCK,
                  (u32)&is_lock);
  if(is_lock == 0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


