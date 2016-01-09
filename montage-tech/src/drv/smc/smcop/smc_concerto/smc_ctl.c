/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifdef DRV_SEL_SMC

#include <string.h>

#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_fifo.h"
#include "hal_base.h"
#include "hal_concerto_irq.h"
#include "sys_regs_concerto.h"

#include "smc_ctl.h"

#define SCCTL_DEBUG  0

#if SCCTL_DEBUG
 #define SCCTL_P(...)    OS_PRINTF(__VA_ARGS__)
#else
 #define SCCTL_P(...)    do{}while(0)
#endif

/************************************************************************/
//  sc get phyclk sel
BOOL smc_get_phyclk_sel(smc_id_t smc_id)
{
  u8 bitshift = 0;

  if (smc_id == SMC0_ID)
  {
    bitshift = 0;
  }
  else if (smc_id == SMC1_ID)
  {
    bitshift = 16;
  }

  return (*((volatile u32 *)R_CLK_SMC_CFG) >> bitshift) & 0x1;
}

//  sc get status
BOOL smc_get_status(smc_id_t smc_id)
{
    u32 R_SMC_STA = 0;

    if (smc_id == SMC0_ID)
    {
        R_SMC_STA = R_SMC0_STA;
    }
    else if (smc_id == SMC1_ID)
    {
        R_SMC_STA = R_SMC1_STA;
    }

    return ((hal_get_u8((volatile u8 *)R_SMC_STA) & 0x2) >> 1);
}

//  sc get rx status
BOOL smc_rx_notempty(smc_id_t smc_id)
{
    u32 R_SMC_STA = 0;

    if (smc_id == SMC0_ID)
    {
        R_SMC_STA = R_SMC0_STA;
    }
    else if (smc_id == SMC1_ID)
    {
        R_SMC_STA = R_SMC1_STA;
    }

    return (hal_get_u8((volatile u8 *)R_SMC_STA) & 0x1);
}

//  sc get one reply
u8 smc_rx_get(smc_id_t smc_id)
{
    u32 R_SMC_DBUF = 0;

    if (smc_id == SMC0_ID)
    {
        R_SMC_DBUF = R_SMC0_DBUF;
    }
    else if (smc_id == SMC1_ID)
    {
        R_SMC_DBUF = R_SMC1_DBUF;
    }

    return hal_get_u8((volatile u8 *)R_SMC_DBUF);
}

// sc set cmd length
void smc_tx_cmdlen(smc_id_t smc_id, u16 len)
{
    u32 R_SMC_CMDLEN_H = 0;
    u32 R_SMC_CMDLEN_L = 0;

    if (smc_id == SMC0_ID)
    {
      R_SMC_CMDLEN_H = R_SMC0_CMDLEN_H;
      R_SMC_CMDLEN_L = R_SMC0_CMDLEN_L;
    }
    else
    {
      R_SMC_CMDLEN_H = R_SMC1_CMDLEN_H;
      R_SMC_CMDLEN_L = R_SMC1_CMDLEN_L;
    }

    do
    {
        hal_put_u8((volatile u8 *)R_SMC_CMDLEN_H, ((len & 0xFF00) >> 8));
        hal_put_u8((volatile u8 *)R_SMC_CMDLEN_L, ((len & 0xFF)));
    }
    while(0);
}

//  sc set cmd data
void smc_tx_cmd(smc_id_t smc_id, u8 data)
{
    u32 R_SMC_DBUF = 0;

    if (smc_id == SMC0_ID)
    {
        R_SMC_DBUF = R_SMC0_DBUF;
    }
    else if (smc_id == SMC1_ID)
    {
        R_SMC_DBUF = R_SMC1_DBUF;
    }

    hal_put_u8((volatile u8 *)R_SMC_DBUF, data);
}

//  sc buffer fifo count
u16 smc_fifo_cnt(smc_id_t smc_id)
{
    u32 R_SMC_BUF_CNT_L = 0;
    u32 R_SMC_BUF_CNT_H = 0;
    u16 fifo_cnt = 0;

    if (smc_id == SMC0_ID)
    {
        R_SMC_BUF_CNT_L = R_SMC0_BUF_CNT_L;
        R_SMC_BUF_CNT_H = R_SMC0_BUF_CNT_H;
    }
    else if (smc_id == SMC1_ID)
    {
        R_SMC_BUF_CNT_L = R_SMC1_BUF_CNT_L;
        R_SMC_BUF_CNT_H = R_SMC1_BUF_CNT_H;
    }
    else
    {
       return 0;
    }

    fifo_cnt = hal_get_u8((volatile u8 *)R_SMC_BUF_CNT_L) +
                ((hal_get_u8((volatile u8 *)R_SMC_BUF_CNT_H) & 0x80) << 1);

    return fifo_cnt;
}

