/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __LOG_MGR_H__
#define __LOG_MGR_H__

/*!
  macro definition for flash protected debug
  */
#define FLASH_PROTECTED_DEBUG

 /*!
   log module id
 */
typedef enum
{
 /*!
   flash log
 */
  LOG_FLASH = 0,
 /*!
   ap scan log
 */
  LOG_AP_SCAN,
   /*!
   ap scan log
 */
  LOG_AP_CHANGE_CHANNEL,
   /*!
   ap scan log
 */
  LOG_AP_EPG,
 /*!
   nim ctrl log
 */
  LOG_NIM_CTRL,
  /*!
   render filter log
   */
   LOG_PIC_RENDER_FILTER,
 /*!
   data manager log
 */
  LOG_DATA_MANAGER,
 /*!
   subtitle_filter log
 */
  LOG_SUBTITLE_FILTER,  
  /*!
   dmx filter for record pin
 */
  LOG_DMX_FILTER_REC_PIN,
  /*!
   flash protect log
   */
   LOG_FLASH_PROTECT,
  /*!
   ap tkgs
   */
   LOG_AP_TKGS,
 /*!
   all module log
 */
  LOG_MODULE_ALL
} log_module_id_t;


/*
typedef enum
{
 LOG_DEBUG= 0x1,
 LOG_PERF = 0x1 << 1,
 LOG_LEVEL_ALL = LOG_DEBUG | LOG_PERF,
} log_level_id_t;
*/

 /*!
   log mananger
 */
typedef struct
{ 
  /*!
  log 
 */
  void (*do_log)(handle_t handle, log_module_id_t mid, 
                const char *p_file_name, u32 line, u32 event, u32 p1, u32 p2);
 /*!
 send one key
 */
 void (*do_send_key)(handle_t handle, log_module_id_t mid);
  /*!
  attach 
 */
  void (*attach)(handle_t handle, log_module_id_t mid, log_t *p_logger);
 /*!
   data 
 */  
  void *p_data;
} log_mgr_t;

/*!
log interface
*/
typedef struct
{
/*!
log interface for perf test
*/
  log_t *p_logger[LOG_MODULE_ALL];
} logger_mgr_t;

 /*!
   init log mananger
 */
log_mgr_t * log_mgr_init(void);
 
 /*!
   init log  ap scan
 */


//void attach(handle_t handle, log_module_id_t mid, log_t *p_logger);



#endif // End for __LOG_MGR_H__
