/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#ifdef ENABLE_ADS
#include "ads_ware.h"
#include "ui_ad_api.h"
#endif
#include "ui_picture_api.h"

typedef struct bmpfile_magic {
  unsigned char magic[2];
}BMP_MAGIC;
 
typedef struct bmpfile_header {
  u32 filesz;
  u16 reserve1;
  u16 reserve2;
  u32 bmp_offset;
}BMP_HEADER;

typedef struct dib_header{
  u32 header_sz;
  s32 width;
  s32 height;
  u16 nplanes;
  u16 bitspp;
  u32 compress_type;
  u32 bmp_bytesz;
  s32 hres;
  s32 vres;
  u32 ncolors;
  u32 nimpcolors;
}DIB_HEADER;

#define BMP_HEADER_SIZE (sizeof(BMP_MAGIC)+sizeof(BMP_HEADER))
#define DIB_HEADER_SIZE  (sizeof(DIB_HEADER))

static void* p_sub_rgn = NULL;
static void* p_montage_ads_sub_rgn = NULL;
u8 com_num = 0;
#ifdef ENABLE_NETWORK
static void *p_net_svc = NULL;
#endif
#ifdef ENABLE_ADS
static u8 g_montage_ads_pic_id = 0;
static pdec_ins_t pic_ins = {IMAGE_FORMAT_UNKNOWN, };
static u8 *p_output = NULL;
#endif
u8 g_led_buffer[10] = {0};
u8 g_led_index = 0;
rect_t g_water_mark_rect = {0};

static BOOL g_uio_enable = FALSE;
void ui_enable_uio(BOOL is_enable)
{
  cmd_t cmd = {0};

  if(g_uio_enable && is_enable)
  {
  	 return;
  }
  else if(g_uio_enable == FALSE && is_enable == FALSE)
  {
  	 return;
  }
  
  if(is_enable == TRUE)
  	{
  	   g_uio_enable = TRUE;
  	}
	else
	{
	   g_uio_enable = FALSE;
	}
  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}
void ui_set_com_num(u8 num)
{
  com_num = num;
}
#if 1
static void _set_fp_display(char *content)
{
  void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  if(g_customer.customer_id == CUSTOMER_JIULIAN)
  {
    memcpy(g_led_buffer,content,3);
    g_led_buffer[3] = '.';
    memcpy((u8 *)(g_led_buffer + 4),(u8 *)(content + 3),com_num - 4);
    
    if(g_led_buffer[com_num] != 0)
    {
      memcpy(&(g_led_buffer[g_led_index + 1]),g_led_buffer + g_led_index,com_num - g_led_index);
      g_led_buffer[g_led_index] = '.';
      //mtos_printk("1~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num + 1);
      uio_display(p_dev, g_led_buffer, com_num + 1);
    }
    else
    {
      //mtos_printk("222222222%s buffer %s \n",__FUNCTION__,g_led_buffer);
      uio_display(p_dev, g_led_buffer, com_num+1);
    }
  }
  else
  {
    if(g_led_buffer[com_num] == 0)
    {
      memcpy(g_led_buffer,content,com_num);
      OS_PRINTF("0~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num);
      uio_display(p_dev, g_led_buffer, com_num);
    }
    else
    {
      memcpy(g_led_buffer,content,g_led_index);
      g_led_buffer[g_led_index] = '.';
      memcpy(&(g_led_buffer[g_led_index + 1]),content + g_led_index,com_num - g_led_index);
      OS_PRINTF("1~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num + 1);
      uio_display(p_dev, g_led_buffer, com_num + 1);
    }
  }
}
#endif

void ui_set_front_panel_by_str(const char * str)
{
  #ifndef WIN32
  char content[5];

  if (strlen(str) > 4)
  {
    memcpy(content, str, 4);
    content[4] = '\0';
  }
  else
  {
    if ((com_num == 4)||(com_num == 5))
    sprintf(content, "%4s", str);
   else 
    sprintf(content, "%3s ", str); 
   }

  _set_fp_display(content);
  #endif
}


void ui_set_front_panel_by_num(u16 num)
{
  #if 1
  char content[5];

   if(com_num == 5)
  {
    sprintf(content, "%.4d", num);
  }
  else if(com_num == 4)
  {
    sprintf(content, "%.4d", num);
  }
  else if(com_num == 3)
  {
    sprintf(content, "%.3d", num);
  }
  else if(com_num == 2)
  {
    sprintf(content, "%.2d", num);
  }
  else if(com_num == 1)
  {
    sprintf(content, "%.1d", num);
  }
  else
  {
    return;
  }

	
  _set_fp_display(content);
  #endif
}

//lint -e438 -e550 -e830
void ui_show_logo(u8 block_id)
{
  //dmx_chanid_t p_chan_handle = NULL;
  RET_CODE ret = SUCCESS;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);

  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
    //dmx_device_t *dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
     //SYS_DEV_TYPE_PTI);
  u32 size = get_dm_block_real_file_size(block_id);
  u8 *addr = mtos_malloc(size);
  MT_ASSERT(NULL != addr);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, size, addr);

  vdec_stop(p_video_dev);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
#ifndef WIN32  
  //pti_dev_set_video_pid(pti_dev, 0x1FFF);
  //dmx_play_chan_open(dmx_dev, DMX_VIDEO_TYPE, 0x1FFF, &p_chan_handle);
  //dmx_chan_start(dmx_dev, p_chan_handle);
#endif  

  //mtos_task_lock();
  vdec_set_data_input(p_video_dev,1);
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);
  OS_PRINTF("-----ui show logo addr 0x%x , size 0x%x\n",addr,size);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)addr, size);
  MT_ASSERT(SUCCESS == ret);
  //ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  //MT_ASSERT(SUCCESS == ret);
  //ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);  
  //MT_ASSERT(SUCCESS == ret);
  
 // mtos_task_unlock();

  mtos_free(addr);
}

