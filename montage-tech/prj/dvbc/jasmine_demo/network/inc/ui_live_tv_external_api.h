/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#ifndef __UI_IPTV_EXTER_H__
#define __UI_IPTV_EXTER_H__

//#include "ui_common.h"

enum iptv_comm_msg
{
    MSG_OPEN_IPTV_MENU =  0x4000 + 1300, 
    MSG_DESKTOP_NET_CONNTECED, 
    MSG_DESKTOP_NET_UNCONNTECED,
    MSG_DESKTOP_JUMP_TO_NEXT_STATUS,
    MSG_LIVETV_UPDATE_TIMING,
    MSG_OPEN_IPTV_CUS_MENU,//entry customer grp
};


//open iptv state machine
void Iptv_Open_State();

//process state
RET_CODE Iptv_On_State_Process_Event(u32 event, u32 para1, u32 para2);
RET_CODE Iptv_On_State_Process_Msg(control_t *ctrl, u16 msg, u32 para1, u32 para2);

//iptv event key map
void Iptv_Register_Evtmap();

//iptv set desktop
void Iptv_Set_Desktop();

//iptv get pg data.
void Iptv_Get_Pg_Info(u8 type,BOOL is_from_desktop);

BOOL Iptv_Find_Cus_Url_Files(void);

void Iptv_Start_Load_Cus_Urls(void);

void Iptv_Cancel_Load_cus_Pg(void);

RET_CODE open_iptv(u32 para1, u32 para2);

#endif
