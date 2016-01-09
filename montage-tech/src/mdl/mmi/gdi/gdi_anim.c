/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "drv_dev.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "lib_rect.h"
#include "lib_unicode.h"
#include "common.h"
#include "osd.h"
#include "gpe.h"
#include "gpe_vsb.h"
#include "surface.h"

#include "flinger.h"

#include "mdl.h"
#include "mmi.h"

#include "gdi.h"
#include "gdi_dc.h"

#include "gui_resource.h"
#include "gdi_private.h"
#include "gdi_anim.h"

/*!
  get transformations.
  */
typedef void(*anim_get_trans)(handle_t anim_handle, u32 cur_time, transformation_t *p_out_trans);

/*!
  get interpolator.
  */
typedef u32(*anim_get_inter)(u32 input);

/*!
  app transformations.
  */
typedef void(*anim_app_trans)(hdc_t hdc);

/*!
  animation type.
  */
typedef enum
{
  /*!
    rotate.
    */
  ANIMATION_ROTATE = 0,
  /*!
    translate.
    */
  ANIMATION_TRANSLATE,
  /*!
    alpha
    */
  ANIMATION_ALPHA,
  /*!
    scale
    */
  ANIMATION_SCALE,
  /*!
    step
    */
  ANIMATION_STEP,    
}anim_type_t;

typedef struct
{
  u32 start_time;
  u32 duration;
  u32 repeat_count;
  u32 repeated;
  u32 start_offset;
  u32 context;
  double scale_factor;
  BOOL is_end;
  BOOL is_cycle_flip;
  BOOL is_rpt_revse;
  
  transformation_t anim_trans;

  anim_type_t anim_type;

  anim_get_trans get_trans;

  anim_app_trans app_trans;

  anim_get_inter get_inter;

  anim_end_notify end_cb;

  anim_rpt_notify rpt_cb;
}animation_t;

/*!
  ratate animation.
  */
typedef struct
{
  /*!
    common anim data.
    */
  animation_t anim;
  /*!
    from degree, 0 ~ 360.
    */
  u16 from_degree;
  /*!
    to degree, 0 ~ 360.
    */
  u16 to_degree;
  /*!
    pivot x, 0 ~ 100.
    */
  u16 pivotx;
  /*!
    pivot y, 0 ~ 100.
    */
  u16 pivoty;
}rotate_anim_t;

/*!
  translate animation.
  */
typedef struct
{
  /*!
    common anim data.
    */
  animation_t anim;
  /*!
    from x.
    */
  u16 from_x;
  /*!
    to x.
    */
  u16 to_x;
  /*!
    from y.
    */
  u16 from_y;
  /*!
    to y
    */
  u16 to_y;  
}translate_anim_t;

/*!
  alpha animation.
  */
typedef struct
{
  /*!
    common anim data.
    */
  animation_t anim;
  /*!
    from alpha 0 ~ 0xFF.
    */
  u8 from_alpha;
  /*!
    to alpha, 0 ~ 0xFF.
    */
  u8 to_alpha;  
}alpha_anim_t;

/*!
  step animation.
  */
typedef struct
{
  /*!
    common anim data.
    */
  animation_t anim;
  /*!
    step animaton type.
    */
  step_type_t step_type;    
  /*!
    from alpha 0 ~ 0xFF.
    */
  u8 from_step;
  /*!
    to alpha, 0 ~ 0xFF.
    */
  u8 to_step;  
}step_anim_t;

/*!
  scale animation.
  */
typedef struct
{
  /*!
    common anim data.
    */
  animation_t anim;
  /*!
    from x scale, 0 mean zoom out to NONE, 100 means normal, more than 100 means zoom in.
    */
  u16 from_xscale;
  /*!
    to x scale, 0 mean zoom out to NONE, 100 means normal, more than 100 means zoom in.
    */
  u16 to_xscale;
  /*!
    from y scale, 0 mean zoom out to NONE, 100 means normal, more than 100 means zoom in.
    */
  u16 from_yscale;
  /*!
    to y scale, 0 mean zoom out to NONE, 100 means normal, more than 100 means zoom in.
    */
  u16 to_yscale;  
  /*!
    pivot x
    */
  u32 pivotx;
  /*!
    pivot y
    */
  u32 pivoty;    
}scale_anim_t;

/*!
  PI
  */
#define ANIM_PI 3.14159265358979323846

static void matrix_init(matrix_t *p_matrix,
  double xx, double xy, double yx,
  double yy, double x0, double y0)
{
  p_matrix->xx = xx; 
  p_matrix->xy = xy;
  p_matrix->yx = yx; 
  p_matrix->yy = yy;
  p_matrix->x0 = x0; 
  p_matrix->y0 = y0;
}

static void matrix_init_identity(matrix_t *p_matrix)
{
  matrix_init(p_matrix, 1, 0,
                        0, 1,
                        0, 0);
}

