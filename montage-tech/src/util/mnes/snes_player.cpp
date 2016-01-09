/*
 * SNes9x for MOTO EZX Modile Phone
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
 * $Id: snes_player.cpp,v 0.10 2006/07/13 $
 */

#include "snes_player.h"
#include <qapplication.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "ezxaudio.h"

#include "snes/snes9x.h"
#include "snes/apu.h"
#include "snes/memmap.h"
#include "snes/soundux.h"
#include "snes/spc700.h"
#include "snes/gfx.h"
#include "snes/display.h"
#include "snes/ppu.h"
#include "snes/cpuexec.h"
#include "snes/snapshot.h"
#include "sys_types.h"
#include "nes_emu.h"

int m_nVolume = 100;		/* soundux.cpp */

struct keymap_item SNES_KEYMAP[] = {
		{"-----",	0, false },
		{"START",	1, false },
		{"SELECT",	2, false },
		{"A",		3, false },
		{"B", 		4, false },
		{"X",		5, false },
		{"Y", 		6, false },
		{"TL",		7, false },
		{"TR", 		8, false },
		{"A-Turbo",	3, true  },
		{"B-Turbo",	4, true  },
		{"X-Turbo",	5, true  },
		{"Y-Turbo",	6, true  },
		{"TL-Turbo",7, true  },
		{"TR-Turbo",8, true  },
		{"P1<>P2",	9, false  }  };

static unsigned int snes_joypad = 0;
static bool joypad_switch = false;

void do_snes_keypad()
{
	static unsigned int turbo = 0;
	turbo ++;
	
	snes_joypad = 0x80000000;
	
	bool function_keys[10] = {false, false, false, false, false, false, false, false, false, false};
	int func_index;
	
	if ( HKCS[EZX_KEY_UP].down ) 		snes_joypad |= SNES_UP_MASK;
	if ( HKCS[EZX_KEY_DOWN].down ) 		snes_joypad |= SNES_DOWN_MASK;
	if ( HKCS[EZX_KEY_LEFT].down ) 		snes_joypad |= SNES_LEFT_MASK;
	if ( HKCS[EZX_KEY_RIGHT].down )		snes_joypad |= SNES_RIGHT_MASK;
	
	for (int i=0;i<10;i++) {
		func_index = GameKeymap[i];
		if ( func_index && HKCS[i].down) {
			function_keys[ SNES_KEYMAP[func_index].keypad ] = true;
			if (HKCS[i].down && SNES_KEYMAP[func_index].turbo)
				function_keys[ SNES_KEYMAP[func_index].keypad ] = turbo & 0x6;
		}
	}

	if ( function_keys[1] )				snes_joypad |= SNES_START_MASK;
	if ( function_keys[2] )				snes_joypad |= SNES_SELECT_MASK;
	if ( function_keys[3] )				snes_joypad |= SNES_A_MASK;
	if ( function_keys[4] )				snes_joypad |= SNES_B_MASK;
	if ( function_keys[5] )				snes_joypad |= SNES_X_MASK;
	if ( function_keys[6] )				snes_joypad |= SNES_Y_MASK;
	if ( function_keys[7] )				snes_joypad |= SNES_TL_MASK;
	if ( function_keys[8] )				snes_joypad |= SNES_TR_MASK;

	// switch joypad for Snes	
	
	if ( joypad_switch ) {
		if (!function_keys[9]) {
			Settings.SwapJoypads = ( Settings.SwapJoypads ) ? 0 : 1;
		}
		joypad_switch = false;
	}
	if ( function_keys[9] )
		joypad_switch = true;

}

void S9xProcessSound();

static bool TimerStarted = false;
void StopTimer();
void InitTimer();

