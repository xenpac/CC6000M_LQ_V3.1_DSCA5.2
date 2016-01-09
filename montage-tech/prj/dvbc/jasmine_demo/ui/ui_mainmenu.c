/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_mainmenu.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "customer_config.h"

//ca
#include "cas_manager.h"
//ad
#include "ui_util_api.h"
//pic
#include "ui_picture_api.h"
#ifdef ENABLE_ADS
#include "ads_ware.h"
#include "ui_ad_api.h"
#endif

#ifdef ENABLE_NETWORK
#include "ui_live_tv_api.h"
#endif

#define ASK_DIALOG_W (340)
#define ASK_DIALOG_H (160)
#define ASK_DIALOG_L ((SCREEN_WIDTH + MAINMENU_AD_WINDOW_W + MAINMENU_AD_WINDOW_X - ASK_DIALOG_W)/2)
#define ASK_DIALOG_T ((SCREEN_HEIGHT - ASK_DIALOG_H) / 2)

#define USB_NOTIFY_W  (COMM_DLG_W)
#define USB_NOTIFY_H  (COMM_DLG_H)
#define USB_NOTIFY_L  ((SCREEN_WIDTH + MAINMENU_AD_WINDOW_W + MAINMENU_AD_WINDOW_X - USB_NOTIFY_W)/2)
#define USB_NOTIFY_T  ((SCREEN_HEIGHT - USB_NOTIFY_H) / 2)

enum main_menu_control_id
{
  IDC_ARROW_L = 1,
  IDC_ARROW_R,
  IDC_MAINMENU_MBOX,
  IDC_MAINMENU_TITLE_TEXT,
  IDC_ITEM_CONT,
  IDC_MAINMENU_ITEM_LIST,
  IDC_AD_WIN,
  IDC_MAIN_TITLE,
};

enum main_menu_btn_id
{
  IDC_ITEM_1 = 0,
  IDC_ITEM_2,
  IDC_ITEM_3,
  IDC_ITEM_4,
  IDC_ITEM_5,
  IDC_ITEM_6,
  IDC_ITEM_7,
  IDC_ITEM_8,
  IDC_ITEM_9,
  IDC_ITEM_10,
  IDC_ITEM_MAX,
  IDC_ARROW_UP,
  IDC_ARROW_DOWN,
  IDC_LINE_1,
  IDC_LINE_2,
  IDC_LINE_3,
  IDC_LINE_4,
  IDC_LINE_5,
  IDC_LINE_6,
  IDC_LINE_7,
  IDC_LINE_8,
  //IDC_LINE_9,
  IDC_LINE_MAX,
};

enum local_msg
{
  MSG_CA_HIDE_MENU = MSG_LOCAL_BEGIN + 460,
};

//MAINTYPE与g_dwIDSName一一对应
typedef enum _MAINMENUTYPE
{
  //?
  MAINMENU_CHANNEL_LIST,
  MAINMENU_INSTALLATION,
  MAINMENU_SYSTEM,
  MAINMENU_TOOLS,
#ifdef ENABLE_UI_MEDIA
  MAINMENU_MEDIA_CENTER,
#endif
  MAINMENU_CA,
  
#ifdef ENABLE_NETWORK
  MAINMENU_NETWORK,
#endif

  MAINMENU_MAX_LEVEL,

}MAINMENUTYPE;

static u16 g_dwIDSName[]=
{
  IDS_CHANNEL,
  IDS_INSTALLATION,
  IDS_SYSTEM,
  IDS_TOOLS,
#ifdef ENABLE_UI_MEDIA
  IDS_MEDIA,
#endif
  IDS_CA2,
#ifdef ENABLE_NETWORK
  IDS_NETWORK,
#endif
};

static u16 g_dwIcon_HL[]=
{
  IM_INDEX_CHANNELLIST_FOCUS,
  IM_INDEX_INSTALLATION_FOCUS,
  IM_INDEX_SYSTEM_FOCUS,
  IM_INDEX_TOOLS_FOCUS,
#ifdef ENABLE_UI_MEDIA
  IM_INDEX_MEDIA_PLAYER_FOCUS,
#endif
  IM_INDEX_CA_FOCUS,
#ifdef ENABLE_NETWORK
  IM_INDEX_NETWORK_FOCUS,
#endif
};

static u16 g_dwIcon_SH[]=
{
  IM_INDEX_CHANNELLIST,
  IM_INDEX_INSTALLATION,
  IM_INDEX_SYSTEM,
  IM_INDEX_TOOLS,
#ifdef ENABLE_UI_MEDIA
  IM_INDEX_MEDIA_PLAYER,
#endif
  IM_INDEX_CA,
#ifdef ENABLE_NETWORK
  IM_INDEX_NETWORK,
#endif
};



//菜单的定义,该结构是用于3种类型的菜单
typedef struct _MainMenuStruct
{
  s8    dwPrevious;        //霞恫说ゼ?绻挥校蛭?1
  s8    dwLevel;           //菜单级别
  u16   dwStringID;        // button上文字的ID
  s8    nNext;             //下级菜单级穑绻挥校蛭?1
  u8    root_id;           //只有当nNext?1时,才有效
  u32   lParam;            //对话虺跏蓟数
  BOOL  bNeedPWD;          //need password or not
  BOOL  bNeedHide;         //是否需要隐?
  u16   next_lev_focus;    //next level button focus
  u16   cur_lev_focus;     //current level button focus,  the low 8bit indicates the focus when initial level, the high 8 bit indicates the focus when the level show hide button
  comm_help_data_t *p_help;
}MAINMAINMENUSTRUCT;

