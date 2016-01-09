/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "string.h"

#include "drv_dev.h"
#include "uio.h"
#include "class_factory.h"
#include "mdl.h"
#include "data_manager.h"

#include "ap_framework.h"
#include "hal_misc.h"
#include "customer_config.h"

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

#include "ap_uio.h"
#include "sys_app.h"


//#include "data_manager.h"

#define KEY_RC_SETS_1   0
#define KEY_RC_SETS_2   1
#define KEY_RC_SETS_3   2
#define KEY_FP_SET      3

enum v_key_plus
{
  V_KEY_PLUS_START = V_KEY_UNKNOW,
};


#ifdef WIN32
static key_map_t ir_keymap0[] =
{
  {0x55, V_KEY_POWER},          //SDLK_p
  {0x4A, V_KEY_MUTE},           //SDLK_j
  {0x48, V_KEY_RECALL},         //SDLK_h
  {0x54, V_KEY_TVRADIO},        //SDLK_t
  {0x30, V_KEY_0},              //SDLK_0
  {0x31, V_KEY_1},              //SDLK_1
  {0x32, V_KEY_2},              //SDLK_2
  {0x33, V_KEY_3},              //SDLK_3
  {0x34, V_KEY_4},              //SDLK_4
  {0x35, V_KEY_5},              //SDLK_5
  {0x36, V_KEY_6},              //SDLK_6
  {0x37, V_KEY_7},              //SDLK_7
  {0x38, V_KEY_8},              //SDLK_8
  {0x39, V_KEY_9},              //SDLK_9
  {0x43, V_KEY_EXIT},           //SDLK_c
  {0x20, V_KEY_OK},             //SDLK_SPACE
  {0x26, V_KEY_UP},             //SDLK_UP
  {0x28, V_KEY_DOWN},           //SDLK_DOWN
  {0x25, V_KEY_LEFT},           //SDLK_LEFT
  {0x27, V_KEY_RIGHT},          //SDLK_RIGHT
  {0x4D, V_KEY_MENU},           //SDLK_m
  {0x4C, V_KEY_PROGLIST},       //SDLK_L
  {0x41, V_KEY_AUDIO},          //SDLK_a
  {0x21, V_KEY_PAGE_UP},        //SDLK_PAGEUP
  {0x22, V_KEY_PAGE_DOWN},      //SDLK_PAGEDOWN
  {0x49, V_KEY_INFO},           //SDLK_i
  {0x46, V_KEY_FAV},            //SDLK_f
  {0xBE, V_KEY_PAUSE},          //.
  {0xBC, V_KEY_PLAY},           //,
  {0xBF, V_KEY_9PIC},           ///
  {0x45, V_KEY_EPG},            //SDLK_e
  {0xE9, V_KEY_SI},             //ctrl + 9
  {0xE8, V_KEY_GRAY},           //ctrl + 8
  {0x52, V_KEY_RED},            //SDLK_r
  {0x47, V_KEY_GREEN},          //SDLK_g
  {0x5A, V_KEY_YELLOW},         //SDLK_y
  {0x42, V_KEY_BLUE},           //SDLK_b
  {0xE7, V_KEY_UPG},            //ctrl + 7
  {0xBA, V_KEY_TTX},            //;
  {0xE6, V_KEY_TEST},           //ctrl + 6
  {0xE5, V_KEY_I2C},            //ctrl + 5
  {0xE4, V_KEY_OTA_FORCE},      //ctrl + 4
  {0x53, V_KEY_SAT},            //SDLK_s
  {0x51, V_KEY_UCAS},           //SDLK_q
  {0x57, V_KEY_SAT_CODE},       //SDLK_w
  {0x58, V_KEY_VIDEO_MODE},            //SDLK_v
  {0x59, V_KEY_REC},            //SDLK_x
  {0x4E, V_KEY_PN},             //SDLK_n
  {0xF9, V_KEY_TS},             //ctlr + s9
  {0xE1, V_KEY_BACK2},          //ctrl + 1
  {0xE2, V_KEY_FORW2},          //ctrl + 2
  {0xE3, V_KEY_BACK},           //ctrl + 3
  {0xE4, V_KEY_FORW},           //ctrl + 4
  {0xE5, V_KEY_REVSLOW},        //ctrl + 5
  {0xE6, V_KEY_SLOW},           //ctrl + 6
  {0xE7, V_KEY_STOP},           //ctrl + 7
  {0x56, V_KEY_VDOWN},          //SDLK_u
  {0x4F, V_KEY_VUP},            //SDLK_o
  {0xDE, V_KEY_SUBT},           //'
  {0x5B, V_KEY_ZOOM},           //SDLK_z
  {0xE1, V_KEY_TVSAT},          //ctrl + 1
  {0xE0, V_KEY_FIND},           //ctrl + 0
  {0x44, V_KEY_D},              //SDLK_d
  {0x70, V_KEY_F1},             //SDLK_F1
  {0x71, V_KEY_F2},             //SDLK_F2
  {0x72, V_KEY_F3},             //SDLK_F3
  {0x73, V_KEY_F4},             //SDLK_F4
  {0xE0, V_KEY_CTRL0},          //ctrl + 0
  {0xE1, V_KEY_CTRL1},          //ctrl + 1
  {0xE2, V_KEY_CTRL2},          //ctrl + 2
  {0xE3, V_KEY_CTRL3},          //ctrl + 3
  {0xE4, V_KEY_CTRL4},          //ctrl + 4
  {0xE5, V_KEY_CTRL5},          //ctrl + 5
  {0xE6, V_KEY_CTRL6},          //ctrl + 6
  {0xE7, V_KEY_CTRL7},          //ctrl + 7
  {0xE8, V_KEY_CTRL8},          //ctrl + 8
  {0xE9, V_KEY_CTRL9},          //ctrl + 9
  {0xF0, V_KEY_CTRLS0},         //ctrl + s0
  {0xF1, V_KEY_CTRLS1},         //ctrl + s1
  {0xF2, V_KEY_CTRLS2},         //ctrl + s2
  {0xF3, V_KEY_CTRLS3},         //ctrl + s3
  {0xF4, V_KEY_CTRLS4},         //ctrl + s4
  {0xF5, V_KEY_CTRLS5},         //ctrl + s5
  {0xF6, V_KEY_CTRLS6},         //ctrl + s6
  {0xF7, V_KEY_CTRLS7},         //ctrl + s7
  {0xF8, V_KEY_CTRLS8},         //ctrl + s8
  {0xF9, V_KEY_CTRLS9},         //ctrl + s9
  {0xFF, V_KEY_UNKNOW},
};

