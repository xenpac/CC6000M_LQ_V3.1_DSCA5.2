/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#ifdef ENABLE_MUSIC_PIC_CONFIG
#include "ui_usb_picture.h"
#include "ui_timer.h"
#include "lib_char.h"
#include "ui_rename.h"
#include "ui_mute.h"
#include "ui_volume_usb.h"
#include "ui_picture.h"
#include "ui_usb_music.h"
#include "ui_signal_api.h"
#ifdef ENABLE_NETWORK
#include "ui_edit_usr_pwd.h"
#include "pnp_service.h"
#endif
#ifdef ENABLE_ADS
#include "ui_ad_api.h"
#endif


//lint -e550 -e746


typedef enum
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_BLUE,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_MUSIC_PLAY,
  MSG_MUSIC_RESUME,
  MSG_MUSIC_PAUSE,
  MSG_MUSIC_NEXT,
  MSG_MUSIC_PRE,
  MSG_MUSIC_STOP,
  MSG_ADD_TO_LIST,
  MSG_ONE_SECOND_ARRIVE,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_SORT,
  MSG_ADD_FAV,
  MSG_FULLSCREEN,
#ifdef ENABLE_NETWORK
  MSG_SWITCH_MEDIA_TYPE,
  MSG_PIC_UPFOLDER,
#endif
}usb_picture_msg_t;

typedef enum
{
  IDC_PIC_PREVIEW = 1,
  IDC_PIC_MSG,
  IDC_PIC_DETAIL_CONT,
  IDC_PIC_GROUP_ARROWL,
  IDC_PIC_GROUP_ARROWR,
  IDC_PIC_GROUP,
  IDC_PIC_LIST,
  IDC_PIC_SBAR,
  IDC_PIC_PATH,
  IDC_PIC_FAV_LIST_CONT,
  IDC_PIC_SORT_LIST,
  IDC_PIC_HELP,
}usb_picture_ctrl_id_t;

typedef enum
{
  IDC_PIC_DETAIL_PIXEL = 1,
  IDC_PIC_DETAIL_FILE_SIZE,
}usb_picture_list_detail_id;

typedef enum
{
  IDC_PIC_FAV_LIST_TITLE= 1,
  IDC_PIC_FAV_LIST_LIST,
  IDC_PIC_FAV_LIST_HELP,
  IDC_PIC_FAV_LIST_SBAR,
}pic_play_list_ctrl_id_t;


static list_xstyle_t  pic_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_PBACK,
  RSI_ITEM_1_HL,
};


static list_xstyle_t mp_list_field_fstyle = //fixed font color
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLUE,
  FSI_WHITE,
};

static list_xstyle_t mp_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t mp_list_attr[PIC_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    50, 10, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 60, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    340, 100, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_play_list_attr[PIC_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    50, 10, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    40, 60, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT,
    280, 100, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_LEFT,
    40, 380, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_sort_list_field_attr[PIC_SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    200, 15, 0, &mp_list_field_rstyle, &mp_list_field_fstyle },
};

static rect_t g_pic_rect = {
    COMM_BG_X + COMM_WIN_SHIFT_X + PIC_PREV_X + 6,
    COMM_BG_Y + COMM_WIN_SHIFT_Y + PIC_PREV_Y + 6,
    COMM_BG_X + COMM_WIN_SHIFT_X + PIC_PREV_X + PIC_PREV_W - 12,
    COMM_BG_Y + COMM_WIN_SHIFT_Y + PIC_PREV_Y + PIC_PREV_H - 12,
    };

static rect_t p_dlg_rc =
{
  PIC_DLG_L,
  PIC_DLG_T,
  PIC_DLG_R,
  PIC_DLG_B,
};

static file_list_t g_list = {0};
static u16 g_ffilter_all[64] = {0};
static partition_t *p_partition = NULL;
static flist_dir_t g_pic_flist_dir = NULL;
static u32 g_partition_cnt = 0;
static utc_time_t cur_play_time = {0};
static utc_time_t total_play_time = {0};
static roll_param_t roll_param = {ROLL_LR, ROLL_SINGLE, 0, FALSE};
static u16 g_picture_player_preview_focus = 0;
static BOOL is_clear_del_icon = TRUE;

#ifdef ENABLE_NETWORK
static u32 is_usb = 1;
static char ipaddress[128] = "";
static u16 ip_address_with_path[32] ={0};
#endif
static BOOL g_backup_roll_status;


static RET_CODE pic_list_update(control_t* p_list, u16 start, u16 size, u32 context);
static RET_CODE pic_fav_list_update(control_t* p_list, u16 start, u16 size, u32 context);

RET_CODE pic_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 pic_list_keymap(u16 key);
RET_CODE pic_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);


u16 pic_fav_list_list_keymap(u16 key);
RET_CODE pic_fav_list_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 pic_sort_list_keymap(u16 key);
RET_CODE pic_sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

#ifdef ENABLE_NETWORK
u16 pic_network_list_keymap(u16 key);

RET_CODE pic_network_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

extern RET_CODE pnp_svc_unmount(u16 *p_url);

u32 get_picture_is_usb()//if is_usb is 0 means picture entry from samba
{
  return is_usb;
}

static RET_CODE on_pic_process_msg(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL;
  control_t *p_cont = NULL;
  p_list_cont = ctrl_get_parent(p_list);
  if(MSG_PIC_BACK_TO_NETWORK_PLACE == msg)
  {
    manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
    manage_open_menu(ROOT_ID_NETWORK_PLACES, 0, 0);
  }

  p_cont = ui_comm_root_get_root(ROOT_ID_USB_PICTURE);
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  
  return SUCCESS;
}
#endif

static RET_CODE pic_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
#ifdef ENABLE_NETWORK
  if (! is_usb)
  {
    // u8 ipaddress[128] = "";
    u8 ip[128] = "\\\\";
    // sprintf(ipaddress, "%d.%d.%d.%d",ip_address.s_b1,ip_address.s_b2,ip_address.s_b3,ip_address.s_b4);
    strcat(ip,ipaddress);

    //str_asc2uni(p_partition[focus].name, p_str);
    dvb_to_unicode(ip,max_length - 1, p_str, max_length);
  }
  else
#endif
  {
	  uni_strncpy(p_str, p_partition[focus].name, max_length);
  } 
  return SUCCESS;
}


static flist_sort_t pic_get_sort_type( control_t *p_list )
{
    control_t *p_parent, *p_sort_list;
    u16 pos;
    flist_sort_t sort_type = DEFAULT_ORDER_MODE;
    p_parent = ctrl_get_parent(p_list);
    p_sort_list = ctrl_get_child_by_id(p_parent,IDC_PIC_SORT_LIST);
    pos = list_get_focus_pos(p_sort_list);
    switch(pos)
    {
      case 0:
        sort_type = DEFAULT_ORDER_MODE;
        break;
    
      case 1:
        sort_type = A_Z_MODE;
        break;
    
      case 2:
        sort_type = Z_A_MODE;
        break;
    
      default:
        sort_type = DEFAULT_ORDER_MODE;
        break;
    }
    return sort_type;
}

