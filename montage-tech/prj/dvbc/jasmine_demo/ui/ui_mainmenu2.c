/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_mainmenu2.h"
#include "ui_mute.h"
#include "ui_pause.h"

#include "ui_notify.h"
#include "customer_config.h"

enum main_menu_control_id
{
  IDC_INVALID = 0,
  IDC_BTN_1,
  IDC_BTN_2,
  IDC_BTN_3,
  IDC_BTN_4,
  IDC_BTN_5,
  IDC_BTN_6,
  IDC_BTN_7,
  IDC_BTN_8,
  IDC_BTN_MAX,
  
  IDC_ICON_1,
  IDC_ICON_2,
  IDC_ICON_3,
  IDC_ICON_4,
  IDC_ICON_5,
  IDC_ICON_6,
  IDC_ICON_7,
  IDC_ICON_8,
  
  IDC_BTN_CONT,
  IDC_TV_WIN,
  IDC_AD_WIN,
  
  IDC_MAINMENU_MBOX,
  IDC_MAINMENU_ICON,
  IDC_MAINMENU_PAUSE_ICON,
  IDC_MAINMENU_MUTE_ICON,
};

//MAINTYPE与g_dwIDSName一一对应
typedef enum _MAINMENUTYPE
{
  //一级
  MAINMENU_HOME,

  //二级
  MAINMENU_CHANNEL_LIST,
  MAINMENU_INSTALLATION,
  MAINMENU_SYSTEM,
  MAINMENU_TOOLS,
  MAINMENU_MEDIA_CENTER,
  MAIN_MENU_NVOD,

}MAINMENUTYPE;

//g_dwIDSName隡AINTYPE一一对应
static u16 g_dwIDSName[]=
{
  IDS_HOME,
  IDS_CHANNEL_LIST,
  IDS_INSTALLATION,
  IDS_SYSTEM,
  IDS_TOOLS,
  IDS_MEDIA_CENTER,
  IDS_NVOD_NVOD,
};

//菜单的定义,该结构是用于3种类型的菜单
typedef struct _MainMenuStruct
{
  s8    dwPrevious;        //霞恫说ゼ侗?绻挥校蛭?1
  s8    dwLevel;           //菜单级别
  u16   dwStringID;        // button上文字的ID
  s8    nNext;             //下级菜单级穑绻挥校蛭?1
  u8    root_id;           //只有当nNext为-1时,才有效
  u32   lParam;            //对话虺跏蓟数
  BOOL  bNeedPWD;          //need password or not
  BOOL  bNeedHide;         //是否需要隐藏
  u16   next_lev_focus;    //next level button focus
  u16   cur_lev_focus;     //current level button focus,  the low 8bit indicates the focus when initial level, the high 8 bit indicates the focus when the level show hide button 
  comm_help_data_t *p_help;
}MAINMAINMENUSTRUCT;

