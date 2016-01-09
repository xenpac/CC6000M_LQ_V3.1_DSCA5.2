/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "pic_api.h"
#include "google_map_api.h"



static handle_t g_map_handle = 0;
static handle_t g_map_region = NULL;

//#ifndef WIN32
static void ui_map_region_init(u8 *p_buffer)
{
  rect_size_t rect_size = {0};
  point_t pos;  
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                          SYS_DEV_TYPE_DISPLAY);
  pos.x = 0;
  pos.y = 0;
  rect_size.w = SCREEN_WIDTH;
  rect_size.h = SCREEN_HEIGHT;
  g_map_region = region_create(&rect_size, PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != g_map_region);
  memset(p_buffer, 0, PICTURE_REGION_USED_SIZE);
#ifdef CONCERTO
  ret = disp_layer_add_region_ex(p_disp_dev, DISP_LAYER_ID_OSD0, g_map_region, &pos, 
    (void *)(((u32)p_buffer) | 0xa0000000),(void *)(((u32)(p_buffer +(PICTURE_REGION_USED_SIZE/2))) | 0xa0000000) );
#else
  ret = disp_layer_add_region(p_disp_dev, DISP_LAYER_ID_OSD0, g_map_region, &pos, 
    (void *)p_buffer);
#endif
  MT_ASSERT(SUCCESS == ret);

  region_show(g_map_region, TRUE);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, TRUE);
  
  if(SUCCESS == ret)
  {
  
  }
}

static void ui_map_region_release(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                          SYS_DEV_TYPE_DISPLAY);

  if(NULL != g_map_region)
  {
    ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_OSD0, g_map_region);
    MT_ASSERT(SUCCESS == ret);

    ret = region_delete(g_map_region);
    MT_ASSERT(SUCCESS == ret);
    g_map_region = NULL;

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, FALSE);
  }

  if(SUCCESS == ret)
  {
  
  }
}
//#endif
/*
static void map_api_callback(handle_t map_handle, u32 content, u32 para1, u32 para2)
{

  fw_notify_root(fw_find_root_by_id(ROOT_ID_GOOGLE_MAP), NOTIFY_T_MSG, FALSE,
		             content, para1, para2);  
		             
}
*/
RET_CODE ui_google_map_init(void)
{
   RET_CODE ret = ERR_FAILURE;
   mul_google_map_chan_t map_chain = {0};
   u8 *p_buffer = NULL;
   u8 *p_region_buffer = NULL;
   u32 buffer_size = 0; 
	
	//create chain
   map_chain.task_stk_size = 16 * KBYTES;
   map_chain.anim_pri = JPEG_CHAIN_ANIM_TASK_PRIORITY;
   map_chain.anim_stk_size = 4 * KBYTES;

   ret = mul_google_map_create_chain(&g_map_handle, &map_chain);
   MT_ASSERT(ret == SUCCESS);

   p_buffer = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_GDI);
   MT_ASSERT(p_buffer != 0);

   p_buffer += MUSIC_MODULE_USED_SIZE;
   memset(p_buffer, 0, MUSIC_MODULE_USED_SIZE);
   buffer_size = PICTURE_MODULE_USED_SIZE;

   mem_mgr_release_block(BLOCK_AV_BUFFER);
  
   p_region_buffer = p_buffer + PICTURE_MODULE_USED_SIZE;
//#ifndef WIN32
   ui_map_region_init(p_region_buffer);
//#else
//   g_map_region = (void *)gdi_get_screen_handle(FALSE);
//#endif

   mul_google_map_set_rend_rgn(g_map_handle, g_map_region); 

   mul_google_map_attach_buffer(g_map_handle, p_buffer, buffer_size); 

   //mul_google_map_register_evt(g_map_handle, map_api_callback);
   
   if(SUCCESS == ret)
   {
  
   }
	
   return SUCCESS;
  
}

RET_CODE ui_google_map_release(void)
{
  if(g_map_handle != 0)
  {
    mul_google_map_detach_buffer(g_map_handle);
    mul_google_map_destroy_chain(g_map_handle);

    g_map_handle = 0;
  }
  
//#ifndef WIN32
  ui_map_region_release();
//#endif

   return SUCCESS;
}

RET_CODE ui_google_map_start(mul_google_map_param_t *p_map_param)
{

   MT_ASSERT(p_map_param != NULL);
   
   if(g_map_handle != 0)
   {
     OS_PRINTF("ui : mul_google_map_start !!!\n");
     mul_google_map_start(g_map_handle, p_map_param);
   }
   
   return SUCCESS;
}

RET_CODE ui_google_map_stop(void)
{
   if(g_map_handle != 0)
   {
      mul_google_map_stop(g_map_handle);
   }
   
   return SUCCESS;
}

RET_CODE ui_google_map_move(map_move_style_t move_style)
{
   if(g_map_handle != 0)
   {
      mul_google_map_move(g_map_handle, move_style);
   }
   
   return SUCCESS;
}

RET_CODE ui_google_map_zoom(map_zoom_style_t zoom_style)
{
   if(g_map_handle != 0)
   {
      mul_google_map_zoom(g_map_handle, zoom_style);
   }
   
   return SUCCESS;
}

