/******************************************************************************
 *
 * Filename: M88DC2800_TDCC_G051F.c
 *
 * Create  : tianxf
 * Date    : 2008-10-30
 *
******************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mem_manager.h"
#include "mtos_mem.h"

#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "i2c.h"
#include "nim.h"
#include "../../nim_priv.h"

#include "mt_fe_def.h"
#include "nim_m88dc2800.h"

#define EACH_FREQ_SAMPLE_NUM 7
#define SYS_TIMEOUT_INFINITY      0xffffffff

static u32 Freq = 0;

static U8 HIBYTE ( U16 val )
{
    U8 i = 0;

    i = ( U8 ) ( ( val >> 8 ) & 0xff );

    return ( i );
}

static U8 LOBYTE ( U16 val )
{
    U8 i = 0;

    i = ( U8 ) ( val & 0xff );

    return ( i );
}

U16 LOWORD ( u32 val32 )
{
    U16 i = 0;

    i = ( U16 ) ( val32 & 0xffff );

    return ( i );
}

U16 HIWORD ( u32 val32 )
{
    U16 i = 0;

    i = ( U16 ) ( ( val32 >> 16 ) & 0xffff );

    return ( i );
}


static struct
{
    S32 FreqKhz;
    struct
    {
        S32 Reg;
        S32 Strength;
    }Sample[EACH_FREQ_SAMPLE_NUM];
}StrengthLUT[] =
{
    //外面一圈的参数都是避免逻辑错误用的
    //freq   |      samples
    {     0,{{0,25},{193,26},{196,32},{201,39},{207,49},{209,51},{255,60}}},
    {107000,{{0,25},{193,26},{196,32},{201,39},{207,49},{209,51},{255,60}}},
    {155000,{{0,25},{193,30},{195,35},{203,48},{206,53},{209,57},{255,60}}},
    {299000,{{0,25},{193,30},{199,37},{203,43},{206,47},{209,60},{255,60}}},
    {387000,{{0,25},{195,32},{199,38},{203,44},{207,50},{209,53},{255,60}}},
    {451000,{{0,25},{193,30},{200,33},{203,39},{208,45},{209,50},{255,60}}},
    {602000,{{0,25},{195,30},{199,37},{201,40},{203,43},{209,55},{255,60}}},
    {858000,{{0,25},{195,29},{199,37},{201,43},{203,46},{209,55},{255,60}}},
    {858000,{{0,25},{195,29},{199,37},{201,43},{203,46},{209,55},{255,60}}}
};

#define FREQ_SAMPLE_NUM (sizeof(StrengthLUT)/sizeof(StrengthLUT[0]))


void TunerDelayMs ( u32 u32Ms )
{
    mtos_task_sleep ( u32Ms );
}


static S32 LinearInterpolation(S32 x, S32 x1, S32 y1, S32 x2, S32 y2)
{
    S32 y;
    if(x1 == x2)
    {
        y = y1/2 + y2/2;
    }
    else
    {
        y =    (
                (x - x1) * y2
                +
                (x2 - x) * y1
            )
            /
            (x2 - x1);
    }
    return y;
}

static u32 M88DC2000CalcSignalStrength(U8 StrengthRegister)
{
//查表+线性插值
    U8 s  = 0;
    S32 s1 = 0;
    S32 s2 = 0;
    S32 f1 = 0;
    S32 f2 = 0;
    S32 r1 = 0;
    S32 r2 = 0;
    //简单起见，找一个大于等于当前频点的采样作参考
    for(f2 = 1; f2 < FREQ_SAMPLE_NUM; ++f2)
    {
        if(StrengthLUT[f2].FreqKhz >= Freq)
        {
            break;
        }
    }
    if(f2 == FREQ_SAMPLE_NUM)
    {
        f2 = FREQ_SAMPLE_NUM - 1;
    }
    f1 = f2 - 1;
    for(r1 = 1; r1 < EACH_FREQ_SAMPLE_NUM; ++r1)
    {
        if(StrengthLUT[f1].Sample[r1].Reg >= StrengthRegister)
        {
            break;
        }
    }
    for(r2 = 1; r2 < EACH_FREQ_SAMPLE_NUM; ++r2)
    {
        if(StrengthLUT[f2].Sample[r2].Reg >= StrengthRegister)
        {
            break;
        }
    }
    s1 = LinearInterpolation(StrengthRegister,
                             StrengthLUT[f1].Sample[r1-1].Reg, StrengthLUT[f1].Sample[r1-1].Strength,
                             StrengthLUT[f1].Sample[r1].Reg, StrengthLUT[f1].Sample[r1].Strength);
    s2 = LinearInterpolation(StrengthRegister,
                             StrengthLUT[f2].Sample[r2-1].Reg, StrengthLUT[f2].Sample[r2-1].Strength,
                             StrengthLUT[f2].Sample[r2].Reg, StrengthLUT[f2].Sample[r2].Strength);
    s  = LinearInterpolation(Freq,
                             StrengthLUT[f1].FreqKhz, s1,
                             StrengthLUT[f2].FreqKhz, s2);
    return s;
}

/***********************************************
 * get the signal intensity expressed in percentage
 * param:   no
 * return:  The signal Strength value (Range is [0,100])
************************************************/
u32 TDCC_G051F_GetSignalStrength ( MT_FE_DC2800_Device_Handle handle )
{
    U8 SignalStrength = 0;

    if ( ( dc2800_dmd_reg_read (handle,0x43) &0x08 ) == 0x08 )
    {
        if (((dc2800_dmd_reg_read (handle,0xE3) & 0xC0) == 0x00)
            && ((dc2800_dmd_reg_read (handle,0xE4) & 0xC0) == 0x00))
        {
            SignalStrength = 255 - (( dc2800_dmd_reg_read (handle,0x55 ) >> 1 ) 
              + ( dc2800_dmd_reg_read(handle,0x56 ) >> 1 ) );
        }
        else
        {
            SignalStrength = 255 - ( ( dc2800_dmd_reg_read (handle,0x3B ) >> 1 ) 
            + ( dc2800_dmd_reg_read (handle,0x3C ) >> 1 ) );
        }
    }
    else
    {
        return 0;
    }

    return  M88DC2000CalcSignalStrength(SignalStrength);
}


