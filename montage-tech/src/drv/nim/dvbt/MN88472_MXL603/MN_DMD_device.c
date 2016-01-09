/* **************************************************** */
/*!
   @file	MN_DMD_device.c
   @brief	Device dependence functions for MN88472
   @author	R.Mori
   @date	2011/7/6
   @param
		(c)	Panasonic
   */
/* **************************************************** */


#include "MN_DMD_common.h"
#include "MN_DMD_driver.h"
#include "MN_DMD_device.h"
#include "MN_I2C.h"
#include "MN8847x.h" 


/* **************************************************** */
/*	Constant*/
/* **************************************************** */
#define	DMD_DVBT_LOCKED_STATUS	10
#define	DMD_DVBT2_LOCKED_STATUS	13
#define	DMD_DVBC_LOCKED_STATUS	8

#define DMD_L1_BANK_SIZE 11
#define DMD_L1_REG_SIZE   8

/* '11/08/01 : OKAMOTO	Correct Error. */
#define DMD_ERROR_STATUS 0xFFFFFFFF


/* **************************************************** */
/*	Local Functions */
/* **************************************************** */
DMD_u32_t	DMD_RegRev(void);
DMD_u32_t	DMD_PSEQRev(void);
DMD_u32_t	DMD_AGC(void);
DMD_u32_t	DMD_System(void);
DMD_u32_t	DMD_Status(DMD_SYSTEM_t sys);			//SSEQ Status
DMD_u32_t	DMD_BER( DMD_SYSTEM_t sys , DMD_u32_t *err , DMD_u32_t *sum);
DMD_u32_t	DMD_BER_dvbt2( DMD_u32_t* err , DMD_u32_t* sum  , DMD_u32_t common );
DMD_u32_t	DMD_PER( DMD_SYSTEM_t sys , DMD_u32_t *err , DMD_u32_t *sum);
DMD_u32_t	DMD_CNR( DMD_SYSTEM_t sys , DMD_u32_t *cnr_i,DMD_u32_t *cnr_d);

/*! Information */
DMD_ERROR_t DMD_information_dvbt( DMD_PARAMETER_t *param , DMD_u32_t id , DMD_u32_t allflag);
DMD_ERROR_t DMD_information_dvbt2( DMD_PARAMETER_t *param , DMD_u32_t id, DMD_u32_t allflag);
DMD_ERROR_t DMD_information_dvbc( DMD_PARAMETER_t *param , DMD_u32_t id);
DMD_u32_t	DMD_get_l1( DMD_u8_t l1info[11][8] , DMD_u32_t	bitw , DMD_u32_t bank , DMD_u32_t adr , DMD_u32_t pos , DMD_u32_t allflag);
DMD_ERROR_t DMD_get_l1all( DMD_u8_t l1info[11][8] );

/* Scan ( for Channel Search )*/
DMD_ERROR_t DMD_scan_dvbt2( DMD_PARAMETER_t *param );
DMD_ERROR_t DMD_scan_dvbt( DMD_PARAMETER_t *param );
DMD_ERROR_t DMD_scan_dvbc( DMD_PARAMETER_t *param );

/* **************************************************** */
/*	Functions */
/* **************************************************** */

DMD_ERROR_t	DMD_system_support( DMD_SYSTEM_t sys )
{
	DMD_ERROR_t	ret;

	switch( sys )
	{
	case DMD_E_DVBT:
	case DMD_E_DVBT2:
	case DMD_E_DVBC:
		ret = DMD_E_OK;
		break;
	case DMD_E_ISDBT:
	case DMD_E_ISDBS:
	case DMD_E_DVBC2:
	case DMD_E_ATSC:
	case DMD_E_QAMB_64QAM:
	case DMD_E_QAMB_256QAM:
	case DMD_E_QAMC_64QAM:
	case DMD_E_QAMC_256QAM:
	case DMD_E_NTSC_M_BTSC:
	case DMD_E_PAL_M_BTSC:
	case DMD_E_PAL_N_BTSC:
	case DMD_E_PAL_B_G_NICAM:
	case DMD_E_PAL_I_NiCAM:
	case DMD_E_PAL_D_NiCAM:
	case DMD_E_PAL_B_G_A2:
	case DMD_E_SECAM_L_NiCAM:
	case DMD_E_SECAM_D_K_A2:
	default:
		ret = DMD_E_ERROR;
		break;
	}

	return ret;
}


/* **************************************************** */
/*! Read chip id */  
/* **************************************************** */
DMD_ERROR_t DMD_device_read_chipid( DMD_u8_t*	data )
{
	DMD_ERROR_t ret = DMD_E_OK;
	ret = DMD_I2C_Read( DMD_BANK_T2_ , DMD_CHIPRD, data);
	return ret;
}

/* **************************************************** */
/*! Load Auto Control Sequence */
/* **************************************************** */
DMD_ERROR_t DMD_device_load_pseq( DMD_PARAMETER_t* param )
{
	DMD_ERROR_t ret;
	DMD_u8_t	data;
	DMD_u32_t count = 0;
	
	DMD_device_read_chipid(&data);
		
	/* Load PSEQ Program */
	ret  = DMD_I2C_Write(   DMD_BANK_T_, DMD_PSEQCTRL , 0x20 );
	ret |= DMD_I2C_Write(   DMD_BANK_T_, DMD_PSEQSET  , 0x03 );
	
//to prevent potential risk that writing PSEQ to DMD wrongly, because some SoC cannot support large bytes writting at one time.
//If your SoC already matched with MN88472, no need to modify.
#if 1 
	if (data == DMD_TYPE_MN88472)
	{
		for ( count = 0; count < MN88472_REG_AUTOCTRL_SIZE; count++)
		{
		DMD_I2C_Write(  DMD_BANK_T_, DMD_PSEQPRG, MN88472_REG_AUTOCTRL[count] );
		}
	}
	else if (data == DMD_TYPE_MN88473)
	{
		for ( count = 0; count < MN88473_REG_AUTOCTRL_SIZE; count++)
		{
		DMD_I2C_Write(  DMD_BANK_T_, DMD_PSEQPRG, MN88473_REG_AUTOCTRL[count] );
		}
	}
	else
	{
	DMD_DBG_TRACE( "ERROR: CHIPID ERROR!! \n" );
	}
#else
		//ret |= DMD_I2C_WriteRead( DMD_BANK_T_, DMD_PSEQPRG  , MN88472_REG_AUTOCTRL , MN88472_REG_AUTOCTRL_SIZE , 0, 0 );
#endif

	ret |= DMD_I2C_Write(   DMD_BANK_T_, DMD_PSEQSET  , 0x00 );

	/* Check Parity bit */
	ret |= DMD_I2C_Read(   DMD_BANK_T_ , DMD_PSEQFLG , &data);
	if( data & 0x10 )
	{
		DMD_DBG_TRACE( "ERROR: PSEQ Parity" ); //sometimes it has no function to judge the PSEQ write procedure successes or not...|||
		ret |= DMD_E_ERROR;
	}

	return ret;
}

/* **************************************************** */
/*! Check Auto Control Runs or not */
// PSEQSET bit0 -> 0 : PSEQ running; 1 : PSEQ hold. 
// Troy.wang added, 20130412 
/* **************************************************** */
DMD_ERROR_t DMD_device_pseq_status( DMD_u8_t* pseq_status )
{
	DMD_ERROR_t ret;
	DMD_u8_t	data;
	ret = DMD_I2C_Read(   DMD_BANK_T_, DMD_PSEQSET  , &data );
	*pseq_status = data&0x01;
    return ret;	
}

/* **************************************************** */
/*! Device Open - wake up*/
/* **************************************************** */
DMD_ERROR_t DMD_device_open( DMD_PARAMETER_t* param )
{

		DMD_ERROR_t ret = DMD_E_OK;
		ret = DMD_set_power_mode(DMD_PWR_MODE_NORMAL);
		return ret;
}
/* **************************************************** */
/*! Device Close - Standby mode : Can reduce power consumption by about 300mW */
/* **************************************************** */
DMD_ERROR_t DMD_device_close(  DMD_PARAMETER_t* param )
{
		DMD_ERROR_t ret = DMD_E_OK;
	    ret = DMD_set_power_mode(DMD_PWR_MODE_STANDBY);
		return ret;
}
/* **************************************************** */
/*! Device term */
/* **************************************************** */
DMD_ERROR_t DMD_device_term(  DMD_PARAMETER_t* param )
{
	 return DMD_E_OK;
}

/* **************************************************** */
/*!	Get Register Settings for Broadcast system */
/* **************************************************** */
DMD_ERROR_t DMD_device_init(  DMD_PARAMETER_t* param )
{
	DMD_ERROR_t	ret;
	DMD_u8_t rd;
	/* Tranfer common register setting */
	DMD_device_read_chipid(&rd);
  mtos_printk("rd=%d\n",rd);
	if (rd == DMD_TYPE_MN88472)
	{
	  ret = DMD_trans_reg(  MN88472_REG_COMMON );
	}
  else if(rd == DMD_TYPE_MN88473)
	{
	  ret = DMD_trans_reg(  MN88473_REG_COMMON );
	}
	else
	{
		ret = DMD_E_ERROR;
		DMD_DBG_TRACE( "ERROR init: Not Supported Demod type ! \n");
	}
	
	DMD_DBG_TRACE("DMD_device_init ret =%d \n",ret);

//test I2C communication
	DMD_I2C_Write( DMD_BANK_T2_ , DMD_PLPID,  0x99 );
	DMD_I2C_Read( DMD_BANK_T2_ , DMD_PLPID,  &rd );	
	DMD_DBG_TRACE("\n ---- Test Demod I2C !! --- reg[PLPID = 0x%x < 0x99 means I2C OK>\n", rd);
	DMD_I2C_Write( DMD_BANK_T2_ , DMD_PLPID,  0x0 ); //back to default value.
	
  return ret;
}
/* **************************************************** */
/*!	Set Register setting for each broadcast system & bandwidth */
/* **************************************************** */
DMD_ERROR_t	DMD_device_set_system(  DMD_PARAMETER_t* param)
{
	DMD_ERROR_t	ret;
	DMD_u8_t	nosupport,rd;
	ret = DMD_E_OK;
	nosupport = 0;

	DMD_device_read_chipid(&rd);

#ifdef COMMON_DEBUG_ON 
    DMD_DBG_TRACE("set_system in ! chipid[%d], system[%d], bw[%d] \n",rd,param->system,param->bw);
#endif
	if (rd == DMD_TYPE_MN88472)
	{
		switch( param->system ){
			case DMD_E_DVBT2:
				switch( param->bw ){
					case DMD_E_BW_8MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT2_8MHZ );
						break;
					case DMD_E_BW_6MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT2_6MHZ );
						break;
					case DMD_E_BW_5MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT2_5MHZ );
						break;
					case DMD_E_BW_1_7MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT2_1_7MHZ );
						break;
					case DMD_E_BW_7MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT2_7MHZ );
						break;
					default:
						nosupport = 1;
						break;
				}
				break;
			case DMD_E_DVBT:
				switch( param->bw ){
					case DMD_E_BW_8MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT_8MHZ );
						break;
					case DMD_E_BW_7MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT_7MHZ );
						break;
					case DMD_E_BW_6MHZ:
						ret = DMD_trans_reg(  MN88472_REG_DVBT_6MHZ );
						break;
					default:
						nosupport = 1;
						break;	
				}
				break;
			case DMD_E_DVBC:
					ret = DMD_trans_reg(  MN88472_REG_DVBC );
				break;
			default:
					nosupport = 1;
		}
	}
	else if (rd == DMD_TYPE_MN88473)  
	{
		switch( param->system ){
			case DMD_E_DVBT2:
				switch( param->bw ){
					case DMD_E_BW_8MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT2_8MHZ );
						break;
					case DMD_E_BW_6MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT2_6MHZ );
						break;
					case DMD_E_BW_5MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT2_5MHZ );
						break;
					case DMD_E_BW_1_7MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT2_1_7MHZ );
						break;
					case DMD_E_BW_7MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT2_7MHZ );
						break;
					default:
						nosupport = 1;
						break;
				}
				break;
			case DMD_E_DVBT:
				switch( param->bw ){
					case DMD_E_BW_8MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT_8MHZ );
						break;
					case DMD_E_BW_7MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT_7MHZ );
						break;
					case DMD_E_BW_6MHZ:
						ret = DMD_trans_reg(  MN88473_REG_DVBT_6MHZ );
						break;
					default:
						nosupport = 1;
						break;	
				}
				break;
			case DMD_E_DVBC:
					ret = DMD_trans_reg(  MN88473_REG_DVBC );
				break;
			default:
					nosupport = 1;
		}
	}
	else
	{
	    DMD_DBG_TRACE( "ERROR : Not Supported Demod type ! \n");
	   ret = DMD_E_ERROR;
	}
	
	if( nosupport )
	{
		DMD_DBG_TRACE( "ERROR : Not Supported System");
	}
	//DMD_CHECK_ERROR( ret , "DMD_device_set_system" );
	return ret;
}
/* **************************************************** */
/*! Pretune Process */
/* **************************************************** */
DMD_ERROR_t DMD_device_pre_tune( DMD_PARAMETER_t* param )
{
	return DMD_E_OK;
}
/* **************************************************** */
/*! Posttune Process */
/* **************************************************** */
DMD_ERROR_t DMD_device_post_tune( DMD_PARAMETER_t* param )
{
	DMD_device_reset(  param);
	return DMD_E_OK;
}
/* **************************************************** */
/*! Soft Reset */
/* **************************************************** */
DMD_ERROR_t DMD_device_reset( DMD_PARAMETER_t* param )
{
	//DMD_I2C_Write(   DMD_BANK_T2_,DMD_INVSET ,0x80);//for debugging,    

	DMD_I2C_Write(   DMD_BANK_T2_,DMD_RSTSET1,0x9f); //0xF8
   	DMD_wait( 10);	 //wait 10ms for Demod itself, troy.wang, 20130226 
	return DMD_E_OK;
}

