/*****************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd                                    */
/* MONTAGE PROPRIETARY AND CONFIDENTIAL                                      */
/* Copyright (c) 2014 Montage Technology Group Limited. All Rights Reserved. */
/*****************************************************************************/
/*****************************************************************************
 * Filename:        mt_fe_def.h
 *
 * Description:     Definitions of Jazz demodulator driver.
 *
 * Current version: 1.00.13
 *
 * History:
 * Description      Version     Date        Author
 * ---------------------------------------------------------------------------
 * Create           0.00.01     2012.02.01  YZ.Huang
 * Modify           1.00.00     2012.02.02  YZ.Huang
 * Modify           1.00.12     2012.04.27  YZ.Huang
 * Upgrade          1.00.13     2012.05.03  YZ.Huang
 *****************************************************************************/
#ifndef __MT_FE_DEF_H__
#define __MT_FE_DEF_H__

#include "mt_fe_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUNER_I2C_ADDR		0xC2	/* The tuner address is FIXED   */
#define M88DC2000_I2C_ADDR	0x38	/* M88DC2000 address            */
#define STBLT_IMP			0		/* 0: default ; 1:stability improvement*/

/*****************************************************************************/


typedef struct _MT_FE_DC2800_TS_OUTPUT_SETTING
{
	BOOL								active_edge_clk;		/*selects the active edge of pin M_CKOUT to update the MPEG-TS output.
																	0: active at falling edge		1: active at rising edge*/
	BOOL								polar_sync;				/*define the output polarity of pin M_SYNC
																	0: pin M_SYNC is active high ;	1: pin M_SYNC is active low*/
	BOOL								polar_val;				/*define the output polarity of pin M_VAL
																	0: pin M_VAL is active high ;	1: pin M_VAL is active low*/
	BOOL								polar_err;				/*define the output polarity of pin M_ERR
																	0: pin M_ERR is active high ;	1: pin M_ERR is active low*/
	MT_FE_TS_OUT_MODE					output_mode;			/*parallel interface, serial interface, common  interface*/
	MT_FE_TS_OUT_MAX_CLOCK				output_clock;
	U8									custom_duty_cycle;		/* User defined ducy cycle of output clock. tmp1 = nibble[7:4], tmp2 = nibble[3:0] */
																/* If enable auto adjusting function, this variable defines the minimum divide ratio */
	BOOL								serial_pin_select;		/*in the serial output mode only, select data output pin
																	0: select pin D0;	1: select pin D7*/
	BOOL								output_high_Z;			/*TS output high-impedance status
																	0: disable high-Z, normal output;	1: enable high-Z, no output*/
} MT_FE_DC2800_TS_OUTPUT_SETTINGS;


typedef struct _MT_FE_DTV_PARAMETER_DVBC
{
	U32		freq_KHz;
	U16		sym_KSs;
	U16		qam_code;
	BOOL	inverted;
	U32		param_reserved;
} MT_FE_DTV_PARAMETER_DVBC;

typedef enum _MT_FE_DC2800_CHIP_MODE
{
	DC2800_BYPASS	 = 0,
	DC2800_NEW		 = 1,
	DC2800_OLD		 = 2
} MT_FE_DC2800_CHIP_MODE;

typedef struct _MT_FE_DC2800_ON_BOARD_SETTINGS
{
	U8		chip_mode;			// 0: new DC2800        1: new Jazz        2: old DC2800
	BOOL	bSingleXTAL;
	BOOL	bReadCali;
	U8		iVppSel;			// 0: 2Vpp, 1: 1Vpp
	U8		iGainStep;			// 0: default, 1: FY
	U32		xtal_KHz;
	U32		chip_version;
	U32		on_board_reserved;
} MT_FE_DC2800_ON_BOARD_SETTINGS;


