/* diseqcapi.h */
/********************************************************************\


\********************************************************************/

#include "Atbm781x.h"
#include "sys_types.h"
#ifndef DISEQC_API_H_DEFINED
#define DISEQC_API_H_DEFINED

/* ================================== */
/* Definitions of commonly used types */
/* ================================== */

//typedef int BOOL;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DISEQC_RTS_TRIALS 500 /* repetitions of  checking times if DiSEqC write is not finished */
#if 0
typedef enum
{
  ATBM_SUCCESSFUL = 0,         /* No error */
  ATBM_FAILED,                 /* General error */  
  ATBM_DISEQC_INITIALIZED,
  ATBM_DISEQC_NOT_INITIALIZED,
  ATBM_NO_ACKNOWLEDGE,         /* No acknowledge from I2C bus */
  ATBM_NO_SLAVE_ACK            /* No acknowledge from devices on the GPIO I2C bus */
} ATBM_STATUS; 
#endif
typedef enum						/* DiSEqC Modes: 0-5 */
{
	DISEQC_ONLY = 0,
	DISEQC_CONT_AND_BURST,			/* DiSEqC with continuous, tone burst and GPIO */
	DISEQC_CONT,					/* DiSEqC with continuous tone and GPIO (no burst )*/
	DISEQC_BURST,					/* DiSEqC with tone burst and GPIO (no continuous) */
	CONT_TONE,						/* no DISEQC only 22KHz cont tone and GPIO port selection */
	ONLY_GPIO						/* set GPIO pin only - for other purposes - no DiSEqC or SAT function */
}DISEQC_MOD;

typedef enum
{
	TONE_LEVEL_0DOT65V = 0,
	TONE_LEVEL_0DOT75V ,
	TONE_LEVEL_0DOT85V,
	TONE_LEVEL_0DOT95V ,
	TONE_LEVEL_1DOT05V,
	TONE_LEVEL_1DOT15V ,
	TONE_LEVEL_1DOT25V,
	TONE_LEVEL_1DOT35V
}DISEQC_TONE_LEVEL;


typedef enum
{
	DISEQC_22KHZ = 0,
	DISEQC_BIN,
	DISEQC_BIN_INVERTED
}DISEQC_MODULATION_MOD_E;

typedef struct DISEQC_WRITE_PARAM
{
	uint8  NoOfBytesToSend;
	uint8  TransmitByte[8];
	uint16 delayBeforeDiseqc;		/* delay1 in ms before and in case of ToneBurst also behind DiSEqC */
	uint16 delayBeforeCont;		    /* delay2 in ms before cont tone starts again */
}DISEQC_WRITE_PARAM;

typedef struct DISEQC_OUT_PARAM
{
	DISEQC_MOD diseqc_mod_e;		/* see description in ATBM_enums */
	BOOL contToneVal;				/* zero (22kHz off) selects low band, high (22kHz on) selects high band */
	BOOL toneBurstVal;				/* zero selects LNB a, high selects LNB b */
	BOOL polarizationPortVal;		/* GPIO Port 3 low-high switch */
}DISEQC_OUT_PARAM;


typedef struct DISEQC_READ_PARAM
 {
	 BOOL binaryModulation;			/* binary demodulated or 22kHz modulated */
	 BOOL readBinInvert;				/* only for V2: inverted binary input signal */
	 uint8 NoOfBytesToRead;
	 uint8 MagThreshold;
	 /* uint8 bitDuration; */
	 BOOL DirPinByDiseqc;			/* DISEQC DIRection pin controlled by DiSEqC if TRUE */
	 BOOL DirPinPolarity;
}DISEQC_READ_PARAM;				

typedef struct DISEQC_PARAM
{
	DISEQC_WRITE_PARAM write_params;			/*DiSEqC transmission params */
	DISEQC_OUT_PARAM out_params;				/*analog signal specification */
	DISEQC_READ_PARAM read_params;				/*DiSEqC read settings*/
	DISEQC_MODULATION_MOD_E diseqcModulation_e;
	BOOL DiseqcInitialized;
}DISEQC_PARAM;

typedef struct DISEQC_REPLY_INFO
{
	BOOL parityError;
	uint8 receivedBytesCount;
	uint8 receivedBytes[8];	/* contains the DiSEqC read bytes */
}DISEQC_REPLY_INFO;

typedef struct DISEQC_PARAMS_INPUT
{
	uint8 ContinuousToneEnable;
	uint8 ToneBurstEnable;
	uint8 ToneSignal;	
	uint8 PolarityInverted;
	uint8 ToneBurstValue; //Tone burst A,or Tone burst B
	uint8 ContinuousToneValue; //0:off, 1:on
	uint32 SysClockHz;
	DISEQC_WRITE_PARAM write_params;	
	DISEQC_READ_PARAM read_params;				/*DiSEqC read settings*/
    DISEQC_REPLY_INFO reply_info;
}DISEQC_PARAMS_INPUT_T;


/****************************************************************************
Function:   ATBMSetLNBByGPIO
Parameters: u8LnbPinLevel, 1 Set LNB High, 0 Set LNB low.                  
Return:     ATBM_STATUS.
Description:
	The LNB voltage or polarity can be controlled by GPIO	
******************************************************************************/
ATBM_STATUS ATBMSetLNBByGPIO(uint8  u8LnbPinLevel);

/****************************************************************************
Function:   ATBMGetLNBStatusFromGPIO
Parameters: u8LnbPinLevel,  0 LNB low, 1 LNB High.               
Return:     ATBM_STATUS.
Description:
	The LNB voltage or polarity can be controlled by GPIO	
	at the same time, the GPIO state which indicating LNB status can 
	be read from register 	
******************************************************************************/
ATBM_STATUS ATBMGetLNBStatusFromGPIO(uint8 *pu8LnbPinLevel);

