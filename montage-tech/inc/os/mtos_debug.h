/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __MTOS_DEBUG_H__
#define  __MTOS_DEBUG_H__
/*!
  TCB useful information
  */
typedef struct
{
  /*!
    the Count of switch time for current task in recent 1 second
    */
  u16 _TaskSwtichCnt;
  /*!
    the average CPU usage for current task in recent 1 second
    */
  u8 _TaskCPUUsage;
  /*!
    Task status
    */
  u8 _OSTCBStat;
  /*!
    Task priority (0 == highest, 63 == lowest)
    */
  u8 _OSTCBPrio;
  /*!
    Ascii description of task status
    */
  u8 _StatDescription[6];
  /*!
    Ascii description of task name
    */
  u8 _PrioDescription[20];
}tcb_info_t;

/*!
  Global os information
  */
typedef struct
{
  /*!
    size of structure  (debug_os_info_t)
    */
  u16 StructSize;
  /*!
    number of total task
    */
  u8 TaskNum;
  /*!
    Number of tasks created
    */
  u8 _OSTaskCtr;
  /*!
    Interrupt nesting level
    */
  u8 _OSIntNesting;
  /*!
    Int indicater
    */
  u8 _OSIntExitY;
  /*!
    Multitasking lock nesting level
    */
  u8 _OSLockNesting;
  /*!
    Priority of current task
    */
  u8 _OSPrioCur;
  /*!
    Priority of highest priority task
    */
  u8 _OSPrioHighRdy;
  /*!
    Flag indicating that kernel is running
    */
  u8 _OSRunning;
  /*!
    Current CPU usage, 0-100 percent
    */
  u8 _OSCPUUsage;
  /*!
    Task TCB info
    */
  tcb_info_t _OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];
}debug_os_info_t;

/*!
  os debug info for message queue, only available in debug mode
  */
typedef struct
{
  /*!
    time tick from power on, unit: ms
    */
  u32 tick;
  /*!
    current message structure, os_msg_t, here use u8 to avoid compile issue
    */
  u8 cur_msg[12];
  /*!
    this msg is to send or to receive, send: 1, receive: 0
    */
  u8 send_or_rcv;
  /*!
    message queue id
    */
  u8 msgq_id;
  /*!
    current task priority, idetification for task
    */
  u8 cur_task_prio;
  /*!
    message count remaining in current message queue
    */
  u8 m_qleft;  
  /*!
    index of current inqueue message
    */
  u16 m_qin;
  /*!
    index of current outqueue message
    */
  u16 m_qout;
  /*!
    index of buffer used by inqueue message
    */
  u16 b_in;
  /*!
    index of buffer used by outqueue message
    */
  u16 b_out;
}os_debug_msg_t;

/*!
  task info
  */
typedef struct debug_task_info
{
  /*!
    task switch count in 1 second
    */
  u16 task_switch_cnt;
  /*!
    task cpu usage
    */
  u8 task_cpu_usage;
  /*!
    task name
    */
  u8 task_name[6];
}debug_task_info_t;

/*!
  task stack info
  */
typedef struct
{
  /*!
    stack start pointer
    */
  u32 stack_ptr;
  /*!
    stack size
    */
  u32 stack_size;
  /*!
    stack max used size
    */
  u16 stack_used;
  /*!
    stack min free size
    */
  u16 stack_free;
}debug_task_stack_t;

/*!
  task stack info
  */
typedef struct
{
  /*!
    task stack info
    */
  debug_task_stack_t stack[OS_MAX_TASKS + OS_N_SYS_TASKS];
}debug_task_stack_info_t;


/*!
  debug memory information, alloced by upper user
  */
typedef struct
{
  /*!
    start address
    */
  u8 *start_addr;
  /*!
    memory size for common printk 
    */
  u32 common_print_size;
  /*!
    memory size for message queue print
    */
  u32 msgq_print_size;
  /*!
    memory size os global info
    */
  u32 os_info_size;
  /*!
    memory size for stack info
    */
  u32 stack_info_size;
  /*!
    memory size for memory mapping info
    */
  u32 mem_map_size;
}debug_mem_t;


/*!
  debug data infor, size and start address
  */
typedef struct
{
  /*!
    start address
    */
  u32 ddi_addr;
  /*!
    size
    */
  u32 ddi_len;
}debug_data_info_t;

/*!
  init the debug memory, allocted by Upper layer
  
  \param[in] p_mem_info the pointer to memory info
  */
void os_debug_init(debug_mem_t *p_mem_info);


/*!
  put stack info
  
  \param[in] prio the task priority
  \param[in] stack_ptr the stack start address
  \param[in] stack_size the stack size

  \return 0 if success, else if fail
  */
int debug_put_stack_info(u32 prio, u32 stack_ptr, u32 stack_size);

/*!
  get stack info, parse folling the struct debug_task_stack_info_t
  
  \param[out] p_info return the address and size,

  \return 0 if success, else if fail
  */
int debug_get_stack_info(debug_data_info_t *p_info);




/*!
  get memory map info, not available now
  
  \param[out] p_info return the address and size,

  \return 0 if success, else if fail
  */
int debug_get_mem_info(debug_data_info_t *p_info);




/*!
  put stack info
  
  \param[in] p_buff the ascii string to print
  \param[in] stack_size the string size

  \return 0 if success, else if fail
  */
int debug_put_common_print(u8 *p_buff, u32 len);
/*!
  get common print info, "``" is the end indicator
  
  \param[out] p_info return the address and size,

  \return 0 if success, else if fail
  */
int debug_get_common_print(debug_data_info_t *p_info);




/*!
  put message queue print info
  
  \param[in] msg_id the message queue index
  \param[in] p_msg the message to send or to receive, in format os_msg_t
  \param[in] send send or receive message, send: 'S', receive:'R'
  \param[in] msg_left the messages remain in current queue
  \param[in] msg_qin the input index of message queue
  \param[in] msg_qout the output index of message queue
  \param[in] buf_in the input buffer index, for extention message only
  \param[in] buf_out the output buffer index, for extention message only

  \return 0 if success, else if fail
  */
int debug_print_msgq_info(u8 msg_id, void *p_msg, u8 send, u8 msg_left, 
                          u8 msg_qin, u8 msg_qout, u8 buf_in, u8 buf_out);
/*!
  get message queue info, parse folling the struct os_debug_msg_t
  
  \param[out] p_info return the address and size,

  \return 0 if success, else if fail
  */
int debug_get_msgq_print(debug_data_info_t *p_info);




/*!
  put task name to os information
  
  \param[in] prio the priority of the task
  \param[in] p_task_name the name string

  \return 0 if success, else if fail
  */
int debug_put_task_name(u32 prio, u8 *p_task_name);

/*!
  put task information to os information
  
  \param[in] prio the priority of the task
  \param[in] cpu_usage the task cpu usage in the past 1 sencond
  \param[in] switch_cnt the os switch cnt of current task in the past 1 second

  \return 0 if success, else if fail
  */
int debug_put_task_info(u32 prio, u32 cpu_usage, u32 switch_cnt);

/*!
  get os relative info, parse folling the struct debug_os_info_t
  
  \param[out] p_info return the address and size,

  \return 0 if success, else if fail
  */
int debug_get_os_info(debug_data_info_t *p_info);




/*!
  dead loop when entering exception
  */
void os_dead_loop(void);

#endif //__MTOS_DEBUG_H__