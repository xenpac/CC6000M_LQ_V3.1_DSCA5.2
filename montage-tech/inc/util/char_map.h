/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CHAR_MAP_H__
#define __CHAR_MAP_H__

/*!
   The structure is defined to describe the relation between key and value.
  */
typedef struct char_map
{
  /*!
     The key
    */
  u16 key;
  /*!
     The related value
    */
  u16 value;
}char_map_t;


/*!
   The structure is defined to describe the information of charmap.
  */
typedef struct
{
  /*!
     The char map
    */
  const char_map_t *p_table;
  /*!
     The length of map
    */
  u32 count;
}char_map_info_t;


/*!
   Standard procedure of binary search in the table.

   \param[in] key Specifies the key.
   \param[in] p_table Points to the table.
   \param[in] count	Specifies the size of table.

   \return If successful, the matched value, Otherwise return 0xFFFF.
  */
u16 map_lookup(u16 key, const char_map_t *p_table, s32 count);

/*!
   Reverse search in the table.

   \param[in] value	Specifies the value.
   \param[in] p_table Points to the table.
   \param[in] count	Specifies the size of table.

   \return If successful, the matched key, Otherwise return 0xFFFF.
  */
u16 reverse_lookup(u16 value, const char_map_t *p_table, s32 count);

#endif
