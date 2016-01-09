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

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define UI_SUBT_STR_LEN        30

#define UI_SUBT_LANG_LEN       3

#define UI_SUBT_LIST_ITEM_CNT  6
#define UI_SUBT_LIST_ITEM_H    36

#define UI_SUBT_TITLE_X 20
#define UI_SUBT_TITLE_Y 20
#define UI_SUBT_TITLE_W 294
#define UI_SUBT_TITLE_H 46

#define UI_SUBT_LIST_X UI_SUBT_TITLE_X
#define UI_SUBT_LIST_Y (UI_SUBT_TITLE_Y + UI_SUBT_TITLE_H + 10)
#define UI_SUBT_LIST_W (UI_SUBT_TITLE_W - 10)
#define UI_SUBT_LIST_H 248

#define UI_SUBT_LIST_MID_X 10
#define UI_SUBT_LIST_MID_Y 10
#define UI_SUBT_LIST_MID_W (UI_SUBT_LIST_W - 2 * UI_SUBT_LIST_MID_X)
#define UI_SUBT_LIST_MID_H (UI_SUBT_LIST_H - 2 * UI_SUBT_LIST_MID_Y)
#define UI_SUBT_LIST_MID_G 0

#define UI_SUBT_SBAR_X (UI_SUBT_LIST_X + UI_SUBT_LIST_W + 4)
#define UI_SUBT_SBAR_Y UI_SUBT_LIST_Y
#define UI_SUBT_SBAR_W 6
#define UI_SUBT_SBAR_H UI_SUBT_LIST_H

#define UI_SUBT_SBAR_MID_X 0
#define UI_SUBT_SBAR_MID_Y 4
#define UI_SUBT_SBAR_MID_W (UI_SUBT_SBAR_W - 2 * UI_SUBT_SBAR_MID_X)
#define UI_SUBT_SBAR_MID_H (UI_SUBT_SBAR_H - 2 * UI_SUBT_SBAR_MID_Y)
#define UI_SUBT_SBAR_MID_G 12

#define UI_SUBT_LIST_ITEM_1_W 30
#define UI_SUBT_LIST_ITEM_1_X 10
#define UI_SUBT_LIST_ITEM_2_W 48
#define UI_SUBT_LIST_ITEM_2_X 40
#define UI_SUBT_LIST_ITEM_3_W 160
#define UI_SUBT_LIST_ITEM_3_X 88

#define UI_SUBT_MAIN_X (SCREEN_WIDTH / 8)
#define UI_SUBT_MAIN_Y 100
#define UI_SUBT_MAIN_W (UI_SUBT_SBAR_X + UI_SUBT_SBAR_W + 20)
#define UI_SUBT_MAIN_H (UI_SUBT_LIST_H + UI_SUBT_LIST_Y + 20)

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
        UI_SUBT_LIST_ITEM_1_W, 
        UI_SUBT_LIST_ITEM_1_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_rstyle
    },
    { 
        LISTFIELD_TYPE_UNISTR, 
        UI_SUBT_LIST_ITEM_2_W, 
        UI_SUBT_LIST_ITEM_2_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_fstyle
    },
    { 
        LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER, 
        UI_SUBT_LIST_ITEM_3_W, 
        UI_SUBT_LIST_ITEM_3_X, 
        0, 
        &slist_field_rstyle, 
        &slist_field_fstyle
    }
};

static void *p_rsc_hdl = NULL;

/*!
 * Video view subt control id
 */ 
typedef enum
{
    IDC_VIDEO_SUBT_LIST = 1,
}ui_video_subt_id_t;

u16 _ui_video_subt_keymap(u16 key);
RET_CODE _ui_video_subt_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void _ui_video_subt_set_attr(BOOL insert, u16 *p_path);

