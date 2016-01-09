/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*!
 \file gui_resource.c
   this file  implement the functions defined in  gui_resource.h, also it implement some internal used
   function. All these functions are about how to decribe, set and draw a pbar control.
 */
#include "sys_types.h"
#include "sys_define.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"
#include "charsto.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"
#include "pdec.h"
#include "common.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "ctrl_base.h"

#include "gui_resource.h"
#include "vf_ft.h"
#include "gui_xml.h"
#include "class_factory.h"

#include "LzmaIf.h"
#include "lz4.h"

#include "ctrl_base.h"

#include "gui_xml.h"

//#define RSC_DEBUG
#ifdef RSC_DEBUG
#define RSC_PRINTF    OS_PRINTF
#else
#define RSC_PRINTF DUMMY_PRINTF
#endif

/*!
  resource type.
  */
typedef enum 
{
  /*!
    bitmap
    */
  RSC_COMM_BMP = 1,
  /*!
    palette
    */
  RSC_COMM_PAL,
  /*!
    local font
    */
  RSC_COMM_FONT,
  /*!
    string table
    */
  RSC_COMM_STRTAB,
  /*!
    script
    */
  RSC_COMM_SCRIPT
}rsc_comm_type_t;

/*!
  global variable for saving information about resource memory.
  */
//-static rsc_main_t g_rsc_info;

/*!
  Internal used API, to decompressed resource in rle mode.

  \param[in] rsc_handle : resource handle.
  \param[in] p_input : input data
  \param[in] data_size : data size before compress.
  \param[out] p_output : output data.
  
  \return : data size afer compress.
  */
static u32 rsc_rle_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output);

/*!
  get decompressed resource data.

  \param[in] rsc_handle : resource handle.
  \param[in] hdr_addr : Resource header.
  
  \return : decompressed data.
  */
static u8 *rsc_decompress_data(handle_t rsc_handle, rsc_comm_type_t type);

static rsc_comm_info_t *_get_rsc_comm_info(handle_t rsc_handle, rsc_comm_type_t type)
{
  rsc_comm_info_t *p_comm_info = NULL;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  switch(type)
  {
    case RSC_COMM_BMP:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->bmp_info;
      break;
      
    case RSC_COMM_PAL:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->palette_info;
      break;
      
    case RSC_COMM_FONT:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->font_info;
      break;

    case RSC_COMM_SCRIPT:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->script_info;
      break;

    case RSC_COMM_STRTAB:
      p_comm_info = (rsc_comm_info_t *)&p_rsc_info->strtab_info;
      break;

    default:
      p_comm_info = NULL;
  }

  return p_comm_info;
}

static inline BOOL rsc_is_data_on_flash(u32 addr)
{
  charsto_device_t *p_charsto = (charsto_device_t *)dev_find_identifier(
    NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);

  return (dev_io_ctrl(
    p_charsto, CHARSTO_IOCTRL_IS_VALID_ADDR, addr) == SUCCESS) ? TRUE : FALSE;
}


static rsc_head_t *_get_rsc_comm_head(rsc_comm_info_t *p_comm_info)
{
  u32 addr = 0;
  MT_ASSERT(p_comm_info != NULL);

  addr = (u32)p_comm_info;
  addr += sizeof(rsc_comm_info_t);

  return (rsc_head_t *)(addr);
}

static u32 lz_read_var_size(u32 *p_x, u8 *p_buf)
{
  u32 y = 0, b = 0, num_bytes = 0;

  y = 0;
  num_bytes = 0;
  
  do
  {
    b = (u32)(*p_buf++);
    y = (y << 7) | (b & 0x0000007f);
    ++ num_bytes;
  }
  while(b & 0x00000080);

  *p_x = y;

  return num_bytes;
}

static u32 rsc_lz77_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output)
{
  u8 marker = 0, symbol = 0;
  u32 i = 0, inpos = 0, outpos = 0, length = 0, offset = 0;

  if(data_size < 1)
  {
    return 0;
  }

  marker = p_input[0];
  inpos = 1;

  outpos = 0;
  do
  {
    symbol = p_input[inpos++];
    if(symbol == marker)
    {
      if(p_input[inpos] == 0)
      {
        p_output[outpos++] = marker;
        ++inpos;
      }
      else
      {
        inpos += lz_read_var_size(&length, &p_input[inpos]);
        inpos += lz_read_var_size(&offset, &p_input[inpos]);

        for(i = 0; i < length; ++i)
        {
          p_output[outpos] = p_output[outpos - offset];
          ++outpos;
        }
      }
    }
    else
    {
      p_output[outpos++] = symbol;
    }
  }
  while(inpos < data_size);

  return 0;
}

