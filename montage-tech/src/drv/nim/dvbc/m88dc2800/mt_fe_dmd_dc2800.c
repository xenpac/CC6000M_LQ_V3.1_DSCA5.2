/*****************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd                                    */
/* MONTAGE PROPRIETARY AND CONFIDENTIAL                                      */
/* Copyright (c) 2015 Montage Technology Group Limited. All Rights Reserved. */
/*****************************************************************************/
/*
 * Filename:            mt_fe_dmd_dc2800.c
 *
 * Description:         Montage QAM demodulator driver of Jazz.
 *
 * Current Version:     1.02.10
 *
 *  History:
 *
 *  Description     Version     Date        Author
 *---------------------------------------------------------------------------
 *  File Create     0.00.01     2012.02.01  YZ.Huang
 *  Modify          1.00.00     2012.02.02  YZ.Huang
 *  Modify          1.00.01     2012.03.06  YZ.Huang
 *  Modify          1.00.02     2012.03.12  YZ.Huang
 *  Modify          1.00.03     2012.03.13  YZ.Huang
 *  Modify          1.00.04     2012.03.20  YZ.Huang
 *  Modify          1.00.05     2012.03.27  YZ.Huang
 *  Modify          1.00.06     2012.03.28  YZ.Huang
 *  Modify          1.00.10     2012.04.12  YZ.Huang
 *  Modify          1.00.12     2012.04.27  YZ.Huang
 *  Modify          1.00.13     2012.05.03  YZ.Huang
 *  Update          1.00.14     2012.05.07  YZ.Huang
 *  Update          1.00.15     2012.05.15  YZ.Huang
 *  Update          1.00.16     2012.05.24  YZ.Huang
 *  Update          1.00.17     2012.06.05  YZ.Huang
 *  Update          1.00.18     2012.06.06  YZ.Huang
 *  Update          1.00.19     2012.06.12  YZ.Huang
 *  Update          1.00.20     2012.06.29  YZ.Huang
 *  Update          1.00.21     2012.07.04  YZ.Huang
 *  Update          1.00.22     2012.07.13  YZ.Huang
 *  Update          1.00.23     2012.07.25  YZ.Huang
 *  Update          1.00.24     2012.10.16  YZ.Huang
 *  Update          1.00.25     2012.11.07  YZ.Huang
 *  Update          1.00.26     2012.12.27  YZ.Huang
 *  Update          1.00.27     2013.01.08  YZ.Huang
 *  Update          1.00.28     2013.03.15  YZ.Huang
 *  Update          1.00.29     2013.06.14  YZ.Huang
 *  Update          1.00.30     2013.07.09  YZ.Huang
 *  Update          1.00.31     2013.07.24  YZ.Huang
 *  Update          1.00.32     2014.03.13  YZ.Huang
 *  Update          1.01.00     2014.04.14  YZ.Huang
 *  Update          1.01.01     2014.07.23  YZ.Huang
 *  Update          1.02.00     2014.07.29  YZ.Huang
 *  Update          1.02.01     2014.08.15  YZ.Huang
 *  Update          1.02.02     2014.08.27  YZ.Huang
 *  Update          1.02.03     2014.11.17  YZ.Huang
 *  Update          1.02.04     2014.12.23  YZ.Huang
 *  Update          1.02.06     2015.04.09  YZ.Huang
 *  Update          1.02.07     2015.05.07  YZ.Huang
 *  Update          1.02.08     2015.07.07  YZ.Huang
 *  Update          1.02.09     2015.09.22  YZ.Huang
 *  Update          1.02.10     2015.10.10  YZ.Huang
 *****************************************************************************/

#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "i2c.h"
#include "nim.h"

#include "mt_fe_i2c.h"
#include "mt_fe_tn_tc2800.h"
#include "mt_fe_common.h"
#include "nim_m88dc2800.h"
#include "mt_fe_tn_tc2000.h"

MT_FE_TN_TC2800_SETTINGS tuner_config = {0};
extern nim_m88dc280_priv_t *g_p_dc2800_priv;

u8 have_extra_iVppSel_iGainStep = 0;
u8 extra_iVppSel = 0;
u8 extra_iGainStep = 0;