void ui_show_logo_by_data(u8 *data, u32 size)
{
  RET_CODE ret;
  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);

  MT_ASSERT(data != NULL);
  MT_ASSERT(size > 0);

  vdec_stop(p_video_dev);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
  vdec_set_data_input(p_video_dev,1);
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);  
  MT_ASSERT(SUCCESS == ret);

#if 0
  u32 size1 = get_dm_block_real_file_size(LOGO_BLOCK_ID_M1);
  u8 *addr = mtos_malloc(size1);
  MT_ASSERT(NULL != addr);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), LOGO_BLOCK_ID_M1, 0, 0, size1, addr);
  ret = vdec_dec_one_frame(p_video_dev, addr, size1);
#endif
  
  ret = vdec_dec_one_frame(p_video_dev, data, size);
  MT_ASSERT(SUCCESS == ret);
  
  ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  MT_ASSERT(SUCCESS == ret); 

  
  //mtos_free(addr);
}

//lint +e438 +e550 +e830

//lint -e124 -e578 -e668 -e831
void* logo_region = NULL;
RET_CODE ui_show_logo_in_osd(u8 *data, u32 size, rect_t *rect, pdec_image_format_t format)
{
  RET_CODE ret = ERR_FAILURE;
  pic_info_t pic_info = {0};
  pic_param_t pic_param = {PIC_FRAME_MODE,};
  drv_dev_t *p_pic_dev = NULL;
  void *p_disp = NULL;
  void *p_gpe = NULL;
  u32 decode_data_size = 0;
  rect_size_t rect_size = {0};
  rect_t _rect = {0};
  rect_t fill_rect = {0};
  point_t pos = {0};
  gpe_param_vsb_t param = {0, 0, 0, 0, 0, 0};
  pdec_ins_t pic_ins = {IMAGE_FORMAT_UNKNOWN,};
  u8 *p_output = NULL;
  void *region_buffer = NULL;
  u32 align = 0;
  u32 region_size = 0;
  void *p_video_dev = NULL;

  MT_ASSERT(data != NULL);
  MT_ASSERT(size != 0);
  
  switch (format)
  {
    case IMAGE_FORMAT_JPEG:
    case IMAGE_FORMAT_GIF:
    case IMAGE_FORMAT_PNG:
    case IMAGE_FORMAT_BMP:
    break;

    default:
      return ret;
  }

  p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video_dev);
   
  p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  p_gpe = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe);

  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(p_pic_dev != NULL);
  
  vdec_stop(p_video_dev);// stop video decode.

  memset(&pic_ins, 0, sizeof(pic_ins));
  ret = pdec_getinfo(p_pic_dev, data, size, &pic_info, &pic_ins);
  MT_ASSERT(ret == SUCCESS);
  UI_PRINTF("pic width:%d, height:%d, format:%d, bbp:%d\n", 
              pic_info.src_width, pic_info.src_height, pic_info.image_format, pic_info.bbp);
  if(pic_info.image_format == IMAGE_FORMAT_JPEG)
  {
    decode_data_size = (pic_info.src_width + 16) * (pic_info.src_height + 16) * 4;
    pic_param.output_format = PIX_FMT_AYCBCR8888;
  }
  else
  {
    decode_data_size = pic_info.src_width * pic_info.src_height * 4; 
    pic_param.output_format = PIX_FMT_ARGB8888;
  }

  //UI_PRINTF("decode_data_size:%d\n", decode_data_size);
  p_output = (u8 *)mtos_align_malloc(decode_data_size, 8);
  MT_ASSERT(p_output != NULL);
  memset(p_output, 0, decode_data_size);

  pic_param.dec_mode = DEC_FRAME_MODE;
  pic_param.scale_w_num = 1;
  pic_param.scale_w_demo = 1;
  pic_param.scale_h_num = 1;
  pic_param.scale_h_demo = 1;
  pic_param.disp_width = pic_info.src_width;
  pic_param.disp_height = pic_info.src_height;      
  pic_param.p_src_buf = data;
  pic_param.p_dst_buf = p_output;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.src_size = size;

  //decode pic data
  UI_PRINTF("pdec_setinfo\n");
  ret = pdec_setinfo(p_pic_dev, &pic_param, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  UI_PRINTF("pdec_start\n");
  ret = pdec_start(p_pic_dev, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  UI_PRINTF("pdec_stop\n");
  ret = pdec_stop(p_pic_dev, &pic_ins);
  MT_ASSERT(ret == SUCCESS);

  //region size
  rect_size.w = (unsigned)(int)(RECTWP(rect));//(((rect->right - rect->left) + 8) * 8)/8;
  rect_size.h = (unsigned)(int)RECTHP(rect);//(rect->bottom - rect->top);
  pos.x = rect->left;
  pos.y = rect->top;

  UI_PRINTF("region w:%d, h:%d, x:%d, y:%d\n", rect_size.w, rect_size.h, pos.x, pos.y);

  //show ads rect
  _rect.top = (s16)((rect_size.h - pic_info.src_height)/2);
  _rect.bottom = (s16)((_rect.top + (s16)pic_info.src_height));
  _rect.left = (s16)((rect_size.w - pic_info.src_width)/2);
  _rect.right = (s16)(_rect.left + (s16)pic_info.src_width);

  if(_rect.top < 0)
  {
    _rect.top = 0;
    _rect.bottom = _rect.top + (s16)rect_size.h;
  }

  if(_rect.left < 0)
  {
    _rect.left = 0;
    _rect.right = _rect.left + (s16)rect_size.w;
  }
  UI_PRINTF("ads _rect top:%d, bottom:%d, left:%d, right:%d\n", _rect.top, _rect.bottom, _rect.left, _rect.right);

  fill_rect.right = RECTWP(&_rect);
  fill_rect.bottom = RECTHP(&_rect);
  if (logo_region != NULL)
  {
    ret = disp_layer_remove_region(p_disp, DISP_LAYER_ID_OSD0, logo_region);
    region_delete(logo_region);
    logo_region = NULL;
  }
  if(logo_region == NULL)
  {
    logo_region = region_create(&rect_size, (pix_fmt_t)pic_param.output_format);
    MT_ASSERT(NULL != logo_region);

    //ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_OSD0,
    //                            region, &pos, NULL);
    ret = disp_calc_region_size(p_disp, DISP_LAYER_ID_OSD0, logo_region, &align, &region_size);
    region_buffer = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_GDI);
    MT_ASSERT(region_buffer != 0);
    mem_mgr_release_block(BLOCK_AV_BUFFER);
    MT_ASSERT(mem_mgr_get_block_size(BLOCK_AV_BUFFER) >= (region_size * 2));
    memset(region_buffer, 0, region_size * 2);

    #ifdef WIN32
    ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_OSD0, logo_region, &pos , region_buffer);
    #else
    ret = disp_layer_add_region_ex(p_disp, DISP_LAYER_ID_OSD0, logo_region, &pos
    , (void *)(((u32)region_buffer) | 0xa0000000),(void *)(((u32)(region_buffer + region_size)) | 0xa0000000) );
    #endif
    gpe_draw_image_vsb(p_gpe, logo_region, &_rect, (void *)p_output, 
                      NULL, 0, 4 * pic_info.src_width, decode_data_size, 
                      (pix_fmt_t)(pic_param.output_format), &param, &fill_rect);
    disp_layer_update_region(p_disp, logo_region, NULL);
    region_show(logo_region, TRUE);
    ret = disp_layer_show(p_disp, DISP_LAYER_ID_OSD0, TRUE);
    MT_ASSERT(SUCCESS == ret);
  }
  mtos_align_free(p_output);
  return ret;
}
//lint +e124 +e578 +e668 +e831

