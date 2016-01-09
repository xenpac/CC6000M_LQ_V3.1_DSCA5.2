#include <stdio.h>
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "MaxLinearDataTypes.h"
#include "MxL608_TunerApi.h"
#include "MxL608_OEM_Drv.h"

#define EXAMPLE_DEV_MAX 2
//#define MXL603_INC_CURRENT

#define DBG_MXL   1
#if DBG_MXL
#define DBG_MXL_PRINT OS_PRINTF
#else
#define DBG_MXL_PRINT(...) do{}while(0);
#endif
static MXL608_CFG_T cfg;
void Mxl608_cfg(PMXL608_CFG_T p_cfg)
{
  if(p_cfg)
  {
    memcpy(&cfg, p_cfg, sizeof(MXL608_CFG_T));
  }
}
#ifdef MXL603_INC_CURRENT
/* Increase driving current for IFamp
 */
static MXL_STATUS MDrv_Tuner_IncCurrent(void)
{
    UINT8 status = MXL_SUCCESS;
    DBG_MXL_PRINT("MDrv_Tuner_IncCurrent\n");
    status |= MxLWare603_OEM_WriteRegister(MXL603_I2C_ADDR, 0x5B, 0x10);
    status |= MxLWare603_OEM_WriteRegister(MXL603_I2C_ADDR, 0x5C, 0xB1);

    return (MXL_STATUS) status;
}
#endif
/* Example of OEM Data, customers should have
below data structure declared at their appropriate 
places as per their software design 

typedef struct
{
  UINT8   i2c_address;
  UINT8   i2c_bus;
  sem_type_t sem;
  UINT16  i2c_cnt;
} user_data_t;  

user_data_t device_context[EXAMPLE_DEV_MAX];
*/

