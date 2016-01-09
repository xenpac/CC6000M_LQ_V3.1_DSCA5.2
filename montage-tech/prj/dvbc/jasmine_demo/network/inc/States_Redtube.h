/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Redtube.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2013-11-7 10:14:44
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
/*001+*/ SID_REDTUBE = SID_ROOT,
/*002|--+*/ SID_REDTUBE_INACTIVE,
/*003|--+*/ SID_REDTUBE_ACTIVE,
/*004|  |--+*/ SID_REDTUBE_DEFAULT,
/*005|     |--+*/ SID_REDTUBE_DEF_VDOLIST,
/*006|        |--+*/ SID_REDTUBE_DEF_VDO_REQ,
/*007|        |--+*/ SID_REDTUBE_DEF_VDOLIST_NORMAL,
/*008|        |--+*/ SID_REDTUBE_DEF_VDOLIST_URL_REQ,
/*009|  |--+*/ SID_REDTUBE_CATEGORY,
/*010|     |--+*/ SID_REDTUBE_CATE_LIST,
/*011|        |--+*/ SID_REDTUBE_CATE_LIST_REQ,
/*012|        |--+*/ SID_REDTUBE_CATE_LIST_NORMAL,
/*013|        |--+*/ SID_REDTUBE_CATE_1ST_VDO_REQ,
/*014|     |--+*/ SID_REDTUBE_CATE_VDOLIST,
/*015|        |--+*/ SID_REDTUBE_CATE_VDOLIST_NORMAL,
/*016|        |--+*/ SID_REDTUBE_CATE_VDO_REQ,
/*017|        |--+*/ SID_REDTUBE_CATE_VDOLIST_URL_REQ,
/*018|  |--+*/ SID_REDTUBE_SEARCH,
/*019|     |--+*/ SID_REDTUBE_SEARCH_VDOLIST,
/*020|        |--+*/ SID_REDTUBE_INPUT,
/*021|        |--+*/ SID_REDTUBE_SEARCHING,
/*022|        |--+*/ SID_REDTUBE_SEARCH_VDOLIST_NORMAL,
/*023|        |--+*/ SID_REDTUBE_SEARCH_VDO_REQ,
/*024|        |--+*/ SID_REDTUBE_SEARCH_VDOLIST_URL_REQ,
};

