/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

#include "lib_util.h"

#include "vfs.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "ifilter.h"

//filter
#include "eva_filter_factory.h"

#include "mp3_decode_frame.h"
#include "mp3_player_filter.h"
#include "class_factory.h"
#include "lib_memp.h"
#include "err_check_def.h"

/*!
  BIG_ENDIAN OR LITTLE_ENDIAN
  */
#define BIG_ENDIAN
#ifdef  BIG_ENDIAN
/*!
  FOURCC
  */
#define FOURCC(a,b,c,d) FOURCCBE(a,b,c,d)
#else
/*!
  FOURCC
  */
#define FOURCC(a,b,c,d) FOURCCLE(a,b,c,d)
#endif


/*!
 * the table of bitrate (kbits/s)
 */
const u16 BitrateTable[2][3][15] =
{
    {
        {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448},
        {0,32,48,56,64 ,80 ,96 ,112,128,160,192,224,256,320,384},
        {0,32,40,48,56 ,64 ,80 ,96 ,112,128,160,192,224,256,320}
    },
    {
        {0,32,48,56,64 ,80 ,96 ,112,128,144,160,176,192,224,256},
        {0,8 ,16,24,32 ,40 ,48 ,56 ,64 ,80 ,96 ,112,128,144,160},
        {0,8 ,16,24,32 ,40 ,48 ,56 ,64 ,80 ,96 ,112,128,144,160}
    }
};


/*!
 * the table of samplerate
 */
const  u32 SamplerateTable[3][3] =
{
    {44100,48000,32000 }, // MPEG-1
    {22050,24000,16000 }, // MPEG-2
    {11025,12000,8000  }  // MPEG-2.5
};

/*!
  mp3 id3v1 genre
  */
static const char *id3v1_genre_str[ID3V1_GENRE_MAX + 1] = {
        "Blues","Classic Rock","Country","Dance","Disco",
        "Funk","Grunge","Hip-Hop","Jazz","Metal","New Age",
        "Oldies","Other","Pop","R&B","Rap","Reggae","Rock",
        "Techno","Industrial","Alternative","Ska","Death Metal",
        "Pranks","Soundtrack","Euro-Techno","Ambient","Trip-Hop",
        "Vocal","Jazz+Funk","Fusion","Trance","Classical",
        "Instrumental","Acid","House","Game","Sound Clip",
        "Gospel","Noise","AlternRock","Bass","Soul","Punk",
        "Space","Meditative","Instrumental Pop","Instrumental Rock",
        "Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic",
        "Pop-Folk","Eurodance","Dream","Southern Rock","Comedy",
        "Cult","Gangsta","Top 40","Christian Rap","Pop/Funk","Jungle",
        "Native American","Cabaret","New Wave","Psychadelic","Rave",
        "Showtunes","Trailer","Lo-Fi","Tribal","Acid Punk","Acid Jazz",
        "Polka","Retro","Musical","Rock & Roll","Hard Rock","Folk",
        "Folk-Rock","National Folk","Swing","Fast Fusion","Bebob",
        "Latin","Revival","Celtic","Bluegrass","Avantgarde",
        "Gothic Rock","Progressive Rock","Psychedelic Rock",
        "Symphonic Rock","Slow Rock","Big Band","Chorus","Easy Listening",
        "Acoustic","Humour","Speech","Chanson","Opera","Chamber Music",
        "Sonata","Symphony","Booty Bass","Primus","Porn Groove",
        "Satire","Slow Jam","Club","Tango","Samba","Folklore",
        "Ballad","Power Ballad","Rhythmic Soul","Freestyle","Duet",
        "Punk Rock","Drum Solo","A capella","Euro-House","Dance Hall"
};


/*!
  * mpeg audio frame parse
  */
static s32 _parseMpegFrameHdr(
        unsigned char * hdrBuf,
        u32 bufSize,
        mpegaudioframeinfo_t *info,
        u32 firstFrame);

/*!
  * calculate the frame_size
  */
//static u32 _calculateFrame(mpegaudioframeinfo_t *info);


/*!
  * big-endian to local endian
  */
static u32 _bigendian2Local(unsigned char *valueAddr, u32 len);

/*!
  * little-endian to local endian
  */
//static u32 _litendian2Local(unsigned char *valueAddr, u32 len);

/*!
  * swap endian:
  *
  * little-endian -> big-endian
  * big-endian -> little-endian
  */
static void _exchangeByteEndian(unsigned char *valueAddr, u32 len);


typedef struct
{
  /*!
    lib memp
    */
  lib_memp_t p_heap;    
  /*!
    use memory heap
    */
  BOOL use_memp;  
}mp3_frame_priv_t;

/*!
  mp3 malloc buffer 
  */
void *mp3_frame_malloc(u32 size)
{
  mp3_frame_priv_t *p_mp3_priv = NULL;
  void *p_buf = NULL;

  p_mp3_priv = class_get_handle_by_id(MP3_FRAME_CLASS_ID);
  if(p_mp3_priv == NULL)
  {
    p_buf = mtos_malloc(size);
    return p_buf;
  }
  
  if(p_mp3_priv->use_memp)
  {
    p_buf = lib_memp_align_alloc(&p_mp3_priv->p_heap, size, 8);
    return p_buf;
  }
  else
  {
    p_buf = mtos_malloc(size);
    return p_buf;
  }
}

/*!
  mp3 free buffer
  */
void mp3_frame_free(void *p_buffer)
{
  mp3_frame_priv_t *p_mp3_priv = NULL;

  p_mp3_priv = class_get_handle_by_id(MP3_FRAME_CLASS_ID);
  if(p_mp3_priv == NULL)
  {
    mtos_free(p_buffer);
    return ;
  }
  
  if(p_mp3_priv->use_memp)
  {
    lib_memp_align_free(&p_mp3_priv->p_heap,  p_buffer);
    return;
  }
  else
  {
    mtos_free(p_buffer);
    return ;
  }
}

/*!
 * _av_strlcpy
 */
u32 _av_strlcpy(u8 *p_dst, const u8 *p_src, u32 size)
{
    u32 len = 0;
    while (++len < size && *p_src)
        *p_dst++ = *p_src++;
    if (len <= size)
        *p_dst = 0;
    return len + strlen(p_src) - 1;
}

/*!
 * _id3v1_get_string
 */
static void _id3v1_get_string(u8 *p_str, u32 str_size,
                              const u8 *p_buf, u32 buf_size)
{
    u32 i = 0;
    u8 c = 0;
    u8 *p_q = NULL;

    p_q = p_str;
    for(i = 0; i < buf_size; i++) {
        c = p_buf[i];
        if(c == '\0')
            break;
        if((u32)(p_q - p_str) >= (str_size - 1))
            break;
        *p_q++ = c;
    }
    *p_q = '\0';

}

