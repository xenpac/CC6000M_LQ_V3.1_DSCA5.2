/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ABS_FDT_H_
#define __ABS_FDT_H_

/*!
  \file fdt.h
  This file defines the structure of abs FDT information(detail refer to the ABS spec).
  Notice:
    Our software support two methods to get the FDT data:ts packet mode and multi-section mode.
    The engine only transport the raw data to application(ap_is).    

  */

/*!
  We can get some detail files from the FDT raw data.
  File type descriptor
  */
typedef enum
{
  /*!
   Unknow file type: this is error case!
   */
  IS_FILE_UNKNOW = 0,
  /*!
   BMP File: !
   */
  IS_FILE_BMP,
  /*!
   MPG File: !
   */
  IS_FILE_MPG,
  /*!
   XML File: !
   */
  IS_FILE_XML
} abs_is_file_type_t;

/*!
  XML File type descriptor
  */
typedef enum
{
  /*!
     reserved;error case
   */
  XML_UNKNOW = 0,
  /*!
     The xml file which descripe the detail info of ROOT page !
   */
  ROOT_INDEX,
  /*!
     The xml file which descripe the detail info of the second-level page !
   And this page include news items.
   */
  XWSS_2PAGE,
  /*!
     The xml file which descripe the detail info of the second-level page !
   And this page include Agriculture Information items.
   */
  NYZX_2PAGE,
  /*!
     The xml file which descripe the detail info of the second-level page !
   And this page include Science park items.
   */
  KJYD_2PAGE,
  /*!
     The xml file which descripe the detail info of the second-level page !
   And this page include Weather Forecast items.
   */
  TQYB_2PAGE,
  /*!
     The xml file which descripe the detail info of the third level page page !
   And this page include detail of every item.
   */
  XWSS_3PAGE
} abs_xml_type_t;


/*!
  FDT data structure which used to parse the raw data!
  */
typedef struct
{
  /*!
     ext id of current FDT table!
    */
  u16                table_id_ext;
  /*!
    FDT version,if information service updated,the version will changed!
    */
  u8                 version;
  /*!
    this file type:xml,bmp,mpg?
    */
  abs_is_file_type_t file_type;
  /*!
    this xml file type:root?news 2nd;tqyb 2nd; news 3rd?
    */
  abs_xml_type_t     xml_file_type;
  /*!
    section number
    */
  u8                 sec_number;
  /*!
    Last section number
    */
  u8                 last_sec_number;
  /*!
     file data length
    */
  u32                file_data_length;
  /*!
    file data buffer
    */
  u8 *                p_file_data;
} abs_fdt_t;

/*!
  The temp buffer structure which used to recv the raw data!(TS packet mode)
  */
typedef struct
{
  /*!
     the pid of fdt packets!
   */
  u16 fdt_pid;
  /*!
    the pti ring buffer ptr!
   */
  filter_ext_buf_t * p_fdt_extern_buf;
} abs_fdt_req_context_t;

/*!
  Parse fdt section.

  \param[in] handle Service handle
  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_abs_fdt(handle_t handle, dvb_section_t *p_sec) ;
/*!
  Request FDT raw data with multi-section mode

  \param[in] p_sec: Section information for request operation in DVB
  \param[in] table_id: FDT table id
  \param[in] para2: the context buffer which store the pid info and data buffer ptr
  */
void request_abs_fdt_multi_section(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __ABS_FDT_H_