RET_CODE ui_hide_logo_in_osd(void)
{
  RET_CODE ret = SUCCESS;
  void *p_disp = NULL;
  
  if (logo_region != NULL)
  {
    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);
    ret = disp_layer_remove_region(p_disp, DISP_LAYER_ID_OSD0, logo_region);
    region_delete(logo_region);
    logo_region = NULL;
  }

  return ret;
}


#ifdef ENABLE_ADS
u8* get_pic_buf_addr(u32* p_len)
{
  u32 pic_size = 0;
  u32 block_size = 0;
  u8* p_addr = NULL;
  ads_res_t ads_res = {0};
  
	 p_addr = (u8*)mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
	 mem_mgr_release_block(BLOCK_REC_BUFFER);
	 block_size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
	 
	 memset(p_addr, 0, block_size);

  ads_res.id = g_montage_ads_pic_id;
  ads_io_ctrl(ADS_ID_ADT_MT, ADS_IOCMD_AD_RES_GET, &ads_res);
  
   if(ads_res.p_head)
   {
      UI_PRINTF("File size:%d\n", ads_res.p_head->dwFileSize);
		  *p_len = ads_res.p_head->dwFileSize;
		  pic_size = ads_res.p_head->dwFileSize;
		  
		  pic_size = ROUNDUP(pic_size, 4);
		  p_addr += (block_size - pic_size);
		  
		  memcpy(p_addr, ads_res.p_data, *p_len);
   }
   else
   	{
		  *p_len =0;
		  pic_size = 0;
		  
		  pic_size = ROUNDUP(pic_size, 4);
		  p_addr += (block_size - pic_size);
		  
		  //memset(p_addr, 0, *p_len);
   	}

  return p_addr;
}

