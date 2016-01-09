/***************************************************************************** 
Copyright 2012, AltoBeam Inc. All rights reserved.
     
File Name: AtbmDiseqcAPI.c
   
Version: 0.9
Released 2013-10-16

Description: Example programming interface for ATBM781x	

GPIO LNB out control, High, low (13, 18V for example)

Start and Stop continuous tone(0,22K tone On Off)
Send Tone Burst  (Tone burst A or Tone burst B)

DiSEqC 1.x 
Send DiSEqC only command
Send DiSEqC command with tone burst and continuous tone

DiSEqC 2.x 
Get DiSEqC RX bytes for DiSEqC 2.0 read required
******************************************************************************/
#include <stdio.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"

#include "AtbmDiseqcAPI.h"

#define ATBM_DISEQC_DEBUG 1

DISEQC_PARAM diseqc_params;
DISEQC_PARAMS_INPUT_T diseqc_params_in;
unsigned int diseqc_clock_khz = 60000;


ATBM_STATUS  ATBMReadRegister16(uint16 u16RegAddr, unsigned char *pData)
{
	ATBM_STATUS status;
	unsigned char u8BaseAddress, u8OffsetAddress;
	u8BaseAddress = u16RegAddr/256;
	u8OffsetAddress = u16RegAddr&0xff;

	status = ATBMRead(u8BaseAddress, u8OffsetAddress, pData);

	return status;
}

ATBM_STATUS ATBMWriteRegister16(uint16 u16RegAddr, uint8 data)
{
	ATBM_STATUS status;
	unsigned char u8BaseAddress, u8OffsetAddress;
	u8BaseAddress = u16RegAddr/256;
	u8OffsetAddress = u16RegAddr&0xff;

	status = ATBMWrite(u8BaseAddress, u8OffsetAddress, (uint8)(data));

	return status;
}

/*Set DiSEqC clock and NCO count*/
ATBM_STATUS ATBMDiseqcSetClock()
{
	ATBM_STATUS status = ATBM_FAILED;
	uint8 u8Iter = 0;
    uint8 u8DiseqcEn;
	unsigned int u32DiseqcSysClock = 60000;
  
	unsigned short u16ClkFreqIn;
	unsigned char u8ClkFreq[2], u8NcoCount = 0;

	//diseqc_params.diseqcModulation_e = DISEQC_22KHZ; //22K tone mode or Binary mode
	diseqc_params.out_params.diseqc_mod_e = DISEQC_ONLY;		
	diseqc_params.out_params.contToneVal = 0;
	diseqc_params.out_params.toneBurstVal = 0;
	diseqc_params.out_params.polarizationPortVal = FALSE;	/* not used port */

	diseqc_params.write_params.delayBeforeDiseqc = 15;		/* 15 ms */
	diseqc_params.write_params.delayBeforeCont = 15;		/* 15 ms */

	for (u8Iter = 0; u8Iter <  8; u8Iter++)	/* reset off TransmitByte[1]-TransmitByte[7]*/
	{ 
		diseqc_params.write_params.TransmitByte[u8Iter] = 0;
	}		

	u32DiseqcSysClock = u32DiseqcSysClock * 1000;

	u16ClkFreqIn = (unsigned short)(u32DiseqcSysClock*1.0/(22000*2) + 0.5);

	u8ClkFreq[0] = (uint8)(u16ClkFreqIn&0xff);
	u8ClkFreq[1] = (uint8)(u16ClkFreqIn/256);

	u8NcoCount = (unsigned char)(u32DiseqcSysClock*1.0/(22000*64) + 0.5);

	/*Set DiSEqC clock and NCO count*/
	status = ATBMWriteRegister16(0x6605, u8ClkFreq[0]);

	if(status == ATBM_SUCCESSFUL)
	{
		status =   ATBMWriteRegister16(0x6606, u8ClkFreq[1]);
	}
	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMWriteRegister16(0x6608, u8NcoCount);
	}
	status |= ATBMRead(0x06, 0x23, &u8DiseqcEn);
	u8DiseqcEn|=0x03;
	status |= ATBMWrite(0x06, 0x23, u8DiseqcEn);
	return status;
}

/******************************************************************************
ATBMGetDiseqcBusyInfo												
This function checks and sets the DisqcBusy flag, If the DisqcBusy is set true
no further DiSEqC transmission is allowed,   This function has to be used 
after every execution of a DiSEqC transmission, in order to see if the next 
operation like switch on continuous tone or switch on tone burst is allowed.					 			
******************************************************************************/
ATBM_STATUS ATBMGetDiseqcBusyInfo ( BOOL * DisqcBusy)
{
	uint32 status;

	uint8	u8Data;

	/* check if last DiSEqC Transmission is still busy bit0 */
	status = ATBMReadRegister16(0x6623, &u8Data);

	if(ATBM_SUCCESSFUL == status)
	{
		if (u8Data &= 0x01) /* if bit set 1 DiSEqC interface not busy */
			*DisqcBusy = FALSE;
		else 
			*DisqcBusy = TRUE;	/* DiSEqC interface is busy */
	}
	else
	{
		*DisqcBusy = TRUE;	/*Assuming DiSEqC is busy as we don't know the state*/
	}
	return (status);
}


/*Make sure correct clock is set and Check DiSEqC state */
ATBM_STATUS ATBMCheckDiSEqCState()
{
	ATBM_STATUS status = ATBM_FAILED;
	uint32 U32TryCount = 0;
	BOOL pbDisqcBusy;

	/* DiSEqC clock initialization. */
	status = ATBMDiseqcSetClock();
	if(status != ATBM_SUCCESSFUL)
		return status;

	/*
	checking DiSEqC state, and waiting when busy detected until DiSEqC is in free state for further operation until timeout
	*/
	do
	{
		status = ATBMGetDiseqcBusyInfo(&pbDisqcBusy);
	}while ((TRUE == pbDisqcBusy) && (++U32TryCount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL== status));	

	if((status != ATBM_SUCCESSFUL)||(TRUE == pbDisqcBusy))
	{
		ATBMDebugPrint("DiSEqC state Check failed\n");
		status = ATBM_FAILED;
	}
	return status;

}

/*********************************************************************************************
ATBMWriteDiseqcBuffer
Writes in pstDiseqcWrite.NoOfBytesToSend defined number of bytes into DiSEqC send buffer. 						
The following Send DiSEqC command uses these	Bytes from the Buffer as message content				
********************************************************************************************/
ATBM_STATUS ATBMWriteDiseqcBuffer ( DISEQC_WRITE_PARAM *pstDiseqcWrite)
{
	uint8 u8Iter = 0;
	uint8 u8Data, u8Tmp;
	uint32 statusCmd = ATBM_SUCCESSFUL;
	ATBM_STATUS status = ATBM_SUCCESSFUL;
	uint16 u16Address = 0x6609;

	if ((pstDiseqcWrite->NoOfBytesToSend < 0x01)  || (pstDiseqcWrite->NoOfBytesToSend > 0x08))
	{	
		/*data length should not be less than 1 or more than 8 */
		status = ATBM_FAILED;
		ATBMDebugPrint("Error in ATBM_WriteDiseqcBuffer failed as number of DiSEqC Bytes out of valid range\n");  
	}
	else
	{
		for (u8Iter = 0; (u8Iter<pstDiseqcWrite->NoOfBytesToSend) && (ATBM_SUCCESSFUL == statusCmd); u8Iter++)
		{	/* write data bytes into DiSEqC buffer */
			statusCmd = ATBMWriteRegister16( (uint16) (u16Address + u8Iter), pstDiseqcWrite->TransmitByte[u8Iter]);
		}

		if(ATBM_SUCCESSFUL == statusCmd)
		{
			statusCmd = ATBMReadRegister16( 0x6601, &u8Data);
			if(ATBM_SUCCESSFUL == statusCmd)
			{
				u8Tmp  = pstDiseqcWrite->NoOfBytesToSend;
				u8Tmp  = u8Tmp&0x0F;
				u8Data &=0xf0;
				u8Data |= u8Tmp;
				statusCmd = ATBMWriteRegister16( 0x6601, u8Data);
			}
		}
	}

	/* Check status for both DiSEqC parameters and I2C operations*/
	if((ATBM_SUCCESSFUL == statusCmd) && (ATBM_SUCCESSFUL == status))
	{
		status = ATBM_SUCCESSFUL;
	}	
	else
	{
		status = ATBM_FAILED;	
		ATBMDebugPrint("Error in ATBMWriteDiseqcBuffer: failed\n");       
	}	
	return (status);
}


