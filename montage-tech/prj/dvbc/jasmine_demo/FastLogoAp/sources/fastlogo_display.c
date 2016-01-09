/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include <assert.h>
#include "sys_types.h"
#include "sys_define.h"
#include <stdio.h>
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_printk.h"
#include "hal_base.h"
#include "lib_rect.h"
#include "common.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "drv_dev.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "class_factory.h"
#include "fastlogo_display.h"

static void fl_reg_disp_set_graphic_ctrl_still_sel_hd(u8 data)
{
  reg_disp_graphic_ctrl_t d;
  d.all = *(volatile u32 *)REG_DISP_GRAPHIC_CTRL;
  d.bitc.still_sel_hd = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_GRAPHIC_CTRL, d.all);
}

static void fl_reg_disp_set_still_x_hd_hd_still_startx(u16 data)
{
  reg_disp_still_x_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_X_HD;
  d.bitc.hd_still_startx = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_X_HD, d.all);
}

static void fl_reg_disp_set_still_x_hd_hd_still_endx(u16 data)
{
  reg_disp_still_x_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_X_HD;
  d.bitc.hd_still_endx = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_X_HD, d.all);
}

static void fl_reg_disp_set_still_y_hd_hd_still_starty(u16 data)
{
  reg_disp_still_y_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_Y_HD;
  d.bitc.hd_still_starty = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_Y_HD, d.all);
}

static void fl_reg_disp_set_still_y_hd_hd_still_endy(u16 data)
{
reg_disp_still_y_hd_t d;
d.all = *(volatile u32 *)REG_DISP_STILL_Y_HD;
d.bitc.hd_still_endy = data;
hal_put_u32((volatile unsigned long *)REG_DISP_STILL_Y_HD, d.all);
}

static void fl_reg_disp_set_graphic_ctrl_still_hd_format_444(u8 data)
{
  reg_disp_graphic_ctrl_t d;
  d.all = *(volatile u32 *)REG_DISP_GRAPHIC_CTRL;
  d.bitc.still_hd_format_444 = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_GRAPHIC_CTRL, d.all);
}

static void fl_reg_disp_set_still_stride_hd_cb_byte_sel(u8 data)
{
  reg_disp_still_stride_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_STRIDE_HD;
  d.bitc.cb_byte_sel = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_STRIDE_HD, d.all);
}

static void fl_reg_disp_set_still_stride_hd_cr_byte_sel(u8 data)
{
  reg_disp_still_stride_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_STRIDE_HD;
  d.bitc.cr_byte_sel = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_STRIDE_HD, d.all);
}

static void fl_reg_disp_set_still_stride_hd_y_byte_sel(u8 data)
{
  reg_disp_still_stride_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_STRIDE_HD;
  d.bitc.y_byte_sel = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_STRIDE_HD, d.all);
}

static void fl_reg_disp_set_still_stride_hd_hd_still_stride(u16 data)
{
  reg_disp_still_stride_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_STRIDE_HD;
  d.bitc.hd_still_stride = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_STRIDE_HD, d.all);
}

static void fl_reg_disp_set_still_start_addr_hd_hd_still_start_addr(u32 data)
{
  reg_disp_still_start_addr_hd_t d;
  d.all = *(volatile u32 *)REG_DISP_STILL_START_ADDR_HD;
  d.bitc.hd_still_start_addr = data;
  hal_put_u32((volatile unsigned long *)REG_DISP_STILL_START_ADDR_HD, d.all);
}

/*!
   Set still picture target window position.

   \param[in] layer layer id.
   \param[in] startx  x.
   \param[in] endx   end x.
   \param[in] starty  y.
   \param[in] endy   end y.
  */
static void fl_disp_set_still_pos(disp_layer_id_t layer,
                             u32 startx,
                             u32 endx,
                             u32 starty,
                             u32 endy)
{
  if(DISP_LAYER_ID_STILL_SD == layer)
  {
    OS_PRINTF("not support layer!\n");
  }
  else if(DISP_LAYER_ID_STILL_HD == layer)
  {
    fl_reg_disp_set_still_x_hd_hd_still_startx(startx);
    fl_reg_disp_set_still_x_hd_hd_still_endx(endx);
    fl_reg_disp_set_still_y_hd_hd_still_starty(starty);
    fl_reg_disp_set_still_y_hd_hd_still_endy(endy);
  }
  else
  {
    MT_ASSERT(0);
  }
}

