/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_define.h"
#include "sys_types.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "ts_packet.h"
#include "mem_manager.h"
#include "string.h"
#include "err_check_def.h"

#define TS_SYNC_FLAG_POS  (0)
#define TS_SYNC_FLAG      (0x47)

#define ADPTION_NO_PAYLOAD_FLAG           (0) 
#define ADPTION_FIELD_FLAG_EXISTING_FLAG  (1)


#define SECTION_HEAD_LENGTH         (3)
#define PACKET_STARTING_IN_PAYLOAD  (1)
#define TS_PKT_LEN                  (188)

void create_ts_packet_buf(psi_sec_info_t *p_ts_buf,
u8 partition_id, u16 user_id, u16 section_max_size)
{
  mem_mgr_alloc_param_t alloc_param = { 0 };
  
  /*!
    Allocating TS buffer for saving temporary parsing result
  */
  CHECK_FAIL_RET_VOID(p_ts_buf != NULL);

  p_ts_buf->i_section_max_size = section_max_size;
  p_ts_buf->b_complete_header  = 0;
  p_ts_buf->i_continuity_counter = 0;

  p_ts_buf->i_need           = 0;
  p_ts_buf->b_this_sec_using = FALSE;

  alloc_param.id      = partition_id;
  alloc_param.user_id = user_id;
  alloc_param.size    = section_max_size;

  p_ts_buf->p_cur_sec_buf    = MEM_ALLOC(&alloc_param);

  if(p_ts_buf->p_cur_sec_buf != NULL)
  {
    memset(p_ts_buf->p_cur_sec_buf, 0, p_ts_buf->i_section_max_size);
  }

}

static void packet_cpy(void * p_dst_data, void *p_src_data, u32 len)
{
  memcpy(p_dst_data, p_src_data, len);
}

/*!
  Parse ts packet
 */
