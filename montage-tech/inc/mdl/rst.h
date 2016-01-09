/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __RST_H_
#define __RST_H_

/*!
  running status event description
  */
typedef struct
{
  /*!
    Ts id
    */
  u16 ts_id ;
  /*!
    Original network id
    */
  u16 org_id;
  /*!
    Service id 
    */
  u16 svc_id;
  /*!
    Event id
    */
  u16 evt_id;
  /*!
    Running status
    */
  u8 run_sts;
}rst_evt_des_t;

/*!
  Rst table definition
  */
typedef struct
{
  /*!
    Event count
    */
  u8 evt_cnt;
  /*!
    Event running status description
    */
  rst_evt_des_t evt_des[DVB_MAX_RST_EVT_NUM];
}rst_t;

/*!
  Parse rst section.

  \param[in] p_sec: section data filter by PTI driver.
  */
void parse_rst(dvb_section_t *p_sec);

/*!
  Request a RST section data.

  \param[in] sec_number: EIT section number in TS.
  */
void request_rst_section(u32 sec_number);

#endif // End for __RST_H_