#else
static key_map_t ir_keymap0[] =
{
  {0x0a, V_KEY_POWER},          //p
  {0x19, V_KEY_MUTE},           //j
  {0x12, V_KEY_RECALL},         //h
  {0x00, V_KEY_0},              //0
  {0x01, V_KEY_1},              //1
  {0x02, V_KEY_2},              //2
  {0x03, V_KEY_3},              //3
  {0x04, V_KEY_4},              //4
  {0x05, V_KEY_5},              //5
  {0x06, V_KEY_6},              //6
  {0x07, V_KEY_7},              //7
  {0x08, V_KEY_8},              //8
  {0x09, V_KEY_9},              //9
  {0x14, V_KEY_FAV},            //f
  {0x1C, V_KEY_TVRADIO},        //t
  {0x0B, V_KEY_UP},             //direction key up
  {0x11, V_KEY_LEFT},           //direction key left
  {0x0D, V_KEY_OK},             //enter
  {0x10, V_KEY_RIGHT},          //direction key right
  {0x0E, V_KEY_DOWN},           //direction key down
  {0x16, V_KEY_MENU},           //m
  {0x1F, V_KEY_CANCEL},         //c
  {0x1E, V_KEY_PAGE_UP},        //Page up
  {0x13, V_KEY_PAGE_DOWN},      //Page down
  {0x40, V_KEY_RED},            //'
  {0x42, V_KEY_GREEN},          ///
  {0x41, V_KEY_BLUE},           //.
  {0x43, V_KEY_YELLOW},         //,};
  {0x44, V_KEY_TTX},
  {0x17, V_KEY_EPG},
  {0x1D, V_KEY_AUDIO},
  {0x1B, V_KEY_F1},
  {0x45, V_KEY_F2},
  {0x46, V_KEY_F3},
  {0x47, V_KEY_F4},
  {0x48, V_KEY_INFO},
  {0X4D, V_KEY_REC},//recommend key
  {0x1A, V_KEY_LANG},//language switch
  {0x06, V_KEY_VUP},//volume up
  {0x0B, V_KEY_VDOWN},//volume down
  {0x1D, V_KEY_EXIT},//exit
  {0X1C, V_KEY_BACK},
  {0x2B, V_KEY_GAME},//game
  {0x2E, V_KEY_TV_PLAYBACK},//playback
  {0x3B, V_KEY_SEARCH},  //search
  {0x2F, V_KEY_MAIL},      //mail
  {0xFF, V_KEY_DATA_BROADCAST},             //DATA BOARDCAST
  {0xFF, V_KEY_NVOD},           //NVOD
  {0xFF, V_KEY_FAVUP},             //FAVUP
  {0xFF, V_KEY_FAVDOWN},            //FAVDOWN
  {0xFF, V_KEY_STOCK},            //stock
  {0xFF, V_KEY_BOOK},            //book
  {0xFF, V_KEY_RADIO},            //radio
  {0xFF, V_KEY_TV},            //tv
  {0xFF, V_KEY_F5},               //F5
  {0xFF, V_KEY_PAUSE},               //pause
  {0xFF, V_KEY_CHUP},            //channel up
  {0xFF, V_KEY_CHDOWN},            //channel down
  {0xFF, V_KEY_INPUT},            //input method
  {0xFF, V_KEY_HELP},               //help
  {0xFF, V_KEY_SET},               //set
  {0x55, V_KEY_SUBT},
  {0x57, V_KEY_UNKNOW},         //media
  {0x03, V_KEY_GOTO},
  {0x1c, V_KEY_ZOOM},
  {0x0c, V_KEY_VIDEO_MODE},
  {0x1f, V_KEY_ASPECT_MODE},
  {0xFF, V_KEY_CHANNEL_LIST},       //
  {0xFF, V_KEY_STOP},
  {0xFF, V_KEY_PREV},
  {0xFF, V_KEY_NEXT},
  {0xFF, V_KEY_BACK2},
  {0xFF, V_KEY_FORW2},
  {0xFF, V_KEY_LIST},
  {0xFF, V_KEY_PLAY},  
  {0xFF, V_KEY_PVR},  

};
static key_map_t ir_keymap1[] =
{
  {0x0a, V_KEY_POWER},          //p
  {0x19, V_KEY_MUTE},           //j
  {0x12, V_KEY_RECALL},         //h
  {0x00, V_KEY_0},              //0
  {0x01, V_KEY_1},              //1
  {0x02, V_KEY_2},              //2
  {0x03, V_KEY_3},              //3
  {0x04, V_KEY_4},              //4
  {0x05, V_KEY_5},              //5
  {0x06, V_KEY_6},              //6
  {0x07, V_KEY_7},              //7
  {0x08, V_KEY_8},              //8
  {0x09, V_KEY_9},              //9
  {0x14, V_KEY_FAV},            //f
  
  {0x1C, V_KEY_TVRADIO},        //t
  {0x0B, V_KEY_UP},             //direction key up
  {0x11, V_KEY_LEFT},           //direction key left
  {0x0D, V_KEY_OK},             //enter
  {0x10, V_KEY_RIGHT},          //direction key right
  {0x0E, V_KEY_DOWN},           //direction key down
  {0x16, V_KEY_MENU},           //m
  {0x1F, V_KEY_CANCEL},         //c
  {0x1E, V_KEY_PAGE_UP},        //Page up
  {0x13, V_KEY_PAGE_DOWN},      //Page down
  {0x40, V_KEY_RED},            //'
  {0x42, V_KEY_GREEN},          ///
  {0x41, V_KEY_BLUE},           //.
  {0x43, V_KEY_YELLOW},         //,};
  {0x44, V_KEY_TTX},
  {0x17, V_KEY_EPG},
  
  {0x1D, V_KEY_AUDIO},
  {0x1B, V_KEY_F1},
  {0x45, V_KEY_F2},
  {0x46, V_KEY_F3},
  {0x47, V_KEY_F4},
  {0x48, V_KEY_INFO},
  {0X4D, V_KEY_REC},//recommend key
  {0x1A, V_KEY_LANG},//language switch
  {0x06, V_KEY_VUP},//volume up
  {0x0B, V_KEY_VDOWN},//volume down
  {0x1D, V_KEY_EXIT},//exit
  {0X1C, V_KEY_BACK},
  {0x2B, V_KEY_GAME},//game
  {0x2E, V_KEY_TV_PLAYBACK},//playback
  {0x3B, V_KEY_SEARCH},  //search
  {0x2F, V_KEY_MAIL},      //mail
  
  {0xFF, V_KEY_DATA_BROADCAST},             //DATA BOARDCAST
  {0xFF, V_KEY_NVOD},           //NVOD
  {0xFF, V_KEY_FAVUP},             //FAVUP
  {0xFF, V_KEY_FAVDOWN},            //FAVDOWN
  {0xFF, V_KEY_STOCK},            //stock
  {0xFF, V_KEY_BOOK},            //book
  {0xFF, V_KEY_RADIO},            //radio
  {0xFF, V_KEY_TV},            //tv
  {0xFF, V_KEY_F5},               //F5
  {0xFF, V_KEY_PAUSE},               //pause
  {0xFF, V_KEY_CHUP},            //channel up
  {0xFF, V_KEY_CHDOWN},            //channel down
  {0xFF, V_KEY_INPUT},            //input method
  {0xFF, V_KEY_HELP},               //help
  {0xFF, V_KEY_SET},               //set
  {0x55, V_KEY_SUBT},
  
  {0x57, V_KEY_UNKNOW},         //media
  {0x03, V_KEY_GOTO},
  {0x1c, V_KEY_ZOOM},
  {0x0c, V_KEY_VIDEO_MODE},
  {0x1f, V_KEY_ASPECT_MODE},
  {0xFF, V_KEY_CHANNEL_LIST},       //
  {0xFF, V_KEY_STOP},
  {0xFF, V_KEY_PREV},
  {0xFF, V_KEY_NEXT},
  {0xFF, V_KEY_BACK2},
  {0xFF, V_KEY_FORW2},
  {0xFF, V_KEY_LIST},
  {0xFF, V_KEY_PLAY},      
  {0xFF, V_KEY_PVR},   
  {0xFF, V_KEY_REC},
};


