/* **************************************************** */
/*!
   @file	MN_DMD_Tuner_template.c
   @brief	Tuner API wrapper (sample)
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */
//---------- this is sample Tuner API  ----------------
#include "MN_DMD_driver.h"
#include "MN_DMD_common.h"
#if (TUNER_TYPE == MXL603)   
#include "MxL608_TunerApi.h"
#include "MaxLinearDataTypes.h"
#elif (TUNER_TYPE == NXP18250B)
// NXP Architecture includes
#include "tmCompId.h"
#include "tmNxtypes.h"
#include "tmFrontend.h"
#include "tmbslFrontEndCfgItem.h"
#include "tmbslFrontEndTypes.h"

// Components includes
#include "tmbslTDA18250A.h"
#else
#endif
/* **************************************************** */
/* **************************************************** */

/*! Tuner Initialize Function*/
DMD_ERROR_t DMD_API DMD_Tuner_init( )
{
	//TODO:	Please call tuner initialize API here

	DMD_ERROR_t ret = DMD_E_ERROR ;
#if (TUNER_TYPE == MXL603)   
extern MXL_STATUS MXL603_INIT(void);

	ret =  MXL603_INIT( );	
#elif (TUNER_TYPE == NXP18250B)
	ret = tmbsliTDA18250A_Init( );	
#else	
#endif
     if (ret != DMD_E_OK)
    {
      DMD_DBG_TRACE("Error! DMD_Tuner_init\n");    
     }
 
	//this function is called by DMD_init
	
	//DMD_DBG_TRACE("Tuner is not implemeted\n");
	return ret ;
}

/*! Tuner Tune Function */
DMD_ERROR_t DMD_API DMD_Tuner_tune( DMD_PARAMETER_t *param)
{
	//TODO:	Please call tune  API here
	DMD_ERROR_t ret = DMD_E_OK;
	//DMD_u32_t rf_in_hz = param->freq; 
  DMD_u8_t bw = 0;
  DMD_u32_t in_bw = param->bw;
	#if (TUNER_TYPE == MXL603)   
  switch(in_bw)
  {
  case 	DMD_E_BW_6MHZ:
    bw = 6;
    break;
  case 	DMD_E_BW_7MHZ:
    bw = 7;
    break;
	case DMD_E_BW_8MHZ:
    bw = 8;
    break;
	case DMD_E_BW_5MHZ:
    bw = 5;
    break;
  }
extern  MXL_STATUS Mxl603SetFreqBw(MS_U32 dwFreq, MS_U8 ucBw);

	ret = Mxl603SetFreqBw(param->freq, bw); //global parameter elsewhere 
	#elif (TUNER_TYPE == NXP18250B)
	ret = tmbslTDA18250A_SetRF(  rf_in_hz);
	#else
  #endif
     if (ret != DMD_E_OK)
      {
    		DMD_DBG_TRACE(" Pls. retune - until tuner output stable .\n");    
    		ret = DMD_E_ERROR;
     }
	 
	//this function is called by DMD_tune
	//DMD_DBG_TRACE("Tuner is not implemeted\n");
	return ret;
}

/*! Tuner Termination Function */
DMD_ERROR_t DMD_API DMD_Tuner_term( )
{
	//TODO:	Please call tune  API here
	//this function is called by DMD_term
	DMD_DBG_TRACE("Tuner is not implemeted\n");
	return DMD_E_OK;
}

/*! Tuner Tune Function */
DMD_ERROR_t DMD_API DMD_Tuner_set_system( DMD_PARAMETER_t *param)
{
	//TODO:	Please call tune  API here
	DMD_ERROR_t ret = DMD_E_ERROR;
#if (TUNER_TYPE == MXL603)   
extern MXL_STATUS Mxl603SetSystemMode(void);
	ret = Mxl603SetSystemMode( );//global parameter elsewhere 
#elif (TUNER_TYPE == NXP18250B)
	DMD_BANDWIDTH_t dmd_bw = param->bw;
	TDA18250AStandardMode_t tuner_bw;
    if (( param->system == DMD_E_DVBT2)||(param->system == DMD_E_DVBT))
   {		
        switch(dmd_bw)
		{
		case DMD_E_BW_6MHZ:           
				tuner_bw = TDA18250A_DVBT_6MHz;
		        break;
		case DMD_E_BW_7MHZ:          
				tuner_bw = TDA18250A_DVBT_7MHz;
		        break;      
		case DMD_E_BW_8MHZ:            
				tuner_bw = TDA18250A_DVBT_8MHz;
		        break;
		case DMD_E_BW_5MHZ:
		case DMD_E_BW_1_7MHZ:
		default:  
       DMD_DBG_TRACE("--- UNSUPPORT BANDWIDTH!!---\n");
				  return DMD_E_ERROR;
		}
    }
	else if( param->system == DMD_E_DVBC)
		{
		    tuner_bw = TDA18250A_QAM_8MHz;
		}
	else
		{
			DMD_DBG_TRACE("--- UNSUPPORT SYSTEM!!---\n");
			return DMD_E_ERROR;
		}
    ret = tmbslTDA18250A_SetStandardMode(  tuner_bw);
#else
#endif
     if (ret != DMD_E_OK)
    {
      DMD_DBG_TRACE("Error! DMD_Tuner_set_system \n");    
     }	
	
	//this function is called by DMD_tune
	//DMD_DBG_TRACE("Tuner is not implemeted\n");
	return ret ;
}
void Tuner_check_RF_input_power(int* rxPwrPtr)
{
#if (TUNER_TYPE == MXL603)  
	DMD_u8_t devId;
	devId = MXL603_I2C_ADDR;
  extern MXL_STATUS MxLWare603_API_ReqTunerRxPower(UINT8 devId, SINT16* rxPwrPtr);
	MxLWare603_API_ReqTunerRxPower(devId, (SINT16*)rxPwrPtr); 
#elif (TUNER_TYPE == NXP18250B)
	tmbslTDA18250A_GetPowerLevel(rxPwrPtr);
#else
#endif 
}
