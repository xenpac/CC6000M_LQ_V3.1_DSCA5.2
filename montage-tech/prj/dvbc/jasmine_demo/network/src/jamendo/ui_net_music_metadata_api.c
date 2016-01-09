/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"


#include "net_music_filter.h"
#include "net_music_api.h"
#include "ap_framework.h"
#include "ui_network_music.h"
#include "ui_net_music_metadata_api.h"
#include "music_api_net.h"

typedef enum
{
    NETWORK_MUSIC_API_EVT_FAILED = ((APP_NET_MUSIC_METADATA << 16) + 0),
    NETWORK_MUSIC_API_EVT_SUCCESS,
}net_music_metadata_evt_t;

static u16 g_nm_context = 0;
static net_music_tracks_content_t g_track_content;
static net_music_albums_content_t g_album_content;
static net_music_artists_content_t g_artist_content;




u16 ui_net_music_metadata_evtmap(u32 event);

void ui_save_net_music_tracks_data(net_music_tracks_content_t *ptrack )
{

    //reset old data
    memset(&g_track_content,0,sizeof(net_music_tracks_content_t));

    memcpy(&g_track_content,ptrack,sizeof(net_music_tracks_content_t));

    return;
}

void ui_save_net_music_ablum_data(net_music_albums_content_t *palbum)
{
    memset(&g_album_content,0,sizeof(net_music_albums_content_t));

    memcpy(&g_album_content,palbum,sizeof(net_music_albums_content_t));

    return;
}

void ui_save_net_music_artist_data(net_music_artists_content_t *partist)
{
    memset(&g_artist_content,0,sizeof(net_music_artists_content_t));

    memcpy(&g_artist_content,partist,sizeof(net_music_artists_content_t));

    return;
}

static void ui_net_music_metadata_callback(net_music_filter_evt_t event, void *p_data, u32 request_type, u32 context)
{
    event_t evt = {0};
    net_music_data_base_t *p_data_base = (net_music_data_base_t*)p_data;

    switch(request_type)
    {
    case TYPE_TRACKS://track
        if(context == g_nm_context)
        {
            if(NET_MUSIC_MSG_PARSE_ERROR == event)
            {
                //got error. should notify user
                evt.id = NETWORK_MUSIC_API_EVT_FAILED;
                evt.data1 = TYPE_TRACKS;

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt);
                OS_PRINTF("network music:request track info,NET_MUSIC_MSG_PARSE_ERROR..\n");
            }
            else
            {
                //nofigy ui got a category
                evt.id = NETWORK_MUSIC_API_EVT_SUCCESS;
                evt.data1 = TYPE_TRACKS;
                evt.data2 = (u32)(&g_track_content);

                ui_save_net_music_tracks_data(&p_data_base->tracks_content);

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt); 
                OS_PRINTF("network music:request track info,NETWORK_MUSIC_API_EVT_SUCCESS..\n");
            }
        }      
        break;
    case TYPE_ALBUMS://ablum
        if(context == g_nm_context)
        {
            if(NET_MUSIC_MSG_PARSE_ERROR == event)
            {
                //got error. should notify user
                evt.id = NETWORK_MUSIC_API_EVT_FAILED;
                evt.data1 = TYPE_ALBUMS;

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt);
            }
            else
            {
                //nofigy ui got a category
                evt.id = NETWORK_MUSIC_API_EVT_SUCCESS;
                evt.data1 = TYPE_ALBUMS;
                evt.data2 = (u32)(&g_album_content);

                ui_save_net_music_ablum_data(&p_data_base->albums_content);

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt); 
            }
        }      
        break;

    case TYPE_ARTISTS://artist
        if(context == g_nm_context)
        {
            if(NET_MUSIC_MSG_PARSE_ERROR == event)
            {
                //got error. should notify user
                evt.id = NETWORK_MUSIC_API_EVT_FAILED;
                evt.data1 = TYPE_ARTISTS;

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt);
            }
            else
            {
                //nofigy ui got a category
                evt.id = NETWORK_MUSIC_API_EVT_SUCCESS;
                evt.data1 = TYPE_ARTISTS;
                evt.data2 = (u32)(&g_artist_content);

                ui_save_net_music_artist_data(&p_data_base->artists_content);

                ap_frm_send_evt_to_ui(APP_NET_MUSIC_METADATA, &evt); 
            }
        }      
        break;
    default:
        break;

    }
   // mul_net_music_stop();
    return;
}

RET_CODE ui_net_music_metadata_init(void)
{
    RET_CODE ret = ERR_FAILURE;
    mul_net_music_attr_t p_attr = {0};
    p_attr.stk_size = 16 * KBYTES;
    p_attr.cb = ui_net_music_metadata_callback;

    mul_music_net_init();
    //MT_ASSERT(SUCCESS == ret);

    ret = mul_net_music_create_chn(&p_attr);
    MT_ASSERT(SUCCESS == ret);

    ret = mul_net_music_register_event(ui_net_music_metadata_callback, NULL);
    MT_ASSERT(SUCCESS == ret);
    ret = mul_net_music_start();
    MT_ASSERT(SUCCESS == ret);

    fw_register_ap_evtmap(APP_NET_MUSIC_METADATA, ui_net_music_metadata_evtmap);
    fw_register_ap_msghost(APP_NET_MUSIC_METADATA, ROOT_ID_NETWORK_MUSIC); 

    if(SUCCESS == ret)
    {

    }
	
    return SUCCESS;   
}

RET_CODE ui_net_music_metadata_release(void)
{
    RET_CODE ret = ERR_FAILURE;
    ret = mul_net_music_stop();
    MT_ASSERT(SUCCESS == ret);
    ret = mul_net_music_destroy_chn();
    MT_ASSERT(SUCCESS == ret);
    mul_music_net_deinit();
    //MT_ASSERT(SUCCESS == ret);
    
    fw_unregister_ap_evtmap(APP_NET_MUSIC_METADATA);
    fw_unregister_ap_msghost(APP_NET_MUSIC_METADATA,ROOT_ID_NETWORK_MUSIC);
    if(SUCCESS == ret)
    {

    }
	
    return SUCCESS; 
}

RET_CODE ui_net_music_load_metadata(net_music_param_t *param)
{ 
    RET_CODE ret = ERR_FAILURE;
    
    MT_ASSERT(param != NULL);
    g_nm_context++;

    //ret = mul_net_music_start();
   // MT_ASSERT(SUCCESS == ret);
    ret = mul_net_music_get(param, g_nm_context);
    if(SUCCESS == ret)
    {

    }
	
    return ret;
}


net_music_tracks_content_t *ui_net_music_get_track_list(void)
{
    return &g_track_content;
}

net_music_albums_content_t *ui_net_music_get_album_list(void)
{
    return &g_album_content;
}

net_music_artists_content_t *ui_net_music_get_artist_list(void)
{
    return &g_artist_content;
}



BEGIN_AP_EVTMAP(ui_net_music_metadata_evtmap)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_FAILED, MSG_NET_MUSIC_API_EVT_FAILED)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_SUCCESS, MSG_NET_MUSIC_API_EVT_SUCCESS)
END_AP_EVTMAP(ui_net_music_metadata_evtmap)
