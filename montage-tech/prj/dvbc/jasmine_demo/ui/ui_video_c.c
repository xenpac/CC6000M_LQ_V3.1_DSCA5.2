/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#ifdef ENABLE_FILE_PLAY
#include "file_play_api.h"
#ifdef ENABLE_NETWORK
#include "ui_onmov_provider.h"
#endif

#include "ui_video.h"
#include "ui_mute.h"
#include "lib_char.h"
#include "sys_status.h"
#include "common_filter.h"

/*!
 * Struct for ui video module
 */
typedef struct
{
    u16 url_cnt;
    u8  **pp_url_list;
} ui_net_url_t;

typedef union 
{
    ui_net_url_t net_url;
    u16 *p_file_name;
} ui_vdo_url_t;

typedef struct
{
    u8  play_func;
    u32 seek_play_time;
    ui_vdo_url_t vdo_url;
#ifdef ENABLE_NETWORK
    ui_play_mode_t play_mode;
#endif
} ui_vdo_waiting_context_t;

typedef struct
{
    /*!
     * Handle for file play api
     */
    u32                 m_handle;
    /*!
     * Handle for file play api subtitle
     */
    u32                 m_sub_handle;
    /*!
     * Handle for file play api subtitle
     */
    void                 *p_sub_rgn;

    u16                  p_file_name[MAX_FILE_PATH];
    /*!
     * Speed of current play
     */
    mul_fp_play_speed_t m_speed;
    /*!
     * save the play time for continue play second
     */
    u32                 play_time_s;

    u32                 seek_play_time;
    /*!
     * User stop, file will be checked to play continue
     */
    BOOL                usr_stop;

    /*!
     * Init variable
     */
    BOOL                inited;

    //BOOL                b_seek_play;
    /*!
     * Audio track array
     */
    mul_fp_audio_track_t *p_audio_track_arr;

    /*!
     * Current playing insert sub id, if user selects same on, do nothing. 
     */
    u8                   playing_insert_sub_id;

    ui_vdo_waiting_context_t waiting_context;

}ui_fp_c_priv;

/*!
 * Globle struct variable
 */
static ui_fp_c_priv g_fp_c_priv = {0};

//lint -e607
#define printf_ret(ret)   if(SUCCESS != ret)\
                          {\
                              UI_VIDEO_PRINF("[%s]: ret = %d in %d line\n", \
                                                __FUNCTION__, ret, __LINE__);\
                          }

//lint +e607

//#define MAX_COLOR_PALETTE  (8)
//static u32 default_video_subt[MAX_COLOR_PALETTE] = 
//{
//   0x0, //alpha
//   0xFFFFFFFF, //white
//   0xFF0000FF, //red
//   0x000000FF, //black
//   0x00FF00FF, //green
//   0x0000FFFF, //blue
//   0x808080FF, //gray
//};

typedef enum
{
    API_VIDEO_EVENT_EOS = ((APP_FILEPLAY << 16) + 0),
    API_VIDEO_EVENT_UP_TIME,
    API_VIDEO_EVENT_RESOLUTION,
    API_VIDEO_EVENT_UNSUPPORTED_VIDEO,
    API_VIDEO_EVENT_TRICK_TO_BEGIN,
    API_VIDEO_EVENT_UNSUPPORTED_MEMORY,
    API_VIDEO_EVENT_UNSUPPORT_SEEK,
    API_VIDEO_EVENT_LOAD_MEDIA_EXIT,
    API_VIDEO_EVENT_LOAD_MEDIA_ERROR,
    API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS,
    API_VIDEO_EVENT_STOP_CFM, 
    API_VIDEO_EVENT_UPDATE_BPS,
    API_VIDEO_EVENT_FINISH_BUFFERING,
    API_VIDEO_EVENT_REQUEST_CHANGE_SRC,
    API_VIDEO_EVENT_SET_PATH_FAIL,
    API_VIDEO_EVENT_LOAD_MEDIA_TIME,
    API_VIDEO_EVENT_FINISH_UPDATE_BPS,
    API_VIDEO_EVENT_MAX,
}fplay_api_evt_t;


u16 ui_file_play_evtmap(u32 event);
RET_CODE ui_file_play_evtproc(u32 event, u32 para1, u32 para2);
static RET_CODE _ui_video_c_start(void);

static int video_play_pause_resume = 0;
extern u32 g_video_fw_cfg_addr;
extern u32 g_video_fw_cfg_size;
extern u32 g_audio_fw_cfg_size;
extern u32 g_audio_fw_cfg_addr;
extern u32 g_video_file_play_addr;                       //fileplay addr   and  pvr play addr
extern u32 g_video_file_play_size;                       //for fileplay size = PVR rec size + PVR play size

/*=================================================================================================
                        Video controller internel function
=================================================================================================*/
void ui_video_c_play_end(void)
{
    ui_video_c_reset_speed();
    ui_video_c_sub_off();
    ui_video_c_stop();
}


