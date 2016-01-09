/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CPBAR_CTRL_H__
#define __CPBAR_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif

/*!
   Register circular progress bar control class to system.

   \param[in] max_cnt : max progress bar control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE cpbar_register_class(u16 max_cnt);

/*!
   Set step.

   \param[in] p_ctrl : cicular progress bar control
   \param[in] step : steps
   \return : NULL
  */
void cpbar_set_step(control_t *p_ctrl, u16 step);

/*!
   Get step value.

   \param[in] p_ctrl : cicular progress bar control
   \return : current value
  */
u16 cpbar_get_step(control_t *p_ctrl);

/*!
   Set current value.

   \param[in] p_ctrl : cicular progress bar control
   \param[in] curn : current value
   
   \return : NULL
  */
void cpbar_set_curn(control_t *p_ctrl, u16 curn);

/*!
   Get current value.

   \param[in] p_ctrl : cicular progress bar control
   
   \return : current value
  */
u16 cpbar_get_curn(control_t *p_ctrl);

/*!
  circular progress bar set color.
  */
void cpbar_set_color(control_t *p_ctrl, u32 color);

/*!
  circular progress bar get color.
  */
u32 cpbar_get_color(control_t *p_ctrl);

/*!
   cicular progress proc.

   \param[in] p_ctrl : cicular progress bar control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE.
  */
RET_CODE cpbar_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif

