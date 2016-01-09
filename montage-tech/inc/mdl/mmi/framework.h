/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__
#ifdef __cplusplus
extern "C" {
#endif
/////////////////////////////////////////////////////////////////////////////
//	window management
/////////////////////////////////////////////////////////////////////////////

/*!
   The id of invalid root.
  */
#define ROOT_ID_INVALID    0

/*!
  root id waitting.
  */
#define ROOT_ID_WAITTING  0xFF00
/*!
   The window management event list.
  */
typedef enum
{
  /*!
     Framework is initialized.
    */
  WINDOW_MANAGE_INIT = 0,
  /*!
     Pre-open a mainwin.

     \param[in] para1 The mainwin id which be opened.
    */
  WINDOW_MANAGE_PRE_OPEN,
  /*!
     Post-open a mainwin.

     \param[in] para1 The mainwin id which be opened.
    */
  WINDOW_MANAGE_POST_OPEN,
  /*!
     Pre-close a mainwin.

     \param[in] para1 The mainwin id which be closed.
    */
  WINDOW_MANAGE_PRE_CLOSE,
  /*!
     Post-close a mainwin.

     \param[in] para1 The mainwin id which be closed.
    */
  WINDOW_MANAGE_POST_CLOSE,
  /*!
    window is closed.

    \param[in] para1 The mainwin id which had be closed.
    */
  WINDOW_MANAGE_CLOSED,
  /*!
     The focus of the mainwins is changed.

     \param[in] para1 The mainwin id which is from.
     \param[in] para1 The mainwin id which is to.
    */
  WINDOW_MANAGE_FOCUS_CHANGED,
  /*!
     An UIO event is captured.

     \param[in] para1 The key content.
    */
  WINDOW_MANAGE_ON_UIO_EVENT,

  /*!
     The end of events.
    */
  WINDOW_MANAGE_EVT_INVALID
} window_manage_evt_t;


/*!
   The type definition of the event processing function

   \param[in] event The event of window management.
   \param[in] para1	The first parameter
   \param[in] para2	The second parameter

   \Return					: the result of processing.
  */
typedef RET_CODE (*window_manage_t)(u32 event, u32 para1, u32 para2);

/*!
  The type definition of default timer call back.
  */
typedef void (*dft_tmr_cb_t)(u32 context);

/*!
   The entry of message mapping
  */
typedef struct
{
  /*!
     The message
    */
  u16 event;
  /*!
     The corresponding process function
    */
  window_manage_t proc;
  /*!
      Reserve for extension
    */
  u32 reserve;
} manage_evtmap_t;

/*!
   This function is only used for define a window_manage_t.

   \param[in] p_entry	Point to the wm_evtmap entry
   \param[in] event	The event
   \param[in] para1	The first parameter
   \param[in] para2	The second parameter
   
   \Return The result of processing.
  */
RET_CODE fw_on_wm_event(const manage_evtmap_t *p_entry,
                        u32 event,
                        u32 para1,
                        u32 para2);

/*!
   This macro is used to define the function which to process the event of 
   window management.
   
   \remark Before use this macro, it is recommended to declare the function
           at first.
  */
#define BEGIN_WINDOW_MANAGE(manage) \
static const manage_evtmap_t manage##_entrys[] = \
{

/*!
   This macro is used to define the relation between event and 
   its processing function.
  */
#define ON_MENU_EVENT(evt, proc) \
  { \
    evt, proc, 0 \
  },

/*!
   This macro is used as the end of the definition.
  */
#define END_WINDOW_MANAGE(manage) \
  { \
    WINDOW_MANAGE_EVT_INVALID, NULL, 0 \
  } \
}; \
RET_CODE manage(u32 event, u32 para1, u32 para2) \
{ \
  return fw_on_wm_event(manage##_entrys, event, para1, para2); \
}

/////////////////////////////////////////////////////////////////////////////
//	end of window management
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	ap event map
/////////////////////////////////////////////////////////////////////////////

/*!
   Define the type of the processing function for a key
  */
typedef u16 (*ap_evtmap_t)(u32 event);

/*!
   The entry of message mapping
  */
typedef struct
{
  /*!
     The event from ap
    */
  u32 ap_evt;
  /*!
     The message which is convert
    */
  u16 ui_msg;
} ap_evtmap_entry_t;


/*!
   This function is only used for define ap_evtmap_t.

   \param[in] p_entry	Point to the event map entry
   \param[in] event	The event
   
   \return The message which is convert from the event.
  */
u16 fw_on_ap_event(const ap_evtmap_entry_t *p_evtmap, u32 event);

/*!
   This macro is used to define the function which to convert an ap event to
   a ui message.
   
   \remark Before use this macro, it is recommended to declare the function
           at first.
  */
#define BEGIN_AP_EVTMAP(evtmap) \
static const ap_evtmap_entry_t evtmap ## _entrys[] = \
{

/*!
   This macro is used to define the relation between event and 
   its corresponding message.
  */
#define CONVERT_EVENT(evt, msg) \
  { \
    evt, msg \
  },

/*!
   This macro is used as the end of the definition.
  */
#define END_AP_EVTMAP(evtmap) \
  { \
    0, MSG_INVALID \
  } \
}; \
u16 evtmap(u32 event) \
{ \
  return fw_on_ap_event(evtmap##_entrys, event); \
}

/////////////////////////////////////////////////////////////////////////////
//	end of ap event map
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	ap event process
/////////////////////////////////////////////////////////////////////////////

/*!
   Define the type of the processing function for a key
  */
typedef RET_CODE (*ap_evtproc_t)(u32 event, u32 para1, u32 para2);

/*!
   The entry of evt process
  */
typedef struct
{
  /*!
     The event from ap
    */
  u32 ap_evt;
  /*!
     The corresponding process function
    */
  ap_evtproc_t p_proc;
} ap_evtproc_entry_t;


/*!
   This function is only used for define ap_evtproc_t.

   \param[in] p_entry	Point to the event map entry
   \param[in] event	The event
   
   \return The message which is convert from the event.
  */
RET_CODE fw_on_ap_event_proc(const ap_evtproc_entry_t *p_entry, u32 event, u32 para1, u32 para2);

/*!
   Evt proc map entrys.
  */
#define BEGIN_AP_EVTPROC(evtproc) \
static const ap_evtproc_entry_t evtproc ## _entrys[] = \
{

/*!
   Evt proc map member.
  */
#define ON_EVENTPROC(evt, p_proc) \
  { \
    evt, p_proc \
  },

/*!
   Evt proc searching interface.
  */
#define END_AP_EVTPROC(evtproc) \
  { \
    0, NULL \
  } \
}; \
RET_CODE evtproc(u32 event, u32 para1, u32 para2) \
{ \
  return fw_on_ap_event_proc(evtproc##_entrys, event, para1, para2); \
}

/////////////////////////////////////////////////////////////////////////////
//	end of ap event proc
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	desktop operations
/////////////////////////////////////////////////////////////////////////////

/*!
   The structure is defined to descript the configuration information of
   the ui framework.
  */
typedef struct
{
  /*!
     The coordinate of the desktop
    */
  rect_t root_rc;
  /*!
     The time for sleeping when nothing to do
    */
  u32 tmout_idle;
  /*!
     The time for recieving message
    */
  u32 tmout_receive_msg;
  /*!
     The time for sending message
    */
  u32 tmout_send_msg;
  /*!
     The id of the desktop
    */
  u8 root_id;
  /*!
     The maximum count of the mainwin (include desktop) which can be opened
     at the same time
    */
  u8 max_win_cnt;
  /*!
     The maximum size of the message queue for receiving message
    */
  u8 max_msg_cnt;
  /*!
     The maximum count of the message host
    */
  u8 max_host_cnt;
  /*!
     The maximum size of the timers
    */
  u8 max_tmr_cnt;
  /*!
     The rectangle style of the desktop
    */
  u32 root_rc_style;
  /*!
    the worker task priority.
    */
  u32 worker_priority;
  /*!
    worker task stack size.
    */
  u32 worker_stk_size;    
} fw_config_t;

/*!
   Initialize the framework.

   \param[in] config Points to the configuration information.
   \param[in] keymap Specifies the keymap of desktop.
   \param[in] proc Specifies the proc of desktop.
   \param[in] manage Specifies the window management.

   \return If successful, return TRUE. Otherwise return FALSE.
  */
BOOL fw_init(fw_config_t *p_config,
             keymap_t keymap,
             msgproc_t proc,
             window_manage_t manage);

/*!
   Release the framework.
  */
void fw_release(void);

/*!
   Determines whether there is nothing.

   \return If there have no mainwin except desktop return TRUE.
          Otherwise return FALSE.
  */
BOOL fw_is_empty(void);

/*!
   Sets a keymap for desktop.

   \param[in] keymap Specifies the keymap to be seted.
  */
void fw_set_keymap(keymap_t keymap);

/*!
   Sets a proc for desktop.

   \param[in] proc Specifies the proc to be seted.
  */
void fw_set_proc(msgproc_t proc);

/*!
   Sets a window management proc.

   \param[in] manage Specifies the manage to be seted.
  */
void fw_set_manage(window_manage_t manage);

/*!
   Gets the root container of desktop.

   \return If successful return the root container. Otherwise return NULL.
  */
control_t *fw_get_root(void);

/*!
   Gets the root container of the focus mainwin.

   \return If successful return the root container. Otherwise return NULL.
  */
control_t *fw_get_focus(void);


/*!
   Sets the root container as the focus mainwin. 

   \return If successful return SUCCESS. Otherwise return ERR_FAILURE.
  */
RET_CODE fw_set_focus(control_t *p_focus);


/*!
   Gets the id of the focus mainwin.

   \return If successful return the id. Otherwise return ROOT_ID_INVALID.
  */
u8 fw_get_focus_id(void);

/*!
   Gets the root container of the focus mainwin.

   \return If successful return the root container. Otherwise return NULL.
  */
control_t *fw_get_first_active(void);

/*!
   Gets the id of the focus mainwin.

   \return If successful return the id. Otherwise return ROOT_ID_INVALID.
  */
u8 fw_get_first_active_id(void);

/*!
   Finds a root container by the specified id.

   \param[in] root_id Specifies the id of the mainwin.

   \return If successful return the root container.
          Otherwise return NULL.
  */
control_t *fw_find_root_by_id(u16 root_id);

/*!
   Creates a mainwin.

   \param[in] root_id Specifies the id of the mainwin.
   \param[in] x Specifies the initial horizontal position of the mainwin.
   \param[in] y Specifies the initial vertical position of the mainwin.
   \param[in] w Specifies the width.
   \param[in] h Specifies the height.
   \param[in] parent_root_id Specifies the id of the parent mainwin.
   \param[in] para Specifies the additional parameter.
   \param[in] attr Specifies the attribute.
   \param[in] style Specifies the style.

   \return If successful return the root container.
          Otherwise return NULL.
  */
control_t *fw_create_mainwin(u16 root_id,
                             u16 x,
                             u16 y,
                             u16 w,
                             u16 h,
                             u16 parent_root_id,
                             u32 para,
                             obj_attr_t attr,
                             u8 style);

/*!
  recaculate gcrgn.
  */
RET_CODE fw_reset_mainwin(u16 root_id);

/*!
  move mainwin.
  
  \param[in] root_id Specifies the id of the mainwin.
  \param[in] x Specifies the initial horizontal position of the mainwin.
  \param[in] y Specifies the initial vertical position of the mainwin.

  \return success or err_failure.
  */
RET_CODE fw_move_mainwin(u16 root_id, u16 x, u16 y);
                             

/*!
   Attachs a root container to a mainwin.

   \param[in] root_id Specifies the id of the mainwin.
   \param[in] parent_root_id Specifies the id of the parent mainwin.

   \return If successful return the root container.
          Otherwise return NULL.
  */
control_t *fw_attach_root(control_t *p_root, u16 parent_root_id);

/*!
   Paints all mainwins.
  */
void fw_paint_all_mainwin(void);

/*!
   Destroies all mainwins.

   \param[in] is_redraw Update view or not.
   
   \return If successful return the root id of desktop, Otherwise return 
   the root id which returns ERR_FAILURE when process the message MSG_SAVE.
  */
u8 fw_destroy_all_mainwin(BOOL is_redraw);

/*!
   Destroies a mainwin by the specified root container.

   \param[in] root Points to a root container.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_destroy_mainwin_by_root(control_t *p_root);

/*!
   Destroies a mainwin by the specified id.

   \param[in] root_id Specifies the id.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_destroy_mainwin_by_id(u16 root_id);

/////////////////////////////////////////////////////////////////////////////
//	end of desktop operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	message operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Registers the event map for an app.

   \param[in] ap_id Specifies the id of an app.
   \param[in] evtmap Specifies the event map.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_register_ap_evtmap(u16 ap_id, ap_evtmap_t evtmap);

/*!
   Unregisters an app's event map.

   \param[in] ap_id Specifies the id of an app.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_unregister_ap_evtmap(u16 ap_id);

/*!
   Registers the event proc for an app.

   \param[in] ap_id Specifies the id of an app.
   \param[in] evtproc Specifies the event proc.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_register_ap_evtproc(u16 ap_id, ap_evtproc_t evtproc);

/*!
   Unregisters an app's event proc.

   \param[in] ap_id Specifies the id of an app.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_unregister_ap_evtproc(u16 ap_id);

/*!
   Registers a host onto an app. Only the reigisted hosts can receive
   the events from the app.

   \param[in] ap_id Specifies the id of an app.
   \param[in] root_id Specifies the id of a root which be as a host.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_register_ap_msghost(u16 ap_id, u16 root_id);

/*!
   Unregisters a host from an app.

   \param[in] ap_id Specifies the id of an app.
   \param[in] root_id Specifies the id of a root which be unregisted.

   \return If successful return TRUE, Otherwise return FALSE.
  */
BOOL fw_unregister_ap_msghost(u16 ap_id, u16 root_id);

/*!
   The default main loop of a mainwin.

   \param[in] root_id Specifies the id of a mainwin.
  */
void fw_default_mainwin_loop(u16 root_id);

/*!
   The type of nofity method
  */
typedef enum
{
  /*!
     Notify a message.
    */
  NOTIFY_T_MSG = 1,
  /*!
     Notify a key.
    */
  NOTIFY_T_KEY
}notify_type_t;

/*!
   Notifies its parent.

   \param[in] root_id Specifies the id of a root.
   \param[in] type Specifies the type of the notify.
   \param[in] is_sync Determines whether the notify is synchronized or NOT.
   \param[in] content Specifies the content of the notify.
   \param[in] para1 Specifies the first parameter.
   \param[in] para2 Specifies the second parameter.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_notify_parent(u16 root_id,
                          u8 type,
                          BOOL is_sync,
                          u16 content,
                          u32 para1,
                          u32 para2);
/*!
   Notifies its child.

   \param[in] root_id Specifies the id of a root.
   \param[in] child_id Specifies the id of its child.
   \param[in] type Specifies the type of the notify.
   \param[in] is_sync Determines whether the notify is synchronized or NOT.
   \param[in] content Specifies the content of the notify.
   \param[in] para1 Specifies the first parameter.
   \param[in] para2 Specifies the second parameter.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_notify_child(u16 root_id,
                         u16 child_id,
                         u8 type,
                         BOOL is_sync,
                         u16 content,
                         u32 para1,
                         u32 para2);

/*!
   Notifies its child.

   \param[in] p_ctrl Specifies the control whidh post msg.
   \param[in] type Specifies the type of the notify.
   \param[in] is_sync Determines whether the notify is synchronized or NOT.
   \param[in] content Specifies the content of the notify.
   \param[in] para1 Specifies the first parameter.
   \param[in] para2 Specifies the second parameter.

   \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_notify_root(control_t *p_ctrl,
                         u8 type,
                         BOOL is_sync,
                         u16 content,
                         u32 para1,
                         u32 para2);

/////////////////////////////////////////////////////////////////////////////
//	end of message operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	timer operations
/////////////////////////////////////////////////////////////////////////////

/*!
   Creates a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.
    \param[in] tm_out Specifies the time out.
    \param[in] is_circle Determines whether the timer is circle or NOT.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_create(u16 owner, u16 notify, u32 tm_out, BOOL is_circle);

/*!
   Destroies a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_destroy(u16 owner, u16 notify);

/*!
   Starts a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_start(u16 owner, u16 notify);

/*!
   Stops a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_stop(u16 owner, u16 notify);

/*!
   Resets a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.
    \param[in] tm_out Specifies the time out.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_reset(u16 owner, u16 notify, u32 tm_out);

/*!
   Gets the spare time of a timer.

    \param[in] owner Specifies the id of the owmer.
    \param[in] notify Specifies the notify when the timer is expired.
    \param[out] sparetime Points to buffer.

    \return If successful return SUCCESS, Otherwise return ERR_FAILTURE.
  */
RET_CODE fw_tmr_get_sparetime(u16 owner, u16 notify, u32 *p_sparetime);

/////////////////////////////////////////////////////////////////////////////
//	end of timer operations
/////////////////////////////////////////////////////////////////////////////


/*!
  worker proc
  */
typedef void (*worker_proc_t)(void);

/*!
  framework is working.
  */
BOOL fw_is_working(void);

/*!
  start worker
  */
BOOL fw_start_worker(u16 worker_root, worker_proc_t wproc, u32 context);

/*!
  framework enter waitting state.(open a menu for waitting.)
  */
void fw_watting(u32 rstyle_root, u32 rstyle_wait, u32 img_id);

/*!
  get anim.
  */
void fw_get_anim(u16 root_id, anim_in_t *p_anim_in, anim_out_t *p_anim_out);

/*!
  set anim.
  */
void fw_set_anim(u16 root_id, anim_in_t anim_in, anim_out_t anim_out);

#ifdef __cplusplus
}
#endif
#endif
