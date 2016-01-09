/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>
#include "stdarg.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#include  "sys_types.h"
#include  "sys_define.h"
#include "lib_util.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "mem_manager.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"

#include "class_factory.h"

#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"

#include "drv_dev.h"
#include "charsto.h"
#include "fcrc.h"
//#include "lzma.h"
#include "lzmaif.h"

#include "dmx.h"

#include "mdl.h"
#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
//eva
#include "media_format_define.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"


//filter
#include "eva_filter_factory.h"
#include "demux_filter.h"

#include "dvb_protocol.h"
#include "ota_public_def.h"
#include "ota_filter.h"
#include "ota_filter_dsmcc.h"
#include "ota_filter_dsmcc_intra.h"


/*!
  dsmcc ddm head block number
  */
#define OTA_DDM_HEAD_MAX_BLOCK_NUMBER 16

/*!
  dsmcc ddm head mark
  */
#define UPG_DDM_HEAD_MARK     0x5AA55AA5

/*!
  defines ota filter  burn file type. 
  */
typedef enum tag_ota_file_type
{
  /*!
  unkown 
  */
  OTA_FILE_TYPE_UNKOWN = 0,
/*!
  only block 
  */
  OTA_FILE_TYPE_ONLY_BLOCK,
/*!
  main code 
  */
  OTA_FILE_TYPE_MAIN_CODE,
/*!
  all flash code 
  */
  OTA_FILE_TYPE_ALL_CODE,
  /*!
  all flash code by factory
  */
  OTA_FILE_TYPE_ALL_CODE_FACTORY,
}ota_file_type_t;

/*!
  defines ota filter  burn file type. 
  */
typedef enum tag_ota_upg_mode
{
  /*!
  unkown 
  */
  OTA_UPG_MODE_ONLY_BLOCK = 0,
/*!
  only block 
  */
  OTA_UPG_MODE_BLOCK_IN_FILE,
/*!
  main code 
  */
  OTA_UPG_MODE_ALL_BLOCK_IN_FILE,
}ota_upg_mode_t;

/*!
 ota Block information of data 
 */
typedef struct tag_ddm_data_info
{
  
 /*!
    ota upg block  
   */
  u8 ota_block_id;/**maincode and all flash block is 0xFF***/
  /*!
    ota file type
    */
  ota_file_type_t file_type;
   /*!
    upg mode
    */
   ota_upg_mode_t upg_mode; /***work on file type is all flash or main code file type***/
/*!
    ota new version
    */
  u16 ota_new_ver;
  /*!
    ota block position
    */
  u32 block_offset;
  /*!
    CRC byte
    */
  u32 file_crc;
  /*!
    file size
    */
  u32 file_size;
}ddm_block_info_t;

typedef struct tag_ddm_info
{
  /*!
    the header mark,0x5aa55aa5
    */
  u32 mark;
  /*!
    the head info size
    */
  u32 header_info_size;
  /*!
    the head info size
    */
  u32 header_info_crc;
  /*!
    the head reserve
    */
  u32 reserve1;
  /*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_db_data_clear;
/*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_sys_data_clear;
/*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_upg_by_stbid;
   /*!
    the ca data clear flag,TRUE:clease
    */
  BOOL g_ca_data_clear;
/*!
    the ads data clear flag,TRUE:clease
    */
  BOOL g_ads_data_clear;
  /*!
    the head reserve
    */
  u32 reserve4;
  /*!
    the user data clear flag,TRUE:clease
    */
  u8 stbid_start[128];
  /*!
    the user data clear flag,TRUE:clease
    */
  u8 stbid_end[128];
  

  /*!
    the block number
    */
   u8 block_number ;
  /*!
    the all or main code break up to block
    only main code type or all flash type block  work on OTA_UPG_MODE_BLOCK_IN_FILE mode
    */
  tag_ota_cut_t break_block;
  /*!
    the all or main code break up to block
    main code or all flash type work on OTA_UPG_MODE_ALL_BLOCK_IN_FILE MODE
    */
  tag_ota_jump_t jump_block;
  /*!
    bloock info
    */
   ddm_block_info_t upg_block_info_table[OTA_DDM_HEAD_MAX_BLOCK_NUMBER];
}ddm_data_head_info_t;




 /*!
   dsmcc get private data handle
   */
dsmcc_priv_t *dsmcc_get_priv(void)
{
   return (dsmcc_priv_t *)class_get_handle_by_id(OTA_DSMCC_CLASS_ID);
}

/*!
  ota filter debug printf porting,the function run by project (intra).
  */
void ota_dsmcc_debug_printf(ota_debug_level_t level,const char *p_fmt, ...)
{

  dsmcc_priv_t *p_priv =  NULL;
  char buffer[MAX_PRINT_BUFFER_LEN] = {0};
  p_print_list p_args = NULL;

  p_priv = dsmcc_get_priv();

  if(p_priv->debug_level < OTA_DEBUG_ALL)
  {
    return;
  }
  if(level >= p_priv->debug_level)
  {

     if(p_priv->ota_dsmcc_debug_printf != NULL)
      {
        PRINTF_VA_START(p_args, p_fmt);
        ck_vsnprintf(buffer,sizeof(buffer), p_fmt, p_args);
        PRINTF_VA_END(p_args);
        p_priv->ota_dsmcc_debug_printf("%s", buffer);
      }

  }

  return;
}


 /*!
   parse dsi group info
   */
static void parse_dsi_group_info_v2(dsmcc_priv_t *p_priv, u8 *p_gi)
{
  u16 i = 0;
  u16 j = 0;
  u16 k = 0;
  u16 compa_desc_len = 0;
  u16 descript_cnt = 0;
  u16 len  = 0;
  u16 descript_type = 0;
  u32 step = 0;
  u8 *p_tem = NULL;
  u8 *p_priv_tem = NULL;
  dsmcc_dsi_info_t dsi_info;

  memset(&dsi_info,0,sizeof(dsmcc_dsi_info_t));
  
  //dsi_info.group_num = MAKE_WORD2(p_gi[0], p_gi[1]);
  dsi_info.is_force = p_gi[0];
  dsi_info.group_num = p_gi[1];  
   
  step = 2;

  for(i = 0; i < dsi_info.group_num; i++)
  {
    dsi_info.group_info[i].group_id = make32(&p_gi[step]);

    dsi_info.group_info[i].file_total_size = make32(&p_gi[step + 4]);

    compa_desc_len = MAKE_WORD2(p_gi[step + 8], p_gi[step + 9]);

    descript_cnt = MAKE_WORD2(p_gi[step + 10], p_gi[step + 11]);

    step += 12;
    len = 0;
    for(j = 0; j < descript_cnt; j++)
    {
       //descrip_len = step;
       p_tem = &p_gi[step];
       descript_type = p_tem[len];
        if(SYS_HARDWARE_DESC == descript_type)
        {
            MT_ASSERT(p_tem[len + 1] == 0x9);
            MT_ASSERT(p_tem[len + 2] == 1);
            
            dsi_info.group_info[i].oui =
              p_tem[len + 3] << 16 | p_tem[len + 4] << 8 | p_tem[len + 5];
            dsi_info.group_info[i].hw_mod_id = MAKE_WORD2(p_tem[len + 6], p_tem[len + 7]);
            dsi_info.group_info[i].hw_version = MAKE_WORD2(p_tem[len + 8], p_tem[len + 9]);
            //len  += 11;

        }
        else if(SYS_SOFTWARE_DESC == descript_type)
        {
            MT_ASSERT(p_tem[len + 1] == 0x9);
            MT_ASSERT(p_tem[len + 2] == 1);
            dsi_info.group_info[i].oui =
              p_tem[len + 3] << 16 | p_tem[len + 4] << 8 | p_tem[len + 5];
            dsi_info.group_info[i].sw_mod_id = MAKE_WORD2(p_tem[len + 6], p_tem[len + 7]);
            dsi_info.group_info[i].sw_version = MAKE_WORD2(p_tem[len + 8], p_tem[len + 9]);
            //len  += 11;
        }
        else if(SYS_SOFTWARE_FORMAT_DESC == descript_type)
        {
            MT_ASSERT(p_tem[len + 1] == 0x9);
            MT_ASSERT(p_tem[len + 2] == 1);
            dsi_info.group_info[i].sw_block_id = p_tem[len + 3];
            dsi_info.group_info[i].sw_is_zipped = p_tem[len + 4];
            dsi_info.group_info[i].sw_is_spilt = p_tem[len + 5];
            //len  += 6;
        }
        len  += 11;
        MT_ASSERT(p_tem[len - 1] == 0);
    }
    step += len + 4;//4bytes gap between groups
/*********get grop priv data info**************/
     p_priv_tem = &p_gi[step];
     k = 0;
     if((p_priv_tem[k] == 0x00) && (p_priv_tem[k + 1] == 0x0b))
      {
         dsi_info.group_info[i].m_info_size =  p_priv_tem[k + 2] << 8 | p_priv_tem[k + 3];
         if((dsi_info.group_info[i].m_info_size > 0)
           // && (dsi_info.group_info[i].m_info_size <= sizeof(ddm_data_head_info_t))
             && (dsi_info.group_info[i].p_info_buffer == NULL))
          {
            dsi_info.group_info[i].p_info_buffer = mtos_malloc(dsi_info.group_info[i].m_info_size);
            memset(dsi_info.group_info[i].p_info_buffer,0,dsi_info.group_info[i].m_info_size);
            memcpy(dsi_info.group_info[i].p_info_buffer,
                          &p_priv_tem[k + 4],
                          dsi_info.group_info[i].m_info_size);
          }
         else
          {
             dsi_info.group_info[i].m_info_size  = 0;
          }
         step += (dsi_info.group_info[i].m_info_size + 4);
      }
/******************************/
  }
  memcpy(&p_priv->dsi_info,&dsi_info,sizeof(dsmcc_dsi_info_t));
}

 /*!
   parse dsi info
   */
