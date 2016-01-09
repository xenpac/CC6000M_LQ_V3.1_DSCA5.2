/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"

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

#include "gdi_private.h"
#include "class_factory.h"

#ifdef GDI_DEBUG
//-static u32 cliprc_max_cnt;
//-static u32 cliprc_curn_cnt;
#endif


typedef void (*cb_overlap_t)(cliprgn_t * p_rgn,          \
                           const cliprc_t *p_rc1,      \
                           const cliprc_t *p_rc1_tail, \
                           const cliprc_t *p_rc2,      \
                           const cliprc_t *p_rc2_tail, \
                           int top, int bottom);

typedef void (*cb_non_overlap_t)(cliprgn_t *p_rgn,          \
                               const cliprc_t *p_rc,      \
                               const cliprc_t *p_rc_tail, \
                               int top, int bottom);

static cliprc_t *gdi_alloc_cliprect(lib_memf_t *p_memf)
{
#ifdef GDI_DEBUG 
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);
  
  p_gdi_info->cliprc_curn_cnt++;
  if(p_gdi_info->cliprc_curn_cnt > p_gdi_info->cliprc_max_cnt)
  {
    p_gdi_info->cliprc_max_cnt = p_gdi_info->cliprc_curn_cnt;
  }
#endif
  return (cliprc_t *)lib_memf_alloc(p_memf);
}

static void gdi_free_cliprect(lib_memf_t *p_memf, void *p_rc)
{
#ifdef GDI_DEBUG
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);

  p_gdi_info->cliprc_curn_cnt--;
#endif
  lib_memf_free(p_memf, p_rc);
}

#ifdef GDI_DEBUG
void gdi_cliprc_usage(void)
{
  gdi_main_t *p_gdi_info = NULL;
  
  p_gdi_info = (gdi_main_t *)class_get_handle_by_id(GDI_CLASS_ID);  
  MT_ASSERT(p_gdi_info != NULL);
  
  GDI_PRINTF("-------------------------\n");
  GDI_PRINTF(" CLIPRC max = %d\n", p_gdi_info->cliprc_max_cnt);
  GDI_PRINTF("-------------------------\n");
}
#endif

/*!
  static functions
  */
#ifdef GDI_DEBUG
void gdi_dump_cliprgn(cliprgn_t *p_rgn)
{
  u16 i = 0;
  cliprc_t *p_rc;

  GDI_PRINTF("rgn = 0x%.8x\nbound (%3d, %3d, %3d, %3d)\n", 
    p_rgn, p_rgn->bound.left, p_rgn->bound.top, 
    RECTW(p_rgn->bound), RECTH(p_rgn->bound));

  p_rc = p_rgn->p_head;
  while(p_rc != NULL)
  {
    GDI_PRINTF("rc%.2d  (%3d, %3d, %3d, %3d)\n", 
      i + 1, p_rc->rc.left, p_rc->rc.top,
      RECTW(p_rc->rc), RECTH(p_rc->rc));
    p_rc = p_rc->p_next, i++;
  }
}
#endif

/*
 * calculate the bound of a region
 */
static void calc_bound(cliprgn_t *p_rgn)
{
  cliprc_t *p_rc = NULL;
  rect_t *p_bound = NULL;

  if(p_rgn->p_head == NULL)
  {
    p_rgn->bound.left = 0; p_rgn->bound.top = 0;
    p_rgn->bound.right = 0;p_rgn->bound.bottom = 0;
    return;
  }

  p_bound = &p_rgn->bound;

  /*
   * since head is the first rectangle in the region, it must have the
   * smallest top and since tail is the last rectangle in the region,
   * it must have the largest bottom, because of banding. Initialize left and
   * right from head and tail, resp., as good things to initialize them
   * to...
   */
  p_bound->left = p_rgn->p_head->rc.left;
  p_bound->top = p_rgn->p_head->rc.top;
  p_bound->right = p_rgn->p_tail->rc.right;
  p_bound->bottom = p_rgn->p_tail->rc.bottom;

  p_rc = p_rgn->p_head;
  while(NULL != p_rc)
  {
    if(p_rc->rc.left < p_bound->left)
    {
      p_bound->left = p_rc->rc.left;
    }
    if(p_rc->rc.right > p_bound->right)
    {
      p_bound->right = p_rc->rc.right;
    }

    p_rc = p_rc->p_next;
  }
}


/*
 *  allocate a new clipping rect and link to the region's tail.
 */
static void alloc_cliprect(cliprgn_t *p_rgn, cliprc_t **p_rc)
{
  *p_rc = gdi_alloc_cliprect(p_rgn->p_heap);
  if(*p_rc == NULL)
  {
    MT_ASSERT(0);
    *p_rc = NULL;
    return;
  }
  // link to rgn's tail
  (*p_rc)->p_next = NULL;
  (*p_rc)->p_prev = p_rgn->p_tail;

  if(p_rgn->p_tail != NULL)
  {
    p_rgn->p_tail->p_next = *p_rc;
  }
  p_rgn->p_tail = *p_rc;

  if(p_rgn->p_head == NULL)
  {
    p_rgn->p_head = *p_rc;
  }
}


/*
 *		   merge_band
 *
 *	  attempt to merge the rects in the current band with those in the
 *	  previous one. Used only by REGION_RegionOp.
 *
 * Results:
 *	  The new index for the previous band.
 *
 * Side Effects:
 *	  If coalescing takes place:
 *		  - rectangles in the previous band will have their bottom fields
 *		    altered.
 *		  - some clipping rect will be deleted.
 *
 */
