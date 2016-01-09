/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Youtube.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2013-10-16 19:11:55
 **
 ** Modification Histroy£º
 **
 ******************************************************************************/
#define SFUNE void (*)(void)
#define SFUNC BOOL (*)(PVOID, MSGID, u32, u32)
#define SFUNA STATEID (*)(PVOID, MSGID, u32, u32)

//State IDs
enum
{
/*001+*/ SID_YOUTUBE = SID_ROOT,
/*002|--+*/ SID_YOUTUBE_INACTIVE,
/*003|--+*/ SID_YOUTUBE_ACTIVE,
/*004|  |--+*/ SID_YOUTUBE_MOST_POPU,
/*005|     |--+*/ SID_YOUTUBE_MP_REQ_OPEN,
/*006|     |--+*/ SID_YOUTUBE_MP_VDOLIST,
/*007|        |--+*/ SID_YOUTUBE_MP_VDOLIST_NORMAL,
/*008|        |--+*/ SID_YOUTUBE_MP_INFO,
/*009|        |--+*/ SID_YOUTUBE_MP_REQUEST,
/*010|        |--+*/ SID_YOUTUBE_MP_REQ_URL,
/*011|  |--+*/ SID_YOUTUBE_CATEGORY,
/*012|     |--+*/ SID_YOUTUBE_CATE_REQ_OPEN,
/*013|     |--+*/ SID_YOUTUBE_CATE_LIST,
/*014|        |--+*/ SID_YOUTUBE_CATE_LIST_NORMAL,
/*015|        |--+*/ SID_YOUTUBE_CATE_REQ_VDO,
/*016|     |--+*/ SID_YOUTUBE_CATE_VDOLIST,
/*017|        |--+*/ SID_YOUTUBE_CATE_VDOLIST_NORMAL,
/*018|        |--+*/ SID_YOUTUBE_CATE_VDO_INFO,
/*019|        |--+*/ SID_YOUTUBE_CATE_VDO_REQ,
/*020|        |--+*/ SID_YOUTUBE_CATE_VDO_REQ_URL,
/*021|  |--+*/ SID_YOUTUBE_SEARCH,
/*022|     |--+*/ SID_YOUTUBE_INPUT,
/*023|     |--+*/ SID_YOUTUBE_SEARCHING,
/*024|     |--+*/ SID_YOUTUBE_SEARCH_VDOLIST,
/*025|        |--+*/ SID_YOUTUBE_SEARCH_VDOLIST_NORMAL,
/*026|        |--+*/ SID_YOUTUBE_SEARCH_INFO,
/*027|        |--+*/ SID_YOUTUBE_SEARCH_REQUEST,
/*028|        |--+*/ SID_YOUTUBE_SEARCH_REQ_URL,
/*029|  |--+*/ SID_YOUTUBE_COUNTRY,
/*030|     |--+*/ SID_YOUTUBE_COUNTRY_OPTLIST,
/*031|  |--+*/ SID_YOUTUBE_RESOLUTION,
/*032|     |--+*/ SID_YOUTUBE_RESOLUTION_OPTLIST,
/*033|  |--+*/ SID_YOUTUBE_TIME_MODE,
/*034|     |--+*/ SID_YOUTUBE_TIME_MODE_OPTLIST,
};

