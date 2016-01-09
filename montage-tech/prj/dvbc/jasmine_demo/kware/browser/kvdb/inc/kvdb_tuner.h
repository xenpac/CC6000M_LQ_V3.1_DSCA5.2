/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _KVDB_TUNER_H_
#define _KVDB_TUNER_H_

#ifdef __cplusplus
	extern "C" {
#endif

int kvdb_porting_delivery_tune(int frequency, int symbol_rate, int modulation);

unsigned short kvdb_porting_get_startup_pid(void);
void kvdb_turner_deinit(void);


#ifdef __cplusplus
}
#endif

#endif

