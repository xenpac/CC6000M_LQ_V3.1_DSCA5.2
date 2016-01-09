/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NM_SIMPLE_H__
#define __UI_NM_SIMPLE_H__

/*!
 * Const value
 */
//Class list container
#define NM_SIMPLE_CLASS_LIST_CONTX    (120 - 30)
#define NM_SIMPLE_CLASS_LIST_CONTY    (100)
#define NM_SIMPLE_CLASS_LIST_CONTW    (200 + 30)
#define NM_SIMPLE_CLASS_LIST_CONTH    (570)

//Logo
#define NM_SIMPLE_LOGO_X    (0)
#define NM_SIMPLE_LOGO_Y    (0)
#define NM_SIMPLE_LOGO_W    (NM_SIMPLE_CLASS_LIST_CONTW)
#define NM_SIMPLE_LOGO_H    (90)

//Class list
#define NM_SIMPLE_CLASS_LIST_X    (10)//(20)
#define NM_SIMPLE_CLASS_LIST_Y    (90)
#define NM_SIMPLE_CLASS_LIST_W    (180 + 30)//(160)
#define NM_SIMPLE_CLASS_LIST_H    (400)

#define NM_SIMPLE_CLASS_LIST_MIDL   (0)
#define NM_SIMPLE_CLASS_LIST_MIDT   (0)
#define NM_SIMPLE_CLASS_LIST_MIDW   (NM_SIMPLE_CLASS_LIST_W - NM_SIMPLE_CLASS_LIST_MIDL - NM_SIMPLE_CLASS_LIST_MIDL)
#define NM_SIMPLE_CLASS_LIST_MIDH   (NM_SIMPLE_CLASS_LIST_H - NM_SIMPLE_CLASS_LIST_MIDT - NM_SIMPLE_CLASS_LIST_MIDT)

#define NM_SIMPLE_CLASS_LIST_ICON_VGAP    (4)
#define NM_SIMPLE_CLASS_LIST_ITEM_NUM_ONE_PAGE  (9)
#define NM_SIMPLE_CLASS_LIST_ITEM_COUNT   (8)

#define NM_SIMPLE_SEARCH_HELP_X     (NM_SIMPLE_CLASS_LIST_X)
#define NM_SIMPLE_SEARCH_HELP_Y     (NM_SIMPLE_CLASS_LIST_CONTH - NM_SIMPLE_SEARCH_HELP_H)
#define NM_SIMPLE_SEARCH_HELP_W     (NM_SIMPLE_CLASS_LIST_W)
#define NM_SIMPLE_SEARCH_HELP_H     (50)

//Page container
#define NM_SIMPLE_PAGE_CONTX    (NM_SIMPLE_CLASS_LIST_CONTX + NM_SIMPLE_CLASS_LIST_CONTW + 10)
#define NM_SIMPLE_PAGE_CONTY    (100)
#define NM_SIMPLE_PAGE_CONTW    (830 + 30)
#define NM_SIMPLE_PAGE_CONTH    (50)

//Page arrow left
#define NM_SIMPLE_PAGE_ARROWL_X   (150 + 15)
#define NM_SIMPLE_PAGE_ARROWL_Y   (7)
#define NM_SIMPLE_PAGE_ARROWL_W   (36)
#define NM_SIMPLE_PAGE_ARROWL_H   (36)
//Page arrow right
#define NM_SIMPLE_PAGE_ARROWR_X   (NM_SIMPLE_PAGE_CONTW - NM_SIMPLE_PAGE_ARROWL_X - NM_SIMPLE_PAGE_ARROWR_W + 15)
#define NM_SIMPLE_PAGE_ARROWR_Y   (7)
#define NM_SIMPLE_PAGE_ARROWR_W   (36)
#define NM_SIMPLE_PAGE_ARROWR_H   (36)
//Page number
#define NM_SIMPLE_PAGE_NUMBER_X    ((NM_SIMPLE_PAGE_CONTW - NM_SIMPLE_PAGE_NUMBER_W) >> 1)
#define NM_SIMPLE_PAGE_NUMBER_Y    (7)
#define NM_SIMPLE_PAGE_NUMBER_W    (250)
#define NM_SIMPLE_PAGE_NUMBER_H    (36)

