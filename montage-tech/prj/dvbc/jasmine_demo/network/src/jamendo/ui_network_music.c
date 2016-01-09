/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_network_music.h"
#include "ui_net_music_metadata_api.h"
#include "ui_net_music_play_ctrl_api.h"
#include "ui_keyboard_v2.h"
#include "ui_network_music_search.h"
#include "ui_mute.h"
#include "ui_volume_usb.h"
#include "iconv_ext.h"
#include "ui_picture_api.h"
#include "music_api_mt.h"
#include "music_api_mplayer.h"


extern iconv_t g_cd_utf16le_to_utf8;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef enum
{
    MSG_SWITCH_MODE = MSG_LOCAL_BEGIN + 900,
    MSG_NM_SEARCH,
    MSG_VOLUME_UP,
    MSG_VOLUME_DOWN,
    MSG_BLUE,
    MSG_RETURN,
    MSG_MUSIC_PLAY,
    MSG_MUSIC_RESUME,
    MSG_MUSIC_PAUSE,
    MSG_MUSIC_NEXT,
    MSG_MUSIC_PRE,
    MSG_MUSIC_STOP,
    MSG_MUSIC_INPUT_COMP,
}nm_local_msg_t;

typedef enum
{
    IDC_NM_MAIN_INFO_CONT = 1,
    IDC_NM_POP_MAIN_LIST,
    IDC_NM_TRACK_LIST_CONT,
    IDC_NM_ALBUM_SINGER_CONT,
    IDC_NM_HELP_BAR,
    IDC_NM_MODE_CONT,
    IDC_NM_WEB_SITE,
    IDC_NM_MODE_NAME,
    IDC_NM_POP_TOTAL,
    IDC_NM_POP_MONTH,
    IDC_NM_POP_WEEK,
    IDC_NM_COVER_PIC_CONT,
    IDC_NM_COVER_DEFAULT_BMP,
    IDC_NM_PAGE_CONT,
    IDC_NM_LIST_PAGE_NUM,
    IDC_NM_TRACKLIST,
    IDC_NM_CELL_CONT_START,
    IDC_NM_CELL_PIC,
    IDC_NM_CELL_NAME,
    IDC_NM_PLAY_DETAIL_CONT,
    IDC_NM_PLAY_PLAY_ICON,
    IDC_NM_PLAY_REPEAT_ICON,
    IDC_NM_PLAY_PROGRESS,
    IDC_NM_PLAY_CURTM,
    IDC_NM_PLAY_TOLTM,
    IDC_NM_PLAY_TRACK_NAME,
}network_music_ctrl_id_t;

typedef struct
{
    u16 name;
} nm_pop_mainlist_item;

static nm_pop_mainlist_item pop_mainlist[3] =
{
    {
        IDS_POPULAR_TOTAL
    },
    {
        IDS_POPULAR_MONTH
    },
    {
        IDS_POPULAR_WEEK
    },

};

static u16 nm_mode_name[3] ={IDS_MODE_TRACK,IDS_MODE_ALBUM,IDS_MODE_ARTIST};


static list_xstyle_t pop_mainlist_item_rstyle =
{
    RSI_ITEM_8_SH,
    RSI_PBACK,
    RSI_ITEM_2_HL,
    RSI_ITEM_2_SH,
    RSI_ITEM_2_HL,
};

static list_xstyle_t pop_mainlist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t pop_mainlist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
};

static const list_field_attr_t popmainlist_attr[] =
{
    {
        LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
            160,//162,
            0,
            0,
            &pop_mainlist_field_rstyle,
            &pop_mainlist_field_fstyle
    }
};




static list_xstyle_t tracklist_item_rstyle =
{
    RSI_PBACK,
    RSI_MAIN_BG,//RSI_ITEM_2_SH,
    RSI_ITEM_13_HL,//RSI_ITEM_2_HL,
    RSI_MAIN_BG,//RSI_ITEM_2_SH,
    RSI_ITEM_13_HL//RSI_ITEM_2_HL,
};

static list_xstyle_t tracklist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};

static list_xstyle_t tracklist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
};

typedef enum
{
    TRACKLIST_FIELD_ID_NAME = 0,
    TRACKLIST_FIELD_ID_DURATION,
    TRACKLIST_FIELD_ID_ALBUM,
    TRACKLIST_FIELD_ID_ARTIST,
}ui_nm_track_list_filed_id_t;

static const list_field_attr_ext_t tracklist_attr[] =
{

    //TRACKLIST_FIELD_ID_NAME
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_TOP,
    300, 35, 5, 5, &tracklist_field_rstyle, &tracklist_field_fstyle},
    //TRACKLIST_FIELD_ID_DURATION
    {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
    100, 35, NM_TRACK_LIST_W - 110, 5, &tracklist_field_rstyle, &tracklist_field_fstyle},
    //TRACKLIST_FIELD_ID_ALBUM
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_TOP,
    300, 35,100, 35, &tracklist_field_rstyle, &tracklist_field_fstyle},
    //TRACKLIST_FIELD_ID_ARTIST
     {LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_TOP,
    300, 35, NM_TRACK_LIST_W - 400, 35, &tracklist_field_rstyle, &tracklist_field_fstyle}
};

typedef enum
{
    NM_MODE_UNKNOW = 0,
    NM_MODE_TRACK,
    NM_MODE_ALBUM ,
    NM_MODE_ARTIST,
    NM_MODE_MAX    
}nm_list_mode;

typedef enum
{
    NM_POPUL_UNKNOW = 0,
    NM_POPU_TOTAL,
    NM_POPU_MONTH,
    NM_POPU_WEEK,
}nm_list_popu_kind;

u32 request_popu_order[3][3] = {{TRACKS_POPULARITY_TOTAL,TRACKS_POPULARITY_MONTH,TRACKS_POPULARITY_WEEK},
                               {ALBUMS_POPULARITY_TOTAL,ALBUMS_POPULARITY_MONTH,ALBUMS_POPULARITY_WEEK},
                               {ARTISTS_POPULARITY_TOTAL,ARTISTS_POPULARITY_MONTH,ARTISTS_POPULARITY_WEEK}};

BOOL g_nm_is_playing = FALSE;
s16 g_mainpoplist_index = -1;
nm_list_mode g_cur_mode = NM_MODE_TRACK;
nm_list_mode g_last_mode = NM_MODE_TRACK;
nm_list_mode g_track_list_from_mode = NM_MODE_TRACK;

static u8 curWebsitesIndex = 0;
static s16 g_cur_album_index_in_total = 0;// index in total
static s16 g_curAlbumlistIndex = 0; //index in one page

static s16 g_cur_draw_index = 0; //for draw cover pic in list
static s16 g_need_draw_count = 0;//不满一页时可能小于10个
static s16 g_draw_end_flag[NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE] = {0};


static s16 g_album_cur_page = 0;
static s16 g_album_total_page = 0;
static s16 g_track_cur_page = 0;
static s16 g_track_total_page = 0;
static s16 g_track_total_num = 0;

static BOOL g_album_artist_data_ready = FALSE;
static BOOL g_track_data_ready = FALSE;

static utc_time_t cur_play_time = {0};
static utc_time_t total_play_time = {0};
static network_music_play_state_t g_cur_play_state = NETWORK_MUSIC_STAT_INVALID;
static network_music_play_mode_t g_cur_play_mode = PLAY_MODE_CYCLE_CUR_LIST;
s8 g_track_url[256] = {0};


//search
typedef enum
{
    NM_SEARCH_TRACK = 0,
    NM_SEARCH_ALBUM,
    NM_SEARCH_ARTIST,
}net_music_search_type;



#define NM_MAX_SEARCH_NAME_LEN     (16)
static u16 g_searchKey[NM_MAX_SEARCH_NAME_LEN+1] = {0};
static u32 g_cur_search_type = NM_SEARCH_TRACK;
//lint -esym(551,g_cur_search_type)

//menu cont
u16 network_music_cont_keymap(u16 key);
RET_CODE network_music_cont_proc(control_t *p_cont, u16 msg,
                                 u32 para1, u32 para2);
//pop main list(left side)
u16 network_music_pop_mainlist_keymap(u16 key);
RET_CODE network_music_pop_mainlist_proc(control_t *p_cont, u16 msg,
                              u32 para1, u32 para2);

u16 network_music_album_artist_keymap(u16 key);
RET_CODE network_music_album_artist_proc(control_t *p_cont, u16 msg,
                                         u32 para1, u32 para2);

u16 network_music_cell_cont_keymap(u16 key);
RET_CODE network_music_cell_cont_proc(control_t *p_cont, u16 msg,
                                      u32 para1, u32 para2);


//track list (right side)
u16 network_music_tracklist_keymap(u16 key);
RET_CODE network_music_tracklist_proc(control_t *p_cont, u16 msg,
                           u32 para1, u32 para2);

static void netmusic_open_dlg(u16 str_id, u32 time_out)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    //COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    NM_PAGE_CONTX + (NM_PAGE_CONTW - COMM_DLG_W)/2,NM_PAGE_CONTX + (NM_PAGE_CONTH - COMM_DLG_H)/2,COMM_DLG_W, COMM_DLG_H,
  };
  dlg_data.content = str_id;
  dlg_data.dlg_tmout = time_out;
  ui_comm_dlg_open(&dlg_data);
}

static void ui_network_music_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void ui_network_music_pic_deinit(void)
{
  pic_stop();
  ui_pic_release();
}

static RET_CODE ui_draw_cover_for_item_on_focus(control_t * p_pic_cont,u8 *pic_url)
{
    rect_t rect;

    ctrl_get_frame(p_pic_cont, &rect);
    ctrl_client2screen(p_pic_cont, &rect);
    if( strlen((char*)pic_url) > 0)
    {
        u32 identity = 0;
        g_cur_draw_index = 1;
        identity = (((u32)curWebsitesIndex)<<24)
            | ((g_cur_mode&0XFF)<<16)
            | (u16)g_cur_draw_index;

        ui_pic_play_by_url(pic_url, &rect, identity);
    }


    return SUCCESS;

}

static RET_CODE ui_draw_cover_pic_in_list(control_t *p_list_cont,u16 cur_mode,u8 cell_cont_index)
{
    control_t *p_item = NULL;
    control_t *p_pic = NULL;
    rect_t rect;
    char pic_url[512] = {0};

    if (cur_mode == NM_MODE_ALBUM)
    {
        net_music_albums_content_t *p_content = ui_net_music_get_album_list();
        strcpy(pic_url,p_content->albums_content[g_cur_draw_index + g_album_cur_page * NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE].img_url);
    }
    else if (cur_mode == NM_MODE_ARTIST)
    {
        net_music_artists_content_t *p_content = ui_net_music_get_artist_list();
        strcpy(pic_url,p_content->artists_content[g_cur_draw_index + g_album_cur_page * NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE].img_url);
    }

    p_item = ctrl_get_child_by_id(p_list_cont, IDC_NM_CELL_CONT_START + cell_cont_index);
    p_pic = ctrl_get_child_by_id(p_item, IDC_NM_CELL_PIC);

    ctrl_get_frame(p_pic, &rect);
    ctrl_client2screen(p_pic, &rect);

    if( strlen(pic_url) > 0)
    {
        u32 identity = 0;

        identity = ((u32)curWebsitesIndex<<24)
            | ((g_cur_mode&0XFF)<<16)
            | (u16)g_cur_draw_index;

        ui_pic_play_by_url((u8*)pic_url, &rect, identity);
        return SUCCESS;
    }
    else
    {
        return ERR_PARAM;
    }

}

static void ui_network_music_clear_draw_end_flag(void)
{
    u8 i;
    g_cur_draw_index = 0;
    for( i = 0; i < NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE; i++ )
    {
        g_draw_end_flag[i] = 0;
    }
}

static void ui_network_music_stop_draw_cover()
{
    pic_stop();
	g_need_draw_count = 0;
}

static void ui_list_start_roll(control_t *p_cont)
{
    roll_param_t p_param;
    MT_ASSERT(p_cont != NULL);

    memset(&p_param, 0, sizeof(roll_param_t));
    gui_stop_roll(p_cont);

    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats = 0;
    p_param.is_force = FALSE;

    gui_start_roll(p_cont, &p_param);

}


