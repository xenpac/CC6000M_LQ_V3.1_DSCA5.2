/*****************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd                                    */
/* MONTAGE PROPRIETARY AND CONFIDENTIAL                                      */
/* Copyright (c) 2015 Montage Technology Group Limited. All Rights Reserved. */
/*****************************************************************************/
/*
 * Filename:        mt_fe_dmd_dc2800_tuner.c
 *
 * Description:     Interface of tuner.
 *
 *****************************************************************************/
/*****************************************************************************/
/*     Version:     1.02.09                                                  */
/*********************Supported Tuner List************************************/
/*  Vendor  Name    Model   Version Date    Time    Author      Comments     */
/*  Montage TC2800  -       1.02.03 141117  10:00   YZ.Huang    Update       */
/*  Montage TC3800  -       1.02.06 150325  11:00   YZ.Huang    Update       */
/*  NXP     TDA     18250   1.02.07 150507  11:00   YZ.Huang    Create        */
/*  NXP     TDA     18250A  1.02.07 150507  11:00   YZ.Huang    Create        */
/*  MxL     MxL     608     1.02.09 150728  17:00   YZ.Huang    Create       */
/*  All     Default Tuner   1.02.05 150210  17:00   YZ.Huang    Update       */
/*****************************************************************************/

#include "sys_types.h"
#include "mtos_mem.h"
#include "mt_fe_i2c.h"
#include "mt_fe_tn_tc2800.h"
#include "mt_fe_tn_TDA18250.h"
#include "mt_fe_tn_tc3800.h"
#include "mt_fe_tn_R836.h"
#include "mt_fe_tn_MxL608.h"
#include <string.h>

MT_FE_RET mt_fe_dc2800_tn_get_version_default(void *dev_handle, U32 *version_no, U32 *version_time)
{
	*version_no = 10209;		// for Can tuner, use version number of this file
	*version_time = 15072817;	// for Can tuner, use version time of this file

	return MtFeErr_Ok;
}

/*  Montage TC2800  - 1.00.00 120209  15:00   YZ.Huang    Create              */
/*                    1.00.01 120312  13:00   YZ.Huang    Update              */
/*                    1.00.02 120412  13:00   YZ.Huang    Update  V3.00.08    */
/*                    1.00.06 120524  11:00   YZ.Huang    Update  V3.00.12    */
/*                    1.00.07 120605  18:00   YZ.Huang    Update  V3.00.14    */
/*                    1.00.08 120606  16:00   YZ.Huang    Update  V3.00.15    */
/*                    1.00.09 120629  15:00   YZ.Huang    Update  V3.00.17    */
/*                    1.00.10 120704  11:00   YZ.Huang    Update  V3.00.18    */
/*                    1.00.11 120713  11:00   YZ.Huang    Update  V3.00.19    */
/*                    1.00.12 120725  15:00   YZ.Huang    Update  V3.00.20    */
/*                    1.00.13 121016  10:00   YZ.Huang    Update  V3.00.22    */
/*                    1.00.15 130108  14:00   YZ.Huang    Update  V3.00.23    */
/*                    1.00.16 130314  14:00   YZ.Huang    Update  V3.00.24    */
/*                    1.00.17 130520  18:00   YZ.Huang    Update  V3.00.26    */
/*                    1.00.18 130614  11:00   YZ.Huang    Update  V3.00.27    */
/*                    1.00.19 130709  16:00   YZ.Huang    Update  V3.00.28    */
/*                    1.00.20 130724  18:00   YZ.Huang    Update  V3.00.29    */
/*                    1.00.21 140311  19:00   YZ.Huang    Update  V3.00.30    */
/*                    1.02.03 141117  10:00   YZ.Huang    Update  V3.00.31    */

