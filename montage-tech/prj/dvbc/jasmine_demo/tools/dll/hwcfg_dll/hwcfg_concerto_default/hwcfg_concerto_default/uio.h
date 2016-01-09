/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#ifndef __UIO_H__
#define __UIO_H__
#ifdef __cplusplus
extern "C" {
#endif

  /*!
    GPIO pin information, 1 Byte
    */
  typedef struct
  {
    /*!
      Polarity of GPIO, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH active(light)
      */
    u8 polar:1;
    /*!
      GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
      */
    u8 io   :1;
    /*!
      GPIO direction: GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
      */
    u8 reserve   :6;	
    /*!
      GPIO index, upto 64 GPIO, 63 means invalid
      */
    u8 pos  :8;
  }pan_gpio_info_t;

  /*!
    GPIO pannel configuration
    */
  typedef struct        // Total 28 bytes
  {
    /*!
      refer to "enum pan_scan_type", start of offset 0
      */
    u8  type_scan:4;
    /*!
      0: Stand-alone LBD, 1: LBD in shifter.
      */
    u8  type_lbd :1;
    /*!
      0: Stand-alone COM, 1: COM in shifter.
      */
    u8  type_com: 1;
    /*!
      Number of scan PIN, MUST <= 2, 0: no scan;
      */
    u8  num_scan: 2;

    /*!
      Number of COM PIN, MUST <= 8, 0: no com; start of offset 1
      */
    u8  num_com : 4;
    /*!
      Position of colon flag, 0 to 7, 8 no colon
      */
    u8  pos_colon:4;

    /*!
 Attach MCU Type:
     FP type   bit1:FD620; bit2:PT6315 HL_886A; bit3:FD650;bit5: PT6311B HL_D336WA;
     bit6:PT6315 VFD16_0808FN
     bit 0: specify leds sequence, bit 4: 1, 3-line mcu, 0:2-line mcu;
Attache GPIO Type:
    bit0:1 CT1642 send data sequence, left -> right and 0 revert sequence, right -> left
    bit2:1:default com number(max number) ,0 specify com number
    bit4,5:use general gpio number (mux flatch,fclock, fdata...)
  */
    u8  status;

    /*!
      Shifter latch PIN, offset 3
      */
    pan_gpio_info_t flatch;
    /*!
      Shifter clock PIN
      */
    pan_gpio_info_t fclock;
    /*!
      Shifter data PIN
      */
    pan_gpio_info_t fdata;
    /*!
      Panel scan PIN
      */
    pan_gpio_info_t scan[2];
    /*!
      COM PIN, offset 8
      COM[4-7] is leds position, just for warriors now!!!
      */
    pan_gpio_info_t  com[8];
    /*!
      LBD PIN,IN sequence: power,lock ..., offset 16
      */
    pan_gpio_info_t  lbd[4];
    /*!
      COM scan interval in ms, offset 20
      */
    u32 com_scan_intv;
    /*!
      Repeat interval in ms, for rpt key only
      */
    u32 repeat_intv;
  }pan_hw_info_t;


  /*!
    LEB bitmap
    */
  typedef struct
  {
    /*!
      ascii character to display
      */
    u8 ch;
    /*!
      bitmap
      */
    u8 bitmap;
  }led_bitmap_t;


#ifdef __cplusplus
}
#endif
#endif //__UIO_H__
