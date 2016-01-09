#ifndef ATBM781X_H
#define ATBM781X_H

#include "sys_define.h"
/**********************SDK Redefine Data Type ********************************/
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long   uint32;
typedef unsigned char ATBM_STATUS;

typedef char			int8;
typedef short           int16;
typedef int             int32;

/**********************debugPrintf Define ********************************/
#define  ATBMDEBUGPRINTENABLE     1
#if ATBMDEBUGPRINTENABLE
#define ATBMDebugPrint   OS_PRINTF
#else
#define ATBMDebugPrint
#endif



/**********************Next is define the T and T2 BandWidth Macro*******************************/
#define  ATBM_DVBT_BANDWIDTH_1DOT7MHZ     0/* only support for T2 */
#define  ATBM_DVBT_BANDWIDTH_5MHZ         1
#define  ATBM_DVBT_BANDWIDTH_6MHZ         2
#define  ATBM_DVBT_BANDWIDTH_7MHZ         3
#define  ATBM_DVBT_BANDWIDTH_8MHZ         4
#define  ATBM_DVBT_BANDWIDTH_10MHZ        5

/**********************Chip Support Mode *******************************************************/
#define ATBM_DVBC_MODE                 0
#define ATBM_RSV_MODE                  1     //Reserved
#define ATBM_DVBT_MODE                 2
#define ATBM_DVBT2_MODE                3
#define ATBM_DVBS_MODE                 4
#define ATBM_DVBS2_MODE                5
#define ATBM_DVB_UNKNOWN               255
/***********************USB Port Read/Write Return Status ****************************************/
#define ATBM_SUCCESSFUL                0     /* No error */
#define	ATBM_FAILED                    1     /* General error, usually ATBM APIs not correctly used or DTV system error */
#define	ATBM_I2C_ERROR                 2     /*General I2C error,usually from I2C interface */
#define	ATBM_NO_ACKNOWLEDGE            4     /* No acknowledge from I2C bus */
#define	ATBM_NO_SLAVE_ACK              8     /* No acknowledge from devices on the GPIO I2C bus */


/**********************************************************************************************************************************
*struct MPEG_TS_mode_t
*@ui8TSTransferType: TS stream transfer type, can be set to parallel(8 bit data bus) or serial(1 bit data bus) mode
*@Demod output edge: demod will output TS data on this edge of TS stream clock
*@ui8SPIClockConstantOutput: TS stream clock can be set outputting all the time or only during TS data valid (188 bytes)
**********************************************************************************************************************************/
/*****************ui8TSTransferType Option Value***************************/
#define TS_PARALLEL_MODE             1
#define TS_SERIAL_MODE               0
/**********************ui8OutputEdge Option Value***************************/
#define TS_OUTPUT_FALLING_EDGE       1
#define TS_OUTPUT_RISING_EDGE        0
/**********************ui8TSSPIMSBSelection Option Value******************/
#define TS_SPI_MSB_ON_DATA_BIT7      1
#define TS_SPI_MSB_ON_DATA_BIT0      0
/**********************ui8TSSSIOutputSelection Option Value***************/
#define TS_SSI_OUTPUT_ON_DATA_BIT7   1
#define TS_SSI_OUTPUT_ON_DATA_BIT0   0
/**********************ui8SPIClockConstantOutput Option Value*************/
#define TS_CLOCK_CONST_OUTPUT        1
#define TS_CLOCK_VALID_OUTPUT        0
typedef struct MPEG_TS_MODE_STRUCT
{
	uint8 ui8TSTransferType;
	uint8 ui8OutputEdge;
	uint8 ui8TSSPIMSBSelection;
	uint8 ui8TSSSIOutputSelection;
	uint8 ui8SPIClockConstantOutput;
}MPEGTSMode;


#define   INPUT_IQ_SWAP           1
#define   INPUT_IQ_NORMAL        0

typedef enum
{
	ATBM_PIN_IF_AGC_T = 0,
    ATBM_PIN_IF_AGC_S,
    ATBM_PIN_RF_AGC_T,
	ATBM_PIN_GPO_POLARITY,
	ATBM_PIN_TS_LOCK,
	ATBM_PIN_TS_ERROR,
	ATBM_PIN_FEC_LOCK,
	ATBM_PIN_FEC_ERROR,
	ATBM_PIN_RESERVED
}ATBM_GPIO_PIN_TYPE;

