/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ABS_UTIL_H_
#define __ABS_UTIL_H_

/*!
  Terminal device Information(TDI)
  ref GD/J 027-2009 4.3.3.1.3.1
  */
typedef struct
{
  /*!
    manufacture id
    */
  u8 manufacture_id;
  /*!
    model id
    */
  u8 model_id;
  /*!
    hardware id
    */
  u8 hardware_id;
  /*!
    TDI version
    */
  u8 tdi_version;
  /*!
    CA indent number
    */
  u32 ca_ident_number;
  /*!
    Software release number
    */
  u32 release_number;
  /*!
    Broadcaster
    */
  u32 broadcaster;
  /*!
    KIB
    */
  u16 kib[8];
  /*!
    STB id
    */
  u8 stb_id[5];
  /*!
    loader version
    */
  u16 loader_version;
  /*!
    reserved
    */
  u8 reserved[213];
  /*!
    CRC number
    */
  u32 tdi_crc;
} abs_tdi_t;

/*!
  ABS flash header
  ref GD/J 027-2009 4.3.3.1.3.1
  */
typedef struct
{
  /*!
    Last upgrade software version
    */
  u16 software_version;
  /*!
    reserved
    */
  u8 reserved1[2];
  /*!
    NVRAM data in last upgrade
    */
  u32 last_download_parameters[8];
  /*!
    Number of delivery
    */
  u8 delivery_count;
  /*!
    default delivery 1
    */
  u8 delivery_1[14];
  /*!
    default delivery 2
    */
  u8 delivery_2[14];
  /*!
    default delivery 3
    */
  u8 delivery_3[14];
  /*!
    default delivery 4
    */
  u8 delivery_4[14];
  /*!
    default delivery 5
    */
  u8 delivery_5[14];
  /*!
    Software signature
    */
  u8 signature[152];
  /*!
    reserved
    */
  u8 reserved2[121];
  /*!
    Flash header CRC
    */
  u32 flash_header_crc;
} abs_flash_header_t;

/*!
  Read tdi

  \param[in,out] p_tdi TDI data
  */
void abs_util_read_tdi(abs_tdi_t *p_tdi);

/*!
  Read ABS flash header

  \param[in,out] p_header Header data

  \return FALSE means we meet a bad(CRC failed) header
  */
BOOL abs_util_read_flash_header(abs_flash_header_t *p_header);

#endif // End for __ABS_UTIL_H_

