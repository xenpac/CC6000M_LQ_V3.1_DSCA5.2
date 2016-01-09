/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Simple.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2014-9-3 14:16:43
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
/*001+*/ SID_SIMPLE = SID_ROOT,
/*002|--+*/ SID_SIMPLE_INACTIVE,
/*003|--+*/ SID_SIMPLE_ACTIVE,
/*004|  |--+*/ SID_SIMPLE_INIT,
/*005|  |--+*/ SID_SIMPLE_RESOURCE,
/*006|     |--+*/ SID_SIMPLE_SUBCLASS,
/*007|        |--+*/ SID_SIMPLE_SUBCLASS_REQ,
/*008|        |--+*/ SID_SIMPLE_SUBCLASS_LIST,
/*009|     |--+*/ SID_SIMPLE_VDO,
/*010|        |--+*/ SID_SIMPLE_VDO_REQ,
/*011|        |--+*/ SID_SIMPLE_VDO_LIST,
/*012|        |--+*/ SID_SIMPLE_VDO_URL_REQ,
/*013|     |--+*/ SID_SIMPLE_SEARCH,
/*014|        |--+*/ SID_SIMPLE_SEARCH_VDO_REQ,
/*015|        |--+*/ SID_SIMPLE_SEARCH_VDO_LIST,
/*016|        |--+*/ SID_SIMPLE_SEARCH_URL_REQ,
/*017|     |--+*/ SID_SIMPLE_VDO_INFO,
/*018|     |--+*/ SID_SIMPLE_RESOLUTION,
/*019|  |--+*/ SID_SIMPLE_INIT_FAILED,
/*020|  |--+*/ SID_SIMPLE_DEINIT,
};

//Prototype of state entry, transition condition and action, and exit
static void SenSimple_Inactive(void);
static STATEID StaSimple_inactive_on_open_simple_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Inactive(void);
static void SenSimple_Active(void);
static STATEID StaSimple_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Active(void);
static void SenSimple_Init(void);
static STATEID StaSimple_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_init_on_newclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Init(void);
static void SenSimple_Resource(void);
static BOOL    StcSimple_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resource_on_classlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resource_on_classlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Resource(void);
static void SenSimple_Subclass(void);
static void SexSimple_Subclass(void);
static void SenSimple_SubclassReq(void);
static STATEID StaSimple_subclass_on_newsubclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_get_subclasslist_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_SubclassReq(void);
static void SenSimple_SubclassList(void);
static BOOL    StcSimple_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_subclasslist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_subclasslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_subclasslist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_subclass_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_SubclassList(void);
static void SenSimple_Vdo(void);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Vdo(void);
static void SenSimple_VdoReq(void);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_newpagevdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_VdoReq(void);
static void SenSimple_VdoList(void);
static STATEID StaSimple_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_VdoList(void);
static void SenSimple_VdoUrlReq(void);
static STATEID StaSimple_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_VdoUrlReq(void);
static void SenSimple_Search(void);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Search(void);
static void SenSimple_SearchVdoReq(void);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_newsearchvdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_SearchVdoReq(void);
static void SenSimple_SearchVdoList(void);
static STATEID StaSimple_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_vdolist_info_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_SearchVdoList(void);
static void SenSimple_SearchUrlReq(void);
static STATEID StaSimple_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_SearchUrlReq(void);
static void SenSimple_VdoInfo(void);
static BOOL    StcSimple_is_on_info_detail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_info_on_infodetail_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_info_detail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_vdo_info_on_infodetail_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_VdoInfo(void);
static void SenSimple_Resolution(void);
static BOOL    StcSimple_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resolution_on_optlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resolution_on_optlist_select(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcSimple_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_resolution_on_optlist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Resolution(void);
static void SenSimple_InitFailed(void);
static void SexSimple_InitFailed(void);
static void SenSimple_Deinit(void);
static STATEID StaSimple_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaSimple_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexSimple_Deinit(void);

//State transition table
static const STATE_TRANS_T SttSimple_Inactive[] =
{/*002|--+ SID_SIMPLE_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_SIMPLE_REQ, NULL, (SFUNA) StaSimple_inactive_on_open_simple_req},
};

static const STATE_TRANS_T SttSimple_Active[] =
{/*003|--+ SID_SIMPLE_ACTIVE */
    {SID_SIMPLE_INACTIVE, MSG_SAVE, NULL, (SFUNA) StaSimple_active_on_quick_exit},
    {SID_INTERNAL, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaSimple_active_on_exit},
    {SID_INTERNAL, MSG_EXIT, NULL, (SFUNA) StaSimple_active_on_exit},
    {SID_INTERNAL, MSG_EXIT_ALL, NULL, (SFUNA) StaSimple_active_on_exit},
};

static const STATE_TRANS_T SttSimple_Init[] =
{/*004|  |--+ SID_SIMPLE_INIT */
    {SID_INTERNAL, MSG_NETMEDIA_WEBSITE_INIT_SUCCESS, NULL, (SFUNA) StaSimple_init_on_init_success},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED, NULL, (SFUNA) StaSimple_init_on_newclasslist_arrived},
    {SID_SIMPLE_INIT_FAILED, MSG_NETMEDIA_WEBSITE_INIT_FAILED, NULL, NULL},
    {SID_SIMPLE_INIT_FAILED, MSG_NETMEDIA_GET_CLASS_LIST_FAILED, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_Resource[] =
{/*005|  |--+ SID_SIMPLE_RESOURCE */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_class_list, (SFUNA) StaSimple_resource_on_classlist_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcSimple_is_on_class_list, (SFUNA) StaSimple_resource_on_classlist_selected},
};

static const STATE_TRANS_T SttSimple_SubclassReq[] =
{/*007|        |--+ SID_SIMPLE_SUBCLASS_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, NULL, (SFUNA) StaSimple_subclass_on_newsubclasslist_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED, NULL, (SFUNA) StaSimple_subclass_on_get_subclasslist_failed},
    {SID_SIMPLE_SUBCLASS_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_SubclassList[] =
{/*008|        |--+ SID_SIMPLE_SUBCLASS_LIST */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_subclass_list, (SFUNA) StaSimple_subclass_on_subclasslist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcSimple_is_on_subclass_list, (SFUNA) StaSimple_subclass_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcSimple_is_on_subclass_list, (SFUNA) StaSimple_subclass_on_subclasslist_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_subclass_list, (SFUNA) StaSimple_subclass_on_subclasslist_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaSimple_resource_on_search_pressed},
    {SID_SIMPLE_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaSimple_subclass_on_input_completed},
};