static void pic_update_detail(control_t *p_cont, u32 width, u32 height, BOOL is_paint, BOOL is_clear)
{
  control_t *p_detail_cont = NULL;
  control_t *p_fsize = NULL, *p_pixel = NULL, *p_list = NULL, *p_list_cont = NULL;
  u8 asc_str[256];
  u32 file_size = 0;
  media_fav_t *p_media = NULL;

  p_detail_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_DETAIL_CONT);

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_FAV_LIST_LIST);
  }
  else
  {
    p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);
  }

  list_get_focus_pos(p_list);

  p_fsize = ctrl_get_child_by_id(p_detail_cont, IDC_PIC_DETAIL_FILE_SIZE);
  p_pixel = ctrl_get_child_by_id(p_detail_cont, IDC_PIC_DETAIL_PIXEL);

  if(!is_clear)
  {
    ui_pic_get_cur(&p_media);
    if(p_media == NULL)
    {
        OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
        return;
    }

    //set file size.
    file_size = (u32)file_list_get_file_size(p_media->path);

    memset(asc_str, 0, sizeof(asc_str));
    ui_conver_file_size_unit(file_size / 1024, asc_str);

    text_set_content_by_ascstr(p_fsize, asc_str);

    //set width & height.
    sprintf((char *)asc_str, "%d x %d", (int)width, (int)height);
    text_set_content_by_ascstr(p_pixel, asc_str);
  }
  else
  {
    text_set_content_by_ascstr(p_fsize, (u8 *)" ");
    text_set_content_by_ascstr(p_pixel, (u8 *)" ");
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_fsize, TRUE);
    ctrl_paint_ctrl(p_pixel, TRUE);
  }
}

static void pic_fav_list_update_help(control_t *p_mbox, BOOL is_paint)
{
  u8 i;
  u16 icon[PIC_HELP_MBOX_TOL] =
    {
      IM_EPG_COLORBUTTON_RED,
      IM_EPG_COLORBUTTON_YELLOW,
      IM_EPG_COLORBUTTON_GREEN,
    };
  u16 str[PIC_HELP_MBOX_TOL] =
    {
      IDS_PLAY,
      IDS_DELETE,
      IDS_DELETE_ALL,
    };

  for(i = 0; i < PIC_PLAY_LIST_HELP_MBOX_TOL; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, str[i]);
    mbox_set_content_by_icon(p_mbox, i, icon[i], icon[i]);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_mbox, TRUE);
  }
}

static void pic_update_path(control_t *p_list)
{
  control_t *p_path;
  u16 uni_str[MAX_FILE_PATH] = {0};

  p_path = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PIC_PATH);

  //dvb_to_unicode(,
  //  MAX_FILE_PATH - 1, uni_str, MAX_FILE_PATH);

  uni_strncpy(uni_str, file_list_get_cur_path(g_pic_flist_dir), MAX_FILE_PATH);
  text_set_content_by_unistr(p_path, uni_str);

  ctrl_paint_ctrl(p_path, TRUE);
}

static BOOL pic_file_list_is_enter_dir(u16 *p_filter, u16 unit_cnt, u16 *p_path)
{
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};
  u16 cur_path[255] = {0};
  MT_ASSERT(p_path != NULL);

  uni_strcpy(cur_path, p_path);

  p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);

  if (p_temp != NULL)
  {
    //parent dir
    str_asc2uni((u8 *)"..", parent);
    str_asc2uni((u8 *)".", curn);
    if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
      p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);
      if (p_temp != NULL)
      {
        p_temp[0] = 0/*'\0'*/;
      }
    }
    //cur dir
    else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

static void pic_file_list_update(control_t *p_list)
{
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  sort_type = pic_get_sort_type(p_list);

  file_list_leave_dir(g_pic_flist_dir);
  g_pic_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, g_list.p_file[0].name);
  if(g_pic_flist_dir != NULL)
  {
     file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
     file_list_sort(&g_list, (int)g_list.file_count, sort_type);
     list_set_count(p_list, (u16)(g_list.file_count), PIC_LIST_PAGE);
     list_set_focus_pos(p_list, 0);
     pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
     ui_pic_build_play_list_indir(&g_list);
     ctrl_paint_ctrl(p_list, TRUE);
     pic_update_path(p_list);
  }
}

static RET_CODE on_pic_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL;
  BOOL is_enter_dir = FALSE;
  p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);

#ifdef ENABLE_NETWORK
  if(!is_usb)
  {
    gui_stop_roll(p_list);
    ui_time_enable_heart_beat(FALSE);
    close_mute();
    //ui_get_ip_path_mount(&p_url);
    //pnp_service_vfs_unmount(p_url);

#ifndef MIN_AV_64M
    ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif

    return ERR_NOFEATURE;
  }
#endif

  if (g_pic_flist_dir != NULL &&  g_list.file_count != 0)
  {
    is_enter_dir = pic_file_list_is_enter_dir(g_ffilter_all, 100, g_list.p_file[0].name);
  }
  else
  {
    is_enter_dir = FALSE;
  }

  if(is_enter_dir)
  {
    pic_stop();
    ui_pic_clear(0);//fix exit can clear pic show
    pic_file_list_update(p_list);//simon fix bug #27346
    return SUCCESS;
  }
  else
  {
    pic_stop();
    gui_stop_roll(p_list);

    ui_time_enable_heart_beat(FALSE);
  }
  close_mute();
#ifndef MIN_AV_64M
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  return ERR_NOFEATURE;
}


static RET_CODE on_pic_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL, *p_list_cont = NULL, *p_fav_list = NULL;

  ui_init_signal();
  ui_ca_set_roll_status(g_backup_roll_status);

  if(g_pic_flist_dir != NULL)
  {
    file_list_leave_dir(g_pic_flist_dir);
  }

  p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);

  gui_stop_roll(p_list);

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT);

  p_fav_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_FAV_LIST_LIST);

  gui_stop_roll(p_fav_list);

  ui_enable_chk_pwd(TRUE);
  ui_pic_unload_fav_list();

  pic_stop();

  ui_pic_release();
  ui_deinit_play_timer();

#ifdef ENABLE_NETWORK
  if(!is_usb && (fw_find_root_by_id(ROOT_ID_USB_FILEPLAY) == NULL))
  {
    u16 *p_url;
    ui_get_ip_path_mount(&p_url);
    pnp_service_vfs_unmount(p_url);
  }
#endif
#ifndef MULTI_PIC_ADV
  ui_show_watermark();
#endif
#ifdef ENABLE_ADS
  ui_adv_pic_init(PIC_SOURCE_BUF);
#endif
  return ERR_NOFEATURE;
}

static RET_CODE on_pic_evt_update_size(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  pic_update_detail(p_cont, para1, para2, TRUE, FALSE);

  return SUCCESS;
}

static RET_CODE on_pic_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL;

  p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);
  if(p_list != NULL)
  {
    gui_stop_roll(p_list);
  }

  pic_stop();
#ifndef MIN_AV_64M
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  manage_close_menu(ROOT_ID_USB_PICTURE, 0, 0);

  return SUCCESS;
}

static RET_CODE on_pic_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL, *p_list = NULL;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;

  if(g_list.p_file == NULL)
  {
    return ERR_FAILURE;
  }

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_FAV_LIST_LIST);
  }
  else
  {
    p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);
  }

  pic_stop();

  pos = list_get_focus_pos(p_list);

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_media = ui_pic_get_fav_media_by_index(pos);
    if(p_media == NULL)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    ret = ui_pic_set_play_index_fav_by_name(p_media->path);
    if(ret == FALSE)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    ui_pic_play_curn(&p_media, &g_pic_rect);
  }
  else
  {
    if(g_list.p_file[pos].type == NOT_DIR_FILE)
    {
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
        OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
        return ERR_FAILURE;
      }
      ui_pic_play_curn(&p_media, &g_pic_rect);
    }
    else
    {
      pic_stop();
      ui_pic_clear(0);//fix exit from fav list to ...
    }
  }

  return SUCCESS;
}