MT_FE_RET mt_fe_dmd_dc2800_config_default(MT_FE_DC2800_Device_Handle handle)
{
	MT_FE_DC2800_SUPPORTED_TUNER tuner_type;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dev_index							 = 0;
	handle->demod_dev_addr						 = 0x38;
	handle->demod_type							 = MT_FE_DEMOD_M88DC2800;
	handle->dtv_mode							 = MtFeType_DVBC;

	handle->channel_param.freq_KHz				 = 762000000;
	handle->channel_param.sym_KSs				 = 6900;
	handle->channel_param.qam_code				 = 64;
	handle->channel_param.inverted				 = 0;
	handle->channel_param.param_reserved		 = 0;

	handle->tuner_settings.tuner_init_OK		 = FALSE;
	handle->tuner_settings.tuner_type			 = TN_MONTAGE_TC3800;
	handle->tuner_settings.tuner_dev_addr		 = 0xC2;
	handle->tuner_settings.tuner_mode			 = 0;
	handle->tuner_settings.tuner_handle			 = NULL;
	handle->tuner_settings.tuner_reserved		 = 0;


	handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_tc3800;
	handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_tc3800;
	handle->tuner_settings.tuner_get_offset		 = mt_fe_dc2800_tn_get_offset_tc3800;
	handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_tc3800;

	handle->tuner_settings.tuner_sleep			 = mt_fe_dc2800_tn_sleep_tc3800;
	handle->tuner_settings.tuner_wakeup			 = mt_fe_dc2800_tn_wakeup_tc3800;
	handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_tc3800;

	handle->on_board_settings.chip_mode			 = 1;
	handle->on_board_settings.bSingleXTAL		 = TRUE;
	handle->on_board_settings.bReadCali			 = FALSE;
         if(have_extra_iVppSel_iGainStep != 0)
         {
            handle->on_board_settings.iVppSel			 = extra_iVppSel;
            handle->on_board_settings.iGainStep			 = extra_iGainStep;
            have_extra_iVppSel_iGainStep = 0;
         }
         else
         {
            handle->on_board_settings.iVppSel			 = 0;
            handle->on_board_settings.iGainStep			 = 0;
         }
	handle->on_board_settings.xtal_KHz			 = 28800;
	handle->on_board_settings.chip_version		 = 0xD0;
	handle->on_board_settings.on_board_reserved	 = 0;

	handle->ts_output_settings.active_edge_clk	 = 0;
	handle->ts_output_settings.polar_sync		 = 0;
	handle->ts_output_settings.polar_val		 = 0;
	handle->ts_output_settings.polar_err		 = 0;
	handle->ts_output_settings.output_mode		 = MtFeTsOutMode_Common;
	handle->ts_output_settings.output_clock		 = MtFeTSOut_Max_Clock_Userdefine;
	handle->ts_output_settings.custom_duty_cycle = 0;
	handle->ts_output_settings.serial_pin_select = 0;
	handle->ts_output_settings.output_high_Z	 = 0;

	handle->dmd_set_reg							 = _mt_fe_dmd_set_reg;
	handle->dmd_get_reg							 = _mt_fe_dmd_get_reg;
	handle->write_fw							 = NULL;
	handle->tn_set_reg							 = _mt_fe_tn_set_reg;
	handle->tn_get_reg							 = _mt_fe_tn_get_reg;
	handle->tn_write							 = _mt_fe_tn_write;
	handle->tn_read								 = _mt_fe_tn_read;
	handle->mt_sleep							 = _mt_sleep;
	handle->Set32Bits							 = _mt_fe_write32;
	handle->Get32Bits							 = _mt_fe_read32;


	tuner_type = mt_fe_dc2800_check_tuner_type(handle);

	handle->tuner_settings.tuner_type = tuner_type;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dmd_dc2800_select_tuner(MT_FE_DC2800_Device_Handle handle, MT_FE_DC2800_SUPPORTED_TUNER tuner_type)
{
	handle->tuner_settings.tuner_type = tuner_type;

	tuner_type = mt_fe_dc2800_check_tuner_type(handle);

	handle->tuner_settings.tuner_type = tuner_type;

	if(tuner_type == TN_MONTAGE_TC2800)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_tc2800;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_tc2800;
		handle->tuner_settings.tuner_get_offset		 = mt_fe_dc2800_tn_get_offset_tc2800;
		handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_tc2800;

		handle->tuner_settings.tuner_sleep			 = mt_fe_dc2800_tn_sleep_tc2800;
		handle->tuner_settings.tuner_wakeup			 = mt_fe_dc2800_tn_wakeup_tc2800;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_tc2800;
	}
	else if(tuner_type == TN_ALPS_TDAC)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_ALPS_TDAC;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_ALPS_TDAC;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}
	else if(tuner_type == TN_XUGUANG_XDCT6A)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_XuGuang_XDCT6A;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_XuGuang_XDCT6A;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}
	else if(tuner_type == TN_NXP_TDA18250)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_TDA18250;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_TDA18250;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_TDA18250;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_TDA18250;
	}
	else if(tuner_type == TN_NXP_TDA18250A)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_TDA18250A;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_TDA18250A;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_TDA18250A;
		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_TDA18250;
	}
	else if(tuner_type == TN_LG_TDCC)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_LG_TDCC;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_LG_TDCC;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}
	else if(tuner_type == TN_LG_TDCC_G1X1F)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_LG_TDCC_G1X1F;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_LG_TDCC_G1X1F;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}
	else if(tuner_type == TN_MONTAGE_TC3800)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_tc3800;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_tc3800;
		handle->tuner_settings.tuner_get_offset		 = mt_fe_dc2800_tn_get_offset_tc3800;
		handle->tuner_settings.tuner_get_strength	 = mt_fe_dc2800_tn_get_strength_tc3800;

		handle->tuner_settings.tuner_sleep			 = mt_fe_dc2800_tn_sleep_tc3800;
		handle->tuner_settings.tuner_wakeup			 = mt_fe_dc2800_tn_wakeup_tc3800;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_tc3800;
	}
	else if(tuner_type == TN_RAFAEL_R836)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_R836;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_R836;
		handle->tuner_settings.tuner_get_offset		 = NULL;//mt_fe_dc2800_tn_get_offset_tc3800;
		handle->tuner_settings.tuner_get_strength	 = NULL;//mt_fe_dc2800_tn_get_strength_tc3800;
		
		handle->tuner_settings.tuner_sleep			 = mt_fe_dc2800_tn_sleep_R836;//mt_fe_dc2800_tn_sleep_tc3800;
		handle->tuner_settings.tuner_wakeup			 = mt_fe_dc2800_tn_wakeup_R836;//mt_fe_dc2800_tn_wakeup_tc3800;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_R836;
	}
	else if(tuner_type == TN_MXL_608)
	{
		handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_MXL608;
		handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_MXL608;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}
	else
	{
		handle->tuner_settings.tuner_init			 = NULL;
		handle->tuner_settings.tuner_set			 = NULL;
		handle->tuner_settings.tuner_get_offset		 = NULL;
		handle->tuner_settings.tuner_get_strength	 = NULL;

		handle->tuner_settings.tuner_sleep			 = NULL;
		handle->tuner_settings.tuner_wakeup			 = NULL;
		handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
	}

	handle->tuner_settings.tuner_init_OK			 = FALSE;

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dmd_dc2800_config_tuner_settings(MT_FE_DC2800_Device_Handle handle, MT_FE_DC2800_TN_SETTINGS *tuner_config)
{
	MT_FE_RET ret = MtFeErr_Ok;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if(tuner_config == NULL)
	{
		return MtFeErr_NullPointer;
	}

	handle->tuner_settings.tuner_init_OK		 = tuner_config->tuner_init_OK;
	handle->tuner_settings.tuner_type			 = tuner_config->tuner_type;
	handle->tuner_settings.tuner_dev_addr		 = tuner_config->tuner_dev_addr;
	handle->tuner_settings.tuner_mode			 = tuner_config->tuner_mode;
	handle->tuner_settings.tuner_handle			 = tuner_config->tuner_handle;
	handle->tuner_settings.tuner_reserved		 = tuner_config->tuner_reserved;

	handle->tuner_settings.tuner_init			 = tuner_config->tuner_init;
	handle->tuner_settings.tuner_set			 = tuner_config->tuner_set;
	handle->tuner_settings.tuner_get_offset		 = tuner_config->tuner_get_offset;
	handle->tuner_settings.tuner_get_strength	 = tuner_config->tuner_get_strength;

	handle->tuner_settings.tuner_sleep			 = tuner_config->tuner_sleep;
	handle->tuner_settings.tuner_wakeup			 = tuner_config->tuner_wakeup;
	handle->tuner_settings.tuner_get_version	 = tuner_config->tuner_get_version;

	return ret;
}


