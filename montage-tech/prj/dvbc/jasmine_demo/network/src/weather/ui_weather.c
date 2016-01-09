/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_weather.h"
//#include "html.h"
//#include "html_parse.h"
#include "weather_filter.h"
#include "weather_api.h"
#include "ui_edit_city.h"
#include "ui_keyboard_v2.h"



enum local_msg
{
  MSG_ADD_CITY = MSG_LOCAL_BEGIN + 850,
  MSG_EDIT_CITY,
  MSG_DELETE_CITY,
  MSG_SWITCH_UNIT,
  MSG_WEATHER_EVT_DATA_UPDATED,
  MSG_WEATHER_EVT_PARSE_ERROR,
  MSG_WEATHER_EVT_CITY_ERROR,
  MSG_WEATHER_EVT_DATA_ERROR,
};

enum select_area_ctl_id
{
  IDC_INVALID = 0,
  IDC_AREA_STATE = 1,
  IDC_AREA_COUNTRY,
  IDC_AREA_PRVIENCE,
  IDC_AREA_CITY,
};

enum weather_ctl_id
{

  IDC_AREA_LIST = 11,
  IDC_AREA_NAME_ITEM = 12,
  IDC_WEATHER_DETAIL_CONT = 22,
  IDC_WEATHER_CITY_CONT = 23,
  IDC_HELP_BAR = 24,
  IDC_WEATHER_INFO_ITEM = 25,
  IDC_WEATHER_CITY = 45,
  IDC_WEATHER_CITY_INFO = 46,
  IDC_WEATHER_CITY_TEMP1 = 47,
  IDC_WEATHER_CITY_TEMP2 = 48,
  IDC_WEATHER_CITY_TEMP3 = 49,
  IDC_WEATHER_CITY_PICTURE = 50,
  IDC_WEATHER_DATA_CONT = 51,
  IDC_WEATHER_PICTURE_CONT = 52,
  IDC_WEATHER_TEMP_CONT = 53,
  IDC_WEATHER_DATE_ITEM = 54,
  IDC_WEATHER_PIC_ITEM = 66,
  IDC_WEATHER_TEMP_ITEM = 75,
};

static list_xstyle_t area_list_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_12_SH,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
};

static list_xstyle_t area_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t area_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t area_list_attr[AREA_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    15, 9, 10, &area_list_field_rstyle,  &area_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    185, 25, 10, &area_list_field_rstyle,  &area_list_field_fstyle},
};

typedef struct
{
  u32 id;
  u32 id_pic_big;
  u32 id_pic_small;
  u8 des[30];
}ui_weather_pic_t;

static u32 city_count = 0;
static BOOL b_add = FALSE;
static u16 city_name[10][32] ={{0},};

ui_weather_pic_t g_pic_idx[MAX_WEATHER_PIC] = {
  {0,IM_WEATHERICONS_TWISTER_1,IM_WEATHERICONS_TWISTER_2,"tornado"},
  {1,IM_WEATHERICONS_TWISTER_1,IM_WEATHERICONS_TWISTER_2,"tropical storm"},
  {2,IM_WEATHERICONS_TWISTER_1,IM_WEATHERICONS_TWISTER_2,"hurricane"},
  {3,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"severe thunderstorms"},
  {4,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"thunderstorms"},
  {5,IM_WEATHERICONS_SLEET_1,IM_WEATHERICONS_SLEET_2,"mixed rain and snow"},
  {6,IM_WEATHERICONS_SLEET_1,IM_WEATHERICONS_SLEET_2,"mixed rain and sleet"},
  {7,IM_WEATHERICONS_SLEET_1,IM_WEATHERICONS_SLEET_2,"mixed snow and sleet"},
  {8,IM_WEATHERICONS_RAIN_SMALL_1,IM_WEATHERICONS_RAIN_SMALL_2,"freezing drizzle"},
  {9,IM_WEATHERICONS_RAIN_SMALL_1,IM_WEATHERICONS_RAIN_SMALL_2,"drizzle"},
  {10,IM_WEATHERICONS_RAIN_SMALL_1,IM_WEATHERICONS_RAIN_SMALL_2,"freezing rain"},
  {11,IM_WEATHERICONS_RAIN_MID_1,IM_WEATHERICONS_RAIN_MID_2,"showers"},
  {12,IM_WEATHERICONS_RAIN_MID_1,IM_WEATHERICONS_RAIN_MID_2,"showers"},
  {13,IM_WEATHERICONS_SNOW_MID_1,IM_WEATHERICONS_SNOW_MID_2,"snow flurries"},
  {14,IM_WEATHERICONS_SNOW_MID_1,IM_WEATHERICONS_SNOW_MID_2,"light snow showers"},
  {15,IM_WEATHERICONS_SNOW_BIG_1,IM_WEATHERICONS_SNOW_BIG_2,"blowing snow"},
  {16,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"snow"},
  {17,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"hail"},
  {18,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"sleet"},
  {19,IM_WEATHERICONS_SANDSTORN_1,IM_WEATHERICONS_SANDSTORN_2,"dust"},
  {20,IM_WEATHERICONS_RAIN_SMALL_1,IM_WEATHERICONS_RAIN_SMALL_2,"foggy"},
  {21,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"haze"},
  {22,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"smoky"},
  {23,IM_WEATHERICONS_WIND_1,IM_WEATHERICONS_WIND_2,"blustery"},
  {24,IM_WEATHERICONS_WIND_1,IM_WEATHERICONS_WIND_2,"windy"},
  {25,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"cold"},
  {26,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"cloudy"},
  {27,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"mostly cloudy (night)"},
  {28,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"mostly cloudy (day)"},
  {29,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"partly cloudy (night)"},
  {30,IM_WEATHERICONS_CLOUDYSKY_1,IM_WEATHERICONS_CLOUDYSKY_2,"partly cloudy (day)"},
  {31,IM_WEATHERICONS_SUN_1,IM_WEATHERICONS_SUN_2,"clear (night)"},
  {32,IM_WEATHERICONS_SUN_1,IM_WEATHERICONS_SUN_2,"sunny"},
  {33,IM_WEATHERICONS_SUNNYTHENCLOUDY_1,IM_WEATHERICONS_SUNNYTHENCLOUDY_2,"fair (night)"},
  {34,IM_WEATHERICONS_SUNNYTHENCLOUDY_1,IM_WEATHERICONS_SUNNYTHENCLOUDY_2,"fair (day)"}, 	
  {35,IM_WEATHERICONS_SLEET_1,IM_WEATHERICONS_SLEET_2,"mixed rain and hail"},
  {36,IM_WEATHERICONS_SUN_1,IM_WEATHERICONS_SUN_2,"hot"},
  {37,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"isolated thunderstorms"},
  {38,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"scattered thunderstorms"},
  {39,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"scattered thunderstorms"},
  {40,IM_WEATHERICONS_RAIN_1,IM_WEATHERICONS_RAIN_2,"scattered showers"},
  {41,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"heavy snow"},
  {42,IM_WEATHERICONS_SNOW_BIG_1,IM_WEATHERICONS_SNOW_BIG_2,"scattered snow showers"},
  {43,IM_WEATHERICONS_SNOW_BIG_1,IM_WEATHERICONS_SNOW_BIG_2,"heavy snow"},
  {44,IM_WEATHERICONS_CLOUDY_1,IM_WEATHERICONS_CLOUDY_2,"partly cloudy"},
  {45,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"thundershowers"},
  {46,IM_WEATHERICONS_SNOW_1,IM_WEATHERICONS_SNOW_2,"snow showers"},
  {47,IM_WEATHERICONS_THUNDERL_1,IM_WEATHERICONS_THUNDERL_2,"isolated thundershowers"}, 
  {3200,0,0,"not available"}};

