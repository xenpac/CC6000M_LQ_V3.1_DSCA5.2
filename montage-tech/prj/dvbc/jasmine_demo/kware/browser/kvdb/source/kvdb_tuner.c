
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
#include "mtos_msg.h"
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"
#include "uio.h"
#include "smc_op.h"
#include "lib_util.h"
#include "lpower.h"

#include "kvdb_tuner.h"
#include "kvdb_event.h"
#include "browser_adapter.h"
#define KVDB_TUNER_DEBUG_PRINTF
#ifdef KVDB_TUNER_DEBUG_PRINTF
#define KVDB_TUNER_DEBUG OS_PRINTF
#else
#define KVDB_TUNER_DEBUG DUMMY_PRINTF
#endif

static unsigned int           kvdb_tuner_event=0;
static unsigned short	 kvdb_init_pid = 0;
static BOOL status = FALSE;
static nim_device_t *p_nim_dev;
static  u32 *p_stack = NULL;

extern bowser_adapter_priv_t g_browser_priv;


#if 1
#define KVDB_TURNER_TASK_SIZE 5*1024
static void tuner_task(void *param)
{	
          nim_channel_perf_t perf;  	
         KVDB_TUNER_DEBUG("%s, %d \n ", __FUNCTION__, __LINE__); 
	do 
	{
	    nim_channel_perf(p_nim_dev, &perf);
		 if(0== perf.lock)//信号中断!!!!!!!
		{
		    status = FALSE;
			kvdb_tuner_event=TUNER_SIGNAL_STOP;
           
		}
		else //信号中断后恢复了!!!!!!!!!!!!
		{
		    if(!status)  
          {  
              status = TRUE;
  			    kvdb_tuner_event=TUNER_SIGNAL_RESUME;
          }
		}
      mtos_task_sleep(40);
	}while(1);
	
}


int kdvb_tuner_task_create(u32 task_priority)
{
    KVDB_TUNER_DEBUG("%s, %d \n ", __FUNCTION__, __LINE__); 

    if(status == TRUE)
    {
          p_stack = mtos_malloc(KVDB_TURNER_TASK_SIZE);

          if(p_stack)
            {
                s32  ret = mtos_task_create((u8*)"turner_monitor",
                                                                    tuner_task,
                                                                          (void * )0,
                                                                              task_priority,
                                                                                            p_stack,
                                                                                                  KVDB_TURNER_TASK_SIZE);
                  if(!ret)
                    {
                          KVDB_TUNER_DEBUG("os task err ret = 0x%x\n", ret);
                          MT_ASSERT(0);
                    }
            }
       }
  return SUCCESS;
}

#endif
int kvdb_porting_delivery_tune(int frequency, int symbol_rate, int modulation)
{
    RET_CODE ret;
    nim_channel_info_t channel_info;
    memset(&channel_info, 0x0, sizeof(nim_channel_info_t));
    channel_info.frequency = frequency/10;
    channel_info.param.dvbc.symbol_rate = symbol_rate/10;
    if(modulation == 1)
      {
         channel_info.param.dvbc.modulation = NIM_MODULA_QAM16;
      }
    else if(modulation == 2)
      {
         channel_info.param.dvbc.modulation = NIM_MODULA_QAM32;

      }
    else if(modulation ==3)
      {
         channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;

      }
    else if(modulation == 4 )
      {
         channel_info.param.dvbc.modulation = NIM_MODULA_QAM128;

      }
    else
      {
         channel_info.param.dvbc.modulation = NIM_MODULA_QAM64;
      }
      ret = nim_channel_connect(p_nim_dev, &channel_info, 0); 
  	  MT_ASSERT(SUCCESS == ret);  
	if(1 == channel_info.lock)
	{
	      kvdb_tuner_event =TUNER_DELIVERY_SUCCESS; 
	}
	else
	{
	      kvdb_tuner_event = TUNER_DELIVERY_FAILED;
	}	

	return SUCCESS;
}

void kvdb_set_starup_pid(u16 pid)
{
      kvdb_init_pid = pid;
}
void kvdb_set_turn_status( unsigned int event)
{
    kvdb_tuner_event = event;
    if(kvdb_tuner_event ==TUNER_DELIVERY_SUCCESS )
      {
            status = TRUE;
      //      kdvb_tuner_task_create(g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start + 3);
    }
}
unsigned short kvdb_porting_get_startup_pid(void)
{

    KVDB_TUNER_DEBUG("%s %d\n", __FUNCTION__, __LINE__);
	return kvdb_init_pid;
	
}

int kvdb_tuner_init(void)
{
    p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                  DEV_IDT_TYPE,
                                                  SYS_DEV_TYPE_NIM);
    MT_ASSERT(p_nim_dev != NULL);
      

	return SUCCESS;
}

void kvdb_tuner_deinit(void)
{
      mtos_task_delete(g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start + 3);
      if(p_stack)
        {

             mtos_free(p_stack);
        }
}
#if 1
int kvdb_tuner_event_get(unsigned int *event)
{
	*event=kvdb_tuner_event;
	kvdb_tuner_event=0;
	return 1;
}
#endif

