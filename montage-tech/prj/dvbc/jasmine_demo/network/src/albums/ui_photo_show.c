/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"

#include "albums_filter.h"

#include "albums_public.h"

#include "ui_photo_show.h"

#include "ui_albums_api.h"

#include "ui_picture_api.h"

#include "ui_pic_play_mode_set.h"

#include "ui_albums.h"

typedef enum
{
  IDC_PHOTO_SHOW_PAUSE = 1,
}photos_id_t;

enum local_msg
{
  MSG_GREEN= MSG_LOCAL_BEGIN + 400,
  MSG_BLUE,
  MSG_RED,
  MSG_YELLOW,
  MSG_PHOTO_TIMER,
  MSG_SLIDE_PAUSE,
  MSG_SLIDE_RESUME,
};

enum comm_photo_msg
{
  MSG_SLIDE_SHOW_PIC = MSG_LOCAL_BEGIN + 600,
  MSG_SHOW_PIC_TIMEOUT,
};
static mul_pic_param_t pic_param = {{0},};
static photos_t *p_photos = NULL;
static s16 photo_id = 0;
static u16 pic_flip = 0;
static BOOL g_photo_slide_pause = FALSE;
static rect_t g_photo_rect = {(SCREEN_WIDTH - OSD0_WIDTH) / 2,
                              (SCREEN_HEIGHT - OSD0_HEIGHT) / 2,
                              (SCREEN_WIDTH + OSD0_WIDTH) / 2,
                              (SCREEN_HEIGHT + OSD0_HEIGHT) / 2};

//cont
#define PHOTO_SHOW_CONT_X  0
#define PHOTO_SHOW_CONT_Y  0
#define PHOTO_SHOW_CONT_W  SCREEN_WIDTH //960
#define PHOTO_SHOW_CONT_H  SCREEN_HEIGHT //600

#define PHOTO_SHOW_PAUSEX  (PHOTO_SHOW_CONT_W - PHOTO_SHOW_PAUSEW - 50)
#define PHOTO_SHOW_PAUSEY  10
#define PHOTO_SHOW_PAUSEW  56
#define PHOTO_SHOW_PAUSEH  56
#define RAND(x)    ((u16)mtos_ticks_get()%((u16)x)) 
#define REND_ANIM_MAX (REND_ANIM_RAND+1)

u16 photo_show_menu_keymap(u16 key);
RET_CODE photo_show_menu_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


static void show_photo_timeout_tmr_create(void)
{
  if(pic_play_get_slide_show_sts())
  {
    fw_tmr_create(ROOT_ID_PHOTO_SHOW, MSG_SHOW_PIC_TIMEOUT, 20 * 1000, FALSE);
  }
}

static void show_photo_timeout_tmr_destroy()
{
  fw_tmr_destroy(ROOT_ID_PHOTO_SHOW, MSG_SHOW_PIC_TIMEOUT);
}



