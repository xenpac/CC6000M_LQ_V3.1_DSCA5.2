/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"

#include "char_map.h"
#include "iso_6937.h"
#include "iso_8859.h"
#include "gb2312.h"
#include "mtos_printk.h"
#include "lib_char.h"
#include "lib_unicode.h"
#include "mtos_mem.h"
#include "class_factory.h"
#include "iconv_ext.h"
#include "err_check_def.h"

/*!
   arabic change letter position id
  */
typedef enum
{
    ARABIC_POS_BASE = 0,
    ARABIC_POS_LAST,
    ARABIC_POS_FIRST,
    ARABIC_POS_MIDDLE
}arabic_position_id_t; 

static u16 arabic_table[][5]=  
{
  //base,    last,      first,     middle,  alone
  {0x0621, 0xfe80, 0xfe80, 0xfe80, 0xfe80},   // 0x0621
  {0x0622, 0xfe82, 0xfe81, 0xfe82, 0xfe81},   //0x0622
  {0x0623, 0xfe84, 0xfe83, 0xfe84, 0xfe83},
  {0x0624, 0xfe86, 0xfe85, 0xfe86, 0xfe85},
  {0x0625, 0xfe88, 0xfe87, 0xfe88, 0xfe87},
  {0x0626, 0xfe8a, 0xfe8b, 0xfe8c, 0xfe89},
  {0x0627, 0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d},
  {0x0628, 0xfe90, 0xfe91, 0xfe92, 0xfe8f},   // 0x0628
  {0x0629, 0xfe94, 0xfe93, 0xfe93, 0xfe93},
  {0x062a, 0xfe96, 0xfe97, 0xfe98, 0xfe95},   // 0x062A
  {0x062b, 0xfe9a, 0xfe9b, 0xfe9c, 0xfe99},
  {0x062c, 0xfe9e, 0xfe9f, 0xfea0, 0xfe9d},
  {0x062d, 0xfea2, 0xfea3, 0xfea4, 0xfea1},
  {0x062e, 0xfea6, 0xfea7, 0xfea8, 0xfea5},
  {0x062f, 0xfeaa, 0xfea9, 0xfeaa, 0xfea9},
  {0x0620, 0xfeac, 0xfeab, 0xfeac, 0xfeab},   // 0x0630
  {0x0631, 0xfeae, 0xfead, 0xfeae, 0xfead},
  {0x0632, 0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf},
  {0x0633, 0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1},
  {0x0634, 0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5},
  {0x0635, 0xfeba, 0xfebb, 0xfebc, 0xfeb9},   // 0x635
  {0x0636, 0xfebe, 0xfebf, 0xfec0, 0xfebd},
  {0x0637, 0xfec2, 0xfec3, 0xfec4, 0xfec1},
  {0x0638, 0xfec6, 0xfec7, 0xfec8, 0xfec5},   // 0x0638
  {0x0639, 0xfeca, 0xfecb, 0xfecc, 0xfec9},
  {0x063a, 0xfece, 0xfecf, 0xfed0, 0xfecd},   // 0x063A
  {0x063b, 0x063b, 0x063b, 0x063b, 0x063b},
  {0x063c, 0x063c, 0x063c, 0x063c, 0x063c},
  {0x063d, 0x063d, 0x063d, 0x063d, 0x063d},
  {0x063e, 0x063e, 0x063e, 0x063e, 0x063e},
  {0x063f, 0x063f, 0x063f, 0x063f, 0x063f},
  {0x0640, 0x0640, 0x0640, 0x0640, 0x0640},   // 0x0640
  {0x0641, 0xfed2, 0xfed3, 0xfed4, 0xfed1},
  {0x0642, 0xfed6, 0xfed7, 0xfed8, 0xfed5},
  {0x0643, 0xfeda, 0xfedb, 0xfedc, 0xfed9},
  {0x0644, 0xfede, 0xfedf, 0xfee0, 0xfedd},
  {0x0645, 0xfee2, 0xfee3, 0xfee4, 0xfee1},
  {0x0646, 0xfee6, 0xfee7, 0xfee8, 0xfee5},
  {0x0647, 0xfeea, 0xfeeb, 0xfeec, 0xfee9},
  {0x0648, 0xfeee, 0xfeed, 0xfeee, 0xfeed},   // 0x0648
  {0x0649, 0xfef0, 0xfeef, 0xfef0, 0xfeef},
  {0x064a, 0xfef2, 0xfef3, 0xfef4, 0xfef1},   // 0x064A
  {0x067E, 0xfb57, 0xfb58, 0xfb59, 0xfb56},
  {0x0686, 0xfb7b, 0xfb7c, 0xfb7d, 0xfb7a},
  {0x0698, 0xfb8b, 0xfb8a, 0xfb8b, 0xfb8a},
  {0x06A9, 0xfb8f, 0xfb90, 0xfb91, 0xfb8e},
  {0x06AF, 0xfb93, 0xfb94, 0xfb95, 0xfb92},
  {0x06cc, 0xfbfd, 0xfbfe, 0xfbff, 0xfbfc},
};
#define ARABIC_POS_CONT (sizeof(arabic_table)/(sizeof(u16)*5))