/*!
 * mp3_id3v1_parse
 */
u32 mp3_id3v1_parse(mpegaudioframeinfo_t *p_s,u8 *p_buf)
{
    //u8 str[5];
    u32 genre = 0;

    if (!(p_buf[0] == 'T' &&
          p_buf[1] == 'A' &&
          p_buf[2] == 'G'))
    return 0;

    _id3v1_get_string(p_s->id3info.title, sizeof(p_s->id3info.title), p_buf + 3, 30);
    _id3v1_get_string(p_s->id3info.author, sizeof(p_s->id3info.author), p_buf + 33, 30);
    _id3v1_get_string(p_s->id3info.album, sizeof(p_s->id3info.album), p_buf + 63, 30);
    //id3v1_get_string(str, sizeof(str), buf + 93, 4);
    //s->year = atoi(str);
    _id3v1_get_string(p_s->id3info.comment, sizeof(p_s->id3info.comment), p_buf + 97, 30);
    if (p_buf[125] == 0 && p_buf[126] != 0)
        p_s->id3info.track = p_buf[126];
    genre = p_buf[127];
    if (genre <= 125)
        _av_strlcpy(p_s->id3info.genre, id3v1_genre_str[genre], sizeof(p_s->id3info.genre));

    return 1;
}


/*!
 * mp3_id3v2_match
 */
u32 mp3_id3v2_match(u8 *p_buf)
{
    if(p_buf == NULL)
    return 0;

    return (p_buf[0] == 'I' &&
            p_buf[1] == 'D' &&
            p_buf[2] == '3' &&
            p_buf[3] != 0xff &&
            p_buf[4] != 0xff); /*&&
            (p_buf[6] & 0x80) == 0 &&
            (p_buf[7] & 0x80) == 0 &&
            (p_buf[8] & 0x80) == 0 &&
            (p_buf[9] & 0x80) == 0); */
}

/*!
 * mp3_id3v2_culculatelen
 */
u32 mp3_id3v2_culculatelen(unsigned char *p_buf,u32 len)
{
   if(p_buf == NULL)
     return 0;

   return ((p_buf[6] & 0x7f) << 21) |
          ((p_buf[7] & 0x7f) << 14) |
          ((p_buf[8] & 0x7f) << 7) |
          (p_buf[9] & 0x7f);
}


u32 makeInt(const char *p_b, u32 off, u32 len) {
         u32 i = 0, ret = p_b[off] & 0xff;
         for (i = 1; i < len; i++) {
             ret <<= 8;
             ret |= p_b[off + i] & 0xff;
         }
         return ret;
}

u32 getText(mpegaudioframeinfo_t *p_info, const char *p_buf, u32 off, u32 max_size, u16 Version)
{
   u32 fsize = 0, len = 0;
   u32 id_part = 0,frame_header = 0;
   u8 enc = 0;           //encode_num
   u8 head[4] = {0};
   u32 Id = 0;


   id_part = 4, frame_header = 10;
   if(Version == 2)
   {
       id_part = 3;
       frame_header = 6;
   }

   if(off > 0)
   {
      memcpy(head,p_buf + off,id_part);
      off += id_part;

      fsize = len = makeInt(p_buf, off, id_part);
      if(fsize > max_size)
             fsize = 0;

      off += id_part;     // frame size = frame id bytes
      if (Version > 2)
         off += 2;       // flag: 2 bytes

      enc = p_buf[off];
      p_info->id3info.encode_type = enc;

      len--;              // Text encoding: 1 byte
      off++;              // Text encoding: 1 byte
      if (len <= 0 || off + len > max_size || enc < 0 || enc >= 4) //TEXT_ENCODING.length)
         return fsize + frame_header;

      Id = FOURCC(head[0],head[1],head[2],head[3]);

      switch(Id)
      {
         case FOURCC('T','I','T','2'):   //title
           case FOURCC(0,'T','T','2'):
            if(len < MP3_TILE_LEN)
            {
               memcpy(p_info->id3info.title, p_buf + off, len);
            }
            break;

         case FOURCC('T','P','E','1'):    //author
         case FOURCC(0,'T','P','1'):
            if(len < MP3_AUTHOR_LEN)
            {
               memcpy(p_info->id3info.author, p_buf + off, len);
            }
            break;

         case FOURCC('T','A','L','B'):    //album
         case FOURCC(0,'T','A','L'):
            if(len < MP3_ALBUM_LEN)
            {
               memcpy(p_info->id3info.album, p_buf + off, len);
            }
            break;

         case FOURCC('T','C','O','N'):    //genre
         case FOURCC(0,'T','C','O'):
            if(len < MP3_GENRE_LEN)
            {
               memcpy(p_info->id3info.genre, p_buf + off, len);
            }
            break;

         case FOURCC('T','C','O','P'):    //copyright
         case FOURCC(0,'T','C','R'):
            if(len < MP3_RIGHT_LEN)
            {
               memcpy(p_info->id3info.copyright, p_buf + off, len);
            }
            break;
         case FOURCC('U', 'S', 'L', 'T'):
            if(len < MP3_LRC_LEN)
            {
               memcpy(p_info->id3info.lyrics,p_buf + off,len);
            }
            break;
         case FOURCC('A','P','I','C'): //offset: MP3_JPG_OFFSET  18

                 //p_info->p_pic_buf = (void *)&p_buf[off];
                 //p_info->pbuf_len = len;
 /*
                  if(len > MP3_JPG_OFFSET)
                  {
                      p_info->p_pic_buf = mtos_malloc(len - MP3_JPG_OFFSET); //should free
                      CHECK_FAIL_RET_ZERO(p_info->p_pic_buf != NULL);
                      memset(p_info->p_pic_buf, 0, len - MP3_JPG_OFFSET);

                      memcpy(p_info->p_pic_buf, (void *)&p_buf[off + MP3_JPG_OFFSET],
                             len - MP3_JPG_OFFSET);
                      p_info->pbuf_len = len - MP3_JPG_OFFSET;
                  }
                  else
                  {
                      p_info->p_pic_buf = NULL;
                      p_info->pbuf_len = 0;
                  }
*/
              {

               u32 offset = 0;
               const char *p_buffer = NULL;

               p_buffer = p_buf + off;
               while((p_buffer[0] != 0x00) && (offset < len))  //读到MIME type字符串结尾
               {
                  p_buffer ++;
                  offset ++;
               }

               if(offset >= len)
               {
                  //
                  break;
               }

               if((len > offset) && ((len - offset) > 2))
               {
                  p_buffer = p_buffer + 2;                   //跳过picture type
                  offset = offset + 2;
               }
               else
               {
                  //
                  break;
               }

               if((enc == 0x01) || (enc == 0x02))  //UTF-16 coding
               {
                 unsigned short t[1] = {0};
                 if((len > offset) && ((len - offset) > 2))
                 {
                    memcpy(t, p_buffer, 2);
                    p_buffer += 2;
                    offset += 2;
                 }
                 else
                 {
                   //
                   break;
                 }

                 while((t[0] != 0) && (offset < len))
                 {
                   if((len > offset) && ((len - offset) > 2))
                   {
                      memcpy(t, p_buffer, 2);
                      p_buffer += 2;
                      offset += 2;
                   }
                   else
                   {
                     //
                     break;
                   }
                 }

                 if(offset >= len)
                 {
                    //
                    break;
                 }
               }
               else
               {
                  while((p_buffer[0] != 0x00) && (offset < len)) //读到Description结尾
                  {
                     p_buffer ++;
                     offset ++;
                  }

                  if(offset >= len)
                  {
                     break;
                  }

                  p_buffer ++; //
                  offset ++;
               }

               if(len > offset)
               {
                   p_info->p_pic_buf = mp3_frame_malloc(len - offset); //should free
                   CHECK_FAIL_RET_ZERO(p_info->p_pic_buf != NULL);
                   memset(p_info->p_pic_buf, 0, len - offset);

                   memcpy(p_info->p_pic_buf, p_buffer, len - offset);
                   p_info->pbuf_len = len - offset;
               }

               break;
             }

         case FOURCC('T','R','C','K'):    //trck
         case FOURCC(0,'T','R','K'):
            p_info->id3info.track = p_buf[off];
            break;

         default:
           break;

      }

   }

  return fsize + frame_header;

}


