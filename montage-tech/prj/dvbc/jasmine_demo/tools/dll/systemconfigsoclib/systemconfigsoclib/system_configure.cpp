/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "stdafx.h"
#include <Izard/SystemConfigure/system_configure_native_interface.h>
#include <windows.h>
#include <stdarg.h>
#include <wchar.h>
#include "FlashParser.h"
///////////////////////////////////////////////////////////////////////////////
//uncomment following clause to enable log print
///////////////////////////////////////////////////////////////////////////////
#define PRINT_LOG_ENABLED

#define revert_endian_16(x)  (((x&0xff)<<8)|(x>>8))
#define revert_endian_32(x)  (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))
static bool g_bBigEdian = FALSE;

#define SYS_CONFIG_SW_VERSION_KEY                        L"Software version"
#define SYS_CONFIG_MAIN_FREQ_KEY                         L"Main TP(MHz)"
#define SYS_CONFIG_MAIN_FREQ_SYMBOL_RATE_KEY             L"Symbol Rate"
#define SYS_CONFIG_MAIN_FREQ_DEMODULATE_KEY              L"Demudulate"
#define SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY                L"BackupTP1(MHz)"
#define SYS_CONFIG_BACKUP_MAIN_FREQ_1_SYMBOL_RATE_KEY    L"BackupTP1 Symbol Rate"
#define SYS_CONFIG_BACKUP_MAIN_FREQ_1_DEMODULATE_KEY     L"BackupTP1 Demudulate"
#define SYS_CONFIG_UPGRADE_FREQ_KEY                      L"OTA TP(MHz)"
#define SYS_CONFIG_UPGRADE_FREQ_SYMBOL_RATE_KEY          L"OTA TP Symbol Rate"
#define SYS_CONFIG_UPGRADE_FREQ_DEMODULATE_KEY           L"OTA TP Demudulate"
#define SYS_CONFIG_AD_FREQ_KEY                           L"AD TP(MHz)"
#define SYS_CONFIG_AD_FREQ_SYMBOL_RATE_KEY               L"AD TP Symbol Rate"
#define SYS_CONFIG_AD_FREQ_DEMODULATE_KEY                L"AD TP Demudulate"
#define SYS_CONFIG_NVOD_FREQ_KEY                         L"NVOD TP(MHz)"
#define SYS_CONFIG_NVOD_FREQ_SYMBOL_RATE_KEY             L"NVOD TP Symbol Rate"
#define SYS_CONFIG_NVOD_FREQ_DEMODULATE_KEY              L"NVOD TP Demudulate"
#define SYS_CONFIG_PRINT_FLAG_KEY                        L"Print Info"
#define SYS_CONFIG_LANGUAGE_SET_KEY                      L"Language Set"
#define SYS_CONFIG_VIDEO_OUTPUT_KEY                      L"AV output"
#define SYS_CONFIG_VOLUME_MODE_KEY                       L"Audio Mode"
#define SYS_CONFIG_GLOBAL_VOLUME_KEY                     L"Globla volume"
#define SYS_CONFIG_TRACK_MODE                            L"Track Mode"
#define SYS_CONFIG_AUDIO_TRACK_KEY                       L"Audio track"
#define SYS_CONFIG_WATERMARK_ENABLE_KEY                  L"Enable WaterMark"
#define SYS_CONFIG_WATERMARK_X_KEY                       L"Watermark position X"
#define SYS_CONFIG_WATERMARK_Y_KEY                       L"Watermark position Y"
#define SYS_CONFIG_TV_SYSTEM_MODE                        L"Tv System Mode"
#define SYS_CONFIG_TV_RESOLUTION                         L"Tv Resolution"
#define SYS_CONFIG_TV_RATIO                              L"Tv Ratio"
#define SYS_CONFIG_VIDEO_EFFECTS                         L"Video Effects"
#define SYS_CONFIG_AUDIO_OUTPUT                          L"Audio Output"
#define SYS_CONFIG_SET_TIMEOUT                           L"Set Timeout"
#define SYS_CONFIG_SET_TRANSPARENT                       L"Set Transparent"
#define SYS_CONFIG_SET_SWITCH_MODE                       L"Set SWITCH MODE"
#ifdef ENABLE_NETWORK
#define SYS_CONFIG_SET_NET_MAC                         L"Net Mac"
#endif
#define SYS_CONFIG_GMT_USAGE                             L"GMT Usage"
#define SYS_CONFIG_GMT_OFFSET                            L"GMT Offset"
#define SYS_CONFIG_SUMMER_TIME                           L"Summer Time"
#define SYS_CONFIG_DATE                                  L"Date"
#define SYS_CONFIG_TIME                                  L"Time"
#define SYS_CONFIG_VIDEO_QUALITY                         L"Video quality"
#define SYS_CONFIG_OTA_OUI                               L"OUI"
#define SYS_CONFIG_OTA_HW_MOD_ID_KEY                     L"HW MOD ID"
#define SYS_CONFIG_OTA_SW_MOD_ID_KEY                     L"SW MOD ID"
#define SYS_CONFIG_OTA_HW_VERSION_KEY                    L"OTA HW version"
#define SYS_CONFIG_OTA_SW_VERSION_KEY                    L"OTA SW version"
#define DATE_SEPARATER                                                   "-"

/*!
  Key map
  */
typedef struct
{
  /*!
    Hot key
    */
  u8 h_key;
  /*!
    Virtual key
    */
  wchar_t v_key[32];
} key_map_config_t;

static key_map_config_t ir_keymap1[] =
{
	{0x0a, L"Usercode High"},          //p
	{0x19, L"Usercode Low"},           //j
	{0x0a, L"V_KEY_POWER"},          //p
	{0x19, L"V_KEY_MUTE"},           //j
	{0x12, L"V_KEY_RECALL"},         //h
	{0x00, L"V_KEY_0"},              //0
	{0x01, L"V_KEY_1"},              //1
	{0x02, L"V_KEY_2"},              //2
	{0x03, L"V_KEY_3"},              //3
	{0x04, L"V_KEY_4"},              //4
	{0x05, L"V_KEY_5"},              //5
	{0x06, L"V_KEY_6"},              //6
	{0x07, L"V_KEY_7"},              //7
	{0x08, L"V_KEY_8"},              //8
	{0x09, L"V_KEY_9"},              //9
	{0x14, L"V_KEY_FAV"},            //f
	{0x1C, L"V_KEY_TVRADIO"},        //t
	{0x0B, L"V_KEY_UP"},             //direction key up
	{0x11, L"V_KEY_LEFT"},           //direction key left
	{0x0D, L"V_KEY_OK"},             //enter
	{0x10, L"V_KEY_RIGHT"},          //direction key right
	{0x0E, L"V_KEY_DOWN"},           //direction key down
	{0x16, L"V_KEY_MENU"},           //m
	{0x1F, L"V_KEY_CANCEL"},         //c
	{0x1E, L"V_KEY_PAGE_UP"},        //Page up
	{0x13, L"V_KEY_PAGE_DOWN"},      //Page down
	{0x40, L"V_KEY_RED"},            //'
	{0x42, L"V_KEY_GREEN"},          ///
	{0x41, L"V_KEY_BLUE"},           //.
	{0x43, L"V_KEY_YELLOW"},         //,};
	{0x44, L"V_KEY_TTX"},
	{0x17, L"V_KEY_EPG"},
	{0x1D, L"V_KEY_AUDIO"},
	{0x1B, L"V_KEY_F1"},
	{0x45, L"V_KEY_F2"},
	{0x46, L"V_KEY_F3"},
	{0x47, L"V_KEY_F4"},
	{0x48, L"V_KEY_INFO"},
	{0X4D, L"V_KEY_REC"},//recommend key
	{0x1A, L"V_KEY_LANG"},//language switch
	{0x06, L"V_KEY_VUP"},//volume up
	{0x0B, L"V_KEY_VDOWN"},//volume down
	{0x1D, L"V_KEY_EXIT"},//exit
	{0X1C, L"V_KEY_BACK"},
	{0x2B, L"V_KEY_GAME"},//game
	{0x2E, L"V_KEY_TV_PLAYBACK"},//playback
	{0x3B, L"V_KEY_SEARCH"},  //search
	{0x2F, L"V_KEY_MAIL"},      //mail
	{0xFF, L"V_KEY_DATA_BROADCAST"},             //DATA BOARDCAST
	{0xFF, L"V_KEY_NVOD"},           //NVOD
	{0xFF, L"V_KEY_FAVUP"},             //FAVUP
	{0xFF, L"V_KEY_FAVDOWN"},            //FAVDOWN
	{0xFF, L"V_KEY_STOCK"},            //stock
	{0xFF, L"V_KEY_BOOK"},            //book
	{0xFF, L"V_KEY_RADIO"},            //radio
	{0xFF, L"V_KEY_TV"},            //tv
	{0xFF, L"V_KEY_F5"},               //F5
	{0xFF, L"V_KEY_PAUSE"},               //pause
	{0xFF, L"V_KEY_CHUP"},            //channel up
	{0xFF, L"V_KEY_CHDOWN"},            //channel down
	{0xFF, L"V_KEY_INPUT"},            //input method
	{0xFF, L"V_KEY_HELP"},               //help
	{0xFF, L"V_KEY_SET"},               //set
	{0x55, L"V_KEY_SUBT"},
	{0x57, L"V_KEY_UNKNOW"},         //media
	{0x03, L"V_KEY_GOTO"},
	{0x1c, L"V_KEY_ZOOM"},
	{0x0c, L"V_KEY_VIDEO_MODE"},
	{0x1f, L"V_KEY_ASPECT_MODE"},
	{0xFF, L"V_KEY_CHANNEL_LIST"},       //
	{0xFF, L"V_KEY_STOP"},
	{0xFF, L"V_KEY_PREV"},
	{0xFF, L"V_KEY_NEXT"},
	{0xFF, L"V_KEY_BACK2"},
	{0xFF, L"V_KEY_FORW2"},
	{0xFF, L"V_KEY_LIST"},
	{0xFF,L" V_KEY_PLAY"},       
};

