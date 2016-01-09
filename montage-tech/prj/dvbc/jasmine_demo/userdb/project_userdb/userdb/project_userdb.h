/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "mtos_msg.h"
#include "service.h"
#include "mdl.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "db_dvbs.h"

#define NORF_BASE_ADDR             0x08000000
#define FLASH_SIZE	(4 * 1024 * 1024)
#define SAT_PROPERTY_NUM (motor_type + 1)//12
#define TP_PROPERTY_NUM (emm_num + 1)//20
#define PG_PROPERTY_NUM (language_index + 1)//20
#define STR_SAT_COMPARE_NUM 8
#define STR_TP_COMPARE_NUM 3
#define STR_PG_COMPARE_NUM 5
#define LNB_FREQ_CNT   (17) 
#define FREQ_VAL_CNT   (13)

#define SAT_COMPARE_NUM 12
#define TP_COMPARE_NUM 3
#define PG_COMPARE_NUM 12
//validation

#define SAT_NAME_MAX 8

#define SAT_LONGITUDE_MIN 0
#define SAT_LONGITUDE_MAX 18000

#define SAT_BAND_FREQ_MIN 950
#define SAT_BAND_FREQ_MAX 2150

#define TP_SYM_MIN 1000
#define TP_SYM_MAX 45000

typedef enum
{
  SAT_NAME = 0,
  sat_select,
  longitude,
  longitude_type,
  LNB_freq,
  user_band,
  band_freq,
  diseqc_port,
  diseqc11_port,
  K22,
  Polarity, 
  position,
  motor_type, 
} Property_sat;

typedef enum 
{
  freq = 0,//tp
  sym,
  polarity,
  tp_select,
  nit_pid,
  pat_version,
  tp_is_scramble,
  is_on22k,
  nim_type,
  emm_num
} Property_tp;

typedef enum
{ 
  pg_name = 0,//pg
  fav_grp_flag,
  tv_flag,
  lck_flag,
  skp_flag,
  volume,
  audio_channel,
  pg_is_scrambled,
  audio_ch_num,
  video_pid,
  pcr_pid,
  audio_track,
  service_type,
  video_type,
  s_id,
  orig_net_id,
  ts_id,
  default_order,
  ca_system_id,
  pmt_pid ,
  ecm_num,
  cas_ecm,
  p_id,
  type,
  language_index
} Property_pg;

//Str_compare
const wchar_t *Str_Sat_compare[STR_SAT_COMPARE_NUM] =
{L"Name",L"Longitude_type",L"LNB_freq",L"Diseqc1.0_port",L"Diseqc1.1_port",L"22k",L"Polarity",L"Motor_type"};

const wchar_t *Str_TP_compare[STR_TP_COMPARE_NUM] =
{L"Polarity",L"Scramble",L"22k"};

const wchar_t *Str_PG_compare[STR_PG_COMPARE_NUM] =
{L"Audio_channel",L"Scrambled",L"Audio_track",L"Audio_type",L"Name"};

//key
const wchar_t *Sat_Property_key[SAT_PROPERTY_NUM] =
{L"Name",L"Select",L"Longitude",L"Longitude_type",L"LNB_freq",L"User_band",L"Band_freq",L"Diseqc1.0_port",
 L"Diseqc1.1_port",L"22K",L"Polarity",L"Positioner",L"Motor_type"};
const wchar_t *Tp_Property_key[TP_PROPERTY_NUM] =
{L"Freq",L"Sym",L"Polarity",L"Select",L"Nit_pid",L"Pat_version",L"Scramble",
 L"22k",L"Nim_type",L"Emm_num"};
const wchar_t *Pg_Property_key[PG_PROPERTY_NUM] =
{L"Name",L"Fav_grp_flag",L"Tv_flag",L"Lck_flag",L"Skp_flag",L"Volume",
 L"Audio_channel",L"Scrambled",L"Audio_ch_num",L"Video_pid",L"Pcr_pid",L"Audio_track",
 L"Service_type",L"Video_type",L"S_id",L"Orig_net_id",L"Ts_id",L"Default_order",
 L"Ca_system_id",L"Pmt_pid",L"Ecm_num",L"Cas_ecm",L"Audio_p_id",L"Audio_type",
 L"Audio_lang_index"
 };

//value
const wchar_t *Sat_Property_value[SAT_PROPERTY_NUM] =
{L"123",L"0",L"0.00",L"E",L"5150",L"1",L"0",
 L"Disable",L"Disable",L"off",L"Auto",L"0",L"None"};
const wchar_t *Tp_Property_value[TP_PROPERTY_NUM] =
{L"0",L"0",L"H",L"0",L"0",L"0",L"not scrambled",
 L"off",L"0",L"0"};