static void matrix_multiply (matrix_t *p_result, matrix_t *p_src1, matrix_t *p_src2)
{
  matrix_t tmp = {0};
  
  tmp.xx = p_src1->xx * p_src2->xx + p_src1->xy * p_src2->yx;

  tmp.xy = p_src1->xx * p_src2->xy + p_src1->xy * p_src2->yy;

  tmp.yx = p_src1->yx * p_src2->xx + p_src1->yy * p_src2->yx;

  tmp.yy = p_src1->yx * p_src2->xy + p_src1->yy * p_src2->yy;

  tmp.x0 = p_src1->xx * p_src2->x0 + p_src1->xy * p_src2->y0 + p_src2->x0;
  tmp.y0 = p_src1->yx * p_src2->x0 + p_src1->yy * p_src2->y0 + p_src2->y0;

  *p_result = tmp;
}

static void matrix_init_translate (matrix_t *p_matrix, double tx, double ty)
{
  matrix_init (p_matrix, 1, 0, 
                         0, 1,
                         tx, ty);
}

static void matrix_translate (matrix_t *p_matrix, double tx, double ty)
{
  matrix_t tmp;

  matrix_init_translate (&tmp, tx, ty);

  matrix_multiply (p_matrix, &tmp, p_matrix);
}

static void matrix_init_rotate (matrix_t *p_matrix, double radians)
{
  double s = 0;
  double c = 0;

  s = sin(radians);
  c = cos(radians);

  matrix_init(p_matrix, c, 0 - s,
                        s, c,
                        0, 0);
}

static void matrix_rotate (matrix_t *p_matrix, double radians, double pivotx, double pivoty)
{
  matrix_t tmp = {0};

  matrix_init_rotate (&tmp, radians);

  matrix_multiply (p_matrix, &tmp, p_matrix);
}

static void matrix_init_scale (matrix_t *p_matrix, double sx, double sy)
{
  matrix_init(p_matrix, sx, 0,
                        0, sy,
                        0,  0);
}

static void matrix_scale (matrix_t *p_matrix, double sx, double sy, double pivotx, double pivoty)
{
  matrix_t tmp = {0};

  matrix_init_scale (&tmp, sx, sy);

  matrix_multiply(p_matrix, &tmp, p_matrix);
}

static u32 linear_get_inter(u32 input)
{
  return input;
}

static u32 cycle_get_inter(u32 input)
{
  return (u32)abs(sin(2 * ANIM_PI * input / 100) * 100);
}

static u32 accel_get_inter(u32 input)
{
  return (input * input);
}

static u32 decel_get_inter(u32 input)
{
  return (100 - (100 - input) * (100 - input));
}

static void rotate_anim_get_trans(handle_t anim_handle, 
  u32 inter_time, transformation_t *p_out_trans)
{
  rotate_anim_t *p_rotate_anim = (rotate_anim_t *)anim_handle;
  u32 degrees = 0;
  
  MT_ASSERT(p_rotate_anim != NULL);
  MT_ASSERT(p_out_trans != NULL);

  p_rotate_anim = (rotate_anim_t *)anim_handle;
  
  degrees = p_rotate_anim->from_degree +
    (((p_rotate_anim->to_degree - p_rotate_anim->from_degree) * inter_time) / 100);

  matrix_init(&p_rotate_anim->anim.anim_trans.matrix, 1, 0, 0, 1, 0, 0);

  if((p_rotate_anim->pivotx == 0) && (p_rotate_anim->pivoty == 0))
  {
    matrix_rotate(&p_out_trans->matrix, (double)degrees, 0, 0);
  }
  else
  {
    matrix_rotate(&p_out_trans->matrix, (double)degrees,
      (double)(p_rotate_anim->pivotx * p_rotate_anim->anim.scale_factor),
      (double)(p_rotate_anim->pivoty * p_rotate_anim->anim.scale_factor));
  }
}

static void translate_anim_get_trans(handle_t anim_handle,
  u32 inter_time, transformation_t *p_out_trans)
{
  translate_anim_t *p_translate_anim = (translate_anim_t *)anim_handle;
  u32 dx = 0, dy = 0;  
  
  MT_ASSERT(p_translate_anim != NULL);
  MT_ASSERT(p_out_trans != NULL);

  dx = p_translate_anim->from_x;
  dy = p_translate_anim->from_y;

  if(p_translate_anim->from_x != p_translate_anim->to_x)
  {
    dx = p_translate_anim->from_x +
      (((p_translate_anim->to_x - p_translate_anim->from_x) * inter_time) / 100);
  }

  if(p_translate_anim->from_y != p_translate_anim->to_y)
  {
    dy = p_translate_anim->from_y +
      (((p_translate_anim->to_y - p_translate_anim->from_y) * inter_time) / 100);
  }

  matrix_translate(&p_out_trans->matrix, (double)dx, (double)dy);
}

