/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_AUTO_FEED_H__
#define __UI_AUTO_FEED_H__

RET_CODE open_super_osd(u32 para1, u32 para2);
void close_super_osd(void);
RET_CODE on_ca_super_osd_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2);
#endif



