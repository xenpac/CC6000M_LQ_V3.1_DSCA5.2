/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __NIM_H__
#define __NIM_H__


/*!
    IO control command
  */
typedef enum nim_ioctrl_cmd 
{
    /*!
      Get current channel lock status. This can be used in conjunction 
      with nim_channel_set for channel connection in blind scan.

      */
    NIM_IOCTRL_CHANNEL_CHECK_LOCK = DEV_IOCTRL_TYPE_LOCK + 0,
    /*!
        Change spectral I/Q polarity
      */
    NIM_IOCTRL_CHANGE_IQ = DEV_IOCTRL_TYPE_LOCK + 1, 
    /*!
        NIM sets TS output mode, the parameter format is,
        bit0~7: ts interface selection, like Parallel/Serial/Common, see enum nim_ts_mode, if use default, set NIM_TS_INTF_DEF
        bit8: CLKOUT polarity, 0:Active at falling edge; 1:Active at rising edge, default: falling
        bit9: SYNC polarity, 0:Active Low; 1:Active High, default: High
        bit10: VAL polarity, 0:Active Low; 1:Active High, default: High
        bit11: ERROR polarity, 0:Active Low; 1:Active High, default: High
        bit16~23: if use default setting for polarity of CLKOUT/SYNC/VAL/ERROR, set NIM_TS_CTRL_DEF
      */
    NIM_IOCTRL_SET_TS_MODE = DEV_IOCTRL_TYPE_LOCK + 2,
    /*!
        For DVB-S, command to set LNB polarity         
      */
    NIM_IOCTRL_SET_PORLAR = DEV_IOCTRL_TYPE_LOCK + 3,
    /*!
        DVB-S dish device DiSEqC 1.x command
      */
    NIM_IOCTRL_DISEQC1X = DEV_IOCTRL_TYPE_LOCK + 4,
    /*!
        DVB-S dish device DiSEqC 2.x command
      */
    NIM_IOCTRL_DISEQC2X = DEV_IOCTRL_TYPE_LOCK + 5,
    /*!
        For DVB-S, command to set LNB power on/off        
      */
    NIM_IOCTRL_SET_LNB_ONOFF = DEV_IOCTRL_TYPE_LOCK + 6,
    /*!
       For DVB-S LNB, command to set 22KHz wave on/off
      */  
    NIM_IOCTRL_SET_22K_ONOFF = DEV_IOCTRL_TYPE_LOCK + 7,
    /*!
        For DVB-S, command to set 12V on/off        
      */  
    NIM_IOCTRL_SET_12V_ONOFF = DEV_IOCTRL_TYPE_LOCK + 8,
    /*!
        For ABS, used to switch demodulator between testing and normal modes
      */  
    NIM_IOCTRL_SET_WORK_MODE = DEV_IOCTRL_TYPE_LOCK + 9,
     /*!
      Change tuner mode
      */
    NIM_IOCTRL_CHANGE_TN_MODE = DEV_IOCTRL_TYPE_LOCK + 10,   
    /*!
        Get global lock status
      */
    NIM_IOCTRL_CHECK_LOCK = DEV_IOCTRL_TYPE_LOCK + 11,
    /*!
        Set tuner clock out or not
      */
    NIM_IOCTRL_TUNER_CLOCK_OUT = DEV_IOCTRL_TYPE_LOCK + 12,
    /*!
        Dump the registers in NIM
      */
    NIM_IOCTRL_DUMP_REGS = DEV_IOCTRL_TYPE_LOCK + 13,

   /*!
        Set dm recover from error
      */
    NIM_IOCTRL_RECOVER = DEV_IOCTRL_TYPE_LOCK + 14,
    
    /*!
      get signal info
      */
    NIM_IOCTRL_GET_SIGNAL_INFO = DEV_IOCTRL_TYPE_LOCK + 15, 
    /*!
      get total packages
      */
    NIM_IOCTRL_GET_TOTAL_PACKAGES = DEV_IOCTRL_TYPE_LOCK + 16, 
    /*!
      get total packages
      */
    NIM_IOCTRL_GET_ERR_PACKAGES = DEV_IOCTRL_TYPE_LOCK + 17,    
    /*
        For DVB-S, command to get LNB polarity
      */  
    NIM_IOCTRL_GET_PORLAR = DEV_IOCTRL_TYPE_UNLOCK + 1,
    /*!
        For DVB-S LNB, command to get 22KHz wave on/off status 
      */  
    NIM_IOCTRL_GET_22K_ONOFF = DEV_IOCTRL_TYPE_UNLOCK + 2,    
    /*!
        Get tuner chip's version number
      */  
    NIM_IOCTRL_GET_TN_VERSION = DEV_IOCTRL_TYPE_UNLOCK + 3,
    /*!
        Command to cancel from scan
      */  
    NIM_IOCTRL_SCAN_CANCEL = DEV_IOCTRL_TYPE_UNLOCK + 4,
    /*!
        Command to cancel from scan
      */  
    NIM_IOCTRL_GET_DMD_VERSION = DEV_IOCTRL_TYPE_UNLOCK + 5,
      /*!
      Command to ge tuner sleep from scan
    */  
    NIM_IOCTRL_SET_TUNER_SLEEP = DEV_IOCTRL_TYPE_UNLOCK + 6,
      /*!
      Command to ge tuner wake up from scan
    */  
    NIM_IOCTRL_SET_TUNER_WAKEUP = DEV_IOCTRL_TYPE_UNLOCK + 7,
          /*!
      Command to get tuner type
    */  
    NIM_IOCTRL_GET_TN_TYPE = DEV_IOCTRL_TYPE_UNLOCK + 8,
    /*!
        Get channel parameter setting range, parammeter see nim_channel_param_range_t
      */
    NIM_IOCTRL_GET_CHN_PARAM_RANGE = DEV_IOCTRL_TYPE_UNLOCK + 9,  
    /*!
        Check if the short circuit happened
      */  
    NIM_IOCTRL_CHECK_LNB_SC_PROT = DEV_IOCTRL_TYPE_UNLOCK + 10,
    /*!
        Check if the short circuit happened
      */  
    NIM_IOCTRL_LNB_SC_PROT_RESTORE = DEV_IOCTRL_TYPE_UNLOCK + 11,    
    /*!
      Pull down pin
      */
    NIM_REMOVE_PROTECT = DEV_IOCTRL_TYPE_UNLOCK + 12,    
    /*!
      Pull up pin
      */
    NIM_ENABLE_CHECK_PROTECT = DEV_IOCTRL_TYPE_UNLOCK + 13,    
    /*!
      Check sc
      */
    NIM_SC_CHECK = DEV_IOCTRL_TYPE_UNLOCK + 14,   
    /*!
      get channel info parameten;
      */
    NIM_IOCTRL_GET_CHANNEL_INFO = DEV_IOCTRL_TYPE_UNLOCK + 15, 
    /*!
      set channel info
      */
    NIM_IOCTRL_SET_CHANNEL_INFO = DEV_IOCTRL_TYPE_UNLOCK + 16, 
    /*!
     set tuner bandwidth
      */
    NIM_IOCTRL_SET_TUNER_BANDWIDTH = DEV_IOCTRL_TYPE_UNLOCK + 17,  
    /*!
       0=J83B
       1=J83A
        */
    NIM_IOCTRL_SWITCH_J83B_J83A = DEV_IOCTRL_TYPE_UNLOCK + 18, 

} nim_ioctrl_cmd_t;

