/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DRV_MISC_H__
#define __DRV_MISC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
    Video field information
  */
typedef enum
{
    FIELD_PARRITY_EVEN = 0,
    FIELD_PARRITY_ODD = 1
} field_parity_t;

/*!
  Standard of video format
  */
typedef enum
{
    VID_STD_PAL,
    VID_STD_NTSC
} video_std_t;

/*!
  SUBTITLE   sys  cfg
  */
typedef enum
{
    SYS_1280_720,
    SYS_720_576,
    SYS_1920_1080
} subt_sys_cfg_t;


/*!
   SDK or total solution
*/
typedef enum
{
    SUBT_REGION_IN_DECODER,
    SUBT_REGION_OUT_DECODER,
}subt_region_pos_t;

/*!
  Get video standard
  */
video_std_t vbi_get_video_std(void);

/*!
  Get the parity (even or odd) of the field to be displayed ?????
  */
field_parity_t field_parity_going_to_display(void);

/*!
  Get offset

  \param[in] pts
  \param[in] stc

  \return Return offset
  */
s32 vbi_get_pts_offset(u32 pts, u32 stc);

/*!
  Get STC
  */
u32 vbi_get_stc(void);

/*!
   c++ constructors init
  */
void mt_hal_invoke_constructors(void);

#ifdef __cplusplus
}
#endif


#endif

