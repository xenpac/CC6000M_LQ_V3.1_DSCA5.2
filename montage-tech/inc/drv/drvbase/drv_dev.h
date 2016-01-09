/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DRV_DEV_H__
#define __DRV_DEV_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
    Common device handle
  */
typedef struct drv_dev
{
  /*!
    The device base control part
    */
  void *p_base;
  /*!
    The device private control part
    */
  void *p_priv;
} drv_dev_t;


/*!
  Definition for overall media type used for stream device
  */
typedef enum
{
  /*!
      NULL media type
    */
  MEDIA_NULL,
  /*!
      Video un-compressed
    */
  MEDIA_UNCOMPRESSED_VIDEO,
  /*!
      Audio PCM
    */
  MEDIA_PCM,
  /*!
      Video MPEG2 es
    */
  MEDIA_MPEG2_VIDEO,
  /*!
      Audio MPEG2 es
    */
  MEDIA_MPEG2_AUDIO,
  /*!
      Video H.264 es
    */
  MEDIA_H264_VIDEO,
  /*!
      Video AVS es
    */
  MEDIA_AVS_VIDEO,
  /*!
      MPEG-1 system stream
    */
  MEDIA_MPEG1_SYSTEM,
  /*!
      MPEG-2 program stream
    */
  MEDIA_MPEG2_PROGRAM,
  /*!
      MPEG-2 transport stream
    */
  MEDIA_MPEG2_TRANSPORT,
  /*!
      MPEG-2 packaged element stream
    */
  MEDIA_MPEG2_PES,
  /*!
      Teletext
    */
  MEDIA_TEXT,
  /*!
      TCP/IP
    */
  MEDIA_TCPIP,
  /*!
      gif file
    */
  MEDIA_GIF,
  /*!
      jpeg file
    */
  MEDIA_JPG,
}dev_media_type_t;

/*!
  This function used to tell the media code mapped to feature code byte index
  */
#define MEDIA_TO_FEATURE_MASK(media)  (1 << ((media - 1) & 0x1f))

/*!
  This function used to tell the media code mapped to feature code byte index
  */
#define MEDIA_TO_FEATURE_IDX(media)   (((media - 1)>> 5) & 1)

/*!
  The max number of media type
  */
#define MEDIA_TYPE_NUM    64

/*!
  Device media capability (16B)
  Even TYPE_MEDIA use 8bit coding, we use bit mask for MEDIA feature storage for save memory.
  The relationship between TYPE_MEDIA and device feature media type bit mask is:
  media_i[media_type >> 5] |= (1 << (media_type & 0x1f));
  Please reference MEDIA_TO_FEATURE_MASK()/MEDIA_TO_FEATURE_IDX()/FEATURE_MASK_TO_MEDIA() for
  more detail information.
  At current device support upto 64 media types.
 */
typedef struct
{
  /*!
      Device input media type bit mask
    */
  u32 media_i[MEDIA_TYPE_NUM >> 5];
  /*!
      Device output media type bit mask
    */
  u32 media_o[MEDIA_TYPE_NUM >> 5];
}dev_media_cap_t;

/*!
  Declaration for meml_buf
  */
struct meml_buf;


/*!
  Media data push callback function type between media devices.

  \param[in] p_sdev The device handle which register this callback
  \param[in] ipin_id The input pin id registered by this device
  \param[in] p_lbuf The buffer link list which carry the stream media data.
  */
typedef RET_CODE (*dev_string_push_cb_t)(void *p_dev, u8 ipin_id, struct meml_buf *p_lbuf);

/*!
 Device bridge structure, [source_dev]----->[destin_dev]
 */
typedef struct
{
  /*!
      Source device handle
    */
  void *p_sdev;
  /*!
      Destin device handle
    */
  void *p_ddev;
  /*!
      Transfer media type, see dev_media_type_t
    */
  u8 media;
  /*!
      Source device stream meida output pin id allocated(output)
    */
  u8 sdev_opin_id : 4;
  /*!
      Destin device stream meida input pin id allocated(output)
    */
  u8 ddev_ipin_id : 4;
}dev_stream_bridge_t;