typedef struct DVBT_TUNER_CONFIG_STRUCT
{
	uint8   ui8IQSwapMode;
    uint8   ui8BandwidthType; //eg.ATBM_DVBT_BANDWIDTH_8MHZ
	uint8   ui8BandwidthMHz;//  5,6,7,8
	uint32 u32IFFrequencyKHz;  //Tuner IF frequency for DVB-T,DVB-T2 and DVB-C, Unit is KHz
}DVBT_TUNER_INPUT_T;

typedef struct DVBC_TUNER_CONFIG_STRUCT
{
	uint8   ui8IQSwapMode;
	uint32 u32IFFrequencyKHz;
	uint32 u32SymbolRateK; //Unit is Kbps, Auto detection. this is legacy code.
}DVBC_TUNER_INPUT_T;

typedef struct DVBS_TUNER_CONFIG_STRUCT
{
	uint8   ui8IQSwapMode;
	uint32 ui32SymbolRateK; //DVB-S/S2 symbol rate, actually not used. auto detection.
}DVBS_TUNER_INPUT_T;

typedef struct ATBM_PIN_CONFIG_STRUCT
{
   ATBM_GPIO_PIN_TYPE GPOPinStatus;    //0x0023 bit[7:4]
   ATBM_GPIO_PIN_TYPE GPOPinAgcDvbS;   //0x0021 bit[3:0] = 3
   ATBM_GPIO_PIN_TYPE GPOPinRFAgcDvbT; //0x0021 bit[7:4] //Not used set 0
   ATBM_GPIO_PIN_TYPE GPOPinAgcDvbT;   //0x0022 bit[3:0] = 1
   ATBM_GPIO_PIN_TYPE GPO4PinStatus;   //0x0022 bit[7:4]
   ATBM_GPIO_PIN_TYPE GPO5PinStatus;   //0x0023 bit[3:0]
}ATBM_PIN_CONFIG_T;


typedef struct ATBM_DVB_CONFIG_STRUCT
{
	//Common setting
	MPEGTSMode stMPEGTSMode;      //TS interface configuration
	uint32 ui32DemodClkKHz;//Demodulator clock Select Unit is KHz

	ATBM_PIN_CONFIG_T GpioPinConfigs;

    //Set DVB-T(T2),  DVB-C, DVB-S, DVB-S2 mode
	uint8  ui8ConfigDVBType;    //eg. ATBM_DVBT_MODE(contains auto detection of DVB-T2), ATBM_DVBC_MODE

	//DVB-T,T2 setting
	DVBT_TUNER_INPUT_T DvbTxTunerInput; //DVB-T and DVB-T2

	//DVB-S/S2 setting
	DVBS_TUNER_INPUT_T DvbSxTunerInput;
	uint32 ui32DVBSSysClkKHz; //for DVB-S/S2 System clock,  all unit is KHz


	//DVB-C setting
	DVBC_TUNER_INPUT_T DvbCTunerInput;

}stATBMDvbConfig;


#define DVBT2_MAX_PLP_NUMBER         255     /* DVBT2 PLP maximum number*/

/* DVB-T FFT mode*/
typedef enum {
	DVBT_MODE_2K = 0,
	DVBT_MODE_8K,
	DVBT_MODE_RESERVED3,
	DVBT_MODE_UNKNOWN
}DVBT_FFT_MODE_TYPE;

/* DVB-T guard interval*/
typedef enum{
	DVBT_GI_1_32 = 0,
	DVBT_GI_1_16,
	DVBT_GI_1_8,
	DVBT_GI_1_4,
	DVBT_GI_UNKNOWN
} DVBT_GUARD_INTERVAL_TYPE;

/*DVB-T constellation*/
typedef enum{
	DVBT_CONSTELLATION_QPSK = 0,
	DVBT_CONSTELLATION_16QAM,
	DVBT_CONSTELLATION_64QAM,
	DVBT_CONSTELLATION_UNKNOWN
}DVBT_CONSTELLATION_TYPE;