static u32 rsc_jpg_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output)
{
  u8 *p_buff = NULL;
  pic_info_t pic_info = {0};
  pdec_ins_t pic_ins = {0};
  pic_param_t pic_param = {0};
  drv_dev_t *p_pic_dev = NULL;
  RET_CODE ret = SUCCESS;

  MT_ASSERT(p_input != NULL);
  MT_ASSERT(p_output != NULL);
  MT_ASSERT(data_size != 0);

  p_buff = mtos_align_malloc(data_size, 8);

  MT_ASSERT(p_buff != NULL);

  rsc_read_data(rsc_handle, (u32)p_input, p_buff, data_size);

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);  

  ret = pdec_getinfo(p_pic_dev, p_buff, data_size, &pic_info, &pic_ins);
  MT_ASSERT(ret == SUCCESS);
  
  pic_param.dec_mode = DEC_FRAME_MODE;
  pic_param.scale_w_num = 1;
  pic_param.scale_w_demo = 1;
  pic_param.scale_h_num = 1;
  pic_param.scale_h_demo = 1;
  pic_param.disp_width = pic_info.src_width;
  pic_param.disp_height = pic_info.src_height;      
  pic_param.p_src_buf = p_buff;
  pic_param.p_dst_buf = p_output;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.output_format = PIX_FMT_ARGB8888;  
  
  pic_param.src_size = data_size;
  
  ret = pdec_setinfo(p_pic_dev, &pic_param, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  ret = pdec_start(p_pic_dev, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  ret = pdec_stop(p_pic_dev, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  if(p_buff != NULL)
  {
    mtos_align_free(p_buff);
  }

  return pic_param.disp_stride * 4;
}

#if 1

/*!
   RLE decode for some resource data.

 \param[in] p_input : point to input data
 \param[in] data_size : input data size
 \param[out] p_output : point to output buffer
 \return : data size after decode
 */
static u32 rsc_rle_decode(handle_t rsc_handle, u8 *p_input, u32 data_size, u8 *p_output)
{
#if 0  
  u32 output_pos = 0;    //offset in output buffer
  u32 input_pos = 0;      //offset in input buffer
  u8 i = 0;                //position in temporary buffer
  u8 data;
  u8 size = 0;

  u8 *buf_addr;
  u32 unread_size, read_size, buf_size;

  buf_addr = rsc_get_comm_buf_addr();
  buf_size = rsc_get_comm_buf_size();
  
  unread_size = data_size;

  read_size = unread_size > buf_size ? buf_size : unread_size;
  unread_size -= read_size;
  rsc_read_data((u32)(input + input_pos), buf_addr, read_size);

  do
  {
    if(i < read_size)//read data from buffer
    {
      size = buf_addr[i];
    }
    else                        //read another nbytes from flash
    {
      read_size = unread_size > buf_size ? buf_size : unread_size;
      unread_size -= read_size;
      rsc_read_data((u32)(input + input_pos), buf_addr, read_size);
      i = 0;      
      size = buf_addr[i];   
    }
    input_pos++;
    i++;
    
    if(size <128)
    {
      if(i < read_size)//read data from buffer
      {
        data = buf_addr[i];
      }
      else                        //data is in another nbyters.
      {
        read_size = unread_size > buf_size ? buf_size : unread_size;
        unread_size -= read_size;
        rsc_read_data((u32)(input + input_pos), buf_addr, read_size);
        i = 0;
        data = buf_addr[i];
      }
      memset((output + output_pos), data, size);
      i ++;
      input_pos ++;
    }
    else
    {
      size -=128;
      if((i + size) < read_size)//can be read from memory completely
      {
        rsc_read_data((u32)(buf_addr + i), (output + output_pos), size);
      }
      else                                    //read from flash
      {
        rsc_read_data((u32)(input + input_pos), (output + output_pos), size);
      }
      i += size;
      input_pos += size;
    }
    output_pos += size;
  }while(input_pos < data_size);

  return output_pos;
#else
  u32 output_pos = 0;    //offset in output buffer
  u32 input_pos = 0;      //offset in input buffer
  u8 buffer[DATA_BUF_SIZE];
  u8 i = 0;                //position in temporary buffer
  u8 data = 0;
  u8 size = 0;

  rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);

  do
  {
    if(i < DATA_BUF_SIZE)//read data from buffer
    {
      size = buffer[i];
    }
    else                        //read another nbytes from flash
    {
      rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);
      i = 0;      
      size = buffer[i];   
    }
    input_pos++;
    i++;
    
    if(size <128)
    {
      if(i < DATA_BUF_SIZE)//read data from buffer
      {
        data = buffer[i];
      }
      else                        //data is in another nbyters.
      {
        rsc_read_data(rsc_handle, (u32)(p_input + input_pos), buffer, DATA_BUF_SIZE);
        i = 0;
        data = buffer[i];
      }
      memset((p_output + output_pos), data, size);
      i ++;
      input_pos ++;
    }
    else
    {
      size -=128;
      if((i + size) < DATA_BUF_SIZE)//can be read from memory completely
      {
        rsc_read_data(rsc_handle, (u32)(buffer + i), (p_output + output_pos), size);
      }
      else                                    //read from flash
      {
        rsc_read_data(rsc_handle, (u32)(p_input + input_pos),
          (p_output + output_pos), size);
      }
      i += size;
      input_pos += size;
    }
    output_pos += size;
  }while(input_pos != data_size);

  return output_pos;
#endif

}

