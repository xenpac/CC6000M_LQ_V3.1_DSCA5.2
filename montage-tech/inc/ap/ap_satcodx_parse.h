/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_SATCODX_PARSE_H_
#define __AP_SATCODX_PARSE_H_

/*!
  \file ap_satcodx_parse.h
  This is the declearation of parse module in Satcodx app.
  */

/*!
  Sync header string in SDX structure
  */
#define SATCODX_HEADER_STR ("SATCODX")
/*!
  Version 03 string in SDX structure
  */
#define SATCODX_V3_STR ("03")
/*!
  Version 04 string in SDX structure
  */
#define SATCODX_V4_STR ("04")

/*!
  Size of SDX structure for version 3
  */
#define SATCODX_V3_BLOCK_SIZE 127
/*!
  Size of SDX structure for version 4
  */
#define SATCODX_V4_BLOCK_SIZE 131

/*!
  Length of header in SDX structure
  */
#define SATCODX_HEADER_LEN                             7
/*!
  Length of version in SDX structure
  */
#define SATCODX_VERSION_LEN                            2
/*!
  Sync ver offset in SDX structure
  */
#define SATCODX_SYNC_VER_OFFSET (SATCODX_HEADER_LEN + 1)
/*!
  Sync byte lengthin SDX structure
  */
#define SATCODX_SYNC_BYTE_LEN                        \
             (SATCODX_SYNC_VER_OFFSET + SATCODX_VERSION_LEN)
/*!
  Length of satellite name in SDX structure
  */
#define SATCODX_SAT_NAME_LEN                          18
/*!
  Length of type in SDX structure
  */
#define SATCODX_TYPE_LEN                                   4
/*!
  Length of QRG in SDX structure
  */
#define SATCODX_QRG_LEN                                    9
/*!
  Length of frequency in SDX structure
  */
#define SATCODX_FREQ_VALID_LEN                       6
/*!
  Length of program name in SDX structure
  */
#define SATCODX_PROG_NAME_LEN                       8
/*!
  Length of program extension name in SDX structure
  */
#define SATCODX_PROG_NAME_EXT_LEN               12
/*!
  Length of position code in SDX structure
  */
#define SATCODX_POS_CODE_LEN                         4
/*!
  Length of cover code in SDX structure
  */
#define SATCODX_COVER_CODE_LEN                     8
/*!
  Length of audio left in SDX structure
  */
#define SATCODX_AUDIO_LEFT_LEN                       3
/*!
  Length of audio right in SDX structure
  */
#define SATCODX_AUDIO_RIGHT_LEN                     3
/*!
  Length of symbol rate in SDX structure
  */
#define SATCODX_SYMBOL_RATE_LEN                     5
/*!
  Length of video pid in SDX structure
  */
#define SATCODX_VPID_LEN                                   4
/*!
  Length of audio pid in SDX structure
  */
#define SATCODX_APID_LEN                                   4
/*!
  Length of pcr pid in SDX structure
  */
#define SATCODX_PCR_PID_LEN                             4
/*!
  Length of service pid in SDX structure
  */
#define SATCODX_SID_LEN                                     5
/*!
  Length of network pid in SDX structure
  */
#define SATCODX_NID_LEN                                     5
/*!
  Length of ts pid in SDX structure
  */
#define SATCODX_TID_LEN                                     5
/*!
  Length of bc language in SDX structure
  */
#define SATCODX_BC_LANGUAGE_LEN                   3
/*!
  Length of country code in SDX structure
  */
#define SATCODX_COUNTRY_CODE_LEN                2
/*!
  Length of language code in SDX structure
  */
#define SATCODX_LANGUAGE_CODE_LEN              3
/*!
  Length of cryptography mode in SDX structure
  */
#define SATCODX_CRYPT_MODE_LEN                     4
/*!
  Length of pmt pid in SDX structure
  */
#define SATCODX_PMT_PID_LEN                            4
/*!
  Eos in SDX structure
  */
#define SATCODX_CHAR_EOS                                 ('\0')
/*!
  Space in SDX structure
  */
#define SATCODX_CHAR_SPACE                              (' ')
/*!
  Channel type tv in SDX structure
  */
#define SATCODX_CHANNEL_TYPE_TV                    ('T')
/*!
  Channel type radio in SDX structure
  */
#define SATCODX_CHANNEL_TYPE_RADIO              ('R')
/*!
  Channel type data in SDX structure
  */
