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
#include "ui_rename.h"
#include "lib_char.h"
#include "file_play_api.h"
#include "ui_video.h"
#include "ui_mute.h"
#include "ui_volume_usb.h"
#ifdef ENABLE_NETWORK
#include "ui_edit_usr_pwd.h"
#include "pnp_service.h"
#endif
#ifdef ENABLE_ADS
#include "ui_ad_api.h"
#endif


/*!
 Const value
 */

/*!
 *Play line num
 */
#define VIDEO_V_PLAY_ICON_LINE  1
/*!
 *Play time string length
 */
#define VIDEO_V_PLAY_TIME_LEN   32

/*!
 *Group
 */
#define VIDEO_V_GROUP_X  (VIDEO_V_PREV_X + VIDEO_V_PREV_W + 20)
#define VIDEO_V_GROUP_Y  VIDEO_V_PREV_Y
#define VIDEO_V_GROUP_W  (COMM_BG_W - VIDEO_V_GROUP_X - WIN_LR_SPACE)
#define VIDEO_V_GROUP_H  46

/*!
 * List scrollbar
 */
#define VIDEO_V_LIST_SBAR_X  (VIDEO_V_LIST_X + VIDEO_V_LISTW + 4)
#define VIDEO_V_LIST_SBAR_Y  (VIDEO_V_LIST_Y)
#define VIDEO_V_LIST_SBARW  6
#define VIDEO_V_LIST_SBARH  VIDEO_V_LISTH

#define VIDEO_V_LIST_SBAR_MIDL  0
#define VIDEO_V_LIST_SBAR_MIDT  12
#define VIDEO_V_LIST_SBAR_MIDR  6
#define VIDEO_V_LIST_SBAR_MIDB  (VIDEO_V_LIST_SBARH - VIDEO_V_LIST_SBAR_MIDT)


//bottom infor
#define VIDEO_V_PATH_X  VIDEO_V_LIST_X
#define VIDEO_V_PATH_Y  (VIDEO_V_LIST_Y + VIDEO_V_LISTH + 6)
#define VIDEO_V_PATH_W  324
#define VIDEO_V_PATH_H  32


/*!
 * List
 */
#define VIDEO_V_LIST_X  VIDEO_V_GROUP_X
#define VIDEO_V_LIST_Y  (VIDEO_V_GROUP_Y + VIDEO_V_GROUP_H + 10)
#define VIDEO_V_LISTW   VIDEO_V_GROUP_W
#define VIDEO_V_LISTH   330//420

#define VIDEO_V_LIST_MIDL  8
#define VIDEO_V_LIST_MIDT  8
#define VIDEO_V_LIST_MIDW  (VIDEO_V_LISTW - 2 * VIDEO_V_LIST_MIDL)
#define VIDEO_V_LIST_MIDH  (VIDEO_V_LISTH - 2 * VIDEO_V_LIST_MIDT)

#define VIDEO_V_LIST_ICON_VGAP 4
#define VIDEO_V_LIST_ITEM_NUM_ONE_PAGE  8//10

/*!
 * Play detail
 */
#define VIDEO_V_PLAY_CONT_X  VIDEO_V_PREV_X
#ifdef USB_FILEPLAY_PREVIEW
#define VIDEO_V_PLAY_CONT_Y  (VIDEO_V_PREV_Y + VIDEO_V_PREV_H + 10)
#else
#define VIDEO_V_PLAY_CONT_Y  VIDEO_V_LIST_Y
#endif
#define VIDEO_V_PLAY_CONT_W  VIDEO_V_PREV_W
#ifdef USB_FILEPLAY_PREVIEW
#define VIDEO_V_PLAY_CONT_H  160//140
#else
#define VIDEO_V_PLAY_CONT_H  VIDEO_V_LISTH
#endif

/*!
 * Play detail file name
 */
#define VIDEO_V_DETAIL_FILE_NAME_X  10
#define VIDEO_V_DETAIL_FILE_NAME_Y  10
#define VIDEO_V_DETAIL_FILE_NAME_W  (VIDEO_V_PLAY_CONT_W - 2 * VIDEO_V_DETAIL_FILE_NAME_X)//470
#define VIDEO_V_DETAIL_FILE_NAME_H  30

/*!
 * Play detail file resolution
 */
#define VIDEO_V_DETAIL_FILE_PIXEL_X  10
#define VIDEO_V_DETAIL_FILE_PIXEL_Y  50
#define VIDEO_V_DETAIL_FILE_PIXEL_W (VIDEO_V_PLAY_CONT_W - 2 * VIDEO_V_DETAIL_FILE_PIXEL_X)
#define VIDEO_V_DETAIL_FILE_PIXEL_H  30

/*!
 * Play detail file size
 */
#define VIDEO_V_DETAIL_FILE_SIZE_X  10//230
#define VIDEO_V_DETAIL_FILE_SIZE_Y  90
#define VIDEO_V_DETAIL_FILE_SIZE_W  (VIDEO_V_PLAY_CONT_W - 2 * VIDEO_V_DETAIL_FILE_SIZE_X)
#define VIDEO_V_DETAIL_FILE_SIZE_H  30

/*!
 * Play detail current time
 */
#define VIDEO_V_PLAY_CURTM_X  60
#define VIDEO_V_PLAY_CURTM_Y  130
#define VIDEO_V_PLAY_CURTM_W  100
#define VIDEO_V_PLAY_CURTM_H  30

/*!
 * Play detail total time
 */
#define VIDEO_V_PLAY_TOLTM_X  170
#define VIDEO_V_PLAY_TOLTM_Y  130
#define VIDEO_V_PLAY_TOLTM_W  160
#define VIDEO_V_PLAY_TOLTM_H  30


/*!
 * Play detail play state icon
 */
#define VIDEO_V_PLAY_ICON_X  0
#define VIDEO_V_PLAY_ICON_Y  130
#define VIDEO_V_PLAY_ICON_W  50
#define VIDEO_V_PLAY_ICON_H  28


/*!
 * Play unsupport flag
 */
#define VIDEO_V_PLAY_UNSUPPORT_W  170
#define VIDEO_V_PLAY_UNSUPPORT_H  28
#define VIDEO_V_PLAY_UNSUPPORT_X  (VIDEO_V_PREV_X + (VIDEO_V_PREV_W - VIDEO_V_PLAY_UNSUPPORT_W) / 2)
#ifdef USB_FILEPLAY_PREVIEW
#define VIDEO_V_PLAY_UNSUPPORT_Y  (VIDEO_V_PREV_Y + (VIDEO_V_PREV_H - VIDEO_V_PLAY_UNSUPPORT_H) / 2)
#else
#define VIDEO_V_PLAY_UNSUPPORT_Y  (VIDEO_V_PLAY_CONT_Y + VIDEO_V_PLAY_CURTM_Y + VIDEO_V_PLAY_CURTM_H + \
	                                                         (VIDEO_V_PLAY_CONT_H - VIDEO_V_PLAY_CURTM_Y - VIDEO_V_PLAY_CURTM_H - VIDEO_V_PLAY_UNSUPPORT_H) / 2)
#endif

/*!
 * Play continue
 */
#define VIDEO_V_CONTINUE_W       300
#define VIDEO_V_CONTINUE_H       120
#define VIDEO_V_CONTINUE_X       ((SCREEN_WIDTH - VIDEO_V_CONTINUE_W) / 2)
#define VIDEO_V_CONTINUE_Y       ((SCREEN_HEIGHT - VIDEO_V_CONTINUE_H) / 2)

#define M_VIDEO_V_UNSUPPORT_SHOW_TIME  3000

/*!
 * Video color style
 */
#define VIDEO_V_FSI_COLOR FSI_WHITE
#define VIDEO_V_RSI_COLOR RSI_PBACK

/*!
 * Video list style
 */
static list_xstyle_t video_v_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_xstyle_t video_v_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLUE,
  FSI_WHITE,
};
static const list_field_attr_t video_list_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR,
        50,
        10,
        0,
        &video_v_list_field_rstyle,
        &video_v_list_field_fstyle
    },
    {
        LISTFIELD_TYPE_ICON,
        40,
        60,
        0,
        &video_v_list_field_rstyle,
        &video_v_list_field_fstyle
    },

    {
        LISTFIELD_TYPE_UNISTR | STL_LEFT ,
        340,
        100,
        0,
        &video_v_list_field_rstyle,
        &video_v_list_field_fstyle
    }
};

