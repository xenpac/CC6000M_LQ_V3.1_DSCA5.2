/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/


#ifndef __SYS_REGS_JAZZ_H__
#define __SYS_REGS_JAZZ_H__

 /*!
  CPU System Clock Definition : CPU CLOCK: in MHz 
  */
 #define SYS_AHB_CLOCK_M        (mtos_cpu_freq_get() / 1000000)

/*!
  comments
  */
#define SYS_AHB_CLOCK           ((SYS_AHB_CLOCK_M) * 1000000)

/*!
  comments
  */
#define SYS_APB_CLOCK_M         ((SYS_AHB_CLOCK_M) / 2)

/*!
  comments
  */
#define SYS_APB_CLOCK           ((SYS_APB_CLOCK_M) * 1000000)

/*!
  comments
  */
#define SYS_BASE_CLOCK_M    24

/*!
  comments
  */
#define SYS_BASE_CLOCK      ((SYS_BASE_CLOCK_M) * 1000000)

/*!
  Defination of UART Register
  */
#define  UART0_ADDR_BASE    0x42000000

/*!
  comments
  */
#define  UART1_ADDR_BASE    0x4a000000

/*!
  Definition of IRQ register
  */
#define    IRQ_BASE_ADDR                0x60000000

/*!
  Defination of TIMER Register
  */
#define  TIMER_ADDR_BASE    0x60000000

/*!
  Definition of watchdog register
  */
#define  WATCHDOG_ADDR_BASE           0x60000000

/*!
  Definition of RTC register
  */
#define  RTC_ADDR_BASE  0x60030000

/*!
  Defination of CPM Register
  */
#define CPM_PHYCLK_CFG 0x70000000

/*!
  0x7000008c
  */
#define CPM_ANA_AOREG  ((CPM_PHYCLK_CFG) + 0x8c)

/*!
  Defination of PIN_MUX Register
  */
#define PIN_MUX_SELECT 0x6f880000



/*!
  Definition of INFO register
  */
#define    INFO_BASE_ADDR          0x6f890000

/*!
  0x6f890000
  */
#define    R_INFO_0_MASK_REG                 ((INFO_BASE_ADDR) + 0x00)
/*!
  0x6f890004
  */
#define    R_INFO_0_GLOBAL_REG               ((INFO_BASE_ADDR) + 0x04)
/*!
  0x6f890008
  */
#define    R_INFO_0_GINT_STAT_CLR_REG  ((INFO_BASE_ADDR) + 0x08)
/*!
  0x6f89000c
  */
#define    R_INFO_0_GINT_STAT_REG    ((INFO_BASE_ADDR) + 0x0c)

/*!
  0x6f890010
  */
#define    R_INFO_1_MASK_REG                 ((INFO_BASE_ADDR) + 0x10)
/*!
  0x6f890014
  */
#define    R_INFO_1_GLOBAL_REG               ((INFO_BASE_ADDR) + 0x14)
/*!
  0x6f890018
  */
#define    R_INFO_1_GINT_STAT_CLR_REG  ((INFO_BASE_ADDR) + 0x18)
/*!
  0x6f89001c
  */
#define    R_INFO_1_GINT_STAT_REG        ((INFO_BASE_ADDR) + 0x1c)

/*!
  0x6f890020
  */
#define    R_INFO_2_MASK_REG                 ((INFO_BASE_ADDR) + 0x20)
/*!
  0x6f890024
  */
#define    R_INFO_2_GLOBAL_REG               ((INFO_BASE_ADDR) + 0x24)
/*!
  0x6f890028
  */
#define    R_INFO_2_GINT_STAT_CLR_REG   ((INFO_BASE_ADDR) + 0x28)
/*!
  0x6f89002c
  */
#define    R_INFO_2_GINT_STAT_REG    ((INFO_BASE_ADDR) + 0x2c)


/*!
  Definition of DISC registers
  */
#define DISC_VID_ZOOM_M0_BASE_REG      0x50000000
/*!
  comments
  */
#define DISC_VID_ZOOM_M1_BASE_REG      0x50300000

/*!
  全局设置
  */
#define R_DISC_VID_GLB_CTRL_CMD_REG      DISC_VID_ZOOM_M0_BASE_REG
/*!
  comments
  */
#define R_DISC_VID_GLB_FIELD_DISP_REG      0x50000004
/*!
  0x60050010
  */
#define R_DISC_VID_GLB_WINDOWCUT_REG    0x50100010
/*!
  0x60060008
  */
#define R_DISC_VID_GLB_PLAN_ALPHA_REG    0x50200008

/*!
  模式0缩放控制 0x50000004
  */
#define R_DISC_VID_ZOOM_M0_DISP_REG          ((DISC_VID_ZOOM_M0_BASE_REG) + 0x04)
/*!
  0x50000008
  */
#define R_DISC_VID_ZOOM_M0_RATIO_REG      ((DISC_VID_ZOOM_M0_BASE_REG) + 0x08)
/*!
  0x5000000c
  */
#define R_DISC_VID_ZOOM_M0_SRCLINE_REG    ((DISC_VID_ZOOM_M0_BASE_REG) + 0x0c)
/*!
  0x50000010
  */
#define R_DISC_VID_ZOOM_M0_SRCPIXEL_REG    ((DISC_VID_ZOOM_M0_BASE_REG) + 0x10)
/*!
  0x50000014
  */
#define R_DISC_VID_ZOOM_M0_POS_X_REG      ((DISC_VID_ZOOM_M0_BASE_REG) + 0x14)
/*!
  0x50000018
  */
#define R_DISC_VID_ZOOM_M0_POS_Y_REG      ((DISC_VID_ZOOM_M0_BASE_REG) + 0x18)

/*!
  模式1缩放控制 0x50300000
  */
#define R_DISC_VID_ZOOM_M1_CTRL_REG      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x00)
/*!
  0x50300004
  */
#define R_DISC_VID_ZOOM_M1_RENEW_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x04)
/*!
  0x50300008
  */
#define R_DISC_VID_ZOOM_M1_POSX_REG      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x08)
/*!
  0x5030000c
  */
#define R_DISC_VID_ZOOM_M1_POSY_REG       ((DISC_VID_ZOOM_M1_BASE_REG) + 0x0c)
/*!
  0x50300010
  */
#define R_DISC_VID_ZOOM_M1_HF_REG       ((DISC_VID_ZOOM_M1_BASE_REG) + 0x10)
/*!
  0x50300018
  */
#define R_DISC_VID_ZOOM_M1_VF_REG      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x18)
/*!
  0x50300020
  */
#define R_DISC_VID_ZOOM_M1_HFRAM0_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x20)
/*!
  0x50300024
  */
#define R_DISC_VID_ZOOM_M1_HFRAM1_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x24)
/*!
  0x50300028
  */
#define R_DISC_VID_ZOOM_M1_HFRAM2_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x28)
/*!
  0x5030002c
  */
#define R_DISC_VID_ZOOM_M1_HFRAM3_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x2c)
/*!
  0x50300030
  */
#define R_DISC_VID_ZOOM_M1_HFRAM4_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x30)
/*!
  0x50300034
  */
#define R_DISC_VID_ZOOM_M1_HFRAM5_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x34)
/*!
  0x50300048
  */
#define R_DISC_VID_ZOOM_M1_VFRAM0_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x48)
/*!
  0x5030004c
  */
#define R_DISC_VID_ZOOM_M1_VFRAM1_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x4c)
/*!
  0x50300058
  */
#define R_DISC_VID_ZOOM_M1_NTSC_POSY_REG  ((DISC_VID_ZOOM_M1_BASE_REG) + 0x58)
/*!
  0x5030005c
  */
#define R_DISC_VID_ZOOM_M1_NTSC_VF_REG    ((DISC_VID_ZOOM_M1_BASE_REG) + 0x5c)

/*!
  全屏显示控制
  */
