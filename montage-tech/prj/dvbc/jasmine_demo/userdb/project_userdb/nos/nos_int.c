/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include ".\inc\nos_ii.h"
#include "mtos_printk.h"
#include ".\inc\mips_cpu.h"
#include "list.h"
#include "lib_bitops.h"
#include "hal_irq.h"
#include "hal_base.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "../inc/mtos_os.h"

#define EXC_NONE                  0x000
#define EXC_SYSCALL               0x001
#define EXC_BREAK                 0x002
#define EXC_COP0                  0x004
#define EXC_OVERFLOW              0x008
#define EXC_ILLEGAL_INST          0x010
#define EXC_HW0                   0x020
#define EXC_HW1                   0x040
#define EXC_HW2                   0x080
#define EXC_HW3                   0x100
#define EXC_NMI                   0x200
#define EXC_MISALIGN              0x400
#define EXC_INVLID_ADDR           0x800


#define EXCEPTION_PRINT OS_PRINTF

/************************************************************************
 * Defination of IRQ Structure
 ************************************************************************/
extern u32 OSIntCtxSwFlag;

typedef struct
{
  list_head_t que;
  u32 func;
  u8 index; //index in handle array
}nos_irq_handle_t;

typedef struct
{
  nos_irq_handle_t handle[IRQ_SOURCE_MAX_NUM];
  u32 used[(IRQ_SOURCE_MAX_NUM + 31)>>5];
}nos_irq_handle_info_t;

typedef struct os_irq_info
{
  list_head_t que_head[IRQ_USER_INT_NUM]; // the header of handle list linked to every IRQ line
  nos_irq_handle_info_t handle_info; // INT Handle poll
  u32 intmask;
}os_irq_info_t;


static struct os_irq_info irq_info;
#define MAXINTERRUPTS 7
typedef void (*HW_IRQ_HANDLE)(u32);
HW_IRQ_HANDLE hw_irq_vector[MAXINTERRUPTS];
u32 hw_irq_mask[MAXINTERRUPTS];

#define PRINTT_CONTEXT(reg)   EXCEPTION_PRINT("\rR%d:[0x%x]",reg,((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_##reg)

void log_context(void)
{

  EXCEPTION_PRINT("\n");
  /*
  EXCEPTION_PRINT("\rat:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_at);
  EXCEPTION_PRINT("\rv0:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_2);
  EXCEPTION_PRINT("\rv1:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_3);
  EXCEPTION_PRINT("\ra0:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_4);
  EXCEPTION_PRINT("\ra1:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_5);
  EXCEPTION_PRINT("\ra2:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_6);
  EXCEPTION_PRINT("\ra3:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_7);
  EXCEPTION_PRINT("\rt0:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_8);
  EXCEPTION_PRINT("\rt1:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_9);
  EXCEPTION_PRINT("\rt2:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_10);
  EXCEPTION_PRINT("\n");
  EXCEPTION_PRINT("\rt3:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_11);
  EXCEPTION_PRINT("\rt4:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_12);
  EXCEPTION_PRINT("\rt5:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_13);
  EXCEPTION_PRINT("\rt6:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_14);
  EXCEPTION_PRINT("\rt7:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_15);
  EXCEPTION_PRINT("\rt8:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_24);
  EXCEPTION_PRINT("\rt9:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_25);
  EXCEPTION_PRINT("\rs0:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_16);
  EXCEPTION_PRINT("\rs1:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_17);
  EXCEPTION_PRINT("\rs2:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_18);
  EXCEPTION_PRINT("\rs3:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_19);
  EXCEPTION_PRINT("\n");
  EXCEPTION_PRINT("\rs4:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_20);
  EXCEPTION_PRINT("\rs5:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_21);
  EXCEPTION_PRINT("\rs6:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_22);
  EXCEPTION_PRINT("\rs7:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_23);
  EXCEPTION_PRINT("\rgp:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_28);
  EXCEPTION_PRINT("\rsp:[0x%x]  ",OSTCBCur->OSTCBStkPtr);
  EXCEPTION_PRINT("\rs8/fp:[0x%x] ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_30);
  EXCEPTION_PRINT("\rra:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_31);
  EXCEPTION_PRINT("\rhi:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_hi);
  EXCEPTION_PRINT("\rlo:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_lo);
  EXCEPTION_PRINT("\rexception_pc:[0x%x]  ",((OS_REGS *)(OSTCBCur->OSTCBStkPtr))->reg_pc);
  EXCEPTION_PRINT("\n");
*/
}