#else
static u32 rle_decode(u8 *rle, u32 *len, u8 *buf)
{
  u32 i, last, out_size;
  u8 size;

  out_size = 0;
  i = 0, last = *len - 1;
  do
  {
    size = *(rle + i);
    if(size < 128)
    {
      if(i > last)
      {
        break;
      }
      memset(buf + out_size, *(rle + i + 1), size);
      i += 2;
    }
    else
    {
      size -= 128;
      if(size + i > last)
      {
        break;
      }
      memcpy(buf + out_size, rle + i + 1, size);
      i += size + 1;
    }
    out_size += size;
  } while(i < *len);

  *len = i;
  return out_size;
}

#endif

/* external API */
/*!
    Get the resource head by it's resource type and id.

 \param[in] type 		: resource type
 \param[in] id		 	: resource id
 \return 				: if failed, return NULL, otherwise return the head of the resource.
 */
u32 rsc_get_hdr_addr(handle_t rsc_handle, u8 type, u16 id)
{
  rsc_info_t info;
  rsc_idx_t idx = {0};
  BOOL find = FALSE;
  u32 offset = 0;
  u16 i = 0;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(RSC_INVALID_ID == id)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr rsc_invalid_id \n\n");
    return 0;
  }
  offset = (u32)(p_rsc_info->p_rsc_data_addr + sizeof(rsc_info_t) * type);
  rsc_read_data(rsc_handle, offset, &info, sizeof(rsc_info_t));
#ifdef RSC_DEBUG
  rsc_dump_info(&info);
#endif

  if(info.type != type)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr offset:0x%x, info.type:%d, type:%d \n\n",
               offset,info.type,type);
    return 0;
  }

  offset = (u32)(p_rsc_info->p_rsc_data_addr + info.offset);
  if(type != RSC_TYPE_SCRIPT)
  {
    /* Notice: the index of resource must be continuous for this solution */
    offset += (id - 1/* id is base on 1*/) * sizeof(rsc_idx_t);
    rsc_read_data(rsc_handle, offset, &idx, sizeof(rsc_idx_t));
    find = (BOOL)(id == idx.id);
  }
  else
  {
    find = FALSE;
    for(i = 0; i < info.count; i++)
    {
      rsc_read_data(rsc_handle, offset, &idx, sizeof(rsc_idx_t));
#ifdef RSC_DEBUG
      rsc_dump_idx(&idx);
#endif

      if(id == idx.id)
      {
        find = TRUE;
        break;
      }
      offset += sizeof(rsc_idx_t);
    }
  }
  if(!find)
  {
    OS_PRINTF("\n\n rsc_get_hdr_addr rsc not find \n\n");
    return 0;
  }

  return (u32)(p_rsc_info->p_rsc_data_addr + info.offset + idx.offset);

}


/*!
  Get a decompressed resource data by it's head.

  \param[in] rsc_head	: resource head
  \return : if failed, return NULL, otherwise return the buffer address of the data
  */