static MAINMAINMENUSTRUCT g_MainMenuMap[] =
{
  //(Level 2):channel list
  {-1, MAINMENU_CHANNEL_LIST, IDS_EPG, -1, ROOT_ID_EPG,   0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_CHANNEL_LIST, IDS_CHANNEL_LIST, -1, ROOT_ID_PROG_LIST,   0, FALSE,  TRUE, 0, 0, 0},
  {-1, MAINMENU_CHANNEL_LIST, IDS_CHANNEL_EDIT, -1, ROOT_ID_CHANNEL_EDIT,   0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_CHANNEL_LIST, IDS_CHANNEL_INFORMATION, -1, ROOT_ID_PRO_INFO,   0, FALSE,  FALSE, 0, 0, 0},
#ifdef CUSTOMER_HCI
  {-1, MAINMENU_CHANNEL_LIST, IDS_CATEGORY_HK, -1, ROOT_ID_CATEGORY,   0, FALSE,  FALSE, 0, 0, 0},
  #else
  {-1, MAINMENU_CHANNEL_LIST, IDS_CATEGORY, -1, ROOT_ID_CATEGORY,   0, FALSE,  FALSE, 0, 0, 0},
#endif
#ifdef NVOD_ENABLE
  {-1, MAINMENU_CHANNEL_LIST, IDS_NVOD_NVOD, -1, ROOT_ID_NVOD,   0, FALSE,  FALSE, 0, 0, 0},
#endif
#ifdef MOSAIC_ENABLE
  {-1, MAINMENU_CHANNEL_LIST, IDS_MOSAIC, -1, ROOT_ID_MOSAIC,   0, FALSE,  FALSE, 0, 0, 0},
#endif
  {-1, MAINMENU_CHANNEL_LIST, IDS_FAV_LIST, -1, ROOT_ID_FAV_LIST,   0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_CHANNEL_LIST, IDS_EDIT_FAV_CHANNEL, -1, ROOT_ID_FAV_EDIT,   0, FALSE,  FALSE, 0, 0, 0},
#ifdef DISPLAY_AND_VOLUME_AISAT 
  {-1, MAINMENU_CHANNEL_LIST, IDS_POWER_CHANNEL_SETTING, -1, ROOT_ID_POWER_ON_CHANNEL,   0, FALSE,  FALSE, 0, 0, 0},
#endif
  //(Level 2):installation
  {-1, MAINMENU_INSTALLATION, IDS_AUTO_SEARCH, -1,  ROOT_ID_AUTO_SEARCH,   0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_INSTALLATION, IDS_MANUAL_SEARCH, -1,  ROOT_ID_MANUAL_SEARCH,    0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_INSTALLATION, IDS_RANGE_SEARCH, -1,  ROOT_ID_RANGE_SEARCH,  0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_INSTALLATION, IDS_DELETE_ALL,        -1,  ROOT_ID_DELETE_ALL, 0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_INSTALLATION, IDS_FACTORY_SET, -1,  ROOT_ID_FACTORY_SET,        0, FALSE,  FALSE, 0, 0, 0},
#ifdef NIT_SETTING 
  {-1, MAINMENU_INSTALLATION, IDS_FUNCTION_SET, -1,  ROOT_ID_NIT_SETTING,        0, FALSE,  FALSE, 0, 0, 0},
#endif  
#ifdef IMPORT_AND_EXPORT
  {-1, MAINMENU_INSTALLATION, IDS_USB_IMPORT, -1,  ROOT_ID_IMPORT,        0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_INSTALLATION, IDS_USB_EXPORT, -1,  ROOT_ID_EXPORT,        0, FALSE,  FALSE, 0, 0, 0},
#endif
//(Level 2):system
  {-1, MAINMENU_SYSTEM, IDS_LANGUAGE, -1,  ROOT_ID_LANGUAGE,      0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_SYSTEM, IDS_AV_SETTING, -1,  ROOT_ID_TVSYS_SET,            0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_SYSTEM, IDS_LOCAL_TIME_SET, -1,  ROOT_ID_TIME_SET,       0, FALSE,  FALSE, 0, 0, 0},
#ifdef CUSTOMER_HCI
  {-1, MAINMENU_SYSTEM, IDS_BOOK_MANAGE_HK, -1,  ROOT_ID_BOOK_LIST,      0, FALSE, FALSE, 0, 0, 0},
#else
  {-1, MAINMENU_SYSTEM, IDS_BOOK_MANAGE, -1,  ROOT_ID_BOOK_LIST,      0, FALSE, FALSE, 0, 0, 0},
#endif  
  {-1, MAINMENU_SYSTEM, IDS_PARENTAL_LOCK, -1,  ROOT_ID_PARENTAL_LOCK,            0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_SYSTEM, IDS_OSD_SET, -1,  ROOT_ID_OSD_SET,       0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_SYSTEM, IDS_FREQUENCY_SETTING, -1,  ROOT_ID_FREQ_SET,      0, FALSE, FALSE, 0, 0, 0},
 #ifdef DISPLAY_AND_VOLUME_AISAT
  {-1, MAINMENU_SYSTEM, IDS_DISPLAY_SETTING, -1,  ROOT_ID_DISPLAY,      0, FALSE, FALSE, 0, 0, 0},
#endif
 //(Level 2):tools
  {-1, MAINMENU_TOOLS, IDS_INFORMATION,  -1,  ROOT_ID_INFO,      0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_TOOLS, IDS_UPGRADE_BY_RS232,  -1,  ROOT_ID_UPGRADE_BY_RS232,      0, FALSE, TRUE, 0, 0, 0},
  {-1, MAINMENU_TOOLS, IDS_UPGRADE_BY_SAT, -1,  ROOT_ID_OTA, 0, FALSE, FALSE, 0, 0, 0},
#ifdef ENABLE_USB_CONFIG
  {-1, MAINMENU_TOOLS, IDS_UPGRADE_BY_USB,         -1,  ROOT_ID_UPGRADE_BY_USB,    0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_TOOLS, IDS_BACKUP_BY_USB,  -1,  ROOT_ID_DUMP_BY_USB,      0, FALSE, TRUE, 0, 0, 0},
#endif
  {-1, MAINMENU_TOOLS, IDS_GAME,         -1,  ROOT_ID_GAME,    0, FALSE, FALSE, 0, 0, 0},
#ifdef USB_TEST
  {-1, MAINMENU_TOOLS, IDS_USB_TEST,        -1,  ROOT_ID_USB_TEST, 0, FALSE, FALSE, 0, 0, 0},
#endif
  {-1, MAINMENU_TOOLS, IDS_TS_RECORD,         -1,  ROOT_ID_TS_RECORD,    0, FALSE, TRUE, 0, 0, 0},
#ifdef GOBY_PLUS
  {-1, MAINMENU_TOOLS, IDS_UPGRADE_BY_NET,         -1,  ROOT_ID_UPGRADE_BY_NETWORK,    0, FALSE, FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  //(Level 2):media center
  {-1, MAINMENU_MEDIA_CENTER, IDS_MUSIC,         -1, ROOT_ID_USB_MUSIC,         0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_MEDIA_CENTER, IDS_PICTURE,         -1, ROOT_ID_USB_PICTURE,         0, FALSE, FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_FILE_PLAY
  {-1, MAINMENU_MEDIA_CENTER, IDS_MOVIES,      -1, ROOT_ID_USB_FILEPLAY, 0, FALSE,  FALSE, 0, 0, 0},
#endif
#ifdef BROWSER_APP
  {-1, MAINMENU_MEDIA_CENTER, IDS_DATA_BROADCAST,   -1, ROOT_ID_BROWSER, 0, FALSE,  FALSE, 0, 0, 0},

#endif
#ifdef ENABLE_PVR_REC_CONFIG
  {-1, MAINMENU_MEDIA_CENTER, IDS_RECORD_MANAGER,         -1, ROOT_ID_RECORD_MANAGER,         0, FALSE,  FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_UI_MEDIA
  {-1, MAINMENU_MEDIA_CENTER, IDS_HDD_INFO,       -1, ROOT_ID_HDD_INFO,         0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_MEDIA_CENTER, IDS_STORAGE_FORMAT,         -1, ROOT_ID_STORAGE_FORMAT,         0, FALSE, FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_TIMESHIFT_CONFIG
  {-1, MAINMENU_MEDIA_CENTER, IDS_DVR_CONFIG,         -1, ROOT_ID_DVR_CONFIG,         0, FALSE, FALSE, 0, 0, 0},
#endif

#ifdef ENABLE_UI_MEDIA
  {-1, MAINMENU_MEDIA_CENTER, IDS_JUMP,      -1, ROOT_ID_JUMP, 0, FALSE,  FALSE, 0, 0, 0},
#endif
  //(Level 2):CA
#ifdef UDRM_CA
  {-1, MAINMENU_CA, IDS_UDRM_MENU,         -1, ROOT_ID_INVALID,         0, FALSE, FALSE, 0, 0, 0},
#else
  {-1, MAINMENU_CA, IDS_CA,         -1, ROOT_ID_INVALID,         0, FALSE, FALSE, 0, 0, 0},
  #ifdef PLAAS_DEXIN_STB_ID_COMBINE
  {-1, MAINMENU_CA, IDS_STB_ID_SETTING,         -1, ROOT_ID_STB_ID_SELECT,         0, FALSE, FALSE, 0, 0, 0},
  #endif
#endif
#ifdef ENABLE_NETWORK
  //(Level 2):Network
  {-1, MAINMENU_NETWORK, IDS_MAC_ADDR,         -1, ROOT_ID_MAC_SETTING,         0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_NETWORK, IDS_NETWORK_CONFIG,         -1, ROOT_ID_SUBNETWORK_CONFIG,         0, FALSE, FALSE, 0, 0, 0},
  //{-1, MAINMENU_NETWORK, IDS_NETWORK_APP,      -1, ROOT_ID_SUBMENU_NETWORK, 0, FALSE,  FALSE, 0, 0, 0},
  {-1, MAINMENU_NETWORK, IDS_ONLINE_MOVIE_APP,      -1, ROOT_ID_SUBMENU_NM, 0, FALSE,  FALSE, 0, 0, 0},
  //{-1, MAINMENU_NETWORK, IDS_STB_NAME_SET,      -1, ROOT_ID_STB_NAME, 0, FALSE,  FALSE, 0, 0, 0},  
  //{-1, MAINMENU_NETWORK, IDS_IPTV,      -1, ROOT_ID_LIVE_TV, 0, FALSE,  FALSE, 0, 0, 0},
  //{-1, MAINMENU_NETWORK, IDS_VOD,      -1, ROOT_ID_IPTV, 0, FALSE,  FALSE, 0, 0, 0},
  //{-1, MAINMENU_NETWORK, IDS_NETWORK_PLACE,      -1, ROOT_ID_NETWORK_PLACES, 0, FALSE,  FALSE, 0, 0, 0},
#endif
};

static u8 g_dwBtnID[]=
{
  IDC_ITEM_1,
  IDC_ITEM_2,
  IDC_ITEM_3,
  IDC_ITEM_4,
  IDC_ITEM_5,
  IDC_ITEM_6,
  IDC_ITEM_7,
  IDC_ITEM_8,
  IDC_ITEM_9,
  IDC_ITEM_10,
};

static rect_t g_del_all_dlg_rc =
{
  ASK_DIALOG_L,
  ASK_DIALOG_T,
  ASK_DIALOG_L + ASK_DIALOG_W,
  ASK_DIALOG_T + ASK_DIALOG_H,
};

static rect_t g_restore_dlg_rc =
{
  ASK_DIALOG_L,
  ASK_DIALOG_T,
  ASK_DIALOG_L + ASK_DIALOG_W,
  ASK_DIALOG_T + ASK_DIALOG_H,
};

#if 1
static list_xstyle_t menu_list_item_rstyle =
{
  RSI_MENU_ITEM_BG,
  RSI_MENU_ITEM_BG,
  RSI_MAINMENU_ITEM_HL,
  RSI_MAINMENU_ITEM_HL,
  RSI_MAINMENU_ITEM_HL,
};

#if (defined CUSTOMER_JIESAI_WUNING  || defined CUSTOMER_YINHE_TR || defined CUSTOMER_JIZHONG_ANXIN || CUSTOMER_YINHE_LINGGANG)
static list_xstyle_t menu_list_field_fstyle =
{
  FSI_WHITE_28,
  FSI_WHITE_28,
  FSI_WHITE_28,
  FSI_WHITE_28,
  FSI_WHITE_28,
};
#else
static list_xstyle_t menu_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};
#endif

static list_xstyle_t menu_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

#endif


static u16   g_nStart = 0;
static u16   g_nStartPaint = 0;

static u16   g_nEnd = 9;
static u16   g_nEndPaint = 9;
static u16   g_nListTotal = 0;

static u8 g_dwFocusBtnID = IDC_ITEM_1;
static u8 g_ucLevelType = MAINMENU_CHANNEL_LIST;

static u8 g_title_focus_index = 4;

static u16   g_nCountOfBtn = 0;

extern u16 g_ts_jump_select;

#define MAINMENU_REC_INDEX 4

static u8 g_curn_menu = 0;

static u16 g_dwTitleCount = sizeof(g_dwIDSName)/sizeof(u16);

static BOOL g_bFirstEnter = FALSE;/*add for when directly enter scroll menu refresh error */

static u8 g_search_root_id = 0;

u16 pwdlg_keymap_in_menu(u16 key);

RET_CODE pwdlg_proc_in_menu(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 main_menu_item_keymap(u16 key);

RET_CODE main_menu_item_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

//u16 main_menu_cont_keymap(u16 key);

RET_CODE main_menu_cont_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
u16 main_menu_cont_keymap(u16 key);

RET_CODE main_menu_btn_cont_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

u16 main_menu_factory_pwdlg_keymap(u16 key);

u16 main_menu_delete_all_pwdlg_keymap(u16 key);

u16 main_menu_import_pwdlg_keymap(u16 key);

u16 main_menu_export_pwdlg_keymap(u16 key);

RET_CODE main_menu_delete_all_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

RET_CODE main_menu_factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE main_menu_import_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

RET_CODE main_menu_export_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

RET_CODE search_proc_in_menu(control_t *ctrl, u16 msg, u32 para1, u32 para2);
#ifdef ENABLE_ADS
//extern BOOL ui_adv_pic_play(ads_ad_type_t type, u8 root_id);
#endif
void ui_set_mainmenu_open_level(u32 type)
{
    g_ucLevelType = type;
}
static BOOL CalStartToEnd(u32 LevelType)
{
  u16 ii = 0, jj = 0;
  u16 nCount = sizeof(g_MainMenuMap)/sizeof(MAINMAINMENUSTRUCT);
  u8 hide_count = 0;

  for ( ; ii<nCount; )
  {
    if (g_MainMenuMap[ii].dwLevel == (s8)LevelType)
    {
      break;
    }
    else
    {
      ii++;
    }
  }

  if(ii == nCount)
  {
    return FALSE;
  }

  g_nStart = ii;
  g_nStartPaint = ii;

  for ( ; ii<nCount; )
  {
    if (g_MainMenuMap[ii].dwLevel != (s8)LevelType)
    {
      break;
    }
    else
    {
      ii++;
    }
  }

  g_nEnd = ii - 1;

  for(jj=g_nStart; jj<=g_nEnd; jj++)
  {
    if(g_MainMenuMap[jj].bNeedHide)
    {
      hide_count++;
    }
  }

  if ( g_nEnd - g_nStart - hide_count >= g_nCountOfBtn )
  {
    g_nEndPaint = g_nStartPaint + g_nCountOfBtn - 1;
  }
  else
  {
    g_nEndPaint = g_nEnd;
  }
  g_nListTotal = g_nEndPaint - g_nStartPaint - hide_count + 1;
  return TRUE;
}
#if 0
static BOOL ShowBtn(control_t * p_btn_cont)
{
  control_t *p_ctrl = NULL;
  u16 ii = 0, jj = 0;
  //u16 nCount = g_nEndPaint - g_nStartPaint + 1;
  u8 hide_count = 0;
  
  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if(g_MainMenuMap[ii].bNeedHide)
    {
      hide_count++;
    }
  }

  for ( ii = 0; ii<g_nCountOfBtn; ii++)
  {
    p_ctrl = ctrl_get_child_by_id(p_btn_cont, g_dwBtnID[ii]);
    if(NULL != p_ctrl) //for bug 14595
    {
        if(ii<=g_nEndPaint-g_nStartPaint-hide_count)
        {
          //ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);
          ctrl_set_sts(p_ctrl, OBJ_STS_SHOW);
        }
        else
        {
          ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
        }
    }
  }

  for ( ii = 0, jj = g_nStartPaint; jj<=g_nEndPaint; jj++)
  {
    if(g_MainMenuMap[jj].bNeedHide == FALSE)
    {
      p_ctrl = ctrl_get_child_by_id(p_btn_cont, g_dwBtnID[ii]);
      if(NULL != p_ctrl)
      {
          text_set_content_by_strid(p_ctrl, g_MainMenuMap[jj].dwStringID);
      }
      ii++;
    }
  }

  return TRUE;
}
#endif
static void MainMenuShowTitle(control_t *p_cont, BOOL bforce)
{
  u8 i,index;
  control_t * p_mbox, *p_title_text;

  p_mbox = ctrl_get_child_by_id(p_cont, IDC_MAINMENU_MBOX);
  p_title_text = ctrl_get_child_by_id(p_cont, IDC_MAINMENU_TITLE_TEXT);
  
  for(i = 0; i<g_dwTitleCount; i++)
  {
    index = (u8)((2*g_dwTitleCount-g_title_focus_index+g_ucLevelType+i)%g_dwTitleCount);

    mbox_set_content_by_icon(p_mbox, i, g_dwIcon_HL[index], g_dwIcon_SH[index]);
  }
  text_set_content_by_strid(p_title_text, g_dwIDSName[g_ucLevelType]);
  if(bforce)
  {
    ctrl_paint_ctrl(p_mbox, bforce);
  }
}

static BOOL  MainMenuShowChange(control_t * p_ctrl, u8 NewLevel, u8 dwBtnID, BOOL bforce)
{
  g_ucLevelType  = NewLevel;

  //ShowBtn(p_ctrl);
    MainMenuShowTitle(p_ctrl->p_parent, bforce);
#if 0
  if(ctrl_get_active_ctrl(p_ctrl)!=NULL)
  {
    ctrl_default_proc(ctrl_get_active_ctrl(p_ctrl), MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(ctrl_get_child_by_id(p_ctrl, dwBtnID), MSG_GETFOCUS, 0, 0);

    g_dwFocusBtnID = dwBtnID;
  }
  #endif
  
  g_dwFocusBtnID = dwBtnID;
  return TRUE;
}

#if 1
static RET_CODE item_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{  // TODO
  u16 ii = 0, jj = 0;
  u16 cnt = list_get_count(p_list);
  u16 hide_cnt = 0;
  for(jj = 0;jj < start;jj++)
  {
	if((g_MainMenuMap[jj].bNeedHide == TRUE))
	{
		hide_cnt++;
	}
  }
  jj = g_nStartPaint + start + hide_cnt;
  for(ii = 0; ii < size;)
 {
    if((g_MainMenuMap[jj].bNeedHide == FALSE))
    {
      if((ii + start) < cnt )
      {
        list_set_field_content_by_strid(p_list, (start + ii), 0, g_MainMenuMap[jj].dwStringID);
        ii++;
      }
      else
      {
        break;
      }
    }
    jj++;
  }
  if(INVALID_LIST_CONTEXT != context)
  {
    if(FALSE==g_bFirstEnter)
    {
        ctrl_paint_ctrl(ctrl_get_parent(p_list), FALSE);
    }
  }
 g_bFirstEnter = FALSE;

  return SUCCESS;
}
#endif

static BOOL  ChangeLevelMenu(control_t * p_ctrl, u8 NewLevel, u8 dwBtnID, BOOL bforce)
{
  control_t * p_list = NULL;

  p_list = ctrl_get_child_by_id(p_ctrl, IDC_MAINMENU_ITEM_LIST);
  MT_ASSERT(p_list != NULL);
    
  CalStartToEnd(NewLevel);
  MainMenuShowChange(p_ctrl,NewLevel, dwBtnID, bforce);

  list_set_count(p_list, g_nListTotal, MENU_LIST_PAGE_NUM);
  list_set_focus_pos(p_list, g_dwFocusBtnID);
  
  item_list_update(p_list, list_get_valid_pos(p_list), g_nListTotal, INVALID_LIST_CONTEXT);
  return TRUE;
}
//lint -e656
RET_CODE open_main_menu(u32 para1, u32 para2)
{
  control_t *p_cont, *p_bg, *p_mbox_title,*p_icon, *p_ad_win, *p_title_text, *p_title, *p_list;
  u16 i;
  u16 x,y;

  g_nCountOfBtn = sizeof(g_dwBtnID)/sizeof(u8);

  g_nStart = 0;
  g_nStartPaint = 0;
  g_nEnd = 8;
  g_nEndPaint = 8;
  CalStartToEnd(g_ucLevelType);
  g_dwFocusBtnID = (u8)(IDC_ITEM_1+g_MainMenuMap[g_nStartPaint].cur_lev_focus);
  g_bFirstEnter = TRUE;
  
  p_cont = ui_comm_root_create(ROOT_ID_MAINMENU, 0,
                          MAINMENU_CONT_X, MAINMENU_CONT_Y,
                          MAINMENU_CONT_W, MAINMENU_CONT_H,
                          RSC_INVALID_ID,
                          RSC_INVALID_ID);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  
  ctrl_set_keymap(p_cont, main_menu_cont_keymap);
  ctrl_set_proc(p_cont, main_menu_cont_proc);

//hide comm title
  ui_comm_title_hide(p_cont);

//creat mainmenu title
  p_title = ctrl_create_ctrl(CTRL_CONT, IDC_MAIN_TITLE,
                                        MAINMENU_TITLE_ICON_X, MAINMENU_TITLE_ICON_Y, 
                                        MAINMENU_TITLE_ICON_W, MAINMENU_TITLE_ICON_H,
                                        p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_TITLE_BG, RSI_TITLE_BG, RSI_TITLE_BG);
  //AD window
  
  p_ad_win = ctrl_create_ctrl(CTRL_BMAP, IDC_AD_WIN,
                                              MAINMENU_AD_WINDOW_X, MAINMENU_AD_WINDOW_Y, 
                                              MAINMENU_AD_WINDOW_W,MAINMENU_AD_WINDOW_H, 
                                              p_cont, 0);
  
  ctrl_set_rstyle(p_ad_win, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  
  #ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_MENU_UP, ROOT_ID_MAINMENU);
  #endif

  //mbox title icon
  p_mbox_title = ctrl_create_ctrl(CTRL_MBOX, (u8)IDC_MAINMENU_MBOX,
                                  MAINMENU_MBOX_TITLE_X, MAINMENU_MBOX_TITLE_Y,
                                  MAINMENU_MBOX_TITLE_W, MAINMENU_MBOX_TITLE_H,
                                  p_cont, 0);
  ctrl_set_rstyle(p_mbox_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_style(p_mbox_title, STL_EX_ALWAYS_HL);
  mbox_enable_icon_mode(p_mbox_title, TRUE);
  mbox_enable_string_mode(p_mbox_title, FALSE);
  mbox_set_count(p_mbox_title, g_dwTitleCount, g_dwTitleCount, 1);
  mbox_set_item_rstyle(p_mbox_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_item_interval(p_mbox_title, 0, 0);
  //mbox_set_string_fstyle(p_mbox_title, FSI_WHITE_32, FSI_WHITE, FSI_WHITE);
  //mbox_set_content_strtype(p_mbox_title, MBOX_STRTYPE_STRID);
  mbox_set_icon_align_type(p_mbox_title, STL_TOP | STL_VCENTER);
  //mbox_set_string_align_type(p_mbox_title, STL_BOTTOM | STL_VCENTER);
  mbox_set_focus(p_mbox_title, g_title_focus_index);

  //creat title text
  p_title_text = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_MAINMENU_TITLE_TEXT,
  	                                 MAINMENU_TITLE_X, MAINMENU_TITLE_Y,
  	                                 MAINMENU_TITLE_W, MAINMENU_TITLE_H,
  	                                 p_cont, 0);
  ctrl_set_rstyle(p_title_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_title_text, FSI_WHITE_48, FSI_WHITE_48, FSI_WHITE_48);
  text_set_align_type(p_title_text, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_title_text, TEXT_STRTYPE_STRID);
   
  // create btn container
  p_bg = ctrl_create_ctrl(CTRL_CONT, IDC_ITEM_CONT, MAINMENU_CONT_ITEM_X, MAINMENU_CONT_ITEM_Y,
                           MAINMENU_CONT_ITEM_W, MAINMENU_CONT_ITEM_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_bg, RSI_MAINMENU_FRM, RSI_MAINMENU_FRM, RSI_MAINMENU_FRM);
  ctrl_set_proc(p_bg, main_menu_btn_cont_proc);

  //arrow icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP, (u8)IDC_ARROW_UP,
                                  MAINMENU_ARROW_UP_X, MAINMENU_ARROW_UP_Y,
                                  MAINMENU_ARROW_UP_W, MAINMENU_ARROW_UP_H,
                                  p_bg, 0);
  bmap_set_content_by_id(p_icon, IM_INDEX_ARROW_UP);
  //ctrl_set_sts(p_icon, OBJ_STS_HIDE);
  
  //arrow icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP, (u8)IDC_ARROW_DOWN,
                                  MAINMENU_ARROW_DOWN_X, MAINMENU_ARROW_DOWN_Y,
                                  MAINMENU_ARROW_DOWN_W, MAINMENU_ARROW_DOWN_H,
                                  p_bg, 0);
  bmap_set_content_by_id(p_icon, IM_INDEX_ARROW_DOWN);
  //ctrl_set_sts(p_icon, OBJ_STS_HIDE);
  
  //create button gap line
  for(i=0; i<IDC_LINE_MAX-IDC_LINE_1; i++)
  {
    x = MAINMENU_ITEM_X;
    y = (u16)(MAINMENU_ITEM_Y - MAINMENU_ITEM_V_LINE_H + i*(MAINMENU_ITEM_H + MAINMENU_ITEM_V_GAP));

    p_icon = ctrl_create_ctrl(CTRL_BMAP, (u8)IDC_LINE_1 + i,
                                  x, y,
                                  MAINMENU_ITEM_V_LINE_W, MAINMENU_ITEM_V_LINE_H,
                                  p_bg, 0);
    bmap_set_content_by_id(p_icon, IM_INDEX_MENULINE);
  }
#if 1
  //creat item list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_MAINMENU_ITEM_LIST,
                     MAINMENU_LIST_X, MAINMENU_LIST_Y,
                     MAINMENU_LIST_W, MAINMENU_LIST_H, 
                     p_bg,  0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, main_menu_item_keymap);
  ctrl_set_proc(p_list, main_menu_item_proc);  
  
  ctrl_set_mrect(p_list, MAINMENU_LIST_MID_L, MAINMENU_LIST_MID_T,
    MAINMENU_LIST_MID_L + MAINMENU_LIST_MID_W, MAINMENU_LIST_MID_T + MAINMENU_LIST_MID_H);
  list_set_item_interval(p_list, 2);
  list_set_item_rstyle(p_list, &menu_list_item_rstyle);
  list_set_count(p_list, MENU_LIST_PAGE_NUM, MENU_LIST_PAGE_NUM);
  list_set_field_count(p_list, 1, MENU_LIST_PAGE_NUM);
  list_set_focus_pos(p_list, g_dwFocusBtnID);
  list_set_update(p_list, item_list_update, 0);
  list_set_field_attr(p_list, 0, LISTFIELD_TYPE_STRID | STL_CENTER | STL_VCENTER,
                              360, 10, 0);
  list_set_field_rect_style(p_list, 0, &menu_list_field_rstyle);
  list_set_field_font_style(p_list, 0, &menu_list_field_fstyle);
#else
  //create button
  for(i=0; i<IDC_ITEM_MAX-1; i++)
  {

    x = MAINMENU_ITEM_X;
    y = MAINMENU_ITEM_Y + i*(MAINMENU_ITEM_H + MAINMENU_ITEM_V_GAP);


    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_ITEM_1 + i),
                              x, y, MAINMENU_ITEM_W,MAINMENU_ITEM_H,
                              p_bg, 0);
    ctrl_set_rstyle(p_btn[i],
                    RSI_MAINMENU_ITEM_SH,
                    RSI_MAINMENU_ITEM_HL,
                    RSI_MAINMENU_ITEM_SH);
    ctrl_set_keymap(p_btn[i], main_menu_item_keymap);
    ctrl_set_proc(p_btn[i], main_menu_item_proc);
    text_set_font_style(p_btn[i],
                        FSI_COMM_CTRL_SH,
                        FSI_COMM_CTRL_SH,
                        FSI_COMM_CTRL_GRAY);
    text_set_align_type(p_btn[i], STL_CENTER | STL_VCENTER);
    text_set_offset(p_btn[i], 40, 0);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    //text_set_content_by_strid(p_btn[i], btn_str[i]);

    ctrl_set_related_id(p_btn[i],
                        0,                                       
                        (u8)((i - 1 + (IDC_ITEM_MAX-1))%(IDC_ITEM_MAX-1) + 1),                
                        0,                                        
                        (u8)((i + 1) % (IDC_ITEM_MAX-1) + 1));      
  }
#endif
  ChangeLevelMenu(p_bg,g_ucLevelType,g_dwFocusBtnID, FALSE);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);  
  
  return SUCCESS;
}
//lint +e656

