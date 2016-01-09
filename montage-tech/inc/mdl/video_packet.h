/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __VIDEO_PACKET_H_
#define __VIDEO_PACKET_H_

/*!
  \file video_packet.h
  
  Video_packet.h provides the API about how to capture one video ts packet in 
  order to check the scramble state of certain program
  
  Development policy:
  This module should work together with DVB module
  */

/*!
  Parse video packet to check the scramble flag 
  \param[in] handle Service handle
  \param[in] p_sec: ts packet section from PTI driver
  */
void parse_ts_packet(handle_t handle, dvb_section_t *p_sec);

/*!
  Request video ts packet
  \param[in] p_sec section information to be requested
  \param[in] para1 table id for request operation
  \param[in] para2 parameter for request operation
  */
void request_ts_packet(dvb_section_t *p_sec, u32 para1, u32 para2);

#endif // End for __VIDEO_PACKET_H_