static MAINMAINMENUSTRUCT g_MainMenuMap[] = 
{
  //(Level 1):HOME
  {-1, MAINMENU_HOME, IDS_CHANNEL_LIST, MAINMENU_CHANNEL_LIST, ROOT_ID_INVALID,   0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_HOME, IDS_INSTALLATION, MAINMENU_INSTALLATION,  ROOT_ID_INVALID,    0, FALSE, FALSE,  0, 0, 0},
  {-1, MAINMENU_HOME, IDS_SYSTEM, MAINMENU_SYSTEM, ROOT_ID_INVALID,    0, FALSE, FALSE, 0, 0, 0},
  {-1, MAINMENU_HOME, IDS_TOOLS, MAINMENU_TOOLS, ROOT_ID_INVALID,    0, FALSE, FALSE,  0, 0, 0},
  {-1, MAINMENU_HOME, IDS_MEDIA_CENTER, MAINMENU_MEDIA_CENTER, ROOT_ID_INVALID,   0, FALSE, FALSE, 0, 0, 0},
#ifdef NVOD_ENABLE
  {-1, MAINMENU_HOME, IDS_NVOD_NVOD,  -1,  ROOT_ID_NVOD,      0, FALSE, FALSE, 0, 0, 0},
#endif
  //(Level 2):channel list
  {MAINMENU_HOME, MAINMENU_CHANNEL_LIST, IDS_CHANNEL_LIST, -1, ROOT_ID_PROG_LIST,   0, FALSE,  FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_CHANNEL_LIST, IDS_CHANNEL_EDIT, -1, ROOT_ID_CHANNEL_EDIT,   0, FALSE,  FALSE, 0, 0, 0},

  //(Level 2):installation
  {MAINMENU_HOME, MAINMENU_INSTALLATION, IDS_AUTO_SEARCH, -1,  ROOT_ID_AUTO_SEARCH,   0, FALSE,  FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_INSTALLATION, IDS_MANUAL_SEARCH, -1,  ROOT_ID_MANUAL_SEARCH,    0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_INSTALLATION, IDS_RANGE_SEARCH, -1,  ROOT_ID_RANGE_SEARCH,  0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_INSTALLATION, IDS_UPGRADE_BY_SAT, -1,  ROOT_ID_OTA, 0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_INSTALLATION, IDS_FACTORY_SET, -1,  ROOT_ID_FACTORY_SET,        0, FALSE,  FALSE, 0, 0, 0},

  //(Level 2):system
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_LANGUAGE, -1,  ROOT_ID_LANGUAGE,      0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_AV_SETTING, -1,  ROOT_ID_TVSYS_SET,            0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_LOCAL_TIME_SET, -1,  ROOT_ID_TIME_SET,       0, FALSE,  FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_BOOK_MANAGE, -1,  ROOT_ID_BOOK_LIST,      0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_PARENTAL_LOCK, -1,  ROOT_ID_PARENTAL_LOCK,            0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_OSD_SET, -1,  ROOT_ID_OSD_SET,       0, FALSE,  FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_FAVORITE, -1,  ROOT_ID_FAVORITE,      0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_SYSTEM, IDS_FREQUENCY_SETTING, -1,  ROOT_ID_FREQ_SET,      0, FALSE, FALSE, 0, 0, 0},

  //(Level 2):tools
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_INFORMATION,  -1,  ROOT_ID_INFO,      0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_FACTORY_SET,         -1,  ROOT_ID_FACTORY_SET,    0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_DELETE_ALL,        -1,  ROOT_ID_DELETE_ALL, 0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_UPGRADE_BY_RS232,  -1,  ROOT_ID_UPGRADE_BY_RS232,      0, FALSE, FALSE, 0, 0, 0},
#ifdef ENABLE_USB_CONFIG
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_UPGRADE_BY_USB,         -1,  ROOT_ID_UPGRADE_BY_USB,    0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_BACKUP_BY_USB,  -1,  ROOT_ID_DUMP_BY_USB,      0, FALSE, FALSE, 0, 0, 0},
#endif
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_GAME,         -1,  ROOT_ID_GAME,    0, FALSE, FALSE, 0, 0, 0},
#ifdef USB_TEST
  {MAINMENU_HOME, MAINMENU_TOOLS, IDS_USB_TEST,        -1,  ROOT_ID_USB_TEST, 0, FALSE, FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_MUSIC_PIC_CONFIG  
  //(Level 2):media center
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_MUSIC,         -1, ROOT_ID_USB_MUSIC,         0, FALSE, FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_PICTURE,         -1, ROOT_ID_USB_PICTURE,         0, FALSE, FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_FILE_PLAY
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_MOVIES,      -1, ROOT_ID_USB_FILEPLAY, 0, FALSE,  FALSE, 0, 0, 0},
#endif
#ifdef ENABLE_PVR_REC_CONFIG
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_RECORD_MANAGER,         -1, ROOT_ID_RECORD_MANAGER,         0, FALSE,  FALSE, 0, 0, 0},
#endif
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_HDD_INFO,       -1, ROOT_ID_HDD_INFO,         0, FALSE,  FALSE, 0, 0, 0},
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_STORAGE_FORMAT,         -1, ROOT_ID_STORAGE_FORMAT,         0, FALSE, FALSE, 0, 0, 0},
#ifdef ENABLE_TIMESHIFT_CONFIG
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_DVR_CONFIG,         -1, ROOT_ID_DVR_CONFIG,         0, FALSE, FALSE, 0, 0, 0},
#endif
  {MAINMENU_HOME, MAINMENU_MEDIA_CENTER, IDS_JUMP,      -1, ROOT_ID_JUMP, 0, FALSE,  FALSE, 0, 0, 0},
};

