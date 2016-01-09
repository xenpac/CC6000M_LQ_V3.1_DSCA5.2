/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/
/*!
 Include files
 */
#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_online_movie.h"
//#include "ui_onmov_provider.h"
#include "ui_onmov_api.h"
#include "netplay.h"
#include "ui_video_player_gprot.h"
#include "commonData.h"
#include "youtubeDataProvider.h"
#include "ui_youporn_api.h"

/*!
 Const value
 */
//left  container
#define ONMOV_LEFT_CONTX   120
#define ONMOV_LEFT_CONTY   100
#define ONMOV_LEFT_CONTW  170
#define ONMOV_LEFT_CONTH   570

//bmp
#define ONMOV_BMPX   0
#define ONMOV_BMPY   0
#define ONMOV_BMPW  ONMOV_LEFT_CONTW
#define ONMOV_BMPH   130

//director  container
#define ONMOV_DIRECTORX   20
#define ONMOV_DIRECTORY   130
#define ONMOV_DIRECTORW  130
#define ONMOV_DIRECTORH   50

//director text
#define ONMOV_DIRECTOR_TEXTX   10
#define ONMOV_DIRECTOR_TEXTY   ONMOV_DIRECTOR_TEXTX + ONMOV_DIRECTORH
#define ONMOV_DIRECTOR_TEXTW  ONMOV_DIRECTORW + 2*ONMOV_DIRECTOR_TEXTX
#define ONMOV_DIRECTOR_TEXTH   100

//actor  
#define ONMOV_ACTOR_TEXT_H   240

//right title
#define ONMOV_RIGHT_TITLEX 300
#define ONMOV_RIGHT_TITLEY 100
#define ONMOV_RIGHT_TITLEW 860
#define ONMOV_RIGHT_TITLEH 40

//picture
#define ONMOV_PICTUREX 300
#define ONMOV_PICTUREY 150
#define ONMOV_PICTUREW 310
#define ONMOV_PICTUREH 460

//desc cont
#define ONMOV_DESC_CNTX 620
#define ONMOV_DESC_CNTY 150
#define ONMOV_DESC_CNTW 540
#define ONMOV_DESC_CNTH 460

//bottom help container
#define ONMOV_BOTTOM_HELP_X  300
#define ONMOV_BOTTOM_HELP_Y  620
#define ONMOV_BOTTOM_HELP_W  860
#define ONMOV_BOTTOM_HELP_H  50

//description text title
#define ONMOV_TEXT_TITLE_X  50
#define ONMOV_TEXT_TITLE_Y  20
#define ONMOV_TEXT_TITLE_W  ONMOV_DESC_CNTW - 2*ONMOV_TEXT_TITLE_X
#define ONMOV_TEXT_TITLE_H  40

//description text
#define ONMOV_TEXT_X  ONMOV_TEXT_TITLE_X
#define ONMOV_TEXT_Y  ONMOV_TEXT_TITLE_Y + ONMOV_TEXT_TITLE_H
#define ONMOV_TEXT_W  ONMOV_TEXT_TITLE_W
#define ONMOV_TEXT_H  240

//play item
#define ONMOV_PLAY_ITEM_X  (ONMOV_DESC_CNTW - ONMOV_PLAY_ITEM_W)/2
#define ONMOV_PLAY_ITEM_Y  320
#define ONMOV_PLAY_ITEM_W  160
#define ONMOV_PLAY_ITEM_H  80

//SD,HD,UHD item
#define  SD_PLAY_ITEM_X  45
#define  SD_PLAY_ITEM_Y  400
#define  SD_PLAY_ITEM_W 90
#define  SD_PLAY_ITEM_H  40

#define  HD_PLAY_ITEM_X  225
#define  HD_PLAY_ITEM_Y  400
#define  HD_PLAY_ITEM_W 90
#define  HD_PLAY_ITEM_H  40

