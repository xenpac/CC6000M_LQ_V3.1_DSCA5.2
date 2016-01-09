/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PBAR_CTRL_H__
#define __PBAR_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_progressbar.h

   This file defined some interfaces of progressbar control for application
   layer modules.
  */

/*!
   Progressbar direction mask, 0 is vertical, 1 is horizontal.
  */
#define PBAR_DIRECTION_MASK     0x00000001
/*!
   Progressbar workmode mask,
   0 : left/bottom is min value; 1 : right/top is min value.
  */
#define PBAR_WORKMODE_MASK      0x00000002
/*!
   Progressbar type mask, 0 : block bar; 1 : continuous bar;
  */
#define PBAR_CONTINUOUS_MASK    0x00000004

/*!
  pbar seg description.
  */
typedef struct
{
  /*!
    percent min.
    */
  u16 per_min;
  /*!
    percent max.
    */
  u16 per_max;
  /*!
    rstyle id.
    */
  u32 rstyle_id;
}pbar_seg_des_t;

/*!
  pbar seg info.
  */
typedef struct
{
  /*!
    seg count.
    */
  u16 seg_count;    
  /*!
    seg descriptions.
    */    
  pbar_seg_des_t *p_seg_des;
}pbar_seg_t;

/*!
   Register progress bar control class to system.

   \param[in] max_cnt : max progress bar control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE pbar_register_class(u16 max_cnt);

/*!
   Set rectangle style.

   \param[in] p_ctrl : progress bar control
   \param[in] min_rstyle : color style at min side
   \param[in] max_rstyle : color style at max side
   \param[in] cur_rstyle : color style of current point
   \return : NULL
  */
void pbar_set_rstyle(control_t *p_ctrl,
                     u32 min_rstyle,
                     u32 max_rstyle,
                     u32 cur_rstyle);

/*!
  pbar get rstyle.
  */
void pbar_get_rstyle(control_t *p_ctrl, u32 *p_min_rstyle, u32 *p_max_rstyle, u32 *p_curn_rstyle);


/*!
   Set count.

   \param[in] p_ctrl : progress bar control
   \param[in] min : min value
   \param[in] max : max value
   \param[in] step : steps
   \return : NULL
  */
void pbar_set_count(control_t *p_ctrl, u16 min, u16 max, u16 step);

/*!
   Set current value.

   \param[in] p_ctrl : progress bar control
   \param[in] curn : current value
   \return : NULL
  */
void pbar_set_current(control_t *p_ctrl, u16 curn);

/*!
   Set direction.

   \param[in] p_ctrl : progress bar control
   \param[in] dir : 0 is vertical ; 1 is horizontal
   \return : NULL
  */
void pbar_set_direction(control_t *p_ctrl, u8 dir);

/*!
  pbar is horizontal.
  */
BOOL pbar_is_hori(control_t *p_ctrl);

/*!
  pbar is continue mode.
  */
BOOL pbar_is_continue(control_t *p_ctrl);

/*!
  pbar get interval.
  */
u8 pbar_get_interval(control_t *p_ctrl);

/*!
   Set continuous mode.

   \param[in] p_ctrl : progress bar control
   \param[in] continuous : 0 is block bar ; 1 is continuous bar
   \param[in] interval : interval between blocks
   \return : NULL
  */
void pbar_set_workmode(control_t *p_ctrl, u8 continuous, u8 interval);

/*!
   Get current value.

   \param[in] p_ctrl : progress bar control
   \return : current value
  */
u16 pbar_get_current(control_t *p_ctrl);

/*!
   Progress bar increase

   \param[in] p_ctrl : progress bar control
   \return : TRUE/FALSE
  */
BOOL pbar_increase(control_t *p_ctrl);

/*!
   Progress bar decrease

   \param[in] p_ctrl : progress bar control
   \return	 : TRUE/FALSE
  */
BOOL pbar_decrease(control_t *p_ctrl);


/*!
   Control proc.

   \param[in] p_ctrl : progress bar control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE.
  */
RET_CODE pbar_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
  pbar set seg.
  */
void pbar_set_seg(control_t *p_ctrl, pbar_seg_t *p_seg);

#ifdef __cplusplus
}
#endif

#endif
