#include "iconv_ext.h"

#include <stdlib.h>
#include <string.h>

/* Get errno declaration and values. */
#include <errno.h>
/* Some systems, like SunOS 4, don't have EILSEQ. Some systems, like BSD/OS,
   have EILSEQ in a different header.  On these systems, define EILSEQ
   ourselves. */
#ifndef EILSEQ
#define EILSEQ
#endif


static char** all_encoding_names = NULL;
static struct encoding* all_encodings = NULL;
static unsigned char sg_encodings_total = 0;

enum
{
  ei_ascii,
  ei_utf8,
  ei_ucs2,
  ei_euc_cn,
  ei_iso8859_9,
  /*new ei_xxx inserted here*/
  ei_encodings_count
};

/*
 * Conversion loops.
 */
//#include "loops.h"

struct uc_to_mb_fallback_locals {
  unsigned char* l_outbuf;
  size_t l_outbytesleft;
  int l_errno;
};

static void uc_to_mb_write_replacement (const char *buf, size_t buflen,
                                        void* callback_arg)
{
  struct uc_to_mb_fallback_locals * plocals =
    (struct uc_to_mb_fallback_locals *) callback_arg;
  /* Do nothing if already encountered an error in a previous call. */
  if (plocals->l_errno == 0) {
    /* Attempt to copy the passed buffer to the output buffer. */
    if (plocals->l_outbytesleft < buflen)
      plocals->l_errno = E2BIG;
    else {
      memcpy(plocals->l_outbuf, buf, buflen);
      plocals->l_outbuf += buflen;
      plocals->l_outbytesleft -= buflen;
    }
  }
}

struct mb_to_uc_fallback_locals {
  conv_t l_cd;
  unsigned char* l_outbuf;
  size_t l_outbytesleft;
  int l_errno;
};

static void mb_to_uc_write_replacement (const unsigned int *buf, size_t buflen,
                                        void* callback_arg)
{
  struct mb_to_uc_fallback_locals * plocals =
    (struct mb_to_uc_fallback_locals *) callback_arg;
  /* Do nothing if already encountered an error in a previous call. */
  if (plocals->l_errno == 0) {
    /* Attempt to convert the passed buffer to the target encoding. */
    conv_t cd = plocals->l_cd;
    unsigned char* outptr = plocals->l_outbuf;
    size_t outleft = plocals->l_outbytesleft;
    for (; buflen > 0; buf++, buflen--) {
      ucs4_t wc = *buf;
      int outcount;
      if (outleft == 0) {
        plocals->l_errno = E2BIG;
        break;
      }
      outcount = cd->ofuncs.xxx_wctomb(cd,outptr,wc,outleft);
      if (outcount != RET_ILUNI)
        goto outcount_ok;
      /* Handle Unicode tag characters (range U+E0000..U+E007F). */
      if ((wc >> 7) == (0xe0000 >> 7))
        goto outcount_zero;
      /* Try transliteration. */
	  /*
      if (cd->transliterate) {
        outcount = unicode_transliterate(cd,wc,outptr,outleft);
        if (outcount != RET_ILUNI)
          goto outcount_ok;
      }
      */
      if (cd->discard_ilseq) {
        outcount = 0;
        goto outcount_ok;
      }
      #ifndef LIBICONV_PLUG
      else if (cd->fallbacks.uc_to_mb_fallback != NULL) {
        struct uc_to_mb_fallback_locals locals;
        locals.l_outbuf = outptr;
        locals.l_outbytesleft = outleft;
        locals.l_errno = 0;
        cd->fallbacks.uc_to_mb_fallback(wc,
                                        uc_to_mb_write_replacement,
                                        &locals,
                                        cd->fallbacks.data);
        if (locals.l_errno != 0) {
          plocals->l_errno = locals.l_errno;
          break;
        }
        outptr = locals.l_outbuf;
        outleft = locals.l_outbytesleft;
        outcount = 0;
        goto outcount_ok;
      }
      #endif
      outcount = cd->ofuncs.xxx_wctomb(cd,outptr,0xFFFD,outleft);
      if (outcount != RET_ILUNI)
        goto outcount_ok;
      plocals->l_errno = EILSEQ;
      break;
    outcount_ok:
      if (outcount < 0) {
        plocals->l_errno = E2BIG;
        break;
      }
      #ifndef LIBICONV_PLUG
      if (cd->hooks.uc_hook)
        (*cd->hooks.uc_hook)(wc, cd->hooks.data);
      #endif
      //if (!(outcount <= outleft)) abort();
      if (!(outcount <= outleft)) return;
      outptr += outcount; outleft -= outcount;
    outcount_zero: ;
    }
    plocals->l_outbuf = outptr;
    plocals->l_outbytesleft = outleft;
  }
}

