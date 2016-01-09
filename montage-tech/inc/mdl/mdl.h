/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MDL_H_
#define __MDL_H_

/*!
  All supported virtual keys
  */
enum v_key
{
  /*!
    Invalid key
    */
  V_KEY_INVALID = 0,
  /*!
    Power key
    */
  V_KEY_POWER = 1,
  /*!
    Mute key
    */
  V_KEY_MUTE = 2,
  /*!
    Recall key 
    */
  V_KEY_RECALL = 3,
  /*!
    TV/Radio switch
    */
  V_KEY_TVRADIO = 4,
  /*!
    Key 0
    */
  V_KEY_0 = 5,
  /*!
    Key 1 
    */
  V_KEY_1 = 6,
  /*!
    Key 2 
    */
  V_KEY_2 = 7,
  /*!
    Key 3
    */
  V_KEY_3 = 8,
  /*!
    Key 4
    */
  V_KEY_4 = 9,
  /*!
    Key 5
    */
  V_KEY_5 = 10,
  /*!
    Key 6
    */
  V_KEY_6 = 11,
  /*!
    Key 7
    */
  V_KEY_7 = 12,
  /*!
    Key 8
    */
  V_KEY_8 = 13,
  /*!
    Key 9
    */
  V_KEY_9 = 14,
  /*!
    Cancel key
    */
  V_KEY_CANCEL = 15,
  /*!
    OK key
    */
  V_KEY_OK = 16,
  /*!
    Up key
    */
  V_KEY_UP = 17,
  /*!
    Down key
    */
  V_KEY_DOWN = 18,
  /*!
    Left key
    */
  V_KEY_LEFT = 19,
  /*!
    Right key
    */
  V_KEY_RIGHT = 20,
  /*!
    Menu key
    */
  V_KEY_MENU = 21,
  /*!
    Porgram list key
    */
  V_KEY_PROGLIST = 22,
  /*!
    Audio key
    */
  V_KEY_AUDIO = 23,
  /*!
    Page up key
    */
  V_KEY_PAGE_UP = 24,
  /*!
    Page down key
    */
  V_KEY_PAGE_DOWN = 25,
  /*!
    Infor Key
    */
  V_KEY_INFO = 26,
  /*!
    Favorite key
    */
  V_KEY_FAV = 27,
  /*!
    Pause key
    */
  V_KEY_PAUSE = 28,
  /*!
    Play key
    */
  V_KEY_PLAY = 29,
  /*!
    9 Picture key
    */
  V_KEY_9PIC = 30,
  /*!
    EPG key
    */
  V_KEY_EPG = 31,
  /*!
    SI key
    */
  V_KEY_SI = 32,
  /*!
    Gray key
    */
  V_KEY_GRAY = 33,
  /*!
    Red key 
    */
  V_KEY_RED = 34,
  /*!
    Green key
    */
  V_KEY_GREEN = 35,
  /*!
    Yellow key
    */
  V_KEY_YELLOW = 36,
  /*!
    Blue key
    */
  V_KEY_BLUE = 37,
  /*!
    UPG key
    */
  V_KEY_UPG = 38,
  /*!
    TTX key
    */
  V_KEY_TTX = 39,
  /*!
    Test key
    */
  V_KEY_TEST = 40,
  /*!
    I2C key
    */
  V_KEY_I2C = 41,
  /*!
    F2 Key
    */
  V_KEY_F1 = 42,
  /*!
    F2 Key
    */
  V_KEY_F2 = 43,
  /*!
    F3 Key
    */
  V_KEY_F3 = 44,
  /*!
    F4 Key
    */
  V_KEY_F4 = 45,
  /*!
    F5 Key
    */
  V_KEY_F5 = 46,
  /*!
    STOCK Key
    */
  V_KEY_STOCK = 47,
  /*!
    BOOK Key
    */
  V_KEY_BOOK = 48,
  /*!
    TV Key
    */
  V_KEY_TV = 49,
  /*!
    RADIO Key
    */
  V_KEY_RADIO = 50,
  /*!
    OTA force key
    */
  V_KEY_OTA_FORCE = 51,
  /*!
    SAT key
    */
  V_KEY_SAT = 52,
  /*!
    ENTER ucas
    */
  V_KEY_UCAS = 53,
  /*!
    ENTER satcode
    */
  V_KEY_SAT_CODE = 54,
  /*!
    ctrl + 0
    */
  V_KEY_CTRL0 = 55,
  /*!
    ctrl + 1
    */
  V_KEY_CTRL1 = 56,
  /*!
    ctrl + 2
    */  
  V_KEY_CTRL2 = 57,
  /*!
    ctrl + 3
    */  
  V_KEY_CTRL3 = 58,
  /*!
    ctrl + 4
    */  
  V_KEY_CTRL4 = 59,
  /*!
    ctrl + 5
    */  
  V_KEY_CTRL5 = 60,
  /*!
    ctrl + 6
    */  
  V_KEY_CTRL6 = 61,
  /*!
    ctrl + 7
    */  
  V_KEY_CTRL7 = 62,
  /*!
    ctrl + 8
    */  
  V_KEY_CTRL8 = 63,
  /*!
    ctrl + 9
    */  
  V_KEY_CTRL9 = 64, 
  /*!
    ctrl + s0
    */
  V_KEY_CTRLS0 = 65,
  /*!
    ctrl + s1
    */
  V_KEY_CTRLS1 = 66,
  /*!
    ctrl + s2
    */  
  V_KEY_CTRLS2 = 67,
  /*!
    ctrl + s3
    */  
  V_KEY_CTRLS3 = 68,
  /*!
    ctrl + s4
    */  
  V_KEY_CTRLS4 = 69,
  /*!
    ctrl + s5
    */  
  V_KEY_CTRLS5 = 70,
  /*!
    ctrl + s6
    */  
  V_KEY_CTRLS6 = 71,
  /*!
    ctrl + s7
    */  
  V_KEY_CTRLS7 = 72,
  /*!
    ctrl + s8
    */  
  V_KEY_CTRLS8 = 73,
  /*!
    ctrl + s9
    */  
  V_KEY_CTRLS9 = 74,
  /*!
    pos
    */  
  V_KEY_POS = 75,
  /*!
    rec
    */  
  V_KEY_REC = 76,
  /*!
    pn
    */  
  V_KEY_PN = 77,
  /*!
    ts
    */  
  V_KEY_TS = 78,
  /*!
    back2
    */  
  V_KEY_BACK2 = 79,
  /*!
    forw2
    */  
  V_KEY_FORW2 = 80,
  /*!
    back
    */  
  V_KEY_BACK = 81,
  /*!
    forw
    */  
  V_KEY_FORW = 82,
  /*!
    revslow
    */  
  V_KEY_REVSLOW = 83,
  /*!
    slow
    */  
  V_KEY_SLOW = 84,
  /*!
    stop
    */  
  V_KEY_STOP = 85,
  /*!
    vdown
    */  
  V_KEY_VDOWN = 86,
  /*!
    vup
    */  
  V_KEY_VUP = 87,
  /*!
    subt
    */  
  V_KEY_SUBT = 88,
  /*!
    zoom
    */  
  V_KEY_ZOOM = 89,
  /*!
    tvsat
    */  
  V_KEY_TVSAT = 90,
  /*!
    find
    */  
  V_KEY_FIND = 91,
  /*!
    d
    */  
  V_KEY_D = 92,
  /*!
    tvav
    */  
  V_KEY_TVAV = 93,
  /*!
    twin port
    */
  V_KEY_TWIN_PORT = 94,
  /*!
    CA Hide Menu
    */
  V_KEY_CA_HIDE_MENU = 95,
  /*!
    colorbar
    */
  V_KEY_COLORBAR = 96,
  /*!
    scan
    */
  V_KEY_SCAN = 97,
  /*!
    tp list
    */
  V_KEY_TPLIST = 98,
  /*!
    scan
    */
  V_KEY_VOLUP = 99,
  /*!
    tp list
    */
  V_KEY_VOLDOWN = 100,
  /*!
    language switch
    */
  V_KEY_LANG = 101,
  /*!
    exit
    */
  V_KEY_EXIT = 102,
  /*!
    sleep
    */
  V_KEY_SLEEP = 103,
  /*!
    video mode
    */
  V_KEY_VIDEO_MODE = 104,
  /*!
    game
    */
  V_KEY_GAME = 105,  
  /*!
    tv playback
    */
  V_KEY_TV_PLAYBACK = 106,  
  /*!
    p2p
  */
  V_KEY_P2P = 107,
  /*!
    VBI_INSERTER
  */
  V_KEY_VBI_INSERTER = 108,
  /*!
    ASPECT_MODE
  */
  V_KEY_ASPECT_MODE = 109,
  /*!
    DISPLAY_MODE
  */
  V_KEY_DISPLAY_MODE = 110,
  /*!
    LANGUAGE_SWITCH
  */
  V_KEY_LANGUAGE_SWITCH = 111,
  /*!
    PLAY_TYPE
  */
  V_KEY_PLAY_TYPE = 112,
  /*!
    BEEPER
  */
  V_KEY_BEEPER = 113,
  /*!
    CHANNEL_CHANGE
  */
  V_KEY_CHANNEL_CHANGE = 114,
  /*!
    LNB_POWER
  */
  V_KEY_LNB_POWER = 115,
  /*!
    OSD_TRANSPARENCY
  */
  V_KEY_OSD_TRANSPARENCY = 116,
  /*!
    LOOP_THROUGH
  */
  V_KEY_LOOP_THROUGH = 117,
  /*!
    BISS_KEY
  */
  V_KEY_BISS_KEY = 118,
  /*!
    SEARCH key
    */
  V_KEY_SEARCH = 119,
  /*!
    SEARCH key
    */
  V_KEY_MAIL = 120,
  /*!
    FACTORY key
    */
  V_KEY_FACTORY = 121,
  /*!
    FavUp key
    */
  V_KEY_FAVUP = 122,
  /*!
    FavDown key
    */
  V_KEY_FAVDOWN = 123,
  /*!
    NVOD key
    */
  V_KEY_NVOD = 124,
  /*!
    Data broadcast key
    */
  V_KEY_DATA_BROADCAST = 125,
      /*!
    Goto key
    */
  V_KEY_GOTO = 126,
    /*!
    CA Info key
    */
  V_KEY_CA_INFO = 127,
    /*!
    BISS and CryptoWorks SUPER Password_KEY
  */
  V_KEY_SUPERPASSWORD_KEY = 128,
    /*!
    previous
    */
  V_KEY_PREV = 129,
    /*!
    next
    */
  V_KEY_NEXT = 130,
    /*!
    repeat
    */
  V_KEY_REPEAT = 131,
    /*!
    list
    */
  V_KEY_LIST = 132,
    /*!
    gbox
    */
  V_KEY_GBOX = 133,
  /*!
    channel up
    */
  V_KEY_CHUP = 134,
    /*!
    channel down
    */
  V_KEY_CHDOWN = 135,
    /*!
    input
    */
  V_KEY_INPUT = 136,
    /*!
    help
    */
  V_KEY_HELP = 137,
    /*!
    set
    */
  V_KEY_SET = 138,
    /*!
    channel list
    */
  V_KEY_CHANNEL_LIST = 139,
  /*!
    customer define key
    */
  V_KEY_CUST_DEFINE_START = 140,
  /*!
    Unkown key
    */
  V_KEY_UNKNOW = V_KEY_CUST_DEFINE_START + 500,
};

