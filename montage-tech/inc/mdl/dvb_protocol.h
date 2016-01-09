/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_PROTOCOL_H_
#define __DVB_PROTOCOL_H_
/*!
  \file dvb_protocol.h
  
  This file provides the common value definition used in dvb protocol
  */

/*!
  Max section length
  */
#define MAX_SECTION_LENGTH                (4096)  


/*!
  CRC length
  */
#define CRC_LEN     (4)
/*!
  Ts packet timeout interval
  */
#define MPEG_TS_PACKET_TIMEOUT             4000

/*!
  tkgs time out interval(5s)
  */
#define TKGS_TIMEOUT                       5000  

/*!
  PAT time out interval
  */
#define PAT_TIMEOUT                        2000
/*!
  CAT time out interval
  */
#define CAT_TIMEOUT                        2000
/*!
  PMT time out interval
  */
#define PMT_TIMEOUT                        4000
/*!
  SDT time out interval
  */
#define SDT_TIMEOUT                        5000//15000
/*!
  BAT time out interval
  */
#define BAT_TIMEOUT                        5000
/*!
  FDT time out interval
  */
#define FDT_TIMEOUT                        25000
/*!
  NIT time out interval
  */
#define NIT_TIMEOUT                       (15000)
/*!
  EIT time out interval
  */
#define EIT_TIMEOUT                        10000
/*!
   Fix me !!!!
   EMM TIMEOUT
  */
#define EMM_TIMEOUT                       1000

/*!
   Fix me !!!!
   EMM TIMEOUT
  */
#define ECM_TIMEOUT                       1000

/*!
  TOT time out interval
  */
#define TOT_TIMEOUT                        200
/*!
  TDT time out interval
  */
#define TDT_TIMEOUT                        60000
/*!
  RST time out interval
  */
#define RST_TIMEOUT                        500
/*!
	DSMCC DDB time out interval
  */
#define DSMCC_DDB_TIMEOUT                  50000
/*!
  DSMCC MSG time out interval
  */
#define DSMCC_MSG_TIMEOUT                  50000
/*!
  DDS time out interval
  */
#define DDS_TIMEOUT                        20000
/*!
  FLAG as the table id when request ts packet
  */
#define MPEG_TS_PACKET                     0xFD
/*!
	PAT PID
  */
#define DVB_PAT_PID                        0x00
/*!
  CAT PID
  */
#define DVB_CAT_PID                        0x01 
/*!
  PMT PID
  */
#define DVB_PMT_PID                        0x02
/*!
  NIT PID
  */
#define DVB_NIT_PID                        0x10
/*!
  SDT PID
  */
#define DVB_SDT_PID                        0x11
/*!
  EIT PID
  */
#define DVB_EIT_PID                        0x12
/*!
  RST PID
  */
#define DVB_RST_PID                        0x13
/*!
  TDT PID
  */
#define DVB_TDT_PID                        0x14
/*!
	BAT PID
  */
#define DVB_BAT_PID                        0x11
/*!
  TOT PID
  */
#define DVB_TOT_PID                        0x14
/*!
  SIT PID
  */
#define DVB_SIT_PID                        (0x001F)
/*!
  PAT table id
  */
#define DVB_TABLE_ID_PAT                   0x00
/*!
  CAT table id
  */
#define DVB_TABLE_ID_CAT                   0x01
/*!
  PMT table id
  */
#define DVB_TABLE_ID_PMT                   0x02
/*!
  NIT table id for actual stream
  */
#define DVB_TABLE_ID_NIT_ACTUAL            0x40
/*!
  NIT table id for other stream
  */
#define DVB_TABLE_ID_NIT_OTHER             0x41
/*!
  SDT table id for actual stream
  */
#define DVB_TABLE_ID_SDT_ACTUAL            0x42
/*!
  SDT table id for other stream
  */
#define DVB_TABLE_ID_SDT_OTHER             0x46
/*!
  BAT table id
  */
#define DVB_TABLE_ID_BAT                   0x4A
/*!
  EIT table id for actual stream with PF info
  */
#define DVB_TABLE_ID_EIT_ACTUAL            0x4E 
/*!
  EIT table id for other stream with PF info
  */
#define DVB_TABLE_ID_EIT_OTHER             0x4F
/*!
  EIT table id for actual stream with schedule info
  */
