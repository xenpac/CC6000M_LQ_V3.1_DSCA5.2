/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_NM_CLASSIC_H__
#define __UI_NM_CLASSIC_H__

/*!
 * Const value
 */
//Class list container
#define NM_CLASSIC_CLASS_LIST_CONTX    (120 - 30)
#define NM_CLASSIC_CLASS_LIST_CONTY    (100)
#define NM_CLASSIC_CLASS_LIST_CONTW    (200 + 30)
#define NM_CLASSIC_CLASS_LIST_CONTH    (570)

//Logo
#define NM_CLASSIC_LOGO_X    (0)
#define NM_CLASSIC_LOGO_Y    (0)
#define NM_CLASSIC_LOGO_W    (NM_CLASSIC_CLASS_LIST_CONTW)
#define NM_CLASSIC_LOGO_H    (90)

//Class list
#define NM_CLASSIC_CLASS_LIST_X    (10)
#define NM_CLASSIC_CLASS_LIST_Y    (90)
#define NM_CLASSIC_CLASS_LIST_W    (180 + 30)
#define NM_CLASSIC_CLASS_LIST_H    (400)

#define NM_CLASSIC_SEARCH_HELP_X     (NM_CLASSIC_CLASS_LIST_X)
#define NM_CLASSIC_SEARCH_HELP_Y     (NM_CLASSIC_CLASS_LIST_CONTH - NM_CLASSIC_SEARCH_HELP_H)
#define NM_CLASSIC_SEARCH_HELP_W     (NM_CLASSIC_CLASS_LIST_W)
#define NM_CLASSIC_SEARCH_HELP_H     (50)


#define NM_CLASSIC_CLASS_LIST_MIDL   (0)
#define NM_CLASSIC_CLASS_LIST_MIDT   (0)
#define NM_CLASSIC_CLASS_LIST_MIDW   (NM_CLASSIC_CLASS_LIST_W - NM_CLASSIC_CLASS_LIST_MIDL - NM_CLASSIC_CLASS_LIST_MIDL)
#define NM_CLASSIC_CLASS_LIST_MIDH   (NM_CLASSIC_CLASS_LIST_H - NM_CLASSIC_CLASS_LIST_MIDT - NM_CLASSIC_CLASS_LIST_MIDT)

#define NM_CLASSIC_CLASS_LIST_ICON_VGAP    (4)
#define NM_CLASSIC_CLASS_LIST_ITEM_NUM_ONE_PAGE  (9)
#define NM_CLASSIC_CLASS_LIST_ITEM_COUNT   (8)

//Page container
#define NM_CLASSIC_PAGE_CONTX    (NM_CLASSIC_CLASS_LIST_CONTX + NM_CLASSIC_CLASS_LIST_CONTW + 10)
#define NM_CLASSIC_PAGE_CONTY    (100)
#define NM_CLASSIC_PAGE_CONTW    (860)
#define NM_CLASSIC_PAGE_CONTH    (50)

//Page arrow left
#define NM_CLASSIC_PAGE_ARROWL_X   (150)
#define NM_CLASSIC_PAGE_ARROWL_Y   (7)
#define NM_CLASSIC_PAGE_ARROWL_W   (36)
#define NM_CLASSIC_PAGE_ARROWL_H   (36)
//Page arrow right
#define NM_CLASSIC_PAGE_ARROWR_X   (NM_CLASSIC_PAGE_CONTW - NM_CLASSIC_PAGE_ARROWL_X - NM_CLASSIC_PAGE_ARROWR_W)
#define NM_CLASSIC_PAGE_ARROWR_Y   (7)
#define NM_CLASSIC_PAGE_ARROWR_W   (36)
#define NM_CLASSIC_PAGE_ARROWR_H   (36)
//Page number
#define NM_CLASSIC_PAGE_NUMBER_X    ((NM_CLASSIC_PAGE_CONTW - NM_CLASSIC_PAGE_NUMBER_W) >> 1)
#define NM_CLASSIC_PAGE_NUMBER_Y    (7)
#define NM_CLASSIC_PAGE_NUMBER_W    (150)
#define NM_CLASSIC_PAGE_NUMBER_H    (36)