static void parse_dsmcc_dsi_info_v2(dsmcc_priv_t *p_priv,u8 *p_dsi, u16 len)
{
  u16 priv_data_len = 0;

  MT_ASSERT(p_dsi[20] == 0);
  MT_ASSERT(p_dsi[21] == 0);

  priv_data_len = MAKE_WORD2(p_dsi[22], p_dsi[23]);
  MT_ASSERT(priv_data_len + 24 == len);
  if(priv_data_len > 0)
  {
    parse_dsi_group_info_v2(p_priv,&p_dsi[24]);
  }

}
 /*!
 dsmcc parse dii info(intra).
 */
static void parse_dsmcc_dii_info_v2(dsmcc_priv_t *p_priv, u8 *p_dii, u16 len)
{
  u32 download_id = 0;
  u16 block_size = 0;
  u16 num_module = 0;
  u16 i = 0;
  u16 module_cul_len = 0;
  u8 *p_dii_mod = NULL;
  dsmcc_dii_info_t dii_info = {0};
  u8 module_info_len = 0;
  u16 next_module_id = 0;
  u8 position = 0;
  u16 offset = 0;
  u16 step = 0;
  u8 gap = 0;
  u8 *p_tem = NULL;
  u32 num  = 0;

  download_id = make32(&p_dii[0]);

  block_size = MAKE_WORD2(p_dii[4], p_dii[5]);
  num_module = MAKE_WORD2(p_dii[18], p_dii[19]);

  module_cul_len = 0;
  p_dii_mod = &p_dii[20];

  for(i = 0; i < num_module; i++)
  { 
    num = dii_info.dii_type_info[dii_info.type_num].module_num;
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_id = make16(&p_dii[20 + step]);
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_size = make32(&p_dii[22 + step]);
    dii_info.dii_type_info[dii_info.type_num].mod_info[num].mod_ver =  p_dii[26 + step];
    dii_info.dii_type_info[dii_info.type_num].module_num++;
    module_info_len = p_dii[27 + step];
    offset = 0;
    p_tem = &p_dii[28 + step];
  
    while(module_info_len)
    {
      gap = 0;
      
      if(p_tem[offset] == MODULE_LINK_TAG)
      {
        offset++;
        gap = p_tem[offset++] + 2;
        position = p_tem[offset++];
        next_module_id = make16(&p_tem[offset]);
        offset += 2;
      }
      else if (p_tem[offset] == MODULE_INFO_TAG)
      {
        offset++;
        gap = p_tem[offset++] + 2;
        dii_info.dii_type_info[dii_info.type_num].mod_type = p_tem[offset++];
        dii_info.dii_type_info[dii_info.type_num].is_data_valid = p_tem[offset++];
        dii_info.dii_type_info[dii_info.type_num].start_addr = make32(&p_tem[offset]);
        offset += 4;
      }
      module_info_len -= gap;
    }

    if((position == 0x02) ||(next_module_id == 0))
    {
      dii_info.type_num++;
    }
    
    step += offset + 8;

  }

memcpy(&p_priv->dii_info,&dii_info,sizeof(dsmcc_dii_info_t));
}

/*!
 dsmcc parse find no exist block(intra).
 */
static BOOL dsmcc_find_no_exist_block(u8 *block_pos)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 i = 0;
   *block_pos = 0;
   
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if(p_priv->p_dsmcc_block[i].exist_bit != TRUE)
    { 
      *block_pos = i;
      return TRUE;
    }
  }
  return FALSE;
}

/*!
 dsmcc parse get type size(intra).
 */
static u32 dsmcc_get_type_size(dsmcc_dii_type_info_t *p_dii)
{
  u32 i = 0;
  u32 size = 0;

  for(i = 0; i < p_dii->module_num; i++)
  {
    size += p_dii->mod_info[i].mod_size;
  }

  return size;
}

/*!
 dsmcc parse find block from id(intra).
 */
static BOOL dsmcc_find_block_from_group_id(u8 *pos)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->p_dsmcc_block[i].exist_bit == TRUE)
      && (p_priv->p_dsmcc_block[i].group_id == p_priv->group_id))
        
    { 
      *pos = i;
      return TRUE;
    }
  }
  return FALSE;
}

/*!
  dsmcc para check ota data group block (intra)
  */
static BOOL dsmcc_check_ota_data_group_block(u8 block_id,u8 *pos)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->p_dsmcc_block[i].exist_bit == TRUE)
       && (p_priv->p_dsmcc_block[i].dl_group_bit == TRUE)
       && (p_priv->p_dsmcc_block[i].data_len > 0)
       && (p_priv->p_dsmcc_block[i].id == block_id))
      { 
        *pos = i;
        return TRUE;
      }
  }
  return FALSE;
}

/*!
  dsmcc para find no exist block (intra)
  */
static BOOL dsmcc_find_no_exist_ota_info_block(ota_block_info_t *block,u8 *pos)
{
   u8 i = 0;
   *pos = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if(block[i].exist_bit != TRUE)
    { 
      *pos = i;
      return TRUE;
    }
  }
  return FALSE;
}

/*!
  dsmcc para find  exist block (intra)
  */
 static u8 dsmcc_find_ota_info_block_from_id(ota_block_info_t *block,u8 block_id)
{
  u8 i = 0;
  for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((block[i].exist_bit == TRUE)
        && (block[i].id == block_id))
    { 
      return i;
    }
  }
  return 0xFF;
}

/*!
  dsmcc para check ota data downlaod bloxk (intra)
  */
static BOOL dsmcc_check_ota_data_dl_block(u8 block_id,u8 *pos)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 i = 0;
   for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
  {
    if((p_priv->p_dsmcc_block[i].exist_bit == TRUE)
       && (p_priv->p_dsmcc_block[i].id == block_id))
      { 
        if(p_priv->p_dsmcc_block[i].downloaded_bit == TRUE)
          {
            if((p_priv->p_dsmcc_block[i].burn_size > 0)
                && (p_priv->p_dsmcc_block[i].p_burn_buf != NULL))
              {
                *pos = i;
                 return TRUE;
              }
          }
        else
          {
            *pos = i;
            return TRUE;
          }
      }
  }
  return FALSE;
}

/*!
 dsmcc parse data base(intra).
 */
static BOOL parse_dsmcc_db_data_v2(u8 *p_ddb, u16 len,
          u8 section_number,
          dsmcc_pare_db_t *p_status)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u16 mod_id = 0;
  u8 mod_ver = 0;
  u16 block_num = 0;
  u32 offset = 0;
  u8 *p_dst = NULL;
  u32 content_len = 0;

#if 0//def WIN32
{
    FILE *fp = NULL;
    u32 filesize = 0;
    u32 crc = 0;
    u8 crc_data[4] ;
    
    fp = fopen("upg_file.bin", "rb");
    if(NULL != fp)
    {
        fseek(fp, 0L, SEEK_END);
        filesize =  ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        fread(p_priv->p_save_buf, 1, filesize, fp);
        fclose(fp);
        crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,p_priv->p_save_buf,filesize);
        crc_data[0] = (crc >> 24) & 0xff;
        crc_data[1] = (crc >> 16) & 0xff;
        crc_data[2] = (crc >> 8) & 0xff;
        crc_data[3] = (crc >> 0) & 0xff;
        memcpy(p_priv->p_save_buf + filesize,crc_data,sizeof(u32));
        p_priv->total_len  = filesize + sizeof(u32);
        p_status->finish_bit = TRUE;
        return TRUE;
    }
}
#endif

  mod_id = MAKE_WORD2(p_ddb[0], p_ddb[1]);
  mod_ver = p_ddb[2];
  block_num = MAKE_WORD2(p_ddb[4], p_ddb[5]);
  content_len = len - 6;
  p_dst = &p_ddb[6];

  if(mod_id > p_priv->max_mod_dl_number)
  {
    return FALSE;
  }
    
  if(p_priv->p_mod_dl_info[mod_id].section_info[section_number].is_received == TRUE)
  {
    return FALSE;
  }

  offset = mod_id * 256 * 4066 + section_number * 4066;

  if(p_priv->total_len < offset)
  {
    return FALSE;
  }

