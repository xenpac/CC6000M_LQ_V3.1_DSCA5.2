/*******************************************************************************
 *
 * FILE NAME          : MxL_Debug.h
 * 
 * AUTHOR             : Brenndon Lee
 *
 * DATE CREATED       : 10/25/2010
 *
 * DESCRIPTION        : Debug header files 
 *
 *******************************************************************************
 *                Copyright (c) 2010, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MXL_DEBUG_H__
#define __MXL_DEBUG_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include <stdio.h>
#include "MaxLinearDataTypes.h"

/******************************************************************************
    Macros
******************************************************************************/
//#define MXL_DBG
#if MXL_DBG
#define MxL_DLL_DEBUG0 mtos_printk
#else
#define MxL_DLL_DEBUG0(...) do{}while(0)
#endif

#endif /* __MXL_DEBUG_H__*/
