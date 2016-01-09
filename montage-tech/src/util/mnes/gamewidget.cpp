/*
 * EZX Emulator for MOTO EZX Modile Phone
 * Copyright (C) 2006 OopsWare. CHINA.
 *
 * This program is free software; you can redistribute it and/or modify
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
 * $Id: gamewidget.cpp,v 0.10 2006/06/07 $
 */
#include "types.h"
#include <stdio.h>
#include "string.h"

#include "stdlib.h"
#include "sys_types.h"
#include "nes_emu.h"

#include "lib_rect.h"

#include "gamewidget.h"

GameWidget *wgGame	= 0;

bool GameLooping	= false;
int  GameFrameSkip	= -1;

unsigned short *VideoBuffer = NULL;

GameWidget::GameWidget(void)
{
#if 0
	VideoBuffer = (unsigned short *)NES_MALLOC_DMA(NES_VBUF_SIZE);
#else
	VideoBuffer = (unsigned short *)NES_MALLOC(NES_VBUF_SIZE);
#endif

	NES_ASSERT(VideoBuffer != NULL);

	NES_MEMSET(VideoBuffer, 0, NES_VBUF_SIZE);
}

GameWidget::~GameWidget(void)
{
	if(VideoBuffer)
	{
#if 0	
		NES_FREE_DMA(VideoBuffer);
#else
		NES_FREE(VideoBuffer);
#endif

		VideoBuffer = NULL;
	}
}

static void copy_row2(unsigned short *src, int src_w, unsigned short *dst, int dst_w)
{
	int i;
	int pos, inc;
	unsigned short pixel = 0;

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for ( i=dst_w; i>0; --i ) 
	{
		while ( pos >= 0x10000L ) 
		{
			pixel = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel;
		pos += inc;
	}
}

int EZX_SoftStretch(void *src, int sx, int sy, int sw, int sh, int sp, void *dst, int dw, int dh)
{
	int pos, inc;
	int dst_width;
	int dst_maxrow;
	int src_row, dst_row;
	unsigned char *srcp = NULL;
	unsigned char *dstp;

	/* Set up the data... */
	pos = 0x10000;
	inc = (sh << 16) / dh;
	src_row = sy * sp; //srcrect->y;
	dst_row = 0; //dstrect->y;
	dst_width = dw * 2;

	/* Perform the stretch blit */
	for(dst_maxrow = dst_row + dh; dst_row < dst_maxrow; ++dst_row ) 
	{
		dstp = (unsigned char *)dst + (dst_row * 640);	// + (dtrect->x*bpp);
		while ( pos >= 0x10000L ) 
		{
			srcp = (unsigned char *)src + src_row + sx * 2;
			src_row += sp;
			pos -= 0x10000L;
		}

		copy_row2((unsigned short *)srcp, sw, (unsigned short *)dstp, dw);
		pos += inc;
	}

	/* We need to unlock the surfaces if they're locked */
	return(0);
}

