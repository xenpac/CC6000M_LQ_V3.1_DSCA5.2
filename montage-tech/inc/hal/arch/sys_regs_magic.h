/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SYS_REGS_MAGIC_H__
#define __SYS_REGS_MAGIC_H__

/************************************************************************
 * Definition of system control register
 ************************************************************************/
#define   SYSCTRL_BASE_ADDR                       0x40000000
#define   R_SYSCTRL_RESET_STATUS                  (SYSCTRL_BASE_ADDR + 0x00)
#define   R_SYSCTRL_SOFT_RESET                    (SYSCTRL_BASE_ADDR + 0x10)
#define   R_SYSCTRL_RESET_CTRL                    (SYSCTRL_BASE_ADDR + 0x14)
#define   M_SYSCTRL_RESET_ADEC_RST                0x8
#define   M_SYSCTRL_RESET_AOUT_RST                0x400
#define   M_SYSCTRL_RESET_ADECOUT_RST             0x408
#define   R_SYSCTRL_LPCLOCK                       (SYSCTRL_BASE_ADDR + 0x1C)
#define   R_SYSCTRL_SET_AUDIO_CLK                 (SYSCTRL_BASE_ADDR + 0x34)
#define   R_SYSCTRL_SET_ADAC_REG1                 (SYSCTRL_BASE_ADDR + 0x38)
#define   M_SYSCTRL_ADAC_MUTE                     0x100
#define   M_SYSCTRL_BYPASS_SDM                    0x10000
#define   M_SYSCTRL_ADAC1_FS_BYPASS               0x100000
#define   M_SYSCTRL_ADAC1_FS_RST                  0x80000
#define   M_SYSCTRL_ADAC1_DAC_UP                  0x20040
#define   R_SYSCTRL_SET_ADAC_REG2                 (SYSCTRL_BASE_ADDR + 0x3C)
#define   R_SYSCTRL_SET_SYSCLK                    (SYSCTRL_BASE_ADDR + 0x44)
#define   R_SYSCTRL_VDAC_CFG0                    (SYSCTRL_BASE_ADDR + 0xbc)
#define   R_SYSCTRL_VDAC_CFG1                    (SYSCTRL_BASE_ADDR + 0xc0)
/*
#define   R_SYSCTRL_SET_VDAC                      (SYSCTRL_BASE_ADDR + 0x48)
#define   M_SYSCTRL_VDAC_PU0                       0x100
#define   M_SYSCTRL_VDAC_PU1                       0x200
#define   M_SYSCTRL_VDAC_PU2                       0x400
#define   M_SYSCTRL_VDAC_PU3                       0x800
#define   M_SYSCTRL_VDAC0_PU                      0x800
#define   M_SYSCTRL_VDAC1_PU                      0x400
#define   M_SYSCTRL_VDAC2_PU                      0x200
#define   M_SYSCTRL_VDAC3_PU                      0x100
*/
#define   R_SYSCTRL_PRIOR_L                       (SYSCTRL_BASE_ADDR + 0x50)
#define   R_SYSCTRL_PRIOR_H                       (SYSCTRL_BASE_ADDR + 0x54)
#define   R_SYSCTRL_CFGPIN                        (SYSCTRL_BASE_ADDR + 0x58)
#define   R_SYSCTRL_CLKOBS                        (SYSCTRL_BASE_ADDR + 0x5C)
#define   R_SYSCTRL_PC_RESET                      (SYSCTRL_BASE_ADDR + 0x60)
#define   R_SYSCTRL_CACHE_CTRL                    (SYSCTRL_BASE_ADDR + 0x64)
#define   R_SYSCTRL_CHIP_VER                      (SYSCTRL_BASE_ADDR + 0x88)
#define   R_SYSCTRL_PRIORL_PAIR                   (SYSCTRL_BASE_ADDR + 0x8C)
#define   R_SYSCTRL_PRIORH_PAIR                   (SYSCTRL_BASE_ADDR + 0x90)

#define   S_SYSCTRL_RESET_I2C                     11
                                                  
#define   S_SYSCTRL_I_CACHE_CLEAR                 0
#define   S_SYSCTRL_CACHE_ENABLE                  1
#define   S_SYSCTRL_POSTWR_ENABLE                 2
                                                  
#define   S_SYSCTRL_RESET_POWER                   0
#define   S_SYSCTRL_RESET_WATCHDOG                1
#define   S_SYSCTRL_RESET_SOFT                    3

#define   M_SYSCTRL_MEM_ADDR_LIMIT                0xFF003FFF
#define   S_SYSCTRL_SDRAM_ADDR_LIMIT              14
#define   S_SYSCTRL_FLASH_ADDR_LIMIT              20

/*******************************************************************
 * Definition of MCU register
 *******************************************************************/
 #define   MCU_BASE_ADDR                           0x2E00C000
 #define   R_SYSCTRL_SET_VDAC                      (MCU_BASE_ADDR + 0x40)
#define   M_SYSCTRL_VDAC_PU0                       0x1
#define   M_SYSCTRL_VDAC_PU1                       0x2
#define   M_SYSCTRL_VDAC_PU2                       0x4
#define   M_SYSCTRL_VDAC_PU3                       0x8

/*******************************************************************
 * Definition of IRQ register
 *******************************************************************/
#define   IRQ_BASE_ADDR                           0x31000000
#define   R_IRQ_MODE                              (IRQ_BASE_ADDR+0x00)
#define   R_IRQ_MASK                              (IRQ_BASE_ADDR+0x04)
#define   M_IRQ_MASK_SMC                          0x100
#define   R_IRQ_VECTOR                            (IRQ_BASE_ADDR+0x08)
#define   R_IRQ_COW0                              (IRQ_BASE_ADDR+0x0c)
#define   R_IRQ_PRIOR0                            (IRQ_BASE_ADDR+0x10)
#define   R_IRQ_PRIOR1                            (IRQ_BASE_ADDR+0x14)
#define   R_IRQ_COW1                              (IRQ_BASE_ADDR+0x20)
#define   R_WATCHDOG_DLY                          (IRQ_BASE_ADDR+0x40)
#define   R_WATCHDOG_CTRL                         (IRQ_BASE_ADDR+0x44)
#define   R_TRIG_MODE                           (IRQ_BASE_ADDR+0x60)


/************************************************************************
 * Definition of I2C Module
 ************************************************************************/
