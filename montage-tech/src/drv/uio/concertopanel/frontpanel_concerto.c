/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_magic.h"
#include "mtos_fifo.h"
#include "mtos_int.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "drv_dev.h"
#include "hal_concerto_irq.h"
#include "hal_timer.h"
#include "hal_base.h"
#include "hal_misc.h"
#include "uio.h"
#include "i2c.h"
#include "../uio_priv.h"

#define FREQ_KHZ 1000

#define LEDKB_HOST_REG_BASE 0xBF154000
#define LEDKB_CTRL      (LEDKB_HOST_REG_BASE)
#define LEDKB_TIMESET   (LEDKB_HOST_REG_BASE + 4)
#define LEDKB_TIMESEL   (LEDKB_HOST_REG_BASE + 8)
#define LEDKB_INTMASK_KEYFILT   (LEDKB_HOST_REG_BASE + 0x0C)
#define LEDKB_CLK_PARA   (LEDKB_HOST_REG_BASE + 0x10)
#define LEDKB_DEN_PARA   (LEDKB_HOST_REG_BASE + 0x14)
#define LEDKB_DAT_PARA(i)   (LEDKB_HOST_REG_BASE + 0x18 + (i) * 4)
#define LEDKB_DAT_DELAY_NUM (LEDKB_HOST_REG_BASE + 0x60)
#define LEDKB_KEY_VALUE0   (LEDKB_HOST_REG_BASE + 0x70)
#define LEDKB_KEY_VALUE1   (LEDKB_HOST_REG_BASE + 0x74)
#define LEDKB_KEY_PRESS0   (LEDKB_HOST_REG_BASE + 0x78)
#define LEDKB_KEY_PRESS1   (LEDKB_HOST_REG_BASE + 0x7C)
#define LEDKB_KEY_RELEASE0   (LEDKB_HOST_REG_BASE + 0x80)
#define LEDKB_KEY_RELEASE1   (LEDKB_HOST_REG_BASE + 0x84)

#define FP_DEBUG 0
#if FP_DEBUG
#define FP_PRINTF(...)  \
do \
{\
OS_PRINTF(__VA_ARGS__); \
} while(0)
#else
 #define FP_PRINTF DUMMY_PRINTF
#endif

/***************************************************************************
* Hardware related
****************************************************************************/
//set the command of system parameter
#define FD650_BIT_ENABLE  0x01 // open/close bit
#define FD650_BIT_SLEEP   0x04 // sleep control bit
#define FD650_BIT_7SEG    0x08 // 7 segment control bit
#define FD650_BIT_INTENS1 0x10 // 1 level brightness
#define FD650_BIT_INTENS2 0x20 // 2 level brightness
#define FD650_BIT_INTENS3 0x30 // 3 level brightness
#define FD650_BIT_INTENS4 0x40 // 4 level brightness
#define FD650_BIT_INTENS5 0x50 // 5 level brightness
#define FD650_BIT_INTENS6 0x60 // 6 level brightness
#define FD650_BIT_INTENS7 0x70 // 7 level brightness
#define FD650_BIT_INTENS8 0x00 // 8 level brightness

#define FD650_SYSOFF 0x0400 // disable display and key input
// enable display and key input
#define FD650_SYSON  (FD650_SYSOFF | FD650_BIT_ENABLE)
#define FD650_SLEEPOFF FD650_SYSOFF // close sleep
#define FD650_SLEEPON (FD650_SYSOFF | FD650_BIT_SLEEP) // open sleep
#define FD650_7SEG_ON (FD650_SYSON | FD650_BIT_7SEG) // enable 7seg mode
#define FD650_8SEG_ON (FD650_SYSON | 0x00) // enable 8seg mode
// enable display, key input, 1 level brightness
#define FD650_SYSON_1 (FD650_SYSON | FD650_BIT_INTENS1)
//......
// enable display, key input, 4 level brightness
#define FD650_SYSON_4 (FD650_SYSON | FD650_BIT_INTENS4)
//......
// enable display, key input, 8 level brightness
#define FD650_SYSON_8 (FD650_SYSON | FD650_BIT_INTENS8)

// command of set data
#define FD650_DIG0 0x1400 // display data on seg 0(need 8-bits-data)
#define FD650_DIG1 0x1500 // display data on seg 1(need 8-bits-data)
#define FD650_DIG2 0x1600 // display data on seg 2(need 8-bits-data)
#define FD650_DIG3 0x1700 // display data on seg 3(need 8-bits-data)

#define FD650_DOT 0x80 // display radix point

// command of read key input
#define FD650_GET_KEY 0x0700 // read key input and return the key value

/*
*	BEGIN : define for concerto gpio cmd
*/
typedef enum
{
  /*!
    power
    */
  GPIO_FP_SET_POWER_LBD = 0,
  /*!
    lock
    */
  GPIO_FP_SET_LOCK_LBD,
  /*!
    other
    */
  GPIO_FP_SET_COLON,
  /*!
    reserve
    */
  GPIO_FP_SET_RESERVE,
}GPIO_FP_SET_CMD;

 static u8 g_lbd_gpio_num[4];


static pan_hw_info_t g_gpio_p_info;

#define R_GPIO_BASE_ADDR            0xBF0A0000
#define R_GPIO0_WDATA                   R_GPIO_BASE_ADDR
#define R_GPIO0_WR_N                    (R_GPIO_BASE_ADDR + 0x4)
#define R_GPIO0_RDATA                   (R_GPIO_BASE_ADDR + 0x8)
#define R_GPIO0_MASK_N                 (R_GPIO_BASE_ADDR + 0xc)
#define R_GPIO1_WDATA                  (R_GPIO_BASE_ADDR + 0x10)
#define R_GPIO1_WR_N                    (R_GPIO_BASE_ADDR + 0x14)
#define R_GPIO1_RDATA                   (R_GPIO_BASE_ADDR + 0x18)
#define R_GPIO1_MASK_N                 (R_GPIO_BASE_ADDR + 0x1c)
#define R_AO_GPIO_WDATA              (R_GPIO_BASE_ADDR + 0x20)
#define R_AO_GPIO_WR_N                (R_GPIO_BASE_ADDR + 0x24)
#define R_AO_GPIO_RDATA               (R_GPIO_BASE_ADDR + 0x28)
#define R_AO_GPIO_MASK_N             (R_GPIO_BASE_ADDR + 0x2c)

#define R_PIN_BASE_ADDR              0xbf156000
#define R_PIN8_BASE_ADDR            R_PIN_BASE_ADDR
#define R_PIN0_BASE_ADDR            (R_PIN_BASE_ADDR + 0x4)
#define R_PIN1_BASE_ADDR            (R_PIN_BASE_ADDR + 0x8)
#define R_PIN2_BASE_ADDR            (R_PIN_BASE_ADDR + 0xc)
#define R_PIN3_BASE_ADDR            (R_PIN_BASE_ADDR + 0x10)
#define R_PIN4_BASE_ADDR            (R_PIN_BASE_ADDR + 0x14)
#define R_PIN5_BASE_ADDR            (R_PIN_BASE_ADDR + 0x18)
#define R_PIN6_BASE_ADDR            (R_PIN_BASE_ADDR + 0x1c)
#define R_PIN7_BASE_ADDR            (R_PIN_BASE_ADDR + 0x20)

#define R_IE_BASE_ADDR              0xbf156300
#define R_IE0_BASE_ADDR            R_IE_BASE_ADDR
#define R_IE1_BASE_ADDR            (R_IE_BASE_ADDR+0x4)
#define R_IE2_BASE_ADDR            (R_IE_BASE_ADDR+0x8)
#define R_IE3_BASE_ADDR            (R_IE_BASE_ADDR+0xc)

#define R_AO_GPIO_MODE 0xBF0A0120
#define R_AO_GPIO_STA_CLR 0xBF0A0124
#define R_AO_GPIO_STATE 0xBF0A012C

#define R_GPIO0_MODE 0xBF0A0100
#define R_GPIO0_STA_CLR 0xBF0A0104
#define R_GPIO0_STATE 0xBF0A010C

#define R_GPIO1_MODE 0xBF0A0110
#define R_GPIO1_STA_CLR 0xBF0A0114
#define R_GPIO1_STATE 0xBF0A011C


/*
*	END : define for concerto gpio com
*/

/******************************************************
 *   internal variables
 ******************************************************/
#define FP_MAX_LED_NUM  4
#define FP_RPT_KEY_SCAN_TIME  200 /* repeate key scan interval time in ms */
#define HK_SCAN_TIME  180 /* repeate key scan interval time in ms */
static const led_bitmap_t fp_bitmap[] =
{
  {'.', 0x80},
  {'0', 0x3F},  {'1', 0x06},  {'2', 0x5B},  {'3', 0x4F},
  {'4', 0x66},  {'5', 0x6D},  {'6', 0x7D},  {'7', 0x07},
  {'8', 0x7F},  {'9', 0x6F},  {'a', 0x77},  {'A', 0x77},
  {'b', 0x7C},  {'B', 0x7C},  {'c', 0x39},  {'C', 0x39},
  {'d', 0x5E},  {'D', 0x5E},  {'e', 0x79},  {'E', 0x79},
  {'f', 0x71},  {'F', 0x71},  {'g', 0x6F},  {'G', 0x3D},
  {'h', 0x76},  {'H', 0x76},  {'i', 0x04},  {'I', 0x30},
  {'j', 0x0E},  {'J', 0x0E},  {'l', 0x38},  {'L', 0x38},
  {'n', 0x54},  {'N', 0x37},  {'o', 0x5C},  {'O', 0x3F},
  {'p', 0x73},  {'P', 0x73},  {'q', 0x67},  {'Q', 0x67},
  {'r', 0x50},  {'R', 0x77},  {'s', 0x6D},  {'S', 0x6D},
  {'t', 0x78},  {'T', 0x31},  {'u', 0x3E},  {'U', 0x3E},
  {'y', 0x6E},  {'Y', 0x6E},  {'z', 0x5B},  {'Z', 0x5B},
  {':', 0x80},  {'-', 0x40},  {'_', 0x08},  {' ', 0x00}
};

