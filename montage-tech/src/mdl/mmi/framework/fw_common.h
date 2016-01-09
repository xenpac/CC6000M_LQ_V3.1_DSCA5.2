/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __FW_COMMON_H__
#define __FW_COMMON_H__
/////////////////////////////////////////////////////////////////////////////
// macro definition
/////////////////////////////////////////////////////////////////////////////

/*!
   The macro is defined to enable debug print information or NOT.
  */

/*!
   Macro for debug
  */
#define FW_PRINTF DUMMY_PRINTF

/*!
   This macro is defined to identify the message is belong to ui.
  */
#define FW_INTERNAL_MSG    0x07FFFFFF

/*!
   To calculate the maximal count of apps.
  */
#define MAX_AP_CNT    (APP_LAST - APP_FRAMEWORK)

/*!
   The mainwin is in idle
   */
#define QS_IDLE           0x0

/*!
   The mainwin is in opening
   */
#define QS_OPEN           0x1000

/*!
   The mainwin is in quiting
  */
#define QS_QUIT           0x2000
/*!
   The mainwin is in painting
  */
#define QS_PAINT          0x3000

/*!
   Makes the queue state.
  */
#define MAKE_QS(root_id, s) ((root_id << 16) | s)

/*!
   Gets the root from a queue state.
  */
#define GET_QS_ROOT(qs) ((u16)(qs >> 16))

/*!
   Gets the queue state.
  */
#define GET_QS_STATE(qs) ((u16)(qs & 0xFFFF))

/*!
   Sets the queue state of framework.
  */
#define FW_RESET_STATE(root_id) \
  do { \
    fw_main_t *p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID); \
    MT_ASSERT(p_info != NULL); \
    p_info->infor.msg_q_state = MAKE_QS(root_id, QS_IDLE); \
  } while(0);

/*!
   Sets the queue state of framework.
  */
#define FW_SET_STATE(root_id, state) \
  do { \
    fw_main_t *p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID); \
    MT_ASSERT(p_info != NULL); \
    if(GET_QS_ROOT(p_info->infor.msg_q_state) == root_id) \
    { \
      p_info->infor.msg_q_state = MAKE_QS(root_id, state); \
    } \
  } while(0);

/*!
   Invokes the window manage.
  */
#define FW_MANAGEMENT(event, para1, para2) \
  do { \
    fw_main_t *p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID); \
    MT_ASSERT(p_info != NULL); \
    if(p_info->infor.manage != NULL) \
    { \
      p_info->infor.manage(event, para1, para2); \
    } \
  } while(0);

/////////////////////////////////////////////////////////////////////////////
// end of macro definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// structure definition
/////////////////////////////////////////////////////////////////////////////

/*!
   The message type
  */
typedef enum
{
  /*!
     A message from UIO.
    */
  MSG_T_UIO = 1,
  /*!
     A message from an app.
    */
  MSG_T_APP,
  /*!
     A message from an expired timer.
    */
  MSG_T_TMR,
  /*!
     A notify message.
    */
  MSG_T_INT
}fw_msg_type_t;

/*!
   The structure of msg external part
  */
typedef struct
{
  /*!
     The type
    */
  u8 type;
  /*!
     The content
    */
  u16 content;
  /*!
     The host, for MSG_T_APP it is an app id.
    */
  u16 host;
  /*!
     The destination
    */
  u32 dest;
  /*!
     The first parameter
    */
  u32 para1;
  /*!
     The second parameter
    */
  u32 para2;
}fw_msg_ext_t;

/*!
   The structure of framework message
  */
typedef struct
{
  /*!
     The common message header
    */
  os_msg_t head;
  /*!
     The extend part
    */
  fw_msg_ext_t ext;
}fw_msg_t;

/*!
   The structure of a mainwin
  */
typedef struct mainwin
{
  /*!
     The style
    */
  u8 style;
  /*!
     The root container
    */
  control_t *p_root;
  /*!
     The parent
    */
  struct mainwin *p_parent;
  /*!
     The child
    */
  struct mainwin *p_child;
  /*!
     The next
    */
  struct mainwin *p_next;
  /*!
     The previous
    */
  struct mainwin *p_prev;
  /*!
     The global clip region information
    */
  crgn_info_t gcrgn_info;
  /*!
    animation in.
    */
  anim_in_t anim_in;
  /*!
    animation out.
    */
  anim_out_t anim_out;    
}mainwin_t;


/*!
   The structure of a z-node
  */
typedef struct znode
{
  /*!
     Handle of the mainwin
    */
  mainwin_t *p_host;
  /*!
     The previous node
    */
  struct znode *p_prev;
  /*!
     The next node
    */
  struct znode *p_next;
  /*!
     The flag of updating
    */
  BOOL is_need_update;
}znode_t;

/*!
   The structure of a z-list
  */
typedef struct
{
  /*!
     The count of znode
    */
  u32 cnt;
  /*!
     The top node on z-order
    */
  znode_t *p_top;
}zlist_t;


