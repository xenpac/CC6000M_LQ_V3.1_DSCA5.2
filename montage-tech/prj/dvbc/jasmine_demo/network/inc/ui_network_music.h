/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef _UI_NETWORK_MUSIC_H_
#define _UI_NETWORK_MUSIC_H_

#include "ui_common.h"



// main info contain   
#define NM_MAIN_INFO_CONT_X  120
#define NM_MAIN_INFO_CONT_Y  90
#define NM_MAIN_INFO_CONT_W  200
#define NM_MAIN_INFO_CONT_H  560 

//mode contain 
#define NM_MODE_CONT_X  10
#define NM_MODE_CONT_Y  20
#define NM_MODE_CONT_W  NM_MAIN_INFO_CONT_W - 2*NM_MODE_CONT_X
#define NM_MODE_CONT_H  120

//web site 
#define NM_WEB_SITE_X   0
#define NM_WEB_SITE_Y   0
#define NM_WEB_SITE_W   180
#define NM_WEB_SITE_H   70

//mode name 
#define NM_MODE_NAME_X  0
#define NM_MODE_NAME_Y  (NM_WEB_SITE_Y + NM_WEB_SITE_H)
#define NM_MODE_NAME_W  NM_MODE_CONT_W
#define NM_MODE_NAME_H  NM_MODE_CONT_H - NM_WEB_SITE_H

//pop main list 
#define NM_POP_MAIN_LIST_X  10
#define NM_POP_MAIN_LIST_Y  NM_MODE_CONT_Y + NM_MODE_CONT_H + 20
#define NM_POP_MAIN_LIST_W  NM_MODE_CONT_W
#define NM_POP_MAIN_LIST_H  200

#define NM_POP_LIST_ITEM_COUNT        3
#define NM_POP_LIST_ITEM_NUM_ONE_PAGE 3

//left side cover_pic 
#define NM_COVER_PIC_CONT_X  20
#define NM_COVER_PIC_CONT_Y  NM_POP_MAIN_LIST_Y + NM_POP_MAIN_LIST_H
#define NM_COVER_PIC_CONT_W  160
#define NM_COVER_PIC_CONT_H  160

//load pic bmp
#define NM_COVER_LOAD_BMP_X 5
#define NM_COVER_LOAD_BMP_Y 5
#define NM_COVER_LOAD_BMP_W 150
#define NM_COVER_LOAD_BMP_H 150



//page container  
#define NM_PAGE_CONTX       330
#define NM_PAGE_CONTY       90
#define NM_PAGE_CONTW       830
#define NM_PAGE_CONTH       40

//page arrow left
#define NM_PAGE_ARROWL_X    150
#define NM_PAGE_ARROWL_Y    2
#define NM_PAGE_ARROWL_W    36
#define NM_PAGE_ARROWL_H    36
//page arrow right
#define NM_PAGE_ARROWR_X    (NM_PAGE_CONTW - NM_PAGE_ARROWL_X - NM_PAGE_ARROWR_W)
#define NM_PAGE_ARROWR_Y    2
#define NM_PAGE_ARROWR_W    36
#define NM_PAGE_ARROWR_H    36
//page name
#define NM_PAGE_NAME_X      250
#define NM_PAGE_NAME_Y      2
#define NM_PAGE_NAME_W      100
#define NM_PAGE_NAME_H      36
//page value
#define NM_PAGE_VALUE_X     450
#define NM_PAGE_VALUE_Y     2
#define NM_PAGE_VALUE_W     100
#define NM_PAGE_VALUE_H     36

//music track list cont 
#define NM_TRACK_LIST_CONT_X 330
#define NM_TRACK_LIST_CONT_Y 140
#define NM_TRACK_LIST_CONT_W 830 
#define NM_TRACK_LIST_CONT_H 400 //70×5+10×5

//track list ok
#define NM_TRACK_LIST_X     0
#define NM_TRACK_LIST_Y     0
#define NM_TRACK_LIST_W     NM_TRACK_LIST_CONT_W
#define NM_TRACK_LIST_H     360//400
#define NM_TRACK_ITEM_V_GAP 10

#define NM_TRACK_LIST_ITEM_NUM_ONE_PAGE  5

