/**
  Copyright (C) 2008 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA10024local.h
 *                %version: CFR_FEAP#28 %
 *
 * \date          %date_modified%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 * Detailled description may be added here.
 *
 * \section info Change Information
 *
 * \verbatim
   Date          Modified by CRPRNr  TASKNr  Maintenance description
   -------------|-----------|-------|-------|-----------------------------------
    JULY-2007   | A.TANT    |       |       | CREATION OF TDA10024 ARCHITECTURE 2.0.0
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/

#ifndef _TMBSLTDA10024LOCAL_H //-----------------
#define _TMBSLTDA10024LOCAL_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       MACRO DEFINITION                                     */
/*============================================================================*/
#define TDA10024_BSL_COMP_NUM       2
#define TDA10024_BSL_MAJOR_VER      0
#define TDA10024_BSL_MINOR_VER      22

#define TDA10024_MUTEX_TIMEOUT          TMBSL_FRONTEND_MUTEX_TIMEOUT_INFINITE


#define POBJ_SRVFUNC_SIO pObj->sRWFunc
#define POBJ_SRVFUNC_STIME pObj->sTime
#define P_DBGPRINTEx pObj->sDebug.Print
#define P_DBGPRINTVALID ((pObj != Null) && (pObj->sDebug.Print != Null))

/* Status of the carrier phase lock loop */
typedef enum _tmbslTDA10024State_t
{
    /** status Unknown */
    tmbslTDA10024StateUnknown = 0,
    /** Channel locked*/
    tmbslTDA10024StateLocked,
    /** Channel not locked */
    tmbslTDA10024StateNotLocked,
    /** Channel lock in process */
    tmbslTDA10024StateSearching,
    /** Channel is synchronised */
    tmbslTDA10024StateSynchronised,
    /** Channel is not synchronised */
    tmbslTDA10024StateNotSynchronised,
    tmbslTDA10024StateMax
} tmbslTDA10024State_t;

/* Alexandre Tant*/
typedef enum _tmbslTDA10024AlgoState_t
{
    tmbslTDA10024AlgoStateInit = 0,
    tmbslTDA10024AlgoStateGainManagement,
    tmbslTDA10024AlgoStateEqualizerConvergence,
    tmbslTDA10024AlgoStateAutoChannel,
    tmbslTDA10024AlgoStateLockVerif,
    tmbslTDA10024AlgoStateStuffingIssue
} tmbslTDA10024AlgoState_t;

/*---------*/
/* CHANNEL */
/*---------*/
#define TDA10024_MAX_UNITS              4

#define TDA10024_JQAMFILTER_NB          15
#define TDA10024_JQAMFILTER_ADDR_OFFSET 0x04