/************************************************************************/
static void smc_interrupt_set(smc_id_t smc_id, void (*int_sc_handle)(void))
{
  u32 IRQ_SMC_ID = 0;

  if (smc_id == SMC0_ID)
  {
    IRQ_SMC_ID = IRQ_SMC0_ID;
  }
  else if (smc_id == SMC1_ID)
  {
    IRQ_SMC_ID = IRQ_SMC1_ID;
  }

  if (int_sc_handle != NULL)
  {
      mtos_irq_request(IRQ_SMC_ID, int_sc_handle, RISE_EDGE_TRIGGER);
  }
  else
  {
      mtos_irq_release(IRQ_SMC_ID, NULL);
  }
}

/************************************************************************
 * sc set ETU
 ************************************************************************/
static void smc_etu_set(smc_id_t smc_id, u32 ETU)
{
  u8 data = 0;
  u8 etu1 = 0;
  u8 etu2 = 0;
  u8 etu3 = 0;

  u32 REG_SMC_ETU1_SET;
  u32 REG_SMC_ETU2_SET;
  u32 REG_SMC_ETU3_SET;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_ETU1_SET = R_SMC0_ETU1_SET;
      REG_SMC_ETU2_SET = R_SMC0_ETU2_SET;
      REG_SMC_ETU3_SET = R_SMC0_ETU3_SET;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_ETU1_SET = R_SMC1_ETU1_SET;
      REG_SMC_ETU2_SET = R_SMC1_ETU2_SET;
      REG_SMC_ETU3_SET = R_SMC1_ETU3_SET;
  }
  else
  {
      return;
  }

  etu1 = (u8)((ETU&0xFF0000) >> 16);
  etu2 = (u8)((ETU&0xFF00) >> 8);
  etu3 = (u8)((ETU&0xFF));

  data = hal_get_u8 ((volatile u8 *)REG_SMC_ETU1_SET);
  data &= ~(0x3);
  data |= etu1;
  hal_put_u8 ((volatile u8 *)REG_SMC_ETU1_SET, data);

  hal_put_u8 ((volatile u8 *)REG_SMC_ETU2_SET, etu2);
  hal_put_u8 ((volatile u8 *)REG_SMC_ETU3_SET, etu3);

}

/**************** 1518 unpdate smc ctrl part *************************/
static void smc_conf_set_freqdiv(smc_id_t smc_id, u8 freq_div)
{
  u8 data = 0;
  u32 R_SMC_FRQ_CFG = 0;

  if (smc_id == SMC0_ID)
  {
      R_SMC_FRQ_CFG = R_SMC0_FRQ_CFG;
  }
  else if (smc_id == SMC1_ID)
  {
      R_SMC_FRQ_CFG = R_SMC1_FRQ_CFG;
  }

  data = hal_get_u8 ((volatile u8 *)R_SMC_FRQ_CFG);
  data &= ~(0x1F << 1);
  data |= (freq_div << 1);
  hal_put_u8((volatile u8 *)R_SMC_FRQ_CFG, data);
}

static void smc_conf_set_phyclk(smc_id_t smc_id, u8 phyclk)
{
  u32 data = 0;
  u8 bitshift = 0;
  u8 frq_div = 0;

  if (smc_id == SMC0_ID)
  {
    bitshift = 0;
  }
  else if (smc_id == SMC1_ID)
  {
    bitshift = 16;
  }

  data = hal_get_u32 ((volatile u32 *)R_CLK_SMC_CFG);
  if (phyclk == SMC_PHYCLK_APB)
  {
    data &= ~ (1 << bitshift);
    frq_div = SMC_FREQUENCY_DIV_APB;
  }
  else if (phyclk == SMC_PHYCLK_12MHZ)
  {
    data |= 1 << bitshift;
    frq_div = SMC_FREQUENCY_DIV_12MHZ;
  }
  hal_put_u32((volatile u32 *)R_CLK_SMC_CFG, data);

  smc_conf_set_freqdiv(smc_id, frq_div);
}