static RET_CODE ui_fileplay_callback(fp_handle_t fp_handle, mul_fp_play_event_t content, u32 para)
{
  event_t evt = {0};

  OS_PRINTF("##%s, content == %d##\n", __FUNCTION__, content);
  switch(content)
  {
    case MUL_PLAY_EVENT_EOS:
    {
        evt.id = API_VIDEO_EVENT_EOS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UP_TIME:
    {
        evt.id = API_VIDEO_EVENT_UP_TIME;
        evt.data1 = para;
        g_fp_c_priv.play_time_s = para / 1000;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UP_RESOLUTION:
    {
        evt.id = API_VIDEO_EVENT_RESOLUTION;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UNSUPPORT_MEMORY:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORTED_MEMORY;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UNSUPPORTED_VIDEO:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORTED_VIDEO;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_TRICK_TO_BEGIN:
    {
        evt.id = API_VIDEO_EVENT_TRICK_TO_BEGIN;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
   case MUL_PLAY_EVENT_UNSUPPORT_SEEK:
    {
        evt.id = API_VIDEO_EVENT_UNSUPPORT_SEEK;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_EXIT:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_EXIT;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_ERROR:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_ERROR;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_FILE_ES_TASK_EXIT:
    {
        evt.id = API_VIDEO_EVENT_STOP_CFM;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_UPDATE_BPS:
    {
        evt.id = API_VIDEO_EVENT_UPDATE_BPS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_FINISH_BUFFERING:
    {
        evt.id = API_VIDEO_EVENT_FINISH_BUFFERING;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }    
    case MUL_PLAY_EVENT_REQUEST_CHANGE_SRC:
    {
        evt.id = API_VIDEO_EVENT_REQUEST_CHANGE_SRC;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_SET_PATH_FAIL:
    {
        evt.id = API_VIDEO_EVENT_SET_PATH_FAIL;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    case MUL_PLAY_EVENT_LOAD_MEDIA_TIME:
    {
        evt.id = API_VIDEO_EVENT_LOAD_MEDIA_TIME;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }
    /*case MUL_PLAY_EVENT_FINISH_UPDATE_BPS:
    {
        evt.id = API_VIDEO_EVENT_FINISH_UPDATE_BPS;
        evt.data1 = para;
        ap_frm_send_evt_to_ui(APP_FILEPLAY, &evt);
        break;
    }*/
    default:
    {
        break;
    }
  }
  return SUCCESS;

}

static RET_CODE _ui_video_c_register_evt(fn_fp_event_cb cb)
{
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UP_TIME, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_EOS, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UP_RESOLUTION, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UNSUPPORTED_VIDEO, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UNSUPPORT_MEMORY, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_TRICK_TO_BEGIN, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UNSUPPORT_SEEK, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_LOAD_MEDIA_EXIT, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_LOAD_MEDIA_ERROR, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_LOAD_MEDIA_SUCCESS, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_FILE_ES_TASK_EXIT, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_UPDATE_BPS, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_REQUEST_CHANGE_SRC, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_SET_PATH_FAIL, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_LOAD_MEDIA_TIME, cb);
    mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_FINISH_BUFFERING, cb);
    //mul_file_play_register_evt(g_fp_c_priv.m_handle, MUL_PLAY_EVENT_FINISH_UPDATE_BPS, cb);
    return SUCCESS;
}

static void _ui_video_c_uni2utf8(u16 *in, u8 *out, u16 out_size)
{
    u32 i   = 0;
    u32 cnt = 0;

    cnt = uni_strlen(in);

    MT_ASSERT(out_size >= (3 * cnt + 1));

    for (i = 0; i < uni_strlen(in); i++)
    {
        u16 unicode = in[i];

        if (unicode <= 0x007f)
        {
            *out = (u8)unicode;
            out += 1;
        }
        else if (unicode >= 0x0080 && unicode <= 0x07ff)
        {
            *out = 0xc0 | (u8)(unicode >> 6);
            out += 1;
            *out = 0x80 | (unicode & 0x003f);
            out += 1;
        }
        else if (unicode >= 0x0800)
        {
            *out = 0xe0 | (unicode >> 12);
            out += 1;
            *out = 0x80 | (unicode >> 6 & 0x003f);
            out += 1;
            *out = 0x80 | (unicode & 0x003f);
            out += 1;
        }

    }

    *out = '\0';
}

//lint -e438 
static void _ui_video_c_utf82uni(u8 *in, u16 *out, u16 out_size)
{
    u16 index = 0;
    //MT_ASSERT(out_size >= strlen(in));
    
    while(*in && index <= out_size)
    {
        if (*in <= 0x7f)
        {
            *out = (u16) *in;
        }
        else if ((*in & (0xff << 5)) == 0xc0)
        {
            u16 unicode = 0;

            unicode = (*in & 0x1F) << 6; 
            in++;
            
            unicode |= (*in)  & 0x3F; 

            *out = unicode;
        }
        else if ((*in & (0xff << 4))== 0xe0)
        {
            u16 unicode = 0;

            unicode = (u16)(((*in) & 0x1F) << 12); 
            in++;

            unicode |= ((*in) & 0x3F) << 6; 
            in++;

            unicode |= ((*in) & 0x3F); 

            *out = unicode;
        }

        in++;
        out++;
        index ++;
    }

    out = 0;
}
//lint +e438 

void _ui_video_c_dump_utf8(u8 *utf8)
{
    u8 *p = utf8;

    UI_VIDEO_PRINF("[%s]:begin\n UTF8:", __FUNCTION__);
    while(*p)
    {
        UI_VIDEO_PRINF("[%02X]", *p);
        p++;
    }
    
    UI_VIDEO_PRINF("\n");
}

void _ui_video_c_dump_unicode(u16 *p_uni)
{
    u8 *p = (u8 *)p_uni;
    u32 i = 0;

    UI_VIDEO_PRINF("[%s]:begin len:%d \n UNI:", __FUNCTION__, uni_strlen(p_uni));
    for (i = 0; i < uni_strlen(p_uni); i++)
    {
        UI_VIDEO_PRINF("[%02X]", *p);
        p++;
    }
    
    UI_VIDEO_PRINF("\n");
}
/*=================================================================================================
                        Video controller interface function
=================================================================================================*/
void ui_video_c_reset_speed(void)
{
    g_fp_c_priv.m_speed = MUL_PLAY_NORMAL_PLAY;
}

void ui_video_c_enable_tv_mode_change(BOOL enable)
{
    if(0 != g_fp_c_priv.m_handle)
    {
        mul_file_play_enable_tv_mode_change(g_fp_c_priv.m_handle, enable);
    }
}
/*!
 * Get current play time
 */
u32 ui_video_c_get_play_time(void)
{
    return g_fp_c_priv.play_time_s;
}

/*!
 * Get current play state
 */
mul_fp_play_state_t ui_video_c_get_play_state(void)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

    if(0 == g_fp_c_priv.m_handle)
    {
        UI_VIDEO_PRINF("[%s]: handle null\n", __FUNCTION__);
        return (mul_fp_play_state_t)ERR_FAILURE;
    }

    mul_file_play_get_play_state(g_fp_c_priv.m_handle, &state);

    UI_VIDEO_PRINF("[%s]: play status = %d\n", __FUNCTION__, state);

    return state;
}
void ui_video_c_play_normal(void)
{
    mul_file_play_set_speed(g_fp_c_priv.m_handle, MUL_PLAY_NORMAL_PLAY);
    ui_video_c_reset_speed();
}
/*!
 * Fast speed play video
 */
RET_CODE ui_video_c_fast_play(void)
{
    RET_CODE ret = SUCCESS;

    UI_VIDEO_PRINF("[%s]: speed = %d\n", __FUNCTION__, g_fp_c_priv.m_speed);

    /*!
     * Speed loop
     */
    if(MUL_PLAY_FORWORD_32X == g_fp_c_priv.m_speed)
    {
        g_fp_c_priv.m_speed = MUL_PLAY_FORWORD_2X;
    }
    /*!
     * First time set fast play or last time on fast back state, set as 2X
     */
    else if(g_fp_c_priv.m_speed <= 0)
    {
        g_fp_c_priv.m_speed = MUL_PLAY_FORWORD_2X;
    }
    /*!
     * Otherwise multiple 2
     */
    else
    {
        g_fp_c_priv.m_speed=  (mul_fp_play_speed_t)(g_fp_c_priv.m_speed * 2);
    }

    ret = mul_file_play_set_speed(g_fp_c_priv.m_handle, g_fp_c_priv.m_speed);

    printf_ret(ret);

    return ret;
}
/*!
 * Fast speed back play video
 */
RET_CODE ui_video_c_fast_back(void)
{
    RET_CODE ret = SUCCESS;

    UI_VIDEO_PRINF("[%s]: speed = %d\n", __FUNCTION__, g_fp_c_priv.m_speed);

    /*!
     * Speed loop
     */
    if(MUL_PLAY_BACKWORD_32X == g_fp_c_priv.m_speed)
    {
        g_fp_c_priv.m_speed = MUL_PLAY_BACKWORD_2X;
    }
    /*!
     * First time set fast play or last time on fast back state, set as 2X
     */
    else if(g_fp_c_priv.m_speed >= 0)
    {
        g_fp_c_priv.m_speed = MUL_PLAY_BACKWORD_2X;
    }
    /*!
     * Otherwise multiple 2
     */
    else
    {
        g_fp_c_priv.m_speed=  (mul_fp_play_speed_t)(g_fp_c_priv.m_speed * 2);
    }

    ret = mul_file_play_set_speed(g_fp_c_priv.m_handle, g_fp_c_priv.m_speed);

    printf_ret(ret);

    return ret;
}
void ui_video_c_switch_time(video_play_time_t *p_time, u32 sec)
{
    p_time->hour = (u8)(sec / 3600);
    p_time->min = (u8)((sec - p_time->hour * 3600) / 60);
    p_time->sec = (u8)(sec - p_time->hour * 3600 - p_time->min * 60);
}
/*!
 * Get total time of video
 */
u32 ui_video_c_get_total_time_by_sec(void)
{
    u32                 seconds     = 0;
    mul_fp_state_info_t status_info = {0};

    if(0 != g_fp_c_priv.m_handle)
    {
        mul_file_play_get_status_info(g_fp_c_priv.m_handle, &status_info);
    }

    UI_VIDEO_PRINF("[%s]: total time = %d s\n", __FUNCTION__, status_info.duration);

    seconds = status_info.duration;

    return seconds;
}
/*!
 * Get total time of video
 */
void ui_video_c_get_total_time(video_play_time_t *p_time)
{
    u32 seconds = 0;
    
    seconds = ui_video_c_get_total_time_by_sec();

    ui_video_c_switch_time(p_time, seconds);
}
/*!
 * Get resolution of video
 */
void ui_video_c_get_resolution(rect_size_t *p_rect)
{
    mul_fp_state_info_t status_info = {0};

    MT_ASSERT(NULL != p_rect);

    if(0 != g_fp_c_priv.m_handle)
    {
        mul_file_play_get_status_info(g_fp_c_priv.m_handle, &status_info);
    }
    p_rect->w = (u32)status_info.video_disp_w;
    p_rect->h = (u32)status_info.video_disp_h;

    UI_VIDEO_PRINF("[%s]: resolution %d x %d\n", __FUNCTION__, p_rect->w, p_rect->h);
}

/*!
 * Get play speed, if < 0; it is backword, > 0 forword, == 0 normal play
 */
int ui_video_c_get_play_speed(void)
{
    return (int)g_fp_c_priv.m_speed;
}
/*!
 * Get plug-in subtitle count
 */
u16 ui_video_c_get_insert_sub_cnt(void)
{
    mul_fp_video_subtitle_t *p_subt_info = NULL;

    mul_file_play_get_insert_subt_info(g_fp_c_priv.m_handle, &p_subt_info);

    UI_VIDEO_PRINF("[%s]: insert sub cnt = %d\n", __FUNCTION__, p_subt_info->cnt);

    return (u16)p_subt_info->cnt;
}

//lint -e613
int enc_utf8_to_unicode_one(u8* pInput, u16 *Unic)  
{  
    u8 b1, b2, b3, b4, b5, b6;  
    int utfbytes = 3;  
    unsigned char *pOutput = NULL;  

    MT_ASSERT(pInput != NULL && Unic != NULL);  
    
    *Unic = 0x0; //  
    pOutput = (unsigned char *) Unic;

    switch ( utfbytes )  
    {  
        case 0:  
            *pOutput     = *pInput;  
            utfbytes    += 1;  
            break;  
        case 2:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (u8)((b1 << 6) + (b2 & 0x3F));  
            *(pOutput+1) = (u8)((b1 >> 2) & 0x07);  
            break;  
        case 3:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (u8)((b2 << 6) + (b3 & 0x3F));  
            *(pOutput+1) = (u8)((b1 << 4) + ((b2 >> 2) & 0x0F));  
            break;  
        case 4:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (u8)((b3 << 6) + (b4 & 0x3F));  
            *(pOutput+1) = (u8)((b2 << 4) + ((b3 >> 2) & 0x0F));  
            *(pOutput+2) = (u8)(((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03));  
            break;  
        case 5:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (u8)((b4 << 6) + (b5 & 0x3F));  
            *(pOutput+1) = (u8)((b3 << 4) + ((b4 >> 2) & 0x0F));  
            *(pOutput+2) = (u8)((b2 << 2) + ((b3 >> 4) & 0x03));  
            *(pOutput+3) = (u8)((b1 << 6));  
            break;  
        case 6:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            b6 = *(pInput + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (u8)((b5 << 6) + (b6 & 0x3F));  
            *(pOutput+1) = (u8)((b5 << 4) + ((b6 >> 2) & 0x0F));  
            *(pOutput+2) = (u8)((b3 << 2) + ((b4 >> 4) & 0x03));  
            *(pOutput+3) = (u8)(((b1 << 6) & 0x40) + (b2 & 0x3F));  
            break;  
        default:  
            return 0;  
    }  
  
    return utfbytes;  
}  
//lint +e613

/*!
 * Get plug-in subtitle string
 */
void ui_video_c_get_insert_sub_by_idx(u16 idx, u16 *p_buf, u16 str_len)
{
    mul_fp_video_subtitle_t *p_subt_info = NULL;
    u32 i=0,j=0,utf8=0;
    u8 buf[MUL_FP_SUBTITLE_LEN];
    u32 len;

    mul_file_play_get_insert_subt_info(g_fp_c_priv.m_handle, &p_subt_info);

    UI_VIDEO_PRINF("[%s]: in sub[%d] = %s, code = %s\n", __FUNCTION__,
                                                         idx,
                                                         p_subt_info->subtitle[idx].title,
                                                         p_subt_info->subtitle[idx].code);
   
    len=strlen(p_subt_info->subtitle[idx].title);
    
    strcpy((char *)buf,p_subt_info->subtitle[idx].title);
    while(i<len)
    {
        if(buf[i] > 0xe0 &&( i+2 < len) &&(buf[i+1] >= 0x80)&&(buf[i+2] >= 0x80))
        {
            enc_utf8_to_unicode_one(&buf[i],p_buf+j);
            //OS_PRINTF("0x%x\n",p_buf[j]&0xffff);
            i+=2;
            j++;
	     p_buf[j]=0;
            utf8=1;
        }
        else
            i++;
    }
    if(!utf8)
        dvb_to_unicode((u8 *)p_subt_info->subtitle[idx].title, str_len - 1, p_buf, str_len);
}
/*!
 * set subtitle id
 */
RET_CODE ui_video_c_set_insert_sub_id_by_idx(int idx)
{
    RET_CODE ret = SUCCESS;

    ret = mul_file_play_set_subt_by_idx(g_fp_c_priv.m_handle, idx);

    printf_ret(ret);

    return ret;
}
/*!
 * Show subtitle
 */
RET_CODE ui_video_c_sub_on(void *p_attr)
{
    rect_size_t     rect_size = {720, 70};
    RET_CODE        ret       = 0;
    void           *p_disp    = NULL;
    void           *p_gpe     = NULL;
    point_t         pos       = {0};
    rect_t          rect      = {0, 0, 720, 70};
    mul_fp_subt_attr_t *p_att= (mul_fp_subt_attr_t *)p_attr;

    MT_ASSERT(NULL != p_attr);
    UI_VIDEO_PRINF("[%s] start\n", __FUNCTION__);

    p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);

    p_gpe = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
    MT_ASSERT(NULL != p_gpe);

    if(g_fp_c_priv.p_sub_rgn != NULL)
    {
      disp_layer_remove_region(p_disp, DISP_LAYER_ID_SUBTITL, g_fp_c_priv.p_sub_rgn);
      region_delete(g_fp_c_priv.p_sub_rgn);
      g_fp_c_priv.p_sub_rgn = NULL;
    }


    g_fp_c_priv.p_sub_rgn = region_create(&rect_size, p_att->char_fmt);
    MT_ASSERT(NULL != g_fp_c_priv.p_sub_rgn);

    pos.x = 250;
    pos.y = 600;
    ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL,
                                 g_fp_c_priv.p_sub_rgn, &pos, NULL);


    //region_set_palette(g_fp_c_priv.p_sub_rgn, (u32 *)default_video_subt, MAX_COLOR_PALETTE);

    
    gpe_draw_rectangle_vsb(p_gpe, g_fp_c_priv.p_sub_rgn, &rect, p_att->window_color);
    disp_layer_update_region(p_disp, g_fp_c_priv.p_sub_rgn, NULL);
    //ret = region_show(g_fp_c_priv.p_sub_rgn, TRUE);
    //MT_ASSERT(SUCCESS == ret);
    ret = disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);
    MT_ASSERT(SUCCESS == ret);

    ((mul_fp_subt_attr_t *)p_attr)->p_region = g_fp_c_priv.p_sub_rgn;

    return mul_file_play_subt_init((mul_fp_subt_attr_t *)p_attr, &g_fp_c_priv.m_sub_handle);
}
/*!
 * Set current playing subtitle id
 */
void ui_video_c_set_playing_insert_sub_id(u8 id)
{
    g_fp_c_priv.playing_insert_sub_id = id;
}
/*!
 * Get current playing subtitle id
 */
u8 ui_video_c_get_playing_insert_sub_id(void)
{
    return g_fp_c_priv.playing_insert_sub_id;
}
/*!
 * Show subtitle
 */
RET_CODE ui_video_c_sub_show(void)
{
    if(NULL == g_fp_c_priv.p_sub_rgn)
    {
        return ERR_FAILURE;
    }
        
    return region_show(g_fp_c_priv.p_sub_rgn, TRUE);
}

/*!
 * Hide subtitle
 */
RET_CODE ui_video_c_sub_hide(void)
{
    if(NULL == g_fp_c_priv.p_sub_rgn)
    {
        return ERR_FAILURE;
    }
    
    return region_show(g_fp_c_priv.p_sub_rgn, FALSE);
}

/*!
 * Hide subtitle
 */
RET_CODE ui_video_c_sub_off(void)
{
    void           *p_disp;

    UI_VIDEO_PRINF("[%s] sub_handle = 0x%x\n", __FUNCTION__, g_fp_c_priv.m_sub_handle);

    if(0 != g_fp_c_priv.m_sub_handle)
    {
        mul_file_play_subt_deinit(g_fp_c_priv.m_sub_handle);
        g_fp_c_priv.m_sub_handle = 0;
        
        if(g_fp_c_priv.p_sub_rgn != NULL)
        {
            p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
            MT_ASSERT(NULL != p_disp);

            disp_layer_remove_region(p_disp, DISP_LAYER_ID_SUBTITL, g_fp_c_priv.p_sub_rgn);
            region_delete(g_fp_c_priv.p_sub_rgn);

            g_fp_c_priv.p_sub_rgn = NULL;
        }
    }

    return SUCCESS;
}

RET_CODE ui_video_c_seek(u32 time_sec)
{
    RET_CODE            ret       = SUCCESS;
    mul_fp_state_info_t status_info = {0};

    if(0 != g_fp_c_priv.m_handle)
    {
        mul_file_play_get_status_info(g_fp_c_priv.m_handle, &status_info);
    }

    ui_video_c_get_play_state();

    //if(MUL_PLAY_STATE_PAUSE == cur_state)
    //{
        UI_VIDEO_PRINF("[%s] seek time: %d s total: %d s\n", 
                            __FUNCTION__, time_sec, status_info.duration);
        
        ret = mul_file_play_seek(g_fp_c_priv.m_handle, (s32)time_sec);
    //}

    printf_ret(ret);

    return ret;
}

/*!
 * Pause or resume playing
 */
RET_CODE ui_video_c_pause_resume(void)
{
    RET_CODE            ret       = SUCCESS;
    mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;

    cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    UI_VIDEO_PRINF("[%s]: cur_state = %d\n", __FUNCTION__, cur_state);

    switch(cur_state)
    {
        case MUL_PLAY_STATE_PAUSE:
        case MUL_PLAY_STATE_PREVIEW:
        {
            ret = mul_file_play_resume(g_fp_c_priv.m_handle, NULL);
            video_play_pause_resume = 0;
            break;
        }
        case MUL_PLAY_STATE_PLAY:
        {
            ret = mul_file_play_pause(g_fp_c_priv.m_handle, NULL);
            video_play_pause_resume = 1;
            break;
        }
        case MUL_PLAY_STATE_SPEED_PLAY:
        {
            if(video_play_pause_resume == 1)
            {
              ret = mul_file_play_resume(g_fp_c_priv.m_handle, NULL);
              video_play_pause_resume = 0;
            }
            else
              ui_video_c_play_normal();
            break;
        }
        default:
        {
            ret = ERR_PARAM;
            break;
        }
    }

    printf_ret(ret);

    return ret;
}
/*!
 * Set usr stop flag
 */
void ui_video_c_set_usr_stop_flag(BOOL flag)
{
  g_fp_c_priv.usr_stop = flag;
}

RET_CODE ui_video_c_clean_waiting_context(void)
{
    ui_vdo_waiting_context_t *p_next_context;
    u8  **pp_url_list;
    u16 *p_file_name;
    u16 url_cnt, i;

    p_next_context = &g_fp_c_priv.waiting_context;

    if (p_next_context->play_func == VIDEO_PLAY_FUNC_ONLINE)
    {
        pp_url_list = p_next_context->vdo_url.net_url.pp_url_list;
        if (pp_url_list != NULL)
        {
            url_cnt = p_next_context->vdo_url.net_url.url_cnt;
            for (i = 0; i < url_cnt; i++)
            {
                if (pp_url_list[i] != NULL)
                {
                    mtos_free(pp_url_list[i]);
                }
            }
            mtos_free(pp_url_list);
            memset(p_next_context, 0, sizeof(ui_vdo_waiting_context_t));
        }
    }
    else if (p_next_context->play_func == VIDEO_PLAY_FUNC_USB)
    {
        p_file_name = p_next_context->vdo_url.p_file_name;
        if (p_file_name != NULL)
        {
            mtos_free(p_file_name);
            memset(p_next_context, 0, sizeof(ui_vdo_waiting_context_t));
        }
    }

    return SUCCESS;
}

/*!
 * Stop playing
 */
RET_CODE ui_video_c_stop(void)
{
    RET_CODE            ret    = SUCCESS;
	mul_fp_play_state_t status = {MUL_PLAY_STATE_NONE, };

	video_play_pause_resume = 0;
    if(g_fp_c_priv.inited == FALSE)
      return SUCCESS;

    status = (mul_fp_play_state_t)ui_video_c_get_play_state();

    UI_VIDEO_PRINF("[%s]: play_state = %d\n", __FUNCTION__, status);

    ui_video_c_sub_off();

    /*!
     * Check play state, avoid stopping twice
     */
    if (MUL_PLAY_STATE_NONE != status && MUL_PLAY_STATE_STOP != status)
    {
        ret = mul_file_play_force_stop(g_fp_c_priv.m_handle, NULL);
    }

    ui_video_c_clean_waiting_context();

    printf_ret(ret);

    return ret;
}
/*!
 * Get total time of video
 */
int ui_video_c_get_audio_track_num(void)
{
    mul_fp_state_info_t status_info = {0};

    if(0 != g_fp_c_priv.m_handle)
    {
        mul_file_play_get_status_info(g_fp_c_priv.m_handle, &status_info);
    }

    UI_VIDEO_PRINF("[%s]: audio track num: %d\n", __FUNCTION__, status_info.audio_track_num);

    return status_info.audio_track_num;
}
/*!
 * Change audio track
 */
RET_CODE ui_video_c_change_audio_track(int idx)
{
    RET_CODE ret = SUCCESS;
    int      cnt = 0;

    cnt = ui_video_c_get_audio_track_num();
    
    if(idx >= cnt)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## idx[%d] is more than total number\n", __FUNCTION__, idx);
        return ERR_FAILURE;
    }

    if(cnt <= 1)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## cnt = %d\n", __FUNCTION__, cnt);
        return SUCCESS;
    }
    
    UI_VIDEO_PRINF("[%s]: idx: %d \n", __FUNCTION__, idx);

    ret = mul_file_play_change_audio_track(g_fp_c_priv.m_handle, idx);
    printf_ret(ret);

    return ret;
}
void ui_video_c_free_audio_track(void)
{
    if(NULL != g_fp_c_priv.p_audio_track_arr)
    {
        mtos_free(g_fp_c_priv.p_audio_track_arr);
        g_fp_c_priv.p_audio_track_arr = NULL;
    }
}
/*!
 * Get audio track language
 */
RET_CODE ui_video_c_load_all_audio_track(void)
{
    u32       audio_track_num = 0;
    u32       size            = 0;
    RET_CODE  ret             = SUCCESS;

    ui_video_c_free_audio_track();

    audio_track_num = (u32)ui_video_c_get_audio_track_num();

    /*!
     * 1 track can't be changed
     */
    if(audio_track_num <= 1)
    {
        return SUCCESS;
    }

    size = audio_track_num * sizeof(mul_fp_audio_track_t);

    g_fp_c_priv.p_audio_track_arr = (mul_fp_audio_track_t *)mtos_malloc(size);
    memset(g_fp_c_priv.p_audio_track_arr, 0, size);

    ret = mul_file_play_get_audio_track(g_fp_c_priv.m_handle, g_fp_c_priv.p_audio_track_arr);

    printf_ret(ret);

    return ret;
}
/*!
 * Get audio track language
 */
RET_CODE ui_video_c_get_audio_track_lang_by_idx(int idx, u16 *p_buf, u16 len)
{
    int   cnt         = 0;
    char *p_lang      = NULL;
    char *p_title     = NULL;
    char  asc_buf[40] = {0};
    
    cnt = ui_video_c_get_audio_track_num();

    memset(p_buf, 0, len);

    if(cnt <= 1)
    {
        sprintf((char *)asc_buf, "snd%d",  idx + 1);
        UI_VIDEO_PRINF("[%s]: asc:%s\n", __FUNCTION__, asc_buf);
        str_nasc2uni((u8 *)asc_buf, p_buf, len);
        return SUCCESS;
    }
    
    if(idx >= cnt)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## idx[%d] is more than total number\n", __FUNCTION__, idx);
        return ERR_FAILURE;
    }
    
    UI_VIDEO_PRINF("[%s]: idx = %d\n", __FUNCTION__, idx);

    MT_ASSERT(NULL != g_fp_c_priv.p_audio_track_arr);

    p_lang = g_fp_c_priv.p_audio_track_arr[idx].p_lang;
    p_title = g_fp_c_priv.p_audio_track_arr[idx].p_title; 

    /*!
     * language or title must be valided
     */
    if((NULL != p_lang) && (0 != strlen(p_lang)) && (NULL != p_title) && (0 != strlen(p_lang)))
    {
        sprintf(asc_buf, "%s-%s-%d",  p_title, p_lang, idx + 1);

        if(strlen(asc_buf) > 15)
        {
            if(strlen(p_title) > 5)
            {
                p_title[5] = '\0';
            }
            if(strlen(p_lang) > 5)
            {
                p_lang[5] = '\0';
            }
            sprintf(asc_buf, "%s-%s-%d",  p_title, p_lang, idx + 1);
        }
        
        _ui_video_c_utf82uni((u8 *)asc_buf, p_buf, len);
    } 
    else if((NULL != p_lang) && (0 != strlen(p_lang)))
    {
        sprintf(asc_buf, "%s-%d",  p_lang, idx + 1);

        if(strlen(asc_buf) > 15)
        {
            if(strlen(p_lang) > 10)
            {
                p_lang[10] = '\0';
            }
            sprintf(asc_buf, "%s-%d",  p_lang, idx + 1);
        }
        _ui_video_c_utf82uni((u8 *)asc_buf, p_buf, len);
    } 
    else if((NULL != p_title) && (0 != strlen(p_title)))
    {
        sprintf(asc_buf, "%s-%d",  p_title, idx + 1);
        if(strlen(asc_buf) > 15)
        {
            if(strlen(p_title) > 10)
            {
              p_title[10] = '\0';
            }
            sprintf(asc_buf, "%s-%d",  p_title, idx + 1);
        }
        _ui_video_c_utf82uni((u8 *)asc_buf, p_buf, len);
    } 
    else
    {
        sprintf(asc_buf, "snd%d",  idx + 1);
        str_nasc2uni((u8 *)asc_buf, p_buf, len);
    }

    return SUCCESS;
}
#if 0
/*!
 * When press play key in preview state, play video
 */
RET_CODE ui_video_c_load_media_from_local(u16 *p_file_name, BOOL seek_play)
{
    u8                 *p_asc_name = NULL;
    mul_fp_play_state_t play_state = MUL_PLAY_STATE_NONE;
    RET_CODE            ret        = SUCCESS;
    
    MT_ASSERT(NULL != p_file_name);

    UI_PRINTF("@@@ui_video_c_load_media_from_local()\n");

    /*!
     * If user player another file, but current file is on playing pausing or resuming,
     * stop it and then play another file.
     */
    play_state = ui_video_c_get_play_state();
    UI_VIDEO_PRINF("[%s]: play_state = %d\n", __FUNCTION__, play_state);
    if((MUL_PLAY_STATE_NONE != play_state) && (MUL_PLAY_STATE_STOP != play_state))
    {
        ui_video_c_stop();
        return ERR_NOFEATURE;
    }

    /*!
     * Otherwise replay the new file
     */
    p_asc_name = (u8*)mtos_malloc(3*MAX_FILE_PATH + 1);
    MT_ASSERT(p_asc_name != NULL);

    memset(p_asc_name, 0, 3*MAX_FILE_PATH + 1);
    _ui_video_c_uni2utf8(p_file_name, p_asc_name, (3*MAX_FILE_PATH + 1));
    //UI_VIDEO_PRINF("@@@file path=%s\n", p_asc_name);
    ret = mul_file_play_load_media(g_fp_c_priv.m_handle, p_asc_name);
    if (ret == SUCCESS)
    {
        g_fp_c_priv.p_file_name = p_file_name;
        g_fp_c_priv.b_seek_play = seek_play;
    }

    mtos_free(p_asc_name);
    p_asc_name = NULL;

    return ret;
}

RET_CODE ui_video_c_load_media_from_net(u8 *p_asc_name, BOOL seek_play)
{
    mul_fp_play_state_t play_state = MUL_PLAY_STATE_NONE;
    RET_CODE            ret        = SUCCESS;

    MT_ASSERT(NULL != p_asc_name);

    /*!
     * If user player another file, but current file is on playing pausing or resuming,
     * stop it and then play another file.
     */
    play_state = ui_video_c_get_play_state();
    UI_VIDEO_PRINF("[%s]: play_state = %d\n", __FUNCTION__, play_state);

    if((MUL_PLAY_STATE_NONE != play_state) && (MUL_PLAY_STATE_STOP != play_state))
    {
        ui_video_c_stop();
    }

    /*!
     * Otherwise replay the new file
     */
    UI_VIDEO_PRINF("@@@url=%s\n", p_asc_name);

    ret = mul_file_play_load_media(g_fp_c_priv.m_handle, p_asc_name);
    if (ret == SUCCESS)
    {
        g_fp_c_priv.p_file_name = NULL;
        g_fp_c_priv.b_seek_play = seek_play;
    }
    return ret;
}


RET_CODE ui_video_c_play(void)
{
    RET_CODE            ret        = SUCCESS;

    if (g_fp_c_priv.b_seek_play)
    {
        UI_VIDEO_PRINF("[%s]: video seek: %d \n", __FUNCTION__, g_fp_c_priv.play_time_s);
    }
    else
    {
        g_fp_c_priv.play_time_s = 0;
    }
    UI_VIDEO_PRINF("ui_video_c_play\n");
    ret = mul_file_play_start(g_fp_c_priv.m_handle, g_fp_c_priv.play_time_s);
    UI_VIDEO_PRINF("ui_video_c_play 111\n");
    ui_video_c_set_usr_stop_flag(FALSE);
    if (ret == SUCCESS)
    {
        ui_set_mute(ui_is_mute());
        /*!
         * Load plug-in subtitle but not display
         */
       if (g_fp_c_priv.p_file_name != NULL)
       {
         ui_video_m_load_subt(g_fp_c_priv.p_file_name);
         ui_video_subt_bg_init();
       }
    }

    /*If in play state, set speed as normal*/
    ui_video_c_reset_speed();

    printf_ret(ret);

    return ret;
}
#endif

/*!
 * When press play key in preview state, play video
 */
 //lint -e438 
RET_CODE ui_video_c_play_by_file(u16 *p_file_name, u32 seek_play_time)
{
    ui_vdo_waiting_context_t *p_next_context;
    u16 *p_file_name_temp;
    u8  *p_asc_name = NULL;
    mul_fp_play_state_t play_state = MUL_PLAY_STATE_NONE;
    RET_CODE ret        = SUCCESS;
    u32 str_len;

    MT_ASSERT(NULL != p_file_name);

    /*!
     * If user player another file, but current file is on playing pausing or resuming,
     * stop it and then play another file.
     */
    play_state = ui_video_c_get_play_state();
    UI_VIDEO_PRINF("[%s]: play_state = %d\n", __FUNCTION__, play_state);

    if (play_state != MUL_PLAY_STATE_NONE)
    {
        ui_video_c_stop();

        p_next_context = &g_fp_c_priv.waiting_context;
        p_next_context->play_func = VIDEO_PLAY_FUNC_USB;
        p_next_context->seek_play_time = seek_play_time;
        str_len = uni_strlen(p_file_name);
        p_file_name_temp = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
        MT_ASSERT(p_file_name_temp != NULL);
        uni_strcpy(p_file_name_temp, p_file_name);
        p_next_context->vdo_url.p_file_name = p_file_name_temp;

        return ERR_FAILURE;
    }

    /*!
     * Otherwise replay the new file
     */
    p_asc_name = (u8*)mtos_malloc(3*MAX_FILE_PATH + 1);
    MT_ASSERT(p_asc_name != NULL);
    memset(p_asc_name, 0, 3*MAX_FILE_PATH + 1);
    _ui_video_c_uni2utf8(p_file_name, p_asc_name, (3*MAX_FILE_PATH + 1));

    //UI_VIDEO_PRINF("@@@file path=%s\n", p_asc_name);
    ret = mul_file_play_load_ex_media(g_fp_c_priv.m_handle, p_asc_name, 0/*VDO_PLAY_MODE_INVALID*/);
    if (ret == SUCCESS)
    {
        g_fp_c_priv.seek_play_time = seek_play_time;
        memcpy(g_fp_c_priv.p_file_name, p_file_name, MAX_FILE_PATH);
    }

    mtos_free(p_asc_name);
    p_asc_name = NULL;

    return ret;
}
//lint +e438 

#ifdef ENABLE_NETWORK
RET_CODE ui_video_c_play_by_url_ex(u8 **pp_url_list, u32 url_cnt, u32 seek_play_time, ui_play_mode_t play_mode)
{
    ui_vdo_waiting_context_t *p_next_context;
    u8  **pp_url_list_temp;
    u8  *p_net_url;
    mul_fp_play_state_t play_state = MUL_PLAY_STATE_NONE;
    RET_CODE ret = SUCCESS;
    u32 str_len;
    u16 i;

    MT_ASSERT(NULL != *pp_url_list);

    /*!
     * If user player another file, but current file is on playing pausing or resuming,
     * stop it and then play another file.
     */
    play_state = ui_video_c_get_play_state();
    UI_VIDEO_PRINF("[%s]: play_state = %d\n", __FUNCTION__, play_state);
    memset(g_fp_c_priv.p_file_name, 0, MAX_FILE_PATH);
    if (play_state != MUL_PLAY_STATE_NONE)
    {
        ui_video_c_stop();
        
        p_next_context = &g_fp_c_priv.waiting_context;
        p_next_context->play_func = VIDEO_PLAY_FUNC_ONLINE;
        p_next_context->seek_play_time = seek_play_time;
        p_next_context->play_mode = play_mode;

        pp_url_list_temp = (u8 **)mtos_malloc(url_cnt * sizeof(u8 *));
        MT_ASSERT(pp_url_list_temp != NULL);
        for (i = 0; i < url_cnt; i++)
        {
            if (pp_url_list[i] != NULL)
            {
                str_len = strlen(pp_url_list[i]);
                p_net_url = (char *)mtos_malloc(str_len + 1);
                MT_ASSERT(p_net_url != NULL);
                strcpy((char *)p_net_url, (char *)pp_url_list[i]);
                pp_url_list_temp[i] = (u8 *)p_net_url;
            }
        }
        p_next_context->vdo_url.net_url.pp_url_list = pp_url_list_temp;
        p_next_context->vdo_url.net_url.url_cnt = url_cnt;

        return ERR_FAILURE;
    }

    /*!
     * Otherwise replay the new file
     */
    if (url_cnt == 1)
    {
        ret = mul_file_play_load_ex_media(g_fp_c_priv.m_handle, (u8 *)pp_url_list[0], play_mode);
        OS_PRINTF("##%s, line[%d], load media url:[%s]##\n", __FUNCTION__,__LINE__, pp_url_list[0]);
    }
    else
    {
        ret = mul_file_play_load_mul_media(g_fp_c_priv.m_handle, (char **)pp_url_list, url_cnt);
    }

    if (ret == SUCCESS)
    {
        g_fp_c_priv.seek_play_time = seek_play_time;
    }

    return ret;
}

RET_CODE ui_video_c_play_by_url(u8 *p_net_url, u32 seek_play_time, ui_play_mode_t play_mode)
{
    u8  *pp_url_list[1];

    pp_url_list[0] = p_net_url;

    return ui_video_c_play_by_url_ex(pp_url_list, 1, seek_play_time, play_mode);
}
#endif

static RET_CODE _ui_video_c_start(void)
{
    RET_CODE ret           = SUCCESS;

    g_fp_c_priv.play_time_s = g_fp_c_priv.seek_play_time;
    UI_VIDEO_PRINF("@@@_ui_video_c_start play_time_s=%d\n", g_fp_c_priv.play_time_s);
    video_play_pause_resume = 0;

    ret = mul_file_play_start(g_fp_c_priv.m_handle, g_fp_c_priv.play_time_s);
    UI_VIDEO_PRINF("@@@_ui_video_c_start end\n");
    //set_volume(sys_status_get_global_volume());
    ui_video_c_set_usr_stop_flag(FALSE);
    if (ret == SUCCESS)
    {
        ui_set_mute(ui_is_mute());

        /*!
         * Load plug-in subtitle but not display
         */
        if(strlen((void *)g_fp_c_priv.p_file_name))
        {
          ret = ui_video_m_load_subt(g_fp_c_priv.p_file_name);
          if(ret == SUCCESS)
          {
            ui_video_subt_bg_init();     
          }
        }
    }

    /*If in play state, set speed as normal*/
    ui_video_c_reset_speed();
    printf_ret(ret);

    return ret;
}

BOOL ui_video_c_get_usr_stop_flag(void)
{
  return g_fp_c_priv.usr_stop;
}


static u8* sg_preload_aud_buf = NULL;
static u8* sg_preload_vid_buf = NULL;
/*!
 * Destroy file play module
 */
RET_CODE ui_video_c_destroy(void)
{
  void *p_video = NULL;
  vdec_buf_policy_t policy = VDEC_OPENDI_64M;
  
    UI_VIDEO_PRINF("[%s]: start\n", __FUNCTION__);

    video_play_pause_resume = 0;

    if(g_fp_c_priv.inited)
    {
        mul_file_play_destroy(g_fp_c_priv.m_handle);

        if(sg_preload_aud_buf)
        {
          mtos_free(sg_preload_aud_buf);
          sg_preload_aud_buf = NULL;
        }
        if(sg_preload_vid_buf)
        {
          mtos_free(sg_preload_vid_buf);
          sg_preload_vid_buf = NULL;
        }

        g_fp_c_priv.m_handle = 0;
        g_fp_c_priv.inited = FALSE;
        g_fp_c_priv.play_time_s = 0;
        fw_unregister_ap_evtmap(APP_FILEPLAY);
        fw_unregister_ap_evtproc(APP_FILEPLAY);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_USB_FILEPLAY);
   //     OS_PRINTF("@@@@fw_unregister_ap_msghost  ROOT_ID_USB_FILEPLAY ret = %d @@@@@,",fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_USB_FILEPLAY));
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_FILEPLAY_BAR);
#ifdef ENABLE_NETWORK
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_NETWORK_PLAYBAR); //fix bug 32018 ROOT_ID_VIDEO_PLAYER
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_VIDEO_PLAYER); 
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_MAINMENU);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_SMALL_LIST);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_PROG_BAR);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_BACKGROUND);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_LIVE_TV);
        fw_unregister_ap_msghost(APP_FILEPLAY, ROOT_ID_IPTV_PLAYER);