#define FP_TABLE_SIZE sizeof(fp_bitmap) / sizeof(led_bitmap_t)

typedef struct
{
  u16 dis_buff[FP_MAX_LED_NUM];    /* Buffer for LED display scan */
  u16 cur_key_val;
  u16 last_key_val;
  u32 timer_id;
  fp_cfg_t config;
}fp_priv_t;

static BOOL irq_requested_fp = FALSE;
static BOOL irq_requested_gpio = FALSE;

extern BOOL uio_check_rpt_key_concerto(uio_type_t uio, u8 code, u8 ir_index);
static u8 fd650_read(void *p_bus_dev);
static void fd650_write(void *p_bus_dev, u16 cmd);

/******************************************************
 *   Internal functions
 ******************************************************/
static void fp_key_timer_cb(void)
{
  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
  fp_priv_t *p_fp = (fp_priv_t *)p_lld->p_fp;
  u8 key_data = 0;

  if (p_fp->config.fp_type == HAL_FD650)
  {
  	fd650_write(p_fp->config.p_bus_dev, FD650_SYSON_4 | FD650_8SEG_ON); 
    key_data = fd650_read(p_fp->config.p_bus_dev);
    if ((0 != key_data) && (0xFF != key_data))
    {
      p_fp->cur_key_val = key_data | (UIO_FRONTPANEL << 8);
      if (p_fp->last_key_val != p_fp->cur_key_val)
      {
        mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
        FP_PRINTF("key_value: %x\n", p_fp->cur_key_val);
        p_fp->last_key_val = p_fp->cur_key_val;
      }
      else if (TRUE == uio_check_rpt_key_concerto(UIO_FRONTPANEL, (u8)p_fp->cur_key_val, 0))
      {
        mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
        FP_PRINTF("#RptKey: %x\n", p_fp->cur_key_val);
      }
    }
    else if ((p_fp->last_key_val & 0xFF) != key_data)
    {
      p_fp->last_key_val = key_data | (UIO_FRONTPANEL << 8);
    }
  }
  else
  {
    key_data = p_fp->cur_key_val & 0xFF;
    if ((0 != key_data) && (0xFF != key_data))
    {
      /* check if current key is repeate key */
      if (TRUE == uio_check_rpt_key_concerto(UIO_FRONTPANEL, (u8)p_fp->cur_key_val, 0))
      {
        mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
        FP_PRINTF("#RptKey: %x\n", p_fp->cur_key_val);
      }
    }
  }
}

static u8 fp_find_bitmap(fp_cfg_t *p_cfg, u8 data)
{
  u32 i = 0;
  u8 bit =0;
  for(i = 0; i < p_cfg->map_size; i++)
  {
    if(p_cfg->p_map[i].ch == data)
    {
      bit = p_cfg->p_map[i].bitmap;
      break;
    }
  }

  if(i >= p_cfg->map_size)  //if not found
  {
    bit = 0;
  }

  return bit;
}

static uio_device_t* g_p_dev;
	
static void scankey_isr(void)
{
    u32 key_press[2] ={0};
    u32 key_int = 0;
    u32 key_release[2] = {0};
    u8  key_int_status = 0;
    //uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
    //                                   DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    g_p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                       DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    uio_device_t *p_dev = g_p_dev;
    lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
    uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
    fp_priv_t *p_fp = (fp_priv_t *)p_lld->p_fp;
    u32 frontkey_value = 0;
    key_press[0] = 0;
    key_press[1] = 0;
    key_release[0] = 0;
    key_release[1] = 0;

    key_int = hal_get_u32((unsigned long *)LEDKB_KEY_PRESS0);
    key_int_status = (key_int >> 24) & 0x0f;

    if(hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT)& 0x100000)
    {
        key_press[0] = hal_get_u32((unsigned long *)LEDKB_KEY_PRESS0);
        key_release[0] = hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE0);
        hal_put_u32((unsigned long *)LEDKB_KEY_PRESS0,key_press[0]);
        hal_put_u32((unsigned long *)LEDKB_KEY_RELEASE0,key_release[0]);
    }
    if(hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT) & 0x200000)
    {
        key_press[1] = hal_get_u32((unsigned long *)LEDKB_KEY_PRESS1);
        key_release[1] = hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE1);
        hal_put_u32((unsigned long *)LEDKB_KEY_PRESS1,key_press[1]);
        hal_put_u32((unsigned long *)LEDKB_KEY_RELEASE1,key_release[1]);
    }

    hal_put_u32((unsigned long *)LEDKB_KEY_PRESS0,key_int);

    key_press[0] = key_press[0]&0x3ffff;
    key_release[0] = key_release[0]&0x3ffff;
    key_press[1] = key_press[1]&0x3ffff;
    key_release[1] = key_release[1]&0x3ffff;

    if(key_int_status&0x05)//Any key been press
    {
      hal_timer_stop(p_fp->timer_id);          // in case rcv repeat press key
      if(key_press[0])
      {
        frontkey_value = (key_press[0] >> 4);
        p_fp->cur_key_val = frontkey_value | (UIO_FRONTPANEL << 8);
        mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
        FP_PRINTF("scankey_isr key0_value=%08x been press \n",p_fp->cur_key_val);
      }
      if(key_press[1])
      {
        frontkey_value = (key_press[1] >> 4);
        p_fp->cur_key_val = frontkey_value | (UIO_FRONTPANEL << 8);
        mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
        FP_PRINTF("scankey_isr key1_value=%08x been press \n",p_fp->cur_key_val);
      }
      hal_timer_start(p_fp->timer_id);
    }
    if(key_int_status&0x0a)//Any key been release
    {
      hal_timer_stop(p_fp->timer_id);
      if(key_release[0])
      {
          frontkey_value = (key_release[0] >> 4);
          p_fp->cur_key_val = frontkey_value | (UIO_FRONTPANEL << 8);
          mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
          FP_PRINTF("scankey_isr key0_value=%08x been release  \n",p_fp->cur_key_val);
      }
      if(key_release[1])
      {
          frontkey_value = (key_release[1] >> 4);
          p_fp->cur_key_val = frontkey_value | (UIO_FRONTPANEL << 8);
          mtos_fifo_put(&p_priv->fifo, p_fp->cur_key_val);
          FP_PRINTF("scankey_isr key1_value=%08x been release  \n",p_fp->cur_key_val);
      }
    }
}



static void gpio_ao_gpio_clr(void)
{
	*((volatile u32 *)R_AO_GPIO_STA_CLR)=0xff;
	*((volatile u32 *)R_AO_GPIO_STA_CLR)=0x00;
}

static void gpio_gpio0_clr(void)
{
	*((volatile u32 *)R_GPIO0_STA_CLR)=0xffffffff;
	*((volatile u32 *)R_GPIO0_STA_CLR)=0x00;
}

static void gpio_gpio1_clr(void)
{
	*((volatile u32 *)R_GPIO1_STA_CLR)=0xffffffff;
	*((volatile u32 *)R_GPIO1_STA_CLR)=0x00;
}