MT_FE_RET mt_fe_dc2800_tn_init_tc2800(void *dev_handle)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
    MT_FE_TN_TC2800_Handle tn_handle = NULL;

    handle->tuner_settings.tuner_handle = (U32)mtos_malloc(sizeof(MT_FE_TN_TC2800_SETTINGS));
    memset((void *)handle->tuner_settings.tuner_handle, 0, sizeof(MT_FE_TN_TC2800_SETTINGS));
    
    tn_handle = (MT_FE_TN_TC2800_Handle)handle->tuner_settings.tuner_handle;
    mt_fe_tn_init_tc2800(handle);
    tn_handle->tuner_dev_addr = handle->tuner_settings.tuner_dev_addr;
    handle->tuner_settings.tuner_init_OK = TRUE;

    return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_tc2800(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	if(!(handle->tuner_settings.tuner_init_OK))
	{
		return MtFeErr_Uninit;
	}

	handle->channel_param.freq_KHz	 = freq_KHz;
	handle->channel_param.sym_KSs	 = symbol_rate_KSs;

	mt_fe_tn_set_freq_tc2800(handle, freq_KHz);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_offset_tc2800(void *dev_handle, S32 *freq_offset_KHz)
{
	*freq_offset_KHz = 0;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_tc2800(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	*p_gain = 0;

	*p_strength = (U32)(mt_fe_tn_get_signal_strength_tc2800(handle) + 107.0);		// dBm + 107 ->dBuV

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_sleep_tc2800(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	mt_fe_tn_sleep_tc2800(handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_wakeup_tc2800(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	mt_fe_tn_wakeup_tc2800(handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_version_tc2800(void *dev_handle, U32 *version_no, U32 *version_time)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_TN_TC2800_Handle tuner_handle = (MT_FE_TN_TC2800_Handle)(handle->tuner_settings.tuner_handle);

	*version_no = 0;
	*version_time = 0;
	if(!(handle->tuner_settings.tuner_init_OK))
	{
		return MtFeErr_Uninit;
	}

	*version_no = tuner_handle->tuner_version;
	*version_time = tuner_handle->tuner_time;

	return MtFeErr_Ok;
}

MT_FE_DC2800_Device_Handle demod_handle;

/*  MxL     MxL     608     1.02.09 150728  17:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_MXL608(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	demod_handle = handle;

	handle->tuner_settings.tuner_type = TN_MXL_608;

	mt_fe_tn_init_MxL608(handle);

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dc2800_tn_set_freq_MXL608(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

	ret = mt_fe_tn_set_freq_MxL608(handle, freq_KHz, symbol_rate_KSs);

	return ret;
}

/*  ALPS    TDAC    -       1.00.00 120209  17:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_ALPS_TDAC(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	handle->tuner_settings.tuner_type = TN_ALPS_TDAC;

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_ALPS_TDAC(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

	U8  tuner_reg[5];
	U16 N;


	N = (U16)((freq_KHz + 36125) * 10 / 625);	/*	step:	62.5 KHz	*/
	tuner_reg[0] = (U8)((N >> 8) & 0x7f);
	tuner_reg[1] = (U8)(N & 0xff);
	tuner_reg[2] = 0x9b;


	if(freq_KHz < 125000)
		tuner_reg[3] = 0xa0;
	else if(freq_KHz < 174000)
		tuner_reg[3] = 0xa2;
	else if(freq_KHz < 366000)
		tuner_reg[3] = 0xe2;
	else if(freq_KHz < 766000)
		tuner_reg[3] = 0xa8;
	else
		tuner_reg[3] = 0x28;


	tuner_reg[4] = 0xc6;


	ret = handle->tn_write(handle, tuner_reg, sizeof(tuner_reg));

	handle->mt_sleep(1);

	return ret;
}


/*  XuGuang XDCT    6A      1.00.00 120209  17:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_XuGuang_XDCT6A(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	handle->tuner_settings.tuner_type = TN_XUGUANG_XDCT6A;

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dc2800_tn_set_freq_XuGuang_XDCT6A(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

	U8	tuner_reg[5];
	U16 N;



	N = (U16)((freq_KHz + 36000) * 100 / 16667);	/*	step:	166.67 KHz	*/
	tuner_reg[0] = (U8)((N >> 8) & 0x7f);
	tuner_reg[1] = (U8)(N & 0xff);
	tuner_reg[2] = 0xA0;


	if(freq_KHz <= 151000)
		tuner_reg[3] = 0x41;
	else if(freq_KHz <= 447000)
		tuner_reg[3] = 0x42;
	else
		tuner_reg[3] = 0x48;


	tuner_reg[4] = 0xC0;


	ret = handle->tn_write(handle, tuner_reg, sizeof(tuner_reg));

	handle->mt_sleep(1);

	return ret;
}


/*  NXP		TDA     18250   1.00.03 120413  16:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_TDA18250(void *dev_handle)
{
	MT_FE_RET ret;
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	ret = mt_fe_tn_init_TDA18250(handle);
    handle->tuner_settings.tuner_type = TN_NXP_TDA18250;
	handle->tuner_settings.tuner_init_OK = (ret == MtFeErr_Ok) ? TRUE : FALSE;

	return ret;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_TDA18250(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	return mt_fe_tn_set_freq_TDA18250(dev_handle, freq_KHz, symbol_rate_KSs);
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_TDA18250(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	return mt_fe_tn_get_signal_strength_TDA18250(dev_handle, p_gain, p_strength);
}

MT_FE_RET mt_fe_dc2800_tn_get_version_TDA18250(void *dev_handle, U32 *version_no, U32 *version_time)
{
	*version_no = 30018;
	*version_time = 11061510;

	return MtFeErr_Ok;
}


/*  NXP		TDA     18250A  1.02.07 150507  11:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_TDA18250A(void *dev_handle)
{
	MT_FE_RET ret;
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	
	ret = mt_fe_tn_init_TDA18250A(handle);
	handle->tuner_settings.tuner_init_OK = (ret == MtFeErr_Ok) ? TRUE : FALSE;
	
	return ret;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_TDA18250A(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	return mt_fe_tn_set_freq_TDA18250A(dev_handle, freq_KHz, symbol_rate_KSs);
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_TDA18250A(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	return mt_fe_tn_get_signal_strength_TDA18250A(dev_handle, p_gain, p_strength);
}

/*  TDCC_G051F   ZhouYong.Wu    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_TDCC_G051F(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	handle->tuner_settings.tuner_type = TN_TDCCG051F;
    handle->tuner_settings.tuner_dev_addr = 0xC0;

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}

extern signed long TDCC_G051F_Set(MT_FE_DC2800_Device_Handle handle , U32 freq_KHz, U16 nqam);
extern unsigned long TDCC_G051F_GetSignalStrength ( MT_FE_DC2800_Device_Handle handle );
MT_FE_RET mt_fe_dc2800_tn_set_freq_TDCC_G051F(void *dev_handle, U32 freq, U16 symbol_rate)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

    TDCC_G051F_Set(handle, freq, symbol_rate);

	return ret;
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_TDCC_G051F(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	*p_gain = 0;

	*p_strength = TDCC_G051F_GetSignalStrength(handle);	// dBm + 107 ->dBuV

	return MtFeErr_Ok;
}
/*  LG		TDCC    G0X1X   1.00.05 120503  17:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_LG_TDCC(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	handle->tuner_settings.tuner_type = TN_LG_TDCC;

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_LG_TDCC(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

	U8	tuner_reg[5];
	U16 N;



	N = (U16)((freq_KHz + 36125) * 10 / 625);	/*	step:	62.5 KHz	*/
	tuner_reg[0] = (U8)((N >> 8) & 0x7f);
	tuner_reg[1] = (U8)(N & 0xff);
	tuner_reg[2] = 0x93;


	if (freq_KHz <= 148000)
		tuner_reg[3] = 0x01;
	else if (freq_KHz <= 430000)
		tuner_reg[3] = 0x02;
	else
		tuner_reg[3] = 0x08;


	tuner_reg[4] = 0xc3;


	ret = handle->tn_write(handle, tuner_reg, sizeof(tuner_reg));

	handle->mt_sleep(1);

	return ret;
}


/*  LG		TDCC    G1X1F   1.00.05 121227  15:00   YZ.Huang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_LG_TDCC_G1X1F(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	handle->tuner_settings.tuner_type = TN_LG_TDCC;

	handle->tuner_settings.tuner_init_OK = TRUE;

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dc2800_tn_set_freq_LG_TDCC_G1X1F(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	MT_FE_RET ret = MtFeErr_Ok;

	U8	tuner_reg[5];
	U16 N;



	N = (U16)((freq_KHz + 36125) * 10 / 625);	/*	step:	62.5 KHz	*/
	tuner_reg[0] = (U8)((N >> 8) & 0x7f);
	tuner_reg[1] = (U8)(N & 0xff);
	tuner_reg[2] = 0xa3;


	if (freq_KHz <= 148000)
		tuner_reg[3] = 0x01;
	else if (freq_KHz <= 430000)
		tuner_reg[3] = 0x02;
	else
		tuner_reg[3] = 0x08;


	tuner_reg[4] = 0xd1;


	ret = handle->tn_write(handle, tuner_reg, sizeof(tuner_reg));

	handle->mt_sleep(1);

	return ret;
}


/*  Montage TC3800  - 1.01.00 140703  11:00   YZ.Huang    Create              */
/*                    1.01.00 140703  11:00   YZ.Huang    Update              */



MT_FE_RET mt_fe_dc2800_tn_init_tc3800(void *dev_handle)
{
        MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
        MT_FE_Tuner_Handle_TC3800 tuner_handle = NULL;

        handle->tuner_settings.tuner_handle = (U32)mtos_malloc(sizeof(MT_FE_TN_DEVICE_SETTINGS_TC3800));
        memset((void *)handle->tuner_settings.tuner_handle, 0, sizeof(MT_FE_TN_DEVICE_SETTINGS_TC3800));

        tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);
        tuner_handle->tuner_dev_addr = handle->tuner_settings.tuner_dev_addr;
        tuner_handle->tuner_mode = handle->tuner_settings.tuner_loopthrough;
        tuner_handle->tuner_bandwidth = handle->tuner_settings.tuner_bandwidth;
        tuner_handle->p_private = handle;
        mt_fe_tn_init_tc3800(tuner_handle);
        handle->tuner_settings.tuner_init_OK = TRUE;

        return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_tc3800(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_Tuner_Handle_TC3800 tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);

	if(!(handle->tuner_settings.tuner_init_OK))
	{
		return MtFeErr_Uninit;
	}

	handle->channel_param.freq_KHz	 = freq_KHz;
	handle->channel_param.sym_KSs	 = symbol_rate_KSs;
         tuner_handle->tuner_bandwidth = handle->tuner_settings.tuner_bandwidth;
	mt_fe_tn_set_freq_tc3800(tuner_handle, freq_KHz);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_offset_tc3800(void *dev_handle, S32 *freq_offset_KHz)
{
	*freq_offset_KHz = 0;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_tc3800(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_Tuner_Handle_TC3800 tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);

	*p_gain = 0;

	*p_strength = (U32)(mt_fe_tn_get_signal_strength_tc3800(tuner_handle) + 107.0);		// dBm + 107 ->dBuV

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_sleep_tc3800(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_Tuner_Handle_TC3800 tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);

	mt_fe_tn_sleep_tc3800(tuner_handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_wakeup_tc3800(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_Tuner_Handle_TC3800 tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);

	mt_fe_tn_wakeup_tc3800(tuner_handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_version_tc3800(void *dev_handle, U32 *version_no, U32 *version_time)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_Tuner_Handle_TC3800 tuner_handle = (MT_FE_Tuner_Handle_TC3800)(handle->tuner_settings.tuner_handle);

	*version_no = 0;
	*version_time = 0;
	if(!(handle->tuner_settings.tuner_init_OK))
	{
		return MtFeErr_Uninit;
	}

	*version_no = tuner_handle->tuner_version;
	*version_time = tuner_handle->tuner_time;

	return MtFeErr_Ok;
}

MT_FE_DC2800_SUPPORTED_TUNER mt_fe_dc2800_check_tuner_type(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	MT_FE_DC2800_SUPPORTED_TUNER tuner_type = handle->tuner_settings.tuner_type;

	MT_FE_RET ret;

	if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800))
	{
		if(handle->tn_get_reg != NULL)
		{
			U8 Reg01H = 0;
			
			ret = handle->tn_get_reg(handle, 0x01, &Reg01H);
			if(ret != MtFeErr_Ok)
			{
				return tuner_type;
			}

			if((Reg01H & 0xF0) == 0xB0)
			{
				tuner_type = TN_MONTAGE_TC3800;
			}
			else if((Reg01H == 0x0D) || (Reg01H == 0x8E))
			{
				tuner_type = TN_MONTAGE_TC2800;
			}
			else
			{
				return tuner_type;
			}
		}
	}

	return tuner_type;
}

/*  RAFAEL R836   2.9E 150318  15:00   BJ.Wang    Create        */
MT_FE_RET mt_fe_dc2800_tn_init_R836(void *dev_handle)
{
	MT_FE_RET ret;
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
	ret = mt_fe_tn_init_R836(handle);
	handle->tuner_settings.tuner_init_OK = (ret == MtFeErr_Ok) ? TRUE : FALSE;
	
	return ret;
}

MT_FE_RET mt_fe_dc2800_tn_set_freq_R836(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs)
{
	return mt_fe_tn_set_freq_R836(dev_handle, freq_KHz, symbol_rate_KSs);
}

MT_FE_RET mt_fe_dc2800_tn_sleep_R836(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	mt_fe_tn_sleep_R836(handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_wakeup_R836(void *dev_handle)
{
	MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

	mt_fe_tn_wakeup_R836(handle);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dc2800_tn_get_strength_R836(void *dev_handle, U32 *p_gain, U32 *p_strength)
{
	return mt_fe_tn_get_signal_strength_R836(dev_handle, p_gain, p_strength);
}

MT_FE_RET mt_fe_dc2800_tn_get_version_R836(void *dev_handle, U32 *version_no, U32 *version_time)
{
	*version_no = 2095; 
	*version_time = 150318;
	
	return MtFeErr_Ok;
}

extern MT_FE_DC2800_Device_Handle dc_2800_get_handle();
void tuner_attach_ALPS_TDAC_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_ALPS_TDAC;

  p_handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_ALPS_TDAC;
  p_handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_ALPS_TDAC;
  p_handle->tuner_settings.tuner_get_offset		 = NULL;
  p_handle->tuner_settings.tuner_get_strength	 = NULL;

  p_handle->tuner_settings.tuner_sleep			 = NULL;
  p_handle->tuner_settings.tuner_wakeup			 = NULL;
  p_handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
}

void tuner_attach_LG_TDCC_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_LG_TDCC;

  p_handle->tuner_settings.tuner_init             = mt_fe_dc2800_tn_init_LG_TDCC;
  p_handle->tuner_settings.tuner_set             = mt_fe_dc2800_tn_set_freq_LG_TDCC;
  p_handle->tuner_settings.tuner_get_offset         = NULL;
  p_handle->tuner_settings.tuner_get_strength     = NULL;

  p_handle->tuner_settings.tuner_sleep             = NULL;
  p_handle->tuner_settings.tuner_wakeup             = NULL;
  p_handle->tuner_settings.tuner_get_version     = mt_fe_dc2800_tn_get_version_default;
}

void tuner_attach_LG_TDCC_G1X1F_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_LG_TDCC_G1X1F;
  
  p_handle->tuner_settings.tuner_init             = mt_fe_dc2800_tn_init_LG_TDCC_G1X1F;
  p_handle->tuner_settings.tuner_set             = mt_fe_dc2800_tn_set_freq_LG_TDCC_G1X1F;
  p_handle->tuner_settings.tuner_get_offset         = NULL;
  p_handle->tuner_settings.tuner_get_strength     = NULL;

  p_handle->tuner_settings.tuner_sleep             = NULL;
  p_handle->tuner_settings.tuner_wakeup             = NULL;
  p_handle->tuner_settings.tuner_get_version     = mt_fe_dc2800_tn_get_version_default;
}
