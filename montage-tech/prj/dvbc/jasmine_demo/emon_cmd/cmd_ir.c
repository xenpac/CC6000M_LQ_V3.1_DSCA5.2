/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "emon_cmd.h"

#include "mtos_msg.h"
#include "ap_framework.h"

#include "ap_uio.h"
#include "cmd_ir.h"
#include "lib_rect.h"
#include "lib_memp.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "lib_memf.h"
#include "flinger.h"
#include "gdi.h"
#include "ctrl_base.h"
#include "gui_resource.h"
#include "gui_paint.h"
#include "framework.h"
#include "mdl.h"
#include "uio_key_def.h"

/******************************************************************************/

#define KEY_STR_LEN (4)

// charactor up from secure CRT is 0x1b with "[A"
// charactor F1 from secure CRT is 0x1b with "OP"

#define CTRL_WORD_NONE (0x0)
#define CTRL_WORD_SUB (0x1B)




#define STEP_0 (1 << 0)
#define STEP_1 (1 << 1)
#define STEP_2 (1 << 2)




#define CMD_KEY_POWER ("P")
#define CMD_KEY_MUTE ("J")
#define CMD_KEY_RECALL ("H")
#define CMD_KEY_TVRAIDO ("T")
#define CMD_KEY_0 ("0")
#define CMD_KEY_1 ("1")
#define CMD_KEY_2 ("2")
#define CMD_KEY_3 ("3")
#define CMD_KEY_4 ("4")
#define CMD_KEY_5 ("5")
#define CMD_KEY_6 ("6")
#define CMD_KEY_7 ("7")
#define CMD_KEY_8 ("8")
#define CMD_KEY_9 ("9")
#define CMD_KEY_EXIT ("C")
#define CMD_KEY_OK (" ") //blank.
#define CMD_KEY_UP ("[A")
#define CMD_KEY_DOWN ("[B")
#define CMD_KEY_LEFT ("[D")
#define CMD_KEY_RIGHT ("[C")
#define CMD_KEY_MENU ("M") 
#define CMD_KEY_PROGLIST ("L") 
#define CMD_KEY_AUDIO ("A") 
#define CMD_KEY_PAGEUP ("=") 
#define CMD_KEY_PAGEDOWN ("-") 
#define CMD_KEY_INFO ("I") 
#define CMD_KEY_FAV ("F") 
#define CMD_KEY_EPG ("E") 
#define CMD_KEY_RED ("R") 
#define CMD_KEY_GREEN ("G") 
#define CMD_KEY_YELLOW ("Y") 
#define CMD_KEY_BLUE ("B") 
#define CMD_KEY_VDOWN ("U") 
#define CMD_KEY_VUP ("O") 
#define CMD_KEY_ZOOM ("Z") 
#define CMD_KEY_D ("D") 
#define CMD_KEY_F1 ("OP") 
#define CMD_KEY_F2 ("OQ") 
#define CMD_KEY_F3 ("OR") 
#define CMD_KEY_F4 ("OS")


typedef struct
{
  u8 UIKey;
  char *KeyStr;
}CMD_KEY_MAP;
/******************************************************************************/