static list_xstyle_t video_v_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};

comm_help_data_t2 help_data[] = //help bar data
{
  {
    4, 200, {40, 280,40, 280},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_SELECT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_PARTITION,
    },
  },
#ifdef ENABLE_NETWORK
  {
    8, 70, {40, 160, 40, 160,40, 160,40, 160},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_SELECT,
      HELP_RSC_BMP   | IM_RECALL,
      HELP_RSC_STRID | IDS_BACK,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_SWITCH_TYPE,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_NETWORK_PLACE,
    },
  },
#endif
};

/*!
 * Type define
 */

/*!
 * Video play state icon
 */
typedef enum
{
    VIDEO_PLAY_ICON_PLAY = 0,
    VIDEO_PLAY_ICON_PAUSE,
    VIDEO_PLAY_ICON_STOP,
    VIDEO_PLAY_ICON_MAX,
}ui_video_v_play_icon_t;

/*!
 * Video view message
 */
typedef enum
{
    MSG_VIDEO_V_EVT_PLAY_END = MSG_LOCAL_BEGIN + 0x100,
    MSG_VIDEO_V_STOP,
    MSG_VIDEO_V_PAUSE_PLAY,
    MSG_VIDEO_V_FULLSCREEN,
    MSG_VOLUME_UP,
    MSG_VOLUME_DOWN,
    MSG_VIDEO_V_UPFOLDER,
    MSG_VIDEO_V_CHANGE_PARTITION,
    MSG_VIDEO_V_DISAPPEAR_UNSUPPORT,
#ifdef ENABLE_NETWORK
	  MSG_SWITCH_MEDIA_TYPE,
    MSG_BACK_TO_NETWORK_PLACE,
#endif
}ui_video_v_msg_t;

/*!
 * Video view control id
 */
typedef enum
{
    IDC_VIDEO_V_PREVIEW = 1,
    IDC_VIDEO_V_SBAR,
    IDC_VIDEO_V_LIST,
    IDC_VIDEO_V_PATH,
    IDC_VIDEO_V_PLAY_FILENAME,
    IDC_VIDEO_V_FILE_SIZE,
    IDC_VIDEO_V_FILE_PIXEL,
    IDC_VIDEO_V_PLAY_DETAIL_CONT,
    IDC_VIDEO_V_PLAY_CURTM,
    IDC_VIDEO_V_PLAY_TOLTM,
    IDC_VIDEO_V_PLAY_ICON,
    IDC_VIDEO_V_UNSUPPORT,
    IDC_VIDEO_V_GROUP_ARROWL,
    IDC_VIDEO_V_GROUP_ARROWR,
    IDC_VIDEO_V_GROUP,
}ui_video_v_ctrl_id_t;

/*!
 * exit type
 */
typedef enum
{
 EXIT_TYPE_EXIT_NONE = 1,
 EXIT_TYPE_EXIT_ALL,
 EXIT_TYPE_EXIT,
}ui_video_v_exit_type_t;

/*!
 Macro
 */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static void _ui_video_v_play_video(control_t *p_list, u16 focus);
static RET_CODE _ui_video_v_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2);
static void _ui_video_v_set_cur_playing_file_focus(control_t *p_list);

static BOOL preview_state = FALSE;

#ifdef ENABLE_NETWORK
static u32 is_usb = 1;
static char ipaddress[128] = "";
static u16 ip_address_with_path[32] ={0};
static u16  play_focus = 0;
#endif

static u32 g_partition_cnt = 0;
static partition_t *p_partition = NULL;
static ui_video_v_exit_type_t g_exit_type = EXIT_TYPE_EXIT_NONE;
static BOOL b_video_v_file_is_exit = TRUE;
/*!
 * Function define
 */
u16 _ui_video_v_list_keymap(u16 key);
RET_CODE _ui_video_v_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 _ui_video_v_cont_keymap(u16 key);
RET_CODE _ui_video_v_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

#ifdef ENABLE_NETWORK
u16 _ui_video_network_v_list_keymap(u16 key);
RET_CODE _ui_video_network_v_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


extern RET_CODE pnp_svc_unmount(u16 *p_url);
u32 get_video_is_usb()//if is_usb is 0 means video entry from samba 
{
  return is_usb;
}
void set_play_focus(u16 index)
{
  play_focus = index;
}

#endif

/*================================================================================================
                           video play view internel function
 ================================================================================================*/

static void _ui_video_v_stop_show_logo(void)
{
    void *p_video_dev = NULL;
    void *p_disp_dev  = NULL;
    void *p_dmx_dev   = NULL;

    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    if(NULL == p_disp_dev)
    {
        return;
    }

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

    if(preview_state)
    {
        preview_state = FALSE;
        p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

        if(NULL != p_video_dev)
        {
            vdec_stop(p_video_dev);
        }

        p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

        if(NULL != p_dmx_dev)
        {
            dmx_av_reset(p_dmx_dev);
        }
    }
}

/*!
 * Show playing video resolution
 */
static RET_CODE  _ui_video_v_show_resolution(void)
{
    control_t *p_play      = NULL;
    control_t *p_pixel     = NULL;
    u8         asc_str[32] = {0};
    rect_size_t _rect      = {0};

    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_pixel = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_FILE_PIXEL);
    MT_ASSERT(p_pixel != NULL);
    
    ui_video_m_get_saved_resolution(&_rect);

    sprintf((char *)asc_str, "%ld x %ld", _rect.w, _rect.h);
        MT_ASSERT(p_pixel != NULL);
    text_set_content_by_ascstr(p_pixel, asc_str);
    ctrl_paint_ctrl(p_pixel, TRUE);

    return SUCCESS;
}
/*!
 * Show playing video resolution
 */
static RET_CODE  _ui_video_v_update_resolution(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    rect_size_t *p_rect =  (rect_size_t *)para1;

    if(ROOT_ID_USB_FILEPLAY != fw_get_focus_id())
    {
      return SUCCESS;
    }

    if(NULL == p_rect)
    {
        return ERR_FAILURE;
    }

    ui_video_m_save_resolution(p_rect);
    _ui_video_v_show_resolution();

    return SUCCESS;
}
/*!
 * Update total time
 */
static RET_CODE _ui_video_v_update_cur_time(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    control_t         *p_time = NULL;
    control_t         *p_play   = NULL;
    video_play_time_t  play_time     = {0};
    char               time_str[VIDEO_V_PLAY_TIME_LEN] = {0};

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_CURTM);

    UI_VIDEO_PRINF("_ui_video_v_update_cur_time %d\n", para1);

    ui_video_c_switch_time(&play_time, para1 / 1000);

    sprintf(time_str, "%02d:%02d:%02d", play_time.hour, play_time.min, play_time.sec);

    text_set_content_by_ascstr(p_time, (u8 *)time_str);

    ctrl_paint_ctrl(p_time, TRUE);
    return SUCCESS;
}

/*!
 * Scroll list
 */
static void _ui_video_v_list_scroll(control_t *p_cont)
{
    roll_param_t p_param = {ROLL_LR};

    MT_ASSERT(p_cont != NULL);

    gui_stop_roll(p_cont);

    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats = 0;
    p_param.is_force = FALSE;

    gui_start_roll(p_cont, &p_param);
}
static void _ui_video_v_init(void)
{
    subt_rsc_hdl_deinit();
}

void ui_video_v_destroy(void)
{
    /*!
     * release subt mem.
     */
    subt_rsc_hdl_destroy();
    ui_video_c_stop();

#ifdef ENABLE_NETWORK	
    if((fw_find_root_by_id(ROOT_ID_USB_MUSIC) == NULL) && !is_usb)
    {
      u16 * p_url = NULL;
      ui_get_ip_path_mount(&p_url);
      pnp_service_vfs_unmount(p_url);
    }
#endif
	
    ui_video_c_destroy();
}
/*!
 * Exit video view
 */