static RET_CODE pic_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  u16 cnt = list_get_count(p_list);
  flist_type_t type = FILE_TYPE_UNKNOW;
  u16 icon_id = RSC_INVALID_ID;

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON */
      if(g_list.p_file[i + start].type != DIRECTORY)
      {
        type = flist_get_file_type(g_list.p_file[i + start].p_name);
        switch(type)
        {
          case FILE_TYPE_JPG:
            icon_id = IM_MP3_ICON_JPG;
            break;

          case FILE_TYPE_PNG:
            icon_id = IM_MP3_ICON_PNG;
            break;

          case FILE_TYPE_GIF:
            icon_id = IM_MP3_ICON_GIF;
            break;

          case FILE_TYPE_BMP:
            icon_id = IM_MP3_ICON_BMP;
            break;

          default:
            icon_id = RSC_INVALID_ID;
            break;
        }
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, icon_id);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, IM_MP3_ICON_FOLDER);
      }

      /* NAME */
      //dvb_to_unicode(g_list.p_file[i + start].p_name, MAX_FILE_PATH - 1, file_uniname, MAX_FILE_PATH);
      uni_strncpy(file_uniname, g_list.p_file[i + start].p_name, MAX_FILE_PATH);

      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);
    }

  }

  return SUCCESS;
}

static RET_CODE pic_fav_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  u16 icon_id = 0;
  u16 cnt = list_get_count(p_list);
  flist_type_t type = FILE_TYPE_UNKNOW;
  media_fav_t *p_media = NULL;

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    ui_pic_get_fav_list(&p_media);
  }

  for (i = 0; i<start && p_media!=NULL; i++)
  {
    p_media = p_media->p_next;
  }

  for (i = 0; i < size; i++)
  {
    if ((i + start < cnt) && (p_media != NULL) )
    {
      /* NO. */
      sprintf((char *)asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON TYPE*/
      type = flist_get_file_type(p_media->path);
      switch(type)
      {
        case FILE_TYPE_JPG:
          icon_id = IM_MP3_ICON_JPG;
          break;

        case FILE_TYPE_PNG:
          icon_id = IM_MP3_ICON_PNG;
          break;

        case FILE_TYPE_GIF:
          icon_id = IM_MP3_ICON_GIF;
          break;

        case FILE_TYPE_BMP:
          icon_id = IM_MP3_ICON_BMP;
          break;

        default:
          icon_id = RSC_INVALID_ID;
          break;
      }
      list_set_field_content_by_icon(p_list, (u16)(start + i), 1, icon_id);

      /* NAME */
      //str_asc2uni(p_media->path,file_uniname);
      //dvb_to_unicode(p_media->p_filename, MAX_FILE_PATH - 1, file_uniname, MAX_FILE_PATH);
      uni_strncpy(file_uniname, p_media->p_filename, MAX_FILE_PATH);

      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);

      /* ICON DEL*/
      if(ui_pic_is_one_fav_del(start + i))
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_TV_DEL);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, RSC_INVALID_ID);
      }
    }

    if(p_media!=NULL)
    {
      p_media = p_media->p_next;
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_set_play_mode(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  manage_open_menu(ROOT_ID_PIC_PLAY_MODE_SET, ROOT_ID_USB_PICTURE, 0);

  return ret;
}


static RET_CODE on_pic_list_change_focus(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret=SUCCESS;

  ret = list_class_proc(p_list, msg, 0, 0);

  list_get_focus_pos(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }
  ui_play_timer_start();
  if(ui_is_mute() ) //set mute
  {
    open_mute(0, 0);
  }

  return ret;
}

static RET_CODE on_pic_list_change_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cbox;
  u16 focus = 0;
  u16 count = 0;
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  sort_type = pic_get_sort_type(p_list);

  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PIC_GROUP);
  cbox_class_proc(p_cbox, MSG_INCREASE, 0, 0);

  focus = cbox_dync_get_focus(p_cbox);

  if(g_partition_cnt > 0)
  {
    if(g_pic_flist_dir == NULL)
       return ERR_FAILURE;

    file_list_leave_dir(g_pic_flist_dir);
    g_pic_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, p_partition[focus].letter);
    if(g_pic_flist_dir != NULL)
    {
       file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
       file_list_sort(&g_list, (int)g_list.file_count, sort_type);
       count = (u16)g_list.file_count;

       list_set_count(p_list, count, PIC_LIST_PAGE);
       list_set_focus_pos(p_list, 0);
       //list_select_item(p_list, 0);

       pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
       ui_pic_build_play_list_indir(&g_list);

       ctrl_paint_ctrl(p_list, TRUE);

       pic_update_path(p_list);
    }
    else
    {
       OS_PRINTF("\n file_list_enter_dir failure. \n");
    }
  }

  return SUCCESS;
}

#if 0
static RET_CODE on_pic_list_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_USB_PICTURE, para1);
    return SUCCESS;
}
#endif

static RET_CODE on_pic_add_to_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  int pos = 0;
  comm_dlg_data_t dlg_data = {0};

  pos = list_get_focus_pos(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  if(g_list.p_file[pos].type == NOT_DIR_FILE)
  {
    dlg_data.parent_root = ROOT_ID_USB_PICTURE;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = PIC_DLG_L;
    dlg_data.y = PIC_DLG_T;
    dlg_data.w = PIC_DLG_W;
    dlg_data.h = PIC_DLG_H;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;

    ui_comm_dlg_open(&dlg_data);

    if(ui_pic_add_one_fav(g_list.p_file[pos].name))
    {
      ui_pic_save_fav_list(g_list.p_file[0].name[0]);
    }
    //list_set_field_content_by_icon(p_list, pos, 1, IM_TV_FAV);

    //mtos_task_delay_ms(100);
    ui_comm_dlg_close();
  }

  //set HighLight to the next item
  //list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);
  //ctrl_process_msg(p_list, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_pic_open_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_fav_list_cont, *p_play_list, *p_play_list_title, *p_msg;
  u16 count=0;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;
  comm_dlg_data_t dlg_data_notexist =
  {
    ROOT_ID_USB_PICTURE,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    PIC_DLG_L,
    PIC_DLG_T,
    PIC_DLG_W,
    PIC_DLG_H,
    IDS_PLAY_LIST_NOT_EXIST,
    2000,
    FALSE,
    0,
    {0},
  };

  p_fav_list_cont = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PIC_FAV_LIST_CONT);
  p_play_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_LIST);
  p_play_list_title = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_TITLE);

  count = (u16)ui_pic_get_fav_count();
  if(count > 0)
  {
    ui_pic_set_play_mode(PIC_MODE_FAV);
    ctrl_set_attr(p_fav_list_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_fav_list_cont, OBJ_STS_SHOW);

    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_play_list, MSG_GETFOCUS, 0, 0);

    text_set_content_by_strid(p_play_list_title, IDS_IMG_PLAY_LIST);
    ctrl_set_active_ctrl(p_fav_list_cont, p_play_list);

    list_set_count(p_play_list, count, PIC_PLAY_LIST_LIST_PAGE);
    list_set_focus_pos(p_play_list, 0);
    //list_select_item(p_play_list, 0);
    p_media = ui_pic_get_fav_media_by_index(0);  //  play highlight pic
    if(p_media == NULL)
    {
      OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
      return ERR_FAILURE;
    }
    pic_stop();
    ret = ui_pic_set_play_index_fav_by_name(p_media->path);
    if(ret == FALSE)
    {
      OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
      return ERR_FAILURE;
    }
    p_msg = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PIC_MSG);
    if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
    {
      ctrl_set_sts(p_msg, OBJ_STS_HIDE);
      ctrl_erase_ctrl(p_msg);
    }
    
    ui_pic_set_play_mode(PIC_MODE_FAV);
    ui_pic_play_curn(&p_media, &g_pic_rect);
    pic_fav_list_update(p_play_list, list_get_valid_pos(p_play_list), PIC_PLAY_LIST_LIST_PAGE, 0);

    ctrl_paint_ctrl(p_fav_list_cont, TRUE);

      // empty invrgn but not draw
    //ctrl_empty_invrgn(p_list);

    gui_stop_roll(p_list);

    gui_start_roll(p_play_list, &roll_param);
  }
  else
  {
    ui_comm_dlg_open(&dlg_data_notexist);
  }

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_change_focus(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 pos;
  media_fav_t *p_media = NULL;
  control_t  *p_msg;
  BOOL set_ret = FALSE;

  ret = list_class_proc(p_list, msg, 0, 0);
  pos = list_get_focus_pos(p_list);
  p_media = ui_pic_get_fav_media_by_index(pos);
  ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_PIC_LIST);

  if(g_list.p_file == NULL)
  {
    return ret;
  }
  p_msg = ctrl_get_child_by_id(ctrl_get_parent(p_list->p_parent), IDC_PIC_MSG);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
  {
    ctrl_set_sts(p_msg, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_msg);
  }
  pic_stop();
  set_ret = ui_pic_set_play_index_fav_by_name(p_media->path);
  if(set_ret == FALSE)
  {
    OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
    return ERR_FAILURE;
  }
  //pic_update_detail(p_pic_list, TRUE, FALSE);
  ui_pic_set_play_mode(PIC_MODE_FAV);
  ui_pic_play_curn(&p_media, &g_pic_rect);

  return ret;
}