/*!
   Set still picture display mode.

   \param[in] layer layer id.
   \param[in] stride memory storage stride in pixel;
   \param[in] fmt pixel format;
  */
static void fl_disp_set_still_attribute(disp_layer_id_t layer, u16 stride, pix_fmt_t fmt)
{
  if(DISP_LAYER_ID_STILL_SD == layer)
  {
    OS_PRINTF("not support layer!\n");
  }
  else if(DISP_LAYER_ID_STILL_HD == layer)
  {
    if(PIX_FMT_CBY0CRY18888 == fmt)
    {
      fl_reg_disp_set_graphic_ctrl_still_hd_format_444(FALSE);
      fl_reg_disp_set_still_stride_hd_cb_byte_sel(3);
      fl_reg_disp_set_still_stride_hd_cr_byte_sel(1);
      fl_reg_disp_set_still_stride_hd_y_byte_sel(2);
    }
    else if(PIX_FMT_AYCBCR8888 == fmt)
    {
      fl_reg_disp_set_graphic_ctrl_still_hd_format_444(TRUE);
      fl_reg_disp_set_still_stride_hd_cb_byte_sel(1);
      fl_reg_disp_set_still_stride_hd_cr_byte_sel(0);
      fl_reg_disp_set_still_stride_hd_y_byte_sel(2);
    }
    else
    {
      MT_ASSERT(0);
    }
    fl_reg_disp_set_still_stride_hd_hd_still_stride(stride);
  }
  else
  {
    MT_ASSERT(0);
  }

}

/*!
   Set region header to register.

   \param[in] p_dp priv handle.
   \param[in] layer layer id.
   \param[in] addr  start addr.
   \param[in] size one luma field size in still region in kbytes.
  */
static void fl_disp_set_layer_start_addr(disp_layer_id_t layer,
                                   u32 addr)
{
  u32 addr_align = (addr >> ALIGN_SHIFT);//in warriors in unit 64bit

  switch(layer)
  {
    case DISP_LAYER_ID_BACKGROUND:
    case DISP_LAYER_ID_VIDEO_SD:
    case DISP_LAYER_ID_VIDEO_HD:
    case DISP_LAYER_ID_DUMMY_OSD:
    case DISP_LAYER_ID_SUBTITL:
    case DISP_LAYER_ID_OSD0:
    case DISP_LAYER_ID_OSD1:
      OS_PRINTF("DISP:No need to set start addr in layer %d!\n", layer);
      return;
    case DISP_LAYER_ID_STILL_HD:
      fl_reg_disp_set_still_start_addr_hd_hd_still_start_addr(addr_align);
      break;
    default:
      MT_ASSERT(0);
  }
}

/*!
   clear region header to register.

   \param[in] p_dp priv handle.
   \param[in] layer layer id.
  */
static void fl_disp_clear_layer_start_addr(disp_layer_id_t layer)
{
  //restore to the preset value incording to the spec
  switch(layer)
  {
    case DISP_LAYER_ID_BACKGROUND:
    case DISP_LAYER_ID_VIDEO_SD:
    case DISP_LAYER_ID_VIDEO_HD:
    case DISP_LAYER_ID_DUMMY_OSD:
    case DISP_LAYER_ID_STILL_SD:
    case DISP_LAYER_ID_SUBTITL:
    case DISP_LAYER_ID_OSD0:
    case DISP_LAYER_ID_OSD1:
      OS_PRINTF("DISP:No need to clear start addr in layer %d!\n", layer);
      return;
    case DISP_LAYER_ID_STILL_HD:
      fl_reg_disp_set_still_start_addr_hd_hd_still_start_addr(0x20000);
      break;
    default:
      MT_ASSERT(0);
  }

}

/*!
   Set layer onoff to register.

   \param[in] p_dp priv handle.
   \param[in] layer layer id.
   \param[in] b_on enable flag.
  */
