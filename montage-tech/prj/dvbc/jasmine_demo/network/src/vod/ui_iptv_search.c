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
#if ENABLE_NETWORK
#include "commonData.h"
#include "IPTVDataProvider.h"
#include "ui_iptv_api.h"

#include "ui_iptv_prot.h"

#include "SM_StateMachine.h"
#include "States_IptvSearch.h"

/*!
 * Const value
 */
#define IPTV_KBD_CONTX           (90)//(60)
#define IPTV_KBD_CONTY           (90)//(60)
#define IPTV_KBD_CONTW           (430)//(460)
#define IPTV_KBD_CONTH           (600)

#define IPTV_KBD_ITEM_H_GAP (27)//(21)

#define IPTV_EDITOR_NAME_X ((IPTV_KBD_CONTW - IPTV_EDITOR_NAME_W) >> 1)
#define IPTV_EDITOR_NAME_Y (3)
#define IPTV_EDITOR_NAME_W (100)
#define IPTV_EDITOR_NAME_H (IPTV_EDITOR_Y - IPTV_EDITOR_NAME_Y)

/*the coordinate for the search edit box*/
#define IPTV_EDITOR_X ((IPTV_KBD_CONTW - IPTV_EDITOR_W) >> 1)
#define IPTV_EDITOR_Y (65)
#define IPTV_EDITOR_W (330)//(370)//(IPTV_KBD_CONTW - IPTV_EDITOR_X - IPTV_EDITOR_NAME_X)
#define IPTV_EDITOR_H (35)//(IPTV_EDITOR_NAME_H)

/*the coordinate for the mbox */
#define IPTV_MBOX_X (IPTV_EDITOR_X + ((IPTV_EDITOR_W - IPTV_MBOX_W) >> 1))
#define IPTV_MBOX_Y (IPTV_EDITOR_Y + IPTV_EDITOR_H + IPTV_KBD_ITEM_H_GAP)
#define IPTV_MBOX_W (270)//(300)
#define IPTV_MBOX_H (300)

/*the attributes for the mbox */
#define IPTV_MBOX_ROW  (6)
#define IPTV_MBOX_COL  (6)
#define IPTV_MBOX_H_GAP (6)
#define IPTV_MBOX_V_GAP (6)

/*the coordinate for the delete button*/
#define IPTV_DEL_BTN_X (IPTV_MBOX_X)
#define IPTV_DEL_BTN_Y (IPTV_MBOX_Y + IPTV_MBOX_H + IPTV_KBD_ITEM_H_GAP)
#define IPTV_DEL_BTN_W (80)//(90)
#define IPTV_DEL_BTN_H (41)//(30)

/*the coordinate for the space button*/
#define IPTV_SPACE_BTN_X (IPTV_DEL_BTN_X + IPTV_DEL_BTN_W + 15)
#define IPTV_SPACE_BTN_Y (IPTV_DEL_BTN_Y)
#define IPTV_SPACE_BTN_W (IPTV_DEL_BTN_W)
#define IPTV_SPACE_BTN_H (IPTV_DEL_BTN_H)

/*the coordinate for the clear button*/
#define IPTV_CLR_BTN_X (IPTV_SPACE_BTN_X + IPTV_SPACE_BTN_W + 15)
#define IPTV_CLR_BTN_Y (IPTV_DEL_BTN_Y)
#define IPTV_CLR_BTN_W (IPTV_DEL_BTN_W)
#define IPTV_CLR_BTN_H (IPTV_DEL_BTN_H)

/*the coordinate for the search button*/
#define IPTV_SEARCH_BTN_X (IPTV_MBOX_X + IPTV_MBOX_W - IPTV_SEARCH_BTN_W)
#define IPTV_SEARCH_BTN_Y (IPTV_DEL_BTN_Y + IPTV_DEL_BTN_H + IPTV_KBD_ITEM_H_GAP)
#define IPTV_SEARCH_BTN_W (120)
#define IPTV_SEARCH_BTN_H (IPTV_DEL_BTN_H)

// client container
#define IPTV_CLIENT_CONTX           (IPTV_KBD_CONTX + IPTV_KBD_CONTW + 10)
#define IPTV_CLIENT_CONTY           (IPTV_KBD_CONTY)
#define IPTV_CLIENT_CONTW           (660)//(690)
#define IPTV_CLIENT_CONTH           (IPTV_KBD_CONTH)

#define IPTV_TITLE_X           (20)
#define IPTV_TITLE_Y           (3)
#define IPTV_TITLE_W           (IPTV_CLIENT_CONTW - IPTV_TITLE_X - IPTV_TITLE_X)
#define IPTV_TITLE_H           (IPTV_VDOLIST_Y - IPTV_TITLE_Y)//(57)

#define IPTV_VDOLIST_X           (20)
#define IPTV_VDOLIST_Y           (IPTV_EDITOR_Y)//(IPTV_TITLE_Y + IPTV_TITLE_H + 20)
#define IPTV_VDOLIST_W           (IPTV_CLIENT_CONTW - IPTV_VDOLIST_X - IPTV_VDOLIST_X)
#define IPTV_VDOLIST_H           (IPTV_CLIENT_CONTH - IPTV_VDOLIST_Y - 20)

#define IPTV_VDOLIST_ITEM_V_GAP (2)//(4)

#define IPTVSEARCH_VDOLIST_PAGE_SIZE    (12)//(13)

#define IPTV_VDOLIST_NUM_X                (15)//(10)
#define IPTV_VDOLIST_NUM_Y                (0)
#define IPTV_VDOLIST_NUM_W                (70)

#define IPTV_VDOLIST_NAME_X                (IPTV_VDOLIST_NUM_X + IPTV_VDOLIST_NUM_W + 10)
#define IPTV_VDOLIST_NAME_Y                (0)
#define IPTV_VDOLIST_NAME_W                (IPTV_VDOLIST_W - IPTV_VDOLIST_NAME_X - IPTV_VDOLIST_NUM_X)

/*matrix key count*/
#define KB_IPTVSEARCH_MATRIX_KEYS_COUNT                    (36)
/*button count*/
#define KB_CUSTOM_KEYS_MAXRIX_COUNT                    (4)
/*the max length for the edit box*/

