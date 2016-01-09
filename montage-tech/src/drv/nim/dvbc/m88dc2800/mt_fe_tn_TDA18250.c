/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "mt_fe_tn_TDA18250.h"

MT_FE_DC2800_Device_Handle tda_handle = NULL;


tmErrorCode_t UserWrittenI2CRead(tmUnitSelect_t tUnit, UInt32 AddrSize, UInt8* pAddr, UInt32 ReadLen, UInt8* pData)
{
	U8 p_buf[128];
	U8 i = 0;
	p_buf[0] = *pAddr;

	tda_handle->tn_write(tda_handle, pAddr, (unsigned short)AddrSize);

	tda_handle->tn_read(tda_handle, p_buf, (unsigned short)ReadLen);

	for(i = 0; i < ReadLen; i ++)
	{
		pData[i] = p_buf[i];
	}

	return TM_OK;
}


tmErrorCode_t UserWrittenI2CWrite(tmUnitSelect_t tUnit, UInt32 AddrSize, UInt8* pAddr, UInt32 WriteLen, UInt8* pData)
{
	U8 p_buf[128];
	U8 i = 0;

	p_buf[0] = *pAddr;
	for(i = 0; i < WriteLen; i ++)
	{
		p_buf[i + 1] = pData[i];
	}

	tda_handle->tn_write(tda_handle, p_buf, (unsigned short)(WriteLen + 1));

	return TM_OK;
}

tmErrorCode_t UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms)
{
	tda_handle->mt_sleep(tms);

	return TM_OK;
}