#define I2C_BASE_ADDR                             0x42000000
#define R_I2C_PRER_H                              (I2C_BASE_ADDR+0x18)
#define R_I2C_PRER_L                              (I2C_BASE_ADDR+0x1c)
#define R_I2C_TXR                                 (I2C_BASE_ADDR+0x08)
#define R_I2C_RXR                                 (I2C_BASE_ADDR+0x0c)
#define R_I2C_SR                                  (I2C_BASE_ADDR+0x04)
#define R_I2C_CTR                                 (I2C_BASE_ADDR+0x10)
#define R_I2C_CR                                  (I2C_BASE_ADDR+0x14)

/************************************************************************
 * Definition of NOR-FLASH register
 ************************************************************************/
#define NORF_BASE_ADDR                            0x08000000
#define NORF_BASE_ADDR_NON_CACHE                  (NORF_BASE_ADDR | 0x10000000)
#define NORF_CTRL_BASE_ADDR                       0x30000000
#define R_NORF_SOFT_CTRL0                         (NORF_CTRL_BASE_ADDR + 0)
#define R_NORF_SOFT_CTRL1                         (NORF_CTRL_BASE_ADDR + 0x0c)
#define R_NORF_SOFT_DATA0                         (NORF_CTRL_BASE_ADDR + 0x04)
#define R_NORF_SOFT_DATA1                         (NORF_CTRL_BASE_ADDR + 0x08)
#define R_NORF_SPI_CTRL                           (NORF_CTRL_BASE_ADDR + 0x14)
#define R_NORF_SOFT_DUMMY0                        (NORF_CTRL_BASE_ADDR + 0x18)
#define R_NORF_SOFT_DUMMY1                        (NORF_CTRL_BASE_ADDR + 0x1c)
#define R_NORF_HW_WR_CTRL                         (NORF_CTRL_BASE_ADDR + 0x20)
#define R_NORF_CMD_REL                            (NORF_CTRL_BASE_ADDR + 0x800C)


/*************************************************
      definition of DMA register
**************************************************/
#define   DMA_BASE_ADDR                           0x3d860000
#define   R_DMA_BUSY                              0x3f000004
#define   R_DMA_SRC_STARTADD                      (DMA_BASE_ADDR+0x0)
#define   R_DMA_DST_STARTADD                      (DMA_BASE_ADDR+0x4)
#define   R_DMA_DST_BLKPARA0                      (DMA_BASE_ADDR+0x8)
#define   R_DMA_DST_BLKPARA1                      (DMA_BASE_ADDR+0xc)
#define   R_DMA_ALU_DWORD                         (DMA_BASE_ADDR+0x10)
#define   R_DMA_CMD_OPCODE                        (DMA_BASE_ADDR+0x14)

/**************************************************************************
*
* SDRAM controllor
*
***************************************************************************/
#define   SDRAM_BASE_ADDR                         0x20000000
#define   R_SDRAM_TIMING                          (SDRAM_BASE_ADDR + 0x00)
#define   R_SDRAM_MODE                            (SDRAM_BASE_ADDR + 0x04)
#define   R_SDRAM_REF_PAR                         (SDRAM_BASE_ADDR + 0x08)
#define   R_SDRAM_REF_PRECHA                      (SDRAM_BASE_ADDR + 0x0c)
#define   R_SDRAM_TYPE                            (SDRAM_BASE_ADDR + 0x10)
#define   R_SDRAM_IDLE_CNT                        (SDRAM_BASE_ADDR + 0x14)

/**************************************************************************
     Definition of DISC registers
**************************************************/
#define DISC_BASE_ADDR                            0x3B000000

#define R_DISC_VID_CMD                            (DISC_BASE_ADDR + 0x00)
#define M_CMD_HF_EN                               0x1
#define M_CMD_VF_EN                               0x10
#define M_CMD_BPIC_EN                             0x100
#define M_CMD_SPIC_EN                             0x1000
#define M_CMD_SPIC_DISP_MODE                      0x10000
#define M_CMD_VF_IN_FRAME                         0x100000
#define M_CMD_VID_DISPLAY                         0x1000000
#define M_VIDCMD_AUTO_CHSWITCH                    0x10000000
#define R_DISC_RATIO                              (DISC_BASE_ADDR + 0x04)
#define M_RATIO_H_INT                             0xF
#define M_RATIO_H_FRA                             0xFFF0
#define S_RATIO_H_FRA                             4
#define S_RATIO_V_INT                             16
#define S_RATIO_V_FRA                             20
#define R_SOURCE_LINE                             (DISC_BASE_ADDR + 0x08)
#define S_SRCLINE_START_LINE                      1
#define M_SRCLINE_START_LINE                      0x3FE
#define M_SRCLINE_HTABLE                          0x30000
#define S_SRCLINE_HTABLE                          16
#define M_SRCLINE_VTABLE                          0x100000
#define S_SRCLINE_VTABLE                          20
#define M_SRCLINE_VDISP_FIELD                     0x30000000
#define S_SRCLINE_VDISP_FIELD                     28
#define M_SRCLINE_HF_CANCLE                       0x1000000
#define R_DISC_SRC_PIXEL                          (DISC_BASE_ADDR + 0x0C)
#define S_SRC_PXL_END                             16
#define R_DISC_WINDOW_X                           (DISC_BASE_ADDR + 0x10)
#define R_DISC_WINDOW_Y                           (DISC_BASE_ADDR + 0x14)
#define S_DISC_WINDOW_R                           16
#define R_DISC_WINDOW_CUT                         (DISC_BASE_ADDR + 0x18)
#define S_DISC_WINCUT_RIGHT                       8
#define S_DISC_WINCUT_BOT                         16
#define S_DISC_WINCUT_TOP                         24
#define R_DISC_BPIC_COLOR                         (DISC_BASE_ADDR + 0x1C)
#define M_DSC_BPIC_SPIC_ENDIAN                    0x10000000
#define R_DISC_SPIC_POSX                          (DISC_BASE_ADDR + 0x20)
#define M_SPIC_POSX_START                         0x3FF
#define M_SPIC_POSX_END                           0x3FF0000
#define S_SPIC_POSX_END                           16
#define R_DISC_SPIC_POSY                          (DISC_BASE_ADDR + 0x24)
#define M_SPIC_POSY_START                         0x3FF
#define M_SPIC_POSY_END                           0x3FF0000
#define S_SPIC_POSY_END                           16
#define R_OSD_CMD                                 (DISC_BASE_ADDR + 0x28)
#define M_OSD_CMD_EN                              0x1
#define M_OSD_FILT_PARAM                          0x7000000
#define S_OSD_FILT_PARAM                          24
#define M_OSD_CMD_PALPHA_EN                       0x100
#define M_OSD_CMD_PALPHA                          0xFF0000
#define S_OSD_CMD_PALPHA                          16
                                                
