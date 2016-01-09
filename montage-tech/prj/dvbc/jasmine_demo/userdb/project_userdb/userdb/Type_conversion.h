/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

int charTowchar(wchar_t *dest ,const char *src);
int wcharTochar(char *dest ,const wchar_t *src); 
int wcharToint(const wchar_t *wchar);
int intTowchar(wchar_t *dest ,const int * cint);   
int checklongitude(wchar_t *dest ,const wchar_t *wchar);
int curelongitude(wchar_t *dest ,const wchar_t *src);
int find_file_type(char *path, char *parameter);  