/*DVB-T code rate*/
typedef enum{
	DVBT_CODE_RATE_1_2 = 0,
	DVBT_CODE_RATE_2_3,
	DVBT_CODE_RATE_3_4,
	DVBT_CODE_RATE_5_6,
	DVBT_CODE_RATE_7_8,
	DVBT_CODE_RATE_RESERVED5,
	DVBT_CODE_RATE_RESERVED6,
	DVBT_CODERATE_UNKNOWN
}DVBT_CODE_RATE_TYPE;

/*DVB-T hierarchy*/
typedef enum{
	DVBT_HIERARCHY_NON = 0,   /*Non-hierarchical*/
	DVBT_HIERARCHY_1,
	DVBT_HIERARCHY_2,
	DVBT_HIERARCHY_4,
	DVBT_HIERARCHY_UNKNOWN
}DVBT_HIERARCHY_TYPE;

/* DVB-T HP, LP profile*/
typedef enum{
	DVBT_HP_PROFILE = 0,
	DVBT_LP_PROFILE,
	DVBT_PROFILE_UNKNOWN
} DVBT_PROFILE_TYPE;



/***********************************************************/
/*                 DVB-T2 parameters                                                          */
/************************************************************/
/* DVB-T2 FFT mode*/
typedef enum{
	DVBT2_MODE_1K = 0,
	DVBT2_MODE_2K,
	DVBT2_MODE_4K,
	DVBT2_MODE_8K,
	DVBT2_MODE_16K,
	DVBT2_MODE_32K,
	DVBT2_MODE_RESERVED,
	DVBT2_MODE_UNKNOWN
}DVBT2_FFT_MODE_TYPE;


typedef enum
{
	DVBT2_GI_1_32 = 0,
	DVBT2_GI_1_16,
	DVBT2_GI_1_8,
	DVBT2_GI_1_4,
	DVBT2_GI_1_128,
	DVBT2_GI_19_128,
	DVBT2_GI_19_256,
	DVBT2_GI_UNKNOWN
}DVBT2_GUARD_INTERVAL_TYPE;


typedef enum {
	DVBT2_BASE_PROFILE = 0,
	DVBT2_LITE_PROFILE,
	DVBT2_BASE_LITE_UNKNOWN
} DVBT2_BASE_OR_LITE_TYPE;

typedef enum {
	DVBT2_V111 = 0,
	DVBT2_V121,
	DVBT2_V131,
	DVBT2_VERSION_UNKNOWN
} DVBT2_VERSION_TYPE;


/* DVBT2 S1 signaling definition */
typedef enum {
	DVBT2_SISO = 0,
	DVBT2_MISO,
	DVBT2_NON_DVBT2,                         /* Non DVBT2 */
	DVBT2_LITE_SISO,                      /* Reserved */
	DVBT2_LITE_MISO,
	DVBT2_S1_RESERVED5,
	DVBT2_S1_RESERVED6,
	DVBT2_SISO_MISO_UNKNOWN

} DVBT2_SISO_MISO_TYPE;


typedef enum {
	DVBT2_BANDWIDTH_NO_EXT= 0,
	DVBT2_BANDWIDTH_EXTENDED,
	DVBT2_BANDWIDTH_EXT_UNKNOWN
} DVBT2_BANDWIDTH_EXT_TYPE;

typedef enum
{
	DVBT2_QPSK = 0,
	DVBT2_QAM_16,
	DVBT2_QAM_64,
	DVBT2_QAM_256,
	DVBT2_QAM_RESERVED4,
	DVBT2_QAM_RESERVED5,
	DVBT2_QAM_RESERVED6,
	DVBT2_QAM_RESERVED7,
	DVBT2_QAM_UNKNOWN
}DVBT2_CONSTELLATION_TYPE;



typedef enum
{
	DVBT2_CODE_RATE_1_2 = 0,
	DVBT2_CODE_RATE_3_5,
	DVBT2_CODE_RATE_2_3,
	DVBT2_CODE_RATE_3_4,
	DVBT2_CODE_RATE_4_5,
	DVBT2_CODE_RATE_5_6,
	DVBT2_CODE_RATE_1_3,//for t2 lite
	DVBT2_CODE_RATE_2_5,//for t2 lite
	DVBT2_CODE_RATE_UNKNOWN
}DVBT2_CODE_RATE_TYPE;