/*!
  Device link item structure, this structure descript one of media data output.
  */
typedef struct
{
  /*!
      Destin device handle
    */
  void *p_ddev;
  /*!
      Destin device push data callback
    */
  dev_string_push_cb_t push_cb;
  /*!
      Output meida type supported by this output pin
    */
  u8 media;
  /*!
      Destin device input pin id binded to this output pin
    */
  u8 ipin_id : 4;
}dev_output_pin_t;


/*!
    Field mask of device state
  */
#define DEV_FLAGS_MASK_STAT (0x0F)

/*!
    Field mask of device reference counter
  */
#define DEV_FLAGS_MASK_REFC (0xF0)

/*!
    Device standard status
  */
typedef enum dev_state
{
    /*!
        Device is in working status, including IDLE, PAUSE and RUNNING
      */
    DEV_STATE_MASK_WORK = 0x02,
    /*!
        Device is attached
      */
    DEV_STATE_ATTACHED = 0x01,
    /*!
        Device is in idle status.
      */
    DEV_STATE_IDLE = 0x07,
    /*!
        Device is in running status.
      */
    DEV_STATE_RUNNING = 0x0B,
    /*!
        Device is in pause status.
      */
    DEV_STATE_PAUSE = 0x0F
} dev_state_t;

/*!
    Max device handle number.
    As we use a pre-allocated array for fast search of the device handle, the device handle number should be limited in the system.
  */
#define DEV_MAX_NUMBER (32)

/*!
    Device identification modes
  */
typedef enum dev_idt
{
    /*!
        Identify device by its type + index
      */
    DEV_IDT_ID  = 1,
    /*!
        Identify device by its name
      */
    DEV_IDT_NAME,
    /*!
        Identify device by its type
      */
    DEV_IDT_TYPE
} dev_idt_t;

/*!
    Device i/o control type
  */
typedef enum dev_ioctrl_type
{
  /*!
    The i/o control command type that does not need semaphone protection
    */
  DEV_IOCTRL_TYPE_UNLOCK = 0x80000000,
  /*!
    The i/o control command type that needs semaphone protection
    */
  DEV_IOCTRL_TYPE_LOCK = 0x40000000
} dev_ioctrl_type_t;

/*!
    General device ioctrl commands used for all types of device drivers.
    The commands with bit15 equal to '1' are for all types of device drivers, and are defined in this module.
    The commands with bit15 equal to '0' are for customized device drivers, and are defined in other driver's header files.
    We use only 16bits for the purposes of performance and being compatible with 16-bit instructions.
  */
typedef enum
{
  /*!
      Commands for power management
    */
  DEV_IOCTRL_POWER = DEV_IOCTRL_TYPE_LOCK + 0x8001,
  /*!
      Command for notify device a new input pin inserted
    */
  DEV_IOCTRL_NOTIFY_IPIN_INSERT,
  /*!
      Command for notify device a new output pin inserted
    */
  DEV_IOCTRL_NOTIFY_OPIN_INSERT,
  /*!
      Command for notify device an old input pin removed
    */
  DEV_IOCTRL_NOTIFY_IPIN_REMOVE,
  /*!
      Command for notify device an old output pin removed
    */
  DEV_IOCTRL_NOTIFY_OPIN_REMOVE,
  /*!
      Command for set the print level to a driver
    */
  DEV_IOCTRL_SET_PRINT_LEVEL,
}dev_ioctrl_cmd_t;

/*!
    Device power status (used as a parameter for the IO command DEV_IOCTRL_POWER)
  */