#define R_DISC_VID_FULLSCREEN_HF_REG      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x14)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_VF_REG      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x1c)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_HFRAM0      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x38)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_HFRAM1      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x3c)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_HFRAM2      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x40)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_HFRAM3      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x44)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_VFRAM0      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x50)
/*!
  comments
  */
#define R_DISC_VID_FULLSCREEN_VFRAM1      ((DISC_VID_ZOOM_M1_BASE_REG) + 0x54)

/*!
  comments
  */
#define R_DISC_VID_ADDR_SET0_REG        0x6f400000
/*!
  comments
  */
#define R_DISC_VID_ADDR_SET1_REG        0x6f400004
/*!
  comments
  */
#define R_DISC_VID_LIMIT_VIDBUF_REG      0x6f400068
/*!
  comments
  */
#define R_DISC_VID_LIMIT_ESBUF_REG      0x6f400060  
/*!
  comments
  */
#define R_DISC_VID_VENC_GAIN_REG        0x6f840034  
/*!
  comments
  */
#define R_DISC_VID_P2N2P_SET_REG        0x6f870000  
/*!
  comments
  */
#define R_DISC_VID_GLOBALINFO_REG        0x6f890004
/*!
  comments
  */
#define R_DISC_VID_STATUS_FRAMENUM_REG    0x6f910000
/*!
  comments
  */
#define R_DISC_VID_STATUS_ESNUM_REG      0x6f900004
/*!
  comments
  */
#define R_DISC_VID_STATUS_BOLCKNUM_REG    0x6f910004
/*!
  comments
  */
#define R_DISC_VID_RESE_CTRL              0x6f800010 
/*!
  comments
  */
#define R_DISC_VID_VDEC_MODE_REG        0x6f840000
/*!
  comments
  */
#define R_DISC_VID_TS_PARSE_SET_REG      0x6f8a0000  
/*!
  comments
  */
#define R_DISC_VID_ES_PLAYER_REG        0x6f8a0004
/*!
  comments
  */
#define R_DISC_VID_VES_CTRL_ADDR_REG      0x6f400084  
/*!
  comments
  */
#define R_DISC_VID_AUTO_CHECK_REG        0x6f830014  
/*!
  comments
  */
#define R_DISC_VID_VID_VDEC_POWEROFF_REG  0x6f8f0004  
/*!
  comments
  */
#define R_DISC_VID_VID_CHCHG_DISPLAY_REG    0x6f810004
/*!
  comments
  */
#define R_DISC_VID_VDEC_HIGH_PICTURE_REG    0x6f810014

/*!
  comments
  */
#define R_DISC_VID_STATUS_PCR_REG        0x6f93001C  
/*!
  comments
  */
#define R_DISC_VID_STATUS_VPTS_REG      0x6f930020
/*!
  comments
  */
#define R_DISC_VID_STATUS_VDTS_REG      0x6f930024
/*!
  comments
  */
#define R_DISC_VID_STATUS_FIRST_VPTS_REG    0x6f93003C
/*!
  comments
  */
#define R_DISC_VID_STATUS_FIRST_VDTS_REG    0x6f930040

/*!
  comments
  */
#define M_DISC_VID_MASK_ZOOM_AUTOH_CANCEL    0x00000001

/*!
  comments
  */
#define M_DISC_VID_MASK_ZOOM_AUTOSWITCH  0x00000001



/*!
  comments
  */
#define M_DISC_VID_MASK_VIDEO_DISPLAY    0x00000001
/*!
  comments
  */
#define M_DISC_VID_MASK_ZOOM_H        0x00000010
/*!
  comments
  */
#define M_DISC_VID_MASK_ZOOM_V        0x00000100
/*!
  comments
  */
#define M_DISC_VID_VF_IN_AUTO           0x00000000
/*!
  comments
  */
#define M_DISC_VID_VF_IN_FRAME         0x00003000
/*!
  comments
  */
#define M_DISC_VID_VF_IN_FIELD        0x00002000
/*!
  comments
  */
#define M_DISC_VID_CHROM_FILTER_CLOSE   0x00010000
/*!
  comments
  */
#define M_DISC_VID_MASK_CHROM_EDES      0x00100000
/*!
  comments
  */
#define M_DISC_VID_MASK_AVESBUF_READ    0x02000000
/*!
  comments
  */
#define M_DISC_VID_ES_BUFFER_RESET      0x20000000

/*!
  comments
  */
#define M_DISC_VID_MASK_HTABLEBEGIN        0x00100000
/*!
  comments
  */
#define M_DISC_VID_MASK_HTABLEBEGIN_FULLSCN  0x01000000
/*!
  comments
  */
#define M_DISC_VID_MASK_VTABLEBEGIN      0x00000001
/*!
  comments
  */
#define M_DISC_VID_MASK_VTABLEBEGIN_FULLSCN  0x00000100
/*!
  comments
  */
#define M_DISC_VID_MASK_NTSC_VTABLEBEGIN  0x00000010

/*!
  change program display
  */
#define M_DISC_VID_DISPLAY_BLACK_MODE                0x00000003
/*!
  comments
  */
#define M_DISC_VID_DISPLAY_ONE_FIELD_DECODE_STOP     0x00000100
/*!
  comments
  */
#define M_DISC_VID_DISPLAY_TWO_FIELD_CHG_PROG       0x00000200

/*!
  HD program display
  */
#define M_DISC_VDEC_DISPLAY_BLACK_MODE      0x00010000
/*!
  comments
  */
#define M_DISC_VDEC_HD_CHECK_ENABLE        0x00008000
/*!
  comments
  */
#define M_DISC_VDEC_HD_CHECK_RESET        0x00020000

/*!
  pal 制 n 制 转 换 模 式 定 义
  */
#define M_DISC_VID_MODE_AUTO        0x0
/*!
  comments
  */
#define M_DISC_VID_MODE_PAL        0x2
/*!
  comments
  */
#define M_DISC_VID_MODE_NTSC        0x3

/*!
  videoout type
  */
#define M_DISC_VIDEOOUT_TYPE_SVID_CVBS  0x0
/*!
  comments
  */
#define M_DISC_VIDEOOUT_TYPE_CVBS_YUV  0x1
/*!
  comments
  */
#define M_DISC_VIDEOOUT_TYPE_CVBS_RGB  0x2
/*!
  comments
  */
#define M_DISC_VIDEOOUT_TYPE_2CVBS_SVID  0x3
/*!
  Definition of Address manager registers
  */
  
/*!
  TS buffer register define
  */
#define  ES_BUF_ADDR_BASE          0x6f400000
                                            
/*!
  comments
  */
#define  R_ES_BUF_AV_ADDR_REG          ((ES_BUF_ADDR_BASE) + 0x0) 
/*!
  comments
  */
#define  R_ES_BUF_VBI_ADDR_REG        ((ES_BUF_ADDR_BASE) + 0x4) 
/*!
  comments
  */
#define  R_ES_BUF_VIDGUARD_REG        ((ES_BUF_ADDR_BASE) + 0x8) 
/*!
  comments
  */
#define  R_ES_BUF_AUDGUARD0_REG        ((ES_BUF_ADDR_BASE) + 0xc) 
/*!
  comments
  */
#define  R_ES_BUF_AUDGUARD1_REG        ((ES_BUF_ADDR_BASE) + 0x10)
/*!
  comments
  */
#define  R_ES_BUF_VBIGUARD_REG        ((ES_BUF_ADDR_BASE) + 0x14)
/*!
  comments
  */
#define  R_ES_BUF_BLOCKSIZE_REG        ((ES_BUF_ADDR_BASE) + 0x18)

/*!
  comments
  */
#define ES_BUF_STRIDE            768

/*!
  VIDEO buffer register define
  */
#define  VIDEO_BUF_ADDR_BASE                0x6f400020
                                             
/*!
  comments
  */