#ifdef ENABLE_NETWORK
static BOOL check_network_stat(void)
{
  net_conn_stats_t net_stat;
  net_stat = ui_get_net_connect_status();
#ifdef WIN32
  return TRUE;
#else
  return (net_stat.is_eth_insert && net_stat.is_eth_conn)
  	      || (net_stat.is_wifi_insert && net_stat.is_wifi_conn)  	      
          || (net_stat.is_3g_insert && net_stat.is_3g_conn)
  	      || (net_stat.is_gprs_conn);

#endif
}

static BOOL check_livetv_entry_sts(void)
{
	net_conn_stats_t net_stat;
	net_stat = ui_get_net_connect_status();
#ifdef WIN32
	return TRUE;
#else
	if((net_stat.is_eth_conn == FALSE) && (net_stat.is_wifi_conn == FALSE) && (net_stat.is_3g_conn == FALSE))
	{
		return FALSE;
	}
	return TRUE;
#endif
}
#endif

static RET_CODE on_item_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 dwFocusBtn = 0;
  RET_CODE iRet = SUCCESS;
  u16 ii=0;
  u16 jj=0;
  u8 HideCount = 0;
  BOOL bPassPsw = FALSE;
  pwd_set_t pwd_set;
  BOOL is_db_full = FALSE;
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_MAINMENU,
    PWD_DLG_FOR_CHK_X,
    PWD_DLG_FOR_CHK_Y,
    IDS_MSG_INPUT_PASSWORD,
    pwdlg_keymap_in_menu,
    pwdlg_proc_in_menu,
    PWDLG_T_COMMON
  };
  comm_pwdlg_data_t factory_pwdlg_data =
  {
    ROOT_ID_MAINMENU,
    PWD_DLG_FOR_CHK_X,
    PWD_DLG_FOR_CHK_Y,
    IDS_MSG_INPUT_PASSWORD,
    main_menu_factory_pwdlg_keymap,
    main_menu_factory_pwdlg_proc,
    PWDLG_T_COMMON
  };
  comm_dlg_data_t p_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    USB_NOTIFY_L, USB_NOTIFY_T,
    USB_NOTIFY_W, USB_NOTIFY_H,
    IDS_PLEASE_CONNECT_USB,
    3000,
  };
