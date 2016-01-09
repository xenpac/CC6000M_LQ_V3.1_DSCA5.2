/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/

#include "cmd_entry.h"
#include "emon_cmd.h"
#include "hal_uart.h"
#include "sys_app.h"
/******************************************************************************/

//#define SUPPORT_IR_INTERRUPT_TEST

#define EMON_DEFAULT_SERIAL_PORT (0)

#define MAX_COMMAND_LIST (100)
#define MAX_COMMAND_NAME_LEN (8)


#define EMON_KEY_BACKSPACE (8)

#define EMON_KEY_UP (0x41)
#define EMON_KEY_DOWN (0x42)
#define EMON_KEY_LEFT (0x44)
#define EMON_KEY_RIGHT (0x43)

#define EMON_KEY_PRE (EMON_KEY_UP)
#define EMON_KEY_NEXT (EMON_KEY_DOWN)

/******************************************************************************/


typedef struct
{
  int fnKey;
  char *KeyStr;
}FN_KEY_MAP;

/******************************************************************************/

//#define STR_TEN_SPACE    "           "
//#define STR_TWENTY_SPACE "                   "
//#define STR_THIRTY_SPACE "                           " //28 space

#define STR_TEN_SPACE    "          "
#define STR_FIFTEEN_SPACE "               "
#define STR_TWENTY_SPACE "                   "


// for secure crt
#define FN_CTRL_WORD_SUB (0x1B)

#define KEY_STR_UP ("[A")
#define KEY_STR_DOWN ("[B")
#define KEY_STR_LEFT ("[D")
#define KEY_STR_RIGHT ("[C")

#define KEY_STR_F1 ("OP") 
#define KEY_STR_F2 ("OQ") 
#define KEY_STR_F3 ("OR") 
#define KEY_STR_F4 ("OS")


#define FN_STEP_0 (1 << 0)
#define FN_STEP_1 (1 << 1)


/******************************************************************************/
static EMON_COMMAND_LIST *pComamndLists[MAX_COMMAND_LIST];
static char pCommandNames[MAX_COMMAND_LIST][MAX_COMMAND_NAME_LEN];
static int ComandListCounter = 0;
static BOOL g_bFirstPrintTitle = TRUE;


static FN_KEY_MAP keyMap[] = 
{
  {EMON_FN_KEY_UP,KEY_STR_UP},
  {EMON_FN_KEY_DOWN,KEY_STR_DOWN},
  {EMON_FN_KEY_LEFT,KEY_STR_LEFT},
  {EMON_FN_KEY_RIGHT,KEY_STR_RIGHT},
  {EMON_FN_KEY_F1,KEY_STR_F1},
  {EMON_FN_KEY_F2,KEY_STR_F2},
  {EMON_FN_KEY_F3,KEY_STR_F3},
  {EMON_FN_KEY_F4,KEY_STR_F4},
};

static const u32 keyMapSize = sizeof(keyMap) / sizeof(keyMap[0]);
/******************************************************************************/



static int EMON_Help(char *param);
static int EMON_List(char *param);


static void EmonLoop(void* firstCommand);

/******************************************************************************/


static EMON_COMMAND_LIST aEmonCommands[] = 
{
  {EMON_Help,     "?", "alias for 'help'",FALSE},
  {EMON_Help,     "help", "help <cmd> - print command help",FALSE},
  {EMON_List,     "ls", "ls - print all avaiable commands help",FALSE},
#if EMON_HISTORY_ENABLE
  {EMON_History,  "hist","hist - print the last commands",FALSE},
#endif
  {0, 0, 0, 0}
};

static EMON_COMMAND_LIST *FindInList(EMON_COMMAND_LIST *pList,char *cmd)
{
  EMON_COMMAND_LIST *pItem = pList;

  while (pItem->EMON_COMMAND)
  {
    //if (!strcmp(pItem->Command,cmd))
    if (!strcasecmp(pItem->Command,cmd))
    {
      return pItem;
    }
    pItem++;
  }
  return NULL;
}

