/*****************************************************************************************
 *
 * FILE NAME          : MxL603_OEM_Drv.c
 *
 * AUTHOR             : Mahendra Kondur
 *
 * DATE CREATED       : 12/23/2011
 *
 * DESCRIPTION        : This file contains I2C driver and Sleep functins that
 *                      OEM should implement for MxL603 APIs
 *
 *****************************************************************************************
 *                Copyright (c) 2011, MaxLinear, Inc.
 ****************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "MxL608_OEM_Drv.h"





#if (DEMODE_TYPE == DEMOD_MN88472)

#include "MN_DMD_driver.h"
#include "MN_i2c.h"
extern DMD_ERROR_t DMD_TCB_WriteRead(DMD_u8_t slvadr , DMD_u8_t adr , DMD_u8_t* wdata , DMD_u32_t wlen , DMD_u8_t* rdata , DMD_u32_t rlen);

#endif


static u32 s_DEMOD_TYPE = 0;

void MxWare603_OEM_init(UINT32 Demd_t)
{ 
  s_DEMOD_TYPE = Demd_t;
}
/*----------------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare603_OEM_WriteRegister
--|
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C write operation.
--|
--| RETURN VALUE  : True or False
--|
--|-------------------------------------------------------------------------------------*/

MXL_STATUS MxLWare603_OEM_WriteRegister(UINT8 devId, UINT8 RegAddr, UINT8 RegData)
{
  // OEM should implement I2C write protocol that complies with MxL603 I2C
  // format.

  // 8 bit Register Write Protocol:
  // +------+-+-----+-+-+----------+-+----------+-+-+
  // |MASTER|S|SADDR|W| |RegAddr   | |RegData(L)| |P|
  // +------+-+-----+-+-+----------+-+----------+-+-+
  // |SLAVE |         |A|          |A|          |A| |
  // +------+---------+-+----------+-+----------+-+-+
  // Legends: SADDR (I2c slave address), S (Start condition), A (Ack), N(NACK),
  // P(Stop condition)

  MXL_STATUS status = MXL_FALSE;


/* If OEM data is implemented, customer needs to use OEM data structure related operation
   Following code should be used as a reference.
   For more information refer to sections 2.5 & 2.6 of MxL603_mxLWare_API_UserGuide document.

  UINT8 i2cSlaveAddr;
  UINT8 i2c_bus;
  user_data_t * user_data = (user_data_t *) MxL603_OEM_DataPtr[devId];

  if (user_data)
  {
    i2cSlaveAddr = user_data->i2c_address;           // get device i2c address
    i2c_bus = user_data->i2c_bus;                   // get device i2c bus

    sem_up(user_data->sem);                         // up semaphore if needed

    // I2C Write operation
    status = USER_I2C_WRITE_FUNCTION(i2cSlaveAddr, i2c_bus, RegAddr, RegData);

    sem_down(user_data->sem);                       // down semaphore
    user_data->i2c_cnt++;                           // user statistics
  }

*/

  /* If OEM data is not required, customer should treat devId as I2C slave Address */

    MS_U8 pArray[6];

    pArray[0] = RegAddr;
    pArray[1] = RegData;
#if (DEMODE_TYPE == DEMOD_MN88472)
   {
      devId = devId;
      RegAddr = RegAddr;
      RegData = RegData;
      //DMD_DEBUG(" Tuner write in , devid[%x], regaddr[%x], data[%x] \n", devId, RegAddr,RegData);

      status = DMD_TCB_WriteRead(devId, RegAddr, &RegData, 1, 0, 0);

   }   

#endif
   return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare603_OEM_ReadRegister
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

MXL_STATUS MxLWare603_OEM_ReadRegister(UINT8 devId, UINT8 RegAddr, UINT8 *DataPtr)
{
  // OEM should implement I2C read protocol that complies with MxL603 I2C
  // format.

  // 8 bit Register Read Protocol:
  // +------+-+-----+-+-+----+-+----------+-+-+
  // |MASTER|S|SADDR|W| |0xFB| |RegAddr   | |P|
  // +------+-+-----+-+-+----+-+----------+-+-+
  // |SLAVE |         |A|    |A|          |A| |
  // +------+-+-----+-+-+----+-+----------+-+-+
  // +------+-+-----+-+-+-----+--+-+
  // |MASTER|S|SADDR|R| |     |MN|P|
  // +------+-+-----+-+-+-----+--+-+
  // |SLAVE |         |A|Data |  | |
  // +------+---------+-+-----+--+-+
  // Legends: SADDR(I2c slave address), S(Start condition), MA(Master Ack), MN(Master NACK),
  // P(Stop condition)

  MXL_STATUS status = MXL_TRUE;

/* If OEM data is implemented, customer needs to use OEM data structure related operation
   Following code should be used as a reference.
   For more information refer to sections 2.5 & 2.6 of MxL603_mxLWare_API_UserGuide document.

  UINT8 i2cSlaveAddr;
  UINT8 i2c_bus;
  user_data_t * user_data = (user_data_t *) MxL603_OEM_DataPtr[devId];

  if (user_data)
  {
    i2cSlaveAddr = user_data->i2c_address;           // get device i2c address
    i2c_bus = user_data->i2c_bus;                   // get device i2c bus

    sem_up(user_data->sem);                         // up semaphore if needed

    // I2C Write operation
    status = USER_I2C_READ_FUNCTION(i2cSlaveAddr, i2c_bus, RegAddr, DataPtr);

    sem_down(user_data->sem);                       // down semaphore
    user_data->i2c_cnt++;                           // user statistics
  }

*/

  /* If OEM data is not required, customer should treat devId as I2C slave Address */

  UINT8 pArray[6];

  pArray[0] = 0xFB;
  pArray[1] = RegAddr;

#if (DEMODE_TYPE == DEMOD_MN88472)
  {
    devId = devId;
    RegAddr = RegAddr;
    *DataPtr = *DataPtr;
 
  //  mtos_printk(" Tuner read  in , devid[%x], regaddr[%x] ,data[%x]\n", devId, RegAddr,*DataPtr);

    status = DMD_TCB_WriteRead(devId, 0xFB, &RegAddr, 1, 0, 0);
    status =  DMD_TCB_WriteRead(devId, 0, 0, 0, DataPtr, 1);
  //  mtos_printk(" Tuner read  in , devid[%x], regaddr[%x] ,data[%x]\n", devId, RegAddr,*DataPtr);
    if(status == DMD_E_OK)
    {
      return status = MXL_TRUE ;
    } 
    else
    {
      mtos_printk("i2c read err\n");
      return status = MXL_FALSE;
    }
  }
#endif
  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare603_OEM_Sleep
--|
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 01/10/2010
--|
--| DESCRIPTION   : This function complete sleep operation. WaitTime is in ms unit
--|
--| RETURN VALUE  : None
--|
--|-------------------------------------------------------------------------------------*/

void MxLWare603_OEM_Sleep(UINT16 DelayTimeInMs)
{
  // OEM should implement sleep operation
#if (DEMODE_TYPE == DEMOD_MN88472)
  {
    DMD_wait(DelayTimeInMs);
  }  
#endif
}


