/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SRC_FILTER_H_
#define __FILE_SRC_FILTER_H_

/*!
file source filter source
*/
typedef enum tag_fsrc_filter_source
{
  /*!
	source from file
	*/
  SRC_FROM_FILE = 0,
  /*!
	source from buffer
	*/
  SRC_FROM_BUFFER,
  /*!
	source from block
	*/
  SRC_FROM_BLOCK
}fsrc_filter_source_t;

/*!
define source pin attribute
*/
typedef struct tag_src_pin_attr
{
  /*!
	is circular read, default to no circular
	*/
  BOOL is_circular;
  /*!
	is share read, default to engross
	*/
  BOOL is_share;
  /*!
	Number of buffers created. default to 1, ignore 0.
	*/
  u32 buffers;
  /*!
	Size of each buffer in bytes. default to KBYTE, ignore 0.
	*/
  u32 buffer_size;
  /*!
	if user need use extern buffer, set it 
	*/
  u8 *p_extern_buf;
  /*!
	if p_extern_buf not NULL, it's valid
	*/
  u32 extern_buf_size;
  /*!
	fsrc filter source from
	*/
  fsrc_filter_source_t src;
}src_pin_attr_t;

/*!
file source filter command define
*/
typedef enum tag_fsrc_filter_cmd
{
  /*!
	config read file' name
	*/
  FSRC_CFG_FILE_NAME,
  /*!
	config source buffer
	*/
  FSRC_CFG_SRC_BUFFER,
  /*!
	config source pin attribute //see src_pin_attr_t
	*/
  FSRC_CFG_SOURCE_PIN,

}fsrc_filter_cmd_t;

/*!
file source filter event define
*/
typedef enum tag_fsrc_filter_evt
{
  /*!
	config read file' name
	*/
  FILE_READ_END = FILE_SOURCE_FILTER << 16,
  /*!
	file source block read insufficient
	*/
  FSRC_MSG_BLOCK_READ_INSUFFICIENT,
}fsrc_filter_evt_t;

/*!
create a file source instance
filter ID:FILE_SOURCE_FILTER
\return return the instance of fsrc_filter
*/
ifilter_t * fsrc_filter_create(void *p_para);

#endif // End for __FILE_SRC_FILTER_H_