static void smc_conf_set_pinmode(smc_id_t smc_id,
  u8 iopin_mode, u8 rstpin_mode, u8 clkpin_mode)
{
  u32 R_SMC_PIN_CFG = 0;
  u8 data = 0;

  if (smc_id == SMC0_ID)
  {
    R_SMC_PIN_CFG = R_SMC0_PIN_CFG;
  }
  else if (smc_id == SMC1_ID)
  {
    R_SMC_PIN_CFG = R_SMC1_PIN_CFG;
  }

  if (iopin_mode)
  {
    data |= 1 << 0;
  }
  if (rstpin_mode)
  {
    data |= 1 << 1;
  }
  if (clkpin_mode)
  {
    data |= 1 << 2;
  }

  hal_put_u8((volatile u8 *)R_SMC_PIN_CFG, data);
}

static void smc_conf_set_socket(smc_id_t smc_id, u8 socket_type)
{
  u32 REG_SMC_FRQ_CFG = 0;
  u8 data = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_FRQ_CFG = R_SMC0_FRQ_CFG;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_FRQ_CFG = R_SMC1_FRQ_CFG;
  }
  else
  {
      return;
  }

  data = hal_get_u8((volatile u8 *)REG_SMC_FRQ_CFG);
  if (socket_type == SMC_SOCKET_CLOSED)
  {
    data &= ~0x1;
  }
  else if (socket_type == SMC_SOCKET_OPEN)
  {
    data |= 0x1;
  }
  hal_put_u8((volatile u8 *)REG_SMC_FRQ_CFG, data);
}

static void smc_nc_set(smc_id_t smc_id, u8 protect_time)
{
  u32 REG_SMC_NC_SET = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_NC_SET = R_SMC0_NC_SET;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_NC_SET = R_SMC1_NC_SET;
  }
  else
  {
      return;
  }

  hal_put_u8((volatile u8 *)REG_SMC_NC_SET, protect_time);
}

static void smc_ctrl_set (smc_id_t smc_id,
    smc_bitstop_t stop,
        u8 ctrl_nset_en,
        u8 ctrl_check_en,
        u8 ctrl_parity_en,
        u8 ctrl_ieinsert_en,
        u8 ctrl_ieremove_en,
        u8 ctrl_ierx_en,
        u8 ctrl_iecmd_en)
{
  u8 data = 0;
  u32 REG_SMC_TRANS_CTRL = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC0_TRANS_CTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC1_TRANS_CTRL;
  }
  else
  {
      return;
  }

  data |= (stop << 7);
  data |= (ctrl_nset_en << 6);
  data |= (ctrl_check_en << 5);
  data |= (ctrl_parity_en << 4);
  data |= (ctrl_ieinsert_en << 3);
  data |= (ctrl_ieremove_en << 2);
  data |= (ctrl_ierx_en << 1);
  data |= (ctrl_iecmd_en);

  hal_put_u8((volatile u8 *)REG_SMC_TRANS_CTRL, data);
}

static void smc_ctrl_set_stop(smc_id_t smc_id, smc_bitstop_t stop)
{
  u8 data = 0;
  u32 REG_SMC_TRANS_CTRL = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC0_TRANS_CTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC1_TRANS_CTRL;
  }
  else
  {
      return;
  }

  data = hal_get_u8 ((volatile u8 *)REG_SMC_TRANS_CTRL);
  if (stop == SMC_STOP_TWO)
  {
    data &= (~(1 << 7));
  }
  else if (stop == SMC_STOP_ONE)
  {
    data |= (1 << 7);
  }
  hal_put_u8((volatile u8 *)REG_SMC_TRANS_CTRL, data);
}

static void smc_ctrl_set_check(smc_id_t smc_id, u8 ctrl_check_en)
{
  u8 data = 0;
  u32 REG_SMC_TRANS_CTRL = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC0_TRANS_CTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC1_TRANS_CTRL;
  }
  else
  {
      return;
  }

  data = hal_get_u8 ((volatile u8 *)REG_SMC_TRANS_CTRL);
  if (ctrl_check_en == SMC_DISABLE)
  {
    data &= (~(1 << 5));
  }
  else if (ctrl_check_en == SMC_ENABLE)
  {
    data |= (1 << 5);
  }
  hal_put_u8((volatile u8 *)REG_SMC_TRANS_CTRL, data);
}

