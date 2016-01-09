/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FIFO_DB_H_
#define __FIFO_DB_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
  init fifo db

  \return 
  */
void fifo_db_init(void);

/*!
  Create fifo db

  \param[in] p_buf buffer addr
  \param[in] db_size database size

  \return 
  */
void fifo_db_create(u8 *p_buf, u32 db_size);

/*!
  des fifo db

  \return 
  */
void fifo_db_destroy(void);

/*!
  add data to fifo db

  \param[in] p_tag data tag
  \param[in] p_data data
  \param[in] data_size data size

  \return 
  */
void fifo_db_add(char *p_tag, u8 *p_data, u32 data_size);

/*!
  add data to fifo db

  \param[in] p_tag data tag
  \param[out] pp_data data

  \return data size, if it's zero means don't found
  */
u32 fifo_db_query(char *p_tag, u8 **pp_data);

#ifdef __cplusplus
}
#endif

#endif // End for __FIFO_DB_H_

