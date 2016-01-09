/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __WAIT_CTRL_H__
#define __WAIT_CTRL_H__
#ifdef __cplusplus
extern "C" {
#endif

/*!
  orb img type.
  */
typedef enum
{
  /*!
    img by id.
    */
  WAIT_IMG_ID,
  /*!
    img by ext.
    */
  WAIT_IMG_EXT,
}wait_img_t;

/*!
   Register circular progress bar control class to system.

   \param[in] max_cnt : max progress bar control number
   \return : SUCCESS or ERR_FAILURE
  */
RET_CODE wait_register_class(u16 max_cnt);

/*!
  set wait img.
  */
void wait_set_img(control_t *p_ctrl, wait_img_t img_type, u32 img_data);

/*!
   cicular progress proc.

   \param[in] p_ctrl : cicular progress bar control
   \param[in] msg : msg
   \param[in] para1 : parameter
   \param[in] para2 : parameter
   \return : SUCCESS or ERR_NOFEATURE.
  */
RET_CODE wait_class_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#ifdef __cplusplus
}
#endif

#endif


