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
typedef int (*pf_Enable)(struct NativeProperty*, struct NativePropertyArray *);

#define SHORTLENGTH 30
#define MIDDLELENGTH 100
#define LONGLENGTH 200

typedef struct NativeProperty
{
  wchar_t key[SHORTLENGTH];
  wchar_t value[MIDDLELENGTH];
} NativeProperty;

typedef enum {
  DBI_UNKNOWN, DBI_SAT, DBI_TP, DBI_PG
} DBI_TYPE;

typedef struct NativePropertyUICfg {
  DBI_TYPE type;
  wchar_t Name[SHORTLENGTH];
  wchar_t default_val[MIDDLELENGTH];
  int hasEnabler;
  int isOpen;
  wchar_t* group_name;
  unsigned char group_name_len;
  wchar_t* options;     //combine all the option strings into one.
                        // And split the options with semicolon(";")
                        // E.g. LNB type's options is "Standard;User;Universal"
  unsigned int options_len;
  pf_Enable m_enabler;
  pf_Validation m_validation;
  pf_Compare m_compare;
}NativePropertyUICfg;

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

typedef struct NativePropertyUICfgArray {
  unsigned int size;
  NativePropertyUICfg* first;
} NativePropertyUICfgArray;

typedef struct NativeFavGroups {
  unsigned int size;
  wchar_t** group_name;
} NativeFavGroups;

// interface the project should implement
//__declspec(dllexport) int ProjectInitial(char* parameter, Proinit_Type type);  // the parameter is a path to the userdb related file
//__declspec(dllexport) int Parse( unsigned char* buff, long size, Type type, struct NativePropertyTable** SATtable, struct NativePropertyTable** TPtable, struct NativePropertyTable** PGtable);
//__declspec(dllexport) int ReOrder( struct NativePropertyTable* SATtable, struct NativePropertyTable* TPtable, struct NativePropertyTable* PGtable, Type type, unsigned char* buff, long* size);
//__declspec(dllexport) int Clean();

typedef enum {
    DBI_FILE_UNKNOWN,
    DBI_FILE_FLASH,
    DBI_FILE_USERDB,
    DBI_FILE_PRESET
} DBI_FILE_TYPE;

__declspec(dllexport) int Load( DBI_FILE_TYPE type,  
                wchar_t* file_path, 
                struct NativePropertyTable** SATtable, 
                struct NativePropertyTable** TPtable, 
                struct NativePropertyTable** PGtable, 
                struct NativeFavGroups* FavGroup);
                
__declspec(dllexport) int Save( DBI_FILE_TYPE type,  
                wchar_t* file_path, 
                struct NativePropertyTable* SATtable, 
                struct NativePropertyTable* TPtable, 
                struct NativePropertyTable* PGtable,
                struct NativeFavGroups* FavGroup);
                
__declspec(dllexport) int UIConfig( struct NativePropertyUICfgArray* SAT_rules, 
                   struct NativePropertyUICfgArray* TP_rules, 
                   struct NativePropertyUICfgArray* PG_rules);
                
__declspec(dllexport) int Clean();

__declspec(dllexport)  NativePropertyUICfgArray* SatTemplate();
__declspec(dllexport)  NativePropertyUICfgArray* TPTemplate();
__declspec(dllexport)  NativePropertyUICfgArray* PGTemplate();

#endif  // INC_USERDB_NATIVE_INTERFACE_H_