u32 synchSafeInt(const char *p_b, u32 off) {
         u32 i = (p_b[off] & 0x7f) << 21;
         i |= (p_b[off + 1] & 0x7f) << 14;
         i |= (p_b[off + 2] & 0x7f) << 7;
         i |=  p_b[off + 3] & 0x7f;
         return i;
     }


/*!
 * mp3_id3v2_parse
 */
u32 mp3_id3v2_parse(mpegaudioframeinfo_t *p_info, const char *p_buf, u32 buf_len, u32 off)
{
   BOOL ID3v2Footer = 0;
   u16 Version = 0;
   u32 ExHeaderSize = 0;
   u32 size = 0;
   u32 max_size = 0,pos = 0;


   Version = p_buf[off + 3] & 0xff;

   if(Version > 2 && (p_buf[off + 5] & 0x40) != 0)
   ExHeaderSize = 1;        //设置为1表示有扩展头

   ID3v2Footer = (p_buf[off + 5] & 0x10) != 0;
   size = synchSafeInt(p_buf, (off + 6));
   size += 10;                 // ID3 header:10bytes

   off = off + 10;
   max_size = size;
   pos = off;

   if(ExHeaderSize == 1) {
   ExHeaderSize = synchSafeInt(p_buf, off);
   pos += ExHeaderSize;
  }

  if(ID3v2Footer)
    max_size -= 10;

  while(pos < max_size)
    pos += getText(p_info, p_buf, pos, max_size, Version);

  return (pos - off);

}

/*!
 *
 */
mpegaudioret_t mp3_syncframeinfo_parse(
        unsigned char *p_buf,
        u32 bufSize,
        mpegaudioframeinfo_t *p_info,
        u32 firstFrame)
{
    mpegaudioret_t ret = {0};
    u32 i = 0;
    u32 loopSize = 0;
    mpegaudioframeinfo_t tmp = {0};
    mpegaudioframeinfo_t *p_FrameInfo = NULL;

    ret.mErrCode = MPEG_AUDIO_NEED_MORE;
    ret.mNextPos = 0;

    p_FrameInfo = p_info;
    if(!p_FrameInfo)
        p_FrameInfo = &tmp;

    loopSize = bufSize - 2;
    for(; i < loopSize; i++)
    {
        // frame sign: 1111 1111 111x xxxxb
        //if((p_buf[i]==0xff) && ((p_buf[i+1]&0xe0)==0xe0)&& ((p_buf[i+2]&0xf0) != 0xF0))
        if((p_buf[i] == 0xFF) && ((p_buf[i + 1] & 0xE0) == 0xE0)
          && ((p_buf[i + 2] & 0xF0) != 0xF0) &&(p_buf[i + 1] != 0xFF))
        {

            //memset(p_FrameInfo, 0, sizeof(*p_FrameInfo));
            ret.mErrCode = _parseMpegFrameHdr(p_buf + i, bufSize - i, p_FrameInfo, firstFrame);
            if((MPEG_AUDIO_OK == ret.mErrCode) || (MPEG_AUDIO_NEED_MORE == ret.mErrCode)
                    ||  (MPEG_AUDIO_ERR == ret.mErrCode))
            {

                break;
            }

        }
        else if((i == (loopSize - 1)) && (p_buf[i + 1] != 0xff))
        {
            i++;
        }
    }

    ret.mNextPos = i;

    //if(i > 0 && ret.mErrCode != MPEG_AUDIO_OK)
    //    memset(p_FrameInfo, 0, sizeof(mpegaudioframeinfo_t));

    return ret;

}


/*!
 * parseMpegFrameHdr
 */
