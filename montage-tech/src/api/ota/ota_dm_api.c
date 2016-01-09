/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <assert.h>
#include <string.h>
// system
#include "sys_types.h"
#include "sys_define.h"


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

//#include "data_manager.h"
//os
#include "hal_misc.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_fifo.h"
#include "mtos_msg.h"
#include "mtos_mutex.h"

#include "class_factory.h"
#include "drv_dev.h"
#include "charsto.h"
#include "mdl.h"

#include "nim.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "fcrc.h"
#include "ota_public_def.h"
#include "ota_dm_api.h"
#include "err_check_def.h"


#define OTA_DM_MAX_PIECE_NUMBER 32

#define OTA_DM_PIECE_MARK 0x2E3C4B5A


/*!
  piece head ifo
  */
typedef struct piece_head_info_tg
{
/*!
  piece mark
  */
  u32 mark;
/*!
  piece id
  */
  u32 id;
/*!
  piece size
  */
  u32 size;
/*!
  piece crc
  */
  u32 crc;
}piece_head_info_t;

/*!
  piece table ifo
  */
typedef struct piece_table_info_tg
{
  /*!
    piece id
    */
   u8 piece_id;
  /*!
    piece pos
    */
   u32 piece_pos;
    /*!
    piece size
    */
   u32 piece_size;
}piece_table_info_t;

/*!
  ota dm priv
  */
typedef struct ota_dm_priv_tg
{
  /*!
    dm config
    */
  ota_dm_config_t dm_config;
  /*!
    Device operation protect mutex, if it is mutex
    */
  void *p_mutex;
  /*!
     piece table number
    */
  u16 piece_table_num;
   /*!
     piece table
    */
  piece_table_info_t piece_table[OTA_DM_MAX_PIECE_NUMBER];
   /*!
     flash st
    */
  charsto_prot_status_t flash_st;
 /*!
    dm forbid
    */
  BOOL dm_forbid;
/*!
  dm dmh info
  */
 dm_dmh_info_t *p_dmh_info;
}ota_dm_priv_t;

/*!
  internal date headle
  */
ota_dm_priv_t *ota_dm_api_get_priv(void)
{
   return (ota_dm_priv_t *)class_get_handle_by_id(OTA_DM_CLASS_ID);
}

static void ota_dm_api_printf(ota_dm_debug_level_t level,const char *p_fmt, ...)
{
  ota_dm_priv_t *p_priv =  NULL;
  char buffer[MAX_PRINT_BUFFER_LEN] = {0};
  p_print_list p_args = NULL;
  p_priv = ota_dm_api_get_priv();

  
  if(p_priv->dm_config.debug_level  < OTA_DM_DEBUG_ALL)
  {
    return;
  }
  if(level >= p_priv->dm_config.debug_level)
  {
     if(p_priv->dm_config.ota_dm_api_printf != NULL)
      {
        PRINTF_VA_START(p_args, p_fmt);
        ck_vsnprintf(buffer,sizeof(buffer), p_fmt, p_args);
        PRINTF_VA_END(p_args);
        p_priv->dm_config.ota_dm_api_printf("%s", buffer);
      }
  }
  return;
}

/*!
  data debug
  */
#if 0
void ota_dm_api_debug_data_printf(u8 * buff,u32 size)
{
    u32 i =0;
    u8 *print_buff = buff;
    if(NULL == print_buff)
     return ;
    OS_PRINTF("\n\n");
    for(i = 0;i < size;i ++)
      {
          if((i % 16) == 0)
          OS_PRINTF("\n");
          OS_PRINTF(" 0x%2x  ",print_buff[i]);
      }
    OS_PRINTF("\n\n");
}
#endif
/*!
  crc init
  */
static void ota_dm_api_piece_crc_init(void)
{
  RET_CODE ret = SUCCESS;
  
   /* create crc32 table */
  ret = crc_setup_fast_lut(CRC32_ARITHMETIC_CCITT);
  CHECK_FAIL_RET_VOID(SUCCESS == ret); 
}

/*!
  crc generate
  */
static u32 ota_dm_api_piece_generate(u8 *buf, u32 len)
{
  return crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, buf, len);
}


/*!
  internal lock
  */
static void ota_dm_api_lock(ota_dm_priv_t *p_priv)
{
  if(p_priv->dm_config.is_use_mutex)
  {
    mtos_mutex_take(p_priv->p_mutex);
  }
  else
  {
    mtos_task_lock();
  }
}

/*!
  internal unlock
  */
static void ota_dm_api_unlock(ota_dm_priv_t *p_priv)
{
  if(p_priv->dm_config.is_use_mutex)
  {
    mtos_mutex_give(p_priv->p_mutex);
  }
  else
  {
    mtos_task_unlock();
  }
}

/*!
  internal flash unprotect
  */
static void ota_dm_api_charsto_unprotect_all(void)
{

  void *p_dev = NULL;

  ota_dm_priv_t *p_priv =  NULL;
  charsto_prot_status_t st = {0};

  p_priv = ota_dm_api_get_priv();
  if(p_priv->dm_config.is_protect == FALSE)
  {
    return;
  }
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   CHECK_FAIL_RET_VOID(NULL != p_dev);

  //unprotect
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&(p_priv->flash_st));
  st.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st);
}

/*!
  internal flash protect
  */
static void ota_dm_api_charsto_protect_all(void)
{
  void *p_dev = NULL;

  ota_dm_priv_t *p_priv =  NULL;
  p_priv = ota_dm_api_get_priv();
  if(p_priv->dm_config.is_protect == FALSE)
  {
    return;
  }

   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   CHECK_FAIL_RET_VOID(NULL != p_dev);

  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&(p_priv->flash_st));
}

/*!
  internal flash destroy write,it only use test dm api
  */
RET_CODE ota_dm_api_charsto_destroy_write
                                                                  (u32 addr,u32 offset,
                                                                  u8 *data,u32 wr_size)
{
    void *p_dev = NULL;
    u8 *p_block_buffer = NULL;
    RET_CODE ret = SUCCESS;
    ota_dm_priv_t *p_priv =  NULL;
    p_priv = ota_dm_api_get_priv();

    if(p_priv->dm_forbid == TRUE)
    {
        ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                        "ota dm[%s] err:dm forbid!\n",
                                         __FUNCTION__);
        return ERR_FAILURE; 
    }

    if((addr != p_priv->dm_config.ota_dm_addr)
        && (addr != p_priv->dm_config.ota_dm_backup_addr))
      {
         CHECK_FAIL_RET_CODE(0);
      }
    
    CHECK_FAIL_RET_CODE(offset < CHARSTO_SECTOR_SIZE);
    CHECK_FAIL_RET_CODE((offset + wr_size)< CHARSTO_SECTOR_SIZE);
    
    p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
    if(p_block_buffer == NULL)
    {
       return ERR_FAILURE; 
    }
    
   memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   CHECK_FAIL_RET_CODE(NULL != p_dev);
    /**read  block 64kbyte one time****/
  ret = charsto_read(p_dev,addr, p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
  {
    mtos_free(p_block_buffer);
    p_block_buffer = NULL;
    return ERR_FAILURE;
  }

  memcpy(p_block_buffer + offset,data,wr_size);
  ota_dm_api_charsto_unprotect_all();
  charsto_erase(p_dev,addr, 1);
  charsto_writeonly(p_dev,addr,p_block_buffer,CHARSTO_SECTOR_SIZE);
  ota_dm_api_charsto_protect_all();
  mtos_free(p_block_buffer);
  p_block_buffer = NULL;
  return SUCCESS;
}