/*
typedef struct
{
  u32 oui;
  u16 manufacture_id;
  u16 hw_mod_id;
  u16 sw_mod_id;
  u16 hw_version;
  u16 sw_version;
  u16 reserved1;
  u32 reserved;
}ota_tdi_tmp_t;

typedef struct 
{
  char debug_info;
  char sig_str_ratio;
  char sig_str_max;
  char sig_qua_max;
  char str_l_qua;
  unsigned int str_0_gain;
  char bs_times;
  char spi_clk;
  char rd_mode;
  char standby_mode;
  char watchdog_mode;
  char str_poweron[10];
  char str_standby[10];
  char str_sw_version[20];
  char str_hw_version[20];
  char str_last_upgrade_date[20];
  ota_tdi_tmp_t ota_tdi;
}misc_options_t;
*/
static sys_status_t g_status;
static misc_options_t g_misc_options;

FlashParser g_FlashParser;
wchar_t g_LastErrorMsg[256];

static int ret=0;

#ifdef PRINT_LOG_ENABLED
void LOGMSG(const char *fmt, ... )
{
	if (fmt==NULL || strlen(fmt)==0)
		return;
	
	char *msg = new char[1024];
	int   len = 0;

	SYSTEMTIME st;
	GetLocalTime(&st);
	len = sprintf(msg, "%04d-%02d-%02d %02d:%02d:%02d.%03d, ",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);

	va_list ap;
	va_start(ap, fmt);
	vsprintf(msg+len, fmt, ap);
	va_end(ap);

	FILE *fp;
  fp = fopen("sys_config.log", "a");
  fprintf(fp, "%s\n", msg);
  fclose(fp);
	delete [] msg;
}
#else
#define LOGMSG
#endif

