/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"

#include "albums_filter.h"
#include "albums_public.h"
#include "albums_api.h"

#include "ui_albums_api.h"

typedef enum
{
  ALBUMS_API_EVT_FAILED = ((APP_ALBUM << 16) + 0),
  ALBUMS_API_EVT_SUCCESS,
}albums_api_evt_t;

static u16 albums_context = 0;

static photos_t g_photos_data = {0};

static albums_t g_albums_data = {0};

u16 ui_albums_evtmap(u32 event);

void ui_albums_free_albums()
{
  u16 i = 0;

  for(i = 0; i < g_albums_data.album_total; i++)
  {
    if(g_albums_data.p_album[i].p_album_id != NULL)
  	{
      mtos_free(g_albums_data.p_album[i].p_album_id);
      g_albums_data.p_album[i].p_album_id = NULL;
    }

    if(g_albums_data.p_album[i].p_album_name != NULL)
    {
      mtos_free(g_albums_data.p_album[i].p_album_name);
      g_albums_data.p_album[i].p_album_name = NULL;
    }
    
    if(g_albums_data.p_album[i].p_user_id != NULL)
    {
      mtos_free(g_albums_data.p_album[i].p_user_id);
      g_albums_data.p_album[i].p_user_id = NULL;
    }
    
    if(g_albums_data.p_album[i].p_user_name != NULL)
    {
      mtos_free(g_albums_data.p_album[i].p_user_name);
      g_albums_data.p_album[i].p_user_name = NULL;
    }
    
    if(g_albums_data.p_album[i].thumb.p_thumb_url != NULL)
    {
      mtos_free(g_albums_data.p_album[i].thumb.p_thumb_url);
      g_albums_data.p_album[i].thumb.p_thumb_url = NULL;
    }		
  }

  if(g_albums_data.p_album != NULL)
  {
    mtos_free(g_albums_data.p_album);
    g_albums_data.p_album = NULL;
  }

  memset(&g_albums_data, 0, sizeof(g_albums_data));
}

void ui_albums_save_albums(albums_t *p_albums)
{
  u16 i = 0, str_len = 0;
  
  MT_ASSERT(p_albums != NULL);

  //free previous albums data.
  ui_albums_free_albums();

  //save new data.
  g_albums_data.album_total = p_albums->album_total;

  if(g_albums_data.album_total == 0)
  {
    return;
  }

  g_albums_data.p_album = 
    (album_info_t *)mtos_malloc(g_albums_data.album_total * sizeof(album_info_t));
  MT_ASSERT(g_albums_data.p_album != NULL);  
  memset(g_albums_data.p_album, 0, g_albums_data.album_total * sizeof(album_info_t));
  
  for(i = 0; i < g_albums_data.album_total; i++)
  {
		g_albums_data.p_album[i].photos_num = p_albums->p_album[i].photos_num;
		g_albums_data.p_album[i].thumb.height = p_albums->p_album[i].thumb.height;
    g_albums_data.p_album[i].thumb.width = p_albums->p_album[i].thumb.width;
    
    //save albums id.
    if(p_albums->p_album[i].p_album_id != NULL)
    {
      str_len = (u16)strlen(p_albums->p_album->p_album_id);

      g_albums_data.p_album[i].p_album_id = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_albums_data.p_album[i].p_album_id != NULL);

      memset(g_albums_data.p_album[i].p_album_id, 0, str_len + 1);      

      memcpy(g_albums_data.p_album[i].p_album_id, p_albums->p_album[i].p_album_id, str_len);
    }

		//save album name.
    if(p_albums->p_album[i].p_album_name != NULL)
    {
      str_len = (u16)strlen(p_albums->p_album[i].p_album_name);

      g_albums_data.p_album[i].p_album_name = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_albums_data.p_album[i].p_album_name != NULL);

      memset(g_albums_data.p_album[i].p_album_name, 0, str_len + 1);

      memcpy(g_albums_data.p_album[i].p_album_name, p_albums->p_album[i].p_album_name, str_len);
    }

		//save user id.
    if(p_albums->p_album[i].p_user_id != NULL)
    {
      str_len = (u16)strlen(p_albums->p_album[i].p_user_id);

      g_albums_data.p_album[i].p_user_id = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_albums_data.p_album[i].p_user_id != NULL);

      memset(g_albums_data.p_album[i].p_user_id, 0, str_len + 1);

      memcpy(g_albums_data.p_album[i].p_user_id, p_albums->p_album[i].p_user_id, str_len);
    }
		
		
		//save user name.
    if(p_albums->p_album[i].p_user_name != NULL)
    {
      str_len = (u16)strlen(p_albums->p_album[i].p_user_name);

      g_albums_data.p_album[i].p_user_name = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_albums_data.p_album[i].p_user_name != NULL);

      memset(g_albums_data.p_album[i].p_user_name, 0, str_len + 1);

      memcpy(g_albums_data.p_album[i].p_user_name, p_albums->p_album[i].p_user_name, str_len);
    }


    //save thumbnail url
    if(p_albums->p_album[i].thumb.p_thumb_url != NULL)
    {
      str_len = (u16)strlen(p_albums->p_album[i].thumb.p_thumb_url );

      g_albums_data.p_album[i].thumb.p_thumb_url = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_albums_data.p_album[i].thumb.p_thumb_url  != NULL);

      memset(g_albums_data.p_album[i].thumb.p_thumb_url, 0, str_len + 1);

      memcpy(g_albums_data.p_album[i].thumb.p_thumb_url,
        p_albums->p_album[i].thumb.p_thumb_url , str_len);
    }
  }  
}

