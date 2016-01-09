/*****************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                    */
/* Montage Proprietary and Confidential                                      */
/* Copyright (c) 2015 Montage Technology Group Limited and its affiliated companies*/
/*****************************************************************************/


//#include <stdio.h>
#include "mt_fe_tn_MxL608.h"

#define EXAMPLE_DEV_MAX 2
#define MXL608_I2C_ADDR 0x60


MT_FE_RET mt_fe_tn_init_MxL608(void *dev_handle)
{
	MXL_STATUS status;
	UINT8 devId;
	MXL_BOOL singleSupply_3_3V;
	MXL608_XTAL_SET_CFG_T xtalCfg;
	MXL608_IF_OUT_CFG_T ifOutCfg;
	MXL608_AGC_CFG_T agcCfg;
	MXL608_TUNER_MODE_CFG_T tunerModeCfg;

	/* If OEM data is implemented, customer needs to use OEM data structure
	related operation. Following code should be used as a reference.
	For more information refer to sections 2.5 & 2.6 of
	MxL608_mxLWare_API_UserGuide document.

	for (devId = 0; devId < EXAMPLE_DEV_MAX; devId++)
	{
	// assigning i2c address for  the device
	device_context[devId].i2c_address = GET_FROM_FILE_I2C_ADDRESS(devId);

	// assigning i2c bus for  the device
	device_context[devId].i2c_bus = GET_FROM_FILE_I2C_BUS(devId);

	// create semaphore if necessary
	device_context[devId].sem = CREATE_SEM();

	// sample stat counter
	device_context[devId].i2c_cnt = 0;

	status = MxLWare608_API_CfgDrvInit(devId, (void *) &device_context[devId]);

	// if you donn't want to pass any oem data, just use NULL as a parameter:
	// status = MxLWare608_API_CfgDrvInit(devId, NULL);
	}

	*/

	/* If OEM data is not required, customer should treat devId as
	I2C slave Address */
	devId = MXL608_I2C_ADDR;

	//Step 1 : Soft Reset MxL608
	status = MxLWare608_API_CfgDevSoftReset(devId);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgDevSoftReset\n");
	//}

	//Step 2 : Overwrite Default
	//singleSupply_3_3V = MXL_DISABLE;
	singleSupply_3_3V = MXL_ENABLE;		// 140417
	status = MxLWare608_API_CfgDevOverwriteDefaults(devId, singleSupply_3_3V);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgDevOverwriteDefaults\n");
	//}

	//Step 3 : XTAL Setting
	xtalCfg.xtalFreqSel = MXL608_XTAL_16MHz;
	//xtalCfg.xtalFreqSel = MXL608_XTAL_24MHz;	// 140417
	xtalCfg.xtalCap = 12;
	xtalCfg.clkOutEnable = MXL_ENABLE;
	//xtalCfg.clkOutEnable = MXL_DISABLE;		// 140417
	xtalCfg.clkOutDiv = MXL_DISABLE;
	xtalCfg.clkOutExt = MXL_DISABLE;
	//xtalCfg.singleSupply_3_3V = MXL_DISABLE;
	xtalCfg.singleSupply_3_3V = MXL_ENABLE;	// 140417
	xtalCfg.XtalSharingMode = MXL_DISABLE;
	status = MxLWare608_API_CfgDevXtal(devId, xtalCfg);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgDevXtal\n");
	//}

	//Step 4 : IF Out setting
	ifOutCfg.ifOutFreq = MXL608_IF_5MHz;
	ifOutCfg.ifInversion = MXL_DISABLE;
	ifOutCfg.gainLevel = 11;
	ifOutCfg.manualFreqSet = MXL_DISABLE;
	ifOutCfg.manualIFOutFreqInKHz = 0;
	status = MxLWare608_API_CfgTunerIFOutParam(devId, ifOutCfg);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgTunerIFOutParam\n");
	//}

	//Step 5 : AGC Setting
	agcCfg.agcType = MXL608_AGC_EXTERNAL;
	agcCfg.setPoint = 66;
	agcCfg.agcPolarityInverstion = MXL_DISABLE;
	status = MxLWare608_API_CfgTunerAGC(devId, agcCfg);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgTunerAGC\n");
	//}

	//Step 6 : Application Mode setting
	tunerModeCfg.signalMode = MXL608_DIG_DVB_C;
	tunerModeCfg.ifOutFreqinKHz = 5000;
	tunerModeCfg.xtalFreqSel = MXL608_XTAL_16MHz;
	//tunerModeCfg.xtalFreqSel = MXL608_XTAL_24MHz;	// 140417
	tunerModeCfg.ifOutGainLevel = 11;
	status = MxLWare608_API_CfgTunerMode(devId, tunerModeCfg);
	//if (status != MXL_SUCCESS)
	//{
	//	printf("Error! MxLWare608_API_CfgTunerMode\n");
	//}

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_tn_set_freq_MxL608(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MXL_STATUS status;
	UINT8 devId;
	MXL608_CHAN_TUNE_CFG_T chanTuneCfg;
	MXL_BOOL refLockPtr = MXL_UNLOCKED;
	MXL_BOOL rfLockPtr = MXL_UNLOCKED;

	devId = MXL608_I2C_ADDR;

	chanTuneCfg.bandWidth = MXL608_CABLE_BW_8MHz;
	chanTuneCfg.freqInHz = freq_KHz * 1000;
	chanTuneCfg.signalMode = MXL608_DIG_DVB_C;
	chanTuneCfg.xtalFreqSel = MXL608_XTAL_16MHz;
	//chanTuneCfg.xtalFreqSel = MXL608_XTAL_24MHz;	// 140417
	chanTuneCfg.startTune = MXL_START_TUNE;
	status = MxLWare608_API_CfgTunerChanTune(devId, chanTuneCfg);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare608_API_CfgTunerChanTune\n");
	}

	// Wait 15 ms
	MxLWare608_OEM_Sleep(15);

	// Read back Tuner lock status
	status = MxLWare608_API_ReqTunerLockStatus(devId, &rfLockPtr, &refLockPtr);

#if 0
	if (status == MXL_TRUE)
	{
		if (MXL_LOCKED == rfLockPtr && MXL_LOCKED == refLockPtr)
		{
			printf("Tuner locked\n");
		}
		else
			printf("Tuner unlocked\n");
	}
#endif

	return MtFeErr_Ok;
}

extern MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type;
void tuner_attach_MxL608(void)
{
  dc2800_tuner_type = TN_MXL_608;
}