//use it when show pic in no audio and video
void ui_show_logo_bg(ads_type_t ads_type , rect_t *rect)
{
  mul_pic_param_t pic_param;
  
//  u32 addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
//  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), block_id);

  if(ads_type == PIC_TYPE_MAIN_MENU)
  {
    g_montage_ads_pic_id = 0xc1;
  }
  else if(ads_type == PIC_TYPE_SUB_MENU)
  {
    g_montage_ads_pic_id = 0xc2;
  }
  
  ui_pic_init(PIC_SOURCE_BUF);

  pic_param.anim = REND_ANIM_NONE;
  pic_param.file_size = 0;
  pic_param.is_file = FALSE;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = rect->left;
  pic_param.win_rect.top = rect->top;
  pic_param.win_rect.right = rect->right;
  pic_param.win_rect.bottom = rect->bottom;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.buf_get = (u32)get_pic_buf_addr; 

  pic_start(&pic_param);
}

void ui_close_logo_bg(void)
{
  OS_PRINTF("enter %s\n", __FUNCTION__);
  pic_stop();
  ui_pic_release();
  OS_PRINTF("exit %s\n", __FUNCTION__);
}
#endif

BOOL is_region_y_coordinate_covered(rect_t *region_rect1, rect_t *region_rect2)
{
 
  if((region_rect1->top <= region_rect2->top && region_rect1->bottom >=region_rect2->top) 
    || (region_rect1->top > region_rect2->top && region_rect1->top <=region_rect2->bottom))
  {
    return TRUE;
  }
  return FALSE;
}

//lint -e732
void ui_show_ads_pic(ads_type_t ads_type , rect_t *region_rect)
{
#ifdef ENABLE_ADS
  pic_info_t pic_info = {0};
  pic_param_t pic_param = {PIC_FRAME_MODE, };
  drv_dev_t *p_pic_dev = NULL;
  void *p_disp = NULL;
  void *p_gpe = NULL;
  u8 *p_data = NULL;
  u32 data_size = 0;
  u32 decode_data_size = 0;
  rect_size_t rect_size = {0};
  rect_t rect = {0};
  rect_t fill_rect = {0};
  point_t pos = {0};
  ads_res_t ads_res = {0};
  RET_CODE ret = SUCCESS;
  gpe_param_vsb_t param = {0, 0, 0, 0, 0, 0};
  
  //param.enable_colorkey = TRUE;
  //param.colorkey = 0x4d00feff; //colorkey alpha 0x4d
  
  
  p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  p_gpe = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe);
  
  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);

  MT_ASSERT(p_pic_dev != NULL);  

  if(ads_type == PIC_TYPE_MAIN_MENU)
  {
    ads_res.id = MAIN_MENU_RES_ID;
  }
  else if(ads_type == PIC_TYPE_SUB_MENU)
  {
    ads_res.id = CH_LIST_RES_ID;
  }

 //get montage ads data
 ads_io_ctrl(ADS_ID_ADT_MT, ADS_IOCMD_AD_RES_GET, &ads_res);
 if(ads_res.p_head)
 {
    UI_PRINTF("File size:%d\n", ads_res.p_head->dwFileSize);
    p_data = ads_res.p_data;
    data_size = ads_res.p_head->dwFileSize;
   
	  MT_ASSERT(p_data != NULL);
	  MT_ASSERT(data_size != 0);
    
	  memset(&pic_ins, 0, sizeof(pic_ins));
	  ret = pdec_getinfo(p_pic_dev, p_data, data_size, &pic_info, &pic_ins);
	  MT_ASSERT(ret == SUCCESS);
	  UI_PRINTF("pic width:%d, height:%d, format:%d, bbp:%d\n", 
	  	                pic_info.src_width, pic_info.src_height, pic_info.image_format, pic_info.bbp);
	  if(pic_info.image_format == IMAGE_FORMAT_JPEG)
	  {
        decode_data_size = (pic_info.src_width + 16) * (pic_info.src_height + 16) * 4;
	    pic_param.output_format = PIX_FMT_AYCBCR8888;
	  }
	  else
	  {
      decode_data_size = pic_info.src_width * pic_info.src_height * 4; 
	     pic_param.output_format = PIX_FMT_ARGB8888;
	  }

   UI_PRINTF("decode_data_size:%d\n", decode_data_size);
	  p_output = (u8 *)mtos_align_malloc(decode_data_size, 8);
	  MT_ASSERT(p_output != NULL);
    memset(p_output, 0, decode_data_size);
	  
	  pic_param.dec_mode = DEC_FRAME_MODE;
	  pic_param.scale_w_num = 1;
	  pic_param.scale_w_demo = 1;
	  pic_param.scale_h_num = 1;
	  pic_param.scale_h_demo = 1;
	  pic_param.disp_width = pic_info.src_width;
	  pic_param.disp_height = pic_info.src_height;      
	  pic_param.p_src_buf = p_data;
	  pic_param.p_dst_buf = p_output;
	  pic_param.flip = PIC_NO_F_NO_R;
	  pic_param.src_size = data_size;

	  //decode pic data
	  UI_PRINTF("pdec_setinfo\n");
	  ret = pdec_setinfo(p_pic_dev, &pic_param, &pic_ins);
	  MT_ASSERT(ret == SUCCESS);

	  UI_PRINTF("pdec_start\n");
	  ret = pdec_start(p_pic_dev, &pic_ins);
	  MT_ASSERT(ret == SUCCESS);

	  UI_PRINTF("pdec_stop\n");
	  ret = pdec_stop(p_pic_dev, &pic_ins);
	  MT_ASSERT(ret == SUCCESS);

	  //region size
	  rect_size.w = (((region_rect->right - region_rect->left) + 8) * 8)/8;
	  rect_size.h = (region_rect->bottom - region_rect->top);
    pos.x = region_rect->left;
    pos.y = region_rect->top;

    UI_PRINTF("region w:%d, h:%d, x:%d, y:%d\n", rect_size.w, rect_size.h, pos.x, pos.y);
    
	  //show ads rect
	  rect.top = (s16)((rect_size.h - pic_info.src_height)/2);
	  rect.bottom = rect.top + (s16)pic_info.src_height;
	  rect.left = ((s16)rect_size.w - (s16)pic_info.src_width)/2;
	  rect.right = rect.left + (s16)pic_info.src_width;

   if(rect.top < 0)
   	{
   	  rect.top = 0;
	   rect.bottom = rect.top + (s16)rect_size.h;
   	}

   if(rect.left < 0)
   	{
      rect.left = 0;
      rect.right = rect.left + (s16)rect_size.w;
   	}
   UI_PRINTF("ads rect top:%d, bottom:%d, left:%d, right:%d\n", rect.top, rect.bottom, rect.left, rect.right);
    
  fill_rect.right = RECTWP(&rect);
  fill_rect.bottom = RECTHP(&rect);
  if(p_montage_ads_sub_rgn == NULL)
  {
    p_montage_ads_sub_rgn = region_create(&rect_size,PIX_FMT_ARGB8888);
    MT_ASSERT(NULL != p_montage_ads_sub_rgn);

    ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL,
                                   p_montage_ads_sub_rgn, &pos, NULL);

    gpe_draw_image_vsb(p_gpe, p_montage_ads_sub_rgn, &rect, (void *)p_output, 
                                        NULL, 0, 4 * pic_info.src_width, decode_data_size, 
                                        PIX_FMT_ARGB8888, &param, &fill_rect);
    disp_layer_update_region(p_disp, p_montage_ads_sub_rgn, NULL);
    region_show(p_montage_ads_sub_rgn, TRUE);
    ret = disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
    MT_ASSERT(SUCCESS == ret);
  }
  
  mtos_align_free(p_output);
 	}