static void ui_network_music_update_page_num(control_t* p_page_cont, s16 page_num, s16 total_page)
{
    control_t *p_page_value = NULL;
    char num[22] = {0};
    s16 page_cnt = page_num + 1;

    p_page_value = ctrl_get_child_by_id(p_page_cont,IDC_NM_LIST_PAGE_NUM);
	if(0 == total_page)
	{
		page_cnt = 0;
	}
    sprintf(num, "%d/%d", page_cnt, total_page);
    text_set_content_by_ascstr(p_page_value, (u8*)num);
    ctrl_paint_ctrl(p_page_cont,TRUE);

}

static RET_CODE ui_network_music_update_pop_mainlist(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 i, cnt;
    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_strid(p_list, i, 0, pop_mainlist[i].name);
    }

    return SUCCESS;
}

static RET_CODE ui_network_music_update_tracklist(control_t* p_list, u16 start, u16 size, u32 context)
{
    control_t * p_menu,*p_list_cont,*p_page_cont;
    u16 i;
    net_music_tracks_content_t *p_track_data = ui_net_music_get_track_list();
    u32 count = p_track_data->tracks_count, duration;
    u8 hour,min,sec;
    char ch_duration[9] = {0};
    char name[128] = {0};
    g_track_cur_page = (s16)(start/NM_TRACK_LIST_ITEM_NUM_ONE_PAGE);
    g_track_total_page = (s16)((p_track_data->tracks_count + NM_TRACK_LIST_ITEM_NUM_ONE_PAGE -1)/NM_TRACK_LIST_ITEM_NUM_ONE_PAGE);
    for (i = start; i < start + size && i < count; i++)
    {
        sprintf(name, "%03d.%s",i + 1, p_track_data->tracks_content[i].name);
        list_set_field_content_by_ascstr(p_list, i, TRACKLIST_FIELD_ID_NAME, (u8*)name);
        duration = p_track_data->tracks_content[i].duration;
        hour = (u8) (duration / 3600);
        min = (u8) ((duration % 3600) / 60);
        sec = (u8) (duration % 60);
        sprintf(ch_duration, "%02d:%02d:%02d", hour, min, sec);
        list_set_field_content_by_ascstr(p_list, i, TRACKLIST_FIELD_ID_DURATION, (u8*)ch_duration);
        list_set_field_content_by_ascstr(p_list, i, TRACKLIST_FIELD_ID_ALBUM, (u8*)(p_track_data->tracks_content[i].album_name));
        list_set_field_content_by_ascstr(p_list, i, TRACKLIST_FIELD_ID_ARTIST, (u8*)(p_track_data->tracks_content[i].artist_name));
    }
    p_list_cont = ctrl_get_parent(p_list);
    p_menu = ctrl_get_parent(p_list_cont);
    p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
    ui_network_music_update_page_num(p_page_cont,g_track_cur_page,g_track_total_page);


    return SUCCESS;
}

static RET_CODE ui_network_music_update_album_artist_list(control_t* p_list_cont, u16 start, u16 size, u32 context)
{
    u32 i = 0;
    control_t * p_menu,*p_page_cont,*p_cell_cont;
    control_t *p_pic,*p_text;
    u32 album_cnt = 0;
    RET_CODE ret = 0;

    ui_network_music_clear_draw_end_flag();
    if (context == TYPE_ALBUMS)
    {
        net_music_albums_content_t *p_albumdata = ui_net_music_get_album_list();

        album_cnt = p_albumdata->albums_count;
        for (i = start; i< start + size ;i++)
        {
            p_cell_cont = ctrl_get_child_by_id(p_list_cont, (u16)(int)(IDC_NM_CELL_CONT_START + i%10));
            p_pic = ctrl_get_child_by_id(p_cell_cont, IDC_NM_CELL_PIC);
            p_text = ctrl_get_child_by_id(p_cell_cont, IDC_NM_CELL_NAME);
            if(i < p_albumdata->albums_count)
            {
              text_set_content_by_ascstr(p_text, (u8*)(p_albumdata->albums_content[i].name));
              bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
              ctrl_set_sts(p_cell_cont, OBJ_STS_SHOW);
            }else
            {
              text_set_content_by_ascstr(p_text, (u8*)"");
              bmap_set_content_by_id(p_pic, 0);
              ctrl_set_sts(p_cell_cont, OBJ_STS_HIDE);
            }

        }
        ctrl_paint_ctrl(p_list_cont,TRUE);
        g_need_draw_count = (s16)((p_albumdata->albums_count - start > NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE)?NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE:p_albumdata->albums_count - start);


    }
    else
    {
        net_music_artists_content_t *p_artistdata = ui_net_music_get_artist_list();

       album_cnt = p_artistdata->artists_count;
	for (i = start; i< start + size ;i++)
	{
         p_cell_cont = ctrl_get_child_by_id(p_list_cont, IDC_NM_CELL_CONT_START + i%10);
	  p_pic = ctrl_get_child_by_id(p_cell_cont, IDC_NM_CELL_PIC);
	  p_text = ctrl_get_child_by_id(p_cell_cont, IDC_NM_CELL_NAME);

         if(i < p_artistdata->artists_count)
         {
    		text_set_content_by_ascstr(p_text, (u8*)(p_artistdata->artists_content[i].name));
    		bmap_set_content_by_id(p_pic, IM_INDEX_NETWORK_MOVIE_PIC);
              ctrl_set_sts(p_cell_cont, OBJ_STS_SHOW);
         }
	  else
         {
             text_set_content_by_ascstr(p_text, (u8*)(""));
    	      bmap_set_content_by_id(p_pic, 0);
             ctrl_set_sts(p_cell_cont, OBJ_STS_HIDE);
         }
	}
       ctrl_paint_ctrl(p_list_cont,TRUE);
	g_need_draw_count = (s16)((p_artistdata->artists_count -start> NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE)?NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE:p_artistdata->artists_count - start);

    }

    g_album_cur_page = (s16)(start/NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE);

    g_album_total_page = (s16)((album_cnt + NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE - 1)/NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE);

    p_menu = ctrl_get_parent(p_list_cont);
    p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);

    ui_network_music_update_page_num(p_page_cont,g_album_cur_page,g_album_total_page);

    for (i = 0;i<(u16)(g_need_draw_count);i++)  //可能cover url有错，错的就跳过
    {
        ret = ui_draw_cover_pic_in_list(p_list_cont,g_cur_mode,(u8)i);
        if (SUCCESS == ret)
        {
            break;
        }
        g_draw_end_flag[i] = 1;
        g_cur_draw_index = (s16)i;

    }

    return SUCCESS;
}

static void ui_network_music_reset_music_playbar(void)
{
    control_t *p_menu,*p_main_info_cont,*p_cover_cont,*p_default_pic;
    control_t * p_playcont,*p_trackname,*p_play_icon;
    control_t * p_curtime,* p_toltime, *p_progress;


    p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    p_playcont = ctrl_get_child_by_id(p_menu,IDC_NM_PLAY_DETAIL_CONT);
    p_curtime = ctrl_get_child_by_id(p_playcont,IDC_NM_PLAY_CURTM);
    p_toltime = ctrl_get_child_by_id(p_playcont,IDC_NM_PLAY_TOLTM);
    p_progress = ctrl_get_child_by_id(p_playcont,IDC_NM_PLAY_PROGRESS);
    p_trackname = ctrl_get_child_by_id(p_playcont,IDC_NM_PLAY_TRACK_NAME);
    p_play_icon = ctrl_get_child_by_id(p_playcont,IDC_NM_PLAY_PLAY_ICON);

    text_set_content_by_ascstr(p_curtime, (u8*)("00:00"));
    text_set_content_by_ascstr(p_toltime, (u8*)("00:00"));
    pbar_set_current(p_progress,0);
    text_set_content_by_ascstr(p_trackname, (u8*)(" "));
    bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_PLAY_2);
    ctrl_paint_ctrl(p_playcont,TRUE);
    
    p_main_info_cont = ctrl_get_child_by_id(p_menu,IDC_NM_MAIN_INFO_CONT);
    p_cover_cont = ctrl_get_child_by_id(p_main_info_cont,IDC_NM_COVER_PIC_CONT);
    p_default_pic = ctrl_get_child_by_id(p_cover_cont,IDC_NM_COVER_DEFAULT_BMP);
    bmap_set_content_by_id(p_default_pic, IM_PIC_JAMEDO_LOGO);
    ctrl_paint_ctrl(p_cover_cont,TRUE);
    return;
}

static void ui_network_music_set_play_state(network_music_play_state_t state)
{
    g_cur_play_state = state;
}

static network_music_play_state_t ui_network_music_get_play_state()
{
     return g_cur_play_state;
}

static network_music_play_mode_t ui_network_music_get_play_mode()
{
    return g_cur_play_mode;
}

static u32 ui_network_music_convert_time_to_sec(utc_time_t *p_dur_time)
{
    u32 sec = 0;

    sec = (((p_dur_time->year * 365 +p_dur_time->day)
        * 24 + p_dur_time->hour)
        * 60 + p_dur_time->minute)
        *60 + p_dur_time->second;
    return sec;
}

static RET_CODE ui_network_music_update_next_play_track_pos(control_t *p_root)
{
    control_t *p_list_cont,*p_tracklist;
    net_music_tracks_content_t *p_trackcontent = ui_net_music_get_track_list();
    network_music_play_mode_t cur_play_mode = ui_network_music_get_play_mode();
    u16 focus = 0;

    p_list_cont = ctrl_get_child_by_id(p_root,IDC_NM_TRACK_LIST_CONT);
    p_tracklist = ctrl_get_child_by_id(p_list_cont,IDC_NM_TRACKLIST);
    OS_PRINTF("\n go into ui_network_music_update_next_play_track_pos\n");
    switch (cur_play_mode)
    {
    case PLAY_MODE_CYCLE_CUR_SONG:
        {
            //do nothing;
        }
       return SUCCESS;
    case PLAY_MODE_CYCLE_CUR_LIST:
        {
	     u16 next = 0;
            focus = list_get_focus_pos(p_tracklist);
            next = (u16)((focus+1)%p_trackcontent->tracks_count);
            list_set_focus_pos(p_tracklist,next);
	     OS_PRINTF("\n mode :PLAY_MODE_CYCLE_CUR_LIST,set new focus\n");
        }
      return SUCCESS;
    case PLAY_MODE_CYCLE_LIST_RADOM:
        {
            utc_time_t timenow = {0};
            u32 seed=0;
            time_get(&timenow, TRUE);
            seed = ui_network_music_convert_time_to_sec(&timenow);
            srand(seed);
            focus = (u16)(rand()%(int)(p_trackcontent->tracks_count));
            list_set_focus_pos(p_tracklist,focus);

        }
      return SUCCESS;
    case PLAY_MODE_NO_REPEAT:
        {
          return ERR_FAILURE;
        }
      //break;
    default:
        {
        }
        break;
    }
    
   OS_PRINTF("\n go out ui_network_music_update_next_play_track_pos\n");
   return ERR_FAILURE;
}

static RET_CODE ui_network_music_pre_play_track(u8* track_url,u8* cover_url)
{
    control_t *p_menu,*p_play,*p_trackname,*p_play_icon;
    control_t *p_list_cont,*p_tracklist,*p_total_time;
    u16 focus = 0;
    net_music_tracks_content_t *p_track_content = ui_net_music_get_track_list();
    u8 trackname[64] = {0};
    u8 total_time[6] = {0};
    RET_CODE ret = 0;

    p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    p_list_cont = ctrl_get_child_by_id(p_menu,IDC_NM_TRACK_LIST_CONT);
    p_tracklist = ctrl_get_child_by_id(p_list_cont,IDC_NM_TRACKLIST);
    p_play = ctrl_get_child_by_id(p_menu,IDC_NM_PLAY_DETAIL_CONT);
    p_trackname = ctrl_get_child_by_id(p_play,IDC_NM_PLAY_TRACK_NAME);
    p_play_icon = ctrl_get_child_by_id(p_play, IDC_NM_PLAY_PLAY_ICON);
    p_total_time = ctrl_get_child_by_id(p_play,IDC_NM_PLAY_TOLTM);

    focus = list_get_focus_pos(p_tracklist);
    if(0xffff == focus)
      {
      return SUCCESS;
      }
 
    ctrl_paint_ctrl(p_tracklist, TRUE);
    strcpy((char*)track_url,p_track_content->tracks_content[focus].audio);
    strcpy((char*)cover_url,p_track_content->tracks_content[focus].album_image);
    if(ret == SUCCESS)
    {
    }
    MT_ASSERT(ret == SUCCESS);
    total_play_time.minute = (u8)(p_track_content->tracks_content[focus].duration/60);
    total_play_time.second = (u8)(p_track_content->tracks_content[focus].duration%60);
    sprintf((char*)total_time,"%02d:%02d",total_play_time.minute,total_play_time.second);
    text_set_content_by_ascstr(p_total_time,total_time);

    sprintf((char*)trackname,"%d.%s",focus+1,p_track_content->tracks_content[focus].name);
    text_set_content_by_ascstr(p_trackname, trackname);
    bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_PLAY_2_SELECT);
    ctrl_paint_ctrl(p_play,TRUE);

    return SUCCESS;
}

