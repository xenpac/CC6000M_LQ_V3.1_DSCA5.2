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
 * $Id: nes_player.h,v 0.10 2006/07/05 $
 */

#ifndef _NES_PLAYER_
#define _NES_PLAYER_

#include "types.h"

void run_nes_emulator(uint16 *p_rom_path);

void run_nes_emulator_on_ram(void *p_rom, unsigned int rom_size);

#endif	// _NES_PLAYER_
