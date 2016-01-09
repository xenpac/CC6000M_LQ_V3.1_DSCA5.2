/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DMX_INTERFACE_H_
#define __DMX_INTERFACE_H_


/*!
  defines interface name macro
  */
#define DMX_INTERFACE_NAME    "dmx_i"

/*!
  demux interface
  */
typedef struct tag_dmx_interface
{
  /*!
    point to the owner
    */
  PRIVATE interface_t *p_owner;

  /*!
    request table

    \param[in] _this dmx pin handle
    \param[in] p_req pointer request info 
    */
  RET_CODE (*i_request)(handle_t _this, void *p_req);

  /*!
    free table

    \param[in] _this dmx pin handle
    \param[in] p_free pointer free info 
    */
  RET_CODE (*i_free)(handle_t _this, void *p_free);
}dmx_interface_t;


#endif // End for __DMX_INTERFACE_H_

