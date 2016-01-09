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

#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_mutex.h"
#include "mtos_misc.h"

#include "kvdbdtv_api.h"
#include <kvdb_event.h>

#include "browser_adapter.h"


#define KVDB_TASK_DEBUG_PRINTF
#ifdef KVDB_TASK_DEBUG_PRINTF
#define KVDB_TASK_DEBUG OS_PRINTF
#else
#define KVDB_TASK_DEBUG DUMMY_PRINTF
#endif
static u32 *pstack = NULL;

typedef  void (*kvdb_panel_task)(void *param);
extern bowser_adapter_priv_t g_browser_priv;

#define KVDB_TASK_MESSAGE_SIZE 8*1024
//extern int kvdb_tuner_event_get(unsigned int *event);
extern int  kvdb_input_event_get(unsigned int event[3]);
extern int kvdb_tuner_event_get(unsigned int *event);
	 
static void kvdbdtv_task(void *param)
{
       int ret = 0;
	unsigned short exit = 0;
	unsigned short kvdbhandle = 1;
	unsigned int event[3] = {0};
	unsigned int tuner_event=0;
   KVDB_TASK_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);

	kvdbhandle = KVDB_CREATE_BROWSER();
	if (0 == kvdbhandle)
	{

		while(1) 
		{
			ret = kvdb_input_event_get(event);
			if ((1 == ret)||(2 == ret))
			{
    			   if(ret== 2)             //kingvondb  receive exit message ，destrtoy task   
                {   
                      break;
                }
    			  exit=KVDB_EVENT_PROC(event);
		
            }
			else
			{
				tuner_event=0;
				kvdb_tuner_event_get(&tuner_event);
				if((tuner_event == TUNER_DELIVERY_SUCCESS)	||(tuner_event == TUNER_DELIVERY_FAILED))
				{ //判断锁定没有
				    event[0]=TUNER_MESSAGE ;
	                        event[1]=tuner_event ;
	                        event[2]=0 ;

			          KVDB_EVENT_PROC(event);
				} 
				else if (tuner_event == TUNER_SIGNAL_STOP)
				{  //信号中断!
				             event[0]=TUNER_MESSAGE ;
	                        event[1]=TUNER_SIGNAL_STOP ;
	                        event[2]=0 ;
			           KVDB_EVENT_PROC(event);
				}
				else if (tuner_event == TUNER_SIGNAL_RESUME)
				{//信号恢复!
				    event[0]=TUNER_MESSAGE ;
	                        event[1]=TUNER_SIGNAL_RESUME ;
	                        event[2]=0 ;
			           KVDB_EVENT_PROC(event);
				} 
			}
          mtos_task_sleep(30);
			
		}
		
		KVDB_DESTROY_BROWSER();
       g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.deinit();
       if(pstack)
      {
          mtos_free(pstack);
       }
       mtos_task_exit();

	}
	kvdbhandle =1;
 }

int  kvdb_mw_go(void)
{
    RET_CODE ret;
   KVDB_TASK_DEBUG("%s, %d \n", __FUNCTION__, __LINE__);
    pstack = mtos_malloc(KVDB_TASK_MESSAGE_SIZE);
    MT_ASSERT(pstack != NULL);
    ret  = mtos_task_create((u8 *)"kvdb_task",
                                            kvdbdtv_task,
                                                    (void *)0,
                                        g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start+1,
                                                pstack,
                                                    KVDB_TASK_MESSAGE_SIZE);
    MT_ASSERT(ret != SUCCESS);
	return SUCCESS;
}



