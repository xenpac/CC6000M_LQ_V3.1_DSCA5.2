/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_utils.h"

 RET_CODE ui_link_list_create(ui_link_list_t **p_list,u16 size)  
 {  
   ui_link_list_t *p=NULL;  
   s16 i;  
   if(*p_list != NULL)
   {
     return ERR_FAILURE;
   }
   
  *p_list = (ui_link_list_t *)mtos_malloc(sizeof(ui_link_list_t));  
   if(*p_list==NULL)  
   {
     return ERR_FAILURE;
   }
   (*p_list)->p_next = NULL;  
   for(i=size;i>0;i--)  
   {  
     p = (ui_link_list_t *)mtos_malloc(sizeof(ui_link_list_t));  
     if(p==NULL)  
     {
      return ERR_FAILURE;  
     }
     p->p_priv_date = NULL;  
     p->p_next = (*p_list)->p_next;  
     (*p_list)->p_next = p;  
   }  
   return SUCCESS;  
}  

RET_CODE ui_link_list_insert(ui_link_list_t **p_list,u16 pos,void *p_item)  
{  
   ui_link_list_t *p,*q;  
   u16 i;  

   p = *p_list;  
   i = 0;  

   if(pos == 0)
   {
     return ERR_FAILURE;  
   }
   
   while(p!=NULL && i<pos-1)
   {  
     p = p->p_next;  
     i++;
   }  
   if(p==NULL || i > pos-1)  
   {
     return ERR_FAILURE;  
   }
   q = (ui_link_list_t*)mtos_malloc(sizeof(ui_link_list_t));
   if(q!=NULL)
   {  
     q->p_priv_date = p_item;  
     q->p_next = p->p_next;  
     p->p_next = q;  
     return SUCCESS;  
   }  
   else  
   {  
     return ERR_FAILURE;  
   }  
}  

  RET_CODE ui_link_list_get_item(ui_link_list_t **p_list,u16 pos,void **pp_item)  
 {  
    ui_link_list_t * p;  
    s16 i;    
  
    p = *p_list;  
    i = 0;  

    if(pos == 0)
    {
      return ERR_FAILURE;  
    }
    
    while(p!=NULL && i<pos)
    {  
      p = p->p_next;  
      i++;  
    }  
    if((p==NULL)||(i>pos))  
    {
     *pp_item = NULL;
      return ERR_FAILURE;  
    }  
    *pp_item = p->p_priv_date;  
     return SUCCESS;  
 }  

 RET_CODE ui_link_list_delete(ui_link_list_t **p_list,u16 pos,void **pp_item)  
 {  
    ui_link_list_t *p,*q;  
    s16 i;  
    p = *p_list;  
    i = 0;  
    while(p!=NULL && i<pos-1)
    {  
      p = p->p_next;  
      i++;
    }  
    if(p->p_next==NULL || i > pos-1)  
    {
     *pp_item = NULL;
      return ERR_FAILURE;  
    }
    q = p->p_next;  
    p->p_next = q->p_next;  
    if(pp_item != NULL)  
    {
     *pp_item = q->p_priv_date;  
    }
    mtos_free(q);  
    return SUCCESS;  
}  

 int ui_link_list_set_item(ui_link_list_t **p_list,u16 pos,void *p_item)  
 {  
   ui_link_list_t *p=NULL;  
   s16 i;  
   p = *p_list;  
   i = 0;  
   while(p!=NULL && i<pos)
   {  
     p = p->p_next;  
     i++;
   }  
   if(p==NULL || i > pos) 
   {
     return ERR_FAILURE;  
   }
   p->p_priv_date = p_item;  
   return SUCCESS;  
 }  

 int ui_link_list_find_item_pos(ui_link_list_t **p_list,u16 *pos,void* p_item)  
 {  
   ui_link_list_t * p;  
   s16 i;  
   p = *p_list;  
   i = 0;  
   while(p!=NULL)
   { 
     if(p->p_priv_date == p_item)  
     {  
      *pos = i;   
       return SUCCESS;  
     }  
     p = p->p_next;  
     i++;
   }  
   return ERR_FAILURE;    
 }  
 
 int ui_link_list_get_size(ui_link_list_t **p_list)  
 {  
    ui_link_list_t *p;  
    s16 i;  
 
    p = *p_list;  
    i = 0;  
    while(p!=NULL)  
    {  
      p = p->p_next;  
      i++;   
    }  
    return i;  
}  


 int ui_link_list_destory(ui_link_list_t **p_list)    
 {  
   ui_link_list_t *p,*q;  
   s16 i;  

   p = *p_list;  
   i = 0;  
   while(p!=NULL)  
   {  
     q = p->p_next;
     mtos_free(p);  
     p = q;  
   }  
   *p_list = NULL;
       
     return SUCCESS;  
 }  
 