static RET_CODE _ui_video_v_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);

    UI_VIDEO_PRINF("[%s]: exit all\n", __FUNCTION__);
    
    state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    ui_video_c_stop();
    ui_video_m_reset();

    if(state == MUL_PLAY_STATE_NONE
      || state == MUL_PLAY_STATE_LOAD_MEDIA 
      || state == MUL_PLAY_STATE_STOP)
    {
      ui_video_v_destroy();
      manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
      //ui_close_all_mennus();
    }
    else
    {
      ui_video_c_stop();
      g_exit_type = EXIT_TYPE_EXIT_ALL;
    }
    return SUCCESS;
}


static RET_CODE _ui_video_v_change_volume(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_USB_FILEPLAY, 1);
    return SUCCESS;
}

static void _ui_video_v_show_unsupport(void)
{
    control_t *p_unsp = NULL;

    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);

    ctrl_set_sts(p_unsp, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_unsp, TRUE);
}

static void _ui_video_v_hide_unsupport(void)
{
    control_t *p_unsp = NULL;

    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);

    ctrl_set_sts(p_unsp, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_unsp);
}

/*!
 * Get highlight play state icon index
 */
static u16 _ui_video_v_get_hl_play_icon_idx()
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

    state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    switch(state)
    {
        case MUL_PLAY_STATE_PLAY:
        case MUL_PLAY_STATE_LOAD_MEDIA:
        {
            return VIDEO_PLAY_ICON_PLAY;
        }
        case MUL_PLAY_STATE_PAUSE:
        case MUL_PLAY_STATE_PREVIEW:
        {
            return VIDEO_PLAY_ICON_PAUSE;
        }
        case MUL_PLAY_STATE_STOP:
        {
            return VIDEO_PLAY_ICON_STOP;
        }
        default:
        {
            return VIDEO_PLAY_ICON_MAX;
        }
    }
}

/*!
 * Update group
 */
static RET_CODE _ui_video_v_update_group(control_t *p_cbox, u16 focus, u16 *p_str, u16 max_length)
{
  uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}

/*!
 * Update total time
 */
static void _ui_video_v_update_total_time(void)
{
    control_t         *p_t_time = NULL;
    control_t         *p_play   = NULL;
    video_play_time_t  play_time     = {0};
    u8               time_str[VIDEO_V_PLAY_TIME_LEN] = {0};

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_t_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_TOLTM);

    ui_video_m_get_saved_total_time(&play_time);

    sprintf((char *)time_str, "/ %02d:%02d:%02d", play_time.hour, play_time.min, play_time.sec);

    text_set_content_by_ascstr(p_t_time, time_str);

    ctrl_paint_ctrl(p_t_time, TRUE);
}
/*!
 * Update play icon
 */
static void _ui_video_v_update_play_icon(void)
{
    u16        j      = 0;
    control_t *p_icon = NULL;
    control_t *p_play = NULL;

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_icon = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_ICON);

    j = _ui_video_v_get_hl_play_icon_idx();

    if(j == VIDEO_PLAY_ICON_PLAY)
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_PLAY_2_SELECT);
    }
    else if(j == VIDEO_PLAY_ICON_PAUSE)
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_PAUSE_SELECT);
    }
    else
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_STOP_SELECT);
    }

    ctrl_paint_ctrl(p_icon, TRUE);
}
/*!
 * Show playing video name
 */
static void _ui_video_v_show_select_file_name(u16 focus)
{
    control_t *p_play                 = NULL;
    control_t *p_name                 = NULL;
    u16        uniname[MAX_FILE_PATH] = {0};

    /*!
     * Change file name
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_name = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_FILENAME);

    ui_video_m_get_name_by_idx(focus, uniname, MAX_FILE_PATH);

    text_set_content_by_unistr(p_name, uniname);

    ctrl_paint_ctrl(p_name, TRUE);
}

/*!
 * Show playing video name
 */
static void _ui_video_v_show_select_file_size(u16 focus)
{
    control_t *p_play      = NULL;
    control_t *p_content   = NULL;
    u32        size        = 0;
    u8         asc_str[32] = {0};

    /*!
     * Change file name
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_content = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_FILE_SIZE);

    ui_video_m_get_size_by_idx(focus, &size);
    ui_conver_file_size_unit(size, asc_str);
    text_set_content_by_ascstr(p_content, asc_str);
    ctrl_paint_ctrl(p_content, TRUE);
}

static void _ui_video_v_reset_play_cur_time(control_t *p_list)
{
    control_t *p_time = NULL;
    control_t *p_play = NULL;

    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);

    p_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_CURTM);

    text_set_content_by_ascstr(p_time, (u8 *)"00:00:00");

    ctrl_paint_ctrl(p_time, TRUE);
}
/*!
 * Update preview info
 */
static void _ui_video_v_update_preview_info(u16 focus)
{

    _ui_video_v_show_select_file_name(focus);
    _ui_video_v_show_select_file_size(focus);
    _ui_video_v_update_play_icon();
    _ui_video_v_update_total_time();
    _ui_video_v_update_cur_time(NULL, 0, 0, 0);
    _ui_video_v_show_resolution();
}
/*!
 * list update function
 */
static RET_CODE _ui_video_v_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16         i                           = 0;
    u16         cnt                         = 0;
    file_type_t type                        = UNKNOW_FILE;
    u16         icon_id                     = 0;
    u8          asc_str[32]                 = {0};
    u16         file_uniname[MAX_FILE_PATH] = {0};

    cnt = list_get_count(p_list);
    cnt = MIN(cnt, size + start);
    for (i = start; i < cnt; i++)
    {
        type = ui_video_m_get_type_by_idx(i);

        if(UNKNOW_FILE == type)
        {
            UI_VIDEO_PRINF("[%s] ##err## unknown file\n", __FUNCTION__);
            continue;
        }

        /* NO. */
        sprintf((char *)asc_str, "%.3d ", i + 1);

        list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

        /* ICON */
        if(DIRECTORY == type)
        {
            list_set_field_content_by_icon(p_list, i, 1, IM_MP3_ICON_FOLDER);
        }
        else
        {
            icon_id = ui_video_m_get_video_icon(ui_video_m_get_name_ptr_by_idx(i));
            list_set_field_content_by_icon(p_list, i, 1, icon_id);
        }

        /* NAME */
        ui_video_m_get_name_by_idx(i, file_uniname, MAX_FILE_PATH);

        list_set_field_content_by_unistr(p_list, i, 2, file_uniname);
    }

    return SUCCESS;
}


/*!
 * Video list update path info
 */
static void _ui_video_v_update_path_info(control_t *p_list)
{
    control_t *p_bottom_info = NULL;
    u16       *p_path        = NULL;
    u16 uni_str[MAX_FILE_PATH] = {0};

    p_bottom_info = ctrl_get_child_by_id(p_list->p_parent, IDC_VIDEO_V_PATH);

    if(NULL == p_bottom_info)
    {
        return;
    }
    p_path = ui_video_m_get_cur_path();
    uni_strncpy(uni_str, p_path, MAX_FILE_PATH);
    text_set_content_by_unistr(p_bottom_info, uni_str);

    ctrl_paint_ctrl(p_bottom_info, TRUE);
}

/*!
 * up unsupported txt
 */
static RET_CODE _ui_video_v_unsupported_video(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus  = 0;
    rect_size_t rect = {0};
    video_play_time_t play_time = {0};
    if(ROOT_ID_USB_FILEPLAY == fw_get_focus_id())
    {

       focus = ui_video_m_get_cur_playing_idx();
       ui_video_m_set_support_flag_by_idx(focus, FALSE);

      /*!
       * Show name/size/icon/total time/current time/resolution
       */
       ui_video_c_get_resolution(&rect);
       ui_video_m_save_resolution(&rect);
       ui_video_c_get_total_time(&play_time);
       ui_video_m_save_total_time(&play_time);
      /*!
       * Show name/size/icon/total time/current time/resolution
       */
      _ui_video_v_update_preview_info(focus);

      _ui_video_v_show_unsupport();
      
      /*Set timer for play next video*/
      fw_tmr_create(ROOT_ID_USB_FILEPLAY, 
                    MSG_VIDEO_V_DISAPPEAR_UNSUPPORT, 
                    M_VIDEO_V_UNSUPPORT_SHOW_TIME,
                    FALSE);
    }

    return SUCCESS;
}


/*!
 * Video list change focus
 */
static RET_CODE _ui_video_v_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
		control_t* p_popup = NULL;

		p_popup = fw_find_root_by_id(ROOT_ID_POPUP);
		if(p_popup)
		{
		return SUCCESS;
		}

    ret = list_class_proc(p_list, msg, 0, 0);

    _ui_video_v_list_scroll(p_list);

    return ret;
}

