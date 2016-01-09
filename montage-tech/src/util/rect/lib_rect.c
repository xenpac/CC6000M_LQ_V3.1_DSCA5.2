/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "sys_types.h"
#include "sys_define.h"

#include "string.h"

#include "lib_rect.h"

void set_rect(rect_t *p_rc, s16 left, s16 top, s16 right, s16 bottom)
{
  p_rc->left = left, p_rc->top = top;
  p_rc->right = right, p_rc->bottom = bottom;
}


void empty_rect(rect_t *p_rc)
{
  memset(p_rc, 0, sizeof(rect_t));
}


void copy_rect(rect_t *p_dst_rc, const rect_t *p_src_rc)
{
  memcpy(p_dst_rc, p_src_rc, sizeof(rect_t));
}


BOOL is_empty_rect(const rect_t *p_rc)
{
  if(p_rc->left == p_rc->right)
  {
    return TRUE;
  }
  if(p_rc->top == p_rc->bottom)
  {
    return TRUE;
  }

  return FALSE;
}

BOOL is_invalid_rect(const rect_t *p_rc)
{
  if(p_rc->left >= p_rc->right)
  {
    return TRUE;
  }
  if(p_rc->top >= p_rc->bottom)
  {
    return TRUE;
  }

  return FALSE;
}


BOOL is_equal_rect(const rect_t *p_rc1, const rect_t *p_rc2)
{
  if(p_rc1->left != p_rc2->left)
  {
    return FALSE;
  }
  if(p_rc1->top != p_rc2->top)
  {
    return FALSE;
  }
  if(p_rc1->right != p_rc2->right)
  {
    return FALSE;
  }
  if(p_rc1->bottom != p_rc2->bottom)
  {
    return FALSE;
  }

  return TRUE;
}


void normalize_rect(rect_t *p_rc)
{
  s16 tmp = 0;

  if(p_rc->left > p_rc->right)
  {
    tmp = p_rc->left;
    p_rc->left = p_rc->right;
    p_rc->right = tmp;
  }

  if(p_rc->top > p_rc->bottom)
  {
    tmp = p_rc->top;
    p_rc->top = p_rc->bottom;
    p_rc->bottom = tmp;
  }
}


/* 1 in 2 ? */
BOOL is_rect_covered(const rect_t *p_rc1, const rect_t *p_rc2)
{
  if(p_rc1->left < p_rc2->left
    || p_rc1->top < p_rc2->top
    || p_rc1->right > p_rc2->right
    || p_rc1->bottom > p_rc2->bottom)
  {
    return FALSE;
  }

  return TRUE;
}


BOOL is_rect_intersected(const rect_t *p_rc1, const rect_t *p_rc2)
{
  s16 left = MAX(p_rc1->left, p_rc2->left);
  s16 top = MAX(p_rc1->top, p_rc2->top);
  s16 right = MIN(p_rc1->right, p_rc2->right);
  s16 bottom = MIN(p_rc1->bottom, p_rc2->bottom);

  if(left >= right || top >= bottom)
  {
    return FALSE;
  }

  return TRUE;
}


BOOL intersect_rect(rect_t *p_dst_rc,
                    const rect_t *p_src_rc1,
                    const rect_t *p_src_rc2)
{
  p_dst_rc->left = MAX(p_src_rc1->left, p_src_rc2->left);
  p_dst_rc->top = MAX(p_src_rc1->top, p_src_rc2->top);
  p_dst_rc->right = MIN(p_src_rc1->right, p_src_rc2->right);
  p_dst_rc->bottom = MIN(p_src_rc1->bottom, p_src_rc2->bottom);

  if(p_dst_rc->left >= p_dst_rc->right || p_dst_rc->top >= p_dst_rc->bottom)
  {
    return FALSE;
  }

  return TRUE;
}