/*-----------------------------------------------*/
/*  DEFAULT VALUES FOR CONFIGURATION MANAGEMENT  */
/*-----------------------------------------------*/
#define OM57XX_XTALL_DEF            28920000
#define OM57XX_MODE_DVB_DEF         0
#define OM57XX_MODE_MCNS_DEF        1
#define OM57XX_PLLMFACTOR_DVB_DEF   0x07
#define OM57XX_PLLNFACTOR_DVB_DEF   0x00
#define OM57XX_PLLPFACTOR_DVB_DEF   0x03
#define OM57XX_PLLMFACTOR_MCNS_DEF  0x11
#define OM57XX_PLLNFACTOR_MCNS_DEF  0x00
#define OM57XX_PLLPFACTOR_MCNS_DEF  0x03
#define OM57XX_FSAMPLING_DEF        1
#define OM57XX_POLAPWM1_DEF         0
#define OM57XX_POLAPWM2_DEF         0
#define OM57XX_IFMAX_DEF            0xff
#define OM57XX_IFMIN_DEF            0x96
#define OM57XX_TUNMAX_DEF           0xff
#define OM57XX_TUNMIN_DEF           0x00
#define OM57XX_AGCTRESHOLD_DEF      255
#define OM57XX_EQUALTYPE_DEF        1    /* 1 for linear transversal, 2 for dfe */ 
#define OM57XX_BANDTYPE_DEF         2    /* 1: band type other , 2: band type SECAM L */
#define OM57XX_EQCONF1_DEF          0x70
#define OM57XX_CAR_C_DEF            0x00
#define OM57XX_GAIN1_DEF            0x80
#define OM57XX_BERDEPTH_DVB_DEF     2
#define OM57XX_BERDEPTH_MCNS_DEF    1
#define OM57XX_BERWINDOW_DEF        8           /* Depth of the sliding window used in MCNS to calculate the BER (possible values: 8, 64, 512, 4096) */
#define OM57XX_CLKOFFSETRANGE_DEF   0
#define OM57XX_IQSWAP_DEF           1
#define OM57XX_IF_DVB_DEF           5000000
#define OM57XX_IF_MCNS_DEF          43750000
#define OM57XX_OCLK1_DEF            1
#define OM57XX_PARASER1_DEF         0
#define OM57XX_MSBFIRST1_DEF        0
#define OM57XX_MODEABC1_DEF         0
#define OM57XX_PARADIV1_DEF         0
#define OM57XX_SYNCWIDTH1_DEF       1
#define OM57XX_DENP1_DEF            0
#define OM57XX_UNCORP1_DEF          0
#define OM57XX_SYNCP1_DEF           0
#define OM57XX_OCLK2_DEF            1
#define OM57XX_MSBFIRST2_DEF        0
#define OM57XX_SYNCWIDTH2_DEF       1
#define OM57XX_DENP2_DEF            0
#define OM57XX_UNCORP2_DEF          0
#define OM57XX_SYNCP2_DEF           0
#define OM57XX_SWPOS_DEF            1
#define OM57XX_SWDYN_DEF            1
#define OM57XX_SWSTEP_DEF           1
#define OM57XX_SWLENGTH_DEF         2

#define OM5763_FSAMPLING_DEF        0
#define OM5763_IF_DVB_DEF           5000000
#define OM5763_IF_MCNS_DEF          4000000

#define CU1216_PLLMFACTOR_MCNS_DEF  0x09
#define CU1216_PLLNFACTOR_MCNS_DEF  0x00
#define CU1216_PLLPFACTOR_MCNS_DEF  0x03
#define CU1216_PLLMFACTOR_DVB_DEF  0x07
#define CU1216_PLLNFACTOR_DVB_DEF  0x00
#define CU1216_PLLPFACTOR_DVB_DEF  0x03
#define CU1216_XTALL_FREQ_28       28920000

#define OM5795_PLLMFACTOR_MCNS_DEF  0x09
#define OM5795_PLLNFACTOR_MCNS_DEF  0x00
#define OM5795_PLLPFACTOR_MCNS_DEF  0x03
#define OM5795_PLLMFACTOR_DVB_DEF  0x07
#define OM5795_PLLNFACTOR_DVB_DEF  0x00
#define OM5795_PLLPFACTOR_DVB_DEF  0x03
#define OM5795_XTALL_FREQ_28       28920000
#define OM5795_IFMIN_DEF            0x60

#define OM57XX_STARTSR_DEF              7500000         /* Max SR specified by Nordig spec is 7.2 MS/s */
#define OM57XX_STOPSR_DEF               1000000
#define OM57XX_SRMAXMARGIN_DEF          30
#define OM57XX_SRMINMARGIN_DEF          20     
    