#define ARABIC_SET1_SIZE  30
#define ARABIC_SET2_SIZE  42
/*!
   first connect
  */
static u16 arabic_change_set1[ARABIC_SET1_SIZE] = 
{
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626, 0x6cc,
    0x67e, 0x686, 0x698, 0x6a9, 0x6af, 0x6cc,      
};

/*!
   last connect
  */
static u16 arabic_change_set2[ARABIC_SET2_SIZE] = 
{
     0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 
     0x642, 0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 
     0x646, 0x62a, 0x644, 0x628, 0x64a, 0x633, 0x634, 
     0x638, 0x626, 0x627, 0x623, 0x625, 0x622, 0x62f, 
     0x630, 0x631, 0x632, 0x648, 0x624, 0x629, 0x649,
     0x6cc, 0x67e, 0x686, 0x698, 0x6a9, 0x6af, 0x6cc,  
};

/*!
   combine letter
  */
#define ARABIC_COMBINE_LETTER    0x0644

static u16 arabic_combine[][3]=
{
    {0x0622, 0xFEF5, 0xFEF6},
    {0x0623, 0xFEF7, 0xFEF8},
    {0x0625, 0xFEF9, 0xFEFA},
    {0x0627, 0xFEFB, 0xFEFC},
};

/*!
   arabic combine letter check resault
  */
typedef enum 
{
    ARABIC_COMBINE_LETTER_NO = 1,
    ARABIC_COMBINE_LETTER_SET1,
    ARABIC_COMBINE_LETTER_SET2,    
    ARABIC_COMBINE_LETTER_END,
}arabic_combine_id_t;



/*!
   The structure is defined to the private information.
  */
typedef struct
{
  /*!
    */  
  BOOL is_bigendian;
  /*!
     The dvb text convertor which is used if no character 
     selection information is given in a text item. 
    */
  dvb_txt_convertor_t def_convertor;
}lib_char_info_t;

/*
 * CP1256
 */
static const unsigned short cp1256_2uni[128] = {
  /* 0x80 */
  0x20ac, 0x067e, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
  0x02c6, 0x2030, 0x0679, 0x2039, 0x0152, 0x0686, 0x0698, 0x0688,
  /* 0x90 */
  0x06af, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
  0x06a9, 0x2122, 0x0691, 0x203a, 0x0153, 0x200c, 0x200d, 0x06ba,
  /* 0xa0 */
  0x00a0, 0x060c, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x06be, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  /* 0xb0 */
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
  0x00b8, 0x00b9, 0x061b, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x061f,
  /* 0xc0 */
  0x06c1, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627,
  0x0628, 0x0629, 0x062a, 0x062b, 0x062c, 0x062d, 0x062e, 0x062f,
  /* 0xd0 */
  0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00d7,
  0x0637, 0x0638, 0x0639, 0x063a, 0x0640, 0x0641, 0x0642, 0x0643,
  /* 0xe0 */
  0x00e0, 0x0644, 0x00e2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x0649, 0x064a, 0x00ee, 0x00ef,
  /* 0xf0 */
  0x064b, 0x064c, 0x064d, 0x064e, 0x00f4, 0x064f, 0x0650, 0x00f7,
  0x0651, 0x00f9, 0x0652, 0x00fb, 0x00fc, 0x200e, 0x200f, 0x06d2,
};

void cp1256_to_unicode(u8 *p_char1256,
                              s32 length,
                              u16 *p_unicode,
                              s32 maxlen)
{
  s32 si = 0, di = 0;
  unsigned char c = 0;

  CHECK_FAIL_RET_VOID(NULL != p_char1256
         && 0 != length
         && NULL != p_unicode
         && 0 != maxlen);

  for(si = di = 0; (si < length) && (di < maxlen); si++)
  {
    c = p_char1256[si];
    if(p_char1256[si] < 0x80)
    {
       p_unicode[di++] = (u16)c;
    }
    else
    {
       p_unicode[di++] = (u16)cp1256_2uni[c - 0x80];
    }
  }

  if(di < maxlen)
  {
    p_unicode[di] = 0x0000;
  }
  else
  {
    p_unicode[maxlen] = 0x0000;
  }
}

/*!
   This function allows you to reassign a dvb text convertor which is used if 
   no character selection information is given in a text item. 
   (for those non-standard encoding text)

   \param[in] p_dvb Points to the DVB SI string.
   \param[in] length Specifies the length of the DVB SI string.
   \param[in] p_unicode Points to the unicode string.
   \param[in] maxlen Specifies the maximal length of the unicode string.

  */