/*!
  DiSEqC level. X0 represents both 1.0 and 2.0, and the same for X1, X2 and X3.
  */
typedef enum nim_diseqc_level
{
    /*!
        DiSEqC level 1.0 or 2.0
      */
    NIM_DISEQC_LEVEL_X0 = 0,
    /*!
        DiSEqC level 1.1 or 2.1
      */    
    NIM_DISEQC_LEVEL_X1,
    /*!
        DiSEqC level 1.2 or 2.2
      */    
    NIM_DISEQC_LEVEL_X2,
    /*!
        DiSEqC level 1.3 or 2.3
      */    
    NIM_DISEQC_LEVEL_X3
}nim_diseqc_level_t;

/*!
  DiSEqC mode, used for IO control "NIM_IOCTRL_DISEQC1X" and 
  "NIM_IOCTRL_DISEQC2X" commands
  */
typedef enum nim_diseqc_mode 
{
  /*!
    Burst mode, on for 12.5mS = 0
    */
  NIM_DISEQC_BURST0 = 1,
  /*!
    Burst mode, modulated 1:2 for 12.5mS = 1
    */    
  NIM_DISEQC_BURST1,
  /*!
    Modulated with bytes for DISEQC instructions
    */    
  NIM_DISEQC_BYTES
} nim_diseqc_mode_t;

/*!
  DiSEqC error code used for "struct nim_diseqc_command" " result", 
  only used for DISEQC 2.X
  */