/* **************************************************** */
/*! scan */
/* **************************************************** */
DMD_ERROR_t DMD_device_scan( DMD_PARAMETER_t* param )
{
	DMD_ERROR_t	ret;
	ret = DMD_E_ERROR;
	switch( param->system )
	{
	case DMD_E_DVBT2:
		ret = DMD_scan_dvbt2( param );
		break;
	case DMD_E_DVBT:
		ret = DMD_scan_dvbt( param );
		break;
	case DMD_E_DVBC:
		ret = DMD_scan_dvbc( param );
		break;
	default:
		return DMD_E_ERROR;
	}
	return ret;
}

/* **************************************************** */
/*! Set Information  */
/* **************************************************** */
DMD_ERROR_t DMD_device_set_info(  DMD_PARAMETER_t* param , DMD_u32_t id , DMD_u32_t val )
{
	DMD_ERROR_t	ret =DMD_E_ERROR;
	DMD_u8_t	rd;
	switch( param->system )
	{
		case DMD_E_DVBT:
			switch( id )
			{
			case DMD_E_INFO_DVBT_HIERARCHY_SELECT:
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_RSDSET_T , &rd );
				if( val == 1 )
				{
					rd |= 0x10;
				}
				else
				{
					rd &= 0xef;
				}
				DMD_I2C_Write(   DMD_BANK_T_ , DMD_RSDSET_T , rd );
				param->info[DMD_E_INFO_DVBT_HIERARCHY_SELECT]	= (rd >> 4) & 0x1;
				ret = DMD_E_OK;
				break;
			case DMD_E_INFO_DVBT_MODE:
				ret =DMD_E_OK;
				if( val == DMD_E_DVBT_MODE_NOT_DEFINED ){
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDSET_T  ,  0xba );
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDASET_T ,  0x13 );
				}
				else
				{
					DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0xf0 , 0xf0 );
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDASET_T    , 0x0  );
					switch( val )
					{
					case DMD_E_DVBT_MODE_2K:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x0c , 0x00 );
						break;
					case DMD_E_DVBT_MODE_8K:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x0c , 0x08 );
						break;
					default:
						ret = DMD_E_ERROR;
						break;
					}

				}
				break;
			case DMD_E_INFO_DVBT_GI:
				ret =DMD_E_OK;
				if( val == DMD_E_DVBT_GI_NOT_DEFINED ){
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDSET_T  ,  0xba );
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDASET_T ,  0x13 );
				}
				else
				{
					DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0xf0 , 0xf0 );
					DMD_I2C_Write(   DMD_BANK_T_ , DMD_MDASET_T    , 0x0  );
					switch( val )
					{
					case DMD_E_DVBT_GI_1_32:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x03 , 0x00 );
						break;
					case DMD_E_DVBT_GI_1_16:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x03 , 0x01 );
						break;
					case DMD_E_DVBT_GI_1_8:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x03 , 0x02 );
						break;
					case DMD_E_DVBT_GI_1_4:
						DMD_I2C_MaskWrite(   DMD_BANK_T_  , DMD_MDSET_T , 0x03 , 0x03 );
						break;
					default:
						ret = DMD_E_ERROR;
						break;
					}

				}
     		} //case DVB-T end 
			break;	/* '12/02/22 : OKAMOTO	Correct error. */
		case DMD_E_DVBT2:
			switch( id )
			{
				case	DMD_E_INFO_DVBT2_SELECTED_PLP	:
					rd = (DMD_u8_t) val;
					DMD_I2C_Write(   DMD_BANK_T2_ , DMD_PLPID , rd );
					ret = DMD_E_OK;
					break;

			}
			break;	
		
		default:
			ret = DMD_E_ERROR;
			break;
	}

	return ret;
}

/* **************************************************** */
/*! Get Information  */
/* **************************************************** */
DMD_ERROR_t DMD_device_get_info( DMD_PARAMETER_t* param , DMD_u32_t id )
{
	DMD_u32_t i;
	switch( param->system ){
		case DMD_E_DVBT:
			if( id == DMD_E_INFO_DVBT_ALL ){
				param->info[DMD_E_INFO_DVBT_ALL] = DMD_E_INFO_DVBT_END_OF_INFORMATION;
				for( i=1 ; i < param->info[DMD_E_INFO_DVBT_ALL] ; i++ )
					DMD_information_dvbt( param , i , 1 );
			}
			else
					DMD_information_dvbt( param , id , 0 );
			break;
		case DMD_E_DVBT2:
			if( id == DMD_E_INFO_DVBT2_ALL ){
				param->info[DMD_E_INFO_DVBT2_ALL] = DMD_E_INFO_DVBT2_END_OF_INFORMATION;
				for( i=1 ; i < param->info[DMD_E_INFO_DVBT2_ALL] ; i++ )
					DMD_information_dvbt2( param , i , 1);
			}
			else if( id == DMD_E_INFO_DVBT2_L1_ALL )
			{
				DMD_information_dvbt2( param , id , 0);
				for(i=DMD_E_INFO_DVBT2_L1_ALL+1;i<DMD_E_INFO_DVBT2_END_OF_INFORMATION;i++)
				{
					DMD_information_dvbt2( param , i , 1);
				}
			}
			else
					DMD_information_dvbt2( param , id , 0);
			break;
		case DMD_E_DVBC:
			if( id == DMD_E_INFO_DVBC_ALL ){
				param->info[DMD_E_INFO_DVBC_ALL] = DMD_E_INFO_DVBC_END_OF_INFORMATION;
				for( i=1 ; i < param->info[DMD_E_INFO_DVBC_ALL] ; i++ )
					DMD_information_dvbc( param , i );
			}
			else
					DMD_information_dvbc( param , id );
			break;
		default:
			DMD_DBG_TRACE( "Unsupported Broadcast system" );
			return DMD_E_ERROR;
	}
	return DMD_E_OK;
}
	
