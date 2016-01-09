/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

****************************************************************************/
#define SFUNE void (*)(void)
#define SFUNC BOOL (*)(PVOID, MSGID, u32, u32)
#define SFUNA STATEID (*)(PVOID, MSGID, u32, u32)

//State IDs
enum
{
/*001+*/ SID_IPTV = SID_ROOT,
/*002|--+*/ SID_IPTV_INACTIVE,
/*003|--+*/ SID_IPTV_ACTIVE,
/*004|  |--+*/ SID_IPTV_INITIALIZE,
/*005|  |--+*/ SID_IPTV_RESOURCE,
/*006|     |--+*/ SID_IPTV_RES_VDO_REQ,
/*007|     |--+*/ SID_IPTV_RES_VDOLIST_NORMAL,
/*008|  |--+*/ SID_IPTV_INIT_FAILED,
/*009|  |--+*/ SID_IPTV_DEINIT,
};

//Prototype of state entry, transition condition and action, and exit
static void SenIptv_Inactive(void);
static STATEID StaIptv_inactive_on_open_iptv_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_Inactive(void);
static void SenIptv_Active(void);
static STATEID StaIptv_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_Active(void);
static void SenIptv_Initialize(void);
static STATEID StaIptv_initialize_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_initialize_on_newresname_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_initialize_on_newrescatgry_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_Initialize(void);
static void SenIptv_Resource(void);
static STATEID StaIptv_resource_on_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_catlist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_catlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_reslist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_reslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_Resource(void);
static void SenIptv_ResVdoReq(void);
static STATEID StaIptv_resource_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_ResVdoReq(void);
static void SenIptv_ResVdolistNormal(void);
static STATEID StaIptv_resource_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcIptv_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaIptv_resource_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_ResVdolistNormal(void);
static void SenIptv_InitFailed(void);
static void SexIptv_InitFailed(void);
static void SenIptv_Deinit(void);
static STATEID StaIptv_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexIptv_Deinit(void);

//State transition table
static const STATE_TRANS_T SttIptv_Inactive[] =
{/*002|--+ SID_IPTV_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_IPTV_REQ, NULL, (SFUNA) StaIptv_inactive_on_open_iptv_req},
};

static const STATE_TRANS_T SttIptv_Active[] =
{/*003|--+ SID_IPTV_ACTIVE */
    {SID_IPTV_INACTIVE, MSG_SAVE, NULL, (SFUNA) StaIptv_active_on_quick_exit},
    {SID_INTERNAL, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaIptv_active_on_exit},
    {SID_INTERNAL, MSG_EXIT, NULL, (SFUNA) StaIptv_active_on_exit},
    {SID_INTERNAL, MSG_EXIT_ALL, NULL, (SFUNA) StaIptv_active_on_exit},
};

static const STATE_TRANS_T SttIptv_Initialize[] =
{/*004|  |--+ SID_IPTV_INITIALIZE */
    {SID_INTERNAL, MSG_IPTV_EVT_INIT_SUCCESS, NULL, (SFUNA) StaIptv_initialize_on_init_success},
    {SID_INTERNAL, MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE, NULL, (SFUNA) StaIptv_initialize_on_newresname_arrive},
    {SID_INTERNAL, MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE, NULL, (SFUNA) StaIptv_initialize_on_newrescatgry_arrive},
    {SID_IPTV_INIT_FAILED, MSG_IPTV_EVT_INIT_FAIL, NULL, NULL},
    {SID_IPTV_INIT_FAILED, MSG_IPTV_EVT_GET_RES_NAME_FAIL, NULL, NULL},
    {SID_IPTV_INIT_FAILED, MSG_IPTV_EVT_GET_RES_CATGRY_FAIL, NULL, NULL},
};

static const STATE_TRANS_T SttIptv_Resource[] =
{/*005|  |--+ SID_IPTV_RESOURCE */
    {SID_INTERNAL, MSG_FOCUS_KEY, NULL, (SFUNA) StaIptv_resource_on_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_UP, (SFUNC) StcIptv_is_on_vdolist, (SFUNA) StaIptv_resource_on_change_page},
    {SID_INTERNAL, MSG_PAGE_DOWN, (SFUNC) StcIptv_is_on_vdolist, (SFUNA) StaIptv_resource_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptv_is_on_catlist, (SFUNA) StaIptv_resource_on_catlist_selected},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptv_is_on_reslist, (SFUNA) StaIptv_resource_on_reslist_selected},
    {SID_INTERNAL, MSG_UPDATE, NULL, (SFUNA) StaIptv_resource_on_subwindow_exit},
};

