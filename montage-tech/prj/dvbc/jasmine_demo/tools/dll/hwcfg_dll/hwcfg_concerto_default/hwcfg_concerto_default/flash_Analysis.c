/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "comm_head.h"
#include "flash_Analysis.h"
//#include "block_process.h"

/************local variable define*****************************/
/*!
  header offset in flash.bin  
!*/
#define HEADER_OFFSET 16
/*!
  header size
!*/
#define HEADER_SIZE 12


static void _file_debug_block(u16 num, u8 *p_buffer)
{
  u8 id = 0;
  u8 type = 0;
  u16 node_num = 0;
  u32 base_addr = 0;
  u32 size = 0;
  u32 crc = 0;
  u8 version[9];
  u8 name[9];
  flash_block_t *p_block = NULL;

  if(!p_buffer)
  {
    return;
  }

  p_block = (flash_block_t *)p_buffer;
  
  id = p_block->id;
  type = p_block->type;
  node_num = p_block->node_num;
  base_addr = p_block->base_addr;
  size = p_block->size;
  crc = p_block->crc;

  /* version */
  memset(version, 0, sizeof(u8) * 9);
  memcpy(version, p_block->version, sizeof(u8) * 8);

  /* name */
  memset(name, 0, sizeof(u8) * 9);
  memcpy(name, p_block->name, sizeof(u8) * 8);

  OS_PRINTF("[UPG]---------- Block[%d] ----------\n", num);
  OS_PRINTF("[UPG]          ID: 0x%x\n", id);
  OS_PRINTF("[UPG]          Type: %d\n", type);
  OS_PRINTF("[UPG]          Node Num: %d\n", node_num);
  OS_PRINTF("[UPG]          Addr: 0x%x\n", base_addr);
  OS_PRINTF("[UPG]          Size: 0x%x\n", size);
  OS_PRINTF("[UPG]          CRC: 0x%x\n", crc);
  OS_PRINTF("[UPG]          Version: %s\n", version);
  OS_PRINTF("[UPG]          Name: %s\n", name);

  return;
}

static void _file_debug(u8 *p_flash_buf, u32 addr_offset)
{
  u8 i = 0;
  u16 offset = 0;
  u16 block_num = 0;
  u16 bh_size = 0;
  u8 *p_buffer = NULL; 
  u8 header[HEADER_SIZE];
  u8 *p_addr = NULL;

  if(!p_flash_buf)
  {
    OS_PRINTF("#@ [%s] mem is NULL\n", __FUNCTION__);
    return;
  }
  /*!
    [0xC][0xB0000]
    ---------- 16 bytes ----------
    [*^_^*DM(^o^)]12 bytes
    [?] 4 bytes
    ---------- HEADER[12 bytes] ----------
    [sdram_size] 4 bytes
    [flash_size] 4 bytes
    [block_num] 2 bytes
    [bh_size] 2 bytes
    ---------- BLOCK HEADER ----------
    [block0 header]48 bytes(usb_upg_file_block_t)
    [block1 header]48 bytes(usb_upg_file_block_t)
    [block2 header]48 bytes(usb_upg_file_block_t)
    ......
    */

  /* read header */
  p_addr = (u8 *)(p_flash_buf + addr_offset+ HEADER_OFFSET);
  memcpy(header, p_addr, HEADER_SIZE);

  block_num = MAKE_WORD(header[8], header[9]);
  bh_size = MAKE_WORD(header[10], header[11]);

  p_buffer = mtos_malloc((block_num * bh_size));
  if(!p_buffer)
  {
    OS_PRINTF("#@ [%s] cannot malloc any mem\n", __FUNCTION__);
    return;
  }

  p_addr += HEADER_SIZE;
  /* reader block headers */
  memcpy(p_buffer, p_addr, (block_num * bh_size));

  for(i = 0; i < block_num; i ++)
  {
    offset = i * sizeof(flash_block_t);
    _file_debug_block(i, &p_buffer[offset]);
  }

  if(p_buffer)
  {
    mtos_free(p_buffer);
    p_buffer = NULL;
  }

  return;
}

