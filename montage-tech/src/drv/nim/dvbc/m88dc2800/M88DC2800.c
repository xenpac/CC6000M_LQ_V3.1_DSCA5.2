/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*
 * Filename:		M88DC2800.c
 * Current Version:	2.251a1
 *
 * Description:	Montage M88DC2800 QAM demodulator IC driver.
 				This driver is for Montage TC2800 tuner.
 *
 ****************************************************************************/
#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "i2c.h"
#include "nim.h"
#include "nim_m88dc2800.h"

//#include "M88DC2800.h"
#include "mt_fe_common.h"
#include "mt_fe_def.h"

#define Sleep mtos_task_delay_ms
#define ReadReg(reg) dc2800_dmd_reg_read(g_p_dc2800_priv->dmd_addr, reg)
#define WriteReg(reg, val) dc2800_dmd_reg_write(g_p_dc2800_priv->dmd_addr, reg, val)		

static MT_FE_DC2800_SETTINGS dc2800_device_handle;

void M88DC2000RegFix4Jazz(U8 mode, U16 qam);

/***********************************************
 Config tuner via M88DC2000 I2C repeater
 freq:		RF frequency, unit: KHz
 sym:			symbol rate, unit: KBaud
 Qam:			QAM mode, 16, 32, 64, 128, 256
 Inverted:	Spectrum inverted(1) or not(0)
 xtal:		in KHz
 ************************************************/
void M88DC2000SetChannel(U32 freq, U32 sym, U16 qam, U8 inverted, U32 xtal) 
{
#if 0
	WriteReg(0x80, 0x01);
	M88DC2000RegInitial();
	M88DC2000SetSym(sym, xtal);
	M88DC2000SetQAM(qam);
	M88DC2000SetTxMode(inverted, 0); /*	J83A					*/
	//M88DC2000SetTsType(1); /* set it when driver initialization			*/
	M88DC2000SoftReset();
	M88DC2000RegFix4Jazz(g_p_dc2800_priv->chip_mode, qam);
#else
   mt_fe_dmd_dc2800_connect(&dc2800_device_handle, freq, sym, qam, inverted);
#endif
}

void M88DC2000RegFix4Jazz(U8 mode, U16 qam)
{
	U8 tmp;

	if(mode == 0)			// DC2800 mode
	{
		tmp = ReadReg(0x00);
		tmp &= 0x7F;
		WriteReg(0x00, tmp);
		
		tmp = ReadReg(0x03);
		tmp &= 0xDF;
		tmp |= 0x10;
		WriteReg(0x03, tmp);
		
		tmp = ReadReg(0x04);
		tmp |= 0x02;
		WriteReg(0x04, tmp);
		
		tmp = ReadReg(0x4C);
		tmp |= 0x40;
		WriteReg(0x4C, tmp);
		
		tmp = ReadReg(0x5F);
		tmp &= 0x3F;
		WriteReg(0x5F, tmp);
		
		tmp = ReadReg(0x72);
		tmp &= 0x7F;
		WriteReg(0x72, tmp);
		
		tmp = ReadReg(0x89);
		tmp &= 0x0F;
		WriteReg(0x89, tmp);
		
		tmp = ReadReg(0xE5);
		tmp &= 0x7F;
		WriteReg(0xE5, tmp);
		
		tmp = ReadReg(0xE8);
		tmp &= 0xFE;
		WriteReg(0xE8, tmp);
		
		tmp = ReadReg(0xF9);
		tmp &= 0xFA;
		tmp |= 0x02;
		WriteReg(0xF9, tmp);
		
		tmp = ReadReg(0xFE);
		tmp |= 0x08;
		WriteReg(0xFE, tmp);
	}
	else if(mode == 1)		// Jazz mode
	{
		tmp = ReadReg(0x00);
		tmp |= 0x80;
		WriteReg(0x00, tmp);

		WriteReg(0x01, 0x42);
		WriteReg(0x01, 0x62);//2012.1.6 test for lock time
		WriteReg(0x02, 0x60);

		tmp = ReadReg(0x03);
		tmp |= 0x30;
		WriteReg(0x03, tmp);

		WriteReg(0x24, 0xFF);
		WriteReg(0x25, 0x00);
		WriteReg(0x26, 0xFA);
		WriteReg(0x27, 0x80);
		WriteReg(0x5F, 0x0C);

		if(qam == 16)
			WriteReg(0x62, 0x18);
		else
			WriteReg(0x62, 0x28);

		tmp = ReadReg(0x00);
		if( ((tmp & 0x70) == 0x20) || ((tmp & 0x70) == 0x10) || ((tmp & 0x70) == 0x00) )		// 128 QAM or 32QAM or 16QAM
		{
			WriteReg(0x6C, 0x52);
		}
		else
		{
			WriteReg(0x6C, 0x62);
		}

		WriteReg(0x6D, 0xA3);
		WriteReg(0x6F, 0x00);
		WriteReg(0x75, 0x18);
		WriteReg(0x7F, 0x71);

		tmp = ReadReg(0x93);
		tmp &= 0x7F;
		WriteReg(0x93, tmp);

		WriteReg(0xE5, 0xA6);
		WriteReg(0xE8, 0x3B);
		WriteReg(0xF0, 0x83);
		//WriteReg(0xF1, 0xFF);//2012.1.6 test for lock time
		WriteReg(0xF4, 0x0F);
		//WriteReg(0xF4, 0x00);//2012.1.6 test for lock time
		tmp = ReadReg(0xF9);
		tmp &= 0xF8;
		tmp |= 0x02;
		WriteReg(0xF9, tmp);

		WriteReg(0xFB, 0x44);
		//WriteReg(0xFC, 0xF1);//2012.1.6 test for lock time
		WriteReg(0xFC, 0x91);
		WriteReg(0xFD, 0x13);
		WriteReg(0xFE, 0x50);

		//WriteReg(0x00, 0xC6);//2012.1.6 test for lock time
		//WriteReg(0x03, 0x3c);//2012.1.6 test for lock time
	}
	else
	{
	}

	return;
}

