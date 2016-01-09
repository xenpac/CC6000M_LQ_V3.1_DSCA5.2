#ifndef _EZX_SOUND_MGR_H_
#define _EZX_SOUND_MGR_H_

#include "nes/types.h"
#include "nes/sound_mgr.h"

#define SAMPLE_RATE		22050
//#define SOUND_BUF_LEN	( SAMPLE_RATE / 30 * 2 )
#define SOUND_BUF_LEN	735 * 2
#define SAMPLE_BITS		16

class ezx_sound_mgr : public sound_mgr
{
public:
  ezx_sound_mgr(bool);
  ~ezx_sound_mgr();
  
  void reset() {}
  
  // lock down for a period of inactivity
  void freeze() {}
  void thaw() {}
  
  void clear_buffer() {};
  
  boolean lock(sound_buf_pos, void** buf, uint32* buf_len);
  void unlock();
  
  int get_sample_rate();
  int get_sample_bits();
  
  // returns SOUND_BUF_LOW or SOUND_BUF_HIGH
  sound_buf_pos get_currently_playing_half();
  
  boolean IsNull();
  
  void ezx_pause(bool, bool);
  
 private:
  int dspfd;
  int buffer_locked;
//  uint8 buffer[8840];
};

#endif // _EZX_SOUND_MGR_H_