/*!
  internal flash piece write
  */
static RET_CODE ota_dm_api_charsto_write
                                      (piece_table_info_t *piece_info,
                                      u8 *data,u32 wr_size)
{
  ota_dm_priv_t *p_priv =  NULL;
  RET_CODE ret = SUCCESS;
  void *p_dev = NULL;
  u8 *p_block_buffer = NULL;
  u32 crc_32_read = 0;

  u8 *p_read_crc_buffer = NULL;
  u8 *p_write_crc_buffer = NULL ;
  piece_head_info_t w_head_info = {0};
  piece_head_info_t r_head_info = {0};
  u32 data_len = 0;
  u8 *p_piece_buffer = NULL;
   
   p_priv = ota_dm_api_get_priv();

   if(p_priv->dm_forbid == TRUE)
  {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                      "ota dm[%s] err:dm forbid!\n",
                                       __FUNCTION__);
      return ERR_FAILURE; 
  }

   if(piece_info->piece_size < (wr_size + sizeof(piece_head_info_t)))
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                      "ota dm[%s] err:the write size is bigger piece size!\n",
                                       __FUNCTION__);
      return ERR_FAILURE; 
    }

     p_piece_buffer = mtos_malloc(piece_info->piece_size);
    if(p_piece_buffer == NULL)
    {
       ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:malloc fail!\n",__FUNCTION__);
       return ERR_FAILURE; 
    }
    
    p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
    if(p_block_buffer == NULL)
    {
       mtos_free(p_piece_buffer);
       p_piece_buffer = NULL;
       ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:malloc fail!\n",__FUNCTION__);
       return ERR_FAILURE; 
    }
   memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   CHECK_FAIL_RET_CODE(NULL != p_dev);
   
  memset(p_piece_buffer,0xFF,piece_info->piece_size);
  data_len = wr_size;
  if(data_len + sizeof(piece_head_info_t) > piece_info->piece_size)
    {
       data_len = piece_info->piece_size - sizeof(piece_head_info_t);
    }

  w_head_info.mark = OTA_DM_PIECE_MARK;
  w_head_info.id = piece_info->piece_id;
  w_head_info.size = piece_info->piece_size - sizeof(piece_head_info_t);
  
  ota_dm_api_piece_crc_init();
  memcpy(p_piece_buffer + sizeof(piece_head_info_t),data,data_len);
  p_write_crc_buffer = p_piece_buffer + sizeof(piece_head_info_t); 
  w_head_info.crc  = ota_dm_api_piece_generate(p_write_crc_buffer,w_head_info.size);
  
  memcpy(p_piece_buffer,(u8 *)(&w_head_info),sizeof(piece_head_info_t)); /**piece head**/
  
  /**read  block 64kbyte one time****/
  ret = charsto_read(p_dev,p_priv->dm_config.ota_dm_addr, p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
  {
    mtos_free(p_piece_buffer);
    p_piece_buffer =  NULL;
    mtos_free(p_block_buffer);
    p_block_buffer = NULL;
    ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:charsto read fail!\n",
                                       __FUNCTION__);
    return ERR_FAILURE;
  }
  memset((u8 *)(&r_head_info),0,sizeof(piece_head_info_t));
  memcpy((u8 *)(&r_head_info),p_block_buffer + piece_info->piece_pos,sizeof(piece_head_info_t));
  p_read_crc_buffer = p_block_buffer + piece_info->piece_pos + sizeof(piece_head_info_t);
  ota_dm_api_piece_crc_init();
  crc_32_read = ota_dm_api_piece_generate(p_read_crc_buffer,r_head_info.size);

  /****check crc and read block*****/
   if((r_head_info.mark != OTA_DM_PIECE_MARK)
       || (r_head_info.id != piece_info->piece_id)
       || (r_head_info.crc != crc_32_read)
       || (r_head_info.crc != w_head_info.crc))
    {
        ota_dm_api_printf(OTA_DM_DEBUG_INFO,
              "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x w_crc:0x%x\n",
              __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
              r_head_info.mark,r_head_info.id,r_head_info.crc,crc_32_read,w_head_info.crc);
        memcpy(p_block_buffer + piece_info->piece_pos,p_piece_buffer,piece_info->piece_size);
        /***writw back block****/
        ota_dm_api_charsto_unprotect_all();
        charsto_erase(p_dev,p_priv->dm_config.ota_dm_addr, 1);
        charsto_writeonly(p_dev, p_priv->dm_config.ota_dm_addr,p_block_buffer,CHARSTO_SECTOR_SIZE);
        ota_dm_api_charsto_protect_all();
    }
   
   if(p_priv->dm_config.disable_backup_block == TRUE)
    {
      mtos_free(p_piece_buffer);
      p_piece_buffer =  NULL;
      mtos_free(p_block_buffer);
      p_block_buffer =  NULL;
      return SUCCESS;
    }


  /**read back up block 64kbyte one time****/
  memset(p_block_buffer,0,CHARSTO_SECTOR_SIZE);
  ret = charsto_read(p_dev,p_priv->dm_config.ota_dm_backup_addr,
            p_block_buffer,CHARSTO_SECTOR_SIZE);
  if (ret != SUCCESS)
  {
    mtos_free(p_piece_buffer);
    p_piece_buffer =  NULL;
    mtos_free(p_block_buffer);
    p_block_buffer = NULL;
    ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:charsto read fail!\n",
                                       __FUNCTION__);
    return ERR_FAILURE;
  }

  memset((u8 *)(&r_head_info),0,sizeof(piece_head_info_t));
  memcpy((u8 *)(&r_head_info),p_block_buffer + piece_info->piece_pos,sizeof(piece_head_info_t));
  p_read_crc_buffer = p_block_buffer + piece_info->piece_pos + sizeof(piece_head_info_t);
  ota_dm_api_piece_crc_init();
  crc_32_read = ota_dm_api_piece_generate(p_read_crc_buffer,r_head_info.size);
  
  if((r_head_info.mark != OTA_DM_PIECE_MARK)
    || (r_head_info.id != piece_info->piece_id)
    || (r_head_info.crc != crc_32_read)
    || (r_head_info.crc != w_head_info.crc))
    {
        ota_dm_api_printf(OTA_DM_DEBUG_INFO,
        "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x w_crc:0x%x\n",
        __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
        r_head_info.mark,r_head_info.id,r_head_info.crc,crc_32_read,w_head_info.crc);
        memcpy(p_block_buffer + piece_info->piece_pos,p_piece_buffer,piece_info->piece_size);
        /***writw back block****/
        ota_dm_api_charsto_unprotect_all();
        charsto_erase(p_dev,p_priv->dm_config.ota_dm_backup_addr, 1);
        charsto_writeonly(p_dev, p_priv->dm_config.ota_dm_backup_addr,
                                       p_block_buffer,CHARSTO_SECTOR_SIZE);
        ota_dm_api_charsto_protect_all();
    }

  mtos_free(p_piece_buffer);
  p_piece_buffer = NULL;
  mtos_free(p_block_buffer);
  p_block_buffer =  NULL;
  return SUCCESS;

}
/*!
  internal flash piece read
  */