#ifdef ENABLE_NETWORK
  comm_dlg_data_t network_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    USB_NOTIFY_L, USB_NOTIFY_T,
    USB_NOTIFY_W, USB_NOTIFY_H,
    IDS_DLNA_WARNING_INFO1,
    3000,
  };
  comm_dlg_data_t livetv_dlg_data =
  {
    ROOT_ID_MAINMENU,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_GET_LIVETV_SUCCESS,
    2000,
  };
#endif

  dwFocusBtn = g_dwFocusBtnID;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(TRUE == g_MainMenuMap[ii].bNeedPWD )
      {
        sys_status_get_pwd_set(&pwd_set);
        ui_comm_pwdlg_open(&pwdlg_data);
        bPassPsw = TRUE;
      }
      else
      {
        bPassPsw = TRUE;
      }

      if(bPassPsw)
      {
        if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
        {
          ui_comm_dlg_close();
        }

        if(msg == MSG_FOCUS_LEFT || msg == MSG_FOCUS_RIGHT || msg == MSG_EMPTY)
        {
          switch(msg)
          {
            case MSG_FOCUS_LEFT:
              if(g_ucLevelType == MAINMENU_CHANNEL_LIST)
              {
                g_ucLevelType = MAINMENU_MAX_LEVEL - 1;
              }
              else
              {
                g_ucLevelType--;
              }
              break;

            case MSG_FOCUS_RIGHT:
              if(g_ucLevelType == MAINMENU_MAX_LEVEL - 1)
              {
                g_ucLevelType = MAINMENU_CHANNEL_LIST;
              }
              else
              {
                g_ucLevelType++;
              }
              break;

            default:
              break;
          }
          for(jj=g_nStartPaint; jj<=g_nEndPaint; jj++)
          {
             g_MainMenuMap[jj].cur_lev_focus = 0;
          }
          ChangeLevelMenu(p_ctrl->p_parent,g_ucLevelType,(u8)(IDC_ITEM_1+g_MainMenuMap[ii].next_lev_focus), FALSE);
          ctrl_paint_ctrl(p_ctrl->p_parent->p_parent, TRUE);
          break;
        }
        else if(msg == MSG_SELECT)
        {
          switch(g_MainMenuMap[ii].dwStringID)
          {
            case IDS_FACTORY_SET:
              ui_comm_pwdlg_open(&factory_pwdlg_data);
              break;
            case IDS_DELETE_ALL:
              if (sys_status_get_curn_prog_mode() != CURN_MODE_NONE)
              {
                comm_pwdlg_data_t delete_all_pwdlg_data =
                {
                    ROOT_ID_MAINMENU,
                    PWD_DLG_FOR_CHK_X,
                    PWD_DLG_FOR_CHK_Y,
                    IDS_MSG_INPUT_PASSWORD,
                    main_menu_delete_all_pwdlg_keymap,
                    main_menu_delete_all_pwdlg_proc,
                    PWDLG_T_COMMON
                };
                ui_comm_pwdlg_open(&delete_all_pwdlg_data);
              }
              break;
            #ifdef UDRM_CA
            case IDS_UDRM_MENU:
            #else
            case IDS_CA:
            #endif
              #ifdef ENABLE_CA
              cas_manage_process_menu(SUB_MENU_CONDITIONAL,p_ctrl, para1, para2);
                #ifdef ENABLE_ADS
                    pic_adv_stop();
                #endif
              #endif
              break;
            case IDS_UPGRADE_BY_USB:
            case IDS_TS_RECORD:
              if(!(ui_get_usb_status()))
              {
                ui_comm_dlg_open(&p_data);
              }
              else
              {
                g_dwFocusBtnID = dwFocusBtn;
                if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
                {
                  #ifdef ENABLE_ADS
                    // on_focus_changed will do this, so here just do nothing.
                    //pic_adv_stop();
                  #else
                    ui_close_ads_pic();
                  #endif
                  manage_open_menu(g_MainMenuMap[ii].root_id, 0, 0);
                }
              }
              break;
	     case IDS_AUTO_SEARCH:
		 if(CUSTOMER_ID == CUSTOMER_AISAT_HOTEL)
		 {
                   comm_pwdlg_data_t search_pwdlg_data =
                   {
                      ROOT_ID_MAINMENU,
                      PWD_DLG_FOR_CHK_X,
                      PWD_DLG_FOR_CHK_Y,
                      IDS_MSG_INPUT_PASSWORD,
                      pwdlg_keymap_in_menu,
                      search_proc_in_menu,
                      PWDLG_T_COMMON
                    };
                    g_search_root_id = g_MainMenuMap[ii].root_id;
                    g_dwFocusBtnID = dwFocusBtn;
                    ui_comm_pwdlg_open(&search_pwdlg_data);
                }
		  else
                {
                    g_dwFocusBtnID = dwFocusBtn;
                    if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
                    {
                       #ifdef ENABLE_ADS
                       //pic_adv_stop();
                       #else
                       ui_close_ads_pic();
                       #endif
                       manage_open_menu(g_MainMenuMap[ii].root_id, ROOT_ID_MAINMENU, 0);
                     }
                 }
		   break;
            case IDS_RANGE_SEARCH:
            case IDS_MANUAL_SEARCH:
              sys_status_get_status(BS_DB_FULL, &is_db_full);
              if(is_db_full)
              {
                ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, NULL, 0);
              }
	       else if(CUSTOMER_ID == CUSTOMER_AISAT_HOTEL)
              {
                comm_pwdlg_data_t search_pwdlg_data =
                {
                  ROOT_ID_MAINMENU,
                  PWD_DLG_FOR_CHK_X,
                  PWD_DLG_FOR_CHK_Y,
                  IDS_MSG_INPUT_PASSWORD,
                  pwdlg_keymap_in_menu,
                  search_proc_in_menu,
                  PWDLG_T_COMMON
                };
                g_search_root_id = g_MainMenuMap[ii].root_id;
                g_dwFocusBtnID = dwFocusBtn;
                ui_comm_pwdlg_open(&search_pwdlg_data);
              }
              else
              {
                g_dwFocusBtnID = dwFocusBtn;
                if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
                {
                  #ifdef ENABLE_ADS
                    // on_focus_changed will do this, so here just do nothing.
                    //pic_adv_stop();
                  #else
                    ui_close_ads_pic();
                  #endif
                  manage_open_menu(g_MainMenuMap[ii].root_id, ROOT_ID_MAINMENU, 0);
                }
              }
              break;
	     case IDS_USB_IMPORT:
		if(!(ui_get_usb_status()))
              {
                ui_comm_dlg_open(&p_data);
              }
		else
		{
		   comm_pwdlg_data_t import_pwdlg_data =
                 {
                    ROOT_ID_MAINMENU,
                    PWD_DLG_FOR_CHK_X,
                    PWD_DLG_FOR_CHK_Y,
                    IDS_MSG_INPUT_PASSWORD,
                    main_menu_import_pwdlg_keymap,
                    main_menu_import_pwdlg_proc,
                    PWDLG_T_COMMON
                  };
                  ui_comm_pwdlg_open(&import_pwdlg_data);
		  }
		  break;
		case IDS_USB_EXPORT:
		if(!(ui_get_usb_status()))
              {
                ui_comm_dlg_open(&p_data);
              }
		else
		{
		   comm_pwdlg_data_t export_pwdlg_data =
                 {
                    ROOT_ID_MAINMENU,
                    PWD_DLG_FOR_CHK_X,
                    PWD_DLG_FOR_CHK_Y,
                    IDS_MSG_INPUT_PASSWORD,
                    main_menu_export_pwdlg_keymap,
                    main_menu_export_pwdlg_proc,
                    PWDLG_T_COMMON
                  };
                  ui_comm_pwdlg_open(&export_pwdlg_data);
		  }
		  break;
#ifdef ENABLE_NETWORK
            case IDS_IPTV:
              if (!check_livetv_entry_sts())
              {
                ui_comm_dlg_open(&network_data);
              }
              else
              {
                if(para2 == 1) //used for open livetv after get livetv data
                {
                  ui_comm_dlg_open(&livetv_dlg_data);
                }
                {
                  if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
                  {
                    manage_open_menu(g_MainMenuMap[ii].root_id, ROOT_ID_MAINMENU, 0);
                  }
                }
              }
              break;
#endif
            default:
#ifdef ENABLE_UI_MEDIA
              if(!(ui_get_usb_status()) && (g_ucLevelType==MAINMENU_MEDIA_CENTER))
              {
                ui_comm_dlg_open(&p_data);
              }
              else
#endif
#ifdef ENABLE_NETWORK
              if (!check_network_stat() && 
                g_ucLevelType == MAINMENU_NETWORK &&
                g_MainMenuMap[ii].dwStringID != IDS_NETWORK_CONFIG &&
                g_MainMenuMap[ii].dwStringID != IDS_STB_NAME_SET &&
                g_MainMenuMap[ii].dwStringID != IDS_MAC_ADDR)
              {
                ui_comm_dlg_open(&network_data);
              }
              else
#endif
              {
                g_dwFocusBtnID = dwFocusBtn;
                if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
                {
                  #ifdef ENABLE_ADS
                    // on_focus_changed will do this, so here just do nothing.
                    //pic_adv_stop();
                  #else
                    ui_close_ads_pic();
                  #endif
                  manage_open_menu(g_MainMenuMap[ii].root_id, ROOT_ID_MAINMENU, 0);
                }
              }
              break;
          }
        }
      }
      break;
    }

    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }

  return iRet;
}

