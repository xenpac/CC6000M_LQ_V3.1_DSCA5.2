/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include <ctype.h>
#include "emon_cmd.h"
#ifndef WIN32
#include "hal_misc.h"
#endif
#include "lib_util.h"
#include "vfs.h"
#include "lib_unicode.h"
#include "sys_app.h"
#include "hal_base.h"
#include "mtos_mem.h"

#include "lib_rect.h"
#include "common.h"
#include "drv_dev.h"
#include "display.h"
#include "cmd_system.h"


/******************************************************************************/


#define MEM_DISP_8_HELP_STR \
"md.b <addr [count]> - Display value(1byte) of memory address(0x80000000 cache memory, 0xa0000000 non-cache memory, 9e000000 flash memory)"
#define MEM_DISP_16_HELP_STR \
"md.w <addr [count]> - Display value(2bytes) of memory address(0x80000000 cache memory, 0xa0000000 non-cache memory, 9e000000 flash memory)"
#define MEM_DISP_32_HELP_STR \
"md.l <addr [count]> - Display value(4bytes) of memory address(0x80000000 cache memory, 0xa0000000 non-cache memory, 9e000000 flash memory)"
#define MEM_DISP_HELP_STR \
"md <addr [count]> - Display value(4bytes equal with md.l) of memory address(0x80000000 cache memory, 0xa0000000 non-cache memory, 9e000000 flash memory)"

#define DEFAULT_DUMP_FILE_PATH "C:\\mt_dump.bin"

/******************************************************************************/

typedef enum  
{
	TARGET_ENDIAN_UNKNOWN = 0,
	TARGET_BIG_ENDIAN = 1, 
	TARGET_LITTLE_ENDIAN = 2
}target_endianness;

typedef struct 
{
	target_endianness endianness;	/* target endianness */
}sys_target;

static sys_target gTarget;

/******************************************************************************/


static int mem_dump2flash(char *param);


/******************************************************************************/


static BOOL is_in_range(u32 addr, u32 size, u32 range_b, u32 range_e)
{
  if (addr < range_b)
  {
    return FALSE;
  }
  if (addr + size > range_e)
  {
    return FALSE;
  }

  return TRUE;
}

static BOOL mem_addr_legal(u32 addr, u32 size)
{
  u32 flash_b = 0;
  u32 flash_e = 0;
  u32 cache_mem_b = 0;
  u32 cache_mem_e = 0;
  u32 nc_mem_b = 0; // none cache memory begin
  u32 nc_mem_e = 0; // none cache memory end
  u32 reg_b = 0;
  u32 reg_e = 0;

  flash_b = get_flash_addr();
  flash_e = flash_b + CHARSTO_SIZE;

  cache_mem_b = 0x80000000;
  cache_mem_e = WHOLE_MEM_END & 0x8FFFFFFF;

  nc_mem_b = 0xa0000000;
  nc_mem_e = WHOLE_MEM_END;

  // FIXME:
  reg_b = 0xbf000000;
  reg_e = 0xbff00000; 

  if (is_in_range((u32)addr, size, flash_b, flash_e) ||
      is_in_range((u32)addr, size, cache_mem_b, cache_mem_e) ||
      is_in_range((u32)addr, size, nc_mem_b, nc_mem_e) ||
      is_in_range((u32)addr, size, reg_b, reg_e))
  {
    return TRUE;
  }
  
  return FALSE;
}

static inline u32 le_to_h_u32(const u8* buf)
{
  return (u32)(buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24);
}

static inline u16 le_to_h_u16(const u8* buf)
{
  return (u16)(buf[0] | buf[1] << 8);
}

static inline u32 be_to_h_u32(const u8* buf)
{
  return (u32)(buf[3] | buf[2] << 8 | buf[1] << 16 | buf[0] << 24);
}

static inline u16 be_to_h_u16(const u8* buf)
{
  return (u16)(buf[1] | buf[0] << 8);
}

static inline void h_u32_to_le(u8* buf, u32 val)
{
  buf[3] = (u8) (val >> 24);
  buf[2] = (u8) (val >> 16);
  buf[1] = (u8) (val >> 8);
  buf[0] = (u8) (val >> 0);
}

static inline void h_u32_to_be(u8* buf, u32 val)
{
  buf[0] = (u8) (val >> 24);
  buf[1] = (u8) (val >> 16);
  buf[2] = (u8) (val >> 8);
  buf[3] = (u8) (val >> 0);
}

