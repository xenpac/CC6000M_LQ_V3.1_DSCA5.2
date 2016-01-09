/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/


#include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_mem.h"
#include "mtos_task.h"

#include "drv_dev.h"
#include "dmx.h"

#include "sys_app.h"

#include "app_demux.h"
#include "emon_cmd.h"
#include "cmd_dmx.h"

typedef struct cmd_filter_
{
  app_dmx_easy_filter_handle filter;
  u16 pid;
  u8 version;
  u8 section_num;

  struct cmd_filter_ *next;
}cmd_filter;

static cmd_filter *filter_list = NULL;


static cmd_filter *_alloc_filter(void)
{
  cmd_filter *f;

  f = mtos_malloc(sizeof(cmd_filter));
  if (f == NULL)
  {
    return NULL;
  }
  memset(f, 0, sizeof(cmd_filter));

  if (filter_list == NULL)
  {
    filter_list = f;
  }
  else
  {
    cmd_filter *p = filter_list;

    while (p->next != NULL)
    {
      p = p->next;
    }
    p->next = f;
  }

  return f;
}

static RET_CODE _free_filter(cmd_filter *f)
{
  cmd_filter *p;
  cmd_filter *p_pre = NULL;
  
  MT_ASSERT(f != NULL);

  if (f == filter_list)
  {
    filter_list = filter_list->next;
  }
  else
  {
    p = filter_list;
    
    while (p != NULL)
    {
      if (p == f)
      {
        p_pre->next = p->next;
        break;
      }
      p_pre = p;
      p = p->next;
    }
  }
  memset(f, 0, sizeof(cmd_filter));
  mtos_free(f);
  return SUCCESS;
}


static void _data_handler(void *self, u8 *p_data, u32 data_size, void *private_data)
{
  u32 i;
  
  APPLOGI("tid:0x%x, private_data:%p, size:%d",p_data[0], private_data, data_size);
  for (i = 0; i < data_size && i < 16; i++)
  {
    APPLOGI("[0x%x,]",p_data[i]);
  }
  APPLOGI("\n");
}


static int _start_filter(char *param)
{
  #define _MAX_STR_LEN (10)
  RET_CODE ret;
  int pid;
  int version = -1;
  int section_num = -1;
  cmd_filter *f;
  app_dmx_easy_filter_param f_param;
  char str_ver[_MAX_STR_LEN] = "Ignore";
  char str_section_num[_MAX_STR_LEN] = "Ignore";

  if(EMON_CheckCommand(param, 1))
  {
    sscanf(param,"%4d", &pid);
  }
  else if (EMON_CheckCommand(param, 2))
  {
    sscanf(param,"%4d %2d", &pid, &version);
  }
  else if (EMON_CheckCommand(param, 3))
  {
    sscanf(param,"%4d %2d %2d", &pid, &version, &section_num);
  }
  else
  {
    return ERR_FAILURE;
  }
  APPLOGI("[%s][%d]\n",__FUNCTION__,__LINE__);

  f = _alloc_filter();
  
  APPLOGI("[%s][%d]\n",__FUNCTION__,__LINE__);
  memset(&f_param, 0, sizeof(app_dmx_easy_filter_param));
  f_param.pid = pid & 0xFFFF;
  f_param.buffer_size = 0;
  f_param.data_callback = _data_handler;
  f_param.enable_crc = TRUE;
  f_param.one_shot = FALSE;
  f_param.private_data = (void*)f;
  if (version != -1)
  {
    f_param.match[3] = version & 0x1f;
    f_param.mask[3] = 0x1f;
    memset(str_ver, 0, sizeof(str_ver));
    sprintf(str_ver, "%d", version);
    f_param.depth = 4;
  }
  if (section_num != -1)
  {
    f_param.match[4] = section_num & 0xff;
    f_param.mask[4] = 0xff;
    memset(str_section_num, 0, sizeof(str_section_num));
    sprintf(str_section_num, "%d", section_num);
    f_param.depth = 5;
  }

  f->pid = pid;
  f->version = version;
  f->section_num = section_num;
  f->filter = app_dmx_alloc_easy_filter();
  //APPLOGI("[%s][%d]\n",__FUNCTION__,__LINE__);
  ret = app_dmx_set_easy_filter(f->filter, &f_param);
  //APPLOGI("[%s][%d]\n",__FUNCTION__,__LINE__);
  ret = app_dmx_start_easy_filter(f->filter);
  //APPLOGI("[%s][%d]\n",__FUNCTION__,__LINE__);

  if (ret != SUCCESS)
  {
    _free_filter(f);
  }

  APPLOGI("start filter, pid:0x%x, version:%s, section_num:%s, private:%p\n",
          pid, str_ver, str_section_num, f_param.private_data);

  return SUCCESS;
}

static int _stop_filter(char *param)
{
  RET_CODE ret = SUCCESS;
  s32 index = 0;
  cmd_filter *f;
  
  if(!EMON_CheckCommand(param, 1))
  {
    return ERR_FAILURE;
  }
  sscanf(param,"%ld", &index);

  f = filter_list;
  while (index > 0 && f != NULL)
  {
    f = f->next;
    index--;
  }
  if (index != 0 || f == NULL)
  {
    APPLOGE("error index:%d\n", index);
    return SUCCESS;
  }

  ret |= app_dmx_stop_easy_filter(f->filter);
  //APPLOGI("[%s][%d]ret:%d\n",__FUNCTION__,__LINE__, ret);
  ret |= app_dmx_free_easy_filter(f->filter);
  //APPLOGI("[%s][%d]ret:%d\n",__FUNCTION__,__LINE__, ret);

  if (ret == SUCCESS)
  {
    ret = _free_filter(f);
    //APPLOGI("[%s][%d]ret:%d\n",__FUNCTION__,__LINE__, ret);
  }

  return SUCCESS;
}

