/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_network_music_search.h"
#include "ui_keyboard_v2.h"

typedef enum
{
    IDC_NM_SEARCH_TRACK = 1,
    IDC_NM_SEARCH_ALBUM,
    IDC_NM_SEARCH_ARTIST,
    IDC_NM_SEARCH_PROMPT,
}nm_search_ctrl_id;

#define NM_MAX_SEARCH_NAME_LEN 16

//search
u16 *p_search_string;
edit_save_cb search_cb;
s16 focus = 0;

u16 network_music_search_dlg_keymap(u16 key);

RET_CODE network_music_search_dlg_proc(control_t *p_cont, u16 msg,
                                 u32 para1, u32 para2);

void ui_search_dlg_close(void)
{
     manage_close_menu(ROOT_ID_NETWORK_MUSIC_SEARCH, 0, 0);
}


s16 ui_network_music_get_focus_btn_index(void)
{
    return focus;
}

RET_CODE on_network_music_search_change_focus(control_t *p_cont, u16 msg,
                                              u32 para1, u32 para2)
{
    switch (msg)
    {
    case MSG_FOCUS_LEFT:
        {
            focus = (focus - 1 < 0)?2:focus - 1;
        }
        break;
    case MSG_FOCUS_RIGHT:
        {
            focus = (focus + 1>3)?0:focus + 1;
        }
        break;
    default:
        break;
    }
    return ERR_NOFEATURE;
}


RET_CODE on_network_music_search_selected(control_t *p_cont, u16 msg,
                                 u32 para1, u32 para2)
{
    kb_param_t param;
    param.uni_str = p_search_string;
    param.type = KB_INPUT_TYPE_SENTENCE;
    param.max_len = NM_MAX_SEARCH_NAME_LEN;
    param.cb = search_cb;
    
    ui_search_dlg_close();
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    return SUCCESS;
}



RET_CODE ui_netmusic_search_close()
{
    ui_search_dlg_close();
    return SUCCESS;
}

RET_CODE on_network_music_search_cancel(control_t *p_cont, u16 msg,
                               u32 para1, u32 para2)
{
    ui_netmusic_search_close();
    return SUCCESS;

}

RET_CODE ui_net_music_search_open(u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_txt, *p_btn[3] ={NULL};
    u16 i, cnt, x, y;    u16 btn[3] = { IDS_MODE_TRACK, IDS_MODE_ALBUM,IDS_MODE_ARTIST };

    search_cb = (void*)para1;
    p_search_string = (u16*)para2;
    

    if(fw_find_root_by_id(ROOT_ID_NETWORK_MUSIC_SEARCH) != NULL) // already opened
    {
        UI_PRINTF("UI: already open a dialog, force close it! \n");
        ui_netmusic_search_close();
        return SUCCESS;
    }

    p_cont = p_txt = NULL;
    x = y = cnt = 0;

    // create root at first
    p_cont = fw_create_mainwin( (u8)ROOT_ID_NETWORK_MUSIC_SEARCH,
        NM_SEARCH_CONT_X, NM_SEARCH_CONT_Y, NM_SEARCH_CONT_W, NM_SEARCH_CONT_H, 0, 0,OBJ_ATTR_ACTIVE,0);
    ctrl_set_rstyle(p_cont,RSI_POPUP_BG,RSI_POPUP_BG,RSI_POPUP_BG);

    ctrl_set_keymap(p_cont, network_music_search_dlg_keymap);
    ctrl_set_proc(p_cont, network_music_search_dlg_proc);

    p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_SEARCH_PROMPT,
        50, 50, 400, 30, p_cont, 0);
    ctrl_set_rstyle(p_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //text_set_font_style(p_txt, FSI_BLACK_20, FSI_BLACK_20, FSI_BLACK_20);
    text_set_font_style(p_txt, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_txt, (u8*)"Please choose search mode!");

 
    x = (NM_SEARCH_CONT_W /3 - NM_SEARCH_DLG_BTN_W) /2;
    y = (NM_SEARCH_CONT_H - NM_SEARCH_DLG_BTN_H ) / 2 + 50;
   
    cnt = 3; //button num 
    for (i = 0; i < 3; i++)
    {
        p_btn[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_NM_SEARCH_TRACK + i),
            x, y, NM_SEARCH_DLG_BTN_W, NM_SEARCH_DLG_BTN_H, p_cont, 0);
        ctrl_set_rstyle(p_btn[i],
            RSI_ITEM_4_SH, RSI_ITEM_4_HL, RSI_ITEM_4_SH);
        text_set_font_style(p_btn[i],
            FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
        text_set_content_type(p_btn[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_btn[i],btn[i]);

        ctrl_set_related_id(p_btn[i],
            (u8)((i - 1 + cnt) % cnt + 1), /* left */
            0,                             /* up */
            (u8)((i + 1) % cnt + 1),       /* right */
            0);                            /* down */

        x += NM_SEARCH_CONT_W / 3;
    }
    focus = 0;
    ctrl_default_proc(p_btn[0], MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_cont, FALSE);
    return SUCCESS;
}




BEGIN_KEYMAP(network_music_search_dlg_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_LEFT,MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT,MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_CANCEL,MSG_CANCEL)
ON_EVENT(V_KEY_MENU,MSG_CANCEL)
END_KEYMAP(network_music_search_dlg_keymap, NULL)


BEGIN_MSGPROC(network_music_search_dlg_proc, cont_class_proc)
ON_COMMAND(MSG_FOCUS_LEFT,on_network_music_search_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT,on_network_music_search_change_focus)
ON_COMMAND(MSG_SELECT, on_network_music_search_selected)
ON_COMMAND(MSG_CANCEL,on_network_music_search_cancel)
END_MSGPROC(network_music_search_dlg_proc, cont_class_proc)