#define DVB_TABLE_ID_EIT_SCH_ACTUAL        0x50
/*!
  EIT table id 0x51 for actual stream with schedule info
  */
#define DVB_TABLE_ID_EIT_SCH_ACTUAL_51     0x51

/*!
  Max table id of EIT schedule in actual stream
  */
#define DVB_TABLE_ID_EIT_SCH_ACTUAL_MAX    0x5F 
/*!
  Table id of EIT schedule in other stream
  */
#define DVB_TABLE_ID_EIT_SCH_OTHER         0x60
/*!
  Table id 61 of EIT schedule in other stream
  */
#define DVB_TABLE_ID_EIT_SCH_OTHER_61      0x61
/*!
  Max table id of EIT schedule in other stream
  */
#define DVB_TABLE_ID_EIT_SCH_OTHER_MAX     0x6F
/*!
  TDT table id
  */
#define DVB_TABLE_ID_TDT                   0x70
/*!
  RST table id
  */
#define DVB_TABLE_ID_RUN_STATUS            0x71
/*!
  Stuffing table id
  */
#define DVB_TABLE_ID_STUFFING              0x72
/*!
  TOT table id
  */
#define DVB_TABLE_ID_TOT                   0x73
/*!
  SIT table id
  */
#define DVB_TABLE_ID_SIT                   (0x7F)
/*!
  DSMCC DDM table id
  */
#define DVB_TABLE_ID_DSMCC_DDM             0x3C
/*!
  DSMCC MSG table id
  */
#define DVB_TABLE_ID_DSMCC_MSG             0x3B
/*!
  Table id for message section of ECM
  */
#define DVB_TABLE_ID_ECM                 0x80
/*!
  Table id for EMM information
  */
#define DVB_TABLE_ID_EMM                 0x82
/*!
  FDT table id
  */
#define DVB_TABLE_ID_FDT                   0x90
/*!
  ABS download data table id
  */
#define DVB_TABLE_ID_DDS                   0xA0
/*!
  TKGS table id
  */
#define DVB_TABLE_ID_TKGS                  0xA7
/*!
  Stream tag definitions in stream type desc
  */
/*!
  VIDEO TAG for MPEG1
  */
#define DVB_STREAM_MG1_VIDEO               0x01
/*!
  VIDEO TAG for MPEG2
  */
#define DVB_STREAM_MG2_VIDEO               0x02
/*!
  VIDEO TAG for AVS
  */
#define DVB_STREAM_AVS_VIDEO               0x42
/*!
  VIDEO TAG for MPEG4
  */
#define DVB_STREAM_MPEG4_VIDEO               0x10

/*!
  AUDIO TAG for MPEG1
  */
#define DVB_STREAM_MG1_AUDIO               0x03
/*!
  AUDIO TAG for MPEG2
  */
#define DVB_STREAM_MG2_AUDIO               0x04
/*!
  TAG for system private data description
  */
#define DVB_STREAM_PRIVATE_SEC             0x05
/*!
  TAG for user private data description
  */
#define DVB_STREAM_PESRIVA_SEC             0x06

/*!
  CA descriptor id
  */
#define CA_DESC_ID                         (0x9)
/*!
  TAG for AAC audio desc
  */
#define DVB_STREAM_AAC_AUDIO               0x0F

/*!
  TAG for AAC audio desc
  */
#define DVB_STREAM_AAC_AUDIO_1              0x11

/*!
  TAG for OTA SEC
  */
#define DVB_STREAM_OTA_SEC                 0xA0
/*!
  TAG FOR 264 VIDEO DESC
  */
#define DVB_STREAM_264_VIDEO               0x1B
/*!
  TAG FOR AVS VIDEO DESC
  */
#define DVB_STREAM_AVS_VIDEO               0x42
/*!
  TAG FOR CA DESC
  */
#define DVB_DESC_CA                        0x09
/*!
  TAG FOR LANGUAGE CODE
  */
#define DVB_DESC_ISO_639_LANGUAGE          0x0A
/*!
  TAG FOR MAXIMUM BIT RATE
  */
#define DVB_DESC_MAXIMUM_BITRATE           0x0E

/*!
  Descriptor id in all PSI and SI table
  */
/*!
  NETWORK NAME descriptor id
  */
