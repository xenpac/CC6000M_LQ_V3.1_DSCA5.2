/**
 * Copyright (C) 2005 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmbslTDA10024_.c
 *
 * \date          26-Nov-2007
 *
 * \brief         -
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
   26-Nov-2007  | X.RAZAVET |       |       | 10024 configuration.
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/


/*============================================================================*/
/*                   STANDARD INCLUDE FILES                                   */
/*============================================================================*/


/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/
#include "tmbslTDA10024_Cfg.h"


/*============================================================================*/
/*                       MACRO DEFINITION                                     */
/*============================================================================*/
#define TMBSLTDA10024_CFG_COMPATIBILITY_NB 3

#if TMBSLTDA10024_CFG_VERSION != TMBSLTDA10024_CFG_COMPATIBILITY_NB
    #error "Wrong cfg files version !!!!"
#endif

/*============================================================================*/
/*                   PUBLIC FUNCTIONS DEFINITIONS                             */
/*============================================================================*/

/*============================================================================*/
/* tmbslTDA10024_GetConfig                                                  */
/*============================================================================*/
tmbslTDA10024_Cfg_Err_t
tmbslTDA10024_CFG_GetConfig( tmbslTDA10024_Cfg_t* pConfig )
{
    if (pConfig != (void*)0)
    {
        pConfig->CompatibilityNb_U          = TMBSLTDA10024_CFG_COMPATIBILITY_NB;
        pConfig->IF_U                       = TMBSLTDA10024_CFG_IF;
        pConfig->IQSwapped                  = TMBSLTDA10024_CFG_IQ_SWAPP;
        pConfig->Xtall_U                    = TMBSLTDA10024_CFG_XTALL;
        pConfig->DVB_PLL_MFactor_U          = TMBSLTDA10024_CFG_DVB_PLLMFACTOR;
        pConfig->DVB_PLL_NFactor_U          = TMBSLTDA10024_CFG_DVB_PLLNFACTOR;
        pConfig->DVB_PLL_PFactor_U          = TMBSLTDA10024_CFG_DVB_PLLPFACTOR;
        pConfig->DVB_High_Sampling_Clock    = TMBSLTDA10024_CFG_DVB_HIGH_SAMPLING_CLOCK;
        pConfig->MCNS_PLL_MFactor_U         = TMBSLTDA10024_CFG_MCNS_PLLMFACTOR;
        pConfig->MCNS_PLL_NFactor_U         = TMBSLTDA10024_CFG_MCNS_PLLNFACTOR;
        pConfig->MCNS_PLL_PFactor_U         = TMBSLTDA10024_CFG_MCNS_PLLPFACTOR;
        pConfig->MCNS_High_Sampling_Clock   = TMBSLTDA10024_CFG_MCNS_HIGH_SAMPLING_CLOCK;
        pConfig->OUT_ParaSer_U              = TMBSLTDA10024_CFG_OUT_PARASER;
        pConfig->OUT_ModeABC_U              = TMBSLTDA10024_CFG_OUT_MODE_ABC;
        pConfig->OUT_ParaDiv_U              = TMBSLTDA10024_CFG_OUT_PARADIV;
        pConfig->OUT_MsbLsb_U               = TMBSLTDA10024_CFG_OUT_LSB_MSB;
        pConfig->OUT_Poclk_U                = TMBSLTDA10024_CFG_OUT_POCLK;
        pConfig->OUT_Sync_Width_U           = TMBSLTDA10024_CFG_OUT_SYNC_WIDTH;
        pConfig->OUT_DEN_Pol_U              = TMBSLTDA10024_CFG_OUT_DEN_POL;
        pConfig->OUT_UNCOR_Pol_U            = TMBSLTDA10024_CFG_OUT_UNCOR_POL;
        pConfig->OUT_SYNC_Pol_U             = TMBSLTDA10024_CFG_OUT_SYNC_POL;
        pConfig->AGC1Pola                   = TMBSLTDA10024_CFG_AGC_1_POLA;
        pConfig->AGC2Pola                   = TMBSLTDA10024_CFG_AGC_2_POLA;
        pConfig->AGCTunMax                  = TMBSLTDA10024_CFG_AGC_TUNER_MAX;
        pConfig->AGCTunMin                  = TMBSLTDA10024_CFG_AGC_TUNER_MIN;
        pConfig->AGCIFMax                   = TMBSLTDA10024_CFG_AGC_IF_MAX;
        pConfig->AGCIFMin                   = TMBSLTDA10024_CFG_AGC_IF_MIN;
        pConfig->AGCThreshold               = TMBSLTDA10024_CFG_AGC_THRESHOLD;
        pConfig->EqualType                  = TMBSLTDA10024_CFG_EQUAL_TYPE;
        pConfig->BandType                   = TMBSLTDA10024_CFG_BAND_TYPE;
        pConfig->EqConf1                    = TMBSLTDA10024_CFG_EQCONF1;
        pConfig->CAR_C                      = TMBSLTDA10024_CFG_CAR_C;
        pConfig->GAIN1                      = TMBSLTDA10024_CFG_GAIN1;
        pConfig->SWPos                      = TMBSLTDA10024_CFG_SWPOS;
        return tmbslTDA10024_Cfg_NoError_E;
    }
    else
    {
        return tmbslTDA10024_Cfg_InvalidValue_E;
    }
}


/*============================================================================*/
/*                     END OF FILE                                            */
/*============================================================================*/