static size_t unicode_loop_convert (iconv_t icd,
                                    const char* * inbuf, size_t *inbytesleft,
                                    char* * outbuf, size_t *outbytesleft)
{
  conv_t cd = (conv_t) icd;
  size_t result = 0;
  const unsigned char* inptr = (const unsigned char*) *inbuf;
  size_t inleft = *inbytesleft;
  unsigned char* outptr = (unsigned char*) *outbuf;
  size_t outleft = *outbytesleft;
  while (inleft > 0) {
    state_t last_istate = cd->istate;
    ucs4_t wc;
    int incount;
    int outcount;
    incount = cd->ifuncs.xxx_mbtowc(cd,&wc,inptr,inleft);
    if (incount < 0) {
      if ((unsigned int)(-1-incount) % 2 == (unsigned int)(-1-RET_ILSEQ) % 2) {
        /* Case 1: invalid input, possibly after a shift sequence */
        incount = DECODE_SHIFT_ILSEQ(incount);
        if (cd->discard_ilseq) {
          switch (cd->iindex) {
            case ei_ucs2:
              incount += 2; break;
            default:
              incount += 1; break;
          }
          goto outcount_zero;
        }
        #ifndef LIBICONV_PLUG
        else if (cd->fallbacks.mb_to_uc_fallback != NULL) {
          unsigned int incount2;
          struct mb_to_uc_fallback_locals locals;
          switch (cd->iindex) {
            case ei_ucs2:
              incount2 = 2; break;
            default:
              incount2 = 1; break;
          }
          locals.l_cd = cd;
          locals.l_outbuf = outptr;
          locals.l_outbytesleft = outleft;
          locals.l_errno = 0;
          cd->fallbacks.mb_to_uc_fallback((const char*)inptr+incount, incount2,
                                          mb_to_uc_write_replacement,
                                          &locals,
                                          cd->fallbacks.data);
          if (locals.l_errno != 0) {
            inptr += incount; inleft -= incount;
            //errno = locals.l_errno;
            result = -1;
            break;
          }
          incount += incount2;
          outptr = locals.l_outbuf;
          outleft = locals.l_outbytesleft;
          result += 1;
          goto outcount_zero;
        }
        #endif
        inptr += incount; inleft -= incount;
        //errno = EILSEQ;
        result = -1;
        break;
      }
      if (incount == RET_TOOFEW(0)) {
        /* Case 2: not enough bytes available to detect anything */
        //errno = EINVAL;
        result = -1;
        break;
      }
      /* Case 3: k bytes read, but only a shift sequence */
      incount = DECODE_TOOFEW(incount);
    } else {
      /* Case 4: k bytes read, making up a wide character */
      if (outleft == 0) {
        cd->istate = last_istate;
        //errno = E2BIG;
        result = -1;
        break;
      }
      outcount = cd->ofuncs.xxx_wctomb(cd,outptr,wc,outleft);
      if (outcount != RET_ILUNI)
        goto outcount_ok;
      /* Handle Unicode tag characters (range U+E0000..U+E007F). */
      if ((wc >> 7) == (0xe0000 >> 7))
        goto outcount_zero;
      /* Try transliteration. */
      result++;
	  /*
      if (cd->transliterate) {
        outcount = unicode_transliterate(cd,wc,outptr,outleft);
        if (outcount != RET_ILUNI)
          goto outcount_ok;
      }
      */
      if (cd->discard_ilseq) {
        outcount = 0;
        goto outcount_ok;
      }
      #ifndef LIBICONV_PLUG
      else if (cd->fallbacks.uc_to_mb_fallback != NULL) {
        struct uc_to_mb_fallback_locals locals;
        locals.l_outbuf = outptr;
        locals.l_outbytesleft = outleft;
        locals.l_errno = 0;
        cd->fallbacks.uc_to_mb_fallback(wc,
                                        uc_to_mb_write_replacement,
                                        &locals,
                                        cd->fallbacks.data);
        if (locals.l_errno != 0) {
          cd->istate = last_istate;
          //errno = locals.l_errno;
          return -1;
        }
        outptr = locals.l_outbuf;
        outleft = locals.l_outbytesleft;
        outcount = 0;
        goto outcount_ok;
      }
      #endif
      outcount = cd->ofuncs.xxx_wctomb(cd,outptr,0xFFFD,outleft);
      if (outcount != RET_ILUNI)
        goto outcount_ok;
      cd->istate = last_istate;
      //errno = EILSEQ;
      result = -1;
      break;
    outcount_ok:
      if (outcount < 0) {
        cd->istate = last_istate;
        //errno = E2BIG;
        result = -1;
        break;
      }
      #ifndef LIBICONV_PLUG
      if (cd->hooks.uc_hook)
        (*cd->hooks.uc_hook)(wc, cd->hooks.data);
      #endif
      //if (!(outcount <= outleft)) abort();
      if (!(outcount <= outleft)) return -1;
      outptr += outcount; outleft -= outcount;
    }
  outcount_zero:
    //if (!(incount <= inleft)) abort();
    if (!(incount <= inleft)) return -1;
    inptr += incount; inleft -= incount;
  }
  *inbuf = (const char*) inptr;
  *inbytesleft = inleft;
  *outbuf = (char*) outptr;
  *outbytesleft = outleft;
  return result;
}

