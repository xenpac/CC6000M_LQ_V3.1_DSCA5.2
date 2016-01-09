/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DATA_MANAGER_V3_H_
#define __DATA_MANAGER_V3_H_

/*!
  v3 int para
  */
typedef struct tag_dm_v3_init_para
{
  /*!
    use to check the param, the value must equal to DM_VER_3
    */
  dm_version_t ver;
  /*!
    max block number managed
    */
  u16 max_blk_num;
  /*!
    create a task to monitor flash, auto cleanup block
    */
  BOOL open_monitor;
  /*!
    is test mode : if your project is test mode, please set TRUE.
    Because sometimes is hardware error, but we must assure system can work well in normal mode.
    */
  BOOL test_mode;
  /*!
    use mutex to protect critial block
    */
  BOOL use_mutex;
  /*!
    flash base address
    */
  u32 flash_base_addr;
  /*!
    the task prioty
    */
  u32 task_prio;
  /*!
    use to check the param, the value must equal to sizeof(dm_v2_init_para_t)
    */
  u32 task_stack_size;
  /*!
    the mutex prioty
    */
  u32 mutex_prio;
  /*!
    mem size for ghost in sdram
    */
  u32 piece_size;
  /*!
    piece addr in sdram
    */
  u32 ghost_addr;
  /*!
    using node max len
    */
  u32 using_max_node_len;
}dm_v3_init_para_t;

/*!
  Initialize data manager32 and init all the variables and attach process 
  functions 
  
  \param[in] p_para /see dm_v2_init_para_t
  \param[in] flash_base_addr 
  */
void dm_init_v3(dm_v3_init_para_t *p_para);

#endif //__DATA_MANAGER_V3_H_
