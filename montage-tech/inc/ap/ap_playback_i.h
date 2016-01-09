/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __AP_PLAYBACK_I_H_
#define __AP_PLAYBACK_I_H_

/*!
  invalid program number, using to initialize pg_id or check the pg_id
  */
#define INVALID_PG_ID 0xFFFF

/*!
  default teletext page
  */
#define INVALID_TTX_PAGE 0xFFF

/*!
  support teletext service number
  */
#define MAX_TTX_SERVICE 5

/*!
  support subtitle service number
  */
#define MAX_SUBT_SERVICE   16//10   ttx sub+dvb sub more than 10

/*!
  support language number
  */
#define LANG_NUM 3

/*!
  Max subtitle description
  */
#define MAX_SUBT_DESCRIPTION  DVB_MAX_SUBTILTE_DESC

/*!
  Max TTX description
  */
#define MAX_TTX_DESCRIPTION   DVB_MAX_TELETEXT_DESC

/*!
  Sub pixel buf size
  */
#define SUB_PIXEL_BUF_SIZE  (417792) //(((720) * (576)) + (3 * KBYTES))

/*!
  TTX display mode
  */
typedef enum
{
  /*!
    Idle
    */
  TTX_DISPLAY_MODE_IDLE,
  /*!
    Normal
    */
  TTX_DISPLAY_MODE_NORMAL,
  /*!
    TTX subtitle mode
    */
  TTX_DISPLAY_MODE_SUBTITLE
} ttx_display_mode_t;

/*!
  Subtitle types
  */
typedef enum
{
  /*!
    undefine
    */
  SUBT_TYPE_UNDEF = 0,
  /*!
    DVB subtitles with no monitor aspect ratio criticality
    */
  SUBT_TYPE_NORMAL,
  /*!
    DVB subtitles for display on 4:3 aspect ratio monitor
    */
  SUBT_TYPE_AR4_3,
  /*!
    DVB subtitles for display on 16:9 aspect ratio monitor
    */
  SUBT_TYPE_AR16_9,
  /*!
    DVB subtitles for display on 2.21:1 aspect ratio monitor
    */
  SUBT_TYPE_AR2P21_1,
  /*!
    DVB subtitles for display on hd
    */
  SUBT_TYPE_AR_HD,    
  /*!
    DVB subtitles (for hearing impaired people)
    with no monitor aspect ratio criticality
    */
  SUBT_TYPE_NORMAL_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 4:3 aspect ratio monitor
    */
  SUBT_TYPE_AR4_3_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 16:9 aspect ratio monitor
    */
  SUBT_TYPE_AR16_9_HI,
  /*!
    DVB subtitles (for hearing impaired people)
    for display on 2.21:1 aspect ratio monitor
    */
  SUBT_TYPE_AR2P21_1_HI,
  /*!
    teletext subtitle
    */
  SUBT_TYPE_TTX_NORMAL,
  /*!
    teletext subtitle for hearing impaired people
    */
  SUBT_TYPE_TTX_HEARING_HI,
  /*!
    teletext subtitle for hearing impaired people
    */
  TTX_INITIAL_PAGE,
  /*!
    teletext subtitle for hearing impaired people
    */
  TTX_ADDITIONAL_INFO_PAGE,
  /*!
    teletext subtitle for hearing impaired people
    */
  TTX_PROG_SCHEDULE_PAGE
} subt_type_t;

/*!
  teletext service define
  */
typedef struct
{
  /*!
    teletext service's language id
    */
  u8 lang[LANG_NUM];
  /*!
    sub-title type
    */
  subt_type_t type;
} ttx_service_t;

/*!
  subtitle service define
  */
typedef struct
{
  /*!
    subtitle service's language id
    */
  u8 lang[LANG_NUM];
  /*!
    subtitle service's type
    */
  subt_type_t type;
} subt_service_t;

/*!
  In current stream, subtitle service information
  */
typedef struct
{
  /*!
    subtitle service's number
    */
  u8 service_cnt;
  /*!
    all of services' information
    */
  subt_service_t service[MAX_SUBT_SERVICE];
  /*!
    current service's information
    */
  subt_service_t cur_service;
} pb_subt_info_t;

/*!
  In current stream, teletext service information
  */
typedef struct
{
  /*!
    teletext service's number
    */
  u8 service_cnt;
  /*!
    all of teletexts' information
    */
  ttx_service_t service[MAX_TTX_SERVICE];
  /*!
    current teletext's information
    */
  ttx_service_t cur_service;
} pb_ttx_info_t;

/*!
  Construct pb policy
  \param[out] policy for playback
  */
pb_policy_t *construct_pb_policy(void);

/*!
  Construct pb policy
  \param[in] policy for playback
  */
void destruct_pb_policy(pb_policy_t *p_pb_policy);

/*!
  Get subtitle information
  \param[out]: buffer for fetching subt description
  */
BOOL get_subt_description(class_handle_t handle,
        pb_subt_info_t *p_info, u16 pg_id);
/*!
  Get ttx description
  \param[out]: buffer for saving ttx description
  */
BOOL get_ttx_description(class_handle_t handle,
        pb_ttx_info_t *p_info, u16 pg_id);

/*!
  Get ttx description
  \param[out]: buffer for saving ttx description
  */
BOOL get_ttx_description_1(class_handle_t handle,
        pb_ttx_info_t *p_info, u16 pg_id);

/*!
  Get ttx description
  \param[in] policy for playback
  */
void subt_txt_info_reset(handle_t handle);

/*!
  Set cur pg id //work round
  \param[out]: buffer for saving ttx description
  */
void stc_set_pg_id(handle_t handle, u16 pg_id);
/*!
  init  ttx sub headle
  \param[out] sub ttx handle
  */
handle_t init_sub_ttx_1(void);
/*!
  start suble dec
  \param[in] sub ttx handle
  */
void start_subt_dec_1(handle_t handle);

#endif // End for __AP_PLAYBACK_I_H_

