/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "mtos_fifo.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "hal_concerto_irq.h"
#include "drv_dev.h"
#include "hal_base.h"
#include "hal_misc.h"
#include "uio.h"
#include "../uio_priv.h"

#define IRDA_DEBUG 0
#if IRDA_DEBUG
#define IRDA_PRINT OS_PRINTF
#else
#define IRDA_PRINT(...)     do {} while(0)
#endif

#define DEFAULT_SAMPLE_FREQ 1 // 187.5KHz by default
#define RC5_WF_IGNORE_T_BIT 1 // ignore T bit for RC5 wave-filter

#define IRDA_CONCERTO_CHK_RPTKEY() (hal_get_u32((volatile u32 *)R_IR_GLOBAL_STA) & 0x01)
#define IRDA_CONCERTO_CHK_OVERFLOW() ((hal_get_u32((volatile u32 *)R_IR_GLOBAL_STA) >> 1) & 0x01)
#define IRDA_CONCERTO_CHK_KUPSTA() ((hal_get_u32((volatile u32 *)R_IR_INT_RAWSTA) >> 1) & 0x1)
#define IRDA_CONCERTO_CHK_KDNSTA() (hal_get_u32((volatile u32 *)R_IR_INT_RAWSTA) & 0x1)
#define IRDA_CONCERTO_CHK_WFSTA() ((hal_get_u32((volatile u32 *)R_IR_INT_RAWSTA) >> 4) & 0xF)

#define GET_R_IR_WAVEFILT_MASK(n) (R_IR_BASE_ADDR + 0x90 + (4 * n))
#define GET_R_IR_WFN_ONTSET(n) (R_IR_BASE_ADDR + 0x100 + (8 * n))
#define GET_R_IR_WFN_PRDSET(n) (R_IR_BASE_ADDR + 0x104 + (8 * n))

#define IRDA_RPT_TIME_MS 300

/*****************************************************************
* IRDA external APIs declaration
*****************************************************************/
extern BOOL uio_check_rpt_key_concerto(uio_type_t uio, u8 code, u8 ir_index);

/*****************************************************************
* IRDA internal APIs declaration
*****************************************************************/
void irda_concerto_soft_reset(void);
static void irda_concerto_idle_set(u8 idle);
static void irda_concerto_sample_clk_set(u8 sample_clk);
static void irda_concerto_wave_len_set(u8 channel, u8 len);
static void irda_concerto_wave_add_set(u8 channel, u8 addr);
static void irda_concerto_wave_mask_bit_set(u32 bit, u32 value);
static void irda_concerto_wave_channel_set(u8 channel, u8 is_en);
static void irda_concerto_wave_channel_int_set(u8 channel_int, u8 is_en);
static void irda_concerto_wave_wfn_set(u8 n, u16 ontime_l, u16 ontime_h, u16 period_l, u16 period_h);

/*****************************************************************
* IRDA internal variables
*****************************************************************/
static BOOL is_ir_intialized = 0;

typedef struct
{
  BOOL print_user_code;
  u16 sw_user_code[IRDA_MAX_USER];
  u8 user_code_set;
  u8 irda_protocol;
  u16 user_code;
  u32 irda_nec_repeat_time;
  u8 irda_sw_recv_decimate;
  u8 irda_wfilt_channel;
  u32 irda_flag;
  irda_wfilt_cfg_t irda_wfilt_channel_cfg[IRDA_MAX_CHANNEL];
}irda_priv_t;

typedef struct
{
  u16 ontime;
  u16 period;
  u8 protocol;
} irda_dec_para_t;

typedef struct
{
  union
  {
    struct
    {
      u16 usercode;
      u16 code;
    } nec;
    struct
    {
      u8 ucode;
      u8 kcode;
    } konka;
    struct
    {
      u8 ucode;
      u8 kcode;
    } rc5;
    struct
    {
      u32 ucode;
      u16 kcode;
    } pana;
  } r;
  u32 state;
  u16 *p_sigtbl;
  u8 counter;
  u8 repeat_interval;
} irda_dec_result_t;

/*!
  IRDA Protocol
*/
typedef enum
{
  IRDA_SW_NEC = 1,
  IRDA_SW_KONKA,
  IRDA_SW_RC5,
  IRDA_SW_PANA
} irda_sw_pro_t;

/*!
  IRDA NEC Decoder State
*/
typedef enum
{
  E_NEC_STA_IDLE = 0,
  E_NEC_STA_START,
  E_NEC_STA_USRL,
  E_NEC_STA_USRH,
  E_NEC_STA_KEY,
  E_NEC_STA_YEK,
  E_NEC_STA_REPEAT,
  E_NEC_STA_FINISH
} irda_pro_nec_status_t;

/*!
  IRDA NEC Decoder Signal
*/
typedef enum
{
  E_NEC_SIG_START = 0,
  E_NEC_SIG_0,
  E_NEC_SIG_1,
  E_NEC_SIG_REPEAT,
  E_NEC_SIG_INVALID
} irda_pro_nec_sig_t;

#if DEFAULT_SAMPLE_FREQ //  Sample freq 187.5 KHz
static u16 nec_sigtbl[16] = {1180, 2192, 2284, 3288, 70, 130, 140, 270, 70, 130, 300, 540, 1180,
        2192, 1476, 2280};
#else //  Sample freq 46.875 KHz
static u16 nec_sigtbl[16] = {295, 548, 571, 822, 17, 45, 35, 68, 17, 45, 75, 135, 295,
        548, 369, 570};
#endif

#if IRDA_DEBUG
static const char * nec_sig_strs[5] = {
"E_NEC_SIG_START","E_NEC_SIG_0","E_NEC_SIG_1",
"E_NEC_SIG_REPEAT", "E_NEC_SIG_INVALID"
};

static const char * nec_sta_strs[8] = {
"E_NEC_STA_IDLE","E_NEC_STA_START","E_NEC_STA_USRL",
"E_NEC_STA_USRH", "E_NEC_STA_KEY","E_NEC_STA_YEK",
"E_NEC_STA_REPEAT", "E_NEC_STA_FINISH"
};
#endif

/*!
  IRDA Konka Decoder State
  */
typedef enum
{
  E_KK_STA_IDLE = 0,
  E_KK_STA_START,
  E_KK_STA_USR,
  E_KK_STA_KEY,
  E_KK_STA_SYNC,
  E_KK_STA_FINISH
} irda_pro_kk_status_t;

/*!
  IRDA Konka Decoder Signal
  */
typedef enum
{
  E_KK_SIG_START = 0,
  E_KK_SIG_BIT0,
  E_KK_SIG_BIT1,
  E_KK_SIG_SYNC,
  E_KK_SIG_INVALID
}  irda_pro_kk_sig_t;

#if DEFAULT_SAMPLE_FREQ //  Sample freq 187.5 KHz
static u16 konka_sigtbl[16] = {520, 608, 1040, 1200, 88, 128, 350, 400, 88, 120, 540, 590, 88,
        128, 750, 856};
#else //  Sample freq 46.875 KHz
static u16 konka_sigtbl[16] = {130, 160, 260, 300, 20, 40, 87, 100, 20, 40, 135, 148, 20,
      40, 187, 213};
#endif

#if IRDA_DEBUG
static const char * kk_sig_strs[5] = {
"E_KK_SIG_START","E_KK_SIG_BIT0","E_KK_SIG_BIT1",
"E_KK_SIG_SYNC", "E_KK_SIG_INVALID"
};

static const char * kk_sta_strs[6] = {
"E_KK_STA_IDLE","E_KK_STA_START","E_KK_STA_USR",
"E_KK_STA_KEY","E_KK_STA_SYNC","E_KK_STA_FINISH"
};
#endif

static u16 rc5_sigtbl[8] = {133, 266, 266, 433, 433, 600, 533, 800};

/*!
  IRDA RC5 Decoder State
*/
typedef enum
{
  E_RC5_STA_IDLE = 0,
  E_RC5_STA_S1,
  E_RC5_STA_S2,
  E_RC5_STA_T,
  E_RC5_STA_USR,
  E_RC5_STA_KEY,
  E_RC5_STA_FINISH,
  E_RC5_STA_INVALID
} irda_pro_rc5_status_t;

/*!
  IRDA Pana Decoder State
  */
typedef enum
{
  E_PANA_STA_IDLE = 0,
  E_PANA_STA_START,
  E_PANA_STA_USR,
  E_PANA_STA_KEY,
  E_PANA_STA_SYNC,
  E_PANA_STA_FINISH
} irda_pro_pana_status_t;

/*!
  IRDA Pana Decoder Signal
  */
typedef enum
{
  E_PANA_SIG_START = 0,
  E_PANA_SIG_BIT0,
  E_PANA_SIG_BIT1,
  E_PANA_SIG_INVALID
}  irda_pro_pana_sig_t;

//static u32 pana_sigtbl[12] = {546, 819, 816, 1224, 57, 86, 114, 172, 57, 86, 259, 390};
static u16  pana_sigtbl[12] = {546, 819, 816, 1224, 53, 90, 114, 172, 53, 90, 259, 390};
#ifdef IRDA_DEBUG
char * pana_sig_strs[4] = {
"E_PANA_SIG_START","E_PANA_SIG_BIT0","E_PANA_SIG_BIT1",
"E_PANA_SIG_INVALID"
};

