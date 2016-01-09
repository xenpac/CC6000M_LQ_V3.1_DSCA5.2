/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVBT_UTIL_H_
#define __DVBT_UTIL_H_

/*!
  DVBT or DVBT2 lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;    
  /*!
    bandwidth
    */
  u32 band_width;
  /*!
    see enum nim_type in nim.h
    */
  u32 nim_type :8;
  /*!
    plp id
    */
  u32 plp_id :8;
  /*!
    plp id
    */
  u32 plp_index :8;
  /*!
    reserved
    */
  u32 reserve  :8;
} dvbt_lock_info_t;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
#if AUSTRALIA_FREQTBL_NEW
/*!
  Australia frequency Table size
  */
#define FREQ_TAL_SIZE   7
/*!
  Australia frequency Table
  */
typedef struct  _FreqTblType
{
  u8 u8CHNo;   //channel no.
  u32 u32Freq;   //frequency
  s8 s8CHNoOffset;  //No. offset ----->for displaying 0:= No.  +X:No.-X and add "A"   -X:No.-X
}FreqTblType;
#endif

/*!
  Country table
  */
typedef enum _EN_COUNTRY_TYPE
{
  //COUNTRY_TYPE_EUROPE,
  COUNTRY_TYPE_ENGLAND,
  COUNTRY_TYPE_GERMANY,
  COUNTRY_TYPE_TAIWAN,
  COUNTRY_TYPE_ITALY,
  COUNTRY_TYPE_FRANCE,
  COUNTRY_TYPE_CHINA,
  COUNTRY_TYPE_AUSTRALIA,
  COUNTRY_TYPE_BRAZIL,
  COUNTRY_TYPE_CHILE,
  COUNTRY_TYPE_HONGKONG,
  COUNTRY_TYPE_ARGENTINA,
  COUNTRY_TYPE_PERU,
  COUNTRY_TYPE_UK,
  COUNTRY_TYPE_RUSSIAN,
  COUNTRY_TYPE_INDIA,
  COUNTRY_TYPE_COLOMBIA,
  COUNTRY_TYPE_THAILAND,  
  
  COUNTRY_TYPE_VIETNAM,
  COUNTRY_TYPE_NIGERIA,
  COUNTRY_TYPE_SINGAPORE,
  COUNTRY_TYPE_GHANA,
  COUNTRY_TYPE_AFRICA,
  
  COUNTRY_TYPE_DJIBOUTI,
  COUNTRY_TYPE_MONGOLIA,  
  COUNTRY_TYPE_POLAND,  
  
  COUNTRY_TYPE_GREEK,             //
  COUNTRY_TYPE_SLOVENIAN,
  COUNTRY_TYPE_SPAIN,
  COUNTRY_TYPE_DENMARK,
  COUNTRY_TYPE_FINLAND,
  COUNTRY_TYPE_SWEDEN,
  
  COUNTRY_TYPE_OTHER,
  COUNTRY_TYPE_NUM
} dvbt_country_t;

//--------------------------------------------------------------------------------------------------
/// Get RF Channel parameters
/// @param  u8RF_CH                 \b IN:  RF channel number
/// @param  u8Country               \b IN:  country
/// @param  u16BandWidth            \b IN:  channel bandwidth (kHz)
/// @param  u32Frequency            \b IN:  the frequency of the RF channel
/// @return TRUE: get the corresponding frequency
///         FALSE: failed to get the frequency.
//--------------------------------------------------------------------------------------------------
BOOL dvbt_get_tp_setting(u8 u8RF_CH, u8 u8Country, u16 *u16BandWidth, u32 *u32Frequency);

//--------------------------------------------------------------------------------------------------
/// Get RF channel number by frequency
/// @param  u8Country               \b IN:  country
/// @param  u32Frequency            \b IN:  the frequency of the RF channel
/// @return RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_freq2channelnumber(u8 u8Country, u32 u32Frequency);

//--------------------------------------------------------------------------------------------------
/// Get highest RF channel number by country
/// @param  u8Country               \b IN:  country
/// @return the highest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_max_rf_ch_num(u8 u8Country);

//--------------------------------------------------------------------------------------------------
/// Get lowest RF channel number by country
/// @param  u8Country               \b IN:  country
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_min_rf_ch_num(u8 u8Country);

//--------------------------------------------------------------------------------------------------
/// Get Next RF channel number by country by current Rf Ch
/// @param  u8Country               \b IN:  country
/// @param  u8RF_CH               \b IN:  current RF channel
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_next_rf_ch_num(u8 u8Country, u8 u8RF_CH);

//--------------------------------------------------------------------------------------------------
/// Get prev RF channel number by country by current Rf Ch
/// @param  u8Country               \b IN:  country
/// @param  u8RF_CH               \b IN:  current RF channel
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_prev_rf_ch_num(u8 u8Country, u8 u8RF_CH);



#endif // End for __DVBS_UTIL_H_

