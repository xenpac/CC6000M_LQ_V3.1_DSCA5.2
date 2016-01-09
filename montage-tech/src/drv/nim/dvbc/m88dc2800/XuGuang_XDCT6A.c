#include "sys_types.h"
#include "mtos_misc.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "i2c.h"
#include "nim.h"
#include "mt_fe_def.h"
#include "nim_m88dc2800.h"

//static s32 XDCT6A_Init()
//{
//  return 0;
//}

//static u32 XDCT6A_GetSignalStrength ( void )
//{
//	return 0;
//}

#if 0
static s32 XDCT6A_Set(MT_FE_DC2800_Device_Handle handle ,u32 freq_KHz, nim_modulation_t nqam)
{
  s32 ret = 0;
  U8	tuner_reg[5];
  U16 N;

  N = (U16)((freq_KHz + 36000) * 100 / 16667);	/*	step:	166.67 KHz	*/
  tuner_reg[0] = (U8)((N >> 8) & 0x7f);
  tuner_reg[1] = (U8)(N &0xff);
  tuner_reg[2] = 0xA0;


  if(freq_KHz <= 151000)
    tuner_reg[3] = 0x41;
  else if(freq_KHz <= 447000)
    tuner_reg[3] = 0x42;
  else
    tuner_reg[3] = 0x48;

  tuner_reg[4] = 0xC0;

  ret = dc2800_tn_seq_write(handle->tuner_settings.tuner_dev_addr, tuner_reg, 5);

  return ret;
}
#endif

extern MT_FE_DC2800_SUPPORTED_TUNER dc2800_tuner_type;

void tuner_attach_XG_XDCT6A(void)
{
#if 0
  //OS_PRINTF ("attach TN_XDCT6A\n");
  g_dc2800_tn_drv.init = XDCT6A_Init;
  g_dc2800_tn_drv.get_signal_strength = XDCT6A_GetSignalStrength;
  g_dc2800_tn_drv.set_freq = XDCT6A_Set;
  g_dc2800_tn_drv.tn_id = TN_XDCT6A;
#else
  dc2800_tuner_type = TN_XUGUANG_XDCT6A;
#endif
}

extern MT_FE_DC2800_Device_Handle dc_2800_get_handle();
void tuner_attach_XG_XDCT6A_only_self(void)
{
  MT_FE_DC2800_Device_Handle p_handle = dc_2800_get_handle();

  p_handle->tuner_settings.tuner_type = TN_XUGUANG_XDCT6A;

  p_handle->tuner_settings.tuner_init			 = mt_fe_dc2800_tn_init_XuGuang_XDCT6A;
  p_handle->tuner_settings.tuner_set			 = mt_fe_dc2800_tn_set_freq_XuGuang_XDCT6A;
  p_handle->tuner_settings.tuner_get_offset		 = NULL;
  p_handle->tuner_settings.tuner_get_strength	 = NULL;

  p_handle->tuner_settings.tuner_sleep			 = NULL;
  p_handle->tuner_settings.tuner_wakeup			 = NULL;
  p_handle->tuner_settings.tuner_get_version	 = mt_fe_dc2800_tn_get_version_default;
}