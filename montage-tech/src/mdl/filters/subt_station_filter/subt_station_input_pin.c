/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_util.h"
#include "lib_rect.h"

#include "drv_dev.h"
#include "common.h"
#include "drv_misc.h"
#include "vbi_inserter.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "ifilter.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_filter.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"

#include "common_filter.h"
#include "dmx.h"
#include "demux_interface.h"
#include "demux_filter.h"
#include "subt_station_database.h"
#include "subt_station_filter.h"
#include "subt_station_parse.h"
#include "subt_station_input_pin_intra.h"
//#include "subt_station_input_pts_pin_intra.h"
#include "subt_station_output_pin_intra.h"
#include "subt_station_filter_intra.h"

#ifndef EOF
#define EOF (-1)
#endif

static const u8 ff_log2_tab[256]={
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static inline const int av_log2(unsigned int v)
{
    int n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}


#define GET_UTF8(val, GET_BYTE, ERROR)\
    val = GET_BYTE;\
    {\
        int ones = 7 - av_log2(val ^ 255);\
        if(ones == 1)\
            ERROR\
        val &= 127 >> ones;\
        while(--ones > 0){\
            int tmp = GET_BYTE - 128;\
            if(tmp >> 6)\
                ERROR\
            val = (val << 6) + tmp;\
        }\
    }

static subt_station_in_pin_priv_t *get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);
  return &((subt_station_in_pin_t *)_this)->private_data;
}

static void sync_request(handle_t _this, media_sample_t *p_sample, u32 size, u32 pos)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  iasync_reader_t *p_reader = p_priv->p_reader;

  p_sample->context = 1; // seek mode
  p_reader->sync_read(p_reader, p_sample, size, pos);
}

static void request_sample(handle_t _this, media_sample_t *p_sample, u32 pos, BOOL seek)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  iasync_reader_t *p_reader = p_priv->p_reader;

  p_reader->request(p_reader, &p_sample->format, LINE_MAX_SIZE, 0, pos, NULL, seek);
}

static RET_CODE get_line(u8 *p_data, u32 *p_len)
{
  int ch = 0;
  u32 i = 0;
  s32 ret = 0;

  while(i < *p_len)
  {
    ch = p_data[i];
    if ('\n' == ch)      
      break;
    else if (EOF == ch)
    {
      ret = -2;
      break;
    }
    else
      i++;

    if(i >= LINE_MAX_SIZE)
    {
     // EPRINT(" line is too long!!");
      ret = -1;
      break;
    }
  }

  if(i == *p_len)
  {
    return -3;
  }
  
  i ++;
  *p_len = i;


  return ret;
}


/*
 *		A poor man's conversion of Unicode to UTF8
 */
static void to_utf8(u8 *dst, const char *src, unsigned int cnt)
{
  unsigned int ch = 0;
  unsigned int i = 0;

  for (i = 0; i < cnt; i ++)
  {
    ch = *src ++ & 255;
    ch += (*src ++ & 255) << 8;
    if (ch < 0x80)
    *dst ++ = ch;
    else if (ch < 0x1000)
    {
      *dst ++ = 0xc0 + (ch >> 6);
      *dst ++ = 0x80 + (ch & 63);
    }
    else
    {
      *dst ++ = 0xe0 + (ch >> 12);
      *dst ++ = 0x80 + ((ch >> 6) & 63);
      *dst ++ = 0x80 + (ch & 63);
    }
  }
  *dst = 0;
}

static int utf8_len(const u8 *b)
{
    int len = 0;
    int val = 0;
    while(*b){
        GET_UTF8(val, *b++, return -1;)
        len++;
    }
    return len;
}

// return bytes
static s32 _uni_line_len(const u16 *p_str, s32 len)
{
  const u16 *p_eos = p_str;

  while(*p_eos != 0xd && *(p_eos + 1) != 0xa)
  {
    len -= 2;
    p_eos ++;
    if(len <= 0)
      return -1;
  }

  return (u32)((u32)p_eos - (u32)p_str + 4);
}

static BOOL is_utf8(char *rawtext,int len)
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
       // ¡Á?????¨º?0¦Ì?ASCII¡Á?¡¤?
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

