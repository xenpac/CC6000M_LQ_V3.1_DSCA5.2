/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __HID_H__
#define __HID_H__
 /*!
    key name
    */
#define KEYMASK_RIGHT      (1 << 0) 
/*!
    key name
    */
#define KEYMASK_LEFT    (1 << 1) 
/*!
    key name
    */
#define KEYMASK_UP    (1 << 2)  
/*!
    key name
    */
#define KEYMASK_DOWN   (1 << 3) 
/*!
    key name
    */
#define KEYMASK_Y      (1 << 4)  
/*!
    key name
    */
#define KEYMASK_B      (1 << 5) 
/*!
    key name
    */
#define KEYMASK_A      (1 << 6)
/*!
    key name
    */
#define KEYMASK_X      (1 << 7) 
/*!
    key name
    */
#define KEYMASK_L2       (1 << 8)  
/*!
    key name
    */
#define KEYMASK_R2       (1 << 9) 
/*!
    key name
    */
#define KEYMASK_L1       (1 << 10)  
/*!
    key name
    */
#define KEYMASK_R1       (1 << 11) 
/*!
    key name
    */
#define KEYMASK_SELECT  (1 << 12) 
/*!
    key name
    */
#define KEYMASK_START   (1 << 13)        
/*!
  HID device type 
  */
typedef enum
{
/*!
  usb joystick
  */
  HID_JOYSTICK = (0x55AA << 16) | (1 << 1),
  /*!
  usb SCANNER
  */
  HID_SCANNER = (0x55AA << 16) | (1 << 2),
 /*!
  max value 
  */
  HID_MAX,
}hid_device_type;

#if 1

/*!
 joystick cofnig; do not use now
  */
typedef struct
{
/*!
  reservd cfg 
  */
 void (*hid_callback)(void *data,u8 len);

}hid_cfg_t;

#endif


#if 0
/*!
 joystick cofnig; do not use now
  */
typedef struct
{
/*!
  reservd cfg 
  */
  u32 data;

}joystick_cfg_t;



/*!
  HID device cfg 
  */
typedef struct
{
 /*!
  joystick cfg 
  */
  joystick_cfg_t *p_joystick_cfg;
  
}hid_cfg_t;

#endif
/*!
  attach 
  */
typedef struct
{
  /*!
  	base
    */
  void *p_base;
  /*!
  	priv
    */
  void *p_priv;
} hid_device_t;
/*!
  \malloc
  \param[in] sz: size
  return Return malloc address
  */
u8 * _hid_malloc(u32 sz);
/*!
  \malloc
  \param[in] pbuf: adress
  return void
  */
void _hid_mfree(u8 *pbuf);

/*!
  \clearn HID device FIFO
  \param[in] p_dev: hid device hanle
  \param[in] hidtype: hid device type 
  \param[in] special device id (0,1... max)
  */
void hid_clear_datas(hid_device_t *p_dev, hid_device_type hidtype, u8 devid_id);
/*!
 \ get datas from HID device FIFO
  \param[in] p_dev: hid device hanle
  \param[in] hidtype: hid device type 
 \ param[in] special device id (0,1... max)
 \ param[out] p_code: array for get datas(u16)
 return Return SUCCESS for get datas.
        Return other none data gets.
  */
RET_CODE hid_get_datas(hid_device_t *p_dev, hid_device_type hidtype, u8 devid_id, u16 *p_code);
/*!
  \get HID device data number of unit
  \param[in] p_dev: hid device hanle
 \param[in] hidtype: hid device type 
 \return Return 0-255.
  */
u32 hid_unit_num(hid_device_t *p_dev, hid_device_type hidtype);
/*!
  \hid device attach
  \param[in] p_name: attach name
  \return Return SUCCESS for attach ok.
        Return other for error.
  */
void *hcd_buffer_dma_alloc(
  u32       size,
  u32    *dma
);
/*!
  \hcd_buffer_dma_free
        Return void
  */
void hcd_buffer_dma_free(
  u32       size,
  void      *addr,
  u32    dma
);  
/*!
  \joystick_attach
   \param[in] p_name: attach name
  \return Return SUCCESS for attach ok.
        Return other for error.
  */
RET_CODE joystick_attach(char *p_name);
/*!
  \usb_hid_scannergun_attach
  \param[in] p_name: attach name
  \return Return SUCCESS for attach ok.
        Return other for error.
  */
RET_CODE usb_hid_scannergun_attach(char *p_name);

#endif

