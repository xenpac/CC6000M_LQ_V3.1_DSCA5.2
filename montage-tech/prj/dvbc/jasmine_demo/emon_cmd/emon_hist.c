/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
//------------Global Layer---------


#include "emon_cmd.h"
#include "sys_app.h"

#if EMON_HISTORY_ENABLE


static EMON_HISTORY aHistory;


int EMON_History_init(void)
{
  memset(&aHistory, 0, sizeof(aHistory));

  return SUCCESS;
}

void EMON_AddToHistory(char *string)
{
  memset(aHistory.cHistory[aHistory.iWritePtr], 0, sizeof(aHistory.cHistory[aHistory.iWritePtr]));
  strncpy(aHistory.cHistory[aHistory.iWritePtr], string, sizeof(aHistory.cHistory[aHistory.iWritePtr]));
  aHistory.iWritePtr++;
  if (aHistory.iWritePtr >= HISTORY_V_SIZE)
  {
    aHistory.iWritePtr %= HISTORY_V_SIZE;
    aHistory.wPtrLooped = TRUE;
  }
  aHistory.iReadPtr = aHistory.iWritePtr;
}

char *EMON_GetPreCommand(void)
{
  int position;

  if (aHistory.iReadPtr == 0)
  {
    if (!aHistory.wPtrLooped)
    {
      return NULL;
    }
    else
    {
      position = HISTORY_V_SIZE - 1;
    }
  }
  else
  {
    position = aHistory.iReadPtr - 1;
  }

  MT_ASSERT(position >= 0);
  MT_ASSERT(position < HISTORY_V_SIZE);

  if (position == aHistory.iWritePtr)
  {
    return NULL;
  }

  aHistory.iReadPtr = position;
  return aHistory.cHistory[position];
}

char* EMON_GetNextCommand(void)
{
  int position;

  if (aHistory.iReadPtr == aHistory.iWritePtr)
  {
    return NULL;
  }
  position = (aHistory.iReadPtr + 1) % HISTORY_V_SIZE;

  MT_ASSERT(position >= 0);
  MT_ASSERT(position < HISTORY_V_SIZE);

  if (position == aHistory.iWritePtr)
  {
    return NULL;
  }
  else
  {
    aHistory.iReadPtr = position;
  }

  return aHistory.cHistory[position];
}

char* EMON_GetHistCommand(int number)
{
  MT_ASSERT(number < HISTORY_V_SIZE);

  return aHistory.cHistory[number];
}



int EMON_History(char *param)
{
  int i;

  if (!aHistory.wPtrLooped)
  {
    for(i = 0; i < aHistory.iWritePtr; i++)
    {
      APPLOGA("%d: %s\n", i, aHistory.cHistory[i]);
    }
  }
  else
  {
    int index;
    for(i = 0; i < HISTORY_V_SIZE; i++)
    {
      index = (i + aHistory.iWritePtr) % HISTORY_V_SIZE;
      APPLOGA("%d: %s\n", i, aHistory.cHistory[index]);
    }
  }
  

  return SUCCESS;
}

u32 EMON_WhenHistory(char* aHistoryCommand)
{
  APPLOGA("EMON> %s",aHistoryCommand);

  //getsAfterHistory(aHistoryCommand, strlen(aHistoryCommand));

  return strlen(aHistoryCommand);
}


// //////////////////////////////////////////////////////////////////////////
// Function Name: 
//   gets
//
// Input Parameters:
//   char *buffer - The buffer to fill with the string, should be pre allocated
//
// Return Value:
//   returns its argument if successful. A NULL pointer indicates an error.
//
// Description: 
//   implementation of the stdio gets with the UART
// //////////////////////////////////////////////////////////////////////////
#if 0
static char *getsAfterHistory(char *buffer, int length)
{
  u8 ucByte;


  while(1)
  {
    if (UART_ReceiveByte(UART_DEBUG, &ucByte))
    {
      UART_WriteToHost( UART_DEBUG, &ucByte, 1 );
      if((ucByte == '\n') || (ucByte == '\r'))
      {
        // 
        // "carriage return" received, finished getting the string, put '\0' and return
        //
        buffer[length] = '\0';
        ucByte = '\n';
        UART_WriteToHost( UART_DEBUG, &ucByte, 1 );
        return buffer;
      }

      if(ucByte == '\b' && length > 0)
      {
        //
        // Backspace character
        //
        length--;
        ucByte = ' ';
        UART_WriteToHost( UART_DEBUG, &ucByte, 1 );
        ucByte = '\b';
        UART_WriteToHost( UART_DEBUG, &ucByte, 1 );
      }

      else
      {
        buffer[length] = ucByte;
        length ++;
      }
    }
    OS_Sleep(1); 
  }
  return NULL; // We never ging to be here
}
#endif

#endif
