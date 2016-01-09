/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*
 * Filename:        mt_fe_i2c.c
 *
 * Description:     2-wire bus functions for FE module.
 *
 *
 * Author:          YZ.Huang
 *
 * Version:         1.01.00
 * Date:            2014-12-23

 * History:
 * Description      Version     Date            Author
 *---------------------------------------------------------------------------
 * File Create      1.01.00     2012.02.28      YZ.Huang
 *---------------------------------------------------------------------------
 ***************************************************************************/
#include "mt_fe_def.h"
#include "mtos_task.h"
#include "hal_base.h"
#include "mt_fe_tn_tc3800.h"

extern u8 dc2800_dmd_reg_read
    (MT_FE_DC2800_Device_Handle p_handle, u8 reg);
extern void dc2800_tn_reg_write
    (MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
extern u8 dc2800_tn_reg_read
    (MT_FE_DC2800_Device_Handle p_handle, u8 reg);
extern void dc2800_tn_reg_write
    (MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
extern void dc2800_dmd_reg_write
    (MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
extern s32 dc2800_tn_seq_write
    (MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 len);
extern s32 dc2800_tn_seq_read
    (MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen);
/*****************************************************************
** Function: _mt_fe_dmd_set_reg
**
**
** Description:    write data to demod register
**
**
** Inputs:
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    reg_index        U8            register index
**    data            U8            value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_set_reg(void *dev_handle, U8 reg_index, U8 data)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

   dc2800_dmd_reg_write(handle, reg_index, data);
    /*
        TODO:
            Obtain the i2c mutex
    */



    /*
        TODO:
            write data to demodulator register
    */



    /*
        TODO:
            Release the i2c mutex
    */



    return MtFeErr_Ok;
}


/*****************************************************************
** Function: _mt_fe_dmd_get_reg
**
**
** Description:    read data from demod register
**
**
** Inputs:
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    reg_index        U8            register index
**
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    p_buf            U8*            register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;

   *p_buf = dc2800_dmd_reg_read(handle, reg_index);
    /*
        TODO:
            Obtain the i2c mutex
    */



    /*
        TODO:
            read demodulator register value
    */



    /*
        TODO:
            Release the i2c mutex
    */


    return MtFeErr_Ok;
}


/*****************************************************************
** Function: _mt_fe_tn_set_reg
**
**
** Description:    write data to tuner register
**
**
** Inputs:
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    reg_index        U8            register index
**    data            U8            value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_set_reg(void *dev_handle, U8 reg_index, U8 data)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
    //MT_FE_RET ret = MtFeErr_I2cErr;
    //U8 value;

    if(handle == NULL)
    {
        return MtFeErr_Uninit;
    }

    dc2800_tn_reg_write(handle, reg_index, data);
#if 0

    /*
        TODO:
            Obtain the i2c mutex
    */


    /*open I2C repeater*/
    /*Do not care to close the I2C repeater, it will close by itself*/
    if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
    {
        _mt_fe_dmd_get_reg(handle, 0x87, &value);
        value &= 0x0F;
        value |= 0x90;    // bit7         = 1, Enable I2C repeater
                        // bit[6:4]     = 1, Enable I2C repeater for 1 time
        _mt_fe_dmd_set_reg(handle, 0x87, value);
    }
    else if(handle->on_board_settings.chip_mode == 2)
    {
        _mt_fe_dmd_get_reg(handle, 0x86, &value);
        value |= 0x80;
        _mt_fe_dmd_set_reg(handle, 0x86, value);
    }
    else
    {
        return MtFeErr_NoSupportFunc;
    }


    /*Do not sleep any time after I2C repeater is opened.*/
    /*please set tuner register at once.*/


    /*
        TODO:
            write value to tuner register
    */



    /*
        TODO:
            Release the i2c mutex
    */

#endif

    return MtFeErr_Ok;
}




/*****************************************************************
** Function: _mt_fe_tn_get_reg
**
**
** Description:    get tuner register data
**
**
** Inputs:
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    register        U8            register address
**
**
** Outputs:
**
**    Parameter        Type        Description
**    ----------------------------------------------------------
**    p_buf            U8*            register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_get_reg(void *dev_handle, U8 reg_index, U8 *p_buf)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
    //MT_FE_RET    ret = MtFeErr_I2cErr;
    //U8 value;

    if(handle == NULL)
    {
        return MtFeErr_Uninit;
    }

    *p_buf = dc2800_tn_reg_read(handle, reg_index);
#if 0
    /*
        TODO:
            Obtain the i2c mutex
    */


    /*open I2C repeater*/
    /*Do not care to close the I2C repeater, it will close by itself*/
    if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
    {
        _mt_fe_dmd_get_reg(handle, 0x87, &value);
        value &= 0x0F;
        value |= 0xA0;    // bit7         = 1, Enable I2C repeater
                        // bit[6:4]     = 2, Enable I2C repeater for 2 times if there're 2 stops after the repeater
                        // bit[6:4]  = 1, Enable I2C repeater for 1 time if there's only 1 stop after the repeater
        _mt_fe_dmd_set_reg(handle, 0x87, value);
    }
    else if(handle->on_board_settings.chip_mode == 2)
    {
        _mt_fe_dmd_get_reg(handle, 0x86, &value);
        value |= 0x80;
        _mt_fe_dmd_set_reg(handle, 0x86, value);
    }
    else
    {
        return MtFeErr_NoSupportFunc;
    }
    /*Do not sleep any time after I2C repeater is opened.*/
    /*please read tuner register at once.*/


    /*
        TODO:
            read tuner register value
    */


    /*
        TODO:
            Release the i2c mutex
    */