RET_CODE open_photo_show(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_pause = NULL;
  
  if(para2 == 0)
  {
    return ERR_FAILURE;
  }
  
  photo_id = (s16)para1;
  
  p_photos = ui_albums_get_photos_list();

  g_photo_slide_pause = FALSE;
  
  //create menu.
  p_menu = fw_create_mainwin(ROOT_ID_PHOTO_SHOW,
    PHOTO_SHOW_CONT_X, PHOTO_SHOW_CONT_Y,
    PHOTO_SHOW_CONT_W, PHOTO_SHOW_CONT_H, 0, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_menu != NULL);
  ctrl_set_keymap(p_menu, photo_show_menu_keymap);
  ctrl_set_proc(p_menu, photo_show_menu_proc);
  ctrl_set_rstyle(p_menu, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_pause = ctrl_create_ctrl(CTRL_BMAP, IDC_PHOTO_SHOW_PAUSE,
    PHOTO_SHOW_PAUSEX, PHOTO_SHOW_PAUSEY,
    PHOTO_SHOW_PAUSEW, PHOTO_SHOW_PAUSEH, p_menu, 0);
  ctrl_set_sts(p_pause, OBJ_STS_HIDE);
  

  ctrl_default_proc(p_pause, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  pic_stop();
  
  ui_pic_set_url((u8 *)(p_photos->p_photo[photo_id].p_photo_url));

  pic_flip = PIC_NO_F_NO_R;
  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = ((SCREEN_WIDTH - OSD0_WIDTH) / 2);
  pic_param.win_rect.top = ((SCREEN_HEIGHT - OSD0_HEIGHT) / 2);
  pic_param.win_rect.right = ((SCREEN_WIDTH + OSD0_WIDTH) / 2);
  pic_param.win_rect.bottom = ((SCREEN_HEIGHT + OSD0_HEIGHT) / 2);
  pic_param.file_size = 0;
  pic_param.handle = 0;
  pic_param.is_thumb = FALSE;
  
  pic_start(&pic_param);   
  show_photo_timeout_tmr_create();
  ui_comm_showdlg_open(NULL, IDS_LOADING_WITH_WAIT, NULL, 0);

  return SUCCESS;
}

static void photo_rotate(control_t *p_cont, BOOL is_clock)
{
  control_t *p_pause = NULL;

  p_pause = ctrl_get_child_by_id(p_cont, IDC_PHOTO_SHOW_PAUSE);

  if(pic_flip != PIC_NO_F_NO_R)
  {
    bmap_set_content_by_id(p_pause, is_clock ? IM_SWORD_R : IM_SWORD_L);
  }
  else
  {
    bmap_set_content_by_id(p_pause, IM_PAUSE);
  }
  
  g_photo_slide_pause = TRUE;
  
  ctrl_set_sts(p_pause, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_pause, TRUE);
}

static void photo_pause(control_t *p_cont, BOOL is_pause)
{
  control_t *p_pause = NULL;

  p_pause = ctrl_get_child_by_id(p_cont, IDC_PHOTO_SHOW_PAUSE);

  bmap_set_content_by_id(p_pause, IM_PAUSE);

  if(is_pause)
  {
    g_photo_slide_pause = TRUE;
    ctrl_set_sts(p_pause, OBJ_STS_SHOW);
    
    ctrl_paint_ctrl(p_pause, TRUE);
  }
  else
  {
    g_photo_slide_pause = FALSE;
    ctrl_set_sts(p_pause, OBJ_STS_HIDE);
    
    ctrl_erase_ctrl(p_pause);
  }
}

static void photo_tmr_destroy(void)
{
  ui_comm_dlg_close2();
  
  fw_tmr_destroy(ROOT_ID_PHOTO_SHOW, MSG_SLIDE_SHOW_PIC);
}

static void photo_tmr_create(void)
{
  u32 tm_out = 0;

  if(pic_play_get_slide_show_sts())
  {
    tm_out = pic_play_get_slide_interval();
    
    fw_tmr_create(ROOT_ID_PHOTO_SHOW, MSG_SLIDE_SHOW_PIC, tm_out * 1000, FALSE);
  }
}

static RET_CODE photo_show_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  photo_tmr_destroy();
  show_photo_timeout_tmr_destroy();
  pic_stop();
  ui_pic_clear_all(0);
  if(fw_find_root_by_id(ROOT_ID_ALBUMS))
  {
  fw_notify_root(fw_find_root_by_id(ROOT_ID_ALBUMS), NOTIFY_T_MSG, FALSE, MSG_UPDATE, 1, (u32)(int)(++photo_id));
  }
  return ERR_NOFEATURE;
}

static RET_CODE photo_show_draw_start(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_comm_dlg_close2();
  return SUCCESS;
}


static RET_CODE photo_show_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  show_photo_timeout_tmr_destroy();
  ui_comm_dlg_close2();
  if(g_photo_slide_pause)
  {
    return SUCCESS;
  }
  photo_tmr_create();
  return SUCCESS;
}

static RET_CODE photo_show_prev_pic(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(g_photo_slide_pause)
  {
    return SUCCESS;
  }

  fw_tmr_destroy(ROOT_ID_PHOTO_SHOW, MSG_SLIDE_SHOW_PIC);
  
  pic_stop();
  
  if(pic_play_get_special_effect() == TRUE)
  {
    pic_param.anim= (rend_anim_t)RAND(REND_ANIM_MAX);
  }
  else
  {
    pic_param.anim = REND_ANIM_NONE;
  }

  photo_id--;

  if(photo_id >= 0)
  {
    ui_pic_set_url((u8 *)(p_photos->p_photo[photo_id].p_photo_url));

    pic_start(&pic_param);   
  }
  else
  {
    if(p_photos->page != 1)
    {
      photo_id = (s16)(p_photos->per_page -1);
      albums_ext_prev_page();
    }
    else
    {
      photo_id = 0;
    }
  }

  return SUCCESS;
}


