/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "mtos_mutex.h"
#include "ui_common.h"
#include "ui_config.h"
#include "pic_api.h"
#include "ui_usb_music.h"
#include "ui_picture_api.h"
#include "ui_pic_play_mode_set.h"
#include "ads_ware.h"
#ifdef ENABLE_ADS
#include "ui_ads_display.h"
#endif
#include "ui_ad_api.h"
static mul_pic_param_t g_pic_param = {{0},};
static handle_t pic_adv_handle = 0;
static handle_t pic_adv_region = 0;

static u8* g_adv_pic_addr = NULL;
static u32 g_pic_size = 0;
static rect_t g_rect = {0};
static void *ui_adv_chain_mutext = NULL;
static u32 g_adm_id;
extern u32 g_sub_buffer_size;

extern u16 ui_pic_evtmap(u32 event);

extern void pic_api_callback(handle_t pic_handle, u32 content, u32 para1, u32 para2);

void ui_adv_set_adm_id(u32 adm_id)
{
    g_adm_id = adm_id;
}

u32 ui_adv_get_adm_id(void)
{
    return g_adm_id;
}

//creat for iris ad 

/*********************START************************************
***************************************************************/
//lint -e438 -e550
#ifndef WIN32
/*ADV Region  create */
static RET_CODE ui_adv_pic_region_init(const rect_t *rect)
{
  rect_size_t rect_size = {0};
  point_t pos;  
  RET_CODE ret = ERR_FAILURE;
  void *p_region_buffer = NULL;
  u32 region_block_size = 0;
  u32 align = 0;
  u32 region_size = 0;
  void *p_disp_dev;


  p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  //set coordinate
  pos.x = rect->left;
  pos.y = rect->top;
  rect_size.w = ((u16)((rect->right - rect->left) + 7) >> 3) << 3; //must be multiple of 8
  rect_size.h =  (u16)(rect->bottom - rect->top);
#if 0
  if(pos.x+rect_size.w > SCREEN_WIDTH)
  {
      pos.x = SCREEN_WIDTH-rect_size.w;
  }
  if(pos.y+rect_size.h > SCREEN_HEIGHT)
  {
      pos.y = SCREEN_HEIGHT-rect_size.h;
  }
#else
//layer的输出统一大小为1280*760，不变化
  if(pos.x+rect_size.w > 1280)
  {
      pos.x = 1280-rect_size.w;
  }
  if(pos.y+rect_size.h > 720)
  {
      pos.y = 720-rect_size.h;
  }
#endif
  OS_PRINTF("@@@region x=%d, y = %d, w = %d, h = %d \n",pos.x,pos.y,rect_size.w,rect_size.h);

  region_block_size = g_sub_buffer_size;
  MT_ASSERT(region_block_size != 0);

  pic_adv_region = region_create(&rect_size, PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != pic_adv_region);
  ret = disp_calc_region_size(p_disp_dev, DISP_LAYER_ID_SUBTITL, pic_adv_region, &align, &region_size);
  OS_PRINTF("region_size=0x%x,region_block_size=0x%x\n",region_size,region_block_size);
  if (ret != SUCCESS)
  {
    OS_PRINTF("disp_calc_region_size Failure!\n");
    return ERR_FAILURE;
  }
  MT_ASSERT(region_block_size >= region_size);
  //memset(p_region_buffer, 0, region_size);
  ret = disp_layer_add_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, 
                                                  pic_adv_region, &pos, p_region_buffer);
  if(ret != SUCCESS)
  {
    OS_PRINTF("ui_adv_pic_region_init  add region fail, LINE : %d!!!\n",__LINE__);
    return ERR_FAILURE;
  }
  //ret = disp_layer_update_region(p_disp_dev, pic_adv_region, NULL);
  
  region_show(pic_adv_region, TRUE);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, TRUE);
  return SUCCESS;
}

//adv region release
static void ui_adv_pic_region_release(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                          SYS_DEV_TYPE_DISPLAY);

  if(NULL != pic_adv_region)
  {
    ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, pic_adv_region);
    MT_ASSERT(SUCCESS == ret);

    ret = region_delete(pic_adv_region);
    MT_ASSERT(SUCCESS == ret);
    pic_adv_region = NULL;
  }
}
#endif
//lint +e438 +e550