static RET_CODE ota_dm_api_charsto_read(piece_table_info_t *piece_info,u8 *data,u32 get_size)
{
  ota_dm_priv_t *p_priv =  NULL;
  RET_CODE ret = SUCCESS;
  void *p_dev = NULL;
  u8 *p_read_buffer = NULL;

  u32 crc_32_check = 0;
  u32 read_addr = 0;
  u8 *p_read_crc_buffer = NULL;
  u8 *p_back_data = NULL;
  u32 back_data_size = 0;
  piece_head_info_t head_info = {0};
  piece_head_info_t b_head_info = {0};
  
  p_priv = ota_dm_api_get_priv();
  
  if(p_priv->dm_forbid == TRUE)
  {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                      "ota dm[%s] err:dm forbid!\n",
                                       __FUNCTION__);
      return ERR_FAILURE; 
  }
  
  memset(data,0,get_size);
  
  p_read_buffer = mtos_malloc(piece_info->piece_size);
  if(p_read_buffer == NULL)
  {
     ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:malloc fail!\n",__FUNCTION__);
     return ERR_FAILURE; 
  }

 memset(p_read_buffer,0,piece_info->piece_size);
 p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
 CHECK_FAIL_RET_CODE(NULL != p_dev);
   
 /**read one block 64kbyte one time****/
  read_addr = p_priv->dm_config.ota_dm_addr + piece_info->piece_pos;
 
  ret = charsto_read(p_dev,read_addr, p_read_buffer,piece_info->piece_size);
  if (ret != SUCCESS)
  {
    mtos_free(p_read_buffer);
    p_read_buffer =  NULL;
    ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:charsto read fail!\n",__FUNCTION__);
    return ERR_FAILURE;
  }

  memset((u8 *)(&head_info),0,sizeof(piece_head_info_t));
  memcpy((u8 *)(&head_info),p_read_buffer,sizeof(piece_head_info_t));
  
  p_read_crc_buffer = p_read_buffer + sizeof(piece_head_info_t);
  
  /****check crc and read back block*****/
  ota_dm_api_piece_crc_init();
 crc_32_check = ota_dm_api_piece_generate(p_read_crc_buffer,head_info.size);
 
 ota_dm_api_printf(OTA_DM_DEBUG_INFO,
  "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x\n",
   __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
  head_info.mark,head_info.id,head_info.crc,crc_32_check);

 if((head_info.mark == OTA_DM_PIECE_MARK)
     && (head_info.id == piece_info->piece_id)
     && (head_info.crc == crc_32_check))
  {
      memcpy(data,p_read_crc_buffer,get_size);

      /****check backup block and repair back block*****/
       if (p_priv->dm_config.disable_backup_block == FALSE)
        {
            back_data_size = head_info.size;
            p_back_data = mtos_malloc(head_info.size);
            if(p_back_data == NULL)
            {
                mtos_free(p_read_buffer);
                p_read_buffer =  NULL;
                ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                  "ota dm[%s] err:repair backup fail!\n",__FUNCTION__);
                return SUCCESS;
            }
           memcpy(p_back_data,p_read_crc_buffer,back_data_size);
            
           memset((u8 *)(&b_head_info),0,sizeof(piece_head_info_t));
           memset(p_read_buffer,0,piece_info->piece_size);
           
           read_addr = p_priv->dm_config.ota_dm_backup_addr + piece_info->piece_pos;
           ret = charsto_read(p_dev,read_addr,p_read_buffer,piece_info->piece_size);
           if (ret == SUCCESS)
            {
               memcpy((u8 *)(&head_info),p_read_buffer,sizeof(piece_head_info_t));
               p_read_crc_buffer = p_read_buffer + sizeof(piece_head_info_t);
                ota_dm_api_piece_crc_init();
                crc_32_check = ota_dm_api_piece_generate(p_read_crc_buffer,head_info.size);
           if((head_info.mark != OTA_DM_PIECE_MARK)
               || (head_info.id != piece_info->piece_id)
               || (head_info.crc != crc_32_check))
               {
                  ota_dm_api_printf(OTA_DM_DEBUG_INFO,
                    "ota dm[%s] :repait backup piece!\n",__FUNCTION__);
                  ota_dm_api_charsto_write(piece_info,p_back_data,back_data_size);
               }
            }
           mtos_free(p_back_data);
           p_back_data = NULL;
           
        }
      
      mtos_free(p_read_buffer);
      p_read_buffer = NULL;
      return SUCCESS;
  }
else if (p_priv->dm_config.disable_backup_block == FALSE)
  {
      memset(p_read_buffer,0,piece_info->piece_size);
      read_addr = p_priv->dm_config.ota_dm_backup_addr + piece_info->piece_pos;
      ret = charsto_read(p_dev,read_addr, p_read_buffer,piece_info->piece_size);
      if (ret == SUCCESS)
      {
          memset((u8 *)(&head_info),0,sizeof(piece_head_info_t));
          memcpy((u8 *)(&head_info),p_read_buffer,sizeof(piece_head_info_t));
          p_read_crc_buffer = p_read_buffer + sizeof(piece_head_info_t);

            /****check crc and read back block*****/
           ota_dm_api_piece_crc_init();
           crc_32_check = ota_dm_api_piece_generate(p_read_crc_buffer,head_info.size);
           
           ota_dm_api_printf(OTA_DM_DEBUG_INFO,
            "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x\n",
            __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
            head_info.mark,head_info.id,head_info.crc,crc_32_check);
           if((head_info.mark == OTA_DM_PIECE_MARK)
               && (head_info.id == piece_info->piece_id)
               && (head_info.crc == crc_32_check))
            {
                memcpy(data,p_read_crc_buffer,get_size);
                /****repair curr block*****/
                ota_dm_api_printf(OTA_DM_DEBUG_INFO,
                                                "ota dm[%s] :repait current piece!\n",__FUNCTION__);
                ota_dm_api_charsto_write(piece_info,p_read_crc_buffer,head_info.size);
                mtos_free(p_read_buffer);
                p_read_buffer = NULL;
                return SUCCESS;
            }
      }
  }

 //debug_data_printf(p_block_buffer+part_pos,size+4);
 mtos_free(p_read_buffer);
 p_read_buffer = NULL;
 ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:fail!\n",__FUNCTION__);
 return ERR_FAILURE;
}

/*!
  internal flash piece read,if return TRUE,it is not destory!
  */
