/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_INT_H__
#define __MTOS_INT_H__

/*!
  Interrupt Line Number
  */
#define IRQ_USER_INT_NUM        64

/*!
  The max number of sources shared one interrupt line
  */
#define IRQ_SOURCE_MAX_NUM        64

/*!
  Define the type of interrupt bottom half routine's id
  */
typedef unsigned int mtos_bhr_t;

/*!
  irq triggle type
  */
typedef enum
{
  /*!
    High level trigger
    */
  HIGH_LEVEL_TRIGGER,
  /*!
    Rise edge trigger
    */
  RISE_EDGE_TRIGGER,
  /*!
    Fall edge trigger
    */
  FALL_EDGE_TRIGGER,
  /*!
    low level trigger
    */
  LOW_LEVEL_TRIGGER,
  /*!
   * Both edges trigger
   */
  BOTH_EDGES_TRIGGER  
}irq_trg_t;


/*!
  Initiate the hardware interrupt relative
  
  \param[in] None
  */
void mtos_irq_init(void);

/*!
  request an irq 
  
  \param[in] irq the irq index
  \param[in] handle the irq handler
  \parma[in] type no use now, set to IRQ_INT_TYPE_NORMAL
  */
void mtos_irq_request(u8 irq, void (*handle)(void), u8 type);

/*!
  release an irq
  
  \param[in] irq the irq index
  \param[in] handle the irq handler to be de-registered, 
                   if is NULL, means that you want to release all handles registered on this IRQ line
  */
void mtos_irq_release(u8 irq, void (*handle)(void));


/*!
  Enable/disable hardware irq
  
  \param[in] enable FALSE-disable all irq, TRUE-enable irq
  */
void mtos_irq_enable(BOOL enable);

/*!
  check current is in interrupter mode
  
  \param[out]   -TRUE-for in  irq mode FALSE for not

  \return TRUE if success, FALSE if fail
  
  */
BOOL mtos_irq_check_int(BOOL *p_int);


/*!
  Set irq the trigger type, only useful for external GPIO interrupt now
  
  \param[in] irq the irq index
  \param[in] type the irq trigger type

  \return TRUE if success, FALSE if fail
  */
BOOL mtos_irq_set_trigger(u8 irq, irq_trg_t type);

/*!
  Create a routine used as a bottom half interrupt service.
  
  \param[out] p_bhr The pointer to the caller provide space, used to store handle of BHR(Bottom Half Routine).
  \param[in] routine Function pointer to the entry of BHR.
  \param[in] param The parameter to be transferred to the BHR.
  
  \return TRUE if success, FALSE if fail
  */
BOOL mtos_irq_bhr_create(mtos_bhr_t *p_bhr,
      void (*routine)(void *),
      void *param);

/*!
  Delete a specific routine in bottom half interrupt service.
  
  \param[in] p_bhr The pointer to the handle of BHR want to be destroied.
  
  \return TRUE if success, FALSE if fail
  */
BOOL mtos_irq_bhr_destroy(mtos_bhr_t *p_bhr);

/*!
  Start a routine in a bottom half interrupt service.
  
  \param[in] p_bhr The pointer to the handle of BHR want to be started.
  
  \return TRUE if success, FALSE if fail
  */
BOOL mtos_irq_bhr_start(mtos_bhr_t *p_bhr);

#endif //__MTOS_INT_H__
