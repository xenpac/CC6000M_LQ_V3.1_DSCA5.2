/* -*- mode: c++; tab-width: 8; c-file-style: "nester" -*- */

/*
** nester - NES emulator
** Copyright (C) 2000  Darren Ranalli
**
** This program is NES_FREE software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

/* 
** NesterDC by Ken Friece
** This routine was so different than the original, that I included 
** diff output at the bottom of the routine. The orginal is also called
** nes_rom.cpp.original
*/

//#define ENABLE_UNZIP

#include "nester.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nes_rom.h"
#include "nes.h"
#include "debug.h"
#include "sys_types.h"
#include "nes_emu.h"

#ifdef ENABLE_UNZIP
# include "unzip.h"

int
findAlternateExtInZip(unzFile file, const char *path, const char *ext)
{
  const char *p;
  char *name, *q;
  int ret;

  p = strrchr(path, '/');
  if (!p)
    p = path;

  name = (char *)NES_MALLOC(strlen(path) + strlen(ext) + 1);
  strcpy(name, p);

  q = strrchr(name, '.');
  if (!q) {
    q = name + strlen(name);
    strcpy(q, ".");
  }
  strcpy(q + 1, ext);

  ret = unzLocateFile(file, name, 2/*case insensivity*/);

  NES_FREE(name);
  return ret;
}
#endif

bool
NES_ROM::initialize (const uint16 *filename)
{
  mapper = 0;
  crc = 0;
  fds = 0;
  image_type = invalid_image;
  
  trainer    = NULL;
  ROM_banks  = NULL;
  VROM_banks = NULL;
  
  *rom_name = 0;
  *rom_path = 0;
  
  return load_rom (filename);
}


bool
NES_ROM::initialize_on_ram (void *p_rom)
{
  mapper = 0;
  crc = 0;
  fds = 0;
  image_type = invalid_image;
  
  trainer    = NULL;
  ROM_banks  = NULL;
  VROM_banks = NULL;
  
  *rom_name = 0;
  *rom_path = 0;
  
  return load_rom_on_ram (p_rom);
}


NES_ROM::~NES_ROM()
{
  if (trainer)
  {
  	NES_FREE (trainer);
	}
  if (ROM_banks)   
  {
  	NES_FREE (ROM_banks);
	}
  if (VROM_banks) 
  {
  	NES_FREE (VROM_banks);
	}
}


uint8
NES_ROM::normalize_bank_num (uint8 bank_num)
{
  uint8 rv = 0x1;
  
  if (bank_num == 0) return 0;
  
  while (bank_num > rv)
    rv <<= 1;
  
  return rv;
}


bool
NES_ROM::load_rom (const uint16 *filename)
{
  hfile_t fp = NULL;
  bool isZip = false;

  NES_PRINT("@ load rom\n");
  
  set_path_info (filename);
  
#ifdef ENABLE_UNZIP
  unzFile zipfp;

  zipfp = unzOpen(filename);
  if (zipfp)
  {
    isZip = true;

    if (findAlternateExtInZip(zipfp, filename, "nes") != UNZ_OK)
      if (findAlternateExtInZip(zipfp, filename, "fds") != UNZ_OK)
        unzGoToFirstFile(zipfp);

    if (unzOpenCurrentFile(zipfp) != UNZ_OK)
      goto error;

    if (unzReadCurrentFile(zipfp, (voidp)&header, 16) != 16)
      goto error;
  }
  else
#endif
  {
  	NES_PRINT("@ vfs open rom\n");
    fp = vfs_open((u16 *)filename, VFS_READ);
    if (!fp) 
    {
    	char rom_path[ROM_PATH_LEN] = {0};
    	str_uni2asc((u8 *)rom_path, filename);
    	
    	NES_PRINT("@ vfs open rom failed %s\n", rom_path);
    	goto error;
  	}

  	NES_PRINT("@ vfs read rom\n");
    if (vfs_read ((void*)&header, 16, 1, fp) != 16) 
    {
    	NES_PRINT("@vfs read rom failed\n");
      goto error;
    }
  }
  
  /* patch for 260-in-1(#235) */
  header.num_16k_rom_banks = (!header.dummy) ? 256 : header.dummy;
  
  /* set num_banks to 2^x */
  header.num_16k_rom_banks = normalize_bank_num(header.num_16k_rom_banks);
  header.num_8k_vrom_banks = normalize_bank_num(header.num_8k_vrom_banks);
  
  if(!NES_MEMCMP(header.id, "NES", 3) && (header.ctrl_z == 0x1A)) 
  {
#ifdef ENABLE_UNZIP
    if (isZip)
      image_type = load_nes_rom ((hfile_t)zipfp, isZip);
    else
#endif
      image_type = load_nes_rom (fp, isZip);
  }
  else if(!NES_MEMCMP(header.id, "FDS", 3) && (header.ctrl_z == 0x1A))
  {
#ifdef ENABLE_UNZIP
    if (isZip)
      image_type = load_fds_rom ((hfile_t)zipfp, isZip);
    else
#endif
      image_type = load_fds_rom (fp, isZip);
  }
  else
  {
  	NES_PRINT("header id error, %s\n", header.id);
    goto error;
  }
  
#ifdef ENABLE_UNZIP
  if (isZip)
  {
      unzCloseCurrentFile(zipfp);
      unzClose(zipfp);
  }
  else
#endif
	{
      vfs_close (fp);
			fp = NULL;
  }
  
  if (image_type == invalid_image)
  {
  	NES_PRINT("nes type error, %d\n", image_type);
    goto error;
  }
  correct_rom_image ();
  
  return true;
  
error:
#ifdef ENABLE_UNZIP
  unzCloseCurrentFile(zipfp);
  unzClose(zipfp);
#else
  if (fp) 
  {
  	vfs_close (fp);
  	fp = NULL;
	}
#endif
  image_type = invalid_image;
  
  return false;
}