typedef enum
{
	DVBT2_PP1 =0,
	DVBT2_PP2,
	DVBT2_PP3,
	DVBT2_PP4,
	DVBT2_PP5,
	DVBT2_PP6,
	DVBT2_PP7,
	DVBT2_PP8,
	DVBT2_PP_RESERVED8,       /* Reserved */
	DVBT2_PP_RESERVED9,
	DVBT2_PP_RESERVED10,
	DVBT2_PP_RESERVED11,
	DVBT2_PP_RESERVED12,
	DVBT2_PP_RESERVED13,
	DVBT2_PP_RESERVED14,
	DVBT2_PP_RESERVED15,      /* Reserved */
	DVBT2_PP_UNKNOWN
}DVBT2_PILOT_PATTERN_TYPE;

typedef enum
{
	DVBT2_PAYLOAD_GFPS =0,
	DVBT2_PAYLOAD_GCS,
	DVBT2_PAYLOAD_GSE,
	DVBT2_PAYLOAD_TS,
	DVBT2_PAYLOAD_UNKNOWN
}DVBT2_PLP_PAYLOAD_TYPE;

typedef enum {
	DVBT2_COMMON_PLP = 0,                       /* DVBT2 PLP Common Type */
	DVBT2_DATA_PLP_TYPE1,                       /* DVBT2 PLP Data Type 1 */
	DVBT2_DATA_PLP_TYPE2,                       /* DVBT2 PLP Data Type 2 */
	DVBT2_PLP_TYPE_RSVD3,                       /*Reserved */
	DVBT2_PLP_TYPE_RSVD4,
	DVBT2_PLP_TYPE_RSVD5,
	DVBT2_PLP_TYPE_RSVD6,
	DVBT2_PLP_TYPE_RSVD7,                       /* Reserved */
	DVBT2_PLP_TYPE_UNKNOWN
} DVBT2_PLP_TYPE;

typedef enum
{
	DVBT2_ISSY_NO = 0,
	DVBT2_ISSY_SHORT,
	DVBT2_ISSY_LONG,
	DVBT2_ISSY_UNKNOWN
}DVBT2_ISSY_TYPE;

typedef enum
{
	DVBT2_LDPC_SHORT =0,
	DVBT2_LDPC_NORMAL,
	DVBT2_LDPC_RESERVED,
	DVBT2_LDPC_LENGTH_UNKNOWN
}DVBT2_FEC_LENGTH_TYPE;


typedef enum
{
	DVBT2_NORMAL_MODE =0,
	DVBT2_HEM_MODE,
	DVBT2_STREAM_MODE_UNKNOWN
}DVBT2_STREAM_MODE_TYPE;


typedef struct DVBT_TPS_STRUCT
{
	DVBT_FFT_MODE_TYPE       DvbtFFTMode;
	DVBT_GUARD_INTERVAL_TYPE DvbtGuardInterval;
	DVBT_CONSTELLATION_TYPE  DvbtConstellation;
	DVBT_CODE_RATE_TYPE       DvbtHpCodeRate;
	DVBT_CODE_RATE_TYPE       DvbtLpCodeRate;
	DVBT_HIERARCHY_TYPE      DvbtHierarchy;
	DVBT_PROFILE_TYPE        DvbtProfile;
}DVBT_PARAMS;