static key_map_t ir_keymap2[] =
{
  {0x0a, V_KEY_POWER},          //p
  {0x19, V_KEY_MUTE},           //j
  {0x12, V_KEY_RECALL},         //h
  {0x00, V_KEY_0},              //0
  {0x01, V_KEY_1},              //1
  {0x02, V_KEY_2},              //2
  {0x03, V_KEY_3},              //3
  {0x04, V_KEY_4},              //4
  {0x05, V_KEY_5},              //5
  {0x06, V_KEY_6},              //6
  {0x07, V_KEY_7},              //7
  {0x08, V_KEY_8},              //8
  {0x09, V_KEY_9},              //9
  {0x14, V_KEY_FAV},            //f

  
  {0x1C, V_KEY_TVRADIO},        //t
  {0x0B, V_KEY_UP},             //direction key up
  {0x11, V_KEY_LEFT},           //direction key left
  {0x0D, V_KEY_OK},             //enter
  {0x10, V_KEY_RIGHT},          //direction key right
  {0x0E, V_KEY_DOWN},           //direction key down
  {0x16, V_KEY_MENU},           //m
  {0x1F, V_KEY_CANCEL},         //c
  {0x1E, V_KEY_PAGE_UP},        //Page up
  {0x13, V_KEY_PAGE_DOWN},      //Page down
  {0x40, V_KEY_RED},            //'
  {0x42, V_KEY_GREEN},          ///
  {0x41, V_KEY_BLUE},           //.
  {0x43, V_KEY_YELLOW},         //,};
  {0x44, V_KEY_TTX},
  {0x17, V_KEY_EPG},
  
  {0x1D, V_KEY_AUDIO},
  {0x1B, V_KEY_F1},
  {0x45, V_KEY_F2},
  {0x46, V_KEY_F3},
  {0x47, V_KEY_F4},
  {0x48, V_KEY_INFO},
  {0X4D, V_KEY_REC},//recommend key
  {0x1A, V_KEY_LANG},//language switch
  {0x06, V_KEY_VUP},//volume up
  {0x0B, V_KEY_VDOWN},//volume down
  {0x1D, V_KEY_EXIT},//exit
  {0X1C, V_KEY_BACK},
  {0x2B, V_KEY_GAME},//game
  {0x2E, V_KEY_TV_PLAYBACK},//playback
  {0x3B, V_KEY_SEARCH},  //search
  {0x2F, V_KEY_MAIL},   //mail

  {0xFF, V_KEY_DATA_BROADCAST},             //DATA BOARDCAST
  {0xFF, V_KEY_NVOD},           //NVOD
  {0xFF, V_KEY_FAVUP},             //FAVUP
  {0xFF, V_KEY_FAVDOWN},            //FAVDOWN
  {0xFF, V_KEY_STOCK},            //stock
  {0xFF, V_KEY_BOOK},            //book
  {0xFF, V_KEY_RADIO},            //radio
  {0xFF, V_KEY_TV},            //tv
  {0xFF, V_KEY_F5},               //F5
  {0xFF, V_KEY_PAUSE},               //pause
  {0xFF, V_KEY_CHUP},            //channel up
  {0xFF, V_KEY_CHDOWN},            //channel down
  {0xFF, V_KEY_INPUT},            //input method
  {0xFF, V_KEY_HELP},               //help
  {0xFF, V_KEY_SET},               //set
  {0x55, V_KEY_SUBT},
  
  {0x57, V_KEY_UNKNOW},         //media
  {0x03, V_KEY_GOTO},
  {0x1c, V_KEY_ZOOM},
  {0x0c, V_KEY_VIDEO_MODE},
  {0x1f, V_KEY_ASPECT_MODE},
  {0xFF, V_KEY_CHANNEL_LIST},       //
  {0xFF, V_KEY_STOP},
  {0xFF, V_KEY_PREV},
  {0xFF, V_KEY_NEXT},
  {0xFF, V_KEY_BACK2},
  {0xFF, V_KEY_FORW2},
  {0xFF, V_KEY_LIST},
  {0xFF, V_KEY_PLAY},   
  {0xFF, V_KEY_PVR},  
  
};
#endif