static RET_CODE ui_network_music_stop_play()
{
    RET_CODE ret = 0;
    if (!g_nm_is_playing)
    {
        return SUCCESS;
    }
    ret = ui_net_music_play_ctrl_stop();
    g_nm_is_playing = FALSE;
    ui_network_music_set_play_state(NETWORK_MUSIC_STAT_STOP);
    ui_network_music_reset_music_playbar();
    return ret;
}
static RET_CODE ui_network_music_stop_play_without_reset()
{
    RET_CODE ret = 0;
    if (!g_nm_is_playing)
    {
        return SUCCESS;
    }
    ret = ui_net_music_play_ctrl_stop();
    g_nm_is_playing = FALSE;
    ui_network_music_set_play_state(NETWORK_MUSIC_STAT_STOP);
    return ret;
}


static RET_CODE ui_network_music_do_play(void)
{

    control_t *p_menu,*p_main_cont,*p_cover_cont;
    RET_CODE ret;
    u8 cover_url[256] = {0};
	
    if(TRUE == ui_is_mute())
      {
        ui_set_mute(TRUE);
      }
    else
      {
        ui_set_mute(FALSE);
      }
	
    memset(g_track_url,0,sizeof(g_track_url));
    ui_network_music_pre_play_track((u8*)g_track_url,cover_url);
    if(strlen(g_track_url) < 20)
     {
      return ERR_FAILURE;
     }
	
    if (g_nm_is_playing)
    {
        ret = ui_network_music_stop_play_without_reset();
        OS_PRINTF("%s :stop play,line :%d\n",__FUNCTION__,__LINE__);
        MT_ASSERT(ret == SUCCESS);
    }

    ret = ui_net_music_play_ctrl_set_url((u8*)g_track_url);
    MT_ASSERT(ret == SUCCESS);

    ret = ui_net_music_play_ctrl_start();
    MT_ASSERT(ret == SUCCESS);
    g_nm_is_playing = TRUE;

   ui_network_music_set_play_state(NETWORK_MUSIC_STAT_PLAY);
    p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    p_main_cont = ctrl_get_child_by_id(p_menu, IDC_NM_MAIN_INFO_CONT);
    p_cover_cont = ctrl_get_child_by_id(p_main_cont,IDC_NM_COVER_PIC_CONT);

    ui_draw_cover_for_item_on_focus(p_cover_cont,cover_url);

#if 0
   ui_net_music_play_ctrl_pause();
   ui_net_music_play_ctrl_resume();
   ui_network_music_set_play_state(NETWORK_MUSIC_STAT_PLAY);
#endif
 
    return ret;
}

static void ui_network_music_edit_cb(void)
{
    control_t *p_root = NULL;
    p_root = fw_find_root_by_id(ROOT_ID_NETWORK_MUSIC);

    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_MUSIC_INPUT_COMP, 0, 0);
    }
}


static RET_CODE on_network_music_exit_all(control_t *p_cont, u16 msg,
                                          u32 para1, u32 para2)
{

    if (g_nm_is_playing)
    {
        ui_network_music_stop_play();
    }
    ui_comm_dlg_close();  
	
    return ERR_NOFEATURE;
}

static RET_CODE on_network_music_exit(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
    OS_PRINTF("zzzzzzzzzzzzzzzzzzzzzzzz___music_exit\n");
    ui_comm_dlg_close();  

    ui_network_music_stop_play();

    return ERR_NOFEATURE;
}

static RET_CODE on_network_music_get_data_failed(control_t *p_cont, u16 msg,
                                                 u32 para1, u32 para2)
{
      control_t *p_menu,*p_page_cont;
    switch (para1)
    {
      case TYPE_TRACKS:
        g_track_total_num = 0;
        g_track_data_ready = FALSE;
        p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
        p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
        ui_network_music_update_page_num(p_page_cont,0,0);
	 return SUCCESS;
      //break;
      case TYPE_ALBUMS:
      case TYPE_ARTISTS:
        g_album_artist_data_ready = FALSE;
        if(TYPE_ALBUMS == para1)
	 {				
          p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
          p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
          ui_network_music_update_page_num(p_page_cont,0,0);
	   return SUCCESS;
	  }
	  else if(TYPE_ARTISTS== para1)
	  {
         p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
         p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
         ui_network_music_update_page_num(p_page_cont,0,0);
	  return SUCCESS;
        }
            
       break;
     default:
	break;
    }
    return SUCCESS;

}

static RET_CODE on_network_music_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u8 i = 0;
    u32 identity = 0;
    control_t* p_root = NULL;
    control_t* p_album_list = NULL;
    control_t* p_cell = NULL;
    control_t* p_pic = NULL;
    RET_CODE ret =0;

    //if( ROOT_ID_NETWORK_MUSIC != fw_get_focus_id() )
    //  return SUCCESS;

    identity = ((u32)(curWebsitesIndex)<<24)
        | (((g_cur_mode) &0XFF)<<16)
        | (u16)g_cur_draw_index;

    if( para1 != identity  )
    {
        return SUCCESS;
    }

    p_root =  ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);

    p_album_list = ctrl_get_child_by_id(p_root, IDC_NM_ALBUM_SINGER_CONT);
    p_cell = ctrl_get_child_by_id(p_album_list, (u16)(IDC_NM_CELL_CONT_START + g_cur_draw_index));
    p_pic = ctrl_get_child_by_id(p_cell, IDC_NM_CELL_PIC);

    bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_pic, TRUE);

    g_draw_end_flag[g_cur_draw_index] = 1;
    pic_stop();

    for( i = 0; i < g_need_draw_count; i++ )
    {
        if(0 == g_draw_end_flag[i])
        {
            g_cur_draw_index = i;
            ret = ui_draw_cover_pic_in_list(p_album_list,g_cur_mode,i);
            if (SUCCESS == ret)
            {
                break;
            }
            g_draw_end_flag[i] = 1;
        }

    }

    return SUCCESS;
}


static RET_CODE on_network_music_get_data_success(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
    control_t *p_listcont,*p_menu,*p_page_cont;

    fw_tmr_destroy(ROOT_ID_NETWORK_MUSIC, MSG_NET_MUSIC_TIMEOUT);

    switch (para1)
    {
    case TYPE_TRACKS:
        {
            control_t * p_list;
            net_music_tracks_content_t * p_trackcontent = ui_net_music_get_track_list();
            g_track_total_num = (s16)(p_trackcontent->tracks_count);
            p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
            if(FALSE == g_track_data_ready)
            {
              break;
            }
        
            ui_comm_dlg_close();
            if(0 == g_track_total_num)	
            {				
		//ui_comm_cfmdlg_open(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);					
        	p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
        	ui_network_music_update_page_num(p_page_cont,0,0);
		return SUCCESS;
	     }
            p_listcont = ctrl_get_child_by_id(p_ctrl,IDC_NM_TRACK_LIST_CONT);
            p_list = ctrl_get_child_by_id(p_listcont,IDC_NM_TRACKLIST);
            list_set_valid_pos(p_list, 0);
            list_set_current_pos(p_list,0);
            list_set_count(p_list, (u16)g_track_total_num, NM_TRACK_LIST_ITEM_NUM_ONE_PAGE);
            ui_network_music_update_tracklist(p_list,0,NM_TRACK_LIST_ITEM_NUM_ONE_PAGE,0);
            if (g_track_list_from_mode == NM_MODE_ALBUM || g_track_list_from_mode == NM_MODE_ARTIST)
            {
                control_t *p_main_cont,*p_cover_cont;
                list_set_focus_pos(p_list,0);
                list_select_item(p_list,0);
                p_main_cont = ctrl_get_child_by_id(p_menu, IDC_NM_MAIN_INFO_CONT);
                p_cover_cont = ctrl_get_child_by_id(p_main_cont,IDC_NM_COVER_PIC_CONT);
		  //ui_draw_cover_for_item_on_focus(p_cover_cont,(u8*)(p_trackcontent->tracks_content[0].album_image));
		  //lint -e663
                ui_draw_cover_for_item_on_focus(p_cover_cont, (u8*)(p_trackcontent->tracks_content->album_image));
		  //lint +e663
            }
            ctrl_set_sts(p_listcont,OBJ_STS_SHOW);

            ctrl_paint_ctrl(p_list,TRUE);
            ctrl_paint_ctrl(p_listcont->p_parent,TRUE);
            g_track_data_ready = TRUE;


        }
        break;
    case TYPE_ALBUMS:
    case TYPE_ARTISTS:
        {
          ui_comm_dlg_close();
      
      if(FALSE == g_album_artist_data_ready)
        {
          break;
        }

      if(TYPE_ALBUMS == para1)
			{				
				net_music_albums_content_t *p_album = ui_net_music_get_album_list();
				if(p_album->albums_count == 0)
          {
          p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
          p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
          ui_network_music_update_page_num(p_page_cont,0,0);
					return SUCCESS;
          }
			}
			else if(TYPE_ARTISTS== para1)
			{
				net_music_artists_content_t *p_artist = ui_net_music_get_artist_list();				
				if(p_artist->artists_count == 0)
         {         
          p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
         p_page_cont = ctrl_get_child_by_id(p_menu,IDC_NM_PAGE_CONT);
         ui_network_music_update_page_num(p_page_cont,0,0);
					return SUCCESS;
          }
       }
            p_listcont = ctrl_get_child_by_id(p_ctrl,IDC_NM_ALBUM_SINGER_CONT);
            //update page num
            //update album pic and name
            ctrl_set_sts(p_listcont,OBJ_STS_SHOW);
            g_curAlbumlistIndex = 0;
            g_cur_album_index_in_total = 0;
            ui_network_music_update_album_artist_list(p_listcont,0,NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE,(u32)para1);



            ctrl_paint_ctrl(p_listcont,TRUE);
            g_album_artist_data_ready = TRUE;

        }
        break;
    default:
        break;
    }
    return SUCCESS;
}


static RET_CODE on_network_music_play_end(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
    RET_CODE ret = 0;
    ret = ui_network_music_stop_play();

    ret = ui_network_music_update_next_play_track_pos(p_ctrl);
    if(SUCCESS == ret)
      {
        ui_network_music_do_play();
      }
    return SUCCESS;
}

static RET_CODE on_network_music_update_play_progress(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
    control_t *p_play_cont, *p_play_curtm, *p_total_tm, *p_play_progress;
    char asc[32];
    static u16 percent = 0;
    u16 temp = 0;
    p_play_cont = ctrl_get_child_by_id(p_ctrl, IDC_NM_PLAY_DETAIL_CONT);

    p_play_curtm = ctrl_get_child_by_id(p_play_cont, IDC_NM_PLAY_CURTM);
    p_total_tm = ctrl_get_child_by_id(p_play_cont, IDC_NM_PLAY_TOLTM);
    p_play_progress = ctrl_get_child_by_id(p_play_cont, IDC_NM_PLAY_PROGRESS);

    //  time_up(&cur_play_time, 1);
    cur_play_time.second = (u8)(para1%60);
    cur_play_time.minute = (u8)(para1/60);

    sprintf(asc, "%02d:%02d", cur_play_time.minute, cur_play_time.second);
    text_set_content_by_ascstr(p_play_curtm, (u8*)(asc));

    sprintf(asc, "%02d:%02d", total_play_time.minute, total_play_time.second);
    text_set_content_by_ascstr(p_total_tm, (u8*)(asc));

    temp = percent;
    if(time_conver(&total_play_time) != 0)
    {
        percent = (u16)(time_conver(&cur_play_time)*100/time_conver(&total_play_time));
    }

    //OS_PRINTF("percent = %d\n",percent);

    pbar_set_current(p_play_progress, percent);

    if (temp != percent)
    {
        ctrl_paint_ctrl(p_play_progress, TRUE);
    }
    ctrl_paint_ctrl(p_play_curtm, TRUE);
    ctrl_paint_ctrl(p_total_tm, TRUE);

    if(time_cmp(&cur_play_time, &total_play_time, FALSE) >= 0)
    {
        cur_play_time.second--;
    }

    return SUCCESS;
}