#define  R_VIDEO_BUF_ADDR0_REG                 ((VIDEO_BUF_ADDR_BASE) + 0x0)
/*!
  comments
  */
#define  R_VIDEO_BUF_ADDR1_REG                  ((VIDEO_BUF_ADDR_BASE) + 0x4)
/*!
  comments
  */
#define  R_VIDEO_BUF_ADDR2_REG                  ((VIDEO_BUF_ADDR_BASE) + 0x8)
/*!
  comments
  */
#define  R_VIDEO_BUF_ADDR3_REG                  ((VIDEO_BUF_ADDR_BASE) + 0xc)

/*!
  comments
  */
#define  VIDEO_BUF_MASK                0xffff

/*!
  comments
  */
#define  R_VIDEO_BUF_START_DEF32M        0x00007280
/*!
  comments
  */
#define  R_VIDEO_BUF_START_DEF16M                 0x00003280
/*!
  comments
  */
#define  R_VIDEO_BUF_START_DEF8M                 0x00001280



/*!
  OSD buffer register define
  */
#define  OSD_BUF_ADDR_BASE                  0x6f400040
                                                                                          
/*!
  comments
  */
#define  R_STP_BUF_ADDR0_REG              ((OSD_BUF_ADDR_BASE) + 0x0) 
/*!
  comments
  */
#define  R_STP_BUF_ADDR1_REG              ((OSD_BUF_ADDR_BASE) + 0x4) 
/*!
  comments
  */
#define  R_OSD_BUF_ADDR0_REG              ((OSD_BUF_ADDR_BASE) + 0x8) 
/*!
  comments
  */
#define  R_SUBP_BUF_ADDR0_REG            ((OSD_BUF_ADDR_BASE) + 0x10) 

/*!
  comments
  */
#define    R_INFO_1_MASK_REG                 ((INFO_BASE_ADDR) + 0x10)
/*!
  comments
  */
#define    R_INFO_1_GLOBAL_REG               ((INFO_BASE_ADDR) + 0x14)
/*!
  comments
  */
#define    R_INFO_1_GINT_STAT_CLR_REG  ((INFO_BASE_ADDR) + 0x18) 
/*!
  comments
  */
#define    R_INFO_1_GINT_STAT_REG        ((INFO_BASE_ADDR) + 0x1c)

/*!
  comments
  */
#define    R_INFO_2_MASK_REG                 ((INFO_BASE_ADDR) + 0x20)
/*!
  comments
  */
#define    R_INFO_2_GLOBAL_REG               ((INFO_BASE_ADDR) + 0x24)
/*!
  comments
  */
#define    R_INFO_2_GINT_STAT_CLR_REG   ((INFO_BASE_ADDR) + 0x28)
/*!
  comments
  */
#define    R_INFO_2_GINT_STAT_REG    ((INFO_BASE_ADDR) + 0x2c)

/*!
  sub
  */
#define R_SUB_CMD_REG           0x50200004
/*!
  comments
  */
#define R_SUB_BUF_REG    R_SUBP_BUF_ADDR0_REG
/*!
  comments
  */
#define M_SUB_CMD_REG_DEFAULT   0xff0000
/*!
  comments
  */
#define M_SUB_CMD_SUBEN            0x00000001
/*!
  comments
  */
#define M_SUB_CMD_PLANE_ALPHA_EN  0x00000100
/*!
  comments
  */
#define M_SUB_CMD_PLANE_ALPHA  0x00FF0000
/*!
  comments
  */
#define S_SUB_CMD_PLANE_ALPHA  (16)

/*!
  osd
  */
#define OSD_BASE_ADDR      0x50200000
/*!
  comments
  */
#define R_OSD_CMD_REG            ((OSD_BASE_ADDR) + 0x00)
/*! 
  comments
  */
#define R_OSD_EDGEWEIGHT_REG  ((OSD_BASE_ADDR) + 0x08)
/*!
  comments
  */
#define R_OSD_BUF_REG      R_OSD_BUF_ADDR0_REG
/*!
  comments
  */
#define R_OSD_CHROM_REG    ((OSD_BASE_ADDR) + 0x0c)

/*!
  comments
  */
#define R_VID_STILL_SUB_OSD_ORDER ((OSD_BASE_ADDR) + 0x10)

/*!
  comments
  */
#define M_VID_STILL_ORDER_MASK  0x00000001  
/*!
  comments
  */
#define M_SUB_OSD_ORDER_MASK    0x00000010

/*!
  comments
  */
#define M_OSD_CMD_OSDEN      0x00000001
/*!
  comments
  */
#define M_OSD_CMD_PLANEEN      0x00000100
/*!
  comments
  */
#define M_OSD_CMD_PLANE_ALPHA  0x00FF0000
/*!
  comments
  */
#define S_OSD_CMD_PLANE_ALPHA  (16)

/*!
  spic
  */
#define R_SPIC_CTRL_CMD_REG   0x50100000
/*!
  comments
  */
#define   R_SPIC_X_POS_REG      0x50100008
/*!
  comments
  */
#define R_SPIC_Y_POS_REG      0x5010000c
/*!
  comments
  */
#define   M_SPIC_FRAME_SEL            0x00000100
/*!
  comments
  */
#define   M_SPIC_DISPEN_SEL     0x00000010
/*!
  comments
  */
#define M_SPIC_MODE_FRAME   0x0
/*!
  comments
  */
#define   M_SPIC_MODE_FIELD     0x1
/*!
  comments
  */
#define   R_SPIC_BUF0_REG     R_STP_BUF_ADDR0_REG
/*!
  comments
  */
#define   R_SPIC_BUF1_REG     R_STP_BUF_ADDR1_REG
/*!
  Definition of I2C Module
  */
#define I2C_MASTER0_BASE_ADDR                             0x43000000
/*!
  comments
  */
#define R_I2C_MASTER0_PRER_H                              ((I2C_MASTER0_BASE_ADDR) + 0x1a)
/*!
  comments
  */
#define R_I2C_MASTER0_PRER_L                              ((I2C_MASTER0_BASE_ADDR) + 0x1e)
/*!
  comments
  */
#define R_I2C_MASTER0_TXR                                 ((I2C_MASTER0_BASE_ADDR) + 0x0a)
/*!
  comments
  */
#define R_I2C_MASTER0_RXR                                 ((I2C_MASTER0_BASE_ADDR) + 0x0e)
/*!
  comments
  */
#define R_I2C_MASTER0_SR                                  ((I2C_MASTER0_BASE_ADDR) + 0x06)
/*!
  comments
  */
#define R_I2C_MASTER0_CTR                                 ((I2C_MASTER0_BASE_ADDR) + 0x12)
/*!
  comments
  */
#define R_I2C_MASTER0_CR                                  ((I2C_MASTER0_BASE_ADDR) + 0x16)

/*!
  comments
  */
#define I2C_MASTER1_BASE_ADDR                             0x4b000000
/*!
  comments
  */
#define R_I2C_MASTER1_PRER_H                              ((I2C_MASTER1_BASE_ADDR) + 0x1a)
/*!
  comments
  */
#define R_I2C_MASTER1_PRER_L                              ((I2C_MASTER1_BASE_ADDR) + 0x1e)
/*!
  comments
  */
#define R_I2C_MASTER1_TXR                                 ((I2C_MASTER1_BASE_ADDR) + 0x0a)
/*!
  comments
  */
#define R_I2C_MASTER1_RXR                                 ((I2C_MASTER1_BASE_ADDR) + 0x0e)
/*!
  comments
  */
#define R_I2C_MASTER1_SR                                  ((I2C_MASTER1_BASE_ADDR) + 0x06)
/*!
  comments
  */
#define R_I2C_MASTER1_CTR                                 ((I2C_MASTER1_BASE_ADDR) + 0x12)
/*!
  comments
  */
#define R_I2C_MASTER1_CR                                  ((I2C_MASTER1_BASE_ADDR) + 0x16)

