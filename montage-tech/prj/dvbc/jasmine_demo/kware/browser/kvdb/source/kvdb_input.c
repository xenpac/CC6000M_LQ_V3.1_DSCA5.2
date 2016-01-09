/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
 #include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_mutex.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "kvdb_event.h"

#define MAX_EVENT 12
#define MAX_KEY 4

typedef struct EVENT_T {
	int event[3];
	struct EVENT_T* next;
//	struct EVENT_T* prev;
}EVENT_T;

typedef struct INPUT_EVENT {
	EVENT_T event[MAX_EVENT];
	EVENT_T* read;
	EVENT_T* write;
	unsigned int cnt;
	u32    e_lock;
}INPUT_EVENT;
struct INPUT_EVENT porting_event;
//struct pan_device * kvdb_gppan = NULL;
#define KVDB_INPUT_DEBUG_PIRNTF
#ifdef  KVDB_INPUT_DEBUG_PIRNTF
#define KVDB_INPUT_DEBUG OS_PRINTF
#else
#define KVDB_INPUT_DEBUG DUMMY_PRINTF
#endif
#if 0
static void kvdb_input_sem_lock(void )
{
    RET_CODE ret;
    ret = mtos_sem_take((os_sem_t * )&porting_event.e_lock, 0);
    MT_ASSERT(ret != TRUE);
}
static void kvdb_input_sem_unlock(void)
{
    RET_CODE ret;
    ret = mtos_sem_give((os_sem_t * )&porting_event.e_lock);

    MT_ASSERT(ret != TRUE);
}
#endif
RET_CODE get_events(unsigned int event[3])
{
    RET_CODE ret;
  //  kvdb_input_sem_lock();
    if(porting_event.read == porting_event.write)//queue is empty
     {
        event[0] = 0;
        event[1] = 0;
        event[2] = 0;
        porting_event.cnt = 0;
        ret = -1;
     }
    else
      {
          event[0] = porting_event.read->event[0];
          event[1] = porting_event.read->event[1];
          event[2] = porting_event.read->event[2];
          porting_event.cnt --;
          porting_event.read = porting_event.read->next;
          ret = 0;
      }
//    kvdb_input_sem_unlock();
  //  KVDB_INPUT_DEBUG("ret = %d \n", ret);
    return ret;
}

RET_CODE Input_Send( int * event ,int waitmode)
{
      RET_CODE ret;
//      kvdb_input_sem_lock();
      if(porting_event.write->next == porting_event.read)
      {
          ret =-1;
      }
      else if(porting_event.cnt > MAX_KEY)
      {
          ret = -1;
      }
      else
      {
          porting_event.write->event[0] = event[0];
          porting_event.write->event[1] = event[1];
          porting_event.write->event[2] = event[2];
          porting_event.write = porting_event.write->next;
          porting_event.cnt++;
          ret = 0;
      }
   //  kvdb_input_sem_unlock();
     return ret;
}

void send_message_to_tail(int e0,int e1, int e2)
{
    int event[3];
    event[0] = e0;
    event[1] = e1;
    event[2] = e2;
    Input_Send(event, 0);
}
int kvdb_input_event_get(unsigned int event[3])
{

	event[0]=0 ;
	event[1]=0 ;
	event[2]=0 ;
    	
    get_events(event);

	switch(event[1])
	{
    	case IR_KEY_UP :
		   event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_UP ;
        	event[2]=0 ;
        	return 1 ;
    
    	case IR_KEY_DOWN :
		event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_DOWN ;
        	event[2]=0 ;
        	return 1 ;
    
    	case IR_KEY_RIGHT :
		event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_RIGHT ;
        	event[2]=0 ;
        	return 1 ;
    
    	case IR_KEY_LEFT :
		   event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_LEFT ;
        	event[2]=0 ;
        	return 1 ;
    
    	case IR_KEY_OK :
		event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_SELECT ;
        	event[2]=0 ;
        	return 1 ;
			
	case IR_KEY_EXIT :
		    event[0]=KEY_MESSAGE ;
        	event[1]=KV_KEY_EXIT ;
        	event[2]=0 ;
        	return 2 ;

    	default :
        	return 0 ;
    }
    
    return 0 ;
}
void kvdb_input_init(void )
{
 //   RET_CODE ret ;
    u32 index = 0;
//    ret  = mtos_sem_create((os_sem_t * )&porting_event.e_lock, 1);
   // MT_ASSERT(ret != SUCCESS);
     
    memset(&porting_event, 0x0, sizeof(INPUT_EVENT) );
    for(index = 0; index <MAX_EVENT-1; index++)
      {
          porting_event.event[index].next = &porting_event.event[index +1];
      }
    porting_event.event[MAX_EVENT -1].next = &porting_event.event[0];
    porting_event.read = &porting_event.event[0];
     porting_event.write = &porting_event.event[0];
     porting_event.cnt = 0;
}