s32 _parseMpegFrameHdr(
        unsigned char *p_hdrBuf,
        u32 bufSize,
        mpegaudioframeinfo_t *p_info,
        u32 firstFrame)
{

    unsigned char index_I = 0;
  unsigned char index_II = 0;

    if(bufSize < 4) // frame head,at least 4 bytes.
        return MPEG_AUDIO_NEED_MORE;

    // Protection Bit
    p_info->frameinfo.mProtection = (p_hdrBuf[1] & 0x01);
    p_info->frameinfo.mProtection = (0 == p_info->frameinfo.mProtection ? 1 : 0);

    if(p_info->frameinfo.mProtection && bufSize < 6)
    {
        // protected by 16 bit CRC following header
        return MPEG_AUDIO_NEED_MORE;
    }


    // MPEG Version
    p_info->frameinfo.mMPEGVersion = ((p_hdrBuf[1]>>3) & 0x03);
    switch(p_info->frameinfo.mMPEGVersion)
    {
    case 0:
        p_info->frameinfo.mMPEGVersion = 25;
        break;
    case 2:
        p_info->frameinfo.mMPEGVersion = 20;
        break;
    case 3:
        p_info->frameinfo.mMPEGVersion = 10;
        break;
    default:
        p_info->frameinfo.mMPEGVersion = 0;
        return MPEG_AUDIO_ERR;
    };

    // Layer index
    p_info->frameinfo.mLayer = ((p_hdrBuf[1]>>1) & 0x03);
    switch(p_info->frameinfo.mLayer)
    {
    case 1:
        p_info->frameinfo.mLayer = 3;
        break;
    case 2:
        p_info->frameinfo.mLayer = 2;
        break;
    case 3:
        p_info->frameinfo.mLayer = 1;
        break;
    default:
        p_info->frameinfo.mLayer = 0;
        return MPEG_AUDIO_ERR;
    };

    // Bit_Rate
    p_info->frameinfo.mBitrate = ((p_hdrBuf[2]>>4)&0x0f);
    if(p_info->frameinfo.mBitrate == 0x0f)
        return MPEG_AUDIO_ERR;

    index_I = (p_info->frameinfo.mMPEGVersion == 10 ? 0 : 1);
    index_II = p_info->frameinfo.mLayer - 1;
  if((index_I > 1)||(index_II > 2)||(p_info->frameinfo.mBitrate > 14))
       return MPEG_AUDIO_ERR;
    p_info->frameinfo.mBitrate = BitrateTable[index_I][index_II][p_info->frameinfo.mBitrate];

    // samplerate
    p_info->frameinfo.mSamplerate = ((p_hdrBuf[2]>>2)&0x03);
    if(p_info->frameinfo.mSamplerate == 0x03)
        return MPEG_AUDIO_ERR;

    index_I = 2; // MPEG-2.5 by default
    if(p_info->frameinfo.mMPEGVersion == 20)
        index_I = 1;
    else if(p_info->frameinfo.mMPEGVersion == 10)
        index_I = 0;
  if((index_I > 2)||(p_info->frameinfo.mSamplerate > 2))
       return MPEG_AUDIO_ERR;
    p_info->frameinfo.mSamplerate = SamplerateTable[index_I][p_info->frameinfo.mSamplerate];


    // Padding size
    p_info->frameinfo.mPaddingSize = ((p_hdrBuf[2]>>1)&0x01);
    if(p_info->frameinfo.mPaddingSize)
    {
        p_info->frameinfo.mPaddingSize = ((p_info->frameinfo.mLayer == 1) ? 4 : 1);
    }

    // channel mode
    p_info->frameinfo.mChannelMode = ((p_hdrBuf[3]>>6)&0x03);
    switch(p_info->frameinfo.mChannelMode)
    {
    case 0:
        p_info->frameinfo.mChannelMode = 3;
        break;
    case 1:
        p_info->frameinfo.mChannelMode = 0;
        break;
    case 2:
        p_info->frameinfo.mChannelMode = 2;
        break;
    case 3:
    default:
        p_info->frameinfo.mChannelMode = 1;
    };

    // in MPEG-1 Layer II，only some bitrate and some channelmode is permit.
    // in MPEG -2/2.5，Without this limitation.
    if((p_info->frameinfo.mMPEGVersion == 10) && (p_info->frameinfo.mLayer == 2))
    {
        if((32 == p_info->frameinfo.mBitrate)
                || (48 == p_info->frameinfo.mBitrate)
                || (56 == p_info->frameinfo.mBitrate)
                || (80 == p_info->frameinfo.mBitrate))
        {
            if(p_info->frameinfo.mChannelMode != 1)
                return MPEG_AUDIO_ERR;
        }
        else if((224 == p_info->frameinfo.mBitrate)
                || (256 == p_info->frameinfo.mBitrate)
                || (320 == p_info->frameinfo.mBitrate)
                || (384 == p_info->frameinfo.mBitrate))
        {
            if(p_info->frameinfo.mChannelMode == 1)
                return MPEG_AUDIO_ERR;
        }
    }

    //  Extension Mode
    p_info->frameinfo.mExtensionMode = ((p_hdrBuf[3]>>4)&0x03);

    p_info->frameinfo.mCopyrightBit = ((p_hdrBuf[3]>>3)&0x01);

    p_info->frameinfo.mOriginalBit = ((p_hdrBuf[3]>>2)&0x01);


    // The emphasis indication is here to tell the decoder that the file must be
    // de-emphasized, that means the decoder must 're-equalize' the sound after
    // a Dolby-like noise suppression. It is rarely used.
    p_info->frameinfo.mEmphasis = ((p_hdrBuf[3])&0x03);
    if(0x2 == p_info->frameinfo.mEmphasis)
        return MPEG_AUDIO_ERR;

    if(p_info->frameinfo.mProtection)
    {
        // This checksum directly follows the frame header and is a big-endian
        // WORD.
        // So maybe you shoud convert it to little-endian.
        //p_info->frameinfo.mCRCValue = *((u16 *)(p_hdrBuf + 4));
        memcpy(&p_info->frameinfo.mCRCValue, (p_hdrBuf + 4), sizeof(u16));
        _bigendian2Local(
                (unsigned char *)(&(p_info->frameinfo.mCRCValue)),
                sizeof(p_info->frameinfo.mCRCValue));
    }

    // SamplesPerFrame
    p_info->frameinfo.mSamplesPerFrame = 1152;
    if(1 == p_info->frameinfo.mLayer)
    {
        p_info->frameinfo.mSamplesPerFrame = 384;
    }
    else if((p_info->frameinfo.mMPEGVersion != 10) && (3 == p_info->frameinfo.mLayer))
    {
        p_info->frameinfo.mSamplesPerFrame = 576;
    }

    // SideInfo
    p_info->frameinfo.mSideInfoSize = 0;
    if(3 == p_info->frameinfo.mLayer)
    {
        if(p_info->frameinfo.mMPEGVersion != 10) // MPEG-2/2.5 (LSF)
        {
            if(p_info->frameinfo.mChannelMode != 1) // Stereo, Joint Stereo, Dual Channel
                p_info->frameinfo.mSideInfoSize = 17;
            else // Mono
                p_info->frameinfo.mSideInfoSize = 9;
        }
        else // MPEG-1.0
        {
            if(p_info->frameinfo.mChannelMode != 1) // Stereo, Joint Stereo, Dual Channel
                p_info->frameinfo.mSideInfoSize = 32;
            else // Mono
                p_info->frameinfo.mSideInfoSize = 17;
        }
    }


    p_info->frameinfo.mBitrateType = 0; // common CBR by default


    return MPEG_AUDIO_OK;
}


/*!
 * mp3_vbrxinghdr_parse
 */
