/****************************************************************************
Function:    ATBMRead
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8RegValue:The Pointer Which Return The Data Read Form Chip.
Return:      Read  Status:ATBM_SUCCESSFUL is Normal and other is abnormal.

Description:
This function used to read  data form chip register	 	  
****************************************************************************/
ATBM_STATUS   ATBMRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue);

/****************************************************************************
Function:    ATBMWrite
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8data:Data which will be write to chip.
Return:      Write status:ATBM_SUCCESSFUL is normal and other is abnormal.

Description:
This function used to write data to chip register 
****************************************************************************/
ATBM_STATUS   ATBMWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data);

/****************************************************************************
Function:    ATBMWriteRegArray
Parameters:  *ui8ARegTable, i32TableLen
Return:      I2C Write Status   0 is Success,Other is Failed

Description:
This function writes a group of registers to demodulator	 	 
****************************************************************************/
ATBM_STATUS  ATBMWriteRegArray(uint8 *ui8ARegTable, int i32TableLen);

/**********************same as  pow(x,y), y should greater than 0**************/

uint32  FixPow(uint32 x,uint32 y);

/**********************if add calcu overflow ,use this function*************************/
void ATBM64Add(ATBM64Data *pstSum,uint32 ui32Addend);

/**********************2 32bit data multipe,then use the next fuction *****************/
void ATBM64Mult(ATBM64Data *pstRst,uint32 m1, uint32 m2);

uint32 ATBM64Div(ATBM64Data stDivisor,ATBM64Data stDividend);
ATBM64Data ATBM64DivReturn64(ATBM64Data stDivisor,ATBM64Data stDividend);

/****************************************************************************
Function:   ATBMLatchOn
Parameters: none
Return:    Read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
latch on the register for reading,make sure the register group during reading period not change		 
******************************************************************************/
ATBM_STATUS  ATBMLatchOn(void);

/****************************************************************************
Function:   ATBMLatchOff
Parameters: none
Return:    Read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
latch off the register for reading		 
******************************************************************************/
ATBM_STATUS  ATBMLatchOff(void);

/****************************************************************************
Function:    ATBMI2CByPassOn
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed
Description:      
Enable demodulator's I2C gate function to pass I2C commands between tuner and host.  
****************************************************************************/
ATBM_STATUS  ATBMTunerI2CViaDemodOn(void);

/****************************************************************************
Function:    ATBMI2CByPassOff
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed
Description:      
Disable the I2C pass-through. when I2C via demodulator is used,
Tuner is disconnected from the I2C BUS after this function is called. 
****************************************************************************/
ATBM_STATUS  ATBMTunerI2CViaDemodOff(void);

/****************************************************************************
Function:    ATBMHoldDSP
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed  

Description:      
This function should be called before set tuner  frequency.
****************************************************************************/
ATBM_STATUS  ATBMHoldDSP(void);

/****************************************************************************
Function:    ATBMStartDSP
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed  

Description:      
This function should be called after set tuner  frequency.
Attention: ATBMHoldDSP and ATBMStartDSP should be used together.
For example:
ATBMHoldDSP();
SetTunerFrequency(Frequency_KHz);
ATBMStartDSP();        
****************************************************************************/
ATBM_STATUS  ATBMStartDSP(void);

/****************************************************************************
Function:    ATBMChipID
Parameters:  ui8ChipID:chip id will return by this pointer
Return:      I2C read Status   0 is Success,Other is Failed
Description:      
Get the chip id
****************************************************************************/
ATBM_STATUS  ATBMChipID(uint8 *ui8ChipID);
ATBM_STATUS ATBMSetTSMode( MPEGTSMode stTSMode);
ATBM_STATUS ATBMTSLockFlag(uint8 *LockStatus);
ATBM_STATUS ATBMSetGPIOHighLow(ATBM_GPIO_PIN_TYPE gpio_pin, uint8 attribute);
ATBM_STATUS ATBMSetGpioPins(void);
ATBM_STATUS ATBMStandBy();
ATBM_STATUS  ATBMStandByWakeUp();
ATBM_STATUS ATBMSusPend(void);
ATBM_STATUS ATBMSetSysClkClockDVBTx(void);
ATBM_STATUS ATBMSetSysClkClockDVBSx(void);
ATBM_STATUS ATBMSetSysClkClockDVBC(void);
ATBM_STATUS  ATBMSetDvbBWSFI(uint32 u32FsADCKhz, uint32 u32InvsFsAdcValue);
int ATBMInit(void);
ATBM_STATUS ATBMPowerOnInit();