static const STATE_TRANS_T SttIptv_ResVdoReq[] =
{/*006|     |--+ SID_IPTV_RES_VDO_REQ */
    {SID_INTERNAL, MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE, NULL, (SFUNA) StaIptv_resource_on_newpagevdo_arrive},
    {SID_INTERNAL, MSG_IPTV_EVT_GET_PAGE_VDO_FAIL, NULL, (SFUNA) StaIptv_resource_on_get_pagevdo_fail},
    {SID_INTERNAL, MSG_CLOSE_CFMDLG_NTF, NULL, (SFUNA) StaIptv_resource_on_vdoreq_dlg_closed},
};

static const STATE_TRANS_T SttIptv_ResVdolistNormal[] =
{/*007|     |--+ SID_IPTV_RES_VDOLIST_NORMAL */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaIptv_resource_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaIptv_resource_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaIptv_resource_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaIptv_resource_pic_draw_fail},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcIptv_is_on_vdolist, (SFUNA) StaIptv_resource_on_vdolist_selected},
    {SID_INTERNAL, MSG_NUMBER, NULL, (SFUNA) StaIptv_resource_on_input_number},
    {SID_IPTV_RES_VDO_REQ, MSG_SELECT, (SFUNC) StcIptv_is_on_page_num, (SFUNA) StaIptv_resource_on_page_num_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcIptv_is_on_page_num, (SFUNA) StaIptv_resource_on_page_num_exit},
};

static const STATE_TRANS_T SttIptv_Deinit[] =
{/*009|  |--+ SID_IPTV_DEINIT */
    {SID_IPTV_INACTIVE, MSG_IPTV_EVT_DEINIT_SUCCESS, NULL, (SFUNA) StaIptv_deinit_on_deinit_success},
};

//State tree
static const STATE_NODE_T pStateTree_Iptv[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_IPTV */ {SID_NULL, SID_IPTV_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_IPTV_INACTIVE */ {SID_IPTV, SID_NULL, (SFUNE) SenIptv_Inactive, (SFUNE) SexIptv_Inactive, SttIptv_Inactive, sizeof(SttIptv_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_IPTV_ACTIVE */ {SID_IPTV, SID_IPTV_INITIALIZE, (SFUNE) SenIptv_Active, (SFUNE) SexIptv_Active, SttIptv_Active, sizeof(SttIptv_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_IPTV_INITIALIZE */ {SID_IPTV_ACTIVE, SID_NULL, (SFUNE) SenIptv_Initialize, (SFUNE) SexIptv_Initialize, SttIptv_Initialize, sizeof(SttIptv_Initialize)/sizeof(STATE_TRANS_T)},
/*005|  |--+ SID_IPTV_RESOURCE */ {SID_IPTV_ACTIVE, SID_IPTV_RES_VDO_REQ, (SFUNE) SenIptv_Resource, (SFUNE) SexIptv_Resource, SttIptv_Resource, sizeof(SttIptv_Resource)/sizeof(STATE_TRANS_T)},
/*006|     |--+ SID_IPTV_RES_VDO_REQ */ {SID_IPTV_RESOURCE, SID_NULL, (SFUNE) SenIptv_ResVdoReq, (SFUNE) SexIptv_ResVdoReq, SttIptv_ResVdoReq, sizeof(SttIptv_ResVdoReq)/sizeof(STATE_TRANS_T)},
/*007|     |--+ SID_IPTV_RES_VDOLIST_NORMAL */ {SID_IPTV_RESOURCE, SID_NULL, (SFUNE) SenIptv_ResVdolistNormal, (SFUNE) SexIptv_ResVdolistNormal, SttIptv_ResVdolistNormal, sizeof(SttIptv_ResVdolistNormal)/sizeof(STATE_TRANS_T)},
/*008|  |--+ SID_IPTV_INIT_FAILED */ {SID_IPTV_ACTIVE, SID_NULL, (SFUNE) SenIptv_InitFailed, (SFUNE) SexIptv_InitFailed, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*009|  |--+ SID_IPTV_DEINIT */ {SID_IPTV_ACTIVE, SID_NULL, (SFUNE) SenIptv_Deinit, (SFUNE) SexIptv_Deinit, SttIptv_Deinit, sizeof(SttIptv_Deinit)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Iptv = {
    pStateTree_Iptv,
    "APP_IPTV_T",
    sizeof(pStateTree_Iptv)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Iptv_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Iptv));
}

s32 Iptv_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Iptv));
}

BOOL Iptv_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Iptv), stateID);
}

s32 Iptv_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Iptv), ctrl, msg, para1, para2);
}
