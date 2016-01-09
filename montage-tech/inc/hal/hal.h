/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __HAL_H__
#define __HAL_H__

/*!
  hal config parameters
  */
typedef struct
{
  /*!
      board id
    */
  u8 board_id;
  /*!
      reserved1
    */  
  u8 reserved1;
  /*!
      reserved2
    */  
  u16 reserved2;
}hal_config_t;

/*!
  hal_magic_attach
   */
char hal_magic_attach(void);
/*!
  hal_magic_attach
   */
void hal_libra_attach(void);
/*!
  hal_warriors_attach
   */
void hal_warriors_attach(hal_config_t *p_config);

/*!
  hal_orchid_attach
   */
char hal_orchid_attach(void);

/*!
  hal win32 attach
  */
void hal_win32_attach(hal_config_t *p_config);

#endif //__HAL_H__