/*!
 * Macro
 */

/*!
 * Type define
 */
typedef enum
{
    IDC_IPTV_INVALID = 0,
    IDC_IPTV_KBD_CONT,
    IDC_IPTV_EDITOR,
    IDC_IPTV_MBOX,

    IDC_IPTV_BTN_DEL,
    IDC_IPTV_BTN_SPACE,
    IDC_IPTV_BTN_CLR,
    IDC_IPTV_BTN_SEARCH,

    IDC_IPTV_CLIENT_CONT,
    IDC_IPTV_TITLE,
    IDC_IPTV_VDOLIST,
} ui_iptvsearch_ctrl_id_t;

typedef struct
{
    control_t *hEditor;
    control_t *hMbox;
    control_t *hBtnSearch;
    control_t *hBtnDel;
    control_t *hBtnSpace;
    control_t *hBtnClr;
    control_t *hTitle;
    control_t *hVdolist;

    control_t *hKbdCont;
    control_t *hClientCont;

    al_iptv_vdo_item_t *vdoList;
    u32 total_vdo;//
    u16 vdo_cnt;
    u16 curVdoListIdx;

    u16 vdo_page_size;
    
    u32 total_page;     // total video page
    u32 curPageNum;   // video page num
    u32 savPageNum;   // video page num

    u16 cur_pos; // the current postion focus for the matrix
} ui_iptv_search_app_t;

/*!
 * Function define
 */
static u16 iptv_search_cont_keymap(u16 key);
static RET_CODE iptv_search_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static u16 keyboard_input_method_keymap(u16 key);
static u16 iptv_search_method_keymap(u16 key);
static u16 iptv_delete_method_keymap(u16 key);
static u16 iptv_space_method_keymap(u16 key);
static u16 iptv_clear_method_keymap(u16 key);
u16 iptvsearch_vdolist_keymap(u16 key);
RET_CODE iptvsearch_vdolist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static BOOL ui_iptvsearch_init_app_data(void);
static BOOL ui_iptvsearch_release_app_data(void);

static RET_CODE keyboard_input_method_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE iptv_search_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE iptv_delete_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE iptv_space_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static RET_CODE iptv_clear_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

/*!
 * Priv data
 */
// Video list style
static list_xstyle_t vdolist_item_rstyle =
{
    RSI_PBACK,
    RSI_PBACK,
    RSI_OTT_BUTTON_HL,
    RSI_OTT_BUTTON_HL,
    RSI_OTT_BUTTON_HL,
};

static list_xstyle_t vdolist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t vdolist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
};
static const list_field_attr_t vdolist_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_VCENTER,
        IPTV_VDOLIST_NUM_W, 
        IPTV_VDOLIST_NUM_X, 
        IPTV_VDOLIST_NUM_Y,
        &vdolist_field_rstyle,
        &vdolist_field_fstyle
    },
    {
        LISTFIELD_TYPE_EXTSTR | STL_LEFT | STL_VCENTER,
        IPTV_VDOLIST_NAME_W,
        IPTV_VDOLIST_NAME_X,
        IPTV_VDOLIST_NAME_Y,
        &vdolist_field_rstyle,
        &vdolist_field_fstyle
    }
};

u16 kb_iptv_custom_string[3] = {'\0', '\0', ' '};

// App data
static ui_iptv_search_app_t *this = NULL;


/*================================================================================================
                           iptv search internel function
 ================================================================================================*/

static BOOL ui_iptvsearch_init_app_data(void)
{
    if (this)
    {
        ui_iptvsearch_release_app_data();
    }

    this = (ui_iptv_search_app_t *)mtos_malloc(sizeof(ui_iptv_search_app_t));
    MT_ASSERT(this != NULL);
    memset((void *)this, 0, sizeof(ui_iptv_search_app_t));  

    return TRUE;
}

static void ui_iptvsearch_release_vdolist(void)
{
    u16 i;

    if (this->vdoList)
    {
        for (i = 0; i < this->vdo_cnt; i++)
        {
            if (this->vdoList[i].name)
            {
                mtos_free(this->vdoList[i].name);
            }
            if (this->vdoList[i].img_url)
            {
                mtos_free(this->vdoList[i].img_url);
            }
        }
        mtos_free(this->vdoList);
        this->vdoList = NULL;
    }
}

static BOOL ui_iptvsearch_release_app_data(void)
{
    if (this)
    {
        ui_iptvsearch_release_vdolist();

        mtos_free(this);
        this = NULL;
    }
    
    return TRUE;
}

static void ui_iptvsearch_update_title(control_t* p_ctrl, u32 context)
{
    u16 *p_unistr = NULL;
    u16 uni_str1[50+1];
    u16 uni_str2[20+1];
    u8  asc_str[20+1];

    OS_PRINTF("@@@ui_iptvsearch_update_title total_vdo=%d\n", this->total_vdo);
    if (this->total_vdo > 0)
    {
        memset(uni_str1, 0, sizeof(uni_str1));
        memset(uni_str2, 0, sizeof(uni_str2));
        memset(asc_str, 0, sizeof(asc_str));

        p_unistr = (u16 *)gui_get_string_addr(IDS_SEARCH_RESULT);
        uni_strcpy(uni_str1, p_unistr);

        sprintf(asc_str, "(%ld", this->total_vdo);
        str_asc2uni(asc_str, uni_str2);
        uni_strcat(uni_str1, uni_str2, 50);

        p_unistr = (u16 *)gui_get_string_addr(IDS_RELATED_ARTICAL);
        uni_strcat(uni_str1, p_unistr , 50);

        memset(uni_str2, 0, sizeof(uni_str2));
        str_asc2uni(")", uni_str2);
        uni_strcat(uni_str1, uni_str2, 50);

        text_set_content_by_unistr(p_ctrl, uni_str1);
    }
    else
    {
        if (context == SID_IPTV_SEARCH_SEARCH_NORMAL)
        {
            p_unistr = (u16 *)gui_get_string_addr(IDS_SEARCH_RUNNING);
        }
        else if (context == SID_IPTV_SEARCH_SEARCH_REQ)
        {
            p_unistr = (u16 *)gui_get_string_addr(IDS_NO_RELEVANT_RESULTS);
        }
        if (p_unistr)
        {
            text_set_content_by_unistr(p_ctrl, p_unistr);
        }
    }
}

