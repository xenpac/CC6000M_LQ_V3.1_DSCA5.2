/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FSINK_PIN_INTRA_H_
#define __FSINK_PIN_INTRA_H_

/*!
  max file size [ 4G ]
  */
#define MAX_FAT_FILE_SIZE ((4) * (1024) * (MBYTES))

/*!
  file sink pin private data
  */
typedef struct tag_file_sink_pin_private
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
    write size
    */
  u64 w_size;
  /*!
    flush ticks
    */
  u32 flush_ticks;
  /*!
    file counter
    */
  u32 file_counter;
  /*!
    is share mode
    */
  BOOL is_share;
  /*!
    sink mode
    */
  u32 mode;
  /*!
    ready size
    */
  u32 ready_size;
  /*!
    max size
    */
  u64 max_size;
  /*!
    media idx
    */
  media_idx_t *p_media_idx;
}fsink_pin_private_t;


/*!
  the input pin
  */
typedef struct tag_fsink_pin
{
  /*!
    public base class, must be the first member
    */
  FATHER sink_pin_t base_pin;
  /*!
    private data
    */
  fsink_pin_private_t private_data;
/*!
  set file_name
  */
void (*set_file_name)(handle_t _this, u16 *p_file_name, BOOL is_share);
/*!
  set sink mode
  */
void (*set_sink_mode)(handle_t _this, u8 sink_mode, media_idx_t *p_media_idx);
/*!
  set sink mode
  */
void (*set_sink_ready_size)(handle_t _this, u32 ready_size, u64 max_size);
/*!
  set sink mode
  */
void (*seek)(handle_t _this, s64 offset, u32 mode);
}fsink_pin_t;

/*!
  create a pin

  \return return the instance of usb_pin_t
  */
fsink_pin_t * fsink_pin_create(fsink_pin_t *p_pin, interface_t *p_owner);

#endif // End for __USB_PIN_INTRA_H_
