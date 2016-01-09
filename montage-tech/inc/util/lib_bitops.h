/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __LIB_BITOPS_H__
#define __LIB_BITOPS_H__

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define BITS_PER_LONG 32

#define BIT(nr)			(1UL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

void set_bit(u32 num, u32 *addr);
void clear_bit(u32 num, u32 *addr);
void set_clear_bit(u32 num, u32 *addr, u8 set);
u32 find_next_zero_bit(u32 *addr, u32 offset);
void change_bit(u32 nr, u32 *addr);
s32 test_bit(u32 nr, u32 *addr);
s32 test_and_set_bit(u32 nr, u32 *addr);
s32 test_and_clear_bit(u32 nr, u32 *addr);
s32 test_and_change_bit(u32 nr, u32 *addr);

#endif //__LIB_BITOPS_H__


