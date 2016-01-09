/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
// //////////////////////////////////////////////////////////////////////////
/// \file cmd.h
/// \brief This file contains the defines and functions of the emon command.
///
///Herader file for the Emon.
/// \addtogroup Emon
/// \{
// //////////////////////////////////////////////////////////////////////////
#ifndef _EMON_CMD_H_
#define _EMON_CMD_H_


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sys_types.h"
#include "sys_cfg.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "sys_define.h"


#define EMON_HISTORY_ENABLE (1)



//
// Definition of list entry
//
///////////////////////////////////////////////////////////////////////////////
/// \struct EMON_COMMAND_LIST
/// \brief   emon command list
///////////////////////////////////////////////////////////////////////////////
typedef const struct _EMON_COMMAND_LIST
{
  int (*EMON_COMMAND)( char *params );  ///< Command to execute
  char *Command;  ///< Command name as the user needs to call
  char *Help; ///< Help string
  int reserved; ///<   ....
} EMON_COMMAND_LIST;

#define HISTORY_H_SIZE (256)
#define HISTORY_V_SIZE (16)



///////////////////////////////////////////////////////////////////////////////
/// \struct EMON_HISTORY
/// \brief   define  emon  history struct
///////////////////////////////////////////////////////////////////////////////
typedef struct _EMON_HISTORY
{
  char cHistory[HISTORY_V_SIZE][HISTORY_H_SIZE];///<history store db
  int iWritePtr;///< wright operation
  int iReadPtr;///<read operation
  BOOL wPtrLooped;
} EMON_HISTORY;

typedef enum
{
  EMON_FN_KEY_INVALID = 256,
  
  EMON_FN_KEY_UP,
  EMON_FN_KEY_DOWN,
  EMON_FN_KEY_LEFT,
  EMON_FN_KEY_RIGHT,
  EMON_FN_KEY_F1,
  EMON_FN_KEY_F2,
  EMON_FN_KEY_F3,
  EMON_FN_KEY_F4,

}EMON_FN_KEY;


//SDE6_MIGRATION:
/////////////////////////////////////////////////////////////////////////////
/// \brief      Initialization of Emon core
///
/// \return    Return  SUCCESS if success else FAILURE.
///
/////////////////////////////////////////////////////////////////////////////
int EMON_Initialise(void);


//SDE6_MIGRATION:
/////////////////////////////////////////////////////////////////////////////
/// \brief     Add a command list to the emon parser
///
/// \param[in] pList - List of Emon command. Last entry must be 0.
/// \param[in] cName - name
///
/// \return        On error ERR_FAILURE otherwise SUCCESS
///
/////////////////////////////////////////////////////////////////////////////
int EMON_Register_Test_Functions(EMON_COMMAND_LIST * pList, char* cName);


/////////////////////////////////////////////////////////////////////////////
/// \brief     Init history modle
///
///
/// \return      None
/////////////////////////////////////////////////////////////////////////////
int EMON_History_init(void);

/////////////////////////////////////////////////////////////////////////////
/// \brief     Add a string to the histroy buffer.
///
/// \param[in] string - The string you want to add.
///
/// \return      None
/////////////////////////////////////////////////////////////////////////////
void EMON_AddToHistory(char *string);

/////////////////////////////////////////////////////////////////////////////
/// \brief     Get the next command from history.Parameter number as the benchmark.
///
///
/// \return      Next command pointer. NULL if no history.
/////////////////////////////////////////////////////////////////////////////
char* EMON_GetNextCommand(void);

/////////////////////////////////////////////////////////////////////////////
/// \brief     Get the previous command from history.Parameter number as the benchmark.
///
///
/// \return      Next command pointer. NULL if no history.
/////////////////////////////////////////////////////////////////////////////
char *EMON_GetPreCommand(void);

/////////////////////////////////////////////////////////////////////////////
/// \brief     This function just print all of command which are in the history.
///
/// \param[in] param - Not used yet
///
/// \return      None
/////////////////////////////////////////////////////////////////////////////
int EMON_History(char *param);

/////////////////////////////////////////////////////////////////////////////
/// \brief     Check if the command is in correct format
///
/// \param[in] param - command string
/// \param[in] iNumOfParam - The parameters number of command
///
/// \return      TRUE if the command is in correct format,else return FALSE
/////////////////////////////////////////////////////////////////////////////
BOOL EMON_CheckCommand(char *param, int iNumOfParam);

/////////////////////////////////////////////////////////////////////////////
/// \brief     Print  help info of the command.
///
/// \param[in] func - The command related function pointer.
/// \param[in] param - command pointer
/// \param[in] numberOfParam - The number of instructions 
///
/// \return      SUCCESS if find help info on EMON function,else return ERR_FAILURE
/////////////////////////////////////////////////////////////////////////////
int EMON_HandleParam(void *func, char *param,int numberOfParam);


/////////////////////////////////////////////////////////////////////////////
/// \brief     remove one backspace.if you want remove more than one ,you can duplicate  call this function 
///
/// \param[in] pcCommand - command string
/// \return      None
/////////////////////////////////////////////////////////////////////////////
void EMON_RemoveBackspace(char* pcCommand);


/////////////////////////////////////////////////////////////////////////////
/// \brief     get charactor from serial port.
///
/// 
/// \return   charactor
/////////////////////////////////////////////////////////////////////////////
u8 EMON_GetCh(void);

/////////////////////////////////////////////////////////////////////////////
/// \brief     put charactor to serial port.
///
/// \param[in] charactor - charactor
/// \return      None
/////////////////////////////////////////////////////////////////////////////
void EMON_PutCh(u8 c);


/////////////////////////////////////////////////////////////////////////////
/// \brief     get charactor from serial port. 
///
/// 
/// \return   It's a FN(functional)key in case of return value > 255, otherwise  charactor
/////////////////////////////////////////////////////////////////////////////
int EMON_GetChEx(u8 **chBuff, int *size);


#endif
/// \}