/* **************************************************** */
/* MN88472 Local functions */
/* **************************************************** */
DMD_ERROR_t DMD_information_dvbt(  DMD_PARAMETER_t *param , DMD_u32_t id , DMD_u32_t allflag)
{
	DMD_u32_t	biterr,bitnum,packerr,packnum;
	DMD_u32_t	cnr_i,cnr_d;
	DMD_u8_t	rd;

	switch( id ){
		case	DMD_E_INFO_DVBT_ALL	:
			param->info[DMD_E_INFO_DVBT_ALL] = DMD_E_INFO_DVBT_END_OF_INFORMATION;
			break;
		case	DMD_E_INFO_DVBT_REGREV	:
			param->info[DMD_E_INFO_DVBT_REGREV] = DMD_RegRev();
			break;
		case	DMD_E_INFO_DVBT_PSEQRV	:
			param->info[DMD_E_INFO_DVBT_PSEQRV] = DMD_PSEQRev( );
			break;
		case	DMD_E_INFO_DVBT_SYSTEM	:
			param->info[DMD_E_INFO_DVBT_SYSTEM] = DMD_System( );
			break;
		case	DMD_E_INFO_DVBT_STATUS	:
			if( allflag == 1 ) break;
		case	DMD_E_INFO_DVBT_LOCK	:
			param->info[DMD_E_INFO_DVBT_STATUS] = DMD_Status( param->system);
			if( param->info[DMD_E_INFO_DVBT_STATUS] >= DMD_DVBT_LOCKED_STATUS ){
				param->info[DMD_E_INFO_DVBT_LOCK] = DMD_E_LOCKED;
			}
			else if( param->info[DMD_E_INFO_DVBT_LOCK] == DMD_E_LOCKED )
			{
				param->info[DMD_E_INFO_DVBT_LOCK] = DMD_E_ERROR;
			}
			break;
		case	DMD_E_INFO_DVBT_AGC	:
			param->info[DMD_E_INFO_DVBT_AGC] = DMD_AGC( );
			break;
		case	DMD_E_INFO_DVBT_BERRNUM	:
			if( allflag == 1 ) break;
		case	DMD_E_INFO_DVBT_BITNUM	:
			DMD_BER( param->system,&biterr,&bitnum);
			param->info[DMD_E_INFO_DVBT_BERRNUM]  = biterr;
			param->info[DMD_E_INFO_DVBT_BITNUM]   = bitnum;
			break;
		case	DMD_E_INFO_DVBT_CNR_INT	:
			if( allflag == 1 ) break;
		case	DMD_E_INFO_DVBT_CNR_DEC	:
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBT_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBT_CNR_DEC]  = cnr_d;
			break;
		case	DMD_E_INFO_DVBT_PERRNUM	:
			if( allflag == 1 ) break;
		case	DMD_E_INFO_DVBT_PACKETNUM	:
			DMD_PER( param->system,&packerr,&packnum);
			param->info[DMD_E_INFO_DVBT_PERRNUM]  = packerr;
			param->info[DMD_E_INFO_DVBT_PACKETNUM]   = packnum;
			break;
		case	DMD_E_INFO_DVBT_ERRORFREE	:
			DMD_I2C_Read(  DMD_BANK_T_ , DMD_ERRFLG_T , &rd );
			param->info[DMD_E_INFO_DVBT_ERRORFREE] = (rd & 0x1)?0:1;
			break;
		case	DMD_E_INFO_DVBT_SQI	:
			//Get CNR
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBT_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBT_CNR_DEC]  = cnr_d;
			//Get BER
			DMD_BER( param->system,&biterr,&bitnum);
			param->info[DMD_E_INFO_DVBT_BERRNUM]  = biterr;
			param->info[DMD_E_INFO_DVBT_BITNUM]   = bitnum;
			//Get PER, troy.wang added, 20130628
			DMD_PER( param->system,&packerr,&packnum);
			param->info[DMD_E_INFO_DVBT_PERRNUM]  = packerr;
			param->info[DMD_E_INFO_DVBT_PACKETNUM]   = packnum;
			//Get constelation mode 
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD2_T , &rd );
				param->info[DMD_E_INFO_DVBT_CONSTELLATION]	= (rd >> 3 ) & 0x3; 
			   param->info[DMD_E_INFO_DVBT_HIERARCHY]	= rd & 0x7; //troy, 20130131, DVB-T NON-HIERARCHY
			//Get code rate			
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD3_T , &rd );
				param->info[DMD_E_INFO_DVBT_HP_CODERATE]	= (rd >> 3 ) & 0x7;
				param->info[DMD_E_INFO_DVBT_LP_CODERATE]	= rd & 0x7;
			//Calc SQI
			param->info[DMD_E_INFO_DVBT_SQI] = DMD_calc_SQI( param );
			break;
		case	DMD_E_INFO_DVBT_HIERARCHY_SELECT	:
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_RSDSET_T , &rd );
			param->info[DMD_E_INFO_DVBT_HIERARCHY_SELECT]	= (rd >> 4) & 0x1;
			break;
		case	DMD_E_INFO_DVBT_MODE	:
		case	DMD_E_INFO_DVBT_GI	:
		case	DMD_E_INFO_DVBT_LENGTH_INDICATOR	:
		case	DMD_E_INFO_DVBT_CONSTELLATION	:
		case	DMD_E_INFO_DVBT_HIERARCHY	:
		case	DMD_E_INFO_DVBT_HP_CODERATE	:
		case	DMD_E_INFO_DVBT_LP_CODERATE	:
		case	DMD_E_INFO_DVBT_CELLID	:
			if( allflag == 1 ) break;
		case	DMD_E_INFO_DVBT_TPS_ALL	:
			//Get TPS Information
			if( DMD_Status( param->system) < DMD_DVBT_LOCKED_STATUS ){
				//Sync is not established , TPS information is not available
				param->info[DMD_E_INFO_DVBT_TPS_ALL]	=	0;
			}
			else
			{
				param->info[DMD_E_INFO_DVBT_TPS_ALL]	=	1;
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD4_T , &rd );
				param->info[DMD_E_INFO_DVBT_MODE]	= rd & 0x3;
				param->info[DMD_E_INFO_DVBT_GI]	= (rd >> 2 ) & 0x3;
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD1_T , &rd );
				param->info[DMD_E_INFO_DVBT_LENGTH_INDICATOR]	= rd & 0x3f;
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD2_T , &rd );
				param->info[DMD_E_INFO_DVBT_CONSTELLATION]	= (rd >> 3 ) & 0x3; 
				param->info[DMD_E_INFO_DVBT_HIERARCHY]	= rd & 0x7;
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_TMCCD3_T , &rd );
				param->info[DMD_E_INFO_DVBT_HP_CODERATE]	= (rd >> 3 ) & 0x7;
				param->info[DMD_E_INFO_DVBT_LP_CODERATE]	= rd & 0x7;
				//param->info[DMD_E_INFO_DVBT_HIERARCHY]	= rd & 0x7; //troy.wangyx masked, 20121207, it's redundant and it's wrong to get HP info. here . 
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_CELLIDU_T , &rd );
				param->info[DMD_E_INFO_DVBT_CELLID]	= rd * 0x100;
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_CELLIDL_T , &rd );
				param->info[DMD_E_INFO_DVBT_CELLID]	+= rd ;

			}
			break;

		default:
			return DMD_E_ERROR;
	}


	return DMD_E_OK;
}
/*! DVBT2 Information */
DMD_ERROR_t DMD_information_dvbt2(  DMD_PARAMETER_t *param , DMD_u32_t id, DMD_u32_t allflag)
{
	DMD_u32_t	biterr,bitnum,packerr,packnum;
	DMD_u32_t	cnr_i,cnr_d;
	DMD_u8_t	rd;
	static	DMD_u8_t	l1info[11][8];			//L1 Information Register
	switch( id ){
		case	DMD_E_INFO_DVBT2_ALL	:
			param->info[DMD_E_INFO_DVBT2_ALL] = DMD_E_INFO_DVBT2_END_OF_INFORMATION;
			break;
		case	DMD_E_INFO_DVBT2_REGREV	:
			param->info[DMD_E_INFO_DVBT2_REGREV] = DMD_RegRev();
			break;
		case	DMD_E_INFO_DVBT2_PSEQRV	:
			param->info[DMD_E_INFO_DVBT2_PSEQRV] = DMD_PSEQRev( );
			break;
		case	DMD_E_INFO_DVBT2_SYSTEM	:
			param->info[DMD_E_INFO_DVBT2_SYSTEM] = DMD_System( );
			break;
		case	DMD_E_INFO_DVBT2_LOCK	:
			param->info[DMD_E_INFO_DVBT2_STATUS] = DMD_Status( param->system);
			if( param->info[DMD_E_INFO_DVBT2_STATUS] >= DMD_DVBT2_LOCKED_STATUS ){
				param->info[DMD_E_INFO_DVBT2_LOCK] = DMD_E_LOCKED;
			} else if( param->info[DMD_E_INFO_DVBT2_LOCK] == DMD_E_LOCKED )
			{
				param->info[DMD_E_INFO_DVBT2_LOCK] = DMD_E_LOCK_ERROR;
			}
			break;
		case	DMD_E_INFO_DVBT2_AGC	:
			param->info[DMD_E_INFO_DVBT2_AGC] = DMD_AGC( );
			break;
		case	DMD_E_INFO_DVBT2_BERRNUM	:
		case	DMD_E_INFO_DVBT2_BITNUM	:
			DMD_BER( param->system,&biterr,&bitnum);
			param->info[DMD_E_INFO_DVBT2_BERRNUM]  = biterr;
			param->info[DMD_E_INFO_DVBT2_BITNUM]   = bitnum;
			break;
		case	DMD_E_INFO_DVBT2_CNR_INT	:
		case	DMD_E_INFO_DVBT2_CNR_DEC	:
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBT2_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBT2_CNR_DEC]  = cnr_d;
			break;
		case	DMD_E_INFO_DVBT2_PERRNUM	:
		case	DMD_E_INFO_DVBT2_PACKETNUM	:
			DMD_PER( param->system,&packerr,&packnum);
			param->info[DMD_E_INFO_DVBT2_PERRNUM]  = packerr;
			param->info[DMD_E_INFO_DVBT2_PACKETNUM]   = packnum;
			break;
		case	DMD_E_INFO_DVBT2_ERRORFREE	:
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_ERRFLG , &rd );
				param->info[DMD_E_INFO_DVBT2_ERRORFREE] = (rd & 0x1)?0:1;
				break;
		case	DMD_E_INFO_DVBT2_SQI	:
			//Get CNR
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBT2_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBT2_CNR_DEC]  = cnr_d;
			//Get BER
			DMD_BER( param->system,&biterr,&bitnum);
			param->info[DMD_E_INFO_DVBT2_BERRNUM]  = biterr;
			param->info[DMD_E_INFO_DVBT2_BITNUM]   = bitnum;
			//Get PER, troy.wang added, 20130628
			DMD_PER( param->system,&packerr,&packnum);
			param->info[DMD_E_INFO_DVBT2_PERRNUM]  = packerr;
			param->info[DMD_E_INFO_DVBT2_PACKETNUM]   = packnum;
			//Get constelation mode 
			param->info[DMD_E_INFO_DVBT2_DAT_PLP_MOD]= DMD_get_l1(   l1info, 3 , 3 , 1 , 7, allflag);
			//Get code rate
			param->info[DMD_E_INFO_DVBT2_DAT_PLP_COD]= DMD_get_l1(   l1info, 3 , 3 , 1 , 4, allflag); 
			//Calc SQI
			param->info[DMD_E_INFO_DVBT2_SQI] = DMD_calc_SQI( param );
			break; 
		case	DMD_E_INFO_DVBT2_MODE	:
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_SSEQRD1 , &rd );
				param->info[DMD_E_INFO_DVBT2_MODE] = rd & 0x7;
				break;
		case	DMD_E_INFO_DVBT2_GI	:	
		  		param->info[DMD_E_INFO_DVBT2_GI]= DMD_get_l1(   l1info, 3 , 0 , 2 , 6 , 0); 
				break;
		case	DMD_E_INFO_DVBT2_BERRNUM_C	:
		case	DMD_E_INFO_DVBT2_BITNUM_C	:
			DMD_BER_dvbt2( &biterr,&bitnum,1);
			param->info[DMD_E_INFO_DVBT2_BERRNUM_C]   = biterr;
			param->info[DMD_E_INFO_DVBT2_BITNUM_C]   = bitnum;
			break;
		case	DMD_E_INFO_DVBT2_SELECTED_PLP	:
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_PLPID , &rd );
				param->info[DMD_E_INFO_DVBT2_SELECTED_PLP] = rd ;
				break;
		case DMD_E_INFO_DVBT2_L1_ALL:
			DMD_get_l1all( l1info );
			break;
		case DMD_E_INFO_DVBT2_TYPE :			  param->info[DMD_E_INFO_DVBT2_TYPE]= DMD_get_l1(   l1info, 8 , 0 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_BW_EXT :			  param->info[DMD_E_INFO_DVBT2_BW_EXT]= DMD_get_l1(   l1info, 1 , 0 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_S1 :			  param->info[DMD_E_INFO_DVBT2_S1]= DMD_get_l1(   l1info, 3 , 0 , 1 , 6 , allflag); break;
		case DMD_E_INFO_DVBT2_S2 :			  param->info[DMD_E_INFO_DVBT2_S2]= DMD_get_l1(   l1info, 4 , 0 , 1 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_PAPR :			  param->info[DMD_E_INFO_DVBT2_PAPR]= DMD_get_l1(   l1info, 4 , 0 , 2 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_MOD :			  param->info[DMD_E_INFO_DVBT2_L1_MOD]= DMD_get_l1(   l1info, 4 , 0 , 3 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_COD :			  param->info[DMD_E_INFO_DVBT2_L1_COD]= DMD_get_l1(   l1info, 2 , 0 , 3 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_FEC_TYPE :		  param->info[DMD_E_INFO_DVBT2_L1_FEC_TYPE]= DMD_get_l1(   l1info, 2 , 0 , 3 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_POST_SIZE :		  param->info[DMD_E_INFO_DVBT2_L1_POST_SIZE]= DMD_get_l1(   l1info, 18 , 1 , 0 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_POST_INFO_SIZE :	  param->info[DMD_E_INFO_DVBT2_L1_POST_INFO_SIZE]= DMD_get_l1(   l1info, 18 , 1 , 3 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_PILOT_PATTERN :		  param->info[DMD_E_INFO_DVBT2_PILOT_PATTERN]= DMD_get_l1(   l1info, 4 , 0 , 4 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_TX_ID_AVAILABILITY :	  param->info[DMD_E_INFO_DVBT2_TX_ID_AVAILABILITY]= DMD_get_l1(   l1info, 8 , 10 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_CELL_ID :			  param->info[DMD_E_INFO_DVBT2_CELL_ID]= DMD_get_l1(   l1info, 16 , 10 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_NETWORK_ID :		  param->info[DMD_E_INFO_DVBT2_NETWORK_ID]= DMD_get_l1(   l1info, 16 , 10 , 3 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_T2_SYSTEM_ID :		  param->info[DMD_E_INFO_DVBT2_T2_SYSTEM_ID]= DMD_get_l1(   l1info, 16 , 10 , 5 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_NUM_T2_FRAMES :		  param->info[DMD_E_INFO_DVBT2_NUM_T2_FRAMES]= DMD_get_l1(   l1info, 8 , 0 , 5 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_NUM_DATA_SYMBOLS :	  param->info[DMD_E_INFO_DVBT2_NUM_DATA_SYMBOLS]= DMD_get_l1(   l1info, 12 , 0 , 6 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_REGEN_FLAG :		  param->info[DMD_E_INFO_DVBT2_REGEN_FLAG]= DMD_get_l1(   l1info, 3 , 0 , 4 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_POST_EXTENSION :	  param->info[DMD_E_INFO_DVBT2_L1_POST_EXTENSION]= DMD_get_l1(   l1info, 1 , 0 , 4 , 0 , allflag); break;
		case DMD_E_INFO_DVBT2_NUM_RF :			  param->info[DMD_E_INFO_DVBT2_NUM_RF]= DMD_get_l1(   l1info, 3 , 10 , 7 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_CURRENT_RF_IDX :		  param->info[DMD_E_INFO_DVBT2_CURRENT_RF_IDX]= DMD_get_l1(   l1info, 3 , 10 , 7 , 2 , allflag); break;
		case DMD_E_INFO_DVBT2_SUB_SLICES_PER_FRAME :	  param->info[DMD_E_INFO_DVBT2_SUB_SLICES_PER_FRAME]= DMD_get_l1(   l1info, 15 , 2 , 1 , 6 , allflag); break;
		case DMD_E_INFO_DVBT2_SUB_SLICE_INTERVAL :	  param->info[DMD_E_INFO_DVBT2_SUB_SLICE_INTERVAL]= DMD_get_l1(   l1info, 22 , 2 , 3 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_NUM_PLP :			  param->info[DMD_E_INFO_DVBT2_NUM_PLP]= DMD_get_l1(   l1info, 8 , 2 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_NUM_AUX :			  param->info[DMD_E_INFO_DVBT2_NUM_AUX]= DMD_get_l1(   l1info, 4 , 9 , 5 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_PLP_MODE :		  param->info[DMD_E_INFO_DVBT2_PLP_MODE]= DMD_get_l1(   l1info, 2 , 3 , 2 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_FEF_TYPE :		  param->info[DMD_E_INFO_DVBT2_FEF_TYPE]= DMD_get_l1(   l1info, 4 , 9 , 0 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_FEF_LENGTH :		  param->info[DMD_E_INFO_DVBT2_FEF_LENGTH]= DMD_get_l1(   l1info, 22 , 9 , 1 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_FEF_INTERVAL :		  param->info[DMD_E_INFO_DVBT2_FEF_INTERVAL]= DMD_get_l1(   l1info, 8 , 9 , 4 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_ID :		  param->info[DMD_E_INFO_DVBT2_DAT_PLP_ID]= DMD_get_l1(   l1info, 8 , 7 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_TYPE :		  param->info[DMD_E_INFO_DVBT2_DAT_PLP_TYPE]= DMD_get_l1(   l1info, 3 , 3 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_PAYLOAD_TYPE :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_PAYLOAD_TYPE]= DMD_get_l1(   l1info, 5 , 3 , 0 , 4 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_GROUP_ID :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_GROUP_ID]= DMD_get_l1(   l1info, 8 , 7 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_COD :		  param->info[DMD_E_INFO_DVBT2_DAT_PLP_COD]= DMD_get_l1(   l1info, 3 , 3 , 1 , 4 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_MOD :		  param->info[DMD_E_INFO_DVBT2_DAT_PLP_MOD]= DMD_get_l1(   l1info, 3 , 3 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_ROTATION :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_ROTATION]= DMD_get_l1(   l1info, 1 , 3 , 2 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_FEC_TYPE :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_FEC_TYPE]= DMD_get_l1(   l1info, 2 , 3 , 1 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_NUM_BLOCKS_MAX :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_NUM_BLOCKS_MAX]= DMD_get_l1(   l1info, 10 , 3 , 4 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_FRAME_INTEVAL :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_FRAME_INTEVAL]= DMD_get_l1(   l1info, 8 , 3 , 6 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_TIME_IL_LENGTH :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_TIME_IL_LENGTH]= DMD_get_l1(   l1info, 8 , 3 , 3 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_TIME_IL_TYPE :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_TIME_IL_TYPE]= DMD_get_l1(   l1info, 1 , 3 , 2 , 0 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_FF_FLAG :		  param->info[DMD_E_INFO_DVBT2_DAT_FF_FLAG]= DMD_get_l1(   l1info, 1 , 7 , 2 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_ID :		  param->info[DMD_E_INFO_DVBT2_COM_PLP_ID]= DMD_get_l1(   l1info, 8 , 7 , 4 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_TYPE :		  param->info[DMD_E_INFO_DVBT2_COM_PLP_TYPE]= DMD_get_l1(   l1info, 3 , 5 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_PAYLOAD_TYPE :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_PAYLOAD_TYPE]= DMD_get_l1(   l1info, 5 , 5 , 0 , 4 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_GROUP_ID :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_GROUP_ID]= DMD_get_l1(   l1info, 8 , 7 , 5 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_COD :		  param->info[DMD_E_INFO_DVBT2_COM_PLP_COD]= DMD_get_l1(   l1info, 3 , 5 , 1 , 4 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_MOD :		  param->info[DMD_E_INFO_DVBT2_COM_PLP_MOD]= DMD_get_l1(   l1info, 3 , 5 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_ROTATION :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_ROTATION]= DMD_get_l1(   l1info, 1 , 5 , 2 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_FEC_TYPE :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_FEC_TYPE]= DMD_get_l1(   l1info, 2 , 5 , 1 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_NUM_BLOCKS_MAX :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_NUM_BLOCKS_MAX]= DMD_get_l1(   l1info, 10 , 5 , 4 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_FRAME_INTEVAL :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_FRAME_INTEVAL]= DMD_get_l1(   l1info, 8 , 5 , 6 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_TIME_IL_LENGTH :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_TIME_IL_LENGTH]= DMD_get_l1(   l1info, 8 , 5 , 3 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_TIME_IL_TYPE :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_TIME_IL_TYPE]= DMD_get_l1(   l1info, 1 , 5 , 2 , 0 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_FF_FLAG :		  param->info[DMD_E_INFO_DVBT2_COM_FF_FLAG]= DMD_get_l1(   l1info, 1 , 7 , 6 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_FRAME_IDX :		  param->info[DMD_E_INFO_DVBT2_FRAME_IDX]= DMD_get_l1(   l1info, 8 , 8 , 0 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_TYPE_2_START :		  param->info[DMD_E_INFO_DVBT2_TYPE_2_START]= DMD_get_l1(   l1info, 22 , 8 , 2 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_L1_CHANGE_COUNTER :	  param->info[DMD_E_INFO_DVBT2_L1_CHANGE_COUNTER]= DMD_get_l1(   l1info, 8 , 8 , 1 , 7 , allflag); break;
		case DMD_E_INFO_DVBT2_START_RF_IDX :		  param->info[DMD_E_INFO_DVBT2_START_RF_IDX]= DMD_get_l1(   l1info, 8 , 3 , 4 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_FIRST_RF_IDX :	  param->info[DMD_E_INFO_DVBT2_DAT_FIRST_RF_IDX]= DMD_get_l1(   l1info, 3 , 7 , 2 , 2 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_START :		  param->info[DMD_E_INFO_DVBT2_DAT_PLP_START]= DMD_get_l1(   l1info, 22 , 4 , 2 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_DAT_PLP_NUM_BLOCKS :	  param->info[DMD_E_INFO_DVBT2_DAT_PLP_NUM_BLOCKS]= DMD_get_l1(   l1info, 10 , 4 , 0 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_FIRST_RF_IDX :	  param->info[DMD_E_INFO_DVBT2_COM_FIRST_RF_IDX]= DMD_get_l1(   l1info, 3 , 7 , 6 , 2 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_START :		  param->info[DMD_E_INFO_DVBT2_COM_PLP_START]= DMD_get_l1(   l1info, 22 , 6 , 2 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_COM_PLP_NUM_BLOCKS :	  param->info[DMD_E_INFO_DVBT2_COM_PLP_NUM_BLOCKS]= DMD_get_l1(   l1info, 10 , 6 , 0 , 1 , allflag); break;
		case DMD_E_INFO_DVBT2_STATIC_FLAG :		  param->info[DMD_E_INFO_DVBT2_STATIC_FLAG]= DMD_get_l1(   l1info, 1 , 3 , 2 , 5 , allflag); break;
		case DMD_E_INFO_DVBT2_STATIC_PADDING_FLAG :	  param->info[DMD_E_INFO_DVBT2_STATIC_PADDING_FLAG]= DMD_get_l1(   l1info, 1 , 3 , 2 , 4 , allflag); break;
		case DMD_E_INFO_DVBT2_IN_BAND_A_FLAG :		  param->info[DMD_E_INFO_DVBT2_IN_BAND_A_FLAG]= DMD_get_l1(   l1info, 1 , 3 , 2 , 3 , allflag); break;
		case DMD_E_INFO_DVBT2_IN_BAND_B_FLAG :		  param->info[DMD_E_INFO_DVBT2_IN_BAND_B_FLAG]= DMD_get_l1(   l1info, 1 , 3 , 2 , 2 , allflag); break;
#if (DEMOD_TYPE == DMD_TYPE_MN88473)
		case DMD_E_INFO_DVBT2_L1_POST_SCRAMBLE_ENABLED_FLG:
			DMD_I2C_Read( DMD_BANK_T2_ , DMD_L1DECRD1 , &rd );
			param->info[DMD_E_INFO_DVBT2_L1_POST_SCRAMBLE_ENABLED_FLG] = (rd & 0x2)>>1;//20131219, add for indicator of L1_POST_SCRAMBLE
#endif
      default:
			return DMD_E_ERROR;

	}
	return DMD_E_OK;
}

/*
Author  : Troy.wang
Date    : 20130621
Purpose : Detailed signal's modulation parameter detect(especially for unknow field test signal)
*/
void DMD_signal_information_printout_all( DMD_PARAMETER_t *param)
{
		DMD_u8_t	i;
		DMD_get_info(param,DMD_E_INFO_LOCK);
		if ( param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED )
		{
			DMD_DBG_TRACE(" CLK bias :  \n");  
			DMD_Carrier_Frequency_bias( param);  // check IF bias. 
			DMD_DBG_TRACE("[Advice] IF bias should be less than 100Khz, otherwise, pls. make sure tuner output 5Mhz IF \n");
			DMD_XTAL_Frequency_bias( param);    // check XTAL bias. 
			DMD_DBG_TRACE("[Advice] XTAL bias should be less than 50ppm, otherwise,demod hard to lock signal. pls. check : \n");
			DMD_DBG_TRACE("1.(Hareware)if DEMOD has independent XTAL,try to change capacity aroud it\n");
			DMD_DBG_TRACE("2.(software)if DEMOD get CLK from Tuner,try to change capacity by adjust param <xtalCfg.xtalCap>_Mxl603\n");
			
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_DETFLG, &i );
			DMD_DBG_TRACE("Interference detect : DET FLG = 0x%x \n",i);  // to check interference 

			DMD_wait(2000);//wait for TS output 
			DMD_get_info( param , DMD_E_INFO_CNR_INT ); 
			DMD_get_info( param, DMD_E_INFO_PERRNUM);
			DMD_DBG_TRACE(" CNR = %d.%d\n",param->info[DMD_E_INFO_CNR_INT],param->info[DMD_E_INFO_CNR_DEC]); 
			DMD_DBG_TRACE(" PER: ERRORNUM = %d,PACKNUM = %d\n",param->info[DMD_E_INFO_PERRNUM],param->info[DMD_E_INFO_PACKETNUM]);
			DMD_DBG_TRACE("[Advice]if PER : ERRORNUM = 0 and PACKNUM > 0, it means the TS output from Demod has no problem! If no video, pls. check : \n");
			DMD_DBG_TRACE("1. Demod TS output mode and Soc receive mode matches or not\n");
			DMD_DBG_TRACE("2. Check SoC's TS receiving code\n");

			DMD_DBG_TRACE("---------------BEGIN : Modulation parameters------------------- \n");  
			DMD_get_info(param,DMD_E_INFO_ALL);

			for(i = 0; i < param->info[DMD_E_INFO_ALL];i++)
			{
				DMD_DBG_TRACE("No.%d : %30s : ", i, DMD_info_title(param->system, i));
				DMD_DBG_TRACE( "[Text]%30s : [Value]%d \n", DMD_info_value(param->system, i, param->info[i]), param->info[i]);
			}				
			DMD_DBG_TRACE("----------------END : Modulation parameters------------------------\n");  
		}
		else
		{
			DMD_DBG_TRACE(" Signal unlocked, read out value invalid !! ");
			DMD_DBG_TRACE("[Advice]pls. follow the steps to check : ");
			DMD_DBG_TRACE("1. pls. make sure the signal itself no problem, eg. use MP set to test \n ");
			DMD_DBG_TRACE("2. pls. check the RF level is too low or not, by func. <Tuner_check_RF_input_power>\n ");
			DMD_DBG_TRACE("3. if you are using Maxlinear tuner, pls. check param <singleSupply_3_3V> with its power supply design \n ");
			DMD_DBG_TRACE("4. pls. check if you have set correct registers same as XTAL+IF, in func. <DMD_device_set_system>\n ");
			DMD_DBG_TRACE("5. pls. set Tuner self-AGC Mxl603_(agcCfg.agcType = MXL603_AGC_SELF)\n if no problem, pls. check circuit of External AGC\n ");
			DMD_DBG_TRACE("6. pls. try to receive DVB-T or change FFT size to lower one, eg. 2K, 8K, until signal locked\n ");
			DMD_DBG_TRACE("When signal locked, check debug of <CLK bias :>, normally it's caused by XTAL bias more than 50ppm!\n ");
			DMD_DBG_TRACE("7. After confirmed 1~5 steps, still NG :  Contact us ! \n ");
		}
}
					
/*! DVBC Information */
DMD_ERROR_t DMD_information_dvbc( DMD_PARAMETER_t *param , DMD_u32_t id)
{
	DMD_u32_t	err,sum;
	DMD_u32_t	cnr_i,cnr_d,symbol_rate;
	DMD_u8_t	rd;

	switch( id ){
		case	DMD_E_INFO_DVBC_ALL	:
			param->info[DMD_E_INFO_DVBC_ALL] = DMD_E_INFO_DVBC_END_OF_INFORMATION;
			break;
		case	DMD_E_INFO_DVBC_REGREV	:
			param->info[DMD_E_INFO_DVBC_REGREV] = DMD_RegRev();
			break;
		case	DMD_E_INFO_DVBC_PSEQRV	:
			param->info[DMD_E_INFO_DVBC_PSEQRV] = DMD_PSEQRev( );
			break;
		case	DMD_E_INFO_DVBC_SYSTEM	:
			param->info[DMD_E_INFO_DVBC_SYSTEM] = DMD_System( );
			break;
		case	DMD_E_INFO_DVBC_LOCK	:
			param->info[DMD_E_INFO_DVBC_STATUS] = DMD_Status( param->system);
			if( param->info[DMD_E_INFO_DVBC_STATUS] >= DMD_DVBC_LOCKED_STATUS ){
				param->info[DMD_E_INFO_DVBC_LOCK] = DMD_E_LOCKED;
			}
			break;
		case	DMD_E_INFO_DVBC_AGC	:
			param->info[DMD_E_INFO_DVBC_AGC] = DMD_AGC( );
			break;
		case	DMD_E_INFO_DVBC_BERRNUM	:
		case	DMD_E_INFO_DVBC_BITNUM	:
			DMD_BER( param->system,&err,&sum);
			param->info[DMD_E_INFO_DVBC_BERRNUM]  = err;
			param->info[DMD_E_INFO_DVBC_BITNUM]   = sum;
			break;
		case	DMD_E_INFO_DVBC_CNR_INT	:
		case	DMD_E_INFO_DVBC_CNR_DEC	:
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBC_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBC_CNR_DEC]  = cnr_d;
			break;
		case	DMD_E_INFO_DVBC_PERRNUM	:
		case	DMD_E_INFO_DVBC_PACKETNUM	:
			DMD_PER( param->system,&err,&sum);
			param->info[DMD_E_INFO_DVBC_PERRNUM]  = err;
			param->info[DMD_E_INFO_DVBC_PACKETNUM]   = sum;
			break;
		case	DMD_E_INFO_DVBC_ERRORFREE	:
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_ERRFLG_T , &rd );
			param->info[DMD_E_INFO_DVBT_ERRORFREE] = (rd & 0x1)?0:1;
			break;
	/* ---20130418,troy.wang added, DVBC SQI calculation--- BEGIN */	
		case	DMD_E_INFO_DVBC_SQI	:
			//Get CNR
			DMD_CNR( param->system,&cnr_i,&cnr_d);
			param->info[DMD_E_INFO_DVBC_CNR_INT]  = cnr_i;
			param->info[DMD_E_INFO_DVBC_CNR_DEC]  = cnr_d;
			//Get BER
			DMD_BER( param->system,&err,&sum);
			param->info[DMD_E_INFO_DVBC_BERRNUM]  = err;
			param->info[DMD_E_INFO_DVBC_BITNUM]   = sum;
			//Get constelation mode 
			DMD_I2C_Read(   DMD_BANK_C_, DMD_SSEQMON1_C, &rd );
			param->info[DMD_E_INFO_DVBC_CONSTELLATION]= (rd&0x07);
			//Get code rate - DVBC no code rate
	         //param->info[DMD_E_INFO_DVBT2_DAT_PLP_COD]= DMD_get_l1(   l1info, 3 , 3 , 1 , 4, allflag); 
			//Calc SQI
			param->info[DMD_E_INFO_DVBC_SQI] = DMD_calc_SQI( param );
			break;
	/* ---20130418,troy.wang added, DVBC SQI calculation--- END */
	
    //troy added, 20130507, some customer want to show DVB-C constellation and symbol rate
	   case DMD_E_INFO_DVBC_CONSTELLATION:
			DMD_I2C_Read(   DMD_BANK_C_, DMD_SSEQMON1_C, &rd );
			param->info[DMD_E_INFO_DVBC_CONSTELLATION]= (rd&0x07); //pls. refer to structrue "DMD_DVBC_CONST_t"
			break;
	   case DMD_E_INFO_DVBC_SYMBOL_RATE:
			DMD_DVBC_GET_SYMBOL_RATE(param,&symbol_rate);
			param->info[DMD_E_INFO_DVBC_SYMBOL_RATE]= symbol_rate; //unit : Khz
			break;

		default:
			return DMD_E_ERROR;

	}
	return DMD_E_OK;

}

/* **************************************************** */
/* Informations */
/* **************************************************** */
//! Infomation : Register set Version
DMD_u32_t	DMD_RegRev()
{
	return DMD_RegSet_Rev;
}
//! Infomation : Auto control Version
DMD_u32_t	DMD_PSEQRev( )
{
	DMD_u8_t	rd;	
	DMD_I2C_Read(   DMD_BANK_T_ , DMD_PSEQOP1_T , &rd );
	return (DMD_u32_t) rd;
}
//! Infomation : AGC Loop filter Level
//Bigger value means bigger RF power lever 
#define AGC_MIN 230  //-100dBm
#define AGC_MAX 896  //0dBm
#define AGC_RANGE (AGC_MAX - AGC_MIN)
DMD_u32_t	DMD_AGC(void)
{
	DMD_u8_t	rd;
	DMD_u32_t	ret;

	// Set to default -> AGC monitor AGCRD selection : AGC loop filter value (input signal level)
	DMD_I2C_Write(   DMD_BANK_T2_ , DMD_AGCRDSET, 0x00); 

	DMD_I2C_Read(   DMD_BANK_T2_ , DMD_AGCRDU , &rd );
	ret = rd * 4;
	DMD_I2C_Read(   DMD_BANK_T2_ , DMD_AGCRDL , &rd );
	ret += rd >> 6;

	return ret;

}

//Troy.wang added, 20120629, feedback IF center frequency bias for Tuner to retune, which is used for field test
//Return : kHz
DMD_s32_t DMD_Carrier_Frequency_bias( DMD_PARAMETER_t* param)
{ 
	DMD_u8_t  CarrierErrU = 0,CarrierErrM = 0, CarrierErrL = 0, CarrierDVBCFlag = 0;
	DMD_u32_t CarrierErrRd = 0; //unsigned
	DMD_s32_t CarrierErrKHz = 0; //signed. 
	DMD_u8_t  notsupport = 0,fNegativeCarrierErr = 0;

	/* Get estimated carrier error */	
	switch( param->system )
	 {
		case DMD_E_DVBT2:
		     {
				//AGC monitor AGCRD selection : Carrier frequency error
				DMD_I2C_Write(   DMD_BANK_T2_ , DMD_AGCRDSET, 0x06 );
				//AGCRD read [15:0]
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_AGCRDU , &CarrierErrU);
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_AGCRDL , &CarrierErrL);
				
				#ifdef COMMON_DEBUG_ON
				DMD_DBG_TRACE("dvbt2 carrier, 0x%x%x \n",CarrierErrU,CarrierErrL);
				#endif

				CarrierErrRd = ( ((DMD_u32_t)CarrierErrU << 8) & 0xFF00) + CarrierErrL;

				/* Signed Two's Compliment, so if the MSB is 1, set invert and add 1 */
				if (0x8000 & CarrierErrRd) 
				{
				CarrierErrRd = ((~CarrierErrRd) & 0x0000FFFF) + 0x1;
				fNegativeCarrierErr = 1;
				}
				/*
				Carrer frequency error can be calculated by the following equation:
				Carrier frequency error = (fS*10^6) / 2^17 * AGCRRD[15:0] [Hz] 
				                                   = AGCRRD[15:0] *(fS*10^3) / 2^17 [KHz]
				//1MHz = 10^6 Hz, 2^17 = 131072
				where
				fS : FFT sampling frequency
				8 MHz bandwidth : 64/7 MHz
				7 MHz bandwidth : 8 MHz
				6 MHz bandwidth : 48/7 MHz
				5 MHz bandwidth : 40/7 MHz
				1.7 MHz bandwidth : 131/71 MHz
				*/  				
				switch( param->bw ) // Integer ,For better accuracy; Uint : KHz
				{
					case DMD_E_BW_8MHZ:
						CarrierErrKHz = 64*CarrierErrRd/7*1000/131072; 
						break;
						case DMD_E_BW_7MHZ:
						CarrierErrKHz = 8*CarrierErrRd*1000/131072; 
						break;
					case DMD_E_BW_6MHZ:
						CarrierErrKHz = 48*CarrierErrRd/7*1000/131072; 
						break;
					case DMD_E_BW_5MHZ:
						CarrierErrKHz = 40*CarrierErrRd/7*1000/131072; 
						break;
					case DMD_E_BW_1_7MHZ:
						CarrierErrKHz = 131*CarrierErrRd/71*1000/131072;
						break;
					default:
						notsupport = 1; //Hz
						break;
					}

					// Set to default -> AGC monitor AGCRD selection : AGC loop filter value (input signal level)
					DMD_I2C_Write(   DMD_BANK_T2_ , DMD_AGCRDSET, 0x00 );
			} //case DMD_E_DVBT2 ends. 
			break;			
			
		case DMD_E_DVBT:
				{		
				DMD_u8_t dvbt_fft_mode;
				DMD_u32_t dvbt_ffts;
			   // Synchronization monitor SYN2RD_T selection : Carrier frequency error
				DMD_I2C_Write(   DMD_BANK_T_ , DMD_SYN2RDSET_T, 0x00 );
				//SYN2RDU read [10:0]
			 	DMD_I2C_Read(   DMD_BANK_T_ , DMD_SYN2RDU_T, &CarrierErrU);
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_SYN2RDL_T, &CarrierErrL);

				CarrierErrRd = ( ((DMD_u32_t)CarrierErrU << 8) & 0xFF00) + CarrierErrL;

				/* Signed Two's Compliment, so if the MSB is 1, set invert and add 1 */
				if (0x0400 & CarrierErrRd) //valid 11 bits.
				{
				CarrierErrRd = ((~CarrierErrRd) & 0x000007FF) + 0x1;
				fNegativeCarrierErr = 1;
				}
				/*
				Carrier frequency error = (fS*10^6) /ffts * SYN2RD[10:0] [Hz] 
				                                   = SYN2RD[10:0]*(fS*10^3) / ffts [Khz]
				where				
				ffts : FFT size (8K: 8192, 2K: 2048)
				fS : FFT sampling frequency
				8 MHz bandwidth : 64/7 MHz
				7 MHz bandwidth : 8 MHz
				6 MHz bandwidth : 48/7 MHz
				  */
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_MDRD_T, &dvbt_fft_mode );
				dvbt_fft_mode = (dvbt_fft_mode&0x0C)>>2; //FFT_S_T: bit[2:3] ; 0xC0, troy.wang modified, previous position was wrong... 20120920  

				if ( dvbt_fft_mode == 0 ) // 2K mode
					{
					    dvbt_ffts = 2048;
					}
				else if ( dvbt_fft_mode == 2 )//8K mode
					{
					    dvbt_ffts = 8192;
					} 
				else
					{
						notsupport = 1;
						break;
					}
					
				switch( param->bw ) //Integer , For better accuracy ; Unit : KHz 
				{
					case DMD_E_BW_8MHZ:
						CarrierErrKHz =  CarrierErrRd*1000*64/7/dvbt_ffts; 
						break;
			      	case DMD_E_BW_7MHZ:
						CarrierErrKHz = CarrierErrRd*1000*8/dvbt_ffts;
						break;
					case DMD_E_BW_6MHZ:
						CarrierErrKHz = CarrierErrRd*1000*48/7/dvbt_ffts;
						break;
					default:
						notsupport = 1;
						break;
				 }
			} //case DMD_E_DVBT: ends.
			break;
			
		case DMD_E_DVBC:

			DMD_I2C_Read(   DMD_BANK_C_ , DMD_AFCMON4_C, &CarrierErrL);
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_AFCMON3_C, &CarrierErrM);
				CarrierErrRd = (((DMD_u32_t)CarrierErrM << 8)& 0xFF00) + CarrierErrL;
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_AFCMON2_C, &CarrierErrU);
				CarrierErrRd = ( ((DMD_u32_t)CarrierErrU << 16) & 0xFF0000) + CarrierErrRd;

			/* Signed Two's Compliment, so if the MSB is 1, set invert and add 1 */
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_AFCMON1_C, &CarrierDVBCFlag);
			if (0x01 & CarrierDVBCFlag) 
			{
				CarrierErrRd = ((~CarrierErrRd) & 0x00FFFFFF) + 0x1;
				fNegativeCarrierErr = 1;
			}

			/*
                Calculate carrier frequency fERR error from AFCMON_C and
				sampling frequency fADCK is described by the following equation:
				fERR = fADCK / 2^27 * AFCMON_C[23:0] = fADCK / 2^18 * AFCMON_C / 2^9 Hz
			*/
			CarrierErrKHz = ((DMD_ADCK_FREQ*1000)/262144*CarrierErrRd/512)/1000;		
			
			break;
		default:
			notsupport = 1;
			break;
	}

			if(notsupport == 1)
			{
				CarrierErrKHz = 0;
				DMD_DBG_TRACE( "ERROR : Not Supported mode");
			}
			else
			{                          				 	
				if (1 == fNegativeCarrierErr) 
				CarrierErrKHz = CarrierErrKHz * (-1);

				DMD_DBG_TRACE(" Carrier frequency bias is [%d Khz] \n", CarrierErrKHz);
			}
 
	return CarrierErrKHz;
}

//Troy.wang added, 20120629, feedback Clock frequency bias, , which is used for signal lock issue.
//unit Hz .
DMD_s32_t DMD_XTAL_Frequency_bias( DMD_PARAMETER_t* param)
{ 
	DMD_u8_t  ClockErrU = 0, ClockErrL = 0;
	DMD_u32_t ClockErr = 0;
	DMD_s32_t ClockErrPPM = 0;
	DMD_u8_t  fNegativeClockErr = 0;
	DMD_u8_t nosupport = 0;

	/* Get estimated clock error */	
	switch( param->system ){
		case DMD_E_DVBT2:
				//Synchronization monitor SYN1RD selection : Clock frequency error
				DMD_I2C_Write(   DMD_BANK_T2_ , DMD_SYN1RDSET, 0x01 );

				//Calculate clock frequency error from SYN1RD : read [15:0]
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_SYN1RDU , &ClockErrU );
				DMD_I2C_Read(   DMD_BANK_T2_ , DMD_SYN1RDL, &ClockErrL );				 
			break;			
		case DMD_E_DVBT:
				//Synchronization monitor SYN1RD _T selection : Clock frequency error
				DMD_I2C_Write(   DMD_BANK_T_, DMD_SYN1RDSET_T, 0x01 );

				//Calculate clock frequency error from SYN1RD_T : read [15:0]
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_SYN1RDU_T , &ClockErrU );
				DMD_I2C_Read(   DMD_BANK_T_ , DMD_SYN1RDL_T , &ClockErrL );
			break;
			
		//DVB-C demodulator monitor doesn't have relative registers.
		case DMD_E_DVBC:
		default:
			nosupport = 1;
			break;
	}

	if( nosupport )
	{
		ClockErrPPM = 0;
		DMD_DBG_TRACE( "ERROR : Not Supported System");
	}
	else
	{
		ClockErr = ( ((DMD_u32_t)ClockErrU << 8) & 0xFF00) + ClockErrL;

		/* Signed Two's Compliment, so if the MSB is 1, set invert and add 1 */
		if (0x8000 & ClockErr) 
		{
			ClockErr = ((~ClockErr) & 0x0000FFFF) + 0x1;
			fNegativeClockErr = 1;
		}

	switch( param->system ){
			case DMD_E_DVBT2:
				// Equation : SYN1RD/2^24 * 10^6 [ppm]	, for better accuracy :	
				// ClockErrPPM = (ClockErr) / 2^24 * 2^20 - (ClockErr)/ 2^24 * 48576	
				ClockErrPPM = (ClockErr/16) - (ClockErr*48576/16777216);		 
				break;			
			case DMD_E_DVBT:
				// Equation : SYN1RD/2^27 * 10^6 [ppm]	, for better accuracy :	
				// ClockErrPPM = (ClockErr) / 2^27 * 2^20 - (ClockErr)/ 2^27 * 48576	
				ClockErrPPM = (ClockErr/128) - (ClockErr*48576/134217728);
				break;
			case DMD_E_DVBC:
			default:
	        break;
		}

		if (1 == fNegativeClockErr) 
		ClockErrPPM = ClockErrPPM * (-1);
	}

 	DMD_DBG_TRACE(" XTAL frequency bias is [%d ppm] \n", ClockErrPPM);
	return ClockErrPPM;
}


//troy.wang, 20131015, customer want to show constellation map 
/*
Input :
 action - 0: stop operation; 1 : start operation
 mon_type - different type of cell to monitor
output : 
  None
*/
DMD_ERROR_t DMD_Constellation_monitor_pre_setting( DMD_PARAMETER_t* param, DMD_u8_t action, DMD_DVBT2_CON_MON_t dvbt2_mon_type, DMD_DVBT_CON_MON_t dvbt_mon_type)
{
	DMD_ERROR_t ret = DMD_E_OK;
	DMD_u8_t ctmod = 0, temprd = 0;
	
	switch( param->system ){
		case DMD_E_DVBT2:
		if ( 1 == action )
			{
			DMD_I2C_Read( DMD_BANK_T_ , DMD_CTSET,  &temprd );
			//select type -> 0 : data plp ;  1 : common plp;  2 : L1pre;   3 : L1post
			ctmod = (0x03&dvbt2_mon_type)|0x04; 
			ctmod = ctmod | (temprd&0xf8); //not change bit 3~7 unknown. 
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET,  ctmod );  
			}
		else
			{
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET, 0x00 ); //stop monitoring
			}

			break;
				
		case DMD_E_DVBT:
		if ( 1 == action )
			{
			//start monitoring, only need do once before continous read. 
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CNSET_T , 0x00 );  //original 0x10.

			//select type -> 0 : CTA(Pre-deinterleave) ;  1 : CTB(Post-deinterleave)
			ctmod = dvbt_mon_type ? (0x60) : (0x40) ;
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET1_T , ctmod ); //original 0x00; contellation monitor start
			}
		else
			{
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CNSET_T , 0x10 ); //stop monitoring
			}
			break;
				
		case DMD_E_DVBC:
			//default OPEN. 
			break;
			
		default:
			DMD_DBG_TRACE("CONST MON unsupport system !\n");
			ret = DMD_E_ERROR;
	}

return ret;
}

//troy.wang, 20131015, customer want to show constellation map
/*
Input :
 CarrierNo - customer can set a range to check. eg. carrier 0~500 
output : 
  I, Q signal - I,Q is real co-ordinates of data points
*/
DMD_ERROR_t DMD_Constellation_monitoring( DMD_PARAMETER_t* param, DMD_u32_t CarrierNo,DMD_s32_t* p_IMON_RD, DMD_s32_t* p_QMON_RD_p)
{ 
	DMD_u8_t CarrierNoUp, CarrierNoLow, tIMONrd, tQMONrd,temprd;
	DMD_u8_t fNegative_i = 0,fNegative_q = 0;
	DMD_ERROR_t ret = DMD_E_OK;
	DMD_s32_t IMON_RD = 0, QMON_RD = 0;

    //pls. firstly set monitor type and start operation.
		
	switch( param->system ){
		case DMD_E_DVBT2:
			//start monitor [ Need start monitor every time ?! ]
			DMD_I2C_Read( DMD_BANK_T_ , DMD_CTSET,  &temprd );
			temprd |= 0x04;
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET,  temprd );

			//select carrier number 
			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTCARU , &temprd );
			CarrierNoUp = (DMD_u8_t)(CarrierNo >> 8 )&(0x7f);  
			CarrierNoUp = CarrierNoUp | (temprd&0x80); //not change bit7 unknown.
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTCARU , CarrierNoUp );	
			CarrierNoLow = (DMD_u8_t)CarrierNo; 	
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTCARL , CarrierNoLow);		 

			DMD_wait(1); //wait for monitoring. 
			
			//   I signal = CTRD[15:8]
			//   Q signal = CTRD[7:0] 
			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTRDU , &tIMONrd );
			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTRDL , &tQMONrd );
			IMON_RD = tIMONrd;
			QMON_RD = tQMONrd;
			
			if (0x80 & (tIMONrd)) 
				{
				IMON_RD = ((~tIMONrd) & 0x0000007F) + 0x1;
				fNegative_i = 1;
				}
			
			if (0x80 & (tQMONrd)) 
				{
				QMON_RD = ((~tQMONrd) & 0x0000007F) + 0x1;
				fNegative_q = 1;
				}	
		break;			
			
		case DMD_E_DVBT:

			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTSET1_T , &temprd );
			//select carrier number 
			CarrierNoUp = (DMD_u8_t)((CarrierNo >> 8 )&(0x1f));  
			CarrierNoUp = CarrierNoUp | (temprd&0x60); //not change bit CTLD_T and CTSEL_T
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET1_T , CarrierNoUp );	
			CarrierNoLow = (DMD_u8_t)CarrierNo; 	
			DMD_I2C_Write( DMD_BANK_T_ , DMD_CTSET2_T , CarrierNoLow);

			DMD_wait(1); //wait for monitoring. 
            
			// I signal = CTRD_T[15:8]
			// Q signal = CTRD_T[7:0]
			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTRDU_T , &tIMONrd );
			DMD_I2C_Read( DMD_BANK_T_ ,DMD_CTRDL_T , &tQMONrd );
			IMON_RD = tIMONrd;
			QMON_RD = tQMONrd;
			
			if (0x80 & (tIMONrd)) 
				{
				IMON_RD = ((~tIMONrd) & 0x0000007F) + 0x1;
				fNegative_i = 1;
				}
			
			if (0x80 & (tQMONrd)) 
				{
				QMON_RD = ((~tQMONrd) & 0x0000007F) + 0x1;
				fNegative_q = 1;
				}
		break;			
			
		case DMD_E_DVBC:
			//No need to selece carrier number 
			
			DMD_I2C_Read( DMD_BANK_C_ ,DMD_IMON1_C, &tIMONrd );
			IMON_RD = (tIMONrd&0x03) << 8;
			DMD_I2C_Read( DMD_BANK_C_ ,DMD_IMON2_C, &tIMONrd );
			IMON_RD = IMON_RD + tIMONrd;

			DMD_I2C_Read( DMD_BANK_C_ ,DMD_QMON1_C, &tQMONrd );
			QMON_RD = (tQMONrd&0x03) << 8;
			DMD_I2C_Read( DMD_BANK_C_ ,DMD_QMON2_C, &tQMONrd );
			QMON_RD = QMON_RD + tQMONrd;

			/* Signed Two's Compliment, so if the MSB is 1, set invert and add 1 */
			if (0x0200 & (IMON_RD)) 
			{
			IMON_RD = ((~IMON_RD) & 0x000001FF) + 0x1;
			fNegative_i = 1;
			}

			if (0x0200 & (QMON_RD)) 
			{
			QMON_RD = ((~QMON_RD) & 0x000001FF) + 0x1;
			fNegative_q = 1;
			}
		break;
			
		default:
			return DMD_E_ERROR;
	}
           
			*p_IMON_RD = IMON_RD;
			*p_QMON_RD_p = QMON_RD;

			if (1 == fNegative_i) 
				*p_IMON_RD = IMON_RD * (-1);

			if (1 == fNegative_q) 
				*p_QMON_RD_p = QMON_RD * (-1);				
	
 	DMD_DBG_TRACE(" DTV[%d] carrier[%d] -> I signal = %d, Q signal = %d \n",param->system,CarrierNo,*p_IMON_RD, *p_QMON_RD_p);
	return ret;
}


/*! Information : System detect (from register setting) */
DMD_u32_t	DMD_System( )
{
	DMD_u8_t	rd;
	DMD_u32_t	ret;

	DMD_I2C_Read(   DMD_BANK_T2_ , DMD_DTVSET , &rd );

	rd &= 0x7;
	switch( rd ){
		case 2:
			ret = DMD_E_DVBT;
			break;
		case 3:
			ret = DMD_E_DVBT2;
			break;
		case 4:
			ret = DMD_E_DVBC;
			break;
		default:
			ret =DMD_E_NOT_DEFINED;
			break;

	}
	return ret;
}
/*! Information : Reciever Status ( Detail )  */
DMD_u32_t	DMD_Status( DMD_SYSTEM_t sys ){
	DMD_u8_t	rd;
	DMD_u32_t	ret;

	ret = 0;
	switch( sys ){
		case DMD_E_DVBT:
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_SSEQRD_T , &rd );
			ret = (DMD_u32_t) (rd & 0xf);
			break;
		case DMD_E_DVBT2:
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_SSEQFLG , &rd );
			ret = (DMD_u32_t) (rd & 0xf);
			break;
		case DMD_E_DVBC:
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_SSEQMON2_C , &rd );
			ret = (DMD_u32_t) (rd & 0xf);
			break;
		/* '11/08/01 : OKAMOTO	Correct Error. */
		default:
			ret = DMD_ERROR_STATUS;
			break;
	};

	return ret;
}
//! Information : ERROR Num
DMD_u32_t	DMD_BER( DMD_SYSTEM_t sys , DMD_u32_t* err , DMD_u32_t* sum ){
	DMD_u8_t	rd;

	switch( sys )
	{
		case DMD_E_DVBT:
		case DMD_E_DVBC:
			//SET BERSET1[5] = 0
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERSET1_T , &rd );
			rd &= 0xDF;	//1101_1111
			DMD_I2C_Write(   DMD_BANK_T_ , DMD_BERSET1_T , rd );
			//SET BERRDSET[3:0] = 0101
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERRDSET_T, &rd );
			rd = (rd & 0xF0 ) | 0x5;
			DMD_I2C_Write(   DMD_BANK_T_ , DMD_BERRDSET_T, rd );
			//Read ERROR
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERRDU_T , &rd );
			*err = rd * 0x10000;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERRDM_T , &rd );
			*err += rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERRDL_T , &rd );
			*err += rd ;
			//Read BERLEN
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERLENRDU_T , &rd );
			*sum = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_BERLENRDL_T , &rd );
			*sum += rd;
			*sum = *sum * 203 * 8;
			break;

		case DMD_E_DVBT2:
			DMD_BER_dvbt2(   err , sum , 0 );	//Data PLP
			break;

		default:
			break;
	}
	return 0;
}




//! Information : ERROR Num
DMD_u32_t	DMD_BER_dvbt2(  DMD_u32_t* err , DMD_u32_t* sum  , DMD_u32_t common )
{
		DMD_u8_t	rd;

		//DATA PLP 
		DMD_I2C_Read( DMD_BANK_T2_ , DMD_BERSET , &rd );

		if( common == 0 )
		{
			rd |= 0x20;		//BERSET[5] = 1 (BER after LDPC)
			rd &= 0xef;		//BERSET[4] = 0 (Data PLP)
		}
		else
		{
			rd |= 0x20;		//BERSET[5] = 1 (BER after LDPC)
			rd |= 0x10;		//BERSET[4] = 1 (Common PLP)
		}
		DMD_I2C_Write(   DMD_BANK_T2_ , DMD_BERSET , rd );

		
		//Read ERROR
		DMD_I2C_Read(   DMD_BANK_T2_ , DMD_BERRDU , &rd );
		*err = rd * 0x10000;
		DMD_I2C_Read(   DMD_BANK_T2_ , DMD_BERRDM , &rd );
		*err += rd * 0x100;
		DMD_I2C_Read(   DMD_BANK_T2_ , DMD_BERRDL , &rd );
		*err += rd ;
		//Read BERLEN
		DMD_I2C_Read(   DMD_BANK_T2_ , DMD_BERLEN , &rd );
		*sum = (1 << (rd & 0xf) );
		
		if( common == 0 ) //selected PLP information( data plp )
		{
				 DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET2 , 0x3 );
				 DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TPD2   , &rd );
		}
		else		 // common PLP information 
		{
				 DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET2 , 0x5 );
				 DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TPD2    , &rd ); //troy.wang, 20120226, added for right FEC type read. 
		}
		
		if( (rd & 0x1 ) == 1 )
		{
			//FEC TYPE = 1
			switch( (rd>>2) & 0x7 )//code rate
			{
			case 0:	*sum = (*sum) * 32400; break;
			case 1:	*sum = (*sum) * 38880; break;
			case 2:	*sum = (*sum) * 43200; break;
			case 3:	*sum = (*sum) * 48600; break;
			case 4:	*sum = (*sum) * 51840; break;
			case 5:	*sum = (*sum) * 54000; break;
			}
		}
		else
		{
			//FEC TYPE = 0
			switch( (rd>>2) & 0x7 )//code rate
			{
			case 0:	*sum = (*sum) * 7200 * 4; break;
			case 1:	*sum = (*sum) * 9720 * 4; break;
			case 2:	*sum = (*sum) * 10800* 4; break;
			case 3:	*sum = (*sum) * 11880* 4; break;
			case 4:	*sum = (*sum) * 12600* 4; break;
			case 5:	*sum = (*sum) * 13320* 4; break;
			}
		}
		
		return 0;
}



//! Information : CNR
DMD_u32_t	DMD_CNR( DMD_SYSTEM_t sys , DMD_u32_t* cnr_i , DMD_u32_t* cnr_d ){
	DMD_u8_t	rd;
	DMD_s32_t	cnr;
	DMD_s32_t	sig,noise;
	switch( sys )
	{
		case DMD_E_DVBT:
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_CNRDU_T , &rd );
			cnr = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_CNRDL_T , &rd );
			cnr += rd;
			if( cnr != 0 )
			{
				//cnr = 65536 / cnr;
				cnr = log10_easy( 65536 ) - log10_easy( cnr ) + 200; //troy, 20130131, for better accuracy, when Signal is at QPSK, CNR is very large, 65536 / CNRD = integer, not float. 
				if( cnr < 0 ) cnr = 0;
			}
			else
				cnr = 0;

			*cnr_i = (DMD_u32_t ) cnr / 100;
			*cnr_d = (DMD_u32_t ) cnr % 100;
			break;
		case DMD_E_DVBT2:
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_CNRDU , &rd );
			cnr = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_CNRDL , &rd );
			cnr += rd;
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_CNFLG , &rd );
			if( cnr != 0 )
			{
				if( rd & 0x4 )
				{
					//MISO
					//cnr = 16384 / cnr;
					cnr =  log10_easy( 16384 ) - log10_easy( cnr ) - 600; //troy, 20130131, for better accuracy, when Signal is at QPSK, CNR is very large, 65536 / CNRD = integer, not float. 
					if( cnr < 0 ) cnr = 0;
					*cnr_i = (DMD_u32_t ) cnr / 100;
					*cnr_d = (DMD_u32_t ) cnr % 100;
				}
				else
				{
					//SISO
					//cnr = 65536 / cnr;
			    	cnr = log10_easy( 65536 ) - log10_easy( cnr ) + 200; //troy, 20130131, for better accuracy, when Signal is at QPSK, CNR is very large, 65536 / CNRD = integer, not float. 
					if( cnr < 0 ) cnr = 0;
					*cnr_i = (DMD_u32_t ) cnr / 100;
					*cnr_d = (DMD_u32_t ) cnr % 100;

				}
			}
			else
			{
				*cnr_i = 0;
				*cnr_d = 0;
			}
			break;
		case DMD_E_DVBC:
			
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_CNMON1_C , &rd );
			sig = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_CNMON2_C , &rd );
			sig += rd;
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_CNMON3_C , &rd );
			noise = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_C_ , DMD_CNMON4_C , &rd );
			noise += rd;

			if( noise != 0 )
				{
				  //cnr = log10_easy(sig * 8  / noise);
				   cnr = log10_easy(sig * 8) - log10_easy(noise); //troy, 20130131, for better accuracy
				}
			else
				cnr = 0;

			if( cnr < 0 ) cnr = 0;
			*cnr_i = (DMD_u32_t ) cnr / 100;
			*cnr_d = (DMD_u32_t ) cnr % 100;
			break;

   
      default:
			break;
	}
	return 0;
}
//! Information : Packet Error
DMD_u32_t	DMD_PER( DMD_SYSTEM_t sys , DMD_u32_t* err , DMD_u32_t* sum ){
	DMD_u8_t	rd;

	switch( sys )
	{
		case DMD_E_DVBT:
		case DMD_E_DVBC:
		case DMD_E_DVBT2:
			//Read ERROR
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_PERRDU , &rd );
			*err = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_PERRDL , &rd );
			*err += rd ;
			//Read BERLEN
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_PERLENRDU , &rd );
			*sum = rd * 0x100;
			DMD_I2C_Read(   DMD_BANK_T_ , DMD_PERLENRDL , &rd );
			*sum += rd;
			break;
		default:
			break;
	}
	return 0;

}

