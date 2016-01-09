/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SUBT_TTX_CTRL_API_H__
#define __SUBT_TTX_CTRL_API_H__

/*!
   these api is high level of subt and ttx, all these api call  mul level api
  */

/*!
  SUBT_TTX_CTRL_LANG_LEN
  */
#define SUBT_TTX_CTRL_LANG_LEN  4
/*!
  SUBT_CTRL_TYPE_IDX
  */
#define SUBT_CTRL_TYPE_IDX      3
/*!
  SUBT_CTRL_DR_MAX
  */
#define SUBT_CTRL_DR_MAX        10 
/*!
  TTX_CTRL_DR_MAX
  */
#define TTX_CTRL_DR_MAX         5

/*!
  Subtitle types
  */
typedef enum
{
  /*!
    undefine
    */
  E_SUBT_TYPE_UNDEF = 0,
  /*!
    DVB subtitles with no monitor aspect ratio criticality
    */
  E_SUBT_TYPE_NORMAL,
  /*!
    DVB subtitles for display on 4:3 aspect ratio monitor
    */
  E_SUBT_TYPE_AR4_3,
  /*!
    DVB subtitles for display on 16:9 aspect ratio monitor
    */
  E_SUBT_TYPE_AR16_9,
  /*!
    DVB subtitles for display on 2.21:1 aspect ratio monitor
    */
  E_SUBT_TYPE_AR2P21_1,
  /*!
    DVB subtitles for display on hd
    */
  E_SUBT_TYPE_AR_HD,    
  /*!
    DVB subtitles (for hearing impaired people)
    with no monitor aspect ratio criticality
    */
  E_SUBT_TYPE_NORMAL_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 4:3 aspect ratio monitor
    */
  E_SUBT_TYPE_AR4_3_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 16:9 aspect ratio monitor
    */
  E_SUBT_TYPE_AR16_9_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 2.21:1 aspect ratio monitor
    */
  E_SUBT_TYPE_AR2P21_1_HI,
  /*!
    teletext subtitle
    */
  E_SUBT_TYPE_TTX_NORMAL,
  /*!
    teletext subtitle for hearing impaired people
    */
  E_SUBT_TYPE_TTX_HEARING_HI,
  /*!
    teletext subtitle for hearing impaired people
    */
  E_TTX_INITIAL_PAGE,
  /*!
    teletext subtitle for hearing impaired people
    */
  E_TTX_ADDITIONAL_INFO_PAGE,
  /*!
    teletext subtitle for hearing impaired people
    */
  E_TTX_PROG_SCHEDULE_PAGE
}subt_ttx_type_t;

/*!
  Subtitle descriptor
  */
typedef struct
{
/*!
  language str
  */
  u8              m_lang[SUBT_TTX_CTRL_LANG_LEN];
/*!
  types of des
  */
  subt_ttx_type_t m_type;
/*!
  types of des
  */
  u16 pid;
} subt_ttx_dr_i_t;

/*!
  subt_service_i_t
  */
typedef struct
{
  /*!
    cnt
    */
    u8              m_cnt;
  /*!
    subt_ttx_dr_i_t
    */
    subt_ttx_dr_i_t m_dr[SUBT_CTRL_DR_MAX];
}subt_service_i_t;

/*!
  subt_attr_i_t
  */
typedef struct
{
  /*!
    start flag
    */
    BOOL m_start;
  /*!
    ttx type subt flag
    */
    BOOL m_ttx_osd;    
  /*!
    ttx type subt flag
    */
    u16 dmx_in;
}subt_attr_i_t;

/*!
  ttx_service_i_t
  */
typedef struct
{
  /*!
    cnt
    */
    u8              m_cnt;
  /*!
    subt_ttx_dr_i_t
    */
    subt_ttx_dr_i_t m_dr[TTX_CTRL_DR_MAX];
}ttx_service_i_t;



/*!
  callback
  */
typedef void (*subt_ctrl_call_back)(u32 param);


/*!
  subt_ttx_ctrl_init_t int params
  */
