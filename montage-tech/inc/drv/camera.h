/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __CAMERA_H__
#define __CAMERA_H__

/*!
    IO control command
  */
typedef enum uvc_ioctrl_cmd 
{
    /*!
      start usb camera capture
      */
    UVC_START_CAPTURE,
    /*!
      stop usb camera capture
      */
    UVC_STOP_CAPTURE, 
    /*!
    get usb camera stream data
    */
    UVC_GET_DATA_STREAM,

}uvc_ioctrl_cmd_t;

/*!
  uvc datastream struct
  */
typedef struct uvc_data_stream_info
{
  /*!
    stream data buffer pointer
    */ 
  u8 *p_buffer;
      /*!
   expected length
    */ 
  int  expected_length;   
            /*!
   actual_length
    */
  int  actual_length; 
}uvc_data_stream_info_t; 
/*!
    config image quality
  */
typedef enum uvc_img_quality 
{
    /*!
      config image high quality 
      */
    IMG_QUALITY_HIGH,
    /*!
      config image medium quality 
      */
    IMG_QUALITY_MEDIUM, 
    /*!
    config image low quality 
    */
    IMG_QUALITY_LOW,

}uvc_img_quality_t;

/*!
  uvc_device_config
  */
typedef struct uvc_device_config
{
  /*!
    image quality
    */ 
  uvc_img_quality_t ImgQuality;
      /*!
   FrameRate
    */ 
  int  FrameRate;   
}uvc_device_config_t; 
/*!
  CAMERA START CAPTURE

  \param[in] dev  device NAME
 */
int camera_open(char *p_name);

/*!
  start camera capture

  \param[in] fd
  \param[in] buf  point to read buff
  \param[in] expected read length 
 */
int camera_read(int fd, char *p_buffer, int buffer_len);
/*!
  CAMERA STOP CAPTURE

  \param[in] dev  fd
 */
int camera_close(int fd);

/*!
  IP CAMERA DETECT

  \param[name] detected ip camera name
 */
int ip_camera_detect(char name[32]);
#endif //__CAMERA_H__