MXL_STATUS MXL603_INIT(void)
{
  MXL603_TUNER_MODE_CFG_T tunerModeCfg; //global parameter 
//  MXL603_CHAN_TUNE_CFG_T chanTuneCfg;//set to be global value
  MXL_STATUS status; 
  MXL_BOOL singleSupply_3_3V;
  MXL603_XTAL_SET_CFG_T xtalCfg;
  MXL603_IF_OUT_CFG_T ifOutCfg;
  MXL603_AGC_CFG_T agcCfg;
  //MXL603_TUNER_MODE_CFG_T tunerModeCfg; //set to be global value
  //MXL603_CHAN_TUNE_CFG_T chanTuneCfg;//set to be global value
 // MXL_BOOL refLockPtr;
 // MXL_BOOL rfLockPtr;
  MXL_BOOL loopThroughCtrl;
  UINT8 devId;
/* If OEM data is implemented, customer needs to use OEM data structure  
   related operation. Following code should be used as a reference. 
   For more information refer to sections 2.5 & 2.6 of 
   MxL603_mxLWare_API_UserGuide document.

  for (devId = 0; devId < EXAMPLE_DEV_MAX; devId++)
  {
    // assigning i2c address for  the device
    device_context[devId].i2c_address = GET_FROM_FILE_I2C_ADDRESS(devId);     

    // assigning i2c bus for  the device
    device_context[devId].i2c_bus = GET_FROM_FILE_I2C_BUS(devId);                      

    // create semaphore if necessary
    device_context[devId].sem = CREATE_SEM();                                                           

    // sample stat counter
    device_context[devId].i2c_cnt = 0;                                                                               

    status = MxLWare603_API_CfgDrvInit(devId, (void *) &device_context[devId]);  

    // if you don抰 want to pass any oem data, just use NULL as a parameter:
    // status = MxLWare603_API_CfgDrvInit(devId, NULL);  
  }

*/

  /* If OEM data is not required, customer should treat devId as 
     I2C slave Address */
  devId = MXL603_I2C_ADDR;
    
  //Step 1 : Soft Reset MxL603
  status = MxLWare603_API_CfgDevSoftReset(devId);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgDevSoftReset\n");    
  }
    
  //Step 2 : Overwrite Default
  singleSupply_3_3V = MXL_ENABLE;//MXL_DISABLE;
  status = MxLWare603_API_CfgDevOverwriteDefaults(devId, singleSupply_3_3V);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgDevOverwriteDefaults\n");    
  }

  //Step 3 : XTAL Setting 
  #if 0
  /* Dual XTAL for tuner and demod separately*/
  xtalCfg.xtalFreqSel = MXL603_XTAL_16MHz;
  xtalCfg.xtalCap = 12;  //匹配电容和晶振型号相关
  xtalCfg.clkOutEnable = MXL_DISABLE;
  #else
  /* Single XTAL for tuner and demod sharing*/
  xtalCfg.xtalFreqSel = MXL603_XTAL_24MHz;
  xtalCfg.xtalCap = 25;  //匹配电容和晶振型号相关
  xtalCfg.clkOutEnable = MXL_ENABLE;
  #endif 
  xtalCfg.clkOutDiv = MXL_DISABLE;
  xtalCfg.clkOutExt = MXL_DISABLE;

	/*Parameter : singleSupply_3_3V
	    - Enable :  Single power supply to Tuner (3.3v only;  3.3v -> 1.8V tuner internally inverts ) 
	    - Disable : Dual power supply to Tuner (3.3v+1.8v; internal regulator be bypassed) 
	★ If set wrongly toward hardware layout, Tuner will loose control of AGC(at least) 
	*/
  xtalCfg.singleSupply_3_3V = cfg.xtalCfg.singleSupply_3_3V;//MXL_DISABLE;
  
  xtalCfg.XtalSharingMode = MXL_DISABLE;
  status = MxLWare603_API_CfgDevXtal(devId, xtalCfg);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgDevXtal\n");    
  }

  //Step 4 : IF Out setting
  //IF freq set, should match Demod request 
  ifOutCfg.ifOutFreq = MXL603_IF_5MHz;
  
  ifOutCfg.ifInversion = MXL_DISABLE;
  ifOutCfg.gainLevel = 11;
  ifOutCfg.manualFreqSet = MXL_DISABLE;
  ifOutCfg.manualIFOutFreqInKHz = 0;
  status = MxLWare603_API_CfgTunerIFOutParam(devId, ifOutCfg);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgTunerIFOutParam\n");    
  }

  //Step 5 : AGC Setting
  //agcCfg.agcType = MXL603_AGC_EXTERNAL;
  agcCfg.agcType = cfg.agcCfg.agcType;// MXL603_AGC_SELF; //if you doubt DMD IF-AGC part, pls. use Tuner self AGC instead.

  agcCfg.setPoint = 66;
  agcCfg.agcPolarityInverstion = MXL_DISABLE;
  status = MxLWare603_API_CfgTunerAGC(devId, agcCfg);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgTunerAGC\n");    
  }

  //Step 6 : Application Mode setting
  tunerModeCfg.signalMode = MXL603_DIG_DVB_T_DTMB;//MXL603_DIG_ISDBT_ATSC;
  
   //IF freq set, should match Demod request 
  tunerModeCfg.ifOutFreqinKHz = 5000;
   
#ifdef MXL603_INC_CURRENT

    status = MDrv_Tuner_IncCurrent();
    if (status != MXL_SUCCESS)
    {
        DBG_MXL_PRINT("Error! MDrv_Tuner_IncCurrent\n");
        return status;
    }
#endif  
  #if 0
  /* Dual XTAL for tuner and demod separately*/
  tunerModeCfg.xtalFreqSel = MXL603_XTAL_16MHz;
  #else
   /* Single XTAL for tuner and demod sharing*/
   tunerModeCfg.xtalFreqSel = MXL603_XTAL_24MHz;
  #endif
  
  tunerModeCfg.ifOutGainLevel = 11;
  status = MxLWare603_API_CfgTunerMode(devId, tunerModeCfg);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("##### Error! pls. make sure return value no problem, otherwise, it will cause Tuner unable to unlock signal #####\n");   
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgTunerMode\n");    
  }
  
  //Step : Loop Through setting
  loopThroughCtrl = MXL_DISABLE;
  status = MxLWare603_API_CfgTunerLoopThrough(devId, loopThroughCtrl,NULL,NULL);