//Prototype of state entry, transition condition and action, and exit
static void SenRedtube_Inactive(void);
static STATEID StaRedtube_on_open_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_Inactive(void);
static void SenRedtube_Active(void);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_on_mainlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_on_active_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_Active(void);
static void SenRedtube_Default(void);
static void SexRedtube_Default(void);
static void SenRedtube_DefVdolist(void);
static void SexRedtube_DefVdolist(void);
static void SenRedtube_DefVdoReq(void);
static STATEID StaRedtube_default_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_DefVdoReq(void);
static void SenRedtube_DefVdolistNormal(void);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_DefVdolistNormal(void);
static void SenRedtube_DefVdolistUrlReq(void);
static STATEID StaRedtube_default_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_get_playurl_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_urlreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_DefVdolistUrlReq(void);
static void SenRedtube_Category(void);
static void SexRedtube_Category(void);
static void SenRedtube_CateList(void);
static void SexRedtube_CateList(void);
static void SenRedtube_CateListReq(void);
static STATEID StaRedtube_category_on_newcatelist_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_get_catelist_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_catereq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_CateListReq(void);
static void SenRedtube_CateListNormal(void);
static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_catelist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_catelist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_CateListNormal(void);
static void SenRedtube_Cate1stVdoReq(void);
static STATEID StaRedtube_category_on_1stpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_get_1stpagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_1stvdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_Cate1stVdoReq(void);
static void SenRedtube_CateVdolist(void);
static void SexRedtube_CateVdolist(void);
static void SenRedtube_CateVdolistNormal(void);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_CateVdolistNormal(void);
static void SenRedtube_CateVdoReq(void);
static STATEID StaRedtube_category_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_category_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_CateVdoReq(void);
static void SenRedtube_CateVdolistUrlReq(void);
static STATEID StaRedtube_default_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_get_playurl_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_urlreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_CateVdolistUrlReq(void);
static void SenRedtube_Search(void);
static void SexRedtube_Search(void);
static void SenRedtube_SearchVdolist(void);
static void SexRedtube_SearchVdolist(void);
static void SenRedtube_Input(void);
static void SexRedtube_Input(void);
static void SenRedtube_Searching(void);
static STATEID StaRedtube_search_on_search_succ(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_search_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_searchfail_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_Searching(void);
static void SenRedtube_SearchVdolistNormal(void);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdolist_updown(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdolist_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_mainlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_change_focus(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcRedtube_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_SearchVdolistNormal(void);
static void SenRedtube_SearchVdoReq(void);
static STATEID StaRedtube_search_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_search_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_SearchVdoReq(void);
static void SenRedtube_SearchVdolistUrlReq(void);
static STATEID StaRedtube_default_on_newplayurl_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_get_playurl_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_urlreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaRedtube_default_on_play_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexRedtube_SearchVdolistUrlReq(void);

//State transition table
static const STATE_TRANS_T SttRedtube_Inactive[] =
{/*002|--+ SID_REDTUBE_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_REQ, NULL, (SFUNA) StaRedtube_on_open_req},
};

static const STATE_TRANS_T SttRedtube_Active[] =
{/*003|--+ SID_REDTUBE_ACTIVE */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcRedtube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcRedtube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcRedtube_is_on_mainlist, (SFUNA) StaRedtube_on_mainlist_selected},
    {SID_REDTUBE_INACTIVE, MSG_EXIT, NULL, (SFUNA) StaRedtube_on_active_exit},
    {SID_REDTUBE_INACTIVE, MSG_EXIT_ALL, NULL, (SFUNA) StaRedtube_on_active_exit},
    {SID_REDTUBE_INACTIVE, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaRedtube_on_active_exit},
};

static const STATE_TRANS_T SttRedtube_DefVdoReq[] =
{/*006|        |--+ SID_REDTUBE_DEF_VDO_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaRedtube_default_on_newpagevdo_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaRedtube_default_on_get_pagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_default_on_vdoreq_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_DefVdolistNormal[] =
{/*007|        |--+ SID_REDTUBE_DEF_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_updown},
    {SID_INTERNAL, MSG_PAGE_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_page},
    {SID_INTERNAL, MSG_PAGE_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_page},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaRedtube_default_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcRedtube_is_on_mainlist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_REDTUBE_DEF_VDOLIST_URL_REQ, MSG_SELECT, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_vdolist_selected},
};

static const STATE_TRANS_T SttRedtube_DefVdolistUrlReq[] =
{/*008|        |--+ SID_REDTUBE_DEF_VDOLIST_URL_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PLAY_URL_ARRIVE, NULL, (SFUNA) StaRedtube_default_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PLAY_URL_FAIL, NULL, (SFUNA) StaRedtube_default_on_get_playurl_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_default_on_urlreq_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaRedtube_default_on_play_exit},
};

static const STATE_TRANS_T SttRedtube_CateListReq[] =
{/*011|        |--+ SID_REDTUBE_CATE_LIST_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_CATE_LIST_ARRIVE, NULL, (SFUNA) StaRedtube_category_on_newcatelist_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_CATE_LIST_FAIL, NULL, (SFUNA) StaRedtube_category_on_get_catelist_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_category_on_catereq_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_CateListNormal[] =
{/*012|        |--+ SID_REDTUBE_CATE_LIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcRedtube_is_on_mainlist, (SFUNA) StaRedtube_category_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcRedtube_is_on_catelist, (SFUNA) StaRedtube_category_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcRedtube_is_on_catelist, (SFUNA) StaRedtube_category_on_change_focus},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcRedtube_is_on_catelist, (SFUNA) StaRedtube_category_on_catelist_selected},
};

static const STATE_TRANS_T SttRedtube_Cate1stVdoReq[] =
{/*013|        |--+ SID_REDTUBE_CATE_1ST_VDO_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaRedtube_category_on_1stpagevdo_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaRedtube_category_on_get_1stpagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_category_on_1stvdoreq_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_CateVdolistNormal[] =
{/*015|        |--+ SID_REDTUBE_CATE_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_category_on_vdolist_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_category_on_vdolist_updown},
    {SID_INTERNAL, MSG_PAGE_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_category_on_vdolist_change_page},
    {SID_INTERNAL, MSG_PAGE_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_category_on_vdolist_change_page},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaRedtube_default_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcRedtube_is_on_mainlist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_REDTUBE_CATE_LIST_NORMAL, MSG_BACK, (SFUNC) StcRedtube_is_on_vdolist, NULL},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_REDTUBE_CATE_VDOLIST_URL_REQ, MSG_SELECT, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_category_on_vdolist_selected},
};

static const STATE_TRANS_T SttRedtube_CateVdoReq[] =
{/*016|        |--+ SID_REDTUBE_CATE_VDO_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaRedtube_category_on_newpagevdo_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaRedtube_category_on_get_pagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_category_on_vdoreq_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_CateVdolistUrlReq[] =
{/*017|        |--+ SID_REDTUBE_CATE_VDOLIST_URL_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PLAY_URL_ARRIVE, NULL, (SFUNA) StaRedtube_default_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PLAY_URL_FAIL, NULL, (SFUNA) StaRedtube_default_on_get_playurl_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_default_on_urlreq_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaRedtube_default_on_play_exit},
};

static const STATE_TRANS_T SttRedtube_Input[] =
{/*020|        |--+ SID_REDTUBE_INPUT */
    {SID_REDTUBE_SEARCHING, MSG_INPUT_COMP, NULL, NULL},
};

static const STATE_TRANS_T SttRedtube_Searching[] =
{/*021|        |--+ SID_REDTUBE_SEARCHING */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaRedtube_search_on_search_succ},
    {SID_INTERNAL, MSG_RT_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaRedtube_search_on_search_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_search_on_searchfail_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_SearchVdolistNormal[] =
{/*022|        |--+ SID_REDTUBE_SEARCH_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_FOCUS_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_search_on_vdolist_updown},
    {SID_INTERNAL, MSG_FOCUS_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_search_on_vdolist_updown},
    {SID_INTERNAL, MSG_PAGE_UP, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_search_on_vdolist_change_page},
    {SID_INTERNAL, MSG_PAGE_DOWN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_search_on_vdolist_change_page},
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaRedtube_default_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaRedtube_default_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, (SFUNC) StcRedtube_is_on_mainlist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_INTERNAL, MSG_BACK, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_INTERNAL, MSG_MAIN, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_default_on_change_focus},
    {SID_REDTUBE_SEARCH_VDOLIST_URL_REQ, MSG_SELECT, (SFUNC) StcRedtube_is_on_vdolist, (SFUNA) StaRedtube_search_on_vdolist_selected},
};

static const STATE_TRANS_T SttRedtube_SearchVdoReq[] =
{/*023|        |--+ SID_REDTUBE_SEARCH_VDO_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaRedtube_search_on_newpagevdo_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaRedtube_search_on_get_pagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_search_on_vdoreq_dlg_closed},
};

static const STATE_TRANS_T SttRedtube_SearchVdolistUrlReq[] =
{/*024|        |--+ SID_REDTUBE_SEARCH_VDOLIST_URL_REQ */
    {SID_INTERNAL, MSG_RT_EVT_NEW_PLAY_URL_ARRIVE, NULL, (SFUNA) StaRedtube_default_on_newplayurl_arrive},
    {SID_INTERNAL, MSG_RT_EVT_GET_PLAY_URL_FAIL, NULL, (SFUNA) StaRedtube_default_on_get_playurl_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaRedtube_default_on_urlreq_dlg_closed},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaRedtube_default_on_play_exit},
};

//State tree
static const STATE_NODE_T pStateTree_Redtube[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_REDTUBE */ {SID_NULL, SID_REDTUBE_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_REDTUBE_INACTIVE */ {SID_REDTUBE, SID_NULL, (SFUNE) SenRedtube_Inactive, (SFUNE) SexRedtube_Inactive, SttRedtube_Inactive, sizeof(SttRedtube_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_REDTUBE_ACTIVE */ {SID_REDTUBE, SID_REDTUBE_DEFAULT, (SFUNE) SenRedtube_Active, (SFUNE) SexRedtube_Active, SttRedtube_Active, sizeof(SttRedtube_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_REDTUBE_DEFAULT */ {SID_REDTUBE_ACTIVE, SID_REDTUBE_DEF_VDOLIST, (SFUNE) SenRedtube_Default, (SFUNE) SexRedtube_Default, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*005|     |--+ SID_REDTUBE_DEF_VDOLIST */ {SID_REDTUBE_DEFAULT, SID_REDTUBE_DEF_VDO_REQ, (SFUNE) SenRedtube_DefVdolist, (SFUNE) SexRedtube_DefVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*006|        |--+ SID_REDTUBE_DEF_VDO_REQ */ {SID_REDTUBE_DEF_VDOLIST, SID_NULL, (SFUNE) SenRedtube_DefVdoReq, (SFUNE) SexRedtube_DefVdoReq, SttRedtube_DefVdoReq, sizeof(SttRedtube_DefVdoReq)/sizeof(STATE_TRANS_T)},
/*007|        |--+ SID_REDTUBE_DEF_VDOLIST_NORMAL */ {SID_REDTUBE_DEF_VDOLIST, SID_NULL, (SFUNE) SenRedtube_DefVdolistNormal, (SFUNE) SexRedtube_DefVdolistNormal, SttRedtube_DefVdolistNormal, sizeof(SttRedtube_DefVdolistNormal)/sizeof(STATE_TRANS_T)},
/*008|        |--+ SID_REDTUBE_DEF_VDOLIST_URL_REQ */ {SID_REDTUBE_DEF_VDOLIST, SID_NULL, (SFUNE) SenRedtube_DefVdolistUrlReq, (SFUNE) SexRedtube_DefVdolistUrlReq, SttRedtube_DefVdolistUrlReq, sizeof(SttRedtube_DefVdolistUrlReq)/sizeof(STATE_TRANS_T)},
/*009|  |--+ SID_REDTUBE_CATEGORY */ {SID_REDTUBE_ACTIVE, SID_REDTUBE_CATE_LIST, (SFUNE) SenRedtube_Category, (SFUNE) SexRedtube_Category, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*010|     |--+ SID_REDTUBE_CATE_LIST */ {SID_REDTUBE_CATEGORY, SID_REDTUBE_CATE_LIST_REQ, (SFUNE) SenRedtube_CateList, (SFUNE) SexRedtube_CateList, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*011|        |--+ SID_REDTUBE_CATE_LIST_REQ */ {SID_REDTUBE_CATE_LIST, SID_NULL, (SFUNE) SenRedtube_CateListReq, (SFUNE) SexRedtube_CateListReq, SttRedtube_CateListReq, sizeof(SttRedtube_CateListReq)/sizeof(STATE_TRANS_T)},
/*012|        |--+ SID_REDTUBE_CATE_LIST_NORMAL */ {SID_REDTUBE_CATE_LIST, SID_NULL, (SFUNE) SenRedtube_CateListNormal, (SFUNE) SexRedtube_CateListNormal, SttRedtube_CateListNormal, sizeof(SttRedtube_CateListNormal)/sizeof(STATE_TRANS_T)},
/*013|        |--+ SID_REDTUBE_CATE_1ST_VDO_REQ */ {SID_REDTUBE_CATE_LIST, SID_NULL, (SFUNE) SenRedtube_Cate1stVdoReq, (SFUNE) SexRedtube_Cate1stVdoReq, SttRedtube_Cate1stVdoReq, sizeof(SttRedtube_Cate1stVdoReq)/sizeof(STATE_TRANS_T)},
/*014|     |--+ SID_REDTUBE_CATE_VDOLIST */ {SID_REDTUBE_CATEGORY, SID_REDTUBE_CATE_VDOLIST_NORMAL, (SFUNE) SenRedtube_CateVdolist, (SFUNE) SexRedtube_CateVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*015|        |--+ SID_REDTUBE_CATE_VDOLIST_NORMAL */ {SID_REDTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenRedtube_CateVdolistNormal, (SFUNE) SexRedtube_CateVdolistNormal, SttRedtube_CateVdolistNormal, sizeof(SttRedtube_CateVdolistNormal)/sizeof(STATE_TRANS_T)},
/*016|        |--+ SID_REDTUBE_CATE_VDO_REQ */ {SID_REDTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenRedtube_CateVdoReq, (SFUNE) SexRedtube_CateVdoReq, SttRedtube_CateVdoReq, sizeof(SttRedtube_CateVdoReq)/sizeof(STATE_TRANS_T)},
/*017|        |--+ SID_REDTUBE_CATE_VDOLIST_URL_REQ */ {SID_REDTUBE_CATE_VDOLIST, SID_NULL, (SFUNE) SenRedtube_CateVdolistUrlReq, (SFUNE) SexRedtube_CateVdolistUrlReq, SttRedtube_CateVdolistUrlReq, sizeof(SttRedtube_CateVdolistUrlReq)/sizeof(STATE_TRANS_T)},
/*018|  |--+ SID_REDTUBE_SEARCH */ {SID_REDTUBE_ACTIVE, SID_REDTUBE_SEARCH_VDOLIST, (SFUNE) SenRedtube_Search, (SFUNE) SexRedtube_Search, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*019|     |--+ SID_REDTUBE_SEARCH_VDOLIST */ {SID_REDTUBE_SEARCH, SID_REDTUBE_INPUT, (SFUNE) SenRedtube_SearchVdolist, (SFUNE) SexRedtube_SearchVdolist, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*020|        |--+ SID_REDTUBE_INPUT */ {SID_REDTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenRedtube_Input, (SFUNE) SexRedtube_Input, SttRedtube_Input, sizeof(SttRedtube_Input)/sizeof(STATE_TRANS_T)},
/*021|        |--+ SID_REDTUBE_SEARCHING */ {SID_REDTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenRedtube_Searching, (SFUNE) SexRedtube_Searching, SttRedtube_Searching, sizeof(SttRedtube_Searching)/sizeof(STATE_TRANS_T)},
/*022|        |--+ SID_REDTUBE_SEARCH_VDOLIST_NORMAL */ {SID_REDTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenRedtube_SearchVdolistNormal, (SFUNE) SexRedtube_SearchVdolistNormal, SttRedtube_SearchVdolistNormal, sizeof(SttRedtube_SearchVdolistNormal)/sizeof(STATE_TRANS_T)},
/*023|        |--+ SID_REDTUBE_SEARCH_VDO_REQ */ {SID_REDTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenRedtube_SearchVdoReq, (SFUNE) SexRedtube_SearchVdoReq, SttRedtube_SearchVdoReq, sizeof(SttRedtube_SearchVdoReq)/sizeof(STATE_TRANS_T)},
/*024|        |--+ SID_REDTUBE_SEARCH_VDOLIST_URL_REQ */ {SID_REDTUBE_SEARCH_VDOLIST, SID_NULL, (SFUNE) SenRedtube_SearchVdolistUrlReq, (SFUNE) SexRedtube_SearchVdolistUrlReq, SttRedtube_SearchVdolistUrlReq, sizeof(SttRedtube_SearchVdolistUrlReq)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Redtube = {
    pStateTree_Redtube,
    "APP_REDTUBE_T",
    sizeof(pStateTree_Redtube)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Redtube_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Redtube));
}

s32 Redtube_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Redtube));
}

BOOL Redtube_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Redtube), stateID);
}

s32 Redtube_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Redtube), ctrl, msg, para1, para2);
}
