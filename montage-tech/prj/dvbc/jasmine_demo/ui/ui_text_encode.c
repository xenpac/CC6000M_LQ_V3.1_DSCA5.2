/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_language.h"
#include "ui_fav_list.h"
#include "sys_status.h"
#include "iso_639_2.h"

#include "ui_video.h"
#include "file_play_api.h"
#include "common_filter.h"
#include "ui_music_api.h"
#include "ui_text_encode.h"


static list_xstyle_t flist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
};


static list_xstyle_t slist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t slist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_field_attr_t slist_attr[] =
{
    { 
        LISTFIELD_TYPE_ICON | STL_LEFT | STL_VCENTER, 
        UI_TEXT_ENCODE_LIST_ITEM_1_W, 
        UI_TEXT_ENCODE_LIST_ITEM_1_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_rstyle
    },
    { 
        LISTFIELD_TYPE_UNISTR, 
        UI_TEXT_ENCODE_LIST_ITEM_2_W, 
        UI_TEXT_ENCODE_LIST_ITEM_2_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_fstyle
    },
    { 
        LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER, 
        UI_TEXT_ENCODE_LIST_ITEM_3_W, 
        UI_TEXT_ENCODE_LIST_ITEM_3_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_fstyle
    }
};


/*!
 * Video view subt control id
 */ 
typedef enum
{
    IDC_TEXT_ENCODE_SCROLLBAR = 1,
    IDC_TEXT_ENCODE_LIST,
    IDC_TEXT_ENCODE_TITLE,  
}ui_text_encode_id_t;

u16 _ui_text_encode_keymap(u16 key);
RET_CODE _ui_text_encode_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u16 _ui_text_encode_get_focus(void)
{
    u16 focus                      = 0;
    language_set_t lang_set        = {0};
  
    sys_status_get_lang_set(&lang_set);
    if(fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR))
    {
      focus = (u16)lang_set.text_encode_video;
    }
    else if(fw_find_root_by_id(ROOT_ID_USB_MUSIC_FULLSCREEN))
    {
      focus = (u16)lang_set.text_encode_music;
    }
    return focus;
}

static RET_CODE _ui_text_encode_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
    u16 i                                 = 0;  
    u16 focus                             = 0;
    u8 asc_str[UI_TEXT_ENCODE_STR_LEN]    = {0};
    char **pp_text_encode                 = NULL;
    
    focus = _ui_text_encode_get_focus();
    
    pp_text_encode = sys_status_get_text_encode();

    for(i = 0; i < UI_TEXT_ENCODE_LIST_ITEM_CNT; i++)
    { 
        sprintf((char *)asc_str, "[%02d] ", i + 1);
        list_set_field_content_by_ascstr(p_ctrl, i, 1, asc_str);
        
        list_set_field_content_by_ascstr(p_ctrl, i, 2, (u8 *)pp_text_encode[i]);
        
        if(i == focus)
        {
           list_set_field_content_by_icon(p_ctrl, i, 0, IM_TV_SELECT);
        }
        else
        {
            list_set_field_content_by_icon(p_ctrl, i, 0, 0);
        }
    }  
    
    //ctrl_paint_ctrl(p_ctrl, TRUE);
    return SUCCESS;
}

static control_t* _ui_text_encode_create_main()
{
    control_t *p_cont = NULL;

    p_cont = fw_create_mainwin(ROOT_ID_TEXT_ENCODE,
                               UI_TEXT_ENCODE_MAIN_X, 
                               UI_TEXT_ENCODE_MAIN_Y,
                               UI_TEXT_ENCODE_MAIN_W, 
                               UI_TEXT_ENCODE_MAIN_H, 
                               0, 
                               0, 
                               OBJ_ATTR_ACTIVE, 
                               0);
                               
    if (NULL == p_cont)
    {
        return NULL;
    }
    
    ctrl_set_rstyle(p_cont, RSI_COMMAN_BG, RSI_COMMAN_BG, RSI_COMMAN_BG); 
    
    ctrl_set_keymap(p_cont, ui_comm_root_keymap);
    ctrl_set_proc(p_cont, ui_comm_root_proc);
    
    return p_cont;
}