#define SATCODX_CHANNEL_TYPE_DATA                ('D')
/*!
  Channel type package in SDX structure
  */
#define SATCODX_CHANNEL_TYPE_PACKAGE          ('_')
/*!
  Polarity vertical in SDX structure
  */
#define  SATCODX_POL_TYPE_V                             ('0')
/*!
  Polarity horizontal in SDX structure
  */
#define  SATCODX_POL_TYPE_H                             ('1')
/*!
  Polarity left circular in SDX structure
  */
#define  SATCODX_POL_TYPE_LEFT_CIRCULAR      ('2')
/*!
  Polarity right circular in SDX structure
  */
#define  SATCODX_POL_TYPE_RIGHT_CIRCULAR    ('3')
/*!
  Position code presicion
  */
#define SATCODX_POS_CODE_PRESICION             10
/*!
  Position code round
  */
#define SATCODX_POS_CODE_ROUND                   36000
/*!
  Position code east limitation
  */
#define SATCODX_POS_CODE_EAST_LIMIT            18000
/*!
  Position code east flag
  */
#define SATCODX_POS_CODE_EAST_FLAG             ((u16) 0x0000)
/*!
  Position code west flag
  */
#define SATCODX_POS_CODE_WEST_FLAG            ((u16) 0x8000)
/*!
  Universal lnb type
  */
#define SATCODX_UNIVERSAL_LNB_TYPE               2
/*!
  Universal lnb low
  */
#define SATCODX_UNIVERSAL_LNB_LOW               9750
/*!
  Universal lnb high
  */
#define SATCODX_UNIVERSAL_LNB_HIGH              10600
/*!
  Default pid value
  */
#define SATCODX_PID_DEFAULT_VALUE                 8191

/*!
  SDX3 structure
  */
typedef struct _SDX3_st_
{
  /*!
    should be 'SATCODX'
    */
  s8 Header[SATCODX_HEADER_LEN];
  /*!
    Settings page
    */
  s8 SPage;
  /*!
    Satcodx Protocol Version
    */
  s8 Version[SATCODX_VERSION_LEN];
  /*!
    should be 'LICENSEREQUIRED'
    */
  s8 SatName[SATCODX_SAT_NAME_LEN];
  /*!
    T:Tv / R:Radio / D:Data / _:Package
    */
  s8 TR;
  /*!
    broadcast system e.g. : PAL_, MPG2, ...
    */
  s8 Type[SATCODX_TYPE_LEN];
  /*!
    Frequency 'GHzMHzkHz'
    */
  s8 QRG[SATCODX_QRG_LEN];
  /*!
    Polarization 0:v 1:h 2:l-circ 3:r-circ
    */
  s8 Pol;
  /*!
    Program name (first 8 letters)
    */
  s8 Name[SATCODX_PROG_NAME_LEN];
  /*!
    Position code 0192 : astra ... 
    */
  s8 PosCode[SATCODX_POS_CODE_LEN];
  /*!
    SatcoDX coverage code
    */
  s8 CoverCode[SATCODX_COVER_CODE_LEN];
  /*!
    analog audio left
    */
  s8 AudioLeft[SATCODX_AUDIO_LEFT_LEN];
  /*!
    analog audio right
    */
  s8 AudioRight[SATCODX_AUDIO_RIGHT_LEN];
  /*!
    Symbol Rate
    */
  s8 SRate[SATCODX_SYMBOL_RATE_LEN];
  /*!
    fec
    */
  s8 FEC;
  /*!
    Video PID
    */
  s8 VPID[SATCODX_VPID_LEN];
  /*!
    Audio PID
    */
  s8 APID[SATCODX_APID_LEN];
  /*!
    PCR PID
    */
  s8 PCR[SATCODX_PCR_PID_LEN];
  /*!
    Service PID
    */
  s8 SID[SATCODX_SID_LEN];
  /*!
    Network PID
    */
  s8 NID[SATCODX_NID_LEN];
  /*!
    TS PID
    */
  s8 TID[SATCODX_TID_LEN];
  /*!
    main broadcast language
    */
  s8 BCLanguage[SATCODX_BC_LANGUAGE_LEN];
  /*!
    should be '_'
    */
  s8 Always_;
  /*!
    Country code
    */
  s8 CountryCode[SATCODX_COUNTRY_CODE_LEN];
  /*!
    Language code
    */
  s8 LanguageCode[SATCODX_LANGUAGE_CODE_LEN];
  /*!
    Cryptography Mode
    */
  s8 CryptMode[SATCODX_CRYPT_MODE_LEN];
  /*!
    second part of name
    */
  s8 Name2[SATCODX_PROG_NAME_EXT_LEN];
} SDX3;

