/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _ADS_DESAI_INCLUDE_H_
#define _ADS_DESAI_INCLUDE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
  The ads desai priv
  */
typedef struct
{
/*!    
    module id
    */
  u8 id;
/*!
    Platform type
  */
    ads_platform_type_t platform_type;
/*!    
    module version
    */
  u8 *version;
/*!    
    AD URL (for VC type is u8*)
    */
  u8 *p_ad_url;
/*!    
    welcome ad showtime
    */
  u32 show_time;
/*!    
    task priority start, if is 0, means no task needed
  */
  u8 task_prio_start;
/*!    
    task priorityend, if is 0, means no task needed
  */
  u8 task_prio_end;
/*!    
   flash_start_adr
    */
  u32 flash_start_adr;  
/*!    
   flash_size
    */
  u32 flash_size;
/*!    
   Channel_frequency
  */
  u32 channel_frequency;  
/*!    
   Channel_symbolrate
  */
  u32 channel_symbolrate;
/*!    
   Qam
  */
  u16 channel_qam;
/*!    
    The handle of demux device
    */
  void *p_dmx_dev;
/*!    
    The handle of demux device  ads_pic_in_t
    */
  u8 pic_in;
/*!
   The begin message
  */
  u32 msg_to_ui_start;
/*!
   The end message
  */
  u32 msg_to_ui_end;
/*!    
    The callback of nvram reading function
    */
  RET_CODE (*nvram_read)(u32 offset, u8 *p_buf, u32 *size);
/*!    
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(u32 offset, u8 *p_buf, u32 size);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase)(u32 length);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase2)(u32 offset, u32 length);
/*!    
    The callback of read gif data function
    */
  RET_CODE (*read_gif)(u32 length, u8 *p_data);
/*!    
    The callback of read float data function
    */
  RET_CODE (*read_float)(u32 floatnum, u8 *p_playsize, u16 *p_contentlen, u8 *p_data);
/*!    
    The callback of read rec function
    */
  RET_CODE (*read_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);
/*!    
    The callback of write rec function
    */
  RET_CODE (*write_rec)(u32 x, u32 y, u32 width, u32 height, u8 *p_dest);

/*!    
    The callback of get current data
    */
  RET_CODE (*get_date)(u32 *date);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_time)(u32 *time);
/*!    
    The callback of get current time
    */
  RET_CODE (*get_channel_info)(u16 *tsid, u16 *sid, u16 *nid);
/*!    
    The callback of read ads(pic | text) data function
    */
  RET_CODE (*read_data)(void *priv);
/*!    
    The notify message to UI
    pay attention, the parameter content will not be changed
    */  
  RET_CODE (*notify_msg_to_ui)(BOOL is_sync, u16 content, u32 para1, u32 para2);
}ads_desai_priv_t;


#ifdef __cplusplus
}
#endif //__cplusplus
#endif//_ADS_DESAI_INCLUDE_H_