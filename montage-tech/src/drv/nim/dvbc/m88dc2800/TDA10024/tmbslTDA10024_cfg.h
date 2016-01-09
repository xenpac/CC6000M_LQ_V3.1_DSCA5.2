/**
 * Copyright (C) 2005 Koninklijke Philips Electronics N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of Koninklijke Philips Electronics N.V. and is confidential in
 * nature. Under no circumstances is this software to be  exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of Koninklijke Philips Electronics N.V.
 *
 * \file          tmbslTDA10024_Cfg.c
 *
 * \date          02-Oct-2007
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


#ifndef TMBSLTDA10024_CFG_H 
#define TMBSLTDA10024_CFG_H


/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*/
/*                       ENUM OR TYPE DEFINITION                              */
/*============================================================================*/
typedef enum _tmbslTDA10024_Cfg_Err_t
{
    tmbslTDA10024_Cfg_NoError_E,
    tmbslTDA10024_Cfg_InvalidValue_E
} tmbslTDA10024_Cfg_Err_t;


/*============================================================================*/
/* Generic Configuration items                                                */
/*============================================================================*/


typedef struct _tmbslTDA10024_Cfg_t
{
    unsigned int    CompatibilityNb_U;
    unsigned int    IF_U;
    unsigned char   IQSwapped;
    unsigned int    Xtall_U;
    unsigned char   DVB_PLL_MFactor_U;
    unsigned char   DVB_PLL_NFactor_U;
    unsigned char   DVB_PLL_PFactor_U;
    unsigned char   DVB_High_Sampling_Clock;
    unsigned char   MCNS_PLL_MFactor_U;
    unsigned char   MCNS_PLL_NFactor_U;
    unsigned char   MCNS_PLL_PFactor_U;
    unsigned char   MCNS_High_Sampling_Clock;
    unsigned char   OUT_ParaSer_U;
    unsigned char   OUT_ModeABC_U;
    unsigned char   OUT_ParaDiv_U;
    unsigned char   OUT_MsbLsb_U;
    unsigned char   OUT_Poclk_U;
    unsigned char   OUT_Sync_Width_U;
    unsigned char   OUT_DEN_Pol_U;
    unsigned char   OUT_UNCOR_Pol_U;
    unsigned char   OUT_SYNC_Pol_U;
    unsigned char   AGC1Pola;
    unsigned char   AGC2Pola;
    unsigned char   AGCTunMax;
    unsigned char   AGCTunMin;
    unsigned char   AGCIFMax;
    unsigned char   AGCIFMin;
    unsigned char   AGCThreshold;
    unsigned char   EqualType;
    unsigned char   BandType;
    unsigned char   EqConf1;
    unsigned char   CAR_C;
    unsigned char   GAIN1;
    unsigned char   SWPos;
} tmbslTDA10024_Cfg_t;

/*============================================================================*/
/* macro define                                                               */
/*============================================================================*/

/*============================================================================*/
/* bsl type define                                                            */
/*============================================================================*/

#define TMBSLTDA10024_CFG_VERSION 3

/**********************************************/
/****** TS OUTPUT SELECTION FOR 10024 ******/
/**********************************************/
/* to configure MODE A, B or C output */
/* MODE A: Value = 0                  */
/* MODE B: Value = 1                  */
/* MODE C: Value = 2                  */
#define TMBSLTDA10024_CFG_OUT_MODE_A   0
#define TMBSLTDA10024_CFG_OUT_MODE_B   1
#define TMBSLTDA10024_CFG_OUT_MODE_C   2

/* to configure Parallel or Serial mode */
/* Parallel: Value = 0                  */
/* Serial:   Value = 1                  */
#define TMBSLTDA10024_CFG_OUT_PARALLEL 0
#define TMBSLTDA10024_CFG_OUT_SERIAL   1

/* Parallel mode B clock */
/* default: Value = 7 correspond to a division of the sampling clock by 8 */
/* Value possible [1:16] */

