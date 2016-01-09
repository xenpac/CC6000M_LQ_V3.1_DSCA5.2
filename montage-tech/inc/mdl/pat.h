/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __PAT_H_
#define __PAT_H_

/*!
  Program info in PAT
  */
typedef struct
{
  /*!
    LNB number
    */
  u32 lbn : 16;
  /*!
    PMT pid
    */
  u32 pmt_pid : 16;
} pat_prog_t;

/*!
  PAT struct
  */
typedef struct
{
  /*!
    Programs info in PAT
    */
  pat_prog_t progs[DVB_MAX_PAT_PROG_NUM];
  /*!
    Program number in PAT and this value equal to service id
    */
  u16 prog_num;
} pat_t;

/*!
  Parse pat section

  \param[in] p_sec: Section data filtered by PTI driver
  */
void parse_pat(handle_t handle, dvb_section_t *p_sec);

/*!
  Request a PAT data
  \param[in] p_sec: section information
  \param[in] para1: pat table id
  \param[in] para2 useless parameter
  */
void request_pat(dvb_section_t *p_sec, u32 table_id, u32 para2);

/*!
  Request a PAT data for DVBT
  \param[in] p_sec: section information
  \param[in] para1: pat table id
  \param[in] para2 useless parameter
  */
void request_pat_dvbt(dvb_section_t *p_sec, u32 table_id, u32 para2);

#endif // End for __PAT_H_

