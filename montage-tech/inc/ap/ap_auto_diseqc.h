/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_AUTODISEQC_H_
#define __AP_AUTODISEQC_H_


/*!
  Delcare all command for auto diseqc
  */
typedef enum
{
  /*!
    Start detect diseqc
  */
  DISEQC_CMD_START_DETECT = 0, 
  /*!
    Pause detect diseqc
  */
  DISEQC_CMD_PAUSE_DETECT,
  /*!
    Resume detect diseqc
    */
  DISEQC_CMD_RESUME_DETECT, 
  /*!
    Cancel detect diseqc
    */
  DISEQC_CMD_CANCEL_DETECT = SYNC_CMD
}auto_diseqc_cmd_t;

/*!
  Declare the event for auto diseqc
*/
typedef enum
{
  DISEQC_EVT_FINISHED = ((APP_AUTODISEQC << 16) | ASYNC_EVT),
  DISEQC_EVT_SYNC_END
} auto_diseqc_evt_t;

/*!
  the diseqc info for the scanning
*/
typedef struct
{
  /*!
    the frequency
  */
  u32 diseqc_frequency;
  /*!
    the symbol
  */
  u32 diseqc_symbol;
  /*!
    the diseqc port num
  */
  u16 diseqc_port_num;
  /*!
    the diseqc type :0 for diseqc 1.0; 1 for diseqc 1.1
  */
  u16 diseqc_type;
}diseqc_freq_info_t;


/*!
  Implementation policy definition 
*/
typedef struct
{
  /*!
    Implementation policy for diseqc nit found.
  */
  void (*on_nit_found)(u16 para1, u16 para2);
  /*!
    Implementation policy for diseqc type 
  */
  void (*set_diseqc_port)(u8 para);
  /*!
    Private data for time policy
  */
  void *p_data;  
}auto_diseqc_policy_t;

/*!
  Singleton patern interface which can get a auto_diseqc_detect application's
  instance
  
  \return instance address
*/
app_t *construct_ap_auto_diseqc(auto_diseqc_policy_t *p_policy);

#endif