static RET_CODE ui_iptvsearch_update_vdolist(control_t *p_vdolist, u16 start, u16 size, u32 context)
{
    u16 i, cnt, start_num;
    u8  asc_str[10+1];

    OS_PRINTF("@@@ui_iptvsearch_update_vdolist start=%d\n", start);
    cnt = list_get_count(p_vdolist);
    start_num = (this->savPageNum - 1) * this->vdo_page_size + 1;

    for (i = start; i < start + size && i < cnt; i++)
    {
        sprintf(asc_str, "%d.", start_num+i);
        list_set_field_content_by_ascstr(p_vdolist, i, 0, asc_str);
        list_set_field_content_by_extstr(p_vdolist, i, 1, this->vdoList[i].name);
    }

    return SUCCESS;
}

static void ui_iptvsearch_update_curVdoPage(u32 context)
{
    list_set_count(this->hVdolist, this->vdo_cnt, this->vdo_page_size);
    list_set_focus_pos(this->hVdolist, this->curVdoListIdx);

    if (!ctrl_is_onfocus(this->hKbdCont))
    {
        ctrl_process_msg(this->hVdolist, MSG_GETFOCUS, 0, 0);
    }

    ui_iptvsearch_update_title(this->hTitle, context);
    ui_iptvsearch_update_vdolist(this->hVdolist, 0, this->vdo_page_size, 0);

    ctrl_paint_ctrl(this->hClientCont, TRUE);
}

static RET_CODE on_iptvsearch_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    OS_PRINTF("@@@on_iptvsearch_destory\n");
    IptvSearch_CloseStateTree();

    ui_iptvsearch_release_app_data();

    p_root = fw_find_root_by_id(ROOT_ID_IPTV);
    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    return ERR_NOFEATURE;
}

static RET_CODE ui_iptvsearch_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV_SEARCH,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    OS_PRINTF("@@@ui_iptvsearch_on_open_cfm_dlg\n");
    if (IptvSearch_IsStateActive(SID_IPTV_SEARCH_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);

        p_root = fw_find_root_by_id(ROOT_ID_IPTV_SEARCH);
        if (p_root)
        {
            fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
        }
    }

    return SUCCESS;
}

/*fresh the keyborad*/
static void update_input_method(control_t *p_mbox)
{
  u8 i = 0;
  u16 uni_str[2];
  uni_str[1] = '\0';

  uni_str[0] = 0x61;
  for(i = 0; i < KB_IPTVSEARCH_MATRIX_KEYS_COUNT; i++)
  {
    mbox_set_content_by_unistr(p_mbox, i, uni_str);
    if((uni_str[0] >= 0x30
       && uni_str[0] <= 0x39)
      || (uni_str[0] >= 0x61
         && uni_str[0] <= 0x7a))
    {
      uni_str[0]++;
    }
    if(uni_str[0] == 0x7b)
    {
      uni_str[0] = 0x30;
    }
  }
}

/*when the matrix focus get out of the matrix,*/
static RET_CODE on_iptvsearch_change_focus_mbox(control_t *p_mbox, u16 msg, u32 para1, u32 para2)	
{	
    this->cur_pos = mbox_get_focus(p_mbox);
    OS_PRINTF("@@@on_iptvsearch_change_focus_mbox this->cur_pos=%d\n", this->cur_pos);

    switch(msg)
    {
        case MSG_FOCUS_DOWN:
            if ((this->cur_pos / IPTV_MBOX_ROW) == 5)
            {
                if((this->cur_pos == 30) || (this->cur_pos == 31))
                {
                    ctrl_change_focus(p_mbox, this->hBtnDel);
                }
                else if((this->cur_pos == 32) || (this->cur_pos == 33))
                {
                    ctrl_change_focus(p_mbox, this->hBtnSpace);
                }
                else if((this->cur_pos == 34) || (this->cur_pos == 35))
                {
                    ctrl_change_focus(p_mbox, this->hBtnClr);
                }	
                return SUCCESS;
            }
            break;
        case MSG_FOCUS_RIGHT:
            if ((this->cur_pos % IPTV_MBOX_ROW) == 5)
            {
                if (this->vdoList)
                {
                    ctrl_change_focus(p_mbox, this->hVdolist);
                    return SUCCESS;
                }
            }
            break;
        default:
            break;
    }

    mbox_class_proc(p_mbox, msg, para1, para2);
    return SUCCESS;
}

/*when press the matrix, refresh the  letter in the edit */
static RET_CODE on_iptvsearch_input_letter(control_t *p_mbox, u16 msg,
                                     u32 para1, u32 para2)	
{
	u32 *p_str = NULL;
	u16 uni_str[2] = {0};
	
	this->cur_pos = mbox_get_focus(p_mbox);
	p_str = mbox_get_content_str(p_mbox, this->cur_pos);
	uni_strcpy(uni_str, (u16 *)(*p_str));
	ebox_input_uchar(this->hEditor, uni_str[0]);
	ctrl_paint_ctrl(this->hEditor, TRUE); 

	return SUCCESS;
}

/*when press the delete button,delete the letter in the edit */
static RET_CODE on_iptvsearch_delete(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ebox_back_space(this->hEditor);
	ctrl_paint_ctrl(this->hEditor, TRUE);
	
	return SUCCESS;
}

static RET_CODE on_iptvsearch_delete_move(control_t *p_button, u16 msg, u32 para1, u32 para2)	
{
    switch(msg)
    {
        case MSG_FOCUS_UP:
            this->cur_pos = 30;
            mbox_set_focus(this->hMbox, this->cur_pos);
            ctrl_change_focus(this->hBtnDel, this->hMbox);
            return SUCCESS;
        case MSG_FOCUS_DOWN:
            ctrl_change_focus(this->hBtnDel, this->hBtnSearch);
            return SUCCESS;
        case MSG_FOCUS_LEFT:
            ctrl_change_focus(this->hBtnDel, this->hBtnClr);
            return SUCCESS;
        case MSG_FOCUS_RIGHT:
            ctrl_change_focus(this->hBtnDel, this->hBtnSpace);
            return SUCCESS;
        default:
            break;
    }

    return ERR_NOFEATURE;
}