static void FindStrInList(EMON_COMMAND_LIST *pList,char *cmd)
{
  EMON_COMMAND_LIST *pItem = pList;
  char*pTemp = NULL;
  char str[0x30];

  while (pItem->EMON_COMMAND)
  {
    pTemp = strstr(pItem->Command,cmd);
    if( NULL != pTemp )
    {
      if(g_bFirstPrintTitle)
      {
        APPLOGA(" Command List            \t %s  \t Command Help \n",STR_TEN_SPACE);
        APPLOGA("-------------------------------------------------------------\n");
        g_bFirstPrintTitle = FALSE;
      }
      memset(str,0x00,sizeof(str));
      if(strlen(pItem->Command) <=  7 )
      {
        sprintf(str,"%s",STR_TWENTY_SPACE);
      }
      else if(strlen(pItem->Command) <=  15)
      {
        sprintf(str,"%s",STR_FIFTEEN_SPACE);
      }
      else
      {
        sprintf(str,"%s",STR_TEN_SPACE);
      }
      APPLOGA("[\" %s \"%s\t]   \t %s\n",pItem->Command,str,pItem->Help);
    }
    pItem++;
  }
}
static void ShowAllCommandList(EMON_COMMAND_LIST *pList)
{
  EMON_COMMAND_LIST *pItem = pList;
  char str[0x30];

  while (pItem->EMON_COMMAND)
  {
    if( NULL != pItem->Command )
    {
      if(g_bFirstPrintTitle)
      {
        APPLOGA(" Command List %s \t  \t Command Help \n",STR_FIFTEEN_SPACE);
        APPLOGA("-------------------------------------------------------------\n");
        g_bFirstPrintTitle = FALSE;
      }
      memset(str,0x00,sizeof(str));
      if(strlen(pItem->Command) <=  7 )
      {
        sprintf(str,"%s",STR_TWENTY_SPACE);
      }
      else if(strlen(pItem->Command) <=  15)
      {
        sprintf(str,"%s",STR_FIFTEEN_SPACE);
      }
      else
      {
        sprintf(str,"%s",STR_TEN_SPACE);
      }
      APPLOGA("[\" %s \"%s\t]   \t %s\n",pItem->Command,str,pItem->Help);
    }
    pItem++;
  }
}
static int EMON_Help(char *param)
{
  int i;
  unsigned int lenth;
  char aCommand[0x30];
  char*pTemp;

  //If help with no parameter, it will show all command.
  if(param == NULL || EMON_CheckCommand(param, 0))
  {
    g_bFirstPrintTitle = TRUE;
    for (i=0; i<ComandListCounter; i++)
    {
      ShowAllCommandList(pComamndLists[i]);
    }
  }
  else
  {
    pTemp = param;
    while(pTemp != NULL && pTemp[0] == ' ')
    {
      pTemp++;
    }

    if(strlen(pTemp) > (sizeof(aCommand)-1))
    {
      lenth = sizeof(aCommand)-1;
    }
    else
    {
      lenth = strlen(pTemp);
    }
    memset(aCommand,0x00,sizeof(aCommand));
    memcpy(aCommand,pTemp,lenth);

    g_bFirstPrintTitle = TRUE;
    for (i=0; i<ComandListCounter; i++)
    {
      FindStrInList(pComamndLists[i],aCommand);
    }
    if(g_bFirstPrintTitle)
    {
      APPLOGA("Can't Find the command which includes the string \"%s\"",aCommand);
    }
  }
  return SUCCESS;
}

static int EMON_List(char *param)
{
  #define TAB_SIZE 8
  char aCommand[20];
  int i;
  int ret = SUCCESS;
  BOOL cmdFound = FALSE;

  if (EMON_CheckCommand(param, 0))
  {
    //print all module's name
    for (i=0; i<ComandListCounter; i++)
    {
      APPLOGA("%s\t\t", pCommandNames[i]);
      if (i%4 == 0)
      {
        APPLOGA("\n");
      }
    }
  }
  else if (EMON_CheckCommand(param, 1))
  {
    //print all command belongs to that module
    sscanf(param, "%s", aCommand);

    if (strcmp(aCommand, "all") == 0)  //list all emon command
    {
      for (i=0; i<ComandListCounter; i++)
      {
        EMON_COMMAND_LIST *pItem = pComamndLists[i];
        int j=1;

        APPLOGA("[%s]:\n", pCommandNames[i]);
        while (pItem->EMON_COMMAND)
        {
          APPLOGA("%s\t",pItem->Command);
          if (strlen(pItem->Command) < TAB_SIZE)
          {
            APPLOGA("\t");
          }

          pItem++;
          if (!(j%4) || !pItem->EMON_COMMAND)
          {
            APPLOGA("\n");
          }
          j++;
        }

        if (j != 5)
          APPLOGA("\n");
      }
    }
    else    //only list command belongs to that module
    {
      for (i=0; i<ComandListCounter; i++)
      {
        if (strcmp(aCommand, pCommandNames[i]) == 0)
        {
          EMON_COMMAND_LIST *pItem = pComamndLists[i];
          int j=1;

          while (pItem->EMON_COMMAND)
          {
            APPLOGA("%s\t",pItem->Command);
            if (strlen(pItem->Command) < TAB_SIZE)
            {
              APPLOGA("\t");
            }

            pItem++;
            if (!(j%4) || !pItem->EMON_COMMAND)
            {
              APPLOGA("\n");
            }
            j++;
          }

          if (j != 5)
            APPLOGA("\n");

          cmdFound = TRUE;
          break;
        }
      }
      if (!cmdFound)
      {
        APPLOGA("Can't find the module name[%s]\n", aCommand);
        ret = ERR_FAILURE;
      }
      else
      {
        ret = SUCCESS;
      }
    }
  }
  APPLOGA("\n");
  return ret;
}