comm_dlg_data_t weather_delete_dlg_data =
{
	ROOT_ID_WEATHER_FORECAST,
	DLG_FOR_ASK | DLG_STR_MODE_STATIC,
	COMM_DLG_X,                        COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
	IDS_MSG_ASK_FOR_DEL,
	0,
};

extern void gb2312_2_utf16le(char* inbuf, int in_len, char* outbuf, int out_len);
typedef struct 
{
  int low, high;
}temp_t;
typedef struct
{
  temp_t temp_cf[2]; //0 for C, 1 for F
}temp_cf_t;
static temp_cf_t sg_temperature[5];
static int sg_cur_temp[2]; //0 for C, 1 for F
static u8 sg_c_or_f; // 0, celsius, 1, fahr
int celsius_to_fahr(int cels)
{
  float fahr = cels*9.0/5 + 32;
  int f = cels*9/5 + 32;
  if( (fahr - f) >= 0.5)
    f++;
  return f;
}
u16 weather_area_select_cont_keymap(u16 key);
RET_CODE weather_area_select_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
//static void weather_set_keymap_proc(u16 idc);
u16 weather_cont_keymap(u16 key);
RET_CODE weather_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

#ifdef WIN32
int snprintf(char *string, size_t count, const char *format, ...)
{
  char temp[1024] = {0};
  int ret = 0;
  ret = sprintf(temp, format);
  memcpy(string, temp, count);
  string[count] = 0;
  return ret;
}
#endif
void fill_weather_detail_city(control_t *p_detail_cont, weather_detail_t *p_data)
{
  control_t *p_city_cont = NULL;
  control_t *p_tmp_cont = NULL;
  u8 city[100] = {0};
  char* fmt;
  u8 asc_str[32];
  u16 uni_str[32];
  int t;
  p_city_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY);
  sprintf(city, "%s/%s/%s", p_data->para.w_yahoo.location.city, p_data->para.w_yahoo.location.region, p_data->para.w_yahoo.location.country);
  text_set_content_by_ascstr(p_city_cont, city);
  p_tmp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY_TEMP1);
  fmt = sg_c_or_f ? "%d°„F" : "%d°„C";
  t = atoi(p_data->para.w_yahoo.wind.chill);
  sg_cur_temp[0] = t;
  sg_cur_temp[1] = celsius_to_fahr(t);
  
  snprintf(asc_str, sizeof(asc_str), fmt, sg_cur_temp[sg_c_or_f]);
  gb2312_2_utf16le(asc_str, strlen(asc_str), (char*)uni_str, sizeof(uni_str)-1);
  text_set_content_by_unistr(p_tmp_cont, uni_str);
  return;
}
/*
void fill_weather_big_temp(control_t *p_detail_cont, weather_detail_t *p_data)
{
  control_t *p_temp_cont = NULL;
	u8 word[5] = {0};
	u8 len = 0;
  int cont = 0;
  u32 i;
  u8 num[10] = {IM_NUMBER_0,IM_NUMBER_1,IM_NUMBER_2,IM_NUMBER_3,IM_NUMBER_4,IM_NUMBER_5,
                IM_NUMBER_6,IM_NUMBER_7,IM_NUMBER_8,IM_NUMBER_9};
  strcpy(word, p_data->wind.chill);
	len = strlen(p_data->wind.chill);
  for(i = 0; i < len; i++)
  {
    cont = atoi(word[i]);
    p_temp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY_TEMP1+i);
    bmap_set_content_by_id(p_temp_cont, num[cont]);
  }
  p_temp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY_TEMP3);
  bmap_set_content_by_id(p_temp_cont, IM_WEATHER_DEGREE_1);

  
}*/

