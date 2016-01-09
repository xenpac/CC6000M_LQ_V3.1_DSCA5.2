/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

/******************************************************************************/
#ifndef __DEVS_H__
#define __DEVS_H__

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

/* av sync */
extern RET_CODE win32_avsync_attach(char * name);
#define AVSYNC_ATTACH win32_avsync_attach
#define AVSYNC_NAME "AVSYNC_WIN32_0"


#endif //__SYS_DEVS_H__

