/*******************************************************************************
 **
 ** Copyright (c) 2013, xxx.
 ** All rights reserved.
 **
 ** Document Name: States_Classic.h
 ** Abstract     :
 **
 ** Current Ver  : 
 ** Author       : shaowenrong
 ** Date         : 2014-9-9 17:47:07
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
/*001+*/ SID_CLASSIC = SID_ROOT,
/*002|--+*/ SID_CLASSIC_INACTIVE,
/*003|--+*/ SID_CLASSIC_ACTIVE,
/*004|  |--+*/ SID_CLASSIC_INIT,
/*005|  |--+*/ SID_CLASSIC_RESOURCE,
/*006|     |--+*/ SID_CLASSIC_SUBCLASS,
/*007|        |--+*/ SID_CLASSIC_SUBCLASS_REQ,
/*008|        |--+*/ SID_CLASSIC_SUBCLASS_LIST,
/*009|     |--+*/ SID_CLASSIC_VDO,
/*010|        |--+*/ SID_CLASSIC_VDO_REQ,
/*011|        |--+*/ SID_CLASSIC_VDO_LIST,
/*012|        |--+*/ SID_CLASSIC_VDO_URL_REQ,
/*013|     |--+*/ SID_CLASSIC_SEARCH,
/*014|        |--+*/ SID_CLASSIC_SEARCH_VDO_REQ,
/*015|        |--+*/ SID_CLASSIC_SEARCH_VDO_LIST,
/*016|        |--+*/ SID_CLASSIC_SEARCH_URL_REQ,
/*017|     |--+*/ SID_CLASSIC_RESOLUTION,
/*018|  |--+*/ SID_CLASSIC_INIT_FAILED,
/*019|  |--+*/ SID_CLASSIC_DEINIT,
};

