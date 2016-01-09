/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"

//util
#include "class_factory.h"
#include "simple_queue.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "ifilter.h"

#include "eva_filter_factory.h"

#include "ap_framework.h"
#include "err_check_def.h"

/*!
  array depth
  */
#define USAGE_FILTER_NUM (100)

ifilter_t * demux_filter_create(void);
/*!
  filter factory
  */
typedef struct tag_filter_factory
{
  /*!
    this filter's id
    */
  filter_id_t id;
  /*!
    this filter's create function
    */
  filter_create_f f;
}filter_factory_t;


static filter_factory_t *p_filter_list = NULL;

RET_CODE eva_add_filter_by_id(filter_id_t id, ifilter_t **pp_filter, void *p_para)
{
  u32 i = 0;

  if(id <= UNKNOWN_FILTER_ID)
  {
    OS_PRINTF("input id is error, id = 0x%x.\n", id);
    return ERR_FAILURE;
  }

  for(i = 0; i < USAGE_FILTER_NUM; i++)
  {
    if(p_filter_list[i].id == id)
    {
      *pp_filter = p_filter_list[i].f(p_para);
      return SUCCESS;
    }

    if(p_filter_list[i].id == 0)  //find end
    {
      break;
    }
  }

  OS_PRINTF("can't found the filter, please make sure you regsiter it.\n");
  return ERR_FAILURE;
}

RET_CODE eva_register_filter(filter_id_t id, filter_create_f f)
{
  u32 i = 0;
  
  if(id <= UNKNOWN_FILTER_ID)
  {
    OS_PRINTF("input id is error, id = 0x%x.\n", id);
    return ERR_FAILURE;
  }

  if(NULL == f)
  {
    OS_PRINTF("input filter_create_function is NULL.\n");
    return ERR_FAILURE;
  }

  for(i = 0; i < USAGE_FILTER_NUM; i++)
  {
    if(p_filter_list[i].id == id)
    {
      OS_PRINTF("the filter was registered, id = 0x%x.\n", id);
      return ERR_FAILURE;
    }

    if(p_filter_list[i].id == 0)  //find end, add the new
    {
      p_filter_list[i].id = id;
      p_filter_list[i].f = f;
      return SUCCESS;
    }
  }

  OS_PRINTF("the array is full.\n");
  return ERR_FAILURE;
}

RET_CODE eva_filter_factory_init(void)
{
  CHECK_FAIL_RET_CODE((filter_id_t)APP_LAST < UNKNOWN_FILTER_ID);

  p_filter_list = (filter_factory_t *)mtos_malloc(
                        sizeof(filter_factory_t) * USAGE_FILTER_NUM);
  CHECK_FAIL_RET_CODE(p_filter_list != NULL);
  memset(p_filter_list, 0, sizeof(filter_factory_t) * USAGE_FILTER_NUM);
  return SUCCESS;
}

RET_CODE eva_filter_factory_deinit(void)
{
  if(p_filter_list != NULL)
  {
    mtos_free(p_filter_list);
  }
  return SUCCESS;
}