void fl_disp_set_layer_onoff(disp_layer_id_t layer, BOOL b_on)
{
  switch(layer)
  {
    case DISP_LAYER_ID_BACKGROUND:
    case DISP_LAYER_ID_STILL_SD:
    case DISP_LAYER_ID_VIDEO_SD:
    case DISP_LAYER_ID_VIDEO_HD:
    case DISP_LAYER_ID_SUBTITL:
    case DISP_LAYER_ID_OSD0:
    case DISP_LAYER_ID_OSD1:
    case DISP_LAYER_ID_DUMMY_OSD:
      OS_PRINTF("layer onoff not support\n");
      break;
    case DISP_LAYER_ID_STILL_HD:
      fl_reg_disp_set_graphic_ctrl_still_sel_hd(b_on);
      break;
    default:
      MT_ASSERT(0);
  }
}

/*!
   Update the regions in certain layer.

   \param[in] p_dp priv handle.
   \param[in] layer layer id.
   \param[in] b_update create all the regions if TRUE.
  */
void fl_disp_st_layer_update(u32 rgn_addr,rect_vsb_t *p_rect,
                             disp_layer_id_t layer,pix_fmt_t pix_fmt,
                             BOOL b_update)
{
  if(TRUE == b_update)
  {
    fl_disp_clear_layer_start_addr(layer);
    fl_disp_set_layer_onoff(layer, FALSE);
  }

  //match with hardware, width = right -left + 1, height = bot - top + 1
  //startx = left + 1; starty = top + 1, mini value is 1
  //endx = startx + width - 1; endy = starty + width - 1;
  fl_disp_set_still_pos(layer,
                        p_rect->x + 1,
                        p_rect->x + 1 + p_rect->w - 1,
                        p_rect->y + 1,
                        p_rect->y + 1 + p_rect->h - 1);
  fl_disp_set_still_attribute(layer, (u16)(p_rect->w), pix_fmt);
  fl_disp_set_layer_start_addr(layer, rgn_addr);
}

avc_video_mode_1_t fl_disp_get_sd_mode(u8 focus)
{
  avc_video_mode_1_t sd_mode = AVC_VIDEO_MODE_AUTO_1;

  switch(focus)
  {
    case 0:
      sd_mode = AVC_VIDEO_MODE_PAL_1;
      break;
    case 1:
      sd_mode = AVC_VIDEO_MODE_NTSC_1;
      break;
    case 2:
      sd_mode = AVC_VIDEO_MODE_AUTO_1;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return sd_mode;
}

avc_video_mode_1_t fl_disp_get_hd_mode(u8 focus,u32 *p_w,u32 *p_h)
{
  avc_video_mode_1_t hd_mode = AVC_VIDEO_MODE_AUTO_1;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  disp_sys_t video_std = VID_SYS_AUTO;

  avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);
  video_std = avc_get_video_mode_1(avc_handle);
  switch (video_std)
  {
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_480I;
          *p_w = 720;
          *p_h = 480;
          break;
        case 1:
          hd_mode = AVC_VIDEO_MODE_480P;
          *p_w = 720;
          *p_h = 480;
          break;
        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_60HZ;
          *p_w = 1280;
          *p_h = 720;
          break;
        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_60HZ;
          *p_w = 1920;
          *p_h = 1080;
          break;
        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_60HZ;
          *p_w = 1920;
          *p_h = 1080;
          break;
        default:
          MT_ASSERT(0);
          break;
      }
      break;
    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_576I;
          *p_w = 720;
          *p_h = 576;
          break;
        case 1:
          hd_mode = AVC_VIDEO_MODE_576P;
          *p_w = 720;
          *p_h = 576;
          break;
        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_50HZ;
          *p_w = 1280;
          *p_h = 720;
          break;
        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_50HZ;
          *p_w = 1920;
          *p_h = 1080;
          break;
        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_50HZ;
          *p_w = 1920;
          *p_h = 1080;
          break;
        default:
          MT_ASSERT(0);
          break;
      }
      break;
    default:
      break;
  }
  return hd_mode;
}

avc_video_aspect_1_t fl_disp_get_video_aspect(u8 focus)
{
  switch(focus)
  {
    case 0:
      return AVC_VIDEO_ASPECT_AUTO_1;
    case 1:
      return AVC_VIDEO_ASPECT_43_LETTERBOX_1;
    case 2:
      return AVC_VIDEO_ASPECT_43_PANSCAN_1;
    case 3:
      return AVC_VIDEO_ASPECT_169_1;
    default:
      return AVC_VIDEO_ASPECT_AUTO_1;
  }
}
