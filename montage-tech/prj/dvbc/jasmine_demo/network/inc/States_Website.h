/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Website.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2014-9-24 11:57:41
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
/*001+*/ SID_WEBSITE = SID_ROOT,
/*002|--+*/ SID_WEBSITE_INACTIVE,
/*003|--+*/ SID_WEBSITE_ACTIVE,
/*004|  |--+*/ SID_WEBSITE_INIT,
/*005|  |--+*/ SID_WEBSITE_WEBSITE,
/*006|  |--+*/ SID_WEBSITE_INIT_FAILED,
/*007|  |--+*/ SID_WEBSITE_DEINIT,
};

//Prototype of state entry, transition condition and action, and exit
static void SenWebsite_Inactive(void);
static STATEID StaWebsite_inactive_on_open_website_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexWebsite_Inactive(void);
static void SenWebsite_Active(void);
static STATEID StaWebsite_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexWebsite_Active(void);
static void SenWebsite_Init(void);
static STATEID StaWebsite_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_init_on_newweblist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexWebsite_Init(void);
static void SenWebsite_Website(void);
static STATEID StaWebsite_web_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcWebsite_is_on_website_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcWebsite_is_on_website_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_on_weblist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcWebsite_is_on_website_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_on_weblist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_web_on_weblist_update(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexWebsite_Website(void);
static void SenWebsite_InitFailed(void);
static void SexWebsite_InitFailed(void);
static void SenWebsite_Deinit(void);
static STATEID StaWebsite_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaWebsite_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexWebsite_Deinit(void);

//State transition table
static const STATE_TRANS_T SttWebsite_Inactive[] =
{/*002|--+ SID_WEBSITE_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_WEBSITE_REQ, NULL, (SFUNA) StaWebsite_inactive_on_open_website_req},
};

static const STATE_TRANS_T SttWebsite_Active[] =
{/*003|--+ SID_WEBSITE_ACTIVE */
    {SID_WEBSITE_INACTIVE, MSG_SAVE, NULL, (SFUNA) StaWebsite_active_on_quick_exit},
    {SID_INTERNAL, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaWebsite_active_on_exit},
    {SID_INTERNAL, MSG_EXIT, NULL, (SFUNA) StaWebsite_active_on_exit},
    {SID_INTERNAL, MSG_EXIT_ALL, NULL, (SFUNA) StaWebsite_active_on_exit},
};

static const STATE_TRANS_T SttWebsite_Init[] =
{/*004|  |--+ SID_WEBSITE_INIT */
    {SID_INTERNAL, MSG_NETMEDIA_INIT_SUCCESS, NULL, (SFUNA) StaWebsite_init_on_init_success},
    {SID_WEBSITE_INIT_FAILED, MSG_NETMEDIA_INIT_FAILED, NULL, NULL},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_WEBSITE_INFO_ARRIVED, NULL, (SFUNA) StaWebsite_init_on_newweblist_arrived},
    {SID_WEBSITE_INIT_FAILED, MSG_NETMEDIA_GET_WEBSITE_INFO_FAILED, NULL, NULL},
};

static const STATE_TRANS_T SttWebsite_Website[] =
{/*005|  |--+ SID_WEBSITE_WEBSITE */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaWebsite_web_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaWebsite_web_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaWebsite_web_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaWebsite_web_pic_draw_fail},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcWebsite_is_on_website_list, (SFUNA) StaWebsite_web_on_change_page},
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcWebsite_is_on_website_list, (SFUNA) StaWebsite_web_on_weblist_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcWebsite_is_on_website_list, (SFUNA) StaWebsite_web_on_weblist_selected},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaWebsite_web_on_weblist_update},
};

static const STATE_TRANS_T SttWebsite_Deinit[] =
{/*007|  |--+ SID_WEBSITE_DEINIT */
    {SID_WEBSITE_INACTIVE, MSG_NETMEDIA_DEINIT_SUCCESS, NULL, (SFUNA) StaWebsite_deinit_on_deinit_success},
    {SID_WEBSITE_INACTIVE, MSG_NETMEDIA_DEINIT_FAILED, NULL, (SFUNA) StaWebsite_deinit_on_deinit_success},
};

//State tree
static const STATE_NODE_T pStateTree_Website[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_WEBSITE */ {SID_NULL, SID_WEBSITE_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_WEBSITE_INACTIVE */ {SID_WEBSITE, SID_NULL, (SFUNE) SenWebsite_Inactive, (SFUNE) SexWebsite_Inactive, SttWebsite_Inactive, sizeof(SttWebsite_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_WEBSITE_ACTIVE */ {SID_WEBSITE, SID_WEBSITE_INIT, (SFUNE) SenWebsite_Active, (SFUNE) SexWebsite_Active, SttWebsite_Active, sizeof(SttWebsite_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_WEBSITE_INIT */ {SID_WEBSITE_ACTIVE, SID_NULL, (SFUNE) SenWebsite_Init, (SFUNE) SexWebsite_Init, SttWebsite_Init, sizeof(SttWebsite_Init)/sizeof(STATE_TRANS_T)},
/*005|  |--+ SID_WEBSITE_WEBSITE */ {SID_WEBSITE_ACTIVE, SID_NULL, (SFUNE) SenWebsite_Website, (SFUNE) SexWebsite_Website, SttWebsite_Website, sizeof(SttWebsite_Website)/sizeof(STATE_TRANS_T)},
/*006|  |--+ SID_WEBSITE_INIT_FAILED */ {SID_WEBSITE_ACTIVE, SID_NULL, (SFUNE) SenWebsite_InitFailed, (SFUNE) SexWebsite_InitFailed, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*007|  |--+ SID_WEBSITE_DEINIT */ {SID_WEBSITE_ACTIVE, SID_NULL, (SFUNE) SenWebsite_Deinit, (SFUNE) SexWebsite_Deinit, SttWebsite_Deinit, sizeof(SttWebsite_Deinit)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Website = {
    pStateTree_Website,
    "APP_WEBSITE_T",
    sizeof(pStateTree_Website)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Website_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Website));
}

s32 Website_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Website));
}

BOOL Website_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Website), stateID);
}

s32 Website_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Website), ctrl, msg, para1, para2);
}