typedef struct DVBT2_PARAMS_STRUCT
{
	/*DVB-T2 Transmit params */
	DVBT2_FFT_MODE_TYPE            Dvbt2FFTMode;
	DVBT2_BANDWIDTH_EXT_TYPE  Dvbt2BandwidthExt;
	DVBT2_GUARD_INTERVAL_TYPE    Dvbt2GuardInterval;
	DVBT2_PILOT_PATTERN_TYPE       Dvbt2PilotPattern;
	DVBT2_SISO_MISO_TYPE              Dvbt2SisoMiso;
	DVBT2_VERSION_TYPE                  Dvbt2Version;
	DVBT2_BASE_OR_LITE_TYPE             Dvbt2BaseOrLiteType;
    uint16                       Dvbt2CellId;
    uint16                       Dvbt2NetworkId;
	uint16                       Dvbt2T2SystemId;

	uint8 u8FefType;
	uint32 u32FefLength;
	uint8   u8FefInterval;


	/*
	DVB-T2 PLP Parameters

	For single PLP T2 mode:
	u8DataPlpNumber == 1, u8DataPlpIdArray[0] is the PLP_ID.,
	and the PLP_ID is default  set by demodulator, usually can be used as DVB-T.

	For multi-PLP application:
	u8DataPlpNumber is the data PLP number. the maximum value is 255.

	u8DataPlpIdArray contains all Data PLP ID.  common PLP is not listed in, as common plp is processed by demodulator internally
	*/
	uint8 u8DataPlpNumber;
	uint8 u8DataPlpIdArray[255];

	/*
	When Single PLP used, DVB-T2  demodulator output  single PLP.
	When Multi-PLP used, DVB-T2 demodulator output only one active data PLP
	This is the current received Data PLP parameters
	*/
	uint8                                           u8ActivePlpId;
	DVBT2_PLP_TYPE                         Dvbt2DataPLPType;  /*Data PLP Type1 or Data PLP  Type2*/
	DVBT2_PLP_PAYLOAD_TYPE          Dvbt2PayloadType;
	DVBT2_CONSTELLATION_TYPE       Dvbt2Constellation;
	DVBT2_CODE_RATE_TYPE           Dvbt2CodeRate;
	uint8                          u8ConstellationRotation;
	DVBT2_FEC_LENGTH_TYPE         Dvbt2FecLengthType;
	DVBT2_ISSY_TYPE                   Dvbt2IssyType;
	DVBT2_STREAM_MODE_TYPE     Dvbt2StreamType;
}DVBT2_PARAMS;

typedef struct DVBT_T2_SIGNAL_PARAMS_STRUCT
{
	uint8 DVBTxType;
	DVBT_PARAMS         DvbtSignalParams;
	DVBT2_PARAMS       Dvbt2SignalParams;
}DVB_T_T2_SIGNAL_PARAMS;


//DVBS, DVB-S2
typedef enum
{
	DVBSx_MODU_QPSK = 0,
	DVBSx_MODU_8PSK,
	DVBSx_MODU_16APSK,
	DVBSx_MODU_32APSK,
	DVBSx_MODU_UNKNOWN
}DVBS_S2_CONSTELLATION_TYPE;

typedef enum
{
	DVBS_CODE_RATE_1_2 = 0,		//Code rate 1/2
	DVBS_CODE_RATE_2_3 = 1,		//Code rate 2/3
	DVBS_CODE_RATE_3_4 = 2,		//Code rate 3/4
	DVBS_CODE_RATE_5_6 = 3,		//Code rate 5/6
	DVBS_CODE_RATE_7_8 = 4,		//Code rate 7/8
	DVBS_CODE_RATE_UNKNOWN
}DVBS_CODE_RATE_TYPE;

typedef enum
{
    DVBS2_CODE_RATE_DUMMY = 0,
	DVBS2_CODE_RATE_1_4 = 1,
	DVBS2_CODE_RATE_1_3 = 2,
	DVBS2_CODE_RATE_2_5 = 3,
	DVBS2_CODE_RATE_1_2 = 4,
	DVBS2_CODE_RATE_3_5 = 5,
	DVBS2_CODE_RATE_2_3 = 6,
	DVBS2_CODE_RATE_3_4 = 7,
	DVBS2_CODE_RATE_4_5 = 8,
	DVBS2_CODE_RATE_5_6 = 9,
	DVBS2_CODE_RATE_8_9 = 10,
	DVBS2_CODE_RATE_9_10 = 11,
	DVBS2_CODE_RATE_UNKNOWN
}DVBS2_CODE_RATE_TYPE;

typedef struct DVBSx_SIGNAL_PARAMS_STRUCT
{
	DVBS_S2_CONSTELLATION_TYPE DVBSxModu; //Constellation
	uint8 DVBSxType;                        //ATBM_DVBS_MODE or ATBM_DVBS2_MODE
	DVBS_CODE_RATE_TYPE             DVBSCodeRate;
	DVBS2_CODE_RATE_TYPE           DVBS2CodeRate;
	uint32                                         DVBSxSymbolRateK;
}DVBSx_SIGNAL_PARAMS;
typedef struct st64Data
{
	uint32 ui32Low,ui32High;
}ATBM64Data;

//
ATBM_STATUS   ATBMGetSignalParamsT(DVBT_PARAMS *pDvbTParams);
ATBM_STATUS   ATBMPrintSignalParamsT(DVBT_PARAMS*pDvbTParams );
ATBM_STATUS  ATBMGetDVBTxSigalStrength(uint8 *ui8TxSSI);//only for 2176 specific