static const STATE_TRANS_T SttSimple_Vdo[] =
{/*009|     |--+ SID_SIMPLE_VDO */
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_exit},
};

static const STATE_TRANS_T SttSimple_VdoReq[] =
{/*010|        |--+ SID_SIMPLE_VDO_REQ */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_change_page},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED, NULL, (SFUNA) StaSimple_vdo_on_newpagevdo_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PAGE_VDO_FAILED, NULL, (SFUNA) StaSimple_vdo_on_get_pagevdo_failed},
    {SID_SIMPLE_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_VdoList[] =
{/*011|        |--+ SID_SIMPLE_VDO_LIST */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaSimple_vdo_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_selected},
    {SID_SIMPLE_VDO_INFO, MSG_INFO, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_info_pressed},
    {SID_INTERNAL, MSG_NUMBER, NULL, (SFUNA) StaSimple_vdo_on_input_number},
    {SID_SIMPLE_VDO_REQ, MSG_SELECT, (SFUNC) StcSimple_is_on_page_num, (SFUNA) StaSimple_vdo_on_page_num_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_page_num, (SFUNA) StaSimple_vdo_on_page_num_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaSimple_resource_on_search_pressed},
    {SID_SIMPLE_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaSimple_vdo_on_input_completed},
};

static const STATE_TRANS_T SttSimple_VdoUrlReq[] =
{/*012|        |--+ SID_SIMPLE_VDO_URL_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, NULL, (SFUNA) StaSimple_vdo_popu_on_newplayurl_arrived},
    {SID_SIMPLE_VDO_LIST, MSG_UPDATE, NULL, (SFUNA) StaSimple_vdo_on_subwindow_exit},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PLAY_URLS_FAILED, NULL, (SFUNA) StaSimple_vdo_popu_on_get_playurl_failed},
    {SID_SIMPLE_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_Search[] =
{/*013|     |--+ SID_SIMPLE_SEARCH */
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_vdolist_exit},
};