// fSYM = fADCK / RATEMON_C * (2^18) 
DMD_ERROR_t DMD_DVBC_GET_SYMBOL_RATE(  DMD_PARAMETER_t *param,DMD_u32_t *dvbc_get_symbolrate_Khz )
{
	DMD_ERROR_t ret = DMD_E_ERROR;  
	DMD_u8_t rd;
	DMD_u32_t fadc;
		
	if ( param->system == DMD_E_DVBC)
    {
		//Read FAD_C 
		DMD_I2C_Read(   DMD_BANK_C_ , DMD_RATERDU_C , &rd );
		fadc = rd * 0x10000;
		DMD_I2C_Read(   DMD_BANK_C_ , DMD_RATERDM_C , &rd );
		fadc += rd * 0x100;
		DMD_I2C_Read(   DMD_BANK_C_ , DMD_RATERDL_C , &rd );
		fadc += rd ;

		//*dvbc_get_symbolrate = (Xtal*10^6)/fadc*(2^18) -> unit : Hz
		//  = (xtal*10^3)*(2^16)/fadc*(2^2)   -> unit Khz, for better accuracy, for most 32bits. 
		*dvbc_get_symbolrate_Khz = (DMD_ADCK_FREQ*65536)/fadc*4;//troy, 20130507, treat XTAL seperately 
		ret = DMD_E_OK;
	}
	else 
	{
		DMD_DBG_TRACE(" This function is for DVB-C only !\n");		
		ret = DMD_E_ERROR;  
	}

	return ret;
}
//add by OEM
void DMD_get_status( DMD_PARAMETER_t *param )
{
#ifdef COMMON_DEBUG_ON
  DMD_u8_t rd = 0;
	int rxPwrPtr = 0;
extern void Tuner_check_RF_input_power(int* rxPwrPtr);

	//step1 : RF power level
	Tuner_check_RF_input_power(&rxPwrPtr);
	DMD_DEBUG(" rf level = %d dBm \n",rxPwrPtr);
	DMD_DEBUG("[Advice]if rf level lower than sensibility threshold, pls. check RF line connection or ask Tuner company to check hardware\n") ; 

	//step2 : T2-base or T2-lite 
	if (param->system == DMD_E_DVBT2)
	{
	DMD_device_dvbt2_read_P1_flag(&rd );
	DMD_DEBUG("DMD_scan T2 profile = %d[0/1 : T2_base; 2 : Non-T2; 3/4 : T2_lite; other : reserved ]; FEF exist ? [%d]",((rd&0x7f)>>3),(rd&0x01));
	DMD_DEBUG("[Advice]pls. use function <DMD_set_t2_work_profile> to set suitable work mode \n") ; 
	}

	//step3 : 
	/*when signal sometimes can be locked, or locked but abnormal*/
	DMD_signal_information_printout_all(param);	
#endif
}

