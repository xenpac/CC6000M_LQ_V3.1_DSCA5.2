/***************************************************************************** 
Copyright 2012, AltoBeam Inc. All rights reserved.
     
File Name: AtbmCommon.c
   
Version: 1.0
Released 2013-10-16

Description: Example programming interface for ATBM781x	

Important struct:

gstATBMDvbConfig
           All hardware dependent parameters should be customized according to system design          

Important functions:

ATBMPowerOnInit()
           This function is the first function should be called. default DVB-T/T2 8MHz Bandwidth. 
           ATBMSetDvbtBandwidth(unsigned char u8bandwidthMHz) function can be used to set Bandwidth for DVB-T/T2.

ATBMSetDVBTxMode(unsigned char u8bandwidthMHz)
           This function sets the demodulator working in DVB-T/T2 auto mode.
		   The demodulator can receive DVB-T and DVB-T2 without discrimination on register setting.
           For example, it can be also used in mode switching between DVB-T/T2 and DVB-C
		   
           
ATBMSetDVBCMode()
           This function sets the demodulator working in DVB-C mode.
           For example, it can be used in mode switching between DVB-T/T2 and DVB-C

ATBMSetDvbsMode() 
          This function sets the demodulator working in DVB-S mode.

ATBMSetDvbs2Mode( );        
          This function sets the demodulator working in DVB-S2 mode.

ATBMTSLockFlag(uint8 *LockStatus);
           This function checks the demodulator TS locking status.   
******************************************************************************/
#include <stdio.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"

#include "Atbm781x.h"

DVBT2_PARAMS gDvbt2Params;
DVBT_PARAMS gDvbtParams;
stATBMDvbConfig gstATBMDvbConfig;


unsigned char ui8ATBMSlaveAddress = 0x80;

int i32SNRLogTable[]=
{
	0, 100,
	2, 104,
	4, 109,
	6, 114,
	8, 120,
	10, 125,
	12, 131,
	14, 138,
	16, 144,
	18, 151,
	20, 158,
	22, 166,
	24, 173,
	26, 182,
	28, 190,
	30, 199,
	32, 208	
};
#if 0

/****************************************************************************
Function:    ATBMSleep
Parameters:  ui32ms :Unit is Millisecond
Return:      none
Description:
This function implement cpu time delay,it should be implemented by user	 	  
****************************************************************************/
void   ATBMSleep(uint32 ui32ms)
{
 // Sleep(ui32ms);
}
/****************************************************************************
Function:    ATBMMutexIni
Parameters:  none
Return:      none
Description:
This function initialize the mutex for   ATBMMutexLock and ATBMMutexUnlock call
        it should be implement by user.generally ,user can define an handle(windows)
		or a variable(linux) in  current file as a global parameter,then initialize
		it in the function
****************************************************************************/
ATBM_STATUS ATBMMutexIni(void)
{
	return ATBM_SUCCESSFUL;
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

}
/****************************************************************************
Function:    ATBMI2CRead
Parameters:   ui8BaseAddr register Base Address, ui8RegisterAddr:register offset Address;
ui8RegValue:The Pointer Which Return The Data Read Form Chip.
Return:      Read  Status:ATBM_SUCCESSFUL is Normal and other is abnormal.

Description:
This function used I2CRead to read  data form chip register,it should be implemented by user	 	  
****************************************************************************/
ATBM_STATUS   ATBMI2CRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue)
{
	//CI2CBUS::read_demod_bda(gDemod_addr,ui8BaseAddr, ui8RegisterAddr,1,ui8RegValue);
	ATBM_STATUS ui8Status =  ATBM_SUCCESSFUL; 
       uint8 ui8AddrData[2];

	ui8AddrData[0] = ui8BaseAddr;
	ui8AddrData[1] = ui8RegisterAddr;

	//One step I2C format, please port this function according to I2C format of this function..
	//return I2CRead(ui8ATBMSlaveAddr, 2, ui8AddrData, 1, ui8RegValue);
	
	return ui8Status;
}
/****************************************************************************
Function:    ATBMI2CWrite
Parameters:  ui8BaseAddr register Base Address, ui8RegisterAddr:register offset Address;
                    ui8data:  Data which will be write to chip.
Return:         Write status:ATBM_SUCCESSFUL is normal and other is abnormal.

Description:
This function used I2CWrite to write data to chip register,it should be implemented by user	 	  
****************************************************************************/
ATBM_STATUS   ATBMI2CWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data)
{ 
	//CI2CBUS::write_demod_bda(gDemod_addr,ui8BaseAddr, ui8RegisterAddr,1,ui8data);
	ATBM_STATUS ui8Status =  ATBM_SUCCESSFUL; 
	uint8 ui8I2CData[3];

	ui8I2CData[0] = ui8BaseAddr;
	ui8I2CData[1] = ui8RegisterAddr;
	ui8I2CData[2] = ui8data;	

	//ui8Status = I2CWrite(ui8ATBMSlaveAddress, ui8I2CData, 3);

	return ui8Status;	
}
#endif
/****************************************************************************
Function:    ATBMRead
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8RegValue:The Pointer Which Return The Data Read Form Chip.
Return:      Read  Status:ATBM_SUCCESSFUL is Normal and other is abnormal.

Description:
This function used to read  data form chip register	 	  
****************************************************************************/
ATBM_STATUS   ATBMRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue)
{
	ATBM_STATUS ui8Status;

	ATBMMutexLock();
	ui8Status = ATBMI2CRead( ui8BaseAddr, ui8RegisterAddr, ui8RegValue);
       ATBMMutexUnlock();
	return ui8Status;
}


/****************************************************************************
Function:    ATBMWrite
Parameters:  ui8BaseAddr Chip Base Address,eg.0x88; ui8RegisterAddr:Chip Sub Address;
ui8data:Data which will be write to chip.
Return:      Write status:ATBM_SUCCESSFUL is normal and other is abnormal.

Description:
This function used to write data to chip register 
****************************************************************************/
ATBM_STATUS   ATBMWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data)
{ 
	ATBM_STATUS ui8Status;
    ATBMMutexLock();
    ui8Status = ATBMI2CWrite(ui8BaseAddr,ui8RegisterAddr,ui8data);    
    ATBMMutexUnlock();
	ATBMDebugPrint("%0x  %0x  %0x\n",ui8BaseAddr,ui8RegisterAddr,ui8data);
	return ui8Status;
}



/****************************************************************************
Function:   ATBMDVBCRead
Parameters: uAddress: Register physical address;ui32RData - data returned by this pointer 
Return:    read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
For DVB-C register read	 
******************************************************************************/

