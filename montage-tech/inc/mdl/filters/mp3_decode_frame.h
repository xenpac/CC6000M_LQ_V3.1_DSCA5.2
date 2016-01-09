/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MPEGAUDIOFRAME_H__
#define __MPEGAUDIOFRAME_H__


/*!
  ID3v1_GENRE_MAX
  */
#define ID3V1_GENRE_MAX 125

/*!
 * big endian or small endian
 */
//#define BIG_ENDIAN

/*!
  FOURCCBE
  */
#define FOURCCBE(a,b,c,d) \
  (((u8)(a) << 24) | ((u8)(b) << 16) | \
    ((u8)(c) << 8) | ((u8)(d) << 0))

/*!
  FOURCCLE
  */
#define FOURCCLE(a,b,c,d) \
  (((u8)(a) << 0) | ((u8)(b) << 8) | \
    ((u8)(c) << 16) | ((u8)(d)<< 24))



/*!
  MPEGAudioRet::mErrCode
  */
typedef enum tag_mpeg_audio_ret{
      MPEG_AUDIO_OK = 0,
      MPEG_AUDIO_NEED_MORE = -99,
      MPEG_AUDIO_ERR       = -199,
}mpeg_audio_ret_t;

/*!
  MP3_FRAME_HEAD_SIZE
  */
#define  MP3_FRAME_HEAD_SIZE 4


/*!
  the return value for mp3_syncframeinfo_parse
  */
typedef struct 
{
    /*!
      ErrorCode
      */
    s32 mErrCode;
    
    /*!
      NextPos
      */    
    u32 mNextPos;
    
} mpegaudioret_t;

/*!
  _mp3_getframe_state 
  */
typedef struct
{
   /*!
     first_frame
     */
   BOOL   first_frame;  
   
   /*!
     state
     */
   u8   state;
       
   /*!
     sideinfo_size
     */    
   u32   sideinfo_size; 
    
   /*!  
     xing_head
     */ 
   BOOL   xing_head;
    
   /*!
     left_len
     */ 
   u32   left_len;
   
   /*!
     head_pos
     */
   s32   head_pos;
   
   /*!
     need_len
     */
   u32   need_len;  

}mp3_getframe_state_t;

/*!
  parse_state_t
  */
typedef enum
{
  NOT_OK   = 1000,
  IDLE     = 1001,
  ID3V2_OK = 1002,
  FRAME_OK = 1003,
  ID3V1_OK = 1004,
  XING_OK  = 1005,
  ALL_OK   = 1006
    
}parse_state_t;

/*!
  MP3_TILE_LEN
  */
#define MP3_TILE_LEN    128
/*!
  MP3_AUTHOR_LEN
  */
#define MP3_AUTHOR_LEN  64
/*!
  MP3_RIGHT_LEN
  */
#define MP3_RIGHT_LEN   128
/*!
  MP3_COMMENT_LEN
  */
#define MP3_COMMENT_LEN 128
/*!
  MP3_ALBUM_LEN
  */
#define MP3_ALBUM_LEN   128
/*!
  MP3_GENRE_LEN
  */
#define MP3_GENRE_LEN   32
/*!
  MP3_LRC_LEN
  */
#define MP3_LRC_LEN     2048
/*!
  MP3_JPG_OFFSET
  */
#define MP3_JPG_OFFSET  18

/*!
  the info of ID3v1 or ID3v2
  */
typedef struct
{
    /*!
      filename
      */
    //u8 filename[1024];  
    
    /*! 
      title    
      */     
    u8 title[MP3_TILE_LEN];   
    
    /*! 
      author  
      */      
    u8 author[MP3_AUTHOR_LEN];  
    
     /*! 
       copyright  
       */      
    u8 copyright[MP3_RIGHT_LEN];   
   
    /*!
      comment      
      */    
    u8 comment[MP3_COMMENT_LEN];     
    
    /*!  
      album  
      */   
    u8 album[MP3_ALBUM_LEN]; 
    
    /*! 
      year  
      */        
    u32  year; 
      
    /*! 
      track
      */            
    u32  track; 
    
    /*!  
      genre      
      */
    u8 genre[MP3_GENRE_LEN];
    /*!  
      lyrics      
      */
     u8 lyrics[MP3_LRC_LEN];
   /*! 
     0:"GBK", 
     1:"UTF-16", 
     2:"UTF-16BE", 
     3:"UTF-8"              
     */  
     u8 encode_type;
}id3info_t;