#if 0
  //Step 7 : Channel frequency & bandwidth setting
  chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
  chanTuneCfg.freqInHz = 666000000;
  chanTuneCfg.signalMode = MXL603_DIG_DVB_T_DTMB;
  
    #if 0
  /* Dual XTAL for tuner and demod separately*/
  chanTuneCfg.xtalFreqSel = MXL603_XTAL_16MHz;
    #else
   /* Single XTAL for tuner and demod sharing*/
    chanTuneCfg.xtalFreqSel = MXL603_XTAL_24MHz;
	#endif 
	
  chanTuneCfg.startTune = MXL_START_TUNE;
  status = MxLWare603_API_CfgTunerChanTune(devId, chanTuneCfg);
  if (status != MXL_SUCCESS)
  {
    DMD_DEBUG("Error! MxLWare603_API_CfgTunerChanTune\n");    
  }


  // Wait 15 ms 
  MxLWare603_OEM_Sleep(15);

  // Read back Tuner lock status : no need in initialization . 
  status = MxLWare603_API_ReqTunerLockStatus(devId, &rfLockPtr, &refLockPtr);
  if (status == MXL_TRUE)
  {
    if (MXL_LOCKED == rfLockPtr && MXL_LOCKED == refLockPtr)
    {
      DMD_DEBUG("Tuner locked\n");
    }
    else
      DMD_DEBUG("Tuner unlocked\n");
  }

  // To Change Channel, GOTO Step #7

  // To change Application mode settings, GOTO Step #6
#endif
  return status;
}


MXL_STATUS Mxl603SetSystemMode(void)
{
#if 0
  MXL_STATUS status; 
  UINT8 devId;
  MXL603_TUNER_MODE_CFG_T tunerModeCfg;
  devId = MXL603_I2C_ADDR;  								 

  //Step 6 : Application Mode setting
  tunerModeCfg.signalMode = MXL603_DIG_DVB_T_DTMB;//MXL603_DIG_ISDBT_ATSC;
  tunerModeCfg.ifOutFreqinKHz = 5000;
  tunerModeCfg.ifOutGainLevel = 11;

  //  tunerModeCfg is global struct. 
  status = MxLWare603_API_CfgTunerMode(devId, tunerModeCfg);
  if (status != MXL_SUCCESS)
  {
    DBG_MXL_PRINT("Error! MxLWare603_API_CfgTunerMode\n");    
  }
#endif
  return MXL_SUCCESS;

}  

