/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"

//driver
#include "hal_misc.h"
#include "drv_dev.h"
#include "dmx.h"

//util
#include "class_factory.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "input_pin.h"
#include "output_pin.h"
#include "source_pin.h"
#include "ifilter.h"
#include "chain.h"
#include "demux_filter.h"
#include "demux_interface.h"
#include "demux_pin_intra.h"
#include "err_check_def.h"

/*!
  defines pes invalid pid
  */
#define PES_INVALID_PID      0xFFFF

/*!
  the pes pin report structure.
  */
typedef struct tag_pes_pin_report
{
  u32 no_ts_pkt;
  u32 trans_err_ind;
  u32 not_first_valid;
  u32 trans_scramb;
  u32 no_playload;
  u32 continu_count_err;
  u32 pes_packet_num_1;
  u32 data_length_err_1;
  u32 data_length_err_2;
  u32 pes_buf_overflow;
  u32 adfd_len_err;
  u32 pes_packet_num_2;
  //u32 ts_packet_num;
  //u32 ts_ticks;
}pes_pin_report_t;

/*!
  the pes pin private data define
  */
typedef struct tag_pes_pin_private
{
 /*!
  dmx driver's handle
  */
  dmx_device_t      *p_dmx_dev;
 /*!
  dmx buffer pointer
  */
  u8                *p_dmx_buf;
 /*!
  ts packet pointer
  */
  u8                *p_ts_pkt;
 /*!
  pes buffer pointer
  */
  u8                *p_pes_buf;
 /*!
  pes copy size
  */
  u32               pes_copy_size;
 /*!
  ts continuity counter
  */
  u16               ts_cty_count;
 /*!
  pes pid
  */
  u16               pes_pid;
 /*!
  dmx channel id
  */
  dmx_chanid_t      chan_id;
 /*!
  mem alloc interface
  */
  imem_allocator_t *p_alloc;
 /*!
  pes pin report
  */
  pes_pin_report_t report;
  /*!
  pes pin report
  */
  u16 dmx_in;
}pes_pin_private_t;

/*!
  pes pin get private
  */
static pes_pin_private_t *pes_pin_get_priv(handle_t _this)
{
  CHECK_FAIL_RET_NULL(NULL != _this);
  return (pes_pin_private_t *)(((demux_pin_t *)_this)->p_child_priv);
}

/*!
  psi pin alloc

  \param[in] _this psi pin handle
  \param[in] p_chart current chart pointer
  */
static void _pes_pin_alloc(pes_pin_private_t *p_priv, u16 pid)
{
  dmx_device_t        *p_dmx_dev  = p_priv->p_dmx_dev;
  dmx_slot_setting_t   slot_set   = { 0 };  
  dmx_filter_setting_t filter_set = { 0 };
  RET_CODE             ret        = ERR_FAILURE;
  u16                  index      = 0;
  u8                  *p_data     = NULL;
  #ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
  #else
  chip_ic_t chip_ic_id = IC_MAGIC;
  #endif

  CHECK_FAIL_RET_VOID(p_priv    != NULL);
  CHECK_FAIL_RET_VOID(p_dmx_dev != NULL);

  for(index = 0; index < PES_TS_PACKET_NUMBER; index++)
  {
    p_priv->p_dmx_buf[index * 188] = 0;
  }

  p_priv->pes_pid = pid;

  //open channel
  slot_set.pid  = pid;
  slot_set.in   = p_priv->dmx_in;
  slot_set.type = DMX_CH_TYPE_TSPKT;
  if(chip_ic_id == IC_CONCERTO)
  {
    slot_set.muldisp = DMX_SLOT_MUL_SEC_DIS_DISABLE;//for bug 63129
  }
      
  ret = dmx_si_chan_open(p_priv->p_dmx_dev, &slot_set, &p_priv->chan_id);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  CHECK_FAIL_RET_VOID(p_priv->chan_id != DMX_INVALID_CHANNEL_ID);

  //set buffer.
  CHECK_FAIL_RET_VOID(p_priv->p_dmx_buf != NULL);
  p_data = (u8 *)(((u32)p_priv->p_dmx_buf + 3) & (~3)); //align 4
  
  ret = dmx_si_chan_set_buffer(p_priv->p_dmx_dev, p_priv->chan_id, p_data, PES_TS_PACKET_BUF_SIZE);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  //set filter.
  filter_set.continuous = TRUE;
  filter_set.ts_packet_mode = DMX_EIGHT_MODE;
  ret = dmx_si_chan_set_filter(p_priv->p_dmx_dev, p_priv->chan_id, &filter_set);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  //start channel
  ret = dmx_chan_start(p_priv->p_dmx_dev, p_priv->chan_id);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
}