ATBM_STATUS  ATBMDVBCRead(uint32 uAddress,uint32 *ui32RData)
{

	ATBM_STATUS ui8Status;
	uint8 ui8PData[4] = {0};
	ATBMMutexLock();
	ui8Status = ATBMI2CWrite(0x15,0x03,0x0);
	ui8Status |= ATBMI2CWrite(0x15,0x01,uAddress&0xff);
	ui8Status |= ATBMI2CWrite(0x15,0x02,(uAddress>>8)&0xff );

	ui8Status |= ATBMI2CWrite(0x01, 0x0d, 1);//ro_latch_on
	ui8Status |= ATBMI2CRead(0x15, 0x08, &ui8PData[0]);
	ui8Status |= ATBMI2CRead(0x15, 0x09, &ui8PData[1]);
	ui8Status |= ATBMI2CRead(0x15, 0x0a, &ui8PData[2]);
	ui8Status |= ATBMI2CRead(0x15, 0x0b,  &ui8PData[3]);
	ui8Status |= ATBMI2CWrite(0x01, 0x0d,0 );//ro_latch_off
	ATBMMutexUnlock();
	*ui32RData = (ui8PData[3]<<24) + (ui8PData[2]<<16) + (ui8PData[1]<<8) +ui8PData[0];

	return ui8Status;
}
/****************************************************************************
Function:   ATBMDVBCWrite
Parameters: ui32Address: Register physical address;ui32Data - 32bit data 
Return:    Write status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
For DVB-C register write		 
******************************************************************************/
ATBM_STATUS  ATBMDVBCWrite(uint32 ui32Address, uint32 ui32Data)
{
	// if(  (true == CI2CBUS::m_RecordSettings)&&(CI2CBUS::m_sw))
	// 	{
	// 		CString str;
	// 		str.Format(L"%x  %x\n",ui32Address,ui32Data);
	// 		CI2CBUS::m_sw.WriteString(str);
	// 		CI2CBUS::m_WriteDvbcFlag=true;
	// 	}

	//-------------------- write --------------------------//
	ATBM_STATUS ui8Status = ATBM_SUCCESSFUL;

	ATBMMutexLock();
	

	ui8Status = ATBMI2CWrite(0x15,0x03,0x0);
	ui8Status |= ATBMI2CWrite(0x15,0x01,ui32Address&0xff);
	ui8Status |= ATBMI2CWrite(0x15,0x02,(ui32Address>>8)&0xff );

	ui8Status |= ATBMI2CWrite(0x15,0x04,(uint8)(ui32Data&0xff));
	ui8Status |= ATBMI2CWrite(0x15,0x05,(uint8)((ui32Data>>8)&0xff));
	ui8Status |= ATBMI2CWrite(0x15,0x06,(uint8)((ui32Data>>16)&0xff));
	ui8Status |= ATBMI2CWrite(0x15,0x07,(uint8)((ui32Data>>24)&0xff));
	ui8Status |= ATBMI2CWrite(0x15, 0x03,0x1);

	ui8Status |= ATBMI2CWrite(0x15, 0x03,0x0);
	ATBMDebugPrint("%0x  %0x\n",ui32Address,ui32Data);
	ATBMMutexUnlock();
	// 	if(  true ==CI2CBUS::m_RecordSettings)
	// 	{
	// 	   CI2CBUS::m_WriteDvbcFlag=false;
	// 	}
	return ui8Status;
}

/****************************************************************************
Function:   ATBMDVBSWrite
Parameters: ui16Addr: Register physical address;ui32Data - 32bit data 
Return:    Write status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
For DVBS and DVBS2 mode register write		 
******************************************************************************/
ATBM_STATUS  ATBMDVBSWrite(uint16 ui16Addr,uint32 ui32Data)
{

	//-------------------- write --------------------------//
	ATBM_STATUS atbmStatus = ATBM_SUCCESSFUL;
/*	uint8 ui8PSetting[] =
	{
		0x29,0x02,0x0,
		0x29,0x00,ui16Addr&0xff,     //address low part,
		0x29,0x01,(ui16Addr>>8)&0xff,//address high part
		0x29,0x03,(uint8)(ui32Data&0xff),
		0x29,0x4,(uint8)((ui32Data>>8)&0xff),
		0x29,0x5,(uint8)((ui32Data>>16)&0xff),
		0x29,0x6,(uint8)((ui32Data>>24)&0xff), 
		0x29,0x02,0x1,
	};*/
	// 	if(  (true == CI2CBUS::m_RecordSettings))
	// 	{
	// 		CString str;
	// 		str.Format(L"%x  %x\n",ui16Addr,ui32Data);
	// 		CI2CBUS::m_sw.WriteString(str);
	// 		CI2CBUS::m_WriteDvbcFlag=true;
	// 	}
	ATBMMutexLock();
	atbmStatus = ATBMI2CWrite(0x29,0x02,0x00);//
	atbmStatus |= ATBMI2CWrite(0x29,0x00,ui16Addr&0xff);//
	atbmStatus |= ATBMI2CWrite(	0x29,0x01,(uint8)(ui16Addr>>8)&0xff);//

	atbmStatus |= ATBMI2CWrite(0x29,0x03,(uint8)(ui32Data&0xff));//
	atbmStatus |= ATBMI2CWrite(0x29,0x4,(uint8)((ui32Data>>8)&0xff));//
	atbmStatus |= ATBMI2CWrite(0x29,0x5,(uint8)((ui32Data>>16)&0xff));//
	atbmStatus |= ATBMI2CWrite(0x29,0x6,(uint8)((ui32Data>>24)&0xff));//
	atbmStatus |= ATBMI2CWrite(0x29,0x02,0x01);//
	ATBMDebugPrint("%0x  %0x\n",ui16Addr,ui32Data);
	ATBMMutexUnlock();

	return atbmStatus;

}
/****************************************************************************
Function:   ATBMDVBSRead
Parameters: ui16Addr: Register physical address;ui32RegData - 32bit data read from chip register
Return:    Read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
For DVBS and DVBS2 mode register Read		 
******************************************************************************/
ATBM_STATUS  ATBMDVBSRead(uint16 ui16Addr,uint32 *ui32RegData)
{
	ATBM_STATUS atbmStatus = ATBM_SUCCESSFUL;
	uint8 data[4] = {0};

	ATBMMutexLock();

	atbmStatus = ATBMI2CWrite(0x29,0x02,0x00);//
	atbmStatus |= ATBMI2CWrite(0x29,0x00,ui16Addr&0xff);//
	atbmStatus |= ATBMI2CWrite(0x29,0x01,(ui16Addr>>8)&0xff);//

	atbmStatus |= ATBMI2CWrite(0x01, 0x0d,1);//ro_latch_on
	atbmStatus |=ATBMI2CRead(0x29, 0x7,&data[0]);
	atbmStatus |=ATBMI2CRead(0x29, 0x8,&data[1]);
	atbmStatus |=ATBMI2CRead(0x29, 0x9,&data[2]);
	atbmStatus |=ATBMI2CRead(0x29, 0xa,&data[3]);
	atbmStatus |=ATBMI2CWrite(0x01, 0x0d, 0);//ro_latch_off
	ATBMMutexUnlock();
	*ui32RegData = (data[3]<<24) + (data[2]<<16) + (data[1]<<8) +data[0];
	return atbmStatus;
}
/****************************************************************************
Function:    ATBMWriteRegArray
Parameters:  *ui8ARegTable, i32TableLen
Return:      I2C Write Status   0 is Success,Other is Failed

Description:
This function writes a group of registers to demodulator	 	 
****************************************************************************/
ATBM_STATUS  ATBMWriteRegArray(uint8 *ui8ARegTable, int i32TableLen)
{
	ATBM_STATUS ui8Status = 0;
	int i32Iter;
	for(i32Iter=0;i32Iter<i32TableLen;i32Iter+=3)
	{
		ui8Status |= ATBMWrite(ui8ARegTable[i32Iter],ui8ARegTable[i32Iter+1],ui8ARegTable[i32Iter+2]);
		//ATBMDebugPrint("%x %x %x\n", ui8ARegTable[i32Iter],ui8ARegTable[i32Iter+1], ui8ARegTable[i32Iter+2]); 		
	}
	return ui8Status;
}