void ts_packet_to_sec(psi_sec_info_t *p_dvbpsi,u8 *p_data)
{
  u8 i_expected_counter = 0; 
  u8 *p_payload_pos = NULL; 
  
  /*!
    Beginning of the new section,updated 
    to NULL when the new section is handled 
   */
  u8  *p_new_pos = NULL; 
  s32 i_available = 0; 
  
  /*!
   Do not check  TS start code 
   */
  if(p_data[TS_SYNC_FLAG_POS] != TS_SYNC_FLAG) 
  {
    //epg_dbg(("PSI decoder not a TS packet!\n"));
    return;
  }

  if(MASK_FIFTH_BIT_OF_BYTE(p_data[3]) == ADPTION_NO_PAYLOAD_FLAG) 
  {
    //no valid payload!!
   // epg_dbg(("PSI decoder not valid payload!\n"));
    return;
  }
  
  /*!This is a valid ts packet!*/ 
  /*now we have a section to fill data!*/
  i_expected_counter = 
                  SYS_GET_LOW_HALF_BYTE(p_dvbpsi->i_continuity_counter + 1); 
  p_dvbpsi->i_continuity_counter = SYS_GET_LOW_HALF_BYTE(p_data[3]); 

  /*!
    This may not right when the first packet has been captured
   */
  if(i_expected_counter != p_dvbpsi->i_continuity_counter) 
  {
    //epg_dbg(("TS discontinuity (received %d, expected %d)",
         //   p_dvbpsi->i_continuity_counter, i_expected_counter));
    p_dvbpsi->b_this_sec_using = FALSE;
  }
  
  //(adaption field)
  if(MASK_SIXTH_BIT_OF_BYTE(p_data[3])== ADPTION_FIELD_FLAG_EXISTING_FLAG) 
  {
    p_payload_pos = p_data + 5 + p_data[4]; 
  }
  else
  {
    p_payload_pos = p_data + 4;
  }
  
  /* Unit start -> skip the pointer_field and a new section begins */
  //payload unit start indicator bit
  if(MASK_SEVENTH_BIT_OF_BYTE(p_data[1]) == PACKET_STARTING_IN_PAYLOAD)
  {
    p_new_pos = p_payload_pos + *p_payload_pos + 1; 
    p_payload_pos += 1; 
  }

  if(p_dvbpsi->b_this_sec_using == FALSE)
  {
    if(p_new_pos)
    {
      p_dvbpsi->b_this_sec_using = TRUE;
      p_dvbpsi->p_payload_pos = p_dvbpsi->p_cur_sec_buf;
      p_payload_pos = p_new_pos;
      p_new_pos = NULL;
      /*!
       Just need the header to know how long is the section
       */
      /*!
        At least 3bytes are needed*/
      p_dvbpsi->i_need = SECTION_HEAD_LENGTH; 
      p_dvbpsi->b_complete_header = 0;
    }
    else
    {
      /* No new section => return */
      //epg_dbg(("PSI decoder no valid section!\n"));
      return;
    }
  }

  /* Remaining bytes in the payload */
  i_available = TS_PKT_LEN + p_data - p_payload_pos;
  while(i_available > 0)
  {
    if(i_available >= p_dvbpsi->i_need)
    {
      packet_cpy(p_dvbpsi->p_payload_pos, p_payload_pos, p_dvbpsi->i_need);
      //memcpy(p_dvbpsi->p_payload_pos, p_payload_pos, p_dvbpsi->i_need);
      p_payload_pos += p_dvbpsi->i_need;
      p_dvbpsi->p_payload_pos += p_dvbpsi->i_need;
      i_available -= p_dvbpsi->i_need;

      if(!p_dvbpsi->b_complete_header)
      {
        /* Header is complete */
        p_dvbpsi->b_complete_header = 1;
        /* Compute p_section->i_length and update p_dvbpsi->i_need */
        p_dvbpsi->i_need = 
        MAKE_WORD(p_dvbpsi->p_cur_sec_buf[2], 
        (SYS_GET_LOW_HALF_BYTE(p_dvbpsi->p_cur_sec_buf[1])));
        if(p_dvbpsi->i_need > 
                    (p_dvbpsi->i_section_max_size - SECTION_HEAD_LENGTH))
        {
          //epg_dbg(("PSI section too long=%d!!\n",p_dvbpsi->i_need));
          p_dvbpsi->b_this_sec_using = FALSE;
          if(p_new_pos)
          {
            p_dvbpsi->b_this_sec_using = TRUE;
            p_dvbpsi->p_payload_pos = p_dvbpsi->p_cur_sec_buf;
            p_payload_pos = p_new_pos;
            p_new_pos = NULL;
            p_dvbpsi->i_need = SECTION_HEAD_LENGTH;
            p_dvbpsi->b_complete_header = 0;
            i_available = TS_PKT_LEN + p_data - p_payload_pos;
          }
          else
          {
            i_available = 0;
          }
        }
      }
      else
      {      
        p_dvbpsi->b_this_sec_using = FALSE;
        /* If there is a new section not being handled then go forward
        in the packet */
        if(p_new_pos)
        {
          p_dvbpsi->b_this_sec_using = TRUE;
          p_dvbpsi->p_payload_pos = p_dvbpsi->p_cur_sec_buf;
          p_payload_pos = p_new_pos;
          p_new_pos = NULL;
          p_dvbpsi->i_need = SECTION_HEAD_LENGTH;
          p_dvbpsi->b_complete_header = 0;
          i_available = TS_PKT_LEN + p_data - p_payload_pos;
        }
        else
        {
          i_available = 0;
        }
      }
    }
    else
    {
      /* There aren't enough bytes in this packet to complete the
      header/section */
      packet_cpy(p_dvbpsi->p_payload_pos, p_payload_pos, i_available);
      //memcpy(p_dvbpsi->p_payload_pos, p_payload_pos, i_available);
      p_dvbpsi->p_payload_pos += i_available;
      p_dvbpsi->i_need -= i_available;
      i_available = 0;
    }
  }
  
}

void release_ts_packet_buf(psi_sec_info_t *p_ts_buf,
  u8 partition_id, u16 user_id)
{
  mem_mgr_free_param_t free_param = {0};

  CHECK_FAIL_RET_VOID(p_ts_buf  != NULL);

  free_param.id      = partition_id;
  free_param.p_addr  = p_ts_buf->p_cur_sec_buf;
  free_param.user_id = user_id;
  FREE_MEM(&free_param);

  p_ts_buf->p_cur_sec_buf        = NULL;
  p_ts_buf->i_section_max_size   = 0;

  p_ts_buf->b_complete_header    = 0;
  p_ts_buf->i_continuity_counter = 0;

  p_ts_buf->i_need           = 0;
  p_ts_buf->b_this_sec_using = FALSE;

}

#ifdef __cplusplus
}
#endif // End for __cplusplus