static void exception_handle(u32 cause)
{
#ifdef ASSERT_ON
    u32 exe_addr=0, exe_inst=0;
    
    __asm__ volatile ("mfc0 $14,%0 \n":"=r"(exe_addr));
    __asm__ volatile ("mfc0 $16,%0 \n":"=r"(exe_inst));
    //EXCEPTION_PRINT("\n\r EXC_BREAK, task[%d], break code addr[0x%x], inst[0x%x]\n", OSPrioCur,exe_addr,exe_inst);
    log_context();
    if(cause & EXC_MISALIGN)
    {
      EXCEPTION_PRINT("\n\r EXC_MISALIGN\n");
    }
    if(cause & EXC_INVLID_ADDR)
    {
      EXCEPTION_PRINT("\n\r EXC_INVLID_ADDR\n");
    }

    u32 volatile loop_count=0;
    while(loop_count>=0)
    {
      loop_count++;
    }
#endif    
}

void hal_irq_int_enable(u8 irq)
{
  u32 dtmp = 0;

  dtmp = hal_get_u32((u32 *)R_IRQ_MASK);
  dtmp &= (~(0x1 << irq));
  hal_put_u32((u32 *)R_IRQ_MASK, dtmp);
}

void hal_irq_int_disable(u8 irq)
{
  u32 dtmp = 0;

  dtmp = hal_get_u32((u32 *)R_IRQ_MASK);
  dtmp |= (0x1 << irq);
  hal_put_u32((u32 *)R_IRQ_MASK, dtmp);
}


void hal_hw_int_enable(u32 irq_num)
{
  u32 tmp;
  u32 mask;

  // only lowest four bits of mask register in cop0 are valid
  // and only hardware interrupt 0 ~3 are maskable,hard ware interrupt
  // nmi and software excption are unmaskable 
  mask = ~(1<<irq_num);
  //ASM_2NOP;
  //GET_MASK(tmp);
  //ASM_2NOP;
  tmp = tmp & mask;
  //ASM_2NOP;
  //SET_MASK(tmp);
  //ASM_2NOP;
}

void hal_hw_int_disable(u32 irq_num)
{
  u32 tmp;
  u32 mask;

  // only lowest four bits of mask register in cop0 are valid
  // and only hardware interrupt 0 ~3 are maskable,hard ware interrupt
  // nmi and software excption are unmaskable 
  mask = 1<<irq_num;
  //GET_MASK(tmp);
  tmp = tmp | mask;
  //SET_MASK(tmp);
}

/************************************************************************
* syscall ISR
* param:  void
* return: no
************************************************************************/
void IRQSyscallISR(u32 cause)     // system call , break and overflow exception
                                     // handler
{
  if (cause & EXC_SYSCALL)
  {
    EXCEPTION_PRINT("\n\r EXC_SYSCALL\n");
  }
  else if (cause & EXC_BREAK)
  {
    EXCEPTION_PRINT("\n\r EXC_BREAK\n");
  }
  else if (cause & EXC_COP0)
  {
    EXCEPTION_PRINT("\n\r EXC_COP0\n");
  }
  else if (cause & EXC_OVERFLOW)
  {
    EXCEPTION_PRINT("\n\r EXC_OVERFLOW\n");
  }
  else
  {

  }
  exception_handle(cause);
  return;
}


/************************************************************************
* Invalid inst ISR
* param:  void
* return: no
************************************************************************/
void IRQInvalidInstISR(u32 cause) // invalid inst exception handler
{
  if (cause & EXC_ILLEGAL_INST)
  {
    EXCEPTION_PRINT("\n\r Invalid instruct IRQ\n");
  }
  else
  {
    EXCEPTION_PRINT("\n\r error Invalid IRQ\n");
  }
  exception_handle(cause);
  return;
}

void nos_irq_process(u32 vec)
{
  list_head_t *temp = NULL;
  list_head_t *node = NULL;
  nos_irq_handle_t *handle_node = NULL;
  void (*handle)(void);

  list_for_each_safe(node, temp, &irq_info.que_head[vec])
  {
    handle_node = list_entry(node, nos_irq_handle_t, que);
    handle = (void (*)(void))handle_node->func;
    if (handle != NULL)
    {
      (*handle)();
    }    
  }
}