static cliprc_t *merge_band(
  cliprgn_t *p_rgn,          /* the region that contained bands */
  cliprc_t *p_prev_band,     /* the header of previous band */
  cliprc_t *p_curn_band /* the header of current band */)
{
  cliprc_t *p_dst_band = NULL;/* the header of destination band */
  cliprc_t *p_prev_band_rc = NULL;/* the current rect of previous band */
  cliprc_t *p_curn_band_rc = NULL;/* the current rect of current band */
  cliprc_t *p_temp_rc = NULL;/* the temporary rect */
  int curn_band_rc_cnt = 0;/* the count of rects in current band */
  int prev_band_rc_cnt = 0;/* the count of rects in previous band */
  int top = 0;/* the top coordinate of current band */

  if(p_prev_band == NULL)
  {
    p_prev_band = p_rgn->p_head;
  }
  if(p_curn_band == NULL)
  {
    p_curn_band = p_rgn->p_head;
  }

  if(p_prev_band == p_curn_band)
  {
    return p_prev_band;
  }

  p_dst_band = p_curn_band_rc =p_curn_band;

  p_prev_band_rc = p_prev_band;
  p_temp_rc = p_prev_band;
  prev_band_rc_cnt = 0;
  while(p_temp_rc != p_curn_band)
  {
    prev_band_rc_cnt++;
    p_temp_rc = p_temp_rc->p_next;
  }

  /*
   * Figure out how many rectangles are in the current band. Have to do
   * this because multiple bands could have been added in REGION_RegionOp
   * at the end when one region has been exhausted.
   */
  p_curn_band_rc = p_curn_band;
  top = p_curn_band_rc->rc.top;
  curn_band_rc_cnt = 0;
  while(p_curn_band_rc && (p_curn_band_rc->rc.top == top))
  {
    curn_band_rc_cnt++;
    p_curn_band_rc = p_curn_band_rc->p_next;
  }

  if(NULL != p_curn_band_rc)
  {
    /*
     * If more than one band was added, we have to find the start
     * of the last band added so the next coalescing job can start
     * at the right place... (given when multiple bands are added,
     * this may be pointless -- see above).
     */
    p_temp_rc = p_rgn->p_tail;
    while(p_temp_rc->p_prev->rc.top == p_temp_rc->rc.top)
    {
      p_temp_rc = p_temp_rc->p_prev;
    }
    p_dst_band = p_temp_rc;
  }

  if((curn_band_rc_cnt == prev_band_rc_cnt) && (curn_band_rc_cnt != 0))
  {
    p_curn_band_rc = p_curn_band;
    /*
     * The bands may only be coalesced if the bottom of the previous
     * matches the top scanline of the current.
     */
    if(p_prev_band_rc->rc.bottom == p_curn_band_rc->rc.top)
    {
      /*
       * Make sure the bands have rects in the same places. This
       * assumes that rects have been added in such a way that they
       * cover the most area possible. I.e. two rects in a band must
       * have some horizontal space between them.
       */
      do
      {
        if((p_prev_band_rc->rc.left != p_curn_band_rc->rc.left)
            || (p_prev_band_rc->rc.right != p_curn_band_rc->rc.right))
        {
          /*
           * The bands don't line up so they can't be coalesced.
           */
          return p_dst_band;
        }
        p_prev_band_rc = p_prev_band_rc->p_next;
        p_curn_band_rc = p_curn_band_rc->p_next;

        (prev_band_rc_cnt--);
        
      } while(prev_band_rc_cnt);

      /*
       * If only one band was added to the region, we have to backup
       * dst_band to the start of the previous band.
       */
      if(p_curn_band_rc == NULL)
      {
        p_dst_band = p_prev_band;
      }

      /*
       * The bands may be merged, so set the bottom of each rect
       * in the previous band to that of the corresponding rect in
       * the current band.
       */
      /*
       * for implementation of MiniGUI, we should free
       * the clipping rects merged.
       */
      p_curn_band_rc = p_curn_band;
      p_prev_band_rc = p_prev_band;
      do
      {
        p_prev_band_rc->rc.bottom = p_curn_band_rc->rc.bottom;
        p_prev_band_rc = p_prev_band_rc->p_next;

        if(NULL != p_curn_band_rc->p_next)
        {
          p_curn_band_rc->p_next->p_prev = p_curn_band_rc->p_prev;
        }
        else
        {
          p_rgn->p_tail = p_curn_band_rc->p_prev;
        }
        if(NULL != p_curn_band_rc->p_prev)
        {
          p_curn_band_rc->p_prev->p_next = p_curn_band_rc->p_next;
        }
        else
        {
          p_rgn->p_head = p_curn_band_rc->p_next;
        }

        p_temp_rc = p_curn_band_rc->p_next;
        gdi_free_cliprect(p_rgn->p_heap, p_curn_band_rc);
        p_curn_band_rc = p_temp_rc;

        (curn_band_rc_cnt--);
        
      } while(curn_band_rc_cnt);

      /*
       *
       * If more than one band was added to the region, copy the
       * other bands down. The assumption here is that the other bands
       * came from the same region as the current one and no further
       * coalescing can be done on them since it's all been done
       * already... dst_band is already in the right place.
       */
      /* no need to copy for implementation of MiniGUI -- they are freed.
         if(temp_rc == regionEnd) {
         dst_band = prev_band;
         }

         else {
         do {
       *prev_band_rc++ = *curn_band_rc++;
         } while(curn_band_rc != regionEnd);
         }
       */
    }
  }
  return(p_dst_band);
}