static RET_CODE on_pic_list_sort(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{

  RET_CODE ret = SUCCESS;

  control_t *p_cont, *p_sort_list;

  p_cont = p_list->p_parent;
  ui_comm_dlg_close();
  gui_stop_roll(p_list);
  p_sort_list = ctrl_get_child_by_id(p_cont, IDC_PIC_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);

  return ret;
}

RET_CODE picture_list_update()
{
  control_t  *p_list;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;

  p_list = ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST);
  pos = list_get_focus_pos(p_list);

  if(g_list.p_file[pos].type == NOT_DIR_FILE)
  {
      pic_stop();
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return ERR_FAILURE;
      }
      //pic_update_detail(p_list, TRUE, FALSE);
      ui_pic_set_play_mode(PIC_MODE_NORMAL);
      ui_pic_play_curn(&p_media, &g_pic_rect);
  }
  else
  {
    pic_update_path(p_list);
    //Dir, shoulde update path
  }
  return SUCCESS;
}

RET_CODE picture_sort_update()
{
  control_t  *p_fav_list_cont,*p_fav_list, *p_msg;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;

  p_fav_list_cont = ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_FAV_LIST_CONT);
  p_fav_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_LIST);

  pos = list_get_focus_pos(p_fav_list);

  p_media = ui_pic_get_fav_media_by_index(pos);
  ctrl_get_child_by_id(p_fav_list->p_parent->p_parent, IDC_PIC_LIST);
  p_msg = ctrl_get_child_by_id(ctrl_get_parent(p_fav_list_cont), IDC_PIC_MSG);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
  {
    ctrl_set_sts(p_msg, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_msg);
  }
  pic_stop();
  ret = ui_pic_set_play_index_fav_by_name(p_media->path);
  if(ret == FALSE)
  {
     OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
     return ERR_FAILURE;
  }

  //pic_update_detail(p_pic_list, TRUE, FALSE);
  ui_pic_set_play_mode(PIC_MODE_FAV);
  ui_pic_play_curn(&p_media, &g_pic_rect);
  return SUCCESS;
}

static RET_CODE on_pic_list_select(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  int pos = 0;
  picture_param_t p_param;
  control_t *p_root = NULL;
  BOOL ret = FALSE;
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  sort_type = pic_get_sort_type(p_list);

  pos = list_get_focus_pos(p_list);

  if(g_list.p_file == NULL)
  {
    return SUCCESS;
  }

  switch(g_list.p_file[pos].type)
  {
  case DIRECTORY:
    {
      if(g_pic_flist_dir == NULL)
        return ERR_FAILURE;

      file_list_leave_dir(g_pic_flist_dir);
      g_pic_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, g_list.p_file[pos].name);
      if(g_pic_flist_dir != NULL)
      {
         file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
         file_list_sort(&g_list, (int)g_list.file_count, sort_type);
         list_set_count(p_list, (u16)(g_list.file_count), PIC_LIST_PAGE);
         list_set_focus_pos(p_list, 0);
         pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
         ui_pic_build_play_list_indir(&g_list);
         ctrl_paint_ctrl(p_list, TRUE);
         pic_update_path(p_list);
      }
      else
      {
         OS_PRINTF("\n file_list_enter_dir failure. \n");
      }
      g_picture_player_preview_focus = 0;
      pic_stop();
      ui_pic_clear(0);//fix select ... clear pic show
    }
    break;

  case NOT_DIR_FILE:
    if(fw_find_root_by_id(ROOT_ID_PICTURE) != NULL)
    {
       break;
    }
    ui_pic_set_play_mode(PIC_MODE_NORMAL);
    ui_pic_build_play_list_indir(&g_list);
    ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
    if(ret == FALSE)
    {
        OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
        return ERR_FAILURE;
    }
    //pic_update_detail(p_list, TRUE, FALSE);
    p_param.cb = picture_list_update;

    pic_stop();
    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
    //send a msg to open picture.
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_FULLSCREEN, (u32) &p_param, 0);

    //manage_open_menu(ROOT_ID_PICTURE, (u32) &p_param, 0);
    break;

  default:
    break;
  }

  return SUCCESS;
}

#if 0
static RET_CODE on_pic_list_next_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_next(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_prev_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_pre(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_end_music(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
	media_fav_t *p_media = NULL;

	if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
	{
		if(ui_music_pre_play_next(&p_media))
		{
		  //play next music, calculate music total time.
		  music_reset_time_info(p_media->path);

		  music_player_next(p_media->path);
		  ui_music_reset_curn(p_media->path);
		}
	}

	return SUCCESS;
}

#endif

static RET_CODE on_pic_fav_list_delet(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus;
  focus = list_get_focus_pos(p_list);
  ui_pic_set_one_del(focus);
  
  is_clear_del_icon = 0;
  
  pic_fav_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
  list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);

  if(list_get_count(p_list) == 1)
    list_draw_item_ext(p_list, focus, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_delte_all(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 count = 0, index = 0;
  count = list_get_count(p_fav_list);
  if(is_clear_del_icon == 0)
  {
    is_clear_del_icon = 1;
    ui_pic_undo_save_del();
  }
  for(index = 0; index < count; index++)
  {
    ui_pic_set_one_del(index);
  }

  pic_fav_list_update(p_fav_list, list_get_valid_pos(p_fav_list), PIC_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_fav_list, TRUE);

  return SUCCESS;
}

static void pic_fav_list_save_del()
{
    u16 i;
    for(i=0; i<g_partition_cnt; i++)
    {
        ui_pic_save_del((u8)p_partition[i].letter[0]);
    }
    ui_pic_set_del_modified(FALSE);
}

static void pic_fav_list_undo_save_del()
{
  ui_pic_undo_save_del();
}

static RET_CODE on_pic_fav_list_select(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_fav_list_cont, *p_play_list, *p_menu, *p_root;
  u16 focus = 0, count = 0;
  media_fav_t *p_focus_media = NULL;
  BOOL ret = FALSE;
  picture_param_t p_param;

  focus = list_get_focus_pos(p_fav_list);

  p_fav_list_cont = ctrl_get_parent(p_fav_list);
  p_menu = ctrl_get_parent(p_fav_list_cont);
  p_play_list = ctrl_get_child_by_id(p_menu, IDC_PIC_LIST);

  if(ui_pic_is_del_modified())
  {
    ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
      pic_fav_list_save_del, pic_fav_list_undo_save_del, 0);

    if(fw_find_root_by_id(ROOT_ID_USB_PICTURE) == NULL)
    {
      return ERR_FAILURE;
    }
    
    //reset focus & count, update fav list.
    count = ui_pic_get_play_count_of_fav();
    list_set_count(p_fav_list, count, PIC_PLAY_LIST_LIST_PAGE);

    if(focus >= count)
    {
      list_set_focus_pos(p_fav_list, 0);
    }
    pic_fav_list_update(p_fav_list, list_get_valid_pos(p_fav_list), PIC_PLAY_LIST_LIST_PAGE, 0);
  }

  if(ui_pic_get_play_count_of_fav() > 0)
  {
    //get current focus & focus media.
    focus = list_get_focus_pos(p_fav_list);

    p_focus_media = ui_pic_get_fav_media_by_index(focus);
    if(p_focus_media == NULL)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    //fav list count > 0, then play it.
    ui_pic_set_play_mode(PIC_MODE_FAV);
    ret = ui_pic_set_play_index_fav_by_name(p_focus_media->path);
    if(ret == FALSE)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    p_param.cb = picture_sort_update;


    pic_stop();
    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
    //send a msg to open picture.
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_FULLSCREEN, (u32) &p_param, 0);

    //manage_open_menu(ROOT_ID_PICTURE, (u32) &p_param, 0);
  }
  else
  {
    //hide the fav list & focus on the normal list.
    ctrl_set_sts(p_fav_list_cont, OBJ_STS_HIDE);

    ctrl_process_msg(p_fav_list, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_play_list, MSG_GETFOCUS, 0, 0);

    pic_list_update(p_play_list, list_get_valid_pos(p_play_list), PIC_LIST_PAGE, 0);

    ctrl_paint_ctrl(p_menu, TRUE);

    ui_pic_set_play_mode(PIC_MODE_NORMAL);

    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);

    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_exit(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_menu, *p_fav_cont, *p_normal_list, *p_root;

  gui_stop_roll(p_fav_list);

  if(ui_pic_is_del_modified())
  {
    ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
                                  pic_fav_list_save_del, pic_fav_list_undo_save_del, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_USB_PICTURE) == NULL)
  {
    return ERR_FAILURE;
  }

  p_fav_cont = ctrl_get_parent(p_fav_list);
  p_menu = ctrl_get_parent(p_fav_cont);
  p_normal_list = ctrl_get_child_by_id(p_menu, IDC_PIC_LIST);

  ctrl_process_msg(p_fav_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_normal_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_fav_cont, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_menu, TRUE);

  ui_pic_set_play_mode(PIC_MODE_NORMAL);

  p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);

  fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);

  gui_start_roll(p_normal_list, &roll_param);

  return SUCCESS;
}


static RET_CODE pic_sort_list_update(control_t* p_list, u16 start, u16 size,
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [3] = {
    IDS_SORT_DEFAULT,
    IDS_SORT_NAME_AZ,
    IDS_SORT_NAME_ZA,
  };

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_strid(p_list, (u16)(start + i), 0,
                                      strid[start + i]);
    }
  }
  return SUCCESS;
}

static RET_CODE on_pic_sort_list_select(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_pic_list ,*p_msg;
  u16 pos;
  u32 i;
  u16 pic_no;
  u16 focus_name[MAX_FILE_PATH];
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  media_fav_t *p_media = NULL;

  p_cont = ctrl_get_parent(p_sort_list);
  p_pic_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);

  pic_no = list_get_focus_pos(p_pic_list);
  memcpy(focus_name, g_list.p_file[pic_no].name, MAX_FILE_PATH * sizeof(u16));

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  if(g_list.p_file == NULL)
  {
    gui_start_roll(p_pic_list, &roll_param);
    return ret;
  }

  pos = list_get_focus_pos(p_sort_list);
  switch(pos)
  {
    case 0:
      sort_type = DEFAULT_ORDER_MODE;
      break;

    case 1:
      sort_type = A_Z_MODE;
      break;

    case 2:
      sort_type = Z_A_MODE;
      break;

    default:
      sort_type = DEFAULT_ORDER_MODE;
      break;
  }

  file_list_sort(&g_list, (int)g_list.file_count, sort_type);
  pic_list_update(p_pic_list, list_get_valid_pos(p_pic_list), PIC_LIST_PAGE, 0);

  for(i = 0; i < g_list.file_count; i++)
  {
    if(uni_strcmp(focus_name, g_list.p_file[i].name) == 0)
    {
      list_set_focus_pos(p_pic_list, (u16)i);
      list_select_item(p_pic_list, (u16)i);
      pic_no = (u16)i;
    }
  }

  ctrl_process_msg(p_pic_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_pic_list, TRUE);

  if(g_list.p_file[pic_no].type == NOT_DIR_FILE)
  {
    p_msg = ctrl_get_child_by_id(ctrl_get_parent(p_sort_list), IDC_PIC_MSG);
    if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
    {
      ctrl_set_sts(p_msg, OBJ_STS_HIDE);
      ctrl_erase_ctrl(p_msg);
    }
    pic_stop();
    ui_pic_set_play_mode(PIC_MODE_NORMAL);
    ui_pic_play_curn(&p_media, &g_pic_rect);
  }

 gui_start_roll(p_pic_list, &roll_param);
  return ret;
}

static RET_CODE on_pic_sort_list_exit(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;

  p_cont = p_sort_list->p_parent;
  p_mp_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);
  gui_start_roll(p_mp_list, &roll_param);
  return ret;
}