//Client area
#define NM_SIMPLE_CLIENT_CONTX    (NM_SIMPLE_CLASS_LIST_CONTX + NM_SIMPLE_CLASS_LIST_CONTW + 10)
#define NM_SIMPLE_CLIENT_CONTY    (NM_SIMPLE_PAGE_CONTY + NM_SIMPLE_PAGE_CONTH + 10)
#define NM_SIMPLE_CLIENT_CONTW    (830 + 30)
#define NM_SIMPLE_CLIENT_CONTH    (450)

//Video list
#define NM_SIMPLE_VDO_LIST_X    (0 + 15)
#define NM_SIMPLE_VDO_LIST_Y    (0)
#define NM_SIMPLE_VDO_LIST_W    (NM_SIMPLE_CLIENT_CONTW -30  /*- NM_SIMPLE_MOVLIST_SBAR_W - 6*/)
#define NM_SIMPLE_VDO_LIST_H    (NM_SIMPLE_CLIENT_CONTH)
#define NM_SIMPLE_VDO_LIST_ITEM_V_GAP   (10)

#define NM_SIMPLE_VDO_LIST_ITEM_NUM_ONE_PAGE  (4)

#define NM_SIMPLE_RATED_ICON_Y    (45)
#define NM_SIMPLE_RATED_ICON_W    (20)
#define NM_SIMPLE_RATED_ICON_H    (20)

//Subclass list
#define NM_SIMPLE_SUBCLASS_LIST_PAGE_SIZE (6)

//Option list
#define NM_SIMPLE_OPT_LIST_ITEM_NUM_ONE_PAGE  (6)

//Info picture
#define NM_SIMPLE_INFO_PIC_X      (215)
#define NM_SIMPLE_INFO_PIC_Y      (0)
#define NM_SIMPLE_INFO_PIC_W      (400)
#define NM_SIMPLE_INFO_PIC_H      (280)

//Info detail
#define NM_SIMPLE_INFO_DETAIL_CONTX   (0)
#define NM_SIMPLE_INFO_DETAIL_CONTY   (NM_SIMPLE_INFO_PIC_Y + NM_SIMPLE_INFO_PIC_H + 10)
#define NM_SIMPLE_INFO_DETAIL_CONTW   (NM_SIMPLE_CLIENT_CONTW)
#define NM_SIMPLE_INFO_DETAIL_CONTH   (NM_SIMPLE_CLIENT_CONTH - NM_SIMPLE_INFO_DETAIL_CONTY)

#define NM_SIMPLE_INFO_DETAIL_X       (COMM_BOX1_BORDER)
#define NM_SIMPLE_INFO_DETAIL_Y       (COMM_BOX1_BORDER)
#define NM_SIMPLE_INFO_DETAIL_W       (NM_SIMPLE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - COMM_BOX1_BORDER - NM_SIMPLE_INFO_DETAIL_SBAR_W - 10)
#define NM_SIMPLE_INFO_DETAIL_H       (NM_SIMPLE_INFO_DETAIL_CONTH - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

#define NM_SIMPLE_INFO_DETAIL_MIDX    (0)
#define NM_SIMPLE_INFO_DETAIL_MIDY    (0)
#define NM_SIMPLE_INFO_DETAIL_MIDW    (NM_SIMPLE_INFO_DETAIL_W)
#define NM_SIMPLE_INFO_DETAIL_MIDH    (NM_SIMPLE_INFO_DETAIL_H)

#define NM_SIMPLE_INFO_DETAIL_SBAR_X    (NM_SIMPLE_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - NM_SIMPLE_INFO_DETAIL_SBAR_W)
#define NM_SIMPLE_INFO_DETAIL_SBAR_Y    (NM_SIMPLE_INFO_DETAIL_Y)
#define NM_SIMPLE_INFO_DETAIL_SBAR_W    (6)
#define NM_SIMPLE_INFO_DETAIL_SBAR_H    (NM_SIMPLE_INFO_DETAIL_H)

