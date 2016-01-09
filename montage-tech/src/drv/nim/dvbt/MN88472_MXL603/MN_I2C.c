/* **************************************************** */
/*!
   @file	MN_DMD_I2C_WIN.c
   @brief	I2C communication wrapper
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "drv_dev.h"
#include "mtos_printk.h"
#include "../inc/drv/bus/i2c.h"
#include "MN_DMD_driver.h"
#include "MN_I2C.h"
#include "MN_DMD_common.h"
/* **************************************************** */
/* **************************************************** */
extern os_sem_t g_mn_mxl_i2c_rw_mutex;

static i2c_bus_t *s_mn_mxl_dmd_dev = NULL;

#ifndef PC_CONTROL_FE

#define DMD_I2C_MAXSIZE	127
/*! I2C Initialize Function*/
DMD_ERROR_t DMD_API DMD_I2C_open(void)
{
	//TODO:	Please call I2C initialize API here
	//this function is called by DMD_API_open
  
	return DMD_E_OK;
}
void DMD_API DMD_I2C_Init(void *p_i2c)
{
  s_mn_mxl_dmd_dev = (i2c_bus_t *)p_i2c;
}

/*! Write 1byte */
DMD_ERROR_t DMD_I2C_Write(DMD_u8_t	slvadr , DMD_u8_t adr , DMD_u8_t data)
{
	//TODO:	Please call I2C 1byte Write API
  RET_CODE ret;
  u8 buf[2];

  buf[0] = adr;
  buf[1] = data;
  mtos_sem_take(&g_mn_mxl_i2c_rw_mutex, 0);
  ret = i2c_write(s_mn_mxl_dmd_dev, slvadr, buf, 2, I2C_PARAM_DEV_SOC_INTER);
  mtos_sem_give(&g_mn_mxl_i2c_rw_mutex);
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	DMD_I2C_Write FAILED!\n");
  	return DMD_E_ERROR;
  }
  
	return DMD_E_OK;
}

/*! Read 1byte */
DMD_ERROR_t DMD_I2C_Read(DMD_u8_t	slvadr , DMD_u8_t adr , DMD_u8_t *data )
{
	//TODO:	call I2C 1byte Write API
  RET_CODE ret;
	//u8 buf[2] = {0};

	*data = adr;
  mtos_sem_take(&g_mn_mxl_i2c_rw_mutex, 0);
  ret = i2c_std_read(s_mn_mxl_dmd_dev, slvadr, data, 1, 1, I2C_PARAM_DEV_SOC_INTER);
  mtos_sem_give(&g_mn_mxl_i2c_rw_mutex);
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	DMD_I2C_Read reg %x FAILED!\n", *data);
  	return DMD_E_ERROR;
  }
  return DMD_E_OK;
}

/* 
Write/Read any Length to DMD and Tuner;
DMD_u8_t slvadr : DMD's slave address.   
DMD_u8_t adr    : DMD's REG address; 

For writing CMD to tuner, 
DMD_u8_t slvadr is DMD's slave address;
DMD_u8_t adr is DMD's register TCBCOM(0xF7)
!!!Tuner's slave addr. and REG addr. are as continous data< upper layer : data[x]> 
*/
DMD_ERROR_t DMD_I2C_Write_Anylenth(DMD_u8_t	slvadr , DMD_u8_t adr , DMD_u8_t* wdata , DMD_u32_t wlen)
{

	//TODO:	Please call I2C Read/Write API here
  RET_CODE ret;
  u32 i = 0;
  u8 *p_buf = (u8 *)mtos_malloc(wlen + 1);

  p_buf[0] = adr;
  for( i = 0; i < wlen; i++)
  {
    p_buf[i+1] = wdata[i];
  }
  mtos_sem_take(&g_mn_mxl_i2c_rw_mutex, 0);
  ret = i2c_write(s_mn_mxl_dmd_dev, slvadr, p_buf, wlen + 1, I2C_PARAM_DEV_SOC_INTER);
  mtos_sem_give(&g_mn_mxl_i2c_rw_mutex);
  mtos_free(p_buf);
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:DMD_I2C_Write_Anylenth FAILED!\n");
  	return DMD_E_ERROR;
  }
  
	return DMD_E_OK;
}