static size_t unicode_loop_reset (iconv_t icd,
                                  char* * outbuf, size_t *outbytesleft)
{
  conv_t cd = (conv_t) icd;
  if (outbuf == NULL || *outbuf == NULL) {
    /* Reset the states. */
    memset(&cd->istate,'\0',sizeof(state_t));
    memset(&cd->ostate,'\0',sizeof(state_t));
    return 0;
  } else {
    size_t result = 0;
    if (cd->ifuncs.xxx_flushwc) {
      state_t last_istate = cd->istate;
      ucs4_t wc;
      if (cd->ifuncs.xxx_flushwc(cd, &wc)) {
        unsigned char* outptr = (unsigned char*) *outbuf;
        size_t outleft = *outbytesleft;
        int outcount = cd->ofuncs.xxx_wctomb(cd,outptr,wc,outleft);
        if (outcount != RET_ILUNI)
          goto outcount_ok;
        /* Handle Unicode tag characters (range U+E0000..U+E007F). */
        if ((wc >> 7) == (0xe0000 >> 7))
          goto outcount_zero;
        /* Try transliteration. */
        result++;
		/*
        if (cd->transliterate) {
          outcount = unicode_transliterate(cd,wc,outptr,outleft);
          if (outcount != RET_ILUNI)
            goto outcount_ok;
        }
		*/
        if (cd->discard_ilseq) {
          outcount = 0;
          goto outcount_ok;
        }
        #ifndef LIBICONV_PLUG
        else if (cd->fallbacks.uc_to_mb_fallback != NULL) {
          struct uc_to_mb_fallback_locals locals;
          locals.l_outbuf = outptr;
          locals.l_outbytesleft = outleft;
          locals.l_errno = 0;
          cd->fallbacks.uc_to_mb_fallback(wc,
                                          uc_to_mb_write_replacement,
                                          &locals,
                                          cd->fallbacks.data);
          if (locals.l_errno != 0) {
            cd->istate = last_istate;
            //errno = locals.l_errno;
            return -1;
          }
          outptr = locals.l_outbuf;
          outleft = locals.l_outbytesleft;
          outcount = 0;
          goto outcount_ok;
        }
        #endif
        outcount = cd->ofuncs.xxx_wctomb(cd,outptr,0xFFFD,outleft);
        if (outcount != RET_ILUNI)
          goto outcount_ok;
        cd->istate = last_istate;
        //errno = EILSEQ;
        return -1;
      outcount_ok:
        if (outcount < 0) {
          cd->istate = last_istate;
          //errno = E2BIG;
          return -1;
        }
        #ifndef LIBICONV_PLUG
        if (cd->hooks.uc_hook)
          (*cd->hooks.uc_hook)(wc, cd->hooks.data);
        #endif
        //if (!(outcount <= outleft)) abort();
        if (!(outcount <= outleft)) return -1;
        outptr += outcount;
        outleft -= outcount;
      outcount_zero:
        *outbuf = (char*) outptr;
        *outbytesleft = outleft;
      }
    }
    if (cd->ofuncs.xxx_reset) {
      unsigned char* outptr = (unsigned char*) *outbuf;
      size_t outleft = *outbytesleft;
      int outcount = cd->ofuncs.xxx_reset(cd,outptr,outleft);
      if (outcount < 0) {
        //errno = E2BIG;
        return -1;
      }
      //if (!(outcount <= outleft)) abort();
      if (!(outcount <= outleft)) return -1;
      *outbuf = (char*) (outptr + outcount);
      *outbytesleft = outleft - outcount;
    }
    memset(&cd->istate,'\0',sizeof(state_t));
    memset(&cd->ostate,'\0',sizeof(state_t));
    return result;
  }
}




//extern void *mtos_malloc(unsigned int size);
extern void* mtos_malloc_alias(unsigned int size);
//extern void  mtos_free(void *p_addr);
extern void  mtos_free_alias(void *p_addr);

/*
 * search by encoding name
 */