/*
 +-------------------+
 |         1         |-> band 1
 |-----+-------+-----|
 |     |||||||||     |
 |  2  |||||||||  3  |-> band 2
 |     |||||||||     |
 |-----+-------+-----|
 |         4         |-> band 3
 +-------------------+
 */
/*
 * 		  cliprgn_set_op
 *
 *	  apply an operation to two regions. Called by Union,
 *	  Xor, Subtract, Intersect...
 *
 * Results:
 *	  None.
 *
 * Side Effects:
 *	  The new region is overwritten.
 *
 * Notes:
 *	  The idea behind this function is to view the two regions as sets.
 *	  Together they cover a rectangle of area that this function divides
 *	  into horizontal bands where points are covered only by one region
 *	  or by both. For the first case, the nonOverlapFunc is called with
 *	  each the band and the band's upper and lower bound. For the
 *	  second, the do_overlap is called to process the entire band. It
 *	  is responsible for clipping the rectangles in the band, though
 *	  this function provides the boundaries.
 *	  At the end of each band, the new region is coalesced, if possible,
 *	  to reduce the number of rectangles in the region.
 *
 */
static void
cliprgn_set_op(
  cliprgn_t *p_dst_rgn,        /* destination region */
  const cliprgn_t *p_src_rgn1, /* source region 1 */
  const cliprgn_t *p_src_rgn2, /* source region 2 */
  cb_overlap_t p_do_overlap,          /* callback function for overlap */
  cb_non_overlap_t p_do_non_overlap1, //cbfunction for non-overlap in src_rgn1
  cb_non_overlap_t p_do_non_overlap2) //cb function for non-overlap in src_rgn2
{
  cliprgn_t tmp_rgn = {{0}};   /* a temporary region, if need */
  cliprgn_t *p_rgn = NULL;/* point to destination region */
  const cliprc_t *p_rc1 = NULL;/* point to the header of band in src_rgn1 */
  const cliprc_t *p_rc2 = NULL;/* point to the header of band in src_rgn2 */
  const cliprc_t *p_rc1_tail = NULL;/* point to the tail of band in src_rgn1 */
  const cliprc_t *p_rc2_tail = NULL;/* point to the tail of band in src_rgn2 */
  int ybot = 0;/* the bottom of intersected part */
  int ytop = 0;/* the top of intersected part */
  /*!
    point to the header of previous band in dst_rgn.
    */
  cliprc_t *p_prev_band = NULL;
  /*!
    point to the header of current band in dst_rgn.
    */
  cliprc_t *p_curn_band = NULL;
  /*!
    the top of non-overlapping band.
    */
  int ntop = 0;
  /*!
    the bottom of non-overlapping band.
    */
  int nbot = 0;

  /*
   * Initialization:
   * set rc1, rc2, rc1_tail and rc2_tail appropriately, preserve the important
   * parts of the destination region until the end in case it's one of
   * the two source regions, then mark the "new" region empty, allocating
   * another array of rectangles for it to use.
   */
  p_rc1 = p_src_rgn1->p_head;
  p_rc2 = p_src_rgn2->p_head;

  /*
   * dst_rgn may be one of the source regions so we create a new region
   * otherwise empty it.
   */
  if(p_dst_rgn == p_src_rgn1 || p_dst_rgn == p_src_rgn2)
  {
    gdi_init_cliprgn(&tmp_rgn, p_dst_rgn->p_heap);
    p_rgn = &tmp_rgn;
  }
  else
  {
    gdi_empty_cliprgn(p_dst_rgn);
    p_rgn = p_dst_rgn;
  }

  /*
   * Initialize ybot and ytop.
   * In the upcoming loop, ybot and ytop serve different functions depending
   * on whether the band being handled is an overlapping or non-overlapping
   * band.
   *  In the case of a non-overlapping band (only one of the regions
   * has points in the band), ybot is the bottom of the most recent
   * intersection and thus clips the top of the rectangles in that band.
   * ytop is the top of the next intersection between the two regions and
   * serves to clip the bottom of the rectangles in the current band.
   *  For an overlapping band (where the two regions intersect), ytop clips
   * the top of the rectangles of both regions and ybot clips the bottoms.
   */
  if(p_src_rgn1->bound.top < p_src_rgn2->bound.top)
  {
    ybot = p_src_rgn1->bound.top;
  }
  else
  {
    ybot = p_src_rgn2->bound.top;
  }

  /*
   * prev_band serves to mark the start of the previous band so rectangles
   * can be coalesced into larger rectangles. qv. miCoalesce, above.
   * In the beginning, there is no previous band, so prev_band == curn_band
   * (curn_band is set later on, of course, but the first band will always
   * start at index 0). prev_band and curn_band must be indices because of
   * the possible expansion, and resultant moving, of the new region's
   * array of rectangles.
   */
  p_prev_band = p_rgn->p_head;

  do
  {
    p_curn_band = p_rgn->p_tail;

    /*
     * This algorithm proceeds one source-band (as opposed to a
     * destination band, which is determined by where the two regions
     * intersect) at a time. r1BandEnd and rc2_tail serve to mark the
     * rectangle after the last one in the current band for their
     * respective regions.
     */
    p_rc1_tail = p_rc1;
    while(p_rc1_tail && (p_rc1_tail->rc.top == p_rc1->rc.top))
    {
      p_rc1_tail = p_rc1_tail->p_next;
    }

    p_rc2_tail = p_rc2;
    while(p_rc2_tail && (p_rc2_tail->rc.top == p_rc2->rc.top))
    {
      p_rc2_tail = p_rc2_tail->p_next;
    }

    /*
     * First handle the band that doesn't intersect, if any.
     *
     * Note that attention is restricted to one band in the
     * non-intersecting region at once, so if a region has n
     * bands between the current position and the next place it overlaps
     * the other, this entire loop will be passed through n times.
     */
    if(p_rc1->rc.top < p_rc2->rc.top)
    {
      ntop = MAX(p_rc1->rc.top, ybot);
      nbot = MIN(p_rc1->rc.bottom, p_rc2->rc.top);

      if((ntop != nbot) && (p_do_non_overlap1 != NULL))
      {
        (*p_do_non_overlap1)(p_rgn, p_rc1, p_rc1_tail, ntop, nbot);
      }

      ytop = p_rc2->rc.top;
    }
    else if(p_rc2->rc.top < p_rc1->rc.top)
    {
      ntop = MAX(p_rc2->rc.top, ybot);
      nbot = MIN(p_rc2->rc.bottom, p_rc1->rc.top);

      if((ntop != nbot) && (p_do_non_overlap2 != NULL))
      {
        (*p_do_non_overlap2)(p_rgn, p_rc2, p_rc2_tail, ntop, nbot);
      }

      ytop = p_rc1->rc.top;
    }
    else
    {
      ytop = p_rc1->rc.top;
    }

    /*
     * If any rectangles got added to the region, try and coalesce them
     * with rectangles from the previous band. Note we could just do
     * this test in miCoalesce, but some machines incur a not
     * inconsiderable cost for function calls, so...
     */
    if(p_rgn->p_tail != p_curn_band)
    {
      p_prev_band = merge_band(p_rgn, p_prev_band, p_curn_band);
    }

    /*
     * Now see if we've hit an intersecting band. The two bands only
     * intersect if ybot > ytop
     */
    ybot = MIN(p_rc1->rc.bottom, p_rc2->rc.bottom);
    p_curn_band = p_rgn->p_tail;
    if(ybot > ytop)
    {
      (*p_do_overlap)(p_rgn, p_rc1, p_rc1_tail, p_rc2, p_rc2_tail, ytop, ybot);
    }

    if(p_rgn->p_tail != p_curn_band)
    {
      p_prev_band = merge_band(p_rgn, p_prev_band, p_curn_band);
    }

    /*
     * if we've finished with a band (bottom == ybot) we skip forward
     * in the region to the next band.
     */
    if(p_rc1->rc.bottom == ybot)
    {
      p_rc1 = p_rc1_tail;
    }
    if(p_rc2->rc.bottom == ybot)
    {
      p_rc2 = p_rc2_tail;
    }
  } while(p_rc1 && p_rc2);

  /*
   * deal with whichever region still has rectangles left.
   */
  p_curn_band = p_rgn->p_tail;
  if(NULL != p_rc1)
  {
    if(p_do_non_overlap1 != NULL)
    {
      do
      {
        p_rc1_tail = p_rc1;
        while((p_rc1_tail) && (p_rc1_tail->rc.top == p_rc1->rc.top))
        {
          p_rc1_tail = p_rc1_tail->p_next;
        }
        (*p_do_non_overlap1)(p_rgn, p_rc1, p_rc1_tail,
                           MAX(p_rc1->rc.top, ybot), p_rc1->rc.bottom);
        p_rc1 = p_rc1_tail;
      } while(p_rc1);
    }
  }
  else if((p_rc2) && (p_do_non_overlap2 != NULL))
  {
    do
    {
      p_rc2_tail = p_rc2;
      while((p_rc2_tail) && (p_rc2_tail->rc.top == p_rc2->rc.top))
      {
        p_rc2_tail = p_rc2_tail->p_next;
      }
      (*p_do_non_overlap2)(p_rgn, p_rc2, p_rc2_tail,
                         MAX(p_rc2->rc.top, ybot), p_rc2->rc.bottom);
      p_rc2 = p_rc2_tail;
    } while(NULL != p_rc2);
  }

  if(p_rgn->p_tail != p_curn_band)
  {
    (void)merge_band(p_rgn, p_prev_band, p_curn_band);
  }

  /*
   * A bit of cleanup. To keep regions from growing without bound,
   * we shrink the array of rectangles to match the new number of
   * rectangles in the region. This never goes to 0, however...
   *
   * Only do this stuff if the number of rectangles allocated is more than
   * twice the number of rectangles in the region (a simple optimization...).
   */
  if(p_rgn != p_dst_rgn)
  {
    gdi_empty_cliprgn(p_dst_rgn);
    *p_dst_rgn = tmp_rgn;
  }
}


