/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __HAL_UART_H_
#define __HAL_UART_H_

/*!
  The max UART number supported
  */
#define UART_MAX_NUM          2

/*!
  The fifo buffer size for UART
  */
#define UART_BUF_SIZE         512

/*!
  Suspend UART untill the data is received
  */
#define UART_WAIT_FOREVER    0xFFFFFFFF
/*!
  The UART ID 
  */
enum UART_ID 
{
  /*!
    The first UART
    */
  UART_ID_0 = 0, 
  /*!
    The second UART
    */
  UART_ID_1    
};

/*!
  The UART commands supported by io control
  */
enum UART_CMD
{
  /*!
    Enable/disable interrupt mode of receiving data, 
    param1: TRUE-enable interrupt mode, FALSE-disable interrupt mode, enable polling mode
    */
  UART_CMD_EN_INTERRUPT = 0, 
 /*!
  */
  UART_CMD_EN_TX_INTRRUPT,
 /*!
  */
  UART_CMD_EN_RX_THR_FIFO,
 /*!
  */
  UART_CMD_EN_TX_THR_FIFO,
 /*!
  */
  UART_CMD_EN_PARITY_ERROR_INTERRUPT,
  /*!
  */
  UART_CMD_EN_RX_OVERFLOW_INTERRUPT,
  /*!
  */
  UART_CMD_EN_TONE_INTERRUPT,
  /*!
  */
  UART_CMD_GET_COUNT,

 /*!
  */
  UART_CMD_GET_RX_COUNT
}; 


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//              APIs
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/*!
  initiate UART
  
  \param[in] num the UART number to support

   \return SUCCESS if OK, else fail
  */
s32 uart_init(u8 num);

/*!
  Flush the uart hardware data, and clean the software FIFO
  
  \param[in] id the uart id, refer to UART_ID 
  */
void uart_flush(u8 id);

/*!
  Flush the uart hardware data, and clean the software FIFO
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] baudrate the UART baud rate  
  \param[in] databits not available now 
  \param[in] stopbits not available now
  \param[in] parity not available now
  */
void uart_set_param(u8 id,u32 baudrate, u8 databits, u8 stopbits, u8 parity);
/*!

  only use for the abv uart ca
  
  Flush the uart hardware data, and clean the software FIFO
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] baudrate the UART baud rate  
  \param[in] databits not available now 
  \param[in] stopbits not available now
  \param[in] parity not available now
  */
void uart_set_param_ca(u8 id,u32 baudrate, u8 databits, u8 stopbits, u8 parity);

/*!
  Send out a byte
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] chval the byte to send out

  \return SUCCESS
  */
s32 uart_write_byte(u8 id,u8 chval);

/*!
  Send out data sequence
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] p_data the pointer to sending data
  \param[in] len the length to send

  \return SUCCESS
  */
s32 uart_write(u8 id,u8 *p_data, u32 len);

/*!
  only use for ABV uart ca
  Send out data sequence
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] p_data the pointer to sending data
  \param[in] len the length to send

  \return SUCCESS
  */
s32 uart_write_ca(u8 id,u8 *p_data, u32 len);

/*!
  Send out data sequence
  
  \param[in] id the uart id, refer to UART_ID 
  \param[in] p_data the pointer to sending data
  \param[in] len the length to send

  \return SUCCESS
  */
s32 uart_write_dma(u8 id,u8 *p_data, u32 len);
/*!
  read an UART byte by interrupt mode(default), return fail if no data is available untill timeout
  
  \param[in] id the uart id, refer to UART_ID 
  \param[out] p_data store the data got
  \param[in] timeout the timeout value in ms

  \return
      SUCCESS: data got
      ERR_TIMEOUT: timeout
      else: fail
  */
s32 uart_read_byte(u8 id,u8 *p_data, u32 timeout);
/*!
  read an UART byte by interrupt mode(default), return fail if no data is available untill timeout
  
  \param[in] id the uart id, refer to UART_ID 
  \param[out] p_data store the data got
  \param[in] timeout the timeout value in ms

  \return
      SUCCESS: data got
      ERR_TIMEOUT: timeout
      else: fail
  */
s32 uart_read_dma(u8 id, u8 *p_data, u32 len);
/*!
  read an UART byte in polling mode. CALL UART_CMD_EN_INTERRUPT command to disable interrupt mode first
  
  \param[in] id the uart id, refer to UART_ID 
  \param[out] p_data store the received byte 
  \param[in] timeout the timeout value in ms

  \return
      SUCCESS: data got
      ERR_TIMEOUT: timeout
      else: fail
  */
s32 uart_read_byte_polling(u8 id,u8 *p_data, u32 timeout);

/*!
  UART io control commands
  
  \param[in] cmd refer to UART_CMD
  \param[in] param1 refer to UART_CMD
  \param[in] param2 refer to UART_CMD

  \return SUCCESS, else fail
  */
s32 uart_ioctl(u8 cmd, u32 param1, u32 param2);

#endif //end of __HAL_UART_H_