void fill_weather_detail_temp(control_t *p_detail_cont, weather_detail_t *p_data)
{
  control_t *p_temp_cont = NULL;
  control_t *p_forecast_temp_cont = NULL;
  control_t *p_cont = NULL;
  u32 pic_id = 0;
  u32 id_pic_big = 0;
  u32 id_pic_small = 0;
  int i;
  p_temp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY_PICTURE);
  p_forecast_temp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_PICTURE_CONT);
  for(i = 0; i < 5; i++)
  {
    pic_id = g_pic_idx[p_data->para.w_yahoo.forecast[i].img_code].id;
    id_pic_big = g_pic_idx[p_data->para.w_yahoo.forecast[i].img_code].id_pic_big;
    id_pic_small = g_pic_idx[p_data->para.w_yahoo.forecast[i].img_code].id_pic_small;
    if(pic_id == p_data->para.w_yahoo.forecast[i].img_code)
    {
      p_cont = ctrl_get_child_by_id(p_forecast_temp_cont, IDC_WEATHER_PIC_ITEM + i);
      if(i == 0)
      {
        bmap_set_content_by_id(p_temp_cont, id_pic_big);
        bmap_set_content_by_id(p_cont, id_pic_small);
      }else{
        bmap_set_content_by_id(p_cont, id_pic_small);
      }
    }  
  }
}

void fill_weather_detail_param(control_t *p_detail_cont, weather_detail_t *p_data)
{

	control_t *p_item = NULL;
	int i = 0;
	u8 tmp[20] = {0};
	//p_param_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_DETAIL_CONT);
	for (i = 1; i < 12; i += 2)
	{
		memset(tmp, 0, 20);
		p_item = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_INFO_ITEM+i);
		switch(i)
		{
			case 1://sunrise
				text_set_content_by_ascstr(p_item, p_data->para.w_yahoo.astronomy.sunrise);
				break;
			case 3://sunset
				text_set_content_by_ascstr(p_item, p_data->para.w_yahoo.astronomy.sunset);
				break;
			case 5://humidity
				sprintf(tmp, "%s %%", p_data->para.w_yahoo.atmosphere.humidity);
				text_set_content_by_ascstr(p_item, tmp);
				break;
			case 7://visibility
				sprintf(tmp, "%s %s", p_data->para.w_yahoo.atmosphere.visibility, p_data->para.w_yahoo.units.distance);
				text_set_content_by_ascstr(p_item, tmp);
				break;
			case 9://wind
				sprintf(tmp, "%s %s", p_data->para.w_yahoo.wind.speed, p_data->para.w_yahoo.units.speed);
				text_set_content_by_ascstr(p_item, tmp);
				break;
			case 11://Pressure
				sprintf(tmp, "%s mb", p_data->para.w_yahoo.atmosphere.pressure);
				text_set_content_by_ascstr(p_item, tmp);
				break;
		}
	}
	
	
}
void fill_weather_forecast_date(control_t *p_detail_cont, weather_detail_t *p_data)
{
  control_t *p_forecast_cont = NULL;
  control_t *p_cont = NULL;
  u32 week = 0;
	u32 i;
  p_forecast_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_DATA_CONT);
  for(i = 0; i < 10; i++)
  {
    p_cont = ctrl_get_child_by_id(p_forecast_cont, IDC_WEATHER_DATE_ITEM + i);
    if(i < 5)
    {
			if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Mon"))
        week = IDS_MONDAY;
			if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Tue"))
			  week = IDS_TUESDAY; 
      if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Wed"))
			  week = IDS_WEDNESDAY;
      if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Thu"))
			  week = IDS_THURSDAY;
      if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Fri"))
			  week = IDS_FRIDAY;
      if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Sat"))
			  week = IDS_SATURDAY;
      if(0 == strcmp(p_data->para.w_yahoo.forecast[i].day, "Sun"))
			  week = IDS_SUNDAY;
      text_set_content_by_strid(p_cont, week);
    }else{
      text_set_content_by_ascstr(p_cont, p_data->para.w_yahoo.forecast[i-5].date);
    }
  }
}