void M88DC2000Initial(void)
{
    mt_fe_dmd_dc2800_config_default(&dc2800_device_handle);
    mt_fe_dmd_dc2800_select_tuner(&dc2800_device_handle,g_dc2800_tn_drv.tn_id);
    mt_fe_dmd_dc2800_init(&dc2800_device_handle);
}
/***********************************************
 Initialize the internal registers in M88DC2000
************************************************/
void M88DC2000RegInitial(void)
{
	U8 RegE3H, RegE4H;

	WriteReg(0x00,0x48);
	WriteReg(0x01,0x09);
	WriteReg(0xFB,0x0A);
	WriteReg(0xFC,0x0B);
	WriteReg(0x02,0x0B);
	WriteReg(0x03,0x18);
	WriteReg(0x05,0x0D);
	WriteReg(0x36,0x80);
	WriteReg(0x43,0x40);
	WriteReg(0x55,0x7A);
	WriteReg(0x56,0xD9);
	WriteReg(0x57,0xDF);
	WriteReg(0x58,0x39);
	WriteReg(0x5A,0x00);
	WriteReg(0x5C,0x71);
	WriteReg(0x5D,0x23);
	//WriteReg(0x86, 0x40);
	WriteReg(0xF9,0x08);
	WriteReg(0x61,0x40);
	WriteReg(0x62,0x0A);
	WriteReg(0x90,0x06);
	WriteReg(0xDE,0x00);
	WriteReg(0xA0,0x03);
	WriteReg(0xDF,0x81);
	WriteReg(0xFA,0x40);
	WriteReg(0x37,0x10);
	WriteReg(0xF0,0x40);
	WriteReg(0xF2,0x9C);
	WriteReg(0xF3,0x40);

	RegE3H = ReadReg(0xE3);
	RegE4H = ReadReg(0xE4);
	if (((RegE3H & 0xC0) == 0x00) && ((RegE4H & 0xC0) == 0x00))
	{
		WriteReg(0x30,0xFF);
		WriteReg(0x31,0x00);
		WriteReg(0x32,0x00);
		WriteReg(0x33,0x00);
		WriteReg(0x35,0x32);
		WriteReg(0x40,0x00);
		WriteReg(0x41,0x10);
		WriteReg(0xF1,0x02);
		WriteReg(0xF4,0x04);
		WriteReg(0xF5,0x00);
		WriteReg(0x42,0x14);
		//WriteReg(0xE1, 0x25);
	}
	else if (((RegE3H & 0xC0) == 0x80) && ((RegE4H & 0xC0) == 0x40))
	{
		WriteReg(0x30,0xFF);
		WriteReg(0x31,0x00);
		WriteReg(0x32,0x00);
		WriteReg(0x33,0x00);
		WriteReg(0x35,0x32);
		WriteReg(0x39,0x00);
		WriteReg(0x3A,0x00);
		WriteReg(0x40,0x00);
		WriteReg(0x41,0x10);
		WriteReg(0xF1,0x00);
		WriteReg(0xF4,0x00);
		WriteReg(0xF5,0x40);
		WriteReg(0x42,0x14);
		//WriteReg(0xE1, 0x25);
	}
	else if ((RegE3H == 0x80 || RegE3H == 0x81) && (RegE4H == 0x80 || RegE4H == 0x81))
	{
		WriteReg(0x30,0xFF);
		WriteReg(0x31,0x00);
		WriteReg(0x32,0x00);
		WriteReg(0x33,0x00);
		WriteReg(0x35,0x32);
		WriteReg(0x39,0x00);
		WriteReg(0x3A,0x00);
		WriteReg(0xF1,0x00);
		WriteReg(0xF4,0x00);
		WriteReg(0xF5,0x40);
		WriteReg(0x42,0x24);
		//WriteReg(0xE1, 0x25);

		WriteReg(0x92,0x7F);
		WriteReg(0x93,0x91);
		WriteReg(0x95,0x00);
		WriteReg(0x2B,0x33);
		WriteReg(0x2A,0x2A);
		WriteReg(0x2E,0x80);
		WriteReg(0x25,0x25);
		WriteReg(0x2D,0xFF);
		WriteReg(0x26,0xFF);
		WriteReg(0x27,0x00);
		WriteReg(0x24,0x25);
		WriteReg(0xA4,0xFF);
		WriteReg(0xA3,0x0D);
	}
	else
	{
		WriteReg(0x30,0xFF);
		WriteReg(0x31,0x00);
		WriteReg(0x32,0x00);
		WriteReg(0x33,0x00);
		WriteReg(0x35,0x32);
		WriteReg(0x39,0x00);
		WriteReg(0x3A,0x00);
		WriteReg(0xF1,0x00);
		WriteReg(0xF4,0x00);
		WriteReg(0xF5,0x40);
		WriteReg(0x42,0x24);
		//WriteReg(0xE1, 0x27);

		WriteReg(0x92,0x7F);
		WriteReg(0x93,0x91);
		WriteReg(0x95,0x00);
		WriteReg(0x2B,0x33);
		WriteReg(0x2A,0x2A);
		WriteReg(0x2E,0x80);
		WriteReg(0x25,0x25);
		WriteReg(0x2D,0xFF);
		WriteReg(0x26,0xFF);
		WriteReg(0x27,0x00);
		WriteReg(0x24,0x25);
		WriteReg(0xA4,0xFF);
		WriteReg(0xA3,0x10);
	}

	WriteReg(0xF6,0x4E);
	WriteReg(0xF7,0x20);
	WriteReg(0x89,0x02);
	WriteReg(0x14,0x08);
	WriteReg(0x6F,0x0D);
	WriteReg(0x10,0xFF);
	WriteReg(0x11,0x00);
	WriteReg(0x12,0x30);
	WriteReg(0x13,0x23);
	WriteReg(0x60,0x00);
	WriteReg(0x69,0x00);
	WriteReg(0x6A,0x03);
	WriteReg(0xE0,0x75);
	//WriteReg(0x8D, 0x29);
	WriteReg(0x4E,0xD8);
	WriteReg(0x88,0x80);
	WriteReg(0x52,0x79);
	WriteReg(0x53,0x03);
	WriteReg(0x59,0x30);
	WriteReg(0x5E,0x02);
	WriteReg(0x5F,0x0F);
	WriteReg(0x71,0x03);
	WriteReg(0x72,0x12);
	WriteReg(0x73,0x12);
}




