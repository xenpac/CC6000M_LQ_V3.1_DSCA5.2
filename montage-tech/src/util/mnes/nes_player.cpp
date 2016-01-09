/*
 * NES for MOTO EZX Modile Phone
 * Copyright (C) 2006 OopsWare. CHINA.
 *
 * This program is NES_FREE software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: nes_player.cpp,v 0.10 2006/07/05 $
 */
#include "string.h"
#include "nes_player.h"
//#include <qapplication.h>

#include "nes_sound_mgr.h"

#include "sys_types.h"
#include "nes_emu.h"
#include "lib_rect.h"
#include "nes/nes.h"

#include "gamewidget.h"

extern nes_emu_cfg_t g_emu_cfg;

class NES_GameWidget : public GameWidget 
{
  public:
    NES_GameWidget() {};
  	~NES_GameWidget() {};
};

static uint32 frame_count = 0;

uint16 get_nesscreen_pixel_color(int, int)
{
  /* NOT SUPPORTED */
  return 0; 
}

static void clear_keypad(NES *emu)
{
	NES_pad *pad = NULL;
	int i = 0;
	
	for(i = 0; i < 4; i++)
	{
		pad = emu->get_pad(i);

		pad->nes_A = false;
		pad->nes_B = false;
		pad->nes_LEFT = false;
		pad->nes_RIGHT = false;
		pad->nes_UP = false;
		pad->nes_DOWN = false;
		pad->nes_SELECT = false;
		pad->nes_START = false;
	}
}

static u32 old_irda_ticks = 0;
static u32 old_joy_ticks = 0;
static u32 key_joy[4] = {0}, key_irda = 0;

static BOOL nes_get_keypad(NES *emu)
{
	NES_pad *pad = NULL;
	u32 key[4] = {0};
	unsigned int i = 0;
	u32 new_ticks = 0;
	BOOL is_key_get = FALSE;

	new_ticks = NES_GET_TICKS();

	if((new_ticks - old_irda_ticks) > 10)
	{
		key_irda = 0;
		
		nes_emu_get_irda_key(&key_irda);

		old_irda_ticks = new_ticks;

		is_key_get = TRUE;
	}

	if((new_ticks - old_joy_ticks) > 3)
	{
		key[0] = 0;
		key[1] = 0;
		key[2] = 0;
		key[3] = 0;
		
		nes_emu_get_joy_key(4, key_joy);

		old_joy_ticks = new_ticks;

		is_key_get = TRUE;
	}

	if(is_key_get)
	{
		key[0] = key_joy[0] | key_irda;
		key[1] = key_joy[1];
		key[2] = key_joy[2];
		key[3] = key_joy[3];

		for(i = 0; i < 4; i++)
		{
			pad = emu->get_pad(i);

			pad->nes_A = (bool)(key[i] & NES_VKEY_A);
			pad->nes_B = (bool)(key[i] & NES_VKEY_B);
			pad->nes_LEFT = (bool)(key[i] & NES_VKEY_LEFT);
			pad->nes_RIGHT = (bool)(key[i] & NES_VKEY_RIGHT);
			pad->nes_UP= (bool)(key[i] & NES_VKEY_UP);
			pad->nes_DOWN = (bool)(key[i] & NES_VKEY_DOWN);
			pad->nes_SELECT = (bool)(key[i] & NES_VKEY_SELECT);
			pad->nes_START = (bool)(key[i] & NES_VKEY_START);

			if(key[i] & NES_VKEY_EXIT)
			{
				return FALSE;
			}

			if(key[i] & NES_VKEY_RESET)
			{
				emu->reset();
			}
		}
	}
	
	return TRUE;
}

ezx_sound_mgr *snd_mgr = NULL;
NES *emu = NULL;

static void nes_render_screen()
{
	nes_emu_blt(VideoBuffer, 32, 0, 256, 240);
	
	return;
}