int EMON_Initialise(void)
{
  BOOL result = FALSE;
  int ret;
  
  ComandListCounter = 0;
  ret = EMON_Register_Test_Functions(aEmonCommands, "emon");
  if (ret != SUCCESS)
  {
    return ret;
  }
  emon_entry_init();
#if EMON_HISTORY_ENABLE
  EMON_History_init();
#endif
  

  result = mtos_task_create((u8 *)"emon",
                        EmonLoop,
                        NULL,
                        EMON_TASK_PRIO,
                        NULL,
                        DEFAULT_STKSIZE);
  MT_ASSERT(result);
  return SUCCESS;
}


int EMON_Register_Test_Functions(EMON_COMMAND_LIST * pList, char* cName)
{
  if (ComandListCounter >= MAX_COMMAND_LIST)
  {
    APPLOGA("Too many command list\n");
    return ERR_FAILURE;
  }

  strncpy( pCommandNames[ComandListCounter], cName, MAX_COMMAND_NAME_LEN-1);
  pCommandNames[ComandListCounter][MAX_COMMAND_NAME_LEN-1] = 0;

  pComamndLists[ComandListCounter++] = pList;
  return SUCCESS;
}


static int EMON_Parse(char *pCommandStr)
{
  int i;
  char aCommand[HISTORY_H_SIZE];
  EMON_COMMAND_LIST *pCommand;
  int ret = ERR_FAILURE;
  char ch_backup;

  sscanf(pCommandStr,"%s",aCommand);

  for (i=0; i<ComandListCounter; i++)
  {
    pCommand = FindInList(pComamndLists[i],aCommand);
    if (pCommand)
    {
      ch_backup = pCommandStr[strlen(aCommand)-1];
      pCommandStr[strlen(aCommand)-1] = ' ';
      ret = pCommand->EMON_COMMAND(pCommandStr+strlen(aCommand)-1);
      if (ret != SUCCESS)
      {
        APPLOGA("Error cmd: %s\n", pCommand->Command);
        APPLOGA("Usage: %s\n", pCommand->Help);
      }
      pCommandStr[strlen(aCommand)-1] = ch_backup;
      return ret;
    }
  }
  APPLOGA("Cannot find \"%s\" command\n",aCommand);
  return ret;
}

static u8 _get_char(void)
{
  u8 ch = 0;
  s32 ret;

  //lint -e716 while(1)
  while (1)
  {
    ret = uart_read_byte(EMON_DEFAULT_SERIAL_PORT, &ch, 5);
    if (ret != SUCCESS)
    {
      mtos_task_sleep(100);
      //continue;
    }
    mtos_task_sleep(1);
    
    return ch;
  }
}

u8 EMON_GetCh(void)
{
#ifdef WIN32
  return Win_GetChar();
#else
  return _get_char();
#endif
}

static int keyConvert(u8 *keyStr)
{
  int key = EMON_FN_KEY_INVALID;
  u32 i;

  MT_ASSERT(keyStr != NULL);
  // The first charactor should be control byte CTRL_WORD_SUB/CTRL_WORD_NONE
  
  for (i = 0; i < keyMapSize; i++)
  {
    //APPLOGA("str0:%s,str1:%s\n",keyStr + 1, keyMap[i].KeyStr);
    // skip control word for key string.
    if (strcasecmp((char*)keyStr + 1, keyMap[i].KeyStr) == 0)
    {
      key = keyMap[i].fnKey;
      break;
    }
  }

  return key;
}