void run_snes_emulator(const char *fn)
{
	uint32 saved_flags;
	QString SRamSaveName;
	
	ZeroMemory (&Settings, sizeof (Settings));
	joypad_switch = false;

    Settings.SoundPlaybackRate = 4;	// 4:22050
    Settings.Stereo = TRUE;
    Settings.SoundBufferSize = 1024;
    Settings.CyclesPercentage = 100;
    Settings.DisableSoundEcho = TRUE;
    Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
    Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
    Settings.ShutdownMaster = TRUE;
    Settings.FrameTimePAL = 22000;
    Settings.FrameTimeNTSC = 16667;
    Settings.FrameTime = Settings.FrameTimeNTSC;
    Settings.DisableSampleCaching = FALSE;
    Settings.DisableMasterVolume = TRUE;
    Settings.Mouse = FALSE;
    Settings.SuperScope = FALSE;
    Settings.MultiPlayer5 = FALSE;
    Settings.ControllerOption = 0;
    Settings.Transparency = TRUE;
    Settings.SixteenBit = TRUE;
    Settings.SupportHiRes = FALSE;
    Settings.NetPlay = FALSE;
    Settings.AutoSaveDelay = 30;
    Settings.ApplyCheats = TRUE;
    Settings.TurboMode = FALSE;
    Settings.TurboSkipFrames = 15;	

	wgGame = new SNES_GameWidget( 0 );
	wgGame->doLoadConfig();

    Settings.SkipFrames = (GameFrameSkip < 0) ? AUTO_FRAMERATE : GameFrameSkip;

//	if (Settings.ForceNoTransparency)
//		Settings.Transparency = FALSE;
//	if (Settings.Transparency)
//		Settings.SixteenBit = TRUE;

    Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;

    if (!Memory.Init () || !S9xInitAPU())
		goto finish;
	
	if (!GameMute)
		S9xInitSound (Settings.SoundPlaybackRate, Settings.Stereo, Settings.SoundBufferSize);
	S9xSetSoundMute (GameMute);
	
	saved_flags = CPU.Flags;

    S9xSetRenderPixelFormat (RGB565);

    //S9xInitInputDevices ();
    
    GFX.Screen = (uint8 *) NES_MALLOC(320 * 240 * 2);
    GFX.Pitch = 320 * 2;
    
	GFX.SubScreen = (uint8 *)NES_MALLOC(512 * 480 * 2);
	GFX.ZBuffer = (uint8 *)NES_MALLOC(512 * 480 * 2);
	GFX.SubZBuffer = (uint8 *)NES_MALLOC(512 * 480 * 2);

    if (!S9xGraphicsInit ())
		goto finish;
	
	wgGame->showFullScreen();
	wgGame->UpdateRect(0, 0, 320, 240, true);
	qApp->processEvents();
	
	if (!Memory.LoadROM (fn))
		goto finish;

	SRamSaveName = wgGame->RomSaveName + ".srm";
	Memory.LoadSRAM ( SRamSaveName.local8Bit() );
	
	CPU.Flags = saved_flags;
	
	TimerStarted = false;
	if (!GameMute) InitTimer();
	
	GameLooping = true;
	while (GameLooping) {
		qApp->processEvents();
		if ( GamePauseing ) {
			wgGame->PauseProc();
			continue;
		}

		do_snes_keypad();
		S9xMainLoop();

	}

	StopTimer();
	Memory.SaveSRAM ( SRamSaveName.local8Bit() );
	
finish:
	
	S9xSetSoundMute (TRUE);
	if (so.sound_fd >= 0) 
		ezx_close_dsp(so.sound_fd);
	so.sound_fd = -1;

	Memory.Deinit ();
    S9xDeinitAPU ();	
	S9xGraphicsDeinit();

	if ( GFX.Screen ) {
		NES_FREE(GFX.Screen);
		GFX.Screen = NULL;
	}
	
	if ( GFX.SubScreen ) {
		NES_FREE(GFX.SubScreen);
		GFX.SubScreen = NULL;
	}
	if ( GFX.ZBuffer ) {
		NES_FREE(GFX.ZBuffer);
		GFX.ZBuffer = NULL;
	}
	if ( GFX.SubZBuffer ) {
		NES_FREE(GFX.SubZBuffer);
		GFX.SubZBuffer = NULL;
	}
	
	if (wgGame) {
		delete wgGame;
		wgGame = 0;
	}
	
}