char * pana_sta_strs[5] = {
"E_PANA_STA_IDLE","E_PANA_STA_START","E_PANA_STA_USR",
"E_PANA_STA_KEY","E_PANA_STA_FINISH"
};
#endif
/*****************************************************************
* IRDA internal functions
*****************************************************************/
static void irda_concerto_get_ontime_and_period(u16 * p_ontime, u16 * p_period)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  u16 m1dat_ont;
  u16 m1dat_prd;

  // Data FIFO Pop
  ptmp = R_IR_COMBUF_POP;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp |= 0x1;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  // Data FIFO fetch
  ptmp = R_IR_COM_DATA;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  m1dat_ont = dtmp & 0xFFFF;
  m1dat_prd = (dtmp & 0xFFFF0000) >> 16;

  *p_ontime = m1dat_ont;
  *p_period = m1dat_prd;
}

static u8 irda_concerto_get_recv_num(void)
{
  return ((hal_get_u32((volatile u32 *)R_IR_GLOBAL_STA) >> 8) & 0xFF);
}

static u8 irda_concerto_nec_decoder_parse_sig(u16 ontime, u16 period, u16 * sigtbl)
{
  if((ontime >= sigtbl[0]) && (ontime <= sigtbl[1]) &&
      (period >= sigtbl[2]) && (period <= sigtbl[3]))
  {
    return E_NEC_SIG_START;
  }
  else if((ontime >= sigtbl[4]) && (ontime <= sigtbl[5]) &&
          (period >= sigtbl[6]) && (period <= sigtbl[7]))
  {
    return E_NEC_SIG_0;
  }
  else if((ontime >= sigtbl[8]) && (ontime <= sigtbl[9]) &&
          (period >= sigtbl[10]) && (period <= sigtbl[11]))
  {
    return E_NEC_SIG_1;
  }
  else if((ontime >= sigtbl[12]) && (ontime <= sigtbl[13]) &&
          (period >= sigtbl[14]) && (period <= sigtbl[15]))
  {
    return E_NEC_SIG_REPEAT;
  }
  else
  {
    return E_NEC_SIG_INVALID;
  }
}

inline static BOOL irda_concerto_nec_decode_check_keycode(u16 code)
{
  u8 ch = (code >> 8) & 0xff;
  u8 cl = code & 0xff;
  if((ch + cl) == 0xff)
    return TRUE;
  else
    return FALSE;
}

inline static void irda_concerto_nec_reset_decoder(irda_dec_result_t *p_result)
{
  p_result->state = E_NEC_STA_IDLE;
  p_result->counter = 0;
  p_result->r.nec.code = 0;
  p_result->r.nec.usercode = 0;
}

