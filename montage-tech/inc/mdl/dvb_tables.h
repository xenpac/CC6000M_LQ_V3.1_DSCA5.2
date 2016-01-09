/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DVB_TABLES_H_
#define __DVB_TABLES_H_

/*!
 defines dvb filter transfter buffer size.
 */
typedef union tag_dvb_tables
{
 /*!
  pat
  */
  pat_eli_t     m_pat;
 /*!
  cat 
  */
  cat_eli_t     m_cat;
 /*!
  cat cas 
  */
  cat_cas_eli_t m_cat_cas;
 /*!
  pmt 
  */ 
  pmt_eli_t     m_pmt;
 /*!
  bat
  */ 
  bat_eli_t     m_bat;
 /*!
  ecm
  */ 
  ecm_eli_t     m_ecm;
 /*!
  emm
  */
  emm_eli_t     m_emm;
 /*!
  nit
  */ 
  nit_eli_t     m_nit;
 /*!
  sdt
  */
  sdt_eli_t     m_sdt;
 /*!
  tot
  */ 
  tot_eli_t     m_tot;
 /*!
  video packet
  */ 
  video_packet_eli_t  m_video_pkt;
}dvb_tables_t;

#endif  //End for __DVB_TABLES_H_