static void concerto_gpio_config( pan_gpio_info_t  com_info_para)
{
    u32 gpio_temp=0;
	u32 gpio_offset=0;
	pan_gpio_info_t  * com_info=&com_info_para;



	/*** BEGIN: debug code ***/
	//mtos_printk("para = %d\n",com_info->pos);

	/*** END: debug code ***/

	/**************COM  SET *****************/

	if((com_info->pos==37)|(com_info->pos==38))
	{
		mtos_printk("GPIO COM MODULE CONFIG GPIO ERROR!!!\n");
		return;
	}
	else
	{
		//mtos_printk("concerto_gpio_config com_info->pos = %d\n",com_info->pos);
	}

	/*************** IE SEL CONFIG ******************/
	if((com_info->pos>0)&&(com_info->pos<30))
	{
		*(volatile u32 *)R_IE0_BASE_ADDR|=(0x1<<(31-gpio_offset));
	}
	else if((com_info->pos>29)&&(com_info->pos<63))
	{
		switch(com_info->pos)
		{
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
				*(volatile u32 *)R_IE1_BASE_ADDR|=(0x1<<(31-(gpio_offset-30)));
				break;
			case 36:
				*(volatile u32 *)R_IE1_BASE_ADDR|=(0x1<<23);
				break;
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
				*(volatile u32 *)R_IE1_BASE_ADDR|=(0x1<<(19-(gpio_offset-39)));
				break;
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
				*(volatile u32 *)R_IE2_BASE_ADDR|=(0x1<<(31-(gpio_offset-49)));
				break;
			default:
				break;
		}
	}
	else if((com_info->pos>99)&&(com_info->pos<108))
	{

		gpio_offset = com_info->pos - 100;
		switch(com_info->pos)
		{
			case 100:
			case 101:
			case 102:
			case 103:
				*(volatile u32 *)R_IE3_BASE_ADDR|=(0x1<<(26-gpio_offset));
				break;
			case 104:
				*(volatile u32 *)R_IE3_BASE_ADDR|=(0x1<<31);
			case 105:
			case 106:
				*(volatile u32 *)R_IE3_BASE_ADDR|=(0x1<<28);
			case 107:
				*(volatile u32 *)R_IE3_BASE_ADDR|=(0x1<<29);
				break;
			default:
				break;
		}


	}


	/********** AO_GPIO config ***********/
	if((com_info->pos>99)&&(com_info->pos<108))
	{
	 	gpio_offset = com_info->pos -100;

		//MASK CONFIG
		*(volatile u32 *)R_AO_GPIO_MASK_N=0xff;

		//WR CONFIG
		*(volatile u32 *)R_AO_GPIO_WR_N|=(0x1<<gpio_offset);;

		if((com_info->pos>99)&&(com_info->pos<104))
		{
			//PIN8 SEL CONFIG
			gpio_temp = *((volatile u32 *)R_PIN8_BASE_ADDR);
			gpio_temp &= ~(0xf<<(12-gpio_offset*4));
			gpio_temp |= (0x2<<(12-gpio_offset*4));
			*(volatile u32 *)R_PIN8_BASE_ADDR=gpio_temp;
		}
		else if((com_info->pos>103)&&(com_info->pos<108))
		{
			//PIN SEL CONFIG
			gpio_temp = *((volatile u32 *)R_PIN8_BASE_ADDR);
			gpio_temp &= ~(0xf<<(28-(gpio_offset-4)*4));
			gpio_temp |= (0x2<<(28-(gpio_offset-4)*4));
			*(volatile u32 *)R_PIN8_BASE_ADDR=gpio_temp;
		}
	 }
	 /********** GPIO0 config ***********/
	 else if((com_info->pos>=0)&&(com_info->pos<33)) // GPIO0
	 {
	 	gpio_offset = com_info->pos;

		//MASK CONFIG
		*(volatile u32 *)R_GPIO0_MASK_N=0xffffffff;

		//WR CONFIG
		*(volatile u32 *)R_GPIO0_WR_N|=0xffffffff;

		if((com_info->pos>=0)&&(com_info->pos<14))
		{
			//PIN0 SEL CONFIG
			gpio_temp = *((volatile u32 *)R_PIN0_BASE_ADDR);
			gpio_temp &= (~(3<<(26-gpio_offset*2)));
			gpio_temp |= (0x2<<(26-gpio_offset*2));
			*(volatile u32 *)R_PIN0_BASE_ADDR=gpio_temp;
			gpio_temp = *((volatile u32 *)R_PIN0_BASE_ADDR);
		}
		else
		{
			switch(com_info->pos)
			{
				case 14://PIN0 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN0_BASE_ADDR);
					gpio_temp &= ~(3<<28);
					gpio_temp |= (0x2<<28);
					*(volatile u32 *)R_PIN0_BASE_ADDR=gpio_temp;
					break;
				case 15://PIN1 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN1_BASE_ADDR);
					gpio_temp &= ~(3<<20);
					gpio_temp |= (0x2<<20);
					*(volatile u32 *)R_PIN1_BASE_ADDR=gpio_temp;
				case 16://PIN1 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN1_BASE_ADDR);
					gpio_temp &= ~(3<<16);
					gpio_temp |= (0x2<<16);
					*(volatile u32 *)R_PIN1_BASE_ADDR=gpio_temp;
					break;
				case 17:
				case 18:
				case 19:
				case 20:	//PIN1 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN1_BASE_ADDR);
					gpio_temp &= ~(3<<((gpio_offset-17)*4));
					gpio_temp |= (0x2<<((gpio_offset-17)*4));
					*(volatile u32 *)R_PIN1_BASE_ADDR=gpio_temp;
					break;
				case 21://PIN2 SEL CONFIG
				case 22://PIN2 SEL CONFIG
				case 23://PIN2 SEL CONFIG
				case 24://PIN2 SEL CONFIG
				case 25://PIN2 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN2_BASE_ADDR);
					gpio_temp &= ~(3<<(8-(gpio_offset-21)*2));
					gpio_temp |= (0x2<<(8-(gpio_offset-21)*2));
					*(volatile u32 *)R_PIN2_BASE_ADDR=gpio_temp;
					break;
				case 26://PIN2 SEL CONFIG
				case 27://PIN2 SEL CONFIG
				case 28://PIN2 SEL CONFIG
				case 29://PIN2 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN2_BASE_ADDR);
					gpio_temp &= ~(3<<(10+(gpio_offset-26)*2));
					gpio_temp |= (0x2<<(10+(gpio_offset-26)*2));
					*(volatile u32 *)R_PIN2_BASE_ADDR=gpio_temp;
					break;
				case 30://PIN2 SEL CONFIG
				       gpio_temp = *((volatile u32 *)R_PIN2_BASE_ADDR);
					gpio_temp &= ~(3<<24);
					gpio_temp |= (0x2<<24);
					*(volatile u32 *)R_PIN2_BASE_ADDR=gpio_temp;
					break;
				case 31://PIN2 SEL CONFIG
					gpio_temp = *((volatile u32 *)R_PIN2_BASE_ADDR);
					gpio_temp &= ~(3<<28);
					gpio_temp |= (0x2<<28);
					*(volatile u32 *)R_PIN2_BASE_ADDR=gpio_temp;
					break;
				default:
					break;
			}

		}
	 }
	 /********** GPIO1 config ***********/
	 else if((com_info->pos>31)&&(com_info->pos<63)) //GPIO1
	 {

	 	gpio_offset = com_info->pos;

		//MASK CONFIG
		*(volatile u32 *)R_GPIO1_MASK_N=0xffffffff;

		//WR CONFIG
		*(volatile u32 *)R_GPIO1_WR_N|=0xffffffff;

		switch(com_info->pos)
		{
			case 32:
			case 33:
			case 34:
			case 35:	//PIN3 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN3_BASE_ADDR);
				gpio_temp &= ~(3<<((gpio_offset-32)*4));
				gpio_temp |= (0x2<<((gpio_offset-32)*4));
				*(volatile u32 *)R_PIN3_BASE_ADDR=gpio_temp;
				break;
			case 36://PIN3 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN3_BASE_ADDR);
				gpio_temp &= ~(3<<20);
				gpio_temp |= (0x2<<20);
				*(volatile u32 *)R_PIN3_BASE_ADDR=gpio_temp;
				break;
			case 39:
			case 40:
			case 41:	//PIN4 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN4_BASE_ADDR);
				gpio_temp &= ~(3<<(8+(gpio_offset-39)*4));
				gpio_temp |= (0x2<<(8+(gpio_offset-39)*4));
				*(volatile u32 *)R_PIN4_BASE_ADDR=gpio_temp;
				break;
			case 42:
			case 43:	//PIN4 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN4_BASE_ADDR);
				gpio_temp &= ~(3<<(18+(gpio_offset-42)*2));
				gpio_temp |= (0x2<<(18+(gpio_offset-42)*2));
				*(volatile u32 *)R_PIN4_BASE_ADDR=gpio_temp;
				break;
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:	//PIN5 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN5_BASE_ADDR);
				gpio_temp &= ~(3<<(gpio_offset-44)*2);
				gpio_temp |= (0x2<<(gpio_offset-44)*2);
				*(volatile u32 *)R_PIN5_BASE_ADDR=gpio_temp;
				break;
			case 49:
			case 50:
			case 51:
			case 52:	//PIN5 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN5_BASE_ADDR);
				gpio_temp &= ~(3<<(16+(gpio_offset-49)*2));
				gpio_temp |= (0x2<<(16+(gpio_offset-49)*2));
				*(volatile u32 *)R_PIN5_BASE_ADDR=gpio_temp;
				break;
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:	//PIN6 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN6_BASE_ADDR);
				gpio_temp &= ~(3<<(gpio_offset-53)*2);
				gpio_temp |= (0x2<<(gpio_offset-53)*2);
				*(volatile u32 *)R_PIN6_BASE_ADDR=gpio_temp;
				break;
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:	//PIN6 SEL CONFIG
				gpio_temp = *((volatile u32 *)R_PIN6_BASE_ADDR);
				gpio_temp &= ~(3<<(12+(gpio_offset-53)*4));
				gpio_temp |= (0x2<<(12+(gpio_offset-53)*4));
				*(volatile u32 *)R_PIN6_BASE_ADDR=gpio_temp;
				break;
			default:
				break;
		}
	 }
}


static void gpio_delay_ms ( u32 number )
{
   u32 clk = 0;
   u32 dms = 0;

   hal_module_clk_get(HAL_CPU0, &clk);
   dms = number * (clk / 1000);
   while(dms --)
    {
        asm volatile ("nop");
    }
}

