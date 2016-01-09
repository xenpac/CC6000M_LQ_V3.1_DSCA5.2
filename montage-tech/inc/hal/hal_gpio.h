/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_GPIO_H_
#define __HAL_GPIO_H_

/*!
  Max number of GPIO PIN
  */
#define GPIO_MAX_PIN                     36

/*!
  Invalid number of GPIO PIN
  */
#define GPIO_INVALID_PIN                 63


#ifndef ENSEMBLE
/*!
  Max number of GPIO PIN for warriors
  */
#define GPIO_MAX_PIN_WA                     63
#else
/*!
  Max number of GPIO PIN for warriors
  */
#define GPIO_MAX_PIN_WA                     24
#endif
/*!
  Invalid number of GPIO PIN //now not use 63,
  beacuse it may affect front panel parameter
  */
#define GPIO_INVALID_PIN_WA                 63


/*!
  GPIO direction, output
  */
#define GPIO_DIR_OUTPUT                   0x0
/*!
  GPIO direction, input
  */
#define GPIO_DIR_INPUT                    0x1

/*!
  GPIO output value, low
  */
#define GPIO_LEVEL_LOW                    0x0
/*!
  GPIO output value, high
  */
#define GPIO_LEVEL_HIGH                   0x1

/*!
  GPIO pin number value
  */
#define GPIO_PIN_NUM_1                1 

/*!
  GPIO pin number value,for lock
  */
#define GPIO_PIN_NUM_2                2 
/*!
  GPIO pin number value,for standby
  */
#define GPIO_PIN_NUM_3                3    

/*!
  GPIO pin number value
  */
#define GPIO_PIN_NUM_4               4 

/*!
  GPIO pin number value
  */
#define GPIO_PIN_NUM_5               5
/*!
  GPIO pin number value
  */
#define GPIO_PIN_NUM_6               6
/*!
  GPIO pin number value
  */
#define GPIO_PIN_NUM_7               7

/*!
  GPIO io control command
  */
typedef enum 
{
  /*!
    Enable/disable specific GPIO, param1: GPIO pin number, param2: TRUE-enable, FALSE-disable
    */
  GPIO_CMD_IO_ENABLE,
  GPIO_CMD_IO_INTRRUPT,
  GPIO_CMD_IO_INT_RELEASE,
  GPIO_FP_SET_LBD,
}gpio_io_cmd_t;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//              APIs
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
  initiate the GPIO interfaces

  \param[in] None 
  */
void gpio_init(void);

/*!
  GPIO mux pin selection

  \param[in] GPIO pin number
  \param[in] selected index
  \Return Return SUCCESS if succeeded, else return fail
 */
s32 gpio_pin_sel(u32 GPIOPin, u8 SelIndex);

/*!
  Enable/disable specific gpio pin

  \param[in] gpiopin Pin number
  \param[in] enable TRUE:enable, FALSE:disable
  \Return Return SUCCESS if success, else if fail
 */
s32 gpio_io_enable(u32 GPIOPin, u32 enable);

/*!
  Set GPIO direction, input or output
  
  \param[in] GPIOPin GPIO Pin number
  \param[in] dir GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
  */
void gpio_set_dir(u32 GPIOPin, u8 dir);

/*!
  Set GPIO output value, high or low
  
  \param[in] GPIOPin GPIO Pin number
  \param[in] val output value, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH
  */
void gpio_set_value(u32 GPIOPin , u8 val);

/*!
  Get GPIO direction
  
  \param[in] GPIOPin GPIO Pin number
  \param[out] p_dir return GPIO direction, GPIO_DIR_OUTPUT or GPIO_DIR_INPUT
  */
void gpio_get_dir(u32 GPIOPin , u8 *p_dir);

/*!
  Get GPIO value for input GPIO
  
  \param[in] GPIOPin GPIO Pin number
  \param[out] p_val return GPIO value, GPIO_LEVEL_LOW or GPIO_LEVEL_HIGH
  */
void gpio_get_value(u32 GPIOPin , u8 *p_val);

/*!
  GPIO io command handler
  
  \param[in] cmd the GPIO command to process
  \param[in] param1 the first parameter
  \param[in] param2 the second parameter
  
  \return SUCCESS if cmd is excuted successfully, else fail
  */
s32 gpio_ioctl(gpio_io_cmd_t cmd, u32 param1, u32 param2);

#endif //__HAL_GPIO_H_