u32 mp3_vbrxinghdr_parse(unsigned char *p_xingHdr, u32 bufSize,
        mpegaudioframeinfo_t *p_info)
{
  u32 offset = 8;
  unsigned char flags = 0;

  //p_xingHdr=p_xingHdr+p_info->mSideInfoSize;  //if sideinfo exists,add 2010-12-21
  flags = p_xingHdr[7]; //

    if(bufSize < 4)
        return MPEG_AUDIO_NEED_MORE;

    p_info->frameinfo.mBitrateType = 0;

    // for "XING"
    if((p_xingHdr[0] == 'x' || p_xingHdr[0] == 'X')
            && (p_xingHdr[1] == 'i' || p_xingHdr[1] == 'I')
            && (p_xingHdr[2] == 'n' || p_xingHdr[2] == 'N')
            && (p_xingHdr[3] == 'g' || p_xingHdr[3] == 'G'))
    {
        // VBR(XING)
        p_info->frameinfo.mBitrateType = 2;
    }
    // for "INFO"
    else if((p_xingHdr[0] == 'i' || p_xingHdr[0] == 'I')
            && (p_xingHdr[1] == 'n' || p_xingHdr[1] == 'N')
            && (p_xingHdr[2] == 'f' || p_xingHdr[2] == 'F')
            && (p_xingHdr[3] == 'o' || p_xingHdr[3] == 'O'))
    {
        // CBR(INFO)
        p_info->frameinfo.mBitrateType = 1;
    }

    if(!p_info->frameinfo.mBitrateType) // no "XING" or "INFO" header
        return MPEG_AUDIO_NEED_MORE;

    if(bufSize < offset)
        return MPEG_AUDIO_NEED_MORE;

    if(flags & 0x01) // Frames field is present
    {
        if(bufSize < (offset + 4))
            return MPEG_AUDIO_NEED_MORE;
        //int change to s32
        //p_info->vbrinfo.mTotalFrames = *((s32 *)(p_xingHdr + offset));
        memcpy(&p_info->vbrinfo.mTotalFrames, (p_xingHdr + offset), sizeof(u32));

        _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mTotalFrames)),
                sizeof(p_info->vbrinfo.mTotalFrames));

        offset += 4;

    }
    if(flags & 0x02) // Bytes field is present
    {
        if(bufSize < (offset + 4))
            return MPEG_AUDIO_NEED_MORE;

        //p_info->vbrinfo.mTotalBytes = *((u32 *)(p_xingHdr + offset));
        memcpy(&p_info->vbrinfo.mTotalBytes, (p_xingHdr + offset), sizeof(u32));

        _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mTotalBytes)),
                sizeof(p_info->vbrinfo.mTotalBytes));

        offset += 4;
    }
    if(flags & 0x04) // TOC field is present
    {
        if(bufSize < (offset + 100))
            return MPEG_AUDIO_NEED_MORE;

        offset += 100;
    }
    if(flags & 0x08) // Quality indicator field is present
    {
    u32 quality = 0;
        if(bufSize < (offset + 4))
            return MPEG_AUDIO_NEED_MORE;

        //quality = *((u32 *)(p_xingHdr + offset));
        memcpy(&quality, (p_xingHdr + offset), sizeof(u32));

        _bigendian2Local((unsigned char *)(&quality),
                sizeof(quality));

        p_info->vbrinfo.mQuality = 100 - quality;

        offset += 4;
    }

    return TRUE;
}


/*!
 * mp3_vbrvbrihdr_parse
 */
u32 mp3_vbrvbrihdr_parse(unsigned char *p_vbriHdr, u32 bufSize,
        mpegaudioframeinfo_t *p_info)
{
  unsigned char *p_offset = NULL;
    u16 entrySize = 0;

    if(bufSize < 4)
        return MPEG_AUDIO_NEED_MORE;

    p_info->frameinfo.mBitrateType = 0;

    // for "VBRI"
    if((p_vbriHdr[0] == 'v' || p_vbriHdr[0] == 'V')
            && (p_vbriHdr[1] == 'b' || p_vbriHdr[1] == 'B')
            && (p_vbriHdr[2] == 'r' || p_vbriHdr[2] == 'R')
            && (p_vbriHdr[3] == 'i' || p_vbriHdr[3] == 'I'))
    {
        // VBR
        p_info->frameinfo.mBitrateType = 3;
    }

    if(!p_info->frameinfo.mBitrateType) // no "VBRI" header
        return MPEG_AUDIO_ERR;

    if(bufSize < 26)
        return MPEG_AUDIO_NEED_MORE;

    p_offset = (p_vbriHdr + 4);

    // VBRI version
    //p_info->vbrinfo.mVBRIVersionID = *((u16 *)p_offset);
    memcpy(&p_info->vbrinfo.mVBRIVersionID, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mVBRIVersionID)),
            sizeof(p_info->vbrinfo.mVBRIVersionID));

    p_offset += 2;

    // Delay
    (p_info->vbrinfo.mVBRIDelay)[0] = *p_offset;
    (p_info->vbrinfo.mVBRIDelay)[1] = *(p_offset + 1);

    p_offset += 2;

    // Quality indicator
    //p_info->vbrinfo.mQuality = *((u16 *)p_offset);
    memcpy(&p_info->vbrinfo.mQuality, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mQuality)),
            sizeof(p_info->vbrinfo.mQuality)); // can not make sure the big endian

    p_info->vbrinfo.mQuality = 100 - p_info->vbrinfo.mQuality;

    p_offset += 2;

    // total bytes
    //p_info->vbrinfo.mTotalBytes = *((u32 *)p_offset);
    memcpy(&p_info->vbrinfo.mTotalBytes, p_offset, sizeof(u32));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mTotalBytes)),
            sizeof(p_info->vbrinfo.mTotalBytes));

    p_offset += 4;


    // total frames number
    //p_info->vbrinfo.mTotalFrames = *((int *)p_offset);
    memcpy(&p_info->vbrinfo.mTotalFrames, p_offset, sizeof(u32));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mTotalFrames)),
            sizeof(p_info->vbrinfo.mTotalFrames));

    p_offset += 4;

    // Number of entries within TOC table
    //p_info->vbrinfo.mEntriesNumInTOCTable = *((u16 *)p_offset);
    memcpy(&p_info->vbrinfo.mEntriesNumInTOCTable, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mEntriesNumInTOCTable)),
            sizeof(p_info->vbrinfo.mEntriesNumInTOCTable));

    p_offset += 2;

    // Scale factor of TOC table entries
    //p_info->vbrinfo.mTOCTableFactor = *((u16 *)p_offset);
    memcpy(&p_info->vbrinfo.mTOCTableFactor, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mTOCTableFactor)),
            sizeof(p_info->vbrinfo.mTOCTableFactor));

    p_offset += 2;

    // Size per table entry in bytes (max 4)
    //entrySize = *((u16 *)p_offset);
    memcpy(&entrySize, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&entrySize), sizeof(entrySize));

    p_info->vbrinfo.mEntrySize = entrySize;

    p_offset += 2;

    // Frames per table entry
    //p_info->vbrinfo.mFramesNumPerTable = *((u16 *)p_offset);
    memcpy(&p_info->vbrinfo.mFramesNumPerTable, p_offset, sizeof(u16));

    _bigendian2Local((unsigned char *)(&(p_info->vbrinfo.mFramesNumPerTable)),
            sizeof(p_info->vbrinfo.mFramesNumPerTable));

    p_offset += 2;


    return TRUE;
}