RET_CODE open_usb_picture(u32 para1, u32 para2)
{
  control_t *p_menu, *p_group;
  control_t *p_detail_cont, *p_preview, *p_msg = NULL;
  control_t *p_play_list_cont, *p_play_list_title, *p_play_list;
  control_t *p_detail_pixel;
  control_t *p_list, *p_list_sbar;
  control_t *p_sort_list;
  control_t *p_help;
  control_t *p_detail_file_size;
  control_t *p_path;
  u16 i;
  u16 count=0;
  
  comm_help_data_t2 help = //help bar data
  {
    10, 70, {40, 120, 40, 120, 40, 120, 40, 120, 40, 120},
    {
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_PLAY_LIST,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_RED,
      HELP_RSC_STRID | IDS_SORT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_BLUE,
      HELP_RSC_STRID | IDS_SETUP,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_PARTITION,
      HELP_RSC_BMP   | IM_FAV,
      HELP_RSC_STRID | IDS_FAV,
     },
  };
#ifdef ENABLE_NETWORK
  comm_help_data_t2 help_network = //help bar data
  {
    6, 70, {40, 120,40, 120, 40, 200},
    {
      HELP_RSC_BMP   | IM_OK,
      HELP_RSC_STRID | IDS_SELECT,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_YELLOW,
      HELP_RSC_STRID | IDS_SWITCH_TYPE,
      HELP_RSC_BMP   | IM_EPG_COLORBUTTON_GREEN,
      HELP_RSC_STRID | IDS_NETWORK_PLACE,
    },
  };
#endif

  /*Create Menu*/
  p_menu = ui_comm_root_create(ROOT_ID_USB_PICTURE,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_TITLEICON_TV, IDS_PICTURE);
  if(p_menu == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_menu, pic_cont_proc);

#ifdef ENABLE_NETWORK
  is_usb = (para1 == 1)?0:1;
  if(!is_usb)
  {
     memcpy(ipaddress, (char *)para2, 128);
  }
#endif

  {
    str_asc2uni((u8 *)"|jpeg|JPEG|jpg|JPG|gif|GIF|BMP|bmp|PNG|png", g_ffilter_all);

    ui_epg_stop();
    #ifdef ENABLE_ADS
    pic_adv_stop();
    ui_adv_pic_release();
    #endif
    ui_pic_init(PIC_SOURCE_FILE);

    //initialize global variables.
#ifdef ENABLE_NETWORK
    if(! is_usb)
    {
      ui_get_ip_path(ip_address_with_path);
      //ui_comm_dlg_open(&dlg_data); 
      //ui_evt_disable_ir();
      //_ui_pic_m_free_dir_and_list();
      g_pic_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, ip_address_with_path);
      if(NULL == g_pic_flist_dir)
      {
         //ui_comm_dlg_close();    
         OS_PRINTF("[%s]: ##ERR## file list null\n", __FUNCTION__);
         count = 0;
      }
      else
      {
         file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
         //ui_comm_dlg_close();
         count = (u16)g_list.file_count;
      }
    }
    else
#endif
    {
      p_partition = NULL;
      g_partition_cnt = 0;
      g_partition_cnt = file_list_get_partition(&p_partition);

      if(g_partition_cnt > 0)
      {
        g_pic_flist_dir = file_list_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT,
        p_partition[0].letter);
        if (g_pic_flist_dir != NULL)
        {
          file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
          count = (u16)g_list.file_count;
        }
        else
        {
          count = 0;
        }
        for(i=0; i<g_partition_cnt; i++)
        {
            ui_pic_load_fav_list(p_partition[i].letter[0]);
        }
      }
      else
      {
        ui_pic_set_play_mode(PIC_MODE_NORMAL);
        memset(&g_list, 0, sizeof(file_list_t));
        memset(&cur_play_time, 0, sizeof(utc_time_t));
        memset(&total_play_time, 0, sizeof(utc_time_t));
      }
    }

    OS_PRINTF("total count = %d\n", count);
    ui_pic_build_play_list_indir(&g_list);
  }

  //group
  p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_PIC_GROUP, PIC_GROUP_X,
                             PIC_GROUP_Y, PIC_GROUP_W,
                             PIC_GROUP_H, p_menu, 0);
  ctrl_set_rstyle(p_group, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE, RSI_SMALL_LIST_TITLE);
  cbox_set_font_style(p_group, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);

  cbox_dync_set_count(p_group, (u16)g_partition_cnt);
  cbox_dync_set_update(p_group, pic_group_update);
  cbox_dync_set_focus(p_group, 0);

  //detail information
  p_detail_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_DETAIL_CONT, PIC_DETAIL_CONT_X,
    PIC_DETAIL_CONT_Y, PIC_DETAIL_CONT_W, PIC_DETAIL_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_detail_cont, RSI_PIC_DETAIL, RSI_PIC_DETAIL, RSI_PIC_DETAIL);

  p_detail_pixel = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_DETAIL_PIXEL,
    PIC_DETAIL_PIXEL_X, PIC_DETAIL_PIXEL_Y,
    PIC_DETAIL_PIXEL_W, PIC_DETAIL_PIXEL_H, p_detail_cont, 0);
  ctrl_set_rstyle(p_detail_pixel, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_detail_pixel, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX);
  text_set_align_type(p_detail_pixel, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_detail_pixel, TEXT_STRTYPE_UNICODE);

  p_detail_file_size = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_DETAIL_FILE_SIZE,
    PIC_DETAIL_FILE_SIZE_X, PIC_DETAIL_FILE_SIZE_Y,
    PIC_DETAIL_FILE_SIZE_W, PIC_DETAIL_FILE_SIZE_H, p_detail_cont, 0);
  ctrl_set_rstyle(p_detail_file_size, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_detail_file_size, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX);
  text_set_align_type(p_detail_file_size, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_detail_file_size, TEXT_STRTYPE_UNICODE);

  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_PREVIEW, PIC_PREV_X,
    PIC_PREV_Y, PIC_PREV_W, PIC_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_preview, RSI_PIC_PREV, RSI_PIC_PREV, RSI_PIC_PREV);

  p_msg = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_MSG, PIC_MSG_X, PIC_MSG_Y,
    PIC_MSG_W, PIC_MSG_H, p_menu, 0);
  ctrl_set_rstyle(p_msg, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_align_type(p_msg, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_msg, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_msg, TEXT_STRTYPE_STRID);
  ctrl_set_sts(p_msg, OBJ_STS_HIDE);

  //scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PIC_SBAR, PIC_LIST_SBARX,
    PIC_LIST_SBARY, PIC_LIST_SBARW, PIC_LIST_SBARH, p_menu, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_PIC_SBAR, RSI_PIC_SBAR, RSI_PIC_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_PIC_SBAR_MID, RSI_PIC_SBAR_MID,
                     RSI_PIC_SBAR_MID);
  /*ctrl_set_mrect(p_list_sbar, PIC_LIST_SBAR_MIDL, PIC_LIST_SBAR_MIDT,
    PIC_LIST_SBAR_MIDR, PIC_LIST_SBAR_MIDB);*/

  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_LIST, PIC_LISTX,
    PIC_LISTY, PIC_LISTW, PIC_LISTH, p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_PIC_LCONT_LIST, RSI_PIC_LCONT_LIST, RSI_PIC_LCONT_LIST);
#ifdef ENABLE_NETWORK
  if(! is_usb)
  {
    ctrl_set_keymap(p_list, pic_network_list_keymap);
    ctrl_set_proc(p_list, pic_network_list_proc);
  }
  else
#endif
  {
    ctrl_set_keymap(p_list, pic_list_keymap);
    ctrl_set_proc(p_list, pic_list_proc);
  }
  ctrl_set_mrect(p_list, PIC_LIST_MIDL, PIC_LIST_MIDT,
    PIC_LIST_MIDW + PIC_LIST_MIDL, PIC_LIST_MIDH + PIC_LIST_MIDT);
  list_set_item_interval(p_list, PIC_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &pic_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, count, PIC_LIST_PAGE);
  list_set_field_count(p_list, PIC_LIST_FIELD, PIC_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  //list_select_item(p_list, 0);
  list_set_update(p_list, pic_list_update, 0);
  ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

  for (i = 0; i < PIC_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(mp_list_attr[i].attr), (u16)(mp_list_attr[i].width),
                        (u16)(mp_list_attr[i].left), (u8)(mp_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, mp_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, mp_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_list, p_list_sbar);
  pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);

  //path text
  p_path = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_PATH,
    PIC_PATH_TEXT_X, PIC_PATH_TEXT_Y,
    PIC_PATH_TEXT_W, PIC_PATH_TEXT_H, p_menu, 0);
  ctrl_set_rstyle(p_path, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_path, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX);
  text_set_align_type(p_path, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_path, TEXT_STRTYPE_UNICODE);
  if(g_pic_flist_dir != NULL)
  {
#ifdef ENABLE_NETWORK
    if(! is_usb)
    {
      text_set_content_by_unistr(p_path, file_list_get_cur_path(g_pic_flist_dir));
    }
    else
#endif
    {
      text_set_content_by_unistr(p_path, p_partition[0].name);
    }
  }

  //help
