/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
/* Actually, this version of config.h is manually edited from the above */

/* Have usable Cairo library and font backend */
/* #undef HAVE_CAIRO 1 */

/* Whether Cairo can use FreeType for fonts */
/*
#ifndef PANGO_VISUALC_NO_FC
#define HAVE_CAIRO_FREETYPE 1
#endif
*/

/* Whether Cairo has PDF support */
/* #undef HAVE_CAIRO_PDF 1 */

/* Whether Cairo has PNG support */
/* #undef HAVE_CAIRO_PNG 1 */

/* Whether Cairo has PS support */
/* #undef HAVE_CAIRO_PS 1 */

/* Whether Cairo can use Quartz for fonts */
/* #undef HAVE_CAIRO_QUARTZ */

/* Whether Cairo can use the Win32 GDI for fonts */
/* #undef HAVE_CAIRO_WIN32 1 */

/* Whether Cairo has Xlib support */
/* #undef HAVE_CAIRO_XLIB */

/* Whether CoreText is available on the system */
/* #undef HAVE_CORE_TEXT */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_DIRENT_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the `flockfile' function. */
/* #undef HAVE_FLOCKFILE */

/* Have FreeType 2 library */
#ifndef PANGO_VISUALC_NO_FC
#define HAVE_FREETYPE 1
#endif

/* Define to 1 if you have the `getpagesize' function. */
/* #undef HAVE_GETPAGESIZE */

/* Define to 1 if you have the <inttypes.h> header file. */
/*#undef HAVE_INTTYPES_H */

/* Define to 1 if you have the <memory.h> header file. */
/* #undef HAVE_MEMORY_H */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <stdint.h> header file. */
/*
#if (!defined (_MSC_VER) || (_MSC_VER >= 1600))
#define HAVE_STDINT_H 1
#endif
*/

/* Define to 1 if you have the <stdlib.h> header file. */
/* #undef HAVE_STDLIB_H 1 */

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
/* #undef HAVE_STRING_H 1 */

/* Define to 1 if you have the `strtok_r' function. */
/* #undef HAVE_STRTOK_R */

/* Define to 1 if you have the `sysconf' function. */
/* #undef HAVE_SYSCONF */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* #undef HAVE_SYS_MMAN_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
/* #undef HAVE_SYS_STAT_H 1 */

/* Define to 1 if you have the <sys/types.h> header file. */
/* #undef HAVE_SYS_TYPES_H 1 */

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Have Xft library */
/* #undef HAVE_XFT */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
/* #undef LT_OBJDIR ".libs/" */

/* Module interface version */
/* #undef MODULE_VERSION "1.8.0" */

/* Name of package */
/* #undef PACKAGE "pango" */

/* Define to the address where bug reports for this package should be sent. */
/* #undef PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=pango" */

/* Define to the full name of this package. */
#define PACKAGE_NAME "pango"

/* Define to the full name and version of this package. */
/* #undef PACKAGE_STRING "pango 1.36.3" */

/* Define to the one symbol short name of this package. */
/* #undef PACKAGE_TARNAME "pango" */

/* Define to the home page for this package. */
/* #undef PACKAGE_URL "" */

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.36.3"

/* PANGO binary age */
#define PANGO_BINARY_AGE 3603

/* PANGO interface age */
/* #undef PANGO_INTERFACE_AGE 3 */

/* PANGO major version */
#define PANGO_VERSION_MAJOR 1

/* PANGO micro version */
#define PANGO_VERSION_MICRO 3

/* PANGO minor version */
#define PANGO_VERSION_MINOR 36

/* Define to 1 if you have the ANSI C header files. */
/* #undef STDC_HEADERS 1 */

/* Whether to load modules via .la files rather than directly */
/* #undef USE_LA_MODULES */

/* Define platform
    PANGO_OS_WIN32
    PANGO_OS_UCOS
    PANGO_OS_UNIX)
*/
#if defined (_MSC_VER)
#define PANGO_OS_WIN32
#else
#define PANGO_OS_UCOS
#endif

/* pango support markup feature */
/* #undef PANGO_MARKUP_ENABLE */

/* pango support file system */
/* #undef PANGO_FILE_ENABLE */

/* pango support locale feature */
/* #undef PANGO_LOCALE_ENABLE */

/* pango support gmodule */
/* #undef PANGO_GMODULE_ENABLE */

/* Version number of package */
/* #undef VERSION "1.36.3" */

/* freetype use memory face */
#define PANGO_USE_MEM_FACE

/* pango support release extra memory */
/* #undef PANGO_MIN_MEM_MODE */