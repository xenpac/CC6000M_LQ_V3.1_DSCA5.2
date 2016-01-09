/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_int.h"
#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mem_manager.h"
#include "drv_dev.h"
#include "../../drvbase/drv_dev_priv.h"

#include "hal_base.h"
#include "hal_irq.h"
#include "hal_gpio.h"

#include "smc_op.h"
#include "smc_pro.h"

//#define PPS
//#define SMC_PRO_PRINT_ON

#ifdef SMC_PRO_PRINT_ON
#define SMC_PRO_PRINTF     OS_PRINTF
#define SMC_PRO_DUMP(data, len) { const int l = (len);\
                        int i = 0;\
                        for(i = 0 ; i < l ; i ++)
                        {\
                          SMC_PRO_PRINTF(" %02x",*((data) + i));\
                        }\
                        SMC_PRO_PRINTF("\n"); }
#else
#define SMC_PRO_PRINTF DUMMY_PRINTF
#define SMC_PRO_DUMP(data,len) ;
#endif

#define ATR_CONVENTION_DIRECT 0 /* Direct convention */
#define ATR_CONVENTION_INVERSE  1 /* Inverse convention */
#define ATR_PRO_TYPE_T0 0 /* Protocol type T=0 */
#define ATR_PRO_TYPE_T1 1 /* Protocol type T=1 */
#define ATR_PRO_TYPE_T2 2 /* Protocol type T=2 */
#define ATR_PRO_TYPE_T3 3 /* Protocol type T=3 */
#define ATR_PRO_TYPE_T14  14  /* Protocol type T=14 */
#define ATR_INF_BYTE_TA 0 /* Interface byte TAi */
#define ATR_INF_BYTE_TB 1 /* Interface byte TBi */
#define ATR_INF_BYTE_TC 2 /* Interface byte TCi */
#define ATR_INF_BYTE_TD 3 /* Interface byte TDi */

