/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _KVdb_demux_H__
#define _KVdb_demux_H__

#define KDVB_PANEL_FILETER_1 1
#define KDVB_PANEL_FILETER_2 2
#define KDVB_PANEL_FILETER_3 3
#define KDVB_PANEL_FILETER_4 4

#define KDVB_FILTER_DEPTH_SIZE 16
typedef enum
{
  FILTER_FREE,
  FILTER_BUSY,
  
}kdvb_filter_status_t;

typedef RET_CODE (*kvdb_dmx_event_notify)(struct dmx_device *dev, u32 filter_id, u8 * data_got, u32 data_size);


  typedef struct
  {
  
  BOOL  started;
  /*!
  abc
    */
  kdvb_filter_status_t  m_FilterStatus;
  /*!
  abc
    */
  u8  m_byReqId;
  /*!
  abc
    */
  u16 m_PID;
  /*!
  abc
    */
  u8  m_uFilterData[KDVB_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u8  m_uFilterMask[KDVB_FILTER_DEPTH_SIZE];
  /*!
  abc
    */
  u16 m_dwReqHandle;
  /*!
  abc
    */
  u32 m_dwStartMS;
  /*!
  abc
    */
  u32 m_timeout;
  /*!
  abc
    */ 
   kvdb_dmx_event_notify call_back;
  /*
  abc
  */
  u8  *p_buf;
  }kdvb_filter_struct_t;




unsigned int kvdb_porting_filter_open(unsigned short PID,unsigned char tableID,unsigned short ext);

int kvdb_porting_filter_close(unsigned int slot);

int kvdb_porting_filter_recv(unsigned int slot, unsigned char *buffer, int max_len);

void kvdb_porting_demux_init(void);

void kvdb_porting_demux_exit(void);

#endif

