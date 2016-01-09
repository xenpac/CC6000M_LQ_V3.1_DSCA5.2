/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __EPG_FILTER_INTRA_H_
#define __EPG_FILTER_INTRA_H_

/*!
 epg filter private data.
 */
typedef struct tag_epg_filter_priv
{
/*!
 epg filter handle
 */  
 void *p_this;
/*!
 epg pin member.
 */
 epg_pin_t m_pin;
}epg_filter_priv_t;


/*!
 epg filter 
 */
typedef struct tag_epg_filter
{
/*!
 base class
 */  
 FATHER sink_filter_t m_sink_filter;
/*!
 epg fitler private data. 
 */
 epg_filter_priv_t    m_priv_data;
}epg_filter_t;


#endif//End for __EPG_FILTER_INTRA_H_