memcpy(p_priv->p_save_buf + offset,p_dst,content_len);
p_priv->p_mod_dl_info[mod_id].section_info[section_number].is_received = TRUE;
p_priv->received_len += content_len;
memset(p_status,0,sizeof(dsmcc_pare_db_t));

p_status->progress_all = (u8)(100 * p_priv->received_len
                                           / p_priv->total_len);
if(p_priv->received_len >= p_priv->total_len)
{
  /****finish*****/
  p_status->finish_bit = TRUE;
}

return TRUE;

}

/*!
 dsmcc parse add group ota block(intra).
 */
BOOL dsmcc_parse_add_ddm_info_to_ota_block(u8 *p_buf,
                        BOOL is_force,
                        BOOL is_data,
                        ota_block_data_verify_t function)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 pos = 0;
   ddm_data_head_info_t *p_ddm_info = NULL;
   u8 i = 0;
   u8 b_cnk = 0;
   u32 dm_head_crc = 0;
   u32 block_crc = 0;
   char *p_crc_buffer = NULL;
   u8 *p_block_buffer = NULL;
   u32 block_size = 0;
   u32 crc_buffer_size = 0;
   u8 block_id = 0;

   p_ddm_info = mtos_malloc(sizeof(ddm_data_head_info_t));
   
   if(p_ddm_info == NULL)
    {
      return FALSE;
    }
   memset(p_priv->p_dsmcc_block,0,sizeof(ota_data_info_t) * OTA_MAX_BLOCK_NUMBER);
   memset(p_ddm_info,0,sizeof(ddm_data_head_info_t));
   memcpy((u8 *)p_ddm_info,p_buf,sizeof(ddm_data_head_info_t));
   p_crc_buffer = (char *)p_buf + sizeof(u32) * 4;
   crc_buffer_size = p_ddm_info->header_info_size - sizeof(u32) * 4;
   dm_head_crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                p_crc_buffer,
                crc_buffer_size); 
#if 0
  {
    FILE *fp;
    fp = fopen("dm_ifno.bin", "wb");
    if(NULL != fp)
    {
    fwrite(p_crc_buffer, 1, crc_buffer_size, fp);
    fclose(fp);
    }
  }
#endif

  if((p_ddm_info->mark != UPG_DDM_HEAD_MARK)
    &&(dm_head_crc != p_ddm_info->header_info_crc))
  {
    return FALSE;
  }

  memcpy(p_priv->stbid_start,p_ddm_info->stbid_start,sizeof(u8) * 128);
  memcpy(p_priv->stbid_end,p_ddm_info->stbid_end,sizeof(u8) * 128);

   for(i = 0;i < p_ddm_info->block_number;i ++)
    {
      if(is_data == TRUE)
        {
            block_id = p_ddm_info->upg_block_info_table[i].ota_block_id;
            p_block_buffer = p_buf + p_ddm_info->upg_block_info_table[i].block_offset;
            block_size = p_ddm_info->upg_block_info_table[i].file_size;
            if(function != NULL)
            {
               if(function(block_id,p_block_buffer,block_size) != TRUE)
                {
                   continue;
                }
            }
            block_crc =  crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,
                                p_block_buffer,
                                block_size);
            if(block_crc != p_ddm_info->upg_block_info_table[i].file_crc)
              {
                continue;
              }
        }
#if 0 /***don't clear,because tools fix*****/    
      if(p_ddm_info->upg_block_info_table[i].ota_block_id == OTA_ALL_MAIN_CODE_BLOCK_ID)
        {
          memset(p_priv->p_dsmcc_block,0,sizeof(ota_data_info_t) * OTA_MAX_BLOCK_NUMBER);
        }
#endif
      if(TRUE == dsmcc_find_no_exist_block(&pos))
      {
         p_priv->p_dsmcc_block[pos].id = p_ddm_info->upg_block_info_table[i].ota_block_id;
         p_priv->p_dsmcc_block[pos].ota_new_ver = p_ddm_info->upg_block_info_table[i].ota_new_ver;
         if(is_data == TRUE)
          {
             p_priv->p_dsmcc_block[pos].p_burn_buf = p_buf 
                                          + p_ddm_info->upg_block_info_table[i].block_offset;
             p_priv->p_dsmcc_block[pos].burn_size = p_ddm_info->upg_block_info_table[i].file_size;
             p_priv->p_dsmcc_block[pos].downloaded_bit = TRUE;
          }
         else
          {
            p_priv->p_dsmcc_block[pos].p_burn_buf =  NULL;
            p_priv->p_dsmcc_block[pos].burn_size = 0;
            p_priv->p_dsmcc_block[pos].downloaded_bit = FALSE;
          }
         
         p_priv->p_dsmcc_block[pos].g_user_db_data_clear =  p_ddm_info->g_user_db_data_clear;
         p_priv->p_dsmcc_block[pos].g_user_sys_data_clear = p_ddm_info->g_user_sys_data_clear;
         
         p_priv->p_dsmcc_block[pos].g_ca_data_clear =  p_ddm_info->g_ca_data_clear;
         p_priv->p_dsmcc_block[pos].g_ads_data_clear = p_ddm_info->g_ads_data_clear;
         
         p_priv->p_dsmcc_block[pos].crc = p_ddm_info->upg_block_info_table[i].file_crc;
         p_priv->p_dsmcc_block[pos].is_new_buf = FALSE;
         if(is_force == 0)
        {
          p_priv->p_dsmcc_block[pos].is_factory_force = FALSE;
        }
       else if((is_force == 1) 
        && (p_ddm_info->upg_block_info_table[i].ota_block_id == OTA_ALL_MAIN_CODE_BLOCK_ID))
        {
         if(p_priv->p_dsmcc_block[pos].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
          {
             p_priv->p_dsmcc_block[pos].is_factory_force = TRUE;
             /****it will judge the block size,it is all flash or all maincode*****/
          }
        } 
        if((p_priv->p_dsmcc_block[pos].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
           && (p_priv->p_dsmcc_block[pos].is_factory_force != TRUE))
        {
          /******add all or main code cut block info********/
          if(p_ddm_info->upg_block_info_table[i].upg_mode == OTA_UPG_MODE_BLOCK_IN_FILE)
            {
              p_priv->p_dsmcc_block[pos].is_data_cut = TRUE;
              p_priv->p_dsmcc_block[pos].break_block.block_num = 
                                                  p_ddm_info->break_block.block_num;
              for(b_cnk = 0;b_cnk < p_ddm_info->break_block.block_num;b_cnk ++)
              {
                 p_priv->p_dsmcc_block[pos].break_block.block_id[b_cnk] = 
                                                  p_ddm_info->break_block.block_id[b_cnk];
              }
            }
          else          
            {
               p_priv->p_dsmcc_block[pos].is_data_cut = FALSE;
            }

          /******add all or main code just  block info********/
          if(p_ddm_info->upg_block_info_table[i].upg_mode == OTA_UPG_MODE_ALL_BLOCK_IN_FILE)
          {
             p_priv->p_dsmcc_block[pos].jump_block.block_num = p_ddm_info->jump_block.block_num;
            for(b_cnk = 0;b_cnk < p_ddm_info->jump_block.block_num;b_cnk ++)
            {
               p_priv->p_dsmcc_block[pos].jump_block.block_id[b_cnk] = 
                                          p_ddm_info->jump_block.block_id[b_cnk];
            }
          }
          else
          {
            p_priv->p_dsmcc_block[pos].jump_block.block_num = 0;
          }
          
        }
       //p_priv->p_dsmcc_block[pos].downloaded_bit = TRUE;
       p_priv->p_dsmcc_block[pos].exist_bit = TRUE;
      }
     #if 0 /***fix app all + ota block****/
      if(p_ddm_info->upg_block_info_table[i].ota_block_id == OTA_ALL_MAIN_CODE_BLOCK_ID)
        {
          break;
        }
      #endif
    }

   mtos_free(p_ddm_info);
   p_ddm_info =  NULL;
   return TRUE;
}
/*!
 dsmcc parse dsmcc free dsi info buff.
 */