static BOOL ota_dm_api_chech_intact_piece(piece_table_info_t *piece_info)
{
  ota_dm_priv_t *p_priv =  NULL;
  RET_CODE ret = SUCCESS;
  void *p_dev = NULL;
  u8 *p_read_buffer = NULL;

  u32 crc_32_check = 0;
  u32 read_addr = 0;
  u8 *p_read_crc_buffer = NULL;
  piece_head_info_t head_info = {0};

  BOOL piece_destory_bit = FALSE;
  BOOL piece_back_destory_bit = FALSE;

  p_priv = ota_dm_api_get_priv();

  p_read_buffer = mtos_malloc(piece_info->piece_size);
  if(p_read_buffer == NULL)
  {
     ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:malloc fail!\n",__FUNCTION__);
     return TRUE; /****return it don't break*****/
  }

 memset(p_read_buffer,0,piece_info->piece_size);
 p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
 CHECK_FAIL_RET_FALSE(NULL != p_dev);
   
 /**read one block 64kbyte one time****/
  read_addr = p_priv->dm_config.ota_dm_addr + piece_info->piece_pos;
 
  ret = charsto_read(p_dev,read_addr, p_read_buffer,piece_info->piece_size);
  if (ret != SUCCESS)
  {
    mtos_free(p_read_buffer);
    p_read_buffer = NULL;
    ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:charsto read fail!\n",__FUNCTION__);
    return TRUE; /****return it don't break*****/
  }

  memset((u8 *)(&head_info),0,sizeof(piece_head_info_t));
  memcpy((u8 *)(&head_info),p_read_buffer,sizeof(piece_head_info_t));
  
  p_read_crc_buffer = p_read_buffer + sizeof(piece_head_info_t);
  
  /****check crc and read back block*****/
  ota_dm_api_piece_crc_init();
 crc_32_check = ota_dm_api_piece_generate(p_read_crc_buffer,head_info.size);
 
 ota_dm_api_printf(OTA_DM_DEBUG_INFO,
    "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x\n",
    __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
    head_info.mark,head_info.id,head_info.crc,crc_32_check);
 
 if((head_info.mark == OTA_DM_PIECE_MARK)
     && (head_info.id == piece_info->piece_id)
     && (head_info.crc == crc_32_check))
  {
       piece_destory_bit = FALSE;
  }
  else
  {
     ota_dm_api_printf(OTA_DM_DEBUG_INFO,
                                      "ota dm[%s]:piece [%d]is broken\n",
                                      __FUNCTION__,piece_info->piece_id);
     piece_destory_bit = TRUE;
  }

 if (p_priv->dm_config.disable_backup_block == FALSE)
  {
      memset(p_read_buffer,0,piece_info->piece_size);
      read_addr = p_priv->dm_config.ota_dm_backup_addr + piece_info->piece_pos;
      ret = charsto_read(p_dev,read_addr, p_read_buffer,piece_info->piece_size);
      if (ret == SUCCESS)
      {
          memset((u8 *)(&head_info),0,sizeof(piece_head_info_t));
          memcpy((u8 *)(&head_info),p_read_buffer,sizeof(piece_head_info_t));
          p_read_crc_buffer = p_read_buffer + sizeof(piece_head_info_t);

            /****check crc and read back block*****/
           ota_dm_api_piece_crc_init();
           crc_32_check = ota_dm_api_piece_generate(p_read_crc_buffer,head_info.size);
           
           ota_dm_api_printf(OTA_DM_DEBUG_INFO,
            "ota dm[%s] head_info[%d][%d]:mark:0x%x;id:0x%x,crc:0x%x check_crc:0x%x\n",
            __FUNCTION__,piece_info->piece_id,piece_info->piece_pos,
            head_info.mark,head_info.id,head_info.crc,crc_32_check);
           if((head_info.mark == OTA_DM_PIECE_MARK)
               && (head_info.id == piece_info->piece_id)
               && (head_info.crc == crc_32_check))
            {
               piece_back_destory_bit = FALSE;
            }
           else
            {
               ota_dm_api_printf(OTA_DM_DEBUG_INFO,
                                                "ota dm[%s]:back piece [%d]is broken\n",
                                                __FUNCTION__,piece_info->piece_id);
               piece_back_destory_bit = TRUE;
            }
      }
  }

 //debug_data_printf(p_block_buffer+part_pos,size+4);
 mtos_free(p_read_buffer);
 p_read_buffer =  NULL;
 

if(p_priv->dm_config.disable_backup_block == FALSE)
{
  if((piece_destory_bit == TRUE) && (piece_back_destory_bit == TRUE))
    {
       return FALSE;
    }
}
else
{
     if(piece_destory_bit == TRUE) 
    {
       return FALSE;
    }
}
 
 return TRUE;
}

/*!
  internal piece table
  */
static u8 ota_dm_api_get_piece_table_index(u8 piece_id)
{
    ota_dm_priv_t *p_priv =  NULL;
    u8 i = 0;
    p_priv = ota_dm_api_get_priv();

    for(i = 0;i < p_priv->piece_table_num;i++)
    {
      if(p_priv->piece_table[i].piece_id == piece_id)
        {
          return i;
        }
    }
    return 0xff;
}

/*!
  internal get piece total size
  */
static u32 ota_dm_api_get_piece_total_size(void)
{
    ota_dm_priv_t *p_priv =  NULL;
    u8 i = 0;
    u32 total_size = 0;
    p_priv = ota_dm_api_get_priv();
    for(i = 0;i < p_priv->piece_table_num;i++)
    {
      total_size += p_priv->piece_table[i].piece_size;
    }
    
    return total_size;
}

/*!
  ota data manage api proting:reset data block
  !!!!! \O/  note: don't use this function,but factory ota!!!!!
  !!!!!  /\   !!!!!
  */
void mul_ota_dm_api_reset_data_block(void)
{
    ota_dm_priv_t *p_priv =  NULL;
    void *p_dev = NULL;
    p_priv = ota_dm_api_get_priv();
    if(p_priv->dm_forbid == TRUE)
    {
        ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                        "ota dm[%s] err:dm forbid!\n",
                                         __FUNCTION__);
        return; 
    }
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    CHECK_FAIL_RET_VOID(NULL != p_dev);
    ota_dm_api_lock(p_priv);
    ota_dm_api_charsto_unprotect_all();
    charsto_erase(p_dev,p_priv->dm_config.ota_dm_addr, 1);
    charsto_erase(p_dev,p_priv->dm_config.ota_dm_backup_addr, 1);
    ota_dm_api_charsto_protect_all();
    ota_dm_api_unlock(p_priv);
}
/*!
  ota data manage api proting:check intata
  input param 1:peice id
  return TRUE or FALSE,note:it is FALSE,it is destoried!need to init picec!
  */
BOOL mul_ota_dm_api_check_intact_picec(u32 piece_id)
{
   ota_dm_priv_t *p_priv =  NULL;
   u8 index = 0;
   BOOL ret = 0;
   piece_table_info_t piece_info = {0};

   p_priv = ota_dm_api_get_priv();
   index = ota_dm_api_get_piece_table_index((u8)piece_id);
   if(index == 0xff)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ASSERT,
                                        "ota dm[%s] err:piece id no exist!\n",
                                        __FUNCTION__,piece_id);
      CHECK_FAIL_RET_FALSE(0); /****don't move it***/
      return FALSE;
    }
   memcpy(&piece_info,&p_priv->piece_table[index],sizeof(piece_table_info_t));
   ota_dm_api_lock(p_priv);
   ret = ota_dm_api_chech_intact_piece(&piece_info);
   ota_dm_api_unlock(p_priv);
   return ret;
}

/*!
  ota data manage api proting:read data
  input param 1:peice id
  input param 2:read data buffer
  input param 3:read data size
  return ERR or SUCCESS
  */
RET_CODE mul_ota_dm_api_read_data(u32 piece_id,u8 *p_data, u32 rd_size)
{
   ota_dm_priv_t *p_priv =  NULL;
   u8 index = 0;
   RET_CODE ret = 0;
   piece_table_info_t piece_info = {0};
   
   p_priv = ota_dm_api_get_priv();
   index = ota_dm_api_get_piece_table_index((u8)piece_id);
   if(index == 0xff)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                       "ota dm[%s] err:piece id no exist!\n",
                                        __FUNCTION__,piece_id);
      return ERR_FAILURE;
    }
   memcpy(&piece_info,&p_priv->piece_table[index],sizeof(piece_table_info_t));
   ota_dm_api_lock(p_priv);
   ret = ota_dm_api_charsto_read(&piece_info,p_data,rd_size);
   ota_dm_api_unlock(p_priv);
   return ret;
}
/*!
  ota data manage api proting:write data
  input param 1:peice id
  input param 2:write data buffer
  input param 3:write  data size
  return ERR or SUCCESS
  */
