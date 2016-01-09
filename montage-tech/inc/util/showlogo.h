/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SHOWLOGO_H__
#define __SHOWLOGO_H__

/*!
  comment
  */
typedef enum 
{
  DRAW_ON_OSD = 0,
  DRAW_ON_STILL
}draw_on_layer_t;

/*!
   show logo function

   \param[in] disp_addr display memory address
   \param[in] p_input   jpeg picture content.
   \param[in] input_size jpeg picture size.
   \param[in] draw on layer.
  */
BOOL  show_logo(u32 disp_addr,u8 *p_input,u32 input_size, draw_on_layer_t type);

/*!
   show logo v2 function

   \param[in] disp_addr display memory address
   \param[in] p_input   jpeg picture content.
   \param[in] input_size jpeg picture size.
   \param[in] draw on layer.
   \param[in] width.
   \param[in] height.
  */
BOOL  show_logo_v2(u32 disp_addr,u8 *p_input,u32 input_size, draw_on_layer_t type, u32 w, u32 h);

/*!
   show usb upgrade string function

   \param[in] disp_addr display memory address
   \param[in] p_input   jpeg picture content.
   \param[in] location x.
   \param[in] location y.
  */
BOOL  show_pic(u32 disp_addr,u8 *p_input,u32 input_size, u32 x, u32 y);

#endif //__SHOWLOGO_H__
