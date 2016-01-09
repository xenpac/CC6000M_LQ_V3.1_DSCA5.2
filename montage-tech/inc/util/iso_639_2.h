/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ISO_639_2_H__
#define __ISO_639_2_H__

/*!
  iso 639_2 descriptor length
  */
#define ISO_639_2_DESC_LENGTH (3)

/*!
   Gets the index by its language desc.

   \param[in] p_desc Points to the language description.
   \return .

  */
u16 iso_639_2_desc_to_idx(const char *p_desc);

/*!
   Gets the language by its index.

   \param[in] idx Specifies the index of language.
   \return Points to the language description (Not a string).
  */
const char * iso_639_2_idx_to_desc(u16 idx);

/*!
  Gets language type by its idx.

   \param[in] idx Specifies the index of language.
   \return language type.
  */
u16 iso_639_2_idx_to_type(u16 idx);

#endif

