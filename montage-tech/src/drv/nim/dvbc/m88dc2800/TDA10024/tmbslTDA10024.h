/**
  Copyright (C) 2008 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA10024.h
 *                %version: CFR_STB#10 %
 *
 * \date          %date_modified%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 * Detailled description may be added here.
 *
 * \section info Change Information
 *
 * \verbatim
   Date          Modified by CRPRNr  TASKNr  Maintenance description
   -------------|-----------|-------|-------|-----------------------------------
   9-JULY-2007  | A.TANT    |       |       | CREATION OF TDA10024 ARCHITECTURE 2.0.0
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/

#ifndef _TMBSLTDA10024_H //-----------------
#define _TMBSLTDA10024_H

#include "..\TDA18250\tmNxTypes.h"
#include "..\TDA18250\tmbslFrontEndTypes.h"
#include "tmbslTDA10024_Cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/


/*============================================================================*/
/*                       MACRO DEFINITION                                     */
/*============================================================================*/
#define TMBSL_TDA10024_NBREG        100

/* SW Error codes */
#define TDA10024_ERR_BASE               (CID_COMP_DEMOD | CID_LAYER_BSL)
#define TDA10024_ERR_COMP               (CID_COMP_DEMOD | CID_LAYER_BSL | TM_ERR_COMP_UNIQUE_START)

#define TDA10024_ERR_BAD_UNIT_NUMBER    (TDA10024_ERR_BASE + TM_ERR_BAD_UNIT_NUMBER)
#define TDA10024_ERR_NOT_INITIALIZED    (TDA10024_ERR_BASE + TM_ERR_NOT_INITIALIZED)
#define TDA10024_ERR_INIT_FAILED        (TDA10024_ERR_BASE + TM_ERR_INIT_FAILED)
#define TDA10024_ERR_BAD_PARAMETER      (TDA10024_ERR_BASE + TM_ERR_BAD_PARAMETER)
#define TDA10024_ERR_NOT_SUPPORTED      (TDA10024_ERR_BASE + TM_ERR_NOT_SUPPORTED)
#define TDA10024_ERR_HW_FAILED          (TDA10024_ERR_COMP + 0x0001)
#define TDA10024_ERR_NOT_READY          (TDA10024_ERR_COMP + 0x0002)
#define TDA10024_ERR_BAD_CRC            (TDA10024_ERR_COMP + 0x0003)
#define TDA10024_ERR_BAD_VERSION        (TDA10024_ERR_COMP + 0x0004)

    typedef enum _tmbslTDA10024ScanMode_t
    {
        tmbslTDA10024ScanModeUnknown = 0,
        tmbslTDA10024ScanModeNormal,
        tmbslTDA10024ScanModeScan
    } tmbslTDA10024ScanMode_t;

    typedef enum _tmbslTDA10024BandType_t
    {
        tmbslTDA10024BandTypeUnknown = 0,
        tmbslTDA10024BandTypeOther,
        tmbslTDA10024BandTypeSecamL
    } tmbslTDA10024BandType_t;

    typedef enum _tmbslTDA10024GPIOConfig_t
    {
        tmbslTDA10024GPIOConfigUnknown = 0,
        tmbslTDA10024GPIOConfigFEL,
        tmbslTDA10024GPIOConfigIT,
        tmbslTDA10024GPIOConfigCTRL
    } tmbslTDA10024GPIOConfig_t;
    
    typedef enum _tmbslTDA10024FECMode_t
    {
        tmbslTDA10024FECModeUnknown = 0,
        tmbslTDA10024FECModeAnnexA,
        tmbslTDA10024FECModeAnnexB,
        tmbslTDA10024FECModeMax
    }tmbslTDA10024FECMode_t;

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/