static key_map_t fp_keymap[] =
{
  {0x01, V_KEY_POWER},
  {0x08, V_KEY_UP},
  {0x04, V_KEY_DOWN},
  {0x10, V_KEY_LEFT},
  {0x20, V_KEY_RIGHT},
  {0x40, V_KEY_OK},
  {0x80, V_KEY_MENU},
  {0x04, V_KEY_VUP},
  {0x04, V_KEY_VDOWN},
};

#ifdef PATCH_ENABLE
static u16 enter_ucas_key_list[] =
{
  V_KEY_8,
  V_KEY_8,
  V_KEY_8,
  V_KEY_8,
};
#endif

static u16 enter_uart_ui[] =
{
  V_KEY_MUTE,
  V_KEY_9,
  V_KEY_9,
  V_KEY_9,
};

static u16 preset_pg_for_ae[] =
{
  V_KEY_1,
  V_KEY_5,
  V_KEY_9,
  V_KEY_3,
  V_KEY_5,
 };

static u16 enter_ca_hide_menu_key_list[] = 
{
  V_KEY_RED,
  V_KEY_1,
};

static u16 load_pg_aisat[] =
{
  V_KEY_2,
  V_KEY_4,
  V_KEY_7,
  V_KEY_2,
  V_KEY_8,
 };