RET_CODE mul_ota_dm_api_write_data(u32 piece_id,u8 *p_data, u32 wr_size)
{
   ota_dm_priv_t *p_priv =  NULL;
   u8 index = 0;
   RET_CODE ret = ERR_FAILURE;
   piece_table_info_t piece_info = {0};
   
   p_priv = ota_dm_api_get_priv();
   index = ota_dm_api_get_piece_table_index((u8)piece_id);
   if(index == 0xff)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                      "ota dm[%s] err:piece id no exist!\n",
                                       __FUNCTION__,piece_id);
      return ERR_FAILURE;
    }
   memcpy(&piece_info,&p_priv->piece_table[index],sizeof(piece_table_info_t));
   ota_dm_api_lock(p_priv);
   ret = ota_dm_api_charsto_write(&piece_info,p_data,wr_size);
   ota_dm_api_unlock(p_priv);
   return ret;
}

/*!
  ota data manage api proting:init boot load save info
  input param 1:info pointer
  */
void mul_ota_dm_api_init_bootload_info(ota_bl_info_t *p_bl_info)
{
   ota_dm_priv_t *p_priv =  NULL;
   p_priv = ota_dm_api_get_priv();
   if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
    {
      mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID,
                                                        (u8 *)p_bl_info,sizeof(ota_bl_info_t));
    }
}

/*!
  ota data manage api proting:read boot load  info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_bootload_info(ota_bl_info_t *p_bl_info)
{
   ota_dm_priv_t *p_priv =  NULL;
   RET_CODE ret = ERR_FAILURE;
   
   p_priv = ota_dm_api_get_priv();
   ret = mul_ota_dm_api_read_data(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID,
                    (u8 *)p_bl_info,sizeof(ota_bl_info_t));
   if(ret == ERR_FAILURE)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                      "ota dm[%s] err:read data err,it need init\n",
                                      __FUNCTION__);
      memset(p_bl_info,0,sizeof(ota_bl_info_t));
    }
}

/*!
  ota data manage api proting:save boot load  info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_bootload_info(ota_bl_info_t *p_bl_info)
{
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID,
                                                         (u8 *)p_bl_info,sizeof(ota_bl_info_t));
}

/*!
  ota data manage api proting:save boot load  info:destroy flag
  input param 1:flag
  */
void mul_ota_dm_api_save_flash_destroy_flag(BOOL destroy_flag)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.destroy_flag = destroy_flag;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}
/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:flag
  */
BOOL  mul_ota_dm_api_read_flash_destroy_flag(void)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  return  bl_info.destroy_flag;
}

/*!
  ota data manage api proting:save boot load  info:destroy flag
  input param 1:flag
  */
void mul_ota_dm_api_save_flash_destroy_block_id(u32 block_id)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.destroy_block_id = block_id;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}


/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:flag
  */
u32  mul_ota_dm_api_read_flash_destroy_block_id(void)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  return  bl_info.destroy_block_id;
}

/*!
  ota data manage api proting:save boot load  info:load block id
  input param 1:flag
  */
void mul_ota_dm_api_save_load_block_id(u32 block_id)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.load_block_id = block_id;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}

/*!
  ota data manage api proting:read boot load  info:load block id
  return:flag
  */
u32  mul_ota_dm_api_read_load_block_id(void)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  return  bl_info.load_block_id;
}

/*!
  ota data manage api proting:save boot load  info:ota status
  input param 1:status
  */
void mul_ota_dm_api_save_ota_status(u32 status)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.ota_status = status;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}
/*!
  ota data manage api proting:read boot load  info:destroy flag
  return:status
  */
u32  mul_ota_dm_api_read_ota_status(void)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  return  bl_info.ota_status;
}


/*!
  ota data manage api proting:save boot load  info:ota fail time
  input param 1:fail time
  */
void mul_ota_dm_api_save_ota_fail_times(u32 times)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.fail_times = times;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}
/*!
  ota data manage api proting:read boot load  info:fail time
  return:fail time
  */
u32  mul_ota_dm_api_read_ota_fail_times(void)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  return  bl_info.fail_times;
}

/*!
  ota data manage api proting:save boot load  info:ota block id
  input param 1:fail time
  */
void mul_ota_dm_api_save_ota_block_id(u32 current_id,u32 back_up_id)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  bl_info.ota_curr_id = current_id;
  bl_info.ota_backup_id = back_up_id;
  mul_ota_dm_api_save_bootload_info(&bl_info);
}
/*!
  ota data manage api proting:read boot load  info:ota block id
  return:fail time
  */
void  mul_ota_dm_api_read_ota_block_id(u32 *p_current_id,u32 *p_back_up_id)
{
  ota_bl_info_t bl_info = {0};
  mul_ota_dm_api_read_bootload_info(&bl_info);
  *p_current_id = bl_info.ota_curr_id;
  *p_back_up_id = bl_info.ota_backup_id;
}



/*!
  ota data manage api proting:init ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_init_ota_param(ota_param_t *p_param)
{
   ota_dm_priv_t *p_priv =  NULL;
   p_priv = ota_dm_api_get_priv();
   if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID) == FALSE)
  {
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                         (u8 *)p_param,sizeof(ota_param_t));
  }
}
/*!
  ota data manage api proting:read ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_ota_param(ota_param_t *p_param)
{
   ota_dm_priv_t *p_priv =  NULL;
   RET_CODE ret = ERR_FAILURE;
   p_priv = ota_dm_api_get_priv();
   ret = mul_ota_dm_api_read_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                            (u8 *)p_param,sizeof(ota_param_t));
   if(ret == ERR_FAILURE)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,
                                        "ota dm[%s] err:read data err,it need init!\n",
                                     __FUNCTION__);
      memset(p_param,0,sizeof(ota_param_t));
    }
}

/*!
  ota data manage api proting:save ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param(ota_param_t *p_param)
{
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                       (u8 *)p_param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:save ota param upg version
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param_upg_version(u32 version)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    param.upg_version = version;
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                       (u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read ota param upg version
  input param 1:info pointer
  */
u32 mul_ota_dm_api_read_ota_param_upg_version(void)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    return param.upg_version;
}

/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_ota_param_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(&param.ota_tp,p_tp,sizeof(nim_para_t));
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                      (u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_ota_param_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(p_tp,&param.ota_tp,sizeof(nim_para_t));
}


/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_set_ota_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(&param.ota_set_tp,p_tp,sizeof(nim_para_t));
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                             (u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_set_ota_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(p_tp,&param.ota_set_tp,sizeof(nim_para_t));
}

/*!
  ota data manage api proting:save ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_save_set_backup_ota_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(&param.ota_set_back_tp,p_tp,sizeof(nim_para_t));
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                            (u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read ota param info tp
  input param 1:info pointer
  */
void mul_ota_dm_api_read_set_backup_ota_tp(nim_para_t *p_tp)
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(p_tp,&param.ota_set_back_tp,sizeof(nim_para_t));
}


/*!
  ota data manage api proting:save ota param block info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_all_block_info(
                            block_ota_info_t block_table[OTA_DM_MAX_BLOCK_NUMBER])
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(param.block_table,block_table,
                   sizeof(block_ota_info_t) * OTA_DM_MAX_BLOCK_NUMBER);
    mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,
                                                      (u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read ota param block info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_all_block_info(
      block_ota_info_t block_table[OTA_DM_MAX_BLOCK_NUMBER])
{
    ota_param_t param;
    mul_ota_dm_api_read_ota_param(&param);
    memcpy(block_table,param.block_table,sizeof(block_ota_info_t) * OTA_DM_MAX_BLOCK_NUMBER);
}

/*!
  ota data manage api proting:save one block ota param info
  input param 1:info pointer
  */