/*!
  SDX3 pointer type to structure
  */
typedef SDX3 *PSDX3;

/*!
  SDX3 pointer type to pointer type
  */
typedef SDX3   **PPSDX3; 

/*!
  SDX4 structure
  */
typedef struct _SDX4_st_
{
  /*!
    should be 'SATCODX'
    */
  s8 Header[SATCODX_HEADER_LEN];
  /*!
    Settings page
    */
  s8 SPage;
  /*!
    Satcodx Protocol Version
    */
  s8 Version[SATCODX_VERSION_LEN];
  /*!
    should be 'LICENSEREQUIRED'
    */
  s8 SatName[SATCODX_SAT_NAME_LEN];
  /*!
    T:Tv / R:Radio / D:Data / _:Package
    */
  s8 TR;
  /*!
    broadcast system e.g. : PAL_, MPG2, ...
    */
  s8 Type[SATCODX_TYPE_LEN];
  /*!
    Frequency 'GHzMHzkHz'
    */
  s8 QRG[SATCODX_QRG_LEN];
  /*!
    Polarization 0:v 1:h 2:l-circ 3:r-circ
    */
  s8 Pol;
  /*!
    Program name (first 8 letters)
    */
  s8 Name[SATCODX_PROG_NAME_LEN];
  /*!
    Position code 0192 : astra ... 
    */
  s8 PosCode[SATCODX_POS_CODE_LEN];
  /*!
    SatcoDX coverage code
    */
  s8 CoverCode[SATCODX_COVER_CODE_LEN];
  /*!
    analog audio left
    */
  s8 AudioLeft[SATCODX_AUDIO_LEFT_LEN];
  /*!
    analog audio right
    */
  s8 AudioRight[SATCODX_AUDIO_RIGHT_LEN];
  /*!
    Symbol Rate
    */
  s8 SRate[SATCODX_SYMBOL_RATE_LEN];
  /*!
    fec
    */
  s8 FEC;
  /*!
    Video PID
    */
  s8 VPID[SATCODX_VPID_LEN];
  /*!
    Audio PID
    */
  s8 APID[SATCODX_APID_LEN];
  /*!
    PCR PID
    */
  s8 PCR[SATCODX_PCR_PID_LEN];
  /*!
    Service PID
    */
  s8 SID[SATCODX_SID_LEN];
  /*!
    Network PID
    */
  s8 NID[SATCODX_NID_LEN];
  /*!
    TS PID
    */
  s8 TID[SATCODX_TID_LEN];
  /*!
    main broadcast language
    */
  s8 BCLanguage[SATCODX_BC_LANGUAGE_LEN];
  /*!
    should be '_'
    */
  s8 Always_;
  /*!
    Country code
    */
  s8 CountryCode[SATCODX_COUNTRY_CODE_LEN];
  /*!
    Language code
    */
  s8 LanguageCode[SATCODX_LANGUAGE_CODE_LEN];
  /*!
    Cryptography mode
    */
  s8 CryptMode[SATCODX_CRYPT_MODE_LEN];
  /*!
    second part of name
    */
  s8 Name2[SATCODX_PROG_NAME_EXT_LEN];
  /*!
    decimal PMT PID code	
    */
  s8 PmtPID[SATCODX_PMT_PID_LEN];
} SDX4;

/*!
  SDX4 pointer type to structure
  */
typedef SDX4 *PSDX4;

/*!
  Satcodx node position
  */
typedef struct
{
  /*!
    pointer to buffer position
    */
  u8 *p_buf_pos;
  /*!
    current buffer size
    */
  u32 cur_buf_size;
  /*!
    pointer to node
    */
  u8 *p_node;
  /*!
    current node size
    */
  u32 cur_node_size;
}satcodx_node_pos_t;


/*!
  Convert ascii string into u32
  \param[in] p_buf pointer to ascii string
  \param[in] buf_size size of ascii string
  */
u32 ap_satcodx_parse_get_u32_from_data_buf(
                       u8 *p_buf, u32 buf_size);

