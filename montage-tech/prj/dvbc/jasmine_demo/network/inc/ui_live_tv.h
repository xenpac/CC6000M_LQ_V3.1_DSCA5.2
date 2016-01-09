/****************************************************************************

****************************************************************************/
#ifndef __UI_LIVE_TV_H__
#define __UI_LIVE_TV_H__

/* coordinate */
#define LIVE_TV_CONT_X             ((SCREEN_WIDTH - LIVE_TV_CONT_W) / 2)
#define LIVE_TV_CONT_Y             (SCREEN_HEIGHT - LIVE_TV_CONT_H)
#define LIVE_TV_CONT_W             SCREEN_WIDTH
#define LIVE_TV_CONT_H             SCREEN_HEIGHT

//list container
#define LIVE_TV_LIST_CONT_X        50
#define LIVE_TV_LIST_CONT_Y        60//80
#define LIVE_TV_LIST_CONT_W        320
#define LIVE_TV_LIST_CONT_H        525

//liveTV group container
#define LIVE_TV_GROUP_CONT_X 0
#define LIVE_TV_GROUP_CONT_Y 0
#define LIVE_TV_GROUP_CONT_W (LIVE_TV_LIST_CONT_W - 2*LIVE_TV_GROUP_CONT_X)
#define LIVE_TV_GROUP_CONT_H 65

//liveTV group left bmp
#define LIVE_TV_GROUPL_X       0
#define LIVE_TV_GROUPL_Y       ((LIVE_TV_GROUP_CONT_H - LIVE_TV_GROUPL_H) >> 1)
#define LIVE_TV_GROUPL_W      36
#define LIVE_TV_GROUPL_H       LIVE_TV_GROUP_CONT_H

//liveTV group right bmp
#define LIVE_TV_GROUPR_X       LIVE_TV_GROUP_CONT_W - LIVE_TV_GROUPL_X - LIVE_TV_GROUPL_W
#define LIVE_TV_GROUPR_Y       LIVE_TV_GROUPL_Y
#define LIVE_TV_GROUPR_W      LIVE_TV_GROUPL_W
#define LIVE_TV_GROUPR_H       LIVE_TV_GROUPL_H

//liveTV group 
#define LIVE_TV_GROUP_X       LIVE_TV_GROUPL_X + LIVE_TV_GROUPL_W
#define LIVE_TV_GROUP_Y       0
#define LIVE_TV_GROUP_W      LIVE_TV_LIST_CONT_W - 2*LIVE_TV_GROUP_X - 110
#define LIVE_TV_GROUP_H       LIVE_TV_GROUP_CONT_H

//liveTV list
#define LIVE_TV_LIST_X  0//5
#define LIVE_TV_LIST_Y  LIVE_TV_GROUP_CONT_Y + LIVE_TV_GROUP_CONT_H 
#define LIVE_TV_LIST_W  LIVE_TV_GROUP_CONT_W - 6 -2
#define LIVE_TV_LIST_H  LIVE_TV_LIST_CONT_H - LIVE_TV_GROUP_CONT_H

//liveTV listbar
#define LIVE_TV_LIST_BAR_X  LIVE_TV_LIST_X + LIVE_TV_LIST_W// + LIVE_TV_LIST_MIDL
#define LIVE_TV_LIST_BAR_Y  LIVE_TV_LIST_Y + LIVE_TV_LIST_MIDT
#define LIVE_TV_LIST_BAR_W  6
#define LIVE_TV_LIST_BAR_H  LIVE_TV_LIST_H - 2*LIVE_TV_LIST_MIDT

#define LIVE_TV_LIST_MIDL  2////4
#define LIVE_TV_LIST_MIDT  4///4
#define LIVE_TV_LIST_MIDW  (LIVE_TV_LIST_W - 2 * LIVE_TV_LIST_MIDL)
#define LIVE_TV_LIST_MIDH  (LIVE_TV_LIST_H - 2 * LIVE_TV_LIST_MIDT)

//exit dlg
#define LIVE_TV_DLG_X   ((SCREEN_WIDTH -LIVE_TV_DLG_W)/2)
#define LIVE_TV_DLG_Y   ((SCREEN_HEIGHT-LIVE_TV_DLG_H)/2)
#define LIVE_TV_DLG_W   300
#define LIVE_TV_DLG_H    200

//number play
#define LIVE_TV_NUM_PLAY_FRM_X       SCREEN_WIDTH - LIVE_TV_LIST_CONT_W - 30
#define LIVE_TV_NUM_PLAY_FRM_Y       30
#define LIVE_TV_NUM_PLAY_FRM_W       150//191
#define LIVE_TV_NUM_PLAY_FRM_H       60

#define LIVE_TV_NUM_PLAY_TXT_X       0
#define LIVE_TV_NUM_PLAY_TXT_Y       0
#define LIVE_TV_NUM_PLAY_TXT_W       (LIVE_TV_NUM_PLAY_FRM_W-2*LIVE_TV_NUM_PLAY_TXT_X)
#define LIVE_TV_NUM_PLAY_TXT_H       (LIVE_TV_NUM_PLAY_FRM_H-2*LIVE_TV_NUM_PLAY_TXT_Y)

//---------------------------------------------------------------

//liveTV EPG info controls
#define LIVE_TV_EPG_FRM_X      ((SCREEN_WIDTH - LIVE_TV_EPG_FRM_W) / 2) 
#define LIVE_TV_EPG_FRM_Y       500
#define LIVE_TV_EPG_FRM_W      1120
#define LIVE_TV_EPG_FRM_H       130

//first line current P
#define LIVE_TV_EPG_CURR_P_X      30 
#define LIVE_TV_EPG_CURR_P_Y       0
#define LIVE_TV_EPG_CURR_P_W      120 
#define LIVE_TV_EPG_CURR_P_H       40//50

//live tv help bar
#define LIVE_TV_HELP_FRM_X       LIVE_TV_LIST_CONT_X
#define LIVE_TV_HELP_FRM_Y       LIVE_TV_LIST_CONT_Y + LIVE_TV_LIST_CONT_H
#define LIVE_TV_HELP_FRM_W       LIVE_TV_LIST_CONT_W
#define LIVE_TV_HELP_FRM_H       70

//livetv show bps
#define LIVE_TV_BPS_X			((SCREEN_WIDTH - LIVE_TV_BPS_W) / 2) 
#define LIVE_TV_BPS_Y			200
#define LIVE_TV_BPS_W			520
#define LIVE_TV_BPS_H		60

//others
#define LIVE_TV_LCONT_LIST_VGAP 4
#define LIVE_TV_NUM_PLAY_CNT        4
/* rect style */

/* font style */
/* others */
#define LIVE_TV_LIST_ITEM_NUM_ONE_PAGE  10
#define LIVE_TV_LIST_FIELD 3//3
#define LIVETV_LOAD_MEDIA_MAX_TIME 12
#endif


