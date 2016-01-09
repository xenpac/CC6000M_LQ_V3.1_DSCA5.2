/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __VBI_VSB_H__
#define __VBI_VSB_H__

/*!
    NULL PAGE
  */
#define TTX_NULL_PAGE_NO        0x8FF

/*!
    teletext key
  */
typedef enum
{
    /*!
        TTX_KEY_0
      */
    TTX_KEY_0,
    /*!
        TTX_KEY_1
      */
    TTX_KEY_1,
    /*!
        TTX_KEY_2
      */
    TTX_KEY_2,
    /*!
        TTX_KEY_3
      */
    TTX_KEY_3,
    /*!
        TTX_KEY_4
      */
    TTX_KEY_4,
    /*!
        TTX_KEY_5
      */
    TTX_KEY_5,
    /*!
        TTX_KEY_6
      */
    TTX_KEY_6,
    /*!
        TTX_KEY_7
      */
    TTX_KEY_7,
    /*!
        TTX_KEY_8
      */
    TTX_KEY_8,
    /*!
        TTX_KEY_9
      */
    TTX_KEY_9,
    /*!
        TTX_KEY_SUBPAGE
      */
    TTX_KEY_SUBPAGE,
    /*!
        TTX_KEY_MIX
      */
    TTX_KEY_MIX,
    /*!
        TTX_KEY_HOLD
      */
    TTX_KEY_HOLD,
    /*!
        TTX_KEY_CONCEAL
      */
    TTX_KEY_CONCEAL,
    /*!
        TTX_KEY_RED
      */
    TTX_KEY_RED,
    /*!
        TTX_KEY_GREEN
      */
    TTX_KEY_GREEN,
    /*!
        TTX_KEY_YELLOW
      */
    TTX_KEY_YELLOW,
    /*!
        TTX_KEY_CYAN
      */
    TTX_KEY_CYAN,
    /*!
        TTX_KEY_INDEX
      */
    TTX_KEY_INDEX,
    /*!
        TTX_KEY_SIZE
      */
    TTX_KEY_SIZE,
    /*!
        TTX_KEY_CANCEL
      */
    TTX_KEY_CANCEL,
    /*!
        TTX_KEY_UP
      */
    TTX_KEY_UP,
    /*!
        TTX_KEY_DOWN
      */
    TTX_KEY_DOWN,
    /*!
        TTX_KEY_LEFT
      */
    TTX_KEY_LEFT,
    /*!
        TTX_KEY_RIGHT
      */
    TTX_KEY_RIGHT,
    /*!
        TTX_KEY_PAGE_UP
      */
    TTX_KEY_PAGE_UP,
    /*!
        TTX_KEY_PAGE_DOWN
      */
    TTX_KEY_PAGE_DOWN,
    /*!
        TTX_KEY_TRANSPARENT
      */
    TTX_KEY_TRANSPARENT
} ttx_key_t;

/*!
   Font size
*/
typedef enum
{
    TTX_FONT_NORMAL,
    TTX_FONT_SMALL,
    TTX_FONT_HD
}ttx_font_size_t;

/*!
   SDK or total solution
*/
typedef enum
{
    TTX_REGION_IN_DECODER,
    TTX_REGION_OUT_DECODER,
}ttx_region_pos_t;

/*!
   national char reset
*/
typedef enum
{
    /*!
        Czech_Slovak
      */
    CZECH_SLOVAK,
    /*!
        English
      */
    ENGLISH,
    /*!
        Estonian
      */
    ESTONIAN,
    /*!
        French
      */
    FRENCH,
    /*!
        German
      */
    GERMAN,
    /*!
        Italian
      */
    ITALIAN,
    /*!
        Lettish_Lithuanian
      */
    LETTISH_LITHUANIAN,
    /*!
        Pollsh
      */
    POLISH,               
    /*!
        Portuguese_Spanish
      */
    PORTUGUESE_SPANISH,
    /*!
        Rumanlan
      */
    ROMANIAN,                
    /*!
        Serbian_Croatian_Slovenian
      */
    SERBIAN_CROATIAN_SLOVENIAN, 
    /*!
        Swedish_Finnish
      */
    SWEDISH_FINNISH,
    /*!
        Turkish
      */
    TURKISH,
}ttx_national_char_t;

/*!
  VBI event define
  */
typedef enum
{
  /*!
    VBI event define
    */
  VBI_EVT_BEGIN = (0xbb << 16),
  /*!
    VBI TTX HIDE COMMAND is ready
    */
  VBI_TTX_HIDED,
  /*!
    VBI TTX STOP COMMAND is ready
    */
  VBI_TTX_STOPPED,
  /*!
    VBI event end
    */
  VBI_EVT_END
}vbi_evt_t;

/*!
   Font size
  */
typedef struct
{
   /*!
    pal font
    */
    u8 *p_pal_font;
   /*!
    ntsl font
    */
    u8 *p_ntsl_font;
   /*!
    small font
    */
    u8 *p_small_font;
   /*!
    HD font
    */
    u8 *p_hd_font;
}ttx_font_src_t;

/*!
   ttx font src enum
  */
typedef enum
{
   /*!
    pal font
    */  
    TTX_FONT_SRC_PAL,
   /*!
    ntsl font
    */
    TTX_FONT_SRC_NTSL,
   /*!
    small font
    */ 
    TTX_FONT_SRC_SMALL,
   /*!
    hd font
    */ 
    TTX_FONT_SRC_HD
}ttx_font_type_t;

/*!
  callback function definition
  */
typedef void (*vbi_callback)(void *para);

/*!
  call back function and para
  */
typedef struct
{
    /*!
      callback function
      */
    vbi_callback callback;
    /*!
      callback para
      */
    u32 para;
}vbi_callback_para;

/*!
   initialize VBI module

   \param[in] prio the task's priority
   \param[in] stack_size the size of the stack

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_init_vsb(u32 prio, u32 stack_size, void *cb);

/*!
   Set the PID of VBI PES

   \param[in] pid the PID of VBI PES

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_pid_vsb(u16 pid);

/*!
   Set the region handle

   \param[in] p_rgn: region handle

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_region_handle_vsb(void *p_rgn);

/*!
   Start teletext decoder

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_start_vsb(u32 max_page_num, u32 max_sub_page_num, u32 cb_addr);

/*!
   Stop teletext decoder

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_stop_vsb();

/*!
   Set teletext decoder pause ON/OFF

   \param[in] is_pause TRUE for pause ON, and FALSE for pause OFF

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_pause_vsb(void);

/*!
   Set teletext decoder pause ON/OFF

   \param[in] is_pause TRUE for pause ON, and FALSE for pause OFF

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_resume_vsb(void);



/*!
   Set teletext display page

   \param[in] page_no Page number of dispaly page.
   \param[in] is_subtitle TURE if the display page is a teletext
              subtitle page, else FALSE

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_show_vsb(BOOL is_subtitle, u32 page_no);

/*!
   Set teletext display ON/OFF

   \param[in] is_display TRUE set teletext display, and FALSE set hide
   \param[in] sync Call this function in synchronization (TRUE)
              of asynchronization (FALSE)

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_ttx_hide_vsb(BOOL sync);

/*!
   Post teletext key to subtitle task

   \param[in] key Teletext key

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_post_ttx_key_vsb(ttx_key_t key);

/*!
   Start vbi inserter.
  */
RET_CODE vbi_inserter_start_vsb(void);

/*!
   Stop vbi inserter.
  */
RET_CODE vbi_inserter_stop_vsb(void);

/*!
   Set vbi inserter.

   \param[in] insertion vbi insertion

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_inserter_set_insertion(u32 insertion);

/*!
   Set video standar.

   \param[in] std standar of video

   \return Return 0 for success and others for failure.
  */
RET_CODE vbi_set_video_std_vsb(video_std_t std);

/*!
   Stop vbi font size(NORMAL size or SMALL size).
  */
RET_CODE vbi_set_font_size_vsb(ttx_font_size_t font_size);

/*!
   vbi get font(NORMAL size or SMALL size).
  */
u8 *vbi_get_font(ttx_font_type_t src);

/*!
   vbi set font src(NORMAL size or SMALL size).
  */
void vbi_set_font_src(ttx_font_src_t *p_font);

/*!
   vbi RESET font src().
   reset all font point to NULL.
  */
void vbi_reset_font_src(void);

/*!
    get memory size
*/
BOOL vbi_is_big_mem(void);

/*!
    set platform, SDK or total solution
*/
RET_CODE vbi_set_region_pos(ttx_region_pos_t pos);

/*!
    get platform, SDK or Total solution
*/
ttx_region_pos_t vbi_get_region_pos(void);
/*!
    get width and height
*/
void ttx_get_window_size(unsigned short *pWidth, unsigned short *pHeight);
/*!
    set vbi's ts in parameter
*/
void vbi_set_ts_in(u8 ts_in);


#endif
