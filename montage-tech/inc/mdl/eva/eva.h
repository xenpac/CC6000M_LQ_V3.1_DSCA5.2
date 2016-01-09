/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __EVA_H_
#define __EVA_H_

/*!
  eva print out level
  */
typedef enum 
{
  /*!
    undefine the level, default to ERROR level
    */
  E_PRINT_UNDEF,
  /*!
    eva print out all debug info and stream info,
    this option will give crazily print!!!
    */
  E_PRINT_STREAM_INFO,
  /*!
    eva print out all debug info
    */
  E_PRINT_ALL,
  /*!
    eva print out from major level
    */
  E_PRINT_MAJOR,
  /*!
    eva print out from error level
    */
  E_PRINT_ERROR,
  /*!
    eva has not any print out
    */
  E_PRINT_NONE,
}eva_print_level;


/*!
  create eva system parameter define
  */
typedef struct tag_eva_init_para
{
  /*!
    print level. 
    */
  eva_print_level debug_level;
  /*!
    eva system task priority
    */
  u32 eva_sys_task_prio_start;
  /*!
    eva system task priority
    */
  u32 eva_sys_task_prio_end;
}eva_init_para_t;


/*!
  init eva system

  \param[in] p_para /see eva_init_para_t.

  \return return SUCCESS
  */
RET_CODE eva_init(eva_init_para_t *p_para);


/*!
  send a system message. 
  */
RET_CODE eva_send_sys_msg(handle_t _this, interface_t *p_interface,
                          os_msg_t *p_msg);

/*!
  send a error message. 
  */
RET_CODE eva_send_error_msg(handle_t _this, os_msg_t *p_msg);


/*!
  get eva version
  
  \return return version
  */
char *eva_get_version(void);

#endif // End for __EVA_H_