/**
    \brief  Create an instance of the TDA10024 demodulator by setting its parameters

    \param  demodUnit : Demodulator unit number
    \param  sParam : Setup parameters 
  
     

    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong or
                   the demod instance is already initialized
                              
    \note  Setup parameters \n Especially:
                 -the hardware address of I2C registers table, \n
                 -the pointers of system functions,\n
                 -the pointers of tuner function,\n
                 -the tuner unit number.

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024Init
(
    tmUnitSelect_t            demodUnit,    /* I: Demodulator unit number */
        tmbslFrontEndDependency_t *psSrvFunc,    /* I: Setup parameters        */
        tmbslTDA10024_Cfg_t *psCfg
);


/**
    \brief  Destroy an instance of the TDA10024 demodulator

    \param  demodUnit : Demodulator unit number
    

    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized  
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t 
tmbslTDA10024DeInit 
(
    tmUnitSelect_t demodUnit    /* I: Demod unit number */
);


/**
    \brief  Get version of this device 

    \param  pSWVersion : Pointer to the software version received
    

    \return The call results.
            - TM_OK means the call is successful
             
    \note   Values defined in the tm10024local.h file

    \sa     NA
 */
extern tmErrorCode_t   
tmbslTDA10024GetSWVersion 
(
    ptmSWVersion_t pSWVersion   /* I: Receives SW Version */
);


/**
    \brief  Set the power state of the TDA10024 demodulator

    \param  demodUnit : Demodulator unit number
    \param  ePowerState : Power state of the device
    Especially:
      the demodulator is in standby
      the demodulator is power on
      

    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong  
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024SetPowerState
(
    tmUnitSelect_t demodUnit,   /* I: DemodUnit number          */
    tmPowerState_t ePowerState  /* I: Power state of the device */
);


/**
    \brief  Get the power state of the TDA10024 demodulator

    \param  demodUnit : Demodulator unit number
    \param  pPowerState : Pointer on power state of the device


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong  
             
    \note   NA

    \sa     NA
 */

extern tmErrorCode_t
tmbslTDA10024GetPowerState
(
    tmUnitSelect_t  demodUnit,  /* I: demodUnit number             */
    ptmPowerState_t pPowerState /* O: Receives current power state */
);


/**
    \brief  Program the symbol rate

    \param  demodUnit : Demodulator unit number
    \param  uSR : Symbol rate value expressed in bauds 


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                - TMBSL_ERR_DEMOD_BAD_PARAMETER: the channel spacing is not in range allowed
                - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024SetSR 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number   */
    UInt32         uSR          /* I: Symbol Rate (bauds) */
);


/**
    \brief  Get the current symbol rate

    \param  demodUnit : Demodulator unit number
    \param  puCS : Pointer on symbol rate expressed in baud


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetSR 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number   */
    UInt32         *puCS        /* 0: Symbol Rate (bauds) */
);


/**
    \brief  Program the spectral inversion

    \param  demodUnit : Demodulator unit number
    \param  eSI : Spectral Inversion


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TMBSL_ERR_DEMOD_BAD_PARAMETER: the spectrual inversion takes not allowed values
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong

    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024SetSI 
(
    tmUnitSelect_t      demodUnit,  /* I: Demod unit number  */
    tmFrontEndSpecInv_t eSI         /* I: Spectral Inversion */
);


/**
    \brief  Read the spectral inversion regarding to the value of the 
            IQswapped at the input of the tuner.

    \param  demodUnit : Demodulator unit number
    \param  peSI : Pointer on the Spectral Inversion


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   the register is readed each time

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetSI 
(
    tmUnitSelect_t      demodUnit,  /* I: Demod unit number  */
    tmFrontEndSpecInv_t *peSI       /* O: Spectral Inversion */
);


/**
    \brief  Program the modulation  (4->256-QAM)

    \param  demodUnit : Demodulator unit number
    \param  eMOD : Modulation type


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TMBSL_ERR_DEMOD_BAD_PARAMETER: the modulation type takes not allowed values
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024SetMod 
(
    tmUnitSelect_t         demodUnit,   /* I: Demod unit number */
    tmFrontEndModulation_t eMOD         /* I: Modulation type   */
);


