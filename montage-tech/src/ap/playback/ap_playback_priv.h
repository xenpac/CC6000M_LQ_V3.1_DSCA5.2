/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_PLAYBACK_PRIV_H_
#define __AP_PLAYBACK_PRIV_H_

/*!
  Get the implementation of playback
  return playback application instance
  */
pb_policy_t *construct_def_ap_playback(void);

/*!
  Init subtitle/teletext control module

  \return module handle
  */
handle_t init_sub_ttx(void);

/*!
  Start subtitle decode

  \param[in] handle subtitle/teletext control module handle
  */
void start_subt_dec(handle_t handle);

/*!
  Stop subtitle decode

  \param[in] handle subtitle/teletext control module handle
  */
void stop_subt_dec(handle_t handle);

/*!
  Set subtitle language and type
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] lang subtitle language
  \param[in] type subtitle type
  */
void set_subt_service(handle_t handle, u32 lang, u32 type);

/*!
  Show subtitle

  \param[in] handle subtitle/teletext control module handle  
  */
void show_subtitle(handle_t handle);

/*!
  Hide subtitle

  \param[in] handle subtitle/teletext control module handle  
  */
void hide_subtitle(handle_t handle);

/*!
  Start teletext decode

  \param[in] handle subtitle/teletext control module handle  
  */
void start_ttx_dec(handle_t handle);

/*!
  Stop teletext decode

  \param[in] handle subtitle/teletext control module handle  
  */
BOOL stop_ttx_dec(handle_t handle);

/*!
  set teletext language

  \param[in] handle subtitle/teletext control module handle
  \param[in] pref_lang Prefer language
  */
void set_ttx_lang(handle_t handle, u32 pref_lang);

/*!
  Post teletext key
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] key teletext key
  */
void post_ttx_key(handle_t handle, u32 key);

/*!
  show teletext
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] region_handle region
  */
void show_ttx(handle_t handle, u32 region_handle);

/*!
  hide teletext
  
  \param[in] handle subtitle/teletext control module handle
  */
BOOL hide_ttx(handle_t handle);

/*!
  Start vbi inserter
  
  \param[in] handle subtitle/teletext control module handle
  */
void start_vbi_inserter(handle_t handle);

/*!
  Stop vbi inserter
  
  \param[in] handle subtitle/teletext control module handle
  */
void stop_vbi_inserter(handle_t handle);

/*!
  Update PMT
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] p_pmt pmt data
  */
void pmt_update(handle_t handle, void *p_pmt_data);

/*!
  Stop subtitle decode

  \param[in] handle subtitle/teletext control module handle
  */
void stop_subt_dec_1(handle_t handle);

/*!
  Set subtitle language and type
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] lang subtitle language
  \param[in] type subtitle type
  */
void set_subt_service_1(handle_t handle, u32 index, u32 type);

/*!
  Set subtitle language and type
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] lang subtitle language
  \param[in] type subtitle type
  */
void set_subt_service_lang_code(handle_t handle, u32 code, u32 type);

/*!
  Show subtitle

  \param[in] handle subtitle/teletext control module handle  
  */
void show_subtitle_1(handle_t handle);

/*!
  Hide subtitle

  \param[in] handle subtitle/teletext control module handle  
  */
BOOL hide_subtitle_1(handle_t handle);

/*!
  Start teletext decode

  \param[in] handle subtitle/teletext control module handle  
  */
void start_ttx_dec_1(handle_t handle);

/*!
  Stop teletext decode

  \param[in] handle subtitle/teletext control module handle  
  */
BOOL stop_ttx_dec_1(handle_t handle);

/*!
  set teletext language

  \param[in] handle subtitle/teletext control module handle
  \param[in] pref_lang Prefer language
  */
void set_ttx_lang_1(handle_t handle, u32 index);

/*!
  Post teletext key
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] key teletext key
  */
void post_ttx_key_1(handle_t handle, u32 key);

/*!
  show teletext
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] region_handle region
  */
void show_ttx_1(handle_t handle, u32 region_handle);

/*!
  hide teletext
  
  \param[in] handle subtitle/teletext control module handle
  */
BOOL hide_ttx_1(handle_t handle);

/*!
  Start vbi inserter
  
  \param[in] handle subtitle/teletext control module handle
  */
void start_vbi_inserter_1(handle_t handle);

/*!
  Stop vbi inserter
  
  \param[in] handle subtitle/teletext control module handle
  */
void stop_vbi_inserter_1(handle_t handle);

/*!
  Reset teletext
  
  \param[in] handle subtitle/teletext control module handle
  */
void subt_txt_info_reset_1(handle_t handle);

/*!
  get all ttx/subtitle pids
  
  \param[in] place store pids
  return pids cnt
  */
u32 get_subt_pids(u16 *pids);

/*!
  Update PMT
  
  \param[in] handle subtitle/teletext control module handle
  \param[in] p_pmt pmt data
  */
void pmt_update_1(handle_t handle, void *p_pmt_data, u8 ts_in);

#endif // End for __AP_PLAYBACK_PRIV_H_