/*
 *			 do_overlap_intersect
 *
 * Handle an overlapping band for REGION_Intersect.
 *
 * Results:
 *	  None.
 *
 * Side Effects:
 *	  Rectangles may be added to the region.
 *
 */
static void
do_overlap_intersect(cliprgn_t *p_rgn,
                     const cliprc_t *p_rc1, const cliprc_t *p_rc1_tail,
                     const cliprc_t *p_rc2, const cliprc_t *p_rc2_tail,
                     int top, int bottom)
{
  int left = 0, right = 0;
  cliprc_t *p_nrc = NULL;

  while((p_rc1 != p_rc1_tail) && (p_rc2 != p_rc2_tail))
  {
    left = MAX(p_rc1->rc.left, p_rc2->rc.left);
    right = MIN(p_rc1->rc.right, p_rc2->rc.right);

    /*
     * If there's any overlap between the two rectangles, add that
     * overlap to the new region.
     * There's no need to check for subsumption because the only way
     * such a need could arise is if some region has two rectangles
     * right next to each other. Since that should never happen...
     */
    if(left < right)
    {
      alloc_cliprect(p_rgn, &p_nrc);

      p_nrc->rc.left = left;
      p_nrc->rc.top = top;
      p_nrc->rc.right = right;
      p_nrc->rc.bottom = bottom;
    }

    /*
     * Need to advance the pointers. Shift the one that extends
     * to the right the least, since the other still has a chance to
     * overlap with that region's next rectangle, if you see what I mean.
     */
    if(p_rc1->rc.right < p_rc2->rc.right)
    {
      p_rc1 = p_rc1->p_next;
    }
    else if(p_rc2->rc.right < p_rc1->rc.right)
    {
      p_rc2 = p_rc2->p_next;
    }
    else
    {
      p_rc1 = p_rc1->p_next;
      p_rc2 = p_rc2->p_next;
    }
  }
}