void dvb_set_def_convertor(dvb_txt_convertor_t convertor, BOOL is_bigendian)
{
  lib_char_info_t *p_info = NULL;

  p_info = (lib_char_info_t *)class_get_handle_by_id(LIB_CHAR_CLASS_ID);

  /* not register */
  if (p_info == NULL) 
  {
    p_info = (lib_char_info_t *)mtos_malloc(sizeof(lib_char_info_t));
    CHECK_FAIL_RET_VOID(p_info != NULL);

    class_register(LIB_CHAR_CLASS_ID, (class_handle_t)(p_info));
  }

  /* not assigned */
  if (convertor == NULL)
  {
    /* it is default Latin 00 table character. ISO-6937 is used. */
    p_info->def_convertor = (dvb_txt_convertor_t)iso6937_to_unicode;
  }
  else
  {
    p_info->def_convertor = convertor;
  }
  p_info->is_bigendian = is_bigendian;
}


/**
 * name		: get_iso639_refcode
 * description	: return reference code of ISO8859/ISO6937 according to iso639 code.
 * parameter	: 1
 *	IN	UINT8 *iso639:	the ISO-639 string, could be 2 or 3 letters.
 * return value	: UINT32
 *	0	: no reference code.
 *	1~15	: the ISO-8859 code page value.
 *	6937	: the ISO-6937 reference code.
 *	other	: reserved.
 */

u32 get_iso639_refcode(u8 *iso639)
{
  u32 refcode = 0;

  if (iso639 == NULL) {
    refcode = 0;
  } else if ((memcmp(iso639, "slk", 3)==0) || (memcmp(iso639, "SLK", 3)==0) \
    ||(memcmp(iso639, "pol", 3)==0) || (memcmp(iso639, "POL", 3)==0) \
    ||(memcmp(iso639, "slo", 3)==0) || (memcmp(iso639, "SLO", 3)==0)){
    refcode = 0x02; // ISO/IEC 8859-2 
  } else if ((memcmp(iso639, "rus", 3)==0) || (memcmp(iso639, "RUS", 3)==0)\
    ||(memcmp(iso639, "wen", 3)==0) || (memcmp(iso639, "WEN", 3)==0)) {
    refcode = 0x05; // ISO/IEC 8859-5 
  } else if ((memcmp(iso639, "ara", 3)==0) || (memcmp(iso639, "ARA", 3)==0) \
    ||(memcmp(iso639, "per", 3)==0) || (memcmp(iso639, "PER", 3)==0) \
    ||(memcmp(iso639, "fas", 3)==0) || (memcmp(iso639, "FAS", 3)==0) \
                ||(memcmp(iso639, "ira", 3)==0) || (memcmp(iso639, "IRA", 3)==0)\
                ||(memcmp(iso639, "zho", 3)==0)|| (memcmp(iso639, "ZHO", 3)==0)) {
    refcode = 0x06; // ISO/IEC 8859-6 Latin/Arabic alphabet
  } else if ((memcmp(iso639, "gre", 3)==0) || (memcmp(iso639, "GRE", 3)==0)) {
    refcode = 0x07; // ISO/IEC 8859-7 Latin/Greek alphabet
  } else if ((memcmp(iso639, "heb", 3)==0) || (memcmp(iso639, "HEB", 3)==0)) {
    refcode = 0x08; // ISO/IEC 8859-8 Latin/Hebrew alphabet
  } else if ((memcmp(iso639, "tur", 3)==0) || (memcmp(iso639, "TUR", 3)==0)\
    ||(memcmp(iso639, "kur", 3)==0) || (memcmp(iso639, "KUR", 3)==0)) {
    refcode = 0x09; // ISO/IEC 8859-9 
  } else if ((memcmp(iso639, "dan", 3)==0) || (memcmp(iso639, "DAN", 3)==0) \
    ||(memcmp(iso639, "ice", 3)==0) || (memcmp(iso639, "ICE",3)==0) \
    ||(memcmp(iso639, "isl", 3)==0) || (memcmp(iso639, "ISL",3)==0) \
    ||(memcmp(iso639, "nor", 3)==0) || (memcmp(iso639, "NOR",3)==0) \
    ||(memcmp(iso639, "nno", 3)==0) || (memcmp(iso639, "NNO",3)==0) \
    ||(memcmp(iso639, "sve", 3)==0) || (memcmp(iso639, "SVE", 3)==0) \
    ||(memcmp(iso639, "swe", 3)==0) || (memcmp(iso639, "SWE", 3)==0)){
    refcode = 0x0a; // ISO/IEC 8859-10
	} else if ((memcmp(iso639, "tha", 3)==0) || (memcmp(iso639, "THA", 3)==0)) {
		refcode = 0x0b; // ISO/IEC 8859-11     
  } else if ((memcmp(iso639, "fin", 3)==0) || (memcmp(iso639, "FIN", 3)==0) \
    ||(memcmp(iso639, "est", 3)==0) || (memcmp(iso639, "EST",3)==0)){
    refcode = 0x0d; // ISO/IEC 8859-13 
  } else if ((memcmp(iso639, "por", 3)==0) || (memcmp(iso639, "POR",3)==0) \
    ||(memcmp(iso639, "spa", 3)==0) || (memcmp(iso639, "SPA",3)==0) \
    ||(memcmp(iso639, "cat", 3)==0) || (memcmp(iso639, "CAT",3)==0) \
    ||(memcmp(iso639, "esl", 3)==0) || (memcmp(iso639, "ESL",3)==0) \
    ||(memcmp(iso639, "fra", 3)==0) || (memcmp(iso639, "fre",3)==0) \
    ||(memcmp(iso639, "ger", 3)==0) || (memcmp(iso639, "GER",3)==0) \
    ||(memcmp(iso639, "sco", 3)==0) || (memcmp(iso639, "SCO",3)==0) \
    ||(memcmp(iso639, "iri", 3)==0) || (memcmp(iso639, "IRI",3)==0) \
    ||(memcmp(iso639, "dan", 3)==0) || (memcmp(iso639, "DAN",3)==0) \
    ||(memcmp(iso639, "dut", 3)==0) || (memcmp(iso639, "DUT",3)==0) \
    ||(memcmp(iso639, "nla", 3)==0) || (memcmp(iso639, "NLA",3)==0) \
    ||(memcmp(iso639, "deu", 3)==0) || (memcmp(iso639, "DEU",3)==0)){
    refcode = 0x0f; // ISO/IEC 8859-15
  } else if ((memcmp(iso639, "hun", 3)==0) || (memcmp(iso639, "HUN", 3)==0) \
    ||(memcmp(iso639, "ron", 3)==0) || (memcmp(iso639, "RON", 3)==0) \
    ||(memcmp(iso639, "rum", 3)==0) || (memcmp(iso639, "RUM", 3)==0) \
    ||(memcmp(iso639, "ita", 3)==0) || (memcmp(iso639, "ITA", 3)==0) \
    ||(memcmp(iso639, "slv", 3)==0) || (memcmp(iso639, "SLV", 3)==0)) {
    refcode = 0x10; // ISO/IEC 8859-16 
  } else if ((memcmp(iso639, "chi", 3)==0) || (memcmp(iso639, "zho", 3)==0)) {
    refcode = 0x13; // GB2312
  } else if ((memcmp(iso639, "cze", 3)==0) || (memcmp(iso639, "CZE", 3)==0)
    ||(memcmp(iso639, "eng", 3)==0) || (memcmp(iso639, "wel", 3)==0)
    ||(memcmp(iso639, "ces", 3)==0) || (memcmp(iso639, "ces", 3)==0)) {
    refcode = 6937;
  } 
  return refcode;
}

