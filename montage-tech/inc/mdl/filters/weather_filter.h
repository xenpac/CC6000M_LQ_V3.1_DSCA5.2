/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CHAR_FILTER_INTRA_H_
#define __CHAR_FILTER_INTRA_H_

/*!
  subt filter command define
  */
typedef enum tag_weather_filter_cmd
{
  /*!
    start.
    \param[in] 
    */
  WEATHER_CONFIG_WEB_CMD,
  /*!
    start.
    \param[in] 
    */
  WEATHER_PARSE_CMD,
 
}weather_filter_cmd_t;
/*!
  parse
  */
typedef enum tag_weather_web
{
  /*!
    parse done
    */
  WEATHER_WEB_YAHOO,
  /*!
    parse done
    */
  WEATHER_WEB_SINA,
  /*!
    parse done
    */
  WEATHER_WEB_CHINA,
}weather_web_t;
/*!
  weather filter event define
  */
typedef enum tag_weather_filter_evt
{
  /*!
    parse done
    */
  WEATHER_MSG_PARSE_DONE = WEATHER_FILTER << 16, //WEATHER_FILTER << 16,
  /*!
    parse ERROR
    */
  WEATHER_MSG_GET_DATA_ERROR,
  /*!
    parse ERROR
    */
  WEATHER_MSG_PARSE_ERROR,
  /*!
    parse ERROR
    */
  WEATHER_MSG_CITY_ERROR

}weather_filter_evt_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  u8 qu_name[30];
  /*!
  parse done
  */
  u8 py_name[30];
}root_locat_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  root_locat_t locat;
  /*!
  parse done
  */
  u16 prim_locat;
  /*!
  parse done
  */
  u8 tem1[4];
  /*!
  parse done
  */
  u8 tem2[4];
  /*!
  parse done
  */
  u32 state1;
  /*!
  parse done
  */
  u32 state2;  
  /*!
  parse done
  */
  u16 stateDetailed[20];
  /*!
  parse done
  */
  u16 windState[30];
}china_weather_t;

/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char city[20];
  /*!
  parse done
  */
  char region[20];
  /*!
  parse done
  */
  char country[50];
  /*!
    is_monitor
    */
  u32 b_monitor : 1;
  /*!
    monitor interval second
    */
  u32 monitor_interval_sec : 30;
}weather_input_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char temperature[5];
  /*!
  parse done
  */
  char distance[5];
  /*!
  parse done
  */
  char pressure[5];
  /*!
  parse done
  */
  char speed[5];
}weather_units_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char chill[5];
  /*!
  parse done
  */
  char direction[6];
  /*!
  parse done
  */
  char speed[7];
}weather_wind_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char humidity[5];
  /*!
  parse done
  */
  char visibility[6];
  /*!
  parse done
  */
  char pressure[7];
  /*!
  parse done
  */
  u32 rising;
}weather_atmosphere_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char sunrise[10];
  /*!
  parse done
  */
  char sunset[10];
}weather_astronomy_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  char day[4];
  /*!
  parse done
  */
  char date[15];
  /*!
  parse done
  */
  char low[5];
  /*!
  parse done
  */
  char high[5];
  /*!
  parse done
  */
  char text[50];
  /*!
  parse done
  */
  u32 img_code;
}weather_forecast_t;
/*!
  parse done
  */
typedef struct
{
  /*!
  parse done
  */
  u32 woeid;
  /*!
  parse done
  */
  weather_input_t location;
  /*!
  parse done
  */
  weather_units_t units;
  /*!
  parse done
  */
  weather_wind_t wind;
  /*!
  parse done
  */
  weather_atmosphere_t atmosphere;
  /*!
  parse done
  */
  weather_astronomy_t astronomy;
  /*!
  parse done
  */
  weather_forecast_t forecast[5];
  /*!
  parse
  */
 // china_weather_overview_t china_weather[50];
}yahoo_weather_t;
/*!
  parse
  */
typedef struct
{
  /*!
  parse
  */
  u8 qu_name[60];
  /*!
  parse
  */
  u8 qu_status1[30];
  /*!
  parse
  */
  u8 qu_status2[30];
  /*!
  parse
  */
  u8 py_status1[50];
  /*!
  parse
  */
  u8 py_status2[50];
  /*!
  parse
  */
  u8 qu_direction1[30];
  /*!
  parse
  */
  u8 qu_direction2[30];
  /*!
  parse
  */
  u8 py_power1[10];
  /*!
  parse
  */
  u8 py_power2[10];
  /*!
  parse
  */
  u8 temperature1[5];
  /*!
  parse
  */
  u8 temperature2[5];
  /*!
  date
  */
  u8 date[16];
  /*!
  parse
  */
  u32 ssd;
  /*!
  parse
  */
  u32 tgd1;
  /*!
  parse
  */
  u32 tgd2;
  /*!
  parse
  */
  u32 zwx;
  /*!
  parse
  */
  u32 ktk;
  /*!
  parse
  */
  u32 pollution;
  /*!
  parse
  */
  u32 xcz;
  /*!
  parse
  */
  u32 zho;
  /*!
  parse
  */
  u32 diy;
  /*!
  parse
  */
  u32 fas;
  /*!
  parse
  */
  u32 chy;
}sina_weather_t;
/*!
  start.
  */
typedef union
{
  /*!
  parse
  */
  yahoo_weather_t w_yahoo;
  /*!
  parse
  */
  china_weather_t w_china[50];
  /*!
  parse
  */
  sina_weather_t  w_sina;
}weather_web_para_t;