/*!
 * Check same playing file
 */
static BOOL _ui_video_v_same_file(u16 focus)
{
    u16 *p_last_file = NULL;
    u16 *p_next_file = NULL;

    p_last_file = ui_video_m_get_cur_playing_file_path();
    p_next_file = ui_video_m_get_path_by_idx(focus);

    if((NULL == p_last_file) || (NULL == p_last_file) || (NULL == p_next_file))
    {
        UI_VIDEO_PRINF("[%s] ##err##\n", __FUNCTION__);
        return FALSE;
    }

    UI_VIDEO_PRINF("[%s] last:%s cur:%s\n", __FUNCTION__, p_last_file, p_next_file);

    if(0 == uni_strcmp(p_last_file, p_next_file))
    {
        return TRUE;
    }

    return FALSE;
}

/*!
 * Play video by focus
 */
static RET_CODE _ui_video_v_play_continue(u16 focus)
{
    RET_CODE  ret    = ERR_FAILURE;
    u16      *p_path = NULL;
    dlg_ret_t dlg_ret;
    comm_dlg_data_t choose_dlg = {
                                    ROOT_ID_USB_FILEPLAY,
                                    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                                    VIDEO_V_CONTINUE_X,
                                    VIDEO_V_CONTINUE_Y,
                                    VIDEO_V_CONTINUE_W,
                                    VIDEO_V_CONTINUE_H,
                                    IDS_RESUME_PLAY,
                                    0,
                                 };

    p_path = ui_video_m_get_path_by_idx(focus);
    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    dlg_ret = ui_comm_dlg_open2(&choose_dlg);
    if(dlg_ret == DLG_RET_YES)
    {
        ret = ui_video_c_play_by_file(p_path, ui_video_c_get_play_time());
    }
    else
    {
        ret = ui_video_c_play_by_file(p_path, 0);
    }

    return ret;
}

static BOOL _ui_video_v_check_continue(u16 focus)
{
    BOOL check = FALSE;

    check = _ui_video_v_same_file(focus);

    if(FALSE == check)
    {
        return FALSE;
    }

    check = ui_video_c_get_usr_stop_flag();

    if(FALSE == check)
    {
        return FALSE;
    }

    check = ui_video_m_check_cur_playing_file_support();

    if(FALSE == check)
    {
        return FALSE;
    }

    return TRUE;
}
/*!
 * Play video by focus
 */
static RET_CODE _ui_video_v_play_by_focus(u16 focus)
{
    u16               *p_path  = NULL;
    control_t *p_unsp = NULL;
    p_path = ui_video_m_get_path_by_idx(focus);

    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    UI_VIDEO_PRINF("[%s] play file:%s\n", __FUNCTION__, p_path);

    /*!
     * Stop logo, otherwise it will show masaic
     */
    _ui_video_v_stop_show_logo();

    /*!
     * If show "unsupport" last time, hide it
     */
    
    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);
    if(ctrl_get_sts(p_unsp) == OBJ_STS_SHOW)
    {
      _ui_video_v_hide_unsupport();
      fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);
    }


    /*!
     * Check play at begain or play at the time which is stopped by user
     */
    if(_ui_video_v_check_continue(focus))
    {
        _ui_video_v_play_continue(focus);
    }
    else
    {
        //ui_video_c_load_media_from_local(p_path, FALSE);
        ui_video_c_play_by_file(p_path, 0);
        ui_video_m_set_cur_playing_file_by_idx(focus);
        UI_PRINTF("@@@_ui_video_v_play_by_focus() idx=%d\n", focus);
    }

    return SUCCESS;
}
static RET_CODE _ui_video_v_on_load_media_success(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    rect_size_t rect = {0};
    video_play_time_t play_time = {0};
    u16 focus  = 0;
    control_t *p_unsp = NULL;
    
    focus = ui_video_m_get_cur_playing_idx();
    UI_PRINTF("@@@_ui_video_v_on_load_media_success() idx=%d\n", focus);

    if ( ROOT_ID_USB_FILEPLAY != fw_get_focus_id() )
    {
      return SUCCESS;
    }
    
    if(b_video_v_file_is_exit == FALSE)
    {
      //return SUCCESS;
    } 
    //ui_video_c_play();
    b_video_v_file_is_exit = FALSE;
  
    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, TRUE);

    /*!
     * Show name/size/icon/total time/current time/resolution
     */
     ui_video_c_get_resolution(&rect);
     ui_video_m_save_resolution(&rect);
     ui_video_c_get_total_time(&play_time);
     ui_video_m_save_total_time(&play_time);
    /*!
     * Show name/size/icon/total time/current time/resolution
     */
    _ui_video_v_update_preview_info(focus);
    
    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);
    if(ctrl_get_sts(p_unsp) == OBJ_STS_SHOW)
    {
      _ui_video_v_hide_unsupport();
      fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);
    }
    return SUCCESS;
}

static RET_CODE _ui_video_v_on_load_media_error(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus  = 0;
    rect_size_t rect = {0};
    video_play_time_t play_time = {0};

    UI_PRINTF("@@@on_netplay_load_media_error()\n");

    if ( ROOT_ID_USB_FILEPLAY != fw_get_focus_id() )
    {
        return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, FALSE);

    /*!
    * Show name/size/icon/total time/current time/resolution
    */
    ui_video_c_get_resolution(&rect);
    ui_video_m_save_resolution(&rect);
    ui_video_c_get_total_time(&play_time);
    ui_video_m_save_total_time(&play_time);
    /*!
    * Show name/size/icon/total time/current time/resolution
    */
    _ui_video_v_update_preview_info(focus);

    _ui_video_v_show_unsupport();
    
    fw_tmr_create(ROOT_ID_USB_FILEPLAY, 
                  MSG_VIDEO_V_DISAPPEAR_UNSUPPORT, 
                  M_VIDEO_V_UNSUPPORT_SHOW_TIME,
                  FALSE);
    return SUCCESS;
}
/*!
 * Pause or resume
 */
static RET_CODE _ui_video_v_pause_resume(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    RET_CODE            ret   = SUCCESS;
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    u16                 focus = 0;

    state = ui_video_c_get_play_state();
    if(state == MUL_PLAY_STATE_NONE || state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
        return SUCCESS;
    }

    if(state == MUL_PLAY_STATE_STOP)
    {
        focus = list_get_focus_pos(p_list);
        if(_ui_video_v_check_continue(focus))
        {
            _ui_video_v_play_continue(focus);
        }
   
        return SUCCESS;
    }

    ret = ui_video_c_pause_resume();

    if(SUCCESS == ret)
    {
        _ui_video_v_update_play_icon();
    }

    return SUCCESS;
}

/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_stop(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;

    UI_VIDEO_PRINF("[%s]: stop\n", __FUNCTION__);

    ret = ui_video_c_stop();

    _ui_video_v_update_play_icon();

    _ui_video_v_reset_play_cur_time(p_list);

    return ret;
}

/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_on_eos(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 idx = 0,focus = 0;
    UI_VIDEO_PRINF("[%s]: eos \n", __FUNCTION__);

    _ui_video_v_update_play_icon();

    _ui_video_v_reset_play_cur_time(p_list);
    
    if(fw_get_focus_id() == ROOT_ID_USB_FILEPLAY)
    {
      ui_video_c_play_end();
        
      ui_video_m_get_next_file_idx(&idx);
      
      _ui_video_v_play_by_focus(idx);
      
      _ui_video_v_set_cur_playing_file_focus(p_list);

      focus = list_get_focus_pos(p_list);

      _ui_video_v_update_preview_info(focus);
      
      list_select_item(p_list, focus);
      
      ctrl_paint_ctrl(p_list, TRUE);
    }
    else
    {
    }

    return SUCCESS;
}
/*!
 * After enter next folder or partition, need to check current playing file is in it or not
 */