static RET_CODE on_item_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 dwFocusBtn = 0;
  u16 ii=0, jj=0;
  u8 HideCount = 0;
  
  dwFocusBtn = g_dwFocusBtnID;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(-1 != g_MainMenuMap[ii].dwPrevious)
      {
        ii = 0;
        //计算将要竦媒沟愕目丶?
        while(g_MainMenuMap[ii].nNext != g_ucLevelType)
        {
          ii++;
        }

        for(jj=0; jj<ii; jj++)
        {
          if(g_MainMenuMap[ii].dwLevel == g_MainMenuMap[jj].dwLevel)
          {
            if(g_MainMenuMap[jj].bNeedHide)
            {
              HideCount++;
            }
          }
        }

        g_MainMenuMap[ii].next_lev_focus = (u16)(dwFocusBtn-IDC_ITEM_1);
        CalStartToEnd((u8)g_MainMenuMap[ii].dwLevel);//更新g_nStartPaint，用于计算下一个得焦愕腷tn
        ChangeLevelMenu(p_ctrl->p_parent,(u8)g_MainMenuMap[ii].dwLevel, g_dwBtnID[ii-g_nStartPaint-HideCount], FALSE);
        ctrl_paint_ctrl(p_ctrl->p_parent->p_parent, TRUE);
        break;
      }
      else
      {
      #ifdef ENABLE_ADS
        pic_adv_stop();
      #endif
        if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
        {
          ui_set_front_panel_by_str("----");
          ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
          return SUCCESS;
        }
        else
        {
          manage_close_menu(ROOT_ID_MAINMENU, 0, 0);
        }
        break;
      }
    }

    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }
  return SUCCESS;

}