static int encodings_lookup(const char* name, unsigned char* p_free_entry)
{
  char** p;
  unsigned char i;

  if( NULL != p_free_entry )
  {
    *p_free_entry = -1;
    for( i = 0, p = all_encoding_names; i < sg_encodings_total; i++ )
      if( NULL == *(p+i) )
      {
        *p_free_entry = i;
        break;
      }
  }

  for( i = 0, p = all_encoding_names; i < sg_encodings_total; i++ )
  {
    if( NULL == *(p+i) )
      continue;
    if( 0 == strcmp(name, *(p+i)) )
      return i;
  }
  
  return -1;
}



int iconv_init(unsigned char  n_encodings)
{
  if( 0 == n_encodings )
    return -1;

  sg_encodings_total = n_encodings;
  
  all_encodings = 
      (struct encoding*)mtos_malloc_alias(sg_encodings_total * sizeof(struct encoding));
  if( NULL == all_encodings )
    return -1;
  memset(all_encodings, 0, sg_encodings_total * sizeof(struct encoding));

  all_encoding_names = 
      (char**)mtos_malloc_alias( (sg_encodings_total) * sizeof(char*) );
  if( NULL == all_encoding_names )
    return -1;
  memset(all_encoding_names, 0, (sg_encodings_total) * sizeof(char*));

  return 0;
}

int iconv_deinit()
{
  sg_encodings_total = 0;
  mtos_free_alias(all_encodings);
  mtos_free_alias(all_encoding_names);
  return 0;
}

int iconv_add_encoding(const char* enc_name, struct encoding* p_enc)
{
  unsigned char free_idx;
  if( 0 == sg_encodings_total )
    return -1;
  
  if( -1 ==  encodings_lookup(enc_name, &free_idx) )
  {
    if( -1 == free_idx )
      return -1;
    all_encodings[free_idx].ifuncs = p_enc->ifuncs;
    all_encodings[free_idx].ofuncs = p_enc->ofuncs;
    all_encodings[free_idx].oflags = p_enc->oflags;
    all_encoding_names[free_idx] = (char*)enc_name;
  }
  else
  {
    //encoding already added
    return -1;
  }
  return 0;
}

int iconv_del_encoding(const char* enc_name)
{
  int idx;
  if( 0 == sg_encodings_total )
    return -1;
  
  idx = encodings_lookup(enc_name, NULL);
  if( -1 == idx )
  {
    //did not find
    return -1;
  }
  memset(all_encodings + idx, 0, sizeof(all_encodings[idx]));
  all_encoding_names[idx] = NULL;

  return 0;
}



iconv_t iconv_open (const char* tocode, const char* fromcode)
{
  struct conv_struct * cd;
  int from_index;
  int to_index;

  to_index = encodings_lookup(tocode, NULL);
  if( -1 == to_index)
    return (iconv_t)(-1);

  from_index = encodings_lookup(fromcode, NULL);
  if( -1 == from_index)
    return (iconv_t)(-1);

  cd = (struct conv_struct *)mtos_malloc_alias(sizeof(struct conv_struct));
  if (cd == NULL) {
    return (iconv_t)(-1);
  }

  cd->iindex = from_index;
  cd->ifuncs = all_encodings[from_index].ifuncs;
  cd->oindex = to_index;
  cd->ofuncs = all_encodings[to_index].ofuncs;
  cd->oflags = all_encodings[to_index].oflags;
  cd->lfuncs.loop_convert = unicode_loop_convert;
  cd->lfuncs.loop_reset = unicode_loop_reset;
  memset(&cd->istate,'\0',sizeof(state_t));
  memset(&cd->ostate,'\0',sizeof(state_t));
  cd->transliterate = 0;
  cd->discard_ilseq = 1;
  cd->fallbacks.mb_to_uc_fallback = NULL;
  cd->fallbacks.uc_to_mb_fallback = NULL;
  cd->fallbacks.mb_to_wc_fallback = NULL;
  cd->fallbacks.wc_to_mb_fallback = NULL;
  cd->fallbacks.data = NULL;
  cd->hooks.uc_hook = NULL;
  cd->hooks.wc_hook = NULL;
  cd->hooks.data = NULL;

  return (iconv_t)cd;
}

size_t iconv (iconv_t icd,
              char* * inbuf, size_t *inbytesleft,
              char* * outbuf, size_t *outbytesleft)
{
  conv_t cd = (conv_t) icd;
  if (inbuf == NULL || *inbuf == NULL)
    return cd->lfuncs.loop_reset(icd,outbuf,outbytesleft);
  else
    return cd->lfuncs.loop_convert(icd,
                                   (const char* *)inbuf,inbytesleft,
                                   outbuf,outbytesleft);
}

int iconv_close (iconv_t icd)
{
  conv_t cd = (conv_t) icd;
  mtos_free_alias(cd);
  return 0;
}