void SNES_GameWidget::doGamePause(bool pause)
{
	if ( pause ) {
		
		StopTimer();

		S9xSetSoundMute (TRUE);
		if (so.sound_fd >= 0) 
			ezx_close_dsp(so.sound_fd);
		so.sound_fd = -1;
		
	} else {
		
		//Settings.APUEnabled = Settings.NextAPUEnabled = !snes_mute;
		if ( !GameMute ) {
			S9xInitSound (Settings.SoundPlaybackRate, Settings.Stereo, Settings.SoundBufferSize);
			S9xSetSoundMute (FALSE);
			InitTimer();
		}

		Settings.SkipFrames = (GameFrameSkip < 0) ? AUTO_FRAMERATE : GameFrameSkip;
			
	}
}

bool SNES_GameWidget::doGameReset()
{
	S9xReset();
	return true;
}

bool SNES_GameWidget::doSaveGame(const uint16 *fn)
{
	return S9xFreezeGame (fn);
}

bool SNES_GameWidget::doLoadGame(const uint16 *fn)
{
	return S9xLoadSnapshot(fn);
}

QString SNES_GameWidget::doConfigItemPerfix(const QString &s) const
{
	return "SNES_" + s;
}

int SNES_GameWidget::getKeymapCount()
{
	return sizeof(SNES_KEYMAP) / sizeof(struct keymap_item);
}

struct keymap_item * SNES_GameWidget::getKeymapData()
{
	return &SNES_KEYMAP[0];
}

static int Rates[8] =
{
    //0, 8192, 11025, 16500, 22050, 29300, 36600, 44100
    0, 8000, 11025, 16000, 22050, 32000, 44100, 48000
};

static int BufferSizes [8] =
{
    0, 256, 256, 256, 512, 512, 1024, 1024
};

static long log2 (long num)
{
    long n = 0;
   	while (num >>= 1) n++;
    return (n);
}

bool8_32 S9xOpenSoundDevice (int mode, bool8_32 stereo, int buffer_size)
{
    int J;
	
	so.playback_rate = Rates[mode & 0x07];
	so.sixteen_bit = TRUE;
    so.stereo = stereo;
    
    if (buffer_size == 0)
		buffer_size = BufferSizes [mode & 7];
	
    if (buffer_size > MAX_BUFFER_SIZE / 4)
		buffer_size = MAX_BUFFER_SIZE / 4;
	if (so.sixteen_bit)
		buffer_size *= 2;
    if (so.stereo)
		buffer_size *= 2;
	
    int power2 = log2 (buffer_size);
    J = power2 | (3 << 16);
	if (( so.sound_fd = ezx_open_dsp( so.playback_rate, stereo ? 2 : 1, AFMT_S16_LE, J ) ) < 0) 
		return FALSE;
	
    S9xSetPlaybackRate (so.playback_rate);
    ioctl (so.sound_fd, SNDCTL_DSP_GETBLKSIZE, &so.buffer_size);
    
printf ("Rate: %d, Buffer size: %d, 16-bit: %s, Stereo: %s, Encoded: %s\n",
		so.playback_rate, so.buffer_size, so.sixteen_bit ? "yes" : "no",
		so.stereo ? "yes" : "no", so.encoded ? "yes" : "no");
	
    return TRUE;
}

void S9xMessage (int /* type */, int /* number */, const char *message)
{
    fprintf (stderr, "%s\n", message);
}

extern void DisplayChar (uint16 *Screen, uint8 c);

bool8_32 S9xDeinitUpdate (int, int)  // (int Width, int Height)
{
/*
	uint8 *Screen = (uint8 *)(&VideoBuffer[8][32]) + 2 + (IPPU.RenderedScreenHeight - 10) * 640;
	char string [16];
	sprintf (string, "%02d/%02d", IPPU.DisplayedRenderedFrameCount,(int) Memory.ROMFramesPerSecond);
	Screen += (8 - 1) * sizeof(uint16);
	for (int i = 0; i < 5; i++) {
		DisplayChar ((uint16*)Screen, string [i]);
		Screen +=  (8 - 1) * sizeof (uint16);
	}
*/	
	switch ( GameScreenMode ) {
	case 1:
		EZX_SoftStretch(GFX.Screen, 0, 0, 256, 224, 320 * 2, &VideoBuffer, 320, 240);
		if (!GamePauseing)
			wgGame->UpdateRect(0, 0, 320, 240);
		break;
	default:
		unsigned char * ps = (unsigned char *) GFX.Screen;
		unsigned char * pd = (unsigned char *)(&VideoBuffer[8]) + 64;
		for (int i=0;i<224;i++) {
			memcpy( pd, ps, 256 * 2 );
			ps += GFX.Pitch;
			pd += 320 * 2;
		}
		if (!GamePauseing)
			wgGame->UpdateRect(32, 8, 256, 224);
	}
	return TRUE;
}

