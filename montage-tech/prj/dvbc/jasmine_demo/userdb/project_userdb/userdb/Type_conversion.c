/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include <windows.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <locale.h> 

#include "Type_conversion.h" 
#include "../interface/userdb_native_interface.h"

int charTowchar(wchar_t *dest ,const char *src)
{
	if((dest == NULL) || (src == NULL))
	{
	  return 0;
	}
    memset(dest,'\0',256);
    setlocale(LC_ALL,"zh_CN.UTF-8");
    mbstowcs(dest,src,256);  

	return wcslen(dest);
}

//function: wcharTochar  size chr size
//need to modify
int wcharTochar(char *dest ,const wchar_t *src) 
{   
   if((dest == NULL) || (src == NULL))
	{
	  return 0;
	}
   memset(dest,'\0',256);
   wcstombs( dest, src, 40 );
   
   return strlen(dest);
}   

int wcharToint(const wchar_t *wchar)   
{
  int i = 0;
  int num = 0;
  wchar_t chr[256];
 
  memset(chr,'\0',256);
  num = wcslen(wchar);

   memcpy(chr,wchar,num*2);
   while(chr[i]!= '\0')
   {
     if(chr[i] == '.')
	{
	 break;
	} 
	 i ++;
   }
   for(; i < num; i++)
   {
    chr[i] = chr[i + 1];
   }
  return  _wtoi(chr);
}
//need to modify
int intTowchar(wchar_t *dest ,const int cint)   
{
	wchar_t buf[20];

	if((dest == NULL))
	{
	  return 0;
	}
	memset(dest,'\0',256);
	_itow(cint,buf,10);
   
	memcpy(dest,buf,wcslen(buf)*2);

   return wcslen(dest);
}
//need to modify
int checklongitude(wchar_t *dest ,const wchar_t *src)   
{   
  int i = 0, j = 0, g = 0;
  int num = 0;
  wchar_t chr[256];

    if((dest == NULL) || (src == NULL))
	{
	  return 0;
	}
   memset(chr,'\0',256);
   num = wcslen(src);
   memcpy(chr,src,num*2);
  
   for(i = 0;i < num; i ++)
   {
    if(chr[i]=='.')
	{
		j = i + 1;
	  break;
	}	
   }
   for(j;j > 0 && j < num; j ++)
   {
	 if(g > 1)
	 {
	   chr[j] = '\0';
	 }
	 g ++;
   }
   if(g == 0)
   {
   wcscat(chr,L".00");
   }
   else if( g == 1)
   {
    wcscat(chr,L"0");
   }

	memset(dest,'\0',256);
    memcpy(dest,chr,wcslen(chr)*2);
   return wcslen(dest);
} 

//
int curelongitude(wchar_t *dest ,const wchar_t *src)      
{
  int i = 0, j = 0;
  int num = 0;
  wchar_t chr[256];
 
  memset(chr,'\0',256);
  num = wcslen(src);

   for(i = 0; i < num; i++)
   {
    if(i == num - 2)
	{
	 chr[i] = '.';
	 j =1;
	}
    chr[i+ j] = src[i];
   }
   memset(dest,'\0',256);
   memcpy(dest,chr,wcslen(chr)*2);
  return  wcslen(dest);
}

int findfavflag(wchar_t **dest ,const wchar_t *src)      
{
  int i = 0, j = 0, g = 0,ret = 0;
  int num = 0;
  wchar_t chr[256];
  wchar_t pg_fav[8][20] = {'\0'};

    if(src == NULL)
	{
	  return 0;
	}
   memset(chr,'\0',256);
   num = wcslen(src);
   memcpy(chr,src,num*2);
  
   for(i = 0;i < num; i ++)
   {
    if(chr[i]==';')
	{   
		j ++;
		g = 0;
	 
	}
	else
	{
	 pg_fav[j][g++] = chr[i];
	}
   }

   memcpy(dest,pg_fav,sizeof(pg_fav));
   if(num == 0)
   {
	 ret = 0;
   }
   else
   {
    ret = j + 1;
   }

   return ret;
}

//
int find_file_type(char *path, char *parameter)      
{
  char Type[30];
  int type = 0;
  int i = 0,len = 0,j = 0;

  len = strlen(parameter);
  for(i = 0; i < len; i ++)
  {
	path[i] = parameter[i];
    if(parameter[i] == ';')
    {
	    path[i] = '\0';
      strcpy(Type,parameter + i + 1);
	  break;
    }
  } 
  if(strcmp(Type,"FLASH") == 0)
  {
    type = FLASH;
  }
  else
  {
    type = USER_DB;
  }  
  return type;
}