MXL_STATUS Mxl603SetFreqBw(MS_U32 dwFreq /*Khz*/, MS_U8 ucBw /*MHz*/)
{
    MXL_STATUS status;
    MXL603_BW_E eBw = ucBw - 6 ;
    MXL603_CHAN_TUNE_CFG_T chanTuneCfg;
    MXL_BOOL refLockPtr = MXL_UNLOCKED;
    MXL_BOOL rfLockPtr = MXL_UNLOCKED;
    UINT8 regData = 0;

    chanTuneCfg.signalMode = MXL603_DIG_DVB_T_DTMB;

    switch (chanTuneCfg.signalMode)
    {
    case MXL603_DIG_DVB_C:
        switch (eBw)
        {
        case MXL603_CABLE_BW_6MHz:
        case MXL603_CABLE_BW_7MHz:
        case MXL603_CABLE_BW_8MHz:
            chanTuneCfg.bandWidth = eBw;
            break;
        default:
            chanTuneCfg.bandWidth = MXL603_CABLE_BW_8MHz;
        }
        break;
    case MXL603_DIG_DVB_T_DTMB:
        eBw = eBw + MXL603_TERR_BW_6MHz;
        switch (eBw)
        {
        case MXL603_TERR_BW_6MHz:
        case MXL603_TERR_BW_7MHz:
        case MXL603_TERR_BW_8MHz:
            chanTuneCfg.bandWidth = eBw;
            break;
        default:
            chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
        }
        break;
    case MXL603_DIG_ISDBT_ATSC:
        eBw = eBw + MXL603_TERR_BW_6MHz;
        switch (eBw)
        {
        case MXL603_TERR_BW_6MHz:
        case MXL603_TERR_BW_7MHz:
        case MXL603_TERR_BW_8MHz:
            chanTuneCfg.bandWidth = eBw;
            break;
        default:
            chanTuneCfg.bandWidth = MXL603_TERR_BW_6MHz;
        }
        break;
    default:
        chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
        break;
    }
#ifdef MXL603_INC_CURRENT
    status = MDrv_Tuner_IncCurrent();
    if (status != MXL_SUCCESS)
    {
        DBG_MXL_PRINT("Error! MDrv_Tuner_IncCurrent\n");
        //return status;
    }
#endif
    chanTuneCfg.freqInHz = dwFreq;// * 1000;
    chanTuneCfg.xtalFreqSel = MXL603_XTAL_24MHz;
    chanTuneCfg.startTune = MXL_START_TUNE;
    status = MxLWare603_API_CfgTunerChanTune(MXL603_I2C_ADDR, chanTuneCfg);
    if (status != MXL_SUCCESS)
    {
        DBG_MXL_PRINT("Error! MxLWare603_API_CfgTunerChanTune\n");
    }

    // Wait 15 ms
    MxLWare603_OEM_Sleep(15);

    // Read back Tuner lock status
    status = MxLWare603_API_ReqTunerLockStatus(MXL603_I2C_ADDR, &rfLockPtr, &refLockPtr);
    if (status == MXL_TRUE)
    {
        if ((MXL_LOCKED == rfLockPtr) && (MXL_LOCKED == refLockPtr))
        {
            DBG_MXL_PRINT("Tuner locked\n");

            MxLWare603_OEM_ReadRegister(MXL603_I2C_ADDR, 0x5B, &regData);
            DBG_MXL_PRINT("MxL603_0x5B : %x\n", regData);
            MxLWare603_OEM_ReadRegister(MXL603_I2C_ADDR, 0x5C, &regData);
            DBG_MXL_PRINT("MxL603_0x5C : %x\n", regData);
        }
        else
            DBG_MXL_PRINT("Tuner unlocked\n");
    }

    return status;
}
#if 0
MXL_STATUS Mxl603SetFreqBw(void)
{
  MXL_STATUS status; 
    UINT8 devId=0;
    UINT8 count=0;
  MXL_BOOL refLockPtr;
  MXL_BOOL rfLockPtr;
 
   devId = MXL603_I2C_ADDR;
 
  //Step 7 : Channel frequency & bandwidth setting
 /*
  chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
  chanTuneCfg.freqInHz = 666000000;
  chanTuneCfg.signalMode = MXL603_DIG_DVB_T_DTMB;
  chanTuneCfg.startTune = MXL_START_TUNE;
 */

   //chanTuneCfgis global struct. 
  status = MxLWare603_API_CfgTunerChanTune(devId, chanTuneCfg);
  if (status != MXL_SUCCESS)
  { 
     status = MXL_FALSE;
    DMD_DEBUG("Error! MxLWare603_API_CfgTunerChanTune\n");    
	  return status;
  }
  
  // Wait 15 ms 
  MxLWare603_OEM_Sleep(15);
   status = MXL_FALSE;
   
	do  /* 20130121, Troy.wang added, wait Tuner to output signal stable */
	{
	  // Read back Tuner lock status
	  if (MxLWare603_API_ReqTunerLockStatus(devId, &rfLockPtr, &refLockPtr) == MXL_TRUE)
	  {
	    if (MXL_LOCKED == rfLockPtr && MXL_LOCKED == refLockPtr)
	    {
	      DMD_DEBUG("Tuner locked\n"); //If system runs into here, it mean that Tuner locked and output IF stable!!
           status = MXL_TRUE;
		    break;
	    } 	
	  }
	  MxLWare603_OEM_Sleep(10); //wait 10ms
	  count++;
	}while(count < 30); //timeout 300ms 
 
  return status; 
}
#endif