#endif
    }

  p_video = (void *)dev_find_identifier(NULL
   , DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);
  vdec_stop(p_video);

  OS_PRINTF("VIDEO_FW_CFG_ADDR :0x%x\n",g_video_fw_cfg_addr);
  vdec_set_buf(p_video, policy, g_video_fw_cfg_addr);

#ifndef WIN32
  vdec_do_avsync_cmd(p_video,AVSYNC_NO_PAUSE_SYNC_CMD,0);
#endif
    return SUCCESS;
}
/*!
 * Create file play module
 */
RET_CODE ui_video_c_create(u8 play_func)
{
    RET_CODE      ret  = SUCCESS;
    mul_fp_attr_t attr = {0};
    av_set_t      av_set = {0};
    video_play_pause_resume = 0;

    if(g_fp_c_priv.inited)
    {
      return SUCCESS;
    }

    if (play_func == VIDEO_PLAY_FUNC_USB)
    {
      attr.fs_task_prio = FILE_PLAYBACK_PRIORITY;
      attr.fs_task_size = FILE_PLAYBACK_STKSIZE;
      attr.vdec_start = g_video_fw_cfg_addr;
      attr.vdec_size = g_video_fw_cfg_size;
      attr.audio_start = g_audio_fw_cfg_addr;
      attr.audio_size = g_audio_fw_cfg_size;

      attr.v_mem_size = g_video_file_play_size;
      attr.v_mem_start = g_video_file_play_addr;

      attr.net_task_priority = FILE_PLAY_NET_TASK_PRIORITY;
      
       #ifdef MIN_AV_64M
        #ifdef MIN_AV_SDONLY
          attr.vdec_policy = VDEC_SDINPUT_ONLY;
        #else
          attr.vdec_policy = VDEC_BUFFER_AD_UNUSEPRESCALE;
        #endif
      #else
        attr.vdec_policy = VDEC_OPENDI_64M;
      #endif
/*
      //attr.stack_preload_size = 128*KBYTES;
      //attr.task_preload_priority = NET_PLAYBACK_PRIORITY;
      //attr.preload_video_buffer_size = 512*KBYTES;
      //attr.preload_audio_buffer_size = 128*KBYTES;
*/
    sys_status_get_av_set(&av_set);
    
    attr.audio_output = av_set.digital_audio_output;  //0-lpcm, 1-bs

    ret = mul_file_play_create(&attr, &g_fp_c_priv.m_handle);

      _ui_video_c_register_evt(ui_fileplay_callback);
      printf_ret(ret);

      fw_register_ap_evtmap(APP_FILEPLAY, ui_file_play_evtmap);
      fw_register_ap_evtproc(APP_FILEPLAY, ui_file_play_evtproc);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_USB_FILEPLAY);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_FILEPLAY_BAR);

      g_fp_c_priv.inited = TRUE;
    }
    else if (play_func == VIDEO_PLAY_FUNC_ONLINE)
    {
#ifdef ENABLE_NETWORK
      attr.fs_task_prio = FILE_PLAYBACK_PRIORITY;
      attr.fs_task_size = 512*1024;
      attr.vdec_start = g_video_fw_cfg_addr;
      attr.vdec_size = g_video_fw_cfg_size;
      attr.audio_start = g_audio_fw_cfg_addr;
      attr.audio_size = g_audio_fw_cfg_size;

      attr.v_mem_size = g_video_file_play_size; // -4M for network play
      attr.v_mem_start = g_video_file_play_addr;
     

      attr.stack_preload_size = 128*KBYTES;
      attr.task_preload_priority = NET_PLAYBACK_PRIORITY;
      
      #ifdef MIN_AV_64M
        #ifdef MIN_AV_SDONLY
          attr.vdec_policy = VDEC_SDINPUT_ONLY;
        #else
          attr.vdec_policy = VDEC_BUFFER_AD_UNUSEPRESCALE;
        #endif
      #else
        attr.vdec_policy = VDEC_OPENDI_64M;
      #endif

      attr.preload_audio_buffer_size = 128*KBYTES;
      if(NULL == sg_preload_aud_buf)
      sg_preload_aud_buf = (u8*)mtos_malloc(attr.preload_audio_buffer_size); //tmp
      MT_ASSERT(NULL != sg_preload_aud_buf);
      attr.p_preload_audio_buf = sg_preload_aud_buf;

      attr.preload_video_buffer_size = 512*KBYTES;
      if(NULL == sg_preload_vid_buf)
      sg_preload_vid_buf = (u8*)mtos_malloc(attr.preload_video_buffer_size); //tmp
      MT_ASSERT(NULL != sg_preload_vid_buf);
      attr.p_preload_video_buf = sg_preload_vid_buf;

      sys_status_get_av_set(&av_set);
      attr.audio_output = av_set.digital_audio_output;  //0-lpcm, 1-bs
	  
      ret = mul_file_play_create(&attr, &g_fp_c_priv.m_handle);

      _ui_video_c_register_evt(ui_fileplay_callback);
      printf_ret(ret);

      fw_register_ap_evtmap(APP_FILEPLAY, ui_file_play_evtmap);
      fw_register_ap_evtproc(APP_FILEPLAY, ui_file_play_evtproc);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_NETWORK_PLAYBAR);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_VIDEO_PLAYER); 	  
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_MAINMENU);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_SMALL_LIST);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_PROG_BAR);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_BACKGROUND);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_LIVE_TV);
      fw_register_ap_msghost(APP_FILEPLAY, ROOT_ID_IPTV_PLAYER);
      g_fp_c_priv.inited = TRUE;