static void smc_ctrl_set_parity(smc_id_t smc_id, u8 ctrl_parity_en)
{
  u8 data = 0;
  u32 REG_SMC_TRANS_CTRL = 0;

  if (smc_id == SMC0_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC0_TRANS_CTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      REG_SMC_TRANS_CTRL = R_SMC1_TRANS_CTRL;
  }
  else
  {
      return;
  }

  data = hal_get_u8 ((volatile u8 *)REG_SMC_TRANS_CTRL);
  if (ctrl_parity_en == SMC_DISABLE)
  {
    data &= (~(1 << 4));
  }
  else if (ctrl_parity_en == SMC_ENABLE)
  {
    data |= (1 << 4);
  }
  hal_put_u8((volatile u8 *)REG_SMC_TRANS_CTRL, data);
}

static void smc_delay_us(int t)
{
  u32 i = 0;
  u32 cnt = t * 8;

  for(i = 0;i < cnt;i++)
  {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

/***********************************************************************
 * Initialize SC Device Block
 **********************************************************************/
void smc_init(smc_id_t smc_id, void (*smc_interrupt_handle)(void), u8 phyclk, u8 convt_en,
  u8 convt_value, u8 detect_pin_pol, u8 vcc_enable_pol,
  u8 iopin_mode, u8 rstpin_mode, u8 clkpin_mode)
{
  smc_bitstop_t stop  = SMC_STOP_TWO;
  u8 ctrl_nset_en = SMC_DISABLE;
  u8 ctrl_check_en = SMC_ENABLE;
  u8 ctrl_parity_en = SMC_ENABLE;
  u8 ctrl_ieinsert_en = SMC_ENABLE;
  u8 ctrl_ieremove_en = SMC_ENABLE;
  u8 ctrl_ierx_en = SMC_ENABLE;
  u8 ctrl_iecmd_en = SMC_DISABLE;

  u32 reg_smc_cpctrl = 0;

  if (smc_id == SMC0_ID)
  {
      reg_smc_cpctrl = R_SMC0_CPCTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      reg_smc_cpctrl = R_SMC1_CPCTRL;
  }

  SCCTL_P("\nsmc_init(%d)...", smc_id);

  smc_conf_set_socket(smc_id, (detect_pin_pol == 1) ? SMC_SOCKET_OPEN : SMC_SOCKET_CLOSED);

  (*(volatile unsigned char *)reg_smc_cpctrl) |= 1 << 2;
  (*(volatile unsigned char *)reg_smc_cpctrl) |= 1 << 4;
  (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1 << 5);

  smc_conf_set_phyclk(smc_id, phyclk);
  smc_conf_set_pinmode(smc_id, iopin_mode, rstpin_mode, clkpin_mode);

  smc_nc_set (smc_id, 0);
  smc_ctrl_set (smc_id,
      stop,
      ctrl_nset_en,
      ctrl_check_en,
      ctrl_parity_en,
      ctrl_ieinsert_en,
      ctrl_ieremove_en,
      ctrl_ierx_en,
      ctrl_iecmd_en);

  smc_interrupt_set(smc_id, smc_interrupt_handle);

  smc_set_etu(smc_id, SMC_ETU_DEFAULT | (convt_en << 7) | (convt_value << 6));

  /* SC Insert/Remove status Change */
  if (smc_get_status(smc_id))
  {
      if (vcc_enable_pol)
        (*(volatile unsigned char *)reg_smc_cpctrl) |= (1<<0); // VCC
      else
        (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1<<0); // VCC
      (*(volatile unsigned char *)reg_smc_cpctrl) |= (1<<6); // DATA
      smc_delay_us(120);
      (*(volatile unsigned char *)reg_smc_cpctrl) |= (1<<5); // CLK
      smc_delay_us(600);
      (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1<<2); // RST
  }
  else
  {
      (*(volatile unsigned char *)reg_smc_cpctrl) |= (1<<2); // RST
      (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1<<5); // CLK
      (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1<<6); // DATA
      if (vcc_enable_pol)
        (*(volatile unsigned char *)reg_smc_cpctrl) &= ~(1<<0); // VCC
      else
        (*(volatile unsigned char *)reg_smc_cpctrl) |= (1<<0); // VCC
  }
  //smc_rstctrl_set_powerctrl(SMC_ENABLE);
  //enable_smc();
}

/************************************************************************
 * SMC Soft Reset
 * param:  smc_id
 *              delays
 * return: <0: FALSE 0:TRUE
 ************************************************************************/
void smc_reset(smc_id_t smc_id, u16 delays)
{
    u8 data = 0;
    u32 reg_smc_cpctrl = 0;

    if (smc_id == SMC0_ID)
    {
        reg_smc_cpctrl = R_SMC0_CPCTRL;
    }
    else if (smc_id == SMC1_ID)
    {
        reg_smc_cpctrl = R_SMC1_CPCTRL;
    }
    else
    {
        return;
    }

    data = hal_get_u8((volatile u8 *)reg_smc_cpctrl);
    data |= (1 << 2);
    hal_put_u8((volatile u8 *)reg_smc_cpctrl, data);

    mtos_task_delay_ms(delays); //Delay Time is delaysms

    data = hal_get_u8((volatile u8 *)reg_smc_cpctrl);
    data &=(~(1 << 2));
    hal_put_u8((volatile u8 *)reg_smc_cpctrl, data);
}

/************************************************************************
 * SMC FIFO Reset
 * param:  void
 * return: void
 ************************************************************************/
void smc_flush(smc_id_t smc_id)
{
  u8 data = 0;
  u32 reg_smc_cpctrl = 0;

  if (smc_id == SMC0_ID)
  {
      reg_smc_cpctrl = R_SMC0_CPCTRL;
  }
  else if (smc_id == SMC1_ID)
  {
      reg_smc_cpctrl = R_SMC1_CPCTRL;
  }
  else
  {
      return;
  }

  data = hal_get_u8((volatile u8 *)reg_smc_cpctrl);
  data |= (1 << 1);
  hal_put_u8((volatile u8 *)reg_smc_cpctrl, data);

  data &= (~(1 << 1));
  hal_put_u8((volatile u8 *)reg_smc_cpctrl, data);
}

/************************************************************************
 * sc set bit coding
 * T14: no check, no parity
 * othes: check and parity
 ************************************************************************/
void  smc_set_bitcoding(smc_id_t smc_id, BOOL bT14)
{
  if (bT14)
  {
    if (!smc_get_phyclk_sel(smc_id)) // APB CLK
      smc_conf_set_freqdiv(smc_id, SMC_FREQUENCY_DIV_T14);
    smc_ctrl_set_stop(smc_id, SMC_STOP_ONE);
    smc_ctrl_set_check(smc_id, SMC_DISABLE);
    smc_ctrl_set_parity(smc_id, SMC_DISABLE);
  }
  else
  {
    if (!smc_get_phyclk_sel(smc_id)) // APB CLK
      smc_conf_set_freqdiv(smc_id, SMC_FREQUENCY_DIV_DEFAULT);
    smc_ctrl_set_stop(smc_id, SMC_STOP_TWO);
    smc_ctrl_set_check(smc_id, SMC_ENABLE);
    smc_ctrl_set_parity(smc_id, SMC_ENABLE);
  }
}

void smc_set_etu(smc_id_t smc_id, u32 ETU)
{
  smc_etu_set(smc_id, ETU);
}

void smc_controller_glb_rst(smc_id_t smc_id)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  u8 reset_bit = 0;

  if (smc_id == SMC0_ID)
  {
    reset_bit = 18;
  }
  else if (smc_id == SMC1_ID)
  {
    reset_bit = 19;
  }
  else
  {
    SCCTL_P("\nInvalid Smart Card ID\n");
    return;
  }

  ptmp = R_RST_REQ(2);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp |= 1 << reset_bit;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_RST_ALLOW(2);
  do
  {
    dtmp = hal_get_u32((volatile u32 *)ptmp);
  }
  while (!((dtmp >> reset_bit) & 0x1));

  ptmp = R_RST_CTRL(2);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~ (1 << reset_bit);
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  dtmp |= 1 << reset_bit;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_RST_REQ(2);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~ (1 << reset_bit);
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

#endif //#ifdef DRV_SEL_SMC