/*!
  comments
  */
#define I2C_MASTER2_BASE_ADDR                             0x46000000
/*!
  comments
  */
#define R_I2C_MASTER2_PRER_H                              ((I2C_MASTER1_BASE_ADDR) + 0x1b)
/*!
  comments
  */
#define R_I2C_MASTER2_PRER_L                              ((I2C_MASTER1_BASE_ADDR) + 0x1f)
/*!
  comments
  */
#define R_I2C_MASTER2_TXR                                 ((I2C_MASTER1_BASE_ADDR) + 0x0b)
/*!
  comments
  */
#define R_I2C_MASTER2_RXR                                 ((I2C_MASTER1_BASE_ADDR) + 0x0f)
/*!
  comments
  */
#define R_I2C_MASTER2_SR                                  ((I2C_MASTER1_BASE_ADDR) + 0x07)
/*!
  comments
  */
#define R_I2C_MASTER2_CTR                                 ((I2C_MASTER1_BASE_ADDR) + 0x13)
/*!
  comments
  */
#define R_I2C_MASTER2_CR                                  ((I2C_MASTER1_BASE_ADDR) + 0x17)


/*!
  Definition of reset registers
  */
#define RST_BASE_ADDR                             0x6f800000
/*!
  comments
  */
#define R_RST_MEDIARST_MODE                       ((RST_BASE_ADDR) + 0x0)
/*!
  comments
  */
#define R_RST_CHSWITCH_MODE                       ((RST_BASE_ADDR) + 0x4)
/*!
  comments
  */
#define R_RST_VOUTRST_EN                          ((RST_BASE_ADDR) + 0x8)
/*!
  comments
  */
#define R_RST_GLOBAL0_MASK                        ((RST_BASE_ADDR) + 0xc)
/*!
  comments
  */
#define R_RST_GLOBAL0_CTRL                        ((RST_BASE_ADDR) + 0x10)
/*!
  comments
  */
#define R_RST_BUS_DETECT_MODE                     ((RST_BASE_ADDR) + 0x14)
/*!
  comments
  */
#define R_RST_SYNCRST_MODE                        ((RST_BASE_ADDR) + 0x18)
/*!
  comments
  */
#define R_RST_GLOBAL1_MASK                        ((RST_BASE_ADDR) + 0x20)
/*!
  comments
  */
#define R_RST_GLOBAL1_CTRL                        ((RST_BASE_ADDR) + 0x24)


/*!
  Definition of audio config register
  */
#define TS_PARSE_BASE        0x6f8a0000

/*!
  comments
  */
#define TS_PARSE_SET_REG            ((TS_PARSE_BASE) + 0x00)
/*!
  comments
  */
#define TS_ES_PLAYER_REG          ((TS_PARSE_BASE) + 0x04)  

/*!
  comments
  */
#define AUDIO_BASE_ADDR            0x6f820000
/*!
  comments
  */
#define ADEC_STATUS_BASE       0x6f920000
/*!
  comments
  */
#define AES_CTRL_ADDR_REG                 0x6f400080
/*!
  comments
  */
#define AES_CTRL_RD_ADDR_REG           0x6f400090
/*!
  comments
  */
#define ES_STATUS_REG         0x6f900000

/*!
  comments
  */
#define R_AUDIO_PLAY_SEL_REG      ((AUDIO_BASE_ADDR) + 0x00)
/*!
  comments
  */
#define R_AUDIO_DIF_PARAM_REG      ((AUDIO_BASE_ADDR) + 0x04)    
/*!
  comments
  */
#define R_AUDIO_VOLUME_SET_REG    ((AUDIO_BASE_ADDR) + 0x08)
/*!
  comments
  */
#define R_AUDIO_MUTE_MODE0_REG    ((AUDIO_BASE_ADDR) + 0x0c)
/*!
  comments
  */
#define R_AUDIO_MUTE_MODE1_REG    ((AUDIO_BASE_ADDR) + 0x10)

/*!
  comments
  */
#define R_ADEC_STATUS_REG0          ((ADEC_STATUS_BASE) + 0x00)    
/*!
  comments
  */
#define R_ADEC_STATUS_REG1          ((ADEC_STATUS_BASE) + 0x04)    
/*!
  comments
  */
#define R_ADEC_STATUS_REG2          ((ADEC_STATUS_BASE) + 0x08)    
/*!
  comments
  */
#define R_ADEC_STATUS_REG3          ((ADEC_STATUS_BASE) + 0x0c)    


/*!
  comments
  */
#define R_AUDOUT_OUTPUT_MODE        ((AUDIO_BASE_ADDR) + 0x04)
/*!
  comments
  */
#define R_AUDOUT_NPCM_ADDR_BEGIN_REG  ((AUDIO_BASE_ADDR) + 0x10)
/*!
  comments
  */
#define R_AUDOUT_NPCM_WADDR_REG      ((AUDIO_BASE_ADDR) + 0x14)
/*!
  comments
  */
#define R_AUDOUT_NPCM_BUF_SIZE_REG    ((AUDIO_BASE_ADDR) + 0x20)

/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS1_REG    ((AUDIO_BASE_ADDR) + 0x24)
/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS2_REG    ((AUDIO_BASE_ADDR) + 0x28)
/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS3_REG    ((AUDIO_BASE_ADDR) + 0x2c)
/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS4_REG    ((AUDIO_BASE_ADDR) + 0x30)
/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS5_REG    ((AUDIO_BASE_ADDR) + 0x34)
/*!
  comments
  */
#define R_AUDOUT_CHAN_STATUS6_REG    ((AUDIO_BASE_ADDR) + 0x38)

/*!
  comments
  */
#define R_AUDOUT_SPDIF_MOD_CTRL_REG  ((AUDIO_BASE_ADDR) + 0x40)


/*!
  comments
  */
#define R_AUDOUT_SHAREINT_MASK_REG    ((AUDIO_BASE_ADDR) + 0x54)
/*!
  comments
  */
#define R_AUDOUT_SHAREINT_CLR_REG    ((AUDIO_BASE_ADDR) + 0x58)
/*!
  comments
  */
#define R_AUDOUT_SHAREINT_MOD0_REG    ((AUDIO_BASE_ADDR) + 0x5c)
/*!
  comments
  */
#define R_AUDOUT_SHAREINT_MOD1_REG    ((AUDIO_BASE_ADDR) + 0x60)
/*!
  comments
  */
#define R_AUDOUT_SHAREINT_STAT_REG    ((AUDIO_BASE_ADDR) + 0x64)
/*!
  comments
  */
#define R_AUDOUT_NPCM_BUF_TOL_REG    ((AUDIO_BASE_ADDR) + 0x4c)

/*!
  comments
  */
#define R_AUDOUT_NPCM_RMBUF_REG    ((ADEC_STATUS_BASE) + 0x18)
/*!
  comments
  */
#define R_AUDOUT_NPCM_FRAME_INFO_REG  ((ADEC_STATUS_BASE) + 0x18)

/*!
  comments
  */
#define R_AES_RADDR_UPDATE_REG      ((ES_BUF_ADDR_BASE) + 0x90)
/*!
  comments
  */
#define R_ES_NUM_REG            ((ES_STATUS_REG) + 0x4)


/*!
  comments
  */
#define R_AUD_STATUS_APTS_REG    0x6f930028
/*!
  comments
  */
#define R_AUD_STATUS_ADTS_REG    0x6f93002C
/*!
  comments
  */
#define R_AUD_STATUS_FIRST_VPTS_REG  0x6f930044
/*!
  comments
  */
#define R_AUD_STATUS_FIRST_VDTS_REG  0x6f930048

/*!
  comments
  */
#define S_AUDIO_TS_OR_ES_SHIFT      0
/*!
  comments
  */
#define S_AUDIO_PCM_OR_MPEG_SHIFT    1
/*!
  comments
  */
