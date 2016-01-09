/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#ifndef _FASTLOGO_MEM_64M_SD_H_
#define _FASTLOGO_MEM_64M_SD_H_


#define THIS_WHOLE_MEM_END                0xa4000000

#define THIS_AV_INIT_CODE_SIZE            0x0000

#define THIS_AV_STACK_SIZE                0x20000

#define THIS_VIDEO_FW_CFG_SIZE            (0x16c4000-0x268000)

#define THIS_VID_DI_CFG_SIZE              0x110000

#define THIS_VBI_BUF_SIZE                 0x10000

#define THIS_AUDIO_FW_CFG_SIZE            0x180000

#define THIS_AP_AV_SHARE_MEM_SIZE         (1024) 

#define THIS_VID_SD_WR_BACK_SIZE          0x4bf000

#define THIS_VID_SD_WR_BACK_FIELD_NO      0x6

#define THIS_AP_AV_FREEZE_MEM_SIZE        (0)

#define THIS_AP_AV_FREEZE_MEM_ADDR        (0)

#endif