static u8 atr_num_ib_table[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

/*!
  Encodes clockrate conversion
  */
#define SMART_FI_DEFAULT           1
/*!
  Encodes bitrate conversion
  */
#define SMART_DI_DEFAULT           1
/*!
  620 for IS1521, 590 for IS1526
  */
#define SMC_ETU_T14_DEF    620

#define SMC_GT_T0 12
#define SMC_GT_T1 11
#define SMC_WT 9600
#define SMC_MAX_ATR 32
#define SMC_PPSS_CLA 0xff
#define SMC_DEFAULT_WI 10
#define SMC_CMD_P3_SPE 0x00
#define SMC_STATUS_BTYE_OK 0x9000

#define RFU -1
#define SMC_FS -1
#define SMC_PRO_CT_CK_SIZE 4

/*!
Clock rate table - indicates the F value to be used.
   */
typedef struct
{
  int fi;
  int fmax;
} smc_clk_rate_t;


/*!
smc pro private struct
   */
typedef struct
{
  u32 card_type;
  scard_device_t *p_smc;
  scard_protocol_t m_protocol;
}smc_pro_priv_t;

/*!
define the size of the smc pro ct name
   */
#define SMC_PRO_CT_NAME_SZ 20

/*!
Card type definition
   */
typedef enum
{
  SMC_CARD_DEF = 0,
  SMC_CARD_CONAX,
  SMC_CARD_VIACCESS,
  SMC_CARD_IRDETO_162,
  SMC_CARD_DREAMCRYPT_223,
  SMC_CARD_DVN_TEST,
  SMC_CARD_IRDETO_557,
  SMC_CARD_IRDETO_178,
  SMC_CARD_LIHE_E02B,
  SMC_CARD_VIACCESS3585,
  SMC_CARD_VIACCESS7289,
  SMC_CARD_TONGFANG,
  SMC_CARD_ATOSCARD,
}scard_card_type_list_t;

/*!
Card type struct definition
   */
typedef struct
{
  scard_card_type_list_t ct_e;
  u8   *p_ct_atr;
  u32 *p_ct_ck;
  u8    name[SMC_PRO_CT_NAME_SZ];
}scard_card_type_t;

static smc_pro_priv_t g_smc_pro_priv = {0};

#if 0
/*!
Values of D defined in protocol.
   */
static u16 baud_rate_table[] = { RFU, 1, 2, 4, 8, 16, 32,
                                   RFU, 12, 20, RFU, RFU,
                                   RFU, RFU, RFU, RFU};
/*!
Values of Fi, Fmax defined in protocol.
   */
static smc_clk_rate_t clock_rate_table[] = { { 372, 4 },
                                               { 372, 5 },
                                               { 558, 6 },
                                               { 744, 8 },
                                               { 1116, 12 },
                                               { 1488, 16 },
                                               { 1860, 20 },
                                               { RFU, RFU },
                                               { RFU, RFU },
                                               { 512, 5 },
                                               { 768, 7 },
                                               { 1024, 10 },
                                               { 1536, 15 },
                                               { 2048, 20 },
                                               { RFU, RFU },
                                               { RFU, RFU } };
#endif

/*!
   Initialize HW to be ready for ATR reception.
 */
static void itn_proll_config_def(void)
{
  scard_config_t smc_cfg;
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  smc_cfg.bit_d_rate = 1;
  smc_cfg.clk_f_rate = 372;
  smc_cfg.blk_guard = 0xff;
  smc_cfg.blk_time = 19200;
  smc_cfg.ch_guard = 12;
  smc_cfg.ch_time = 19200;
  smc_cfg.clk_ref = 19;
  smc_cfg.conv_set = FALSE;
  smc_cfg.parity_check = FALSE;
  scard_set_config(p_priv->p_smc, &smc_cfg);
}


static BOOL itn_check_card(scard_atr_desc_t *p_atr)
{
  scard_pro_cfg_t pro_cfg;
  OS_PRINTF("\nitn_check_card\n");
  if(scard_pro_atr_parse(p_atr, &pro_cfg) != SUCCESS)
  {
    OS_PRINTF("\nitn_check_card::ATR parse fail\n");
    return FALSE;
  }
  return TRUE;
}

static void _calculate_etu(smc_working_param_t *p_working_param)
{
  u16 Fi = 372, Di = 1;
  u32 MaxClock = 4 * MHZ;
  u32 WWT = 0;

  /* Calculate work waiting time in work etus */
  WWT = 960 * p_working_param->wi;

  /* Next translate our rate conversion factor Interger into F.
  * Used conversion table in IS0 7816-3 for values, coded this way to
  * make it easier to read.
  */
  switch(p_working_param->FI)
  {
    case 0: /* Internal clock, treat differently */
      Fi = 372;
      MaxClock = 4*MHZ;
      break;
    case 1:
      Fi = 372;
      MaxClock = 5*MHZ;
      break;
    case 2:
      Fi = 558;
      MaxClock = 6*MHZ;
      break;
    case 3:
      Fi = 744;
      MaxClock = 8*MHZ;
      break;
    case 4:
      Fi = 1116;
      MaxClock = 12*MHZ;
      break;
    case 5:
      Fi = 1488;
      MaxClock = 16*MHZ;
      break;
    case 6:
      Fi = 1860;
      MaxClock = 20*MHZ;
      break;
    case 9:
      Fi = 512;
      MaxClock = 5*MHZ;
      break;
    case 10:
      Fi = 768;
      MaxClock = 7500000;
      break;
    case 11:
      Fi = 1024;
      MaxClock = 10*MHZ;
      break;
    case 12:
      Fi = 1536;
      MaxClock = 15*MHZ;
      break;
    case 13:
      Fi = 2048;
      MaxClock = 20*MHZ;
      break;
    default:
      Fi = 372;
      MaxClock = 0;
      break;
  }

  /* Now adjust using the Bit rate adjustment factor D.
  * This is also taken from ISO7816-3 and coded this way
  * to make it easier to read.
  */
  switch(p_working_param->DI)
  {
    case 1:
      Di = 1;
      break;
    case 2:
      Di = 2;
      WWT = WWT * 2;
      break;
    case 3:
      Di = 4;
      WWT = WWT * 4;
      break;
    case 4:
      Di = 8;
      WWT = WWT * 8;
      break;
    case 5:
      Di = 16;
      WWT = WWT * 16;
      break;
    case 6:
      Di = 32;
      WWT = WWT * 32;
      break;
    case 8:
      Di = 12;
      WWT = WWT * 12;
      break;
    case 9:
      Di = 20;
      WWT = WWT * 20;
      break;
    case 10:
      Di = 1;
      Fi = Fi / 2;
      WWT = WWT / 2;
      break;
    case 11:
      Di = 1;
      Fi = Fi / 4;
      WWT = WWT / 4;
      break;
    case 12:
      Di = 1;
      Fi = Fi / 8;
      WWT = WWT / 8;
      break;
    case 13:
      Di = 1;
      Fi = Fi / 16;
      WWT = WWT / 16;
      break;
    case 14:
      Di = 1;
      Fi = Fi / 32;
      WWT = WWT / 32;
      break;
    case 15:
      Di = 1;
      Fi = Fi / 64;
      WWT = WWT / 64;
      break;
    default:
      Di = 1;
      break;
  }

  /* Set maximum clock frequency */
  p_working_param->clk_max = MaxClock;

  /* Set subsequent clock frequency */
  p_working_param->Fi = Fi;
  p_working_param->Di = Di;

  /* Set new WWT */
  p_working_param->wwt = WWT;
}

RET_CODE scard_pro_atr_parse(scard_atr_desc_t *p_atr, scard_pro_cfg_t *p_cfg)
{
  u8 TDi = 0;
  u8 pointer = 0, pn = 0;
  u8 i = 0;
  u8 modeT = 0, FirstT = 0;
  u8 *p_buf = p_atr->p_buf;

  memset(p_cfg, 0, sizeof(scard_pro_cfg_t));

  /* Check size of p_buf */
  if (p_atr->atr_len < 2)
  {
    return ERR_FAILURE;
  }

  /* Store T0 and TS */
  p_cfg->TS = p_buf[0];
  if((p_cfg->TS != 0x3b) && (p_cfg->TS != 0x3f))
  {
    OS_PRINTF("TS error\n");
    return ERR_FAILURE;
  }
  
  p_cfg->T0 = TDi = p_buf[1];
  pointer = 1;

  /* Store number of historical bytes */
  p_cfg->hbn = TDi & 0x0F;

  /* TCK is not present by default */
  (p_cfg->TCK).present = FALSE;

  /* Extract interface bytes */
  while(TDi != 0)
  {
    /* Check p_buf is long enought */
    if (pointer + atr_num_ib_table[(0xF0 & TDi) >> 4] >= p_atr->atr_len)
    {
      return ERR_FAILURE;
    }
    /* Check TAi is present */
    if ((TDi | 0xEF) == 0xFF)
    {
      pointer++;
      p_cfg->ib[pn][ATR_INF_BYTE_TA].value = p_buf[pointer];
      p_cfg->ib[pn][ATR_INF_BYTE_TA].present = TRUE;
      if(pn == 0)
      {
        //TA1
        p_cfg->work_param.FI = (p_buf[pointer] & 0xF0) >> 4;
        p_cfg->work_param.DI = p_buf[pointer] & 0x0F;
      }
      else if(pn == 1)
      {
        //TA2
        p_cfg->work_param.SpecificMode = 1;
        p_cfg->work_param.SpecificType = p_buf[pointer] & 0x0F;
        p_cfg->work_param.SpecificTypeChangable = (p_buf[pointer] & 0x80) ? 1 : 0;
      }
      else if(pn > 1 && modeT == 1)
      {
        p_cfg->work_param.IFSC = p_buf[pointer];
      }
    }
    else
    {
      p_cfg->ib[pn][ATR_INF_BYTE_TA].present = FALSE;
    }
    /* Check TBi is present */
    if ((TDi | 0xDF) == 0xFF)
    {
      pointer++;
      p_cfg->ib[pn][ATR_INF_BYTE_TB].value = p_buf[pointer];
      p_cfg->ib[pn][ATR_INF_BYTE_TB].present = TRUE;
      if(pn == 0)
      {
        //TB1
        p_cfg->work_param.IInt = (p_buf[pointer] & 0x60) >> 5;
        p_cfg->work_param.PInt1 = p_buf[pointer] & 0x1f;
      }
      else if(pn == 1)
      {
        //TB2
        p_cfg->work_param.PInt2 = p_buf[pointer];
      }
      else if ((pn > 1) && (modeT == 1))
      {
        p_cfg->work_param.cwi = p_buf[pointer] & 0x0f;
        p_cfg->work_param.bwi = p_buf[pointer] >> 4;
      }
    }
    else
    {
      p_cfg->ib[pn][ATR_INF_BYTE_TB].present = FALSE;
    }

    /* Check TCi is present */
    if ((TDi | 0xBF) == 0xFF)
    {
      pointer++;
      p_cfg->ib[pn][ATR_INF_BYTE_TC].value = p_buf[pointer];
      p_cfg->ib[pn][ATR_INF_BYTE_TC].present = TRUE;
      if(pn == 0)
      {
        //TC1
        p_cfg->work_param.N = p_buf[pointer];
      }
      else if(pn == 1)
      {
        //TC2
        p_cfg->work_param.wi = p_buf[pointer];
      }
      else if((pn > 1) && (modeT == 1))
      {
        /* bit0==1 use CRC, ==0 use LRC */
        p_cfg->work_param.RC = p_buf[pointer] & 0x01;
      }
    }
    else
    {
      p_cfg->ib[pn][ATR_INF_BYTE_TC].present = FALSE;
    }

    /* Read TDi if present */
    if ((TDi | 0x7F) == 0xFF)
    {
      pointer++;
      TDi = p_cfg->ib[pn][ATR_INF_BYTE_TD].value = p_buf[pointer];
      modeT = p_buf[pointer] & 0x0F;
      p_cfg->ib[pn][ATR_INF_BYTE_TD].present = TRUE;
      (p_cfg->TCK).present = ((TDi & 0x0F) != ATR_PRO_TYPE_T0);
      if (pn >= ATR_MAX_PROTOCOLS)
      {
        return ERR_FAILURE;
      }
      pn++;
    }
    else
    {
      if(pn == 0)
      {
        modeT = 0;
      }
      p_cfg->ib[pn][ATR_INF_BYTE_TD].present = FALSE;
      TDi = 0;
    }
    p_cfg->work_param.SupportedProtocolTypes |= (0x01 << modeT);
    if(pn == 0)
    {
      FirstT = modeT;
    }
  }

  /* Store number of protocols */
  p_cfg->pn = pn + 1;

  /* Store historical bytes */
  if (pointer + p_cfg->hbn >= p_atr->atr_len)
  {
    return ERR_FAILURE;
  }

  //  memcpy (atr->hb, p_buf + pointer + 1, atr->hbn);
  for(i = 0; i < p_cfg->hbn; i++)
  {
    *(p_cfg->hb + i) = *(p_buf + pointer + 1 + i);
  }
  pointer += (p_cfg->hbn);

  /* Store TCK  */
  if ((p_cfg->TCK).present)
  {
    if ((pointer + 1) >= p_atr->atr_len)
    {
      OS_PRINTF("actually no Tck\n");
    }
    else
    {
      pointer++;
      (p_cfg->TCK).value = p_buf[pointer];
    }
  }

  if (p_cfg->work_param.SpecificMode)
  {
    /* Choose specific mode */
    p_cfg->work_param.WorkingType = p_cfg->work_param.SpecificType;
  }
  else
  {
    /* Choose first offered protocol */
    p_cfg->work_param.WorkingType = FirstT;

    /* Use default FInt and Dint */
    if (p_cfg->ib[0][ATR_INF_BYTE_TA].present == FALSE)
    {
      p_cfg->work_param.FI = SMART_FI_DEFAULT;
      p_cfg->work_param.DI = SMART_DI_DEFAULT;
    }
  }

  /* Calculate work etu */
  _calculate_etu(&p_cfg->work_param);

  /* config convention */
  if(p_atr->p_buf[0] == 0x3f)
  {
    p_cfg->work_param.convention = 1;
  }

  OS_PRINTF("ATR raw data: ");
  for(i = 0; i < p_atr->atr_len; i++)
  {
    OS_PRINTF("0x%02x ", p_atr->p_buf[i]);
  }
  OS_PRINTF("\n");
  
  OS_PRINTF("ATR:hb[%s],con[%d],Fi[%d],Di[%d],clk[%d],T[%d],PI1[%d],PI2[%d],II[%d],N[%d]\n", 
    p_cfg->hb, p_cfg->work_param.convention, p_cfg->work_param.Fi, p_cfg->work_param.Di, 
    p_cfg->work_param.clk_max, p_cfg->work_param.WorkingType, p_cfg->work_param.PInt1, 
    p_cfg->work_param.PInt2, p_cfg->work_param.IInt ,p_cfg->work_param.N);

  p_cfg->length = pointer + 1;
  return SUCCESS;
}


RET_CODE scard_pro_interface_cfg(smc_working_param_t *p_param)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;
  scard_config_t smc_cfg = {0};

  scard_get_config(p_priv->p_smc, &smc_cfg);
  OS_PRINTF("get scard config: clk_ref[%d], clk_f_rate[%d], bit_d_rate[%d], con_set[%d]\n", 
    smc_cfg.clk_ref, smc_cfg.clk_f_rate, smc_cfg.bit_d_rate, smc_cfg.conv_set);

  /* Protocol check */
  if(((1 << p_param->WorkingType) & p_param->SupportedProtocolTypes)
    != (1 << p_param->WorkingType))
  {
    OS_PRINTF("not supported protocol[%d], %s %d\n",
      p_param->WorkingType, __FUNCTION__,__LINE__);
  }

  /* common setting */
  smc_cfg.parity_check = p_param->parity;
  smc_cfg.conv_set = p_param->convention;

  /* calculate char extra guard time */
  if(p_param->N == 255)
  {
    if(p_param->WorkingType == 0)
    {
      smc_cfg.ch_guard = SMC_GT_T0;
    }
    else if(p_param->WorkingType == 1)
    {
      smc_cfg.ch_guard = SMC_GT_T1;
    }
  }
  else
  {
    smc_cfg.ch_guard = 12 + p_param->N;
  }

  /* calculate Fi and Di */
  if(p_param->DI != 0)
  {
    /* use DI and FI */
    _calculate_etu(p_param);
  }

  if(p_param->WorkingType == SMC_PROTOCOL_T14)
  {
    p_param->Fi = SMC_ETU_T14_DEF;
    p_param->Di = 1;
    smc_cfg.clk_f_rate = SMC_ETU_T14_DEF;
    smc_cfg.bit_d_rate = 1;
  }
  else
  {
    smc_cfg.clk_f_rate = p_param->Fi;
    smc_cfg.bit_d_rate = p_param->Di;
  }

  /* calculate char time or block time for T1 or T14 */
  if (SMC_PROTOCOL_T1 == p_param->WorkingType ||
      SMC_PROTOCOL_T14 == p_param->WorkingType)
  {
    smc_cfg.ch_time = SMC_GT_T1 + (1 << p_param->cwi);//CWT
    smc_cfg.blk_time = SMC_GT_T1 +
    (1 << p_param->bwi) * 960 * 372 * p_param->Di
        / p_param->Fi; //BWT

    smc_cfg.blk_guard = 22;//BGT
  }

  scard_set_config(p_priv->p_smc, &smc_cfg);
  
  OS_PRINTF("set scard config: clk_ref[%d], clk_f_rate[%d], bit_d_rate[%d], con_set[%d]\n", 
    smc_cfg.clk_ref, smc_cfg.clk_f_rate, smc_cfg.bit_d_rate, smc_cfg.conv_set);
  
  p_priv->m_protocol = p_param->WorkingType;

  return SUCCESS;
}