#if 0
static void ui_adv_pic_clear(u32 color)
{
 
  OS_PRINTF("pic clear, 0x%x\n", color);

  if(pic_adv_handle != 0)
  {
    mul_pic_clear(pic_adv_handle, color);
  }
}
#endif
void pic_adv_start(mul_pic_param_t *p_pic_param)
{
  MT_ASSERT(p_pic_param != NULL);
  
  OS_PRINTF("pic start decoder, 0x%x\n", pic_adv_handle);

  if(pic_adv_handle != 0)
  {
    mul_pic_start(pic_adv_handle, p_pic_param);
  }
}

void pic_adv_stop(void)
{
  OS_PRINTF("pic stop decoder, 0x%x\n", pic_adv_handle);
  #ifdef MULTI_PIC_ADV
  ui_adv_stop_mul_pic();
  #endif
  if(pic_adv_handle != 0)
  {
    mul_pic_stop(pic_adv_handle);
      
  //release region of current adv
  #ifndef WIN32
    ui_adv_pic_region_release();
  #endif
  }

  //ui_adv_pic_clear(0x00000000);
}

void ui_adv_pic_init(pic_source_t src)
{
  mul_pic_chan_t chan = {0};
  RET_CODE ret = ERR_FAILURE;
  //u8 *p_buffer = NULL;
  //u32 dec_buffer_size = 0;
  
  chan.task_stk_size = 16 * KBYTES;
  #ifndef WIN32
  chan.anim_pri = JPEG_CHAIN_ANIM_TASK_PRIORITY;
  #endif
  chan.anim_stk_size = 4 * KBYTES;
  
  if(pic_adv_handle != NULL)
  {
    return;
  }
  
  if (src == PIC_SOURCE_NET)
  {
    ret = mul_pic_create_net_chain(&pic_adv_handle, &chan);
  }
  else if( src  == PIC_SOURCE_FILE )
  {
    ret = mul_pic_create_chain(&pic_adv_handle, &chan);
  }
  else if( src == PIC_SOURCE_BUF )
  {
    ret = mul_pic_create_buffer_chain(&pic_adv_handle, &chan);
  }

  if(SUCCESS != ret)
  {
    pic_adv_handle = 0;

    return;
  }
  
  OS_PRINTF("pic chain created, 0x%x\n", pic_adv_handle);  

  memset(&g_pic_param, 0, sizeof(mul_pic_param_t));

  mul_pic_set_dec_mode(pic_adv_handle, DEC_FRAME_MODE); 
 
  
  //mul_pic_attach_buffer(pic_adv_handle, p_buffer, dec_buffer_size); 

  mul_pic_register_evt(pic_adv_handle, pic_api_callback);
  
  //OS_PRINTF("mul pic attach buffer: 0x%x, size : 0x%x\n", (u32)p_buffer, dec_buffer_size);  

  fw_register_ap_evtmap(APP_PICTURE, ui_pic_evtmap);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_MAINMENU); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_BAR); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_LIST); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_VOLUME);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_SMALL_LIST);    
}

void ui_adv_pic_release(void)
{
  OS_PRINTF("pic chain destoryed, 0x%x\n", pic_adv_handle);

  if(pic_adv_handle != 0)
  {
    mul_pic_destroy_chain(pic_adv_handle);

    pic_adv_handle = 0;
  }
#ifndef WIN32
  ui_adv_pic_region_release();
#endif
  fw_unregister_ap_evtmap(APP_PICTURE);
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_MAINMENU); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PROG_BAR); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PROG_LIST); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_VOLUME);
}


void ui_adv_set_fullscr_rect(u16 x, u16 y, u16 w, u16 h)
{
  if(x > (SCREEN_WIDTH - w))
    x = SCREEN_WIDTH - w;
  
  if(y > (SCREEN_HEIGHT- h))
    y = SCREEN_HEIGHT - h;

  g_rect.left = (s16)x;
  g_rect.right = (s16)(g_rect.left + w);
  g_rect.top = (s16)y;
  g_rect.bottom = (s16)(g_rect.top + h);
}

