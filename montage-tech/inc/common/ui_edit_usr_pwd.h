/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_EDIT_USR_PWD_H__
#define __UI_EDIT_USR_PWD_H__

/* coordinate */
#define EDIT_USR_PWD_CONT_X          ((SCREEN_WIDTH - EDIT_USR_PWD_CONT_W) / 2)
#define EDIT_USR_PWD_CONT_Y           ((SCREEN_HEIGHT- EDIT_USR_PWD_CONT_H) / 2)
#define EDIT_USR_PWD_CONT_W          440
#define EDIT_USR_PWD_CONT_H          (2*EDIT_USR_PWD_TOP_BTM_VGAP+USR_PWD_ITEM_CNT*EDIT_USR_PWD_ITEM_VGAP+(USR_PWD_ITEM_CNT+1)*EDIT_USR_PWD_EBOX_H)

#define EDIT_USR_PWD_EBOX_X 20
#define EDIT_USR_PWD_EBOX_Y 20
#define EDIT_USR_PWD_EBOX_LW 120
#define EDIT_USR_PWD_EBOX_RW (EDIT_USR_PWD_CONT_W-2*EDIT_USR_PWD_EBOX_X-EDIT_USR_PWD_EBOX_LW)
#define EDIT_USR_PWD_EBOX_H COMM_CTRL_H

#define EDIT_USR_PWD_TOP_BTM_VGAP 10
#define EDIT_USR_PWD_ITEM_VGAP 10
/* rect style */

/* font style */
/*fstyle*/
#define FSI_EDIT_USR_PWD_SH          FSI_WHITE       //font styel of single satellite information edit
#define FSI_EDIT_USR_PWD_HL          FSI_BLACK

/*others*/
#define USR_PWD_ITEM_CNT 2
RET_CODE open_edit_usr_pwd(u32 para1, u32 para2);

RET_CODE ui_get_ip(char *ipaddr);

RET_CODE ui_get_ip_path_mount(u16 **pp_ip_path);

RET_CODE ui_get_ip_path(u16 *ip_path);



#endif