void dsmcc_parse_free_dsi_info_buffer(void)
{
  dsmcc_dsi_info_t *p_dsi = NULL;
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u8 i = 0;
   p_dsi = (dsmcc_dsi_info_t *)&p_priv->dsi_info;
  for(i = 0;i < p_dsi->group_num;i ++)
  {
      if((p_dsi->group_info[i].p_info_buffer != NULL)
         && (p_dsi->group_info[i].m_info_size > 0))
        {
           mtos_free(p_dsi->group_info[i].p_info_buffer);
           p_dsi->group_info[i].p_info_buffer = NULL;
           p_dsi->group_info[i].m_info_size = 0;
        }
  }
}

/*!
 dsmcc parse dsmcc get dsi upg info.
 */
BOOL dsmcc_parse_get_dsi_upg_info(u8 group_id,
                                                      upg_check_info_t *p_upg_info)
{
    dsmcc_dsi_info_t *p_dsi = NULL;
    dsmcc_priv_t *p_priv = dsmcc_get_priv();
    u8 i = 0;
    u8 dsi_group_id = 0;
    
    p_dsi = (dsmcc_dsi_info_t *)&p_priv->dsi_info;
    for(i = 0; i < p_dsi->group_num; i ++)
    {
      dsi_group_id = (u8)p_dsi->group_info[i].group_id;
      if(dsi_group_id == group_id)
        {
           p_upg_info->upg_new_version = p_dsi->group_info[i].sw_version;
           p_upg_info->upg_old_version = 0;
           p_upg_info->upg_file_size = p_dsi->group_info[i].file_total_size;
           p_upg_info->dsi_group_id = (u8)p_dsi->group_info[i].group_id;
           if(p_dsi->group_info[i].sw_is_zipped)
            {
              p_upg_info->zipped_bit = TRUE;
            }
           else
            {
              p_upg_info->zipped_bit = FALSE;
            }
           p_upg_info->focre_bit = TRUE;
           p_upg_info->upg_bit = TRUE;
           p_upg_info->dsi_priv_bit = FALSE;
           dsmcc_parse_free_dsi_info_buffer();
           return TRUE;
        }
    }
    
     p_upg_info->upg_new_version = 0;
     p_upg_info->upg_old_version = 0;
     p_upg_info->upg_file_size = 0;
     p_upg_info->dsi_group_id = 0;
     p_upg_info->focre_bit = FALSE;
     p_upg_info->upg_bit = FALSE;
     dsmcc_parse_free_dsi_info_buffer();
     return FALSE;
}

/*!
 dsmcc parse dsmcc check dsi upg info.
 */
BOOL dsmcc_parse_check_dsi_upg_info(ota_hw_tdi_t hw_tdi,
                                    u32 old_version, 
                                    upg_check_info_t *p_upg_info,
                                    ota_version_check_t ota_version_check,
                                    ota_dsi_info_check_t ota_dsi_info_check)
{
    dsmcc_dsi_info_t *p_dsi = NULL;
    dsmcc_priv_t *p_priv = dsmcc_get_priv();
    u8 i = 0;
    BOOL ret = FALSE;
    BOOL upg_flag = FALSE;
    ota_dsi_info_t dsi_info = {0};
    p_dsi = (dsmcc_dsi_info_t *)&p_priv->dsi_info;
    
#if 0 /***check factiory upg****/
     p_dsi->is_force  = 1;
#endif    
     if(p_dsi->is_force == 0)
    {
      p_priv->is_factory_force = FALSE;
    }
   else if(p_dsi->is_force == 1)
    {
      p_priv->is_factory_force = TRUE;
    } 

  
    ota_dsmcc_debug_printf(OTA_DEBUG_INFO,
      " [%s]:check info:out:0x%x,h_vers:0x%x,h_id:0x%x,s_id:0x%x,old_ver:0x%x\n",
    __FUNCTION__,
    hw_tdi.oui,
    hw_tdi.hw_version,
    hw_tdi.hw_mod_id,
    hw_tdi.sw_mod_id,
    old_version);
    ota_dsmcc_debug_printf(OTA_DEBUG_INFO,
      " [%s]:dsi info:out:0x%x,h_vers:0x%x,h_id:0x%x,s_id:0x%x,old_ver:0x%x\n",
      __FUNCTION__,
      p_dsi->group_info[i].oui,
      p_dsi->group_info[i].hw_version,
      p_dsi->group_info[i].hw_mod_id,
      p_dsi->group_info[i].sw_mod_id,
      p_dsi->group_info[i].sw_version);

    for(i = 0; i < p_dsi->group_num; i ++)
    {

      if(TRUE == p_dsi->group_info[i].sw_is_spilt)
      {
         break;
      }

       p_upg_info->dsi_priv_bit = FALSE;
       if(p_dsi->group_info[i].m_info_size > 0)
        {
           if(dsmcc_parse_add_ddm_info_to_ota_block(p_dsi->group_info[i].p_info_buffer,
                 p_priv->is_factory_force,FALSE,NULL) == TRUE)
            {
               p_upg_info->dsi_priv_bit = TRUE;
            }

        }

     if(ota_dsi_info_check == NULL)
     {
      upg_flag = FALSE;
      if((p_priv->is_factory_force == TRUE) 
        && (p_dsi->group_info[i].hw_version == hw_tdi.hw_version)
        && (p_dsi->group_info[i].hw_mod_id == hw_tdi.hw_mod_id)
        && p_dsi->group_info[i].sw_block_id == OTA_ALL_MAIN_CODE_BLOCK_ID)
        {
          if((ota_version_check == NULL)
            && ((p_dsi->group_info[i].sw_version > old_version)
            || ((p_dsi->group_info[i].sw_version == 1) && (old_version == 0xFFFF))))
                /**reset**/
            {
              upg_flag = TRUE;
            }
            else if(ota_version_check != NULL)
            {
              ret = ota_version_check(p_dsi->group_info[i].sw_version,(u16)old_version,
                                                       p_priv->stbid_start,p_priv->stbid_end);
              if(TRUE == ret)
              {
                upg_flag = TRUE;
              }
            } 
           if(upg_flag == TRUE)
            {
             p_upg_info->upg_new_version = p_dsi->group_info[i].sw_version;
             p_upg_info->upg_old_version = old_version;
             p_upg_info->upg_file_size = p_dsi->group_info[i].file_total_size;
             p_upg_info->dsi_group_id = (u8)p_dsi->group_info[i].group_id;
             if(p_dsi->group_info[i].sw_is_zipped)
              {
                p_upg_info->zipped_bit = TRUE;
              }
             else
              {
                p_upg_info->zipped_bit = FALSE;
              }
             p_upg_info->focre_bit = TRUE;
             p_upg_info->upg_bit = TRUE;
             dsmcc_parse_free_dsi_info_buffer();
             return TRUE;
           }
        }
      if((p_dsi->group_info[i].oui == hw_tdi.oui)
        && (p_dsi->group_info[i].hw_mod_id == hw_tdi.hw_mod_id)
        && (p_dsi->group_info[i].hw_version == hw_tdi.hw_version)
        && (p_dsi->group_info[i].sw_mod_id == hw_tdi.sw_mod_id)) 
      {
          if((ota_version_check == NULL)
              && ((p_dsi->group_info[i].sw_version > old_version)
              || ((p_dsi->group_info[i].sw_version == 1) && (old_version == 0xFFFF))))
                  /**reset**/
              {
                upg_flag = TRUE;
              }
              else if(ota_version_check != NULL)
              {
                ret = ota_version_check(p_dsi->group_info[i].sw_version,(u16)old_version,
                                                         p_priv->stbid_start,p_priv->stbid_end);
                if(TRUE == ret)
                {
                  upg_flag = TRUE;
                }
              }

              OS_PRINTF("#####@@@@@debug check: check over\n");
              if(upg_flag == TRUE)
              {
               p_upg_info->upg_new_version = p_dsi->group_info[i].sw_version;
               p_upg_info->upg_old_version = old_version;
               p_upg_info->upg_file_size = p_dsi->group_info[i].file_total_size;
               p_upg_info->dsi_group_id = (u8)p_dsi->group_info[i].group_id;
               if(p_dsi->group_info[i].sw_is_zipped)
                {
                  p_upg_info->zipped_bit = TRUE;
                }
               else
                {
                  p_upg_info->zipped_bit = FALSE;
                }
               p_upg_info->focre_bit = FALSE;
               p_upg_info->upg_bit = TRUE;
               dsmcc_parse_free_dsi_info_buffer();
               return TRUE;
               }
        }
     }
     else
      {
         memset(&dsi_info,0,sizeof(ota_dsi_info_t));
         dsi_info.is_force = p_priv->is_factory_force;
         memcpy(dsi_info.stbid_start,p_priv->stbid_start,sizeof(u8) * 128);
         memcpy(dsi_info.stbid_end,p_priv->stbid_end,sizeof(u8) * 128);
         dsi_info.oui = p_dsi->group_info[i].oui;
         dsi_info.hw_version = p_dsi->group_info[i].hw_version;
         dsi_info.hw_mod_id = p_dsi->group_info[i].hw_mod_id;
         dsi_info.sw_version = p_dsi->group_info[i].sw_version;
         dsi_info.sw_mod_id = p_dsi->group_info[i].sw_mod_id;
         
         ret = ota_dsi_info_check(&dsi_info);
         if(ret == TRUE)
          {
             p_upg_info->upg_new_version = p_dsi->group_info[i].sw_version;
             p_upg_info->upg_old_version = old_version;
             p_upg_info->upg_file_size = p_dsi->group_info[i].file_total_size;
             p_upg_info->dsi_group_id = (u8)p_dsi->group_info[i].group_id;
             if(p_dsi->group_info[i].sw_is_zipped)
              {
                p_upg_info->zipped_bit = TRUE;
              }
             else
              {
                p_upg_info->zipped_bit = FALSE;
              }
             if(p_priv->is_factory_force == TRUE)
              {
                p_upg_info->focre_bit = TRUE;
              }
             else
              {
                p_upg_info->focre_bit = FALSE;
              }
             p_upg_info->upg_bit = TRUE;
             dsmcc_parse_free_dsi_info_buffer();
             return TRUE;
             }
      }
    }

     p_upg_info->upg_new_version = 0;
     p_upg_info->upg_old_version = 0;
     p_upg_info->upg_file_size = 0;
     p_upg_info->dsi_group_id = 0;
     p_upg_info->focre_bit = FALSE;
     p_upg_info->upg_bit = FALSE;
     dsmcc_parse_free_dsi_info_buffer();
     return FALSE;
}



