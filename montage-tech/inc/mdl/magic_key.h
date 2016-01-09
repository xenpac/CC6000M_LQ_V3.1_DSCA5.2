/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __MAGIC_KEY_H__
#define __MAGIC_KEY_H__

  /*!
     max magic list count
    */
#define MAX_MAGIC_LIST  (20)

/*!
  magic key type
  */
typedef enum
{
  /*!
    IR  key
    */
  IR_TYPE_MGC_KEY = 0,
  /*!
    Front pannel key
    */
  FP_TYPE_MGC_KEY,
  /*!
    Both FP key and IRDA input is valid  
    */
  ALL_TYPE_MGC_KEY,
}magic_key_type_t;

/*!
  magic key list parameter
  */
typedef struct
{
  /*!
    v_key value
    */
  u16 v_key;
  /*!
    v_key type  0:ir key 1: fp_key 2: all type support
    */
  magic_key_type_t key_type;
} magic_key_list_t;



/*!
  detect magic key
  \param[in] p_handle: magic key handle
  \param[in] key: v_key
  \param[in] key_type: current v_key type
  */
u16 detect_magic_keylist(void *p_handle,u16 key, magic_key_type_t key_type);

/*!
  register magic key list 
  \param[in] p_handle: magic key handle
  \param[in] p_key_list: a group of v_keys, see enum magic_key_list_t
  \param[in] key_cnt: count of a group v_keys
  \param[in] magic_key: which v_key you wanna return
  */
BOOL register_magic_keylist(void *p_handle,magic_key_list_t *p_key_list, u8 key_cnt, u16 magic_key);
/*!
  init magic keylist
  */
BOOL init_magic_keylist(void);

#endif