static inline void h_u16_to_le(u8* buf, u32 val)
{
  buf[1] = (u8) (val >> 8);
  buf[0] = (u8) (val >> 0);
}

static inline void h_u16_to_be(u8* buf, u32 val)
{
  buf[0] = (u8) (val >> 8);
  buf[1] = (u8) (val >> 0);
}



/* read a u32 from a buffer in target memory endianness */
u32 target_buffer_get_u32(sys_target *target, const u8 *buffer)
{
  if (target->endianness == TARGET_LITTLE_ENDIAN)
    return le_to_h_u32(buffer);
  else
    return be_to_h_u32(buffer);
}


/* read a u16 from a buffer in target memory endianness */
u16 target_buffer_get_u16(sys_target *target, const u8 *buffer)
{
  if (target->endianness == TARGET_LITTLE_ENDIAN)
    return le_to_h_u16(buffer);
  else
    return be_to_h_u16(buffer);
}

/* read a u8 from a buffer in target memory endianness */
static u8 target_buffer_get_u8(sys_target *target, const u8 *buffer)
{
  return *buffer & 0x0ff;
}

/* write a uint32_t to a buffer in target memory endianness */
void target_buffer_set_u32(sys_target *target, u8 *buffer, u32 value)
{
  if (target->endianness == TARGET_LITTLE_ENDIAN)
    h_u32_to_le(buffer, value);
  else
    h_u32_to_be(buffer, value);
}


/* write a u16 to a buffer in target memory endianness */
void target_buffer_set_u16(sys_target *target, u8 *buffer, u16 value)
{
  if (target->endianness == TARGET_LITTLE_ENDIAN)
    h_u16_to_le(buffer, value);
  else
    h_u16_to_be(buffer, value);
}

/* write a u8 to a buffer in target memory endianness */
static void target_buffer_set_u8(sys_target *target, u8 *buffer, u8 value)
{
  *buffer = value;
}


static int _read_memory(sys_target *target, u8 *src,
      u32 width, u32 count,
      u8 *dst)
{
  MT_ASSERT(src != NULL);
  MT_ASSERT(dst != NULL);

  if ((u32)src % width != 0)
  {
    return ERR_FAILURE;
  }
  
  memcpy(dst, src, width * count);

  return SUCCESS;
}

static void mem_dump_ex(u32 addr, s32 size, u8 width)
{
  u32 x = 0;
  u32 y = 0;
  u32 z = 0;
  u8 *p = (u8*)addr;
  u8  target_buf[32];
  s32 ret;

  //APPLOGA("buffer:%p, size:%d, width:%d\n", p, size, width);


  while (size > 0) {
    y = (u32)((size < 16) ? size : 16); /* y = min(bytes, 16); */

    ret = _read_memory(&gTarget, p, width, y / width, target_buf);
    if (ret != 0) {
      APPLOGA("Read memory failed\n");
      return;
    }

    APPLOGA("%08x ", (int)(p));
    switch (width) {
      case 4:
        for (x = 0; x < 16 && x < y; x += 4) {
          z = target_buffer_get_u32(&gTarget, &(target_buf[x]));
          APPLOGA("%08x ", (z));
        }
        for (; (x < 16) ; x += 4)
          APPLOGA("         ");
        break;
      case 2:
        for (x = 0; x < 16 && x < y; x += 2) {
          z = target_buffer_get_u16(&gTarget, &(target_buf[x]));
          APPLOGA("%04x ", (z));
        }
        for (; (x < 16) ; x += 2)
          APPLOGA("     ");
        break;
      case 1:
      default:
        for (x = 0 ; (x < 16) && (x < y) ; x += 1) {
          z = target_buffer_get_u8(&gTarget, &(target_buf[x]));
          APPLOGA("%02x ", (z));
        }
        for (; (x < 16) ; x += 1)
          APPLOGA("   ");
        break;
    }
    /* ascii-ify the bytes */
    for (x = 0 ; x < y ; x++) {
      if ((target_buf[x] >= 0x20) &&
      (target_buf[x] <= 0x7e)) {
        /* good */
      } else {
        /* smack it */
        target_buf[x] = '.';
      }
    }
    /* space pad  */
    while (x < 16) {
      target_buf[x] = ' ';
      x++;
    }
    /* terminate */
    target_buf[16] = 0;
    /* print - with a newline */
    APPLOGA("%s\n", target_buf);
    /* NEXT... */
    size -= 16;
    p += 16;
  }
}