#define R_SUB_CMD                                 0x3B00002C
#define M_SUB_CMD_EN                              0x1
#define M_SUB_CMD_PALPHA_EN                       0x100
#define M_SUB_CMD_PALPHA                          0xFF0000
#define S_SUB_CMD_PALPHA                          16

#define R_DISC_BACK_COLOR                         (DISC_BASE_ADDR + 0x1c)
                                               
#define R_OSD_EDGE_WEIGHT                         (DISC_BASE_ADDR + 0x30)
#define M_OSD_EDGE_WEIGHT                         0xFFFF
#define S_OSD_BOT_EDGE_WEIGHT                     8
                                               
#define R_DISC_CHS_VID_X                          (DISC_BASE_ADDR + 0x34)
#define S_CHS_VID_X_RIGHT                         16
#define R_DISC_CHS_VID_Y                          (DISC_BASE_ADDR + 0x38)
#define S_CHS_VID_Y_END                           16
                                                  
#define R_DISC_VFCHS_RATIO                        (DISC_BASE_ADDR + 0x3C)
#define S_CHS_VRATIO_NUM                          16
#define S_CHS_VRATIO_FRA                          4
                                                  
#define R_DISC_RENEW_FLAG                         (DISC_BASE_ADDR + 0x40)
#define M_RENEW_CHS_VF                            0x1
#define M_RENEW_FSCRN_VF                          0x100
#define M_RENEW_CHS_HF                            0x100000
#define M_RENEW_FSCRN_HF                          0x1000000
                                                  
#define R_DISC_HFCHS_RATIO                        (DISC_BASE_ADDR + 0x44)
#define S_CHS_HRATIO_NUM                          16
#define S_CHS_HRATIO_FRA                          4
#define R_DISC_HFFSCRN_RATIO                      (DISC_BASE_ADDR + 0x48)
#define S_FSCRN_HRATIO_NUM                        16
#define S_FSCRN_HRATIO_FRA                        4
                                                  
#define R_DISC_HFCHS_RAM0                         (DISC_BASE_ADDR + 0x4C)
#define R_DISC_HFCHS_RAM1                         (DISC_BASE_ADDR + 0x50)
#define R_DISC_HFCHS_RAM2                         (DISC_BASE_ADDR + 0x54)
#define R_DISC_HFCHS_RAM3                         (DISC_BASE_ADDR + 0x58)
#define R_DISC_HFCHS_RAM4                         (DISC_BASE_ADDR + 0x5C)
#define R_DISC_HFCHS_RAM5                         (DISC_BASE_ADDR + 0x60)
#define R_DISC_HFFSCRN_RAM0                       (DISC_BASE_ADDR + 0x64)
#define R_DISC_HFFSCRN_RAM1                       (DISC_BASE_ADDR + 0x68)
#define R_DISC_HFFSCRN_RAM2                       (DISC_BASE_ADDR + 0x6C)
#define R_DISC_HFFSCRN_RAM3                       (DISC_BASE_ADDR + 0x70)
                                                  
#define R_DISC_VFFSCRN_RATIO                      (DISC_BASE_ADDR + 0x74)
#define S_FSCRN_VRATIO_NUM                        16
#define S_FSCRN_VRATIO_FRA                        4
                                               
#define R_DISC_VFCHS_RAM0                         (DISC_BASE_ADDR + 0x78)
#define R_DISC_VFCHS_RAM1                         (DISC_BASE_ADDR + 0x7C)
#define R_DISC_VFFSCRN_RAM0                       (DISC_BASE_ADDR + 0x80)
#define R_DISC_VFFSCRN_RAM1                       (DISC_BASE_ADDR + 0x84)
                                                  
#define R_VID_DECOMPRESSION                       (DISC_BASE_ADDR + 0x8C)
#define M_VID_DECMPRS_EN                          0x1
#define M_VID_CMPRS_MODE                          0X10
                                               
#define R_OSD_COLOR_KEY                           (DISC_BASE_ADDR + 0x90)
#define R_DISC_VID_CFG                            (DISC_BASE_ADDR + 0x94)
#define M_CFG_SPIC_INTERVAL_H                     0xF
#define M_CFG_SPIC_INTERVAL_V                     0x1F0
#define S_CFG_SPIC_INTERVAL_V                     4
                                               
#define M_OSD_COLORKEY_EN                         0x1000000
#define M_OSD_COLORKEY_RGB                        0xFFFFFF
#define S_OSD_COLORKEY_G                          8
#define S_OSD_COLORKEY_B                          16
#define R_DISC_REG_CONFIG                         (DISC_BASE_ADDR + 0xA4)
#define M_DISC_REG_CFG_EN                         0x1
#define M_DISC_REG_CFG_PN                         0xE
#define S_DISC_REG_CFG_PN                         0x1
#define M_DISC_CFG_PROGRESSIVE                    0x80
#define M_DISC_REG_CFG_N2P_EN                     0x100
#define M_DISC_REG_CFG_P2N_EN                     0x200
#define R_DISC_VERT_CONFIG                        (DISC_BASE_ADDR + 0xA8)
#define M_DISC_CHROM_IND_EN                       0x20000000
#define R_DISC_SCALER_CONFIG                      (DISC_BASE_ADDR + 0xAC)
#define M_DISC_ADAPTIVE_MODE                      0x30000000
#define M_DISC_ADAPTIVE_CHROM                     0x40000000
#define M_DISC_D2_FACTOR                          0xF000
#define S_DISC_D2_FACTOR                          12
#define M_DISC_D2_SHIFT                           0xF00
#define S_DISC_D2_SHIFT                           8
#define M_DISC_D1_OFFSET                          0xF
#define M_DISC_EN_NO_704                          0x2
#define M_DISC_ADAPTIVE_EN                        0x1
#define R_DISC_REG_INFO                           (DISC_BASE_ADDR + 0xEC)