// Count the number of bytes of a UTF-8 character
#define UTF8_CHAR_LEN(C) ((((u32)0xE5000000 >> ((C >> 3) & 0x1E)) & 3) + 1)

static s32 _utf8_to_unicode(u8 *src, u32 srcLen, u16 *dst, u32 maxlen)
{
  s32 srcIdx = 0, dstIdx = 0;
  u16 unicode = 0;
  u8 i = 0;
  
  while ((srcIdx < srcLen) && (dstIdx < (maxlen -1)))
  {
    if ((srcIdx + UTF8_CHAR_LEN(src[srcIdx]) > srcLen))
    {
      break;
    }
    for (i = 0; i < 8; i++)
    {
      if ((src[srcIdx] & (0x80 >> i)) == 0)
      {
        break;
      }
    }
    switch(i)
    {
      case 0: //ascii
        unicode = (u16)src[srcIdx++];
        break;
      case 1: // unknow character
        unicode = 0xFFFD;
        srcIdx += 1;
        break;
      case 2: // 2 byte
        if ((src[srcIdx + 1] & 0xC0) != 0x80)
        {
          unicode = 0xFFFD;
          srcIdx += 1;
          break;
        }
        unicode = ((u16)(src[srcIdx] & 0x1F)) << 6;
        unicode = unicode | (u16)(src[srcIdx + 1] & 0x3F);
        unicode = unicode;
        srcIdx += 2;
        break;
      case 3: // 3 byte
        if ((src[srcIdx + 1] & 0xC0) != 0x80 || (src[srcIdx + 2] & 0xC0) != 0x80)
        {
          unicode = 0xFFFD;
          srcIdx += 1;
          break;
        }
        unicode = ((u16)(src[srcIdx] & 0x0F)) << 12;
        unicode = unicode | ((u16)(src[srcIdx + 1] & 0x3F)) << 6;
        unicode = unicode | (u16)(src[srcIdx + 2] & 0x3F);
        unicode = unicode;
        srcIdx += 3;
        break;
      default: // no process for byte > 3 utf8 character
        unicode = 0xFFFD;
        srcIdx += i;
    }
    dst[dstIdx++] = unicode;
  }
  dst[dstIdx] = 0x0000;
  return dstIdx;
}