MT_FE_RET mt_fe_dmd_dc2800_calibration(MT_FE_DC2800_Device_Handle handle, U8 iMaxTimes)
{
	U8 data, tmp1, tmp2, tmp3, i;

	handle->dmd_get_reg(handle, 0x86, &data);
	data &= 0x77;
	handle->dmd_set_reg(handle, 0x86, data); //not invert demod system clock

	handle->dmd_get_reg(handle, 0x32, &tmp1);
	tmp1 |= 0x53;//53//5f
	handle->dmd_set_reg(handle, 0x32, tmp1); //set clear lms_lock log

	handle->dmd_get_reg(handle, 0x31, &tmp1);
	tmp1 |= 0x06;
	handle->dmd_set_reg(handle, 0x31, tmp1); //enable calibration & ext control

	handle->dmd_get_reg(handle, 0x33, &tmp1);
	tmp1 |= 0x43;//42-div2//43-div4//4b-div8
	handle->dmd_set_reg(handle, 0x33, tmp1); //enable ext control & clock div4

	handle->dmd_get_reg(handle, 0x34, &tmp1);
	tmp1 |= 0x40;//43//40
	handle->dmd_set_reg(handle, 0x34, tmp1); //enable ext control

	for(i = 0; i < iMaxTimes; i ++)
	{
		handle->dmd_get_reg(handle, 0x30, &tmp2);
		tmp2 |= 0x0F;
		handle->dmd_set_reg(handle, 0x30, tmp2); //reset calibration

		handle->mt_sleep(5);//wait 5ms

		handle->dmd_get_reg(handle, 0x32, &tmp1);
		tmp1 &= 0x3f;
		handle->dmd_set_reg(handle, 0x32, tmp1);//release clear lms_lock log

		tmp2 &= 0xF0;
		handle->dmd_set_reg(handle, 0x30, tmp2);//release calibration

		handle->mt_sleep(10); //wait 10ms

		handle->dmd_get_reg(handle, 0x39, &tmp3);  //judge if calibration is success
		if((tmp3 == 0x2c) || (tmp3 == 0x2d) || (tmp3 == 0x2e))  //after calibration, reg 0x39 should be 0x2c/2d/2e
			break;
	}

	handle->dmd_get_reg(handle, 0x31, &tmp1);
	tmp1 &= 0x00;
	handle->dmd_set_reg(handle, 0x31, tmp1); //disable calibration

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dmd_dc2800_init(MT_FE_DC2800_Device_Handle handle)
{
	MT_FE_RET ret = MtFeErr_Ok;
	U8 data = 0, tmp1 = 0, i = 0, tmp2 = 0;
	U32 reg1 = 0, reg2 = 0, reg3 = 0;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if((handle->on_board_settings.chip_mode == 1) && ((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800)))
	{
		handle->Get32Bits(0xBF5D0008, &reg1);
		handle->Get32Bits(0xBF5D0064, &reg2);

		reg3 = reg1 | 0x01;		// bit 0 = 0b'1
		handle->Set32Bits(0xBF5D0008, reg3);

		reg3 = reg2;			// bit [19:15] = 0b'11111
		reg3 |= 0x000F8000;
		handle->Set32Bits(0xBF5D0064, reg3);

		handle->dmd_get_reg(handle, 0x87, &data);
		data &= 0xF8;		// 1V
		if(handle->on_board_settings.iVppSel == 0)		// 2V
		{
			data |= 0x03;
		}
		handle->dmd_set_reg(handle, 0x87, data);  //Set 1V or 2V Vdpp

		mt_fe_dmd_dc2800_calibration(handle, 5);

		if(handle->on_board_settings.bReadCali)
		{
			/*Read Registers*/
			handle->mt_sleep(40); //wait 40ms

			for(i = 0x39; i <= 0x3F; i ++)
			{
				handle->dmd_get_reg(handle, i, &tmp1);
			}

			handle->dmd_get_reg(handle, 0x36, &tmp2);
			tmp2 |= 0x02;
			handle->dmd_set_reg(handle, 0x36, tmp2);

			for(i = 0; i < 28; i ++)
			{
				handle->dmd_get_reg(handle, 0x35, &tmp1);
			}

			tmp2 &= 0xFD;
			handle->dmd_set_reg(handle, 0x36, tmp2);
		}

		handle->dmd_get_reg(handle, 0x86, &data);
		data |= 0x08;
		handle->dmd_set_reg(handle, 0x86, data);

		handle->Set32Bits(0xBF5D0008, reg1);
		handle->Set32Bits(0xBF5D0064, reg2);
	}

	if(handle->tuner_settings.tuner_init != NULL)
	{
		ret = handle->tuner_settings.tuner_init(handle);
	}

	handle->Get32Bits(0xBF5D0040, &reg1);
	if(handle->on_board_settings.xtal_KHz == 29100)
	{
		reg1 |= 0x00010000;
	}
	else	// default 28800
	{
		reg1 &= 0xFFFEFFFF;
	}

	handle->Set32Bits(0xBF5D0040, reg1);

	return ret;
}


MT_FE_RET mt_fe_dmd_dc2800_connect(MT_FE_DC2800_Device_Handle handle, U32 freq_KHz, U16 symbol_rate_KSs, U16 qam, U8 inverted)
{
	MT_FE_RET ret = MtFeErr_Ok;
	U8 tmp1, tmp2;
	U32 ulTmp;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if(symbol_rate_KSs >= 7150)		// select 29.1M XTAL
	{
		if(handle->on_board_settings.xtal_KHz != 29100)
		{
			handle->Get32Bits(0xBF5D0040, &ulTmp);
			if((ulTmp & 0x00010000) == 0)
			{
				ulTmp |= 0x00010000;
				handle->Set32Bits(0xBF5D0040, ulTmp);
			}

			handle->on_board_settings.xtal_KHz = 29100;
		}
	}
	else							// select default 28.8M XTAL
	{
		if(handle->on_board_settings.xtal_KHz != 28800)
		{
			handle->Get32Bits(0xBF5D0040, &ulTmp);
			if((ulTmp & 0x00010000) != 0)
			{
				ulTmp &= 0xFFFEFFFF;
				handle->Set32Bits(0xBF5D0040, ulTmp);
			}

			handle->on_board_settings.xtal_KHz = 28800;
		}
	}

	handle->channel_param.freq_KHz	 = freq_KHz;
	handle->channel_param.sym_KSs	 = symbol_rate_KSs;
	handle->channel_param.qam_code	 = qam;
	handle->channel_param.inverted	 = inverted;

	handle->dmd_get_reg(handle, 0x86, &tmp1);
	handle->dmd_get_reg(handle, 0x87, &tmp2);

	if(handle->on_board_settings.chip_mode == 2)
	{
		handle->dmd_set_reg(handle, 0x84, 0x2a);
		handle->dmd_set_reg(handle, 0x84, 0x6a);
	}
	else
	{
		handle->dmd_set_reg(handle, 0x84, 0x00);
		handle->dmd_set_reg(handle, 0x84, 0x40);
	}

	if(handle->tuner_settings.tuner_type != TN_MXL_608)
	{
		handle->dmd_set_reg(handle, 0x80, 0x01);
	}

	if(handle->tuner_settings.tuner_init_OK == FALSE)
	{
		if(handle->tuner_settings.tuner_init != NULL)
		{
			ret = handle->tuner_settings.tuner_init(handle);
		}
	}

	if(handle->tuner_settings.tuner_set != NULL)
	{
		ret = handle->tuner_settings.tuner_set(handle, freq_KHz, symbol_rate_KSs);
	}

	_mt_fe_dmd_dc2800_set_demod(handle);

	_mt_fe_dmd_dc2800_set_symbol_rate(handle, symbol_rate_KSs, handle->on_board_settings.xtal_KHz);

	_mt_fe_dmd_dc2800_set_QAM(handle, qam);
	_mt_fe_dmd_dc2800_set_tx_mode(handle, inverted, 0);		/* J83A */
	_mt_fe_dmd_dc2800_set_ts_output(handle, handle->ts_output_settings.output_mode);


	_mt_fe_dmd_dc2800_set_demod_appendix(handle, handle->on_board_settings.chip_mode, qam);


	if((handle->tuner_settings.tuner_type == TN_NXP_TDA18250) || (handle->tuner_settings.tuner_type == TN_NXP_TDA18250A))
	{
		handle->dmd_set_reg(handle, 0x16, (inverted == 1) ? 0x13 : 0xEC);
		handle->dmd_set_reg(handle, 0x17, (inverted == 1) ? 0x8E : 0x72);
	}

	if((handle->tuner_settings.tuner_type == TN_MXL_608) && (handle->on_board_settings.chip_mode != 2))
	{
		handle->dmd_set_reg(handle, 0x16, (inverted == 1) ? 0x13 : 0xEC);
		handle->dmd_set_reg(handle, 0x17, (inverted == 1) ? 0x8E : 0x72);
	}
	
	if(handle->tuner_settings.tuner_type == TN_RAFAEL_R836)
	{
		handle->dmd_set_reg(handle, 0x16, (inverted == 1) ? 0x13 : 0xEC);
		handle->dmd_set_reg(handle, 0x17, (inverted == 1) ? 0x8E : 0x72);
	}


	tmp1 &= 0x7F;
	tmp2 &= 0x07;

	handle->dmd_set_reg(handle, 0x86, tmp1);
	handle->dmd_set_reg(handle, 0x87, tmp2);

	if(handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800)
	{
		if(handle->on_board_settings.iVppSel == 0)		// 2V
		{
			handle->tn_set_reg(handle, 0x1B, 0x53);
		}
		else	// 1V
		{
			handle->tn_set_reg(handle, 0x1B, 0x47);
		}
	}
	else if(handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800)
	{
		if(handle->on_board_settings.iVppSel == 0)		// 2V
		{
			handle->tn_set_reg(handle, 0x1E, 0x9D);
		}
		else	// 1V
		{
			if(handle->on_board_settings.iGainStep == 1)	// FY
			{
				handle->tn_set_reg(handle, 0x1E, 0x9D);
			}
			else if(handle->on_board_settings.iGainStep == 2)	// FY
			{
				handle->tn_set_reg(handle, 0x1E, 0xA5);
			}
			else
			{
				handle->tn_set_reg(handle, 0x1E, 0x95);
			}
		}
	}


	mt_fe_dmd_dc2800_soft_reset(handle);

	if((handle->tuner_settings.tuner_type == TN_NXP_TDA18250) || (handle->tuner_settings.tuner_type == TN_NXP_TDA18250A))
	{
		handle->mt_sleep(30);
	}
	else
	{
		handle->mt_sleep(10);
	}
	return ret;
}

MT_FE_RET _mt_fe_dmd_dc2800_set_demod_appendix(MT_FE_DC2800_Device_Handle handle, U8 mode, U16 qam)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8 tmp;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if(mode == 0)			// DC2800 mode
	{
		handle->dmd_get_reg(handle, 0x00, &tmp);
		tmp &= 0x7F;
		handle->dmd_set_reg(handle, 0x00, tmp);

		handle->dmd_get_reg(handle, 0x03, &tmp);
		tmp &= 0xDF;
		tmp |= 0x10;
		handle->dmd_set_reg(handle, 0x03, tmp);

		handle->dmd_get_reg(handle, 0x04, &tmp);
		tmp |= 0x02;
		handle->dmd_set_reg(handle, 0x04, tmp);

		handle->dmd_get_reg(handle, 0x4C, &tmp);
		tmp |= 0x40;
		handle->dmd_set_reg(handle, 0x4C, tmp);

		handle->dmd_get_reg(handle, 0x5F, &tmp);
		tmp &= 0x3F;
		handle->dmd_set_reg(handle, 0x5F, tmp);

		handle->dmd_get_reg(handle, 0x72, &tmp);
		tmp &= 0x7F;
		handle->dmd_set_reg(handle, 0x72, tmp);

		handle->dmd_get_reg(handle, 0x89, &tmp);
		tmp &= 0x0F;
		handle->dmd_set_reg(handle, 0x89, tmp);

		handle->dmd_get_reg(handle, 0xE5, &tmp);
		tmp &= 0x7F;
		handle->dmd_set_reg(handle, 0xE5, tmp);

		handle->dmd_get_reg(handle, 0xE8, &tmp);
		tmp &= 0xFE;
		handle->dmd_set_reg(handle, 0xE8, tmp);

		if(qam == 256)
		{
			handle->dmd_set_reg(handle, 0x7F, 0x71);
			handle->dmd_set_reg(handle, 0xE8, 0x3B);
		}

		handle->dmd_get_reg(handle, 0xF9, &tmp);
		tmp &= 0xFA;
		tmp |= 0x02;
		handle->dmd_set_reg(handle, 0xF9, tmp);

		handle->dmd_get_reg(handle, 0xFE, &tmp);
		tmp |= 0x08;
		handle->dmd_set_reg(handle, 0xFE, tmp);


		if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) && (qam == 128))
		{
			handle->dmd_set_reg(handle, 0x2a, 0x24);
		}
		else if (handle->tuner_settings.tuner_type == TN_MXL_608)
		{
			if ((qam == 16) || (qam == 32))
			{
				handle->dmd_set_reg(handle, 0x2a, 0x2a);
			}
			else
			{
				handle->dmd_set_reg(handle, 0x2a, 0x2c);
			}
		}
		else
		{
			handle->dmd_set_reg(handle, 0x2a, 0x27);
		}
	}
	else if(mode == 1)		// Jazz mode
	{
		handle->dmd_get_reg(handle, 0x00, &tmp);
		tmp |= 0x80;
		handle->dmd_set_reg(handle, 0x00, tmp);

		handle->dmd_set_reg(handle, 0x01, 0x42);
		handle->dmd_set_reg(handle, 0x01, 0x62);

		if((qam == 256) || (qam == 128))
		{
			handle->dmd_set_reg(handle, 0x02, 0x10);
		}
		else
		{
			handle->dmd_set_reg(handle, 0x02, 0x60);
		}

		handle->dmd_get_reg(handle, 0x03, &tmp);
		tmp |= 0x30;
		handle->dmd_set_reg(handle, 0x03, tmp);

		handle->dmd_set_reg(handle, 0x04, 0x88);

		handle->dmd_set_reg(handle, 0x24, 0xFF);
		handle->dmd_set_reg(handle, 0x25, 0x00);
		handle->dmd_set_reg(handle, 0x26, 0xF9);
		handle->dmd_set_reg(handle, 0x27, 0x80);

		handle->dmd_get_reg(handle, 0x5F, &tmp);
		tmp &= 0x3F;
		handle->dmd_set_reg(handle, 0x5F, tmp);


		if(qam == 16)
			handle->dmd_set_reg(handle, 0x62, 0x18);
		else
			handle->dmd_set_reg(handle, 0x62, 0x28);

		handle->dmd_get_reg(handle, 0x00, &tmp);
		if( ((tmp & 0x70) == 0x20) || ((tmp & 0x70) == 0x10) || ((tmp & 0x70) == 0x00) )		// 128 QAM or 32QAM or 16QAM
		{
			handle->dmd_set_reg(handle, 0x6C, 0x52);
		}
		else
		{
			handle->dmd_set_reg(handle, 0x6C, 0x62);
		}

		handle->dmd_set_reg(handle, 0x6D, 0xA3);
		handle->dmd_set_reg(handle, 0x6F, 0x00);
		handle->dmd_set_reg(handle, 0x75, 0x18);
		handle->dmd_set_reg(handle, 0x7F, 0x71);

		handle->dmd_get_reg(handle, 0x93, &tmp);
		tmp &= 0x7F;
		handle->dmd_set_reg(handle, 0x93, tmp);

		handle->dmd_set_reg(handle, 0xE5, 0xA6);
		handle->dmd_set_reg(handle, 0xE8, 0x3B);
		handle->dmd_set_reg(handle, 0xF0, 0x83);
		handle->dmd_set_reg(handle, 0xF4, 0x0F);
		handle->dmd_get_reg(handle, 0xF9, &tmp);
		tmp &= 0xF8;
		tmp |= 0x02;
		handle->dmd_set_reg(handle, 0xF9, tmp);

		if ((handle->tuner_settings.tuner_type == TN_MXL_608))
		{
			if ((qam == 16) || (qam == 32))
			{
				handle->dmd_set_reg(handle, 0xFB, 0x24);
			}
			else
			{
				handle->dmd_set_reg(handle, 0xFB, 0x44);
			}
		}
		else
		{
			handle->dmd_set_reg(handle, 0xFB, 0x44);
		}

		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_set_reg(handle, 0xFC, 0xC1);
		}
		else
		{
			handle->dmd_set_reg(handle, 0xFC, 0x91);
		}
		handle->dmd_set_reg(handle, 0xFD, 0x13);
		handle->dmd_set_reg(handle, 0xFE, 0x50);

		if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) && (qam == 128))
		{
			handle->dmd_set_reg(handle, 0x2a, 0x24);
		}
		else if (handle->tuner_settings.tuner_type == TN_MXL_608)
		{
			if ((qam == 16) || (qam == 32))
			{
				handle->dmd_set_reg(handle, 0x2a, 0x2a);
			}
			else
			{
				handle->dmd_set_reg(handle, 0x2a, 0x2c);
			}
		}
		else
		{
			handle->dmd_set_reg(handle, 0x2a, 0x27);
		}

		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_get_reg(handle, 0x86, &tmp);
			tmp |= 0x08;
			handle->dmd_set_reg(handle, 0x86, tmp);
		}
	}
	else
	{
	}

	if((handle->tuner_settings.tuner_type == TN_NXP_TDA18250) || (handle->tuner_settings.tuner_type == TN_NXP_TDA18250A))
	{
		handle->dmd_set_reg(handle, 0x36, 0x81);
	}

	return ret;
}