/*!
  IRDA NEC Decoder
*/
static void irda_concerto_nec_decoder(irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  u8 sig = irda_concerto_nec_decoder_parse_sig(p_para->ontime, p_para->period, p_result->p_sigtbl);
  IRDA_PRINT("\nnec irda_decoder sig=%s, state=%s\n", nec_sig_strs[sig],nec_sta_strs[p_result->state]);
  if(p_result->state == E_NEC_STA_IDLE)
  {
    if(sig == E_NEC_SIG_START)
    {
      p_result->counter = 0;
      p_result->state = E_NEC_STA_START;
    }
    else
    {
      p_result->counter = 0;
    }
  }
  else if(p_result->state == E_NEC_STA_START)
  {
    if(sig == E_NEC_SIG_0 || sig == E_NEC_SIG_1)
    {
      p_result->state = E_NEC_STA_USRL;
      if(sig == E_NEC_SIG_0)
      {
        p_result->r.nec.usercode &= ~(1 << p_result->counter);
      }
      else
      {
        p_result->r.nec.usercode |= (1 << p_result->counter);
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_USRL)
  {
    if(sig == E_NEC_SIG_0 || sig == E_NEC_SIG_1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.usercode &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.nec.usercode |= (1 << p_result->counter);
        }
      }
      else
      {
        p_result->counter = 0;
        p_result->state = E_NEC_STA_USRH;
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.usercode &= ~(1 << (p_result->counter + 8));
        }
        else
        {
          p_result->r.nec.usercode |= (1 << (p_result->counter + 8));
        }
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_USRH)
  {
    if(sig == E_NEC_SIG_0 || sig == E_NEC_SIG_1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.usercode &= ~(1 << (p_result->counter + 8));
        }
        else
        {
          p_result->r.nec.usercode |= (1 << (p_result->counter + 8));
        }
      }
      else
      {
        p_result->counter = 0;
        p_result->state = E_NEC_STA_KEY;
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.code &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.nec.code |= (1 << p_result->counter);
        }
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_KEY)
  {
    if(sig == E_NEC_SIG_0 || sig == E_NEC_SIG_1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.code &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.nec.code |= (1 << p_result->counter);
        }
      }
      else
      {
        p_result->counter = 0;
        p_result->state = E_NEC_STA_YEK;
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.code &= ~(1 << (p_result->counter + 8));
        }
        else
        {
          p_result->r.nec.code |= (1 << (p_result->counter + 8));
        }
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_YEK)
  {
    if(sig == E_NEC_SIG_0 || sig == E_NEC_SIG_1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_NEC_SIG_0)
        {
          p_result->r.nec.code &= ~(1 << (p_result->counter + 8));
        }
        else
        {
          p_result->r.nec.code |= (1 << (p_result->counter + 8));
        }
        if(p_result->counter == 7)
        {
          p_result->state = E_NEC_STA_FINISH;
        }
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_FINISH)
  {
    if(sig == E_NEC_SIG_START)
    {
      p_result->state = E_NEC_STA_START;
      p_result->counter = 0;
      p_result->r.nec.code = 0;
      p_result->r.nec.usercode = 0;
    }
    else if(sig == E_NEC_SIG_REPEAT)
    {
      p_result->counter = 0;
      if(p_result->repeat_interval == 0)
      {
        p_result->state = E_NEC_STA_FINISH;
      }
      else
      {
        p_result->state = E_NEC_STA_REPEAT;
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_NEC_STA_REPEAT)
  {
    if(sig == E_NEC_SIG_START)
    {
      p_result->state = E_NEC_STA_START;
      p_result->counter = 0;
      p_result->r.nec.code = 0;
      p_result->r.nec.usercode = 0;
    }
    else if(sig == E_NEC_SIG_REPEAT)
    {
      p_result->counter ++;
      if(p_result->counter >= p_result->repeat_interval)
      {
        p_result->state = E_NEC_STA_FINISH;
        p_result->counter = 0;
      }
    }
    else
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }

  if(p_result->state == E_NEC_STA_FINISH)
  {
    if(!irda_concerto_nec_decode_check_keycode(p_result->r.nec.code))
    {
      irda_concerto_nec_reset_decoder(p_result);
    }
  }
}/*IRDA NEC Decoder END*/


static u8 irda_concerto_pana_decoder_parse_sig(u16 ontime, u16 period, u16 * sigtbl)
{
  if((ontime >= sigtbl[0]) && (ontime <= sigtbl[1]) &&
     (period >= sigtbl[2]) && (period <= sigtbl[3]))
  {
    return E_PANA_SIG_START;
  }
  else if((ontime >= sigtbl[4]) && (ontime <= sigtbl[5]) &&
          (period >= sigtbl[6]) && (period <= sigtbl[7]))
  {
    return E_PANA_SIG_BIT0;
  }
  else if((ontime >= sigtbl[8]) && (ontime <= sigtbl[9]) &&
          (period >= sigtbl[10]) && (period <= sigtbl[11]))
  {
    return E_PANA_SIG_BIT1;
  }
  else
  {
    return E_PANA_SIG_INVALID;
  }
}

inline static void irda_concerto_pana_reset_decoder(irda_dec_result_t *p_result)
{
  p_result->state = E_PANA_STA_IDLE;
  p_result->counter = 0;
  p_result->r.pana.ucode = 0;
  p_result->r.pana.kcode = 0;
}

/*IRDA Pana Decoder */
static void irda_concerto_pana_decoder(irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  u8 sig = irda_concerto_pana_decoder_parse_sig(p_para->ontime, p_para->period, p_result->p_sigtbl);
  IRDA_PRINT("irda_decoder sig=%s, state=%s\n", pana_sig_strs[sig], pana_sta_strs[p_result->state]);
  if(p_result->state == E_PANA_STA_IDLE)
  {
    if(sig == E_PANA_SIG_START)
    {
      p_result->counter = 0;
      p_result->state = E_PANA_STA_START;
    }
    else
    {
      p_result->counter = 0;
    }
  }
  else if(p_result->state == E_PANA_STA_START)
  {
    if(sig == E_PANA_SIG_BIT0 || sig == E_PANA_SIG_BIT1)
    {
      p_result->state = E_PANA_STA_USR;
      p_result->counter = 0;
      if(sig == E_PANA_SIG_BIT0)
      {
        p_result->r.pana.ucode &= ~(1 << p_result->counter);
      }
      else
      {
        p_result->r.pana.ucode |= (1 << p_result->counter);
      }
    }
    else
    {
      irda_concerto_pana_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_PANA_STA_USR)
  {
    if(sig == E_PANA_SIG_BIT0 || sig == E_PANA_SIG_BIT1)
    {
      p_result->counter ++;
      if(p_result->counter < 32)
      {
        if(sig == E_PANA_SIG_BIT0)
        {
          p_result->r.pana.ucode &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.pana.ucode |= (1 << p_result->counter);
        }
      }
      else
      {
        p_result->state = E_PANA_STA_KEY;
        p_result->counter = 0;
        if(sig == E_PANA_SIG_BIT0)
        {
          p_result->r.pana.kcode &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.pana.kcode |= (1 << p_result->counter);
        }
      }
    }
    else
    {
      irda_concerto_pana_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_PANA_STA_KEY)
  {
    if(sig == E_PANA_SIG_BIT0 || sig == E_PANA_SIG_BIT1)
    {
      p_result->counter ++;
      if(p_result->counter < 16)
      {
        if(sig == E_PANA_SIG_BIT0)
        {
          p_result->r.pana.kcode &= ~(1 << p_result->counter);
        }
        else
        {
          p_result->r.pana.kcode |= (1 << p_result->counter);
        }
        if(p_result->counter == 15)
        {
          p_result->state = E_PANA_STA_FINISH;
          p_result->counter = 0;
        }
      }
    }
    else
    {
      irda_concerto_pana_reset_decoder(p_result);
    }
  }
}/*IRDA Pana Decoder END */




static u8 irda_concerto_konka_decoder_parse_sig(u16 ontime, u16 period, u16 * sigtbl)
{
  if((ontime >= sigtbl[0]) && (ontime <= sigtbl[1]) &&
      (period >= sigtbl[2]) && (period <= sigtbl[3]))
  {
    return E_KK_SIG_START;
  }
  else if((ontime >= sigtbl[4]) && (ontime <= sigtbl[5]) &&
             (period >= sigtbl[6]) && (period <= sigtbl[7]))
  {
    return E_KK_SIG_BIT0;
  }
  else if((ontime >= sigtbl[8]) && (ontime <= sigtbl[9]) &&
             (period >= sigtbl[10]) && (period <= sigtbl[11]))
  {
    return E_KK_SIG_BIT1;
  }
  else if((ontime >= sigtbl[12]) && (ontime <= sigtbl[13]) &&
             (period >= sigtbl[14]) && (period <= sigtbl[15]))
  {
    return E_KK_SIG_SYNC;
  }
  else
  {
    return E_KK_SIG_INVALID;
  }
}

inline static void irda_concerto_konka_reset_decoder(irda_dec_result_t *p_result)
{
  p_result->state = E_KK_STA_IDLE;
  p_result->counter = 0;
  p_result->r.konka.ucode = 0;
  p_result->r.konka.kcode = 0;
}

/*IRDA Konka Decoder */
static void irda_concerto_konka_decoder(irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  u8 sig = irda_concerto_konka_decoder_parse_sig(p_para->ontime, p_para->period, p_result->p_sigtbl);
  IRDA_PRINT("\nkonka irda_decoder sig=%s, state=%s\n", kk_sig_strs[sig], kk_sta_strs[p_result->state]);
  if(p_result->state == E_KK_STA_IDLE)
  {
    if(sig == E_KK_SIG_START)
    {
      p_result->counter = 0;
      p_result->state = E_KK_STA_START;
    }
    else
    {
      p_result->counter = 0;
    }
  }
  else if(p_result->state == E_KK_STA_START)
  {
    if(sig == E_KK_SIG_BIT0 || sig == E_KK_SIG_BIT1)
    {
      p_result->state = E_KK_STA_USR;
      p_result->counter = 0;
      if(sig == E_KK_SIG_BIT0)
      {
        p_result->r.konka.ucode &= ~(1 << (7-p_result->counter));
      }
      else
      {
        p_result->r.konka.ucode |= (1 << (7-p_result->counter));
      }
    }
    else
    {
      irda_concerto_konka_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_KK_STA_USR)
  {
    if(sig == E_KK_SIG_BIT0 || sig == E_KK_SIG_BIT1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_KK_SIG_BIT0)
        {
          p_result->r.konka.ucode &= ~(1 << (7-p_result->counter));
        }
        else
        {
          p_result->r.konka.ucode |= (1 << (7-p_result->counter));
        }
      }
      else
      {
        p_result->state = E_KK_STA_KEY;
        p_result->counter = 0;
        if(sig == E_KK_SIG_BIT0)
        {
          p_result->r.konka.kcode &= ~(1 << (7-p_result->counter));
        }
        else
        {
          p_result->r.konka.kcode |= (1 << (7-p_result->counter));
        }
      }
    }
    else
    {
      irda_concerto_konka_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_KK_STA_KEY)
  {
    if(sig == E_KK_SIG_BIT0 || sig == E_KK_SIG_BIT1)
    {
      p_result->counter ++;
      if(p_result->counter < 8)
      {
        if(sig == E_KK_SIG_BIT0)
        {
          p_result->r.konka.kcode &= ~(1 << (7-p_result->counter));
        }
        else
        {
          p_result->r.konka.kcode |= (1 << (7-p_result->counter));
        }
        if(p_result->counter == 7)
        {
          p_result->state = E_KK_STA_SYNC;
          p_result->counter = 0;
        }
      }
    }
    else
    {
      irda_concerto_konka_reset_decoder(p_result);
    }
  }
  else if(p_result->state == E_KK_STA_SYNC)
  {
    if(sig == E_KK_SIG_SYNC)
    {
      p_result->state = E_KK_STA_FINISH;
    }
    else
    {
      irda_concerto_konka_reset_decoder(p_result);
    }
  }
}/*IRDA Konka Decoder END */

/*IRDA Soft Decoder*/
static RET_CODE irda_concerto_decoder(irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  if(p_para == NULL || p_result == NULL)
  {
    return ERR_PARAM;
  }

  switch(p_para->protocol)
  {
    case IRDA_NEC:
      return ERR_NOFEATURE;

    case IRDA_SW_NEC:
      irda_concerto_nec_decoder(p_para, p_result);
      return SUCCESS;

    case IRDA_SW_KONKA:
      irda_concerto_konka_decoder(p_para, p_result);
      return SUCCESS;
    case IRDA_SW_PANA:
      //mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
      irda_concerto_pana_decoder(p_para, p_result);
      return SUCCESS;
    default:
      return ERR_NOFEATURE;
    }
}/*IRDA Soft Decoder END*/

/*!
  IRDA RC5 decoder
*/
static void irda_concerto_rc5_decoder(uio_priv_t *p_priv,
  irda_priv_t *p_irda, irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  u16 overtime = ((*((volatile u32 *)R_IR_GLOBAL_CTRL) >> 16) & 0xFF) << 4;
  u16 recv_num = 0;
  u16 irda_code = 0;

  static u8 bit_val[14] = { 0 };
  static u8 half_bit1 = 0;
  static s8 bit_T = -1;

  recv_num = irda_concerto_get_recv_num();
  while(recv_num)
  {
    irda_concerto_get_ontime_and_period(&p_para->ontime, &p_para->period);
    IRDA_PRINT("(%d)#ontime:%d, period:%d\n", recv_num, p_para->ontime, p_para->period);

    if ((p_para->ontime >= p_result->p_sigtbl[0] && p_para->ontime <= p_result->p_sigtbl[1]) &&
          (p_para->period >= p_result->p_sigtbl[2] && p_para->period <= p_result->p_sigtbl[3]))
    {
      if (p_result->state <= E_RC5_STA_S2)
      {
        if (p_result->state > E_RC5_STA_IDLE)
        {
          p_result->counter ++;
        }
        bit_val[p_result->counter] = 1;
        half_bit1 = 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        p_result->state ++;
      }
      else
      {
        bit_val[++ p_result->counter] = half_bit1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_T) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
        {
          p_result->state = E_RC5_STA_INVALID;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[2] && p_para->ontime <= p_result->p_sigtbl[3]) &&
                 (p_para->period >= p_result->p_sigtbl[4] && p_para->period <= p_result->p_sigtbl[5]))
    {
      if (!half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 1;
        half_bit1 = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }

        bit_val[++ p_result->counter] = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
        {
          p_result->state = E_RC5_STA_INVALID;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[0] && p_para->ontime <= p_result->p_sigtbl[1]) &&
                 (p_para->period >= p_result->p_sigtbl[4] && p_para->period <= p_result->p_sigtbl[5]))
    {
      if (half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 0;
        half_bit1= 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_T) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
        {
          p_result->state = E_RC5_STA_INVALID;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[2] && p_para->ontime <= p_result->p_sigtbl[3]) &&
                 (p_para->period >= p_result->p_sigtbl[6] && p_para->period <= p_result->p_sigtbl[7]))
    {
      if (!half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 1;
        half_bit1 = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
       	}

        bit_val[++ p_result->counter] = 0;
        half_bit1= 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_S2) ||
             (p_result->state == E_RC5_STA_T) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
        {
          p_result->state = E_RC5_STA_INVALID;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[0] && p_para->ontime <= p_result->p_sigtbl[1]) &&
                (p_para->period >= (p_result->p_sigtbl[0] + overtime) && p_para->period <= (p_result->p_sigtbl[1] + overtime)))
    {
      bit_val[++ p_result->counter] = half_bit1;
      if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
      {
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        p_result->state = E_RC5_STA_FINISH;
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[2] && p_para->ontime <= p_result->p_sigtbl[3]) &&
                (p_para->period >= (p_result->p_sigtbl[2] + overtime) && p_para->period <= (p_result->p_sigtbl[3] + overtime)))
    {
      bit_val[++ p_result->counter] = half_bit1;
      half_bit1 = 0;

      IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
        __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);

      bit_val[++ p_result->counter] = 0;
      if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 13))
      {
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        p_result->state = E_RC5_STA_FINISH;
      }
    }
    else
    {
      p_result->state = E_RC5_STA_INVALID;
    }

    recv_num --;

    if (p_result->state == E_RC5_STA_FINISH)
    {
      p_result->r.rc5.ucode = 0;
      p_result->r.rc5.ucode |= (bit_val[3] & 0x1) << 4;
      p_result->r.rc5.ucode |= (bit_val[4] & 0x1) << 3;
      p_result->r.rc5.ucode |= (bit_val[5] & 0x1) << 2;
      p_result->r.rc5.ucode |= (bit_val[6] & 0x1) << 1;
      p_result->r.rc5.ucode |= bit_val[7] & 0x1;

      p_result->r.rc5.kcode = 0;
      p_result->r.rc5.kcode |= (bit_val[8] & 0x1) << 5;
      p_result->r.rc5.kcode |= (bit_val[9] & 0x1) << 4;
      p_result->r.rc5.kcode |= (bit_val[10] & 0x1) << 3;
      p_result->r.rc5.kcode |= (bit_val[11] & 0x1) << 2;
      p_result->r.rc5.kcode |= (bit_val[12] & 0x1) << 1;
      p_result->r.rc5.kcode |= bit_val[13] & 0x1;

      p_irda->user_code = p_result->r.rc5.ucode;
      irda_code = p_result->r.rc5.kcode | (UIO_IRDA << 8);
      if (bit_T == bit_val[2])
      {
        irda_code |= 1 << 15;
      }
      else
      {
        bit_T = bit_val[2];
      }

      OS_PRINTF("   user code: %08x\n", p_result->r.rc5.ucode);
      OS_PRINTF("   irda code: %08x, %x\n", p_result->r.rc5.kcode, irda_code);

      mtos_fifo_put(&p_priv->fifo, irda_code);

      half_bit1 = 0;
      p_result->counter = 0;
      memset(bit_val, 0x0, 14);
      p_result->state = E_RC5_STA_IDLE;
    }
    else if (p_result->state == E_RC5_STA_INVALID)
    {
      break;
    }
  }

  if (p_result->state == E_RC5_STA_INVALID)
  {
    half_bit1 = 0;
    p_result->counter = 0;
    memset(bit_val, 0, 14);
    p_result->state = E_RC5_STA_IDLE;

    while(recv_num)
    {
      irda_concerto_get_ontime_and_period(&p_para->ontime, &p_para->period);
      recv_num --;
    }
  }
}