static RET_CODE _ui_video_subt_list_update(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
    u16 i                          = 0;
    u16 cnt_i                      = 0;
    u16 cnt_o                      = 0;    
    u16 focus                      = 0;
    u8 asc_str[UI_SUBT_STR_LEN]    = {0};
    u16 subt_lang[UI_SUBT_STR_LEN+1] = {0};       

    focus = ui_video_m_get_subt_focus();

    cnt_i = ui_video_c_get_insert_sub_cnt();
    cnt_o = ui_video_m_get_subt_cnt();

    /*!
     * First add insert subtitle
     */
    for(i = 0; i < cnt_i; i++)
    { 
        sprintf((char *)asc_str, "[%02d] ", i + 1);
        list_set_field_content_by_ascstr(p_ctrl, i, 1, asc_str);

        ui_video_c_get_insert_sub_by_idx(i, subt_lang, UI_SUBT_STR_LEN+1);
        
        list_set_field_content_by_unistr(p_ctrl, i, 2, subt_lang);
        if(i == focus)
        {
           list_set_field_content_by_icon(p_ctrl, i, 0, IM_TV_SELECT);
        }
        else
        {
            list_set_field_content_by_icon(p_ctrl, i, 0, 0);
        }
    }  
    
    for(i = cnt_i; i < cnt_i + cnt_o; i++)
    { 
        sprintf((char *)asc_str, "[%02d] ", i + 1);
        list_set_field_content_by_ascstr(p_ctrl, i, 1, asc_str);

        ui_video_m_get_subt_by_idx(i - cnt_i, subt_lang, UI_SUBT_STR_LEN);
        
        list_set_field_content_by_unistr(p_ctrl, i, 2, subt_lang);
        if(i == focus)
        {
           list_set_field_content_by_icon(p_ctrl, i, 0, IM_TV_SELECT);
        }
        else
        {
            list_set_field_content_by_icon(p_ctrl, i, 0, 0);
        }
    }

    list_set_field_content_by_ascstr(p_ctrl, cnt_i + cnt_o, 1, (u8 *)"");
    list_set_field_content_by_unistr(p_ctrl, 
                                     cnt_i + cnt_o,
                                     2, 
                                     (u16 *)gui_get_string_addr(IDS_SUBTITLE_OFF));

    if((cnt_i + cnt_o) == focus)
    {
       list_set_field_content_by_icon(p_ctrl, focus, 0, IM_TV_SELECT);
    }
    else
    {
        list_set_field_content_by_icon(p_ctrl, (cnt_i + cnt_o), 0, 0);
    }                                     
    
    //ctrl_paint_ctrl(p_ctrl, TRUE);
    return SUCCESS;
}

