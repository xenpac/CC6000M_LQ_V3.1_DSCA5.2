/*
 * UCS-2LE = UCS-2 little endian
 */

#include "iconv_ext.h"
#include "ucs2le.h"

static int
ucs2le_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  if (n >= 2) {
    if (s[1] >= 0xd8 && s[1] < 0xe0) {
      return RET_ILSEQ;
    } else {
      *pwc = s[0] + (s[1] << 8);
      return 2;
    }
  }
  return RET_TOOFEW(0);
}

static int
ucs2le_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x10000 && !(wc >= 0xd800 && wc < 0xe000)) {
    if (n >= 2) {
      r[0] = (unsigned char) wc;
      r[1] = (unsigned char) (wc >> 8);
      return 2;
    } else
      return RET_TOOSMALL;
  }
  return RET_ILUNI;
}


int iconv_add_ucs2le(const char* enc_name)
{
  struct encoding enc;

  enc.ifuncs.xxx_mbtowc = ucs2le_mbtowc;
  enc.ifuncs.xxx_flushwc = NULL;
  enc.ofuncs.xxx_wctomb = ucs2le_wctomb;
  enc.ofuncs.xxx_reset = NULL;
  enc.oflags = 0;

  return iconv_add_encoding(enc_name, &enc);
}
