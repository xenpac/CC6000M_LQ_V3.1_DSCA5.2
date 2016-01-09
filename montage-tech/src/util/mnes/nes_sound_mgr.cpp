#include "nes/types.h"
#include "nes_sound_mgr.h"
#include "nes/debug.h"

#include "ezxaudio.h"
#include <string.h>

#include "nes_emu.h"

extern nes_emu_cfg_t g_emu_cfg;

#define NES_FREGMENT	(10 | 4 << 16)
#define NES_CHANNELS	1

ezx_sound_mgr::ezx_sound_mgr(bool mute)
{
  buffer_locked = false;
  aud_pcm_param_vsb_t aud_parm = {0};

  if ( mute )
  {
  	dspfd = -1;

  	aud_mute_onoff_vsb(g_emu_cfg.p_aud_dev, TRUE);
  	aud_stop_vsb(g_emu_cfg.p_aud_dev);
  }
  else
  {
  	//dspfd = ezx_open_dsp ( SAMPLE_RATE, NES_CHANNELS, AFMT_S16_LE, NES_FREGMENT );

		dspfd = 1;

  	aud_mute_onoff_vsb(g_emu_cfg.p_aud_dev, FALSE);

		aud_stop_vsb(g_emu_cfg.p_aud_dev);



  	aud_start_vsb(g_emu_cfg.p_aud_dev, AUDIO_PCM, AUDIO_NO_FILE);

		//aud_parm.bits = 16;
		//aud_parm.channel_mode = AUDIO_CHANNEL_STEREO_VSB;
		//aud_parm.exist_channels = NES_CHANNELS;
		//aud_parm.is_big_endian = FALSE;
		//aud_parm.is_stereo = TRUE;
		//aud_parm.play_mode = AUDIO_PCM_MODE_VSB;
		//aud_parm.sample = AUDIO_SAMPLE_22_VSB;

		aud_set_pcm_param_vsb(g_emu_cfg.p_aud_dev, &aud_parm);
  }

  NES_MEMSET (EzxAudioBuffer, 0, EzxAudioBufferSize); // sizeof(EzxAudioBuffer)
}

ezx_sound_mgr::~ezx_sound_mgr()
{
  if (dspfd >= 0)
  {
  	//ezx_close_dsp(dspfd);
  	aud_stop_vsb(g_emu_cfg.p_aud_dev);
	}
  dspfd = -1;
}


boolean ezx_sound_mgr::lock(sound_buf_pos, void** buf, uint32* buf_len)
{
  if (buffer_locked)
  {
  	return false;
  }

  buffer_locked = true;
  *buf = EzxAudioBuffer;
  *buf_len = SOUND_BUF_LEN;

  return true;
}

extern int sprintf (char *s, const  char *p_format, ...);

void ezx_sound_mgr::unlock()
{
  if (!buffer_locked)
  {
  	return;
  }

  buffer_locked = false;

  if ( dspfd >= 0 )
  {
  	//ezx_play_dsp(dspfd, EzxAudioBuffer, SOUND_BUF_LEN);

  	aud_file_pushesbuffer_vsb(g_emu_cfg.p_aud_dev, (u32)EzxAudioBuffer, SOUND_BUF_LEN, 0);
	}

}


/* return the alternation of SOUND_BUF_LOW and SOUND_BUF_HIGH,
   because of NES_APU::DoFrame implement. */
sound_mgr::sound_buf_pos ezx_sound_mgr::get_currently_playing_half()
{
  static sound_mgr::sound_buf_pos last = SOUND_BUF_HIGH;

  if (last == SOUND_BUF_HIGH)
    last = SOUND_BUF_LOW;
  else
    last = SOUND_BUF_HIGH;

  return last;
}

void ezx_sound_mgr::ezx_pause(bool pause, bool mute)
{
	aud_pcm_param_vsb_t aud_parm = {0};

	if ( pause )
	{

		if (dspfd >= 0)
		{
			//ezx_close_dsp(dspfd);
			aud_stop_vsb(g_emu_cfg.p_aud_dev);
		}
  		dspfd = -1;

	}
	else
	{
		if (!mute && (dspfd < 0))
		{
			buffer_locked = false;
			memset (EzxAudioBuffer, 0, EzxAudioBufferSize);
			//dspfd = ezx_open_dsp ( SAMPLE_RATE, NES_CHANNELS, AFMT_S16_LE, NES_FREGMENT );

	  	aud_mute_onoff_vsb(g_emu_cfg.p_aud_dev, FALSE);

			aud_stop_vsb(g_emu_cfg.p_aud_dev);

			//aud_parm.bits = 16;
			//aud_parm.channel_mode = AUDIO_CHANNEL_STEREO_VSB;
			//aud_parm.exist_channels = NES_CHANNELS;
			//aud_parm.is_big_endian = FALSE;
			//aud_parm.is_stereo = TRUE;
			//aud_parm.play_mode = AUDIO_PCM_MODE_VSB;
			//aud_parm.sample = AUDIO_SAMPLE_22_VSB;

			aud_set_pcm_param_vsb(g_emu_cfg.p_aud_dev, &aud_parm);

	  	aud_start_vsb(g_emu_cfg.p_aud_dev, AUDIO_PCM, AUDIO_NO_FILE);
		}

	}
}

int ezx_sound_mgr::get_sample_rate()
{
	return (dspfd >= 0) ? SAMPLE_RATE : 0;
}

int ezx_sound_mgr::get_sample_bits()
{
	return (dspfd >= 0) ? SAMPLE_BITS : 0;
}

boolean ezx_sound_mgr::IsNull()
{
	return (dspfd >= 0) ? FALSE : TRUE;
}
