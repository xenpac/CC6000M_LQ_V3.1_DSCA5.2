#ifndef __ICONV_EXT_H__
#define __ICONV_EXT_H__


/* Get size_t declaration.
   Get wchar_t declaration if it exists. */
#include <stddef.h>

/*
 * supported encodings:
    "ascii",
    "utf8",
    "ucs2"
    "euc_cn",
    "cp1256",
    ...
 */
#include "ascii.h"
#include "utf8.h"
#include "ucs2.h"
#include "ucs2le.h"
#include "euc_cn.h"
#include "cp1256.h"
#include "iso8859_9.h"

#undef iconv_t
#define iconv_t libiconv_t
typedef void* iconv_t;



typedef unsigned int ucs4_t;


typedef unsigned int state_t;


typedef struct conv_struct * conv_t;


/*
 * Data type for conversion multibyte -> unicode
 */
struct mbtowc_funcs {
  int (*xxx_mbtowc) (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n);
  int (*xxx_flushwc) (conv_t conv, ucs4_t *pwc);
};

/*
 * Data type for conversion unicode -> multibyte
 */
struct wctomb_funcs {
  int (*xxx_wctomb) (conv_t conv, unsigned char *r, ucs4_t wc, int n);
  int (*xxx_reset) (conv_t conv, unsigned char *r, int n);
};


/*
 * Data type for general conversion loop.
 */
struct loop_funcs {
  size_t (*loop_convert) (iconv_t icd,
                          const char* * inbuf, size_t *inbytesleft,
                          char* * outbuf, size_t *outbytesleft);
  size_t (*loop_reset) (iconv_t icd,
                        char* * outbuf, size_t *outbytesleft);
};


/* Hook performed after every successful conversion of a Unicode character. */
typedef void (*iconv_unicode_char_hook) (unsigned int uc, void* data);
/* Hook performed after every successful conversion of a wide character. */
typedef void (*iconv_wide_char_hook) (wchar_t wc, void* data);
/* Set of hooks. */
struct iconv_hooks {
  iconv_unicode_char_hook uc_hook;
  iconv_wide_char_hook wc_hook;
  void* data;
};

/* Fallback function.  Invoked when a small number of bytes could not be
   converted to a Unicode character.  This function should process all
   bytes from inbuf and may produce replacement Unicode characters by calling
   the write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_mb_to_uc_fallback)
             (const char* inbuf, size_t inbufsize,
              void (*write_replacement) (const unsigned int *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);
/* Fallback function.  Invoked when a Unicode character could not be converted
   to the target encoding.  This function should process the character and
   may produce replacement bytes (in the target encoding) by calling the
   write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_uc_to_mb_fallback)
             (unsigned int code,
              void (*write_replacement) (const char *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);


/* Fallback function.  Invoked when a number of bytes could not be converted to
   a wide character.  This function should process all bytes from inbuf and may
   produce replacement wide characters by calling the write_replacement
   callback repeatedly.  */
typedef void (*iconv_wchar_mb_to_wc_fallback)
             (const char* inbuf, size_t inbufsize,
              void (*write_replacement) (const wchar_t *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);
/* Fallback function.  Invoked when a wide character could not be converted to
   the target encoding.  This function should process the character and may
   produce replacement bytes (in the target encoding) by calling the
   write_replacement callback repeatedly.  */
typedef void (*iconv_wchar_wc_to_mb_fallback)
             (wchar_t code,
              void (*write_replacement) (const char *buf, size_t buflen,
                                         void* callback_arg),
              void* callback_arg,
              void* data);

/* Set of fallbacks. */
struct iconv_fallbacks {
  iconv_unicode_mb_to_uc_fallback mb_to_uc_fallback;
  iconv_unicode_uc_to_mb_fallback uc_to_mb_fallback;
  iconv_wchar_mb_to_wc_fallback mb_to_wc_fallback;
  iconv_wchar_wc_to_mb_fallback wc_to_mb_fallback;
  void* data;
};



/* Return code if invalid input after a shift sequence of n bytes was read.
   (xxx_mbtowc) */
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)       (-2-2*(n))
/* Retrieve the n from the encoded RET_... value. */
#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2





/*
 * Contents of a conversion descriptor.
 */
struct conv_struct {
  struct loop_funcs lfuncs;
  /* Input (conversion multibyte -> unicode) */
  int iindex;
  struct mbtowc_funcs ifuncs;
  state_t istate;
  /* Output (conversion unicode -> multibyte) */
  int oindex;
  struct wctomb_funcs ofuncs;
  int oflags;
  state_t ostate;
  int transliterate;
  int discard_ilseq;
  struct iconv_fallbacks fallbacks;
  struct iconv_hooks hooks;
};

/*
 * def of encoding
 */
struct encoding {
  struct mbtowc_funcs ifuncs; /* conversion multibyte -> unicode */
  struct wctomb_funcs ofuncs; /* conversion unicode -> multibyte */
  int oflags;                 /* flags for unicode -> multibyte conversion */
};


typedef struct {
  unsigned short indx; /* index into big table */
  unsigned short used; /* bitmask of used entries */
} Summary16;

#ifdef __cplusplus
extern "C" {
#endif
/*
 *
 */
int iconv_init(unsigned char  n_encodings);
/*
 *
 */
int iconv_deinit();

/*
 *
 */
int iconv_add_encoding(const char* enc_name, struct encoding* p_enc);

/*
 *
 */
int iconv_del_encoding(const char* enc_name);

/*
 *
 */

iconv_t iconv_open (const char* tocode, const char* fromcode);

/*
 *
 */
size_t iconv (iconv_t cd,  char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);

/*
 *
 */
int iconv_close (iconv_t cd);

#ifdef __cplusplus
}
#endif
#define ICONV_ADD_ENC(xxx)   iconv_add_##xxx(#xxx)


#endif

