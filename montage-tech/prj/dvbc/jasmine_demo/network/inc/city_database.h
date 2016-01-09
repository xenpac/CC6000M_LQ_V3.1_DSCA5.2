/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CITY_DATABASE_H__
#define __CITY_DATABASE_H__

#define NAME_LEN 24

typedef enum
{
  COUNTRY_India,//Asia
  COUNTRY_SaudiArabia,//Asia
  COUNTRY_Maldives,//Asia
  COUNTRY_UnitedArabEmirates,//Asia
  COUNTRY_Israel,//Asia
  COUNTRY_Mongolia,//Asia
  COUNTRY_Iraq,//Asia
  COUNTRY_Iran,//Asia
  COUNTRY_China,//Asia
  Asia_MAX_NUM    
}Asia_CountryN_t;

typedef enum
{
  COUNTRY_Turkey,//Europe
  COUNTRY_Russia,//Europe
  Europe_MAX_NUM    
}Europe_CountryN_t;


typedef struct
{
  u8 CityName[NAME_LEN];
  
}CityInfo_t;


typedef struct
{
  u8 CityNum;

  u8 ProvienceName[NAME_LEN];

  CityInfo_t *City;
  
}ProvienceInfo_t;



typedef struct
{
  u8 CountryName[NAME_LEN];

  u8 ProvienceNum;

  ProvienceInfo_t *Prov;
}CountryInfo_t;


typedef struct
{
  u8 CountryNum;

  u8 StateName[NAME_LEN];

  CountryInfo_t *Count;
}StateInfo_t;

typedef struct
{
  u8 StateNum;

  StateInfo_t *State[2];
}WordInfo_t;


u8 city_get_state_num();
u8 city_get_country_num(u8 state_index);
u8 city_get_provience_num(u8 state_index, u8 cty_index);
u8 city_get_city_num(u8 state_index, u8 cty_index, u8 prov_index);
void city_get_state_name(u8 state_index, u8 *name);
void city_get_country_name(u8 state_index, u8 cty_index, u8 *name);
void city_get_provience_name(u8 state_index, u8 cty_index, u8 prov_index, u8 *name);
void city_get_city_name(u8 state_index, u8 cty_index, u8 prov_index, u8 city_index,u8 *name);

void init_word_data();

#endif