#define  UHD_PLAY_ITEM_X  405
#define  UHD_PLAY_ITEM_Y  400
#define  UHD_PLAY_ITEM_W 90
#define  UHD_PLAY_ITEM_H  40

//others
#define LEFT_TEXT_ITEM 4




typedef enum
{
    IDC_ONMOV_LEFT_CONT = 1,
    IDC_ONMOV_BMP,
    IDC_DIRECTOR,
    IDC_DIRECTOR_FIELD,
    IDC_ACTOR,
    IDC_ACTOR_FIELD,
    
    IDC_ONMOV_TITLE,
    IDC_ONMOV_PICTURE,
    IDC_ONMOV_DESCNT,
    IDC_ONMOV_DESCNT_TITLE,
    IDC_ONMOV_DESCNT_TXT,
    IDC_ONMOV_PLAY,
    IDC_ONMOV_PLAY_SD,
    IDC_ONMOV_PLAY_HD,
    IDC_ONMOV_PLAY_UHD,
    IDC_ONMOV_BTM_HELP,
}ui_onmov_ctrl_id_t;

static comm_help_data_t movie_help_data =
{
    2,2,
    {IDS_PLAY, IDS_EXIT},
    {IM_HELP_OK, IM_HELP_EXIT}
};

static comm_help_data_t movie_help_data_2 =
{
    3,3,
    {IDS_SELECT, IDS_PLAY, IDS_EXIT},
    {IM_CHANGE,IM_HELP_OK, IM_HELP_EXIT}
};

static netplay_media_info_t* s_pCurMoive = NULL;
static youx_item* sg_yp_item = NULL;
static u16 desc_uni_str[1024] = {0};

static BOOL sg_youporn_flag = FALSE;
static char* sg_play_url = NULL;
enum
{
  E_PLAY_SD = 0,
  E_PLAY_HD,
  E_PLAY_UHD,
  E_PLAY_MODE_CNT,
};
static s8 sg_play_mode_focus = 0;

/*!
 * Function define
 */
u16 online_mov_cont_keymap(u16 key);
RET_CODE online_mov_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 omd_playbtn_keymap(u16 key);
RET_CODE omd_playbtn_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
//extern int utf8_to_utf16(const char *ins, tchar_t *outs, u32 max_len);
extern iconv_t g_cd_utf8_to_utf16le;


static void omd_pic_init(void) 
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void omd_pic_deinit(void)
{
  pic_stop();

  ui_pic_release();
}


static void omd_pic_play_start(void)
{
  control_t *p_menu = NULL;
  control_t *p_picture = NULL;
  rect_t rect;
    
  p_menu = ui_comm_root_get_root(ROOT_ID_ONMOV_DESCRIPTION);
  p_picture = ctrl_get_child_by_id(p_menu, IDC_ONMOV_PICTURE);

  ctrl_get_frame(p_picture, &rect);
  ctrl_client2screen(p_picture, &rect);
  if( TRUE == sg_youporn_flag)
  {
    if( sg_yp_item->image )
    {
      ui_pic_play_by_url(sg_yp_item->image, &rect, 0);
 //     bmap_set_content_by_id(p_picture, RSC_INVALID_ID);
//      ctrl_paint_ctrl(p_picture, TRUE);
    }
  }
  else if ( s_pCurMoive->p_logo_url )
  {
    ui_pic_play_by_url(s_pCurMoive->p_logo_url, &rect, 0);
    
//    bmap_set_content_by_id(p_picture, RSC_INVALID_ID);
//    ctrl_paint_ctrl(p_picture, TRUE);
    //bmap_set_content_by_id(p_picture, IM_INDEX_NETWORK_MOVIE_PIC);
    //ctrl_paint_ctrl(p_picture, TRUE);
  }
}


