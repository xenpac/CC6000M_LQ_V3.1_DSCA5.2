/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*******************************************************************************
* This file is licensed under the terms of your license agreement(s) with	     *
* Montage covering this file. Redistribution, except as permitted by the terms *
* of your license agreement(s) with Montage, is strictly prohibited.		       *
********************************************************************************
*
* SVN Code File Version $Rev: 14975 $
*
*******************************************************************************/

/*******************************************************************************
	Version		Date	Descriptions				Author
********************************************************************************
	1.00.00		100907	Initialize version			Youzhong.Huang
	2.01.03		100908	Modified					Youzhong.Huang
	2.01.04		101122	Modified					Youzhong.Huang
*******************************************************************************/
#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "i2c.h"
#include "nim.h"
#include "mt_fe_def.h"
#include "nim_m88dc2800.h"
#include "mt_fe_tn_tc2800.h"
#include "mt_fe_tn_tc2000.h"
#include "mt_fe_common.h"

#define _mt_sleep mtos_task_delay_ms

tc2000_tn_config tc2k_default = {0};

MT_FE_DC2800_Device_Handle tc2000_handle = NULL;

static S32 _i2c_user_2wb_wr(U8 b2wAdx, U8 icReg, U8 *data, U8 byteCnt)
{
	U8			buf[256];
	U16			i;

	buf[0] = icReg;
	for(i = 0; i < byteCnt; i ++)
	{
		buf[i + 1] =  data[i];
	}
    //dc2800_tn_seq_write(b2wAdx, buf, byteCnt+1);
    tc2000_handle->tn_write(tc2000_handle, buf, (unsigned short)(byteCnt + 1));
	return 0;
}


static S32 _i2c_user_2wb_rd(U8 b2wAdx, U8 icReg, U8 *data, U8 byteCnt)
{
    data[0] = icReg;
   // dc2800_tn_std_read(b2wAdx, data, 1, byteCnt);

    U8 p_buf[128];
    U8 i = 0;

	tc2000_handle->tn_write(tc2000_handle, data, 1);

	tc2000_handle->tn_read(tc2000_handle, p_buf, (unsigned short)byteCnt);

	for(i = 0; i < byteCnt; i ++)
	{
		data[i] = p_buf[i];
	}

	return 0;
}


/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start Main API Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

TC2K_RET mt_fe_tn_tc2000_init(tc2000_tn_config* tuner_cfg)
{
	TC2K_RET ret;
     
	if(tuner_cfg->init_OK == 0)
	{
		tuner_cfg->b2wAddress		 = TC2K_b2wAddress_C0;
		tuner_cfg->ltaEnaSel		 = TC2K_ltaEnaSel_on;
		tuner_cfg->broadcastMedium	 = TC2K_broadcastMedium_cable;
		tuner_cfg->rfAttMode		 = TC2K_rfAttMode_seriesPin_internalAtt;
		tuner_cfg->rfAttBias		 = TC2K_rfAttBias_1p8v;
		tuner_cfg->specInvSel		 = TC2K_specInvSel_off;
		tuner_cfg->ifBw				 = TC2K_ifBw_8MHz;
		tuner_cfg->FifHz			 = 36000000;
		tuner_cfg->ifOutputSel		 = TC2K_ifOutputSel_1;
		tuner_cfg->ifOutPutLevel_IF1 = TC2K_ifOutputLevel_0_dB_nominal;
		tuner_cfg->ifOutPutLevel_IF2 = TC2K_ifOutputLevel_0_dB_nominal;
		tuner_cfg->ifDacMode_IF1	 = TC2K_ifDacMode_Normal;
		tuner_cfg->ifDacMode_IF2	 = TC2K_ifDacMode_Normal;
		tuner_cfg->agcCtrlMode		 = TC2K_agcCtrlMode_demod;
		tuner_cfg->modulation		 = TC2K_modulation_DVB_C;
		tuner_cfg->tunerCnt			 = TC2K_tunerCnt_single;
		tuner_cfg->rxPwr			 = TC2K_rxPwr_auto;
        
              tc2000_handle->tuner_settings.tuner_dev_addr = TC2K_b2wAddress_C0;
	}

	ret = mt_fe_tn_tc2000_active(tuner_cfg);

	return ret;
}



