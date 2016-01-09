/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GDI_ANIM_H__
#define __GDI_ANIM_H__

/*!
  interpolator type
  */
typedef enum
{
  /*!
    linear.
    */
  INTERPOLATOR_LINEAR = 0,
  /*!
    cycle.
    */
  INTERPOLATOR_CYCLE,
  /*!
    accelerate.
    */
  INTERPOLATOR_ACCELERATE,
  /*!
    decelerate.
    */
  INTERPOLATOR_DECELERATE,
}inter_type_t;

/*!
  step animation sub type.
  */
typedef enum
{
  /*!
    do step animation in z direction rotate mode.
    */
  STEP_ZROTATE,
  /*!
    step in from left.
    */
  STEP_IN_LEFT,
  /*!
    step in from right.
    */
  STEP_IN_RIGHT,
  /*!
    step in from top.
    */
  STEP_IN_TOP,
  /*!
    step in from bottom.
    */
  STEP_IN_BOTTOM,
  /*!
    step out from left
    */
  STEP_OUT_LEFT,    
  /*!
    step out from top
    */
  STEP_OUT_TOP,   
  /*!
    step out from right
    */
  STEP_OUT_RIGHT,   
  /*!
    step out from bottom
    */
  STEP_OUT_BOTTOM,     
}step_type_t;

/*!
  notify for animation end.
  */
typedef void(*anim_end_notify)(u32 context);

/*!
  notify for animation repeat.
  */
typedef void(*anim_rpt_notify)(u32 context);

/*!
  set interpolator type, default is INTERPOLATOR_LINEAR.
  */
void anim_set_inter(handle_t anim_handle, inter_type_t inter_type);

/*!
  set start time, default is the animation create time.
  */
void anim_set_start_time(handle_t anim_handle, u32 start_time);

/*!
  set duration time, default is 0.
  */
void anim_set_duration(handle_t anim_handle, u32 duration);

/*!
  set repeat count & repeat mode, default is 0 & FALSE. (0 means forever).
  */
void anim_set_repeat(handle_t anim_handle, u32 repeat_count, BOOL is_rpt_revse);

/*!
  is matrix identity.
  */
BOOL is_matrix_identity(matrix_t *p_matrix);

/*!
  anim get tranformation.
  */
BOOL anim_get_transf(handle_t anim_handle, u32 cur_time, transformation_t *p_out_trans);

/*!
  new rotate animation.
  */
handle_t rotate_anim_new(u32 from_degree, u32 to_degree,
  u32 pivotx, u32 pivoty, inter_type_t inter);

/*!
  new translate animation.
  */
handle_t translate_anim_new(u32 from_x, u32 to_x, u32 from_y, u32 to_y, inter_type_t inter);

/*!
  new alpha animation.
  */
handle_t alpha_anim_new(u32 from_alpha, u32 to_alpha, inter_type_t inter);

/*!
  new scale animation.
  */
handle_t scale_anim_new(u32 from_xscale, u32 to_xscale,
  u32 from_yscale, u32 to_yscale, u32 pivotx, u32 pivoty, inter_type_t inter);

/*!
  step anim new.
  */
handle_t step_anim_new(step_type_t step_type, inter_type_t inter);
  

/*!
  free animation.
  */
void anim_free(handle_t anim_handle);

/*!
  start animation.
  */
RET_CODE gdi_start_anim(hdc_t hdc);

/*!
  set animation.
  */
RET_CODE gdi_set_anim(hdc_t hdc, handle_t anim_handle);

/*!
  set callback, which will be called when animation finished.
  */
void anim_set_end_cb(handle_t anim_handle, anim_end_notify end_cb, u32 context);

/*!
  set callback, which will be called when animation repeat.
  */
void anim_set_rpt_cb(handle_t anim_handle, anim_end_notify rpt_cb, u32 context);

/*!
  create animation surface.
  */
RET_CODE gdi_create_anim_surf(hdc_t hdc);

/*!
  delete animation surface.
  */
RET_CODE gdi_delete_anim_surf(hdc_t hdc);

/*!
  init animation module.
  */
RET_CODE gdi_init_anim(u32 anim_addr, u32 anim_size);

/*!
  release animation module.
  */
void gdi_release_anim(void);
#endif
