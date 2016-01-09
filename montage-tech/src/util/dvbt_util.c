/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "drv_dev.h"
#include "nim.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"

#define AUSTRALIA_FREQTBL_NEW 0
#if AUSTRALIA_FREQTBL_NEW
FreqTblType FreqTbl[FREQ_TAL_SIZE]  =
{
  //must sort by channel no.  from small to large
  //if the channel is invalid,set the frequency 0
  //No. offset ----->for displaying 0:= No. ; +X:No.-X and add "A"  ; -X:No.-X
  {6,177500,0}, // 6~9
  {10,205500,1}, // 9A
  {11,212500,-1}, // 10~12
  {14,0,-1}, //13~20
  {21,480500,0}, // 21~27
  {28,527500,1}, // 27A
  {29,529500,-1}, // 28~75
};
#endif

//--------------------------------------------------------------------------------------------------
/// Get RF Channel parameters
/// @param  u8RF_CH                 \b IN:  RF channel number
/// @param  u8Country               \b IN:  country
/// @param  u16BandWidth            \b IN:  channel bandwidth (kHz)
/// @param  u32Frequency            \b IN:  the frequency of the RF channel
/// @return TRUE: get the corresponding frequency
///         FALSE: failed to get the frequency.
//--------------------------------------------------------------------------------------------------
BOOL dvbt_get_tp_setting(u8 u8RF_CH, u8 u8Country, u16 *u16BandWidth, u32 *u32Frequency)
{
  u8 u8MaxRFCh = dvbt_get_max_rf_ch_num(u8Country);
  u8 u8MinRFCh = dvbt_get_min_rf_ch_num(u8Country);
  if(u8RF_CH < u8MinRFCh || u8RF_CH > u8MaxRFCh)
  {
    *u32Frequency = 0;
    return FALSE;
  }

  switch(u8Country)
  {
    case COUNTRY_TYPE_COLOMBIA:
      if ((u8RF_CH >= 2) && (u8RF_CH <= 6))   //54.000 ~ 82.000 Mhz
      {
        *u32Frequency = 54000L + (u8RF_CH - 2) * 6000L;
        *u16BandWidth = 6000;
      }
      else if((u8RF_CH >= 7) && (u8RF_CH <= 13))//174.000~210.000 Mhz
      {
        *u32Frequency = 174000L + (u8RF_CH - 7) * 6000L;
        *u16BandWidth = 6000;
      }
      else
      {
        *u32Frequency = 470000L + (u8RF_CH - 14) * 6000L;//470.000~800.000 Mhz
        *u16BandWidth = 6000;
      }
      break;

    case COUNTRY_TYPE_CHILE:
      if ((u8RF_CH >= 14) && (u8RF_CH <= 79))   //473.000 ~ 803.000 Mhz
      {
        *u32Frequency = 473000L + (u8RF_CH - 14) * 6000L;
        *u16BandWidth = 6000;
      }
      else if((u8RF_CH >= 7) && (u8RF_CH <= 13))//177.000~473.000 Mhz
      {
        *u32Frequency = 177000L + (u8RF_CH - 7) * 6000L;
        *u16BandWidth = 6000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;

    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
      if ((u8RF_CH >= 14) && (u8RF_CH <= 69))   //473.143 ~ 803.143 Mhz
      {
        *u32Frequency = 473143L + (u8RF_CH - 14) * 6000L;
        *u16BandWidth = 6000;
      }
      else if((u8RF_CH >= 7) && (u8RF_CH <= 13))//177.143~473.143 Mhz
      {
        *u32Frequency = 177143L + (u8RF_CH - 7) * 6000L;
        *u16BandWidth = 6000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;
      
    case COUNTRY_TYPE_ARGENTINA:
      if ((u8RF_CH ==14))
      {
        *u32Frequency = 527000;
        *u16BandWidth = 6000;
      }
      else
      {
        *u32Frequency = 647000;
        *u16BandWidth = 6000;
      }
      break;

    case COUNTRY_TYPE_UK:
    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND: 
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_CHINA:
    case COUNTRY_TYPE_RUSSIAN:
    case COUNTRY_TYPE_GHANA:
  	 case COUNTRY_TYPE_GREEK:       //
	 case COUNTRY_TYPE_SLOVENIAN:
	 case COUNTRY_TYPE_SPAIN:
	 case COUNTRY_TYPE_DENMARK:
	 case COUNTRY_TYPE_FINLAND:
	 case COUNTRY_TYPE_SWEDEN:   
      if ((u8RF_CH >= 5) && (u8RF_CH <= 12))   //177.5 ~ 226.5 Mhz
      {
        *u32Frequency = 177500L + (u8RF_CH - 5) * 7000L;
        *u16BandWidth = 7000;
      }
      else if ((u8RF_CH >= 21) && (u8RF_CH <= 70))   //474 ~ 858 Mhz
      {
        *u32Frequency = 474000L + (u8RF_CH - 21) * 8000L;
        *u16BandWidth = 8000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;

    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_FRANCE:
      if ((u8RF_CH >= 5) && (u8RF_CH <= 12))   //177.5 ~ 226.5 Mhz
      {
        *u32Frequency = 177500L + (u8RF_CH - 5) * 7000L;
        *u16BandWidth = 7000;
      }
      else if ((u8RF_CH >= 21) && (u8RF_CH <= 70))   //474 ~ 858 Mhz
      {
        *u32Frequency = 474000L + (u8RF_CH - 21) * 8000L;
        *u16BandWidth = 8000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;

    case COUNTRY_TYPE_AUSTRALIA:
      {
#if AUSTRALIA_FREQTBL_NEW
        s8 s8Index = FREQ_TAL_SIZE -1;

        for(s8Index = FREQ_TAL_SIZE -1;s8Index >= 0;s8Index--)
        {
          if(u8RF_CH >= FreqTbl[s8Index].u8CHNo)
          {
            break;
          }
        }
        if(s8Index >= 0 && FreqTbl[s8Index].u32Freq > 0)
        {
          *u32Frequency = FreqTbl[s8Index].u32Freq + (u8RF_CH - FreqTbl[s8Index].u8CHNo) * 7000L;
        }
        else
        {
          *u32Frequency = 0;
        }
#else
        if ((u8RF_CH >= 6) && (u8RF_CH <= 13))   //177.5 ~ 226.5 Mhz  6~9 9A~12
        {
          *u32Frequency = 177500L + (u8RF_CH - 6) * 7000L;
        }
        else if ((u8RF_CH >= 27) && (u8RF_CH <= 71))   //474 ~ 858 Mhz
        {
          *u32Frequency = 522500L + (u8RF_CH - 27) * 7000L;
        }
        else
        {
          *u32Frequency = 0;
        }
#endif
        *u16BandWidth = 7000;
      }
      break;

    case COUNTRY_TYPE_ITALY:
      {
        u32 u32FreqTbl[]={177500,186000,194500,203500,212500,219500,226500,233500}; // 5 ~ 12

        if ((u8RF_CH >= 5) && (u8RF_CH <= 12))   //177.5 ~ 226.5 Mhz  6~9 9A~12
        {
          *u32Frequency = u32FreqTbl[u8RF_CH - 5];
          *u16BandWidth = 7000;
        }
        else if ((u8RF_CH >= 21) && (u8RF_CH <= 70))   //474 ~ 858 Mhz
        {
          *u32Frequency = 474000L + (u8RF_CH - 21) * 8000L;
          *u16BandWidth = 8000;
        }
        else
        {
          *u32Frequency = 0;
        }
      }
      break;

  case COUNTRY_TYPE_TAIWAN:
      if ((u8RF_CH >= 24) && (u8RF_CH <= 36))   //533 ~ 605 Mhz
      {
        *u32Frequency = 533000L + (u8RF_CH - 24) * 6000L;
        *u16BandWidth = 6000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;

  case COUNTRY_TYPE_VIETNAM:
      if ((u8RF_CH >= 6) && (u8RF_CH <= 12))   //178 ~ 226 Mhz
      {
        *u32Frequency = 178000L + (u8RF_CH - 6) * 8000L;
        *u16BandWidth = 8000;
      }
      else if ((u8RF_CH >= 21) && (u8RF_CH <= 60))   //474 ~ 786 Mhz
      {
        *u32Frequency = 474000L + (u8RF_CH - 21) * 8000L;
        *u16BandWidth = 8000;
      }
      else
      {
        *u32Frequency = 0;
      }
      break;

    case COUNTRY_TYPE_NIGERIA:
      {
        if ((u8RF_CH >= 6) && (u8RF_CH <= 13))   //177.5 ~ 226.5 Mhz  6~9 9A~12
        {
          *u32Frequency = 177500L + (u8RF_CH - 6) * 7000L;
          *u16BandWidth = 7000;
        }
        else if ((u8RF_CH >= 21) && (u8RF_CH <= 71))   
        {
          *u32Frequency = 474000L + (u8RF_CH - 21) * 8000L;
          *u16BandWidth = 8000;
        }        
        else
        {
          *u32Frequency = 0;
        }
      }
      break;
   case COUNTRY_TYPE_AFRICA:
    if ((u8RF_CH >= 1) && (u8RF_CH <= 10))   //653 ~ 707 Mhz  1~10
    {
      *u32Frequency = 653000L + (u8RF_CH - 1) * 6000L;
      *u16BandWidth = 6000;
    }
    break;
  break;

  case COUNTRY_TYPE_DJIBOUTI:
      if ((u8RF_CH >= 4) && (u8RF_CH <= 13))   //167~230MHz
      {
        *u32Frequency = 167000L + (u8RF_CH - 4) * 7000L;
        *u16BandWidth = 7000;
      }
      else
      {
        *u32Frequency = 0;
      }
      
  break;

  case COUNTRY_TYPE_MONGOLIA:
      if ((u8RF_CH >= 1) && (u8RF_CH <= 14))   //582~686MHz
      {
        *u32Frequency = 582000L + (u8RF_CH - 1) * 8000L;
        *u16BandWidth = 8000;
      }
      else
      {
        *u32Frequency = 0;
      }
  break;

	 case COUNTRY_TYPE_POLAND:
	   	if ((u8RF_CH >= 6) && (u8RF_CH <= 12))	 //171 ~ 219 Mhz
	   	{
	   	  *u32Frequency = 171000L + (u8RF_CH - 6) * 8000L;
	   	  *u16BandWidth = 8000;
	   	}
	   	else if ((u8RF_CH >= 13) && (u8RF_CH <= 61))   //474 ~ 858 Mhz
	   	{
	   	  *u32Frequency = 474000L + (u8RF_CH - 13) * 8000L;
	   	  *u16BandWidth = 8000;
	   	}
	   	else
	   	{
	   	  *u32Frequency = 0;
	   	}
	   	break;

  case COUNTRY_TYPE_OTHER:
    *u32Frequency  = 98000 + (u8RF_CH * 8000L);
    *u16BandWidth = 8000;
    break;

  default:
    *u32Frequency = 0;
    break;
  }

  /* Frequency and symbol rate already verified in Scan Menu.
     0 = default value. */
  if (*u32Frequency != 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


//--------------------------------------------------------------------------------------------------
/// Get RF channel number by frequency
/// @param  u8Country               \b IN:  country
/// @param  u32Frequency            \b IN:  the frequency of the RF channel
/// @return RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_freq2channelnumber(u8 u8Country, u32 u32Frequency)
{
  u8  u8RF_CH = 0;

  switch(u8Country)
  {
    default:
      u8RF_CH = 0;
      break;

    case COUNTRY_TYPE_COLOMBIA:
      if (u32Frequency <= 78000L)
      {           // 54 ~ 78
        u8RF_CH = (u8)((u32Frequency - 54000L) / 6000L) + 2;
      }
      else if((u32Frequency >= 174000L)&&(u32Frequency <= 210000L))
      {         // 174 ~ 210
        u8RF_CH = (u8)((u32Frequency - 174000L) / 6000L) + 7;
      }
      else
      {         //470 ~ 800
        u8RF_CH = (u8)((u32Frequency - 470000L) / 6000L) + 14;
      }
      break;

    case COUNTRY_TYPE_CHILE:
      if (u32Frequency < 473000L)
      {           // 177.143 ~ 473.143
        u8RF_CH = (u8)((u32Frequency - 177000L) / 6000L) + 7;
      }
      else
      {
        u8RF_CH = (u8)((u32Frequency - 473000L) / 6000L) + 14;
      }
      break;

    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
      if (u32Frequency < 473143L)
      {           // 177.143 ~ 473.143
        u8RF_CH = (u8)((u32Frequency - 177143L) / 6000L) + 7;
      }
      else
      {
        u8RF_CH = (u8)((u32Frequency - 473143L) / 6000L) + 14;
      }
      break;

    case COUNTRY_TYPE_ARGENTINA:
      if (u32Frequency ==527000)
      {           // 177.143 ~ 473.143
          u8RF_CH = 14;
      }
      else
      {
          u8RF_CH = 15;
      }
      break;

    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND:  
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_UK:
    case COUNTRY_TYPE_CHINA:
    case COUNTRY_TYPE_RUSSIAN: 
    case COUNTRY_TYPE_GHANA:
    case COUNTRY_TYPE_GREEK:            //
    case COUNTRY_TYPE_SLOVENIAN:
    case COUNTRY_TYPE_SPAIN:
    case COUNTRY_TYPE_DENMARK:
    case COUNTRY_TYPE_FINLAND:
    case COUNTRY_TYPE_SWEDEN:    
      if (u32Frequency < 474000L)
      {           // 177.5 ~ 226.5
        u8RF_CH = (u8)((u32Frequency - 177500L) / 7000L) + 5;
      }
      else
      {
        u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 21;
      }
      break;

    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_FRANCE:
      if (u32Frequency < 474000L)
      {           // 177.5 ~ 226.5
        u8RF_CH = (u8)((u32Frequency - 177500L) / 7000L) + 5;
      }
      else
      {
        u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 21;
      }
      break;

    case COUNTRY_TYPE_ITALY:
      {
        u32 u32FreqTbl[]={177500,186000,194500,203500,212500,219500,226500,233500}; // 5 ~ 12
        u16 i = 0;
        if(u32Frequency < 474000L)
        {
          for(i = 0;i < sizeof(u32FreqTbl) / sizeof(u32);i++)
          {
            if(u32Frequency <= u32FreqTbl[i])
            {
              u8RF_CH = 5 + i;
              break;
            }
          }
        }
        else
        {
          u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 21;
        }
        break;
      }

    case COUNTRY_TYPE_AUSTRALIA:
    {
      #if AUSTRALIA_FREQTBL_NEW
        s8 s8Index = FREQ_TAL_SIZE -1;

        for(s8Index = FREQ_TAL_SIZE - 1;s8Index >= 0;s8Index--)
        {
          if(FreqTbl[s8Index].u32Freq > 0 && u32Frequency >= FreqTbl[s8Index].u32Freq)
          {
            break;
          }
        }
        if(s8Index >= 0)
        {
          u8RF_CH = (u8)((u32Frequency - FreqTbl[s8Index].u32Freq) / 7000L)
            + FreqTbl[s8Index].u8CHNo;
        }
        else
        {
          u8RF_CH = FreqTbl[0].u8CHNo;
        }
      #else
        if (u32Frequency < 522500L)
        {           // 177.5 ~ 226.5
          u8RF_CH = (u8)((u32Frequency - 177500L) / 7000L) + 6;
        }
        else
        {
          u8RF_CH = (u8)((u32Frequency - 522500L) / 7000L) + 27;
        }
      #endif
        break;
    }

    case COUNTRY_TYPE_TAIWAN:
    {
      //if (u8RF_CH >= 24 && u8RF_CH <= 36)
      if (u32Frequency >= 533000L && u32Frequency <= 605000L)
      {            //533 ~ 605 Mhz
        u8RF_CH = (u8)((u32Frequency - 533000L) / 6000L) + 24;
      }
      break;
    }

    case COUNTRY_TYPE_VIETNAM:
    {
      if (u32Frequency < 474000L)
      {            //178 ~ 226 Mhz
        u8RF_CH = (u8)((u32Frequency - 178000L) / 8000L) + 6;
      }
      else
      {
        u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 21;
      }
      break;
    }

    case COUNTRY_TYPE_NIGERIA:
      if (u32Frequency >= 177500L && u32Frequency <= 226500L)
      {
        u8RF_CH = (u8)((u32Frequency - 177500L) / 7000L) + 6;
      }
      else if (u32Frequency >= 474000L && u32Frequency <=858000L)
      {
        u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 21;
      }
      break;
      
    case COUNTRY_TYPE_AFRICA:
      u8RF_CH = (u8)((u32Frequency - 653000L) / 6000L) + 1;
      break;    

    case COUNTRY_TYPE_DJIBOUTI:
      if (u32Frequency >= 167000L && u32Frequency <= 230000L)
      {            //167 ~ 230 Mhz
        u8RF_CH = (u8)((u32Frequency - 167000L) / 7000L) + 4;
      } 
      
    break;

    case COUNTRY_TYPE_MONGOLIA:
      if (u32Frequency >= 582000L && u32Frequency <= 686000L)
      {            //582 ~ 686 Mhz
        u8RF_CH = (u8)((u32Frequency - 582000L) / 8000L) + 1;
      } 
    break;

   case COUNTRY_TYPE_POLAND:
      if (u32Frequency >= 171000L && u32Frequency <= 219000L)
      {
        u8RF_CH = (u8)((u32Frequency - 171000L) / 8000L) + 6;
      }
      else if (u32Frequency >= 474000L && u32Frequency <=858000L)
      {
        u8RF_CH = (u8)((u32Frequency - 474000L) / 8000L) + 13;
      }   
   break;

    case COUNTRY_TYPE_OTHER:
      u8RF_CH = (u8)((u32Frequency - 98000) / 8000);
      break;
  }

  return u8RF_CH;
}

//--------------------------------------------------------------------------------------------------
/// Get highest RF channel number by country
/// @param  u8Country               \b IN:  country
/// @return the highest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_max_rf_ch_num(u8 u8Country)
{
  switch(u8Country)
  {
    case COUNTRY_TYPE_CHILE:
      return 79;
    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
    case COUNTRY_TYPE_COLOMBIA:
      return 69;
    case COUNTRY_TYPE_ARGENTINA:
      return 15;
    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_CHINA:
      return 69;
    case COUNTRY_TYPE_RUSSIAN:
    case COUNTRY_TYPE_UK:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND:  
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_ITALY:
    case COUNTRY_TYPE_FRANCE:
    case COUNTRY_TYPE_NIGERIA:
    case COUNTRY_TYPE_GHANA:     
    case COUNTRY_TYPE_GREEK:     //
    case COUNTRY_TYPE_SLOVENIAN:
    case COUNTRY_TYPE_SPAIN:
    case COUNTRY_TYPE_DENMARK:
    case COUNTRY_TYPE_FINLAND:
    case COUNTRY_TYPE_SWEDEN:
      return 70;
    case COUNTRY_TYPE_AUSTRALIA:
#if AUSTRALIA_FREQTBL_NEW
      return 76;
#else
      return 71;
#endif

    case COUNTRY_TYPE_TAIWAN:
      return 35;

    case COUNTRY_TYPE_VIETNAM:
      return 60;
    case COUNTRY_TYPE_AFRICA:
      return 10;

    case COUNTRY_TYPE_DJIBOUTI:
     return 13;
   case COUNTRY_TYPE_MONGOLIA:
     return 14;     

   case COUNTRY_TYPE_POLAND:
     return 61;
      
    case COUNTRY_TYPE_OTHER:
      return 100 ;

    default:
      return 0;
  }
}

//--------------------------------------------------------------------------------------------------
/// Get lowest RF channel number by country
/// @param  u8Country               \b IN:  country
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_min_rf_ch_num(u8 u8Country)
{
  switch(u8Country)
  {
    case COUNTRY_TYPE_CHILE:
    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
      return 7;
    case COUNTRY_TYPE_ARGENTINA:
      return 14;
#if (MARKET == MARKET_HONGKONG || MARKET == MARKET_CHINA)
    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_CHINA:
      return 21;
#endif
    case COUNTRY_TYPE_UK:
      return 21;
    case COUNTRY_TYPE_RUSSIAN:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND: 
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_ITALY:
    case COUNTRY_TYPE_FRANCE:
    case COUNTRY_TYPE_GREEK:     //
    case COUNTRY_TYPE_SLOVENIAN:
    case COUNTRY_TYPE_SPAIN:
    case COUNTRY_TYPE_DENMARK:
    case COUNTRY_TYPE_FINLAND:
    case COUNTRY_TYPE_SWEDEN:    
    {
#if (DVB_VHF_ENABLE == 1)
      return 5;
#else
      return 21;
#endif
    }
    case COUNTRY_TYPE_AUSTRALIA:
    {
#if (DVB_VHF_ENABLE == 1)
      return 6;
#else
      return 21;
#endif
    }
    case COUNTRY_TYPE_TAIWAN:
    {
      return 24;
    }
    case COUNTRY_TYPE_COLOMBIA:
    {
      return 2;
    }
    
    case COUNTRY_TYPE_NIGERIA:
    case COUNTRY_TYPE_VIETNAM:
    {
      return 6;
    }
    case COUNTRY_TYPE_GHANA:
      return 5;
    case COUNTRY_TYPE_AFRICA:
      return 1;
    
    case COUNTRY_TYPE_DJIBOUTI: //Djibouti
    {
       return 4;
    }
    case COUNTRY_TYPE_MONGOLIA:
    {
       return 1;
    }

   case COUNTRY_TYPE_POLAND:
   {
     return 6;    
   }
    
    case COUNTRY_TYPE_OTHER:
      return 0;
    default:
      return 0;
  }
}

//--------------------------------------------------------------------------------------------------
/// Get Next RF channel number by country by current Rf Ch
/// @param  u8Country               \b IN:  country
/// @param  u8RF_CH               \b IN:  current RF channel
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_next_rf_ch_num(u8 u8Country, u8 u8RF_CH)
{
  u8 u8MaxRFCh = dvbt_get_max_rf_ch_num(u8Country);
  u8 u8MinRFCh = dvbt_get_min_rf_ch_num(u8Country);
  if(u8RF_CH == u8MaxRFCh)
  {
    u8RF_CH = u8MinRFCh;
    return u8RF_CH;
  }

  switch (u8Country)
  {
    case COUNTRY_TYPE_COLOMBIA:
    case COUNTRY_TYPE_CHILE:
    case COUNTRY_TYPE_OTHER:
    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
    case COUNTRY_TYPE_ARGENTINA:
    {
      if (u8RF_CH < u8MaxRFCh)
      {
          u8RF_CH++;
      }
      else
      {
          u8RF_CH =  u8MinRFCh;
      }
      return u8RF_CH;
    }
#if (MARKET == MARKET_HONGKONG || MARKET == MARKET_CHINA)
    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_CHINA:
    {
      if (u8RF_CH < u8MaxRFCh)
      {
        u8RF_CH++;
      }
      else
      {
        u8RF_CH =  21;
      }
      return u8RF_CH;
    }
#endif
    case COUNTRY_TYPE_RUSSIAN:
    case COUNTRY_TYPE_UK:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND:  
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_ITALY:
    case COUNTRY_TYPE_FRANCE:
    case COUNTRY_TYPE_GREEK:     //
    case COUNTRY_TYPE_SLOVENIAN:
    case COUNTRY_TYPE_SPAIN:
    case COUNTRY_TYPE_DENMARK:
    case COUNTRY_TYPE_FINLAND:
    case COUNTRY_TYPE_SWEDEN:    
    {
#if (DVB_VHF_ENABLE == 1)
      if (u8RF_CH >= u8MinRFCh && u8RF_CH < 12)
      {
        u8RF_CH++;
      }
      else if (u8RF_CH >= 12 && u8RF_CH < 21)
      {
        u8RF_CH =  21;
      }
      else if (u8RF_CH >= 21 && u8RF_CH < u8MaxRFCh)
      {
        u8RF_CH++;
      }
      else
      {
        u8RF_CH = u8MinRFCh;
      }
      return u8RF_CH;
#else
      if (u8RF_CH < u8MaxRFCh)
      {
        u8RF_CH++;
      }
      else
      {
        u8RF_CH =  21;
      }
      return u8RF_CH;
#endif
  }
  case COUNTRY_TYPE_AUSTRALIA:
  {
#if AUSTRALIA_FREQTBL_NEW
    if (u8RF_CH < 13)
    {
      u8RF_CH++;
    }
    else if(u8RF_CH >= 13 && u8RF_CH < 21)
    {
      u8RF_CH =  21;
    }
    else if (u8RF_CH >= 21 && u8RF_CH < u8MaxRFCh)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH =  u8MinRFCh;
    }
#else
    if (u8RF_CH < 13)
    {
      u8RF_CH++;
    }
    else if(u8RF_CH >= 13 && u8RF_CH < 27)
    {
      u8RF_CH =  27;
    }
    else if (u8RF_CH >= 27 && u8RF_CH < u8MaxRFCh)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH =  u8MinRFCh;
    }
#endif
    return u8RF_CH;
  }
  case COUNTRY_TYPE_TAIWAN:
  {
    if (u8RF_CH >= 24 && u8RF_CH < 35)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = 24;
    }
    return u8RF_CH;
  }

  case COUNTRY_TYPE_NIGERIA:
  {
    if ((u8RF_CH >= 6 && u8RF_CH < 13) || (u8RF_CH >= 21 && u8RF_CH < u8MaxRFCh))
    {
      u8RF_CH++;
    }
    else if(u8RF_CH == 13)
    {
      u8RF_CH = 21;
    }
    else
    {
      u8RF_CH = u8MinRFCh;
    }
    return u8RF_CH;
  }
  case COUNTRY_TYPE_VIETNAM:
  {
    if ((u8RF_CH >= 6 && u8RF_CH < 12) || (u8RF_CH >= 21 && u8RF_CH < u8MaxRFCh))
    {
      u8RF_CH++;
    }
    else if(u8RF_CH == 12)
    {
      u8RF_CH = 21;
    }
    else
    {
      u8RF_CH = u8MinRFCh;
    }
    return u8RF_CH;
  }
  case COUNTRY_TYPE_GHANA:
    if (u8RF_CH >= u8MinRFCh && u8RF_CH < 12)
    {
      u8RF_CH++;
    }
    else if (u8RF_CH >= 12 && u8RF_CH < 21)
    {
      u8RF_CH =  21;
    }
    else if (u8RF_CH >= 21 && u8RF_CH < u8MaxRFCh)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = u8MinRFCh;
    }
    return u8RF_CH;    
   case COUNTRY_TYPE_AFRICA:
    if (u8RF_CH >= 1 && u8RF_CH < 10)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = 1;
    }
    return u8RF_CH;
    
  case COUNTRY_TYPE_DJIBOUTI:
  {
    if (u8RF_CH >= 4 && u8RF_CH < 13)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = 4;
    }
    return u8RF_CH;
  } 
     
  case COUNTRY_TYPE_MONGOLIA:
  {
    if (u8RF_CH >= 1 && u8RF_CH < 14)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = 1;
    }
    return u8RF_CH;
  }

  case COUNTRY_TYPE_POLAND:
  {
    if (u8RF_CH >= 6 && u8RF_CH < 61)
    {
      u8RF_CH++;
    }
    else
    {
      u8RF_CH = 6;
    }
    return u8RF_CH;
  }
  
  default:
    return 0;
  }
}