static control_t* _ui_video_subt_create_main()
{
    control_t *p_cont = NULL;

    p_cont = fw_create_mainwin(ROOT_ID_FILEPLAY_SUBT,
                               UI_SUBT_MAIN_X, 
                               UI_SUBT_MAIN_Y,
                               UI_SUBT_MAIN_W, 
                               UI_SUBT_MAIN_H, 
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

static control_t* _ui_video_subt_create_scrollbar(control_t *p_cont)
{
    control_t *p_sbar = NULL;
    
    p_sbar = ctrl_create_ctrl(CTRL_SBAR, 
                              IDC_FLIST_SBAR,
                              UI_SUBT_SBAR_X, 
                              UI_SUBT_SBAR_Y,
                              UI_SUBT_SBAR_W, 
                              UI_SUBT_SBAR_H,
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
                   UI_SUBT_SBAR_MID_G, 
                   UI_SUBT_SBAR_MID_W, 
                   UI_SUBT_SBAR_MID_H - UI_SUBT_SBAR_MID_G);

    return p_sbar;                      
}

static void _ui_video_subt_create_list(control_t *p_cont, u16 cnt)
{
    u8         i      = 0;
    u16        focus  = 0;
    control_t *p_list = NULL;
    control_t *p_sbar = NULL;
    
    p_list = ctrl_create_ctrl(CTRL_LIST, 
                              IDC_VIDEO_SUBT_LIST, 
                              UI_SUBT_LIST_X,
                              UI_SUBT_LIST_Y, 
                              UI_SUBT_LIST_W, 
                              UI_SUBT_LIST_H, 
                              p_cont, 
                              0);

    if(NULL == p_list)
    {
        return;
    }
                              
    ctrl_set_rstyle(p_list, RSI_COMMON_RECT1, RSI_COMMON_RECT1, RSI_COMMON_RECT1);
    ctrl_set_keymap(p_list, _ui_video_subt_keymap);
    ctrl_set_proc(p_list, _ui_video_subt_proc);

    ctrl_set_mrect(p_list, 
                   UI_SUBT_LIST_MID_X, 
                   UI_SUBT_LIST_MID_Y,
                   UI_SUBT_LIST_MID_W, 
                   UI_SUBT_LIST_MID_H);
                      
    list_set_item_rstyle(p_list, &flist_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    /*!
     * Last one is "SUBTITLE OFF"
     */
    list_set_count(p_list, cnt + 1, UI_SUBT_LIST_ITEM_CNT);    
    
    list_set_field_count(p_list, ARRAY_SIZE(slist_attr), UI_SUBT_LIST_ITEM_CNT);

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

    focus = ui_video_m_get_subt_focus();
    list_set_focus_pos(p_list, focus);
    list_set_update(p_list, _ui_video_subt_list_update, 0);
    _ui_video_subt_list_update(p_list, list_get_valid_pos(p_list), UI_SUBT_LIST_ITEM_CNT, 0);

    p_sbar = _ui_video_subt_create_scrollbar(p_cont);
    if(NULL == p_sbar)
    {
        return;
    }
    
    list_set_scrollbar(p_list, p_sbar); 

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);    
}

static void _ui_video_subt_create_title(control_t *p_cont)
{
    control_t *p_ttl = NULL;
    
    p_ttl = ctrl_create_ctrl(CTRL_TEXT, 
                             10, 
                             UI_SUBT_TITLE_X, 
                             UI_SUBT_TITLE_Y, 
                             UI_SUBT_TITLE_W, 
                             UI_SUBT_TITLE_H, 
                             p_cont, 
                             0);

    if(NULL == p_ttl)
    {
        return;
    }
    
    ctrl_set_rstyle(p_ttl, RSI_FLIST_TITLE, RSI_FLIST_TITLE, RSI_FLIST_TITLE);
    text_set_font_style(p_ttl, FSI_FLIST_TEXT, FSI_FLIST_TEXT, FSI_FLIST_TEXT);
    text_set_content_type(p_ttl, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_ttl, IDS_SUBTITLE_LANGUAGE);
}
static u16 _ui_video_subt_get_osd_tm(void)
{
    osd_set_t osd_set = {0};

    sys_status_get_osd_set(&osd_set);

    return (u16)(osd_set.timeout * 1000);
}
RET_CODE ui_video_subt_open(u32 para1, u32 para2)
{
    u16        cnt_i  = 0;
    u16        cnt_o  = 0;
    control_t *p_cont = NULL;


    cnt_i = ui_video_c_get_insert_sub_cnt();
    cnt_o = ui_video_m_get_subt_cnt();
    
    p_cont = _ui_video_subt_create_main();
                               
    if (NULL == p_cont)
    {
        return ERR_FAILURE;
    }

    _ui_video_subt_create_title(p_cont);
    _ui_video_subt_create_list(p_cont, cnt_o + cnt_i);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

    fw_tmr_create(ROOT_ID_FILEPLAY_SUBT, MSG_EXIT, _ui_video_subt_get_osd_tm(), FALSE);
    return SUCCESS;
}
static void _ui_video_subt_close()
{
    fw_tmr_destroy(ROOT_ID_FILEPLAY_SUBT, MSG_EXIT);
    manage_close_menu(ROOT_ID_FILEPLAY_SUBT, 0, 0);
}

static void _ui_video_subt_off()
{
    ui_video_c_sub_off();
    _ui_video_subt_close();
}


RET_CODE _ui_video_get_glyph(glyph_input_t *p_input, glyph_output_t *p_output)
{
  rsc_char_info_t info = {0};
  rsc_fstyle_t fstyle = {0};
  BOOL ret_boo = FALSE;
  //gui_main_t *p_info = NULL;
  MT_ASSERT(p_rsc_hdl != 0);
  //p_info = (gui_main_t *)class_get_handle_by_id(PAINT_CLASS_ID);

  fstyle.font_id = FONT_ID_1;

  fstyle.color = 0xFFFFFFFF;

  fstyle.face_id = 0;
  fstyle.width = 24;
  fstyle.height = 24; 
  //fstyle.attr |= VFONT_STROK;

  ret_boo = rsc_get_char(p_rsc_hdl, &fstyle, p_input->char_code, &info);
  if(ret_boo != TRUE)
  {
    return ERR_FAILURE;
  }   

  p_output->width = info.width;
  p_output->height = info.height;
  p_output->alpha_pitch = info.alpha_pitch;
  p_output->ckey = info.ckey;
  p_output->enable_ckey = TRUE;

  p_output->p_char = info.p_char;
  p_output->p_alpha = info.p_alpha;
  p_output->p_strok_char = info.p_strok_char;
  p_output->p_strok_alpha = info.p_strok_alpha;

  p_output->step_height = info.step_height;
  p_output->step_width = info.step_width;
  p_output->x_step = info.x_step;

  p_output->xoffset = info.xoffset;
  p_output->yoffset = info.yoffset;
  p_output->pitch = info.pitch;

  return SUCCESS;
}



static void _ui_video_subt_set_attr(BOOL insert, u16 *p_path)
{
    mul_fp_subt_attr_t subt_attr  = {0};
    if((FALSE == insert) && (NULL == p_path))
    {   
        return;
    }

    //close the old first
    ui_video_c_sub_off();

    subt_attr.char_bpp = 32;
    subt_attr.char_fmt = PIX_FMT_ARGB8888;
    subt_attr.font_color = C_WHITE;
    subt_attr.bg_color = C_BLUE;
    subt_attr.window_color = C_TRANS;
    subt_attr.char_rect.left= 0;
    subt_attr.char_rect.top= 0;
    subt_attr.char_rect.right= 700;
    subt_attr.char_rect.bottom = 150;
    subt_attr.insert_subt = insert;
    //subt_attr.p_rsc_cfg = &g_rsc_config;
    subt_attr.rsc_get_glyph = _ui_video_get_glyph;
    subt_attr.p_subt_path = p_path;
    subt_attr.show_rect.left = 0;
    subt_attr.show_rect.top = 0;
    subt_attr.show_rect.right = 700;
    subt_attr.show_rect.bottom = 150;
    subt_attr.sp_buf_cnts = 2;
    subt_attr.sp_buf_size = DEFAULT_STKSIZE;
    subt_attr.stk_size = DEFAULT_STKSIZE;
    //subt_attr.task_prio = MDL_VIDEO_SUBTITLE_PRIORITY;

    (void)ui_video_c_sub_on((void *)&subt_attr);
}
static RET_CODE _ui_video_subt_focus_change(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
      fw_tmr_reset(ROOT_ID_FILEPLAY_SUBT, MSG_EXIT, _ui_video_subt_get_osd_tm());
      return ERR_NOFEATURE;
}
static void _ui_video_subt_set_text_encode(void)
{
    language_set_t lang_set = {0};

    sys_status_get_lang_set(&lang_set);

    OS_PRINTF("ui_video_sub: text encode:%d\n", lang_set.text_encode_video);
    ui_video_c_set_charset((str_fmt_t)lang_set.text_encode_video);
}
static void _ui_video_subt_show_by_focus(u16 focus)
{
    u16 cnt_i = 0;
    u16 cnt_o = 0;

    cnt_i = ui_video_c_get_insert_sub_cnt();
    cnt_o = ui_video_m_get_subt_cnt();
        
    if(focus >= (cnt_i + cnt_o))
    {
        _ui_video_subt_off();
        ui_video_m_set_subt_focus(cnt_i + cnt_o);
        return;
    }
      
    /*!
     * At the begain of list is plug_in subtitle and at the last of list is insert subtitle
     */
    if(focus < cnt_i)
    {
        ui_video_c_set_insert_sub_id_by_idx(focus);
        _ui_video_subt_set_attr(TRUE, NULL);
    }
    else
    {
        _ui_video_subt_set_attr(FALSE, ui_video_m_get_cur_subt_path(focus - cnt_i));
    }
    
    ui_video_m_set_subt_focus(focus);

    _ui_video_subt_set_text_encode();
}
/*!
 * Get sub language focus by globle sub language index
 */
void ui_video_subt_bg_init(void)
{
  u8 index                       = 0;
  u16 i                          = 0;
  u16 cnt_i                      = 0;
  u16 cnt_o                      = 0;
  u8 asc_str[UI_SUBT_STR_LEN]    = {0};
  u16 subt_lang[UI_SUBT_STR_LEN+1] = {0}; 
  language_set_t lang_set        = {0};
  char **pp_lang_b               = NULL;
  char **pp_lang_t               = NULL;
  
  sys_status_get_lang_set(&lang_set);
  index = lang_set.sub_title;
  
  cnt_i = ui_video_c_get_insert_sub_cnt();
  cnt_o = ui_video_m_get_subt_cnt();

  pp_lang_b = sys_status_get_lang_code(TRUE);
  pp_lang_t = sys_status_get_lang_code(FALSE);
   
  for(i = 0; i < cnt_i; i++)
  {
    ui_video_c_get_insert_sub_by_idx(i, subt_lang, UI_SUBT_STR_LEN+1);
    str_uni2asc(asc_str, subt_lang);

    if((0 == STRCMPI(pp_lang_b[index], (char *)asc_str) )
      || (0 == STRCMPI(pp_lang_t[index], (char *)asc_str)))
    {
      _ui_video_subt_show_by_focus(i);
      return;
    }
  }
  
  for(i = 0; i < cnt_o; i++)
  { 
    ui_video_m_get_subt_by_idx(i, subt_lang, UI_SUBT_STR_LEN);
    str_uni2asc(asc_str, subt_lang);
    
    if((0 == STRCMPI(pp_lang_b[index], (char *)asc_str) )
      || (0 == STRCMPI(pp_lang_t[index], (char *)asc_str)))
    {
      _ui_video_subt_show_by_focus(i + cnt_i);
      return ;
    }
  }  
  
  _ui_video_subt_show_by_focus(0);
}

static RET_CODE _ui_video_subt_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;
    
    focus = list_get_focus_pos(p_ctrl);
    
    _ui_video_subt_show_by_focus(focus);

    ui_video_c_sub_show();
    
    _ui_video_subt_close();
    
    return  SUCCESS;
}


static RET_CODE _ui_video_subt_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    _ui_video_subt_close();
    return  SUCCESS;
}