void fill_weather_forecast_temperature(control_t *p_detail_cont, weather_detail_t *p_data)
{
  control_t *p_temp_cont = NULL;
  control_t *p_temp_item = NULL;
  u8 asc_str[64] = {0};
	//u16 tmpf[2] = {0x2109, 0};
  u16 uni_str[64] = {0};
	//u8 string_asc[8] = {0};
  int i;
  int low, high, t;
  char* fmt;
  fmt = sg_c_or_f ? "%d°„F - %d°„F" : "%d°„C - %d°„C";
  p_temp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_TEMP_CONT);
  for(i = 0; i < 5; i++)
  {
    p_temp_item = ctrl_get_child_by_id(p_temp_cont, IDC_WEATHER_TEMP_ITEM + i);

		//u_itoa(p_data->forecast[i].low, string_asc, 10);
		//str_asc2uni(string_asc, low);

		//u_itoa(p_data->forecast[i].high, string_asc, 10);
		//str_asc2uni(string_asc, high);

    t = atoi(p_data->para.w_yahoo.forecast[i].low);
    sg_temperature[i].temp_cf[0].low = t;
    sg_temperature[i].temp_cf[1].low = celsius_to_fahr(t);
    t = atoi(p_data->para.w_yahoo.forecast[i].high);
    sg_temperature[i].temp_cf[0].high = t;
    sg_temperature[i].temp_cf[1].high = celsius_to_fahr(t);

    low = sg_temperature[i].temp_cf[sg_c_or_f].low;
    high = sg_temperature[i].temp_cf[sg_c_or_f].high;

    snprintf(asc_str, sizeof(asc_str), fmt, low, high);

    gb2312_2_utf16le(asc_str, strlen(asc_str), (char*)uni_str, sizeof(uni_str)-1);
    text_set_content_by_unistr(p_temp_item, uni_str);
  }
}

static RET_CODE on_weather_update_data(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	weather_detail_t *p_data = (weather_detail_t *)para2;
  control_t *p_detail_cont = NULL;
  p_detail_cont = ctrl_get_child_by_id(p_cont, IDC_WEATHER_DETAIL_CONT);
  fill_weather_detail_city(p_detail_cont, p_data);
  //fill_weather_big_temp(p_detail_cont, p_data);
	fill_weather_detail_param(p_detail_cont, p_data);
	fill_weather_forecast_date(p_detail_cont, p_data);
  fill_weather_forecast_temperature(p_detail_cont, p_data);
  fill_weather_detail_temp(p_detail_cont, p_data);
  ctrl_paint_ctrl(p_detail_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_weather_parse_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 3000);
  return SUCCESS;
}

void ui_weather_call_back(mul_weather_state_t event_type, s32 event_value, void *p_args)
{
  if(MUL_WEATHER_PARSE_DONE == event_type)
  {
    if(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST))
    {
    fw_notify_root(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST), NOTIFY_T_MSG, FALSE,
  			MSG_WEATHER_EVT_DATA_UPDATED, event_value, (u32)p_args);
    }
  }
  else if(MUL_WEATHER_PARSE_ERROR == event_type)
  {
    if(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST))
    {
    fw_notify_root(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST), NOTIFY_T_MSG, FALSE,
			MSG_WEATHER_EVT_PARSE_ERROR, event_value, (u32)p_args);
    }
  }
  else if(MUL_WEATHER_CITY_ERROR == event_type)
  {
    if(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST))
    {
    fw_notify_root(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST), NOTIFY_T_MSG, FALSE,
			MSG_WEATHER_EVT_CITY_ERROR, event_value, (u32)p_args);
    }
  }
  else if(MUL_WEATHER_DATA_ERROR == event_type)
  {
    if(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST))
    {
    fw_notify_root(fw_find_root_by_id(ROOT_ID_WEATHER_FORECAST), NOTIFY_T_MSG, FALSE,
			MSG_WEATHER_EVT_DATA_ERROR, event_value, (u32)p_args);
    }
  }
  
}

static void do_add_city(u16 *name)
{
  uni_strncpy((u16*)(city_name[city_count]), name, 32);
  //city_count ++;
  sys_status_set_city(city_count,name);
  city_count ++;
  sys_status_set_city_cnt(city_count);
  sys_status_save();
}

static void do_edit_city(u16 focus,u16 *name)
{
  //uni_strncpy((u16*)(city_name[focus]), name, 32);
  sys_status_set_city(focus,name);
  sys_status_save();
}

static void do_delete_city(u16 focus,void *name)
{
  u16 i = 0;
  u16 city_name[32]={0};

  for(i =focus; i < city_count - 1; i++)
  {
     sys_status_get_city(i+1, city_name);
     sys_status_set_city(i, city_name);
  }

  city_count --;
  sys_status_set_city_cnt(city_count);
  sys_status_save();
}

static void add_city(void)
{
  edit_city_param_t param;
  char *name = "\0";

  b_add = 1;
  param.uni_str = (u16*) name;
  param.max_len = 32;
//  param.cb = edit_ip_update;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.add_type = 1;

  manage_open_menu(ROOT_ID_EDIT_CITY, (u32)&param, ROOT_ID_WEATHER_FORECAST);
}

static void edit_city(u16 *name)
{
 edit_city_param_t param;

  b_add = 0;

  param.max_len = 32;
  param.uni_str = (u16 *)name;
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.add_type = 0;

  manage_open_menu(ROOT_ID_EDIT_CITY, (u32)&param, ROOT_ID_WEATHER_FORECAST);
}

static RET_CODE city_list_update(control_t* p_list, u16 start, u16 size,
                                     u32 context)
{
  u16 cnt = list_get_count(p_list);
  u16 i;
  u8 asc_str[8];
  u16 city_name[32]={0};

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf(asc_str, "%.d ", (u16)(start + i+1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      sys_status_get_city(i + start, city_name);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1,city_name);
    }

  }
  return SUCCESS;
}

