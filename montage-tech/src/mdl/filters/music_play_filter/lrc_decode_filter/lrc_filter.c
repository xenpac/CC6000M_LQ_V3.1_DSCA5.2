/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_mutex.h"
#include "mtos_event.h"
#include "mtos_misc.h"

#include "common.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_unicode.h"
#ifdef ENABLE_ADVANCEDCA_CIPHERENGINE
#include "hal_secure.h"
#endif
#include "media_data.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"

#include "input_pin.h"
#include "output_pin.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "common_filter.h"
#include "lrc_filter.h"
#include "lrc_output_pin_intra.h"
#include "lrc_input_pin_intra.h"
#include "lrc_input_music_pin_intra.h"
#include "lrc_filter_intra.h"

#include "mt_time.h"
#include "err_check_def.h"

typedef enum
{
    LRC_NORMAL_MODE,
    LRC_SEEK_MODE,
}lrc_play_mode_t;

static lrc_filter_private_t * get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return &((lrc_filter_t *)_this)->private_data;
}
/*
BOOL isUTF8(char *rawtext,int len)
{
  s32 score = 0;
  s32 i = 0, rawtextlen = 0;
  s32 goodbytes = 0, asciibytes = 0;
  // Maybe also use UTF8 Byte Order Mark: EF BB BF
  // Check to see if characters fit into acceptable ranges
  rawtextlen = len;
  for(i = 0; i < rawtextlen; i++)
  {
    if((rawtext[i] & 0x7F) == rawtext[i])
    {
       // 最高位是0的ASCII字符
      asciibytes++;
      // Ignore ASCII, can throw off count
    }
    else if(-64 <= rawtext[i] && rawtext[i] <= -33
      //-0x40~-0x21
      && // Two bytes
      i + 1 < rawtextlen && -128 <= rawtext[i + 1]
      && rawtext[i + 1] <= -65)
    {
      goodbytes += 2;
      i++;
    }
    else if(-32 <= rawtext[i]
      && rawtext[i] <= -17
      && // Three bytes
      i + 2 < rawtextlen && -128 <= rawtext[i + 1]
      && rawtext[i + 1] <= -65 && -128 <= rawtext[i + 2]
      && rawtext[i + 2] <= -65)
    {
      goodbytes += 3;
      i += 2;
    }
  }
  if(asciibytes == rawtextlen)
  {
    return FALSE;
  }
  score = 100 * goodbytes / (rawtextlen - asciibytes);
  // If not above 98, reduce to zero to prevent coincidental matches
  // Allows for some (few) bad formed sequences
  if(score > 98)
    return TRUE;
  else if (score > 95 && goodbytes > 30)
    return TRUE;
  else
    return FALSE;

}
*/
static BOOL isUTF16L(u8 *rawtext,int len)
{
   //u16 utxt[1] = {0};

   if((rawtext[0] == 0xFF) && (rawtext[1] == 0xFE))      //UTF16 LE
   {
      return TRUE;
   }
   else if((rawtext[0] == 0xFE) && (rawtext[1] == 0xFF))  //UTF16 BE
   {

   }

   /*
    D800-DBFF  High-half zone of UTF-16
    DC00-DFFF Low-half zone of UTF-16
    */  
 /*   
   memcpy(utxt, rawtext, 2);
   if((utxt[0] > 0xDC00) && (utxt[0] < 0xDFFF))      //UTF16 LE
   {
     return TRUE;
   }
   else if((utxt[0] > 0xD800) && (utxt[0] < 0xDBFF)) //UTF16 BE
   {

   }  
*/
   return FALSE;
   
}