/*!
 dsmcc parse dsmcc dsi or dii.
 */
u16 parse_dsmcc_dsi_dii_section_v2(u8  *p_data,
            u32 data_size)
{
    dsmcc_priv_t *p_priv = dsmcc_get_priv();
    u8 *p_buf = p_data;
    u16 msg_type = 0;
    u16 msg_len = 0;
    u32 transaction_id = 0;
    u32 sec_len = 0;
    u32 crc_check = 0;
    u32 sec_crc = 0;

    sec_len = MAKE_WORD(p_buf[2], (p_buf[1] & 0x0f));
    if(((sec_len) > DSMCC_DDM_BUF_SIZE) || ((sec_len) < 26))
    {
        ota_dsmcc_debug_printf(OTA_DEBUG_ERROR,
                                                   " [%s]:crc is err sec_len = %ld\n",
                                                   __FUNCTION__,
                                                   sec_len);
        return 0;
        }
        sec_crc =  (p_buf[sec_len -1] << 24)  |
                                                (p_buf[sec_len ] << 16)     |
                                                (p_buf[sec_len + 1] << 8)  |
                                                (p_buf[sec_len + 2]);

        crc_check = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,p_buf,sec_len - 1);
        if(crc_check != sec_crc)
        {
          ota_dsmcc_debug_printf(OTA_DEBUG_ERROR,
                                             " [%s]:crc is err sec crc:crc_check = 0x%x : 0x%x\n",
                                            __FUNCTION__,sec_crc,crc_check);
        return 0;
        }
  
    MT_ASSERT(NULL != p_buf);
    if(p_buf[8] != 0x11)
    {
        ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:Not DSMCC msg\n",__FUNCTION__);
        return 0;
    }
    if(p_buf[9] != 0x03)
    {
        ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:Not U-N download msg\n",__FUNCTION__);
        return 0;
    }
    transaction_id = make32(&p_buf[12]);

    msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);

    if(data_size < ((u32)msg_len + 20))
    {
       ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:Not DSMCC msg\n",__FUNCTION__);
       return 0;
    }
      msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);

    ota_dsmcc_debug_printf(OTA_DEBUG_ERROR,
            " [%s]:get msg type from infotable\n",
            __FUNCTION__,
            msg_type);
    switch(msg_type)
    {
        case DSMCC_MSG_DSI:
          parse_dsmcc_dsi_info_v2(p_priv,&p_buf[20], msg_len);
          break;
        case DSMCC_MSG_DII:
          parse_dsmcc_dii_info_v2(p_priv,&p_buf[20], msg_len);
          break;
        default:
          msg_type = 0;
          ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:should not come here\n",__FUNCTION__);
       
          break;
    }
  return msg_type;  
}


/*!
 dsmcc parse dsmcc table info.
 */
void parse_dsmcc_reset_db_dl_info(void)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   memset(p_priv->p_mod_dl_info,0,sizeof(mod_info_t) * p_priv->max_mod_dl_number);
   
}

/*!
 dsmcc parse dsmcc table info.
 */
BOOL parse_dsmcc_db_section_v2(u8 *p_pare_buf,u32 size,
              dsmcc_pare_db_t *p_status,
              u32 group_id,u8 *p_save_buf)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u8 *p_buf = p_pare_buf;
  u16 msg_type = 0;
  u16 msg_len = 0;
  u32 transaction_id = 0;
  u32 sec_len = 0;
  u32 crc_check = 0;
  u32 sec_crc = 0;
  u8 section_number = 0;

  if(group_id != p_priv->group_id)
  {
    return FALSE;
  }

  MT_ASSERT(NULL != p_buf);

  sec_len = MAKE_WORD(p_buf[2], (p_buf[1] & 0x0f));
  if(((sec_len) > DSMCC_DDM_BUF_SIZE) || ((sec_len) < 26))
  {
    return FALSE;
  }
  sec_crc =  (p_buf[sec_len -1] << 24)  |
                                                (p_buf[sec_len ] << 16)     |
                                                (p_buf[sec_len + 1] << 8)  |
                                                (p_buf[sec_len + 2]);
  
  crc_check = crc_fast_calculate(CRC32_ARITHMETIC_CCITT,0xFFFFFFFF,p_buf,sec_len - 1);
  if(crc_check != sec_crc)
  {
    return FALSE;
  }

  if(p_buf[8] != 0x11)
  {
    ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:Not DSMCC msg\n",__FUNCTION__);
    return FALSE;
  }
  if(p_buf[9] != 0x03)
  {
    ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:Not U-N download msg\n",__FUNCTION__);
    return FALSE;
  }
  section_number = p_buf[6];
 //last_section_number = p_buf[7];

  transaction_id = make32(&p_buf[12]);

  msg_len = MAKE_WORD2(p_buf[18], p_buf[19]);
  msg_type = MAKE_WORD2(p_buf[10], p_buf[11]);
  if(DSMCC_MSG_DDB != msg_type)
  {
      ota_dsmcc_debug_printf(OTA_DEBUG_ERROR," [%s]:should not come here\n",__FUNCTION__);
      return FALSE;
  }
  return parse_dsmcc_db_data_v2(&p_buf[20], msg_len, section_number,p_status);

}


/*!
  request dsmcc dsi or dii section
  */
void request_dsmcc_dsi_dii_section_v2(psi_request_data_t *p_req_data,u32 para1, u32 para2)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u16 msg_type = 0;
  u16 dsmcc_pid = 0;
  /* msg type could be DSI or DII */
  u16 dsmcc_msg_type = 0;
  u16 group_id = 0;
  dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  msg_type = (u16)(para1 & 0xFFFF);
  
  p_priv->data_pid = dsmcc_pid;
  if(msg_type == 0x01)
  {
    dsmcc_msg_type = DSMCC_MSG_DII;
  }
  else
  {
     dsmcc_msg_type = DSMCC_MSG_DSI;
  }

  
  switch(dsmcc_msg_type)
  {
    case DSMCC_MSG_DII:
      group_id = (u16)(para2 & 0xFFFF);
      p_priv->group_id = group_id;
      break;
    default:
      group_id = 0x00;
      break;

  }