static RET_CODE start_city_name_load(control_t *p_ctrl)
{
  u16 focus, total;
  control_t *p_list;
  u16 city_cnt = 0;
  u16 city_name[50] = {0};
  u8 asc_str[8];
	u32 i;
  p_list = ui_comm_root_get_ctrl(ROOT_ID_WEATHER_FORECAST, IDC_AREA_LIST);
  MT_ASSERT(p_list != NULL);
  city_cnt = sys_status_get_city_cnt();

  if(city_count != 0 && city_count != city_cnt)
  {
    OS_PRINTF("*ctiy num has error! why...! what to do...\n");
  }

  // for bug 33921. when open weather, 
  // we update city_count whether city_cnt is 0 or not.
  city_count = city_cnt;
  if(0 == city_cnt)
    return SUCCESS;
	
	total = city_cnt;
	focus = city_cnt - 1;
	list_set_count(p_list, total, AREA_LIST_PAGE);
	list_set_focus_pos(p_list, focus);

  for(i = 0; i < city_cnt; i++)
  {
    sys_status_get_city(i, city_name);
    sprintf(asc_str, "%.d ", (u16)(i+1));
    list_set_field_content_by_ascstr(p_list, (u16)(i), 0, asc_str);
    list_set_field_content_by_unistr(p_list, (u16)(i), 1,city_name);
  }
  city_count = city_cnt;
  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  return SUCCESS;
}
/*
static void delete_city(u16 focus,void *name)
{
  u16 i = 0;
  for(i =focus;i<city_count - 1;i++)
  {
    uni_strncpy((u16*)(city_name[i]), (u16*)(city_name[i+1]), 32);
  }
   memset((void* )(city_name[city_count - 1]),0, 16);

   city_count --;
}
*/
RET_CODE open_weather(u32 para1, u32 para2)
{
  control_t *p_menu, *p_weather_detail_cont, *p_help_bar,*p_ctrl,*p_area_list;
  control_t  *p_data_cont, *p_pic_cont, *p_temp_cont;
  u16 x=0, y=0, w=0, h=0, i =0;// ,j = 0;
  u32 text_type = 0, align_type = 0;

  mul_weather_attr_t attr_data = {0};
	int context = 10110;

  comm_help_data_t help_data =
  {
     7,7,
    {IDS_MOVE,IDS_ADD,IDS_EDIT, IDS_DELETE, IDS_CHANGE_UNIT,IDS_OK,IDS_EXIT},
    {IM_HELP_ARROW,IM_HELP_RED,IM_HELP_GREEN,IM_HELP_YELLOW,IM_HELP_BLUE,IM_HELP_OK,IM_HELP_EXIT}
  };
  u16 weather_info_str[WEATHER_INFO_CNT] =
  {
    IDS_SUNRISE,
    IDS_SUNSET,
    IDS_HUMIDITY,
    IDS_VISIBILITY,
    IDS_WIND,
    IDS_PRESSURE,
  };
  /*u16 area_str[WEATHER_AREA_CNT] =
  {
    IDS_STATE,
    IDS_COUNTRY,
    IDS_PROVIENCE,
    IDS_CITY,
  };*/

	//attr_data.priority = WEATHER_CHAIN_PRIORITY;
	attr_data.stk_size = 16 * KBYTES;
  p_menu = ui_comm_root_create_netmenu(ROOT_ID_WEATHER_FORECAST, 0, IM_INDEX_WEATHER_BANNER, IDS_WEATHER_FORECAST);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_menu, weather_cont_keymap);
  ctrl_set_proc(p_menu, weather_cont_proc);
  //left area container
  p_area_list =
			ctrl_create_ctrl(CTRL_LIST, IDC_AREA_LIST, WEATHER_AREA_LIST_X,
			WEATHER_AREA_LIST_Y, WEATHER_AREA_LIST_W+30,
			WEATHER_AREA_LIST_H, p_menu, 0);
  
  ctrl_set_rstyle(p_area_list, RSI_ITEM_11_SH, RSI_ITEM_11_SH, RSI_ITEM_11_SH);
  ctrl_set_keymap(p_area_list, weather_area_select_cont_keymap);
  ctrl_set_proc(p_area_list, weather_area_select_cont_proc);

  ////
  ctrl_set_mrect(p_area_list, WEATHER_AREA_LIST_MID_L, WEATHER_AREA_LIST_MID_T,
                          WEATHER_AREA_LIST_MID_W , 
                          WEATHER_AREA_LIST_MID_H );
                          //WEATHER_AREA_LIST_MID_W + WEATHER_AREA_LIST_MID_L, 
                          //WEATHER_AREA_LIST_MID_H + WEATHER_AREA_LIST_MID_T);
  list_set_item_interval(p_area_list, WEATHER_AREA_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_area_list, &area_list_item_rstyle);
  list_enable_select_mode(p_area_list, TRUE);
  list_set_count(p_area_list, 0, AREA_LIST_PAGE);
  list_set_field_count(p_area_list, AREA_LIST_FIELD, AREA_LIST_PAGE);
  list_set_focus_pos(p_area_list, 0);
  list_set_update(p_area_list,city_list_update, 0);
  
  for (i = 0; i < AREA_LIST_FIELD; i++)
  {
    list_set_field_attr(p_area_list, (u8)i, (u32)(area_list_attr[i].attr),
      (u16)(area_list_attr[i].width),
      (u16)(area_list_attr[i].left),
      (u8)(area_list_attr[i].top));
    list_set_field_rect_style(p_area_list, (u8)i, area_list_attr[i].rstyle);
    list_set_field_font_style(p_area_list, (u8)i, area_list_attr[i].fstyle);
  }
  start_city_name_load(p_area_list);
  //right weather detail info container
  p_weather_detail_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_WEATHER_DETAIL_CONT, WEATHER_DETAIL_CONT_X,
			WEATHER_DETAIL_CONT_Y, WEATHER_DETAIL_CONT_W,
			WEATHER_DETAIL_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_weather_detail_cont, RSI_ITEM_12_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);