static void _ui_video_v_set_cur_playing_file_focus(control_t *p_list)
{
    RET_CODE ret = SUCCESS;
    u32      idx = 0;

    ret = ui_video_m_find_idx_by_cur_play_file(&idx);

    if(SUCCESS == ret)
    {
        UI_VIDEO_PRINF("[%s]: idx: %d \n", __FUNCTION__, idx);
        list_set_focus_pos(p_list, (u16)idx);
        list_select_item(p_list, (u16)idx);
    }
}
/*!
 * Rebuild list when enter next folder or next partition
 */
static void _ui_video_v_rebuild_list(control_t *p_list)
{
    u32 cnt = 0;

    cnt = ui_video_m_get_total();

    list_set_count(p_list, (u16)cnt, VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);

    _ui_video_v_list_update(p_list, list_get_valid_pos(p_list), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE, 0);
    _ui_video_v_set_cur_playing_file_focus(p_list);

    ctrl_paint_ctrl(p_list, TRUE);

    _ui_video_v_list_scroll(p_list);
    _ui_video_v_update_path_info(p_list);
}
/*!
 * Update total time
 */
static RET_CODE _ui_video_v_up_folder(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 *p_path = NULL;
    u32   len    = 0;

    p_path = ui_video_m_get_path_by_idx(0);

    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    len = uni_strlen(p_path);

    UI_VIDEO_PRINF("[%s]: %d %d . =%d\n", __FUNCTION__, p_path[len - 1], p_path[len - 2], '.');

    if((len > 2) && ('.' != (u8)p_path[len - 1] || '.' != (u8)p_path[len - 2]))
    {
        // root dir
        return ERR_FAILURE;
    }

    ui_video_m_back_to_parent_dir();
    _ui_video_v_rebuild_list(p_list);

    return SUCCESS;
}
/*!
 * Receive fullscreen close msg
 */
static RET_CODE _ui_video_v_on_fullscreen_close(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;

    UI_VIDEO_PRINF("[%s]: fullscreen close p_list:%p,para1:%d\n", __FUNCTION__,p_list, para1);

    _ui_video_v_set_cur_playing_file_focus(p_list);

    focus = list_get_focus_pos(p_list);

    _ui_video_v_update_preview_info(focus);

    // If we receive exit all message from full screen ,then we'll also close the video menu.
    if (para1 == MSG_EXIT_ALL)
    {
      fw_notify_root(p_list, NOTIFY_T_MSG, TRUE, MSG_EXIT_ALL, 0, 0);
    }
    return SUCCESS;
}
static RET_CODE _ui_video_v_change_partition(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 cur_partition = 0;
    u32 partition_cnt = 0;
    control_t *p_cbox = NULL;

    p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_VIDEO_V_GROUP);

    partition_cnt = ui_video_m_get_partition_cnt();

    /*!
     * If only 1 partition, needn't change
     */
    if(partition_cnt <= 1)
    {
        return SUCCESS;
    }

    cur_partition = ui_video_m_get_cur_partition();
    if(cur_partition + 1 >= partition_cnt )
      ui_video_m_load(0);
    else
      ui_video_m_load(cur_partition + 1);

    cbox_class_proc(p_cbox, MSG_INCREASE, 0, 0);
    
    _ui_video_v_rebuild_list(p_list);

    return SUCCESS;
}

static void _ui_video_v_enter_fullscreen(void)
{
#ifdef ENABLE_NETWORK	
    manage_open_menu(ROOT_ID_FILEPLAY_BAR, is_usb, 0);
#else
    manage_open_menu(ROOT_ID_FILEPLAY_BAR, TRUE, 0);
#endif
    //ui_video_c_sub_show();
}
/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_fullscreen(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = ui_video_c_get_play_state();

    if((MUL_PLAY_STATE_PLAY == state) || (MUL_PLAY_STATE_PAUSE == state))
    {
        _ui_video_v_enter_fullscreen();
    }

    return SUCCESS;
}
/*!
 * Enter folder
 */
static void _ui_video_v_enter_folder(control_t *p_list, u16 focus)
{
    ui_video_m_go_to_dir(focus);
    _ui_video_v_rebuild_list(p_list);
}
/*!
 * Play video
 */
static void _ui_video_v_play_video(control_t *p_list, u16 focus)
{
    mul_fp_play_state_t state = ui_video_c_get_play_state();
    BOOL b_same = _ui_video_v_same_file(focus);

    UI_PRINTF("@@@_ui_video_v_play_video() state=%d, same=%d\n", state, b_same);
    if (b_same == TRUE && MUL_PLAY_STATE_LOAD_MEDIA == state)
    {
        return;
    }

    if(b_same == TRUE && MUL_PLAY_STATE_PLAY == state)
    {
        if (ui_video_m_check_cur_playing_file_support() == TRUE)
    {
        _ui_video_v_enter_fullscreen();
        }
        return;
    }

    if(b_same == TRUE && MUL_PLAY_STATE_PAUSE == state)
    {
        ui_video_c_pause_resume();
        _ui_video_v_update_play_icon();
        return;
    }

    _ui_video_v_play_by_focus(focus);

    list_select_item(p_list, focus);
    ctrl_paint_ctrl(p_list, TRUE);
}
/*!
 * Video list select
 */
static RET_CODE _ui_video_v_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16         focus  = 0;
    file_type_t type   = UNKNOW_FILE;

    focus = list_get_focus_pos(p_list);

    if(INVALIDID == focus)
    {
        return ERR_FAILURE;
    }

    type = ui_video_m_get_type_by_idx(focus);

    UI_VIDEO_PRINF("[%s]: type: %d", __FUNCTION__, type);

    if(UNKNOW_FILE == type)
    {
        return ERR_FAILURE;
    }

    if(DIRECTORY == type)
    {
        _ui_video_v_enter_folder(p_list, focus);
        _ui_video_v_rebuild_list(p_list);
    }
    else
    {
        _ui_video_v_play_video(p_list, focus);
    }

    return SUCCESS;
}

//lint -e830
//lint -e550
//lint -e438

/*!
 * Create play state icon control
 */
static void _ui_video_v_creat_play_icon(control_t *p_play)
{
    control_t *p_play_icon = NULL;

    p_play_icon = ctrl_create_ctrl(CTRL_BMAP,
                                   IDC_VIDEO_V_PLAY_ICON,
                                   VIDEO_V_PLAY_ICON_X,
                                   VIDEO_V_PLAY_ICON_Y,
                                   VIDEO_V_PLAY_ICON_W,
                                   VIDEO_V_PLAY_ICON_H,
                                   p_play,
                                   0);

    MT_ASSERT(p_play_icon != NULL);
}

//lint +e830
//lint +e550
//lint +e438

/*!
 * Create play detail control
 */
static control_t * _ui_video_v_creat_play_container(control_t *p_menu)
{
    control_t *p_play_cont = NULL;

    p_play_cont = ctrl_create_ctrl(CTRL_CONT,
                                   IDC_VIDEO_V_PLAY_DETAIL_CONT,
                                   VIDEO_V_PLAY_CONT_X,
                                   VIDEO_V_PLAY_CONT_Y,
                                   VIDEO_V_PLAY_CONT_W,
                                   VIDEO_V_PLAY_CONT_H,
                                   p_menu,
                                   0);
    if(NULL == p_play_cont)
    {
        return NULL;
    }

    ctrl_set_rstyle(p_play_cont, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);

    return p_play_cont;
}
/*!
 * Create play total time control
 */
static void _ui_video_v_creat_play_total_time(control_t *p_play)
{
    control_t *p_total_time = NULL;

    p_total_time = ctrl_create_ctrl(CTRL_TEXT,
                                    IDC_VIDEO_V_PLAY_TOLTM,
                                    VIDEO_V_PLAY_TOLTM_X,
                                    VIDEO_V_PLAY_TOLTM_Y,
                                    VIDEO_V_PLAY_TOLTM_W,
                                    VIDEO_V_PLAY_TOLTM_H,
                                    p_play,
                                    0);

    MT_ASSERT(p_total_time != NULL);
    
    ctrl_set_rstyle(p_total_time, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);

    text_set_font_style(p_total_time, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_total_time, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_total_time, TEXT_STRTYPE_UNICODE);
}
/*!
 * Create play current time control
 */