/**************************************************
      Definition of PTI registers
**************************************************/
#define PID_FILTER_BASE                           0x33000000
#define R_PTI_TS_CTRL                             (0x33000000+0x0480)
#define PTI_SLOT_BASE                             (PID_FILTER_BASE + 0x0)
#define PTI_FDATA0_BASE                           (PID_FILTER_BASE + (32 << 2))
#define PTI_FDATA1_BASE                           (PTI_FDATA0_BASE + (32 << 2))
#define PTI_FDATA2_BASE                           (PTI_FDATA1_BASE + (32 << 2))
#define PTI_FMASK0_BASE                           (PTI_FDATA2_BASE + (32 << 2))
#define PTI_FMASK1_BASE                           (PTI_FMASK0_BASE + (32 << 2))
#define PTI_FMASK2_BASE                           (PTI_FMASK1_BASE + (32 << 2))
#define PTI_FCONF_BASE                            (PTI_FMASK2_BASE + (32 << 2))
#define M_PTI_FILT_SECNUM                         0x1FF
#define S_PTI_FILT_SLOTNUM                        26
#define M_PTI_FILT_BUFLEN                         0x03FFFE00
#define S_PTI_FILT_BUFLEN                         9
#define PTI_FADDR_BASE                            (PTI_FCONF_BASE + (32 << 2))
#define M_PTI_FILT_ADDR                           0x03FFFFFF
#define S_PTI_FILT_CRCEN                          29
#define S_PTI_FILT_INT_FREQ                       29
#define M_PTI_SM_TCRK                             0x10000000
#define S_PTI_SM_TCRK                             28
#define S_PTI_TS_MODE_FILTERING                   27
#define M_PTI_CIRCLE_BUFFER                       0x4000000
#define S_PTI_CIRCLE_BUFFER                       26


#define REG_GLOBAL_BASE                           0x33000500   
#define R_PTI_GLOBAL                              (REG_GLOBAL_BASE + 0x0)
#define M_PTI_BYTE_MASK_EN                        0x10000000
#define M_PTI_TUNER_DATA_EN                       0x100
#define M_PTI_INPUT_CFG                           0x1
#define R_PTI_INT_VEC                             (REG_GLOBAL_BASE + 0x4)
#define R_PTI_RESSTA0                             (REG_GLOBAL_BASE + 0x8)
#define R_PTI_RESSTA1                             (REG_GLOBAL_BASE + 0xC)
                                                  

/**************************************************
      Definition of Address manager registers
**************************************************/
#define ADDRMNG_BASE_ADDR                         0x37100000
                                                  
#define R_TS_BUF_ADDR0                            (ADDRMNG_BASE_ADDR + 0x0)
#define M_TSBUF_ABUF_ADDR                         0x7FFF
#define M_TSBUF_VBUF_ADDR                         0x7FF0000
#define S_TSBUF_VBUF_ADDR                         16
#define R_TS_BUF_ADDR1                            (ADDRMNG_BASE_ADDR + 0x4)
#define M_TSBUF_ABUF_END                          0x7FF
#define M_TSBUF_VBUF_END                          0x7FF0000
#define S_TSBUF_VBUF_END                          16
#define R_TS_BUF_ADDR2                            (ADDRMNG_BASE_ADDR + 0x8)
#define M_TSBUF_VSIZE                             0x7FF
#define M_TSBUF_ASIZE                             0x1FF800
#define S_TSBUF_ASIZE                             11
#define M_TSBUF_VGUARD                            0xFFE00000
#define S_TSBUF_VGUARD                            21
#define R_TS_BUF_ADDR3                            (ADDRMNG_BASE_ADDR + 0xC)
#define S_TSBUF_AGUARD_Q                          10
#define M_TSBUF_AGUARD_N                          0x3FF
#define R_TAGBUF_ADDR0                            (ADDRMNG_BASE_ADDR + 0x10)
#define R_TAGBUF_ADDR1                            (ADDRMNG_BASE_ADDR + 0x14)
#define R_TAGBUF_ADDR2                            (ADDRMNG_BASE_ADDR + 0x18)
#define R_TAGBUF_ADDR3                            (ADDRMNG_BASE_ADDR + 0x1C)
#define M_TAGBUF_ROW_TOP                          0x7FFF
#define M_TAGBUF_ROW_BOT                          0x7FFF0000
#define S_TAGBUF_ROW_BOT                          16
#define R_VIDBUF_ADDR0                            (ADDRMNG_BASE_ADDR + 0x20)
#define R_VIDBUF_ADDR1                            (ADDRMNG_BASE_ADDR + 0x24)
#define R_VIDBUF_ADDR2                            (ADDRMNG_BASE_ADDR + 0x28)
#define R_VIDBUF_ADDR3                            (ADDRMNG_BASE_ADDR + 0x2C)
#define M_VIDBUF_ADDR_ROW                         0x7FFF
#define R_PIC9_BUF_ADDR_I                         (ADDRMNG_BASE_ADDR + 0x30)
#define R_PIC9_BUF_ADDR_P                         (ADDRMNG_BASE_ADDR + 0x34)
#define R_PIC9_BUF_ADDR_B                         (ADDRMNG_BASE_ADDR + 0x38)
#define M_PIC9BUF_ADDR_ROW                        0x7FFF
#define R_SPIC_ODD_LUM_ADDR                       (ADDRMNG_BASE_ADDR + 0x40)
#define R_SPIC_EVEN_LUM_ADDR                      (ADDRMNG_BASE_ADDR + 0x44)
#define M_SPIC_LUMA_ADDR                          0xFFFF
#define R_SPIC_CHROMA_ADDR                        (ADDRMNG_BASE_ADDR + 0x48)
#define M_SPIC_CHROMA_ADDR                        0xFFFF
#define R_OSD_BUF_ADDR                            (ADDRMNG_BASE_ADDR + 0x50)
#define R_SUB_BUF_ADDR                            (ADDRMNG_BASE_ADDR + 0x54)
#define R_ES_BUF_LIMIT                            (ADDRMNG_BASE_ADDR + 0x60)
#define M_ESBUF_LIMIT_L                           0x3FFF
#define M_ESBUF_LIMIT_H                           0x3FFF0000
#define S_ESBUF_LIMIT_H                           16
#define R_CA_ADDR_LIMITKB                         (ADDRMNG_BASE_ADDR + 0x64)
#define S_PTI_HADDR_LIMITKB                       16
#define M_PTI_HADDR_LIMITKB                       0x3FFF0000
#define M_PTI_LADDR_LIMITKB                       0x3FFF
#define R_VID_BUF_LIMIT                           (ADDRMNG_BASE_ADDR + 0x68)
#define M_VIDBUF_LIMIT_H                          0x3FFF0000
#define S_VIDBUF_LIMIT_H                          16
#define M_VIDBUF_LIMIT_L                          0x3FFF
#define R_ADDRMANAGE_DMA_LIMIT                    (ADDRMNG_BASE_ADDR + 0x6C)
#define R_ADDRMANAGE_SDRAM_PROTECT                (ADDRMNG_BASE_ADDR + 0x74)
#define R_PIC9_BUF_LIMIT                          (ADDRMNG_BASE_ADDR + 0x78)
#define M_PIC9BUF_LIMIT_H                         0x3FFF0000
#define S_PIC9BUF_LIMIT_H                         16
#define M_PIC9BUF_LIMIT_L                         0x3FFF
#define R_TAG_BUF_LIMIT                           (ADDRMNG_BASE_ADDR + 0x7C)
#define M_TAGBUF_LIMIT_L                          0x3FFF
#define M_TAGBUF_LIMIT_H                          0x3FFF0000
#define S_TAGBUF_LIMIT_H                          16
#define R_AES_CTRL_ADDR                           (ADDRMNG_BASE_ADDR + 0x80)
#define R_VES_CTRL_ADDR                           (ADDRMNG_BASE_ADDR + 0x84)
#define R_VID_LINEAR_ADDR                         (ADDRMNG_BASE_ADDR + 0x90)
#define M_LINEAR_WIDTH_LUMA                       0xFFC00000
#define S_LINEAR_WIDTH_LUMA                       22
#define M_LINEAR_WIDTH_CHR                        0xFF80
#define S_LINEAR_WIDTH_CHR                        7
#define M_LINEAR_ADDR_MODE                        0x1


