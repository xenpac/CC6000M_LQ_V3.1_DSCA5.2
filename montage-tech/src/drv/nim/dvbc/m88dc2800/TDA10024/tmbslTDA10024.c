/**
$Header: 
(C) Copyright 2007 NXP Semiconductors, All rights reserved

This source code and any compilation or derivative thereof is the sole
property of NXP Corporation and is provided pursuant to a Software
License Agreement.    This code is the proprietary information of NXP
Corporation and is confidential in nature.    Its use and dissemination by
any party other than NXP Corporation is strictly limited by the
confidential information provisions of the Agreement referenced above.
-----------------------------------------------------------------------------
FILE NAME:        tmbslTDA10024.c

DESCRIPTION:    Function for the silicon demodulator TDA10024

DOCUMENT REF: <References to specification or other documents related to
              this module>

NOTES:
*/


/*============================================================================*/
/*                   STANDARD INCLUDE FILES                                   */
/*============================================================================*/
/*
tmCompId                (def errors)
nxtypes                 (standard types)
tmflags                 (dependance machine)
tmbslFrontEndCfgItem.h  (enum config)
tmFrontend.h            (common enum)
tmbslfrontend.h         (def struct init)
tmbslFrontEndTypes.h    (bsl commo, types)
*/

#include "../TDA18250/tmNxTypes.h"
#include "../TDA18250/tmCompId.h"
#include "../TDA18250/tmFrontEnd.h"
#include "../TDA18250/tmbslFrontEndTypes.h"
#include "../TDA18250/tmUnitParams.h"

#include "tmbslTDA10024.h"
#include "tmbslTDA10024local.h"
#include "tmbslTDA10024_cfg.h"

#include "tmbslTDA10024Instance.h"

/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/

/*============================================================================*/
/*                   MACRO DEFINITION                                         */
/*============================================================================*/
/*#define NO_FLOAT*/
#define SEND_TRACEFCT1(x,y)
#define SEND_TRACEFCT2(x,y)
#define SEND_TRACEFCT3(x,y)
#define SEND_TRACEFCT4(x,y)
#define SEND_TRACE(x,y) 

#define NO_FLOAT

/*============================================================================*/
/*                   TYPE DEFINITION                                          */
/*============================================================================*/


/*============================================================================*/
/*                   PUBLIC VARIABLES DEFINITION                              */
/*============================================================================*/

/*============================================================================*/
/*                   STATIC VARIABLES DECLARATIONS                            */
/*============================================================================*/


/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/


/*============================================================================*/
/*                   STATIC FUNCTIONS DECLARATIONS                            */
/*============================================================================*/
static tmErrorCode_t TDA10024WriteSR (tmUnitSelect_t tUnit, UInt32 *puSR);
static tmErrorCode_t TDA10024WriteQAM (tmUnitSelect_t tUnit, UInt32 uMod);
static UInt16        TDA10024_CalcTimer(tmUnitSelect_t tUnit, UInt32 uNbSymbol);
static UInt32        TDA10024_CalcSymbols(tmUnitSelect_t tUnit, UInt32 uNbFrames);
static tmErrorCode_t TDA10024_AlgoInit(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus);
static tmErrorCode_t TDA10024_AlgoGainManagement(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus);
static tmErrorCode_t TDA10024_AlgoSecamLGain(tmUnitSelect_t tUnit);
static tmErrorCode_t TDA10024_AlgoNormalGain(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus, UInt32 *puTime);
static tmErrorCode_t TDA10024_AlgoEqualizerConvergence(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus, tmbslTDA10024AlgoState_t *peNextState);
static tmErrorCode_t TDA10024_AlgoAutoChannel(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus, tmbslTDA10024AlgoState_t *peNextState);
static tmbslFrontEndState_t TDA10024_AlgoMod(tmUnitSelect_t tUnit);
static tmbslFrontEndState_t TDA10024_AlgoClkOffset(tmUnitSelect_t tUnit); 
static tmErrorCode_t TDA10024_AlgoLockVerif(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus, tmbslTDA10024AlgoState_t *peNextState);
static tmErrorCode_t TDA10024_AlgoStuffingIssue(tmUnitSelect_t tUnit, tmbslFrontEndState_t *peStatus);
static tmErrorCode_t TDA10024_AlgoExit(tmUnitSelect_t tUnit);
static tmErrorCode_t TDA10024GetLockStatus(tmUnitSelect_t tUnit, tmbslTDA10024State_t *peLockStatus);

#ifdef NO_FLOAT
void   TDA10024Mul64(UInt32 uUp1, UInt32 uUp2, UInt32* uHighInt, UInt32* uLowInt);
UInt32 TDA10024Div64(UInt32 uHighInt, UInt32 uLowInt, UInt32 uDown);
UInt32 TDA10024MulDiv32(UInt32 uUp1, UInt32 uUp2, UInt32 uDown);
#endif

/*============================================================================*/
/*                   PUBLIC FUNCTIONS DEFINITIONS                             */
/*============================================================================*/