/*!
  psi psi free

  \param[in] _this psi pin handle
  \param[in] p_chart current chart pointer
  */
static void _pes_pin_free(pes_pin_private_t *p_priv)
{
  dmx_device_t *p_dev = p_priv->p_dmx_dev;
  RET_CODE      ret   = ERR_FAILURE;

  CHECK_FAIL_RET_VOID(p_priv != NULL);

  if(p_priv->chan_id == DMX_INVALID_CHANNEL_ID)
    return;

  ret = dmx_chan_stop(p_dev, p_priv->chan_id);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);
  
  ret = dmx_chan_close(p_dev, p_priv->chan_id);
  CHECK_FAIL_RET_VOID(ret == SUCCESS);

  p_priv->pes_pid = PES_INVALID_PID;
  p_priv->chan_id = DMX_INVALID_CHANNEL_ID;
  p_priv->p_ts_pkt = NULL;
}

/*!
  psi pin request table

  \param[in] _this psi pin handle
  \param[in] p_req pointer psi pin request parameter
  */
static RET_CODE pes_pin_request(handle_t _this, void *p_data)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);
  pes_request_data_t *p_req = (pes_request_data_t *)p_data;

  CHECK_FAIL_RET_CODE(p_req != NULL);
  CHECK_FAIL_RET_CODE(p_req->pid != PES_INVALID_PID);

  //if the same pid, directly return it.
  if(p_priv->pes_pid == p_req->pid)
  {
    return SUCCESS;
  }

  _pes_pin_free(p_priv);

  _pes_pin_alloc(p_priv, p_req->pid);

  p_priv->p_ts_pkt = p_priv->p_dmx_buf;
  memset(&p_priv->report, 0x0, sizeof(pes_pin_report_t));

  return SUCCESS;
}

static RET_CODE pes_pin_free(handle_t _this, void *p_data)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);

  _pes_pin_free(p_priv);

  return SUCCESS;
}

static BOOL is_new_chip(void)
{
#ifndef WIN32
  chip_ic_t chip_ic_id = hal_get_chip_ic_id();
#else
  chip_ic_t chip_ic_id = IC_MAGIC;
#endif
  return ((chip_ic_id != IC_MAGIC) && (chip_ic_id != IC_WIZARDS));
}

static RET_CODE pes_pin_on_open(handle_t _this)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);

  if(p_priv->p_dmx_buf == NULL)
  {
    p_priv->p_dmx_buf = (u8 *)mtos_malloc(PES_TS_PACKET_BUF_SIZE);
    CHECK_FAIL_RET_CODE(p_priv->p_dmx_buf != NULL);
    memset(p_priv->p_dmx_buf, 0x0, PES_TS_PACKET_BUF_SIZE);
    if(is_new_chip())
      p_priv->p_dmx_buf = (u8 *)(((u32)p_priv->p_dmx_buf) | 0xa0000000);
  }

  if(p_priv->p_pes_buf == NULL)
  {
    p_priv->p_pes_buf = (u8 *)mtos_malloc(PES_TS_PACKET_BUF_SIZE);
    CHECK_FAIL_RET_CODE(p_priv->p_pes_buf != NULL);
    memset(p_priv->p_pes_buf, 0x0, PES_TS_PACKET_BUF_SIZE);
  }

  p_priv->pes_copy_size = 0;
  p_priv->ts_cty_count  = 0;
  p_priv->pes_pid       = PES_INVALID_PID;
  p_priv->chan_id       = DMX_INVALID_CHANNEL_ID;

  return SUCCESS;
}

static RET_CODE pes_pin_on_close(handle_t _this)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);

  _pes_pin_free(p_priv);

  if(p_priv->p_dmx_buf != NULL)
  {
    if(is_new_chip())
      p_priv->p_dmx_buf = (u8 *)(((u32)p_priv->p_dmx_buf) & 0x8FFFFFFF);
    mtos_free(p_priv->p_dmx_buf);
    p_priv->p_dmx_buf = NULL;
  }

  if(p_priv->p_pes_buf != NULL)
  {
    mtos_free(p_priv->p_pes_buf);
    p_priv->p_pes_buf = NULL;
  }

  return SUCCESS;
}

