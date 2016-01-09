/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_EDIT_IPADDRESS_H__
#define __UI_EDIT_IPADDRESS_H__

//container
#define EDIT_IP_CONT_X        (460)
#define EDIT_IP_CONT_Y          (210)
#define EDIT_IP_CONT_W          (450)
#define EDIT_IP_CONT_H          (EDIT_IP_TITLE_H + EDIT_IP_EBOX_H + HELP_ITEM_H \
                                                       + ((EDIT_IP_CONT_V_GAP + EDIT_IP_TITLE_V_GAP) << 1) + EDIT_IP_ITEM_VGAP)
#define EDIT_IP_CONT_H_GAP         DLG_CONTENT_GAP
#define EDIT_IP_CONT_V_GAP         DLG_CONTENT_GAP
//title
#define EDIT_IP_TITLE_X         ((EDIT_IP_CONT_W - EDIT_IP_TITLE_W) >> 1)
#define EDIT_IP_TITLE_Y         10
#define EDIT_IP_TITLE_W         (220)
#define EDIT_IP_TITLE_H         COMM_CTRL_H
#define EDIT_IP_TITLE_V_GAP        (7)

//ip edit box
#define EDIT_IP_EBOX_X          EDIT_IP_CONT_H_GAP
#define EDIT_IP_EBOX_Y          (EDIT_IP_TITLE_Y + EDIT_IP_TITLE_H + EDIT_IP_TITLE_V_GAP)
#define EDIT_IP_EBOX_LW         (160)
#define EDIT_IP_EBOX_RW         (EDIT_IP_CONT_W - (EDIT_IP_CONT_H_GAP + EDIT_IP_EBOX_LW + EDIT_IP_CONT_H_GAP))
#define EDIT_IP_EBOX_H          COMM_CTRL_H

#define EDIT_IP_ITEM_VGAP       (2)

/* rect style */
#define RSI_EDIT_IP_FRM         RSI_POPUP_BG

/*others*/
typedef  RET_CODE (*OBJ_EDIT_IP_CB)(BOOL is_add, void *name);

typedef struct
{
  void *name;
  u16 max_len;
  OBJ_EDIT_IP_CB cb;
} edit_ip_param_t;

RET_CODE open_edit_ipaddress(u32 para1, u32 para2);

#endif