/*============================================================================*/
/* tmbslTDA10024Init                                                          */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024Init
(
    tmUnitSelect_t            tUnit,
    tmbslFrontEndDependency_t *psSrvFunc,
    tmbslTDA10024_Cfg_t *psCfg
)
{
    ptmTDA10024Object_t     pObj = Null;
    tmErrorCode_t           err = TDA10024_ERR_NOT_INITIALIZED;
    //tmbslTDA10024_Cfg_Err_t CfgError;
    //tmbslTDA10024_Cfg_t     sCfgTDA10024;

    if(psSrvFunc == Null || psCfg == Null)
    {
        err = TDA10024_ERR_BAD_PARAMETER;
        return err;
    }
    
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    /* check driver state */
    if (err == TM_OK || err == TDA10024_ERR_NOT_INITIALIZED)
    {
        if (pObj != Null && pObj->sApiFlags.iInit == True)
        {
            err = TDA10024_ERR_NOT_INITIALIZED;
        }
        else 
        {
            /* initialize the Object */
            if (pObj == Null)
            {
                err = TDA10024AllocInstance(tUnit, &pObj);
                if (err != TM_OK || pObj == Null)
                {
                    err = TDA10024_ERR_NOT_INITIALIZED;        
                }
            }

            if (err == TM_OK)
            {

                pObj->tUnit = tUnit;
                pObj->sRWFunc = psSrvFunc->sIo;
                pObj->sTime = psSrvFunc->sTime;
                pObj->sDebug = psSrvFunc->sDebug;

                if(  psSrvFunc->sMutex.Init != Null
                    && psSrvFunc->sMutex.DeInit != Null
                    && psSrvFunc->sMutex.Acquire != Null
                    && psSrvFunc->sMutex.Release != Null)
                {
                    pObj->sMutex = psSrvFunc->sMutex;

                    err = pObj->sMutex.Init(&pObj->pMutex);
                }

                pObj->sApiFlags.iInit = True;

                tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024Init(0x%08X)", tUnit);

                /* default hardware config */
                /* pObj->sConfig.eModeDvbMcns      = OM57XX_MODE_MCNS_DEF; */
                pObj->sConfig.bIFMax            = OM57XX_IFMAX_DEF;
                pObj->sConfig.bIFMin            = OM57XX_IFMIN_DEF;
                pObj->sConfig.bTUNMax           = OM57XX_TUNMAX_DEF;
                pObj->sConfig.bTUNMin           = OM57XX_TUNMIN_DEF;
                pObj->sConfig.bAGCtreshold      = OM57XX_AGCTRESHOLD_DEF;
                pObj->sConfig.bEqualType        = OM57XX_EQUALTYPE_DEF;
                pObj->sConfig.eBandType         = OM57XX_BANDTYPE_DEF;
                pObj->sConfig.bEqConf1          = OM57XX_EQCONF1_DEF;
                pObj->sConfig.bCAR_C            = OM57XX_CAR_C_DEF;
                pObj->sConfig.bGAIN1            = OM57XX_GAIN1_DEF;
                pObj->sConfig.bPolaPWM1         = OM57XX_POLAPWM1_DEF;
                pObj->sConfig.bPolaPWM2         = OM57XX_POLAPWM2_DEF;
                /* pObj->sConfig.bBERdepth         = OM57XX_BERDEPTH_MCNS_DEF; */
                pObj->sConfig.bClkOffsetRange   = OM57XX_CLKOFFSETRANGE_DEF;
                pObj->sConfig.bTUN_IQSwap       = OM57XX_IQSWAP_DEF;
                
                pObj->sConfig.bOUT_bParaSer1    = OM57XX_PARASER1_DEF;
                pObj->sConfig.bOUT_bMSBFirst1   = OM57XX_MSBFIRST1_DEF;
                pObj->sConfig.bOUT_ModeABC1     = OM57XX_MODEABC1_DEF;
                pObj->sConfig.bOUT_ParaDiv1     = OM57XX_PARADIV1_DEF;
                pObj->sConfig.bOUT_OClk2        = OM57XX_OCLK2_DEF;
                pObj->sConfig.bOUT_OClk1        = OM57XX_OCLK1_DEF;
                
                pObj->sConfig.bOUT_bMSBFirst2   = OM57XX_MSBFIRST2_DEF;
                pObj->sConfig.bSwPos            = OM57XX_SWPOS_DEF;
                pObj->sConfig.bSwDyn            = OM57XX_SWDYN_DEF;
                pObj->sConfig.bSwStep           = OM57XX_SWSTEP_DEF;
                pObj->sConfig.bSwLength         = OM57XX_SWLENGTH_DEF;

                /*CfgError = tmbslTDA10024_CFG_GetConfig(&sCfgTDA10024);
                if (CfgError != tmbslTDA10024_Cfg_NoError_E)
                {
                    err = TM_ERR_NOT_SUPPORTED;
                }*/
            }
            
            if (err == TM_OK)
            {
#if 0
                switch (sCfgTDA10024.CompatibilityNb_U)
                {
                case 3:
                    pObj->sConfig.uTUN_IF               = sCfgTDA10024.IF_U;
                    pObj->sConfig.bTUN_IQSwap           = sCfgTDA10024.IQSwapped;
                    pObj->sConfig.uXtalFreq             = sCfgTDA10024.Xtall_U;
                    pObj->sConfig.bDVB_PLL_M_Factor     = sCfgTDA10024.DVB_PLL_MFactor_U;
                    pObj->sConfig.bDVB_PLL_N_Factor     = sCfgTDA10024.DVB_PLL_NFactor_U;
                    pObj->sConfig.bDVB_PLL_P_Factor     = sCfgTDA10024.DVB_PLL_PFactor_U;
                    pObj->sConfig.bDVB_Fsampling        = sCfgTDA10024.DVB_High_Sampling_Clock;
                    pObj->sConfig.bMCNS_PLL_M_Factor    = sCfgTDA10024.MCNS_PLL_MFactor_U;
                    pObj->sConfig.bMCNS_PLL_N_Factor    = sCfgTDA10024.MCNS_PLL_NFactor_U;
                    pObj->sConfig.bMCNS_PLL_P_Factor    = sCfgTDA10024.MCNS_PLL_PFactor_U;
                    pObj->sConfig.bMCNS_Fsampling       = sCfgTDA10024.MCNS_High_Sampling_Clock;
                    pObj->sConfig.bOUT_bParaSer1        = sCfgTDA10024.OUT_ParaSer_U;
                    pObj->sConfig.bOUT_ModeABC1         = sCfgTDA10024.OUT_ModeABC_U;
                    pObj->sConfig.bOUT_ParaDiv1         = sCfgTDA10024.OUT_ParaDiv_U;
                    pObj->sConfig.bOUT_bMSBFirst1       = sCfgTDA10024.OUT_MsbLsb_U;
                    pObj->sConfig.bOUT_OClk1            = sCfgTDA10024.OUT_Poclk_U;
                    pObj->sConfig.bOUT_OClk2            = sCfgTDA10024.OUT_Poclk_U;
                    pObj->sConfig.bPolaPWM1             = sCfgTDA10024.AGC1Pola;
                    pObj->sConfig.bPolaPWM2             = sCfgTDA10024.AGC2Pola;
                    pObj->sConfig.bTUNMax               = sCfgTDA10024.AGCTunMax;
                    pObj->sConfig.bTUNMin               = sCfgTDA10024.AGCTunMin;
                    pObj->sConfig.bIFMax                = sCfgTDA10024.AGCIFMax;
                    pObj->sConfig.bIFMin                = sCfgTDA10024.AGCIFMin;
                    pObj->sConfig.bAGCtreshold          = sCfgTDA10024.AGCThreshold;
                    pObj->sConfig.bEqualType            = sCfgTDA10024.EqualType;
                    pObj->sConfig.eBandType             = sCfgTDA10024.BandType;
                    pObj->sConfig.bEqConf1              = sCfgTDA10024.EqConf1;
                    pObj->sConfig.bCAR_C                = sCfgTDA10024.CAR_C;
                    pObj->sConfig.bGAIN1                = sCfgTDA10024.GAIN1;
                    pObj->sConfig.bSwPos                = sCfgTDA10024.SWPos;
                    break;
                case 2:
                case 1:
                    err = TDA10024_ERR_BAD_VERSION;        
                    break;
                }
#endif
                pObj->sConfig.uTUN_IF               = psCfg->IF_U;
                pObj->sConfig.bTUN_IQSwap           = psCfg->IQSwapped;
                pObj->sConfig.uXtalFreq             = psCfg->Xtall_U;
                pObj->sConfig.bDVB_PLL_M_Factor     = psCfg->DVB_PLL_MFactor_U;
                pObj->sConfig.bDVB_PLL_N_Factor     = psCfg->DVB_PLL_NFactor_U;
                pObj->sConfig.bDVB_PLL_P_Factor     = psCfg->DVB_PLL_PFactor_U;
                pObj->sConfig.bDVB_Fsampling        = psCfg->DVB_High_Sampling_Clock;
                pObj->sConfig.bMCNS_PLL_M_Factor    = psCfg->MCNS_PLL_MFactor_U;
                pObj->sConfig.bMCNS_PLL_N_Factor    = psCfg->MCNS_PLL_NFactor_U;
                pObj->sConfig.bMCNS_PLL_P_Factor    = psCfg->MCNS_PLL_PFactor_U;
                pObj->sConfig.bMCNS_Fsampling       = psCfg->MCNS_High_Sampling_Clock;
                pObj->sConfig.bOUT_bParaSer1        = psCfg->OUT_ParaSer_U;
                pObj->sConfig.bOUT_ModeABC1         = psCfg->OUT_ModeABC_U;
                pObj->sConfig.bOUT_ParaDiv1         = psCfg->OUT_ParaDiv_U;
                pObj->sConfig.bOUT_bMSBFirst1       = psCfg->OUT_MsbLsb_U;
                pObj->sConfig.bOUT_bMSBFirst2       = psCfg->OUT_MsbLsb_U;
                pObj->sConfig.bOUT_OClk1            = psCfg->OUT_Poclk_U;
                pObj->sConfig.bOUT_OClk2            = psCfg->OUT_Poclk_U;
                pObj->sConfig.bOUT_DenPol1          = psCfg->OUT_DEN_Pol_U;
                pObj->sConfig.bOUT_DenPol2          = psCfg->OUT_DEN_Pol_U;
                pObj->sConfig.bOUT_SyncPol1         = psCfg->OUT_SYNC_Pol_U;
                pObj->sConfig.bOUT_SyncPol2         = psCfg->OUT_SYNC_Pol_U;
                pObj->sConfig.bOUT_UncorPol1        = psCfg->OUT_UNCOR_Pol_U;
                pObj->sConfig.bOUT_UncorPol2        = psCfg->OUT_UNCOR_Pol_U;
                pObj->sConfig.bOUT_SyncWidth1       = psCfg->OUT_Sync_Width_U;
                pObj->sConfig.bOUT_SyncWidth2       = psCfg->OUT_Sync_Width_U;
                pObj->sConfig.bPolaPWM1             = psCfg->AGC1Pola;
                pObj->sConfig.bPolaPWM2             = psCfg->AGC2Pola;
                pObj->sConfig.bTUNMax               = psCfg->AGCTunMax;
                pObj->sConfig.bTUNMin               = psCfg->AGCTunMin;
                pObj->sConfig.bIFMax                = psCfg->AGCIFMax;
                pObj->sConfig.bIFMin                = psCfg->AGCIFMin;
                pObj->sConfig.bAGCtreshold          = psCfg->AGCThreshold;
                pObj->sConfig.bEqualType            = psCfg->EqualType;
                pObj->sConfig.eBandType             = psCfg->BandType;
                pObj->sConfig.bEqConf1              = psCfg->EqConf1;
                pObj->sConfig.bCAR_C                = psCfg->CAR_C;
                pObj->sConfig.bGAIN1                = psCfg->GAIN1;
                pObj->sConfig.bSwPos                = psCfg->SWPos;
		

            }
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024Init(0x%08X) failed.", tUnit));

    return err;
}


/*============================================================================*/
/* tmbslTDA10024DeInit                                                        */
/*============================================================================*/
tmErrorCode_t 
tmbslTDA10024DeInit
(
    tmUnitSelect_t tUnit
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);
    
    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024DeInit(0x%08X)", tUnit);

    if(err == TM_OK)
    {
        if(pObj->sMutex.DeInit != Null)
        {
            if(pObj->pMutex != Null)
            {
                err = pObj->sMutex.DeInit(pObj->pMutex);
            }

            pObj->sMutex.Init = Null;
            pObj->sMutex.DeInit = Null;
            pObj->sMutex.Acquire = Null;
            pObj->sMutex.Release = Null;

            pObj->pMutex = Null;
        }
    }

    err = TDA10024DeAllocInstance(tUnit);

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024DeInit(0x%08X) failed.", tUnit));

    return err;
}
/*============================================================================*/
/* tmbslTDA10024GetSWVersion                                                  */
/*============================================================================*/
tmErrorCode_t     
tmbslTDA10024GetSWVersion
(
    ptmSWVersion_t      pSWVersion
)
{
    pSWVersion->compatibilityNr = TDA10024_BSL_COMP_NUM;
    pSWVersion->majorVersionNr  = TDA10024_BSL_MAJOR_VER;
    pSWVersion->minorVersionNr  = TDA10024_BSL_MINOR_VER;

    return TM_OK;
}

/*============================================================================*/
/* tmbslTDA10024Reset                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024Reset
(
    tmUnitSelect_t tUnit    /* I: Unit number */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
        
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024Reset(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = TDA10024WriteInit(tUnit);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024Reset(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetPowerState                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetPowerState
(
    tmUnitSelect_t tUnit,
    tmPowerState_t ePowerState
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uByte = 0; 

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetPowerState(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        pObj->ePowerState = ePowerState;
        
        /* Implementation */
        switch (ePowerState)
        {
            case tmPowerStandby:
                uByte = TDA10024_GPR_STDBY_BIT;
                /* save the test register */
                err = TDA10024Read(tUnit, TDA10024_TEST_IND, 1, &(pObj->uTestRegSave));
                if (err== TM_OK)
                {
                    /* set the chip in standby mode */
                    err = TDA10024WriteBit(tUnit, TDA10024_GPR_IND, TDA10024_GPR_STDBY_BIT, uByte);
                }
                break;
            case tmPowerOn:
                uByte = 0;
                /* set the chip in power on mode */
                err = TDA10024WriteBit(tUnit, TDA10024_GPR_IND, TDA10024_GPR_STDBY_BIT, uByte);
                if (err== TM_OK)
                {
                    /* restore the test register */
                    err = TDA10024Write(tUnit, TDA10024_TEST_IND, 1, &(pObj->uTestRegSave));
                }
                break;
                
            case tmPowerOff:
            case tmPowerSuspend:
            default:
                err = TDA10024_ERR_BAD_PARAMETER;
                break;
        }

        (void)TDA10024MutexRelease(pObj);
    }    
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetPowerState(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetPowerState                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetPowerState
(
    tmUnitSelect_t  tUnit,
    ptmPowerState_t pPowerState
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10023GetPowerState(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        *pPowerState = pObj->ePowerState;

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetPowerState(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetBER                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetBER
(
    tmUnitSelect_t tUnit,
    UInt32         *puBer
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               puBerBuf[3], puUncorBuf[2], uBerDepth;
    UInt32              uUncor;
    UInt32              uMeanBer;
    static UInt32       suPrevBer[TDA10024_MAX_UNITS][64];
    static UInt32       suIndex[TDA10024_MAX_UNITS] = {0,0,0,0};
    static UInt32       suWindowSize[TDA10024_MAX_UNITS] = {64,64,64,64};
    UInt32              uIndexFromtUnit= UNIT_PATH_INDEX_GET(tUnit);

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetBER(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
        {
            if (err == TM_OK)
            {
                /* MCNS mode */
                /* Read RSBER */
                err = TDA10024Read(tUnit, TDA10024_RSBERLO_IND, 2, puBerBuf);
            }

            if (err == TM_OK)
            {
            /* Read RSUNCOR */
            err = TDA10024Read(tUnit, TDA10024_RSUNCORLO_IND, 2, puUncorBuf);
            } 
            if (err == TM_OK)
            {
                *puBer = puBerBuf[1]<<8 | puBerBuf[0];
                uUncor = puUncorBuf[1]<<8 | puUncorBuf[0];

                /* We consider that there are 16 wrong bits per uncor */
                *puBer += (uUncor*16);


                /* read the BER depth */
                err = TDA10024Read(tUnit, TDA10024_RSCFG_IND, 1, &uBerDepth);
            }
            
            if (err == TM_OK)
            {
                if(((uBerDepth & TDA10024_RSCFG_PRG_TBER_MSK)>>2) == 0x01)
                {
                    if ( (pObj->sConfig.uBERwindow > 0) && (pObj->sConfig.uBERwindow < 64) )
                    {
                        UInt32 i;

                        /* If not already done, configure sliding window */
                        if (suWindowSize[uIndexFromtUnit] != pObj->sConfig.uBERwindow)
                        {
                            suWindowSize[uIndexFromtUnit] = pObj->sConfig.uBERwindow;
                            suIndex[uIndexFromtUnit] = 0;
                        }

                        uMeanBer = *puBer;

                        /* Calculate the mean value with a sliding window of N samples */
                        for (i = 0; i<pObj->sConfig.uBERwindow; i++)
                        {
                            uMeanBer += suPrevBer[uIndexFromtUnit][i];
                        }

                        uMeanBer -= suPrevBer[uIndexFromtUnit][suIndex[uIndexFromtUnit]];
                        suPrevBer[uIndexFromtUnit][suIndex[uIndexFromtUnit]] = *puBer;
                        suIndex[uIndexFromtUnit]++;
                        if(suIndex[uIndexFromtUnit] == pObj->sConfig.uBERwindow)
                            suIndex[uIndexFromtUnit] = 0;

                        *puBer = uMeanBer/pObj->sConfig.uBERwindow;
                    }
                    else
                        err = TM_ERR_NOT_SUPPORTED;
                }

                if (err == TM_OK)
                {
                    switch(uBerDepth & TDA10024_RSCFG_PRG_TBER_MSK)
                    {
                        case 0x00:    /* 262144 */
                            *puBer *= 545;     /* 100000000/(262144*7) = 54.5         (7 is the number of bits per symbol) */
                            *puBer += 5;
                            *puBer /= 10;
                            break;
                        case 0x04:    /* 2097152 */
                            *puBer *= 681;     /* 100000000/(2097152*7) = 6.812 */
                            *puBer += 50;
                            *puBer /= 100;
                            break;
                        case 0x08:    /* 16777216 */
                            *puBer *= 851;     /* 100000000/(16777216*7) = 0.8515 */
                            *puBer += 500;
                            *puBer /= 1000;
                            break;
                        case 0x0c:    /* 134217728 */
                            *puBer *= 1064;     /* 100000000/(134217728*7) = 0.1064 */
                            *puBer += 5000;
                            *puBer /= 10000;
                            break;
                    }
                }
            }

        }
        else
        {
            if (err == TM_OK)
            {
                /* DVB mode */
                err = TDA10024Read(tUnit, TDA10024_BERLSB_IND, 3, puBerBuf);
            }
            if (err == TM_OK)
            {
                *puBer = puBerBuf[2]<<16 | puBerBuf[1]<<8 | puBerBuf[0];

                /* read the BER depth */
                err = TDA10024Read(tUnit, TDA10024_FECDVBCFG1_IND, 1, &uBerDepth);
            }
            if (err == TM_OK)
            {
                switch(uBerDepth & TDA10024_FECDVBCFG1_PVBER_MSK)
                {
                case 0x00:    /* 1,00E+05 */
                    *puBer *= 1000;
                    break;
                case 0x40:    /* 1,00E+06 */
                    *puBer *= 100;
                    break;
                case 0x80:    /* 1,00E+07 */
                    *puBer *= 10;
                    break;
                case 0xc0:    /* 1,00E+08 */
                    break;
                }
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetBER(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetSNR                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetSNR
(
    tmUnitSelect_t tUnit,
    Int8           *pbSNR
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32              dwSNR = 0;
    UInt32              dwMSE = 0;
    UInt32              uCounter = 0;
    UInt8               uMSE = 0;

    /* Get a driver instance */
    err = TDA10024GetInstance(tUnit, &pObj);

    if (err == TM_OK)
    {
        /* Try to acquire driver mutex */
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        tmDBGPRINTEx(DEBUGLVL_BLAB, "tmbslTDA10024GetSNR(0x%08X)", tUnit);

        if(pbSNR == Null)
        {
            err = TDA10024_ERR_BAD_PARAMETER;
        }
        
        if(err == TM_OK)
        {
            for(uCounter = 0; uCounter < 10; uCounter++)
            {
                /* Average value of the MSE on 10 values */
                err = TDA10024Read(tUnit, TDA10024_MSE_IND, 1, &uMSE);

                if(err == TM_OK)
                {
                    dwMSE += uMSE;
                }
                else
                {
                    break;
                }
            }
        }

        if(err == TM_OK)
        {
            /* Calculate the SNR regarding to the modulation */
            switch (pObj->sCurrentChannel.eMOD)
            {
            case tmFrontEndModulationQam16:
                if((dwMSE>=180) && (dwMSE<=800))
                {
                    dwSNR = (1950000/(32*dwMSE + 1380)) + 108;
                }
                break;

            case tmFrontEndModulationQam32:
                if((dwMSE>=130) && (dwMSE<=640))
                {
                    dwSNR = (2150000/(40*dwMSE + 5000)) + 135;
                }
                break;

            case tmFrontEndModulationQam64:
                if((dwMSE>=90) && (dwMSE<=400))
                {
                    dwSNR = (2100000/(40*dwMSE + 5000)) + 125;
                }
                break;

            case tmFrontEndModulationQam128:
                if((dwMSE>=80) && (dwMSE<=330))
                {
                    dwSNR = (1850000/(38*dwMSE + 4000)) + 138;
                }
                break;

            case tmFrontEndModulationQam256:
                if((dwMSE>=100) && (dwMSE<=250))
                {
                    dwSNR = (1800000/(100*dwMSE + 400)) + 203;
                }
                break;

            default:
                err = TDA10024_ERR_NOT_SUPPORTED;
                break;
            }

            /* Update value */
            *pbSNR = (Int8)((dwSNR+5)/10);
        }

        /* Release driver mutex */
        (void)TDA10024MutexRelease(pObj);
    }

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetAFC                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetAFC
(
    tmUnitSelect_t tUnit,
    Int32          *plAFC
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetAFC(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        /* read the registers */
        err = TDA10024Read(tUnit, TDA10024_AFC_IND, 1, (UInt8*)plAFC);

        if (err == TM_OK)
        {
            /* signed value */
            if (*plAFC & 0x80)
            {
                *plAFC |= 0xFFFFFF00;
            }

            /* calculate the value in Hz */
            *plAFC *= (Int32)((pObj->sCurrentChannel.uSR + 256)/512);

            /* invert the AFC to display the offset and not the correction */
            if (pObj->sConfig.bTUN_IQSwap == 1)
            {
                *plAFC = -*plAFC;
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetAFC(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetACC                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetACC
(
    tmUnitSelect_t tUnit,
    Int32          *plACC
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetACC(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        err = TDA10024Read(tUnit, TDA10024_CKOFF_IND, 1, (UInt8*)plACC);

        if (err == TM_OK)
        {
            /* signed value if needed -> 8 bits */
            if (*plACC & 0x80)
                *plACC |= 0xFFFFFF00;

            /* calculate the error in ppm - assumed +/-480ppm for DYN */
            *plACC *= 1000000;
            *plACC /= 262144;     /* 262144 = 2^18 */

            /* add constant use in the algo to extend the clk offset range */
            /* TBD */
            *plACC += pObj->lClkOffCst;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetACC(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetUBK                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetUBK
(
    tmUnitSelect_t tUnit,
    UInt32         *puUBK
)
{
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    ptmTDA10024Object_t pObj = Null;
    UInt8               puBytes[4];

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetUBK(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        /* Implementation */
        if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
        {
            /* MCNS mode */
            if (err == TM_OK)
            {
                err = TDA10024Read(tUnit, TDA10024_CPT_RSB_UNCOR1_IND,    4, puBytes);
            }
            if (err == TM_OK)
            {
                *puUBK = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

                /* reset the counter is needed if there are uncors */
                //if (*puUBK)
                //{
                //    err = TDA10024WriteBit(tUnit, TDA10024_RSCFG_IND, TDA10024_RSCFG_CLB_CPT_RSB_BIT, 0);
                //    if (err == TM_OK)
                //    {
                //        err = TDA10024WriteBit(tUnit, TDA10024_RSCFG_IND, TDA10024_RSCFG_CLB_CPT_RSB_BIT, TDA10024_RSCFG_CLB_CPT_RSB_BIT);
                //    }
                //}
            }
        }
        else
        {
            /* DVB mode */
            if (err == TM_OK)
            {
                err = TDA10024Read(tUnit, TDA10024_CPT_TSP_UNCOR1_IND,    4, puBytes);
            }
            if (err == TM_OK)
            {
                *puUBK = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0];

                ///* reset the counter is needed if there are uncors */
                //if (*puUBK)
                //{
                //    err = TDA10024WriteBit(tUnit, TDA10024_FECDVBCFG1_IND, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT, 0);
                //    if (err == TM_OK)
                //    {
                //        err = TDA10024WriteBit(tUnit, TDA10024_FECDVBCFG1_IND, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT);
                //    }
                //}
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetUBK(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024MainAlgo                                                      */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024MainAlgo
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peStatus
)
{
    tmErrorCode_t            err = TDA10024_ERR_NOT_INITIALIZED;
    ptmTDA10024Object_t      pObj = Null;
    tmbslTDA10024AlgoState_t eNextState;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024MainAlgo(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        *peStatus = tmbslFrontEndStateSearching;

        /* init the algo */
        if ( pObj->bState == tmbslTDA10024AlgoStateInit )
        {
            err = TDA10024_AlgoInit(tUnit, peStatus);
            pObj->bState++;
        }

        /* Gain Management */
        else if ( pObj->bState == tmbslTDA10024AlgoStateGainManagement )
        {
            err = TDA10024_AlgoGainManagement(tUnit, peStatus);
            if (err == TM_OK)
            {
                if (*peStatus == tmbslFrontEndStateSearching)
                {
                    pObj->bState++;
                }
                else
                {
                    pObj->bState = tmbslTDA10024AlgoStateInit;
                }
            }
        }

        /* Equalizer Convergence */
        else if ( pObj->bState == tmbslTDA10024AlgoStateEqualizerConvergence )
        {
            /* Based on the flag iScan, the EqualizerConvergence will
            set the next state either to Init or to AutoChannel.
            In scanning we don't do the whole algorithm */
            err = TDA10024_AlgoEqualizerConvergence(tUnit, peStatus, &eNextState);
            if (err == TM_OK)
            {
                pObj->bState = eNextState;
            }
        }
        
        /* Auto Channel */
        else if (pObj->bState == tmbslTDA10024AlgoStateAutoChannel)
        {
            /*
            AutoChannel returns:                                             
            No carrier if there is not carrier                    
            Searching if the search is not complete.
            In the case go to the algo state specified by AutoChannel
            (If not synchronised but not finished to check all possibilities then it will return to Equalizer convergence */
            err = TDA10024_AlgoAutoChannel(tUnit, peStatus, &eNextState);
            if (err == TM_OK)
            {
                pObj->bState = eNextState;
            }
        }

        /* Lock Verif */
        else if (pObj->bState == tmbslTDA10024AlgoStateLockVerif)
        {
            err = TDA10024_AlgoLockVerif(tUnit, peStatus, &eNextState);
            if (err == TM_OK)
            {
                pObj->bState = eNextState;
            }
        }

        /* Stuffing Issue */
        else if (pObj->bState == tmbslTDA10024AlgoStateStuffingIssue)
        {
            err = TDA10024_AlgoStuffingIssue(tUnit, peStatus);
        }

        if (err == TM_OK)
        {
            /* End of Algo and Reset if needed */
            if ((*peStatus == tmbslFrontEndStateLocked) || (*peStatus == tmbslFrontEndStateNotLocked))
            {
                TDA10024_AlgoExit(tUnit);
                pObj->bState = tmbslTDA10024AlgoStateInit;
            }
        }
        
        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024MainAlgo(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024StopAlgo                                                      */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024StopAlgo
(
    tmUnitSelect_t tUnit
)
{
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    ptmTDA10024Object_t pObj = Null;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024StopAlgo(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if(err == TM_OK)
    {
        TDA10024_AlgoExit(tUnit);
        pObj->bState = tmbslTDA10024AlgoStateInit;
        (void)TDA10024MutexRelease(pObj);    
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024StopAlgo(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetLockStatus                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetLockStatus
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peLockStatus
)
{
    
    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    tmbslTDA10024State_t eLockStatus = tmbslTDA10024StateUnknown;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetLockStatus(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = TDA10024GetLockStatus(tUnit, &eLockStatus);

        if (err == TM_OK)
        {
            if (eLockStatus == tmbslTDA10024StateLocked)
            {
                *peLockStatus = tmbslFrontEndStateLocked;
            }
            else
            {
                *peLockStatus = tmbslFrontEndStateNotLocked;
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetLockStatus(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetAgcIf                                                      */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetAgcIf
(
    tmUnitSelect_t tUnit,    /* I: Unit number */
    UInt32*        pAgcIf    /* O: AGC IF      */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetAgcIf(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        /* set MSBs to '0' */
        *pAgcIf = 0;
        err = TDA10024Read(tUnit, TDA10024_AGCIF_IND, 1, (UInt8*)pAgcIf);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetAgcIf(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetAgcTuner                                                   */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetAgcTuner
(
    tmUnitSelect_t tUnit,       /* I: Unit number */
    UInt32*        pAgcTuner    /* O: AGC Tuner   */
)
{
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    ptmTDA10024Object_t pObj = Null;
    
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetAgcTuner(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        /* set MSBs to '0' */
        *pAgcTuner = 0;
        err = TDA10024Read(tUnit, TDA10024_AGCTUN_IND, 1, (UInt8*)pAgcTuner);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetAgcTuner(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetSR                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetSR
(
    tmUnitSelect_t tUnit,
    UInt32         uSR
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetSR(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    /* test the parameter value */
    /*
    SACLK = SysClk/2     (SACLK max = 36MHz)
    (SACLK/64) < uSR < (SACLK/4)
    or with high sampling clock (SACLK = SysClk, SACLK max = 72MHz)
    (SACLK/128) < uSR < (SACLK/8)
    */
    if (err == TM_OK)
    {
        if (uSR < (pObj->uSysClk/(2*64)) || uSR > (pObj->uSysClk/(2*4)))
        {
            err = TDA10024_ERR_BAD_PARAMETER;
        }
    
        if (err == TM_OK)
        {
            err = TDA10024WriteSR (tUnit, &uSR);
        }
        if (err == TM_OK)
        {
            /* update current config */
            pObj->uRefSR = uSR;
            pObj->sCurrentChannel.uSR = uSR;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetSR(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetSR                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetSR
(
    tmUnitSelect_t tUnit,
    UInt32         *puSR
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetSR(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        *puSR = pObj->sCurrentChannel.uSR;

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetSR(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetSI                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetSI
(
    tmUnitSelect_t      tUnit,
    tmFrontEndSpecInv_t eSI
)
{
    
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uSi = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetSI(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* test the parameter value*/
        if (eSI >= tmFrontEndSpecInvMax)
        {
            err = TDA10024_ERR_BAD_PARAMETER;
        }
    }
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        switch (eSI)
        {
        case tmFrontEndSpecInvAuto:
            pObj->sApiFlags.iAutoSi = True;
            break;

        case tmFrontEndSpecInvOff:
            pObj->sApiFlags.iAutoSi = False;
            if (pObj->sConfig.bTUN_IQSwap)
                uSi = TDA10024_CARCONF_INVIQ_BIT;
            else
                uSi = 0;
            break;

        case tmFrontEndSpecInvOn:
            pObj->sApiFlags.iAutoSi = False;
            if (pObj->sConfig.bTUN_IQSwap)
                uSi = 0;
            else
                uSi = TDA10024_CARCONF_INVIQ_BIT;
            break;

        default:
            err = TDA10024_ERR_BAD_PARAMETER;
            break;
        }

        if (err == TM_OK)
        {
            /* write the spectral inversion */
            if(pObj->sApiFlags.iAutoSi == False)
            {
                err = TDA10024WriteBit(tUnit, 
                    TDA10024_CARCONF_IND, 
                    TDA10024_CARCONF_AUTOINVIQ_BIT | TDA10024_CARCONF_INVIQ_BIT, 
                    uSi);
            }
            else
            {
                err = TDA10024WriteBit(tUnit, 
                    TDA10024_CARCONF_IND, 
                    TDA10024_CARCONF_AUTOINVIQ_BIT, 
                    TDA10024_CARCONF_AUTOINVIQ_BIT);
            }
        }
        if (err == TM_OK)
        {
            /* update value */
            pObj->sCurrentChannel.eSI = eSI;

            /* start algo */
            pObj->sApiFlags.iStartAlgo = True;
        }

        (void)TDA10024MutexRelease(pObj);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetSI(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetSI                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetSI
(
    tmUnitSelect_t      tUnit,
    tmFrontEndSpecInv_t *peSI
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uSi;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetSI(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        /* Implementation */
        err = TDA10024Read(tUnit, TDA10024_CARCONF_IND, 1, &uSi);

        if (err == TM_OK)
        {
            /* test different cases regarding to the spectral inversion in the tuner */
            if ((uSi & TDA10024_CARCONF_INVIQ_BIT) && pObj->sConfig.bTUN_IQSwap)
                *peSI = tmFrontEndSpecInvOff;
            else if ((uSi & TDA10024_CARCONF_INVIQ_BIT) && !pObj->sConfig.bTUN_IQSwap)
                *peSI = tmFrontEndSpecInvOn;
            else if (!(uSi & TDA10024_CARCONF_INVIQ_BIT) && pObj->sConfig.bTUN_IQSwap)
                *peSI = tmFrontEndSpecInvOn;
            else if (!(uSi & TDA10024_CARCONF_INVIQ_BIT) && !pObj->sConfig.bTUN_IQSwap)
                *peSI = tmFrontEndSpecInvOff;
                
            /* update value */
            pObj->sCurrentChannel.eSI = *peSI;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetSI(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetMod                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetMod
(
    tmUnitSelect_t         tUnit,
    tmFrontEndModulation_t eMOD
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32              uMod = 2;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetMod(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* test the parameter value */
        if (eMOD >= tmFrontEndModulationMax)
        {
            err = TDA10024_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        /* In MCNS mode, only 64QAM and 256QAM are allowed */
        if ((pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB) &&
            (eMOD != tmFrontEndModulationAuto) &&
            (eMOD != tmFrontEndModulationQam64) &&
            (eMOD != tmFrontEndModulationQam256) )
        {
            err = TDA10024_ERR_BAD_PARAMETER;
        }
    }

    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Implementation */
        switch (eMOD)
        {
        case tmFrontEndModulationAuto:
            pObj->sApiFlags.iAutoQam = True;
            uMod = 2;
            break;
        case tmFrontEndModulationQam16:
            pObj->sApiFlags.iAutoQam = False;
            uMod = 0;
            break;
        case tmFrontEndModulationQam32:
            pObj->sApiFlags.iAutoQam = False;
            uMod = 1;
            break;
        case tmFrontEndModulationQam64:
            pObj->sApiFlags.iAutoQam = False;
            uMod = 2;
            break;
        case tmFrontEndModulationQam128:
            pObj->sApiFlags.iAutoQam = False;
            uMod = 3;
            break;
        case tmFrontEndModulationQam256:
            pObj->sApiFlags.iAutoQam = False;
            uMod = 4;
            break;
        default:
            err = TDA10024_ERR_BAD_PARAMETER;
            break;
        }

        if (err == TM_OK)
        {
            /* write the new modulation */
            err = TDA10024WriteQAM(tUnit, uMod);
        }

        if (err == TM_OK)
        {
            /* update current config */
            if (tmFrontEndModulationAuto == eMOD)
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam64;
            else
                pObj->sCurrentChannel.eMOD = eMOD;

            /* start algo */
            pObj->sApiFlags.iStartAlgo = True;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetMod(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetMod                                                        */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetMod
(
    tmUnitSelect_t         tUnit,
    tmFrontEndModulation_t *peMOD
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetMod(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        /* Implementation */
        *peMOD = pObj->sCurrentChannel.eMOD;

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetMod(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024ManageBandType                                                */
/*============================================================================*/
tmErrorCode_t 
tmbslTDA10024ManageBandType
(    
    tmUnitSelect_t tUnit
) 
{ 
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uValue,uByte; 

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetMod(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        uValue = pObj->sConfig.eBandType; 
        /* enhanced SECAM-L algo */
        if ( uValue == tmbslTDA10024BandTypeSecamL )    
        {    
            if (err == TM_OK)
            {
                /* set POSAGC to 0  */
                err = TDA10024Read(tUnit, TDA10024_AGCCONF2_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte &= 0xdf ;    /* set bit 5 to 0 */
                err = TDA10024Write(tUnit, TDA10024_AGCCONF2_IND, 1, &uByte);
            } 
            if (err == TM_OK)
            {
                /* set AUTOGNYQ to 0 */
                err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte &= 0x7f;    /* set bit7 to 0 */
                    /* set register GAIN1 to 0 */
                uByte &= 0xFB;     /* set bit2 to 0 */
                err = TDA10024Write(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                /* set register GAIN2 to 0 and register GAIN3 to 0 */
                err = TDA10024Read(tUnit, TDA10024_CLKCONF_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte &= 0xcf;    /* set bit5 to 0 */
                err = TDA10024Write(tUnit, TDA10024_CLKCONF_IND,1, &uByte);
            }
            if (err == TM_OK)
            {
                /* set register GAAF to 0 */
                err = TDA10024Read(tUnit, TDA10024_GAIN2_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte &= 0x8f;    /* set bits 6,5,4 to 0 */
                err = TDA10024Write(tUnit, TDA10024_GAIN2_IND,1, &uByte);
            }

        } /* end of SECAM-L algo */
        else 
        { 
            if (err == TM_OK)
            {
                /* set POSAGC to 1 */
                err = TDA10024Read(tUnit, TDA10024_AGCCONF2_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte |= 0x20 ;    /* set bit 5 to 1 */
                err = TDA10024Write(tUnit, TDA10024_AGCCONF2_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                /* set AUTOGNYQ to 1 */
                err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
            }
            if (err == TM_OK)
            {
                uByte |= 0x80;    /* set bit7 to 1 */
                err = TDA10024Write(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024ManageBandType(0x%08X) failed.", tUnit));

    return err; 
} 

///*============================================================================*/
///* tmbslTDA10024GetScanStep                                                     */
///*============================================================================*/
//tmErrorCode_t
//tmbslTDA10024GetScanStep
//(
//    tmUnitSelect_t tUnit,
//    Int32          *plScanStep /* Hz */
//)
//{
//    ptmTDA10024Object_t pObj = Null;
//    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
//
//    /* check input parameters */
//    err = TDA10024GetInstance(tUnit, &pObj);
//
//    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetSRStep(0x%08X)", tUnit);
//
//    if (err == TM_OK)
//    {
//        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
//    }
//    
//    if (err == TM_OK)
//    {
//        *plScanStep = pObj->lScanStep;
//
//        (void)TDA10024MutexRelease(pObj);
//    }
//    
//    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetSRStep(0x%08X) failed.", tUnit));
//
//    return err;
//}

/*============================================================================*/
/* tmbslTDA10024SetI2CSwitchState                                             */
/*============================================================================*/
extern tmErrorCode_t
tmbslTDA10024SetI2CSwitchState
(
    tmUnitSelect_t                tUnit,    /* I: Unit number      */
    tmbslFrontEndI2CSwitchState_t eState    /* I: I2C switch state */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uBytes[2] = {0, 0};
    
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetI2CSwitchState(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        if (pObj->sConfig.bEnableI2cSwitch)
        {
            if (eState == tmbslFrontEndI2CSwitchStateClosed)
            {
                /* TDA10024 I2C Switch Close Protocol - Start - Do not Alter */
                if (err == TM_OK)
                {
                    err = TDA10024Read(tUnit, TDA10024_TEST_IND, 1, uBytes);
                }

                if (err == TM_OK)
                {
                    uBytes[0] &= ~TDA10024_TEST_SCLT0_BIT;
                    err = TDA10024Write(tUnit, TDA10024_TEST_IND, 1, uBytes);
                }

                if (err == TM_OK)
                {
                    uBytes[0] |= TDA10024_TEST_BYPIIC_BIT;
                    err = TDA10024Write(tUnit, TDA10024_TEST_IND, 1, uBytes);
                }
                /* TDA10024 I2C Switch Close Protocol - End - Do not Alter */
            }
            else 
            {
                err = TDA10024WriteBit(tUnit,
                    TDA10024_TEST_IND, TDA10024_TEST_BYPIIC_BIT|TDA10024_TEST_SCLT0_BIT, 
                    TDA10024_TEST_SCLT0_BIT);
            }            
        }

        (void)TDA10024MutexRelease(pObj);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetI2CSwitchState(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetI2CSwitchState                                             */
/*============================================================================*/
extern tmErrorCode_t
tmbslTDA10024GetI2CSwitchState
(
    tmUnitSelect_t                tUnit,      /* I: Unit number      */
    tmbslFrontEndI2CSwitchState_t *peState    /* O: I2C switch state */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uValue;
        
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetI2CSwitchState(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        err = TDA10024Read(tUnit,TDA10024_TEST_IND, 1, &uValue);

        if (err ==TM_OK)
        {
            if (TDA10024_TEST_BYPIIC_BIT & uValue)
            {
                *peState = tmbslFrontEndI2CSwitchStateClosed;
            }
            else
            {
                *peState = tmbslFrontEndI2CSwitchStateOpen;
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetI2CSwitchState(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetIFAGCOutput                                                */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetIFAGCOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number   */
    tmbslFrontEndGpioConfig_t eState    /* I: IF AGC Output */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uState;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetIFAGCOutput(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if(eState == tmbslFrontEndGpioConfTriState)
        {
            uState = TDA10024_AGCCONF2_TRIAGC_BIT;
        }
        else
        {
            uState = 0;
        }

        err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_TRIAGC_BIT, uState);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetIFAGCOutput(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetRFAGCOutput                                                */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetRFAGCOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number   */
    tmbslFrontEndGpioConfig_t eState    /* I: RF AGC Output */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uState;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetRFAGCOutput(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if(eState == tmbslFrontEndGpioConfTriState)
        {
            uState = TDA10024_AGCCONF2_TRIAGC_BIT;
        }
        else
        {
            uState = 0;
        }

        err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_TRIAGC_BIT, uState);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetRFAGCOutput(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetSampling                                                   */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetSampling
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    tmbslTDA10024FECMode_t eMode,
    UInt8 bMFactor,
    UInt8 bNFactor,
    UInt8 bPFactor,
    UInt8 bFSampling
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetSampling(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if (eMode == tmbslTDA10024FECModeAnnexA)
        {
            pObj->sConfig.bDVB_Fsampling = bFSampling;
            pObj->sConfig.bDVB_PLL_M_Factor = bMFactor;
            pObj->sConfig.bDVB_PLL_N_Factor = bNFactor;
            pObj->sConfig.bDVB_PLL_P_Factor = bPFactor;
        }
        else
        {
            pObj->sConfig.bMCNS_Fsampling = bFSampling;
            pObj->sConfig.bMCNS_PLL_M_Factor = bMFactor;
            pObj->sConfig.bMCNS_PLL_N_Factor = bNFactor;
            pObj->sConfig.bMCNS_PLL_P_Factor = bPFactor;
        }

        if (eMode == pObj->sConfig.eModeDvbMcns)
        {
            err = TDA10024WriteInit(tUnit);
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetSampling(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetSampling                                                   */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetSampling
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    tmbslTDA10024FECMode_t eMode,
    UInt8 *pbMFactor,
    UInt8 *pbNFactor,
    UInt8 *pbPFactor,
    UInt8 *pbFSampling
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetSampling(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if (eMode == tmbslTDA10024FECModeAnnexA)
        {
            *pbFSampling = pObj->sConfig.bDVB_Fsampling;
            *pbMFactor = pObj->sConfig.bDVB_PLL_M_Factor;
            *pbNFactor = pObj->sConfig.bDVB_PLL_N_Factor;
            *pbPFactor = pObj->sConfig.bDVB_PLL_P_Factor;
        }
        else
        {
            *pbFSampling = pObj->sConfig.bMCNS_Fsampling;
            *pbMFactor = pObj->sConfig.bMCNS_PLL_M_Factor;
            *pbNFactor = pObj->sConfig.bMCNS_PLL_N_Factor;
            *pbPFactor = pObj->sConfig.bMCNS_PLL_P_Factor;
        }
		
        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetSampling(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetIF                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetIF
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    UInt32         uIF      /* I: If in Herz  */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               puByte[3]; 
    Int32               lDeltaF; 
    UInt8               bSampling;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetIF(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        pObj->sConfig.uTUN_IF = uIF;
        
        if (pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexA)
        {
            bSampling = pObj->sConfig.bDVB_Fsampling;
        }
        else
        {
            bSampling = pObj->sConfig.bMCNS_Fsampling;
        }

        /* set ALGOD and deltaF */
        if(bSampling)
        {
            /* FSAMPLING = 1 - high sampling clock */
            /* SACLK = Sysclk    (SACLK max = 72MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000);
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/500);
            lDeltaF /= (Int32)(pObj->uSysClk/1000);
            lDeltaF -= 53248; // 53248 = (2^20/32) * 13/8
        }
        else
        {
            /* FSAMPLING = 0 - low sampling clock */
            /* SACLK = Sysclk/2 (SACLK max = 36MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000);
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/1000);
            lDeltaF /= (Int32)(pObj->uSysClk/2000);
            lDeltaF -= 40960;    /* 40960 = (2^20/32) * 5/4; */
        }
        puByte[0] = (UInt8)lDeltaF;
        puByte[1] = (UInt8)(((lDeltaF>>8) & 0x7F) | TDA10024_DELTAF_MSB_ALGOD_BIT);
        err = TDA10024Write(tUnit, TDA10024_DELTAF_LSB_IND, 2, puByte);

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetIF(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetIF                                                         */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetIF
(
    tmUnitSelect_t tUnit,   /* I: Unit number */
    UInt32         *puIF    /* I: If in Herz  */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetIF(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        *puIF = pObj->sConfig.uTUN_IF;

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetIF(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetTSOutput                                                   */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetTSOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number */
    tmbslFrontEndGpioConfig_t eState    /* I: TS Output   */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uState;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetTSOutput(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if(eState == tmbslFrontEndGpioConfTriState)
        {
            uState = TDA10024_INTPS_TRIS_BIT|TDA10024_INTPS_TRIP_BIT;
        }
        else
        {
            uState = 0;
        }

        err = TDA10024WriteBit(tUnit, TDA10024_INTPS_IND, TDA10024_INTPS_TRIS_BIT|TDA10024_INTPS_TRIP_BIT, uState);
        
        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetTSOutput(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetTSOutput                                                   */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetTSOutput
(
    tmUnitSelect_t            tUnit,    /* I: Unit number */
    tmbslFrontEndGpioConfig_t *eState    /* I: TS Output   */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uState;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetTSOutput(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    err = TDA10024Read(tUnit, TDA10024_INTPS_IND, 1, &uState);

    if (err == TM_OK)
    {
        uState &= TDA10024_INTPS_TRIS_BIT|TDA10024_INTPS_TRIP_BIT;
       
        if(uState == 0)
        {
            *eState = tmbslFrontEndGpioConfOutputOpenDrain;
        }
        else
        {
            *eState = tmbslFrontEndGpioConfTriState;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetTSOutput(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetGPIOConfig                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024SetGPIOConfig
(
    tmUnitSelect_t              tUnit,
    tmbslTDA10024GPIOConfig_t   eGPIOConfig
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uValue;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetGPIOConfig(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        switch (eGPIOConfig)
        {
        case tmbslTDA10024GPIOConfigFEL:
            uValue = 0x0;
            err = TDA10024WriteBit(tUnit, TDA10024_TEST_IND, TDA10024_GPIOCFG_MSK, uValue);
            break;
        case tmbslTDA10024GPIOConfigIT:
            uValue = 0x1;
            err = TDA10024WriteBit(tUnit, TDA10024_TEST_IND, TDA10024_GPIOCFG_MSK, uValue);
            break;
        case tmbslTDA10024GPIOConfigCTRL:
            uValue = 0x2;
            err = TDA10024WriteBit(tUnit, TDA10024_TEST_IND, TDA10024_GPIOCFG_MSK, uValue);
            break;
        default:
            err = TDA10024_ERR_BAD_PARAMETER;
            break;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetGPIOConfig(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetGPIOConfig                                                 */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetGPIOConfig
(
    tmUnitSelect_t              tUnit,
    tmbslTDA10024GPIOConfig_t   *peGPIOConfig
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uValue;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetGPIOConfig(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    err = TDA10024Read(tUnit, TDA10024_TEST_IND, 1, &uValue);
    
    if (err == TM_OK)
    {
        uValue = uValue & TDA10024_GPIOCFG_MSK;

        switch (uValue)
        {
        case 0x0:
            *peGPIOConfig = tmbslTDA10024GPIOConfigFEL;
            break;
        case 0x1:
            *peGPIOConfig = tmbslTDA10024GPIOConfigIT;
            break;
        case 0x2:
            *peGPIOConfig = tmbslTDA10024GPIOConfigCTRL;
            break;
        default:
            *peGPIOConfig = tmbslTDA10024GPIOConfigUnknown;
            break;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetGPIOConfig(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024SetFECMode                                                    */
/*============================================================================*/
extern tmErrorCode_t
tmbslTDA10024SetFECMode
(
    tmUnitSelect_t         tUnit,
    tmbslTDA10024FECMode_t     eMode
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetFECMode(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        pObj->sConfig.eModeDvbMcns = eMode;
        err = TDA10024WriteInit(tUnit);

        (void)TDA10024MutexRelease(pObj);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetFECMode(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetFECMode                                                    */
/*============================================================================*/
tmErrorCode_t
tmbslTDA10024GetFECMode
(
    tmUnitSelect_t         tUnit,
    tmbslTDA10024FECMode_t *peMode
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetFECMode(0x%08X)", tUnit);
    
    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }
    
    if (err == TM_OK)
    {
        *peMode = pObj->sConfig.eModeDvbMcns;

        (void)TDA10024MutexRelease(pObj);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetFECMode(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024Gain                                                          */
/*============================================================================*/
tmErrorCode_t 
tmbslTDA10024Gain
(    
    tmUnitSelect_t tUnit,
    UInt8          bGain,
    UInt8          *pbTest
) 
{ 
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uByte, uByteSat[7];
    UInt32              uSATADC, uHALFADC, uSATDEC1, uSATDEC2, uSATDEC3, uSATAAF, uSATNYQ;
    Int8                bTEST_SAT, bTEST_HALFADC;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024Gain(0x%08X)", tUnit);

    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        /* Increase GNYQ gain */
        err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte);

        if (err == TM_OK)
        {
            uByte &= 0x8f;
            uByte |= (bGain<<4);
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND,1, &uByte);
        }
             
        if (err == TM_OK)
        {
            TDA10024Wait(tUnit, (UInt16)(1000*50*(pObj->uNbSamplesAGC)/(pObj->sCurrentChannel.uSR)));
            TDA10024Wait(tUnit, (UInt16)(1000*2*(pObj->uNbSamplesSSAT)/(pObj->sCurrentChannel.uSR)));
        
            /* Read saturation counters */
            err = TDA10024Read(tUnit, TDA10024_SATNYQ_IND, 7, uByteSat);
        }
              
        if (err == TM_OK)
        {
            uSATADC = uByteSat[1]; 
            uHALFADC = uByteSat[2]; 
            uSATDEC1 = uByteSat[3]; 
            uSATDEC2 = uByteSat[4]; 
            uSATDEC3 = uByteSat[5]; 
            uSATAAF = uByteSat[6]; 
            uSATNYQ = uByteSat[0]; 

            /* Time to refresh sat register = Nb of samples selected in SSAT register / Sr */
            TDA10024Wait(tUnit, (UInt16)(1000*2*(pObj->uNbSamplesSSAT)/(pObj->sCurrentChannel.uSR)));
            /* Read twice HALFADC to take higher value */
            err = TDA10024Read(tUnit, TDA10024_HALFADC_IND, 1, &uByte);
        }
             
        if (err == TM_OK)
        {
            uHALFADC = (uHALFADC>uByte) ? uHALFADC : uByte;

            bTEST_SAT = (Int8)((uSATADC > 1) || (uSATDEC1 > 1) || (uSATDEC2 > 1) || (uSATDEC3 > 1) || (uSATAAF > 1) || (uSATNYQ > 1));
            bTEST_HALFADC = (Int8)(uHALFADC == 0xFF);
            
            *pbTest = bTEST_SAT || bTEST_HALFADC;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024Gain(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024ResetUBK                                                      */
/*============================================================================*/
tmErrorCode_t 
tmbslTDA10024ResetUBK
(    
    tmUnitSelect_t tUnit
) 
{ 
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024ResetUBK(0x%08X)", tUnit);

    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
       if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
        {
            /* MCNS mode */
            err = TDA10024WriteBit(tUnit, TDA10024_RSCFG_IND, TDA10024_RSCFG_CLB_CPT_RSB_BIT, 0);
            if (err == TM_OK)
            {
                err = TDA10024WriteBit(tUnit, TDA10024_RSCFG_IND, TDA10024_RSCFG_CLB_CPT_RSB_BIT, TDA10024_RSCFG_CLB_CPT_RSB_BIT);
            }
        }
        else
        {
            /* DVB mode */
            err = TDA10024WriteBit(tUnit, TDA10024_FECDVBCFG1_IND, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT, 0);
            if (err == TM_OK)
            {
                err = TDA10024WriteBit(tUnit, TDA10024_FECDVBCFG1_IND, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT, TDA10024_FECDVBCFG1_CLB_CPT_TSP_BIT);
            }
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024ResetUBK(0x%08X) failed.", tUnit));

    return err;
}


/*============================================================================*/
/* tmbslTDA10024SetScanMode                                                   */
/*============================================================================*/
extern
tmErrorCode_t 
tmbslTDA10024SetScanMode
(    
    tmUnitSelect_t tUnit,
    tmbslTDA10024ScanMode_t eMode
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024SetScanMode(0x%08X)", tUnit);

    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if (eMode == tmbslTDA10024ScanModeNormal)
        {
            pObj->sApiFlags.iScan = 0;
            pObj->bNoCarrierIfAGCMax = 0;
        }
        else
        {
            pObj->sApiFlags.iScan = 1;
            pObj->bNoCarrierIfAGCMax = 1;
        }

        (void)TDA10024MutexRelease(pObj);
    }


    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024SetScanMode(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* tmbslTDA10024GetScanMode                                                   */
/*============================================================================*/
extern
tmErrorCode_t 
tmbslTDA10024GetScanMode
(    
    tmUnitSelect_t tUnit,
    tmbslTDA10024ScanMode_t *peMode
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "tmbslTDA10024GetScanMode(0x%08X)", tUnit);

    if(err == TM_OK)
    {
        err = TDA10024MutexAcquire(pObj, TDA10024_MUTEX_TIMEOUT);
    }

    if (err == TM_OK)
    {
        if (pObj->sApiFlags.iScan)
        {
            *peMode = tmbslTDA10024ScanModeScan;
        }
        else
        {
            *peMode = tmbslTDA10024ScanModeNormal;
        }

        (void)TDA10024MutexRelease(pObj);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10024GetScanMode(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/*                   STATIC FUNCTIONS DEFINTIONS                              */
/*============================================================================*/


/*============================================================================*/
/* TDA10024GetIQ                                                              */
/*============================================================================*/
tmErrorCode_t
TDA10024GetIQ
(
    tmUnitSelect_t tUnit,
    UInt32         *puIQ
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               puIQbuf[2];

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024GetIQ(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* Implementation */
        err = TDA10024Read(tUnit, TDA10024_CONSTI_IND, 2, puIQbuf);
    }
    
    if (err == TM_OK)
    {
        /* update return values */
        *puIQ = puIQbuf[0] << 8 | puIQbuf[1];
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024GetIQ(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024GetCOEF                                                            */
/*============================================================================*/
tmErrorCode_t
TDA10024GetCOEF
(
    tmUnitSelect_t tUnit,
    Int32          *plCoef
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               alICoef[52], alRCoef[52];
    UInt32              uCounter;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024GetCOEF(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* read the imaginary part of coef */
        err = TDA10024Read(tUnit, TDA10024_IEQCO_IND, 52, alICoef);
    }

    if (err == TM_OK)
    {
    /* read the real part of coef */
    err = TDA10024Read(tUnit, TDA10024_REQCO_IND, 52, alRCoef);
    } 

    if (err == TM_OK)
    {
        /* fill the array */
        for (uCounter = 0; uCounter < 52; uCounter += 2)
        {
            /* imaginary part */
            *plCoef = (alICoef[uCounter] << 3) | (alICoef[uCounter+1] >> 5);
            if (*plCoef & 0x0400)
                *plCoef |= 0xFFFFF800;
            plCoef++;
        }
        for (uCounter = 0; uCounter < 52; uCounter += 2)
        {
            /* real part */
            *plCoef = (alRCoef[uCounter] << 3) | (alRCoef[uCounter+1] >> 5);
            if (*plCoef & 0x0400)
                *plCoef |= 0xFFFFF800;
            plCoef++;
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024GetCOEF(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024WriteInit                                                            */
/*============================================================================*/
tmErrorCode_t
TDA10024WriteInit
(
    tmUnitSelect_t tUnit
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               puByte[3], uByte;
    Int32               lDeltaF;
    UInt32              uSACLK=0;
    UInt8               uPLL3 = 0;
    UInt8               bPLL_M_Factor = 0;
    UInt8               bPLL_N_Factor = 0;
    UInt8               bPLL_P_Factor = 0;
    UInt8               bFSampling = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024WriteInit(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        if (pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexA)
        {
            bFSampling = pObj->sConfig.bDVB_Fsampling;
            bPLL_M_Factor = pObj->sConfig.bDVB_PLL_M_Factor;
            bPLL_N_Factor = pObj->sConfig.bDVB_PLL_N_Factor;
            bPLL_P_Factor = pObj->sConfig.bDVB_PLL_P_Factor;
        }
        else
        {
            bFSampling = pObj->sConfig.bMCNS_Fsampling;
            bPLL_M_Factor = pObj->sConfig.bMCNS_PLL_M_Factor;
            bPLL_N_Factor = pObj->sConfig.bMCNS_PLL_N_Factor;
            bPLL_P_Factor = pObj->sConfig.bMCNS_PLL_P_Factor;
        }
        
        /* calculate the system frequency */
        pObj->uSysClk = pObj->sConfig.uXtalFreq * (bPLL_M_Factor+1);
        pObj->uSysClk /= (bPLL_N_Factor+1)*(bPLL_P_Factor+1);

        /* Calculate the sampling clock */
        if(bFSampling)
        {
            /* high sampling clock */
            uSACLK = pObj->uSysClk;
        }
        else
        {
            /* low sampling clock */
            uSACLK = pObj->uSysClk/2;
        }

        /* read the PLL M,N,P values */
        err = TDA10024Read(tUnit, TDA10024_PLL1_IND, 2, puByte);
    }
         
    if (err == TM_OK)
    {
        err = TDA10024Read(tUnit, TDA10024_GPR_IND, 1, &uByte);
    }
         
    /* change the PLL M,N,P values and FSAMPLING only if different */
    if((puByte[0] != bPLL_M_Factor) || 
         ((puByte[1] & TDA10024_PLL2_PDIV_MSK) != bPLL_N_Factor) ||
         (((puByte[1] & TDA10024_PLL2_NDIV_MSK)>>6) != bPLL_P_Factor) ||
         (((uByte & TDA10024_GPR_FSAMPLING_BIT) >> 5) != bFSampling) )
    {
        
        if (err == TM_OK)
        {
            /* disable the PLL */
            err = TDA10024Read(tUnit, TDA10024_PLL3_IND, 1, &uPLL3);
        }
        if (err == TM_OK)
        {
            uPLL3 |= (TDA10024_PLL3_BYPPLL_BIT | TDA10024_PLL3_PDPLL_BIT);
            err = TDA10024Write(tUnit, TDA10024_PLL3_IND, 1, &uPLL3);
        }
        if (err == TM_OK)
        {
            /*
            !!!! NOTE !!!! : When the PLL is disable, TDA10024 registers can't be read
            Therefore, TDA10024Read and TDA10024WriteBit functions must not be called
            */

            /* wait 100ms */
            TDA10024Wait(tUnit, 100);

            /* PLL factors */
            puByte[0]    = bPLL_M_Factor;
            puByte[1]    = bPLL_N_Factor;
            puByte[1] |= (UInt8)(bPLL_P_Factor << 6);

            /* write the PLL registers with PLL bypassed */
            err = TDA10024Write(tUnit, TDA10024_PLL1_IND, 2, puByte);
        }
            
        /* Set FSAMPLING */
        if(bFSampling)
        {
            /*
            err = TDA10024WriteBit(tUnit,
            TDA10024_GPR_IND, 
            TDA10024_GPR_FSAMPLING_BIT | TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT,
            TDA10024_GPR_FSAMPLING_BIT | TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT);
            */

            if (err == TM_OK)
            {
                uByte = TDA10024_GPR_FSAMPLING_BIT | TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT;
                err = TDA10024Write(tUnit, TDA10024_GPR_IND, 1, &uByte);
            }
        }
        else
        {
            /*
            err = TDA10024WriteBit(tUnit,
            TDA10024_GPR_IND, 
            TDA10024_GPR_FSAMPLING_BIT | TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT, 
            0                            | TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT);
            */
            if (err == TM_OK)
            {
                uByte = TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT;
                err = TDA10024Write(tUnit, TDA10024_GPR_IND, 1, &uByte);
            }
                
        }

        /* enable the PLL */
        /*
        err = TDA10024WriteBit(tUnit,
           TDA10024_PLL3_IND, TDA10024_PLL3_BYPPLL_BIT | TDA10024_PLL3_PDPLL_BIT, 0);
        */
        if (err == TM_OK)
        {
            uPLL3 &= ~(TDA10024_PLL3_BYPPLL_BIT | TDA10024_PLL3_PDPLL_BIT);
            err = TDA10024Write(tUnit, TDA10024_PLL3_IND, 1, &uPLL3);
        }
            
        if (err == TM_OK)
        {
            /* wait 100ms */
            TDA10024Wait(tUnit, 100);
        }

    }

    /* Set DVB/MCNS mode */
    if (pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
    {
        if (err ==TM_OK)
        {
            /* MCNS mode */
            err = TDA10024WriteBit(tUnit,
                TDA10024_RESET_IND,
                TDA10024_RESET_DVBMCNS_BIT, 
                TDA10024_RESET_DVBMCNS_BIT);
        }
            
        if (err == TM_OK)
        {
            /* set the BER depth */
            err = TDA10024WriteBit(tUnit,
                TDA10024_RSCFG_IND, TDA10024_RSCFG_PRG_TBER_MSK, pObj->sConfig.bBERdepth << 2);
        }
            
        if (err == TM_OK)
        {
            /* Configure the IT to know if there are Uncor */
            err = TDA10024WriteBit(tUnit, TDA10024_ITSEL1_IND, 0x04, 0x04);
        }
        

        if (err == TM_OK)
        {
            /* Configure the IT to know if there are Uncor */
            err = TDA10024WriteBit(tUnit, TDA10024_ITSEL2_IND, 0x04, 0x04);
        }
    }
    else
    {
        if (err == TM_OK)
        {
            /* DVB mode */
            err = TDA10024WriteBit(tUnit,
                TDA10024_RESET_IND, 
                TDA10024_RESET_DVBMCNS_BIT, 
                0);
        }
            
        if (err == TM_OK)
        {
            /* set the BER depth */
            err = TDA10024WriteBit(tUnit,
                TDA10024_FECDVBCFG1_IND,
                TDA10024_FECDVBCFG1_PVBER_MSK,
                pObj->sConfig.bBERdepth << 6);
        }
    }

    /* set GAIN1 bit to 0 */
    if ( pObj->sConfig.eBandType != tmbslTDA10024BandTypeSecamL ) 
    {
        if (err == TM_OK)
        {
            uByte = pObj->sConfig.bGAIN1;
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
        }
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
        }
        if (err == TM_OK)
        {
            uByte &= 0x7C;
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
        }
    }

    if (err == TM_OK)
    {
        /* program the SWEEP Ramp mode */
        err = TDA10024WriteBit(tUnit, TDA10024_CARCONF_IND, TDA10024_SWEEP_MODE_MSK, TDA10024_SWEEP_MODE_MSK);
    }
        
    if (err == TM_OK)
    {
        /* Nb of samples in STAT depends on reg SSAT */
        err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte);
    }
    
    if (err == TM_OK)
    {
        switch ((uByte & 0x03) )    
        {
            case 0:
            pObj->uNbSamplesSSAT = 16384;
            break;
            case 1:
            pObj->uNbSamplesSSAT = 8192;
            break;
            case 2:
            pObj->uNbSamplesSSAT = 4096;
            break;
            case 3:
            pObj->uNbSamplesSSAT = 2048; 
            break;                
        }
        /* set the acquisition to +/-480ppm */
        err = TDA10024WriteBit(tUnit,
            TDA10024_CLKCONF_IND, TDA10024_CLKCONF_DYN_BIT, TDA10024_CLKCONF_DYN_BIT);
    }
        
    if (pObj->sConfig.bClkOffsetRange)
    {
        if (err == TM_OK)
        {
            /* set CLK_C to the minimum */
            err = TDA10024WriteBit(tUnit, TDA10024_CLKCONF_IND, TDA10024_CLK_C_MASK, 0x00);
        }
            
    }
    else
    {
        if (err == TM_OK)
        {
            /* set CLK_C to the default value (2) */
            err = TDA10024WriteBit(tUnit, TDA10024_CLKCONF_IND, TDA10024_CLK_C_MASK, 0x02);
        }
            
    }

    /* TRIAGC, POSAGC, enable AGCIF */
    if ( pObj->sConfig.eBandType != tmbslTDA10024BandTypeSecamL ) 
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit,
                TDA10024_AGCCONF2_IND,
                TDA10024_AGCCONF2_TRIAGC_BIT | TDA10024_AGCCONF2_POSAGC_BIT | TDA10024_AGCCONF2_ENAGCIF_BIT,
                TDA10024_AGCCONF2_POSAGC_BIT | TDA10024_AGCCONF2_ENAGCIF_BIT);
        }
            
        if (err == TM_OK)
        {
            /* set the AGCREF */
            uByte = TDA10024_AGCREF_DEF; /*optimized for adjacent signal*/
            err = TDA10024Write(tUnit, TDA10024_AGCREF_IND, 1, &uByte);
        }
            
    } 
    else 
    { 
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, 
                TDA10024_AGCCONF2_IND, 
                TDA10024_AGCCONF2_TRIAGC_BIT | TDA10024_AGCCONF2_POSAGC_BIT | TDA10024_AGCCONF2_ENAGCIF_BIT, 
                TDA10024_AGCCONF2_ENAGCIF_BIT);
        }
         
        if (err == TM_OK)
        {
            /* set the AGCREF */
            uByte = TDA10024_AGCREF_SECAM_DEF; /*optimized for adjacent signal*/
            err = TDA10024Write(tUnit, TDA10024_AGCREF_IND, 1, &uByte);
        }
        
    }
    
    if (err == TM_OK)
    {
        /* set SACLK_OFF */
        /* Remove SACLK sice it is not used and generate interferences */
        err = TDA10024WriteBit(tUnit,
            TDA10024_CONTROL_IND,
            TDA10024_CONTROL_OLDBYTECLK_BIT ,
            TDA10024_CONTROL_OLDBYTECLK_BIT );
    }
         

    /* program CS depending on SACLK and set GAINADC */
    /*
    if (uSACLK < 25000000)
    {
        uByte = (0<<6) | TDA10024_ADC_GAINADC_BIT | TDA10024_ADC_TWOS_BIT;
    }
    else if (uSACLK < 35000000)
    {
        uByte = (1<<6) | TDA10024_ADC_GAINADC_BIT | TDA10024_ADC_TWOS_BIT;;
    }
    else if (uSACLK < 50000000)
    {
        uByte = (2<<6) | TDA10024_ADC_TWOS_BIT;;
    }
    else
    {
        uByte = (3<<6) | TDA10024_ADC_TWOS_BIT;;
    }
    */ 

    if (err == TM_OK)
    {
        if (uSACLK < 35000000)
        {
            uByte = 0x08;
        }
        else if (uSACLK < 60000000)
        {
            uByte = 0x48;
        }
        else if (uSACLK<80000000)
        {
            uByte = 0x88;
        }
        else
        {
            uByte = 0xC8;
        }

        err = TDA10024Write(tUnit, TDA10024_ADC_IND, 1, &uByte);
    }
        
    /* set the polarity of the PWM for the AGC */
    if (pObj->sConfig.bPolaPWM1)
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_PPWMTUN_BIT, TDA10024_AGCCONF2_PPWMTUN_BIT);
        }
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_PPWMTUN_BIT, 0);
        }
    }

    if (pObj->sConfig.bPolaPWM2)
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_PPWMIF_BIT, TDA10024_AGCCONF2_PPWMIF_BIT);
        }
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_AGCCONF2_IND, TDA10024_AGCCONF2_PPWMIF_BIT, 0);
        }
    }
    
    if (err == TM_OK)
    {
        /* set the threshold for the IF AGC */
        puByte[0] = pObj->sConfig.bIFMax;
        puByte[1] = pObj->sConfig.bIFMin;
        err = TDA10024Write(tUnit, TDA10024_IFMAX_IND, 2, puByte);
    }
        
    if (err == TM_OK)
    {
        /* set the threshold for the TUN AGC */
        puByte[0] = pObj->sConfig.bTUNMax;
        puByte[1] = pObj->sConfig.bTUNMin;
        err = TDA10024Write(tUnit, TDA10024_TUNMAX_IND, 2, puByte);
    }
        

    if (err == TM_OK)
    {
        /* configure the equalizer */
        if(pObj->sConfig.bEqualType == 0)
        {
            /* disable the equalizer */
            uByte = pObj->sConfig.bEqConf1;
        }
        else
        {
            /* enable the equalizer and set the DFE bit */
            uByte = pObj->sConfig.bEqConf1 | 
                    TDA10024_EQCONF1_ENADAPT_BIT | 
                    TDA10024_EQCONF1_ENEQUAL_BIT | 
                    (pObj->sConfig.bEqualType-1);
        }
        err = TDA10024Write(tUnit, TDA10024_EQCONF1_IND, 1, &uByte);
    }
        
     
    if (err == TM_OK)
    {
        err = TDA10024WriteBit(tUnit,
        TDA10024_EQCONF2_IND,
        TDA10024_EQCONF2_SGNALGO_BIT | TDA10024_EQCONF2_STEPALGO_BIT,
        TDA10024_EQCONF2_SGNALGO_BIT | TDA10024_EQCONF2_STEPALGO_BIT);
    }
        

    if (err == TM_OK)
    {
        /* changejb force the CTPHASE to 1 bit 6 reg 0x1c */
        err = TDA10024WriteBit(tUnit, TDA10024_EQCONF2_IND, 0x40, 0x40);
    }
        
    if (err == TM_OK)
    {
        /* set ALGOD and deltaF */
        if(bFSampling)
        {
            /* FSAMPLING = 1 - high sampling clock */
            /* SACLK = Sysclk    (SACLK max = 72MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000);
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/500);
            lDeltaF /= (Int32)(pObj->uSysClk/1000);
            lDeltaF -= 53248; // 53248 = (2^20/32) * 13/8
        }
        else
        {
            /* FSAMPLING = 0 - low sampling clock */
            /* SACLK = Sysclk/2 (SACLK max = 36MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000);
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/1000);
            lDeltaF /= (Int32)(pObj->uSysClk/2000);
            lDeltaF -= 40960;    /* 40960 = (2^20/32) * 5/4; */
        }
        puByte[0] = (UInt8)lDeltaF;
        puByte[1] = (UInt8)(((lDeltaF>>8) & 0x7F) | TDA10024_DELTAF_MSB_ALGOD_BIT);
        err = TDA10024Write(tUnit, TDA10024_DELTAF_LSB_IND, 2, puByte);
    }
        
    if (err == TM_OK)
    {
        /* set the KAGCIF and KAGCTUN to acquisition mode (low time constant) */
        uByte = 0x92;
        err = TDA10024Write(tUnit, TDA10024_AGCCONF1_IND, 1, &uByte);
    }
        
    if (err == TM_OK)
    {
        /* Nb of samples used to refresh AGC values */
        err = TDA10024Read(tUnit, TDA10024_INVQ_AGC_IND, 1, &uByte);
    }
        
    if (err == TM_OK)
    {
        switch ((uByte & TDA10024_AGC_INTEG_NB_MSK))
        {
            case 0:
            pObj->uNbSamplesAGC = 512;
            break;
            case 1:
            pObj->uNbSamplesAGC = 1024;
            break;
            case 2:
            pObj->uNbSamplesAGC = 2048;
            break;
            case 3:
            pObj->uNbSamplesAGC = 4096;
            break;
            case 4:
            pObj->uNbSamplesAGC = 8192;
            break;
        }

        /* set carrier algorithm parameters */
        //uByte    = TDA10024_SWEEP_DEF;
        uByte = 0;
        uByte |= pObj->sConfig.bSwPos << 7;
        uByte |= pObj->sConfig.bSwDyn << 4;
        uByte |= pObj->sConfig.bSwStep << 2;
        uByte |= pObj->sConfig.bSwLength;
        err = TDA10024WriteBit(tUnit, TDA10024_SWEEP_IND, 0x7F, uByte);
    }
        

    /* set the MPEG output  polarity */
    /*if(pObj->sConfig.bOUT_OClk1)
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_INTP1_IND, TDA10024_INTP1_POCLKP_BIT, TDA10024_INTP1_POCLKP_BIT);
        }
            
    }     
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_INTP1_IND, TDA10024_INTP1_POCLKP_BIT, 0);
        }
            
    }*/
    if(err == TM_OK)
    {
        uByte = (pObj->sConfig.bOUT_OClk1 | (pObj->sConfig.bOUT_DenPol1 << 1) | (pObj->sConfig.bOUT_UncorPol1 << 2) | (pObj->sConfig.bOUT_SyncPol1 << 3));
        err = TDA10024WriteBit(tUnit, TDA10024_INTP1_IND, 0x0f, uByte);
    }


    /*if(pObj->sConfig.bOUT_OClk2)
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_INTS1_IND, TDA10024_INTS1_POCLKS_BIT, TDA10024_INTS1_POCLKS_BIT);
        }
            
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_INTS1_IND, TDA10024_INTS1_POCLKS_BIT, 0);
        }
            
    }*/
    if(err == TM_OK)
    {
        uByte = (pObj->sConfig.bOUT_OClk2 | (pObj->sConfig.bOUT_DenPol2 << 1) | (pObj->sConfig.bOUT_UncorPol2 << 2) | (pObj->sConfig.bOUT_SyncPol2 << 3));
        err = TDA10024WriteBit(tUnit, TDA10024_INTS1_IND, 0x0f, uByte);
    }

    if (err == TM_OK)
    {
        /* TS interface 1 */
        uByte = 0;
        if(pObj->sConfig.bOUT_bMSBFirst1)
        {
            uByte |= TDA10024_INTP2_MSBFIRSTP_BIT;
        }

        if(pObj->sConfig.bOUT_bParaSer1)
        {
            /* SERIAL */
            uByte |= TDA10024_INTP2_INTSEL_SER_DEF;
        }
        else
        {
            /* PARALLEL */
            /* set to 1 MSB if parallel */
            uByte |= TDA10024_INTP2_MSBFIRSTP_BIT;

            if(pObj->sConfig.bOUT_ModeABC1 == 0)
            {
                /* PARALLEL mode A */
                uByte |= TDA10024_INTP2_INTSEL_PARA_DEF;
            }
            else if(pObj->sConfig.bOUT_ModeABC1 == 1)
            {
                /* PARALLEL mode B */
                uByte |= TDA10024_INTP2_INTSEL_PARB_DEF;
                uByte |= pObj->sConfig.bOUT_ParaDiv1 << 4;
            }
            else /* if(pObj->sConfig.bOUT_ModeABC1 == 2) */
            {
                /* PARALLEL mode C */
                uByte |= TDA10024_INTP2_INTSEL_PARC_DEF;
            }
        }

        err = TDA10024Write(tUnit, TDA10024_INTP2_IND, 1, &uByte);
    }
        
    /* TS interface 2 */
    if(pObj->sConfig.bOUT_bMSBFirst2)
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit,
                TDA10024_INTPS_IND, TDA10024_INTPS_MSBFIRSTS_BIT, TDA10024_INTPS_MSBFIRSTS_BIT);
        }
            
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit,
                TDA10024_INTPS_IND, TDA10024_INTPS_MSBFIRSTS_BIT, 0);
        }
            
    }

    if (err == TM_OK)
    {
        /* sync byte width */
        uByte = (pObj->sConfig.bOUT_SyncWidth1 << 5) | (pObj->sConfig.bOUT_SyncWidth2 << 4);
        err = TDA10024WriteBit(tUnit,
                               TDA10024_INTPS_IND, 0x30, uByte);
    }

    if (err == TM_OK)
    {
        /* disable the tri state of the outputs */
        err = TDA10024WriteBit(tUnit,
            TDA10024_INTPS_IND, TDA10024_INTPS_TRIP_BIT | TDA10024_INTPS_TRIS_BIT, 0);
    }
            
    if (err == TM_OK)
    {
        err = TDA10024WriteBit(tUnit, TDA10024_EQSTEP_IND, TDA10024_EQSTTRAC_MSK, 0x28);
    }
            
    if (err == TM_OK)
        {
            err = TDA10024WriteBit(tUnit, TDA10024_EQSTEP_IND, TDA10024_EQSTACQ_MSK, 0x01);
    }
            
    if (err == TM_OK)
    {
        uByte = 0x60;
        err = TDA10024Write(tUnit, TDA10024_ERAGC_THD_IND, 1, &uByte);
    }

    if (err == TM_OK)
    {
        if (pObj->sConfig.bEnableI2cSwitch)
        {
            err = TDA10024WriteBit(tUnit,
                TDA10024_TEST_IND, TDA10024_TEST_BYPIIC_BIT|TDA10024_TEST_SCLT0_BIT, 
                TDA10024_TEST_SCLT0_BIT);
        }
    }

    if (err == TM_OK)
    {
        err = tmbslTDA10024ManageBandType(tUnit);
    }

    if (err == TM_OK)
    {
        /* Soft reset */
        err = TDA10024ResetAll(tUnit);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024WriteInit(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024ResetAll                                                           */
/*============================================================================*/
tmErrorCode_t
TDA10024ResetAll
(
    tmUnitSelect_t tUnit
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024ResetAll(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* reset the chip */
        err = TDA10024WriteBit(tUnit,
            TDA10024_GPR_IND,
            TDA10024_GPR_CLBS2_BIT | TDA10024_GPR_CLBS_BIT,
            TDA10024_GPR_CLBS2_BIT | 0);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024ResetAll(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024WriteQAM                                                           */
/*============================================================================*/
static tmErrorCode_t
TDA10024WriteQAM
(
    tmUnitSelect_t tUnit,
    UInt32         uMod
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    static tm10024Qam_t sTypeQAM[] =
    {
        /* QAM  LOCKTHR MSETH   AREF    AGCREFNYQ   ERAGCNYQ_THD */
        { 0x00, 0x87,   0xa2,   0x91,   0x8c,       0x57}, /* 16 QAM <=> qam=0 */
        { 0x04, 0x64,   0x74,   0x96,   0x8c,       0x57}, /* 32 QAM <=> qam=1 */
        { 0x08, 0x46,   0x43,   0x6a,   0x6a,       0x56}, /* 64 QAM <=> qam=2 */
        { 0x0c, 0x36,   0x34,   0x7e,   0x78,       0x52}, /* 128 QAM <=> qam=3 */
        { 0x10, 0x26,   0x23,   0x6c,   0x5c,       0x4a}, /* 256 QAM <=> qam=4 */
        { 0x14, 0x78,   0x8c,   0x96,   0x78,       0x4c}  /* 4 QAM <=> qam=5 */
    };


    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024WriteQAM(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* LOCKTHR */
        err = TDA10024Write(tUnit, TDA10024_LOCKTHR_IND, 1, (UInt8*)&sTypeQAM[uMod].uLockthr);
    }
           
    if (err == TM_OK)
    {
        /* MSETH */
        err = TDA10024Write(tUnit, TDA10024_MSETH_IND, 1, (UInt8*)&sTypeQAM[uMod].uMseth);
    }
            
    if (err == TM_OK)
    {
        /* AREF */
        err = TDA10024Write(tUnit, TDA10024_AREF_IND, 1, (UInt8*)&sTypeQAM[uMod].uAref);
    }
        
    if (err == TM_OK)
    {
        /* AGCREFNYQ */
        err = TDA10024Write(tUnit, TDA10024_AGCREFNYQ_IND, 1, (UInt8*)&sTypeQAM[uMod].uAgcRefNyq);
    }
        
    if (err == TM_OK)
    {
        /* ERAGCNYQ_THD */
        err = TDA10024Write(tUnit, TDA10024_ERAGCNYQ_THD_IND, 1, (UInt8*)&sTypeQAM[uMod].uErAgcNyqThd);
    }
        
    if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
    {
        /* MCNS mode */

        if (err == TM_OK)
        {
        /* GPR: program the modulation in QAM bits */
            err = TDA10024WriteBit(tUnit, TDA10024_GPR_IND, TDA10024_GPR_QAM_MSK, (UInt8)sTypeQAM[uMod].uQam);
        }
            
        /* CTRL1: program the modulation in QAMMODE bit */
        if(uMod == 2)
        {
            if (err == TM_OK)
            {
                /* 64 QAM */
                err = TDA10024WriteBit(tUnit,
                    TDA10024_CTRL1_IND, TDA10024_CTRL1_QAMMODE_BIT, TDA10024_CTRL1_QAMMODE_BIT);
            }
            
            if (err == TM_OK)
            {
            /* write roll off */
                    err = TDA10024WriteBit(tUnit, TDA10024_GAIN2_IND, TDA10024_ROLL_OFF_MSK, 0);
            }
            
        }
        else if(uMod == 4)
        {
            if (err == TM_OK)
            {
            /* 256 QAM */
                err = TDA10024WriteBit(tUnit, TDA10024_CTRL1_IND, TDA10024_CTRL1_QAMMODE_BIT, 0);
            }
                
            if (err == TM_OK)
            {
                /* write roll off */
                err = TDA10024WriteBit(tUnit,
                    TDA10024_GAIN2_IND, TDA10024_ROLL_OFF_MSK, 1);
            }
        }
    }
    else
    {
        /* DVB mode */

        if (err == TM_OK)
        {
        /* GPR: program the modulation in QAM bits */
            err = TDA10024WriteBit(tUnit,
                TDA10024_GPR_IND, TDA10024_GPR_QAM_MSK, (UInt8)sTypeQAM[uMod].uQam);
        }
            
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024WriteQAM(0x%08X) failed.", tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024WriteSR                                                            */
/*============================================================================*/
static tmErrorCode_t
TDA10024WriteSR
(
    tmUnitSelect_t tUnit,
    UInt32         *puSR
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               puWrite[4];
    UInt32              uBDR, uBDRI, uSR;
    UInt8               uNDec, uSFil;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024WriteSR(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        uSR = *puSR;

        /* calculate the number of decimation and the antialias filter */
        if (uSR/10 < pObj->uSysClk/984)
        {
            uNDec = 3;
            uSFil = 1;
        }
        else if (uSR/10 < pObj->uSysClk/640)
        {
            uNDec = 3;
            uSFil = 0;
        }
        else if (uSR/10 < pObj->uSysClk/492)
        {
            uNDec = 2;
            uSFil = 1;
        }
        else if (uSR/10 < pObj->uSysClk/320)
        {
            uNDec = 2;
            uSFil = 0;
        }
        else if (uSR/10 < pObj->uSysClk/246)
        {
            uNDec = 1;
            uSFil = 1;
        }
        else if (uSR/10 < pObj->uSysClk/160)
        {
            uNDec = 1;
            uSFil = 0;
        }
        else if (uSR/10 < pObj->uSysClk/123)
        {
            uNDec = 0;
            uSFil = 1;
        }
        else
        {
            uNDec = 0;
            uSFil = 0;
        }

        /* program SFIL */
        err = TDA10024WriteBit(tUnit, TDA10024_GAIN2_IND, TDA10024_GAIN2_SFIL_MSK, uSFil << 7);
    }
        
    if (err == TM_OK)
    {
        /* program NDEC */
        err = TDA10024WriteBit(tUnit, TDA10024_CLKCONF_IND, TDA10024_CLKCONF_NDEC_MSK, uNDec << 6);
    }
        

    /* program the symbol frequency registers */
    
    if (err == TM_OK)
    {
        /* calculate the inversion of the symbol frequency */
        uBDRI     = pObj->uSysClk*16;
        uBDRI >>= uNDec;
        uBDRI    += *puSR/2;
        uBDRI    /= *puSR;
        if (uBDRI > 255)
        {
            uBDRI = 255;
        }

        /* calculate the symbol rate */
        uBDR = TDA10024MulDiv32(1<<(24+uNDec), *puSR, pObj->uSysClk);

        /* program the value in register of the symbol rate */
        puWrite[0] = (UInt8)(uBDR);
        puWrite[1] = (UInt8)(uBDR >> 8);
        puWrite[2] = (UInt8)(uBDR >> 16);
        puWrite[3] = (UInt8)uBDRI;
        err = TDA10024Write(tUnit, TDA10024_BDRLSB_IND, 4, puWrite);
    }
        
    if (err == TM_OK)
    {
        /* return the value programmed */
        *puSR = TDA10024MulDiv32(uBDR, pObj->uSysClk, 1<<(24+uNDec)); 
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024WriteSR(0x%08X) failed.", tUnit));

    return err;
}

#if 0
/*============================================================================*/
/* TDA10024ResetDeltaF                                                        */
/*============================================================================*/
static tmErrorCode_t 
TDA10024ResetDeltaF( 
    tmUnitSelect_t tUnit
    ) 
{ 
    UInt8 puByte[3]; 
    Int32 lDeltaF; 
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024ResetDeltaF(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* set ALGOD and deltaF  */
        if(pObj->sConfig.bFsampling) 
        { 
            /* FSAMPLING = 1 - high sampling clock */
            /* SACLK = Sysclk    (SACLK max = 72MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000); 
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/500); 
            lDeltaF /= (Int32)(pObj->uSysClk/1000); 
            lDeltaF -= 53248;                                     /* 53248 = (2^20/32) * 13/8 */
        } 
        else 
        { 
            /* FSAMPLING = 0 - low sampling clock */
            /* SACLK = Sysclk/2 (SACLK max = 36MHz) */
            lDeltaF    = (Int32)(pObj->sConfig.uTUN_IF/1000); 
            lDeltaF *= 32768;                                     /* 32768 = 2^20/32 */
            lDeltaF += (Int32)(pObj->uSysClk/1000); 
            lDeltaF /= (Int32)(pObj->uSysClk/2000); 
            lDeltaF -= 40960;                                     /* 53248 = (2^20/32) * 5/4 */
        } 
     
        puByte[0] = (UInt8)lDeltaF; 
        puByte[1] = (UInt8)(((lDeltaF>>8) & 0x7F) | TDA10024_DELTAF_MSB_ALGOD_BIT); 
        err = TDA10024Write(tUnit, TDA10024_DELTAF_LSB_IND, 2, puByte); 
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024ResetDeltaF(0x%08X) failed.", tUnit));

    return err; 
} 
#endif 

/*============================================================================*/
/* TDA10024GetLockStatus                                                      */
/*============================================================================*/
static tmErrorCode_t
TDA10024GetLockStatus
(
    tmUnitSelect_t       tUnit,
    tmbslTDA10024State_t *peLockStatus
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uSynchro = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024GetLockStatus(0x%08X)", tUnit);
    
    if (pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
    {
        if (err == TM_OK)
        {
            err = TDA10024Read(tUnit, TDA10024_STATUS_MCNS_IND, 1, &uSynchro); 
        }
        if (err == TM_OK)
        {
            if (uSynchro & TDA10024_STATUS_MCNS_MPEGLCK_BIT)
            {
                *peLockStatus = tmbslTDA10024StateLocked;
            }
            else if (uSynchro & TDA10024_STATUS_MCNS_FRAMELCK_BIT)
            { 
                *peLockStatus = tmbslTDA10024StateSynchronised; 
            }
            else
            {
                *peLockStatus = tmbslTDA10024StateNotSynchronised;
            }
        }
    }
    else
    {
        if (err == TM_OK)
        {
            err = TDA10024Read(tUnit, TDA10024_STATUS_IND, 1, &uSynchro); 
        }
        if (err == TM_OK)
        {
            if (uSynchro & TDA10024_STATUS_FEL_BIT)
            {
                *peLockStatus = tmbslTDA10024StateLocked;
            }
            else if (uSynchro & TDA10024_STATUS_FSYNC_BIT)
            { 
                *peLockStatus = tmbslTDA10024StateSynchronised; 
            }
            else
            {
                *peLockStatus = tmbslTDA10024StateNotSynchronised;
            }
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024GetLockStatus(0x%08X) failed.", tUnit));

    return err;
}

/*============================================================================*/
/*                   TIMER FUNCTIONS                                          */
/*============================================================================*/

/*============================================================================*/
/* TDA10024_CalcTimer                                                         */
/*============================================================================*/
static UInt16
TDA10024_CalcTimer
(
    tmUnitSelect_t tUnit,       /* I: Demod unit number             */
    UInt32         uNbSymbol    /* I: Number of symbols to wait for */
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32              uTime = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_CalcTimer(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* prevent division by 0 */
        if (0 == pObj->sCurrentChannel.uSR) 
        {
            uTime = 0;
        }
        else
        {
            /* calculate the timer in milliseconds */
            uTime = uNbSymbol;
            uTime *= 1000;
            uTime += pObj->sCurrentChannel.uSR/2;
            uTime /= pObj->sCurrentChannel.uSR;
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_CalcTimer(0x%08X) failed.", tUnit));

    return (UInt16)uTime;
}

/*============================================================================*/
/* TDA10024_CalcSymbols                                                       */
/*============================================================================*/
static UInt32
TDA10024_CalcSymbols
(
    tmUnitSelect_t tUnit,
    UInt32         uNbFrames
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32              uNbBits;
    UInt32              uNbSymbols = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_CalcSymbols(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        switch (pObj->sCurrentChannel.eMOD)
        {
        case tmFrontEndModulationQam16:
            uNbBits = 4;
            break;
        case tmFrontEndModulationQam32:
            uNbBits = 5;
            break;
        case tmFrontEndModulationQam64:
            uNbBits = 6;
            break;
        case tmFrontEndModulationQam128:
            uNbBits = 7;
            break;
        case tmFrontEndModulationQam256:
            uNbBits = 8;
            break;
        default:
            uNbBits = 2;
        }
    
        uNbSymbols = (UInt32)(uNbFrames*204*8/uNbBits+1) ; /* +1 for round to the superior integer */
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_CalcSymbols(0x%08X) failed.", tUnit));

    return uNbSymbols;
}

/*============================================================================*/
/*                   I2C FUNCTIONS                                            */
/*============================================================================*/

/*============================================================================*/
/* TDA10024Write                                                              */
/*============================================================================*/
tmErrorCode_t
TDA10024Write
(
    tmUnitSelect_t tUnit,
    UInt8          uIndex_U,
    UInt32         uNBytes_U,
    UInt8*         puData_U
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_BLAB, "TDA10024Write(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = POBJ_SRVFUNC_SIO.Write (tUnit, 1, &uIndex_U, uNBytes_U, puData_U);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024Write(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024WriteBit                                                           */
/*============================================================================*/
tmErrorCode_t
TDA10024WriteBit
(
    tmUnitSelect_t tUnit,
    UInt8          uIndex_U,
    UInt32         uMask_U,
    UInt8          uData_U
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               byte;

    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_BLAB, "TDA10024WriteBit(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = TDA10024Read(tUnit, uIndex_U, 1, &byte);
    }
    
    if (err == TM_OK)
    {
        byte = (~(UInt8)uMask_U & byte) | ((UInt8)uMask_U & (UInt8)uData_U);
        err = TDA10024Write(tUnit, uIndex_U, 1, &byte);
    }
    
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024WriteBit(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024Read                                                               */
/*============================================================================*/
tmErrorCode_t
TDA10024Read
(
    tmUnitSelect_t tUnit,
    UInt8          uIndex_U,
    UInt32         uNBytes_U,
    UInt8*         puData_U
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_BLAB, "TDA10024Read(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = POBJ_SRVFUNC_SIO.Read (tUnit, 1, &uIndex_U, uNBytes_U, puData_U);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024Read(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024Wait                                                               */
/*============================================================================*/
tmErrorCode_t
TDA10024Wait
(
    tmUnitSelect_t tUnit,
    UInt32         uTime
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024Wait(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        err = POBJ_SRVFUNC_STIME.Wait (tUnit, uTime);
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024Wait(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024MutexAcquire                                                       */
/*============================================================================*/
extern tmErrorCode_t
TDA10024MutexAcquire
(
    ptmTDA10024Object_t pObj,
    UInt32              timeOut
)
{
    tmErrorCode_t   err = TM_OK;

    if(pObj->sMutex.Acquire != Null && pObj->pMutex != Null)
    {
        err = pObj->sMutex.Acquire(pObj->pMutex, timeOut);
    }
    
    return err;
}

/*============================================================================*/
/* TDA10024MutexRelease                                                       */
/*============================================================================*/
extern tmErrorCode_t
TDA10024MutexRelease
(
    ptmTDA10024Object_t pObj
)
{
    tmErrorCode_t   err = TM_OK;

    if(pObj->sMutex.Release != Null && pObj->pMutex != Null)
    {
        err = pObj->sMutex.Release(pObj->pMutex);
    }
    
    return err;
}

/*============================================================================*/
/*                   ALGORITHM FUNCTIONS                                      */
/*============================================================================*/


/*============================================================================*/
/* TDA10024_AlgoMod                                                           */
/*============================================================================*/
static tmbslFrontEndState_t
TDA10024_AlgoMod
(
    tmUnitSelect_t tUnit
)
{

    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32               uMod = 0;
    tmbslFrontEndState_t eState = tmbslFrontEndStateUnknown;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoMod(0x%08X)", tUnit);

    if (err == TM_OK)
    {
        /* program new modulation */
        if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
        {
            /* MCNS mode */
            switch (pObj->sCurrentChannel.eMOD)
            {
            case tmFrontEndModulationQam64:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam256;
                uMod = 4;
                break;

            case tmFrontEndModulationQam256:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam64;
                uMod = 2;
                break;

            default:
                eState = tmbslFrontEndStateNotLocked;
                err = TDA10024_ERR_NOT_INITIALIZED;
                break;
            }
        }
        else if (!pObj->sApiFlags.iScan)
        {
            /* DVB mode */
            switch (pObj->sCurrentChannel.eMOD)
            {
            case tmFrontEndModulationQam64:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam256;
                uMod = 4;
                break;

            case tmFrontEndModulationQam256:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam16;
                uMod = 0;
                break;

            case tmFrontEndModulationQam16:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam32;
                uMod = 1;
                break;

            case tmFrontEndModulationQam32:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam128;
                uMod = 3;
                break;

            case tmFrontEndModulationQam128:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam64;
                uMod = 2;
                break;

            default:
                eState = tmbslFrontEndStateNotLocked;
                err = TDA10024_ERR_NOT_INITIALIZED;
                break;
            }
        }
        else if (pObj->sApiFlags.iScan) //no need to find 4,16,32 QAM modulation for the moment
        {
            /* DVB mode */
            switch (pObj->sCurrentChannel.eMOD)
            {
            case tmFrontEndModulationQam64:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam256;
                uMod = 4;
                break;

            case tmFrontEndModulationQam256:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam128;
                uMod = 3;
                break;

            case tmFrontEndModulationQam128:
                pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam64;
                uMod = 2;
                break;

            default:
                eState = tmbslFrontEndStateNotLocked;
                err = TDA10024_ERR_NOT_INITIALIZED;
                break;
            }
        }

        if(err == TM_OK)
        {
            /* write new QAM */
            err = TDA10024WriteQAM(tUnit, uMod);
        }
    }

    if (err == TM_OK)
    {
        /* if 64qam the algorithm is completed */
        if (2 == uMod)
        {
            eState = tmbslFrontEndStateNotLocked;
        }
        else
            eState = tmbslFrontEndStateSearching;
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoMod(0x%08X) failed.", pObj->tUnit));

    return eState;
}

/*============================================================================*/
/* TDA10024_AlgoClkOffset                                                     */
/*============================================================================*/
static tmbslFrontEndState_t
TDA10024_AlgoClkOffset
(
    tmUnitSelect_t tUnit
)
{
    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    tmbslFrontEndState_t eState = tmbslFrontEndStateUnknown;
    UInt32               uSrOffset, uNewSr;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoClkOffset(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* calculate the offset +-400ppm - assumed +/-480ppm for DYN */
        uSrOffset = pObj->sCurrentChannel.uSR*40/100000;

        /* set the offset */
        switch (pObj->bStateClkOffset)
        {
        case 0:
            pObj->lClkOffCst = -2*400;
            pObj->bStateClkOffset++;
            uNewSr = pObj->sCurrentChannel.uSR - 2*uSrOffset;
            eState = tmbslFrontEndStateSearching;
            break;

        case 1:
            pObj->lClkOffCst = +2*400;
            pObj->bStateClkOffset++;
            uNewSr = pObj->sCurrentChannel.uSR + 2*uSrOffset;
            eState = tmbslFrontEndStateSearching;
            break;

        case 2:
            pObj->lClkOffCst = -4*400;
            pObj->bStateClkOffset++;
            uNewSr = pObj->sCurrentChannel.uSR - 4*uSrOffset;
            eState = tmbslFrontEndStateSearching;
            break;

        case 3:
            pObj->lClkOffCst = +4*400;
            pObj->bStateClkOffset++;
            uNewSr = pObj->sCurrentChannel.uSR + 4*uSrOffset;
            eState = tmbslFrontEndStateSearching;
            break;
        default:
            pObj->lClkOffCst = 0;
            pObj->bStateClkOffset = 0;
            uNewSr = pObj->sCurrentChannel.uSR;
            eState = tmbslFrontEndStateNotLocked;
        }
        /* write new SR */
        err = TDA10024WriteSR (tUnit, &uNewSr); 
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoClkOffset(0x%08X) failed.", pObj->tUnit));

    return eState;
}

/*============================================================================*/
/* TDA10024_AlgoExit                                                          */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoExit
(
    tmUnitSelect_t tUnit
)
{   
    tmErrorCode_t err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8 uByte = 0;

    /* switch KAGCTUN and KAGCIF to tracking mode (high constant time) for the hum */
    uByte = 0xBF;
    err = TDA10024Write(tUnit, TDA10024_AGCCONF1_IND, 1, &uByte);

    return TM_OK;
}

/*============================================================================*/
/* TDA10024_AlgoInit                                                */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoInit
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peStatus
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uByte;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoInit(0x%08X)", tUnit);
    

    /* In Auto QAM we always begin with 64QAM */
    if (pObj->sApiFlags.iAutoQam )
    {
        if (err == TM_OK)
        {
            /* write the new modulation */
            err = TDA10024WriteQAM(tUnit, 2);
        }
        if (err == TM_OK)
        {
            pObj->sCurrentChannel.eMOD = tmFrontEndModulationQam64;
        }

    }

    if (err == TM_OK)
    {
        /* Init of algorithm variables */
        pObj->lClkOffCst = 0;
        pObj->bStateClkOffset = 0;

        /* set KAGCTUN and KAGCIF to acqusition mode (low constant time) */
        uByte = 0x92;
        err = TDA10024Write(tUnit, TDA10024_AGCCONF1_IND, 1, &uByte);
    }

    if (err == TM_OK)
    {
        /* program the CARCONF */
        err = TDA10024WriteBit(tUnit, TDA10024_CARCONF_IND, TDA10024_CARCONF_CAR_C_MSK, pObj->sConfig.bCAR_C);
    }

    if (err == TM_OK)
    {
        /* reset all chip */
        err = TDA10024ResetAll(tUnit);
    }

    if (err == TM_OK)
    {
        /* 10ms: depends on KAGC settings (KAGCTUN = 2 & KAGCIF = 2 => 5ms + 5ms) */
        /* Check if time delay needs to be ajusted towards sampling clock         */
        err = TDA10024Wait(tUnit, 10);
    }

    *peStatus = tmbslFrontEndStateSearching;

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoInit(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024_AlgoGainManagement                                                */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoGainManagement
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peStatus
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uAgcRF;
    UInt8               uAgcIF;
    UInt32              uTime = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoGainManagement(0x%08X)", tUnit);
    
    *peStatus = tmbslFrontEndStateUnknown;

    /* If there is no signal, we can decide to return "No Carrier" */
    if ((pObj->sCurrentChannel.uSR >= 3000000)
        && (pObj->bNoCarrierIfAGCMax))
    { 
        if (err == TM_OK)
        {
            /* read the AGC RF value */
            err = TDA10024Read(tUnit,  TDA10024_AGCTUN_IND, 1, &uAgcRF);
            err = TDA10024Read(tUnit,  TDA10024_AGCIF_IND, 1, &uAgcIF);
        }
        if (err == TM_OK)
        {
            if ((uAgcRF >= 240) && (uAgcIF >= 240))
            {
                *peStatus = tmbslFrontEndStateNotLocked;
            }
            else
            {
                uAgcRF = uAgcRF;
            }
        }
    }

    if (*peStatus != tmbslFrontEndStateNotLocked)
    {
        /* Different gain management regarding the Band type */
        if ( pObj->sConfig.eBandType == tmbslTDA10024BandTypeSecamL )  
        {
            if (err == TM_OK)
            {
                err = TDA10024_AlgoSecamLGain(tUnit);
            }
            if (err == TM_OK)
            {
                *peStatus = tmbslFrontEndStateSearching;
            }
        }
        else
        {
            if (err == TM_OK)
            {
                err = TDA10024_AlgoNormalGain(tUnit, peStatus, &uTime);
            }            
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoGainManagement(0x%08X) failed.", pObj->tUnit));

    return err;
}

/*============================================================================*/
/* TDA10024_AlgoNormalGain                                                    */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoNormalGain
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peStatus,
    UInt32               *puTime
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    Int32               lRCentralCoef=0, lICentralCoef=0;
    UInt8               puReadCoef[2];
    UInt8               bGainMax;
    UInt32              bGainNyq;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoNormalGain(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* read the real part of the central coef of the equalizer */
        err = TDA10024Read(tUnit, TDA10024_REQCO_CENTRALCOEF_IND, 2, puReadCoef);
    }

    if (err == TM_OK)
    {
        lRCentralCoef = puReadCoef[0] << 3 | puReadCoef[1] >> 5;
        if (lRCentralCoef & 0x400)
        {
            lRCentralCoef |= 0xFFFFF800;
        }

        /* lRCentralCoef^2 */
        lRCentralCoef *= lRCentralCoef;

        /* read the imaginary part of the central coef of the equalizer */
        err = TDA10024Read(tUnit, TDA10024_IEQCO_CENTRALCOEF_IND, 2, puReadCoef);
    }
    
    if (err == TM_OK)
    {
        lICentralCoef = puReadCoef[0] << 3 | puReadCoef[1] >> 5;
        if (lICentralCoef & 0x400)
            lICentralCoef |= 0xFFFFF800;

        /* lICentralCoef^2 */
        lICentralCoef *= lICentralCoef;
    }

    /* test the module */
    if ((lRCentralCoef + lICentralCoef) > 700*700) 
    {
    
        if (err == TM_OK)
        {
            /* don't test all gains in scanning */
            if (pObj->sApiFlags.iScan)
                bGainMax = 5;
            else
                bGainMax = 7;
        
            /* Read GNYQ */
            err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, puReadCoef);
        } 
        if (err == TM_OK)
        {
            bGainNyq= puReadCoef[0] >> 5;

            /* test if gain max is reached */
            if (bGainNyq >= bGainMax)
            {
                *peStatus = tmbslFrontEndStateNotLocked;
                *puTime = 0;
            }
            else
            {
                /* wait for synchro */
                *puTime = TDA10024_CalcTimer(tUnit, 10000);
                *peStatus = tmbslFrontEndStateSearching;
            }
        }
    }
    else
    {
        if (err == TM_OK)
        {
            *puTime = 0;
            *peStatus = tmbslFrontEndStateSearching;
        }
    }


    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoNormalGain(0x%08X) failed.", pObj->tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024_AlgoSecamLGain                                                    */
/*============================================================================*/
static tmErrorCode_t 
TDA10024_AlgoSecamLGain
(
    tmUnitSelect_t tUnit
) 
{ 
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8               uByte, uByteSat[7];
    Int8                bGain=0, bGainMax=0, bGainMin=0;
    UInt32              uSATADC, uHALFADC, uSATDEC1, uSATDEC2, uSATDEC3, uSATAAF, uSATNYQ;
    Int8                bTEST_SAT=0, bTEST_HALFADC=0;
    UInt8 uIFAGC;
    UInt8 uRFAGC;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoSecamLGain(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        /* don't test all gains in scanning */
        if (pObj->sApiFlags.iScan)
        {
            bGainMin = 3;
            bGainMax = 4;
        }
        else
        {
            bGainMin = 2;
            bGainMax = 6;
        }

        bGain = bGainMin-1;
    }

    do
    {
        if (err == TM_OK)
        {
            /* Increase GNYQ gain */
            bGain++;
            err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte); 
        }
        if (err == TM_OK)
        {
            uByte &= 0x8f;
            uByte |= (bGain<<4);
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND,1, &uByte); 
        } 
        if (err == TM_OK)
        {
            err = TDA10024Wait(tUnit, (UInt32)(1000*50*(pObj->uNbSamplesAGC)/(pObj->sCurrentChannel.uSR)));
        }
        if (err == TM_OK)
        {
            err = TDA10024Wait(tUnit, (UInt32)(1000*2*(pObj->uNbSamplesSSAT)/(pObj->sCurrentChannel.uSR)));
        }
        if (err == TM_OK)
        {
            /* Read saturation counters */
            err = TDA10024Read(tUnit, TDA10024_SATNYQ_IND, 7, uByteSat); 
        } 
        if (err == TM_OK)
        {
            uSATADC  = uByteSat[1]; 
            uHALFADC = uByteSat[2]; 
            uSATDEC1 = uByteSat[3]; 
            uSATDEC2 = uByteSat[4]; 
            uSATDEC3 = uByteSat[5]; 
            uSATAAF  = uByteSat[6]; 
            uSATNYQ  = uByteSat[0]; 

            /* Time to refresh sat register = Nb of samples selected in SSAT register / Symbol Rate */
            err = TDA10024Wait(tUnit, (UInt32)(1000*2*(pObj->uNbSamplesSSAT)/(pObj->sCurrentChannel.uSR)));
        }   
        if (err == TM_OK)
        {
            /* Read twice HALFADC to take highest value */
            err = TDA10024Read(tUnit, TDA10024_HALFADC_IND, 1, &uByte); 
        }
        if (err == TM_OK)
        {
            uHALFADC = (uHALFADC>uByte) ? uHALFADC : uByte;

            bTEST_SAT = (Int8)((uSATADC > 1) || (uSATDEC1 > 1) || (uSATDEC2 > 1) || (uSATDEC3 > 1) || (uSATAAF > 1) || (uSATNYQ > 1));
            bTEST_HALFADC = (Int8)(uHALFADC == 0xFF);
        }
    }
    while ((bGain < bGainMax) && (bTEST_SAT || bTEST_HALFADC) && (err == TM_OK));
    
    /* Wait for the AGC convergence */
    if (err == TM_OK)
    {
        err = TDA10024Wait(tUnit, 10);
    }

    if (err == TM_OK)
    {
        /* read the AGC RF value */
        err = TDA10024Read(tUnit,  TDA10024_AGCTUN_IND, 1, &uRFAGC);
        err = TDA10024Read(tUnit,  TDA10024_AGCIF_IND, 1, &uIFAGC);
    }
 
    /* To prevent saturation in high RF level */
    while ((uRFAGC == 0) && (uIFAGC == 0)&& (bGain>bGainMin))
    {
        if (err == TM_OK)
        {
            /* Decrease GNYQ gain */
            bGain--;
            err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte); 
        }
        if (err == TM_OK)
        {
            uByte &= 0x8f;
            uByte |= (bGain<<4);
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND,1, &uByte); 
        }
        
        /* Wait for the AGC convergence */
        if (err == TM_OK)
        {
            err = TDA10024Wait(tUnit, 10);
        }

        if (err == TM_OK)
        {
            /* read the AGC RF value */
            err = TDA10024Read(tUnit,  TDA10024_AGCTUN_IND, 1, &uRFAGC);
            err = TDA10024Read(tUnit,  TDA10024_AGCIF_IND, 1, &uIFAGC);
        }

    }


    while ((uRFAGC == 255) && (uIFAGC == 255)&& (bGain<bGainMax))
    {
        if (err == TM_OK)
        {
            /* Decrease GNYQ gain */
            bGain++;
            err = TDA10024Read(tUnit, TDA10024_GAIN1_IND, 1, &uByte); 
        }
        if (err == TM_OK)
        {
            uByte &= 0x8f;
            uByte |= (bGain<<4);
            err = TDA10024Write(tUnit, TDA10024_GAIN1_IND,1, &uByte); 
        }
        
        /* Wait for the AGC convergence */
        if (err == TM_OK)
        {
            err = TDA10024Wait(tUnit, 10);
        }

        if (err == TM_OK)
        {
            /* read the AGC RF value */
            err = TDA10024Read(tUnit,  TDA10024_AGCTUN_IND, 1, &uRFAGC);
            err = TDA10024Read(tUnit,  TDA10024_AGCIF_IND, 1, &uIFAGC);
        }

    }
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoSecamLGain(0x%08X) failed.", pObj->tUnit));

    return err; 
} 

/*============================================================================*/
/* TDA10024_AlgoEqualizerConvergence                                          */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoEqualizerConvergence
(
    tmUnitSelect_t           tUnit,
    tmbslFrontEndState_t     *peStatus,
    tmbslTDA10024AlgoState_t *peNextState
)
{
    ptmTDA10024Object_t pObj = Null;
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32              uTotalTime = 0;
    UInt32              uTickPeriod = 0;
    UInt8               uMSE = 0;
    UInt8               uMSETH = 0;
    UInt32              uTimeOut;
    tmbslTDA10024State_t eState = tmbslTDA10024StateNotLocked;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoEqualizerConvergence(0x%08X)", tUnit);
    
    if (err == TM_OK)
    {
        if (pObj->sApiFlags.iScan)
        {
            uTimeOut = 600; /* short timeout for scanning */
            uTickPeriod = 1;
            
            do
            {
                if (err == TM_OK)
                {
                    err = TDA10024Wait(tUnit, uTickPeriod); 
                }
                if (err == TM_OK)
                {
                    uTotalTime += uTickPeriod;
                    err = TDA10024GetLockStatus(tUnit, &eState);
                }
            }        
            while ((uTotalTime < uTimeOut) && (eState != tmbslTDA10024StateLocked) && (err == TM_OK));

            if (err == TM_OK)
            {
                if (eState != tmbslTDA10024StateLocked)
                {
                        *peNextState = tmbslTDA10024AlgoStateInit;
                        *peStatus = tmbslFrontEndStateNotLocked;
                }
                else
                {
                    *peNextState = tmbslTDA10024AlgoStateInit;
                    *peStatus = tmbslFrontEndStateLocked;
                }
            }
        }
        else
        {
            uTimeOut = 1500; /* long timeout for difficult fading */
            uTickPeriod = 10;
            err = TDA10024Read(tUnit, TDA10024_MSETH_IND, 1, &uMSETH);

            do
            {
                if (err == TM_OK)
                {
                    err = TDA10024Wait(tUnit, uTickPeriod); 
                }
                if (err == TM_OK)
                {
                    uTotalTime += uTickPeriod;
                    err = TDA10024Read(tUnit, TDA10024_MSE_IND, 1, &uMSE);
                }
            }        
            while ((uTotalTime < uTimeOut) && (uMSE>=uMSETH) && (err == TM_OK));

            if (err == TM_OK)
            {
                if ((uTotalTime >= uTimeOut) && (uMSE>=uMSETH))
                {
                    /* If the equalizer did not converge, reutrn not locked */    
                    *peNextState = tmbslTDA10024AlgoStateInit;
                        *peStatus = tmbslFrontEndStateNotLocked;
                }
                else
                {
                    /* If the equalizer converged, go to next state */
                    *peNextState = tmbslTDA10024AlgoStateAutoChannel;
                    *peStatus = tmbslFrontEndStateSearching;
                }
            }
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoEqualizerConvergence(0x%08X) failed.", pObj->tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024_AlgoAutoChannel                                                   */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoAutoChannel
(
    tmUnitSelect_t           tUnit,
    tmbslFrontEndState_t     *peStatus,
    tmbslTDA10024AlgoState_t *peNextState
)
{
    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32               uTime = 0;
    UInt32               uNbSymbols = 0;
    tmbslTDA10024State_t eLockStatus;
    tmbslFrontEndState_t eAutoQAMStatus = tmbslFrontEndStateNotLocked;
    tmbslFrontEndState_t eClkOffsetStatus = tmbslFrontEndStateNotLocked;
    UInt32               i = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoAutoChannel(0x%08X)", tUnit);
    
    *peStatus = tmbslFrontEndStateUnknown;

    /* Wait for frame synchro */
    do
    {
        if (err == TM_OK)
        {
            /* We wait 19 frames, it is the worst case based on p45 of the datasheet v1.6, with alpha = 6 */
            uNbSymbols = TDA10024_CalcSymbols(tUnit, 107);
            uTime = TDA10024_CalcTimer(tUnit, uNbSymbols);
            err = TDA10024Wait(tUnit, uTime);
        }
        if (err == TM_OK)
        {
            /* read the synchro register */
            err = TDA10024GetLockStatus(tUnit, &eLockStatus);
            i++;
        }
    } while ((eLockStatus == tmbslTDA10024StateNotSynchronised) && (i<5) && (err == TM_OK));
    
    if (err == TM_OK)
    {
        if ((eLockStatus == tmbslTDA10024StateSynchronised) || (eLockStatus == tmbslTDA10024StateLocked))
        {
            /* If the chip is synchronised, no need to search for channel parameters */
            *peStatus = tmbslFrontEndStateSearching;
            *peNextState = tmbslTDA10024AlgoStateLockVerif;
        }
        else
        {

            if (pObj->sApiFlags.iAutoQam)
            {
                eAutoQAMStatus = TDA10024_AlgoMod(tUnit);
            }

            /* If the chip is not synchronised and extended clk recovery is used */
            if ((eAutoQAMStatus == tmbslFrontEndStateNotLocked) && (pObj->sConfig.bClkOffsetRange))
            {
                eClkOffsetStatus = TDA10024_AlgoClkOffset(tUnit);
            }
            
            if ((eAutoQAMStatus == tmbslFrontEndStateNotLocked) && (eClkOffsetStatus == tmbslFrontEndStateNotLocked))
            {
                /* All clks have been tested */
                *peStatus = tmbslFrontEndStateNotLocked;
                *peNextState = tmbslTDA10024AlgoStateInit;
            }
            else
            {
                /* Wait for equalizer because of change in SR */
                *peStatus = tmbslFrontEndStateSearching;
                *peNextState = tmbslTDA10024AlgoStateEqualizerConvergence; 
            }
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoAutoChannel(0x%08X) failed.", pObj->tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024_AlgoStuffingIssue                                                  */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoStuffingIssue
(
    tmUnitSelect_t       tUnit,
    tmbslFrontEndState_t *peStatus
)
{
    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    UInt8                uByteIT,uByteTestUncor,uByteUncor;
    UInt8                puBytes[4];
    UInt8                uByteW;
    UInt32               uTick = 0;
    UInt32               uTotalTime = 0;
    UInt32               j = 0;
    tmbslFrontEndState_t eLockStatus;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoStuffingIssue(0x%08X)", tUnit);
    
    *peStatus = tmbslFrontEndStateUnknown;

    /* test if frame sync and DVB */

    if (err == TM_OK)
    {
        uByteTestUncor = 0;
        uByteUncor = 0;
        do
        {
            if (j>0)
            {
                if (err == TM_OK)
                {
                    uByteW = 0xFB;
                    err = TDA10024Write(tUnit, TDA10024_RESET_IND , 1, &uByteW);
                }
                if (err == TM_OK)
                {
                    /* wait 100ms */
                    err = TDA10024Wait(tUnit, 100);
                }
            }

            if (err == TM_OK)
            {
                /* read the Uncors register a first time */
                err = TDA10024Read(tUnit, TDA10024_CPT_RSB_UNCOR1_IND, 4, puBytes);
            }

            if (err == TM_OK)
            {
                uByteTestUncor = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0]; 

                /* reset the Uncor interruption */
                err = TDA10024Read(tUnit, TDA10024_ITSTAT1_IND, 1, &uByteIT);
            }
            if (err == TM_OK)
            {
                /* wait 1ms */
                err = TDA10024Wait(tUnit, 1);
            }
            if (err == TM_OK)
            {
                /* Read the Uncors interruption register */
                err = TDA10024Read(tUnit, TDA10024_ITSTAT1_IND, 1, &uByteIT);
            }

            if (err == TM_OK)
            {
                /* reda the Uncors register a second time */
                err = TDA10024Read(tUnit, TDA10024_CPT_RSB_UNCOR1_IND, 4, puBytes);
            }
            if (err == TM_OK)
            {
                uByteUncor = (puBytes[3]<<24) | (puBytes[2]<<16) | (puBytes[1]<<8) | puBytes[0]; 
                j++;
            }

        } while ((uByteUncor == uByteTestUncor) && ((uByteIT & 0x04) != 0x00) && (j<10) && (err == TM_OK));
    }

    if (j == 10)
    {    
        if (err == TM_OK)
        {
            TDA10024_AlgoExit(tUnit);
            *peStatus = tmbslFrontEndStateNotLocked;
        }
    }
    else
    {
        if (err == TM_OK)
        {
            uTick = 5;
            uTotalTime = 0;
            err = tmbslTDA10024GetLockStatus(tUnit, &eLockStatus);
        }

        while ((eLockStatus != tmbslFrontEndStateLocked) && (uTotalTime < 100) && (err == TM_OK))
        {
            if (err == TM_OK)
            {
                err = TDA10024Wait(tUnit, 5);
            }
            if (err == TM_OK)
            {
                err = tmbslTDA10024GetLockStatus(tUnit, &eLockStatus);
            }
            if (err == TM_OK)
            {
                uTotalTime += uTick;
            }
        }

    
        if (eLockStatus != tmbslFrontEndStateLocked)
        {
            if (err == TM_OK)
            {
                TDA10024_AlgoExit(tUnit);
            }
            if (err == TM_OK)
            {
                *peStatus = tmbslFrontEndStateNotLocked;
            }
        }
        else
        {
            if (err == TM_OK)
            {
                TDA10024_AlgoExit(tUnit);
            }
            if (err == TM_OK)
            {
                *peStatus = tmbslFrontEndStateLocked;
            }
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoStuffingIssue(0x%08X) failed.", pObj->tUnit));
    
    return err;
}

/*============================================================================*/
/* TDA10024_AlgoLockVerif                                                     */
/*============================================================================*/
static tmErrorCode_t
TDA10024_AlgoLockVerif
(
    tmUnitSelect_t           tUnit,
    tmbslFrontEndState_t     *peStatus,
    tmbslTDA10024AlgoState_t *peNextState
)
{
    ptmTDA10024Object_t  pObj = Null;
    tmErrorCode_t        err = TDA10024_ERR_NOT_INITIALIZED;
    UInt32               uTime = 0;
    UInt32               uNbSymbols = 0;
    tmbslTDA10024State_t eLockStatus = tmbslTDA10024StateUnknown;
    UInt32               i = 0;

    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    tmDBGPRINTEx(DEBUGLVL_VERBOSE, "TDA10024_AlgoLockVerif(0x%08X)", tUnit);
    
    do
    {
        if (err == TM_OK)
        {
            err = TDA10024GetLockStatus(tUnit, &eLockStatus);
        }
        if (eLockStatus != tmbslTDA10024StateLocked)
        {
            if (err == TM_OK)
            {
                /* Wait for 1 packet for the read solomon decoder   */
                /* and 8 packets for the Descrambler                */
                /* (p46 of the datasheet v1.6)                      */
                uNbSymbols = TDA10024_CalcSymbols(tUnit, 9);
                uTime = TDA10024_CalcTimer(tUnit,uNbSymbols);
                err = TDA10024Wait(tUnit, uTime);
            }
            if (err == TM_OK)
            {
                err = TDA10024GetLockStatus(tUnit, &eLockStatus);
            }
            if (err == TM_OK)
            {
                i++;
            }
        }
    } while ((eLockStatus != tmbslTDA10024StateLocked) && (i<5) && (err == TM_OK));
    
    if (err == TM_OK)
    {
        if (eLockStatus != tmbslTDA10024StateLocked)
        {
            *peStatus = tmbslFrontEndStateNotLocked;
            *peNextState = tmbslTDA10024AlgoStateInit;
        }
        else if(pObj->sConfig.eModeDvbMcns == tmbslTDA10024FECModeAnnexB)
        {
            *peStatus = tmbslFrontEndStateSearching;
            *peNextState = tmbslTDA10024AlgoStateStuffingIssue;
        }
        else
        {
            *peStatus = tmbslFrontEndStateLocked;
            *peNextState = tmbslTDA10024AlgoStateInit;
        }
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "TDA10024_AlgoLockVerif(0x%08X) failed.", pObj->tUnit));
    
    return err;
}


#ifdef NO_FLOAT

/*============================================================================*/
/* Math functions:                                                            */
/*============================================================================*/


/*============================================================================*/
/* TDA10024Mul64                                                              */
/*============================================================================*/
void TDA10024Mul64(UInt32 uUp1, UInt32 uUp2, UInt32* uHighInt, UInt32* uLowInt)
{
    UInt32    uLowIntTmp, uHighIntTmp, uUp1_16, uUp1_0, uUp2_16, uUp2_0;
    UInt8     bCounter;

    /* init value */
    uUp1_0    = uUp1 & 0x0000FFFF;
    uUp1_16 = uUp1 >> 16;
    uUp2_0    = uUp2 & 0x0000FFFF;
    uUp2_16 = uUp2 >> 16;

    /* calculate X1Y1 */
    *uHighInt = uUp1_16*uUp2_16;

    /* calculate X0Y1+X1Y0 */
    uLowIntTmp = uUp1_16*uUp2_0+uUp1_0*uUp2_16;
    uHighIntTmp = 0;
    for (bCounter = 0; bCounter < 16; bCounter++)
    {
        if ((uLowIntTmp & 0x80000000) || (uHighIntTmp != 0))
        {
            uHighIntTmp <<= 1; 
            uHighIntTmp |= (uLowIntTmp>>31);
        }
        uLowIntTmp <<= 1;
    }

    /* test overflow for low byte */
    *uLowInt    = (uUp1_0*uUp2_0)>>1;
    *uLowInt += uLowIntTmp>>1;
    if (*uLowInt & 0x80000000)
        uHighIntTmp++;

    /* calculate X0Y0 Low Byte */
    *uLowInt = uUp1_0*uUp2_0+uLowIntTmp; 

    /* calculate High Byte */
    *uHighInt += uHighIntTmp;
}

/*============================================================================*/
/* TDA10024Div64                                                              */
/*============================================================================*/
UInt32 TDA10024Div64(UInt32 uHighInt, UInt32 uLowInt, UInt32 uDown)
{
    UInt32 volatile uResult;     /* no optimisation */
    UInt32 volatile uRemain1;    /* no optimisation */
    UInt32 volatile uRemain2;    /* no optimisation */
    UInt32 uHighInt1, uLowInt1;
    UInt32 uHighInt2, uLowInt2, uRemain3;

    /* divide uHighInt by uDown */
    uResult = 0xFFFFFFFF/uDown;
    uResult *= uHighInt;

    /* calculate the remainder */
    TDA10024Mul64((0xFFFFFFFF%uDown), uHighInt, &uHighInt1, &uLowInt1);

    /* uRemain = Div64(uHighIntTmp, uLowIntTmp, uDown); */
    /* divide the remainder */
    uRemain1 =    0xFFFFFFFF/uDown;
    uRemain1 *= uHighInt1;

    /* calculate the remainder of the remainder */
    TDA10024Mul64((0xFFFFFFFF%uDown), uHighInt1, &uHighInt2, &uLowInt2);

    /* uRemain = Div64(uHighIntTmp, uLowIntTmp, uDown); */
    /* divide the remainder                             */
    uRemain2 =    0xFFFFFFFF/uDown;
    uRemain2 *= uHighInt2;

    /* calculate the remainder of the remainder */
    uRemain3 = 0xFFFFFFFF%uDown;
    uRemain3 *= uHighInt2;

    /* update the remainder */
    uRemain2 += (uLowInt2+uHighInt2+uRemain3+(uDown/2))/uDown;

    /* update the remainder */
    uRemain1 += (uLowInt1+uHighInt1+(uDown/2))/uDown + uRemain2;

    /* divide uLowInt by uDown and add remain */
    uResult += (uLowInt+uHighInt+(uDown/2))/uDown + uRemain1;

    return uResult;
}

/*============================================================================*/
/* TDA10024Div64                                                              */
/*============================================================================*/
UInt32 TDA10024MulDiv32(UInt32 uUp1, UInt32 uUp2, UInt32 uDown)
{
    UInt32 uHighInt, uLowInt;

    /* multiplied 32bits*32Bits->64bits */
    TDA10024Mul64(uUp1, uUp2, &uHighInt, &uLowInt);

    /* divide 64bits/32bits->32bits */
    return TDA10024Div64(uHighInt, uLowInt, uDown);
}

#endif /* NO_FLOAT */

