/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __CAS_PORTING_DRV_DEMUX_H__
#define __CAS_PORTING_DRV_DEMUX_H__

#define CAS_FILTER_DEPTH_SIZE    8

#define CAS_FILTER_MAX_NUM        10
#define CAS_FILTER_BUFFER_SIZE  (16 * (KBYTES) + 188)
#define CAS_DATA_BUFFER_SIZE     (1024)
#define CAS_SECTION_TASK_STACK_SIZE     (16 * (KBYTES))

typedef enum _e_cas_filter_status
{
    CAS_FILTER_STATUS_FREE,
    CAS_FILTER_STATUS_BUSY
}e_cas_filter_status_t;

typedef struct _s_cas_filter_struct
{
    e_cas_filter_status_t filter_status;
    u8 once_filter_flag;
    u8 req_id;
    u16 pid;
    u8 filter_data[CAS_FILTER_DEPTH_SIZE];
    u8 filter_mask[CAS_FILTER_DEPTH_SIZE];
    u8 mask_len;
    u16 req_handle;
    u32 start_ms;
    u32 req_timeout;
    u8 *p_buf;
}s_cas_filter_struct_t;

typedef struct _s_cas_data_got
{
    u8 req_id;
    u8 timeout;
    u16 pid;
    u8 *p_data;
    u16 length;
}s_cas_data_got_t;

void cas_porting_drv_dmx_init(u32 monitor_pri,void (*pFun)(u8));

extern RET_CODE cas_porting_drv_dmx_set_filter( u8        byReqID,  
											   const u8* pbyFilter,  
											   const u8* pbyMask, 
											   u8        byLen, 
											   u16       wPid, 
											   u8        byWaitSeconds,
											   u8        once_filter_flag);

extern void cas_porting_drv_dmx_release_filter( u8  byReqID, u16 wPid );

extern void cas_porting_drv_dmx_release_once_filter( void );

extern void cas_porting_drv_data_monitor(void *pData);

extern RET_CODE cas_porting_drv_dmx_filter_free(s_cas_filter_struct_t *filter);

extern RET_CODE cas_porting_drv_dmx_get_chanid_bypid(u16 pid, u16 *p_channel);

extern RET_CODE cas_porting_drv_dmx_descrambler_onoff(u16 channel, BOOL enable);

extern RET_CODE cas_porting_drv_dmx_descrambler_set_even_keys(u16 channel, u8 *p_key, s32 key_length);

extern RET_CODE cas_porting_drv_dmx_descrambler_set_odd_keys(u16 channel, u8 *p_key, s32 key_length);
#endif