static RET_CODE omd_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t* p_new_focus = NULL;
  if( TRUE == sg_youporn_flag )
    return SUCCESS;

  if( MSG_FOCUS_LEFT == msg )
    sg_play_mode_focus--;
  else if( MSG_FOCUS_RIGHT== msg )
    sg_play_mode_focus++;

  sg_play_mode_focus = (sg_play_mode_focus+3)%3;

  ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);

  p_new_focus = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl),
                              IDC_ONMOV_PLAY_SD + sg_play_mode_focus);
  ctrl_process_msg(p_new_focus, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_new_focus, TRUE);

  return SUCCESS;
}

static BOOL sg_play_btn_pressed = FALSE;
static RET_CODE omd_on_playbtn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  static ui_video_player_param vdoPlayerParam;
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u16 string_char[32 + 1];
  
  comm_dlg_data_t dlg_data =
    {
      0,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X + 250,
      COMM_DLG_Y,
      COMM_DLG_W,
      COMM_DLG_H,
      IDS_MSG_GET_URL,
      0,
    };  

  if( TRUE == sg_youporn_flag && NULL == sg_play_url )
  {
    sg_play_btn_pressed = TRUE;
    ui_comm_dlg_open(&dlg_data);
    return SUCCESS;
  }
  
  omd_pic_deinit();

  memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));
  if( TRUE == sg_youporn_flag )
  {
    src_len = strlen(sg_yp_item->title) + 1;
    dest_len = sizeof(string_char);
    inbuf = (char *)sg_yp_item->title;
    outbuf = (char *)string_char;
    iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
    vdoPlayerParam.name = string_char;

    vdoPlayerParam.url_cnt = 1;
    vdoPlayerParam.pp_urlList = (u8 **)mtos_malloc(sizeof(u8 *));
    MT_ASSERT(vdoPlayerParam.pp_urlList != NULL);
    vdoPlayerParam.pp_urlList[0] = sg_play_url;
  }
  else
  {
    u8 focus_id = ctrl_get_ctrl_id(p_ctrl);

    if( NULL == s_pCurMoive->p_url )
      return SUCCESS; //null url check for bugs: 32205,32556

    if( IDC_ONMOV_PLAY_SD == focus_id )
    {
      OS_PRINTF("PLAY SD\n");
#ifndef WIN32	  
      netplay_set_video_resolution(NETPLAY_VIDEO_RESOLUTION_NORMAL);
#endif
    }
    else if( IDC_ONMOV_PLAY_HD == focus_id )
    {
      OS_PRINTF("PLAY HD\n");
#ifndef WIN32	  
      netplay_set_video_resolution(NETPLAY_VIDEO_RESOLUTION_HIGH);
#endif
    }
    else if( IDC_ONMOV_PLAY_UHD == focus_id )
    {
      OS_PRINTF("PLAY UHD\n");
#ifndef WIN32	  
      netplay_set_video_resolution(NETPLAY_VIDEO_RESOLUTION_SUPER);
#endif
    }

    src_len = strlen(s_pCurMoive->name) + 1;
    dest_len = sizeof(string_char);
    inbuf = (char *)s_pCurMoive->name;
    outbuf = (char *)string_char;
    iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
    vdoPlayerParam.name = string_char;

    vdoPlayerParam.url_cnt = 1;
    vdoPlayerParam.pp_urlList = (u8 **)mtos_malloc(sizeof(u8 *));
    MT_ASSERT(vdoPlayerParam.pp_urlList != NULL);
    vdoPlayerParam.pp_urlList[0] = s_pCurMoive->p_url;
  }

  manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_ONMOV_DESCRIPTION, (u32)&vdoPlayerParam);

  if (vdoPlayerParam.pp_urlList != NULL)
  {
    mtos_free(vdoPlayerParam.pp_urlList);
  }
  return SUCCESS;
}

/*!
 * Video view entry
 */
