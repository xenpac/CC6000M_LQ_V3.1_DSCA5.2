/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"

#include "news_filter.h"
#include "news_api.h"

#include "ui_news_api.h"

typedef enum
{
  NEWS_API_EVT_FAILED = ((APP_NEWS << 16) + 0),
  NEWS_API_EVT_SUCCESS,
}albums_api_evt_t;

static u16 news_context = 0;

static news_t g_news_data = {0};

static news_html_t g_news_detail_data = {0};

u16 ui_news_evtmap(u32 event);

void free_news_mem()
{
  u16 i = 0;// str_len = 0;
  
  for(i = 0; i < g_news_data.count; i ++)
  {
  	if(g_news_data.p_item[i].p_title)
		{
			mtos_free(g_news_data.p_item[i].p_title);
		}
    if(g_news_data.p_item[i].p_description)
    {
      mtos_free(g_news_data.p_item[i].p_description);
    }
    if(g_news_data.p_item[i].p_link)
    {
      mtos_free(g_news_data.p_item[i].p_link);
    }
    if(g_news_data.p_item[i].p_pubDate)
    {
      mtos_free(g_news_data.p_item[i].p_pubDate);
    }

  	if(g_news_data.p_item[i].source.p_url)
		{
			mtos_free(g_news_data.p_item[i].source.p_url);
		}
		
    if(g_news_data.p_item[i].source.p_text)
    {
      mtos_free(g_news_data.p_item[i].source.p_text);
    }
		
    if(g_news_data.p_item[i].media_content.p_url)
    {
      mtos_free(g_news_data.p_item[i].media_content.p_url);
    }
    
    if(g_news_data.p_item[i].media_text.p_content)
    {
      mtos_free(g_news_data.p_item[i].media_text.p_content);
    }
    
    if(g_news_data.p_item[i].media_credit.p_role)
    {
      mtos_free(g_news_data.p_item[i].media_credit.p_role);
    }
  }	

  memset(&g_news_data, 0, sizeof(news_t));
}