RET_CODE scard_pro_deactive(void)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  return scard_deactive(p_priv->p_smc);
}

RET_CODE scard_pro_rw_transaction(scard_opt_desc_t *p_rwopt)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;

  //SMC_PRO_PRINTF("%s line%d\n",__FUNCTION__,__LINE__);
  return scard_rw_transaction(p_priv->p_smc, p_rwopt);
}

/*********************************************************************************
T=0 Protocol Process
**********************************************************************************/
#define T0_CMD_LENGTH   5
#define T0_CLA_OFFSET   0
#define T0_INS_OFFSET   1
#define T0_P1_OFFSET    2
#define T0_P2_OFFSET    3
#define T0_P3_OFFSET    4
#define T0_RETRIES_DEFAULT  3
#define T0_MAX_LENGTH   256

static s32 smc_t0_procedure_bytes(scard_device_t *dev, u8 INS, s16 num_to_transfer, u8 status[2])
{
  s32 r = 0;
  u32 act_size = 0;
  u8 buff = 0;

  SMC_PRO_PRINTF("t0: PROC: ");

  do
  {
    do
    {
      if (scard_read(dev, &buff, 1, &act_size) != SUCCESS)
      {
        return ERR_TIMEOUT;
      }
      SMC_PRO_PRINTF("%02x ", buff);
    } while(buff == 0x60);  /*NULL, send by the card to reset WWT*/

    if((buff&0xF0) == 0x60 || (buff&0xF0) == 0x90) //SW1
    {
      status[0] = buff;
      if(scard_read(dev, &buff, 1, &act_size) != SUCCESS)
      {
        return ERR_TIMEOUT;
      }
      SMC_PRO_PRINTF("%02x\n", buff);
      status[1] = buff;
      return 0;
    }
    else //ACK
    {
      if((buff ^ INS) == 0)
      {
        /* ACK == INS*/
        /*Vpp is idle. All remaining bytes are transfered subsequently.*/
        r = num_to_transfer;
      }
      else if((buff ^ INS) == 0xFF)
      {
        /* ACK == ~INS*/
        /*Vpp is idle. Next byte is transfered subsequently.*/
        r = 1;
      }
      else if((buff ^ INS) == 0x01)
      {
        /* ACK == INS+1*/
        /*Vpp is active. All remaining bytes are transfered subsequently.*/
        r = num_to_transfer;
      }
      else if((buff ^ INS) == 0xFE)
      {
        /* ACK == ~INS+1*/
        /*Vpp is active. Next bytes is transfered subsequently.*/
        r = 1;
      }
      else
      {
        SMC_PRO_PRINTF("iso: cannot handle procedure %02x (INS=%02x)\n", buff, INS);
        return -1;
      }
      if(r > num_to_transfer)
      {
        SMC_PRO_PRINTF("iso: data overrun r=%d num_to_transfer=%d", r, num_to_transfer);
        return -1;
      }
    }
  } while(0 == r);

  SMC_PRO_PRINTF("\n");
  return r;
}