ota_dsmcc_debug_printf(OTA_DEBUG_INFO,
  " [%s]:request dsmcc dsi dii start pid[0x%x],table id[0x%x],g_id[0x%x],type[0x%x]\n",
                                                           __FUNCTION__,
                                         dsmcc_pid,
                                         DVB_TABLE_ID_DSMCC_MSG,
                                         group_id,
                                         dsmcc_msg_type);

  p_req_data->media_type = MF_DSMCC;
  p_req_data->table_id   = DVB_TABLE_ID_DSMCC_MSG;
  p_req_data->req_mode   = DMX_DATA_SINGLE;
  p_req_data->pid        = dsmcc_pid;
  p_req_data->timeout    = DSMCC_MSG_TIMEOUT;
  p_req_data->slot_type  = DMX_CH_TYPE_SECTION;
  p_req_data->enable_crc = TRUE;

  memset(p_req_data->filter_value, 0, sizeof(u8) * DMX_SECTION_FILTER_SIZE);
  memset(p_req_data->filter_mask, 0,sizeof(u8) * DMX_SECTION_FILTER_SIZE);
  
  p_req_data->filter_value[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_req_data->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_req_data->filter_value[1] = (u8)(group_id >> 8);
  p_req_data->filter_mask[1] = 0xFF;
  p_req_data->filter_value[2] = (u8)(group_id & 0xFF);
  p_req_data->filter_mask[2] = 0xFF;
  p_req_data->filter_value[8] = (u8)(dsmcc_msg_type >> 8);
  p_req_data->filter_mask[8] = 0xFF;
  p_req_data->filter_value[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_req_data->filter_mask[9] = 0xFF;
#else
  p_req_data->filter_value[3] = (u8)(group_id >> 8);
  p_req_data->filter_mask[3] = 0xFF;
  p_req_data->filter_value[4] = (u8)(group_id & 0xFF);
  p_req_data->filter_mask[4] = 0xFF;
  p_req_data->filter_value[10] = (u8)(dsmcc_msg_type >> 8);
  p_req_data->filter_mask[10] = 0xFF;
  p_req_data->filter_value[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_req_data->filter_mask[11] = 0xFF;
#endif

 OS_PRINTF(" [%s]:request dsmcc dsi dii pid[0x%x],table id[0x%x],g_id[0x%x],type[0x%x]\n",
                      __FUNCTION__,
                      p_req_data->pid,
                      p_req_data->table_id,
                      group_id,
                      dsmcc_msg_type);
    

}



/*!
  request dsmcc dsi or dii section
  */
void request_dsmcc_dsi_dii_multi_section_v2(psi_request_data_t *p_req_data,u32 para1, u32 para2)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u16 msg_type = 0;
  u16 dsmcc_pid = 0;
  /* msg type could be DSI or DII */
  u16 dsmcc_msg_type = 0;
  u16 group_id = 0;
  dsmcc_pid = (para1 & 0xFFFF0000) >> 16;
  msg_type = (u16)(para1 & 0xFFFF);
  
  p_priv->data_pid = dsmcc_pid;

#if 0  
  if(msg_type == 0x01)
  {
    dsmcc_msg_type = DSMCC_MSG_DII;
  }
  else
  {
     dsmcc_msg_type = DSMCC_MSG_DSI;
  }
#else
  dsmcc_msg_type = msg_type;
#endif
  
  switch(dsmcc_msg_type)
  {
    case DSMCC_MSG_DII:
      group_id = (u16)(para2 & 0xFFFF);
      p_priv->group_id = group_id;
      break;
    default:
      group_id = 0x00;
      break;

  }

ota_dsmcc_debug_printf(OTA_DEBUG_INFO,
  " [%s]:request dsmcc dsi dii start pid[0x%x],table id[0x%x],g_id[0x%x],type[0x%x]\n",
                                          __FUNCTION__,
                                          dsmcc_pid,
                                          DVB_TABLE_ID_DSMCC_MSG,
                                          group_id,
                                          dsmcc_msg_type);

  p_req_data->media_type = MF_DSMCC;
  p_req_data->table_id   = DVB_TABLE_ID_DSMCC_MSG;
  //p_req_data->req_mode   = DMX_DATA_SINGLE;
  p_req_data->req_mode   = DMX_DATA_MULTI;
  p_req_data->pid        = dsmcc_pid;
  p_req_data->timeout    = DSMCC_MSG_TIMEOUT;
  p_req_data->slot_type  = DMX_CH_TYPE_SECTION;
  p_req_data->enable_crc = TRUE;

  memset(p_req_data->filter_value, 0, sizeof(u8) * DMX_SECTION_FILTER_SIZE);
  memset(p_req_data->filter_mask, 0,sizeof(u8) * DMX_SECTION_FILTER_SIZE);
  
  p_req_data->filter_value[0] = DVB_TABLE_ID_DSMCC_MSG;
  p_req_data->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_req_data->filter_value[1] = (u8)(group_id >> 8);
  p_req_data->filter_mask[1] = 0xFF;
  p_req_data->filter_value[2] = (u8)(group_id & 0xFF);
  p_req_data->filter_mask[2] = 0xFF;
  p_req_data->filter_value[8] = (u8)(dsmcc_msg_type >> 8);
  p_req_data->filter_mask[8] = 0xFF;
  p_req_data->filter_value[9] = (u8)(dsmcc_msg_type & 0xFF);
  p_req_data->filter_mask[9] = 0xFF;
#else
  p_req_data->filter_value[3] = (u8)(group_id >> 8);
  p_req_data->filter_mask[3] = 0xFF;
  p_req_data->filter_value[4] = (u8)(group_id & 0xFF);
  p_req_data->filter_mask[4] = 0xFF;
  p_req_data->filter_value[10] = (u8)(dsmcc_msg_type >> 8);
  p_req_data->filter_mask[10] = 0xFF;
  p_req_data->filter_value[11] = (u8)(dsmcc_msg_type & 0xFF);
  p_req_data->filter_mask[11] = 0xFF;
#endif
}


/*!
  request dsmcc data base multi section
  */
void request_dsmcc_db_multi_section_v2(psi_request_data_t *p_req_data,u32 para1, u32 para2)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u16 dsmcc_pid = (u16)para1;
  u8 group_id = 0;
  u8 type = 0;
  group_id = (u8)para2 ;
  
  p_priv->group_id = group_id;
  p_priv->data_pid = dsmcc_pid;
  
  type = MODULE_TYPE_ALL;
  ota_dsmcc_debug_printf(OTA_DEBUG_INFO," [%s]:requesting a dsmcc ddb section\n",__FUNCTION__);
    
  p_req_data->media_type = MF_DSMCC;
  p_req_data->table_id   = DVB_TABLE_ID_DSMCC_DDM;
  p_req_data->req_mode   = DMX_DATA_MULTI;
  p_req_data->pid        = dsmcc_pid;
  p_req_data->timeout    = DSMCC_DDB_TIMEOUT;
  p_req_data->slot_type  = DMX_CH_TYPE_SECTION;
  p_req_data->enable_crc = TRUE;


  memset(p_req_data->filter_value, 0, sizeof(u8) * DMX_SECTION_FILTER_SIZE);
  memset(p_req_data->filter_mask, 0, sizeof(u8) * DMX_SECTION_FILTER_SIZE);

  //Set filter parameter
  p_req_data->filter_value[0] = DVB_TABLE_ID_DSMCC_DDM;
  p_req_data->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_req_data->filter_value[1] = group_id ;
  p_req_data->filter_mask[1] = 0xFF;
  p_req_data->filter_value[2] = type;
  p_req_data->filter_mask[2] = 0xFF;

  p_req_data->filter_value[8] = (u8)(DSMCC_MSG_DDB >> 8);
  p_req_data->filter_mask[8] = 0xFF;
  p_req_data->filter_value[9] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_req_data->filter_mask[9] = 0xFF;
#else
  p_req_data->filter_value[3] = (u8)group_id ;
  p_req_data->filter_mask[3] = 0xFF;
  p_req_data->filter_value[4] = (u8)type;
  p_req_data->filter_mask[4] = 0xFF;
 

  p_req_data->filter_value[10] = (u8)(DSMCC_MSG_DDB >> 8);
  p_req_data->filter_mask[10] = 0xFF;
  p_req_data->filter_value[11] = (u8)(DSMCC_MSG_DDB & 0xFF);
  p_req_data->filter_mask[11] = 0xFF;
#endif

}


/*!
  dsmcc para add dii info
  */
BOOL dsmcc_parse_add_dii_info(u16 group_id)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   u8 pos = 0;
   if((TRUE == dsmcc_find_block_from_group_id(&pos))
       && (group_id == p_priv->group_id))
    {
      if(p_priv->dii_info.type_num == 1) /***no split mode***/
      {
        p_priv->p_dsmcc_block[pos].data_len = 
          dsmcc_get_type_size(&p_priv->dii_info.dii_type_info[0]);
      }
      else
      {
        p_priv->p_dsmcc_block[pos].data_len = 0;
      }
      p_priv->p_dsmcc_block[pos].dl_group_bit = TRUE;
      return TRUE;
    }
   return FALSE;
}

/*!
  dsmcc para check parse check dii info
  */