static control_t* _ui_text_encode_create_scrollbar(control_t *p_cont)
{
    control_t *p_sbar = NULL;
    
    p_sbar = ctrl_create_ctrl(CTRL_SBAR, 
                              IDC_TEXT_ENCODE_SCROLLBAR,
                              UI_TEXT_ENCODE_SBAR_X, 
                              UI_TEXT_ENCODE_SBAR_Y,
                              UI_TEXT_ENCODE_SBAR_W, 
                              UI_TEXT_ENCODE_SBAR_H,
                              p_cont, 
                              0);

    if(NULL == p_sbar)
    {
        return NULL;
    }
    
    ctrl_set_rstyle(p_sbar, RSI_FLIST_SBAR, RSI_FLIST_SBAR, RSI_FLIST_SBAR);
    sbar_set_autosize_mode(p_sbar, TRUE);
    sbar_set_direction(p_sbar, 0);
    
    sbar_set_mid_rstyle(p_sbar, 
                        RSI_FLIST_SBAR_MID, 
                        RSI_FLIST_SBAR_MID,
                        RSI_FLIST_SBAR_MID);
                        
    ctrl_set_mrect(p_sbar, 
                   0, 
                   UI_TEXT_ENCODE_SBAR_MID_G, 
                   UI_TEXT_ENCODE_SBAR_MID_W, 
                   UI_TEXT_ENCODE_SBAR_MID_H - UI_TEXT_ENCODE_SBAR_MID_G);

    return p_sbar;                      
}

static void _ui_text_encode_create_list(control_t *p_cont)
{
    u8         i      = 0;
    u16        focus  = 0;
    control_t *p_list = NULL;
    control_t *p_sbar = NULL;
    
    p_list = ctrl_create_ctrl(CTRL_LIST, 
                              IDC_TEXT_ENCODE_LIST, 
                              UI_TEXT_ENCODE_LIST_X,
                              UI_TEXT_ENCODE_LIST_Y, 
                              UI_TEXT_ENCODE_LIST_W, 
                              UI_TEXT_ENCODE_LIST_H, 
                              p_cont, 
                              0);

    if(NULL == p_list)
    {
        return;
    }
                              
    ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
    ctrl_set_keymap(p_list, _ui_text_encode_keymap);
    ctrl_set_proc(p_list, _ui_text_encode_proc);

    ctrl_set_mrect(p_list, 
                   UI_TEXT_ENCODE_LIST_MID_X, 
                   UI_TEXT_ENCODE_LIST_MID_Y,
                   UI_TEXT_ENCODE_LIST_MID_W, 
                   UI_TEXT_ENCODE_LIST_MID_H);
                      
    list_set_item_rstyle(p_list, &flist_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    list_set_count(p_list, UI_TEXT_ENCODE_LIST_ITEM_CNT, UI_TEXT_ENCODE_LIST_ITEM_CNT);    
    
    list_set_field_count(p_list, ARRAY_SIZE(slist_attr), UI_TEXT_ENCODE_LIST_ITEM_CNT);

    for (i = 0; i < ARRAY_SIZE(slist_attr); i++)
    {
        list_set_field_attr(p_list, 
                            i, 
                            slist_attr[i].attr, 
                            slist_attr[i].width,
                            slist_attr[i].left, 
                            slist_attr[i].top);
                            
        list_set_field_rect_style(p_list, i, slist_attr[i].rstyle);
        list_set_field_font_style(p_list, i, slist_attr[i].fstyle);
    }

    focus = _ui_text_encode_get_focus();
    list_set_focus_pos(p_list, focus);
    list_set_update(p_list, _ui_text_encode_list_update, 0);
    _ui_text_encode_list_update(p_list, list_get_valid_pos(p_list), UI_TEXT_ENCODE_LIST_ITEM_CNT, 0);

    p_sbar = _ui_text_encode_create_scrollbar(p_cont);
    if(NULL == p_sbar)
    {
        return;
    }
    
    list_set_scrollbar(p_list, p_sbar); 

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);    
}