/*
Trigger a DiSEqC command or Tone burst after DiSEqC registers configure done.
This is an internal function.
*/
ATBM_STATUS ATBMTriggerCommand(DISEQC_PARAM* pstDiseqcParams)
{
	uint8 u8Data = 0; 
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMReadRegister16(0x6600, &u8Data);

	if(status == ATBM_SUCCESSFUL)
	{
		u8Data|= 0x04; //set bit[2] 1 trigger a transmission to occur
		status = ATBMWriteRegister16( 0x6600, u8Data);
		ATBMSleep(5);
		u8Data &= 0xFB; //clear bit[2] 0 trigger a transmission to end
		status = ATBMWriteRegister16( 0x6600, u8Data);
	}
	ATBMSleep(20);
	if(pstDiseqcParams->write_params.NoOfBytesToSend > 0)
	{
		ATBMSleep(60); /*DiSEqC write command takes typically 54ms */	
	}
	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Error trigger DiSEqC operation\n");
	}
	return status;
}

/***************************************************************************************************
ATBMSwitchOffContTone
Switched Continuous Tone value to zero, but still enabled						

in case of 22kHz or binary by writing 0 in bit3 of addr. 0x6600		
in case of binary inverted, continuous value high			 											
***************************************************************************************************/
ATBM_STATUS ATBMSwitchOffContTone( DISEQC_PARAM * pstDiseqcParams)
{
	uint32 statusCmd;
	ATBM_STATUS status;
	uint8	u8Data;

	statusCmd = ATBMReadRegister16( 0x6600, &u8Data);
	if (pstDiseqcParams->diseqcModulation_e != DISEQC_BIN_INVERTED)
	{
		u8Data |= 0x08;	/* enables cont tone */
		u8Data &= 0xEF;	/* reset bit 4 (continuous tone value = 0, but enabled) */
	}
	else
	{
		u8Data |= 0x18;	/* enable cont tone and sets cont tone on, as polarity is inverted, 1 means 0 */
	}
	if (ATBM_SUCCESSFUL == statusCmd)
		statusCmd = ATBMWriteRegister16( 0x6600, u8Data); 


	/* Check status */
	if(ATBM_SUCCESSFUL == statusCmd)
	{
		status = ATBM_SUCCESSFUL;
	}
	else
	{
		status = ATBM_FAILED;
#if ATBM_DISEQC_DEBUG
		ATBMDebugPrint("ATBMSwitchOffContTone: failed\n");
#endif
	}
	return (status);

} /* End of ATBMSwitchOffContTone */