/*!
 * mp3_Playtime_Culculate
 */
u32 mp3_playtime_culculate(mpegaudioframeinfo_t *p_mp3_info)
{

  if(p_mp3_info == NULL)  return 0;

  if((p_mp3_info->frameinfo.mBitrateType == 0)||(p_mp3_info->frameinfo.mBitrateType == 1))
  {
    if(p_mp3_info->frameinfo.mBitrate == 0) return 0;
    p_mp3_info->mDuration = (p_mp3_info->mFileSize - p_mp3_info->id3v2_len - 128)
                           /((p_mp3_info->frameinfo.mBitrate * 1000) / 8);
  }
  else if((p_mp3_info->frameinfo.mBitrateType == 2)||(p_mp3_info->frameinfo.mBitrateType == 3))
  {
     if(p_mp3_info->frameinfo.mSamplerate == 0) return 0;
    p_mp3_info->mDuration = p_mp3_info->vbrinfo.mTotalFrames
                           *p_mp3_info->frameinfo.mSamplesPerFrame
                           /p_mp3_info->frameinfo.mSamplerate;
  }

  return 1;

}


/*!
 * _calculateFrame
 */
/*
u32 _calculateFrame(mpegaudioframeinfo_t *p_info)
{
    if(p_info->frameinfo.mSamplerate == 0) return 1;
    p_info->vbrinfo.mFrameSize = (p_info->frameinfo.mSamplesPerFrame
                                 *p_info->frameinfo.mBitrate * 1000)
            / (8 * p_info->frameinfo.mSamplerate)
            + p_info->frameinfo.mPaddingSize;

    return MPEG_AUDIO_OK;
}
*/
BOOL IsBig_Endian()
{
    unsigned short test = 0x1122;
    if(*((unsigned char *)&test) == 0x11)
       return TRUE;
else
    return FALSE;

}

/*!
 * _bigendian2Local
 */
u32 _bigendian2Local(unsigned char *p_valueAddr, u32 len)
{
    if(len <= 0 || len % 2 != 0)
        return -1;

   if(!IsBig_Endian())
    {
      _exchangeByteEndian(p_valueAddr, len);
    }

    return 0;
}

/*!
 * _litendian2Local
 */
/*
u32 _litendian2Local(unsigned char *p_valueAddr, u32 len)
{
    if(len <= 0 || len % 2 != 0)
        return -1;

   if(IsBig_Endian())
      _exchangeByteEndian(p_valueAddr, len);


    return 0;
}
*/
/*!
 * _exchangeByteEndian
 */
void _exchangeByteEndian(unsigned char *p_valueAddr, u32 len)
{
  u32 i = 0;
    u32 n = len >> 1;

    for(i = 0; i < n; i++)
    {
        unsigned char v = *(p_valueAddr + i);
        *(p_valueAddr + i) = *(p_valueAddr + len - 1 - i);
        *(p_valueAddr + len - 1 - i) = v;
    }
}

//////////////////////////////////////////////////////////////////////////


