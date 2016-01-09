/*****************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd                                    */
/* MONTAGE PROPRIETARY AND CONFIDENTIAL                                      */
/* Copyright (c) 2014 Montage Technology Group Limited. All Rights Reserved. */
/*****************************************************************************/

//*******      V3.00.30     *********

// mt_fe_tn_tc2800.h: interface for the M88TC2800.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _M88TC2800_H
#define  _M88TC2800_H

#include "mt_fe_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* M88TC2800                                                            */
/************************************************************************/

#if 0
#if 1
#define	U8	unsigned char							/* 8bit unsigned					*/
#define	S8	signed char								/* 8bit unsigned					*/
#define	U16	unsigned short							/* 16bit unsigned					*/
#define	S16	signed short							/* 16bit unsigned					*/
#define	U32	unsigned int							/* 32bit unsigned					*/
#define	S32	signed int								/* 16bit unsigned					*/
#define	DB	double
#else
typedef	unsigned char	U8							/* 8bit unsigned					*/
typedef	unsigned char	S8							/* 8bit unsigned					*/
typedef	unsigned short	U16							/* 16bit unsigned					*/
typedef	signed short	S16							/* 16bit unsigned					*/
typedef	unsigned int	U32							/* 32bit unsigned					*/
typedef	signed int		S32							/* 16bit unsigned					*/
typedef	double			DB
#endif
#endif

#define  MT_TC2800_CUSTOM_CONFIG	0


#define  TUNER_I2C_ADDR_TC2800		0xc2


typedef struct _MT_FE_TN_TC2800_Settings
{
	U8		tuner_init_OK;			// Tuner initialize status
	U8		tuner_dev_addr;			// Tuner device address

	U32		tuner_freq;				// RF frequency to be set, unit: KHz
	U16		tuner_qam;				// Reserved
	S16		tuner_mode;				// DTV mode, 0: DVB-C, 1: CTTB, 2: MMDS

	U8		tuner_bandwidth;		// Bandwidth of the channel, unit: MHz, 6/7/8

	U8		tuner_loopthrough;		// Tuner loop through switch, 0/1

	U32		tuner_crystal;			// Tuner crystal frequency, unit: KHz
	U32		tuner_dac;				// Tuner DAC frequency, unit: KHz
	S16		tuner_offset_KHz;

	U16		tuner_mtt;				// Montage tuner

	U16		tuner_custom_cfg;		// Tuner user config
	U32		tuner_version;			// Tuner driver version number
	U32		tuner_time;				// Tuner driver update time
} MT_FE_TN_TC2800_SETTINGS, *MT_FE_TN_TC2800_Handle;


//extern S32 _mt_fe_tn_get_reg(MT_FE_DC2800_Device_Handle handle, U8 reg_addr, U8 *reg_data);
//extern S32 _mt_fe_tn_set_reg(MT_FE_DC2800_Device_Handle handle, U8 reg_addr, U8 reg_data);
extern void _mt_sleep(U32 ticks_ms);

void _mt_fe_tn_set_RF_front_tc2800(MT_FE_DC2800_Device_Handle handle);
void _mt_fe_tn_set_PLL_freq_tc2800(MT_FE_DC2800_Device_Handle handle);
void _mt_fe_tn_set_BB_tc2800(MT_FE_DC2800_Device_Handle handle);
void _mt_fe_tn_set_DAC_tc2800(MT_FE_DC2800_Device_Handle handle);
void _mt_fe_tn_preset_tc2800(MT_FE_DC2800_Device_Handle handle);
void _mt_fe_tn_set_appendix_tc2800(MT_FE_DC2800_Device_Handle handle);

void mt_fe_tn_wakeup_tc2800(MT_FE_DC2800_Device_Handle handle);
void mt_fe_tn_sleep_tc2800(MT_FE_DC2800_Device_Handle handle);

void mt_fe_tn_init_tc2800(MT_FE_DC2800_Device_Handle handle);
void mt_fe_tn_set_freq_tc2800(MT_FE_DC2800_Device_Handle handle, U32 Freq_KHz);

void mt_fe_tn_set_BB_filter_band_tc2800(MT_FE_DC2800_Device_Handle handle, U8 bandwidth);

double mt_fe_tn_get_signal_strength_tc2800(MT_FE_DC2800_Device_Handle handle);

void mt_fe_tn_get_diagnose_info(MT_FE_DC2800_Device_Handle handle, U32* data1, U32* data2);

#ifdef __cplusplus
}
#endif

#endif
