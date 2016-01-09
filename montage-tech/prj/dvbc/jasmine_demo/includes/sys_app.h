/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __SYS_APP_H_
#define __SYS_APP_H_

/*!
   printf level
  */
typedef enum
{
  PRINTF_DEBUG_LEVEL = 0,
  PRINTF_WARNING_LEVEL,
  PRINTF_ERROR_LEVEL,
  PRINTF_INFO_LEVEL,
  PRINTF_ALWAYS_LEVEL,
}app_printf_level;

void  app_printf(app_printf_level level,const char *p_format, ...);

// debug log
#define APPLOGD(fmt, ...)\
do\
{app_printf(PRINTF_DEBUG_LEVEL, "D[%s][%d]" fmt "\n", __FUNCTION__, __LINE__ , ##__VA_ARGS__);}\
while (0)

// warning log
#define APPLOGW(fmt, ...)\
do\
{app_printf(PRINTF_WARNING_LEVEL, fmt , ##__VA_ARGS__);}\
while (0)

// error log
#define APPLOGE(fmt, ...)\
do\
{app_printf(PRINTF_ERROR_LEVEL, "E[%s][%d]" fmt , __FUNCTION__, __LINE__ , ##__VA_ARGS__);}\
while (0)

// info log
#define APPLOGI(fmt, ...)\
do\
{app_printf(PRINTF_INFO_LEVEL, fmt , ##__VA_ARGS__);}\
while (0)

// always print the message
#define APPLOGA(fmt, ...)\
do\
{app_printf(PRINTF_ALWAYS_LEVEL, fmt , ##__VA_ARGS__);}\
while (0)


#endif
