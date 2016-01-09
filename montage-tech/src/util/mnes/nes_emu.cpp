#include "string.h"

#include "stdlib.h"

#include "sys_types.h"

#include "lib_rect.h"

#include "types.h"
#include "nes_emu.h"

#include "nes_player.h"

extern bool GameLooping;

#define NES_STK_SIZE (32 * 1024)

#define NES_MSGQ_DEPTH  10

nes_emu_cfg_t g_emu_cfg = {0};

static u32 *p_nes_stk = NULL;

void *nes_malloc(u32 size)
{
  if(g_emu_cfg.p_heap != NULL)
  {
    return lib_memp_align_alloc(g_emu_cfg.p_heap, size, 8);
  }
  else
  {
    return mtos_align_malloc(size, 8);
  }	
}

void nes_free(void *p_buffer)
{
  if(g_emu_cfg.p_heap != NULL)
  {
    lib_memp_align_free(g_emu_cfg.p_heap, p_buffer);
  }
  else
  {
    mtos_align_free(p_buffer);
  }
}

static void nes_entry(void *p_param)
{
	if(g_emu_cfg.is_file)
	{
  	run_nes_emulator(g_emu_cfg.rom_path);
	}
	else
	{
		run_nes_emulator_on_ram(g_emu_cfg.p_rom, g_emu_cfg.rom_size);
	}
  NES_TASK_EXIT();
}

extern "C" BOOL nes_emu_start(nes_emu_cfg_t *p_cfg)
{
  BOOL ret_boo = FALSE;

  NES_ASSERT(p_cfg != NULL);
  
  NES_ASSERT(p_cfg->task_prio != 0);

  NES_MEMCPY(&g_emu_cfg, p_cfg, sizeof(nes_emu_cfg_t));

  NES_ASSERT(p_nes_stk == NULL);
  
  p_nes_stk = (u32 *)NES_MALLOC(NES_STK_SIZE);

	NES_ASSERT(p_nes_stk != NULL);

  NES_MEMSET(p_nes_stk, 0, NES_STK_SIZE);

  /* start app task */
  ret_boo = NES_TASK_CREATE((u8 *)"nes",
                   nes_entry,
                   NULL,
                   p_cfg->task_prio,
                   p_nes_stk,
                   NES_STK_SIZE); 

	NES_ASSERT(ret_boo == TRUE);
	
  return TRUE;
}

extern "C" void nes_emu_stop(void)
{
	NES_PRINT("nes emu stop\n");
	
	GameLooping = FALSE;
	
  return;
}


void nes_emu_blt(void *p_buffer, int x, int y, int w, int h)
{
	NES_ASSERT(g_emu_cfg.gpe_blt != NULL);
	
  g_emu_cfg.gpe_blt(p_buffer, x, y, w, h);
}

void nes_emu_get_joy_key(u32 pad_idx, u32 *p_key)
{
	if(g_emu_cfg.joy_key_get != NULL)
	{
  	g_emu_cfg.joy_key_get(pad_idx, p_key);
  }
}

void nes_emu_get_irda_key(u32 *p_key)
{
	if(g_emu_cfg.irda_key_get != NULL)
	{
  	g_emu_cfg.irda_key_get(p_key);
  }
}

void nes_emu_exit(BOOL is_unsupport)
{
	NES_ASSERT(g_emu_cfg.emu_exit != NULL);
	
  g_emu_cfg.emu_exit(is_unsupport);

  if(p_nes_stk != NULL)
  {
		NES_FREE(p_nes_stk);

		p_nes_stk = NULL;
  }

  NES_MEMSET(&g_emu_cfg, 0, sizeof(nes_emu_cfg_t));
}
