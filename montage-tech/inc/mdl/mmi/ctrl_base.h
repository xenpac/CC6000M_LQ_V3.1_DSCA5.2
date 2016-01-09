/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CTRL_BASE_H__
#define __CTRL_BASE_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
   Ctrl printf define
  */
#define CTRL_PRINTF DUMMY_PRINTF

/*!
   \file ctrl_base.h

   This file defined common data structures and macro constant for all control
   classes.
   It also defines some interfaces for aplication layer modules, all these
   interfaces are
   common used by all control classes.
  */


/*!
   Class name table size, A-Z
  */
#define CLASS_NAME_TABLE_SIZE    26

/*!
   Align center in horizontal direction
  */
#define STL_CENTER     0x00000000L
/*!
   Align left
  */
#define STL_LEFT       0x10000000L
/*!
   Align right
  */
#define STL_RIGHT      0x20000000L
/*!
   Align center in vertical direction
  */
#define STL_VCENTER    0x00000000L
/*!
   Align top
  */
#define STL_TOP        0x40000000L
/*!
   Align bottom
  */
#define STL_BOTTOM     0x80000000L

/*!
   Container is a topmost container
  */
#define STL_EX_TOPMOST      0x00000001L
/*!
   Control is a whole highlight control
  */
#define STL_EX_WHOLE_HL     0x00000002L

/*!
   Control is a whole highlight control
  */
#define STL_EX_ALWAYS_HL    0x00000004L

/*!
  ctrl type.
  */
typedef enum
{
  /*!
    bitmap
    */
  CTRL_BMAP = 0,
  /*!
    combo-box.
    */
  CTRL_CBOX,
  /*!
    container.
    */
  CTRL_CONT,
  /*!
    edit-box.
    */
  CTRL_EBOX,
  /*!
    ip-box.
    */
  CTRL_IPBOX,
  /*!
    list.
    */
  CTRL_LIST,
  /*!
    matrix-box.
    */
  CTRL_MBOX,
  /*!
    number-box.
    */
  CTRL_NBOX,
  /*!
    progress bar.
    */
  CTRL_PBAR,
  /*!
    scroll bar.
    */
  CTRL_SBAR,
  /*!
    set box.
    */
  CTRL_SBOX,
  /*!
    textfield.
    */
  CTRL_TEXT,
  /*!
    time-box.
    */
  CTRL_TBOX,
  /*!
    album control.
    */
  CTRL_ALBUM,
  /*!
    block control.
    */
  CTRL_BLOCK,    
  /*!
    ctrl circular progress bar.
    */
  CTRL_CPBAR,  
  /*!
    ctrl orb.
    */
  CTRL_ORB,
  /*!
    ctrl wait.
    */
  CTRL_WAIT,    
  /*!
    ctrl class cnt.
    */
  CTRL_TCNT,
}ctrl_type_t;

/*!
   Control attribute
  */
typedef enum
{
  /*!
     Control is active
    */
  OBJ_ATTR_ACTIVE = 0,
  /*!
     Control is highlight
    */
  OBJ_ATTR_HL,
  /*!
     Control is inactive
    */
  OBJ_ATTR_INACTIVE,
  /*!
     Control is hidden
    */
  //OBJ_ATTR_HIDDEN
}obj_attr_t;


/*!
   Control attribute
  */
typedef enum
{
  /*!
     Control is active
    */
  OBJ_STS_SHOW = 0,
  /*!
     Control is highlight
    */
  OBJ_STS_HIDE,
}obj_sts_t;


#pragma pack(4)
/*!
   Control rectangle style
  */
typedef struct
{
  /*!
     Rect style index during OBJ_ATTR_ACTIVE
    */
  u32 show_idx;
  /*!
     Rect style index during OBJ_ATTR_HL
    */
  u32 hl_idx;
  /*!
     Rect style index during OBJ_ATTR_INACTIVE
    */
  u32 gray_idx;
} ctrl_rstyle_t;


/*!
   Predefine of control structure.
  */
struct control;

/*!
   Predefine of control class structure.
  */
struct control_class;

/*!
   Define the type of process function for a key
  */
typedef u16 (*keymap_t)(u16 key);
/*!
   Define the type of process function for a message
  */
typedef RET_CODE (*msgproc_t)(struct control *p_ctrl, u16 msg,
                              u32 para1, u32 para2);
/*!
   Define the type of painting function
  */