/************************************************************************
* fast ISR
* param:  void
* return: no
************************************************************************/
BOOL IRQHW3ISR(u32 cause)        // timetick interrupt handler
{
  u32 dtmp = 0;

  // Get Vector
  dtmp = hal_get_u32((u32 *)R_IRQ_VECTOR);
  
  // Call back to user
 nos_irq_process(dtmp);
  hal_put_u32((u32 *)R_IRQ_COW0, 0x04);
  return TRUE;
}


/************************************************************************
* normal ISR
* param:  void
* return: no
************************************************************************/
BOOL IRQHW2ISR(u32 cause)         // hardware interrupt2 interrupt handler
{
  u32 dtmp = 0;

  // Get Vector
  dtmp = hal_get_u32((u32 *)R_IRQ_VECTOR);
  
  // Call back to user
  //if(dtmp != 3)
  //  OS_PRINTF("INT:%d \n",dtmp);
 nos_irq_process(dtmp);
  hal_put_u32((u32 *)R_IRQ_COW0, 0x04);
  return TRUE;
}


/************************************************************************
* 1 pin ISR. useless
* param:  void
* return: no
************************************************************************/
void IRQHW1ISR(u32 cause)          // hardware interrupt1 interrupt handler
{
  //OS_PRINTF("\n\r error HW1 IRQ\n");
  exception_handle(cause);
}


/************************************************************************
* 0 pin ISR. useless
* param:  void
* return: no
************************************************************************/
void IRQHW0ISR(u32 cause)          // hardware interrupt0 interrupt handler
{
  //OS_PRINTF("\n\r error HW0 IRQ\n");
  exception_handle(cause);
}


/************************************************************************
* NMI ISR. useless
* param:  void
* return: no
************************************************************************/
void IRQNMIISR(u32 cause)         // unmaskable interrupt handler
{
  //OS_PRINTF("\n\r error NMI IRQ\n");
  exception_handle(cause);
}

void nos_irq_dispatch(u32 exc_cause)
{
#if 0
  register u32 $KT0 __asm__ ("$26");
  u32 IRQNum = MAXINTERRUPTS-1;

  OSIntCtxSwFlag = 0;

  while (IRQNum >= 0) {
    if (exc_cause & hw_irq_mask[IRQNum]) {
      (*(hw_irq_vector[IRQNum]))(exc_cause);
      break;
    }
    IRQNum--;
  }
  
  OSIntExit();
  
  if(OSIntCtxSwFlag == 1){
    // Call user-definable
    OSTaskSwHook();
    
    OSTCBCur = OSTCBHighRdy;
    OSPrioCur = OSPrioHighRdy;
  }
  
  //restore the mask_saved__ var from cop0's mask_saved register
  //GET_MASK_SAVED(mask_saved__);
  GET_SR_SAVED(sr_saved__);
   
  /* Get the stack pointer of the task to resume*/
if(  $SP != (OS_REGS *)OSTCBHighRdy->OSTCBStkPtr) {
    //msg("y");
    $SP = (OS_REGS *)OSTCBHighRdy->OSTCBStkPtr;
}
  RESTORE_ALL($KT0);
  ERET($KT0);
#endif
}


void nos_hw_irq_install(u32 IRQNum, HW_IRQ_HANDLE ISRFun, u32 ISRMask)
{
  if (IRQNum < 0 || IRQNum >= MAXINTERRUPTS)
    return;
  
  /* Replace with new ISR function */
  hw_irq_vector[IRQNum] = ISRFun;
  hw_irq_mask[IRQNum] = ISRMask;
}


void __os_irq_request(u8 irq, void (*handle)(void), u8 type)
{
  u32 i = 0;
  list_head_t *p_temp = NULL;
  nos_irq_handle_t *p_handle_node = NULL;
  
  /* find the unusing space to store the handle */
  for(i = 0; i < IRQ_SOURCE_MAX_NUM; i++)
  {
    if(test_and_set_bit(i, irq_info.handle_info.used) == 0)
    {
      break;
    }
  }
  MT_ASSERT(i != IRQ_SOURCE_MAX_NUM);

  /* check if this function has been registered */
  list_for_each(p_temp, &irq_info.que_head[irq])
  {
    p_handle_node = list_entry(p_temp, nos_irq_handle_t, que);
    if(p_handle_node->func == (u32)handle)
    {
      return;
    }
  }
  /* add node to the handle list of this IRQ */
  list_add_tail(&irq_info.handle_info.handle[i].que, &irq_info.que_head[irq]);
  irq_info.handle_info.handle[i].func = (u32)handle;
  irq_info.handle_info.handle[i].index = i;

  hal_irq_int_enable(irq);
}

