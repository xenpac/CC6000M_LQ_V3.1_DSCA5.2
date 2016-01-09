/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_RECT_H__
#define __LIB_RECT_H__

/*!
   A rectangle defined by coordinates of corners.
  */
typedef struct
{
  /*!
     The x coordinate of the upper-left corner of the rectangle.
    */
  s16 left;
  /*!
     The y coordinate of the upper-left corner of the rectangle.
    */
  s16 top;
  /*!
     The y coordinate of the lower-right corner of the rectangle.
    */
  s16 right;
  /*!
     The y coordinate of the lower-right corner of the rectangle.
    */
  s16 bottom;
}rect_t;

/*!
   A point defined by x, y coordinates.
  */
typedef struct
{
  /*!
     The x coordinate of the point.
    */
  s16 x;
  /*!
     The y coordinate of the point.
    */
  s16 y;
}point_t;

/*!
   Gets the width of a RECT object by using the pointer to it.
  */
#define RECTWP(prc)    ((prc)->right - (prc)->left)
/*!
   Gets the height of a RECT object by using the pointer to it.
  */
#define RECTHP(prc)    ((prc)->bottom - (prc)->top)
/*!
   Gets the width of a RECT object.
  */
#define RECTW(rc)      ((rc).right - (rc).left)
/*!
   Gets the height of a RECT object.
  */
#define RECTH(rc)      ((rc).bottom - (rc).top)

/*!
   Sets the coordinates of the specified rectangle.

   \param[out] p_rc Points to the rectangle that to be set.
   \param[in] left Specifies the x-coordinate of the rectangle's
              upper-left corner.
   \param[in] top Specifies the y-coordinate of the rectangle's
              upper-left corner.
   \param[in] right Specifies the x-coordinate of the rectangle's
              lower-right corner.
   \param[in] bottom Specifies the y-coordinate of the rectangle's
              lower-right corner.
  */
void set_rect(rect_t *p_rc, s16 left, s16 top, s16 right, s16 bottom);


/*!
   Empties a rectangle.

   \param[in] p_rc Points to the rectangle.
  */
void empty_rect(rect_t *p_rc);

/*!
   Copies a rectangle to another.

   \param[in] p_dst_rc Points to the destination rectangle.
   \param[in] p_src_rc Points to the source rectangle.
  */
void copy_rect(rect_t *p_dst_rc, const rect_t *p_src_rc);

/*!
   Determines whether a rectangle is empty.

   \param[in] p_rc Points to the rectangle.

   \return The return value is TRUE if the rectangle is empty.
           Otherwise returns FALSE.
  */
BOOL is_empty_rect(const rect_t *p_rc);

/*!
   Determines whether a rectangle is empty.

   \param[in] p_rc Points to the rectangle.

   \return The return value is TRUE if the rectangle is empty.
           Otherwise returns FALSE.
  */
BOOL is_invalid_rect(const rect_t *p_rc);

/*!
   Determines whether a rectangle is equal to another.

   \param[in] p_rc1 Points to the rectangle.
   \param[in] p_rc2 Points to another rectangle.

   \return The return value is TRUE if the rectangle is equal to another.
           Otherwise returns FALSE.
  */
BOOL is_equal_rect(const rect_t *p_rc1, const rect_t *p_rc2);

/*!
   Normalize a rectangle

   \param[in] p_rc Points to the rectangle.
  */
void normalize_rect(rect_t *p_rc);

/*!
   Determines whether a rectangle is covered on another

   \param[in] p_rc1 Points to the rectangle.
   \param[in] p_rc2 Points to another rectangle.

   \return The return value is TRUE if the rectangle is covered on another.
           Otherwise returns FALSE.
  */
BOOL is_rect_covered(const rect_t *p_rc1, const rect_t *p_rc2);

/*!
   Determines whether a rectangle is intersected with another

   \param[in] p_rc1 Points to the region.
   \param[in] p_rc2 Points to another.

   \return The return value is TRUE if the region is intersected with another.
           Otherwise returns FALSE.
  */
BOOL is_rect_intersected(const rect_t *p_rc1, const rect_t *p_rc2);

/*!
   Intersects two rectangle.

   \param[out] p_dst_rc Points to the rectangle that place
                     the result of intersected.
   \param[in] p_src_rc1 Points to the rectangle.
   \param[in] p_src_rc2 Points to another rectangle.

   \return The return value is TRUE if the rectangle is intersected with
           another.Otherwise returns FALSE.
  */
BOOL intersect_rect(rect_t *p_dst_rc,
                    const rect_t *p_src_rc1,
                    const rect_t *p_src_rc2);
/*!
   Unions two rectangle.

   \param[out] p_dst_rc Points to the rectangle that contained
              p_src_rc1 and p_src_rc1.
   \param[in] p_src_rc1 Points to the rectangle.
   \param[in] p_src_rc2 Points to another rectangle.

   \return The return value is TRUE if the function is successful.
           Otherwise returns FALSE.
  */
BOOL union_rect(rect_t *p_dst_rc,
                const rect_t *p_src_rc1,
                const rect_t *p_src_rc2);
/*!
   Subtracts a rectangle from another.

   \param[out] difference_rcs Points to the array of rectangles that determined
                              by subtracting the rectangle points to by
                              subtrahend_rc from the rectangle points to
                              by minuend_rc.
   \param[in] minuend_rc Points to a rectangle from which the function subtracts
                         the rectangle pointed to by subtrahend_rc.
   \param[in] subtrahend_rc Points to to a rectangle that the function subtracts
                            from the rectangle pointed to by minuend_rc.

   \return The return value is TRUE if the function is successful.
           Otherwise returns FALSE.
  */
u16 subtract_rect(rect_t *p_difference_rcs,
                  const rect_t *p_minuend_rc,
                  const rect_t *p_subtrahend_rc);

/*!
   Gets the bounding rectangle of two rectangles

   \param[out] p_dst_rc Points to the rectangle that receives the enclosing
   bound.
   \param[in] p_src_rc1 Points to the rectangle.
   \param[in] p_src_rc2 Points to another rectangle.
  */
void generate_boundrect(rect_t *p_dst_rc,
                        const rect_t *p_src_rc1,
                        const rect_t *p_src_rc2);

/*!
   Moves rectangle by the specified offsets.

   \param[in] p_rc Points to the rectangle.
   \param[in] x Specifies the x-coordinate.
   \param[in] y Specifies the y-coordinate.
  */
void offset_rect(rect_t *p_rc, s16 x, s16 y);

/*!
   Extends a rectangle

   \param[in] p_rc Points to the rectangle.
   \param[in] cx Specifies the value on x-coordinate.
   \param[in] cy Specifies the value on y-coordinate.
  */
void extend_rect(rect_t *p_rc, s16 cx, s16 cy);

/*!
   Extends a rectangle to a point

   \param[in] p_rc Points to the rectangle.
   \param[in] x Specifies the x-coordinate of the point.
   \param[in] y Specifies the y-coordinate of the point.
  */
void extend_rect_to_pt(rect_t *p_rc, s16 x, s16 y);

/*!
   Determines whether a point is in the rectangle

   \param[in] p_rc Points to the rectangle.
   \param[in] x Specifies the x-coordinate of the point.
   \param[in] y Specifies the y-coordinate of the point.

   \return The return value is TRUE if the point is in the rectangle.
           Otherwise returns FALSE.
  */
BOOL is_pt_in_rect(const rect_t *p_rc, s16 x, s16 y);

#endif