/*when press the space button */
static RET_CODE on_iptvsearch_space(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 uni_str[2] = {0};

	uni_str[0] = kb_iptv_custom_string[2];
	ebox_input_uchar(this->hEditor, uni_str[0]);
	ctrl_paint_ctrl(this->hEditor, TRUE); 

	return SUCCESS;
}

static RET_CODE on_iptvsearch_space_move(control_t *p_button, u16 msg, u32 para1, u32 para2)	
{
    switch(msg)
    {
        case MSG_FOCUS_UP:
            this->cur_pos = 32;
            mbox_set_focus(this->hMbox, this->cur_pos);
            ctrl_change_focus(this->hBtnSpace, this->hMbox);
            return SUCCESS;
        case MSG_FOCUS_DOWN:
            ctrl_change_focus(this->hBtnSpace, this->hBtnSearch);
            return SUCCESS;
        case MSG_FOCUS_LEFT:
            ctrl_change_focus(this->hBtnSpace, this->hBtnDel);
            return SUCCESS;
        case MSG_FOCUS_RIGHT:
            ctrl_change_focus(this->hBtnSpace, this->hBtnClr);
            return SUCCESS;
        default:
            break;
    }

    return ERR_NOFEATURE;
}


/*when press the clear button */
static RET_CODE on_iptvsearch_clear(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 uni_str[2] = {0};

	uni_str[0] = kb_iptv_custom_string[0];
	ebox_set_content_by_unistr(this->hEditor, uni_str);
	ctrl_paint_ctrl(this->hEditor, TRUE); 

	return SUCCESS;
}

static RET_CODE on_iptvsearch_clear_move(control_t *p_button, u16 msg,
	                                  u32 para1, u32 para2)	
{
    switch(msg)
    {
        case MSG_FOCUS_UP:
            this->cur_pos = 34;
            mbox_set_focus(this->hMbox, this->cur_pos);
            ctrl_change_focus(this->hBtnClr, this->hMbox);
            return SUCCESS;
        case MSG_FOCUS_DOWN:
            ctrl_change_focus(this->hBtnClr, this->hBtnSearch);
            return SUCCESS;
        case MSG_FOCUS_LEFT:
            ctrl_change_focus(this->hBtnClr, this->hBtnSpace);
            return SUCCESS;
        case MSG_FOCUS_RIGHT:
            if (this->vdoList)
            {
                ctrl_change_focus(this->hBtnClr, this->hVdolist);
            }
            else
            {
                ctrl_change_focus(this->hBtnClr, this->hBtnDel);
            }
            return SUCCESS;
        default:
        break;
    } 	
    return ERR_NOFEATURE;
}

/*================================================================================================
                           iptv search sm function
 ================================================================================================*/
static BOOL    StcIptvSearch_is_on_matrixkey(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    if (ctrl_id == IDC_IPTV_MBOX)
    {
        return TRUE;
    }
    
    return FALSE;
}

static BOOL    StcIptvSearch_is_on_customkey(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    if (ctrl_id >= IDC_IPTV_BTN_DEL && ctrl_id <= IDC_IPTV_BTN_CLR)
    {
        return TRUE;
    }

    return FALSE;
}

static BOOL    StcIptvSearch_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    if (ctrl_id == IDC_IPTV_VDOLIST)
    {
        return TRUE;
    }
    
    return FALSE;
}
static BOOL    StcIptvSearch_is_on_searchkey(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    if (ctrl_id == IDC_IPTV_BTN_SEARCH)
    {
        return TRUE;
    }
    
    return FALSE;
}

static void SenIptvSearch_Inactive(void)
{
    OS_PRINTF("@@@SenIptvSearch_Inactive\n");
}
static STATEID StaIptvSearch_inactive_on_open_iptv_search_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptvSearch_inactive_on_open_iptv_search_req\n");
    return SID_IPTV_SEARCH_SEARCH_NORMAL;
}
static void SexIptvSearch_Inactive(void)
{
    OS_PRINTF("@@@SexIptvSearch_Inactive\n");
}


static void SenIptvSearch_Active(void)
{
    OS_PRINTF("@@@SenIptvSearch_Active\n");
    ui_iptv_set_page_size(this->vdo_page_size);
}
static STATEID StaIptvSearch_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    OS_PRINTF("@@@StaIptvSearch_active_on_exit\n");
    switch (msg)
    {
        case MSG_INTERNET_PLUG_OUT:
        case MSG_EXIT:
        case MSG_INFO:
            p_root = fw_find_root_by_id(ROOT_ID_IPTV_SEARCH);
            if (p_root)
            {
                fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
            }
            break;
        default:
            break;
    }

    return SID_NULL;
}

static void SexIptvSearch_Active(void)
{
    OS_PRINTF("@@@SexIptvSearch_Active\n");
}

static void SenIptvSearch_Search(void)
{
    OS_PRINTF("@@@SenIptvSearch_Search\n");
}