static int mem_disp_8bits(char *param)
{
  u32 addr;
  u32 count;
  u8 width = sizeof(u8);
  u32 bytes = 0;


  if (EMON_CheckCommand(param, 2))
  {
    sscanf(param,"%lx %ld", &addr,&count);
  }
  else if (EMON_CheckCommand(param, 1))
  {
    sscanf(param,"%lx", &addr);
    count = sizeof(u32) / width;
  }
  else
  {
    return ERR_FAILURE;
  }
  if (!(mem_addr_legal(addr, count)))
  {
    return ERR_FAILURE;
  }

  bytes = count * width;

  mem_dump_ex(addr, (s32)bytes, width);
  return SUCCESS;
}


static int mem_disp_16bits(char *param)
{
  u32 addr;
  u32 count;
  u8 width = sizeof(u16);
  u32 bytes = 0;


  if (EMON_CheckCommand(param, 2))
  {
    sscanf(param,"%lx %ld", &addr,&count);
  }
  else if (EMON_CheckCommand(param, 1))
  {
    sscanf(param,"%lx", &addr);
    count = sizeof(u32) / width;
  }
  else
  {
    return ERR_FAILURE;
  }
  if (!(mem_addr_legal(addr, count)))
  {
    return ERR_FAILURE;
  }

  bytes = count * width;

  mem_dump_ex(addr, (s32)bytes, width);
  return SUCCESS;
}


static int mem_disp_32bits(char *param)
{
  u32 addr;
  u32 count;
  u8 width = sizeof(u32);
  u32 bytes = 0;


  if (EMON_CheckCommand(param, 2))
  {
    sscanf(param,"%lx %ld", &addr,&count);
  }
  else if (EMON_CheckCommand(param, 1))
  {
    sscanf(param,"%lx", &addr);
    count = sizeof(u32) / width;
  }
  else
  {
    return ERR_FAILURE;
  }
  if (!(mem_addr_legal(addr, count)))
  {
    return ERR_FAILURE;
  }

  bytes = count * width;

  mem_dump_ex(addr, (s32)bytes, width);
  return SUCCESS;
}


static int mem_disp(char *param)
{
  return mem_disp_32bits(param);
}


static int _fill_register(u32 addr, u32 value, u32 width)
{
  switch (width) {
  case 4:
    value = value & 0xFFFFFFFF;
    *(volatile u32 *)addr = value;
    break;
  case 2:
    value = value & 0xFFFF;
    *(volatile u16 *)addr = value;
    break;
  case 1:
    value = value & 0xFF;
    *(volatile u8 *)addr = value;
    break;
  default:
    return ERR_FAILURE;
  }
  return SUCCESS;
}


static int _fill_mem(sys_target *target, u32 addr,
      u32 width, u32 count, u32 value)
{
  u32 i;
  u8 *dst = (u8*)addr;
  
  MT_ASSERT(dst != NULL);

  if ((u32)dst % width != 0)
  {
    return ERR_FAILURE;
  }
  if (count == 1)
  {
    return _fill_register(addr, value, width);
  }

  for (i = 0; i < count; i++) {
    switch (width) {
    case 4:
      target_buffer_set_u32(target, dst + i * width, value);
      break;
    case 2:
      target_buffer_set_u16(target, dst + i * width, (u16)value);
      break;
    case 1:
      target_buffer_set_u8(target, dst + i * width, (u8)value);
      break;
    default:
      return ERR_FAILURE;
    }
  }
  return SUCCESS;
}



