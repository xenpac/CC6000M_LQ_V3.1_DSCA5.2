/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_FLASH_PROTECT_IMP_H__
#define __LOG_FLASH_PROTECT_IMP_H__

/*!
  flash protect check point
  */
typedef enum
{
  DM_INIT_V2,
  DM_WRITE_NODE_TO_FLASH,
  DM_WRITE_NODE_V2,
  DM_FLUSH_CACHE_V2,
  DM_DIRECT_WRITE_V2,
  DM_DIRECT_ERASER_V2,
  DM_WRITE_DELETE_NODE,
  DM_DELETE_NODE_V2,
  DM_RESTORE_V2,
  DM_RESTORE_CACHE_V2,
  DM_WRITE_ONLY_V2,
  DM_DATA_ERASE_V2,
  DM_UNPROTECTED,
  DM_PROTECTED,
}flash_protect_checkpoint_t;

 /*!
   init
 */
handle_t log_flash_protect_init(void);

#endif // End for __LOG_FLASH_PROTECT_IMP_H__