static BOOL g_is_uiokey_disable = FALSE;

u32 last_ticks = 0;
u32 curn_ticks = 0;

#define MAX_MAGIC_LIST 14
#define GET_MAGIC_LIST_CNT(x) (sizeof(x)/sizeof(u16))

typedef struct
{
  u16 magic_key;
  u16 *p_key_list;
  u8 key_cnt;
  u8 key_state;
}magic_key_info_t;

magic_key_info_t g_magic_keylist[MAX_MAGIC_LIST];

static void init_magic_keylist(void)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for(i = 0; i < MAX_MAGIC_LIST; i++)
  {
    p_key_info->p_key_list = NULL;
    p_key_info->key_cnt = 0;
    p_key_info->key_state = 0;

    p_key_info++;
  }
}


//#ifdef PATCH_ENABLE
static BOOL register_magic_keylist(u16 *p_key_list, u8 key_cnt, u16 magic_key)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for(i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if(p_key_info->magic_key == V_KEY_INVALID)
    {
      p_key_info->magic_key = magic_key;
      p_key_info->p_key_list = p_key_list;
      p_key_info->key_cnt = key_cnt;
      p_key_info->key_state = 0;

      return TRUE;
    }

    p_key_info++;
  }

  return FALSE;
}


//#endif

static BOOL verify_key(magic_key_info_t *p_key_info, u16 key)
{
  if(key == p_key_info->p_key_list[p_key_info->key_state])
  {
    p_key_info->key_state++;
  }
  else
  {
    if(key == p_key_info->p_key_list[0])
    {
      p_key_info->key_state = 1;
    }
    else
    {
      p_key_info->key_state = 0;
    }
  }

  if(p_key_info->key_state == p_key_info->key_cnt)
  {
    p_key_info->key_state = 0;
    return TRUE;
  }

  return FALSE;
}


