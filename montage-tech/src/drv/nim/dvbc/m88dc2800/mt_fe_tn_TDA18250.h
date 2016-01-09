/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef _MT_FE_TN_TDA18250_H
#define _MT_FE_TN_TDA18250_H

#ifdef __cplusplus
extern "C"
{
#endif

#include ".\TDA18250\tmNxTypes.h"
#include ".\TDA18250\tmCompId.h"
#include ".\TDA18250\tmFrontEnd.h"
#include ".\TDA18250\tmbslFrontEndTypes.h"

#include ".\TDA18250\tmddTDA18250.h"

#include ".\TDA18250\tmUnitParams.h"
#include ".\TDA18250\tmddTDA18250local.h"
#include ".\TDA18250\tmbslTDA18250local.h"
#include ".\TDA18250\tmddTDA18250Instance.h"
#include ".\TDA18250\tmbslTDA18250InstanceCustom.h"
#include ".\TDA18250\tmddTDA18250InstanceCustom.h"
#include ".\TDA18250A\inc\tmCompId.h"
#include ".\TDA18250A\inc\tmNxTypes.h"
#include ".\TDA18250A\inc\tmFrontEnd.h"
#include ".\TDA18250A\inc\tmbslFrontEndCfgItem.h"
#include ".\TDA18250A\inc\tmbslFrontEndTypes.h"

// Components includes
#include ".\TDA18250A\tmbslTDA18250A\tmbslTDA18250A.h"

#include "mt_fe_def.h"

tmErrorCode_t tmbslTDA18250Init(tmUnitSelect_t tUnit, tmbslFrontEndDependency_t* psSrvFunc);
tmErrorCode_t tmbslTDA18250SetPowerState(tmUnitSelect_t tUnit, tmTDA18250PowerState_t powerState);
tmErrorCode_t TDA18250SetStandardMode(tmUnitSelect_t tUnit, tmTDA18250StandardMode_t StandardMode);
tmErrorCode_t tmbslTDA18250SetRf(tmUnitSelect_t  tUnit, UInt32 uRF);
tmErrorCode_t tmbslTDA18250GetLockStatus(tmUnitSelect_t tUnit, tmbslFrontEndState_t* pLockStatus);

MT_FE_RET mt_fe_tn_init_TDA18250(void *dev_handle);
MT_FE_RET mt_fe_tn_set_freq_TDA18250(void *dev_handle, U32 freq, U16 symbol_rate);
MT_FE_RET mt_fe_tn_get_signal_strength_TDA18250(void *dev_handle,  U32 * p_gain, U32 * p_strength);

MT_FE_RET mt_fe_tn_init_TDA18250A(void *dev_handle);
MT_FE_RET mt_fe_tn_set_freq_TDA18250A(void *dev_handle, U32 freq, U16 symbol_rate);
MT_FE_RET mt_fe_tn_get_signal_strength_TDA18250A(void *dev_handle,  U32 * p_gain, U32 * p_strength);
#ifdef __cplusplus
};
#endif

#endif
