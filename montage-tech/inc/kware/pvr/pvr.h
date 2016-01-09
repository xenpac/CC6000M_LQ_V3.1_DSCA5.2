/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PVR_H__
#define __PVR_H__

#include "lib_rect.h"


#define pvrFOURCC( ch0, ch1, ch2, ch3 )				\
		( (unsigned int)(unsigned char)(ch0) | ( (unsigned int)(unsigned char)(ch1) << 8 ) |	\
		( (unsigned int)(unsigned char)(ch2) << 16 ) | ( (unsigned int)(unsigned char)(ch3) << 24 ) )
		
#define PVR_MAX_AUDIO_NUM	(32)
#define PVR_MAX_DVBSUB_NUM	(8)
#define PVR_MAX_TTXSUB_NUM	(8)


#define PLAY_SPEED_1X	( 1 )
#define PLAY_SPEED_2X	( 2 )
#define PLAY_SPEED_4X	( 4 )
#define PLAY_SPEED_8X	( 8 )
#define PLAY_SPEED_16X	( 16 )
#define PLAY_SPEED_32X	( 32 )


typedef  int (* pvr_av_stop_cb)(unsigned int);///input value, 1-freeze, 0-no freeze

typedef enum {
    Mode_CircularFileMode,
    Mode_LineFileMode
} PVR_TimeShfitMode;
typedef enum    
{
	PVR_PLAY_STOP  	= 0,
	PVR_PLAY_PLAY  	= 1,
	PVR_PLAY_PAUSE 	= 2,
	PVR_PLAY_FF	 	= 3,	
	PVR_PLAY_STEP  	= 4,
	PVR_PLAY_FB	 	= 5,
	PVR_PLAY_SLOW	= 6,	
	PVR_PLAY_REVSLOW = 7,
	PVR_PLAY_JUMP = 8,
	PVR_PLAY_PREV = 9,
	PVR_PLAY_NEXT = 10,
	PVR_PLAY_TOTAL = 11,
}PVR_PLAYSTATE;

typedef enum {
    PVR_REC_START = 1,
    PVR_REC_PAUSE ,
    PVR_REC_RESUME,
    PVR_REC_STOP,
    PVR_REC_EXIT,
    PVR_REC_CLEANUP,
} PVR_RECORDSTATE;

typedef enum
{
	PVR_AUDIO_MPEG = 0x50,				/*!< Mpeg decode type */
	PVR_AUDIO_AAC = pvrFOURCC('M', 'P', '4', 'A'),			/*!< Mpeg-AAC decode type */
	PVR_AUDIO_AAC_LATM = pvrFOURCC('M', 'P', '4', 'L'),
	PVR_AUDIO_DTS = 0x2001,				/*!< DTS decode type */
	PVR_AUDIO_PCM = pvrFOURCC('B', 'P', 'C', 'M'),				/*!< PCM decode type */
	PVR_AUDIO_MP3 = 0,				/*!< Mp3 decode type */
	PVR_AUDIO_WMA,				/*!< WMA decode type */
	PVR_AUDIO_AC3,				/*!< AC3 decode type */
	PVR_AUDIO_OGG,				/*!< OGG decode type */
	PVR_AUDIO_BEEP_TONE,		/*!< Beep Tone decode type */
	PVR_AUDIO_COOK,				/*!< Cook decode type */
	PVR_AUDIO_AMR_NB,			/*!< Adaptive Multi-Rate Narrowband decode type*/
	PVR_AUDIO_AMR_WB,			/*!< Adaptive Multi-Rate Wideband decode type*/
	PVR_AUDIO_FLAC,			    /*!< Free Lossless Audio Codec  decode type*/
	PVR_AUDIO_EAC3,				/*!< E-AC3 decode type */
	PVR_AUDIO_TOTAL				/*!< Count of total, just for reference, not a real type */
}PVRAUDIO_TYPE;
typedef enum
{
	PVR_VIDEO_MPEG2 = 0x10000002,
	PVR_VIDEO_H264 = 0x10000005,
	PVR_VIDEO_AVC	= pvrFOURCC('a', 'v', 'c', '1'),
	PVR_VIDEO_VC1 = pvrFOURCC('W', 'V', 'C', '1'),
	PVR_VIDEO_MPEG4 = 0x10000004,
	PVR_VIDEO_DIVX3 = 0,			/*!< Divx3 decode type */
	PVR_VIDEO_RMVB,				/*!< RMVB decode type */
	PVR_VIDEO_AVS,				/*!< AVS decode type */
	PVR_VIDEO_MJPEG,			/*!< Mjpeg decode type */

	PVR_VIDEO_LOGO,     /* for reference, not a real decode type */
	PVR_VIDEO_LOGO_MPEG,
	/* Still */
	PVR_VIDEO_STILL,			/*!< For reference, not a real type */
	PVR_VIDEO_JPEG_HW,			/*!< Hardware-Jpeg decode type */
	PVR_VIDEO_JPEG_SW,			/*!< Software-Jpeg decode type */
	PVR_VIDEO_GIF,				/*!< Gif decode type */
	PVR_VIDEO_BMP,				/*!< BMP decode type */
	PVR_VIDEO_PNG,				/*!< PNG decode type */
	PVR_VIDEO_TIFF,				/*!< TIFF decode type */
	/* Draw Data */
	PVR_VIDEO_STILL_DIRECT_DRAW,
	PVR_VIDEO_TOTAL		/*!< Count of total, just for reference, not a real type */	

}PVRVIDEO_TYPE;
typedef struct
{
      int absolute_path;
      
      unsigned int fileplay_task_priority;
      unsigned int record_task_priority;
      unsigned int task_preload_priority;

      unsigned int vdec_mem_addr;
      unsigned int vdec_mem_len;
      unsigned int adec_mem_addr;
      unsigned int adec_mem_len;

      pvr_av_stop_cb av_stop;
}PVR_SYS_PARAM;
typedef struct
{
	int  	   iTimeshiftState; // 0->only record , 1->timeshift , 2-> auto timeshift
	//int        bTS;
	//int        bTV;
	unsigned int dRecUnit;
	//unsigned int iTunerIndex;
	unsigned int maxFileSizeInMB;
	unsigned short usVidPID;//for play
	unsigned short usAudPID[PVR_MAX_AUDIO_NUM];//for play

	unsigned short usPCRPID;
	//unsigned short usPvtPID;
	//unsigned short	usProgNum;			/*!< program number */
	unsigned short	usPmtPid;
	unsigned short 	u16SubtitlePID[PVR_MAX_DVBSUB_NUM];
	unsigned short	u16TTXSubPID[PVR_MAX_TTXSUB_NUM];
	//unsigned short	awOtherPesPids[ 16 ];
	//unsigned short	awOtherPsiPids[ 16 ];
	PVRVIDEO_TYPE eVideoType;//for play
	PVRAUDIO_TYPE eAudioType[PVR_MAX_AUDIO_NUM];//for play

	//unsigned short PmtSectionSize;
	//unsigned char * pPMTSectionBuffer;
	char *acFullPath;
	//unsigned int uiRecDurationSec; /* in unit of second */

      int isAllTP;
      PVR_TimeShfitMode mode;
      unsigned int deleteFlag;// 1:delete recorded files when timeshift off,    0:don't delete.
}PVR_PARAM;

