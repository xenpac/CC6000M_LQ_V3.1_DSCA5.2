/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DRV_SVC_H__
#define __DRV_SVC_H__


/*!
  The service thread nod callback function
  */
typedef RET_CODE (*DRVSVC_NOD_FUNC)(void *p_param, u32 event_status);

/*!
  The event flag mask for service thread scheduler waiting
  */
#define DRVSVC_EVENTM_SCHD  0x0FFFFFFF
/*!
  The event flag mask for user operation
  */
#define DRVSVC_EVENTM_USER  0x00FFFFFF

/*!
  The event flag for service nod operation protect
  */
#define DRVSVC_EVENT_PROT 0x20000000
/*!
  The event flag for sync with service thread scheduler
  */
#define DRVSVC_EVENT_SYNC 0x10000000

/*!
  The event flag for destroy service thread scheduler
  */
#define DRVSVC_EVENT_DSTR 0x04000000
/*!
  The event flag for inserted a new nod
  */
#define DRVSVC_EVENT_INS  0x02000000
/*!
  The event flag for removeed a old nod
  */
#define DRVSVC_EVENT_RMV  0x01000000

/*!
  This structure defined the nod in a service thread, which descript one process function callback. (Total size 24Bytes)
  */
typedef struct drvsvc_nod {
/*!
  Service nod function handler
  */
  DRVSVC_NOD_FUNC func;
/*!
  Service nod function param
  */
  void *p_param;
/*!
  Service nod wakeup cycle
  */
  u32 cycle;
/*!
  Service nod cared event mask
  */
  u32 emask : 24;
/*!
  Service nod running priority
  */
  u32 pri : 8;
/*!
  The timer link next nod
  */
  struct drvsvc_nod *p_next;  
/*!
  The timer link time tick for cycle style service nod
  */
  u32 time;
}drvsvc_nod_t;

/*!
  This structure defined a service thread handle. (Total size 20Bytes), We use this structure identify a service thread.
  */
typedef struct drvsvc_handle {
/*!
  The service thread wait event
  */
  mtos_evt_t event;
/*!
  The pointer to the service nod array
  */
  drvsvc_nod_t *p_nods;
/*!
  The pointer to the timer link list
  */
  drvsvc_nod_t *p_timers;
/*!
  The max nod number
  */
  u8 max_nod;
/*!
  Current nod number 
  */
  u8 cur_nod;
/*!
  The service thread priority
  */
  u8 prio;
/*!
  The flag to sync some operation
  */
  u8 flag_sync;
}drvsvc_handle_t;

#ifdef __cplusplus
extern "C"
{
#endif

/*!
  Create a driver service thread handle based on specified thread priority and stack_size.

  \param[in] prio The service thread priority
  \param[in] stack The service thread stack buffer pointer
  \param[in] stack_size The service thread stack size
  \param[in] max_nod The service thread max nod number

  \return NULL: Failure, Others: The service thread handle  
*/
extern drvsvc_handle_t *drvsvc_create(u8 prio, u32 *p_stack, u32 stack_size, u8 max_nod);

/*!
  Destroy the specific driver service thread

  \param[in] p_handle The service thread handle.

  \return None
*/
extern void drvsvc_destroy(drvsvc_handle_t *p_handle);

/*!
  Insert a nod into specified service thread. We support three type of service nod:
  1, Event driving service nod:
    Callback only when specified event flag set; The param should be:
      "emask" set as the event flag;
      "cycle" set as MTOS_WAIT_FOREVER.
  2, Cycle running service nod:
    Callback only when specified cycle time reach; The param should be:
      "emask" set as 0;
      "cycle" set as the cycle time value.
  3, Event driving timeout service nod:
    Callback when specified event flag set or timeout; The param should be:
      "emask" set as the event flag;
      "cycle" set as the timeout value.  
  Please note:
    System defined event include:
      DRVSVC_EVENT_DSTR: Service thread destroy event;
      DRVSVC_EVENT_INS:  Service nod insert event;
      DRVSVC_EVENT_RMV:  Service nod remove event.
    Following paramater combination is forbiden:
      1, "emask" set as 0; "cycle" set as MTOS_WAIT_FOREVER or 0.
      
  \param[in] p_handle The service thread handle.
  \param[in] func The service nod callback function.
  \param[in] param The service nod callback function parameter.
  \param[in] emask The service nod wait event mask. Total 32 bits and only low 24 bit for user, set 1 if care.
  \param[in] cycle The service nod cycle time value in OS ticks

  \return SUCCESS if success, Other if failed.
  */
extern RET_CODE drvsvc_nod_insert(drvsvc_handle_t *p_handle, DRVSVC_NOD_FUNC func,
  void *p_param, u32 emask, u32 cycle);

/*!
  Remove a nod from a specified service thread.

  \param[in] p_handle The service thread handle.
  \param[in] func The service nod callback function.
  \param[in] emask The service event want to be removed. 
                             If equal to 0, the all events on this nod will be removed at all.

  \return SUCCESS if success, Other if failed.
  */
extern RET_CODE drvsvc_nod_remove(drvsvc_handle_t *p_handle, DRVSVC_NOD_FUNC func, u32 emask);

/*!
  Set the service thread event.
  This operation will wakeup this service thread and go through all matched service nod in this service thread.

  \param[in] p_handle The service thread handle.
  \param[in] event The event bit(s) to be set.

  \return None
  */
extern void drvsvc_event_set(drvsvc_handle_t *p_handle, u32 event);

/*!
  Clear the service thread event.
  This operation will make the matched event service nods pending.

  \param[in] p_handle The service thread handle.
  \param[in] event The event bit(s) to be cleared.

  \return None
  */
extern void drvsvc_event_clear(drvsvc_handle_t *p_handle, u32 event);

#ifdef __cplusplus
}
#endif

#endif

