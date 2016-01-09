/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef  __MTOS_EVENT_H__
#define  __MTOS_EVENT_H__

/*!
  Event Set flag for match certifications: Any bit match
  */
#define MTOS_EVENT_FLAG_MATCHONE    0
/*!
  Event Set flag for match certifications: All bits matches
  */
#define MTOS_EVENT_FLAG_MATCHALL    0x80000000
/*!
  Event Set flag for post matching action, auto clear matching bit(s)
  */
#define MTOS_EVENT_FLAG_AUTOCLEAR   0x40000000
/*!
  The value to indicate event flag waiting for ever
  */
#define MTOS_WAIT_FOREVER 0xFFFFFFFF

/*!
  Define the event type
  */
typedef unsigned int mtos_evt_t;

/*!
  Create a an Event Set.
  
  \param[out] p_event The pointer to the caller provide space, used to store handle of Event Set.

  \return TRUE if OK, Others if fail
  */
BOOL mtos_event_create(mtos_evt_t *p_event);

/*!
  Delete an Event Set.
  
  \param[in] p_event The pointer to the caller provide space, used to store handle of Event Set.

  \return TRUE if OK, Others if fail
  */
BOOL mtos_event_destroy(mtos_evt_t *p_event);

/*!
  wait on Event Set for it meets specified conditions.
  
  \param[in] p_event The pointer to the caller provide space, used to store handle of Event Set.
  \param[in] type When caller specified multi-conditions, this flag used to determine if all conditions need to be fulfilled.
                          see MTOS_EVENT_FLAG_XXX
  \param[in] flag This is the bit(s) mask to declare one or more conditions. 
  \param[out] p_value The pointer to the caller provided space, used to store the snap-shot of the 
                             Event Set status when conditions met.
  \param[in] timeout This parameters used to define the maximum time, in ticks, 
                              system should wait for the conditions to be fulfilled.
                              use MTOS_WAIT_FOREVER means wait forever

  \return TRUE if OK, Others if fail
  */
BOOL mtos_event_wait(mtos_evt_t *p_event,
      unsigned int type,
      unsigned int flag,
      unsigned int *p_value,
      unsigned int timeout);

/*!
  Set specified bit(s) in Event Set.
  
  \param[in] p_event The pointer to the caller provide space, used to store handle of Event Set.
  \param[in] mask This is the bit(s) pattern want to be set.

  \return TRUE if OK, Others if fail
  */
BOOL mtos_event_set(mtos_evt_t *p_event, unsigned int mask);

/*!
  Query current Event Set.
  
  \param[in] p_event The pointer to the caller provide space, used to store handle of Event Set.
  \param[out] mask current Event Set flags.

  \return TRUE if OK, Others if fail
  */
BOOL mtos_event_query(mtos_evt_t *p_event, unsigned int *mask);


#endif //__MTOS_EVENT_H__

