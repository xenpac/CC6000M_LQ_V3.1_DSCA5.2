#include "sys_types.h"
#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "drv_dev.h"
#include "mtos_printk.h"
#include "../inc/drv/bus/i2c.h"
#include "nim.h"
#include "atbm781x.h"
#include "MaxLinearDataTypes.h"

extern os_sem_t g_atbm_i2c_rw_mutex;

static i2c_bus_t *s_atbm_dmd_dev = NULL;
static i2c_bus_t *s_atbm_tuner_dev = NULL;

void ATBM_INIT(void *p_dmd, void *p_tuner)
{
  if(p_dmd)
	{
    s_atbm_dmd_dev = (i2c_bus_t *)p_dmd;
  }
	if(p_tuner)
	{
    s_atbm_tuner_dev = (i2c_bus_t *)p_tuner;
	}
}

void  ATBMSleep(uint32 ui32ms)
{
  mtos_task_delay_ms(ui32ms);
}

/****************************************************************************
Function:    ATBMMutexLock
Parameters:  none
Return:      none
Description:
This function is to get the mutex for I2C Read,	 it should be implement by user	.
         if you implement the function ATBMMutexIni,you can get the mutex
		 in this function.
****************************************************************************/
void ATBMMutexLock(void)
{
  mtos_sem_take(&g_atbm_i2c_rw_mutex, 0);
}
/****************************************************************************
Function:    ATBMMutexUnlock
Parameters:  none
Return:      none
Description:
This function is to get the mutex for I2C Write,it should be implement by user
              if you implement the function ATBMMutexIni,you can release the mutex
			  in this function.
****************************************************************************/
void ATBMMutexUnlock(void)
{
  mtos_sem_give(&g_atbm_i2c_rw_mutex);
}
/****************************************************************************
Function:    ATBMI2CRead
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8RegValue:The Pointer Which Return The Data Read Form Chip.
Return:      Read  Status:ATBM_SUCCESSFUL is Normal and other is abnormal.

Description:
This function used to read  data form chip register,it should be implemented by user
****************************************************************************/
ATBM_STATUS   ATBMI2CRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue)
{
  RET_CODE ret;
	uint8 buf[2] = {0};

	buf[0] = ui8BaseAddr;
	buf[1] = ui8RegisterAddr;

//  *ui8RegValue = ui8RegisterAddr;
  ret = i2c_std_read(s_atbm_dmd_dev, 0x80, buf, 2, 1, I2C_PARAM_DEV_SOC_INTER);
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	ATBMI2CRead reg %x FAILED!\n", ui8RegisterAddr);
  	return ATBM_I2C_ERROR;
  }
	*ui8RegValue = buf[0];
	return ATBM_SUCCESSFUL;
}
/****************************************************************************
Function:    ATBMI2CWrite
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8data:Data which will be write to chip.
Return:      Write status:ATBM_SUCCESSFUL is normal and other is abnormal.

Description:
This function used to write data to chip register,it should be implemented by user
****************************************************************************/
ATBM_STATUS   ATBMI2CWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data)
{
	//CI2CBUS::write_demod_bda(gDemod_addr,ui8BaseAddr, ui8RegisterAddr,1,ui8data);
  RET_CODE ret;
  uint8 buf[2];
  buf[0] = ui8BaseAddr;
  buf[1] = ui8RegisterAddr;
  buf[2] = ui8data;
  ret = i2c_write(s_atbm_dmd_dev, 0x80, buf, 3, I2C_PARAM_DEV_SOC_INTER);
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	ATBMI2CWrite reg %x data %x FAILED!\n", ui8RegisterAddr, ui8data);
  	return ATBM_I2C_ERROR;
  }
	return ATBM_SUCCESSFUL;
}
//////////////////////////////
//FOR tuner
//////////////////////////////

MS_BOOL MDrv_IIC_Write(MS_U8 u8SlaveID, MS_U8 *pu8Addr, MS_U8 u8AddrSize, MS_U8 *pu8Buf, MS_U32 u32BufSize)
{
  RET_CODE ret;

  ATBMMutexLock();
  ret = i2c_write(s_atbm_tuner_dev,u8SlaveID, pu8Buf, 2, I2C_PARAM_DEV_SOC_INTER);
  ATBMMutexUnlock();
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	MDrv_IIC_Write reg %x FAILED!\n", pu8Buf[0]);
  	return FALSE;
  }
	return TRUE;

}

MS_BOOL MDrv_IIC_Read(MS_U8 u8SlaveID, MS_U8 *pu8Addr, MS_U8 u8AddrSize, MS_U8 *pu8Buf, MS_U32 u32BufSize)
{
  RET_CODE ret;
  ATBMMutexLock();
  ret = i2c_std_read(s_atbm_tuner_dev, u8SlaveID, pu8Addr, 2, 1, I2C_PARAM_DEV_SOC_INTER);
  ATBMMutexUnlock();
  if (ret != SUCCESS)
  {
  	OS_PRINTF("MT:	MDrv_IIC_Read reg %x FAILED!\n", pu8Addr[0]);
  	return FALSE;
  }
	*pu8Buf = pu8Addr[0];
	return TRUE;


}