/*!
   The structure of the message host
  */
typedef struct ap_msghost
{
  /*!
     The host id
    */
  u16 root_id;
  /*!
     Points to the next
    */
  struct ap_msghost *p_next;
}ap_msghost_t;


/*!
   The structure of the entry of app's event
  */
typedef struct
{
  /*!
     The app id
    */
  u16 ap_id;
  /*!
     The event map
    */
  ap_evtmap_t evtmap;
  /*!
     The event proc
    */
  ap_evtproc_t evtproc;
  /*!
     The first host
    */
  ap_msghost_t *p_first_host;
}ap_evt_entry_t;

/*!
   The structure of the entry of ui's timer
  */
typedef struct
{
  /*!
     The timer id
    */
  s32 tmr;
  /*!
     The owner of timer (root_id)
    */
  u16 owner;
  /*!
     The notify content of timer
    */
  u16 notify;
  /*!
     The flag determines whether be circle or NOT
    */
  BOOL is_circle;
  /*!
     The flag that whether be used or NOT
    */
  BOOL is_used;
}ui_tmr_entry_t;

/*!
   The structure is defined to descript the global information of framework
  */
typedef struct
{
  /*!
     The first hosted mainwin, it should be point to desktop
    */
  mainwin_t *p_first_hosted;
  /*!
     The msg queue of receiving message
    */
  s32 msg_q_id;
  /*!
     The message queue buffer address
    */
  u32 msg_q_buf;
  /*!
     The message queue state
    */
  u32 msg_q_state;
  /*!
     The manage mainwin method
    */
  window_manage_t manage;
  /*!
     The app event entrys
    */
  ap_evt_entry_t ap_evt_entrys[MAX_AP_CNT];
  /*!
     The ui timer entrys
    */
  ui_tmr_entry_t *p_ui_tmr_entrys;
  /*!
     The buffer object of the msghost
    */
  lib_memf_t ap_msghost_heap;
  /*!
     The buffer address of the msghost
    */
  void *p_ap_msghost_addr;
  /*!
     The list of normal z-node(mainwin)
    */
  zlist_t zl_normal;
  /*!
     The list of topmost z-node(mainwin)
    */
  zlist_t zl_topmost;
  /*!
     The znode on focus
    */
  znode_t *p_focus;
  /*!
     The buffer object of the mainwin
    */
  lib_memf_t mainwin_heap;
  /*!
     The buffer address of the mainwin
    */
  void *p_mainwin_addr;
  /*!
     The buffer object of the znodes
    */
  lib_memf_t znode_heap;
  /*!
     The buffer address of the znodes
    */
  void *p_znode_addr;
  /*!
    is worker task working.
    */
  BOOL is_working;
  /*!
    worker root id.
    */
  u16 worker_root;   
  /*!
    worker context
    */
  u32 worker_context;    
  /*!
    worker stack.
    */
  void *p_worker_stk; 
  /*!
    worker proc
    */
  worker_proc_t wproc;    
}fw_infor_t;

/*!
   This structure is defined to descript the global information of the module.
  */
typedef struct
{
  /*!
     The information of the framework.
    */
  fw_infor_t infor;
  /*!
     The configuration of the framework.
    */
  fw_config_t config;
}fw_main_t;


/////////////////////////////////////////////////////////////////////////////
// end of structure definition
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// z-order operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Initialize the global clipping region for a znode's host.

   \param[in] p_node Points to a znode.
  */
void znode_init_gcrgn(znode_t *p_node);

/*!
   Subtracts a rectangle from a global clipping region of a znode's host.

   \param[in] p_node Points to a znode.
   \param[in] rc Points to a rectangle.
  */
void znode_subtract_gcrgn(znode_t *p_node, rect_t *p_rc);

/*!
   Determines whether the znode is belong to topmost list.

   \param[in] p_node Points to a znode.
  */
BOOL znode_is_topmost(znode_t *p_node);

/*!
   Sets the style for a znode's host.

   \param[in] p_node Points to a znode.
   \param[in] new_style Specifies the new style to be set.
  */
void znode_set_style(znode_t *p_node, obj_attr_t new_style);

/*!
   Adds a znode onto a zlist.

   \param[in] p_list Points to a zlist.
   \param[in] p_node Points to a znode..
  */
void zlist_add_node(zlist_t *p_list, znode_t *p_node);

/*!
   Removes a znode onto a zlist.

   \param[in] p_list Points to a zlist.
   \param[in] p_node Points to a znode..
  */
void zlist_remove_node(zlist_t *p_list, znode_t *p_node);

/*!
   Gets the last znode of a zlist.

   \param[in] p_list Points to a zlist.

   \return If successful return the last node, Otherwise return NULL.
  */
znode_t *zlist_get_the_last(zlist_t *p_list);

/*!
   Gets the root container of a znode's host.

   \param[in] p_node Points to a znode.

   \return If successful return the root container, Otherwise return NULL.
  */
