/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef INC_USERDB_NATIVE_INTERFACE_H_
#define INC_USERDB_NATIVE_INTERFACE_H_

#include <wchar.h>

struct NativePropertyArray;

typedef int (*pf_Validation)(struct NativeProperty*, struct NativePropertyArray *);
typedef int (*pf_Compare)(struct NativeProperty*, struct NativeProperty*);

#define SHORTLENGTH 40
#define MIDDLELENGTH 100
#define LONGLENGTH 200

typedef struct NativeProperty
{
  wchar_t key[SHORTLENGTH];
  wchar_t value[MIDDLELENGTH];
  wchar_t group[SHORTLENGTH];
  int editable;
  wchar_t* options;             //combine all the option strings into one. 
  unsigned int options_len;
  // And split the options with semicolon(";")
  // E.g. LNB type's options is "Standard;User;Universal"
  pf_Validation m_validation;
  pf_Compare m_compare;
} NativeProperty;

typedef struct NativePropertyArray
{
  unsigned int size;
  unsigned int id;
  unsigned int parent_id;
  NativeProperty* first;
} NativePropertyArray;

typedef struct NativePropertyTable{
  unsigned int size;
  NativePropertyArray** first;
} NativePropertyTable;

typedef enum {
  FLASH = 1,
  USER_DB
} Type;

typedef enum {
  OPEN = 1,
  SAVE
} Proinit_Type;



// interface the project should implement
__declspec(dllexport) int ProjectInitial(char* parameter, Proinit_Type type);  // the parameter is a path to the userdb related file
__declspec(dllexport) int Parse( unsigned char* buff, long size, Type type, struct NativePropertyTable** SATtable, struct NativePropertyTable** TPtable, struct NativePropertyTable** PGtable);
__declspec(dllexport) int ReOrder( struct NativePropertyTable* SATtable, struct NativePropertyTable* TPtable, struct NativePropertyTable* PGtable, Type type, unsigned char* buff, long* size);
__declspec(dllexport) int Clean();

__declspec(dllexport)  NativePropertyArray* SatTemplate();
__declspec(dllexport)  NativePropertyArray* TPTemplate();
__declspec(dllexport)  NativePropertyArray* PGTemplate();

#endif  // INC_USERDB_NATIVE_INTERFACE_H_