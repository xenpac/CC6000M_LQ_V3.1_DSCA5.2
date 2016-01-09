#ifndef __NES_MAPPER_163_H
#define __NES_MAPPER_163_H

#include "nes_mapper.h"
#include "nes_mapper.h"

/////////////////////////////////////////////////////////////////////
// Mapper 163
//
// TEST ROM: PAL_1.nes
// Author  : OopsWare 2006/07/07
// HomePage: http://oopsware.googlepages.com

class NES_mapper163 : public NES_mapper
{
public:
  ~NES_mapper163() {}
  
  void  Reset();
  void  MemoryWrite(uint32 addr, uint8 data);

protected:
private:
};
/////////////////////////////////////////////////////////////////////

#endif