static CMD_KEY_MAP keyMap[] = 
{
  {V_KEY_POWER,CMD_KEY_POWER},
  {V_KEY_MUTE,CMD_KEY_MUTE},
  {V_KEY_RECALL,CMD_KEY_RECALL},
  {V_KEY_TVRADIO,CMD_KEY_TVRAIDO},
  {V_KEY_0,CMD_KEY_0},
  {V_KEY_1,CMD_KEY_1},
  {V_KEY_2,CMD_KEY_2},
  {V_KEY_3,CMD_KEY_3},
  {V_KEY_4,CMD_KEY_4},
  {V_KEY_5,CMD_KEY_5},
  {V_KEY_6,CMD_KEY_6},
  {V_KEY_7,CMD_KEY_7},
  {V_KEY_8,CMD_KEY_8},
  {V_KEY_9,CMD_KEY_9},
  {V_KEY_CANCEL,CMD_KEY_EXIT},
  {V_KEY_OK,CMD_KEY_OK},
  {V_KEY_UP,CMD_KEY_UP},
  {V_KEY_DOWN,CMD_KEY_DOWN},
  {V_KEY_LEFT,CMD_KEY_LEFT},
  {V_KEY_RIGHT,CMD_KEY_RIGHT},
  {V_KEY_MENU,CMD_KEY_MENU},
  {V_KEY_PROGLIST,CMD_KEY_PROGLIST},
  {V_KEY_AUDIO,CMD_KEY_AUDIO},
  {V_KEY_PAGE_UP,CMD_KEY_PAGEUP},
  {V_KEY_PAGE_DOWN,CMD_KEY_PAGEDOWN},
  {V_KEY_INFO,CMD_KEY_INFO},
  {V_KEY_FAV,CMD_KEY_FAV},
  {V_KEY_EPG,CMD_KEY_EPG},
  {V_KEY_RED,CMD_KEY_RED},
  {V_KEY_GREEN,CMD_KEY_GREEN},
  {V_KEY_YELLOW,CMD_KEY_YELLOW},
  {V_KEY_BLUE,CMD_KEY_BLUE},
  {V_KEY_VUP,CMD_KEY_VUP},
  {V_KEY_VDOWN,CMD_KEY_VDOWN},
  {V_KEY_ZOOM,CMD_KEY_ZOOM},
  {V_KEY_D,CMD_KEY_D},
  {V_KEY_F1,CMD_KEY_F1},
  {V_KEY_F2,CMD_KEY_F2},
  {V_KEY_F3,CMD_KEY_F3},
  {V_KEY_F4,CMD_KEY_F4},
};

static const u32 keyMapSize = sizeof(keyMap) / sizeof(keyMap[0]);


/******************************************************************************/
static u8 cmdKey2UIKey(u8 *keyStr);


/******************************************************************************/



static u8 cmdKey2UIKey(u8 *keyStr)
{
  u8 key = V_KEY_INVALID;
  u32 i;

  //lint -e731
  MT_ASSERT(keyStr != NULL);
  // The first charactor should be control byte CTRL_WORD_SUB/CTRL_WORD_NONE
  
  for (i = 0; i < keyMapSize; i++)
  {
    //OS_PRINTF("str0:%s,str1:%s\n",keyStr + 1, keyMap[i].KeyStr);
    // skip control word for key string.
    if (strcasecmp((char*)keyStr + 1, keyMap[i].KeyStr) == 0)
    {
      key = keyMap[i].UIKey;
      break;
    }
  }

  return key;//cmdKey;
}

static int ir_loop(char *param)
{
  u8 ch;
  u8 UIKey;
  u32 flag = 0;
  u8 keyStr[KEY_STR_LEN];
  
  memset(keyStr, 0, sizeof(keyStr));
  //lint -e716
  while (1)
  {
    ch = EMON_GetCh();
    if (ch == CTRL_WORD_SUB)
    {
      flag = STEP_0;
      keyStr[0] = ch;
      continue;
    }
    if (flag == STEP_0)
    {
      if (ch == '[' || ch == 'O')
      {
        flag = STEP_1;
        keyStr[1] = ch;
        continue;
      }
      else
      {
        flag = 0;
      }
    }
    if (flag == STEP_1)
    {
      keyStr[2] = ch;
    }
    flag = 0;
    if (keyStr[0] != CTRL_WORD_SUB)
    {
      if (ch == 0x3) // ^c
      {
        break; //break from while(1)
      }
      else
      {
        keyStr[0] = CTRL_WORD_NONE;
        keyStr[1] = ch;
      }
    }
    
    UIKey = cmdKey2UIKey(keyStr);
    if (UIKey != V_KEY_INVALID)
    {
      event_t evt = {0};
      evt.id = UIO_EVT_KEY;
      evt.data1 = UIKey;
      ap_frm_send_evt_to_ui(APP_UIO, &evt);
    }
    memset(keyStr, 0, sizeof(keyStr));
    mtos_task_sleep(100);
  }

  return SUCCESS;
}



static EMON_COMMAND_LIST cmd_ir[] = 
{
  {ir_loop, "ir", "ir - loop to receive key then send to UI(ctrl+c to quit)",FALSE},
  {0, 0, 0, 0}
};


int cmd_ir_init(void)
{
  return EMON_Register_Test_Functions(cmd_ir, "ir");
}


