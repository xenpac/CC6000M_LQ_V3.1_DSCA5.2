/*
 * ASCII
 */
#include "iconv_ext.h"
#include "ascii.h"


static int
ascii_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;
  if (c < 0x80) {
    *pwc = (ucs4_t) c;
    return 1;
  }
  return RET_ILSEQ;
}

static int
ascii_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x0080) {
    *r = wc;
    return 1;
  }
  return RET_ILUNI;
}


int iconv_add_ascii(const char* enc_name)
{
  struct encoding enc;

  enc.ifuncs.xxx_mbtowc = ascii_mbtowc;
  enc.ifuncs.xxx_flushwc = NULL;
  enc.ofuncs.xxx_wctomb = ascii_wctomb;
  enc.ofuncs.xxx_reset = NULL;
  enc.oflags = 0;
  
  return iconv_add_encoding(enc_name, &enc);
}