int CheckSWVersion(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_SW_VERSION_KEY) != 0)
  {
    LOGMSG("INVLAID SW VERSION KEY");
    wcscpy(g_LastErrorMsg, L"INVLAID SW VERSION KEY");
    return 0;
  }
  if (property->value[0] != L'0' || (property->value[1] != L'x' && property->value[1] != L'X') || wcslen(property->value) != 10)
  {
    LOGMSG("[%s, %d] INVALID SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"SW VERSION Must be lowcase hex value prefixed with 0x");
    return 0;
  }
  for(u32 i=0; i<8; i++)
  {
    if (!iswxdigit(property->value[i+2]))
    {
      LOGMSG("[%s, %d] INVALID SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
      wcscpy(g_LastErrorMsg, L"INVLAID SW VERSION Must be lowcase hex value prefixed with 0x");
      return 0;
    }
  }
  LOGMSG("[%s, %d] GOOD SW VERSION VALUE: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int isdigitnum(wchar_t *str)
{
  size_t size = wcslen(str);
  for(size_t i=0; i<size; i++)
  {
    if(!iswdigit(str[i]))
    {
      return 0;
    }
  }
  return 1;
}

int CheckFreqency(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];
  int nValue;

  if (wcscmp(property->key, SYS_CONFIG_MAIN_FREQ_KEY) != 0 
    && wcscmp(property->key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_UPGRADE_FREQ_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_AD_FREQ_KEY) != 0
    && wcscmp(property->key, SYS_CONFIG_NVOD_FREQ_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID FREQUENCY KEY");
    return 0;
  }  
  wcstombs(szValue, property->value, MIDDLELENGTH);

  nValue = atoi(szValue);
  if (nValue<45 || nValue>999)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"Frequency must be between 45 and 999");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckSymbolRate(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
	char szValue[MIDDLELENGTH];
	int nValue;

	wcstombs(szValue, property->value, MIDDLELENGTH);

	if (wcscmp(property->key, SYS_CONFIG_MAIN_FREQ_SYMBOL_RATE_KEY) != 0 
		&& wcscmp(property->key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_SYMBOL_RATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_UPGRADE_FREQ_SYMBOL_RATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_AD_FREQ_SYMBOL_RATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_NVOD_FREQ_SYMBOL_RATE_KEY) != 0)
	{
		LOGMSG("[%s, %d] INVALID SYMBOLRATE KEY", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID SYMBOLRATE KEY");
		return 0;
	}

	if (!isdigitnum(property->value))
	{
		LOGMSG("[%s, %d] INVALID SYMBOLRATE value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"Symbol rate must be between 0 and 9999");
		return 0;
	}

	nValue = atoi(szValue);
	if (nValue<0 || nValue>9999)
	{
		LOGMSG("[%s, %d] INVALID SYMBOLRATE value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"Symbol rate must be between 0 and 9999");
		return 0;
	}
	LOGMSG("[%s, %d] GOOD SYMBOLRATE value: %s", __FUNCTION__, __LINE__, szValue);
	return 1;
}

int CheckDemodulate(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
	if (wcscmp(property->key, SYS_CONFIG_MAIN_FREQ_DEMODULATE_KEY) != 0 
		&& wcscmp(property->key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_DEMODULATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_UPGRADE_FREQ_DEMODULATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_AD_FREQ_DEMODULATE_KEY) != 0
		&& wcscmp(property->key, SYS_CONFIG_NVOD_FREQ_DEMODULATE_KEY) != 0)
	{
		LOGMSG("[%s, %d] INVALID LANGUAGE SET KEY", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID LANGUAGE SET KEY");
		return 0;
	}

	if (wcscmp(property->value, L"AUTO") != 0 
		&& wcscmp(property->value, L"BPSK") != 0
		&& wcscmp(property->value, L"QPSK") != 0
		&& wcscmp(property->value, L"8PSK") != 0
		&& wcscmp(property->value, L"QAM16") != 0
		&& wcscmp(property->value, L"QAM32") != 0
		&& wcscmp(property->value, L"QAM64") != 0
		&& wcscmp(property->value, L"QAM128") != 0
		&& wcscmp(property->value, L"QAM256") != 0)
	{
		LOGMSG("[%s, %d] INVALID DEMODULATE SET VALUE", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID DEMODULATE SET VALUE");
		return 0;
	}
	LOGMSG("[%s, %d] GOOD DEMODULATE SET VALUEs", __FUNCTION__, __LINE__);
	return 1;
}

int CheckPrintFlag(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_PRINT_FLAG_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID PRINT FLAG KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID PRINT FLAG KEY");
    return 0;
  }

  if (wcscmp(property->value, L"close") != 0 
    && wcscmp(property->value, L"open") != 0)
  {
    LOGMSG("[%s, %d] INVALID PRINT FLAG VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID PRINT FLAG VALUE, must be close or open");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD PRINT FLAG VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckLanguage(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_LANGUAGE_SET_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID LANGUAGE SET KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID LANGUAGE SET KEY");
    return 0;
  }

  if (wcscmp(property->value, L"English") != 0 
    && wcscmp(property->value, L"简体中文") != 0)
  {
    LOGMSG("[%s, %d] INVALID LANGUAGE SET VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID LANGUAGE SET VALUE");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD LANGUAGE SET VALUEs", __FUNCTION__, __LINE__);
  return 1;
}
int CheckVideoOutput(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_VIDEO_OUTPUT_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID VIDEO OUTPUT KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID VIDEO OUTPUT KEY");
    return 0;
  }

  if (wcscmp(property->value, L"CVBS") != 0 
    && wcscmp(property->value, L"SVDIEO") != 0
    && wcscmp(property->value, L"RGB") != 0
    && wcscmp(property->value, L"YUV") != 0
    && wcscmp(property->value, L"CVBS_YUV") != 0
    && wcscmp(property->value, L"CVBS_SVDIEO") != 0
    && wcscmp(property->value, L"CVBS_CVBS") != 0)
  {
    LOGMSG("[%s, %d] INVALID VIDEO OUTPUT VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID VIDEO OUTPUT VALUE");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD VIDEO OUTPUT VALUEs", __FUNCTION__, __LINE__);
  return 1;
}
int CheckVolumeMode(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_VOLUME_MODE_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID VOLUME MODE KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID VOLUME MODE KEY");
    return 0;
  }

  if (wcscmp(property->value, L"Single volume") != 0 
    && wcscmp(property->value, L"Global volume") != 0)
  {
    LOGMSG("[%s, %d] INVALID VOLUME MODE VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID VOLUME MODE VALUE");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD VOLUME MODE VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckGlobalVolume(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];
  int nValue;
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_GLOBAL_VOLUME_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID GLOBAL VOLUME KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID GLOBAL VOLUME KEY");
    return 0;
  }
  
  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID GLOBAL VOLUME value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"GLOBAL VOLUME value must be between 0 and 32");
    return 0;
  }

  nValue = atoi(szValue);
  if (nValue<0 || nValue>32)
  {
    LOGMSG("[%s, %d] INVALID FREQUENCY value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"GLOBAL VOLUME value must be between 0 and 32");
    return 0;
  }

  LOGMSG("[%s, %d] GOOD GLOBAL VOLUME value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckTrackMode(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_TRACK_MODE) != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK MODE KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID TRACK MODE KEY");
    return 0;
  }

  if (wcscmp(property->value, L"Single Track") != 0 
    && wcscmp(property->value, L"Global Track") != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK MODE VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID TRACK MODE VALUE");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD TRACK MODE VALUEs", __FUNCTION__, __LINE__);
  return 1;
}

int CheckTrack(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_AUDIO_TRACK_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID AUDIO TRACK KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID AUDIO TRACK KEY");
    return 0;
  }

  if (wcscmp(property->value, L"Stereo") != 0 
    && wcscmp(property->value, L"Left") != 0
    && wcscmp(property->value, L"Right") != 0)
  {
    LOGMSG("[%s, %d] INVALID TRACK VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID TRACK VALUE");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD AUDIO TRACK VALUE", __FUNCTION__, __LINE__);
  return 1;
}

int CheckEnableWaterMark(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
	if (wcscmp(property->key, SYS_CONFIG_WATERMARK_ENABLE_KEY) != 0)
	{
		LOGMSG("[%s, %d] INVALID WaterMark MODE KEY", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID WaterMark MODE KEY");
		return 0;
	}

	if (wcscmp(property->value, L"Enable Watermark") != 0 
		&& wcscmp(property->value, L"Disable Watermark") != 0)
	{
		LOGMSG("[%s, %d] INVALID WaterMark MODE VALUE", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID WaterMark MODE VALUE");
		return 0;
	}
	LOGMSG("[%s, %d] GOOD WaterMark MODE VALUEs", __FUNCTION__, __LINE__);
	return 1;
}

int CheckWatermarkPosX(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
	char szValue[MIDDLELENGTH];
	int nValue;

	wcstombs(szValue, property->value, MIDDLELENGTH);

	if (wcscmp(property->key, SYS_CONFIG_WATERMARK_X_KEY) != 0)
	{
		LOGMSG("[%s, %d] INVALID WaterMark KEY", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID WaterMark KEY");
		return 0;
	}

	if (!isdigitnum(property->value))
	{
		LOGMSG("[%s, %d] INVALID WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"WaterMark position_X must be between 0 and 1920");
		return 0;
	}

	nValue = atoi(szValue);
	if (nValue<0 || nValue>1920)
	{
		LOGMSG("[%s, %d] INVALID WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"WaterMark position_X must be between 0 and 1920");
		return 0;
	}
	LOGMSG("[%s, %d] GOOD WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
	return 1;
}

int CheckWatermarkPosY(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
	char szValue[MIDDLELENGTH];
	int nValue;

	wcstombs(szValue, property->value, MIDDLELENGTH);

	if (wcscmp(property->key, SYS_CONFIG_WATERMARK_Y_KEY) != 0)
	{
		LOGMSG("[%s, %d] INVALID WaterMark KEY", __FUNCTION__, __LINE__);
		wcscpy(g_LastErrorMsg, L"INVALID WaterMark KEY");
		return 0;
	}

	if (!isdigitnum(property->value))
	{
		LOGMSG("[%s, %d] INVALID WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"WaterMark position_y must be between 0 and 1080");
		return 0;
	}

	nValue = atoi(szValue);
	if (nValue<0 || nValue>1080)
	{
		LOGMSG("[%s, %d] INVALID WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
		wcscpy(g_LastErrorMsg, L"WaterMark position_y must be between 0 and 1920");
		return 0;
	}
	LOGMSG("[%s, %d] GOOD WaterMark value: %s", __FUNCTION__, __LINE__, szValue);
	return 1;
}
int CheckOui(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];
  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_OTA_OUI) != 0)
  {
    LOGMSG("INVLAID OTA OUI KEY");
    wcscpy(g_LastErrorMsg, L"INVLAID OTA OUI KEY");
    return 0;
  }
  if (property->value[0] != L'0' || (property->value[1] != L'x' && property->value[1] != L'X'))
  {
    LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"OTA OUI Must be lowcase hex value prefixed with 0x");
    return 0;
  }
  for(u32 i=0; i<4; i++)
  {
    if (!iswxdigit(property->value[i+2]))
    {
      LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
      wcscpy(g_LastErrorMsg, L"OTA OUI Must be lowcase hex value prefixed with 0x");
      return 0;
    }
  }
  LOGMSG("[%s, %d] GOOD OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}
int CheckIRkey(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];
  u8 i = 0;
  BOOL b_find_key = FALSE;

  wcstombs(szValue, property->value, MIDDLELENGTH);

  for(i = 0; i < sizeof(ir_keymap1) / sizeof(key_map_config_t); i++)
  {
    if (wcscmp(property->key, ir_keymap1[i].v_key) == 0)
	{
     b_find_key = TRUE;
	 break;
	}
  }
  if (b_find_key == FALSE)
  {
    LOGMSG("INVLAID IR KEY");
    wcscpy(g_LastErrorMsg, L"INVLAID IR KEY");
    return 0;
  }

  if (property->value[0] != L'0' || (property->value[1] != L'x' && property->value[1] != L'X') || wcslen(property->value) != 4)
  {
    LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"IR Key Must be lowcase hex value prefixed with 0x");
    return 0;
  }

  for(u32 i=0; i<2; i++)
  {
    if (!iswxdigit(property->value[i+2]))
    {
      LOGMSG("[%s, %d] INVALID OTA OUI VALUE: %s", __FUNCTION__, __LINE__, szValue);
      wcscpy(g_LastErrorMsg, L"IR Key Must be lowcase hex value prefixed with 0x");
      return 0;
    }
  }

  LOGMSG("[%s, %d] GOOD IR KEY VALUE: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}
int CheckID(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  char szValue[MIDDLELENGTH];

  wcstombs(szValue, property->value, MIDDLELENGTH);

  if (wcscmp(property->key, SYS_CONFIG_OTA_HW_MOD_ID_KEY) != 0
   && wcscmp(property->key, SYS_CONFIG_OTA_SW_MOD_ID_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA MOD ID KEY", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA MOD ID KE must be decimal value");
    return 0;
  }
  
  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA MOD ID value: %s", __FUNCTION__, __LINE__, szValue);
    wcscpy(g_LastErrorMsg, L"INVALID OTA MOD ID");
    return 0;
  }

  LOGMSG("[%s, %d] GOOD OTA MOD ID value: %s", __FUNCTION__, __LINE__, szValue);
  return 1;
}

int CheckOTAHWVersion(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_OTA_HW_VERSION_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA HW VERSION KEY VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID OTA HW VERSION KEY VALUE");
    return 0;
  }

  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA HW VERSION VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA HW VERSION VALUE must be decimal value");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD OTA HW VERSION VALUE", __FUNCTION__, __LINE__);
  return 1;
}

int CheckOTASWVersion(struct NativeProperty* property, struct NativePropertyArray* /*all_propertys*/)
{
  if (wcscmp(property->key, SYS_CONFIG_OTA_SW_VERSION_KEY) != 0)
  {
    LOGMSG("[%s, %d] INVALID OTA SW VERSION KEY VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"INVALID OTA SW VERSION KEY VALUE");
    return 0;
  }

  if (!isdigitnum(property->value))
  {
    LOGMSG("[%s, %d] INVALID OTA SW VERSION VALUE", __FUNCTION__, __LINE__);
    wcscpy(g_LastErrorMsg, L"OTA SW VERSION VALUE must be decimal value");
    return 0;
  }
  LOGMSG("[%s, %d] GOOD OTA SW VERSION VALUE", __FUNCTION__, __LINE__);
  return 1;
}
int ProjectInitial(wchar_t* /*initial_parameter*/)
{
  //DO NOTHING HERE
  return 0;
}

void SSData_endian_convert()
{
  g_status.sw_version = revert_endian_32(g_status.sw_version);
  g_status.upgrade_tp.tp_freq = revert_endian_32(g_status.upgrade_tp.tp_freq);
  g_status.main_tp1.tp_freq = revert_endian_32(g_status.main_tp1.tp_freq);
  g_status.main_tp2.tp_freq = revert_endian_32(g_status.main_tp2.tp_freq);
  g_status.nvod_tp.tp_freq = revert_endian_32(g_status.nvod_tp.tp_freq);
  g_status.ad_tp.tp_freq = revert_endian_32(g_status.ad_tp.tp_freq);
}
int Load(wchar_t* file_path, struct NativePropertyArray* all_propertys)
{
//  Clean(all_propertys);
  u32 size = 0;
  u8 asc_buf[32];
  u8 cfg_tmp[128] = {0};
  char sztmp[32]={0};
  u8 i = 0;
  int iLength = 0;
  char temp_mac[32] = {0};
  u16 index;
  char mac_map[3] = {0};
  wchar_t mac[32]={'\0'};
  
  ret = 1;
  g_FlashParser.Clear();
  if(g_FlashParser.LoadBinFile(file_path) == FALSE)
  {
    return FALSE;
  }
  //Load SSData
  if (g_FlashParser.GetSSData((char *)&g_status, sizeof(g_status)) == FALSE)
  {
    return FALSE;
  }
  else if(g_bBigEdian == TRUE)
  {
    SSData_endian_convert();
  }
  //Load MiscOptions
  if (g_FlashParser.GetMiscOptions((char *)&g_misc_options, sizeof(g_misc_options)) == FALSE)
  {
    return FALSE;
  }

  if (g_FlashParser.GetIRData((char *)cfg_tmp, sizeof(ir_keymap1) / sizeof(key_map_config_t)) == FALSE)
  {
    return FALSE;
  }
  else
  {
    for(i = 0; i < sizeof(ir_keymap1) / sizeof(key_map_config_t); i++)
    {
      ir_keymap1[i].h_key = cfg_tmp[i];
    }
  }
  //arrange properties
  //all_propertys->size = 10;
  all_propertys->first = new NativeProperty[255];
  memset(all_propertys->first, 0, sizeof(NativeProperty)*255);

  //-------------------------
  // Basic Settings
  //-------------------------
  //sw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0x%08x", g_status.sw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
  //main freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_MAIN_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp1.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_MAIN_FREQ_SYMBOL_RATE_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp1.tp_sym);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_MAIN_FREQ_DEMODULATE_KEY);
  switch(g_status.main_tp1.nim_modulate)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BPSK");
	  break;
  case 2:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QPSK");
	  break;
  case 3:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8PSK");
	  break;
  case 4:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM16");
	  break;
  case 5:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM32");
	  break;
  case 6:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  case 7:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM128");
	  break;
  case 8:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM256");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //main freq backup 1
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp2.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_BACKUP_MAIN_FREQ_1_SYMBOL_RATE_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.main_tp2.tp_sym);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_BACKUP_MAIN_FREQ_1_DEMODULATE_KEY);
  switch(g_status.main_tp2.nim_modulate)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BPSK");
	  break;
  case 2:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QPSK");
	  break;
  case 3:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8PSK");
	  break;
  case 4:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM16");
	  break;
  case 5:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM32");
	  break;
  case 6:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  case 7:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM128");
	  break;
  case 8:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM256");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //upgrade freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_UPGRADE_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.upgrade_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_UPGRADE_FREQ_SYMBOL_RATE_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.upgrade_tp.tp_sym);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_UPGRADE_FREQ_DEMODULATE_KEY);
  switch(g_status.upgrade_tp.nim_modulate)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BPSK");
	  break;
  case 2:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QPSK");
	  break;
  case 3:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8PSK");
	  break;
  case 4:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM16");
	  break;
  case 5:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM32");
	  break;
  case 6:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  case 7:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM128");
	  break;
  case 8:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM256");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //ad freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AD_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.ad_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AD_FREQ_SYMBOL_RATE_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.ad_tp.tp_sym);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AD_FREQ_DEMODULATE_KEY);
  switch(g_status.ad_tp.nim_modulate)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BPSK");
	  break;
  case 2:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QPSK");
	  break;
  case 3:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8PSK");
	  break;
  case 4:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM16");
	  break;
  case 5:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM32");
	  break;
  case 6:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  case 7:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM128");
	  break;
  case 8:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM256");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  //nvod freq
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_NVOD_FREQ_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.nvod_tp.tp_freq/1000);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_NVOD_FREQ_SYMBOL_RATE_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.nvod_tp.tp_sym);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_NVOD_FREQ_DEMODULATE_KEY);
  switch(g_status.nvod_tp.nim_modulate)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BPSK");
	  break;
  case 2:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QPSK");
	  break;
  case 3:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8PSK");
	  break;
  case 4:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM16");
	  break;
  case 5:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM32");
	  break;
  case 6:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  case 7:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM128");
	  break;
  case 8:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM256");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"QAM64");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Auto;BPSK;QPSK;8PSK;QAM16;QAM32;QAM64;QAM128;QAM256;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
#if 0
  //print flag
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_PRINT_FLAG_KEY);
  switch(g_status.printf_switch_set.printf_flag)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"关闭");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"打开");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"关闭");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"关闭;打开;");
  all_propertys->first[size].m_validation = NULL;
  size++;
#endif
  //language set
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_LANGUAGE_SET_KEY);
  switch(g_status.lang_set.osd_text)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"English");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"简体中文");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"English");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"English;简体中文;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"English;简体中文;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  //video output
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VIDEO_OUTPUT_KEY);
  switch(g_status.av_set.video_output)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"SVDIEO");
    break;
  case 2:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"RGB");
    break;
  case 3:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"YUV");
    break;
  case 4:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_YUV");
    break;
  case 5:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_SVDIEO");
    break;
  case 6:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_CVBS");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"CVBS_YUV");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"CVBS;SVDIEO;RGB;YUV;CVBS_YUV;CVBS_SVDIEO;CVBS_CVBS;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"CVBS;SVDIEO;RGB;YUV;CVBS_YUV;CVBS_SVDIEO;CVBS_CVBS;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //volume mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VOLUME_MODE_KEY);
  switch(g_status.audio_set.is_global_volume)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Single volume");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Global volume");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Single volume");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Single volume;Global volume;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Single volume;Global volume;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //global volume
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_GLOBAL_VOLUME_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d",g_status.audio_set.global_volume);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
  //track mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TRACK_MODE);
  switch(g_status.audio_set.is_global_track)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Single Track");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Global Track");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Single Track");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Single Track;Global Track;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Single Track;Global Track;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
  
  //audio track
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AUDIO_TRACK_KEY);
  switch(g_status.audio_set.global_track)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Stereo");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Left");
    break;
  case 2:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Right");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Stereo");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"Stereo;Left;Right;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Stereo;Left;Right;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  all_propertys->first[size].m_validation = NULL;
  size++;