/*!
  \file mdl.h
  This file provides the common definition used in all files of mid-ware.
  The interface from MDL to AP also defined here
  */
/*! MDL enum module*/
enum
{
  /*!
    MDL module list start
    */
  MDL_MODULE_START = 0,
  /*!
    MDL network interface module control module
    */  
  MDL_NIM_CTRL,
  /*!
    MDL engine module
    */
  MDL_DVB,
  /*!
    Transport shell module
    */
  MDL_TRANSPORT_SHELL,
  /*!
    MDL EPG data module
    */
  MDL_EPG_DATA,
  /*!
    MDL VBI module
    */
  MDL_VBI,
  /*!
    MDL PNP SERVICE module
    */
  MDL_PNP_SVC,
  /*!
    MDL SUBT module
    */
  MDL_SUBT,
  /*!
    MDL MONITOR SERVICE module
    */
  MDL_MONITOR_SVC,
  /*!
    MDL MONITOR SERVICE module
    */
  MDL_NET_SVC,
  /*!
    MDL MONITOR SERVICE module
    */
  MDL_HTTPD_SVC,
  /*!
    MDL module list end
    */
  MDL_MODULE_END
};

/*!
  Initialize middleware utility module
  */
void mdl_init(void);
/*!
  Set message queue id

  \param[in] handle class handle
  \param[in] id queue id
  */
void mdl_set_msgq(handle_t handle, u32 id);

/*!
  Broadcast message

  \param[in] p_msg message information to be broadcast
  */
void mdl_broadcast_msg(os_msg_t *p_msg);

/*!
  Send message
  
  \param[in] p_msg message information to be sent
  */
void mdl_send_msg(os_msg_t *p_msg);

#endif // End for __MDL_H_