/*---------*/
/*  INDEX  */
/*---------*/
#define TDA10024_GPR_IND                0x00
#define TDA10024_AGCREF_IND             0x01
#define TDA10024_AGCCONF1_IND           0x02
#define TDA10024_CLKCONF_IND            0x03
#define TDA10024_CARCONF_IND            0x04
#define TDA10024_LOCKTHR_IND            0x05
#define TDA10024_EQCONF1_IND            0x06
#define TDA10024_EQSTEP_IND             0x07
#define TDA10024_MSETH_IND              0x08
#define TDA10024_AREF_IND               0x09
#define TDA10024_BDRLSB_IND             0x0A
#define TDA10024_BDRMID_IND             0x0B
#define TDA10024_BDRMSB_IND             0x0C
#define TDA10024_BDRINV_IND             0x0D
#define TDA10024_GAIN1_IND              0x0E
#define TDA10024_TEST_IND               0x0F
#define TDA10024_FECDVBCFG1_IND         0x10
#define TDA10024_STATUS_IND             0x11
#define TDA10024_INTP1_IND              0x12
#define TDA10024_FECDVBCFG2_IND         0x13
#define TDA10024_BERLSB_IND             0x14
#define TDA10024_BERMID_IND             0x15
#define TDA10024_BERMSB_IND             0x16
#define TDA10024_AGCTUN_IND             0x17
#define TDA10024_MSE_IND                0x18
#define TDA10024_AFC_IND                0x19
#define TDA10024_IDENTITY_IND           0x1A
#define TDA10024_ADC_IND                0x1B
#define TDA10024_EQCONF2_IND            0x1C
#define TDA10024_CKOFF_IND              0x1D
#define TDA10024_CONTROL_IND            0x1E
#define TDA10024_RESET_IND              0x1F
#define TDA10024_INTP2_IND              0x20
#define TDA10024_SATNYQ_IND             0x21
#define TDA10024_SATADC_IND             0x22
#define TDA10024_HALFADC_IND            0x23
#define TDA10024_SATDEC1_IND            0x24
#define TDA10024_SATDEC2_IND            0x25
#define TDA10024_SATDEC3_IND            0x26
#define TDA10024_SATAAF_IND             0x27
#define TDA10024_PLL1_IND               0x28
#define TDA10024_PLL2_IND               0x29
#define TDA10024_PLL3_IND               0x2A
#define TDA10024_INTS1_IND              0x2B
#define TDA10024_INTPS_IND              0x2C
#define TDA10024_SWEEP_IND              0x2D
#define TDA10024_AGCCONF2_IND           0x2E
#define TDA10024_AGCIF_IND              0x2F
#define TDA10024_SATTHR_IND             0x30
#define TDA10024_HALFTHR_IND            0x31
#define TDA10024_ITSEL1_IND             0x32
#define TDA10024_ITSEL2_IND             0x33
#define TDA10024_PWMREF_IND             0x34
#define TDA10024_TUNMAX_IND             0x35
#define TDA10024_TUNMIN_IND             0x36
#define TDA10024_DELTAF_LSB_IND         0x37
#define TDA10024_DELTAF_MSB_IND         0x38
#define TDA10024_CONSTI_IND             0x39
#define TDA10024_CONSTQ_IND             0x3A
#define TDA10024_IFMAX_IND              0x3B
#define TDA10024_IFMIN_IND              0x3C

#define TDA10024_GAIN2_IND              0x3D
#define TDA10024_ITSTAT1_IND            0x3E
#define TDA10024_ITSTAT2_IND            0x3F

#define TDA10024_REQCO_IND              0x40
#define TDA10024_REQCO_CENTRALCOEF_IND  0x50

#define TDA10024_CPT_TSP_UNCOR1_IND     0x74
#define TDA10024_CPT_TSP_UNCOR2_IND     0x75
#define TDA10024_CPT_TSP_UNCOR3_IND     0x76
#define TDA10024_CPT_TSP_UNCOR4_IND     0x77
#define TDA10024_CPT_TSP_COR1_IND       0x78
#define TDA10024_CPT_TSP_COR2_IND       0x79
#define TDA10024_CPT_TSP_COR3_IND       0x7A
#define TDA10024_CPT_TSP_COR4_IND       0x7B
#define TDA10024_CPT_TSP_OK1_IND        0x7C
#define TDA10024_CPT_TSP_OK2_IND        0x7D
#define TDA10024_CPT_TSP_OK3_IND        0x7E
#define TDA10024_CPT_TSP_OK4_IND        0x7F

#define TDA10024_IEQCO_IND              0x80
#define TDA10024_IEQCO_CENTRALCOEF_IND  0x90