//lint -e64
BOOL ui_adv_set_pic_by_type(ads_ad_type_t type)
{
  RET_CODE ret = SUCCESS;
#ifndef WIN32
#ifdef ENABLE_ADS
  ads_info_t ads_info = {0};
  u16 pg_id = 0;
  dvbs_prog_node_t pg = {0};
  if((pg_id = sys_status_get_curn_group_curn_prog_id()) != INVALIDID)
  {
    if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
    {
      OS_PRINTF("ADV ui_adv_set_pic_by_type SID[%d] TS_ID[%d] NET_ID[%d]\n",pg.s_id, pg.ts_id,pg.orig_net_id);
      ads_info.service_id = (u16)pg.s_id;
      ads_info.ts_id = (u16)pg.ts_id;
      ads_info.network_id= (u16)pg.orig_net_id;
    }  
  }

  ads_info.ads_media_type = ADS_AD_TYPE_PIC;
  ads_info.ads_type = type;
  OS_PRINTF("%s(%d)  type=[%d]  \n", __FUNCTION__, __LINE__, type);

  switch(type)
  {
    case ADS_AD_TYPE_MENU_UP:
      g_rect.left = 160 + 90;
      g_rect.right = g_rect.left + 300;
      g_rect.top = 60 + 200;
      g_rect.bottom = g_rect.top + 330;
      break;
      
      case ADS_AD_TYPE_CHLIST:
      g_rect.left = 160 + 80;
      g_rect.right = g_rect.left+300;
      g_rect.top = 60 + 466;
      g_rect.bottom = g_rect.top+130;
      break;
      
      case ADS_AD_TYPE_CHBAR:
      g_rect.left = 160 + 620;
      g_rect.right = g_rect.left+300;
      g_rect.top = 60 + 446;
      g_rect.bottom = g_rect.top+130;
      break;
      
      case ADS_AD_TYPE_MENU_DOWN:
      g_rect.left = 407;
      g_rect.right = g_rect.left+280;
      g_rect.top = 333;
      g_rect.bottom = g_rect.top+140;
      break;

      case ADS_AD_TYPE_SUB_MENU:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 135;
      g_rect.bottom = g_rect.top+110;
      break;
      
    case ADS_AD_TYPE_SEARCHING:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 90;
      g_rect.bottom = g_rect.top+110;
      break;
      
    case ADS_AD_TYPE_SIGNAL:
      g_rect.left = 60;
      g_rect.right = g_rect.left+245;
      g_rect.top = 160;
      g_rect.bottom = g_rect.top+110;
      break;
      
    case ADS_AD_TYPE_MAIN_MENU:
      g_rect.left = 12;
      g_rect.right = 12 +265;
      g_rect.top = 15;
      g_rect.bottom = 15+220;
      break;

    case ADS_AD_TYPE_BANNER:
      g_rect.left = 417;
      g_rect.right = g_rect.left+280;
      g_rect.top = 350;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_OTHER_GIF:
      g_rect.left = 42;
      g_rect.right = g_rect.left+280;
      g_rect.top = 375;
      g_rect.bottom = g_rect.top+140;
      break;

    case ADS_AD_TYPE_CHANGE_CHANNEL:
      g_rect.left = 0;
      g_rect.right = SCREEN_WIDTH - g_rect.left;
      g_rect.top = 0;
      g_rect.bottom = SCREEN_HEIGHT - g_rect.top;
      break;

    case ADS_AD_TYPE_VOLBAR:
      g_rect.left = 984;
      g_rect.right = g_rect.left+250;
      g_rect.top = 525;
      g_rect.bottom = g_rect.top+120;
      break;

    case ADS_AD_POWER_OFF:
      g_rect.left = 0;
      g_rect.right = g_rect.left+1280;
      g_rect.top = 0;
      g_rect.bottom = g_rect.top+720;
      break;
      
    default:
      g_rect.left = 0;
      g_rect.right = SCREEN_WIDTH - g_rect.left;
      g_rect.top = 0;
      g_rect.bottom = SCREEN_HEIGHT - g_rect.top;
      break;
  }
    
  OS_PRINTF("start show picture tickets2=%d\n", mtos_ticks_get());

  ret = ads_display(g_adm_id, &ads_info);

#endif
#endif
  OS_PRINTF("ui_adv_set_pic_by_type ret = %d\n", ret); 
  return (ret==SUCCESS)?TRUE : FALSE;
}

//lint +e64

BOOL ui_adv_get_pic_rect(rect_t * rect_region)
{
  BOOL ret = FALSE;
  
  if(pic_adv_region != NULL)
  {
    rect_region->top = g_rect.top;
    rect_region->bottom = g_rect.bottom;
    rect_region->left = g_rect.left;
    rect_region->right = g_rect.right;
    ret =  TRUE;
  }
  return ret;
}

