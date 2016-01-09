/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __GLB_INFO_H__
#define __GLB_INFO_H__

/*!
  This structure defines the interrupt source in global info.
  */
typedef enum
{
  /*!
    video decoder starts decoding one frame
    */
  GLB_INT_VDEC_ONE_FRAME = 1,
  /*!
    video encoder output field number
    */
  GLB_INT_VENC_OUTPUT_FIELD,
  /*!
    video encoder output top field
    */
  GLB_INT_VENC_TOP_FIELD,
  /*!
    video encoder output disp field
    */
  GLB_INT_VENC_DISP_FIELD
}glb_info_int_t;


/*!
  Defines the type of the interrupt service routine for glb_info
  */
typedef void (*glb_info_callback_t)(void);


/*!
  Install the ISR of glb_info. All interrupt sources in glb_info are disabled.
  */
void glb_info_init(void);

/*!
  Install the magic ISR of glb_info. All interrupt sources in glb_info are disabled.
  */
void magic_glb_info_init(void);


/*!
  Install the ISR of an interrupt source in glb_info

  \param[in] int_id The interrupt source in glb_info.
  \param[in] func The ISR for the interrupt.

  \return Return SUCCESS for ISR register done.
        Return ERR_NO_RSRC for unsupported interrupt source.
  */
RET_CODE glb_info_isr_register(glb_info_int_t int_id, glb_info_callback_t func);

/*!
  Uninstall the ISR of an interrupt source in glb_info

  \param[in] func The ISR for the interrupt.

  \return Return SUCCESS for Uninstalling ISR done.
        Return ERR_NO_RSRC for unsupported interrupt source.
  */
RET_CODE glb_info_isr_unregister(glb_info_int_t int_id);

/*!
  Enable/disable an interrupt source in glb_info.

  \param[in] int_id The interrupt source in glb_info.
  \param[in] is_en TRUE to enable the interrupt and FALSE to disable it.

  \return Return SUCCESS for interrupt state setting done.
        Return ERR_NO_RSRC for unsupported interrupt source.
  */
RET_CODE glb_info_isr_en(glb_info_int_t int_id, BOOL is_en);

/*!
  glb_info state query.

  \param[in] int_id The interrupt source in glb_info.

  \return Return TRUE/FALSE.
  */
BOOL glb_info_isr_query_state(glb_info_int_t int_id);
#endif  // __GLB_INFO_H__