/******************************************************************************************************
ATBMDisableBurstContOff		
Disables Tone burst, Tone Burst controlled by bit0 = 0 of 0x6600						
switches off (but enabled) continuous tone in case of 22kHz or binary by writing 0 in bit3 of 0x6600		
in case of binary inverted, disable burst tone switch on the cont	
******************************************************************************************************/
ATBM_STATUS ATBMDisableBurstContOff( DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status;
	uint8	u8Data;

	status = ATBMReadRegister16( 0x6600, &u8Data);
	if (pstDiseqcParams->diseqcModulation_e != DISEQC_BIN_INVERTED)
	{
		u8Data |= 0x08;	/* enables cont tone */
		u8Data &= 0xEE;	/* reset bit4 (continuous tone off) and bit0 (Tone burst disabled) */
	}
	else
	{
		u8Data |= 0x18;	/* set continuous tone */
		u8Data &= 0xFE;	/* tone burst disable */
	}
	if (ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16( 0x6600, u8Data);

	if(status != ATBM_SUCCESSFUL)
	{
		status = ATBM_FAILED;
	}
	return (status);
} /* End of ATBMDisableBurstContOff */


/******************************************************************************
ATBMDisableContAndBurst			
Disables continuous tone and tone burst		
sets both values to zero	 
reset also DiSEqC send bit.																										
******************************************************************************/
ATBM_STATUS ATBMDisableContAndBurst ( )
{
	ATBM_STATUS status;
	uint8	u8Data;

	status = ATBMReadRegister16( 0x6600, &u8Data);

	u8Data &= 0xE0;	/* reset also DiSEqC send bit.*/
	if (ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16( 0x6600, u8Data);

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{
		ATBMDebugPrint("DisableContAndBurst: failed\n");
	}
	return (status);
} /* End of ATBMDisableContAndBurst */


/*******************************************************************************************************
ATBMToneburstValueSet     
Generate tone burst without a DiSEqC message before.				
uses a zero byte DiSEqC message followed by tone burst value low																															
only used inside the DiSEqC read command				
*******************************************************************************************************/
ATBM_STATUS ATBMToneburstValueSet(DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status;
	uint8	u8Data;		/* data byte send/receive via I2C */

	status = ATBMReadRegister16( 0x6600, &u8Data);
	if(ATBM_SUCCESSFUL == status)
	{

		if(pstDiseqcParams->out_params.toneBurstVal == 0)
		{
			u8Data |= 0x01;	/* enable tone burst */
			u8Data &= 0xFD;	/* set tone burst value 0*/
		}

		if(pstDiseqcParams->out_params.toneBurstVal == 1)
		{
			u8Data |= 0x03;	/* enable tone burst 1 */	
		}
		status = ATBMWriteRegister16( 0x6600, u8Data);
	}

	/* check one time, if writing is allowed (IC not busy)  */
	if(ATBM_SUCCESSFUL == status)
	{
		status = ATBMReadRegister16( 0x6623, &u8Data);
	}
	if ((u8Data & 0x01) && (ATBM_SUCCESSFUL == status)) /* bit0 = 1  DiSEqC NOT busy (bitwise and) */
	{
		status = ATBMTriggerCommand(pstDiseqcParams);
	}

	if(ATBM_SUCCESSFUL != status)
	{
		status = ATBM_FAILED;
#if ATBM_DISEQC_DEBUG
		ATBMDebugPrint("ATBMToneburstValueSet: DiSEqC is still busy, no new transmission allowed\n"); 
#endif
	}

	return (status);
}/* End of ATBMToneburstValueSet */

/******************************************************************************************************
ATBMSetToneburst				
Sets the tone burst value according diseqcOutParams->toneBurstVal		
enables Tone burst, the tone burst need be triggered just like DiSEqC command.
and it can be send following DiSEqC command.
******************************************************************************************************/
ATBM_STATUS ATBMSetToneburst(  DISEQC_OUT_PARAM *diseqcOutParams)
{
	ATBM_STATUS status;
	uint8	u8Data;

	status = ATBMReadRegister16( 0x6600, &u8Data);
	u8Data |= 0x01;	/*set bit 0 (Tone burst enable) */

	/* set correct value */
	if  (TRUE == diseqcOutParams->toneBurstVal)
		u8Data |= 0x02;	/* set bit 1 (Tone burst val = 1) */
	else
		u8Data &= 0xFD;	/* reset bit 1 (Tone burst val= 0) */

	if (ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16( 0x6600, u8Data);

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{
		status = ATBM_FAILED;
#if ATBM_DISEQC_DEBUG
		ATBMDebugPrint("ATBMSetToneburst: failed\n");
#endif
	}
	return (status);

} /* End of ATBMSetToneburst */

/******************************************************************************************
ATBMDisableToneburst		         	
disables Tone burst by writing 0 in bit0 of 0x6600	
******************************************************************************************/
ATBM_STATUS ATBMDisableToneburst ()
{
	ATBM_STATUS status;
	uint8	u8Data;

	status = ATBMReadRegister16( 0x6600, &u8Data);
	u8Data &= 0xFE;	/*reset bit 0 (Tone burst disabled) */

	if (ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16( 0x6600, u8Data);

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{
		ATBMDebugPrint("ATBM DisableToneburst failed\n");
	}
	return (status);

} /* End of ATBMDisableToneburst */


/******************************************************************************************
ATBMDefaultsDiseqcReadParameters
Set DiSEqC with default options. This is an internal function.
******************************************************************************************/
ATBM_STATUS ATBMDefaultsDiseqcReadParameters(DISEQC_READ_PARAM* pstDiseqcReadParam, DISEQC_REPLY_INFO* pstDiseqcReplyInfo)
{
	uint8 u8Iter = 0;
	ATBM_STATUS status = ATBM_FAILED;

	pstDiseqcReadParam->DirPinByDiseqc = TRUE;		/* Pin controlled by DiSEqC */
	pstDiseqcReadParam->DirPinPolarity = TRUE;		/* high during receive state */
	pstDiseqcReadParam->MagThreshold = 0x20;		/* threshold for decoding */
	pstDiseqcReadParam->binaryModulation = FALSE;		/* binary input instead of 22kHz */
	pstDiseqcReadParam->readBinInvert = FALSE;
	pstDiseqcReadParam->NoOfBytesToRead = 8;            /* default maxim possible value */
	/*Reset the DiSEqC received values in the structure */
	for (u8Iter = 0; u8Iter < 8; u8Iter++)
	{ 
		pstDiseqcReplyInfo->receivedBytes[u8Iter] = 0; 
	}
	pstDiseqcReplyInfo->parityError = FALSE;
	pstDiseqcReplyInfo->receivedBytesCount = 0;

	status = ATBM_SUCCESSFUL;

	return status;
}

/****************************************************************************************************** 
ATBMDiseqcReadInit 														
set the 22kHz demodulation oscillator	 								
set the DC Threshold for input signal detection of DiSEqC reply		
set the 22kHz reply decoding oscillator frequency						
set the DiSEqC read pin functionality										
This is an internal function.                                                                                                                                              
******************************************************************************************************/
ATBM_STATUS ATBMDiseqcReadInit(DISEQC_READ_PARAM* pstDiseqcReadParam)
{	
	ATBM_STATUS status; 
	uint8 u8Data;

	/* set DiSEqC input signal modulation */
	status = ATBMReadRegister16(0x6600, &u8Data);
	if (ATBM_SUCCESSFUL == status)
	{
		if (pstDiseqcReadParam->binaryModulation == TRUE)	
		{	 /* DiSEqC input gets demodulated signal */
			u8Data &= 0xDF;		/* reset bit 5 */	/*bit 5(0: demodulated signal in, 1: tone in)*/
		}
		else
		{	/* DiSEqC input gets 22kHz-modulated signal, directly from tuner */
			u8Data |= 0x20;		/* set bit 5 */	
		}
		u8Data |= 0x40;		/* set bit 6 */	
		status = ATBMWriteRegister16(0x6600, u8Data);	/* no write check */
	}

	if (ATBM_SUCCESSFUL == status)
	{	
		status = ATBMReadRegister16(0x6602, &u8Data); 
	}
	if (ATBM_SUCCESSFUL == status)
	{
		/* V2 Option */
		if (pstDiseqcReadParam->readBinInvert == TRUE)	
		{	
			if (pstDiseqcReadParam->binaryModulation == TRUE) /* DiSEqC input gets demodulated signal, e.g. from LNBH21 */
				u8Data |= 0x40;		/* set bit 6 */
			else
				u8Data &= 0xBF;		/* reset bit 6: DiSEqC gets not inverted input signal */
		}
		else
		{	/* DiSEqC input gets not inverted DiSEqC signal,e.g. directly from tuner */
			u8Data &= 0xBF;		/* reset bit 6 */
		}
		/* End V2 Option */

		if (pstDiseqcReadParam->DirPinByDiseqc == TRUE)	
		{	 /* Pin is controlled by DiSEqC */
			u8Data &= 0xDF;		/* reset bit 5 */	
		}
		else
		{	/* Pin is controlled by pstDiseqcReadParam->DirPinPolarity */
			u8Data |= 0x20;		/* set bit 5 */	
		}

		if (pstDiseqcReadParam->DirPinPolarity == TRUE)	
		{	 
			u8Data &= 0xEF;		/* reset to 0 bit 4 */
		}
		else
		{	/* Pin is controlled by pstDiseqcReadParam->DirPinPolarity */
			u8Data |= 0x10;		/* set 1 bit 4 */	
		}
		status = ATBMWriteRegister16( 0x6602, u8Data); 
	}

	/* set bit duration to distinguish between 0 and 1 for decoding */	
	if (ATBM_SUCCESSFUL == status)
	{ 
		status = ATBMReadRegister16( 0x6613, &u8Data); 

		/* set to 0x0B */
		u8Data &=0x07;
		u8Data |=0x58; //related with DiSEqC clock.

		/* data = (data << 3)& 0xF8;	 always bit0..2 = 0 estimated */
		status = ATBMWriteRegister16( 0x6613, u8Data);
	}

	/* set MagThreshold for reply demodulation: */
	if (ATBM_SUCCESSFUL == status)
	{
		u8Data = pstDiseqcReadParam->MagThreshold;		
		status = ATBMWriteRegister16( 0x6607, u8Data);
	}	

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{	
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBM_DiseqcReadInit: failed\n"); 
#endif
	}
	return (status);
}/* End of ATBMDiseqcReadInit */


/****************************************************************************************** 
ATBMDiseqcModulationSet	
writes into register to set selected mode:	
22kHz
Binary or Binary Inverted	
Only set DiSEqC out mode
******************************************************************************************/
ATBM_STATUS ATBMDiseqcModulationSet(  DISEQC_PARAM* pstDiseqcParams)
{	
	ATBM_STATUS status= ATBM_SUCCESSFUL; 

	uint8 u8Data; 

	status = ATBMReadRegister16( 0x6601, &u8Data);
	switch (pstDiseqcParams->diseqcModulation_e)
	{
	case DISEQC_22KHZ:
		{
			u8Data |=0x10;	/* bit4=1 sets 22kHz */
			u8Data &=0xDF;	/* bit5=0 sets not inverted */
			break;
		}
	case DISEQC_BIN:
		{
			u8Data &=0xCF;	/* bit4=0 sets binary and bit5=0 not inverted */
			break;
		}
	case DISEQC_BIN_INVERTED:
		{
			u8Data &=0xEF;	/* bit4=0 sets binary */
			u8Data |=0x20;	/* bit5=1 sets inverted */
			break;
		}
	default:
		{
			status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
			ATBMDebugPrint("Error in ATBM_DiseqcModulationInit: no valid DiSEqC modulation mode\n");  
#endif
			break;
		}
	} /* mode switch end */
	if (ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16(0x6601, u8Data);

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBMDiseqcModulationSet\n"); 
#endif
	}
	return (status);
}/* End of ATBMDiseqcModulationSet */


/*********************************************************************************************
ATBMDiseqcDelaySet
sets delay to value defined in struct DISEQC_WRITE_PARAM				
if delay greater then max value: forced to max value and set ATBM_FAILED																		
*********************************************************************************************/
ATBM_STATUS ATBMDiseqcDelaySet (  DISEQC_WRITE_PARAM* pstDiseqcWrite)
{
	uint8	u8Data;
	ATBM_STATUS status;

	/*delays in ms (default for both: 15ms)*/
	/* delay1 max 90 ms */
	if (pstDiseqcWrite->delayBeforeDiseqc>90)
	{
		u8Data = 0xFF;
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBMDiseqcDelaySet: delayBeforeDiseqc value out of limit"); 
#endif
	}
	else
	{
		/* rounded (+5/10) and two digit (+20/10) added to the register value */
		u8Data = (uint8)(((float)(pstDiseqcWrite->delayBeforeDiseqc) * 27.5 + 25.0)/10.0);
	}
	status = ATBMWriteRegister16( 0x6603, u8Data);	 

	/* delay2 max 350ms(242 = 0xF2, real max 370ms) */
	if (pstDiseqcWrite->delayBeforeCont>350)
	{
		u8Data = 0xFF;
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBMDiseqcDelaySet: delayBeforeCont value out of limit\n"); 
#endif
	}
	else
	{
		/* rounded (+5/10) and one digit (+10/10) added to the register value */
		u8Data = (uint8)(((float)(pstDiseqcWrite->delayBeforeCont) * 6.875 +15.0)/10.0);
	}
	if(ATBM_SUCCESSFUL == status)
		status = ATBMWriteRegister16( 0x6604, u8Data);

	/* Check status */
	if(ATBM_SUCCESSFUL != status) 
	{

#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBM_DiseqcDelayInit\n");  
#endif
	}
	return (status);
} /* End of ATBMDiseqcDelaySet */



/***************************************************************************************
ATBMDiseqcSet					
Configures the DiSEqC settings                   
Calls the following subroutines:				
ATBMDisableContAndBurst								   
ATBMDiseqcModulationSet								
ATBMDiseqcDelaySet										
failed if one of the three functions failed						
***************************************************************************************/
ATBM_STATUS ATBMDiseqcSet(DISEQC_PARAM* pstDiseqcParams)
{	/* DISEQC_MODULATION_MOD_E * pstDisqcModulation */
	ATBM_STATUS status; 

	status = ATBMDisableContAndBurst();

	if (ATBM_SUCCESSFUL == status)
		status = ATBMDiseqcModulationSet(pstDiseqcParams);

	if (ATBM_SUCCESSFUL == status)
		status	= ATBMDiseqcDelaySet(&pstDiseqcParams->write_params);

	if(ATBM_SUCCESSFUL != status) 
	{	
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBM_DiseqcDelayInit failed\n");   
#endif
	}
	return(status);
}/* End of ATBMDiseqcSet */



/*********************************************************************************
ATBMSetAnalogueDishTone:		     
Check DiSEqC status, if busy waits until DiSEqC is ready, 
then	sets cont tone value and enables continuous tone										
*********************************************************************************/
ATBM_STATUS ATBMSetAnalogueDishTone(DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status = ATBM_SUCCESSFUL;
	BOOL pbDisqcBusy=FALSE;			/* if the  pbDisqcBusy is TRUE, no further DiSEqC transmission is allowed */
	uint32 u8TryCount = 0;
	uint8 u8Data;

	status = ATBMReadRegister16( 0x6600, &u8Data);
	u8Data |= 0x08;			/* Set bit3 to enable continuous tone */
	switch (pstDiseqcParams->diseqcModulation_e)
	{
	case DISEQC_22KHZ:
	case DISEQC_BIN:
		{
			if (TRUE == pstDiseqcParams->out_params.contToneVal)
				u8Data |= 0x10;	/*Set bit 4 (Cont_tone val= 1)*/
			else
				u8Data &= 0xEF;	/*Reset bit 4 (Cont_tone val= 0)*/	
			break;
		}
	case DISEQC_BIN_INVERTED:
		{
			if (TRUE == pstDiseqcParams->out_params.contToneVal)
				u8Data &= 0xEF;	/* reset bit 4 (Cont_tone val= 0)*/	
			else
				u8Data |= 0x10;	/* set bit 4 (Cont_tone val= 1)*/
			break;
		}
	default:	/* no valid DiSEqC modulation mode */
		{
			status = ATBM_FAILED;
			break;
		}
	}/*End switch diseqcModulation_e  */

	if(pstDiseqcParams->out_params.diseqc_mod_e !=CONT_TONE)
	{
		do		
		{
			status = ATBMGetDiseqcBusyInfo ( &pbDisqcBusy);
		}while ((TRUE == pbDisqcBusy) && (++u8TryCount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL== status));	
	}

	/*Set DiSEqC port value before continuous tone is set */ 
	if ((ATBM_SUCCESSFUL == status) && (FALSE == pbDisqcBusy))
	{
		status = ATBMWriteRegister16( 0x6600, u8Data);
	}

	/* Check status */
	if(ATBM_SUCCESSFUL != status)
	{
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBMSetAnalogueDishTone: No valid modulation mode or Acknowledge of Busy check\n");  
#endif
	}

	return (status);
}/* End ATBMSetAnalogueDish */


/******************************************************************************************	
ATBMDiseqcBackwardsCompatible
This subroutine is only necessary if you don't use the ATBMDiseqcSatControl.					
In this case you have decided to generate your	DiSEqC message under your full control					
In such a case, ATBMDiSEqCBackwardsCompatible	has to be called after the last DiSEqC	transmission
is finished  to set set the the continuous tone.																			
******************************************************************************************/
ATBM_STATUS ATBMDiseqcBackwardsCompatible( DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status = ATBM_SUCCESSFUL; /* don't change value */
	uint32 u8TryCount = 0;
	BOOL pbDisqcBusy;		/* if the pbDisqcBusy is TRUE no further DiSEqC transmission is allowed */

	switch (pstDiseqcParams->out_params.diseqc_mod_e)
	{
	case DISEQC_CONT_AND_BURST:
	case DISEQC_CONT:				/* set DiSEqC port and continuous tone value*/ 
		{
			status = ATBMSetAnalogueDishTone(pstDiseqcParams);		/* enables also cont tone */
			break;
		}
	case DISEQC_BURST:			
		{
			/* sets only DiSEqC port value: */ 
			pstDiseqcParams->out_params.diseqc_mod_e = ONLY_GPIO;
			/* to avoid GPIO setting before DiSEqC is finished */	
			do		
			{
				status = ATBMGetDiseqcBusyInfo (&pbDisqcBusy);
			}while ((TRUE == pbDisqcBusy) && (++u8TryCount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL== status));				
			pstDiseqcParams->out_params.diseqc_mod_e = DISEQC_BURST;
			break; /*this function only set continuous tone*/
		}
	case DISEQC_ONLY:
		break;
	case CONT_TONE:
	case ONLY_GPIO:
	default:
		{
			status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
			ATBMDebugPrint("Error in ATBMDiseqcBackwardsCompatible\n");   
#endif
			break;
		}
	}

	return(status);
} /* End of ATBMDiseqcBackwardsCompatible */


/*********************************************************************************************	
ATBMDiseqcSatControl									
It starts a DiSEqC single transmission  depending on the selected mode by the	
analogue settings like tone burst, continuous tone 																															
Take care
in case of any DiSEqC transmission, it is mandatory,								
that the commands you intend to send via DiSEqC, were first written			
by means of the subroutine ATBMWriteDiseqcBuffer in the DiSEqC				
buffer, before the ATBMDiseqcSatControl is called!	

TAKE CARE: ATBMWriteDiseqcBuffer must FIRST execute to set/define the transmitted data

*********************************************************************************************/
ATBM_STATUS ATBMDiseqcSatControl (DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status = ATBM_SUCCESSFUL; 
	DISEQC_MOD u8Mode = pstDiseqcParams->out_params.diseqc_mod_e;
	uint32 u8TryCount = 0;
	BOOL diseqcUsed;
	BOOL pbDisqcBusy;
	switch(u8Mode)
	{
	case ONLY_GPIO:
		{
			status = ATBM_SUCCESSFUL;
			diseqcUsed = FALSE;
			break;
		}
	case CONT_TONE:	/* only continuous tone */
		{
			status = ATBMSetAnalogueDishTone (pstDiseqcParams);
			if (ATBM_SUCCESSFUL == status)
				status = ATBMDisableToneburst();
			diseqcUsed = FALSE;
			break;
		}
		/* DiSEqC transmission requested modes are following: */
	case DISEQC_BURST:
	case DISEQC_CONT_AND_BURST:
		{
			diseqcUsed = TRUE;
			/* to avoid continuous tone is set before GPIO is set: */
			status = ATBMSwitchOffContTone (pstDiseqcParams);	
			/* to avoid tone burst before last DiSEqC is finished (in case of DiSEqC repeat) */	
			do		
			{
				status = ATBMGetDiseqcBusyInfo (&pbDisqcBusy);
			}while ((TRUE == pbDisqcBusy) && (++u8TryCount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL== status));	
			if ((ATBM_SUCCESSFUL== status) && (FALSE == pbDisqcBusy))
				status = ATBMSetToneburst (&(pstDiseqcParams->out_params));
			break;
		}
	case DISEQC_CONT:	/* DiSEqC with continuous tone and GPIO 3 */
	case DISEQC_ONLY:	/* no GPIO 3 for polarization */
		{	
			diseqcUsed = TRUE;
			status = ATBMDisableBurstContOff (pstDiseqcParams);
			break;
		}

	default:
		{
			diseqcUsed = FALSE;
			status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
			ATBMDebugPrint("Error in ATBMDiseqcSatControl: no valid DiSEqC mode\n");  
#endif
			break;
		}
	} /* mode switch end */

	if ((TRUE == diseqcUsed) && (ATBM_SUCCESSFUL == status))
	{
		/* in case of DiSEqC Repeat it makes no sense to write every times into buffer */
		/* status = ATBM_WriteDiseqcBuffer (pstNimParams, &pstDiseqcParams->write_params); 
		if (ATBM_SUCCESSFUL == status) */
		do		
		{
			status = ATBMGetDiseqcBusyInfo( &pbDisqcBusy);
		}while ((TRUE == pbDisqcBusy) && (++u8TryCount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL== status));

		if((FALSE == pbDisqcBusy)&&(diseqcUsed == TRUE))
			//	status = ATBMSendDiseqc (pstDiseqcParams);		
			status = ATBMTriggerCommand(pstDiseqcParams);
		else 
			status = ATBM_FAILED;

		if (ATBM_SUCCESSFUL == status)
			status = ATBMDiseqcBackwardsCompatible(pstDiseqcParams);
	}

	return (status);	/* exit subroutine */
} /* End ATBMDiseqcSatControl */




/******************************************************************************************
ATBMDiseqcReadRequest  
Is used to send a DiSEqC write message with reply required	
(DiSEqC command Framing data is 0xE2 or 0xE3		
1, check mode make sure is DiSEqC command mode
2, check DiSEqC command bytes length.
3, switch off tone burst and continuous tone
4, set reply request and time delay
5, send DiSEqC command
6, send Tone burst and continuous tone if required
7, read DiSEqC replied data.		

TAKE CARE:	ATBMWriteDiseqcBuffer must FIRST execute.
******************************************************************************************/
ATBM_STATUS ATBMDiseqcReadRequest (  DISEQC_PARAM* pstDiseqcParams,DISEQC_REPLY_INFO* pstDiseqcReplyInfo)
{
	ATBM_STATUS status = ATBM_SUCCESSFUL;
	ATBM_STATUS statusInfo = ATBM_SUCCESSFUL; 

	ATBM_STATUS statusTemp = 0;				/* temp status to avoid overwriting of status, 	*/
	/* specially used for analogue settings after DiSEqC			*/
	uint8 goOnFlag = FALSE;		    /* if set analogue settings after DiSEqC are carried out*/
	uint8 length;					/* length of bytes to send via DiSEqC */
	uint8 bytes2read;				/* length of bytes to read via DiSEqC */
	uint8 u8Data;					/* data byte send/receive via I2C */
	uint16 address = 0x6626;		/* I2C address of the first received byte  */
	BOOL replyExpected;
	BOOL disqcBusy = TRUE;
	uint32 tryAmount;
	uint8 i;

	/* check if mode is valid DiSEqC mode */
	if ((pstDiseqcParams->out_params.diseqc_mod_e == CONT_TONE) || (pstDiseqcParams->out_params.diseqc_mod_e == ONLY_GPIO))
	{
		status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("Error in ATBMDiseqcReadRequest: DiSEqC transmission is not selected\n");  
#endif
	}

	if (ATBM_SUCCESSFUL == status)
	{
		/* how many bytes should be transmitted via DiSEqC: */ 
		length = pstDiseqcParams->write_params.NoOfBytesToSend;
		if (length  > 8)
		{
			status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
			ATBMDebugPrint("Error in ATBMDiseqcReadRequest: number of DiSEq Bytes is not valid\n"); 
#endif
		}
	}

	if (ATBM_SUCCESSFUL == status)
	{	
		/* reset data of last received bytes */
		for (i=0; i<8;i++)
			pstDiseqcReplyInfo->receivedBytes[i] = 0;

		/* reset info about the amount of received bytes */
		pstDiseqcReplyInfo->receivedBytesCount = 0x00;

		pstDiseqcReplyInfo->parityError= 0x00;

		/* length of bytes should be read from DiSEqC*/ 	
		bytes2read = pstDiseqcParams->read_params.NoOfBytesToRead;
		if ((bytes2read<1) || (bytes2read>8))
		{			
#ifdef ATBM_DISEQC_DEBUG
			ATBMDebugPrint("Error in ATBMDiseqcReadRequest: Number of DiSEqC Bytes is invalid, force using max value 8\n "); 
#endif
			bytes2read = 8; 
		}

		/*Disable tone burst and continuous tone*/
		status = ATBMDisableBurstContOff (pstDiseqcParams);

		/*Reset the message received flag	*/
		if (ATBM_SUCCESSFUL == status)
		{
			status = ATBMReadRegister16( 0x6613, &u8Data);
			if (ATBM_SUCCESSFUL == status)
			{
				u8Data |= 0x04;	/* set clear received bit */
				status = ATBMWriteRegister16( 0x6613, u8Data);
			}
			if (ATBM_SUCCESSFUL == status)
			{
				u8Data &=0xFB;	/* release clear received bit */
				status = ATBMWriteRegister16( 0x6613, u8Data);
			}
		}

		if (status == ATBM_SUCCESSFUL)
		{
			/* always reply expected: */
			replyExpected = TRUE;

			/* set number of expected receive bytes (if zero: IC doesn't wait for reply) */
			status = ATBMReadRegister16( 0x6602, &u8Data);
			if(ATBM_SUCCESSFUL == status)
			{
				u8Data &=0xF0;
				u8Data |=0x40; //invert
				u8Data |= bytes2read;
				status = ATBMWriteRegister16( 0x6602, u8Data);
			}

			if(ATBM_SUCCESSFUL == status)
			{			
				/* force delay2 (time master waits until the DiSEqC message should be received) */
				/*	 180ms (150 ms + 2 byte (2*13.5ms)) => 180*22/32= 124 = 0x7C */
				u8Data = 0x7C;
				status = ATBMWriteRegister16( 0x6604, u8Data);
			}

			if(ATBM_SUCCESSFUL == status)
			{	
				/* prepare DiSEqC transmission with read request */
				/* check if DiSEqC transmission (with reply-request) from master is allowed */
				tryAmount = 0;
				do{
					status =  ATBMGetDiseqcBusyInfo (&disqcBusy);
				}while ((disqcBusy==TRUE) && (++tryAmount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL == status));

				if (DISEQC_RTS_TRIALS == tryAmount)	     /* check (bit0), if DiSEqC is ready (bitwise and) */
				{
					status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
					ATBMDebugPrint("ATBM_DiseqcReadRequest Error: DiSEqC is still busy, no new transmission allowed\n"); 
#endif
				}
				else if((disqcBusy==FALSE) && (ATBM_SUCCESSFUL == status))
				{
					/* send after all the real DiSEqC reply-request message: */
					status = ATBMTriggerCommand(pstDiseqcParams);	
				}
			}

			/*After DiSEqC command complete, cut the waiting  time*/
			if(ATBM_SUCCESSFUL == status)
			{
				/* check if reply from Slave is received  bit1 = 1 or if delayBeforeCont is gone bit0 = 1*/
				/* bit1 = 1  DiSEqC message is received or time out via bit0 (delayBeforeCont is gone) */
				/* as long as both bits are 0, the DiSEqC is busy */
				tryAmount = 0;				
				do{
					status = ATBMReadRegister16( 0x6623, &u8Data);
				} while (((u8Data & 0x02)==0) && ((u8Data & 0x01)==0) && ((ATBM_SUCCESSFUL == status)&& (++tryAmount < DISEQC_RTS_TRIALS) )); 

				/* to shorten the DiSEqC receive, if the data are received, delayBeforeCont set to 0: */
				if ( (!(u8Data & 0x01)) && (ATBM_SUCCESSFUL == status) )
				{
					status = ATBMWriteRegister16( 0x6604, 0x00);/* don't use data! */
				}

				if(ATBM_SUCCESSFUL == status)
					goOnFlag = TRUE;
			}

			/* check of DiSEqC busy - This is only necessary in case of using DiSEqC receive flag!!*/
			tryAmount = 0;
			do{
				statusTemp = ATBMGetDiseqcBusyInfo (&disqcBusy);
			}while ((disqcBusy==TRUE) && (++tryAmount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL == statusTemp));

			/* write original delayBeforeCont to set time after tone burst: */
			if (pstDiseqcParams->write_params.delayBeforeCont > 350) /* max 350 ms */
			{
				u8Data = 0xFF;	/* forced to max (368.7) ms */
				statusInfo = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
				ATBMDebugPrint("Error in ATBM_DiseqcReadRequest delayBeforeDiseqc should not more than 350ms\n"); 
#endif
			}
			else
			{
				/*rounded (+5/10) and one digit (+10/10) added to the register value */
				u8Data = (uint8)(((float)(pstDiseqcParams->write_params.delayBeforeCont)*6.875 + 15.0)/10.0 + 10);
			}
			status = ATBMWriteRegister16( 0x6604, u8Data);


			/* check if  tone burst has to be set: */
			if ((goOnFlag == TRUE) && (ATBM_SUCCESSFUL == status))
			{
				if (DISEQC_ONLY != pstDiseqcParams->out_params.diseqc_mod_e)
				{
					if ((DISEQC_CONT_AND_BURST == pstDiseqcParams->out_params.diseqc_mod_e )||(DISEQC_BURST == pstDiseqcParams->out_params.diseqc_mod_e))
					{	
						/* set tone burst */					
						statusTemp = ATBMToneburstValueSet(pstDiseqcParams);

						if (ATBM_SUCCESSFUL == statusTemp)
						{
							if (DISEQC_BURST == pstDiseqcParams->out_params.diseqc_mod_e)
							{		
								pstDiseqcParams->out_params.diseqc_mod_e = ONLY_GPIO;
								tryAmount = 0;
								do{
									status =  ATBMGetDiseqcBusyInfo (&disqcBusy);
								}while ((disqcBusy==TRUE) && (++tryAmount < DISEQC_RTS_TRIALS) && (ATBM_SUCCESSFUL == status));
								//	statusTemp = ATBMSetPolarizationPort (&pstDiseqcParams->out_params);
								pstDiseqcParams->out_params.diseqc_mod_e = DISEQC_BURST;
							}
							else /* DISEQC_CONT_AND_BURST mode */
							{
								statusTemp = ATBMSetAnalogueDishTone(pstDiseqcParams);
							}
						}
					}
					else if (DISEQC_CONT == pstDiseqcParams->out_params.diseqc_mod_e )
						statusTemp = ATBMSetAnalogueDishTone(pstDiseqcParams);
				}
			}

			if (ATBM_SUCCESSFUL == status) /* not statusTemp to react if DiSEqC send had success */
			{
				/* save parity status of received reply */
				status = ATBMReadRegister16( 0x6623, &u8Data);

				if(ATBM_SUCCESSFUL == status )
				{
					if (u8Data & 0x04)	/* check bit 2 of 0x6623 */
						pstDiseqcReplyInfo->parityError = TRUE; 
					else
						pstDiseqcReplyInfo->parityError = FALSE;
					/* save how many bytes are received: */
					u8Data = u8Data>>3;
					pstDiseqcReplyInfo->receivedBytesCount = (u8Data & 0x0F);
					if(pstDiseqcReplyInfo->receivedBytesCount >8)
						pstDiseqcReplyInfo->receivedBytesCount = 8;

					/* save replied bytes */
					for (i=0; (i < pstDiseqcReplyInfo->receivedBytesCount) && (ATBM_SUCCESSFUL == status); i++)
					{	/* write data bytes into DiSEqC buffer */
						status  = ATBMReadRegister16( (uint16) (address + i), &pstDiseqcReplyInfo->receivedBytes[i] );
					}
				}
			}
		} 
	} /* End if number of bytes write <8 and selected mode is DiSEqC mode*/

	/* Check status */	
	if((ATBM_SUCCESSFUL == status) && (ATBM_SUCCESSFUL == statusTemp))
	{
		status = ATBM_SUCCESSFUL;
	}	
	else
	{
		status = ATBM_FAILED;
#ifdef ATBM_DISEQC_DEBUG
		ATBMDebugPrint("ATBM_DiseqcReadRequest: failed\n");
#endif
	}
	return (status);

} /* End of ATBMDiseqcReadRequest */


void Diseqc_Control(DISEQC_PARAMS_INPUT_T  *Diseqc_params_in)
{	
	//uint8 i;
	ATBM_STATUS status = ATBM_FAILED;

	if(Diseqc_params_in->ToneSignal ==1)
	{
		diseqc_params.diseqcModulation_e = DISEQC_22KHZ;
	}
	else
	{
		ATBMSetBinaryMode(Diseqc_params_in->PolarityInverted);
		/*if(Diseqc_params_in->PolarityInverted == 1)
		{
		diseqc_params.diseqcModulation_e = DISEQC_BIN_INVERTED;
		}
		else
		{
		diseqc_params.diseqcModulation_e = DISEQC_BIN;
		}*/
	}

	/*if((Diseqc_params_in->ContinuousToneEnable == 0)&&(Diseqc_params_in->ToneBurstEnable == 0))
	{
	diseqc_params.out_params.diseqc_mod_e = DISEQC_ONLY;
	}*/

	if((Diseqc_params_in->ContinuousToneEnable == 1)&&(Diseqc_params_in->ToneBurstEnable == 1)&&(Diseqc_params_in->write_params.NoOfBytesToSend != 0))
	{
		//diseqc_params.out_params.diseqc_mod_e = DISEQC_CONT_AND_BURST;
		ATBMSendDiseqcBurstToneContTone(&Diseqc_params_in->write_params.TransmitByte[0], Diseqc_params_in->write_params.NoOfBytesToSend,  Diseqc_params_in->ToneBurstValue,  Diseqc_params_in->ContinuousToneValue);
	}
	if((Diseqc_params_in->ContinuousToneEnable == 1)&&(Diseqc_params_in->ToneBurstEnable == 0)&&(Diseqc_params_in->write_params.NoOfBytesToSend != 0))
	{
		if(Diseqc_params_in->write_params.TransmitByte[0] >= 0xe2)
		{
           ATBMSendDiseqcWithReply(&Diseqc_params_in->write_params.TransmitByte[0],  Diseqc_params_in->write_params.NoOfBytesToSend,  &Diseqc_params_in->reply_info.receivedBytes[0],  &Diseqc_params_in->reply_info.receivedBytesCount);
		   ATBMSetContinuousTone(Diseqc_params_in->ContinuousToneValue);
		}
		else
		{
			diseqc_params.out_params.diseqc_mod_e = DISEQC_CONT;	
			ATBMSendDiseqcContTone(&Diseqc_params_in->write_params.TransmitByte[0],  Diseqc_params_in->write_params.NoOfBytesToSend,  Diseqc_params_in->ContinuousToneValue);
		}
	}
	if((Diseqc_params_in->ContinuousToneEnable == 0)&&(Diseqc_params_in->ToneBurstEnable == 1))
	{

		diseqc_params.out_params.diseqc_mod_e = DISEQC_BURST;
		if(Diseqc_params_in->write_params.NoOfBytesToSend == 0)
			ATBMSendToneBurst(Diseqc_params_in->ToneBurstValue);
	}

	//diseqc_params.write_params.NoOfBytesToSend = Diseqc_params_in->write_params.NoOfBytesToSend ;	
	if((Diseqc_params_in->ContinuousToneEnable == 1)&&(Diseqc_params_in->write_params.NoOfBytesToSend == 0))
	{
		//diseqc_params.out_params.diseqc_mod_e = CONT_TONE;
		ATBMSetContinuousTone(Diseqc_params_in->ContinuousToneValue);
	}

	if((Diseqc_params_in->ContinuousToneEnable == 0)&&(Diseqc_params_in->write_params.NoOfBytesToSend == 0))
	{
		Diseqc_params_in->ContinuousToneValue = 0;
		ATBMSetContinuousTone(Diseqc_params_in->ContinuousToneValue);
	}
	/*diseqc_params.out_params.toneBurstVal = Diseqc_params_in->ContinuousToneEnable;
	diseqc_params.out_params.polarizationPortVal = FALSE;	

	diseqc_params.write_params.delayBeforeDiseqc = 15;		
	diseqc_params.write_params.delayBeforeCont = 15;		*/

	if((Diseqc_params_in->ContinuousToneEnable == 0)&&(Diseqc_params_in->ToneBurstEnable == 0)&&(Diseqc_params_in->write_params.NoOfBytesToSend != 0))
	{
		if(Diseqc_params_in->write_params.TransmitByte[0] >= 0xe2)
		{
			ATBMSendDiseqcWithReply(&Diseqc_params_in->write_params.TransmitByte[0],  Diseqc_params_in->write_params.NoOfBytesToSend,  &Diseqc_params_in->reply_info.receivedBytes[0],  &Diseqc_params_in->reply_info.receivedBytesCount);

		}
		else
		{
			ATBMSendDiseqc(&Diseqc_params_in->write_params.TransmitByte[0],  Diseqc_params_in->write_params.NoOfBytesToSend);
		}
		//	diseqc_params.write_params.TransmitByte[0] = 0xE0;		/* 0xE0 means master's first transmission no reply expect */
		/*	for (i=0; i< Diseqc_params_in->write_params.NoOfBytesToSend;i++)												
		{ 
		diseqc_params.write_params.TransmitByte[i] = Diseqc_params_in->write_params.TransmitByte[i];
		}	*/
		/* sending first 3 bytes will causes reset of any devices */
	}
	status = ATBM_SUCCESSFUL;
}

/*this is internal function*/
ATBM_STATUS ATBMDiseqcControl(DISEQC_PARAM* pstDiseqcParams)
{
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMDiseqcSet(pstDiseqcParams);     

	if(status == ATBM_SUCCESSFUL)
	{	/* Set DiSEqC Transmit data */
		status = ATBMWriteDiseqcBuffer (&pstDiseqcParams->write_params);
	}
	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcSatControl (pstDiseqcParams);
	}
	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Failed in DiseqcBurstToneContTone transmission\n");
	}
	return status;
}


ATBM_STATUS ATBMSetLNBByGPIO(uint8  u8LnbPinLevel)
{
	ATBM_STATUS status = ATBM_FAILED;
	status = ATBM_SUCCESSFUL;
	return status;

}

ATBM_STATUS ATBMGetLNBStatusFromGPIO(uint8 *pu8LnbPinLevel)
{
	ATBM_STATUS status = ATBM_FAILED;

	return status;
}

/* 
ContTone22KOnOff = 1, 22K continuous tone on
ContTone22KOnOff = 0, 22K continuous tone off 
*/
ATBM_STATUS ATBMSetContinuousTone(BOOL ContTone22KOnOff)
{
	ATBM_STATUS status = ATBM_FAILED;
	/*Set clock and NCO count */
	status = ATBMCheckDiSEqCState();

	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcSet(&diseqc_params); 
	}

	if(ContTone22KOnOff == TRUE)
	{
		/*Enable continuous tone and set continuous tone value.*/
		diseqc_params.out_params.diseqc_mod_e = CONT_TONE;		
		diseqc_params.out_params.contToneVal = 1;
		if(status == ATBM_SUCCESSFUL)
		{
			status = ATBMDiseqcSatControl (&diseqc_params);
		}		
	}
	else
	{
		if(status == ATBM_SUCCESSFUL)
		{      /*Switch off Continuous tone*/
			status = ATBMSwitchOffContTone (&diseqc_params);
		}
	}

	/*Check status*/
	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Start continuous Tone  failed\n");
	}
	return status;;
}