#endif
}
//lint +e732

//lint -e438 -e550 -e830
void ui_close_ads_pic(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                          SYS_DEV_TYPE_DISPLAY);

  if(NULL != p_montage_ads_sub_rgn)
  {
    ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, p_montage_ads_sub_rgn);
    MT_ASSERT(SUCCESS == ret);

    ret = region_delete(p_montage_ads_sub_rgn);
    MT_ASSERT(SUCCESS == ret);
    p_montage_ads_sub_rgn = NULL;

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, FALSE);
  }
}
//lint +e438 +e550 +e830

void ui_enable_video_display(BOOL is_enable)
{
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
  
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, is_enable);
}

static void _config_osd(rect_t *p_scr)
{
  u8 ss_value = 0;
	sys_status_t *p_systatus = NULL;
	rect_t flinger_rect =
	{
    0,
    0,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
	};
	flinger_cfg_t bot = 
	{
	  SURFACE_OSD1,
	  1,
	  &flinger_rect,
  };
	rect_t screen_rect = 
	{
    SCREEN_POS_PAL_L,
    SCREEN_POS_PAL_T,
    SCREEN_POS_PAL_L + SCREEN_WIDTH,
    SCREEN_POS_PAL_T + SCREEN_HEIGHT
  };
  screen_reset_t reset_info = 
  {
    &screen_rect,
    NULL,
    &bot,
    COLORFORMAT_ARGB8888,         //osd color format.
    NULL,                         //palette.
    C_TRANS,                      //transparent color.
    C_KEY,                        //color key.
  };
  gdi_reset_screen(&reset_info);

  // get sys status
  p_systatus = sys_status_get();

  ss_value = p_systatus->osd_set.transparent;
  UI_PRINTF("set global alpha = %d\n", (100 - ss_value) * 255 / 100);
  gdi_set_global_alpha((u8)((100 - ss_value) * 255 / 100));
}

void ui_config_ttx_osd(void)
{
  disp_sys_t video_std = VID_SYS_NTSC_J;
  rsc_palette_t hdr_pal = {{0}};
  u8 *p_data = NULL;
  palette_t pal = {0};
  void *p_disp = NULL;
	rect_t flinger_rect =
	{
    0,
    0,
    1280,
    720,
	};
	flinger_cfg_t bot = 
	{
	  SURFACE_OSD1,
	  1,
	  &flinger_rect,
  };
	rect_t screen_rect = 
	{
    0,
    0,
    1280,
    720
  };
  screen_reset_t reset_info = 
  {
    &screen_rect,
    NULL,
    &bot,
    COLORFORMAT_RGB8BIT,                //osd color format.
    &pal,                               //palette.
    C_TRANS_8BIT,                       //transparent color.
    C_KEY_8BIT,                         //color key.
  };
  
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      break;
    default:
      break;
  }  

  //offset_rect(&rc_scr, x, y);

  rsc_get_palette(gui_get_rsc_handle(), 1, &hdr_pal, (u8**)&p_data);
  pal.cnt = hdr_pal.color_num;
  pal.p_entry = (color_t *)p_data;
  pal.p_entry[C_TRANS_8BIT].a = 0;

  gdi_reset_screen(&reset_info);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);
  disp_layer_color_key_onoff(p_disp, DISP_LAYER_ID_OSD1, FALSE);
}

