/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
// systemconfigsoclib.h : main header file for the systemconfigsoclib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CsystemconfigsoclibApp
// See systemconfigsoclib.cpp for the implementation of this class
//

class CsystemconfigsoclibApp : public CWinApp
{
public:
	CsystemconfigsoclibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