static void pes_pin_on_destroy(handle_t _this)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);

  if(p_priv != NULL)
  {
    mtos_free((void *)p_priv);
  }
}

static u8 *pes_pin_next_packet(u8 *p_ts_pkt, u8 *p_dmx_pkt)
{
  if(p_ts_pkt == (p_dmx_pkt + (PES_TS_PACKET_BUF_SIZE - 188)))
    p_ts_pkt = p_dmx_pkt;
  else
    p_ts_pkt +=188;

  return p_ts_pkt;
}


static RET_CODE pes_pin_on_fill_sample(handle_t _this, media_sample_t *p_sample,
                                     u32 fill_len, s64 position)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);
  u8 adfd_ctrl = 0;  /* adaption_field_ctrl */
  u8 adfd_len = 0;   /* adaption_field_length */
  u8 cty_count = 0;  /* continuity_counter */
  pes_pin_report_t *p_report = &p_priv->report;
  u32 pes_data_length = 0;

  //decide channel id
  if(p_priv->chan_id == DMX_INVALID_CHANNEL_ID)
  {
   // OS_PRINTF("return in pes line %d\n", __LINE__);
    return ERR_FAILURE;
  }

  if(p_priv->p_ts_pkt == NULL)
  {
    return ERR_FAILURE;
  }

  while(1)
  {
    if(p_priv->p_ts_pkt[0] != 0x47)
    {
      //OS_PRINTF("return err_failure %d  address 0x%x \n", p_priv->p_ts_pkt[0], p_priv->p_ts_pkt);
      p_priv->p_ts_pkt[0] = 0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      p_report->no_ts_pkt++;
      return ERR_FAILURE;
    }
    
    //transport_error_indicator, continue.
    if((p_priv->p_ts_pkt[1] & 0x80) == 0x80)
    {
      p_priv->p_ts_pkt[0] = 0x0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      p_priv->ts_cty_count = 0xff;
      p_priv->pes_copy_size = 0;
      OS_PRINTF("PES PIN : error packet, skip\n");
      p_report->trans_err_ind++;
      continue;
    }

    //payload_unit_start_indicator, 1 stands for first valid packet
    if((p_priv->pes_copy_size == 0) && ((p_priv->p_ts_pkt[1] & 0x40) != 0x40))
    {
      p_priv->p_ts_pkt[0] = 0x0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      p_priv->ts_cty_count = 0xff;
      //OS_PRINTF("PES PIN : not first valid packet\n");
      p_report->not_first_valid++;
      continue;
    }

    /*!
     transport_scrambling_control :
         0 : stands for not scramble.
     notes : transport packet must't scrambling.
     */
    if((p_priv->p_ts_pkt[3] & 0xc0) != 0)
    {
      p_priv->p_ts_pkt[0] = 0x0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      //p_priv->ts_cty_count = 0xff;
      //p_priv->pes_copy_size = 0;
      //OS_PRINTF("PES PIN: scrambling, skip \n");
      p_report->trans_scramb++;
      continue;
    }

    /*!
      adaption_field_ctrl
      00 : reserved
      01 : no adaption field, payload only
      10 : adaption field only, no payload
      11 : adaption filed followed by payload
      */
    adfd_ctrl = (p_priv->p_ts_pkt[3] & 0x30) >> 4;

    if(adfd_ctrl == 0x02 || adfd_ctrl == 0x0) //no payload skip
    {
      p_priv->p_ts_pkt[0] = 0x0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      //OS_PRINTF("PES PIN: no payload skip \n");
      p_report->no_playload++;
      continue;
    }
    else if(adfd_ctrl == 0x03)//adaption field followed by payload
    {
      //adaptation_field_length + sizeof(adaptation_field_length)
      //sizeof(adaptation_field_length = 1)
      adfd_len = p_priv->p_ts_pkt[4] + 1;
      if(adfd_len >= 184)
      {
        p_report->adfd_len_err++;
        //OS_PRINTF("PES PIN!: TS Packet Error #4\n");
        p_priv->p_ts_pkt[0] = 0x0;
        p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
        continue;
      }
    }
    else if(adfd_ctrl == 0x01) //only has payload
    {
      adfd_len = 0;
    }

    /*!
     continuity_counter
     */
    cty_count = p_priv->p_ts_pkt[3] & 0x0f;

    //first ts packet.
    if((p_priv->ts_cty_count == 0xff) || ((p_priv->p_ts_pkt[1] & 0x40) == 0x40))
    {
      p_priv->ts_cty_count = cty_count;
    }
    else
    {
      //To achieve maximum(1111B), rewind 0
      if(p_priv->ts_cty_count == 0x0f)
        p_priv->ts_cty_count = 0;
      else
        p_priv->ts_cty_count ++;

      if(p_priv->ts_cty_count != cty_count)
      {
        //OS_PRINTF("PES PIN!:  continuity counter error.\n");
        p_priv->ts_cty_count = 0xff;
        p_priv->pes_copy_size = 0;

        p_priv->p_ts_pkt[0] = 0x0;
        p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
        p_report->continu_count_err++;
        continue;
      }
    }

    //next first start begin, check whether old pes packet is ready, and transfer to down filter.
    if(((p_priv->p_ts_pkt[1] & 0x40) == 0x40) && (p_priv->pes_copy_size != 0))
    {
     /*!
      pes_copy_size must bigger than
      (packet_start_code_prefix + stream_id + pes_packet_length = 6),
      or will have wild pointer, very dangerous.
      */
      if(p_priv->pes_copy_size > 6)
      {
        pes_data_length = (((u16)p_priv->p_pes_buf[4] << 8)
                                & 0xff00) + p_priv->p_pes_buf[5] + 6;

        if(p_priv->pes_copy_size >= pes_data_length)
        {
          //intend transfer old pes packet to down filter.
          p_sample->p_data  = p_priv->p_pes_buf;
          p_sample->payload = pes_data_length;
          p_sample->context = p_priv->pes_pid;
          p_sample->state = SAMP_STATE_GOOD;

          p_priv->pes_copy_size = 0;
          p_priv->ts_cty_count = 0xff;

          //p_priv->p_ts_pkt[0] = 0x0;
          //p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
          //OS_PRINTF("ticks  %d  func %s\n", mtos_ticks_get(), __FUNCTION__);
          p_report->pes_packet_num_1++;
          return SUCCESS;
        }
        else
        {
          //OS_PRINTF("PES PIN!:    data length error #1 [%d - %d]\n",
           // pes_data_length, p_priv->pes_copy_size);
          p_priv->pes_copy_size = 0;
          p_priv->ts_cty_count  = 0xff;
          p_report->data_length_err_1++;
          continue;
        }
      }
      else
      {
        p_report->data_length_err_2++;
        //OS_PRINTF("PES PIN!:    data length error #2 [%d]\n", p_priv->pes_copy_size);

        p_priv->pes_copy_size = 0;
        p_priv->ts_cty_count   = 0xff;
        continue;
      }
    }

    //184 = 188 - 4(ts packet header)
    //pes buffer overflow
    if((p_priv->pes_copy_size + 184 - adfd_len) > PES_TS_PACKET_BUF_SIZE)
    {
      //OS_PRINTF("PES PIN!: PES buffer overflow\n");
      p_priv->ts_cty_count = 0xff;
      p_priv->pes_copy_size = 0;

      p_priv->p_ts_pkt[0] = 0x0;
      p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
      p_report->pes_buf_overflow++;
      continue;
    }

    if(adfd_len < 184)
    {
      //temp solution, close irq.
      //mtos_critical_enter(&sr);
      memcpy(p_priv->p_pes_buf + p_priv->pes_copy_size
            , p_priv->p_ts_pkt + 4 + adfd_len
            , 184 - adfd_len);
      //mtos_critical_exit(sr);

      p_priv->pes_copy_size += 184 - adfd_len;
    }

    if(p_priv->pes_copy_size > 6)
    {
      pes_data_length = (((u16)p_priv->p_pes_buf[4] << 8)
                        & 0xff00) + p_priv->p_pes_buf[5] + 6;

      if(p_priv->pes_copy_size >= pes_data_length)
      {
        //intend transfer old pes packet to down filter.
        p_sample->p_data = p_priv->p_pes_buf;
        p_sample->payload = pes_data_length;
        p_sample->context = p_priv->pes_pid;
        p_sample->state = SAMP_STATE_GOOD;

        p_priv->pes_copy_size = 0;
        p_priv->ts_cty_count = 0xff;

        p_priv->p_ts_pkt[0] = 0x0;
        p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
        //    OS_PRINTF("ticks  %d  func %s\n", mtos_ticks_get(), __FUNCTION__);
        p_report->pes_packet_num_2++;
        return SUCCESS;
      }
    }

    p_priv->p_ts_pkt[0] = 0x0;
    p_priv->p_ts_pkt = pes_pin_next_packet(p_priv->p_ts_pkt, p_priv->p_dmx_buf);
  }

  //return ERR_FAILURE;
}