/* to configure the TS with MSB or LSB first */
    /* MSB: Value = 1 */
    /* LSB: Value = 0 */
#define TMBSLTDA10024_CFG_OUT_MSB_FIRST 1
#define TMBSLTDA10024_CFG_OUT_LSB_FIRST 0

    /* to configure sync width of the TS  */
    /* first bit: Value = 0 */
    /* first byte: Value = 1 */
#define TMBSLTDA10024_CFG_OUT_SYNC_FIRST_BIT 0
#define TMBSLTDA10024_CFG_OUT_SYNC_FIRST_BYTE 1

/* to configure clock edge              */
    /* rising edge:  Value = 1              */
    /* falling edge: Value = 0              */
#define TMBSLTDA10024_CFG_OUT_CLOCK_RISING_EDGE  1
#define TMBSLTDA10024_CFG_OUT_CLOCK_FALLING_EDGE 0

    /* to configure SYNC pol of the TS  */
    /* first bit: Value = 0 */
    /* first byte: Value = 1 */
#define TMBSLTDA10024_CFG_OUT_SYNC_NOT_INVERTED    0
#define TMBSLTDA10024_CFG_OUT_SYNC_INVERTED        1

    /* to configure DEN pol of the TS	*/
    /* first bit: Value = 0 */
    /* first byte: Value = 1 */
#define TMBSLTDA10024_CFG_OUT_DEN_NOT_INVERTED    0
#define TMBSLTDA10024_CFG_OUT_DEN_INVERTED        1

    /* to configure UNCOR pol of the TS	*/
    /* first bit: Value = 0 */
    /* first byte: Value = 1 */
#define TMBSLTDA10024_CFG_OUT_UNCOR_NOT_INVERTED    0
#define TMBSLTDA10024_CFG_OUT_UNCOR_INVERTED        1

/****** AGC CONFIGURATION ******/
/* AGC Pola */
#define TMBSLTDA10024_CFG_AGC_POLA_NORMAL   0
#define TMBSLTDA10024_CFG_AGC_POLA_INVERTED 1

/* High Sampling Clock */
#define TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_DISABLED 0
#define TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_ENABLED 1

/****** IF SPECTRAL INVERSION SWAPPED ******/
/* to configure I/Q swapping */
/* not swapped: Value = 0              */
/* swapped:     Value = 1              */
#define TMBSLTDA10024_IQ_SWAPPED        1
#define TMBSLTDA10024_IQ_NOT_SWAPPED    0

/*============================================================================*/
/* SET current configuration HERE                                             */
/*============================================================================*/

/* Sampling clock */
#define TMBSLTDA10024_CFG_XTALL         16000000
#define TMBSLTDA10024_CFG_DVB_PLLMFACTOR    11
#define TMBSLTDA10024_CFG_DVB_PLLNFACTOR            0   /* 0: with Xtal=4MHz , 3: with Xtal=16MHz */
#define TMBSLTDA10024_CFG_DVB_PLLPFACTOR    2
#define TMBSLTDA10024_CFG_DVB_HIGH_SAMPLING_CLOCK   TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_DISABLED

    //#define TMBSLTDA10024_CFG_XTALL                     28920000
    //#define TMBSLTDA10024_CFG_DVB_PLLMFACTOR            7
    //#define TMBSLTDA10024_CFG_DVB_PLLNFACTOR            0   /* 0: with Xtal=4MHz , 3: with Xtal=16MHz */
    //#define TMBSLTDA10024_CFG_DVB_PLLPFACTOR            3
    //#define TMBSLTDA10024_CFG_DVB_HIGH_SAMPLING_CLOCK   TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_ENABLED

    /* code modified in for these values --Austin*/