u8 gospell_shake_remove_flag = 0xf;
u8 gospell_shake_remove(void)
{
	int ao_gpio_temp=0;
	int gpio0_temp=0;
	gpio_delay_ms(5);	
	ao_gpio_temp = *((volatile u32 *)0xBF0A0028);
	gpio0_temp = *((volatile u32 *)0xBF0A0008);	

	if (gospell_shake_remove_flag == 1)
	{
		if((ao_gpio_temp&(0x1<<1))&&\
		(ao_gpio_temp&(0x1<<2))&&\
		(ao_gpio_temp&(0x1<<5))&&\
		(ao_gpio_temp&(0x1<<6))&&\
		(ao_gpio_temp&(0x1<<7))&&//)
		(ao_gpio_temp&(0x1<<7))&&\
		(gpio0_temp&(0x1<<30))&&\
		(gpio0_temp&(0x1<<31)))
		{
			gpio_ao_gpio_clr();
			gpio_gpio0_clr();
			gpio_gpio1_clr();		
			return 0;
		}	
	}
	else
	{
		gpio_delay_ms(30);
	}

	return 1;
}

static u8 concerto_gpio_Read( pan_gpio_info_t * com_info)
{
	u32 gpio_offset=0;
	
	if(gospell_shake_remove()!=1)
		return 0;	

	/*
	*	AO_GPIO READ
	*/
	 if((com_info->pos>99)&&(com_info->pos<108))  //AO_GPIO
	 {
	 	gpio_offset=com_info->pos -100;

		if((*(volatile u32 *)R_AO_GPIO_STATE)&(0x1<<gpio_offset))
		{
			gpio_delay_ms(5);
			gpio_ao_gpio_clr();
			return 1;
		}
		else
		{
			return 0;
		}
	 }
	 else if((com_info->pos>=0)&&(com_info->pos<32)) // GPIO0
	 {
	 	gpio_offset=com_info->pos;
		//mtos_printk("concerto_gpio_Read com_info->pos = %d ,R_GPIO0_STATE = 0x%x\n",gpio_offset,*(volatile u32 *)R_GPIO0_STATE);
		if((*(volatile u32 *)R_GPIO0_STATE)&(0x1<<gpio_offset))
		{			
			gpio_delay_ms(5);
			gpio_gpio0_clr();
			return 1;
		}
		else
		{
			return 0;
		}
	 }
	 else if((com_info->pos>31)&&(com_info->pos<63)) //GPIO1
	 {
	 	gpio_offset=com_info->pos-31;
		if((*(volatile u32 *)R_GPIO1_STATE)&(0x1<<gpio_offset))
		{
			gpio_delay_ms(5);
			gpio_gpio1_clr();
			return 1;
		}
		else
		{
			return 0;
		}
	 }
	 return 0xff;
}

static void gpio_led_set(u32 gpio_num,u32 ledval)
{
	u32 temp=0;
	if((gpio_num>99)&&(gpio_num<108))
	{
		gpio_num = gpio_num-100;
		temp = *((volatile u32 *)R_AO_GPIO_WR_N);
		temp &= ~(0x1<<gpio_num);
		*(volatile u32 *)R_AO_GPIO_WR_N=temp;

		temp = *((volatile u32 *)R_AO_GPIO_WDATA);
		if(ledval==1)
		{
			temp |= (0x1<<gpio_num);
		}
		else
		{
			temp &= (~(0x1<<gpio_num));
		}
		*(volatile u32 *)R_AO_GPIO_WDATA=temp;
	}
	else if((gpio_num>=0)&&(gpio_num<32))
	{
		temp = *((volatile u32 *)R_GPIO0_WR_N);
		temp &= (~(0x1<<gpio_num));
		*(volatile u32 *)R_GPIO0_WR_N=temp;

		temp = *((volatile u32 *)R_GPIO0_WDATA);
		if(ledval==1)
		{
			temp |= (0x1<<gpio_num);
		}
		else
		{
			temp &= (~(0x1<<gpio_num));
		}
		*(volatile u32 *)R_GPIO0_WDATA=temp;

	}
	else if((gpio_num>=31)&&(gpio_num<63))
	{
		gpio_num = gpio_num -32;
		temp = *((volatile u32 *)R_GPIO1_WR_N);
		temp &= (~(0x1<<gpio_num));
		*(volatile u32 *)R_GPIO1_WR_N=temp;

		temp = *((volatile u32 *)R_GPIO1_WDATA);
		if(ledval==1)
		{
			temp |= (0x1<<gpio_num);
		}
		else
		{
			temp &= (~(0x1<<gpio_num));
		}
		*(volatile u32 *)R_GPIO1_WDATA=temp;
	}
	else
	{
		mtos_printk("gpio_display receive invalid gpio\n");
	}
}

static void gpio_display(u32 p_data)
{
	pan_hw_info_t led_p_info;
	u32 gpio_num=0;
	u32 ledval=0;
	pan_gpio_info_t  led_com[4];

	//mtos_printk("gpio_display get p_data = 0x%x\n",p_data);

	/*
	*	p_data   bit8         represents 		led value
	*	p_data   bit0-7      represents 		led gpio num
	*/
	led_com[0].pos=p_data&0x7f;
	led_p_info.com[0]=led_com[0];

	ledval=(p_data&0x80)>>7;
	gpio_num=p_data&0x7f;
	concerto_gpio_config(led_p_info.com[0]);
	gpio_led_set(gpio_num,ledval);

}

static void gpio_scankey_isr(void)
{
	u32 i;
	//uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
	//                                   DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
	g_p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                       DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
	uio_device_t *p_dev = g_p_dev;
	lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
	uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

	u32 frontkey_value = 0;

	for(i=0;i<g_gpio_p_info.num_com;i++)
	{
		if(concerto_gpio_Read(&(g_gpio_p_info.com[i])))
		{
			frontkey_value = 0x100+i;
			//mtos_printk("gpio_scankey_isr > mtos_fifo_put > frontkey_value = %d\n",frontkey_value);
			mtos_fifo_put(&p_priv->fifo, frontkey_value);
			break;
		}
	}

	if(i==4)
	{
		//mtos_printk("no press scaned \n",i);
	}

}

#if 0
static u32 g_ticks_value = 0;

static void repeat_key_process(u32 frontkey_value)
{
	g_ticks_value = mtos_ticks_get();

}
#endif

void orris_gpio_scankey(void *param)
{
	//u32 i;
	uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
	                                   DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
	lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
	uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;

	u32 frontkey_value = 0;
	u32 gpio_temp=0;
	//u32 gpio_offset=0;

	while(1)
	{
		/*
		*	first time
		*	set ao_gpio1 input
		*	set ao_gpio2 input
		*/
		gpio_temp = *(volatile u32 *)R_AO_GPIO_WR_N;
		gpio_temp |= (0x1<<1);
		*(volatile u32 *)R_AO_GPIO_WR_N=gpio_temp;

		gpio_temp = *(volatile u32 *)R_AO_GPIO_WR_N;
		gpio_temp |= (0x1<<2);
		*(volatile u32 *)R_AO_GPIO_WR_N=gpio_temp;

		mtos_task_sleep(30);
		#if 0
		mtos_printk("1st >>> set input R_AO_GPIO_WR_N = 0x%x\n",*(volatile u32 *)R_AO_GPIO_WR_N);
		mtos_printk("1st >>> time set input R_AO_GPIO_RDATA = 0x%x\n",*(volatile u32 *)R_AO_GPIO_RDATA);
		#endif

		/*
		*	get key1    value zero represet key down
		*/
		if(!((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<1)))
		{
			frontkey_value = 0x100+1;
			mtos_fifo_put(&p_priv->fifo, frontkey_value);
			mtos_printk("orris_gpio_scankey frontkey_value = 0x%d>>>>>\n",frontkey_value);
			mtos_task_sleep(200);
			continue;
		}

		/*
		*	get key2  value zero represet key down
		*/
	 	if(!((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<2)))
		{
			frontkey_value = 0x100+2;
			mtos_fifo_put(&p_priv->fifo, frontkey_value);
			mtos_printk("orris_gpio_scankey frontkey_value = 0x%d>>>>>\n",frontkey_value);
			mtos_task_sleep(200);
			continue;
		}

		/*
		*	if key1 key2 no value
		*	second time
		*	set ao_gpio1 input
		*	set ao_gpio2 output
		*/
		gpio_temp = *(volatile u32 *)R_AO_GPIO_WR_N;
		gpio_temp |= (0x1<<1);
		*(volatile u32 *)R_AO_GPIO_WR_N=gpio_temp;

		gpio_temp = *(volatile u32 *)R_AO_GPIO_WR_N;
		gpio_temp &= ~(0x1<<2);
		*(volatile u32 *)R_AO_GPIO_WR_N=gpio_temp;

		gpio_temp = *(volatile u32 *)R_AO_GPIO_WDATA;
		gpio_temp &= ~(0x1<<2);
		*(volatile u32 *)R_AO_GPIO_WDATA=gpio_temp;

		#if 0
		mtos_printk("2nd >>> set input R_AO_GPIO_WR_N = 0x%x\n",*(volatile u32 *)R_AO_GPIO_WR_N);
		mtos_printk("2nd >>> set input R_AO_GPIO_RDATA = 0x%x\n",*(volatile u32 *)R_AO_GPIO_RDATA);
		#endif

		/*
		*	get key3  value zero represet key down
		*/
	 	if(!((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<1)))
		{
			frontkey_value = 0x100+3;
			mtos_fifo_put(&p_priv->fifo, frontkey_value);
			mtos_printk("orris_gpio_scankey frontkey_value = 0x%d>>>>>\n",frontkey_value);
			mtos_task_sleep(200);
			continue;
		}

	}

}