void ui_config_9pic_osd(void)
{
  rect_t rc_scr = 
    { 0, 0,
      SCREEN_9PIC_WIDTH,
      SCREEN_9PIC_HEIGHT };

  s16 x, y;
  disp_sys_t video_std = VID_SYS_NTSC_J;
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    //Temporal solution. 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      x = SCREEN_9PIC_POS_NTSC_L;
      y = SCREEN_9PIC_POS_NTSC_T;
      break;
    default:
      x = SCREEN_9PIC_POS_PAL_L;
      y = SCREEN_9PIC_POS_PAL_T;
  }

  offset_rect(&rc_scr, x, y);
  _config_osd(&rc_scr);
}

void ui_config_normal_osd(void)
{
  rect_t rc_scr = 
    { 0, 0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT };

  s16 x, y;
  disp_sys_t video_std = VID_SYS_NTSC_J;
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      x = SCREEN_POS_NTSC_L;
      y = SCREEN_POS_NTSC_T;
      break;
    default:
      x = SCREEN_POS_PAL_L;
      y = SCREEN_POS_PAL_T;
  }

  offset_rect(&rc_scr, x, y);
  _config_osd(&rc_scr);
}

/*
the input size is Kbytes
*/
void ui_conver_file_size_unit(u32 k_size, u8 *p_str)
{
  u32 m_unit = 1024*1024;
  u32 mod_value = 0;
  if(k_size == 0)
  {
    sprintf((char *)p_str,"0M Bytes");
  }
  else if(k_size/1024/1024 >0)
  {
    mod_value = k_size%m_unit;
    sprintf((char *)p_str,"%ld.%ld%ldG Bytes", k_size/m_unit,mod_value*10/m_unit,(mod_value*10%m_unit)*10/m_unit);
  }
  else if(k_size/1024 >0)
  {
    mod_value = k_size%1024;
    sprintf((char *)p_str,"%ld.%ld%ldM Bytes", k_size/1024,mod_value*10/1024,(mod_value*10%1024)*10/1024);
  }
  else
  {
    sprintf((char *)p_str,"%ldK Bytes", k_size);
  }
}

void ui_set_front_panel_by_str_with_upg(const char *str_flag, u8 process)
{
  char str[5] = {0};
  static u8 i,j;
  s32 cmp = 0;
  void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  if (com_num == 0)
    return;

  memset(str,' ',5);
  if(process == 100)
  {
    i = 0;
  }
  if(i % com_num == 0)
  {
    memset(str,(int)'-',1);
    i++;
  }
  else if(i % com_num == 1)
  {
    memset(str+1,(int)'_',1);
    i++;
  }
  else if(i % com_num == 2)
  {
    i++;
    memset(str + 2,(int)'-',1);
  }
  else if(i % com_num == 3)
  {
    memset(str + 3,(int)'_',1);
    i++;
  }
  else if(i % com_num == 4)
  {
    memset(str + 4,(int)'_',1);
    i++;
  }
  else if(i % com_num == 5)
  {
    memset(str + 5,(int)'_',1);
    i++;
  }

  if(memcmp(str_flag,"W",1) == 0)
  {
    j = 1;
  }

 if(process == 100)
 {
     if((memcmp(str_flag,"B",1) == 0))
    {
       memcpy(str,"OFF ",4);
     }
     if(memcmp(str_flag,"R",1) == 0)
     {
        if(j == 1)
        {
          memcpy(str,"END ",3);
          j = 0;
        }
     }
  }
 
cmp = memcmp(str_flag,"ERR",3);

if(cmp == 0)
{
  memcpy(str,"E01 ",4); 
}
  
{
  uio_display(p_dev, (u8 *)str, com_num);
}
}

static s32 BmpReader(const u8* bmpaddr, const u32 size, BMP_HEADER* p_bmp_header, DIB_HEADER* p_dib_header)
{
  const u8* p = bmpaddr;

  if(*p != 'B' || *(p+1) != 'M')
    return -1;

  p += 2; //skip magic
  memcpy(p_bmp_header, p, sizeof(BMP_HEADER));

  if(p_bmp_header->filesz != size)
    return -1;

  p += sizeof(BMP_HEADER);
  memcpy(p_dib_header, p, sizeof(DIB_HEADER));
    
  return 0;
}

void bmp_up2down(u8 *p_data, u32 image_width, u32 image_height, u16 bpp)
{
    u32 index = bpp/8;
    u32 h, w, coordm, coordn;
    u8 temp;
    for (h = 0; h < image_height/2; h++)
    {
        for (w = 0; w < image_width; w++)
        {
            coordm = index*(h*image_width + w);
            coordn = index*((image_height - h -1)*image_width + w);
            temp = p_data[coordm];
            p_data[coordm] = p_data[coordn];
            p_data[coordn] = temp;
            temp = p_data[coordm+1];
            p_data[coordm + 1] = p_data[coordn + 1]; 
            p_data[coordn + 1] = temp;
            temp = p_data[coordm + 2];
            p_data[coordm + 2] = p_data[coordn + 2];
            p_data[coordn + 2] = temp;
        }
    }
}

