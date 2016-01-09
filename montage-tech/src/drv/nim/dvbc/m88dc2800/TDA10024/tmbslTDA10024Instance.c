/**
  Copyright (C) 2008 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA10024Instance.c
 *                %version: CFR_FEAP#10 %
 *
 * \date          %date_modified%
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
 * \verbatim
   Date          Modified by CRPRNr  TASKNr  Maintenance description
   -------------|-----------|-------|-------|-----------------------------------
   26-Nov-2007  | V.VRIGNAUD|       |       | COMPATIBILITY WITH NXPFE
                | C.CAZETTES|       |       | 
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/

#include "../TDA18250/tmNxTypes.h"
#include "../TDA18250/tmCompId.h"
//#include "..\TDA18250\tmbslFrontEndCfgItem.h"
#include "../TDA18250/tmFrontEnd.h"
#include "../TDA18250/tmbslFrontEndTypes.h"

#include "tmbslTDA10024.h"
#include "tmbslTDA10024local.h"

#include "tmbslTDA10024Instance.h"



/*----------------------------------------------------------------------------*/
/* Global data:                                                               */
/*----------------------------------------------------------------------------*/
tmTDA10024Object_t gTDA10024Instance[] = 
{
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* Unit not set temporary */
        Null,                           /* pMutex */
        {   
            (UInt32)0,                  /* Channel structure not initialized */
            (UInt32)6900000, 
            (tmFrontEndSpecInv_t)0,                
            (tmFrontEndModulation_t)0
        },
        {  
            tmbslTDA10024FECModeAnnexA,
            OM57XX_BANDTYPE_DEF,
            OM57XX_EQCONF1_DEF,
            OM57XX_CAR_C_DEF,
            OM57XX_GAIN1_DEF,
            OM57XX_XTALL_DEF,
            OM57XX_PLLMFACTOR_DVB_DEF,
            OM57XX_PLLNFACTOR_DVB_DEF,
            OM57XX_PLLPFACTOR_DVB_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_PLLMFACTOR_MCNS_DEF,
            OM57XX_PLLNFACTOR_MCNS_DEF,
            OM57XX_PLLPFACTOR_MCNS_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_IFMAX_DEF,
            OM57XX_IFMIN_DEF,
            OM57XX_TUNMAX_DEF,
            OM57XX_TUNMIN_DEF,
            OM57XX_AGCTRESHOLD_DEF,
            OM57XX_EQUALTYPE_DEF,
            OM57XX_POLAPWM1_DEF,
            OM57XX_POLAPWM2_DEF,
            OM57XX_BERDEPTH_DVB_DEF,
            OM57XX_BERWINDOW_DEF,
            OM57XX_CLKOFFSETRANGE_DEF,
            OM57XX_IQSWAP_DEF,
            OM57XX_IF_DVB_DEF,
            OM57XX_OCLK1_DEF,
            OM57XX_PARASER1_DEF,
            OM57XX_MSBFIRST1_DEF,
            OM57XX_MODEABC1_DEF,
            OM57XX_PARADIV1_DEF,
            OM57XX_SYNCWIDTH1_DEF,
            OM57XX_DENP1_DEF,
            OM57XX_UNCORP1_DEF,
            OM57XX_SYNCP1_DEF,
            OM57XX_OCLK2_DEF,
            OM57XX_MSBFIRST2_DEF,
            OM57XX_SYNCWIDTH2_DEF,
            OM57XX_DENP2_DEF,
            OM57XX_UNCORP2_DEF,
            OM57XX_SYNCP2_DEF,
            OM57XX_SWDYN_DEF,
            OM57XX_SWSTEP_DEF,
            OM57XX_SWLENGTH_DEF,
            OM57XX_SWPOS_DEF,
            True
        },
        {0, 0, 0, 0, 0},             /* api flags */
        {0, 0},                         /* Read Write Functions */
        {0, 0},                         /* Time Function */
        {0},                            /* Debug Function */
        {0, 0, 0, 0},                   /* Mutex Function */
        0,                              /* system clock */
        0,                              /* gain algo */
        0,                              /* syncreg */
        0,                              /* state of the algo */
        0,                              /* state of the clock offset algo */
        0,                              /* clock offset range */
        0,                              /* Nb samples of SSAT */
        290,                            /* uSWDynValue */
        468,                            /* uSWStepValue */
        0,                              /* uNbSamplesAGC: Nb of samples used to refresh AGC values */
        0,                              /* uEqualizerSymbolsNb */
        0,                              /* bNoCarrierIfAGCMax */
        0,                              /* uRefSR */
        0,                              /* ePowerState */
        0x40,                           /* uTestRegSave */
    },
    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* Unit not set temporary */
        Null,                           /* pMutex */
        {   
            (UInt32)0,                  /* Channel structure not initialized */
            (UInt32)6900000, 
            (tmFrontEndSpecInv_t)0,                
            (tmFrontEndModulation_t)0
        },
        {
            tmbslTDA10024FECModeAnnexA,
            OM57XX_BANDTYPE_DEF,
            OM57XX_EQCONF1_DEF,
            OM57XX_CAR_C_DEF,
            OM57XX_GAIN1_DEF,
            OM57XX_XTALL_DEF,
            OM57XX_PLLMFACTOR_DVB_DEF,
            OM57XX_PLLNFACTOR_DVB_DEF,
            OM57XX_PLLPFACTOR_DVB_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_PLLMFACTOR_MCNS_DEF,
            OM57XX_PLLNFACTOR_MCNS_DEF,
            OM57XX_PLLPFACTOR_MCNS_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_IFMAX_DEF,
            OM57XX_IFMIN_DEF,
            OM57XX_TUNMAX_DEF,
            OM57XX_TUNMIN_DEF,
            OM57XX_AGCTRESHOLD_DEF,
            OM57XX_EQUALTYPE_DEF,
            OM57XX_POLAPWM1_DEF,
            OM57XX_POLAPWM2_DEF,
            OM57XX_BERDEPTH_DVB_DEF,
            OM57XX_BERWINDOW_DEF,
            OM57XX_CLKOFFSETRANGE_DEF,
            OM57XX_IQSWAP_DEF,
            OM57XX_IF_DVB_DEF,
            OM57XX_OCLK1_DEF,
            OM57XX_PARASER1_DEF,
            OM57XX_MSBFIRST1_DEF,
            OM57XX_MODEABC1_DEF,
            OM57XX_PARADIV1_DEF,
            OM57XX_SYNCWIDTH1_DEF,
            OM57XX_DENP1_DEF,
            OM57XX_UNCORP1_DEF,
            OM57XX_SYNCP1_DEF,
            OM57XX_OCLK2_DEF,
            OM57XX_MSBFIRST2_DEF,
            OM57XX_SYNCWIDTH2_DEF,
            OM57XX_DENP2_DEF,
            OM57XX_UNCORP2_DEF,
            OM57XX_SYNCP2_DEF,
            OM57XX_SWDYN_DEF,
            OM57XX_SWSTEP_DEF,
            OM57XX_SWLENGTH_DEF,
            OM57XX_SWPOS_DEF,
            True
        },
        {0, 0, 0, 0, 0},             /* api flags */
        {0, 0},                         /* Read Write Functions */
        {0, 0},                         /* Time Function */
        {0},                            /* Debug Function */
        {0, 0, 0, 0},                   /* Mutex Function */
        0,                              /* system clock */
        0,                              /* gain algo */
        0,                              /* syncreg */
        0,                              /* state of the algo */
        0,                              /* state of the clock offset algo */
        0,                              /* clock offset range */
        0,                              /* Nb samples of SSAT */
        290,                            /* uSWDynValue */
        468,                            /* uSWStepValue */
        0,                              /* uNbSamplesAGC: Nb of samples used to refresh AGC values */
        0,                              /* uEqualizerSymbolsNb */
        0,                              /* bNoCarrierIfAGCMax */
        0,                              /* uSRStep */
        0,                              /* ePowerState */
        0x40,                           /* uTestRegSave */
    },    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* Unit not set temporary */
        Null,                           /* pMutex */
        {   
            (UInt32)0,                  /* Channel structure not initialized */
            (UInt32)6900000, 
            (tmFrontEndSpecInv_t)0,                
            (tmFrontEndModulation_t)0
        },
        {
            tmbslTDA10024FECModeAnnexA,
            OM57XX_BANDTYPE_DEF,
            OM57XX_EQCONF1_DEF,
            OM57XX_CAR_C_DEF,
            OM57XX_GAIN1_DEF,
            OM57XX_XTALL_DEF,
            OM57XX_PLLMFACTOR_DVB_DEF,
            OM57XX_PLLNFACTOR_DVB_DEF,
            OM57XX_PLLPFACTOR_DVB_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_PLLMFACTOR_MCNS_DEF,
            OM57XX_PLLNFACTOR_MCNS_DEF,
            OM57XX_PLLPFACTOR_MCNS_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_IFMAX_DEF,
            OM57XX_IFMIN_DEF,
            OM57XX_TUNMAX_DEF,
            OM57XX_TUNMIN_DEF,
            OM57XX_AGCTRESHOLD_DEF,
            OM57XX_EQUALTYPE_DEF,
            OM57XX_POLAPWM1_DEF,
            OM57XX_POLAPWM2_DEF,
            OM57XX_BERDEPTH_DVB_DEF,
            OM57XX_BERWINDOW_DEF,
            OM57XX_CLKOFFSETRANGE_DEF,
            OM57XX_IQSWAP_DEF,
            OM57XX_IF_DVB_DEF,
            OM57XX_OCLK1_DEF,
            OM57XX_PARASER1_DEF,
            OM57XX_MSBFIRST1_DEF,
            OM57XX_MODEABC1_DEF,
            OM57XX_PARADIV1_DEF,
            OM57XX_SYNCWIDTH1_DEF,
            OM57XX_DENP1_DEF,
            OM57XX_UNCORP1_DEF,
            OM57XX_SYNCP1_DEF,
            OM57XX_OCLK2_DEF,
            OM57XX_MSBFIRST2_DEF,
            OM57XX_SYNCWIDTH2_DEF,
            OM57XX_DENP2_DEF,
            OM57XX_UNCORP2_DEF,
            OM57XX_SYNCP2_DEF,
            OM57XX_SWDYN_DEF,
            OM57XX_SWSTEP_DEF,
            OM57XX_SWLENGTH_DEF,
            OM57XX_SWPOS_DEF,
            True
        },
        {0, 0, 0, 0, 0},             /* api flags */
        {0, 0},                         /* Read Write Functions */
        {0, 0},                         /* Time Function */
        {0},                            /* Debug Function */
        {0, 0, 0, 0},                   /* Mutex Function */
        0,                              /* system clock */
        0,                              /* gain algo */
        0,                              /* syncreg */
        0,                              /* state of the algo */
        0,                              /* state of the clock offset algo */
        0,                              /* clock offset range */
        0,                              /* Nb samples of SSAT */
        290,                            /* uSWDynValue */
        468,                            /* uSWStepValue */
        0,                              /* uNbSamplesAGC: Nb of samples used to refresh AGC values */
        0,                              /* uEqualizerSymbolsNb */
        0,                              /* bNoCarrierIfAGCMax */
        0,                              /* uSRStep */
        0,                              /* ePowerState */
        0x40,                           /* uTestRegSave */ 
    },    {
        (tmUnitSelect_t)(-1),           /* Unit not set */
        (tmUnitSelect_t)(-1),           /* Unit not set temporary */
        Null,                           /* pMutex */
        {   
            (UInt32)0,                  /* Channel structure not initialized */
            (UInt32)6900000, 
            (tmFrontEndSpecInv_t)0,                
            (tmFrontEndModulation_t)0
        },
        {
            tmbslTDA10024FECModeAnnexA,
            OM57XX_BANDTYPE_DEF,
            OM57XX_EQCONF1_DEF,
            OM57XX_CAR_C_DEF,
            OM57XX_GAIN1_DEF,
            OM57XX_XTALL_DEF,
            OM57XX_PLLMFACTOR_DVB_DEF,
            OM57XX_PLLNFACTOR_DVB_DEF,
            OM57XX_PLLPFACTOR_DVB_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_PLLMFACTOR_MCNS_DEF,
            OM57XX_PLLNFACTOR_MCNS_DEF,
            OM57XX_PLLPFACTOR_MCNS_DEF,
            OM57XX_FSAMPLING_DEF,
            OM57XX_IFMAX_DEF,
            OM57XX_IFMIN_DEF,
            OM57XX_TUNMAX_DEF,
            OM57XX_TUNMIN_DEF,
            OM57XX_AGCTRESHOLD_DEF,
            OM57XX_EQUALTYPE_DEF,
            OM57XX_POLAPWM1_DEF,
            OM57XX_POLAPWM2_DEF,
            OM57XX_BERDEPTH_DVB_DEF,
            OM57XX_BERWINDOW_DEF,
            OM57XX_CLKOFFSETRANGE_DEF,
            OM57XX_IQSWAP_DEF,
            OM57XX_IF_DVB_DEF,
            OM57XX_OCLK1_DEF,
            OM57XX_PARASER1_DEF,
            OM57XX_MSBFIRST1_DEF,
            OM57XX_MODEABC1_DEF,
            OM57XX_PARADIV1_DEF,
            OM57XX_SYNCWIDTH1_DEF,
            OM57XX_DENP1_DEF,
            OM57XX_UNCORP1_DEF,
            OM57XX_SYNCP1_DEF,
            OM57XX_OCLK2_DEF,
            OM57XX_MSBFIRST2_DEF,
            OM57XX_SYNCWIDTH2_DEF,
            OM57XX_DENP2_DEF,
            OM57XX_UNCORP2_DEF,
            OM57XX_SYNCP2_DEF,
            OM57XX_SWDYN_DEF,
            OM57XX_SWSTEP_DEF,
            OM57XX_SWLENGTH_DEF,
            OM57XX_SWPOS_DEF,
            True
        },
        {0, 0, 0, 0, 0},             /* api flags */
        {0, 0},                         /* Read Write Functions */
        {0, 0},                         /* Time Function */
        {0},                            /* Debug Function */
        {0, 0, 0, 0},                   /* Mutex Function */
        0,                              /* system clock */
        0,                              /* gain algo */
        0,                              /* syncreg */
        0,                              /* state of the algo */
        0,                              /* state of the clock offset algo */
        0,                              /* clock offset range */
        0,                              /* Nb samples of SSAT */
        290,                            /* uSWDynValue */
        468,                            /* uSWStepValue */
        0,                              /* uNbSamplesAGC: Nb of samples used to refresh AGC values */
        0,                              /* uEqualizerSymbolsNb */
        0,                              /* bNoCarrierIfAGCMax */
        0,                              /* uSRStep */
        0,                              /* ePowerState */
        0x40,                           /* uTestRegSave */
    }
};