DMD_ERROR_t DMD_get_check_lock( DMD_PARAMETER_t *param )
{
		DMD_ERROR_t	ret = DMD_E_ERROR;
		DMD_u8_t	rd = 0;
  //	DMD_u32_t  time_count_all = 34;//120 + 50;
  // 	DMD_u32_t  count = 0;
  param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_ERROR;	
  if(param->system == DMD_E_DVBT)
  {
		DMD_I2C_Read(DMD_BANK_T_  ,DMD_SSEQRD_T , &rd);
   // mtos_printk("DMD_E_DVBT LOCK st=%x\n",rd);
		if ( rd & 0x80 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
      ret = DMD_E_ERROR;
		}
		else if ( rd & 0x20 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSYNC;	
		}
		else if( (rd & 0xf) >= 10 )
		{
			ret = DMD_E_OK;
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
		}
  }
  else if(param->system == DMD_E_DVBT2)
  {
    DMD_I2C_Read(DMD_BANK_T2_  ,DMD_SSEQFLG , &rd);
    //mtos_printk("DMD_E_DVBT2 LOCK st=%x\n",rd);

    if ( rd & 0x40 )
    {
      param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
      ret = DMD_E_ERROR;

    }
    else if( (rd & 0xf) >= 13 )
    {
      ret = DMD_E_OK;
      param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
    }
    //DMD_BERLEN_adjust_dvbt2(param);
  }
  return ret;
}
void DMD_scan_dvbt2_ver2( DMD_PARAMETER_t *param )
{
	DMD_u32_t	time_count_all = 46;//180+50;//
	DMD_u32_t  count = 0;
   while(count < time_count_all) 	//timeout most 1.9s
   {
      DMD_get_check_lock(param);
  		count++;
  		DMD_wait(50);			//wait 10ms
   }
}
//end OEM
/* **************************************************** */
/* Scan (for Channel search) */
/* **************************************************** */
DMD_ERROR_t DMD_scan_dvbt2( DMD_PARAMETER_t *param )
{
 /*
 [ Background ]
    Based on technical analysis, that MN88472 takes max 1800ms to establish SYNC status. 
 If cannot establish signal, it will be Signal's problem : 
 1. Signal gets too strong noise;
 2. Signal too week;
 3. Nonstandard signal. 
 
 [ If MN88472 cannot lock signal in timeout length, but customer want to know why ]
    Pls. ask customer to provide : 
 Plan A : The TS stream and relative modulation parameter configuration 
 Plan B : Help to get RAW RF signal by using RF capture. 

 [ Suggestion ]
    As field environment is complicated, to decrease the Risk, 
 we recommend to increase timeout length to 50~100ms. 
    The only side effect it will be : 
 - May increase the search time in the channel with T2 signal in, but the signal quality is bad. 
 */
	DMD_ERROR_t	ret = DMD_E_OK;
#if 0
	DMD_u32_t	time_count_all = 180+10;
	DMD_u32_t  count = 0;
	DMD_u8_t	rd;

	param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_ERROR;	

   while(count < time_count_all) 	//timeout most 1.9s
   	{
		//P1 Give up
		DMD_I2C_Read(   DMD_BANK_T2_  ,DMD_SSEQFLG , &rd );
		
	#ifdef COMMON_DEBUG_ON 	
		DMD_DBG_TRACE("DVB-T2 SCAN c=%d	rd =%x	\n	",count,rd );
	#endif
				
		if ( rd & 0x40 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
			break;
		}
		
		//Sync judgement 
		if( (rd & 0xf) >= 13 )
		{
			ret = DMD_E_OK;
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
			break;
		}
		count++;
		DMD_wait( 10);			//wait 10ms
			
	}	
#endif
	return ret;
}