static void _ui_text_encode_create_title(control_t *p_cont)
{
    control_t *p_ttl = NULL;
    
    p_ttl = ctrl_create_ctrl(CTRL_TEXT, 
                             IDC_TEXT_ENCODE_TITLE, 
                             UI_TEXT_ENCODE_TITLE_X, 
                             UI_TEXT_ENCODE_TITLE_Y, 
                             UI_TEXT_ENCODE_TITLE_W, 
                             UI_TEXT_ENCODE_TITLE_H, 
                             p_cont, 
                             0);

    if(NULL == p_ttl)
    {
        return;
    }
    
    ctrl_set_rstyle(p_ttl, RSI_FLIST_TITLE, RSI_FLIST_TITLE, RSI_FLIST_TITLE);
    text_set_font_style(p_ttl, FSI_FLIST_TEXT, FSI_FLIST_TEXT, FSI_FLIST_TEXT);
    text_set_content_type(p_ttl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ttl, IDS_TEXT_ENCODE);
}

static u16 _ui_text_encode_get_osd_tm(void)
{
    osd_set_t osd_set = {0};

    sys_status_get_osd_set(&osd_set);

    return (u16)(osd_set.timeout * 1000);
}

RET_CODE ui_text_encode_open(u32 para1, u32 para2)
{
    control_t *p_cont = NULL;

    p_cont = _ui_text_encode_create_main();
                               
    if (NULL == p_cont)
    {
        return ERR_FAILURE;
    }

    _ui_text_encode_create_title(p_cont);
    _ui_text_encode_create_list(p_cont);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

    fw_tmr_create(ROOT_ID_TEXT_ENCODE, MSG_EXIT, _ui_text_encode_get_osd_tm(), FALSE);
    return SUCCESS;
}

static void _ui_text_encode_close()
{
    fw_tmr_destroy(ROOT_ID_TEXT_ENCODE, MSG_EXIT);
    manage_close_menu(ROOT_ID_TEXT_ENCODE, 0, 0);
}

static RET_CODE _ui_text_encode_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    fw_tmr_reset(ROOT_ID_TEXT_ENCODE, MSG_EXIT, _ui_text_encode_get_osd_tm());
    return ERR_NOFEATURE;
}

static RET_CODE _ui_text_encode_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus                      = 0;
    language_set_t lang_set        = {0};

    focus = list_get_focus_pos(p_ctrl);
    
    sys_status_get_lang_set(&lang_set);

    #ifdef ENABLE_FILE_PLAY
    if(fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR))
    {
      lang_set.text_encode_video = (u8)focus;
      ui_video_c_set_charset((str_fmt_t)focus);
    }
    else 
    #endif
    if(fw_find_root_by_id(ROOT_ID_USB_MUSIC_FULLSCREEN))
    {
      lang_set.text_encode_music = (u8)focus;
      ui_music_set_charset((str_fmt_t)focus);
    }

    sys_status_set_lang_set(&lang_set);

    sys_status_save();
     
    _ui_text_encode_close();
    
    return  SUCCESS;
}


static RET_CODE _ui_text_encode_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    _ui_text_encode_close();
    return  SUCCESS;
}

BEGIN_KEYMAP(_ui_text_encode_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_RED, MSG_EXIT)  
  ON_EVENT(V_KEY_MENU, MSG_EXIT)  
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(_ui_text_encode_keymap, NULL)

BEGIN_MSGPROC(_ui_text_encode_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, _ui_text_encode_focus_change)  
  ON_COMMAND(MSG_FOCUS_DOWN, _ui_text_encode_focus_change)  
  ON_COMMAND(MSG_PAGE_UP, _ui_text_encode_focus_change)  
  ON_COMMAND(MSG_PAGE_DOWN, _ui_text_encode_focus_change)  
  ON_COMMAND(MSG_SELECT, _ui_text_encode_select)
  ON_COMMAND(MSG_EXIT, _ui_text_encode_exit)
  ON_COMMAND(MSG_PLUG_OUT, _ui_text_encode_exit)
END_MSGPROC(_ui_text_encode_proc, list_class_proc)
