/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"

#include "sys_define.h"
#include "mtos_printk.h"
#include "epg_util.h"


epg_type_t epg_type [] =
{
  {0x00, "undefined content"},
  {0x01, "undefined content"},
  {0x02, "undefined content"},
  {0x03, "undefined content"},
  {0x04, "undefined content"},
  {0x05, "undefined content"},
  {0x06, "undefined content"},
  {0x07, "undefined content"},
  {0x08, "undefined content"},
  {0x09, "undefined content"},
  {0x0a, "undefined content"},
  {0x0b, "undefined content"},
  {0x0c, "undefined content"},
  {0x0d, "undefined content"},
  {0x0e, "undefined content"},
  {0x0f, "undefined content"},
    
  {0x10, "movie/drama (general)"},
  {0x11, "detective/thriller"},
  {0x12, "adventure/western/war"},
  {0x13, "science fiction/fantasy/horror"},
  {0x14, "comedy"},
  {0x15, "soap/melodrama/folkloric"},
  {0x16, "romance"},
  {0x17, "serious/classical/religious/historical movie/drama"},
  {0x18, "adult movie/drama"},
  {0x19, "reserved for future use"},
  {0x1a, "reserved for future use"},
  {0x1b, "reserved for future use"},
  {0x1c, "reserved for future use"},
  {0x1d, "reserved for future use"},
  {0x1e, "reserved for future use"},
  {0x1f, "user defined"},

  {0x20, "news/current affairs (general)"},
  {0x21, "news/weather report"},
  {0x22, "news magazine"},
  {0x23, "documentary"},
  {0x24, "discussion/interview/debate"},
  {0x25, "reserved for future use"},
  {0x26, "reserved for future uset"},
  {0x27, "reserved for future use"},
  {0x28, "reserved for future use"},
  {0x29, "reserved for future use"},
  {0x2a, "reserved for future use"},
  {0x2b, "reserved for future use"},
  {0x2c, "reserved for future use"},
  {0x2d, "reserved for future use"},
  {0x2e, "reserved for future use"},
  {0x2f, "user defined"},

  {0x30,  "show/game show (general)"},
  {0x31,  "game show/quiz/contest"},
  {0x32,  "variety show"},
  {0x33,  "talk show"},
  {0x34,  "reserved for future use"},
  {0x35,  "reserved for future use"},
  {0x36,  "reserved for future use"},
  {0x37,  "reserved for future use"},
  {0x38,  "reserved for future use"},
  {0x39,  "reserved for future use"},
  {0x3a,  "reserved for future use"},
  {0x3b,  "reserved for future use"},
  {0x3c,  "reserved for future use"},
  {0x3d,  "reserved for future use"},
  {0x3e,  "reserved for future use"},
  {0x3f,  "user defined"},

  {0x40,  "sports (general)"},
  {0x41,  "special events (Olympic Games, World Cup, etc.)"},
  {0x42,  "sports magazines"},
  {0x43,  "football/soccer"},
  {0x44,  "tennis/squash"},
  {0x45,  "team sports (excluding football)"},
  {0x46,  "athletics"},
  {0x47,  "motor sport"},
  {0x48,  "water sport"},
  {0x49,  "winter sports"},
  {0x4a,  "equestrian"},
  {0x4b,  "martial sports"},
  {0x4c,  "reserved for future use"},
  {0x4d,  "reserved for future use"},
  {0x4e,  "reserved for future use"},
  {0x4f,  "user define"},

  {0x50,  "children's/youth programmes (general)"},
  {0x51,  "pre-school children's programmes"},
  {0x52,  "entertainment programmes for 6 to14"},
  {0x53,  "entertainment programmes for 10 to 16"},
  {0x54,  "informational/educational/school programmes"},
  {0x55,  "cartoons/puppets"},
  {0x56,  "reserved for future use"},
  {0x57,  "reserved for future use"},
  {0x58,  "reserved for future use"},
  {0x59,  "reserved for future use"},
  {0x5a,  "reserved for future use"},
  {0x5b,  "reserved for future use"},
  {0x5c,  "reserved for future use"},
  {0x5d,  "reserved for future use"},
  {0x5e,  "reserved for future use"},
  {0x5f,  "user defined"},

  {0x60,  "music/ballet/dance (general)"},
  {0x61,  "rock/pop"},
  {0x62,  "serious music/classical music"},
  {0x63,  "folk/traditional music"},
  {0x64,  "jazz"},
  {0x65,  "musical/opera"},
  {0x66,  "ballet"},
  {0x67,  "reserved for future use"},
  {0x68,  "reserved for future use"},
  {0x69,  "reserved for future use"},
  {0x6a,  "reserved for future use"},
  {0x6b,  "reserved for future use"},
  {0x6c,  "reserved for future use"},
  {0x6d,  "reserved for future use"},
  {0x6e,  "reserved for future use"},
  {0x6f,  "user defined"},

  {0x70,  "arts/culture (without music, general)"},
  {0x71,  "performing arts"},
  {0x72,  "fine arts"},
  {0x73,  "religion"},
  {0x74,  "popular culture/traditional arts"},
  {0x75,  "literature"},
  {0x76,  "film/cinema"},
  {0x77,  "experimental film/video"},
  {0x78,  "broadcasting/press"},
  {0x79,  "new media"},
  {0x7a,  "arts/culture magazines"},
  {0x7b,  "fashion"},
  {0x7c,  "reserved for future use"},
  {0x7d,  "reserved for future use"},
  {0x7e,  "reserved for future use"},
  {0x7f,  "user defined"},

  {0x80,  "social/political issues/economics (general)"},
  {0x81,  "magazines/reports/documentary"},
  {0x82,  "economics/social advisory"},
  {0x83,  "remarkable people"},
  {0x84,  "reserved for future use"},
  {0x85,  "reserved for future use"},
  {0x86,  "reserved for future use"},
  {0x87,  "reserved for future use"},
  {0x88,  "reserved for future use"},
  {0x89,  "reserved for future use"},
  {0x8a,  "reserved for future use"},
  {0x8b,  "reserved for future use"},
  {0x8c,  "reserved for future use"},
  {0x8d,  "reserved for future use"},
  {0x8e,  "reserved for future use"},
  {0x8f,  "user defined"},

  {0x90,  "education/science/factual topics(general)"},
  {0x91,  "nature/animals/environment"},
  {0x92,  "technology/natural sciences"},
  {0x93,  "medicine/physiology/psychology"},
  {0x94,  "foreign countries/expeditions"},
  {0x95,  "social/spiritual sciences"},
  {0x96,  "further education"},
  {0x97,  "languages"},
  {0x98,  "reserved for future use"},
  {0x99,  "reserved for future use"},
  {0x9a,  "reserved for future use"},
  {0x9b,  "reserved for future use"},
  {0x9c,  "reserved for future use"},
  {0x9d,  "reserved for future use"},
  {0x9e,  "reserved for future use"},
  {0x9f,  "user defined"},

  {0xa0,  "leisure hobbies (general)"},
  {0xa1,  "tourism/travel"},
  {0xa2,  "handicraft"},
  {0xa3,  "motoring"},
  {0xa4,  "fitness and health"},
  {0xa5,  "cooking"},
  {0xa6,  "advertisement/shopping"},
  {0xa7,  "gardening"},
  {0xa8,  "reserved for future use"},
  {0xa9,  "reserved for future use"},
  {0xaa,  "reserved for future use"},
  {0xab,  "reserved for future use"},
  {0xac,  "reserved for future use"},
  {0xad,  "reserved for future use"},
  {0xae,  "reserved for future use"},
  {0xaf,  "user defined"},

  {0xb0,  "original language"},
  {0xb1,  "black and white"},
  {0xb2,  "unpublished"},
  {0xb3,  "live broadcast"},
  {0xb4,  "reserved for future use"},
  {0xb5,  "reserved for future use"},
  {0xb6,  "reserved for future use"},
  {0xb7,  "reserved for future use"},
  {0xb8,  "reserved for future use"},
  {0xb9,  "reserved for future use"},
  {0xba,  "reserved for future use"},
  {0xbb,  "reserved for future use"},
  {0xbc,  "reserved for future use"},
  {0xbd,  "reserved for future use"},
  {0xbe,  "reserved for future use"},
  {0xbf,  "user defined"},

  {0xc0,  "reserved for future use"},
  {0xc1,  "reserved for future use"},
  {0xc2,  "reserved for future use"},
  {0xc3,  "reserved for future use"},
  {0xc4,  "reserved for future use"},
  {0xc5,  "reserved for future use"},
  {0xc6,  "reserved for future use"},
  {0xc7,  "reserved for future use"},
  {0xc8,  "reserved for future use"},
  {0xc9,  "reserved for future use"},
  {0xca,  "reserved for future use"},
  {0xcb,  "reserved for future use"},
  {0xcc,  "reserved for future use"},
  {0xcd,  "reserved for future use"},
  {0xce,  "reserved for future use"},
  {0xcf,  "reserved for future use"},

  {0xd0,  "reserved for future use"},
  {0xd1,  "reserved for future use"},
  {0xd2,  "reserved for future use"},
  {0xd3,  "reserved for future use"},
  {0xd4,  "reserved for future use"},
  {0xd5,  "reserved for future use"},
  {0xd6,  "reserved for future use"},
  {0xd7,  "reserved for future use"},
  {0xd8,  "reserved for future use"},
  {0xd9,  "reserved for future use"},
  {0xda,  "reserved for future use"},
  {0xdb,  "reserved for future use"},
  {0xdc,  "reserved for future use"},
  {0xdd,  "reserved for future use"},
  {0xde,  "reserved for future use"},
  {0xdf,  "reserved for future use"},

  {0xe0,  "reserved for future use"},
  {0xe1,  "reserved for future use"},
  {0xe2,  "reserved for future use"},
  {0xe3,  "reserved for future use"},
  {0xe4,  "reserved for future use"},
  {0xe5,  "reserved for future use"},
  {0xe6,  "reserved for future use"},
  {0xe7,  "reserved for future use"},
  {0xe8,  "reserved for future use"},
  {0xe9,  "reserved for future use"},
  {0xea,  "reserved for future use"},
  {0xeb,  "reserved for future use"},
  {0xec,  "reserved for future use"},
  {0xed,  "reserved for future use"},
  {0xee,  "reserved for future use"},
  {0xef,  "reserved for future use"},

  {0xf0,  "user defined"},
  {0xf1,  "user defined"},
  {0xf2,  "user defined"},
  {0xf3,  "user defined"},
  {0xf4,  "user defined"},
  {0xf5,  "user defined"},
  {0xf6,  "user defined"},
  {0xf7,  "user defined"},
  {0xf8,  "user defined"},
  {0xf9,  "user defined"},
  {0xfa,  "user defined"},
  {0xfb,  "user defined"},
  {0xfc,  "user defined"},
  {0xfd,  "user defined"},
  {0xfe,  "user defined"},
  {0xff,  "user defined"},
};