/**************************************************
      Definition of reset registers
**************************************************/
#define RESET_BASE_ADDR                           0x34000000
#define R_RESET_MODE                              (RESET_BASE_ADDR + 0x0)
#define M_AV_RST_MODE                             0x1
#define M_AOUT_RST_MODE                           0x8
#define R_CHANNEL_SWT_RST                         (RESET_BASE_ADDR + 0x4)
#define M_CHANNEL_RST_MODE                        0x01
#define R_HBUSREQ_MASK                            (RESET_BASE_ADDR + 0xC)
#define M_HBUSREQ_ESBUF_REQ                       0xC0
#define M_HBUSREQ_CATS                            0x3000 
#define R_HBUSREQ_CTRL                            (RESET_BASE_ADDR + 0x14)
#define R_SYNC_RESET_MODE                         (RESET_BASE_ADDR + 0x18)
#define M_DECODE_ONEFRAME_RST                     0x1
#define R_HBUSREQ_MONITOR0                        (RESET_BASE_ADDR + 0x1C)
#define R_CPU_LATENCY                             (RESET_BASE_ADDR + 0xDC)
#define R_PROTECT_ADDR_LOW                        (RESET_BASE_ADDR + 0xE0)
#define R_PROTECT_ADDR_HIGH                       (RESET_BASE_ADDR + 0xE4)
#define R_VIO_HMASTER                             (RESET_BASE_ADDR + 0xE8)


/**************************************************
      Definition of CA top registers
**************************************************/
#define CA_TOP_BASE_ADDR                          0x36000000
#define R_TSD_REG0                                (CA_TOP_BASE_ADDR + 0x0)
#define M_MARKER_BIT_MASK                         0x6
#define M_TSD0_STARTPIC_MODE                      0x100
#define R_TSD_REG1                                (CA_TOP_BASE_ADDR + 0x4)
#define M_TSD_REG1_INPUT                          0x1
#define M_TSD1_ES_MODE                            0x101
#define R_TSD_REG2                                (CA_TOP_BASE_ADDR + 0x8)
#define R_TSD_REG3                                (CA_TOP_BASE_ADDR + 0xC)


/**************************************************
      Definition of PINMUX registers
**************************************************/
#define PINMUX_BASE_ADDR                          0x3C000000
#define PINMUX_MUX_SEL                            (PINMUX_BASE_ADDR + 0x0)
#define R_PINMUX_PIN_SEL                          (PINMUX_BASE_ADDR + 0x0)
#define M_PINMUX_SEL                              0x7FFFFFFF
#define M_PINMUX_I2S_EN                           0x2
#define S_PINMUX_SEL_CLKOBS                       0
#define S_PINMUX_SEL_I2S                          1
#define S_PINMUX_SEL_656                          2
#define S_PINMUX_SEL_UART1                        3
#define S_PINMUX_SEL_I2C_MASTER                   4
#define S_PINMUX_SEL_AGC                          5
#define S_PINMUX_SEL_OBSVRST                      6
#define S_PINMUX_SEL_SMCMDVCC                     7
#define S_PINMUX_SEL_GPIO(x)                     ((x>=12) ? (x-4) : 0)
#define M_SMCMDVCC_EN                             0x80


/**************************************************
      Definition of Avsync_ctrl registers
**************************************************/
#define AVSYNC_CTRL_BASE_ADDR                     0x39000000
#define R_AVSYNC_REG0                             (AVSYNC_CTRL_BASE_ADDR + 0x0)
#define R_AVSYNC_REG1                             (AVSYNC_CTRL_BASE_ADDR + 0x4)
#define R_AVSYNC_REG2                             (AVSYNC_CTRL_BASE_ADDR + 0x8)
#define R_AVSYNC_REG3                             (AVSYNC_CTRL_BASE_ADDR + 0xC)
#define R_AVSYNC_REG4                             (AVSYNC_CTRL_BASE_ADDR + 0x10)
#define R_AVSYNC_REG5                             (AVSYNC_CTRL_BASE_ADDR + 0x14)
#define R_AVSYNC_REG6                             (AVSYNC_CTRL_BASE_ADDR + 0x18)
#define R_AVSYNC_REG7                             (AVSYNC_CTRL_BASE_ADDR + 0x1C)
#define R_AVSYNC_REG8                             (AVSYNC_CTRL_BASE_ADDR + 0x20)
#define M_AVSYNC_REG8_AUTOSYNC                    0x1
#define R_AVSYNC_REG9                             (AVSYNC_CTRL_BASE_ADDR + 0x24)
#define M_AVSYNC_REG9_A_SYNC                      0x1
#define R_AVSYNC_REG10                            (AVSYNC_CTRL_BASE_ADDR + 0x28)
#define M_AVSYNC_REG10_V_SYNC                     0x1
#define R_SEL_REQ_MODE_REG                        (AVSYNC_CTRL_BASE_ADDR + 0x2C)
#define R_AVSYNC_CLOCK_REG                        (AVSYNC_CTRL_BASE_ADDR + 0x30)
#define R_AVSYNC_ES_MODE                          (AVSYNC_CTRL_BASE_ADDR + 0x34)
#define R_AVSYNC_ES_MODE1                         (AVSYNC_CTRL_BASE_ADDR + 0x38)
/*******************************************************************
 * Defination of TIMER Register
 *******************************************************************/
