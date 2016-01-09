/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __M88DC2800_H__
#define __M88DC2800_H__

#define MAX_BS_TIMES 3
#define MAX_TP_ONE_SCAN 50

#define X_TAL 28800
#define DC2800_SLEEP(n) (mtos_task_delay_ms (n))
#define DEMOD_I2C_ADDR 0x38
/* tuner default i2c address setting */ 
#define TUNER_TDCC_G051F_I2C_ADDR     0xC0
#define  TUNER_I2C_ADDR_TC2800        0xc2

#define CONTAIN_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - (int)&((type *)0)->member)) 

typedef enum
{
    TC2K_b2wAddress_C0 = 0xC0,
    TC2K_b2wAddress_C2 = 0xC2,
    TC2K_b2wAddress_C4 = 0xC4,
    TC2K_b2wAddress_C6 = 0xC6
} TC2K_b2wAddress;

typedef enum 
{
  TN_TDCC_G051F = 1,
  TN_TC2000,
  TN_TC2800,
  TN_XDCT6A,
}dc2800_tn_id_t;

typedef struct
 {
    dc2800_tn_id_t tn_id;
    s32 (*init)(void);
    u32 (*get_signal_strength)(void);
    s32 (*set_freq) (u32 freq_KHz, nim_modulation_t qam);
}dc2800_tn_drv_t;

typedef struct
{
    u32 x_crystal;
    nim_channel_info_t cur_channel;
    u8 dmd_addr;
    u8 chip_mode; // 0: new DC2800		1: new Jazz			2: old DC2800     3:new concerto
    u8 tuner_loopthrough;
    u8 tuner_mode; //tuner mode if needed, 0: DVB-C, 2: MMDS
    u8 tuner_bandwidth;
    u8 tuner_type;
    i2c_bus_t *p_i2c;
    MT_FE_DC2800_SETTINGS handle;
}nim_m88dc280_priv_t;

extern os_sem_t reg_rw_mutex;

void dc2800_dmd_reg_write_unlock(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
u8 dc2800_dmd_reg_read_unlock(MT_FE_DC2800_Device_Handle p_handle, u8 reg);
void dc2800_dmd_reg_write(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
u8 dc2800_dmd_reg_read(MT_FE_DC2800_Device_Handle p_handle, u8 reg);
void dc2800_tn_reg_write(MT_FE_DC2800_Device_Handle p_handle, u8 reg, u8 data);
u8 dc2800_tn_reg_read(MT_FE_DC2800_Device_Handle p_handle, u8 reg);
s32 dc2800_tn_std_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_tn_seq_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_dmd_std_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_dmd_seq_read(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 wlen, u32 rlen);
s32 dc2800_tn_seq_write(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 len);
s32 dc2800_dmd_seq_write(MT_FE_DC2800_Device_Handle p_handle, u8 *p_buf, u32 len);
MT_FE_DC2800_Device_Handle dc_2800_get_handle(void);

extern u16 parse_qam(nim_modulation_t qam_id);

#endif