#else
      //FIXME:Add other play_funcs
      ret = ERR_NOFEATURE;
#endif
    }

    return ret;
}

BOOL ui_video_c_init_state(void)
{
  return g_fp_c_priv.inited;
}

void ui_video_c_set_charset(str_fmt_t charset)
{
    mul_file_play_set_charset(g_fp_c_priv.m_handle, charset);
}

RET_CODE ui_video_c_play_next(void)
{
    ui_vdo_waiting_context_t *p_next_context;
    u16 *p_file_name;

    p_next_context = &g_fp_c_priv.waiting_context;

#ifdef ENABLE_NETWORK
    if (p_next_context->play_func == VIDEO_PLAY_FUNC_ONLINE)
    {
        u8  **pp_url_list;

        pp_url_list = p_next_context->vdo_url.net_url.pp_url_list;
        if (pp_url_list != NULL)
        {
            ui_video_c_play_by_url_ex(pp_url_list, p_next_context->vdo_url.net_url.url_cnt, p_next_context->seek_play_time, p_next_context->play_mode);
            ui_video_c_clean_waiting_context();
            return SUCCESS;
        }
    }
    else
#endif
    if (p_next_context->play_func == VIDEO_PLAY_FUNC_USB)
    {
        p_file_name = p_next_context->vdo_url.p_file_name;
        if (p_file_name != NULL)
        {
            ui_video_c_play_by_file(p_next_context->vdo_url.p_file_name, p_next_context->seek_play_time);
            ui_video_c_clean_waiting_context();
            return SUCCESS;
        }
    }

    return ERR_NOFEATURE;
}

