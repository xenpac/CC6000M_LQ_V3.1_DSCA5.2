/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __SYS_DEVS_H__
#define __SYS_DEVS_H__

/* device register function deinifination,
  * System device name definition
  */

#ifdef WIN32
/* char storage */
extern RET_CODE charsto_win32_attach(char *name);
#define CHARSTO_ATTACH  charsto_win32_attach
#define CHARSTO_NAME "CHARSTO_WIN32_0"

#else //not WIN32
/* char storage */
extern RET_CODE spi_sonata_attach(char *name);
#define CHARSTO_ATTACH  spi_sonata_attach
#define CHARSTO_NAME "SPI_SONATA"

#endif
#endif //__SYS_DEVS_H__