static RET_CODE on_network_music_switch_mode(control_t *p_ctrl, u16 msg,
                                             u32 para1, u32 para2)
{
    control_t *p_main_info = NULL, * p_mode_name = NULL,* p_mode_cont = NULL;
  //  control_t *p_track_list_cont,*p_ablum_singer_cont;
    control_t * p_ctrlfoucs,*p_pop_main_list;
//	net_music_param_t param = {0};
    g_last_mode = g_cur_mode;
    g_cur_mode = (nm_list_mode)(( (g_cur_mode + 1) > 3 ) ? 1 : (g_cur_mode + 1));
    //g_cur_mode++;

    g_track_data_ready = FALSE;
    g_album_artist_data_ready = FALSE;
    ui_network_music_stop_draw_cover();
    ui_network_music_stop_play();


    p_ctrlfoucs = ctrl_get_active_ctrl(p_ctrl);
    if (p_ctrlfoucs->id != ROOT_ID_NETWORK_MUSIC)
    {
        ctrl_default_proc(p_ctrl,MSG_LOSTFOCUS,0,0);
    }
    //ctrl_default_proc(p_ctrl,MSG_LOSTFOCUS,0,0);
    p_main_info = ctrl_get_child_by_id(p_ctrl, IDC_NM_MAIN_INFO_CONT);
    p_mode_cont = ctrl_get_child_by_id(p_main_info, IDC_NM_MODE_CONT);
    p_mode_name = ctrl_get_child_by_id(p_mode_cont, IDC_NM_MODE_NAME);
    p_pop_main_list = ctrl_get_child_by_id(p_main_info, IDC_NM_POP_MAIN_LIST);
    ctrl_default_proc(p_pop_main_list, MSG_GETFOCUS, 0, 0);
    list_select_item(p_pop_main_list,0);
    list_set_focus_pos(p_pop_main_list,0);
    ctrl_paint_ctrl(p_ctrl, TRUE);

    //伦笊辖悄Ｊ矫?

    text_set_content_by_strid(p_mode_name, nm_mode_name[g_cur_mode-1]);
    ctrl_paint_ctrl(p_mode_name, TRUE);



    //high light 移到 total popular，根据模式名切换右边列表形式；

    fw_tmr_reset(ROOT_ID_NETWORK_MUSIC, MSG_MUSIC_CHANGE_MODE, 700);
    fw_tmr_create(ROOT_ID_NETWORK_MUSIC, MSG_MUSIC_CHANGE_MODE, 700, FALSE);
    return SUCCESS;

}


static RET_CODE on_network_music_search(control_t *p_cont, u16 msg,
                                        u32 para1, u32 para2)
{
    manage_open_menu(ROOT_ID_NETWORK_MUSIC_SEARCH,(u32)ui_network_music_edit_cb,(u32)g_searchKey);
    return SUCCESS;
}

static RET_CODE  on_network_music_do_search(control_t *p_ctrl, u16 msg,
                                            u32 para1, u32 para2)
{
    s16 focus = ui_network_music_get_focus_btn_index();
    control_t *p_main_info = NULL, * p_mode_name = NULL,* p_mode_cont = NULL;
    control_t *p_track_list_cont,*p_ablum_singer_cont;
    control_t * p_ctrlfoucs,*p_pop_main_list;
    net_music_param_t param;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;

    memset(&param, 0, sizeof(net_music_param_t));
    inbuf = (char *)g_searchKey;
    outbuf = (char *)param.key_word;
    src_len = sizeof(g_searchKey);
    if( uni_strlen(g_searchKey) < 3)
    {
      ui_comm_cfmdlg_open_gb(NULL, (u8*)"keyword is too short!",  NULL, 0);
      return SUCCESS;
    }
    dest_len = sizeof(param.key_word);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    param.b_search = TRUE;
    param.music_img_size = IMG_SIZE_130;
    param.limit = 100;
    g_last_mode = g_cur_mode;
    g_cur_mode = (nm_list_mode)(focus + 1);//(search mode <=> music mode)
    g_cur_search_type = (u32)(int)focus;
    //以下处理和switch mode基本一致，search结果呈现时，根据search类型，显示对应的界面。
    ui_network_music_stop_draw_cover();
    ui_network_music_stop_play();


    p_ctrlfoucs = ctrl_get_active_ctrl(p_ctrl);
    if (p_ctrlfoucs->id != ROOT_ID_NETWORK_MUSIC)
    {
        ctrl_default_proc(p_ctrl,MSG_LOSTFOCUS,0,0);
    }
    //ctrl_default_proc(p_ctrl,MSG_LOSTFOCUS,0,0);
    p_main_info = ctrl_get_child_by_id(p_ctrl, IDC_NM_MAIN_INFO_CONT);
    p_mode_cont = ctrl_get_child_by_id(p_main_info, IDC_NM_MODE_CONT);
    p_mode_name = ctrl_get_child_by_id(p_mode_cont, IDC_NM_MODE_NAME);
    p_pop_main_list = ctrl_get_child_by_id(p_main_info, IDC_NM_POP_MAIN_LIST);
    ctrl_default_proc(p_pop_main_list, MSG_GETFOCUS, 0, 0);
    list_select_item(p_pop_main_list,0);
    list_set_focus_pos(p_pop_main_list,0);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    //更新左上角模式?
    text_set_content_by_strid(p_mode_name, nm_mode_name[g_cur_mode - 1]);
    ctrl_paint_ctrl(p_mode_name, TRUE);
    //high light 移到 total popular，根据模矫谢挥冶吡斜硇问剑?
    p_track_list_cont = ctrl_get_child_by_id(p_ctrl, IDC_NM_TRACK_LIST_CONT);
    p_ablum_singer_cont = ctrl_get_child_by_id(p_ctrl,IDC_NM_ALBUM_SINGER_CONT);
    if (g_cur_mode != NM_MODE_TRACK )
    {
        ctrl_set_sts(p_track_list_cont,OBJ_STS_HIDE);
        ctrl_set_sts(p_ablum_singer_cont,OBJ_STS_HIDE);
        ctrl_paint_ctrl(p_ctrl, TRUE);
        g_track_list_from_mode = NM_MODE_UNKNOW;

        //request data for default category
        //param.music_type = (g_cur_mode == NM_MODE_ALBUM)?TYPE_ALBUMS:TYPE_ARTISTS;
        //param.albums_order = request_popu_order[g_cur_mode -1][0];
        if(NM_MODE_ALBUM == g_cur_mode)
        {
            param.music_type = TYPE_ALBUMS;
            param.albums_order = (net_music_albums_order_t)(request_popu_order[g_cur_mode -1][focus]);
        }
        else if(NM_MODE_ARTIST == g_cur_mode)
        {
            param.music_type = TYPE_ARTISTS;
            param.artists_order = (net_music_artists_order_t)(request_popu_order[g_cur_mode -1][focus]);
        }
        ui_net_music_load_metadata(&param);
        g_track_data_ready = FALSE;
        g_album_artist_data_ready = TRUE;
    }
    else if (g_cur_mode ==  NM_MODE_TRACK)
    {

        ctrl_set_sts(p_ablum_singer_cont,OBJ_STS_HIDE);
        ctrl_set_sts(p_track_list_cont,OBJ_STS_HIDE);
        ctrl_paint_ctrl(p_ctrl, TRUE);
        g_track_list_from_mode = NM_MODE_TRACK;
        param.music_type = TYPE_TRACKS;
        param.tracks_order = (net_music_tracks_order_t)(request_popu_order[g_cur_mode -1][0]);
        ui_net_music_load_metadata(&param);
        g_album_artist_data_ready = FALSE;
        g_track_data_ready = TRUE;
        OS_PRINTF("network music open :request track info first time..\n");

    }

    //在另一个消息处理函数中，把返回的数据填入到右侧列表中；
    return SUCCESS;
}

static RET_CODE on_network_music_set_paly_mode(control_t *p_cont, u16 msg,
                                            u32 para1, u32 para2)
{
    u16 IM_id[4]= {IM_MP3_ICON_LISTPLAY, IM_MP3_ICON_LISTPLAY_R, IM_MP3_ICON_SINGLE_R, IM_MP3_ICON_NO_REPEAT};
    control_t * p_play_cont,*p_repeat_icon;
    g_cur_play_mode = (network_music_play_mode_t)((g_cur_play_mode + 1 >NETWORK_MUSIC_PLAY_MODE_COUNT)?1:(g_cur_play_mode + 1));

    p_play_cont = ctrl_get_child_by_id(p_cont,IDC_NM_PLAY_DETAIL_CONT);
    p_repeat_icon = ctrl_get_child_by_id(p_play_cont,IDC_NM_PLAY_REPEAT_ICON);

    bmap_set_content_by_id(p_repeat_icon, IM_id[g_cur_play_mode - 1]);
    ctrl_paint_ctrl(p_play_cont,TRUE);

    return SUCCESS;
}


/*!
 * list update function
 */




static RET_CODE on_nm_pop_mainlist_change_focus(control_t *p_list, u16 msg,
                                             u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
    control_t* p_popup = NULL;
    p_popup = fw_find_root_by_id(ROOT_ID_POPUP);
    if(p_popup)
    {
        return SUCCESS;
    }
    ret = list_class_proc(p_list, msg, 0, 0);
    ui_list_start_roll(p_list);
    return ret;
}



static RET_CODE on_nm_pop_mainlist_selected(control_t *p_ctrl, u16 msg,
           u32 para1, u32 para2)
{
    u16 focus = 0;
    control_t *p_menu;
    control_t *p_trackListcont, *p_tracklist,*p_ablum_singer_cont;
    net_music_param_t param;

    memset(&param, 0, sizeof(net_music_param_t));
    focus = list_get_focus_pos(p_ctrl);
    g_mainpoplist_index = (s16)focus;

    list_select_item(p_ctrl, focus);
    ctrl_paint_ctrl(p_ctrl, TRUE);


    p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);

    if (g_cur_mode == NM_MODE_TRACK)
    {       
       p_trackListcont = ctrl_get_child_by_id(p_menu,IDC_NM_TRACK_LIST_CONT);
       p_tracklist = ctrl_get_child_by_id(p_trackListcont,IDC_NM_TRACKLIST);
      /*clear the tracklist background*/
       ctrl_set_sts(p_trackListcont,OBJ_STS_HIDE);       
       ctrl_paint_ctrl(p_menu, TRUE);
       
       param.music_type = TYPE_TRACKS;
       param.limit = 100;
      // param.albums_order = request_popu_order[g_cur_mode - 1][focus];
       param.tracks_order = (net_music_tracks_order_t)(request_popu_order[g_cur_mode - 1][focus]);
       param.music_img_size = IMG_SIZE_150;
       list_set_valid_pos(p_tracklist, 0);
       list_set_current_pos(p_tracklist, 0);
        
        netmusic_open_dlg(IDS_LOADING_WITH_WAIT, 0);
       ui_net_music_load_metadata(&param);


    }
    else if (g_cur_mode != NM_MODE_TRACK)
    {
        if(NM_MODE_ALBUM == g_cur_mode)
        {
            param.music_type = TYPE_ALBUMS;
            param.albums_order = (net_music_albums_order_t)(request_popu_order[g_cur_mode -1][focus]);
        }
        else if(NM_MODE_ARTIST == g_cur_mode)
        {
             param.music_type = TYPE_ARTISTS;
             param.artists_order = (net_music_artists_order_t)(request_popu_order[g_cur_mode -1][focus]);
        }
         /*clear the singerlist or albumlist background*/
        p_ablum_singer_cont = ctrl_get_child_by_id(p_menu,IDC_NM_ALBUM_SINGER_CONT);          
        ctrl_set_sts(p_ablum_singer_cont,OBJ_STS_HIDE);       
        ctrl_paint_ctrl(p_menu, TRUE);
        //param.music_type = TYPE_ALBUMS;
        param.limit = 100;
        //param.albums_order = request_popu_order[g_cur_mode - 1][focus];
        param.music_img_size = IMG_SIZE_150;
        
        netmusic_open_dlg(IDS_LOADING_WITH_WAIT, 0);
        ui_net_music_load_metadata(&param);

    }


    return SUCCESS;
}