/*
 *			 do_overlap_union
 *
 *	  Handle an overlapping band for the union operation. Picks the
 *	  left-most rectangle each time and merges it into the region.
 *
 * Results:
 *	  None.
 *
 * Side Effects:
 *	  Rectangles are overwritten in region->rects and region->numRects will
 *	  be changed.
 *
 */
static void
do_overlap_union(cliprgn_t *p_rgn,
                 const cliprc_t *p_rc1, const cliprc_t *p_rc1_tail,
                 const cliprc_t *p_rc2, const cliprc_t *p_rc2_tail,
                 int top, int bottom)
{
  cliprc_t *p_nrc = NULL;
/* start of the macro */
#define MERGERECT(r)                          \
  if((p_rgn->p_head)                             \
      && (p_rgn->p_tail->rc.top == top)           \
      && (p_rgn->p_tail->rc.bottom == bottom)     \
      && (p_rgn->p_tail->rc.right >= r->rc.left)) \
  {                                           \
    if(p_rgn->p_tail->rc.right < r->rc.right)    \
    {                                         \
      p_rgn->p_tail->rc.right = r->rc.right;      \
    }                                         \
  }                                           \
  else                                        \
  {                                           \
    alloc_cliprect(p_rgn, &p_nrc);                \
    p_nrc->rc.top = top;                        \
    p_nrc->rc.bottom = bottom;                  \
    p_nrc->rc.left = r->rc.left;                \
    p_nrc->rc.right = r->rc.right;              \
  }                                           \
  r = r->p_next;
/* end of the macro */
  while((p_rc1 != p_rc1_tail) && (p_rc2 != p_rc2_tail))
  {
    if(p_rc1->rc.left < p_rc2->rc.left)
    {
      MERGERECT(p_rc1);
    }
    else
    {
      MERGERECT(p_rc2);
    }
  }

  if(p_rc1 != p_rc1_tail)
  {
    do
    {
      MERGERECT(p_rc1);
    } while(p_rc1 != p_rc1_tail);
  }
  else
  {
    while(p_rc2 != p_rc2_tail)
    {
      MERGERECT(p_rc2);
    }
  }
}


/*!
  do_overlap_subtract

  Overlapping band subtraction. x1 is the left-most point not yet checked.

  Results:
    None.

  Side Effects:
    region may have rectangles added to it.
  */
static void do_overlap_subtract(cliprgn_t *p_rgn,
  const cliprc_t *p_rc1, const cliprc_t *p_rc1_tail,
  const cliprc_t *p_rc2, const cliprc_t *p_rc2_tail,
  int top, int bottom)
{
  cliprc_t *p_nrc = NULL;
  int left = 0;

  left = p_rc1->rc.left;
  while((p_rc1 != p_rc1_tail) && (p_rc2 != p_rc2_tail))
  {
    if(p_rc2->rc.right <= left)
    {
      /*!
        Subtrahend missed the boat: go to next subtrahend.
        */
      p_rc2 = p_rc2->p_next;
    }
    else if(p_rc2->rc.left <= left)
    {
      /*!
        Subtrahend preceeds minuend: nuke left edge of minuend.
        */
      left = p_rc2->rc.right;
      if(left >= p_rc1->rc.right)
      {
        /*!
          Minuend completely covered: advance to next minuend and
          reset left fence to edge of new minuend.
          */
        p_rc1 = p_rc1->p_next;
        if(p_rc1 != p_rc1_tail)
        {
          left = p_rc1->rc.left;
        }
      }
      else
      {
        /*!
          Subtrahend now used up since it doesn't extend beyond minuend
          */
        p_rc2 = p_rc2->p_next;
      }
    }
    else if(p_rc2->rc.left < p_rc1->rc.right)
    {
      /*!
        Left part of subtrahend covers part of minuend: add uncovered
        part of minuend to region and skip to next subtrahend.
        */
      alloc_cliprect(p_rgn, &p_nrc);
      p_nrc->rc.left = left;
      p_nrc->rc.top = top;
      p_nrc->rc.right = p_rc2->rc.left;
      p_nrc->rc.bottom = bottom;
      left = p_rc2->rc.right;
      if(left >= p_rc1->rc.right)
      {
        /*!
          Minuend used up: advance to new...
          */
        p_rc1 = p_rc1->p_next;
        if(p_rc1 != p_rc1_tail)
        {
          left = p_rc1->rc.left;
        }
      }
      else
      {
        /*!
          Subtrahend used up
          */
        p_rc2 = p_rc2->p_next;
      }
    }
    else
    {
      /*!
        Minuend used up: add any remaining piece before advancing.
        */
      if(p_rc1->rc.right > left)
      {
        alloc_cliprect(p_rgn, &p_nrc);
        p_nrc->rc.left = left;
        p_nrc->rc.top = top;
        p_nrc->rc.right = p_rc1->rc.right;
        p_nrc->rc.bottom = bottom;
      }
      p_rc1 = p_rc1->p_next;
      if(p_rc1 !=p_rc1_tail)
      {
        left = p_rc1->rc.left;
      }
    }
  }

  /*!
    Add remaining minuend rectangles to region.
    */
  while(p_rc1 != p_rc1_tail)
  {
    alloc_cliprect(p_rgn, &p_nrc);
    p_nrc->rc.left = left;
    p_nrc->rc.top = top;
    p_nrc->rc.right =p_rc1->rc.right;
    p_nrc->rc.bottom = bottom;
    p_rc1 =p_rc1->p_next;
    if(p_rc1 != p_rc1_tail)
    {
      left = p_rc1->rc.left;
    }
  }
}