#define TDA10024_AGCREFNYQ_IND          0xB4
#define TDA10024_ERAGC_THD_IND          0xB5
#define TDA10024_ERAGCNYQ_THD_IND       0xB6
#define TDA10024_SCIN_THDL_IND          0xB7
#define TDA10024_SCIN_THDH_IND          0xB8
#define TDA10024_SCIN_CPTL_IND          0xB9
#define TDA10024_SCIN_CPTH_IND          0xBA
#define TDA10024_SCIN_CPT_IND           0xBB
#define TDA10024_SCIN_NBTSAT_IND        0xBC
#define TDA10024_SATDEC0_IND            0xBD
#define TDA10024_INVQ_AGC_IND           0xBE
#define TDA10024_BW_AGC_IND             0xBF
#define TDA10024_XTAL_PLL4_IND          0xC0
#define TDA10024_PLL5_IND               0xC1
#define TDA10024_CLBSTIM_I2CSWTCH_IND   0xC2
#define TDA10024_TIMING_SCAN_IND        0xC3
#define TDA10024_TIMING_PPM_IND         0xC4
#define TDA10024_TIMING_PARA1_IND       0xC5
#define TDA10024_TIMING_PARA2_IND       0xC6
#define TDA10024_TIMING_STATUS_IND      0xC7
#define TDA10024_CPT_TRANS_IND          0xC8
#define TDA10024_PERCENTAGE_IND         0xC9
#define TDA10024_ERTIM_THD_IND          0xCA
#define TDA10024_DSP_IND                0xCB
#define TDA10024_POWER1_IND             0xCC
#define TDA10024_POWER2_IND             0xCD
#define TDA10024_POWER3_IND             0xCE

#define TDA10024_CTRL1_IND              0xD0
#define TDA10024_CTRL2_IND              0xD1
#define TDA10024_TRELDAT_IND            0xD2
#define TDA10024_TRELCOR_IND            0xD3
#define TDA10024_FECSYNC_IND            0xD4
#define TDA10024_DRNDSD1_IND            0xD5
#define TDA10024_DRNDSD2_IND            0xD6
#define TDA10024_DRNDSD3_IND            0xD7
#define TDA10024_CPT_RSB_UNCOR1_IND     0xD8
#define TDA10024_CPT_RSB_UNCOR2_IND     0xD9
#define TDA10024_CPT_RSB_UNCOR3_IND     0xDA
#define TDA10024_CPT_RSB_UNCOR4_IND     0xDB
#define TDA10024_FECRSYNC_IND           0xDC
#define TDA10024_DEINTRLV_IND           0xDD

#define TDA10024_STATUS_MCNS_IND        0xE0
#define TDA10024_VITERBI_LOCK_IND       0xE1
#define TDA10024_RSUNCORLO_IND          0xE2
#define TDA10024_RSUNCORHI_IND          0xE3
#define TDA10024_RSBERLO_IND            0xE4
#define TDA10024_RSBERHI_IND            0xE5
#define TDA10024_RSCFG_IND              0xE6
#define TDA10024_CPT_RSB_COR1_IND       0xE7
#define TDA10024_CPT_RSB_COR2_IND       0xE8
#define TDA10024_CPT_RSB_COR3_IND       0xE9
#define TDA10024_CPT_RSB_COR4_IND       0xEA
#define TDA10024_CPT_RSB_OK1_IND        0xEB
#define TDA10024_CPT_RSB_OK2_IND        0xEC
#define TDA10024_CPT_RSB_OK3_IND        0xED
#define TDA10024_CPT_RSB_OK4_IND        0xEE

#define TDA10024_EMPTY_FECMCNS_IND      0xFA
#define TDA10024_EMPTY1_TOP_IND         0xFB
#define TDA10024_EMPTY2_TOP_IND         0xFC
#define TDA10024_EMPTY_DEMOD_IND        0xFD
#define TDA10024_EMPTY_FEC_DVB_IND      0xFE