static RET_CODE on_mainmenu_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_close_ads_pic();
  return ERR_NOFEATURE;
}
static RET_CODE on_item_ts_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
   u16 ii=0;
   for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
   {
       if(g_MainMenuMap[ii].dwStringID == IDS_TS_RECORD)
       {
	    g_MainMenuMap[ii].bNeedHide = (g_MainMenuMap[ii].bNeedHide == TRUE) ? FALSE : TRUE;
	}
   }
   on_item_update(p_ctrl,MSG_EMPTY,para1,para2);
   return SUCCESS;
}
static RET_CODE calc_focus()
{
  u16 ii=0, jj=0;
  u8 HideCount = 0;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
     g_MainMenuMap[ii].cur_lev_focus = (u16)(g_dwFocusBtnID-IDC_ITEM_1);

    if((g_dwFocusBtnID == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(-1 != g_MainMenuMap[ii].dwPrevious)
      {
        ii = 0;
        //计算将要获得焦点的控?
        while(g_MainMenuMap[ii].nNext != g_ucLevelType)
        {
          ii++;
        }

        for(jj=0; jj<ii; jj++)
        {
          if(g_MainMenuMap[ii].dwLevel == g_MainMenuMap[jj].dwLevel)
          {
            if(g_MainMenuMap[jj].bNeedHide)
            {
              HideCount++;
            }
          }
        }

        g_MainMenuMap[ii].next_lev_focus = (u16)(g_dwFocusBtnID-IDC_ITEM_1);
        break;
      }
      else
      {
        //manage_close_menu(ROOT_ID_MAINMENU, 0, 0);
      }
    }

    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }

  return SUCCESS;

}


