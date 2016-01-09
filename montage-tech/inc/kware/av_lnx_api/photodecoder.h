/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef  __PHOTODECODER_H_
#define  __PHOTODECODER_H_

/*
 Coordinate. Negative value is not allowed.
 */
typedef unsigned short 	GD_COOR;

/**
 Point: X and Y
 */
typedef struct {
/*!
  xxxxxxxx
  */
	GD_COOR		x;	
/*!
  xxxxxxxx
  */
	GD_COOR		y;
} GD_POINT;

/**
 Rectangle: X, Y, Height and Width.
 */
typedef struct {
/*!
  xxxxxxxx
  */
	GD_COOR		x;	///< x
	/*!
  xxxxxxxx
  */
	GD_COOR		y;	///< y
	/*!
  xxxxxxxx
  */
	unsigned short		w;	///< width
	/*!
  xxxxxxxx
  */
	unsigned short		h;	///< height
} GD_RECT;

/**
 Size: Height and Width
*/
typedef struct {
/*!
  xxxxxxxx
  */
	unsigned short	w;	///< width
	/*!
  xxxxxxxx
  */
	unsigned short	h;	///< height
} GD_SIZE;

/*!
  xxxxxxxx
  */
typedef enum _JPEG_ROTATE{
/*!
  xxxxxxxx
  */
	JPEG_NO_ROT = 0,
	/*!
  xxxxxxxx
  */
	JPEG_ROT_90_CW,
	/*!
  xxxxxxxx
  */
	JPEG_ROT_180_CW,
	/*!
  xxxxxxxx
  */
	JPEG_ROT_270_CW
} JPEG_ROTATE;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_ERROR_CODE{
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_OK,
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_FAIL,
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_UNSUPPORT,
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_USERBREAK,
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_PARAM,
	/*!
  xxxxxxxx
  */
	PHOTO_ERR_NOMEM,
} PHOTO_ERROR_CODE;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_ZOOM_FACTOR {
/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_FIT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_5PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_10PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_15PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_20PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_30PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_50PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_70PECENT,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_1X,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_1_5X,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_2X,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_3X,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_4X,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_MAX = PHOTO_ZOOM_4X,
} PHOTO_ZOOM_FACTOR;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_ROTATE_FACTOR {
/*!
  xxxxxxxx
  */
	PHOTO_ROTATE_0 = JPEG_NO_ROT,
	/*!
  xxxxxxxx
  */
	PHOTO_ROTATE_90,
	/*!
  xxxxxxxx
  */
	PHOTO_ROTATE_180,
	/*!
  xxxxxxxx
  */
	PHOTO_ROTATE_270,
} PHOTO_ROTATE_FACTOR;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_DECODE_MODE {
/*!
  xxxxxxxx
  */
	PHOTO_FULLSCREEN_MODE,		// avp_jpeg.c define 0: fullscreen, if you change mode, you need to fix avp_jpeg.c, too.
/*!
  xxxxxxxx
  */
PHOTO_THUMBNAIL_MODE,
} PHOTO_DECODE_MODE;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_DECODE_PIC {
/*!
  xxxxxxxx
  */
	PHOTO_DECODE_THUMBNAIL_FISRT,
	/*!
  xxxxxxxx
  */
	PHOTO_DECODE_FULL_PICTURE_FISRT,
} PHOTO_DECODE_PIC;
/*!
  xxxxxxxx
  */
typedef enum _PHOTO_DISPLAY_MODE {
/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_TO_NONE,
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_TO_FIT,	
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_TO_FULL,
} PHOTO_DISPLAY_MODE;
/*!
  xxxxxxxx
  */
typedef struct _PHOTO_ZOOM_MOVE {
/*!
  xxxxxxxx
  */
	unsigned char	moveX;
/*!
  xxxxxxxx
  */
	unsigned char	moveY;
} PHOTO_ZOOM_MOVE;
/*!
  xxxxxxxx
  */
typedef enum _JPEG_DECODE_MODULE {
/*!
  xxxxxxxx
  */
	JPEG_HW_DECODE_MODE,	//CxM
	/*!
  xxxxxxxx
  */
	JPEG_SW_DECODE_MODE,	//FreeImage
} JPEG_DECODE_MODULE;
/*!
  xxxxxxxx
  */
typedef struct _PHOTO_PLAYER	PHOTO_PLAYER;
/*!
  xxxxxxxx
  */
typedef struct _PHOTO_INFO {
/*!
  xxxxxxxx
  */
	unsigned char	*maker;
/*!
  xxxxxxxx
  */
	unsigned char	*model;
/*!
  xxxxxxxx
  */
	unsigned char	*dateTime;
/*!
  xxxxxxxx
  */
	unsigned int	width;
/*!
  xxxxxxxx
  */
	unsigned int 	height;
/*!
  xxxxxxxx
  */
	unsigned char	*exposureTime;
/*!
  xxxxxxxx
  */
	unsigned int	fNumber;
/*!
  xxxxxxxx
  */
	unsigned char	*isoSpeed;
/*!
  xxxxxxxx
  */
	unsigned char	*exposureValue;
/*!
  xxxxxxxx
  */
	unsigned char	*flash;
/*!
  xxxxxxxx
  */
	unsigned char	*exposureMode;
/*!
  xxxxxxxx
  */
	unsigned char	*whiteBalance;
} PHOTO_INFO;
/*!
  xxxxxxxx
  */