#define S_AUDIO_PCM_STEREO_SHIFT      2
/*!
  comments
  */
#define S_AUDIO_PCM_ENDIAN_SHIFT      3
/*!
  comments
  */
#define S_AUDIO_PCM_FREQ_SHIFT      4
  
/*!
  comments
  */
#define S_AUDIO_ZERO_NUM_SHIFT      0
/*!
  comments
  */
#define S_AUDIO_PCM_NUM_SHIFT        6
/*!
  comments
  */
#define S_AUDIO_RIGHT_CH_SHIFT      12
/*!
  comments
  */
#define S_AUDIO_JUSTIFY_MODE_SHIFT    13
/*!
  comments
  */
#define S_AUDIO_FREQ_MODE_SHIFT      15
/*!
  comments
  */
#define S_AUDIO_SPDIF_SET_SHIFT      16
/*!
  comments
  */
#define S_AUDIO_FMT_SET_SHIFT        31

/*!
  comments
  */
#define S_AUDIO_OUPUT_SHIFT        24
/*!
  comments
  */
#define S_AUDIO_TRACK_SHIFT        26
/*!
  comments
  */
#define S_AUDIO_TRACK_CP_CTRL_SHIFT    28
/*!
  comments
  */
#define S_ADUIO_TRACK_SPEC_MODE_SHIFT  29

/*!
  comments
  */
#define S_AUDIO_CHMUTE_MODE_SHIFT    16
/*!
  comments
  */
#define S_AUDIO_AERRMUTE_MODE_SHIFT    17

/*!
  comments
  */
#define S_AUDIO_MUTE_CA_RST_EN_SHIFT    16
/*!
  comments
  */
#define S_AUDIO_MUTE_STAT_CTRL_SHIFT    17
/*!
  comments
  */
#define S_AUDIO_MUTE_CA_RST_MODE_SHIFT  18

/*!
  comments
  */
#define S_AUDIO_HEAD_ENCODE_FMT_SHIFT  19
/*!
  comments
  */
#define S_AUDIO_HEAD_SAMPLE_FREQ_SHIFT  10
/*!
  comments
  */
#define S_AUDIO_HEAD_STEREO_MODE_SHIFT  6


/*!
  comments
  */
#define M_AUDIO_TS_OR_ES_MASK        0x00000001
/*!
  comments
  */
#define M_AUDIO_PCM_OR_MPEG_MASK      0x00000002
/*!
  comments
  */
#define M_AUDIO_PCM_STEREO_MASK             0x00000004
/*!
  comments
  */
#define M_AUDIO_PCM_ENDIAN_MASK             0x00000008
/*!
  comments
  */
#define M_AUDIO_PCM_FREQ_MASK        0x00000070
  
/*!
  comments
  */
#define M_AUDIO_ZERO_NUM_MASK        0x0000003f
/*!
  comments
  */
#define M_AUDIO_PCM_NUM_MASK               0x00000fc0
/*!
  comments
  */
#define M_AUDIO_RIGHT_CH_MASK        0x00001000
/*!
  comments
  */
#define M_AUDIO_JUSTIFY_MODE_MASK      0x00006000
/*!
  comments
  */
#define M_AUDIO_FREQ_MODE_MASK             0x00008000
/*!
  comments
  */
#define M_AUDIO_SPDIF_SET_MASK             0x03ff0000
/*!
  comments
  */
#define M_AUDIO_FMT_SET_MASK               0x40000000
  
/*!
  comments
  */
#define M_AUDIO_TRACK_MASK               0x0c000000
/*!
  comments
  */
#define M_AUDIO_OUTPUT_MASK               0x03000000
/*!
  comments
  */
#define M_AUDIO_VOLUME_MASK               0x0000ffff
  
/*!
  comments
  */
#define M_AUDIO_CHMUTE_FRAMENUM_MASK    0x000000ff
/*!
  comments
  */
#define M_AUDIO_AERRMUTE_FRAMENUM_MASK         0x0000ff00
  
/*!
  comments
  */
#define M_AUDIO_MUTE_CHK_NUM_MASK      0x000000ff
/*!
  comments
  */
#define M_AUDIO_MUTE_CHK_GUARD_MASK           0x00003f00
  
/*!
  comments
  */
#define M_AUDIO_HEAD_ENCODE_FMT_MASK      0x00080000
/*!
  comments
  */
#define M_AUDIO_HEAD_SAMPLE_FREQ_MASK      0x00000c00
/*!
  comments
  */
#define M_AUDIO_HEAD_STEREO_MODE_MASK      0x000000c0


/*!
  comments
  */
#define M_AUDOUT_SPDIF_MOD_CFG_MUTE_MASK      0xc0000000
/*!
  comments
  */
#define S_AUDOUT_SPDIF_MOD_CFG_MUTE_SHIFT      30
/*!
  comments
  */
#define M_AOUT_HDREAD_MODE_BIT      0x00010000

/*!
  comments
  */
#define S_AES_NUM_SHIFT_NUM        16
/*!
  comments
  */
#define M_AES_NUM_MASK_BIT        0x1fff

/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_MASK          0x0007f000

/*!
  comments
  */
#define M_AUDOUT_INT_PCM_FRMDONE_MASK      0x00000001
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_DIE_MASK        0x00000002
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_EMPTY_MASK      0x00000004
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_VOLOVERFLOW_MASK  0x00000008
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_FRMINFOERR_MASK    0x00000010
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_NEWFRMSTA_MASK    0x00000020
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_FSCHANGE_MASK    0x00000040
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_ALMOST_EMP_MASK    0x00000080
/*!
  comments
  */
#define M_AUDOUT_INT_PCM_ALMOST_FULL_MASK    0x00000100

/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_FRMDONE_MASK    0x00001000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_DIE_MASK        0x00002000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_NEWFRMSTA_MASK    0x00004000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_EMPTY_MASK      0x00008000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_FSCHANGE_MASK    0x00010000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_ALMOST_EMP_MASK  0x00020000
/*!
  comments
  */
#define M_AUDOUT_INT_NPCM_ALMOST_FULL_MASK  0x00040000

/*!
  comments
  */
#define M_AUDOUT_APP_MOD_MASK          0x00000001
/*!
  comments
  */
#define M_AUDOUT_PCM_MOD_MASK          0x00000002
/*!
  comments
  */
#define M_AUDOUT_CPYRIGT_MOD_MASK        0x00000004
/*!
  comments
  */
#define M_AUDOUT_PRE_EMP_L_MASK        0x00000038
/*!
  comments
  */
#define M_AUDOUT_PRE_EMP_R_MASK        0x000001c0
/*!
  comments
  */
#define M_AUDOUT_PCM_WIDTH_MASK        0x07800000
/*!
  comments
  */
#define S_AUDOUT_PRE_EMP_L_SHIFT        3
/*!
  comments
  */
#define S_AUDOUT_PRE_EMP_R_SHIFT        6
/*!
  comments
  */
#define S_AUDOUT_PCM_WIDTH_SHIFT        23


/*!
  comments
  */
#define M_AUDOUT_CHAN_NUM_L_MASK        0x000000ff
/*!
  comments
  */
#define M_AUDOUT_CHAN_NUM_R_MASK        0x0000ff00
/*!
  comments
  */
#define M_AUDOUT_CPYMOD_MASK          0x00030000
/*!
  comments
  */
#define M_AUDOUT_AUXDATMOD_MASK        0x001c0000
/*!
  comments
  */
#define M_AUDOUT_ALIGN_LEV_MASM        0x00600000
/*!
  comments
  */
#define M_AUDOUT_REF_SIG_MASK          0x01800000
/*!
  comments
  */
#define M_AUDOUT_SCALE_FLAG_MASK        0x02000000
/*!
  comments
  */
#define M_AUDOUT_MAINDAT_VALID_MASK      0x04000000
/*!
  comments
  */