#define DVB_DESC_NETWORK_NAME              0x40
/*!
  NETWORK NAME descriptor id
  */
#define DVB_DESC_SERVICE_LIST              0x41
/*!
  Stuffing descriptor id
  */
#define DVB_DESC_STUFFING                  0x42
/*!
  Satellite delivery descriptor id
  */
#define DVB_DESC_SATELLITE_DELIVERY        0x43
/*!
  Cable delivery descriptor id
  */
#define DVB_DESC_CABLE_DELIVERY            0x44
/*!
  Terrestrial delivery system descriptor  
  */
#define DVB_DESC_TERRERTRIAL_DELIVERY      0x5A
/*!
  VBI teletext descriptor id
  */
#define DVB_DESC_VBI_TELETEXT              0x46
/*!
  Bouquet name descriptor id
  */
#define DVB_DESC_BOUQUET_NAME              0x47
/*!
  Service descriptor id
  */
#define DVB_DESC_SERVICE                   0x48
/*!
  Linkage descriptor id
  */
#define DVB_DESC_LINKAGE                   0x4a
/*!
  logical channel descriptor id
  */
#define DVB_DESC_LOGICAL_CHANNEL           0x81
/*!
  NVOD reference descriptor id
  */
#define DVB_DESC_NVOD_REFERENCE            0x4b
/*!
  Time shifted service descriptor id
  */
#define DVB_DESC_TIME_SHIFTED_SERVICE      0x4c
/*!
  Short event descriptor id
  */
#define DVB_DESC_SHORT_EVENT               0x4d
/*!
  Extend event
  */
#define DVB_DESC_EXT_EVENT                 0x4e
/*!
  Time shifted event descriptor id
  */
#define DVB_DESC_TIME_SHIFTED_EVT          0x4f
/*!
  Component descriptor id
  */
#define DVB_DESC_COMPONENT_DESC            0x50
/*!
  mosaic descriptor id
  */
#define DVB_DESC_MOSAIC_DESC               0x51
/*!
  Stream identifier descriptor id
  */
#define DVB_DESC_STREAM_IDENTIFIER         0x52

/*!
  CA scramble descriptor
  */
#define DVB_CA_IDENTIFIER                  0x53

/*!
  Content descriptor id
  */
#define DVB_DESC_CONTENT                   0x54
/*!
  Parental rating descriptor id
  */
#define DVB_DESC_PARENTAL_RATING           0x55
/*!
  Teletext descriptor id
  */
#define DVB_DESC_TELETEXT                  0x56
/*!
  Local time offset descriptor id
  */
#define DVB_DESC_LOALTIME_OFFSET           0x58
/*!
  Subtitle descriptor id
  */
#define DVB_DESC_SUBTITLE                  0x59
/*!
  Terrestrial delivery descriptor id
  */
#define DVB_DESC_TERRESTRIAL_DELIVERY      0x5A
/*!
  Terrestrial 2 delivery descriptor id
  */
#define DVB_DESC_TERRESTRIAL2_DELIVERY      0x04
/*!
  Multi-lingual network name descriptor id
  */
#define DVB_DESC_MULTILIGUAL_NETWORK_NAME  0x5B
/*!
  Multi-lingual bouquet name descriptor id 
  */
#define DVB_DESC_MBOUQUET_NAME             0x5C
/*!
  Multi-lingual service name descriptor id
  */
#define DVB_DESC_ML_SVCNAME                0x5D
/*!
  Private data specifier descriptor id
  */
#define DVB_DESC_PRIVATE_DATA              0x5F
/*!
  Frequency list descriptor id
  */
#define DVB_DESC_FREQUENCY_LIST            0x62
/*!
  CA system descriptor id
  */
#define DVB_DESC_CA_SYSTEM                 0x65
/*!
  Data broadcast descriptor id
  */
#define DVB_DESC_DATA_BRODCAST_ID          0x66
/*!
  AC3 descriptor id
  */
#define DVB_DESC_AC3                       0x6A
/*!
  E-AC3 descriptor id
  */
#define DVB_DESC_EAC3                      0x7A
/*!
  Cell list descriptor id
  */
#define DVB_DESC_CELL_LIST                 0x6C
/*!
  Cell frequency descriptor id
  */
