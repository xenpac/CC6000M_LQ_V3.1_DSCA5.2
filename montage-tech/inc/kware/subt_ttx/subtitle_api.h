/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUBTITLE_VSB_H__
#define __SUBTITLE_VSB_H__

/*!
  SUBT event define
  */
typedef enum
{
  /*!
    SUBT event define
    */
  SUBT_EVT_BEGIN = (0xbc << 16),
  /*!
    SUBT STOP COMMAND is ready
    */
  SUBT_EVT_STOPPED,
  /*!
    SUBT event end
    */
  SUBT_EVT_END
}subt_evt_t;

/*!
  callback function definition
  */
typedef void (*subt_callback)(void *para);

/*!
  call back function and para
  */
typedef struct
{
    /*!
      callback function
      */
    subt_callback callback;
    
    /*!
      callback para
      */
    u32 para;
}subt_callback_para;


/*!
   initialize subtitle module

   \param[in] prio the task's priority
   \param[in] stack_size the size of the stack

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_init_vsb(u32 prio, u32 stack_size, disp_layer_id_t layer, void *cb);

/*!
   Set the PID and page ID that want to display

   \param[in] pid PID of subtilte PES
   \param[in] composition_page Page id of composition page
   \param[in] ancillary_page Page id of ancillary page

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_page_vsb(u16 pid, u16 composition_page, u16 ancillary_page);

/*!
   Start subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_start_vsb(void);

/*!
   Stop subtilte

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_stop_vsb();

/*!
   Set subtitle decoder pause ON/OFF

   \param[in] is_pause TRUE for pause on, and FALSE for pause off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_pause_vsb(BOOL is_pause);

/*!
   Set subtitle display ON/OFF

   \param[in] is_pause TRUE for display on, and FALSE for display off.

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_display_vsb(BOOL is_display);

/*!
   Set subtitle dec position.

   \param[in] set dec buffer in decoder or out decoder

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_region_pos(subt_region_pos_t pos);

/*!
   Set video standard.

   \param[in] std standar of video

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_video_std_vsb(video_std_t std);

/*!
   Set subtitle sys .

   \param[in] sys 

   \return Return 0 for success and others for failure.
  */
RET_CODE subt_set_sys_cfg_vsb(subt_sys_cfg_t cfg);

/*!
   Set ts slot in.

   \param[ts_in] 0 for magic and warriors , 2 for sonata

  */
void subt_set_ts_in(u8 ts_in);
#endif