ATBM_STATUS   ATBMGetSignalParamsT2(DVBT2_PARAMS *pDvbT2Params);
ATBM_STATUS   ATBMPrintSignalParamsT2(DVBT2_PARAMS *pDvbT2Params);

ATBM_STATUS   ATBMLockFlagTAndT2(uint8 *LockStatus);


#if 0
/****************************function for gui,in the file MerlinSDK.c ***************************/
uint8         ATBMRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr);        //For GUI Version test
uint32        ATBMDVBSRead(uint16 ui16Addr);                              //For GUI Version
uint8         ATBMGetRegSize(uint8 ui8BitWidth); 	//used for gui
//void          ATBMGetRegInfor(uint32 ui32ReadSize,uint8 ui8BaseStart,uint8 ui8SubStart,uint8* ui8PRegWd,uint32 *ui32PReturnValue,CString *pstr);//used for gui
uint8         ATBMGetPLPInfor(uint8 *ui8PAPLPValue);
#endif


/****************************DVB-T and DVB-T2 functions ***************************/
ATBM_STATUS ATBMSetDVBT2DisablePLPDecode(void);  
ATBM_STATUS ATBMSetDVBT2EnablePLPDecode(void);
ATBM_STATUS   ATBMSetDVBTxMode(unsigned char u8bandwidthMHz); //Common API for DVB-T and DVB-T2, called by ATBMSetDVBT2() or ATBMSetDVBT()
//void          ATBMSetDvbtBandwidth(unsigned char u8bandwidthMHz);
ATBM_STATUS   ATBMSetFreqAndRateRatio(stATBMDvbConfig stPara);
ATBM_STATUS   ATBMSetBandWidthPara(stATBMDvbConfig stPara);
ATBM_STATUS   ATBMSetADC(uint8 ui8Adc);
ATBM_STATUS   ATBMSwapIQ(uint8 ui8IQ);

ATBM_STATUS ATBMDVBTxSigalParamsLockCheck(uint8 *pu8LockStatus);
ATBM_STATUS ATBMSetDVBT2BaseProfile(void);
ATBM_STATUS ATBMSetDVBT2LiteProfile(void);
ATBM_STATUS ATBMSetDVBT2BaseAndLiteProfile(void);
ATBM_STATUS ATBMSetAutoPLPT2(void);
/*DVBTx API functions can be used for DVB-T and DVB-T2*/
ATBM_STATUS ATBMTSLockFlag(uint8 *LockStatus);
ATBM_STATUS ATBMDVBTxChannelLockCheck(uint8 *pu8LockStatus);
ATBM_STATUS ATBMDVBTxChannelLockCheckForManual(uint8 *LockStatus);
ATBM_STATUS   ATBMDVBTxSignalIFAGC(uint16 *ui16CalcValue);
ATBM_STATUS ATBMDVBTxSignalQuality(uint8 *pu8SQI);
ATBM_STATUS ATBMGetDVBTxBER(uint32 *pu32BERx10E9);
ATBM_STATUS ATBMGetDVBTxSNR(uint32 *pu32SNRx10);
ATBM_STATUS ATBMGetDVBTxCarrierOffset(int *pi32Value)	;
uint8  ATBMGetDVBTxSampleOffsetPPM(int*pi32Value);

/*DVB-T2 Get status functions*/
ATBM_STATUS   ATBMGetDVBT2SignalQuality(uint8 *ui8T2SQ);
ATBM_STATUS   ATBMGetDVBT2SampleOffsetPPM(int*i32Value);
ATBM_STATUS   ATBMGetDVBT2CFO(int*i32Value);
ATBM_STATUS   ATBMGetDVBT2SNR(uint32 *ui32SNRValue);
ATBM_STATUS   ATBMGetDVBT2BER(uint32 *ui32T2BER);
ATBM_STATUS   ATBMGetDVBT2PostBCHFER(uint32 *ui32T2FER);
ATBM_STATUS   ATBMSetPLPForMultiPLPDVBT2(uint8 u8PLPid );