static u8 *rsc_decompress_data(handle_t rsc_handle, rsc_comm_type_t type)
{
  u32 *p_buf_addr = NULL;
  u32 data_addr = 0, org_size = 0, cmp_size = 0, buf_size = 0, out_size = 0;
  rsc_bmp_info_t *p_bmp_info = NULL;
  rsc_head_t *p_hdr_rsc = NULL;
  rsc_comm_info_t *p_comm_info = NULL;
  u8 *p_fake = NULL;

  p_comm_info = _get_rsc_comm_info(rsc_handle, type);
  if(p_comm_info == NULL)
  {
    OS_PRINTF("rsc_decompress_data: can NOT get comm_info(type = %d), ERROR\n",
              type);
    return NULL;
  }
  p_hdr_rsc = _get_rsc_comm_head(p_comm_info);

  p_buf_addr = &p_comm_info->unzip_buf_addr;
  buf_size = p_comm_info->unzip_buf_size;

  switch(type)
  {
    case RSC_COMM_FONT:      
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_font_t);
      break;
    case RSC_COMM_PAL:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_palette_t);
      break;
    case RSC_COMM_BMP:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_bitmap_t);
      break;
    case RSC_COMM_STRTAB:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_string_t);
      break;
    case RSC_COMM_SCRIPT:
      data_addr = p_comm_info->rsc_hdr_addr + sizeof(rsc_script_t);
      break;
    default:
      MT_ASSERT(0);
      return NULL;
  }

  org_size = p_hdr_rsc->org_size;
  cmp_size = p_hdr_rsc->cmp_size;
  
  switch(p_hdr_rsc->c_mode)
  {
    case  CMP_MODE_NONE:
      if(buf_size > 0)
      {
        /* buffer is ok, copy to buf */
        if(org_size > buf_size)
        {
          RSC_PRINTF(
            "RSC: buf_size is non-zero(%d), but NOT enough(%d), ERROR!\n",
            org_size, buf_size);
          MT_ASSERT(0);
        }
        else
        {
          rsc_read_data(rsc_handle, data_addr, (void *)(*p_buf_addr), org_size);
        }
      }
      else
      {
        /* no buffer, return address on FLASH */
        *p_buf_addr = data_addr;
      }
      break;

    case CMP_MODE_RLE:
      //os_mutex_lock(lock);
      if(org_size > buf_size)
      {
        RSC_PRINTF(
          "RSC: buf_size is non-zero(%d), but NOT enough(%d), ERROR!\n",
          org_size, buf_size);
        MT_ASSERT(0);
      }
      else
      {
        rsc_rle_decode(rsc_handle, (u8 *)data_addr, cmp_size, (u8 *)(*p_buf_addr));
      }
      break;

#if 0
    case CMP_MODE_ZIP:
      rsc_unzip(data, output);
      break;
#endif
    case CMP_MODE_LZ77:
      rsc_lz77_decode(rsc_handle, (u8 *)data_addr, cmp_size, (u8 *)(*p_buf_addr));
      break;

    case CMP_MODE_LZMA:
      out_size = buf_size;
      
      p_fake = mtos_malloc(cmp_size);
      MT_ASSERT(p_fake != NULL);     
      
      init_fake_mem_lzma(p_fake, cmp_size);
      
      lzma_decompress((unsigned long *)(*p_buf_addr), &out_size, (void *)data_addr, cmp_size);

      mtos_free(p_fake);
      break;

    case CMP_MODE_LZ4:
      LZ4_decompress_fast((const char *)data_addr, (char *)(*p_buf_addr), org_size);
      break;      

    case CMP_MODE_JPG: /*only for bitmap*/
      if(RSC_TYPE_BITMAP != p_hdr_rsc->type)
      {
        return NULL;
      }

      p_bmp_info = (rsc_bmp_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_BMP);
      MT_ASSERT(p_bmp_info != NULL);
      
      p_bmp_info->hdr_bmp.pitch = 
        rsc_jpg_decode(rsc_handle, (u8 *)data_addr, cmp_size, (u8 *)(*p_buf_addr));
      break;

    default:
      return NULL;
  }

  return (u8 *)(*p_buf_addr);
}

/*!
    Get a rectangle style by style id

 \param[in] idx		: rectangle style id
 \return 			: a pointer point to the specified rstyle
 */
rsc_rstyle_t *rsc_get_rstyle(handle_t rsc_handle, u32 idx)
{
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  if(idx >= XML_RSTYLE_START)
  {
    return xml_get_rstyle(idx);
  }
  
  if(idx < p_rsc_info->rstyle_cnt)
  {
    return(p_rsc_info->p_rstyle_tab + idx);
  }
  else
  {
    return p_rsc_info->p_rstyle_tab;
  }
}

/*!
    Determine a rectangle style has a background or not

 \param[in] p_rstyle		: a pointer point to the specified rstyle
 \return 			: has background or not.
 */
BOOL rsc_is_rstyle_has_bg(rsc_rstyle_t *p_rstyle)
{
  // Using simple rules to determine now
  return (!RSTYLE_IS_R_IGNORE(p_rstyle->bg)
          && !RSTYLE_IS_R_COPY_BG(p_rstyle->bg));
}


/*!
    Determine a rectangle style need copy its background or not

 \param[in] p_rstyle		: a pointer point to the specified rstyle
 \return 			: need or not.
 */
BOOL rsc_is_rstyle_cpy_bg(rsc_rstyle_t *p_rstyle)
{
  // Using simple rules to determine now
  return RSTYLE_IS_R_COPY_BG(p_rstyle->bg);
}


/*!
   Get font style by style id.

 \param[in] idx		:font style id
 \return 			: a pointer point to the specified fstyle
 */
rsc_fstyle_t *rsc_get_fstyle(handle_t rsc_handle, u32 idx)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  if(idx >= XML_FSTYLE_START)
  {
    return xml_get_fstyle(idx);
  }
  else
  {
    MT_ASSERT(idx < p_rsc_info->fstyle_cnt);
    return (p_rsc_info->p_fstyle_tab + idx);
  }
}