static u8 g_dwBtnID[]=
{
  IDC_BTN_1,
  IDC_BTN_2,
  IDC_BTN_3,
  IDC_BTN_4,
  IDC_BTN_5,
  IDC_BTN_6,
  IDC_BTN_7,
  IDC_BTN_8,
};

static u8 g_dwIconID[]=
{
  IDC_ICON_1,
  IDC_ICON_2,
  IDC_ICON_3,
  IDC_ICON_4,
  IDC_ICON_5,
  IDC_ICON_6,
  IDC_ICON_7,
  IDC_ICON_8,
};

static rect_t g_del_all_dlg_rc =
{
  440,
  260,
  400+440,
  200+260,
};

static rect_t g_restore_dlg_rc =
{
  440,
  260,
  400+440,
  200+260,
};


//static u16 bmp_sh[] = {IM_NUMBER_1, IM_NUMBER_1, IM_NUMBER_1, IM_NUMBER_1, IM_NUMBER_1, IM_NUMBER_1, IM_NUMBER_1,IM_NUMBER_1};
//static u16 bmp_hl[] = {IM_NUMBER_2, IM_NUMBER_2, IM_NUMBER_2, IM_NUMBER_2, IM_NUMBER_2, IM_NUMBER_2, IM_NUMBER_2,IM_NUMBER_2};

static u16   g_nStart = 0;
static u16   g_nStartPaint = 0;

static u16   g_nEnd = 9;
static u16   g_nEndPaint = 9;

static u8 g_dwFocusBtnID = IDC_BTN_1;
static u8 g_ucLevelType = 0;

static u16   g_nCountOfBtn = 0;

extern u16 g_ts_jump_select;

#define MAINMENU_REC_INDEX 4

static u8 g_curn_menu = 0;

u16 pwdlg_keymap_in_menu(u16 key);

u16 t_edit_keymap(u16 key);

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

RET_CODE main_menu_delete_all_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

RET_CODE main_menu_factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

static BOOL CalStartToEnd(u32 LevelType)
{
  u16 ii = 0, jj = 0;
  u16 nCount = sizeof(g_MainMenuMap)/sizeof(MAINMAINMENUSTRUCT);
  u8 hide_count = 0;

  for ( ; ii<nCount; )
  {
    if (g_MainMenuMap[ii].dwLevel == LevelType)
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
    if (g_MainMenuMap[ii].dwLevel != LevelType)
    {
      break;
    } 
    else
    {
      ii++;
    }
  }
  
  g_nEnd = ii - 1;

  for(jj=g_nStart; jj<g_nEnd; jj++)
  {  
    if(g_MainMenuMap[g_nStart+jj].bNeedHide)
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

  return TRUE;
}

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

static BOOL ShowIconNum(control_t * p_btn_cont)
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
    p_ctrl = ctrl_get_child_by_id(p_btn_cont, g_dwIconID[ii]);
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
      p_ctrl = ctrl_get_child_by_id(p_btn_cont, g_dwIconID[ii]);
      if(NULL != p_ctrl)
      {
        if(g_dwFocusBtnID - IDC_BTN_1 == ii)
        {
          ctrl_set_style(p_ctrl, STL_EX_ALWAYS_HL);
        }
        else
        {
          ctrl_set_style(p_ctrl, STL_EX_WHOLE_HL);
        }
      }
      ii++;
    }
  }

  return TRUE;
}

static BOOL ShowTVwin2(control_t * p_btn_cont)
{
  control_t * p_ctrl;

  p_ctrl = ctrl_get_child_by_id(p_btn_cont->p_parent, IDC_AD_WIN);

  ctrl_set_attr(p_ctrl, OBJ_ATTR_ACTIVE);

  return TRUE;
}

static BOOL ShowHelp(control_t * p_btn_cont, BOOL bforce)
{  
  return TRUE;
}