static RET_CODE on_show_pic_timeout(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ctrl_process_msg(p_cont, MSG_SLIDE_SHOW_PIC, 0, 0);
  return SUCCESS;
}

static RET_CODE photo_show_next_pic(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{   
  pic_showmode_t param;
  if(g_photo_slide_pause)
  {
    return SUCCESS;
  }
  fw_tmr_destroy(ROOT_ID_PHOTO_SHOW, MSG_SLIDE_SHOW_PIC);
  fw_tmr_destroy(ROOT_ID_PHOTO_SHOW, MSG_SHOW_PIC_TIMEOUT);
  
  pic_stop();
  
  if(pic_play_get_special_effect() == TRUE)
  {
    pic_param.anim= (rend_anim_t)RAND(REND_ANIM_MAX);
  }
  else
  {
    pic_param.anim = REND_ANIM_NONE;
  }
  sys_status_get_pic_showmode(&param, FALSE);
  
  photo_id++;
  if(photo_id >= (s16)p_photos->photo_total)
    {    
      if( FALSE ==  param.slide_repeat )
     {
        photo_id --;
        manage_close_menu(ROOT_ID_PHOTO_SHOW, 0, 0);//exit.
        return SUCCESS;
     }
     else
     {
        photo_id = 0;
     }
    }
  if(photo_id < (s16)p_photos->photo_total)
  {
    if(photo_id < p_photos->cnt)
    {
      ui_pic_set_url((u8 *)(p_photos->p_photo[photo_id].p_photo_url));

      pic_start(&pic_param);   
      show_photo_timeout_tmr_create();
    }
    else
    {
      RET_CODE ret = SUCCESS;
      ret = albums_ext_next_page();
      if(ret != SUCCESS )
      {
       // pic_showmode_t param;
      //  sys_status_get_pic_showmode(&param, FALSE);
        if( FALSE ==  param.slide_repeat )
        {
          
          photo_id --;
          manage_close_menu(ROOT_ID_PHOTO_SHOW, 0, 0);//exit.
          return SUCCESS;
         }        
	  ret = albums_ext_first_page();
	  OS_PRINTF("the result is %d\n",ret);
      }
      photo_id = 0;
    }
  }
  /*else //bug 33979
  {
    //return to upper menu. 
    photo_id --;
    manage_close_menu(ROOT_ID_PHOTO_SHOW, 0, 0);
  }*/

  return SUCCESS;
}
static RET_CODE photo_change_focus(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  photo_tmr_destroy();
  show_photo_timeout_tmr_destroy();

  pic_stop();
  
  if(pic_play_get_special_effect())
  {
    pic_param.anim= (rend_anim_t)RAND(REND_ANIM_MAX);
  }
  else
  {
    pic_param.anim = REND_ANIM_NONE;
  }
  switch(msg)
  {
    case MSG_FOCUS_UP:
      photo_show_prev_pic(p_cont, msg, para1, para2);
      break;
    case MSG_FOCUS_DOWN:
      photo_show_next_pic(p_cont, msg, para1, para2);
      break;
     default:
       break;
  }

  return SUCCESS;
}

static RET_CODE photo_show_rotate(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  switch(pic_flip)
  {
    case PIC_NO_F_NO_R:
      pic_flip = ((msg == MSG_FOCUS_RIGHT) ? PIC_R_270 : PIC_R_90);
      break;

    case PIC_R_90:
      pic_flip = ((msg == MSG_FOCUS_RIGHT) ? PIC_NO_F_NO_R : PIC_R_180);
      break;

    case PIC_R_180:
      pic_flip = ((msg == MSG_FOCUS_RIGHT) ? PIC_R_90 : PIC_R_270);
      break;

    case PIC_R_270:
      pic_flip = ((msg == MSG_FOCUS_RIGHT) ? PIC_R_180 : PIC_NO_F_NO_R);
      break;

    default:
      MT_ASSERT(0);
      break;
  }  

  pic_stop();
  ui_pic_set_url((u8 *)(p_photos->p_photo[photo_id].p_photo_url));

  pic_param.flip = (pic_flip_rotate_type_t)pic_flip;
  
  pic_start(&pic_param);   

  photo_rotate(p_cont, (BOOL)(msg == MSG_FOCUS_RIGHT));
        
  return SUCCESS;
}

static RET_CODE photo_show_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{    
  return SUCCESS;
}

static RET_CODE photo_show_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  photos_t *p_photoes = NULL;
  
  if(fw_get_focus_id() != ROOT_ID_PHOTO_SHOW)
  {
    return ERR_FAILURE;
  }  

  if(para1 != ALBUMS_EVT_PHOTOS_LIST)
  {
    return ERR_FAILURE;
  }
  
  if(para2 == 0)
  {
    return ERR_FAILURE;
  }

  p_photoes = (photos_t *)para2;
  if(p_photoes->cnt == 0)
  {
    return ERR_FAILURE;
  }

  pic_stop();
  
  ui_pic_set_url((u8 *)(p_photoes->p_photo[photo_id].p_photo_url));

  pic_start(&pic_param);        
  show_photo_timeout_tmr_create();

  return SUCCESS;
}