/*!
  The Frame head information
  */
typedef struct 
{
    /*!
      MPEG-1.0: 10
      MPEG-2.0: 20
      MPEG-2.5: 25
      invalid : other
      */
    u32 mMPEGVersion : 6;
    
    /*!
      Layer I  : 1
      Layer II : 2
      Layer III: 3
      invalid  : other
      */
    u8 mLayer : 3;
    
    /*!
      in kbits/s
      */
    u32 mBitrate : 10;
    
    /*!
      in Bytes
      */
    u32 mPaddingSize : 4;
    
    
    /*!
      Channel mode
     
      Joint Stereo (Stereo) - 0
      Single channel (Mono) - 1
      Dual channel (Two mono channels) - 2
      Stereo - 3
      */
    u32 mChannelMode : 3;
    
    /*!
      Mode extension, Only used in Joint Stereo Channel mode.
      not process
      */
    u32 mExtensionMode : 3;
    
    /*!
      Emphasis:
      The emphasis indication is here to tell the decoder that the file must be 
      de-emphasized, that means the decoder must 're-equalize' the sound after 
      a Dolby-like noise suppression. It is rarely used.
      
      0 - none
      1 - 50/15 ms
      2 - reserved (invalid)
      3 - CCIT J.17
      */
    u32 mEmphasis : 3;

    
    /*!
      in Hz
      */
    u32 mSamplerate : 17;
    

    /*!
      Protection off: 0
      Protection on : 1
      */
    u32 mProtection : 2;
    

    /*!
      Copyright bit, only informative
      */
    u32 mCopyrightBit : 2;
    
    /*!
      Original bit, only informative
      */
    u32 mOriginalBit : 2;
    
    /*!
      the size of SideInfo(Layer III), in Bytes
      */
    u32 mSideInfoSize : 7;   
    
    /*!
      Samples per frame
      */
    u32 mSamplesPerFrame : 12;
    
    /*!
      0 - CBR
      1 - CBR(INFO)
      2 - VBR(XING)
      3 - VBR(VBRI)
      */
    u32 mBitrateType : 3;
    
    /*!
      2 Bytes
      */
    u32 mCRCValue;

}frameheadinfo_t;


/*!
  XING, INFO or VBRI Header 
  */
typedef struct
{
    /*!
      mTotalFrames
      */
    u32 mTotalFrames;
    
    /*!
      mTotalBytes of file besides id3 tag.
      */
    u32 mTotalBytes;
    
    /*!
      Quality indicator     
      From 0 - worst quality to 100 - best quality
      */
    u32 mQuality;


    // only for VBRI Header 
    
    /*!
      Size per table entry in bytes (max 4)
      */
    u32  mEntrySize : 4;
    
    
    /*!
      Number of entries within TOC table
      */
    u32  mEntriesNumInTOCTable;
    
    /*!
      Scale factor of TOC table entries
      */
    u32  mTOCTableFactor;
    
    /*!
      VBRI version
      */
    u32 mVBRIVersionID;
    
    /*!
      Frames per table entry
      */
    u32 mFramesNumPerTable;
    
    /*!
      VBRI delay     
      Delay as Big-Endian float.
      */
    u8 mVBRIDelay[2]; 
    
    /*!
      Frame size
      */
    u32 mFrameSize;


}vbrinfo_t;

/*!
 * the mpeg audio info struct.
 */