BOOL ui_get_watermark_rect(rect_t * rect_region)
{
  BOOL ret = FALSE;

  if(NULL != p_sub_rgn)
  {
    rect_region->top = g_water_mark_rect.top;
    rect_region->bottom = g_water_mark_rect.bottom;
    rect_region->left = g_water_mark_rect.left;
    rect_region->right = g_water_mark_rect.right;
    
    ret = TRUE;
  }
  return ret;
}
//lint -e438 -e550 -e830 -e668 -e831
void ui_show_watermark(void)
{
  pic_info_t pic_info = {PIC_FRAME_MODE,};
  pic_param_t pic_param = {PIC_FRAME_MODE,};
  drv_dev_t *p_pic_dev = NULL;
  void *p_disp = NULL;
  void *p_gpe = NULL;
  u8 *addr = NULL;
  u32 size = 0;
  u32 decode_data_size = 0;
  rect_size_t rect_size = {0};
  rect_t rect = {0};
  rect_t fill_rect = {0};
  point_t pos = {0};
  RET_CODE ret = SUCCESS;
  gpe_param_vsb_t param = {0, 0, 0, 0, 0, 0};
  osd_set_t osd_set;
  pdec_ins_t pic_ins_warter = {IMAGE_FORMAT_UNKNOWN,};
  u8 *p_output_water = NULL;
  BMP_HEADER bmp_header;
  DIB_HEADER dib_header;
  u32 i = 0;
  u32 width=0,height=0;
  u32 tmpwidth = 0;
  
  size = get_dm_block_real_file_size(LOGO_BLOCK_ID_M2);
  addr = mtos_malloc(size);
  //for pc_lint
  if(NULL == addr)
  {
    MT_ASSERT(0);
    return;
  }
  dm_read(class_get_handle_by_id(DM_CLASS_ID), LOGO_BLOCK_ID_M2, 0, 0, size, addr);
  MT_ASSERT(size != 0);
  
  p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);

  p_gpe = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_gpe);
  
  p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(p_pic_dev != NULL);  
  
  if(!g_customer.b_WaterMark || p_sub_rgn != NULL)
  {
    #if 0
    if(!disp_layer_is_show(p_disp, DISP_LAYER_ID_SUBTITL))
    {
      ret = disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
      MT_ASSERT(SUCCESS == ret);
    }
	#endif
    mtos_free(addr);
    return;
  }
  
  param.enable_colorkey = TRUE;
  param.colorkey = 0x4d00feff; //colorkey alpha 0x4d

  sys_status_get_osd_set(&osd_set);
  if (osd_set.enable_subtitle)
  {
    OS_PRINTF("Water mark cannot display when subtitle on\n");
    mtos_free(addr);
    return;
  }
 
  memset(&pic_ins_warter, 0, sizeof(pic_ins_warter));
  ret = pdec_getinfo(p_pic_dev, addr, size, &pic_info, &pic_ins_warter);
  MT_ASSERT(ret == SUCCESS);

  OS_PRINTF("pic width:%d, height:%d, format:%d, bbp:%d\n", pic_info.src_width, pic_info.src_height, pic_info.image_format, pic_info.bbp);
  if(pic_info.image_format != IMAGE_FORMAT_BMP)
  {
      if(pic_info.image_format == IMAGE_FORMAT_JPEG)
      {
        decode_data_size = (pic_info.src_width + 16) * (pic_info.src_height + 16) * 4;
        pic_param.output_format = PIX_FMT_AYCBCR8888;
      }
      else 
      {
        decode_data_size = (pic_info.src_width/2 + 1) * 2 * pic_info.src_height * 4; 
        pic_param.output_format = PIX_FMT_ARGB8888;
      }
      
      OS_PRINTF("decode_data_size:%d\n", decode_data_size);
      p_output_water = (u8 *)mtos_align_malloc(decode_data_size, 8);
      MT_ASSERT(p_output_water != NULL);
      memset(p_output_water, 0, decode_data_size);
      
      pic_param.dec_mode = DEC_FRAME_MODE;
      pic_param.scale_w_num = 1;
      pic_param.scale_w_demo = 1;
      pic_param.scale_h_num = 1;
      pic_param.scale_h_demo = 1;
      pic_param.disp_width = pic_info.src_width;
      pic_param.disp_height = pic_info.src_height;      
      pic_param.p_src_buf = addr;
      pic_param.p_dst_buf = p_output_water;
      pic_param.flip = PIC_NO_F_NO_R;
      pic_param.src_size = size;

      UI_PRINTF("pdec_setinfo\n");
      ret = pdec_setinfo(p_pic_dev, &pic_param, &pic_ins_warter);
      MT_ASSERT(ret == SUCCESS);

      UI_PRINTF("pdec_start\n");
      ret = pdec_start(p_pic_dev, &pic_ins_warter);
      MT_ASSERT(ret == SUCCESS);

      UI_PRINTF("pdec_stop\n");
      ret = pdec_stop(p_pic_dev, &pic_ins_warter);
      MT_ASSERT(ret == SUCCESS);
      width = pic_info.src_width;
      height = pic_info.src_height;
      tmpwidth = 4 * (( pic_info.src_width + 1 ) / 2 * 2);
  }
  else
  {
        ret = BmpReader((unsigned char*)addr, size, &bmp_header, &dib_header);

        MT_ASSERT(0 == ret);
        width = (u32)dib_header.width;
        height = (u32)dib_header.height;
        decode_data_size = (u32)(dib_header.bitspp / 8 * dib_header.width * dib_header.height);
        p_output_water = (u8 *)mtos_align_malloc(decode_data_size, 8);
        MT_ASSERT(NULL != p_output_water);
        memset(p_output_water, 0 , decode_data_size);
        memcpy(p_output_water, addr + bmp_header.bmp_offset, decode_data_size);
        //32bit bmp set alpha to 0x4d

        for(i = 0; i < decode_data_size; i++)
        {
          if(i % 4 == 3)
          {
            p_output_water[i] = 0x4D;  //alpha 0x4d
          }
        }

        //bmp reverse
        bmp_up2down(p_output_water, (u32)dib_header.width, (u32)dib_header.height, (u32)dib_header.bitspp);
        tmpwidth = (u32)(dib_header.bitspp / 8 * dib_header.width);

  }
  
  rect_size.w = (width + 3 ) / 4 * 4;
  rect_size.h = height;
  pos.x = (s16)g_customer.x_WaterMark;
  pos.y = (s16)g_customer.y_WaterMark;

  if(pos.x + (s16)rect_size.w > 1280)
  {
    pos.x = 1280 - (s16)rect_size.w;
  }

  if(pos.y + (s16)rect_size.h > 720)
  {
    pos.y = 720 - (s16)rect_size.h;
  }
  
  g_water_mark_rect.left = pos.x;
  g_water_mark_rect.right = pos.x + (s16)rect_size.w;
  g_water_mark_rect.top = pos.y;
  g_water_mark_rect.bottom = pos.y + (s16)rect_size.h;
  
  #ifdef ENABLE_ADS
  rect_t adv_rect = {0};
  if(ui_adv_get_pic_rect(&adv_rect))
  {
    if(is_region_y_coordinate_covered(&g_water_mark_rect, &adv_rect))
    {
      OS_PRINTF("[ui_show_watermark] region covered, won't show watermark! \n");
      
      mtos_align_free(p_output_water);
      mtos_free(addr);
      return;
    }
  }
  #endif
  //show rect
  rect.top = 0;
  rect.bottom = rect.top + (s16)height;
  rect.left = 0;
  rect.right = rect.left + (s16)width;

  fill_rect.right = RECTWP(&rect);
  fill_rect.bottom = RECTHP(&rect);  

  if(p_sub_rgn == NULL)
  {
    OS_PRINTF("region w:%d, h:%d, x:%d, y:%d\n", rect_size.w, rect_size.h, pos.x, pos.y);

    p_sub_rgn = region_create(&rect_size,PIX_FMT_ARGB8888);
    MT_ASSERT(NULL != p_sub_rgn);
    ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL,
    p_sub_rgn, &pos, NULL);
   
    //pitch must be 8 bytes align
    gpe_draw_image_vsb(p_gpe, p_sub_rgn, &rect, (void *)p_output_water, 
                      NULL, 0, tmpwidth, decode_data_size, 
                      PIX_FMT_ARGB8888, &param, &fill_rect);
  
    disp_layer_update_region(p_disp, p_sub_rgn, NULL);
    region_show(p_sub_rgn, TRUE);
    ret = disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
    MT_ASSERT(SUCCESS == ret);
  }
      
  mtos_align_free(p_output_water);
  mtos_free(addr);
}

void ui_watermark_release(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                                          SYS_DEV_TYPE_DISPLAY);

  if(!g_customer.b_WaterMark || NULL == p_sub_rgn)
  {
    return;
  }

  ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, p_sub_rgn);
  MT_ASSERT(SUCCESS == ret);

  ret = region_delete(p_sub_rgn);
  MT_ASSERT(SUCCESS == ret);
  p_sub_rgn = NULL;

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, FALSE);
  
  memset(&g_water_mark_rect, 0, sizeof(rect_t));
}
void ui_water_mark_refresh(void)
{
#if 0
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  if(pic_watermark_region != NULL)
  {
    disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, FALSE);
    disp_layer_update_region(p_disp_dev, pic_watermark_region, NULL);
    disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, TRUE);
  }
  #endif
  //guoteng: need to be improved
  if(p_sub_rgn != NULL)
  {
  ui_watermark_release();
  ui_show_watermark();
  }
}
//lint +e438 +e550 +e830 +e668 +e831 

#ifdef ENABLE_NETWORK
void ui_set_net_svc_instance(void *p_instance)
{
  p_net_svc = p_instance;
}

void * ui_get_net_svc_instance(void)
{
  return p_net_svc;
}
#endif