RET_CODE scard_pro_t0_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  s32 error = SUCCESS;
  u8 write_to_smartcard = 0;
  u32 size = 0;
  s16 num_to_transfer = 0;
  s16 length = 0;
  s16 temp_length = 0;
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;
  u32 num_to_write = p_rwopt->bytes_out;
  s32 ret = 0;

  p_rwopt->bytes_in_actual = 0;

  if(num_to_write > T0_CMD_LENGTH)
  {
    write_to_smartcard = 1;
    num_to_transfer = p_command[T0_P3_OFFSET];
    SMC_PRO_PRINTF("iso: wr, size[%d]\n", num_to_transfer);
  }
  else if(num_to_write == T0_CMD_LENGTH)
  {
    write_to_smartcard = 0;
    if(p_response == NULL)
    {
      /* write data to smart card*/
      num_to_transfer = 0;
    }
    else
    {
      /*read data from smart card*/
      num_to_transfer = (p_command[T0_P3_OFFSET] == 0) ? T0_MAX_LENGTH : p_command[T0_P3_OFFSET];
    }
    SMC_PRO_PRINTF("iso: rd, size[%d]\n", num_to_transfer);
  }
  else
  {
    SMC_PRO_PRINTF("iso: error p_command length!\n");
    return ERR_FAILURE;
  }

  /* Check the CLA and INS bytes are valid */
  if (p_command[T0_CLA_OFFSET] != 0xFF)
  {
    //if ((p_command[T0_INS_OFFSET] & 0x01) == 0 &&
    if((p_command[T0_INS_OFFSET] & 0xF0) != 0x60 &&
    (p_command[T0_INS_OFFSET] & 0xF0) != 0x90)
    {
      SMC_PRO_PRINTF("iso: scard_write call!\n");
      if(scard_write(g_smc_pro_priv.p_smc, p_command, T0_CMD_LENGTH, &size) != SUCCESS)
      {
        SMC_PRO_PRINTF("iso: write cmd error!\n");
        return ERR_FAILURE;
      }
      length = 0;
      while(1)
      {
        temp_length = smc_t0_procedure_bytes(g_smc_pro_priv.p_smc,
          p_command[T0_INS_OFFSET], num_to_transfer - length, status);
        if(temp_length == 0)
        {
          return SUCCESS;
        }
        else if(temp_length < 0)
        {
          return ERR_FAILURE;
        }
        if (write_to_smartcard == 1)
        {
          if(scard_write(g_smc_pro_priv.p_smc,
            p_command + T0_CMD_LENGTH + length, temp_length, &size) != SUCCESS)
          {
            SMC_PRO_PRINTF("iso: write data error!\n");
            return ERR_FAILURE;
          }
          SMC_PRO_PRINTF("iso: -> WDATA: ");
          SMC_PRO_DUMP(p_command + T0_CMD_LENGTH + length, temp_length);
        }
        else
        {
          ret = scard_read(g_smc_pro_priv.p_smc,
            p_response + length, temp_length, &size);
          if (ret != SUCCESS)
          {
            SMC_PRO_PRINTF("iso: read data error!\n");
            return ERR_FAILURE;
          }
          p_rwopt->bytes_in_actual += size;
          SMC_PRO_PRINTF("iso: <- DATA: ");
          SMC_PRO_DUMP(p_response + length, temp_length);
        }
        length += temp_length;
      }
    }
    else
    {
      SMC_PRO_PRINTF("iso: INS is invalid!\n");
      /* INS is invalid */
      error = ERR_FAILURE;
    }
  }
  else
  {
    SMC_PRO_PRINTF("iso: CLA is invalid!\n");
    /* CLA is invalid */
    error = ERR_FAILURE;
  }
  return error;
}