static void _ui_video_v_creat_play_cur_time(control_t *p_play)
{
    control_t *p_cur_time = NULL;

    p_cur_time = ctrl_create_ctrl(CTRL_TEXT,
                                  IDC_VIDEO_V_PLAY_CURTM,
                                  VIDEO_V_PLAY_CURTM_X,
                                  VIDEO_V_PLAY_CURTM_Y,
                                  VIDEO_V_PLAY_CURTM_W,
                                  VIDEO_V_PLAY_CURTM_H,
                                  p_play,
                                  0);
    MT_ASSERT(p_cur_time != NULL);

    ctrl_set_rstyle(p_cur_time, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_cur_time, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_cur_time, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_cur_time, TEXT_STRTYPE_UNICODE);
}

/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_name(control_t *p_play)
{
    control_t *p_name = NULL;

    p_name = ctrl_create_ctrl(CTRL_TEXT,
                              IDC_VIDEO_V_PLAY_FILENAME,
                              VIDEO_V_DETAIL_FILE_NAME_X,
                              VIDEO_V_DETAIL_FILE_NAME_Y,
                              VIDEO_V_DETAIL_FILE_NAME_W,
                              VIDEO_V_DETAIL_FILE_NAME_H,
                              p_play,
                              0);
    MT_ASSERT(p_name != NULL);

    ctrl_set_rstyle(p_name, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_name, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_name, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_name, TEXT_STRTYPE_UNICODE);
}

/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_size(control_t *p_play)
{
    control_t *p_name = NULL;

    p_name = ctrl_create_ctrl(CTRL_TEXT,
                              IDC_VIDEO_V_FILE_SIZE,
                              VIDEO_V_DETAIL_FILE_SIZE_X,
                              VIDEO_V_DETAIL_FILE_SIZE_Y,
                              VIDEO_V_DETAIL_FILE_SIZE_W,
                              VIDEO_V_DETAIL_FILE_SIZE_H,
                              p_play,
                              0);
    MT_ASSERT(p_name != NULL);

    ctrl_set_rstyle(p_name, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_name, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_name, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_name, TEXT_STRTYPE_UNICODE);
}
/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_pixel(control_t *p_play)
{
    control_t *p_pix = NULL;

    p_pix = ctrl_create_ctrl(CTRL_TEXT,
                             IDC_VIDEO_V_FILE_PIXEL,
                             VIDEO_V_DETAIL_FILE_PIXEL_X,
                             VIDEO_V_DETAIL_FILE_PIXEL_Y,
                             VIDEO_V_DETAIL_FILE_PIXEL_W,
                             VIDEO_V_DETAIL_FILE_PIXEL_H,
                              p_play,
                              0);
    MT_ASSERT(p_pix != NULL);

    ctrl_set_rstyle(p_pix, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_pix, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_pix, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_pix, TEXT_STRTYPE_UNICODE);
}

/*!
 * Create partition group 
 */
static control_t * _ui_video_v_creat_group(control_t *p_menu)
{
    control_t *p_group       = NULL;
    p_partition = NULL;
    g_partition_cnt = 0;
    g_partition_cnt = file_list_get_partition(&p_partition);

    //group
    p_group = ctrl_create_ctrl(CTRL_CBOX, 
                               IDC_VIDEO_V_GROUP, 
                               VIDEO_V_GROUP_X,
                               VIDEO_V_GROUP_Y, 
                               VIDEO_V_GROUP_W,
                               VIDEO_V_GROUP_H, 
                               p_menu, 
                               0);
    
    ctrl_set_rstyle(p_group, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE);
    cbox_set_font_style(p_group, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
    cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
    cbox_enable_cycle_mode(p_group, TRUE);
  
    cbox_dync_set_count(p_group, (u16)g_partition_cnt);
    cbox_dync_set_update(p_group, _ui_video_v_update_group);
    cbox_dync_set_focus(p_group, 0);

    return p_group;
}

/*!
 * Create partition cur path
 */
static control_t * _ui_video_v_creat_path(control_t *p_menu)
{
    control_t *p_bottom_info = NULL;

    //cur path
    p_bottom_info = ctrl_create_ctrl(CTRL_TEXT,
                                     IDC_VIDEO_V_PATH,
                                     VIDEO_V_PATH_X,
                                     VIDEO_V_PATH_Y,
                                     VIDEO_V_PATH_W,
                                     VIDEO_V_PATH_H,
                                     p_menu,
                                     0);

    ctrl_set_rstyle(p_bottom_info, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_bottom_info, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_bottom_info, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_bottom_info, TEXT_STRTYPE_UNICODE);

    if(ui_video_m_get_partition_name(0) != NULL 
#ifdef ENABLE_NETWORK	
    && is_usb
#endif
    )
    {
        text_set_content_by_unistr(p_bottom_info, ui_video_m_get_partition_name(0));
    }
#ifdef ENABLE_NETWORK
    else if (! is_usb)
    {
      OS_PRINTF("Network path: %s\n", ipaddress);
      text_set_content_by_ascstr(p_bottom_info, ipaddress);
    }
#endif

    return p_bottom_info;
}


/*!
 * Create play list scrolbar control
 */
static control_t * _ui_video_v_creat_sbar(control_t *p_menu)
{
    control_t *p_sbar = NULL;

    p_sbar = ctrl_create_ctrl(CTRL_SBAR,
                                   IDC_VIDEO_V_SBAR,
                                   VIDEO_V_LIST_SBAR_X,
                                   VIDEO_V_LIST_SBAR_Y,
                                   VIDEO_V_LIST_SBARW,
                                   VIDEO_V_LIST_SBARH,
                                   p_menu,
                                   0);

    MT_ASSERT(p_sbar != NULL);

    ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_sbar, TRUE);
    sbar_set_direction(p_sbar, 0);

    sbar_set_mid_rstyle(p_sbar,
                        RSI_SCROLL_BAR_MID,
                        RSI_SCROLL_BAR_MID,
                        RSI_SCROLL_BAR_MID);

    ctrl_set_mrect(p_sbar,
                   VIDEO_V_LIST_SBAR_MIDL,
                   VIDEO_V_LIST_SBAR_MIDT,
                   VIDEO_V_LIST_SBAR_MIDR,
                   VIDEO_V_LIST_SBAR_MIDB);
    return p_sbar;
}

/*!
 * Create video list control
 */
static void _ui_video_v_creat_list(control_t *p_menu, control_t *p_sbar)
{
    u8         i      = 0;
    u16        cnt    = 0;
    control_t *p_list = NULL;
    p_list = ctrl_create_ctrl(CTRL_LIST,
                              IDC_VIDEO_V_LIST,
                              VIDEO_V_LIST_X,
                              VIDEO_V_LIST_Y,
                              VIDEO_V_LISTW,
                              VIDEO_V_LISTH,
                              p_menu,
                              0);

    ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
#ifdef ENABLE_NETWORK	
    if(! is_usb)
    {
      ctrl_set_keymap(p_list, _ui_video_network_v_list_keymap);
      ctrl_set_proc(p_list, _ui_video_network_v_list_proc);
    }
    else
#endif
    {
      ctrl_set_keymap(p_list, _ui_video_v_list_keymap);
      ctrl_set_proc(p_list, _ui_video_v_list_proc);
    }
    ctrl_set_mrect(p_list,
                   VIDEO_V_LIST_MIDL,
                   VIDEO_V_LIST_MIDT,
                   VIDEO_V_LIST_MIDW + VIDEO_V_LIST_MIDL,
                   VIDEO_V_LIST_MIDH + VIDEO_V_LIST_MIDT);

    list_set_item_interval(p_list, VIDEO_V_LIST_ICON_VGAP);

    list_set_item_rstyle(p_list, &video_v_list_item_rstyle);

    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    cnt = (u16)ui_video_m_get_total();

    list_set_count(p_list, cnt, VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(video_list_attr), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, _ui_video_v_list_update, 0);
    ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

    for (i = 0; i < ARRAY_SIZE(video_list_attr); i++)
    {
        list_set_field_attr(p_list,
                            i,
                            video_list_attr[i].attr,
                            video_list_attr[i].width,
                            video_list_attr[i].left,
                            video_list_attr[i].top);

        list_set_field_rect_style(p_list, i, video_list_attr[i].rstyle);
        list_set_field_font_style(p_list, i, video_list_attr[i].fstyle);
    }

    list_set_scrollbar(p_list, p_sbar);
    _ui_video_v_list_update(p_list, list_get_valid_pos(p_list), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE, 0);
    _ui_video_v_list_scroll(p_list);

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
}