/****************************************************************************
Function:   LeftTable
Description:
use for Log10Convert			 
******************************************************************************/

int LeftTable(int i32InValue)
{
	int i32OutValue = 0;
	int i32Iter;	
	int i32TableSize = sizeof(i32SNRLogTable)/sizeof(int);
	for(i32Iter=0; i32Iter<i32TableSize; i32Iter+=2)
	{	
		if(i32InValue<=i32SNRLogTable[i32Iter+1])
		{		
			i32OutValue = i32SNRLogTable[i32Iter];	
			return i32OutValue;
		}	
	}
	i32OutValue = 32;
	return i32OutValue;
}
/**********************same as  100 * log10(i32InValue)*************************/

uint32 Log10Convertx10(uint32 i32InValue)
{
	uint8       		ui8Index = 0;
	uint32		        ui32Tmp = 1;
	uint32              ui32InValueLeft;
	uint32              ui32LogValue;    
	if(i32InValue == 0)
	{
		return 0;
	}

	do 
	{	
		ui32Tmp = ui32Tmp << 1;
		if (i32InValue < ui32Tmp)
		{
			break;	
		}
	}
	while(++ui8Index < 32);
	ui32LogValue = 301*ui8Index/100;
	ui32InValueLeft = i32InValue*100/(1<<ui8Index);
	ui32LogValue = ui32LogValue*10 + LeftTable(ui32InValueLeft);
	//ATBMDebugPrint(" i32InValue = %d  log ret = %d\n", i32InValue,  ui32LogValue);
	return ui32LogValue;	
}

uint32 Log10Convert(uint32 i32InValue)
{
	uint8       		ui8Index = 0;
	uint32		        ui32Tmp = 1;
	uint32              ui32InValueLeft;
	uint32              ui32LogValue;    
	if(i32InValue == 0)
	{
		return 0;
	}

	do 
	{	
		ui32Tmp = ui32Tmp << 1;
		if (i32InValue < ui32Tmp)
		{
			break;	
		}
	}
	while(++ui8Index < 32);
	ui32LogValue = 301*ui8Index/100;
	ui32InValueLeft = i32InValue*100/(1<<ui8Index);
	ui32LogValue = ui32LogValue + LeftTable(ui32InValueLeft)/10;
	//ATBMDebugPrint(" i32InValue = %d  log ret = %d\n", i32InValue,  ui32LogValue);
	return ui32LogValue;	
}
/**********************same as  pow(x,y), y should greater than 0**************/

uint32  FixPow(uint32 x,uint32 y)
{
	uint32 ui32Rst = x;
	uint32 ui32Iter;
	if (y == 0)
	{
		return 1;
	}

	for (ui32Iter = 1; ui32Iter<y;ui32Iter++)
	{
		ui32Rst*= x;
	}
	return ui32Rst;
}
/**********************if add calcu overflow ,use this function*************************/
void ATBM64Add(ATBM64Data *pstSum,uint32 ui32Addend)
{
	uint32 ui32LowPart;
	ui32LowPart = pstSum->ui32Low;
	pstSum->ui32Low += ui32Addend;
	if (ui32LowPart > pstSum->ui32Low)
	{
		pstSum->ui32High++;
	}
}

/**********************2 32bit data multipe,then use the next fuction *****************/
void ATBM64Mult(ATBM64Data *pstRst,uint32 m1, uint32 m2)
{
	uint32 ui32LowPart,ui32Tmp;
	pstRst->ui32Low = (m1&0xffff) * (m2&0xffff);
	pstRst->ui32High = (m1>>16) * (m2>>16);

	ui32LowPart = pstRst->ui32Low;
	ui32Tmp = (m1>>16) * (m2&0xffff);
	pstRst->ui32Low  += (ui32Tmp<<16);
	pstRst->ui32High += (ui32Tmp>>16);
	if(ui32LowPart > pstRst->ui32Low)
	{
		pstRst->ui32High++;
	}

	ui32LowPart = pstRst->ui32Low;
	ui32Tmp = (m2>>16) * (m1&0xffff);
	pstRst->ui32Low  += (ui32Tmp<<16);
	pstRst->ui32High += (ui32Tmp>>16);
	if(ui32LowPart > pstRst->ui32Low)
	{
		pstRst->ui32High++;
	}
}