/*DVB-T get Status Functions*/
ATBM_STATUS   ATBMGetDVBTSigalQuality(uint8 *ui8TSQ);
ATBM_STATUS   ATBMGetDVBTSampleOffsetPPM(int*i32Value);
ATBM_STATUS   ATBMGetDVBTCFO(int*i32Value);
ATBM_STATUS   ATBMGetDVBTSNR(uint32 *ui32SNRValue);
ATBM_STATUS   ATBMGetDVBTBER(uint32 *ui32BER);
ATBM_STATUS   ATBMGetDVBTPER(uint32 *ui32PER);

/****************************DVB-C functions************************************/
ATBM_STATUS   ATBMSetDVBCMode(void );
ATBM_STATUS   ATBMDVBCSet( stATBMDvbConfig stPara);
ATBM_STATUS   ATBMDVBCSetQAM(void);
ATBM_STATUS   ATBMDVBCSetCarrier(uint32 ui32OSCFreq, uint32 ui32SymbolRateK);
ATBM_STATUS   ATBMDVBCSetSymbolRate(uint32 ui32OSCFreq, uint32 ui32SymbolRateK);
/*DVB-C get status Functions,*/
ATBM_STATUS   ATBMTSLockFlag(uint8 *LockStatus);
ATBM_STATUS   ATBMDVBCChannelLockCheck(uint8 *LockStatus);
ATBM_STATUS   ATBMGetDVBCSymbolRate(uint32 *ui32SymRate);
ATBM_STATUS   ATBMGetDVBCCarrierOffset(int *i32COValue);
ATBM_STATUS   ATBMGetDVBCQAM(uint8 *ui8QAMIndex);
ATBM_STATUS   ATBMGetDVBCSNR(int *i32SNR);
ATBM_STATUS   ATBMGetDVBCSignalStrength(int *i32SigStr);
ATBM_STATUS   ATBMGetDVBCBER(uint32 *ui32BER);
ATBM_STATUS   ATBMGetDVBCPER(uint32 *ui32PER);




/****************************DVB-S, DVB-S2 Functions******************************/
ATBM_STATUS   ATBMSetDvbsMode(void );
ATBM_STATUS   ATBMSetDvbs2Mode(void );
ATBM_STATUS   ATBMSetSampleRate(stATBMDvbConfig stPara);

ATBM_STATUS   ATBMTSLockFlag(uint8 *LockStatus);
ATBM_STATUS   ATBMDVBSxChannelLockCheck(uint8 *pu8DvbSxLockStatus);

ATBM_STATUS   ATBMDVBSxSNR(uint32 *pu32SNRx10);
ATBM_STATUS   ATBMDVBSxSignalQuality(uint8 *pu8SQI);
ATBM_STATUS   ATBMDVBSxBER(uint32 *pui32BER);
ATBM_STATUS   ATBMDVBSxSignalParams(DVBSx_SIGNAL_PARAMS *pstDVBSxParams);
ATBM_STATUS   ATBMGetDVBSxCarrierOffset(int32 *i32CarrierOfst);
ATBM_STATUS   ATBMGetDVBSxSymbolRate(uint32 *pui32SymbolRate);
/****************************Status Function,for for DVBS  ******************************/
ATBM_STATUS   ATBMGetDVBSSNR(uint32 *ui32SNR);
ATBM_STATUS   ATBMGetDVBSBER(uint32 *ui32BER);
ATBM_STATUS   ATBMGetDVBSUncorrectablePER(uint32 *ui32PER);
/****************************Status Function,for for DVBS2  ******************************/
ATBM_STATUS   ATBMGetDVBS2BER(uint32 *ui32S2BER);
ATBM_STATUS   ATBMGetDVBS2SNR(uint32 *i32SNR);

/*****************************DVBS S2 Blind scan used*************************************/
typedef struct ATBMDvbSxChannel
{
	uint32 u32FrequencyKHz;		//Center frequency in units of kHz.
	uint32 u32SymbolRateK;		//The symbol rate.
	DVBSx_SIGNAL_PARAMS stDvbSxSignalPrams;
}stATBMDvbSxChannel;

#define MAX_SCANED_CHANNEL_NUMBER 500

struct ATBMDvbSxChannelScanParams
{
	uint32  u32ScanStartFrequencyKHz;  //blind scan start frequency
	uint32 u32ScanStopFrequencyKHz;   //blind scan stop frequency