static void reset_magic_keylist(void)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for(i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if(p_key_info->magic_key != V_KEY_INVALID)
    {
      p_key_info->key_state = 0;
    }

    p_key_info++;
  }
}


static u16 detect_magic_keylist(u16 key)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  curn_ticks = mtos_ticks_get();

  if((curn_ticks - last_ticks) > 2000)
  {
    reset_magic_keylist();
  }

  last_ticks = curn_ticks;

  for(i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if(p_key_info->p_key_list != NULL)
    {
      if(verify_key(p_key_info, key))
      {
        return p_key_info->magic_key;
      }
    }

    p_key_info++;
  }

  return V_KEY_INVALID;
}


u8 get_key_value(u8 key_set, u8 vkey)
{
  u8 i = 0;
  key_map_t *p_map = NULL;
  u8 num = 0;

  if(key_set == 0)
  {
    p_map = ir_keymap0;
    num = sizeof(ir_keymap0) / sizeof(key_map_t);
  }
#ifndef WIN32
  else if (key_set == 1)
  {
    p_map = ir_keymap1;
    num = sizeof(ir_keymap1) / sizeof(key_map_t);
  }
  else if (key_set == 2)
  {
    p_map = ir_keymap2;
    num = sizeof(ir_keymap2) / sizeof(key_map_t);
  }
#endif
  else
  {
    p_map = fp_keymap;
    num = sizeof(fp_keymap) / sizeof(key_map_t);
  }

  for( ; i < num; i++)
  {
    if(p_map[i].v_key == vkey)
    {
      return p_map[i].h_key;
    }
  }

  OS_PRINTF("key_set = %d, vkey = %d\n", key_set, vkey);
  MT_ASSERT(0);
  return 0;
}


void ap_uio_init_kep_map(void)
{
  uio_device_t *dev;
  u8 rpt_key[8];
  dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                      SYS_DEV_TYPE_UIO);
#ifndef WIN32
  u8 i = 0;
  u8 cfg_tmp[128] = {0};
  u32 read_len = 0;
  u16 ir_usr_code[IRDA_MAX_USER] = {0};

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            IRKEY_BLOCK_ID, 0, 0,
                            128,
                            (u8 *)cfg_tmp);

  if(read_len > 0)
  {
    for(i = 0; i < sizeof(ir_keymap0) / sizeof(key_map_t); i++)
    {
      ir_keymap0[i].h_key = cfg_tmp[i+2];
    }

    i = 0;
    ir_usr_code[0] = (cfg_tmp[i] << 8) | cfg_tmp[i + 1];
  }

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                              IRKEY1_BLOCK_ID, 0, 0,
                              128,
                              (u8 *)cfg_tmp);

  if(read_len > 0)
  {
    for(i = 0; i < sizeof(ir_keymap1) / sizeof(key_map_t); i++)
    {
      ir_keymap1[i].h_key = cfg_tmp[i+2];
    }

    i = 0;
    ir_usr_code[1] = (cfg_tmp[i] << 8) | cfg_tmp[i + 1];
  }


  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            IRKEY2_BLOCK_ID, 0, 0,
                            128,
                            (u8 *)cfg_tmp);

  if(read_len > 0)
  {
    for(i = 0; i < sizeof(ir_keymap2) / sizeof(key_map_t); i++)
    {
      ir_keymap2[i].h_key = cfg_tmp[i+2];
    }

    i = 0;
    ir_usr_code[2] = (cfg_tmp[i] << 8) | cfg_tmp[i + 1];
  }


  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     FPKEY_BLOCK_ID, 0, 0,
                     128,
                     (u8 *)cfg_tmp);

  if(read_len > 0)
  {
    for(i = 0; i < sizeof(fp_keymap) / sizeof(key_map_t); i++)
    {
      fp_keymap[i].h_key = cfg_tmp[i];
    }
  }

  uio_set_user_code(dev, 3, ir_usr_code);
#endif

  rpt_key[0] = get_key_value(KEY_RC_SETS_1, V_KEY_DOWN);
  rpt_key[1] = get_key_value(KEY_RC_SETS_1, V_KEY_UP);
  rpt_key[2] = get_key_value(KEY_RC_SETS_1, V_KEY_RIGHT);
  rpt_key[3] = get_key_value(KEY_RC_SETS_1, V_KEY_LEFT);
  rpt_key[4] = get_key_value(KEY_RC_SETS_1, V_KEY_VDOWN);
  rpt_key[5] = get_key_value(KEY_RC_SETS_1, V_KEY_VUP);