static RET_CODE on_nm_pop_mainlist_change_to_detail_list(control_t *p_ctrl, u16 msg,
                                                      u32 para1, u32 para2)
{
    control_t *p_menu;
    control_t *p_track_list_cont,*p_ablum_singer_cont;
    control_t *p_track_list;
    s16 track_num = 0;
   // ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    p_menu = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    
    if (g_cur_mode == NM_MODE_TRACK)
    {
      net_music_tracks_content_t * p_trackcontent = ui_net_music_get_track_list();
      track_num = (s16)(p_trackcontent->tracks_count);
        if ((FALSE == g_track_data_ready)||(0 == track_num))
        {
         // ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
            return SUCCESS;
        }
        if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
        {
            return SUCCESS;
        }
        p_track_list_cont = ctrl_get_child_by_id(p_menu, IDC_NM_TRACK_LIST_CONT);
        p_track_list = ctrl_get_child_by_id(p_track_list_cont, IDC_NM_TRACKLIST);
        
        ctrl_default_proc(p_track_list, MSG_GETFOCUS, 0, 0);
        list_set_focus_pos(p_track_list,0);
        list_select_item(p_track_list, 0);
        ctrl_paint_ctrl(p_track_list_cont, TRUE);
    }
    else
    {
        control_t * p_cellcont = NULL;
        control_t * p_name_text = NULL;
        if (FALSE == g_album_artist_data_ready)
        {
          ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0);
            return SUCCESS;
        }
        if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
        {
            return SUCCESS;
        }
        p_ablum_singer_cont = ctrl_get_child_by_id(p_menu, IDC_NM_ALBUM_SINGER_CONT);
        p_cellcont = ctrl_get_child_by_id(p_ablum_singer_cont, (u16)(IDC_NM_CELL_CONT_START + g_curAlbumlistIndex));
        p_name_text = ctrl_get_child_by_id(p_cellcont,IDC_NM_CELL_NAME);
        text_set_font_style(p_name_text, FSI_BLACK_16, FSI_BLACK_16, FSI_BLACK_16);
        ctrl_default_proc(p_cellcont, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_ablum_singer_cont, TRUE);
    }
    ctrl_default_proc(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_paint_ctrl(p_ctrl, TRUE);
    return SUCCESS;
}

static RET_CODE on_nm_tracklist_change_play_state(control_t *p_list, u16 msg,
     u32 para1, u32 para2)
{
  control_t *p_menu;
  control_t *p_play_cont,*p_play_icon;
  network_music_play_state_t play_status;

  play_status = ui_network_music_get_play_state();
  p_menu = ctrl_get_parent(p_list->p_parent);
  p_play_cont = ctrl_get_child_by_id(p_menu, IDC_NM_PLAY_DETAIL_CONT);
  p_play_icon = ctrl_get_child_by_id(p_play_cont,IDC_NM_PLAY_PLAY_ICON);

  switch(msg)
  {
  case MSG_MUSIC_PLAY:
   {
      ui_network_music_do_play();
   }
  break;
  case MSG_MUSIC_RESUME:
  case MSG_MUSIC_PAUSE:
    if(play_status == NETWORK_MUSIC_STAT_PAUSE)
    {
      ui_net_music_play_ctrl_resume();
      //fw_tmr_create(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR, 1000, TRUE);
      ui_network_music_set_play_state(NETWORK_MUSIC_STAT_PLAY);
     //
     bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_PLAY_2_SELECT);
    }
    else if(play_status == NETWORK_MUSIC_STAT_PLAY)
    {
      ui_net_music_play_ctrl_pause();
      //fw_tmr_destroy(ROOT_ID_USB_MUSIC, MSG_ONE_SECOND_UPDATE_PROCESS_BAR);
      ui_network_music_set_play_state(NETWORK_MUSIC_STAT_PAUSE);
      bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_PAUSE_SELECT);
    }
    else if((play_status == NETWORK_MUSIC_STAT_INVALID) || (play_status == NETWORK_MUSIC_STAT_STOP))
    {
      //on_music_list_play(p_list);
    }
    break;
/*
  case MSG_MUSIC_PRE:
    if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE) || (play_status == MUSIC_STAT_STOP))
    {
      ui_net_music_play_ctrl_pause(NETWORK_MUSIC_STAT_FB);
      music_update_play_status_icon(p_playicon_mbox, TRUE);
      do_play_pre_music(p_list);
      music_update_play_file_name(p_list, TRUE);
      music_update_select_item(p_list);
    }
    break;

  case MSG_MUSIC_NEXT:
    if((play_status == MUSIC_STAT_PLAY) || (play_status == MUSIC_STAT_PAUSE) || (play_status == MUSIC_STAT_STOP))
    {
      ui_music_set_play_status(MUSIC_STAT_FF);
      music_update_play_status_icon(p_playicon_mbox, TRUE);
      do_play_next_music(p_list);
      music_update_play_file_name(p_list, TRUE);
      music_update_select_item(p_list);
    }
    break;
*/
  case MSG_MUSIC_STOP:
    if((play_status == NETWORK_MUSIC_STAT_PLAY) || (play_status == NETWORK_MUSIC_STAT_PAUSE))
    {
      ui_network_music_stop_play();
      bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_STOP_SELECT);
    }
    break;

  default:
    break;
  }

  ctrl_paint_ctrl(p_play_cont, TRUE);

  return SUCCESS;

}


static RET_CODE on_nm_tracklist_slected(control_t *p_list, u16 msg,
                                        u32 para1, u32 para2)
{
    ui_network_music_do_play();

    return SUCCESS;
}

static RET_CODE  on_nm_tracklist_change_focus(control_t *p_list, u16 msg,
     u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;
    control_t* p_popup = NULL;
    s16 focus =0;
    focus = (s16)(list_get_focus_pos(p_list));
    if (MSG_FOCUS_DOWN  == msg && ((focus+1)%NM_TRACK_LIST_ITEM_NUM_ONE_PAGE == 0))
    {
        if(0 == g_track_total_num)
        {
          return SUCCESS;
        }
        ctrl_process_msg(p_list, MSG_PAGE_DOWN, 0, 0);
        list_set_focus_pos(p_list, (u16)((focus+1)% g_track_total_num));
        ctrl_paint_ctrl(p_list,TRUE);
        return SUCCESS;
    }

    if ( MSG_FOCUS_UP == msg  && (focus%NM_TRACK_LIST_ITEM_NUM_ONE_PAGE == 0))
    {
        focus = (focus > 0)?focus -1:g_track_total_num -1;
        ctrl_process_msg(p_list, MSG_PAGE_UP, 0, 0);
        list_set_focus_pos(p_list, (u16)focus);
        ctrl_paint_ctrl(p_list,TRUE);
        return SUCCESS;
    }

    p_popup = fw_find_root_by_id(ROOT_ID_POPUP);
    if(p_popup)
    {
        return SUCCESS;
    }
    ret = list_class_proc(p_list, msg, 0, 0);
    ui_list_start_roll(p_list);


    return ret;
}

static RET_CODE on_nm_tracklist_change_page(control_t *p_list, u16 msg,
                                            u32 para1, u32 para2)
{
    return ERR_NOFEATURE;

}

static RET_CODE on_nm_tracklist_return(control_t *p_list, u16 msg,
                                       u32 para1, u32 para2)
{
    control_t * p_menu,*p_list_cont,*p_album_cont,*p_main_info,*p_popmain_list;
    //control_t * p_cover_cont,*p_pic_default;
    u32 context = ( g_cur_mode == NM_MODE_ALBUM)?TYPE_ALBUMS:TYPE_ARTISTS;

    p_list_cont = ctrl_get_parent(p_list);
    p_menu = ctrl_get_parent(p_list_cont);
    p_album_cont = ctrl_get_child_by_id(p_menu,IDC_NM_ALBUM_SINGER_CONT);
    p_main_info = ctrl_get_child_by_id(p_menu,IDC_NM_MAIN_INFO_CONT);
    //p_cover_cont = ctrl_get_child_by_id(p_main_info,IDC_NM_COVER_PIC_CONT);
    //p_pic_default = ctrl_get_child_by_id(p_cover_cont,IDC_NM_COVER_DEFAULT_BMP);
    if (g_track_list_from_mode == NM_MODE_ALBUM || g_track_list_from_mode == NM_MODE_ARTIST)
    {
        ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
        ctrl_default_proc(p_album_cont, MSG_GETFOCUS, 0, 0);
        ctrl_set_sts(p_list_cont,OBJ_STS_HIDE);
        ctrl_set_sts(p_album_cont,OBJ_STS_SHOW);

	    //ctrl_paint_ctrl(p_album_cont, TRUE);
        ui_network_music_stop_play();
	 ui_network_music_update_album_artist_list(p_album_cont, (u16)(g_album_cur_page* NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE),
	 	NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE,context);
        ctrl_paint_ctrl(p_album_cont, TRUE);

        g_track_list_from_mode = NM_MODE_UNKNOW;
        g_track_data_ready = FALSE;
        g_album_artist_data_ready = TRUE;
    }
    else
    {
        p_popmain_list = ctrl_get_child_by_id(p_main_info,IDC_NM_POP_MAIN_LIST);
        ui_network_music_stop_play();
        ctrl_default_proc(p_list,MSG_LOSTFOCUS,0,0);
        ctrl_default_proc(p_popmain_list,MSG_GETFOCUS,0,0);
        ctrl_paint_ctrl(p_list,TRUE);
        ctrl_paint_ctrl(p_popmain_list, TRUE);

    }
   
    return SUCCESS;
}

static RET_CODE on_nm_tracklist_draw_cover_end(control_t *p_list, u16 msg,
                                               u32 para1, u32 para2)
{
    control_t * p_root;
    control_t * p_main_cont,*p_cover_cont,*p_default_pic;
    RET_CODE ret;
    OS_PRINTF("call %s @ line %d\n",__FUNCTION__,__LINE__);
    p_root =  ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    p_main_cont = ctrl_get_child_by_id(p_root, IDC_NM_MAIN_INFO_CONT);
    p_cover_cont = ctrl_get_child_by_id(p_main_cont,IDC_NM_COVER_PIC_CONT);
    p_default_pic = ctrl_get_child_by_id(p_cover_cont,IDC_NM_COVER_DEFAULT_BMP);

    bmap_set_content_by_id(p_default_pic, RSC_INVALID_ID);
    ctrl_paint_ctrl(p_default_pic, TRUE);
    g_cur_draw_index = 0;
    pic_stop();

    if (g_nm_is_playing)
    {
        OS_PRINTF("call %s @ line %d\n",__FUNCTION__,__LINE__);
        ret = ui_net_music_play_ctrl_start();
	 if(ret == SUCCESS)
	 {
	 }
        MT_ASSERT(ret == SUCCESS);
    }

    return SUCCESS;
}


static RET_CODE on_nm_tracklist_change_volume(control_t *p_cont, u16 msg,
     u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_NETWORK_MUSIC, para1);
    return SUCCESS;
}