ATBM_STATUS ATBMSendToneBurst(BOOL u8ToneBurstType)
{
	ATBM_STATUS status = ATBM_FAILED;
	uint8 u8Data;

	status = ATBMCheckDiSEqCState();

	/*Tone burst using DiSEqC with Burst mode, but No real DiSEqC used.*/
	diseqc_params.out_params.diseqc_mod_e = DISEQC_BURST;
	diseqc_params.out_params.toneBurstVal = u8ToneBurstType;

	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcSet(&diseqc_params); 
	}
	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMReadRegister16( 0x6601, &u8Data);
	}
	if(status == ATBM_SUCCESSFUL)
	{
		u8Data &=0xf0;  /*Set DiSEqC transmit data length = 0, no DiSEqC command*/
		status = ATBMWriteRegister16( 0x6601, u8Data);
	}
	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcSatControl (&diseqc_params);
	}
	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Send Tone Burst failed\n");
	}

	return status;
}

ATBM_STATUS ATBMSetToneLevel(DISEQC_TONE_LEVEL  ToneLevel)
{
	uint8 u8Data = 0;
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMRead(0x06, 0x11, &u8Data);

	u8Data &= 0xC7;

	if(ToneLevel  ==  TONE_LEVEL_0DOT65V)
	{
		u8Data|=0x00;
	}
	if(ToneLevel  ==  TONE_LEVEL_0DOT75V)
	{
		u8Data|=0x08;
	}
	if(ToneLevel  ==  TONE_LEVEL_0DOT85V)
	{
		u8Data|=0x10;
	}
	if(ToneLevel  ==  TONE_LEVEL_0DOT95V)
	{
		u8Data|=0x18;
	}
	if(ToneLevel  ==  TONE_LEVEL_1DOT05V)
	{
		u8Data|=0x20;
	}
	if(ToneLevel  ==  TONE_LEVEL_1DOT15V)
	{
		u8Data|=0x28;
	}
	if(ToneLevel  ==  TONE_LEVEL_1DOT25V)
	{
		u8Data|=0x30;
	}
	if(ToneLevel  ==  TONE_LEVEL_1DOT35V)
	{
		u8Data|=0x38;
	}
	status |= ATBMWrite(0x06, 0x11, u8Data);
    return status;
}