RET_CODE dvb_to_unicode_epg(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen, u32 refcode)
{

  if(refcode == 0)//not match
  {
    return dvb_to_unicode(p_dvb, length, p_unicode, maxlen);
  }
  else if(refcode == 6937)
  {
    if((p_dvb[0] == 0x15) && (length > 1))
    {
      _utf8_to_unicode((p_dvb + 1), (length - 1), p_unicode, maxlen);
    }
    else if((p_dvb[0] == 0x11) && (length > 1))
    {
      dvb_to_unicode((p_dvb), length, p_unicode, maxlen);
    }
    else
    {
      iso6937_to_unicode(p_dvb, length, p_unicode, maxlen);
    }
    return TRUE;
  }
  else if(refcode == 0x13)
  {
    if((p_dvb[0] == 0x11) && (length > 1))
    {
      dvb_to_unicode((p_dvb), length, p_unicode, maxlen);
    }
    else
    {
      gb2312_to_unicode(p_dvb, length, p_unicode, maxlen);
    }
    return TRUE;
  }
  else if((refcode == 0x06)&&(p_dvb[0] == 0x15)&&(length > 1)) //fix 41353 for langcode ira
  {
    _utf8_to_unicode((p_dvb + 1), (length - 1), p_unicode, maxlen);
    return TRUE;
  }
  else
  {
    iso8859_to_unicode(refcode, p_dvb, length, p_unicode, maxlen);
    return TRUE;
  }
}


RET_CODE dvb_to_unicode(u8 *p_dvb, s32 length, u16 *p_unicode, s32 maxlen)
{
  s32 si = 0;
  u32 code = 0;
  lib_char_info_t *p_info = NULL;
  u32 judge_len = 16;
  u32 i = 0; 
  u32 messy_len = 0;

  p_info = (lib_char_info_t *)class_get_handle_by_id(LIB_CHAR_CLASS_ID);

  if(length < 1)
  {
    *p_unicode = 0x0000;
    return ERR_FAILURE;
  }

  /*!
    temp solution for borden. 
    In front 16 bit of the ACSII code whether there is a messy code. 
    */
  if(length < 16)
  {
    judge_len = length;
  }

  for(i = 0; i < judge_len; i++)
  {
    if(p_dvb[i] >= 0x20)
    {
      /* already reassigned the default convertor */
      if (p_info != NULL) 
      {
        p_info->def_convertor(p_dvb, length, p_unicode, maxlen);
      }
      else
      {
        /* it is default Latin 00 table character. ISO-6937 is used. */
        iso6937_to_unicode(p_dvb, length, p_unicode, maxlen);
      }

      return SUCCESS;
    }
    else if((p_dvb[i] <= 0x0F) && (p_dvb[i] >= 0x01))
    {
      code = p_dvb[i] + 0x04;
      si = 1;

      break;
    }
    else if((p_dvb[i] == 0x10) && (length >= 3))
    {
      code = (p_dvb[1] << 8) | p_dvb[2];
      si = 3;

      break;
    }
    else if((p_dvb[i] == 0x13) || (p_dvb[i] == 0x14))
    {
      /*
        * 0x13: GB-2312-1980
        * 0x14: ISO/IEC 10646-1, Big5
        * Traditional/Simplified Chinese, will select non-Chinese???
        */
      si = 1;
      if((length - si) > 0)
      {
          gb2312_to_unicode(p_dvb + si, length - si, p_unicode, maxlen);
      }
      return SUCCESS;
    }
    else if((p_dvb[i] == 0x15) )
    {
      si = 1;
      if((length - si) > 0)
      {
        _utf8_to_unicode(p_dvb + si, length - si, p_unicode, maxlen);
      }
      return SUCCESS;
    }
    else if(p_dvb[i] == 0x11)
    {
      /* 0x11: ISO-10646-1 Basic Multilingual Plane (so-called p_unicode) */
      length = (length - 1) / 2 > maxlen ? 2 * maxlen : length - 1;
      /* need to verify on endian problem. */
      memcpy((u8 *)p_unicode, p_dvb + 1, length);
      p_unicode[length >> 1] = 0x0000;

      if (p_info != NULL) 
      {
        if (p_info->is_bigendian)
        {
          return SUCCESS;
        }
      }

      swap_unistr_endian(p_unicode);

      return SUCCESS;
    }
    else if((p_dvb[i] == 0x15))
    {
      /*
        * 0x15: UTF-8 encoding of ISO/IEC 10646 [16], Basic Multilingual Plane (BMP)
        */
      /* already reassigned the default convertor */
      {
        si = 1;//except first bit 0x15
        if(length <= si)
        {
          return SUCCESS;
        }
        
        {
          iconv_t g_cd_utf8_to_utf16le = NULL;
          char *p_inbuf = NULL;
          char *p_outbuf = NULL;
          size_t in_len = 0;
          size_t out_len = 0;
          p_inbuf = p_dvb + si;//except first bit 0x15
          p_outbuf = (u8 *)p_unicode;
          in_len = length - si;//except first bit 0x15
          out_len = maxlen;
          
          g_cd_utf8_to_utf16le  = iconv_open("ucs2le", "utf8");
          iconv(g_cd_utf8_to_utf16le, &p_inbuf, &in_len, &p_outbuf, &out_len);
          iconv_close(g_cd_utf8_to_utf16le);
        }
      }

      return SUCCESS;
    }
    else
    {
      /*
        * 0x12: KSC5601-1987
        *
        * 0x15-0x1F is reserved according to SPEC.
        */
      //*p_unicode = 0x0000;
      //return ERR_FAILURE;
      messy_len++;
    }
  }

  /*!
    temp soultion for borden.
    if all bit is messy, return err_failure. 
    */
  if(messy_len == judge_len)
  {
    *p_unicode = 0x0000;
    return ERR_FAILURE;
  }

  
  /*
    * try to use iso8859
    */
  if((code < SUPPORT_ISO8859_CNT) && ((length - si) > 0)) //temp solution andy.chen
  {
    iso8859_to_unicode(code, p_dvb + si, length - si, p_unicode, maxlen);
    return SUCCESS;
  }
  else
  {
    *p_unicode = 0x0000;
    return ERR_FAILURE;
  }
}

