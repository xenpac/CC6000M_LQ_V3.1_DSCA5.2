/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SBAR_CTRL_H__
#define __SBAR_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_progressbar.h

   This file defined some interfaces of scrollbar control for application layer
   modules.
  */

/*!
   Scrollbar direction mask, 0 is vertical; 1 is horizontal.
  */
#define SBAR_DIRECTION_MASK    0x00000001
/*!
   Scrollbar autosize mode mask, 0 is disable; 1 is enable.
  */
#define SBAR_AUTOSIZE_MASK     0x00000002   // 0 is disable ; 1 is enable

/*!
   Register scroll bar control class to system.

   \param[in] max_cnt : max scroll bar control number
   \return : SUCESS or ERR_FAILURE.
  */
RET_CODE sbar_register_class(u16 max_cnt);

/*!
   Set middle move block r-style.
   \param[in] p_ctrl : scroll bar control
   \param[in] n_mid_rstyle : normal move block rstyle
   \param[in] h_mid_rstyle : highlight move block rstyle
   \param[in] g_mid_rstyle : gray move block rstyle
   \return : NULL
  */
void sbar_set_mid_rstyle(control_t *p_ctrl,
                        u32 n_mid_rstyle,
                        u32 h_mid_rstyle,
                        u32 g_mid_rstyle);

/*!
  sbar get mid rstyle.
  */
void sbar_get_mid_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle);


/*!
   Set total count and page count of scroll bar.

   \param[in] p_ctrl : scroll bar control
   \param[in] page : page count
   \param[in] total : total count
   \return : NULL
  */
void sbar_set_count(control_t *p_ctrl, u16 page, u16 total);

/*!
   Set current position of scroll bar.

   \param[in] p_ctrl : scroll bar control
   \param[in] cur_pos : new pos
   \return : NULL
  */
void sbar_set_current_pos(control_t *p_ctrl, u16 cur_pos);

/*!
   Set auto size.

   \param[in] p_ctrl : scroll bar control
   \param[in] is_autosize : autosize or not.
   \return : NULL
  */
void sbar_set_autosize_mode(control_t *p_ctrl, BOOL is_autosize);

/*!
  sbar is autosize mode.
  */
BOOL sbar_is_autosize(control_t *p_ctrl);

/*!
   Set direction

   \param[in] p_ctrl : scroll bar control
   \param[in] dir : horizontal or vertical
   \return : NULL
  */
void sbar_set_direction(control_t *p_ctrl, BOOL is_hori);

/*!
  sbar is horizontal.
  */
BOOL sbar_is_hori(control_t *p_ctrl);

/*!
   Get current position of scroll bar

   \param[in] p_ctrl : scroll bar control
   \return : current pos
  */
u16 sbar_get_current_pos(control_t *p_ctrl);

/*!
   Increase scroll bar

   \param[in] p_ctrl : scroll bar control
   \return : TRUE/FALSE
  */
BOOL sbar_increase(control_t *p_ctrl);

/*!
   Decrease scroll bar

   \param[in] p_ctrl : scroll bar control
   \return : TRUE/FALSE
  */
BOOL sbar_decrease(control_t *p_ctrl);

/*!
   Control proc.

   \param[in] p_ctrl : scroll bar control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE
  */
RET_CODE sbar_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif
#endif