static RET_CODE sst_in_pin_pre_parse(handle_t _this, u8 *p_data, u32 len)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  RET_CODE ret = ERR_FAILURE;
  subt_station_key_point_t key = {0};

  if(p_priv->sst_type == SUBT_SYS_UNKOWN)
  {
    p_priv->sst_type = subt_station_get_type(p_data, len);
  }

  if((p_priv->sst_type != SUBT_SYS_UNKOWN))
  {
    ret = subt_station_scan_func(p_data, len, p_priv->sst_type, &key);
    key.pos = p_priv->parse_pos;
  }

  if(ret == SUCCESS)
  {
    // add key point to db
   // OS_PRINTF("start %d, pos %x\n", key.start, key.pos);
    //if(key.pos == 0x3f8d)
    //  OS_PRINTF("kk");
    sst_db_save_out_item(&key);
    
  }
  return ret;
}

RET_CODE sst_in_pin_pre_scan(handle_t _this)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  ipin_t *p_ipin = (ipin_t *)_this;
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  media_sample_t sample = {{0}};
  u32 size = 2 * KBYTES;
  u8 *p_line = NULL;
  u8 *p_read = NULL;
  u32 line_len = 0;
  u32 remain_len = 0;
  s8 ret = 0;


  if(!p_priv->enable_pre_scan 
    || p_priv->scan_state == SST_IN_STATE_END)
  {
    //OS_PRINTF("pre_scan end\n");
    return SUCCESS;
  }
  
  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
    (void **)&p_priv->p_reader);
  // scanning the input file, save the key points
  sample.p_data = mtos_malloc(size);
  sample.total_buffer_size = size;
  p_priv->scan_state = SST_IN_STATE_REQUEST;
  
  while(p_priv->scan_state != SST_IN_STATE_ON_CYCLE_DONE
          && p_priv->scan_state != SST_IN_STATE_END)
  {
    switch(p_priv->scan_state)
    {
      case SST_IN_STATE_REQUEST:
        sync_request(_this, &sample, size, p_priv->parse_pos);
        p_read = sample.p_data;
        remain_len = sample.payload;
        if(remain_len == 0)
        {
          p_priv->scan_state = SST_IN_STATE_END;
        }    
        else
        {
          p_priv->scan_state = SST_IN_STATE_GET_LINE;
          if(p_priv->codec_fmt == SUBT_CODEC_UNKNOW)
          {
            if(*p_read == 0xFF && *(p_read +1) == 0xFE)
            {//unicode
              p_priv->codec_fmt = SUBT_CODEC_UNICODE;
              //ignore the 0xff 0xfe
              p_read += 2;
              p_priv->parse_pos += 2; 
            }
          }
          
        }
        break;
      case SST_IN_STATE_GET_LINE:
        
        if(p_priv->codec_fmt != SUBT_CODEC_UNICODE)
        {
          line_len = remain_len;
          ret = get_line(p_read, &line_len);
          if(ret == -3)// not found the '\n'
          {
            memset(p_priv->p_pre_scan_buf, 0, LINE_MAX_SIZE);
            p_priv->scan_state = SST_IN_STATE_ON_CYCLE_DONE;
            break;
          }
          else if (ret == 0)
          {
            remain_len -= line_len;
            memcpy(p_priv->p_pre_scan_buf, p_read, line_len);
            p_read += line_len;
            p_line = p_priv->p_pre_scan_buf;
            p_priv->scan_state = SST_IN_STATE_PARSE;
          }
        }
        else
        {
          line_len =  _uni_line_len((u16 *)p_read, remain_len);
          if(line_len == -1)
          {
            memset(p_priv->p_pre_scan_buf, 0, LINE_MAX_SIZE);
            p_priv->scan_state = SST_IN_STATE_ON_CYCLE_DONE;
            break;
          }
          remain_len -= line_len;
          to_utf8(p_priv->p_pre_scan_buf, p_read, (line_len - 4) / 2);
          p_read += line_len;
          p_line = p_priv->p_pre_scan_buf;
          p_priv->scan_state = SST_IN_STATE_PARSE;
        }
        break;
      case SST_IN_STATE_PARSE:
        if(p_priv->codec_fmt == SUBT_CODEC_UNICODE)
        {
          sst_in_pin_pre_parse(_this, p_line, (line_len - 4) / 2);
        }
        else
        {
          sst_in_pin_pre_parse(_this, p_line, line_len);
        }
        p_priv->parse_pos += line_len;
        p_priv->scan_state = SST_IN_STATE_GET_LINE;
        break;
      case SST_IN_STATE_END:
      default:
        break;
    }
  }
  mtos_free(sample.p_data);
  
  return SUCCESS;
}

static void sst_in_pin_on_receive(handle_t _this, media_sample_t *p_sample)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  u8 *p_data = NULL;
  u32 data_len = 0;
  RET_CODE ret = ERR_FAILURE;
  subt_station_content_t content = {0};
  ipin_t *p_this = (ipin_t *)_this;
  os_msg_t msg = {0};

  u32 cur_pts = p_sample->context;
  OS_PRINTF("sst_in_pin_on_receive ...\n");
  p_data = p_sample->p_data;
  data_len = p_sample->payload;


  if(data_len == 0)
  {
    OS_PRINTF("no data input...\n");
    return;
  }

  if(p_priv->codec_fmt == SUBT_CODEC_UNKNOW)
  {
    if(is_utf8(p_data, data_len))
    {
      p_priv->codec_fmt = SUBT_CODEC_UTF8;
    }
    else
    {
      p_priv->codec_fmt = SUBT_CODEC_GB2312;
    }
  }
  else if(p_priv->codec_fmt == SUBT_CODEC_UNICODE)
  {
    //data_len = _uni_line_len((u16 *)p_data, data_len);
    to_utf8(p_priv->p_pre_scan_buf, p_data, (data_len - 4) / 2);
    data_len = utf8_len(p_priv->p_pre_scan_buf);
    p_data = p_priv->p_pre_scan_buf;
  }
  
  if(p_priv->sst_type == SUBT_SYS_UNKOWN)
  {
    p_priv->sst_type = subt_station_get_type(p_data, data_len);
  }

  if((p_priv->sst_type != SUBT_SYS_UNKOWN))
  {
    ret = subt_station_parse_func(p_data, data_len, p_priv->sst_type, &content);
    content.codec_fmt = p_priv->codec_fmt;
    if(content.codec_fmt == SUBT_CODEC_UNICODE)
    {
      content.codec_fmt = SUBT_CODEC_UTF8;
    }
    OS_PRINTF("out codec %d, %s\n", content.codec_fmt, &content.event[0][0]);
  }

  if(ret == SUCCESS)
  {
    // show it
    //the input data is currently, show it, else ignore
    if(cur_pts >= content.start
      || cur_pts == 1) // == 1 meas seek mode
    {
      //notify filter to show
      msg.content = SST_INT_SHOW_SUBT;
      msg.para1 = (u32)&content;
      OS_PRINTF("SST_INT_SHOW_SUBT\n");
      p_this->send_message_to_filter(p_this, &msg);
      p_priv->last_start = content.start;
      p_priv->last_end = content.end;      
    }
  }
}


static RET_CODE sst_in_pin_on_open(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE sst_in_pin_on_start(handle_t _this)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);


  p_priv->codec_fmt = SUBT_CODEC_UNKNOW;

  if(!p_priv->enable_pre_scan)
  {
    return SUCCESS;
  }
  sst_in_pin_pre_scan(_this);
  
  return SUCCESS;
}

static RET_CODE sst_in_pin_on_stop(handle_t _this)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);

  p_priv->sst_type = SUBT_SYS_UNKOWN;
  p_priv->codec_fmt = SUBT_CODEC_UNKNOW;
  return SUCCESS;
}


static void sst_in_report(handle_t _this, void *p_state)
{
  
}


RET_CODE sst_in_pin_request_data(handle_t _this, u32 start, u32 pos, BOOL seek)
{
  ipin_t *p_ipin = (ipin_t *)_this;
  media_sample_t sample = {{0}};
  ipin_t *p_connecter = (ipin_t *)p_ipin->get_connected(_this);
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);

  p_connecter->get_interface(p_connecter, IASYNC_READER_INTERFACE,
                            (void **)&p_priv->p_reader);

  memset(&sample, 0, sizeof(media_sample_t));
  sample.context = start;
  request_sample(_this, &sample, pos, seek);
  return SUCCESS;
}

RET_CODE sst_in_pin_process_internal_subt(handle_t _this, media_sample_t *p_sample)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  u8 *p_data = NULL;
  u32 data_len = 0;
  RET_CODE ret = ERR_FAILURE;
  subt_station_content_t content = {0};
  ipin_t *p_this = (ipin_t *)_this;
  os_msg_t msg = {0};
  u32 cur_pts = p_sample->context;
  subt_station_in_item_t item = {0};