static s8 _file_find_block_index(img_file_t *p_file, u8 block_id)
{
  u8 i = 0;

  if(!p_file)
  {
    return -1;
  }

  for(i = 0; i < p_file->block_num; i++)
  {
    if(block_id == p_file->blocks[i].id)
    {
      return i;
    }
  }

  return -1;
}

static void _file_get_block_info(img_file_t *p_file,
                                 u8 block_id,
                                 flash_block_t *p_block)
{
  s8 index = -1;

  if((!p_file) || (!p_block))
  {
    return;
  }

  index = _file_find_block_index(p_file, block_id);
  if(-1 == index)
  {
    return;
  }

  memcpy(p_block, &p_file->blocks[index], sizeof(flash_block_t));
  
  return;
}

static u32 _file_get_block_addr(img_file_t *p_file, u8 block_id)
{
  s8 index = -1;

  if(!p_file)
  {
    return 0;
  }

  index = _file_find_block_index(p_file, block_id);
  if(-1 == index)
  {
    return 0;
  }

  return p_file->blocks[index].base_addr;
}

static u32 _file_get_block_size(img_file_t *p_file, u8 block_id)
{
  s8 index = -1;

  if(!p_file)
  {
    return 0;
  }

  index = _file_find_block_index(p_file, block_id);
  if(-1 == index)
  {
    return 0;
  }

  return p_file->blocks[index].size;
}