/*---------------------*/
/*  JQAM FILTER INDEX  */
/*---------------------*/
#define TDA10024_JQAM_MODE_IND      0x00
#define TDA10024_REQ_TS_IDH_IND     0x01
#define TDA10024_REQ_TS_IDL_IND     0x02
#define TDA10024_REQ_ON_IDH_IND     0x03
#define TDA10024_REQ_ON_IDL_IND     0x04
#define TDA10024_REG5_IND           0x05
#define TDA10024_TS_STATUS_H_IND    0x06
#define TDA10024_TS_STATUS_L_IND    0x07
#define TDA10024_RCV_STATUS4_IND    0x08
#define TDA10024_RCV_STATUS3_IND    0x09
#define TDA10024_RCV_STATUS2_IND    0x0A
#define TDA10024_RCV_STATUS1_IND    0x0B
#define TDA10024_TS_ID1H_IND        0x0C
#define TDA10024_TS_ID1L_IND        0x0D
#define TDA10024_ON_ID1H_IND        0x0E
#define TDA10024_ON_ID1L_IND        0x0F
#define TDA10024_TS_ID2H_IND        0x10
#define TDA10024_TS_ID2L_IND        0x11
#define TDA10024_ON_ID2H_IND        0x12
#define TDA10024_ON_ID2L_IND        0x13
#define TDA10024_TS_ID3H_IND        0x14
#define TDA10024_TS_ID3L_IND        0x15
#define TDA10024_ON_ID3H_IND        0x16
#define TDA10024_ON_ID3L_IND        0x17
#define TDA10024_TS_ID4H_IND        0x18
#define TDA10024_TS_ID4L_IND        0x19
#define TDA10024_ON_ID4H_IND        0x1A
#define TDA10024_ON_ID4L_IND        0x1B
#define TDA10024_TS_ID5H_IND        0x1C
#define TDA10024_TS_ID5L_IND        0x1D
#define TDA10024_ON_ID5H_IND        0x1E
#define TDA10024_ON_ID5L_IND        0x1F
#define TDA10024_TS_ID6H_IND        0x20
#define TDA10024_TS_ID6L_IND        0x21
#define TDA10024_ON_ID6H_IND        0x22
#define TDA10024_ON_ID6L_IND        0x23
#define TDA10024_TS_ID7H_IND        0x24
#define TDA10024_TS_ID7L_IND        0x25
#define TDA10024_ON_ID7H_IND        0x26
#define TDA10024_ON_ID7L_IND        0x27
#define TDA10024_TS_ID8H_IND        0x28
#define TDA10024_TS_ID8L_IND        0x29
#define TDA10024_ON_ID8H_IND        0x2A
#define TDA10024_ON_ID8L_IND        0x2B
#define TDA10024_TS_ID9H_IND        0x2C
#define TDA10024_TS_ID9L_IND        0x2D
#define TDA10024_ON_ID9H_IND        0x2E
#define TDA10024_ON_ID9L_IND        0x2F
#define TDA10024_TS_ID10H_IND       0x30
#define TDA10024_TS_ID10L_IND       0x31
#define TDA10024_ON_ID10H_IND       0x32
#define TDA10024_ON_ID10L_IND       0x33
#define TDA10024_TS_ID11H_IND       0x34
#define TDA10024_TS_ID11L_IND       0x35
#define TDA10024_ON_ID11H_IND       0x36
#define TDA10024_ON_ID11L_IND       0x37
#define TDA10024_TS_ID12H_IND       0x38
#define TDA10024_TS_ID12L_IND       0x39
#define TDA10024_ON_ID12H_IND       0x3A
#define TDA10024_ON_ID12L_IND       0x3B
#define TDA10024_TS_ID13H_IND       0x3C
#define TDA10024_TS_ID13L_IND       0x3D
#define TDA10024_ON_ID13H_IND       0x3E
#define TDA10024_ON_ID13L_IND       0x3F
#define TDA10024_TS_ID14H_IND       0x40
#define TDA10024_TS_ID14L_IND       0x41
#define TDA10024_ON_ID14H_IND       0x42
#define TDA10024_ON_ID14L_IND       0x43
#define TDA10024_TS_ID15H_IND       0x44
#define TDA10024_TS_ID15L_IND       0x45
#define TDA10024_ON_ID15H_IND       0x46
#define TDA10024_ON_ID15L_IND       0x47
#define TDA10024_SCIN_NBTSAT_IND    0xBC

