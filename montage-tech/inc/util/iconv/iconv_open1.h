/*
 * Copyright (C) 1999-2008, 2011 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* Part 1 of iconv_open.
   Input: const char* tocode, const char* fromcode.
   Output:
     unsigned int from_index;
     int from_wchar;
     unsigned int to_index;
     int to_wchar;
     int transliterate;
     int discard_ilseq;
   Jumps to 'invalid' in case of errror.
 */
{
  //char buf[MAX_WORD_LENGTH+10+1];
  //const char* cp;
  //char* bp;
  const struct alias * ap;
  //unsigned int count;

  transliterate = 0;
  discard_ilseq = 0;

  /* Before calling aliases_lookup, convert the input string to upper case,
   * and check whether it's entirely ASCII (we call gperf with option "-7"
   * to achieve a smaller table) and non-empty. If it's not entirely ASCII,
   * or if it's too long, it is not a valid encoding name.
   */
  for (to_wchar = 0;;) {
    /* Search tocode in the table. */
	/*
    for (cp = tocode, bp = buf, count = MAX_WORD_LENGTH+10+1; ; cp++, bp++) {
      unsigned char c = * (unsigned char *) cp;
      OS_PRINTF("%c\n", c);
     OS_PRINTF("count = %d\n", count);
      OS_PRINTF("0x%x\n", c);
         OS_PRINTF("3\n");	 
  if (c >= 0x80){
	 OS_PRINTF("x\n");
        goto invalid;
      	}
      OS_PRINTF("1\n");	 
      if (c >= 'a' && c <= 'z')
        c -= 'a'-'A';
      OS_PRINTF("2\n");	 
      *bp = c;
      OS_PRINTF("*bp = %c\n", *bp);
	  
      if (c == '\0')
        break;
      if (--count == 0){
	 OS_PRINTF("y\n");
        goto invalid;
      	}
    }
    */
//    OS_PRINTF("a\n");
	/*
    for (;;) {
      if (bp-buf >= 10 && memcmp(bp-10,"//TRANSLIT",10)==0) {
        bp -= 10;
        *bp = '\0';
        transliterate = 1;
        continue;
      }
      if (bp-buf >= 8 && memcmp(bp-8,"//IGNORE",8)==0) {
        bp -= 8;
        *bp = '\0';
        discard_ilseq = 1;
        continue;
      }
      break;
    }
	*/
	/*
    OS_PRINTF("b\n");
	
    if (buf[0] == '\0') {
        goto invalid;
    }
	*/
//   OS_PRINTF("tocode: %s\n", tocode);
//	OS_PRINTF("str len = %d\n", strlen(tocode));
    ap = aliases_lookup(tocode,strlen(tocode));
    if (ap == NULL) {
      ap = aliases2_lookup(buf);
      if (ap == NULL)
        goto invalid;
    }
//    OS_PRINTF("c\n");
    to_index = ap->encoding_index;
    break;
  }
  //OS_PRINTF("to_index: %d\n", to_index);
  for (from_wchar = 0;;) {
    /* Search fromcode in the table. */
	/*
    for (cp = fromcode, bp = buf, count = MAX_WORD_LENGTH+10+1; ; cp++, bp++) {
      unsigned char c = * (unsigned char *) cp;
      if (c >= 0x80)
        goto invalid;
      if (c >= 'a' && c <= 'z')
        c -= 'a'-'A';
      *bp = c;
      if (c == '\0')
        break;
      if (--count == 0)
        goto invalid;
    }
    for (;;) {
      if (bp-buf >= 10 && memcmp(bp-10,"//TRANSLIT",10)==0) {
        bp -= 10;
        *bp = '\0';
        continue;
      }
      if (bp-buf >= 8 && memcmp(bp-8,"//IGNORE",8)==0) {
        bp -= 8;
        *bp = '\0';
        continue;
      }
      break;
    }
    if (buf[0] == '\0') {
        goto invalid;
    }
	*/
//	OS_PRINTF("fromcode: %s\n", fromcode);
//	OS_PRINTF("str len fromcode: %d\n", strlen(fromcode));
    ap = aliases_lookup(fromcode,strlen(fromcode));
//	OS_PRINTF("addr ap1: 0x%x\n", ap);
    if (ap == NULL) {
      ap = aliases2_lookup(buf);
      if (ap == NULL)
        goto invalid;
    }
    from_index = ap->encoding_index;
//    OS_PRINTF("from_index: %d\n", from_index);
	
    break;
  }
}
