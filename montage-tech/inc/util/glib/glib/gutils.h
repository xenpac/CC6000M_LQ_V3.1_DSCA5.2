/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
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

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_UTILS_H__
#define __G_UTILS_H__

#if !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif

#include <glib/gtypes.h>
#include <stdarg.h>

G_BEGIN_DECLS

/* Define G_VA_COPY() to do the right thing for copying va_list variables.
 * glibconfig.h may have already defined G_VA_COPY as va_copy or __va_copy.
 */
#if !defined (G_VA_COPY)
#  if defined (__GNUC__) && defined (__PPC__) && (defined (_CALL_SYSV) || defined (_WIN32))
#    define G_VA_COPY(ap1, ap2)	  (*(ap1) = *(ap2))
#  elif defined (G_VA_COPY_AS_ARRAY)
#    define G_VA_COPY(ap1, ap2)	  memmove ((ap1), (ap2), sizeof (va_list))
#  else /* va_list is a pointer */
#    define G_VA_COPY(ap1, ap2)	  ((ap1) = (ap2))
#  endif /* va_list is a pointer */
#endif /* !G_VA_COPY */

/* inlining hassle. for compilers that don't allow the 'inline' keyword,
 * mostly because of strict ANSI C compliance or dumbness, we try to fall
 * back to either '__inline__' or '__inline'.
 * G_CAN_INLINE is defined in glibconfig.h if the compiler seems to be 
 * actually *capable* to do function inlining, in which case inline 
 * function bodies do make sense. we also define G_INLINE_FUNC to properly 
 * export the function prototypes if no inlining can be performed.
 * inline function bodies have to be special cased with G_CAN_INLINE and a
 * .c file specific macro to allow one compiled instance with extern linkage
 * of the functions by defining G_IMPLEMENT_INLINES and the .c file macro.
 */
#if defined (G_HAVE_INLINE) && defined (__GNUC__) && defined (__STRICT_ANSI__)
#  undef inline
#  define inline __inline__
#elif !defined (G_HAVE_INLINE)
#  undef inline
#  if defined (G_HAVE___INLINE__)
#    define inline __inline__
#  elif defined (G_HAVE___INLINE)
#    define inline __inline
#  else /* !inline && !__inline__ && !__inline */
#    define inline  /* don't inline, then */
#  endif
#endif
#ifdef G_IMPLEMENT_INLINES
#  define G_INLINE_FUNC _GLIB_EXTERN
#  undef  G_CAN_INLINE
#elif defined (__GNUC__) 
#  define G_INLINE_FUNC static __inline __attribute__ ((unused))
#elif defined (G_CAN_INLINE) 
#  define G_INLINE_FUNC static inline
#else /* can't inline */
#  define G_INLINE_FUNC _GLIB_EXTERN
#endif /* !G_INLINE_FUNC */

/* Character sets */
#define G_CSET_A_2_Z	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define G_CSET_a_2_z	"abcdefghijklmnopqrstuvwxyz"
#define G_CSET_DIGITS	"0123456789"
#define G_CSET_LATINC	"\300\301\302\303\304\305\306"\
			"\307\310\311\312\313\314\315\316\317\320"\
			"\321\322\323\324\325\326"\
			"\330\331\332\333\334\335\336"
#define G_CSET_LATINS	"\337\340\341\342\343\344\345\346"\
			"\347\350\351\352\353\354\355\356\357\360"\
			"\361\362\363\364\365\366"\
			"\370\371\372\373\374\375\376\377"

#define g_snprintf  snprintf

GLIB_AVAILABLE_IN_ALL
void                  g_nullify_pointer    (gpointer    *nullify_location);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_prgname          (void);
GLIB_AVAILABLE_IN_ALL
void                  g_set_prgname          (const gchar *prgname);
#ifdef G_MIN_MEM_MODE
GLIB_AVAILABLE_IN_ALL
void                  g_free_prgname (void);
#endif

/* inline function implementations
 */
#if defined (G_CAN_INLINE) || defined (__G_UTILS_C__)
G_INLINE_FUNC gint
g_bit_nth_lsf (gulong mask,
	       gint   nth_bit)
{
  if (G_UNLIKELY (nth_bit < -1))
    nth_bit = -1;
  while (nth_bit < ((GLIB_SIZEOF_LONG * 8) - 1))
    {
      nth_bit++;
      if (mask & (1UL << nth_bit))
	return nth_bit;
    }
  return -1;
}
G_INLINE_FUNC gint
g_bit_nth_msf (gulong mask,
	       gint   nth_bit)
{
  if (nth_bit < 0 || G_UNLIKELY (nth_bit > GLIB_SIZEOF_LONG * 8))
    nth_bit = GLIB_SIZEOF_LONG * 8;
  while (nth_bit > 0)
    {
      nth_bit--;
      if (mask & (1UL << nth_bit))
	return nth_bit;
    }
  return -1;
}
G_INLINE_FUNC guint
g_bit_storage (gulong number)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__OPTIMIZE__)
  return G_LIKELY (number) ?
	   ((GLIB_SIZEOF_LONG * 8U - 1) ^ (guint) __builtin_clzl(number)) + 1 : 1;
#else
  register guint n_bits = 0;
  
  do
    {
      n_bits++;
      number >>= 1;
    }
  while (number);
  return n_bits;
#endif
}
#endif  /* G_CAN_INLINE || __G_UTILS_C__ */


G_END_DECLS

#endif /* __G_UTILS_H__ */