/*********************************************************************************
T=1 Protocol Process
**********************************************************************************/
/* Generally useful things */
#define MAXBLOCKLEN             3 + 254 + 2

/* Our internal state markers */
#define SMC_T1_OURTX          0x0001
#define SMC_T1_CHAINING_US    0x0002
#define SMC_T1_CHAINING_THEM  0x0004
#define SMC_T1_TXWAITING      0x0008
#define SMC_T1_SRESPEXPECT    0x0010
#define SMC_T1_VPPHIGHTILNAD  0x0020
#define SMC_T1_GOTNAK         0x0040
#define SMC_T1_WTX            0x0080

/* Block types */
#define BLOCK_TYPE_BIT          0xC0
#define S_REQUEST_BLOCK         0xC0
#define R_BLOCK                 0x80
#define I_BLOCK_1               0x00
#define I_BLOCK_2               0x40

/* In S-block */
#define S_RESYNCH_REQUEST       0x00
#define S_IFS_REQUEST           0x01
#define S_ABORT_REQUEST         0x02
#define S_WTX_REQUEST           0x03
#define S_VPP_ERROR_P_RESPONSE    0x24
#define S_P_RESPONSE_BIT          0x20
#define S_RESYNCH_P_RESPONSE      0xE0

/* I-block */
#define I_CHAINING_BIT          0x20
#define I_SEQUENCE_BIT          0x40