/*!
  Init satcodx parser
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
BOOL ap_satcodx_parser_init(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Deinit satcodx parser
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
void ap_satcodx_parser_deinit(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Search satcodx sync header from data buffer
  \param[out] p_satcodx_node pointer to satcodx node pos in buffer
  */
BOOL ap_satcodx_parse_search_sync_header(
                       satcodx_node_pos_t * p_satcodx_node);

/*!
  Push satcodx node into satcodx implementation data  buffer
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  \param[in] p_satcodx_node pointer to sacodx node pos
  */
BOOL ap_satcodx_parser_push_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_node_pos_t *p_satcodx_node);

/*!
  Push satcodx data into satcodx implementation data  buffer
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
void ap_satcodx_parser_push_data(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Create parser database list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
BOOL ap_satcodx_parse_create_db_list(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Free parser database list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
void   ap_satcodx_parse_free_db_list(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Create parser result list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
BOOL ap_satcodx_parse_create_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Free parser result list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
void   ap_satcodx_parse_free_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Create parser database satellite list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  */
BOOL ap_satcodx_parse_create_sat_list(
                       satcodx_impl_data_t *p_satcodx_impl_data);

/*!
  Add satellite node into result list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_sat_node pointer to satcode satellite node
  */
BOOL ap_satcodx_parse_add_sat_node_to_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data, 
                       sat_node_t *p_sat_node);                       

/*!
  Free satellite list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data  
  \param[out] p_result_list pointer to satcodx result list
  */
void  ap_satcodx_parse_free_sat_list(
                       satcodx_impl_data_t *p_satcodx_impl_data, 
                       satcodx_result_list_t *p_result_list);

/*!
  Create tp list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_sat_node pointer to satcodx satllite node
  */
BOOL ap_satcodx_parse_create_tp_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t *p_sat_node);

/*!
  Add tp node into satcodx result list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] p_sat_node pointer to satcodx satllite node
  \param[in] p_dvbs_tp_node pointer to dvbs tp node
  */
BOOL ap_satcodx_parse_add_tp_node_to_result_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t *p_sat_node,
                       dvbs_tp_node_t *p_dvbs_tp_node);

/*!
  Free tp list
  \param[out] p_satcodx_impl_data pointer to satcodx implementation data
  \param[in] p_sat_node pointer to satcodx satllite node
  */
void  ap_satcodx_parse_free_tp_list(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       satcodx_sat_node_t *p_sat_node);

/*!
  Create tp list
  \param[out] pSdx3 pointer to SDX3 structure
  */
BOOL ap_satcodx_parser_filter_data(PSDX3 pSdx3);

/*!
  Obtain satellite node from SDX3 structure
  \param[in] pSdx3 pointer to SDX3 structure
  \param[out] p_sat_node pointer to satcodx satllite node
  */
BOOL ap_satcodx_parser_get_sat_node(PSDX3 pSdx3, 
                       sat_node_t *p_sat_node);

/*!
  Obtain tp node from SDX3 structure
  \param[in] pSdx3 pSdx3 pointer to SDX3 structure
  \param[out] p_tp_node pointer to satcodx tp node
  */
BOOL ap_satcodx_parser_get_tp_node(PSDX3 pSdx3,
                       dvbs_tp_node_t *p_tp_node);

/*!
  Obtain program node from SDX3 structure
  \param[in] pSdx3 pointer to SDX3 structure
  \param[out] p_prog_node pointer to satcodx program node
  */
BOOL ap_satcodx_parser_get_prog_node(PSDX3 pSdx3,
                       dvbs_prog_node_t *p_prog_node);

/*!
  Store satellite node into satcodx implementation data
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_sat_node pointer to satcodx satllite node
  */
BOOL ap_satcodx_parser_store_sat_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       sat_node_t *p_sat_node);

/*!
  Store tp node into satcodx implementation data
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_dvbs_tp_node pointer to satcodx tp node
  */
BOOL ap_satcodx_parser_store_tp_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       dvbs_tp_node_t *p_dvbs_tp_node);

/*!
  Store program node into satcodx implementation data
  \param[in] p_satcodx_impl_data pointer to satcodx implementation data
  \param[out] p_dvbs_prog_node pointer to satcodx program node
  */
BOOL ap_satcodx_parser_store_prog_node(
                       satcodx_impl_data_t *p_satcodx_impl_data,
                       dvbs_prog_node_t *p_dvbs_prog_node);

#endif // End for __AP_SATCODX_PARSE_H_

