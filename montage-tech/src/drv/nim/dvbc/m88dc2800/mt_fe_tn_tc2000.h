/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with      *
* Montage covering this file. Redistribution, except as permitted by the terms *
* of your license agreement(s) with Montage, is strictly prohibited.           *
********************************************************************************
*
* SVN Header File Version $Rev: 14975 $
*
*******************************************************************************/

#ifndef __TC2K_Tuner_H__
#define __TC2K_Tuner_H__

#include "nim_m88dc2800.h"

/*******************************************************************************
* Driver revisions.
*******************************************************************************/

#define TC2K_swRevMajor 2
#define TC2K_swRevMinor 1
#define TC2K_swRevBuild 4

/*******************************************************************************
* RegImage - Compile time switch. Use register image if defined.
*******************************************************************************/

#define TC2K_RegImage

/*******************************************************************************
* Basic data types.
*******************************************************************************/

/**********************************************************************************************************************/
#ifndef U8
	#define U8 unsigned char
#endif
#ifndef U16
	#define U16 unsigned int
#endif
#ifndef S16
	#define S16 signed short
#endif
#ifndef S32
	#define S32 signed long
#endif
#ifndef U32
	#define U32 unsigned long
#endif

/*******************************************************************************
* Definitions used in the entr_data data structure and in mt_fe_tn_tc2000.c.
*******************************************************************************/

#define TC2K_ifAccTop_Cnt			2
#define TC2K_ifCalBw_Cnt			4
#define TC2K_tfcLb_Cnt				2
#define TC2K_tfcMb_Cnt				2
#define TC2K_tfcHb_Cnt				2
#define TC2K_tfcTtLut_Cnt			7

/*******************************************************************************
* Enumerated inputs for the top level API calls.
*******************************************************************************/

typedef enum
{
	TC2K_2wbSync_no,
	TC2K_2wbSync_yes
} TC2K_2wbSync;

typedef enum
{
	TC2K_ltaEnaSel_off,
	TC2K_ltaEnaSel_on
} TC2K_ltaEnaSel;

/* index for ifAgcRange[2]. do not change order */
typedef enum
{
	TC2K_broadcastMedium_cable,
	TC2K_broadcastMedium_ota
} TC2K_broadcastMedium;

/* index for rfAttRange[3] */
typedef enum
{
	TC2K_rfAttMode_seriesPin_shuntPin,
	TC2K_rfAttMode_seriesPin_internalAtt,
	TC2K_rfAttMode_seriesPin_shuntPin_internalAtt
} TC2K_rfAttMode;

typedef enum
{
	TC2K_rfAttBias_1p8v,
	TC2K_rfAttBias_3p3v
} TC2K_rfAttBias;

typedef enum
{
	TC2K_specInvSel_off,
	TC2K_specInvSel_on
} TC2K_specInvSel;

/* used as index in _mt_fe_tn_tc2000_cfgBw(). do not change order */
typedef enum
{
	TC2K_ifBw_6MHz,
	TC2K_ifBw_7MHz,
	TC2K_ifBw_8MHz,
	TC2K_ifBw_User
} TC2K_ifBw;

typedef enum
{
	TC2K_ifOutputSel_1,
	TC2K_ifOutputSel_2
} TC2K_ifOutputSel;

typedef enum
{
	TC2K_agcCtrlMode_demod,
	TC2K_agcCtrlMode_internal
} TC2K_agcCtrlMode;

/* used as index for rssiPar[] in mt_fe_tn_tc2000.c(Cfg)*/
typedef enum
{
	TC2K_modulation_DVB_C,
	TC2K_modulation_J83_Annex_B,
	TC2K_modulation_DVB_T,
	TC2K_modulation_ATSC,
	TC2K_modulation_ISDB_T,
	TC2K_modulation_DMB_T,
	TC2K_modulation_PAL_B,
	TC2K_modulation_PAL_D_I_K,
	TC2K_modulation_PAL_G_H,
	TC2K_modulation_NTSC,
	TC2K_modulation_SECAM,
	TC2K_modulation_SECAM_L
} TC2K_modulation;

typedef enum
{
	TC2K_tunerCnt_single,
	TC2K_tunerCnt_dual
} TC2K_tunerCnt;

typedef enum
{
	TC2K_rxPwr_low,
	TC2K_rxPwr_nominal,
	TC2K_rxPwr_medium,
	TC2K_rxPwr_high,
	TC2K_rxPwr_auto
} TC2K_rxPwr;

/* index for tc2000_rssiCF[] */
typedef enum
{
	TC2K_lnaModeStatus_cable,
	TC2K_lnaModeStatus_ota,
	TC2K_lnaModeStatus_low_noise
} TC2K_lnaModeStatus;

typedef enum
{
	TC2K_ifOutputLevel_Auto,
	TC2K_ifOutputLevel_neg3p2_dB,
	TC2K_ifOutputLevel_neg2p8_dB,
	TC2K_ifOutputLevel_neg2p3_dB,
	TC2K_ifOutputLevel_neg2p0_dB,
	TC2K_ifOutputLevel_neg1p6_dB,
	TC2K_ifOutputLevel_neg1p0_dB,
	TC2K_ifOutputLevel_neg0p5_dB,
	TC2K_ifOutputLevel_0_dB_nominal,
	TC2K_ifOutputLevel_0p6_dB,
	TC2K_ifOutputLevel_1p2_dB,
	TC2K_ifOutputLevel_1p8_dB,
	TC2K_ifOutputLevel_2p5_dB,
	TC2K_ifOutputLevel_3p3_dB,
	TC2K_ifOutputLevel_4p0_dB,
	TC2K_ifOutputLevel_5p0_dB,
	TC2K_ifOutputLevel_6p0_dB
} TC2K_ifOutputLevel;

typedef enum
{
	TC2K_ifDacMode_Normal,
	TC2K_ifDacMode_Optimize
} TC2K_ifDacMode;

/*******************************************************************************
* API error codes.
*******************************************************************************/

typedef enum
{
	TC2K_ok,
	TC2K_err_rw,
	TC2K_err_not_calibrated,
	TC2K_err_not_implemented,
	TC2K_err_invalid_parameter,
	TC2K_err_unknown,
	TC2K_err_ifcsm_timeout,
	TC2K_err_ifcsm_fail,
	TC2K_err_ifAccTop_setTtPwr,
	TC2K_err_tfcSm_timeout,
	TC2K_err_tfcSm_fail,
	TC2K_err_tfCal_fail,
	TC2K_err_freqPllSm_timeout,
	TC2K_err_freqPll_unlocked,
	TC2K_err_cfgCalDet_timeout,
	TC2K_err_tfSetCh_fail,
	TC2K_err_unsuported_ic_rev
} TC2K_RET;