#define S_AUDOUT_CHAN_NUM_R_SHIFT          8
/*!
  comments
  */
#define S_AUDOUT_CPYMOD_SHIFT          16
/*!
  comments
  */
#define S_AUDOUT_AUXDATMOD_SHIFT        18
/*!
  comments
  */
#define S_AUDOUT_ALIGN_LEV_SHIFT        21
/*!
  comments
  */
#define S_AUDOUT_REF_SIG_SHIFT          23
/*!
  comments
  */
#define S_AUDOUT_STATU_VALID_SHIFT        27

/*!
  comments
  */
#define M_AUDOUT_NPCM_SIZE_MASK            0x0000ffff
/*!
  comments
  */
#define M_AUDOUT_USER_SIZE_MASK            0xffff0000
/*!
  comments
  */
#define M_AUDOUT_PCM_ALMOST_FULL_MASK   0xffff0000
/*!
  comments
  */
#define M_AUDOUT_PCM_ALMOST_EMPTY_MASK     0x0000ffff
/*!
  comments
  */
#define M_AUDOUT_NPCM_ALMOST_FULL_MASK   0xffff0000
/*!
  comments
  */
#define M_AUDOUT_NPCM_ALMOST_EMPTY_MASK    0x0000ffff

/*!
  comments
  */
#define M_AUDOUT_PCM_SIZE_MASK             0x007fffff

/*!
  comments
  */
#define M_AUDOUT_PCM_VOL_MASK               0x00000007
/*!
  comments
  */
#define M_AUDOUT_PCM_EMP_NUM_MASK           0x000000f0
/*!
  comments
  */
#define M_AUDOUT_PCM_BUF_FULL_MASK          0x00000100
/*!
  comments
  */
#define M_AUDOUT_PCM_ACMODE_MASK            0x0003f000
/*!
  comments
  */
#define M_AUDOUT_PCM_FRAME_FS_MASK          0x00f00000

/*!
  comments
  */
#define M_AUDOUT_ERROR_DET_ENABLE           0x1111000
/*!
  comments
  */
#define M_AUDOUT_FRAME_DET_TOL_MASK              0x00000fff
/*!
  comments
  */
#define S_AUDOUT_FRAME_LEN_MODE0_SHIFT           12  
/*!
  comments
  */
#define S_AUDOUT_FRAME_LEN_MODE1_SHIFT           16
/*!
  comments
  */
#define S_AUDOUT_FRAME_LEN_MODE2_SHIFT           20 
/*!
  comments
  */
#define S_AUDOUT_FRAME_LEN_MODE3_SHIFT           24

/*!
  comments
  */
#define M_AUDOUT_ZERO_NUM_MASK      0x0000003f
/*!
  comments
  */
#define M_AUDOUT_PCM_NUM_MASK      0x00000fc0
/*!
  comments
  */
#define M_AUDOUT_RIGHT_CH_MASK      0x00001000
/*!
  comments
  */
#define M_AUDOUT_JUSTIFY_MODE_MASK    0x00006000
/*!
  comments
  */
#define M_AUDOUT_FREQ_MODE_MASK      0x00008000

/*!
  comments
  */
#define S_AUDOUT_PCM_NUM_SHIFT      6
/*!
  comments
  */
#define S_AUDOUT_RIGHT_CH_SHIFT      12
/*!
  comments
  */
#define S_AUDOUT_JUSTIFY_MODE_SHIFT    13
/*!
  comments
  */
#define S_AUDOUT_FREQ_MODE_SHIFT      15
/*!
  comments
  */
#define S_AUDOUT_I2S_SPDIF_SET_SHIFT    16

/*!
  comments
  */
#define M_AUDOUT_VOLUME_MASK            0x0000ffff
/*!
  comments
  */
#define M_AUDOUT_PCM_VOLUME_ZOOM_MASK       0x00010000 
/*!
  comments
  */
#define M_AUDOUT_PCM_PREC_MASK              0x01000000
/*!
  comments
  */
#define M_AUDOUT_EMPTY_BUF_READ_MASK        0x10000000
/*!
  comments
  */
#define M_AUDOUT_BUF_RD_CS_MASK             0x20000000

/*!
  comments
  */
#define M_AUDOUT_TIME_EN_MASK               0x00000001
/*!
  comments
  */
#define M_AUDOUT_ADDR_EN_MASK               0x00000010
/*!
  comments
  */
#define M_AUDOUT_NULL_EN_MASK               0x00000100
/*!
  comments
  */
#define M_AUDOUT_NULL_PERRID_MASK           0x00fffe00
/*!
  comments
  */
#define M_AUDOUT_NPCM_FS_CS_MASK            0x01000000
/*!
  comments
  */
#define M_AUDOUT_NPCM_CH_CS_MASK            0x02000000
/*!
  comments
  */
#define M_AUDOUT_SPDIF_MUTE_MASK            0x40000000
/*!
  comments
  */
#define M_AUDOUT_NPCM_MUTE_MASK             0x80000000
/*!
  comments
  */
#define S_AUDOUT_NULL_PERIOD_SHIFT          9

/*!
  comments
  */
#define M_AUDOUT_I2S_MCLK_MOD_MASK          0x00000001
/*!
  comments
  */
#define M_AUDOUT_I2S_MCLK_AUTO_CHG_MASK     0x00000010
/*!
  comments
  */
#define M_AUDOUT_SPDIF_MCLK_MUTLI_NUM_MASK  0x00000f00
/*!
  comments
  */
#define M_AUDOUT_SPDIF_MCLK_DIV2_EN_MASK    0x00001000
/*!
  comments
  */
#define S_AUDOUT_SPDIF_MCLK_MUTLI_NUM_SHIF  8 

/*!
  comments
  */
#define M_AUDOUT_BUF_TOL_MASK                0x3f000000
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID0_MASK              0x00000007
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID1_MASK              0x00000070
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID2_MASK              0x00000700
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID3_MASK              0x00007000
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID4_MASK              0x00070000
/*!
  comments
  */
#define M_AUDOUT_CHAN_ID5_MASK              0x00700000
/*!
  comments
  */
#define M_AUDOUT_TRACK_CP_CTRL_MASK     0x40000000

/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT0                0
/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT1                4
/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT2                8
/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT3                12
/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT4                16
/*!
  comments
  */
#define S_AUDOUT_CHAN_SHIFT5                20


/*!
  definition of Avsync Register 
  */
#define R_AVSYNC_REG_RSTSYNC      ((RST_BASE_ADDR) + 0x0)
/*!
  comments
  */
#define R_AVSYNC_REG_AUDIO_CLK    0x6f8f0008
/*!
  comments
  */
#define R_AVSYNC_REG_TS_ADDR2      0x6f400008
/*!
  comments
  */
#define R_AVSYNC_REG_TS_ADDR3      0x6f40000c
/*!
  comments
  */
#define R_AVSYNC_REG_VID_TOLERANCE  0x6f830000
/*!
  comments
  */
#define R_AVSYNC_REG_VID_TOLERANCE1  0x6f830004
/*!
  comments
  */
#define R_AVSYNC_REG_AUD_TOLERANCE  0x6f830008
/*!
  comments
  */
#define R_AVSYNC_REG_AUD_TOLERANCE1  0x6f83000c
/*!
  comments
  */
#define R_AVSYNC_REG_DIFF_TOLE    0x6f830014
/*!
  comments
  */
#define R_AVSYNC_REG_AUD_FREQ_ADJUST  0x6f830018
/*!
  comments
  */
#define R_AVSYNC_REG_FREQ_SEL      0x6f83001c
/*!
  comments
  */
#define R_AVSYNC_REG_AVSYNC_SET    0x6f830020
/*!
  comments
  */
#define R_AVSYNC_REG_ASYNC_SET    0x6f830024
/*!
  comments
  */