control_t *zlist_get_the_root(znode_t *p_node);

/*!
   Gets the first znode which is activate from a zlist.

   \param[in] p_list Points to a zlist.

   \return If successful return the first znode, Otherwise return NULL.
  */
znode_t *zlist_get_the_first_activable(zlist_t *p_list);

/*!
   Finds a znode which is equal to the specified znode from a zlist.

   \param[in] p_list Points to the zlist.
   \param[in] p_node Points to the specified znode.

   \return If successful return the znode, Otherwise return NULL.
  */
znode_t *zlist_find_node_by_node(zlist_t *p_list, znode_t *p_node);

/*!
   Finds a znode by a specified mainwin from a zlist.

   \param[in] p_list Points to the zlist.
   \param[in] p_win Points to the specified mainwin.

   \return If successful return the znode, Otherwise return NULL.
  */
znode_t *zlist_find_node_by_mainwin(zlist_t *p_list, mainwin_t *p_win);

/*!
   Finds a znode by a specified root container from a zlist.

   \param[in] p_list Points to the zlist.
   \param[in] p_root Points to the specified root container.

   \return If successful return the znode, Otherwise return NULL.
  */
znode_t *zlist_find_node_by_root(zlist_t *p_list, control_t *p_root);

/*!
   Finds a znode by a specified root container's id from a zlist.

   \param[in] p_list Points to the zlist.
   \param[in] root_id The specified root container's id.

   \return If successful return the znode, Otherwise return NULL.
  */
znode_t *zlist_find_node_by_id(zlist_t *p_list, u16 root_id);

/*!
   Clips a zlist by a specified znode.

   \param[in] p_list Points to the zlist.
   \param[in] p_node Points to the specified znode.
  */
void zlist_clip_list_by_node(zlist_t *p_list, znode_t *p_node);

/*!
   Clips the znodes which are under the specified znode.

   \param[in] p_node Points to the specified znode.
  */
void zlist_clip_under_this_node(znode_t *p_node);

/*!
   Clips a znode by the specified zlist.

   \param[in] p_node Points to the node.
   \param[in] p_list Points to the specified list.
  */
void zlist_clip_node_by_list(znode_t *p_node, zlist_t *p_list);

/*!
   Clips a znode by the znodes which are above the znode in a specified zlist.

   \param[in] p_node Points to the node.
   \param[in] p_list Points to the specified list.

   \remark it is must that the znode is in the specified p_list.
  */
void zlist_clip_node_by_all_above_this(znode_t *p_node, zlist_t *p_list);

/////////////////////////////////////////////////////////////////////////////
// end of z-order operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// mainwin operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Finds a mainwin by a specified root container's id.

   \param[in] root_id The specified root container's id.

   \return If successful return the mainwin, Otherwise return NULL.
  */
mainwin_t *fw_find_mainwin_by_id(u16 root_id);

/*!
   Finds a mainwin by a specified root container.

   \param[in] p_root Points to The specified root container.

   \return If successful return the mainwin, Otherwise return NULL.
  */
mainwin_t *fw_find_mainwin_by_root(control_t *p_root);

/////////////////////////////////////////////////////////////////////////////
// end of mainwin operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// message operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Gets a message which sends to the specified root container.

   \param[in] root_id The specified root container's id.
   \param[out] p_msg Points to a buffer to store the message.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_get_msg(u16 root_id, fw_msg_t *p_msg);

/*!
   Empties the message queue.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_empty_msg(void);

/*!
   Posts a timeout message with the specified destination, content, parameters.

   \param[in] dest Specifies the destination.
   \param[in] content Specifies the content.
   \param[in] para1 Specifies the first parameter.
   \param[in] para2 Specifies the second parameter.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_post_tmout(u32 dest, u16 content, u32 para1, u32 para2);

/*!
   Posts a notify message with the specified type, destination, content,
   parameters.

   \param[in] type Specifies the type.
   \param[in] dest Specifies the destination.
   \param[in] content Specifies the content.
   \param[in] para1 Specifies the first parameter.
   \param[in] para2 Specifies the second parameter.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_post_notify(u32 type, u32 dest, u16 content, u32 para1, u32 para2);

/*!
   Dispatch a message.

   \param[in] p_msg Points to the specified message.

   \return If successful return TRUE, Otherwise return FALSE.
  */
RET_CODE fw_dispatch_msg(fw_msg_t *p_msg);

/////////////////////////////////////////////////////////////////////////////
// end of message operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// timer operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Initialize the timer module.
   \param[in] tm_out: time interval for default timer.
   \param[in] owner : owner of the default timer.
  */
void fw_tmr_init(void);

/*!
   Release the timer module.
  */
void fw_tmr_release(void);


/////////////////////////////////////////////////////////////////////////////
// end of timer operations
/////////////////////////////////////////////////////////////////////////////


#endif
