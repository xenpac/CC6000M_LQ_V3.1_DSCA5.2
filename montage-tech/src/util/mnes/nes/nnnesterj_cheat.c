#include "types.h"
#include <stdio.h>
#include "nnnesterj_cheat.h"
#include "nes_emu.h"


static void
set_membase (nes_nnnesterj_cheatinfo_t *ci, uint8 *RAM_base, uint8 *SRAM_base)
{
  if (ci->cheatinfo.address < 0x10000)
  {
    ci->mem_base = RAM_base;
    ci->mem_mask = 0x7ff;
  }
  else
  {
    ci->mem_base = SRAM_base;
    ci->mem_mask = 0x1fff;
  }
  
  ci->cheatinfo.address &= ci->mem_mask;
}


int
nnnesterj_cheat_load (nes_nnnesterj_cheatinfo_t *nes_nnnesterj_cheatinfo, 
                      int *nes_nnnesterj_cheatinfo_size, 
                      uint8 *RAM_base,
                      uint8 *SRAM_base,
                      const uint16 *filename)
{
  hfile_t fp = NULL;
  uint32 revision;
  int size;
  nes_nnnesterj_cheatinfo_t *p;
  int i;
  
  fp = vfs_open(filename, VFS_READ);
  if (fp == NULL) goto error;
  
  revision = 0;
  if (vfs_read (&revision, 4, 1, fp) < 4) goto error;
  
  if (vfs_read (&size, 4, 1, fp) < 4) goto error;
  if (size > *nes_nnnesterj_cheatinfo_size - 1) 
    size = *nes_nnnesterj_cheatinfo_size - 1;
  
  vfs_seek(fp, 0x10, VFS_SEEK_SET);
  
  if (revision == 0) 
  {
    /* revision 0 */
    p = nes_nnnesterj_cheatinfo;
    for (i = 0; i < size; ++i)
    {
      if (vfs_read (p->cheatinfo.comment, 17, 1, fp) < 17) goto error;
      if (vfs_read (&(p->cheatinfo.address), 4, 1, fp) < 4) goto error;
      if (vfs_read (&(p->cheatinfo.data), 1, 1, fp) < 1) goto error;
      if (vfs_read (&(p->cheatinfo.check_flag), 2, 1, fp) < 2) goto error;
      p->cheatinfo.flag = 0;
      p->cheatinfo.size = 0;
      
      set_membase (p, RAM_base, SRAM_base);
      
      ++p;
    }
  }
  else if (revision == 1)
  {
    /* revision 1 */
    p = nes_nnnesterj_cheatinfo;
    for (i = 0; i < size; ++i)
    {
      if (vfs_read (&(p->cheatinfo), sizeof(nnnesterj_cheatinfo_t), 1, fp) < sizeof(nnnesterj_cheatinfo_t)) goto error;
      
      set_membase (p, RAM_base, SRAM_base);
      
      ++p;
    }
  } 
  else 
  {
    /* unknown */
    goto error;
  }
  
  vfs_close (fp);
  *nes_nnnesterj_cheatinfo_size = size;
  
  return 1;
  
error:
  *nes_nnnesterj_cheatinfo_size = 0;
  if (fp) { vfs_close(fp); fp = NULL; }
  return 0;
}