#ifndef WIN32
  rpt_key[6] = get_key_value(KEY_RC_SETS_1, V_KEY_CHDOWN);
  rpt_key[7] = get_key_value(KEY_RC_SETS_1, V_KEY_CHUP);
#endif
  uio_set_rpt_key(dev, UIO_IRDA, rpt_key,
                  sizeof(rpt_key) / sizeof(u8), KEY_RC_SETS_1);

  rpt_key[0] = get_key_value(KEY_RC_SETS_2, V_KEY_DOWN);
  rpt_key[1] = get_key_value(KEY_RC_SETS_2, V_KEY_UP);
  rpt_key[2] = get_key_value(KEY_RC_SETS_2, V_KEY_RIGHT);
  rpt_key[3] = get_key_value(KEY_RC_SETS_2, V_KEY_LEFT);
#ifndef WIN32
  rpt_key[4] = get_key_value(KEY_RC_SETS_2, V_KEY_VDOWN);
  rpt_key[5] = get_key_value(KEY_RC_SETS_2, V_KEY_VUP);
  rpt_key[6] = get_key_value(KEY_RC_SETS_2, V_KEY_CHDOWN);
  rpt_key[7] = get_key_value(KEY_RC_SETS_2, V_KEY_CHUP);
#endif
  uio_set_rpt_key(dev, UIO_IRDA, rpt_key,
                  sizeof(rpt_key) / sizeof(u8), KEY_RC_SETS_2);

  rpt_key[0] = get_key_value(KEY_RC_SETS_3, V_KEY_DOWN);
  rpt_key[1] = get_key_value(KEY_RC_SETS_3, V_KEY_UP);
  rpt_key[2] = get_key_value(KEY_RC_SETS_3, V_KEY_RIGHT);
  rpt_key[3] = get_key_value(KEY_RC_SETS_3, V_KEY_LEFT);
#ifndef WIN32
  rpt_key[4] = get_key_value(KEY_RC_SETS_3, V_KEY_VDOWN);
  rpt_key[5] = get_key_value(KEY_RC_SETS_3, V_KEY_VUP);
  rpt_key[6] = get_key_value(KEY_RC_SETS_3, V_KEY_CHDOWN);
  rpt_key[7] = get_key_value(KEY_RC_SETS_3, V_KEY_CHUP);
#endif
  uio_set_rpt_key(dev, UIO_IRDA, rpt_key,
                  sizeof(rpt_key) / sizeof(u8), KEY_RC_SETS_3);

  rpt_key[0] = get_key_value(KEY_FP_SET, V_KEY_DOWN);
  rpt_key[1] = get_key_value(KEY_FP_SET, V_KEY_UP);
  rpt_key[2] = get_key_value(KEY_FP_SET, V_KEY_RIGHT);
  rpt_key[3] = get_key_value(KEY_FP_SET, V_KEY_LEFT);
  uio_set_rpt_key(dev, UIO_FRONTPANEL, rpt_key,
                  sizeof(rpt_key) / sizeof(u8), KEY_FP_SET);


  init_magic_keylist();

  register_magic_keylist(enter_uart_ui, GET_MAGIC_LIST_CNT(enter_uart_ui), V_KEY_BISS_KEY);

  register_magic_keylist(preset_pg_for_ae, GET_MAGIC_LIST_CNT(preset_pg_for_ae), V_KEY_TV_GUIDE);

  register_magic_keylist(enter_ca_hide_menu_key_list, GET_MAGIC_LIST_CNT(enter_ca_hide_menu_key_list), V_KEY_CA_HIDE_MENU);

  register_magic_keylist(load_pg_aisat, GET_MAGIC_LIST_CNT(load_pg_aisat), V_KEY_LOAD_PG);
  
}

BOOL ap_uio_key_disable(u16 vkey)
{

  if(!g_is_uiokey_disable)
  {
      return FALSE;
  }
  else
  {
    if(vkey != V_KEY_POWER)
    {
      return TRUE;
    }
    else
    {      
      return FALSE;
    }
  }

  //OS_PRINTF("\r\n ***ap_uio_key_disable vkey value[%d]*** ", vkey);
  return TRUE;
}

BOOL get_uio_key_stayus(void)
{
  return g_is_uiokey_disable;
}

void set_uio_key_status(BOOL is_disable)
{
  g_is_uiokey_disable = is_disable;
}