//Client area
#define NM_CLASSIC_CLIENT_CONTX    (NM_CLASSIC_CLASS_LIST_CONTX + NM_CLASSIC_CLASS_LIST_CONTW + 10)
#define NM_CLASSIC_CLIENT_CONTY    (NM_CLASSIC_PAGE_CONTY + NM_CLASSIC_PAGE_CONTH + 10)
#define NM_CLASSIC_CLIENT_CONTW    (860)
#define NM_CLASSIC_CLIENT_CONTH    (450)

//video list
#define NM_CLASSIC_VDO_LIST_CONTX    (0 +15)//(NM_CLASSIC_LIST_CONTX + NM_CLASSIC_LIST_CONTW)
#define NM_CLASSIC_VDO_LIST_CONTY    0//(NM_CLASSIC_PAGE_CONTY + NM_CLASSIC_PAGE_CONTH)
#define NM_CLASSIC_VDO_LIST_CONTW    (830)//(COMM_BG_W - NM_CLASSIC_VDO_LIST_CONTX - NM_CLASSIC_LIST_CONTX)
#define NM_CLASSIC_VDO_LIST_CONTH    450

#define NM_CLASSIC_VDO_LIST_ITEM_X    15//0
#define NM_CLASSIC_VDO_LIST_ITEM_Y    0
#define NM_CLASSIC_VDO_LIST_ITEM_W    150//195//200
#define NM_CLASSIC_VDO_LIST_ITEM_H    220

#define NM_CLASSIC_VDO_LIST_ITEM_HGAP   12
#define NM_CLASSIC_VDO_LIST_ITEM_VGAP   10

#define NM_CLASSIC_VDO_PIC_X    5// 10
#define NM_CLASSIC_VDO_PIC_Y    5//  10
#define NM_CLASSIC_VDO_PIC_W    140
#define NM_CLASSIC_VDO_PIC_H    170

#define NM_CLASSIC_VDO_NAME_X   (NM_CLASSIC_VDO_PIC_X)
#define NM_CLASSIC_VDO_NAME_Y   (NM_CLASSIC_VDO_PIC_Y + NM_CLASSIC_VDO_PIC_H + 5)
#define NM_CLASSIC_VDO_NAME_W   (NM_CLASSIC_VDO_PIC_W)
#define NM_CLASSIC_VDO_NAME_H   (35)

#define NM_CLASSIC_VDO_NUM_ONE_PAGE  10
#define NM_CLASSIC_VDO_ITEM_ROW     (2)
#define NM_CLASSIC_VDO_ITEM_COL     (5)

//Option list
#define NM_CLASSIC_OPT_LIST_X    (0)
#define NM_CLASSIC_OPT_LIST_Y    (0)
#define NM_CLASSIC_OPT_LIST_W    (NM_CLASSIC_CLIENT_CONTW /*- NM_SIMPLE_MOVLIST_SBAR_W - 6*/)
#define NM_CLASSIC_OPT_LIST_H    (NM_CLASSIC_CLIENT_CONTH)
#define NM_CLASSIC_OPT_LIST_ITEM_V_GAP   (10)
#define NM_CLASSIC_OPT_LIST_ITEM_NUM_ONE_PAGE  (6)

//Subclass list
#define NM_CLASSIC_SUBCLASS_LIST_ITEM_NUM_ONE_PAGE (6)

//Info picture
#define NM_CLASSIC_INFO_PIC_X      (215)
#define NM_CLASSIC_INFO_PIC_Y      (0)
#define NM_CLASSIC_INFO_PIC_W      (400)
#define NM_CLASSIC_INFO_PIC_H      (280)

//Info detail
#define NM_CLASSIC_INFO_DETAIL_CONTX   (0)
#define NM_CLASSIC_INFO_DETAIL_CONTY   (NM_CLASSIC_INFO_PIC_Y + NM_CLASSIC_INFO_PIC_H + 10)
#define NM_CLASSIC_INFO_DETAIL_CONTW   (NM_CLASSIC_CLIENT_CONTW)
#define NM_CLASSIC_INFO_DETAIL_CONTH   (NM_CLASSIC_CLIENT_CONTH - NM_CLASSIC_INFO_DETAIL_CONTY)