/*
twsbGetData_MN88472_TCB(slvadr, adr, wdata)
This is specific function for MN88472 to read Tuner reg's data.
★ If you use MN88472 to control Tuner, you may need to create new I2C bottom code.

There are two steps to read Tuner, take Mxl603 for example. (See Tuner I2C read protocal as below)
Step 1 : Tell Tuner which Reg to read 
      -> Pls. call function DMD_I2C_Write_Anylenth()
Step 2 : Read Tuner Reg's data
      -> Pls. call function DMD_TCBI2C_Read()
------ Tuner Mxl603 I2C read protocal -------
// 8 bit Register Read Protocol:
  Step1
  // +------+-+-----+-+-+----+-+----------+-+-+
  // |MASTER|S|SADDR|W| |0xFB| |RegAddr   | |P|
  // +------+-+-----+-+-+----+-+----------+-+-+
  // |SLAVE |         |A|    |A|          |A| |
  // +------+-+-----+-+-+----+-+----------+-+-+
  Step2
  // +------+-+-----+-+-+-----+--+-+
  // |MASTER|S|SADDR|R| |     |MN|P|
  // +------+-+-----+-+-+-----+--+-+
  // |SLAVE |         |A|Data |  | |
  // +------+---------+-+-----+--+-+
  // SADDR(I2c slave address), S(Start condition), 
  // A(Slave Ack), MN(Master NACK),  P(Stop condition)

// [ Pls pay attention here ! This is what customer need complete ]
------ Step1 I2C commnication between BE and DMD -------
软件实现 : 将 SADDR-M , 0xFB 和 R-addr-M 当做三个数据写给DMD，DMD将此三个数据转发给Tuner
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----
  // |BE |S|SADDR|W| |R-addr| |SADDR-M|W| |0xFB| |R-addr-M| |P|
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----
  // |DMD|         |A|      |A|         |A|    |A|        |A|    
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----
  SADDR(DMD's slave address), R-addr(DMD's reg addr. -> TCBCOM ),
  SADDR-M(Tuner's slave address)
  R-addr-M(tell Tuner which reg address to read out)
    
------ Step2 I2C commnication between BE and DMD -------
软件实现 : 请按照 "BE" 列的流程修改 BE I2C 底层函数 
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----
  // |BE |S|SADDR|W| |R-addr| |SADDR-M|R| |S|SADDR|R| |      |MN|P|
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----
  // |DMD|         |A|      |A|         |A|         |A| Data |  | |
  // +------+-+-----+-+-+----+-+----------+-+-++------+-+-----+-+-+----

SADDR(DMD's slave address), R-addr(DMD's reg addr. -> TCBCOM「0xF7」 ),
SADDR-M(Tuner's slave address)

  You can also refer to file "PRODUCT_SPECIFICATIONS_MN88472_ver041_120120.pdf"
  page 34 to see the TCB control flow.
*/
DMD_ERROR_t DMD_TCBI2C_Read(DMD_u8_t slvadr , DMD_u8_t adr , DMD_u8_t* wdata , DMD_u32_t wlen , DMD_u8_t* rdata , DMD_u32_t rlen)
{
  RET_CODE ret;

  mtos_sem_take(&g_mn_mxl_i2c_rw_mutex, 0);
  rdata[0] = adr;
  rdata[1] = *wdata;
  ret = i2c_std_read(s_mn_mxl_dmd_dev, slvadr, rdata, 2, rlen, I2C_PARAM_DEV_SOC_INTER);
  mtos_sem_give(&g_mn_mxl_i2c_rw_mutex);
  if (ret < 0)
  {
    OS_PRINTF("DMD_TCBI2C_Read reg %x Failure!!!\n", adr);
    return DMD_E_ERROR;
  }	
  return DMD_E_OK;
}

/*! Write/Read any Length*/
// This is general API for you to communicate with external device which DIRECTLY connect to BE,
// However, to communicate with Tuner through DMD MN88472, you may need to modify Bottom code of I2C sequence. 
DMD_ERROR_t DMD_I2C_WriteRead(DMD_u8_t	slvadr , DMD_u8_t adr , DMD_u8_t* wdata , DMD_u32_t wlen , DMD_u8_t* rdata , DMD_u32_t rlen)
{
	//TODO:	Please call I2C Read/Write API here
	return DMD_E_OK;
}

/*! Timer wait */
DMD_ERROR_t DMD_wait( DMD_u32_t msecond )
{

	//TODO: call timer wait function 
  mtos_task_delay_ms(msecond);
	return DMD_E_OK;
}

/*! Get System Time (ms) */
DMD_ERROR_t DMD_timer( DMD_u32_t* tim )
{


	return DMD_E_OK;
}

#endif //#ifndef PC_CONTROL_FE