#define DVB_DESC_CELL_FREQUE               0x6D
/*!
  Default authority descriptor id
  */
#define DVB_DESC_DEFAULT_AUTHORITY         0x73
/*!
  Time slice descriptor id
  */
#define DVB_DESC_TIME_SLICE                0x77
/*!
  S2 satellite delivery descriptor id
  */
#define DVB_DESC_S2_SATELLITE_DELIVERY     0x79
/*!
  Extension descriptor id
  */
#define DVB_DESC_EXTENSION                 0x7F
/*!
  Service update descriptor id in ABS project
  */
#define DVB_DESC_SERVICE_UPDATE            0x80
/*!
  VCR programming descriptor id
  */
#define DVB_DESC_VCR_PROGRAMMING           0x80
/*!
  Logic channel descriptor id in ABS project
  */
#define DVB_DESC_LOGIC_CHANNEL             0x81
/*!
  logical screen descriptor
  */
#define DVB_DESC_LOGIC_SCREEN              0x81
/*!
  stream type of ac3
  */
#define DVB_STREAM_AC3_AUDIO               0x81
/*!
  Access descriptor id
  */
#define DVB_DESC_ACCESS                    0x81
/*!
  Channel volume compensation descriptor id
  */
#define DVB_DESC_CHNL_VLM_CMPST            0x82
/*!
  logical channel descriptor describle LCN(for DVB-T)
  */
#define DVB_DESC_LCD                        0x83


/*!
  TKGS standard descriptor
  */
#define DVB_DESC_TKSG_STANDARD             0x91

/*!
  TKGS multilingual user message descriptor
  */
#define DVB_DESC_TKGS_USER_MSG             0x90

/*!
  TKGS multilingual category label descriptor.
  */
#define DVB_DESC_CATEGORY_LABEL      0x92

/*!
  TKGS multilingual service list name descriptor
  */
#define DBV_DESC_MULTI_SVC_LIST_NAME       0x93

/*!
  TKGS service list location descriptor
  */
#define DVB_DESC_SVC_LIST_LOCATION         0x94

/*!
  TKGS extended service descriptor
  */
#define DVB_DESC_TKGS_EXTEND_SVC           0x95 

/*!
  TKGS constant control word descriptor
  */
#define DVB_DESC_TKGS_CONSTANT_CTR_WORD    0x96  

/*!
  TKGS component descriptor
  */
#define DVB_DESC_TKGS_COMPONENT            0x98

/*! 
  TKGS satellite broadcast source descriptor
  */
#define DVB_DESC_TKGS_SATELLITE            0xA0

/*!
  TKGS cable broadcast source descriptor  
  */
#define DVB_DESC_TKGS_CABLE                0xA1

/*!
  TKGS terrestrial broadcast source descriptor
  */
#define DVB_DESC_TKGS_TERRESTRIAL          0xA2


/*!
  ABS software upgrade descriptor id
  */
#define ABS_DESC_SOFTEWARE_UPGRADE         0xE2

/*!
  private ad descriptor
  */
#define DVB_PRV_DESC_AD                     0xD2


/*!
  Ts packets id for ts ota
  */
#define DVB_TABLE_ID_OTA_TS 0xEE

/*!
  download control section
  */
#define DVB_TABLE_ID_DCS 0xA2

/*!
  partition control section
  */
#define DVB_TABLE_ID_PCS 0xFD

/*!
  partition control section
  */
#define DVB_TABLE_ID_DTM 0xFC

/*!
  MAX program number in PAT
  */
#define DVB_MAX_PAT_PROG_NUM   128
/*! 
  Max event number per each section while each section can record events only 
  in  three hours
  */
#define DVB_MAX_SDT_SVC_NUM    64
/*! 
  Max event number per each section while each section can record events only 
  in  three hours
  */
#define DVB_MAX_BAT_SVC_NUM    512
/*!
  MAX event number in rst
  */
#define DVB_MAX_RST_EVT_NUM    20

/*!
  nvod reference service
  */
#define DVB_NVOD_REF_SVC          (0x04)
/*!
  nvod time shifted service
  */
#define DVB_NVOD_TIME_SHIFTED_SVC (0x05)
/*!
  mosaic service
  */
#define DVB_MOSAIC_SVC              (0x06)

#endif // End for __DVB_PROTOCOL_H_

