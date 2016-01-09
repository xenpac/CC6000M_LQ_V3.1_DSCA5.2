#ifndef __RW_REG_H__
#define __RW_REG_H__

/*!
  xxxx
  */
int   init_rw_reg();

/*!
  xxxx
  */
void  deinit_rw_reg();


/*!
  Write 8 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
void reg_put_u8(volatile unsigned char *p_addr, unsigned char  data);


/*!
  Write 16 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
void reg_put_u16(volatile unsigned short *p_addr, unsigned short data);


/*!
  Write 32 bits register
  
  \param[in] addr register address
  \param[in] p_addr data to write
  */
void reg_put_u32(volatile unsigned long *p_addr, unsigned long data);


/*!
  Get 8 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
unsigned char reg_get_u8(volatile unsigned char *p_addr);


/*!
  Get 16 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
unsigned short reg_get_u16(volatile unsigned short *p_addr);


/*!
  Get 32 bits register value
  
  \param[in] p_addr register address

  \return register value
  */
unsigned long reg_get_u32(volatile unsigned long *p_addr);

#endif


