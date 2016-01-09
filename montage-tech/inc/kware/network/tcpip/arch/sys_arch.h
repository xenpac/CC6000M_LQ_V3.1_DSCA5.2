#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_mem.h"

#include "includes.h"

#define LWIP_STK_SIZE      (32 * 1024)

#define LWIP_TASK_MAX    5  //max number of lwip tasks
#define LWIP_START_PRIO  10   //first prio of lwip tasks

#define SYS_MBOX_NULL   (void *)0
#define SYS_SEM_NULL    0

#define MAX_QUEUES 20
#define MAX_MBOX_DEPTH 200
#define MBOX_USER_NAME "lwip"

/*!
  * None
  */
typedef struct {
		/*!
      * None
      */
    OS_EVENT*   pQ;
    /*!
      * None
      */
    void*       pvQEntries[MAX_MBOX_DEPTH];
} TQ_DESCR, *PQ_DESCR;

typedef os_sem_t sys_sem_t;
typedef PQ_DESCR sys_mbox_t;
typedef unsigned char sys_thread_t; /* based on our mtos api, it's no use*/
typedef u32 sys_prot_t;

/* Critical Region Protection */
/* These functions must be implemented in the sys_arch.c file.
   In some implementations they can provide a more light-weight protection
   mechanism than using semaphores. Otherwise semaphores can be used for
   implementation */
/** SYS_ARCH_DECL_PROTECT
 * declare a protection variable. This macro will default to defining a variable of
 * type sys_prot_t. If a particular port needs a different implementation, then
 * this macro may be defined in sys_arch.h.
 */
#define SYS_ARCH_DECL_PROTECT(lev) sys_prot_t cpu_sr;
/** SYS_ARCH_PROTECT
 * Perform a "fast" protect. This could be implemented by
 * disabling interrupts for an embedded system or by using a semaphore or
 * mutex. The implementation should allow calling SYS_ARCH_PROTECT when
 * already protected. The old protection level is returned in the variable
 * "lev". This macro will default to calling the sys_arch_protect() function
 * which should be implemented in sys_arch.c. If a particular port needs a
 * different implementation, then this macro may be defined in sys_arch.h
 */
 /* Simon feng: please be noted, SYS_ARCH_PROTECT and SYS_ARCH_UNPROTECT 
 * should be used only in one thread.
 */
#define SYS_ARCH_PROTECT(lev) { mtos_critical_enter(&cpu_sr);}
/** SYS_ARCH_UNPROTECT
 * Perform a "fast" set of the protection level to "lev". This could be
 * implemented by setting the interrupt level to "lev" within the MACRO or by
 * using a semaphore or mutex.  This macro will default to calling the
 * sys_arch_unprotect() function which should be implemented in
 * sys_arch.c. If a particular port needs a different implementation, then
 * this macro may be defined in sys_arch.h
 */
#define SYS_ARCH_UNPROTECT(lev) { mtos_critical_exit(cpu_sr); }

#endif
