/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_MOSAIC_API_H__
#define __UI_MOSAIC_API_H__

u8 ui_mosaic_get_logic_cell_id();

u8 ui_mosaic_get_mosaic_view();

BOOL ui_init_mosaic();

BOOL ui_mosaic_get_cur_mosaic_prog(dvbs_prog_node_t* cur_mosaic_pg);

BOOL ui_mosaic_get_cur_logic_cell_prog(dvbs_prog_node_t* cur_logic_cell_pg);

BOOL ui_mosaic_get_logic_cell_prog(u8 logic_cell_id);

u16 ui_mosaic_get_cur_cell_prog_id(void);

BOOL ui_mosaic_change_logic_cell_id(u16 msg);

BOOL change_mosaic_prog(s8 offset);

BOOL ui_mosaic_calc_cell_rect(rect_t *rc);

BOOL ui_mosaic_calc_cell_rect_by_logic_cell_id(rect_t *rc, u8 lg_cell_id);

BOOL ui_mosaic_exit_cell_prog(void);

BOOL ui_mosaic_exit(void);

#endif