#if 0
  //watermark
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_WATERMARK_ENABLE_KEY);
  switch(g_status.b_WaterMark)
  {
  case 0:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Disable Watermark");
	  break;
  case 1:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Enable Watermark");
	  break;
  default:
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Disable Watermark");
	  break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  swprintf(all_propertys->first[size].options, LONGLENGTH-1, L"Enable Watermark;Disable Watermark;");
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_WATERMARK_X_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.x_WaterMark);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].m_validation = NULL;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_WATERMARK_Y_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_status.y_WaterMark);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].m_validation = NULL;
  size++;
#endif

  //tv system mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TV_SYSTEM_MODE);
  switch(g_status.av_set.tv_mode)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"PAL");
    break;
  case 1:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"NTSC");
    break;
  case 2:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
  iLength = wcslen(L"PAL;NTSC;Auto;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,L"PAL;NTSC;Auto;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //tv resolution
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TV_RESOLUTION);
  if(g_status.av_set.tv_mode == 0)
  {
	  switch(g_status.av_set.tv_resolution)
	  {
		  case 0:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"576i");
			break;
		  case 1:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"576p");
			break;
		  case 2:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"720p");
			break;
		  case 3:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080i");
			break;
		  case 4:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080p");
			break;
		  default:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"576i");
			break;
	  }
	  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
	  iLength = wcslen(L"576i;576p;720p;1080i;1080p;");
      all_propertys->first[size].options =  new wchar_t[iLength+1];
	  wcscpy_s(all_propertys->first[size].options, iLength+1,L"576i;576p;720p;1080i;1080p;");
	  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

	  size++;
  }
  else if(g_status.av_set.tv_mode == 1)
  {
	  switch(g_status.av_set.tv_resolution)
	  {
		  case 0:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480i");
			break;
		  case 1:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480p");
			break;
		  case 2:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"720p");
			break;
		  case 3:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080i");
			break;
		  case 4:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080p");
			break;
		  default:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480i");
			break;
	  }
	  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
	  iLength = wcslen(L"480i;480p;720p;1080i;1080p;");
      all_propertys->first[size].options =  new wchar_t[iLength+1];
	  wcscpy_s(all_propertys->first[size].options, iLength+1, L"480i;480p;720p;1080i;1080p;");
	  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

	  size++;
  }
  else if(g_status.av_set.tv_mode == 2)
  {
	  switch(g_status.av_set.tv_resolution)
	  {
		  case 0:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480i");
			break;
		  case 1:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480p");
			break;
		  case 2:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"720p");
			break;
		  case 3:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080i");
			break;
		  case 4:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"1080p");
			break;
		  default:
			swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"480i");
			break;
	  }
	  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
	  iLength = wcslen(L"480i;480p;576i;576p;720p;1080i;1080p;");
      all_propertys->first[size].options =  new wchar_t[iLength+1];
	  wcscpy_s(all_propertys->first[size].options, iLength+1,L"480i;480p;576i;576p;720p;1080i;1080p;");
	  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

	  size++;
  }

  //tv ratio
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TV_RATIO);
  switch(g_status.av_set.tv_ratio)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"4:3LetterBox");
		break;
	  case 2:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"4:3PanScan");
		break;
	  case 3:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"16:9");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Auto");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
	  iLength = wcslen(L"Auto;4:3LetterBox;4:3PanScan;16:9;");
      all_propertys->first[size].options =  new wchar_t[iLength+1];
	  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Auto;4:3LetterBox;4:3PanScan;16:9;");
	  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //video quality
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VIDEO_QUALITY);
  switch(g_status.video_quality)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Standard");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Default");
		break;
	  case 2:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Vivid");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Default");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  
	  iLength = wcslen(L"Standard;Default;Vivid;");
      all_propertys->first[size].options =  new wchar_t[iLength+1];
	  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Standard;Default;Vivid;");
	  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //video effects
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_VIDEO_EFFECTS);
  switch(g_status.av_set.video_effects)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Sharpening");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Adaptive");
		break;
	  case 2:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Soft");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Sharpening");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"Sharpening;Adaptive;Soft;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"Sharpening;Adaptive;Soft;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //audio output
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_AUDIO_OUTPUT);
  switch(g_status.av_set.digital_audio_output)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"LPCM Out");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"BS Out");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"LPCM Out");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"LPCM Out;BS Out;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1, L"LPCM Out;BS Out;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //display time
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SET_TIMEOUT);
  switch(g_status.osd_set.timeout)
  {
  case 3:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"3");
    break;
  case 4:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"4");
    break;
  case 5:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"5");
    break;
  case 6:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"6");
    break;
  case 7:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"7");
    break;
  case 8:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"8");
    break;
  case 9:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"9");
    break;
  case 10:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"10");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"3");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"3;4;5;6;7;8;9;10;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"3;4;5;6;7;8;9;10;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //transparent
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SET_TRANSPARENT);
  switch(g_status.osd_set.transparent)
  {
  case 0:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0");
    break;
  case 10:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"10");
    break;
  case 20:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"20");
    break;
  case 30:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"30");
    break;
  case 40:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"40");
    break;
  default:
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0");
    break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"0;10;20;30;40;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"0;10;20;30;40;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  //switch mode
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SET_SWITCH_MODE);
  switch(g_status.play_set.mode)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Video Freeze");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Black Screen");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Video Freeze");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"Video Freeze;Black Screen;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"Video Freeze;Black Screen;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_GMT_USAGE);
  switch(g_status.time_set.gmt_usage)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Off");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"On");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Off");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"Off;On;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"Off;On;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_GMT_OFFSET);

   if(g_status.time_set.gmt_offset >= 23)//+
   {
  	swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"UTC+%.2d:%.2d", (g_status.time_set.gmt_offset - 23)/2, (g_status.time_set.gmt_offset - 23)%2*30);
   }
   else
  {
  	swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"UTC-%.2d:%.2d", (23 - g_status.time_set.gmt_offset)/2, (23 - g_status.time_set.gmt_offset)%2*30);
  }
  
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"UTC-11:30;UTC-11:00;UTC-10:30;UTC-10:00;UTC-09:30;UTC-09:00;UTC-08:30;UTC-08:00;UTC-07:30;UTC-07:00;UTC-06:30;UTC-06:00;UTC-05:30;UTC-05:00;UTC-04:30;UTC-04:00;UTC-03:30;UTC-03:00;UTC-02:30;UTC-02:00;UTC-01:30;UTC-01:00;UTC-00:30;UTC+00:00;UTC+00:30;UTC+01:00;UTC+01:30;UTC+02:00;UTC+02:30;UTC+03:00;UTC+03:30;UTC+04:00;UTC+04:30;UTC+05:00;UTC+05:30;UTC+06:00;UTC+06:30;UTC+07:00;UTC+07:30;UTC+08:00;UTC+08:00;UTC+08:30;UTC+09:00;UTC+09:30;UTC+10:00;UTC+10:30;UTC+11:00;UTC+11:30;UTC+12:00;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"UTC-11:30;UTC-11:00;UTC-10:30;UTC-10:00;UTC-09:30;UTC-09:00;UTC-08:30;UTC-08:00;UTC-07:30;UTC-07:00;UTC-06:30;UTC-06:00;UTC-05:30;UTC-05:00;UTC-04:30;UTC-04:00;UTC-03:30;UTC-03:00;UTC-02:30;UTC-02:00;UTC-01:30;UTC-01:00;UTC-00:30;UTC+00:00;UTC+00:30;UTC+01:00;UTC+01:30;UTC+02:00;UTC+02:30;UTC+03:00;UTC+03:30;UTC+04:00;UTC+04:30;UTC+05:00;UTC+05:30;UTC+06:00;UTC+06:30;UTC+07:00;UTC+07:30;UTC+08:00;UTC+08:00;UTC+08:30;UTC+09:00;UTC+09:30;UTC+10:00;UTC+10:30;UTC+11:00;UTC+11:30;UTC+12:00;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SUMMER_TIME);
  switch(g_status.time_set.summer_time)
  {
	  case 0:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Off");
		break;
	  case 1:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"On");
		break;
	  default:
		swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"Off");
		break;
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");

  iLength = wcslen(L"Off;On;");
  all_propertys->first[size].options =  new wchar_t[iLength+1];
  wcscpy_s(all_propertys->first[size].options, iLength+1,  L"Off;On;");
  all_propertys->first[size].options_len = wcslen(all_propertys->first[size].options);

  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_DATE);
  if((g_status.time_set.sys_time.month >= 1) &&(g_status.time_set.sys_time.month <=9)
  	&&(g_status.time_set.sys_time.day >= 1) &&(g_status.time_set.sys_time.day <=9))
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d-%d%d-%d%d", g_status.time_set.sys_time.year,
	  	0,g_status.time_set.sys_time.month,0,g_status.time_set.sys_time.day);
  }
  else if((g_status.time_set.sys_time.month >= 1) &&(g_status.time_set.sys_time.month <=9)
  	&&(g_status.time_set.sys_time.day >= 10) )
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d-%d%d-%d", g_status.time_set.sys_time.year,
	  	0,g_status.time_set.sys_time.month,g_status.time_set.sys_time.day);
  }
  else if((g_status.time_set.sys_time.month >= 10) &&(g_status.time_set.sys_time.month <=12)
  	&&(g_status.time_set.sys_time.day >= 1) &&(g_status.time_set.sys_time.day <=9))
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d-%d-%d%d", g_status.time_set.sys_time.year,
	  	g_status.time_set.sys_time.month,0,g_status.time_set.sys_time.day);
  }
 else
  {
  	swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d-%d-%d", g_status.time_set.sys_time.year,g_status.time_set.sys_time.month,g_status.time_set.sys_time.day);
  }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  size++;

  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_TIME);
    if((g_status.time_set.sys_time.hour >= 0) &&(g_status.time_set.sys_time.hour <=9)
  	&&(g_status.time_set.sys_time.minute >= 0) &&(g_status.time_set.sys_time.minute <=9))
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d%d:%d%d", 0,g_status.time_set.sys_time.hour,
	  	0,g_status.time_set.sys_time.minute);
  }
  else if((g_status.time_set.sys_time.hour >= 0) &&(g_status.time_set.sys_time.hour <=9)
  	&&(g_status.time_set.sys_time.minute >= 10) )
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d%d:%d",0, g_status.time_set.sys_time.hour,
	  	g_status.time_set.sys_time.minute);
  }
  else if((g_status.time_set.sys_time.hour >= 10) &&(g_status.time_set.sys_time.hour <=24)
  	&&(g_status.time_set.sys_time.minute >= 0) &&(g_status.time_set.sys_time.minute <=9))
  {
	  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d:%d%d",g_status.time_set.sys_time.hour,
	  	0, g_status.time_set.sys_time.minute);
  }
 else 
 {
	 swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d:%d", g_status.time_set.sys_time.hour,g_status.time_set.sys_time.minute);
 }
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  size++;
 #ifdef ENABLE_NETWORK
  //mac
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_SET_NET_MAC);
   swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%c%c.%c%c.%c%c.%c%c.%c%c.%c%c", g_status.mac_set.mac_one[0],g_status.mac_set.mac_one[1],
   g_status.mac_set.mac_two[0],g_status.mac_set.mac_two[1],g_status.mac_set.mac_three[0],g_status.mac_set.mac_three[1],
   g_status.mac_set.mac_four[0],g_status.mac_set.mac_four[1],g_status.mac_set.mac_five[0],g_status.mac_set.mac_five[1],
   g_status.mac_set.mac_six[0],g_status.mac_set.mac_six[1]); 
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"Basic set");
  all_propertys->first[size].options_len= 0;
  size++;