#define TMBSLTDA10024_CFG_MCNS_PLLMFACTOR           17
#define TMBSLTDA10024_CFG_MCNS_PLLNFACTOR           0   /* 0: with Xtal=4MHz , 3: with Xtal=16MHz */
#define TMBSLTDA10024_CFG_MCNS_PLLPFACTOR           3
#define TMBSLTDA10024_CFG_MCNS_HIGH_SAMPLING_CLOCK  TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_DISABLED

    //#define TMBSLTDA10024_CFG_MCNS_PLLMFACTOR           9
    //#define TMBSLTDA10024_CFG_MCNS_PLLNFACTOR           0   /* 0: with Xtal=4MHz , 3: with Xtal=16MHz */
    //#define TMBSLTDA10024_CFG_MCNS_PLLPFACTOR           3
    //#define TMBSLTDA10024_CFG_MCNS_HIGH_SAMPLING_CLOCK  TMBSLTDA10024_CFG_HIGH_SAMPLING_CLOCK_ENABLED

/* IF */
#define TMBSLTDA10024_CFG_IF            5000000
#define TMBSLTDA10024_CFG_IQ_SWAPP      TMBSLTDA10024_IQ_NOT_SWAPPED

/* Output */
#define TMBSLTDA10024_CFG_OUT_PARASER   TMBSLTDA10024_CFG_OUT_PARALLEL
#define TMBSLTDA10024_CFG_OUT_MODE_ABC  TMBSLTDA10024_CFG_OUT_MODE_A 
#define TMBSLTDA10024_CFG_OUT_PARADIV   7 /* Value possible [1:16] */
#define TMBSLTDA10024_CFG_OUT_LSB_MSB   TMBSLTDA10024_CFG_OUT_MSB_FIRST
#define TMBSLTDA10024_CFG_OUT_POCLK     TMBSLTDA10024_CFG_OUT_CLOCK_RISING_EDGE
#define TMBSLTDA10024_CFG_OUT_SYNC_WIDTH     TMBSLTDA10024_CFG_OUT_SYNC_FIRST_BYTE
#define TMBSLTDA10024_CFG_OUT_DEN_POL     TMBSLTDA10024_CFG_OUT_DEN_NOT_INVERTED
#define TMBSLTDA10024_CFG_OUT_UNCOR_POL     TMBSLTDA10024_CFG_OUT_UNCOR_NOT_INVERTED
#define TMBSLTDA10024_CFG_OUT_SYNC_POL     TMBSLTDA10024_CFG_OUT_SYNC_NOT_INVERTED

/* AGC */
#define TMBSLTDA10024_CFG_AGC_THRESHOLD     0xFF
#define TMBSLTDA10024_CFG_AGC_1_POLA        TMBSLTDA10024_CFG_AGC_POLA_NORMAL
#define TMBSLTDA10024_CFG_AGC_2_POLA        TMBSLTDA10024_CFG_AGC_POLA_NORMAL
#define TMBSLTDA10024_CFG_AGC_TUNER_MAX     0xFF
#define TMBSLTDA10024_CFG_AGC_TUNER_MIN     0x00
#define TMBSLTDA10024_CFG_AGC_IF_MAX        0xFF
#define TMBSLTDA10024_CFG_AGC_IF_MIN        0x00

/* EQUALIZER */
#define TMBSLTDA10024_CFG_EQUAL_TYPE        0x01 /* 1 for linear transversal, 2 for dfe */
#define TMBSLTDA10024_CFG_BAND_TYPE         0x02 /* 1: band type other , 2: band type SECAM L */
#define TMBSLTDA10024_CFG_EQCONF1           0x70 

/* CARCONF */
#define TMBSLTDA10024_CFG_CAR_C             0x00 

/* GAIN1 */
#define TMBSLTDA10024_CFG_GAIN1             0x80

/* SWEEP */
#define TMBSLTDA10024_CFG_SWPOS             0x01 

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/

/******************************************************************************/
/** Configure the consumer parameters
 *
 *
 * @param ptmdlFrontEndCfg_S (In) : parameters structure
 *
 *
 ******************************************************************************/
extern tmbslTDA10024_Cfg_Err_t
tmbslTDA10024_CFG_GetConfig(
        tmbslTDA10024_Cfg_t*     pConfig
        );

#ifdef __cplusplus
}
#endif
#endif
/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/