/*!
  start.
  */
typedef struct
{
  /*!
  parse
  */
  weather_web_t weather_web; // 0 yahoo, 1, sina. 2 china weather
  /*!
  parse
  */
  weather_web_para_t para;
}weather_detail_t;

/*!
  parse done
  */
typedef enum tag_weather_img
{
  /*!
  tornado
  */
  WEATHER_IMG_TORNADO                   = 0,
  /*!
  tropical storm
  */
  WEATHER_IMG_TROPICAL_STORM,
  /*!
  hurricane
  */
  WEATHER_IMG_HURRICANE,
  /*!
  severe thunderstorms
  */
  WEATHER_IMG_SEVERE_THUNDERSTORMS,
  /*!
 thunderstorms
  */
  WEATHER_IMG_THUNDERSTORMS,
  /*!
   mixed rain and snow
  */
  WEATHER_IMG_MIXED_RAIN_AND_SNOW,
  /*!
  mixed rain and sleet
  */
  WEATHER_IMG_MIXED_RAIN_AND_SLEET,
  /*!
  mixed snow and sleet
  */
  WEATHER_IMG_MIXED_SNOW_AND_SLEET,
  /*!
  freezing drizzle
  */
  WEATHER_IMG_FREEZING_DRIZZLE,
  /*!
  drizzle
  */
  WEATHER_IMG_DRIZZLE,
  /*!
  freezing rain
  */
  WEATHER_IMG_FREEZING_RAIN             = 10,
  /*!
  showers
  */
  WEATHER_IMG_SHOWERS1,
  /*!
  showers 
  */
  WEATHER_IMG_SHOWERS2,
  /*!
  snow flurries
  */
  WEATHER_IMG_SNOW_FLURRIES,
  /*!
  light snow showers
  */
  WEATHER_IMG_LIGHT_SNOW_SHOWERS,
  /*!
  blowing snow
  */
  WEATHER_IMG_BLOWING_SNOW,
  /*!
  snow
  */
  WEATHER_IMG_SNOW,
  /*!
  hail
  */
  WEATHER_IMG_HAIL,
  /*!
  sleet 
  */
  WEATHER_IMG_SLEET,
  /*!
  dust
  */
  WEATHER_IMG_DUST,
  /*!
  foggy
  */
  WEATHER_IMG_FOGGY                     = 20,
  /*!
  haze
  */
  WEATHER_IMG_HAZE,
  /*!
  smoky
  */
  WEATHER_IMG_SMOKY,
  /*!
  blustery
  */
  WEATHER_IMG_BLUSTERY,
  /*!
  windy
  */
  WEATHER_IMG_WINDY,
  /*!
  cold
  */
  WEATHER_IMG_COLD,
  /*!
  cloudy
  */
  WEATHER_IMG_CLOUDY,
  /*!
  mostly cloudy (night)
  */
  WEATHER_IMG_MOSTLY_CLOUDY_NIGHT,
  /*!
  mostly cloudy (day)
  */
  WEATHER_IMG_MOSTLY_CLOUDY_DAY,
  /*!
  partly cloudy (night)
  */
  WEATHER_IMG_PARTLY_CLOUDY_NIGHT,
  /*!
  partly cloudy (day)
  */
  WEATHER_IMG_PARTLY_CLOUDY_DAY         = 30,
  /*!
  clear (night)
  */
  WEATHER_IMG_CLEAR_NIGHT,
  /*!
  sunny
  */
  WEATHER_IMG_SUNNY,
  /*!
  fair (night)
  */
  WEATHER_IMG_FAIR_NIGHT,
  /*!
  fair (day)
  */
  WEATHER_IMG_FAIR_DAY,
  /*!
  mixed rain and hail
  */
  WEATHER_IMG_MIXED_RAIN_AND_HAIL,
  /*!
  hot
  */
  WEATHER_IMG_HOT,
  /*!
  isolated thunderstorms
  */
  WEATHER_IMG_ISOLATED_THUNDERSTORMS,
  /*!
  scattered thunderstorms
  */
  WEATHER_IMG_SCATTERED_THUNDERSTORMS1,
  /*!
  scattered thunderstorms
  */
  WEATHER_IMG_SCATTERED_THUNDERSTORMS2,
  /*!
  scattered showers
  */
  WEATHER_IMG_SCATTERED_SHOWERS         = 40,
  /*!
  heavy snow
  */
  WEATHER_IMG_HEAVY_SNOW1,
  /*!
  scattered snow showers
  */
  WEATHER_IMG_SCATTERED_SNOW_SHOWERS,
  /*!
  heavy snow
  */
  WEATHER_IMG_HEAVY_SNOW2,
  /*!
  partly cloudy
  */
  WEATHER_IMG_PARTLY_CLOUDY,
  /*!
  thundershowers
  */
  WEATHER_IMG_THUNDERSHOWERS,
  /*!
  snow showers
  */
  WEATHER_IMG_SNOW_SHOWERS,
  /*!
  isolated thundershowers
  */
  WEATHER_IMG_ISOLATED_THUNDERSHOWERS,
  /*!
  not available
  */
  WEATHER_IMG_NOT_AVAILABLE             = 3200,
}weather_img_t;

/*!
  subt_filter_create
  */
ifilter_t * weather_filter_create(void *p_para);

#endif