u32 mp3_head_parse(u8 *p_buf,u32 buflen,mpegaudioframeinfo_t *p_mp3info)
{
  u8 *p_head_buf = p_buf;
  u32 head_buf_len = buflen;

  if((p_head_buf == NULL) || (head_buf_len == 0))
    return 0;

  while((head_buf_len) && (p_mp3info->status != ALL_OK))
  {
    if(p_mp3info->is_mp3file == FALSE)
      break;

    if(head_buf_len < 4)
    {
       //need more buffer
       p_mp3info->needmorebuf = TRUE;
       p_mp3info->need_bufpos = p_mp3info->filebuf_pos;
       p_mp3info->need_buflen = 4;
       break;
    }

    switch(p_mp3info->status)
    {
        case IDLE:
        {
           u32 ret = 0;
           ret = mp3_id3v2_match(p_head_buf);
           if(ret == 0)
           {
             p_mp3info->status = ID3V2_OK;
             break;
           }
           else if(ret == 1)
           {
             u32 id3v2_status = 0;
             if(head_buf_len < 10)
             {
               //need more buffer
               head_buf_len = 0;
               p_mp3info->needmorebuf = TRUE;
               p_mp3info->need_bufpos = 0;
               p_mp3info->need_buflen = 10;
               break;
             }

             p_mp3info->id3v2_len = mp3_id3v2_culculatelen(p_head_buf,head_buf_len);
             if(head_buf_len < (p_mp3info->id3v2_len + 14))
             {
                 //need more buffer
                 head_buf_len = 0;
                 p_mp3info->needmorebuf = TRUE;
                 p_mp3info->need_bufpos = 0;
                 p_mp3info->need_buflen = 14 + p_mp3info->id3v2_len;
                 break;
             }

             id3v2_status = mp3_id3v2_parse(p_mp3info, p_head_buf,head_buf_len, 0);
             if(head_buf_len >= (p_mp3info->id3v2_len + 10))
             {
                head_buf_len -= (p_mp3info->id3v2_len + 10);
                p_head_buf += (p_mp3info->id3v2_len + 10);
             }

             //for mp3 file have two id3 sign.
             if((p_head_buf[0] == 'I') && (p_head_buf[1] == 'D') && (p_head_buf[2] == '3'))
             {
                 u32 id3_v2len = mp3_id3v2_culculatelen(p_head_buf,head_buf_len);

                 p_mp3info->need_bufpos = id3_v2len + p_mp3info->id3v2_len;
                 p_mp3info->need_buflen = 50 * 1024;
                 p_mp3info->status = ID3V2_OK;
                 
                 head_buf_len = 0;
               
                 break;
             }
             
             if(id3v2_status >= 1)
             {
                p_mp3info->filebuf_pos = 10 + p_mp3info->id3v2_len;
                p_mp3info->status = ID3V2_OK;
                break;
             }

           }

         }

         case ID3V2_OK:
         {
           mpegaudioret_t ret = {0};
           ret = mp3_syncframeinfo_parse(p_head_buf,head_buf_len,p_mp3info,1);
           /*if((p_mp3info->frameinfo.mLayer > 3) || (p_mp3info->frameinfo.mLayer < 1))
           {
                  //is not mp3 file
                  head_buf_len = 0;
                  p_mp3info->is_mp3file = FALSE;
                  break;
           }*/
           
           if(ret.mErrCode == MPEG_AUDIO_NEED_MORE)
           {
               p_mp3info->findcount++;
               if(p_mp3info->findcount > 41)
               {
                  //is not mp3 file
                  head_buf_len = 0;
                  p_mp3info->is_mp3file = FALSE;
                  break;
               }

               //need more buffer
               head_buf_len = 0;
               p_mp3info->needmorebuf = TRUE;
               p_mp3info->need_bufpos = p_mp3info->filebuf_pos + 
                                        (SIZEREQ * (p_mp3info->findcount - 1));

			   if(p_mp3info->need_bufpos > 3)
               {
                 p_mp3info->need_bufpos -= 3;
              }
               p_mp3info->need_buflen = SIZEREQ;
               break;
           }
           else if(ret.mErrCode == MPEG_AUDIO_ERR)
           {
               p_mp3info->need_buflen = 1; //for second call mp3_head_parse,malloc buf_len is legal
               p_mp3info->is_mp3file = FALSE;
               break;
           }
           else
           {
               p_mp3info->findcount = 0;
               p_mp3info->is_mp3file = TRUE;
               p_mp3info->status = FRAME_OK;
               if(head_buf_len >= (ret.mNextPos + 4))
               {
                 head_buf_len = head_buf_len - ret.mNextPos - 4;
                 p_head_buf = p_head_buf + ret.mNextPos + 4;
                 p_mp3info->filebuf_pos += 4;
               }
           }
         }

         case FRAME_OK:
         {
             u32 xing = 0,vbr = 0;
             p_mp3info->findcount++;
             if(p_mp3info->findcount > 4)
             {
                p_mp3info->findcount = 0;
                p_mp3info->status = XING_OK;
                break;
             }

             if (p_mp3info->frameinfo.mSideInfoSize > 0)
             {
               if(head_buf_len > p_mp3info->frameinfo.mSideInfoSize + 8)
               {
                  p_head_buf += p_mp3info->frameinfo.mSideInfoSize;
                  head_buf_len -= p_mp3info->frameinfo.mSideInfoSize;
               }
               else
              {
                  //need more buffer
                  p_mp3info->needmorebuf = TRUE;
                  p_mp3info->need_bufpos = p_mp3info->filebuf_pos;
                  p_mp3info->need_buflen = p_mp3info->frameinfo.mSideInfoSize + 120;
                  break;
              }

           }

/*
          //2012-11-02
          if(head_buf_len >= (10 + 120))       //ID3 tag len 10,"xing" size is 120
          {
             p_head_buf = p_head_buf + 10;         //ID3 tag len is 10
             head_buf_len = head_buf_len - 10;
          }
*/
           xing = mp3_vbrxinghdr_parse(p_head_buf,head_buf_len,p_mp3info);
           if(xing == TRUE)
           {
             p_mp3info->findcount = 0;
             p_mp3info->status = XING_OK;
             break;
           }

           vbr = mp3_vbrvbrihdr_parse(p_head_buf,head_buf_len,p_mp3info);
           if(vbr)
           {
             p_mp3info->findcount = 0;
             p_mp3info->status = XING_OK;
             break;
           }
           else
           {
             p_mp3info->needmorebuf = TRUE;
             p_mp3info->need_bufpos = p_mp3info->filebuf_pos;
             p_mp3info->need_buflen = p_mp3info->frameinfo.mSideInfoSize + 120;
             break;
                       //need more buffer
           }

         }

         case XING_OK:
         {
           u32 id3v1 = 0;

           p_mp3info->findcount++;
           if(p_mp3info->findcount > 2)
           {
              p_mp3info->findcount = 0;
              p_mp3info->status = ID3V1_OK;
              break;
           }

           id3v1 = mp3_id3v1_parse(p_mp3info,p_head_buf);
           if(id3v1)
           {
             p_mp3info->status = ID3V1_OK;
             break;
           }
           else
           {
             //need more buffer
             head_buf_len = 0;
             p_mp3info->needmorebuf = TRUE;
             p_mp3info->need_bufpos = p_mp3info->mFileSize - 128;
             p_mp3info->need_buflen = 128;
             break;
           }

         }

         case ID3V1_OK:
         {
             p_mp3info->needmorebuf = FALSE;
             mp3_playtime_culculate(p_mp3info);

           //if(1)
           //{
             p_mp3info->status = ALL_OK;
             break;
           //}

         }

         default:
           break;

    }
  }

  return 1;

}