BOOL rsc_get_font(handle_t rsc_handle, u16 font_id, rsc_font_t *p_hdr_font, u8 **p_data)
{
  u32 addr = 0;
  rsc_font_info_t *p_font_info = NULL;

  if(font_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_font_info = (rsc_font_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_FONT);
  MT_ASSERT(p_font_info != NULL);
 
  if(p_font_info->comm.id == font_id)
  {
    *p_data = (u8 *)p_font_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_FONT, font_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_font_info->comm.id = font_id;
    p_font_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_font_info->hdr_font, sizeof(rsc_font_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_FONT);
  }

  memcpy(p_hdr_font, &p_font_info->hdr_font, sizeof(rsc_font_t));
  return TRUE;
}

//need mutex protect
/*!
  Get decompressed bitmap data.

  \param[in] bmp_id			: bitmap id
  \param[out] hdr_bmp			: a point pointer to the bitmap header
  \param[out] data			: a point pointer to the bitmap data buffer
  \return 					: bitmap resource head got by it's id
  */
BOOL rsc_get_bmp(handle_t rsc_handle, u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 **p_data)
{
  u32 addr = 0;
  rsc_bmp_info_t *p_bmp_info = NULL;

  if(bmp_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_bmp_info = (rsc_bmp_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_BMP);
  MT_ASSERT(p_bmp_info != NULL);
 
  if(p_bmp_info->comm.id == bmp_id)
  {
    *p_data = (u8 *)p_bmp_info->comm.unzip_buf_addr;
  }
  else if(bmp_id >= XML_BMAP_START)
  {
    *p_data = (u8 *)p_bmp_info->comm.unzip_buf_addr;

    p_bmp_info->comm.id = bmp_id;
    
    xml_get_bmp(bmp_id, p_hdr_bmp, *p_data, &p_bmp_info->comm.rsc_hdr_addr);

    memcpy(&p_bmp_info->hdr_bmp, p_hdr_bmp, sizeof(rsc_bitmap_t));
    return TRUE;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_BITMAP, bmp_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_bmp_info->comm.id = bmp_id;
    p_bmp_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_bmp_info->hdr_bmp, sizeof(rsc_bitmap_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_BMP);
  }

  memcpy(p_hdr_bmp, &p_bmp_info->hdr_bmp, sizeof(rsc_bitmap_t));
  return TRUE;
}


/*!
    Get a string data by language id and string id.

 \param[in] id				: language id
 \param[in] string_idx		: string id
 \return					: string buffer
 */
BOOL rsc_get_string(handle_t rsc_handle, u16 strtab_id, u16 string_id, u8 **p_data)
{
  u16 offset = 0;
  u32 addr = 0;
  rsc_strtab_info_t *p_strtab_info = NULL;

  if(string_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_strtab_info = (rsc_strtab_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_STRTAB);
  MT_ASSERT(p_strtab_info != NULL);

  // check string table 
  if(p_strtab_info->comm.id == strtab_id)
  {
    addr = p_strtab_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_STRING, strtab_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }
    
    p_strtab_info->comm.id = strtab_id;
    p_strtab_info->comm.rsc_hdr_addr = addr;
    
    rsc_read_data(rsc_handle, addr, &p_strtab_info->hdr_strtab, sizeof(rsc_string_t));
    addr = (u32)rsc_decompress_data(rsc_handle, RSC_COMM_STRTAB);
  }

  // read the offset of string
  rsc_read_data(rsc_handle,
    addr + ((string_id - 1/* base on 1*/) << 1), &offset, sizeof(u16));
  
  *p_data = (u8 *)(addr + offset);

  return TRUE;
}

/*!
    Get palette resource.

 \param[in] palette_id		: palette id
 \param[out] entrys			: a point pointer to the palette buffer
 \return 					: palette resource head got by it's id
 */
BOOL rsc_get_palette(handle_t rsc_handle, u16 palette_id, rsc_palette_t *p_hdr_pal, u8 **p_data)
{
  u32 addr = 0;
  rsc_pal_info_t *p_pal_info = NULL;

  OS_PRINTF("pal id %d\n", palette_id);

  if(palette_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_pal_info = (rsc_pal_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_PAL);
  MT_ASSERT(p_pal_info != NULL);

  if(p_pal_info->comm.id == palette_id)
  {
    *p_data = (u8 *)p_pal_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_PALETTE, palette_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_pal_info->comm.id = palette_id;
    p_pal_info->comm.rsc_hdr_addr = addr;

    rsc_read_data(rsc_handle, addr, &p_pal_info->hdr_pal, sizeof(rsc_palette_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_PAL);
  }
  
  memcpy(p_hdr_pal, &p_pal_info->hdr_pal, sizeof(rsc_palette_t));
  return TRUE;
}

BOOL rsc_get_script(handle_t rsc_handle, u16 spt_id, rsc_script_t *p_hdr_spt, u8 **p_data)
{
  u32 addr = 0;
  rsc_script_info_t *p_spt_info = NULL;

  if(spt_id == RSC_INVALID_ID)
  {
    return FALSE;
  }

  p_spt_info = (rsc_script_info_t *)_get_rsc_comm_info(rsc_handle, RSC_COMM_SCRIPT);
  MT_ASSERT(p_spt_info != NULL);

  if(p_spt_info->comm.id == spt_id)
  {
    *p_data = (u8 *)p_spt_info->comm.unzip_buf_addr;
  }
  else
  {
    addr = rsc_get_hdr_addr(rsc_handle, RSC_TYPE_SCRIPT, spt_id);
    if(addr == 0)
    {
      *p_data = NULL;
      return FALSE;
    }

    p_spt_info->comm.id = spt_id;
    p_spt_info->comm.rsc_hdr_addr = addr;

    rsc_read_data(rsc_handle, addr, &p_spt_info->hdr_spt, sizeof(rsc_script_t));
    *p_data = rsc_decompress_data(rsc_handle, RSC_COMM_SCRIPT);
  }
  
  memcpy(p_hdr_spt, &p_spt_info->hdr_spt, sizeof(rsc_script_t));
  return TRUE;
}

/*!
    Get string attrib.

 \param[in] char_code	: unicode of the character
 \param[in] style		: font style
 \param[out] p_width		: width of the character
 \param[out] p_height		: height of the character
 \return 				: NULL
 */
void rsc_get_char_attr(handle_t rsc_handle, u16 char_code,
  rsc_fstyle_t *p_style, u16 *p_width, u16 *p_height)
{
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  if(p_rsc_info->get_attr == NULL)
  {
    *p_width = 0;
    *p_height = 0;
    
    return;
  }

  p_rsc_info->get_attr(p_rsc_info->p_priv, char_code, p_style, p_width, p_height);
}

/*!
    Get string attrib.

 \param[in] str			: head of the font resource
 \param[in] style		: font style
 \param[out] width		: width of the character
 \param[out] height		: height of the character
 \return 				: SUCCESS
 */
s32 rsc_get_string_attr(handle_t rsc_handle, u16 *p_str, rsc_fstyle_t *p_style, u16 *p_width,
                            u16 *p_height)
{
  u16 char_width = 0, char_height = 0;
  u16 i = 0, blank = 0;
  u16 buf[DATA_BUF_SIZE];
  u16 *p_buf_addr = NULL;
  BOOL is_use_buf = rsc_is_data_on_flash((u32)p_str);

  *p_width = 0;
  *p_height = 0;

  p_buf_addr = is_use_buf ? buf : p_str;

  if(is_use_buf)
  {
    rsc_read_data(rsc_handle, (u32)p_str, p_buf_addr, DATA_BUF_SIZE * sizeof(u16));
  }
  
  while(0 != p_buf_addr[i])
  {
    rsc_get_char_attr(rsc_handle, p_buf_addr[i], p_style, &char_width, &char_height);
    *p_width += char_width;
    *p_height = MAX(*p_height, char_height);
    i++;

    if(is_use_buf)
    {
      if(i == DATA_BUF_SIZE)
      {
        p_str += DATA_BUF_SIZE;
        i = 0;
        rsc_read_data(rsc_handle, (u32)p_str, p_buf_addr, DATA_BUF_SIZE * sizeof(u16));
      }
    }
  }


  if(p_buf_addr[0] == 0)//actually it's "\n", temp solution
  {
    blank = char_asc2uni(' ');
    rsc_get_char_attr(rsc_handle, blank, p_style, &char_width, &char_height);

    *p_height = MAX(*p_height, char_height);

  }
  return SUCCESS;  
}
/*!
    Get a character resource data.

 \param[in] head		: head of the font resource
 \param[in] style		: font style
 \param[in] char_code	: unicode of the character
 \param[out] width		: width of the character
 \param[out] height		: height of the character
 \param[out] color_key	: color key of the character
 \return 				: if success, return the data buffer, else return null.
 */
BOOL rsc_get_char(handle_t rsc_handle,
  rsc_fstyle_t *p_fstyle, u16 char_code, rsc_char_info_t *p_info)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  MT_ASSERT(p_fstyle != NULL);
  MT_ASSERT(p_info != NULL);

  if(p_rsc_info->get_char == NULL)
  {
    return FALSE;
  }
  
  return p_rsc_info->get_char(p_rsc_info->p_priv, char_code, p_fstyle, p_info);
}


/*!
    Set current language type.

 \param[in] language		: language id.
 \return 					: NULL
 */
void rsc_set_curn_language(handle_t rsc_handle, u16 language)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_rsc_info->curn_language_id = language;
}


/*!
    Get language id

 \return 			: language id
 */
u16 rsc_get_curn_language(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->curn_language_id;
}


/*!
   set current language.

 \param[in]language			: palette id
 \return					: NULL
 */
void rsc_set_curn_palette(handle_t rsc_handle, u16 palette)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_rsc_info->curn_palette_id = palette;
}

/*!
   get current language.

 \return					: palette id
 */
u16 rsc_get_curn_palette(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->curn_palette_id;
}


static void *rsc_alloc(handle_t rsc_handle, u32 size)
{
  rsc_main_t *p_rsc_info = NULL;
  void *p_addr = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_addr = (void *)lib_memp_alloc(&p_rsc_info->heap, size);

  //p_addr = mtos_dma_malloc(size);

  memset(p_addr, 0, size);

  return p_addr;
}

static void *rsc_align_alloc(handle_t rsc_handle, u32 size, u32 alignment)
{
  rsc_main_t *p_rsc_info = NULL;
  void *p_addr = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  p_addr = (void *)lib_memp_align_alloc(&p_rsc_info->heap, size, alignment);

  memset(p_addr, 0, size);

  return p_addr;
}

static void rsc_align_free(handle_t rsc_handle, void *p_buf)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  lib_memp_align_free(&p_rsc_info->heap, p_buf);
}