static RET_CODE on_btn_cont_msg(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  control_t *new_act_ctrl;
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
    ui_comm_dlg_close();
  }

  cont_class_proc(p_ctrl, msg, para1, para2);
  new_act_ctrl = ctrl_get_active_ctrl(p_ctrl);

  g_dwFocusBtnID = (u8)new_act_ctrl->id;

  calc_focus();
  return SUCCESS;
}

static RET_CODE on_item_change_focus(control_t *p_ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
   // on_btn_cont_msg(p_ctrl->p_parent, msg, para1, para2);
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
    ui_comm_dlg_close();
  }
  
  list_class_proc(p_ctrl, msg, para1, para2);

  g_dwFocusBtnID = (u8)list_get_focus_pos(p_ctrl);

  calc_focus();
  return SUCCESS;
}
static RET_CODE on_search_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();

  g_dwFocusBtnID = g_dwFocusBtnID;
  if(fw_find_root_by_id(g_search_root_id) == NULL)
  {
    #ifdef ENABLE_ADS
      pic_adv_stop();
    #else
      ui_close_ads_pic();
    #endif
    manage_open_menu(g_search_root_id, 0, 0);
  }
  return SUCCESS;
}
static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    ui_set_front_panel_by_str("----");
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
    return SUCCESS;
  }
  #ifdef ENABLE_ADS
  pic_adv_stop();
  #endif
  return ERR_NOFEATURE;
}


static RET_CODE on_exit_mainmenu(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    ui_set_front_panel_by_str("----");
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
    return SUCCESS;
  }
  #ifdef ENABLE_ADS
  pic_adv_stop();
  #endif
  return ERR_NOFEATURE;
}


static RET_CODE on_save_mainmenu(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  if(sys_status_get_curn_prog_mode() == CURN_MODE_NONE)
  {
    fw_notify_root(p_ctrl, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    // break "close all menus"
    return ERR_FAILURE;
  }

  return SUCCESS;
}


static RET_CODE on_mainmenu_plugin(control_t *p_ctrl,
                                   u16 msg,
                                   u32 para1,
                                   u32 para2)
{
#if 0
  control_t *p_mbox = ctrl_get_child_by_id(p_ctrl, IDC_MAINMENU_MBOX);

  mbox_set_item_status(p_mbox, MAINMENU_REC_INDEX, MBOX_ITEM_NORMAL);

  mbox_draw_item_ext(p_mbox, MAINMENU_REC_INDEX, TRUE);
#endif
  return SUCCESS;
}


static RET_CODE on_mainmenu_plugout(control_t *p_ctrl,
                                    u16 msg,
                                    u32 para1,
                                    u32 para2)
{
#if 0
  control_t *p_mbox = ctrl_get_child_by_id(p_ctrl, IDC_MAINMENU_MBOX);
  control_t *p_mbox_icon = ctrl_get_child_by_id(p_ctrl, IDC_MAINMENU_ICON);

  mbox_set_item_status(p_mbox, MAINMENU_REC_INDEX, MBOX_ITEM_DISABLED);

  if(MAINMENU_REC_INDEX == mbox_get_focus(p_mbox))
  {
    mbox_set_focus(p_mbox, 0);
    mbox_set_focus(p_mbox_icon, 0);
    mbox_draw_item_ext(p_mbox, 0, TRUE);
    g_curn_menu = 0;
  }

  mbox_draw_item_ext(p_mbox, MAINMENU_REC_INDEX, TRUE);
  ctrl_paint_ctrl(p_ctrl,TRUE);
#endif
  return SUCCESS;
}

static RET_CODE on_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}


static RET_CODE on_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}


static RET_CODE on_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();
  /* to play */
  if(g_curn_menu == 1)
  {
    //manage_open_menu(ROOT_ID_SUBMENU,
                     //(u32)g_menu_entrys[g_curn_menu], 0);
  }
  else
  {
    //manage_open_menu(ROOT_ID_SUBMENU,
                    // (u32)g_menu_entrys[g_curn_menu], 0);
  }

  return SUCCESS;
}
extern void categories_mem_reset(void);
void restore_to_factory(void)
{
  u16 view_id;
  utc_time_t  p_time;
  BOOL is_scart = FALSE;
  language_set_t lang_set;

  //reset timeshift
  g_ts_jump_select = 0;
  // set front panel
  ui_set_front_panel_by_str("----");
  ui_stop_play(STOP_PLAY_BLACK, TRUE);


  // disable epg at first
//  ui_epg_stop();
//  ui_epg_release();

  // kill tmr

  //set default use common buffer as ext buffer
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
  sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
  sys_status_load();
  categories_mem_reset();
  if(is_scart)
 {
 	sys_status_set_status(BS_IS_SCART_OUT, TRUE);
 }
  sys_status_check_group();
  sys_status_save();

  //clear history
  ui_clear_play_history();

  // set environment according ss_data
  sys_status_reload_environment_setting(TRUE);

  sys_status_get_utc_time( &p_time);
  time_set(&p_time);
  
    //set ca language
  memset(&lang_set, 0, sizeof(language_set_t));
  sys_status_get_lang_set(&lang_set);
#ifdef ENABLE_CA
  if(CAS_ID == CONFIG_CAS_ID_DS)
  {
    OS_PRINTF("@@%s, line = %d, language_index = %d \n",__FUNCTION__,__LINE__,lang_set.osd_text);
    ui_ca_set_language(lang_set);
  }
  if(CAS_ID == CONFIG_CAS_ID_UNITEND)
  {
    if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT))
      fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
  }
#endif
  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }
  if (ui_is_notify())
  {
    ui_set_notify(NULL, NOTIFY_TYPE_STRID, RSC_INVALID_ID, 0);
  }

  // send cmd to ap frm and wait for finish

  //create a new view after load default, and save the new view id.
  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id((u8)view_id);

  //if(CUSTOMER_ID != CUSTOMER_DEFAULT)
  {
    //stop monitor service
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
    if(CUSTOMER_JIZHONGMEX == CUSTOMER_ID)
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_FULL, 0);
    else
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
  }

}

static RET_CODE on_factory_pwdlg_cancel(control_t *p_ctrl,
                                u16 msg,
                                u32 para1,
                                u32 para2)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_factory_pwdlg_correct(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  ui_comm_pwdlg_close();

  ui_comm_ask_for_savdlg_open(&g_restore_dlg_rc,
                                IDS_MSG_ASK_FOR_RESTORE_TO_FACTORY,
                                restore_to_factory, NULL, 0);


  update_signal();
#if 0
  {//fix bug 33253 33225
    u8 idx;
    rect_t orc;
    idx = manage_find_preview(ROOT_ID_MAINMENU, &orc);
    if(idx != INVALID_IDX)
    {
      avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
      avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                          orc.left, orc.top, orc.right, orc.bottom);
      //avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                           //orc.left, orc.top, orc.right, orc.bottom);
    }
  }