void __os_irq_release(u8 irq, void (*handle)(void))
{
  u32 i = 0;
  list_head_t *temp = NULL;
  list_head_t *node = NULL;
  nos_irq_handle_t *handle_node = NULL;

  if(NULL == handle)
  {
    /* release all handles registerd on this IRQ line */
    list_for_each_safe(node, temp, &irq_info.que_head[irq])
    {
      handle_node = list_entry(node, nos_irq_handle_t, que);
      list_del(node);
      handle_node->func = 0;
      clear_bit(handle_node->index, irq_info.handle_info.used);
    }
    MT_ASSERT(list_empty(&irq_info.que_head[irq]));
    hal_irq_int_disable(irq);
    return;
  }
  
  /* find the apecific handle to remove */
  for(i = 0; i < IRQ_SOURCE_MAX_NUM; i++)
  {
    if(irq_info.handle_info.handle[i].func == (u32)handle)
    {
      break;
    }
  }
  if(i == IRQ_SOURCE_MAX_NUM)
  {
    /* no handle match, disable this int and return */
    hal_irq_int_disable(irq);
    return;
  }

  /* remove node for handle list */
  irq_info.handle_info.handle[i].func = 0;
  list_del(&irq_info.handle_info.handle[i].que);
  clear_bit(i, irq_info.handle_info.used);

  if(list_empty(&irq_info.que_head[irq]))
  {
    /* if has not sharing source handles, release this IRQ line */
    hal_irq_int_disable(irq);
  }
}



/************************************************************************
* IRQ Int Init
* param:  no
* return: no
************************************************************************/
void __os_irq_init(void)
{
  u8 i = 0;

  memset(&irq_info, 0x00, sizeof(os_irq_info_t));
  for(i = 0; i < IRQ_USER_INT_NUM; i++)
  {
    /* empty the handle list */
    list_init(&irq_info.que_head[i]);
  }
  
  hal_hw_int_enable(3);
  hal_hw_int_enable(2);
  hal_hw_int_enable(1);
  hal_hw_int_enable(0);

  // install the excpetion and interruption handler
 nos_hw_irq_install(0, IRQSyscallISR, EXC_SYSCALL | EXC_BREAK | EXC_COP0 | EXC_OVERFLOW);
 nos_hw_irq_install(1, IRQInvalidInstISR, EXC_ILLEGAL_INST);
 nos_hw_irq_install(2, (HW_IRQ_HANDLE)IRQHW3ISR, EXC_HW3);
 nos_hw_irq_install(3, (HW_IRQ_HANDLE)IRQHW2ISR, EXC_HW2);
 nos_hw_irq_install(4, IRQHW1ISR, EXC_HW1);
 nos_hw_irq_install(5, IRQHW0ISR, EXC_HW0);
 nos_hw_irq_install(6, IRQNMIISR, EXC_NMI);
}

void __os_irq_enable(BOOL enable)
{
  if(enable)
  {
    //SET_SR(1);  
  }
  else
  {
    //SET_SR(0);
  }
}

BOOL __os_irq_set_trigger(u8 irq, irq_trg_t type)
{
  u32 dtmp = 0;

  dtmp = hal_get_u32((volatile u32 *)R_IRQ_MODE);
  
  //OS_PRINTF("\r\os_irq_set_trigger: irq is %d type is %d", irq, type);

  if (HIGH_LEVEL_TRIGGER == type)                                //High Level Triger
  {
    dtmp &= ~(0x1 << irq);
    dtmp |= (0x1 << (irq + 16));
  }
  else if (RISE_EDGE_TRIGGER == type)                           //Rise Edge Triger
  {
    dtmp |= (0x1 << irq);
    dtmp |= (0x1 << (irq + 16));
  }
  else if (FALL_EDGE_TRIGGER == type)                           //Fall Edge Triger
  {
    dtmp |= (0x1 << irq);
    dtmp &= ~(0x1 << (irq + 16));
  }
  else if (LOW_LEVEL_TRIGGER == type)                           //Low Level Triger
  {
    dtmp &= ~(0x1 << irq);
    dtmp &= ~(0x1 << (irq + 16));
  }
  else
  {
    //OS_PRINTF("\n Unknown irq type %d\n", type);
  }

  hal_put_u32((volatile u32 *)R_IRQ_MODE,dtmp);
  return TRUE;
}