int EMON_GetChEx(u8 **chBuff, int *size)
{
  u8 ch;
  u32 flag = 0;
  static u8 keyStr[4];
  int exKey = 0;
  
  memset(keyStr, 0, sizeof(keyStr));
  //lint -e716
  while (1)
  {
    ch = EMON_GetCh();
    if (ch == FN_CTRL_WORD_SUB)
    {
      flag = FN_STEP_0;
      keyStr[0] = ch;
      //EMON_PutCh(ch);
      continue;
    }
    if (flag == FN_STEP_0)
    {
      if (ch == '[' || ch == 'O')
      {
        flag = FN_STEP_1;
        keyStr[1] = ch;
        //EMON_PutCh(ch);
        continue;
      }
      else
      {
        flag = 0;
      }
    }
    if (flag == FN_STEP_1)
    {
      //EMON_PutCh(ch);
      keyStr[2] = ch;
    }
    flag = 0;

    if (chBuff != NULL)
    {
      *chBuff = keyStr;
    }
    if (size != NULL)
    {
      *size = (int)strlen((char*)keyStr);
    }

    if (keyStr[0] == FN_CTRL_WORD_SUB)
    {
      exKey = keyConvert(keyStr);
      if (exKey != EMON_FN_KEY_INVALID)
      {
        return exKey;
      }
    }
    else
    {
      return ch;
    }
  }
}


void EMON_PutCh(u8 c)
{
#ifdef WIN32
  Win_PutChar(c);
#else
  uart_write_byte(EMON_DEFAULT_SERIAL_PORT, c);
#endif
}

static char *get_command(char *buffer, int size)
{
  u8 ch = 0;
  int value = 0; 
  int index = 0;
  int max_index = 0;
  int i;
  u8 *pBuffer = NULL;
  int bufferSize = 0;
  BOOL endLineFlag = FALSE;
#if EMON_HISTORY_ENABLE
  char *hist = NULL;
#endif
  //lint -e716 while(1)
  while(1)
  {
    //ch = EMON_GetCh();
    value = EMON_GetChEx(&pBuffer, &bufferSize);
    if (value <= 0xFF)
    {
      ch = value & 0xFF;
    }
    else
    {
      switch(value)
      {
        case EMON_FN_KEY_LEFT:
          if (index > 0 && max_index > 0)
          {
            if (pBuffer != NULL)
            {
              for (i = 0; i < bufferSize; i++)
              {
                EMON_PutCh(pBuffer[i]);
              }
            }
            index--;
          }
          continue;
        case EMON_FN_KEY_RIGHT:
          if (index < max_index)
          {
            if (pBuffer != NULL)
            {
              for (i = 0; i < bufferSize; i++)
              {
                EMON_PutCh(pBuffer[i]);
              }
            }
            index++;
          }
          continue;
        
        case EMON_FN_KEY_UP:
        #if EMON_HISTORY_ENABLE
          hist = EMON_GetPreCommand();
          if (hist != NULL)
          {
            for (i = 0; i <= max_index; i++)
            {
              EMON_PutCh('\b');
            }
            for (i = 0; i <= max_index; i++)
            {
              EMON_PutCh(' ');
            }
            for (i = 0; i < max_index; i++)
            {
              EMON_PutCh('\b');
            }
            memcpy(buffer, hist, (u32)size);
            index = (int)strlen(hist);
            max_index = index;
            APPLOGA("%s", hist);
          }
        #endif
          continue;
        case EMON_FN_KEY_DOWN:
        #if EMON_HISTORY_ENABLE
          hist = EMON_GetNextCommand();
          if (hist != NULL)
          {
            for (i = 0; i <= max_index; i++)
            {
              EMON_PutCh('\b');
            }
            for (i = 0; i <= max_index; i++)
            {
              EMON_PutCh(' ');
            }
            for (i = 0; i < max_index; i++)
            {
              EMON_PutCh('\b');
            }
            memcpy(buffer, hist, (u32)size);
            index = (int)strlen(hist);
            max_index = index;
            APPLOGA("%s", hist);
          }
        #endif
          continue;
        default:
          continue;
      }
    }
    if (ch != 0)
    {
      if (ch == 0x3) // ^c
      {
        APPLOGA("<INTERRUPT>\n");
        return NULL;
      }
      if((ch == '\n') || (ch == '\r'))
      {
        buffer[max_index] = '\0';
        if (endLineFlag)
        {
          APPLOGA("\n");
          return buffer;
        }
        endLineFlag = TRUE;
        continue;
      }

      if (ch == '\b')
      {
        if (index > 0 && max_index > 0)
        {
          //
          // Backspace character
          //
          index--;
          for (i = index; i < max_index; i++)
          {
            buffer[i] = buffer[i+1];
          }
          max_index--;
          EMON_PutCh('\b');
          for (i = index; i <= max_index; i++)
          {
            EMON_PutCh((u8)buffer[i]);
          }
          EMON_PutCh(' ');
          for (i = index; i <= max_index; i++)
          {
            EMON_PutCh('\b');
          }
        }
      }
      else
      {
        if (max_index < size - 1)
        {
          if (index < max_index)
          {
            for (i = max_index; i > index; i--)
            {
              buffer[i] = buffer[i-1];
            }
            //memcpy(&buffer[index+1], &buffer[index], (unsigned int)(max_index - index));
          }
          buffer[index] = (char)ch;
          for (i = index; i <= max_index; i++)
          {
            EMON_PutCh((u8)buffer[i]);
          }
          for (i = index; i < max_index; i++)
          {
            EMON_PutCh('\b');
          }
          index++;
          max_index++;
        }
      }
    }
    else
    {
      if (endLineFlag)
      {
        APPLOGA("\n");
        return buffer;
      }
    }
  }
  //lint -e527 
  return NULL; // We never ging to be here
}


