/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
// project_userdb_unittest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "project_userdb_unittest.h"
//#include "../project_userdb/interface/userdb_native_interface.h"
extern "C"
{
#include "../project_userdb/interface/userdb_native_interface.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FLASH_SIZE	(4 * 1024 * 1024)
// The one and only application object

CWinApp theApp;

using namespace std;
NativePropertyTable *sat_table,*tp_table,*pg_table;
long int size;
 wchar_t key[SHORTLENGTH];
 int key_len;
 
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	FILE *p_file = NULL;
	//init buf 
  unsigned  char* buff = (unsigned char *)malloc(FLASH_SIZE);
    memset(buff, 0, FLASH_SIZE);
   
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		//p_file = fopen("D:\\work\\project_userdb\\project_userdb_unittest\\flash.bin","rb");
	    //if (p_file == NULL)
	    //{
		//  AfxMessageBox(L"Open the input file failed!");
	    //}
        //fread(buff, 1, FLASH_SIZE, p_file);FLASH USER_DB
		char* parameter1 = "D:\\user_db.bin;USER_DB";
		char* parameter = "D:\\flash.bin;FLASH";
    ProjectInitial(parameter1,OPEN);
	
	Parse( buff, size,FLASH, &sat_table, &tp_table, &pg_table);

    ProjectInitial(parameter,SAVE);
	ReOrder(sat_table, tp_table, pg_table,FLASH, buff,  &size);	
	 ProjectInitial(parameter,OPEN);
	
	Parse( buff, size,FLASH, &sat_table, &tp_table, &pg_table);
	Clean();
#if 0
	struct NativePropertyArray *pp =	SatTemplate();
	 for(int i =0; i < 2; i++)
    {
	   key_len = wcslen(pp->first[i].key);
       memcpy(key,pp->first[i].key,key_len*4);
    }
#endif
       // free(buff);
	}

	return nRetCode;
}