typedef struct
{
	U8	tunerCalibrated;
	U8	tunerIdle;
	S32	FxtalHz;
	S32	FcompHz;
	S32	FchanHz;
	S32	ifBwHz;
	S32	FifHz;
	S32	FifMaxHz;
	S32	FifN;
	S32	FifD;
	S32	FloHz;
	S32	FspinHz;
	S32	FspinMaxHz;
	S32	FspinN;
	S32	FspinD;
	U8	ifBwWd[TC2K_ifCalBw_Cnt];
	S32	ifAccTop[TC2K_ifAccTop_Cnt];
	U8	ifAccTop_dB;
	U8	tffCwLb[TC2K_tfcLb_Cnt];
	U8	tffCwMb[TC2K_tfcMb_Cnt];
	U8	tffCwHb[TC2K_tfcHb_Cnt];
	S32	tffcalSlope[3];
	U8	tffcalCw[3];
	U8	tfgCwLb[TC2K_tfcLb_Cnt];
	U8	tfgCwMb[TC2K_tfcMb_Cnt];
	U8	tfgCwHb[TC2K_tfcHb_Cnt];
	S16	tfcTtLut[TC2K_tfcTtLut_Cnt];
	S16	calTmpr;
	S16	curTmpr;
	S32	freqLbMax;
	S32	freqMbMax;
	U8	regImage[187];
	U8	b2wReg1;
	U8	b2wRegN;
	S16	rssiLnaOffset;
	U8	dieRev;
	U8	dieCfg;
	U8	synCPcurRestore;
	U8	vcoIselRestore;
	U8	tfcTtPwrLbDebug[TC2K_tfcLb_Cnt];
	U8	tfcTtPwrMbDebug[TC2K_tfcMb_Cnt];
	U8	tfcTtPwrHbDebug[TC2K_tfcHb_Cnt];
	U8	pllReTrigCntDebug;
	S16	agcDelayDebug[8];
	S16	parDebug;
} tc2k_data;

/******************************************************************************
* tfcTtLut[6] was moved from mt_fe_tn_tc2000.c(TfCal) into tc2k_data to support testing.
* tfcTtPwrLb[5], tfcTtPwrMb[5], tfcTtPwrHb[5] are new and also support testing.
******************************************************************************/

typedef struct
{
	U8						init_OK;

	TC2K_b2wAddress			b2wAddress;
	S32						FchanHz;
	S32						FchanErrHz;
	TC2K_ltaEnaSel			ltaEnaSel;
	TC2K_broadcastMedium	broadcastMedium;
	TC2K_rfAttMode			rfAttMode;
	TC2K_rfAttBias			rfAttBias;
	TC2K_specInvSel			specInvSel;
	TC2K_ifBw				ifBw;
	S32						ifBwUserHz;
	U8						*ifBwUserFIR;
	S32						FifHz;
	S32						FifErrHz;
	TC2K_ifOutputSel		ifOutputSel;
	TC2K_ifOutputLevel		ifOutPutLevel_IF1;
	TC2K_ifOutputLevel		ifOutPutLevel_IF2;
	TC2K_ifDacMode			ifDacMode_IF1;
	TC2K_ifDacMode			ifDacMode_IF2;
	TC2K_agcCtrlMode		agcCtrlMode;
	TC2K_modulation			modulation;
	TC2K_tunerCnt			tunerCnt;
	TC2K_rxPwr				rxPwr;
	TC2K_lnaModeStatus		lnaModeStatus;
	S16						RSSI;
	tc2k_data				tunerData;
} tc2000_tn_config;

/*******************************************************************************
* Constants used in top level API functions.
*******************************************************************************/

#define TC2K_tunerCalibrated_yes 0
#define TC2K_tunerCalibrated_no 1

#define TC2K_tunerIdle_yes 0
#define TC2K_tunerIdle_no 1

#define TC2K_FchanMin 20000000
#define TC2K_FchanMax 900000000

/*******************************************************************************
* Top level API function prototypes.
*******************************************************************************/
RET_CODE mt_fe_tn_tc2000_init_wrapper (void);
void M88DC2800RegInitial ( void );