typedef enum nim_diseqc_error 
{
  /*!
    DiSEqC operation success
    */  
  NIM_DISEQC2_SUCCESS = 0,
  /*!
    DiSEqC device no reply
    */     
  NIM_DISEQC2_ERR_NOREPLY,
  /*!
    DiSEqC device parity error
    */     
  NIM_DISEQC2_ERR_PARITY,
  /*!
    DiSEqC device unknown
    */     
  NIM_DISEQC2_ERR_UNKNOWN,
  /*!
    DiSEqC device buffer full
    */     
  NIM_DISEQC2_ERR_BUFFULL
} nim_diseqc_error_t;

/*!
  DiSEqC param, used for IO control "NIM_IOCTRL_DISEQC1X" 
  and "NIM_IOCTRL_DISEQC2X" command
  */
typedef struct nim_diseqc_cmd 
{
  /*!
    diseqc mode, see nim_diseqc_mode_t 
    */
  u8 mode;
  /*!
    DiSEqC 2.x command result, see nim_diseqc_error_t
    */    
  u8 result;
  /*!
    DiSEqC TX command string length in bytes
    */    
  u8 tx_len;
  /*!
    DiSEqC 2.x RX command string length in bytes
    */    
  u8 rx_len;
  /*!
    DiSEqC TX command string
    */    
  u8 *p_tx_buf;
  /*!
    DiSEqC 2.x RX command string
    */    
  u8 *p_rx_buf;
}nim_diseqc_cmd_t;


/*!
    NIM TS output mode
  */
typedef enum nim_ts_mode
{
    /*!
        TS default interface
      */    
    NIM_TS_INTF_DEF = 0, 
    /*!
        TS parall interface for IOCTRL command NIM_IOCTRL_SET_TS_MODE and driver config
      */
    NIM_TS_INTF_PARALLEL,
    /*!
        TS serial interface for IOCTRL command NIM_IOCTRL_SET_TS_MODE and driver config
      */    
    NIM_TS_INTF_SERIAL,
    /*!
        TS common interface for IOCTRL command NIM_IOCTRL_SET_TS_MODE and driver config
      */    
    NIM_TS_INTF_COMMON,
    /*!
        TS default polarity of control signal CLKOUT/SYNC/VAL/ERROR
      */    
    NIM_TS_CTRL_DEF,    
} nim_ts_intf_t;

/*!
  Polarization, used for IO control "NIM_IOCTRL_SET_PORLAR" command
  */
typedef enum nim_lnb_power 
{
    /*!
        LNB power on
      */
    NIM_LNB_POWER_ON = 0x01,   
    /*!
        LNB power off
      */        
    NIM_LNB_POWER_OFF = 0x00,   
    /*!
        LNB short circuit protection happened
      */        
    NIM_LNB_SC_PROTING = 0x02,   
    /*!
        LNB short circuit protection not happened
      */        
    NIM_LNB_SC_NO_PROTING = 0x03,
} nim_lnb_power_t;

/*!
  Polarization, used for IO control "NIM_IOCTRL_SET_PORLAR" command
  */
typedef enum nim_lnb_porlar 
{
    /*!
        LNB porlarity horizontal
      */
    NIM_PORLAR_HORIZONTAL = 0x00,   
    /*!
        LNB porlarity vertical
      */        
    NIM_PORLAR_VERTICAL = 0x01,     
    /*!
        LNB porlarity left
      */        
    NIM_PORLAR_LEFT = 0x02,
    /*!
        LNB porlarity right 
      */        
    NIM_PORLAR_RIGHT = 0x03,
    /*!
        All LNB porlarity
      */        
    NIM_PORLAR_ALL = 0x04,     
} nim_lnb_porlar_t;

/*!
  NIM forward error correction code rate
  */
typedef enum nim_code_rate 
{
    /*!
      NIM none code rate
      */
    NIM_CR_NONE = 0,
    /*!
      NIM code rate, 1/4
      */
    NIM_CR_1_4,    
    /*!
      NIM code rate, 1/3
      */
    NIM_CR_1_3,     
    /*!
      NIM code rate, 2/5
      */
    NIM_CR_2_5,      
    /*!
      NIM code rate 1/2
      */
    NIM_CR_1_2,
    /*!
      NIM code rate, 3/5
      */
    NIM_CR_3_5,    
    /*!
      NIM code rate 2/3
      */        
    NIM_CR_2_3,
    /*!
      NIM code rate 3/4
      */
    NIM_CR_3_4,
    /*!
      NIM code rate 4/5
      */
    NIM_CR_4_5,
    /*!
      NIM code rate 5/6
      */
    NIM_CR_5_6,
    /*!
      NIM code rate 6/7
      */
    NIM_CR_6_7,
    /*!
      NIM code rate 7/8
      */
    NIM_CR_7_8,
    /*!
      NIM code rate 8/9
      */
    NIM_CR_8_9,
    /*!
      NIM code rate, 9/10
      */
    NIM_CR_9_10,    
    /*!
      NIM auto code rate 
      */
    NIM_CR_AUTO
} nim_code_rate_t;

