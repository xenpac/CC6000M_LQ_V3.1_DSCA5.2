/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __VIDEO_PACKET_TABLE_H_
#define __VIDEO_PACKET_TABLE_H_

/*!
  \file video_packet.h
  
  Video_packet.h provides the API about how to capture one video ts packet in 
  order to check the scramble state of certain program
  
  Development policy:
  This module should work together with DVB module
  */

/*!
  defiens video packet eli structure.
  */
typedef struct tag_video_packet_eli
{
 /*!
  pid 
  */
  u16 pid;
 /*!
  sid
  */
  u16 sid;
 /*!
  is scramble.
  */
  BOOL is_scramble;
}video_packet_eli_t;

/*!
  parse pat table process
  
  \param[in] p_param : dvb prase parameter pointer.
  \param[in] p_input : dmx program special information.
  \param[out] p_output : dvb prased data.
  */
RET_CODE video_packet_parse(dvb_parse_param_t *p_param, u8 *p_input, u8 * p_output);

/*!
  video packet request param

  \param[in] p_param : request static parameters
  */
void video_packet_request(dvb_request_param_t *p_param);


#endif // End for __VIDEO_PACKET_TABLE_H_