void ui_albums_free_photos()
{
  u16 i = 0, j = 0;

  for(i = 0; i < g_photos_data.cnt; i++)
  {
		if(g_photos_data.p_photo[i].p_photo_url != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_photo_url);
			g_photos_data.p_photo[i].p_photo_url = NULL;
		}

  	//free photo id.
		if(g_photos_data.p_photo[i].p_photo_id != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_photo_id);
			g_photos_data.p_photo[i].p_photo_id = NULL;
		}

  	//free photo id.
		if(g_photos_data.p_photo[i].p_photo_name != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_photo_name);
			g_photos_data.p_photo[i].p_photo_name = NULL;
		}

		//free album id.
		if(g_photos_data.p_photo[i].p_album_id != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_album_id);
			g_photos_data.p_photo[i].p_album_id = NULL;
		}

		//free user id.
		if(g_photos_data.p_photo[i].p_user_id != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_user_id);
			g_photos_data.p_photo[i].p_user_id = NULL;
		}

        #if 0
        //free user name.
		if(g_photos_data.p_photo[i].p_user_name != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_user_name);
			g_photos_data.p_photo[i].p_user_name = NULL;
		}
        #endif

		//free description.
		if(g_photos_data.p_photo[i].p_photo_description != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_photo_description);
			g_photos_data.p_photo[i].p_photo_description = NULL;
		}


		//free thumbnail url
		for(j = 0; j < g_photos_data.p_photo[i].thumb_num; j++)
		{
			if(g_photos_data.p_photo[i].p_thumb[j].p_thumb_url != NULL)
			{
				mtos_free(g_photos_data.p_photo[i].p_thumb[j].p_thumb_url);
				g_photos_data.p_photo[i].p_thumb[j].p_thumb_url = NULL;
			}
		}

		if(g_photos_data.p_photo[i].p_thumb != NULL)
		{
			mtos_free(g_photos_data.p_photo[i].p_thumb);
			g_photos_data.p_photo[i].p_thumb = NULL;
		}
  }

  if(g_photos_data.p_photo != NULL)
  {
		mtos_free(g_photos_data.p_photo);
		g_photos_data.p_photo = NULL;
  }

  memset(&g_photos_data, 0, sizeof(g_photos_data));
}