static void rsc_free(handle_t rsc_handle, void *p_buf)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  lib_memp_free(&p_rsc_info->heap, p_buf);
}


u8 * rsc_get_comm_buf_addr(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(p_rsc_info->comm_buf_size == 0)
  {
    return NULL;
  }

  return p_rsc_info->p_comm_buf_addr;
}

u32 rsc_get_comm_buf_size(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  return p_rsc_info->comm_buf_size;
}

static void init_rsc_comm_info(handle_t rsc_handle, u8 type, u32 buf_size)
{
  rsc_comm_info_t *p_rsc_info = NULL;

  p_rsc_info = _get_rsc_comm_info(rsc_handle, type);
  MT_ASSERT(p_rsc_info != NULL);
  
  if(buf_size > 0)
  {
    p_rsc_info->unzip_buf_addr = (u32)rsc_align_alloc(rsc_handle, buf_size, 8);
    MT_ASSERT(p_rsc_info->unzip_buf_addr != 0);
  }

  p_rsc_info->unzip_buf_size = buf_size;
  p_rsc_info->id = RSC_INVALID_ID;
}

static void rsc_comm_info_release(handle_t res_handle, u8 type)
{
  rsc_comm_info_t *p_rsc_info = NULL;

  p_rsc_info = _get_rsc_comm_info(res_handle, type);
  MT_ASSERT(p_rsc_info != NULL);
  
  if(p_rsc_info->unzip_buf_addr != 0)
  {
    rsc_align_free(res_handle, (void *)p_rsc_info->unzip_buf_addr);
    
    p_rsc_info->unzip_buf_addr = 0;
    p_rsc_info->unzip_buf_size = 0;
    p_rsc_info->id = RSC_INVALID_ID;
  }
}