/*------------------*/
/*  DEFAULT VALUES  */
/*------------------*/
#define TDA10024_IDENTITY_DEF       0x7D

#define TDA10024_INTP2_INTSEL_PARA_DEF 0x00
#define TDA10024_INTP2_INTSEL_PARB_DEF 0x01
#define TDA10024_INTP2_INTSEL_PARC_DEF 0x02
#define TDA10024_INTP2_INTSEL_SER_DEF 0x03

#define TDA10024_PWMREF_DEF         0x80
#define TDA10024_AGCREF_DEF         0x30        /* optimized for adjacent signal */

#define TDA10024_AGCREF_SECAM_DEF   0x50        /* optimized for adjacent signal */

#define TDA10024_SWEEP_DEF          0x80

/*----------------*/
/*  DEFINE MASKS  */
/*----------------*/
#define TDA10024_GPR_QAM_MSK            0x1C

#define TDA10024_CLKCONF_NDEC_MSK       0xC0

#define TDA10024_CLK_C_MASK             0x07

#define TDA10024_CARCONF_CAR_C_MSK      0x0F

#define TDA10024_PLL2_PDIV_MSK          0x3F
#define TDA10024_PLL2_NDIV_MSK          0xC0

#define TDA10024_GAIN1_GNYQ_MSK         0x70
#define TDA10024_GAIN1_AUTOGNYQ_MSK     0x80 

#define TDA10024_FECDVBCFG1_PVBER_MSK   0xC0

#define TDA10024_GAIN2_SFIL_MSK         0x80

#define TDA10024_RSCFG_PRG_TBER_MSK     0x0C

#define TDA10024_JQAM_MODE_JQAM_MODE_MSK    0x03

#define TDA10024_ROLL_OFF_MSK           0x03
#define TDA10024_SWEEP_MODE_MSK         0x10
#define TDA10024_AGC_INTEG_NB_MSK       0x07
#define TDA10024_EQSTTRAC_MSK           0x38
#define TDA10024_EQSTACQ_MSK            0x07
#define TDA10024_GPIOCFG_MSK            0x03

/*---------------*/
/*  DEFINE BITS  */
/*---------------*/
#define TDA10024_GPR_CLBS_BIT           0x01
#define TDA10024_GPR_CLBS2_BIT          0x02
#define TDA10024_GPR_FSAMPLING_BIT      0x20
#define TDA10024_GPR_FIRSTIF_BIT        0x40
#define TDA10024_GPR_STDBY_BIT          0x80

#define TDA10024_CLKCONF_DYN_BIT        0x08

#define TDA10024_CARCONF_INVIQ_BIT      0x20
#define TDA10024_CARCONF_AUTOINVIQ_BIT  0x40

#define TDA10024_EQCONF1_ENEQUAL_BIT    0x02
#define TDA10024_EQCONF1_ENADAPT_BIT    0x04

#define TDA10024_TEST_BYPIIC_BIT        0x80
#define TDA10024_TEST_SCLT0_BIT         0x40

#define TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT 0x20

#define TDA10024_STATUS_CARLOCK_BIT     0x02
#define TDA10024_STATUS_FSYNC_BIT       0x04
#define TDA10024_STATUS_FEL_BIT         0x08
#define TDA10024_STATUS_NODVB_BIT       0x40

#define TDA10024_INTP1_POCLKP_BIT       0x01

#define TDA10024_ADC_GAINADC_BIT        0x20
#define TDA10024_ADC_TWOS_BIT           0x08

#define TDA10024_EQCONF2_SGNALGO_BIT    0x20
#define TDA10024_EQCONF2_STEPALGO_BIT   0x10
#define TDA10024_EQCONF2_CTADAPT_BIT    0x08

#define TDA10024_CONTROL_OLDBYTECLK_BIT 0x80
#define TDA10024_CONTROL_SACLK_ON_BIT   0x04

#define TDA10024_RESET_DVBMCNS_BIT      0x80

#define TDA10024_INTP2_MSBFIRSTP_BIT    0x04

