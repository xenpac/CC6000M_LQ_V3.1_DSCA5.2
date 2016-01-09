/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
 ****************************************************************************/
#ifndef __UI_GUI_DEFINES_H__
#define __UI_GUI_DEFINES_H__

#define C_TRANS           0x00000
#define C_KEY             0xFF00FEFF

#define C_BLUE            0xFF1E6498
#define C_YELLOW        0xFFCC9900

#define C_BLACK           0xFF000000
#define C_WHITE           0xFFFFFFFF


enum rect_style_id
{
  RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn
  RSI_TRANSPARENT,
  RSI_BLUE,
  RSI_YELLOW,
  RSI_WHITE,
  RSI_BLACK,
  RSI_DLG_FRM,

  RSTYLE_CNT,
};

enum font_style_id
{
  FSI_BLACK = 0,        //black string
  FSI_WHITE,            //white string
  FSTYLE_CNT,
};

enum font_lib_id
{
  FONT_ID_1 = 1,
  FONT_ID_2
};

#define MAX_RSTYLE_CNT        50
#define MAX_FSTYLE_CNT        10

#define FSI_COMM_TXT_N          FSI_WHITE
#define FSI_COMM_TXT_HL        FSI_BLACK
#define FSI_COMM_TXT_G          FSI_WHITE

#define RSI_COMM_TXT_N          RSI_BLUE
#define RSI_COMM_TXT_HL        RSI_YELLOW
#define RSI_COMM_TXT_G          RSI_BLUE

extern rsc_config_t g_rsc_config;

#endif