#endif
  //-------------------------
  //OTA UPGRADE
  //-------------------------
  //ota oui
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_OUI);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0x%04x", g_misc_options.ota_tdi.oui);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA Parameters");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
  //hw mod id
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_HW_MOD_ID_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.hw_mod_id);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA Parameters");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
  //sw mod id
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_SW_MOD_ID_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.sw_mod_id);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA Parameters");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
  //hw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_HW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.hw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA Parameters");
  all_propertys->first[size].options_len= 0;
  all_propertys->first[size].m_validation = NULL;
  size++;
#if 0
  //sw version
  swprintf(all_propertys->first[size].key, SHORTLENGTH-1, SYS_CONFIG_OTA_SW_VERSION_KEY);
  swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"%d", g_misc_options.ota_tdi.sw_version);
  swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"OTA Parameters");
  all_propertys->first[size].m_validation = NULL;
  size++;
#endif
  //-------------------------
  //IR CONFIG
  //-------------------------

  for(i = 0; i < sizeof(ir_keymap1) / sizeof(key_map_config_t); i++)
  {
    swprintf(all_propertys->first[size].key, SHORTLENGTH-1,ir_keymap1[i].v_key);
    swprintf(all_propertys->first[size].value, MIDDLELENGTH-1, L"0x%02x", ir_keymap1[i].h_key);
    swprintf(all_propertys->first[size].group, SHORTLENGTH-1, L"IR Set");
    all_propertys->first[size].options_len= 0;
    all_propertys->first[size].m_validation = NULL;
    size++;
  }

  all_propertys->size = size;
  return TRUE;
}