/*!
    NIM types used for "struct nim_config" "nim_type"
  */
typedef enum nim_type 
{
    /*!
      NIM for undefined type
      */    
    NIM_UNDEF = 0x00,
    /*!
      NIM for DVB-S
      */
    NIM_DVBS = 0x01,
    /*!
      NIM for DVB-S2
      */
    NIM_DVBS2 = 0x02,    
    /*!
      NIM for DVB-C
      */    
    NIM_DVBC = 0x04,
    /*!
      NIM for ABS-S
      */    
    NIM_ABSS = 0x05,
    /*!
      NIM for DVB-T
      */
    NIM_DVBT = 0x06,
    /*!
      NIM for DVB-T2
      */
    NIM_DVBT2 = 0x07,
    /*!
      NIM for test mode
      */    
    NIM_TEST = 0x08,

   /*!
      NIM for for DVB-T only
      */    
    NIM_DVBT_ONLY = 0x09,
    /*!
      NIM for for DVB-T2 only
      */    
    NIM_DVBT2_ONLY = 0x0a,

   /*!
      NIM for for DVBT  auto  will sercher dvbt2 first, if not lock then dvbt,
      */    
    NIM_DVBT_AUTO = 0x0b,

    /*!
      NIM for for DVBS  auto  will sercher dvbs2 first, if not lock then dvbs,
      */  
    NIM_DVBS_AUTO = 0x0c,
    NIM_DTMB = 0x0d,
} nim_type_t;


/*!
    I/Q inverted indecator used for "struct nim_config" "iq_polar"
  */
typedef enum nim_spectral_polar 
{
    /*!
      I/Q is normal
      */
    NIM_IQ_NORMAL = 0x00,
    /*!
      I/Q is inverted
      */        
    NIM_IQ_INVERT,
    /*!
      Auto I/Q detection
      */        
    NIM_IQ_AUTO
} nim_spectral_polar_t;

/*!
    Uni-cable paramter
  */
typedef struct
{
  /*!
    the flag to indicate if use uni-cable
    */
  u8 use_uc : 1;
  /*!
    the bank number, from 0 to 7
    */
  u8 bank : 7;
  /*!
    the user band number, from 0 to 11
    */  
  u8 user_band;
  /*!
    the user band frequency in MHz.
    */  
  u16 ub_freq_mhz;
}nim_unicable_param_t;

/*!
    Modulation mode used for "struct nim_channel_info" "modulation"
  */
typedef enum nim_modulation 
{
    /*!
      Auto modulation detection
      */    
    NIM_MODULA_AUTO = 0,
    /*!
      BPSK
      */
    NIM_MODULA_BPSK,
    /*!
      QPSK
      */    
    NIM_MODULA_QPSK,
    /*!
      8PSK
      */    
    NIM_MODULA_8PSK,
    /*!
      QAM 16
      */    
    NIM_MODULA_QAM16,
    /*!
      QAM 32
      */    
    NIM_MODULA_QAM32,
    /*!
      QAM 64
      */    
    NIM_MODULA_QAM64,
    /*!
      QAM 128
      */    
    NIM_MODULA_QAM128,  
    /*!
      QAM 256
      */    
    NIM_MODULA_QAM256,  
} nim_modulation_t;

/*!
    The channel performance structure
  */
typedef struct nim_channel_perf 
{
    /*!
      Channel is locked or not 
      */
    u8  lock;
    /*!
      Signal strength from NIM AGC gain
      */    
    u32 agc;
    /*!
      Signal noise rate in percentage
      */    
    u32 snr;
    /*!
      Bit error rate, normally it is a very little number.
      */    
    double ber;    
} nim_channel_perf_t;

/*!
    dvbs channel parameter used for "struct nim_channel_info" "dvbs"
  */
