/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
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

#include "browser_adapter.h"

#include "ap_framework.h"
#include "ap_browser.h"

#include "kvdb_event.h"
#include "kvdb_graphic.h"
#include "kvdb_tuner.h"
#define BROWSER_MEM_SIZE (16* 1024*1024)
typedef enum
{
	BROWSER_STATUS_EXIT = 0,
	BROWSER_STATUS_READY,
	BROWSER_STATUS_START,
	BROWSER_STATUS_PAUSE,
	BROWSER_STATUS_STOP,
	BROWSER_STATUS_END
}BrowserStatus_t;

#define BROWSER_IPANEL_DEBUG
#ifdef BROWSER_IPANEL_DEBUG
#define BROWSER_PRINTF OS_PRINTF
#else
#define BROWSER_PRINTF DUMMY_PRINTF
#endif
//lib_memp_t g_ipanel_memp;
extern bowser_adapter_priv_t g_browser_priv;
extern void kvdb_input_init(void );
extern int kvdb_tuner_init(void);

extern void mid_kvdb_demux_deinit(void);
extern void kvdb_tuner_deinit(void);
extern void mid_kvdb_demux_deinit(void);

//浏览器运行状态
BrowserStatus_t	 m_BrwRunning = 0;

extern void  send_message_to_tail(u32 e0, u32 e1, u32 e2);
void  set_ipanel_exit(void)
{
	m_BrwRunning = BROWSER_STATUS_EXIT;
	return ;
}

RET_CODE kvdb_ipanel_init(void )
{
  /* init browser ipanel2.8 module */  
   kvdb_osddrv_init();
  kvdb_input_init();
//  kvdb_tuner_init();
 //ipanel_porting_demux_init();     
  // ipanel_tuner_init();  
   //ipanel_key_init();
   return SUCCESS;
}


static RET_CODE kvdb_ipanel_deinit(void)
{
  //  ipanel_porting_demux_exit();
   //ipanel_input_deinit()
    BROWSER_PRINTF("%s, %d \n", __FUNCTION__, __LINE__);
    kvdb_osddrv_deinit();
   kvdb_tuner_deinit();
    mid_kvdb_demux_deinit();
    browser_send_event(0, 0,BROWSER_O_EXIT, 0);
    return SUCCESS;
}
#if 0
void ipanel_main_proc(void)
{

  int ret;
  unsigned int  event[3];
  static BrowserStatus_t lastStat = BROWSER_STATUS_EXIT;
  while(m_BrwRunning != BROWSER_STATUS_EXIT)
 {
	if((m_BrwRunning == BROWSER_STATUS_START) && ((lastStat == BROWSER_STATUS_PAUSE) || (lastStat == BROWSER_STATUS_EXIT)))
	{
		BOOL is_mute_flag;
		is_mute_flag = ipanel_get_mute_state();
		ipanel_ioctl(pDtvhandle,IPANEL_AUDIO_VOLUME,&is_mute_flag);		
	}
	lastStat = m_BrwRunning;	
	ret = Input_Recv(event,0);
	if(ret==0) 
	{
		if(event[0] == IPANEL_EVENT_TYPE_IRKEY)
		{
			//收到消息时 break while(1)
			if(event[1] == IPANEL_IRKEY_POWER || event[1] == IPANEL_IRKEY_STANDBY)
			{
				break;
			}
			else if(event[1] == IPANEL_IRKEY_EXIT)
			{
				BROWSER_PRINTF("##########IPANEL_IRKEY_EXIT##########\n");
			}
			else if(event[1] == IPANEL_IRKEY_MENU)
			{					
					ipanel_open_uri(pDtvhandle,chUrl);
					continue;
			}
		}
		
		ipanel_proc(pDtvhandle,event[0],event[1],event[2]);
	}
	if(m_BrwRunning == BROWSER_STATUS_START)
	{
		ipanel_porting_task_sleep(1);
	}
	else
	{
		ipanel_porting_task_sleep(100);
	}
	ipanel_proc(pDtvhandle,0,0,0);
  }
    ipanel28_deinit();
    ipanel_porting_task_destroy(ipanel_main_task_handle);
  }
#endif
RET_CODE kdvb_ipanel_open(void *param)
{
//   browser_io_param_t * p_browser =(browser_io_param_t *)param;
  extern int  kvdb_mw_go(void);
  extern   void mid_kvdb_demux_init();
extern void kvdb_set_turn_status( unsigned int event);

 extern  void kvdb_catch_table(u16 pid, u16 table_id);

  kvdb_ipanel_init();
  mid_kvdb_demux_init();
  kvdb_mw_go();
  //kvdb_set_turn_status(0x500);
 kvdb_catch_table(0,0);
  

  #if 0
 //  browser_pg_para *p_pg_param = (browser_pg_para *)p_browser->p_param;
   //BROWSER_PRINTF("\n net_id:%d,ts_id:%d,s_id:%d",p_pg_param->net_id, p_pg_param->ts_id, p_pg_param->s_id);
   ipanel28_init(p_browser);
   m_BrwRunning = BROWSER_STATUS_START;
  /* init ad data */
  pDtvhandle	= ipanel_create(p_browser ->mem_size); //12* 1024*1024 + 512 * 1024;
  sprintf(chUrl, "UI://init.htm?dvb://%d.%d.%d/", 5, 78, 51);
 //sprintf(chUrl, "UI://init.htm?dvb://%d.%d.%d/", p_pg_param->net_id, p_pg_param->ts_id, p_pg_param->s_id);
  BROWSER_PRINTF("chUrl === %s\n",chUrl);
  ipanel_open_uri(pDtvhandle,chUrl);  

  ipanel_main_task_handle =ipanel_porting_task_create((CONST CHAR_T*)"ipanel_proc",
                                             (IPANEL_TASK_PROC)ipanel_main_proc,
                                              NULL, 30,
                                              30*1024);
  #endif
    return SUCCESS;
}

static RET_CODE kvdb_ipanel_close(void)
{
   m_BrwRunning = BROWSER_STATUS_EXIT;  
  return SUCCESS;
}

static RET_CODE kvdb_ipanel_resume(void)
{
  return SUCCESS;
}

static RET_CODE kvdb_ipanel_pause(void)
{
  return SUCCESS;
}

static RET_CODE kvdb_ipanel_io_ctrl(u32 cmd, void *param)
{
  browser_io_param_t *p_param = (browser_io_param_t *) param;
  switch(cmd)
  {
    case CMD_START_BROWSER:
      BROWSER_PRINTF("\n CMD_START_BROWSER \n");
      kdvb_ipanel_open(p_param);
      break;
    case CMD_SEND_IRKEY_MSG:
      BROWSER_PRINTF("\n CMD_SEND_IRKEY_MSG \n");
      send_message_to_tail(KEY_MESSAGE,p_param->key_event,0);
      break;
    default:
      break;
  }
  return SUCCESS;
}
RET_CODE kvdb_ipanel_attach(browser_module_cfg_t *p_cfg)
{
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].attached = 1;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].inited = 0;

    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.init = NULL/*ipanel28_init*/;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.deinit = kvdb_ipanel_deinit;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.open = kdvb_ipanel_open;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.close = kvdb_ipanel_close;  
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.resume = kvdb_ipanel_resume;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.pause = kvdb_ipanel_pause;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].func.io_ctrl = kvdb_ipanel_io_ctrl;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_start = p_cfg->task_prio;
    g_browser_priv.adm_op[BROWSER_ID_IPANEL28].task_pro_end = p_cfg->task_end_pro;
    

  return SUCCESS;
}