typedef void (*paint_t)(struct control *p_ctrl, hdc_t hdc);

/*!
   Max length of control class name
  */
#define CTRL_CLASS_NAME_LEN    12

/*!
   Max number string length, for s32
  */
#define NUM_STRING_LEN         10

/*!
   This structure define information about a control
  */
typedef struct control
{
  ctrl_type_t ctrl_type;
  /*!
     Defined in form OBJ_ATTR_XXX.
    */
  obj_attr_t attr;
  /*!
    defined in from obj_sts_xxx.
    */
  obj_sts_t sts;    
  /*!
     The styles of control.
    */
  u8 style;
  /*!
     Every ctrl has its exclusive id in one window except 0.
    */
  u16 id;
  /*!
     The left control id.
    */
  u16 left;
  /*!
     The up control id.
    */
  u16 up;
  /*!
     The right control id.
    */
  u16 right;
  /*!
     The down control id.
    */
  u16 down;
  /*!
    control bg id.(image id.)
    */
  u16 ctrl_bg;   
  /*!
     Rectangle style of a control
    */
  ctrl_rstyle_t rstyle;
  /*!
     The rectangle in parent's coordinate.
    */
  rect_t frame;
  /*!
    The effective area for draw content(mid rect).
    */
  rect_t mrect;    
  /*!
     Clip region information about a control
    */
  crgn_info_t invrgn_info;
  /*!
     The parent of this control.
    */
  struct control *p_parent;
  /*!
     The child of this control.
    */
  struct control *p_child;
  /*!
     The next sibling control.
    */
  struct control *p_next;
  /*!
     The prev sibling control.
    */
  struct control *p_prev;
  /*!
     The active child control.
    */
  struct control *p_active_child;

  /*!
     Keymap of the control
    */
  keymap_t p_keymap;

  /*!
     Message process function of the control
    */
  msgproc_t p_proc;

  /*!
     Painting function of the control
    */
  paint_t p_paint;

  /*!
     Control private attribute.
    */
  u32 priv_attr;
  /*!
     Control private context.
    */
  u32 priv_context;
  /*!
     Pointer to Control Class Info struct.
    */
  struct control_class *p_oci;
}control_t;

/*!
   This structure defines some informations about a certain control class.
  */
typedef struct control_class
{
  /*!
     Class name
    */
  ctrl_type_t ctrl_type;
  /*!
     Default ctrl
    */
  control_t *p_default_ctrl;
  /*!
     Default data
    */
  void *p_default_data;
  /*!
     Data size
    */
  u32 data_size;
  /*!
     Data heap
    */
  lib_memf_t data_heap;
  /*!
     Data heap address
    */
  void *p_data_heap_addr;
}control_class_t;

/*!
   This structure defines the informations about register a control class.
  */
typedef struct
{
  /*!
     The maxiam number of controls
    */
  u16 max_cnt;
  /*!
     Default ctrl
    */
  control_t *p_default_ctrl;
  /*!
     Data size
    */
  u32 data_size;
} control_class_register_info_t;

/*!
   Control library
  */
typedef struct
{
  /*!
     Hash table of control classes
    */
  control_class_t *p_class_table[CTRL_TCNT];
  /*!
     Class heap
    */
  lib_memf_t class_heap;
  /*!
     Class heap address
    */
  void *p_class_heap_addr;
  /*!
     Control heap
    */
  //lib_memf_t ctrl_heap;
  /*!
     Control heap address
    */
  //void *p_ctrl_heap_addr;
  /*!
    string heap addr
    */
  void *p_string_heap_addr;
  /*!
    string heap
    */
  lib_memp_t string_heap; 
  /*!
    string count, just used for string debug
    */
  u16 count;    
} control_lib_t;

/*!
   Class name error.
  */
#define ERR_CTRL_CTRL_TYPE        ((s32) - 0x100)

/*!
   The entry of key mapping
  */
typedef struct
{
  /*!
     The key
    */
  u16 key;
  /*!
     The corresponding message
    */
  u16 msg;
  /*!
     Reserve for extension
    */
  u32 reserve;
} keymap_entry_t;

/*!
   The entry of message mapping
  */
typedef struct
{
  /*!
     The message
    */
  u16 msg;
  /*!
     The corresponding process function
    */
  msgproc_t p_proc;
  /*!
     Reserve for extension
    */
  u32 reserve;
} msgmap_entry_t;

#pragma pack()