uint8  ATBM64ComPare(ATBM64Data stPara,ATBM64Data stPara1)
{
	uint8 ui8Flag = 0;//default :stPara<=stPara1
	if (stPara.ui32Low>=stPara1.ui32Low)
	{
		if (stPara.ui32High >= stPara1.ui32High)
		{
			ui8Flag = 1;
		}
	}
	else
	{
		if (stPara.ui32High > stPara1.ui32High)
		{
			ui8Flag = 1;
		}
	}
	return ui8Flag;

}
uint32 ATBM64Div(ATBM64Data stDivisor,ATBM64Data stDividend)
{
	uint8  ui8DivLeftShit = 0;
	uint32 ui32Rst = 0;
	ATBM64Data stDataTmp = stDividend;
	if (stDivisor.ui32High == 0&&stDivisor.ui32Low == 0)
	{
		return 0;
	}
	else if (stDividend.ui32High == 0&&stDividend.ui32Low == 0)
	{
		return 0;
	}
	else if (stDividend.ui32High > stDivisor.ui32High)
	{
		return 0;
	}
	else if (!ATBM64ComPare(stDivisor,stDividend))
	{
		return 0;
	}
	else if (stDividend.ui32High == 0 && stDivisor.ui32High == 0)
	{
		return stDivisor.ui32Low/stDividend.ui32Low;
	}
	else// normal condition  stDivisor >= stDividend
	{
		do
		{
			++ui8DivLeftShit;
			stDataTmp.ui32High<<=1;
			if(stDataTmp.ui32Low&0x80000000)
			{
				stDataTmp.ui32High +=1;
			}
			stDataTmp.ui32Low<<=1;

			if (!ATBM64ComPare(stDivisor,stDataTmp)||stDataTmp.ui32High>=0x80000000)
			{
				if (stDataTmp.ui32High<0x80000000)
				{
					stDataTmp.ui32Low>>=1;
					if (stDataTmp.ui32High&1)
					{
						stDataTmp.ui32Low |= 0x80000000;
					}
					stDataTmp.ui32High>>=1;
					ui8DivLeftShit--;
				}


				stDivisor.ui32High -= stDataTmp.ui32High;
				if(stDivisor.ui32Low >= stDataTmp.ui32Low)
				{
					stDivisor.ui32Low -= stDataTmp.ui32Low;
				}
				else
				{
					stDivisor.ui32Low +=0xffffffff - stDataTmp.ui32Low;
					stDivisor.ui32Low += 1;
					stDivisor.ui32High -= 1;
				}
				ui32Rst = (1<<ui8DivLeftShit);;
				break;
			}
		}while(ui8DivLeftShit);
		ui32Rst += ATBM64Div(stDivisor,stDividend);
	}
	return ui32Rst;
}
ATBM64Data ATBM64DivReturn64(ATBM64Data stDivisor,ATBM64Data stDividend)
{
	uint8  ui8DivLeftShit = 0;
	uint32 ui32LowPart;
	ATBM64Data i64Data,i64Data1;
	ATBM64Data stDataTmp = stDividend;
	i64Data.ui32High = 0;
	i64Data.ui32Low = 0;
	if (stDivisor.ui32High == 0&&stDivisor.ui32Low == 0)
	{
		return i64Data;
	}
	else if (stDividend.ui32High == 0&&stDividend.ui32Low == 0)
	{
		return i64Data;
	}
	else if (stDividend.ui32High > stDivisor.ui32High)
	{
		return i64Data;
	}
	else if (!ATBM64ComPare(stDivisor,stDividend))
	{
		return i64Data;
	}
	else if (stDividend.ui32High == 0 && stDivisor.ui32High == 0)
	{
		i64Data.ui32Low = stDivisor.ui32Low/stDividend.ui32Low;
		return i64Data;
	}
	else// normal condition  stDivisor >= stDividend
	{
		do
		{
			++ui8DivLeftShit;
			stDataTmp.ui32High<<=1;
			if(stDataTmp.ui32Low&0x80000000)
			{
				stDataTmp.ui32High +=1;
			}
			stDataTmp.ui32Low<<=1;
			if (!ATBM64ComPare(stDivisor,stDataTmp)||stDataTmp.ui32High>=0x80000000)
			{
				if (stDataTmp.ui32High<0x80000000)
				{
					stDataTmp.ui32Low>>=1;
					if (stDataTmp.ui32High&1)
					{
						stDataTmp.ui32Low |= 0x80000000;
					}
					stDataTmp.ui32High>>=1;
					ui8DivLeftShit--;
				}


				stDivisor.ui32High -= stDataTmp.ui32High;
				if(stDivisor.ui32Low >= stDataTmp.ui32Low)
				{
					stDivisor.ui32Low -= stDataTmp.ui32Low;
				}
				else
				{
					stDivisor.ui32Low +=0xffffffff - stDataTmp.ui32Low;
					stDivisor.ui32Low += 1;
					stDivisor.ui32High -= 1;
				}
				if (ui8DivLeftShit>=32)
				{
					i64Data.ui32High = (1<<(ui8DivLeftShit-32));
					i64Data.ui32Low = 0; 
				}
				else
				{
					i64Data.ui32Low = (1<<ui8DivLeftShit);
					i64Data.ui32High = 0;
				}
				break;
			}
		}while(ui8DivLeftShit);
		i64Data1 = ATBM64DivReturn64(stDivisor,stDividend);
		ui32LowPart = i64Data.ui32Low;
		i64Data.ui32Low += i64Data1.ui32Low;
		i64Data.ui32High += i64Data1.ui32High;
		if (ui32LowPart>i64Data.ui32Low)
		{
			i64Data.ui32High+=1;
		}

	}
	return i64Data;
}

/****************************************************************************
Function:   ATBMLatchOn
Parameters: none
Return:    Read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
latch on the register for reading,make sure the register group during reading period not change		 
******************************************************************************/
ATBM_STATUS  ATBMLatchOn(void)
{
	return ATBMWrite(0x01,0x0d,1);
}
/****************************************************************************
Function:   ATBMLatchOff
Parameters: none
Return:    Read status:ATBM_SUCCESSFUL is normal and other is abnormal.
Description:
latch off the register for reading		 
******************************************************************************/
ATBM_STATUS  ATBMLatchOff(void)
{
	return ATBMWrite(0x01,0x0d,0);
}
/****************************************************************************
Function:    ATBMI2CByPassOn
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed
Description:      
Enable demodulator's I2C gate function to pass I2C commands between tuner and host.  
****************************************************************************/
ATBM_STATUS  ATBMTunerI2CViaDemodOn(void)
{
	/* 1: Enable demodulator's I2C gate to pass I2C commands between tuner and host. */
	return ATBMWrite(0x01,0x03, 1); 
}

/****************************************************************************
Function:    ATBMI2CByPassOff
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed
Description:      
Disable the I2C pass-through. when I2C via demodulator is used,
Tuner is disconnected from the I2C BUS after this function is called. 
****************************************************************************/
ATBM_STATUS  ATBMTunerI2CViaDemodOff(void)
{
	/*0: Disable demodulator's I2C gate function to pass I2C commands between tuner and host*/
	return ATBMWrite(0x01,0x03, 0); 
}
/****************************************************************************
Function:    ATBMHoldDSP
Parameters:  none
Return:      I2C Write Status   0 is Success,Other is Failed  

Description:      
This function should be called before set tuner  frequency.
****************************************************************************/
ATBM_STATUS  ATBMHoldDSP(void)
{	 
	return ATBMWrite(0x00, 0x05, 0x01); 
}

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
ATBM_STATUS  ATBMStartDSP(void)
{
	return ATBMWrite(0x00, 0x05, 0x00); 
}
/****************************************************************************
Function:    ATBMChipID
Parameters:  ui8ChipID:chip id will return by this pointer
Return:      I2C read Status   0 is Success,Other is Failed
Description:      
Get the chip id
****************************************************************************/
ATBM_STATUS  ATBMChipID(uint8 *ui8ChipID)
{
	return ATBMRead(0x00, 0x00, ui8ChipID); 

}

int ATBMLog2(int i32Data)//对数换底公式，作为内部函数使用
{
	int i32Tmp = 2;
	return 10*Log10Convert(i32Data)/Log10Convert(i32Tmp);
}