void mul_ota_dm_api_save_one_block_info(u8 block_id,block_ota_info_t *p_block_info)
{
    ota_param_t param;
    u8 i = 0;
    mul_ota_dm_api_read_ota_param(&param);
    /****find exist block******/
    for(i = 0;i < OTA_DM_MAX_BLOCK_NUMBER;i++)
    {
      if((param.block_table[i].id == block_id)
         &&(param.block_table[i].exist_bit == TRUE))
        {
          break;
        }
    }
    
    if(i >= OTA_DM_MAX_BLOCK_NUMBER)
    {
      /****find no user  block******/
      for(i = 0;i < OTA_DM_MAX_BLOCK_NUMBER;i++)
      {
        if(param.block_table[i].exist_bit != TRUE)
          {
            break;
          }
      }
    }
   CHECK_FAIL_RET_VOID(i < OTA_DM_MAX_BLOCK_NUMBER);

   memcpy(&param.block_table[i],p_block_info,sizeof(block_ota_info_t));
   param.block_table[i].exist_bit = TRUE;
   mul_ota_dm_api_write_data(OTA_DM_BLOCK_PIECE_OTA_PARAM_ID,(u8 *)&param,sizeof(ota_param_t));
}

/*!
  ota data manage api proting:read one block info
  input param 1:info pointer
  */
void mul_ota_dm_api_read_one_block_info(u8 block_id,block_ota_info_t *p_block_info)
{
    ota_param_t param;
    u8 i = 0;
    mul_ota_dm_api_read_ota_param(&param);
    /****find exist block******/
    for(i = 0;i < OTA_DM_MAX_BLOCK_NUMBER;i++)
    {
      if((param.block_table[i].id == block_id)
         &&(param.block_table[i].exist_bit == TRUE))
        {
          break;
        }
    }
    if(i >= OTA_DM_MAX_BLOCK_NUMBER)
    {
      ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"ota dm[%s] err:block id:%d no exist!\n",
        __FUNCTION__,block_id);
      memset(p_block_info,0,sizeof(block_ota_info_t));
    }
    else
    {
        memcpy(p_block_info,&param.block_table[i],sizeof(block_ota_info_t));
    }
}



/*!
  ota data manage api proting:register piece
  note:piece id dn't repeat;the total size dn't biger block size
  */
void mul_ota_dm_api_register_piece(piece_register_t *p_para)
{
    ota_dm_priv_t *p_priv =  NULL;
    u8 index = 0;
    u32 piece_size = 0;
    u32 total_size = 0;
    u32 align_size = 0;

    p_priv = ota_dm_api_get_priv();
    
    align_size = p_priv->dm_config.align_size;
    
    if(align_size < 16)
    {
       align_size = 16;
    }
        
    if(align_size > PICE_MAX_ALIGN_SIZE)
    {
       align_size = PICE_MAX_ALIGN_SIZE;
    }

    
    /***find exist id***/
    index = ota_dm_api_get_piece_table_index(p_para->piece_id);
    CHECK_FAIL_RET_VOID(index == 0XFF);
    
    /***count piece total size***/
    total_size =  ota_dm_api_get_piece_total_size();
    p_para->piece_size += sizeof(piece_head_info_t);
    piece_size = ((p_para->piece_size + (align_size - 1)) / align_size) * align_size; 
    
    CHECK_FAIL_RET_VOID((total_size + piece_size) < CHARSTO_SECTOR_SIZE);

   p_priv->piece_table[p_priv->piece_table_num].piece_id = p_para->piece_id;
   p_priv->piece_table[p_priv->piece_table_num].piece_pos = total_size;
   p_priv->piece_table[p_priv->piece_table_num].piece_size = piece_size;

ota_dm_api_printf(OTA_DM_DEBUG_INFO,"ota dm piece table[%d].id:0x%x\n",
      p_priv->piece_table_num,p_priv->piece_table[p_priv->piece_table_num].piece_id);
ota_dm_api_printf(OTA_DM_DEBUG_INFO,"ota dm piece table[%d].pos:0x%x\n",
      p_priv->piece_table_num,p_priv->piece_table[p_priv->piece_table_num].piece_pos);
ota_dm_api_printf(OTA_DM_DEBUG_INFO,"ota dm piece table[%d].size:0x%x\n",
      p_priv->piece_table_num,p_priv->piece_table[p_priv->piece_table_num].piece_size);

   p_priv->piece_table_num ++;
   
}

/*!
  ota data manage api proting:api init
  note:piece id dn't repeat
  */
RET_CODE mul_ota_dm_api_init(ota_dm_config_t *p_cfg)
{

  ota_dm_priv_t *p_ota_dm_priv = NULL;
  piece_register_t register_para = {0};
  BOOL ret = FALSE;
  u32 block_size = 0;
  u32 block_addr = 0;
  dm_dmh_info_t *p_dmh_info =  NULL;
  u8 i = 0,j = 0;
  
  if(p_ota_dm_priv == NULL)
  {
    p_ota_dm_priv  = (ota_dm_priv_t *)mtos_malloc(sizeof(ota_dm_priv_t));
  }  
  CHECK_FAIL_RET_CODE(p_ota_dm_priv != NULL);
  memset(p_ota_dm_priv,0, sizeof(ota_dm_priv_t));
  
  p_ota_dm_priv->p_dmh_info = 
       (dm_dmh_info_t *)mtos_malloc(sizeof(dm_dmh_info_t));
  CHECK_FAIL_RET_CODE(p_ota_dm_priv->p_dmh_info != NULL);
  memset(p_ota_dm_priv->p_dmh_info,0, sizeof(dm_dmh_info_t));

  ret = mul_ota_dm_api_find_dmh_info(0,0,
          p_cfg->flash_size,
          p_ota_dm_priv->p_dmh_info);
  //CHECK_FAIL_RET_CODE(ret == TRUE);

  CHECK_FAIL_RET_CODE(p_cfg->pp_dm_info != NULL);

  *p_cfg->pp_dm_info = p_ota_dm_priv->p_dmh_info;
  p_dmh_info =  p_ota_dm_priv->p_dmh_info;
  p_ota_dm_priv->dm_forbid = FALSE;
  
  class_register(OTA_DM_CLASS_ID,(void *)p_ota_dm_priv);

  memcpy(&p_ota_dm_priv->dm_config,p_cfg,sizeof(ota_dm_config_t));
  ota_dm_api_printf(OTA_DM_DEBUG_INFO,"******ota dm api init******\n");
  ota_dm_api_printf(OTA_DM_DEBUG_INFO,"******ota dm data info******\n");
  for(i = 0;i < p_dmh_info->header_num;i ++)
  {
     for(j = 0; j < p_dmh_info->header_group[i].block_num; j++)
      {
        ota_dm_api_printf(OTA_DM_DEBUG_INFO,"block[0x%x].addr:0x%x ---size: 0x%x\n",
                                p_dmh_info->header_group[i].dm_block[j].id,
                                p_dmh_info->header_group[i].dm_block[j].base_addr,
                                p_dmh_info->header_group[i].dm_block[j].size);
      }
   }
  
  block_addr = mul_ota_dm_api_get_block_addr(p_cfg->ota_dm_block_id);
  block_size = mul_ota_dm_api_get_block_size(p_cfg->ota_dm_block_id);
  if(((block_addr % CHARSTO_SECTOR_SIZE) == 0)
  && ((block_size % CHARSTO_SECTOR_SIZE) == 0)
  && (block_addr != 0)
  && (block_size != 0))
  {
    p_ota_dm_priv->dm_config.ota_dm_addr = block_addr;
    ota_dm_api_printf(OTA_DM_DEBUG_INFO,"ota dm addr:0x%x\n",block_addr);
  }
 else
  {
    CHECK_FAIL_RET_CODE(0);
  }
  

  if(p_ota_dm_priv->dm_config.disable_backup_block == FALSE)
  {
      block_addr = mul_ota_dm_api_get_block_addr(p_cfg->ota_dm_backup_block_id);
      block_size = mul_ota_dm_api_get_block_size(p_cfg->ota_dm_backup_block_id);
      if(((block_addr % CHARSTO_SECTOR_SIZE) == 0)
      && ((block_size % CHARSTO_SECTOR_SIZE) == 0)
      && (block_addr != 0)
      && (block_size != 0))
      {
        p_ota_dm_priv->dm_config.ota_dm_backup_addr = block_addr;
        ota_dm_api_printf(OTA_DM_DEBUG_INFO,"ota dm backup addr:0x%x\n",block_addr);
      }
     else
      {
        CHECK_FAIL_RET_CODE(0);
      }
  }
  
   if(p_ota_dm_priv->dm_config.is_use_mutex)
  {
    p_ota_dm_priv->dm_config.is_use_mutex = TRUE;
    p_ota_dm_priv->p_mutex = mtos_mutex_create(p_ota_dm_priv->dm_config.mutex_prio);
    CHECK_FAIL_RET_CODE(p_ota_dm_priv->p_mutex != NULL);
  }
  else
  {
    p_ota_dm_priv->dm_config.is_use_mutex = FALSE;
    p_ota_dm_priv->p_mutex = NULL;
  }

  /***register boot load info piece****/
  register_para.piece_id = OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID;
  register_para.piece_size = sizeof(ota_bl_info_t);
  mul_ota_dm_api_register_piece(&register_para);

  /***register ota info piece****/
  register_para.piece_id = OTA_DM_BLOCK_PIECE_OTA_PARAM_ID;
  register_para.piece_size = sizeof(ota_param_t);
  mul_ota_dm_api_register_piece(&register_para);

  return SUCCESS;
}