static BOOL  MainMenuShowChange(control_t * p_ctrl, u8 NewLevel, u8 dwBtnID, BOOL bforce_help)
{
  g_ucLevelType  = NewLevel;

  ShowBtn(p_ctrl);
  ShowTVwin2(p_ctrl);
  
  ui_comm_title_set_content(p_ctrl->p_parent, g_dwIDSName[g_ucLevelType]);

  if(ctrl_get_active_ctrl(p_ctrl)!=NULL)
  {
    ctrl_default_proc(ctrl_get_active_ctrl(p_ctrl), MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(ctrl_get_child_by_id(p_ctrl, dwBtnID), MSG_GETFOCUS, 0, 0);

    g_dwFocusBtnID = dwBtnID;
    ShowIconNum(p_ctrl);
  }
  else
  {
    ShowIconNum(p_ctrl);
  }
  ShowHelp(p_ctrl, bforce_help);

  return TRUE;
}

static BOOL  ChangeLevelMenu(control_t * p_ctrl, u8 NewLevel, u8 dwBtnID, BOOL bforce_help)
{
  CalStartToEnd(NewLevel);
  MainMenuShowChange(p_ctrl,NewLevel, dwBtnID, bforce_help);
  return TRUE;
}

RET_CODE open_main_menu(u32 para1, u32 para2)
{
  control_t *p_cont, *p_bg, *p_btn[IDC_BTN_MAX-1], *p_icon, *p_tv_win, *p_ad_win;
  u16 i;
  u16 x,y;
  static comm_help_data_t help_data = 
  {
  4,4,
    {IDS_SELECT,IDS_OK,IDS_MENU,IDS_EXIT},
    {IM_SHORTCUT,IM_OK,IM_MENU,IM_EXIT}
  };
  
  g_nCountOfBtn = sizeof(g_dwBtnID)/sizeof(u8);

  g_nStart = 0;
  g_nStartPaint = 0;
  g_nEnd = 8;
  g_nEndPaint = 8;
  g_dwFocusBtnID = IDC_BTN_1+g_MainMenuMap[0].cur_lev_focus;
  g_ucLevelType = MAINMENU_HOME;
  
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
  
  //TV preview window
  p_tv_win = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_TV_WIN,
                              MAINMENU_PREV_X, MAINMENU_PREV_Y,
                              MAINMENU_PREV_W, MAINMENU_PREV_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_tv_win, RSI_TV, RSI_TV, RSI_TV);

  //AD window
  p_ad_win = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_AD_WIN,
                              MAINMENU_AD_WINDOW_X, MAINMENU_AD_WINDOW_Y,
                              MAINMENU_AD_WINDOW_W, MAINMENU_AD_WINDOW_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ad_win, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);


  // create bg
  p_bg = ctrl_create_ctrl(CTRL_CONT, IDC_BTN_CONT, MAINMENU_CONT_ITEM_X, MAINMENU_CONT_ITEM_Y,
                           MAINMENU_CONT_ITEM_W, MAINMENU_CONT_ITEM_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_bg, RSI_MAINMENU_FRM, RSI_MAINMENU_FRM, RSI_MAINMENU_FRM);
  ctrl_set_proc(p_bg, main_menu_btn_cont_proc);

  //create button
  for(i=0; i<IDC_BTN_MAX-1; i++)
  {

    x = MAINMENU_ITEM_X;
    y = MAINMENU_ITEM_Y + i*(MAINMENU_ITEM_H + MAINMENU_ITEM_V_GAP);

    
    p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_1 + i),
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
    text_set_align_type(p_btn[i], STL_LEFT | STL_VCENTER);
    text_set_offset(p_btn[i], 40, 0);
    text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
    //text_set_content_by_strid(p_btn[i], btn_str[i]);
    
    p_icon = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_ICON_1 + i),
                              MAINMENU_ICON_X, y, MAINMENU_ICON_W,MAINMENU_ICON_H,
                              p_bg, 0);
    
    ctrl_set_rstyle(p_icon,
                    RSI_MENU_ICON_SH,
                    RSI_MENU_ICON_HL,
                    RSI_MENU_ICON_SH);
    text_set_font_style(p_icon,
                    FSI_COMM_CTRL_SH,
                    FSI_COMM_CTRL_SH,
                    FSI_COMM_CTRL_GRAY);
    text_set_align_type(p_icon, STL_CENTER| STL_VCENTER);
    text_set_content_type(p_icon, TEXT_STRTYPE_DEC);
    text_set_content_by_dec(p_icon, i + 1);
    ctrl_set_related_id(p_btn[i],
                        0,                                        /* left */
                        (u8)((i - 1 + (IDC_BTN_MAX-1))%(IDC_BTN_MAX-1) + 1),                 /* up */
                        0,                                        /* right */
                        (u8)((i + 1) % (IDC_BTN_MAX-1) + 1));      /* down */
  }

  ChangeLevelMenu(p_bg,g_ucLevelType,g_dwFocusBtnID, FALSE);
  
  //ui_comm_help_create2((comm_help_data_t2*)&common_help_data, p_cont, FALSE);
  ui_comm_help_create(&help_data, p_cont);
  
  ctrl_default_proc(p_btn[g_dwFocusBtnID-IDC_BTN_1], MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}

