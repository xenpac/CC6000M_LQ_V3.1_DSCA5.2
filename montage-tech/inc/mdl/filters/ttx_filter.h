/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __TTX_FILTER_H__
#define __TTX_FILTER_H__


/*!
  ttx dbg handle
  */
#define TTX_DBG   OS_PRINTF
/*!
  ttx filter command define
  */
typedef enum tag_ttx_filter_cmd
{
  /*!
    config params see ttx_filter_params_cfg_t
    */
  TTX_FILTER_CMD_CFG_PARAMS,
  /*!
    start decode
    */
  TTX_FILTER_CMD_START_DECODE,
  /*!
    stop decode
    */
  TTX_FILTER_CMD_STOP_DECODE,
  /*!
    send key command
    \param[in] lpara: ttx key value see ttx_filter_key_t
    */
  TTX_FILTER_CMD_COLOR_KEY,
  /*!
    start insert
    */
  TTX_FILTER_CMD_START_INSERT,
  /*!
    stop insert
    */
  TTX_FILTER_CMD_STOP_INSERT,
  /*!
    set service id for pmt info check.
    \param[in] lpara: ttx data Pid
    */
  TTX_FILTER_CMD_SET_PID,
  /*!
    show page no.
    \param[in] lpara: page_no 100~899 << 16 | sub_no
    */
  TTX_FILTER_CMD_SHOW_PAGE_NO,
  /*!
    get cur page no.
    \param[out] lpara: page_no 100~899 << 16 | sub_no
    */
  TTX_FILTER_CMD_GET_CUR_PAGE,
  /*!
    get cur page no.
    \param[in] lpara: page_no 100~899 << 16 | sub_no
    */
  TTX_FILTER_CMD_SET_PAGE,
  /*!
    set pal or ntsl
    \param[in] lpara: see video_std_t in drv_misc.h
    */
  TTX_FILTER_CMD_SET_PN,  
  /*!
    switch to next sub page
  //  \param[in] lpara: cur page id
    */
  TTX_FILTER_CMD_NEXT_SUB,
  /*!
    switch to prev sub page
 //   \param[in] lpara: cur page id
    */
  TTX_FILTER_CMD_PREV_SUB,

  /*!
   Set ttx type
   */
  TTX_FILTER_CMD_SET_TTX_TYPE,
  /*!
    get cur page no.
    \param[in] lpara: 1 show, 0, hide
    */
  TTX_FILTER_CMD_ENABLE_SHOW,
  /*!
    get cur page no.
    \param[in] lpara: 1 show, 0, hide
    */
  TTX_FILTER_CMD_RESET,
  /*!
    get cur page no.
    \param[in] lpara: 0-100
    */
  TTX_FILTER_CMD_SET_TRANSPARENT,
}ttx_filter_cmd_t;

/*!
  ttx filter key
  */
typedef enum tag_ttx_filter_key
{
  /*!
    KEY red
    */
  TTX_FILTER_KEY_RED,
  /*!
    KEY green
    */
  TTX_FILTER_KEY_GREEN,
  /*!
    KEY yellow
    */
  TTX_FILTER_KEY_YELLOW,
  /*!
    KEY cyan
    */
  TTX_FILTER_KEY_CYAN,
}ttx_filter_color_key_t;


/*!
  ttx filter level
  */
typedef enum
{
  /*!
    level 1
    */
  TTX_LEVEL_1,
  /*!
    level 1.5
    */
  TTX_LEVEL_1_5,
  /*!
    level 2.5
    */
  TTX_LEVEL_2_5,
  /*!
    level 3.5
    */
  TTX_LEVEL_3_5
}ttx_level_t;


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
}ttx_font_cfg_t;

/*!
  ttx filter params
  */
typedef struct 
{
  /*!
    ttx filter stack size
    */
  u16 max_page_cache;
  /*!
    ttx filter stack size
    */
  u16 max_sub_page_cache;
  /*!
    ttx filter decoder level
    */
  ttx_level_t level;
  /*!
    ttx font config
    */
  ttx_font_cfg_t font;
  /*!
    out task for show ttx
    */  
  u32 out_pin_task_prio;
  /*!
    out task for show ttx
    */   
  u32 out_pin_task_stack_size;
}ttx_filter_params_cfg_t;


/*!
  ttx_filter_calc_osd_height
  \param[in] _this: the filter handle
  \return height
  */
u16 ttx_filter_calc_osd_height(handle_t _this);


/*!
  ttx_filter_calc_osd_width
  \param[in] _this: the filter handle
  \return width
  */
u16 ttx_filter_calc_osd_width(handle_t _this);

/*!
  ttx_filter_create
  */
ifilter_t * ttx_filter_create(void *p_para);

#endif