/* R-block */
#define R_EDC_ERROR             0x01
#define R_OTHER_ERROR           0x02

/* NAD byte */
#define VPP_NOTHING             0x00
#define VPP_TILNAD              0x08
#define VPP_TILPCB              0x80
#define VPP_INVALID             0x88

/*!
Block type
   */
typedef enum
{
  T1_R_BLOCK = 0,
  T1_S_REQUEST,
  T1_S_P_RESPONSE,
  T1_I_BLOCK,
  T1_CORRUPT_BLOCK
} smc_block_type_t;

/*!
smc_t1_header
   */
typedef struct 
{
  u8 NAD;
  u8 PCB;
  u8 LEN;
} smc_t1_header_t;

/*!
smc_t1_block
   */
typedef struct 
{
  smc_t1_header_t header;
  u8 *p_buffer;
  union
  {
    u8 LRC;
    u8 CRC;
  } EDC;
} smc_t1_block_t;

#define NEXT_SEQ(i)      ((i == 1) ? 0 : 1)

static u8 smc_gen_lrc(u8 *mem, int len)
{
  u8 cs = 0;

  while(len > 0)
  {
    cs ^= *mem++;
    len--;
  }
  return cs;
}

RET_CODE scard_pro_t1_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  u32 actlen = 0;
  u8 lrc = 0;
  u8 buffer[MAXBLOCKLEN];
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;

  p_rwopt->bytes_in_actual = 0;

  /* assume that user has prepared the block and EDC */
  SMC_PRO_PRINTF("scard_pro_t1_transactionT1::CMD:");
  SMC_PRO_PRINTF(p_command, p_command[2] + 4);

  if (scard_write(g_smc_pro_priv.p_smc,
    p_command, p_command[2] + 4, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }
  
  if (scard_read(g_smc_pro_priv.p_smc,
    buffer, 3, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }
  
  if (scard_read(g_smc_pro_priv.p_smc,
    buffer + 3, buffer[2] + 1, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }

  SMC_PRO_PRINTF("scard_pro_t1_transactionT1::T1 ASR:");
  SMC_PRO_PRINTF(buffer, buffer[2] + 4);
  SMC_PRO_PRINTF("\n");

  lrc = smc_gen_lrc(buffer, buffer[2]+3);
  if (lrc != buffer[buffer[2] + 3])
  {
    SMC_PRO_PRINTF("scard_pro_t1_transactionT1::EDR Failure\n");
    return ERR_FAILURE;
  }

  memcpy(p_response, buffer, buffer[2] + 4);
  p_rwopt->bytes_in_actual = buffer[2] + 4;

  return SUCCESS;
}


/*********************************************************************************
T=14 Protocol Process
**********************************************************************************/
#define T14_XOR_START   0x3F  /* Start value for xor checksumm */
#define T14_CMD_LEN_ADDR  5   /* T14 p_command length byte */
#define T14_CMD_HEAD_LEN  6   /* T14 p_command header length, do not include checksum */

RET_CODE scard_pro_t14_transaction(scard_opt_desc_t *p_rwopt, u8 status[2])
{
  u16 length = 0;
  u8 xorsum = 0;
  u32 actlen = 0;
  u8 *p_command = p_rwopt->p_buf_out;
  u8 *p_response = p_rwopt->p_buf_in;
  u32 num_to_write = p_rwopt->bytes_out;

  p_rwopt->bytes_in_actual = 0;

  length = p_command[T14_CMD_LEN_ADDR] + T14_CMD_HEAD_LEN;
  if (num_to_write != length + 1)
  {
    SMC_PRO_PRINTF("iso: error p_command length \n");
    return ERR_FAILURE;
  }

  xorsum = smc_gen_lrc(p_command, length) ^ T14_XOR_START;
  if (p_command[length] != xorsum)
  {
    SMC_PRO_PRINTF("iso: update T14 check sum \n");
    //    p_command[length] = xorsum;
  }

  SMC_PRO_PRINTF("T14 CMD: ");
  SMC_PRO_DUMP(p_command, num_to_write);
  if (scard_write(g_smc_pro_priv.p_smc,
    p_command, num_to_write, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }

  if (scard_read(g_smc_pro_priv.p_smc,
    p_response, 4, &actlen) != SUCCESS)
  {
    return ERR_FAILURE;
  }
  length = 4;
  if (p_response[0] == p_command[0] && p_response[1] == p_command[1])
  {
    status[0] = p_response[2];
    status[1] = p_response[3];
    if (scard_read(g_smc_pro_priv.p_smc,
        p_response + length, 5, &actlen) != SUCCESS)
    {
      return ERR_FAILURE;
    }
    length += 5;
    if (p_response[7])
    {
      if (scard_read(g_smc_pro_priv.p_smc,
          p_response + length, p_response[7], &actlen) != SUCCESS)
      {
        return ERR_FAILURE;
      }
      length += p_response[7];
      if (smc_gen_lrc(p_response, length) != T14_XOR_START)
      {
        SMC_PRO_PRINTF("iso: T14 checksum failed\n");
        return ERR_FAILURE;
      }
    }
  }
  else
  {
    status[0] = p_response[1];
    status[1] = p_response[2];
  }

  p_rwopt->bytes_in_actual = length;

  SMC_PRO_PRINTF("T14 ASR: ");
  SMC_PRO_DUMP(p_response, length);
  return SUCCESS;
}

static void data_dump(u8 *p_addr, u32 size)
{
  u32 i = 0;

  OS_PRINTF("\r\n size is %d: \r ",size);

  for(i = 0; i < size; i++)
  {
    OS_PRINTF("%2x ", p_addr[i]);

    if ((0 == (i % 4)) && (i != 0))
    {
      OS_PRINTF("    ");
    }
    if ((0 == (i % 32)) && (i != 0))
    {
      OS_PRINTF("\r\n");
    }
  }

  OS_PRINTF("\r\n");
}

RET_CODE scard_pro_active(scard_atr_desc_t *p_atr)
{
  smc_pro_priv_t *p_priv = &g_smc_pro_priv;
  scard_config_t smc_cfg;
  RET_CODE ret = SUCCESS;
  BOOL check = TRUE;
  MT_ASSERT((NULL != p_atr) && (NULL != p_atr->p_buf));

  scard_reset(p_priv->p_smc);
  itn_proll_config_def();
  OS_PRINTF("\n******scard_pro_active::ACTIVE TRY 1*****\n");
  ret = scard_active(p_priv->p_smc, p_atr);
  check = itn_check_card(p_atr);
  OS_PRINTF("\n===scard_pro_active::ATR Data_dump====\n");
  data_dump(p_atr->p_buf, p_atr->atr_len);
  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = TRUE;
    scard_set_config(p_priv->p_smc, &smc_cfg);
    OS_PRINTF("\n******scard_pro_active::ACTIVE TRY 2*****\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }

  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = FALSE;
    smc_cfg.clk_ref = 11;
    smc_cfg.clk_f_rate = 625;
    scard_set_config(p_priv->p_smc, &smc_cfg);
    OS_PRINTF("\n******scard_pro_active::ACTIVE TRY 3*****\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }

  if((ret != SUCCESS && ret != ERR_HARDWARE) || check != TRUE)
  {
    scard_reset(p_priv->p_smc);
    scard_get_config(p_priv->p_smc, &smc_cfg);
    smc_cfg.conv_set = FALSE;
    smc_cfg.clk_ref = 18;
    smc_cfg.clk_f_rate = 607;
    scard_set_config(p_priv->p_smc, &smc_cfg);
    OS_PRINTF("\n*****scard_pro_active::ACTIVE TRY 4******\n");
    ret = scard_active(p_priv->p_smc, p_atr);
    check = itn_check_card(p_atr);
  }
  OS_PRINTF("active ret[%d] \n",ret);

  return SUCCESS;
}

RET_CODE scard_pro_register_op(scard_device_t *p_op_device)
{
  g_smc_pro_priv.p_smc = p_op_device;

  return SUCCESS;
}

RET_CODE scard_pro_get_card_type(u32 *card_type)
{
  *card_type = g_smc_pro_priv.card_type;
  return SUCCESS;
}

#ifdef PPS
#define PPS_MAX_LENGTH  6

#define PPS_HAS_PPS1(block) ((block[1] & 0x10) == 0x10)
#define PPS_HAS_PPS2(block) ((block[1] & 0x20) == 0x20)
#define PPS_HAS_PPS3(block) ((block[1] & 0x40) == 0x40)

static int scard_pps_length(u8 *block)
{
  u32 length = 3;
  
  if(PPS_HAS_PPS1(block)) 
  {
    length++;
  }

  if(PPS_HAS_PPS2(block))
  {
    length++;
  }

  if(PPS_HAS_PPS3(block))
  {
    length++;
  }
  return length;
}
static u32 data_cmp(u8 *src,u8 *dst, u32 len)
{
  while (len--)
  {
    if(*src++ != *dst++)
    {
      return 1;
    }
  }
  return 0;
}
RET_CODE smc7816_pps_req(smc_working_param_t *p_param)
{
  u8 request[PPS_MAX_LENGTH] = {0};
  u8 response[PPS_MAX_LENGTH] = {0};
  //u32 baudrate = 0;
  u32 len_request = 0,len_response = 0,len_confirm = 0;

  /* build PPS command */
  /* PPSS: initial byte-always 0xFF */
  request[0] = 0xFF;
  /* PPS0: format byte-encodes T and PPS1 to follow */
  request[1] = (0x10 | p_param->WorkingType);
  /* PPS1: parameter byte-encodes F and D */
  request[2] = (p_param->DI | (p_param->FI << 4));
  /* PPS2 and PPS3 are RFU, so ignore */
  /* PCK : check byte-bitwise XOR of PPSS,PPS0,and PPS1 */
  request[3] = (request[0] ^ request[1] ^ request[2]);

  OS_PRINTF("PPS: request\n");
  data_dump(request, 4);
  if (scard_write(g_smc_pro_priv.p_smc, request, 4, &len_request) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_write error\n");
    return ERR_FAILURE;
  }
  if (4 != len_request)
  {
    OS_PRINTF("PPS: len error\n");
    return ERR_FAILURE;
  }
  /*Get PPSS and PPSO response first*/
  if (scard_read(g_smc_pro_priv.p_smc, response, 2, &len_response) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_read error\n");
    return ERR_FAILURE;
  }
  /*Get the next 4 bytes PPS1 PPS2 PPS3 and PCK*/
  len_confirm = scard_pps_length(response);
  if (scard_read(g_smc_pro_priv.p_smc, (response + 2), len_confirm - 2 , &len_response) != SUCCESS)
  {
    OS_PRINTF("PPS: scard_read2 error\n");
    return ERR_FAILURE;
  }
  OS_PRINTF("PPS: response\n");
  data_dump(response, len_confirm);
  if ((len_request != len_confirm)|| (data_cmp(request, response, len_confirm) == 1))
  {
    OS_PRINTF("PPS: fail\n");
    return ERR_FAILURE;
  }
  /*PPS1 response is not exist,so use Fd and Dd*/
  if(!PPS_HAS_PPS1(response))
  {
    p_param->FI = 0;
    p_param->DI = 1;
  }
  scard_pro_interface_cfg(&p_param);
  return SUCCESS;
}

#endif