static RET_CODE on_nm_album_artist_slected(control_t *p_ctrl, u16 msg,
     u32 para1, u32 para2)
{

    control_t *p_menu,*p_trackListcont;
    control_t *p_tracklist;
    net_music_param_t param;

    memset(&param, 0, sizeof(net_music_param_t));

    ui_network_music_stop_draw_cover();
    ctrl_default_proc(p_ctrl,MSG_LOSTFOCUS,0,0);
    ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
    //change album list to track list
    p_menu = ctrl_get_parent(p_ctrl);
    ctrl_paint_ctrl(p_menu, TRUE);
    p_trackListcont = ctrl_get_child_by_id(p_menu,IDC_NM_TRACK_LIST_CONT);
    ctrl_set_sts(p_trackListcont,OBJ_STS_SHOW);
    p_tracklist = ctrl_get_child_by_id(p_trackListcont,IDC_NM_TRACKLIST);
    ctrl_default_proc(p_tracklist,MSG_GETFOCUS,0,0);
    list_set_current_pos(p_tracklist,0);
    //ctrl_paint_ctrl(p_trackListcont,TRUE);
    g_track_list_from_mode = (g_cur_mode == NM_MODE_ALBUM)?NM_MODE_ALBUM:NM_MODE_ARTIST;
    param.music_type = TYPE_TRACKS;
    if (g_cur_mode == NM_MODE_ALBUM)
    {
      net_music_albums_content_t * p_album_content  = ui_net_music_get_album_list();
      param.index_id = (int)(p_album_content->albums_content[g_cur_album_index_in_total].id);
    }
    else
    {
         net_music_artists_content_t * p_artist_content = ui_net_music_get_artist_list();
         param.index_id = (int)(p_artist_content->artists_content[g_cur_album_index_in_total].id);
    }
    param.limit = 100;
    param.tracks_order =  TRACKS_POPULARITY_TOTAL;
    param.music_img_size = IMG_SIZE_150;
    param.track_enter = (g_cur_mode == NM_MODE_ALBUM)?ALBUMSTOTRACK:ARTISTSTOTRACK;
    g_album_artist_data_ready = FALSE;
    g_track_data_ready = TRUE;

    ui_net_music_load_metadata(&param);

    return SUCCESS;
}






      //ui_draw_cover_pic_in_list(p_ctrl,g_cur_mode,0);




static void on_nm_album_set_focus(control_t *p_cont, s16 last_focus, s16 focus)
{
    control_t* p_item = NULL;
    control_t* p_text = NULL;

    if( last_focus < 0 )
        ;
    else
    {
        p_item = ctrl_get_child_by_id(p_cont, (u16)(IDC_NM_CELL_CONT_START + last_focus));
        p_text = ctrl_get_child_by_id(p_item, IDC_NM_CELL_NAME);
        text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        ctrl_process_msg(p_item, MSG_LOSTFOCUS, 0, 0);

    }

    p_item = ctrl_get_child_by_id(p_cont, (u16)(IDC_NM_CELL_CONT_START + focus));
    p_text = ctrl_get_child_by_id(p_item, IDC_NM_CELL_NAME);
    text_set_font_style(p_text, FSI_BLACK_16, FSI_BLACK_16, FSI_BLACK_16);

    ctrl_process_msg(p_item, MSG_GETFOCUS, 0, 0);

    ctrl_paint_ctrl(p_cont, TRUE);
}

static RET_CODE on_nm_album_artist_change_page(control_t *p_ctrl, u16 msg,
     u32 para1, u32 para2)
{
	  net_music_albums_content_t *album_content = ui_net_music_get_album_list();
	  net_music_artists_content_t *artist_content = ui_net_music_get_artist_list();
      u32 context = ( g_cur_mode == NM_MODE_ALBUM)?TYPE_ALBUMS:TYPE_ARTISTS;
      u8 album_cnt = (u8)((g_cur_mode == NM_MODE_ALBUM)?album_content->albums_count:artist_content->artists_count);
      u8 page_cnt  = (album_cnt + NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE - 1)/NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE;
      u32 total_item = 0;
      if (g_cur_mode == NM_MODE_ALBUM)
      {
          net_music_albums_content_t *p_content = ui_net_music_get_album_list();
          total_item = p_content->albums_count;
      }
      else if (g_cur_mode == NM_MODE_ARTIST)
      {
          net_music_artists_content_t *p_content = ui_net_music_get_artist_list();
          total_item = p_content->artists_count;
      }
      if( g_cur_mode == NM_MODE_TRACK )
          return SUCCESS;
      if(0 == page_cnt)
      {
        return SUCCESS;
      }
      ui_network_music_clear_draw_end_flag();
      switch( msg )
      {
        case MSG_PAGE_UP:
          g_album_cur_page ++;
        break;
        case MSG_PAGE_DOWN:
          g_album_cur_page --;
        break;
	default:
	 break;
      }
      g_album_cur_page = (g_album_cur_page + page_cnt)%page_cnt;
      g_cur_album_index_in_total = (s16)(g_album_cur_page * NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE + g_curAlbumlistIndex);
      if(g_cur_album_index_in_total > (s16)(total_item - 1))
      {
	    s16 old_focus = g_curAlbumlistIndex;
        g_cur_album_index_in_total = (s16)(total_item - 1);
        g_curAlbumlistIndex = g_cur_album_index_in_total%NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE;
        on_nm_album_set_focus(p_ctrl, old_focus, g_curAlbumlistIndex);
      }
      ui_network_music_update_album_artist_list(p_ctrl, (u16)(g_album_cur_page* NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE),
	  	NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE,context);
      ctrl_paint_ctrl(p_ctrl,TRUE);
	  return SUCCESS;
}
static RET_CODE on_nm_album_artist_change_focus(control_t *p_cont, u16 msg,
     u32 para1, u32 para2)
{

    s16 old_focus = -1;
    u32 total_item = 0;
	s16 new_page;
    if (g_cur_mode == NM_MODE_ALBUM)
    {
        net_music_albums_content_t *p_content = ui_net_music_get_album_list();
        total_item = p_content->albums_count;
    }
    else if (g_cur_mode == NM_MODE_ARTIST)
    {
        net_music_artists_content_t *p_content = ui_net_music_get_artist_list();
        total_item = p_content->artists_count;
    }

    switch(msg)
    {
        case MSG_FOCUS_LEFT:
             g_cur_album_index_in_total--;
             if(g_cur_album_index_in_total == -1)
             {
               g_cur_album_index_in_total = (s16)(total_item - 1);
             }
             break;
        case MSG_FOCUS_RIGHT:
             g_cur_album_index_in_total++;
             if((u32)(int)g_cur_album_index_in_total == total_item)
             {
               g_cur_album_index_in_total = 0;
             }
             break;
        case MSG_FOCUS_UP:
        case MSG_FOCUS_DOWN:
             if((g_cur_album_index_in_total/5)%2 == 1)
             {
               g_cur_album_index_in_total = g_cur_album_index_in_total - 5;
             }
             else if((u32)(int)(g_cur_album_index_in_total+5) < total_item)
             {
               g_cur_album_index_in_total = g_cur_album_index_in_total + 5;
             }
             break;
	  default:
             break;
     }

     old_focus = g_curAlbumlistIndex;
     g_curAlbumlistIndex = g_cur_album_index_in_total%NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE;
     new_page = g_cur_album_index_in_total/10;
     if(new_page != g_album_cur_page)
     {
       if(MSG_FOCUS_RIGHT == msg)
         ctrl_process_msg(p_cont, MSG_PAGE_UP, 0, 0);
       if(MSG_FOCUS_LEFT == msg)
         ctrl_process_msg(p_cont, MSG_PAGE_DOWN, 0, 0);
     }
	
     on_nm_album_set_focus(p_cont, old_focus, g_curAlbumlistIndex);

     return SUCCESS;

}

