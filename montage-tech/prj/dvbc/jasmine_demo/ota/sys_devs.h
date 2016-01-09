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

/* nim */
extern RET_CODE nim_win32_attach(char *name);
#define NIM_ATTAGH nim_win32_attach
#define NIM_NAME   "NIM_WIN32_0"

/* pti */
extern RET_CODE pti_soft_attach(char *name);
#define PTI_ATTACH pti_soft_attach
#define PTI_NAME   "PTI_SOFT_0"

/* uio */
extern RET_CODE uio_attach(char * name);
#define UIO_ATTACH uio_attach
#define UIO_NAME   "UIO_WIN32_0"

/* osd */
extern RET_CODE osd_win32_attach(char * name);
#define OSD_ATTACH osd_win32_attach
#define OSD_NAME "OSD_WIN32_0"

/* sub */
extern RET_CODE sub_win32_attach(char * name);
#define SUB_ATTACH sub_win32_attach
#define SUB_NAME "SUB_WIN32_0"

/* gpe */
extern RET_CODE gpe_8bit_attach(char * name);
#define GPE_ATTACH gpe_8bit_attach
#define GPE_NAME "GPE_WIN32_0"

/* audio */
extern RET_CODE win32_audio_attach(char * name);
#define AUDIO_ATTACH win32_audio_attach
#define AUDIO_NAME "AUDIO_WIN32_0"

/* video */
extern RET_CODE video_win32_attach(char *name);
#define VIDEO_ATTACH video_win32_attach
#define VIDEO_NAME "VIDEO_WIN32_0"

/* vbi */
extern RET_CODE win32_vbi_inserter_attach(char * name);
#define VBI_ATTACH win32_vbi_inserter_attach
#define VBI_NAME "VBI_WIN32_0"


/* Scart*/
extern RET_CODE win32_scart_attach(char *name);
#define SCART_ATTACH win32_scart_attach
#define SCART_NAME "SCART_WIN32_0"

/* RF */
extern RET_CODE win32_rf_attach(char * name);
#define RF_ATTACH win32_rf_attach
#define RF_NAME "RF_WIN32_0"

/* av sync */
extern RET_CODE win32_avsync_attach(char * name);
#define AVSYNC_ATTACH win32_avsync_attach
#define AVSYNC_NAME "AVSYNC_WIN32_0"

#else //not WIN32
/* char storage */
extern RET_CODE spi_wz_attach(char *name);
#define CHARSTO_ATTACH  spi_wz_attach
#define CHARSTO_NAME "SPI_WIZARDS_0"

/* i2c */
extern RET_CODE i2c_wzctrller_attach(char *name);
#define I2CM_ATTAGH i2c_wzctrller_attach
//#endif
#define I2CM_NAME   "I2CM_WIZARDS_0"

/* nim */
extern RET_CODE nim_m88cs2200_attach(char *name);
#define NIM_ATTAGH nim_m88cs2200_attach
#define NIM_NAME   "NIM_M88CS2200_0"

/* uio */
extern RET_CODE uio_attach(char *name);
#define UIO_ATTACH uio_attach
#define UIO_NAME   "UIO_WIZARDS_0"

/* osd */
extern RET_CODE wizards_osd_attach(char * name);
#define OSD_ATTACH wizards_osd_attach
#define OSD_NAME "OSD_WIZARDS_0"

/* sub */
extern RET_CODE wizards_sub_attach(char * name);
#define SUB_ATTACH wizards_sub_attach
#define SUB_NAME "SUB_WIZARDS_0"

/* gpe */
extern RET_CODE gpe_8bit_attach(char * name);
#define GPE_ATTACH gpe_8bit_attach
#define GPE_NAME "GPE_WIZARDS_0"

/* pti */
extern RET_CODE wizards_pti_attach(char * name);
#define PTI_ATTACH wizards_pti_attach
#define PTI_NAME "PTI_WIZARDS_0"

/* audio */
extern RET_CODE wizards_audio_attach(char * name);
#define AUDIO_ATTACH wizards_audio_attach
#define AUDIO_NAME "AUDIO_WIZARDS_0"

/* video */
extern RET_CODE wizards_video_attach(char *name);
#define VIDEO_ATTACH wizards_video_attach
#define VIDEO_NAME "VIDEO_WIZARDS_0"

/* av sync */
extern RET_CODE wizards_avsync_attach(char * name);
#define AVSYNC_ATTACH wizards_avsync_attach
#define AVSYNC_NAME "AVSYNC_WIZARDS_0"

/* vbi */
extern RET_CODE magic_vbi_inserter_attach(char * name);
#define VBI_ATTACH magic_vbi_inserter_attach
#define VBI_NAME "VBI_WIZARDS_0"

/* Scart*/
extern RET_CODE magic_scart_attach(char *name);
#define SCART_ATTACH magic_scart_attach
#define SCART_NAME "SCART_WIZARDS_0"

/* RF */
extern RET_CODE magic_rf_attach(char * name);
#define RF_ATTACH magic_rf_attach
#define RF_NAME "RF_WIZARDS_0"

#endif
#endif //__SYS_DEVS_H__