void FileBuild(u8 *p_flash_buf, img_file_t *p_file, u32 addr_offset)
{
  u8 i = 0;
  u16 offset = 0;
  u16 block_num = 0;
  u8 *p_buffer = NULL;
  flash_block_t *p_block = NULL;
  u8 header[HEADER_SIZE];
  u8 *p_addr = NULL;

  if((!p_flash_buf) || (!p_file))
  {
    return;
  }

  /*!
    [0xC][0xB0000]
    ---------- 16 bytes ----------
    [*^_^*DM(^o^)]12 bytes
    [?] 4 bytes
    ---------- HEADER[12 bytes] ----------
    [sdram_size] 4 bytes
    [flash_size] 4 bytes
    [block_num] 2 bytes
    [bh_size] 2 bytes
    ---------- BLOCK HEADER ----------
    [block0 header]48 bytes(usb_upg_file_block_t)
    [block1 header]48 bytes(usb_upg_file_block_t)
    [block2 header]48 bytes(usb_upg_file_block_t)
    ......
    */

  /* read header */
  p_addr = (u8 *)(p_flash_buf + addr_offset+ HEADER_OFFSET);
  memset(header, 0 , sizeof(u8) * HEADER_SIZE);
  memcpy(header, p_addr, HEADER_SIZE);

  p_file->addr_offset = addr_offset;

  p_file->sdram_size = MT_MAKE_DWORD(
    MAKE_WORD(header[0], header[1]),
    MAKE_WORD(header[2], header[3]));
  p_file->flash_size = MT_MAKE_DWORD(
    MAKE_WORD(header[4], header[5]),
    MAKE_WORD(header[6], header[7]));
  block_num = MAKE_WORD(header[8], header[9]);
  p_file->bh_size = MAKE_WORD(header[10], header[11]);

  if(block_num > (DM_MAX_BLOCK_NUM - p_file->block_num))
  {
    block_num = (DM_MAX_BLOCK_NUM - p_file->block_num);
  }
  
  p_buffer = mtos_malloc((block_num * p_file->bh_size));
  if(!p_buffer)
  {
    return;
  }
  p_addr += HEADER_SIZE;
  /* reader block headers */
  memcpy(p_buffer, p_addr, (block_num * p_file->bh_size));


  for(i = 0; i < block_num; i ++)
  {
    offset = i * sizeof(flash_block_t);
    #if 1
    p_block = (flash_block_t *)&p_buffer[offset];
    memcpy(&p_file->blocks[p_file->block_num + i], p_block, sizeof(flash_block_t));
    #else
    p_file->blocks[i].id = p_buffer[offset];
    p_file->blocks[i].type = p_buffer[offset + 1];
    p_file->blocks[i].node_num = MAKE_WORD(p_buffer[offset + 2], p_buffer[offset + 3]);
    p_file->blocks[i].base_addr = MT_MAKE_DWORD(
      MAKE_WORD(p_buffer[offset + 4], p_buffer[offset + 5]),
      MAKE_WORD(p_buffer[offset + 6], p_buffer[offset + 7]));
    p_file->blocks[i].size = MT_MAKE_DWORD(
      MAKE_WORD(p_buffer[offset + 8], p_buffer[offset + 9]),
      MAKE_WORD(p_buffer[offset + 10], p_buffer[offset + 11]));
    p_file->blocks[i].crc = MT_MAKE_DWORD(
      MAKE_WORD(p_buffer[offset + 12], p_buffer[offset + 13]),
      MAKE_WORD(p_buffer[offset + 14], p_buffer[offset + 15]));

    /* version */
    memset(p_file->blocks[i].version, 0, sizeof(u8) * 8);
    memcpy(p_file->blocks[i].version, &p_buffer[offset + 16], sizeof(u8) * 8);

    /* name */
    memset(p_file->blocks[i].name, 0, sizeof(u8) * 8);
    memcpy(p_file->blocks[i].name, &p_buffer[offset + 24], sizeof(u8) * 8);

    /* time */
    memset(p_file->blocks[i].time, 0, sizeof(u8) * 12);
    memcpy(p_file->blocks[i].time, &p_buffer[offset + 32], sizeof(u8) * 12);

    p_file->blocks[i].ota_ver = MAKE_WORD(p_buffer[offset + 44], p_buffer[offset + 45]);
    p_file->blocks[i].reserve = MAKE_WORD(p_buffer[offset + 46], p_buffer[offset + 47]);
    #endif
  }

  p_file->block_num += block_num;

  if(p_buffer)
  {
    mtos_free(p_buffer);
    p_buffer = NULL;
  }
  
  return;
}


s32 BlockRead_EX(block_buf_t **Block, u16 Num)
{
  *Block = (block_buf_t *)mtos_malloc(sizeof(block_buf_t) * Num);
  if(Block == NULL)
  {
    return ERR_NO_MEM;
  }
  else
  {
    return SUCCESS;
  }
}
  
s8 BlockRead(u8 BlockId, flash_file_t *file, block_buf_t *Block)
{
  s8 Ret = -1;
  u8 ImgIndex = 0;
  u8 *addr = NULL;
  u32 tmp_crc= 0;
  u8 *tmp_addr = NULL;
  
  Ret = _file_find_block_index(&file->img_info[ImgIndex], BlockId);
  if(Ret == -1)
  {
    Ret = _file_find_block_index(&file->img_info[++ImgIndex], BlockId);
  }

  if(Ret != -1)
  {
    Block->img_num = ImgIndex;
    Block->block_id = BlockId;
    Block->size = file->img_info[ImgIndex].blocks[Ret].size;
    Block->buf = mtos_malloc(Block->size + 1);
    if(Block->buf == NULL)
    {
      return ERR_NO_MEM;
    }
    addr = (u8 *)(file->flash_buf + (u32)file->img_info[ImgIndex].addr_offset + (u32)file->img_info[ImgIndex].blocks[Ret].base_addr);
    memcpy(Block->buf, addr, Block->size);
	tmp_crc = crc_fast_calculate(0xFFFFFFFF,(u8 *)Block->buf, Block->size);
	tmp_addr = (u8 *)(file->flash_buf + (u32)file->img_info[ImgIndex].addr_offset + 16+12+12+(Ret)*file->img_info[ImgIndex].bh_size);;
	if((u8)(tmp_crc  & 0x000000FF) != tmp_addr[0])
	{
		return ERR_FAILURE;
	}
	if((u8)((tmp_crc & 0x0000FF00)>>8) != tmp_addr[1] )
	{
		return ERR_FAILURE;
	}
	if((u8)((tmp_crc & 0x00FF0000)>>16) != tmp_addr[2] )
	{
		return ERR_FAILURE;
	}
	if((u8)((tmp_crc & 0xFF000000)>>24) != tmp_addr[3] )
	{
		return ERR_FAILURE;
	}
  }

  return Ret;
}