/*!
  ota data manage api proting:destroy
  */
RET_CODE mul_ota_dm_api_destroy(void)
{
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  if(p_ota_dm_priv->dm_config.is_use_mutex)
  {
    CHECK_FAIL_RET_CODE(p_ota_dm_priv->p_mutex != NULL);
    mtos_mutex_delete(p_ota_dm_priv->p_mutex);
  }
  mtos_free(p_ota_dm_priv->p_dmh_info);

  mtos_free(p_ota_dm_priv);
  class_register(OTA_DM_CLASS_ID,(void *)NULL);
  return SUCCESS;
}

/*!
  ota data manage api proting:forbid
  */
void mul_ota_dm_api_forbid(void)
{
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  p_ota_dm_priv->dm_forbid = TRUE;
}

/*!
  ota data manage api proting:un forbid
  */
void mul_ota_dm_api_unforbid(void)
{
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  p_ota_dm_priv->dm_forbid = FALSE;
}

/*!
 ota find dm header info.the function don't bind thi ota data manange,it only suport function api
  input param1: dm start pos distance buf,
  input param2:buffer,
  input param3,buffer size
  input param4,dmh info
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_find_dmh_info(u32 flash_addr,
                    u32 dm_offset,
                    u32 buf_size,
                    dm_dmh_info_t *p_dmh_info)
{
   u32 i = 0,j = 0,k = 0;
   u32 col_setup = 0x10000;
   u8 dmh_mark[OTA_DM_DMH_MARK_SIZE + 1] = "*^_^*DM(^o^)";
   u32 col_total = buf_size / col_setup;
   u32 row_total = 1024; /**1KBYTE**/ 
   u8 *p_header_start = NULL;
   u8 *p_block_start = NULL;
   u8 group_total = 0;
   u32 offset = 0;
   u8 *p_flash_buf =  NULL;
   BOOL ret = SUCCESS;
   void *p_dev = NULL;
   p_flash_buf = mtos_malloc(row_total);
  for(i = 0; i < col_total; i ++)
  {
      memset(p_flash_buf,0,row_total);
      p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
      CHECK_FAIL_RET_FALSE(NULL != p_dev);
      ret = charsto_read(p_dev,flash_addr + i * col_setup, p_flash_buf,row_total);
      if (ret != SUCCESS)
      {
        mtos_free(p_flash_buf); 
        return FALSE;
      }
  
      for(j = 0;j < row_total;j ++)
      {
          if(0 == memcmp(p_flash_buf + j, dmh_mark, OTA_DM_DMH_MARK_SIZE))
           {
             /***dmh tag is 12 + 4****/
             p_header_start = p_flash_buf + j + OTA_DM_DMH_TAG_TOTAL_SIZE;
             
             p_dmh_info->header_group[group_total].dmh_start_pos = (col_setup * i)+ j;
             p_dmh_info->header_group[group_total].sdram_size = 
              MT_MAKE_DWORD(\
              MAKE_WORD(p_header_start[0], p_header_start[1]), 
              MAKE_WORD(p_header_start[2], p_header_start[3]));
             p_dmh_info->header_group[group_total].flash_size = 
              MT_MAKE_DWORD(\
              MAKE_WORD(p_header_start[4], p_header_start[5]), 
              MAKE_WORD(p_header_start[6], p_header_start[7]));
             p_dmh_info->header_group[group_total].block_num = 
              MAKE_WORD(p_header_start[8], p_header_start[9]);
             p_dmh_info->header_group[group_total].bh_size = 
              MAKE_WORD(p_header_start[10], p_header_start[11]);

             p_dmh_info->header_group[group_total].dm_offset = 
                p_dmh_info->header_group[group_total].dmh_start_pos 
                + dm_offset;
             
             if(p_dmh_info->header_group[group_total].block_num > OTA_DM_MAX_BLOCK_NUMBER)
              {
                mtos_free(p_flash_buf); 
                return FALSE;
              }
             for(k = 0; k < p_dmh_info->header_group[group_total].block_num;k++)
              {
                    p_block_start = p_header_start + 
                         OTA_DM_DMH_BASE_INFO_LEN + 
                    (k * p_dmh_info->header_group[group_total].bh_size);

                    p_dmh_info->header_group[group_total].dm_block[k].id = p_block_start[0];
                    p_dmh_info->header_group[group_total].dm_block[k].type = p_block_start[1];
                    p_dmh_info->header_group[group_total].dm_block[k].base_addr = 
                    MT_MAKE_DWORD(\
                    MAKE_WORD(p_block_start[4], p_block_start[5]), 
                    MAKE_WORD(p_block_start[6], p_block_start[7]));
                    p_dmh_info->header_group[group_total].dm_block[k].size = 
                    MT_MAKE_DWORD(\
                    MAKE_WORD(p_block_start[8], p_block_start[9]), 
                    MAKE_WORD(p_block_start[10], p_block_start[11]));
                    p_dmh_info->header_group[group_total].dm_block[k].crc = 
                    MT_MAKE_DWORD(\
                    MAKE_WORD(p_block_start[12], p_block_start[13]), 
                    MAKE_WORD(p_block_start[14], p_block_start[15]));
                   offset = p_dmh_info->header_group[group_total].dmh_start_pos + dm_offset;
                   p_dmh_info->header_group[group_total].dm_block[k].base_addr += offset;

                  memcpy(p_dmh_info->header_group[group_total].dm_block[k].version,
                                    &p_block_start[16],8);
                  memcpy(p_dmh_info->header_group[group_total].dm_block[k].name,
                                   &p_block_start[24],8);
                  memcpy(p_dmh_info->header_group[group_total].dm_block[k].time,
                                    &p_block_start[32],12);
                  p_dmh_info->header_group[group_total].dm_block[k].reserve = 
                  MT_MAKE_DWORD(MAKE_WORD(p_block_start[44], p_block_start[45]), \
                                               MAKE_WORD(p_block_start[46], p_block_start[47]));
              
              }
             group_total ++;
             if(group_total > OTA_DM_DMH_MAX_GROUP_NUMBER)
              {
                 mtos_free(p_flash_buf); 
                 return FALSE;
              }
             p_dmh_info->header_num = group_total;
             break;
           }
      } 
  }

  if(p_dmh_info->header_num == 0)
    {
       mtos_free(p_flash_buf); 
      return FALSE;
    }
  mtos_free(p_flash_buf); 
  return TRUE;
}
     