/*!
  Region Union
  */
  
/*!
  do_non_overlap_union

  Handle a non-overlapping band for the union operation. Just
  Adds the rectangles into the region. Doesn't have to check for
  subsumption or anything.

  Results:
  None.
 
  Side Effects:
  region->numRects is incremented and the final rectangles overwritten
  with the rectangles we're passed.
  */
static void do_non_overlap_union(cliprgn_t *p_rgn,
  const cliprc_t *p_rc, const cliprc_t *p_rc_tail, int top, int bottom)
{
  cliprc_t *p_nrc = NULL;

  while(p_rc != p_rc_tail)
  {
    alloc_cliprect(p_rgn, &p_nrc);
    p_nrc->rc.left = p_rc->rc.left;
    p_nrc->rc.top = top;
    p_nrc->rc.right = p_rc->rc.right;
    p_nrc->rc.bottom = bottom;

    p_rc = p_rc->p_next;
  }
}


/*!
  do_non_overlap_subtract

  Deal with non-overlapping band for subtraction. Any parts from
  region 2 we discard. Anything from region 1 we add to the region.

  Results:
  */
static void do_non_overlap_subtract(cliprgn_t *p_rgn,
  const cliprc_t *p_rc, const cliprc_t *p_rc_tail, int top, int bottom)
{
  cliprc_t *p_nrc = NULL;

  while(p_rc != p_rc_tail)
  {
    alloc_cliprect(p_rgn, &p_nrc);
    p_nrc->rc.left = p_rc->rc.left;
    p_nrc->rc.top = top;
    p_nrc->rc.right = p_rc->rc.right;
    p_nrc->rc.bottom = bottom;

    p_rc = p_rc->p_next;
  }
}


/*!
  end of static functions
  */

/*!
  region operations
  */
  
/*!
  initializes a clipping region.
  */
void gdi_init_cliprgn(cliprgn_t *p_rgn, lib_memf_t *p_heap)
{
  empty_rect(&p_rgn->bound);
  p_rgn->p_head = p_rgn->p_tail = NULL;
  p_rgn->p_heap = p_heap;
}


/*!
  empties a clipping region.
  */
void gdi_empty_cliprgn(cliprgn_t *p_rgn)
{
  cliprc_t *p_rc = NULL, *p_tmp = NULL;

  p_rc = p_rgn->p_head;
  while(NULL != p_rc)
  {
    p_tmp = p_rc->p_next;
    gdi_free_cliprect(p_rgn->p_heap, p_rc);
    p_rc = p_tmp;
  }

  empty_rect(&p_rgn->bound);
  p_rgn->p_head = p_rgn->p_tail = NULL;
}


/*!
  copies one region to another.
  */
BOOL gdi_copy_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn)
{
  cliprc_t *p_orc = p_src_rgn->p_head;
  cliprc_t *p_nrc = NULL, *p_prev_rc = NULL;

  // return false if the destination region is not an empty one.
  if(p_dst_rgn == p_src_rgn)
  {    
    return FALSE;
  }

  gdi_empty_cliprgn(p_dst_rgn);
  if(NULL == p_orc)
  {
    return TRUE;
  }

  p_nrc = gdi_alloc_cliprect(p_dst_rgn->p_heap);
  if(p_nrc == NULL)
  {    
    return FALSE;
  }
  p_dst_rgn->p_head = p_nrc;
  p_nrc->rc = p_orc->rc;

  p_prev_rc = NULL;
  while(NULL != p_orc->p_next)
  {    
    p_nrc->p_next = gdi_alloc_cliprect(p_dst_rgn->p_heap);
    MT_ASSERT(p_nrc->p_next != NULL);
    p_nrc->p_prev = p_prev_rc;

    p_prev_rc =p_nrc;
    p_orc = p_orc->p_next;
    p_nrc = p_nrc->p_next;

    p_nrc->rc = p_orc->rc;
  }

  p_nrc->p_prev = p_prev_rc;
  p_nrc->p_next = NULL;
  p_dst_rgn->p_tail = p_nrc;

  p_dst_rgn->bound = p_src_rgn->bound;

  return TRUE;
}


/*!
  intersects two region.
  */