//  OS_PRINTF("sst_in_pin_process_sample ...\n");


  ret = sst_db_find_in_item(cur_pts, &item);
  if(ret != SUCCESS)
  {
    return ERR_FAILURE;
  }

  p_data = item.p_data;
  /*Use actual len of the data, otherwise there is some dirty data after the string.*/
  data_len = item.data_len;
  if((p_priv->sst_type != SUBT_SYS_UNKOWN))
  {
    ret = subt_station_parse_func(p_data, data_len, p_priv->sst_type, &content);
    //content.codec_fmt = p_priv->codec_fmt;
    //default the internal utf8.. because the utf8 detect is not correct sometimes
    content.codec_fmt = SUBT_CODEC_UTF8; 
    OS_PRINTF("codec %d, %s\n", content.codec_fmt, &content.event[0][0]);
  }
  
  if(ret == SUCCESS)
  {
    // show it
    //the input data is currently, show it, else ignore
    if(cur_pts >= content.end)
    {
      //timeout
      return SUCCESS;
    }
    
    if(cur_pts >= content.start
      && cur_pts <= content.end) // == 1 meas seek mode
    {
      //notify filter to show
      msg.content = SST_INT_SHOW_SUBT;
      msg.para1 = (u32)&content;
      OS_PRINTF("SST_INT_SHOW_SUBT pts %d\n", cur_pts);
      p_this->send_message_to_filter(p_this, &msg);
    }
    else 
    {
        /*! 
         * Return err means this subtitle is not shown, 
         * use p_priv->subt_data, don't read from file seq
         */
        ret = ERR_FAILURE;
    }
  }

  return ret;
}



RET_CODE sst_in_pin_pre_parse_in_subt(handle_t _this, media_sample_t *p_sample)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);
  u8 *p_data = NULL;
  u32 data_len = 0;
  RET_CODE ret = ERR_FAILURE;
  subt_station_content_t content = {0};
  subt_station_in_item_t item = {0};

  p_data = p_sample->p_data;
  data_len = p_sample->payload;
  
  if(p_priv->codec_fmt == SUBT_CODEC_UNKNOW)
  {
    if(is_utf8(p_data, data_len))
    {
      p_priv->codec_fmt = SUBT_CODEC_UTF8;
    }
    else
    {
      p_priv->codec_fmt = SUBT_CODEC_GB2312;
    }
  }

  if(p_priv->sst_type == SUBT_SYS_UNKOWN)
  {
    p_priv->sst_type = subt_station_get_type(p_data, data_len);
  }

  ret = subt_station_parse_func(p_data, data_len, p_priv->sst_type, &content);
  if(ret != SUCCESS)
  {
    return ret;
  }
  //check this content with db
  if(content.start < p_priv->last_start)
  {
    //reset db
    sst_db_reset();
  }

  //save it in db
  item.start = content.start;
  item.end = content.end;
  item.p_data = p_data;
  item.data_len = data_len;
  sst_db_save_in_item(&item);
  p_priv->last_start = content.start;
  OS_PRINTF("save item start %d end %d RET %d\n", item.start, item.end, ret);
  return ret;
}

void sst_in_pin_config(handle_t _this, u8 type, u8 is_internal)
{
  subt_station_in_pin_priv_t *p_priv = get_priv(_this);

  p_priv->sst_type = type;
  p_priv->enable_pre_scan = !is_internal;
}



subt_station_in_pin_t * subt_station_in_pin_create(
    subt_station_in_pin_t *p_pin, interface_t *p_owner)
{
  subt_station_in_pin_priv_t *p_priv = NULL;
  base_input_pin_t *p_input_pin = NULL;
  transf_input_pin_para_t transf_pin_para = {0};
  ipin_t *p_ipin = NULL;
  media_format_t media_format = {MT_FILE_DATA};
  interface_t *p_interface = (interface_t *)p_pin;
  
  //check input parameter
  MT_ASSERT(p_pin != NULL);
  MT_ASSERT(p_owner != NULL);

  //create base class
  transf_pin_para.p_filter = p_owner;
  transf_pin_para.p_name = "subt_station_in_pin";
  transf_input_pin_create(&p_pin->m_pin, &transf_pin_para);

  p_interface->report = sst_in_report;
  //init private date
  p_priv = (subt_station_in_pin_priv_t *)&p_pin->private_data;
  memset(p_priv, 0, sizeof(subt_station_in_pin_priv_t));
  p_priv->p_this = (u8 *)p_pin; //this point

  //init member function

  //overload virtual function
  p_input_pin = (base_input_pin_t *)p_pin;
  p_input_pin->on_receive = sst_in_pin_on_receive;

  p_ipin = (ipin_t *)p_pin;
  p_ipin->add_supported_media_format(p_ipin, &media_format);
  p_ipin->on_open = sst_in_pin_on_open;
  p_ipin->on_start = sst_in_pin_on_start;
  p_ipin->on_stop = sst_in_pin_on_stop;

  return p_pin;
}