ATBM_STATUS ATBMSetTSMode( MPEGTSMode stTSMode)
{
	uint8 ui8SerialEnabled =0x0;/*parallel clock out */
    uint8 ui8ChipID = 0;
	uint8 ui8TSOutputEdegeMode = stTSMode.ui8OutputEdge;  
	uint8 ui8TsClockMode = stTSMode.ui8SPIClockConstantOutput;  
	ATBM_STATUS status = ATBM_SUCCESSFUL;

       status |= ATBMRead(0x00, 0x00, &ui8ChipID);
	/*0x00: SPI data align with rising edge of TSBCLK; 0x01: SPI data align with falling edge of TSBCLK*/
	status |=ATBMWrite(0x18,0x02, ui8TSOutputEdegeMode); 	
	status |=ATBMWrite(0x18,0x03, ui8TsClockMode);  /*either free running or only active when data are valid*/

	if(ui8ChipID == 0xa0)
	{
		status |=ATBMWrite(0x00,0x25, 0xff); 	
		status |=ATBMWrite(0x00,0x26, 0x7);  /*either free running or only active when data are valid*/
	}
	if((ui8ChipID == 0xa4))
	{
		status |=ATBMWrite(0x26,0x24, 0x05); 
		status |=ATBMWrite(0x26,0x25, 0xff); 	
		status |=ATBMWrite(0x26,0x26, 0x7);  /*either free running or only active when data are valid*/
	}
	if((ui8ChipID == 0xa2))
	{	
		status |=ATBMWrite(0x26,0x24, 0x05); 
		status |=ATBMWrite(0x26,0x25, 0xff); 	
		status |=ATBMWrite(0x26,0x26, 0x7);  /*either free running or only active when data are valid*/
	}
	if(stTSMode.ui8TSTransferType==TS_SERIAL_MODE)
	{    
		ui8SerialEnabled = 0x01;    /*enable serial TS mode */   

		//SSI_sync_hold_cycle = 0x00; //0x00:1bit sync; 0x01: 8bit sync
		/*0: serial sync  holds 1 cycle; 1: serial sync holds 8 cycles*/
		//ATBMWrite(0x17,0xb0, SSI_sync_hold_cycle);  

		/*0:disable serial mode; 1: enable serial mode */  
		status |=ATBMWrite(0x18,0x01, ui8SerialEnabled);      
		if(stTSMode.ui8TSSSIOutputSelection == TS_SSI_OUTPUT_ON_DATA_BIT0)
		{
			status |=ATBMWrite(0x18, 0x04, 0x00); 
		}else
		{
			status |=ATBMWrite(0x18, 0x04, 0x01); 
		}
		if(ui8ChipID == 0xa0)
		{
			status |=ATBMWrite(0x00, 0x27, 0x00);
		}
		if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
		{

			status |=ATBMWrite(0x26, 0x27, 0x00);
		//	status |=ATBMWrite(0x26, 0x29, 0x01); //Disable TS delay
		}
	}
	else	
	{	  
		ui8SerialEnabled =0x0; /*parallel clock output */
		/*0: disable serial mode; 1:enable serial mode */	
		status |=ATBMWrite(0x18,0x01, ui8SerialEnabled);      		
		if(stTSMode.ui8TSSPIMSBSelection == TS_SPI_MSB_ON_DATA_BIT7)
		{
			status |=ATBMWrite(0x18, 0x04, 0x00); 
		}else
		{
			status |=ATBMWrite(0x18, 0x04, 0x01); 
		}
		if(ui8ChipID == 0xa0)
		{
			status |=ATBMWrite(0x00, 0x27, 0x03);
		}
		if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
		{
			status |=ATBMWrite(0x26, 0x27, 0x00);
		}
		status |=ATBMWrite(0x26, 0x29, 0x01);
	}
	return status;
}


ATBM_STATUS ATBMTSLockFlag(uint8 *LockStatus)
{
	/*
	Lock Flag indicate current lock status
	1, Must be locked, TS output or FEC locked in stable state.
	2, For occasional TS error, should give lock flag.
	*/
	ATBM_STATUS status = ATBM_SUCCESSFUL;
	uint8 u8TSLockFlag = 0;
	uint8 u8Iter = 0, u8lockedCount = 0;
	*LockStatus = 0;
	for(u8Iter = 0; u8Iter < 5; u8Iter++)
	{
		status |= ATBMRead(0x18, 0x2A, &u8TSLockFlag);
		if((u8TSLockFlag&0x01)&&(status == ATBM_SUCCESSFUL))
		{
			u8lockedCount = u8lockedCount + 1;
		}
	}
	//TS locked
	if(u8lockedCount >= 2)
	{
		*LockStatus = 1;		
	}
	return status;
}

#define  ATBMSUCCESS(status)   (status == ATBM_SUCCESSFUL)


void ATBMPrintStr(const char *StrType, const char *StrValue)
{
	ATBMDebugPrint ("%-20s = %s\r\n", StrType, StrValue);
}


ATBM_STATUS ATBMSetGPIOHighLow(ATBM_GPIO_PIN_TYPE gpio_pin, uint8 attribute)
{
	ATBM_STATUS status = ATBM_FAILED;

	return status;
}
/*


typedef struct ATBM_PIN_CONFIG_STRUCT
{
ATBM_GPIO_PIN_TYPE GPOPinStatus;    //0x0023 bit[7:4]  
ATBM_GPIO_PIN_TYPE GPOPinAgcDvbS;   //0x0021 bit[3:0] = 3
ATBM_GPIO_PIN_TYPE GPOPinRFAgcDvbT; //0x0021 bit[7:4] //Not used set 0
ATBM_GPIO_PIN_TYPE GPOPinAgcDvbT;   //0x0022 bit[3:0] = 1
ATBM_GPIO_PIN_TYPE GPO4PinStatus;   //0x0037 bit[7:4]
ATBM_GPIO_PIN_TYPE GPO5PinStatus;   //0x0037 bit[3:0]
}ATBM_PIN_CONFIG_T;
*/