RET_CODE ui_video_c_on_loadmedia_error(u32 event, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_video_c_on_loadmedia_error\n");
    mul_file_play_set_play_state(g_fp_c_priv.m_handle, MUL_PLAY_STATE_NONE);
    return ui_video_c_play_next();
}

RET_CODE ui_video_c_on_loadmedia_success(u32 event, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_video_c_on_loadmedia_success\n");
    _ui_video_c_start();
    return ERR_NOFEATURE;
}

RET_CODE ui_video_c_on_loadmedia_exit(u32 event, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_video_c_on_loadmedia_exit\n");
    mul_file_play_set_play_state(g_fp_c_priv.m_handle, MUL_PLAY_STATE_NONE);
    return ui_video_c_play_next();
}

RET_CODE ui_video_c_on_stop_cfm(u32 event, u32 para1, u32 para2)
{
    OS_PRINTF("@@@ui_video_c_on_stop_cfm\n");
    mul_file_play_set_play_state(g_fp_c_priv.m_handle, MUL_PLAY_STATE_NONE);
    return ui_video_c_play_next();
}

RET_CODE ui_video_c_on_eos(u32 event, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

    OS_PRINTF("@@@ui_video_c_on_eos\n");
    mul_file_play_get_play_state(g_fp_c_priv.m_handle, &state);

    mul_file_play_set_play_state(g_fp_c_priv.m_handle, MUL_PLAY_STATE_NONE);
    if (state == MUL_PLAY_STATE_STOP)
    {
        return SUCCESS;
    }
    else
    {
        return ERR_NOFEATURE;
    }
}