//city infor
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_WEATHER_CITY, 
                    5, 0, 
                    WEATHER_CITY_DETAIL_W, WEATHER_CITY_DETAIL_H, 
                    p_weather_detail_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_2_HL, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    //text_set_content_by_strid(p_ctrl, IDS_CITY);
//city wether  infor
 
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_WEATHER_CITY_TEMP1,
                              WEATHER_TEMP_DETAIL_X, WEATHER_TEMP_DETAIL_Y, 
                              WEATHER_TEMP_DETAIL_W, WEATHER_TEMP_DETAIL_H, 
                              p_weather_detail_cont, 0);
    #ifndef MODEST
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_12_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    #endif
    text_set_font_style(p_ctrl, FSI_HUGE, FSI_HUGE, FSI_HUGE);
    text_set_align_type(p_ctrl, STL_CENTER | STL_CENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
//city weather picture
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_WEATHER_CITY_PICTURE,
    200, 65,
    200, 200,
    p_weather_detail_cont, 0);
#ifndef MODEST
  ctrl_set_rstyle(p_ctrl, RSI_ITEM_12_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
#endif
  //bmap_set_content_by_id(p_ctrl, IM_HELP_ARROW);
  
//weather detail information text
  x = WEATHER_DETAIL_INFO_ITEM_X;
  y = WEATHER_DETAIL_INFO_ITEM_Y;
  h = WEATHER_DETAIL_INFO_ITEM_H;
  for(i=0; i<12; i++)
  {
    switch(i%2)
    {
      case 0:
        x = WEATHER_DETAIL_INFO_ITEM_X;
        y = WEATHER_DETAIL_INFO_ITEM_Y+i/2*(WEATHER_DETAIL_INFO_ITEM_H+WEATHER_DETAIL_INFO_ITEM_VGAP);
        w = WEATHER_DETAIL_INFO_ITEM_W;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_LEFT | STL_VCENTER;
        break;

      case 1:
        x = WEATHER_DETAIL_INFO_ITEM_X+WEATHER_DETAIL_INFO_ITEM_W+WEATHER_DETAIL_INFO_ITEM_HGAP;
        w = WEATHER_DETAIL_INFO_ITEM_W;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_LEFT| STL_VCENTER;
        break;
    }

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_WEATHER_INFO_ITEM+i),
                              x, y, w, h, p_weather_detail_cont, 0);
    #ifndef MODEST
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_12_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    #endif
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i%2==0)
    {
      text_set_content_by_strid(p_ctrl, weather_info_str[i/2]);
    }
  }

//5 days data container
p_data_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_WEATHER_DATA_CONT, 0, 260, WEATHER_DETAIL_CONT_W,
80, p_weather_detail_cont, 0);
ctrl_set_rstyle(p_data_cont, RSI_BLACK_FRM, RSI_BLACK_FRM, RSI_BLACK_FRM);
//5 days temperature
  x = 0;
  y = 0;
  h = 40;
  for(i=0; i<10; i++)
  {
    switch(i)
    {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
        x = 0+i*(800/5 + 0);
        y = 0;
        w = 800/5;
        text_type = TEXT_STRTYPE_STRID;
        align_type = STL_CENTER | STL_CENTER;
        break;

      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
        x = 0+(i%5)*(800/5 + 0);
        y = 0+40+0;
        w = 800/5;
        text_type = TEXT_STRTYPE_UNICODE;
        align_type = STL_CENTER| STL_CENTER;
        break;
    }

    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_WEATHER_DATE_ITEM+i),
                              x, y, w, h, p_data_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, align_type);
    text_set_content_type(p_ctrl, text_type);

    if(i<5)
    {
      //text_set_content_by_strid(p_ctrl, week_str[i]);
    }
  }
//5 days picture container
p_pic_cont= ctrl_create_ctrl(CTRL_TEXT, IDC_WEATHER_PICTURE_CONT, 0, 260+80, WEATHER_DETAIL_CONT_W,
130, p_weather_detail_cont, 0);
ctrl_set_rstyle(p_ctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
x = 0;
y = 0;
h = 130;
for(i=0; i<5; i++)
{
    x = 0+i*(800/5 + 0);
    w = 800/5;
    p_ctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_WEATHER_PIC_ITEM+i),
                          x, y, w, h, p_pic_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    //bmap_set_content_by_id(p_ctrl, IM_HELP_ARROW);
 }

//5 days temperature container
p_temp_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_WEATHER_TEMP_CONT, 0, 260+80+130, WEATHER_DETAIL_CONT_W,
40, p_weather_detail_cont, 0);
ctrl_set_rstyle(p_temp_cont, RSI_BLACK_FRM, RSI_BLACK_FRM, RSI_BLACK_FRM);
x = 0;
y = 0;
h = 40;
for(i=0; i<5; i++)
{
    x = 0+i*(800/5 + 0);
    w = 800/5;
    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_WEATHER_TEMP_ITEM+i),
                          x, y, w, h, p_temp_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
    text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_ctrl, TEXT_STRTYPE_UNICODE);
    text_set_content_type(p_ctrl, STL_CENTER| STL_CENTER);
   // text_set_content_by_strid(p_ctrl, IDS_DAILY);
    //text_set_content_by_ascstr(p_ctrl, "C - C");

 }

