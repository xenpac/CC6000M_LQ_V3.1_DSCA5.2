/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CTRL_STRING_H__
#define __CTRL_STRING_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   \file ctrl_string.h

   This file defined some interfaces of string operation,
   it's mainly about memory allocate and release.
  */

/*!
   Allocate memory for unicode string.

   \param[in] len : memory size to allocate.
   \return : return the start address of the memory just allocated if success,
    else return NULL.
  */
void *ctrl_unistr_alloc(u32 len);

/*!
   Reallocate memory for unicode string.

   \param[in] p_str : unicode string.
   \param[in] len : memory size to reallocate.
   \return : return the start address of the memory just reallocated if success,
   else return NULL.
  */
void *ctrl_unistr_realloc(void *p_str, u32 len);

/*!
   Free the memory of a unicode string.

   \param[in] p_str : unicode string.
   \return : NULL.
  */
void ctrl_unistr_free(void *p_str);

/*!
   Allocate memory for ascii string.

   \param[in] len : memory size to allocate.
   \return : return the start address of the memory just allocated if success,
   else return NULL.
  */
void *ctrl_ascstr_alloc(u32 len);

/*!
   Free the memory of a ascii string.

   \param[in] p_str : ascii string.
   \return : NULL.
  */
void ctrl_ascstr_free(void *p_str);

/*!
   Reallocate memory for ascii string.

   \param[in] p_str : ascii string.
   \param[in] len : memory size to reallocate.
   \return : return the start address of the memory just reallocated if success,
   else return NULL.
  */
void *ctrl_asc_str_realloc(void *p_str, u32 len);

/*!
   String initial, allocate memory from memory heap.

   \param[in] buf_size : buffer size.
   \return : NULL.
  */
void ctrl_str_init(u32 buf_size);

/*!
   Release memory of a string.

   \return : NULL
  */
void ctrl_str_release(void);

#ifdef __cplusplus
}
#endif

#endif