s32 TDCC_G051F_Set(MT_FE_DC2800_Device_Handle handle ,u32 freq_KHz, nim_modulation_t nqam)
{
    U8 tdata[5] = {0};
    double Fosc = freq_KHz + 36125;
    U16 N = ( U16 ) ( Fosc / 62.5 );    /* step 62.5 KHz */

    tdata[0] = HIBYTE ( N ) & 0x7F;
    tdata[1] = LOBYTE ( N );
    tdata[2] = 0x93;
        
    if (freq_KHz <= 148000)
    {
        tdata[3] = 0x01;
    }
    else if (freq_KHz <= 430000)
    {
        tdata[3] = 0x02;
    }
    else
    {
        tdata[3] = 0x08;
    }
    
    tdata[4] = 0xc3;

    dc2800_tn_seq_write(handle,tdata, 5);
    
    TunerDelayMs(10);

    return 0;
}

//static s32 TDCC_G051F_Init()
//{
//  return 0;
//}

extern MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type;
void tuner_attach_TDCC_G051F(void)
{
#if 0
  //OS_PRINTF ("attach TDCC_G051F\n");
  g_dc2800_tn_drv.init = TDCC_G051F_Init;
  g_dc2800_tn_drv.get_signal_strength = TDCC_G051F_GetSignalStrength;
  g_dc2800_tn_drv.set_freq = TDCC_G051F_Set;
  g_dc2800_tn_drv.tn_id = TN_TDCC_G051F;
#else
  dc2800_tuner_type = TN_TDCCG051F;
#endif
}

extern MT_FE_DC2800_Device_Handle dc_2800_get_handle();
void tuner_attach_TDCC_G051F_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_TDCCG051F;

  p_handle->tuner_settings.tuner_init             = mt_fe_dc2800_tn_init_TDCC_G051F;
  p_handle->tuner_settings.tuner_set             = mt_fe_dc2800_tn_set_freq_TDCC_G051F;
  p_handle->tuner_settings.tuner_get_offset         = NULL;
  p_handle->tuner_settings.tuner_get_strength     = mt_fe_dc2800_tn_get_strength_TDCC_G051F;

  p_handle->tuner_settings.tuner_sleep             = NULL;
  p_handle->tuner_settings.tuner_wakeup             = NULL;
  p_handle->tuner_settings.tuner_get_version     = mt_fe_dc2800_tn_get_version_default;
}