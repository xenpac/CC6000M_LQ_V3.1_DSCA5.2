/*******************************************************************************
 *
 * FILE NAME          : MxL241SF_OEM_Drv.c
 * 
 * AUTHOR             : Brenndon Lee
 * DATE CREATED       : 7/30/2009
 *
 * DESCRIPTION        : This file contains I2C driver functins that OEM should
 *                      implement for MxL241SF APIs
 *                             
 *******************************************************************************
 *                Copyright (c) 2006, MaxLinear, Inc.
 ******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "hal_misc.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "i2c.h"
#include "MxL241SF_OEM_Drv.h"
extern void *p_mxl241_i2c;
/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_WriteRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C write operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_WriteRegister(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 RegData)
{
  MXL_STATUS status = MXL_TRUE;

  // OEM should implement I2C write protocol that complies with MxL241SF I2C
  // format.
 #if 1

  UINT8 Write_Cmd[4]; 
  if(p_mxl241_i2c == NULL)
    return MXL_FALSE;
    Write_Cmd[0] = (RegAddr>>8)&0xFF;
    Write_Cmd[1] = (RegAddr)&0xFF;
    Write_Cmd[2] = (RegData>>8)&0xFF;
    Write_Cmd[3] = (RegData)&0xFF;

  // User should implememnt his own I2C write register routine corresponding to
  // his hardaware.
    status=i2c_write(p_mxl241_i2c, MXL241_CHIP_ADRRESS, Write_Cmd, 4, 0);
    if(status!=0)
    {
    	//libc_printf("MxL Reg Write : status %d : Addr - 0x%x, data - 0x%x\n", status,RegAddr, RegData); 
    	mtos_printk("MxL Reg Write : status %d : Addr[ 0x%x]=  0x%x\n", status,RegAddr, RegData); 
    ;
    }
	//osal_task_sleep(10);
//	osal_semaphore_release(f_MXL241_IIC_Sema_ID);
#endif
  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : Ctrl_ReadRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C read operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS Ctrl_ReadRegister(UINT8 I2cSlaveAddr, UINT16 RegAddr, UINT16 *DataPtr)
{
  MXL_STATUS status = MXL_TRUE;
  // OEM should implement I2C read protocol that complies with MxL241SF I2C
  // format.
#if 1
    UINT8 Read_Cmd[4];
    UINT8   Read_data[2];
    if(p_mxl241_i2c == NULL)
          return MXL_FALSE;
    /* read step 1. accroding to mxl5007 driver API user guide. */
    Read_Cmd[0] = 0xFF;
    Read_Cmd[1] = 0xFB;
    Read_Cmd[2] = (RegAddr>>8)&0xFF;//ADD_HIGH
    Read_Cmd[3] = RegAddr&0xFF;//ADD_LOW

    status=i2c_write(p_mxl241_i2c, MXL241_CHIP_ADRRESS, Read_Cmd, 4, 0);
      
      //osal_task_sleep(1);
      
    status=i2c_read(p_mxl241_i2c, MXL241_CHIP_ADRRESS, Read_data, 2, 0);
    
    *DataPtr=(Read_data[0]<<8)|Read_data[1];
    if(status!=0)
    { 
        //libc_printf("MxL Reg Read : status  %d  : Addr - 0x%x, MSB - 0x%x LSB - 0x%x 0x%x\n",status, RegAddr, Read_data[0],Read_data[1],*DataPtr); 
        mtos_printk("MxL Reg Read : status  %d  : Addr [ 0x%x]= 0x%x\n",status, RegAddr, *DataPtr); 
        ;
    }
 
   #endif

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : GetSystemTimeMs
--| 
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 4/14/2011
--|
--| DESCRIPTION   : This function returns system time in millisecond
--|
--| RETURN VALUE  : System time in millisecond
--|
--|---------------------------------------------------------------------------*/

UINT32 GetSystemTimeMs()
{
  // OEM should implement this function
  UINT32 ticks = 0;
  UINT32 timer_clk = 0;
  ticks = mtos_hw_ticks_get();
  hal_module_clk_get(HAL_TIMER, (u32 *)&timer_clk);
  timer_clk /= 1000;
  return ticks /timer_clk;
}