//bottom help bar
  p_help_bar = ctrl_create_ctrl(CTRL_TEXT, IDC_HELP_BAR,
                   WEATHER_BOTTOM_HELP_X, WEATHER_BOTTOM_HELP_Y, WEATHER_BOTTOM_HELP_W,
                   WEATHER_BOTTOM_HELP_H, p_menu, 0);
  ctrl_set_rstyle(p_help_bar, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_help_bar, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_help_bar, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_help_bar, TEXT_STRTYPE_UNICODE);
  ui_comm_help_create_ext(40, 0, WEATHER_BOTTOM_HELP_W-40, WEATHER_BOTTOM_HELP_H,  &help_data,  p_help_bar);
  //ctrl_default_proc(p_select_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_default_proc(p_area_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

  city_count = sys_status_get_city_cnt();

  mul_weather_init();
  OS_PRINTF("weather--init\n");
  mul_weather_create(&attr_data);
  OS_PRINTF("weather--create\n");
  mul_weather_register_event((weather_call_back)ui_weather_call_back, (void *)context);
  OS_PRINTF("weather--end\n");
  mul_weather_start();
  OS_PRINTF("weather--start\n");
  return SUCCESS;

}


static RET_CODE on_weather_add_city(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  if(city_count > 8)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, NULL, 0);
  }
  else
  {
    add_city();
  }
  return SUCCESS;
}

static RET_CODE on_weather_edit_city(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  u16 focus;
  u16 city_name[32]={0};
  if(city_count > 0)
  {
    focus = list_get_focus_pos(ctrl);
    sys_status_get_city(focus, city_name);
    edit_city((u16 *)(city_name));
  }

  return SUCCESS;
}

/*
static RET_CODE on_weather_delete_city(control_t *ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  u16 focus;
  control_t  *p_list;
  dlg_ret_t dlg_ret;
  u16 ip_name[32]={0};


  if(city_count > 0)
  {
	dlg_ret = ui_comm_dlg_open(&neteork_delete_dlg_data);

	if(dlg_ret == DLG_RET_YES)
	{
    focus = list_get_focus_pos(ctrl);
    //sys_status_get_ip_path_set(focus, ip_name);
     delete_ip(focus, (u16 *)(ip_name));

     p_list = ui_comm_root_get_ctrl(ROOT_ID_WEATHER_FORECAST, IDC_AREA_LIST);
     MT_ASSERT(p_list != NULL);
     list_set_count(p_list, city_count, AREA_LIST_PAGE);
     if(focus < city_count)
      {
       list_set_focus_pos(p_list, focus);
      }
     else
      {
       list_set_focus_pos(p_list, city_count - 1);
      }
       //network_list_update(p_list, list_get_valid_pos(p_list), NETWORK_LIST_PAGE, 0);

       ctrl_paint_ctrl(p_list, TRUE);
   	}
  }

  return SUCCESS;
}
*/
static RET_CODE on_weather_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  mul_weather_unregister_event();
  mul_weather_stop();
  mul_weather_destroy_chn();
  mul_weather_deinit();
  manage_close_menu(ROOT_ID_WEATHER_FORECAST, 0, 0);
  return SUCCESS;
}

static RET_CODE check_network_conn()
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;

  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
     && (eth_connt_stats.is_gprs_conn == FALSE) && (eth_connt_stats.is_3g_conn == FALSE))
  {
     ret = ERR_FAILURE;  
     ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
  }

  return ret;
}

static RET_CODE on_weather_area_select(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL;
  u16 item_idx = 0;
  u16 *p_uni_str = NULL;
  u8 city[50] = {0};
	//u8 city[50] = "xian";
 weather_input_t input = {{0}};
// sprintf(input.city,"shanghai");

  if(ERR_FAILURE == check_network_conn())
  {
    return ERR_FAILURE;
  }
  
  p_list = ui_comm_root_get_ctrl(ROOT_ID_WEATHER_FORECAST, IDC_AREA_LIST);
  item_idx = list_get_focus_pos(p_list);
  p_uni_str = (u16 *)list_get_field_content(p_list, item_idx, 1);
	if (NULL == p_uni_str)
	{
		return ERR_FAILURE;
	}
  str_uni2asc(city, p_uni_str);
  
  sprintf(input.city,city);
  mul_weather_config(&input, WEATHER_WEB_YAHOO);
  //mul_weather_start();
  OS_PRINTF("@@@@on_weather_area_select@@@@\n");
  return SUCCESS;
}