u8 ui_video_c_get_play_func()
{
   return  g_fp_c_priv.waiting_context.play_func;
}

BEGIN_AP_EVTMAP(ui_file_play_evtmap)
CONVERT_EVENT(API_VIDEO_EVENT_EOS, MSG_VIDEO_EVENT_EOS)
CONVERT_EVENT(API_VIDEO_EVENT_UP_TIME, MSG_VIDEO_EVENT_UP_TIME)
CONVERT_EVENT(API_VIDEO_EVENT_RESOLUTION, MSG_VIDEO_EVENT_UP_RESOLUTION)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORTED_VIDEO, MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORTED_MEMORY, MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY)
CONVERT_EVENT(API_VIDEO_EVENT_TRICK_TO_BEGIN, MSG_VIDEO_EVENT_TRICK_TO_BEGIN)
CONVERT_EVENT(API_VIDEO_EVENT_UNSUPPORT_SEEK, MSG_VIDEO_EVENT_UNSUPPORT_SEEK)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_EXIT, MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_ERROR, MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS)
CONVERT_EVENT(API_VIDEO_EVENT_STOP_CFM, MSG_VIDEO_EVENT_STOP_CFM)
CONVERT_EVENT(API_VIDEO_EVENT_UPDATE_BPS, MSG_VIDEO_EVENT_UPDATE_BPS)
CONVERT_EVENT(API_VIDEO_EVENT_REQUEST_CHANGE_SRC, MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC)
CONVERT_EVENT(API_VIDEO_EVENT_SET_PATH_FAIL, MSG_VIDEO_EVENT_SET_PATH_FAIL)
CONVERT_EVENT(API_VIDEO_EVENT_LOAD_MEDIA_TIME, MSG_VIDEO_EVENT_LOAD_MEDIA_TIME)
CONVERT_EVENT(API_VIDEO_EVENT_FINISH_BUFFERING, MSG_VIDEO_EVENT_FINISH_BUFFERING)
CONVERT_EVENT(API_VIDEO_EVENT_FINISH_UPDATE_BPS, MSG_VIDEO_EVENT_FINISH_UPDATE_BPS)

END_AP_EVTMAP(ui_file_play_evtmap)

BEGIN_AP_EVTPROC(ui_file_play_evtproc)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_ERROR, ui_video_c_on_loadmedia_error)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, ui_video_c_on_loadmedia_success)
ON_EVENTPROC(API_VIDEO_EVENT_LOAD_MEDIA_EXIT, ui_video_c_on_loadmedia_exit)
ON_EVENTPROC(API_VIDEO_EVENT_STOP_CFM, ui_video_c_on_stop_cfm)
ON_EVENTPROC(API_VIDEO_EVENT_EOS, ui_video_c_on_eos)
ON_EVENTPROC(API_VIDEO_EVENT_SET_PATH_FAIL,ui_video_c_on_loadmedia_error)
END_AP_EVTPROC(ui_file_play_evtproc)
#endif
