/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_DUMP_H__
#define __GUI_DUMP_H__
/*!
  \file gui_dump.h

  This file defined data structure of script control.
  And defines interfaces for users to open a menu by script data.

  Development policy:
  */
/*!
  Offset type.
  */
typedef enum
{
  /*!
    Invalid offset
    */
  INVALID_OFFSET = 0,
  /*!
    Parent control offset
    */
  PARENT_OFFSET,
  /*!
    Child control offset
    */
  CHILD_OFFSET,
  /*!
    Next control offset
    */  
  NEXT_OFFSET,
  /*!
    Previous control offset
    */
  PREV_OFFSET,
  /*!
    Active child control offset
    */  
  ACTIVE_CHILD_OFFSET,
}offset_ctrl_type_t;

/*!
  For dump menu data.

  \param[in] p_ctrl : root container of a menu, for dump data.
  \return : NULL
  */
void spt_dump_menu_data(control_t *p_ctrl);

#endif

