/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DS_CAS_INCLUDE_H__
#define __DS_CAS_INCLUDE_H__

#define DSCAS_MAX_DISPLAY_EMAIL_NUM (50)

#define KBYTES    1024

#define DS_COMMON_STACK_SIZE (8 * KBYTES) 

#define DSCA_TASK_PRIO  (35) 
#define DSCA_SECTION_TASK_PRIO  (45)   
#define DSCA_SECTION_TASK_STACK_SIZE   (16 * 1024)

#define DS_CAS_TASK_NUM (8)
#define DS_TASK_PRIORITY_0 (35)
#define DS_FILTER_MAX_NUM (10)
#define DS_FILTER_DEPTH_SIZE (8)
#define DS_FILTER_BUFFER_SIZE (16 * (KBYTES) + 188)
#define DS_DATA_BUFFER_SIZE (1024)
#define DS_CAS_SECTION_TASK_PRIO        49


#define DS_REST_OK                    1
#define DS_REST_ERROR              0


#define NIT_TABLE_ID  0x40

#define NIT_PID  0x10

/*!
  Basic type defines
  */
#define U8        unsigned char
/*!
  Basic type defines
  */
#define U16        unsigned short
/*!
  Basic type defines
  */
#define U32        unsigned long


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
		The callback of query program check
	*/
	RET_CODE (*query_check)(u16 net_id, u16 ts_id, u16 sev_id);
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
		task priority
    */
    u32 task_prio_monitor;
	/*!
	    slot id
	*/
	u8 slot;
	/*!
	    smart card status
	*/
    u8 card_status;
}s_cas_ds_priv_t;

typedef enum
{
/*!
    SUCESS 0
  */
  DS_TASK_SUCESS = 0,
  /*!
    FAIL 1
  */
  DS_TASK_FAIL,

}e_ds_task_status_t;

typedef enum _e_ds_filter_status
{
    DS_FILTER_STATUS_FREE,
    DS_FILTER_STATUS_BUSY
}e_ds_filter_status_t;

typedef enum _e_ds_program_status
{
    DS_NOPROGRAM = 0,
    DS_PROGRAM_EXIST
}e_ds_program_t;

typedef enum _e_ds_filterset_status
{
    DS_FILTER_SET_FAIL = 0,
    DS_FILTER_SET_OK
}e_ds_filter_set_t;

typedef enum _e_ds_queue_init
{
    DS_QUEUE_INIT_FAIL = 0,
    DS_QUEUE_INIT_SUCCESS
}e_ds_queue_init_t;

typedef enum _e_ds_queue_get
{
    DS_QUEUE_GET_SUCCESS = 0,
    DS_QUEUE_GET_FAIL 
}e_ds_queue_get_t;

typedef enum _e_ds_queue_send
{
    DS_QUEUE_SEND_SUCESS = 0,
    DS_QUEUE_SEND_FAIL    
}e_ds_queue_send_t;


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
}e_ds_smartcard_status_t;

typedef struct _ds_ca_desc_s
{
    u32   ca_system_id        :16;
    u32   reserve             :3;
    u32   ca_pid              :13;
}ds_ca_desc_t;


#endif