static void pes_pin_report(handle_t _this, void *p_state)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);
  pes_pin_report_t *p_report = NULL;
  
  p_report = &p_priv->report;
  OS_PRINTF("[PES_PIN]:\n\tno_ts_pkt %d, trans_err_ind %d, not_first_valid %d\n"\
            "\ttrans_scramb %d, no_playload %d, continu_count_error %d\n"\
            "\tpes_packet_num_1 %d, data_length_err_1 %d, data_length_err_2 %d\n"\
            "\tpes_buf_overflow %d, adfd_len_err %d, pes_packet_num_2 %d\n",  \
            p_report->no_ts_pkt, p_report->trans_err_ind, p_report->not_first_valid,    \
            p_report->trans_scramb, p_report->no_playload, p_report->continu_count_err,\
            p_report->pes_packet_num_1, p_report->data_length_err_1, p_report->data_length_err_2, \
            p_report->pes_buf_overflow, p_report->adfd_len_err, p_report->pes_packet_num_2);
}

static RET_CODE pes_pin_decide_buffer_size(handle_t _this)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);
  imem_allocator_t *p_alloc = p_priv->p_alloc;
  allocator_properties_t properties = {0};

  //config mem_alloc
  p_alloc->get_properties(p_alloc, &properties);
  properties.buffers = 1;
  properties.buffer_size = PES_TS_PACKET_BUF_SIZE;
  properties.use_virtual_space = TRUE;
  p_alloc->set_properties(p_alloc, &properties, NULL);
  return SUCCESS;
}