u32 get_key_mask_value(u32 bit)
{
  u32 key_value[8]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80};
  u32 scankey_mask = 0;
  scankey_mask =(key_value[bit] << 10) | 0xf;
  return scankey_mask;
}

void DumpLedHostControlRegister(void)
{
  u32 i = 0;
  FP_PRINTF("LEDKB_CTRL:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_CTRL));
  FP_PRINTF("LEDKB_TIMESET:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_TIMESET));
  FP_PRINTF("LEDKB_TIMESEL:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_TIMESEL));
  FP_PRINTF("LEDKB_CLKFREQ_KEYFILT:0x%08x\n",
    hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT));
  FP_PRINTF("LEDKB_CLK_PARA:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_CLK_PARA));
  FP_PRINTF("LEDKB_DEN_PARA:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_DEN_PARA));
  for(i = 0;i < 18;i++)
    FP_PRINTF("LEDKB_DAT_PARA(%d):0x%08x\n",i,hal_get_u32((unsigned long *)LEDKB_DAT_PARA(i)));
  FP_PRINTF("LEDKB_DAT_DELAY_NUM:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_DAT_DELAY_NUM));
  FP_PRINTF("LEDKB_KEY_VALUE0:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_VALUE0));
  FP_PRINTF("LEDKB_KEY_VALUE1:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_VALUE1));
  FP_PRINTF("LEDKB_KEY_PRESS0:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_PRESS0));
  FP_PRINTF("LEDKB_KEY_PRESS1:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_PRESS1));
  FP_PRINTF("LEDKB_KEY_RELEASE0:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE0));
  FP_PRINTF("LEDKB_KEY_RELEASE1:0x%08x\n",hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE1));

  return;
}

void RwRegister_test(void)
{
  u32 i = 0;
  u32 reg = 0;
  u8 ledkb_ctrl_28_24_5bit = 0;
  u8 ledkb_ctrl_18_16_3bit = 0;
  u8 ledkb_ctrl_12_8_5bit = 0;
  u8 ledkb_ctrl_5_3_3bit = 0;
  u8 ledkb_ctrl_0_1bit = 0;
  u32 ledkb_keyfilt_27_24_4bit = 0;
  u32 ledkb_keyfilt_17_0_18bit = 0;
  u32 ledkb_keyfilt_21_20_2bit = 0;
  FP_PRINTF("Set All register to 0 test....\n");
  for(i = 0;i < 26;i++)
      hal_put_u32((unsigned long *)(LEDKB_HOST_REG_BASE + 4*i),0);
  FP_PRINTF("All register should be 0x00000000\n");
  DumpLedHostControlRegister();

  FP_PRINTF("Set All register to 0xffffffff test....\n");
  for(i = 0;i < 26;i++)
      hal_put_u32((unsigned long *)(LEDKB_HOST_REG_BASE + 4*i),0xffffffff);
  reg = hal_get_u32((unsigned long *)LEDKB_CTRL);
  ledkb_ctrl_28_24_5bit = (reg >> 24)&0x1f;
  ledkb_ctrl_18_16_3bit = (reg >> 16)&0x7;
  ledkb_ctrl_12_8_5bit = (reg >> 8)&0x1f;
  ledkb_ctrl_5_3_3bit = (reg >> 3)&0x7;
  ledkb_ctrl_0_1bit = reg&0x1;
  FP_PRINTF("ledkb_ctrl_28_24_5bit=0x%02x expect:0x1f\n",ledkb_ctrl_28_24_5bit);
  FP_PRINTF("ledkb_ctrl_18_16_3bit=0x%02x expect:0x07\n",ledkb_ctrl_18_16_3bit);
  FP_PRINTF("ledkb_ctrl_12_8_5bit=0x%02x expect:0x1f\n",ledkb_ctrl_12_8_5bit);
  FP_PRINTF("ledkb_ctrl_5_3_3bit=0x%02x expect:0x07\n",ledkb_ctrl_5_3_3bit);
  FP_PRINTF("ledkb_ctrl_0_1bit=0x%02x expect:0x01\n",ledkb_ctrl_0_1bit);
  FP_PRINTF("LEDKB_TIMESET:0x%08x expect:0xffffffff\n",
                  hal_get_u32((unsigned long *)LEDKB_TIMESET));
  FP_PRINTF("LEDKB_TIMESEL:0x%08x expect:0xff03ffff\n",
                  hal_get_u32((unsigned long *)LEDKB_TIMESEL));
  reg = hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT);
  ledkb_keyfilt_17_0_18bit = reg&0x1ffff;
  ledkb_keyfilt_21_20_2bit = (reg >> 20)&0x3;
  ledkb_keyfilt_27_24_4bit = (reg >> 24)&0xf;

  FP_PRINTF("ledkb_keyfilt_27_24_4bit=0x%08x expect:0x0000000f\n",ledkb_keyfilt_27_24_4bit);
  FP_PRINTF("ledkb_keyfilt_21_20_2bit=0x%08x expect:0x00000003\n",ledkb_keyfilt_21_20_2bit);
  FP_PRINTF("ledkb_keyfilt_17_0_18bit=0x%08x expect:0x0001ffff\n",ledkb_keyfilt_17_0_18bit);
  FP_PRINTF("LEDKB_CLK_PARA:0x%08x expect:0xffffffff\n",
              hal_get_u32((unsigned long *)LEDKB_CLK_PARA));
  FP_PRINTF("LEDKB_DEN_PARA:0x%08x expect:0xffffffff\n",
               hal_get_u32((unsigned long *)LEDKB_DEN_PARA));
  for(i = 0;i < 18;i++)
     FP_PRINTF("LEDKB_DAT_PARA(%d):0x%08x expect:0xffffffff\n",i,
               hal_get_u32((unsigned long *)LEDKB_DAT_PARA(i)));
  FP_PRINTF("LEDKB_DAT_DELAY_NUM:0x%08x expect:0x000000ff\n",
              hal_get_u32((unsigned long *)LEDKB_DAT_DELAY_NUM));
  FP_PRINTF("LEDKB_KEY_VALUE0:0x%08x expect:0x00000000\n",
              hal_get_u32((unsigned long *)LEDKB_KEY_VALUE0));
  FP_PRINTF("LEDKB_KEY_VALUE1:0x%08x expect:0x00000000\n",
               hal_get_u32((unsigned long *)LEDKB_KEY_VALUE1));
  FP_PRINTF("LEDKB_KEY_PRESS0:0x%08x expect:0x00000000\n",
                hal_get_u32((unsigned long *)LEDKB_KEY_PRESS0));
  FP_PRINTF("LEDKB_KEY_PRESS1:0x%08x expect:0x00000000\n",
                    hal_get_u32((unsigned long *)LEDKB_KEY_PRESS1));
  FP_PRINTF("LEDKB_KEY_RELEASE0:0x%08x expect:0x00000000\n",
                   hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE0));
  FP_PRINTF("LEDKB_KEY_RELEASE1:0x%08x expect:0x00000000\n",
                    hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE1));
  FP_PRINTF("Set even register to 0 and Set odd register to xffffffff test....\n");
  for(i = 0;i < 26;i++)
  {
    if((i % 2)==0)
        hal_put_u32((unsigned long *)(LEDKB_HOST_REG_BASE + 4*i),0xffffffff);
    else
           hal_put_u32((unsigned long *)(LEDKB_HOST_REG_BASE + 4*i),0);
  }
  reg = hal_get_u32((unsigned long *)LEDKB_CTRL);
  ledkb_ctrl_28_24_5bit = (reg >> 24)&0x1f;
  ledkb_ctrl_18_16_3bit = (reg >> 16)&0x7;
  ledkb_ctrl_12_8_5bit = (reg >> 8)&0x1f;
  ledkb_ctrl_5_3_3bit = (reg >> 3)&0x7;
  ledkb_ctrl_0_1bit = reg&0x1;
  FP_PRINTF("ledkb_ctrl_28_24_5bit=0x%02x expect:0x1f\n",ledkb_ctrl_28_24_5bit);
  FP_PRINTF("ledkb_ctrl_18_16_3bit=0x%02x expect:0x07\n",ledkb_ctrl_18_16_3bit);
  FP_PRINTF("ledkb_ctrl_12_8_5bit=0x%02x expect:0x1f\n",ledkb_ctrl_12_8_5bit);
  FP_PRINTF("ledkb_ctrl_5_3_3bit=0x%02x expect:0x07\n",ledkb_ctrl_5_3_3bit);
  FP_PRINTF("ledkb_ctrl_0_1bit=0x%02x expect:0x01\n",ledkb_ctrl_0_1bit);
  FP_PRINTF("LEDKB_TIMESET:0x%08x expect:0x00000000\n",
                                  hal_get_u32((unsigned long *)LEDKB_TIMESET));
  FP_PRINTF("LEDKB_TIMESEL:0x%08x expect:0xff03ffff\n",
                                  hal_get_u32((unsigned long *)LEDKB_TIMESEL));

  reg = hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT);
  FP_PRINTF("LEDKB_INTMASK_KEYFILT=0x%08x expect:0x00000000\n",reg);
  FP_PRINTF("LEDKB_CLK_PARA:0x%08x expect:0xffffffff\n",
                                  hal_get_u32((unsigned long *)LEDKB_CLK_PARA));
  FP_PRINTF("LEDKB_DEN_PARA:0x%08x expect:0x00000000\n",
                                 hal_get_u32((unsigned long *)LEDKB_DEN_PARA));
  for(i = 0;i < 18;i++)
  {
    if((i % 2) == 0)
     FP_PRINTF("LEDKB_DAT_PARA(%d):0x%08x expect:0xffffffff\n",i,
                                    hal_get_u32((unsigned long *)LEDKB_DAT_PARA(i)));
    else
     FP_PRINTF("LEDKB_DAT_PARA(%d):0x%08x expect:0x00000000\n",i,
                                    hal_get_u32((unsigned long *)LEDKB_DAT_PARA(i)));
  }
  FP_PRINTF("LEDKB_DAT_DELAY_NUM:0x%08x expect:0x000000ff\n",
                hal_get_u32((unsigned long *)LEDKB_DAT_DELAY_NUM));
  FP_PRINTF("LEDKB_KEY_VALUE0:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_VALUE0));
  FP_PRINTF("LEDKB_KEY_VALUE1:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_VALUE1));
  FP_PRINTF("LEDKB_KEY_PRESS0:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_PRESS0));
  FP_PRINTF("LEDKB_KEY_PRESS1:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_PRESS1));
  FP_PRINTF("LEDKB_KEY_RELEASE0:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE0));
  FP_PRINTF("LEDKB_KEY_RELEASE1:0x%08x expect:0x00000000\n",
                                   hal_get_u32((unsigned long *)LEDKB_KEY_RELEASE1));
  FP_PRINTF("RwRegister_test over\n");
}

void SetHostFreq(u32 clk)
{
    u32 div = 0;
    u32 ledkb_time_set = 0;
    u32 need_num_clk = 0;
    u32 one_led_display_clk = 0;
    u32 reg_value = 0;
    FP_PRINTF("SetClk:%d\n",clk);
    if(clk == 0) return;
    div = 12000000 / clk - 1;
    if(div > 255)
    {
       FP_PRINTF("Error Clk too low!!!(div>256) %d\n",div);
       return;
    }
    ledkb_time_set = (div << 24)|0xf;
    *((volatile u32 *)LEDKB_TIMESEL) = ledkb_time_set;
    FP_PRINTF("LEDKB_TIMESEL =0x%08x\n", *((volatile u32 *)LEDKB_TIMESEL));
    need_num_clk = 20 * clk / 1000;
    one_led_display_clk = (need_num_clk - 32*8)/4;
    if(one_led_display_clk < 20)
    {
      FP_PRINTF("Error Clk too low %d!!!\n",one_led_display_clk);
      return;
    }
    if(one_led_display_clk > 65535)
    {
      FP_PRINTF("Error Clk too high %d!!!\n",one_led_display_clk);
      return;
    }
    reg_value = (one_led_display_clk << 16)+32;
    *((volatile u32 *)LEDKB_TIMESET) = reg_value;
    FP_PRINTF("LEDKB_TIMESET=%08x\n", *((volatile u32 *)LEDKB_TIMESET));
}

RET_CODE  kbd_host_init_for_ct1642(void)
{
    u32 seg_data[4] ={0};
    //FP_PRINTF("Register_default\n");
    //DumpLedHostControlRegister();
    FP_PRINTF("ledkeyscan_init(ct1642)...\n");
    seg_data[0] = 0xff;
    seg_data[1] = 0xff;
    seg_data[2] = 0xff;
    seg_data[3] = 0xff;

    hal_put_u32((unsigned long *)LEDKB_CLK_PARA,0xb00000);
    hal_put_u32((unsigned long *)LEDKB_DAT_DELAY_NUM,1);
    hal_put_u32((unsigned long *)LEDKB_INTMASK_KEYFILT,0x00100ff0);
    SetHostFreq(200*FREQ_KHZ);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),(seg_data[0] << 10) | 0xe);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),(seg_data[1] << 10) | 0xd);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),(seg_data[2] << 10) | 0xb);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),(seg_data[3] << 10) | 0x7);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(4),get_key_mask_value(0));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(5),get_key_mask_value(1));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(6),get_key_mask_value(2));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(7),get_key_mask_value(3));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(8),get_key_mask_value(4));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(9),get_key_mask_value(5));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(10),get_key_mask_value(6));
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(11),get_key_mask_value(7));
    if (!irq_requested_fp)
    {
        mtos_irq_request(IRQ_LEDKB_ID, scankey_isr, RISE_EDGE_TRIGGER);
        irq_requested_fp = TRUE;
    }
    hal_put_u32((unsigned long *)LEDKB_CTRL,0x12020b09); /*4 led 8 key*/

    return SUCCESS;
}