/***********************************************
   Set spectrum inversion and J83
   Inverted: 1, inverted; 0, not inverted
   J83: 0, J83A; 1, J83C
 ***********************************************/
void M88DC2000SetTxMode(U8 inverted,U8 j83)
{
#if 0
	U8 value = 0;

	if (inverted)	value |= 0x08;		/*	spectrum inverted			*/
	if (j83)		value |= 0x01;		/*	J83C						*/
	WriteReg(0x83, value);
#else
_mt_fe_dmd_dc2800_set_tx_mode(&dc2800_device_handle,inverted,j83);
#endif
}

/***********************************************
   Set		symbol rate
   sym:		symbol rate, unit: KBaud
   xtal:	unit, KHz
************************************************/
void M88DC2000SetSym(U32 sym, U32 xtal)
{
#if 0
	U8	value;
	U8	reg6FH, reg12H;
	DB	fValue;
	U32	dwValue;



	fValue  = 4294967296.0 * (sym + 10) / xtal;
	dwValue = (U32)fValue;
	WriteReg(0x58, (U8)((dwValue >> 24) & 0xff));
	WriteReg(0x57, (U8)((dwValue >> 16) & 0xff));
	WriteReg(0x56, (U8)((dwValue >>  8) & 0xff));
	WriteReg(0x55, (U8)((dwValue >>  0) & 0xff));


	fValue = 2048.0 * xtal / sym;
	dwValue = (U32)fValue;
	WriteReg(0x5D, (U8)((dwValue >> 8) & 0xff));
	WriteReg(0x5C, (U8)((dwValue >> 0) & 0xff));


	value = ReadReg(0x5A);
	if (((dwValue >> 16) & 0x0001) == 0)
		value &= 0x7F;
	else
		value |= 0x80;
	WriteReg(0x5A, value);



	value = ReadReg(0x89);
	if (sym <= 1800)
		value |= 0x01;
	else
		value &= 0xFE;
	WriteReg(0x89, value);



	if (sym >= 6700)
	{
		reg6FH = 0x0D;
		reg12H = 0x30;
	}
	else if (sym >= 4000)
	{
		fValue = 22 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x30;
	}
	else if (sym >= 2000)
	{
		fValue = 14 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x20;
	}
	else
	{
		fValue = 7 * 4096 / sym;
		reg6FH = (U8)fValue;
		reg12H = 0x10;
	}
	WriteReg(0x6F, reg6FH);
	WriteReg(0x12, reg12H);

	if (((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
	{
		if(sym < 3000)
		{
			WriteReg(0x6C,0x16);
			WriteReg(0x6D,0x10);
			WriteReg(0x6E,0x18);
		}
		else
		{
			WriteReg(0x6C,0x14);
			WriteReg(0x6D,0x0E);
			WriteReg(0x6E,0x36);
		}
	}
	else
	{
		WriteReg(0x6C,0x16);
		WriteReg(0x6D,0x10);
		WriteReg(0x6E,0x18);
	}
#else
  _mt_fe_dmd_dc2800_set_symbol_rate(&dc2800_device_handle, sym, xtal);
#endif
}


/***********************************************
   Set QAM mode
   Qam: QAM mode, 16, 32, 64, 128, 256
************************************************/
void M88DC2000SetQAM(U16 qam)
{
#if 0
	U8 reg00H, reg4AH, regC2H, reg44H, reg4CH, reg4DH, reg74H, value;
	U8 reg8BH, reg8EH;

	regC2H = ReadReg(0xC2);
	regC2H &= 0xF8;
	switch(qam)
	{
		case 16:	// 16 QAM
			reg00H = 0x08;
			reg4AH = 0x0F;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			WriteReg(0x6E, 0x18);
			break;
		case 32:	// 32 QAM
			reg00H = 0x18;
			reg4AH = 0xFB;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			WriteReg(0x6E,0x18);
			break;
		case 64:	// 64 QAM
			reg00H = 0x48;
			reg4AH = 0xCD;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			break;
		case 128:	// 128 QAM
			reg00H = 0x28;
			reg4AH = 0xFF;
			regC2H |= 0x02;
			reg44H = 0xA9;
			reg4CH = 0x08;
			reg4DH = 0xF5;
			reg74H = 0x0E;
			reg8BH = 0x5B;
			reg8EH = 0x9D;
			break;
		case 256:	// 256 QAM
			reg00H = 0x38;
			reg4AH = 0xCD;
			if(((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
			{
				regC2H |= 0x02;
			}
			else
			{
				regC2H |= 0x01;
			}
			reg44H = 0xA9;
			reg4CH = 0x08;
			reg4DH = 0xF5;
			reg74H = 0x0E;
			reg8BH = 0x5B;
			reg8EH = 0x9D;
			break;
		default:	// 64 QAM
			reg00H = 0x48;
			reg4AH = 0xCD;
			regC2H |= 0x02;
			reg44H = 0xAA;
			reg4CH = 0x0C;
			reg4DH = 0xF7;
			reg74H = 0x0E;
			if(((ReadReg(0xE3) & 0x80) == 0x80) && ((ReadReg(0xE4) & 0x80) == 0x80))
			{
				reg8BH = 0x5A;
				reg8EH = 0xBD;
			}
			else
			{
				reg8BH = 0x5B;
				reg8EH = 0x9D;
			}
			break;
	}


	WriteReg(0x00,reg00H);

	value = ReadReg(0x88);
	value |= 0x08;
	WriteReg(0x88, value);
	WriteReg(0x4B, 0xFF);
	WriteReg(0x4A, reg4AH);
	value &= 0xF7;
	WriteReg(0x88, value);

	WriteReg(0xC2, regC2H);
	WriteReg(0x44, reg44H);
	WriteReg(0x4C, reg4CH);
	WriteReg(0x4D, reg4DH);
	WriteReg(0x74, reg74H);
	//WriteReg(0x8B, reg8BH);
	//WriteReg(0x8E, reg8EH);
#else
  _mt_fe_dmd_dc2800_set_QAM(&dc2800_device_handle, qam);
#endif
}


/***********************************************
 Soft reset M88DC2000
 Reset the internal status of each funtion block,
 not reset the registers.
************************************************/
void M88DC2000SoftReset(void)
{
#if 0
	WriteReg(0x80, 0x01);
	WriteReg(0x82, 0x00);
	Sleep(1);
	WriteReg(0x80, 0x00);
#else
mt_fe_dmd_dc2800_soft_reset(&dc2800_device_handle);
#endif
}


/***********************************************
   Set the type of MPEG/TS interface
   type: 1, serial format; 2, parallel format; 0, common interface
************************************************/
void M88DC2000SetTsType(U8 type)
{
#if 0
	U8 regC2H;
	if (type == 0)
	{
		//WriteReg(0x84, 0x6C);
		WriteReg(0xC0, 0x43);
		WriteReg(0xE2, 0x06);
		regC2H = ReadReg(0xC2);
		regC2H &= 0xC7;
		regC2H |= 0x10;
		WriteReg(0xC2,regC2H);
		WriteReg(0xC1,0x60);		//common interface
	}
	else if (type == 1)
	{
		//WriteReg(0x84, 0x6A);
		WriteReg(0xC0, 0x47);		//serial format
		WriteReg(0xE2, 0x02);
		regC2H = ReadReg(0xC2);
		regC2H &= 0xC7;
		WriteReg(0xC2,regC2H);
		WriteReg(0xC1,0x00);
	}
	else
	{
		//WriteReg(0x84, 0x6C);
		WriteReg(0xC0, 0x43);		//parallel format
		WriteReg(0xE2, 0x06);
		regC2H = ReadReg(0xC2);
		regC2H &= 0xC7;
		WriteReg(0xC2,regC2H);
		WriteReg(0xC1,0x00);
	}
#else
if(type == 0)
{
  _mt_fe_dmd_dc2800_set_ts_output(&dc2800_device_handle,MtFeTsOutMode_Common);
}
else if(type == 1)
{
  _mt_fe_dmd_dc2800_set_ts_output(&dc2800_device_handle,MtFeTsOutMode_Serial);

}
else
{
  _mt_fe_dmd_dc2800_set_ts_output(&dc2800_device_handle,MtFeTsOutMode_Parallel);
}

#endif
}

/***********************************************
   Get AGC lock status
   returned 1 when locked;0 when unlocked
************************************************/
U8 M88DC2000GetAgcLock(void)
{
	if ((ReadReg(0x43)&0x08)==0x08)
		return 1;
	else
		return 0;
}

/***********************************************
   Get lock status
   returned 1 when locked;0 when unlocked
************************************************/
U8 M88DC2000GetLock(void)
{
#if 0
	if (ReadReg(0x80) < 0x06)
	{
		if ((ReadReg(0xdf)&0x80)==0x80
			&& (ReadReg(0x91)&0x23)==0x03
			&& (ReadReg(0x43)&0x08)==0x08)
			return 1;
		else
			return 0;
	}
	else
	{
		if(g_p_dc2800_priv->chip_mode != 1)	// 2: old DC2800	0: new DC2800
		{
			if ((ReadReg(0x85)&0x08)==0x08)
				return 1;
			else
				return 0;
		}
		else				// 1: new Jazz
		{
			if ((ReadReg(0x85) & 0x11) == 0x11)
				return 1;
			else
				return 0;
		}
	}
#else
MT_FE_LOCK_STATE is_lock;
mt_fe_dmd_dc2800_get_lock_state(&dc2800_device_handle, &is_lock);
if(is_lock == MtFeLockState_Locked)
{
    return 1;
}
else
{
    return 0;
}
#endif
}

/***********************************************
   Get BER (bit error rate)
************************************************/
DB M88DC2000GetBER(void)
{
	static DB	ber = 0;
#if 0
	static DB	ber = 0;
	U16			tmp;




	if (M88DC2000GetLock() == 0)
	{
		ber = 0;
		return ber;
	}


	#if 1

	if ((ReadReg(0xA0) & 0x80) != 0x80)
	{
		tmp  = ReadReg(0xA2) << 8;
		tmp += ReadReg(0xA1);
		ber  = tmp;
		ber /= 33554432.0;					/*	(2^(2*5+12))*8		*/

		WriteReg(0xA0,0x05);
		WriteReg(0xA0,0x85);
	}

	#else

	WriteReg(0xA0,0x05);
	WriteReg(0xA0,0x85);
	do
	{
		if(M88DC2000GetLock() == 0)
			return 1;
		Sleep(500);
	} while ((ReadReg(0xA0) & 0x80) == 0x80);

	tmp  = ReadReg(0xA2) << 8;
	tmp += ReadReg(0xA1);
	ber  = tmp;
	ber /= 33554432.0;						/*	(2^(2*5+12))*8		*/

	#endif

#else
U32 error_bits;
U32 total_bits;
mt_fe_dmd_dc2800_get_ber(&dc2800_device_handle, &error_bits, &total_bits);
ber = (double)error_bits / total_bits;
#endif
	return ber;
}

/***********************************************
   Get SNR (signal noise ratio)
************************************************/
U8 M88DC2000GetSNR(U16 qam)
{
#if 0
	const U32 mes_log[] =
	{
		0,		3010,	4771,	6021, 	6990,	7781,	8451,	9031,	9542,	10000,
		10414,	10792,	11139,	11461,	11761,	12041,	12304,	12553,	12788,	13010,
		13222,	13424,	13617,	13802,	13979,	14150,	14314,	14472,	14624,	14771,
		14914,	15052,	15185,	15315,	15441,	15563,	15682,	15798,	15911,	16021,
		16128,	16232,	16335,	16435,	16532,	16628,	16721,	16812,	16902,	16990,
		17076,	17160,	17243,	17324,	17404,	17482,	17559,	17634,	17709,	17782,
		17853,	17924,	17993,	18062,	18129,	18195,	18261,	18325,	18388,	18451,
		18513,	18573,	18633,	18692,	18751,	18808,	18865,	18921,	18976,	19031
	};
	U32	snr;
	U8	i;
	U32	mse;





	if ((ReadReg(0x91)&0x23)!=0x03)
		return 0;



	mse = 0;
	for (i=0; i<30; i++)
	{
		mse += (ReadReg(0x08) << 8) + ReadReg(0x07);
	}
	mse /= 30;
	if (mse > 80)
		mse = 80;



	switch (qam)
	{
		case 16:	snr = 34080;	break;	/*	16QAM				*/
		case 32:	snr = 37600;	break;	/*	32QAM				*/
		case 64:	snr = 40310;	break;	/*	64QAM				*/
		case 128:	snr = 43720;	break;	/*	128QAM				*/
		case 256:	snr = 46390;	break;	/*	256QAM				*/
		default:	snr = 40310;	break;
	}


	snr -= mes_log[mse-1];					/*	C - 10*log10(MSE)	*/
	snr /= 1000;
	if (snr > 0xff)
		snr = 0xff;


	return (U8)snr;
#else
  U8 snr;
  mt_fe_dmd_dc2800_get_snr(&dc2800_device_handle, &snr);
  return snr;
#endif
}

/***********************************************
   Get signal strength
************************************************/
U8 M88DC2000GetSignalStrength(void)
{
	U8	SignalStrength = 0;

#if 0
	if ((ReadReg(0x43)&0x08) == 0x08)
	{
		if (((ReadReg(0xE3) & 0xC0) == 0x00) && ((ReadReg(0xE4) & 0xC0) == 0x00))
			SignalStrength = 255 - ((ReadReg(0x55) >> 1) + (ReadReg(0x56) >> 1));
		else
			SignalStrength = 255 - ((ReadReg(0x3B) >> 1) + (ReadReg(0x3C) >> 1));
	}

#else
if(dc2800_device_handle.tuner_settings.tuner_get_strength == NULL)
{
  mt_fe_dmd_dc2800_get_strength(&dc2800_device_handle, &SignalStrength);
}
else
{
  U32 gain;
  U32 strength;
  dc2800_device_handle.tuner_settings.tuner_get_strength(&dc2800_device_handle,&gain, &strength);
  SignalStrength = strength;
}
#endif
	return SignalStrength;
}