MT_FE_RET mt_fe_dc2800_tn_init_tc2000(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_tc2000(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_get_signal_strength_tc2000(void *dev_handle, U32 *p_gain, U32 *p_strength);

TC2K_RET mt_fe_tn_tc2000_init(tc2000_tn_config* tuner_cfg);
TC2K_RET mt_fe_tn_tc2000_active(tc2000_tn_config *tunerCfg);
TC2K_RET mt_fe_tn_tc2000_set_chan(tc2000_tn_config *tunerCfg);
TC2K_RET mt_fe_tn_tc2000_idle(tc2000_tn_config *tunerCfg);
TC2K_RET mt_fe_tn_tc2000_rssi(tc2000_tn_config *tunerCfg);
TC2K_RET mt_fe_tn_tc2000_status(tc2000_tn_config *tunerCfg);
TC2K_RET mt_fe_tn_tc2000_code_revision(U8 *revArray);

/*******************************************************************************
* External user supplied function prototypes.
*******************************************************************************/


/*******************************************************************************
* Register access function prototypes.
*******************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_set_reg(tc2000_tn_config *tunerCfg, U8 reg, U8 mask, U8 value, TC2K_2wbSync sync);
TC2K_RET _mt_fe_tn_tc2000_get_reg(tc2000_tn_config *tunerCfg, U8 reg, U8 mask, U8 *value, TC2K_2wbSync sync);
TC2K_RET _mt_fe_tn_tc2000_i2c_write(tc2000_tn_config *tunerCfg, U8 reg, U8 *buf, U8 len, TC2K_2wbSync sync);
TC2K_RET _mt_fe_tn_tc2000_i2c_read(tc2000_tn_config *tunerCfg, U8 reg, U8* buf, U8 len, TC2K_2wbSync sync);

/*******************************************************************************
* IC Configutration: Definitions.
*******************************************************************************/

#define TC2K_cfgWrReg1				18
#define TC2K_cfgRegCnt				169

#define TC2K_cfgChFltrReg1			49
#define TC2K_cfgChFltrRegCnt		44

#define TC2K_cfgIfAccTopFttHz_Ref	93750

#define TC2K_cfgShPinLimitLo_Cablel	188
#define TC2K_cfgShPinLimitHi_Cablel	2
#define TC2K_cfgShPinLimitLo_Ter	132
#define TC2K_cfgShPinLimitHi_Ter	3

#define TC2K_cfgTempAccDelta10Deg	64 /* 6.4 bits/deg, 10 * 6.4 = 64. */
#define TC2K_cfgTmprAccReading25Deg	1980

#define TC2K_rssiRfAttLnaBandOffset 3
#define TC2K_rssiRfAttLutLen 19
#define TC2K_rssiRfAttLutLnaBandOffset 57
#define TC2K_rssiRfAttLutRfAttCfgOffset 19

#define TC2K_rssiDigGainLen 38
#define TC2K_rssiDigGainMin -380
#define TC2K_rssiDigGainStep 20


/*******************************************************************************
* IC Configutration: Function prototypes.
*******************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_cfgIcRev (tc2000_tn_config *tunerCfg);

TC2K_RET _mt_fe_tn_tc2000_cfgIc (tc2000_tn_config *tunerCfg);

TC2K_RET _mt_fe_tn_tc2000_cfgBase (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgIdle (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgIfBwCal (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgIfAccTopCal (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgTfCal (tc2000_tn_config *tunerCfg);

/* cfg ic multi-mode settings */
TC2K_RET _mt_fe_tn_tc2000_cfgBcMedium (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgRfAtten (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgBw (tc2000_tn_config *tunerCfg);
U8		 _mt_fe_tn_tc2000_cfgIfBwUserCwInterp (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgIfOutputLevel (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgMod (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgMultDepen(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgIfDac (tc2000_tn_config *tunerCfg);

/* misc */
TC2K_RET _mt_fe_tn_tc2000_cfgResolveBbErrCondition(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgCalDetRead(tc2000_tn_config *tunerCfg, S32 *Value);
TC2K_RET _mt_fe_tn_tc2000_cfgReadTmprAcc(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgBbAgcDelay(tc2000_tn_config *tunerCfg, U8 timeOutMsec);
TC2K_RET _mt_fe_tn_tc2000_cfgAgcDelay(tc2000_tn_config *tunerCfg, U8 timeOutMsec);
TC2K_RET _mt_fe_tn_tc2000_cfgRssi (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgRssiSearchLut (S16 acc, S16 *lut, S16 lutLen, S16 gainMin,
								   S16 gainStep, S16 *gain);
TC2K_RET _mt_fe_tn_tc2000_cfgHystModeOff (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgHystModeOn (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgCalRfDet (tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_cfgChanChgOpt (tc2000_tn_config *tunerCfg);

/*******************************************************************************
* Channel Selection: Definitions.
*******************************************************************************/

#define TC2K_freqLoSmTimeOutMsec 100

#define TC2K_freqSetLnaModeAccMin 4090

#define TC2K_freqVCO1lowV_max_Hz 867450000
#define TC2K_freqVCO2lowV_max_Hz 930150000
#define TC2K_freqVCO3lowV_max_Hz 1013850000
#define TC2K_freqVCO4lowV_max_Hz 1132150000
#define TC2K_freqVCO5lowV_max_Hz 1215300000
#define TC2K_freqVCO6lowV_max_Hz 1329050000
#define TC2K_freqVCO7lowV_max_Hz 1488300000
#define TC2K_freqVCO8lowV_max_Hz 1594350000
#define TC2K_freqVCO9lowV_max_Hz 1731150000

/*******************************************************************************
* Channel Selection: Function prototypes.
*******************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_freqSetFchHz(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_freqSetFxtal(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_freqSetFloHz(tc2000_tn_config *tunerCfg, S32 floHz);
TC2K_RET _mt_fe_tn_tc2000_freqSetFspinHz(tc2000_tn_config *tunerCfg, S32 FspinHz);
TC2K_RET _mt_fe_tn_tc2000_freqSetFifHz(tc2000_tn_config *tunerCfg, S32 FifHz);
TC2K_RET _mt_fe_tn_tc2000_freqSetTfBand(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_freqSetTtGen(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_freqSetLc(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_freqCfgTtGen(tc2000_tn_config *tunerCfg, U8 LoDiv);

/*******************************************************************************
* TF Control: Definitions.
*******************************************************************************/

#define TC2K_tfgCompCntHb 7
#define TC2K_tfgCompCntMb 3
#define TC2K_tfgCompCntLb 3

#define TC2K_tfcTol 2000000
#define TC2K_tfTempAccDelta25Deg 160 /* 6.4 bits/deg, 25 * 6.4 = 160. */

#define TC2K_tfSmTimeOutMsec 100

/*******************************************************************************
* TF Control: Function prototypes.
*******************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_tfCal(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_tfSetTtLut(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_tfCalOneBand(tc2000_tn_config *tunerCfg, U8 tfBand, U8 tfcCapWdMax, U8 tfFreqCnt, U8 tfCw0[], S32 tfFreq[], U8 tffCw[], U8 tfgCw[]);
TC2K_RET _mt_fe_tn_tc2000_tfGainCal(tc2000_tn_config *tunerCfg, U8 tfBand, S32 VrefComp, U8 *tfgCw);

TC2K_RET _mt_fe_tn_tc2000_tfcFreqGetSlope(U8 c1, U8 c2, S32 f1, S32 f2, U8 CWmax, S32 *slope);
TC2K_RET _mt_fe_tn_tc2000_tfcFreqSetBandTrans(tc2000_tn_config *tunerCfg);

TC2K_RET _mt_fe_tn_tc2000_tfcTrig(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_tfSetTtPwr(tc2000_tn_config *tunerCfg, U8 *ttPwr);

TC2K_RET _mt_fe_tn_tc2000_tfChanChg(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_tfChanChgFreq(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_tfChanChgGain(tc2000_tn_config *tunerCfg);

/*******************************************************************************
* IF Calibration: Definitions.
*******************************************************************************/

#define TC2K_ifCalBw_6MHz 0
#define TC2K_ifCalBw_7MHz 1
#define TC2K_ifCalBw_8MHz 2
#define TC2K_ifCalBw_10MHz 3

#define TC2K_ifCalN 16
#define TC2K_ifCalD_6MHz 64
#define TC2K_ifCalD_7MHz 64
#define TC2K_ifCalD_8MHz 64
#define TC2K_ifCalD_10MHz 64

/* index for ifAccTop[2] in tuner data structure and TC2K_ifAccTopRefN[2] in mt_fe_tn_tc2000.c(IfCal) */
#define TC2K_ifAccTopIndex_Vga2_on_26dB 0
#define TC2K_ifAccTopIndex_Vga2_off_12dB 1
#define TC2K_ifAccTopRefD 10 /* used with TC2K_ifAccTopRefN[] to set ifAccTop */
/* Actual ifAccTop back off in db. Stored in tuner structure for api validation. */

#define TC2K_ifAccTop_Vga2_on_26dB 26
#define TC2K_ifAccTop_Vga2_off_12dB 12

/*******************************************************************************
* IF Calibration: Function prototypes.
*******************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_ifCalBW(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_ifCalBWSngleFreq(tc2000_tn_config *tunerCfg, S32 ifCalVref, S32 ifCalD, U8 ifBw);
TC2K_RET _mt_fe_tn_tc2000_ifSetBw(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_ifTrigSM(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_ifSetTtFreq(tc2000_tn_config *tunerCfg, U8 ifBw);
TC2K_RET _mt_fe_tn_tc2000_ifAccTopCal(tc2000_tn_config *tunerCfg);
TC2K_RET _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff(tc2000_tn_config *tunerCfg, U8 ifAccTopValueIndex);
TC2K_RET _mt_fe_tn_tc2000_ifSetAccTop(tc2000_tn_config *tunerCfg, U8 ifAccTopValueIndex);

/*******************************************************************************
* Manualy codded registers due to reg map.
*******************************************************************************/

#define TC2K_reg186b1w7_nrz 8
#define TC2K_reg186b1w7_rz 72

/*******************************************************************************
* Auto generated register definitions.
*******************************************************************************/

#define TC2K_dieRev_reg 0
#define TC2K_dieRev_mask 31

#define TC2K_dieCfg_reg 0
#define TC2K_dieCfg_mask 224

#define TC2K_synAnLock_reg 1
#define TC2K_synAnLock_mask 1
#define TC2K_synAnLock_off 0
#define TC2K_synAnLock_on 1

#define TC2K_synDigLock_reg 1
#define TC2K_synDigLock_mask 8
#define TC2K_synDigLock_off 0
#define TC2K_synDigLock_on 8

#define TC2K_synVcoRd_reg 1
#define TC2K_synVcoRd_mask 240

#define TC2K_synVtuneRd_reg 2
#define TC2K_synVtuneRd_mask 15

#define TC2K_tmprRd_reg 3
#define TC2K_tmprRd_mask 127

#define TC2K_agcAccRdLo_reg 7
#define TC2K_agcAccRdLo_mask 255

#define TC2K_agcAccRdHi_reg 8
#define TC2K_agcAccRdHi_mask 255

#define TC2K_agcErrLo_reg 10
#define TC2K_agcErrLo_mask 255

#define TC2K_agcErrHi_reg 11
#define TC2K_agcErrHi_mask 31

#define TC2K_calDetRdLo_reg 12
#define TC2K_calDetRdLo_mask 255

#define TC2K_calDetRdHi_reg 13
#define TC2K_calDetRdHi_mask 127

#define TC2K_calDetDone_reg 13
#define TC2K_calDetDone_mask 128
#define TC2K_calDetDone_notDone 0
#define TC2K_calDetDone_done 128

#define TC2K_ifCapWdRd_reg 14
#define TC2K_ifCapWdRd_mask 31

#define TC2K_ifCalErr_reg 14
#define TC2K_ifCalErr_mask 32
#define TC2K_ifCalErr_noErr 0
#define TC2K_ifCalErr_Err 32

#define TC2K_ifCalDone_reg 14
#define TC2K_ifCalDone_mask 64
#define TC2K_ifCalDone_notDone 0
#define TC2K_ifCalDone_done 64

#define TC2K_tfcCapWdRd_reg 15
#define TC2K_tfcCapWdRd_mask 63

#define TC2K_tfcErr_reg 15
#define TC2K_tfcErr_mask 64
#define TC2K_tfcErr_noErr 0
#define TC2K_tfcErr_err 64

#define TC2K_tfcDone_reg 15
#define TC2K_tfcDone_mask 128
#define TC2K_tfcDone_notDone 0
#define TC2K_tfcDone_done 128

#define TC2K_iAttCtrl_reg 18
#define TC2K_iAttCtrl_mask 1
#define TC2K_iAttCtrl_off 0
#define TC2K_iAttCtrl_on 1

#define TC2K_ltaEna_reg 18
#define TC2K_ltaEna_mask 4
#define TC2K_ltaEna_off 0
#define TC2K_ltaEna_on 4

#define TC2K_lnaMode_reg 18
#define TC2K_lnaMode_mask 24
#define TC2K_lnaMode_Cal 0
#define TC2K_lnaMode_terLoNoise 8
#define TC2K_lnaMode_terNorm 16
#define TC2K_lnaMode_cab 24

#define TC2K_dualChipSel_reg 18
#define TC2K_dualChipSel_mask 32
#define TC2K_dualChipSel_single 0
#define TC2K_dualChipSel_dual 32

#define TC2K_mixMode_reg 19
#define TC2K_mixMode_mask 48
#define TC2K_mixMode_phs8 0
#define TC2K_mixMode_phs4 16
#define TC2K_mixMode_phs2 32
#define TC2K_mixMode_disconSig 48

#define TC2K_mixLoBufBiasCurSel_reg 19
#define TC2K_mixLoBufBiasCurSel_mask 192
#define TC2K_mixLoBufBiasCurSel_mA2p0 0
#define TC2K_mixLoBufBiasCurSel_mA2p25 64
#define TC2K_mixLoBufBiasCurSel_mA2p5 128
#define TC2K_mixLoBufBiasCurSel_mA3p0 192

#define TC2K_synLoDiv_reg 20
#define TC2K_synLoDiv_mask 7
#define TC2K_synLoDiv_reg019Lsbp0Idx0 0
#define TC2K_synLoDiv_div4Phs2 1
#define TC2K_synLoDiv_div16Phs8 2
#define TC2K_synLoDiv_div8Phs4 3
#define TC2K_synLoDiv_reg019Lsbp0Idx4 4
#define TC2K_synLoDiv_reg019Lsbp0Idx5 5
#define TC2K_synLoDiv_div32Phs8 6
#define TC2K_synLoDiv_div64Phs8 7

#define TC2K_ttBufCtrl_reg 20
#define TC2K_ttBufCtrl_mask 128
#define TC2K_ttBufCtrl_off 0
#define TC2K_ttBufCtrl_on 128

#define TC2K_vcoIselEna_reg 21
#define TC2K_vcoIselEna_mask 1
#define TC2K_vcoIselEna_off 0
#define TC2K_vcoIselEna_on 1

#define TC2K_sePinCtrl_reg 21
#define TC2K_sePinCtrl_mask 2
#define TC2K_sePinCtrl_off 0
#define TC2K_sePinCtrl_on 2

#define TC2K_shPinCtrl_reg 21
#define TC2K_shPinCtrl_mask 4
#define TC2K_shPinCtrl_off 0
#define TC2K_shPinCtrl_on 4

#define TC2K_tfBand_reg 22
#define TC2K_tfBand_mask 6
#define TC2K_tfBand_LB 0
#define TC2K_tfBand_MB 2
#define TC2K_tfBand_HB 4
#define TC2K_tfBand_TT 6

#define TC2K_tfAttCw_reg 22
#define TC2K_tfAttCw_mask 120
#define TC2K_tfAttCw_AttCtl_0 0
#define TC2K_tfAttCw_AttCtl_1 8
#define TC2K_tfAttCw_AttCtl_2 16
#define TC2K_tfAttCw_AttCtl_3 24
#define TC2K_tfAttCw_AttCtl_4 32
#define TC2K_tfAttCw_AttCtl_5 40
#define TC2K_tfAttCw_AttCtl_6 48
#define TC2K_tfAttCw_AttCtl_7 56
#define TC2K_tfAttCw_AttCtl_8 64
#define TC2K_tfAttCw_AttCtl_9 72
#define TC2K_tfAttCw_AttCtl_10 80
#define TC2K_tfAttCw_AttCtl_11 88
#define TC2K_tfAttCw_AttCtl_12 96
#define TC2K_tfAttCw_AttCtl_13 104
#define TC2K_tfAttCw_AttCtl_14 112
#define TC2K_tfAttCw_AttCtl_15 120

#define TC2K_tfDeQhb_reg 22
#define TC2K_tfDeQhb_mask 128
#define TC2K_tfDeQhb_Off 0
#define TC2K_tfDeQhb_On 128

#define TC2K_tfDeQlb_reg 23
#define TC2K_tfDeQlb_mask 128
#define TC2K_tfDeQlb_off 0
#define TC2K_tfDeQlb_on 128

#define TC2K_rfDetLvlCtrl_reg 27
#define TC2K_rfDetLvlCtrl_mask 12
#define TC2K_rfDetLvlCtrl_rangeM22M20 0
#define TC2K_rfDetLvlCtrl_rangeM26M23 4
#define TC2K_rfDetLvlCtrl_rangeM30M27 8
#define TC2K_rfDetLvlCtrl_rangeM35M31 12

#define TC2K_tmprCtrl_reg 27
#define TC2K_tmprCtrl_mask 16
#define TC2K_tmprCtrl_TmprPwrOff 0
#define TC2K_tmprCtrl_TmprPwrOn 16

#define TC2K_synCpSmEna_reg 30
#define TC2K_synCpSmEna_mask 1
#define TC2K_synCpSmEna_dis 0
#define TC2K_synCpSmEna_ena 1

#define TC2K_synCPcur_reg 30
#define TC2K_synCPcur_mask 24
#define TC2K_synCPcur_mA0p4 0
#define TC2K_synCPcur_mA0p8 8
#define TC2K_synCPcur_mA1p6 16
#define TC2K_synCPcur_mA2p0 24

#define TC2K_synEna_reg 31
#define TC2K_synEna_mask 1
#define TC2K_synEna_Off 0
#define TC2K_synEna_On 1

#define TC2K_synN_reg 32
#define TC2K_synN_mask 255

#define TC2K_synA_reg 33
#define TC2K_synA_mask 31

#define TC2K_synR_reg 34
#define TC2K_synR_mask 127

#define TC2K_synNrange_reg 35
#define TC2K_synNrange_mask 3
#define TC2K_synNrange_range0 0
#define TC2K_synNrange_range1 1
#define TC2K_synNrange_range2 2
#define TC2K_synNrange_range3 3

#define TC2K_synSmClk_reg 35
#define TC2K_synSmClk_mask 12
#define TC2K_synSmClk_vcoSM0 0
#define TC2K_synSmClk_vcoSM1 4
#define TC2K_synSmClk_vcoSM2 8
#define TC2K_synSmClk_vcoSM3 12

#define TC2K_synVcoWr_reg 35
#define TC2K_synVcoWr_mask 240
#define TC2K_synVcoWr_VCO0 0
#define TC2K_synVcoWr_VCO1 16
#define TC2K_synVcoWr_VCO2 32
#define TC2K_synVcoWr_VCO3 48
#define TC2K_synVcoWr_VCO4 64
#define TC2K_synVcoWr_VCO5 80
#define TC2K_synVcoWr_VCO6 96
#define TC2K_synVcoWr_VCO7 112
#define TC2K_synVcoWr_VCO8 128

#define TC2K_synSmTrig_reg 36
#define TC2K_synSmTrig_mask 1
#define TC2K_synSmTrig_dis 0
#define TC2K_synSmTrig_ena 1

#define TC2K_vcoIsel_reg 36
#define TC2K_vcoIsel_mask 12
#define TC2K_vcoIsel_sel0 0
#define TC2K_vcoIsel_sel1 4
#define TC2K_vcoIsel_sel2 8
#define TC2K_vcoIsel_sel3 12

#define TC2K_LnaBuffBiasAdj2_reg 38
#define TC2K_LnaBuffBiasAdj2_mask 12
#define TC2K_LnaBuffBiasAdj2_neg12p5percent 0
#define TC2K_LnaBuffBiasAdj2_Nominal 4
#define TC2K_LnaBuffBiasAdj2_pos12p5percent 8
#define TC2K_LnaBuffBiasAdj2_pos25percent 12

#define TC2K_rfAttBias_reg 38
#define TC2K_rfAttBias_mask 16
#define TC2K_rfAttBias_Volt1p8 0
#define TC2K_rfAttBias_Volt3p3 16

#define TC2K_ttlnjectPt_reg 40
#define TC2K_ttlnjectPt_mask 2
#define TC2K_ttlnjectPt_Lna 0
#define TC2K_ttlnjectPt_Mix 2

#define TC2K_ttPwr_reg 40
#define TC2K_ttPwr_mask 28
#define TC2K_ttPwr_LnaNeg80MixNeg68dBV 0
#define TC2K_ttPwr_LnaNeg74MixNeg62dBV 4
#define TC2K_ttPwr_LnaNeg68MixNeg56dBV 8
#define TC2K_ttPwr_LnaNeg62MixNeg50dBV 12
#define TC2K_ttPwr_LnaNeg56MixNeg44dBV 16
#define TC2K_ttPwr_LnaNeg50MixNeg38dBV 20
#define TC2K_ttPwr_LnaNeg44MixNeg32dBV 24
#define TC2K_ttPwr_LnaNeg44MixNeg32again 28

#define TC2K_ttFilter_reg 43
#define TC2K_ttFilter_mask 255

#define TC2K_ifCapWdWr_reg 44
#define TC2K_ifCapWdWr_mask 31

#define TC2K_calDetTrig_reg 45
#define TC2K_calDetTrig_mask 1
#define TC2K_calDetTrig_ClearTrigger 0
#define TC2K_calDetTrig_Trigger 1

#define TC2K_calDetCtrl_reg 45
#define TC2K_calDetCtrl_mask 64
#define TC2K_calDetCtrl_B2W 0
#define TC2K_calDetCtrl_SM 64

#define TC2K_ifCalTrig_reg 46
#define TC2K_ifCalTrig_mask 16
#define TC2K_ifCalTrig_ClearTrigger 0
#define TC2K_ifCalTrig_Trigger 16

#define TC2K_ifCalVrefLo_reg 47
#define TC2K_ifCalVrefLo_mask 255

#define TC2K_ifCalVrefHi_reg 48
#define TC2K_ifCalVrefHi_mask 127

#define TC2K_ifCapWdCtrl_reg 48
#define TC2K_ifCapWdCtrl_mask 128
#define TC2K_ifCapWdCtrl_B2W 0
#define TC2K_ifCapWdCtrl_SM 128

#define TC2K_swRevMajor_reg 93
#define TC2K_swRevMajor_mask 255

#define TC2K_swRevMinor_reg 95
#define TC2K_swRevMinor_mask 255

#define TC2K_swRevBuild_reg 97
#define TC2K_swRevBuild_mask 255

#define TC2K_bbHysMaxCnt_reg 104
#define TC2K_bbHysMaxCnt_mask 3
#define TC2K_bbHysMaxCnt_time10p9msecs 0
#define TC2K_bbHysMaxCnt_time21p8msecs 1
#define TC2K_bbHysMaxCnt_time43p7msecs 2
#define TC2K_bbHysMaxCnt_time87p4msecs 3

#define TC2K_bbHysDelta_reg 104
#define TC2K_bbHysDelta_mask 12
#define TC2K_bbHysDelta_window16 0
#define TC2K_bbHysDelta_window64 4
#define TC2K_bbHysDelta_window256 8
#define TC2K_bbHysDelta_window1024 12

#define TC2K_rfHystDelta_reg 106
#define TC2K_rfHystDelta_mask 12
#define TC2K_rfHystDelta_window16 0

#define TC2K_dcHysADCMax_reg 106
#define TC2K_dcHysADCMax_mask 240
#define TC2K_dcHysADCMax_ADCMx128 0
#define TC2K_dcHysADCMax_ADCMx256 16
#define TC2K_dcHysADCMax_ADCMx384 32
#define TC2K_dcHysADCMax_ADCMx512 48
#define TC2K_dcHysADCMax_ADCMx640 64
#define TC2K_dcHysADCMax_ADCMx768 80
#define TC2K_dcHysADCMax_ADCMx896 96
#define TC2K_dcHysADCMax_ADCMx1024 112
#define TC2K_dcHysADCMax_ADCMx1152 128
#define TC2K_dcHysADCMax_ADCMx1280 144
#define TC2K_dcHysADCMax_ADCMx1408 160
#define TC2K_dcHysADCMax_ADCMx1536 176
#define TC2K_dcHysADCMax_ADCMx1664 192
#define TC2K_dcHysADCMax_ADCMx1792 208
#define TC2K_dcHysADCMax_ADCMx1920 224
#define TC2K_dcHysADCMax_ADCMx2048 240

#define TC2K_ifAgcSrc_reg 122
#define TC2K_ifAgcSrc_mask 32
#define TC2K_ifAgcSrc_useB2wFnlAgc 0
#define TC2K_ifAgcSrc_useDemodFnlAgc 32

#define TC2K_ifNlo_reg 125
#define TC2K_ifNlo_mask 255

#define TC2K_ifNhi_reg 126
#define TC2K_ifNhi_mask 255

#define TC2K_specInv_reg 127
#define TC2K_specInv_mask 8
#define TC2K_specInv_off 0
#define TC2K_specInv_on 8

#define TC2K_reset_reg 129
#define TC2K_reset_mask 1
#define TC2K_reset_Reset 0
#define TC2K_reset_NotReset 1

#define TC2K_digDCbwFst_reg 132
#define TC2K_digDCbwFst_mask 15
#define TC2K_digDCbwFst_BW29p8kHz 0
#define TC2K_digDCbwFst_BW14p9kHz 1
#define TC2K_digDCbwFst_BW7p5kHz 2
#define TC2K_digDCbwFst_BW3p7kHz 3
#define TC2K_digDCbwFst_BW1p9kHz 4
#define TC2K_digDCbwFst_BW933Hz 5
#define TC2K_digDCbwFst_BW466Hz 6
#define TC2K_digDCbwFst_BW233Hz 7
#define TC2K_digDCbwFst_BW117Hz 8
#define TC2K_digDCbwFst_BW58Hz 9
#define TC2K_digDCbwFst_BW29Hz 10
#define TC2K_digDCbwFst_BW14p6Hz 11
#define TC2K_digDCbwFst_BW7p3Hz 12
#define TC2K_digDCbwFst_BW3p6Hz 13
#define TC2K_digDCbwFst_BW1p8Hz 14
#define TC2K_digDCbwFst_BW0p9Hz 15

#define TC2K_IQPhBwFst_reg 133
#define TC2K_IQPhBwFst_mask 15
#define TC2K_IQPhBwFst_BW3p7kHz 0
#define TC2K_IQPhBwFst_BW1p9kHz 1
#define TC2K_IQPhBwFst_BW933Hz 2
#define TC2K_IQPhBwFst_BW466Hz 3
#define TC2K_IQPhBwFst_BW233Hz 4
#define TC2K_IQPhBwFst_BW117Hz 5
#define TC2K_IQPhBwFst_BW58Hz 6
#define TC2K_IQPhBwFst_BW29Hz 7
#define TC2K_IQPhBwFst_BW14p6Hz 8
#define TC2K_IQPhBwFst_BW7p3Hz 9
#define TC2K_IQPhBwFst_BW3p6Hz 10
#define TC2K_IQPhBwFst_BW1p8Hz 11
#define TC2K_IQPhBwFst_BW0p9Hz 12
#define TC2K_IQPhBwFst_BW0p5Hz 13
#define TC2K_IQPhBwFst_BW0p2Hz 14
#define TC2K_IQPhBwFst_BW0p1Hz 15

#define TC2K_IQGainBwFst_reg 134
#define TC2K_IQGainBwFst_mask 15
#define TC2K_IQGainBwFst_BW7p5kHz 0
#define TC2K_IQGainBwFst_BW3p7kHz 1
#define TC2K_IQGainBwFst_BW1p9Hz 2
#define TC2K_IQGainBwFst_BW933Hz 3
#define TC2K_IQGainBwFst_BW466Hz 4
#define TC2K_IQGainBwFst_BW233Hz 5
#define TC2K_IQGainBwFst_BW117Hz 6
#define TC2K_IQGainBwFst_BW58Hz 7
#define TC2K_IQGainBwFst_BW29p1Hz 8
#define TC2K_IQGainBwFst_BW14p6Hz 9
#define TC2K_IQGainBwFst_BW7p3Hz 10
#define TC2K_IQGainBwFst_BW3p6Hz 11
#define TC2K_IQGainBwFst_BW1p8Hz 12
#define TC2K_IQGainBwFst_BW0p9Hz 13
#define TC2K_IQGainBwFst_BW0p5Hz 14
#define TC2K_IQGainBwFst_BW0p2Hz 15

#define TC2K_IQGainBwSlw_reg 134
#define TC2K_IQGainBwSlw_mask 240
#define TC2K_IQGainBwSlw_BW7p5kHz 0
#define TC2K_IQGainBwSlw_BW3p7kHz 16
#define TC2K_IQGainBwSlw_BW1p9Hz 32
#define TC2K_IQGainBwSlw_BW933Hz 48
#define TC2K_IQGainBwSlw_BW466Hz 64
#define TC2K_IQGainBwSlw_BW233Hz 80
#define TC2K_IQGainBwSlw_BW117Hz 96
#define TC2K_IQGainBwSlw_BW58Hz 112
#define TC2K_IQGainBwSlw_BW29p1Hz 128
#define TC2K_IQGainBwSlw_BW14p6Hz 144
#define TC2K_IQGainBwSlw_BW7p3Hz 160
#define TC2K_IQGainBwSlw_BW3p6Hz 176
#define TC2K_IQGainBwSlw_BW1p8Hz 192
#define TC2K_IQGainBwSlw_BW0p9Hz 208
#define TC2K_IQGainBwSlw_BW0p5Hz 224
#define TC2K_IQGainBwSlw_BW0p2Hz 240

#define TC2K_lcCtrl_reg 136
#define TC2K_lcCtrl_mask 8
#define TC2K_lcCtrl_off 0
#define TC2K_lcCtrl_on 8

#define TC2K_lccLo_reg 137
#define TC2K_lccLo_mask 255

#define TC2K_lccMid_reg 138
#define TC2K_lccMid_mask 255

#define TC2K_lccHi_reg 139
#define TC2K_lccHi_mask 63

#define TC2K_lcsLo_reg 140
#define TC2K_lcsLo_mask 255

#define TC2K_lcsMid_reg 141
#define TC2K_lcsMid_mask 255

#define TC2K_lcsHi_reg 142
#define TC2K_lcsHi_mask 63

#define TC2K_rotLo_reg 143
#define TC2K_rotLo_mask 255

#define TC2K_rotMid_reg 144
#define TC2K_rotMid_mask 255

#define TC2K_rotHi_reg 145
#define TC2K_rotHi_mask 15

#define TC2K_agcAccWrLo_reg 146
#define TC2K_agcAccWrLo_mask 255

#define TC2K_agcAccWrHi_reg 147
#define TC2K_agcAccWrHi_mask 255

#define TC2K_agcAccSel_reg 149
#define TC2K_agcAccSel_mask 15
#define TC2K_agcAccSel_BB 0
#define TC2K_agcAccSel_RF 1
#define TC2K_agcAccSel_IDCA 2
#define TC2K_agcAccSel_QDCA 3
#define TC2K_agcAccSel_IDCD 4
#define TC2K_agcAccSel_QDCD 5
#define TC2K_agcAccSel_PhIQ 6
#define TC2K_agcAccSel_AmpIQ 7
#define TC2K_agcAccSel_LineCancelI 8
#define TC2K_agcAccSel_LineCancelQ 9
#define TC2K_agcAccSel_DigitalAGC 10
#define TC2K_agcAccSel_RFDetADC 11
#define TC2K_agcAccSel_InDAGCADC 12
#define TC2K_agcAccSel_RfZeroPwr 13

#define TC2K_agcAccLoad_reg 149
#define TC2K_agcAccLoad_mask 128
#define TC2K_agcAccLoad_disWr 0
#define TC2K_agcAccLoad_enaWr 128

#define TC2K_ifAgcEna_reg 150
#define TC2K_ifAgcEna_mask 1
#define TC2K_ifAgcEna_off 0
#define TC2K_ifAgcEna_on 1

#define TC2K_rfCloseLoop_reg 150
#define TC2K_rfCloseLoop_mask 4
#define TC2K_rfCloseLoop_off 0
#define TC2K_rfCloseLoop_on 4

#define TC2K_bbAdcBckOff_reg 151
#define TC2K_bbAdcBckOff_mask 15
#define TC2K_bbAdcBckOff_BkOffFrmFS_0dB 0
#define TC2K_bbAdcBckOff_BkOffFrmFS_1dB 1
#define TC2K_bbAdcBckOff_BkOffFrmFS_2dB 2
#define TC2K_bbAdcBckOff_BkOffFrmFS_3dB 3
#define TC2K_bbAdcBckOff_BkOffFrmFS_4dB 4
#define TC2K_bbAdcBckOff_BkOffFrmFS_5dB 5
#define TC2K_bbAdcBckOff_BkOffFrmFS_6dB 6
#define TC2K_bbAdcBckOff_BkOffFrmFS_7dB 7
#define TC2K_bbAdcBckOff_BkOffFrmFS_8dB 8
#define TC2K_bbAdcBckOff_BkOffFrmFS_9dB 9
#define TC2K_bbAdcBckOff_BkOffFrmFS_10dB 10
#define TC2K_bbAdcBckOff_BkOffFrmFS_11dB 11
#define TC2K_bbAdcBckOff_BkOffFrmFS_12dB 12
#define TC2K_bbAdcBckOff_BkOffFrmFS_13dB 13
#define TC2K_bbAdcBckOff_BkOffFrmFS_14dB 14
#define TC2K_bbAdcBckOff_BkOffFrmFS_15dB 15

#define TC2K_ifAgcShift_reg 151
#define TC2K_ifAgcShift_mask 112
#define TC2K_ifAgcShift_bbagcshft_0 0
#define TC2K_ifAgcShift_bbagcshft_1 16
#define TC2K_ifAgcShift_bbagcshft_2 32
#define TC2K_ifAgcShift_bbagcshft_3 48
#define TC2K_ifAgcShift_bbagcshft_4 64
#define TC2K_ifAgcShift_bbagcshft_5 80
#define TC2K_ifAgcShift_bbagcshft_6 96
#define TC2K_ifAgcShift_bbagcshft_7 112

#define TC2K_ifVga2Ena_reg 151
#define TC2K_ifVga2Ena_mask 128
#define TC2K_ifVga2Ena_off 0
#define TC2K_ifVga2Ena_on 128

#define TC2K_ifAgcAccTopLo_reg 152
#define TC2K_ifAgcAccTopLo_mask 255

#define TC2K_ifAgcAccTopHi_reg 153
#define TC2K_ifAgcAccTopHi_mask 15

#define TC2K_ifAgcLoopBw_reg 154
#define TC2K_ifAgcLoopBw_mask 7
#define TC2K_ifAgcLoopBw_BW_4p8Hz 0
#define TC2K_ifAgcLoopBw_BW_2p4Hz 1
#define TC2K_ifAgcLoopBw_BW_1p2Hz 2
#define TC2K_ifAgcLoopBw_BW_0p6Hz 3
#define TC2K_ifAgcLoopBw_BW_0p3Hz 4

#define TC2K_rfDetEna_reg 154
#define TC2K_rfDetEna_mask 16
#define TC2K_rfDetEna_off 0
#define TC2K_rfDetEna_on 16

#define TC2K_rfDetSetPt_reg 155
#define TC2K_rfDetSetPt_mask 15
#define TC2K_rfDetSetPt_Vdet_0p1 0
#define TC2K_rfDetSetPt_Vdet_0p12 1
#define TC2K_rfDetSetPt_Vdet_0p14 2
#define TC2K_rfDetSetPt_Vdet_0p16 3
#define TC2K_rfDetSetPt_Vdet_0p18 4
#define TC2K_rfDetSetPt_Vdet_0p2 5
#define TC2K_rfDetSetPt_Vdet_0p22 6
#define TC2K_rfDetSetPt_Vdet_0p24 7
#define TC2K_rfDetSetPt_Vdet_0p26 8
#define TC2K_rfDetSetPt_Vdet_0p28 9
#define TC2K_rfDetSetPt_Vdet_0p3 10
#define TC2K_rfDetSetPt_Vdet_0p32 11
#define TC2K_rfDetSetPt_Vdet_0p34 12
#define TC2K_rfDetSetPt_Vdet_0p36 13
#define TC2K_rfDetSetPt_Vdet_0p38 14
#define TC2K_rfDetSetPt_Vdet_0p4 15

#define TC2K_agcWindow_reg 155
#define TC2K_agcWindow_mask 112
#define TC2K_agcWindow_rfWndwSz_5p33uS 0
#define TC2K_agcWindow_rfWndwSz_10p67uS 16
#define TC2K_agcWindow_rfWndwSz_21p33uS 32
#define TC2K_agcWindow_rfWndwSz_42p67uS 48
#define TC2K_agcWindow_rfWndwSz_85p33uS 64
#define TC2K_agcWindow_rfWndwSz_170p67uS 80
#define TC2K_agcWindow_rfWndwSz_341p33uS 96
#define TC2K_agcWindow_rfWndwSz_682p67uS 112

#define TC2K_rfAgcLoopBw_reg 156
#define TC2K_rfAgcLoopBw_mask 112
#define TC2K_rfAgcLoopBw_BW80Hz 0
#define TC2K_rfAgcLoopBw_BW40Hz 16
#define TC2K_rfAgcLoopBw_BW20Hz 32
#define TC2K_rfAgcLoopBw_BW10Hz 48
#define TC2K_rfAgcLoopBw_BW5Hz 64
#define TC2K_rfAgcLoopBw_BW2p5Hz 80
#define TC2K_rfAgcLoopBw_reg156LSBp06 96
#define TC2K_rfAgcLoopBw_reg156LSBp07 112

#define TC2K_rfDetCalAve_reg 156
#define TC2K_rfDetCalAve_mask 128
#define TC2K_rfDetCalAve_off 0
#define TC2K_rfDetCalAve_on 128

#define TC2K_dcILoopBw_reg 157
#define TC2K_dcILoopBw_mask 7
#define TC2K_dcILoopBw_BW400Hz 0
#define TC2K_dcILoopBw_BW200Hz 1
#define TC2K_dcILoopBw_BW100Hz 2
#define TC2K_dcILoopBw_BW50Hz 3
#define TC2K_dcILoopBw_BW25Hz 4
#define TC2K_dcILoopBw_BW12p5Hz 5
#define TC2K_dcILoopBw_BW6p25Hz 6
#define TC2K_dcILoopBw_BW3p125Hz 7

#define TC2K_dcQLoopBw_reg 157
#define TC2K_dcQLoopBw_mask 112
#define TC2K_dcQLoopBw_BW400Hz 0
#define TC2K_dcQLoopBw_BW200Hz 16
#define TC2K_dcQLoopBw_BW100Hz 32
#define TC2K_dcQLoopBw_BW50Hz 48
#define TC2K_dcQLoopBw_BW25Hz 64
#define TC2K_dcQLoopBw_BW12p5Hz 80
#define TC2K_dcQLoopBw_BW6p25Hz 96
#define TC2K_dcQLoopBw_BW3p125Hz 112

#define TC2K_rfDetCalFrc_reg 158
#define TC2K_rfDetCalFrc_mask 1
#define TC2K_rfDetCalFrc_off 0
#define TC2K_rfDetCalFrc_on 1

#define TC2K_rfDetCalAuto_reg 158
#define TC2K_rfDetCalAuto_mask 2
#define TC2K_rfDetCalAuto_off 0
#define TC2K_rfDetCalAuto_on 2

#define TC2K_agcSmEna_reg 158
#define TC2K_agcSmEna_mask 4
#define TC2K_agcSmEna_off 0
#define TC2K_agcSmEna_on 4

#define TC2K_dcHysMode_reg 158
#define TC2K_dcHysMode_mask 8
#define TC2K_dcHysMode_off 0
#define TC2K_dcHysMode_on 8

#define TC2K_rfHysMode_reg 158
#define TC2K_rfHysMode_mask 16
#define TC2K_rfHysMode_off 0
#define TC2K_rfHysMode_on 16

#define TC2K_bbHysMode_reg 158
#define TC2K_bbHysMode_mask 32
#define TC2K_bbHysMode_off 0
#define TC2K_bbHysMode_on 32

#define TC2K_iAttLimitLo_reg 159
#define TC2K_iAttLimitLo_mask 255

#define TC2K_iAttLimitHi_reg 160
#define TC2K_iAttLimitHi_mask 15

#define TC2K_iAttLimitTest_reg 160
#define TC2K_iAttLimitTest_mask 32
#define TC2K_iAttLimitTest_off 0
#define TC2K_iAttLimitTest_on 32

#define TC2K_shPinLimitLo_reg 161
#define TC2K_shPinLimitLo_mask 255

#define TC2K_shPinLimitHi_reg 162
#define TC2K_shPinLimitHi_mask 15

#define TC2K_shPinLimitTest_reg 162
#define TC2K_shPinLimitTest_mask 128
#define TC2K_shPinLimitTest_off 0
#define TC2K_shPinLimitTest_on 128

#define TC2K_sePinLimitLo_reg 163
#define TC2K_sePinLimitLo_mask 255

#define TC2K_sePinLimitHi_reg 164
#define TC2K_sePinLimitHi_mask 15

#define TC2K_sePinLimitTest_reg 164
#define TC2K_sePinLimitTest_mask 128
#define TC2K_sePinLimitTest_off 0
#define TC2K_sePinLimitTest_on 128

#define TC2K_digAgcBckOff_reg 165
#define TC2K_digAgcBckOff_mask 3
#define TC2K_digAgcBckOff_neg8p5dB 0
#define TC2K_digAgcBckOff_neg6dB 1
#define TC2K_digAgcBckOff_neg2p5dB 2
#define TC2K_digAgcBckOff_reg165LSBp03 3

#define TC2K_dagcWdwSzVal_reg 165
#define TC2K_dagcWdwSzVal_mask 28
#define TC2K_dagcWdwSzVal_WndwSz_21p33us 0
#define TC2K_dagcWdwSzVal_WndwSz_42p67us 4
#define TC2K_dagcWdwSzVal_WndwSz_85p33us 8
#define TC2K_dagcWdwSzVal_WndwSz_170p67us 12
#define TC2K_dagcWdwSzVal_WndwSz_341p33us 16
#define TC2K_dagcWdwSzVal_WndwSz_682p67us 20
#define TC2K_dagcWdwSzVal_reg165LSBp06 24
#define TC2K_dagcWdwSzVal_reg165LSBp07 28

#define TC2K_digAgcEna_reg 166
#define TC2K_digAgcEna_mask 1
#define TC2K_digAgcEna_off 0
#define TC2K_digAgcEna_on 1

#define TC2K_digAgcBW_reg 166
#define TC2K_digAgcBW_mask 112
#define TC2K_digAgcBW_BW22p5Hz 0
#define TC2K_digAgcBW_BW45Hz 16
#define TC2K_digAgcBW_BW90Hz 32
#define TC2K_digAgcBW_BW180Hz 48
#define TC2K_digAgcBW_BW360Hz 64
#define TC2K_digAgcBW_BW720Hz 80
#define TC2K_digAgcBW_BW1440Hz 96
#define TC2K_digAgcBW_BW2880Hz 112

#define TC2K_digAgcInSel_reg 166
#define TC2K_digAgcInSel_mask 128
#define TC2K_digAgcInSel_Translator_Out 0
#define TC2K_digAgcInSel_Interp_Out 128

#define TC2K_ttSrc_reg 167
#define TC2K_ttSrc_mask 1
#define TC2K_ttSrc_TransNCO 0
#define TC2K_ttSrc_SpinNCO 1

#define TC2K_ttEna_reg 167
#define TC2K_ttEna_mask 32
#define TC2K_ttEna_off 0
#define TC2K_ttEna_on 32

#define TC2K_tfcTrig_reg 167
#define TC2K_tfcTrig_mask 64
#define TC2K_tfcTrig_ClearTrig 0
#define TC2K_tfcTrig_Trig 64

#define TC2K_tfcCapWd0_reg 168
#define TC2K_tfcCapWd0_mask 63

#define TC2K_tfcCapWdMax_reg 168
#define TC2K_tfcCapWdMax_mask 64
#define TC2K_tfcCapWdMax_Max31 0
#define TC2K_tfcCapWdMax_Max63 64

#define TC2K_tfcCapCtrl_reg 168
#define TC2K_tfcCapCtrl_mask 128
#define TC2K_tfcCapCtrl_B2WWrite 0
#define TC2K_tfcCapCtrl_SMWrite 128

#define TC2K_ifLevel_reg 175
#define TC2K_ifLevel_mask 15
#define TC2K_ifLevel_FSRngRed_Nom 0
#define TC2K_ifLevel_FSRngRed_neg0p5 1
#define TC2K_ifLevel_FSRngRed_neg1 2
#define TC2K_ifLevel_FSRngRed_neg1p6 3
#define TC2K_ifLevel_FSRngRed_neg2 4
#define TC2K_ifLevel_FSRngRed_neg2p3 5
#define TC2K_ifLevel_FSRngRed_neg2p8 6
#define TC2K_ifLevel_FSRngRed_neg3p2 7
#define TC2K_ifLevel_FSRngRed_pos6 8
#define TC2K_ifLevel_FSRngRed_pos5 9
#define TC2K_ifLevel_FSRngRed_pos4 10
#define TC2K_ifLevel_FSRngRed_pos3p3 11
#define TC2K_ifLevel_FSRngRed_pos2p5 12
#define TC2K_ifLevel_FSRngRed_pos1p8 13
#define TC2K_ifLevel_FSRngRed_pos1p2 14
#define TC2K_ifLevel_FSRngRed_pos0p6 15

#define TC2K_ifOut2_reg 175
#define TC2K_ifOut2_mask 16
#define TC2K_ifOut2_off 0
#define TC2K_ifOut2_on 16

#define TC2K_ifOut1_reg 175
#define TC2K_ifOut1_mask 32
#define TC2K_ifOut1_off 0
#define TC2K_ifOut1_on 32

#define TC2K_IfDacNrzEna_reg 175
#define TC2K_IfDacNrzEna_mask 64
#define TC2K_IfDacNrzEna_off 0
#define TC2K_IfDacNrzEna_on 64

#define TC2K_shPinDacCtrl_reg 180
#define TC2K_shPinDacCtrl_mask 1
#define TC2K_shPinDacCtrl_off 0
#define TC2K_shPinDacCtrl_on 1

#define TC2K_sePinDacCtrl_reg 180
#define TC2K_sePinDacCtrl_mask 2
#define TC2K_sePinDacCtrl_off 0
#define TC2K_sePinDacCtrl_on 2

#define TC2K_iAttDacCtrl_reg 180
#define TC2K_iAttDacCtrl_mask 4
#define TC2K_iAttDacCtrl_off 0
#define TC2K_iAttDacCtrl_on 4

#define TC2K_ttGenDiv_reg 186
#define TC2K_ttGenDiv_mask 1
#define TC2K_ttGenDiv_off 0
#define TC2K_ttGenDiv_on 1

#define TC2K_reg186b1w7_reg 186
#define TC2K_reg186b1w7_mask 254

#endif