static int mem_write_8bits(char *param)
{
  u32 addr;
  u32 count;
  unsigned int value;
  u32 width = sizeof(u8);
  
  if(EMON_CheckCommand(param, 2))
  {
    count = 1;
    sscanf(param,"%lx %2x", &addr, &value);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else if (EMON_CheckCommand(param, 3))
  {
    sscanf(param,"%lx %2x %ld",(u32*)&addr, &value, &count);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else
  {
    return ERR_FAILURE;
  }

  return _fill_mem(&gTarget, addr, width, count, value);
}

static int mem_write_16bits(char *param)
{
  u32 addr;
  u32 count;
  unsigned int value;
  u32 width = sizeof(u16);
  
  if(EMON_CheckCommand(param, 2))
  {
    count = 1;
    sscanf(param,"%lx %4x", &addr, &value);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else if (EMON_CheckCommand(param, 3))
  {
    sscanf(param,"%lx %4x %ld",(u32*)&addr, &value, &count);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else
  {
    return ERR_FAILURE;
  }

  return _fill_mem(&gTarget, addr, width, count, value);
}


static int mem_write_32bits(char *param)
{
  u32 addr;
  u32 count;
  unsigned int value;
  u32 width = sizeof(u32);
  
  if(EMON_CheckCommand(param, 2))
  {
    count = 1;
    sscanf(param,"%lx %x", &addr, &value);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else if (EMON_CheckCommand(param, 3))
  {
    sscanf(param,"%lx %x %ld",(u32*)&addr, &value, &count);
    if (!(mem_addr_legal(addr, count)))
    {
      return ERR_FAILURE;
    }
  }
  else
  {
    return ERR_FAILURE;
  }

  return _fill_mem(&gTarget, addr, width, count, value);
}


static int mem_write(char *param)
{
  return mem_write_32bits(param);
}


static int sys_reboot(char *param)
{
#ifndef WIN32
  APPLOGA("reset stb! \n", __FUNCTION__);
  hal_pm_reset();
#endif
  return SUCCESS;
}


static int mem2file(u32 addr, u32 size, u8 *filePath)
{
  hfile_t fd;
  u32 write_len = 0;
  u8 *buffer = (u8*)addr;
  u16 fPath[MAX_FILE_NAME];

  memset(fPath, 0, sizeof(fPath));
  if (filePath == NULL)
  {
    str_asc2uni((u8*)DEFAULT_DUMP_FILE_PATH, fPath);
  }
  else
  {
    str_asc2uni(filePath, fPath);
  }
  fd = vfs_open(fPath, VFS_NEW);
  if (fd == NULL)
  {
    return ERR_FAILURE;
  }
  do
  {
    if((addr & 0x9e000000) == 0x9e000000)
    {
      buffer = (void *)mtos_malloc(size);
      if(buffer == NULL)
        return ERR_FAILURE;
      else
      {
        memcpy((void *)buffer,(void *)addr,size);
      }
    }
    write_len = vfs_write(buffer, size, 1, fd);
    if (write_len != size)
    {
      break;
    }
    vfs_flush(fd);
  }while (0);
  vfs_close(fd);
  if (write_len != size)
  {
    return ERR_FAILURE;
  }
  return SUCCESS;
}

static int mem_dump2flash(char *param)
{
  u32 addr;
  u32 ulSize;
  u8 filePath[MAX_FILE_NAME];
  int ret;
  
  if(EMON_CheckCommand(param, 2))// <addr len>
  {
    sscanf(param,"%lx %ld", &addr, &ulSize);
    if (!(mem_addr_legal(addr, ulSize)))
    {
      return ERR_FAILURE;
    }
    ret = mem2file(addr, ulSize, NULL);
  }
  else if(EMON_CheckCommand(param, 3))// //<addr len filepath>
  {
    memset(filePath, 0, sizeof(filePath));
    sscanf(param,"%lx %ld %s", &addr, &ulSize, filePath);
    if (!(mem_addr_legal(addr, ulSize)))
    {
      return ERR_FAILURE;
    }
    ret = mem2file(addr, ulSize, filePath);
  }
  else
  {
    ret = ERR_FAILURE;
  }
  
  return ret;
}

//=============================================================================

#define  STR_NUB   200
#define  REGISTER_NUM 80
#define  DISP_NUM 20


typedef struct
{
  u32  m_RegisterAddr;/*寄存器地址*/
  u32  m_RegisterValue;/*寄存器期望值*/
  u32  m_MaskValue;
}REGISTER_ST;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 tv_mode;
  u8 tv_resolution;
  u8 tv_ratio;
  u8 video_output;
  u8 digital_audio_output;
  u8 rf_system;
  u8 rf_channel;
  u8 video_effects;
} av_set_t;

static REGISTER_ST st_RegisterArr[REGISTER_NUM] =
{
  {0xbf440000,0x01000000,0x01100022},{0xbf44000c,0x00007000,0x30000fff},{0xbf440020,0x00020001,0xffffffff},{0xbf440024,0x00010000,0xfffeffff},{0xbf440048,0xffff0000,0x00ff0000},{0xbf440054,0x00000000,0x00000001},{0xbf4400a8,0x00000000,0x20000000},{0xbf4400b0,0x0100ff00,0x00000001},{0xbf4400d0,0x00000000,0xffffffff},{0xbf4400d8,0x00808080,0x00ffffff},
  {0xbf440150,0x000aff96,0xffffffff},{0xbf440154,0x00f602d4,0xffffffff},{0xbf440158,0x00f6ff96,0xffffffff},{0xbf44015c,0x000a0000,0xffffffff},{0xbf440164,0x00000020,0x00000130},{0xbf443000,0x00000000,0xfffff1ff},{0xbf443014,0x00000000,0x00000001},{0xbf443030,0x00000000,0x00000001},{0xbf443060,0x00000000,0x00000001},{0xbf44306c,0x00000000,0x80000000},
  {0xbf460000,0x00000003,0xfffffffd},{0xbf460004,0x013b0000,0xffffffff},{0xbf460008,0x11362621,0xffffffff},{0xbf46000c,0x01180118,0xffffffff},{0xbf460010,0x0138010d,0xffffffff},{0xbf460014,0x02000108,0xffffffff},{0xbf460018,0x2a098acb,0xffffffff},{0xbf46001c,0x21f07c1f,0xffffffff},{0xbf460020,0x0160010a,0xffffffff},{0xbf460024,0x00780116,0xffffffff},
  {0xbf460028,0x0168011a,0xffffffff},{0xbf46002c,0x0073011a,0xffffffff},{0xbf460030,0x012e0000,0xffffffff},{0xbf460034,0x00000001,0xffffffff},{0xbf460038,0x00000000,0xffffffff},{0xbf46003c,0x00000011,0xffffffff},{0xbf460040,0x01148c46,0xffffffff},{0xbf460044,0x0003898b,0xffffffff},{0xbf460048,0x0002e662,0xffffffff},{0xbf46004c,0x00000041,0xffffffff},
  {0xbf460050,0x29c71c72,0xffffffff},{0xbf460054,0x284bda13,0xffffffff},{0xbf460058,0xc068db8c,0xffffffff},{0xbf46005c,0x80000000,0xffffffff},{0xbf460060,0x062b062b,0xffffffff},{0xbf460064,0x00010000,0xffffffff},{0xbf460068,0x00f8011a,0xffffffff},{0xbf46006c,0x00909094,0xffffffff},{0xbf460070,0x00000004,0xffffffff},{0xbf460074,0x00002000,0xffffffff},
  {0xbf460078,0x06900698,0xffffffff},{0xbf46007c,0x00000000,0xffffffff},{0xbf460080,0x00010206,0xffffffff},{0xbf460084,0x02040205,0xffffffff},{0xbf460088,0x02010004,0xffffffff},{0xbf46008c,0x00080012,0xffffffff},{0xbf460090,0x02030013,0xffffffff},{0xbf460094,0x020f0207,0xffffffff},{0xbf460098,0x000e022d,0xffffffff},{0xbf46009c,0x00170235,0xffffffff},
  {0xbf4600a0,0x0229000a,0xffffffff},{0xbf4600a4,0x021d008c,0xffffffff},{0xbf4600a8,0x009f0111,0xffffffff},{0xbf4600ac,0x01200148,0xffffffff},{0xbf4600b0,0x00000000,0xffffffff},{0xbf4600b4,0x55555555,0xffffffff},{0xbf4600b8,0x55555555,0xffffffff},{0xbf4600bc,0x00000000,0xffffffff},{0xbf4600c0,0x00000000,0xffffffff},{0xbf4600c4,0x55555555,0xffffffff},
  {0xbf4600c8,0x55555555,0xffffffff},{0xbf4600cc,0x00000000,0xffffffff},{0xbf4600d0,0x00003280,0xffffffff},{0xbf4600d4,0x00100000,0xffffffff},{0xbf4600d8,0x01000000,0xffffffff},{0xbf4600dc,0x00000000,0xffffffff},{0xbf4600e0,0x000000cc,0xffffffff},{0xbf4600e4,0x83838280,0xffffffff},{0xbf4600e8,0x07148c46,0xffffffff},{0xbf4600ec,0x00000000,0xffffffff},
};

extern void sys_status_get_av_set(av_set_t *p_set);

static int TE_Register_check(char *param)
{
  void *p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

  MT_ASSERT(p_disp != NULL);
  disp_venc_check(p_disp);
  return SUCCESS;
}

static int  register_print_all(char *param)
{
  int i = 0;
  u32 uReadValue = 0;
  
  APPLOGA(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>disp 部分校验>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
  for(i = 0; i < DISP_NUM; i++)
  {
    uReadValue =  hal_get_u32((volatile u32 *)st_RegisterArr[i].m_RegisterAddr);
    APPLOGA("ResgiterAddr[0x%08x],RegisterValue[0x%08x],MaskValue[0x%08x],uReadValue[0x%08x]\r\n",st_RegisterArr[i].m_RegisterAddr,st_RegisterArr[i].m_RegisterValue,st_RegisterArr[i].m_MaskValue, uReadValue);
  }
  APPLOGA(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>sd_venc 部分校验>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
  for(i = DISP_NUM; i < REGISTER_NUM; i++)
  {
    uReadValue =  hal_get_u32((volatile u32 *)st_RegisterArr[i].m_RegisterAddr);
    APPLOGA(" ResgiterAddr[0x%08x],RegisterValue[0x%08x],MaskValue[0x%08x],uReadValue[0x%08x]\r\n",st_RegisterArr[i].m_RegisterAddr,st_RegisterArr[i].m_RegisterValue,st_RegisterArr[i].m_MaskValue, uReadValue);	
  }
  return SUCCESS;
}

static int bandwidth_check(char *param)
{
  volatile u32 reg_hd = 0xBF47004C;
  volatile u32 reg_sd = 0xBF45003C;
  volatile u32 reg_di = 0xbf442000;
  BOOL is_hd_overflow = FALSE;
  BOOL is_sd_overflow = FALSE;
  BOOL is_di_open = FALSE;
  u32 value;

  value = hal_get_u32((volatile u32 *)reg_hd);
  is_hd_overflow = (value & ~0xFEFFFFFF) > 0 ? TRUE : FALSE;
  value = hal_get_u32((volatile u32 *)reg_sd);
  is_hd_overflow = (value & 0x20) > 0 ? TRUE : FALSE;
  value = hal_get_u32((volatile u32 *)reg_di);
  is_di_open = value & 0x1;
  APPLOGA("HDMI bandwidth status: %s\n", (is_hd_overflow) > 0 ? "overflow" :  "normal");
  APPLOGA("CVBS bandwidth status: %s\n", (is_sd_overflow) > 0 ? "overflow" :  "normal");
  APPLOGA("DI status: %s\n", (is_di_open) > 0 ? "open" :  "closed");
  hal_put_u32((volatile u32 *)reg_hd, 0x1000000);
  volatile u32 reg_tmp = 0xBF460064;
  value = hal_get_u32((volatile u32 *)reg_tmp);
  value |= 1 << 17;
  hal_put_u32((volatile u32 *)reg_tmp, value);
  return 0;
}

static EMON_COMMAND_LIST cmd_sys[] = 
{
  {mem_disp, "md", MEM_DISP_HELP_STR,FALSE},
  {mem_disp_8bits, "md.b", MEM_DISP_8_HELP_STR,FALSE},
  {mem_disp_16bits, "md.w", MEM_DISP_16_HELP_STR,FALSE},
  {mem_disp_32bits, "md.l", MEM_DISP_32_HELP_STR,FALSE},
  {mem_write, "mw", "mw <addr value [count]> - Fill memory by specific value(4bytes hex)",FALSE},
  {mem_write_8bits, "mw.b", "mf <addr value [count]> - Fill memory by specific value(1byte hex)",FALSE},
  {mem_write_16bits, "mw.w", "mf <addr value [count]> - Fill memory by specific value(2bytes hex)",FALSE},
  {mem_write_32bits, "mw.l", "mf <addr value [count]> - Fill memory by specific value(4bytes hex)",FALSE},
  {mem_dump2flash, "m2f", "m2f <addr len [file]> - Write memory to file",FALSE},
  {sys_reboot, "reboot", "reboot - Reboot STB",FALSE},
  {TE_Register_check, "av.rc", "av register check",FALSE},
  {register_print_all, "av.pr", "av register addr value print",FALSE},
  {bandwidth_check, "bdcheck", "check the bundwith if overfolow, and display the DI status",FALSE},
  {0, 0, 0, 0}
};

static target_endianness checkCPUendian(void)
{
  union {
  unsigned int a;
  unsigned char b;            
  }c;
  
  c.a = 1;

  if (c.b == 1)
  {
    return TARGET_LITTLE_ENDIAN;
  }
  else
  {
    return TARGET_BIG_ENDIAN;
  }
}   


int cmd_sys_init(void)
{
  memset(&gTarget, 0, sizeof(gTarget));
  gTarget.endianness = checkCPUendian();
  return EMON_Register_Test_Functions(cmd_sys, "system");
}