typedef struct nim_dvbs_param 
{
    /*!
      symbol rate in Symbols per second, in KSs
      */ 
    u32 symbol_rate;
    /*!
      forward error correction, see enum nim_code_rate_t
      */     
    u8  fec_inner;
    /*!
      NIM type, see enum nim_type_t
      */
    u8 nim_type;
    /*!
      uni-cable parameter
      */    
    nim_unicable_param_t uc_param;   
    /*!
      performance
      */
    nim_channel_perf_t   perf;
} nim_dvbs_param_t;

/*!
    dvbc channel parameter used for "struct nim_channel_info" "dvbc"
  */
typedef struct nim_dvbc_param 
{
    /*!
      symbol rate in Symbols per second, in KSs
      */
    u32 symbol_rate;
    /*!
      modulation type, see enum nim_modulation
      */    
    u16  modulation;
} nim_dvbc_param_t;

/*!
    dvbt channel parameter used for "struct nim_channel_info" "dvbt"
  */
typedef struct nim_dvbt_param 
{
    /*!
      nim_type
      */
    u32 nim_type;
    /*!
      band_width
      */    
    u16  band_width;
    /*!
      current set plp_id
      */    
    u8  plp_id;
    /*!
      PLP-number
     */
    u8  DataPlpNumber;
    /*!
      plp_id index of PLP array
     */
    u8  PLP_index;
     /*!
      PLP-ID
     */
    u8  DataPlpIdArray[255];
} nim_dvbt_param_t;

/*!
    dvbc channel lock status
  */
typedef enum nim_channel_lock 
{
    /*!
      channel is locked
      */
    NIM_CHANNEL_UNLOCK = 0,
    /*!
      channel is unlocked
      */    
    NIM_CHANNEL_LOCK = 1
} nim_channel_lock_t;

/*!
    NIM tuning parameter
  */
typedef  union 
{
    /*!
      DVB-S QPSK parameter, see nim_dvbs_param_t
      */       
    nim_dvbs_param_t dvbs;
    /*!
      DVB-C QAM parameter, see nim_dvbc_param_t
      */        
    nim_dvbc_param_t dvbc;
    /*!
      DVB-T/T2 QAM parameter, see nim_dvbt_param_t
      */        
    nim_dvbt_param_t dvbt;
}nim_param_t;

/*!
  nim channel information
  */
typedef struct nim_channel_info
{
    /*!
      channel is locked or not, see nim_channel_lock_t
      */
    u8 lock;
    /*!
      spectral I/Q porlarity, see nim_spectral_polar_t
      */    
    u8 spectral_polar;
    
    /*!
      NIM type, see enum nim_type_t, this is used for some
        dmd or tuner support two  or more Standard
      */
    u8 nim_type;
    
    
    /*!
      Channel frequency in KHz
      */    
    u32 frequency;
    
    
    /*!
        NIM tuning parameter, see nim_param_t
      */    
    nim_param_t param;


} nim_channel_info_t;

/*!
    The channel blind scan information
  */
typedef struct nim_scan_info 
{
    /*!
      the start frequency(in KHz) in blind scan, will return the next start 
      frequency auto-detected by NIM after every scan window.
      */
    u32 start_freq;
    /*!
      the end frequency(in KHz) in blind scan
      */    
    u32 end_freq;               
    /*!
      the channel information in blind scan, this array is malloced by app and used by driver
      */    
    struct nim_channel_info *p_channel_info;              
    /*!
      the max channel count can be stored in p_channel_info, it is set by app
      */    
    u32 max_count;      
    /*!
      the channel number stored in the channel_info, it will be set by driver
      */    
    u32 channel_num;
    /*!
      the uni-cable paramter, see nim_unicable_param_t
      */    
    nim_unicable_param_t uc_param;
}nim_scan_info_t;

/*!
    The channel setting information
  */
typedef struct nim_channel_set_info
{
    /*!
      the flag to indicate if this channel setting is in blind scan
      */
    u8 for_scan;
    /*!
      the delay time for locking this channel, in microsecond
      */    
    u32 lock_time;
} nim_channel_set_info_t;

/*!
    The permitted range of frequency, used by dvbt usually
  */
typedef struct nim_channel_param_range
{
    /*!
      Channel's frequency max setting in KHz 
      */
    u32 freq_max;
    /*!
      Channel's frequency min setting in KHz 
      */
    u32 freq_min;  
} nim_channel_param_range_t;

/*!
    The configuration information in ABS
  */