ATBM_STATUS ATBMSetGpioPins(void)
{
	ATBM_STATUS status;
	uint8 Reg21Value = 0, Reg22Value = 0, Reg23Value = 0, Reg37Value =0, ui8ChipID;
	ATBMRead(0x00, 0x00, &ui8ChipID);
	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS2_MODE))
	{
		Reg21Value |= 0x3;  //DVB-S/S2 IF AGC
	}

	//RF AGC usually not used. the pin can be used as status indicator if not used as RF AGC.
	switch(gstATBMDvbConfig.GpioPinConfigs.GPOPinRFAgcDvbT)
	{
	case ATBM_PIN_TS_ERROR:
		Reg21Value|= 0x70;
		break;
	case ATBM_PIN_TS_LOCK:
		Reg21Value|= 0x80;
		break;
	case ATBM_PIN_FEC_ERROR:
		Reg21Value|= 0x90;
		break;
	case ATBM_PIN_FEC_LOCK:
		Reg21Value|= 0xa0;
		break;
	default:
		break;
	}
	status = ATBMWrite(0x00, 0x21, Reg21Value); 

	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBT2_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBT_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType  == ATBM_DVBC_MODE))
	{
		Reg22Value = 0x01; //DVB-T/T2,DVB-C IF AGC
	}
	switch(gstATBMDvbConfig.GpioPinConfigs.GPO4PinStatus)
	{
	case ATBM_PIN_TS_ERROR:
		Reg37Value|= 0x70;
			break;
	case ATBM_PIN_TS_LOCK:
		Reg37Value|= 0x80;
			break;
	case ATBM_PIN_FEC_ERROR:
		Reg37Value|= 0x90;
			break;
	case ATBM_PIN_FEC_LOCK:
		Reg37Value|= 0xa0;
			break;
	default:
		break;
	}
	status |= ATBMWrite(0x00, 0x22, Reg22Value); 


	switch(gstATBMDvbConfig.GpioPinConfigs.GPO5PinStatus)
	{
	case ATBM_PIN_TS_ERROR:
		Reg37Value|= 0x07;
		break;
	case ATBM_PIN_TS_LOCK:
		Reg37Value|= 0x08;
		break;
	case ATBM_PIN_FEC_ERROR:
		Reg37Value|= 0x09;
		break;
	case ATBM_PIN_FEC_LOCK:
		Reg37Value|= 0x0A;
		break;
	default:
		break;
	}
      status |= ATBMWrite(0x00, 0x37, Reg37Value); 
	switch(gstATBMDvbConfig.GpioPinConfigs.GPOPinStatus)
	{
	case ATBM_PIN_TS_ERROR:
		Reg23Value|= 0x70;
		break;
	case ATBM_PIN_TS_LOCK:
		Reg23Value|= 0x80;
		break;
	case ATBM_PIN_FEC_ERROR:
		Reg23Value|= 0x90;
		break;
	case ATBM_PIN_FEC_LOCK:
		Reg23Value|= 0xa0;
		break;
	default:
		break;
	}
	status |= ATBMWrite(0x00, 0x23, Reg23Value); 
	return status;
}



ATBM_STATUS ATBMPowerOnInit()
{
	ATBM_STATUS status = ATBM_SUCCESSFUL;

	//Demodulator TS interface  setting	
	gstATBMDvbConfig.stMPEGTSMode.ui8TSTransferType           = TS_SERIAL_MODE;	  
	gstATBMDvbConfig.stMPEGTSMode.ui8OutputEdge               = TS_OUTPUT_FALLING_EDGE;    
	gstATBMDvbConfig.stMPEGTSMode.ui8SPIClockConstantOutput   = TS_CLOCK_VALID_OUTPUT;
	gstATBMDvbConfig.stMPEGTSMode.ui8TSSPIMSBSelection        = TS_SPI_MSB_ON_DATA_BIT7;
	gstATBMDvbConfig.stMPEGTSMode.ui8TSSSIOutputSelection     = TS_SSI_OUTPUT_ON_DATA_BIT0;

    //Set demodulator PIN for AGC and status indicator
    gstATBMDvbConfig.GpioPinConfigs.GPOPinAgcDvbT   = ATBM_PIN_IF_AGC_T; //IF AGC used for DVB/T/T2 DVB-C
    gstATBMDvbConfig.GpioPinConfigs.GPOPinAgcDvbS   = ATBM_PIN_IF_AGC_S; //IF AGC used for DVB-S/S2
    gstATBMDvbConfig.GpioPinConfigs.GPOPinRFAgcDvbT = ATBM_PIN_RESERVED; 
    gstATBMDvbConfig.GpioPinConfigs.GPOPinStatus    = ATBM_PIN_RESERVED;    //Can be used as ATBM_PIN_FEC_LOCK, if not used set as reserved
	gstATBMDvbConfig.GpioPinConfigs.GPO4PinStatus   = ATBM_PIN_RESERVED;
	gstATBMDvbConfig.GpioPinConfigs.GPO5PinStatus   = ATBM_PIN_RESERVED;
// 	//Demodulator DVB-T/T2 setting
	gstATBMDvbConfig.DvbTxTunerInput.ui8IQSwapMode = INPUT_IQ_SWAP; 
	gstATBMDvbConfig.DvbTxTunerInput.u32IFFrequencyKHz = 5000; //5MHz, Here the unit is KHz,  Should be equal to the input of Tuner IF frequency.
	gstATBMDvbConfig.DvbTxTunerInput.ui8BandwidthType = ATBM_DVBT_BANDWIDTH_8MHZ;

	//Demodulator DVB-C setting
	gstATBMDvbConfig.DvbCTunerInput.ui8IQSwapMode = INPUT_IQ_SWAP;
	gstATBMDvbConfig.DvbCTunerInput.u32IFFrequencyKHz = 5000; //5MHz,  Here the unit is KHz,  Should be equal to the input of Tuner IF frequency.
	gstATBMDvbConfig.DvbCTunerInput.u32SymbolRateK = 6875;  //6.875MsymbolPerSeconds,This value is not required to be accurate, Demodulator Auto detection.

	//Demodulator DVB-S/S2 setting
	gstATBMDvbConfig.DvbSxTunerInput.ui8IQSwapMode = INPUT_IQ_SWAP;
	gstATBMDvbConfig.DvbSxTunerInput.ui32SymbolRateK = 27000; //Actually not used, demodulator auto detection.

	//Demodulator Clock Setting           
	gstATBMDvbConfig.ui32DemodClkKHz = 24000; //KHz, 24MHz, or 30400 for 30.4MHz

// 	ATBMSetSysClkClock();

	return status;
}

ATBM_STATUS ATBMStandBy()
{
	ATBM_STATUS ui8Status = ATBM_SUCCESSFUL;
 /*
	    0 5 1    //DSP Reset, TS FEC will not be locked(FAKE detection).
		1 c 0    //I2C on crystal
		6 1c 1   //shut down s/s2 ADC
		6 0  2   //shut down t/t2, C ADC
		6 5 12   //RSSI ADC 
		6 8 1    //PLL
*/
    uint8 u8SAdc = 0, u8TAdc = 0, u8RssiAdc = 0, u8PLL = 0;

	ui8Status |= ATBMWrite(0x00, 0x05, 0x01); //DSP Reset, TS FEC will not be locked.
	ui8Status |= ATBMWrite(0x01, 0x0C, 0x00); //I2C on crystal

	ui8Status |= ATBMWrite(0x26, 0x25, 0x00); //TS data OE
	ui8Status |= ATBMWrite(0x26, 0x26, 0x00); //TS sync OE

        ui8Status |= ATBMRead(0x06, 0x1c,&u8SAdc);
	 ui8Status |= ATBMRead(0x06, 0x00,&u8TAdc);
	 ui8Status |= ATBMRead(0x06, 0x05,&u8RssiAdc);
	 ui8Status |= ATBMRead(0x06, 0x08,&u8PLL);

     u8SAdc|= 0x01;     //Set bit0 = 1
     u8TAdc|= 0x02;     //Set bit1 = 1
     u8RssiAdc |= 0x02; //Set bit1 = 1
     u8PLL|= 0x01;      //Set bit0 = 1

	 ui8Status |= ATBMWrite(0x06, 0x1c,u8SAdc);
	 ui8Status |= ATBMWrite(0x06, 0x00,u8TAdc);
	 ui8Status |= ATBMWrite(0x06, 0x05,u8RssiAdc);
	 ui8Status |= ATBMWrite(0x06, 0x08,u8PLL);

	return ui8Status;
}

