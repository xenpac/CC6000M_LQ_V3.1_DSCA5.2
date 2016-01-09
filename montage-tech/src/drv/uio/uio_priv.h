/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UIO_PRIV_H_
#define __UIO_PRIV_H_

/*!
  UIO configuration
  */
typedef struct lld_uio
{
  /*!
    pointer to private info 
    */
  void *p_priv;
  /*!
    flag if IrDA is enabled
    */
  s32 irda_en;
  /*!
    pointer to IrDA private info 
    */
  void *p_irda;
  /*!
    flag if panel is enabled
    */
  s32 fp_en;
  /*!
    pointer to Frontpanel private info 
    */
  void *p_fp;

  /*!
    function pointer to uio_get_code implementation
    */
  RET_CODE (*get_code)(struct lld_uio *p_dev, u16 *p_code);
  /*!
    function pointer to uio_display implementation
    */
  RET_CODE (*display)(struct lld_uio *p_dev, u8 *p_data, u32 len);
  /*!
    function pointer to uio_set_rpt_key implementation
    */
  RET_CODE (*set_rpt_key)(struct lld_uio *p_dev, uio_type_t uio,u8 *p_keycodes,
                          u32 len, u8 ir_index);
  /*!
    function pointer to uio_clr_rpt_key implementation
    */
  void (*clr_rpt_key)(struct lld_uio *p_dev, uio_type_t uio);
  /*!
    function pointer to uio_clear_key implementation
    */
  void (*clear_key)(struct lld_uio *p_dev);
  /*!
    function pointer to uio_set_sw_usrcode implementation
    */
  RET_CODE (*set_sw_usrcode)(struct lld_uio *p_dev, u8 num, u16 *p_array);
  /*!
    function pointer to irda open implementation
    */
  RET_CODE (*irda_open)(struct lld_uio *p_lld, irda_cfg_t *p_cfg);
  /*!
    function pointer to irda stop implementation
    */
  RET_CODE (*irda_stop)(struct lld_uio *p_lld);
  /*!
    function pointer to irda io control command
    */
  RET_CODE (*irda_io_ctrl)(struct lld_uio *p_lld,u32 cmd, u32 param);
  /*!
    function pointer to panel open implementation
    */
  RET_CODE (*fp_open)(struct lld_uio *p_lld, fp_cfg_t *p_cfg);
  /*!
    function pointer to panel stop implementation
    */  
  RET_CODE (*fp_stop)(struct lld_uio *p_lld);
  /*!
    function pointer to panel io control command
    */
  RET_CODE (*fp_io_ctrl)(struct lld_uio *p_lld,u32 cmd, u32 param);
   /*!
    function pointer to panel start fp-timer implementation
    */
  void (*fp_start_work)(void);
  /*!
    function pointer to panel stop fp-timer implementation
    */
  void (*fp_stop_work)(void);
  /*!
    function pointer to set bitmap
    */
  void (*fp_set_bitmap)(struct lld_uio *p_lld,led_bitmap_t *p_map, u32 size);
}lld_uio_t;

/*!
  UIO private information
  */
typedef struct
{
  /*!
    UIO key fifo
    */
  os_fifo_t fifo;
  /*!
    UIO buffer to hold key
    */
  u16 key_buf[UIO_KEY_FIFO_SIZE];
  /*!
    registered UIO repeat key
    */
  u8 rpt_key[UIO_IRDA_MAX_PRTKEY * IRDA_MAX_USER + UIO_FP_MAX_PRTKEY];
}uio_priv_t;


#endif //__UIO_PRIV_H_