BOOL gdi_intersect_cliprgn(cliprgn_t *p_dst_rgn,
                           const cliprgn_t *p_src_rgn1,
                           const cliprgn_t *p_src_rgn2)
{
  /*!
    check
    */
  if((!(p_src_rgn1->p_head)) || (!(p_src_rgn2->p_head))
      || (!is_rect_intersected(&p_src_rgn1->bound, &p_src_rgn2->bound)))
  {
    gdi_empty_cliprgn(p_dst_rgn);
    return FALSE;
  }
  else
  {
    cliprgn_set_op(p_dst_rgn, p_src_rgn1, p_src_rgn2,
                   do_overlap_intersect, NULL, NULL);
  }

  /*!
    Can't alter dst_rgn's bound before we call miRegionOp because
    it might be one of the source regions and miRegionOp depends
    on the bound of those regions being the same. Besides, this
    way there's no checking against rectangles that will be nuked
    due to coalescing, so we have to examine fewer rectangles.
    */
  calc_bound(p_dst_rgn);

  return TRUE;
}


/*!
  unions two regions.
  */
BOOL gdi_union_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn1,
                       const cliprgn_t *p_src_rgn2)
{
  //  checks all the simple cases

  // region 1 and 2 are the same or region 1 is empty
  if((p_src_rgn1 == p_src_rgn2) || (!(p_src_rgn1->p_head)))
  {
    if(p_dst_rgn != p_src_rgn2)
    {
      gdi_copy_cliprgn(p_dst_rgn, p_src_rgn2);
    }
    return TRUE;
  }

  // if nothing to union (region 2 empty)
  if(NULL == (p_src_rgn2->p_head))
  {
    if(p_dst_rgn != p_src_rgn1)
    {
      gdi_copy_cliprgn(p_dst_rgn, p_src_rgn1);
    }
    return TRUE;
  }

  // region 1 completely subsumes region 2
  if((p_src_rgn1->p_head == p_src_rgn1->p_tail)
      && (p_src_rgn1->bound.left <= p_src_rgn2->bound.left)
      && (p_src_rgn1->bound.top <= p_src_rgn2->bound.top)
      && (p_src_rgn1->bound.right >= p_src_rgn2->bound.right)
      && (p_src_rgn1->bound.bottom >= p_src_rgn2->bound.bottom))
  {
    if(p_dst_rgn != p_src_rgn1)
    {
      gdi_copy_cliprgn(p_dst_rgn, p_src_rgn1);
    }
    return TRUE;
  }

  // region 2 completely subsumes region 1
  if((p_src_rgn2->p_head == p_src_rgn2->p_tail)
      && (p_src_rgn2->bound.left <= p_src_rgn1->bound.left)
      && (p_src_rgn2->bound.top <= p_src_rgn1->bound.top)
      && (p_src_rgn2->bound.right >= p_src_rgn1->bound.right)
      && (p_src_rgn2->bound.bottom >= p_src_rgn1->bound.bottom))
  {
    if(p_dst_rgn != p_src_rgn2)
    {
      gdi_copy_cliprgn(p_dst_rgn, p_src_rgn2);
    }
    return TRUE;
  }
  // normal
  cliprgn_set_op(p_dst_rgn, p_src_rgn1, p_src_rgn2, do_overlap_union,
                 do_non_overlap_union, do_non_overlap_union);

  calc_bound(p_dst_rgn);

  return TRUE;
}


/*!
  substrcts a region from another.
  */
BOOL gdi_subtract_cliprgn(cliprgn_t *p_difference_rgn,
                          const cliprgn_t *p_minuend_rgn,
                          const cliprgn_t *p_subtrahend_rgn)
{
  //  checks all the simple cases
  if((!(p_minuend_rgn->p_head)) || (!(p_subtrahend_rgn->p_head)) 
    || (!is_rect_intersected(
    &p_minuend_rgn->bound, &p_subtrahend_rgn->bound)))
  {
    gdi_copy_cliprgn(p_difference_rgn, p_minuend_rgn);
    return TRUE;
  }

  cliprgn_set_op(p_difference_rgn, p_minuend_rgn, p_subtrahend_rgn,
                 do_overlap_subtract,
                 do_non_overlap_subtract,
                 NULL);

  /*!
    Can't alter newReg's bound before we call miRegionOp because
    it might be one of the source regions and miRegionOp depends
    on the bound of those regions being the unaltered. Besides, this
    way there's no checking against rectangles that will be nuked
    due to coalescing, so we have to examine fewer rectangles.
    */
  calc_bound(p_difference_rgn);

  return TRUE;
}


/*!
  does the XOR operation between two regions.
  */
BOOL gdi_xor_cliprgn(cliprgn_t *p_dst_rgn, const cliprgn_t *p_src_rgn1,
                     const cliprgn_t *p_src_rgn2)
{
  cliprgn_t tmp_a, tmp_b;

  gdi_init_cliprgn(&tmp_a, p_src_rgn1->p_heap);
  gdi_init_cliprgn(&tmp_b, p_src_rgn2->p_heap);

  gdi_subtract_cliprgn(&tmp_a, p_src_rgn1, p_src_rgn2);
  gdi_subtract_cliprgn(&tmp_b, p_src_rgn2, p_src_rgn1);
  gdi_union_cliprgn(p_dst_rgn, &tmp_a, &tmp_b);

  gdi_empty_cliprgn(&tmp_a);
  gdi_empty_cliprgn(&tmp_b);

  return TRUE;
}