static STATEID StaIptvSearch_search_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    OS_PRINTF("@@@StaIptvSearch_search_on_focus_key_hldr ctrl_id=%d\n", ctrl_id);
    if (ctrl_id == IDC_IPTV_MBOX)
    {
        switch (msg)
        {
            case MSG_FOCUS_UP:
            case MSG_FOCUS_DOWN:
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
                on_iptvsearch_change_focus_mbox(ctrl, msg, para1, para2);
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_VDOLIST)
    {
        switch (msg)
        {
            case MSG_FOCUS_UP:
                if (this->curVdoListIdx > 0)
                {
                    list_class_proc(ctrl, msg, para1, para2);
                    this->curVdoListIdx = list_get_focus_pos(ctrl);
                }
                else
                {
                    if (this->curPageNum > 1)
                    {
                        if (this->curPageNum == this->savPageNum)
                        {
                            this->curPageNum--;
                            return SID_IPTV_SEARCH_SEARCH_REQ;
                        }
                    }
                }
                break;

            case MSG_FOCUS_DOWN:
                if (this->curVdoListIdx + 1 < this->vdo_cnt)
                {
                    list_class_proc(ctrl, msg, para1, para2);
                    this->curVdoListIdx = list_get_focus_pos(ctrl);
                }
                else
                {
                    if (this->curPageNum < this->total_page)
                    {
                        if (this->curPageNum == this->savPageNum)
                        {
                            this->curPageNum++;
                            return SID_IPTV_SEARCH_SEARCH_REQ;
                        }
                    }
                    return SID_NULL;
                }
                break;

            case MSG_FOCUS_LEFT:
                ctrl_change_focus(this->hVdolist, this->hKbdCont);
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_BTN_DEL)
    {
        switch (msg)
        {
            case MSG_FOCUS_UP:
            case MSG_FOCUS_DOWN:
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
                on_iptvsearch_delete_move(ctrl, msg, para1, para2);
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_BTN_SPACE)
    {
        switch (msg)
        {
            case MSG_FOCUS_UP:
            case MSG_FOCUS_DOWN:
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
                on_iptvsearch_space_move(ctrl, msg, para1, para2);
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_BTN_CLR)
    {
        switch (msg)
        {
            case MSG_FOCUS_UP:
            case MSG_FOCUS_DOWN:
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
                on_iptvsearch_clear_move(ctrl, msg, para1, para2);
                break;
        }
    }
    else if (ctrl_id == IDC_IPTV_BTN_SEARCH)
    {
        switch (msg)
        {
            case MSG_FOCUS_RIGHT:
                if (this->vdoList)
                {
                    ctrl_change_focus(this->hBtnSearch, this->hVdolist);
                }
                break;
            case MSG_FOCUS_UP:
                ctrl_change_focus(this->hBtnSearch, this->hBtnClr);
                break;
        }
    }

    return SID_NULL;
}
static STATEID StaIptvSearch_search_on_matrixkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 uchar;

    switch (msg)
    {
        case MSG_SELECT:
            OS_PRINTF("@@@StaIptvSearch_search_on_matrixkey_selected MSG_SELECT\n");
            on_iptvsearch_input_letter(ctrl, msg, para1, para2);
            break;
        case MSG_NUMBER:
            OS_PRINTF("@@@StaIptvSearch_search_on_matrixkey_selected MSG_NUMBER\n");
            uchar = (u16)('0' + para1);
            ebox_input_uchar(this->hEditor, uchar);
            ctrl_paint_ctrl(this->hEditor, TRUE);
            break;
    }

    return SID_NULL;
}

static STATEID StaIptvSearch_search_on_customkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u8  ctrl_id = ctrl_get_ctrl_id(ctrl);

    OS_PRINTF("@@@StaIptvSearch_search_on_customkey_selected ctrl_id=%d\n", ctrl_id);
    if (ctrl_id == IDC_IPTV_BTN_DEL)
    {
        on_iptvsearch_delete(ctrl, msg, para1, para2);
    }
    else if (ctrl_id == IDC_IPTV_BTN_SPACE)
    {
        on_iptvsearch_space(ctrl, msg, para1, para2);
    }
    else if (ctrl_id == IDC_IPTV_BTN_CLR)
    {
        on_iptvsearch_clear(ctrl, msg, para1, para2);
    }
    return SID_NULL;
}

static void SexIptvSearch_Search(void)
{
    OS_PRINTF("@@@SexIptvSearch_Search\n");
}

static void SenIptvSearch_SearchNormal(void)
{
    OS_PRINTF("@@@SenIptvSearch_SearchNormal\n");
}

static STATEID StaIptvSearch_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptvSearch_search_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (this->curPageNum > 1)
        {
            if (this->curPageNum == this->savPageNum)
            {
                this->curPageNum--;
                return SID_IPTV_SEARCH_SEARCH_REQ;
            }
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (this->curPageNum < this->total_page)
        {
            if (this->curPageNum == this->savPageNum)
            {
                this->curPageNum++;
                return SID_IPTV_SEARCH_SEARCH_REQ;
            }
        }
    }

    return SID_NULL;
}

static STATEID StaIptvSearch_search_on_searchkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 *p_unistr = NULL;

    ui_iptvsearch_release_vdolist();

    this->total_vdo = 0;
    this->vdo_cnt = 0;
    this->curVdoListIdx = 0;
    this->total_page = 0;
    this->savPageNum = this->curPageNum = 1;
    ui_iptvsearch_update_curVdoPage(SID_IPTV_SEARCH_SEARCH_NORMAL);

    p_unistr = ebox_get_content(this->hEditor);
    ui_iptv_search_set_keyword(p_unistr);
    return SID_IPTV_SEARCH_SEARCH_REQ;
}

static STATEID StaIptvSearch_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_iptv_description_param_t p_param;

    OS_PRINTF("@@@StaIptvSearch_search_on_vdolist_selected curVdoListIdx=%d\n", this->curVdoListIdx);
    if (this->vdo_cnt == 0)
    {
        return SID_NULL;
    }

    p_param.vdo_id = this->vdoList[this->curVdoListIdx].vdo_id;
    p_param.res_id = this->vdoList[this->curVdoListIdx].res_id;
    p_param.b_single_page = this->vdoList[this->curVdoListIdx].b_single_page;
    OS_PRINTF("@@@vdo_id=%d, res_id=%d, b_single_page=%d\n", p_param.vdo_id, p_param.res_id, p_param.b_single_page);
    manage_open_menu(ROOT_ID_IPTV_DESCRIPTION, (u32)&p_param, (u32)ROOT_ID_INVALID);

    return SID_NULL;
}

static void SexIptvSearch_SearchNormal(void)
{
    OS_PRINTF("@@@SexIptvSearch_SearchNormal\n");
}