BOOL union_rect(rect_t *p_dst_rc,
                const rect_t *p_src_rc1,
                const rect_t *p_src_rc2)
{
  rect_t src1, src2;

  memcpy(&src1, p_src_rc1, sizeof(rect_t));
  memcpy(&src2, p_src_rc2, sizeof(rect_t));

  normalize_rect(&src1);
  normalize_rect(&src2);

  if(src1.left == src2.left && src1.right == src2.right)
  {
    if(src1.top <= src2.top && src2.top <= src1.bottom)
    {
      p_dst_rc->left = src1.left;
      p_dst_rc->right = src1.right;
      p_dst_rc->top = src1.top;
      p_dst_rc->bottom = MAX(src1.bottom, src2.bottom);

      return TRUE;
    }
    else if(src1.top >= src2.top && src2.bottom >= src1.top)
    {
      p_dst_rc->left = src1.left;
      p_dst_rc->right = src1.right;
      p_dst_rc->top = src2.top;
      p_dst_rc->bottom = MAX(src1.bottom, src2.bottom);

      return TRUE;
    }

    return FALSE;
  }

  if(src1.top == src2.top && src1.bottom == src2.bottom)
  {
    if(src1.left <= src2.left && src2.left <= src1.right)
    {
      p_dst_rc->top = src1.top;
      p_dst_rc->bottom = src1.bottom;
      p_dst_rc->left = src1.left;
      p_dst_rc->right = MAX(src1.right, src2.right);

      return TRUE;
    }
    else if(src1.left >= src2.left && src2.right >= src1.left)
    {
      p_dst_rc->top = src1.top;
      p_dst_rc->bottom = src1.bottom;
      p_dst_rc->left = src2.left;
      p_dst_rc->right = MAX(src1.right, src2.right);

      return TRUE;
    }

    return FALSE;
  }

  return FALSE;
}


u16 subtract_rect(rect_t *p_difference_rcs,
                  const rect_t *p_minuend_rc,
                  const rect_t *p_subtrahend_rc)
{
  rect_t src1, src2;
  u16 n = 0;

  memcpy(&src1, p_minuend_rc, sizeof(rect_t));
  memcpy(&src2, p_subtrahend_rc, sizeof(rect_t));

  if(!is_rect_intersected(&src1, &src2))
  {
    n = 1;
    p_difference_rcs[0] = src1;
  }
  else
  {
    // top 2 right
    if(src2.top > src1.top)
    {
      p_difference_rcs[n].left = src1.left;
      p_difference_rcs[n].top = src1.top;
      p_difference_rcs[n].right = src1.right;
      p_difference_rcs[n].bottom = src2.top;

      n++;
      src1.top = src2.top;
    }
    if(src2.bottom < src1.bottom)
    {
      p_difference_rcs[n].top = src2.bottom;
      p_difference_rcs[n].left = src1.left;
      p_difference_rcs[n].right = src1.right;
      p_difference_rcs[n].bottom = src1.bottom;

      n++;
      src1.bottom = src2.bottom;
    }
    // left 2 right
    if(src2.left > src1.left)
    {
      p_difference_rcs[n].left = src1.left;
      p_difference_rcs[n].top = src1.top;
      p_difference_rcs[n].right = src2.left;
      p_difference_rcs[n].bottom = src1.bottom;

      n++;
    }
    if(src2.right < src1.right)
    {
      p_difference_rcs[n].left = src2.right;
      p_difference_rcs[n].top = src1.top;
      p_difference_rcs[n].right = src1.right;
      p_difference_rcs[n].bottom = src1.bottom;

      n++;
    }
  }

  return n;
}


void generate_boundrect(rect_t *p_dst_rc,
                        const rect_t *p_src_rc1,
                        const rect_t *p_src_rc2)
{
  rect_t src1, src2;

  memcpy(&src1, p_src_rc1, sizeof(rect_t));
  memcpy(&src2, p_src_rc2, sizeof(rect_t));

  normalize_rect(&src1);
  normalize_rect(&src2);

  p_dst_rc->left = MIN(src1.left, src2.left);
  p_dst_rc->top = MIN(src1.top, src2.top);
  p_dst_rc->right = MAX(src1.right, src2.right);
  p_dst_rc->bottom = MAX(src1.bottom, src2.bottom);
}


void offset_rect(rect_t *p_rc, s16 x, s16 y)
{
  p_rc->left += x;
  p_rc->top += y;
  p_rc->right += x;
  p_rc->bottom += y;
}


void extend_rect(rect_t *p_rc, s16 cx, s16 cy)
{
  p_rc->left -= cx;
  p_rc->top -= cy;
  p_rc->right += cx;
  p_rc->bottom += cy;
}


void extend_rect_to_pt(rect_t *p_rc, s16 x, s16 y)
{
  if(x < p_rc->left)
  {
    p_rc->left = x;
  }
  if(y < p_rc->top)
  {
    p_rc->top = y;
  }
  if(x > p_rc->right)
  {
    p_rc->right = x;
  }
  if(y > p_rc->bottom)
  {
    p_rc->bottom = y;
  }
}


BOOL is_pt_in_rect(const rect_t *p_rc, s16 x, s16 y)
{
  if(x >= p_rc->left
    && x < p_rc->right
    && y >= p_rc->top
    && y < p_rc->bottom)
  {
    return TRUE;
  }

  return FALSE;
}