typedef enum nim_tuner_ver 
{
    /*!
      Tuner M88TS2000
      */
    TS2000 = 0x01,
    /*!
      Tuner M88TS2020
      */    
    TS2020 = 0x02,
    /*!
      Tuner M88TS2022
      */    
    TS2022 = 0x04,
    /*!
      Tuner TDAC3-C02A(dd3k)
      */    
    TN_TDAC3_C02A = 0x05,
    /*!
      Tuner TDA18273
      */    
    TN_NXP_TDA18273 = 0x06,
     /*!
      Tuner MxL5007T
      */    
    TN_MXL_5007T = 0x07,
     /*!
      Tuner MxL603
      */    
    TN_MXL_603 = 0x08,
    /*!
      Tuner TDAC7-D01(dd3k)
      */    
    TN_TDAC7_D01,
    /*!
      Tuner SHARP6306
      */    
    SHARP6306,
    /*!
      Tuner SHARP7306
      */    
    SHARP7306,
    /*!
      Tuner SHARP7803
      */    
    SHARP7803,
    /*!
      Tuner SHARP7903
      */    
    SHARP7903,
    /*!
      Tuner ST6110 chip
      */    
    ST6110,
    /*!
      Tuner ST6110 on board
      */    
    ST6110_ON_BOARD,
    /*!
      Tuner AV2011
      */    
    AV2011 = 0x11,
    /*!
      Tuner AV2026
      */    
    AV2026 = 0x12,
    /*!
      Tuner TC2800
      */
    TC2800 = 0X13,
    /*!
      Tuner TC2000
      */
    TC2000 = 0X14,
    /*!
      Tuner TC_ALPS_TDAC
      */
    TC_ALPS_TDAC = 0X15,
    /*!
      Tuner TC_THOMSON_DCT7070X
      */
    TC_THOMSON_DCT7070X = 0X16,
    /*!
      Tuner TC_XUGUANG_XDCT6A
      */
    TC_XUGUANG_XDCT6A = 0X17,
    /*!
      Tuner TC_LG_TDCC
      */
    TC_LG_TDCC = 0X18,
    /*!
      Tuner TC_LG_TDCC
      */
    TC_TDCCG051F = 0X19,
    /*!
      Tuner TC_MT_NOTSUPPORT
      */
    TC_MT_FE_TN_NOTSUPPORT = 0X20,
     /*!
      Tuner TC_MT_USERDEFINE
      */
    TC_MT_FE_TN_USERDEFINE = 0X21,
     /*!
      TC3800
      */
    TC3800 = 0X22, 
    /*!
      Tuner Unknown
      */
    UNKNOW_TUNER = 0xFF
} nim_tuner_ver_t;

/*!
    NIM on-board pin level configuration
  */
typedef struct nim_pin_config
{
    /*!
      NIM lock indication pin's active level, 0:low active, 1:high active
      */    
    u8 lock_indicate : 1;  
    /*!
      NIM voltage selection pin's active level, 0:select 13v when the pin is low, 1:select 
      13v when the pin is high
      */    
    u8 vsel_when_13v : 1;
    /*!
      VSEL pin's active level when LNB is in standby state, 0: low active, 1:high active
      */    
    u8 vsel_when_lnb_off : 1;
    /*!
      DiSEqC out pin's active level when LNB is in standby state, 0: low active, 1:high active
      */    
    u8 diseqc_out_when_lnb_off : 1;    
    /*!
      LNB enable pin's active level, 0: low active, 1:high active
      */    
    u8 lnb_enable : 1;   
    /*!
      LNB short circuit protection pin's level, 0: protect enable(short circuit happened) on low level, 
      1:protect enable on high level
      */    
    u8 lnb_prot_level : 1;      
    /*!
      LNB enable(power enable or short circuit protection) pin's control by mcu or nim, 1: by mcu, 0:by nim
      */    
    u8 lnb_enable_by_mcu : 1;      
    /*!
      LNB short circuit protection by mcu or nim, 1: by mcu, 0:by nim
      */    
    u8 lnb_prot_by_mcu : 1;      
    /*!
      LNB enable pin's number
      */    
    u8 lnb_enable_pin;     
    /*!
      LNB short circuit protection pin's number
      */    
    u8 lnb_prot_pin;
    /*!
      flag to indicate if lock pin is by mcu's gpio
      */    
    u8 lock_pin_by_mcu : 1;     
    /*!
      lock gpio pin number
      */    
    u8 lock_pin : 7;  
    /*!
      flag to indicate if voltage select pin is by mcu's gpio
      */    
    u8 lnb_vol_pin_by_mcu : 1;     
    /*!
      voltage select gpio pin number
      */    
    u8 lnb_vol_pin : 7;
    /*!
      demod reset gpio pin number
    */
    u8 demod_reset_pin;
    /*!
      lnb voltage selection pin mode, is only valid when lnb_vol_pin_by_mcu is 1
      0:default(demo board); 1:custom mode1; 2:custom mode2; ...
    */
    u8 lnb_vol_pin_mode;    
}nim_pin_config_t;