/***********************************************
 Initialize the internal registers in M88DC2000
************************************************/
MT_FE_RET _mt_fe_dmd_dc2800_set_demod(MT_FE_DC2800_Device_Handle handle)
{
	U8 RegE3H, RegE4H;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dmd_set_reg(handle, 0x00, 0x48);
	handle->dmd_set_reg(handle, 0x01, 0x09);
	handle->dmd_set_reg(handle, 0xFB, 0x0A);
	if(handle->on_board_settings.chip_mode == 1)
	{
		handle->dmd_set_reg(handle, 0xF1, 0xC1);
	}
	else
	{
		handle->dmd_set_reg(handle, 0xFC, 0x0B);
	}
	handle->dmd_set_reg(handle, 0x02, 0x0B);
	handle->dmd_set_reg(handle, 0x03, 0x18);
	handle->dmd_set_reg(handle, 0x05, 0x0D);
	handle->dmd_set_reg(handle, 0x36, 0x80);
	handle->dmd_set_reg(handle, 0x43, 0x40);
	handle->dmd_set_reg(handle, 0x55, 0x7A);
	handle->dmd_set_reg(handle, 0x56, 0xD9);
	handle->dmd_set_reg(handle, 0x57, 0xDF);
	handle->dmd_set_reg(handle, 0x58, 0x39);
	handle->dmd_set_reg(handle, 0x5A, 0x00);
	handle->dmd_set_reg(handle, 0x5C, 0x71);
	handle->dmd_set_reg(handle, 0x5D, 0x23);

	if(handle->on_board_settings.chip_mode == 2)
	{
		handle->dmd_set_reg(handle, 0x86, 0x40);
	}

	handle->dmd_set_reg(handle, 0xF9, 0x08);
	handle->dmd_set_reg(handle, 0x61, 0x40);
	handle->dmd_set_reg(handle, 0x62, 0x0A);
	handle->dmd_set_reg(handle, 0x90, 0x06);
	handle->dmd_set_reg(handle, 0xDE, 0x00);
	handle->dmd_set_reg(handle, 0xA0, 0x03);
	handle->dmd_set_reg(handle, 0xDF, 0x81);
	handle->dmd_set_reg(handle, 0xFA, 0x40);
	handle->dmd_set_reg(handle, 0x37, 0x10);
	handle->dmd_set_reg(handle, 0xF0, 0x40);
	handle->dmd_set_reg(handle, 0xF2, 0x9C);
	handle->dmd_set_reg(handle, 0xF3, 0x40);

	handle->dmd_get_reg(handle, 0xE3, &RegE3H);
	handle->dmd_get_reg(handle, 0xE4, &RegE4H);

	if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800))
	{
		handle->dmd_set_reg(handle, 0x30, 0xD0);
	}
	else
	{
		handle->dmd_set_reg(handle, 0x30, 0xFF);
	}
	handle->dmd_set_reg(handle, 0x31, 0x00);
	handle->dmd_set_reg(handle, 0x32, 0x00);
	handle->dmd_set_reg(handle, 0x33, 0x00);
	handle->dmd_set_reg(handle, 0x35, 0x32);

	if (((RegE3H & 0xC0) == 0x00) && ((RegE4H & 0xC0) == 0x00))
	{
		handle->dmd_set_reg(handle, 0x40, 0x00);
		handle->dmd_set_reg(handle, 0x41, 0x10);
		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_set_reg(handle, 0xF1, 0xFF);
		}
		else
		{
			handle->dmd_set_reg(handle, 0xF1, 0x02);
		}
		handle->dmd_set_reg(handle, 0xF4, 0x04);
		handle->dmd_set_reg(handle, 0xF5, 0x00);
		handle->dmd_set_reg(handle, 0x42, 0x14);
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0xE1, 0x25);
		}
	}
	else if (((RegE3H & 0xC0) == 0x80) && ((RegE4H & 0xC0) == 0x40))
	{
		handle->dmd_set_reg(handle, 0x39, 0x00);
		if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800))
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);	// 0x60
		}
		else
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);
		}
		handle->dmd_set_reg(handle, 0x40, 0x00);
		handle->dmd_set_reg(handle, 0x41, 0x10);

		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_set_reg(handle, 0xF1, 0xFF);
		}
		else
		{
			handle->dmd_set_reg(handle, 0xF1, 0x00);
		}
		handle->dmd_set_reg(handle, 0xF4, 0x00);
		handle->dmd_set_reg(handle, 0xF5, 0x40);
		handle->dmd_set_reg(handle, 0x42, 0x14);
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0xE1, 0x25);
		}
	}
	else if (((RegE3H == 0x80) || (RegE3H == 0x81)) && ((RegE4H == 0x80) || (RegE4H == 0x81)))
	{
		handle->dmd_set_reg(handle, 0x39, 0x00);
		if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800))
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);	// 0x60
		}
		else
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);
		}

		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_set_reg(handle, 0xF1, 0xFF);
		}
		else
		{
			handle->dmd_set_reg(handle, 0xF1, 0x00);
		}
		handle->dmd_set_reg(handle, 0xF4, 0x00);
		handle->dmd_set_reg(handle, 0xF5, 0x40);
		handle->dmd_set_reg(handle, 0x42, 0x24);
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0xE1, 0x25);
		}

		handle->dmd_set_reg(handle, 0x92, 0x7F);
		handle->dmd_set_reg(handle, 0x93, 0x91);
		handle->dmd_set_reg(handle, 0x95, 0x00);
		handle->dmd_set_reg(handle, 0x2B, 0x33);
		handle->dmd_set_reg(handle, 0x2A, 0x2A);
		handle->dmd_set_reg(handle, 0x2E, 0x80);
		handle->dmd_set_reg(handle, 0x25, 0x25);
		handle->dmd_set_reg(handle, 0x2D, 0xFF);
		handle->dmd_set_reg(handle, 0x26, 0xFF);
		handle->dmd_set_reg(handle, 0x27, 0x00);
		handle->dmd_set_reg(handle, 0x24, 0x25);
		handle->dmd_set_reg(handle, 0xA4, 0xFF);
		handle->dmd_set_reg(handle, 0xA3, 0x0D);
	}
	else
	{
		handle->dmd_set_reg(handle, 0x39, 0x00);
		if((handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800) || (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800))
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);	// 0x60
		}
		else
		{
			handle->dmd_set_reg(handle, 0x3A, 0x00);
		}

		if(handle->on_board_settings.chip_mode == 1)
		{
			handle->dmd_set_reg(handle, 0xF1, 0xFF);
		}
		else
		{
			handle->dmd_set_reg(handle, 0xF1, 0x00);
		}
		handle->dmd_set_reg(handle, 0xF4, 0x00);
		handle->dmd_set_reg(handle, 0xF5, 0x40);
		handle->dmd_set_reg(handle, 0x42, 0x24);
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0xE1, 0x27);
		}

		handle->dmd_set_reg(handle, 0x92, 0x7F);
		handle->dmd_set_reg(handle, 0x93, 0x91);
		handle->dmd_set_reg(handle, 0x95, 0x00);
		handle->dmd_set_reg(handle, 0x2B, 0x33);
		handle->dmd_set_reg(handle, 0x2A, 0x2A);
		handle->dmd_set_reg(handle, 0x2E, 0x80);
		handle->dmd_set_reg(handle, 0x25, 0x25);
		handle->dmd_set_reg(handle, 0x2D, 0xFF);
		handle->dmd_set_reg(handle, 0x26, 0xFF);
		handle->dmd_set_reg(handle, 0x27, 0x00);
		handle->dmd_set_reg(handle, 0x24, 0x25);
		handle->dmd_set_reg(handle, 0xA4, 0xFF);
		handle->dmd_set_reg(handle, 0xA3, 0x10);
	}

	handle->dmd_set_reg(handle, 0xF6, 0x4E);
	handle->dmd_set_reg(handle, 0xF7, 0x20);
	handle->dmd_set_reg(handle, 0x89, 0x02);
	handle->dmd_set_reg(handle, 0x14, 0x08);
	handle->dmd_set_reg(handle, 0x6F, 0x0D);
	handle->dmd_set_reg(handle, 0x10, 0xFF);
	handle->dmd_set_reg(handle, 0x11, 0x00);
	handle->dmd_set_reg(handle, 0x12, 0x30);
	handle->dmd_set_reg(handle, 0x13, 0x23);
	handle->dmd_set_reg(handle, 0x60, 0x00);
	handle->dmd_set_reg(handle, 0x69, 0x00);
	handle->dmd_set_reg(handle, 0x6A, 0x03);
	handle->dmd_set_reg(handle, 0xE0, 0x75);

	if(handle->on_board_settings.chip_mode == 2)
	{
		handle->dmd_set_reg(handle, 0x8D, 0x29);
	}

	handle->dmd_set_reg(handle, 0x4E, 0xD8);
	handle->dmd_set_reg(handle, 0x88, 0x80);
	handle->dmd_set_reg(handle, 0x52, 0x79);
	handle->dmd_set_reg(handle, 0x53, 0x03);
	handle->dmd_set_reg(handle, 0x59, 0x30);
	handle->dmd_set_reg(handle, 0x5E, 0x02);
	handle->dmd_set_reg(handle, 0x5F, 0x0F);
	handle->dmd_set_reg(handle, 0x71, 0x03);
	handle->dmd_set_reg(handle, 0x72, 0x12);
	handle->dmd_set_reg(handle, 0x73, 0x12);

	return MtFeErr_Ok;
}