static RET_CODE photo_show_slide_resume(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *media;
  
  photo_tmr_destroy();
  
  photo_pause(p_cont, FALSE);
  
  pic_resume(&media, &g_photo_rect);

  photo_tmr_create();

  return SUCCESS;
}

static RET_CODE photo_show_slide_pause(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  if(g_photo_slide_pause)
  {
    media_fav_t *media;
    
    photo_tmr_destroy();

    photo_pause(p_cont, FALSE);
    
    pic_resume(&media, &g_photo_rect);

    photo_tmr_create();
  }
  else
  {
    photo_tmr_destroy();

    photo_pause(p_cont, TRUE);
    pic_stop();
    pic_start(&pic_param);   
    pic_pause();
  }

  return SUCCESS;
}
static void exit_album(void)
{  
  control_t* root = ui_comm_root_get_root(ROOT_ID_PHOTO_SHOW);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
}

static RET_CODE check_net_conn_sts(void)
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32  
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE)
     && (eth_connt_stats.is_3g_conn == FALSE) && (eth_connt_stats.is_gprs_conn == FALSE))
  {
     ret = ERR_FAILURE;  
  }
#endif
  return ret;
}
static RET_CODE photo_show_config_plug(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u16 stringid = 0;
  if(check_net_conn_sts() == SUCCESS)
  {
    return SUCCESS;
  }
  switch(msg)
  {
    case MSG_PLUG_OUT:
        if(para2 == HP_WIFI)
        {
            OS_PRINTF("####%s, wifi plug out####\n", __FUNCTION__);
            stringid = IDS_WIFI_DEV_NOT_EXIST;  
            ui_comm_cfmdlg_open(NULL, stringid, exit_album, 2000);
        }
        break;
    case MSG_INTERNET_PLUG_OUT:
        OS_PRINTF("####%s, ethernet plug out####\n", __FUNCTION__);
        stringid = IDS_LAN_DEV_NOT_EXIST;  
        ui_comm_cfmdlg_open(NULL, stringid, exit_album, 2000);
        break;
    default:
	 break;
  }
  return SUCCESS;
}



BEGIN_KEYMAP(photo_show_menu_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_PAUSE, MSG_SLIDE_PAUSE)
  ON_EVENT(V_KEY_PLAY, MSG_SLIDE_RESUME) 
END_KEYMAP(photo_show_menu_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(photo_show_menu_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_RIGHT, photo_show_rotate)
  ON_COMMAND(MSG_FOCUS_LEFT, photo_show_rotate)
  ON_COMMAND(MSG_FOCUS_UP, photo_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, photo_change_focus)  
  ON_COMMAND(MSG_DESTROY, photo_show_on_destory)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, photo_show_draw_end)  
  ON_COMMAND(MSG_PIC_EVT_DRAW_START, photo_show_draw_start)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, photo_show_draw_end)
  ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, photo_show_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, photo_show_draw_end)  
  ON_COMMAND(MSG_SLIDE_SHOW_PIC, photo_show_next_pic)  
  ON_COMMAND(MSG_SHOW_PIC_TIMEOUT, on_show_pic_timeout)  
  ON_COMMAND(MSG_ALBUMS_EVT_FAILED, photo_show_failed)
  ON_COMMAND(MSG_ALBUMS_EVT_SUCCESS, photo_show_success)
  ON_COMMAND(MSG_SLIDE_PAUSE, photo_show_slide_pause)
  ON_COMMAND(MSG_SLIDE_RESUME, photo_show_slide_resume)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, photo_show_config_plug)
  ON_COMMAND(MSG_PLUG_OUT, photo_show_config_plug)
END_MSGPROC(photo_show_menu_proc, ui_comm_root_proc)