/*!
   This function is only used for define keymap_t.

   \param[in] p_entry : point to the keymap entry of control
   \param[in] key : the key
   \return : the message which is be convert by key.
  */
u16 ctrl_on_key(const keymap_entry_t *p_entry, u16 key);

/*!
   Keymap entrys
  */
#define BEGIN_KEYMAP(keymap, base) \
  static const keymap_entry_t keymap ## _entrys[] = \
  {

/*!
   Keymap member.
  */
#define ON_EVENT(key, msg) \
  { \
    key, msg, 0 \
  },

/*!
   Keymap interface.
  */
#define END_KEYMAP(keymap, base)  \
  { \
    V_KEY_INVALID, MSG_INVALID, 0 \
  } \
  }; \
  static const keymap_t keymap ## _base = base; \
  u16 keymap(u16 key) \
  { \
    u16 msg; \
    msg = ctrl_on_key(keymap ## _entrys, key); \
    if(msg == MSG_INVALID) \
    { \
      if(keymap ## _base != NULL) \
      { \
        msg = keymap ## _base(key); \
      } \
    } \
    return msg; \
  }

/*!
   This function is only used for define msgproc_t.

   \param[in] p_ctrl : point to a control
   \param[in] p_entry : point to the msgmap entry of control
   \param[in] msg : the message
   \param[in] para1 : the first parameter
   \param[in] para2 : the second parameter
   \return : the result of process message.
  */
RET_CODE ctrl_on_msg(control_t *p_ctrl,
                     const msgmap_entry_t *p_entry,
                     u16 msg,
                     u32 para1,
                     u32 para2);

/*!
   Message proc map entrys.
  */
#define BEGIN_MSGPROC(msgproc, base) \
  static const msgmap_entry_t msgproc ## _entrys[] = \
  {

/*!
   Message proc map member.
  */
#define ON_COMMAND(msg, p_proc) \
  { \
    msg, p_proc, 0 \
  },

/*!
   Memssage proc searching interface.
  */
#define END_MSGPROC(msgproc, base) \
  { \
    MSG_INVALID, NULL, 0 \
  } \
  }; \
  static const msgproc_t msgproc##_base = base; \
  RET_CODE msgproc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2) \
  { \
    RET_CODE ret = SUCCESS; \
    ret = ctrl_on_msg(p_ctrl, msgproc ## _entrys, msg, para1, para2); \
    if(ret == ERR_NOFEATURE) \
    { \
      if(msgproc ## _base != NULL) \
      { \
        ret = msgproc ## _base(p_ctrl, msg, para1, para2); \
      } \
    } \
    return ret; \
 }

/*!
   This function is only used for proc validation check.

   \param[in] p_ctrl : point to a control
   \param[in] proc   : proc function address.  

   \return : NULL.  
  */
void proc_check(control_t *p_ctrl, u32 proc);


/*!
   Message proc map entrys for control default proc.
  */
#define BEGIN_CTRLPROC(msgproc, base) \
  static const msgmap_entry_t msgproc ## _entrys[] = \
  {

/*!
   Message proc map member.
  */
#define ON_COMMAND(msg, p_proc) \
  { \
    msg, p_proc, 0 \
  },

/*!
   Memssage proc searching interface.
  */
#define END_CTRLPROC(msgproc, base) \
  { \
    MSG_INVALID, NULL, 0 \
  } \
  }; \
  static const msgproc_t msgproc##_base = base; \
  RET_CODE msgproc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2) \
  { \
    RET_CODE ret = SUCCESS; \
    proc_check(p_ctrl, (u32)msgproc); \
    ret = ctrl_on_msg(p_ctrl, msgproc ## _entrys, msg, para1, para2); \
    if(ret == ERR_NOFEATURE) \
    { \
      if(msgproc ## _base != NULL) \
      { \
        ret = msgproc ## _base(p_ctrl, msg, para1, para2); \
      } \
    } \
    return ret; \
 }

/*!
   Invalid message
  */
#define MSG_INVALID                0x0000
/*!
   Message type mask
  */
#define MSG_TYPE_MASK              0xFF00
/*!
   Message data mask
  */
#define MSG_DATA_MASK              0x00FF

/*!
   Default message begin address
  */
#define MSG_DEFAULT_BEGIN          0x0001
/*!
   Default message end address
  */
#define MSG_DEFAULT_END            0x1FFF

/*!
   User defined message begin address
  */
#define MSG_USER_BEGIN             0x2000
/*!
   End address of user defined message
  */
#define MSG_USER_END               0x3FFF

/*!
   User defined message begin address
  */
#define MSG_LOCAL_BEGIN            0x4000
/*!
   User defined message begin address
  */
#define MSG_LOCAL_END              0x5FFF

/*!
   User defined message begin address
  */
#define MSG_EXTERN_BEGIN           0x6000
/*!
   User defined message begin address
  */
#define MSG_EXTERN_END             0x7FFF

/*!
   User defined message begin address
  */
#define MSG_SHIFT_DEFAULT_BEGIN    0x8000
/*!
   User defined message begin address
  */
#define MSG_SHIFT_DEFAULT_END      0x9FFF

/*!
   User defined message begin address
  */
#define MSG_SHIFT_USER_BEGIN       0xA000
/*!
   User defined message begin address
  */
#define MSG_SHIFT_USER_END         0xBFFF

/*!
   User defined message begin address
  */
#define MSG_SHIFT_LOCAL_BEGIN      0xC000
/*!
   User defined message begin address
  */
#define MSG_SHIFT_LOCAL_END        0xDFFF

/*!
   User defined message begin address
  */
#define MSG_SHIFT_EXTERN_BEGIN     0xE000
/*!
   User defined message begin address
  */
#define MSG_SHIFT_EXTERN_END       0xFFFF


/*!
   This enum defines some special message.
  */
enum
{
  /*!
     Message for paint a control.
    */
  MSG_PAINT = MSG_DEFAULT_BEGIN,
  /*!
     Message for time out.
    */
  MSG_TMOUT,
  /*!
     Message for do nothing.
    */
  MSG_DO_NOTHING,
  /*!
    Message roll start
    */
  MSG_START_ROLL, 
  /*!
    Message roll stopped
    */
  MSG_ROLL_STOPPED, 
  /*!
    Message rolling
    */
  MSG_ROLLING,
  /*!
    Message roll next.
    */
  MSG_ROLL_NEXT,    
  /*!
    Message Heart beat.
    */
  MSG_HEART_BEAT,
  /*!
    Message update
    */
  MSG_UPDATE,   
  /*!
    Message attr changed.
    */
  MSG_ATTR_CHANGED,    
  /*!
    Message worker done.
    */
  MSG_WORKER_DONE,    
};

/*!
   This enum define messages about focus move
  */
enum
{
  /*!
     Message for get focus
    */
  MSG_GETFOCUS = MSG_DEFAULT_BEGIN + 0x0100,
  /*!
     Message for lost focus
    */
  MSG_LOSTFOCUS,
  /*!
     Message for focus up
    */
  MSG_FOCUS_UP,
  /*!
     Message for focus down
    */
  MSG_FOCUS_DOWN,
  /*!
     Message for focus left
    */
  MSG_FOCUS_LEFT,
  /*!
     Message for focus right
    */
  MSG_FOCUS_RIGHT,
  /*!
     Message for page up
    */
  MSG_PAGE_UP,
  /*!
     Message for page down
    */
  MSG_PAGE_DOWN
};


/*!
   This enum defines message about control
  */
enum
{
  /*!
     Message for create a control
    */
  MSG_CREATE = MSG_DEFAULT_BEGIN + 0x0201,
  /*!
     Message for destroy a control
    */
  MSG_DESTROY,
  /*!
     Message for select a control
    */
  MSG_SELECT,
  /*!
     Message for unselect a control
    */
  MSG_UNSELECT,
  /*!
     Open a list
    */
  MSG_OPEN_LIST,
  /*!
     Close a list
    */
  MSG_CLOSE_LIST,
  /*!
     Focus step back, may be used in editbox
    */
  MSG_BACKSPACE,
  /*!
     Focus increase, may be used in combobox control
    */
  MSG_INCREASE,
  /*!
     Focus decrease, may be used in combobox control
    */
  MSG_DECREASE,
  /*!
     Clean the content of a control
    */
  MSG_EMPTY,
  /*!
     Out of range
    */
  MSG_OUTRANGE,
  /*!
     The content will change
    */
  MSG_CHANGING,
  /*!
     The content is changed
    */
  MSG_CHANGED,
  /*!
     The content is over maxtext
    */
  MSG_MAXTEXT,
  /*!
     Message for created a control
    */
  MSG_CREATED,
  /*!
     Message for left border
    */
  MSG_BORDER,
  /*!
    Message for delete.
    */
  MSG_DELETE,    
};


/*!
   This enum defines some other common messages.
  */
enum
{
  /*!
     Msg yes
    */
  MSG_YES = MSG_DEFAULT_BEGIN + 0x0301,
  /*!
     Msg no
    */
  MSG_NO,
  /*!
     Msg cancel
    */
  MSG_CANCEL,
  /*!
     Msg exit
    */
  MSG_EXIT,
  /*!
     Msg save
    */
  MSG_SAVE
};

/*!
   Shift type
  */
enum
{
  /*!
     Msg char
    */
  MSG_CHAR = MSG_SHIFT_DEFAULT_BEGIN + 0x0000,
  /*!
     Msg number
    */
  MSG_NUMBER = MSG_SHIFT_DEFAULT_BEGIN + 0x0100,
  /*!
   Msg Keyboard char unicode
  */
  MSG_KEYBOARD_CHAR = MSG_SHIFT_DEFAULT_BEGIN + 0x0200
};

/*!
   Debug, dump usage information.
  */
void ctrl_usage(void);

/*!
   Initialize control lib.

   \param[in] max_class_cnt : max control class count in one project
   \param[in] max_ctrl_cnt : max control count in all application
   \return : TURE for success.
  */
BOOL ctrl_init_lib(u16 max_class_cnt, u16 max_ctrl_cnt);

/*!
   Release control lib.

   \return : NULL.
  */
void ctrl_release_lib(void);

/*!
  link buffers for the default control & the default control data.

  \param[in,out] p_info : class register info
  \return : SUCCESS or ERR_FAILURE.
  */
RET_CODE ctrl_link_ctrl_class_buf(control_class_register_info_t *p_info);

/*!
  unlink buffers for the default control & the default control data.

  \param[in,out] p_info : class register info
  \return : SUCCESS or ERR_FAILURE.
  */
RET_CODE ctrl_unlink_ctrl_class_buf(control_class_register_info_t *p_info);


/*!
  register control class.

  \param[in] ctrl_type : ctrl type
  \param[in] p_info : register info
  \return : SUCCESS or ERR_FAILURE.
  */
RET_CODE ctrl_register_ctrl_class(ctrl_type_t ctrl_type,
                                  control_class_register_info_t *p_info);

/*!
  Delete control class.

  \param[in] ctrl_type : control type.
  \return : SUCCESS or ERR_CTRL_CLASS_NAME
  */
RET_CODE ctrl_unregister_ctrl_class(ctrl_type_t ctrl_type);

/*!
  get control class information.

  \param[in] ctrl_type : control type.
  
  \return : a pointer of control class
  */
control_class_t *ctrl_get_ctrl_class(ctrl_type_t ctrl_type);

/*!
  Create control.

  \param[in] ctrl_type : control type.
  \param[in] id : control id.
  \param[in] x : control frame x,the referrence is parent.
  \param[in] y : control frame y,the referrence is parent.
  \param[in] w : control frame w,the referrence is parent.
  \param[in] h : control frame h,the referrence is parent.
  \param[in] p_parent : the parent control of the new control.
  \param[in] para : reserved parameter.
  \return : return a pointer of the new control for success, else return NULL.
  */
control_t *ctrl_create_ctrl(ctrl_type_t ctrl_type,
                            u16 id,
                            u16 x,
                            u16 y,
                            u16 w,
                            u16 h,
                            control_t *p_parent,
                            u32 para);

/*!
   Destroy control.

   \param[in] p_ctrl : control to be destroyed
   \return : TRUE/FALSE
  */
BOOL ctrl_destroy_ctrl(control_t *p_ctrl);

/*!
   Seperate an control , used by framework.
   Sometime desktop's some controls' parent is NULL,
   but need brother information.

   \param[in] p_ctrl : point to the control
   \return : NULL
  */
void ctrl_seperate_ctrl(control_t *p_ctrl);

/*!
  Insert an control , used by framework.
  Sometime desktop's some controls' parent is NULL,
  but need brother information.

  \param[in] p_prev : point to the control
  \param[in] p_ctrl : prev control
  \return : NULL
  */
void ctrl_insert_ctrl(control_t *p_prev, control_t *p_ctrl);

/*!
   Init control.

   \param[in] p_ctrl : control
   \return : SUCCESS or ERR_NO_MEM
  */
RET_CODE ctrl_init_ctrl(control_t *p_ctrl);

/*!
   Free control.

   \param[in] p_ctrl : control
   \return : NULL
  */
void ctrl_free_ctrl(control_t *p_ctrl);


/*!
   Get root container.

   \param[in] p_ctrl : point to the control
   \return : a point pointer to the root container
  */
control_t *ctrl_get_root(control_t *p_ctrl);


/*!
   Get control's parent control.

   \param[in] p_ctrl : point to the control
   \return : a point pointer to the parent control
  */
control_t *ctrl_get_parent(control_t *p_ctrl);


/*!
   Get control's child control.

   \param[in] p_ctrl : point to the control
   \return : a point pointer to the first child control
  */
control_t *ctrl_get_first_child(control_t *p_ctrl);


/*!
   Get control's current focus child control.

   \param[in] p_ctrl : point to the control
   \return : a point pointer to the active child control
  */
control_t *ctrl_get_active_ctrl(control_t *p_ctrl);


/*!
   Get control's previous control.

   \param[in] p_ctrl : point to the control
   \return : a point pointer to the previous control
  */
control_t *ctrl_get_prev_ctrl(control_t *p_ctrl);


/*!
   Get control's next control.

   \param[in] p_ctrl : point to the control
   \return : a pointer point to the next control
  */
control_t *ctrl_get_next_ctrl(control_t *p_ctrl);
/*!
  Get control by id.

  \param[in] p_root : root control
  \param[in] id	 : child control id
  \return : a pointer point to control.
  */
control_t *ctrl_get_ctrl_by_unique_id(control_t *p_root, u16 id);
/*!
  Get child control by id.

  \param[in] p_parent : parent control
  \param[in] id	 : child control id
  \return : a pointer point to chilid control.
  */
control_t *ctrl_get_child_by_id(control_t *p_parent, u16 id);

/*!
  destroy child by id.

  \param[in] p_parent : parent control
  \param[in] id	 : child control id
  \return : TRUE:success ; FALSE: failure
  */
BOOL ctrl_destroy_child_by_id(control_t *p_parent, u16 id);

/*!
   Get control states.

   \param[in] p_ctrl : control
   
   \return : states of the control.
  */
obj_sts_t ctrl_get_sts(control_t *p_ctrl);

/*!
   Get control attribute.

   \param[in] p_ctrl : control
   \return : attribute of the control.
  */
obj_attr_t ctrl_get_attr(control_t *p_ctrl);

/*!
  get control r-style.
  */
void ctrl_get_rstyle(control_t *p_ctrl, ctrl_rstyle_t *p_rstyle);

/*!
   Get control id.

   \param[in] p_ctrl : control
   \return : control id.
  */
u16 ctrl_get_ctrl_id(control_t *p_ctrl);

/*!
  set ctrl bg.
  */
void ctrl_set_bg(control_t *p_ctrl, u16 ctrl_bg);

/*!
  get ctrl bg.
  */
u16 ctrl_get_bg(control_t *p_ctrl);


/*!
   Get control width.

   \param[in] p_ctrl : point to the control
   \return : control width
  */
s16 ctrl_get_width(control_t *p_ctrl);


/*!
   Get control height.

   \param[in] p_ctrl : point to the control
   \return : control height
  */
s16 ctrl_get_height(control_t *p_ctrl);


/*!
   Get control process callback.

   \param[in] p_ctrl : point to the control
   \return : control process callback
  */
msgproc_t ctrl_get_proc(control_t *p_ctrl);


/*!
   Get control keymap callback.

   \param[in] p_ctrl : point to the control
   \return : control keymap fuction
  */
keymap_t ctrl_get_keymap(control_t *p_ctrl);


/*!
   Get control's style.

   \param[in] p_ctrl : point to the control
   \return : control style
  */
u8 ctrl_get_style(control_t *p_ctrl);

/*!
  get control type.

  \param[in] p_ctrl : point to the control

  \return control type.  
  */
ctrl_type_t ctrl_get_type(control_t *p_ctrl);

/*!
   Check whether the control should be shown as whole highlight
   Only check one layer.

   \param[in] p_ctrl : control
   \return : TRUE/FALSE
  */
BOOL ctrl_is_whole_hl(control_t *p_ctrl);

/*!
   Check whether the control should be shown always highlight
   Only check one layer.

   \param[in] p_ctrl : control
   \return : TRUE/FALSE
  */
BOOL ctrl_is_always_hl(control_t *p_ctrl);


/*!
   Check whether the control is a root container.

   \param[in] p_ctrl : control
   \return : TRUE/FALSE
  */
BOOL ctrl_is_root(control_t *p_ctrl);

/*!
   Set control states.

   \param[in] p_ctrl : control
   \param[in] new_sts : new states for setting.
   
   \return : NULL
  */
void ctrl_set_sts(control_t *p_ctrl, obj_sts_t new_sts);

/*!
   Set control states.no recursion

   \param[in] p_ctrl : control
   \param[in] new_sts : new states for setting.
   
   \return : NULL
  */
void ctrl_set_sts_ex(control_t *p_ctrl, obj_sts_t new_sts);

/*!
   Set control attribute.

   \param[in] p_ctrl : control
   \param[in] attribut : new attrib for setting, Please check document about
   attribute type
   \return : NULL
  */
void ctrl_set_attr(control_t *p_ctrl, obj_attr_t attr);


/*!
  Set control rect.

  \param[in] p_ctrl : point to the control
  \param[in] p_frame : the frame of the control want to set.The referrence of
  frame is parent
  \return : NULL
  */
void ctrl_set_frame(control_t *p_ctrl, rect_t *p_frame);

/*!
  Set control rect.

  \param[in] p_ctrl : point to the control
  \param[in] left : left.
  \param[in] top : top.
  \param[in] right : right.
  \param[in] bottom : bottom.
  
  \return : NULL
  */
void ctrl_set_mrect(control_t *p_ctrl, u16 left, u16 top, u16 right, u16 bottom);

/*!
  control resize.
  */
void ctrl_resize(control_t *p_ctrl, rect_t *p_frame);

/*!
  Set control's active child.

  \param[in] p_ctrl : point to the control
  \param[in] p_active_child : point to the control's active child control
  \return : NULL
  */
void ctrl_set_active_ctrl(control_t *p_ctrl, control_t *p_active_child);

/*!
   Set control process function.

   \param[in] p_ctrl : point to the control
   \param[in] p_proc : control process callback
   \return : NULL
  */
void ctrl_set_proc(control_t *p_ctrl, msgproc_t p_proc);


/*!
   Set control vkey map callback.

   \param[in] p_ctrl : point to the control
   \param[in] p_keymap : keymap function of control
   \return : NULL
  */
void ctrl_set_keymap(control_t *p_ctrl, keymap_t p_keymap);


/*!
   Set control id and relative id.

   \param[in] p_ctrl : control
   \param[in] left : left id
   \param[in] up  : up id
   \param[in] right : right id
   \param[in] down : down id
   \return : NULL
  */
void ctrl_set_related_id(control_t *p_ctrl, u16 left, u16 up, u16 right, u16 down);


/*!
  Set control color styles.

  \param[in] p_ctrl : point to the control
  \param[in] sh_style : sh style index
  \param[in] hl_style : highlight style index
  \param[in] gr_style : gray style index
  \return : NULL
  */
void ctrl_set_rstyle(control_t *p_ctrl, u32 sh_style, u32 hl_style, u32 gr_style);

/*!
   Set control's style.

   \param[in] p_ctrl : point to the control
   \param[in] style : the control stylea
   \return : NULL
  */
void ctrl_set_style(control_t *p_ctrl, u8 style);


/*!
   Move control.

   \param[in] p_ctrl : control
   \param[in] x  : x position
   \param[in] y  : y position
   \return : TRUE/FALSE
  */
BOOL ctrl_move_ctrl(control_t *p_ctrl, s16 x, s16 y);

/*!
  Get rect of ctrl ,and the rect's referrence is control.

  \param[in]	p_ctrl : control
  \param[out] p_rc  : output control rect
  \return : NULL
  */
void ctrl_get_client_rect(control_t *p_ctrl, rect_t *p_rc);

/*!
  Get rect of ctrl, and the rect's referrence is control.

  \param[in]	p_ctrl : control
  \param[out] p_rc  : output control rect
  \return : NULL
  */
void ctrl_get_draw_rect(control_t *p_ctrl, rect_t *p_rc);

/*!
   Paint control's children.

   \param[in] p_ctrl : control
   \param[in] hdc    : control DC
   \return : NULL
  */
void ctrl_paint_children(control_t *p_ctrl, hdc_t hdc);

/*!
   hide control.

   \param[in] p_ctrl : control
   \return : TRUE/FALSE
  */
BOOL ctrl_hide_ctrl(control_t *p_ctrl);

/*!
   Erase control.

   \param[in] p_ctrl : control
   \return : TRUE/FALSE
  */
BOOL ctrl_erase_ctrl(control_t *p_ctrl);

/*!
   Paint control.

   \param[in] p_ctrl : control
   \param[in] is_force  : add whole rect into invalid region
   \return : NULL
  */
void ctrl_paint_ctrl(control_t *p_ctrl, BOOL is_force);


/*!
   Check whether the control should be redraw.

   \param[in] p_ctrl : control
   \return : if control is on shwo return TURE, else return FALSE.
  */
BOOL ctrl_is_onshow(control_t *p_ctrl);


/*!
   Check whether the control is on focus

   \param[in] p_ctrl : control
   \return : if control is on focus return TURE, else return FALSE.
  */
BOOL ctrl_is_onfocus(control_t *p_ctrl);


/*!
  paint sub control.

  \param[in] p_ctrl : control
  \param[in] p_sub_ctrl : sub control
  \param[in] is_force  : add sub_ctrl rect to invalid region
  \return : NULL
  */
void ctrl_paint_child(control_t *p_ctrl, control_t *p_sub_ctrl, BOOL is_force);

/*!
   Get the rectangle of frame.

   \param[in] p_ctrl : point to a control
   \param[out] p_rc : for saving the rectangle of frame.
   \return : NULL.
  */
void ctrl_get_frame(control_t *p_ctrl, rect_t *p_rc);

/*!
   Get the rectangle of mid rect.

   \param[in] p_ctrl : point to a control
   \param[out] p_rc : for saving the rectangle of mid rect, the referance is the frame.
   \return : NULL.
  */
void ctrl_get_mrect(control_t *p_ctrl, rect_t *p_rc);

/*!
   Adjust the coordinate of from a opposite coordinate
   to a absolute coordinate.

   \param[in] p_ctrl : point to a control
   \param[out] p_rc : for saving the coordinate after adjusting.
   \return : NULL.
  */
void ctrl_client2screen(control_t *p_ctrl, rect_t *p_rc);


/*!
  Adjust the coordinate from a absolute cooridnate to a opposite cooridinate
  (relative to the frame of the control).

  \param[in] p_ctrl : point to a control
  \param[out] p_rc : for saving the coordinate after adjusting.
  \return : if successful return TRUE, Othewise return FALSE(rc's left-top 
           point  is not in control).
  */
BOOL ctrl_screen2client(control_t *p_ctrl, rect_t *p_rc);

/*!
  control is top most.
  */
BOOL ctrl_is_topmost(control_t *p_ctrl);

/*!
  Common interface for translate key to msg.

  \param[in] p_ctrl : point to a control
  \param[in] key : key value
  \param[in] para1 : para1
  \param[in] para2 : para2
  \return : key value.
  */
u16 ctrl_translate_key(control_t *p_ctrl, u16 key, u32 para1, u32 para2);

/*!
  Common interface for process the message

  \param[in] p_ctrl : point to a control
  \param[in] msg : message
  \param[in] para1 : para1
  \param[in] para2 : para2
  \return : success or not
  */
RET_CODE ctrl_process_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


/*!
   Get the heap object of the control library.

   \return : the heap object.
  */
lib_memf_t *ctrl_get_ctrl_heap(void);


/*!
  This function is only used for inherit msgproc_t.

  \param[in] p_ctrl : point to a control
  \param[in] msg : the message
  \param[in] para1 : the first parameter
  \param[in] para2 : the second parameter
  \return : the result of process message.
  */
RET_CODE ctrl_default_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

/*!
   Get the private context of a control.

   \param[in] p_ctrl : point to a control
   \return : private context.
  */
u32 ctrl_get_context(control_t *p_ctrl);

/*!
   Set the private context of a control.

   \param[in] p_ctrl : point to a control
   \param[in] context : context to be set.
   \return : NULL.
  */
void ctrl_set_context(control_t *p_ctrl, u32 context);

/*!
  set bg.
  */
void ctrl_set_bg(control_t *p_ctrl, u16 ctrl_bg);

/*!
  get bg.
  */
u16 ctrl_get_bg(control_t *p_ctrl);


#ifdef  __cplusplus
}
#endif

#endif