#define R_AVSYNC_REG_VSYNC_SET    0x6f830028
/*!
  comments
  */
#define R_AVSYNC_REG_VBI_TOLERANCE  0x6f83002c
/*!
  comments
  */
#define R_AVSYNC_REG_VBI_TOLERANCE1  0x6f830030
/*!
  comments
  */
#define R_AVSYNC_REG_VBI_CTRL      0x6f830034

/*!
  comments
  */
#define R_AVSYNC_REG_CPU_SYNC_VID  0x6f83003c
/*!
  comments
  */
#define R_AVSYNC_REG_CPU_SYNC_VBI    0x6f830040
/*!
  comments
  */
#define R_AVSYNC_REG_CPU_SYNC_AUD  0x6f830044

/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_REG0    0x6f930000
/*!
  comments
  */
#define R_VBSYNC_REG_STATUS_REG0     0x6f930008
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_REG8    0x6f930004
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_REG22  0x6f93000c

/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_PCR    0x6f930038
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_VPTS    0x6f93003c
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_VDTS    0x6f930040
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_APTS    0x6f930044
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_ADTS    0x6f930048
/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_REG27  0x6f93006c

/*!
  comments
  */
#define R_AVSYNC_REG_PCR        0x6f930054
/*!
  comments
  */
#define R_AVSYNC_REG_PTS_V             0x6f930058
/*!
  comments
  */
#define R_AVSYNC_REG_PTS_A      0x6f93005c
/*!
  comments
  */
#define R_AVSYNC_REG_PTS_VBI      0x6f930060

/*!
  comments
  */
#define R_AVSYNC_REG_STATUS_REG33  0x6f930084

/*!
 Definition of IRDA register
  */
#define IRDA_BASE_ADDR                                   0x44000000
/*!
  comments
  */
#define R_IR_NEC_KEY                                     ((IRDA_BASE_ADDR) + (0x1))
/*!
  comments
  */
#define R_IR_NEC_USERL                                   ((IRDA_BASE_ADDR) + (0x5))
/*!
  comments
  */
#define R_IR_NEC_USERH                                   ((IRDA_BASE_ADDR) + (0x9))
/*!
  comments
  */
#define R_IR_NEC_CONFIG                                  ((IRDA_BASE_ADDR) + (0xd))
/*!
  comments
  */
#define R_IR_NEC_USERL_SET                               ((IRDA_BASE_ADDR) + (0x11))
/*!
  comments
  */
#define R_IR_NEC_USERH_SET                               ((IRDA_BASE_ADDR) + (0x15))
/*!
  comments
  */
#define R_IR_GLOBAL_CTRL                                 ((IRDA_BASE_ADDR) + (0x19))
/*!
  comments
  */
#define R_IR_GLOBAL_CFG                                  ((IRDA_BASE_ADDR) + (0x1d))
/*!
  comments
  */
#define R_IR_RECEIVE_NUM                                 ((IRDA_BASE_ADDR) + (0x21))
/*!
  comments
  */
#define R_IR_GLOBAL_OTSET                                ((IRDA_BASE_ADDR) + (0x25))
/*!
  comments
  */
#define R_IR_COM_ONTIME_L                                ((IRDA_BASE_ADDR) + (0x29))
/*!
  comments
  */
#define R_IR_COM_ONTIME_H                                ((IRDA_BASE_ADDR) + (0x2d))
/*!
  comments
  */
#define R_IR_COM_PERIOD_L                                ((IRDA_BASE_ADDR) + (0x31))
/*!
  comments
  */
#define R_IR_COM_PERIOD_H                                ((IRDA_BASE_ADDR) + (0x35))
/*!
  comments
  */
#define R_IR_NEC_STA                                     ((IRDA_BASE_ADDR) + (0x39))
/*!
  comments
  */
#define R_IR_NEC_KEY_SET                                 ((IRDA_BASE_ADDR) + (0x3d))
/*!
  comments
  */
#define R_IR_START_ONTIME_LH                             ((IRDA_BASE_ADDR) + (0x41))
/*!
  comments
  */
#define R_IR_START_ONTIME_LL                             ((IRDA_BASE_ADDR) + (0x45))
/*!
  comments
  */
#define R_IR_START_ONTIME_HH                             ((IRDA_BASE_ADDR) + (0x49))
/*!
  comments
  */
#define R_IR_START_ONTIME_HL                             ((IRDA_BASE_ADDR) + (0x4d))
/*!
  comments
  */
#define R_IR_START_PERIOD_LH                             ((IRDA_BASE_ADDR) + (0x51))
/*!
  comments
  */
#define R_IR_START_PERIOD_LL                             ((IRDA_BASE_ADDR) + (0x55))
/*!
  comments
  */
#define R_IR_START_PERIOD_HH                             ((IRDA_BASE_ADDR) + (0x59))
/*!
  comments
  */
#define R_IR_START_PERIOD_HL                             ((IRDA_BASE_ADDR) + (0x5d))
/*!
  comments
  */
#define R_IR_REPEAT_PERIOD_LH                            ((IRDA_BASE_ADDR) + (0x61))
/*!
  comments
  */
#define R_IR_REPEAT_PERIOD_LL                            ((IRDA_BASE_ADDR) + (0x65))
/*!
  comments
  */
#define R_IR_REPEAT_PERIOD_HH                            ((IRDA_BASE_ADDR) + (0x69))
/*!
  comments
  */
#define R_IR_REPEAT_PERIOD_HL                            ((IRDA_BASE_ADDR) + (0x6d))
/*!
  comments
  */
#define R_IR_DATA_ONTIME_L                               ((IRDA_BASE_ADDR) + (0x71))
/*!
  comments
  */
#define R_IR_DATA_ONTIME_H                               ((IRDA_BASE_ADDR) + (0x75))
/*!
  comments
  */
#define R_IR_ONE_PERIOD_L                                ((IRDA_BASE_ADDR) + (0x79))
/*!
  comments
  */
#define R_IR_ONE_PERIOD_H                                ((IRDA_BASE_ADDR) + (0x7d))
/*!
  comments
  */
#define R_IR_ZERO_PERIOD_L                               ((IRDA_BASE_ADDR) + (0x81))
/*!
  comments
  */
#define R_IR_ZERO_PERIOD_H                               ((IRDA_BASE_ADDR) + (0x85))
/*!
  comments
  */
#define R_IR_BUF_POP                                     ((IRDA_BASE_ADDR) + (0x89))
/*!
  comments
  */
#define R_IR_WAVEFILT_EN                                 ((IRDA_BASE_ADDR) + (0xc1))
/*!
  comments
  */
#define R_IR_WAVEFILT_LEN0                               ((IRDA_BASE_ADDR) + (0xc5))
/*!
  comments
  */
#define R_IR_WAVEFILT_LEN1                               ((IRDA_BASE_ADDR) + (0xc9))
/*!
  comments
  */
#define R_IR_WAVEFILT_LEN2                               ((IRDA_BASE_ADDR) + (0xcd))
/*!
  comments
  */
#define R_IR_WAVEFILT_LEN3                               ((IRDA_BASE_ADDR) + (0xd1))
/*!
  comments
  */
#define R_IR_WAVEFILT_STADR0                             ((IRDA_BASE_ADDR) + (0xd5))
/*!
  comments
  */
#define R_IR_WAVEFILT_STADR1                             ((IRDA_BASE_ADDR) + (0xd9))
/*!
  comments
  */
#define R_IR_WAVEFILT_STADR2                             ((IRDA_BASE_ADDR) + (0xdd))
/*!
  comments
  */
#define R_IR_WAVEFILT_STADR3                             ((IRDA_BASE_ADDR) + (0xe1))
/*!
  comments
  */
#define R_IR_WAVEFILT_STA                                ((IRDA_BASE_ADDR) + (0xe5))
/*!
  comments
  */