/*!
  IRDA RC5 decoder handling tail bit(s) loss
*/
static void irda_concerto_rc5_decoder_bit_loss(uio_priv_t *p_priv,
  irda_priv_t *p_irda, irda_dec_para_t *p_para, irda_dec_result_t *p_result)
{
  u32 interrupt_interval = 0;
  u16 recv_num = 0;
  u16 irda_code = 0;

  static u32 hw_ticks_start = 0;
  static u8 bit_val[14] = { 0 };
  static u8 half_bit1 = 0;
  static s8 bit_T = -1;

  static u8 prev_lost_2bits = 0;
  static u8 prev_usrcode = 0;
  static u8 prev_keycode = 0;
  static s8 prev_bit_T = -1;

  interrupt_interval = mtos_hw_ticks_get() - hw_ticks_start;
  if ((mtos_hw_ticks_get() - hw_ticks_start) > 16000000)
  {
    IRDA_PRINT("interrput interval: %d\n", interrupt_interval);
    if ((p_result->state== E_RC5_STA_KEY) && (p_result->counter== 11) && half_bit1)
    {
      prev_lost_2bits = 1;
      prev_bit_T = bit_val[2];

      prev_usrcode = 0;
      prev_usrcode |= (bit_val[3] & 0x1) << 4;
      prev_usrcode |= (bit_val[4] & 0x1) << 3;
      prev_usrcode |= (bit_val[5] & 0x1) << 2;
      prev_usrcode |= (bit_val[6] & 0x1) << 1;
      prev_usrcode |= bit_val[7] & 0x1;
  
      prev_keycode = 0;
      prev_keycode |= (bit_val[8] & 0x1) << 5;
      prev_keycode |= (bit_val[9] & 0x1) << 4;
      prev_keycode |= (bit_val[10] & 0x1) << 3;
      prev_keycode |= (bit_val[11] & 0x1) << 2;
      prev_keycode |= 0x2;
    }

    half_bit1 = 0;
    p_result->counter = 0;
    memset(bit_val, 0, 14);
    p_result->state = E_RC5_STA_IDLE;
  }
  hw_ticks_start = mtos_hw_ticks_get();

  recv_num = irda_concerto_get_recv_num();
  while(recv_num)
  {
    irda_concerto_get_ontime_and_period(&p_para->ontime, &p_para->period);
    IRDA_PRINT("(%d)#ontime:%d, period:%d\n", recv_num, p_para->ontime, p_para->period);

    if ((p_para->ontime >= p_result->p_sigtbl[0] && p_para->ontime <= p_result->p_sigtbl[1]) &&
          (p_para->period >= p_result->p_sigtbl[2] && p_para->period <= p_result->p_sigtbl[3]))
    {
      if (p_result->state <= E_RC5_STA_S2)
      {
        if (p_result->state > E_RC5_STA_IDLE)
        {
          p_result->counter ++;
        }
        bit_val[p_result->counter] = 1;
        half_bit1 = 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        p_result->state ++;
      }
      else
      {
        bit_val[++ p_result->counter] = half_bit1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_T) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 12))
        {
          bit_val[++ p_result->counter] = half_bit1;
          half_bit1 = 0;
          IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
            __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
          p_result->state = E_RC5_STA_FINISH;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[2] && p_para->ontime <= p_result->p_sigtbl[3]) &&
                 (p_para->period >= p_result->p_sigtbl[4] && p_para->period <= p_result->p_sigtbl[5]))
    {
      if (!half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 1;
        half_bit1 = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }

        bit_val[++ p_result->counter] = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 12))
        {
          bit_val[++ p_result->counter] = 0;
          IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
            __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
          p_result->state = E_RC5_STA_FINISH;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[0] && p_para->ontime <= p_result->p_sigtbl[1]) &&
                 (p_para->period >= p_result->p_sigtbl[4] && p_para->period <= p_result->p_sigtbl[5]))
    {
      if (half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 0;
        half_bit1= 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_T) ||
            ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 12))
        {
          bit_val[++ p_result->counter] = 1;
          half_bit1 = 0;
          IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
            __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
          p_result->state = E_RC5_STA_FINISH;
        }
      }
    }
    else if ((p_para->ontime >= p_result->p_sigtbl[2] && p_para->ontime <= p_result->p_sigtbl[3]) &&
                 (p_para->period >= p_result->p_sigtbl[6] && p_para->period <= p_result->p_sigtbl[7]))
    {
      if (!half_bit1)
      {
        p_result->state = E_RC5_STA_INVALID;
      }
      else
      {
        bit_val[++ p_result->counter] = 1;
        half_bit1 = 0;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if (((p_result->state >= E_RC5_STA_S1) && (p_result->state <= E_RC5_STA_T)) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }

        bit_val[++ p_result->counter] = 0;
        half_bit1= 1;
        IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
          __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
        if ((p_result->state == E_RC5_STA_S2) ||
             (p_result->state == E_RC5_STA_T) ||
             ((p_result->state == E_RC5_STA_USR) && (p_result->counter >= 8)))
        {
          p_result->state ++;
        }
        else if ((p_result->state == E_RC5_STA_KEY) && (p_result->counter == 12))
        {
          bit_val[++ p_result->counter] = half_bit1;
          half_bit1 = 0;
          IRDA_PRINT("line: %d, state: %d, bit_val[%d]: %d, half_bit1: %d\n",
            __LINE__, p_result->state, p_result->counter, bit_val[p_result->counter], half_bit1);
          p_result->state = E_RC5_STA_FINISH;
        }
      }
    }
    else
    {
      p_result->state = E_RC5_STA_INVALID;
    }

    recv_num --;

    if ((p_result->state == E_RC5_STA_FINISH) || (p_result->state == E_RC5_STA_INVALID))
    {
      break;
    }
  }

  if (p_result->state <= E_RC5_STA_FINISH)
  {
    if (prev_lost_2bits && half_bit1 && (prev_bit_T == bit_val[2]))
    {
      p_irda->user_code = prev_usrcode;
      irda_code = prev_keycode | (UIO_IRDA << 8);
      if (prev_bit_T == bit_T)
      {
        irda_code |= 1 << 15;
      }
      else
      {
        bit_T = prev_bit_T;
      }

      OS_PRINTF("   user code: %08x\n", prev_usrcode);
      OS_PRINTF("   irda code: %08x, %x\n", prev_keycode, irda_code);

      mtos_fifo_put(&p_priv->fifo, irda_code);

      prev_lost_2bits = 0;
      prev_bit_T = -1;
      prev_usrcode = 0;
      prev_keycode = 0;
    }
  }

  switch (p_result->state)
  {
    case E_RC5_STA_FINISH:
      p_result->r.rc5.ucode = 0;
      p_result->r.rc5.ucode |= (bit_val[3] & 0x1) << 4;
      p_result->r.rc5.ucode |= (bit_val[4] & 0x1) << 3;
      p_result->r.rc5.ucode |= (bit_val[5] & 0x1) << 2;
      p_result->r.rc5.ucode |= (bit_val[6] & 0x1) << 1;
      p_result->r.rc5.ucode |= bit_val[7] & 0x1;
  
      p_result->r.rc5.kcode = 0;
      p_result->r.rc5.kcode |= (bit_val[8] & 0x1) << 5;
      p_result->r.rc5.kcode |= (bit_val[9] & 0x1) << 4;
      p_result->r.rc5.kcode |= (bit_val[10] & 0x1) << 3;
      p_result->r.rc5.kcode |= (bit_val[11] & 0x1) << 2;
      p_result->r.rc5.kcode |= (bit_val[12] & 0x1) << 1;
      p_result->r.rc5.kcode |= bit_val[13] & 0x1;

      p_irda->user_code = p_result->r.rc5.ucode;
      irda_code = p_result->r.rc5.kcode | (UIO_IRDA << 8);
      if (bit_T == bit_val[2])
      {
        irda_code |= 1 << 15;
      }
      else
      {
        bit_T = bit_val[2];
      }

      OS_PRINTF("   user code: %08x\n", p_result->r.rc5.ucode);
      OS_PRINTF("   irda code: %08x, %x\n", p_result->r.rc5.kcode, irda_code);

      mtos_fifo_put(&p_priv->fifo, irda_code);

    case E_RC5_STA_INVALID:
      prev_lost_2bits = 0;
      prev_bit_T = -1;
      prev_usrcode = 0;
      prev_keycode = 0;

      half_bit1 = 0;
      p_result->counter = 0;
      memset(bit_val, 0, 14);
      p_result->state = E_RC5_STA_IDLE;

      while(recv_num)
      {
        irda_concerto_get_ontime_and_period(&p_para->ontime, &p_para->period);
        recv_num --;
      }
      break;

    default:
      break;
  }
}

