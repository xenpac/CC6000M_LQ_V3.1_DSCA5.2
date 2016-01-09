/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"
#include "ui_util.h"
#include "str_id.h"

extern iconv_t g_cd_gb2312_to_utf16le;
extern iconv_t g_cd_utf8_to_utf16le;

RET_CODE ui_start_timer(u8 root_id, u16 msg, u32 time_out)
{
    if (fw_tmr_reset(root_id, msg, time_out) == SUCCESS)
    {
        return SUCCESS;
    }

    return fw_tmr_create(root_id, msg, time_out, FALSE);
}

#ifndef WIN32
inline RET_CODE ui_stop_timer(u8 root_id, u16 msg)
{
    return fw_tmr_destroy(root_id, msg);
}
#else
RET_CODE ui_stop_timer(u8 root_id, u16 msg)
{
    return fw_tmr_destroy(root_id, msg);
}
#endif

void convert_gb2312_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len)
{
	int src_len = 0;
	char* outbuf = NULL;
	int dest_len = 0;

	src_len = strlen(asc_str) + 1;
	outbuf = (char*)uni_str;
	dest_len = uni_str_len;
	iconv(g_cd_gb2312_to_utf16le, (char**)&asc_str, &src_len, &outbuf, &dest_len);
}

void convert_utf8_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len)
{
	int src_len = 0;
	char* outbuf = NULL;
	int dest_len = 0;

	src_len = strlen(asc_str) + 1;
	outbuf = (char*)uni_str;
	dest_len = uni_str_len;
	iconv(g_cd_utf8_to_utf16le, (char**)&asc_str, &src_len, &outbuf, &dest_len);
}

void ctrl_change_focus(control_t *p_lfCtrl, control_t *p_gfCtrl)
{
    if (p_lfCtrl != NULL)
    {
        ctrl_process_msg(p_lfCtrl, MSG_LOSTFOCUS, 0, 0);
        ctrl_paint_ctrl(p_lfCtrl, TRUE);
    }
    if (p_gfCtrl != NULL)
    {
        ctrl_set_attr(p_gfCtrl, OBJ_ATTR_ACTIVE);
        ctrl_process_msg(p_gfCtrl, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_gfCtrl, TRUE);
    }
}

BOOL iso_639_get_lang_code(u8 *lang_code, u8 index)
{
  u8 *lang_code_l[LANG_CODE] = 
        {
          "eng",
          "fre","fra",
          "ger","deu",
          "ita",
          "spa",
          "por",
          "rus",
          "tur",
          "pol",
          "ara",
          "fas","per",
        };

  if(index >= LANG_CODE)
  {
    lang_code[0] = 0;
    return FALSE;
  }

  memcpy(lang_code, lang_code_l[index], 3);

  OS_PRINTF("AT [%s] in Line %d, land_code = %c%c%c\n", __FUNCTION__, __LINE__, lang_code_l[index][0], lang_code_l[index][1],lang_code_l[index][2]);
  
  return TRUE;
}

u8 iso_639_get_lang_code_num()
{
  return LANG_CODE;
}

BOOL iso_639_get_lang_code_name(u16 *uni_lang_code, u16 *lang_name_id, u8 index)
{  
  u16 opt_data[11] =
  {
    IDS_LANGUAGE_ENGLISH, IDS_LANGUAGE_FRENCH, IDS_LANGUAGE_GERMAN, 
    IDS_LANGUAGE_ITALIAN, IDS_LANGUAGE_SPANISH, IDS_LANGUAGE_PORTUGUESE, 
    IDS_LANGUAGE_RUSSIAN, IDS_LANGUAGE_TURKISH, IDS_LANGUAGE_POLAND ,
    IDS_LANGUAGE_ARABIC, IDS_LANGUAGE_FARSI,
  };

  u8 offset[LANG_CODE] =
     {
       0,
       0, 1,
       1, 2,
       2,
       2,
       2,
       2,
       2,
       2,
       2,
       2, 3
     };

  BOOL ret = FALSE;

  u8 lang_code[3] = {0};

  ret = iso_639_get_lang_code(lang_code, index);

  if(TRUE == ret)
  {
    uni_lang_code[0] = lang_code[0];
    uni_lang_code[1] = lang_code[1];
    uni_lang_code[2] = lang_code[2];
    *lang_name_id = opt_data[index - offset[index]];
    OS_PRINTF("AT [%s] in Line %d, lang_name_id = %d\n", __FUNCTION__, __LINE__, *lang_name_id);

  }

  return ret;
}


