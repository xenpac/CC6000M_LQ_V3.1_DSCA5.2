/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/


#include "emon_cmd.h"
#include "cmd_entry.h"

#include "cmd_system.h"
#include "cmd_ir.h"
#include "cmd_dmx.h"
#ifdef ONLY1_CA_VER
#include "cmd_cas.h"
#endif
/******************************************************************************/



int emon_entry_init(void)
{
  int ret = SUCCESS;

  ret |= cmd_sys_init();
  ret |= cmd_ir_init();
  ret |= cmd_dmx_init();
#ifdef  ONLY1_CA_VER
  ret |= cmd_cas_init();
#endif
  return ret;
}