//--------------------------------------------------------------------------------------------------
/// Get prev RF channel number by country by current Rf Ch
/// @param  u8Country               \b IN:  country
/// @param  u8RF_CH               \b IN:  current RF channel
/// @return the lowest RF channel number
//--------------------------------------------------------------------------------------------------
u8 dvbt_get_prev_rf_ch_num(u8 u8Country, u8 u8RF_CH)
{
  //--------------------------------------------
  //wyf 20090324 to avoid  invalid channel number when manul scan
  u8 u8MaxRFCh = dvbt_get_max_rf_ch_num(u8Country);
  u8 u8MinRFCh = dvbt_get_min_rf_ch_num(u8Country);
  if(u8RF_CH == u8MinRFCh)
  {
        u8RF_CH = u8MaxRFCh;
     return u8RF_CH;
  }

  switch (u8Country)
  {
    case COUNTRY_TYPE_COLOMBIA:
    case COUNTRY_TYPE_CHILE:
    case COUNTRY_TYPE_OTHER:
    case COUNTRY_TYPE_PERU:
    case COUNTRY_TYPE_BRAZIL:
    case COUNTRY_TYPE_ARGENTINA:
    {
      if (u8RF_CH > u8MinRFCh)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH =  u8MaxRFCh;
      }
      return u8RF_CH;
    }

#if (MARKET == MARKET_HONGKONG || MARKET == MARKET_CHINA)
    case COUNTRY_TYPE_HONGKONG:
    case COUNTRY_TYPE_CHINA:
    {
      if (u8RF_CH > 21)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH =  u8MaxRFCh;
      }
      return u8RF_CH;
    }