//Prototype of state entry, transition condition and action, and exit
static void SenYoutube_Inactive(void);
static STATEID StaYoutube_on_open_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_Inactive(void);
static void SenYoutube_Active(void);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_on_mainlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_Active(void);
static void SenYoutube_MostPopu(void);
static void SexYoutube_MostPopu(void);
static void SenYoutube_MpReqOpen(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_req_open_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_req_open_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_reqopenfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_MpReqOpen(void);
static void SenYoutube_MpVdolist(void);
static void SexYoutube_MpVdolist(void);
static void SenYoutube_MpVdolistNormal(void);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_MpVdolistNormal(void);
static void SenYoutube_MpInfo(void);
static STATEID StaYoutube_most_popu_on_info_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_MpInfo(void);
static void SenYoutube_MpRequest(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_MpRequest(void);
static void SenYoutube_MpReqUrl(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_get_url_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_geturlfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_MpReqUrl(void);
static void SenYoutube_Category(void);
static void SexYoutube_Category(void);
static void SenYoutube_CateReqOpen(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_req_open_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_req_open_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_reqopenfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateReqOpen(void);
static void SenYoutube_CateList(void);
static void SexYoutube_CateList(void);
static void SenYoutube_CateListNormal(void);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_catelist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateListNormal(void);
static void SenYoutube_CateReqVdo(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_firstpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateReqVdo(void);
static void SenYoutube_CateVdolist(void);
static void SexYoutube_CateVdolist(void);
static void SenYoutube_CateVdolistNormal(void);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateVdolistNormal(void);
static void SenYoutube_CateVdoInfo(void);
static STATEID StaYoutube_most_popu_on_info_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateVdoInfo(void);
static void SenYoutube_CateVdoReq(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_vdolist_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_category_on_vdolist_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateVdoReq(void);
static void SenYoutube_CateVdoReqUrl(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_get_url_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_geturlfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CateVdoReqUrl(void);
static void SenYoutube_Search(void);
static void SexYoutube_Search(void);
static void SenYoutube_Input(void);
static void SexYoutube_Input(void);
static void SenYoutube_Searching(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_search_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_search_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_searchfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_Searching(void);
static void SenYoutube_SearchVdolist(void);
static void SexYoutube_SearchVdolist(void);
static void SenYoutube_SearchVdolistNormal(void);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_SearchVdolistNormal(void);
static void SenYoutube_SearchInfo(void);
static STATEID StaYoutube_most_popu_on_info_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_most_popu_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_infotext_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_SearchInfo(void);
static void SenYoutube_SearchRequest(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_newpageitem_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_get_page_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_search_on_getpagefail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_SearchRequest(void);
static void SenYoutube_SearchReqUrl(void);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_youtube_on_verify_identify_code(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_get_url_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_geturlfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_most_popu_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_SearchReqUrl(void);
static void SenYoutube_Country(void);
static void SexYoutube_Country(void);
static void SenYoutube_CountryOptlist(void);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_CountryOptlist(void);
static void SenYoutube_Resolution(void);
static void SexYoutube_Resolution(void);
static void SenYoutube_ResolutionOptlist(void);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_resolution_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_ResolutionOptlist(void);
static void SenYoutube_TimeMode(void);
static void SexYoutube_TimeMode(void);
static void SenYoutube_TimeModeOptlist(void);
static BOOL    StcYoutube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_country_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcYoutube_is_on_optlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaYoutube_time_mode_on_select(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexYoutube_TimeModeOptlist(void);

//State transition table
static const STATE_TRANS_T SttYoutube_Inactive[] =
{/*002|--+ SID_YOUTUBE_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_REQ, NULL, (SFUNA) StaYoutube_on_open_req},
};

static const STATE_TRANS_T SttYoutube_Active[] =
{/*003|--+ SID_YOUTUBE_ACTIVE */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcYoutube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcYoutube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_on_mainlist_selected},
    {SID_YOUTUBE_INACTIVE, MSG_EXIT, NULL, (SFUNA) StaYoutube_on_active_exit},
    {SID_YOUTUBE_INACTIVE, MSG_EXIT_ALL, NULL, (SFUNA) StaYoutube_on_active_exit},
    {SID_YOUTUBE_INACTIVE, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaYoutube_on_active_exit},
};

static const STATE_TRANS_T SttYoutube_MpReqOpen[] =
{/*005|     |--+ SID_YOUTUBE_MP_REQ_OPEN */
    {SID_INTERNAL, MSG_YT_EVT_VDO_REQ_OPEN_SUCC, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_req_open_succ},
    {SID_INTERNAL, MSG_YT_EVT_VDO_REQ_OPEN_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_req_open_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_most_popu_on_reqopenfail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_MpVdolistNormal[] =
{/*007|        |--+ SID_YOUTUBE_MP_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_updown},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_YOUTUBE_MP_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_vdolist_selected},
    {SID_YOUTUBE_MP_INFO, MSG_INFO, NULL, (SFUNA) StaYoutube_most_popu_on_vdolist_info_pressed},
};

static const STATE_TRANS_T SttYoutube_MpInfo[] =
{/*008|        |--+ SID_YOUTUBE_MP_INFO */
    {SID_YOUTUBE_MP_VDOLIST_NORMAL, MSG_BACK, NULL, (SFUNA) StaYoutube_most_popu_on_info_exit},
    {SID_YOUTUBE_MP_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_most_popu_on_infotext_selected, (SFUNA) StaYoutube_most_popu_on_infotext_selected},
};

static const STATE_TRANS_T SttYoutube_MpRequest[] =
{/*009|        |--+ SID_YOUTUBE_MP_REQUEST */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_newpageitem_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_PAGE_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_get_page_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_most_popu_on_getpagefail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_MpReqUrl[] =
{/*010|        |--+ SID_YOUTUBE_MP_REQ_URL */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PLAY_URL_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_URL_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_get_url_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_most_popu_on_geturlfail_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaYoutube_most_popu_on_play_exit},
};

static const STATE_TRANS_T SttYoutube_CateReqOpen[] =
{/*012|     |--+ SID_YOUTUBE_CATE_REQ_OPEN */
    {SID_INTERNAL, MSG_YT_EVT_CATE_REQ_OPEN_SUCC, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_req_open_succ},
    {SID_INTERNAL, MSG_YT_EVT_CATE_REQ_OPEN_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_req_open_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_category_on_reqopenfail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_CateListNormal[] =
{/*014|        |--+ SID_YOUTUBE_CATE_LIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_category_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_catelist, (SFUNA) StaYoutube_category_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_catelist, (SFUNA) StaYoutube_category_on_change_focus},
    {SID_INTERNAL, MSG_SELECT, NULL, (SFUNA) StaYoutube_category_on_catelist_selected},
};

static const STATE_TRANS_T SttYoutube_CateReqVdo[] =
{/*015|        |--+ SID_YOUTUBE_CATE_REQ_VDO */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_firstpageitem_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_PAGE_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_get_page_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_category_on_getpagefail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_CateVdolistNormal[] =
{/*017|        |--+ SID_YOUTUBE_CATE_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_category_on_vdolist_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_category_on_vdolist_updown},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_YOUTUBE_CATE_LIST, MSG_BACK, (SFUNC) StcYoutube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_YOUTUBE_CATE_VDO_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_category_on_vdolist_selected},
    {SID_YOUTUBE_CATE_VDO_INFO, MSG_INFO, NULL, (SFUNA) StaYoutube_most_popu_on_vdolist_info_pressed},
};

static const STATE_TRANS_T SttYoutube_CateVdoInfo[] =
{/*018|        |--+ SID_YOUTUBE_CATE_VDO_INFO */
    {SID_YOUTUBE_CATE_VDOLIST_NORMAL, MSG_BACK, NULL, (SFUNA) StaYoutube_most_popu_on_info_exit},
    {SID_YOUTUBE_CATE_VDO_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_most_popu_on_infotext_selected, (SFUNA) StaYoutube_category_on_infotext_selected},
};

static const STATE_TRANS_T SttYoutube_CateVdoReq[] =
{/*019|        |--+ SID_YOUTUBE_CATE_VDO_REQ */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_newpageitem_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_PAGE_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_category_on_vdolist_get_page_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_category_on_vdolist_getpagefail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_CateVdoReqUrl[] =
{/*020|        |--+ SID_YOUTUBE_CATE_VDO_REQ_URL */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PLAY_URL_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_URL_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_get_url_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_most_popu_on_geturlfail_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaYoutube_most_popu_on_play_exit},
};

static const STATE_TRANS_T SttYoutube_Input[] =
{/*022|     |--+ SID_YOUTUBE_INPUT */
    {SID_YOUTUBE_SEARCHING, MSG_INPUT_COMP, NULL, NULL},
};

static const STATE_TRANS_T SttYoutube_Searching[] =
{/*023|     |--+ SID_YOUTUBE_SEARCHING */
    {SID_INTERNAL, MSG_YT_EVT_SEARCH_SUCC, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_search_on_search_succ},
    {SID_INTERNAL, MSG_YT_EVT_SEARCH_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_search_on_search_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_search_on_searchfail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_SearchVdolistNormal[] =
{/*025|        |--+ SID_YOUTUBE_SEARCH_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_search_on_vdolist_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_search_on_vdolist_updown},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaYoutube_most_popu_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_most_popu_on_change_focus},
    {SID_YOUTUBE_SEARCH_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_is_on_vdolist, (SFUNA) StaYoutube_search_on_vdolist_selected},
    {SID_YOUTUBE_SEARCH_INFO, MSG_INFO, NULL, (SFUNA) StaYoutube_most_popu_on_vdolist_info_pressed},
};

static const STATE_TRANS_T SttYoutube_SearchInfo[] =
{/*026|        |--+ SID_YOUTUBE_SEARCH_INFO */
    {SID_YOUTUBE_SEARCH_VDOLIST_NORMAL, MSG_BACK, NULL, (SFUNA) StaYoutube_most_popu_on_info_exit},
    {SID_YOUTUBE_SEARCH_REQ_URL, MSG_SELECT, (SFUNC) StcYoutube_most_popu_on_infotext_selected, (SFUNA) StaYoutube_search_on_infotext_selected},
};

static const STATE_TRANS_T SttYoutube_SearchRequest[] =
{/*027|        |--+ SID_YOUTUBE_SEARCH_REQUEST */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PAGE_ITEM_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_search_on_newpageitem_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_PAGE_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_search_on_get_page_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_search_on_getpagefail_dlg_closed},
};

static const STATE_TRANS_T SttYoutube_SearchReqUrl[] =
{/*028|        |--+ SID_YOUTUBE_SEARCH_REQ_URL */
    {SID_INTERNAL, MSG_YT_EVT_NEW_PLAY_URL_ARRIVE, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_YT_EVT_GET_URL_FAIL, (SFUNC) StcYoutube_youtube_on_verify_identify_code, (SFUNA) StaYoutube_most_popu_on_get_url_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaYoutube_most_popu_on_geturlfail_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaYoutube_most_popu_on_play_exit},
};

static const STATE_TRANS_T SttYoutube_CountryOptlist[] =
{/*030|     |--+ SID_YOUTUBE_COUNTRY_OPTLIST */
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_select},
};

static const STATE_TRANS_T SttYoutube_ResolutionOptlist[] =
{/*032|     |--+ SID_YOUTUBE_RESOLUTION_OPTLIST */
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_resolution_on_select},
};

static const STATE_TRANS_T SttYoutube_TimeModeOptlist[] =
{/*034|     |--+ SID_YOUTUBE_TIME_MODE_OPTLIST */
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcYoutube_is_on_mainlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_country_on_change_focus},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcYoutube_is_on_optlist, (SFUNA) StaYoutube_time_mode_on_select},
};

//State tree
static const STATE_NODE_T pStateTree_Youtube[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_YOUTUBE */ {SID_NULL, SID_YOUTUBE_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_YOUTUBE_INACTIVE */ {SID_YOUTUBE, SID_NULL, (SFUNE) SenYoutube_Inactive, (SFUNE) SexYoutube_Inactive, SttYoutube_Inactive, sizeof(SttYoutube_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_YOUTUBE_ACTIVE */ {SID_YOUTUBE, SID_YOUTUBE_MOST_POPU, (SFUNE) SenYoutube_Active, (SFUNE) SexYoutube_Active, SttYoutube_Active, sizeof(SttYoutube_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_YOUTUBE_MOST_POPU */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_MP_REQ_OPEN, (SFUNE) SenYoutube_MostPopu, (SFUNE) SexYoutube_MostPopu, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*005|     |--+ SID_YOUTUBE_MP_REQ_OPEN */ {SID_YOUTUBE_MOST_POPU, SID_NULL, (SFUNE) SenYoutube_MpReqOpen, (SFUNE) SexYoutube_MpReqOpen, SttYoutube_MpReqOpen, sizeof(SttYoutube_MpReqOpen)/sizeof(STATE_TRANS_T)},
/*006|     |--+ SID_YOUTUBE_MP_VDOLIST */ {SID_YOUTUBE_MOST_POPU, SID_YOUTUBE_MP_VDOLIST_NORMAL, (SFUNE) SenYoutube_MpVdolist, (SFUNE) SexYoutube_MpVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*007|        |--+ SID_YOUTUBE_MP_VDOLIST_NORMAL */ {SID_YOUTUBE_MP_VDOLIST, SID_NULL, (SFUNE) SenYoutube_MpVdolistNormal, (SFUNE) SexYoutube_MpVdolistNormal, SttYoutube_MpVdolistNormal, sizeof(SttYoutube_MpVdolistNormal)/sizeof(STATE_TRANS_T)},
/*008|        |--+ SID_YOUTUBE_MP_INFO */ {SID_YOUTUBE_MP_VDOLIST, SID_NULL, (SFUNE) SenYoutube_MpInfo, (SFUNE) SexYoutube_MpInfo, SttYoutube_MpInfo, sizeof(SttYoutube_MpInfo)/sizeof(STATE_TRANS_T)},
/*009|        |--+ SID_YOUTUBE_MP_REQUEST */ {SID_YOUTUBE_MP_VDOLIST, SID_NULL, (SFUNE) SenYoutube_MpRequest, (SFUNE) SexYoutube_MpRequest, SttYoutube_MpRequest, sizeof(SttYoutube_MpRequest)/sizeof(STATE_TRANS_T)},
/*010|        |--+ SID_YOUTUBE_MP_REQ_URL */ {SID_YOUTUBE_MP_VDOLIST, SID_NULL, (SFUNE) SenYoutube_MpReqUrl, (SFUNE) SexYoutube_MpReqUrl, SttYoutube_MpReqUrl, sizeof(SttYoutube_MpReqUrl)/sizeof(STATE_TRANS_T)},
/*011|  |--+ SID_YOUTUBE_CATEGORY */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_CATE_REQ_OPEN, (SFUNE) SenYoutube_Category, (SFUNE) SexYoutube_Category, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*012|     |--+ SID_YOUTUBE_CATE_REQ_OPEN */ {SID_YOUTUBE_CATEGORY, SID_NULL, (SFUNE) SenYoutube_CateReqOpen, (SFUNE) SexYoutube_CateReqOpen, SttYoutube_CateReqOpen, sizeof(SttYoutube_CateReqOpen)/sizeof(STATE_TRANS_T)},
/*013|     |--+ SID_YOUTUBE_CATE_LIST */ {SID_YOUTUBE_CATEGORY, SID_YOUTUBE_CATE_LIST_NORMAL, (SFUNE) SenYoutube_CateList, (SFUNE) SexYoutube_CateList, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*014|        |--+ SID_YOUTUBE_CATE_LIST_NORMAL */ {SID_YOUTUBE_CATE_LIST, SID_NULL, (SFUNE) SenYoutube_CateListNormal, (SFUNE) SexYoutube_CateListNormal, SttYoutube_CateListNormal, sizeof(SttYoutube_CateListNormal)/sizeof(STATE_TRANS_T)},
/*015|        |--+ SID_YOUTUBE_CATE_REQ_VDO */ {SID_YOUTUBE_CATE_LIST, SID_NULL, (SFUNE) SenYoutube_CateReqVdo, (SFUNE) SexYoutube_CateReqVdo, SttYoutube_CateReqVdo, sizeof(SttYoutube_CateReqVdo)/sizeof(STATE_TRANS_T)},
/*016|     |--+ SID_YOUTUBE_CATE_VDOLIST */ {SID_YOUTUBE_CATEGORY, SID_YOUTUBE_CATE_VDOLIST_NORMAL, (SFUNE) SenYoutube_CateVdolist, (SFUNE) SexYoutube_CateVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*017|        |--+ SID_YOUTUBE_CATE_VDOLIST_NORMAL */ {SID_YOUTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenYoutube_CateVdolistNormal, (SFUNE) SexYoutube_CateVdolistNormal, SttYoutube_CateVdolistNormal, sizeof(SttYoutube_CateVdolistNormal)/sizeof(STATE_TRANS_T)},
/*018|        |--+ SID_YOUTUBE_CATE_VDO_INFO */ {SID_YOUTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenYoutube_CateVdoInfo, (SFUNE) SexYoutube_CateVdoInfo, SttYoutube_CateVdoInfo, sizeof(SttYoutube_CateVdoInfo)/sizeof(STATE_TRANS_T)},
/*019|        |--+ SID_YOUTUBE_CATE_VDO_REQ */ {SID_YOUTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenYoutube_CateVdoReq, (SFUNE) SexYoutube_CateVdoReq, SttYoutube_CateVdoReq, sizeof(SttYoutube_CateVdoReq)/sizeof(STATE_TRANS_T)},
/*020|        |--+ SID_YOUTUBE_CATE_VDO_REQ_URL */ {SID_YOUTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenYoutube_CateVdoReqUrl, (SFUNE) SexYoutube_CateVdoReqUrl, SttYoutube_CateVdoReqUrl, sizeof(SttYoutube_CateVdoReqUrl)/sizeof(STATE_TRANS_T)},
/*021|  |--+ SID_YOUTUBE_SEARCH */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_INPUT, (SFUNE) SenYoutube_Search, (SFUNE) SexYoutube_Search, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*022|     |--+ SID_YOUTUBE_INPUT */ {SID_YOUTUBE_SEARCH, SID_NULL, (SFUNE) SenYoutube_Input, (SFUNE) SexYoutube_Input, SttYoutube_Input, sizeof(SttYoutube_Input)/sizeof(STATE_TRANS_T)},
/*023|     |--+ SID_YOUTUBE_SEARCHING */ {SID_YOUTUBE_SEARCH, SID_NULL, (SFUNE) SenYoutube_Searching, (SFUNE) SexYoutube_Searching, SttYoutube_Searching, sizeof(SttYoutube_Searching)/sizeof(STATE_TRANS_T)},
/*024|     |--+ SID_YOUTUBE_SEARCH_VDOLIST */ {SID_YOUTUBE_SEARCH, SID_YOUTUBE_SEARCH_VDOLIST_NORMAL, (SFUNE) SenYoutube_SearchVdolist, (SFUNE) SexYoutube_SearchVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*025|        |--+ SID_YOUTUBE_SEARCH_VDOLIST_NORMAL */ {SID_YOUTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenYoutube_SearchVdolistNormal, (SFUNE) SexYoutube_SearchVdolistNormal, SttYoutube_SearchVdolistNormal, sizeof(SttYoutube_SearchVdolistNormal)/sizeof(STATE_TRANS_T)},
/*026|        |--+ SID_YOUTUBE_SEARCH_INFO */ {SID_YOUTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenYoutube_SearchInfo, (SFUNE) SexYoutube_SearchInfo, SttYoutube_SearchInfo, sizeof(SttYoutube_SearchInfo)/sizeof(STATE_TRANS_T)},
/*027|        |--+ SID_YOUTUBE_SEARCH_REQUEST */ {SID_YOUTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenYoutube_SearchRequest, (SFUNE) SexYoutube_SearchRequest, SttYoutube_SearchRequest, sizeof(SttYoutube_SearchRequest)/sizeof(STATE_TRANS_T)},
/*028|        |--+ SID_YOUTUBE_SEARCH_REQ_URL */ {SID_YOUTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenYoutube_SearchReqUrl, (SFUNE) SexYoutube_SearchReqUrl, SttYoutube_SearchReqUrl, sizeof(SttYoutube_SearchReqUrl)/sizeof(STATE_TRANS_T)},
/*029|  |--+ SID_YOUTUBE_COUNTRY */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_COUNTRY_OPTLIST, (SFUNE) SenYoutube_Country, (SFUNE) SexYoutube_Country, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*030|     |--+ SID_YOUTUBE_COUNTRY_OPTLIST */ {SID_YOUTUBE_COUNTRY, SID_NULL, (SFUNE) SenYoutube_CountryOptlist, (SFUNE) SexYoutube_CountryOptlist, SttYoutube_CountryOptlist, sizeof(SttYoutube_CountryOptlist)/sizeof(STATE_TRANS_T)},
/*031|  |--+ SID_YOUTUBE_RESOLUTION */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_RESOLUTION_OPTLIST, (SFUNE) SenYoutube_Resolution, (SFUNE) SexYoutube_Resolution, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*032|     |--+ SID_YOUTUBE_RESOLUTION_OPTLIST */ {SID_YOUTUBE_RESOLUTION, SID_NULL, (SFUNE) SenYoutube_ResolutionOptlist, (SFUNE) SexYoutube_ResolutionOptlist, SttYoutube_ResolutionOptlist, sizeof(SttYoutube_ResolutionOptlist)/sizeof(STATE_TRANS_T)},
/*033|  |--+ SID_YOUTUBE_TIME_MODE */ {SID_YOUTUBE_ACTIVE, SID_YOUTUBE_TIME_MODE_OPTLIST, (SFUNE) SenYoutube_TimeMode, (SFUNE) SexYoutube_TimeMode, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*034|     |--+ SID_YOUTUBE_TIME_MODE_OPTLIST */ {SID_YOUTUBE_TIME_MODE, SID_NULL, (SFUNE) SenYoutube_TimeModeOptlist, (SFUNE) SexYoutube_TimeModeOptlist, SttYoutube_TimeModeOptlist, sizeof(SttYoutube_TimeModeOptlist)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Youtube = {
    pStateTree_Youtube,
    "APP_YOUTUBE_T",
    sizeof(pStateTree_Youtube)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Youtube_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Youtube));
}

s32 Youtube_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Youtube));
}

BOOL Youtube_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Youtube), stateID);
}

s32 Youtube_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Youtube), ctrl, msg, para1, para2);
}