static void irda_concerto_rc5_wfilt_set(u8 *bit_val, u8 * p_channel_len)
{
  u16 overtime = ((*((volatile u32 *)R_IR_GLOBAL_CTRL) >> 16) & 0xFF) << 4;
  u8 j = 0, k = 0;

  while (j <= 13)
  {
    if (j == 13)
    {
      irda_concerto_wave_wfn_set(k, rc5_sigtbl[0], rc5_sigtbl[1], rc5_sigtbl[0] + overtime, rc5_sigtbl[1] + overtime);
      IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
      j ++;
    }
    else if ((bit_val[j] && bit_val[j+1]) || (!bit_val[j] && !bit_val[j+1])) // 11b or 00b
    {
      if (j < 13)
      {
        irda_concerto_wave_wfn_set(k, rc5_sigtbl[0], rc5_sigtbl[1], rc5_sigtbl[2], rc5_sigtbl[3]);
        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
        j ++;
      }
    }
    else if (bit_val[j] && !bit_val[j+1]) // 10b
    {
      if (j < 12)
      {
        if (bit_val[j+2]) // 101b
          irda_concerto_wave_wfn_set(k, rc5_sigtbl[2], rc5_sigtbl[3], rc5_sigtbl[6], rc5_sigtbl[7]);
        else if (!bit_val[j+2]) // 100b
          irda_concerto_wave_wfn_set(k, rc5_sigtbl[2], rc5_sigtbl[3], rc5_sigtbl[4], rc5_sigtbl[5]);

        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j + 1, bit_val[j+1]);
        j += 2;
      }
      else if (j == 12) // 10b
      {
        irda_concerto_wave_wfn_set(k, rc5_sigtbl[2], rc5_sigtbl[3], rc5_sigtbl[2] + overtime, rc5_sigtbl[3] + overtime);

        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j + 1, bit_val[j+1]);
        j += 2;
      }
    }
    else if (!bit_val[j] && bit_val[j+1]) // 01b
    {
      irda_concerto_wave_wfn_set(k, rc5_sigtbl[0], rc5_sigtbl[1], rc5_sigtbl[4], rc5_sigtbl[5]);
      IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
      j ++;
    }

    irda_concerto_wave_mask_bit_set(k, 1);

    k++;
  }

#if RC5_WF_IGNORE_T_BIT
  // ignore samples relevant to T bit
  irda_concerto_wave_mask_bit_set(1, 0);
  irda_concerto_wave_mask_bit_set(2, 0);
#endif

  *p_channel_len = k;
}

static void irda_concerto_rc5_wfilt_set_bit_loss(u8 *bit_val, u8 * p_channel_len)
{
  u8 j = 0, k = 0;

  while (j <= 12)
  {
    if ((bit_val[j] && bit_val[j+1]) || (!bit_val[j] && !bit_val[j+1])) // 11b or 00b
    {
      irda_concerto_wave_wfn_set(k, rc5_sigtbl[0], rc5_sigtbl[1], rc5_sigtbl[2], rc5_sigtbl[3]);
      IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
      j ++;
    }
    else if (bit_val[j] && !bit_val[j+1]) // 10b
    {
      if (j < 12)
      {
        if (bit_val[j+2]) // 101b
          irda_concerto_wave_wfn_set(k, rc5_sigtbl[2], rc5_sigtbl[3], rc5_sigtbl[6], rc5_sigtbl[7]);
        else if (!bit_val[j+2]) // 100b
          irda_concerto_wave_wfn_set(k, rc5_sigtbl[2], rc5_sigtbl[3], rc5_sigtbl[4], rc5_sigtbl[5]);

        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
        IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j + 1, bit_val[j+1]);

        j += 2;
      }
      else if (j == 12)
        break;
    }
    else if (!bit_val[j] && bit_val[j+1]) // 01b
    {
      irda_concerto_wave_wfn_set(k, rc5_sigtbl[0], rc5_sigtbl[1], rc5_sigtbl[4], rc5_sigtbl[5]);
      IRDA_PRINT("%d, bit_val[%d]: %d\n", __LINE__, j, bit_val[j]);
      j ++;
    }

    irda_concerto_wave_mask_bit_set(k, 1);

    k++;
  }

#if RC5_WF_IGNORE_T_BIT
  // ignore samples relevant to T bit
  irda_concerto_wave_mask_bit_set(1, 0);
  irda_concerto_wave_mask_bit_set(2, 0);
#endif

  *p_channel_len = k;
}