bool
NES_ROM::load_rom_on_ram (void *p_rom)
{
  hfile_t fp = NULL;
  bool isZip = false;

  if(p_rom == NULL)
  {
		return false;
  }

  NES_PRINT("@ load rom on ram\n");

	NES_MEMCPY((void *)&header, p_rom, 16);
 
  /* patch for 260-in-1(#235) */
  header.num_16k_rom_banks = (!header.dummy) ? 256 : header.dummy;
  
  /* set num_banks to 2^x */
  header.num_16k_rom_banks = normalize_bank_num(header.num_16k_rom_banks);
  header.num_8k_vrom_banks = normalize_bank_num(header.num_8k_vrom_banks);
  
  if(!NES_MEMCMP(header.id, "NES", 3) && (header.ctrl_z == 0x1A)) 
  {
    image_type = load_nes_rom_on_ram(p_rom, false);
  }
  else if(!NES_MEMCMP(header.id, "FDS", 3) && (header.ctrl_z == 0x1A))
  {
    image_type = load_fds_rom_on_ram(p_rom, false);
  }
  else
  {
    goto error;
  }
  
  if (image_type == invalid_image)
  {
    goto error;
  }
  
  correct_rom_image ();
  
  return true;
  
error:
  image_type = invalid_image;
  
  return false;
}


NES_ROM::rom_image_type
NES_ROM::load_nes_rom (hfile_t fp, bool isZip)
{
  size_t nread;
  
  ROM_banks = (uint8*)NES_MALLOC(header.num_16k_rom_banks * (16*1024));
  if (!ROM_banks) 
  {
  	NES_PRINT("load nes rom failed 0, %d\n", header.num_16k_rom_banks);
  	return invalid_image;
  }
  VROM_banks = (uint8*)NES_MALLOC(header.num_8k_vrom_banks * (8*1024));
  if (!VROM_banks) 
  {
  	NES_PRINT("load nes rom failed 1, %d\n", header.num_8k_vrom_banks);
  	return invalid_image;
  }
  if (has_trainer()) 
  {
    trainer = (uint8*)NES_MALLOC(TRAINER_LEN);
    if(!trainer) 
    {
  		NES_PRINT("load nes rom failed 2!\n");
    	return invalid_image;
    }
    
#ifdef ENABLE_UNZIP
    if (isZip)
    {
      if (unzReadCurrentFile ((unzFile *)fp, trainer, TRAINER_LEN) != TRAINER_LEN)
        return invalid_image;
    }
    else
#endif
      if (vfs_read (trainer, TRAINER_LEN, 1, fp) != TRAINER_LEN) 
      {
  			NES_PRINT("load nes rom failed 3!\n");
      
        return invalid_image;
      }
  }
  
  nread = 16 * 1024 * header.num_16k_rom_banks; 
  if (nread)
  {
#ifdef ENABLE_UNZIP
    if (isZip)
    {
      if (unzReadCurrentFile((unzFile *)fp, ROM_banks, nread) != nread)
        return invalid_image;
    }
    else
#endif
      if (vfs_read(ROM_banks, nread, 1, fp) != nread) 
      {
  			NES_PRINT("load nes rom failed 4!\n");
      
        return invalid_image;
      }
  }
  
  nread = 8 * 1024 * header.num_8k_vrom_banks;
  if (nread)
  {
#ifdef ENABLE_UNZIP
    if (isZip)
    {
      if (unzReadCurrentFile((unzFile *)fp, VROM_banks, nread) != nread)
        return invalid_image;
    }
    else
#endif
      //if (vfs_read(VROM_banks, nread, 1, fp) != nread) 
      if (vfs_read(VROM_banks, nread, 1, fp) == 0) 
      {
  			NES_PRINT("load nes rom failed 5!\n");
      
        return invalid_image;
      }
  }
  
  crc = calc_crc();
  
  return ines_image;
}