#define TDA10024_PLL3_PDPLL_BIT         0x01
#define TDA10024_PLL3_BYPPLL_BIT        0x02
#define TDA10024_PLL3_LOCK_BIT          0x04
#define TDA10024_PLL3_PSACLK_BIT        0x08

#define TDA10024_INTS1_POCLKS_BIT       0x01

#define TDA10024_INTPS_TRIS_BIT         0x01
#define TDA10024_INTPS_TRIP_BIT         0x02
#define TDA10024_INTPS_MSBFIRSTS_BIT    0x40

#define TDA10024_AGCCONF2_PWMIFTY_BIT   0x01        
#define TDA10024_AGCCONF2_PPWMIF_BIT    0x02
#define TDA10024_AGCCONF2_PWMTUNTY_BIT  0x04     
#define TDA10024_AGCCONF2_PPWMTUN_BIT   0x08
#define TDA10024_AGCCONF2_ENAGCIF_BIT   0x10
#define TDA10024_AGCCONF2_POSAGC_BIT    0x20
#define TDA10024_AGCCONF2_TRIAGC_BIT    0x80

#define TDA10024_DELTAF_MSB_ALGOD_BIT   0x80

#define TDA10024_CTRL1_QAMMODE_BIT      0x02

#define TDA10024_STATUS_MCNS_MPEGLCK_BIT    0x04
#define TDA10024_STATUS_MCNS_FRAMELCK_BIT   0x02

#define TDA10024_RSCFG_CLB_CPT_RSB_BIT  0x01

#define TDA10024_PSD_START_BIT          0x01
#define TDA10024_PSD_FINISHED_BIT       0x20

#define TDA10024_ANALYSIS_LENGTH        0x06

#define TDA10024_PART_CHIP              0x0
#define TDA10024_PART_JQAMFILTER        0x1


#define NB_MAX_CHANNEL                  136


/*============================================================================*/
/*                       ENUM OR TYPE DEFINITION                              */
/*============================================================================*/
typedef struct _tm10024config_t
{
    tmbslTDA10024FECMode_t  eModeDvbMcns;
    tmbslTDA10024BandType_t eBandType;
    UInt8                   bEqConf1;
    UInt8                   bCAR_C;
    UInt8                   bGAIN1;
    UInt32                  uXtalFreq;
    UInt8                   bDVB_PLL_M_Factor;
    UInt8                   bDVB_PLL_N_Factor;
    UInt8                   bDVB_PLL_P_Factor;
    UInt8                   bDVB_Fsampling;
    UInt8                   bMCNS_PLL_M_Factor;
    UInt8                   bMCNS_PLL_N_Factor;
    UInt8                   bMCNS_PLL_P_Factor;
    UInt8                   bMCNS_Fsampling;
    UInt8                   bIFMax;
    UInt8                   bIFMin;
    UInt8                   bTUNMax;
    UInt8                   bTUNMin;
    UInt8                   bAGCtreshold;
    UInt8                   bEqualType;
    UInt8                   bPolaPWM1;
    UInt8                   bPolaPWM2;
    UInt8                   bBERdepth;
    UInt32                  uBERwindow;
    UInt8                   bClkOffsetRange;
    UInt8                   bTUN_IQSwap;
    UInt32                  uTUN_IF;
    UInt8                   bOUT_OClk1;
    UInt8                   bOUT_bParaSer1;
    UInt8                   bOUT_bMSBFirst1;
    UInt8                   bOUT_ModeABC1;
    UInt8                   bOUT_ParaDiv1;
        UInt8                   bOUT_SyncWidth1;
        UInt8                   bOUT_DenPol1;
        UInt8                   bOUT_UncorPol1;
        UInt8                   bOUT_SyncPol1;
    UInt8                   bOUT_OClk2;
    UInt8                   bOUT_bMSBFirst2;
        UInt8                   bOUT_SyncWidth2;
        UInt8                   bOUT_DenPol2;
        UInt8                   bOUT_UncorPol2;
        UInt8                   bOUT_SyncPol2;
    UInt8                   bSwDyn;
    UInt8                   bSwStep;
    UInt8                   bSwLength;
    UInt8                   bSwPos;
    UInt8                   bEnableI2cSwitch;
} tm10024config_t, *ptm10024config_t, **pptm10024config_t;