#define TIMER_BASE_ADDR                           0x31000000
#define R_TIMER0_INIT_DATA                        (TIMER_BASE_ADDR+0x18)
#define R_TIMER1_INIT_DATA                        (TIMER_BASE_ADDR+0x1c)
#define R_TIMER2_INIT_DATA                        (TIMER_BASE_ADDR+0x28)
#define R_TIMER3_INIT_DATA                        (TIMER_BASE_ADDR+0x2c)
#define R_TIMER0_CUR_DATA                         (TIMER_BASE_ADDR+0x30)
#define R_TIMER1_CUR_DATA                         (TIMER_BASE_ADDR+0x34)
#define R_TIMER2_CUR_DATA                         (TIMER_BASE_ADDR+0x38)
#define R_TIMER3_CUR_DATA                         (TIMER_BASE_ADDR+0x3c)
#define R_TIMER_TCCW                              (TIMER_BASE_ADDR+0x24)

/**************************************************
      Definition of Audio registers
**************************************************/
#define AUDIO_BASE_ADDR                           0x38000000
#define R_AUDIO_PCMMODE                           (AUDIO_BASE_ADDR + 0x0) 
#define M_AUDIO_PCMMODE_FREQ                      0x70
#define M_AUDIO_PCMMODE_ENDIAN                    0x8
#define M_AUDIO_PCMMODE_STEREO                    0x4
#define M_AUDIO_PCMMODE_PCMEN                     0x2
#define M_AUDIO_PCMMODE_BOOTEN                    0x1
#define R_AUDIO_DIF_PARAM                         (AUDIO_BASE_ADDR + 0x4)
#define M_AUDIO_DIF_I2S_CLK                       0x40000000
#define M_AUDIO_DIF_ZERO_NUM                      0x3F
#define M_AUDIO_DIF_PCM_NUM                       0xFC0
#define S_AUDIO_DIF_PCM_NUM                       6
#define M_AUDIO_DIF_RIGTH_CH                      0x1000
#define M_AUDIO_DIF_JUSTIFY                       0x6000
#define M_AUDIO_DIF_RREQ_MPEG                     0x8000
#define M_AUDIO_DIF_SEL_SPDIF                     0x80000000
#define M_AUDIO_DIF_SPDIF_MODE                    0x20000
#define R_AUDIO_VOLUME_SET                        (AUDIO_BASE_ADDR + 0x8)
#define M_AUDIO_LRCOPY_L2R                        0x08000000
#define M_AUDIO_LRCOPY_R2L                        0x04000000
#define M_AUDIO_RCH_OFF                           0x01000000
#define M_AUDIO_LCH_OFF                           0x02000000
#define M_AUDIO_VOLUME_DB                         0xFFFF
#define S_AUDIO_VOL_CHNNEL                        24
#define R_AUDIO_MUTE_SET                          (AUDIO_BASE_ADDR + 0xC)
#define M_AUDIO_MUTE_FRAME_NUM                    0xFF
#define M_AUDIO_MUTE_SET_MUTE                     0x100
#define R_AUDIO_OUTPUT_CLK_SET                    (AUDIO_BASE_ADDR + 0x10)
#define R_AUDIO_PPSET                             (AUDIO_BASE_ADDR + 0x14)
#define M_AUDIO_PPSET_VSR_EN                      0x40000000
#define S_AUDIO_PPSET_VSR_EN                      30
#define M_AUDIO_PPSET_VSR_CENTER                  0x1C0000
#define S_AUDIO_PPSET_VSR_CENTER                  18
#define M_AUDIO_PPSET_VSR_SPACE                   0x38000
#define S_AUDIO_PPSET_VSR_SPACE                   15

/*******************************************************************
      Definition of Query registers
*******************************************************************/
#define QUERY_BASE_ADDR                           0x3E000000
#define R_QUERY_REG0                              (QUERY_BASE_ADDR  + 0x0)
#define M_QUERY0_AES_CNT                          0xF0000
#define S_QUERY0_AES_CNT                          16
#define M_QUERY0_VES_CNT                          0xF00000
#define S_QUERY0_VES_CNT                          20
#define R_QUERY_REG10                             (QUERY_BASE_ADDR  + 0x10)
#define M_QUERY10_AS_INFO                         0xF00000
#define S_QUERY10_AS_INFO                         20
#define R_QUERY_REG14                             (QUERY_BASE_ADDR  + 0x14)
#define M_QUERY14_VES_KB                          0xFFFF
#define R_QUERY_REG18                             (QUERY_BASE_ADDR  + 0x18)
#define M_QUERY18_VFMT_DEC                        0x38000000
#define S_QUERY18_VFMT_DEC                        27
#define R_QUERY_REG1C                             (QUERY_BASE_ADDR  + 0x1C)
#define M_QUERY1C_VOUT_BANK                       0xC0
#define S_QUERY1C_VOUT_BANK                       6
#define M_QUERY1C_FRATE                           0xF
#define M_QUERY1C_MB_H                            0x3F0000
#define S_QUERY1C_MB_H                            16
#define M_QUERY1C_MB_W                            0x3F00
#define S_QUERY1C_MB_W                            8
#define M_QUERY1C_PIC_TYPE                        0x7000000
#define S_QUERY1C_PIC_TYPE                        24
#define R_QUERY_REG6C                             (QUERY_BASE_ADDR + 0x6C)
#define M_QUERY6C_DEC_FNUM                        0xFFFF
#define R_QUERY_REG90                             (QUERY_BASE_ADDR + 0x90)
#define M_QUERY90_SIZE_H                          0x3FFF0000
#define S_QUERY90_SIZE_H                          16
#define M_QUERY90_SIZE_V                          0x3FFF
#define R_QUERY_PC                                (QUERY_BASE_ADDR  + 0x94)
#define S_QUERY_REG14_ES_ADATA                    16