BOOL is_arabic_uni_code(u16 uni_code)
{
  if((uni_code >= 0x060C) && (uni_code <= 0x06FE)) //base arabic code
  {
      if((uni_code >= 0x0660) && (uni_code <= 0x0669))/////arabic number
        return FALSE;
      else
        return TRUE;
  }
  else if((uni_code >= 0x0750) && (uni_code <= 0x076D)) //extend arabic base code
  {
      return TRUE;
  }
  else if((uni_code >= 0xFB50) && (uni_code <= 0xFDFC)) //alaric code A
  {
      return TRUE;
  }
  else if((uni_code >= 0xFE70) && (uni_code <= 0xFEFC)) //alaric code B
  {
      return TRUE;
  }

  return FALSE;
}

BOOL is_hebrew_uni_code(u16 uni_code)
{
  if((0x05d0 <= uni_code) && (uni_code <= 0x05ea))
  {
    return TRUE;
  }

  return FALSE;
}

 u16 get_arabic_uni_str_len(u16 *uni_code_str)
{
u16 len = 0;
u16 i = 0;

while(0 != uni_code_str[i])
{
  if(is_arabic_uni_code(uni_code_str[i]) == TRUE)
    len ++;
  else
    break;
  i ++;
}

  return len;
}

static BOOL arabic_change_set1_check(u16 uni_code)
{
  u8 i = 0;

  for(i = 0; i < ARABIC_SET1_SIZE; i++)
  {
      if(uni_code == arabic_change_set1[i])
      {
          return TRUE;
      }
  }
  return FALSE;
}

static BOOL arabic_change_set2_check(u16 uni_code)
{
  u8 i = 0;

  for(i = 0; i < ARABIC_SET2_SIZE; i++)
  {
      if(uni_code == arabic_change_set2[i])
      {
          return TRUE;
      }
  }
  return FALSE;
}

static BOOL arabic_get_first_connect_letter(u16 uni_code, u16 *p_connect_letter)
{
  u8 i = 0;

  for(i = 0; i < ARABIC_POS_CONT; i++)
  {
      if(uni_code == arabic_table[i][ARABIC_POS_BASE])
      {
          *p_connect_letter = arabic_table[i][ARABIC_POS_FIRST];
          return TRUE;
      }
  }
  return FALSE;
}

static BOOL arabic_get_last_connect_letter(u16 uni_code, u16 *p_connect_letter)
{
  u8 i = 0;

  for(i = 0; i < ARABIC_POS_CONT; i++)
  {
      if(uni_code == arabic_table[i][ARABIC_POS_BASE])
      {
          *p_connect_letter = arabic_table[i][ARABIC_POS_LAST];
          return TRUE;
      }
  }
  return FALSE;
}

static BOOL arabic_get_mid_connect_letter(u16 uni_code, u16 *p_connect_letter)
{
  u8 i = 0;

  for(i = 0; i < ARABIC_POS_CONT; i++)
  {
      if(uni_code == arabic_table[i][ARABIC_POS_BASE])
      {
          *p_connect_letter = arabic_table[i][ARABIC_POS_MIDDLE];
          return TRUE;
      }
  }
  return FALSE;
}

static RET_CODE arabic_combine_letter_check(u16 *p_base_str, u16 input_code)
{
  u8 i = 0;
  u32 str_len = 0;
  BOOL is_combine = FALSE;
  
  str_len = uni_strlen(p_base_str);
  if(0 == str_len)
  {
      return ARABIC_COMBINE_LETTER_NO;
  }
  
  //check if already at the end of combine letter
  if((str_len >= 2) && (ARABIC_COMBINE_LETTER == p_base_str[str_len - 2]))
  {
    for(i = 0; i < 4; i++)
    {
        if(p_base_str[str_len - 1] == arabic_combine[i][0])
        {
            return ARABIC_COMBINE_LETTER_END;
        }
    }
  }
  
  //check if come into a new combine
  if(ARABIC_COMBINE_LETTER == p_base_str[str_len - 1])
  {
    for(i = 0; i < 4; i++)
    {
        if(input_code == arabic_combine[i][0])
        {
            is_combine = TRUE;
            break;
        }
    }
  }
  if(is_combine && (str_len >= 2))
  {
      if(arabic_change_set1_check(p_base_str[str_len - 2]))
      {
          return ARABIC_COMBINE_LETTER_SET1;
      }
      else 
      {
          return ARABIC_COMBINE_LETTER_SET2;
      }
  }
  return ARABIC_COMBINE_LETTER_NO;

}