static RET_CODE irda_concerto_set_wfilt(irda_priv_t *p_ir)
{
  u8 i = 0;
  u8 j = 0;
  u8 channel_num = 0;
  u8 wfilt_add_len = 0;
  u8 channel_len = 0;
  u8 irda_protocol = 0;
  u32 code = 0;
  u32 mask_code = 0;

  OS_PRINTF("enter wflit\n");
  irda_concerto_soft_reset();
  irda_concerto_idle_set(1);
#if DEFAULT_SAMPLE_FREQ //  Sample freq 187.5 KHz
  irda_concerto_sample_clk_set(2);
#else //  Sample freq 46.875 KHz
  irda_concerto_sample_clk_set(3);
#endif

  irda_concerto_wave_channel_set(0xf, FALSE);
  irda_concerto_wave_channel_int_set(0xf , FALSE);
  channel_num = p_ir->irda_wfilt_channel;
  switch(channel_num)
  {
    case 4:
      irda_concerto_wave_channel_set(0xf, TRUE);
      irda_concerto_wave_channel_int_set(0xf , TRUE);
    case 3:
      irda_concerto_wave_channel_set(0x7, TRUE);
      irda_concerto_wave_channel_int_set(0x7, TRUE);
    case 2:
      irda_concerto_wave_channel_set(0x3, TRUE);
      irda_concerto_wave_channel_int_set(0x3, TRUE);
    case 1:
      irda_concerto_wave_channel_set(0x1, TRUE);
      irda_concerto_wave_channel_int_set(0x1, TRUE);
      break;
    default : 
      return ERR_PARAM;
  }

  for(i = 0; i < channel_num; i++)
  {
    channel_len = p_ir->irda_wfilt_channel_cfg[i].addr_len;
    irda_concerto_wave_add_set(i, wfilt_add_len);
    irda_protocol = p_ir->irda_wfilt_channel_cfg[i].protocol;
    if ((irda_protocol == IRDA_NEC) || (irda_protocol == IRDA_SW_NEC))
    { /* IrDA NEC protocol */ 
      mask_code = p_ir->irda_wfilt_channel_cfg[i].wfilt_mask;
      code = p_ir->irda_wfilt_channel_cfg[i].wfilt_code;
      // 16 bit usercode | 8 bit keycode | 8 bit reversed keycode
      // --->
      // 8 bit reversed keycode | 8 bit keycode | 16 bit usercode
      code = ((code & 0xFFFF0000) >> 16) |
		     ((code & 0xFF00) << 8) |
             ((code & 0xFF) << 24);

      for(j = 0; j < channel_len; j++)
      {
        if(mask_code == 0)
        {
          irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
        }
        else
        {
          if((mask_code >> j) & 0x1)
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
          }
          else
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 0);
          }
        }
        if((code >> j) & 0x1) // Timing of NEC bit 1 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            nec_sigtbl[8], nec_sigtbl[9], nec_sigtbl[10], nec_sigtbl[11]);
        }
        else // Timing of NEC bit 0 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            nec_sigtbl[4], nec_sigtbl[5], nec_sigtbl[6], nec_sigtbl[7]);
        }
      }
      wfilt_add_len += channel_len;
    }
    else if (irda_protocol == IRDA_SW_KONKA)
    { /* IrDA Konka protocol */
      mask_code = p_ir->irda_wfilt_channel_cfg[i].wfilt_mask;
      code = p_ir->irda_wfilt_channel_cfg[i].wfilt_code;
      for(j = 0; j < channel_len; j++)
      {
        if(mask_code == 0)
        {
          irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
        }
        else
        {
          if((mask_code << j) & 0x8000)
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
          }
          else
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 0);
          }
        }
        if((code << j) & 0x8000) // Timing of Konka bit 1 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            konka_sigtbl[8], konka_sigtbl[9], konka_sigtbl[10], konka_sigtbl[11]);
        }
        else // Timing of Konka bit 0 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            konka_sigtbl[4], konka_sigtbl[5], konka_sigtbl[6], konka_sigtbl[7]);
        }
      }
      wfilt_add_len += channel_len;
    }
    else if (irda_protocol == IRDA_SW_RC5)
    { /* IrDA RC5 protocol */
      u8 ucode = 0;
      u8 bit_val[14] = { 0 };
      code = p_ir->irda_wfilt_channel_cfg[i].wfilt_code;
      ucode = (code >> 8) & 0x1F;
      code &= 0x3F;
      code |= (ucode << 6) | (0x3 << 12);
      for (j = 13; j > 0; j --)
      {
        bit_val[13 - j] = (code >> j) & 0x1;
      }
#if !RC5_WF_IGNORE_T_BIT
      bit_val[2] = i & 0x1; // the 3rd bit T
#endif

      if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))
      {
        irda_concerto_rc5_wfilt_set(bit_val, &channel_len);
      }
      else
      {
        irda_concerto_rc5_wfilt_set_bit_loss(bit_val, &channel_len);
      }
      wfilt_add_len += channel_len;
    }
     else if (irda_protocol == IRDA_SW_PANA)
    { /* IrDA Pana protocol */

      mask_code = p_ir->irda_wfilt_channel_cfg[i].wfilt_mask;
      code = p_ir->irda_wfilt_channel_cfg[i].wfilt_code;

      for(j = 0; j < channel_len; j++)
      {
        if(mask_code == 0)
        {
          irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
        }
        else
        {
          if((mask_code >> j) & 0x1)
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 1);
          }
          else
          {
            irda_concerto_wave_mask_bit_set(j + wfilt_add_len, 0);
          }
        }
        if((code >> j) & 0x1) // Timing of Pana bit 1 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            pana_sigtbl[8], pana_sigtbl[9], pana_sigtbl[10], pana_sigtbl[11]);
        }
        else // Timing of Pana bit 0 code
        {
          irda_concerto_wave_wfn_set(j + wfilt_add_len,
            pana_sigtbl[4], pana_sigtbl[5], pana_sigtbl[6], pana_sigtbl[7]);
        }
      }
      
      wfilt_add_len += channel_len;

    }
    irda_concerto_wave_len_set(i, channel_len - 1);

    OS_PRINTF("channel len: %d, wflit_add_len: %d\n", channel_len, wfilt_add_len);
    OS_PRINTF("wfilt code: 0x%x\n", code);
  }

  return SUCCESS;
}