/*******************************************************************
      Definition of video encoder registers
*******************************************************************/
#define VID_ENCODER_BASE_ADDR                     0x35000000
#define R_VENC_REG0                               (VID_ENCODER_BASE_ADDR + 0x0)
#define M_RGB_MODE                                0x100000
#define R_VENC_REG1                               (VID_ENCODER_BASE_ADDR + 0x4)
#define R_VENC_REG2                               (VID_ENCODER_BASE_ADDR + 0x8)
#define R_VENC_REG4                               (VID_ENCODER_BASE_ADDR + 0x10)
#define M_VENC2_CVBS_DELAY                        0xFF0000
#define S_VENC2_CVBS_DELAY                        16
#define R_VENC_REG5                               (VID_ENCODER_BASE_ADDR + 0x14)
#define M_VID_YG_SYNC_EN                          0x1
#define M_VID_PAL_M                               0x80000000
#define M_VID_MODE_DAC0                           0x70000
#define S_VID_MODE_DAC0                           16
#define M_VID_MODE_DAC1                           0x380000
#define S_VID_MODE_DAC1                           19
#define M_VID_MODE_DAC2                           0x1C00000
#define S_VID_MODE_DAC2                           22
#define M_VID_MODE_DAC3                           0xE000000
#define S_VID_MODE_DAC3                           25
#define R_VENC_REG6                               (VID_ENCODER_BASE_ADDR + 0x18)
#define R_VENC_REG7                               (VID_ENCODER_BASE_ADDR + 0x1C)
#define R_VENC_REG8                               (VID_ENCODER_BASE_ADDR + 0x20)
#define R_VENC_REG9                               (VID_ENCODER_BASE_ADDR + 0x24)
#define R_VENC_REG10                              (VID_ENCODER_BASE_ADDR + 0x28)
#define R_VENC_REG14                              (VID_ENCODER_BASE_ADDR + 0x38)
#define R_VENC_REG15                              (VID_ENCODER_BASE_ADDR + 0x3C)
#define M_VENC15_IRE_7P5_EN                       0x10000
#define R_VENC_REG16                              (VID_ENCODER_BASE_ADDR + 0x40)

/*******************************************************************
 * Definition of UART register
 *******************************************************************/
#define UART_BASE_ADDR                            0x41000000
#define R_UART_BAUD                               (UART_BASE_ADDR + 0x0)
#define R_UART_CONCTL                             (UART_BASE_ADDR + 0x4)
#define R_UART_DATA                               (UART_BASE_ADDR + 0x8)
#define R_UART_STATUS                             (UART_BASE_ADDR + 0xC)

#define UART2_BASE_ADDR                           0x4E000000
#define R_UART2_BAUD                              (UART2_BASE_ADDR + 0x0)
#define R_UART2_CONCTL                            (UART2_BASE_ADDR + 0x4)
#define R_UART2_DATA                              (UART2_BASE_ADDR + 0x8)
#define R_UART2_STATUS                            (UART2_BASE_ADDR + 0xC)

#define M_UART_CR_RXCIE                           0x80
#define M_UART_CR_TXCIE                           0x40
#define M_UART_CR_UDRIE                           0x20
#define M_UART_CR_RXEN                            0x10
#define M_UART_CR_TXEN                            0x08
#define M_UART_CR_CHR9                            0x04
#define M_UART_CR_RXB8                            0x02
#define M_UART_CR_TXB8                            0x01
#define M_UART_SR_RXC		                          0x80
#define M_UART_SR_TXC		                          0x40
#define M_UART_SR_UDRE	                          0x20
#define M_UART_SR_FE		                          0x10

/*****************************************************************
 * Definition of GPIO register
 *******************************************************************/
#define  GPIO_BASE_ADDR                           0x32000000
#define  R_GPIO0_W_DATA_REG                       (GPIO_BASE_ADDR+0x00)
#define  R_GPIO0_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x04)
#define  R_GPIO0_R_DATA_REG                       (GPIO_BASE_ADDR+0x08)
#define  R_GPIO0_MASK_REG                         (GPIO_BASE_ADDR+0x0C)
#define  R_GPIO1_W_DATA_REG                       (GPIO_BASE_ADDR+0x10)
#define  R_GPIO1_W_DATA_EN_REG                    (GPIO_BASE_ADDR+0x14)
#define  R_GPIO1_R_DATA_REG                       (GPIO_BASE_ADDR+0x18)
#define  R_GPIO1_MASK_REG                         (GPIO_BASE_ADDR+0x1C)

/*******************************************************************
 * Definition of IRDA register
 *******************************************************************/
#define IRDA_BASE_ADDR                            0x43000000
#define R_IRDA_DATA                               (IRDA_BASE_ADDR + 0x0)
#define R_IRDA_USERL                              (IRDA_BASE_ADDR + 0x4)
#define R_IRDA_USERH                              (IRDA_BASE_ADDR + 0x8)
#define R_IRDA_RATE                               (IRDA_BASE_ADDR + 0xC)
#define R_IRDA_USRMASKL                           (IRDA_BASE_ADDR + 0x10)
#define R_IRDA_USRMASKH                           (IRDA_BASE_ADDR + 0x14)
#define R_IRDA_CTL                                (IRDA_BASE_ADDR + 0x18)
#define R_IRDA_CCFG                               (IRDA_BASE_ADDR + 0x1C)
#define R_IRDA_STAT                               (IRDA_BASE_ADDR + 0x20)
#define R_IRDA_OTSET                              (IRDA_BASE_ADDR + 0x24)
#define R_IRDA_NECSTA                             (IRDA_BASE_ADDR + 0x38)

/*******************************************************************
 * Definition of FrontPanel register
 *******************************************************************/
#define FP_BASE_ADDR                              0x44000000
#define R_FP_PINCFG                               (FP_BASE_ADDR + 0x00)
#define R_FP_LEDCTL                               (FP_BASE_ADDR + 0x04)
#define R_FP_KBTIME                               (FP_BASE_ADDR + 0x08)
#define R_FP_H164CLK                              (FP_BASE_ADDR + 0x0C)
#define R_FP_LEDSEG                               (FP_BASE_ADDR + 0x10)
#define R_FP_KEY_OUT                              (FP_BASE_ADDR + 0x10)
#define R_FP_KEY_IN                               (FP_BASE_ADDR + 0x14)
#define R_FP_KEY_IN2                              (FP_BASE_ADDR + 0x18)
#define R_FP_KEY_STA                              (FP_BASE_ADDR + 0x1C)
#define R_FP_LEDDATA(x)                           ((FP_BASE_ADDR+0x14+((x)*4)))



