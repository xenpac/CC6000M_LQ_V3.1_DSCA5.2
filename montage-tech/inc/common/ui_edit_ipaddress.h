/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_EDIT_IPADDRESS_H__
#define __UI_EDIT_IPADDRESS_H__

/*!
  fix me
  */
#define EDIT_IP_CONT_X        (460)
/*!
  fix me
  */
#define EDIT_IP_CONT_Y          (210)
/*!
  fix me
  */
#define EDIT_IP_CONT_W          (450)
/*!
  fix me
  */
#define EDIT_IP_CONT_H          (EDIT_IP_TITLE_H + EDIT_IP_EBOX_H + HELP_ITEM_H \
                                                       + ((EDIT_IP_CONT_V_GAP + EDIT_IP_TITLE_V_GAP) << 1) + EDIT_IP_ITEM_VGAP)
/*!
  fix me
  */
#define EDIT_IP_CONT_H_GAP         DLG_CONTENT_GAP
/*!
  fix me
  */
#define EDIT_IP_CONT_V_GAP         DLG_CONTENT_GAP
//title
/*!
  fix me
  */
#define EDIT_IP_TITLE_X         ((EDIT_IP_CONT_W - EDIT_IP_TITLE_W) >> 1)
/*!
  fix me
  */
#define EDIT_IP_TITLE_Y         10
/*!
  fix me
  */
#define EDIT_IP_TITLE_W         (220)
/*!
  fix me
  */
#define EDIT_IP_TITLE_H         COMM_CTRL_H
/*!
  fix me
  */
#define EDIT_IP_TITLE_V_GAP        (7)

//ip edit box
/*!
  fix me
  */
#define EDIT_IP_EBOX_X          EDIT_IP_CONT_H_GAP
/*!
  fix me
  */
#define EDIT_IP_EBOX_Y          (EDIT_IP_TITLE_Y + EDIT_IP_TITLE_H + EDIT_IP_TITLE_V_GAP)
/*!
  fix me
  */
#define EDIT_IP_EBOX_LW         (160)
/*!
  fix me
  */
#define EDIT_IP_EBOX_RW         (EDIT_IP_CONT_W - (EDIT_IP_CONT_H_GAP + EDIT_IP_EBOX_LW + EDIT_IP_CONT_H_GAP))
/*!
  fix me
  */
#define EDIT_IP_EBOX_H          COMM_CTRL_H
/*!
  fix me
  */
#define EDIT_IP_ITEM_VGAP       (2)

/* rect style */
/*!
  fix me
  */
#define RSI_EDIT_IP_FRM         RSI_COMMAN_BG

/*!
  fix me
  */
typedef  RET_CODE (*OBJ_EDIT_IP_CB)(BOOL is_add, void *name);

/*!
  fix me
  */
typedef struct
{
/*!
  fix me
  */
  void *name;
/*!
  fix me
  */
  u16 max_len;
/*!
  fix me
  */
  OBJ_EDIT_IP_CB cb;
} edit_ip_param_t;

/*!
  fix me
  */
RET_CODE open_edit_ipaddress(u32 para1, u32 para2);

#endif