static mp3_lrc_filter_t *mp3_lrc_parse(unsigned char *str, u32 buf_size)
{
  unsigned long i = 0 , j = 0;
  unsigned long count = 0;
  unsigned char *p_char = 0;
  unsigned char *p_temp = 0;
  unsigned char lrc[1024] = {0};
  unsigned char *p_lrc = 0;
  mp3_lrc_filter_t *p_f = NULL;
  mp3_lrc_filter_t *p_b = NULL;
  mp3_lrc_filter_t *p_head = NULL;

  if(str == NULL)
  {
     return NULL;
  } 
  p_char = (unsigned char *)strstr((const char *)str,"[");

  if(p_char == NULL)
  {
    return NULL;
  }
  
  buf_size = buf_size - (str - p_char);
  while((p_char != NULL) && ((p_char + 1) != NULL) && (*p_char != '\0') && (buf_size > 0))
  {
    //character between 0 and 9
    if((*p_char == '[') && (*(p_char + 1) >= 48) && (*(p_char + 1) <= 57))
    {
      if(((p_char + 2) == NULL) || ((p_char + 4) == NULL) || ((p_char + 5) == NULL))
      {
         return NULL;
      }
      count = (*(p_char + 2) - '0') * 60 + (*(p_char + 4) - '0') * 10 + *(p_char + 5) - '0';
      p_temp = p_char;

      while((p_temp ++) != NULL)
      {
        if((p_temp == NULL) || ((p_temp + 1) == NULL))
        {
          return NULL;
        }
        
        if((*p_temp == ']') && (*(p_temp + 1) != '['))
        {
          p_temp ++;
          break;
        }

        j++;
        if(j > 1000)
          break;
      }

      p_lrc = lrc;
      while((p_lrc != NULL) && (p_temp != NULL) && (*p_temp != '\n') && (*p_temp != '\r'))
      {
        *p_lrc++ = *p_temp++;
      }
      //*p_lrc ++ = '\r';
      *p_lrc = '\0';

      p_b = (mp3_lrc_filter_t *)mtos_malloc(LRCLEN);
      CHECK_FAIL_RET_NULL(p_b != NULL);
      memset(p_b,0,sizeof(mp3_lrc_filter_t));
      
      p_b->start_time = count;
      strcpy((char *)p_b->lyric,(char *)lrc);

      if(p_b->lyric[0] == 0x0D)
      {
            p_b->lyric[0] = ' ';
            p_b->lyric[1] = '\0';
      }
      else if(p_b->lyric[0] == '\0')
      {
           p_b->lyric[0] = ' ';
           p_b->lyric[1] = '\0';
      }

      if(i == 0)
      {
        p_head = p_f = p_b;
        i++;
      }
      else
      {
        p_f->p_next = p_b;
        p_f = p_b;
        p_b->p_next = NULL;
      }
    }

    p_char ++;
    buf_size --;
  }

  if(p_f != NULL)
  {
     p_f->p_next = NULL;
  }
  
  return p_head;
}


static mp3_lrc_filter_t *mp3_lrc_parse_unicode(unsigned short *str, u32 buf_size)
{
  u32 x = 0;
  unsigned long i = 0 , j = 0;
  unsigned long count = 0;
  unsigned short *p_char = 0;
  unsigned short *p_temp = 0;
  unsigned short lrc[1024];
  unsigned short *p_lrc = 0;
  mp3_lrc_filter_t *p_f = NULL;
  mp3_lrc_filter_t *p_b = NULL;
  mp3_lrc_filter_t *p_head = NULL;

  if(str == NULL)
  {
     return NULL;
  }
  
  p_char = uni_strstr(str,(u16 *)"[");
  if(p_char == NULL)
  {
    return NULL;
  }
  
  buf_size = buf_size - (str - p_char);
  while((p_char != NULL) && ((p_char + 1) != NULL) && (*p_char != '\0') && (buf_size > 0))
  {
    //character between 0 and 9
    if((*p_char == '[') && (*(p_char + 1) >= 48) && (*(p_char + 1) <= 57))
    {
      if(((p_char + 2) == NULL) || ((p_char + 4) == NULL) || ((p_char + 5) == NULL))
      {
         return NULL;
      }
      
      count = (*(p_char + 2) - '0') * 60 + (*(p_char + 4) - '0') * 10 + *(p_char + 5) - '0';
      p_temp = p_char;

      while((p_temp ++) != NULL)
      {
        if((p_temp == NULL) || ((p_temp + 1) == NULL))
        {
          return NULL;
        }      
        
        if((*p_temp == ']') && (*(p_temp + 1) != '['))
        {
          p_temp ++;
          break;
        }

        j++;
        if(j > 1000)
          break;
      }

      p_lrc = lrc;
      while((p_lrc != NULL) && (p_temp != NULL) && (*p_temp != '\n') && (*p_temp != '\r'))
      {
        *p_lrc++ = *p_temp++;
      }
      //*p_lrc ++ = '\r';
      *p_lrc = '\0';

      p_b = (mp3_lrc_filter_t *)mtos_malloc(LRCLEN);
      CHECK_FAIL_RET_NULL(p_b != NULL);
      memset(p_b,0,sizeof(mp3_lrc_filter_t));
      
      p_b->start_time = count;
      //strcpy((char *)p_b->lyric,(char *)lrc);
      x = uni_strlen(lrc);
      memcpy((void *)p_b->lyric, (void *)lrc, 2 * x);

      if(p_b->lyric[0] == 0x0D)
      {
            p_b->lyric[0] = ' ';
            p_b->lyric[1] = '\0';
      }
      else if(p_b->lyric[0] == '\0')
      {
           p_b->lyric[0] = ' ';
           p_b->lyric[1] = '\0';
      }

      if(i == 0)
      {
        p_head = p_f = p_b;
        i++;
      }
      else
      {
        p_f->p_next = p_b;
        p_f = p_b;
        p_b->p_next = NULL;
      }
    }

    p_char ++;
    buf_size --;
  }

  if(p_f != NULL)
  {
     p_f->p_next = NULL;
  }
  
  return p_head;
  
}