/***********************************************
   Set spectrum inversion and J83
   Inverted: 1, inverted; 0, not inverted
   J83: 0, J83A; 1, J83C
 ***********************************************/
MT_FE_RET _mt_fe_dmd_dc2800_set_tx_mode(MT_FE_DC2800_Device_Handle handle, U8 inverted, U8 j83)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8 value = 0;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if (inverted)	value |= 0x08;		/*	spectrum inverted	*/
	if (j83)		value |= 0x01;		/*	J83C				*/

	ret = handle->dmd_set_reg(handle, 0x83, value);

	return ret;
}

/***********************************************
   Set symbol rate
   sym:		symbol rate, unit: KBaud
   xtal:	unit, KHz
************************************************/
MT_FE_RET _mt_fe_dmd_dc2800_set_symbol_rate(MT_FE_DC2800_Device_Handle handle, U32 sym, U32 xtal)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8	value;
	U8	reg6FH, reg12H;
	U8	regE3H, regE4H;
	DB	fValue;
	U32	dwValue;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	fValue  = 4294967296.0 * (sym + 10) / xtal;
	dwValue = (U32)fValue;
	handle->dmd_set_reg(handle, 0x58, (U8)((dwValue >> 24) & 0xff));
	handle->dmd_set_reg(handle, 0x57, (U8)((dwValue >> 16) & 0xff));
	handle->dmd_set_reg(handle, 0x56, (U8)((dwValue >>  8) & 0xff));
	handle->dmd_set_reg(handle, 0x55, (U8)((dwValue >>  0) & 0xff));


	fValue = 2048.0 * xtal / sym;
	dwValue = (U32)fValue;
	handle->dmd_set_reg(handle, 0x5D, (U8)((dwValue >> 8) & 0xff));
	handle->dmd_set_reg(handle, 0x5C, (U8)((dwValue >> 0) & 0xff));


	handle->dmd_get_reg(handle, 0x5A, &value);
	if (((dwValue >> 16) & 0x0001) == 0)
		value &= 0x7F;
	else
		value |= 0x80;
	handle->dmd_set_reg(handle, 0x5A, value);


	handle->dmd_get_reg(handle, 0x89, &value);
	if (sym <= 1800)
		value |= 0x01;
	else
		value &= 0xFE;
	handle->dmd_set_reg(handle, 0x89, value);



	if (sym >= 6700)
	{
		reg6FH = 0x0D;
		reg12H = 0x30;
	}
	else if (sym >= 4000)
	{
		fValue = 22 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x30;
	}
	else if (sym >= 2000)
	{
		fValue = 14 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x20;
	}
	else
	{
		fValue = 7 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x10;
	}
	handle->dmd_set_reg(handle, 0x6F, reg6FH);
	handle->dmd_set_reg(handle, 0x12, reg12H);


	handle->dmd_get_reg(handle, 0xE3, &regE3H);
	handle->dmd_get_reg(handle, 0xE4, &regE4H);

	if (((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
	{
		if(sym < 3000)
		{
			handle->dmd_set_reg(handle, 0x6C, 0x16);
			handle->dmd_set_reg(handle, 0x6D, 0x10);
			handle->dmd_set_reg(handle, 0x6E, 0x18);
		}
		else
		{
			handle->dmd_set_reg(handle, 0x6C, 0x14);
			handle->dmd_set_reg(handle, 0x6D, 0x0E);
			handle->dmd_set_reg(handle, 0x6E, 0x36);
		}
	}
	else
	{
		handle->dmd_set_reg(handle, 0x6C, 0x16);
		handle->dmd_set_reg(handle, 0x6D, 0x10);
		handle->dmd_set_reg(handle, 0x6E, 0x18);
	}

	return ret;
}

/***********************************************
   Set the type of MPEG/TS interface
   type: 1, serial format; 2, parallel format; 0, common interface
************************************************/
MT_FE_RET _mt_fe_dmd_dc2800_set_ts_output(MT_FE_DC2800_Device_Handle handle, MT_FE_TS_OUT_MODE ts_mode)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8 regC2H;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if (ts_mode == MtFeTsOutMode_Common)
	{
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0x84, 0x6C);
		}
		handle->dmd_set_reg(handle, 0xC0, 0x43);
		handle->dmd_set_reg(handle, 0xE2, 0x06);
		handle->dmd_get_reg(handle, 0xC2, &regC2H);
		regC2H &= 0xC7;
		regC2H |= 0x10;
		handle->dmd_set_reg(handle, 0xC2, regC2H);
		handle->dmd_set_reg(handle, 0xC1, 0x60);		//common interface
	}
	else if (ts_mode == MtFeTsOutMode_Serial)
	{
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0x84, 0x6A);
		}
		handle->dmd_set_reg(handle, 0xC0, 0x47);		//serial format
		handle->dmd_set_reg(handle, 0xE2, 0x02);
		handle->dmd_get_reg(handle, 0xC2, &regC2H);
		regC2H &= 0xC7;
		handle->dmd_set_reg(handle, 0xC2, regC2H);
		handle->dmd_set_reg(handle, 0xC1, 0x00);
	}
	else//if (ts_mode == MtFeTsOutMode_Parallel)
	{
		if(handle->on_board_settings.chip_mode == 2)
		{
			handle->dmd_set_reg(handle, 0x84, 0x6C);
		}
		handle->dmd_set_reg(handle, 0xC0, 0x43);		//parallel format
		handle->dmd_set_reg(handle, 0xE2, 0x06);
		handle->dmd_get_reg(handle, 0xC2, &regC2H);
		regC2H &= 0xC7;
		handle->dmd_set_reg(handle, 0xC2, regC2H);
		handle->dmd_set_reg(handle, 0xC1, 0x00);
	}

	return ret;
}