#endif

    case COUNTRY_TYPE_UK:
    case COUNTRY_TYPE_RUSSIAN:
    case COUNTRY_TYPE_ENGLAND:
    case COUNTRY_TYPE_THAILAND:  
    case COUNTRY_TYPE_SINGAPORE:
    case COUNTRY_TYPE_GERMANY:
    case COUNTRY_TYPE_ITALY:
    case COUNTRY_TYPE_FRANCE:
    case COUNTRY_TYPE_GREEK:     //
    case COUNTRY_TYPE_SLOVENIAN:
    case COUNTRY_TYPE_SPAIN:
    case COUNTRY_TYPE_DENMARK:
    case COUNTRY_TYPE_FINLAND:
    case COUNTRY_TYPE_SWEDEN:   
    {
#if (DVB_VHF_ENABLE == 1)
      if (u8RF_CH > u8MinRFCh && u8RF_CH <= 12)
      {
        u8RF_CH--;
      }
      else if (u8RF_CH <= 21 && u8RF_CH > 12)
      {
        u8RF_CH =  12;
      }
      else if (u8RF_CH > 21 && u8RF_CH <= u8MaxRFCh)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = u8MaxRFCh;
      }

      return u8RF_CH;
#else
      if (u8RF_CH > 21)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH =  u8MaxRFCh;
      }

      return u8RF_CH;