void S9xAutoSaveSRAM ()
{
    //Memory.SaveSRAM ( ... );
}

void S9xSetPalette () 
{
}

uint32 S9xReadJoypad (int /* witch1 */)
{
	// in _ZAURUS witch1 allways be "0"
	return snes_joypad;
}

void S9xSyncSpeed ()
{
	S9xProcessEvents (FALSE);
    if (!Settings.TurboMode && Settings.SkipFrames == AUTO_FRAMERATE)
    {
		static struct timeval next1 = {0, 0};
		struct timeval now;
		
		while (gettimeofday (&now, NULL) < 0) ;
		if (next1.tv_sec == 0)
		{
			next1 = now;
			next1.tv_usec++;
		}
		
		if (timercmp(&next1, &now, >))
		{
			if (IPPU.SkippedFrames == 0)
			{
				do
				{
					CHECK_SOUND ();
					//		    S9xProcessEvents (FALSE);
					while (gettimeofday (&now, NULL) < 0) ;
				} while (timercmp(&next1, &now, >));
			}
			IPPU.RenderThisFrame = TRUE;
			IPPU.SkippedFrames = 0;
		}
		else
		{
			if (IPPU.SkippedFrames < 10)	// mfs = 10
			{
				IPPU.SkippedFrames++;
				IPPU.RenderThisFrame = FALSE;
			}
			else
			{
				IPPU.RenderThisFrame = TRUE;
				IPPU.SkippedFrames = 0;
				next1 = now;
			}
		}
		next1.tv_usec += Settings.FrameTime;
		if (next1.tv_usec >= 1000000)
		{
			next1.tv_sec += next1.tv_usec / 1000000;
			next1.tv_usec %= 1000000;
		}
    }
    else
    {
		if (++IPPU.FrameSkip >= (Settings.TurboMode ? Settings.TurboSkipFrames
			: Settings.SkipFrames))
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = TRUE;
		}
		else
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = FALSE;
		}
    }
}

void S9xProcessEvents (bool8_32)
{
//	qApp->processEvents();
}

#define FIXED_POINT 0x10000
#define FIXED_POINT_SHIFT 16
#define FIXED_POINT_REMAINDER 0xffff

static volatile bool8 block_signal = FALSE;
static volatile bool8 pending_signal = FALSE;

void S9xGenerateSound ()
{
    int bytes_so_far = (so.samples_mixed_so_far << 1);
    if (bytes_so_far >= so.buffer_size)
		return;

    block_signal = TRUE;

    so.err_counter += so.err_rate;
    if (so.err_counter >= FIXED_POINT) {
        int sample_count = so.err_counter >> FIXED_POINT_SHIFT;
		int byte_offset;
		int byte_count;

        so.err_counter &= FIXED_POINT_REMAINDER;
		if (so.stereo)
		    sample_count <<= 1;
		byte_offset = bytes_so_far + so.play_position;
	    
	do {
	    int sc = sample_count;
	    byte_count = sample_count;
//	    if (so.sixteen_bit)
		byte_count <<= 1;
	    
	    if ((byte_offset & SOUND_BUFFER_SIZE_MASK) + byte_count > SOUND_BUFFER_SIZE) {
			sc = SOUND_BUFFER_SIZE - (byte_offset & SOUND_BUFFER_SIZE_MASK);
			byte_count = sc;
	//		if (so.sixteen_bit)
			    sc >>= 1;
	    }
	    if (bytes_so_far + byte_count > so.buffer_size)	{
			byte_count = so.buffer_size - bytes_so_far;
			if (byte_count == 0)
				break;
			sc = byte_count;
//		if (so.sixteen_bit)
			sc >>= 1;
		}
	    S9xMixSamplesO (EzxAudioBuffer, sc, byte_offset & SOUND_BUFFER_SIZE_MASK);
	    so.samples_mixed_so_far += sc;
	    sample_count -= sc;
	    bytes_so_far = (so.samples_mixed_so_far << 1);
	    byte_offset += byte_count;
	} while (sample_count > 0);
    }
    block_signal = FALSE;
    if (pending_signal) {
		S9xProcessSound ();
		pending_signal = FALSE;
    }
}

