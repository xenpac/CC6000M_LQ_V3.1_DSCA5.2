/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_ROLL_H__
#define __GUI_ROLL_H__

/*!
  roll content type
  */
typedef enum
{
  /*!
    string id.
    */
  ROLL_STRID = 0,
  /*!
    ascii string.
    */
  ROLL_ASCSTR,
  /*!
    unicode string.
    */
  ROLL_UNISTR,
  /*!
    hex number.
    */
  ROLL_HEX,
  /*!
    dec number.
    */
  ROLL_DEC,
}roll_data_t;

/*!
  roll style
  */
typedef enum
{
  /*!
    left to right
    */
  ROLL_LR = 0,
  /*!
    left to right from rect left border
    */
  ROLL_LB,
}roll_style_t;

/*!
  roll pace
  */
typedef enum
{
  /*!
    pace/1
    */
  ROLL_SINGLE = 1,
  /*!
    pace/2
    */
  ROLL_DOUBLE,
  /*!
    pace/3
    */
  ROLL_TRIPLE,
  /*!
    pace/4
    */
  ROLL_FOURFD,
}roll_pace_t;

/*!
  roll param
  */
typedef struct
{
  /*!
    style
    */
  roll_style_t style;
  /*!
    pace
    */
  roll_pace_t pace;
  /*!
    repeat times, 0 means forever
    */
  u32 repeats;
  /*!
    is force
    */
  BOOL is_force; 
  /*!
    use bg color?
    */
  BOOL use_bg;
  /*!
    bg color
    */
  u32 bg_color;    
}roll_param_t;

/*!
  parameter for create roll surface.
  */
typedef struct
{
  /*!
    control
    */
  control_t *p_ctrl;
  /*!
    rolling rect.
    */
  rect_t rect;
  /*!
    font style id
    */
  u32 fstyle_idx;
  /*!
    context
    */
  u32 context;    
  /*!
    draw style.
    */
  u32 draw_style;
  /*!
    string for paint.
    */
  u16 *p_str;
  /*!
    is picture or not
    */
  BOOL is_pic;
  /*!
    is picture raw data whether picture ID
    */
  BOOL is_pic_rawdata;
  /*!
    picture ID or picture address
    */
  u32 pic_data;
  /*!
    is top.
    */
  BOOL is_topmost;    
}rsurf_ceate_t;

/*!
  create surface for roll & add control into roll list.

  \param[in] p_rsurf      param for create rsurf.
  \param[in] p_param      param for roll.

  \return NULL
  */
void gui_create_rsurf(rsurf_ceate_t *p_rsurf, roll_param_t *p_param);

/*!
  start roll

  \param[in] p_ctrl       control for roll
  \param[in] style        roll style
  \param[in] pace        roll pace

  \return NULL
  */
void gui_start_roll(control_t *p_ctrl, roll_param_t *p_param);

/*!
  stop roll, remove all nodes which related with the control.

  \param[in] p_ctrl       control for roll

  \return NULL
  */
void gui_stop_roll(control_t *p_ctrl);

/*!
  pause roll

  \param[in] p_ctrl       control for roll

  \return NULL
  */
void gui_pause_roll(control_t *p_ctrl);

/*!
  resume roll

  \param[in] p_ctrl       control for roll

  \return NULL
  */
void gui_resume_roll(control_t *p_ctrl);

/*!
  rolling

  \return NULL
  */
void gui_rolling(void);

/*!
  roll init

  \param[in] max_cnt                max roll control cnt
  \param[in] pps                pixel per step

  \return NULL
  */
void gui_roll_init(u8 max_cnt, u8 pps);

/*!
  roll release
  */
void gui_roll_release(void);

/*!
  gui rolling node

  \param[in] p_node       node for roll
  \param[in] hdc          dc for roll

  \return NULL
  */
void gui_rolling_node(void *p_node, hdc_t hdc);

/*!
  control is rolling.

  \param[in] p_ctrl : control

  \return TRUE or FALSE
  */
BOOL ctrl_is_rolling(control_t *p_ctrl);

/*!
  reset roll content

  \param[in] p_ctrl : control

  \return NULL
  */
void gui_roll_reset(control_t *p_ctrl);

/*!
  get roll node context.

  \param[in] p_roll_node: roll node.

  \return context;
  */
u32 gui_roll_get_context(void *p_roll_node);

/*!
  add new content to roll list.

  \param[in] p_ctrl : control.
  \param[in] content : content.
  \param[in] type : type.
  \param[in] p_param : param.

  \return NULL.
  */
void gui_roll_add_list(control_t *p_ctrl, u32 content,
  roll_data_t type, roll_param_t *p_param);

/*!
  save roll parameter.
  */
BOOL gui_save_roll(control_t *p_ctrl);

/*!
  free roll parameter.
  */
void gui_free_roll(void);

/*!
  Set pps
  */
void gui_roll_set_pps(u8 pps);


#endif
