/*
 * Copyright © 2011 Canonical Limited
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Ryan Lortie <desrt@desrt.ca>
 */

#ifndef __GLIB_INIT_H__
#define __GLIB_INIT_H__

#include "gmessages.h"

void glib_init();

#if defined(G_OS_WIN32)
#include <windows.h>

void g_thread_win32_thread_detach (void);
void g_thread_win32_init (void);
#elif defined (G_OS_UCOS)
#elif defined (G_OS_UNIX)
#else
#endif

#endif /* __GLIB_INIT_H__ */