/*!
  ota find dm header info from flash.the function don't bind thi ota data manange,it only suport function api
  input param1: flash addr,
  input param2:flash size,
  input param3,info point
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_find_dmh_info_from_flash(u32 flash_addr,
                    u32 flash_size,
                    dm_dmh_info_t *p_dmh_info)
{
  BOOL ret = SUCCESS;
  ret = mul_ota_dm_api_find_dmh_info(flash_addr,0,flash_size,p_dmh_info);
  return ret;
}

/*!
  get dm block real info.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_get_block_real_info(u8 block_id,block_hdr_info_t *p_block_info)
{
  void *p_dev = NULL;
  u32 block_addr = 0;
  u32 block_size = 0;
  u32 wr_addr = 0;
  RET_CODE ret = ERR_NOFEATURE;
  block_hdr_info_t real_dm_info = {0};
  block_hdr_info_t block_dm_info = {0};
  u16 bisize = sizeof(block_hdr_info_t);
  u32 bl_dm_read_len = sizeof(u8)*64;
  u8 blh_indicator[12] = "@~_~BI~_~@";
  u8 read_buffer[64] = {0}; /***block info at block end****/
  
  
  block_addr = mul_ota_dm_api_get_block_addr(block_id);
  block_size = mul_ota_dm_api_get_block_size(block_id);
  if((block_dm_info.type == 0) 
       && ((block_addr %  CHARSTO_SECTOR_SIZE) == 0)
       && ((block_size %  CHARSTO_SECTOR_SIZE) == 0))
   {
        p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
        CHECK_FAIL_RET_FALSE(NULL != p_dev); 
        wr_addr = block_addr + block_size - bl_dm_read_len;
        ret = charsto_read(p_dev,wr_addr,read_buffer,bl_dm_read_len);
        if((ret == SUCCESS))
        {
             if(0 == memcmp(read_buffer, blh_indicator,12))
              {
                 memcpy(&real_dm_info,read_buffer + bl_dm_read_len - bisize,bisize);
                 if(block_id == real_dm_info.id)
                  {
                     memcpy(p_block_info,&real_dm_info,sizeof(block_hdr_info_t));
                     return TRUE;
                  }
              }
        }
   }

   return mul_ota_dm_api_get_block_info(block_id,p_block_info);

  //return FALSE;
  
}

/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_file_size(u8 block_id)
{
    block_hdr_info_t real_dm_info = {0};
    BOOL ret = TRUE;
    ret = mul_ota_dm_api_get_block_real_info(block_id,&real_dm_info);
    if(ret == TRUE)
    {   
        return real_dm_info.size;
    }
    return 0; 
}

/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_reserve(u8 block_id)
{
    block_hdr_info_t real_dm_info = {0};
    BOOL ret = TRUE;
    ret = mul_ota_dm_api_get_block_real_info(block_id,&real_dm_info);
    if(ret == TRUE)
    {   
        return real_dm_info.reserve;
    }
    return 0; 
}

/*!
  get dm block real info file size.the function don't bind thi ota data manange,it only suport function api
  input param1: block id,
  input param2: block info,
  return TRUE,SUCCESS;
  */
u32 mul_ota_dm_api_get_block_real_crc(u8 block_id)
{
    block_hdr_info_t real_dm_info = {0};
    BOOL ret = TRUE;
    ret = mul_ota_dm_api_get_block_real_info(block_id,&real_dm_info);
    if(ret == TRUE)
    {   
        return real_dm_info.crc;
    }
    return 0; 
}

/*!
  get block addr
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_get_block_addr(u8 block_id)
{
  u8 i = 0,j = 0;
  dm_dmh_info_t *p_dmh_info = NULL;
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  p_dmh_info = p_ota_dm_priv->p_dmh_info;
  
  for(i = 0;i < p_dmh_info->header_num;i ++)
  {
     for(j = 0; j < p_dmh_info->header_group[i].block_num; j++)
      {
        if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
          {
            return p_dmh_info->header_group[i].dm_block[j].base_addr;
          }
      }
   }

  ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"don't get block addr:0x%x\n",block_id);
  return 0;
}

/*!
  get block size
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_get_block_size(u8 block_id)
{
  u8 i = 0,j = 0;
  dm_dmh_info_t *p_dmh_info = NULL;
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  p_dmh_info = p_ota_dm_priv->p_dmh_info;
  
  for(i = 0;i < p_dmh_info->header_num;i ++)
  {
     for(j = 0; j < p_dmh_info->header_group[i].block_num; j++)
      {
        if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
          {
            return p_dmh_info->header_group[i].dm_block[j].size;
          }
      }
   }

  ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"don't get block size:0x%x\n",block_id);
  return 0;
}

/*!
  get block info
  \return 0 if fail, or addr if success
  */
BOOL mul_ota_dm_api_get_block_info(u8 block_id,block_hdr_info_t *p_info)
{
  u8 i = 0,j = 0;
  dm_dmh_info_t *p_dmh_info = NULL;
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  p_dmh_info = p_ota_dm_priv->p_dmh_info;
  
  for(i = 0;i < p_dmh_info->header_num;i ++)
  {
     for(j = 0; j < p_dmh_info->header_group[i].block_num; j++)
      {
        if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
          {
            memcpy(p_info,
              &p_dmh_info->header_group[i].dm_block[j],
              sizeof(block_hdr_info_t));
            return TRUE;
          }
      }
   }
  return FALSE;
}

/*!
  get block data
  \return 0 if fail, or addr if success
  */
u32 mul_ota_dm_api_dm_read(u8 block_id,u16 offset,
                          u32 length, u8 *p_buffer)
{
  void *p_dev = NULL;
  u32 read_addr = 0;
  u32 read_size = 0;
   RET_CODE ret = SUCCESS;
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  CHECK_FAIL_RET_ZERO(NULL != p_dev);
 

  read_addr = mul_ota_dm_api_get_block_addr(block_id);
  read_size = mul_ota_dm_api_get_block_size(block_id);
  if((read_addr == 0)
     && (read_size == 0))
    {
      return 0;
    }
  ret = charsto_read(p_dev,read_addr + offset, p_buffer,length);
  if (ret == SUCCESS)
  {
    return length;
  }
  ota_dm_api_printf(OTA_DM_DEBUG_ERROR,"don't get block data:0x%x\n",block_id);
  return 0;
}
/*!
  get dm head info
  return false if fail, maby no init ota dm
  */
BOOL mul_ota_dm_api_get_dmh_info(dm_dmh_info_t *p_dmh_info)
{
  ota_dm_priv_t *p_ota_dm_priv = NULL;
  p_ota_dm_priv = ota_dm_api_get_priv();
  if(NULL != p_ota_dm_priv->p_dmh_info)
  {
     memcpy(p_dmh_info,p_ota_dm_priv->p_dmh_info,sizeof(dm_dmh_info_t));
     return TRUE;
  }
  else
  {
    return FALSE;
  }
  //return FALSE;
}