/**************** GPIO TYPE FP **************/
RET_CODE  kbd_host_init_for_gpio(void)
{
	/*
	*	nothing to be done
	*/
	mtos_printk("kbd_host_init_for_gpio(gpio)...\n");
	if (!irq_requested_gpio)
	{
		mtos_irq_request(IRQ_GPIO_GINT_ID, gpio_scankey_isr, RISE_EDGE_TRIGGER);
		irq_requested_gpio = TRUE;
	}

	return SUCCESS;
}

/**************** GPIO TYPE FP **************/
RET_CODE  kbd_host_init_for_orris_gpio(fp_cfg_t *p_cfg)
{
	u32 gpio_temp=0;
	//u32 gpio_offset=0;
	u32 * tmp_buf = NULL;
	u8 gpio_ret = 0;
	/*
	*	nothing to be done
	*/
	mtos_printk("ledkeyscan_init(gpio)...\n");


	/*
	*	AO_GPIO0 CONFIG
	*/
	/*
	*	0xbf156000  [13:12]=0x10 ,pin8sel
	*/
	gpio_temp = *(volatile u32 *)R_PIN8_BASE_ADDR;
	gpio_temp &= ~(0x3<<12);
	gpio_temp |= (0x2<<12);
	*(volatile u32 *)R_PIN8_BASE_ADDR=gpio_temp;

	/*
	*	0xbf15630c [26]=0x1,ie3sel
	*/
	gpio_temp = *(volatile u32 *)R_IE3_BASE_ADDR;
	gpio_temp |= (0x1<<26);
	*(volatile u32 *)R_IE3_BASE_ADDR=gpio_temp;

	/*
	*	0xbf0a002c=0x000000ff
	*/
	gpio_temp = *(volatile u32 *)R_AO_GPIO_MASK_N;
	gpio_temp |= 0xff;
	*(volatile u32 *)R_AO_GPIO_MASK_N =gpio_temp;

	/*
	*	AO_GPIO1 CONFIG
	*/
	/*
	*	0xbf156000  [9:8]=0x10 ,pin8sel
	*/
	gpio_temp = *(volatile u32 *)R_PIN8_BASE_ADDR;
	gpio_temp &= ~(0x3<<8);
	gpio_temp |= (0x2<<8);
	*(volatile u32 *)R_PIN8_BASE_ADDR=gpio_temp;

	/*
	*	0xbf15630c [25]=0x1,ie3sel
	*/
	gpio_temp = *(volatile u32 *)R_IE3_BASE_ADDR;
	gpio_temp |= (0x1<<25);
	*(volatile u32 *)R_IE3_BASE_ADDR=gpio_temp;

	/*
	*	AO_GPIO2 CONFIG
	*/
	/*
	*	0xbf156000  [5:4]=0x10 ,pin8sel
	*/
	gpio_temp = *(volatile u32 *)R_PIN8_BASE_ADDR;
	gpio_temp &= ~(0x3<<4);
	gpio_temp |= (0x2<<4);
	*(volatile u32 *)R_PIN8_BASE_ADDR=gpio_temp;

	/*
	*	0xbf15630c [24]=0x1,ie3sel
	*/
	gpio_temp = *(volatile u32 *)R_IE3_BASE_ADDR;
	gpio_temp |= (0x1<<24);
	*(volatile u32 *)R_IE3_BASE_ADDR=gpio_temp;

	/*
	*	create get key task
	*/
	tmp_buf = (u32 *)mtos_malloc(p_cfg->fp_op_stksize);
	memset(tmp_buf,0,p_cfg->fp_op_stksize);
	gpio_ret=mtos_task_create((u8 *)"orris get key", orris_gpio_scankey, (void*)NULL, p_cfg->fp_op_pri,tmp_buf,p_cfg->fp_op_stksize);
	MT_ASSERT(TRUE == gpio_ret);
	return SUCCESS;
}


RET_CODE  kbd_SetEnableKeyConcerto(u8 enable_key)
{
  u32 tmp1 = 0;
  u32 tmp2 = enable_key;
  tmp1 =  hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT);
  tmp1 = (tmp1 & 0xfffff00f)|(tmp2 << 4);
  hal_put_u32((unsigned long *)LEDKB_INTMASK_KEYFILT,tmp1);
  FP_PRINTF("LEDKB_INTMASK_KEYFILT=0x%08x\n",
  hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT));
  return 0;
}

RET_CODE  kbd_GetEnableKeyConcerto(u8 *pEnable_key)
{
  u32 tmp1 = 0;
  if(pEnable_key == NULL) return ERR_FAILURE;
  tmp1 =  hal_get_u32((unsigned long *)LEDKB_INTMASK_KEYFILT);
  *pEnable_key = (u8)((tmp1 >> 4)& 0xff);
  return 0;
}