static void scale_anim_get_trans(handle_t anim_handle,
  u32 inter_time, transformation_t *p_out_trans)
{
  scale_anim_t *p_scale_anim = (scale_anim_t *)anim_handle;
  u32 sx = 100;
  u32 sy = 100;
  u32 scale = 0;;
  
  MT_ASSERT(p_scale_anim != NULL);
  MT_ASSERT(p_out_trans != NULL);

  scale = p_scale_anim->anim.scale_factor;

  if((p_scale_anim->from_xscale != 100) || (p_scale_anim->to_xscale != 100))
  {
    sx = p_scale_anim->from_xscale +
      (((p_scale_anim->to_xscale - p_scale_anim->from_xscale) * inter_time) / 100);
  }

   if((p_scale_anim->from_yscale != 100) || (p_scale_anim->to_yscale != 100))
  {
    sy = p_scale_anim->from_yscale +
      (((p_scale_anim->to_yscale - p_scale_anim->from_yscale) * inter_time) / 100);
  }
  
  if((p_scale_anim->pivotx == 0) && (p_scale_anim->pivoty == 0)) 
  {
    matrix_scale(&p_out_trans->matrix, (double)sx, (double)sy, 0, 0);
  } 
  else 
  {
    matrix_scale(&p_out_trans->matrix, (double)sx, (double)sy,
      p_scale_anim->anim.scale_factor * p_scale_anim->pivotx,
      p_scale_anim->anim.scale_factor * p_scale_anim->pivoty);
  }
}

static void alpha_anim_get_trans(handle_t anim_handle,
  u32 inter_time, transformation_t *p_out_trans)
{
  alpha_anim_t *p_alpha_anim = (alpha_anim_t *)anim_handle;
  
  MT_ASSERT(p_alpha_anim != NULL);
  MT_ASSERT(p_out_trans != NULL);

  if(p_alpha_anim->from_alpha <= p_alpha_anim->to_alpha)
  {
    p_out_trans->alpha = p_alpha_anim->from_alpha +
      (((p_alpha_anim->to_alpha - p_alpha_anim->from_alpha) * inter_time) / 100);
  }
  else
  {
    p_out_trans->alpha = p_alpha_anim->from_alpha -
      (((p_alpha_anim->from_alpha - p_alpha_anim->to_alpha) * inter_time) / 100);
  }
}

static void step_anim_get_trans(handle_t anim_handle,
  u32 inter_time, transformation_t *p_out_trans)
{
  step_anim_t *p_step_anim = (step_anim_t *)anim_handle;
  
  MT_ASSERT(p_step_anim != NULL);
  MT_ASSERT(p_out_trans != NULL);

  if(p_step_anim->from_step <= p_step_anim->to_step)
  {
    p_out_trans->step = p_step_anim->from_step +
      (((p_step_anim->to_step - p_step_anim->from_step) * inter_time) / 100);
  }
  else
  {
    p_out_trans->step = p_step_anim->from_step -
      (((p_step_anim->from_step - p_step_anim->to_step) * inter_time) / 100);
  }
}