int Save(wchar_t* file_path, struct NativePropertyArray* all_propertys)
{
  char value[MIDDLELENGTH];  //the property's value
  u8 cfg_tmp[128] = {0};
  char tmp_value[MIDDLELENGTH];
  u8 i = 0,num = 0,j=0,k=0;
  
  for(unsigned int size=0; size<all_propertys->size; size++)
  {
    //sw version
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SW_VERSION_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      sscanf(value, "0x%08x", &g_status.sw_version);
    }

    //main freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_MAIN_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.main_tp1.tp_freq = atoi(value)*1000;
    }

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_MAIN_FREQ_SYMBOL_RATE_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.main_tp1.tp_sym = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_MAIN_FREQ_DEMODULATE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
		{
			g_status.main_tp1.nim_modulate = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"BPSK") == 0)
		{
			g_status.main_tp1.nim_modulate = 1;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QPSK") == 0)
		{
			g_status.main_tp1.nim_modulate = 2;
		}
		else if (wcscmp(all_propertys->first[size].value, L"8PSK") == 0)
		{
			g_status.main_tp1.nim_modulate = 3;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM16") == 0)
		{
			g_status.main_tp1.nim_modulate = 4;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM32") == 0)
		{
			g_status.main_tp1.nim_modulate = 5;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM64") == 0)
		{
			g_status.main_tp1.nim_modulate = 6;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM128") == 0)
		{
			g_status.main_tp1.nim_modulate = 7;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM256") == 0)
		{
			g_status.main_tp1.nim_modulate = 8;
		}
		else
		{
			g_status.main_tp1.nim_modulate = 6;
		}
	}
    //main freq backup 1
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.main_tp2.tp_freq = atoi(value)*1000;
    }

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_SYMBOL_RATE_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.main_tp2.tp_sym = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_BACKUP_MAIN_FREQ_1_DEMODULATE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
		{
			g_status.main_tp2.nim_modulate = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"BPSK") == 0)
		{
			g_status.main_tp2.nim_modulate = 1;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QPSK") == 0)
		{
			g_status.main_tp2.nim_modulate = 2;
		}
		else if (wcscmp(all_propertys->first[size].value, L"8PSK") == 0)
		{
			g_status.main_tp2.nim_modulate = 3;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM16") == 0)
		{
			g_status.main_tp2.nim_modulate = 4;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM32") == 0)
		{
			g_status.main_tp2.nim_modulate = 5;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM64") == 0)
		{
			g_status.main_tp2.nim_modulate = 6;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM128") == 0)
		{
			g_status.main_tp2.nim_modulate = 7;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM256") == 0)
		{
			g_status.main_tp2.nim_modulate = 8;
		}
		else
		{
			g_status.main_tp2.nim_modulate = 6;
		}
	}
    //upgrade freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_UPGRADE_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.upgrade_tp.tp_freq = atoi(value)*1000;
    }

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_UPGRADE_FREQ_SYMBOL_RATE_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.upgrade_tp.tp_sym = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_UPGRADE_FREQ_DEMODULATE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"BPSK") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 1;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QPSK") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 2;
		}
		else if (wcscmp(all_propertys->first[size].value, L"8PSK") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 3;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM16") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 4;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM32") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 5;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM64") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 6;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM128") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 7;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM256") == 0)
		{
			g_status.upgrade_tp.nim_modulate = 8;
		}
		else
		{
			g_status.upgrade_tp.nim_modulate = 6;
		}
	}
    //ad freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AD_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.ad_tp.tp_freq = atoi(value)*1000;
    }

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AD_FREQ_SYMBOL_RATE_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.ad_tp.tp_sym = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AD_FREQ_DEMODULATE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
		{
			g_status.ad_tp.nim_modulate = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"BPSK") == 0)
		{
			g_status.ad_tp.nim_modulate = 1;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QPSK") == 0)
		{
			g_status.ad_tp.nim_modulate = 2;
		}
		else if (wcscmp(all_propertys->first[size].value, L"8PSK") == 0)
		{
			g_status.ad_tp.nim_modulate = 3;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM16") == 0)
		{
			g_status.ad_tp.nim_modulate = 4;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM32") == 0)
		{
			g_status.ad_tp.nim_modulate = 5;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM64") == 0)
		{
			g_status.ad_tp.nim_modulate = 6;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM128") == 0)
		{
			g_status.ad_tp.nim_modulate = 7;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM256") == 0)
		{
			g_status.ad_tp.nim_modulate = 8;
		}
		else
		{
			g_status.ad_tp.nim_modulate = 6;
		}
	}
    //nvod freq
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_NVOD_FREQ_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.nvod_tp.tp_freq = atoi(value)*1000;
    }

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_NVOD_FREQ_SYMBOL_RATE_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.nvod_tp.tp_sym = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_NVOD_FREQ_DEMODULATE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
		{
			g_status.nvod_tp.nim_modulate = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"BPSK") == 0)
		{
			g_status.nvod_tp.nim_modulate = 1;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QPSK") == 0)
		{
			g_status.nvod_tp.nim_modulate = 2;
		}
		else if (wcscmp(all_propertys->first[size].value, L"8PSK") == 0)
		{
			g_status.nvod_tp.nim_modulate = 3;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM16") == 0)
		{
			g_status.nvod_tp.nim_modulate = 4;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM32") == 0)
		{
			g_status.nvod_tp.nim_modulate = 5;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM64") == 0)
		{
			g_status.nvod_tp.nim_modulate = 6;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM128") == 0)
		{
			g_status.nvod_tp.nim_modulate = 7;
		}
		else if (wcscmp(all_propertys->first[size].value, L"QAM256") == 0)
		{
			g_status.nvod_tp.nim_modulate = 8;
		}
		else
		{
			g_status.nvod_tp.nim_modulate = 6;
		}
	}
#if 0
    //print flag
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_PRINT_FLAG_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"关闭") == 0)
      {
        g_status.printf_switch_set.printf_flag = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"打开") == 0)
      {
        g_status.printf_switch_set.printf_flag = 1;
      }
      else
      {
        g_status.printf_switch_set.printf_flag = 0;
      }
    }
#endif
    //language set
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_LANGUAGE_SET_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"English") == 0)
      {
        g_status.lang_set.osd_text = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"简体中文") == 0)
      {
        g_status.lang_set.osd_text = 1;
      }
      else
      {
        g_status.lang_set.osd_text = 0;
      }
    }
    //video output
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VIDEO_OUTPUT_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"CVBS") == 0)
      {
        g_status.av_set.video_output = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"SVDIEO") == 0)
      {
        g_status.av_set.video_output = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"RGB") == 0)
      {
        g_status.av_set.video_output = 2;
      }
      else if (wcscmp(all_propertys->first[size].value, L"YUV") == 0)
      {
        g_status.av_set.video_output = 3;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_YUV") == 0)
      {
        g_status.av_set.video_output = 4;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_SVDIEO") == 0)
      {
        g_status.av_set.video_output = 5;
      }
      else if (wcscmp(all_propertys->first[size].value, L"CVBS_CVBS") == 0)
      {
        g_status.av_set.video_output = 6;
      }
      else
      {
        g_status.av_set.video_output = 4;
      }
    }
    //volume mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VOLUME_MODE_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Single volume") == 0)
      {
        g_status.audio_set.is_global_volume = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Global volume") == 0)
      {
        g_status.audio_set.is_global_volume = 1;
      }
      else
      {
        g_status.audio_set.is_global_volume = 0;
      }
    }
    //global volume
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_GLOBAL_VOLUME_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_status.audio_set.global_volume = (u8)atoi(value);
    }
    //track mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TRACK_MODE) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Singal Track") == 0)
      {
        g_status.audio_set.is_global_track = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Global Track") == 0)
      {
        g_status.audio_set.is_global_track = 1;
      }
      else
      {
        g_status.audio_set.is_global_track = 0;
      }
    }
    //global track
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AUDIO_TRACK_KEY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Stereo") == 0)
      {
        g_status.audio_set.global_track = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Left") == 0)
      {
        g_status.audio_set.global_track = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Right") == 0)
      {
        g_status.audio_set.global_track = 2;
      }
      else
      {
        g_status.audio_set.global_track = 0;
      }
    }
#if 0
	//watermark
	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_WATERMARK_ENABLE_KEY) == 0)
	{
		if (wcscmp(all_propertys->first[size].value, L"Disable Watermark") == 0)
		{
			g_status.b_WaterMark = 0;
		}
		else if (wcscmp(all_propertys->first[size].value, L"Enable Watermark") == 0)
		{
			g_status.b_WaterMark = 1;
		}
		else
		{
			g_status.b_WaterMark = 0;
		}
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_WATERMARK_X_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.x_WaterMark = atoi(value);
	}

	if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_WATERMARK_Y_KEY) == 0)
	{
		wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
		g_status.y_WaterMark  = atoi(value);
	}