static RET_CODE on_weather_name_check(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_weather_name_update(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u16 *name = (u16 *)para1;
  u16 focus, total;
  control_t  *p_list;
  p_list = ui_comm_root_get_ctrl(ROOT_ID_WEATHER_FORECAST, IDC_AREA_LIST);
  MT_ASSERT(p_list != NULL);
  if(b_add)
  {
    //add city
    do_add_city(name);

    total = city_count;
    focus = city_count - 1;
    list_set_count(p_list, total, AREA_LIST_PAGE);
    list_set_focus_pos(p_list, focus);
    city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
  }
  else
  {
    //edit city
    focus = list_get_focus_pos(p_list);
    do_edit_city(focus,name);
    city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
  }

  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
  return SUCCESS;

}



static RET_CODE on_weather_switch_unit(control_t *p_ctrl,u16 msg,
                                      u32 para1, u32 para2)
{
    control_t *p_detail_cont = NULL;
    control_t *p_temp_ctrl = NULL;
    control_t *p_temp_item = NULL;
    control_t *p_tmp_cont = NULL;
    control_t *p_cont = NULL;
    int i;

    u8 asc_str[64] = {0};
    u16 uni_str[64] = {0};
    int low, high;
    static u32 cnt;
    char* fmt;

    p_cont = ctrl_get_parent(p_ctrl);
    p_detail_cont = ctrl_get_child_by_id(p_cont, IDC_WEATHER_DETAIL_CONT);
    p_temp_ctrl = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_CITY_TEMP1);
  cnt++;
  sg_c_or_f = cnt % 2;
  fmt = sg_c_or_f ?  "%d°„F - %d°„F" : "%d°„C - %d°„C";

    p_tmp_cont = ctrl_get_child_by_id(p_detail_cont, IDC_WEATHER_TEMP_CONT);
    for(i = 0; i < 5; i++)
    {
         p_temp_item = ctrl_get_child_by_id(p_tmp_cont, IDC_WEATHER_TEMP_ITEM + i);

    low = sg_temperature[i].temp_cf[cnt%2].low;
    high = sg_temperature[i].temp_cf[cnt%2].high;

    snprintf(asc_str, sizeof(asc_str), fmt, low, high);
    gb2312_2_utf16le(asc_str, strlen(asc_str) + 1, (char*)uni_str, sizeof(uni_str)-1);
    text_set_content_by_unistr(p_temp_item, uni_str);
              }

  fmt = sg_c_or_f ? "%d°„F" : "%d°„C";
  snprintf(asc_str, sizeof(asc_str), fmt, sg_cur_temp[sg_c_or_f]);
  gb2312_2_utf16le(asc_str, strlen(asc_str) + 1, (char*)uni_str, sizeof(uni_str)-1);
  text_set_content_by_unistr(p_temp_ctrl, uni_str);

  ctrl_paint_ctrl(p_detail_cont, TRUE);
  return SUCCESS;
}
static RET_CODE on_weather_delete_city(control_t *ctrl, u16 msg,
                                      u32 para1, u32 para2)
{
  u16 focus;
  control_t  *p_list;
  dlg_ret_t dlg_ret;
  u16 city_name[32]={0};


  if(city_count > 0)
  {
	  dlg_ret = ui_comm_dlg_open2(&weather_delete_dlg_data);

  	if(dlg_ret == DLG_RET_YES)
  	{
      focus = list_get_focus_pos(ctrl);
      sys_status_get_ip_path_set(focus, city_name);
      do_delete_city(focus, (u16 *)(city_name));

      p_list = ui_comm_root_get_ctrl(ROOT_ID_WEATHER_FORECAST, IDC_AREA_LIST);
      MT_ASSERT(p_list != NULL);
      list_set_count(p_list, city_count, AREA_LIST_PAGE);
      if(focus < city_count)
      {
       list_set_focus_pos(p_list, focus);
      }
      else
      {
       list_set_focus_pos(p_list, city_count - 1);
      }
      city_list_update(p_list, list_get_valid_pos(p_list), AREA_LIST_PAGE, 0);
      ctrl_paint_ctrl(p_list, TRUE);
   	}
  }

  return SUCCESS;
}

static RET_CODE on_weather_exit_all(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  mul_weather_unregister_event();
  mul_weather_stop();
  mul_weather_destroy_chn();
  mul_weather_deinit();
  ui_close_all_mennus();
  return SUCCESS;
}

BEGIN_KEYMAP(weather_area_select_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_RED, MSG_ADD_CITY)
  ON_EVENT(V_KEY_GREEN, MSG_EDIT_CITY)
  ON_EVENT(V_KEY_YELLOW, MSG_DELETE_CITY)
  ON_EVENT(V_KEY_BLUE, MSG_SWITCH_UNIT)
END_KEYMAP(weather_area_select_cont_keymap, NULL)

BEGIN_MSGPROC(weather_area_select_cont_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_weather_area_select)
  ON_COMMAND(MSG_ADD_CITY, on_weather_add_city)
  ON_COMMAND(MSG_EDIT_CITY, on_weather_edit_city)
  ON_COMMAND(MSG_DELETE_CITY, on_weather_delete_city)
  ON_COMMAND(MSG_SWITCH_UNIT, on_weather_switch_unit)
  ON_COMMAND(MSG_EDIT_CITY_CHECK, on_weather_name_check)
  ON_COMMAND(MSG_EDIT_CITY_UPDATE, on_weather_name_update)
END_MSGPROC(weather_area_select_cont_proc, list_class_proc)

BEGIN_KEYMAP(weather_cont_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(weather_cont_keymap, NULL)

BEGIN_MSGPROC(weather_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_weather_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_weather_exit_all)
  ON_COMMAND(MSG_WEATHER_EVT_DATA_UPDATED, on_weather_update_data)
  ON_COMMAND(MSG_WEATHER_EVT_PARSE_ERROR, on_weather_parse_error)
  ON_COMMAND(MSG_WEATHER_EVT_CITY_ERROR, on_weather_parse_error)
  ON_COMMAND(MSG_WEATHER_EVT_DATA_ERROR, on_weather_parse_error)
END_MSGPROC(weather_cont_proc, ui_comm_root_proc)