#ifdef ENABLE_NETWORK
  ui_comm_help_create2(is_usb ? &help : &help_network, p_menu,FALSE);
#else
  ui_comm_help_create2(&help, p_menu,FALSE);
#endif

  //play list container
  p_play_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_FAV_LIST_CONT,
      PIC_PLAY_LIST_CONT_X, PIC_PLAY_LIST_CONT_Y,
      PIC_PLAY_LIST_CONT_W, PIC_PLAY_LIST_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_play_list_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_sts(p_play_list_cont, OBJ_STS_HIDE);

  //play list title
  p_play_list_title  = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_FAV_LIST_TITLE,
    PIC_PLAY_LIST_TITLE_X, PIC_PLAY_LIST_TITLE_Y,
    PIC_PLAY_LIST_TITLE_W, PIC_PLAY_LIST_TITLE_H,
    p_play_list_cont, 0);
  ctrl_set_rstyle(p_play_list_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_list_title, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX);
  text_set_align_type(p_play_list_title, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_list_title, TEXT_STRTYPE_STRID);

  //play list scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PIC_FAV_LIST_SBAR, PIC_PLAY_LIST_SBARX,
    PIC_PLAY_LIST_SBARY, PIC_PLAY_LIST_SBARW, PIC_PLAY_LIST_SBARH, p_play_list_cont, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_PIC_SBAR, RSI_PIC_SBAR, RSI_PIC_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_PIC_SBAR_MID, RSI_PIC_SBAR_MID,
                     RSI_PIC_SBAR_MID);
  /*ctrl_set_mrect(p_list_sbar, PIC_PLAY_LIST_SBAR_MIDL, PIC_PLAY_LIST_SBAR_MIDT,
    PIC_PLAY_LIST_SBAR_MIDR, PIC_PLAY_LIST_SBAR_MIDB);*/

  //play list
  p_play_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_FAV_LIST_LIST, PIC_PLAY_LIST_LIST_X,
    PIC_PLAY_LIST_LIST_Y, PIC_PLAY_LIST_LIST_W, PIC_PLAY_LIST_LIST_H, p_play_list_cont, 0);
  ctrl_set_rstyle(p_play_list, RSI_PIC_LCONT_LIST, RSI_PIC_LCONT_LIST, RSI_PIC_LCONT_LIST);
  ctrl_set_keymap(p_play_list, pic_fav_list_list_keymap);
  ctrl_set_proc(p_play_list, pic_fav_list_list_proc);
  ctrl_set_mrect(p_play_list, PIC_PLAY_LIST_LIST_MIDL, PIC_PLAY_LIST_LIST_MIDT,
    PIC_PLAY_LIST_LIST_MIDL + PIC_PLAY_LIST_LIST_MIDW, PIC_PLAY_LIST_LIST_MIDH + PIC_PLAY_LIST_LIST_MIDT);
  list_set_item_interval(p_play_list, PIC_PLAY_LIST_LIST_VGAP);
  list_set_item_rstyle(p_play_list, &pic_list_item_rstyle);
  list_enable_select_mode(p_play_list, TRUE);
  list_set_select_mode(p_play_list, LIST_SINGLE_SELECT);
  list_set_count(p_play_list, count, PIC_PLAY_LIST_LIST_PAGE);
  list_set_field_count(p_play_list, PIC_PLAY_LIST_LIST_FIELD, PIC_PLAY_LIST_LIST_PAGE);
  list_set_focus_pos(p_play_list, 0);
  list_set_update(p_play_list, pic_fav_list_update, 0);
  //list_select_item(p_play_list, 0);

  for (i = 0; i < PIC_LIST_FIELD; i++)
  {
    list_set_field_attr(p_play_list, (u8)i, (u32)(mp_play_list_attr[i].attr), (u16)(mp_play_list_attr[i].width),
                        (u16)(mp_play_list_attr[i].left), (u8)(mp_play_list_attr[i].top));
    list_set_field_rect_style(p_play_list, (u8)i, mp_play_list_attr[i].rstyle);
    list_set_field_font_style(p_play_list, (u8)i, mp_play_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_play_list, p_list_sbar);

  //play list help
  p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_PIC_FAV_LIST_HELP,
    PIC_PLAY_LIST_HELP_X, PIC_PLAY_LIST_HELP_Y,
    PIC_PLAY_LIST_HELP_W, PIC_PLAY_LIST_HELP_H, p_play_list_cont, 0);
  ctrl_set_rstyle(p_help, RSI_PIC_TITLE, RSI_PIC_TITLE, RSI_PIC_TITLE);
  ctrl_set_mrect(p_help, 0, 0, PIC_PLAY_LIST_HELP_W, PIC_PLAY_LIST_HELP_H);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, PIC_PLAY_LIST_HELP_MBOX_TOL,
    PIC_PLAY_LIST_HELP_MBOX_COL, PIC_PLAY_LIST_HELP_MBOX_ROW);
  mbox_set_item_interval(p_help, PIC_PLAY_LIST_HELP_MBOX_HGAP, PIC_PLAY_LIST_HELP_MBOX_VGAP);
  mbox_set_item_rstyle(p_help, RSI_PIC_TITLE,
    RSI_PIC_TITLE, RSI_PIC_TITLE);
  mbox_set_string_fstyle(p_help, FSI_PIC_INFO_MBOX,
    FSI_PIC_INFO_MBOX, FSI_PIC_INFO_MBOX);
  mbox_set_string_offset(p_help, 40, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);
  pic_fav_list_update_help(p_help, FALSE);

  //sort list
  p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_SORT_LIST,
                            PIC_SORT_LIST_X, PIC_SORT_LIST_Y,
                            PIC_SORT_LIST_W, PIC_SORT_LIST_H,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_sort_list, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG);
  ctrl_set_keymap(p_sort_list, pic_sort_list_keymap);
  ctrl_set_proc(p_sort_list, pic_sort_list_proc);
  ctrl_set_mrect(p_sort_list, PIC_SORT_LIST_MIDL, PIC_SORT_LIST_MIDT,
    PIC_SORT_LIST_MIDL + PIC_SORT_LIST_MIDW, PIC_SORT_LIST_MIDT + PIC_SORT_LIST_MIDH);
  list_set_item_interval(p_sort_list, PIC_SORT_LIST_VGAP);
  list_set_item_rstyle(p_sort_list, &pic_list_item_rstyle);
  list_set_count(p_sort_list, PIC_SORT_LIST_ITEM_TOL, PIC_SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_sort_list, PIC_SORT_LIST_FIELD_NUM, PIC_SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_sort_list, 0);
  list_set_update(p_sort_list, pic_sort_list_update, 0);

  for (i = 0; i < PIC_SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i,
                        (u32)(mp_sort_list_field_attr[i].attr),
                        (u16)(mp_sort_list_field_attr[i].width),
                        (u16)(mp_sort_list_field_attr[i].left),
                        (u8)(mp_sort_list_field_attr[i].top));
    list_set_field_rect_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].fstyle);
  }
  pic_sort_list_update(p_sort_list, list_get_valid_pos(p_sort_list), PIC_SORT_LIST_ITEM_PAGE, 0);
  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);


  //pic_update_detail(p_list, FALSE, (g_list.p_file[0].type != NOT_DIR_FILE));
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  gui_start_roll(p_list, &roll_param);

  ui_time_enable_heart_beat(FALSE);
  g_backup_roll_status = ui_ca_get_roll_status();
  ui_ca_set_roll_status(FALSE);
  g_picture_player_preview_focus = 0;
  if(ui_is_mute())
    open_mute(0, 0);
  ui_init_play_timer(ROOT_ID_USB_PICTURE, MSG_MEDIA_PLAY_TMROUT, 300);

  ui_release_signal();
  return SUCCESS;
}