/*!
  gets the bounding rectangle of a region.
  */
void gdi_get_cliprgn_boundrect(cliprgn_t *p_rgn, rect_t *p_rc)
{
  *p_rc = p_rgn->bound;
}


/*!
  determines whether a region is an empty region.
  */
BOOL gdi_is_empty_cliprgn(const cliprgn_t *p_rgn)
{
  if(p_rgn->p_head == NULL)
  {
    return TRUE;
  }

  return FALSE;
}


/*!
  offsets the region.
  */
void gdi_offset_cliprgn(cliprgn_t *p_rgn, s16 x, s16 y)
{
  cliprc_t *p_crc = p_rgn->p_head;

  while(p_crc != NULL)
  {
    offset_rect(&p_crc->rc, x, y);
    p_crc = p_crc->p_next;
  }

  if(NULL != (p_rgn->p_head))
  {
    offset_rect(&p_rgn->bound, x, y);
  }
}


/*!
  sets a region to contain only one rect.
  */
BOOL gdi_set_cliprgn(cliprgn_t *p_rgn, const rect_t *p_rc)
{
  cliprc_t *p_nrc = NULL;

  if(is_empty_rect(p_rc))
  {
    return FALSE;
  }

  // empty rgn, at first
  gdi_empty_cliprgn(p_rgn);

  // get a new cliprect
  p_nrc = gdi_alloc_cliprect(p_rgn->p_heap);
  if(p_nrc == NULL)
  {
    return FALSE;
  }

  // set cliprect
  p_nrc->rc = *p_rc;
  p_nrc->p_next = NULL;
  p_nrc->p_prev = NULL;

  p_rgn->p_head = p_rgn->p_tail = p_nrc;
  p_rgn->bound = *p_rc;

  return TRUE;
}


/*!
  unions one rectangle to a region.
  */
BOOL gdi_add_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc)
{
  cliprgn_t nrgn;
  cliprc_t crc;

  if(is_empty_rect(p_rc))
  {
    return FALSE;
  }

  crc.rc = *p_rc;
  crc.p_next = NULL;
  crc.p_prev = NULL;

  nrgn.bound = *p_rc;
  nrgn.p_head = &crc;
  nrgn.p_tail = &crc;
  nrgn.p_heap = NULL;

  return gdi_union_cliprgn(p_rgn, p_rgn, &nrgn);
}


/*!
  intersects a rectangle with a region.
  */
BOOL gdi_intersect_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc)
{
  cliprgn_t nrgn;
  cliprc_t crc;

  if(is_empty_rect(p_rc))
  {
    gdi_empty_cliprgn(p_rgn);
    return TRUE;
  }

  crc.rc = *p_rc;
  crc.p_next = NULL;
  crc.p_prev = NULL;

  nrgn.bound = *p_rc;
  nrgn.p_head = &crc;
  nrgn.p_tail = &crc;
  nrgn.p_heap = NULL;

  return gdi_intersect_cliprgn(p_rgn, p_rgn, &nrgn);
}


/*!
  subtracts a rectangle from a region.
  */
BOOL gdi_subtract_cliprect(cliprgn_t *p_rgn, const rect_t *p_rc)
{
  cliprgn_t nrgn;
  cliprc_t crc;

  if(is_empty_rect(p_rc))
  {
    return FALSE;
  }

  crc.rc = *p_rc;
  crc.p_next = NULL;
  crc.p_prev = NULL;

  nrgn.bound = *p_rc;
  nrgn.p_head = &crc;
  nrgn.p_tail = &crc;
  nrgn.p_heap = NULL;

  return gdi_subtract_cliprgn(p_rgn, p_rgn, &nrgn);
}


/*!
  determines whether a point is in a region.
  */
BOOL gdi_is_pt_in_cliprgn(const cliprgn_t *p_rgn, s16 x, s16 y)
{
  int top = 0;
  cliprc_t *p_crc = p_rgn->p_head;

  /*!
    check with bounding rect of clipping region 
    */
  if(y >= p_rgn->p_tail->rc.bottom || y < p_rgn->p_head->rc.top)
  {
    return FALSE;
  }

  /*!
    find the ban in which this point lies 
    */
  p_crc = p_rgn->p_head;
  while(p_crc && y >= p_crc->rc.bottom)
  {
    p_crc = p_crc->p_next;
  }

  if(NULL == p_crc)
  {
    return FALSE;
  }

  /*!
    check in this ban
    */
  top = p_crc->rc.top;
  while(p_crc && p_crc->rc.top == top)
  {
    if(is_pt_in_rect(&p_crc->rc, x, y))
    {
      return TRUE;
    }

    p_crc = p_crc->p_next;
  }

  return FALSE;
}


/*!
  determines whether a rectangle is intersected with a region.
  */
BOOL gdi_is_rect_in_cliprgn(const cliprgn_t *p_rgn, const rect_t *p_rc)
{
  cliprgn_t crgn;
  BOOL ret = FALSE;
  lib_memf_t *p_heap = NULL;
  
  p_heap = gdi_get_cliprc_heap();
  gdi_init_cliprgn(&crgn, p_heap);
  gdi_copy_cliprgn(&crgn, p_rgn);
  gdi_add_cliprect(&crgn, p_rc);  

  gdi_xor_cliprgn(&crgn, p_rgn, &crgn);

  if(gdi_is_empty_cliprgn(&crgn))
  {
    ret = TRUE;
  }
  
  return ret;
}


/*!
  end of region operations
  */