/************************************************************************
 * Definition of Global info register
 ************************************************************************/
#define GLOBAL_INFO_BASE_ADDR                     0x3F000000
#define R_GLOBAL_INFO_MASK                        GLOBAL_INFO_BASE_ADDR
#define M_GLB_INFO_DEC_ONE_FRAME                  0x4000000
#define M_GLB_INFO_ENC_FIELD_NUM                  0x3800
#define R_GLOBAL_INFO                             (GLOBAL_INFO_BASE_ADDR + 0x04)
#define M_GLB_INFO_VID_FORMAT                     0x7
#define M_GLB_INFO_CHROMA_FORMAT                  0x18
#define M_GLB_INFO_PIC_SIZE_ERROR                 0x1000000
#define R_GLOBAL_INFO_CTRL                        (GLOBAL_INFO_BASE_ADDR + 0x08)
#define R_GLOBAL_INFO_STATE                       (GLOBAL_INFO_BASE_ADDR + 0x0C)


/*******************************************************************
 * Definition of IRQ register
 *******************************************************************/
#define MPEG2_VDEC_BASE_ADDR                      0x37000000
#define R_MPEG2_VDEC_REG0                         (MPEG2_VDEC_BASE_ADDR + 0x0)
#define M_VDEC_START_MODE                         0x8000
#define M_PHASE_MODE                              0x800000
#define M_BUFFER_MODE                             0x80000000
#define R_MPEG2_VDEC_REG1                         (MPEG2_VDEC_BASE_ADDR + 0x4)
#define M_VC_AUTO_MODE                            0x10000000
#define M_VDEC_PAUSE_MODE                         0x80000000
#define M_VDEC_FREEZE_EN                          0x40000000
#define M_VDEC_FREEZE_STA                         0x20000000
#define R_MPEG2_VDEC_REG3                         (MPEG2_VDEC_BASE_ADDR + 0xC)
#define R_VIDEO_COMPRESSION                       (MPEG2_VDEC_BASE_ADDR + 0x10)
#define M_COMPRESSION_EN                          0x1
#define M_COMPRESSION_MODE                        0x4
#define M_LUMA_TAG_IN_PIXEL                       0x80
#define R_PIC_9_REG                               (MPEG2_VDEC_BASE_ADDR + 0x14)
#define M_PIC9_MODE_EN                            0x1
#define M_PIC9_Y_RATIO                            0xF00
#define S_PIC9_Y_RATIO                            8
#define M_PIC9_X_RATIO                            0xF000
#define S_PIC9_X_RATIO                            12
#define M_PIC9_FIFO_AFULL_LEVEL                   0x3F0000
#define S_PIC9_FIFO_AFULL_LEVEL                   16
#define M_PIC9_DISP_FNUM                          0xC0000000
#define S_PIC9_DISP_FNUM                          30

/*******************************************************************
 * Definition of PAL/NTSC conv register
 *******************************************************************/
#define PN_CONV_BASE_ADDR                         0x3A000000
#define R_PIC_PN_SET                              (PN_CONV_BASE_ADDR + 0x0)
#define M_PIC_PN_MODE                             0x3



/*******************************************************************
 * Definition of smart card register
 *******************************************************************/
#define SMC_BASE_ADDR                             0x4F000000
                                          
#define R_SMC_CONV                                (SMC_BASE_ADDR + 0x0)
#define R_SMC_PARITY                              (SMC_BASE_ADDR + 0x4)
#define R_SMC_TXRETRY                             (SMC_BASE_ADDR + 0x8)
#define R_SMC_RXRETRY                             (SMC_BASE_ADDR + 0xC)
#define R_SMC_TXTIDE                              (SMC_BASE_ADDR + 0x10)
#define R_SMC_TXCOUNT                             (SMC_BASE_ADDR + 0x14)
#define R_SMC_RXTIDE                              (SMC_BASE_ADDR + 0x18)
#define R_SMC_RXCOUNT                             (SMC_BASE_ADDR + 0x1C)
#define R_SMC_RXTIME                              (SMC_BASE_ADDR + 0x20)
#define R_SMC_TCTRL                               (SMC_BASE_ADDR + 0x24)
#define R_SMC_STABLE                              (SMC_BASE_ADDR + 0x28)
#define R_SMC_ICTRL                               (SMC_BASE_ADDR + 0x2C)
#define R_SMC_ISTAT                               (SMC_BASE_ADDR + 0x30)
#define R_SMC_ATIME                               (SMC_BASE_ADDR + 0x34)
#define R_SMC_DTIME                               (SMC_BASE_ADDR + 0x38)
#define R_SMC_ATR_STIME                           (SMC_BASE_ADDR + 0x3C)
#define R_SMC_ATR_DTIME                           (SMC_BASE_ADDR + 0x40)
#define R_SMC_BLKTIME                             (SMC_BASE_ADDR + 0x44)
#define R_SMC_CHTIME                              (SMC_BASE_ADDR + 0x48)
#define R_SMC_CLKDIV                              (SMC_BASE_ADDR + 0x4C)
#define R_SMC_FD                                  (SMC_BASE_ADDR + 0x50)
#define R_SMC_CONFIG                              (SMC_BASE_ADDR + 0x54)
#define R_SMC_CHGUARD                             (SMC_BASE_ADDR + 0x58)
#define R_SMC_BLKGUARD                            (SMC_BASE_ADDR + 0x5C)
#define R_SMC_RESET                               (SMC_BASE_ADDR + 0x60)
#define R_SMC_RAWSTAT                             (SMC_BASE_ADDR + 0x64)
#define R_SMC_INTEN                               (SMC_BASE_ADDR + 0x68)
#define R_SMC_INTSTAT                             (SMC_BASE_ADDR + 0x6C)
#define R_SMC_INQSTAT                             (SMC_BASE_ADDR + 0x70)
#define R_SMC_DATA                                (SMC_BASE_ADDR + 0x100)


#endif //__SYS_REGS_MAGIC_H__