s8 BlockWrite(flash_file_t *file, block_buf_t *Block)
{
  s8 Ret = -1;
  u8 ImgIndex = 0;
  u8 *addr = NULL;
  u8 *tmp_addr = NULL;
  u32 crc = 0, tmp_crc= 0;
  
  Ret = _file_find_block_index(&file->img_info[ImgIndex], Block->block_id);
  if(Ret == -1)
  {
    Ret = _file_find_block_index(&file->img_info[++ImgIndex], Block->block_id);
  }

  if(Ret != -1)
  {
//    Block->size = file->img_info[ImgIndex].blocks[Ret].size;//????

    if(Block->size > file->img_info[ImgIndex].blocks[Ret].size)
    {
      return ERR_NO_RSRC;
    }
    
    if(Block->buf == NULL)
    {
      return ERR_FAILURE;
    }
    addr = (u8 *)(file->flash_buf + (u32)file->img_info[ImgIndex].addr_offset + (u32)file->img_info[ImgIndex].blocks[Ret].base_addr);
	tmp_addr = addr;

	tmp_crc = crc_fast_calculate(0xFFFFFFFF,(u8 *)tmp_addr, (int)file->img_info[ImgIndex].blocks[Ret].size);
	
    if(addr == NULL || (u8 *)(addr + Block->size) == NULL)
    {
      return ERR_FAILURE;
    }
    memcpy(addr, Block->buf, Block->size);
    if(Block->size < file->img_info[ImgIndex].blocks[Ret].size)
    {
 //     addr = (u8 *)(addr + file->img_info[ImgIndex].blocks[Ret].size - Block->size);
	  addr = (u8 *)(addr + Block->size);
      memset(addr, 0xff, file->img_info[ImgIndex].blocks[Ret].size - Block->size);
    }
  }
  
	//calculate current block's CRC
//	tmp_crc = crc_fast_calculate(0xFFFFFFFF,(u8 *)tmp_addr, (int)file->img_info[ImgIndex].blocks[Ret].size);
  	crc = crc_fast_calculate(0xFFFFFFFF,(u8 *)tmp_addr, (int)file->img_info[ImgIndex].blocks[Ret].size);
	tmp_addr = (u8 *)(file->flash_buf + (u32)file->img_info[ImgIndex].addr_offset + 16+12+12+(Ret)*file->img_info[ImgIndex].bh_size);;
	tmp_addr[0] = (u8)(crc  & 0x000000FF);
	tmp_addr[1] = (u8)((crc & 0x0000FF00)>>8);
	tmp_addr[2] = (u8)((crc & 0x00FF0000)>>16);
	tmp_addr[3] = (u8)((crc & 0xFF000000)>>24);
	
  return SUCCESS;
}



void DeBlockRead(block_buf_t *Block)
{
  if(Block->buf != NULL)
  {
    mtos_free(Block->buf);
	Block->buf = NULL;
  }
}

void DeBlockRead_EX(block_buf_t *Block, u16 Num)
{
  u16 Index = 0;

  for(;Index < Num; Index ++)
  {
    DeBlockRead(&Block[Index]);
  }
}
