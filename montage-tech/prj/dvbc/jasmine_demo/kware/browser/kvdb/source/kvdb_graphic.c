/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
 #include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_mutex.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "drv_dev.h"


#include "common.h"
#include "mem_manager.h"

#include "lib_rect.h"
#include "gpe_vsb.h"

#include "display.h"
#include "region.h"
#include "vdec.h"
#include "mem_cfg.h"


#include "kvdb_graphic.h"

#define KVDB_GRAPHIC_DEBUG_PRINTF
#ifdef   KVDB_GRAPHIC_DEBUG_PRINTF
#define KVDB_GRAPHIC_DEBUG OS_PRINTF
#else
#define KVDB_GRAPHIC_DEBUG DUMMY_PRINTF
#endif
static u8 *buf_logo = NULL;
static void * region = NULL;
static void *p_disp = NULL;  
static void *p_gpe = NULL;  
#define REGION_WIGHT 1280
#define REGION_HEIGHT 720
#define SCREEN_W 640
#define SCREEN_H 520
//static void *p_rgn_buf  = NULL;
//static void * p_log_buf = NULL;
RET_CODE display_init(void)
{
  RET_CODE ret;
    rect_size_t rect_size;
    point_t pos;
    
    pos.x = 0;
    pos.y = 0;
    rect_size.h = REGION_HEIGHT;
    rect_size.w = REGION_WIGHT;

       p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
       MT_ASSERT(NULL != p_disp);
       p_gpe = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
       MT_ASSERT(NULL != p_gpe);
        region = region_create(&rect_size, PIX_FMT_ARGB1555);
       ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL,  region, &pos, NULL);

        ret = region_show(region, TRUE);
        KVDB_GRAPHIC_DEBUG("[%s][%d]ret:%d\n",__FUNCTION__, __LINE__, ret);
        ret = disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
        KVDB_GRAPHIC_DEBUG("[%s][%d]ret:%d\n",__FUNCTION__, __LINE__, ret);
        buf_logo= mtos_align_malloc(640*520*2, 8);

        MT_ASSERT(buf_logo != NULL);
        
    return 0;
}


void  kvdb_osddrv_deinit(void)
{
  KVDB_GRAPHIC_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
   RET_CODE ret = SUCCESS;
   if(region)
    {
         ret = disp_layer_remove_region(p_disp, 
                                           DISP_LAYER_ID_SUBTITL,
                                           region);
            MT_ASSERT(SUCCESS == ret);
          ret = region_delete(region);//destroy the browser region
              MT_ASSERT(SUCCESS == ret);
              region = NULL;
              if(buf_logo)
                {
                     mtos_align_free(buf_logo);
                }
    }
}

void  kvdb_osddrv_init(void )
{
    display_init();
    return;
}
int kvdb_porting_graphics_getInfo(int *width, int *height, void **pbuffer, int *bufWidth,int *bufHeight)
{   
    	u32 dwWidth = 0;
	u32 dwHeight = 0;
  	dwWidth = 640;
  	dwHeight = 520;
    *bufWidth = 640;//IPANEL_GRAPHICS_WIDTH;
	*bufHeight = 520;//IPANEL_GRAPHICS_HEIGHT;
	*width = 640;//IPANEL_GRAPHICS_WIDTH;
	*height = 520;//IPANEL_GRAPHICS_HEIGHT;  
	
   (*pbuffer) = NULL;

	return 0;
}

/******************************************************************/
/*Description: Draw a rectangle image.                            */
/*Input      : start point(x,y),width(w),height(h);               */
/*             color data(bits), and data width(w_src) per line;  */
/*Output     : No                                                 */
/*Return     : 0 -- success, Other fail.                          */
/******************************************************************/
int kvdb_porting_draw_image(int x, int y, int w, int h, unsigned char *bits, int w_src)
{  
  
  RET_CODE ret;
  KVDB_GRAPHIC_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
  mtos_task_lock();
    image_t img;
    float scale_w = 0;
    float scale_h = 0;
  
//    gpe_param_vsb_t gpe_param;
    rect_t src_rect;
    rect_t o_rect;
    u32 new_h, new_w, line, cow =  0;
    u16 *dst = NULL;
    u16 *src = NULL;
  //  u8  *buf= NULL;
    memset(&img, 0x0, sizeof(image_t));
  if(( x < 0 ) ||(y < 0)||(w < 0) ||(h < 0)|| (bits == NULL))
      {
          return  -1;
      }    
    new_w = w;  
	new_h = h; 
    if(x + w > SCREEN_W)
     {
        new_w = SCREEN_W - x;
     }
    if(y +h > SCREEN_H)
      {
          new_h = SCREEN_H - y;
      }  
  // buf = mtos_align_malloc(new_w*new_h*2,4);
  // MT_ASSERT(buf != NULL);
  //  memset(buf, 0x0, new_w*new_h*2);
   scale_w = (float)REGION_WIGHT/ (float)SCREEN_W;
  scale_h= (float)REGION_HEIGHT/ (float)SCREEN_H;
  memset(buf_logo, 0x0, 640*520*2);
  src = (u16 *)bits;
  dst = (u16*)buf_logo;
   new_w = new_w +( 4 - (new_w%4))%4;
  for(line = 0; line < new_h;++ line)
    {       
        for(cow = 0; cow < new_w; cow ++)
          dst[line*(new_w +( 4 - (new_w%4))%4)  + cow] = (src[line*new_w +cow]|0x8000);
      
     }      
    img.pitch = new_w*2;
    img.width = new_w;
    img.height = new_h;
    img.pix_fmt =PIX_FMT_ARGB1555;
    img.alpha = 0;
    img.b_alpha = 0;
    img.p_buf_addr = buf_logo;
    img.buf_sz = new_w*new_h*2;
    img.little_endian = TRUE;
    src_rect.top = 0 ;
    src_rect.left = 0;
    src_rect.bottom = new_h;
    src_rect.right =  new_w;
    o_rect.top = (scale_h*(float)y);
    o_rect.left = (scale_w*(float)x);
    o_rect.bottom =o_rect.top + scale_h*(float)new_h;
    o_rect.right = o_rect.left  + scale_w*(float)new_w;
     
    ret = gpe_draw_image_scale(p_gpe, &img, &src_rect, region, &o_rect);

    ret = disp_layer_update_region(p_disp, region, NULL);
 //  mtos_align_free(buf);

   mtos_task_unlock();
    return SUCCESS;
}