BOOL ap_uio_translate_key(v_key_t *key)
{
  key_map_t *key_map = NULL;
  u32 key_num = 0;
  u32 i = 0;
  u16 magic_key = V_KEY_INVALID;
  BOOL bRet = FALSE;
  u8 keyIndex = 1;
  u16 v_key_detect = V_KEY_INVALID;
  u16 v_key_input = key->v_key;

  MT_ASSERT(key != NULL);
  
app_printf(PRINTF_DEBUG_LEVEL,"ap_uio_translate_key key->type:%d, key->usr:%d, key->v_key:0x%x\n", key->type, key->usr, key->v_key);
#if 0
  if(key->v_key == 0x1 || key->v_key == 0xff)
  	return FALSE;
 #endif

  //lint -e613

  if(key->type == IRDA_KEY)
  {
    if(key->usr == 0)
    {
      key_map = ir_keymap0;
      key_num = sizeof(ir_keymap0) / sizeof(key_map_t);
    }
#ifndef WIN32
    else if(key->usr == 1)
    {
      key_map = ir_keymap1;
      key_num = sizeof(ir_keymap1) / sizeof(key_map_t);
    }
    else if (key->usr == 2)
    {
      key_map = ir_keymap2;
      key_num = sizeof(ir_keymap2) / sizeof(key_map_t);
    }
    else
    {
      key_map = ir_keymap0;
      key_num = sizeof(ir_keymap0) / sizeof(key_map_t);
    }
#endif

  if(g_customer.cus_ir_led_set != NULL)
		g_customer.cus_ir_led_set();
  }
  else if(key->type == FP_KEY)
  {
    key_map = fp_keymap;
    key_num = sizeof(fp_keymap) / sizeof(key_map_t);
  }
  else
  {
    key_map = ir_keymap0;
    key_num = sizeof(ir_keymap0) / sizeof(key_map_t);
  }
  MT_ASSERT(key_map != NULL);
  for(i = 0; i < key_num; i++)
  {
    key_map_t *temp = key_map + i;

    if(temp->h_key == v_key_input)
    {
      OS_PRINTF("AP_UIO: one key translated(h_key: %d, v_key: %d)\n", key->v_key, temp->v_key);

      switch(keyIndex)
      {
        case 1:
          key->v_key = temp->v_key;
          v_key_detect = key->v_key;
          keyIndex++;
          break;

        case 2:
          key->v_key_2 = temp->v_key;
          v_key_detect = key->v_key_2;
          keyIndex++;
          break;

        case 3:
          key->v_key_3 = temp->v_key;
          v_key_detect = key->v_key_3;
          keyIndex++;
          break;

        case 4:
          key->v_key_4 = temp->v_key;
          v_key_detect = key->v_key_4;
          keyIndex++;
          break;

        default:
          return bRet;
      }

      magic_key = detect_magic_keylist(v_key_detect);
      if(magic_key != V_KEY_INVALID)
      {
        //detect key serial succedd, change to magic.

        switch(keyIndex)
        {
          case 1:
            key->v_key = magic_key;
            break;

          case 2:
            key->v_key_2 = magic_key;
            break;

          case 3:
            key->v_key_3 = magic_key;
            break;

          case 4:
            key->v_key_4 = magic_key;
            break;

          default:
            key->v_key = magic_key;
            break;
        }
      }

      bRet = TRUE;
    }
  }
  
  if(ap_uio_key_disable(key->v_key))
  {
    return FALSE;
  }
  //lint +e613
  #ifdef MIS_ADS
  {
     extern BOOL ads_release_key_to_MIS(u8 is_release, u16 v_key);
     bRet = ads_release_key_to_MIS(2,key->v_key);
  }
  #endif

          OS_PRINTF("%s,%d, bert:%d \n",__FUNCTION__,__LINE__,bRet);

  return bRet;
}


ap_uio_policy_t *construct_ap_uio_policy(void)
{
  ap_uio_policy_t *p_uio_policy = mtos_malloc(sizeof(ap_uio_policy_t));

  MT_ASSERT(p_uio_policy != NULL);
  //lint -e668
  memset(p_uio_policy, 0, sizeof(ap_uio_policy_t));
  //lint +e668
  
  p_uio_policy->p_init_kep_map = ap_uio_init_kep_map;
  p_uio_policy->p_translate_key = ap_uio_translate_key;

  return p_uio_policy;
}


void destruct_ap_uio_policy(ap_uio_policy_t *p_uio_policy)
{
  mtos_free(p_uio_policy);
}