#define R_IR_WAVEFILT_STACLR                             ((IRDA_BASE_ADDR) + (0xe9))
/*!
  comments
  */
#define R_IR_WAVEFILT_MASK(i)                               ((IRDA_BASE_ADDR) + (0x101) + (i - 1)*4)
/*!
  comments
  */
#define R_IR_WFN_ONTIME_LL(i)          ((IRDA_BASE_ADDR) + (0x1001) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_ONTIME_LH(i)          ((IRDA_BASE_ADDR) + (0x1005) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_ONTIME_HL(i)          ((IRDA_BASE_ADDR) + (0x1009) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_ONTIME_HH(i)          ((IRDA_BASE_ADDR) + (0x100d) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_PERIOD_LL(i)          ((IRDA_BASE_ADDR) + (0x1011) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_PERIOD_LH(i)          ((IRDA_BASE_ADDR) + (0x1015) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_PERIOD_HL(i)          ((IRDA_BASE_ADDR) + (0x1019) + (0x20) * (i))
/*!
  comments
  */
#define R_IR_WFN_PERIOD_HH(i)          ((IRDA_BASE_ADDR) + (0x101d) + (0x20) * (i))

/*!
 Definition of OTP register
  */
#define OTP_BASE_ADDR                             0x6f8c0000
/*!
  comments
  */
#define R_OTP_TXDB                                (OTP_BASE_ADDR)
/*!
  comments
  */
#define R_OTP_RXDB                                (OTP_BASE_ADDR)
/*!
  comments
  */
#define R_OTP_DATA1                               (OTP_BASE_ADDR)
/*!
  comments
  */
#define R_OTP_DATA2                               ((OTP_BASE_ADDR) + 0x40)
/*!
  comments
  */
#define R_OTP_DATA3                               ((OTP_BASE_ADDR) + 0x80)
/*!
  comments
  */
#define R_OTP_DATA4                               ((OTP_BASE_ADDR) + 0xc0)
/*!
  comments
  */
#define R_OTP_CTRL_DATA                           ((OTP_BASE_ADDR) + 0xe0)
/*!
  comments
  */
#define R_OTP_TIME_CFG                            ((OTP_BASE_ADDR) + 0x100)
/*!
  comments
  */
#define R_OTP_WR_CFG                              ((OTP_BASE_ADDR) + 0x104)
/*!
  comments
  */
#define R_OTP_RD_CFG                              ((OTP_BASE_ADDR) + 0x108)
/*!
  comments
  */
#define R_OTP_CTRL                                ((OTP_BASE_ADDR) + 0x10c)

/*!
  definition of DMA register
  */
#define   DMA_BASE_ADDR                           0x6f860000
/*!
  comments
  */
#define   R_DMA0_SRC_STARTADD                     ((DMA_BASE_ADDR) + 0x0)
/*!
  comments
  */
#define   R_DMA0_GOAL_STARTADD                    ((DMA_BASE_ADDR) + 0x4)
/*!
  comments
  */
#define   R_DMA0_BLOCK_PARA0                      ((DMA_BASE_ADDR) + 0x8)
/*!
  comments
  */
#define   R_DMA0_BLOCK_PARA1                      ((DMA_BASE_ADDR) + 0xc)
/*!
  comments
  */
#define   R_DMA0_ALU_DWORD                        ((DMA_BASE_ADDR) + 0x10)
/*!
  comments
  */
#define   R_DMA1_SRC_STARTADD                     ((DMA_BASE_ADDR) + 0x18)
/*!
  comments
  */
#define   R_DMA1_GOAL_STARTADD                    ((DMA_BASE_ADDR) + 0x1c)
/*!
  comments
  */
#define   R_DMA1_BLOCK_PARA0                      ((DMA_BASE_ADDR) + 0x20)
/*!
  comments
  */
#define   R_DMA1_BLOCK_PARA1                      ((DMA_BASE_ADDR) + 0x24)
/*!
  comments
  */
#define   R_DMA1_ALU_DWORD                        ((DMA_BASE_ADDR) + 0x28)
/*!
  comments
  */
#define   R_DMA_CMD_OPCODE                        ((DMA_BASE_ADDR) + 0x14)
/*!
  comments
  */
#define   R_DMA_BUSY                              0x6f890014
/*!
  comments
  */
#define   R_DMA_LIMIT_ADDR0                       0x6f400078
/*!
  comments
  */
#define   R_DMA_LIMIT_ADDR1                       0x6f40007c

/*!
  Definition of GPIO register
  */
#define  GPIO_BASE_ADDR                         0x41800000
/*!
  comments
  */
#define  R_GPIO_W_DATA_REG                      ((GPIO_BASE_ADDR) + 0x00)
/*!
  comments
  */
#define  R_GPIO_W_DATA_EN_REG                  ((GPIO_BASE_ADDR) + 0x04)
/*!
  comments
  */
#define  R_GPIO_R_DATA_REG                       ((GPIO_BASE_ADDR) + 0x08)
/*!
  comments
  */
#define  R_GPIO_MASK_REG                       ((GPIO_BASE_ADDR) + 0x0c)

/*!
  Definition of SPI register
  */
#define SPIN_BASE_ADDR                             0x2c000000
/*!
  comments
  */
#define R_SPIN_TXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x00))
/*!
  comments
  */
#define R_SPIN_RXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x04))
/*!
  comments
  */
#define R_SPIN_BAUD(i)                            ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x08))
/*!
  comments
  */
#define R_SPIN_MODE_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x0c))
/*!
  comments
  */
#define R_SPIN_INT_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x10))
/*!
  comments
  */
#define R_SPIN_TC(i)                               ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x14))
/*!
  comments
  */
#define R_SPIN_CTRL(i)                             ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x18))
/*!
  comments
  */
#define R_SPIN_STA(i)                              ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x1c))
/*!
  comments
  */
#define R_SPIN_PIN_CFG_IO1(i)                     ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x20))
/*!
  comments
  */
#define R_SPIN_PIN_CFG_IO2(i)                     ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x24))
/*!
  comments
  */
#define R_SPIN_PIN_MODE(i)                        ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x2c))
/*!
  comments
  */
#define R_SPIN_MOSI_CTRL(i)                       ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x34))
/*!
  comments
  */
#define R_SPIN_DELAY_CFG(i)                        ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x38))
/*!
  comments
  */
#define R_SPIN_INT_STA(i)                          ((SPIN_BASE_ADDR) + (i) * (0x1000000) + (0x3c))

/*!
  flash virtual memory address
  */
#define FLASH_BASE_ADDR  0xF0000000
/*!
 Definition of VENC register
  */
#define VENC_BASE_ADDR    0x6f840000
/*!
  comments
  */
#define R_VENC_DELAY        (VENC_BASE_ADDR + 0x08)
/*!
  comments
  */
#define R_VENC_BLANK_P        (VENC_BASE_ADDR + 0x0c)
/*!
  comments
  */
#define R_VENC_CFIG1        (VENC_BASE_ADDR + 0x20)
/*!
  comments
  */
#define R_VENC_CFIG2        (VENC_BASE_ADDR + 0x24)
/*!
  comments
  */
#define R_VENC_CFIG3        (VENC_BASE_ADDR + 0x28)
/*!
  comments
  */
#define R_VENC_CFIG4        (VENC_BASE_ADDR + 0x2c)
/*!
  comments
  */
#define R_VENC_DACNUM        (VENC_BASE_ADDR + 0x64)
/*!
  comments
  */
#define R_VENC_INSTM        (VENC_BASE_ADDR + 0x68)
/*!
  comments
  */
#define R_VENC_COMPRESS        (VENC_BASE_ADDR + 0x6c)
/*!
  comments
  */
#define R_VENC_SET2        (VENC_BASE_ADDR + 0x74)
/*!
  comments
  */
#define R_VENC_COEF_C_LUT1_U        (VENC_BASE_ADDR + 0xb8)

#endif //__SYS_REGS_JAZZ_H__
