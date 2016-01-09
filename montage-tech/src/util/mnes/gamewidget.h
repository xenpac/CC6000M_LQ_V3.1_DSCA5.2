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
 * $Id: gamewidget.h,v 0.10 2006/06/07 $
 */

#ifndef _GAME_WIDGET_
#define _GAME_WIDGET_

class GameWidget
{
  public:
  	GameWidget(void);
  	virtual ~GameWidget(void);
};

extern GameWidget *wgGame;
extern unsigned short *VideoBuffer;
extern bool GameLooping;
extern int  GameFrameSkip;

/* SDL Timer */

int EZX_SoftStretch(void *src, int sx, int sy, int sw, int sh, int sp, void *dst, int dw, int dh);

#endif	// _GAME_WIDGET_