ATBM_STATUS ATBMSendDiseqc(uint8 *pu8DiseqcTxBuffer,  uint8 u8TxBufferLength)
{
	uint8 u8Iter = 0;
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMCheckDiSEqCState();


	diseqc_params.out_params.diseqc_mod_e = DISEQC_ONLY;	
	if((u8TxBufferLength != 0)&&(u8TxBufferLength <= 8))
	{
		//	diseqc_params.write_params.TransmitByte[0] = 0xE0;		/* 0xE0 means master's first transmission no reply expect */
		for (u8Iter = 0; u8Iter < u8TxBufferLength; u8Iter++)												/* reset off TransmitByte[1]-TransmitByte[7]	*/
		{ 
			diseqc_params.write_params.TransmitByte[u8Iter] = pu8DiseqcTxBuffer[u8Iter];
		}				
		diseqc_params.write_params.NoOfBytesToSend = u8TxBufferLength;
	}	

	status =  ATBMDiseqcControl(&diseqc_params);

	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Failed in DiSEqC only transmission\n");
	}
	return status;
}

ATBM_STATUS ATBMSendDiseqcContTone(uint8 *pu8DiseqcTxBuffer,  uint8 u8TxBufferLength,  uint8 ConsToneValue )
{
	uint8 u8Iter = 0;
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMCheckDiSEqCState();

	diseqc_params.out_params.diseqc_mod_e = DISEQC_CONT;	
	diseqc_params.out_params.contToneVal    = ConsToneValue; /*0: no 22k tone output, 1: 22k continuous tone */

	if((u8TxBufferLength != 0)&&(u8TxBufferLength <= 8))
	{
		//	diseqc_params.write_params.TransmitByte[0] = 0xE0;		/* 0xE0 means master's first transmission no reply expect */
		for (u8Iter = 0; u8Iter < u8TxBufferLength; u8Iter++)		/* reset off TransmitByte[1]-TransmitByte[7]	*/
		{ 
			diseqc_params.write_params.TransmitByte[u8Iter] = pu8DiseqcTxBuffer[u8Iter];
		}				
		diseqc_params.write_params.NoOfBytesToSend = u8TxBufferLength;
	}	
	status =  ATBMDiseqcControl(&diseqc_params);

	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Failed in DiSEqC only transmission\n");
	}
	return status;
}