//Bottom help container
#define NM_SIMPLE_BOTTOM_HELP_X     (NM_SIMPLE_CLASS_LIST_CONTX + NM_SIMPLE_CLASS_LIST_CONTW + 10)
#define NM_SIMPLE_BOTTOM_HELP_Y     (NM_SIMPLE_CLIENT_CONTY + NM_SIMPLE_CLIENT_CONTH + 10)
#define NM_SIMPLE_BOTTOM_HELP_W     (830 + 30)
#define NM_SIMPLE_BOTTOM_HELP_H     (50)

#define MAX_SEARCH_NAME_LEN     (16)

/*!
 * Macro
 */


/*!
 * Type define
 */
typedef enum
{
    IDC_NM_SIMPLE_LIST_CONT = 1,
    IDC_NM_SIMPLE_LEFT_BMP,
    IDC_NM_SIMPLE_CLASS_LIST,
    IDC_NM_SIMPLE_PAGE_CONT,
    IDC_NM_SIMPLE_PAGE_NUM,

    IDC_NM_SIMPLE_CLIENT_CONT,
    IDC_NM_SIMPLE_VDO_LIST,
    IDC_NM_SIMPLE_SUBCLASS_LIST,
    IDC_NM_SIMPLE_OPT_LIST,
    IDC_NM_SIMPLE_INFO_PIC,
    IDC_NM_SIMPLE_INFO_DETAIL_CONT,
    IDC_NM_SIMPLE_INFO_DETAIL,
    IDC_NM_SIMPLE_INFO_DETAIL_SBAR,

    IDC_NM_SIMPLE_BTM_HELP,
} ui_NM_SIMPLE_ctrl_id_t;

typedef enum
{
    VDOLIST_FIELD_ID_LOGO,
    VDOLIST_FIELD_ID_TITLE,
    VDOLIST_FIELD_ID_AUTHOR,
    VDOLIST_FIELD_ID_DURATION,
    VDOLIST_FIELD_ID_RATED_START,
    VDOLIST_FIELD_ID_RATED_END = VDOLIST_FIELD_ID_RATED_START + 5/*NM_SIMPLE_RATED_ICON_NUM*/ - 1,
    VDOLIST_FIELD_ID_VIEW_COUNT
} ui_nm_simple_vdo_list_field_id_t;

typedef enum
{
    NM_SIMPLE_RATED_MASK_EMPTY = 0x00,
    NM_SIMPLE_RATED_MASK_HALF = 0x02,
    NM_SIMPLE_RATED_MASK_FULL = 0x03,
} ui_nm_simple_rated_mask_t;

typedef struct
{
    u16 total_subclass;
    u16 curSubClaListIdx;
    al_netmedia_class_item_t *subclaList;
} ui_subclass_context;

typedef struct
{  
    control_t *hClaList;
    control_t *hPageNum;
    control_t *hClientCont;
    control_t *hSubClaList;
    control_t *hVdoList;
    control_t *hOptList;
    control_t *hHelpCont;
    ui_link_list_t *hHisList;

    NETMEDIA_DP_TYPE nm_type;
    NETMEDIA_UI_ATTR_E attr;
    u16 ext_class;

    al_netmedia_class_item_t *classList;
    u16 total_class;
    u16 curClassListIdx;

    al_netmedia_class_item_t *subclaList;
    u16 total_subclass;
    u16 curSubClaListIdx;

    u16 totalSubClaPage;
    u16 curSubClaPageNum;

    al_netmedia_vdo_item_t *vdoList;
    u32 total_vdo;
    u16 vdo_cnt;
    u16 curVdoListIdx;
    u16 curVdoListRendIdx;

    u32 totalVdoPage;
    u32 curVdoPageNum;
    u32 jumpVdoPageNum;
    u16 vdo_page_size;

    u16 searchKey[MAX_SEARCH_NAME_LEN+1];
} ui_nm_simple_app_t;

#endif