typedef enum _MT_FE_DC2800_SUPPORTED_TUNER
{
	MT_FE_TN_NOTSUPPORT	 = 0x00,
	TN_MONTAGE_TC2800,
	TN_MONTAGE_TC2000,
	TN_ALPS_TDAC,
	TN_THOMSON_DCT7070X,
	TN_XUGUANG_XDCT6A,
	TN_NXP_TDA18250,
	TN_LG_TDCC,
	TN_LG_TDCC_G1X1F,
	TN_MONTAGE_TC3800,
	TN_TDCCG051F,
	TN_RAFAEL_R836,
	TN_NXP_TDA18250A,
	TN_MXL_608,
	MT_FE_TN_USERDEFINE	 = 0xFF
} MT_FE_DC2800_SUPPORTED_TUNER;



typedef struct _MT_FE_DC2800_TN_SETTINGS
{
    BOOL                            tuner_init_OK;
    MT_FE_DC2800_SUPPORTED_TUNER    tuner_type;                        /* tuner type*/
    U8                                tuner_dev_addr;                    /* tuner device i2c address*/
    U8                                tuner_loopthrough; /*tuner loopthrough*/
    U8                                tuner_bandwidth; /*tuner_bandwidth*/
    U8                                tuner_mode;                        /* tuner mode if needed, 0: DVB-C, 2: MMDS */
    U8                                tuner_ver;
    U32                                tuner_handle;

    U32                                tuner_reserved;                    /* Reserved for future use*/

    MT_FE_RET (*tuner_init)(void *handle);                                /* tuner init function */
    MT_FE_RET (*tuner_set)(void *handle, U32 freq_KHz, U16 sym_KSs);    /* set tuner function */
    MT_FE_RET (*tuner_get_offset)(void *handle, S32 *freq_offset_KHz);    /* get frequency offset function */
    MT_FE_RET (*tuner_get_strength)(void *handle, U32 *p_gain, U32 *p_strength);        /* get signal strength function */
    MT_FE_RET (*tuner_get_version)(void *handle, U32 *version_no, U32 *version_time);    /* Get Tuner Version Number */
    MT_FE_RET (*tuner_sleep)(void *handle);                                /* Sleep the tuner */
    MT_FE_RET (*tuner_wakeup)(void *handle);                            /* Wakeup the tuner */
} MT_FE_DC2800_TN_SETTINGS, *MT_FE_DC2800_Tuner_Handle;


typedef struct _MT_FE_DC2800_SETTINGS
{
    U8                                dev_index;                    /*index*/
    U8                                demod_dev_addr;                /*demodulator device i2c addres*/
    MT_FE_SUPPORTED_DEMOD            demod_type;                    /*demod type*/
    MT_FE_TYPE                        dtv_mode;                    /*dtv mode*/
    MT_FE_DTV_PARAMETER_DVBC        channel_param;
    MT_FE_DC2800_TN_SETTINGS        tuner_settings;                /*tuner settings*/
    MT_FE_DC2800_ON_BOARD_SETTINGS    on_board_settings;
    MT_FE_DC2800_TS_OUTPUT_SETTINGS    ts_output_settings;

    MT_FE_RET    (*dmd_set_reg)(void *handle, U8 reg_index, U8 reg_value);
    MT_FE_RET    (*dmd_get_reg)(void *handle, U8 reg_index, U8 *p_buf);
    MT_FE_RET    (*write_fw)(void *handle, U8 reg_index, U8 *p_buf, U16 n_byte);
    void        (*mt_sleep)(U32 ticks_ms);
    MT_FE_RET    (*tn_set_reg)(void *handle, U8 reg_index, U8 reg_value);
    MT_FE_RET    (*tn_get_reg)(void *handle, U8 reg_index, U8 *p_buf);
    MT_FE_RET    (*tn_write)(void *handle, U8 *p_buf, U16 n_byte);
    MT_FE_RET    (*tn_read)(void *handle, U8 *p_buf, U16 n_byte);
	MT_FE_RET	(*Set32Bits)(U32 reg_addr, U32 reg_data);
	MT_FE_RET	(*Get32Bits)(U32 reg_addr, U32 *p_data);
} MT_FE_DC2800_SETTINGS, *MT_FE_DC2800_Device_Handle;