MT_FE_RET mt_fe_tn_init_TDA18250(void *dev_handle)
{
	tmErrorCode_t err = TM_OK;
	tmbslFrontEndDependency_t sSrvTunerFunc;

	tda_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	/* Low layer struct set-up to link with user written functions */
	sSrvTunerFunc.sIo.Write				 = UserWrittenI2CWrite;
	sSrvTunerFunc.sIo.Read				 = UserWrittenI2CRead;
	sSrvTunerFunc.sTime.Get				 = Null;
	sSrvTunerFunc.sTime.Wait			 = UserWrittenWait;
	sSrvTunerFunc.sDebug.Print			 = NULL;
	sSrvTunerFunc.sMutex.Init			 = NULL;
	sSrvTunerFunc.sMutex.DeInit			 = NULL;
	sSrvTunerFunc.sMutex.Acquire		 = NULL;
	sSrvTunerFunc.sMutex.Release		 = NULL;
	sSrvTunerFunc.dwAdditionalDataSize	 = 0;
	sSrvTunerFunc.pAdditionalData		 = Null;

	/* Initialize low level driver setup environment */
	//err = tmddTDA18250Init(tUnit, &sSrvTunerFunc);
	err = tmbslTDA18250Init(0, &sSrvTunerFunc);
	/* TDA18250 Master Hardware initialization */
	//err = tmddTDA18250Reset(0);

	err = TDA18250Reset(0);  /* I: Unit number */

	if(err != TM_OK)
		return MtFeErr_Fail;

	/* TDA18250 Master Hardware power state */
	err = tmbslTDA18250SetPowerState(0, tmTDA18250_PowerNormalMode);
	///* TDA18250 Master Hardware power state */
	//err = tmddTDA18250SetPowerState(0, tmTDA18250_PowerNormalMode);

	err = TDA18250SetStandardMode(0, tmTDA18250_DIG_9MHz);
	if(err != TM_OK)
		return MtFeErr_Fail;

	#if 0
	/* TDA18250 Master RF frequency */
	err = tmddTDA18250SetLOFreq(0, uRFMaster);
	if(err != TM_OK)
		return err;

	/* Get TDA18250 Master PLL Lock status */
	err = tmddTDA18250GetLockStatus(0, &PLLLockMaster);
	if(err != TM_OK)
		return err;

	/* Get TDA18250 Master IF */
	err = tmddTDA18250GetIF(0, &uIFMaster);
	if(err != TM_OK)
		return err;
	#endif

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_tn_set_freq_TDA18250(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	tmErrorCode_t err = TM_OK;
	tmbslFrontEndState_t PLLLockMaster = tmbslFrontEndStateUnknown;

	tda_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	tmbslTDA18250SetRf(0, freq_KHz * 1000);

	/* Get TDA18250 Master PLL Lock status */
	err = tmbslTDA18250GetLockStatus(0, &PLLLockMaster);

	if(err != TM_OK)
		return MtFeErr_Fail;
	/* Get TDA18250 Master IF */
	//err = tmbslTDA18250GetIF(0, &uIFMaster);
	//if(err != TM_OK)
	//	return MtFeErr_Fail;

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_tn_get_signal_strength_TDA18250(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	tda_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	*p_gain = 0;
	*p_strength = 0;

	return MtFeErr_Ok;
}

// tUnits to identify the components
#define TunerUnit0 0
#define TunerUnit1 2

tmErrorCode_t Init(tmUnitSelect_t path);
tmErrorCode_t Tune(tmUnitSelect_t path, UInt32 uRF_Hz);

// This function opens the drivers and initializes the components for one path
tmErrorCode_t Init(tmUnitSelect_t path)
{
    tmUnitSelect_t tunerUnit;

    tmbslFrontEndDependency_t  sSrvFunc;
    TDA18250A_AdditionnalData_t sTDA18250AAdditionnalData;
    tmErrorCode_t err = TM_ERR_NOT_INITIALIZED;

    if (path == 0)
    {
        tunerUnit = TunerUnit0;
    }
    else
    {
        tunerUnit = TunerUnit1;
    }

    sSrvFunc.sIo.Write = UserWrittenI2CWrite;
    sSrvFunc.sIo.Read = UserWrittenI2CRead;
    sSrvFunc.sTime.Wait = UserWrittenWait;
	sSrvFunc.sDebug.Print = NULL;
    sSrvFunc.sMutex.Init = NULL;
    sSrvFunc.sMutex.DeInit = NULL;
    sSrvFunc.sMutex.Acquire = NULL;
    sSrvFunc.sMutex.Release = NULL;

    /* Tuner init */
    if (path == 0)
    {
        sTDA18250AAdditionnalData.tOtherUnit = TunerUnit1;
    }
    else
    {
        sTDA18250AAdditionnalData.tOtherUnit = TunerUnit0;
    }
    sSrvFunc.dwAdditionalDataSize = sizeof(TDA18250A_AdditionnalData_t);
    sSrvFunc.pAdditionalData = &sTDA18250AAdditionnalData;
    err = tmbslTDA18250A_Open(tunerUnit, &sSrvFunc);
    if (err == TM_OK)
    {
	    err = tmbslTDA18250A_HwInit(tunerUnit);
    }
    if (err == TM_OK)
    {
	    err = tmbslTDA18250A_SetPowerState(tunerUnit, tmPowerOn);
    }

    return err;
}

// This function tunes a path
tmErrorCode_t Tune(tmUnitSelect_t path, UInt32 uRF_Hz)
{
    tmUnitSelect_t tunerUnit;
    tmErrorCode_t err;

    if (path == 0)
    {
        tunerUnit = TunerUnit0;
    }
    else
    {
        tunerUnit = TunerUnit1;
    }

    // Set Tuner
    err = tmbslTDA18250A_SetStandardMode(tunerUnit, TDA18250A_QAM_8MHz);
    if (err == TM_OK)
    {
        err = tmbslTDA18250A_SetRF(tunerUnit, uRF_Hz);
    }

    return err;
}



MT_FE_RET mt_fe_tn_init_TDA18250A(void *dev_handle)
{
	tmErrorCode_t tm_ret;

	tda_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	tm_ret = Init(0);

	return (tm_ret == TM_OK) ? MtFeErr_Ok : MtFeErr_Fail;
}


MT_FE_RET mt_fe_tn_set_freq_TDA18250A(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	tmErrorCode_t tm_ret;

	tm_ret = Tune(0, freq_KHz * 1000);

	return (tm_ret == TM_OK) ? MtFeErr_Ok : MtFeErr_Fail;
}


MT_FE_RET mt_fe_tn_get_signal_strength_TDA18250A(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	tda_handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	*p_gain = 0;
	*p_strength = 0;

	return MtFeErr_Ok;
}

extern MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type;
void tuner_attach_TDA18250(void)
{
  dc2800_tuner_type = TN_NXP_TDA18250;
}

void tuner_attach_TDA18250A(void)
{
  dc2800_tuner_type = TN_NXP_TDA18250A;
}

extern MT_FE_DC2800_Device_Handle dc_2800_get_handle();
void tuner_attach_TDA18250_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_NXP_TDA18250;

  p_handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_TDA18250;
  p_handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_TDA18250;
  p_handle->tuner_settings.tuner_get_offset		 = NULL;
  p_handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_TDA18250;

  p_handle->tuner_settings.tuner_sleep			 = NULL;
  p_handle->tuner_settings.tuner_wakeup			 = NULL;
  p_handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_TDA18250;
}