DMD_ERROR_t DMD_scan_dvbt( DMD_PARAMETER_t *param )
{
//	DMD_u32_t  time_count_all = 34;//120 + 50;
//	DMD_u32_t  count = 0;
//	DMD_u8_t	rd;
	DMD_ERROR_t	ret = DMD_E_ERROR;

//	param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_ERROR;	

	//set mode&gi search
	DMD_set_info(   param , DMD_E_INFO_DVBT_MODE , DMD_E_DVBT_MODE_NOT_DEFINED );
	DMD_set_info(   param , DMD_E_INFO_DVBT_GI   , DMD_E_DVBT_GI_NOT_DEFINED );
#if 0
   while(count < time_count_all) 	//timeout most 1.7s
	{	
		DMD_I2C_Read(   DMD_BANK_T_  ,DMD_SSEQRD_T , &rd );
		
#ifdef COMMON_DEBUG_ON 	
		DMD_DBG_TRACE("DVB-T SCAN c=%d	rd =%x	\n	",count,rd );
#endif
		
		//OFDM ERROR
		if ( rd & 0x80 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
			break;
		}
		//Mode Search : singal unstable, sync status jump back, for more than 3 times.
		if ( rd & 0x20 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSYNC;	
			break;
		}
		//Sync detection
		if( (rd & 0xf) >= 10 ) //20130701, original : Frame synchronization detection -> Synchronization established 
		{
			ret = DMD_E_OK;
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
			break;
		}
		DMD_wait( 10);			//wait 10ms
       	count++;
	}
#endif
	   
	return ret;
}

