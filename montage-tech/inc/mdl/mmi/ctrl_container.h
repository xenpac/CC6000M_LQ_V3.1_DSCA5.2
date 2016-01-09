/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CONT_CTRL_H__
#define __CONT_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_container.h

   This file defined some interfaces about container control for application
   layer modules.
  */

/*!
   Register container control class to system.

   \param[in] max_cnt : max container control number

   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE cont_register_class(u16 max_cnt);

/*!
   Move container focus

   \param[in] p_ctrl : container control
   \param[in] msg : focus move mode
   \return : NULL
  */
void cont_move_focus(control_t *p_ctrl, u16 msg);

/*!
   Get container clip region information.

   \param[in] p_ctrl : container control
   \return		 : clip region information
  */
crgn_info_t *cont_get_gcrgn_info(control_t *p_ctrl);

/*!
   Set container clip region information

   \param[in] p_ctrl : contrainer control
   \param[in] p_info : clip region information
   \return : NULL
  */
void cont_set_gcrgn_info(control_t *p_ctrl, crgn_info_t *p_info);

/*!
   Default message process function of container control.

   \param[in] p_ctrl : container control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE cont_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif
