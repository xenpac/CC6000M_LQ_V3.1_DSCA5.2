/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GDI_DC_H__
#define __GDI_DC_H__

/*!
  Dc type : screen dc.
  */
#define TYPE_SCREEN     0x0001

/*!
  Dc type : general dc.
  */
#define TYPE_GENERAL    0x0002

/*!
  Dc type : memory dc
  */
#define TYPE_MEMORY     0x0004

/*!
  top dc
  */
#define TYPE_TOP        0x0800

/*!
  Dc type : virtual dc
  */
#define TYPE_VIRTUAL    0x8000

/*!
  Dc type : inherit dc.
  */
#define TYPE_INHERIT    0x10000

/*!
  Dc type : animation dc.
  */
#define TYPE_ANIM    0x20000

/*!
  Initial lock(do nothing.)
  */
#define INIT_LOCK(lock)
/*!
  Lock(do nothing.)
  */
#define LOCK(lock)
/*!
  Unlock(do nothing.)
  */
#define UNLOCK(lock)
/*!
  Clip region lock(do nothing.)
  */
#define LOCK_GCRINFO(pdc)
/*!
  Unlock(do nothing.)
  */
#define UNLOCK_GCRINFO(pdc)
/*!
  Destory lock(do nothing.)
  */
#define DESTROY_LOCK(lock)

/*!
  Structure Device Context.
  */
typedef struct
{
  /*!
    Dc type.
    */
  s32 type; 
  /*!
    Dc is used?
    */
  BOOL is_used;
  /*!
    Current surfaces of this DC 
    */
  void *p_curn_flinger;
  /*!
    Back surfaces of this DC 
    */
  void *p_back_flinger;
  /*!
    flinger for new flinger, save new flinger data.
    */
  void *p_anim_new_flinger;
  /*!
    flinger for old flinger, save old flinger data.
    */
  void *p_anim_old_flinger;    
  /*!
    flinger for tmp flinger, save tmp flinger data.
    */
  void *p_anim_tmp_flinger;    
  /*!
    Current pixel. 
    */
  u32 cur_pixel;
  /*!
    Clip region information 
    */
  cliprgn_t lcrgn;
  /*!
    Clip region information 
    */  
  cliprgn_t ecrgn;
  /*!
    device coordinate, base on screen orignal.
    */
  rect_t dev_rc;
  /*!
    Virtual device rect 
    */
  rect_t vtrl_rc;
  /*!
    Output rectangle  
    */
  rect_t rc_output;
  /*!
    Cliprgn info 
    */
  crgn_info_t *p_gcrgn_info;
  /*!
    animation handle
    */
  handle_t anim_handle;  
  /*!
    transformation.
    */
  transformation_t trans;    
}dc_t;

/*!
  This function convert HDC to PDC.

  \param[in] hdc : hdc
  \return : pdc
  */
dc_t *dc_hdc2pdc(hdc_t hdc);

/*!
  dc is top hdc.
  */
BOOL dc_is_top_hdc(hdc_t hdc);

/*!
  To judge if hdc is memory dc.

  \param[in] hdc : hdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_mem_hdc(hdc_t hdc);

/*!
  To judge if hdc is screen dc.

  \param[in] hdc : hdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_screen_hdc(hdc_t hdc);

/*!
  To judge if hdc is general dc.

  \param[in] hdc : hdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_general_hdc(hdc_t hdc);

/*!
  To judge if hdc is inherit dc.

  \param[in] hdc : hdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_inherit_hdc(hdc_t hdc);

/*!
  is virtual hdc.
  */
BOOL dc_is_virtual_hdc(hdc_t hdc);

/*!
  dc is anim hdc.
  */
BOOL dc_is_anim_hdc(hdc_t hdc);

/*!
  To judge if dc is memory dc.

  \param[in] hdc : pdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_mem_dc(dc_t *p_pdc);

/*!
  To judge if dc is screen dc.

  \param[in] hdc : pdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_screen_dc(dc_t *p_pdc);

/*!
  To judge if dc is general dc.

  \param[in] hdc : pdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_general_dc(dc_t *p_pdc);

/*!
  To judge if dc is virtual dc.

  \param[in] hdc : pdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_virtual_dc(dc_t *p_pdc);

/*!
  To judge if dc is inherit dc.

  \param[in] hdc : hdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_inherit_dc(dc_t *p_pdc);

/*!
  To judge if dc is visible.

  \param[in] hdc : pdc.
  \return : True for yes, otherwise no.
  */
BOOL dc_is_visible(dc_t *p_pdc);

/*!
  Ajust the screen position according to a dc.

  \param[in] pdc : pdc.
  \param[in/out] p_x : x-coordinate
  \param[in/out] p_y : y-coordinate  
  \return : NULL
  */
void coor_sp2sp(dc_t *p_pdc, s16 *p_x, s16 *p_y);

/*!
  Device position to screen positon.

  \param[in] pdc : pdc.
  \param[in/out] p_x : x-coordinate
  \param[in/out] p_y : y-coordinate  
  \return : NULL
  */
void coor_dp2sp(dc_t *p_pdc, s16 *p_x, s16 *p_y);

/*!
  Screen positon to device position

  \param[in] pdc : pdc.
  \param[in/out] p_x : x-coordinate
  \param[in/out] p_y : y-coordinate  
  \return : NULL
  */
void coor_sp2dp(dc_t *p_pdc, s16 *p_x, s16 *p_y);

#endif