ATBM_STATUS ATBMSendDiseqcBurstToneContTone(uint8 *pu8DiseqcTxBuffer, uint8 u8TxBufferLength, uint8 u8ToneBurstType,  uint8 ConsToneValue)
{
	uint8 u8Iter = 0;
	ATBM_STATUS status = ATBM_FAILED;

	status = ATBMCheckDiSEqCState();

	diseqc_params.out_params.diseqc_mod_e = DISEQC_CONT_AND_BURST;	
	diseqc_params.out_params.toneBurstVal   = u8ToneBurstType;      /*0: Tone burst A,   1:Tone burst B */
	diseqc_params.out_params.contToneVal    = ConsToneValue;        /*0: no 22k tone output, 1: 22k continuous tone */

	if((u8TxBufferLength != 0)&&(u8TxBufferLength <= 8))
	{
		//	diseqc_params.write_params.TransmitByte[0] = 0xE0;		/* 0xE0 means master's first transmission no reply expect */
		for (u8Iter = 0; u8Iter < u8TxBufferLength; u8Iter++)												/* reset off TransmitByte[1]-TransmitByte[7]	*/
		{ 
			diseqc_params.write_params.TransmitByte[u8Iter] = pu8DiseqcTxBuffer[u8Iter];
		}				
		diseqc_params.write_params.NoOfBytesToSend = u8TxBufferLength;
	}	

	status =  ATBMDiseqcControl(&diseqc_params);

	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Failed in DiSEqC only transmission\n");
	}
	return status;
}