#endif
	//tv system mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TV_SYSTEM_MODE) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"PAL") == 0)
      {
		  g_status.av_set.tv_mode = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"NTSC") == 0)
      {
        g_status.av_set.tv_mode = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
      {
        g_status.av_set.tv_mode = 2;
      }
      else
      {
        g_status.av_set.tv_mode = 2;
      }
    }

	//tv resolution
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TV_RESOLUTION) == 0)
    {
	  if(g_status.av_set.tv_mode == 0)
	  {
		  if (wcscmp(all_propertys->first[size].value, L"576i") == 0)
		  {
			  g_status.av_set.tv_resolution = 0;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"576p") == 0)
		  {
			g_status.av_set.tv_resolution = 1;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"720p") == 0)
		  {
			g_status.av_set.tv_resolution = 2;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080i") == 0)
		  {
			g_status.av_set.tv_resolution = 3;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080p") == 0)
		  {
			g_status.av_set.tv_resolution = 4;
		  }
		  else
		  {
			g_status.av_set.tv_resolution = 0;
		  }
	  }
	  else if(g_status.av_set.tv_mode == 1)
	  {
		  if (wcscmp(all_propertys->first[size].value, L"480i") == 0)
		  {
			  g_status.av_set.tv_resolution = 0;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"480p") == 0)
		  {
			g_status.av_set.tv_resolution = 1;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"720p") == 0)
		  {
			g_status.av_set.tv_resolution = 2;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080i") == 0)
		  {
			g_status.av_set.tv_resolution = 3;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080p") == 0)
		  {
			g_status.av_set.tv_resolution = 4;
		  }
		  else
		  {
			g_status.av_set.tv_resolution = 0;
		  }
	  }
    else if(g_status.av_set.tv_mode == 2)
    {
		  if ((wcscmp(all_propertys->first[size].value, L"480i") == 0)
			  || (wcscmp(all_propertys->first[size].value, L"576i") == 0))
		  {
			  g_status.av_set.tv_resolution = 0;
		  }
		  else if ((wcscmp(all_propertys->first[size].value, L"480p") == 0)
			  ||(wcscmp(all_propertys->first[size].value, L"576p") == 0))
		  {
			g_status.av_set.tv_resolution = 1;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"720p") == 0)
		  {
			g_status.av_set.tv_resolution = 2;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080i") == 0)
		  {
			g_status.av_set.tv_resolution = 3;
		  }
		  else if (wcscmp(all_propertys->first[size].value, L"1080p") == 0)
		  {
			g_status.av_set.tv_resolution = 4;
		  }
		  else
		  {
			g_status.av_set.tv_resolution = 0;
		  }
	  }
    }   

	//tv ratio
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TV_RATIO) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Auto") == 0)
      {
		  g_status.av_set.tv_ratio = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"4:3LetterBox") == 0)
      {
        g_status.av_set.tv_ratio = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"4:3PanScan") == 0)
      {
        g_status.av_set.tv_ratio = 2;
      }
      else if (wcscmp(all_propertys->first[size].value, L"16:9") == 0)
      {
        g_status.av_set.tv_ratio = 3;
      }
      else 
      {
        g_status.av_set.tv_ratio = 0;
      }
    }

    //video quality
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VIDEO_QUALITY) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Standard") == 0)
      {
		  g_status.video_quality = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Default") == 0)
      {
        g_status.video_quality = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Vivid") == 0)
      {
        g_status.video_quality = 2;
      }
      else 
      {
        g_status.video_quality = 1;
      }
    }

	  //video effects
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_VIDEO_EFFECTS) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Sharpening") == 0)
      {
		  g_status.av_set.video_effects = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Adaptive") == 0)
      {
		  g_status.av_set.video_effects = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Soft") == 0)
      {
          g_status.av_set.video_effects = 2;
      }
      else
      {
		  g_status.av_set.video_effects = 0;
      }
    }

	//audio output
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_AUDIO_OUTPUT) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"LPCM Out") == 0)
      {
		  g_status.av_set.digital_audio_output = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"BS Out") == 0)
      {
        g_status.av_set.digital_audio_output = 1;
      }
      else
      {
        g_status.av_set.digital_audio_output = 0;
      }
    }

	//set timeout
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SET_TIMEOUT) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"3") == 0)
      {
		  g_status.osd_set.timeout = 3;
      }
      else if (wcscmp(all_propertys->first[size].value, L"4") == 0)
      {
		  g_status.osd_set.timeout = 4;
      }
      else if (wcscmp(all_propertys->first[size].value, L"5") == 0)
      {
		  g_status.osd_set.timeout = 5;
      }
      else if (wcscmp(all_propertys->first[size].value, L"6") == 0)
      {
		  g_status.osd_set.timeout = 6;
      }
      else if (wcscmp(all_propertys->first[size].value, L"7") == 0)
      {
		  g_status.osd_set.timeout = 7;
      }
      else if (wcscmp(all_propertys->first[size].value, L"8") == 0)
      {
		  g_status.osd_set.timeout = 8;
      }
      else if (wcscmp(all_propertys->first[size].value, L"9") == 0)
      {
		  g_status.osd_set.timeout = 9;
      }
      else if (wcscmp(all_propertys->first[size].value, L"10") == 0)
      {
		  g_status.osd_set.timeout = 10;
      }
	  else
      {
		  g_status.osd_set.timeout = 3;
      }
    }

   //set transparent
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SET_TRANSPARENT) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"0") == 0)
      {
		  g_status.osd_set.transparent = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"10") == 0)
      {
		  g_status.osd_set.transparent = 10;
      }
      else if (wcscmp(all_propertys->first[size].value, L"20") == 0)
      {
		  g_status.osd_set.transparent = 20;
      }
      else if (wcscmp(all_propertys->first[size].value, L"30") == 0)
      {
		  g_status.osd_set.transparent = 30;
      }
      else if (wcscmp(all_propertys->first[size].value, L"40") == 0)
      {
		  g_status.osd_set.transparent = 40;
      }
	  else
      {
		  g_status.osd_set.transparent = 0;
      }
    }

	   //set switch mode
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SET_SWITCH_MODE) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Video Freeze ") == 0)
      {
		  g_status.play_set.mode = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"Black Screen") == 0)
      {
		  g_status.play_set.mode = 1;
      }
	  else
      {
		  g_status.play_set.mode = 0;
      }
    }

    //gmt usage
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_GMT_USAGE) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Off") == 0)
      {
		  g_status.time_set.gmt_usage = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"On") == 0)
      {
        g_status.time_set.gmt_usage = 1;
      }
      else
      {
        g_status.time_set.gmt_usage = 0;
      }
    }

	//summer time
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SUMMER_TIME) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"Off") == 0)
      {
		  g_status.time_set.summer_time = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"On") == 0)
      {
        g_status.time_set.summer_time = 1;
      }
      else
      {
        g_status.time_set.summer_time = 0;
      }
    }
	
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_GMT_OFFSET) == 0)
    {
      if (wcscmp(all_propertys->first[size].value, L"UTC-11:30") == 0)
      {
        g_status.time_set.gmt_offset = 0;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-11:00") == 0)
      {
        g_status.time_set.gmt_offset = 1;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-10:30") == 0)
      {
        g_status.time_set.gmt_offset = 2;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-10:00") == 0)
      {
        g_status.time_set.gmt_offset = 3;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-09:30") == 0)
      {
        g_status.time_set.gmt_offset = 4;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-09:00") == 0)
      {
        g_status.time_set.gmt_offset = 5;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-08:30") == 0)
      {
        g_status.time_set.gmt_offset = 6;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-08:00") == 0)
      {
        g_status.time_set.gmt_offset = 7;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-07:30") == 0)
      {
        g_status.time_set.gmt_offset = 8;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-07:00") == 0)
      {
        g_status.time_set.gmt_offset = 9;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-06:30") == 0)
      {
        g_status.time_set.gmt_offset = 10;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-06:00") == 0)
      {
        g_status.time_set.gmt_offset = 11;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-05:30") == 0)
      {
        g_status.time_set.gmt_offset = 12;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-05:00") == 0)
      {
        g_status.time_set.gmt_offset = 13;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-04:30") == 0)
      {
        g_status.time_set.gmt_offset = 14;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-04:00") == 0)
      {
        g_status.time_set.gmt_offset = 15;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-03:30") == 0)
      {
        g_status.time_set.gmt_offset = 16;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-03:00") == 0)
      {
        g_status.time_set.gmt_offset = 17;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC-02:30") == 0)
      {
        g_status.time_set.gmt_offset = 18;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-02:00") == 0)
      {
        g_status.time_set.gmt_offset = 19;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-01:30") == 0)
      {
        g_status.time_set.gmt_offset = 20;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-01:00") == 0)
      {
        g_status.time_set.gmt_offset = 21;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC-00:30") == 0)
      {
        g_status.time_set.gmt_offset = 22;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+00:00") == 0)
      {
        g_status.time_set.gmt_offset = 23;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+00:30") == 0)
      {
        g_status.time_set.gmt_offset = 24;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+01:00") == 0)
      {
        g_status.time_set.gmt_offset = 25;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+01:30") == 0)
      {
        g_status.time_set.gmt_offset = 26;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+02:00") == 0)
      {
        g_status.time_set.gmt_offset = 27;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+02:30") == 0)
      {
        g_status.time_set.gmt_offset = 28;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+03:00") == 0)
      {
        g_status.time_set.gmt_offset = 29;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+03:30") == 0)
      {
        g_status.time_set.gmt_offset = 30;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+04:00") == 0)
      {
        g_status.time_set.gmt_offset = 31;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+04:30") == 0)
      {
        g_status.time_set.gmt_offset = 32;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+05:00") == 0)
      {
        g_status.time_set.gmt_offset = 33;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+05:30") == 0)
      {
        g_status.time_set.gmt_offset = 34;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+06:00") == 0)
      {
        g_status.time_set.gmt_offset = 35;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+06:30") == 0)
      {
        g_status.time_set.gmt_offset = 36;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+07:00") == 0)
      {
        g_status.time_set.gmt_offset = 37;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+07:30") == 0)
      {
        g_status.time_set.gmt_offset = 38;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+08:00") == 0)
      {
        g_status.time_set.gmt_offset = 39;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+08:30") == 0)
      {
        g_status.time_set.gmt_offset = 40;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+09:00") == 0)
      {
        g_status.time_set.gmt_offset = 41;
      }
      else if (wcscmp(all_propertys->first[size].value, L"UTC+09:30") == 0)
      {
        g_status.time_set.gmt_offset = 42;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+10:00") == 0)
      {
        g_status.time_set.gmt_offset = 43;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+10:30") == 0)
      {
        g_status.time_set.gmt_offset = 44;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+11:00") == 0)
      {
        g_status.time_set.gmt_offset = 45;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+11:30") == 0)
      {
        g_status.time_set.gmt_offset = 46;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+12:00") == 0)
      {
        g_status.time_set.gmt_offset = 47;
      }
	else if (wcscmp(all_propertys->first[size].value, L"UTC+12:30") == 0)
      {
        g_status.time_set.gmt_offset = 48;
      }

    }

   if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_DATE) == 0)
   {
	wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
	for(j=0;j<MIDDLELENGTH;j++)
	{
	     if((value[j] == '-')&& (num == 0))
	    {
	        tmp_value[k]='\0';
	     	 g_status.time_set.sys_time.year =(u16) atoi(tmp_value);
	        num ++;
		 memset(tmp_value,0,MIDDLELENGTH);
		 k=0;
		 continue;
	     }
           else if((value[j] == '-') && (num == 1))
   	    {
	        tmp_value[k]='\0';
	     	 g_status.time_set.sys_time.month=(u8) atoi(tmp_value);
	        num ++;
		 memset(tmp_value,0,MIDDLELENGTH);
		 k=0;
		 continue;
   	     }
	     else if(value[j]=='\0')
     	    {
	        tmp_value[k]=value[j];
	     	 g_status.time_set.sys_time.day=(u8) atoi(tmp_value);
		 break;
     	    }
	     tmp_value[k]=value[j];
	     k++;
	}
   }

   num = 0;
   k=0;
   memset(tmp_value,0,MIDDLELENGTH);
   
   if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_TIME) == 0)
   {
	wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
	for(j=0;j<MIDDLELENGTH;j++)
	{
	     if((value[j] == ':') && (num == 0))
	    {
	        tmp_value[k]='\0';
	     	 g_status.time_set.sys_time.hour=(u8) atoi(tmp_value);
	        num ++;
		 memset(tmp_value,0,MIDDLELENGTH);
		 k=0;
		 continue;
	     }
           else if((value[j] == '\0'))
   	    {
	        tmp_value[k]=value[j];
	     	 g_status.time_set.sys_time.minute=(u8) atoi(tmp_value);
		 break;
   	     }
	     tmp_value[k]=value[j];
	     k++;
	}
   }
 #ifdef ENABLE_NETWORK
    num = 0;
    k=0;
    memset(tmp_value,0,MIDDLELENGTH);

  //mac
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_SET_NET_MAC) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
	for(j=0;j<MIDDLELENGTH;j++)
	{
           if((value[j] == '\0'))
   	    {
		 strcpy(g_status.mac_set.mac_six,tmp_value);
		 break;
   	     }
	     if(value[j] == '.')
            {
                if(num==0)
		     	 	strcpy(g_status.mac_set.mac_one,tmp_value);
		  else if(num==1)
		     	 	strcpy(g_status.mac_set.mac_two,tmp_value);
		  else if(num==2)
		     	 	strcpy(g_status.mac_set.mac_three,tmp_value);
		  else if(num==3)
		     	 	strcpy(g_status.mac_set.mac_four,tmp_value);
		  else if(num==4)
		     	 	strcpy(g_status.mac_set.mac_five,tmp_value);
		   num ++;
		   memset(tmp_value,0,MIDDLELENGTH);
	          k=0;
		   continue;
        	}
	    tmp_value[k]=value[j];
	    k++;
	}
   }
#endif

    //ota oui
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_OUI) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      sscanf(value, "0x%04x", &g_misc_options.ota_tdi.oui);
    }

    //hw mod id
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_HW_MOD_ID_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.hw_mod_id = (u16)atoi(value);
    }

    //sw mod id
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_SW_MOD_ID_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.sw_mod_id = (u16)atoi(value);
    }

    //hw version
    if (wcscmp(all_propertys->first[size].key, SYS_CONFIG_OTA_HW_VERSION_KEY) == 0)
    {
      wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
      g_misc_options.ota_tdi.hw_version = (u16)atoi(value);
    }
    for(i = 0; i < sizeof(ir_keymap1) / sizeof(key_map_config_t); i++)
    {
      if (wcscmp(all_propertys->first[size].key, ir_keymap1[i].v_key) == 0)
      {
        wcstombs(value, all_propertys->first[size].value, MIDDLELENGTH);
        sscanf(value, "0x%02x", &cfg_tmp[i]);
      }
    }
  }
  if(g_bBigEdian == TRUE)
  {
    SSData_endian_convert();
  }
   if(all_propertys->size >100)
   {
       g_FlashParser.SaveSSData((char *)&g_status, sizeof(g_status));
       g_FlashParser.SaveMiscOptions((char*)&g_misc_options, sizeof(g_misc_options));
       g_FlashParser.SaveIRData((char*)cfg_tmp, sizeof(ir_keymap1) / sizeof(key_map_config_t));  
       if(ret)
   	{
       	Clean(all_propertys);	       
   	}
       //write new bin file  
       return g_FlashParser.SaveBinFile(file_path);
   }
  else
  {
      if(wcscmp(all_propertys->first[0].group, L"Basic set")==0)
      {  
          g_FlashParser.SaveSSData((char *)&g_status, sizeof(g_status));
          if(ret)
          {
            	Clean(all_propertys);	       
          }
          return g_FlashParser.SingleSaveSSData(file_path);
      }
      else if(wcscmp(all_propertys->first[0].group, L"IR Set")==0)
      {
        g_FlashParser.SaveIRData((char*)cfg_tmp, sizeof(ir_keymap1) / sizeof(key_map_config_t));  
          if(ret)
          {
            	Clean(all_propertys);	       
          }
          return g_FlashParser.SingleSaveIRData(file_path);
      }
      else if(wcscmp(all_propertys->first[0].group, L"OTA Parameters")==0)
      {
          g_FlashParser.SaveMiscOptions((char*)&g_misc_options, sizeof(g_misc_options));
          if(ret)
          {
            	Clean(all_propertys);	       
          }
          return g_FlashParser.SingleSaveMiscOptions(file_path);
      }
  }
}

int Clean(struct NativePropertyArray* all_propertys)
{
	for(unsigned int size=0; size<all_propertys->size; size++)
	{
        if (wcslen(all_propertys->first[size].key) == 0)
        {
	     break;
        }
       
        int option_length = 0;
		option_length = all_propertys->first[size].options_len;
		if (option_length != 0)
		{
			delete[] all_propertys->first[size].options;
			all_propertys->first[size].options = 0;
		}
	}

	//delete[] all_propertys->first;
	//all_propertys->first = 0;
        ret = 0;
  return 0;
}

void GetLastErrorMsg(wchar_t* info, size_t info_length)
{
  wcsncpy(info, g_LastErrorMsg, info_length);
}