void S9xProcessSound()
{
	audio_buf_info info;
    if ( (ioctl (so.sound_fd, SNDCTL_DSP_GETOSPACE, &info) == -1 || info.bytes < so.buffer_size))
		return;
    
    {
		int sample_count = so.buffer_size;
		int byte_offset;
		
		sample_count >>= 1;
	
		if (block_signal) {
			pending_signal = TRUE;
			return;
    	}
    
		if (so.samples_mixed_so_far < sample_count)
		{
			byte_offset = so.play_position + (so.samples_mixed_so_far << 1);
			
			if (Settings.SoundSync == 2)
			{
			//	memset (EzxAudioBuffer + (byte_offset & SOUND_BUFFER_SIZE_MASK), 0,
			//		sample_count - so.samples_mixed_so_far);
			}
			else
			{
				//fprintf(stderr, "%s\n", "S9xMixSamplesO P");
				S9xMixSamplesO (EzxAudioBuffer, sample_count - so.samples_mixed_so_far,
					byte_offset & SOUND_BUFFER_SIZE_MASK);
			}
			so.samples_mixed_so_far = 0;
		}
		else
			so.samples_mixed_so_far -= sample_count;
		
		//if (!so.mute_sound)
		int I = 0;
		int J = so.buffer_size;
		
		byte_offset = so.play_position;
		so.play_position = (so.play_position + so.buffer_size) & SOUND_BUFFER_SIZE_MASK;

		do
		{
			//if ( snes_mute ) {
			//	// sleep(1);
			//	EZX_Delay(1);
			//	continue;
			//}
			if ( GameMute || GamePauseing || !GameLooping)
				return;

			if (byte_offset + J > SOUND_BUFFER_SIZE)
			{
				I = ezx_play_dsp (so.sound_fd, (char *) EzxAudioBuffer + byte_offset, SOUND_BUFFER_SIZE - byte_offset);
				if (I > 0) {
					J -= I;
					byte_offset = (byte_offset + I) & SOUND_BUFFER_SIZE_MASK;
				}
			}
			else
			{
				I = ezx_play_dsp (so.sound_fd, (char *) EzxAudioBuffer + byte_offset, J);
				if (I > 0) {
					J -= I;
					byte_offset = (byte_offset + I) & SOUND_BUFFER_SIZE_MASK;
				}
			}
		} while ((I < 0 && errno == EINTR) || J > 0);
		
	}
	
}

bool8 S9xOpenSnapshotFile (const char *fname, bool8 read_only, STREAM *file)
{
	*file = OPEN_STREAM(fname, read_only ? "rb" : "wb");
	return ( *file ) ? TRUE : FALSE ;
}

void S9xCloseSnapshotFile (STREAM file)
{
    CLOSE_STREAM (file);
}

static void SoundTrigger ()
{
    if (Settings.APUEnabled && !so.mute_sound)
		S9xProcessSound();
}

void StopTimer ()
{
    struct itimerval timeout;

    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_usec = 0;
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_usec = 0;
    if (setitimer (ITIMER_REAL, &timeout, NULL) < 0)
		; //perror ("setitimer");
	TimerStarted = false;
}

void InitTimer ()
{
	block_signal = FALSE;
	pending_signal = FALSE;
	
	if ( TimerStarted )
		return;
	
    struct itimerval timeout;
    struct sigaction sa;
    
    sa.sa_handler = (SIG_PF) SoundTrigger;

#if defined (SA_RESTART)
    sa.sa_flags = SA_RESTART;
#else
    sa.sa_flags = 0;
#endif
    
    sigemptyset (&sa.sa_mask);
    sigaction (SIGALRM, &sa, NULL);
    
    timeout.it_interval.tv_sec = 0;
    timeout.it_interval.tv_usec = 10000;
    timeout.it_value.tv_sec = 0;
    timeout.it_value.tv_usec = 10000;
    if (setitimer (ITIMER_REAL, &timeout, NULL) < 0)
		;//perror ("setitimer");

	TimerStarted = true;
}