void anim_set_inter(handle_t anim_handle, inter_type_t inter_type)
{
  animation_t *p_animation = (animation_t *)anim_handle;
  
  MT_ASSERT(p_animation != NULL);

  switch(inter_type)
  {
    case INTERPOLATOR_LINEAR:
      p_animation->get_inter = linear_get_inter;
      break;

    case INTERPOLATOR_CYCLE:
      p_animation->get_inter = cycle_get_inter;
      break;

    case INTERPOLATOR_ACCELERATE:
      p_animation->get_inter = accel_get_inter;
      break;

    case INTERPOLATOR_DECELERATE:
      p_animation->get_inter = decel_get_inter;
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

void anim_set_start_time(handle_t anim_handle, u32 start_time)
{
  animation_t *p_animation = (animation_t *)anim_handle;
  
  MT_ASSERT(p_animation != NULL);

  p_animation->start_time = start_time;
}

void anim_set_duration(handle_t anim_handle, u32 duration)
{
  animation_t *p_animation = (animation_t *)anim_handle;
  
  MT_ASSERT(p_animation != NULL);

  p_animation->duration = duration;
}

void anim_set_repeat(handle_t anim_handle, u32 repeat_count, BOOL is_rpt_revse)
{
  animation_t *p_animation = (animation_t *)anim_handle;
  
  MT_ASSERT(p_animation != NULL);

  p_animation->repeat_count = repeat_count;
  p_animation->is_rpt_revse = is_rpt_revse;
}

  anim_end_notify end_cb;

void anim_set_end_cb(handle_t anim_handle, anim_end_notify end_cb, u32 context)
{
  animation_t *p_animation = NULL;
  
  MT_ASSERT(anim_handle != NULL);

  p_animation = (animation_t *)anim_handle;

  p_animation->end_cb = end_cb;

  p_animation->context = context;
}

void anim_set_rpt_cb(handle_t anim_handle, anim_end_notify rpt_cb, u32 context)
{
  animation_t *p_animation = NULL;
  
  MT_ASSERT(anim_handle != NULL);

  p_animation = (animation_t *)anim_handle;

  p_animation->rpt_cb = rpt_cb;

  p_animation->context = context;
}

BOOL anim_app_transf(handle_t anim_handle, u32 cur_time, transformation_t *p_out_trans)
{
  u32 duration = 0;
  u32 normalized_time = 0;
  u32 interpolated_time = 0;
  animation_t *p_animation = NULL;
  BOOL is_expired = FALSE;
  BOOL is_more = FALSE;

  if(anim_handle == NULL)
  {
    return FALSE;
  }

  MT_ASSERT(p_out_trans != NULL);

  p_animation = (animation_t *)anim_handle;

  if(p_animation->start_time == 0) 
  {
    p_animation->start_time = cur_time;
  }

  duration = p_animation->duration;

  if(duration != 0) 
  {
    normalized_time = 
      (cur_time - (p_animation->start_time + p_animation->start_offset)) * 100 / duration;
  } 
  else 
  {
    normalized_time = (cur_time < p_animation->start_time) ? 0 : 100;
  }

  is_expired = (normalized_time >= 100) ? TRUE : FALSE;
  
  is_more = !is_expired;

  if(normalized_time > 100)
  {
    normalized_time = 100;
  }

  if((normalized_time >= 0) && (normalized_time <= 100)) 
  {
    if(p_animation->is_cycle_flip) 
    {
      normalized_time = 100 - normalized_time;
    }

    interpolated_time = p_animation->get_inter(normalized_time);

    matrix_init_identity(&p_out_trans->matrix);

    p_out_trans->alpha = 0xFF;

    p_animation->get_trans(anim_handle, interpolated_time, p_out_trans);
  }

  if(p_animation->app_trans != NULL)
  {
    p_animation->app_trans((hdc_t)p_animation->context);
  }

  if(is_expired) 
  {
    p_animation->repeated++;
  
    if (p_animation->repeat_count == p_animation->repeated) 
    {
      if(p_animation->is_end == FALSE) 
      {
        p_animation->is_end = TRUE;

        if(p_animation->end_cb != NULL)
        {
          p_animation->end_cb(p_animation->context);
        }
      }
    } 
    else 
    {
      if (p_animation->is_rpt_revse) 
      {
        p_animation->is_cycle_flip = !p_animation->is_cycle_flip;
      }      

      p_animation->start_time = 0;
      is_more = TRUE;

      if(p_animation->rpt_cb != NULL) 
      {
        p_animation->rpt_cb(p_animation->context);
      }
    }
  }

  return is_more;
}

static void alpha_anim_app_trans(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;

  if(p_dc == NULL)
  {
    return;
  }

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(animation flinger ------> tmp flinger)
    flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
      p_dc->p_anim_tmp_flinger, (u16)orc.left, (u16)orc.top,
      FLINGER_ROP_PATCOPY, 255 - p_dc->trans.alpha);  

    //process new flinger(new flinger --->  tmp flinger).
    flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
      p_dc->p_anim_tmp_flinger, (u16)orc.left, (u16)orc.top, FLINGER_ROP_BLEND_DST, 0);


    //process tmp flinger(tmp flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_tmp_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void zrotate_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  rect_t src = {0}, drc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_ZROTATE);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //clear tmp flinger.
    flinger_fill_rect(p_dc->p_anim_tmp_flinger, &orc, 0);

    if(p_dc->trans.step <= 50)
    {
      //dert scale old flinger to tmp flinger.
      temp = RECTW(orc) * p_dc->trans.step / 100;

      src.left = 0;
      src.top = 0;
      src.right = RECTW(orc);
      src.bottom = RECTH(orc);

      drc.left = temp;
      drc.top = 0;
      drc.right = (RECTW(orc) - drc.left);
      drc.bottom = RECTH(orc);

      flinger_dert_scale(p_dc->p_anim_old_flinger, &src, p_dc->p_anim_tmp_flinger, &drc);
    }
    else if(p_dc->trans.step < 100)
    {
      //dert scale old flinger to tmp flinger.
      temp = RECTW(orc) * p_dc->trans.step / 100;

      src.left = 0;
      src.top = 0;
      src.right = RECTW(orc);
      src.bottom = RECTH(orc);

      drc.left = (RECTW(orc) - temp);
      drc.top = 0;
      drc.right = (RECTW(orc) - drc.left);
      drc.bottom = RECTH(orc);

      //dert scale new flinger to tmp flinger.
      flinger_dert_scale(p_dc->p_anim_new_flinger, &src, p_dc->p_anim_tmp_flinger, &drc);
    }  
    else
    {
      src.left = 0;
      src.top = 0;
      src.right = RECTW(orc);
      src.bottom = RECTH(orc);

      drc.left = (RECTW(orc) - temp);
      drc.top = 0;
      drc.right = (RECTW(orc) - drc.left);
      drc.bottom = RECTH(orc);

      //dert scale new flinger to tmp flinger.
      flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, RECTW(orc), RECTH(orc),
        p_dc->p_anim_tmp_flinger, 0, 0, FLINGER_ROP_SET, 0); 
    }

    //process tmp flinger(tmp flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_tmp_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void left_in_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_IN_LEFT);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(old flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(new flinger --->  curn flinger).
    temp = (RECTW(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTW(orc))
    {
      temp = RECTW(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, RECTW(orc) - temp, 0, temp, (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void left_out_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_OUT_LEFT);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(new flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(old flinger --->  curn flinger).
    temp = (RECTW(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTW(orc))
    {
      temp = RECTW(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, temp, 0, RECTW(orc) - temp, (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void right_in_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_IN_RIGHT);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(old flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(new flinger --->  curn flinger).
    temp = (RECTW(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTW(orc))
    {
      temp = RECTW(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, temp, (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.right - temp, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void right_out_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_OUT_RIGHT);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process new flinger(new flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(old flinger --->  curn flinger).
    temp = (RECTW(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTW(orc))
    {
      temp = RECTW(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, RECTW(orc) - temp, (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left + temp, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void top_in_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_IN_TOP);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(old flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(new flinger --->  curn flinger).
    temp = (RECTH(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTH(orc))
    {
      temp = RECTH(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, RECTH(orc) - temp, (u16)RECTW(orc), temp,
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void top_out_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_OUT_TOP);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(new flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(old flinger --->  curn flinger).
    temp = (RECTH(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTH(orc))
    {
      temp = RECTH(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, temp, (u16)RECTW(orc), RECTH(orc) - temp,
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void bottom_in_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_IN_BOTTOM);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(old flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(new flinger --->  curn flinger).
    //process new flinger with step(new flinger --->  curn flinger).
    temp = (RECTH(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTH(orc))
    {
      temp = RECTH(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), temp,
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.bottom - temp,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void bottom_out_step(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;
  rect_t orc = {0}, eff_rc = {0};
  cliprc_t *p_crc = NULL;
  u32 temp = 0;

  MT_ASSERT(p_dc != NULL);
  
  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);
  MT_ASSERT(p_step_anim->step_type == STEP_OUT_BOTTOM);

  copy_rect(&orc, &p_dc->vtrl_rc);

  /* set output rect */
  copy_rect(&p_dc->rc_output, &orc);
  normalize_rect(&p_dc->rc_output);

  if(dc_enter_drawing(p_dc) >= 0)
  {
    gdi_start_batch(dc_is_top_hdc(hdc));

    //process old flinger(new flinger --->  curn flinger).
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_new_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                       p_dc->p_curn_flinger, (u16)orc.left, (u16)orc.top,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }

    //process new flinger with step(old flinger --->  curn flinger).
    temp = (RECTH(orc) + (100 - 1)) / 100;

    temp = temp * p_dc->trans.step;

    if(temp > RECTH(orc))
    {
      temp = RECTH(orc);
    }
    
    p_crc = p_dc->ecrgn.p_head;
    while(NULL != p_crc)
    {
      if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
      {
        flinger_set_cliprect(p_dc->p_curn_flinger, &eff_rc);

        flinger_bitblt(p_dc->p_anim_old_flinger, 0, 0, (u16)RECTW(orc), RECTH(orc) - temp,
                       p_dc->p_curn_flinger, (u16)orc.left + temp, (u16)orc.bottom,
                       FLINGER_ROP_SET, 0);
      }
      p_crc = p_crc->p_next;
    }  

    //if dc is virtual dc, blt from virtual surface to physical surface.
    if(dc_is_virtual_dc(p_dc))
    {
      p_crc = p_dc->ecrgn.p_head;
      
      while(NULL != p_crc)
      {
        if(intersect_rect(&eff_rc, &p_dc->rc_output, &p_crc->rc))
        {
          flinger_set_cliprect(p_dc->p_back_flinger, &eff_rc);

          flinger_bitblt(p_dc->p_curn_flinger, 0, 0, (u16)RECTW(orc), (u16)RECTH(orc),
                         p_dc->p_back_flinger, (u16)orc.left, (u16)orc.top,
                         FLINGER_ROP_SET, 0);
        }

        p_crc = p_crc->p_next;
      }
    }

    dc_leave_drawing(p_dc);

    MT_ASSERT(dc_is_inherit_dc(p_dc) == FALSE);
    
    gdi_end_batch(dc_is_top_hdc(hdc), TRUE, &p_dc->ecrgn.bound);
  }
}

static void step_anim_app_trans(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  step_anim_t *p_step_anim = NULL;

  if(p_dc == NULL)
  {
    return;
  }

  p_step_anim = (step_anim_t *)(p_dc->anim_handle);
  MT_ASSERT(p_step_anim != NULL);

  MT_ASSERT(p_step_anim->anim.anim_type == ANIMATION_STEP);

  switch(p_step_anim->step_type)
  {
    case STEP_ZROTATE:
      zrotate_step(hdc);
      break;

    case STEP_IN_LEFT:
      left_in_step(hdc);
      break;

    case STEP_IN_RIGHT:
      right_in_step(hdc);
      break;

    case STEP_IN_TOP:
      top_in_step(hdc);
      break;

    case STEP_IN_BOTTOM:
      bottom_in_step(hdc);
      break;   

    case STEP_OUT_LEFT:
      left_out_step(hdc);
      break;

    case STEP_OUT_RIGHT:
      right_out_step(hdc);
      break;

    case STEP_OUT_TOP:
      top_out_step(hdc);
      break;

    case STEP_OUT_BOTTOM:
      bottom_out_step(hdc);
      break;

    default:
      MT_ASSERT(0);
      break;
  }
}

handle_t rotate_anim_new(u32 from_degree, u32 to_degree,
  u32 pivotx, u32 pivoty, inter_type_t inter)
{
  rotate_anim_t *p_rotate_anim = NULL;

  p_rotate_anim = (rotate_anim_t *)mmi_alloc_buf(sizeof(rotate_anim_t));

  if(p_rotate_anim == NULL)
  {
    return NULL;
  }

  memset(p_rotate_anim, 0 , sizeof(rotate_anim_t));

  p_rotate_anim->from_degree = from_degree;
  p_rotate_anim->to_degree = to_degree;

  //to set some other parameter.
  p_rotate_anim->anim.anim_type = ANIMATION_ROTATE;

  p_rotate_anim->anim.get_trans = rotate_anim_get_trans;

  anim_set_inter((animation_t *)p_rotate_anim, inter);
  
  anim_set_start_time((animation_t *)p_rotate_anim, mtos_ticks_get());
  
  anim_set_duration((animation_t *)p_rotate_anim, 0);
  
  anim_set_repeat((animation_t *)p_rotate_anim, 0, FALSE);

  return (handle_t)p_rotate_anim;
}

handle_t translate_anim_new(u32 from_x, u32 to_x, u32 from_y, u32 to_y, inter_type_t inter)
{
  translate_anim_t *p_translate_anim = NULL;

  p_translate_anim = (translate_anim_t *)mmi_alloc_buf(sizeof(translate_anim_t));

  if(p_translate_anim == NULL)
  {
    return NULL;
  }

  memset(p_translate_anim, 0 , sizeof(translate_anim_t));

  p_translate_anim->from_x = from_x;
  p_translate_anim->from_y = from_y;
  p_translate_anim->to_x = to_x;
  p_translate_anim->to_y = to_y;

  //to set some other parameter.
  p_translate_anim->anim.anim_type = ANIMATION_TRANSLATE;

  p_translate_anim->anim.get_trans = translate_anim_get_trans;

  anim_set_inter((animation_t *)p_translate_anim, inter);
  
  anim_set_start_time((animation_t *)p_translate_anim, mtos_ticks_get());
  
  anim_set_duration((animation_t *)p_translate_anim, 0);
  
  anim_set_repeat((animation_t *)p_translate_anim, 0, FALSE);
  
  return (handle_t)p_translate_anim;
}

handle_t alpha_anim_new(u32 from_alpha, u32 to_alpha, inter_type_t inter)
{
  alpha_anim_t *p_alpha_anim = NULL;

  p_alpha_anim = (alpha_anim_t *)mmi_alloc_buf(sizeof(alpha_anim_t));
  
  if(p_alpha_anim == NULL)
  {
    return NULL;
  }

  memset(p_alpha_anim, 0 , sizeof(alpha_anim_t));

  p_alpha_anim->from_alpha = from_alpha;
  p_alpha_anim->to_alpha = to_alpha;

  //to set some other parameter.
  p_alpha_anim->anim.anim_type = ANIMATION_ALPHA;

  p_alpha_anim->anim.get_trans = alpha_anim_get_trans;

  p_alpha_anim->anim.app_trans = alpha_anim_app_trans;

  anim_set_inter((animation_t *)p_alpha_anim, inter);
  
  anim_set_start_time((animation_t *)p_alpha_anim, mtos_ticks_get());
  
  anim_set_duration((animation_t *)p_alpha_anim, 0);
  
  anim_set_repeat((animation_t *)p_alpha_anim, 0, FALSE);
  
  return (handle_t)p_alpha_anim;
}

handle_t scale_anim_new(u32 from_xscale, u32 to_xscale,
  u32 from_yscale, u32 to_yscale, u32 pivotx, u32 pivoty, inter_type_t inter)
{
  scale_anim_t *p_scale_anim = NULL;

  p_scale_anim = (scale_anim_t *)mmi_alloc_buf(sizeof(scale_anim_t));

  if(p_scale_anim == NULL)
  {
    return NULL;
  }

  memset(p_scale_anim, 0 , sizeof(scale_anim_t));
  
  p_scale_anim->from_xscale = from_xscale;
  p_scale_anim->from_yscale = from_yscale;
  p_scale_anim->to_xscale = to_xscale;
  p_scale_anim->to_yscale = to_yscale;
  p_scale_anim->pivotx = pivotx;
  p_scale_anim->pivoty = pivoty;

  //to set some other parameter.
  p_scale_anim->anim.anim_type = ANIMATION_SCALE;

  p_scale_anim->anim.get_trans = scale_anim_get_trans;

  anim_set_inter((animation_t *)p_scale_anim, inter);
  
  anim_set_start_time((animation_t *)p_scale_anim, mtos_ticks_get());
  
  anim_set_duration((animation_t *)p_scale_anim, 0);
  
  anim_set_repeat((animation_t *)p_scale_anim, 0, FALSE);
  
  return (handle_t)p_scale_anim;
}

handle_t step_anim_new(step_type_t step_type, inter_type_t inter)
{
  step_anim_t *p_step_anim = NULL;

  p_step_anim = (step_anim_t *)mmi_alloc_buf(sizeof(step_anim_t));
  
  if(p_step_anim == NULL)
  {
    return NULL;
  }

  memset(p_step_anim, 0 , sizeof(step_anim_t));

  p_step_anim->from_step = 0;
  p_step_anim->to_step = 100;

  //to set some other parameter.
  p_step_anim->anim.anim_type = ANIMATION_STEP;

  p_step_anim->step_type = step_type;

  p_step_anim->anim.get_trans = step_anim_get_trans;

  p_step_anim->anim.app_trans = step_anim_app_trans;

  anim_set_inter((animation_t *)p_step_anim, inter);
  
  anim_set_start_time((animation_t *)p_step_anim, mtos_ticks_get());
  
  anim_set_duration((animation_t *)p_step_anim, 0);
  
  anim_set_repeat((animation_t *)p_step_anim, 0, FALSE);
  
  return (handle_t)p_step_anim;
}

void anim_free(handle_t anim_handle)
{
  if(anim_handle != NULL)
  {
    mmi_free_buf((void *)anim_handle);
  }

  return;
}



BOOL is_matrix_identity(matrix_t *p_matrix)
{
  return (p_matrix->xx == 1 &&
          p_matrix->yx == 0 &&
          p_matrix->xy == 0 &&
          p_matrix->yy == 0 &&
          p_matrix->x0 == 0 &&
          p_matrix->y0 == 0);
}

RET_CODE gdi_start_anim(hdc_t hdc)
{
  dc_t *p_dc = (dc_t *)hdc;
  u32 cur_time = 0;
  BOOL ret_boo = TRUE;

  if(p_dc == NULL)
  {
    return ERR_FAILURE;
  }

  //blt new animation flinger.
  flinger_bitblt(p_dc->p_curn_flinger,
                 p_dc->vtrl_rc.left, p_dc->vtrl_rc.top, RECTW(p_dc->vtrl_rc), RECTH(p_dc->vtrl_rc),
                 p_dc->p_anim_new_flinger, 0, 0, FLINGER_ROP_SET, 0);

  while(ret_boo)
  {
    cur_time = mtos_ticks_get();

    ret_boo = anim_app_transf(p_dc->anim_handle, cur_time, &p_dc->trans);
  }
  
  return SUCCESS;
}

RET_CODE gdi_set_anim(hdc_t hdc, handle_t anim_handle)
{
  dc_t *p_dc = (dc_t *)hdc;

  if(p_dc == NULL)
  {
    return ERR_FAILURE;
  }

  MT_ASSERT(p_dc->anim_handle == NULL);

  p_dc->anim_handle = anim_handle;

  p_dc->type |= TYPE_ANIM;
  
  return SUCCESS;
}

RET_CODE gdi_create_anim_surf(hdc_t hdc)
{
  void *p_vflinger = NULL, *p_ref_flinger = NULL;
  flinger_virtual_param_t param = {0};
  flinger_cfg_t scr_cfg = {0};
  gdi_main_t *p_gdi_info = NULL;
  rect_t scr_rect = {0}, rgn_rect = {0};
  dc_t *p_dc = dc_hdc2pdc(hdc);
  u32 pitch = 0, size = 0;
  u16 width = 0, height = 0;
  u8 bpp = 0;

  MT_ASSERT(p_dc->p_anim_new_flinger == NULL);
  MT_ASSERT(p_dc->p_anim_old_flinger == NULL);
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  width = RECTW(p_dc->vtrl_rc);
  height = RECTH(p_dc->vtrl_rc);
  
  if((width == 0) || (height == 0))
  {
    return ERR_FAILURE;
  }

  if((p_gdi_info->p_anim_new == NULL) 
    || (p_gdi_info->p_anim_old == NULL) 
    || (p_gdi_info->p_anim_tmp == NULL))
  {
    return ERR_FAILURE;
  }

  p_ref_flinger = p_dc->p_curn_flinger;

  bpp = flinger_get_bpp(p_ref_flinger);

  pitch = (bpp * width) >> 3;
  size = pitch * height;

  if((size > p_gdi_info->anim_new_size) 
    || (size > p_gdi_info->anim_old_size)
    || (size > p_gdi_info->anim_tmp_size))
  {    
    return ERR_FAILURE;
  }

  //create new animation flinger.
  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_gdi_info->p_anim_new;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);
  
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);

  p_dc->p_anim_new_flinger = p_vflinger;

  //create tmp animation flinger.
  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_gdi_info->p_anim_tmp;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);
  
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);

  p_dc->p_anim_tmp_flinger = p_vflinger;  

  //create old animation flinger.
  scr_rect.right = width;
  scr_rect.bottom = height;
  
  scr_cfg.rect_cnt = 1;
  rgn_rect.left = 0;
  rgn_rect.top = 0;
  rgn_rect.right = RECTW(scr_rect);
  rgn_rect.bottom = RECTH(scr_rect);

  scr_cfg.p_rgn_rect = &rgn_rect;

  param.p_virtual_buf = p_gdi_info->p_anim_old;
  param.p_pal = flinger_get_palette_addr(p_ref_flinger);
  param.format = flinger_get_format(p_ref_flinger);
  param.pitch = pitch;
  param.p_cfg = &scr_cfg;
  param.p_flinger_rect = &scr_rect;

  p_vflinger = flinger_create_virtual(&param);
  MT_ASSERT(p_vflinger != NULL);
  
  flinger_set_colorkey(p_vflinger, SURFACE_INVALID_COLORKEY);  

  p_dc->p_anim_old_flinger = p_vflinger;

  //save old flinger.
  flinger_bitblt(p_dc->p_curn_flinger,
                 p_dc->vtrl_rc.left, p_dc->vtrl_rc.top, RECTW(p_dc->vtrl_rc), RECTH(p_dc->vtrl_rc),
                 p_dc->p_anim_old_flinger, 0, 0, FLINGER_ROP_SET, 0);
  
  p_dc->type |= TYPE_ANIM;

  return 0;
}

RET_CODE gdi_delete_anim_surf(hdc_t hdc)
{
  dc_t *p_dc = dc_hdc2pdc(hdc);
    
  if(p_dc->type & TYPE_ANIM)
  {
    if(p_dc->p_anim_new_flinger != NULL)
    {
      flinger_delete(p_dc->p_anim_new_flinger);
    }
    
    p_dc->p_anim_new_flinger = NULL;

    if(p_dc->p_anim_old_flinger != NULL)
    {
      flinger_delete(p_dc->p_anim_old_flinger);
    }
    
    p_dc->p_anim_old_flinger = NULL;

    if(p_dc->p_anim_tmp_flinger != NULL)
    {
      flinger_delete(p_dc->p_anim_tmp_flinger);
    }
    
    p_dc->p_anim_tmp_flinger = NULL;
    
    return SUCCESS;
  }

  return ERR_FAILURE;
}

RET_CODE gdi_init_anim(u32 anim_addr, u32 anim_size)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  if((anim_addr == 0) || (anim_addr == 0))
  {
    gdi_release_anim();
    
    return ERR_FAILURE;
  }

  p_gdi_info->p_anim_new = (void *)anim_addr;

  p_gdi_info->anim_new_size = anim_size / 3;

  p_gdi_info->p_anim_old = (void *)((u32)p_gdi_info->p_anim_new + anim_size / 3);

  p_gdi_info->anim_old_size = anim_size / 3;

  p_gdi_info->p_anim_tmp = (void *)((u32)p_gdi_info->p_anim_old + anim_size / 3);

  p_gdi_info->anim_tmp_size = anim_size / 3;

  MT_ASSERT(((u32)p_gdi_info->p_anim_new) % 8 == 0);
  MT_ASSERT(((u32)p_gdi_info->p_anim_old) % 8 == 0);
  MT_ASSERT(((u32)p_gdi_info->p_anim_tmp) % 8 == 0);

  return SUCCESS;
}


void gdi_release_anim(void)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->p_anim_new = NULL;

  p_gdi_info->anim_new_size = 0;

  p_gdi_info->p_anim_old = NULL;

  p_gdi_info->anim_old_size = 0;
}