/********************************************************************************
function    : ATBMStandByWakeUp
parameter   :none 
return      :Operate  Status:ATBM_SUCCESSFUL is Normal and other is abnormal.
Description:
*********************************************************************************/
ATBM_STATUS  ATBMStandByWakeUp()
{
	ATBM_STATUS ui8Status = ATBM_SUCCESSFUL;

	/*
	//DVB-S/S2 WakeUp
	6 8 0   //bit 0 set 0 open PLL
	6 1c 0  //bit 0 set 0 open S ADC
	6 0  2  //bit 1 set 0 open T ADC
	6 5 10  //bit 1 set 0 open RSSI ADC
	1 c  1 
	0  5 0
	*/
	uint8 u8SAdc = 0, u8TAdc = 0, u8RssiAdc = 0, u8PLL = 0;

	ui8Status |= ATBMRead(0x06, 0x1c,&u8SAdc);
	ui8Status |= ATBMRead(0x06, 0x00,&u8TAdc);
	ui8Status |= ATBMRead(0x06, 0x05,&u8RssiAdc);
	ui8Status |= ATBMRead(0x06, 0x08,&u8PLL);

	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS2_MODE))
	{
		u8SAdc &= 0xFE;     //Set bit0 = 0 Open S S2 ADC
	}
	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBT_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBT2_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBC_MODE))
	{
		u8TAdc &= 0xFD;     //Set bit1 = 0 Open T T2 C ADC
	}
	//u8RssiAdc &= 0xFD;  //Set bit1 = 0, Currently RSSI ADC is not used.
	u8PLL &= 0xFE;;     //Set bit0 = 0
	ui8Status |= ATBMWrite(0x06, 0x08,u8PLL);
	ui8Status |= ATBMWrite(0x06, 0x1c,u8SAdc);
	ui8Status |= ATBMWrite(0x06, 0x00,u8TAdc);
	ui8Status |= ATBMWrite(0x06, 0x05,u8RssiAdc);
       ui8Status |= ATBMWrite(0x01, 0x0C, 0x01); //I2C on crystal
	ui8Status |= ATBMWrite(0x26, 0x25, 0xFF); //TS data OE
	ui8Status |= ATBMWrite(0x26, 0x26, 0x07); //TS sync OE
	ui8Status |= ATBMWrite(0x00, 0x05, 0x00); //DSP Reset, TS FEC will not be locked.

	return ui8Status;
}

ATBM_STATUS ATBMSusPend(void)
{
	ATBM_STATUS status = ATBM_SUCCESSFUL;

	uint8 u8CrystalPd = 0;
	status = ATBMStandBy();

	//After clock is shut,I2C is not work, Chip can only be started up by using reset PIN or Power Reset.
	status |= ATBMRead(0x06, 0x0E, &u8CrystalPd); 
	u8CrystalPd |= 0x01;
	ATBMWrite(0x06, 0x0E, u8CrystalPd); 

	return status;
}


ATBM_STATUS ATBMSetSysClkClockDVBTx(void)
{

	ATBM_STATUS ui8Status;
    uint8 ui8ChipID = 0, ui8AnaReg = 0;
	//I2C using Crystal clock
	ui8Status   =  ATBMWrite(0x01, 0x0c, 0x00);
	ATBMSleep(1); //Delay 1 millisecond.

    ui8Status |= ATBMRead(0x00, 0x00, &ui8ChipID);
	if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
	{

		ui8Status |=ATBMWrite(0x06, 0x07, 0x03);
		ui8Status |=ATBMWrite(0x06, 0x18, 0x01);
		ui8Status |=ATBMWrite(0x06, 0x06, 0x0c); //1008
		ui8Status |=ATBMWrite(0x06, 0x13, 0x02);
		ui8Status |=ATBMWrite(0x06, 0x0B, 0x02); 
		ui8Status |=ATBMWrite(0x06, 0x0A, 0x0b); //TS
		ui8Status |=ATBMWrite(0x06, 0x1C, 0x41);
	    ui8Status |=ATBMWrite(0x06, 0x21, 0x73);
	}
	if(ui8ChipID == 0xa2)
	{
		ui8Status |= ATBMRead(0x06, 0x1A, &ui8AnaReg);
		ui8AnaReg = ui8AnaReg + 58; //0x40 - 6
		ui8Status |= ATBMWrite(0x06, 0x19, ui8AnaReg);
	}
	if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
	{
		ATBMSleep(1); //Delay 1 millisecond.	
		ui8Status  |= ATBMWrite(0x03, 0x06, 0x00);    // ADC 1x mode
		ui8Status  |= ATBMWrite(0x26, 0x29, 0x00); //bypass TS delay
	}
	return ui8Status;
}

ATBM_STATUS ATBMSetSysClkClockDVBSx(void)
{

	ATBM_STATUS ui8Status;
	uint8 ui8ChipID = 0;

	//I2C using Crystal clock
	ui8Status   =  ATBMWrite(0x01, 0x0c, 0x00);
	ui8Status |= ATBMRead(0x00, 0x00, &ui8ChipID);
	ATBMSleep(1); //Delay 1 millisecond.	
	if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))//
	{

		ui8Status |=ATBMWrite(0x06, 0x07, 0x03);
		ui8Status |=ATBMWrite(0x06, 0x18, 0x03); 
		ui8Status |=ATBMWrite(0x06, 0x06, 0x06); // (3+2) * (6+2)  * 24 = 960 
		ui8Status |=ATBMWrite(0x06, 0x13, 0x02);
              ui8Status |=ATBMWrite(0x06, 0x0b, 0x02);  //sys 
		ui8Status |=ATBMWrite(0x06, 0x0a, 0x06); //TS 120M
		ui8Status |=ATBMWrite(0x06, 0x21, 0x73);
		ui8Status |=ATBMWrite(0x06, 0x10, 0x02); //ADC
		ui8Status |=ATBMWrite(0x06, 0x20, 0x22); //ADC
		ui8Status |=ATBMWrite(0x06, 0x1C, 0x00);
		ui8Status |=ATBMWrite(0x06, 0x05, 0x10);
		ui8Status |=ATBMWrite(0x06, 0x23, 0x03);
	}

	ATBMSleep(1); //Delay 1 millisecond.	
	return ui8Status;
}


