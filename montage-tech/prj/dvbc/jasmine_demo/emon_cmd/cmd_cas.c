/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/


#include <stdio.h>
#include <string.h>

#include "ui_common.h"
#include "fw_common.h"
#include "cas_manager.h"
#include "cas_ware.h"
#include "config_cas.h"
#include "ap_cas.h"
#include "sys_status.h"
#include "only1.h"
#include "emon_cmd.h"
#include "ui_finger_print.h"
extern void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size);

extern finger_msg_t g_p_finger_info;

#if 0
static int show_finger_smc(char *param)
{
    u32 ret;
    u8 sn[64];	
    /*get the card sn*/
    ret = CDCASTB_GetCardSN(sn);
    if(ret != 0)
	return 0;    
   // FingerInfo = mtos_malloc(sizeof(finger_msg_t));
   // MT_ASSERT(p_zip_sys != NULL);
    memcpy(FingerInfo.data, sn, 64);		
    FingerInfo.show_color[0] = 0;
    FingerInfo.show_color[1] = 0;
    FingerInfo.show_color[2] = 0;

    FingerInfo.show_color_back[0] = 0;
    FingerInfo.show_color_back[1] = 0;
    FingerInfo.show_color_back[2] = 0xff;

    FingerInfo.start_time[0] = 50;
    FingerInfo.start_time[1] = 50;
    FingerInfo.during = 60000;

    FingerInfo.transparency = 50;
    FingerInfo.byType = 3;
    FingerInfo.position_num =1;
    FingerInfo.position[0].m_dwX = 300;
    FingerInfo.position[0].m_dwY = 300;	

    fw_post_notify( NOTIFY_T_MSG, ROOT_ID_BACKGROUND, MSG_EMON_SMC_FINGER, 0,(u32)&FingerInfo);
     return SUCCESS;
}
#endif
static RET_CODE finger_print(void)
{
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER);
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_SHOW_FP);
    fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_CLOSE_FP);
	
    if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
    {
      fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
    }
    open_finger_print(0, (u32)(&g_p_finger_info));
      if(g_p_finger_info.during)
       fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER, (u32)(g_p_finger_info.during * 60 * 1000), FALSE);
      
      if(g_p_finger_info.start_time[0])
        fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_SHOW_FP, (u32)(g_p_finger_info.start_time[0] * 60 * 1000), TRUE);
      else
        fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_SHOW_FP, (u32)(60 * 1000), TRUE);
  
  return SUCCESS;
}

static int show_finger(char *param)
{
    int flag;

    if(EMON_CheckCommand(param, 1))
    {
    	sscanf(param,"%d", &flag);
    }	
    switch(flag)
    {
	case 0:
		fw_post_notify( NOTIFY_T_MSG, ROOT_ID_BACKGROUND, MSG_EMON_HIDE_FINGER, 0,0);
              return SUCCESS;
		break;
	case 1:
		g_p_finger_info.byType = 1;
		break;
	case 2:
		g_p_finger_info.byType = 2;
		break;
	default:
		OS_PRINTF("ERROR PARAM\n");
    }
     finger_print();	
     return SUCCESS;
}

static int  set_font_size(char *param)
{
     int width, height;
 
     if(EMON_CheckCommand(param, 2))
     {
	 sscanf(param,"%d %d", &width, &height);	
     }
     g_p_finger_info.only1_font_width= width;
     g_p_finger_info.only1_font_height = height;
     finger_print();
     return SUCCESS;
}

static int  set_font_colour(char *param)
{
     int p_red,p_green,p_blue;
     int f_red,f_green,f_blue;

     if(EMON_CheckCommand(param, 6))
     {
	 sscanf(param,"%d %d %d %d %d %d", &f_red, &f_green,&f_blue,&p_red,&p_green,&p_blue);	
     }
    g_p_finger_info.show_color[0] = (u8)f_red;
    g_p_finger_info.show_color[1] = (u8)f_green;
    g_p_finger_info.show_color[2] = (u8)f_blue;

    g_p_finger_info.show_color_back[0] = (u8)p_red;
    g_p_finger_info.show_color_back[1] = (u8)p_green;
    g_p_finger_info.show_color_back[2] = (u8)p_blue;
    finger_print();	
    return SUCCESS;	
}

static int  set_transparent(char *param)
{
     int transparent;
     
     if(EMON_CheckCommand(param, 1))
     {
	 sscanf(param,"%d", &transparent);	
     }
     g_p_finger_info.transparency= transparent;

     finger_print();
     return SUCCESS;	 
}

static int  set_position(char *param)
{
     int i =0;
     int position_x[4] = {0};
     int position_y[4] = {0};	 
     
     if(EMON_CheckCommand(param, 2))
     {
        g_p_finger_info.position_num = 1;
	 sscanf(param,"%d %d", &(position_x[0]), &(position_y[0]));	
     }
     else if(EMON_CheckCommand(param, 4))
     {
        g_p_finger_info.position_num = 2;
	 sscanf(param,"%d %d %d %d", &(position_x[0]), &(position_y[0]),&(position_x[1]), &(position_y[1]));	
     }	
     else if(EMON_CheckCommand(param, 6))
     {
        g_p_finger_info.position_num = 3;
	 sscanf(param,"%d %d %d %d %d %d", &(position_x[0]), &(position_y[0]),&(position_x[1]),&(position_y[1]),
	 	  &(position_x[2]), &(position_y[2]));	
     }	
     else if(EMON_CheckCommand(param, 8))
     {
        g_p_finger_info.position_num = 4;
	 sscanf(param,"%d %d %d %d %d %d %d %d", &(position_x[0]), &(position_y[0]),&(position_x[1]), &(position_y[1]),
	 	  &(position_x[2]), &(position_y[2]),&(position_x[3]), &(position_y[3]));	
     }	
     else
     {
	 OS_PRINTF("parameter count error\n");
     }
     for(i=0;i<g_p_finger_info.position_num;i++)
     {
	 g_p_finger_info.position[i].m_dwX = (u32)position_x[i];
	 g_p_finger_info.position[i].m_dwY = (u32)position_y[i];	
     }
     finger_print();
     return SUCCESS;	 
}

static int set_time(char *param)
{
     int unit,duration;
     int total;
     
     if(EMON_CheckCommand(param, 3))
     {
	 sscanf(param,"%d %d %d", &unit, &duration,&total);	
     }
     g_p_finger_info.start_time[0]= (u8)unit;
     g_p_finger_info.start_time[1]= (u8)duration;
     g_p_finger_info.during = (u16)total;	 
     finger_print();
     return SUCCESS;	 
}

static EMON_COMMAND_LIST cmd_cas[]=
{
  {show_finger, "fp_pair", "flag 0 hide finger,1 smc,2 stbid(ctrl+c to quit)",FALSE},
  {set_font_size, "fp_setfontsize", "set finger font size w and h(ctrl+c to quit)",FALSE},
  {set_font_colour, "fp_setcolor", "set font colour font R G B back R G B(ctrl+c to quit)",FALSE},
  {set_transparent,"fp_settransparent","set finger transparent ",FALSE},
  {set_position,"fp_setposition","set finger position x y x y x y x y max position is 4 ",FALSE}, 
  {set_time,"fP_settime","set finger display unit during total ",FALSE},
  {0, 0, 0, 0},
};

int cmd_cas_init(void)
{
  return EMON_Register_Test_Functions(cmd_cas, "cas");
}