static void SenIptvSearch_SearchReq(void)
{
    OS_PRINTF("@@@SenIptvSearch_SearchReq\n");
    ui_iptv_search(this->curPageNum);
}

static STATEID StaIptvSearch_search_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)para1;
    al_iptv_vdo_item_t *p_vdoList;
    u16 *p_name;
    u8  *p_url;
    u16 vdo_cnt;
    u16 i;
    u16 str_len;

    OS_PRINTF("@@@StaIptvSearch_search_on_newpagevdo_arrive\n");
    if (fw_get_focus_id() != ROOT_ID_IPTV_SEARCH)
    {
        return SID_NULL;
    }

    OS_PRINTF("@@@p_data->vdo_cnt=%d\n", p_data->vdo_cnt);
    if (this->vdoList)
    {//next req
        if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
        {
            ui_iptvsearch_release_vdolist();

            vdo_cnt = MIN(p_data->vdo_cnt, this->vdo_page_size);
            p_vdoList = (al_iptv_vdo_item_t *)mtos_malloc(vdo_cnt * sizeof(al_iptv_vdo_item_t));
            MT_ASSERT(p_vdoList != NULL);
            for (i = 0; i < vdo_cnt; i++)
            {
                p_vdoList[i].vdo_id = p_data->vdoList[i].vdo_id;
                p_vdoList[i].res_id = p_data->vdoList[i].res_id;
                p_vdoList[i].b_single_page = p_data->vdoList[i].b_single_page;

                str_len = uni_strlen(p_data->vdoList[i].name);
                p_name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_name != NULL);
                uni_strcpy(p_name, p_data->vdoList[i].name);
                p_vdoList[i].name = p_name;

                str_len = strlen(p_data->vdoList[i].img_url);
                p_url = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_url != NULL);
                strcpy(p_url, p_data->vdoList[i].img_url);
                p_vdoList[i].img_url = p_url;
            }
            this->vdoList = p_vdoList;

            this->vdo_cnt = vdo_cnt;
            if (this->curVdoListIdx >= vdo_cnt)
            {
                this->curVdoListIdx = vdo_cnt - 1;
            }
            this->savPageNum = this->curPageNum;

            ui_iptvsearch_update_curVdoPage(SID_IPTV_SEARCH_SEARCH_REQ);
            return SID_IPTV_SEARCH_SEARCH_NORMAL;
        }
        else
        {
            this->curPageNum = this->savPageNum;
            ui_iptv_open_cfm_dlg(ROOT_ID_IPTV_SEARCH, IDS_DATA_ERROR);
            return SID_NULL;
        }
    }
    else
    {// first req
        if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
        {
            vdo_cnt = MIN(p_data->vdo_cnt, this->vdo_page_size);
            p_vdoList = (al_iptv_vdo_item_t *)mtos_malloc(vdo_cnt * sizeof(al_iptv_vdo_item_t));
            MT_ASSERT(p_vdoList != NULL);

            for (i = 0; i < vdo_cnt; i++)
            {
                p_vdoList[i].vdo_id = p_data->vdoList[i].vdo_id;
                p_vdoList[i].res_id = p_data->vdoList[i].res_id;
                p_vdoList[i].b_single_page = p_data->vdoList[i].b_single_page;

                str_len = uni_strlen(p_data->vdoList[i].name);
                p_name = (u16 *)mtos_malloc((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_name != NULL);
                uni_strcpy(p_name, p_data->vdoList[i].name);
                p_vdoList[i].name = p_name;

                str_len = strlen(p_data->vdoList[i].img_url);
                p_url = (u8 *)mtos_malloc((str_len + 1));
                MT_ASSERT(p_url != NULL);
                strcpy(p_url, p_data->vdoList[i].img_url);
                p_vdoList[i].img_url = p_url;
            }
            this->vdoList = p_vdoList;
            if (this->curVdoListIdx >= vdo_cnt)
            {
                this->curVdoListIdx = vdo_cnt - 1;
            }
        }
        else
        {
            vdo_cnt = 0;
            this->curVdoListIdx = 0;
        }

        this->total_vdo = p_data->total_vdo;
        this->vdo_cnt = vdo_cnt;
        this->total_page = p_data->total_page;

        ui_iptvsearch_update_curVdoPage(SID_IPTV_SEARCH_SEARCH_REQ);
        return SID_IPTV_SEARCH_SEARCH_NORMAL;
    }
}

static STATEID StaIptvSearch_search_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptvSearch_search_on_get_pagevdo_fail\n");
    if (fw_get_focus_id() != ROOT_ID_IPTV_SEARCH)
    {
        return SID_NULL;
    }

    this->curPageNum = this->savPageNum;

    ui_iptv_open_cfm_dlg(ROOT_ID_IPTV_SEARCH, IDS_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaIptvSearch_search_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("@@@StaIptvSearch_search_on_vdoreq_dlg_closed\n");
    return SID_IPTV_SEARCH_SEARCH_NORMAL;
}

static void SexIptvSearch_SearchReq(void)
{
    OS_PRINTF("@@@SexIptvSearch_SearchReq\n");
    ui_comm_dlg_close();
}

/*================================================================================================
                           iptv search public works function
 ================================================================================================*/