static BOOL arabic_get_combine_letter(u16 combine_id, u16 input_code, u16 *p_combine_letter)
{
  u8 i = 0;

  for(i = 0; i < 4; i++)
  {
      if(input_code == arabic_combine[i][0])
      {
          if(ARABIC_COMBINE_LETTER_SET1 == combine_id)
          {
              *p_combine_letter = arabic_combine[i][2];
          }
          else
          {
              *p_combine_letter = arabic_combine[i][1];
          }
          return TRUE;
      }
  }
  return FALSE;
}


RET_CODE input_char_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code)
{
  u32 str_len_base = 0;  
  u32 str_len_convert = 0;  
  
  if((NULL == p_base_str) || (NULL == p_convert_str))
  {
    return ERR_FAILURE;
  }
  str_len_base = uni_strlen(p_base_str);
  str_len_convert = uni_strlen(p_convert_str);

  if(0 == str_len_base)
  {
      p_base_str[0] = input_code;
      p_convert_str[0] = input_code;
      return SUCCESS;
  }


  p_base_str[str_len_base] = input_code;
  p_convert_str[str_len_convert] = input_code;
  return SUCCESS;
}


RET_CODE arabic_input_char_convert(u16 *p_base_str, u16 *p_convert_str, u16 input_code)
{
  u32 str_len_base = 0;  
  u32 str_len_convert = 0;  
  u32 combine_id =  ARABIC_COMBINE_LETTER_NO;
  u16 combine_letter = 0;
  u16 connect_letter = 0;
  BOOL b_first_connect = FALSE;
  u16 base_tmp = 0, convert_tmp = 0;
  
  if((NULL == p_base_str) || (NULL == p_convert_str))
  {
    return ERR_FAILURE;
  }
  str_len_base = uni_strlen(p_base_str);
  str_len_convert = uni_strlen(p_convert_str);

  if(0 == str_len_base)
  {
      p_base_str[0] = input_code;
      p_convert_str[0] = input_code;
      return SUCCESS;
  }

  if(!is_arabic_uni_code(input_code))
  {
    p_base_str[str_len_base] = input_code;
    p_convert_str[str_len_convert] = input_code;
    return SUCCESS;
  }

  //step 1: combine letter check
  combine_id = arabic_combine_letter_check(p_base_str, input_code);
  if(ARABIC_COMBINE_LETTER_END == combine_id)
  {
      p_base_str[str_len_base] = input_code;   //base str
      p_convert_str[str_len_convert] = input_code;
      return SUCCESS;
  }
  else if((ARABIC_COMBINE_LETTER_SET1 == combine_id) || (ARABIC_COMBINE_LETTER_SET2 == combine_id))
  {
      if(arabic_get_combine_letter(combine_id, input_code, &combine_letter))
      {
          p_base_str[str_len_base] = input_code;   //base str
          p_convert_str[str_len_convert - 1] = combine_letter;
          p_convert_str[str_len_convert] = 0;
          return SUCCESS;
      }
  }

  //setp 2:connect letter check
  //middle connect possiblity check
  if((str_len_base >= 2) && arabic_change_set1_check(p_base_str[str_len_base - 2]))
  {
      b_first_connect = TRUE;
  }

  base_tmp = input_code;
  convert_tmp = input_code;
  
  //last connect letter check
  if(arabic_change_set2_check(input_code))
  {
    if(b_first_connect)
    {
      if(arabic_get_mid_connect_letter(p_base_str[str_len_base - 1], &connect_letter))
      {
        p_convert_str[str_len_convert - 1] = connect_letter;  //middle connect letter
      }
    }
    else
    {
      if(arabic_get_first_connect_letter(p_base_str[str_len_base - 1], &connect_letter))
      {
        p_convert_str[str_len_convert - 1] = connect_letter;  //last connect letter
      }
    }
  }
  //first connect letter check
  if(arabic_change_set1_check(p_base_str[str_len_base - 1]))
  {
    if(b_first_connect)
    {
      if(arabic_get_last_connect_letter(input_code, &connect_letter))
      {
        base_tmp = input_code;
        convert_tmp = connect_letter;
      }
    }
    else
    {
      if(arabic_get_last_connect_letter(input_code, &connect_letter))
      {
        base_tmp = input_code;
        convert_tmp = connect_letter;        
      }
    }
  }

  //no convert
  p_base_str[str_len_base] = base_tmp;   //base str
  p_convert_str[str_len_convert] = convert_tmp;

  return SUCCESS;
}