typedef struct _tm10024flags_t
{
    unsigned int iScan      :1;
    unsigned int iStartAlgo :1;
    unsigned int iInit      :1;
    unsigned int iAutoSi    :1;
        unsigned int iAutoQam   : 1;
} tm10024flags_t, *ptm10024flags_t;

typedef struct _tm10024channel_t
{
    UInt32              uRF;
    UInt32              uSR;
    tmFrontEndSpecInv_t   eSI;
    tmFrontEndModulation_t eMOD;
} tm10024channel_t, *ptm10024channel_t;

typedef struct _tmTDA10024Object_t 
{
    tmUnitSelect_t              tUnit;
    tmUnitSelect_t              tUnitW;
    ptmbslFrontEndMutexHandle   pMutex;
    tm10024channel_t            sCurrentChannel;
    tm10024config_t             sConfig;
    tm10024flags_t              sApiFlags;
    tmbslFrontEndIoFunc_t       sRWFunc;
    tmbslFrontEndTimeFunc_t     sTime;
    tmbslFrontEndDebugFunc_t    sDebug;
    tmbslFrontEndMutexFunc_t    sMutex;
    UInt32                      uSysClk;
    UInt8                       bGainAlgo;
    UInt32                      uSyncReg;
    tmbslTDA10024AlgoState_t    bState;
    UInt8                       bStateClkOffset;
    Int32                       lClkOffCst;
    UInt16                      uNbSamplesSSAT;     /* Nb of samples in STAT depends on reg SSAT */
    UInt32                      uSWDynValue;
    UInt32                      uSWStepValue;
    UInt16                      uNbSamplesAGC;      /* Nb of samples used to refresh AGC values */
    UInt32                      uEqualizerSymbolsNb;
    UInt8                       bNoCarrierIfAGCMax;
    UInt32                      uRefSR;
    tmPowerState_t              ePowerState;
    UInt8                       uTestRegSave; /* For standby and power on */   
} tmTDA10024Object_t, *ptmTDA10024Object_t, **pptmTDA10024Object_t;

typedef struct _tm10024Qam_t
{
    UInt32  uQam;
    UInt32  uLockthr;
    UInt32  uMseth;
    UInt32  uAref;
    UInt32  uAgcRefNyq;
    UInt32  uErAgcNyqThd;
} tm10024Qam_t, *ptm10024Qam_t;


/*============================================================================*/
/* Internal Prototypes:                                                       */
/*============================================================================*/

/* Local functions */
tmErrorCode_t TDA10024Write(tmUnitSelect_t tUnit, UInt8 uIndex_U, UInt32 uNBytes_U, UInt8* puData_U);
tmErrorCode_t TDA10024WriteBit(tmUnitSelect_t tUnit, UInt8 uIndex_U, UInt32 uMask_U, UInt8 uData_U);
tmErrorCode_t TDA10024Read(tmUnitSelect_t tUnit, UInt8 uIndex_U, UInt32 uNBytes_U, UInt8* puData_U);
tmErrorCode_t TDA10024WriteInit(tmUnitSelect_t tUnit);
tmErrorCode_t TDA10024Wait (tmUnitSelect_t tUnit, UInt32 uTime);
tmErrorCode_t TDA10024GetIQ (tmUnitSelect_t tUnit, UInt32 *puIQ);
tmErrorCode_t TDA10024GetCOEF (tmUnitSelect_t tUnit, Int32 *plCoef);
tmErrorCode_t TDA10024ResetAll (tmUnitSelect_t tUnit);

extern tmErrorCode_t TDA10024MutexAcquire(ptmTDA10024Object_t pObj, UInt32 timeOut);
extern tmErrorCode_t TDA10024MutexRelease(ptmTDA10024Object_t pObj);

#ifdef __cplusplus
}
#endif

#endif /* TMBSLTDA10024LOCAL_H */
/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/