/*============================================================================*/
/* FUNCTION:    tmbslTDA10024AllocInstance:                                   */
/*                                                                            */
/* DESCRIPTION: allocate new instance                                         */
/*                                                                            */
/* RETURN:                                                                    */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA10024AllocInstance
(
    tmUnitSelect_t       tUnit,         /* I: Unit number   */
    pptmTDA10024Object_t ppDrvObject    /* I: Device Object */
)
{ 
    tmErrorCode_t       err = TDA10024_ERR_BAD_UNIT_NUMBER;
    ptmTDA10024Object_t pObj = Null;
    UInt32              uLoopCounter = 0;    

    /* Find a free instance */
    for(uLoopCounter = 0; uLoopCounter<TDA10024_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gTDA10024Instance[uLoopCounter];
        if(pObj->sApiFlags.iInit == False)
        {
            pObj->tUnit = tUnit;
            pObj->tUnitW = tUnit;
            
            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    /* return value */
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA10024DeAllocInstance:                                 */
/*                                                                            */
/* DESCRIPTION: deallocate instance                                           */
/*                                                                            */
/* RETURN:      always TM_OK                                                  */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA10024DeAllocInstance
(
    tmUnitSelect_t  tUnit    /* I: Unit number */
)
{     
    tmErrorCode_t       err = TDA10024_ERR_BAD_UNIT_NUMBER;
    ptmTDA10024Object_t pObj = Null;
 
    /* check input parameters */
    err = TDA10024GetInstance(tUnit, &pObj);

    /* check driver state */
    if (err == TM_OK)
    {
        if (pObj == Null || pObj->sApiFlags.iInit == False)
        {
            err = TDA10024_ERR_NOT_INITIALIZED;
        }
    }

    if ((err == TM_OK) && (pObj != Null)) 
    {
        pObj->sApiFlags.iInit = False;
    }

    /* return value */
    return err;
}

/*============================================================================*/
/* FUNCTION:    tmbslTDA10024GetInstance:                                     */
/*                                                                            */
/* DESCRIPTION: get the instance                                              */
/*                                                                            */
/* RETURN:      always True                                                   */
/*                                                                            */
/* NOTES:                                                                     */
/*============================================================================*/
tmErrorCode_t
TDA10024GetInstance 
(
    tmUnitSelect_t       tUnit,         /* I: Unit number   */
    pptmTDA10024Object_t ppDrvObject    /* I: Device Object */
)
{     
    tmErrorCode_t       err = TDA10024_ERR_NOT_INITIALIZED;
    ptmTDA10024Object_t pObj = Null;
    UInt32              uLoopCounter = 0;    

    /* get instance */
    for(uLoopCounter = 0; uLoopCounter<TDA10024_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gTDA10024Instance[uLoopCounter];
        if(pObj->sApiFlags.iInit == True && pObj->tUnit == tUnit)
        {
            pObj->tUnitW = tUnit;

            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    /* return value */
    return err;
}