RET_CODE ui_open_online_movie_description(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_left_cont = NULL;
    control_t *p_bmp = NULL;
    control_t *p_title = NULL;
    control_t *p_picture = NULL;
    control_t *p_des_cont = NULL;
    control_t *p_bottom_help = NULL;
    control_t *p_ctrl[LEFT_TEXT_ITEM] = {NULL};
    control_t *p_sub = NULL;
    control_t* p_sd = NULL;
    control_t* p_hd = NULL;
    control_t* p_uhd = NULL;

    u16 i = 0;
    u16 left_str[3] =
    { 
      IDS_DIRECTOR,
      0,
      IDS_ACTOR,
    };
    u16 uni_str[64] = {0};

    omd_pic_init();

    sg_youporn_flag = (BOOL)para2;
    if( TRUE == sg_youporn_flag )
      sg_yp_item = (youx_item*)para1;
    else
      s_pCurMoive = (netplay_media_info_t *)para1;


    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create_netmenu(ROOT_ID_ONMOV_DESCRIPTION, 0,
                                IM_INDEX_MEDIAPLAYERDETAIL_BANNER, 
                                TRUE == sg_youporn_flag?IDS_YOUPORN:IDS_NETWORK_PLAY);
    MT_ASSERT(p_menu != NULL);
    ctrl_set_keymap(p_menu, online_mov_cont_keymap);
    ctrl_set_proc(p_menu, online_mov_cont_proc);

    /*!
     * Create help bar
     */
    p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_BTM_HELP,
                                ONMOV_BOTTOM_HELP_X, ONMOV_BOTTOM_HELP_Y, 
                                ONMOV_BOTTOM_HELP_W, ONMOV_BOTTOM_HELP_H, 
                                p_menu, 0);
    ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
    if( TRUE == sg_youporn_flag )
      ui_comm_help_create_ext(60, 0, ONMOV_BOTTOM_HELP_W-60, ONMOV_BOTTOM_HELP_H,  &movie_help_data,  p_bottom_help);
    else
      ui_comm_help_create_ext(60, 0, ONMOV_BOTTOM_HELP_W-60, ONMOV_BOTTOM_HELP_H,  &movie_help_data_2,  p_bottom_help);
		
    //left cont
    p_left_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_LEFT_CONT, 
                                  ONMOV_LEFT_CONTX, ONMOV_LEFT_CONTY, 
                                  ONMOV_LEFT_CONTW, ONMOV_LEFT_CONTH, 
                                  p_menu, 0);
    
    ctrl_set_rstyle(p_left_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    //bmp control
    p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_ONMOV_BMP, ONMOV_BMPX, ONMOV_BMPY, 
      ONMOV_BMPW, ONMOV_BMPH, p_left_cont, 0);
    ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_bmp, IM_ICON_NETWORK_MOVIE);
        OS_PRINTF(">>>>>>>>>>%s, %d\n", __FUNCTION__, __LINE__);
    for(i = 0;i < LEFT_TEXT_ITEM;i++)
    {
        OS_PRINTF(">>>>>>>>>>%s, %d, %d\n", __FUNCTION__, __LINE__, i);
        switch(i)
        {
            case 0:
            case 2:
                p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT , IDC_DIRECTOR+i,
                                    ONMOV_DIRECTORX, 
                                    ONMOV_DIRECTORY+i/2*(ONMOV_DIRECTORH + ONMOV_DIRECTOR_TEXTH), 
                                    ONMOV_DIRECTORW, ONMOV_DIRECTORH, 
                                    p_left_cont, 0);
          ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
          text_set_font_style(p_ctrl[i], FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
          text_set_content_by_strid(p_ctrl[i],left_str[i]);
          break;
        case 1:
        case 3:
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT , IDC_DIRECTOR + i,
                                    ONMOV_DIRECTOR_TEXTX, ONMOV_DIRECTOR_TEXTY 
                                    + (i-1)/2*(ONMOV_DIRECTOR_TEXTH + ONMOV_DIRECTORH), 
                                    ONMOV_DIRECTORW, ONMOV_ACTOR_TEXT_H, 
                                    p_left_cont, 0);
          ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
          text_set_align_type(p_ctrl[i], STL_CENTER| STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
                do
                {
                  if (i == 1)
                  {
                    if( TRUE == sg_youporn_flag )
                    {// no director info
                      text_set_content_by_extstr(p_ctrl[i], (u16*)"");
                    }
                    else
                    {
                      if(s_pCurMoive->p_director)
                    	  {
                    	    char* inbuf = s_pCurMoive->p_director;
                         char* outbuf = (char*)uni_str;
                         int src_len = strlen(inbuf) + 1;
                         int dest_len = sizeof(uni_str);
			
                    	    iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
                         text_set_content_by_unistr(p_ctrl[i], uni_str);
                    	  }
                       else
                       {
                         text_set_content_by_unistr(p_ctrl[i], (u16*)"");
                       }
                    }
                  }
                  else if (i == 3)
                  {
                    if( TRUE == sg_youporn_flag )
                    {// no actor info
                      text_set_content_by_extstr(p_ctrl[i], (u16*)"");
                    }
                    else
                    {
                      if(s_pCurMoive->p_actor)
                    	 {
                    	   char* inbuf = s_pCurMoive->p_actor;
                        char* outbuf = (char*)uni_str;
                        int src_len = strlen(inbuf) + 1;
                        int dest_len = sizeof(uni_str);
			
                    	   iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
                        text_set_content_by_unistr(p_ctrl[i], uni_str);
                    	 }
                      else
                      {
                        text_set_content_by_unistr(p_ctrl[i], (u16*)"");
                      }
                    }
                  }
                }while(0);
                break;
            //text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
        }
    }

    // title 
    p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_TITLE, 
                            ONMOV_RIGHT_TITLEX, ONMOV_RIGHT_TITLEY, 
                            ONMOV_RIGHT_TITLEW, ONMOV_RIGHT_TITLEH, 
                            p_menu, 0);
    ctrl_set_rstyle(p_title, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
    text_set_align_type(p_title, STL_CENTER| STL_VCENTER);
    text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    OS_PRINTF(">>>>>>>>>>%s, %d\n", __FUNCTION__, __LINE__);
    if( TRUE == sg_youporn_flag )
    {
      if(sg_yp_item->title)
      {
        char* inbuf = sg_yp_item->title;
        char* outbuf = (char*)uni_str;
        int src_len = strlen(inbuf) + 1;
        int dest_len = sizeof(uni_str);
        //utf8_to_utf16(sg_yp_item->title, uni_str, sizeof(uni_str));
        OS_PRINTF(">>>>>>>>>>%s, %d\n", __FUNCTION__, __LINE__);
        iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
        text_set_content_by_unistr(p_title, uni_str);
      }
      else
      {
        text_set_content_by_unistr(p_title, NULL);
      }
    }
    else
    {
      if(s_pCurMoive->name)
      {
        char* inbuf = s_pCurMoive->name;
        char* outbuf = (char*)uni_str;
        int src_len = strlen(inbuf) + 1;
        int dest_len = sizeof(uni_str);

        OS_PRINTF(">>>>>>>>>>%s, %d\n", __FUNCTION__, __LINE__);
        iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
        text_set_content_by_unistr(p_title, uni_str);
      }
      else
      {
        text_set_content_by_unistr(p_title, NULL);
      }
    }
    //picture 
    p_picture = ctrl_create_ctrl(CTRL_BMAP, IDC_ONMOV_PICTURE, 
                                ONMOV_PICTUREX, ONMOV_PICTUREY, 
                                ONMOV_PICTUREW, ONMOV_PICTUREH, 
                                p_menu, 0);
    ctrl_set_rstyle(p_picture, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    bmap_set_content_by_id(p_picture, IM_INDEX_NETWORK_MOVIE_PIC);
    
    //create description
    p_des_cont = ctrl_create_ctrl(CTRL_CONT, IDC_ONMOV_DESCNT, 
                                  ONMOV_DESC_CNTX, ONMOV_DESC_CNTY, 
                                  ONMOV_DESC_CNTW, ONMOV_DESC_CNTH, 
                                  p_menu, 0);
    ctrl_set_rstyle(p_des_cont, RSI_BLACK_FRM, RSI_BLACK_FRM, RSI_BLACK_FRM);
   
    //create description title
   p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_DESCNT_TITLE, 
                          ONMOV_TEXT_TITLE_X, ONMOV_TEXT_TITLE_Y, 
                          ONMOV_TEXT_TITLE_W, ONMOV_TEXT_TITLE_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_sub, IDS_DESCRIPTION);
    text_set_font_style(p_sub, FSI_WHITE, FSI_WHITE, FSI_WHITE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_DESCNT_TXT, 
                          ONMOV_TEXT_X, ONMOV_TEXT_Y, 
                          ONMOV_TEXT_W, ONMOV_TEXT_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_font_style(p_sub, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
    text_set_content_type(p_sub, TEXT_STRTYPE_EXTSTR);
    if( TRUE == sg_youporn_flag )
    {
      if(sg_yp_item->discription)
      {
        //utf8_to_utf16(sg_yp_item->discription, desc_uni_str, sizeof(desc_uni_str));
        text_set_content_by_extstr(p_sub, desc_uni_str);
      }
      else
      {
        text_set_content_by_extstr(p_sub, NULL);
      }
    }
    else
    {
      if(s_pCurMoive->p_description)
      {
        char* inbuf = s_pCurMoive->p_description;
        char* outbuf = (char*)desc_uni_str;
        int src_len = strlen(inbuf) + 1;
        int dest_len = sizeof(desc_uni_str);
			
        iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
        text_set_content_by_extstr(p_sub, desc_uni_str);
      }
      else
      {
        text_set_content_by_extstr(p_sub, NULL);
      }
    }
    //create play button

   //------------SD, HD, UHD-----------------
   if( TRUE == sg_youporn_flag )
   {// only play button
     p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_PLAY, 
                          ONMOV_PLAY_ITEM_X, ONMOV_PLAY_ITEM_Y, 
                          ONMOV_PLAY_ITEM_W, ONMOV_PLAY_ITEM_H, 
                          p_des_cont, 0);
     #ifdef MODEST
     ctrl_set_rstyle(p_sub, RSI_ITEM_2_SH, RSI_ITEM_2_HL, RSI_ITEM_2_SH);
     #endif
     text_set_align_type(p_sub, STL_CENTER | STL_VCENTER);
     text_set_font_style(p_sub, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
     text_set_content_type(p_sub, TEXT_STRTYPE_STRID);
     text_set_content_by_strid(p_sub, IDS_PLAY);
     ctrl_set_keymap(p_sub, omd_playbtn_keymap);
     ctrl_set_proc(p_sub, omd_playbtn_proc);
     ctrl_default_proc(p_sub, MSG_GETFOCUS, 0, 0);
   }
   else
   {// SD, HD, UHD button
     char* str_sd = "SD";
     char* str_hd = "HD";
     char* str_uhd = "UHD";
     u16 uni_str[16];
     int src_len = 0;
     char* outbuf = NULL;
     int dest_len = 0;
//     char** p_inbuf = NULL;
     //char** p_outbuf = &outbuf;
OS_PRINTF(">>>>>>>>>>>> %s, %d\n", __FUNCTION__, __LINE__);
     //-----SD--------
     p_sd = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_PLAY_SD, 
                          SD_PLAY_ITEM_X, SD_PLAY_ITEM_Y, 
                          SD_PLAY_ITEM_W, SD_PLAY_ITEM_H, 
                          p_des_cont, 0);
     ctrl_set_rstyle(p_sd, RSI_PLAY_BUTTON_SH, RSI_PLAY_BUTTON_HL, RSI_PLAY_BUTTON_SH);
     text_set_align_type(p_sd, STL_CENTER | STL_VCENTER);
     text_set_font_style(p_sd, FSI_WHITE_20, FSI_BLACK_20, FSI_WHITE_20);
     text_set_content_type(p_sd, TEXT_STRTYPE_UNICODE);

     src_len = strlen(str_sd) + 1;
     outbuf = (char*)uni_str;
     dest_len = sizeof(uni_str);
     iconv(g_cd_utf8_to_utf16le, &str_sd, &src_len, &outbuf, &dest_len);
     text_set_content_by_unistr(p_sd, uni_str);
     ctrl_set_keymap(p_sd, omd_playbtn_keymap);
     ctrl_set_proc(p_sd, omd_playbtn_proc);
     ctrl_default_proc(p_sd, MSG_GETFOCUS, 0, 0);

     //--------HD---------
     p_hd = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_PLAY_HD, 
                          HD_PLAY_ITEM_X, HD_PLAY_ITEM_Y, 
                          HD_PLAY_ITEM_W, HD_PLAY_ITEM_H, 
                          p_des_cont, 0);
     ctrl_set_rstyle(p_hd, RSI_PLAY_BUTTON_SH, RSI_PLAY_BUTTON_HL, RSI_PLAY_BUTTON_SH);
     text_set_align_type(p_hd, STL_CENTER | STL_VCENTER);
     text_set_font_style(p_hd, FSI_WHITE_20, FSI_BLACK_20, FSI_WHITE_20);
     text_set_content_type(p_hd, TEXT_STRTYPE_UNICODE);
     src_len = strlen(str_hd) + 1;
     outbuf = (char*)uni_str;
     dest_len = sizeof(uni_str);
     iconv(g_cd_utf8_to_utf16le, &str_hd, &src_len, &outbuf, &dest_len);
     text_set_content_by_unistr(p_hd, uni_str);
     ctrl_set_keymap(p_hd, omd_playbtn_keymap);
     ctrl_set_proc(p_hd, omd_playbtn_proc);

     //----------UHD------------     
     p_uhd = ctrl_create_ctrl(CTRL_TEXT, IDC_ONMOV_PLAY_UHD, 
                          UHD_PLAY_ITEM_X, UHD_PLAY_ITEM_Y, 
                          UHD_PLAY_ITEM_W, UHD_PLAY_ITEM_H, 
                          p_des_cont, 0);
     ctrl_set_rstyle(p_uhd, RSI_PLAY_BUTTON_SH, RSI_PLAY_BUTTON_HL, RSI_PLAY_BUTTON_SH);
     text_set_align_type(p_uhd, STL_CENTER | STL_VCENTER);
     text_set_font_style(p_uhd, FSI_WHITE_20, FSI_BLACK_20, FSI_WHITE_20);
     text_set_content_type(p_uhd, TEXT_STRTYPE_UNICODE);
     src_len = strlen(str_uhd) + 1;
     outbuf = (char*)uni_str;
     dest_len = sizeof(uni_str);
     iconv(g_cd_utf8_to_utf16le, &str_uhd, &src_len, &outbuf, &dest_len);
     text_set_content_by_unistr(p_uhd, uni_str);
     ctrl_set_keymap(p_uhd, omd_playbtn_keymap);
     ctrl_set_proc(p_uhd, omd_playbtn_proc);
   }
   //---------------------------------------------------


   
    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

    if( TRUE == sg_youporn_flag )
      omd_pic_play_start();
    else
      omd_pic_play_start();
    return SUCCESS;
}