/***************************************************************************
Summary:
front-end tuning status
***************************************************************************/
/* Demodulator APIs */
/*Internal*/
MT_FE_RET _mt_fe_dmd_dc2800_set_demod(MT_FE_DC2800_Device_Handle handle);
MT_FE_RET _mt_fe_dmd_dc2800_set_demod_appendix(MT_FE_DC2800_Device_Handle handle, U8 mode, U16 qam);
MT_FE_RET _mt_fe_dmd_dc2800_set_symbol_rate(MT_FE_DC2800_Device_Handle handle, U32 sym, U32 xtal);
MT_FE_RET _mt_fe_dmd_dc2800_set_QAM(MT_FE_DC2800_Device_Handle handle, U16 qam);
MT_FE_RET _mt_fe_dmd_dc2800_set_tx_mode(MT_FE_DC2800_Device_Handle handle, U8 inverted, U8 j83);
MT_FE_RET _mt_fe_dmd_dc2800_set_ts_output(MT_FE_DC2800_Device_Handle handle, MT_FE_TS_OUT_MODE ts_mode);


/*Public*/
MT_FE_RET mt_fe_dmd_dc2800_config_default(MT_FE_DC2800_Device_Handle handle);
MT_FE_RET mt_fe_dmd_dc2800_select_tuner(MT_FE_DC2800_Device_Handle handle, MT_FE_DC2800_SUPPORTED_TUNER tuner_type);
MT_FE_RET mt_fe_dmd_dc2800_config_tuner_settings(MT_FE_DC2800_Device_Handle handle, MT_FE_DC2800_TN_SETTINGS *tuner_config);
MT_FE_RET mt_fe_dmd_dc2800_calibration(MT_FE_DC2800_Device_Handle handle, U8 iMaxTimes);
MT_FE_RET mt_fe_dmd_dc2800_init(MT_FE_DC2800_Device_Handle handle);
MT_FE_RET mt_fe_dmd_dc2800_connect(MT_FE_DC2800_Device_Handle handle, U32 freq_KHz, U16 symbol_rate_KSs, U16 qam, U8 inverted);
MT_FE_RET mt_fe_dmd_dc2800_soft_reset(MT_FE_DC2800_Device_Handle handle);
MT_FE_RET mt_fe_dmd_dc2800_get_lock_state(MT_FE_DC2800_Device_Handle handle, MT_FE_LOCK_STATE *lock_status);
MT_FE_RET mt_fe_dmd_dc2800_get_ber(MT_FE_DC2800_Device_Handle handle, U32 *error_bits, U32 *total_bits);
MT_FE_RET mt_fe_dmd_dc2800_get_snr(MT_FE_DC2800_Device_Handle handle, U8 *signal_snr);
MT_FE_RET mt_fe_dmd_dc2800_get_strength(MT_FE_DC2800_Device_Handle handle, U8 *signal_strength);
MT_FE_RET mt_fe_dmd_dc2800_get_offset(MT_FE_DC2800_Device_Handle handle, S32 *freq_offset_KHz, S32 *symbol_rate_offset_KSs);
MT_FE_RET mt_fe_dmd_dc2800_get_symbol_rate(MT_FE_DC2800_Device_Handle handle, U16 *symbol_rate_KSs);
MT_FE_RET mt_fe_dmd_dc2800_get_tuner_version(MT_FE_DC2800_Device_Handle handle, U32 *version_no, U32 *version_time);
MT_FE_RET mt_fe_dmd_dc2800_get_driver_version(U32 *version_no, U32 *version_time);
MT_FE_RET mt_fe_dmd_dc2800_get_statistics(MT_FE_DC2800_Device_Handle handle, U32 *agc_lock, U32 *timing_lock,
		  U32 *dagc_lock, U32 *carrier_lock, U32 *sync_lock, U32 *descrambler_lock, U32 *chip_lock);