void run_nes_emulator(uint16 *fn)
{
	uint16 *fb = NULL;
	bool ret_boo = false;
	bool is_unsupport = false;
	
	frame_count = 0;

	wgGame = new NES_GameWidget();
	if(wgGame == NULL)
	{
		NES_PRINT("@#new game widget failed!\n");

		is_unsupport = true;
		goto finish;
	}

	fb = VideoBuffer + 32;
	fb -= 8;
	
	snd_mgr = new ezx_sound_mgr(false);
	if(snd_mgr == NULL)
	{
		NES_PRINT("@#new sound mgr failed!\n");

		is_unsupport = true;
		
		goto finish;
	}

	ret_boo = snd_mgr->initialize();
	if(ret_boo == false)
	{
		NES_PRINT("@#init sound mgr failed!\n");

		is_unsupport = true;
		goto finish;
	}

	emu = new NES(snd_mgr);
	if(emu == NULL)
	{
		NES_PRINT("@#new nes emu failed!\n");

		is_unsupport = true;
		goto finish;
	}

	ret_boo = emu->initialize(fn);
	if(ret_boo == false)
	{
		NES_PRINT("@#init nes emu failed!\n");

		is_unsupport = true;
		goto finish;
	}

	
	emu->set_exsound_enable(true);
	
	nes_render_screen();

	GameLooping = true;

	if(g_emu_cfg.frame_skip != 0)
	{
		//zero means no skip.
		GameFrameSkip = g_emu_cfg.frame_skip;
	}
	else
	{
		GameFrameSkip = 1;
	}

	clear_keypad(emu);

	old_joy_ticks = NES_GET_TICKS();
	old_irda_ticks = NES_GET_TICKS(); 

	key_irda = 0;
	key_joy[0] = 0;
	key_joy[1] = 0;
	key_joy[2] = 0;
	key_joy[3] = 0;
	
	while(GameLooping) 
	{
		if(!nes_get_keypad(emu))
		{
			break;
		}

		// Frame Skip
		if (frame_count == 0) 
		{
			emu->emulate_frame (fb);
			nes_render_screen();
		} 
		else
		{
			emu->emulate_frame_skip();
		}
		
		frame_count++;

		frame_count %= GameFrameSkip;

		#ifdef WIN32		
		NES_TASK_SLEEP(10);
		#endif		

	}

	NES_PRINT("nes emu release rsc\n");

finish:

	if(emu) 
	{
		delete emu;
		emu = NULL;
	}

	if(snd_mgr) 
	{
		delete snd_mgr;
		snd_mgr = NULL;
	}
	
	if(wgGame) 
	{
		delete wgGame;
		wgGame = NULL;
	}

	if(VideoBuffer != NULL)
	{
		NES_FREE(VideoBuffer);
		VideoBuffer = NULL;
	}

	NES_PRINT("nes emu exit\n");
	nes_emu_exit(is_unsupport);
	
	NES_TASK_EXIT();
}

void run_nes_emulator_on_ram(void *p_rom, unsigned int rom_size)
{
	uint16 *fb = NULL;
	bool ret_boo = false;
	bool is_unsupport = false;
	
	frame_count = 0;

	if(p_rom == NULL)
	{
		is_unsupport = true;
		goto finish;
	}

	wgGame = new NES_GameWidget();
	if(wgGame == NULL)
	{
		NES_PRINT("@#new game widget failed!\n");

		is_unsupport = true;
		goto finish;
	}

	fb = VideoBuffer + 32;
	fb -= 8;
	
	snd_mgr = new ezx_sound_mgr(false);
	if(snd_mgr == NULL)
	{
		NES_PRINT("@#new sound mgr failed!\n");

		is_unsupport = true;
		goto finish;
	}

	ret_boo = snd_mgr->initialize();
	if(ret_boo == false)
	{
		NES_PRINT("@#init sound mgr failed!\n");

		is_unsupport = true;
		goto finish;
	}

	emu = new NES(snd_mgr);
	if(emu == NULL)
	{
		NES_PRINT("@#new nes emu failed!\n");

		is_unsupport = true;
		goto finish;
	}

	ret_boo = emu->initialize_rom_on_ram(p_rom);
	if(ret_boo == false)
	{
		NES_PRINT("@#init nes emu failed!\n");

		is_unsupport = true;
		goto finish;
	}

	emu->set_exsound_enable(true);
	
	nes_render_screen();

	GameLooping = true;

	if(g_emu_cfg.frame_skip != 0)
	{
		//zero means no skip.
		GameFrameSkip = g_emu_cfg.frame_skip;
	}
	else
	{
		GameFrameSkip = 1;
	}

	clear_keypad(emu);

	old_joy_ticks = NES_GET_TICKS();
	old_irda_ticks = NES_GET_TICKS(); 

	key_irda = 0;
	key_joy[0] = 0;
	key_joy[1] = 0;
	key_joy[2] = 0;
	key_joy[3] = 0;
	
	while(GameLooping) 
	{
		if(!nes_get_keypad(emu))
		{
			break;
		}	

		// Frame Skip
		if (frame_count == 0) 
		{
			emu->emulate_frame (fb);
			nes_render_screen();
		} 
		else
		{
			emu->emulate_frame_skip();
		}
		
		frame_count++;

		frame_count %= GameFrameSkip;

		#ifdef WIN32		
		NES_TASK_SLEEP(10);
		#endif		

	}

	NES_PRINT("nes emu release rsc\n");

finish:

	if(emu) 
	{
		delete emu;
		emu = NULL;
	}

	if(snd_mgr) 
	{
		delete snd_mgr;
		snd_mgr = NULL;
	}
	
	if(wgGame) 
	{
		delete wgGame;
		wgGame = NULL;
	}

	if(VideoBuffer != NULL)
	{
		NES_FREE(VideoBuffer);
		VideoBuffer = NULL;
	}

	NES_PRINT("nes emu exit\n");
	nes_emu_exit(is_unsupport);
	
	NES_TASK_EXIT();
}