static void EmonLoop(void* firstCommand)
{
  char aCommand[HISTORY_H_SIZE];


  //
  // if firstCommand isn't NULL do it first
  //
  if(firstCommand != NULL)
  {
    EMON_Parse(firstCommand);
  }

  while (1)
  {        
    memset(aCommand, 0, sizeof(aCommand));
    APPLOGA("EMON> ");

    if (get_command(aCommand, sizeof(aCommand)) != NULL)
    {
      if (*aCommand)
      {
    #if EMON_HISTORY_ENABLE
        EMON_AddToHistory(aCommand);
    #endif
        EMON_RemoveBackspace(aCommand);
        if(strlen(aCommand) > 0)
        {
          EMON_Parse(aCommand);
        }
      }
    }
  }
}


BOOL EMON_CheckCommand(char *param, int iNumOfParam)
{
  int i = 0;
  int count = 0;

  if (param == NULL)
  {
    if (iNumOfParam == 0)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  //
  // Remove first space to adjust to the change in the parsing
  //
  if (param != NULL && param[0] == ' ')
  {
    param++;
  }
  for (i=0;i < (int)strlen(param); i++)
  {
    if (param[i] == ' ')
    {
      count++;
      while (param[++i] == ' ')
      {
        ;// do nothing
      }
    }
  }

  if(count != iNumOfParam)
  {
    return FALSE;
  }
  return TRUE;
}

void EMON_RemoveBackspace(char* pcCommand)
{
  if (pcCommand[0] == EMON_KEY_BACKSPACE)
  {
    strcpy(pcCommand, pcCommand + 1);
  }
}


static EMON_COMMAND_LIST *FindFuncInList(EMON_COMMAND_LIST *pList,void  *func)
{
    EMON_COMMAND_LIST *pItem = pList;

    while (pItem->EMON_COMMAND)
    {
        if (pItem->EMON_COMMAND == func)
        {
            return pItem;
        }
        pItem++;
    }
    return NULL;
}

int EMON_HandleParam(void *func, char *param,int numberOfParam)
{
  int i;
  EMON_COMMAND_LIST *pCommand;

  if(!EMON_CheckCommand(param, numberOfParam))
  {
    for (i=0; i<ComandListCounter; i++)
    {
      pCommand = FindFuncInList(pComamndLists[i],func);
      if (pCommand)
      {
        APPLOGA("%s\n",pCommand->Help);
        return ERR_FAILURE ;
      }
    }
    APPLOGA("Cannot find help on EMON function\n");
  }
  return SUCCESS;
}