/*!
    Resource initialize, allocate memory for all resource buffer.

 \param[in] init_info		: it specified buffer size of the resource buffer
 \return 				: return ERR_FAILURE or SUCESS
 */
#define ALIGN_TO_4BYTES(x)    (((u32)(x) + 3) & (~0x3))

#define ALIGN_TO_8BYTES(x)    (((u32)(x) + 8))

handle_t rsc_init(rsc_config_t *p_config)
{
  u32 size = 0;
  rsc_main_t *p_rsc_info = NULL;

  p_rsc_info = (rsc_main_t *)mmi_alloc_buf(sizeof(rsc_main_t));
  memset((void *)p_rsc_info, 0, sizeof(rsc_main_t));
  
  p_rsc_info->p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_rsc_info->p_charsto_dev != NULL);

  p_rsc_info->p_rsc_data_addr = (u8 *)p_config->rsc_data_addr;

  // init data heap
  MT_ASSERT(p_config->comm_buf_size > 0);

  size += ALIGN_TO_8BYTES(p_config->comm_buf_size);
  size += ALIGN_TO_8BYTES(p_config->strtab_buf_size);
  size += ALIGN_TO_8BYTES(p_config->font_buf_size);
  size += ALIGN_TO_8BYTES(p_config->pal_buf_size);
  size += ALIGN_TO_8BYTES(p_config->bmp_buf_size);
  size += ALIGN_TO_8BYTES(p_config->script_buf_size);
  
  p_rsc_info->p_heap_addr = mmi_create_memp(&p_rsc_info->heap, size);
  MT_ASSERT(p_rsc_info->p_heap_addr != NULL);

  /* common data buffer */
  if(p_config->comm_buf_size > 0)
  {
    p_rsc_info->p_comm_buf_addr = rsc_alloc(p_rsc_info, p_config->comm_buf_size);
    MT_ASSERT(p_rsc_info->p_comm_buf_addr != 0);
  }
  p_rsc_info->comm_buf_size = p_config->comm_buf_size;

  /* bmp buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_BMP, p_config->bmp_buf_size);
  
  /* string table buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_STRTAB, p_config->strtab_buf_size);

  /* palette buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_PAL, p_config->pal_buf_size);

  /* font buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_FONT, p_config->font_buf_size);
  
  /* script buffer */
  init_rsc_comm_info((handle_t)p_rsc_info, RSC_COMM_SCRIPT, p_config->script_buf_size);

  /*others*/
  p_rsc_info->curn_language_id = RSC_INVALID_ID;
  p_rsc_info->curn_palette_id = RSC_INVALID_ID;

  p_rsc_info->p_rstyle_tab = p_config->p_rstyle_tab;
  p_rsc_info->rstyle_cnt = p_config->rstyle_cnt;
  p_rsc_info->p_fstyle_tab = p_config->p_fstyle_tab;
  p_rsc_info->fstyle_cnt = p_config->fstyle_cnt;
  p_rsc_info->flash_base = p_config->flash_base;

  return (handle_t)p_rsc_info;
}