#ifdef USB_FILEPLAY_PREVIEW
/*!
 * Create video preview control
 */
static void _ui_video_v_creat_preview(control_t *p_menu)
{
    control_t *p_preview = NULL;

    p_preview = ctrl_create_ctrl(CTRL_CONT,
                                 IDC_VIDEO_V_PREVIEW,
                                 VIDEO_V_PREV_X ,
                                 VIDEO_V_PREV_Y,
                                 VIDEO_V_PREV_W,
                                 VIDEO_V_PREV_H,
                                 p_menu,
                                 0);

    MT_ASSERT(p_preview != NULL);
    ctrl_set_rstyle(p_preview, RSI_TV, RSI_TV, RSI_TV);
}
#endif

/*!
 * Create video group control
 */
static void _ui_video_v_creat_help(control_t *p_menu)
{
#ifdef ENABLE_NETWORK	
    ui_comm_help_create2(is_usb? &help_data[0] : &help_data[1], p_menu,FALSE);
#else
    ui_comm_help_create2(&help_data[0], p_menu,FALSE);
#endif
    //TODO:use macro
    //ui_comm_help_move_pos(p_menu, 65, 20, 920, 50, 42);
 
}
/*!
 * Create video menu control
 */
static control_t * _ui_video_v_creat_menu(void)
{
    control_t *p_menu = NULL;

    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create(ROOT_ID_USB_FILEPLAY,
                                 0,
                                 COMM_BG_X,
                                 COMM_BG_Y,
                                 COMM_BG_W,
                                 COMM_BG_H,
                                 RSC_INVALID_ID,
                                 IDS_MOVIES);

    if(NULL == p_menu)
    {
        return NULL;
    }

    ctrl_set_keymap(p_menu, _ui_video_v_cont_keymap);
    ctrl_set_proc(p_menu, _ui_video_v_cont_proc);


    return p_menu;
}

/*!
 * Exit video view
 */
static RET_CODE _ui_video_v_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    BOOL is_enter_dir = FALSE;
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;


    UI_VIDEO_PRINF("[%s][%d]\n",__FUNCTION__,__LINE__);
    fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);

#ifdef ENABLE_NETWORK
    if(!is_usb)
    {      
      u16 *p_url = NULL;
      
      ui_time_enable_heart_beat(FALSE);
      ui_video_v_destroy();
      ui_get_ip_path_mount(&p_url);
      pnp_service_vfs_unmount(p_url);
      manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
      return SUCCESS;
    }
#endif

    is_enter_dir = ui_video_file_is_enter_dir();
    UI_VIDEO_PRINF("In [%s] Line %d, exit\n",(char *)__FUNCTION__, __LINE__);
    
    if(is_enter_dir)
    {
      _ui_video_v_enter_folder(p_list, 0);
    }
    else
    {     
      state = (mul_fp_play_state_t)ui_video_c_get_play_state();
      ui_video_c_stop();

      ui_video_m_reset();

      if(state == MUL_PLAY_STATE_NONE 
        || state == MUL_PLAY_STATE_LOAD_MEDIA 
        || state == MUL_PLAY_STATE_STOP)
      {
        ui_video_v_destroy();
        manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
      }
      else
      {
        ui_video_c_stop();
        g_exit_type = EXIT_TYPE_EXIT_ALL;
      }
    }
    return SUCCESS;
}


static void _ui_video_v_create_unsupport(control_t *p_menu)
{
    control_t *p_unspt = NULL;

    p_unspt = ctrl_create_ctrl(CTRL_TEXT,
                               IDC_VIDEO_V_UNSUPPORT,
                               VIDEO_V_PLAY_UNSUPPORT_X,
                               VIDEO_V_PLAY_UNSUPPORT_Y,
                               VIDEO_V_PLAY_UNSUPPORT_W,
                               VIDEO_V_PLAY_UNSUPPORT_H,
                               p_menu,
                               0);

    MT_ASSERT(p_unspt != NULL);

    ctrl_set_rstyle(p_unspt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_font_style(p_unspt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_unspt, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_unspt, TEXT_STRTYPE_STRID);

    text_set_content_by_strid(p_unspt, IDS_UNSUPPORTED);

    ctrl_set_sts(p_unspt, OBJ_STS_HIDE);

}


#ifdef ENABLE_NETWORK
static RET_CODE _ui_video_v_exit_to_network_place(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 * p_url = NULL;
    UI_VIDEO_PRINF("[%s]: exit\n", __FUNCTION__);
    if(!is_usb)
    {
      ui_video_v_destroy();
      ui_get_ip_path_mount(&p_url);
      pnp_service_vfs_unmount(p_url);
      manage_open_menu(ROOT_ID_NETWORK_PLACES, 0, 0);
      manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
      manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
    }  
    
    return SUCCESS;
}

static RET_CODE _ui_video_change(control_t *p_list, u16 msg, u32 para1, u32 para2)
{  
	if (is_usb)
		return SUCCESS;
		
  fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);

  ui_time_enable_heart_beat(FALSE);
  /*!
   * release subt mem.
   */
  subt_rsc_hdl_destroy();
  ui_video_c_stop();

  ui_video_c_destroy();  
  
  manage_open_menu(ROOT_ID_USB_MUSIC, 1,  (u32)(ipaddress));

  manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);

	return SUCCESS;
}
#endif

/*!
 * Video view entry
 */
RET_CODE ui_video_v_open(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_sbar = NULL;
    control_t *p_play = NULL;
    RET_CODE   ret    = ERR_FAILURE;
    comm_dlg_data_t dlg_data = {0};
    g_exit_type = EXIT_TYPE_EXIT_NONE;

#ifdef ENABLE_NETWORK	
    is_usb = (para1 == 1)?0:1;
    if(!is_usb)
    {
     memcpy(ipaddress, (char *)para2, 128);
    }
#endif
    if(!ui_is_mute())
    {
      set_volume_usb(sys_status_get_global_media_volume());
    }
    /*!
     * Reset for forget the played information
     */
    ui_video_m_reset();
    
    //initialize global variables.
   // ui_video_m_set_ip_cur(0);
    /*!
     * Load video modle data
     */
#ifdef ENABLE_NETWORK	
  if(!is_usb)
  {
    ui_get_ip_path(ip_address_with_path);
    ret = ui_video_m_net_load(ip_address_with_path);
  }
  else
#endif
	{
		g_partition_cnt = (u16)file_list_get_partition(&p_partition);
   
	    if(g_partition_cnt > 0)
	    {
	      //ui_video_m_set_is_usb(1);
	    ret = ui_video_m_load(0);
		    //ui_video_m_get_ipaddr(ipaddress_temp);
	    }
	}
    

 
    if(SUCCESS != ret)
    {
   //   return ret;
    }
    /*!
     * Create video controller
     */
    ui_video_c_create(VIDEO_PLAY_FUNC_USB);


    /*Create Menu*/
    p_menu = _ui_video_v_creat_menu();
    MT_ASSERT(p_menu != NULL);

    /*!
     * Create help
     */
    _ui_video_v_creat_help(p_menu);

    /*!
     * Create path
     */
    _ui_video_v_creat_path(p_menu);

    /*!
     * Create group
     */
    _ui_video_v_creat_group(p_menu);

#ifdef USB_FILEPLAY_PREVIEW
    /*!
     * Create preview
     */
    _ui_video_v_creat_preview(p_menu);
#endif

    /*!
     * Create list and scrollbar
     */
    p_sbar = _ui_video_v_creat_sbar(p_menu);
    MT_ASSERT(p_sbar != NULL);

    _ui_video_v_creat_list(p_menu, p_sbar);
    //ui_time_enable_heart_beat(TRUE);
    /*!
     * Create play container,and play info
     */
    p_play = _ui_video_v_creat_play_container(p_menu);
    MT_ASSERT(p_play != NULL);

    _ui_video_v_creat_play_file_name(p_play);
    _ui_video_v_creat_play_file_size(p_play);
    _ui_video_v_creat_play_file_pixel(p_play);
    _ui_video_v_creat_play_cur_time(p_play);
    _ui_video_v_creat_play_total_time(p_play);
    _ui_video_v_creat_play_icon(p_play);
    _ui_video_v_create_unsupport(p_menu);

    /*!
     * Show logo and set flag, it will be stopped before play video
     */
   // ui_show_logo(LOGO_BLOCK_ID_M1);
    preview_state = TRUE;

    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

    _ui_video_v_init();

    //is_usb = ui_video_m_get_is_usb();
#ifdef ENABLE_NETWORK
  if (is_usb)
#endif
	{
    if(g_partition_cnt == 1)
    {
     // ui_video_m_set_is_usb(0);
    }
    if(g_partition_cnt == 0)
    {
      dlg_data.parent_root = ROOT_ID_USB_FILEPLAY;
      dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
      dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
      dlg_data.y = ((SCREEN_HEIGHT- 160) / 2);
      dlg_data.w = 400;
      dlg_data.h = 160;
      dlg_data.content = IDS_WAIT_PLEASE;
      dlg_data.dlg_tmout = 0;
      ui_comm_dlg_open(&dlg_data);
      //ui_evt_disable_ir();
    }
  }
  return SUCCESS;
}
/*
static RET_CODE on_net_mount_success(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
 control_t *p_list;
 
 if(ROOT_ID_USB_FILEPLAY ==fw_get_focus_id())
 {
    //ui_evt_enable_ir();
    ui_comm_dlg_close();
    p_list = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_LIST);
   // _ui_video_v_video_info_clear(p_list);
//    ui_video_m_net_load(ip_name);
    _ui_video_v_rebuild_list(p_list);  
 } 
    return SUCCESS;
}

static RET_CODE on_net_mount_fail(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  control_t *p_list;
  p_list = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_LIST);
  //_ui_video_v_video_info_clear(p_list);
  
  //ui_evt_enable_ir();
  ui_comm_dlg_close();
  return SUCCESS;
}

*/
/* Bug 33045 */
static RET_CODE release_video_play(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  //if(g_exit_type != EXIT_TYPE_EXIT_NONE)
  //  ui_video_v_destroy();
  #ifndef MULTI_PIC_ADV
    ui_show_watermark();
  #endif
  #ifdef ENABLE_ADS
    ui_adv_pic_init(PIC_SOURCE_BUF);
  #endif

  g_exit_type = EXIT_TYPE_EXIT_NONE;
  return ERR_NOFEATURE;
}