void ui_albums_save_photos(photos_t *p_photos)
{
  u16 i = 0, j = 0, m = 0, n = 0;
  u16 tw = 0, th = 0;
  char *p_t = NULL;  
  u16 str_len = 0;
  
  MT_ASSERT(p_photos != NULL);
  //free the previeous photos list.
  ui_albums_free_photos();
  
  //save the new photos list.
  g_photos_data.per_page = p_photos->per_page;
  g_photos_data.page = p_photos->page;
  g_photos_data.cnt = p_photos->cnt;
  g_photos_data.photo_total = p_photos->photo_total;

  if(g_photos_data.cnt == 0)
  {
    return;
  }

  g_photos_data.p_photo = 
    (photo_info_t *)mtos_malloc(g_photos_data.cnt * sizeof(photo_info_t));
  MT_ASSERT(g_photos_data.p_photo != NULL);
  memset(g_photos_data.p_photo, 0, g_photos_data.cnt * sizeof(photo_info_t));

  for(i = 0; i < g_photos_data.cnt; i++)
  {
    g_photos_data.p_photo[i].height = p_photos->p_photo[i].height;
    g_photos_data.p_photo[i].size = p_photos->p_photo[i].size;
    g_photos_data.p_photo[i].thumb_num = p_photos->p_photo[i].thumb_num;
    //g_photos_data.p_photo[i].timestamp = p_photos->p_photo[i].timestamp;
    g_photos_data.p_photo[i].width = p_photos->p_photo[i].width;

    memcpy(&g_photos_data.p_photo[i].create_time,
      &p_photos->p_photo[i].create_time,
      sizeof(utc_time_t));

    if(p_photos->p_photo[i].p_photo_url != NULL)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_photo_url);

      g_photos_data.p_photo[i].p_photo_url = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_photo_url != NULL);

      memset(g_photos_data.p_photo[i].p_photo_url, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_photo_url, p_photos->p_photo[i].p_photo_url, str_len);
    }

    if(p_photos->p_photo[i].p_album_id != NULL)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_album_id);

      g_photos_data.p_photo[i].p_album_id = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_album_id != NULL);

      memset(g_photos_data.p_photo[i].p_album_id, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_album_id, p_photos->p_photo[i].p_album_id, str_len);
    }

    if(p_photos->p_photo[i].p_user_id != NULL)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_user_id);

      g_photos_data.p_photo[i].p_user_id = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_user_id != NULL);

      memset(g_photos_data.p_photo[i].p_user_id, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_user_id, p_photos->p_photo[i].p_user_id, str_len);
    }

    #if 0
    if(p_photos->p_photo[i].p_user_name != NULL)
    {
      str_len = strlen(p_photos->p_photo[i].p_user_name);

      g_photos_data.p_photo[i].p_user_name = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_user_name != NULL);

      memset(g_photos_data.p_photo[i].p_user_name, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_user_name, p_photos->p_photo[i].p_user_name, str_len);
    }   
    #endif

    if(p_photos->p_photo[i].p_photo_id != NULL)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_photo_id);

      g_photos_data.p_photo[i].p_photo_id = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_photo_id != NULL);

      memset(g_photos_data.p_photo[i].p_photo_id, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_photo_id, p_photos->p_photo[i].p_photo_id, str_len);
    }

    if(p_photos->p_photo[i].p_photo_name)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_photo_name);

      g_photos_data.p_photo[i].p_photo_name = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_photo_name != NULL);

      memset(g_photos_data.p_photo[i].p_photo_name, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_photo_name, p_photos->p_photo[i].p_photo_name, str_len);
    }

    if(p_photos->p_photo[i].p_photo_description)
    {
      str_len = (u16)strlen(p_photos->p_photo[i].p_photo_description);

      g_photos_data.p_photo[i].p_photo_description = (char *)mtos_malloc(str_len + 1);

      MT_ASSERT(g_photos_data.p_photo[i].p_photo_description != NULL);

      memset(g_photos_data.p_photo[i].p_photo_description, 0, str_len + 1);
      
      memcpy(g_photos_data.p_photo[i].p_photo_description,
        p_photos->p_photo[i].p_photo_description, str_len);
    }

    if(g_photos_data.p_photo[i].thumb_num != 0)
    {
      g_photos_data.p_photo[i].p_thumb =
        (thumb_t *)mtos_malloc(g_photos_data.p_photo[i].thumb_num * sizeof(thumb_t));

      MT_ASSERT(g_photos_data.p_photo[i].p_thumb != NULL);

      //save thumbnail data.
      for(j = 0; j < g_photos_data.p_photo[i].thumb_num; j++)
      {
        g_photos_data.p_photo[i].p_thumb[j].height = p_photos->p_photo[i].p_thumb[j].height;
        g_photos_data.p_photo[i].p_thumb[j].width = p_photos->p_photo[i].p_thumb[j].width;

        if(p_photos->p_photo[i].p_thumb[j].p_thumb_url != NULL)
        {
          str_len = (u16)strlen(p_photos->p_photo[i].p_thumb[j].p_thumb_url);

          g_photos_data.p_photo[i].p_thumb[j].p_thumb_url = (char *)mtos_malloc(str_len + 1);

          MT_ASSERT(g_photos_data.p_photo[i].p_thumb[j].p_thumb_url != NULL);

          memset(g_photos_data.p_photo[i].p_thumb[j].p_thumb_url, 0, str_len + 1);
          
          memcpy(g_photos_data.p_photo[i].p_thumb[j].p_thumb_url,
            p_photos->p_photo[i].p_thumb[j].p_thumb_url, str_len);
        }
      }
      
    }

    //sort thumbnail data by width.(pop sort)
    for(m = 0; m < g_photos_data.p_photo[i].thumb_num; m++)
    {
      for(n = 0; n < (g_photos_data.p_photo[i].thumb_num - 1); n++)
      {
        if(g_photos_data.p_photo[i].p_thumb[n].width > g_photos_data.p_photo[i].p_thumb[n].width)
        {
          tw = g_photos_data.p_photo[i].p_thumb[n].width;
          th = g_photos_data.p_photo[i].p_thumb[n].height;
          p_t = g_photos_data.p_photo[i].p_thumb[n].p_thumb_url;

          g_photos_data.p_photo[i].p_thumb[n].width = 
            g_photos_data.p_photo[i].p_thumb[n + 1].width;
          g_photos_data.p_photo[i].p_thumb[n].height = 
            g_photos_data.p_photo[i].p_thumb[n + 1].height;
          g_photos_data.p_photo[i].p_thumb[n].p_thumb_url = 
            g_photos_data.p_photo[i].p_thumb[n + 1].p_thumb_url;

          g_photos_data.p_photo[i].p_thumb[n + 1].width = tw;
          g_photos_data.p_photo[i].p_thumb[n + 1].height = th;
          g_photos_data.p_photo[i].p_thumb[n + 1].p_thumb_url = p_t;
        }
      }
    }
    
  }
}
 