BOOL ui_adv_pic_play(ads_ad_type_t type, u8 root_id)
{
  BOOL ret = FALSE;

  OS_PRINTF("ADV ui_pic_play type[%d]\n",type);
  pic_adv_stop();
  if(ui_adv_set_pic_by_type(type))
  {
    ret = TRUE;
  }

  return ret;
}

static u8* get_adv_pic_buf_addr(u32* p_len)
{
  *p_len = g_pic_size;
  return g_adv_pic_addr;
}

static void ui_adv_set_pic_param(u8 *pic_add,u32 size)
{
  OS_PRINTF("#@@@ set pic addr = 0x%x, size = 0x%x \n",pic_add, size);
  g_adv_pic_addr = pic_add;
  g_pic_size =  size;
}

RET_CODE ui_adv_set_pic(u32 size, u8 *pic_add)
{
  mul_pic_param_t pic_param = {{0},0};
  RET_CODE ret = ERR_FAILURE;
  rect_t watermark_rect = {0};

  OS_PRINTF("ADV ui_adv_set_pic length[0x%x]\n",size);
  if(pic_add == NULL || size == 0)
  {
    OS_PRINTF("ADV p_data = NULL");
    return ERR_FAILURE;
  }
   // close watermark
  if(ui_get_watermark_rect(&watermark_rect))
  {
    if(is_region_y_coordinate_covered(&g_rect, &watermark_rect))
    {
      OS_PRINTF("region covered, close watermark for adv show \n");
      ui_watermark_release();
    }
    else
    {
      ui_water_mark_refresh();
    }
  }
  #ifndef WIN32
  ret = ui_adv_pic_region_init(&g_rect);
#else
  pic_adv_region = (void *)gdi_get_screen_handle(FALSE);
#endif
  if(ret != SUCCESS)
  {
    OS_PRINTF("ui_adv_set_pic ui_adv_pic_region_init FAIL , LINE = %d\n", __LINE__);
    return ERR_FAILURE;
  }
  mul_pic_set_rend_rgn(pic_adv_handle, pic_adv_region); 

  ui_adv_set_pic_param(pic_add, size);
  
  OS_PRINTF("ADV ui_adv_set_pic g_rect.x[%d], t[%d], r[%d], b[%d]\n",g_rect.left,g_rect.top,g_rect.right,g_rect.bottom);
  pic_param.anim = REND_ANIM_NONE;
  pic_param.file_size = 0;
  pic_param.is_file = FALSE;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = 0;// g_rect.left;
  pic_param.win_rect.top = 0;//g_rect.top;
  pic_param.win_rect.right = (g_rect.right - g_rect.left);
  pic_param.win_rect.bottom = (g_rect.bottom - g_rect.top);
  pic_param.style = REND_STYLE_CENTER;
  pic_param.buf_get = (u32)get_adv_pic_buf_addr; 

  pic_adv_start(&pic_param);
  return SUCCESS;
}


/**********************************Multiple picture support*****************************************/


typedef struct adv_mul_region_
{
  handle_t pic_handle;
  mul_pic_chan_t chain;
  void *p_region;
  void *p_region_buffer;
  u32 region_buffer_size;

  rect_t rect;
  mul_pic_param_t pic_param;
  struct adv_mul_region_ *next;
}adv_mul_pic_t;

//lint -e656
#define ADV_MUL_MAX_PIC ((AD_MUL_PIC_DECODE_END) - (AD_MUL_PIC_DECODE_START) + 1)
static adv_mul_pic_t *adv_mul_pic_list = NULL;
static u8* adv_pic_addr[ADV_MUL_MAX_PIC] = {NULL};
static u32 adv_pic_size[ADV_MUL_MAX_PIC] = {0};
//lint +e656
static u8 adv_mul_pic_index = 0;
static adv_nofity adv_notify_func = NULL;

static adv_mul_pic_t *add_region_node(adv_mul_pic_t *p_node)
{
  MT_ASSERT(p_node != NULL);

  if (adv_mul_pic_list == NULL)
  {
    adv_mul_pic_list = p_node;
  }
  else
  {
    adv_mul_pic_t *p;
    p = adv_mul_pic_list;
    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = p_node;
  }

  return adv_mul_pic_list;
}