/*the entry for the keyboard iptv search*/
RET_CODE ui_open_iptv_search(u32 para1,u32 para2)
{
    control_t *p_cont = NULL;
    control_t *p_kbd_cont = NULL, *p_editor_name = NULL, *p_editor = NULL, *p_mbox = NULL;
    control_t *p_btn_search = NULL, *p_btn_space = NULL, *p_btn_del = NULL, *p_btn_clr = NULL;
    control_t *p_client_cont = NULL, *p_title = NULL, *p_vdolist = NULL;
    u16 *p_unistr;
    u16 i = 0;

    OS_PRINTF("@@@ui_open_iptv_search\n");
    ui_iptvsearch_init_app_data();

    /*!
     * Create Menu
     */
    p_cont = ui_comm_root_create_netmenu(ROOT_ID_IPTV_SEARCH, 0,
                                    IM_INDEX_NETWORK_BANNER, IDS_VOD);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_keymap(p_cont, iptv_search_cont_keymap);
    ctrl_set_proc(p_cont, iptv_search_cont_proc);

    /*!
     * Create keyboard container
     */
    p_kbd_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_KBD_CONT,
                                  IPTV_KBD_CONTX, IPTV_KBD_CONTY,
                                  IPTV_KBD_CONTW, IPTV_KBD_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_kbd_cont, RSI_BOX3, RSI_BOX3, RSI_BOX3);

    // Editor name
    p_editor_name = ctrl_create_ctrl(CTRL_TEXT, (u8)0,
                                 IPTV_EDITOR_NAME_X, IPTV_EDITOR_NAME_Y,
                                 IPTV_EDITOR_NAME_W, IPTV_EDITOR_NAME_H,
                                 p_kbd_cont, 0);
    ctrl_set_rstyle(p_editor_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_editor_name, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_editor_name, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);
    text_set_content_type(p_editor_name, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_editor_name, IDS_SEARCH);

    /*init the edit */
    p_editor = ctrl_create_ctrl(CTRL_EBOX, IDC_IPTV_EDITOR,
                                IPTV_EDITOR_X, IPTV_EDITOR_Y,
                                IPTV_EDITOR_W, IPTV_EDITOR_H,
                                p_kbd_cont, 0);
    ctrl_set_rstyle(p_editor, RSI_OTT_IM_SEARCH, RSI_OTT_IM_SEARCH, RSI_OTT_IM_SEARCH);
    ebox_set_font_style(p_editor, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    ebox_set_maxtext(p_editor, MAX_KEYWORD_LENGTH);
    ebox_set_worktype(p_editor, EBOX_WORKTYPE_SHIFT);

    /*init the keyboard*/
    p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_IPTV_MBOX,
                              IPTV_MBOX_X, IPTV_MBOX_Y,
                              IPTV_MBOX_W, IPTV_MBOX_H,
                              p_kbd_cont, 0);
    ctrl_set_keymap(p_mbox, keyboard_input_method_keymap);
    ctrl_set_proc(p_mbox, keyboard_input_method_proc);
    mbox_enable_icon_mode(p_mbox, TRUE);
    mbox_enable_string_mode(p_mbox, TRUE);
    mbox_set_count(p_mbox, KB_IPTVSEARCH_MATRIX_KEYS_COUNT, IPTV_MBOX_COL, IPTV_MBOX_ROW);
    ctrl_set_mrect(p_mbox, 0, 0, IPTV_MBOX_W, IPTV_MBOX_H);
    mbox_set_item_interval(p_mbox, IPTV_MBOX_H_GAP, IPTV_MBOX_V_GAP);
    mbox_set_item_rstyle(p_mbox, RSI_OTT_KEYBOARD_KEY_HL, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH);
    mbox_set_string_fstyle(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    mbox_set_icon_offset(p_mbox, 0, 0);
    mbox_set_icon_align_type(p_mbox, STL_CENTER | STL_VCENTER);
    mbox_set_string_offset(p_mbox, 0, 0);
    mbox_set_string_align_type(p_mbox, STL_CENTER | STL_VCENTER);
    mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_UNICODE);  
    mbox_set_focus(p_mbox, this->cur_pos);
    update_input_method(p_mbox);

    /*init the delete button */
    p_btn_del = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_BTN_DEL, 
                                IPTV_DEL_BTN_X, IPTV_DEL_BTN_Y,
                                IPTV_DEL_BTN_W, IPTV_DEL_BTN_H,
                                p_kbd_cont, 0);
    text_set_font_style(p_btn_del, FSI_BLACK, FSI_WHITE, FSI_BLACK);
    ctrl_set_rstyle(p_btn_del, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    text_set_content_type(p_btn_del, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn_del, IDS_DELETE);
    ctrl_set_keymap(p_btn_del, iptv_delete_method_keymap);
    ctrl_set_proc(p_btn_del, iptv_delete_proc);

    /*init the  space button */
    p_btn_space = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_BTN_SPACE, 
                                    IPTV_SPACE_BTN_X, IPTV_SPACE_BTN_Y,
                                    IPTV_SPACE_BTN_W, IPTV_SPACE_BTN_H,
                                    p_kbd_cont, 0);
    text_set_font_style(p_btn_space, FSI_BLACK, FSI_WHITE, FSI_BLACK);
    ctrl_set_rstyle(p_btn_space, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    text_set_content_type(p_btn_space, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn_space, IDS_SPACE);
    ctrl_set_keymap(p_btn_space, iptv_space_method_keymap);
    ctrl_set_proc(p_btn_space, iptv_space_proc);

    /*init the clear button */
    p_btn_clr = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_BTN_CLR, 
                                IPTV_CLR_BTN_X, IPTV_CLR_BTN_Y,
                                IPTV_CLR_BTN_W, IPTV_CLR_BTN_H,
                                p_kbd_cont, 0);
    text_set_font_style(p_btn_clr, FSI_BLACK, FSI_WHITE, FSI_BLACK);
    ctrl_set_rstyle(p_btn_clr, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    text_set_content_type(p_btn_clr, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn_clr, IDS_EMPTY);
    ctrl_set_keymap(p_btn_clr, iptv_clear_method_keymap);
    ctrl_set_proc(p_btn_clr, iptv_clear_proc);

    /*init the search button */
    p_btn_search = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_BTN_SEARCH,
                                            IPTV_SEARCH_BTN_X, IPTV_SEARCH_BTN_Y,
                                            IPTV_SEARCH_BTN_W, IPTV_SEARCH_BTN_H,
                                            p_kbd_cont, 0);
    text_set_font_style(p_btn_search, FSI_BLACK, FSI_WHITE, FSI_BLACK);
    ctrl_set_rstyle(p_btn_search, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    text_set_content_type(p_btn_search, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_btn_search, IDS_START_SEARCH);
    ctrl_set_keymap(p_btn_search, iptv_search_method_keymap);
    ctrl_set_proc(p_btn_search, iptv_search_proc);

    /*!
     * Create client container
     */
    p_client_cont = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_IPTV_CLIENT_CONT,
                                  IPTV_CLIENT_CONTX, IPTV_CLIENT_CONTY,
                                  IPTV_CLIENT_CONTW, IPTV_CLIENT_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_client_cont, RSI_BOX3, RSI_BOX3, RSI_BOX3);

    // Title
    p_title = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_IPTV_TITLE,
                                 IPTV_TITLE_X, IPTV_TITLE_Y,
                                 IPTV_TITLE_W, IPTV_TITLE_H,
                                 p_client_cont, 0);
    ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_title, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);
    text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
    p_unistr = (u16 *)gui_get_string_addr(IDS_SEARCH_RESULT);
    text_set_content_by_unistr(p_title, p_unistr);

    // Vdolist
    p_vdolist = ctrl_create_ctrl(CTRL_LIST, (u8)IDC_IPTV_VDOLIST,
                          IPTV_VDOLIST_X, IPTV_VDOLIST_Y,
                          IPTV_VDOLIST_W, IPTV_VDOLIST_H,
                          p_client_cont, 0);
    ctrl_set_rstyle(p_vdolist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_vdolist, iptvsearch_vdolist_keymap);
    ctrl_set_proc(p_vdolist, iptvsearch_vdolist_proc);