/*!
    Release memory.

 \return			: NULL
 */
void rsc_release(handle_t rsc_handle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
  if(p_rsc_info->p_comm_buf_addr != NULL)
  {
    rsc_free(rsc_handle, p_rsc_info->p_comm_buf_addr);

    p_rsc_info->p_comm_buf_addr = NULL;
  }
  
  rsc_comm_info_release((handle_t)p_rsc_info, RSC_COMM_BMP);
  rsc_comm_info_release((handle_t)p_rsc_info, RSC_COMM_STRTAB);
  rsc_comm_info_release((handle_t)p_rsc_info, RSC_COMM_PAL);
  rsc_comm_info_release((handle_t)p_rsc_info, RSC_COMM_FONT);
  rsc_comm_info_release((handle_t)p_rsc_info, RSC_COMM_SCRIPT);  

  mmi_destroy_memp(&p_rsc_info->heap, p_rsc_info->p_heap_addr);

  //release global variable.
  mmi_free_buf((void *)p_rsc_info);
}

void rsc_set_fstyle(handle_t rsc_handle, u32 idx, rsc_fstyle_t *p_new_fstyle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  MT_ASSERT(idx < p_rsc_info->fstyle_cnt);

  MT_ASSERT(p_new_fstyle != NULL);

  memcpy(p_rsc_info->p_fstyle_tab + idx, p_new_fstyle, sizeof(rsc_fstyle_t));
}

void rsc_set_rstyle(handle_t rsc_handle, u32 idx, rsc_rstyle_t *p_new_rstyle)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);

  MT_ASSERT(idx < p_rsc_info->rstyle_cnt);

  MT_ASSERT(p_new_rstyle != NULL);

  memcpy(p_rsc_info->p_rstyle_tab + idx, p_new_rstyle, sizeof(rsc_rstyle_t));
}

/*!
    Get  unistring width.

 \param[in] f_style_idx		: font style id
 \param[in] str				: string data buffer
 \return 					: string width
 */
u16 rsc_get_unistr_width(handle_t rsc_handle, u16 *p_str, u32 f_style_idx)
{
  rsc_fstyle_t *p_fstyle = NULL;
  u16 str_height = 0, str_width = 0;

  p_fstyle = rsc_get_fstyle(rsc_handle, f_style_idx);

  if(rsc_get_string_attr(rsc_handle, p_str, p_fstyle, &str_width, &str_height) != SUCCESS)
  {
    return 0;
  }

  return str_width;
}


/*!
    Get a string width by style id and string id.

 \param[in] f_style_idx		: font style id
 \param[in] string_id		: string id
 \return 					: string width
 */
u16 rsc_get_strid_width(handle_t rsc_handle, u16 string_id, u32 f_style_idx)
{
  u8 *p_bits = NULL;
  u16 str_width = 0;

//  rsc_lock_data(RSC_TYPE_STRING);
  if(!rsc_get_string(rsc_handle, rsc_get_curn_language(rsc_handle), string_id, (u8 **)&p_bits))
  {
    return 0;
  }
  str_width = rsc_get_unistr_width(rsc_handle, (u16 *)p_bits, f_style_idx);
//  rsc_unlock_data(RSC_TYPE_STRING);
  return str_width;
}


void rsc_read_data(handle_t rsc_handle, u32 offset, void *p_buf, u32 len)
{
  rsc_main_t *p_rsc_info = NULL;
  
  p_rsc_info = (rsc_main_t *)rsc_handle;
  MT_ASSERT(p_rsc_info != NULL);
  
//  static u32 times = 0;

  if(rsc_is_data_on_flash(offset)) //read data from flash
  {
#ifdef __LINUX__
if(offset > 0x90000000)
{
charsto_read(p_rsc_info->p_charsto_dev,
             (offset - p_rsc_info->flash_base), (u8 *)p_buf, len);
}
else
{
    memcpy(p_buf, (void *)offset, len);
}
#else
        charsto_read(p_rsc_info->p_charsto_dev,
             (offset - p_rsc_info->flash_base), (u8 *)p_buf, len);

#endif
//    times++;
//    OS_PRINTF("Oh....rsc read data %d tims\n", times);
  }
  else //read data from flash
  {
    if((offset == 0) || (p_buf == NULL))
    {
      OS_PRINTF("0x%x, 0x%x, %d\n", (u32)p_buf, offset, len);
    }
    memcpy(p_buf, (void *)offset, len);
  }
}