#endif
    }
    //------------------------------------------//
    case COUNTRY_TYPE_AUSTRALIA:
    {
    #if AUSTRALIA_FREQTBL_NEW
      if (u8RF_CH > u8MinRFCh && u8RF_CH <=13)
      {
        u8RF_CH--;
      }
      else if (u8RF_CH > 13 && u8RF_CH <= 21)
      {
        u8RF_CH =  13;
      }
      else if (u8RF_CH > 21 && u8RF_CH <= u8MaxRFCh)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH =  u8MaxRFCh;
      }
    #else
      if (u8RF_CH > u8MinRFCh && u8RF_CH <=13)
      {
        u8RF_CH--;
      }
      else if (u8RF_CH > 13 && u8RF_CH <= 27)
      {
        u8RF_CH =  13;
      }
      else if (u8RF_CH > 27 && u8RF_CH <= u8MaxRFCh)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH =  u8MaxRFCh;
      }
   #endif
      return u8RF_CH;
    }

    case COUNTRY_TYPE_TAIWAN:
    {
      if (u8RF_CH > 24 && u8RF_CH <= 35)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = 35;
      }
      return u8RF_CH;
    }

    case COUNTRY_TYPE_NIGERIA:
    {
      if ((u8RF_CH > 6 && u8RF_CH <= 13) || ((u8RF_CH > 21) && (u8RF_CH <= u8MaxRFCh)))
      {
        u8RF_CH--;
      }
      else if(u8RF_CH == 21)
      {
        u8RF_CH = 13;
      }
      else
      {
        u8RF_CH = u8MaxRFCh;
      }
      return u8RF_CH;      
    }
    case COUNTRY_TYPE_VIETNAM:
    {
      if ((u8RF_CH > 6 && u8RF_CH <= 12) || ((u8RF_CH > 21) && (u8RF_CH <= u8MaxRFCh)))
      {
        u8RF_CH--;
      }
      else if(u8RF_CH == 21)
      {
        u8RF_CH = 12;
      }
      else
      {
        u8RF_CH = u8MaxRFCh;
      }
      return u8RF_CH;
   }
    case COUNTRY_TYPE_GHANA:
      if (u8RF_CH > u8MinRFCh && u8RF_CH <= 12)
      {
        u8RF_CH--;
      }
      else if (u8RF_CH <= 21 && u8RF_CH > 12)
      {
        u8RF_CH =  12;
      }
      else if (u8RF_CH > 21 && u8RF_CH <= u8MaxRFCh)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = u8MaxRFCh;
      }
      return u8RF_CH;
    case COUNTRY_TYPE_AFRICA:
      if (u8RF_CH > 1 && u8RF_CH <= 10)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = 10;
      }
      return u8RF_CH;
      
  case COUNTRY_TYPE_DJIBOUTI:
    {
      if (u8RF_CH > 4 && u8RF_CH <= 13)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = 13;
      }
      return u8RF_CH;
    } 

  case COUNTRY_TYPE_MONGOLIA:
    {
      if (u8RF_CH > 1 && u8RF_CH <= 14)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = 14;
      }
      return u8RF_CH;
    }   

    case COUNTRY_TYPE_POLAND:
    {
      if (u8RF_CH > 6 && u8RF_CH <= 61)
      {
        u8RF_CH--;
      }
      else
      {
        u8RF_CH = 61;
      }
      return u8RF_CH;

    }

    default:
        return 0;
  }
}