/*!
  To indicate the NIM demod version
  */
typedef enum 
{
  /*!
    demod 1st version 
    */
  DEM_VER_0 = 0,
  /*!
    demod 2nd version 
    */
  DEM_VER_1,  
  /*!
    demod 3rd version 
    */
  DEM_VER_2,  
    /*!
    demod 4rd version 
    */
  DEM_VER_3,  
}nim_dem_ver_t;

/*!
  The NIM device configuration structure, used by nim_open()
  */
typedef struct nim_config 
{
  /*!
    demod version, see nim_dem_ver_t
    */
    u8 dem_ver;
  /*!
    demod version, see nim_dem_ver_t
    */
    u8 tn_version;
    /*!
      2-wire bus device handle
      */
    void *p_dem_bus;
    /*!
      NIM type, see enum nim_type_t.
      */    
    u8 nim_type;
    /*!
      Demodulator's 2-wire bus address
      */    
    u8 dem_addr;
    /*!
      Demodulator's crystal frequency in KHz
      */    
    u16 dem_crystal_khz;
    /*!
      Demodulator's working clock frequency in KHz
      */    
    u32 dem_clock_khz;
    /*!
      TS output mode, serial or parallel, see enum nim_ts_mode.
      */    
    u32 ts_mode;
    /*!
      2-wire bus device, if tuner i2c is repeated by demod, set NULL here
      */
    void *p_tun_bus;
     /*!
      Tuner's 2-wire bus address
      */    
    u8 tun_addr;
    /*!
      Tuner's crystal frequency in KHz
      */     
    u16 tun_crystal_khz;
    /*!
      Check if the tuner's spectral I/Q pin is connected invertedly.
      */   
    u8 tun_iq_invertion;
    /*!
      The tuner supported by this NIM driver, see enum nim_tuner_ver_t
      */    
    u8 tun_support;
    /*!
      If you need loop RF through function, please set it to 1 , else set it to 0
      */    
    u8 tun_rf_bypass_en : 1; 
    /*!
      If you need clock out from tuner clkout pin to other chip use, set it to 1
      */    
    u8 tun_clk_out_by_tn : 1;
    /*!
      If you need clock out from tuner xtalout pin to demodulator use, please set it to 1 , else set it 0
      */    
    u8 tun_clk_out_by_xtal : 1;    
    /*!
      The tuner clock out division, it is valid if tun_clk_out_by_tn is 1
      */    
    u8 tun_clk_out_div : 5;    
    /*!
      The times of blind scan
      */
    u8 bs_times;
    /*!
      NIM's working mode, lab testing or normal
      */
    u8 work_mode;

    /*!
     NIM's demode system frequency
     */
    u32 x_crystal;
    /*!
      The on-board pin's level configuration
      */    
    nim_pin_config_t pin_config;
    /*!
      The bandwidth of demod
      */   
    u8 demod_band_width;
    /*!
      if use the public sharing driver service, set the handle here 
      */
    void *p_drvsvc;     
    /*!
      if not use the public sharing driver service, set this, the notify task priority
      */
    u32 task_prio;
    /*!
      if not use the public sharing driver service, set this, the notify task size
      */  
    u32 stack_size;
    /*!
      Tuner loopthrough
      */  
    u8  tuner_loopthrough;
    /*!
      tuner_bandwidth
      */  
    u8  tuner_bandwidth;
    /*!
      tuner mode if needed, 0: DVB-C, 2: MMDS
      */  
    u8  tuner_mode;
    /*!
      tuner driver version
      */  
    u8  tuner_ver;
    /*!
        The locking mode of function call, see dev_lock_mode
      */    
    u8 lock_mode;
   /*!
        number of set limit  lock ferquency
      */
    u32 freq_offset_limit;
} nim_config_t;

/*!
    NIM device structure
  */
typedef struct nim_device
{
    /*!
        Pointer to driver base
      */
    void *p_base;
    /*!
        Pointer to private data        
      */
    void *p_priv;    
} nim_device_t;

/*!
  NIM notify message
  */
