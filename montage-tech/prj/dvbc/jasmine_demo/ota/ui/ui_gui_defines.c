/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************
****************************************************************************/
#include "ui_common.h"

rsc_rstyle_t c_tab[MAX_RSTYLE_CNT] =
{
  // RSI_IGNORE
  { 
    {R_IGNORE}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_TRANSPARENT
  { 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_FILL_RECT_TO_BORDER, C_TRANS}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}, 
    {R_IGNORE}
  },

  //RSI_BLUE
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLUE},
    {R_IGNORE},
    {R_IGNORE}, 
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_YELLOW
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_YELLOW},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_WHITE
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_WHITE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_BLACK
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLACK},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_DLG_FRM
  { 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_FILL_RECT_TO_BORDER, C_BLUE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}
  },
};

rsc_fstyle_t f_tab[MAX_FSTYLE_CNT] =
{
  //FSI_BLACK
  { FONT_ENGLISH, C_BLACK,},
  //FSI_WHITE
  { FONT_ENGLISH, C_WHITE,},

};

rsc_config_t g_rsc_config =
{
  512,         // common data buf size
  0,//80 * 336,    //max bitmap size,every pixel cost 2 Byte
  0,           //max language size,every char cast 2 Bytes
  256 * 4,     //palette size,every color cost 2 Bytes
  0,           //must bigger than latin font size
  0,           // script buffer size
  0,           //offset for block
  0,           //flash base addr
  RSTYLE_CNT,
  c_tab,
  FSTYLE_CNT,
  f_tab,

};