ATBM_STATUS ATBMSetBinaryMode(uint8 u8BinaryInverted)
{
	ATBM_STATUS status = ATBM_FAILED;
	uint8 u8ChipID, u8DiseqcOut;
	status = ATBMCheckDiSEqCState();

	/* DISEQC_BIN_INVERTED is valid for binary mode only. Select Binary or Binary inverted.*/
	if(u8BinaryInverted == 1)
	{
		diseqc_params.diseqcModulation_e = DISEQC_BIN_INVERTED;
	}
	else
	{
		diseqc_params.diseqcModulation_e = DISEQC_BIN;
	}
	status = ATBMRead(0x00, 0x00, &u8ChipID);
	if((u8ChipID == 0xa2)||(u8ChipID == 0xa4))
	{
		ATBMRead(0x06, 0x11, &u8DiseqcOut);
		u8DiseqcOut |= 0xc0;  //DiSEqC output 3.3v option Enable if DiSEqC output enable
		ATBMWrite(0x06, 0x11, u8DiseqcOut);
	}
	return status;
}



ATBM_STATUS ATBMSendDiseqcWithReply(uint8 *pu8DiseqcTxBuffer, uint8 u8TxBufferLength,  uint8 *pu8DiseqcReplyBuffer, uint8 *pu8DiseqcReplyLength)
{
	uint8 u8Iter = 0;
	ATBM_STATUS status = ATBM_FAILED;
	DISEQC_REPLY_INFO  DiseqcReplyInfo;
	DISEQC_READ_PARAM  DiseqcReadParams; 
    unsigned char u8DiSEqCInEn = 0;
	/*Check DiSEqC state*/
	status = ATBMCheckDiSEqCState();

	/*This function support only DiSEqC command with Reply information.*/
	diseqc_params.out_params.diseqc_mod_e = DISEQC_ONLY;	

	if((u8TxBufferLength != 0)&&(u8TxBufferLength <= 8))
	{
		//diseqc_params.write_params.TransmitByte[0] = 0xE0;		/* 0xE0 means master's first transmission no reply expect */
		for (u8Iter = 0; u8Iter <  u8TxBufferLength; u8Iter++)		/* Copy data from use level API to DiSEqC struct  parameters:  TransmitByte[1]-TransmitByte[7] */
		{ 
			diseqc_params.write_params.TransmitByte[u8Iter] = pu8DiseqcTxBuffer[u8Iter];
		}	
		diseqc_params.write_params.NoOfBytesToSend = u8TxBufferLength;  /*Set DiSEqC command length*/
	}
	else
	{
		ATBMDebugPrint("Error DiSEqC command\n");
		return ATBM_FAILED;
	}
	/*Set DiSEqC mode*/
	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcSet(&diseqc_params);  
	}
	if(status == ATBM_SUCCESSFUL)
	{  /*Set DiSEqC Transmit  data*/
		status = ATBMWriteDiseqcBuffer (&diseqc_params.write_params);
	}

	if(status == ATBM_SUCCESSFUL)
	{   /*Set read Parameters*/
		status = ATBMDefaultsDiseqcReadParameters(&DiseqcReadParams,  &DiseqcReplyInfo);
		if((diseqc_params.diseqcModulation_e ==  DISEQC_BIN)||(diseqc_params.diseqcModulation_e ==  DISEQC_BIN_INVERTED))
		{
			DiseqcReadParams.binaryModulation = TRUE;
		}
	}


	if(status == ATBM_SUCCESSFUL)
	{
		ATBMRead(0x06, 0x23, &u8DiSEqCInEn);
		u8DiSEqCInEn|=0x01;
		ATBMWrite(0x06, 0x23, u8DiSEqCInEn);
	}

	if(status == ATBM_SUCCESSFUL)
	{
		status = ATBMDiseqcReadInit(&DiseqcReadParams);
	}
	if(status == ATBM_SUCCESSFUL)
	{	/*DiSEqC read operation*/
		status = ATBMDiseqcReadRequest (&diseqc_params, &DiseqcReplyInfo);
	}

	if((DiseqcReplyInfo.receivedBytesCount != 0)&&(DiseqcReplyInfo.receivedBytesCount <= 8))
	{
		for (u8Iter = 0; u8Iter < DiseqcReplyInfo.receivedBytesCount; u8Iter++)												/* reset off Transmit Byte[1]-TransmitByte[7]	*/
		{ 
			pu8DiseqcReplyBuffer[u8Iter] = DiseqcReplyInfo.receivedBytes[u8Iter];
		}		
		*pu8DiseqcReplyLength = DiseqcReplyInfo.receivedBytesCount;
	}
	if(status != ATBM_SUCCESSFUL)
	{
		ATBMDebugPrint("Failed in DiSEqC with reply operation\n");
	}
	return status;
}



