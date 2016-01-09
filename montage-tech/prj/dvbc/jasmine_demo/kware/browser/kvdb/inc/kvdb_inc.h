/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#ifndef __IPANEL_INC_H__
#define __IPANEL_INC_H__

#ifdef __cplusplus
extern "C" {
#endif
/*-------------------------------------------------------------------------------
	Include Files
-------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_jazz.h"
#include "stdio.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
#include "mtos_fifo.h"

#include "drv_dev.h"
#include "dmx.h"
#include "charsto.h"
#include "uio.h"
#include "hal_uart.h"

#include "lib_rect.h"
#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "hal_dma.h"
#include "mtos_sem.h"

#include "nim.h"
#include "smc_op.h"
#include "ipanel_dbg.h"
#include "ipanel_def.h"

#include "ipanel_porting_event.h"
#include "ipanel_typedef.h"
#include "ipanel_base.h"
#include "ipanel_os.h"
#include "ipanel_audio.h"
#include "ipanel_video.h"
#include "ipanel_adec.h"
#include "ipanel_aout.h"
#include "ipanel_avm.h"
#include "ipanel_cursor.h"
#include "ipanel_demux.h"
#include "ipanel_dsm.h"
#include "ipanel_file.h"
#include "ipanel_fpanel.h"
#include "ipanel_graphics.h"
#include "ipanel_media.h"
#include "ipanel_mic.h"
#include "ipanel_network.h"
#include "ipanel_nvram.h"
#include "ipanel_smc.h"
#include "ipanel_socket.h"
#include "ipanel_sound.h"
#include "ipanel_sysapp.h"
#include "ipanel_tuner.h"
#include "ipanel_upgrade.h"
#include "ipanel_vdec.h"
#include "ipanel_vout.h"
#include "ipaneldtv_api.h"
#include "ipanel_app_downloader.h"


extern IPANEL_ATT g_mIpanel;	// iPanel浏览器运行时主要参数


int __iPanelAudioGetMute( void );
int __iPanelAudioSetMute( u32 bMute );
int __iPanelAudioGetVolume( void );
int __iPanelAudioSetVolume( int vol );
int iPanelAudioInit( void );
int iPanelAudioExit( void );
int __iPanelAudioInitGetVolume( void );

int iPanelNvramInit(void);
int iPanelNvramExit(void);

int iPanelDemuxInit(void);
int iPanelDemuxExit(void);

int iPanelSocketInit(void);
int iPanelSocketExit(void);
int iPanelRefreshDNS(void);

void iPanelTaskInit(void);

int iPanel_CA_StartService(u16 srvID);
int iPanel_CA_StopService(u16 srvID);

void IpanelInitServiceStopFlag(void);
int  Ipaneldvb_SearchService_ByServiceId ( u16 srv_serviceid );


#ifdef __cplusplus
}
#endif

#endif