static RET_CODE on_item_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 dwFocusBtn = 0;
  RET_CODE iRet = SUCCESS;
  u16 ii=0;
  u8 HideCount = 0;
  //u8 view_id = 0;
  //u16 pg_count = 0;
  //u16 old_view_type;
  //u32 old_group_context;
  BOOL bPassPsw = FALSE;
  //BOOL bNeedStopPic = TRUE;
 // pwdlg_ret_t pw_ret = PWDLG_RET_SUCCESS;
  pwd_set_t pwd_set;
  //u16 content[64];
  //u8 temp[128];
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
  /*
  rect_t rc_dlg = 
  {
    COMM_DLG_X+158,
    COMM_DLG_Y+90,
    COMM_DLG_X+COMM_DLG_W+158,
    COMM_DLG_Y+COMM_DLG_H+90,
  };
  */
  dwFocusBtn = p_ctrl->id;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(TRUE == g_MainMenuMap[ii].bNeedPWD )
      {
        sys_status_get_pwd_set(&pwd_set);
        //pwdlg_data.value = pwd_set.normal;
//        if(pwd_set.enable_psw_lockchannel == 1)
        {
          ui_comm_pwdlg_open(&pwdlg_data);
         // if(pw_ret==PWDLG_RET_SUCCESS)
          {
            bPassPsw = TRUE;
          }
        }
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
        
        if(-1 != g_MainMenuMap[ii].nNext)
        {
          ChangeLevelMenu(p_ctrl->p_parent,g_MainMenuMap[ii].nNext,IDC_BTN_1+g_MainMenuMap[ii].next_lev_focus, FALSE);
          ctrl_paint_ctrl(p_ctrl->p_parent->p_parent, TRUE);
          break;
        } 
        else
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
                /*
                 * when del all pg, update tools menu.
                 */
                ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);
              }
              break;
            default:
            g_dwFocusBtnID = dwFocusBtn;
            //bug 23811
            if(fw_find_root_by_id(g_MainMenuMap[ii].root_id) == NULL)
              manage_open_menu(g_MainMenuMap[ii].root_id, 0, 0);
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
  
  dwFocusBtn = p_ctrl->id;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(-1 != g_MainMenuMap[ii].dwPrevious)
      {
        if(g_MainMenuMap[ii].dwStringID == IDS_10 &&
            fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
        {
          ui_comm_dlg_close();
        }
        ii = 0;
        //计算将要获得焦点的控件
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

        g_MainMenuMap[ii].next_lev_focus = (u16)(dwFocusBtn-IDC_BTN_1);
        CalStartToEnd(g_MainMenuMap[ii].dwLevel);//更新g_nStartPaint，用于计算下一个将要获得焦点的btn
        ChangeLevelMenu(p_ctrl->p_parent,g_MainMenuMap[ii].dwLevel, g_dwBtnID[ii-g_nStartPaint-HideCount], FALSE);
        ctrl_paint_ctrl(p_ctrl->p_parent->p_parent, TRUE);
        break;
      } 
      else
      {
        manage_close_menu(ROOT_ID_MAINMENU, 0, 0);
      }
    }

    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }

  return SUCCESS;

}

static RET_CODE on_item_paint(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  // check item attr before painting
  //if (g_menu_entry->chk_item != NULL)
  //{
  //  g_menu_entry->chk_item(ctrl);
  //}
  // means pass the msg to base
  return ERR_NOFEATURE;
}

