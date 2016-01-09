/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __CDCAS_INCLUDE_H__
#define __CDCAS_INCLUDE_H__

/*!
    ABC
  */
#define YXSB_CAS_TASK_NUM           6

#define YXSB_TASK_PRIORITY_0      32
#define YXSB_CAS_SECTION_TASK_PRIO        49
#define YXSB_SECTION_TASK_STACK_SIZE (4 * (KBYTES))
#define YXSB_FILTER_DEPTH_SIZE    8
#define YXSB_FILTER_MAX_NUM        8
#define YXSB_FILTER_BUFFER_SIZE  (16 * (KBYTES) + 188)
#define YXSB_DATA_BUFFER_SIZE     (1024)

#define NTSM_OK                0x0000
#define NTSM_ERR_UNSUPPORT          0x9100
#define NTSM_ERR_INS            0x9200
#define NTSM_ERR_TIMEOVER          0x9300

typedef struct
{
    u32   ca_system_id        :16;
    u32   reserve             :3;
    u32   ca_pid              :13;
}yxsb_cas_desc_t;

typedef struct
{
    u16 a_pid;
    yxsb_cas_desc_t a_cas_desc;
}audio_desc_t;

typedef struct
{
    u16 v_pid;
    yxsb_cas_desc_t v_cas_desc;
}video_desc_t;

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
    	flash_start_address of block A
    */
    u32 flash_start_add_A;
    /*!
    	flash_size of block A
    */
    u32 flash_size_A;
    /*!
    	flash_start_address of block B
    */
    u32 flash_start_add_B;
    /*!
    	flash_size of block B
    */
    u32 flash_size_B;
    /*!
    	Network ID of the NIT
    */
    u32 network_id;
    /*!
    	video pid
    */
    video_desc_t vedio_desc;
    /*!
    	audio pid
    */
    audio_desc_t audio_desc;
    /*!
        ecm pid
    */
    yxsb_cas_desc_t cas_desc;
    /*!
        program id
    */
    u16 program_id;
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
}s_cas_yxsb_priv_t;

typedef struct _cas_yxsb_time
{
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
}cas_yxsb_time_t;

#endif