/****************************************************************************
Function:   ATBMSetContinuousTone
Parameters: ContTone22KOnOff, 0: 22KHz tone off,  1:22KHz tone on,            
Return:     ATBM_STATUS.
Description:
	Set 22K continuous tone on or off to control LNB or 22K switcher. 
	for example
	Stop  22KHz tone, call ATBMSetContinuousTone(0); 22K continuous tone stoped
	Start 22KHz tone, call ATBMSetContinuousTone(1); 22K continuous tone start
	attention: 22KHz Continuous tone is modulated signal, no Binary control signal.
******************************************************************************/
ATBM_STATUS ATBMSetContinuousTone(BOOL ContTone22KOnOff);

/****************************************************************************
Function:   ATBMSendToneBurst
Parameters: u8ToneBurstType, 0:Tone Burst'0'(usually satellite A), 1,Tone burst'1'(usually satellite B).                  
Return:     ATBM_STATUS.
Description:
	Send 22KHz tone burst, there are Tone Burst0 and Tone Burst1.
	Call ATBMSendToneBurst(0); Send one time tone Burst 0
    Call ATBMSendToneBurst(1); Send one time tone Burst 1	
******************************************************************************/
ATBM_STATUS ATBMSendToneBurst(BOOL u8ToneBurstType);

/****************************************************************************
Function:   ATBMSetToneLevel
Parameters: ToneLevel.                  
Return:     ATBM_STATUS.
Description:
	The Tone output level voltage controlled by ATBMSetToneLevel
	Default 0.65V,  some device need increase the voltage can call this function.
	Available selection:
	0.65V;0.75V;0.85V;0.95V;1.05V;1.15V;1.25V;1.35V
******************************************************************************/
ATBM_STATUS ATBMSetToneLevel(DISEQC_TONE_LEVEL  ToneLevel);

/****************************************************************************
Function:   ATBMSendDiseqc
Parameters: pu8DiseqcTxBuffer, pointer to DiSEqC command Buffer in Byte.   
            u8TxBufferLength,  DiSEqC command length in Byte

Return:     ATBM_STATUS.
Description:
	Send DiSEqC command to slave device.
	Take care this function only send one way DiSEqC command, 
	if DiSEqC reply required please call ATBMSendDiseqcWithReply.
******************************************************************************/
ATBM_STATUS ATBMSendDiseqc(uint8 *pu8DiseqcTxBuffer,  uint8 u8TxBufferLength);

/****************************************************************************
Function:   ATBMSendDiseqcContTone
Parameters: pu8DiseqcTxBuffer, pointer to DiSEqC command Buffer in Byte.   
            u8TxBufferLength,  DiSEqC command length in Byte
            ConsToneValue,     Continuous tone on or off, 1:on, 0:off

Return:     ATBM_STATUS.
Description:
	Send DiSEqC command to slave device. then Send Continuous tone if ConsToneOnOff == 1
	After DiSEqC command send output continuous tone. the sequence is as following:
	
	--DiSEqC command---Continuous tone------

    if no Continuous Tone needed, just set Parameter ConsToneOnOff == 0 or call ATBMSendDiseqc    
   Take care this function send one way DiSEqC command and analog Tone, no reply
******************************************************************************/
ATBM_STATUS ATBMSendDiseqcContTone(uint8 *pu8DiseqcTxBuffer,  uint8 u8TxBufferLength, uint8 ConsToneValue);

/****************************************************************************
Function:   ATBMSendDiseqcBurstToneContTone
Parameters: pu8DiseqcTxBuffer, pointer to DiSEqC command Buffer in Byte.   
            u8TxBufferLength,  DiSEqC command length in Byte
            u8ToneBurstType,   Tone Burst Type, can be 0(Burst tone0) or 1(Burst tone1)
            ConsToneValue,     Continuous tone on or off, 1:on, 0:off

Return:     ATBM_STATUS.
Description:
	Send DiSEqC command to slave device.
	After DiSEqC command send one time Tone Burst and continuous tone. the sequence is as following:
	
	--DiSEqC command---Burst Tone---Continuous tone------

    if no Continuous Tone needed, just set Parameter ConsToneOnOff == 0
    if no Burst tone needed, please call ATBMSendDiseqcContTone
    if no continuous tone and  No burst tone needed, please call ATBMSendDiseqc

Take care this function send one way DiSEqC command and analog Tone, no reply
******************************************************************************/
ATBM_STATUS ATBMSendDiseqcBurstToneContTone(uint8*pu8DiseqcTxBuffer, uint8 u8TxBufferLength, uint8 u8ToneBurstType,  uint8 ConsToneValue);



ATBM_STATUS ATBMSendDiseqcWithReply(uint8*pu8DiseqcTxBuffer, uint8 u8TxBufferLength,  uint8*pu8DiseqcReplyBuffer, uint8* pu8DiseqcReplyLength);

/****************************************************************************
Function:   ATBMSetBinaryMode
Parameters: u8BinaryInverted, 1 Binary inverted, 0 not inverted.                  
Return:     ATBM_STATUS.Description:
	This function set DiSEqC binary inverted mode.
	Take care, only valid when Binary mode is used, 
	This function should not be used when tone mode is used.
******************************************************************************/
ATBM_STATUS ATBMSetBinaryMode(uint8 u8BinaryInverted);







#endif  /* #ifndef DISEQC_API_H_DEFINED */
