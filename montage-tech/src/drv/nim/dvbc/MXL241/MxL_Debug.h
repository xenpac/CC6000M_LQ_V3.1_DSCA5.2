/*******************************************************************************
 *
 * FILE NAME          : MxL_Debug.h
 * 
 * AUTHOR             : David Zhou
 *                      Ron Green - Added MxL_VFC_DEBUG for the MxL Color 
 *                                  Formater filter.
 *
 * DATE CREATED       : 2/7/2008
 *                      9/15/2008
 *
 * DESCRIPTION        : Debug header files 
 *
 *******************************************************************************
 *                Copyright (c) 2008, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MXL_DEBUG_H__
#define __MXL_DEBUG_H__


/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "MaxLinearDataTypes.h"

/******************************************************************************
    Macros
******************************************************************************/
#define MXL_DBG 0

#if MXL_DBG
#define MxL_DLL_DEBUG0 mtos_printk
#else
#define MxL_DLL_DEBUG0(...) do{}while(0)
#endif

#endif /* __MXL_DEBUG_H__ */