u8 *epg_get_type(u8 index)
{
  return epg_type[index].type;
}


void epg_fifo_flush(epg_fifo_t *p_fifo)
{
    if(p_fifo == 0)
    {
        return;
    }
    
    p_fifo->m_cnt  = 0;
    p_fifo->m_head = 0;
    p_fifo->m_tail = 0;
}



void epg_fifo_put(epg_fifo_t *p_fifo, u32 data)
{
    if(p_fifo == 0)
    {
        return;
    }

    if(p_fifo->m_cnt < p_fifo->m_size) 
    {
        /* buffer isn't full */
        p_fifo->p_buffer[p_fifo->m_head] = data;
        
        p_fifo->m_head = (p_fifo->m_head + 1) % p_fifo->m_size;
        p_fifo->m_cnt++;
        return;
    }
    else if(p_fifo->m_overlay == TRUE) 
    {
        /* buffer is full, but it's overlay*/
        p_fifo->p_buffer[p_fifo->m_head] = data;
        
        p_fifo->m_head = (p_fifo->m_head + 1) % p_fifo->m_size; 
        p_fifo->m_tail = (p_fifo->m_tail + 1) % p_fifo->m_size;
        return;
    }
    
}


BOOL epg_fifo_get(epg_fifo_t *p_fifo, u32 *p_data)
{
    if((p_fifo == 0) || (p_data == 0))
    {
        return FALSE;
    }
    
    if(p_fifo->m_cnt > 0) 
    {
        /* buffer isn't empty */
        *p_data = p_fifo->p_buffer[p_fifo->m_tail];

        p_fifo->m_tail = (p_fifo->m_tail + 1) % p_fifo->m_size;
        p_fifo->m_cnt--;
        
        return TRUE;
    }

    return FALSE;
}