static void lrclist_free(mp3_lrc_filter_t *head)
{
  mp3_lrc_filter_t *p_lrc = NULL;

  while(head != NULL)
  {
    p_lrc = head->p_next;
    mtos_free(head);
    head = p_lrc;
  }
  return;
}

static void mp3_lrc_sort(mp3_lrc_filter_t *head)
{
  mp3_lrc_filter_t *p_point1 = NULL;
  mp3_lrc_filter_t *p_point2 = NULL;
  mp3_lrc_filter_t *p_temp1 = NULL;
  mp3_lrc_filter_t *p_temp2 = NULL;
  mp3_lrc_filter_t st;

  if(head == NULL)
  {
    OS_PRINTF("Lycris is empty!\n");
    return;
  }

  p_point1 = p_point2 = head;

  while(p_point1 != NULL)
  {
    p_point2 = p_point1;

    while(p_point2 != NULL)
    {
      if(p_point1->start_time > p_point2->start_time)
      {
        p_temp1 = p_point1->p_next;
        p_temp2 = p_point2->p_next;

        st = *p_point1;
        *p_point1 = *p_point2;
        *p_point2 = st;

        p_point2->p_next = p_temp2;
        p_point1->p_next = p_temp1;
      }

      p_point2 = p_point2->p_next;
    }

    p_point1 = p_point1->p_next;
  }
}

static void fill_endtime_and_pre_point(mp3_lrc_filter_t *head)
{
  mp3_lrc_filter_t *p_point = NULL ,*p_temp = NULL;

  if(head == NULL)
  {
    OS_PRINTF("Lycris is empty!\n");
    return;
  }

  p_point = head;
  while(p_point != NULL)
  {
       if(p_point->p_next)
     {
       p_point->end_time = p_point->p_next->start_time;

       p_temp = p_point->p_next;
       p_temp->p_pre = p_point;
     }
     else
     {
       p_point->end_time = 300; //total play time,should fix
     }

       p_point = p_point->p_next;
  }

}