const wchar_t *Pg_Property_value[PG_PROPERTY_NUM] =
{L"CCTV-SZ",L"1",L"0",L"1",L"0",L"10",
 L"channel 2",L"scrambled",L"1",L"0",L"1",L"left",
 L"1",L"0",L"1",L"1",L"1",L"1",
 L"1",L"0",L"1",L"1",L"1",L"AC3",
 L"1"
 };

//group
const wchar_t *Sat_Property_group[SAT_PROPERTY_NUM] =
{L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",
 L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",
 L"Adaptable"};
const wchar_t *Tp_Property_group[TP_PROPERTY_NUM] =
{L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Unadaptable",L"Unadaptable",
 L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable"};
const wchar_t *Pg_Property_group[PG_PROPERTY_NUM] =
{L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",
 L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",L"Adaptable",
 L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",
 L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",L"Unadaptable",
 L"Unadaptable"};

//editable
const int *Sat_Property_editable[SAT_PROPERTY_NUM] =
{1,1,1,1,1,1,
 1,1,1,1,1,1,
 1};
const int *Tp_Property_editable[TP_PROPERTY_NUM] =
{1,1,1,1,0,0,
 0,0,0,0};
const int *Pg_Property_editable[PG_PROPERTY_NUM] =
{1,1,1,1,1,1,
 1,1,1,1,1,1,
 0,0,0,0,0,0,
 0,0,0,0,0,0,
 0};

//options
const wchar_t *Sat_Property_options[SAT_PROPERTY_NUM] =
{L"",
 L"0;1",
 L"",
 L"W;E",
 L"5150;5750;5950;9750;10000;10050;10450;10600;10700;10750;11250;11300;5150-5750;5750-5150;Universal(9750-10600);Unicable A;Unicable B",
 L"1;2;3;4",
 L"",
 L"Disable;Port1;Port2;Port3;Port4",
 L"Disable;Port1;Port2;Port3;Port4;Port5;Port6;Port7;Port8;Port9;Port10;Port11;Port12;Port13;Port14;Port15;Port16",
 L"off;on",
 L"Auto;V;H",
 L"",
 L"None;DiSEqC1.2;USALS"};
const wchar_t *Tp_Property_options[TP_PROPERTY_NUM] =
{L"", L"",L"H;V",L"0;1",L"",L"",
 L"not scrambled;scrambled",L"off;on",L"",L""};
const wchar_t *Pg_Property_options[PG_PROPERTY_NUM] =
{L"",
 L"Movies;News;Music;Sports;Child;Educate;Social;Culture",
 L"0;1",L"0;1",L"0;1",L"",
 L"channel 1;channel 2", L"not scrambled;scrambled",L"",L"",L"",L"stereo;left;right",
 L"",L"",L"",L"",L"",L"",
 L"",L"",L"",L"",L"",L"Audio type;AC3",
 L""};

//satellite parameter
const wchar_t *Sat_longitude_type[2] =
{L"W",L"E"};

const wchar_t *Sat_LNB_freq[LNB_FREQ_CNT] =
{L"5150",L"5750",L"5950",L"9750",L"10000",L"10050",
 L"10450",L"10600",L"10700",L"10750",L"11250",L"11300",
 L"5150-5750",L"5750-5150",L"Universal(9750-10600)",L"Unicable A",L"Unicable B"
};

const wchar_t *Sat_diseqc_port[5] =
{L"Disable",L"Port1",L"Port2",L"Port3",L"Port4"};

const wchar_t *Sat_diseqc11_port[17] =
{L"Disable",L"Port1",L"Port2",L"Port3",L"Port4",L"Port5",
 L"Port6",L"Port7",L"Port8",L"Port9",L"Port10",L"Port11",
 L"Port12",L"Port13",L"Port14",L"Port15",L"Port16"};

const wchar_t *Sat_K22[2] =
{L"off",L"on"};

const wchar_t *Sat_Polarity[3] =
{L"Auto",L"V",L"H"};

const wchar_t *Sat_motor_type[3] =
{L"None",L"DiSEqC1.2",L"USALS"};

//TP parameter
const wchar_t *TP_polarity[2] =
{L"H",L"V"};
const wchar_t *TP_is_scramble[2] =
{L"not scrambled",L"scrambled"};
const wchar_t *TP_is_on22k[2] =
{L"off",L"on"};

//PG parameter
const wchar_t *PG_Fav_grp_flag[8] =
{ L"Movies",L"News",L"Music",L"Sports",L"Child",L"Educate",L"Social",L"Culture"};
const wchar_t *PG_audio_channel[3] =
{L"channel 1",L"channel 2",L"Other"};
const wchar_t *PG_is_scrambled[2] =
{L"not scrambled",L"scrambled"};
const wchar_t *PG_audio_track[3] =
{L"stereo",L"left",L"right"};
const wchar_t *PG_audio_type[3] =
{L"Audio type",L"AC3",L"Other"};

 