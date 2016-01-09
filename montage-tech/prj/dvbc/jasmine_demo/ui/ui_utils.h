/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_UTILS_H_
#define __UI_UTILS_H_

typedef struct linklist
{
  void *p_priv_date;
  struct linklist *p_next;
}ui_link_list_t;

RET_CODE ui_link_list_create(ui_link_list_t **p_list,u16 size);
RET_CODE ui_link_list_insert(ui_link_list_t **p_list,u16 pos,void *p_item);
RET_CODE ui_link_list_get_item(ui_link_list_t **p_list,u16 pos,void **pp_item);
RET_CODE ui_link_list_delete(ui_link_list_t **p_list,u16 pos,void **pp_item);
int ui_link_list_set_item(ui_link_list_t **p_list,u16 pos,void *p_item);
int ui_link_list_find_item_pos(ui_link_list_t **p_list,u16 *pos,void* p_item);
int ui_link_list_get_size(ui_link_list_t **p_list);
int ui_link_list_destory(ui_link_list_t **p_list);
#endif