static RET_CODE on_nm_album_artist_return(control_t *p_listcont, u16 msg,
                                          u32 para1, u32 para2)
{
    control_t *p_menu,*p_maininfo,*p_pop_mainlist,*p_item,*p_text;
    p_menu = ctrl_get_parent(p_listcont);
    p_maininfo = ctrl_get_child_by_id(p_menu,IDC_NM_MAIN_INFO_CONT);
    p_pop_mainlist = ctrl_get_child_by_id(p_maininfo,IDC_NM_POP_MAIN_LIST);

    p_item = ctrl_get_child_by_id(p_listcont, (u16)(IDC_NM_CELL_CONT_START + g_curAlbumlistIndex));
    p_text = ctrl_get_child_by_id(p_item, IDC_NM_CELL_NAME);
    text_set_font_style(p_text, FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
    ctrl_process_msg(p_item, MSG_LOSTFOCUS, 0, 0);
    ctrl_default_proc(p_listcont,MSG_LOSTFOCUS,0,0);
    ctrl_default_proc(p_pop_mainlist,MSG_GETFOCUS,0,0);
    ctrl_paint_ctrl(p_listcont,TRUE);
    ctrl_paint_ctrl(p_pop_mainlist,TRUE);
    return SUCCESS;
}

static RET_CODE on_nm_album_artist_cell_selected(control_t *p_cont, u16 msg,
     u32 para1, u32 para2)
{
    return SUCCESS;
}

static RET_CODE on_nm_album_artist_cell_change_focus(control_t *p_cont, u16 msg,
     u32 para1, u32 para2)
{
    return SUCCESS;
}

/*the function is to process the timeout for the music mode changing*/
static RET_CODE on_change_music_mode(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
  control_t *p_track_list_cont, *p_ablum_singer_cont;
  net_music_param_t param;

  memset(&param, 0, sizeof(net_music_param_t));

  p_track_list_cont = ctrl_get_child_by_id(p_ctrl, IDC_NM_TRACK_LIST_CONT);
  p_ablum_singer_cont = ctrl_get_child_by_id(p_ctrl,IDC_NM_ALBUM_SINGER_CONT);
  fw_tmr_destroy(ROOT_ID_NETWORK_MUSIC, MSG_MUSIC_CHANGE_MODE);
//  fw_tmr_create(ROOT_ID_NETWORK_MUSIC, MSG_MUSIC_CHANGE_MODE, 3000, TRUE);
  
  netmusic_open_dlg(IDS_LOADING_WITH_WAIT, 0);
  if (g_cur_mode != NM_MODE_TRACK )
  {
      ctrl_set_sts(p_track_list_cont,OBJ_STS_HIDE);
      ctrl_set_sts(p_ablum_singer_cont,OBJ_STS_HIDE);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      g_track_list_from_mode = NM_MODE_UNKNOW;
  
     //request data for default category
     param.music_type = (g_cur_mode == NM_MODE_ALBUM)?TYPE_ALBUMS:TYPE_ARTISTS;
     param.limit = 100;
     if(NM_MODE_ALBUM == g_cur_mode)
     {
        param.albums_order = (net_music_albums_order_t)(request_popu_order[g_cur_mode -1][0]);
     }
     else if(NM_MODE_ARTIST == g_cur_mode)
     {
         param.artists_order = (net_music_artists_order_t)(request_popu_order[g_cur_mode -1][0]);
     }
     param.music_img_size = IMG_SIZE_130;
     
     
     ui_net_music_load_metadata(&param);
     g_track_data_ready = FALSE;
     g_album_artist_data_ready = TRUE;  
  }
  else if (NM_MODE_TRACK == g_cur_mode)
  {
  
      ctrl_set_sts(p_ablum_singer_cont,OBJ_STS_HIDE);
      ctrl_set_sts(p_track_list_cont,OBJ_STS_HIDE);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      g_track_list_from_mode = NM_MODE_TRACK;
  
      param.music_type = TYPE_TRACKS;
      param.limit = 100;
      param.tracks_order = (net_music_tracks_order_t)(request_popu_order[g_cur_mode -1][0]);
      param.music_img_size = IMG_SIZE_150;
  
      ui_net_music_load_metadata(&param);
      g_album_artist_data_ready = FALSE;  
      g_track_data_ready = TRUE;
  }
        
  return SUCCESS;
}

RET_CODE open_network_music(u32 para1, u32 para2)
{

    control_t *p_menu,*p_help_bar;
    control_t *p_main_info_cont,*p_track_list_cont,*p_album_singer_cont;
    control_t *p_mode_cont,*p_web_site,*p_mode_name;
    control_t *p_pop_main_list;
    control_t *p_cover_pic_cont,*p_left_cover_bmp;
    control_t *p_page_cont,*p_arrow_icon,*p_page_text,*p_page_value;
    control_t *p_track_list;
    control_t *p_cell_cont[NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE];
    control_t *p_cell_pic[NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE];
    control_t *p_cell_name_text[NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE];
    control_t *p_play_cont,*p_play_progress,*p_play_curtm,*p_play_toltm;
    control_t *p_play_icon,*p_repeat_icon,*p_track_name;
    u8 i=0;



    comm_help_data_t help =
    {
        4,
        4,
        {
                IDS_SWITCH,
                IDS_SEARCH,
                IDS_REPEAT,
                IDS_RETURN,
        },

        {
                IM_HELP_RED,
                IM_HELP_GREEN,
                IM_HELP_BLUE,
                IM_HELP_MENU,
        },
    };

    curWebsitesIndex = (u8)para2;

    ui_network_music_pic_init();
    ui_net_music_metadata_init();
    ui_net_music_play_ctrl_init();

 
  // create timer for 20s timeout
  fw_tmr_create(ROOT_ID_NETWORK_MUSIC, MSG_NET_MUSIC_TIMEOUT, 20*1000, FALSE);

    p_menu = ui_comm_root_create_netmenu(ROOT_ID_NETWORK_MUSIC,
        0, IM_INDEX_JAMENDO_BANNER, IDS_NETWORK_MUSIC);
    if(p_menu == NULL)
    {
        return ERR_FAILURE;
    }
    ctrl_set_keymap(p_menu, network_music_cont_keymap);
    ctrl_set_proc(p_menu, network_music_cont_proc);


    //main info
    p_main_info_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_MAIN_INFO_CONT, NM_MAIN_INFO_CONT_X,
        NM_MAIN_INFO_CONT_Y, NM_MAIN_INFO_CONT_W, NM_MAIN_INFO_CONT_H, p_menu, 0);
    ctrl_set_rstyle(p_main_info_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);


    p_mode_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_MODE_CONT, NM_MODE_CONT_X,NM_MODE_CONT_Y,
        NM_MODE_CONT_W,NM_MODE_CONT_H,p_main_info_cont,0);
    ctrl_set_rstyle(p_mode_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //web name
    p_web_site = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_WEB_SITE, NM_WEB_SITE_X, NM_WEB_SITE_Y,
        NM_WEB_SITE_W, NM_WEB_SITE_H, p_mode_cont, 0);
    bmap_set_content_by_id(p_web_site, IM_PIC_MUSIC_LOGO);

    //mode name
    p_mode_name = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_MODE_NAME, NM_MODE_NAME_X, NM_MODE_NAME_Y,
        NM_MODE_NAME_W, NM_MODE_NAME_H, p_mode_cont, 0);
    ctrl_set_rstyle(p_mode_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_mode_name, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_mode_name, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_type(p_mode_name, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_mode_name, IDS_MODE_TRACK);

    //pop main list
    p_pop_main_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_POP_MAIN_LIST,NM_POP_MAIN_LIST_X, NM_POP_MAIN_LIST_Y,
        NM_POP_MAIN_LIST_W, NM_POP_MAIN_LIST_H, p_main_info_cont, 0);
    ctrl_set_rstyle(p_pop_main_list,RSI_PBACK , RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_pop_main_list, network_music_pop_mainlist_keymap);
    ctrl_set_proc(p_pop_main_list, network_music_pop_mainlist_proc);

    ctrl_set_mrect(p_pop_main_list, 0, 0, 180, 180);
    list_set_item_interval(p_pop_main_list, 15);
    list_set_item_rstyle(p_pop_main_list, &pop_mainlist_item_rstyle);
    list_enable_select_mode(p_pop_main_list, TRUE);
    list_set_count(p_pop_main_list, NM_POP_LIST_ITEM_COUNT, NM_POP_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_pop_main_list, ARRAY_SIZE(popmainlist_attr), NM_POP_LIST_ITEM_NUM_ONE_PAGE);
    list_set_select_mode(p_pop_main_list, LIST_SINGLE_SELECT);
    list_set_focus_pos(p_pop_main_list, 0);
    list_select_item(p_pop_main_list,0);
    list_enable_page_mode(p_pop_main_list, TRUE);
    g_cur_mode = NM_MODE_TRACK;
    g_last_mode = NM_MODE_TRACK;


    list_set_update(p_pop_main_list, ui_network_music_update_pop_mainlist, 0);


    for (i = 0; i < ARRAY_SIZE(popmainlist_attr); i++)
    {
        list_set_field_attr(p_pop_main_list, i, popmainlist_attr[i].attr,
            popmainlist_attr[i].width, popmainlist_attr[i].left, popmainlist_attr[i].top);
        list_set_field_rect_style(p_pop_main_list, i, popmainlist_attr[i].rstyle);
        list_set_field_font_style(p_pop_main_list, i, popmainlist_attr[i].fstyle);
    }

    ui_network_music_update_pop_mainlist(p_pop_main_list, list_get_valid_pos(p_pop_main_list), 3, 0);


    //left_down cover pic
    p_cover_pic_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_COVER_PIC_CONT,NM_COVER_PIC_CONT_X, NM_COVER_PIC_CONT_Y,
        NM_COVER_PIC_CONT_W, NM_COVER_PIC_CONT_H, p_main_info_cont, 0);
    ctrl_set_rstyle(p_cover_pic_cont, RSI_BOX_1, RSI_BOX_1, RSI_BOX_1);
    p_left_cover_bmp = ctrl_create_ctrl(CTRL_BMAP,IDC_NM_COVER_DEFAULT_BMP,NM_COVER_LOAD_BMP_X,NM_COVER_LOAD_BMP_Y,
        NM_COVER_LOAD_BMP_W,NM_COVER_LOAD_BMP_H,p_cover_pic_cont,0);
    ctrl_set_rstyle(p_left_cover_bmp, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    bmap_set_content_by_id(p_left_cover_bmp, IM_PIC_JAMEDO_LOGO);


    //page info
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_PAGE_CONT, NM_PAGE_CONTX,
        NM_PAGE_CONTY,NM_PAGE_CONTW, NM_PAGE_CONTH,p_menu, 0);
    ctrl_set_rstyle(p_page_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_arrow_icon = ctrl_create_ctrl(CTRL_BMAP, 0, NM_PAGE_ARROWL_X, NM_PAGE_ARROWL_Y,
        NM_PAGE_ARROWL_W, NM_PAGE_ARROWL_H, p_page_cont, 0);
    bmap_set_content_by_id(p_arrow_icon, IM_ARROW1_L);
    p_arrow_icon = ctrl_create_ctrl(CTRL_BMAP, 0,NM_PAGE_ARROWR_X, NM_PAGE_ARROWR_Y,
        NM_PAGE_ARROWR_W, NM_PAGE_ARROWR_H, p_page_cont, 0);
    bmap_set_content_by_id(p_arrow_icon, IM_ARROW1_R);

    p_page_text = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,NM_PAGE_NAME_X, NM_PAGE_NAME_Y,
        NM_PAGE_NAME_W, NM_PAGE_NAME_H, p_page_cont, 0);
    ctrl_set_rstyle(p_page_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_page_text, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_page_text, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
    text_set_content_by_strid(p_page_text, IDS_PAGE1);

    p_page_value = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_NM_LIST_PAGE_NUM,
        NM_PAGE_VALUE_X, NM_PAGE_VALUE_Y,
        NM_PAGE_VALUE_W, NM_PAGE_VALUE_H,
        p_page_cont, 0);
    ctrl_set_rstyle(p_page_value, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_page_value, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_page_value, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
    text_set_content_type(p_page_value, TEXT_STRTYPE_UNICODE);

    //track list cont
    p_track_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_TRACK_LIST_CONT, NM_TRACK_LIST_CONT_X,
        NM_TRACK_LIST_CONT_Y, NM_TRACK_LIST_CONT_W, NM_TRACK_LIST_CONT_H, p_menu, 0);
    ctrl_set_rstyle(p_track_list_cont, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);

    p_track_list = ctrl_create_ctrl(CTRL_LIST, IDC_NM_TRACKLIST, NM_TRACK_LIST_X, NM_TRACK_LIST_Y,
        NM_TRACK_LIST_W, NM_TRACK_LIST_H, p_track_list_cont, 0);
    ctrl_set_rstyle(p_track_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_track_list, network_music_tracklist_keymap);
    ctrl_set_proc(p_track_list, network_music_tracklist_proc);
    ctrl_set_mrect(p_track_list, NM_TRACK_LIST_X, NM_TRACK_LIST_Y,
        NM_TRACK_LIST_W, NM_TRACK_LIST_H);
    list_set_item_interval(p_track_list, NM_TRACK_ITEM_V_GAP);
    list_set_item_rstyle(p_track_list, &tracklist_item_rstyle);
    list_enable_select_mode(p_track_list, TRUE);
    list_enable_page_mode(p_track_list, TRUE);

    list_set_field_count(p_track_list, ARRAY_SIZE(tracklist_attr), NM_TRACK_LIST_ITEM_NUM_ONE_PAGE);
    list_set_select_mode(p_track_list, LIST_SINGLE_SELECT);
    list_set_focus_pos(p_track_list, 0);

    list_set_update(p_track_list, ui_network_music_update_tracklist, 0);

    for(i = 0; i < ARRAY_SIZE(tracklist_attr); i++)
    {
        list_set_field_attr2(p_track_list, (u8)i, tracklist_attr[i].attr,
            tracklist_attr[i].width, tracklist_attr[i].height, tracklist_attr[i].left, tracklist_attr[i].top);
        list_set_field_rect_style(p_track_list, (u8)i, tracklist_attr[i].rstyle);
        list_set_field_font_style(p_track_list, (u8)i, tracklist_attr[i].fstyle);
    }

    ctrl_set_sts(p_track_list_cont, OBJ_STS_HIDE);

    //Album/Singer cont
    p_album_singer_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_ALBUM_SINGER_CONT, NM_ALBUM_ARTIST_CONT_X,
        NM_ALBUM_ARTIST_CONT_Y, NM_ALBUM_ARTIST_CONT_W, NM_ALBUM_ARTIST_CONT_H, p_menu, 0);
    ctrl_set_rstyle(p_album_singer_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_album_singer_cont, network_music_album_artist_keymap);
    ctrl_set_proc(p_album_singer_cont, network_music_album_artist_proc);

    for (i = 0;i < NM_ALBUM_ARTIST_ITEM_NUM_ONE_PAGE;i++)
    {
        //cell cont for pic and name
        p_cell_cont[i] = ctrl_create_ctrl(CTRL_CONT, IDC_NM_CELL_CONT_START + i,
            NM_ALBUM_ARTIST_CELL_X + (i%5)*(NM_ALBUM_ARTIST_CELL_W + NM_ALBUM_ARTIST_CELL_H_GAP),
            NM_ALBUM_ARTIST_CELL_Y + (i/5)*(NM_ALBUM_ARTIST_CELL_H + NM_ALBUM_ARTIST_CELL_V_GAP),
            NM_ALBUM_ARTIST_CELL_W,  NM_ALBUM_ARTIST_CELL_H, p_album_singer_cont, 0);
        ctrl_set_rstyle(p_cell_cont[i], RSI_PBACK, RSI_ITEM_2_HL, RSI_PBACK);
/*
        ctrl_set_keymap(p_cell_cont[i], network_music_cell_cont_keymap);
        ctrl_set_proc(p_cell_cont[i], network_music_cell_cont_proc);*/

        //ctrl_set_related_id(p_cell_cont[i], ((i == 0)?9:i), (i < 3)?(i + 7):(i - 2),((i == (9 - 1))?1:(i + 2)),(i > 5)?(i - 5):(i + 4));

        //pic_cont for pic from network
        p_cell_pic[i] = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_CELL_PIC, NM_ALBUM_SINGER_CELL_PIC_X,
            NM_ALBUM_SINGER_CELL_PIC_Y, NM_ALBUM_SINGER_CELL_PIC_W, NM_ALBUM_SINGER_CELL_PIC_H, p_cell_cont[i], 0);
        ctrl_set_rstyle(p_cell_pic[i], RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

        p_cell_name_text[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_CELL_NAME, NM_ALBUM_SINGER_CELL_NAME_X, NM_ALBUM_SINGER_CELL_NAME_Y,
            NM_ALBUM_SINGER_CELL_NAME_W, NM_ALBUM_SINGER_CELL_NAME_H, p_cell_cont[i], 0);
        ctrl_set_rstyle(p_cell_name_text[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);

        text_set_font_style(p_cell_name_text[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        text_set_content_type(p_cell_name_text[i], TEXT_STRTYPE_UNICODE);



    }
    ctrl_set_sts(p_album_singer_cont, OBJ_STS_HIDE);

    //play process info bar
    //play container
    p_play_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NM_PLAY_DETAIL_CONT,
        NM_PLAY_CONT_X, NM_PLAY_CONT_Y,NM_PLAY_CONT_W, NM_PLAY_CONT_H, p_menu, 0);
    ctrl_set_rstyle(p_play_cont, RSI_NM_DETAIL, RSI_NM_DETAIL, RSI_NM_DETAIL);

    //playing track name
    p_track_name = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_PLAY_TRACK_NAME,NM_PLAY_TRACK_NAME_X, NM_PLAY_TRACK_NAME_Y,
        NM_PLAY_TRACK_NAME_W, NM_PLAY_TRACK_NAME_H,p_play_cont, 0);
    ctrl_set_rstyle(p_track_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_track_name, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX);
    text_set_align_type(p_track_name, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_track_name, TEXT_STRTYPE_UNICODE);


    //play bar
    //paly/pause icon
    p_play_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_PLAY_PLAY_ICON,
        NM_PLAY_PLAY_ICON_X, NM_PLAY_PLAY_ICON_Y,NM_PLAY_PLAY_ICON_W, NM_PLAY_PLAY_ICON_H, p_play_cont, 0);
    ctrl_set_rstyle(p_play_icon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_play_icon, IM_MP3_ICON_PLAY_2);

    p_repeat_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_NM_PLAY_REPEAT_ICON,
        NM_PLAY_REPEAT_ICON_X, NM_PLAY_REPEAT_ICON_Y,NM_PLAY_REPEAT_ICON_W, NM_PLAY_REPEAT_ICON_H, p_play_cont, 0);
    ctrl_set_rstyle(p_play_icon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    bmap_set_content_by_id(p_repeat_icon, IM_MP3_ICON_LISTPLAY);

    //repeat icon

    //progress
    p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_NM_PLAY_PROGRESS,
        NM_PLAY_PROGRESS_X, NM_PLAY_PROGRESS_Y,
        NM_PLAY_PROGRESS_W, NM_PLAY_PROGRESS_H,
        p_play_cont, 0);
    ctrl_set_rstyle(p_play_progress, RSI_ITEM_1_HL, RSI_ITEM_1_HL, RSI_ITEM_1_HL);
    ctrl_set_mrect(p_play_progress,
        NM_PLAY_PROGRESS_MIDX, NM_PLAY_PROGRESS_MIDY,
        NM_PLAY_PROGRESS_MIDW, NM_PLAY_PROGRESS_MIDH);
    pbar_set_rstyle(p_play_progress, NM_PLAY_PROGRESS_MIN, NM_PLAY_PROGRESS_MAX, NM_PLAY_PROGRESS_MID);
    pbar_set_count(p_play_progress, 0, 100, 100);
    pbar_set_direction(p_play_progress, 1);
    pbar_set_workmode(p_play_progress, 0, 0);
    pbar_set_current(p_play_progress, 0);
    // current time
    p_play_curtm = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_PLAY_CURTM,
        NM_PLAY_CURTM_X, NM_PLAY_CURTM_Y,
        NM_PLAY_CURTM_W, NM_PLAY_CURTM_H,
        p_play_cont, 0);
    ctrl_set_rstyle(p_play_curtm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_play_curtm, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX);
    text_set_align_type(p_play_curtm, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_play_curtm, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_play_curtm, (u8*)("00:00"));
    //total time
    p_play_toltm = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_PLAY_TOLTM,
        NM_PLAY_TOLTM_X, NM_PLAY_TOLTM_Y,
        NM_PLAY_TOLTM_W, NM_PLAY_TOLTM_H,
        p_play_cont, 0);
    ctrl_set_rstyle(p_play_toltm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_play_toltm, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX, FSI_NM_INFO_MBOX);
    text_set_align_type(p_play_toltm, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_play_toltm, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_play_toltm, (u8*)("00:00"));


    //bottom help bar
    p_help_bar = ctrl_create_ctrl(CTRL_TEXT, IDC_NM_HELP_BAR,
        NM_BOTTOM_HELP_X, NM_BOTTOM_HELP_Y, NM_BOTTOM_HELP_W,
        NM_BOTTOM_HELP_H, p_menu, 0);
    ctrl_set_rstyle(p_help_bar, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
    text_set_font_style(p_help_bar, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_help_bar, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_help_bar, TEXT_STRTYPE_UNICODE);
    ui_comm_help_create_ext(40, 0, NM_BOTTOM_HELP_W-40, NM_BOTTOM_HELP_H,  &help,  p_help_bar);

    ctrl_default_proc(p_pop_main_list, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
    netmusic_open_dlg(IDS_LOADING_WITH_WAIT, 0);

	//request track data for tracklist
	if (1)
	{
	    net_music_param_t param;
	    memset(&param, 0, sizeof(net_music_param_t));

	    param.music_type = TYPE_TRACKS;
           param.limit = 100;
           param.tracks_order = (net_music_tracks_order_t)(request_popu_order[g_cur_mode - 1][0]);
           param.music_img_size = IMG_SIZE_150;
           g_track_data_ready = TRUE;

           ui_net_music_load_metadata(&param);
	}

    return SUCCESS;

}

static RET_CODE on_network_music_mute(control_t *p_ctrl, u16 msg,
						u32 para1, u32 para2)
{
      u8 index;
  index = fw_get_focus_id();
  if(index != ROOT_ID_DO_SEARCH && index != ROOT_ID_MAINMENU)
  {
    ui_set_mute((BOOL)(!ui_is_mute()));
  }

  return SUCCESS;
}


 
static void exit_net_music(void)
{
  control_t *p_menu = NULL;
  p_menu =  ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);

  ctrl_process_msg(p_menu, MSG_EXIT_ALL, 0, 0);
}