ATBM_STATUS ATBMSetSysClkClockDVBC(void)
{

	ATBM_STATUS ui8Status = ATBM_SUCCESSFUL;
	uint8 ui8ChipID = 0, ui8AnaReg = 0;
	//I2C using Crystal clock
	ui8Status   =  ATBMWrite(0x01, 0x0c, 0x00);
	ATBMSleep(1); //Delay 1 millisecond.

	ui8Status |= ATBMRead(0x00, 0x00, &ui8ChipID);
	if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
	{
		//DVB-C mode for 24MHz Crystal, System clock=72MHz
		//ui8Status  |= ATBMWrite(0x00, 0x19, 0x01);	//
		//ui8Status  |= ATBMWrite(0x06, 0x06, 0x34);	//FB_div=52
		//ui8Status  |= ATBMWrite(0x06, 0x0a, 0x12);	
		//ui8Status  |= ATBMWrite(0x06, 0x0B, 0x07);	//SYS_DIV=7
		//ui8Status  |= ATBMWrite(0x06, 0x0C, 0x19);	//DVIP_div=25
	    //ui8Status  |= ATBMWrite(0x06, 0x1C, 0x41);	//change ADC clock to OSC clock
		//ui8Status  |= ATBMWrite(0x06, 0x21, 0x73);
		//ui8Status |= ATBMWrite(0x03, 0x06, 0x00);    // ADC 1x mode

		ui8Status |=ATBMWrite(0x06, 0x07, 0x03); 
		ui8Status |=ATBMWrite(0x06, 0x18, 0x01);
		ui8Status |=ATBMWrite(0x06, 0x06, 0x0c);
		ui8Status |=ATBMWrite(0x06, 0x13, 0x02);
		ui8Status |=ATBMWrite(0x06, 0x21, 0x73);
		ui8Status |=ATBMWrite(0x06, 0x0B, 0x05);
		ui8Status |=ATBMWrite(0x06, 0x0A, 0x10); //56MHz TS clock
		ui8Status |=ATBMWrite(0x06, 0x0C, 0x13);
		ui8Status |=ATBMWrite(0x06, 0x1C, 0x41);			
	}
	if(ui8ChipID == 0xa2)
	{
		ui8Status |= ATBMRead(0x06, 0x1A, &ui8AnaReg);
		ui8AnaReg = ui8AnaReg + 58; //0x40 - 6
		ui8Status |= ATBMWrite(0x06, 0x19, ui8AnaReg);
	}
	if((ui8ChipID == 0xa2)||(ui8ChipID == 0xa4))
	{
		ui8Status |=ATBMWrite(0x03, 0x06, 0x00);    // ADC 1x mode
	}
	return ui8Status;
}

//Not used
ATBM_STATUS  ATBMSetDvbBWSFI(uint32 u32FsADCKhz, uint32 u32InvsFsAdcValue)
{
	ATBM_STATUS  ui8Status;
	uint8  u8ParamsMinEst, u8ParamsMaxEst;
	uint32 u32FFT_Len, u32len;
	uint32 u32MinEnd, u32MaxEnd;
	uint32 u32CutPoints1 = 0, u32CutPoints2 = 0;

	uint32 if_points120m, if_points;
	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS2_MODE))
	{
		u32len = 4;
		u8ParamsMinEst = 1; //16/16;   //120M verified
		u8ParamsMaxEst = 58; // 0xe8/4;  //Should read from register or calculation
	}
	if(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBC_MODE)
	{
		u32len = 2;
		u8ParamsMinEst = 1; //16/16;     //24M verified
		u8ParamsMaxEst = 34; // 0x0x22/4; //read from register or calculation
	}

	u32FFT_Len = 65536;
	//	u32len = 4;

	u32MinEnd =  (u8ParamsMinEst*u32FFT_Len*1000)/(u32FsADCKhz*u32len);
	u32MaxEnd = (u8ParamsMaxEst*u32FFT_Len*1000)/(u32FsADCKhz*u32len*4) ;

	ui8Status = ATBMWrite(0x09,0xc3,(u32MinEnd&0xff)); 
	ui8Status |= ATBMWrite(0x09,0xc4,((u32MinEnd>>8)&0xff));  
	ui8Status |= ATBMWrite(0x09,0xc5,((u32MinEnd>>16)&0x01));  


	ui8Status = ATBMWrite(0x09,0xc6,(u32MaxEnd&0xff)); 
	ui8Status |= ATBMWrite(0x09,0xc7,((u32MaxEnd>>8)&0xff));  
	ui8Status |= ATBMWrite(0x09,0xc8,((u32MaxEnd>>16)&0x01));  

	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS2_MODE))
	{
	   u32CutPoints1 =  (0x7fff * u32InvsFsAdcValue)/69905; //120M u32InvsFsAdc is 69905
	   u32CutPoints2 =(0x7fff * u32InvsFsAdcValue)/69905;
	}

   if(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBC_MODE) 
   {
	   u32CutPoints1 =  (0x5aaa * u32InvsFsAdcValue)/0x15555; //24M DVBC u32InvsFsAdc is 0x15555
	   u32CutPoints2 =  (0x5aaa * u32InvsFsAdcValue)/0x15555;
   }

	if(u32CutPoints1>0x7fff)
	{
		u32CutPoints1 = 0x7fff;
	}
	if(u32CutPoints2>0x7fff)
	{
		u32CutPoints2 = 0x7fff;
	}
	ui8Status |= ATBMWrite(0x09,0x5f,(u32CutPoints1&0xff));  //params_max_rate_using_param_cut_points_1
	ui8Status |= ATBMWrite(0x09,0x60,((u32CutPoints1>>8)&0x7f));
	ui8Status |= ATBMWrite(0x09,0x61,(u32CutPoints2&0xff)); //params_max_rate_using_param_cut_points_2
	ui8Status |= ATBMWrite(0x09,0x62,((u32CutPoints2>>8)&0x7f));

	if((gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS_MODE)||(gstATBMDvbConfig.ui8ConfigDVBType == ATBM_DVBS2_MODE))
	{
		if_points120m = 0x4001;
		if_points = (if_points120m -1)*u32InvsFsAdcValue /69905 + 1; //120M u32InvsFsAdc is 69905
		if(if_points >0x7fff)
		{
			if_points = 0x7fff;
		}
		ui8Status |= ATBMWrite(0x09,0xb4,(if_points&0xff)); 
		ui8Status |= ATBMWrite(0x09,0xb5,((if_points>>8)&0x7f));
	}
	return ui8Status;
}


int ATBMInit(void)
{
	ATBMPowerOnInit();
 	ATBMSetDVBTxMode(8);  //5,6,7,8MHz Bandwidth
 	//ATBMSetDVBCMode(); //DVB-C symbol rate auto detection on 8MHz Bandwidth
       //ATBMSetDvbsxMode(); //DVB-S/S2 symbol rate auto detection on 8MHz Bandwidth
 	return 0;
}