static int _list_filter(char *param)
{
  cmd_filter *f;
  int index = 0;
  
  f = filter_list;

  APPLOGI("index \t filter \t pid \t ver \t sec \t \n");

  while (f != NULL)
  {
    APPLOGI("%d \t %p \t 0x%x \t 0x%x \t 0x%x \t \n", index, f->filter, f->pid, f->version, f->section_num);
    index++;
    f = f->next;
  }
  OS_PRINTF("\n");

  return SUCCESS;
}

static int _start_stop_filter_repeatly(char *param)
{
  const int max_filters = 20;
  int try_times = 50;
  int ret = SUCCESS;
  int i;
  app_dmx_easy_filter_handle filters[max_filters];
  app_dmx_easy_filter_param f_param;
  u16 pid = 0;

  memset(filters, 0, sizeof(filters));
  memset(&f_param, 0, sizeof(app_dmx_easy_filter_param));
  f_param.pid = pid & 0xFFFF;
  f_param.buffer_size = 0;
  f_param.data_callback = _data_handler;
  f_param.enable_crc = TRUE;
  f_param.one_shot = FALSE;

  do
  {
    APPLOGI("[%s] start filters(%d)\n", __FUNCTION__, max_filters);
    for (i = 0; i < max_filters; i++)
    {
      filters[i] = app_dmx_alloc_easy_filter();
      ret |= app_dmx_set_easy_filter(filters[i] , &f_param);
      ret |= app_dmx_start_easy_filter(filters[i] );
      f_param.private_data = (void*)filters[i];
      if (ret != SUCCESS)
      {
        APPLOGI("[%s] Test Failed at fileter index:%d\n", __FUNCTION__, i);
      }
    }
    APPLOGI("[%s] stop filters(%d)\n", __FUNCTION__, max_filters);
    for (i = 0; i < max_filters; i++)
    {
      ret |= app_dmx_stop_easy_filter(filters[i]);
      ret |= app_dmx_free_easy_filter(filters[i]);
      if (ret != SUCCESS)
      {
        APPLOGI("[%s] Test Failed at fileter index:%d\n", __FUNCTION__, i);
      }
    }
  } while (try_times-- > 0);

  APPLOGI("Test start/stop filter multi times %s\n", ret == SUCCESS ?"Success" : "Failed");
  return SUCCESS;
}


static int _ca_filter_test(char *param)
{
  const int max_filters = 20;
  app_dmx_easy_filter_handle filters[max_filters];
  app_dmx_easy_filter_param f_param;
  u16 pid = 0x8ff;
  int ret = SUCCESS;
  u8 match[] = {0x82, 0x6b, 0xc2, 0x00, 0x02, 0xdf, 0xd8};
  u8 mask[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  int flag;

  if(!EMON_CheckCommand(param, 1))
  {
    return ERR_FAILURE;
  }
  sscanf(param,"%d", &flag);
  if (flag == 0)
  {
      ret |= app_dmx_stop_easy_filter(filters[0]);
      ret |= app_dmx_free_easy_filter(filters[0]);
      return SUCCESS;
  }



  memset(filters, 0, sizeof(filters));
  memset(&f_param, 0, sizeof(app_dmx_easy_filter_param));
  f_param.pid = pid & 0xFFFF;
  f_param.buffer_size = 0;
  f_param.data_callback = _data_handler;
  f_param.enable_crc = TRUE;
  f_param.one_shot = FALSE;
  memcpy(f_param.match, match, sizeof(match));
  memcpy(f_param.mask, mask, sizeof(mask));
  

  do
  {
    APPLOGI("[%s] start filters(%d)\n", __FUNCTION__, max_filters);
    filters[0] = app_dmx_alloc_easy_filter();
    ret |= app_dmx_set_easy_filter(filters[0] , &f_param);
    ret |= app_dmx_start_easy_filter(filters[0] );
    f_param.private_data = (void*)filters[0];
    if (ret != SUCCESS)
    {
        APPLOGI("[%s] [%d]Test Failed\n", __FUNCTION__, __LINE__);
        break;
    }

  } while (0);

  APPLOGI("Start test filter %s\n", ret == SUCCESS ?"Success" : "Failed");
  return SUCCESS;
}



static EMON_COMMAND_LIST cmd_dmx[] = 
{
  {_start_filter, "ftstart", "dmx.startf <pid [verion] [section_num]> start filter by pid. if verion/section=-1,then ignore that",FALSE},
  {_stop_filter, "ftstop", "dmx.stopf <pid> stop filter by index",FALSE},
  {_list_filter, "ftlist", "dmx.listf <pid> list filter index",FALSE},
  {_start_stop_filter_repeatly, "ftloop", "ftloop, start/stop filter by multi times",FALSE},
  {_ca_filter_test, "fttest", "fttest 1 start test, fttest 0 stop test",FALSE},
  {0, 0, 0, 0}
};

int cmd_dmx_init(void)
{
  return EMON_Register_Test_Functions(cmd_dmx, "dmx");
}

