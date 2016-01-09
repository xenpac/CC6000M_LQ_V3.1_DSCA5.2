
/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef _KVDB_PICTURE_H_
#define _KVDB_PICTURE_H_

#ifdef __cplusplus
	extern "C" {
#endif
/************************************/
void  kvdb_osddrv_init(void );
void  kvdb_osddrv_deinit(void);

/******************************************/
/*Get Display information, The function is belong to display part.*/
int kvdb_porting_graphics_getInfo(int *width, int *height, void **osd_buf, int *bufwidth, int *bufheight);

/*draw image, This function is belong to display part.*/
int kvdb_porting_draw_image(int x, int y, int w, int h, unsigned char *bits, int w_src);


#ifdef __cplusplus
	}
#endif

#endif


