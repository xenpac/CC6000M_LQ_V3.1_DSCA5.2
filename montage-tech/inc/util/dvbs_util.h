/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVBS_UTIL_H_
#define __DVBS_UTIL_H_

/*!
  C band min MHZ(4.65GHZ)
  */
#define DVBS_C_BAND_MIN_MHZ 4650

/*!
  C band max MHZ(6.35GHZ)
  */
#define DVBS_C_BAND_MAX_MHZ 6350

/*!
  Ku band min MHZ(8.6GHZ)
  */
#define DVBS_KU_BAND_MIN_MHZ 8600

/*!
  Ku band max MHZ(8.6GHZ)
  */
#define DVBS_KU_BAND_MAX_MHZ 11800

/*!
  Band type
  */
typedef enum
{
  /*!
    C band
    */
  C_BAND = 0,
  /*!
    Ku band
    */
  KU_BAND = 1
} band_type_t;

/*!
  LNB type referenced by satellite
  */
typedef enum
{
  /*!
    Standard LNB type
    */
  DVBS_LNB_STANDARD = 0,
  /*!
    LNB user type
    */
  DVBS_LNB_USER,
  /*!
    LNB universal type
    */
  DVBS_LNB_UNIVERSAL
} lnb_type_t;

/*!
  LNB power type
  */
typedef enum
{
  /*!
    Power all and both 13 and 18V are supported
    */
  DVBS_LNB_POWER_ALL = 0,
  /*!
    13V, vertical 
    */
  DVBS_LNB_POWER_13V,
  /*! 
    18V, horizantal
    */
  DVBS_LNB_POWER_18V
} lnb_power_t;

/*!
  Local oscillator type
  */
typedef enum
{  
  /*!
    Single local oscillator
    */
  SINGLE_LOCAL_OSCILLATOR = 0,
  /*!
    Single local oscillator
    */
  DOUBLE_LOCAL_OSCILLATOR
} local_oscillator_t;

/*!
  Satellite receiving parameter
  */
typedef struct
{
  /*!
    lnb high frequency
    */
  u32 lnb_high            : 16;
  /*!
    lnb low frequency
    */
  u32 lnb_low             : 16;
  /*! 
    LNB power option, 0: 13/18v 1: 13v 2: 18v 
    */
  u32 lnb_power           : 2;
  /*! 
    LNB types, 0: Standard, 1: user, 2: universal 
    */
  u32 lnb_type            : 2;
  /*!
    0/12v option on/off, 0: off, 1: on
    */
  u32 v12                 : 1;
  /*!
    22k on off state
    */
  u32 k22                 : 1;
 /*!
    user band for unicable
   */
  u32 user_band         :4;
 /*!
    the center freq for unicable
  */
  u32 band_freq           :12;
 /*! 
    unicable type. 0: unicable A; 1: unicable B 
  */
  u32 unicable_type      : 1;
 /*! 
    tuner index 0: tuner 0; 1: tuner 1 
  */
 u32 tuner_index:1;
 /*! 
    tuner type 0: normal; 1: support low frequence scan
  */
 u32 tuner_type:1;
  /*!
    Reserved
    */
  u32 reserve             : 7;

}sat_rcv_para_t;

/*!
  TP receiving parameter
  */
typedef struct
{
  /*! 
    Symbol rate of TP 
    */
  u32 sym;
  /*! 
    Frequency of TP 
    */
  u32 freq;
  /*! 
    Polarity types, 0: horizontal, 1: vertical 
    */
  u32 polarity    : 2;
  /*! 
    Flag on 22k or not at cur tp, 0: 22k off, 1: 22k on 
    */
  u32 is_on22k    : 1;
  /*!
    Reserved
    */
  u32 nim_modulate      : 4;
  /*!
    NIM type, see enum nim_type_t
    */
  u32 nim_type :4;
  /*!
    fec_inner
    */
  u32 fec_inner      : 4;
  /*! 
    PLP id for dvb t2 
    */
  u32 plp_id   : 8;
  /*!
    Reserved
    */
  u32 reserve      : 9;
}tp_rcv_para_t;

/*!
  DVBS lock info
  */
typedef struct
{
  /*!
    Nim ctrl disepc data
    */
  nc_diseqc_info_t disepc_rcv;
  /*!
    Satellite data
    */
  sat_rcv_para_t sat_rcv;
  /*!
    TP data
    */
  tp_rcv_para_t tp_rcv;
} dvbs_lock_info_t;

/*!
  DVBS program info
  */
typedef struct
{
  /*!
    Service ID(DVB concept, program number)
    */
  u16 s_id;
  /*!
    TV mode
    */
  u16 tv_mode;
  /*!
    Video pid
    */
  u16 v_pid;
  /*!
    Audio pid
    */
  u16 a_pid;
  /*!
    PCR pid
    */
  u16 pcr_pid;
  /*!
    PMT pid
    */
  u16 pmt_pid;
  /*!
    Audio track
    */
  u16 audio_track;
  /*!
    Audio volumn;
    */
  u16 audio_volume;
  /*!
    Audio type
    */
  u16 audio_type;
  /*!
    scrambled?
    */
  BOOL is_scrambled;
  /*!
    Video type
    */
  u8 video_type;
  /*!
    Request ecm
    */
  u8 request_ecm;
  /*!
    reserved
    */
  u16 reserved;
  /*!
    Aspect ratio
    */
  u32 aspect_ratio;
  /*!
    context field 1
    */
  u32 context1;
  /*!
    context field 2
    */
  u32 context2;  
  /*!
     crypto length
    */
  u16 crypto_key_length;
  /*!
     crypto key
    */
  u8 crypto_key[16];
}dvbs_program_t;

/*!
  Detect lnb frequency is in C band or ku band
  \param[in] freq LNB frequency
  \param[out] C_BAND means C band, KU_BAND means KU band
  */
band_type_t dvbs_detect_lnb_freq(u32 freq);
/*!
  Calc down frequency
  \param[in] p_tp tp information
  \param[in] p_sat satellite information
  \param[out] down frequency
  */
u16 dvbs_calc_down_freq(tp_rcv_para_t *p_tp, sat_rcv_para_t *p_sat);
/*!
  Calc middle frequency
  \param[in] p_tp tp information 
  \param[in] p_sat satellite information
  \param[in] middle frequency in KHZ
  */
u32 dvbs_calc_mid_freq(tp_rcv_para_t *p_tp, sat_rcv_para_t *p_sat);
/*!
  Calc search information
  \param[in] p_search_info channel search information
  \param[in] p_sat satellite information
  \param[in] p_tp tp information
  */
void dvbs_calc_search_info(nc_channel_info_t *p_search_info,
sat_rcv_para_t *p_sat, tp_rcv_para_t *p_tp);
/*!
  Calc down frequency range
  \param[in] p_search_info channel search information
  \param[in] p_sat satellite information
  \param[in] p_min the minux down frequency
  \param[in] p_max the max down frequency
  */
void dvbs_get_down_freq_range(tp_rcv_para_t *p_tp,
  sat_rcv_para_t *p_sat, u16 *p_min, u16 *p_max);

#endif // End for __DVBS_UTIL_H_