typedef enum _nim_msg {
  /*!
      blind scan find tp
    */
  NIM_BSTpFind,
  /*!
      blind scan tp locked
    */
  NIM_BSTpLocked,
  /*!
      blind scan tp unlocked
    */
  NIM_BSTpUnlock,
  /*!
    blind scan start
  */
  NIM_BSStart,
  /*!
    blind scan finish
  */
  NIM_BSFinish,
  /*!
    blind scan one window start
  */
  NIM_BSOneWinFinish,
  /*!
    blind scan abort
  */
  NIM_BSAbort
} nim_msg_t;

/*!
  pin voltage level
  */
typedef enum 
{
  /*!
    pin voltage level low
    */
  NIM_PIN_LEVEL_LOW = 0,
  /*!
    pin voltage level high
    */
  NIM_PIN_LEVEL_HIGH = 1
}nim_pin_level_t;

/*!
  NIM notify function prototype
*/
typedef RET_CODE (*nim_notify)(nim_msg_t msg, void *p_data);

/*!
  Scan channels from the start frequency to the end frequency and return 
  the next start frequency.

  \param[in] p_dev  The NIM device handle
  \param[in] p_scan_info  The blind scan information, return the next start 
    frequency on "start_freq"

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_channel_scan(nim_device_t *p_dev, 
nim_scan_info_t *p_scan_info);

/*!
  This is a synchronous interface for channel connection. 
  You can know if this frequency is locked or not immediately.
  It can be used in blind scan or manual channel connection mode.

  \param[in] p_dev  The NIM device handle
  \param[in] p_channel_info  The channel information in blind scan. It will return 
    the actual channel information.
  \param[in] for_scan  is blind scan mode or manual connect mode

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_channel_connect(nim_device_t *p_dev, 
              nim_channel_info_t *p_channel_info, BOOL for_scan);

/*!
  This is an asynchronous interface for channel connection. 
  It will return at once without checking lock status. 
  If this frequency can be locked, you can get the state before p_channel_set_info->lock_time.
  It can be used in blind scan or manual channel connection mode by setting p_channel_set_info->for_scan.

  \param[in] p_dev  The NIM device handle
  \param[in] p_channel_info  The channel information
  \param[in] p_channel_set_info  The channel setting information  

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_channel_set(nim_device_t *p_dev, 
               nim_channel_info_t *p_channel_info, 
               nim_channel_set_info_t *p_channel_set_info);

/*!
  Get current channel's performance information such as signal strength, 
  snr and ber.

  \param[in] p_dev  The NIM device handle
  \param[in] p_channel_perf  The channel performance information

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_channel_perf(nim_device_t *p_dev, 
               nim_channel_perf_t *p_channel_perf);

/*!
  Send and receive DiSEqC command.

  \param[in] p_dev  The NIM device handle.
  \param[in] p_diseqc_cmd  DiSEqC command.

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_diseqc_ctrl(nim_device_t *p_dev, 
               nim_diseqc_cmd_t *p_diseqc_cmd);

/*!
  Start a blind scan process
  This is an asynchronous interface, and 
  you shouldn't start a new blind scan process before the last one done.
  Until the blind scan process finished, you shouldn't call nim_channel_set or nim_channel_connect.
  
  \param[in] p_dev  The NIM device handle.
  \param[in] p_scan_info  The start and stop frequency is passed by p_scan_info. And
             the memory pointed by p_scan_info shouldn't be freed before process done.
  
  \return Return 0 for success and others for failure.
  */
RET_CODE nim_blind_scan_start(nim_device_t *p_dev, nim_scan_info_t *p_scan_info);

/*!
  Cancel the blind scan process started by nim_blind_scan_start.
  This is an asynchronous interface, and
  when the process is cancelled, you will be notified by the finished message.
  You have no need to call this function if you got a finished message.
  
  \param[in] p_dev  The NIM device handle.

  \return Return 0 for success and others for failure.
  */
RET_CODE nim_blind_scan_cancel(nim_device_t *p_dev);

/*!
  Register nim notify funciton
  If the blind scan process got a locked frequency, you will get a locked message, at the same time,
  demod and tuner will be in this frequency locked state.
  If the blind scan process got an unlocked frequency, you will get an unlocked message.
  If the blind scan process was finished, you will get a finished message.
  
  \param[in] p_dev  The NIM device handle.
  \param[in] func  The NIM notify function.
  
  \return Return 0 for success and others for failure.
  */
RET_CODE nim_notify_register(nim_device_t *p_dev, nim_notify func);

#endif //__NIM_H__