static void ui_albums_call_back(albums_pub_evt_t event, void *p_data, u16 context)
{
  event_t evt = {0};
  
  switch(event)
  {
    case ALBUMS_EVT_ALBUMS_LIST:
      if(context == albums_context)
      {
        if(p_data== NULL)
        {
          //got error. should notify user
		      evt.id = ALBUMS_API_EVT_FAILED;
          evt.data1 = ALBUMS_EVT_ALBUMS_LIST;
          ui_albums_free_albums();
          ap_frm_send_evt_to_ui(APP_ALBUM, &evt);
        }
        else
        {
          //nofigy ui got a category
          
		      evt.id = ALBUMS_API_EVT_SUCCESS;
          evt.data1 = ALBUMS_EVT_ALBUMS_LIST;
          evt.data2 = (u32)(&g_albums_data);

          ui_albums_save_albums((albums_t *)p_data);
          g_albums_data.context = context;

          ap_frm_send_evt_to_ui(APP_ALBUM, &evt); 
        }
      }      
      break;
      
    case ALBUMS_EVT_PHOTOS_LIST:
      if(context == albums_context)
      {
        if(p_data== NULL)
        {
          //got error. should notify user
		      evt.id = ALBUMS_API_EVT_FAILED;
          evt.data1 = ALBUMS_EVT_PHOTOS_LIST;
          ui_albums_free_photos();
          ap_frm_send_evt_to_ui(APP_VOD, &evt);
        }
        else
        {
          //nofigy ui got a category
		      evt.id = ALBUMS_API_EVT_SUCCESS;
          evt.data1 = ALBUMS_EVT_PHOTOS_LIST;
          evt.data2 = (u32)(&g_photos_data);

          ui_albums_save_photos((photos_t *)p_data);
          g_photos_data.context = context;

          ap_frm_send_evt_to_ui(APP_ALBUM, &evt); 
        }
      }
      break;

    case ALBUMS_EVT_TAGS_LIST:
      break;

    case ALBUMS_EVT_COMMENTS_LIST:
      break;
      
    default:
      break;
  }
}