RET_CODE  kbd_host_init_for_lm8168(void)
{
    u32 seg_data[4] = {0};

    FP_PRINTF("kbd_host_init_for_lm8168...\n");
    seg_data[0] = 0xff;
    seg_data[1] = 0xff;
    seg_data[2] = 0xff;
    seg_data[3] = 0xff;

    hal_put_u32((unsigned long *)LEDKB_CLK_PARA,0xa00000);
    hal_put_u32((unsigned long *)LEDKB_DEN_PARA,0x400000);
    hal_put_u32((unsigned long *)LEDKB_DAT_DELAY_NUM,1);
    hal_put_u32((unsigned long *)LEDKB_INTMASK_KEYFILT,0x00200ff0);
    SetHostFreq(200*FREQ_KHZ);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),0xa00000|(seg_data[0]<<10)|0x04);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),0xa00000|(seg_data[1]<<10)|0x40);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),0xa00000|(seg_data[2]<<10)|0x20);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),0xa00000|(seg_data[3]<<10)|0x10);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(4),0xa00400);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(5),0xa00800);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(6),0xa01000);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(7),0xa02000);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(8),0xa04000);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(9),0xa08000);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(10),0xa10000);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(11),0xa20000);
    if (!irq_requested_fp)
    {
        mtos_irq_request(IRQ_LEDKB_ID, scankey_isr, RISE_EDGE_TRIGGER);
        irq_requested_fp = TRUE;
    }
    hal_put_u32((unsigned long *)LEDKB_CTRL,0x12020b39); /*4 led 8 key*/

    return SUCCESS;
}

RET_CODE  Set_DataPort_watch_scope(u32 data_port)
{
    FP_PRINTF("Set DataPort for Watch scope\n");
    hal_put_u32((unsigned long *)LEDKB_TIMESET,0x3a80020);//
    hal_put_u32((unsigned long *)LEDKB_TIMESEL,0x3b000000);//set clk at 200khz select time 1
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),data_port);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(4),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(5),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(6),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(7),data_port);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(8),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(9),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(10),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(11),data_port);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(12),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(13),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(14),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(15),data_port);

    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(16),data_port);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(17),data_port);

    //hal_put_u32((unsigned long *)LEDKB_CTRL,0x14061109);
    return SUCCESS;
}

RET_CODE  Set_to_watch_delay_ct1642_wave(void)
{
    FP_PRINTF("Set_to_watch_delay_ct1642_wave \n");

  //config display data
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),(0x55 << 10) | 0xe);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),(0x55 << 10) | 0xd);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),(0x55 << 10) | 0xb);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),(0x55 << 10) | 0x7);
    //config keyscan data
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(4),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(5),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(6),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(7),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(8),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(9),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(10),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(11),(0xaa << 10) | 0xf);
    return SUCCESS;
}

RET_CODE  Set_to_watch_delay_lm8168_wave(void)
{
    FP_PRINTF("Set_to_watch_delay_lm8168_wave \n");

    hal_put_u32((unsigned long *)LEDKB_DEN_PARA,0xeeeaaaaa);

  //config display data
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),(0x55 << 10) | 0x4);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),(0x55 << 10) | 0x40);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),(0x55 << 10) | 0x20);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),(0x55 << 10) | 0x10);
    //config keyscan data
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(4),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(5),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(6),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(7),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(8),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(9),(0xaa << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(10),(0x55 << 10) | 0xf);
    hal_put_u32((unsigned long *)LEDKB_DAT_PARA(11),(0xaa << 10) | 0xf);

    return SUCCESS;
}

RET_CODE  SetAllDataPort(u32 port_data)
{
    u32  i = 0;

    FP_PRINTF("SetAllDataPort to %08x \n",port_data);
    for(i = 0;i < 18;i++)
      hal_put_u32((unsigned long *)LEDKB_DAT_PARA(i),port_data);
    return SUCCESS;
}

static u32 revert_bit(u8 code)
{
  u32 bit_value = 0;

  if(code&(1 << 0))
      bit_value |=0x80;
  if(code&(1 << 1))
      bit_value |=0x40;
  if(code&(1 << 2))
      bit_value |=0x20;
  if(code&(1 << 3))
      bit_value |=0x10;
  if(code&(1 << 4))
      bit_value |=0x08;
  if(code&(1 << 5))
      bit_value |=0x04;
  if(code&(1 << 6))
      bit_value |=0x02;
  if(code&(1 << 7))
      bit_value |=0x01;
  FP_PRINTF("revert_bit %02x  -> %08x\n",code,bit_value);
  return bit_value;
}

static u8 fd650_read(void *p_bus_dev) //read the key value
{
  u8 keycode = 0;

  if(p_bus_dev != NULL)
  {
      i2c_raw_write_byte(p_bus_dev,((FD650_GET_KEY >> 7) & 0x3E) | 0x01 | 0x40, 1);
      i2c_raw_read_byte(p_bus_dev, &keycode, 1);
      i2c_raw_stop(p_bus_dev);
  }

  if((keycode & 0x40) == 0)
  {
    keycode = 0;
  }

  return keycode;
}

static void fd650_write(void *p_bus_dev, u16 cmd) //write command
{
  if(p_bus_dev != NULL)
  {
      i2c_raw_write_byte(p_bus_dev, ((u8)(cmd >> 7) & 0x3E) | 0x40, 1);
      i2c_raw_write_byte(p_bus_dev, (u8)cmd, 0);
      i2c_raw_stop(p_bus_dev);
  }
}

static RET_CODE fd650_display(fp_priv_t *p_fp, u8 *p_data, u32 len)
{
  int i = 0, j = 0;
  u8 tmp[FP_MAX_LED_NUM] = {0};
  u8 dot[FP_MAX_LED_NUM] = {0};

  memset(tmp, 0x20, sizeof(tmp)); //space ascii value == 0x20

  for(i = 0; i < len; i++)
  {
    if ((p_data[i] == '.') && (i > 0))
    {
      if (p_data[i - 1] != '.')
      {
        dot[j - 1] = FD650_DOT;
      }
    }
    else
      tmp[j++] = p_data[i];
  }

  for (i = 0; i < FP_MAX_LED_NUM; i++)
  {
    for(j = 0; j < p_fp->config.map_size; j++)
    {
      if(p_fp->config.p_map[j].ch == tmp[i])
      {
        p_fp->dis_buff[i] = p_fp->config.p_map[j].bitmap;
      }
    }
  }
	fd650_write(p_fp->config.p_bus_dev, FD650_SYSON_4 | FD650_8SEG_ON);
  fd650_write(p_fp->config.p_bus_dev, FD650_DIG0 | p_fp->dis_buff[0] | dot[0]);
  fd650_write(p_fp->config.p_bus_dev, FD650_DIG1 | p_fp->dis_buff[1] | dot[1]);
  fd650_write(p_fp->config.p_bus_dev, FD650_DIG2 | p_fp->dis_buff[2] | dot[2]);
  fd650_write(p_fp->config.p_bus_dev, FD650_DIG3 | p_fp->dis_buff[3] | dot[3]);

  return SUCCESS;
}

/******************************************************
 *   External APIs
 ******************************************************/
static RET_CODE panel_concerto_display(lld_uio_t *p_lld,u8 *p_data, u32 len)
{
    fp_priv_t *p_fp = (fp_priv_t *)p_lld->p_fp;
    u32 i = 0, size = len;
    u32 bitmap[4] = {0,0,0,0};

    if(size > FP_MAX_LED_NUM)
    {
        size = FP_MAX_LED_NUM;
    }

    for(i = 0; i < size; i++)
    {
        bitmap[i] = fp_find_bitmap(&((fp_priv_t *)p_lld->p_fp)->config, p_data[i]);
    }
    if (p_fp->config.fp_type == HAL_CT1642)
    {
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),(bitmap[0]<<10)|0xe);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),(bitmap[1]<<10)|0xd);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),(bitmap[2]<<10)|0xb);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),(bitmap[3]<<10)|0x7);
    }
    else if(p_fp->config.fp_type == HAL_LM8168)
    {
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(0),0xa00000|(revert_bit(bitmap[0])<<10)|0x04);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(1),0xa00000|(revert_bit(bitmap[1])<<10)|0x40);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(2),0xa00000|(revert_bit(bitmap[2])<<10)|0x20);
        hal_put_u32((unsigned long *)LEDKB_DAT_PARA(3),0xa00000|(revert_bit(bitmap[3])<<10)|0x10);
    }
    else if (p_fp->config.fp_type == HAL_FD650)
    {
        fd650_display(p_fp, p_data, len);
    }

    return SUCCESS;
}

static u8 hk_gpio_Read( pan_gpio_info_t * com_info)
{
	u32 gpio_offset=0;	

	/*
	*	AO_GPIO READ
	*/
	 if((com_info->pos>99)&&(com_info->pos<108))  //AO_GPIO
	 {
	 	gpio_offset=com_info->pos -100;
		#if 0
		mtos_printk("concerto_gpio_Read com_info->pos = %d ,R_AO_GPIO_RDATA = 0x%x ,(0x1<<gpio_offset) = 0x%x \n",\
							gpio_offset,*(volatile u32 *)R_AO_GPIO_RDATA,(0x1<<gpio_offset));
		mtos_printk("((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<gpio_offset)) = 0x%x\n",((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<gpio_offset)));
		#endif
		if(!(((*(volatile u32 *)R_AO_GPIO_RDATA)&(0x1<<gpio_offset))>>gpio_offset))
		{			
			return 1;
		}
		else
		{
			return 0;
		}
	 }
	
	 return 0xff;
}