void os_irq_request(u8 irq, void (*handle)(void), u8 type)
{
  __os_irq_request(irq, handle, type);
}

void os_irq_release(u8 irq, void (*handle)(void))
{
  __os_irq_release(irq, handle);
}


void os_irq_init(void)
{
  __os_irq_init();
  
}

void os_irq_enable(BOOL enable)
{
  __os_irq_enable(enable);
}

BOOL os_irq_set_trigger(u8 irq, irq_trg_t type)
{
  BOOL result;
  
  result = __os_irq_set_trigger(irq, type);
  
  return result;
}

/********************** for BHR *******************/
/* bhr status */
#define BHR_STATE_NA      0x0   // not available
#define BHR_STATE_WAIT      0x2   // created, but not active
#define BHR_STATE_ACTIVE    0x4   // active, will excute the handle
#define BHR_STATE_RESTARTING  0x100 // user want to restart timer
#define BHR_STATE_DESTROYING  0x200 // user want to destroy timer

typedef void (*BHR_ENTRY)(void*);

/* bhr control block */
typedef struct bhr_ctrl_blk 
{
  list_head_t que;
  BHR_ENTRY func;
  void *param;
  u8 state;
} bhr_cb_t;

typedef struct nos_bhr_priv
{
  list_head_t que_head;
  os_sem_t sem;
  u16 max_num;
  u16 num;
  bhr_cb_t cb_tbl[IRQ_USER_INT_NUM]; 
} nos_bhr_priv_t;

extern void nos_critical_enter(u32 *p_sr);
extern void nos_critical_exit(u32 sr);
extern BOOL nos_sem_create(os_sem_t *p_sem, u16 sem_count);
extern BOOL nos_sem_give(os_sem_t *p_sem);
extern BOOL nos_sem_take(os_sem_t *p_sem, u16 ms, u8 *err);
extern BOOL nos_task_create(u8 *p_taskname,
                     void (*p_taskproc)(void *p_param),
                     void  *p_param,
                     u32    nprio,
                     u32   *pstk,
                     u32    nstksize);
extern void *nos_malloc(u32 size);
extern void nos_free(void *p_addr);

static nos_bhr_priv_t g_bhr_priv;

static void bhr_task(void *param)
{
  list_head_t *p_node = NULL;
  bhr_cb_t *p_bhr_cb = NULL;
  u32 sr;
  u8 error;

  while(1) 
  {
   nos_sem_take(&g_bhr_priv.sem, 0, &error);

    nos_critical_enter(&sr);
    p_node = g_bhr_priv.que_head.p_next;
    if (p_node != &g_bhr_priv.que_head) 
    {
      p_bhr_cb = (bhr_cb_t *)p_node;
      list_del(&(p_bhr_cb->que)); // release from hsr_que
      nos_critical_exit(sr);
      
      p_bhr_cb->func(p_bhr_cb->param);

      nos_critical_enter(&sr);

      if (p_bhr_cb->state & BHR_STATE_DESTROYING) 
      {
        p_bhr_cb->state = BHR_STATE_NA;
      }
      else 
      {
        if (p_bhr_cb->state & BHR_STATE_RESTARTING) 
        {
          p_bhr_cb->state = BHR_STATE_ACTIVE;
          list_add_tail(&p_bhr_cb->que, &g_bhr_priv.que_head);
         nos_sem_give(&g_bhr_priv.sem);
        }
        else
        {
          p_bhr_cb->state = BHR_STATE_WAIT;
        }
      }
    }
    nos_critical_exit(sr);
  }
}


