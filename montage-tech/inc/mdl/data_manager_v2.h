/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_MANAGER_V2_H_
#define __DATA_MANAGER_V2_H_

/*!
  v2 int para
  */
typedef struct tag_dm_v2_init_para
{
  /*!
    use to check the param, the value must equal to sizeof(dm_v2_init_para_t)
    */
  u16 para_size;
  /*!
    max block number managed
    */
  u16 max_blk_num;
  /*!
    flash base address
    */
  u32 flash_base_addr;
  /*!
    create a task to monitor flash, auto cleanup block
    */
  BOOL open_monitor;
  /*!
    the task prioty
    */
  u32 task_prio;
  /*!
    use to check the param, the value must equal to sizeof(dm_v2_init_para_t)
    */
  u32 task_stack_size;
  /*!
    use mutex to protect critial block
    */
  BOOL use_mutex;
  /*!
    the mutex prioty
    */
  u32 mutex_prio;
  /*!
    align to 16 bytes
    */
  BOOL align_4bytes;
  /*!
    is test mode : if your project is test mode, please set TRUE.
    Because sometimes is hardware error, but we must assure system can work well in normal mode.
    */
  BOOL test_mode;
}dm_v2_init_para_t;


/*!
  Initialize data manager32 and init all the variables and attach process 
  functions 
  
  \param[in] p_para /see dm_v2_init_para_t
  \param[in] flash_base_addr 
  */
void dm_init_v2(dm_v2_init_para_t *p_para);

/*!
  Initialize data manager V2 extern function.
  */
void dm_init_v2_ex(dm_v2_init_para_t *p_para);

/*!
  destory data manager v2
  */
void dm_destory_v2(void);

#endif //__DATA_MANAGER_V2_H_ 