/*
 * mt_fe_tn_tc2000_active()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_invalid_parameter if bad input
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET mt_fe_tn_tc2000_active(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	tunerCfg->tunerData.tunerCalibrated = TC2K_tunerCalibrated_no;
	tunerCfg->tunerData.tunerIdle = TC2K_tunerIdle_yes;

	/* @doc Initialize Reg data */
	tunerCfg->tunerData.b2wReg1 = 255;
	tunerCfg->tunerData.b2wRegN = 0;

	/* @doc Write/rewrite the driver revision to the ic for debug. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMajor_reg, TC2K_swRevMajor_mask, TC2K_swRevMajor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMinor_reg, TC2K_swRevMinor_mask, TC2K_swRevMinor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevBuild_reg, TC2K_swRevBuild_mask, TC2K_swRevBuild, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Check input parameters. */
	switch(tunerCfg->b2wAddress) {
		case TC2K_b2wAddress_C0:
		case TC2K_b2wAddress_C2:
		case TC2K_b2wAddress_C4:
		case TC2K_b2wAddress_C6:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	if ((tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_off) && (tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_on))
		return TC2K_err_invalid_parameter;

	if ((tunerCfg->tunerCnt != TC2K_tunerCnt_single) && (tunerCfg->tunerCnt != TC2K_tunerCnt_dual))
		return TC2K_err_invalid_parameter;

	switch (tunerCfg->rfAttMode) {
	case TC2K_rfAttMode_seriesPin_shuntPin:
	case TC2K_rfAttMode_seriesPin_internalAtt:
	case TC2K_rfAttMode_seriesPin_shuntPin_internalAtt:
		break;
	default:
		return TC2K_err_invalid_parameter;
	}

	switch(tunerCfg->rfAttBias) {
		case TC2K_rfAttBias_1p8v:
		case TC2K_rfAttBias_3p3v:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}
	/* End: check input parameters */

	/* @doc Check for valid IC revision for this code. */
	retVal = _mt_fe_tn_tc2000_cfgIcRev(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_freqSetFxtal(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_ifCalBW(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_ifAccTopCal(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_tfCal(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc load base configuration for normal operation. */
	retVal = _mt_fe_tn_tc2000_cfgBase(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Store temperature at calibration in tuner data structure. */
	retVal = _mt_fe_tn_tc2000_cfgReadTmprAcc(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	tunerCfg->tunerData.calTmpr = tunerCfg->tunerData.curTmpr;

	/* @doc Signal mt_fe_tn_tc2000_set_chan() that the basCfg is programmed (tuner is not in idle state). */
	tunerCfg->tunerData.tunerIdle = TC2K_tunerIdle_no;

	/* @doc Set tuner active to true in tuner data structure. */
	tunerCfg->tunerData.tunerCalibrated = TC2K_tunerCalibrated_yes;

	tunerCfg->rxPwr = TC2K_rxPwr_auto;

	tunerCfg->init_OK = 1;

	return TC2K_ok;
}

/*
 * mt_fe_tn_tc2000_set_chan()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_invalid_parameter if bad input
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
 TC2K_RET mt_fe_tn_tc2000_set_chan(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc Exit with error if tuner is not active. */
	if (tunerCfg->tunerData.tunerCalibrated != TC2K_tunerCalibrated_yes)
		return TC2K_err_not_calibrated;

	/* @doc Check input parameters to _mt_fe_tn_tc2000_cfgBase(). */
	if ((tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_off) && (tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_on))
		return TC2K_err_invalid_parameter;

	if ((tunerCfg->tunerCnt != TC2K_tunerCnt_single) && (tunerCfg->tunerCnt != TC2K_tunerCnt_dual))
		return TC2K_err_invalid_parameter;

	switch (tunerCfg->rfAttMode) {
	case TC2K_rfAttMode_seriesPin_shuntPin:
	case TC2K_rfAttMode_seriesPin_internalAtt:
	case TC2K_rfAttMode_seriesPin_shuntPin_internalAtt:
		break;
	default:
		return TC2K_err_invalid_parameter;
	}

	switch(tunerCfg->rfAttBias) {
		case TC2K_rfAttBias_1p8v:
		case TC2K_rfAttBias_3p3v:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	/* @doc Check input parameters. */
	if ((tunerCfg->FchanHz < TC2K_FchanMin) || (tunerCfg->FchanHz > TC2K_FchanMax))
		return TC2K_err_invalid_parameter;

	if ((tunerCfg->FifHz < 0) || (tunerCfg->FifHz > tunerCfg->tunerData.FifMaxHz))
		return TC2K_err_invalid_parameter;

	switch (tunerCfg->broadcastMedium) {
		case TC2K_broadcastMedium_cable:
		case TC2K_broadcastMedium_ota:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->specInvSel) {
		case TC2K_specInvSel_off:
		case TC2K_specInvSel_on:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifBw) {
		case TC2K_ifBw_6MHz:
		case TC2K_ifBw_7MHz:
		case TC2K_ifBw_8MHz:
		case TC2K_ifBw_User:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifOutputSel) {
		case TC2K_ifOutputSel_1:
		case TC2K_ifOutputSel_2:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifDacMode_IF1) {
		case TC2K_ifDacMode_Normal:
		case TC2K_ifDacMode_Optimize:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifDacMode_IF2) {
		case TC2K_ifDacMode_Normal:
		case TC2K_ifDacMode_Optimize:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifOutPutLevel_IF1) {
		case TC2K_ifOutputLevel_Auto:
		case TC2K_ifOutputLevel_neg3p2_dB:
		case TC2K_ifOutputLevel_neg2p8_dB:
		case TC2K_ifOutputLevel_neg2p3_dB:
		case TC2K_ifOutputLevel_neg2p0_dB:
		case TC2K_ifOutputLevel_neg1p6_dB:
		case TC2K_ifOutputLevel_neg1p0_dB:
		case TC2K_ifOutputLevel_neg0p5_dB:
		case TC2K_ifOutputLevel_0_dB_nominal:
		case TC2K_ifOutputLevel_0p6_dB:
		case TC2K_ifOutputLevel_1p2_dB:
		case TC2K_ifOutputLevel_1p8_dB:
		case TC2K_ifOutputLevel_2p5_dB:
		case TC2K_ifOutputLevel_3p3_dB:
		case TC2K_ifOutputLevel_4p0_dB:
		case TC2K_ifOutputLevel_5p0_dB:
		case TC2K_ifOutputLevel_6p0_dB:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->ifOutPutLevel_IF2) {
		case TC2K_ifOutputLevel_Auto:
		case TC2K_ifOutputLevel_neg3p2_dB:
		case TC2K_ifOutputLevel_neg2p8_dB:
		case TC2K_ifOutputLevel_neg2p3_dB:
		case TC2K_ifOutputLevel_neg2p0_dB:
		case TC2K_ifOutputLevel_neg1p6_dB:
		case TC2K_ifOutputLevel_neg1p0_dB:
		case TC2K_ifOutputLevel_neg0p5_dB:
		case TC2K_ifOutputLevel_0_dB_nominal:
		case TC2K_ifOutputLevel_0p6_dB:
		case TC2K_ifOutputLevel_1p2_dB:
		case TC2K_ifOutputLevel_1p8_dB:
		case TC2K_ifOutputLevel_2p5_dB:
		case TC2K_ifOutputLevel_3p3_dB:
		case TC2K_ifOutputLevel_4p0_dB:
		case TC2K_ifOutputLevel_5p0_dB:
		case TC2K_ifOutputLevel_6p0_dB:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->agcCtrlMode) {
		case TC2K_agcCtrlMode_demod:
		case TC2K_agcCtrlMode_internal:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->modulation) {
		case TC2K_modulation_DVB_C:
		case TC2K_modulation_J83_Annex_B:
		case TC2K_modulation_DVB_T:
		case TC2K_modulation_ATSC:
		case TC2K_modulation_ISDB_T:
		case TC2K_modulation_DMB_T:
		case TC2K_modulation_PAL_B:
		case TC2K_modulation_PAL_D_I_K:
		case TC2K_modulation_PAL_G_H:
		case TC2K_modulation_NTSC:
		case TC2K_modulation_SECAM:
		case TC2K_modulation_SECAM_L:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	switch (tunerCfg->rxPwr) {
		case TC2K_rxPwr_low:
		case TC2K_rxPwr_nominal:
		case TC2K_rxPwr_medium:
		case TC2K_rxPwr_high:
		case TC2K_rxPwr_auto:
			break;
		default:
			return TC2K_err_invalid_parameter;
	}
	/* End check param. */
	if (tunerCfg->tunerData.tunerIdle == TC2K_tunerIdle_yes) {
		retVal = _mt_fe_tn_tc2000_cfgBase(tunerCfg);
		if (retVal != TC2K_ok)
			return retVal;
		tunerCfg->tunerData.tunerIdle = TC2K_tunerIdle_no;
	}
	/* @doc Configure IC based on user input. */
	retVal = _mt_fe_tn_tc2000_cfgIc (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Turn off so AGC does not rail. Executes b2w sync. */
	retVal = _mt_fe_tn_tc2000_cfgHystModeOff(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_cfgCalRfDet(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Set Flo, Fspin and Fif based on input parameters including modulation type. */
	retVal = _mt_fe_tn_tc2000_freqSetFchHz(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Frequency information in tuner structure must be current before the IF DAC is configured. */
	retVal = _mt_fe_tn_tc2000_cfgIfDac(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_freqSetLc(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_freqSetTfBand(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_tfChanChg(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_cfgChanChgOpt(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_cfgHystModeOn(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	return TC2K_ok;
}


/*
 * mt_fe_tn_tc2000_idle()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_invalid_parameter if bad input
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET mt_fe_tn_tc2000_idle(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc Check input parameters. */
	if ((tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_off) && (tunerCfg->ltaEnaSel != TC2K_ltaEnaSel_on))
		return TC2K_err_invalid_parameter;

	retVal = _mt_fe_tn_tc2000_cfgIdle(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->ltaEnaSel == TC2K_ltaEnaSel_on)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ltaEna_reg, TC2K_ltaEna_mask, TC2K_ltaEna_on, TC2K_2wbSync_yes);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ltaEna_reg, TC2K_ltaEna_mask, TC2K_ltaEna_off, TC2K_2wbSync_yes);

	/* @doc Signal mt_fe_tn_tc2000_set_chan() that the tuner is idle (must program the baseCfg before doing anything else). */
	tunerCfg->tunerData.tunerIdle = TC2K_tunerIdle_yes;

	return TC2K_ok;
}


TC2K_RET mt_fe_tn_tc2000_rssi(tc2000_tn_config *tunerCfg)
{
	U8 retVal;

	/* @doc Check input parameters. */
	if (tunerCfg->tunerData.tunerCalibrated != TC2K_tunerCalibrated_yes)
		return TC2K_err_not_calibrated;

	retVal = _mt_fe_tn_tc2000_cfgRssi (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*
 * mt_fe_tn_tc2000_status()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_freqPll_unlocked if not locked
 * 			TC2K_err_rw otherwise.
 *
 * @note Return lock condition of pll. There are no other status indicators in the TC2000.
 */
TC2K_RET mt_fe_tn_tc2000_status(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8 aLock;
	U8 dLock;

	/* @doc Check PLL Lock status (returned by function). */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synAnLock_reg, TC2K_synAnLock_mask, &aLock, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synDigLock_reg, TC2K_synDigLock_mask, &dLock, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	if ((aLock != TC2K_synAnLock_on) || (dLock != TC2K_synDigLock_on))
		return TC2K_err_freqPll_unlocked;

	return TC2K_ok;
}

/*
 * mt_fe_tn_tc2000_code_revision()
 *
 * @param[out] revision Current revision..
 *
 * @return  TC2K_ok
 *
 * @note None.
 */
TC2K_RET mt_fe_tn_tc2000_code_revision(U8 *revArray)
{
	revArray[0] = TC2K_swRevMajor;
	revArray[1] = TC2K_swRevMinor;
	revArray[2] = TC2K_swRevBuild;

	return TC2K_ok;
}

/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start Register access Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

/*
 * _mt_fe_tn_tc2000_set_reg()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[in] reg
 * @param[in] mask
 * @param[in] value
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called (indirectly) by mt_fe_tn_tc2000_set_chan().
 */
TC2K_RET _mt_fe_tn_tc2000_set_reg(tc2000_tn_config *tunerCfg, U8 reg, U8 mask, U8 value, TC2K_2wbSync sync)
{
#ifdef TC2K_RegImage
	S32 retVal;

	tunerCfg->tunerData.regImage[reg] &= ~mask;
	tunerCfg->tunerData.regImage[reg] |= value;

	if (reg < tunerCfg->tunerData.b2wReg1)
		tunerCfg->tunerData.b2wReg1 = reg;
	if (reg > tunerCfg->tunerData.b2wRegN)
		tunerCfg->tunerData.b2wRegN = reg;
	if (sync == TC2K_2wbSync_yes) {
		retVal = _i2c_user_2wb_wr ((U8) tunerCfg->b2wAddress, tunerCfg->tunerData.b2wReg1,
			&tunerCfg->tunerData.regImage[tunerCfg->tunerData.b2wReg1],
			(U8) (tunerCfg->tunerData.b2wRegN - tunerCfg->tunerData.b2wReg1 + 1));
		if (retVal != 0)
			return TC2K_err_rw;
		tunerCfg->tunerData.b2wReg1 = 255;
		tunerCfg->tunerData.b2wRegN = 0;
	}
	return TC2K_ok;
#else
	S32 retVal;
	U8 buff;
	retVal = _i2c_user_2wb_rd((U8) tunerCfg->b2wAddress,reg,&buff,1);
	if (retVal != 0)
		return TC2K_err_rw;
	buff &= ~mask;
	buff |= value;
	retVal = _i2c_user_2wb_wr((U8) tunerCfg->b2wAddress,reg,&buff,1);
	if (retVal != 0)
		return TC2K_err_rw;

	return TC2K_ok;
#endif
}

/*
 * _mt_fe_tn_tc2000_get_reg()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[in] reg
 * @param[in] mask
 * @param[out] value
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called (indirectly) by mt_fe_tn_tc2000_set_chan().
 */
TC2K_RET _mt_fe_tn_tc2000_get_reg(tc2000_tn_config *tunerCfg, U8 reg, U8 mask, U8 *value, TC2K_2wbSync sync)
{
#ifdef TC2K_RegImage
	S32 retVal;

	if (sync == TC2K_2wbSync_yes) {
		retVal = _i2c_user_2wb_rd((U8) tunerCfg->b2wAddress,0,&tunerCfg->tunerData.regImage[0],18);
		if (retVal != 0)
			return TC2K_err_rw;
	}
	*value = tunerCfg->tunerData.regImage[reg] & mask;

	return TC2K_ok;
#else
	S32 retVal;
	U8 buff;

	retVal = _i2c_user_2wb_rd((U8) tunerCfg->b2wAddress,reg,&buff,1);
	if (retVal != 0)
		return TC2K_err_rw;
	*value = buff & mask;

	return TC2K_ok;
#endif
}

/*
 * _mt_fe_tn_tc2000_i2c_write()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[in] regaddr address to write to.
 * @param[in] buf data.
 * @param[in] len length of data in bytes.
 *
 * @return  TC2K_ok
 *
 * @note The customer must provide the following I2C write
 * function in order for the TC2000 API software to function correctly..
 */
TC2K_RET _mt_fe_tn_tc2000_i2c_write(tc2000_tn_config *tunerCfg, U8 reg, U8 *buf, U8 len, TC2K_2wbSync sync)
{
#ifdef TC2K_RegImage
	S32 retVal;
	U8	x;

	for (x=0;x<len;++x)
		tunerCfg->tunerData.regImage[reg + x] = buf[x];

	if (reg < tunerCfg->tunerData.b2wReg1)
		tunerCfg->tunerData.b2wReg1 = reg;
	if (reg + len - 1 > tunerCfg->tunerData.b2wRegN)
		tunerCfg->tunerData.b2wRegN = reg  + len - 1;

	if (sync == TC2K_2wbSync_yes) {
		retVal = _i2c_user_2wb_wr ((U8) tunerCfg->b2wAddress, tunerCfg->tunerData.b2wReg1,
			&tunerCfg->tunerData.regImage[tunerCfg->tunerData.b2wReg1],
			(U8) (tunerCfg->tunerData.b2wRegN - tunerCfg->tunerData.b2wReg1 + 1));
		if (retVal != 0)
			return TC2K_err_rw;
		tunerCfg->tunerData.b2wReg1 = 255;
		tunerCfg->tunerData.b2wRegN = 0;
	}

	return TC2K_ok;
#else
	S32 retVal;

	retVal = _i2c_user_2wb_wr ((U8) tunerCfg->b2wAddress, reg, buf, len);
	if (retVal != 0)
		return TC2K_err_rw;

	return TC2K_ok;
#endif
}

/*
 * _mt_fe_tn_tc2000_i2c_read()
 *
 * @param[in] dut I2C address.
 * @param[in] regaddr address to read from.
 * @param[out] buf data.
 * @param[in] len length of data in bytes.
 *
 * @return  TC2K_ok
 *
 * @note The customer must provide the following I2C read
 * function in order for the TC2000 API software to function correctly..
 */
TC2K_RET _mt_fe_tn_tc2000_i2c_read(tc2000_tn_config *tunerCfg, U8 reg, U8 *buf, U8 len, TC2K_2wbSync sync)
{
#ifdef TC2K_RegImage
	TC2K_RET retVal;
	U8	x;

	if (sync == TC2K_2wbSync_yes) {
		retVal = _i2c_user_2wb_rd((U8) tunerCfg->b2wAddress,0,&tunerCfg->tunerData.regImage[0],18);
		if (retVal != TC2K_ok)
			return TC2K_err_rw;
	}
	for(x=0;x<len;++x)
		buf[x] = tunerCfg->tunerData.regImage[reg + x];

	return TC2K_ok;
#else
	TC2K_RET retVal;

	retVal = _i2c_user_2wb_rd((U8) tunerCfg->b2wAddress, reg, buf, len);
	if (retVal != 0)
		return TC2K_err_rw;

	return TC2K_ok;
#endif
}

/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start IC Configutration Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

/*******************************************************************************
*
* cfg for ifCal:
*  rfAtt in max aten
*  lna in lb
*  tfdeQ on
*  tfAtt at min gain
*  ttSrc trans
*  ttPwr -50 dBV @ mixer
*  ifVga2 on
*  calDetCtrl b2w
*  ifCapWdCtrl b2w
*  ifCapWd 0
*
*******************************************************************************/

/* @CodVal tc2000_cfgIfCalRegs */
U8 tc2000_cfgIfCalRegs[TC2K_cfgRegCnt] = {
	0x03,	0x0E,	0x82,	0x06,	0x06,	0x00,	0x12,	0x99,	0x48,	0x08,
	0x00,	0x00,	0x13,	0x03,	0x43,	0x08,	0x18,	0x29,	0x7A,	0x05,
	0x84,	0x15,	0x1A,	0x1C,	0x00,	0x00,	0x00,	0x08,	0x01,	0x00,
	0x00,	0x0B,	0xFD,	0xD0,	0xC8,	0x12,	0x3A,	0xE3,	0xA7,	0x22,
	0x00,	0x80,	0x00,	0xD9,	0xBF,	0x4D,	0x3F,	0x2C,	0x00,	0xF7,
	0x00,	0xD0,	0x3F,	0xAF,	0x3E,	0x34,	0x00,	0xD0,	0x01,	0xC9,
	0x3F,	0x72,	0x3D,	0x3A,	0x00,	0xD4,	0x03,	0xC4,	0x3F,	0x54,
	0x39,	0x3D,	0x00,	0x78,	0x14,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x08,	0x00,	0x00,	0x00,	0x00,	0x00,	0xA0,	0xDC,	0x98,	0x00,
	0x00,	0x11,	0x11,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x30,	0x80,	0xC0,	0x00,	0x00,	0x20,	0x00,	0x00,
	0x00,	0x45,	0xFA,	0x80,	0xDB,	0xBB,	0xCB,	0xBB,	0x01,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x20,	0x00,	0x00,	0x00,
	0x00,	0x00,	0xFB,	0xF6,	0x00,	0x1C,	0x38,	0xA8,	0xA0,	0xAA,
	0x40,	0x00,	0x20,	0x00,	0x80,	0x00,	0x80,	0x09,	0x02,	0x14,
	0xFF,	0x07,	0x00,	0x30,	0x00,	0x00,	0x00,	0x80,	0x7B,	0x80,
	0x00,	0xF0,	0x3F,	0x0F,	0x01,	0x60,	0x70,	0x09,	0x08
};

/**********************************************************************************************************************
*
* base configuratin, esentialy atsc operation with:
*  rfAtt in max aten
*  dac1 and dac2 off
*
* called at end of mt_fe_tn_tc2000_active(). final cfg done in mt_fe_tn_tc2000_set_chan()
*
**********************************************************************************************************************/

/* @CodVal tc2000_cfgBaseRegs */
U8 tc2000_cfgBaseRegs[TC2K_cfgRegCnt] = {
	0x0B,	0x0E,	0x02,	0x06,	0x06,	0x00,	0x12,	0x99,	0x48,	0x08,
	0x00,	0x00,	0x13,	0x03,	0x43,	0x08,	0x18,	0x29,	0x7A,	0x05,
	0x84,	0x15,	0x1A,	0x1C,	0x00,	0x00,	0x00,	0x08,	0x01,	0x00,
	0x00,	0x0B,	0xFD,	0xD0,	0xC8,	0x12,	0x3A,	0xE3,	0xA7,	0x22,
	0x00,	0x80,	0x00,	0xD9,	0xBF,	0x4D,	0x3F,	0x2C,	0x00,	0xF7,
	0x00,	0xD0,	0x3F,	0xAF,	0x3E,	0x34,	0x00,	0xD0,	0x01,	0xC9,
	0x3F,	0x72,	0x3D,	0x3A,	0x00,	0xD4,	0x03,	0xC4,	0x3F,	0x54,
	0x39,	0x3D,	0x00,	0x78,	0x14,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x08,	0x00,	0x00,	0x00,	0x00,	0x00,	0xA0,	0xDC,	0x98,	0x00,
	0x00,	0x11,	0x11,	0x11,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x30,	0x80,	0xC0,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x05,	0xFA,	0x80,	0xDB,	0xBB,	0xCB,	0xBB,	0x07,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0xFA,	0xC6,	0x00,	0x1C,	0x39,	0xC8,	0xA0,	0xAA,
	0x40,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x09,	0x02,	0x14,
	0x7F,	0x07,	0x00,	0x30,	0x00,	0x00,	0x00,	0x80,	0x7B,	0x80,
	0x00,	0xF0,	0x3F,	0x0F,	0x01,	0x60,	0x70,	0x09,	0x08
};

/**********************************************************************************************************************
*
* power on reset condition.
*
**********************************************************************************************************************/

/* @CodVal tc2000_cfgIdleRegs */
static U8 tc2000_cfgIdleRegs[TC2K_cfgRegCnt] = {
	0x00,	0x00,	0x01,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x01,	0x00,	0x00,	0x00,	0x00,	0x00,	0x0C,	0x00,
	0x00,	0x15,	0x00,	0x00,	0x00,	0x00,	0x00,	0x20,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x01,
	0x80,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00
};

/**********************************************************************************************************************
*
* channel filter registers
*
**********************************************************************************************************************/

static U8 tc2000_chFltr_6MHz[TC2K_cfgChFltrRegCnt] = {
	0x05,	0xFF,	0xE8,	0xE4,	0x09,	0x1D,	0xF1,	0xD3,	0x11,	0x00,
	0x40,	0x00,	0xED,	0xBF,	0xA6,	0x3F,	0x16,	0x00,	0x7B,	0x00,
	0xE8,	0x3F,	0x57,	0x3F,	0x1A,	0x00,	0xE8,	0x00,	0xE4,	0x3F,
	0xB9,	0x3E,	0x1D,	0x00,	0xEA,	0x01,	0xE2,	0x3F,	0xAA,	0x3C,
	0x1E,	0x00,	0x3C,	0x0A
};

static U8 tc2000_chFltr_7MHz[TC2K_cfgChFltrRegCnt] = {
	0x00,	0xFC,	0xEA,	0xEE,	0x11,	0x0B,	0xE2,	0x00,	0x2B,	0x00,
	0xE8,	0x3F,	0xD1,	0xBF,	0x3D,	0x00,	0x20,	0x00,	0x9A,	0x3F,
	0x0E,	0x00,	0x85,	0x00,	0xA1,	0x3F,	0x80,	0x3F,	0xD0,	0x00,
	0x39,	0x00,	0xB0,	0x3E,	0x7B,	0x00,	0xC8,	0x01,	0xF2,	0x3D,
	0xE3,	0x3D,	0x75,	0x08
};

static U8 tc2000_chFltr_8MHz[TC2K_cfgChFltrRegCnt] = {
	0x00,	0x00,	0xF9,	0x0D,	0x22,	0xF3,	0xF2,	0x2A,	0xE8,	0x3F,
	0xDD,	0x3F,	0x4B,	0x40,	0xDF,	0x3F,	0xBA,	0x3F,	0x7D,	0x00,
	0xD7,	0x3F,	0x7F,	0x3F,	0xCB,	0x00,	0xCF,	0x3F,	0x1A,	0x3F,
	0x4F,	0x01,	0xC9,	0x3F,	0x46,	0x3E,	0x7C,	0x02,	0xC5,	0x3F,
	0x2D,	0x3B,	0x1B,	0x0A
};

/********************************************************************************
* Used to interpolate user bbBw with calibrated cap words.
********************************************************************************/
static const S32 tc2000_bbBwFreqRatio[31] = {
	1095, 1085, 1080, 1071, 1068, 1064, 1061, 1055, 1055, 1050,
	1050, 1046, 1042, 1044, 1039, 1037, 1039, 1036, 1034, 1035,
	1032, 1029, 1030, 1031, 1027, 1028, 1029, 1029, 1025, 1021, 1026
};

/****************************************************************************************************************
* rssi data
*
* rssiGainMin = gain at first entry in lut
* rssiLut containes agc accumultor values
* rssiStep = corresponding gain step for each accum value in lut.
*
* gain = gainMin + i * rssiStep, where i = index of closest lut entry to current accum value.
*
* rssiRfAttLut indexed by lnaBand (LB,MB,HB:0,1,2), rfAttCfg (2P,PF,2PF:0,1,2).
* Base index passed to interpolation routine = lnaBand*57+rfAttCfg*19
*	S16 tc2000_rssiRfAttLut[171] = {
*		LB-2P column,
*		LB-PF column,
*		LB-2PF column,
*		MB-2P column,
*		MB-PF column,
*		MB-2PF column,
*		HB-2P column,
*		HB-PF column,
*		HB-2PF column,
*	};
*
* rssiRfAttAcc0 and rssiRfAttStep indexed by lnaBand (LB,MB,HB:0,1,2), rfAttCfg (2P,PF,2PF:0,1,2).
* Base index passed to interpolation routine = lnaBand*3+rfAttCfg
*  S16 tc2000_rssiRfAttAcc0[9] = {
*	  LB-2P, LB-PF, LB-2PF,
*	  MB-2P, MB-PF, MB-2PF,
*	  HB-2P, HB-PF, HB-2PF,
*  };
*
* rssiDigGainLut base index is always 0
*
****************************************************************************************************************/

static S16 tc2000_rssiRfAttLut[171] = {
	0x0016,	0x0339,	0x03C1,	0x043C,	0x04AA,	0x0510,	0x0570,	0x05CA,	0x0620,	0x0674,	0x06C9,	0x071F,	0x077A,
	0x07DB,	0x0849,	0x08CD,	0x097A,	0x0A95,	0x0FFF,
	0x000E,	0x02DC,	0x03F8,	0x04AE,	0x053A,	0x05BF,	0x0659,	0x06F9,	0x0785,	0x0800,	0x0873,	0x08EA,	0x0988,
	0x0A44,	0x0AEC,	0x0B8A,	0x0C6C,	0x0D86,	0x0FFF,
	0x0016,	0x035F,	0x0435,	0x04CC,	0x054F,	0x05CD,	0x064E,	0x06D4,	0x0758,	0x07D5,	0x084D,	0x08C2,	0x094E,
	0x0A05,	0x0ABF,	0x0B68,	0x0C42,	0x0D77,	0x0FFF,
	0x003A,	0x033B,	0x03C6,	0x0441,	0x04B1,	0x0517,	0x0577,	0x05D0,	0x0625,	0x067A,	0x06D0,	0x0727,	0x0783,
	0x07E8,	0x0859,	0x08E2,	0x0997,	0x0ABD,	0x0FFF,
	0x001A,	0x02DF,	0x03F6,	0x04A7,	0x0530,	0x05B2,	0x0646,	0x06E2,	0x076F,	0x07E9,	0x0859,	0x08CD,	0x095E,
	0x0A18,	0x0AC7,	0x0B6A,	0x0C40,	0x0D71,	0x0FFF,
	0x000A,	0x034A,	0x040F,	0x0495,	0x050C,	0x057B,	0x05ED,	0x0665,	0x06E3,	0x0763,	0x07E0,	0x085A,	0x08D9,
	0x0979,	0x0A43,	0x0B0B,	0x0BDD,	0x0D39,	0x0FFF,
	0x0019,	0x034D,	0x03E9,	0x046B,	0x04DC,	0x0541,	0x059D,	0x05F5,	0x064B,	0x06A0,	0x06F5,	0x074E,	0x07AC,
	0x0813,	0x0887,	0x0914,	0x09CE,	0x0B01,	0x0FFF,
	0x000A,	0x02BB,	0x03D0,	0x0483,	0x050F,	0x058E,	0x061B,	0x06BB,	0x0750,	0x07CF,	0x0843,	0x08BC,	0x094E,
	0x0A09,	0x0ABD,	0x0B64,	0x0C3E,	0x0D70,	0x0FFF,
	0x0000,	0x035B,	0x041A,	0x0497,	0x0504,	0x0569,	0x05CD,	0x0634,	0x069F,	0x070D,	0x077F,	0x07F4,	0x086C,
	0x08F0,	0x09A4,	0x0A8D,	0x0B74,	0x0CDF,	0x0FFF
};

static const S16 tc2000_rssiRfAttAcc0[9] = {
	-251,	-305,	-347,
	-235,	-292,	-372,
	-202,	-271,	-422,
};
static const S16 tc2000_rssiRfAttStep[9] = {
	14,	17,	19,
	13,	16,	21,
	11,	15,	23
};

static S16 tc2000_rssiDigGainLut[TC2K_rssiDigGainLen] = {
	0x0006,	0x0008,	0x000A,	0x000C,	0x0010,	0x0014,	0x0019,	0x0020,	0x0028,	0x0033,
	0x0040,	0x0051,	0x0066,	0x0080,	0x00A1,	0x00CB,	0x0100,	0x0143,	0x0196,	0x0200,
	0x0284,	0x032B,	0x03FD,	0x0506,	0x0653,	0x07F6,	0x0A06,	0x0C9E,	0x0FE2,	0x1400,
	0x192D,	0x1FB2,	0x27E7,	0x323C,	0x3F3E,	0x4F9F,	0x643C,	0x7E31,
};

/****************************************************************************************************************
*
* bbGain = (bbAcc - bbAccMin) * bbSlope + bbGmin
* if bbGain < 0 then bbGain = 0
* if bbGain > bbGainMax then bbgain = bbGainMax
*
* Absolute value of gain not important. Gain in lna modes is determined empiricaly and compensates.
*
* Array index = 0 for vga2 off, 1 for vga2 on
*
****************************************************************************************************************/
static const S16 tc2000_rssiBbAccMin[2] = {0x0300, 0x0480};
static const S16 tc2000_rssiBbSlope[2] = {110, 220};
static const S16 tc2000_rssiBbGmin[2] = {0, -70};
static const S16 tc2000_rssiBbGmax[2] = {300, 430};

/****************************************************************************************************************
* indexed by TC2K_modulation. 10ths of a dB
* DVB_C, J83_Annex_B, DVB_T, ATSC, ISDB_T, DMB_T, PAL_B, PAL_D_I_K, PAL_G_H, NTSC, SECAM, SECAM_L
****************************************************************************************************************/
static const S16 tc2000_rssiPar[12] = {60,60,90,70,90,90,0,0,0,0,0,0};
//static const S16 tc2000_rssiPar[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

/**********************************************
* indexed with (TC2K_lnaModeStatus + lnaBand)
*	if (lnaBand == TC2K_tfBand_LB)
*		tunerCfg->RSSI += rssiCorrFactor[tunerCfg->lnaModeStatus];
*	else if (lnaBand == TC2K_tfBand_MB)
*		tunerCfg->RSSI += rssiCorrFactor[tunerCfg->lnaModeStatus + 3];
*	else
*		tunerCfg->RSSI += rssiCorrFactor[tunerCfg->lnaModeStatus + 6];
*
* tc2000_rssiCF[]
*   LB-CATV, LB-OTA, LB-LN, LB-CATV
*   MB-CATV, MB-OTA, MB-LN, MB-CATV
*   HB-CATV, HB-OTA, HB-LN, HB-CATV
*
* Value set empiricaly at Pin = -50 dBm
**********************************************/
static const S16 tc2000_rssiCF[12] = {
	-287,	-315,	-366,	-287,
	-270,	-299,	-345,	-270,
	-245,	-270,	-294,	-245
};

static const S16 tc2000_rssiTmprComp[2] = {12,8};

/****************************************************************************************************************
* end of rssi data
****************************************************************************************************************/

TC2K_RET _mt_fe_tn_tc2000_cfgIcRev (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	tunerCfg->tunerData.dieRev = 0xFF;
	tunerCfg->tunerData.dieCfg = 0xFF;

	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_dieRev_reg, TC2K_dieRev_mask, &tunerCfg->tunerData.dieRev , TC2K_2wbSync_yes);
	retVal += _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_dieCfg_reg, TC2K_dieCfg_mask, &tunerCfg->tunerData.dieCfg , TC2K_2wbSync_no);
	tunerCfg->tunerData.dieCfg >>= 5;

	if (retVal == TC2K_ok)
		return TC2K_ok;
	else
		return TC2K_err_rw;

}

/**
 * _mt_fe_tn_tc2000_cfgIc()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success.
 *
 * @note Called by mt_fe_tn_tc2000_set_chan().
 */

TC2K_RET _mt_fe_tn_tc2000_cfgIc (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgIc() 2wb optimization. Requires a w sync after calling. */

	/* @doc Always program registers with multiple dependancies first. */
	retVal = _mt_fe_tn_tc2000_cfgMultDepen (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgBcMedium (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgMod (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->specInvSel != TC2K_specInvSel_on)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_specInv_reg, TC2K_specInv_mask, TC2K_specInv_on, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_specInv_reg, TC2K_specInv_mask, TC2K_specInv_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgBw (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->ifOutputSel == TC2K_ifOutputSel_1) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifOut2_reg, TC2K_ifOut2_mask, TC2K_ifOut2_off, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifOut1_reg, TC2K_ifOut1_mask, TC2K_ifOut1_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifOut1_reg, TC2K_ifOut1_mask, TC2K_ifOut1_off, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifOut2_reg, TC2K_ifOut2_mask, TC2K_ifOut2_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}

	if (tunerCfg->agcCtrlMode == TC2K_agcCtrlMode_internal)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcSrc_reg, TC2K_ifAgcSrc_mask, TC2K_ifAgcSrc_useB2wFnlAgc, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcSrc_reg, TC2K_ifAgcSrc_mask, TC2K_ifAgcSrc_useDemodFnlAgc, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgMultDepen(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc Execute before any single dependancy cfg subroutines before tunerData entries are updated */
	/* @doc or function may fail. Does not upddate tunerData entries. */

	/* @doc Configure misc, Rule 2: if digital - else if analog catv - else analog ota */
	if ((tunerCfg->modulation == TC2K_modulation_DVB_C) ||
		(tunerCfg->modulation == TC2K_modulation_J83_Annex_B) ||
		(tunerCfg->modulation == TC2K_modulation_DVB_T) ||
		(tunerCfg->modulation == TC2K_modulation_ATSC) ||
		(tunerCfg->modulation == TC2K_modulation_ISDB_T) ||
		(tunerCfg->modulation == TC2K_modulation_DMB_T)) {
		/* @doc If any digital modulation, set register to the following. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digDCbwFst_reg, TC2K_digDCbwFst_mask, TC2K_digDCbwFst_BW14p6Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQPhBwFst_reg, TC2K_IQPhBwFst_mask, TC2K_IQPhBwFst_BW1p8Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQGainBwFst_reg, TC2K_IQGainBwFst_mask, TC2K_IQGainBwFst_BW3p6Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMaxCnt_reg, TC2K_bbHysMaxCnt_mask, TC2K_bbHysMaxCnt_time10p9msecs, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable) {
		/* @doc Analog CATV set registers to the following. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digDCbwFst_reg, TC2K_digDCbwFst_mask, TC2K_digDCbwFst_BW29Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQPhBwFst_reg, TC2K_IQPhBwFst_mask, TC2K_IQPhBwFst_BW1p8Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQGainBwFst_reg, TC2K_IQGainBwFst_mask, TC2K_IQGainBwFst_BW1p8Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMaxCnt_reg, TC2K_bbHysMaxCnt_mask, TC2K_bbHysMaxCnt_time10p9msecs, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else {
		/* @doc Analog OTA set registers to the following. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digDCbwFst_reg, TC2K_digDCbwFst_mask, TC2K_digDCbwFst_BW117Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQPhBwFst_reg, TC2K_IQPhBwFst_mask, TC2K_IQPhBwFst_BW58Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IQGainBwFst_reg, TC2K_IQGainBwFst_mask, TC2K_IQGainBwFst_BW58Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMaxCnt_reg, TC2K_bbHysMaxCnt_mask, TC2K_bbHysMaxCnt_time87p4msecs, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}

	/* @doc Configure rfDet, Rule 3: if analog - else if catv - else if atsc - else digital ota*/

	if (tunerCfg->modulation == TC2K_modulation_PAL_B ||
		tunerCfg->modulation == TC2K_modulation_PAL_D_I_K ||
		tunerCfg->modulation == TC2K_modulation_PAL_G_H ||
		tunerCfg->modulation == TC2K_modulation_NTSC ||
		tunerCfg->modulation == TC2K_modulation_SECAM ||
		tunerCfg->modulation == TC2K_modulation_SECAM_L)
	{
		/* @doc Any Analog. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetLvlCtrl_reg, TC2K_rfDetLvlCtrl_mask, TC2K_rfDetLvlCtrl_rangeM26M23, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetSetPt_reg, TC2K_rfDetSetPt_mask, TC2K_rfDetSetPt_Vdet_0p2, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable)
	{
		/* @doc Digital CATV */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetLvlCtrl_reg, TC2K_rfDetLvlCtrl_mask, TC2K_rfDetLvlCtrl_rangeM26M23, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetSetPt_reg, TC2K_rfDetSetPt_mask, TC2K_rfDetSetPt_Vdet_0p24, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else if (tunerCfg->modulation == TC2K_modulation_ATSC)
	{
		/* @doc ATSC OTA. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetLvlCtrl_reg, TC2K_rfDetLvlCtrl_mask, TC2K_rfDetLvlCtrl_rangeM26M23, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetSetPt_reg, TC2K_rfDetSetPt_mask, TC2K_rfDetSetPt_Vdet_0p3, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else if (tunerCfg->modulation == TC2K_modulation_DVB_C ||
			 tunerCfg->modulation == TC2K_modulation_J83_Annex_B)
	{
		/* @doc DVB-C or J.83 OTA.  */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetLvlCtrl_reg, TC2K_rfDetLvlCtrl_mask, TC2K_rfDetLvlCtrl_rangeM26M23, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetSetPt_reg, TC2K_rfDetSetPt_mask, TC2K_rfDetSetPt_Vdet_0p26, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else
	{
		/* @doc Digital OTA, Not ATSC. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetLvlCtrl_reg, TC2K_rfDetLvlCtrl_mask, TC2K_rfDetLvlCtrl_rangeM22M20, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetSetPt_reg, TC2K_rfDetSetPt_mask, TC2K_rfDetSetPt_Vdet_0p34, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}

	if (tunerCfg->modulation == TC2K_modulation_SECAM_L)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, TC2K_rfAgcLoopBw_BW20Hz, TC2K_2wbSync_no);
	else if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, TC2K_rfAgcLoopBw_BW40Hz, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, TC2K_rfAgcLoopBw_BW10Hz, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->modulation != TC2K_modulation_SECAM_L)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, TC2K_agcWindow_rfWndwSz_85p33uS, TC2K_2wbSync_no);
	else if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, TC2K_agcWindow_rfWndwSz_682p67uS, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, TC2K_agcWindow_rfWndwSz_341p33uS, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, TC2K_ifAgcLoopBw_BW_2p4Hz, TC2K_2wbSync_no);
	else if (tunerCfg->modulation == TC2K_modulation_SECAM_L)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, TC2K_ifAgcLoopBw_BW_4p8Hz, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, TC2K_ifAgcLoopBw_BW_0p6Hz, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable) {
		if (tunerCfg->modulation == TC2K_modulation_SECAM_L)
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbAdcBckOff_reg, TC2K_bbAdcBckOff_mask, TC2K_bbAdcBckOff_BkOffFrmFS_5dB, TC2K_2wbSync_no);
		else
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbAdcBckOff_reg, TC2K_bbAdcBckOff_mask, TC2K_bbAdcBckOff_BkOffFrmFS_3dB, TC2K_2wbSync_no);
	}
	else if (tunerCfg->modulation == TC2K_modulation_PAL_B ||
			 tunerCfg->modulation == TC2K_modulation_PAL_D_I_K ||
			 tunerCfg->modulation == TC2K_modulation_PAL_G_H ||
			 tunerCfg->modulation == TC2K_modulation_NTSC ||
			 tunerCfg->modulation == TC2K_modulation_SECAM)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbAdcBckOff_reg, TC2K_bbAdcBckOff_mask, TC2K_bbAdcBckOff_BkOffFrmFS_5dB, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbAdcBckOff_reg, TC2K_bbAdcBckOff_mask, TC2K_bbAdcBckOff_BkOffFrmFS_6dB, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota ||
		tunerCfg->modulation == TC2K_modulation_SECAM_L)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcBckOff_reg, TC2K_digAgcBckOff_mask, TC2K_digAgcBckOff_neg6dB, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcBckOff_reg, TC2K_digAgcBckOff_mask, TC2K_digAgcBckOff_neg2p5dB, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->modulation == TC2K_modulation_SECAM_L)
		if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable)
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, TC2K_ifAgcShift_bbagcshft_7, TC2K_2wbSync_no);
		else
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, TC2K_ifAgcShift_bbagcshft_6, TC2K_2wbSync_no);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, TC2K_ifAgcShift_bbagcshft_4, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgBcMedium()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called _mt_fe_tn_tc2000_cfgIc only. adjust settings for terrestrial or cable operation.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgBcMedium (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgBcMedium() 2wb optimization. Requires a w sync after calling. */
	if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota) {
		/* @doc If in OTA mode. These registes depend on the broadcase medium only. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysADCMax_reg, TC2K_dcHysADCMax_mask, TC2K_dcHysADCMax_ADCMx1408, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysDelta_reg, TC2K_bbHysDelta_mask, TC2K_bbHysDelta_window64, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_LnaBuffBiasAdj2_reg, TC2K_LnaBuffBiasAdj2_mask, TC2K_LnaBuffBiasAdj2_pos25percent, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcILoopBw_reg, TC2K_dcILoopBw_mask, TC2K_dcILoopBw_BW100Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcQLoopBw_reg, TC2K_dcQLoopBw_mask, TC2K_dcQLoopBw_BW100Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lnaMode_reg, TC2K_lnaMode_mask, TC2K_lnaMode_terLoNoise, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_ifSetAccTop(tunerCfg, TC2K_ifAccTopIndex_Vga2_on_26dB);
		if (retVal != TC2K_ok)
			return retVal;
		tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
	}
	else {
		/* @doc Else in CATV mode. These registes depend on the broadcase medium only. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysADCMax_reg, TC2K_dcHysADCMax_mask, TC2K_dcHysADCMax_ADCMx1792, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_LnaBuffBiasAdj2_reg, TC2K_LnaBuffBiasAdj2_mask, TC2K_LnaBuffBiasAdj2_Nominal, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysDelta_reg, TC2K_bbHysDelta_mask, TC2K_bbHysDelta_window16, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcILoopBw_reg, TC2K_dcILoopBw_mask, TC2K_dcILoopBw_BW25Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcQLoopBw_reg, TC2K_dcQLoopBw_mask, TC2K_dcQLoopBw_BW25Hz, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lnaMode_reg, TC2K_lnaMode_mask, TC2K_lnaMode_cab, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_off, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_ifSetAccTop(tunerCfg, TC2K_ifAccTopIndex_Vga2_off_12dB);
		if (retVal != TC2K_ok)
			return retVal;
		tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
	}

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgMod()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called _mt_fe_tn_tc2000_cfgIc only. cfg's ic for digital or analog modulation.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgMod (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgMod() 2wb optimization. Requires a w sync after calling. */

	switch (tunerCfg->modulation) {
		case TC2K_modulation_DVB_C:
		case TC2K_modulation_J83_Annex_B:
		case TC2K_modulation_DVB_T:
		case TC2K_modulation_ATSC:
		case TC2K_modulation_ISDB_T:
		case TC2K_modulation_DMB_T:
			/* @doc If any digital modulation, set register to the following. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcInSel_reg, TC2K_digAgcInSel_mask, TC2K_digAgcInSel_Translator_Out, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, TC2K_dagcWdwSzVal_WndwSz_85p33us, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		case TC2K_modulation_PAL_B:
		case TC2K_modulation_PAL_D_I_K:
		case TC2K_modulation_PAL_G_H:
		case TC2K_modulation_NTSC:
		case TC2K_modulation_SECAM:
			/* @doc Else modulation is analog, set registers to the following. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcInSel_reg, TC2K_digAgcInSel_mask, TC2K_digAgcInSel_Interp_Out, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, TC2K_dagcWdwSzVal_WndwSz_85p33us, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		case TC2K_modulation_SECAM_L:
			/* @doc Else modulation is analog, set registers to the following. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcInSel_reg, TC2K_digAgcInSel_mask, TC2K_digAgcInSel_Interp_Out, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, TC2K_dagcWdwSzVal_WndwSz_682p67us, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		default:
			return TC2K_err_unknown; /* should never happen */
	}

	if (tunerCfg->modulation == TC2K_modulation_ATSC) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_mixLoBufBiasCurSel_reg, TC2K_mixLoBufBiasCurSel_mask, TC2K_mixLoBufBiasCurSel_mA3p0, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_mixLoBufBiasCurSel_reg, TC2K_mixLoBufBiasCurSel_mask, TC2K_mixLoBufBiasCurSel_mA2p0, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgBw()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called _mt_fe_tn_tc2000_cfgIc only. sets analog if bw and channel filter.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgBw (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8	*fir;
	U8	cw;

	/* @doc _mt_fe_tn_tc2000_cfgBw() 2wb optimization. Requires a w sync after calling. */

	if ((tunerCfg->modulation == TC2K_modulation_J83_Annex_B) && (tunerCfg->ifBw == TC2K_ifBw_6MHz)) {
		/* @doc If J83 Annex B and 6 MHz BW then dig/analog BW = 6/8 MHz. */
		fir = tc2000_chFltr_6MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifCalBw_8MHz];
		tunerCfg->tunerData.ifBwHz = 6000000;
	}
	else if ((tunerCfg->modulation == TC2K_modulation_DVB_C) && (tunerCfg->ifBw == TC2K_ifBw_7MHz)) {
		/* @doc Else if DVB-C and 7MHz BW then dig/analog BW = 7/8 MHz. */
		fir = tc2000_chFltr_7MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifCalBw_8MHz];
		tunerCfg->tunerData.ifBwHz = 7000000;
	}
	else if ((tunerCfg->modulation == TC2K_modulation_DVB_C) && (tunerCfg->ifBw == TC2K_ifBw_8MHz)) {
		/* @doc Else if DVB-C and 8MHz BW then dig/analog BW = 8/10 MHz. */
		fir = tc2000_chFltr_8MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifCalBw_10MHz];
		tunerCfg->tunerData.ifBwHz = 8000000;
	}
	else if
		(tunerCfg->ifBw == TC2K_ifBw_8MHz &&
		(tunerCfg->modulation == TC2K_modulation_PAL_G_H || tunerCfg->modulation == TC2K_modulation_PAL_B))
	{
		/* @doc Else if PAL_GH and 8MHz BW then dig/analog BW = 7 MHz. */
		fir = tc2000_chFltr_7MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifCalBw_7MHz];
		tunerCfg->tunerData.ifBwHz = 7000000;
	}
	else if (tunerCfg->ifBw == TC2K_ifBw_6MHz) {
		/* @doc Else if 6MHz BW then both = 6MHz. */
		fir = tc2000_chFltr_6MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifBw_6MHz];
		tunerCfg->tunerData.ifBwHz = 6000000;
	}
	else if (tunerCfg->ifBw == TC2K_ifBw_7MHz) {
		/* @doc Else if 7MHz BW then both = 7MHz. */
		fir = tc2000_chFltr_7MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifBw_7MHz];
		tunerCfg->tunerData.ifBwHz = 7000000;
	}
	else if (tunerCfg->ifBw == TC2K_ifBw_8MHz) {
		/* @doc Else if 8MHz BW then both = 8MHz. */
		fir = tc2000_chFltr_8MHz;
		cw = tunerCfg->tunerData.ifBwWd[TC2K_ifBw_8MHz];
		tunerCfg->tunerData.ifBwHz = 8000000;
	}
	else if (tunerCfg->ifBw == TC2K_ifBw_User) {
		/* @doc Else User defined BW */
		fir = tunerCfg->ifBwUserFIR;
		cw = _mt_fe_tn_tc2000_cfgIfBwUserCwInterp(tunerCfg);
		tunerCfg->tunerData.ifBwHz = tunerCfg->ifBwUserHz;
	}
	else
		return TC2K_err_invalid_parameter; /* should never happen */

	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_cfgChFltrReg1, fir, TC2K_cfgChFltrRegCnt, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCapWdWr_reg, TC2K_ifCapWdWr_mask, cw, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgBase()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called by mt_fe_tn_tc2000_active() at end of subroutine.
 * 		Base configuration for normal opperation. Set
 * 		after calibration in mt_fe_tn_tc2000_active(). Configuration
 * 		is completed in mt_fe_tn_tc2000_set_chan (supports multi
 * 		mode operation).
 */
TC2K_RET _mt_fe_tn_tc2000_cfgBase (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgBase() 2wb optimization. No external r/w sync required before calling. */
	/* @doc Load base configuration. */
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_cfgWrReg1, tc2000_cfgBaseRegs, TC2K_cfgRegCnt, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Rewrite the driver revision to the ic for debug. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMajor_reg, TC2K_swRevMajor_mask, TC2K_swRevMajor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMinor_reg, TC2K_swRevMinor_mask, TC2K_swRevMinor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevBuild_reg, TC2K_swRevBuild_mask, TC2K_swRevBuild, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc If set ltaEna mode. */
	if (tunerCfg->ltaEnaSel == TC2K_ltaEnaSel_on) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ltaEna_reg, TC2K_ltaEna_mask, TC2K_ltaEna_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	/* @doc If dual chip mode. */
	if (tunerCfg->tunerCnt == TC2K_tunerCnt_dual) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dualChipSel_reg, TC2K_dualChipSel_mask, TC2K_dualChipSel_dual, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}
	retVal = _mt_fe_tn_tc2000_cfgRfAtten(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc If rfAttBias <> 1.8V (default) then set to 3.3V */
	if (tunerCfg->rfAttBias == TC2K_rfAttBias_3p3v) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAttBias_reg, TC2K_rfAttBias_mask, TC2K_rfAttBias_Volt3p3, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
	}
	/* @doc Reset IC. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_reset_reg, TC2K_reset_mask, TC2K_reset_Reset, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_reset_reg, TC2K_reset_mask, TC2K_reset_NotReset, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_cfgResolveBbErrCondition(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgRfAtten()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called _mt_fe_tn_tc2000_cfgIc only.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgRfAtten (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgRfAtten() 2wb optimization. Requires a w sync after calling. */

	switch (tunerCfg->rfAttMode) {
		case TC2K_rfAttMode_seriesPin_shuntPin:
			/* @doc Case 2-PIN RF attenuator mode. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinCtrl_reg, TC2K_sePinCtrl_mask, TC2K_sePinCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinCtrl_reg, TC2K_shPinCtrl_mask, TC2K_shPinCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttCtrl_reg, TC2K_iAttCtrl_mask, TC2K_iAttCtrl_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinDacCtrl_reg, TC2K_sePinDacCtrl_mask, TC2K_sePinDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinDacCtrl_reg, TC2K_shPinDacCtrl_mask, TC2K_shPinDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttDacCtrl_reg, TC2K_iAttDacCtrl_mask, TC2K_iAttDacCtrl_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitLo_reg, TC2K_shPinLimitLo_mask, TC2K_cfgShPinLimitLo_Cablel, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitHi_reg, TC2K_shPinLimitHi_mask, TC2K_cfgShPinLimitHi_Cablel, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		case TC2K_rfAttMode_seriesPin_internalAtt:
			/* @doc Case PIN-iAtten RF attenuator mode. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinCtrl_reg, TC2K_sePinCtrl_mask, TC2K_sePinCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinCtrl_reg, TC2K_shPinCtrl_mask, TC2K_shPinCtrl_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttCtrl_reg, TC2K_iAttCtrl_mask, TC2K_iAttCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinDacCtrl_reg, TC2K_sePinDacCtrl_mask, TC2K_sePinDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinDacCtrl_reg, TC2K_shPinDacCtrl_mask, TC2K_shPinDacCtrl_off, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttDacCtrl_reg, TC2K_iAttDacCtrl_mask, TC2K_iAttDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		case TC2K_rfAttMode_seriesPin_shuntPin_internalAtt:
			/* @doc Case 2-PIN-iAtten RF attenuator mode. */
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinCtrl_reg, TC2K_sePinCtrl_mask, TC2K_sePinCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinCtrl_reg, TC2K_shPinCtrl_mask, TC2K_shPinCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttCtrl_reg, TC2K_iAttCtrl_mask, TC2K_iAttCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_sePinDacCtrl_reg, TC2K_sePinDacCtrl_mask, TC2K_sePinDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinDacCtrl_reg, TC2K_shPinDacCtrl_mask, TC2K_shPinDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_iAttDacCtrl_reg, TC2K_iAttDacCtrl_mask, TC2K_iAttDacCtrl_on, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitLo_reg, TC2K_shPinLimitLo_mask, TC2K_cfgShPinLimitLo_Ter, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitHi_reg, TC2K_shPinLimitHi_mask, TC2K_cfgShPinLimitHi_Ter, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			break;
		default:
			return TC2K_err_invalid_parameter;
	}

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgResolveBbErrCondition()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgResolveBbErrCondition(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8 ifVga2Ena;

	/* @doc _mt_fe_tn_tc2000_cfgResolveBbErrCondition() 2wb optimization. No external r/w sync required before calling. */

	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, &ifVga2Ena, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (ifVga2Ena == TC2K_ifVga2Ena_on) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_off, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		_mt_sleep(2);
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_on, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_on, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		_mt_sleep(2);
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_off, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
	}

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgIfDac()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note
 */
TC2K_RET _mt_fe_tn_tc2000_cfgIfDac (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	S32	FalphaHz;
	S32	FbetaHz;
	U8	IfDacNrzEna;
	U8	dacIrefDivSel;
	U8	reg186b1w7;
	S32 f;
	S32	n;

	/* @doc _mt_fe_tn_tc2000_cfgIfDac() 2wb optimization. Requires a w sync after calling. */
	/* @doc Configure IF DAC for spurious avoidance (NRZ normal/RZ avoidance). */

	/* Nrz enumeration is incorrect. off -> NRZ */
	IfDacNrzEna = TC2K_IfDacNrzEna_off;
	dacIrefDivSel = TC2K_ifLevel_FSRngRed_neg1;
	reg186b1w7 = TC2K_reg186b1w7_nrz;

	if ((tunerCfg->ifDacMode_IF1 == TC2K_ifDacMode_Optimize && tunerCfg->ifOutputSel == TC2K_ifOutputSel_1) ||
		(tunerCfg->ifDacMode_IF2 == TC2K_ifDacMode_Optimize && tunerCfg->ifOutputSel == TC2K_ifOutputSel_2))
	{
		FalphaHz = 2*(tunerCfg->tunerData.FifHz + tunerCfg->tunerData.ifBwHz);

		if (tunerCfg->tunerData.ifBwHz >= tunerCfg->tunerData.FifHz)
			FbetaHz = 0;
		else
			FbetaHz = 2*(tunerCfg->tunerData.FifHz - tunerCfg->tunerData.ifBwHz);

		for (n=1;n<6;++n) {
			f = n * 192000000;
			if (tunerCfg->tunerData.FchanHz < f - FalphaHz)
				break;
			if (tunerCfg->tunerData.FchanHz < f + FalphaHz) {
				if ((tunerCfg->tunerData.FchanHz < f - FbetaHz) || (tunerCfg->tunerData.FchanHz > f + FbetaHz)) {
					IfDacNrzEna = TC2K_IfDacNrzEna_on;
					dacIrefDivSel = TC2K_ifLevel_FSRngRed_pos4;
					reg186b1w7 = TC2K_reg186b1w7_rz;
				}
				break;
			}
		}
	}

	/* @doc Always set the IF output level if not in auto mode. */
	if ((tunerCfg->ifOutputSel == TC2K_ifOutputSel_1 && tunerCfg->ifOutPutLevel_IF1 != TC2K_ifOutputLevel_Auto) ||
		(tunerCfg->ifOutputSel == TC2K_ifOutputSel_2 && tunerCfg->ifOutPutLevel_IF2 != TC2K_ifOutputLevel_Auto)) {
		retVal = _mt_fe_tn_tc2000_cfgIfOutputLevel(tunerCfg);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifLevel_reg, TC2K_ifLevel_mask, dacIrefDivSel, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_IfDacNrzEna_reg, TC2K_IfDacNrzEna_mask, IfDacNrzEna, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_reg186b1w7_reg, TC2K_reg186b1w7_mask, reg186b1w7, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgIfOutputLevel()
 *
 * @param[in,out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_unknown for bad param
 * 			TC2K_err_rw otherwise.
 *
 * @note Called _mt_fe_tn_tc2000_cfgIc only.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgIfOutputLevel (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8 userIfOutputLevel;
	U8 ifOuputLvl;

	/* @doc _mt_fe_tn_tc2000_cfgIfOutputLevel() 2wb optimization. Requires a w sync after calling. */

	if (tunerCfg->ifOutputSel == TC2K_ifOutputSel_1)
		userIfOutputLevel = (U8) tunerCfg->ifOutPutLevel_IF1;
	else
		userIfOutputLevel = (U8) tunerCfg->ifOutPutLevel_IF2;

	switch (userIfOutputLevel) {
		case TC2K_ifOutputLevel_neg3p2_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg3p2;
			break;
		case TC2K_ifOutputLevel_neg2p8_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg2p8;
			break;
		case TC2K_ifOutputLevel_neg2p3_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg2p3;
			break;
		case TC2K_ifOutputLevel_neg2p0_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg2;
			break;
		case TC2K_ifOutputLevel_neg1p6_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg1p6;
			break;
		case TC2K_ifOutputLevel_neg1p0_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg1;
			break;
		case TC2K_ifOutputLevel_neg0p5_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_neg0p5;
			break;
		case TC2K_ifOutputLevel_0_dB_nominal:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_Nom;
			break;
		case TC2K_ifOutputLevel_0p6_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos0p6;
			break;
		case TC2K_ifOutputLevel_1p2_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos1p2;
			break;
		case TC2K_ifOutputLevel_1p8_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos1p8;
			break;
		case TC2K_ifOutputLevel_2p5_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos2p5;
			break;
		case TC2K_ifOutputLevel_3p3_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos3p3;
			break;
		case TC2K_ifOutputLevel_4p0_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos4;
			break;
		case TC2K_ifOutputLevel_5p0_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos5;
			break;
		case TC2K_ifOutputLevel_6p0_dB:
			ifOuputLvl = TC2K_ifLevel_FSRngRed_pos6;
			break;
		default:
			return TC2K_err_unknown;
	}

	/* @doc set IF output level. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifLevel_reg, TC2K_ifLevel_mask, ifOuputLvl, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

U8 _mt_fe_tn_tc2000_cfgIfBwUserCwInterp (tc2000_tn_config *tunerCfg)
{
	U8	cw;
	U8	cwChk = 0;
	S32 finkHz;
	S32 fkHz;
	S32 fkHzErr;
	S32 fkHzErrChk = 0;

	if (tunerCfg->ifBwUserHz < 1000000)
		return 31;
	else if (tunerCfg->ifBwUserHz > 10000000)
		return 0;

	fkHz = (S32) 7000;
	cw = tunerCfg->tunerData.ifBwWd[TC2K_ifBw_7MHz];
	finkHz = tunerCfg->ifBwUserHz / 1000;

	if (finkHz == fkHz)
		return cw;
	else if (finkHz > fkHz) {
		while ((finkHz > fkHz) & (cw > 0)) {
			cwChk = cw;
			fkHzErrChk = finkHz - fkHz;
			--cw;
			fkHz *= tc2000_bbBwFreqRatio[cw];
			fkHz /= 1000;
		}
		fkHzErr = fkHz - finkHz;
	}
	else {
		while ((finkHz < fkHz) & (cw < 31)) {
			cwChk = cw;
			fkHzErrChk = fkHz - finkHz;
			fkHz *= 1000;
			fkHz /= tc2000_bbBwFreqRatio[cw];
			++cw;
		}
		fkHzErr = finkHz - fkHz;
	}

	if (fkHzErrChk < fkHzErr)
		return cwChk;
	else
		return cw;

}

/**
 * _mt_fe_tn_tc2000_cfgIdle()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Called by mt_fe_tn_tc2000_idle().
 */
TC2K_RET _mt_fe_tn_tc2000_cfgIdle (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc Load idle configuration. */
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_cfgWrReg1, tc2000_cfgIdleRegs, TC2K_cfgRegCnt, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Rewrite the driver revision to the ic for debug. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMajor_reg, TC2K_swRevMajor_mask, TC2K_swRevMajor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMinor_reg, TC2K_swRevMinor_mask, TC2K_swRevMinor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevBuild_reg, TC2K_swRevBuild_mask, TC2K_swRevBuild, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;


	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgIfBwCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note rf atten max atten
 * 		lna off
 * 		tfBandSel = ttGen
 * 		ttPwr = -44dBV
 * 		ttSrc = trans
 * 		ifVGA2 on
 * 		dac off
 */
TC2K_RET _mt_fe_tn_tc2000_cfgIfBwCal (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgIfBwCal() 2wb optimization. No external r/w sync required before calling. */

	/* @doc Load IF calibration configuration. */
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_cfgWrReg1, tc2000_cfgIfCalRegs, TC2K_cfgRegCnt, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Rewrite the driver revision to the ic for debug. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMajor_reg, TC2K_swRevMajor_mask, TC2K_swRevMajor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevMinor_reg, TC2K_swRevMinor_mask, TC2K_swRevMinor, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_swRevBuild_reg, TC2K_swRevBuild_mask, TC2K_swRevBuild, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Save original pll settings so they can be restored every time entr_set_channel() is called.*/
	/* @doc Must always be done before the first call to entr_freqSetFloHz() */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_vcoIsel_reg, TC2K_vcoIsel_mask, &tunerCfg->tunerData.vcoIselRestore , TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synCPcur_reg, TC2K_synCPcur_mask, &tunerCfg->tunerData.synCPcurRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set rfAttBias. */
	if (tunerCfg->rfAttBias == TC2K_rfAttBias_1p8v)
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAttBias_reg, TC2K_rfAttBias_mask, TC2K_rfAttBias_Volt1p8, TC2K_2wbSync_yes);
	else
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAttBias_reg, TC2K_rfAttBias_mask, TC2K_rfAttBias_Volt3p3, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Reset IC. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_reset_reg, TC2K_reset_mask, TC2K_reset_Reset, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_reset_reg, TC2K_reset_mask, TC2K_reset_NotReset, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgResolveBbErrCondition(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_freqSetFloHz(tunerCfg, 135000000);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttEna_reg, TC2K_ttEna_mask, TC2K_ttEna_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgBbAgcDelay(tunerCfg, 10);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgIfAccTopCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Changes to _mt_fe_tn_tc2000_cfgIfBwCal for _mt_fe_tn_tc2000_ifAccTopCal.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgIfAccTopCal (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgIfAccTopCal() 2wb optimization. No external r/w sync required before calling. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttSrc_reg, TC2K_ttSrc_mask, TC2K_ttSrc_SpinNCO, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_freqSetFspinHz(tunerCfg, TC2K_cfgIfAccTopFttHz_Ref);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgTfCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Changes to tc2000_tn_cfg _mt_fe_tn_tc2000_ifAccTopCal for _mt_fe_tn_tc2000_tfCal.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgTfCal (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgTfCal() 2wb optimization. Requires and external w sync after calling. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitLo_reg, TC2K_shPinLimitLo_mask, 255, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_shPinLimitHi_reg, TC2K_shPinLimitHi_mask, 15, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetCtrl_reg, TC2K_calDetCtrl_mask, TC2K_calDetCtrl_SM, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCapWdWr_reg, TC2K_ifCapWdWr_mask, 31, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgCalDetRead()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[out] value.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgCalDetRead(tc2000_tn_config *tunerCfg, S32 *value)
{
	TC2K_RET retVal;
	U8 buff[2];
	U8 calDetDone;

	U8 i;

	/* @doc _mt_fe_tn_tc2000_cfgCalDetRead() 2wb optimization. May require external w sync before calling. */
	/* @doc _mt_fe_tn_tc2000_cfgCalDetRead() 2wb optimization. Read registers up to date on exit. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetTrig_reg, TC2K_calDetTrig_mask, TC2K_calDetTrig_Trigger, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetTrig_reg, TC2K_calDetTrig_mask, TC2K_calDetTrig_ClearTrigger, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc pole calDetDone. Timeout = 30 msec. */
	for (i=0;i<30;i++) {
		retVal = _mt_fe_tn_tc2000_get_reg(tunerCfg, TC2K_calDetDone_reg, TC2K_calDetDone_mask, &calDetDone, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		if (calDetDone == TC2K_calDetDone_done)
			break;
	}

	if (calDetDone != TC2K_calDetDone_done)
		return TC2K_err_cfgCalDet_timeout;

	retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_calDetRdLo_reg, buff, 2, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	*value = (((S32) (buff[1] & 0x7f))<<8) + (S32) buff[0];

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgReadTmprAcc()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[out] tmpr.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgReadTmprAcc(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8  buff[2];
	U8  agcAccSelRestore;
	U8  rfCloseLoopRestore;

	/* @doc _mt_fe_tn_tc2000_cfgReadTmprAcc() 2wb optimization. No external r/w sync required. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, &agcAccSelRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_rfCloseLoop_reg, TC2K_rfCloseLoop_mask, &rfCloseLoopRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_RFDetADC, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfCloseLoop_reg, TC2K_rfCloseLoop_mask, TC2K_rfCloseLoop_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tmprCtrl_reg, TC2K_tmprCtrl_mask, TC2K_tmprCtrl_TmprPwrOn, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	_mt_sleep(1);

	retVal = _mt_fe_tn_tc2000_i2c_read (tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfCloseLoop_reg, TC2K_rfCloseLoop_mask, rfCloseLoopRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tmprCtrl_reg, TC2K_tmprCtrl_mask, TC2K_tmprCtrl_TmprPwrOff, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, agcAccSelRestore, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	tunerCfg->tunerData.curTmpr = (((S16) buff[1])<<8) + (S16) buff[0];
	if (tunerCfg->tunerData.curTmpr > 2047)
		tunerCfg->tunerData.curTmpr -= 2048;
	else
		tunerCfg->tunerData.curTmpr += 2048;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgBbAgcDelay()
 *
 * @param[in] tunerCfg Tuner configuration structure.
 * @param[in] timeOutMsec.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgBbAgcDelay(tc2000_tn_config *tunerCfg, U8 timeOutMsec)
{
	TC2K_RET retVal;
	S16 agcErr;
	U8  buff[2];
	U8  i;

	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. May require external w sync before calling. */
	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. Read registers up to date on exit. */

	/* @doc Wait for AGC to settel to +/- 40 (accum value) for timeOutMsec. */
	for (i=0;i<timeOutMsec;i++) {
		retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcErrLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		agcErr = (S16) (buff[1] & 0x1F);
		agcErr <<= 8;
		agcErr += (S16) buff[0];
		/* agcErr is 13 bits 2's comp. convert to absolute value. */
		if (agcErr > 0x0FFF)
			/* Value is negative, convert to absolute value. */
			agcErr = 0x2000 - agcErr;
		if (agcErr < 40)
			break;
		_mt_sleep(1);
	}

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgAgcDelay(tc2000_tn_config *tunerCfg, U8 timeOutMsec)
{
	TC2K_RET retVal;
	U8	agcAccSelRestore;
	S16 bbAgcDiff = 0;
	S16 rfAgcDiff = 0;
	S16 digAgcDiff = 0;
	S16 bbAgcNew = 0;
	S16 rfAgcNew = 0;
	S16 digAgcNew = 0;
	S16 bbAgcOld;
	S16 rfAgcOld;
	S16 digAgcOld;
	U8  buff[2];
	U8  i;

	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. May require external w sync before calling. */
	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. Read registers up to date on exit. */

	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, &agcAccSelRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	bbAgcOld = 0;
	rfAgcOld = 4095;
	digAgcOld = 0;

	/* @doc Wait for AGC to settel to +/- 40 (accum value) for timeOutMsec. */
	for (i=0;i<timeOutMsec;i++) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_BB, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		bbAgcNew = (S16) (buff[1] & 0xF);
		bbAgcNew <<= 8;
		bbAgcNew += (S16) buff[0];

		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_RF, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		rfAgcNew = (S16) (buff[1] & 0xF);
		rfAgcNew <<= 8;
		rfAgcNew += (S16) buff[0];

		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_DigitalAGC, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		digAgcNew = (S16) (buff[1] & 0x3F);
		digAgcNew <<= 8;
		digAgcNew += (S16) buff[0];

		bbAgcDiff = bbAgcNew - bbAgcOld;
		rfAgcDiff = rfAgcNew - rfAgcOld;
		digAgcDiff = digAgcNew - digAgcOld;

		if (bbAgcDiff < 75 && bbAgcDiff > -75 &&
			rfAgcDiff < 75 && rfAgcDiff > -75 &&
			digAgcDiff < 75 && digAgcDiff > -75)
			break;

		_mt_sleep(1);

		bbAgcOld = bbAgcNew;
		rfAgcOld = rfAgcNew;
		digAgcOld = digAgcNew;
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, agcAccSelRestore, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	tunerCfg->tunerData.agcDelayDebug[0] = timeOutMsec;
	tunerCfg->tunerData.agcDelayDebug[1] = i;
	tunerCfg->tunerData.agcDelayDebug[2] = bbAgcNew;
	tunerCfg->tunerData.agcDelayDebug[3] = rfAgcNew;
	tunerCfg->tunerData.agcDelayDebug[4] = digAgcNew;
	tunerCfg->tunerData.agcDelayDebug[5] = bbAgcDiff;
	tunerCfg->tunerData.agcDelayDebug[6] = rfAgcDiff;
	tunerCfg->tunerData.agcDelayDebug[7] = digAgcDiff;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgRssi (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8  agcAccSelRestore;
	U8  buff[2];
	S16 acc;
	S16 gain_s16;
	S32 gain_s32;
	U8  vga2Ena;
	U8	lnaBand;
	U8  i;
	S16 *lutPtr;
	S16 lutLen;
	S16 gainStep;
	S16 gainMin;

	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. No external rw sync required. */

	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, &agcAccSelRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* doc Get lnaBand for calculating rfAtt and rssiCorrFactor. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, &lnaBand, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	if (lnaBand == TC2K_tfBand_LB)
		lnaBand = 0;
	else if (lnaBand == TC2K_tfBand_MB)
		lnaBand = 1;
	else
		lnaBand = 2;

	/* @doc Read IF accumulator. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_BB, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	acc = (((S16) buff[1])<<8) + ((S16) buff[0]);

	/* @doc Estimate IF gain in 10ths of a dB. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, &vga2Ena, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	if (vga2Ena == TC2K_ifVga2Ena_off)
		i = 0;
	else
		i = 1;
	gain_s32 = (S32) (acc - tc2000_rssiBbAccMin[i]) ;
	gain_s32 *= (S32) tc2000_rssiBbSlope[i];
	gain_s32 /= (S32) 1000;
	gain_s32 += (S32) tc2000_rssiBbGmin[i];
	if (gain_s32 < (S32) tc2000_rssiBbGmin[i])
		tunerCfg->RSSI = tc2000_rssiBbGmin[i];
	else if (gain_s32 > (S32) tc2000_rssiBbGmax[i])
		tunerCfg->RSSI = tc2000_rssiBbGmax[i];
	else
		tunerCfg->RSSI = (S16) gain_s32;

	/* @doc Read RF accumulator. */
	retVal = _mt_fe_tn_tc2000_set_reg(tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_RF, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	acc = (((S16) buff[1])<<8) + ((S16) buff[0]);

	/* @doc Estimate RF gain in 10ths of a dB. */
	i = lnaBand * TC2K_rssiRfAttLutLnaBandOffset + tunerCfg->rfAttMode * TC2K_rssiRfAttLutRfAttCfgOffset;
	lutPtr = &tc2000_rssiRfAttLut[i];
	lutLen = TC2K_rssiRfAttLutLen;
	i = lnaBand * TC2K_rssiRfAttLnaBandOffset + tunerCfg->rfAttMode;
	gainMin = tc2000_rssiRfAttAcc0[i];
	gainStep = tc2000_rssiRfAttStep[i];
	retVal = _mt_fe_tn_tc2000_cfgRssiSearchLut(acc, lutPtr, lutLen, gainMin, gainStep, &gain_s16);
	if (retVal != TC2K_ok)
		return retVal;
	tunerCfg->RSSI += gain_s16;

	/* @doc Read digital accumulator. */
	retVal = _mt_fe_tn_tc2000_set_reg(tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_DigitalAGC, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	acc = (((S16) buff[1])<<8) + ((S16) buff[0]);

	/* @doc Estimate digital gain in 10ths of a dB. */
	lutPtr = tc2000_rssiDigGainLut;
	lutLen = TC2K_rssiDigGainLen;
	gainMin = TC2K_rssiDigGainMin;
	gainStep = TC2K_rssiDigGainStep;
	retVal = _mt_fe_tn_tc2000_cfgRssiSearchLut (acc, lutPtr, lutLen, gainMin, gainStep, &gain_s16);
	if (retVal != TC2K_ok)
		return retVal;
	tunerCfg->RSSI += gain_s16;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, agcAccSelRestore, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	if (vga2Ena == TC2K_ifVga2Ena_on)
		tunerCfg->RSSI += (tunerCfg->tunerData.curTmpr - TC2K_cfgTmprAccReading25Deg) * tc2000_rssiTmprComp[0] / TC2K_cfgTempAccDelta10Deg;
	else
		tunerCfg->RSSI += (tunerCfg->tunerData.curTmpr - TC2K_cfgTmprAccReading25Deg) * tc2000_rssiTmprComp[1] / TC2K_cfgTempAccDelta10Deg;

	/* @doc Compensate for low noise and high power mode. */
	tunerCfg->RSSI += tunerCfg->tunerData.rssiLnaOffset;

	tunerCfg->RSSI *= -1;

	/* @doc Compensate for PAR */
	tunerCfg->RSSI -= tc2000_rssiPar[tunerCfg->modulation];
	tunerCfg->tunerData.parDebug = tc2000_rssiPar[tunerCfg->modulation];

	tunerCfg->RSSI += tc2000_rssiCF[tunerCfg->lnaModeStatus + lnaBand * 4];

	/* @doc Round and convert to dB (from tenths of a dB). */
	if (tunerCfg->RSSI < 0)
		tunerCfg->RSSI -= 5;
	else
		tunerCfg->RSSI += 5;
	tunerCfg->RSSI /= 10;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgRssiSearchLut (
	S16 acc,
	S16 *lut,
	S16 lutLen,
	S16 gainMin,
	S16 gainStep,
	S16 *gain)
{

	S16 i;

	if (acc <= lut[0])
		*gain = gainMin;
	else if (acc >= lut[lutLen - 1])
		*gain = (lutLen - 1) * gainStep + gainMin;
	else {
		for (i=0; i<lutLen; i++) {
			if (acc <= lut[i])
				break;
		}
		*gain = gainMin + (i-1) * gainStep;
		*gain += ((acc - lut[i-1]) * gainStep) / (lut[i] - lut[i-1]);
	}

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgHystModeOff (tc2000_tn_config *tunerCfg)
{

	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgHystModeOff() 2wb optimization. Requires and external w sync after calling. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfHysMode_reg, TC2K_rfHysMode_mask, TC2K_rfHysMode_off, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgHystModeOn (tc2000_tn_config *tunerCfg)
{

	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgHystModeOn() 2wb optimization. Does not require and external w sync after calling. */

	/* @doc Turn on BB and DC hysteresis if modulation is analog. SECAM */

	/* @doc Turn on dc, bb and rf hsyteresis if SECAM-L OTA. */
	if (tunerCfg->modulation == TC2K_modulation_SECAM_L &&
		tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota)
	{
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfHysMode_reg, TC2K_rfHysMode_mask, TC2K_rfHysMode_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_on, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return TC2K_ok;
	}
	/* @doc If in SECAM-L CATV or any other analog CATV or OTA turn on dd and bb hysteresis. */
	else if (
		tunerCfg->modulation == TC2K_modulation_PAL_B ||
		tunerCfg->modulation == TC2K_modulation_PAL_D_I_K ||
		tunerCfg->modulation == TC2K_modulation_PAL_G_H ||
		tunerCfg->modulation == TC2K_modulation_NTSC ||
		tunerCfg->modulation == TC2K_modulation_SECAM ||
		tunerCfg->modulation == TC2K_modulation_SECAM_L)
	{
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_bbHysMode_reg, TC2K_bbHysMode_mask, TC2K_bbHysMode_on, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dcHysMode_reg, TC2K_dcHysMode_mask, TC2K_dcHysMode_on, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return TC2K_ok;
	}

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_cfgCalRfDet (tc2000_tn_config *tunerCfg)
{

	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_cfgCalRfDet() 2wb optimization. No external r/w sync required. */

	/* @doc Cal RF detector. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetCalFrc_reg, TC2K_rfDetCalFrc_mask, TC2K_rfDetCalFrc_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	_mt_sleep(2);
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetCalFrc_reg, TC2K_rfDetCalFrc_mask, TC2K_rfDetCalFrc_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfDetCalAuto_reg, TC2K_rfDetCalAuto_mask, TC2K_rfDetCalAuto_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_cfgChanChgOpt()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_cfgChanChgOpt (tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	U8	ifAgcLoopBwRestore;
	U8	rfAgcLoopBwRestore;
	U8	ifAgcShiftRestore;
	U8	agcWindowRestore;
	U8	dagcWindowRestore;
	U8	digAgcBWRestore;
	U8	tfAttCwRestore;
	U8	agcAccSelRestore;

	U8	lnaMode;
	U8	vga2Ena;
	U8	tfAttCw;
	U8	ifAccTopIndex;

	S16	PinLowNoise;

	/* @doc _mt_fe_tn_tc2000_cfgBase() 2wb optimization. No external r/w sync required before calling. */

	/* @doc Save AGC settings to restore later. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, &rfAgcLoopBwRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, &ifAgcLoopBwRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, &ifAgcShiftRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, &agcWindowRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, &dagcWindowRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_digAgcBW_reg, TC2K_digAgcBW_mask, &digAgcBWRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, &agcAccSelRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, &tfAttCwRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota) {
		if (tunerCfg->rxPwr == TC2K_rxPwr_low || tunerCfg->rxPwr == TC2K_rxPwr_auto) {
			lnaMode = TC2K_lnaMode_terLoNoise;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_low_noise;
			vga2Ena = TC2K_ifVga2Ena_on;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
			tfAttCw = (tfAttCwRestore >> 3) + 3;
			if (tfAttCw > 15) tfAttCw = 15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
			tfAttCw <<= 3;
		}
		else if (tunerCfg->rxPwr == TC2K_rxPwr_nominal) {
			lnaMode = TC2K_lnaMode_terNorm;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_ota;
			vga2Ena = TC2K_ifVga2Ena_on;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
			tfAttCw = tfAttCwRestore;
			tunerCfg->tunerData.rssiLnaOffset = 0;
		}
		else if (tunerCfg->rxPwr == TC2K_rxPwr_medium) {
			lnaMode = TC2K_lnaMode_terNorm;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_ota;
			vga2Ena = TC2K_ifVga2Ena_off;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
			tfAttCw = (tfAttCwRestore >> 3) + 8;
			if (tfAttCw > 15) tfAttCw = 15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
			tfAttCw <<= 3;
		}
		else {
			lnaMode = TC2K_lnaMode_cab;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
			vga2Ena = TC2K_ifVga2Ena_off;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
			tfAttCw = TC2K_tfAttCw_AttCtl_15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - 15) * 7;
		}
	}
	else {
		if (tunerCfg->rxPwr == TC2K_rxPwr_low) {
			lnaMode = TC2K_lnaMode_terLoNoise;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_low_noise;
			vga2Ena = TC2K_ifVga2Ena_on;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
			tfAttCw = (tfAttCwRestore >> 3) + 3;
			if (tfAttCw > 15) tfAttCw = 15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
			tfAttCw <<= 3;
		}
		else if (tunerCfg->rxPwr == TC2K_rxPwr_nominal || tunerCfg->rxPwr == TC2K_rxPwr_auto) {
			lnaMode = TC2K_lnaMode_cab;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
			vga2Ena = TC2K_ifVga2Ena_off;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
			tfAttCw = tfAttCwRestore;
			tunerCfg->tunerData.rssiLnaOffset = 0;
		}
		else if (tunerCfg->rxPwr == TC2K_rxPwr_medium) {
			lnaMode = TC2K_lnaMode_cab;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
			vga2Ena = TC2K_ifVga2Ena_off;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
			tfAttCw = (tfAttCwRestore >> 3) + 4;
			if (tfAttCw > 15) tfAttCw = 15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
			tfAttCw <<= 3;
		}
		else {
			lnaMode = TC2K_lnaMode_cab;
			tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
			vga2Ena = TC2K_ifVga2Ena_off;
			tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
			ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
			tfAttCw = TC2K_tfAttCw_AttCtl_15;
			tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - 15) * 7;
		}
	}
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lnaMode_reg, TC2K_lnaMode_mask, lnaMode, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, vga2Ena, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, tfAttCw, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_ifSetAccTop (tunerCfg, ifAccTopIndex);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Freeze loops and disable agcSM. */
	/* @doc Ensures agcSm enabled in valid state */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfCloseLoop_reg, TC2K_rfCloseLoop_mask, TC2K_rfCloseLoop_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcEna_reg, TC2K_digAgcEna_mask, TC2K_digAgcEna_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcSmEna_reg, TC2K_agcSmEna_mask, TC2K_agcSmEna_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set ifGain to 4095 (max gain). */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_BB, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccWrHi_reg, TC2K_agcAccWrHi_mask, 15, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccWrLo_reg, TC2K_agcAccWrLo_mask, 255, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set digGain to 1. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, TC2K_agcAccSel_DigitalAGC, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccWrHi_reg, TC2K_agcAccWrHi_mask, 2, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccWrLo_reg, TC2K_agcAccWrLo_mask, 0, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Speed up RF, IF and Dig AGC loops. */
	/* @doc bbBW = 76.8 Hz, rfBw = 160 Hz */
	/* @doc bbRiseTime = 4.6 msec, rfRiseTime = 2.2 msec */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, TC2K_ifAgcLoopBw_BW_4p8Hz, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, TC2K_ifAgcShift_bbagcshft_7, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, TC2K_rfAgcLoopBw_BW80Hz, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, TC2K_agcWindow_rfWndwSz_42p67uS, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, TC2K_dagcWdwSzVal_WndwSz_85p33us, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcBW_reg, TC2K_digAgcBW_mask, TC2K_digAgcBW_BW90Hz, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Enable agcSm and close the agc loops . */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcSmEna_reg, TC2K_agcSmEna_mask, TC2K_agcSmEna_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfCloseLoop_reg, TC2K_rfCloseLoop_mask, TC2K_rfCloseLoop_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcEna_reg, TC2K_digAgcEna_mask, TC2K_digAgcEna_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Delay until agc loops settle (10 msec max). */
	retVal = _mt_fe_tn_tc2000_cfgAgcDelay (tunerCfg, 10);
	if (retVal != TC2K_ok)
		return retVal;

	if (tunerCfg->rxPwr == TC2K_rxPwr_auto)
	{
		retVal = _mt_fe_tn_tc2000_cfgRssi (tunerCfg);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc Set low noise switch point (dBm) for digital or analog mode. */
		if (tunerCfg->modulation == TC2K_modulation_DVB_C ||
			tunerCfg->modulation == TC2K_modulation_J83_Annex_B ||
			tunerCfg->modulation == TC2K_modulation_DVB_T ||
			tunerCfg->modulation == TC2K_modulation_ATSC ||
			tunerCfg->modulation == TC2K_modulation_ISDB_T ||
			tunerCfg->modulation == TC2K_modulation_DMB_T)
			if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota)
				PinLowNoise = -75;
			else
				PinLowNoise = -67;
		else
			PinLowNoise = -70;

		/* @doc Leave in low noise mode if rssi < PinLowNoise dBm. */
		if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota && tunerCfg->RSSI >= PinLowNoise) {
			if (tunerCfg->RSSI < -60) {
				lnaMode = TC2K_lnaMode_terNorm;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_ota;
				vga2Ena = TC2K_ifVga2Ena_on;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
				tfAttCw = tfAttCwRestore;
				tunerCfg->tunerData.rssiLnaOffset = 0;
			}
			else if (tunerCfg->RSSI < -55) {
				lnaMode = TC2K_lnaMode_terNorm;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_ota;
				vga2Ena = TC2K_ifVga2Ena_off;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
				tfAttCw = tfAttCwRestore;
				tunerCfg->tunerData.rssiLnaOffset = 0;
			}
			else if (tunerCfg->RSSI < -20) {
				lnaMode = TC2K_lnaMode_terNorm;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_ota;
				vga2Ena = TC2K_ifVga2Ena_off;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
				tfAttCw = (tfAttCwRestore >> 3) + 8;
				if (tfAttCw > 15) tfAttCw = 15;
				tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
				tfAttCw <<= 3;
			}
			else {
				lnaMode = TC2K_lnaMode_cab;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
				vga2Ena = TC2K_ifVga2Ena_off;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
				tfAttCw = TC2K_tfAttCw_AttCtl_15;
				tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - 15) * 7;
			}
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lnaMode_reg, TC2K_lnaMode_mask, lnaMode, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, vga2Ena, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, tfAttCw, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_ifSetAccTop (tunerCfg, ifAccTopIndex);
			if (retVal != TC2K_ok)
				return retVal;

			retVal = _mt_fe_tn_tc2000_cfgAgcDelay (tunerCfg, 10);
			if (retVal != TC2K_ok)
				return retVal;
		}
		/* @doc CATV, leave in nominal mode if -60 dBm >= rssi < -55 dBm. */
		else if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_cable && (tunerCfg->RSSI < -60 || tunerCfg->RSSI >= -55)){
			if (tunerCfg->RSSI < PinLowNoise) {
				lnaMode = TC2K_lnaMode_terLoNoise;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_low_noise;
				vga2Ena = TC2K_ifVga2Ena_on;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
				tfAttCw = (tfAttCwRestore >> 3) + 3;
				if (tfAttCw > 15) tfAttCw = 15;
				tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
				tfAttCw <<= 3;
			}
			else if (tunerCfg->RSSI < -60) {
				lnaMode = TC2K_lnaMode_cab;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
				vga2Ena = TC2K_ifVga2Ena_on;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_on_26dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_on_26dB;
				tfAttCw = tfAttCwRestore;
				tunerCfg->tunerData.rssiLnaOffset = 0;
			}
			else if (tunerCfg->RSSI < -20) {
				lnaMode = TC2K_lnaMode_cab;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
				vga2Ena = TC2K_ifVga2Ena_off;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
				tfAttCw = (tfAttCwRestore >> 3) + 4;
				if (tfAttCw > 15) tfAttCw = 15;
				tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - tfAttCw) * 7;
				tfAttCw <<=3;
			}
			else {
				lnaMode = TC2K_lnaMode_cab;
				tunerCfg->lnaModeStatus = TC2K_lnaModeStatus_cable;
				vga2Ena = TC2K_ifVga2Ena_off;
				tunerCfg->tunerData.ifAccTop_dB = TC2K_ifAccTop_Vga2_off_12dB;
				ifAccTopIndex = TC2K_ifAccTopIndex_Vga2_off_12dB;
				tfAttCw = TC2K_tfAttCw_AttCtl_15;
				tunerCfg->tunerData.rssiLnaOffset = ((tfAttCwRestore >> 3) - 15) * 7;
			}
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lnaMode_reg, TC2K_lnaMode_mask, lnaMode, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, vga2Ena, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, tfAttCw, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_ifSetAccTop (tunerCfg, ifAccTopIndex);
			if (retVal != TC2K_ok)
				return retVal;

			retVal = _mt_fe_tn_tc2000_cfgAgcDelay (tunerCfg, 10);
			if (retVal != TC2K_ok)
				return retVal;
		}
	}

	/* @doc Restore AGC settings. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccSel_reg, TC2K_agcAccSel_mask, agcAccSelRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcLoopBw_reg, TC2K_ifAgcLoopBw_mask, ifAgcLoopBwRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rfAgcLoopBw_reg, TC2K_rfAgcLoopBw_mask, rfAgcLoopBwRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcShift_reg, TC2K_ifAgcShift_mask, ifAgcShiftRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcWindow_reg, TC2K_agcWindow_mask, agcWindowRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_dagcWdwSzVal_reg, TC2K_dagcWdwSzVal_mask, dagcWindowRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_digAgcBW_reg, TC2K_digAgcBW_mask, digAgcBWRestore, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start Channel Selection Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

/* lc lut for 24 MHz xtal and Fcomp = 0.5 or 1.0 MHz */
static const S32 TC2K_freqLcLut[97] = {
 0x1FFFFF, 0x1FFEE7, 0x1FFB9D, 0x1FF622, 0x1FEE76, 0x1FE49A, 0x1FD88E, 0x1FCA53, 0x1FB9EB, 0x1FA755,
 0x1F9295, 0x1F7BAB, 0x1F6298, 0x1F475E, 0x1F2A00, 0x1F0A7F, 0x1EE8DD, 0x1EC51D, 0x1E9F41, 0x1E774C,
 0x1E4D40, 0x1E2121, 0x1DF2F1, 0x1DC2B3, 0x1D906C, 0x1D5C1D, 0x1D25CC, 0x1CED7B, 0x1CB32E, 0x1C76EA,
 0x1C38B3, 0x1BF88C, 0x1BB67B, 0x1B7283, 0x1B2CAA, 0x1AE4F4, 0x1A9B66, 0x1A5005, 0x1A02D7, 0x19B3E0,
 0x196327, 0x1910AF, 0x18BC80, 0x18669F, 0x180F12, 0x17B5DF, 0x175B0C, 0x16FE9F, 0x16A09E, 0x164111,
 0x15DFFD, 0x157D69, 0x15195C, 0x14B3DD, 0x144CF3, 0x13E4A5, 0x137AF9, 0x130FF8, 0x12A3A8, 0x123612,
 0x11C73B, 0x11572D, 0x10E5EF, 0x107388, 0x100000, 0x0F8B60, 0x0F15AF, 0x0E9EF5, 0x0E273B, 0x0DAE88,
 0x0D34E5, 0x0CBA5B, 0x0C3EF1, 0x0BC2B1, 0x0B45A2, 0x0AC7CD, 0x0A493B, 0x09C9F5, 0x094A03, 0x08C96E,
 0x08483F, 0x07C67E, 0x074435, 0x06C16D, 0x063E2E, 0x05BA82, 0x053671, 0x04B204, 0x042D45, 0x03A83D,
 0x0322F5, 0x029D76, 0x0217C8, 0x0191F6, 0x010C09, 0x008609, 0x000000
};

/**
 * _mt_fe_tn_tc2000_freqSetFchHz()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 *
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetFchHz(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc Set Flo first. Fspin depends on actual programed value of Flo (limited Flo relolution). */
	if (tunerCfg->modulation == TC2K_modulation_NTSC)
		retVal = _mt_fe_tn_tc2000_freqSetFloHz(tunerCfg, tunerCfg->FchanHz - 250000);
	else if ((tunerCfg->modulation == TC2K_modulation_PAL_G_H) || ((tunerCfg->modulation == TC2K_modulation_PAL_B) && (tunerCfg->ifBw == TC2K_ifBw_8MHz)))
		retVal = _mt_fe_tn_tc2000_freqSetFloHz(tunerCfg, tunerCfg->FchanHz - 500000);
	else
		retVal = _mt_fe_tn_tc2000_freqSetFloHz(tunerCfg, tunerCfg->FchanHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set FspinHz. */
	if ((tunerCfg->modulation == TC2K_modulation_PAL_G_H) || ((tunerCfg->modulation == TC2K_modulation_PAL_B) && (tunerCfg->ifBw == TC2K_ifBw_8MHz)))	{
		if (tunerCfg->specInvSel == TC2K_specInvSel_off)
			retVal = _mt_fe_tn_tc2000_freqSetFspinHz(tunerCfg, tunerCfg->tunerData.FloHz + 500000 - tunerCfg->FchanHz);
		else
			retVal = _mt_fe_tn_tc2000_freqSetFspinHz(tunerCfg, tunerCfg->FchanHz - tunerCfg->tunerData.FloHz - 500000);
	}
	else {
		if (tunerCfg->specInvSel == TC2K_specInvSel_off)
			retVal = _mt_fe_tn_tc2000_freqSetFspinHz(tunerCfg, tunerCfg->tunerData.FloHz - tunerCfg->FchanHz);
		else
			retVal = _mt_fe_tn_tc2000_freqSetFspinHz(tunerCfg, tunerCfg->FchanHz - tunerCfg->tunerData.FloHz);
	}
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set FifHz. */
	if ((tunerCfg->modulation == TC2K_modulation_PAL_G_H) || ((tunerCfg->modulation == TC2K_modulation_PAL_B) && (tunerCfg->ifBw == TC2K_ifBw_8MHz)))	{
		if (tunerCfg->specInvSel == TC2K_specInvSel_off)
			retVal = _mt_fe_tn_tc2000_freqSetFifHz (tunerCfg, tunerCfg->FifHz - 500000);
		else
			retVal = _mt_fe_tn_tc2000_freqSetFifHz (tunerCfg, tunerCfg->FifHz + 500000);
	}
	else
		retVal = _mt_fe_tn_tc2000_freqSetFifHz (tunerCfg, tunerCfg->FifHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Store channel frequency (in tuner data structure). */
	tunerCfg->tunerData.FchanHz = tunerCfg->tunerData.FloHz;
	if (tunerCfg->specInvSel == TC2K_specInvSel_off) {
		tunerCfg->tunerData.FchanHz -= tunerCfg->tunerData.FspinHz;
		tunerCfg->tunerData.FchanHz += tunerCfg->FifHz - (tunerCfg->tunerData.FifHz - tunerCfg->FifErrHz);
	}
	else {
		tunerCfg->tunerData.FchanHz += tunerCfg->tunerData.FspinHz;
		tunerCfg->tunerData.FchanHz -= tunerCfg->FifHz - (tunerCfg->tunerData.FifHz - tunerCfg->FifErrHz);
	}

	/* @doc Calculate and store channel frequency error (in tuner data structure). */
	tunerCfg->FchanErrHz = tunerCfg->tunerData.FchanHz - tunerCfg->FchanHz;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetFxtal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success, TC2K_err_rw otherwise.
 *
 * @note Check that the xtal freq is in range and then move it into the tuner data structure..
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetFxtal(tc2000_tn_config *tunerCfg)
{
	/*********************************************
	* parameter checked in mtTuner
	* add case statement when more values added
	* currently only TC2K_Fxtal_24MHz defined
	*********************************************/
	/* @doc Store crystal frequency (in tuner data structure). */
	tunerCfg->tunerData.FxtalHz = 24000000;

	/*************************************************************************
	* determined b Fxtal (put in case statement).
	* ex 24 MHz:
	* Fspin = 24MHz/2 * Nspin / 2^20 = 24MHz * N / 2^21
	* 		 = 46875 * 2^9 * N / 2^21
	* 		 = 46875 * N / 2^12
	* Fspin,max = 2^31-1 / 2^12 too prevent overflow
	* 		 < 2^31 / 2^12 = 2^19 = 524288
	*
	* Calculating Fspin Multiply first when Fspin < Fspin,max (minimal error).
	* Divide first if Fspin >= Fspin,max
	* (error due to integer divide)
	*
	* For synth, |Fspin,max| ~ 250 KHz
	*************************************************************************/
	/* @doc Store FspinMaxHz, FspinN and FspinD (in tuner data structure). */
	tunerCfg->tunerData.FspinMaxHz = 524288;
	tunerCfg->tunerData.FspinN = 4096;
	tunerCfg->tunerData.FspinD = 46875;

	/*************************************************************************
	* ex 24 MHz:
	* Fif = 24MHz*8 * Nspin / 2^16 = 24MHz * N / 2^13
	* 	 = 46875 * 2^9 * N / 2^13
	* 	 = 46875 * N / 2^4
	* Fif,max = 2^31-1 / 2^4 too prevent overflow
	* 		 < 2^31 / 2^4 = 2^27 = 134217728
	*************************************************************************/
	/* @doc Store FifMaxHz, FifN and FifD (in tuner data structure). */
	tunerCfg->tunerData.FifMaxHz = 96000000;
	tunerCfg->tunerData.FifN = 16;
	tunerCfg->tunerData.FifD = 46875;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetFloHz()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] freqHz Frequency to set to.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_freqPllSm_timeout if unable to lock
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetFloHz(tc2000_tn_config *tunerCfg, S32 freqHz)
{
	TC2K_RET retVal;

	S32 N;
	S32 B;
	S32 A;
	S32 R;
	S32 LoDiv;
	S32 LoDivPrime;
	S32 FloHz;
	S32 FvcoPrimeHz;

	U8 LoDivR;
	U8 mixMode;
	U8 synNrange;
	U8 curVco;
	U8 dLock;
	U8 aLock;

	U8 SynVtune;

	S16 i;

	/* @doc _mt_fe_tn_tc2000_freqSetFloHz() 2wb optimization. No external sync required before or after. */

	/* @doc Flo = N*2*Fcomp/LoDiv.																				 */
	/* @doc Fstep = 2*Fcomp/LoDiv.																				 */
	/* @doc Frequency is rounded down when exactly in the middle of two possible frequencies.					 */
	/* @doc Ex. Flo = N*2*Fcomp/LoDiv + (2*Fcomp/LoDiv)/2														 */
	/* @doc		 = (N+1)*2*Fcomp/LoDiv - (2*Fcomp/LoDiv)/2														 */
	/* @doc	 Flo,actual = N*2*Fcomp/LoDiv																		 */
	/* @doc Break points before rounding: 435, 217.5, 108.75, 54.375 MHz. (determin LoDiv and the number of		 */
	/* @doc mixer phases).																						 */
	/* @doc Rounding for each break point is done with the LoDiv setting of the next lower break point.			*/
	/* @doc Ex. If F < 435 MHz then LoDiv = 8.																	 */
	/* @doc	 Fstep = 1MHz*2/8 = 0.25MHz																			 */
	/* @doc	 If Flo = 434,875,000  then  Flo,actual = 434,750,000												 */
	/* @doc	 If Flo = 434,875,001  then  Flo,actual = 435,000,000												 */
	/* @doc All break points must be modified for rounding. 435MHz becomes 435-(2*fcomp/8)/2 = 434.875MHz.		 */
	/* @doc 8 is the LoDiv setting from the next lower break point, 217.5 MHz in this case.						 */

	/* @doc Fstep,max = 0.25MHz for NTSC. Fcomp must be changed from 1MHz to 0.5MHz when						 */
	/* @doc Flo > 434.875. Fcomp cannot be 0.5 MHz at frequencies below 434.875MHz (affects synth performance)	 */
	/* @doc Therfore, the break frequencies do not change with Fcomp.											 */


	if (freqHz > 434875000) {
		LoDiv = 4;
		LoDivR = TC2K_synLoDiv_div4Phs2;
		mixMode = TC2K_mixMode_phs2;
	}
	else if (freqHz > 217437500) {
		LoDiv = 8;
		LoDivR = TC2K_synLoDiv_div8Phs4;
		mixMode = TC2K_mixMode_phs4;
	}
	else if (freqHz > 108718750) {
		LoDiv = 16;
		LoDivR = TC2K_synLoDiv_div16Phs8;
		mixMode = TC2K_mixMode_phs8;
	}
	else if (freqHz > 54359375) {
		LoDiv = 32;
		LoDivR = TC2K_synLoDiv_div32Phs8;
		mixMode = TC2K_mixMode_phs8;
	}
	else {
		LoDiv = 64;
		LoDivR = TC2K_synLoDiv_div64Phs8;
		mixMode = TC2K_mixMode_phs8;
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synLoDiv_reg, TC2K_synLoDiv_mask, LoDivR, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_mixMode_reg, TC2K_mixMode_mask, mixMode, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (freqHz > 434875000 && (	tunerCfg->modulation == TC2K_modulation_NTSC))
	{
		tunerCfg->tunerData.FcompHz = 500000;
		R = 48;
	}
	else {
		tunerCfg->tunerData.FcompHz = 1000000;
		R = 24;
	}

	/* @doc Calculate N. Round to nearest integer. Round 0.5 down. */
	/* @doc N = Flo * LoDiv' / Fcomp */
	LoDivPrime = LoDiv / 2;
	N = freqHz * LoDivPrime;
	N += tunerCfg->tunerData.FcompHz / 2 - 1;
	N /= tunerCfg->tunerData.FcompHz;
	/* @doc Calculate Flo = Fcomp * N / LoDiv' */
	FloHz = tunerCfg->tunerData.FcompHz * N;
	FloHz /= LoDivPrime;

	B = N / 16;
	A = N - B * 16;
	FvcoPrimeHz = FloHz * LoDivPrime;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synN_reg, TC2K_synN_mask, (U8) B, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synA_reg, TC2K_synA_mask, (U8) A, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synR_reg, TC2K_synR_mask, (U8) R, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set synNrange based on N. */
	if (N < 2070 / 2)
		synNrange = TC2K_synNrange_range0;
	else if (N < 2461 / 2)
		synNrange = TC2K_synNrange_range1;
	else if (N < 2926 / 2)
		synNrange = TC2K_synNrange_range2;
	else
		synNrange = TC2K_synNrange_range3;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synNrange_reg, TC2K_synNrange_mask, synNrange, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Restore optimized values. */
	/* @doc Saved in entr_cfgIfBwCal() before first call to this subroutine. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_vcoIsel_reg, TC2K_vcoIsel_mask, tunerCfg->tunerData.vcoIselRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synCPcur_reg, TC2K_synCPcur_mask, tunerCfg->tunerData.synCPcurRestore, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synCpSmEna_reg, TC2K_synCpSmEna_mask, TC2K_synCpSmEna_ena, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_vcoIselEna_reg, TC2K_vcoIselEna_mask, TC2K_vcoIselEna_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set the starting vco to vco0 */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synVcoWr_reg, TC2K_synVcoWr_mask, TC2K_synVcoWr_VCO0, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Triger the SM and pole for locked condition. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synSmTrig_reg, TC2K_synSmTrig_mask, TC2K_synSmTrig_ena, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synSmTrig_reg, TC2K_synSmTrig_mask, TC2K_synSmTrig_dis, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	for (i=0; i<TC2K_freqLoSmTimeOutMsec; i++) {
		/* read analog and digital lock, break if locked. */
		retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synAnLock_reg, TC2K_synAnLock_mask, &aLock, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synDigLock_reg, TC2K_synDigLock_mask, &dLock, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		if ((aLock == TC2K_synAnLock_on) && (dLock == TC2K_synDigLock_on))
			break;
		_mt_sleep(1);
	}

	/* @doc Trigger a second time if not locked */
	if ((aLock != TC2K_synAnLock_on) || (dLock != TC2K_synDigLock_on)) {
		/* @doc Not locked retriggering, set pllRetrigCnt = 1 in tuner data structure. */
		tunerCfg->tunerData.pllReTrigCntDebug = 1;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synSmTrig_reg, TC2K_synSmTrig_mask, TC2K_synSmTrig_ena, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synSmTrig_reg, TC2K_synSmTrig_mask, TC2K_synSmTrig_dis, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		for (i=0; i<TC2K_freqLoSmTimeOutMsec; i++) {
			retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synAnLock_reg, TC2K_synAnLock_mask, &aLock, TC2K_2wbSync_yes);
			if (retVal != TC2K_ok)
				return retVal;
			retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synDigLock_reg, TC2K_synDigLock_mask, &dLock, TC2K_2wbSync_no);
			if (retVal != TC2K_ok)
				return retVal;
			if ((aLock == TC2K_synAnLock_on) && (dLock == TC2K_synDigLock_on))
				break;
			_mt_sleep(1);
		}
	}
	else
		/* @doc Not lock, set pllRetrigCnt = 0 in tuner data structure. */
		tunerCfg->tunerData.pllReTrigCntDebug = 0;

	if ((aLock != TC2K_synAnLock_on) || (dLock != TC2K_synDigLock_on))
		return TC2K_err_freqPllSm_timeout;

	/* @doc Optimize phase noise on if vco = 6 or 8. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synVcoRd_reg, TC2K_synVcoRd_mask, &curVco, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_synVtuneRd_reg, TC2K_synVtuneRd_mask, &SynVtune, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	if (curVco == TC2K_synVcoWr_VCO8) {
		/* @doc If synVtun > 4 then Icp = 1.6mA, override CpSm. */
		if (SynVtune > 4)
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synCPcur_reg, TC2K_synCPcur_mask, TC2K_synCPcur_mA1p6, TC2K_2wbSync_no);
		/* @doc Else Icp = 0.8mA. */
		else
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synCPcur_reg, TC2K_synCPcur_mask, TC2K_synCPcur_mA0p8, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_synCpSmEna_reg, TC2K_synCpSmEna_mask, TC2K_synCpSmEna_dis, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
	}
	else if (curVco == TC2K_synVcoWr_VCO6 && SynVtune <= 8) {
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_vcoIsel_reg, TC2K_vcoIsel_mask, TC2K_vcoIsel_sel1, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_vcoIselEna_reg, TC2K_vcoIselEna_mask, TC2K_vcoIselEna_on, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
	}

	/* @doc Save FloHz in tuner data structure. */
	tunerCfg->tunerData.FloHz = FloHz;

	/* @doc _mt_fe_tn_tc2000_freqCfgTtGen() 2wb optimization. Syncs before exiting */
	retVal = _mt_fe_tn_tc2000_freqCfgTtGen(tunerCfg, (U8) LoDiv);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetFspinHz()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] FspinHz
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetFspinHz(tc2000_tn_config *tunerCfg, S32 FspinHz)
{
	TC2K_RET retVal;
	S32 N;
	S32 Neg;
	U8  buff[3];

	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. No external r/w sync required. */

	if (FspinHz < 0) {
		Neg = 0x100000;
		FspinHz *= -1;
	}
	else
		Neg = 0;

	if (FspinHz < tunerCfg->tunerData.FspinMaxHz) {
		N = FspinHz * tunerCfg->tunerData.FspinN;
		N /= tunerCfg->tunerData.FspinD;
		if (Neg != 0)
			N = Neg - N;
	}
	else {
		/* will be rounded it FspinHz % tunerCfg->tunerData.FspinD != 0 */
		N = FspinHz /  tunerCfg->tunerData.FspinD;
		N *= tunerCfg->tunerData.FspinN;
	}

	retVal = _mt_fe_tn_tc2000_i2c_read (tunerCfg, TC2K_rotHi_reg, buff,1, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	buff[2] = (buff[0] & 0xf0) | ((U8) (N>>16));
	buff[0] = (U8) N;
	buff[1] = (U8) (N>>8);
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_rotLo_reg, buff, 3, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	if (Neg != 0)
		N = N - Neg;

	if (FspinHz < tunerCfg->tunerData.FspinMaxHz) {
		tunerCfg->tunerData.FspinHz = N * tunerCfg->tunerData.FspinD;
		tunerCfg->tunerData.FspinHz /= tunerCfg->tunerData.FspinN;
	}
	else {
		tunerCfg->tunerData.FspinHz = N  / tunerCfg->tunerData.FspinN;
		tunerCfg->tunerData.FspinHz *= tunerCfg->tunerData.FspinD;
	}

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetFifHz()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] FifHz
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetFifHz(tc2000_tn_config *tunerCfg, S32 FifHz)
{
	TC2K_RET retVal;
	S32 N;
	U8  buff[2];

	/* @doc _mt_fe_tn_tc2000_freqSetFifHz() 2wb optimization. Requires a w sync after calling. */

	N = FifHz * tunerCfg->tunerData.FifN / tunerCfg->tunerData.FifD;
	if ((tunerCfg->FifHz - N * tunerCfg->tunerData.FifD / tunerCfg->tunerData.FifN) > ((N+1) * tunerCfg->tunerData.FifD / tunerCfg->tunerData.FifN - tunerCfg->FifHz))
		N += 1;

	buff[0] = (U8) N;
	buff[1] = (U8) (N>>8);
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_ifNlo_reg, buff, 2, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
	  return retVal;

	tunerCfg->tunerData.FifHz = N * tunerCfg->tunerData.FifD / tunerCfg->tunerData.FifN;
	tunerCfg->FifErrHz = tunerCfg->tunerData.FifHz - FifHz;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetTfBand()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetTfBand(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8 tfDeQlb;
	U8 tfBand;

	/* @doc _mt_fe_tn_tc2000_freqSetTfBand() 2wb optimization. Write sync required after calling */

	/* @doc If freq < freqLbMax then tfDeQlb = off and tfBand = LB. */
	if (tunerCfg->tunerData.FchanHz <= tunerCfg->tunerData.freqLbMax) {
		tfDeQlb = TC2K_tfDeQlb_off;
		tfBand = TC2K_tfBand_LB;
	}
	/* @doc ElseIf freq < freqMbMax then tfDeQlb = on and tfBand = MB. */
	else if (tunerCfg->tunerData.FchanHz <= tunerCfg->tunerData.freqMbMax) {
		tfDeQlb = TC2K_tfDeQlb_on;
		tfBand = TC2K_tfBand_MB;
	}
	/* @doc Else (freq > freqMbMax) tfDeQlb = on and tfBand = HB. */
	else {
		tfDeQlb = TC2K_tfDeQlb_on;
		tfBand = TC2K_tfBand_HB;
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfDeQlb_reg, TC2K_tfDeQlb_mask, tfDeQlb, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, tfBand, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqCfgTtGen()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param [in] LoDiv
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_freqCfgTtGen(tc2000_tn_config *tunerCfg, U8 LoDiv)
{
	TC2K_RET retVal;
	U8 ttFilter;

	/* @doc _mt_fe_tn_tc2000_freqCfgTtGen() 2wb optimization. Syncs before exiting */

	if (LoDiv > 8)
		/* @doc If LoDiv > 8 then. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttGenDiv_reg, TC2K_ttGenDiv_mask, TC2K_ttGenDiv_off, TC2K_2wbSync_no);
	else
		/* @doc Else (LoDiv <= 8). */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttGenDiv_reg, TC2K_ttGenDiv_mask, TC2K_ttGenDiv_on, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set tt filter based on LO frequency. */
	if (tunerCfg->tunerData.FloHz < 112500000)
		ttFilter = 0x57;
	else if (tunerCfg->tunerData.FloHz < 225000000)
		ttFilter = 0x56;
	else if (tunerCfg->tunerData.FloHz < 337500000)
		ttFilter = 0x55;
	else if (tunerCfg->tunerData.FloHz < 450000000)
		ttFilter = 0x54;
	else if (tunerCfg->tunerData.FloHz < 562500000)
		ttFilter = 0x53;
	else if (tunerCfg->tunerData.FloHz < 675000000)
		ttFilter = 0x52;
	else if (tunerCfg->tunerData.FloHz < 787500000)
		ttFilter = 0x51;
	else
		ttFilter = 0x50;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttFilter_reg, TC2K_ttFilter_mask, ttFilter, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_freqSetLc()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note PROBLEM!!! This routine only works with fxtal = 24MHz.
 */
TC2K_RET _mt_fe_tn_tc2000_freqSetLc(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	S32 FlcHz;
	S16 iLcc;
	S16 iLcs;
	S32 lcc;
	S32 lcs;
	U8  buff[3];
	U8  lcCtrl;

	/* @doc _mt_fe_tn_tc2000_freqSetLc() 2wb optimization. Write sync before exiting */

	/* @doc Calculate xtal spur at bb. */
	FlcHz = (tunerCfg->tunerData.FloHz + 4000000) / tunerCfg->tunerData.FxtalHz;
	FlcHz *= tunerCfg->tunerData.FxtalHz;
	FlcHz = tunerCfg->tunerData.FloHz - FlcHz;
	if (tunerCfg->specInvSel == TC2K_specInvSel_off)
		FlcHz *= -1;

	if ((FlcHz > -4000001) && (FlcHz < 4000001)) {
		/* @doc If abs(Fspur) < 4MHz then set lc freq. */
		if (FlcHz < 0)
			iLcc = -FlcHz / 31250;
		else
			iLcc = FlcHz / 31250;
		if (iLcc > 96)
			iLcc = 192 - iLcc;
		iLcs = 96 - iLcc;
		if ((iLcc < 0) || (iLcc > 96) || (iLcs < 0) || (iLcs > 96))
			return TC2K_err_unknown;
		lcc = TC2K_freqLcLut[iLcc];
		lcs = TC2K_freqLcLut[iLcs];
		if ((FlcHz > 3000000) || (FlcHz < -3000000))
			lcc = 0x400000 - lcc;
		else if (iLcc == 0)
			lcc -= 1;
		if (FlcHz < 0)
			lcs = 0x400000 - lcs;
		else if (iLcs == 0)
			lcs -= 1;
		lcCtrl = TC2K_lcCtrl_on;
	}
	else {
		/* @doc Else (abs(Fspur) > 4MHz) disable lc. */
		lcc = 0;
		lcs = 0;
		lcCtrl = TC2K_lcCtrl_off;
	}

	/* @doc Set lc registers. */
	buff[0] = (U8) lcc;
	buff[1] = (U8) (lcc>>8);
	buff[2] = (U8) (lcc>>16);
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_lccLo_reg, buff, 3, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	buff[0] = (U8) lcs;
	buff[1] = (U8) (lcs>>8);
	buff[2] = (U8) (lcs>>16);
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_lcsLo_reg, buff, 3, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_lcCtrl_reg, TC2K_lcCtrl_mask, lcCtrl, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start TF Control Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

S32 tc2000_tfFreqLb[TC2K_tfcLb_Cnt] = {57000000, 153000000};
U8 tfCw0Lb[TC2K_tfcLb_Cnt] = {48, 7};
S32 tc2000_tfgCompLb[2] = {1365, 1365};

S32 tc2000_tfFreqMb[TC2K_tfcMb_Cnt] = {207000000, 411000000};
U8 tc2000_tfCw0Mb[TC2K_tfcMb_Cnt] = {45, 8};
S32 tc2000_tfgCompMb[2] = {1685, 1685};

S32 tc2000_tfFreqHb[TC2K_tfcHb_Cnt] = {489000000, 771000000};
U8 tc2000_tfCw0Hb[TC2K_tfcHb_Cnt] = {25, 6};
S32 tc2000_tfgCompHb[2] = {1885, 2600};

S32 tc2000_tfgCompFreqHb[TC2K_tfgCompCntHb] = {475000000, 550000000, 650000000, 700000000, 775000000, 875000000, 1000000000};
S32 tc2000_tfgCompFreqMb[TC2K_tfgCompCntMb] = {225000000, 375000000, 1000000000};
S32 tc2000_tfgCompFreqLb[TC2K_tfgCompCntLb] = {125000000, 150000000, 1000000000};

/* @doc First entry always 0. tc2000_tfgComp?b[] sets level of first cal point */
S16 tc2000_tfgCompCwHb[TC2K_tfgCompCntHb] = {0, 1, 2, 3, 4, 5, 6};
S16 tc2000_tfgCompCwMb[TC2K_tfgCompCntMb] = {0, 0, 0};
S16 tc2000_tfgCompCwLb[TC2K_tfgCompCntLb]= {0, -1, -2};

/*
 * _mt_fe_tn_tc2000_tfCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfCal(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_tfCal() 2wb optimization. No external r/w sync before calling. */

	retVal = _mt_fe_tn_tc2000_cfgTfCal (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_tfSetTtLut (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Calibrate low band. */
	retVal = _mt_fe_tn_tc2000_tfCalOneBand(tunerCfg, TC2K_tfBand_LB, TC2K_tfcCapWdMax_Max63, TC2K_tfcLb_Cnt, tfCw0Lb, tc2000_tfFreqLb, tunerCfg->tunerData.tffCwLb, tunerCfg->tunerData.tfgCwLb);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Calibrate mid band. */
	retVal = _mt_fe_tn_tc2000_tfCalOneBand(tunerCfg, TC2K_tfBand_MB, TC2K_tfcCapWdMax_Max63, TC2K_tfcMb_Cnt, tc2000_tfCw0Mb, tc2000_tfFreqMb, tunerCfg->tunerData.tffCwMb, tunerCfg->tunerData.tfgCwMb);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Calibrate high band. */
	retVal = _mt_fe_tn_tc2000_tfCalOneBand(tunerCfg, TC2K_tfBand_HB, TC2K_tfcCapWdMax_Max31, TC2K_tfcHb_Cnt, tc2000_tfCw0Hb, tc2000_tfFreqHb, tunerCfg->tunerData.tffCwHb, tunerCfg->tunerData.tfgCwHb);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc tfCalDebug. New tfcal subroutines */
	tunerCfg->tunerData.tffcalCw[0] = tunerCfg->tunerData.tffCwLb[0];
	tunerCfg->tunerData.tffcalCw[1] = tunerCfg->tunerData.tffCwMb[0];
	tunerCfg->tunerData.tffcalCw[2] = tunerCfg->tunerData.tffCwHb[0];
	retVal = _mt_fe_tn_tc2000_tfcFreqGetSlope(tunerCfg->tunerData.tffCwLb[0], tunerCfg->tunerData.tffCwLb[TC2K_tfcLb_Cnt-1], tc2000_tfFreqLb[0], tc2000_tfFreqLb[TC2K_tfcLb_Cnt-1], 63, &tunerCfg->tunerData.tffcalSlope[0]);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_tfcFreqGetSlope(tunerCfg->tunerData.tffCwMb[0], tunerCfg->tunerData.tffCwMb[TC2K_tfcMb_Cnt-1], tc2000_tfFreqMb[0], tc2000_tfFreqMb[TC2K_tfcMb_Cnt-1], 63, &tunerCfg->tunerData.tffcalSlope[1]);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_tfcFreqGetSlope(tunerCfg->tunerData.tffCwHb[0], tunerCfg->tunerData.tffCwHb[TC2K_tfcLb_Cnt-1], tc2000_tfFreqHb[0], tc2000_tfFreqHb[TC2K_tfcHb_Cnt-1], 31, &tunerCfg->tunerData.tffcalSlope[2]);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_tfcFreqSetBandTrans(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfSetTtLut()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] tfBand
 * @param[in] tfcCapWdMax
 * @param[in] tfFreqCnt
 * @param[in] tfCw0
 * @param[in] tfFreq
 * @param[out] ttPwrLut
 * @param[in] tffCw
 * @param[out] tfgCw
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfSetTtLut(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8  ttPwr;
	U8  buff[2];
	U8  i;

	/* @doc Initialize ttCalTtPwrLut's. Loop thru each ttPwr. Use for setting ttPwr in presence of jammer. */
	tunerCfg->tunerData.tfcTtLut[0] = 4095;
	for (i=1;i<TC2K_tfcTtLut_Cnt;i++) {
		switch(i) {
			case 1:
				ttPwr = TC2K_ttPwr_LnaNeg74MixNeg62dBV;
				break;
			case 2:
				ttPwr = TC2K_ttPwr_LnaNeg68MixNeg56dBV;
				break;
			case 3:
				ttPwr = TC2K_ttPwr_LnaNeg62MixNeg50dBV;
				break;
			case 4:
				ttPwr = TC2K_ttPwr_LnaNeg56MixNeg44dBV;
				break;
			case 5:
				ttPwr = TC2K_ttPwr_LnaNeg50MixNeg38dBV;
				break;
			case 6:
				ttPwr = TC2K_ttPwr_LnaNeg44MixNeg32dBV;
				break;
			default:
				return TC2K_err_unknown; /* will never happen */
		}

		/* @doc Set ttPwr (in for loop). */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttPwr_reg, TC2K_ttPwr_mask, ttPwr, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc Set if gain to max (loop settles faster). */
		buff[0] = 255;
		buff[1] = 15;
		retVal = _mt_fe_tn_tc2000_i2c_write(tunerCfg, TC2K_agcAccWrLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc Wait for if agc to settle. */
		retVal = _mt_fe_tn_tc2000_cfgBbAgcDelay(tunerCfg, 10);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc read if accum. */
		retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc Store if accum for current ttPwr (in tuner data structure). */
		tunerCfg->tunerData.tfcTtLut[i] = (((S16) buff[1])<<8) + (S16) buff[0];
	}
	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfCalOneBand()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] tfBand
 * @param[in] tfcCapWdMax
 * @param[in] tfFreqCnt
 * @param[in] tfCw0
 * @param[in] tfFreq
 * @param[out] ttPwrLut
 * @param[in] tffCw
 * @param[out] tfgCw
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfCalOneBand(tc2000_tn_config *tunerCfg, U8 tfBand, U8 tfcCapWdMax, U8 tfFreqCnt, U8 tfCw0[], S32 tfFreq[], U8 tffCw[], U8 tfgCw[])
{
	TC2K_RET retVal;
	U8 ttPwr;
	S32 VrefComp;
	U8 i;

	/* @doc _mt_fe_tn_tc2000_tfCalOneBand() 2wb optimization. No external r/w sync before calling. */

	/* @doc Set band specific registers. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, tfBand, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcCapWdMax_reg, TC2K_tfcCapWdMax_mask, tfcCapWdMax, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Loop through all calibration frequencies. */
	for (i = 0; i < tfFreqCnt; i++) {
		/* @doc Set tfCw0 for current cal freq. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcCapWd0_reg, TC2K_tfcCapWd0_mask, tfCw0[i], TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc Must select CapWd0 for setting the test tone power. */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcCapCtrl_reg, TC2K_tfcCapCtrl_mask, TC2K_tfcCapCtrl_B2WWrite, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc Set freq to current cal freq. */
		retVal = _mt_fe_tn_tc2000_freqSetFloHz(tunerCfg, tfFreq[i]);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc Set ttPwr (checks for strong jammers). */
		retVal = _mt_fe_tn_tc2000_tfSetTtPwr (tunerCfg, &ttPwr);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc Store ttPwr for current cal freq in tuner data structure (for debug). */
		switch (tfBand) {
			case TC2K_tfBand_LB:
				tunerCfg->tunerData.tfcTtPwrLbDebug[i] = ttPwr;
				VrefComp = tc2000_tfgCompLb[i];
				break;
			case TC2K_tfBand_MB:
				tunerCfg->tunerData.tfcTtPwrMbDebug[i] = ttPwr;
				VrefComp = tc2000_tfgCompMb[i];
				break;
			case TC2K_tfBand_HB:
				tunerCfg->tunerData.tfcTtPwrHbDebug[i] = ttPwr;
				VrefComp = tc2000_tfgCompHb[i];
				break;
			default:
				return TC2K_err_unknown; /* will never happen */
		}
		/* @doc Reseting tfBand after tfSetTtPwr */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, tfBand, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcCapCtrl_reg, TC2K_tfcCapCtrl_mask, TC2K_tfcCapCtrl_SMWrite, TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc _mt_fe_tn_tc2000_ifTrigSM() 2wb optimization. Read registers up to date on exit. */
		retVal = _mt_fe_tn_tc2000_tfcTrig (tunerCfg);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc Store cw for current cal freq in tuner data structure. */
		retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfcCapWdRd_reg, TC2K_tfcCapWdRd_mask, &tffCw[i], TC2K_2wbSync_no);
		if (retVal != TC2K_ok)
			return retVal;

		/* @doc Run gain cal and store cw for current cal freq in tuner data structure. */
		/* @doc Base gain cal on single point */
		//if (i == 0) {
			retVal = _mt_fe_tn_tc2000_tfGainCal(tunerCfg, tfBand, VrefComp, &tfgCw[i]);
			if (retVal != TC2K_ok)
				return retVal;
		//}

	}
	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfGainCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] tfBand
 * @param[out] tfgCw
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfGainCal(tc2000_tn_config *tunerCfg, U8 tfBand, S32 VrefComp, U8 *tfgCw)
{
	TC2K_RET retVal;

	S32 tfgVref;
	S32 tfgVtarget;
	S32 tfgVdet;
	S32 tfgVchk;
	U8  tfAttMin;
	U8  tfAtt = 0;
	U8  tfAttMax;
	U8  tfAttChk;

	/* @doc _mt_fe_tn_tc2000_freqSetFloHz() 2wb optimization. No external r/w sync required before or after. */

	/* @doc Set tt input to mixer and get reference level. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, TC2K_tfBand_TT, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttlnjectPt_reg, TC2K_ttlnjectPt_mask, TC2K_ttlnjectPt_Mix, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	_mt_sleep (10);
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetCtrl_reg, TC2K_calDetCtrl_mask, TC2K_calDetCtrl_B2W, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &tfgVref);
	if (retVal != TC2K_ok)
		return retVal;

	tfgVtarget = tfgVref * VrefComp;
	tfgVtarget /= 1000;

	/* @doc Set tt input to lna. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, tfBand, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttlnjectPt_reg, TC2K_ttlnjectPt_mask, TC2K_ttlnjectPt_Lna, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Use binary search to find closest gain cw. */
	tfAttMin = 0;
	tfAttMax = 15;
	while (tfAttMin + 1 < tfAttMax) {
		/* @doc In binary search loop. Set gain cw to current value. */
		tfAtt = (tfAttMin + tfAttMax) / 2;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, (U8) (tfAtt<<3), TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		_mt_sleep(5);
		/* @doc In binary search loop. Read detector level. */
		retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &tfgVdet);
		if (retVal != TC2K_ok)
			return retVal;
		/* @doc In binary search loop. Adjust gain cap wd based on detector output. */
		if (tfgVdet < tfgVtarget)
			tfAttMax = tfAtt;
		else
			tfAttMin = tfAtt;
	}

	/* loop exits wih tfAttMin = tfAtt or tfAttMax = tfAtt.	 */
	/* find closest tfAtt or one not equal to tfAtt			 */
	/* tfgVdet -> tfAtt										 */
	/* @doc Exited binary search loop. Calculate and return closest of two posible values. */
	if (tfAttMin == tfAtt)
		tfAttChk = tfAttMax;
	else
		tfAttChk = tfAttMin;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, (U8) (tfAttChk<<3), TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	_mt_sleep(5);
	retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &tfgVchk);
	if (retVal != TC2K_ok)
		return retVal;
	tfgVdet -= tfgVtarget;
	if (tfgVdet < 0)
		tfgVdet *= -1;
	tfgVchk -= tfgVtarget;
	if (tfgVchk < 0)
		tfgVchk *= -1;
	if (tfgVdet < tfgVchk)
		*tfgCw = tfAtt;
	else
		*tfgCw = tfAttChk;

	/* @doc Restore calDet control to sm. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetCtrl_reg, TC2K_calDetCtrl_mask, TC2K_calDetCtrl_SM, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfSetTtPwr()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] ttPwrLut
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfSetTtPwr(tc2000_tn_config *tunerCfg, U8 *ttPwr)
{
	TC2K_RET retVal;

	U8  buff[2];
	S32 ifAcc;
	U8  i;

	/* @doc _mt_fe_tn_tc2000_tfSetTtPwr() 2wb optimization. No external sync required before or after. */

	/* @doc Set RF gain to max. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, 0, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc Turn off test tone and set tt input to lna in preparation to read jammer pwr (if present). */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttEna_reg, TC2K_ttEna_mask, TC2K_ttEna_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttlnjectPt_reg, TC2K_ttlnjectPt_mask, TC2K_ttlnjectPt_Lna, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set if gain to max. */
	buff[0] = 255;
	buff[1] = 15;
	retVal = _mt_fe_tn_tc2000_i2c_write(tunerCfg, TC2K_agcAccWrLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Turn on if agc and wait for it to settle. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	/* @doc _mt_fe_tn_tc2000_cfgBbAgcDelay() 2wb optimization. Read registers up to date on exit. */
	retVal = _mt_fe_tn_tc2000_cfgBbAgcDelay(tunerCfg, 10);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Read the if agc accumulator. */
	retVal = _mt_fe_tn_tc2000_i2c_read(tunerCfg, TC2K_agcAccRdLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	ifAcc = (((S32) buff[1]) << 8) + (S32) buff[0];

	/* @doc Find the lowest tt pwr that is reater than the jammer pwr by searching the tfcTtLut. */
	for (i = 0; i<TC2K_tfcTtLut_Cnt; i++) {
		if (ifAcc >= tunerCfg->tunerData.tfcTtLut[i])
			break;
	}
	if (i == TC2K_tfcTtLut_Cnt)
		--i;
	switch(i) {
		case 0:
			*ttPwr = TC2K_ttPwr_LnaNeg80MixNeg68dBV;
			break;
		case 1:
			*ttPwr = TC2K_ttPwr_LnaNeg74MixNeg62dBV;
			break;
		case 2:
			*ttPwr = TC2K_ttPwr_LnaNeg68MixNeg56dBV;
			break;
		case 3:
			*ttPwr = TC2K_ttPwr_LnaNeg62MixNeg50dBV;
			break;
		case 4:
			*ttPwr = TC2K_ttPwr_LnaNeg56MixNeg44dBV;
			break;
		case 5:
			*ttPwr = TC2K_ttPwr_LnaNeg50MixNeg38dBV;
			break;
		case 6:
			*ttPwr = TC2K_ttPwr_LnaNeg44MixNeg32dBV;
			break;
		default:
			return TC2K_err_unknown; /* will never happen */
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttPwr_reg, TC2K_ttPwr_mask, *ttPwr, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Adjust the if gain by setting the tt injection point to the mixer and running the agc. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, TC2K_tfBand_TT, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttlnjectPt_reg, TC2K_ttlnjectPt_mask, TC2K_ttlnjectPt_Mix, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttEna_reg, TC2K_ttEna_mask, TC2K_ttEna_on, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_cfgBbAgcDelay(tunerCfg, 10);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Freeze the if agc. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_off, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set the tt injection point to the lna input. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttlnjectPt_reg, TC2K_ttlnjectPt_mask, TC2K_ttlnjectPt_Lna, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set RF gain to min. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, 7<<3, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfcTrig()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_tfcSm_timeout
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfcTrig(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	U8 tfcErr;
	U8 tfcDone;
	U8 i;

	/* @doc _mt_fe_tn_tc2000_tfcTrig() 2wb optimization. May require external w sync before calling. */
	/* @doc _mt_fe_tn_tc2000_cfgCalDetRead() 2wb optimization. Read registers up to date on exit. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcTrig_reg, TC2K_tfcTrig_mask, TC2K_tfcTrig_Trig, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcTrig_reg, TC2K_tfcTrig_mask, TC2K_tfcTrig_ClearTrig, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Pole the sm until tfcDone is true or time out is reached. */
	for (i=0;i<100;i++) {
		retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfcDone_reg, TC2K_tfcDone_mask, &tfcDone, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		if (tfcDone == TC2K_tfcDone_done)
			break;
		_mt_sleep(1);
	}

	/* @doc Exit with error if sm timed out. */
	if (tfcDone != TC2K_tfcDone_done)
		return TC2K_err_tfcSm_timeout;

	/* @doc Exit with error if sm returned an error. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfcErr_reg, TC2K_tfcErr_mask, &tfcErr, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	if (tfcErr == TC2K_tfcErr_err)
		return TC2K_err_tfcSm_timeout;

	return TC2K_ok;
}


TC2K_RET _mt_fe_tn_tc2000_tfcFreqGetSlope(U8 c1, U8 c2, S32 f1, S32 f2, U8 CWmax, S32 *slope)
{
	S16 i;
	S32 f;
	S32 ferr;
	S32 ferrChk = 0;
	S32 slopeChk = 0;

	if ((c1 < c2) || (f1 > f2))
		return TC2K_err_tfCal_fail;

	*slope = 1025;
	f1 /= 1000;
	f2 /= 1000;
	f = f1;
	for (i=c2;i<=c1;++i) {
		f *= *slope;
		f /=1000;
	}

	if (f == f2)
		return TC2K_ok;
	else if (f > f2) {
		while (f > f2) {
			ferrChk = f - f2;
			slopeChk = *slope;
			--*slope;
			f = f1;
			for (i=c2;i<=c1;++i) {
				f *= *slope;
				f /=1000;
			}
		}
		ferr = f2 - f;
	}
	else {
		while (f < f2) {
			ferrChk = f2 - f;
			slopeChk = *slope;
			++*slope;
			f = f1;
			for (i=c2;i<=c1;++i) {
				f *= *slope;
				f /=1000;
			}
		}
		ferr = f - f2;
	}

	if (ferrChk < ferr)
		*slope = slopeChk;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_tfcFreqSetBandTrans(tc2000_tn_config *tunerCfg)
{
	S16 i;
	S32 fmax;
	S32 fmin;
	U8	cw;
	S32 slope;

	fmax = tc2000_tfFreqLb[0] / 1000;
	cw = tunerCfg->tunerData.tffcalCw[0];
	slope = tunerCfg->tunerData.tffcalSlope[0];
	for (i=cw-1;i>=0;--i) {
		fmax *= slope;
		fmax /= 1000;
	}

	fmin = tc2000_tfFreqMb[0] / 1000;
	cw = tunerCfg->tunerData.tffcalCw[1];
	slope = tunerCfg->tunerData.tffcalSlope[1];
	for (i=cw+1;i<=63;++i) {
		fmin *= 1000;
		fmin /= slope;
	}

	if (fmin > fmax)
		return TC2K_err_tfCal_fail;

	tunerCfg->tunerData.freqLbMax = (fmax + fmin)/2;
	tunerCfg->tunerData.freqLbMax *= 1000;

	fmax = tc2000_tfFreqMb[0] / 1000;
	cw = tunerCfg->tunerData.tffcalCw[1];
	slope = tunerCfg->tunerData.tffcalSlope[1];
	for (i=cw-1;i>=0;--i) {
		fmax *= slope;
		fmax /= 1000;
	}

	fmin = tc2000_tfFreqHb[0] / 1000;
	cw = tunerCfg->tunerData.tffcalCw[2];
	slope = tunerCfg->tunerData.tffcalSlope[2];
	for (i=cw+1;i<=31;++i) {
		fmin *= 1000;
		fmin /= slope;
	}

	if (fmin > fmax)
		return TC2K_err_tfCal_fail;
	tunerCfg->tunerData.freqMbMax = (fmax + fmin)/2;
	tunerCfg->tunerData.freqMbMax *= 1000;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_tfChanChg()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note None.
 */
TC2K_RET _mt_fe_tn_tc2000_tfChanChg(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_tfcTrig() 2wb optimization. No direct 2wb reads/writes. */

	retVal =  _mt_fe_tn_tc2000_tfChanChgFreq(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal =  _mt_fe_tn_tc2000_tfChanChgGain(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	return retVal;
}

TC2K_RET _mt_fe_tn_tc2000_tfChanChgFreq(tc2000_tn_config *tunerCfg)
{
	TC2K_RET	retVal;
	S32	fch_kHz;
	S32	fcal_kHz;
	U8	calCw;
	S32	calSlope;
	U8	CWmax;
	U8	cw;
	S32	f;
	S32	ferr;
	S32	ferrChk = 0;
	U8	cwChk = 0;
	U8	lnaBand;

	fch_kHz = tunerCfg->tunerData.FchanHz / 1000;

	if (tunerCfg->tunerData.FchanHz > tunerCfg->tunerData.freqMbMax) {
		fcal_kHz = tc2000_tfFreqHb[0] / 1000;
		calCw = tunerCfg->tunerData.tffcalCw[2];
		calSlope = tunerCfg->tunerData.tffcalSlope[2];
		CWmax = 31;
	}
	else if (tunerCfg->tunerData.FchanHz > tunerCfg->tunerData.freqLbMax) {
		fcal_kHz = tc2000_tfFreqMb[0] / 1000;
		calCw = tunerCfg->tunerData.tffcalCw[1];
		calSlope = tunerCfg->tunerData.tffcalSlope[1];
		CWmax = 63;
	}
	else {
		fcal_kHz = tc2000_tfFreqLb[0] / 1000;
		calCw = tunerCfg->tunerData.tffcalCw[0];
		calSlope = tunerCfg->tunerData.tffcalSlope[0];
		CWmax = 63;
	}

	cw = calCw;
	if (fch_kHz != fcal_kHz) {
		f = fcal_kHz;

		if (fch_kHz == fcal_kHz)
			return TC2K_ok;
		else if (fch_kHz > fcal_kHz) {
			while ((f < fch_kHz) & (cw > 0)) {
				cwChk = cw;
				ferrChk = f - fch_kHz;
				--cw;
				f *= calSlope;
				f /= 1000;
			}
			ferr = f - fch_kHz;
		}
		else {
			while ((f > fch_kHz) * (cw < CWmax)) {
				cwChk = cw;
				ferrChk = fch_kHz - f;
				++cw;
				f *= 1000;
				f /= calSlope;
			}
			ferr = fch_kHz - f;
		}

		if (ferrChk < ferr)
			cw = cwChk;

		if (tunerCfg->broadcastMedium == TC2K_broadcastMedium_ota) {
			retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_tfBand_reg, TC2K_tfBand_mask, &lnaBand, TC2K_2wbSync_yes);
			if (retVal != TC2K_ok)
				return retVal;
			if (lnaBand == TC2K_tfBand_MB) {
				switch (tunerCfg->modulation) {
					case TC2K_modulation_PAL_B:
					case TC2K_modulation_PAL_D_I_K:
					case TC2K_modulation_PAL_G_H:
					case TC2K_modulation_NTSC:
					case TC2K_modulation_SECAM:
					case TC2K_modulation_SECAM_L:
						cw += 2;
						if (cw > 63)
							cw = 63;
						break;
					case TC2K_modulation_DVB_C:
					case TC2K_modulation_J83_Annex_B:
					case TC2K_modulation_DVB_T:
					case TC2K_modulation_ISDB_T:
					case TC2K_modulation_ATSC:
					case TC2K_modulation_DMB_T:
						break;
				}
			}
		}
	}

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfcCapWd0_reg, TC2K_tfcCapWd0_mask, cw, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

TC2K_RET _mt_fe_tn_tc2000_tfChanChgGain(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	S32 *freqAry;
	S16 *cwAry;
	S16 aryCnt;
	S16 i;
	S16 cwSigned;

	if (tunerCfg->tunerData.FchanHz > tunerCfg->tunerData.freqMbMax) {
		freqAry = tc2000_tfgCompFreqHb;
		cwAry = tc2000_tfgCompCwHb;
		aryCnt = TC2K_tfgCompCntHb;
		/* @doc First freq in HB cal is too high subtract 1 from tfgCapw[0] */
		cwSigned = (S16) tunerCfg->tunerData.tfgCwHb[0] - 1;
	}
	else if (tunerCfg->tunerData.FchanHz > tunerCfg->tunerData.freqLbMax) {
		freqAry = tc2000_tfgCompFreqMb;
		cwAry = tc2000_tfgCompCwMb;
		aryCnt = TC2K_tfgCompCntMb;
		cwSigned = (S16) tunerCfg->tunerData.tfgCwMb[0];
	}
	else {
		freqAry = tc2000_tfgCompFreqLb;
		cwAry = tc2000_tfgCompCwLb;
		aryCnt = TC2K_tfgCompCntLb;
		cwSigned = (S16) tunerCfg->tunerData.tfgCwLb[0];
	}

	for (i=0;i<aryCnt;++i) {
		if (tunerCfg->tunerData.FchanHz < freqAry[i])
			break;
	}

	if (i == aryCnt)
		--i; /* Slould never happen */

	cwSigned += cwAry[i];

	/* @doc Compensate for lna gain versus temperature. Compensate by ~ 0.7 dB/25 deg. */
	/* @doc Increase gain by decrementing tfAtt if curTmpr > calTmpr. */
	/* @doc Decrease gain by incrementing tfAtt if curTmpr < calTmpr. */
	/* @doc Round up or down at 12.5 deg. */
	retVal = _mt_fe_tn_tc2000_cfgReadTmprAcc (tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;
	if (tunerCfg->tunerData.curTmpr < tunerCfg->tunerData.calTmpr)
		cwSigned -= (tunerCfg->tunerData.curTmpr - tunerCfg->tunerData.calTmpr - TC2K_tfTempAccDelta25Deg / 2) / TC2K_tfTempAccDelta25Deg;
	else
		cwSigned -= (tunerCfg->tunerData.curTmpr - tunerCfg->tunerData.calTmpr + TC2K_tfTempAccDelta25Deg / 2) / TC2K_tfTempAccDelta25Deg;

	/* @doc Make sure cwSigned in range. */
	if (cwSigned < 0)
		cwSigned = 0;
	else if (cwSigned > 15)
		cwSigned = 15;

	/* @doc Add temp comp here */

	switch (cwSigned) {
		case 0:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_0, TC2K_2wbSync_yes);
			break;
		case 1:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_1, TC2K_2wbSync_yes);
			break;
		case 2:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_2, TC2K_2wbSync_yes);
			break;
		case 3:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_3, TC2K_2wbSync_yes);
			break;
		case 4:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_4, TC2K_2wbSync_yes);
			break;
		case 5:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_5, TC2K_2wbSync_yes);
			break;
		case 6:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_6, TC2K_2wbSync_yes);
			break;
		case 7:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_7, TC2K_2wbSync_yes);
			break;
		case 8:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_8, TC2K_2wbSync_yes);
			break;
		case 9:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_9, TC2K_2wbSync_yes);
			break;
		case 10:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_10, TC2K_2wbSync_yes);
			break;
		case 11:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_11, TC2K_2wbSync_yes);
			break;
		case 12:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_12, TC2K_2wbSync_yes);
			break;
		case 13:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_13, TC2K_2wbSync_yes);
			break;
		case 14:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_14, TC2K_2wbSync_yes);
			break;
		case 15:
			retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_tfAttCw_reg, TC2K_tfAttCw_mask, TC2K_tfAttCw_AttCtl_15, TC2K_2wbSync_yes);
			break;
		default:
			return TC2K_err_unknown;
	}
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*******************************************************************************
********************************************************************************
********************************************************************************
*
* Start IF Calibration Subroutines
*
********************************************************************************
********************************************************************************
*******************************************************************************/

/*************************************************
* indexed by TC2K_ifCalBw enum:
* TC2K_ifCalBw_6MHz 0
* TC2K_ifCalBw_7MHz 1
* TC2K_ifCalBw_8MHz 2
* TC2K_ifCalBw_10MHz 3
*
* must be made multi-dimensional when new Fxtal
* frequencies are added.
*************************************************/
static const U8 TC2K_NifHi[TC2K_ifCalBw_Cnt] = {5, 6, 7, 9};
static const U8 TC2K_NifLo[TC2K_ifCalBw_Cnt] = {155, 138, 121, 87};
static const U8 TC2K_NspinHi[TC2K_ifCalBw_Cnt] = {5, 6, 7, 6};
static const U8 TC2K_NspinMid[TC2K_ifCalBw_Cnt] = {155, 138, 121, 169};
static const U8 TC2K_NspinLo[TC2K_ifCalBw_Cnt] = {0, 0, 0, 0};

/* Indexed by TC2K_ifAccTop_Vga2_on_26dB 0 and TC2K_ifAccTop_Vga2_off_12dB 1 */
/* Used with TC2K_ifAccTopRefD to set ifAccTop */
static const S32 TC2K_ifAccTopRefN[TC2K_ifAccTop_Cnt] = {51, 257};

/**
 * _mt_fe_tn_tc2000_ifCalBW()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called (indiectly) in mt_fe_tn_tc2000_active().
 */
TC2K_RET _mt_fe_tn_tc2000_ifCalBW(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	S32 ifCalVref;

	/* @doc _mt_fe_tn_tc2000_ifCalBW() 2wb optimization. No external r/w sync required before calling. */

	/* @doc Load calibration configuration. */
	retVal = _mt_fe_tn_tc2000_cfgIfBwCal(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcEna_reg, TC2K_ifAgcEna_mask, TC2K_ifAgcEna_off, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Get reference level. */
	retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &ifCalVref);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Give SM's control. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetCtrl_reg, TC2K_calDetCtrl_mask, TC2K_calDetCtrl_SM, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCapWdCtrl_reg, TC2K_ifCapWdCtrl_mask, TC2K_ifCapWdCtrl_SM, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Cal 6 MHz */
	retVal = _mt_fe_tn_tc2000_ifCalBWSngleFreq(tunerCfg, ifCalVref, TC2K_ifCalD_6MHz, TC2K_ifCalBw_6MHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Cal 7 MHz */
	retVal = _mt_fe_tn_tc2000_ifCalBWSngleFreq(tunerCfg, ifCalVref, TC2K_ifCalD_7MHz, TC2K_ifCalBw_7MHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Cal 8 MHz */
	retVal = _mt_fe_tn_tc2000_ifCalBWSngleFreq(tunerCfg, ifCalVref, TC2K_ifCalD_8MHz, TC2K_ifCalBw_8MHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Cal 10 MHz */
	retVal = _mt_fe_tn_tc2000_ifCalBWSngleFreq(tunerCfg, ifCalVref, TC2K_ifCalD_10MHz, TC2K_ifCalBw_10MHz);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Restore control to b2w. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_calDetCtrl_reg, TC2K_calDetCtrl_mask, TC2K_calDetCtrl_B2W, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCapWdCtrl_reg, TC2K_ifCapWdCtrl_mask, TC2K_ifCapWdCtrl_B2W, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/**
 * _mt_fe_tn_tc2000_ifCalBWSngleFreq()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called by _mt_fe_tn_tc2000_ifCalBW only.
 */
TC2K_RET _mt_fe_tn_tc2000_ifCalBWSngleFreq(tc2000_tn_config *tunerCfg, S32 ifCalVref, S32 ifCalD, U8 ifBw)
{
	TC2K_RET retVal;
	U8 cw;

	/* @doc _mt_fe_tn_tc2000_ifCalBW() 2wb optimization. No external r/w sync required before calling. */

	retVal = _mt_fe_tn_tc2000_ifSetTtFreq (tunerCfg, ifBw);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCalVrefLo_reg , TC2K_ifCalVrefLo_mask, ((U8)((ifCalVref * TC2K_ifCalN / ifCalD))), TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCalVrefHi_reg , TC2K_ifCalVrefHi_mask, ((U8)((ifCalVref * TC2K_ifCalN / ifCalD) >> 8)), TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_ifTrigSM(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifCapWdRd_reg, TC2K_ifCapWdRd_mask, &cw, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Store CW (in tuner data structure). */
	tunerCfg->tunerData.ifBwWd[ifBw] = cw;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_ifTrigSM()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_ifcsm_timeout
 * 			TC2K_err_rw otherwise.
 *
 * @note Called by _mt_fe_tn_tc2000_ifCalBW only.
 */
TC2K_RET _mt_fe_tn_tc2000_ifTrigSM(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;
	U8 ifCalDone;
	U8 ifCalErr;
	U8 i;

	/* @doc _mt_fe_tn_tc2000_ifTrigSM() 2wb optimization. Read registers up to date on exit. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCalTrig_reg, TC2K_ifCalTrig_mask, TC2K_ifCalTrig_Trigger, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifCalTrig_reg, TC2K_ifCalTrig_mask, TC2K_ifCalTrig_ClearTrigger, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Pole ifCalDone for 20 msec max until complete or timeout occurs. */
	for (i=0; i<20; i++) {
		retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifCalDone_reg, TC2K_ifCalDone_mask, &ifCalDone, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		if (ifCalDone == TC2K_ifCalDone_done)
			break;
		_mt_sleep(1);
	}

	/* @doc Check if SM timed out or completed. */
	if (ifCalDone != TC2K_ifCalDone_done)
		return TC2K_err_ifcsm_timeout;

	/* @doc Check if SM returned and error. */
	retVal = _mt_fe_tn_tc2000_get_reg (tunerCfg, TC2K_ifCalErr_reg, TC2K_ifCalErr_mask, &ifCalErr, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	if (ifCalErr == TC2K_ifCalErr_Err)
		return TC2K_err_ifcsm_timeout;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_ifSetTtFreq()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] ifBw
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called by _mt_fe_tn_tc2000_ifCalBW only. fttHz may be rounded by setting Nif = ftt * > FifN / FifD,
 * but the test tones will always be equal since Nspin is calculated from Nif
 */
TC2K_RET _mt_fe_tn_tc2000_ifSetTtFreq(tc2000_tn_config *tunerCfg, U8 ifBw)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_ifSetTtFreq() 2wb optimization. Write sync before exiting. */

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifNlo_reg, TC2K_ifNlo_mask, TC2K_NifLo[ifBw], TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifNhi_reg, TC2K_ifNhi_mask, TC2K_NifHi[ifBw], TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rotLo_reg, TC2K_rotLo_mask, TC2K_NspinLo[ifBw], TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rotMid_reg, TC2K_rotMid_mask, TC2K_NspinMid[ifBw], TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_rotHi_reg, TC2K_rotHi_mask, TC2K_NspinHi[ifBw], TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* accuracy is not important here */
	tunerCfg->tunerData.FifHz = 0;
	tunerCfg->tunerData.FspinHz = 0;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_ifAccTopCal()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called (indiectly) in mt_fe_tn_tc2000_active().
 */
TC2K_RET _mt_fe_tn_tc2000_ifAccTopCal(tc2000_tn_config *tunerCfg)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_ifAccTopCal() 2wb optimization. No external r/w sync required before calling. */

	retVal = _mt_fe_tn_tc2000_cfgIfAccTopCal(tunerCfg);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Calibrate ifAccTop, vga2 on, 26 dB backoff. */
	retVal = _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff(tunerCfg, TC2K_ifAccTopIndex_Vga2_on_26dB);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Calibrate ifAccTop, vga2 off, 10 dB backoff. */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifVga2Ena_reg, TC2K_ifVga2Ena_mask, TC2K_ifVga2Ena_off, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff(tunerCfg, TC2K_ifAccTopIndex_Vga2_off_12dB);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 * @param[in] tunerMode
 * @param[in] ifAccTopRefN
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_ifAccTop_setTtPwr
 * 			TC2K_err_rw otherwise.
 *
 * @note Called by _mt_fe_tn_tc2000_ifAccTopCal() only.
 */
TC2K_RET _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff(tc2000_tn_config *tunerCfg, U8 ifAccTopValueIndex)
{
	TC2K_RET retVal;
	U8 ttPwr;
	U8 buff[2];
	S32 ifAccTopVref;
	S32 ifAccTopVtarget;
	S32 ifAccTopVdet;
	S32 ifAccMin;
	S32 ifAcc = 0;
	S32 ifAccMax;

	/* @doc _mt_fe_tn_tc2000_ifAccTopCalSingleBckOff() 2wb optimization. No external r/w sync required before calling. */

	/* @doc Set ttPwr to ttPwr,min. */
	ttPwr = TC2K_ttPwr_LnaNeg80MixNeg68dBV;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttPwr_reg, TC2K_ttPwr_mask, ttPwr, TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Set if gain to max (4095). */
	buff[0] = 255;
	buff[1] = 15;
	retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_agcAccWrLo_reg, buff, 2, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
	if (retVal != TC2K_ok)
		return retVal;

	/* @doc Optimize ttPwr (increase ttPwr, with if gain at max, until detector reads ~-6dBFS). */
	/* CalDetVref @ 6 dB backoff ~2200		 */
	/* ttPwr increases 6 dB/step			 */
	/* increase ttPwr while ifTopVref < 2000 */
	retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &ifAccTopVref);
	if (retVal != TC2K_ok)
		return retVal;
	while (ifAccTopVref < 2000) {
		if (ttPwr == TC2K_ttPwr_LnaNeg80MixNeg68dBV)
			ttPwr = TC2K_ttPwr_LnaNeg74MixNeg62dBV;
		else if (ttPwr == TC2K_ttPwr_LnaNeg74MixNeg62dBV)
			ttPwr = TC2K_ttPwr_LnaNeg68MixNeg56dBV;
		else if (ttPwr == TC2K_ttPwr_LnaNeg68MixNeg56dBV)
			ttPwr = TC2K_ttPwr_LnaNeg62MixNeg50dBV;
		else if (ttPwr == TC2K_ttPwr_LnaNeg62MixNeg50dBV)
			ttPwr = TC2K_ttPwr_LnaNeg56MixNeg44dBV;
		else if (ttPwr == TC2K_ttPwr_LnaNeg56MixNeg44dBV)
			ttPwr = TC2K_ttPwr_LnaNeg50MixNeg38dBV;
		else if (ttPwr == TC2K_ttPwr_LnaNeg50MixNeg38dBV)
			ttPwr = TC2K_ttPwr_LnaNeg44MixNeg32dBV;
		else
			return TC2K_err_ifAccTop_setTtPwr;
		/* @doc Update ttPwr (in while loop). */
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ttPwr_reg, TC2K_ttPwr_mask, ttPwr, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		_mt_sleep(5);
		retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &ifAccTopVref);
		if (retVal != TC2K_ok)
			return retVal;
	}

	/* @doc Find ifAccum backoff using binary search. */
	ifAccTopVtarget = ifAccTopVref * TC2K_ifAccTopRefN[ifAccTopValueIndex];
	ifAccTopVtarget >>= TC2K_ifAccTopRefD;
	ifAccMin = 0;
	ifAccMax = 4095;
	while (ifAccMin + 1 < ifAccMax) {
		/* set if gain */
		ifAcc = (ifAccMin + ifAccMax) / 2;
		buff[0] = (U8) ifAcc;
		buff[1] = (U8) (ifAcc >> 8);
		retVal = _mt_fe_tn_tc2000_i2c_write (tunerCfg, TC2K_agcAccWrLo_reg, buff, 2, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_enaWr, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_agcAccLoad_reg, TC2K_agcAccLoad_mask, TC2K_agcAccLoad_disWr, TC2K_2wbSync_yes);
		if (retVal != TC2K_ok)
			return retVal;
		/* read detector */
		_mt_sleep(10);
		retVal = _mt_fe_tn_tc2000_cfgCalDetRead (tunerCfg, &ifAccTopVdet);
		if (retVal != TC2K_ok)
			return retVal;
		if (ifAccTopVdet < ifAccTopVtarget)
			ifAccMin = ifAcc;
		else
			ifAccMax = ifAcc;
	}

	/* @doc Store ifAccTop for current tuner mode (in tuner data structure). */
	tunerCfg->tunerData.ifAccTop[ifAccTopValueIndex] = ifAcc;

	return TC2K_ok;
}

/*
 * _mt_fe_tn_tc2000_ifSetAccTop()
 *
 * @param[in, out] tunerCfg Tuner configuration structure.
 *
 * @return  TC2K_ok on success
 * 			TC2K_err_rw otherwise.
 *
 * @note Called (indirectly) by mt_fe_tn_tc2000_set_chan().
 */
TC2K_RET _mt_fe_tn_tc2000_ifSetAccTop(tc2000_tn_config *tunerCfg, U8 ifAccTopValueIndex)
{
	TC2K_RET retVal;

	/* @doc _mt_fe_tn_tc2000_ifSetAccTop() 2wb optimization. Requires a w sync after calling. */

	/* @doc Write ifAgcAccTop based on tuner configuration (stored in tuner data structure). */
	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcAccTopLo_reg, TC2K_ifAgcAccTopLo_mask, (U8)(tunerCfg->tunerData.ifAccTop[ifAccTopValueIndex]), TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	retVal = _mt_fe_tn_tc2000_set_reg (tunerCfg, TC2K_ifAgcAccTopHi_reg, TC2K_ifAgcAccTopHi_mask, (U8)(tunerCfg->tunerData.ifAccTop[ifAccTopValueIndex]>>8), TC2K_2wbSync_no);
	if (retVal != TC2K_ok)
		return retVal;

	return TC2K_ok;
}

/* nim porting layer */
MT_FE_RET mt_fe_dc2800_tn_init_tc2000(void *dev_handle)
{
  TC2K_RET ret;
  tc2000_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

  ret = mt_fe_tn_tc2000_init(&tc2k_default);

  if(ret == TC2K_ok)
  {
      mtos_printk("11\n");
      tc2000_handle->tuner_settings.tuner_init_OK = TRUE;
      return MtFeErr_Ok;
  }
  else
  {
      return MtFeErr_Fail;
  } 
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_tc2000(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
  TC2K_RET ret;

  tc2k_default.FchanHz = freq_KHz * 1000;

  ret = mt_fe_tn_tc2000_set_chan(&tc2k_default);

  if (ret != TC2K_ok) 
  {
    return MtFeErr_Fail;
  }

  return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_signal_strength_tc2000(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	U8 SignalStrength = 0;
	TC2K_RET ret;
       tc2000_handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	/*
	 if ((dc2800_dmd_reg_read(0x43)&0x08) == 0x08)
	 {
	 if (((dc2800_dmd_reg_read(0xE3) & 0xC0) == 0x00) && ((dc2800_dmd_reg_read(0xE4) & 0xC0) == 0x00))
	 SignalStrength = 255 - ((dc2800_dmd_reg_read(0x55) >> 1) + (dc2800_dmd_reg_read(0x56) >> 1));
	 else
	 SignalStrength = 255 - ((dc2800_dmd_reg_read(0x3B) >> 1) + (dc2800_dmd_reg_read(0x3C) >> 1));
	 }
	 */

	ret = mt_fe_tn_tc2000_rssi(&tc2k_default);
	if (ret != TC2K_ok)
		return MtFeErr_Fail;

	if (tc2k_default.RSSI < -107)
		SignalStrength = 0;
	else
		SignalStrength = (U8) (tc2k_default.RSSI + 107);

       *p_strength = SignalStrength;
       
	return MtFeErr_Ok;
}

extern MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type ;
void tuner_attach_tc2000()
{
  dc2800_tuner_type = TN_MONTAGE_TC2000;
}