void irda_concerto_glb_rst(void)
{
  u32 ptmp = 0;
  u32 dtmp = 0;
  u8 reset_bit = 0;

  ptmp = R_RST_REQ_AO;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp |= 1 << reset_bit;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_RST_ALLOW_AO;
  do
  {
    dtmp = hal_get_u32((volatile u32 *)ptmp);
  }
  while (!((dtmp >> reset_bit) & 0x1));

  ptmp = R_RST_CTRL_AO;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~ (1 << reset_bit);
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  dtmp |= 1 << reset_bit;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_RST_REQ_AO;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~ (1 << reset_bit);
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

void irda_concerto_soft_reset(void)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  u8 loop = 0xFF;

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~0x1;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  while (loop--);

  dtmp |= 0x1;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_wave_channel_set(u8 channel, u8 is_en)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

 // chn_bit = 24 + channel;

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  if (is_en)
  {
  //  dtmp |= 1<<chn_bit;
   dtmp |= channel << 24;
  }
  else
  {
    //dtmp &= ~(1<<chn_bit);
    dtmp &= ~(channel << 24);
  }

  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_wave_channel_int_set(u8 channel_int, u8 is_en)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  ptmp = R_IR_INT_CFG;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  if (is_en)
  {
   dtmp |= channel_int << 4;
  }
  else
  {
    dtmp &= ~(channel_int << 4);
  }
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_wave_mask_bit_set(u32 bit, u32 value)
{
  u8 position = 0;
  u8 field = 0;
  u32 dtmp = 0;
  u32 ptmp = 0;

  position = bit / 32;
  field = bit % 32;

  ptmp = GET_R_IR_WAVEFILT_MASK(position);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~(1 << field);
  dtmp |=(value << field);
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_wave_len_set(u8 channel, u8 len)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  switch (channel)
  {
    case 0:
      ptmp = R_IR_WAVEFILT_CFG0;
      break;
    case 1:
      ptmp = R_IR_WAVEFILT_CFG1;
      break;
    case 2:
      ptmp = R_IR_WAVEFILT_CFG2;
      break;
    case 3:
      ptmp = R_IR_WAVEFILT_CFG3;
      break;
    default:
      break;
  }

  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0xFFFF80FF;
  dtmp |= len << 8;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_wave_add_set(u8 channel, u8 addr)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  switch (channel)
  {
    case 0:
      ptmp = R_IR_WAVEFILT_CFG0;
      break;
    case 1:
      ptmp = R_IR_WAVEFILT_CFG1;
      break;
    case 2:
      ptmp = R_IR_WAVEFILT_CFG2;
      break;
    case 3:
      ptmp = R_IR_WAVEFILT_CFG3;
      break;
    default:
      break;
  }

  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0xFFFFFF80;
  dtmp |= addr;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

 static void irda_concerto_wave_wfn_set(u8 n, u16 ontime_l, u16 ontime_h, u16 period_l, u16 period_h)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  IRDA_PRINT("%s, %d, n: %d, ontime_l: %d, ontime_h: %d, period_l: %d, period_h: %d\n",
    __FUNCTION__, __LINE__, n, ontime_l, ontime_h, period_l, period_h);

  ptmp = GET_R_IR_WFN_ONTSET(n);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0x0;
  dtmp |= (ontime_h << 16) | ontime_l;
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = GET_R_IR_WFN_PRDSET(n);
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0x0;
  dtmp |= (period_h << 16) | period_l;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_idle_set(u8 idle)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  if (idle)
  {
    dtmp |= 0x4;	// Set idle state as High
  }
  else
  {
    dtmp &= ~0x4;	// Set idle state as Low
  }

  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_sample_clk_set(u8 sample_clk)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  switch (sample_clk)
  {
    case 0: // 3MHz
      dtmp &= 0xFFFFFF0F;
      break;
    case 1: // 750KHz
      dtmp &= 0xFFFFFF0F;
      dtmp |= 0x10;
      break;
    case 2:	// 187.5HKz (mode 0 default value)
      dtmp &= 0xFFFFFF0F;
      dtmp |= 0x20;
      break;
    case 3:	// 46.875KHz
       dtmp &= 0xFFFFFF0F;
       dtmp |= 0x30;
       break;
    default:
       break;
  }

  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static void irda_concerto_int_handler(void)
{
  u32 ucode=0;
  u16 kcode=0;
  u16 irda_code = 0;
  u8 i = 0, irh = 0;

  uio_device_t *p_dev = (uio_device_t *)dev_find_identifier(NULL,
                                        DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  lld_uio_t *p_lld = (lld_uio_t *) p_dev->p_priv;
  uio_priv_t *p_priv = (uio_priv_t *)p_lld->p_priv;
  irda_priv_t *p_irda = (irda_priv_t *) p_lld->p_irda;

#if 0
  {
    u8 wave_filt = (*((volatile u32 *)R_IR_INT_RAWSTA) >> 4) & 0xF;
    if (wave_filt)
      OS_PRINTF("%s, %d, wave_filt: 0x%x\n", __FUNCTION__, __LINE__, wave_filt);
  }
#endif

  p_irda->irda_flag = 1;

  if(p_irda->irda_protocol == IRDA_NEC)  /* hardware decoding */
  {
    if (IRDA_CONCERTO_CHK_KDNSTA()) /*key down*/
    {
      u32 ptmp = 0;
      u32 dtmp = 0;

      ptmp = R_IR_NEC_DATA;
      dtmp = hal_get_u32((volatile u32 *)ptmp);

      ucode = dtmp & 0xFFFF;
      kcode = (dtmp >> 16) & 0xFF;
    }
    if (IRDA_CONCERTO_CHK_KUPSTA()) /*key up*/
    {
      IRDA_PRINT("key up interrupt\n");
      return;
    }
  }
  else /* software decoding */
  {
    u16 ontime=0;
    u16 period=0;
    u16 recv_num = 0;

    static irda_dec_para_t dec_para = { 0 };
    static irda_dec_result_t dec_result = {{{0}}};

    if (IRDA_CONCERTO_CHK_OVERFLOW())
    {  // buffer overflow
      IRDA_PRINT("IRDA buffer overflow\n");
      irda_concerto_soft_reset();
      return;
    }

    if(!is_ir_intialized)
    {
      is_ir_intialized = TRUE;
      if (p_irda->irda_protocol == IRDA_SW_NEC)
      {
        dec_result.state = E_NEC_STA_IDLE;
        dec_result.p_sigtbl = nec_sigtbl;
        dec_result.repeat_interval = 2;
      }
      else if (p_irda->irda_protocol == IRDA_SW_KONKA)
      {
        dec_result.state = E_KK_STA_IDLE;
        dec_result.p_sigtbl = konka_sigtbl;
      }
      else if (p_irda->irda_protocol == IRDA_SW_RC5)
      {
        dec_result.state = E_RC5_STA_IDLE;
        dec_result.p_sigtbl = rc5_sigtbl;
      }
      else if (p_irda->irda_protocol == IRDA_SW_PANA)
      {
        dec_result.state = E_PANA_STA_IDLE;
        dec_result.p_sigtbl = pana_sigtbl;
      }
    }

    dec_para.protocol = p_irda->irda_protocol;

    if (dec_para.protocol == IRDA_SW_RC5)
    {
      if ((hal_get_chip_rev() >= CHIP_CONCERTO_B0))
      {
        irda_concerto_rc5_decoder(p_priv, p_irda, &dec_para, &dec_result);
      }
      else
      {
        irda_concerto_rc5_decoder_bit_loss(p_priv, p_irda, &dec_para, &dec_result);
      }

      return;
    }

    recv_num = irda_concerto_get_recv_num();
    while(recv_num)
    {
      irda_concerto_get_ontime_and_period(&ontime, &period);
      IRDA_PRINT("(%d)#ontime:%d, period:%d\n", recv_num, ontime, period);

      dec_para.ontime = ontime;
      dec_para.period = period;
      irda_concerto_decoder(&dec_para, &dec_result);
      if ((p_irda->irda_protocol == IRDA_SW_NEC) &&
	      (dec_result.state == E_NEC_STA_FINISH))
      {
        kcode = dec_result.r.nec.code & 0xff;
        ucode = dec_result.r.nec.usercode;
        break;
      }
      else if ((p_irda->irda_protocol == IRDA_SW_KONKA) &&
               (dec_result.state == E_KK_STA_FINISH))
      {
        kcode = dec_result.r.konka.kcode;
        ucode = dec_result.r.konka.ucode;
        break;
      }
       else if ((p_irda->irda_protocol == IRDA_SW_PANA) &&
               (dec_result.state == E_PANA_STA_FINISH))
      {
        mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
        kcode = dec_result.r.pana.kcode;
        ucode = dec_result.r.pana.ucode;
	mtos_printk("\n kcode:%x,ucode:%x",kcode,ucode);
        break;
      }
      recv_num --;
    }

    while(recv_num)
    {
      irda_concerto_get_ontime_and_period(&ontime, &period);
      recv_num --;
    }

    if (p_irda->irda_protocol == IRDA_SW_NEC)
    {
      if (dec_result.state != E_NEC_STA_FINISH)
      {
        return;
      }
      else
      {
        // Keep E_NEC_STA_FINISH state for repeated keycode
      }
    }
    else if (p_irda->irda_protocol == IRDA_SW_KONKA)
    {
      if (dec_result.state != E_KK_STA_FINISH)
      {
        return;
      }
      else
      {
        dec_result.state = E_KK_STA_IDLE;
      }
    }
     else if (p_irda->irda_protocol == IRDA_SW_PANA)
    {
       //mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
      if (dec_result.state != E_PANA_STA_FINISH)
      {
        return;
      }
      else
      {
        dec_result.state = E_PANA_STA_IDLE;
      }
    }
  }
//mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
#if 1
  if(TRUE == p_irda->user_code_set)
  {
    if (p_irda->print_user_code)
    {
      OS_PRINTF("\nReceive irda data code %d", kcode);
      OS_PRINTF("   user code high: %d, user code low: %d\n",
                  ((ucode & 0xFF00) >> 8), (ucode & 0xFF));
    }

    for(i = 0; i < IRDA_MAX_USER; i++)
    {
      if(p_irda->sw_user_code[i] == ucode)
      {
        break;
      }
    }
    if(i >= IRDA_MAX_USER)
    {
      return;
    }
  }
  #endif
//mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
  if(IRDA_CONCERTO_CHK_RPTKEY())
  {
    if(FALSE == uio_check_rpt_key_concerto(UIO_IRDA, kcode, i))
    {
      return;
    }
  }

  irh = UIO_IRDA | ((i) << UIO_USR_CODE_SHIFT);
  irda_code = kcode | (irh << 8);
  if (p_irda->irda_protocol == IRDA_NEC)
  {
    if(IRDA_CONCERTO_CHK_RPTKEY())
    {
      irda_code |= (1 << 15);
    }
  }
   else if (p_irda->irda_protocol == IRDA_SW_PANA)
  {
    irda_code = kcode;
    p_irda->user_code = ucode;
    mtos_printk("\n %s,%d",__FUNCTION__,__LINE__);
    mtos_printk("   user code: 0x%x\n", ucode);
    mtos_printk("   key code: 0x%x\n", irda_code);
  }
  else
  {
    p_irda->user_code = ucode;
  }

  OS_PRINTF("   user code high: %08x, user code low: %08x\n",
              ((ucode & 0xFF00) >> 8), (ucode & 0xFF));
  OS_PRINTF("   irda code: %08x, %x\n", kcode, irda_code);
  mtos_fifo_put(&p_priv->fifo, irda_code);
}

static u8 irda_concerto_nec_set_rpt_speed(u32 speed_ms)
{
  u32 ot_set = 0;

  /*
   * NEC ir protocol: interval approximately is 110ms between repeat and next repeat.
   * IRDA controller supports a maximum of do once sampling while 15 repeat codes have passed.
  */
  ot_set = (u32)((15 * speed_ms) / 1650);
  ot_set = (ot_set > 0xF)? 0xF : ot_set;
 
  return ot_set;
}

static RET_CODE irda_concerto_set_hw_usercode(u32 en, u32 usercode)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  ptmp = R_IR_NEC_FILT;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  if (en)
  {
    //enable usercode filtering
    //only specific usercode acceptable
    dtmp |= 1 << 31;
    dtmp &= ~(0xFFFF << 8);
    dtmp |= (usercode & 0xFFFF)<<8;
    hal_put_u32((volatile u32 *)ptmp, dtmp);
  }
  else
  {
    //disable usercode filtering
    //all usercode acceptable
    dtmp &= ~ (1 << 31);
    dtmp &= ~(0xFFFF << 8);
    hal_put_u32((volatile u32 *)ptmp, dtmp);
  }

  return SUCCESS;
}

static RET_CODE irda_concerto_set_hw_keycode(u32 en, u32 keycode)
{
  u32 ptmp = 0;
  u32 dtmp = 0;

  ptmp = R_IR_NEC_FILT;
  dtmp = hal_get_u32((volatile u32 *)ptmp);

  if (en)
  {
    //enable keycode filtering
    //only specific keycode acceptable
    dtmp |= 1 << 30;
    hal_put_u32((volatile u32 *)ptmp, dtmp);

    dtmp &= ~0xFF;
    dtmp |= keycode & 0xFF;
    hal_put_u32((volatile u32 *)ptmp, dtmp);
  }
  else
  {
    //disable keycode filtering
    //all keycode acceptable
    dtmp &= ~(1 << 30);
    dtmp &= ~0xFF;
    hal_put_u32((volatile u32 *)ptmp, dtmp);
  }

  return SUCCESS;
}

static RET_CODE irda_concerto_get_usercode(lld_uio_t *p_lld, u16 *p_usercode)
{
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;
  u16 ucode = 0;
  u32 ptmp = 0;
  u32 dtmp = 0;

  if(p_usercode == NULL)
  {
    IRDA_PRINT("Get IRDA user code fail\n");
    return ERR_PARAM;
  }

  if(p_ir->irda_protocol != IRDA_NEC)
  {
    *p_usercode = p_ir->user_code;
  }
  else
  {
    ptmp = R_IR_NEC_DATA;
    dtmp = hal_get_u32((volatile u32 *)ptmp);
    ucode = dtmp & 0xFFFF;

    *p_usercode = ucode;
  }

  return SUCCESS;
}

static RET_CODE irda_concerto_io_ctrl(lld_uio_t * p_lld, u32 cmd, u32 param)
{
  RET_CODE ret = SUCCESS;
  irda_priv_t *p_ir = (irda_priv_t *) p_lld->p_irda;
  int i = 0;
  switch (cmd)
  {
    case UIO_IR_SET_HW_USERCODE:
      ret = irda_concerto_set_hw_usercode(param >> 16, param & 0xffff);
      break;
    case UIO_IR_SET_HW_KEYCODE:
      ret = irda_concerto_set_hw_keycode(param >> 16, param & 0xffff);
      break;
    case UIO_IR_GET_USERCODE:
      ret = irda_concerto_get_usercode(p_lld, (u16 *)param);
      break;
    case UIO_IR_DISABLE_SW_USERCODE:
      p_ir->user_code_set = FALSE;
      for (i = 0; i < IRDA_MAX_USER; i++)
      {
        p_ir->sw_user_code[i] = 0x0;
      }
      break;
    case UIO_IR_PRINT_SW_USERCODE:
      if (param)
      {
        p_ir->print_user_code = TRUE;
      }
      else
      {
        p_ir->print_user_code = FALSE;
      }
      break;
    case UIO_IR_SET_WAVEFILT:
      ret = irda_concerto_set_wfilt(p_ir);
      break;
    case UIO_IR_CHECK_KEY:
      if(p_ir->irda_flag)
      {
        ret = SUCCESS;//irda key pressed
        *((u32 *)param) = p_ir->irda_flag;
      }
      else
      { 
        ret = ERR_FAILURE;//no key pressed
        *((u32 *)param) = p_ir->irda_flag;
      }
      break;
    case UIO_IR_CLEAR_KEY_FLAG:
      p_ir->irda_flag = 0;//set the key flag 0
      break;
    default:
      ret = ERR_NOFEATURE;
      break;
  }

  return ret;
}

static RET_CODE irda_concerto_set_sw_usercode(lld_uio_t * p_lld, u8 num, u16 * p_array)
{
  u8 i = 0;
  irda_priv_t *p_ir = (irda_priv_t *) p_lld->p_irda;

  if (num > IRDA_MAX_USER)
  {
    return ERR_PARAM;
  }
  for (i = 0; i < num; i++)
  {
    p_ir->sw_user_code[i] = p_array[i];
  }
  p_ir->user_code_set = TRUE;

  return SUCCESS;
}

/*!
   Irda soft decoding initialization
*/
void irda_concerto_swdec_init(lld_uio_t *p_lld)
{
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;

  u32 ptmp = 0;
  u32 dtmp = 0;
  u8 sw_int_freq = 0;

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0xF0000001;
  dtmp &= ~(0xF << 24); // Wave filter channel disabled
  dtmp &= ~(0xFF << 16);
  if ((p_ir->irda_protocol == IRDA_SW_RC5) && (hal_get_chip_rev() >= CHIP_CONCERTO_B0))
  {
    dtmp |= 0x20 << 16; // Idle state overtime set as 0x20 (2.667ms)
    dtmp |= 1 << 11;
  }
  else
  {
    dtmp |= 0x7E << 16; // Idle state overtime set as 0x7E (default)
    dtmp &= ~(1 << 11);
  }
  dtmp &= ~(1 << 8); // Discard timeout repeated code (~108ms fixed);
#if DEFAULT_SAMPLE_FREQ
  dtmp |= 1 << 5; // Sample freq 187.5KHz
#else
  dtmp |= 3 << 4; // Sample freq 46.875KHz
#endif
  dtmp |= 1 << 2; // IRDA idle state is High
  dtmp |= 1 << 1; // Soft decoding mode
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  /*
  --IC decoding key down interrupt disabled;
  --IC decoding key up interrupt disabled;
  --Soft decoding Key interrupt enabled;
  --Wave filter interrupt disabled;
  --Soft decoding interrupt every 1 valid sample
  */
  ptmp = R_IR_INT_CFG;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0x0; // Wave filter interrupt disabled
               // IC decoding key up interrupt disabled
               // IC decoding key down interrupt disabled
  dtmp |= 1 << 2; // Soft decoding key interrupt enabled
  /* Repeat sample interrupt frequency
     1. For IRDA protocol with repeated code (i.e. NEC),
        leave INT_FRQ as zero, generate int for each valid sample
	 2. For IRDA protocol without repeated code,
	    configure INT_FRQ as 1 tipically, generate int every 2 valid samples
  */
  if (!p_ir->irda_sw_recv_decimate)
  {
    sw_int_freq = 0;
  }
  else
  {
    sw_int_freq = (p_ir->irda_sw_recv_decimate - 1) & 0x1F;
  }
  dtmp &= ~(0x1F << 8);
  dtmp |= sw_int_freq << 8;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

/*!
   Irda hardware decoding initialization
   Irda controller only supports NEC protocol
*/
void irda_concerto_hwdec_init(lld_uio_t *p_lld)
{
  irda_priv_t *p_ir = (irda_priv_t *)p_lld->p_irda;

  u32 ptmp = 0;
  u32 dtmp = 0;
  u8 rpt_ctrl= 0;

  ptmp = R_IR_NEC_FILT;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= ~(1 << 31); // usercode filter enable
  dtmp &= ~(1 << 30); // keycode filter enable
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_IR_GLOBAL_CTRL;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0xF0000001;
  dtmp &= ~(0xF << 24); // Wave filter channel disabled
  dtmp |= 0x7E << 16; // Idle state overtime set as 0x7E
  // Repeat sample mode
  if((p_ir->irda_nec_repeat_time == 0) || (p_ir->irda_nec_repeat_time > 1650))
  {
    rpt_ctrl = irda_concerto_nec_set_rpt_speed(IRDA_RPT_TIME_MS);
  }
  else
  {
    rpt_ctrl = irda_concerto_nec_set_rpt_speed(p_ir->irda_nec_repeat_time);
  }
  dtmp |= rpt_ctrl << 12;
  dtmp |= 1 << 10; // Enable noise filter
  dtmp |= 1 << 9; // Check if keycode matches inversed keycode
  dtmp &= ~(1 << 8); // Discard timeout repeated code (~108ms fixed);
  dtmp |= 1 << 5; // Sample freq 187.5KHz
  dtmp |= 1 << 2; // IRDA idle state is High
  dtmp &= ~(1 << 1); // IC decoding mode
  hal_put_u32((volatile u32 *)ptmp, dtmp);

  ptmp = R_IR_INT_CFG;
  dtmp = hal_get_u32((volatile u32 *)ptmp);
  dtmp &= 0x0; // Wave filter interrupt disabled
               // Soft decoding key interrupt disabled
               // IC decoding key up interrupt disabled
  dtmp |= 1 << 0; // IC decoding key down interrupt enabled;
  hal_put_u32((volatile u32 *)ptmp, dtmp);
}

static RET_CODE irda_concerto_open(lld_uio_t * p_lld, irda_cfg_t * p_cfg)
{
  irda_priv_t *p_ir = NULL;

  if (NULL != p_lld->p_irda)
  {
    IRDA_PRINT("\nIRDA has already been initialized\n");
    return SUCCESS;
  }

  p_lld->p_irda = (irda_priv_t *) mtos_align_malloc(sizeof(irda_priv_t), 8);
  MT_ASSERT(NULL != p_lld->p_irda);
  memset(p_lld->p_irda, 0, sizeof(irda_priv_t));
  p_ir = p_lld->p_irda;
  p_ir->irda_protocol = p_cfg->protocol;
  p_ir->irda_nec_repeat_time = p_cfg->irda_repeat_time;
  p_ir->irda_sw_recv_decimate = p_cfg->irda_sw_recv_decimate;
  p_ir->irda_wfilt_channel = p_cfg->irda_wfilt_channel;
  p_ir->irda_wfilt_channel_cfg[0] = p_cfg->irda_wfilt_channel_cfg[0];
  p_ir->irda_wfilt_channel_cfg[1] = p_cfg->irda_wfilt_channel_cfg[1];
  p_ir->irda_wfilt_channel_cfg[2] = p_cfg->irda_wfilt_channel_cfg[2];
  p_ir->irda_wfilt_channel_cfg[3] = p_cfg->irda_wfilt_channel_cfg[3];

  is_ir_intialized = FALSE;
  irda_concerto_soft_reset();
  if (p_ir->irda_protocol == IRDA_NEC)
  {
    irda_concerto_hwdec_init(p_lld);
    IRDA_PRINT("IRDA mode 0 initialized\n");
  }
  else
  {
    irda_concerto_swdec_init(p_lld);
    IRDA_PRINT("IRDA mode 1 initialized\n");
  }

  mtos_irq_request(IRQ_IRDA_ID, irda_concerto_int_handler, RISE_EDGE_TRIGGER);
  OS_PRINTF("\nirda open\n");

  return SUCCESS;
}

static RET_CODE irda_concerto_stop(lld_uio_t * p_lld)
{
  mtos_irq_release(IRQ_IRDA_ID, irda_concerto_int_handler);
  if (NULL != p_lld->p_irda)
  {
    mtos_align_free(p_lld->p_irda);
    p_lld->p_irda = NULL;
  }
  is_ir_intialized = FALSE;
  OS_PRINTF("\nirda closed\n");

  return SUCCESS;
}

void irda_attach(lld_uio_t * p_lld)
{
  p_lld->irda_open = irda_concerto_open;
  p_lld->irda_stop = irda_concerto_stop;
  p_lld->irda_io_ctrl = irda_concerto_io_ctrl;
  p_lld->set_sw_usrcode = irda_concerto_set_sw_usercode;
}