typedef enum dev_power_type
{
    /*!
        Device power down (no context backup)
      */
    DEV_POWER_DOWN = 0,
    /*!
        Device sleep (context backup and power down)
      */
    DEV_POWER_SLEEP,
    /*!
        Device standby (stop & keep context)
      */
    DEV_POWER_STANDBY,
    /*!
        Device low speed (running & low performance)
      */
    DEV_POWER_LOWSPEED,
    /*!
        Device full speed (full performance)
      */
    DEV_POWER_FULLSPEED
} dev_power_type_t;

/*!
    Locking mode of device driver's function call
  */
typedef enum dev_lock_mode
{
   /*!
        os lock default use the driver default lock
      */
      
    OS_LOCK_DEF = 0,
   /*!
        os task locking
      */
    OS_TASK_LOCK = 1,
    
   /*!
        os mutex locking
      */
    OS_MUTEX_LOCK,

    /*!
        os sem locking
      */
    OS_SEM_LOCK,

    /*!
        disable interrupt locking
      */
    OS_INTERRUPT_LOCK,
    /*!
        max number of mode
      */
    MAX_NUM_LOCK_MODE,
}dev_lock_mode_t;


/*!
   Find a device handle from the handle table by identifier.
   We support several search methods (identify by iden_type). Those methods include:
   iden_type            identifier                                         Comments
   DEV_IDT_ID     Device id (bit[31:16] device type + bit[15:8]         Find device by id.
   DEV_IDT_NAME   Device name string.                                   Find device by name.
   DEV_IDT_TYPE   Device type (bit[15:0] device type).                  Find device by type.

   \param[in] p_sdev The start device handle. If doing search from the start handle, use NULL. It is of no use when iden_type is equal to DEV_IDT_ID.
   \param[in] ident_type Device identifier type.
   \param[in] ident Device identifier.

   \return  Return non-NULL for the device handle if success and NULL if failure.
  */
void *dev_find_identifier(void *p_sdev, int ident_type, u32 ident);

/*!
  Allocate a device handle and insert this handle into the system device driver handle list.
  This function is used for driver. Device manager will copy "name" and "type" into the newly allocated device handle structure.

  \param[in] p_name The device's handle name.
  \param[in] type The device type. See "sys_define.h" for the definition.
  \param[in] dev_size The size of common device handle structure. (The size is equel to drv_dev_t)
  \param[in] priv_size The size of lld driver's structure. See xxx_priv.h

  \return Return non-NULL for the device handle if success and NULL if failure.
  */
void *dev_allocate(const char *p_name, u16 type, u32 dev_size, u32 priv_size);

/*!
  Free an allocated device handle and remove it from the system device driver handle list.

  \param[in] p_dev The device handle to be released.

  \return void
  */
void dev_free(void *p_dev);

/*!
  Set device handle's state.

  \param[in] p_dev The corresponding device handle.
  \param[in] state Device state, see dev_state_t

  \return void
  */
void dev_set_state(void *p_dev, u8 state);

/*!
  Get device handle's state.

  \param[in] p_dev The corresponding device handle.

  \return Return non-NULL for the device status if success, or NULL if failure.
  */
u8 dev_get_state(void *p_dev);

/*!
  Common open driver interface.

  \param[in] p_dev The device handle.
  \param[in] param The device driver's open function parameter. It is configuration information for the driver.

  \return Return 0 for success and others for failure.
  */
RET_CODE dev_open(void *p_dev, void *p_param);

/*!
  Driver's common close interface.

  \param[in] p_dev This device's handle.

  \return Return 0 for success and others for failure.
  */
RET_CODE dev_close(void *p_dev);

/*!
  Common IO control interface.

  The device manager defines two types of common IO commands for all types of device drivers:
  IO Command                  Param
  ---------------------  -----------------------------
  DEV_IOCTRL_POWER       DEV_POWER_FULLSPEED
                         DEV_POWER_LOWSPEED
                         DEV_POWER_STANDBY
                         DEV_POWER_SLEEP
                         DEV_POWER_DOWN

  Other IO commands are defined in specific device driver header files.

  \param[in] p_dev The device handle.
  \param[in] cmd The IO command.
  \param[in] param The IO parameter.

  \return Return 0 for success and others for failure.
  */