static RET_CODE  _ui_video_v_unsupported_timeout(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  u16 idx = 0,focus = 0;
  RET_CODE ret =0;
  fw_tmr_destroy(ROOT_ID_USB_FILEPLAY, MSG_VIDEO_V_DISAPPEAR_UNSUPPORT);
  
  if(fw_get_focus_id() == ROOT_ID_USB_FILEPLAY)
  {
    //ui_video_c_play_end();
    
    ret=ui_video_m_get_next_file_idx(&idx);
  
    if(ret != SUCCESS)
    {
      return SUCCESS;
    }

    _ui_video_v_play_by_focus(idx);
    
    _ui_video_v_set_cur_playing_file_focus(p_list);

    focus = list_get_focus_pos(p_list);

    _ui_video_v_update_preview_info(focus);
    
    list_select_item(p_list, focus);
    
    ctrl_paint_ctrl(p_list, TRUE);
  }
  else
  {
  }
  return SUCCESS;
}

static RET_CODE _ui_video_v_on_file_task_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  b_video_v_file_is_exit = TRUE;
  
  if(g_exit_type != EXIT_TYPE_EXIT_NONE)
  {
    ui_video_v_destroy();
  }
  if(g_exit_type == EXIT_TYPE_EXIT_ALL)
  {
    ui_close_all_mennus();
  }
  else if(g_exit_type == EXIT_TYPE_EXIT)
  {
    manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
  }
  return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_MSGPROC(_ui_video_v_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_video_v_list_select)
    ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
    ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_exit_all)
    ON_COMMAND(MSG_VIDEO_V_PAUSE_PLAY, _ui_video_v_pause_resume)
    ON_COMMAND(MSG_VIDEO_V_STOP, _ui_video_v_stop)
    ON_COMMAND(MSG_VOLUME_DOWN, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VOLUME_UP, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VIDEO_V_CHANGE_PARTITION, _ui_video_v_change_partition)

    // msg up
    ON_COMMAND(MSG_VIDEO_V_FULLSCREEN, _ui_video_v_fullscreen)
    ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_exit_all)
    ON_COMMAND(MSG_VIDEO_V_UPFOLDER, _ui_video_v_up_folder)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_RESOLUTION, _ui_video_v_update_resolution)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_update_cur_time)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_on_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_unsupported_video)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_on_eos)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, _ui_video_v_unsupported_video)
    ON_COMMAND(MSG_VIDEO_EVENT_FULLSCREEN_CLOSE, _ui_video_v_on_fullscreen_close)
    ON_COMMAND(MSG_VIDEO_V_DISAPPEAR_UNSUPPORT, _ui_video_v_unsupported_timeout)
    ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_on_file_task_exit)
END_MSGPROC(_ui_video_v_list_proc, list_class_proc)

BEGIN_KEYMAP(_ui_video_v_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_BACK, MSG_EXIT)
    ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
    ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_PAUSE, MSG_VIDEO_V_PAUSE_PLAY)
    ON_EVENT(V_KEY_PLAY, MSG_VIDEO_V_PAUSE_PLAY)
    ON_EVENT(V_KEY_STOP, MSG_VIDEO_V_STOP)
    ON_EVENT(V_KEY_RECALL, MSG_VIDEO_V_UPFOLDER)
    ON_EVENT(V_KEY_GREEN, MSG_VIDEO_V_CHANGE_PARTITION)
END_KEYMAP(_ui_video_v_list_keymap, NULL)

#ifdef ENABLE_NETWORK
BEGIN_MSGPROC(_ui_video_network_v_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_video_v_list_select)
    ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
    ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_exit_all)
    ON_COMMAND(MSG_BACK_TO_NETWORK_PLACE, _ui_video_v_exit_to_network_place)
    ON_COMMAND(MSG_VOLUME_DOWN, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VOLUME_UP, _ui_video_v_change_volume)
    ON_COMMAND(MSG_SWITCH_MEDIA_TYPE, _ui_video_change)
    ON_COMMAND(MSG_VIDEO_V_PAUSE_PLAY, _ui_video_v_pause_resume)

    // msg up
    ON_COMMAND(MSG_VIDEO_V_FULLSCREEN, _ui_video_v_fullscreen)
    ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_exit_all)
    ON_COMMAND(MSG_VIDEO_V_UPFOLDER, _ui_video_v_up_folder)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_RESOLUTION, _ui_video_v_update_resolution)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_update_cur_time)
 //   ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_on_stop_cfm)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_on_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_on_eos)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY, _ui_video_v_unsupported_video)
    ON_COMMAND(MSG_VIDEO_EVENT_FULLSCREEN_CLOSE, _ui_video_v_on_fullscreen_close)
END_MSGPROC(_ui_video_network_v_list_proc, list_class_proc)

BEGIN_KEYMAP(_ui_video_network_v_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
    ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_RECALL, MSG_VIDEO_V_UPFOLDER)
    ON_EVENT(V_KEY_YELLOW, MSG_SWITCH_MEDIA_TYPE)
    ON_EVENT(V_KEY_GREEN, MSG_BACK_TO_NETWORK_PLACE)    
   ON_EVENT(V_KEY_PAUSE, MSG_VIDEO_V_PAUSE_PLAY)
   ON_EVENT(V_KEY_PLAY, MSG_VIDEO_V_PAUSE_PLAY)
END_KEYMAP(_ui_video_network_v_list_keymap, NULL)
#endif

BEGIN_KEYMAP(_ui_video_v_cont_keymap, NULL)

END_KEYMAP(_ui_video_v_cont_keymap, NULL)

BEGIN_MSGPROC(_ui_video_v_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, release_video_play)
END_MSGPROC(_ui_video_v_cont_proc, ui_comm_root_proc)
