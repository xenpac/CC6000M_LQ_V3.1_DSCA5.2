/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#ifndef _FASTLOGO_MEM_128M_H_
#define _FASTLOGO_MEM_128M_H_


#define THIS_WHOLE_MEM_END                0xa8000000

#define THIS_AV_INIT_CODE_SIZE            0x5000

#define THIS_AV_STACK_SIZE                (0x100000 - THIS_AV_INIT_CODE_SIZE)

#define THIS_VIDEO_FW_CFG_SIZE            (0x2c00000-0x268000)

#define THIS_VID_DI_CFG_SIZE              0x2d6000

#define THIS_VBI_BUF_SIZE                 0x10000

#define THIS_AUDIO_FW_CFG_SIZE            (0x1a0000-0x20000)

#define THIS_AP_AV_SHARE_MEM_SIZE         (1024) 

#define THIS_VID_SD_WR_BACK_SIZE          0x4bf000

#define THIS_VID_SD_WR_BACK_FIELD_NO      0x6

#define THIS_AP_AV_FREEZE_MEM_SIZE        (1024 * 1024 * 7)

#define THIS_AP_AV_FREEZE_MEM_ADDR        (0xa4a00000 - THIS_AP_AV_FREEZE_MEM_SIZE)

#endif