static RET_CODE lrc_parse_transform(handle_t _this, media_sample_t *p_in)
{
  lrc_filter_private_t *p_priv = get_priv(_this);
  transf_output_pin_t *p_transf_out_pin = NULL;
  media_sample_t *p_output_sample = NULL;
  str_param_t *p_str = NULL;
  u32 c_time = 0, s_time = 0, e_time = 0;
  u8  show_cunt = 0, play_line = 0;//, i = 0;
  mp3_lrc_filter_t *p_start = NULL;
  u8 focus_line = (p_priv->lrc_push_cunt + 1) / 2;
  BOOL is_found = FALSE;
  str_data_t *p_temp = NULL;
  u8 lrc_data[256] = "Unable to find matching lyrics";
  
  if(p_in->format.stream_type == MT_AUDIO)
  {
    p_transf_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
    p_transf_out_pin->query_sample(p_transf_out_pin, &p_output_sample);

    if(p_output_sample == NULL)
    {
      return ERR_FAILURE;
    }

    if(p_priv->p_head == NULL)
    {
      p_str = (str_param_t *)p_output_sample->p_data;

      if(p_priv->is_clear == FALSE)
      {
        p_str->fmt = STR_COLOR;
        p_str->str_union.clr_data.color = 0;
        p_transf_out_pin->push_down(p_transf_out_pin,p_output_sample); 

        p_priv->is_clear = TRUE;
      }
      p_str->fmt = STR_ASCII;

      p_str->str_union.str_data.lines = 1;

      p_str->str_union.str_data.p_data = (str_data_t *)((u8 *)p_str + sizeof(str_param_t));

      p_temp = p_str->str_union.str_data.p_data;
      
      p_temp->str_style = 0;
      p_temp->str_len = (((strlen(lrc_data)) + 3) & (~3));
      p_temp->align = STR_CENTER;

      p_temp->p_str = (void *)((u8 *)p_temp + sizeof(str_data_t));

      memcpy(p_temp->p_str, lrc_data, p_temp->str_len + 1);

      p_output_sample->format.stream_type = MT_FILE_DATA;
      p_transf_out_pin->push_down(p_transf_out_pin,p_output_sample);       

      return SUCCESS;
    }

    c_time = p_in->data_offset;

    if((NULL == p_priv->p_lrc_curr) || (p_output_sample->payload > 0))
    {
      return ERR_FAILURE;
    }

    p_start = p_priv->p_lrc_curr;
#if 1
    //try to caculate the start line.
    while(p_start != NULL)
    {
      play_line++;

      s_time = p_start->start_time;
      e_time   = p_start->end_time;

      if(s_time <= c_time && e_time >= c_time)
      {
        is_found = TRUE;        
        break;
      }
      
      p_start = p_start->p_next;
    };

    if(is_found)
    {
      if(play_line <= focus_line)
      {
        p_start = p_priv->p_lrc_curr;
      }
      else
      {/*
        i = play_line - focus_line;

        while(i != 0)
        {
          p_start = p_start->p_next;
          
          i--;
        }*/

        if((play_line - focus_line) > 1)    //for seek  
        {
           u8 i = play_line - focus_line;
           while(i != 0)
           {
               p_priv->p_lrc_curr = p_priv->p_lrc_curr->p_next;
               i --;
           }
           
           p_start = p_priv->p_lrc_curr;
        }
        else                                     //normal play
        {
            p_priv->p_lrc_curr = p_priv->p_lrc_curr->p_next;
            p_start = p_priv->p_lrc_curr;
        }
        
      }
    }
    else
    {
      p_start = p_priv->p_lrc_curr;
    }

    CHECK_FAIL_RET_CODE(p_start != NULL);
#else
    if(c_time > p_priv->p_lrc_curr->end_time)
    {
       p_priv->p_lrc_curr = p_priv->p_lrc_curr->p_next;
    }

    if(NULL == p_priv->p_lrc_curr)
    {
      return ERR_FAILURE;
    }    

    p_start = p_priv->p_lrc_curr;
#endif
    p_str = 
      (str_param_t *)mtos_align_malloc(p_priv->lrc_push_cunt * (260 + sizeof(str_data_t)), 8);

    p_str->fmt = p_priv->lrc_fmt;

    p_str->str_union.str_data.p_data = (str_data_t *)p_output_sample->p_data + sizeof(str_param_t);

    p_temp = p_str->str_union.str_data.p_data;

    while((p_start != NULL) && (show_cunt < p_priv->lrc_push_cunt))
    {
      s_time = p_start->start_time;
      e_time = p_start->end_time;

      if(s_time <= c_time && e_time > c_time)
      {
        p_temp->str_style = STR_HL;
      }      
      else
      {
        p_temp->str_style = 0;
      }
      p_temp->align = STR_CENTER;
      if(p_priv->lrc_fmt == STR_UNICODE)
      {
         p_temp->str_len = 2 * (((uni_strlen((u16 *)p_start->lyric) + 1) + 3) & (~3));
      }
      else
      {
         p_temp->str_len = (((strlen(p_start->lyric) + 1) + 3) & (~3));
      }
      p_temp->p_str = (void *)((u8 *)p_temp + sizeof(str_data_t));
      /* exceeds the given buffer? */
      if ((p_temp->p_str + p_temp->str_len) >= (p_output_sample->p_data + p_output_sample->total_buffer_size - sizeof(str_data_t)))
      {
        OS_PRINTF("buffer is not enough for storing lrc.\n");
        p_temp->str_len = p_output_sample->p_data + p_output_sample->total_buffer_size - p_temp->p_str;

        /*store the extra '0'*/
        if(p_priv->lrc_fmt == STR_UNICODE)
        {
          if (p_temp->str_len < 4)
            break;
          p_temp->p_str[p_temp->str_len - 1] = 0;
          p_temp->p_str[p_temp->str_len - 2] = 0;
          p_temp->str_len -= 2;
        }
        else
        {
          if (p_temp->str_len < 2)
            break;
          p_temp->p_str[p_temp->str_len - 1] = 0;
          p_temp->str_len -= 1;
        }

        memcpy(p_temp->p_str, p_start->lyric, p_temp->str_len);
        
        break;
      }

      memcpy(p_temp->p_str, p_start->lyric, p_temp->str_len);
      
      p_start = p_start->p_next;

      p_temp = (str_data_t *)((u8 *)p_temp + p_temp->str_len + sizeof(str_data_t));
      
      show_cunt++;
    }

    p_str->str_union.str_data.lines = show_cunt;

    p_output_sample->format.stream_type = MT_FILE_DATA;
    p_output_sample->p_data = (u8 *)p_str;

    p_transf_out_pin->push_down(p_transf_out_pin,p_output_sample);

    mtos_align_free((void *)p_str);
  }
  else
  {
    
    if(p_in->payload == 0)
    {
       p_priv->p_head = NULL;
       return SUCCESS;
    }
    
    p_in->p_data[p_in->payload] = '\n';

    p_priv->lrc_fmt = STR_GB2312;
/*    
    if(isUTF8((char *)p_in->p_data, 1024) == TRUE)
    {
      p_priv->lrc_fmt = STR_UTF8;
    }
*/
    if(isUTF16L(p_in->p_data, 1024) == TRUE)
    {
      p_priv->lrc_fmt = STR_UNICODE;
    }

    if(p_priv->lrc_fmt == STR_UNICODE)
    {
      p_priv->p_head = mp3_lrc_parse_unicode((u16 *)p_in->p_data, p_in->payload);
      //CHECK_FAIL_RET_CODE(p_priv->p_head != NULL);
    }
    else
    {
      p_priv->p_head = mp3_lrc_parse(p_in->p_data, p_in->payload);
      //CHECK_FAIL_RET_CODE(p_priv->p_head != NULL);     
    }

    if(p_priv->p_head != NULL)
    {
      mp3_lrc_sort(p_priv->p_head);
      fill_endtime_and_pre_point(p_priv->p_head);
      p_priv->p_lrc_curr = p_priv->p_head;

    }
    
  }

  return SUCCESS;
}

