/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
//play control api
#include "ui_common.h"
#include "str_filter.h"
#include "music_api.h"

#include "ap_framework.h"
#include "ui_net_music_play_ctrl_api.h"

typedef enum
{
    NETWORK_MUSIC_API_EVT_PLAY_END = ((APP_NET_MUSIC_PLAY_CTRL<< 16) + 0),
    NETWORK_MUSIC_API_EVT_CANNOT_PLAY,
    NETWORK_MUSIC_API_EVT_GET_PLAY_TIME,
    NETWORK_MUSIC_API_EVT_STOPPED,
    NETWORK_MUSIC_API_EVT_DATA_ERROR,
}net_music_play_ctrl_evt_t;



u32 g_handle_nmusic_play = 0;



u16 ui_net_music_play_ctrl_evtmap(u32 event);

void ui_music_paly_ctrl_call_back(u32 pic_handle, u32 content, u32 para1, u32 para2)
{
    music_api_evt_t event = (music_api_evt_t)content;
    event_t evt = {0};

    switch (event)
    {
    case MUSIC_API_EVT_PLAY_END:
        {
            OS_PRINTF("MUSIC_API_EVT_PLAY_END\n");

            evt.id = NETWORK_MUSIC_API_EVT_PLAY_END;
            evt.data1 = 0;
            evt.data2 = 0;


            ap_frm_send_evt_to_ui(APP_NET_MUSIC_PLAY_CTRL, &evt); 
            
        }
        break;
    case MUSIC_API_EVT_CANNOT_PLAY:
        {
            OS_PRINTF("MUSIC_API_EVT_CANNOT_PLAY\n");
	        evt.id = NETWORK_MUSIC_API_EVT_CANNOT_PLAY;
	        evt.data1 = 0;
            evt.data2 = 0;


            ap_frm_send_evt_to_ui(APP_NET_MUSIC_PLAY_CTRL, &evt); 
        }
        break;
    case MUSIC_API_EVT_GET_PLAY_TIME:
        {
            //u32 time = para1;
            OS_PRINTF("MUSIC_API_EVT_GET_PLAY_TIME %d \n",para1);
            evt.id = NETWORK_MUSIC_API_EVT_GET_PLAY_TIME;
            evt.data1 = para1;
            evt.data2 = 0;


            ap_frm_send_evt_to_ui(APP_NET_MUSIC_PLAY_CTRL, &evt);
        }
        break;
    case MUSIC_API_EVT_STOPPED:
        {
            OS_PRINTF("MUSIC_API_EVT_PLAY_END\n");
	        evt.id = NETWORK_MUSIC_API_EVT_STOPPED;
	        evt.data1 = 0;
            evt.data2 = 0;
        }
        break;
    case MUSIC_API_EVT_DATA_ERROR:
      {
            OS_PRINTF("MUSIC_API_EVT_DATA_ERROR\n");
            evt.id = NETWORK_MUSIC_API_EVT_DATA_ERROR;
            evt.data1 = 0;
            evt.data2 = 0;
            ap_frm_send_evt_to_ui(APP_NET_MUSIC_PLAY_CTRL, &evt); 
      }
      break;
    default:
        break;
    }

    return;

}

RET_CODE ui_net_music_play_ctrl_init(void)
{
    RET_CODE ret = SUCCESS;
    music_chain_t music_chain = {0};

    music_chain.task_stk_size = 16*1024;
    music_chain.rsc_get_glyph = NULL;
    ret = mul_music_create_net_chain(&g_handle_nmusic_play,&music_chain);
    MT_ASSERT(SUCCESS == ret);

    ret = music_regist_callback(g_handle_nmusic_play,ui_music_paly_ctrl_call_back);
    MT_ASSERT(SUCCESS == ret);

    fw_register_ap_evtmap(APP_NET_MUSIC_PLAY_CTRL, ui_net_music_play_ctrl_evtmap);
    fw_register_ap_msghost(APP_NET_MUSIC_PLAY_CTRL, ROOT_ID_NETWORK_MUSIC);
    return ret;
}

RET_CODE ui_net_music_play_ctrl_release(void)
{
    RET_CODE ret = 0;
    ret = music_stop(g_handle_nmusic_play);
    MT_ASSERT(SUCCESS == ret);

    ret = music_destroy_chain(g_handle_nmusic_play);
    MT_ASSERT(SUCCESS == ret);

    fw_unregister_ap_evtmap(APP_NET_MUSIC_PLAY_CTRL);
    fw_unregister_ap_msghost(APP_NET_MUSIC_PLAY_CTRL,ROOT_ID_NETWORK_MUSIC);
    return ret;

}

RET_CODE ui_net_music_play_ctrl_set_url(u8* p_url)
{
    RET_CODE ret = 0;
	OS_PRINTF("ui_net_music_play_ctrl_set_url \n");
    ret = mul_music_set_url(g_handle_nmusic_play,p_url);
    return ret;
}

RET_CODE ui_net_music_play_ctrl_start(void)
{
    RET_CODE ret = 0;
	OS_PRINTF("ui_net_music_play_ctrl_start \n");
    ret = music_start(g_handle_nmusic_play);
    return ret;
}

RET_CODE ui_net_music_play_ctrl_stop(void)
{
    RET_CODE ret = 0;
	OS_PRINTF("ui_net_music_play_ctrl_stop \n");
    ret = music_stop(g_handle_nmusic_play);
    return ret;
}

RET_CODE ui_net_music_play_ctrl_pause(void)
{
    RET_CODE ret = 0;
    ret = music_pause(g_handle_nmusic_play);
    return ret;
}

RET_CODE ui_net_music_play_ctrl_resume(void)
{
    RET_CODE ret = 0;
    ret = music_resume(g_handle_nmusic_play);
    return ret;
}



BEGIN_AP_EVTMAP(ui_net_music_play_ctrl_evtmap)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_PLAY_END, MSG_NET_MUSIC_PLAY_END)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_CANNOT_PLAY, MSG_NET_MUSIC_CANNOT_PLAY)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_GET_PLAY_TIME, MSG_NET_MUSIC_GET_PLAY_TIME)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_STOPPED, MSG_NET_MUSIC_STOPPED)
CONVERT_EVENT(NETWORK_MUSIC_API_EVT_DATA_ERROR, MSG_NET_MUSIC_DATA_ERROR)
END_AP_EVTMAP(ui_net_music_play_ctrl_evtmap)