DMD_ERROR_t DMD_scan_dvbc(  DMD_PARAMETER_t *param )
{
	DMD_u32_t	st;
	DMD_u32_t	now;
	DMD_u32_t	timeout = 1800;
	DMD_u8_t	rd;
	DMD_ERROR_t	ret = DMD_E_ERROR;

	DMD_timer(&st);
	param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_ERROR;	

	do
	{
		DMD_I2C_Read(   DMD_BANK_C_  ,DMD_STSMON_C , &rd );

		//QAM ERR
		if( rd & 0x40 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSYNC;	
			break;
		}
		
		DMD_I2C_Read(   DMD_BANK_C_  ,DMD_DMDSTSMON1_C , &rd );
		//VQLOCK
		if( rd & 0x1 )
		{
			ret = DMD_E_OK;
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
			break;
		}
		DMD_wait( 10);	 //wait 10ms
		DMD_timer( &now );
	}
	while( now - st < timeout );	//timeout

	return ret;
}


DMD_u32_t	DMD_get_l1(  DMD_u8_t l1info[11][8] , DMD_u32_t	bitw , DMD_u32_t bank , DMD_u32_t adr , DMD_u32_t pos , DMD_u32_t allflag)
{
	DMD_s32_t	flag = bitw;
	DMD_u8_t	data;
	DMD_u32_t	ret = 0;

	for(;;)
	{
		if( allflag == 1 )
		{
			data = l1info[bank][adr];

		}
		else
		{
			DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET1 , 0 );
			DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET2 , (DMD_u8_t) bank );
			DMD_I2C_Read(   DMD_BANK_T2_ , (DMD_u8_t)(DMD_TPD1 + adr) , &data );
		}

		if( flag <= 8 )
		{
			data = ( data & ((1<<(pos+1))-1) );
			data = ( data >> (pos+1-flag ) ); 
			ret  += data;
		}
		else
		{
			ret += ( data & ((1<<(pos+1))-1) );
		}
		flag -= (pos + 1);
		if( flag <= 0 ) 
		{
			break;
		}
		else
		{
			ret <<= 8;

		}
		adr ++;
		pos = 7;
	}

	return ret;

}

DMD_ERROR_t	DMD_get_l1all(  DMD_u8_t l1info[DMD_L1_BANK_SIZE][DMD_L1_REG_SIZE] )
{
	DMD_u8_t i,j;
	DMD_u8_t	data;

	for( i=0; i < DMD_L1_BANK_SIZE ; i++ )
	{
		DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET1 ,  0 );
		DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TPDSET2 ,  i );
		for( j=0; j< DMD_L1_REG_SIZE; j ++ )
		{
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TPD1 + j , &data );
			l1info[i][j] = data;
		}
	}

	return DMD_E_OK;
}

DMD_ERROR_t	DMD_set_error_flag_output( DMD_u8_t bErrorFlagOutputEnable )
{
       DMD_u8_t data;
	   
	if(bErrorFlagOutputEnable)
		{		
			/* 1st,Adr:0x09(TSSET2) bit[2:0]=1 ("001") */
			if( DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TSSET2 , &data ) == DMD_E_ERROR ){
				return DMD_E_ERROR;
			}
			data &= 0xF8;
			data |= 0x1; //'01'
			if( DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TSSET2 , data ) == DMD_E_ERROR ){
				return DMD_E_ERROR;
			}
		}
	else
	{	
			//Default setting. INT signal ouput 
			/* 1st,Adr:0x09(TSSET2) bit[2:0]=1 ("000") */
			if( DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TSSET2 , &data ) == DMD_E_ERROR )
			{
				return DMD_E_ERROR;
			}
			data &= 0xF8; //'00'
			if( DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TSSET2 , data ) == DMD_E_ERROR ){
				return DMD_E_ERROR;
			}
	}
	return DMD_E_OK;
}


/* '11/08/29 : OKAMOTO	Select TS output. */
DMD_ERROR_t DMD_set_ts_output(  DMD_TSOUT ts_out )
{
    DMD_u8_t rd = 0;
	DMD_device_read_chipid(&rd);

	#ifdef COMMON_DEBUG_ON
	    DMD_DBG_TRACE("--- set TS output mode --- %d\n",ts_out);
	#endif
		
	if (rd == DMD_TYPE_MN88472)
	{
		switch(ts_out)
	       {	       
		case DMD_E_TSOUT_PARALLEL_FIXED_CLOCK:
			//TS parallel (Fixed clock mode)		TSSET1:0x00	FIFOSET:0xE1
			DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x00 );
			DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET , 0xE1 );
			break;
			
		case DMD_E_TSOUT_PARALLEL_VARIABLE_CLOCK:
			//TS parallel (Variable clock mode)		TSSET1:0x00	FIFOSET:0xE3
			DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x00 );
			DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET , 0xE3 );
			break;
			
		case DMD_E_TSOUT_SERIAL_VARIABLE_CLOCK:
			//TS serial(Variable clock mode)		TSSET1:0x1D	FIFOSET:0xE3
			DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x1D );
			DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET , 0xE3 );
			break;
			
		default:
			return DMD_E_ERROR;
	     }  
    }
	else if (rd == DMD_TYPE_MN88473)  
	{
		switch(ts_out)
			       {	       
				case DMD_E_TSOUT_PARALLEL_FIXED_CLOCK:
					//TS parallel (Fixed clock mode), BCK : 75/8 MHz
					DMD_I2C_Read(  DMD_BANK_T2_ , DMD_TSSET2 , &rd );
					rd |= 0x08;//set bit[3] to be 1
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET2 ,rd );
					
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x00 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_CLKSET1, 0x37 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET , 0x04 );
					break;		
					
				case DMD_E_TSOUT_PARALLEL_VARIABLE_CLOCK:
					//TS parallel (Variable clock mode)		
					DMD_I2C_Read(  DMD_BANK_T2_ , DMD_TSSET2 , &rd );
					rd &= 0xf7;//set bit[3] to be 0
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET2 ,rd );
					
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x00 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_CLKSET1, 0x3 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET, 0x05 );
					break;
					
				case DMD_E_TSOUT_SERIAL_FIXED_CLOCK:
					//TS serial(Fixed clock mode), SCK : 75MHz
					DMD_I2C_Read(  DMD_BANK_T2_ , DMD_TSSET2 , &rd );
					rd |= 0x08;//set bit[3] to be 1
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET2 ,rd );
					
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x1D );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_CLKSET1, 0x37 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET, 0x04 );
					break;
					
				case DMD_E_TSOUT_SERIAL_VARIABLE_CLOCK:
					//TS serial(Variable clock mode)		
					DMD_I2C_Read(  DMD_BANK_T2_ , DMD_TSSET2 , &rd );
					rd &= 0xf7;//set bit[3] to be 0 //20140120, troywang, modifiey BUG to set TS output mode !
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET2 ,rd );
					
					DMD_I2C_Write(  DMD_BANK_T2_ , DMD_TSSET1 , 0x1D );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_CLKSET1, 0x3 );
					DMD_I2C_Write(  DMD_BANK_T_ , DMD_FIFOSET, 0x05 );
					break;

				default:
					return DMD_E_ERROR;
		}
	}
	else
	{
	    DMD_DBG_TRACE( "ERROR : Not Supported Demod type ! \n");
	   return DMD_E_ERROR;
	}
	
	return DMD_E_OK;
}

/*
Description : Set DVB-T2 work profile
Input   : 
 param->t2_profile_type 
return : 
Date   : Troy, 20121219
*/
DMD_ERROR_t DMD_set_t2_work_profile(DMD_PARAMETER_t* param) 
{
    DMD_ERROR_t  ret_val = DMD_E_OK ;

	if (param->system != DMD_E_DVBT2)
	{
	   return DMD_E_OK;
	}

	switch (param->t2_profile_type)
	{
		case DMD_E_DVBT2_BASE: //default
		#ifdef COMMON_DEBUG_ON
		    DMD_DBG_TRACE("--- set T2 base mode ---\n");
		#endif
			ret_val = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_DTVSET, 0x03 );
			break;

		case DMD_E_DVBT2_LITE:
		#ifdef COMMON_DEBUG_ON
			DMD_DBG_TRACE("--- set T2 lite mode ---\n");
		#endif
			ret_val = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_DTVSET, 0x0B );
			break;

		default:
			DMD_DBG_TRACE("---ERROR ! Wrong T2 work profile !!--");
			ret_val = DMD_E_ERROR;
			break;
	}

    return (ret_val);
}


