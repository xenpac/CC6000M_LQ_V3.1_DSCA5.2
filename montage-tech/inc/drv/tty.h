/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TTY_H__
#define __TTY_H__


/*!
 note
 */
RET_CODE tty_read(drv_dev_t *p_dev, u8 index, u8 *buf, s32 *ret,s32 cn);
/*!
 note
 */
RET_CODE tty_write(drv_dev_t *p_dev, u8 index,u8 *buf, s32 count);
/*!
 note
 */
RET_CODE tty_ioctl(drv_dev_t *p_dev, u32 cmd, u32 arg);

#endif  // end of BLOCK_H