static const STATE_TRANS_T SttSimple_SearchVdoReq[] =
{/*014|        |--+ SID_SIMPLE_SEARCH_VDO_REQ */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_change_page},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, NULL, (SFUNA) StaSimple_search_on_newsearchvdo_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_SEARCH_VDO_FAILED, NULL, (SFUNA) StaSimple_vdo_on_get_pagevdo_failed},
    {SID_SIMPLE_SEARCH_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_SearchVdoList[] =
{/*015|        |--+ SID_SIMPLE_SEARCH_VDO_LIST */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaSimple_vdo_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaSimple_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_search_on_vdolist_selected},
    {SID_SIMPLE_VDO_INFO, MSG_INFO, (SFUNC) StcSimple_is_on_vdo_list, (SFUNA) StaSimple_vdo_on_vdolist_info_pressed},
    {SID_INTERNAL, MSG_NUMBER, NULL, (SFUNA) StaSimple_vdo_on_input_number},
    {SID_SIMPLE_SEARCH_VDO_REQ, MSG_SELECT, (SFUNC) StcSimple_is_on_page_num, (SFUNA) StaSimple_vdo_on_page_num_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_page_num, (SFUNA) StaSimple_vdo_on_page_num_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaSimple_resource_on_search_pressed},
    {SID_SIMPLE_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaSimple_vdo_on_input_completed},
};

static const STATE_TRANS_T SttSimple_SearchUrlReq[] =
{/*016|        |--+ SID_SIMPLE_SEARCH_URL_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, NULL, (SFUNA) StaSimple_vdo_popu_on_newplayurl_arrived},
    {SID_SIMPLE_SEARCH_VDO_LIST, MSG_UPDATE, NULL, (SFUNA) StaSimple_vdo_on_subwindow_exit},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PLAY_URLS_FAILED, NULL, (SFUNA) StaSimple_vdo_popu_on_get_playurl_failed},
    {SID_SIMPLE_SEARCH_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttSimple_VdoInfo[] =
{/*017|     |--+ SID_SIMPLE_VDO_INFO */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_info_detail, (SFUNA) StaSimple_vdo_info_on_infodetail_focus_key_hldr},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_info_detail, (SFUNA) StaSimple_vdo_info_on_infodetail_exit},
};

static const STATE_TRANS_T SttSimple_Resolution[] =
{/*018|     |--+ SID_SIMPLE_RESOLUTION */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcSimple_is_on_opt_list, (SFUNA) StaSimple_resolution_on_optlist_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcSimple_is_on_opt_list, (SFUNA) StaSimple_resolution_on_optlist_select},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcSimple_is_on_opt_list, (SFUNA) StaSimple_resolution_on_optlist_exit},
};

static const STATE_TRANS_T SttSimple_Deinit[] =
{/*020|  |--+ SID_SIMPLE_DEINIT */
    {SID_SIMPLE_INACTIVE, MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS, NULL, (SFUNA) StaSimple_deinit_on_deinit_success},
    {SID_SIMPLE_INACTIVE, MSG_NETMEDIA_WEBSITE_DEINIT_FAILED, NULL, (SFUNA) StaSimple_deinit_on_deinit_success},
};