#define NM_CLASSIC_INFO_DETAIL_X       (COMM_BOX1_BORDER)
#define NM_CLASSIC_INFO_DETAIL_Y       (COMM_BOX1_BORDER)
#define NM_CLASSIC_INFO_DETAIL_W       (NM_CLASSIC_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - COMM_BOX1_BORDER - NM_CLASSIC_INFO_DETAIL_SBAR_W - 10)
#define NM_CLASSIC_INFO_DETAIL_H       (NM_CLASSIC_INFO_DETAIL_CONTH - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

#define NM_CLASSIC_INFO_DETAIL_MIDX    (0)
#define NM_CLASSIC_INFO_DETAIL_MIDY    (0)
#define NM_CLASSIC_INFO_DETAIL_MIDW    (NM_CLASSIC_INFO_DETAIL_W)
#define NM_CLASSIC_INFO_DETAIL_MIDH    (NM_CLASSIC_INFO_DETAIL_H)

#define NM_CLASSIC_INFO_DETAIL_SBAR_X    (NM_CLASSIC_INFO_DETAIL_CONTW - COMM_BOX1_BORDER - NM_CLASSIC_INFO_DETAIL_SBAR_W)
#define NM_CLASSIC_INFO_DETAIL_SBAR_Y    (NM_CLASSIC_INFO_DETAIL_Y)
#define NM_CLASSIC_INFO_DETAIL_SBAR_W    (6)
#define NM_CLASSIC_INFO_DETAIL_SBAR_H    (NM_CLASSIC_INFO_DETAIL_H)

//Bottom help container
#define NM_CLASSIC_BOTTOM_HELP_X     (NM_CLASSIC_CLASS_LIST_CONTX + NM_CLASSIC_CLASS_LIST_CONTW + 10)
#define NM_CLASSIC_BOTTOM_HELP_Y     (NM_CLASSIC_CLIENT_CONTY + NM_CLASSIC_CLIENT_CONTH + 10)
#define NM_CLASSIC_BOTTOM_HELP_W     (860)
#define NM_CLASSIC_BOTTOM_HELP_H     (50)

/*!
 * Macro
 */
#define MAX_SEARCH_NAME_LEN     (16)

/*!
 * Type define
 */
typedef enum
{
    IDC_NM_CLASSIC_LIST_CONT = 1,
    IDC_NM_CLASSIC_LEFT_BMP,
    IDC_NM_CLASSIC_CLASS_LIST,
    IDC_NM_CLASSIC_PAGE_CONT,
    IDC_NM_CLASSIC_PAGE_NUM,

    IDC_NM_CLASSIC_CLIENT_CONT,
    IDC_NM_CLASSIC_VDO_LIST_CONT,
    IDC_NM_CLASSIC_VDO_ITEM_START,
    IDC_NM_CLASSIC_VDO_ITEM_END = IDC_NM_CLASSIC_VDO_ITEM_START + NM_CLASSIC_VDO_NUM_ONE_PAGE - 1,
    IDC_NM_CLASSIC_VDO_ITEM_PIC,
    IDC_NM_CLASSIC_VDO_ITEM_NAME,

    IDC_NM_CLASSIC_SUBCLASS_LIST,
    IDC_NM_CLASSIC_OPT_LIST,
    IDC_NM_CLASSIC_INFO_PIC,
    IDC_NM_CLASSIC_INFO_DETAIL_CONT,
    IDC_NM_CLASSIC_INFO_DETAIL,
    IDC_NM_CLASSIC_INFO_DETAIL_SBAR,

    IDC_NM_CLASSIC_BTM_HELP,
} ui_NM_CLASSIC_ctrl_id_t;

typedef struct
{ 
    u16 name;
} nm_classic_class_list_item;

typedef struct
{ 
    u16 name;
} nm_classic_class_list_ext_item;

//Subclass list
#define NM_CLASSIC_SUBCLASS_LIST_PAGE_SIZE (6)

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

    al_netmedia_play_url_item_t *urlList;
    u16 url_cnt;
    u16 curUrlListIdx;

    u32 totalVdoPage;
    u32 curVdoPageNum;
    u32 jumpVdoPageNum;
    u8 totalCatePage;
    u8 curCatePageNum;
    u16 vdo_page_size;
    u16 g_searchKey[MAX_SEARCH_NAME_LEN+1];
} ui_nm_classic_app_t;

#endif
