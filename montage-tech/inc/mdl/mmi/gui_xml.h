/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __GUI_XML_H__
#define __GUI_XML_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
  xml skin initial.
  */
BOOL xml_skin_init(u32 mem_addr, u32 mem_size);

/*!
  xml skin release.
  */
void xml_skin_release(void);

/*!
  xml paint menu.
  */
void xml_paint_menu(control_t *p_root, BOOL is_force);

/*!
  get font style
  */
rsc_fstyle_t *xml_get_fstyle(u32 idx);

/*!
  get rstyle
  */
rsc_rstyle_t *xml_get_rstyle(u32 idx);

/*!
  get bmp
  */
BOOL xml_get_bmp(u16 bmp_id, rsc_bitmap_t *p_hdr_bmp, u8 *p_data, u32 *p_bmp_hdr_addr);

/*!
  dump menu.
  */
void xml_dump_menu(control_t *p_root);

/*!
  xml logo init.
  */
BOOL xml_logo_init(void);

/*!
  xml logo release.
  */
void xml_logo_release(void);

/*!
  get logo addr on flash.
  */
BOOL xml_get_logo(u16 logo_id, u32 *p_addr, u32 *p_size);

#ifdef __cplusplus
}
#endif

#endif