/**
    \brief  Get the current modulation (4->256-QAM)

    \param  demodUnit : Demodulator unit number
    \param  peMOD : Pointer on modulation type


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetMod 
(
    tmUnitSelect_t         demodUnit,   /* I: Demod unit number */
    tmFrontEndModulation_t *peMOD       /* O: Modulation type   */
);


/**
    \brief  Get the BER after Reed Solomon decoder

    \param  demodUnit : Demodulator unit number
    \param  puVBER : Pointer on BER


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
                 
                     
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetBER 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number */
    UInt32         *puVBER      /* O: BER               */
);


/**
    \brief  Calculate the signal to noise ratio in dB 

    \param  demodUnit : Demodulator unit number
    \param  pbSNR : Pointer on channel SNR


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TDA10024_ERR_NOT_SUPPORTED: the modulation is not supported by the device
                     
    \note   if required, the GetSNR functionality can be implemented without using float.
            For example, 5 arrays of 256 bytes (one for each modulation) can be created to give 
            directly the SNR corresponding to the value of TDA10024_MSE_IND register

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetSNR 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number */
    Int8           *pbSNR       /* O: SNR               */
);


/**
    \brief  Calculate frequency error in Hz 

    \param  demodUnit : Demodulator unit number
    \param  plAFC : Pointer on frequency error in Hz


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetAFC 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number */
    Int32          *plAFC       /* O: AFC               */
);


/**
    \brief  Get the symbol frequency error in ppm 
    \param  demodUnit : demodulator unit number
    \param  plACC : pointer on ACC in ppm


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
                 
    \note        formula:
                  if DYN= 0: CKoffset*10e6/2^19 ppm
                  if DYN= 1: CKoffset*10e6/2^18 ppm

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetACC
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number      */
    Int32          *plACC       /* O: frequency error in ppm */
);


/**
    \brief  Read the uncorrected block number

    \param  demodUnit : Demodulator unit number
    \param  puUBK : Pointer on the uncorrected block


    \return The call results.
            - TM_OK means the call is successful
            - else a problem has been detected
                 - TMBSL_ERR_DEMOD_BAD_UNIT_NUMBER: the demod unit number is wrong
                 - TMBSL_ERR_DEMOD_NOT_INITIALIZED: the demod is not initialized
                 - TM_ERR_IIC_ERR: the writting from I2C bus in register is wrong
                
             
    \note   NA

    \sa     NA
 */
extern tmErrorCode_t
tmbslTDA10024GetUBK 
(
    tmUnitSelect_t demodUnit,   /* I: Demod unit number */
    UInt32         *puUBK       /* O: Uncor             */
);


extern tmErrorCode_t 
tmbslTDA10024ManageBandType
(    
    tmUnitSelect_t demodUnit    /* I: Demod unit number */
);

extern tmErrorCode_t
tmbslTDA10024MainAlgo
(
    tmUnitSelect_t       demodUnit, /* I: Demod unit number */
    tmbslFrontEndState_t *peStatus  /* O: time to wait      */
);

extern tmErrorCode_t
tmbslTDA10024StopAlgo
(
    tmUnitSelect_t demodUnit    /* I: Demod unit number */
);

extern tmErrorCode_t 
tmbslTDA10024Gain
(    
    tmUnitSelect_t demodUnit,   /* I: Demod unit number */
    UInt8          bGain,
    UInt8         *pbTest
);

extern tmErrorCode_t
tmbslTDA10024GetLockStatus
(
    tmUnitSelect_t       demodUnit,     /* I: Demod unit number */
    tmbslFrontEndState_t *peLockStatus  /* O: Synchro           */
);

//extern tmErrorCode_t
//tmbslTDA10024GetScanStep
//(
//    tmUnitSelect_t tUnit,       /* I: Demod unit number */
//    Int32          *plScanStep  /* O: step scan in Hz   */
//);


