/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_IptvSearch.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2013-12-23 16:53:14
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
/*001+*/ SID_IPTV_SEARCH = SID_ROOT,
/*002|--+*/ SID_IPTV_SEARCH_INACTIVE,
/*003|--+*/ SID_IPTV_SEARCH_ACTIVE,
/*004|  |--+*/ SID_IPTV_SEARCH_SEARCH,
/*005|     |--+*/ SID_IPTV_SEARCH_SEARCH_NORMAL,
/*006|     |--+*/ SID_IPTV_SEARCH_SEARCH_REQ,
};

//Prototype of state entry, transition condition and action, and exit
static void SenIptvSearch_Inactive(void);
static STATEID StaIptvSearch_inactive_on_open_iptv_search_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptvSearch_Inactive(void);
static void SenIptvSearch_Active(void);
static STATEID StaIptvSearch_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptvSearch_Active(void);
static void SenIptvSearch_Search(void);
static STATEID StaIptvSearch_search_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_matrixkey(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_matrixkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_matrixkey(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_matrixkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_customkey(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_customkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptvSearch_Search(void);
static void SenIptvSearch_SearchNormal(void);
static BOOL    StcIptvSearch_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_searchkey(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_searchkey_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptvSearch_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptvSearch_SearchNormal(void);
static void SenIptvSearch_SearchReq(void);
static STATEID StaIptvSearch_search_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptvSearch_search_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptvSearch_SearchReq(void);

//State transition table
static const STATE_TRANS_T SttIptvSearch_Inactive[] =
{/*002|--+ SID_IPTV_SEARCH_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_IPTV_SEARCH_REQ, NULL, (SFUNA) StaIptvSearch_inactive_on_open_iptv_search_req},
};

static const STATE_TRANS_T SttIptvSearch_Active[] =
{/*003|--+ SID_IPTV_SEARCH_ACTIVE */
    {SID_IPTV_SEARCH_INACTIVE, MSG_SAVE, NULL, (SFUNA) StaIptvSearch_active_on_exit},
    {SID_IPTV_SEARCH_INACTIVE, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaIptvSearch_active_on_exit},
    {SID_IPTV_SEARCH_INACTIVE, MSG_EXIT, NULL, (SFUNA) StaIptvSearch_active_on_exit},
    {SID_IPTV_SEARCH_INACTIVE, MSG_INFO, NULL, (SFUNA) StaIptvSearch_active_on_exit},
};

static const STATE_TRANS_T SttIptvSearch_Search[] =
{/*004|  |--+ SID_IPTV_SEARCH_SEARCH */
    {SID_INTERNAL, MSG_FOCUS_UP, NULL, (SFUNA) StaIptvSearch_search_on_focus_key_hldr},
    {SID_INTERNAL, MSG_FOCUS_DOWN, NULL, (SFUNA) StaIptvSearch_search_on_focus_key_hldr},
    {SID_INTERNAL, MSG_FOCUS_LEFT, NULL, (SFUNA) StaIptvSearch_search_on_focus_key_hldr},
    {SID_INTERNAL, MSG_FOCUS_RIGHT, NULL, (SFUNA) StaIptvSearch_search_on_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptvSearch_is_on_matrixkey, (SFUNA) StaIptvSearch_search_on_matrixkey_selected},
    {SID_INTERNAL, MSG_NUMBER, (SFUNC) StcIptvSearch_is_on_matrixkey, (SFUNA) StaIptvSearch_search_on_matrixkey_selected},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptvSearch_is_on_customkey, (SFUNA) StaIptvSearch_search_on_customkey_selected},
};

static const STATE_TRANS_T SttIptvSearch_SearchNormal[] =
{/*005|     |--+ SID_IPTV_SEARCH_SEARCH_NORMAL */
    {SID_INTERNAL, MSG_PAGE_UP, (SFUNC) StcIptvSearch_is_on_vdolist, (SFUNA) StaIptvSearch_search_on_change_page},
    {SID_INTERNAL, MSG_PAGE_DOWN, (SFUNC) StcIptvSearch_is_on_vdolist, (SFUNA) StaIptvSearch_search_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptvSearch_is_on_searchkey, (SFUNA) StaIptvSearch_search_on_searchkey_selected},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptvSearch_is_on_vdolist, (SFUNA) StaIptvSearch_search_on_vdolist_selected},
};

static const STATE_TRANS_T SttIptvSearch_SearchReq[] =
{/*006|     |--+ SID_IPTV_SEARCH_SEARCH_REQ */
    {SID_INTERNAL, MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, NULL, (SFUNA) StaIptvSearch_search_on_newpagevdo_arrive},
    {SID_INTERNAL, MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL, NULL, (SFUNA) StaIptvSearch_search_on_get_pagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaIptvSearch_search_on_vdoreq_dlg_closed},
};

//State tree
static const STATE_NODE_T pStateTree_IptvSearch[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_IPTV_SEARCH */ {SID_NULL, SID_IPTV_SEARCH_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_IPTV_SEARCH_INACTIVE */ {SID_IPTV_SEARCH, SID_NULL, (SFUNE) SenIptvSearch_Inactive, (SFUNE) SexIptvSearch_Inactive, SttIptvSearch_Inactive, sizeof(SttIptvSearch_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_IPTV_SEARCH_ACTIVE */ {SID_IPTV_SEARCH, SID_IPTV_SEARCH_SEARCH, (SFUNE) SenIptvSearch_Active, (SFUNE) SexIptvSearch_Active, SttIptvSearch_Active, sizeof(SttIptvSearch_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_IPTV_SEARCH_SEARCH */ {SID_IPTV_SEARCH_ACTIVE, SID_IPTV_SEARCH_SEARCH_NORMAL, (SFUNE) SenIptvSearch_Search, (SFUNE) SexIptvSearch_Search, SttIptvSearch_Search, sizeof(SttIptvSearch_Search)/sizeof(STATE_TRANS_T)},
/*005|     |--+ SID_IPTV_SEARCH_SEARCH_NORMAL */ {SID_IPTV_SEARCH_SEARCH, SID_NULL, (SFUNE) SenIptvSearch_SearchNormal, (SFUNE) SexIptvSearch_SearchNormal, SttIptvSearch_SearchNormal, sizeof(SttIptvSearch_SearchNormal)/sizeof(STATE_TRANS_T)},
/*006|     |--+ SID_IPTV_SEARCH_SEARCH_REQ */ {SID_IPTV_SEARCH_SEARCH, SID_NULL, (SFUNE) SenIptvSearch_SearchReq, (SFUNE) SexIptvSearch_SearchReq, SttIptvSearch_SearchReq, sizeof(SttIptvSearch_SearchReq)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_IptvSearch = {
    pStateTree_IptvSearch,
    "APP_IPTV_SEARCH_T",
    sizeof(pStateTree_IptvSearch)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 IptvSearch_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_IptvSearch));
}

s32 IptvSearch_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_IptvSearch));
}

BOOL IptvSearch_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_IptvSearch), stateID);
}

s32 IptvSearch_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_IptvSearch), ctrl, msg, para1, para2);
}
