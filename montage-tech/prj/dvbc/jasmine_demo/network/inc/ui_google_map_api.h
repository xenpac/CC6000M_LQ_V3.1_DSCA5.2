/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_GOOGLE_MAP_API_H__
#define __UI_GOOGLE_MAP_API_H__


/*!
  ui_google_map_init
  */
RET_CODE ui_google_map_init(void);

/*!
  ui_google_map_release
  */
RET_CODE ui_google_map_release(void);

/*!
  ui_google_map_start
  */
RET_CODE ui_google_map_start(mul_google_map_param_t *p_map_param);

/*!
  ui_google_map_stop
  */
RET_CODE ui_google_map_stop(void);

/*!
  ui_google_map_move
  */
RET_CODE ui_google_map_move(map_move_style_t move_style);

/*!
  ui_google_map_zoom
  */
RET_CODE ui_google_map_zoom(map_zoom_style_t zoom_style);


#endif