RET_CODE dev_io_ctrl(void *p_dev, u32 cmd, u32 param);

/*!
  Common device detach interface.

  \param[in] p_dev This device's handle.

  \return void
  */
void dev_detach(void *p_dev);

/*!
  Check if this device is in working status.

  \param[in] p_dev The device handle.

  \return Return 0 if the device is in working status or others if not in working status.
  */
RET_CODE drv_check_cond(void *p_dev);

/*!
  Enable and configurate device stream media capability.

  \param[in] p_dev The device handle to be enabled
  \param[in] p_media_cap The media capability this device to support

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_media_enable(void *p_dev, dev_media_cap_t *p_media_cap);

/*!
  Link a specific device as source device in the stream chain.

  \param[in] p_dev The device handle to be linked as source device.
  \param[in] media Media type, see dev_media_type_t
  \param[out] p_push_cb The device's push callback binded to this input pin.
  \param[out] p_ipin_id The device's input pin id linked.

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_src_link(void *p_dev, u32 media,
  dev_string_push_cb_t *p_push_cb, u8 *p_ipin_id);

/*!
  Unlink a specific device which is the source device in the stream chain.

  \param[in] p_dev The device handle to be unlinked from the chain.
  \param[in] ipin_id The input pin id to be unlinked

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_src_unlink(void *p_dev, u8 ipin_id);

/*!
  Link a specific device as destin device in the stream chain.

  \param[in] p_dev The device handle to be link as destion device.
  \param[in] media Media type, see dev_media_type_t
  \param[in] p_push_cb The push callback which to be called
                                  when the data output from this destin device.
  \param[out] p_opin_id The output pin id linked to this destin device.

  \Return The operatin success or not.
  */
extern RET_CODE dev_stream_dst_link(void *p_dev, u32 media,
  dev_string_push_cb_t push_cb, u8 *p_opin_id);

/*!
  Unlink a specific device which is the destin device in the stream chain.

  \param[in] p_dev The device handle to be unlinked from the chain.
  \param[in] opin_id The output pin id to be unlinked

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_dst_unlink(void *p_dev, u8 opin_id);

/*!
  Append a new bridge into a device chain.

  \param[in] p_bridge The device bridge to be appended

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_bridge_append(dev_stream_bridge_t *p_bridge);

/*!
  Remove a new bridge from a device chain.

  \param[in] p_bridge The device bridge to be removed

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_bridge_remove(dev_stream_bridge_t *p_bridge);

/*!
  Create a new device chain.

  \param[in] p_chain_header The device chain to be created
  \param[in] len The device bridge number in this chain

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_chain_create(dev_stream_bridge_t *p_chain_header, u16 len);

/*!
  Destroy a new device chain.

  \param[in] p_chain_header The device chain to be destroyed
  \param[in] len The device bridge number in this chain

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_chain_destroy(dev_stream_bridge_t *p_chain_header, u16 len);

/*!
  Get a specific output pin information in a specific device.

  \param[in] p_dev The device handle
  \param[in] opin_id The output pin id.
  \param[out] p_opin_info The output pin information.

  \Return The operation success or not.
  */
extern RET_CODE dev_stream_opin_get(void *p_dev, u8 opin_id, dev_output_pin_t *p_opin_info);

/*!
  lock the  sem/mutex or task for a specific device refer to  device lock_mode
  
  \param[in] p_base The device_base_t  handle

  \Return void
  */
void drv_lock(void *p_base);

/*!
  unlock the  sem/mutex or task for a specific device refer to  device lock_mode
  
  \param[in] p_base The device_base_t  handle

  \Return void
  */
void  drv_unlock(void *p_base);
#ifdef __cplusplus
}
#endif

#endif //__DRV_DEV_H__