	uint32 u32MinSymbolRateK;             //min symbol rate can be detected. >1M is default setting
	uint32 u32MaxSymbolRateK;            //max symbol rate can be detected <=45M is default setting
	uint32 u32BlindScanStepKHz;                //Blind Scan step in MHz
	uint32 u32CurrentWorkFreKHz;                //Blind Scan step in MHz

	stATBMDvbSxChannel  ATBMDvbSxChannel[MAX_SCANED_CHANNEL_NUMBER]; //channel list used by SDK blind scan module
	uint32 u32ValidChannelNumber,ui32ScanedValideChannelNum; //record current valid channel counter.

	/*Tuner parameters and  functions*/
	uint32 u32TunerLPF;
	ATBM_STATUS (*pFuncTunerSetFrequency)(uint32 CenterFrequency, uint32 u32SymblRate);
	ATBM_STATUS (*pFuncTunerLockStatus)(uint8 *pLockFlag);
};


/****************************Common Function,for DVB-T, DVB-T2, DVB-C, DVB-S and DVB-S2 operation***************************/
int ATBMLog2(int i32Data);
uint32 Log10Convert(uint32 i32InValue);
uint32 Log10Convertx10(uint32 i32InValue);
uint32  FixPow(uint32 x,uint32 y);
void ATBM64Add(ATBM64Data *pstSum,uint32 ui32Addend);
void ATBM64Mult(ATBM64Data *pstRst,uint32 m1, uint32 m2);
uint32 ATBM64Div(ATBM64Data stDivisor,ATBM64Data stDividend);
void ATBMPrintStr(const char *StrType, const char *StrValue);

uint8    ATBMSetTSMode( MPEGTSMode stTSMode);
ATBM_STATUS ATBMTSLockFlag(uint8 *LockStatus);

ATBM64Data ATBM64DivReturn64(ATBM64Data stDivisor,ATBM64Data stDividend);

ATBM_STATUS   ATBMRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue);
ATBM_STATUS   ATBMWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data);
ATBM_STATUS   ATBMDVBCRead(uint32 uAddress,uint32 *ui32RData);
ATBM_STATUS   ATBMDVBCWrite(uint32 ui32Address, uint32 ui32Data);
ATBM_STATUS   ATBMDVBSWrite(uint16 ui16Addr,uint32 ui32Data);
ATBM_STATUS   ATBMDVBSRead(uint16 ui16Addr,uint32 *ui32RegData);
ATBM_STATUS   ATBMWriteRegArray(uint8 *pRegTable, int iTableLen);
ATBM_STATUS   ATBMLatchOn(void);
ATBM_STATUS   ATBMLatchOff(void);
ATBM_STATUS   ATBMTunerI2CViaDemodOn(void);
ATBM_STATUS   ATBMTunerI2CViaDemodOff(void);
ATBM_STATUS   ATBMHoldDSP(void);
ATBM_STATUS   ATBMStartDSP(void);
ATBM_STATUS   ATBMChipID(uint8 *ui8ChipID);

//ATBM_STATUS ATBMPoweOnInit(void);
ATBM_STATUS ATBMSetSysClkClockDVBSx(void);
ATBM_STATUS ATBMSetSysClkClockDVBTx(void);
ATBM_STATUS ATBMSetSysClkClockDVBC(void);
ATBM_STATUS ATBMSetDvbBWSFI(uint32 u32FsADCKHz, uint32 u32InvsFsAdcValue);
ATBM_STATUS ATBMStandBy(void);
ATBM_STATUS ATBMStandByWakeUp(void);
ATBM_STATUS ATBMSusPend(void);
ATBM_STATUS ATBMSetGpioPins(void);
ATBM_STATUS DVBSS2PhaseNoise(void);

ATBM_STATUS ATBMDVBTxModeDetected(uint8 *pu8DVBTxMode);




/****************************I2C and delay  Function, should be implemented *********************************/
void          ATBMSleep(uint32 ui32ms);
ATBM_STATUS   ATBMMutexIni(void);
void          ATBMMutexLock(void);
void          ATBMMutexUnlock(void);
ATBM_STATUS   ATBMI2CRead(uint8 ui8BaseAddr, uint8 ui8RegisterAddr,uint8 *ui8RegValue);
ATBM_STATUS   ATBMI2CWrite(uint8 ui8BaseAddr, uint8 ui8RegisterAddr, uint8 ui8data);


extern   stATBMDvbConfig gstATBMDvbConfig;
#endif