//Prototype of state entry, transition condition and action, and exit
static void SenClassic_Inactive(void);
static STATEID StaClassic_inactive_on_open_classic_req(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Inactive(void);
static void SenClassic_Active(void);
static STATEID StaClassic_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Active(void);
static void SenClassic_Init(void);
static STATEID StaClassic_init_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_init_on_newclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Init(void);
static void SenClassic_Resource(void);
static BOOL    StcClassic_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resource_on_classlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_class_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resource_on_classlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Resource(void);
static void SenClassic_Subclass(void);
static void SexClassic_Subclass(void);
static void SenClassic_SubclassReq(void);
static STATEID StaClassic_subclass_on_newsubclasslist_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_get_subclasslist_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_SubclassReq(void);
static void SenClassic_SubclassList(void);
static BOOL    StcClassic_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_subclasslist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_subclasslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_subclass_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_subclasslist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_subclass_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_SubclassList(void);
static void SenClassic_Vdo(void);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Vdo(void);
static void SenClassic_VdoReq(void);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_newpagevdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_VdoReq(void);
static void SenClassic_VdoList(void);
static STATEID StaClassic_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_VdoList(void);
static void SenClassic_VdoUrlReq(void);
static STATEID StaClassic_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_VdoUrlReq(void);
static void SenClassic_Search(void);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_vdolist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Search(void);
static void SenClassic_SearchVdoReq(void);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_newsearchvdo_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_get_pagevdo_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_SearchVdoReq(void);
static void SenClassic_SearchVdoList(void);
static STATEID StaClassic_vdo_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_vdo_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_search_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resource_on_search_pressed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_input_completed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_SearchVdoList(void);
static void SenClassic_SearchUrlReq(void);
static STATEID StaClassic_vdo_popu_on_newplayurl_arrived(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_vdo_popu_on_get_playurl_failed(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_SearchUrlReq(void);
static void SenClassic_Resolution(void);
static BOOL    StcClassic_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resolution_on_optlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resolution_on_optlist_select(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static BOOL    StcClassic_is_on_opt_list(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_resolution_on_optlist_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Resolution(void);
static void SenClassic_InitFailed(void);
static void SexClassic_InitFailed(void);
static void SenClassic_Deinit(void);
static STATEID StaClassic_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static STATEID StaClassic_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2);
static void SexClassic_Deinit(void);

//State transition table
static const STATE_TRANS_T SttClassic_Inactive[] =
{/*002|--+ SID_CLASSIC_INACTIVE */
    {SID_INTERNAL, MSG_OPEN_CLASSIC_REQ, NULL, (SFUNA) StaClassic_inactive_on_open_classic_req},
};

static const STATE_TRANS_T SttClassic_Active[] =
{/*003|--+ SID_CLASSIC_ACTIVE */
    {SID_CLASSIC_INACTIVE, MSG_SAVE, NULL, (SFUNA) StaClassic_active_on_quick_exit},
    {SID_INTERNAL, MSG_INTERNET_PLUG_OUT, NULL, (SFUNA) StaClassic_active_on_exit},
    {SID_INTERNAL, MSG_EXIT, NULL, (SFUNA) StaClassic_active_on_exit},
    {SID_INTERNAL, MSG_EXIT_ALL, NULL, (SFUNA) StaClassic_active_on_exit},
};

static const STATE_TRANS_T SttClassic_Init[] =
{/*004|  |--+ SID_CLASSIC_INIT */
    {SID_INTERNAL, MSG_NETMEDIA_WEBSITE_INIT_SUCCESS, NULL, (SFUNA) StaClassic_init_on_init_success},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_CLASS_LIST_ARRIVED, NULL, (SFUNA) StaClassic_init_on_newclasslist_arrived},
    {SID_CLASSIC_INIT_FAILED, MSG_NETMEDIA_WEBSITE_INIT_FAILED, NULL, NULL},
    {SID_CLASSIC_INIT_FAILED, MSG_NETMEDIA_GET_CLASS_LIST_FAILED, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_Resource[] =
{/*005|  |--+ SID_CLASSIC_RESOURCE */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_class_list, (SFUNA) StaClassic_resource_on_classlist_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcClassic_is_on_class_list, (SFUNA) StaClassic_resource_on_classlist_selected},
};

static const STATE_TRANS_T SttClassic_SubclassReq[] =
{/*007|        |--+ SID_CLASSIC_SUBCLASS_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_SUBCLASS_LIST_ARRIVED, NULL, (SFUNA) StaClassic_subclass_on_newsubclasslist_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_SUBCLASS_LIST_FAILED, NULL, (SFUNA) StaClassic_subclass_on_get_subclasslist_failed},
    {SID_CLASSIC_SUBCLASS_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_SubclassList[] =
{/*008|        |--+ SID_CLASSIC_SUBCLASS_LIST */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_subclass_list, (SFUNA) StaClassic_subclass_on_subclasslist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcClassic_is_on_subclass_list, (SFUNA) StaClassic_subclass_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcClassic_is_on_subclass_list, (SFUNA) StaClassic_subclass_on_subclasslist_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_subclass_list, (SFUNA) StaClassic_subclass_on_subclasslist_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaClassic_resource_on_search_pressed},
    {SID_CLASSIC_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaClassic_subclass_on_input_completed},
};

static const STATE_TRANS_T SttClassic_Vdo[] =
{/*009|     |--+ SID_CLASSIC_VDO */
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_vdolist_exit},
};

static const STATE_TRANS_T SttClassic_VdoReq[] =
{/*010|        |--+ SID_CLASSIC_VDO_REQ */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_change_page},
    {SID_CLASSIC_VDO_LIST, MSG_NETMEDIA_NEW_PAGE_VDO_ARRIVED, NULL, (SFUNA) StaClassic_vdo_on_newpagevdo_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PAGE_VDO_FAILED, NULL, (SFUNA) StaClassic_vdo_on_get_pagevdo_failed},
    {SID_CLASSIC_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_VdoList[] =
{/*011|        |--+ SID_CLASSIC_VDO_LIST */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaClassic_vdo_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_vdo_on_vdolist_selected},
    {SID_INTERNAL, MSG_NUMBER, NULL, (SFUNA) StaClassic_vdo_on_input_number},
    {SID_CLASSIC_VDO_REQ, MSG_SELECT, (SFUNC) StcClassic_is_on_page_num, (SFUNA) StaClassic_vdo_on_page_num_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_page_num, (SFUNA) StaClassic_vdo_on_page_num_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaClassic_resource_on_search_pressed},
    {SID_CLASSIC_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaClassic_vdo_on_input_completed},
};

static const STATE_TRANS_T SttClassic_VdoUrlReq[] =
{/*012|        |--+ SID_CLASSIC_VDO_URL_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, NULL, (SFUNA) StaClassic_vdo_popu_on_newplayurl_arrived},
    {SID_CLASSIC_VDO_LIST, MSG_UPDATE, NULL, (SFUNA) StaClassic_vdo_on_subwindow_exit},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PLAY_URLS_FAILED, NULL, (SFUNA) StaClassic_vdo_popu_on_get_playurl_failed},
    {SID_CLASSIC_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_Search[] =
{/*013|     |--+ SID_CLASSIC_SEARCH */
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_vdolist_exit},
};

static const STATE_TRANS_T SttClassic_SearchVdoReq[] =
{/*014|        |--+ SID_CLASSIC_SEARCH_VDO_REQ */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_change_page},
    {SID_INTERNAL, MSG_NETMEDIA_NEW_SEARCH_VDO_ARRIVED, NULL, (SFUNA) StaClassic_search_on_newsearchvdo_arrived},
    {SID_INTERNAL, MSG_NETMEDIA_GET_SEARCH_VDO_FAILED, NULL, (SFUNA) StaClassic_vdo_on_get_pagevdo_failed},
    {SID_CLASSIC_SEARCH_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_SearchVdoList[] =
{/*015|        |--+ SID_CLASSIC_SEARCH_VDO_LIST */
    {SID_INTERNAL, MSG_PIC_EVT_DRAW_END, NULL, (SFUNA) StaClassic_vdo_pic_draw_end},
    {SID_INTERNAL, MSG_PIC_EVT_TOO_LARGE, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_UNSUPPORT, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_PIC_EVT_DATA_ERROR, NULL, (SFUNA) StaClassic_vdo_pic_draw_fail},
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_vdolist_focus_key_hldr},
    {SID_INTERNAL, MSG_PAGE_KEY, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_change_page},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcClassic_is_on_vdo_list, (SFUNA) StaClassic_search_on_vdolist_selected},
    {SID_INTERNAL, MSG_NUMBER, NULL, (SFUNA) StaClassic_vdo_on_input_number},
    {SID_CLASSIC_SEARCH_VDO_REQ, MSG_SELECT, (SFUNC) StcClassic_is_on_page_num, (SFUNA) StaClassic_vdo_on_page_num_selected},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_page_num, (SFUNA) StaClassic_vdo_on_page_num_exit},
    {SID_INTERNAL, MSG_SEARCH, NULL, (SFUNA) StaClassic_resource_on_search_pressed},
    {SID_CLASSIC_SEARCH, MSG_INPUT_COMP, NULL, (SFUNA) StaClassic_vdo_on_input_completed},
};