/***********************************************
   Set QAM mode
   Qam: QAM mode, 16, 32, 64, 128, 256
************************************************/
MT_FE_RET _mt_fe_dmd_dc2800_set_QAM(MT_FE_DC2800_Device_Handle handle, U16 qam)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8 reg00H, reg4AH, reg4DH, reg8BH, reg8EH, regC2H, reg44H, reg4CH, reg74H;
	U8 regE3H, regE4H, value;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dmd_get_reg(handle, 0xC2, &regC2H);
	regC2H &= 0xF8;

	handle->dmd_get_reg(handle, 0xE3, &regE3H);
	handle->dmd_get_reg(handle, 0xE4, &regE4H);

	switch(qam)
	{
		case 16:	// 16 QAM
			reg00H = 0x08;
			reg4AH = 0x0F;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			handle->dmd_set_reg(handle, 0x6E, 0x18);
			break;

		case 32:	// 32 QAM
			reg00H = 0x18;
			reg4AH = 0xFB;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			handle->dmd_set_reg(handle, 0x6E, 0x18);
			break;

		case 64:	// 64 QAM
			reg00H = 0x48;
			reg4AH = 0xCD;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			break;

		case 128:	// 128 QAM
			reg00H = 0x28;
			reg4AH = 0xFF;
			regC2H |= 0x02;
			reg44H = 0xA9;
			reg4CH = 0x08;
			reg4DH = 0xF5;
			reg74H = 0x0E;
			reg8BH = 0x5B;
			reg8EH = 0x9D;
			break;

		case 256:	// 256 QAM
			if (((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80) && (STBLT_IMP != 0) && (handle->on_board_settings.chip_mode == 2))
			{
				handle->dmd_set_reg(handle, 0x90, 0x07);
				handle->dmd_set_reg(handle, 0xA3, 0x0B);	//0x10
			}
			reg00H = 0x38;
			reg4AH = 0xCD;
			if(((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
			{
				regC2H |= 0x02;
			}
			else
			{
				regC2H |= 0x01;
			}
			reg44H = 0xA9;
			reg4CH = 0x08;
			reg4DH = 0xF5;
			reg74H = ((STBLT_IMP != 0) && (handle->on_board_settings.chip_mode == 2)) ? 0x11 : 0x0E;
			reg8BH = 0x5B;
			reg8EH = 0x9D;
			break;

		default:	// 64 QAM
			reg00H = 0x48;
			reg4AH = 0xCD;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((regE3H & 0x80) == 0x80) && ((regE4H & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			break;
	}


	handle->dmd_set_reg(handle, 0x00, reg00H);

	handle->dmd_get_reg(handle, 0x88, &value);
	value |= 0x08;
	handle->dmd_set_reg(handle, 0x88, value);
	handle->dmd_set_reg(handle, 0x4B, 0xFF);
	handle->dmd_set_reg(handle, 0x4A, reg4AH);
	value &= 0xF7;
	handle->dmd_set_reg(handle, 0x88, value);

	handle->dmd_set_reg(handle, 0xC2, regC2H);
	handle->dmd_set_reg(handle, 0x44, reg44H);
	handle->dmd_set_reg(handle, 0x4C, reg4CH);
	handle->dmd_set_reg(handle, 0x4D, reg4DH);
	handle->dmd_set_reg(handle, 0x74, reg74H);

	if(handle->on_board_settings.chip_mode == 2)
	{
		handle->dmd_set_reg(handle, 0x8B, reg8BH);
		handle->dmd_set_reg(handle, 0x8E, reg8EH);
	}

	return ret;
}


/***********************************************
 Soft reset M88DC2000
 Reset the internal status of each funtion block,
 not reset the registers.
************************************************/
MT_FE_RET mt_fe_dmd_dc2800_soft_reset(MT_FE_DC2800_Device_Handle handle)
{
	MT_FE_RET ret = MtFeErr_Ok;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dmd_set_reg(handle, 0x80, 0x01);
	handle->dmd_set_reg(handle, 0x82, 0x00);
	handle->mt_sleep(1);
	handle->dmd_set_reg(handle, 0x80, 0x00);

	return ret;
}




MT_FE_RET mt_fe_dmd_dc2800_get_statistics(MT_FE_DC2800_Device_Handle handle,
										  U32 *agc_lock,
										  U32 *timing_lock,
										  U32 *dagc_lock,
										  U32 *carrier_lock,
										  U32 *sync_lock,
										  U32 *descrambler_lock,
										  U32 *chip_lock
										 )
{
	U8 tmp = 0;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dmd_get_reg(handle, 0x43, &tmp);
	*agc_lock = ((tmp & 0x08) == 0x08) ? 1 : 0;

	//*descrambler_lock = ((tmp & 0x10) == 0x10) ? 1 : 0;

	handle->dmd_get_reg(handle, 0x5E, &tmp);
	*timing_lock = ((tmp & 0x80) == 0x80) ? 1 : 0;

	handle->dmd_get_reg(handle, 0x74, &tmp);
	*dagc_lock = ((tmp & 0x80) == 0x80) ? 1 : 0;

	handle->dmd_get_reg(handle, 0xA3, &tmp);
	*carrier_lock = ((tmp & 0x80) == 0x80) ? 1 : 0;

	handle->dmd_get_reg(handle, 0x85, &tmp);
	*sync_lock = ((tmp & 0x10) == 0x10) ? 1 : 0;

	*descrambler_lock = ((tmp & 0x01) == 0x01) ? 1 : 0;

	handle->dmd_get_reg(handle, 0x85, &tmp);

	if(handle->on_board_settings.chip_mode != 1)	// 2: old DC2800	0: new DC2800
	{
		*chip_lock = ((tmp & 0x08) == 0x08) ? 1 : 0;
	}
	else				// 1: new Jazz
	{
		*chip_lock = ((tmp & 0x11) == 0x11) ? 1 : 0;
	}

	return MtFeErr_Ok;
}

/***********************************************
   Get lock status
   returned 1 when locked;0 when unlocked
************************************************/
MT_FE_RET mt_fe_dmd_dc2800_get_lock_state(MT_FE_DC2800_Device_Handle handle, MT_FE_LOCK_STATE *lock_status)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8 reg80H;

	*lock_status = MtFeLockState_Unlocked;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	handle->dmd_get_reg(handle, 0x80, &reg80H);

	if (reg80H < 0x06)
	{
		U8 regDFH, reg91H, reg43H;

		handle->dmd_get_reg(handle, 0xDF, &regDFH);
		handle->dmd_get_reg(handle, 0x91, &reg91H);
		handle->dmd_get_reg(handle, 0x43, &reg43H);

		if (((regDFH & 0x80) == 0x80) &&
			((reg91H & 0x23) == 0x03) &&
			((reg43H & 0x08) == 0x08)
		   )
		{
			*lock_status = MtFeLockState_Locked;
		}
		else
		{
			*lock_status = MtFeLockState_Unlocked;
		}
	}
	else
	{
		U8 reg85H;
		U8 i;

		for(i = 0; i < 10; i ++)
		{
			handle->dmd_get_reg(handle, 0x85, &reg85H);

			if(handle->on_board_settings.chip_mode != 1)
			{
				if((reg85H & 0x08) != 0x08)
				{
					*lock_status = MtFeLockState_Unlocked;
					break;
				}
			}
			else
			{
				if((reg85H & 0x11) != 0x11)
				{
					*lock_status = MtFeLockState_Unlocked;
					break;
				}
			}

			*lock_status = MtFeLockState_Locked;
		}
	}

	return ret;
}


/***********************************************
   Get BER (bit error rate)
************************************************/
MT_FE_RET mt_fe_dmd_dc2800_get_ber(MT_FE_DC2800_Device_Handle handle, U32 *error_bits, U32 *total_bits)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U16	tmp;

	U8 regA0H, regA1H, regA2H, n_byte;


	MT_FE_LOCK_STATE lock_status = MtFeLockState_Unlocked;


	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}


	mt_fe_dmd_dc2800_get_lock_state(handle, &lock_status);


	*error_bits = 0;
	*total_bits = 33554432;


	if (lock_status != MtFeLockState_Locked)
	{
		return MtFeErr_UnLock;
	}


	handle->dmd_get_reg(handle, 0xA0, &regA0H);
	n_byte = regA0H & 0x07;

	if ((regA0H & 0x80) != 0x80)
	{
		handle->dmd_get_reg(handle, 0xA1, &regA1H);
		handle->dmd_get_reg(handle, 0xA2, &regA2H);

		tmp = (regA2H << 8) + regA1H;

		*error_bits = tmp;
		*total_bits = 1 << (n_byte * 2 + 15);

		handle->dmd_set_reg(handle, 0xA0, n_byte);
		n_byte |= 0x80;
		handle->dmd_set_reg(handle, 0xA0, n_byte);
	}
	else
	{
		ret = MtFeErr_Fail;
	}


	return ret;
}

/***********************************************
   Get SNR (signal noise ratio)
************************************************/
MT_FE_RET mt_fe_dmd_dc2800_get_snr(MT_FE_DC2800_Device_Handle handle, U8 *signal_snr)
{
	MT_FE_RET ret = MtFeErr_Ok;

	const U32 mes_log[] =
	{
		0,		 3010,	 4771,	 6021,	 6990,	 7781,	 8451,	 9031,	 9542,	 10000,
		10414,	 10792,	 11139,	 11461,	 11761,	 12041,	 12304,	 12553,	 12788,	 13010,
		13222,	 13424,	 13617,	 13802,	 13979,	 14150,	 14314,	 14472,	 14624,	 14771,
		14914,	 15052,	 15185,	 15315,	 15441,	 15563,	 15682,	 15798,	 15911,	 16021,
		16128,	 16232,	 16335,	 16435,	 16532,	 16628,	 16721,	 16812,	 16902,	 16990,
		17076,	 17160,	 17243,	 17324,	 17404,	 17482,	 17559,	 17634,	 17709,	 17782,
		17853,	 17924,	 17993,	 18062,	 18129,	 18195,	 18261,	 18325,	 18388,	 18451,
		18513,	 18573,	 18633,	 18692,	 18751,	 18808,	 18865,	 18921,	 18976,	 19031
	};
	U32	snr;
	U8	i;
	U32	mse;


	U8 reg91H, reg08H, reg07H;

	U16 qam;


	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	qam = handle->channel_param.qam_code;

	*signal_snr = 0;

	handle->dmd_get_reg(handle, 0x91, &reg91H);

	if ((reg91H & 0x23) != 0x03)
		return MtFeErr_Fail;



	mse = 0;
	for (i = 0; i < 30; i ++)
	{
		handle->dmd_get_reg(handle, 0x08, &reg08H);
		handle->dmd_get_reg(handle, 0x07, &reg07H);

		mse += (reg08H << 8) + reg07H;
	}
	mse /= 30;
	if (mse > 80)
		mse = 80;



	switch (qam)
	{
		case 16:	snr = 34080;	break;	/*	16QAM	*/
		case 32:	snr = 37600;	break;	/*	32QAM	*/
		case 64:	snr = 40310;	break;	/*	64QAM	*/
		case 128:	snr = 43720;	break;	/*	128QAM	*/
		case 256:	snr = 46390;	break;	/*	256QAM	*/
		default:	snr = 40310;	break;
	}


	snr -= mes_log[mse-1];					/*	C - 10*log10(MSE)	*/
	snr /= 1000;
	if (snr > 0xff)
		snr = 0xff;


	*signal_snr = (U8)snr;

	return ret;
}

/***********************************************
   Get signal strength
************************************************/
MT_FE_RET mt_fe_dmd_dc2800_get_strength(MT_FE_DC2800_Device_Handle handle, U8 *signal_strength)
{
	MT_FE_RET ret = MtFeErr_Ok;

	U8	SignalStrength = 0;
	U8 reg43H, regE3H, regE4H;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	ret = handle->dmd_get_reg(handle, 0x43, &reg43H);
	ret = handle->dmd_get_reg(handle, 0xE3, &regE3H);
	ret = handle->dmd_get_reg(handle, 0xE4, &regE4H);

	if ((reg43H & 0x08) == 0x08)
	{
		if (((regE3H & 0xC0) == 0x00) && ((regE4H & 0xC0) == 0x00))
		{
			U8 reg55H, reg56H;

			ret = handle->dmd_get_reg(handle, 0x55, &reg55H);
			ret = handle->dmd_get_reg(handle, 0x56, &reg56H);

			SignalStrength = 255 - (reg55H >> 1) - (reg56H >> 1);
		}
		else
		{
			U8 reg3BH, reg3CH;

			ret = handle->dmd_get_reg(handle, 0x3B, &reg3BH);
			ret = handle->dmd_get_reg(handle, 0x3C, &reg3CH);

			SignalStrength = 255 - (reg3BH >> 1) - (reg3CH >> 1);
		}
	}

	#if 0
	if(handle->tuner_settings.tuner_type == TN_LG_TDCC)
	{
		if(SignalStrength > 159)
		{
			SignalStrength -= 159;
		}
		else
		{
			SignalStrength = 0;
		}
	}
	else if(handle->tuner_settings.tuner_type == TN_MONTAGE_TC2800)
	{
		U32 gain = 0, strength = 0;

		handle->tuner_settings.tuner_get_strength(handle, &gain, &strength);

		SignalStrength = (U8)strength;
	}
	#endif

	if (handle->tuner_settings.tuner_type == TN_MONTAGE_TC3800)
	{
		U32 gain = 0, strength = 0;

		handle->tuner_settings.tuner_get_strength(handle, &gain, &strength);

		SignalStrength = (U8)strength;
	}

	*signal_strength = SignalStrength;

	return ret;
}


MT_FE_RET mt_fe_dmd_dc2800_get_offset(MT_FE_DC2800_Device_Handle handle, S32 *freq_offset_KHz, S32 *symbol_rate_offset_KSs)
{
	MT_FE_RET ret = MtFeErr_Ok;

	S32 freq_offset = 0;
	U8 regA6H = 0, regA7H = 0, reg15H = 0, reg51H = 0;
	S32 offset_val = 0;

	if(handle == NULL)
	{
		return MtFeErr_Uninit;
	}

	if(handle->on_board_settings.chip_mode == 1)		// 0: new DC2800 bypass		or		1: new Jazz
	{
		ret = handle->dmd_get_reg(handle, 0xA6, &regA6H);
		ret = handle->dmd_get_reg(handle, 0xA7, &regA7H);

		offset_val = (regA7H << 8);
		offset_val += regA6H;

		if(offset_val > 32768)
		{
			offset_val -= 65536;
		}

		freq_offset = (offset_val * (S32)(handle->on_board_settings.xtal_KHz)) >> 16;
	}
	else
	{
		ret = handle->dmd_get_reg(handle, 0x15, &reg15H);

		if((reg15H & 0x80) == 0x80)
		{
			offset_val = reg15H - 256;
		}
		else
		{
			offset_val = reg15H;
		}

		freq_offset = offset_val * 7;
	}

	*freq_offset_KHz = freq_offset;


	handle->dmd_get_reg(handle, 0x51, &reg51H);

	if((reg51H & 0x80) == 0x80)
	{
		offset_val = reg51H - 256;
	}
	else
	{
		offset_val = reg51H;
	}


	*symbol_rate_offset_KSs = (S32)(offset_val * (S32)(handle->channel_param.sym_KSs) * 0.5 / 1000);


	return ret;
}


MT_FE_RET mt_fe_dmd_dc2800_get_symbol_rate(MT_FE_DC2800_Device_Handle handle, U16 *symbol_rate_KSs)
{
	U32 dwValue = 0;
	U8 tmp1, tmp2, tmp3, tmp4, tmp5;
	U16 sym = 0;
	S16 sym_offset = 0;
	S8 tmp6;
	U32 xtal = handle->on_board_settings.xtal_KHz;

	handle->dmd_get_reg(handle, 0x55, &tmp1);
	handle->dmd_get_reg(handle, 0x56, &tmp2);
	handle->dmd_get_reg(handle, 0x57, &tmp3);
	handle->dmd_get_reg(handle, 0x58, &tmp4);
	handle->dmd_get_reg(handle, 0x51, &tmp5);

	dwValue = (tmp4 << 24) + (tmp3 << 16) + (tmp2 << 8) + tmp1;

	sym = (dwValue >> 24) * (xtal >> 8) + (((dwValue >> 24) * (xtal % 256)) >> 8) + (((dwValue % 16777216) * (xtal >> 8)) >> 24) - 10;

	if(tmp5 >= 128)
		tmp6 = tmp5 - 256;
	else
		tmp6 = tmp5;

	sym_offset = tmp6 * sym * 5 / 10000;

	sym -= sym_offset;

	*symbol_rate_KSs = sym;

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dmd_dc2800_get_tuner_version(MT_FE_DC2800_Device_Handle handle, U32 *version_no, U32 *version_time)
{
	MT_FE_RET ret = MtFeErr_Ok;

	if(handle->tuner_settings.tuner_get_version != NULL)
	{
		ret = handle->tuner_settings.tuner_get_version(handle, version_no, version_time);
	}
	else
	{
		*version_no = 0;
		*version_time = 0;

		ret = MtFeErr_NoSupportFunc;
	}

	return ret;
}


MT_FE_RET mt_fe_dmd_dc2800_get_driver_version(U32 *version_no, U32 *version_time)
{
	*version_no		 = 10209;
	*version_time	 = 15101010;

	return MtFeErr_Ok;
}