#endif
    return MtFeErr_Ok;
}



MT_FE_RET _mt_fe_tn_write(void *dev_handle, U8 *p_buf, U16 n_byte)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
#if 1
   dc2800_tn_seq_write(handle, p_buf, n_byte);
#else
    //MT_FE_RET    ret = MtFeErr_Ok;
    U8            value;

    if(handle == NULL)
    {
        return MtFeErr_Uninit;
    }

    /*
        TODO:
            Obtain the i2c mutex
    */


    /*open I2C repeater*/
    /*Do not care to close the I2C repeater, it will close by itself*/
    if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
    {
        _mt_fe_dmd_get_reg(handle, 0x87, &value);
        value &= 0x0F;
        value |= 0x90;    // bit7         = 1, Enable I2C repeater
                        // bit[6:4]     = 1, Enable I2C repeater for 1 time
        _mt_fe_dmd_set_reg(handle, 0x87, value);
    }
    else if(handle->on_board_settings.chip_mode == 2)
    {
        _mt_fe_dmd_get_reg(handle, 0x86, &value);
        value |= 0x80;
        _mt_fe_dmd_set_reg(handle, 0x86, value);
    }
    else
    {
        return MtFeErr_NoSupportFunc;
    }

    /*Do not sleep any time after I2C repeater is opened.*/
    /*please write N bytes to register at once.*/


    /*
        TODO:
            write N bytes to tuner
    */



    /*
        TODO:
            Release the i2c mutex
    */

#endif

    return MtFeErr_Ok;
}


MT_FE_RET _mt_fe_tn_read(void *dev_handle, U8 *p_buf, U16 n_byte)
{
    MT_FE_DC2800_Device_Handle handle = (MT_FE_DC2800_Device_Handle)dev_handle;
#if 1
    dc2800_tn_seq_read(handle, p_buf, 0, n_byte);
#else
    //MT_FE_RET    ret = MtFeErr_Ok;
    U8            value;

    /*
        TODO:
            Obtain the i2c mutex
    */


    /*open I2C repeater*/
    /*Do not care to close the I2C repeater, it will close by itself*/
    if((handle->on_board_settings.chip_mode == 0) || (handle->on_board_settings.chip_mode == 1))
    {
        _mt_fe_dmd_get_reg(handle, 0x87, &value);
        value &= 0x0F;
        value |= 0x90;    // bit7         = 1, Enable I2C repeater
                        // bit[6:4]     = 1, Enable I2C repeater for 1 time
        _mt_fe_dmd_set_reg(handle, 0x87, value);
    }
    else if(handle->on_board_settings.chip_mode == 2)
    {
        _mt_fe_dmd_get_reg(handle, 0x86, &value);
        value |= 0x80;
        _mt_fe_dmd_set_reg(handle, 0x86, value);
    }
    else
    {
        return MtFeErr_NoSupportFunc;
    }


    /*Do not sleep any time after I2C repeater is opened.*/
    /*please read N bytes.*/


    /*
        TODO:
            write N bytes to tuner
    */



    /*
        TODO:
            Release the i2c mutex
    */

#endif
    return MtFeErr_Ok;
}


void _mt_sleep(U32 ticks_ms)
{
    /*TODO*/

    /*
        Wait for ticks_ms time, the time unit is millisecond.
    */
  mtos_task_sleep(ticks_ms);
}

MT_FE_RET _mt_fe_write32(U32 reg_addr, U32 reg_data)
{
         hal_put_u32((volatile unsigned long *)reg_addr, reg_data);
	return MtFeErr_Ok;
}


MT_FE_RET _mt_fe_read32(U32 reg_addr, U32 *p_data)
{
         *p_data = hal_get_u32((volatile unsigned long *)reg_addr);
	return MtFeErr_Ok;
}

S32 _mt_fe_tn_get_reg_tc3800(MT_FE_Tuner_Handle_TC3800 handle, U8 reg_addr, U8 *reg_data)
{
        MT_FE_DC2800_Device_Handle h = (MT_FE_DC2800_Device_Handle)handle->p_private;
        return _mt_fe_tn_get_reg(h, reg_addr, reg_data);
}


S32 _mt_fe_tn_set_reg_tc3800(MT_FE_Tuner_Handle_TC3800 handle, U8 reg_addr, U8 reg_data)
{
        MT_FE_DC2800_Device_Handle h = (MT_FE_DC2800_Device_Handle)handle->p_private;
        return _mt_fe_tn_set_reg(h, reg_addr, reg_data);

}

S32 _mt_fe_tn_set_reg_bit_tc3800(MT_FE_Tuner_Handle_TC3800 handle, U8 reg_addr, U8 data, U8 high_bit, U8 low_bit)
{
	U8 tmp = 0, value = 0;

	if(high_bit < low_bit)
	{
		return -1;
	}


	data <<= (7 + low_bit - high_bit);
	data &= 0xFF;
	data >>= (7 - high_bit);
	data &= 0xFF;

	tmp = 0xFF;
	tmp <<= (7 + low_bit - high_bit);
	tmp &= 0xFF;
	tmp >>= (7 - high_bit);
	tmp &= 0xFF;

	_mt_fe_tn_get_reg_tc3800(handle, reg_addr, &value);
	value &= ~tmp;
	value |= data;
	_mt_fe_tn_set_reg_tc3800(handle, reg_addr, value);

	return 0;
}

