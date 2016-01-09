/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SRC_PIN_INTRA_H_
#define __FILE_SRC_PIN_INTRA_H_

/*!
  buffer read
  */
typedef u8 *(*buffer_get)(u32 *p_len);

/*!
  fsrc_pin_private
  */
typedef struct tag_fsrc_pin_private
{
  /*!
    this point !!
    */
  void *p_this;
  /*!
    file name
    */
  u16 file_name[MAX_FILE_PATH];
  /*!
    file handle
    */
  hfile_t file;
  /*!
    file handle
    */
  u64 file_size;
  /*!
    attr
    */
  src_pin_attr_t attr;
  /*!
    buffer read
    */
  buffer_get buf_get;
  /*!
    mem alloc interface
    */
  imem_allocator_t *p_alloc;
  /*!
  read insufficient
  */
  u32 read_insufficient;
}fsrc_pin_private_t;

/*!
  the file source pin define
  */
typedef struct tag_fsrc_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER source_pin_t m_pin;
  /*!
    private data
    */
  fsrc_pin_private_t private_data;

  /*!
    set a file_name
    */
  void (*set_file_name)(handle_t _this, u16 *p_file_name);
  /*!
    set read from buffer
    */
  void (*set_buffer_read)(handle_t _this, void *p_read_data);
  /*!
    config source pin attribute
    */
  void (*config)(handle_t _this, void *p_attr);
}fsrc_pin_t;

/*!
  create a pin

  \return return the instance of fsrc_pin_t
  */
fsrc_pin_t * fsrc_pin_create(fsrc_pin_t *p_fsrc_pin, interface_t *p_owner);

#endif // End for __FILE_SRC_PIN_INTRA_H_