static RET_CODE calc_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 dwFocusBtn = 0;
  u16 ii=0, jj=0;
  u8 HideCount = 0;
  
  dwFocusBtn = p_ctrl->id;

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  { 
     g_MainMenuMap[ii].cur_lev_focus = (u16)(dwFocusBtn-IDC_BTN_1);
     
    if((dwFocusBtn == g_dwBtnID[ii-g_nStartPaint-HideCount]) && (!g_MainMenuMap[ii].bNeedHide))
    {
      if(-1 != g_MainMenuMap[ii].dwPrevious)
      {
        ii = 0;
        //计算将要获得焦点的控件
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

        g_MainMenuMap[ii].next_lev_focus = (u16)(dwFocusBtn-IDC_BTN_1);
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
  control_t *old_act_ctrl, *new_act_ctrl, *p_icon;
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
  {
    ui_comm_dlg_close();
  }
  old_act_ctrl = ctrl_get_active_ctrl(p_ctrl);

  cont_class_proc(p_ctrl, msg, para1, para2);
  new_act_ctrl = ctrl_get_active_ctrl(p_ctrl);
  
  g_dwFocusBtnID = new_act_ctrl->id;
  
  ShowIconNum(p_ctrl);

  if(p_ctrl != NULL && old_act_ctrl != NULL && new_act_ctrl != NULL)
  {
    p_icon = ctrl_get_child_by_id(p_ctrl,old_act_ctrl->id-IDC_BTN_1+IDC_ICON_1);
    if(p_icon != NULL)
    {
      ctrl_paint_ctrl(p_icon, TRUE);
    }

    p_icon = ctrl_get_child_by_id(p_ctrl,new_act_ctrl->id-IDC_BTN_1+IDC_ICON_1);
    if(p_icon != NULL)
    {
      ctrl_paint_ctrl(p_icon, TRUE);
    }
  }

  ShowHelp(p_ctrl, TRUE);
  calc_focus(new_act_ctrl, 0, 0 ,0);
  //ui_comm_help_update2(ctrl->p_parent);
  return SUCCESS;
}

static RET_CODE on_item_change_focus(control_t *p_ctrl, u16 msg, 
                                     u32 para1, u32 para2)
{
    on_btn_cont_msg(p_ctrl->p_parent, msg, para1, para2);

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

static void restore_to_factory(void)
{
  u16 view_id;
  utc_time_t  p_time;
  BOOL is_scart = FALSE;


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
  if(is_scart)
 {
 	sys_status_set_status(BS_IS_SCART_OUT, TRUE);
 }
  sys_status_check_group();
  sys_status_save();

  // set environment according ss_data
  sys_status_reload_environment_setting(TRUE);

  sys_status_get_utc_time( &p_time);
  time_set(&p_time);

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
  BOOL ret = 0;
  ui_comm_pwdlg_close();
  
  ret = ui_comm_ask_for_savdlg_open(&g_restore_dlg_rc,
                                IDS_MSG_ASK_FOR_RESTORE_TO_FACTORY,
                                restore_to_factory, NULL, 0);

  ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);

  return SUCCESS;
}

static void delete_all_prog(void)
{
  ui_dbase_delete_all_pg();
  sys_status_check_group();
  sys_status_save();

  book_check_node_on_delete();
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

	ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);

 	 return SUCCESS;
}

static RET_CODE on_delete_all_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

static RET_CODE on_mainmenu_number_input(control_t *p_cont, u16 msg, 
                                     u32 para1, u32 para2)
{
  control_t *p_btn_cont, *old_act_ctrl, *new_act_ctrl;
  u8 ch = msg^MSG_CHAR;
  u8 index = 0;
  u16 ii = 0, HideCount = 0;

  if((ch>='1') && ((ch<='7')))
  {
    index = (ch-'1');
  }
  else
  {
    return ERR_FAILURE;
  }

  for(ii=g_nStartPaint; ii<=g_nEndPaint; ii++)
  {
    if(g_MainMenuMap[ii].bNeedHide)
    {
      HideCount++;
    }
  }

  if(index > g_nEndPaint - g_nStartPaint - HideCount)
  {
    return ERR_FAILURE;
  }



  p_btn_cont= ctrl_get_child_by_id(p_cont, IDC_BTN_CONT);

  old_act_ctrl = ctrl_get_active_ctrl(p_btn_cont);
  g_dwFocusBtnID = IDC_BTN_1 + index;
  new_act_ctrl = ctrl_get_child_by_id(p_btn_cont, g_dwFocusBtnID);

  if(old_act_ctrl != new_act_ctrl)
  {
    ctrl_process_msg(old_act_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_set_attr(new_act_ctrl, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(new_act_ctrl, MSG_GETFOCUS, 0, 0);

    ShowIconNum(p_btn_cont);
    ShowHelp(p_btn_cont, TRUE);
    
    ctrl_paint_ctrl(old_act_ctrl, TRUE);
    ctrl_paint_ctrl(new_act_ctrl, TRUE);
    ctrl_paint_ctrl(ctrl_get_child_by_id(p_btn_cont,old_act_ctrl->id-IDC_BTN_1+IDC_ICON_1), TRUE);
    ctrl_paint_ctrl(ctrl_get_child_by_id(p_btn_cont,new_act_ctrl->id-IDC_BTN_1+IDC_ICON_1), TRUE);
  }

  on_item_select(new_act_ctrl, msg, para1, para2);

  return SUCCESS;
}

BEGIN_KEYMAP(main_menu_item_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(main_menu_item_keymap, NULL)

BEGIN_MSGPROC(main_menu_item_proc, text_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_EXIT, on_item_exit)
  ON_COMMAND(MSG_PAINT, on_item_paint)
END_MSGPROC(main_menu_item_proc, text_class_proc)

BEGIN_MSGPROC(main_menu_btn_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_btn_cont_msg)
  ON_COMMAND(MSG_FOCUS_DOWN, on_btn_cont_msg)
END_MSGPROC(main_menu_btn_cont_proc, cont_class_proc)

BEGIN_KEYMAP(main_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_1, MSG_CHAR | '1')
  ON_EVENT(V_KEY_2, MSG_CHAR | '2')
  ON_EVENT(V_KEY_3, MSG_CHAR | '3')
  ON_EVENT(V_KEY_4, MSG_CHAR | '4')
  ON_EVENT(V_KEY_5, MSG_CHAR | '5')
  ON_EVENT(V_KEY_6, MSG_CHAR | '6')
  ON_EVENT(V_KEY_7, MSG_CHAR | '7')
  ON_EVENT(V_KEY_8, MSG_CHAR | '8')
  ON_EVENT(V_KEY_9, MSG_CHAR | '9')
  ON_EVENT(V_KEY_0, MSG_CHAR | '0')
END_KEYMAP(main_menu_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(main_menu_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
ON_COMMAND(MSG_EXIT, on_exit_mainmenu)
ON_COMMAND(MSG_SAVE, on_save_mainmenu)
ON_COMMAND(MSG_PLUG_IN, on_mainmenu_plugin)
ON_COMMAND(MSG_PLUG_OUT, on_mainmenu_plugout)
ON_COMMAND(MSG_CHAR, on_mainmenu_number_input)
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

BEGIN_KEYMAP(t_edit_keymap, NULL)
ON_EVENT(V_KEY_0, MSG_BACKSPACE)
ON_EVENT(V_KEY_1, MSG_DELETE)
ON_EVENT(V_KEY_2, MSG_CHAR | '2')
ON_EVENT(V_KEY_3, MSG_CHAR | '3')
ON_EVENT(V_KEY_4, MSG_CHAR | '4')
ON_EVENT(V_KEY_5, MSG_CHAR | '5')
ON_EVENT(V_KEY_6, MSG_CHAR | '6')
ON_EVENT(V_KEY_7, MSG_CHAR | '7')
ON_EVENT(V_KEY_8, MSG_CHAR | '8')
ON_EVENT(V_KEY_9, MSG_CHAR | '9')
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(t_edit_keymap, NULL)

BEGIN_KEYMAP(main_menu_factory_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
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
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_menu_delete_all_pwdlg_keymap, NULL)

BEGIN_MSGPROC(main_menu_delete_all_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_delete_all_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_delete_all_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_delete_all_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_delete_all_pwdlg_exit)
END_MSGPROC(main_menu_delete_all_pwdlg_proc, cont_class_proc)