static RET_CODE omd_on_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  // TODO release op

  return ERR_NOFEATURE;
}

static RET_CODE omd_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root = NULL;
  control_t *p_root_yp = NULL;

  ui_comm_dlg_close(); // fix bug 32673

  sg_play_btn_pressed = FALSE;
  sg_youporn_flag = FALSE;
  sg_play_url = NULL;
  sg_play_mode_focus = 0;  
  omd_pic_deinit();

  p_root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAY);
  p_root_yp = fw_find_root_by_id(ROOT_ID_YOUPORN);
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  else if( p_root_yp != NULL )
  {
    fw_notify_root(p_root_yp, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  return ERR_NOFEATURE;
}

static RET_CODE omd_on_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t* p_pic = ctrl_get_child_by_id(p_cont, IDC_ONMOV_PICTURE);

  bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
  ctrl_paint_ctrl(p_pic, TRUE);

  omd_pic_init();

  if( TRUE == sg_youporn_flag )
    omd_pic_play_start();
  else
    omd_pic_play_start();

  return SUCCESS;
}

static RET_CODE onm_on_play_url(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  char *inbuf, *outbuf;
  size_t src_len, dest_len;
  u16 string_char[32 + 1];

  sg_play_url = (char*)para1;
  OS_PRINTF("%s, url: %s\n", __FUNCTION__, sg_play_url);
  if( TRUE == sg_play_btn_pressed )
  {
    static ui_video_player_param vdoPlayerParam;

    ui_comm_dlg_close();
    omd_pic_deinit();

    sg_play_btn_pressed = FALSE;

    memset(&vdoPlayerParam, 0, sizeof(ui_video_player_param));

    src_len = strlen(sg_yp_item->title) + 1;
    dest_len = sizeof(string_char);
    inbuf = (char *)sg_yp_item->title;
    outbuf = (char *)string_char;
    iconv(g_cd_utf8_to_utf16le, &inbuf, &src_len, &outbuf, &dest_len);
    vdoPlayerParam.name = string_char;

    vdoPlayerParam.url_cnt = 1;
    vdoPlayerParam.pp_urlList = (u8 **)mtos_malloc(sizeof(u8 *));
    MT_ASSERT(vdoPlayerParam.pp_urlList != NULL);
    vdoPlayerParam.pp_urlList[0] = sg_play_url;

    manage_open_menu(ROOT_ID_VIDEO_PLAYER, ROOT_ID_ONMOV_DESCRIPTION, (u32)&vdoPlayerParam);

    if (vdoPlayerParam.pp_urlList != NULL)
    {
      mtos_free(vdoPlayerParam.pp_urlList);
    }
  }
  return SUCCESS;
}

