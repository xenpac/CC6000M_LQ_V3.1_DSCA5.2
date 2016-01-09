/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IMEM_ALLOC_H_
#define __IMEM_ALLOC_H_

/*!
  imem_alloc
  */
#define IMEM_ALLOC_INTERFACE "imem_alloc"

/*!
  private data length
  */
#define IMEM_ALLOC_PRIVAT_DATA (64)

/*!
  the especial extant buffers
  */
typedef struct tag_extant_buffer
{
  /*!
    extend buffer. 
    */
  u8 *p_buffer;
  /*!
    can used size. 
    */
  u32 buffer_len;
}extant_buffer_t;

/*!
  allocator propertie
  */
typedef struct tag_allocator_properties
{
  /*!
    Number of buffers created by the allocator. default to 0
    */
  u32 buffers;
  /*!
    Size of each buffer in bytes, excluding any prefix. default to 0
    */
  u32 buffer_size;
  /*!
    Alignment of the buffer; buffer start will be aligned
    on a multiple of this value. default to 0
    */
  u32 align_size;
  /*!
    sometimes, the user need manage the buffer space by himself. so the sample
    is a loader just. it's a special application
    (for example: dmx filter, the buffer come form demux device).
    default to FALSE
    */
  BOOL use_virtual_space;
  /*!
    this is especial buffer. in some case, we will use the appointed memory, so 
    you can set the properties using it. the sample's buffer will point 
    the buffer list. default to NULL
    */
  extant_buffer_t extant_buffer;
}allocator_properties_t;

/*!
  memory allocator define
  */
typedef struct tag_imem_allocator
{
  /*!
    private data buffer
    */
  u8 private_data[IMEM_ALLOC_PRIVAT_DATA];

  /*!
    The SetProperties method specifies the number of 
    buffers to allocate and the size of each buffer.
    */
  RET_CODE (*set_properties)(handle_t _this,
      allocator_properties_t *p_request, allocator_properties_t *p_actual);
  /*!
    The GetProperties method retrieves the number of 
    buffers that the allocator will create, and the buffer properties.
    */
  RET_CODE (*get_properties)(handle_t _this, allocator_properties_t *p_props);

  /*!
    The Commit method allocates the buffer memory.
    */
  PRIVATE RET_CODE (*commit)(handle_t _this);

  /*!
    The Decommit method releases the buffer memory.
    */
  PRIVATE RET_CODE (*decommit)(handle_t _this);

  /*!
    Retrieves a media sample that contains an empty buffer.
    */
  PRIVATE RET_CODE (*get_sample)(handle_t _this, interface_t **pp_buffer_sample);
  
}imem_allocator_t;

#endif // End for __IMEM_ALLOC_H_

