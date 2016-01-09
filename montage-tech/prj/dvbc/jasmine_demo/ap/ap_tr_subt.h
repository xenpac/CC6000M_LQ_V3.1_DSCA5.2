/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

/*!
  subt policy
  */
typedef struct
{
  /*!
    on initialized
    */
  void (*on_init)(void);
} subt_policy_t;


typedef struct
{
  u32 x;
  u32 y;
  u32 width;
  u32 height;
  u8 *source;
  u8 *source1;
  u32 Bmpwidth;
  u8 Scoll_time;
} subt_draw_osd_pic_t;

typedef struct
{
  u32 x;
  u32 y;
  u32 width;
  u32 height;
  u32 color;
} subt_draw_osd_rect_t;

/*!
  command id list
  */
typedef enum
{
  /*!
    Stop play
    */
  SUBT_PIC_CMD_START = ASYNC_CMD,
  
  SUBT_PIC_CMD_STOP,
  
  SUBT_PIC_CMD_CLOSE,
  
  SUBT_PIC_CMD_START_SYNC = SYNC_CMD,
  
  SUBT_PIC_CMD_STOP_SYNC,
  
  SUBT_PIC_CMD_CLOSE_SYNC,
 
}subt_pic_cmd_t;


typedef enum
{
  /*!
    the first event of pb module,
    if create new event, you must insert it between BEGIN and END
    */
  SUBT_PIC_EVT_BEGIN = ((APP_RESERVED2<< 16) | ASYNC_EVT),
 
  /*!
    ack the sync cmd PB_CMD_SET_MUTE
    */
  SUBT_PIC_EVT_START = ((APP_RESERVED2 << 16) | SYNC_EVT),
  SUBT_PIC_EVT_STOP,  
  SUBT_PIC_EVT_END
} subt_pic_evt_t;

app_t *construct_ap_tr_subt(subt_policy_t *p_policy);