void ui_albums_init(albums_site_t site)
{
  mul_albums_attr_t attr = {0};
  RET_CODE ret = SUCCESS;

  attr.site = site;
  attr.cb = ui_albums_call_back;
  attr.stk_size = ALBUMS_CHAIN_TASK_STK_SIZE;
  
  ret = mul_albums_chn_create(&attr);
  if(ret != SUCCESS)
  {
    MT_ASSERT(0);
  }
  mul_albums_start();

  fw_register_ap_evtmap(APP_ALBUM, ui_albums_evtmap);
  fw_register_ap_msghost(APP_ALBUM, ROOT_ID_ALBUMS); 
  fw_register_ap_msghost(APP_ALBUM, ROOT_ID_PHOTOS); 
  fw_register_ap_msghost(APP_ALBUM, ROOT_ID_PHOTO_SHOW); 
}

void ui_albums_release(void)
{
  RET_CODE ret = SUCCESS;
  
  mul_albums_stop();

  ret = mul_albums_chn_destroy(0);
  if(ret != SUCCESS)
  {
    MT_ASSERT(0);
  }

  ui_albums_free_albums();
  ui_albums_free_photos();

  fw_unregister_ap_evtmap(APP_ALBUM);
  fw_unregister_ap_msghost(APP_ALBUM, ROOT_ID_ALBUMS); 
  fw_unregister_ap_msghost(APP_ALBUM, ROOT_ID_PHOTOS);   
  fw_unregister_ap_msghost(APP_ALBUM, ROOT_ID_PHOTO_SHOW);   
}


RET_CODE ui_albums_get_albums(albums_req_albums_t *p_albums)
{
  if(p_albums == NULL)
  {
    return ERR_FAILURE;
  }

  albums_context++;
  (void)mul_albums_cancel();
  return mul_albums_get_albums(p_albums, albums_context);
}

RET_CODE ui_albums_get_photos(albums_req_photos_t *p_photos)
{
  if(p_photos == NULL)
  {
    return ERR_FAILURE;
  }

  albums_context++;
  (void)mul_albums_cancel();
  return mul_albums_get_photos(p_photos, albums_context);
}

RET_CODE ui_albums_get_tags(albums_req_tags_t *p_tags)
{
  if(p_tags == NULL)
  {
    return ERR_FAILURE;
  }

  albums_context++;
  (void)mul_albums_cancel();
  return mul_albums_get_tags(p_tags, albums_context);
}

RET_CODE ui_albums_get_comments(albums_req_comments_t *p_comments)
{
  if(p_comments == NULL)
  {
    return ERR_FAILURE;
  }

  albums_context++;
  (void)mul_albums_cancel();
  return mul_albums_get_comments(p_comments, albums_context);
}

photos_t *ui_albums_get_photos_list(void)
{
  return &g_photos_data;
}

albums_t *ui_albums_get_albums_list(void)
{
  return &g_albums_data;
}

s32 ui_albums_context_cmp(u16 context)
{
    return (s32)albums_context - context;
}

BEGIN_AP_EVTMAP(ui_albums_evtmap)
  CONVERT_EVENT(ALBUMS_API_EVT_FAILED, MSG_ALBUMS_EVT_FAILED)
  CONVERT_EVENT(ALBUMS_API_EVT_SUCCESS, MSG_ALBUMS_EVT_SUCCESS)
END_AP_EVTMAP(ui_albums_evtmap)