typedef struct _PHOTO_DECODE_PARAM {
/*!
  xxxxxxxx
  */
	PHOTO_DECODE_MODE	decodeMode;		///	0:Full Screen, 1:Thumbnail
	/*!
  xxxxxxxx
  */
	PHOTO_DECODE_PIC	decodePic;		///  0: Deocde Thumbnail Fisrt, 1: Decode Full Picture  Fisrt
	/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_FACTOR	zoomFactor;		///	0:Zoom Fit Screen
		/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_FACTOR	zoomFactorOri;	///   Original Zoom Factor When Fit
		/*!
  xxxxxxxx
  */
	PHOTO_ZOOM_MOVE		zoomMove;		/// from -5 to +5	
		/*!
  xxxxxxxx
  */
	PHOTO_ROTATE_FACTOR	rotateFactor;	///	0:No Rotate
		/*!
  xxxxxxxx
  */
	PHOTO_DISPLAY_MODE	displayMode;	///	0:None, 1:Zoom to Fit, 2:Zoom to Full
		/*!
  xxxxxxxx
  */
	GD_RECT				decodeRect;		/// Which Range to Decode
		/*!
  xxxxxxxx
  */
	GD_RECT				dispRect;		/// Display RECT
		/*!
  xxxxxxxx
  */
	unsigned int				overScanRatio;	/// 0: No OveScan, N: n% disapear
		/*!
  xxxxxxxx
  */
	unsigned int				netRetryCount;	/// 0: No Retry, Max is 3 times
		/*!
  xxxxxxxx
  */
	int index;/// page index ,for GIF
		/*!
  xxxxxxxx
  */
	JPEG_DECODE_MODULE jpegDecodeModule;	
			/*!
  xxxxxxxx
  */
	unsigned char *p_flSrcBuffer;
				/*!
  xxxxxxxx
  */
	unsigned int flSrcBufferSize;
} PHOTO_DECODE_PARAM;
	/*!
  xxxxxxxx
  */
typedef struct _PHOTO_DECODE_RESULT {
/*!
  xxxxxxxx
  */
	void 			*dib;			/// If Non-Jpeg, We Need to Keep Dib for Zoom Rotate
/*!
  xxxxxxxx
  */
	unsigned int				curImage;		/// Current Pixel Data
/*!
  xxxxxxxx
  */
	unsigned char			       	*fileName;		/// Decode File Name
/*!
  xxxxxxxx
  */
	GD_RECT				imageRect;		/// Current Decoded Image Rectangle, With Padding
/*!
  xxxxxxxx
  */
	GD_RECT				realRect;		/// No Padding
/*!
  xxxxxxxx
  */
	GD_SIZE				origSize;		/// Original Image Rectangle
/*!
  xxxxxxxx
  */
	PHOTO_DECODE_PARAM	curDecodeState;	/// Current Decode State
} PHOTO_DECODE_RESULT;

	/*!
  xxxxxxxx
  */
struct _PHOTO_PLAYER {
/*!
  xxxxxxxx
  */
       unsigned char *                  p_map_mem_start;
/*!
  xxxxxxxx
  */
	unsigned int                       map_mem_len;
/*!
  xxxxxxxx
  */
	int                                     dev_fd;
/*!
  xxxxxxxx
  */
	PHOTO_DECODE_RESULT	decodeResult;
	// TODO:  any field you want photo_player help to maintain !!!
};

/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_Init(PHOTO_PLAYER **ppPlayer);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_DeInit(PHOTO_PLAYER **ppPlayer);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_LoadImage(PHOTO_PLAYER *pPlayer, unsigned char *fileName, PHOTO_DECODE_PARAM *decodeParam);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_FreeImage(PHOTO_PLAYER *pPlayer);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_GetThumbnail(PHOTO_PLAYER *pPlayer, unsigned char *fileName, PHOTO_DECODE_PARAM *decodeParam);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_GetThumbnailWithVersion(PHOTO_PLAYER *pPlayer, unsigned char *fileName, PHOTO_DECODE_PARAM *decodeParam,int version);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_SetDecodeStatus(unsigned char decode);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_GetPhotoInfoByFile(PHOTO_INFO *photoInfo, unsigned char *fileName);
/*!
  xxxxxxxx
  */
PHOTO_ERROR_CODE	PhotoDecode_FreePhotoInfo(PHOTO_INFO *photoInfo);
/*!
  xxxxxxxx
  */
unsigned char   				PhotoDecode_isInit();
#endif