NES_ROM::rom_image_type
NES_ROM::load_nes_rom_on_ram (void *p_rom, bool isZip)
{
  size_t nread;
  uint8 *p_addr = (uint8 *)p_rom;
  
  
  ROM_banks = (uint8*)NES_MALLOC(header.num_16k_rom_banks * (16*1024));
  if (!ROM_banks) return invalid_image;
  
  VROM_banks = (uint8*)NES_MALLOC(header.num_8k_vrom_banks * (8*1024));
  if (!VROM_banks) return invalid_image;


  p_addr += 16;
  
  if (has_trainer()) 
  {
    trainer = (uint8*)NES_MALLOC(TRAINER_LEN);
    if(!trainer) 
    {
    	return invalid_image;
    }

    NES_MEMCPY(trainer, p_addr, TRAINER_LEN);

    p_addr += TRAINER_LEN;
  }
  
  nread = 16 * 1024 * header.num_16k_rom_banks; 
  if (nread)
  {
    NES_MEMCPY(ROM_banks, p_addr, nread);

    p_addr += nread;
  }
  
  nread = 8 * 1024 * header.num_8k_vrom_banks;
  if (nread)
  {
		NES_MEMCPY(VROM_banks, p_addr, nread);
		p_addr += nread;
  }
  
  crc = calc_crc();
  
  return ines_image;
}


NES_ROM::rom_image_type
NES_ROM::load_fds_rom (hfile_t fp, bool isZip)
{
  int i;
  uint8 disk_num = header.num_16k_rom_banks;
  
  header.id[0] = 'N';
  header.id[1] = 'E';
  header.id[2] = 'S';
  header.num_16k_rom_banks *= 4;
  header.num_8k_vrom_banks = 0;
  header.flags_1 = 0x40;
  header.flags_2 = 0x10;
  NES_MEMSET(header.reserved, 0, 8);
  
  ROM_banks = (uint8*)NES_MALLOC(16 + 65500 * disk_num);
  if(!ROM_banks) return invalid_image;
  
  VROM_banks = (uint8*)NES_MALLOC (1); /* dummy */
  if(!VROM_banks) return invalid_image;
  
#ifdef ENABLE_UNZIP
  if (isZip) {
    int nread = 65500 * disk_num;
    if (unzReadCurrentFile ((unzFile *)fp, ROM_banks + 16, nread) != nread)
      return invalid_image;
  }
  else
#endif
    if (vfs_read (ROM_banks + 16, 65500 * disk_num, 1, fp) != 65500 * disk_num)
      return invalid_image;
  
  ROM_banks[0] = 'F';
  ROM_banks[1] = 'D';
  ROM_banks[2] = 'S';
  ROM_banks[3] = 0x1A;
  ROM_banks[4] = disk_num;
  
  mapper = 20;
  fds = (ROM_banks[0x1f] << 24) | (ROM_banks[0x20] << 16) |
  (ROM_banks[0x21] <<  8) | (ROM_banks[0x22] <<  0);
  
  for(i = 0; i < disk_num; i++)
  {
    int file_num = 0;
    uint32 pt = 16 + (65500 * i) + 0x3a;
    while(ROM_banks[pt] == 0x03)
    {
      pt += 0x0d;
      pt += ROM_banks[pt] + ROM_banks[pt + 1] * 256 + 4;
      file_num++;
    }
    ROM_banks[16 + (65500 * i) + 0x39] = file_num;
  }
  
  return fds_image;
}