extern tmErrorCode_t
tmbslTDA10024SetI2CSwitchState
(
    tmUnitSelect_t                tUnit,    /* I: Unit number      */
    tmbslFrontEndI2CSwitchState_t eState    /* I: I2C switch state */
);

extern tmErrorCode_t
tmbslTDA10024GetI2CSwitchState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number      */
    tmbslFrontEndI2CSwitchState_t *peState    /* O: I2C switch state */
);


extern tmErrorCode_t
tmbslTDA10024Reset
(
    tmUnitSelect_t tUnit    /* I: Unit number */
);


extern tmErrorCode_t
tmbslTDA10024GetAgcIf
(
    tmUnitSelect_t tUnit,    /* I: Unit number */
    UInt32*        pAgcIf    /* O: AGC IF      */
);

extern tmErrorCode_t
tmbslTDA10024GetAgcTuner
(
    tmUnitSelect_t tUnit,       /* I: Unit number */
    UInt32*        pAgcTuner    /* O: AGC Tuner   */
);

extern tmErrorCode_t
tmbslTDA10024SetIFAGCOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number   */
    tmbslFrontEndGpioConfig_t eState    /* I: IF AGC Output */
);

extern tmErrorCode_t
tmbslTDA10024SetRFAGCOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number   */
    tmbslFrontEndGpioConfig_t eState    /* I: RF AGC Output */
);

extern tmErrorCode_t
tmbslTDA10024SetSampling
(
    tmUnitSelect_t tUnit,
    tmbslTDA10024FECMode_t eMode,
    UInt8 bMFactor,
    UInt8 bNFactor,
    UInt8 bPFactor,
    UInt8 bFSampling
);

extern tmErrorCode_t
tmbslTDA10024GetSampling
(
    tmUnitSelect_t tUnit,
    tmbslTDA10024FECMode_t eMode,
    UInt8 *pbMFactor,
    UInt8 *pbNFactor,
    UInt8 *pbPFactor,
    UInt8 *pbFSampling
);

extern tmErrorCode_t
tmbslTDA10024SetIF
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    UInt32         uIF      /* I: If in Herz  */
);

extern tmErrorCode_t
tmbslTDA10024GetIF
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    UInt32         *puIF    /* I: If in Herz  */
);

extern tmErrorCode_t
tmbslTDA10024SetTSOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number */
    tmbslFrontEndGpioConfig_t eState    /* I: TS Output   */
);

extern tmErrorCode_t
tmbslTDA10024GetTSOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number */
    tmbslFrontEndGpioConfig_t *eState    /* I: TS Output   */
);

extern tmErrorCode_t
tmbslTDA10024SetGPIOConfig(
    tmUnitSelect_t          tUnit,
    tmbslTDA10024GPIOConfig_t eGPIOConfig
);

extern tmErrorCode_t
tmbslTDA10024GetGPIOConfig(
    tmUnitSelect_t          tUnit,
    tmbslTDA10024GPIOConfig_t *peGPIOConfig
);

extern tmErrorCode_t
TDA10024ResetDeltaF
(
    tmUnitSelect_t demodUnit    /* I: Demod unit number */
);

extern tmErrorCode_t
tmbslTDA10024SetFECMode
(
    tmUnitSelect_t         tUnit,
    tmbslTDA10024FECMode_t     eMode
);

extern tmErrorCode_t
tmbslTDA10024GetFECMode
(
    tmUnitSelect_t         tUnit,
    tmbslTDA10024FECMode_t *peMode
);

extern
tmErrorCode_t 
tmbslTDA10024ResetUBK
(    
    tmUnitSelect_t tUnit
);

extern
tmErrorCode_t 
tmbslTDA10024SetScanMode
(    
    tmUnitSelect_t tUnit,
    tmbslTDA10024ScanMode_t eMode
);

extern
tmErrorCode_t 
tmbslTDA10024GetScanMode
(    
    tmUnitSelect_t tUnit,
    tmbslTDA10024ScanMode_t *peMode
);

#ifdef __cplusplus
}
#endif

#endif // TM<MODULE>_H //---------------