typedef struct 
{

  /*!
    the file is or not the mp3 file
    */
  BOOL is_mp3file;
  
  /*!
    the status of statemachine.
    */
  u32 status;
  
  /*!
    more buffer`s position and length
    */
  u32 need_bufpos; 
  
  /*!
    need_buflen
    */
  u32 need_buflen;

  /*!
    needmorebuf
    */
  BOOL needmorebuf;
  
  /*!
    filebuf_pos
    */
  u32  filebuf_pos;

  /*!
    id3v2 length 
    */
  u32 id3v2_len;

  /*!
    if don`t find the message,continue find
    */
  u32 findcount;

  /*!
    the filesize is the size reduce the ID3v1 & ID3v2 
    */
  u32 mFileSize; 

    /*!
      id3v1 struct info
      */
  id3info_t id3info;
       
   /*!
     the struct of frame head information
     */ 
   frameheadinfo_t frameinfo;
    
   /*!
     the struct of vbr information
     */
  vbrinfo_t vbrinfo;

  /*!
    Playtime
    */
  u32 mDuration;
  /*!
    picture buf
    */
  void *p_pic_buf;
  /*!
    pic buf len
    */
  u32   pbuf_len;
    
    
} mpegaudioframeinfo_t;


/*!
  find the first mpeg audio frame info.
  
  @param buf         data area
  @param bufSize     the data size
  @param info[out]   MPEG audio info£¬can be NULL
  @param firstFrame  is the first frame?
  
  @ret  if MPEGAudioRet.mErrCode is MPEG_AUDIO_OK£¬it means parse OK£»or else it should be more data¡£
        if parse ok£¬MPEGAudioRet.mNextPos means the next frame offset£»or else means input the more  
        buffer`s offset¡£
  */
mpegaudioret_t mp3_syncframeinfo_parse(
        unsigned char *buf, 
        u32 bufSize,
        mpegaudioframeinfo_t *info, 
        u32 firstFrame);

/*!
  mp3_vbrxinghdr_parse
  */
u32 mp3_vbrxinghdr_parse(unsigned char *p_xingHdr, u32 bufSize,
        mpegaudioframeinfo_t *p_info);
 
/*!
  mp3_vbrvbrihdr_parse
  */
u32 mp3_vbrvbrihdr_parse(unsigned char *p_vbriHdr, u32 bufSize,
        mpegaudioframeinfo_t *p_info);

/*!
  mp3_id3v1_parse
  */
u32 mp3_id3v1_parse(mpegaudioframeinfo_t *p_s,u8 *p_buf);

/*!
  mp3_id3v2_culculatelen
  */
u32 mp3_id3v2_culculatelen(unsigned char *p_buf,u32 len);

/*!
  mp3_id3v2_match
  */
u32 mp3_id3v2_match(unsigned char *p_buf);       

/*!
  mp3_playtime_culculate 
  */
u32 mp3_playtime_culculate(mpegaudioframeinfo_t *p_mp3_info);

/*!
  mp3_head_parse
  */
u32 mp3_head_parse(u8 *p_buf,u32 buflen,mpegaudioframeinfo_t *p_mp3info);

/*!
  mp3_getframe_buffer 
  */
s32 mp3_getframe_buffer(u8 *p_buf,u32 bufLen,mp3_getframe_state_t *check_state);

/*!
  mp3_parse_info
  */
u32 mp3_parse_info(u16 *p_name, mpegaudioframeinfo_t *p_mp3_info);

/*!
  attach buffer, for memory share. By default, pic module will the malloc buffer.
  */
RET_CODE mul_mp3_attach_buffer(void *p_buffer, u32 buffer_size);

/*!
  detach buffer.
  */
RET_CODE mul_mp3_detach_buffer();

/*!
  mp3 malloc buffer.
  */
void *mp3_frame_malloc(u32 size);

/*!
  mp3 free buffer.
  */
void mp3_frame_free(void *p_buffer);


#endif   //MPEGAUDIOFRAME_H_ 