#endif
  return SUCCESS;
}
extern void create_categories(void);

#ifdef ONLY1_CA_VER
void delete_all_prog(void)
#else
static void delete_all_prog(void)
#endif
{
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  ui_dbase_delete_all_pg();

  //categories_mem_reset();
  //sys_group_reset();

  sys_status_check_group();
  sys_status_save();

  ui_clear_play_history();

  book_check_node_on_delete();

  //create_categories();
  
  ui_set_front_panel_by_str("----");

  //if(CUSTOMER_ID != CUSTOMER_DEFAULT)
  {
    //stop monitor service
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
    if(CUSTOMER_JIZHONGMEX == CUSTOMER_ID)
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_FULL, 0);
    else
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
  }
}

static RET_CODE on_delete_all_pwdlg_cancel(control_t *p_ctrl,
												u16 msg,
							                                u32 para1,
							                                u32 para2)
{
  u16 key;
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_delete_all_pwdlg_correct(control_t *p_ctrl,
                                                  u16 msg,
                                                  u32 para1,
                                                  u32 para2)
{
  ui_comm_pwdlg_close();

  ui_comm_ask_for_savdlg_open(&g_del_all_dlg_rc,
                              IDS_MSG_ASK_FOR_DEL,
                              delete_all_prog,NULL,0);
  return SUCCESS;
}

static RET_CODE on_delete_all_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_import_pwdlg_cancel(control_t *p_ctrl,
												u16 msg,
							                                u32 para1,
							                                u32 para2)
{
  u16 key;
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_import_pwdlg_correct(control_t *p_ctrl,
                                                  u16 msg,
                                                  u32 para1,
                                                  u32 para2)
{
  ui_comm_pwdlg_close();
  OS_PRINTF("start import\n");
  if(ui_get_usb_status() && ui_find_usb_pgdata_file())
  {
    OS_PRINTF("ready to import\n");    
    ui_dbase_pgdata_usb_to_db();
  }
  else
  {
    OS_PRINTF("import unsuccess\n");
    ui_comm_cfmdlg_open(NULL, IDS_IMPORT_USB_PRESET_CHANNEL_FAIL, NULL, 3000);
    update_signal();
  }
  return SUCCESS;
}

static RET_CODE on_import_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_export_pwdlg_cancel(control_t *p_ctrl,
												u16 msg,
							                                u32 para1,
							                                u32 para2)
{
  u16 key;
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_export_pwdlg_correct(control_t *p_ctrl,
                                                  u16 msg,
                                                  u32 para1,
                                                  u32 para2)
{
    rect_t upgrade_dlg_rc =
    {
       ((SCREEN_WIDTH - 400) / 2),
       ((SCREEN_HEIGHT - 200) / 2),
       ((SCREEN_WIDTH - 400) / 2) + 400,
       ((SCREEN_HEIGHT - 200) / 2) + 200,
    };
  ui_comm_pwdlg_close();
  OS_PRINTF("start export\n");
  if(ui_get_usb_status())
  {
    ui_comm_ask_for_dodlg_open(&upgrade_dlg_rc, IDS_EXPORTING_PRESET_CHANNEL,
                               ui_dbase_pgdata_db_to_usb,   NULL, 0);
  }
  return SUCCESS;
}

static RET_CODE on_export_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}
static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("on_item_refresh_ads\n");
  #ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_MENU_UP, ROOT_ID_MAINMENU);
  #endif
  return SUCCESS;
}

static RET_CODE on_ca_hide_menu(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  OS_PRINTF("ENTER hide menu!!!!\n");
  #ifdef ENABLE_CA
    cas_manage_process_menu(CA_OPEN_HIDE_CA_MENU, p_cont, para1, para2);
  #endif
  return SUCCESS;
}

#ifdef ENABLE_NETWORK
static RET_CODE on_open_livetv_menu_after_get_livetv_data(control_t *p_ctrl, 
u16 msg, u32 para1, u32 para2)
{
  control_t *p_list = NULL;
  int ii;
  u8 dwFocusBtn;
  u8 HideCount = 0;

  OS_PRINTF("######%s, line[%d]#########\n", __FUNCTION__, __LINE__);

  dwFocusBtn = g_dwFocusBtnID;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && 
      (!g_MainMenuMap[ii].bNeedHide) &&
      (g_MainMenuMap[ii].dwStringID == IDS_IPTV))
    {
      p_list = ui_comm_root_get_ctrl(ROOT_ID_MAINMENU, IDC_MAINMENU_ITEM_LIST);
      on_item_update(p_list, MSG_SELECT, para1, 1);
      OS_PRINTF("Start to open IPTV app...\n");
      return SUCCESS;
    }

    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }

  OS_PRINTF("Focus is not on IPTV, ignore openning IPTV app.\n");
  return ERR_FAILURE;
}
#endif

BEGIN_KEYMAP(main_menu_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_BLUE, MSG_EMPTY)
END_KEYMAP(main_menu_item_keymap, NULL)
#if 1
BEGIN_MSGPROC(main_menu_item_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_update)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_update)
  ON_COMMAND(MSG_SELECT, on_item_update)
  ON_COMMAND(MSG_EXIT, on_item_exit)
  ON_COMMAND(MSG_EMPTY, on_item_ts_show)
END_MSGPROC(main_menu_item_proc, list_class_proc)
#else
BEGIN_MSGPROC(main_menu_item_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_item_update)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_item_update)
  ON_COMMAND(MSG_SELECT, on_item_update)
  ON_COMMAND(MSG_EXIT, on_item_exit)
END_MSGPROC(main_menu_item_proc, text_class_proc)
#endif
BEGIN_MSGPROC(main_menu_btn_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_btn_cont_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_btn_cont_msg)
END_MSGPROC(main_menu_btn_cont_proc, cont_class_proc)

BEGIN_KEYMAP(main_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CA_HIDE_MENU, MSG_CA_HIDE_MENU)
  ON_EVENT(V_KEY_GREEN, MSG_CA_HIDE_MENU)
END_KEYMAP(main_menu_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(main_menu_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
  ON_COMMAND(MSG_EXIT, on_exit_mainmenu)
  ON_COMMAND(MSG_SAVE, on_save_mainmenu)
  ON_COMMAND(MSG_PLUG_IN, on_mainmenu_plugin)
  ON_COMMAND(MSG_PLUG_OUT, on_mainmenu_plugout)
  ON_COMMAND(MSG_DESTROY, on_mainmenu_destroy)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
  ON_COMMAND(MSG_CA_HIDE_MENU, on_ca_hide_menu)
#ifdef ENABLE_NETWORK
  ON_COMMAND(MSG_OPEN_LIVETV_MENU, on_open_livetv_menu_after_get_livetv_data)
#endif
END_MSGPROC(main_menu_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pwdlg_keymap_in_menu, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
END_KEYMAP(pwdlg_keymap_in_menu, NULL)

BEGIN_MSGPROC(pwdlg_proc_in_menu, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
END_MSGPROC(pwdlg_proc_in_menu, cont_class_proc)

BEGIN_KEYMAP(main_menu_factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_menu_factory_pwdlg_keymap, NULL)

BEGIN_MSGPROC(main_menu_factory_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_factory_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_factory_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_factory_pwdlg_exit)
END_MSGPROC(main_menu_factory_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(main_menu_delete_all_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_menu_delete_all_pwdlg_keymap, NULL)

BEGIN_MSGPROC(main_menu_delete_all_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_delete_all_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_delete_all_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_delete_all_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_delete_all_pwdlg_exit)
END_MSGPROC(main_menu_delete_all_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(main_menu_import_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_menu_import_pwdlg_keymap, NULL)

BEGIN_MSGPROC(main_menu_import_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_import_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_import_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_import_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_import_pwdlg_exit)
END_MSGPROC(main_menu_import_pwdlg_proc, cont_class_proc)

BEGIN_KEYMAP(main_menu_export_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_menu_export_pwdlg_keymap, NULL)

BEGIN_MSGPROC(main_menu_export_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_export_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_export_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_export_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_export_pwdlg_exit)
END_MSGPROC(main_menu_export_pwdlg_proc, cont_class_proc)

BEGIN_MSGPROC(search_proc_in_menu, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_search_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
END_MSGPROC(search_proc_in_menu, cont_class_proc)

