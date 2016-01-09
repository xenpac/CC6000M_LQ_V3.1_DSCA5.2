/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Desktop.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2014-9-9 16:13:08
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
/*001+*/ SID_DESKTOP = SID_ROOT,
/*002|--+*/ SID_DESKTOP_DISCONNECTED,
/*003|--+*/ SID_DESKTOP_CONNECTED,
/*004|  |--+*/ SID_DESKTOP_INITIALIZE,
/*005|     |--+*/ SID_DESKTOP_LIVETV_INIT,
/*006|        |--+*/ SID_DESKTOP_LIVETV_DP_INIT,
/*007|        |--+*/ SID_DESKTOP_LIVETV_CATGRY_INIT,
/*008|        |--+*/ SID_DESKTOP_LIVETV_CHAN_ZIP_INIT,
/*009|        |--+*/ SID_DESKTOP_LIVETV_CHAN_LIST_INIT,
/*010|     |--+*/ SID_DESKTOP_IPTV_INIT,
/*011|  |--+*/ SID_DESKTOP_IDLE,
};

//Prototype of state entry, transition condition and action, and exit
static void SenDesktop_Disconnected(void);
static STATEID StaDesktop_desktop_on_connect_network(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_Disconnected(void);
static void SenDesktop_Connected(void);
static STATEID StaDesktop_desktop_on_disconnect_network(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_Connected(void);
static void SenDesktop_Initialize(void);
static void SexDesktop_Initialize(void);
static void SenDesktop_LivetvInit(void);
static void SexDesktop_LivetvInit(void);
static void SenDesktop_LivetvDpInit(void);
static STATEID StaDesktop_livetv_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_livetv_init_on_init_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_jump_to_desktop_livetv_categry_init(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_LivetvDpInit(void);
static void SenDesktop_LivetvCatgryInit(void);
static STATEID StaDesktop_livetv_init_on_catgry_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_jump_to_desktop_chan_zip_init(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_LivetvCatgryInit(void);
static void SenDesktop_LivetvChanZipInit(void);
static STATEID StaDesktop_livetv_init_on_chan_zip_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_jump_to_desktop_chan_list_init(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_LivetvChanZipInit(void);
static void SenDesktop_LivetvChanListInit(void);
static STATEID StaDesktop_livetv_init_on_livetv_all_chan_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_jump_to_desktop_iptv_init(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_LivetvChanListInit(void);
static void SenDesktop_IptvInit(void);
static STATEID StaDesktop_iptv_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_iptv_init_on_init_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_jump_to_desktop_idle_status(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_IptvInit(void);
static void SenDesktop_Idle(void);
static STATEID StaDesktop_livetv_init_on_init_success_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_livetv_init_on_init_failed_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaDesktop_iptv_init_on_init_success_idle(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexDesktop_Idle(void);

//State transition table
static const STATE_TRANS_T SttDesktop_Disconnected[] =
{/*002|--+ SID_DESKTOP_DISCONNECTED */
    {SID_INTERNAL, MSG_DESKTOP_NET_CONNTECED, NULL, (SFUNA) StaDesktop_desktop_on_connect_network},
};

static const STATE_TRANS_T SttDesktop_Connected[] =
{/*003|--+ SID_DESKTOP_CONNECTED */
    {SID_INTERNAL, MSG_DESKTOP_NET_UNCONNTECED, NULL, (SFUNA) StaDesktop_desktop_on_disconnect_network},
};

static const STATE_TRANS_T SttDesktop_LivetvDpInit[] =
{/*006|        |--+ SID_DESKTOP_LIVETV_DP_INIT */
    {SID_INTERNAL, MSG_INIT_LIVETV_SUCCESS, NULL, (SFUNA) StaDesktop_livetv_init_on_init_success},
    {SID_INTERNAL, MSG_INIT_LIVETV_FAILED, NULL, (SFUNA) StaDesktop_livetv_init_on_init_fail},
    {SID_INTERNAL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, NULL, (SFUNA) StaDesktop_jump_to_desktop_livetv_categry_init},
};

static const STATE_TRANS_T SttDesktop_LivetvCatgryInit[] =
{/*007|        |--+ SID_DESKTOP_LIVETV_CATGRY_INIT */
    {SID_INTERNAL, MSG_GET_LIVE_TV_CATEGORY_ARRIVAL, NULL, (SFUNA) StaDesktop_livetv_init_on_catgry_arrived},
    {SID_INTERNAL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, NULL, (SFUNA) StaDesktop_jump_to_desktop_chan_zip_init},
};

static const STATE_TRANS_T SttDesktop_LivetvChanZipInit[] =
{/*008|        |--+ SID_DESKTOP_LIVETV_CHAN_ZIP_INIT */
    {SID_INTERNAL, MSG_GET_LIVE_TV_GET_ZIP, NULL, (SFUNA) StaDesktop_livetv_init_on_chan_zip_arrived},
    {SID_INTERNAL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, NULL, (SFUNA) StaDesktop_jump_to_desktop_chan_list_init},
};

static const STATE_TRANS_T SttDesktop_LivetvChanListInit[] =
{/*009|        |--+ SID_DESKTOP_LIVETV_CHAN_LIST_INIT */
    {SID_INTERNAL, MSG_GET_LIVE_TV_CHANNELLIST, NULL, (SFUNA) StaDesktop_livetv_init_on_livetv_all_chan_arrived},
    {SID_INTERNAL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, NULL, (SFUNA) StaDesktop_jump_to_desktop_iptv_init},
};

static const STATE_TRANS_T SttDesktop_IptvInit[] =
{/*010|     |--+ SID_DESKTOP_IPTV_INIT */
    {SID_INTERNAL, MSG_IPTV_EVT_INIT_SUCCESS, NULL, (SFUNA) StaDesktop_iptv_init_on_init_success},
    {SID_INTERNAL, MSG_IPTV_EVT_INIT_FAIL, NULL, (SFUNA) StaDesktop_iptv_init_on_init_fail},
    {SID_INTERNAL, MSG_DESKTOP_JUMP_TO_NEXT_STATUS, NULL, (SFUNA) StaDesktop_jump_to_desktop_idle_status},
};

static const STATE_TRANS_T SttDesktop_Idle[] =
{/*011|  |--+ SID_DESKTOP_IDLE */
    {SID_INTERNAL, MSG_INIT_LIVETV_SUCCESS, NULL, (SFUNA) StaDesktop_livetv_init_on_init_success_idle},
    {SID_INTERNAL, MSG_INIT_LIVETV_FAILED, NULL, (SFUNA) StaDesktop_livetv_init_on_init_failed_idle},
    {SID_INTERNAL, MSG_IPTV_EVT_INIT_SUCCESS, NULL, (SFUNA) StaDesktop_iptv_init_on_init_success_idle},
};

//State tree
static const STATE_NODE_T pStateTree_Desktop[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_DESKTOP */ {SID_NULL, SID_DESKTOP_DISCONNECTED, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_DESKTOP_DISCONNECTED */ {SID_DESKTOP, SID_NULL, (SFUNE) SenDesktop_Disconnected, (SFUNE) SexDesktop_Disconnected, SttDesktop_Disconnected, sizeof(SttDesktop_Disconnected)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_DESKTOP_CONNECTED */ {SID_DESKTOP, SID_DESKTOP_INITIALIZE, (SFUNE) SenDesktop_Connected, (SFUNE) SexDesktop_Connected, SttDesktop_Connected, sizeof(SttDesktop_Connected)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_DESKTOP_INITIALIZE */ {SID_DESKTOP_CONNECTED, SID_DESKTOP_LIVETV_INIT, (SFUNE) SenDesktop_Initialize, (SFUNE) SexDesktop_Initialize, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*005|     |--+ SID_DESKTOP_LIVETV_INIT */ {SID_DESKTOP_INITIALIZE, SID_DESKTOP_LIVETV_DP_INIT, (SFUNE) SenDesktop_LivetvInit, (SFUNE) SexDesktop_LivetvInit, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*006|        |--+ SID_DESKTOP_LIVETV_DP_INIT */ {SID_DESKTOP_LIVETV_INIT, SID_NULL, (SFUNE) SenDesktop_LivetvDpInit, (SFUNE) SexDesktop_LivetvDpInit, SttDesktop_LivetvDpInit, sizeof(SttDesktop_LivetvDpInit)/sizeof(STATE_TRANS_T)},
/*007|        |--+ SID_DESKTOP_LIVETV_CATGRY_INIT */ {SID_DESKTOP_LIVETV_INIT, SID_NULL, (SFUNE) SenDesktop_LivetvCatgryInit, (SFUNE) SexDesktop_LivetvCatgryInit, SttDesktop_LivetvCatgryInit, sizeof(SttDesktop_LivetvCatgryInit)/sizeof(STATE_TRANS_T)},
/*008|        |--+ SID_DESKTOP_LIVETV_CHAN_ZIP_INIT */ {SID_DESKTOP_LIVETV_INIT, SID_NULL, (SFUNE) SenDesktop_LivetvChanZipInit, (SFUNE) SexDesktop_LivetvChanZipInit, SttDesktop_LivetvChanZipInit, sizeof(SttDesktop_LivetvChanZipInit)/sizeof(STATE_TRANS_T)},
/*009|        |--+ SID_DESKTOP_LIVETV_CHAN_LIST_INIT */ {SID_DESKTOP_LIVETV_INIT, SID_NULL, (SFUNE) SenDesktop_LivetvChanListInit, (SFUNE) SexDesktop_LivetvChanListInit, SttDesktop_LivetvChanListInit, sizeof(SttDesktop_LivetvChanListInit)/sizeof(STATE_TRANS_T)},
/*010|     |--+ SID_DESKTOP_IPTV_INIT */ {SID_DESKTOP_INITIALIZE, SID_NULL, (SFUNE) SenDesktop_IptvInit, (SFUNE) SexDesktop_IptvInit, SttDesktop_IptvInit, sizeof(SttDesktop_IptvInit)/sizeof(STATE_TRANS_T)},
/*011|  |--+ SID_DESKTOP_IDLE */ {SID_DESKTOP_CONNECTED, SID_NULL, (SFUNE) SenDesktop_Idle, (SFUNE) SexDesktop_Idle, SttDesktop_Idle, sizeof(SttDesktop_Idle)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Desktop = {
    pStateTree_Desktop,
    "APP_DESKTOP_T",
    sizeof(pStateTree_Desktop)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Desktop_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Desktop));
}

s32 Desktop_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Desktop));
}

BOOL Desktop_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Desktop), stateID);
}

s32 Desktop_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Desktop), ctrl, msg, para1, para2);
}