#if 0
    ctrl_set_mrect(p_vdolist,
                  IPTV_CATLIST_MIDL, IPTV_CATLIST_MIDT,
                  IPTV_CATLIST_MIDR, IPTV_CATLIST_MIDB);
#endif
    list_set_item_interval(p_vdolist, IPTV_VDOLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_vdolist, &vdolist_item_rstyle);
    list_set_count(p_vdolist, 0, IPTVSEARCH_VDOLIST_PAGE_SIZE);//should not be removed
    list_set_field_count(p_vdolist, ARRAY_SIZE(vdolist_attr), IPTVSEARCH_VDOLIST_PAGE_SIZE);
    list_set_update(p_vdolist, ui_iptvsearch_update_vdolist, 0);

    for (i = 0; i < ARRAY_SIZE(vdolist_attr); i++)
    {
        list_set_field_attr(p_vdolist, (u8)i, vdolist_attr[i].attr,
                            vdolist_attr[i].width, vdolist_attr[i].left, vdolist_attr[i].top);
        list_set_field_rect_style(p_vdolist, (u8)i, vdolist_attr[i].rstyle);
        list_set_field_font_style(p_vdolist, (u8)i, vdolist_attr[i].fstyle);
    }
    this->vdo_page_size = IPTVSEARCH_VDOLIST_PAGE_SIZE;

    this->hEditor = p_editor;
    this->hMbox = p_mbox;
    this->hBtnSearch = p_btn_search;
    this->hBtnSpace = p_btn_space;
    this->hBtnDel = p_btn_del;
    this->hBtnClr = p_btn_clr;
    this->hTitle = p_title;
    this->hVdolist = p_vdolist;
    this->hKbdCont = p_kbd_cont;
    this->hClientCont = p_client_cont;

    ebox_enter_edit(p_editor);
    ctrl_default_proc(p_mbox, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

    IptvSearch_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_IPTV_SEARCH), NOTIFY_T_MSG, FALSE, MSG_OPEN_IPTV_SEARCH_REQ, 0, 0);
    return SUCCESS;
}

static RET_CODE ui_iptvsearch_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;

    ret = IptvSearch_DispatchMsg(p_ctrl, msg, para1, para2);

    if (ret != SUCCESS)
    {
        OS_PRINTF("@@@@@@iptvsearch unaccepted msg, id=0x%04x\n", msg);
    }

    return ret;
}


/*when press the number in the remoter, refresh the  letter in the edit */
static RET_CODE ui_iptvsearch_on_input_number(control_t *p_mbox, u16 msg, u32 para1, u32 para2)	
{
    para1 = (u32)(msg & MSG_DATA_MASK);
    msg = (msg & MSG_TYPE_MASK);
    ui_iptvsearch_on_state_process(p_mbox, msg, para1, para2);

    return SUCCESS;
}

BEGIN_KEYMAP(iptv_search_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(iptv_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(iptv_search_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL, ui_iptvsearch_on_state_process)

    ON_COMMAND(MSG_OPEN_IPTV_SEARCH_REQ, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_INFO, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_DESTROY, on_iptvsearch_destory)

    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_iptvsearch_on_open_cfm_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_iptvsearch_on_state_process)
END_MSGPROC(iptv_search_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(keyboard_input_method_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(keyboard_input_method_keymap, NULL)


BEGIN_MSGPROC(keyboard_input_method_proc, mbox_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_NUMBER, ui_iptvsearch_on_input_number)
END_MSGPROC(keyboard_input_method_proc, mbox_class_proc)

BEGIN_KEYMAP(iptv_delete_method_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)	
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)	
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_delete_method_keymap, NULL)

BEGIN_MSGPROC(iptv_delete_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
END_MSGPROC(iptv_delete_proc, text_class_proc)

BEGIN_KEYMAP(iptv_space_method_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)	
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)	
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_space_method_keymap, NULL)

BEGIN_MSGPROC(iptv_space_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
END_MSGPROC(iptv_space_proc, text_class_proc)

BEGIN_KEYMAP(iptv_clear_method_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_clear_method_keymap, NULL)

BEGIN_MSGPROC(iptv_clear_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
END_MSGPROC(iptv_clear_proc, text_class_proc)

BEGIN_KEYMAP(iptv_search_method_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_search_method_keymap, NULL)

BEGIN_MSGPROC(iptv_search_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
END_MSGPROC(iptv_search_proc, text_class_proc)

BEGIN_KEYMAP(iptvsearch_vdolist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptvsearch_vdolist_keymap, NULL)

BEGIN_MSGPROC(iptvsearch_vdolist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_LEFT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_FOCUS_RIGHT, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_PAGE_UP, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_PAGE_DOWN, ui_iptvsearch_on_state_process)
    ON_COMMAND(MSG_SELECT, ui_iptvsearch_on_state_process)
END_MSGPROC(iptvsearch_vdolist_proc, list_class_proc)
#endif