static BOOL nos_irq_bhr_create(mtos_bhr_t *p_bhr,
      void (*routine)(void *),
      void *param)
{
  bhr_cb_t *p_bhr_cb = NULL;
  u16 bhr_id = 0;
  u32 sr = 0;

  nos_critical_enter(&sr);
  
  // search for unused entry
  for (bhr_id = 0; bhr_id < g_bhr_priv.max_num; bhr_id++) 
  {
    if (g_bhr_priv.cb_tbl[bhr_id].state == BHR_STATE_NA)
      break;
  }
  if (bhr_id == g_bhr_priv.max_num) 
  { // entry all used
    MT_ASSERT(0);
    nos_critical_exit(sr);
    return FALSE;
  }

  p_bhr_cb = &g_bhr_priv.cb_tbl[bhr_id];

  list_init(&p_bhr_cb->que);
  p_bhr_cb->func = routine;
  p_bhr_cb->param = param;
  *p_bhr = (mtos_bhr_t)p_bhr_cb;
  g_bhr_priv.num += 1;
  p_bhr_cb->state = BHR_STATE_WAIT;
  nos_critical_exit(sr);

  return TRUE;
}

static BOOL nos_irq_bhr_start(mtos_bhr_t *p_bhr)
{
  bhr_cb_t *p_bhr_cb = (bhr_cb_t *)*p_bhr;
  u32 sr = 0;

  if(p_bhr_cb < g_bhr_priv.cb_tbl || 
    p_bhr_cb >= (g_bhr_priv.cb_tbl + g_bhr_priv.max_num * sizeof(bhr_cb_t)))
  { 
    return FALSE;
  }
  if(p_bhr_cb->state == BHR_STATE_NA)
  { 
    return FALSE;
  }

  nos_critical_enter(&sr);
  
  if (p_bhr_cb->state == BHR_STATE_WAIT) 
  {
    p_bhr_cb->state = BHR_STATE_ACTIVE;
    list_add_tail(&p_bhr_cb->que, &g_bhr_priv.que_head);
   nos_sem_give(&g_bhr_priv.sem);
  }
  else 
  {
    p_bhr_cb->state |= BHR_STATE_RESTARTING;
  }
  nos_critical_exit(sr);
  return TRUE;
}

static BOOL nos_irq_bhr_destroy(mtos_bhr_t *p_bhr)
{
  bhr_cb_t *p_bhr_cb = (bhr_cb_t *)*p_bhr;
  u32 sr = 0;

  if(p_bhr_cb < g_bhr_priv.cb_tbl || 
    p_bhr_cb >= (g_bhr_priv.cb_tbl + g_bhr_priv.max_num * sizeof(bhr_cb_t)))
  { 
    return FALSE;
  }
  if(p_bhr_cb->state == BHR_STATE_NA)
  { 
    return FALSE;
  }

  nos_critical_enter(&sr);

  if (p_bhr_cb->state == BHR_STATE_WAIT) 
  {
    p_bhr_cb->state = BHR_STATE_NA;
  }
  else 
  {
    p_bhr_cb->state |= BHR_STATE_DESTROYING;
  }

  g_bhr_priv.num -= 1;
  nos_critical_exit(sr);
  return TRUE;
}

static void nos_bhr_init(u32 max_routine_num, u32 bhr_prio, u32 stack_ptr, u32 stack_size)
{
  /* init bhr */
  memset(&g_bhr_priv, 0x00, sizeof(nos_bhr_priv_t));
  g_bhr_priv.max_num = max_routine_num;
  list_init(&g_bhr_priv.que_head);
 nos_sem_create(&g_bhr_priv.sem, FALSE);
 nos_task_create("bhr_task", bhr_task, &g_bhr_priv,
    bhr_prio, (u32 *)stack_ptr, stack_size);  
}
/********************** end of BHR *******************/

void nos_irq_handler_init(mtos_cfg_t *p_cfg)
{
  mtos_int_t *p_m_int = NULL; 
  
  p_m_int = mtos_get_comp_handler(MTOS_INT);
  p_m_int->irq_enable = os_irq_enable;
  p_m_int->irq_init = os_irq_init;
  p_m_int->irq_release = os_irq_release;
  p_m_int->irq_request = os_irq_request;
  p_m_int->irq_set_trigger = os_irq_set_trigger;
  if(p_cfg->enable_bhr != TRUE)
  {
    p_m_int->bhr_create = NULL;
    p_m_int->bhr_destroy = NULL;
    p_m_int->bhr_start= NULL; 
    return;
  }
  p_m_int->bhr_create = nos_irq_bhr_create;
  p_m_int->bhr_destroy = nos_irq_bhr_destroy;
  p_m_int->bhr_start= nos_irq_bhr_start;
  nos_bhr_init(p_cfg->max_routine_num,
    p_cfg->bhr_prio, 
    p_cfg->bhr_stack_ptr,
    p_cfg->bhr_stack_size);
}