void ui_news_save_news(news_t *p_news)
{
  u16 i = 0;// str_len = 0;
  
  MT_ASSERT(p_news != NULL);

  //free previous albums data.
  
  free_news_mem();
  //save new data.
  g_news_data.count = p_news->count;

  if(g_news_data.count == 0)
  {
    return;
  }

  g_news_data.p_item = (news_item_t *)mtos_malloc(g_news_data.count * sizeof(news_item_t));

  MT_ASSERT(g_news_data.p_item != NULL);

  memset(g_news_data.p_item, 0, g_news_data.count * sizeof(news_item_t));

  for(i = 0; i < g_news_data.count; i++)
  {
    //title
    if(p_news->p_item[i].p_title != NULL)
    {
    	g_news_data.p_item[i].p_title = (char *)mtos_malloc(strlen(p_news->p_item[i].p_title) + 1);
    	memset(g_news_data.p_item[i].p_title, 0, strlen(p_news->p_item[i].p_title) + 1);
    	strcpy(g_news_data.p_item[i].p_title, p_news->p_item[i].p_title);
    }
    		
    //<description>
    if(p_news->p_item[i].p_description != NULL)
    {
      g_news_data.p_item[i].p_description = 
        (char *)mtos_malloc(strlen(p_news->p_item[i].p_description) + 1);
      memset(g_news_data.p_item[i].p_description, 0, strlen(p_news->p_item[i].p_description) + 1);
      strcpy(g_news_data.p_item[i].p_description, p_news->p_item[i].p_description);
    }

    //<link>
    if(p_news->p_item[i].p_link != NULL)
    {
    	g_news_data.p_item[i].p_link = (char *)mtos_malloc(strlen(p_news->p_item[i].p_link) + 1);
    	memset(g_news_data.p_item[i].p_link, 0, strlen(p_news->p_item[i].p_link) + 1);
    	strcpy(g_news_data.p_item[i].p_link, p_news->p_item[i].p_link);
    }

    //<pubDate>
    if(p_news->p_item[i].p_pubDate != NULL)
    {
    	g_news_data.p_item[i].p_pubDate= (char *)mtos_malloc(strlen(p_news->p_item[i].p_pubDate) + 1);
    	memset(g_news_data.p_item[i].p_pubDate, 0, strlen(p_news->p_item[i].p_pubDate) + 1);
    	strcpy(g_news_data.p_item[i].p_pubDate, p_news->p_item[i].p_pubDate);
    }

    //<source>
    if(p_news->p_item[i].source.p_url != NULL)
    {
    	g_news_data.p_item[i].source.p_url =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].source.p_url) + 1);
    	memset(g_news_data.p_item[i].source.p_url, 0, strlen(p_news->p_item[i].source.p_url) + 1);
    	strcpy(g_news_data.p_item[i].source.p_url, p_news->p_item[i].source.p_url);
    }

    if(p_news->p_item[i].source.p_text != NULL)
    {
    	g_news_data.p_item[i].source.p_text =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].source.p_text) + 1);
    	memset(g_news_data.p_item[i].source.p_text, 0, strlen(p_news->p_item[i].source.p_text) + 1);
    	strcpy(g_news_data.p_item[i].source.p_text, p_news->p_item[i].source.p_text);
    }

    //<media:content>
    if(p_news->p_item[i].media_content.p_url != NULL)
    {
    	g_news_data.p_item[i].media_content.p_url =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].media_content.p_url) + 1);
    	memset(g_news_data.p_item[i].media_content.p_url,
    	  0, strlen(p_news->p_item[i].media_content.p_url) + 1);
    	strcpy(g_news_data.p_item[i].media_content.p_url, p_news->p_item[i].media_content.p_url);
    }

    if(p_news->p_item[i].media_content.p_type != NULL)
    {
    	g_news_data.p_item[i].media_content.p_type =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].media_content.p_type) + 1);
    	memset(g_news_data.p_item[i].media_content.p_type,
    	  0, strlen(p_news->p_item[i].media_content.p_type) + 1);
    	strcpy(g_news_data.p_item[i].media_content.p_type, p_news->p_item[i].media_content.p_type);
    }

    g_news_data.p_item[i].media_content.width = p_news->p_item[i].media_content.width;

    g_news_data.p_item[i].media_content.height = p_news->p_item[i].media_content.height;

    //<media:text>
    if(p_news->p_item[i].media_text.p_type != NULL)
    {
    	g_news_data.p_item[i].media_text.p_type =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].media_text.p_type) + 1);
    	memset(g_news_data.p_item[i].media_text.p_type,
    	  0, strlen(p_news->p_item[i].media_text.p_type) + 1);
    	strcpy(g_news_data.p_item[i].media_text.p_type, p_news->p_item[i].media_text.p_type);
    }

    if(p_news->p_item[i].media_text.p_content != NULL)
    {
    	g_news_data.p_item[i].media_text.p_content =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].media_text.p_content) + 1);
    	memset(g_news_data.p_item[i].media_text.p_content,
    	  0, strlen(p_news->p_item[i].media_text.p_content) + 1);
    	strcpy(g_news_data.p_item[i].media_text.p_content, p_news->p_item[i].media_text.p_content);
    }

    //<media:credit>
    if(p_news->p_item[i].media_credit.p_role != NULL)
    {
    	g_news_data.p_item[i].media_credit.p_role =
    	  (char *)mtos_malloc(strlen(p_news->p_item[i].media_credit.p_role) + 1);
    	memset(g_news_data.p_item[i].media_credit.p_role,
    	  0, strlen(p_news->p_item[i].media_credit.p_role) + 1);
    	strcpy(g_news_data.p_item[i].media_credit.p_role, p_news->p_item[i].media_credit.p_role);
    }
  }
  
}

void free_news_html()
{
  u16 i = 0;// str_len = 0;
  
  //free img.
  for(i = 0; i < g_news_detail_data.img_cnt; i ++)
  {
    if(g_news_detail_data.p_img[i])
    {
      mtos_free(g_news_detail_data.p_img[i]);
    }
  }
  //free html content.
  if(g_news_detail_data.p_segment != NULL)
  {
    mtos_free(g_news_detail_data.p_segment);
  } 	

  memset(&g_news_detail_data, 0, sizeof(news_html_t));
}

static void ui_news_detail_save_news(news_html_t *p_news)
{
  u16 i = 0;// str_len = 0;
  
  MT_ASSERT(p_news != NULL);
  
  free_news_html();

  //set img content
  for(i = 0; i < p_news->img_cnt; i++)
  {
    g_news_detail_data.p_img[i] = (char *)mtos_malloc(strlen(p_news->p_img[i]) + 1);
    memcpy(g_news_detail_data.p_img[i], p_news->p_img[i], (strlen(p_news->p_img[i]) + 1));
  }
  
  g_news_detail_data.img_cnt = p_news->img_cnt;
  g_news_detail_data.seg_len = p_news->seg_len;
  //set txt content.
  if(p_news->p_segment != 0)
  {
    g_news_detail_data.p_segment = (char *)mtos_malloc(strlen(p_news->p_segment) + 1);
    memcpy(g_news_detail_data.p_segment, p_news->p_segment, (strlen(p_news->p_segment) + 1));
  }
}