static RET_CODE lrc_on_command(handle_t _this, icmd_t *p_cmd)
{
  lrc_filter_private_t *p_priv = get_priv(_this);

  switch (p_cmd->cmd)
  {
    case LRC_CUNT_SET:
      p_priv->lrc_push_cunt = p_cmd->lpara; 
      CHECK_FAIL_RET_CODE(p_priv->lrc_push_cunt != 0);
      break;
    case LRC_SEEK:
      p_priv->play_mode = LRC_SEEK_MODE;
      p_priv->seek_time = p_cmd->lpara / 1000;
      OS_PRINTF("lrc on command,lrc seek, time: %d\n", p_priv->seek_time);
      break;

    default:
      return ERR_FAILURE;
  }
  return SUCCESS;
}

static RET_CODE lrc_on_close(handle_t _this)
{
   lrc_filter_private_t *p_priv = get_priv(_this);

   lrclist_free(p_priv->p_head);
   p_priv->p_head = NULL;
   p_priv->p_lrc_curr = NULL;

   p_priv->is_clear = FALSE;

   p_priv->play_mode = 0;
   p_priv->seek_time = 0;

   return SUCCESS;
}

static void lrc_on_destroy(handle_t _this)
{
  mtos_free((void *)_this);
}


ifilter_t *lrc_filter_create(void *p_para)
{
  lrc_filter_private_t *p_priv = NULL;
  lrc_filter_t *p_ins = NULL;
  interface_t *p_interface = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_t *p_transffilter = NULL;
  transf_filter_para_t transf_filter_para;

  transf_filter_para.dummy = 0;
  //check input parameter
  p_ins = mtos_malloc(sizeof(lrc_filter_t));
  CHECK_FAIL_RET_NULL(p_ins != NULL);
  memset(p_ins, 0, sizeof(lrc_filter_t));

  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(lrc_filter_private_t));
  p_priv->p_this = p_ins; //this point
  p_priv->lrc_push_cunt = 1;

  //over loading the virtual function for ifilter_t
  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = lrc_on_command;
  p_ifilter->on_close = lrc_on_close;
  p_ifilter->_interface.on_destroy = lrc_on_destroy;  

  //over loading the virtual function for transf_filter_t
  p_transffilter = (transf_filter_t *)p_ins;
  p_transffilter->transform = lrc_parse_transform;

  p_interface = (interface_t *)p_ins;
  lrc_in_pin_create(&p_priv->m_in_pin, p_interface);
  lrc_out_pin_create(&p_priv->m_out_pin, p_interface);

  lrc_music_in_pin_create(&p_priv->m_music_in_pin, p_interface);

  return (ifilter_t *)p_ins;
}

/*!
  end of file
  */