//State tree
static const STATE_NODE_T pStateTree_Simple[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_SIMPLE */ {SID_NULL, SID_SIMPLE_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_SIMPLE_INACTIVE */ {SID_SIMPLE, SID_NULL, (SFUNE) SenSimple_Inactive, (SFUNE) SexSimple_Inactive, SttSimple_Inactive, sizeof(SttSimple_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_SIMPLE_ACTIVE */ {SID_SIMPLE, SID_SIMPLE_INIT, (SFUNE) SenSimple_Active, (SFUNE) SexSimple_Active, SttSimple_Active, sizeof(SttSimple_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_SIMPLE_INIT */ {SID_SIMPLE_ACTIVE, SID_NULL, (SFUNE) SenSimple_Init, (SFUNE) SexSimple_Init, SttSimple_Init, sizeof(SttSimple_Init)/sizeof(STATE_TRANS_T)},
/*005|  |--+ SID_SIMPLE_RESOURCE */ {SID_SIMPLE_ACTIVE, SID_SIMPLE_SUBCLASS, (SFUNE) SenSimple_Resource, (SFUNE) SexSimple_Resource, SttSimple_Resource, sizeof(SttSimple_Resource)/sizeof(STATE_TRANS_T)},
/*006|     |--+ SID_SIMPLE_SUBCLASS */ {SID_SIMPLE_RESOURCE, SID_SIMPLE_SUBCLASS_REQ, (SFUNE) SenSimple_Subclass, (SFUNE) SexSimple_Subclass, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*007|        |--+ SID_SIMPLE_SUBCLASS_REQ */ {SID_SIMPLE_SUBCLASS, SID_NULL, (SFUNE) SenSimple_SubclassReq, (SFUNE) SexSimple_SubclassReq, SttSimple_SubclassReq, sizeof(SttSimple_SubclassReq)/sizeof(STATE_TRANS_T)},
/*008|        |--+ SID_SIMPLE_SUBCLASS_LIST */ {SID_SIMPLE_SUBCLASS, SID_NULL, (SFUNE) SenSimple_SubclassList, (SFUNE) SexSimple_SubclassList, SttSimple_SubclassList, sizeof(SttSimple_SubclassList)/sizeof(STATE_TRANS_T)},
/*009|     |--+ SID_SIMPLE_VDO */ {SID_SIMPLE_RESOURCE, SID_SIMPLE_VDO_REQ, (SFUNE) SenSimple_Vdo, (SFUNE) SexSimple_Vdo, SttSimple_Vdo, sizeof(SttSimple_Vdo)/sizeof(STATE_TRANS_T)},
/*010|        |--+ SID_SIMPLE_VDO_REQ */ {SID_SIMPLE_VDO, SID_NULL, (SFUNE) SenSimple_VdoReq, (SFUNE) SexSimple_VdoReq, SttSimple_VdoReq, sizeof(SttSimple_VdoReq)/sizeof(STATE_TRANS_T)},
/*011|        |--+ SID_SIMPLE_VDO_LIST */ {SID_SIMPLE_VDO, SID_NULL, (SFUNE) SenSimple_VdoList, (SFUNE) SexSimple_VdoList, SttSimple_VdoList, sizeof(SttSimple_VdoList)/sizeof(STATE_TRANS_T)},
/*012|        |--+ SID_SIMPLE_VDO_URL_REQ */ {SID_SIMPLE_VDO, SID_NULL, (SFUNE) SenSimple_VdoUrlReq, (SFUNE) SexSimple_VdoUrlReq, SttSimple_VdoUrlReq, sizeof(SttSimple_VdoUrlReq)/sizeof(STATE_TRANS_T)},
/*013|     |--+ SID_SIMPLE_SEARCH */ {SID_SIMPLE_RESOURCE, SID_SIMPLE_SEARCH_VDO_REQ, (SFUNE) SenSimple_Search, (SFUNE) SexSimple_Search, SttSimple_Search, sizeof(SttSimple_Search)/sizeof(STATE_TRANS_T)},
/*014|        |--+ SID_SIMPLE_SEARCH_VDO_REQ */ {SID_SIMPLE_SEARCH, SID_NULL, (SFUNE) SenSimple_SearchVdoReq, (SFUNE) SexSimple_SearchVdoReq, SttSimple_SearchVdoReq, sizeof(SttSimple_SearchVdoReq)/sizeof(STATE_TRANS_T)},
/*015|        |--+ SID_SIMPLE_SEARCH_VDO_LIST */ {SID_SIMPLE_SEARCH, SID_NULL, (SFUNE) SenSimple_SearchVdoList, (SFUNE) SexSimple_SearchVdoList, SttSimple_SearchVdoList, sizeof(SttSimple_SearchVdoList)/sizeof(STATE_TRANS_T)},
/*016|        |--+ SID_SIMPLE_SEARCH_URL_REQ */ {SID_SIMPLE_SEARCH, SID_NULL, (SFUNE) SenSimple_SearchUrlReq, (SFUNE) SexSimple_SearchUrlReq, SttSimple_SearchUrlReq, sizeof(SttSimple_SearchUrlReq)/sizeof(STATE_TRANS_T)},
/*017|     |--+ SID_SIMPLE_VDO_INFO */ {SID_SIMPLE_RESOURCE, SID_NULL, (SFUNE) SenSimple_VdoInfo, (SFUNE) SexSimple_VdoInfo, SttSimple_VdoInfo, sizeof(SttSimple_VdoInfo)/sizeof(STATE_TRANS_T)},
/*018|     |--+ SID_SIMPLE_RESOLUTION */ {SID_SIMPLE_RESOURCE, SID_NULL, (SFUNE) SenSimple_Resolution, (SFUNE) SexSimple_Resolution, SttSimple_Resolution, sizeof(SttSimple_Resolution)/sizeof(STATE_TRANS_T)},
/*019|  |--+ SID_SIMPLE_INIT_FAILED */ {SID_SIMPLE_ACTIVE, SID_NULL, (SFUNE) SenSimple_InitFailed, (SFUNE) SexSimple_InitFailed, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*020|  |--+ SID_SIMPLE_DEINIT */ {SID_SIMPLE_ACTIVE, SID_NULL, (SFUNE) SenSimple_Deinit, (SFUNE) SexSimple_Deinit, SttSimple_Deinit, sizeof(SttSimple_Deinit)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Simple = {
    pStateTree_Simple,
    "APP_SIMPLE_T",
    sizeof(pStateTree_Simple)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Simple_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Simple));
}

s32 Simple_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Simple));
}

BOOL Simple_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Simple), stateID);
}

s32 Simple_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Simple), ctrl, msg, para1, para2);
}