/******************************************************
 *   Internal functions
 ******************************************************/
static void gpio_key_timer_cb(void)
{
  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *)p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv; 
  u32 key_data1 = 0;
  u32 frontkey_value = 0;
  u32 i =0;
  
    for(i=0;i<g_gpio_p_info.num_com;i++)
   {
   	if(hk_gpio_Read(&(g_gpio_p_info.com[i])))
   	{
   		frontkey_value = 0x100+i;
   		key_data1 = frontkey_value;
   		mtos_fifo_put(&p_priv->fifo, key_data1);   		
   		break;
   	}
   }
}


static RET_CODE panel_concerto_open(lld_uio_t *p_lld, fp_cfg_t *p_cfg)
{
  fp_priv_t *p_panel = NULL;
  u8 i;

  if(NULL != p_lld->p_fp)
  {
    return SUCCESS;
  }
  p_panel = (fp_priv_t *)mtos_malloc(sizeof(fp_priv_t));
  MT_ASSERT(NULL != p_panel);
  memset(p_panel, 0, sizeof(fp_priv_t));
  p_lld->p_fp = p_panel;

  if(NULL == p_cfg->p_map)
  {
    p_panel->config.map_size = FP_TABLE_SIZE;
    p_panel->config.p_map = fp_bitmap;
  }
  else
  {
    p_panel->config.map_size = p_cfg->map_size;
    p_panel->config.p_map = p_cfg->p_map;
  }

  p_panel->config.fp_type = p_cfg->fp_type;

  //mtos_printk("p_panel->config.fp_type = %d\n",p_panel->config.fp_type);
  if(p_panel->config.fp_type == HAL_HK)
  {
	 /* register a timer for repeate keys */
	p_panel->timer_id =
	hal_timer_request(HK_SCAN_TIME, gpio_key_timer_cb, TRUE);
	//mtos_printk("hk request timer id: %d\n", p_panel->timer_id);
  }
  else
  {
	/* register a timer for repeate keys */
	p_panel->timer_id =
	hal_timer_request(FP_RPT_KEY_SCAN_TIME, fp_key_timer_cb, TRUE);
	//mtos_printk("fp request timer id: %d\n", p_panel->timer_id);
  }
 
  
  if (p_panel->config.fp_type == HAL_CT1642)
  {
    kbd_host_init_for_ct1642();
  }
  else if (p_panel->config.fp_type == HAL_LM8168)
  {
    kbd_host_init_for_lm8168();
  }
  else if (p_panel->config.fp_type == HAL_FD650)
  {
	/*
	*	bind gpio to cmd
	*/
	if(p_cfg->p_info!=NULL)
	{
		g_lbd_gpio_num[0] = p_cfg->p_info->lbd[0].pos;
		g_lbd_gpio_num[1] = p_cfg->p_info->lbd[1].pos;
		g_lbd_gpio_num[2] = p_cfg->p_info->lbd[2].pos;
		g_lbd_gpio_num[3] = p_cfg->p_info->lbd[3].pos;
	}
	p_panel->config.p_bus_dev = p_cfg->p_bus_dev;
	hal_timer_start(p_panel->timer_id);
	fd650_write(p_panel->config.p_bus_dev, FD650_SYSON_4 | FD650_8SEG_ON);
  }
  else if (p_panel->config.fp_type == HAL_GPIO)
  {
  	memcpy((void* )(&g_gpio_p_info),p_cfg->p_info,sizeof(pan_hw_info_t));
	/*
	*	bind gpio to cmd
	*/
	g_lbd_gpio_num[0] = p_cfg->p_info->lbd[0].pos;
	g_lbd_gpio_num[1] = p_cfg->p_info->lbd[1].pos;
	g_lbd_gpio_num[2] = p_cfg->p_info->lbd[2].pos;
	g_lbd_gpio_num[3] = p_cfg->p_info->lbd[3].pos;

  	/****** gpio config ******/
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();

	*((volatile u32 *)R_GPIO1_MODE)=0xffffffff;
	*((volatile u32 *)R_GPIO0_MODE)=0xffffffff;
	*((volatile u32 *)R_AO_GPIO_MODE)=0xff;
				
  	for(i=0;i<p_cfg->p_info->num_com;i++)
	{
		concerto_gpio_config(p_cfg->p_info->com[i]);
	}
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();
	kbd_host_init_for_gpio();
  }
  else if (p_panel->config.fp_type == HAL_HK)
  {	
  	memcpy((void* )(&g_gpio_p_info),p_cfg->p_info,sizeof(pan_hw_info_t));
	/*
	*	bind gpio to cmd
	*/
	g_lbd_gpio_num[0] = p_cfg->p_info->lbd[0].pos;
	g_lbd_gpio_num[1] = p_cfg->p_info->lbd[1].pos;
	g_lbd_gpio_num[2] = p_cfg->p_info->lbd[2].pos;
	g_lbd_gpio_num[3] = p_cfg->p_info->lbd[3].pos;

  	/****** gpio config ******/
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();

	*((volatile u32 *)R_GPIO1_MODE)=0xffffffff;
	*((volatile u32 *)R_GPIO0_MODE)=0xffffffff;
	*((volatile u32 *)R_AO_GPIO_MODE)=0xff;
				
  	for(i=0;i<p_cfg->p_info->num_com;i++)
	{
		concerto_gpio_config(p_cfg->p_info->com[i]);
	}
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();	
	hal_timer_start(p_panel->timer_id);
  }
  else if (p_panel->config.fp_type == HAL_ORRIS)
  {
  	memcpy((void* )(&g_gpio_p_info),p_cfg->p_info,sizeof(pan_hw_info_t));

	mtos_printk("p_cfg->p_info->lbd[0].pos = 0x%x\n",p_cfg->p_info->lbd[0].pos);
	/*
	*	bind gpio to cmd
	*/
	g_lbd_gpio_num[0] = p_cfg->p_info->lbd[0].pos;
	g_lbd_gpio_num[1] = p_cfg->p_info->lbd[1].pos;
	g_lbd_gpio_num[2] = p_cfg->p_info->lbd[2].pos;
	g_lbd_gpio_num[3] = p_cfg->p_info->lbd[3].pos;

  	kbd_host_init_for_orris_gpio(p_cfg);
  }
  else if (p_panel->config.fp_type == HAL_GOSPELL)
  {
  	gospell_shake_remove_flag = 1;
  	memcpy((void* )(&g_gpio_p_info),p_cfg->p_info,sizeof(pan_hw_info_t));
	
	/*
	*	bind gpio to cmd
	*/
	g_lbd_gpio_num[0] = p_cfg->p_info->lbd[0].pos;
	g_lbd_gpio_num[1] = p_cfg->p_info->lbd[1].pos;
	g_lbd_gpio_num[2] = p_cfg->p_info->lbd[2].pos;
	g_lbd_gpio_num[3] = p_cfg->p_info->lbd[3].pos;

  	/****** gpio config ******/
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();
	
	*((volatile u32 *)R_GPIO1_MODE)=0xffffffff;
	*((volatile u32 *)R_GPIO0_MODE)=0xffffffff;
	*((volatile u32 *)R_AO_GPIO_MODE)=0xff;
		
  	for(i=0;i<p_cfg->p_info->num_com;i++)
	{
		concerto_gpio_config(p_cfg->p_info->com[i]);
	}
	gpio_ao_gpio_clr();
	gpio_gpio0_clr();
	gpio_gpio1_clr();
	kbd_host_init_for_gpio();
  }

  return SUCCESS;
}

static RET_CODE panel_concerto_stop(lld_uio_t *p_lld)
{
  fp_priv_t *p_fp = (fp_priv_t *)p_lld->p_fp;
  hal_timer_release(p_fp->timer_id);
  //mtos_printk("panel_concerto_stop>>>\n");
  if (irq_requested_gpio)
  {
    //mtos_irq_release(IRQ_GPIO_GINT_ID, gpio_scankey_isr);
    //irq_requested_gpio = FALSE;
  }
  if (irq_requested_fp)
  {
    mtos_irq_release(IRQ_LEDKB_ID, scankey_isr);
    irq_requested_fp = FALSE;
  }
  if(NULL != p_lld->p_fp)
  {
    mtos_free(p_lld->p_fp);
    p_lld->p_fp = NULL;
  }
  return SUCCESS;
}

static RET_CODE panel_concerto_io_ctrl(lld_uio_t *p_lld,u32 cmd, u32 param)
{
	switch(cmd)
	{
		case UIO_FP_SET_POWER_LBD:
			gpio_display(g_lbd_gpio_num[0]|(param<<7));
			break;
		case UIO_FP_SET_LOCK_LBD:
			gpio_display(g_lbd_gpio_num[1]|(param<<7));
			break;
		case UIO_FP_SET_COLON:
			gpio_display(g_lbd_gpio_num[2]|(param<<7));
			break;
		case GPIO_FP_SET_RESERVE:
			gpio_display(g_lbd_gpio_num[3]|(param<<7));
			break;
		default:
			break;
	}
  	return ERR_NOFEATURE;
}

void panel_concerto_attach(lld_uio_t *p_lld)
{
  p_lld->fp_open = panel_concerto_open;
  p_lld->fp_stop = panel_concerto_stop;
  p_lld->fp_io_ctrl = panel_concerto_io_ctrl;
  p_lld->display = panel_concerto_display;
}