static RET_CODE on_pic_too_large(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_msg = NULL;

  /* fix Bug44510 */
  if( ROOT_ID_USB_PICTURE != fw_get_focus_id() )
  {
      return SUCCESS;
  }
  
  OS_PRINTF("%s(Line: %d):  The picture too large.\n", __FUNCTION__, __LINE__);

  pic_stop();
  ui_pic_clear(0);

  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);

  ctrl_set_sts(p_msg, OBJ_STS_SHOW);

  text_set_content_by_strid(p_msg, IDS_MSG_OUT_OF_RANGE);

  ctrl_paint_ctrl(p_msg, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_play_curn(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u16 pos;
  control_t *p_msg = NULL;
  control_t* p_list = NULL;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;

  p_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  pos = list_get_focus_pos(p_list);


  if (g_picture_player_preview_focus != pos)
  {
    if(g_list.p_file[pos].type == NOT_DIR_FILE)
    {
      if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
      {
        ctrl_set_sts(p_msg, OBJ_STS_HIDE);
        ctrl_erase_ctrl(p_msg);
      }

      pic_stop();
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return ERR_FAILURE;
      }
      //pic_update_detail(p_list, TRUE, FALSE);
      ui_pic_set_play_mode(PIC_MODE_NORMAL);
      ui_pic_play_curn(&p_media, &g_pic_rect);
    }
    else
    {
      pic_stop();
      ui_pic_clear(0);//fix change to ... clear pic show
    }
    g_picture_player_preview_focus = pos;
  }
  ui_play_timer_set_state(0);

  return SUCCESS;
}

static RET_CODE on_pic_fullscreen(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
    if(fw_find_root_by_id(ROOT_ID_PICTURE) != NULL)
    {
       return ERR_FAILURE;
    }

   manage_open_menu(ROOT_ID_PICTURE, (u32) &para1, 0);

   return SUCCESS;
}


#ifdef ENABLE_NETWORK
static RET_CODE _ui_picture_change(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  //ui_evt_disable_ir();
  ui_usb_music_exit();
  ui_pic_clear(0);//clear picture buffer
  pic_stop();
  ui_pic_release();
  ui_time_enable_heart_beat(FALSE);
  if(is_usb)
  {
  }
  else
  {
    manage_open_menu(ROOT_ID_USB_FILEPLAY, 1, (u32)( ipaddress));
  }
  if(p_list != NULL)
  {
  // OS_PRINTF("============on_pic_exit gui_stop_roll p_list id = %d============\n",p_list->id);
    gui_stop_roll(p_list);
  }

  manage_close_menu(ROOT_ID_USB_PICTURE, 0, 0);

  return SUCCESS;
}

static RET_CODE ui_pic_up_folder(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  BOOL is_enter_dir = FALSE;

  if (g_list.p_file != NULL)
  {
    is_enter_dir = pic_file_list_is_enter_dir(g_ffilter_all, FILE_LIST_MAX_SUPPORT_CNT, g_list.p_file[0].name);
  }
  else
  {
    is_enter_dir = FALSE;
  }
  if(is_enter_dir)
  {
    pic_file_list_update(p_list);//simon fix bug #27346
  }
  return SUCCESS;
}
#endif


BEGIN_MSGPROC(pic_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_pic_destory)
  ON_COMMAND(MSG_PLUG_OUT, on_pic_plug_out)
  ON_COMMAND(MSG_UPDATE, on_pic_update)
  ON_COMMAND(MSG_PIC_EVT_UPDATE_SIZE, on_pic_evt_update_size)
  ON_COMMAND(MSG_EXIT, on_pic_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_pic_exit)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_pic_too_large)
  ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_pic_play_curn)
  ON_COMMAND(MSG_FULLSCREEN, on_pic_fullscreen)
END_MSGPROC(pic_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pic_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_FAV, MSG_ADD_FAV)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_BLUE, MSG_BLUE)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_RED, MSG_SORT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  #if 0
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_PLAY, MSG_MUSIC_PLAY)
  ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
  ON_EVENT(V_KEY_FORW2, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_BACK2, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)
  #endif
END_KEYMAP(pic_list_keymap, NULL)

BEGIN_MSGPROC(pic_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_list_change_focus)
  //ON_COMMAND(MSG_DECREASE, on_pic_list_change_partition)
  //ON_COMMAND(MSG_INCREASE, on_pic_list_change_partition)
  ON_COMMAND(MSG_GREEN, on_pic_list_change_partition)
  ON_COMMAND(MSG_ADD_FAV, on_pic_add_to_fav_list)
  ON_COMMAND(MSG_YELLOW, on_pic_open_fav_list)
  ON_COMMAND(MSG_BLUE, on_pic_list_set_play_mode)
  ON_COMMAND(MSG_SORT, on_pic_list_sort)
  ON_COMMAND(MSG_SELECT, on_pic_list_select)
  #if 0
  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
  ON_COMMAND(MSG_MUSIC_NEXT, on_pic_list_next_music)
  ON_COMMAND(MSG_MUSIC_PRE, on_pic_list_prev_music)
  ON_COMMAND(MSG_MUSIC_EVT_PLAY_END, on_pic_list_end_music)
  #endif
END_MSGPROC(pic_list_proc, list_class_proc)

#ifdef ENABLE_NETWORK
BEGIN_KEYMAP(pic_network_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_YELLOW, MSG_SWITCH_MEDIA_TYPE)
  //ON_EVENT(V_KEY_RECALL, MSG_EXIT)
  //ON_EVENT(V_KEY_RECALL, MSG_PIC_UPFOLDER)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_GREEN, MSG_PIC_BACK_TO_NETWORK_PLACE)
END_KEYMAP(pic_network_list_keymap, NULL)

BEGIN_MSGPROC(pic_network_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_pic_list_select)
  ON_COMMAND(MSG_SWITCH_MEDIA_TYPE, _ui_picture_change)
  ON_COMMAND(MSG_PIC_UPFOLDER, ui_pic_up_folder)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_list_change_focus)
//  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
//  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
 // ON_COMMAND(MSG_EXIT, on_pic_process_msg)
  ON_COMMAND(MSG_PIC_BACK_TO_NETWORK_PLACE, on_pic_process_msg)
END_MSGPROC(pic_network_list_proc, list_class_proc)
#endif

BEGIN_KEYMAP(pic_fav_list_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_FAV, MSG_ADD_FAV)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_RED, MSG_RED)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(pic_fav_list_list_keymap, NULL)

BEGIN_MSGPROC(pic_fav_list_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_GREEN, on_pic_fav_list_delte_all)
  ON_COMMAND(MSG_YELLOW, on_pic_fav_list_delet)
  ON_COMMAND(MSG_RED, on_pic_fav_list_select)
  ON_COMMAND(MSG_SELECT, on_pic_fav_list_select)
  ON_COMMAND(MSG_EXIT, on_pic_fav_list_exit)
  #if 0
  ON_COMMAND(MSG_MUSIC_EVT_PLAY_END, on_pic_list_end_music)
  #endif
END_MSGPROC(pic_fav_list_list_proc, list_class_proc)

BEGIN_KEYMAP(pic_sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(pic_sort_list_keymap, NULL)

BEGIN_MSGPROC(pic_sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_pic_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_pic_sort_list_exit)
  //ON_COMMAND(MSG_DESTROY, on_sort_list_destory)
END_MSGPROC(pic_sort_list_proc, list_class_proc)
 //lint +e550 +e746

#endif