NES_ROM::rom_image_type
NES_ROM::load_fds_rom_on_ram (void *p_rom, bool isZip)
{
  int i;
  uint8 disk_num = header.num_16k_rom_banks;
	uint8 *p_addr = (uint8 *)p_rom;
  
  header.id[0] = 'N';
  header.id[1] = 'E';
  header.id[2] = 'S';
  header.num_16k_rom_banks *= 4;
  header.num_8k_vrom_banks = 0;
  header.flags_1 = 0x40;
  header.flags_2 = 0x10;
  NES_MEMSET(header.reserved, 0, 8);
  
  ROM_banks = (uint8*)NES_MALLOC(16 + 65500 * disk_num);
  if(!ROM_banks) return invalid_image;
  
  VROM_banks = (uint8*)NES_MALLOC (1); /* dummy */
  if(!VROM_banks) return invalid_image;

  p_addr += 16;
  
	NES_MEMCPY(ROM_banks + 16, p_addr, 65500 * disk_num);

	p_addr += (65500 * disk_num);
	
  ROM_banks[0] = 'F';
  ROM_banks[1] = 'D';
  ROM_banks[2] = 'S';
  ROM_banks[3] = 0x1A;
  ROM_banks[4] = disk_num;
  
  mapper = 20;
  fds = (ROM_banks[0x1f] << 24) | (ROM_banks[0x20] << 16) |
  (ROM_banks[0x21] <<  8) | (ROM_banks[0x22] <<  0);
  
  for(i = 0; i < disk_num; i++)
  {
    int file_num = 0;
    uint32 pt = 16 + (65500 * i) + 0x3a;
    while(ROM_banks[pt] == 0x03)
    {
      pt += 0x0d;
      pt += ROM_banks[pt] + ROM_banks[pt + 1] * 256 + 4;
      file_num++;
    }
    ROM_banks[16 + (65500 * i) + 0x39] = file_num;
  }
  
  return fds_image;
}


void
NES_ROM::correct_rom_image ()
{
  bool header_valid = true;
  int i;
  
  mapper = (header.flags_1 >> 4);
  monitor_type = monitor_ntsc;
  
#include "nes_rom_correct.cpp"
  
  // if there is anything in the reserved bytes,
  // don't trust the high nybble of the mapper number
  for(i = 0; i < sizeof(header.reserved); ++i) 
  {
    if(header.reserved[i] != 0x00) 
    {
      header_valid = false;
      break;
    }
  }
  
  if (header_valid) 
    mapper |= (header.flags_2 & 0xF0);
  else
    LOG("Invalid NES header ($8-$F)");
}


uint32
NES_ROM::calc_crc ()
{
  uint32 i, j;
  uint32 c, crctable[256];
  uint32 nes_crc = 0;
  
  for(i = 0; i < 256; i++) {
    c = i;
    for (j = 0; j < 8; j++) {
      if (c & 1)
	c = (c >> 1) ^ 0xedb88320;
      else
	c >>= 1;
    }
    crctable[i] = c;
  }
  
  for(i = 0; i < header.num_16k_rom_banks; i++) {
    c = ~nes_crc;
    for(j = 0; j < 0x4000; j++)
      c = crctable[(c ^ ROM_banks[i*0x4000+j]) & 0xff] ^ (c >> 8);
    nes_crc = ~c;
  }
  
  return nes_crc;
}


const char* 
NES_ROM::GetRomCrc() 
{
  static char rom_crc[12];
  if (fds)
    sprintf(rom_crc, "F%x", fds);
  else
    sprintf(rom_crc, "%x", crc);
  return rom_crc;
}


void
NES_ROM::set_path_info(const uint16 * fn)
{
  int basename = uni_strlen(fn);
  const uint16 *p;
  uint16 *q;
  const uint16 *r;
  
  while (basename)
  {
    if((fn[basename] == char_asc2uni('\\')) || (fn[basename] == char_asc2uni('/')))
    {
      ++basename;
      break;
    }
    
    --basename;
  }
  
  p = fn + basename;
  q = rom_name;
  while (*p)
    *q++ = *p++;
  *q = 0;
  
  p = fn;
  q = rom_path;
  r = fn + basename;
  while (p != r)
    *q++ = *p++;
  *q = 0;
}