BOOL dsmcc_parse_check_dii_info(ota_block_info_t *p_sys_block,ota_block_info_t *p_re_block)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  u8 i = 0;
  u8 pos1 = 0,pos2 = 0;
  BOOL check_ota_bit = FALSE;
  u8 bl_cnk = 0;
  u8 sys_block_index = 0;
  BOOL break_block_upg_bit = FALSE;

  for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i++)
  {
   if(p_sys_block[i].exist_bit == TRUE)
  {
   
   if((TRUE == dsmcc_check_ota_data_group_block(p_sys_block[i].id,&pos1))
     && ((p_sys_block[i].base_addr % CHARSTO_SECTOR_SIZE) == 0)
     && ((p_sys_block[i].size % CHARSTO_SECTOR_SIZE) == 0))
  {
   if(TRUE == dsmcc_find_no_exist_ota_info_block(p_re_block,&pos2))
    {
      /*******************************************************************
      all flash or main code,don't upg other block,other block ota_version is maincode ota version
      ********************************************************************/
      if(p_priv->p_dsmcc_block[pos1].id == OTA_ALL_MAIN_CODE_BLOCK_ID)
      {
        if((p_priv->is_ota_force != TRUE)
            && ((p_sys_block[i].size < (p_priv->p_dsmcc_block[pos1].data_len - 4)))) 
            /***last 4 byte is crc***/
          {
            continue;
          }
       break_block_upg_bit = FALSE;
       if(p_priv->p_dsmcc_block[pos1].break_block.block_num > 0)
        {
           for(bl_cnk = 0;bl_cnk < p_priv->p_dsmcc_block[pos1].break_block.block_num;bl_cnk ++)
          {
            sys_block_index = dsmcc_find_ota_info_block_from_id(p_sys_block,
                                          p_priv->p_dsmcc_block[pos1].break_block.block_id[bl_cnk]);
            if(sys_block_index != 0xFF)
            {
              if(p_priv->p_dsmcc_block[pos1].ota_new_ver > 
                          p_sys_block[sys_block_index].ota_old_ver)
              {
                  break_block_upg_bit = TRUE;
              }
            }
          }
         }

         if((p_priv->is_ota_force == TRUE)
            || ((p_priv->p_dsmcc_block[pos1].ota_new_ver > p_sys_block[i].ota_old_ver)
                   && (p_priv->p_dsmcc_block[pos1].break_block.block_num == 0))
            ||(break_block_upg_bit == TRUE))
          {
             memset(p_re_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
             memcpy(&p_re_block[0],&p_sys_block[i],sizeof(ota_block_info_t));
             p_re_block[0].exist_bit = TRUE;
             p_re_block[0].upg_bit = TRUE;
             p_re_block[0].ota_new_ver = p_priv->p_dsmcc_block[pos1].ota_new_ver;
             check_ota_bit = TRUE;
             break;
          }
      }
      else if((p_priv->p_dsmcc_block[pos1].ota_new_ver > p_sys_block[i].ota_old_ver)
                 && (p_sys_block[i].size >= (p_priv->p_dsmcc_block[pos1].data_len - 4))) 
                 /***last 4 byte is crc***/
      {
          memcpy(&p_re_block[pos2],&p_sys_block[i],sizeof(ota_block_info_t));
          p_re_block[pos2].exist_bit = TRUE;
          p_re_block[pos2].upg_bit = TRUE;
          p_re_block[pos2].ota_new_ver = p_priv->p_dsmcc_block[pos1].ota_new_ver;
          check_ota_bit = TRUE;
      }
    }
  }
  }
  }

if(check_ota_bit == TRUE)
{
    return TRUE;
}

return FALSE;
}

static BOOL  dsmcc_parse_check_block_id_in_jump_data(tag_ota_jump_t *p_jump_data,u8 cut_block_id)
{
   u8 i = 0;
   for(i = 0;i < p_jump_data->block_num;i ++)
    {
       if(p_jump_data->block_id[i] == cut_block_id)
        {
          return TRUE;
        }
    }
   return FALSE;
}

static BOOL  dsmcc_parse_check_block_id_in_cut_data(tag_ota_cut_t *p_cut_data,u8 cut_block_id)
{
   u8 i = 0;
   for(i = 0;i < p_cut_data->block_num;i ++)
    {
       if(p_cut_data->block_id[i] == cut_block_id)
        {
          return TRUE;
        }
    }
   return FALSE;
}

static BOOL dsmcc_parse_check_block_id_in_dmh_info(ota_dmh_info_t *p_dmh_info,u8 block_id)
{
   u8 i = 0,j = 0;
   for(i = 0;i < p_dmh_info->header_num;i ++)
    {
       for(j = 0;j < p_dmh_info->header_group[i].block_num ;j ++)
        {
           if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
            {
              return TRUE;
            }
        }
    }
   return FALSE;
}

/*!
  dsmcc para check data for donwnload
  */