RET_CODE arabic_str_convert(u16 *p_base_str, u16 *p_convert_str, u16 max_len)
{
  u32 str_len_base = 0;  
  u32 str_len_convert = 0;  
  u32 i = 0;
  u16 transit_str[32] = {0};

  if((NULL == p_base_str) || (NULL == p_convert_str))
  {
      return ERR_FAILURE;
  }
  str_len_convert = uni_strlen(p_convert_str);
  if(0 != str_len_convert)
  {
      memset(p_convert_str, 0, sizeof(u16)*str_len_convert);
  }
  str_len_base = uni_strlen(p_base_str);
  max_len = (max_len > 32) ? 32 : max_len;
  str_len_base = (str_len_base > max_len) ? max_len : str_len_base;

  for(i = 0; i < str_len_base; i++)
  {
      (void)arabic_input_char_convert(transit_str, p_convert_str, p_base_str[i]);
  }
  
  return SUCCESS;
}

BOOL match_str(const u16 *p_src_string,
                  const u16 *p_match_string,
                  BOOL is_case_enabled)
{
  s16 i = 0;
  s16 j = 0;
  s16 nMatchOffset = 0;
  s16 nSourOffset = 0;
  BOOL bIsMatched = TRUE;
  u16 szSource[MAX_STR_LEN_IN_FIND + 2] = {0};
  u16 szMatcher[MAX_STR_LEN_IN_FIND + 2] = {0};
  u16 section_len = 0;
  
  if(p_src_string == NULL
    || p_match_string == NULL)
  {
    //NULL input string
    return FALSE;
  }

  //Is a empty string
  if(p_match_string[0] == 0)
  {
    if(p_src_string[0] == 0)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }


  if(is_case_enabled == TRUE)
  {
    //Save source string into temporary buffer
    memcpy(szSource, p_src_string, uni_strlen((u16 *)p_src_string) + 1);
  }
  else
  {
    //Change source string from lower case to upper case
    i = 0;
    while(p_src_string[i])
    {
      if(p_src_string[i] >= 'A' && p_src_string[i] <= 'Z')
      {
        szSource[i] = p_src_string[i] - 'A' + 'a';
      }
      else
      {
        szSource[i] = p_src_string[i];
      }
      i++;
    }
    szSource[i] = 0;
  }


  //Preprocess input string
  //merge continous '*' into a single '*'
  //Change word with upper case into lower case if case sensitive
  //is not enabled

  i = j = 0;
  while(p_match_string[i])
  {
    szMatcher[j++] = (!is_case_enabled) ?
                     //Lowercase lpszMatch[i] to szMatcher[j]
                     ((p_match_string[i] >= 'A' && p_match_string[i] <= 'Z') ?
                      p_match_string[i] - 'A' + 'a' :
                      p_match_string[i]) : p_match_string[i];
    //Merge '?'
    if(p_match_string[i] == '?')
    {
      while(p_match_string[++i] == '?')
      {
        ;
      }
    }
    else
    {
      i++;
    }
  }
  szMatcher[j] = 0; //Add 0 to the end of a string

  //special handler for match string didn't start with '?'
  i = 0;
  section_len = 0;

  if(szMatcher[0] != '?')
  {
    while(1)
    {
      if((szMatcher[i] != 0) && (szMatcher[i] != '?'))
      {
        section_len++;
      }
      else
      {
        break;
      }
      
      i++;
    }

    if(uni_strncmp(szMatcher, szSource, section_len) != 0)
    {
      return FALSE;
    }
  }

  nMatchOffset = nSourOffset = section_len;
  while(1)
  {
    if(szMatcher[nMatchOffset] == 0)
    {
      bIsMatched = TRUE;
      break;
    }

    if(szSource[nSourOffset] == 0)
    {
      bIsMatched = FALSE;
      break;
    }
  
    if(szMatcher[nMatchOffset] == '?')
    {
      //just skip one charater to match '?'
      nMatchOffset++;
      nSourOffset++;
    }
    else
    {
      section_len = 0;
      i = nMatchOffset;
      
      while(1)
      {
        if((szMatcher[i] != 0) && (szMatcher[i] != '?'))
        {
          section_len++;
        }
        else
        {
          break;
        }
        
        i++;
      }
      
      while((szMatcher[nMatchOffset] != 0) &&
        (szMatcher[nMatchOffset] != '?') &&
        (szSource[nSourOffset] != 0))
      {
        if(uni_strncmp(szMatcher + nMatchOffset, szSource + nSourOffset, section_len) == 0)
        {
          nMatchOffset += section_len;
          nSourOffset += section_len;
          
          break;
        }

        nSourOffset++;
      }
    }
  }

  return bIsMatched;
}

u8 crc_array[256] = 
{
  0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
  0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41, 
  0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
  0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 
  0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
  0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 
  0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
  0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
  0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 
  0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
  0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
  0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
  0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 
  0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 
  0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
  0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9, 
  0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 
  0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
  0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
  0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 
  0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 
  0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 
  0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 
  0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 
  0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
  0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
  0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
  0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 
  0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 
  0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8, 
  0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
  0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

u8 cacl_crc8(u8 *p_buf, u8 len)
{
  u8 crc8 = 0;

  for(len = len; len > 0; len--)
  {
    crc8 = crc_array[crc8 ^ (*p_buf)];
    p_buf++;
  }
  
  return crc8;
}