/*Supported tuner APIs*/
MT_FE_RET mt_fe_dc2800_tn_get_version_default(void *dev_handle, U32 *version_no, U32 *version_time);
/*Montage TC2800*/
MT_FE_RET mt_fe_dc2800_tn_init_tc2800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_tc2800(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_get_offset_tc2800(void *dev_handle, S32 *freq_offset_KHz);
MT_FE_RET mt_fe_dc2800_tn_get_strength_tc2800(void *dev_handle, U32 *p_gain, U32 *p_strength);
MT_FE_RET mt_fe_dc2800_tn_sleep_tc2800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_wakeup_tc2800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_get_version_tc2800(void *dev_handle, U32 *version_no, U32 *version_time);


/*ALPS TDAC*/
MT_FE_RET mt_fe_dc2800_tn_init_ALPS_TDAC(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_ALPS_TDAC(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);

/*XuGuang XDCT 6A*/
MT_FE_RET mt_fe_dc2800_tn_init_XuGuang_XDCT6A(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_XuGuang_XDCT6A(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);

/*MXL 608*/
MT_FE_RET mt_fe_dc2800_tn_init_MXL608(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_MXL608(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);

/*NXT TDA 18250*/
MT_FE_RET mt_fe_dc2800_tn_init_TDA18250(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_TDA18250(void * dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_get_strength_TDA18250(void * dev_handle, U32 * p_gain, U32 * p_strength);
MT_FE_RET mt_fe_dc2800_tn_get_version_TDA18250(void *dev_handle, U32 *version_no, U32 *version_time);

/*NXT TDA 18250A*/
MT_FE_RET mt_fe_dc2800_tn_init_TDA18250A(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_TDA18250A(void * dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_get_strength_TDA18250A(void * dev_handle, U32 * p_gain, U32 * p_strength);
MT_FE_RET mt_fe_dc2800_tn_set_io_TDA18250A(void *dev_handle, BOOL onoff);

/*LG TDCC G0X1F*/
MT_FE_RET mt_fe_dc2800_tn_init_LG_TDCC(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_LG_TDCC(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);

/*LG TDCC G1X1F*/
MT_FE_RET mt_fe_dc2800_tn_init_LG_TDCC_G1X1F(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_LG_TDCC_G1X1F(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
/*  TDCC_G051F    */
MT_FE_RET mt_fe_dc2800_tn_init_TDCC_G051F(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_TDCC_G051F(void *dev_handle, U32 freq, U16 symbol_rate);
MT_FE_RET mt_fe_dc2800_tn_get_strength_TDCC_G051F(void *dev_handle, U32 *p_gain, U32 *p_strength);

/*Montage TC3800*/
MT_FE_RET mt_fe_dc2800_tn_init_tc3800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_tc3800(void *dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_get_offset_tc3800(void *dev_handle, S32 *freq_offset_KHz);
MT_FE_RET mt_fe_dc2800_tn_get_strength_tc3800(void *dev_handle, U32 *p_gain, U32 *p_strength);
MT_FE_RET mt_fe_dc2800_tn_sleep_tc3800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_wakeup_tc3800(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_get_version_tc3800(void *dev_handle, U32 *version_no, U32 *version_time);

/*RAFAEL R836*/
MT_FE_RET mt_fe_dc2800_tn_init_R836(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_set_freq_R836(void * dev_handle, U32 freq_KHz, U16 symbol_rate_KSs);
MT_FE_RET mt_fe_dc2800_tn_sleep_R836(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_wakeup_R836(void *dev_handle);
MT_FE_RET mt_fe_dc2800_tn_get_strength_R836(void * dev_handle, U32 * p_gain, U32 * p_strength);
MT_FE_RET mt_fe_dc2800_tn_get_version_R836(void *dev_handle, U32 *version_no, U32 *version_time);

MT_FE_DC2800_SUPPORTED_TUNER mt_fe_dc2800_check_tuner_type(void *dev_handle);

#ifdef __cplusplus
}
#endif


#endif /* __MT_FE_DEF_H__ */