typedef struct
{
  /*!
    lock mode for task: mutex or tasklock
    */
  u8 lock_mode;
  /*!
    enable_ttx_type_subt
    */
  BOOL enable_ttx_type_subt;
  /*!
    callback of subt ready
    */
  subt_ctrl_call_back subt_ready_cb;
  /*!
    callback of ttx ready
    */
  subt_ctrl_call_back ttx_ready_cb;
  /*!
    ttx param
    */
  mul_ttx_init_param_t ttx_para;
  /*!
    subt param
    */
  mul_subt_init_param_t subt_para;
}subt_ttx_ctrl_init_t;


/*!
  ttx_ctrl_send_key
  \param[in] key: see mul_ttx_key_t
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_send_key(mul_ttx_key_t key);
/*!
  ttx_ctrl_set_insert
  \param[in] enable: TRUE: enable insert
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_set_insert(BOOL enable);


/*!
  ttx_ctrl_set_attr
  \param[in] mul_ttx_attr_t
  \return SUCCESS if no error
 */
 RET_CODE ttx_ctrl_set_attr(mul_ttx_attr_t *p_attr);

/*!
  ttx_ctrl_get_attr
  \param[in] mul_ttx_attr_t
  \return SUCCESS if no error
 */
 RET_CODE ttx_ctrl_get_attr(mul_ttx_attr_t *p_attr);

/*!
  ttx_ctrl_show
  \param[in] p_lang: language of teletext
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_start(u8 *p_lang);

/*!
  ttx_ctrl_show
  \param[in] p_lang: language of teletext
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_show(void);
/*!
  ttx_ctrl_hide
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_hide(void);
/*!
  ttx_ctrl_stop
  \return SUCCESS if no error
  */
RET_CODE ttx_ctrl_stop(void);
/*!
  ttx_ctrl_get_dr
  \return SUCCESS if no error
 */
RET_CODE ttx_ctrl_get_description(ttx_service_i_t *p_subt_service);

/*!
  subt_ctrl_show
  \param[in] p_lang: language of subtitle
  \param[in] type: subt type  
  \return SUCCESS if no error
 */
RET_CODE subt_ctrl_show(u8 *p_lang, subt_ttx_type_t type);

/*!
  subt_ctrl_hide
  \return SUCCESS if no error
  */
RET_CODE subt_ctrl_hide(void);

/*!
  subt_ctrl_get_attr
  \param[in] p_subt_attr: attr of subtitle  
  \return SUCCESS if no error
 */
RET_CODE subt_ctrl_get_attr(subt_attr_i_t *p_subt_attr);

/*!
  subt_ctrl_set_attr
  \param[in] p_subt_attr: attr of subtitle  
  \return SUCCESS if no error
 */
RET_CODE subt_ctrl_set_attr(subt_attr_i_t *p_subt_attr);

/*!
  subt_ctrl_get_dr
  \return SUCCESS if no error
 */
RET_CODE subt_ctrl_get_description(subt_service_i_t *p_subt_service);


/*!
  subt_ctrl_get_dr
  \return SUCCESS if no error
 */
 RET_CODE subt_ctrl_get_cur_descripition(char *p_lang, u8 *p_type);
/*!
  subt_ttx_ctrl_set_dr
  \param[in] p_pmt: pmt data received from pmt.c
  \return SUCCESS if no error
 */
RET_CODE subt_ttx_ctrl_set_pmt(void *p_pmt);


/*!
  subt_ttx_ctrl_set_dr
  \param[in] p_pmt: pmt data received from pmt.c
  \return SUCCESS if no error
 */
void subt_ttx_ctrl_set_pg_id(u16 pg_id);

/*!
  subt_ttx_ctrl_set_dr
  \return SUCCESS if no error
 */
void subt_ttx_ctrl_reset(void);

/*!
  init_subt_ttx_ctrl_priv
  \param[in] p_param: subt_ttx_ctrl_init_t
 */
void init_subt_ttx_ctrl(subt_ttx_ctrl_init_t *p_param);

/*!
  subt_ctrl_ui_enable
  \param[in] type: type for subtitle to show
  \param[in] p_lang: language for subtitle to show
  \return SUCCESS if no error
 */
RET_CODE subt_ctrl_prepare_for_subt(u8 *p_lang,
              subt_ttx_type_t  type, subt_ctrl_call_back pre_open);
#endif