//album list
//music album cont 
#define NM_ALBUM_ARTIST_CONT_X 330
#define NM_ALBUM_ARTIST_CONT_Y 140
#define NM_ALBUM_ARTIST_CONT_W 830 //250×3海报宽度+20×4间隔
#define NM_ALBUM_ARTIST_CONT_H 400 //140×3+10×3
//cell cont 
#define NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE 10
#define NM_ALBUM_ARTIST_CELL_X 0
#define NM_ALBUM_ARTIST_CELL_Y 0
#define NM_ALBUM_ARTIST_CELL_W 150
#define NM_ALBUM_ARTIST_CELL_H 200
#define NM_ALBUM_ARTIST_CELL_H_GAP 20
#define NM_ALBUM_ARTIST_CELL_V_GAP 0
//cell pic cont 
#define NM_ALBUM_SINGER_CELL_PIC_X 5
#define NM_ALBUM_SINGER_CELL_PIC_Y 5
#define NM_ALBUM_SINGER_CELL_PIC_W 140
#define NM_ALBUM_SINGER_CELL_PIC_H 140
//cell name text 
#define NM_ALBUM_SINGER_CELL_NAME_X 5
#define NM_ALBUM_SINGER_CELL_NAME_Y 145
#define NM_ALBUM_SINGER_CELL_NAME_W 140
#define NM_ALBUM_SINGER_CELL_NAME_H 40



//play  
#define NM_PLAY_CONT_X 330
#define NM_PLAY_CONT_Y 550
#define NM_PLAY_CONT_W 830
#define NM_PLAY_CONT_H 100

//playing track name 
#define NM_PLAY_TRACK_NAME_X 10
#define NM_PLAY_TRACK_NAME_Y 10
#define NM_PLAY_TRACK_NAME_W 800
#define NM_PLAY_TRACK_NAME_H 40

//play/pause 
#define NM_PLAY_PLAY_ICON_X 40
#define NM_PLAY_PLAY_ICON_Y 65
#define NM_PLAY_PLAY_ICON_W 20
#define NM_PLAY_PLAY_ICON_H 20
//repeat  
#define NM_PLAY_REPEAT_ICON_X 10
#define NM_PLAY_REPEAT_ICON_Y 65
#define NM_PLAY_REPEAT_ICON_W 20
#define NM_PLAY_REPEAT_ICON_H 20

//play progress
#define NM_PLAY_PROGRESS_X 150 
#define NM_PLAY_PROGRESS_Y 70
#define NM_PLAY_PROGRESS_W 550
#define NM_PLAY_PROGRESS_H 10

#define NM_PLAY_PROGRESS_MIDX 0
#define NM_PLAY_PROGRESS_MIDY 0
#define NM_PLAY_PROGRESS_MIDW 550
#define NM_PLAY_PROGRESS_MIDH 10

#define NM_PLAY_CURTM_X 80
#define NM_PLAY_CURTM_Y 60
#define NM_PLAY_CURTM_W 60
#define NM_PLAY_CURTM_H 30

#define NM_PLAY_TOLTM_X 720
#define NM_PLAY_TOLTM_Y 60
#define NM_PLAY_TOLTM_W 60
#define NM_PLAY_TOLTM_H 30



//bottom help container
#define NM_BOTTOM_HELP_X  120
#define NM_BOTTOM_HELP_Y  660
#define NM_BOTTOM_HELP_W  1040
#define NM_BOTTOM_HELP_H  50


#define RSI_NM_DETAIL  RSI_BOX_1

#define NM_PLAY_PROGRESS_MIN   RSI_PROGRESS_BAR_MID_BLUE
#define NM_PLAY_PROGRESS_MAX RSI_PROGRESS_BAR_BG//RSI_PLAY_PBAR_BACK
#define NM_PLAY_PROGRESS_MID INVALID_RSTYLE_IDX

#define FSI_NM_INFO_MBOX FSI_WHITE

#define NETWORK_MUSIC_PLAY_MODE_COUNT 4
typedef enum
{
  PLAY_MODE_INVALID = 0,
  PLAY_MODE_CYCLE_CUR_LIST,  
  PLAY_MODE_CYCLE_LIST_RADOM,
  PLAY_MODE_CYCLE_CUR_SONG,
  PLAY_MODE_NO_REPEAT,
}network_music_play_mode_t;

typedef enum
{
  NETWORK_MUSIC_STAT_INVALID = 0,
  NETWORK_MUSIC_STAT_FB,
  NETWORK_MUSIC_STAT_FF,
  NETWORK_MUSIC_STAT_PLAY,
  NETWORK_MUSIC_STAT_STOP,
  NETWORK_MUSIC_STAT_PAUSE,
 
}network_music_play_state_t;


RET_CODE open_network_music(u32 para1, u32 para2);

#endif