static const STATE_TRANS_T SttClassic_SearchUrlReq[] =
{/*016|        |--+ SID_CLASSIC_SEARCH_URL_REQ */
    {SID_INTERNAL, MSG_NETMEDIA_NEW_PLAY_URLS_ARRIVED, NULL, (SFUNA) StaClassic_vdo_popu_on_newplayurl_arrived},
    {SID_CLASSIC_SEARCH_VDO_LIST, MSG_UPDATE, NULL, (SFUNA) StaClassic_vdo_on_subwindow_exit},
    {SID_INTERNAL, MSG_NETMEDIA_GET_PLAY_URLS_FAILED, NULL, (SFUNA) StaClassic_vdo_popu_on_get_playurl_failed},
    {SID_CLASSIC_SEARCH_VDO_LIST, MSG_CLOSE_CFMDLG_NTF, NULL, NULL},
};

static const STATE_TRANS_T SttClassic_Resolution[] =
{/*017|     |--+ SID_CLASSIC_RESOLUTION */
    {SID_INTERNAL, MSG_FOCUS_KEY, (SFUNC) StcClassic_is_on_opt_list, (SFUNA) StaClassic_resolution_on_optlist_focus_key_hldr},
    {SID_INTERNAL, MSG_SELECT, (SFUNC) StcClassic_is_on_opt_list, (SFUNA) StaClassic_resolution_on_optlist_select},
    {SID_INTERNAL, MSG_EXIT, (SFUNC) StcClassic_is_on_opt_list, (SFUNA) StaClassic_resolution_on_optlist_exit},
};

