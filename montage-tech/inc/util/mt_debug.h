/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MT_DEBUG_H__
#define __MT_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
   debug module name
  */
typedef enum
{
  /*!
     debug all modules
    */
  MT_DBG_ALL = 0xFFFFFFFF,
  /*!
     IRDA
    */
  MT_DBG_IRDA = 1 << 0,
  /*!
     video decoder
    */
  MT_DBG_VDEC = 1 << 1,
  /*!
     nim
    */
  MT_DBG_NIM = 1 << 2,
  /*!
     video decoder firmware
    */
  MT_DBG_VDEC_FW = 1 << 3,
}mt_dbg_module_t;

/*!
   debug tag name
  */
typedef enum
{
  /*!
     the tag to indicate enter the module 
    */
  MT_DBG_TAG_ENTER = 0,
  /*!
     the tag to indicate exit the module 
    */
  MT_DBG_TAG_EXIT,
  /*!
     video decoder fw init start
  */
  MT_DBG_TAG_VDEC_FW_INIT_START,
  /*!
     video decoder fw init end
  */
  MT_DBG_TAG_VDEC_FW_INIT_END,
  /*!
     video decoder fw find SPS head
  */
  MT_DBG_TAG_VDEC_FW_FIND_SPS,
  /*!
     video decoder fw get stable frame
  */
  MT_DBG_TAG_VDEC_FW_STABLE,
  /*!
     video decoder fw open screen
  */
  MT_DBG_TAG_VDEC_FW_OPEN_SCREEN,
  /*!
     nim channel lock start
  */
  MT_DBG_TAG_NIM_LOCK_SATRT,
  /*!
     nim channel lock stop
  */
  MT_DBG_TAG_NIM_LOCK_STOP,  
}mt_dbg_tag_t;

/*!
   debug level
  */
typedef enum
{
  /*!
     whether debug info is visible depends on system level's definition
    */
  MT_DBG_SYS_DEF = 0,
  /*!
     debug info is always visible
    */
  MT_DBG_ALWAYS_ON = 1,
}mt_dbg_level_t;

/*!
   debug level
  */
typedef enum
{
  /*!
     execute with block mode
    */
  MT_DBG_FLAG_BLOCK = 0,
  /*!
     execute with asynchronous mode
    */
  MT_DBG_FLAG_ASYNC = 1,
  /*!
     usb remove
    */
  MT_DBG_FLAG_USB_REMOVE,
}mt_dbg_flag_t;

/*!
   start file debug flag
  */
typedef enum
{
  /*!
     start with file overwrite mode
    */
  MT_DBG_FLAG_START_OVERWR = 0,
  /*!
     start with file seekend mode
    */
  MT_DBG_FLAG_START_SEEKEND = 1,
}mt_dbg_file_start_flag_t;

/*!
   debug time stamp log node
  */
typedef struct
{
  /*!
     module
    */
  mt_dbg_module_t module;
  /*!
     tag
    */
  u32 tag;  
  /*!
     s
    */
  u32 s;
  /*!
     ms
    */
  u32 ms;
  /*!
     us
    */
  u32 us;
}mt_dbg_time_stamp_t;


/********** TIME STAMP ****************/

/*!
   create a time stamp to debug

   \param[in] modules: the modules you want to record the time stamp, see mt_dbg_module_t.
   \param[in] log_max_cnt: the max log count .
   \param[out] p_id: the id you created.
  */
RET_CODE mt_dbg_time_stamp_create(u32 modules, u32 log_max_cnt, u32 *p_id);

/*!
   destory the time stamp

   \param[in] id: the id you want to destory
  */
RET_CODE mt_dbg_time_stamp_destory(u32 id);

/*!
   start the time stamp, note that to start recording time stamp, you need to call this api

   \param[in] id: the id you want to enable
  */
RET_CODE mt_dbg_time_stamp_start(u32 id);

/*!
   stop the time stamp, note that to stop recording time stamp, you need to call this api

   \param[in] id: the id you want to disable
   \param[out] p_log_cnt: the log count recorded
   \param[out] p_log: pointer to the log list recorded
  */
RET_CODE mt_dbg_time_stamp_stop(u32 id, u32 *p_log_cnt, 
  mt_dbg_time_stamp_t **p_log);

/*!
   time stamp on, used by module who wants to record time stamp

   \param[in] module: the module who wants to stamp on this time.
   \param[in] tag: the tag of this time stamp, see mt_dbg_tag_t, to indicate different time point
  */
RET_CODE mt_dbg_time_stamp_on(mt_dbg_module_t module, u32 tag);


/************* FILE PRINT ******************/

/*!
   check if debug file printf enable
  */
BOOL mt_dbg_file_printf_ena_chk();

/*!
   enable the debug function for log printing into a file

   \param[in] buf_size: the size of the buffer used to as the buffer for file writing.
   \param[in] sys_idle_task_prio: the priority of the idle task in system as a background processing
                                                for the log file writing.
   \param[in] slice_ms: the background task slice in ms.                                                
  */
RET_CODE mt_dbg_file_printf_enable(u32 buf_size, u32 sys_idle_task_prio, u32 slice_ms);

/*!
   disable the debug function for log printing into a file
  */
RET_CODE mt_dbg_file_printf_disable();

/*!
   start log printing into a file, will open a new file.
  */
RET_CODE mt_dbg_file_printf_start(char *p_file_name, u32 flag);

/*!
   force print to serial
  */
RET_CODE mt_dbg_serial_force_print(int print);


/*!
   stop log printing into a file, will close the file

   \param[in] flags: the flag used to indicate stop with which mode.
  */
RET_CODE mt_dbg_file_printf_stop(mt_dbg_flag_t flags);

/*!
   mt_dbg_printf 

   \param[in] level: the debug level, see mt_dbg_level_t.
   \param[in] p_fmt: the printf format.
  */
int mt_dbg_printf(unsigned int level, const char *p_fmt, ...);

/*!
   file printf
   
   \param[in] p_fmt: the printf format.
   \param[in] p_args: the args.
  */
int mt_dbg_printk(const char *p_fmt, char *p_args);

/*!
   enable the debug function for log printing into flash

   \param[in] buf_size: the size of the buffer used to as the buffer for flash writing(strongly recommended that the buf_size is multiple of CHARSTO_SECTOR_SIZE).
   \param[in] flash_addr: the start address of the flash to write.                                                
  */
RET_CODE mt_dbg_crashinfo_flash_write_enable(u32 flash_addr,u32 buf_size);

/*!
   disable the debug function for log printing into flash
  */
RET_CODE mt_dbg_crashinfo_flash_write_disable();
/*!
   flash write record node
  */
typedef struct
{
/*!
     enable the function to write crash info to flash
    */
    u32 enable;
/*!
     flash addr to write
    */
    u32 flash_addr;
/*!
     the addr of buf to write
    */
    char * crashinfo_log_buf;
/*!
     the size of buf
    */
    u32 buf_size;
/*!
     the offset of already used.
    */
    u32 log_cur_offset;
} mt_dbg_crashinfo_flash_write_t;

/*!
   get the flash record node for log printing into flash

   \param[out] mt_dbg_crashinfo_flash_write_t : the flash record info node to get.                                             
  */
mt_dbg_crashinfo_flash_write_t * mt_dbg_crashinfo_flash_write_get_info();

#ifdef __cplusplus
}
#endif

#endif //__MT_DEBUG_H__
