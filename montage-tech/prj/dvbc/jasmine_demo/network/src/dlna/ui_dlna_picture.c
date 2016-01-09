/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_dlna_picture.h"
#include "ui_picture_api.h"
#include "ui_dlna_api.h"
#include "ui_dlna.h"
typedef enum
{
  IDC_PHOTO_SHOW_TTL = 1,
  IDC_PHOTO_SHOW_PREW_CONT,
}photos_id_t;

static mul_pic_param_t pic_param = {{0},};
static BOOL g_pic_loading = FALSE;
static char dlna_pic_path[512];

//cont
#define PHOTO_SHOW_CONT_X  0
#define PHOTO_SHOW_CONT_Y  0
#define PHOTO_SHOW_CONT_W  SCREEN_WIDTH //960
#define PHOTO_SHOW_CONT_H  SCREEN_HEIGHT //600


#define PHOTO_SHOW_PREW_CONTX  0
#define PHOTO_SHOW_PREW_CONTY  0
#define PHOTO_SHOW_PREW_CONTW  PHOTO_SHOW_CONT_W
#define PHOTO_SHOW_PREW_CONTH  (PHOTO_SHOW_CONT_H - PHOTO_SHOW_PREW_CONTY)

u16 dlna_pic_menu_keymap(u16 key);
RET_CODE dlna_pic_menu_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static void dlna_pic_open_dlg(u16 str_id, u32 time_out)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };
  dlg_data.content = str_id;
  dlg_data.dlg_tmout = time_out;
  ui_comm_dlg_open(&dlg_data);
}

RET_CODE open_dlna_picture(u32 para1, u32 para2)
{
  control_t *p_menu = NULL, *p_prew_cont = NULL;

  if(fw_find_root_by_id(ROOT_ID_DLNA_PICTURE))
  {
     
     //return SUCCESS;
     goto display;
  }
  //create menu.
  p_menu = fw_create_mainwin(ROOT_ID_DLNA_PICTURE,
    PHOTO_SHOW_CONT_X, PHOTO_SHOW_CONT_Y,
    PHOTO_SHOW_CONT_W, PHOTO_SHOW_CONT_H, ROOT_ID_DLNA_START, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_menu != NULL);
  ctrl_set_keymap(p_menu, dlna_pic_menu_keymap);
  ctrl_set_proc(p_menu, dlna_pic_menu_proc);
  ctrl_set_rstyle(p_menu, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_prew_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PHOTO_SHOW_PREW_CONT,
    PHOTO_SHOW_PREW_CONTX, PHOTO_SHOW_PREW_CONTY,
    PHOTO_SHOW_PREW_CONTW, PHOTO_SHOW_PREW_CONTH, p_menu, 0);
  ctrl_set_rstyle(p_prew_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  ctrl_default_proc(p_prew_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
  
  ui_pic_init(PIC_SOURCE_NET);

  pic_stop();
  dlna_pic_open_dlg(IDS_LOADING_WITH_WAIT, 0);
  g_pic_loading = TRUE;
  if(para2 != 0)
  {      
	    strcpy(dlna_pic_path, (char*)para2);
  }
  if(para1 == 0)
  {
    return ERR_FAILURE;
  }

  display:
  ui_pic_set_url((u8 *)para1);
  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = 0;
  pic_param.win_rect.top = 0;
  pic_param.win_rect.right = SCREEN_WIDTH;
  pic_param.win_rect.bottom = SCREEN_HEIGHT;
  pic_param.file_size = 0;
  pic_param.handle = 0;
  pic_param.is_thumb = FALSE;
  pic_start(&pic_param);        
  
  return SUCCESS;
}

/*destory the window*/
static RET_CODE dlna_pic_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(g_pic_loading)
  {    
    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
      {
        ui_comm_dlg_close();
      }
    g_pic_loading = FALSE;
  }  
  pic_stop();
  ui_pic_clear_all(0);
  ui_pic_release();
  return ERR_NOFEATURE;
}

/*retry downloading the picture resource*/
void ui_dlna_pic_retry(void)
{
  pic_stop();
  dlna_pic_open_dlg(IDS_LOADING_WITH_WAIT, 0);
  g_pic_loading = TRUE;
  ui_pic_set_url((u8 *)dlna_pic_path);
  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = 0;
  pic_param.win_rect.top = 0;
  pic_param.win_rect.right = SCREEN_WIDTH;
  pic_param.win_rect.bottom = SCREEN_HEIGHT;
  pic_param.file_size = 0;
  pic_param.handle = 0;
  pic_param.is_thumb = FALSE;
  pic_start(&pic_param);        
}

/*abort downloading the current picture */
void ui_dlna_pic_abort(void)
{  
  if(g_pic_loading)
  {    
    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
      {
        ui_comm_dlg_close();
      }
    g_pic_loading = FALSE;
  }  
  manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
}

static RET_CODE dlna_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
  switch(msg)
  {
    case MSG_PIC_EVT_DATA_ERROR:
    	 ui_comm_showdlg_open(NULL, IDS_DATA_ERROR, NULL,2000);
        manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
        return SUCCESS;
   case MSG_PIC_EVT_TOO_LARGE:
        ui_comm_showdlg_open(NULL, IDS_DATA_LARGE, NULL,2000);
        manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
        return SUCCESS;  
   case MSG_PIC_EVT_UNSUPPORT:
        ui_comm_showdlg_open(NULL, IDS_DATA_UNRECOGNIZE, NULL,2000);
        manage_close_menu(ROOT_ID_DLNA_PICTURE, 0, 0);
        return SUCCESS;  
    default:
      break;
  }
  if(g_pic_loading)
  {    
    if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
    {
          ui_comm_dlg_close();
    }
    g_pic_loading = FALSE;
  }
  return SUCCESS;
}

BEGIN_KEYMAP(dlna_pic_menu_keymap, ui_comm_root_keymap)

END_KEYMAP(dlna_pic_menu_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(dlna_pic_menu_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, dlna_pic_on_destory)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, dlna_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, dlna_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, dlna_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, dlna_pic_draw_end)  
END_MSGPROC(dlna_pic_menu_proc, ui_comm_root_proc)


