#ifndef __NES_MENU_H__
#define __NES_MENU_H__
#ifdef __cplusplus
extern "C" {
#endif 

#include "sys_types.h"
#include "sys_define.h"

#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

#include "aud_vsb.h"

#include "lib_unicode.h"
#include "lib_util.h"

#include "lib_memf.h"
#include "lib_memp.h"

#include "mmi.h"

#include "vfs.h"

void *nes_malloc(u32 size);
void nes_free(void *p_buffer);

#if 0
#define NES_MALLOC_DMA    mtos_dma_malloc
#define NES_FREE_DMA      mtos_dma_free
#define NES_MALLOC_ALIGN  mtos_align_malloc
#define NES_FREE_ALIGN    mtos_align_free
#define NES_MALLOC        mtos_malloc
#define NES_FREE          mtos_free
#else
#define NES_MALLOC        nes_malloc
#define NES_FREE          nes_free
#endif

#define NES_MEMSET  memset
#define NES_MEMCPY  memcpy
#define NES_MEMCMP  memcmp

#define NES_TASK_CREATE mtos_task_create
#define NES_TASK_EXIT  mtos_task_exit
#define NES_TASK_SLEEP  mtos_task_sleep
#define NES_TASK_DELAY  mtos_task_delay_ms
#define NES_GET_TICKS mtos_ticks_get

#define NES_ASSERT  mmi_assert

#define NES_PRINT   OS_PRINTF

#define ROM_PATH_LEN 256

typedef void (*gpe_blt_t)(void *p_data, u16 x, u16 y, u16 w, u16 h);

typedef void (*joy_key_get_t)(u32 pad_num, u32 *p_key);

typedef void (*irda_key_get_t)(u32 *p_key);

typedef void (*emu_exit_t)(BOOL is_unsupport);

typedef struct
{
  u32 task_prio;

  u32 frame_skip;

  void *p_nes_rgn;

  void *p_gpe_dev;

  void *p_aud_dev;

  void *p_rom;

  u32 rom_size;

  gpe_blt_t gpe_blt;

  irda_key_get_t irda_key_get;

  joy_key_get_t joy_key_get;
  
  emu_exit_t emu_exit;

  BOOL is_file;

  u16 rom_path[ROM_PATH_LEN];

  lib_memp_t *p_heap;
}nes_emu_cfg_t;

#define NES_VKEY_A 0x00000001
#define NES_VKEY_B 0x00000002
#define NES_VKEY_SELECT  0x00000004
#define NES_VKEY_START   0x00000008
#define NES_VKEY_UP      0x00000010
#define NES_VKEY_DOWN    0x00000020
#define NES_VKEY_LEFT    0x00000040
#define NES_VKEY_RIGHT   0x00000080

#define NES_VKEY_EXIT    0x00000100
#define NES_VKEY_RESET   0x00000200

#define NES_VBUF_SIZE (320 * 240 * 2)

BOOL nes_emu_start(nes_emu_cfg_t *p_cfg);

void nes_emu_stop(void);

void nes_emu_blt(void *p_buffer, int x, int y, int w, int h);

void nes_emu_get_joy_key(u32 pad_num, u32 *p_key);

void nes_emu_get_irda_key(u32 *p_key);

void nes_emu_exit(BOOL is_unsupport);

#ifdef __cplusplus
}
#endif 
#endif