static u8* get_adv_pic_addr(u32 *p_len, u8 pic_index)
{
  MT_ASSERT(p_len != NULL);
  //lint -save -e656
  MT_ASSERT(pic_index < ADV_MUL_MAX_PIC);
  //lint -restore
  //lint -save -e661 -e613
  *p_len = adv_pic_size[pic_index];
  return adv_pic_addr[pic_index];
  //lint -restore
}

static u8* get_adv_pic_buf_addr0(u32* p_len)
{
  return get_adv_pic_addr(p_len, 0);
}
static u8* get_adv_pic_buf_addr1(u32* p_len)
{
  return get_adv_pic_addr(p_len, 1);
}
static u8* get_adv_pic_buf_addr2(u32* p_len)
{
  return get_adv_pic_addr(p_len, 2);
}
static u8* get_adv_pic_buf_addr3(u32* p_len)
{
  return get_adv_pic_addr(p_len, 3);
}

static void mul_pic_lock(void)
{
  if (ui_adv_chain_mutext == NULL)
  {
    ui_adv_chain_mutext = mtos_mutex_create(1);
  }

  MT_ASSERT(ui_adv_chain_mutext != NULL);
  mtos_mutex_take(ui_adv_chain_mutext);
}

static void mul_pic_unlock(void)
{
  if (ui_adv_chain_mutext == NULL)
  {
    ui_adv_chain_mutext = mtos_mutex_create(1);
  }

  MT_ASSERT(ui_adv_chain_mutext != NULL);
  mtos_mutex_give(ui_adv_chain_mutext);
}

//lint -e438 -e550
RET_CODE ui_adv_add_mul_pic(u32 size, u8 *pic_add ,rect_t rect)
{
  mul_pic_param_t pic_param = {{0},0};
  adv_mul_pic_t *adv_mul_pic_node;
  RET_CODE ret = ERR_FAILURE;
   
  //OS_PRINTF("ADV ui_adv_add_mul_pic length[0x%x]\n",size);
  MT_ASSERT(pic_add != NULL);
  MT_ASSERT(size != 0);
  //lint -save -e656
  MT_ASSERT(adv_mul_pic_index < ADV_MUL_MAX_PIC);
  //lint -restore

  adv_mul_pic_node = mtos_malloc(sizeof(adv_mul_pic_t));
  MT_ASSERT(adv_mul_pic_node != NULL);
  //lint -save -e668
  memset(adv_mul_pic_node, 0, sizeof(adv_mul_pic_t));
  //lint -restore

  // TODO: Check rect overlap
  adv_mul_pic_node->rect = rect;
  adv_mul_pic_node->chain.task_stk_size = 16 * KBYTES;
  adv_mul_pic_node->chain.anim_pri = AD_MUL_PIC_DECODE_START + adv_mul_pic_index;
  adv_mul_pic_node->chain.anim_stk_size = 4 * KBYTES;
  ret = mul_pic_create_buffer_chain(&adv_mul_pic_node->pic_handle, &adv_mul_pic_node->chain);
  MT_ASSERT(ret == SUCCESS);
  
  mul_pic_set_dec_mode(adv_mul_pic_node->pic_handle, DEC_FRAME_MODE); 
  mul_pic_register_evt(adv_mul_pic_node->pic_handle, pic_api_callback);
  
#ifndef WIN32
  // init region
  rect_size_t rect_size = {0};
  point_t pos;  
  u32 align = 0;
  u32 rgn_size = 0;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  
  
  //set coordinate
  pos.x = rect.left;
  pos.y = rect.top;
  rect_size.w = (((u16)(rect.right - rect.left) >> 3) + 1) <<3; //must be multiple of 8
  rect_size.h =  (u16)(rect.bottom - rect.top);
  
  OS_PRINTF("@@@region x=%d, y = %d, w = %d, h = %d \n",pos.x,pos.y,rect_size.w,rect_size.h);
  adv_mul_pic_node->p_region = region_create(&rect_size, PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != adv_mul_pic_node->p_region);

  disp_calc_region_size(p_disp_dev, DISP_LAYER_ID_SUBTITL, adv_mul_pic_node->p_region, &align, &rgn_size);
  adv_mul_pic_node->region_buffer_size = rgn_size + align;
  adv_mul_pic_node->p_region_buffer = mtos_align_dma_malloc(adv_mul_pic_node->region_buffer_size, 8);
  memset(adv_mul_pic_node->p_region_buffer, 0, adv_mul_pic_node->region_buffer_size);

  ret = disp_layer_add_region(p_disp_dev, DISP_LAYER_ID_SUBTITL,  adv_mul_pic_node->p_region, &pos, adv_mul_pic_node->p_region_buffer);
  MT_ASSERT(ret == SUCCESS);
  
#else
  adv_mul_pic_node->p_region = (void *)gdi_get_screen_handle(FALSE);
#endif
  mul_pic_set_rend_rgn(adv_mul_pic_node->pic_handle, adv_mul_pic_node->p_region); 
  //mul_pic_attach_buffer(adv_mul_pic_node->pic_handle, adv_mul_pic_node->p_region_buffer, adv_mul_pic_node->region_buffer_size); 

  pic_param.anim = REND_ANIM_NONE;
  pic_param.file_size = 0;
  pic_param.is_file = FALSE;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = 0;//rect.left;
  pic_param.win_rect.top = 0;//rect.top;
  pic_param.win_rect.right = rect.right - rect.left;
  pic_param.win_rect.bottom = rect.bottom - rect.top;
  pic_param.style = REND_STYLE_CENTER;

  adv_pic_size[adv_mul_pic_index] = size;
  adv_pic_addr[adv_mul_pic_index] = pic_add;
  if (adv_mul_pic_index == 0)
  {
    pic_param.buf_get = (u32)get_adv_pic_buf_addr0; 
  }
  else if (adv_mul_pic_index == 1)
  {
    pic_param.buf_get = (u32)get_adv_pic_buf_addr1; 
  }
  else if (adv_mul_pic_index == 2)
  {
    pic_param.buf_get = (u32)get_adv_pic_buf_addr2; 
  }
  else if (adv_mul_pic_index == 3)
  {
    pic_param.buf_get = (u32)get_adv_pic_buf_addr3; 
  }
  else
  {
    MT_ASSERT(0);
  }

  adv_mul_pic_node->pic_param = pic_param;
  
  mul_pic_lock();
  add_region_node(adv_mul_pic_node);
  adv_mul_pic_index++;
  mul_pic_unlock();
  return SUCCESS;
}