static void ui_news_call_back(news_evt_t event, void *p_data, u16 context)
{
  event_t evt = {0};
  
  switch(event)
  {
    case NEWS_EVT_GET_NEWS:
      if(context == news_context)
      {
        if(p_data== NULL)
        {
          //got error. should notify user
		      evt.id = NEWS_API_EVT_FAILED;
          evt.data1 = NEWS_EVT_GET_NEWS;

          ap_frm_send_evt_to_ui(APP_NEWS, &evt);
        }
        else
        {
          //nofigy ui got a category
		      evt.id = NEWS_API_EVT_SUCCESS;
          evt.data1 = NEWS_EVT_GET_NEWS;
          
          evt.data2 = (u32)(&g_news_data);

          ui_news_save_news((news_t *)p_data);

          ap_frm_send_evt_to_ui(APP_NEWS, &evt); 
        }
      } 
      break;
      case NEWS_EVT_GET_DETAIL:
      if(context == news_context)
      {
        if(p_data== NULL)
        {
          //got error. should notify user
		      evt.id = NEWS_API_EVT_FAILED;
          evt.data1 = NEWS_EVT_GET_NEWS;

          ap_frm_send_evt_to_ui(APP_NEWS, &evt);
        }
        else
        {
          //nofigy ui got a category
		      evt.id = NEWS_API_EVT_SUCCESS;
          evt.data1 = NEWS_EVT_GET_DETAIL;
          
          evt.data2 = (u32)(&g_news_detail_data);

          ui_news_detail_save_news((news_html_t *)p_data);

          ap_frm_send_evt_to_ui(APP_NEWS, &evt); 
        }
      }
      break;

    default:
      break;
  }
}


void ui_news_init(news_site_t site)
{
  mul_news_attr_t attr = {0};
  RET_CODE ret = SUCCESS;

  attr.site = site;
  attr.cb = ui_news_call_back;
  attr.stk_size = ALBUMS_CHAIN_TASK_STK_SIZE;
  
  ret = mul_news_create_chn(&attr);
  if(ret != SUCCESS)
  {
    MT_ASSERT(0);
  }
  
  mul_news_start();

  fw_register_ap_evtmap(APP_NEWS, ui_news_evtmap);
  fw_register_ap_msghost(APP_NEWS, ROOT_ID_YAHOO_NEWS); 
  fw_register_ap_msghost(APP_NEWS, ROOT_ID_NEWS_TITLE);   
  fw_register_ap_msghost(APP_NEWS, ROOT_ID_NEWS_INFO);
}

void ui_news_release(void)
{
  RET_CODE ret = SUCCESS;
  
  mul_news_stop();

  ret = mul_news_destroy_chn();
  if(ret != SUCCESS)
  {
    MT_ASSERT(0);
  }

  fw_unregister_ap_evtmap(APP_NEWS);
  fw_unregister_ap_msghost(APP_NEWS, ROOT_ID_YAHOO_NEWS); 
  fw_unregister_ap_msghost(APP_NEWS, ROOT_ID_NEWS_TITLE); 
  fw_unregister_ap_msghost(APP_NEWS, ROOT_ID_NEWS_INFO);
}


RET_CODE ui_news_get(news_req_t *p_news)
{
  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  news_context++;
 
  return mul_news_get(p_news, news_context);
}

RET_CODE ui_news_detail_create_filter(news_t *p_news, u16 pos)
{
  char *url = NULL;
  
  if(p_news == NULL)
  {
    return ERR_FAILURE;
  }

  news_context++;

  url = p_news->p_item[pos].p_link;
 
  if(url == NULL || strlen(url) == 0)
  {
    return ERR_FAILURE;
  }
 
  return mul_news_get_detail(url, news_context);
}


news_t *ui_news_get_news(void)
{
  return &g_news_data;
}

news_html_t *ui_news_get_detail(void)
{
  return &g_news_detail_data;
}

BEGIN_AP_EVTMAP(ui_news_evtmap)
  CONVERT_EVENT(NEWS_API_EVT_FAILED, MSG_NEWS_EVT_FAILED)
  CONVERT_EVENT(NEWS_API_EVT_SUCCESS, MSG_NEWS_EVT_SUCCESS)
END_AP_EVTMAP(ui_news_evtmap)