/*!
 * Init sub language rsc hander.
 */
void subt_rsc_hdl_deinit()
{
  rsc_config_t       rsc_config = {0};
  vf_ft_cfg_t     vfont_cfg = {0};

  if(p_rsc_hdl != NULL)
  {
    vf_ft_detach(p_rsc_hdl);	
    rsc_release(p_rsc_hdl);
    p_rsc_hdl = NULL;
  }

  memcpy(&rsc_config, &g_rsc_config, sizeof(rsc_config));
  rsc_config.bmp_buf_size = 0;
  
  p_rsc_hdl = rsc_init(&rsc_config);

  #ifdef ENABLE_NETWORK  
  vfont_cfg.max_height = 72;
  vfont_cfg.max_width = 72;
#else  
  vfont_cfg.max_height = 32;
  vfont_cfg.max_width = 32;
  
#endif
  vfont_cfg.is_alpha_spt = TRUE;
  vfont_cfg.bpp = 32;
  vfont_cfg.max_cnt = 1;

  vf_ft_attach(p_rsc_hdl, &vfont_cfg);
}
/*!
 * release sub language rsc hander.
 */
void subt_rsc_hdl_destroy()
{
  if(p_rsc_hdl != NULL)
  {
    vf_ft_detach(p_rsc_hdl);	
    rsc_release(p_rsc_hdl);
    p_rsc_hdl = NULL;
  }
}


BEGIN_KEYMAP(_ui_video_subt_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_SUBT, MSG_EXIT)  
  ON_EVENT(V_KEY_MENU, MSG_EXIT)  
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_EXIT, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(_ui_video_subt_keymap, NULL)

BEGIN_MSGPROC(_ui_video_subt_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, _ui_video_subt_focus_change)  
  ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_subt_focus_change)  
  ON_COMMAND(MSG_PAGE_UP, _ui_video_subt_focus_change)  
  ON_COMMAND(MSG_PAGE_DOWN, _ui_video_subt_focus_change)  
  ON_COMMAND(MSG_SELECT, _ui_video_subt_select)
  ON_COMMAND(MSG_EXIT, _ui_video_subt_exit)
  ON_COMMAND(MSG_PLUG_OUT, _ui_video_subt_exit)
END_MSGPROC(_ui_video_subt_proc, list_class_proc)