static RET_CODE on_net_music_timeout(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_TIMEOUT, exit_net_music, 0);

  return SUCCESS;
}

static RET_CODE on_network_music_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if (g_nm_is_playing)
  {
    ui_net_music_play_ctrl_stop();
    g_nm_is_playing = FALSE;
    ui_network_music_set_play_state(NETWORK_MUSIC_STAT_STOP);
  }

  ui_net_music_metadata_release();
  ui_network_music_pic_deinit();
  ui_net_music_play_ctrl_release();
  
  fw_tmr_destroy(ROOT_ID_NETWORK_MUSIC, MSG_NET_MUSIC_TIMEOUT);
  
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(network_music_cont_keymap, NULL)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_RED,MSG_SWITCH_MODE)
ON_EVENT(V_KEY_GREEN,MSG_NM_SEARCH)
ON_EVENT(V_KEY_BLUE, MSG_BLUE)
ON_EVENT(V_KEY_REPEAT, MSG_BLUE)
ON_EVENT(V_KEY_MUTE, MSG_MUTE)
END_KEYMAP(network_music_cont_keymap, NULL)

BEGIN_MSGPROC(network_music_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_MUTE, on_network_music_mute)
ON_COMMAND(MSG_DESTROY, on_network_music_destory)
ON_COMMAND(MSG_EXIT_ALL, on_network_music_exit_all)
ON_COMMAND(MSG_EXIT, on_network_music_exit)
ON_COMMAND(MSG_SWITCH_MODE, on_network_music_switch_mode)
ON_COMMAND(MSG_NM_SEARCH, on_network_music_search)
ON_COMMAND(MSG_MUSIC_INPUT_COMP,on_network_music_do_search)
ON_COMMAND(MSG_BLUE, on_network_music_set_paly_mode)
ON_COMMAND(MSG_PIC_EVT_DRAW_END,on_network_music_pic_draw_end)
ON_COMMAND(MSG_PIC_EVT_UNSUPPORT,on_network_music_pic_draw_end)
ON_COMMAND(MSG_PIC_EVT_DATA_ERROR,on_network_music_pic_draw_end)
ON_COMMAND(MSG_NET_MUSIC_API_EVT_FAILED,on_network_music_get_data_failed)
ON_COMMAND(MSG_NET_MUSIC_API_EVT_SUCCESS,on_network_music_get_data_success)
ON_COMMAND(MSG_NET_MUSIC_PLAY_END, on_network_music_play_end)
ON_COMMAND(MSG_NET_MUSIC_DATA_ERROR, on_network_music_play_end)
ON_COMMAND(MSG_NET_MUSIC_GET_PLAY_TIME, on_network_music_update_play_progress)
ON_COMMAND(MSG_MUSIC_CHANGE_MODE, on_change_music_mode)
ON_COMMAND(MSG_NET_MUSIC_TIMEOUT, on_net_music_timeout)
END_MSGPROC(network_music_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(network_music_pop_mainlist_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(network_music_pop_mainlist_keymap, NULL)

BEGIN_MSGPROC(network_music_pop_mainlist_proc, list_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_nm_pop_mainlist_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_nm_pop_mainlist_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_nm_pop_mainlist_change_to_detail_list)
ON_COMMAND(MSG_SELECT, on_nm_pop_mainlist_selected)
END_MSGPROC(network_music_pop_mainlist_proc, list_class_proc)

BEGIN_KEYMAP(network_music_tracklist_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
ON_EVENT(V_KEY_MENU,MSG_RETURN)
ON_EVENT(V_KEY_PLAY, MSG_MUSIC_RESUME)
ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
ON_EVENT(V_KEY_NEXT, MSG_MUSIC_NEXT)
ON_EVENT(V_KEY_PREV, MSG_MUSIC_PRE)
ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)
END_KEYMAP(network_music_tracklist_keymap, NULL)

BEGIN_MSGPROC(network_music_tracklist_proc, list_class_proc)
ON_COMMAND(MSG_SELECT, on_nm_tracklist_slected)
ON_COMMAND(MSG_FOCUS_UP, on_nm_tracklist_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_nm_tracklist_change_focus)
ON_COMMAND(MSG_VOLUME_DOWN, on_nm_tracklist_change_volume)
ON_COMMAND(MSG_VOLUME_UP, on_nm_tracklist_change_volume)
ON_COMMAND(MSG_PAGE_DOWN, on_nm_tracklist_change_page)
ON_COMMAND(MSG_PAGE_UP, on_nm_tracklist_change_page)
ON_COMMAND(MSG_RETURN,on_nm_tracklist_return)
ON_COMMAND(MSG_PIC_EVT_UNSUPPORT,on_nm_tracklist_draw_cover_end)
ON_COMMAND(MSG_PIC_EVT_DATA_ERROR,on_nm_tracklist_draw_cover_end)
ON_COMMAND(MSG_PIC_EVT_DRAW_END,on_nm_tracklist_draw_cover_end)
ON_COMMAND(MSG_MUSIC_PLAY,on_nm_tracklist_change_play_state)
ON_COMMAND(MSG_MUSIC_RESUME, on_nm_tracklist_change_play_state)
ON_COMMAND(MSG_MUSIC_PAUSE, on_nm_tracklist_change_play_state)
ON_COMMAND(MSG_MUSIC_NEXT, on_nm_tracklist_change_play_state)
ON_COMMAND(MSG_MUSIC_PRE, on_nm_tracklist_change_play_state)
ON_COMMAND(MSG_MUSIC_STOP, on_nm_tracklist_change_play_state)
END_MSGPROC(network_music_tracklist_proc, list_class_proc)

BEGIN_KEYMAP(network_music_album_artist_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_MENU,MSG_RETURN)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(network_music_album_artist_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_music_album_artist_proc, ui_comm_root_proc)
ON_COMMAND(MSG_SELECT, on_nm_album_artist_slected)
ON_COMMAND(MSG_PIC_EVT_DRAW_END,on_network_music_pic_draw_end)
ON_COMMAND(MSG_FOCUS_UP, on_nm_album_artist_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_nm_album_artist_change_focus)
ON_COMMAND(MSG_FOCUS_LEFT, on_nm_album_artist_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_nm_album_artist_change_focus)
ON_COMMAND(MSG_PAGE_UP, on_nm_album_artist_change_page)
ON_COMMAND(MSG_PAGE_DOWN, on_nm_album_artist_change_page)
ON_COMMAND(MSG_RETURN,on_nm_album_artist_return)
ON_COMMAND(MSG_EXIT_ALL, on_network_music_exit_all)
END_MSGPROC(network_music_album_artist_proc, ui_comm_root_proc)

//on cell
BEGIN_KEYMAP(network_music_cell_cont_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(network_music_cell_cont_keymap, NULL)

BEGIN_MSGPROC(network_music_cell_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_SELECT, on_nm_album_artist_cell_selected)
ON_COMMAND(MSG_FOCUS_UP, on_nm_album_artist_cell_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_nm_album_artist_cell_change_focus)
END_MSGPROC(network_music_cell_cont_proc, ui_comm_root_proc)

