/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "class_factory.h"

/*!
  Item definition in class factory
  */
typedef struct
{ 
  /*!
    Item process function sets and the 
    */
  class_handle_t p_item_handle;
}factory_item_t;

/*!
  Class factory items
  */
static factory_item_t g_fact_item[CLASS_ID_CNT] = {{NULL}};

void class_register(u8 class_id, class_handle_t p_class_handle)
{
  if(class_id >= CLASS_ID_CNT)
  {
    return;
  }
  
  g_fact_item[class_id].p_item_handle = p_class_handle;
}

class_handle_t class_get_handle_by_id(u8 class_id)
{
  if(class_id >= CLASS_ID_CNT)
  {
    return NULL;
  }

  return g_fact_item[class_id].p_item_handle;
}