RET_CODE ui_adv_start_mul_pic(void)
{
  RET_CODE ret;
  adv_mul_pic_t *p;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

  mul_pic_lock();
  p = adv_mul_pic_list;
  while (p != NULL)
  {
    if(p->pic_handle != 0)
    {
      ret = mul_pic_start(p->pic_handle, &p->pic_param);
      MT_ASSERT(ret == SUCCESS);
    }
    region_show(p->p_region, TRUE);
    p = p->next;
  }
  mul_pic_unlock();

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, TRUE);
  
  return SUCCESS;
}
RET_CODE ui_adv_stop_mul_pic(void)
{
  RET_CODE ret;
  adv_mul_pic_t *p;
  adv_mul_pic_t *pre;
  BOOL b_disable_sub_show = FALSE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  
  mul_pic_lock();
  p = adv_mul_pic_list;
  if(p != NULL)
  {
    b_disable_sub_show = TRUE;
  }
  
  while (p != NULL)
  {
    OS_PRINTF("[%s][%d]p->pic_handle:%p\n",__FUNCTION__,__LINE__,p->pic_handle);
    ret = mul_pic_stop(p->pic_handle);
    MT_ASSERT(ret == SUCCESS);
    ret = mul_pic_destroy_chain(p->pic_handle);
    MT_ASSERT(ret == SUCCESS);
    disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, p->p_region);
    region_delete(p->p_region);
    p->p_region = NULL;
    mtos_align_dma_free(p->p_region_buffer);

    pre = p;
    p = p->next;
    mtos_free(pre);
    pre = NULL;
  }
  adv_mul_pic_list = NULL;
  adv_mul_pic_index = 0;
  mul_pic_unlock();
  if(b_disable_sub_show)
  {
    disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, FALSE);
  }
  return SUCCESS;
}
//lint +e438 +e550

void ui_adv_register_nofity(adv_nofity func)
{
  adv_notify_func = func;
}

RET_CODE ui_adv_notify(ads_ad_type_t type, void *param)
{
  if (adv_notify_func != NULL)
  {
    return adv_notify_func(type, param);
  }
  return ERR_FAILURE;
}


/**************************************************************
********************** END ************************************/
