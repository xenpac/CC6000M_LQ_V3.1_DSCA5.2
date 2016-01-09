/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __TKGS_H_
#define __TKGS_H_

/*!
  per tkgs table section size
  */
#define TKGS_SECTION_SIZE        (4 * 1024)

/*!
  tkgs standard data size
  */
#define TKGS_STANDARD_DATA_SIZE  (4084)

/*!
  lase section number is a 8-bit field.
  */
#define MAX_TKGS_SEC_NUM         (256)


/*!
  tkgs table struction
  */
typedef struct tag_tkgs_table
{
/*!
  tkgs data length.
  */  
  u16 data_length;
/*!
  version number
  */  
  u8 ver_num;
/*!
  section number
  */  
  u8 sec_num;
/*!
  last section number.
  */
  u8 last_sec_num;
}tkgs_table_t;


/*!
  Parse tkgs section

  \param[in] p_sec: Section data filtered by PTI driver
  */
void parse_tkgs(handle_t handle, dvb_section_t *p_sec);


/*!
  Request a tkgs data
  \param[in] p_sec: section information
  \param[in] table_id: tkgs table id
  \param[in] para2: section number
  */
void request_tkgs_single_mode(dvb_section_t *p_sec, u32 table_id, u32 para2);

/*!
  Request a tkgs data
  \param[in] p_sec: section information
  \param[in] table_id: tkgs table id
  \param[in] para2: section number
  */
void request_tkgs_multi_mode(dvb_section_t *p_sec, u32 table_id, u32 para2);


#endif // End for __TKGS_H_

