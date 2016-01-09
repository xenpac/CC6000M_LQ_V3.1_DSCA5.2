/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SUBT_TTX_API_H__
#define __SUBT_TTX_API_H__

/*!
    mul_subt_init_param_t
  */
typedef struct
{
/*!
    enable_subt
  */
  BOOL enable_subt;
/*!
    subt_stk_size
  */
  u32 subt_stk_size;
/*!
    subt_layer_id
  */
  u8 subt_layer_id;
}mul_subt_init_param_t;

/*!
    mul_ttx_init_param_t
  */
typedef struct
{
/*!
    enable_ttx
  */
  BOOL enable_ttx;
/*!
    enable_ttx_insert should enable ttx fisrt
  */
  BOOL enable_ttx_insert;
/*!
    ttx_stk_size
  */
  u32 ttx_stk_size;
/*!
    ttx_layer_id
  */
  u8 ttx_layer_id;
}mul_ttx_init_param_t;

/*!
  ttx filter level
  */
typedef enum
{
/*!
  level 1
  */
  MUL_TTX_LEVEL_1,
/*!
  level 1.5
  */
  MUL_TTX_LEVEL_1_5,
/*!
  level 2.5
  */
  MUL_TTX_LEVEL_2_5,
/*!
  level 3.5
  */
  MUL_TTX_LEVEL_3_5
}mul_ttx_level_t;

/*!
  ttx filter font config
  */
typedef struct
{
/*!
  standard charset
  */
  u32 *p_std_font;
/*!
  national charset
  */
  u32 *p_ext_font;
/*!
  font w
  */
  u8 font_w;
/*!
  font h
  */
  u8 font_h;
}mul_ttx_font_cfg_t;

/*!
  ttx attr structure
  */
typedef struct
{
  /*!
    ttx cache main page size
    */
  u16 max_page_cache;
  /*!
    ttx cache sub page size
    */
  u16 max_sub_page_cache;
  /*!
    ttx decoder level
    */
  mul_ttx_level_t ttx_level;
  /*!
    ttx font config
    */
  mul_ttx_font_cfg_t font;
  /*!
    see dmx_input_type_t
    */
  u32 dmx_in;
  /*!
    ttx_region
  */
  u32 ttx_region;
  /*!
    out_pin_task_prio
  */
  u32 out_pin_task_prio;
  /*!
    out_pin_task_stack_size
  */
  u32 out_pin_task_stack_size;  
}mul_ttx_attr_t;

/*!
    teletext key
  */
typedef enum
{
    /*!
        TTX_KEY_0
      */
    MUL_TTX_KEY_0,
    /*!
        TTX_KEY_1
      */
    MUL_TTX_KEY_1,
    /*!
        TTX_KEY_2
      */
    MUL_TTX_KEY_2,
    /*!
        TTX_KEY_3
      */
    MUL_TTX_KEY_3,
    /*!
        TTX_KEY_4
      */
    MUL_TTX_KEY_4,
    /*!
        TTX_KEY_5
      */
    MUL_TTX_KEY_5,
    /*!
        TTX_KEY_6
      */
    MUL_TTX_KEY_6,
    /*!
        TTX_KEY_7
      */
    MUL_TTX_KEY_7,
    /*!
        TTX_KEY_8
      */
    MUL_TTX_KEY_8,
    /*!
        TTX_KEY_9
      */
    MUL_TTX_KEY_9,


    /*!
        TTX_KEY_RED  11
      */
    MUL_TTX_KEY_RED,
    /*!
        TTX_KEY_GREEN 12
      */
    MUL_TTX_KEY_GREEN,
    /*!
        TTX_KEY_YELLOW 13
      */
    MUL_TTX_KEY_YELLOW,
    /*!
        TTX_KEY_CYAN 14
      */
    MUL_TTX_KEY_CYAN,
    /*!
        TTX_KEY_CANCEL 15
      */
    MUL_TTX_KEY_CANCEL,


    /*!
        TTX_KEY_UP 16
      */
    MUL_TTX_KEY_UP,
    /*!
        TTX_KEY_DOWN  17
      */
    MUL_TTX_KEY_DOWN,
    /*!
        TTX_KEY_LEFT 18
      */
    MUL_TTX_KEY_LEFT,
    /*!
        TTX_KEY_RIGHT 19
      */
    MUL_TTX_KEY_RIGHT,
    /*!
        TTX_KEY_PAGE_UP 20
      */
    MUL_TTX_KEY_PAGE_UP,
    /*!
        TTX_KEY_PAGE_DOWN  21
      */
    MUL_TTX_KEY_PAGE_DOWN,
    /*!
        TTX_KEY_TRANSPARENT  22
      */
    MUL_TTX_KEY_TRANSPARENT
} mul_ttx_key_t;


/*!
  ttx attr structure
  */
typedef struct
{
  /*!
    ttx cache main page size
    */
  u32 dmx_in;
}mul_subt_attr_t;


/*!
  mul_subtitle_start_decode
  \param[in] sub_pid: subtitle pid from pmt
  \param[in] composition_page_id: see EN300473 chapter 3.1
  \param[in] ancillary_page_id: see EN300473 chapter 3.1
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_start_decode(u16 sub_pid, u16 composition_page_id, u16 ancillary_page_id);

/*!
  mul_subtitle_stop_decode
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_stop_decode(void);

/*!
  mul_subtitle_stop_decode
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_set_attr(mul_subt_attr_t *p_attr);
/*!
  mul_subt_ttx_init
  \param[in] p_params: see mul_subt_ttx_init_param_t.
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_init(mul_subt_init_param_t *p_params);
/*!
  mul_subtitle_deinit
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_deinit(void);

/*!
  mul_teletext_set_insert
  \param[in] enable: enable insert or not
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_set_insert(BOOL enable);

/*!
  mul_subtitle_reset_pid
  \param[in] sub_pid: subtitle pid from pmt
  \param[in] composition_page_id: see EN300473 chapter 3.1
  \param[in] ancillary_page_id: see EN300473 chapter 3.1
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_reset_pid(u16 sub_pid, u16 composition_page_id, u16 ancillary_page_id);

/*!
  mul_subtitle_start_decode
  \param[in] txt_pid: teletelxt pid from pmt
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_start_decode(u16 txt_pid);

/*!
  mul_teletext_reset_pid
  \param[in] txt_pid: teletelxt pid from pmt
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_reset_pid(u16 txt_pid);

/*!
  mul_teletext_stop_decode
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_stop_decode(void);

/*!
  mul_teletext_send_key
  \param[in] key: see mul_ttx_key_t
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_send_key(mul_ttx_key_t key);

/*!
  mul_teletext_open_teletext
  \param[in] txt_pid: teletelxt pid from pmt
  \param[in] p_ttx_lang: language code string
  \param[in] type: from pmt
  \param[in] magazine_num: see 300706 chapter3.1
  \param[in] page_num: pt. pu see 300706 chapter3.1
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_display_teletext(unsigned char *p_ttx_lang,
                                       int type, 
                                       int magazine_num, 
                                       int page_num);

/*!
  mul_teletext_hide_osd
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_hide_osd(void);


/*!
  mul_teletext_get_attr
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_get_attr(mul_ttx_attr_t *p_attr);


/*!
  mul_teletext_set_attr
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_set_attr(mul_ttx_attr_t *p_attr);

/*!
  mul_teletext_init
  \param[in] p_params: see mul_subt_ttx_init_param_t.
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_init(mul_ttx_init_param_t *p_params);

/*!
  mul_teletext_deinit
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_deinit(void);

#endif

