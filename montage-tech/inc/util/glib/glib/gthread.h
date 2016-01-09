/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
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
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_THREAD_H__
#define __G_THREAD_H__

#if !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <glib.h> can be included directly."
#endif

#include <glib/gatomic.h>

G_BEGIN_DECLS
#ifdef G_OS_WIN32
typedef enum
{
  G_THREAD_ERROR_AGAIN /* Resource temporarily unavailable */
} GThreadError;

typedef gpointer (*GThreadFunc) (gpointer data);

typedef struct _GThread         GThread;
#endif

typedef union  _GMutex          GMutex;
typedef struct _GRecMutex       GRecMutex;
#ifdef G_OS_WIN32
typedef struct _GRWLock         GRWLock;
typedef struct _GCond           GCond;
typedef struct _GPrivate        GPrivate;
typedef struct _GOnce           GOnce;

typedef enum
{
  G_THREAD_PRIORITY_LOW,
  G_THREAD_PRIORITY_NORMAL,
  G_THREAD_PRIORITY_HIGH,
  G_THREAD_PRIORITY_URGENT
} GThreadPriority;

struct  _GThread
{
  /*< private >*/
  GThreadFunc func;
  gpointer data;
  gboolean joinable;
  GThreadPriority priority;
};
#endif

union _GMutex
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

#ifdef G_OS_WIN32
struct _GRWLock
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

struct _GCond
{
  /*< private >*/
  gpointer p;
  guint i[2];
};
#endif

struct _GRecMutex
{
  /*< private >*/
  gpointer p;
  guint i[2];
};

#ifdef G_OS_WIN32
#define G_PRIVATE_INIT(notify) { NULL, (notify), { NULL, NULL } }
struct _GPrivate
{
  /*< private >*/
  gpointer       p;
  GDestroyNotify notify;
  gpointer future[2];
};

typedef enum
{
  G_ONCE_STATUS_NOTCALLED,
  G_ONCE_STATUS_PROGRESS,
  G_ONCE_STATUS_READY
} GOnceStatus;

#define G_ONCE_INIT { G_ONCE_STATUS_NOTCALLED, NULL }
struct _GOnce
{
  volatile GOnceStatus status;
  volatile gpointer retval;
};
#endif

#define G_LOCK_NAME(name)             g__ ## name ## _lock
#define G_LOCK_DEFINE_STATIC(name)    static G_LOCK_DEFINE (name)
#define G_LOCK_DEFINE(name)           GMutex G_LOCK_NAME (name)
#define G_LOCK_EXTERN(name)           extern GMutex G_LOCK_NAME (name)

#ifdef G_DEBUG_LOCKS
#  define G_LOCK(name)                G_STMT_START{             \
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                   \
             "file %s: line %d (%s): locking: %s ",             \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name);                                            \
      g_mutex_lock (&G_LOCK_NAME (name));                       \
   }G_STMT_END
#  define G_UNLOCK(name)              G_STMT_START{             \
      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                   \
             "file %s: line %d (%s): unlocking: %s ",           \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name);                                            \
     g_mutex_unlock (&G_LOCK_NAME (name));                      \
   }G_STMT_END
