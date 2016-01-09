/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997, 2002  Peter Mattis, Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __G_PRINTF_H__
#define __G_PRINTF_H__

#include <glib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef G_OS_UCOS
#include <sys_types.h>
#include <mtos_printk.h>
#endif

G_BEGIN_DECLS

#ifdef G_OS_UCOS
#define PRINTF OS_PRINTF
#else
#define PRINTF printf
#endif

#define g_printf    PRINTF
#define g_fprintf   fprintf
#define g_sprintf   sprintf

#define g_vprintf   vprintf
#define g_vfprintf  vfprintf
#define g_vsprintf  vsprintf
#define g_vsnprintf vsnprintf

GLIB_AVAILABLE_IN_ALL
gint                  g_vasprintf (gchar      **string,
				   gchar const *format,
				   va_list      args) G_GNUC_PRINTF(2, 0);

G_END_DECLS

#endif /* __G_PRINTF_H__ */