static const STATE_TRANS_T SttClassic_Deinit[] =
{/*019|  |--+ SID_CLASSIC_DEINIT */
    {SID_CLASSIC_INACTIVE, MSG_NETMEDIA_WEBSITE_DEINIT_SUCCESS, NULL, (SFUNA) StaClassic_deinit_on_deinit_success},
    {SID_CLASSIC_INACTIVE, MSG_NETMEDIA_WEBSITE_DEINIT_FAILED, NULL, (SFUNA) StaClassic_deinit_on_deinit_success},
};

//State tree
static const STATE_NODE_T pStateTree_Classic[] =
{
    {SID_NULL, SID_ROOT, NULL, NULL, NULL, 0},
/*001+ SID_CLASSIC */ {SID_NULL, SID_CLASSIC_INACTIVE, NULL, NULL, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*002|--+ SID_CLASSIC_INACTIVE */ {SID_CLASSIC, SID_NULL, (SFUNE) SenClassic_Inactive, (SFUNE) SexClassic_Inactive, SttClassic_Inactive, sizeof(SttClassic_Inactive)/sizeof(STATE_TRANS_T)},
/*003|--+ SID_CLASSIC_ACTIVE */ {SID_CLASSIC, SID_CLASSIC_INIT, (SFUNE) SenClassic_Active, (SFUNE) SexClassic_Active, SttClassic_Active, sizeof(SttClassic_Active)/sizeof(STATE_TRANS_T)},
/*004|  |--+ SID_CLASSIC_INIT */ {SID_CLASSIC_ACTIVE, SID_NULL, (SFUNE) SenClassic_Init, (SFUNE) SexClassic_Init, SttClassic_Init, sizeof(SttClassic_Init)/sizeof(STATE_TRANS_T)},
/*005|  |--+ SID_CLASSIC_RESOURCE */ {SID_CLASSIC_ACTIVE, SID_CLASSIC_SUBCLASS, (SFUNE) SenClassic_Resource, (SFUNE) SexClassic_Resource, SttClassic_Resource, sizeof(SttClassic_Resource)/sizeof(STATE_TRANS_T)},
/*006|     |--+ SID_CLASSIC_SUBCLASS */ {SID_CLASSIC_RESOURCE, SID_CLASSIC_SUBCLASS_REQ, (SFUNE) SenClassic_Subclass, (SFUNE) SexClassic_Subclass, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*007|        |--+ SID_CLASSIC_SUBCLASS_REQ */ {SID_CLASSIC_SUBCLASS, SID_NULL, (SFUNE) SenClassic_SubclassReq, (SFUNE) SexClassic_SubclassReq, SttClassic_SubclassReq, sizeof(SttClassic_SubclassReq)/sizeof(STATE_TRANS_T)},
/*008|        |--+ SID_CLASSIC_SUBCLASS_LIST */ {SID_CLASSIC_SUBCLASS, SID_NULL, (SFUNE) SenClassic_SubclassList, (SFUNE) SexClassic_SubclassList, SttClassic_SubclassList, sizeof(SttClassic_SubclassList)/sizeof(STATE_TRANS_T)},
/*009|     |--+ SID_CLASSIC_VDO */ {SID_CLASSIC_RESOURCE, SID_CLASSIC_VDO_REQ, (SFUNE) SenClassic_Vdo, (SFUNE) SexClassic_Vdo, SttClassic_Vdo, sizeof(SttClassic_Vdo)/sizeof(STATE_TRANS_T)},
/*010|        |--+ SID_CLASSIC_VDO_REQ */ {SID_CLASSIC_VDO, SID_NULL, (SFUNE) SenClassic_VdoReq, (SFUNE) SexClassic_VdoReq, SttClassic_VdoReq, sizeof(SttClassic_VdoReq)/sizeof(STATE_TRANS_T)},
/*011|        |--+ SID_CLASSIC_VDO_LIST */ {SID_CLASSIC_VDO, SID_NULL, (SFUNE) SenClassic_VdoList, (SFUNE) SexClassic_VdoList, SttClassic_VdoList, sizeof(SttClassic_VdoList)/sizeof(STATE_TRANS_T)},
/*012|        |--+ SID_CLASSIC_VDO_URL_REQ */ {SID_CLASSIC_VDO, SID_NULL, (SFUNE) SenClassic_VdoUrlReq, (SFUNE) SexClassic_VdoUrlReq, SttClassic_VdoUrlReq, sizeof(SttClassic_VdoUrlReq)/sizeof(STATE_TRANS_T)},
/*013|     |--+ SID_CLASSIC_SEARCH */ {SID_CLASSIC_RESOURCE, SID_CLASSIC_SEARCH_VDO_REQ, (SFUNE) SenClassic_Search, (SFUNE) SexClassic_Search, SttClassic_Search, sizeof(SttClassic_Search)/sizeof(STATE_TRANS_T)},
/*014|        |--+ SID_CLASSIC_SEARCH_VDO_REQ */ {SID_CLASSIC_SEARCH, SID_NULL, (SFUNE) SenClassic_SearchVdoReq, (SFUNE) SexClassic_SearchVdoReq, SttClassic_SearchVdoReq, sizeof(SttClassic_SearchVdoReq)/sizeof(STATE_TRANS_T)},
/*015|        |--+ SID_CLASSIC_SEARCH_VDO_LIST */ {SID_CLASSIC_SEARCH, SID_NULL, (SFUNE) SenClassic_SearchVdoList, (SFUNE) SexClassic_SearchVdoList, SttClassic_SearchVdoList, sizeof(SttClassic_SearchVdoList)/sizeof(STATE_TRANS_T)},
/*016|        |--+ SID_CLASSIC_SEARCH_URL_REQ */ {SID_CLASSIC_SEARCH, SID_NULL, (SFUNE) SenClassic_SearchUrlReq, (SFUNE) SexClassic_SearchUrlReq, SttClassic_SearchUrlReq, sizeof(SttClassic_SearchUrlReq)/sizeof(STATE_TRANS_T)},
/*017|     |--+ SID_CLASSIC_RESOLUTION */ {SID_CLASSIC_RESOURCE, SID_NULL, (SFUNE) SenClassic_Resolution, (SFUNE) SexClassic_Resolution, SttClassic_Resolution, sizeof(SttClassic_Resolution)/sizeof(STATE_TRANS_T)},
/*018|  |--+ SID_CLASSIC_INIT_FAILED */ {SID_CLASSIC_ACTIVE, SID_NULL, (SFUNE) SenClassic_InitFailed, (SFUNE) SexClassic_InitFailed, NULL, sizeof(NULL)/sizeof(STATE_TRANS_T)},
/*019|  |--+ SID_CLASSIC_DEINIT */ {SID_CLASSIC_ACTIVE, SID_NULL, (SFUNE) SenClassic_Deinit, (SFUNE) SexClassic_Deinit, SttClassic_Deinit, sizeof(SttClassic_Deinit)/sizeof(STATE_TRANS_T)},
};

static STATE_TREE_INFO_T pStateTreeInfo_Classic = {
    pStateTree_Classic,
    "APP_CLASSIC_T",
    sizeof(pStateTree_Classic)/sizeof(STATE_NODE_T),
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

s32 Classic_OpenStateTree(void)
{
    return SM_OpenStateTree(&(pStateTreeInfo_Classic));
}

s32 Classic_CloseStateTree(void)
{
    return SM_CloseStateTree(&(pStateTreeInfo_Classic));
}

BOOL Classic_IsStateActive(STATEID stateID)
{
    return SM_IsStateActive(&(pStateTreeInfo_Classic), stateID);
}

s32 Classic_DispatchMsg(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return SM_DispatchMsg(&(pStateTreeInfo_Classic), ctrl, msg, para1, para2);
}