static RET_CODE pes_pin_cfg(handle_t _this, void *p_para)
{
  pes_pin_private_t *p_priv = pes_pin_get_priv(_this);

  p_priv->dmx_in = (u32)p_para;
  return SUCCESS;
}

demux_pin_t * attach_pes_pin_instance(handle_t _this)
{
  pes_pin_private_t *p_priv = NULL;
  demux_pin_t *p_pes_pin = (demux_pin_t *)_this;
  base_output_pin_t *p_output_pin = (base_output_pin_t *)_this;
  ipin_t *p_ipin = (ipin_t *)_this;
  interface_t *p_interface = (interface_t *)_this;
  media_format_t media_format = {MT_PES};

  //check input parameter
  CHECK_FAIL_RET_NULL(_this != NULL);

  //create base class
  p_interface->_rename(p_interface, "pes_pin");
  p_interface->report = pes_pin_report;

  //init private date
  p_pes_pin->p_child_priv = mtos_malloc(sizeof(pes_pin_private_t));
  CHECK_FAIL_RET_NULL(p_pes_pin->p_child_priv != NULL);
  p_priv = (pes_pin_private_t *)(p_pes_pin->p_child_priv);
  memset(p_priv, 0x0, sizeof(pes_pin_private_t));

  p_priv->p_dmx_buf  = NULL;
  p_priv->p_ts_pkt   = NULL;
  p_priv->p_pes_buf  = NULL;
  p_priv->pes_pid    = PES_INVALID_PID;
  p_priv->chan_id    = DMX_INVALID_CHANNEL_ID;

  p_priv->p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  CHECK_FAIL_RET_NULL(p_priv->p_dmx_dev != NULL);
  p_ipin->get_interface(p_ipin, IMEM_ALLOC_INTERFACE, (void **)&p_priv->p_alloc);
  p_ipin->add_supported_media_format(p_ipin, &media_format);

  //init member function
  p_pes_pin->request = pes_pin_request;
  p_pes_pin->free    = pes_pin_free;
  p_pes_pin->config  = pes_pin_cfg;
  p_pes_pin->reset_pid = NULL;
  p_pes_pin->pause_resume = NULL;

  //over load virtual function
  p_output_pin->decide_buffer_size = pes_pin_decide_buffer_size;
  p_output_pin->on_fill_sample     = pes_pin_on_fill_sample;

  p_ipin->on_open  = pes_pin_on_open;
  p_ipin->on_close = pes_pin_on_close;

  p_interface->on_destroy = pes_pin_on_destroy;
  
  return p_pes_pin;
}