static RET_CODE omd_on_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  static u8 err_cnt;
  control_t* p_pic = ctrl_get_child_by_id(p_cont, IDC_ONMOV_PICTURE);
  if( MSG_PIC_EVT_DATA_ERROR == msg )
  {
    err_cnt++;
    if( err_cnt < MAX_RETRY_CNT)
    {
      OS_PRINTF("%s(), RETRY!!\n", __FUNCTION__);
      pic_stop();
      omd_pic_play_start();
    }
    else
    {
      err_cnt = 0;
      omd_pic_deinit();
    }
  }
  else if( MSG_PIC_EVT_DRAW_END == msg )
  {
    err_cnt = 0;
    bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_pic, TRUE);
  }
  return SUCCESS;
}

BEGIN_KEYMAP(online_mov_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(online_mov_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(online_mov_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, omd_on_exit)
  ON_COMMAND(MSG_DESTROY, omd_on_destory)
  ON_COMMAND(MSG_UPDATE, omd_on_update)
  ON_COMMAND(MSG_YP_PLAY_URL, onm_on_play_url)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, omd_on_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, omd_on_pic_draw_end)
END_MSGPROC(online_mov_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(omd_playbtn_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(omd_playbtn_keymap, NULL)

BEGIN_MSGPROC(omd_playbtn_proc, text_class_proc)
    ON_COMMAND(MSG_SELECT, omd_on_playbtn_select)
    ON_COMMAND(MSG_FOCUS_LEFT, omd_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, omd_change_focus)
END_MSGPROC(omd_playbtn_proc, text_class_proc)

#endif