s32 mp3_getframe_buffer(u8 *p_buf,u32 bufLen,mp3_getframe_state_t *p_check_state)
{

  u32 i = 0;
  //u32 loopSize = bufLen - 1;
  u32 mFrameSize = 0,mSamplesPerFrame = 0;
  u32 mBitrate = 0, mSamplerate = 0, mPaddingSize = 0;
  u32 index_I = 0,index_II = 0;
  u32 mMPEGVersion = 0, mLayer = 0;
  u32 mSideInfoSize = 0;
  u8  mChannelMode = 0;

  //loopSize = bufLen - 1;
  //for(; i < loopSize; i++)
  //{
    if((p_buf[i] == 0xFF) && ((p_buf[i + 1] & 0xE0) == 0xE0) && ((p_buf[i + 2] & 0xF0) != 0xF0))
    {
      // MPEG Version
      mMPEGVersion = ((p_buf[i + 1] >> 3) & 0x03);
      switch(mMPEGVersion)
      {
      case 0:
        mMPEGVersion = 25;
        break;
      case 2:
        mMPEGVersion = 20;
        break;
      case 3:
        mMPEGVersion = 10;
        break;
      default:
        mMPEGVersion = 0;
        return MPEG_AUDIO_ERR;
      };

      // Layer index
      mLayer = ((p_buf[i + 1] >> 1) & 0x03);
      switch(mLayer)
      {
      case 1:
        mLayer = 3;
        break;
      case 2:
        mLayer = 2;
        break;
      case 3:
        mLayer = 1;
        break;
      default:
        mLayer = 0;
        return MPEG_AUDIO_ERR;
      };

      // Bit_Rate
      mBitrate = ((p_buf[i + 2] >> 4) & 0x0f);
      if(mBitrate == 0x0f)
        return MPEG_AUDIO_ERR;

      index_I = (mMPEGVersion == 10 ? 0 : 1);
      index_II = mLayer - 1;
      if((index_I > 1)||(index_II > 2)||(mBitrate > 14))
        return MPEG_AUDIO_ERR;
      mBitrate = BitrateTable[index_I][index_II][mBitrate];

      // samplerate
      mSamplerate = ((p_buf[i + 2] >> 2) & 0x03);
      if(mSamplerate == 0x03)
        return MPEG_AUDIO_ERR;

      index_I = 2; // MPEG-2.5 by default
      if(mMPEGVersion == 20)
        index_I = 1;
      else if(mMPEGVersion == 10)
        index_I = 0;
      if((index_I > 2)||(mSamplerate > 2))
        return MPEG_AUDIO_ERR;
      mSamplerate = SamplerateTable[index_I][mSamplerate];


      // Padding size
      mPaddingSize = ((p_buf[i + 2] >> 1) & 0x01);
      if(mPaddingSize)
      {
        mPaddingSize = (mLayer == 1 ? 4 : 1);
      }

      // SamplesPerFrame
      mSamplesPerFrame = 1152;
      if(1 == mLayer)
      {
        mSamplesPerFrame = 384;
      }
      else if((mMPEGVersion != 10) && (3 == mLayer))
      {
        mSamplesPerFrame = 576;
      }

      // channel mode
      mChannelMode = ((p_buf[i + 3] >> 6) & 0x03);
      switch(mChannelMode)
      {
      case 0:
        mChannelMode = 3;
        break;
      case 1:
        mChannelMode = 0;
        break;
      case 2:
        mChannelMode = 2;
        break;
      case 3:
      default:
        mChannelMode = 1;
      };

      // SideInfo
      if(3 == mLayer)
      {
        if(mMPEGVersion != 10) // MPEG-2/2.5 (LSF)
        {
          if(mChannelMode != 1) // Stereo, Joint Stereo, Dual Channel
            mSideInfoSize = 17;
          else // Mono
            mSideInfoSize = 9;
        }
        else // MPEG-1.0
        {
          if(mChannelMode != 1) // Stereo, Joint Stereo, Dual Channel
            mSideInfoSize = 32;
          else // Mono
            mSideInfoSize = 17;
        }
      }

      //Culculate Framesize
      mFrameSize = (mSamplesPerFrame * mBitrate * 1000)/ (8 * mSamplerate) + mPaddingSize;

      p_check_state->sideinfo_size = mSideInfoSize;
      /*
      p_check_state->left_len = bufLen - i-1;
      if(p_check_state->first_frame == TRUE)
      {
         //find "xing","vbri","info","lame" head

      }*/

      return mFrameSize;

    }
  //  else if(i == loopSize-1 && p_buf[i+1] != 0xff)
  //  {
  //    i++;
  //  }
  //}

  return mFrameSize;

}

u32 mp3_parse_info(u16 *p_name, mpegaudioframeinfo_t *p_mp3_info)
{
  u8 *p_buffer = NULL;
  u32 buffer_size = 0;
  u32 result = 0;
  hfile_t file = NULL;
  u32 read_size = 0;
  vfs_file_info_t file_info = {0};

  if(p_name == NULL)
  {
    return ERR_FAILURE;
  }
  
  file = vfs_open(p_name, VFS_READ);
  if (file == NULL)
  {
    return 0;
  }

  vfs_get_file_info(file, &file_info);
  buffer_size = SIZEREQ;
  p_buffer = mp3_frame_malloc(buffer_size);
  CHECK_FAIL_RET_ZERO(p_buffer != NULL);

  memset(p_mp3_info, 0, sizeof(mpegaudioframeinfo_t));
  p_mp3_info->status = IDLE;
  p_mp3_info->needmorebuf = TRUE;
  p_mp3_info->filebuf_pos = 0;
  p_mp3_info->mFileSize = file_info.file_size;
  p_mp3_info->findcount = 0;
  p_mp3_info->is_mp3file = TRUE;
  p_mp3_info->id3v2_len = 0;

  memset(p_buffer, 0, buffer_size);
  vfs_seek(file, 0, VFS_SEEK_SET);

  read_size = vfs_read(p_buffer, buffer_size, 1, file);
  result = mp3_head_parse(p_buffer,buffer_size,p_mp3_info);
  mp3_frame_free(p_buffer);
  p_buffer = NULL;

  if(result > 0)
  {
    while((p_mp3_info->needmorebuf == TRUE) || (p_mp3_info->status != ALL_OK))
    {
       p_buffer = (unsigned char *)mp3_frame_malloc(p_mp3_info->need_buflen + 128);
       CHECK_FAIL_RET_ZERO(p_buffer != NULL);
       memset(p_buffer,0,p_mp3_info->need_buflen);

       vfs_seek(file, p_mp3_info->need_bufpos, VFS_SEEK_SET);
       read_size = vfs_read(p_buffer, p_mp3_info->need_buflen, 1, file);
       if(read_size == 0)
       {  
          break;
       }
       
       result = mp3_head_parse(p_buffer,read_size,p_mp3_info);
       if(result < 0)
        break;

        if(p_mp3_info->is_mp3file == FALSE)
          break;

        if(p_buffer != NULL)
        {
            mp3_frame_free(p_buffer);
            p_buffer = NULL;
        }

     }

     if(p_buffer != NULL)
     {
        mp3_frame_free(p_buffer);
        p_buffer = NULL;
     }

   }

  vfs_seek(file, 0, VFS_SEEK_SET);
  vfs_close(file);

  return 1;
}



/*!
  attach buffer, for memory share. By default, mp3 module will the malloc buffer.
  */
RET_CODE mul_mp3_attach_buffer(void *p_buffer, u32 buffer_size)
{
  mp3_frame_priv_t *p_mp3_priv = NULL;
  RET_CODE ret = SUCCESS;

  p_mp3_priv = class_get_handle_by_id(MP3_FRAME_CLASS_ID);
  if(p_mp3_priv == NULL)
  {
    p_mp3_priv = mtos_malloc(sizeof(mp3_frame_priv_t));
    memset(p_mp3_priv, 0, sizeof(mp3_frame_priv_t));
    class_register(MP3_FRAME_CLASS_ID, p_mp3_priv);
  }
  
  if(p_mp3_priv == NULL || p_buffer == NULL || buffer_size == 0)
  {
        return ERR_FAILURE;
  }

  ret = lib_memp_create(&p_mp3_priv->p_heap, (u32)p_buffer, buffer_size);
  if(ret == SUCCESS)
  {
    p_mp3_priv->use_memp = TRUE;
  }

  return ret;
}

/*!
  detach buffer.
  */
RET_CODE mul_mp3_detach_buffer()
{
  mp3_frame_priv_t *p_mp3_priv = NULL;

  p_mp3_priv = class_get_handle_by_id(MP3_FRAME_CLASS_ID);
  if(p_mp3_priv == NULL)
  {
    return ERR_FAILURE;
  }

  p_mp3_priv->use_memp = FALSE;

  return lib_memp_destroy(&p_mp3_priv->p_heap);

}
