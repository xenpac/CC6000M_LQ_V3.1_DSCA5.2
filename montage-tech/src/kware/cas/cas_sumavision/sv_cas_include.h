/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SV_CAS_INCLUDE_H__
#define __SV_CAS_INCLUDE_H__

#define SVCAS_MAX_DISPLAY_EMAIL_NUM (50)

typedef struct
{
	/*!
		The callback of nvram reading function
	*/
	RET_CODE (*nvram_read)(u32 offset, u8 *p_buf, u32 *size);
	/*!
		The callback of nvram writing function
	*/
	RET_CODE (*nvram_write)(u32 offset, u8 *p_buf, u32 size);
	/*!
    	The callback of machine serial number getting
    */
  	RET_CODE (*machine_serial_get)(u8 *p_buf, u32 size);
	/*!
		The callback of software version of STB
	*/
	RET_CODE (*software_version_get)(u8 *p_buf, u32 size);
	/*!
		flash_start_address
	*/
	u32 flash_start_add;
	/*!
		flash_size
	*/
	u32 flash_size;
	/*!
		video pid
	*/
	u16 v_pid;
	/*!
		audio pid
	*/
  	u16 a_pid;
	/*!
		NIT PDSD data
	*/
	u32 private_data_specifier;
	/*!
		task priority
    */
    u32 task_prio;
	/*!
	    slot id
	*/
	u8 slot;
	/*!
	    smart card status
	*/
    u8 card_status;
}s_cas_sv_priv_t;

typedef enum
{
/*!
    ABC
  */
  SC_REMOVE_START,
  /*!
    ABC
  */
  SC_REMOVE_OVER,
  /*!
    ABC
  */
  SC_INSERT_START,
    /*!
    ABC
  */
  SC_INSERT_OVER,
  /*!
    ABC
  */
  SC_STATUS_END
}e_sv_smartcard_status_t;

typedef struct _s_urgency_broadcast_param
{
    u16 wOriNetID;
    u16 wTSID;
    u16 wServiceID;
    u32 wDuration;
}s_urgency_broadcast_param_t;
#endif