#  define G_TRYLOCK(name)                                       \
      (g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,                  \
             "file %s: line %d (%s): try locking: %s ",         \
             __FILE__,        __LINE__, G_STRFUNC,              \
             #name), g_mutex_trylock (&G_LOCK_NAME (name)))
#else  /* !G_DEBUG_LOCKS */
#  define G_LOCK(name) g_mutex_lock       (&G_LOCK_NAME (name))
#  define G_UNLOCK(name) g_mutex_unlock   (&G_LOCK_NAME (name))
#  define G_TRYLOCK(name) g_mutex_trylock (&G_LOCK_NAME (name))
#endif /* !G_DEBUG_LOCKS */

#ifdef G_OS_WIN32
GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_ref                    (GThread        *thread);
GLIB_AVAILABLE_IN_2_32
void            g_thread_unref                  (GThread        *thread);
GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_new                    (const gchar    *name,
                                                 GThreadFunc     func,
                                                 gpointer        data);
GLIB_AVAILABLE_IN_2_32
GThread *       g_thread_try_new                (const gchar    *name,
                                                 GThreadFunc     func,
                                                 gpointer        data,
                                                 const gchar   **error);
GLIB_AVAILABLE_IN_ALL
GThread *       g_thread_self                   (void);
GLIB_AVAILABLE_IN_ALL
void            g_thread_exit                   (gpointer        retval);
GLIB_AVAILABLE_IN_ALL
gpointer        g_thread_join                   (GThread        *thread);
GLIB_AVAILABLE_IN_ALL
void            g_thread_yield                  (void);
#endif

GLIB_AVAILABLE_IN_2_32
void            g_mutex_init                    (GMutex         *mutex);
GLIB_AVAILABLE_IN_2_32
void            g_mutex_clear                   (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_mutex_lock                    (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
gboolean        g_mutex_trylock                 (GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_mutex_unlock                  (GMutex         *mutex);

#ifdef G_OS_WIN32
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_init                  (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_clear                 (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_writer_lock           (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rw_lock_writer_trylock        (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_writer_unlock         (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_reader_lock           (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rw_lock_reader_trylock        (GRWLock        *rw_lock);
GLIB_AVAILABLE_IN_2_32
void            g_rw_lock_reader_unlock         (GRWLock        *rw_lock);
#endif

GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_init                (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_clear               (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_lock                (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
gboolean        g_rec_mutex_trylock             (GRecMutex      *rec_mutex);
GLIB_AVAILABLE_IN_2_32
void            g_rec_mutex_unlock              (GRecMutex      *rec_mutex);

#ifdef G_OS_WIN32
GLIB_AVAILABLE_IN_2_32
void            g_cond_init                     (GCond          *cond);
GLIB_AVAILABLE_IN_2_32
void            g_cond_clear                    (GCond          *cond);
GLIB_AVAILABLE_IN_ALL
void            g_cond_wait                     (GCond          *cond,
                                                 GMutex         *mutex);
GLIB_AVAILABLE_IN_ALL
void            g_cond_signal                   (GCond          *cond);
GLIB_AVAILABLE_IN_ALL
void            g_cond_broadcast                (GCond          *cond);
GLIB_AVAILABLE_IN_2_32
gboolean        g_cond_wait_until               (GCond          *cond,
                                                 GMutex         *mutex,
                                                 gint64          end_time);

GLIB_AVAILABLE_IN_ALL
gpointer        g_private_get                   (GPrivate       *key);
GLIB_AVAILABLE_IN_ALL
void            g_private_set                   (GPrivate       *key,
                                                 gpointer        value);
GLIB_AVAILABLE_IN_2_32
void            g_private_replace               (GPrivate       *key,
                                                 gpointer        value);

GLIB_AVAILABLE_IN_ALL
gpointer        g_once_impl                     (GOnce          *once,
                                                 GThreadFunc     func,
                                                 gpointer        arg);
#endif
GLIB_AVAILABLE_IN_ALL
gboolean        g_once_init_enter               (volatile void  *location);
GLIB_AVAILABLE_IN_ALL
void            g_once_init_leave               (volatile void  *location,
                                                 gsize           result);

#ifdef G_OS_WIN32
#ifdef G_ATOMIC_OP_MEMORY_BARRIER_NEEDED
# define g_once(once, func, arg) g_once_impl ((once), (func), (arg))
#else /* !G_ATOMIC_OP_MEMORY_BARRIER_NEEDED*/
# define g_once(once, func, arg) \
  (((once)->status == G_ONCE_STATUS_READY) ? \
   (once)->retval : \
   g_once_impl ((once), (func), (arg)))
#endif /* G_ATOMIC_OP_MEMORY_BARRIER_NEEDED */
#endif

#ifdef __GNUC__
# define g_once_init_enter(location) \
  (G_GNUC_EXTENSION ({                                               \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer));       \
    (void) (0 ? (gpointer) *(location) : 0);                         \
    (!g_atomic_pointer_get (location) &&                             \
     g_once_init_enter (location));                                  \
  }))
# define g_once_init_leave(location, result) \
  (G_GNUC_EXTENSION ({                                               \
    G_STATIC_ASSERT (sizeof *(location) == sizeof (gpointer));       \
    (void) (0 ? *(location) = (result) : 0);                         \
    g_once_init_leave ((location), (gsize) (result));                \
  }))
#else
# define g_once_init_enter(location) \
  (g_once_init_enter((location)))
# define g_once_init_leave(location, result) \
  (g_once_init_leave((location), (gsize) (result)))
#endif

#ifdef G_OS_WIN32
GLIB_AVAILABLE_IN_2_36
guint          g_get_num_processors (void);
#endif

G_END_DECLS

#endif /* __G_THREAD_H__ */