DMD_ERROR_t DMD_set_power_mode(  DMD_PWR_MODE_t running_mode ) 
{
    DMD_ERROR_t  ret_val = DMD_E_OK ;

	switch (running_mode)
	{
		case DMD_PWR_MODE_NORMAL: //wake up fucntion; after waken up no need to do init again, just re-tune, re-scan 
			ret_val = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_PWDSET, 0x00 );
			break;

		case DMD_PWR_MODE_STANDBY:
			ret_val = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_PWDSET, 0x3E );
			break;

		case DMD_PWR_MODE_SLEEP: //not suggest to set sleep. need to do HW-reset to wake it up
			ret_val = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_PWDSET, 0x3F );
			break;

		default:
			DMD_DBG_TRACE("---Power mode selected error !--");
			ret_val = DMD_E_ERROR;
			break;
	}

    return (ret_val);
}

//set I2C control mode, only for MN88473, 20131219
DMD_ERROR_t DMD_set_dmd_control_tuner_i2c_mode(void)
{
DMD_ERROR_t  ret_val = DMD_E_OK ;
#if (TUNER_I2C_MODE == TUNER_CONTROL_BY_DEMOD)
  #if (DEMOD_CONTROL_TUNER_I2C_MODE == TCB_I2C_FORMAT_REPEAT_MODE)
  ret_val = DMD_I2C_Write( DMD_BANK_T2_ , DMD_TCBSET, 0xD3 );
  #elif (DEMOD_CONTROL_TUNER_I2C_MODE == TCB_I2C_FORMAT_BYPASS_MODE)
  ret_val = DMD_I2C_Write( DMD_BANK_T2_ , DMD_TCBSET, 0xE3);
  #endif
#endif
return ret_val;
}

/*
Purpose     : Increase compatibility of locking non-standard signal, which has high ratio of Dummy cell -- Against DVB-T2 SPEC
              This is sometimes caused by customer who set Modulation parameter by themselves.
Side effect : Some parameter comibination(not DTG or Nordig standard),need more than timeout length to lock !!   
Suggest     : Not use this function !! 
Creator     : Troy.wang, 20130626
*/
void DMD_lock_high_ratio_dummy_cell_signal(void)
{
#if (DEMOD_TYPE == DMD_TYPE_MN88472)
	DMD_I2C_Write(DMD_BANK_T2_,DMD_WAFCSET3,0x13); //default 0x03 -> [warning]only for MN88472
#endif
    ;
}

/*
Cause       : Factory DVB-T signal, which is amplified more than twice, from signal generation to different office;
              20% SETs cannot lock this kind of signal because it need more time to stablize IF AGC by demod(external AGC).
              the NG SET receice signal directly from Signal generation, result is OK.
Remedy      : Add AGC timeout for DVB-T      
Side effect : May extend DVB-T scan time by 100ms
Suggest     : Applied! increase compatibility of receiver
Creator     : Troy.wang, 20131211, For GIEC factory issue 
*/
void DMD_DVBT_lock_high_noise_signal(void)
{
	DMD_I2C_Write(DMD_BANK_T_,DMD_SSEQTAU1_T,0x4B); //default 0x1B
}


/*
Purpose      : Container, for future use
Where to put : DMD_set_system()
Creator      : Troy.wang, 20130626
*/
void DMD_increase_compatibility_with_nonstandard_signal(DMD_PARAMETER_t *param__UNUSED)
{
    //DMD_lock_high_ratio_dummy_cell_signal(); //side effect, not use!
    DMD_DVBT_lock_high_noise_signal();
}

//to get know curret T2 profile 
DMD_ERROR_t DMD_device_dvbt2_read_P1_flag(  DMD_u8_t* p1flg_rd )
{ 
		DMD_ERROR_t  ret_val = DMD_E_OK ;

		//Get FEF status. S2 = xxx1, mixed frame, means T2 super frame including FEF part.  
		ret_val = DMD_I2C_Read( DMD_BANK_T2_ , DMD_P1FLG, p1flg_rd );  

#ifdef COMMON_DEBUG_ON
		DMD_DBG_TRACE("DVB-T2 P1FLG = 0x%x\n",p1flg_rd);
#endif
		return ret_val;
}

/*
Multipath interference detection
DETFLG[1:0],Larger values indicate larger interference levels.
INPUT  : DTV system (DVB-C in cable, no multipath)
OUTPUT : Multipath interface level
Note   : Valid only when signal locked
Creator : troy.wang, 20130628
*/
DMD_ERROR_t DMD_device_read_strong_echo(DMD_PARAMETER_t* param,DMD_u8_t* Multipath_inter_level)
{ 
   DMD_ERROR_t  ret_val = DMD_E_OK ;
		
	if( param->system == DMD_E_DVBT )
	{
	ret_val = DMD_I2C_Read( DMD_BANK_T_, DMD_DETFLG_T, Multipath_inter_level );  
	}
	else if( param->system == DMD_E_DVBT2 )
	{
	ret_val |= DMD_I2C_Read( DMD_BANK_T2_, DMD_DETFLG, Multipath_inter_level );  
	}

	return ret_val;
}

/*ISSY test A*/
/*
Purpose : To ignore ISSY field
The following is register setting to make the demod to ignore the ISSY.
  3rd, Adr:0xF6=0x04(default val=0x05)
  1st, Adr:0x7B=0xBC(default val=0x8C)

    Please test the above setting and check whether you can decode normally
or not. If the setting works, then the main issue would be the signal generator.
  *NOTE : This setting is just for monotoring SPLP(no FEF, no DNP),
          because under this situation ISSY function is optional.

Place to use : before DMD_device_reset()
*/
DMD_ERROR_t DMD_device_dvbt2_ignore_ISSY( void )
{
   DMD_ERROR_t ret = DMD_E_OK;

	DMD_I2C_Write(   DMD_BANK_C_ , DMD_FECSET3 , 0x04 ); //defaut 0x05, temperary, ISSY issue test 
	ret = DMD_I2C_Write(   DMD_BANK_T2_ , DMD_TSFSET2 , 0xBC );//to ignore ISSY field
			  
	return ret;  
}

/*ISSY test B*/
/*
Purpose : To check ISSY field enough or not in interleaving frame. 
< ETSI EN 302 755 V1.3.1 >
    Each Interleaving Frame for each PLP shall carry a TTO, a BUFS and at least one ISCR field.

B.  1. set 3rd, Adr:0xF6, bit[0]=0    <- just for monitoring setting !
    2. read 1st, Adr:0xB6, B7, B8
    3. check 1st, Adr:0xB7, bit[2:0]
To show if ISSY on or not .

    if bit[2:0] shows 0x7, ISSY is used. ISSY is consist of three factor, ISCR, TTO, BUFS.
    Above three bit corresponds to these factors as following :
    bit[2] : ISCR
    bit[1] : TTO
    bit[0] : BUFS
    if bit[2:0] shows other value in spite of ISSY on, there is possibility that input stream
    is inappropriate(becahse these three factor shall be carried in each interleaving frame)
 *NOTE : This setting is just for monotoring. this setting can't use for normal setting.
         And above bit[2:0] might show different value in normal setting.
*/
DMD_ERROR_t DMD_device_dvbt2_ISSY_validity_check( DMD_PARAMETER_t *param )
{
   DMD_ERROR_t ret = DMD_E_OK;
   
	   if (param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED)
	   {  
			DMD_u8_t	rd_tmp, rd_real;
			DMD_I2C_Write(   DMD_BANK_C_ , DMD_FECSET3 , 0x04 ); //defaut 0x05, temperary, ISSY issue test 
			DMD_wait( 10);		  

			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TSFRDU , &rd_tmp );
			DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TSFRDM , &rd_real ); //need read from top to bottom, but only need check TSFRDM status .
			ret = DMD_I2C_Read(   DMD_BANK_T2_ , DMD_TSFRDL , &rd_tmp ); 
			DMD_DBG_TRACE("ISSY status : TSFRDM = 0x%x \n",rd_real);  
       }
	return ret;  
}

#if (DEMOD_TYPE == DMD_TYPE_MN88472)
/* **************************************************** */
/* DVB-T Long delay echo ipmrovement */
/* 
[ Backgroud ]
DVB-T DTG test 10.8.5 Perfermance with long delay echo 
64QAM, CR 2/3, 2K 
< !!! > Si2158 + MN88472 fails  
SPEC : <= 22.2
before modified :  22.2
after modified  :  19.3 

[Principle]
Only affect DVB-T, FFT 2K, long echo delay item. 
The change will make the demodulator do more amendment in dealing echo interface.

[Side-effect]
it will degrade AWGN performance. 

[Notice]
1. since the modification affect another performance,
it needs some control loop :
Pls. add this function after signal locked and decoding begins, once a second

2. Mxl603+MN88472, the test result is OK. 
So, it's better to adjust Si2158 if possible.     
*/
/* **************************************************** */
DMD_ERROR_t DMD_device_dvbt_64qam_2k_long_delay_echoes_improve( DMD_PARAMETER_t *param )
{
    DMD_u8_t cur_dvbt_fft_mode , rd_dvbt_CLPRNK2_T;
 
	// only for Si2158 
	if (( param->system == DMD_E_DVBT)&&(param->info[DMD_E_INFO_LOCK] == DMD_E_LOCKED ))
	{		
		// detect FFT size
		DMD_I2C_Read(   DMD_BANK_T_ , DMD_MDRD_T, &cur_dvbt_fft_mode );
		//FFT_S_T: bit[3:2] -> 0 : 2K mode; 2 : 8K mode
		cur_dvbt_fft_mode = (cur_dvbt_fft_mode & 0x0C)>>2; 

		//CLPRNK2_T setting check
		DMD_I2C_Read(   DMD_BANK_T_ , DMD_CLPRNK2_T, &rd_dvbt_CLPRNK2_T ); 

    DMD_DBG_TRACE(" --- Pana DVB-T --- FFT mode[%x], CLPRNK2set[%x] \n ", cur_dvbt_fft_mode,rd_dvbt_CLPRNK2_T);
			    
		//FFT 2K detected and correponding CLPRNK2_T value not set yet 
		if ( (cur_dvbt_fft_mode == 0) && ( rd_dvbt_CLPRNK2_T != 0x74) )
		{
		  DMD_I2C_Write(   DMD_BANK_T_ , DMD_CLPRNK2_T,  0x74 );
		}
		else if ((cur_dvbt_fft_mode != 0)&& ( rd_dvbt_CLPRNK2_T != 0x54))// FFT 8K status 
		{
		  DMD_I2C_Write(   DMD_BANK_T_ , DMD_CLPRNK2_T,  0x54 );//for other test items' performance 
		}  

	} 

   return DMD_E_OK;
}

/* **************************************************** */
/* DVB-C multipath tolerance ipmrovement */
/* 
[ Backgroud ]
DVB-C, 0.5us delay 
64QAM, symbol rate 6.875MS/s, 64QAM 
< !!! > Si2158 + MN88472 fails  
SPEC : <= 10
before modified :  10.3
after modified  :  8.7 

[Principle]
By setting smaller range of carrier frequency, it makes the demod can
receive a critical signal. 
Only affect DVB-C/ 

[Side-effect]
AFC range will be narrowed from 500Khz to 350Khz.

[ How to check AFC range ]
Change RF freq, and check whether demod is locked or not
    For exam : 
    -> RF= 665.7MHz & 666.3 : locked
    -> RF= 665.6MHz & 666.4 : not locked
    Then AFC range is -300 to +300 kHz

[ Where to use ]
in function "DMD_set_system"
*/
/* **************************************************** */
DMD_ERROR_t DMD_device_dvbc_multipath_tolerance_improve( void )
{
		DMD_ERROR_t ret;
		ret = DMD_I2C_Write(   DMD_BANK_C_ , DMD_QCAFCADD2L_C,  0x65 ); //default, 0x95;  margin more than 1.0 dB ; only for Si2158 + MN88472 
		return ret;
}

#if 0
//add by OEM
DMD_ERROR_t DMD_get_check_lock( DMD_PARAMETER_t *param )
{
		DMD_ERROR_t	ret = DMD_E_ERROR;
		DMD_u8_t	rd = 0;
  //	DMD_u32_t  time_count_all = 34;//120 + 50;
  // 	DMD_u32_t  count = 0;
  param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_ERROR;	
  if(param->system == DMD_E_DVBT)
  {
		DMD_I2C_Read(DMD_BANK_T_  ,DMD_SSEQRD_T , &rd);
    mtos_printk("DMD_E_DVBT LOCK st=%x\n",rd);
		if ( rd & 0x80 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
		}
		else if ( rd & 0x20 )
		{
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSYNC;	
		}
		else if( (rd & 0xf) >= 9 )
		{
			ret = DMD_E_OK;
			param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
		}
  }
  else if(param->system == DMD_E_DVBT2)
  {
    DMD_I2C_Read(DMD_BANK_T2_  ,DMD_SSEQFLG , &rd);
    mtos_printk("DMD_E_DVBT2 LOCK st=%x\n",rd);

    if ( rd & 0x40 )
    {
      param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSIGNAL;	
    }
    else if ( rd & 0x20 )
    {
      param->info[DMD_E_INFO_LOCK] = DMD_E_LOCK_NOSYNC;	
    }
    else if( (rd & 0xf) >= 13 )
    {
      ret = DMD_E_OK;
      param->info[DMD_E_INFO_LOCK] = DMD_E_LOCKED;	
    }
   // DMD_BERLEN_adjust_dvbt2(param);
  }
  return ret;
}
#endif
/*
  We can use Network ID to distiguish different Service 
*/
#endif //(DEMOD_TYPE == DMD_TYPE_MN88472)