BOOL dsmcc_parse_check_data_for_download(BOOL is_force,
                          ota_block_info_t *p_sys_block,
                          ota_block_info_t *p_re_block,
                          ota_version_check_t ota_version_check,
                          BOOL g_dmh_check)
{
    dsmcc_priv_t *p_priv = dsmcc_get_priv();
    u8 i = 0;
    u8 pos1 = 0,pos2 = 0;
    BOOL check_ota_bit = FALSE;
    BOOL ret = FALSE;
    BOOL version_upg_flag = FALSE;
    u8 cnk_sys = 0;
    u8 block_index = 0;
    u8 block_number = 0;
    ota_dmh_info_t dmh_check_info;
    memset(p_re_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
    version_upg_flag = FALSE;
   if(TRUE == dsmcc_check_ota_data_dl_block(OTA_ALL_MAIN_CODE_BLOCK_ID,&pos1)) /***all block****/
    {
        version_upg_flag = FALSE;
        
        memset(&dmh_check_info,0,sizeof(ota_dmh_info_t));
        if(g_dmh_check == TRUE)
       {
        ota_filter_find_dmh_info(p_priv->p_dsmcc_block[pos1].p_burn_buf,
                                                  p_priv->p_dsmcc_block[pos1].burn_size,
                                                  &dmh_check_info,0);
          }
        
        /******all block upg,it may jump same block******/
        if(p_priv->p_dsmcc_block[pos1].is_data_cut == FALSE) 
        {
             /**check app all ****/
            block_index = dsmcc_find_ota_info_block_from_id(p_sys_block,
                                                                        OTA_ALL_MAIN_CODE_BLOCK_ID);
            if(block_index != 0xFF)
            {
             if((ota_version_check == NULL)
                && ((p_priv->p_dsmcc_block[pos1].ota_new_ver > \
                            p_sys_block[block_index].ota_old_ver)
                ||((p_priv->p_dsmcc_block[pos1].ota_new_ver == 0X01) 
                && (p_sys_block[block_index].ota_old_ver == 0XFFFF)))) /**reset**/
                {
                  version_upg_flag = TRUE;
                }
                else if(ota_version_check != NULL)
                {
                  ret = ota_version_check(p_priv->p_dsmcc_block[pos1].ota_new_ver,
                                 p_sys_block[block_index].ota_old_ver,
                                 p_priv->stbid_start,p_priv->stbid_end);
                  if(TRUE == ret)
                  {
                    version_upg_flag = TRUE;
                  }
                }
               /*****else if factory force upg********/
              if(version_upg_flag == TRUE)  /*** app all  is upg *****/
              {
                  /*******************************************************************
                  all flash or main code,don't upg other block,other block ota_version is maincode ota version
                  ********************************************************************/
                  memset(p_re_block,0,sizeof(ota_block_info_t) * OTA_MAX_BLOCK_NUMBER);
                  memcpy(&p_re_block[0],&p_sys_block[block_index],sizeof(ota_block_info_t));
                  p_re_block[0].exist_bit = TRUE;
                  p_re_block[0].upg_bit = TRUE;
                  p_re_block[0].ota_new_ver = p_priv->p_dsmcc_block[pos1].ota_new_ver;
                  check_ota_bit = TRUE;
              }
            else/****compare other block version*****/
            {
             p_priv->p_dsmcc_block[pos1].break_block.block_num = 0;
             for(cnk_sys = 0;cnk_sys < OTA_MAX_BLOCK_NUMBER; cnk_sys ++)
              {
                 version_upg_flag = FALSE; /***clear version flag****/
                 if((p_sys_block[cnk_sys].exist_bit == TRUE) 
                && ((p_sys_block[cnk_sys].base_addr % CHARSTO_SECTOR_SIZE) == 0)
                && ((p_sys_block[cnk_sys].size % CHARSTO_SECTOR_SIZE) == 0))
                {
                 if(TRUE == dsmcc_parse_check_block_id_in_jump_data(\
                            &p_priv->p_dsmcc_block[pos1].jump_block,
                            p_sys_block[cnk_sys].id))
                  {
                     continue;
                  }
                 if((FALSE == dsmcc_parse_check_block_id_in_dmh_info(&dmh_check_info,
                                     p_sys_block[cnk_sys].id))
                      && (g_dmh_check == TRUE))
                  {
                    continue;
                  }
                                       
                 if((ota_version_check == NULL)
                  && ((p_priv->p_dsmcc_block[pos1].ota_new_ver > 
                           p_sys_block[cnk_sys].ota_old_ver)
                  ||((p_priv->p_dsmcc_block[pos1].ota_new_ver == 0X01) 
                  && (p_sys_block[cnk_sys].ota_old_ver == 0XFFFF)))) /**reset**/
                  {
                    version_upg_flag = TRUE;
                  }
                  else if(ota_version_check != NULL)
                  {
                    ret = ota_version_check(p_priv->p_dsmcc_block[pos1].ota_new_ver,
                              p_sys_block[cnk_sys].ota_old_ver,
                              p_priv->stbid_start,p_priv->stbid_end);
                    if(TRUE == ret)
                    {
                      version_upg_flag = TRUE;
                      p_priv->p_dsmcc_block[pos1].is_data_cut = TRUE;
                      block_number = p_priv->p_dsmcc_block[pos1].break_block.block_num;
                      p_priv->p_dsmcc_block[pos1].break_block.block_id[block_number] = 
                                                          p_sys_block[cnk_sys].id;
                      p_priv->p_dsmcc_block[pos1].break_block.block_num ++;
                    }
                  }

                 if((TRUE == dsmcc_find_no_exist_ota_info_block(p_re_block,&pos2))
                  && (version_upg_flag == TRUE))
                  {
                    memcpy(&p_re_block[pos2],
                      &p_sys_block[cnk_sys],
                      sizeof(ota_block_info_t));
                    p_re_block[pos2].exist_bit = TRUE;
                    p_re_block[pos2].upg_bit = TRUE;
                    p_re_block[pos2].ota_new_ver = 
                      p_priv->p_dsmcc_block[pos1].ota_new_ver;
                    check_ota_bit = TRUE;
                  }
                  
                }
              }
            }
            }
        }
        else /******same block upg,it will selete same block upg******/
          {
              /*****compare cut block version********/
              
              for(cnk_sys = 0;cnk_sys < OTA_MAX_BLOCK_NUMBER; cnk_sys ++)
                {
                   version_upg_flag = FALSE; /***clear version flag****/
                   if((p_sys_block[cnk_sys].exist_bit == TRUE) 
                      && ((p_sys_block[cnk_sys].base_addr % CHARSTO_SECTOR_SIZE) == 0)
                       && ((p_sys_block[cnk_sys].size % CHARSTO_SECTOR_SIZE) == 0))
                    {
                       if(FALSE == dsmcc_parse_check_block_id_in_cut_data(\
                                &p_priv->p_dsmcc_block[pos1].break_block,
                                p_sys_block[cnk_sys].id))
                        {
                           continue;
                        }
                        if((FALSE == dsmcc_parse_check_block_id_in_dmh_info(\
                                &dmh_check_info,
                                p_sys_block[cnk_sys].id))
                           && (g_dmh_check == TRUE))
                        {
                          continue;
                        }
                                             
                       if((ota_version_check == NULL)
                          && ((p_priv->p_dsmcc_block[pos1].ota_new_ver
                                    > p_sys_block[cnk_sys].ota_old_ver)
                          ||((p_priv->p_dsmcc_block[pos1].ota_new_ver == 0X01) 
                          && (p_sys_block[cnk_sys].ota_old_ver == 0XFFFF)))) /**reset**/
                        {
                          version_upg_flag = TRUE;
                        }
                        else if(ota_version_check != NULL)
                        {
                          ret = ota_version_check(p_priv->p_dsmcc_block[pos1].ota_new_ver,
                                   p_sys_block[cnk_sys].ota_old_ver,
                                   p_priv->stbid_start,p_priv->stbid_end);
                          if(TRUE == ret)
                          {
                            version_upg_flag = TRUE;
                          }
                        }

                       if((TRUE == dsmcc_find_no_exist_ota_info_block(p_re_block,&pos2))
                        && (version_upg_flag == TRUE))
                        {
                          memcpy(&p_re_block[pos2],&p_sys_block[cnk_sys],sizeof(ota_block_info_t));
                          p_re_block[pos2].exist_bit = TRUE;
                          p_re_block[pos2].upg_bit = TRUE;
                          p_re_block[pos2].ota_new_ver = p_priv->p_dsmcc_block[pos1].ota_new_ver;
                          check_ota_bit = TRUE;
                        }
                    }
                }
          }
    }
   
   /***other block process***/
    {
      for(i = 0; i < OTA_MAX_BLOCK_NUMBER; i ++)
      {
         version_upg_flag = FALSE; /***clear version flag****/
         if(p_sys_block[i].exist_bit == TRUE)
          {
             if((TRUE == dsmcc_check_ota_data_dl_block(p_sys_block[i].id,&pos1))
                 && ((p_sys_block[i].base_addr % CHARSTO_SECTOR_SIZE) == 0)
                 && ((p_sys_block[i].size % CHARSTO_SECTOR_SIZE) == 0)
                 && (p_sys_block[i].id != OTA_ALL_MAIN_CODE_BLOCK_ID))
              {
                 if(TRUE == dsmcc_find_no_exist_ota_info_block(p_re_block,&pos2))
                  {
                        if((ota_version_check == NULL)
                        && ((p_priv->p_dsmcc_block[pos1].ota_new_ver > p_sys_block[i].ota_old_ver)
                             ||((p_priv->p_dsmcc_block[pos1].ota_new_ver == 0X01) 
                                          && (p_sys_block[i].ota_old_ver == 0XFFFF)))) /**reset**/
                        {
                          version_upg_flag = TRUE;
                        }
                        else if(ota_version_check != NULL)
                        {
                          ret = ota_version_check(
                                            p_priv->p_dsmcc_block[pos1].ota_new_ver,
                                            p_sys_block[i].ota_old_ver,
                                            p_priv->stbid_start,p_priv->stbid_end);
                          if(TRUE == ret)
                          {
                            version_upg_flag = TRUE;
                          }
                        }

                      if(version_upg_flag == TRUE)
                      {
                          memcpy(&p_re_block[pos2],&p_sys_block[i],sizeof(ota_block_info_t));
                          p_re_block[pos2].exist_bit = TRUE;
                          p_re_block[pos2].upg_bit = TRUE;
                          p_re_block[pos2].ota_new_ver = p_priv->p_dsmcc_block[pos1].ota_new_ver;
                          check_ota_bit = TRUE;
                      }
                  }
              }
          }
      }
    }
 if(check_ota_bit == TRUE)
  {
      return TRUE;
  }

 return FALSE;
}

  /*!
  dsmcc para process download init
  */
void dsmcc_download_init(u8 *p_save_buf,u32 total_szie)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   p_priv->p_save_buf = p_save_buf;
   p_priv->total_len = total_szie;
   p_priv->received_len = 0;
   p_priv->max_mod_dl_number = (total_szie / (256 * 4066)) + 1;
   p_priv->p_mod_dl_info = mtos_malloc(sizeof(mod_info_t) * p_priv->max_mod_dl_number);
   MT_ASSERT(p_priv->p_mod_dl_info != NULL);
}
/*!
  dsmcc para process download finish
  */
BOOL dsmcc_pare_download_finish(void)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   mtos_free(p_priv->p_mod_dl_info);
   p_priv->p_mod_dl_info = NULL;
   return TRUE;
}

/*!
  dsmcc para clear private dsi info
  */
void dsmcc_parse_clear_dsi_info(void)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   memset(&p_priv->dsi_info,0,sizeof(dsmcc_dsi_info_t));
}

/*!
  dsmcc para clear private dii info
  */
void dsmcc_parse_clear_dii_info(void)
{
   dsmcc_priv_t *p_priv = dsmcc_get_priv();
   memset(&p_priv->dii_info,0,sizeof(dsmcc_dii_info_t));
}

/*!
  dsmcc para destory private init
  */
void dsmcc_parse_deinit(void)
{
  dsmcc_priv_t *p_priv = dsmcc_get_priv();
  if(p_priv != NULL)
  {
      mtos_free(p_priv);
      p_priv = NULL;
  }
}

/*!
  dsmcc para private  init
  */
RET_CODE dsmcc_parse_init(ota_data_info_t *p_block,
                                                          ota_debug_level_t debug_level,
                                                          ota_printf_t printf)
{
  dsmcc_priv_t *p_dsmcc_priv = NULL;
  if(p_dsmcc_priv == NULL)
  {
    p_dsmcc_priv  = (dsmcc_priv_t *)mtos_malloc(sizeof(dsmcc_priv_t));
  } 
  
  MT_ASSERT(p_dsmcc_priv != NULL);
  memset(p_dsmcc_priv,0, sizeof(dsmcc_priv_t));
  p_dsmcc_priv->p_dsmcc_block = p_block;
  p_dsmcc_priv->debug_level = debug_level;
  p_dsmcc_priv->ota_dsmcc_debug_printf = printf;
  class_register(OTA_DSMCC_CLASS_ID,(void *)p_dsmcc_priv);
  return SUCCESS;
}