typedef struct
{
	/*!
    ** the free size of device size in MB.
    */
    unsigned int freeSize;
	
    /*!
    ** Current size in KB.
    */
    unsigned int size;

    /*!
    ** Current write position in in KB.
    */
    unsigned int position;

    /*!
    ** Current duration in seconds.
    */
    unsigned int duration;

    /*!
    ** Current data rate in bits per second.
    ** It indicates the data rate of the data written to hard-disk.
    ** The average over three measurements is taken to avoid big variations.
    */
    unsigned int dataRate;

    /*!
    ** Current number of splits.
    */
    unsigned short splitCount;

    /*!
    ** Current split file size.
    */
    unsigned int splitSize;

} PVR_RecordInfo;

int PVR_Uninit();
int PVR_Init(PVR_SYS_PARAM *para);

int PVR_RCD_Init(char *acRecordPath,PVR_PARAM *pvrParam);
int PVR_RecordStart(PVR_PARAM *pvrParam);
int PVR_RecordStop();
int PVR_RecordPause();
int PVR_RecordResume();
int PVR_RecordStatus();

///freesize  KB,,,,,,,size KB,,,,,,,sec--sec.
int PVR_GetRecordInfo (unsigned int * freesize, unsigned int * size, int * sec, char * record_path);
int PVR_StreamPlay(char * Path, int file_index);
int PVR_StreamStop();
BOOL PVR_IsRecFree(void);
void PVR_ResetSetRecordPlayMode(void);
int PVR_StreamInit(void);
int PVR_StreamUnint(void);
int PVR_StreamResume(void);
int PVR_StreamPause(void);
int PVR_StreamPauseResume(void);
int PVR_StreamForword(void);
int PVR_StreamBackward(void);
int PVR_StreamJump(int jumptotime);
int PVR_GetStreamTotalTime(void);
int PVR_GetStreamCurTime(void);
void PVR_SetPlayStatus(PVR_PLAYSTATE PlayState);
int PVR_StreamPlayStatus(void);
int PVR_StreamPlaySpeed(void);
void PVR_SetPlayMode(unsigned int uiMode);
void PVR_SetPlaySpeed(unsigned int iSpeed);

unsigned int PVR_GetRecordPlayMode(void);
BOOL PVR_IsTsFileLocked(char *pcPath);
BOOL PVR_LockTsFile(char *pcPath, BOOL bSetToLock);
int PVR_TimeshiftSwitchAudio(int eAudioType,int usAudPID); 
int PVR_PlayPhoto(char * PhotoFilePath, char * pcPicBuf, unsigned int iTotalLen, rect_t * p_rect);
int PVR_StopPhoto(void);
int PVR_ClearPhoto(int iStartCol, int iStartRow, int iWidth, int iHeight);

int PVR_RCD_GetFilename(int index, char *name, int name_len);
int PVR_DeleteFile(char *filename);
int PVR_RCD_CleanFile(char *basename);

///should be called before PVR_RecordStop, becasue PVR_RecordStop will release all resource ,include files
////cleanFlag:  0-not clean  1-clean
int PVR_RCD_Set_CleanFlag(char cleanFlag);

 
#endif
