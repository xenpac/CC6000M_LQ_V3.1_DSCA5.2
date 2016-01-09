/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "mtos_printk.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "hal_irq.h"
#include "hal_base.h"
#include "drv_dev.h"
#include "../../../drvbase/drv_dev_priv.h"
#include "../inc/drv/bus/spi.h"
#include "spi_concerto.h"
#include "../spi_priv.h"

#define SPI_DEBUG 1
#if SPI_DEBUG
#define SPI_DBG_PRINTF OS_PRINTF
#define ENTER_FUNC  OS_PRINTF("\n enter %s \n", __FUNCTION__)
#define LEAVE_FUNC  OS_PRINTF("\n leave %s \n", __FUNCTION__)
#else
#define SPI_DBG_PRINTF(x) do{}while(0)
#define ENTER_FUNC(x) do{}while(0)
#define LEAVE_FUNC(x) do{}while(0)
#endif


#define SPI_PHY_CLK_00        330
#define SPI_PHY_CLK_01        166
#define SPI_PHY_CLK_11        24



/*!
  define SPI delay time in us under polling mode
  */
#define SPI_TIME_OUT   20000

/*!
  define SPI delay function in us
  */
#define SPI_DELAY_US mtos_task_delay_us


static void spi_concerto_ioctrl_set_pins_cfg(u8 spi_id, u8 io_num, spi_pins_cfg_t *p_pins_cfg);
static void spi_concerto_ioctrl_set_pins_dir(u8 spi_id,  u8 io_num, spi_pins_dir_t *p_pins_dir);


static unsigned int is_ddr = 0, is_dma = 0, is_secure = 0;


static void spi_concerto_soft_reset(u8 spi_id)
{
    u32 dtmp = 0;

    dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    dtmp &= ~0x1;
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id), dtmp);
    //SPI_DELAY_US(1000);
    dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    dtmp |= 0x1;
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id), dtmp);
    //SPI_DELAY_US(1000);
}

static void spi_concerto_reset(u8 spi_id)
{
/*
  u32 mask = 0;
  u32 ctrl = 0;
  u32 bit = 0;
  u32 rtmp = 0;

  mask = R_RST_GLOBAL1_MASK;
  ctrl = R_RST_GLOBAL1_CTRL;
  switch(spi_id)
  {
  case 0:
    bit = 0;
    break;
  case 1:
    bit = 1;
    break;
  }
  rtmp = hal_get_u32((volatile unsigned long *)mask);
  rtmp &= ~(1 << bit);
  hal_put_u32((volatile unsigned long *)mask, rtmp);

  rtmp = hal_get_u32((volatile unsigned long *)ctrl);
  rtmp &= ~(1 << bit);
  hal_put_u32((volatile unsigned long *)ctrl, rtmp);

  SPI_DELAY_US(1000);

  rtmp = hal_get_u32((volatile unsigned long *)mask);
  rtmp |= (1 << bit);
  hal_put_u32((volatile unsigned long *)mask, rtmp);

  rtmp = hal_get_u32((volatile unsigned long *)ctrl);
  rtmp |= (1 << bit);
  hal_put_u32((volatile unsigned long *)ctrl, rtmp);

  SPI_DELAY_US(1000);
*/
}

inline static void spi_concerto_pinmux(u8 spi_id)
{
//  u32 dtmp = 0;
/*
  dtmp = hal_get_u32((volatile unsigned long *)0x6f880000);
  if(spi_id == 1)
  {
    dtmp &= ~(0x7 << 4);
    dtmp |= (0x7 << 12);
    dtmp &= ~(1 << 16);
    hal_put_u32((volatile unsigned long *)0x6f880000, dtmp);
  }
*/
}


inline static BOOL spi_concerto_is_txd_fifo_full(u8 spi_id)
{
  u32 dtmp = 0;
  dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_STA(spi_id));
  if((31 - ((dtmp >> 8) & 0x3f)) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

inline static BOOL spi_concerto_is_rxd_fifo_empty(u8 spi_id)
{
  u32 dtmp = 0;
  dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_STA(spi_id));
  if((dtmp & 0x3f) > 0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

inline static BOOL spi_concerto_is_trans_complete(u8 spi_id)
{
  u32 dtmp_sta = 0;
  dtmp_sta = hal_get_u32((volatile unsigned long *) R_SPIN_STA(spi_id));
  if(!((dtmp_sta >> 16) & 0x1))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

static RET_CODE spi_concerto_read(lld_spi_t *p_lld, u8 *p_cmd_buf,u32 cmd_len,
              spi_cmd_cfg_t *p_cmd_cfg,u8 *p_data_buf, u32 data_len)
{
  spi_cfg_t *p_priv = NULL;
  u32 timeout = SPI_TIME_OUT;
  u32 i = 0;
  u8 r = 0;
  u32 j = 0;
  u32 dtmp = 0;
  u8 *p_buf = 0;
  u8 cmd0_ionum = 0, cmd0_len = 0, cmd1_ionum = 0, cmd1_len = 0;
  u8 data_ionum = 0;
  u32 cmd_dtmp = 0;
  u8  dummy =0;
//  u8 num =0;

  if((p_cmd_buf == NULL) && (cmd_len != 0))
  {
    return ERR_PARAM;
  }

  if((p_data_buf == NULL) && (data_len != 0))
  {
    return ERR_PARAM;
  }

  p_priv = (spi_cfg_t *)p_lld->p_priv;
  spi_concerto_pinmux(p_priv->spi_id);

  spi_concerto_soft_reset(p_priv->spi_id);

  data_ionum = p_priv->io_num -1;  

  if(cmd_len >= 0)
  {
    if(p_cmd_cfg == NULL)
    {
      cmd0_ionum = 0;
      cmd0_len = cmd_len;
      cmd1_ionum = 0;
      cmd1_len = 0;
      dummy =0;
    }
    else
    {
      cmd0_ionum = p_cmd_cfg->cmd0_ionum -1;
      cmd0_len = p_cmd_cfg->cmd0_len;
      cmd1_ionum = p_cmd_cfg->cmd1_ionum -1;
      cmd1_len = p_cmd_cfg->cmd1_len;
      dummy = p_cmd_cfg->dummy;
    }
  }
  else if(cmd_len < 0)
  {
    cmd0_ionum = 0;
    cmd0_len = 0;
    cmd1_ionum = 0;
    cmd1_len = 0;
    dummy =0;
  }

    //
    if(is_ddr == 0)
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | (cmd1_len & 0x3f) << 24 
                        | (cmd0_ionum & 0x3) << 22 | (cmd0_len & 0x3f) << 16;
    else
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | (0 & 0x1) << 29 | (cmd1_len & 0x3f) << 24 
                        | (cmd0_ionum & 0x3) << 22 | (1 & 0x1) << 21 
                        | (cmd0_len & 0x3f) << 16 | (1 & 0x1) << 13;
    cmd_dtmp |= data_ionum << 14; 
    cmd_dtmp |= dummy << 3;

  if(cmd_len > 0)
  {
    i = cmd_len / 4;
    r = cmd_len % 4;

    p_buf = p_cmd_buf;
    hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), data_len);


    while(i)
    {
      if(spi_concerto_is_txd_fifo_full(p_priv->spi_id))
      {
        continue;
      }

      dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
      hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
      i --;
      p_buf += 4;
    }

    while(spi_concerto_is_txd_fifo_full(p_priv->spi_id));
    if(r > 0)
    {
      dtmp = 0;
      for(j = 1; j <= r; j++)
      {
        dtmp |= (p_buf[0] << (8 * (4 - j)));
        p_buf ++;
      }
      hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
    }
    if(is_secure)
    {
        hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x305);
    }
    else
    {
        hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x205);
    }
  }

if(is_dma == 0)
{
  if(data_len > 0)
  {
    i = data_len / 4;
    r = data_len % 4;

    p_buf = p_data_buf;
    if(cmd_len <= 0)
    {
      hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), data_len);

      if(is_secure)
      {
          hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x305);
      }
      else
      {
          hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x205);
      }
    }


    while(i)
    {

      if(spi_concerto_is_rxd_fifo_empty(p_priv->spi_id))
      {
        continue;
      }

      dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_RXD(p_priv->spi_id));

      p_buf[0] = (dtmp >> 24) & 0xff ;
      p_buf[1] = (dtmp >> 16) & 0xff;
      p_buf[2] = (dtmp >> 8) & 0xff;
      p_buf[3] = dtmp&0xff;
      i --;
      p_buf += 4;
    }

    if(r > 0)
    {
      while(spi_concerto_is_rxd_fifo_empty(p_priv->spi_id));
      dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_RXD(p_priv->spi_id));
      for(j = 1; j <= r; j++)
      {
        p_buf[0] = (dtmp >> (8 * (4 - j))) & 0xff;
        p_buf ++;
      }
    }
  }
  timeout = SPI_TIME_OUT;
  while(!spi_concerto_is_trans_complete(p_priv->spi_id) && timeout)
  {
    timeout --;
  }
  if(timeout == 0)
  {
    OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
  }
}
  return SUCCESS;
}

#if 0
static RET_CODE spi_concerto_write(lld_spi_t *p_lld, u8 *p_cmd_buf, u32 cmd_len,
                  spi_cmd_cfg_t *p_cmd_cfg, u8 *p_data_buf, u32 data_len)
{
  spi_cfg_t *p_priv = NULL;
  u32 timeout = SPI_TIME_OUT;
  u32 i  = 0;
  u8 r = 0;
  u32 j = 0;
  u32 dtmp = 0;
  u8 *p_buf = NULL;
  u8 cmd0_ionum = 0, cmd0_len = 0, cmd1_ionum = 0, cmd1_len = 0;
  u8 data_ionum = 0;
  u32 cmd_dtmp = 0;

  if((p_cmd_buf == NULL) && (cmd_len != 0))
  {
      OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
      return ERR_PARAM;
  }

  if((p_data_buf == NULL) && (data_len != 0))
  {
      OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
      return ERR_PARAM;
  }

  p_priv = (spi_cfg_t *)p_lld->p_priv;
  spi_concerto_pinmux(p_priv->spi_id);

 // spi_concerto_soft_reset(p_priv->spi_id);

  data_ionum = p_priv->io_num -1;   

  if(cmd_len > 0)
  {
      if(p_cmd_cfg == NULL)
      {
        cmd0_ionum = 0;
        cmd0_len = cmd_len;
        cmd1_ionum = 0;
        cmd1_len = 0;
      }
      else
      {
        cmd0_ionum = p_cmd_cfg->cmd0_ionum -1;
        cmd0_len = p_cmd_cfg->cmd0_len;
        cmd1_ionum = p_cmd_cfg->cmd1_ionum -1;
        cmd1_len = p_cmd_cfg->cmd1_len;
      }
    }
    else if(cmd_len <= 0)
    {
      cmd0_ionum = 0;
      cmd0_len = 0;
      cmd1_ionum = 0;
      cmd1_len = 0;
    }

    if(is_ddr == 0)
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | 
                    (cmd1_len & 0x3f) << 24 | 
                    (cmd0_ionum & 0x3) << 22 | 
                    (cmd0_len & 0x3f) << 16;
    else
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | 
                    (0 & 0x1) << 29 | 
                    (cmd1_len & 0x3f) << 24 | 
                    (cmd0_ionum & 0x3) << 22 | 
                    (1 & 0x1) << 21 | 
                    (cmd0_len & 0x3f) << 16 | 
                    (1 & 0x1) << 13;

    cmd_dtmp |= data_ionum << 14;  
    
  if(cmd_len > 0)
  {
      i = cmd_len / 4;
      r = cmd_len % 4;

      p_buf = p_cmd_buf;
      hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), data_len);
      
      if(is_secure)
      {
          hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x303);
      }
      else
      {
          hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x203);
      }

      while(i)
      {
          if(spi_concerto_is_txd_fifo_full(p_priv->spi_id))
          {
              continue;
          }
          dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
          i --;
          p_buf += 4;
      }

      while(spi_concerto_is_txd_fifo_full(p_priv->spi_id));
      if(r > 0)
      {
          dtmp = 0;
          for(j = 1; j <= r; j++)
          {
              dtmp |= (p_buf[0] << (8 * (4 - j)));
              p_buf ++;
          }

          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
      }
  }
if(is_dma == 0)
{
  if(data_len > 0)
  {
      i = data_len / 4;
      r = data_len % 4;

      p_buf = p_data_buf;
      if(cmd_len <= 0)
      {
        hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), data_len);

        if(is_secure)
        {
            hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x303);
        }
        else
        {
            hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x203);
        }
      }

      while(i)
      {
          if(spi_concerto_is_txd_fifo_full(p_priv->spi_id))
          {
              continue;
          }

          dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
          i --;
          p_buf += 4;
      }

      while(spi_concerto_is_txd_fifo_full(p_priv->spi_id));
      if(r > 0)
      {
          dtmp = 0;
          for(j = 1; j <= r; j++)
          {
              dtmp |= (p_buf[0] << (8 * (4 - j)));
              p_buf ++;
          }

          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
      }
    }
    timeout = SPI_TIME_OUT;
    while(!spi_concerto_is_trans_complete(p_priv->spi_id) && timeout)
    {
        timeout --;
    }
    if(timeout == 0)
    {
        OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
    }
}
    return SUCCESS;
}
#else
static RET_CODE spi_concerto_write(lld_spi_t *p_lld, u8 *p_cmd_buf, u32 cmd_len,
                  spi_cmd_cfg_t *p_cmd_cfg, u8 *p_data_buf, u32 data_len)
{
    spi_cfg_t *p_priv = NULL;
    u32 timeout = SPI_TIME_OUT;
    u32 i  = 0;
    u8 r = 0;
    u32 j = 0;
    u32 dtmp = 0;
    u8 *p_buf = NULL;
    u8 cmd0_ionum = 0, cmd0_len = 0, cmd1_ionum = 0, cmd1_len = 0;
    u8 data_ionum = 0;
    u32 cmd_dtmp = 0;
    u32 tmp = 0;

    if((p_cmd_buf == NULL) && (cmd_len != 0))
    {
        OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
        return ERR_PARAM;
    }

    if((p_data_buf == NULL) && (data_len != 0))
    {
        OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
        return ERR_PARAM;
    }

    p_priv = (spi_cfg_t *)p_lld->p_priv;
    spi_concerto_pinmux(p_priv->spi_id);

    // spi_concerto_soft_reset(p_priv->spi_id);

    tmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(p_priv->spi_id));
    tmp &= ~(1 << 30);
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(p_priv->spi_id), tmp);

    data_ionum = p_priv->io_num -1;   

    if(cmd_len > 0)
    {
        if(p_cmd_cfg == NULL)
        {
            cmd0_ionum = 0;
            cmd0_len = cmd_len;
            cmd1_ionum = 0;
            cmd1_len = 0;
        }
        else
        {
            cmd0_ionum = p_cmd_cfg->cmd0_ionum -1;
            cmd0_len = p_cmd_cfg->cmd0_len;
            cmd1_ionum = p_cmd_cfg->cmd1_ionum -1;
            cmd1_len = p_cmd_cfg->cmd1_len;
        }
    }
    else if(cmd_len <= 0)
    {
        cmd0_ionum = 0;
        cmd0_len = 0;
        cmd1_ionum = 0;
        cmd1_len = 0;
    }

    if(is_ddr == 0)
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | 
                    (cmd1_len & 0x3f) << 24 | 
                    (cmd0_ionum & 0x3) << 22 | 
                    (cmd0_len & 0x3f) << 16;
    else
        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | 
                    (0 & 0x1) << 29 | 
                    (cmd1_len & 0x3f) << 24 | 
                    (cmd0_ionum & 0x3) << 22 | 
                    (1 & 0x1) << 21 | 
                    (cmd0_len & 0x3f) << 16 | 
                    (1 & 0x1) << 13;

    cmd_dtmp |= data_ionum << 14;  
    
    if(cmd_len > 0)
    {
        i = cmd_len / 4;
        r = cmd_len % 4;

        p_buf = p_cmd_buf;
        hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), 0);


        while(i)
        {
            if(spi_concerto_is_txd_fifo_full(p_priv->spi_id))
            {
                continue;
            }
            dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
            hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
            i --;
            p_buf += 4;
        }

        while(spi_concerto_is_txd_fifo_full(p_priv->spi_id));
        if(r > 0)
        {
            dtmp = 0;
            for(j = 1; j <= r; j++)
            {
                dtmp |= (p_buf[0] << (8 * (4 - j)));
                p_buf ++;
            }

            hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
        }
        if(is_secure)
        {
            hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x303);
        }
        else
        {
            hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x203);
        }
        timeout = SPI_TIME_OUT;
        while(!spi_concerto_is_trans_complete(p_priv->spi_id) && timeout)
        {
            timeout --;
        }
        if(timeout == 0)
        {
            OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
        }
    }
    if(is_dma == 0)
    {
        if(data_len > 0)
        {
            u32 snd_times = 0;
            u32 left = 0;
            u32 snd_bytes = 128;

            snd_times = data_len / snd_bytes;
            left = data_len % snd_bytes;

            cmd_dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id));
            cmd_dtmp &= ~((0x3f << 24) | (0x3f << 16));

            p_buf = p_data_buf;
            for(i = 0; i < snd_times; i ++)
            {
                snd_bytes = 128;
                while(snd_bytes)
                {
                    dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
                    hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
                    snd_bytes -= 4;
                    p_buf += 4;
                }


                hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), 128);
                if(is_secure)
                {
                    hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x303);
                }
                else
                {
                    hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x203);
                }
                timeout = SPI_TIME_OUT;
                while(!spi_concerto_is_trans_complete(p_priv->spi_id) && timeout)
                {
                    timeout --;
                }
                if(timeout == 0)
                {
                    OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
                }
            }
    
            if(left)
            {
            
                i = left / 4;
                r = left % 4;
                
                while(i)
                {
                    dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
                    hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
                    i --;
                    p_buf += 4;
                }

                if(r > 0)
                {
                    dtmp = 0;
                    for(j = 1; j <= r; j++)
                    {
                        dtmp |= (p_buf[0] << (8 * (4 - j)));
                        p_buf ++;
                    }
                
                    hal_put_u32((volatile unsigned long *)R_SPIN_TXD(p_priv->spi_id), dtmp);
                }

                hal_put_u32((volatile unsigned long *)R_SPIN_TC(p_priv->spi_id), left);
                if(is_secure)
                {
                    hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x303);
                }
                else
                {
                    hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(p_priv->spi_id), cmd_dtmp | 0x203);
                }
                timeout = SPI_TIME_OUT;
                while(!spi_concerto_is_trans_complete(p_priv->spi_id) && timeout)
                {
                    timeout --;
                }
                if(timeout == 0)
                {
                    OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
                }
            }

        }
    }

    tmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(p_priv->spi_id));
    tmp |= (1 << 30);
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(p_priv->spi_id), tmp);

    return SUCCESS;
}

#endif

static void spi_concerto_ioctrl_set_baud_rate(u8 spi_id, u32 clk)
{
//    u8 clk_sel = 0;
    u32 dtmp = 0;
    s16 baud_rate = 1;
    u16 phy_clk = 0;
    u16 int_delay = 0;


/*
    dtmp = hal_get_u32((volatile unsigned long *)0x70000000);
    if(spi_id == 0)
    {
        clk_sel = (dtmp >> 8) & 0x3;
    }
    else if(spi_id == 1)
    {
        clk_sel = (dtmp >> 10) & 0x3;
    }

    switch(clk_sel)
    {
    case 0:
        phy_clk = SPI_PHY_CLK_00;
        break;
    case 1:
        phy_clk = SPI_PHY_CLK_01;
        break;
    case 3:
        phy_clk = SPI_PHY_CLK_11;
        break;
    }
*/
    //
#if 0
    phy_clk = 259;//81;   
#else
    dtmp = * (volatile unsigned int *)(R_CLK_SPI_CFG);

    OS_PRINTF("R_CLK_SPI_CFG = %d!!!\n",dtmp);
    OS_PRINTF("dtmp&0x3 = %d!!!\n",dtmp&0x3);
    switch(spi_id)
    {
        case 0:
            if((dtmp&0x3) == 0)
              phy_clk = 24;
            else if((dtmp&0x3) == 1)
              phy_clk = 365;
            else if((dtmp&0x3) == 2)
              phy_clk = 275;
            else if((dtmp&0x3) == 3)
              phy_clk = 259;
           break;
        case 1:
           if(((dtmp&0x300)>>8) == 0)
                phy_clk = 24;
           else if(((dtmp&0x300)>>8) == 1)
                 phy_clk = 365;
           else if(((dtmp&0x300)>>8) == 2)
                phy_clk = 275;
           else if(((dtmp&0x300)>>8) == 3)
                phy_clk = 259;
           break;
        case 2:
           if(((dtmp&0x30000)>>16) == 0)
                phy_clk = 24;
           else if(((dtmp&0x30000)>>16) == 1)
                phy_clk = 365;
           else if(((dtmp&0x30000)>>16) == 2)
                phy_clk = 275;
           else if(((dtmp&0x30000)>>16) == 3)
                phy_clk = 259;
           break;
        default:
           OS_PRINTF("error spi_id\n");

}
#endif

    dtmp = hal_get_u32((volatile unsigned long *) R_SPIN_BAUD(spi_id));

    OS_PRINTF("phy_clk = %d, clk=%d\n",phy_clk, clk);
    baud_rate = phy_clk / clk;  
    if((phy_clk % clk) > 0)
        baud_rate ++;

    baud_rate = baud_rate > 0 ? baud_rate : 1;

    int_delay = baud_rate / 2;
    dtmp = baud_rate | (int_delay << 28);

    OS_PRINTF("R_SPIN_BAUD: %x\n",dtmp);
    hal_put_u32((volatile unsigned long *) R_SPIN_BAUD(spi_id), dtmp);
}

static void spi_concerto_set_baud_rate(lld_spi_t * p_lld)
{
    spi_cfg_t *p_priv = NULL;

    p_priv = (spi_cfg_t *)p_lld->p_priv;
    spi_concerto_ioctrl_set_baud_rate(p_priv->spi_id, p_priv->bus_clk_mhz);
}

static void spi_concerto_init_mode(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;
    u32 dtmp = 0;
    p_priv = (spi_cfg_t *)p_lld->p_priv;

    dtmp |= 0x1 << 30;    
    dtmp |= 0x1 << 29;    
    dtmp |= 0x7 << 24;  
    dtmp |= p_priv->op_mode << 22;
    dtmp |= p_priv->op_mode << 20;
    dtmp |= 1 << 18;
    dtmp |= 1 << 16;
    dtmp |= p_priv->op_mode << 13;
    dtmp |= 1 << 10;
    dtmp |= 1 << 9;
    dtmp |= 1 << 8;
    dtmp |= 1 << 1;
    dtmp |= 1 << 0;
    //dtmp = 0x67050703; //  dtmp = 0x67050503;
    hal_put_u32((volatile unsigned long *) R_SPIN_MODE_CFG(p_priv->spi_id), dtmp);

}

static void spi_concerto_init_int(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;
    p_priv = (spi_cfg_t *)p_lld->p_priv;
    OS_PRINTF("spi_concerto_init_int[%d]=%d\n",p_priv->spi_id,0x100807);
    hal_put_u32((volatile unsigned long *) R_SPIN_INT_CFG(p_priv->spi_id), 0x100807);
}

static void spi_concerto_set_pin_cfg(lld_spi_t *p_lld)
{
  spi_cfg_t *p_priv = NULL;
  p_priv = (spi_cfg_t *)p_lld->p_priv;

  spi_concerto_ioctrl_set_pins_cfg(p_priv->spi_id,  p_priv->io_num,p_priv->pins_cfg);
  //spi_concerto_ioctrl_set_pins_cfg(p_priv->spi_id, 2,p_priv->pins_cfg);
}

static void spi_concerto_set_pin_dir(lld_spi_t *p_lld)
{
  spi_cfg_t *p_priv = NULL;
  p_priv = (spi_cfg_t *)p_lld->p_priv;
  spi_concerto_ioctrl_set_pins_dir(p_priv->spi_id, p_priv->io_num, p_priv->pins_dir);
}

static void spi_concerto_init_mosi_ctrl(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;
    p_priv = (spi_cfg_t *)p_lld->p_priv;

    if(p_priv->io_num != 4)
    {
        OS_PRINTF("spi_concerto_init_mosi_ctrl[%d]=%d\n",p_priv->spi_id,0xa0);
        hal_put_u32((volatile unsigned long *) R_SPIN_MOSI_CTRL(p_priv->spi_id), 0xa0);
    }
    else
    {
        OS_PRINTF("spi_concerto_init_mosi_ctrl[%d]=%d\n",p_priv->spi_id,0x00);
        hal_put_u32((volatile unsigned long *) R_SPIN_MOSI_CTRL(p_priv->spi_id), 0x00);
    }
}


//´Ë´¦µÄÐÞ¸ÄÐèÒª¸úICÈ·ÈÏÊÇ·ñÈç´Ë
static void spi_concerto_ioctrl_set_clk_delay(u8 spi_id, u8 delay)
{
    u8 int_delay = 0;
    u32 dtmp = 0;
    int_delay = delay / 2;
	dtmp = hal_get_u32((volatile unsigned long *) R_SPIN_BAUD(spi_id));

    dtmp &= ~0xf0000000;
    dtmp |= int_delay << 28;

	hal_put_u32((volatile unsigned long *) R_SPIN_BAUD(spi_id), dtmp);
}

static void spi_concerto_set_clk_delay(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;

    p_priv = (spi_cfg_t *)p_lld->p_priv;
    OS_PRINTF("spi_concerto_set_clk_delay[%d]=:%x\n",p_priv->spi_id,p_priv->bus_clk_delay);
    spi_concerto_ioctrl_set_clk_delay(p_priv->spi_id, p_priv->bus_clk_delay);
}

static void spi_concerto_disable_mem_boot(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;

    p_priv = (spi_cfg_t *)p_lld->p_priv;

    if(p_priv->spi_id == 0)
    {
        hal_put_u32((volatile unsigned long *) R_SPIN_MEM_BOOT(p_priv->spi_id), 0x00000000);
    }
}

static void spi_concerto_init(lld_spi_t * p_lld)
{
    u8 spi_id = 0;
    spi_cfg_t *p_priv = NULL;
    p_priv = (spi_cfg_t *)p_lld->p_priv;
#if 1
    spi_concerto_disable_mem_boot(p_lld);
    spi_concerto_set_baud_rate(p_lld);
    spi_concerto_init_mode(p_lld);
    spi_concerto_init_int(p_lld);
    spi_concerto_set_pin_cfg(p_lld);
    spi_concerto_set_pin_dir(p_lld);
    spi_concerto_init_mosi_ctrl(p_lld);
    spi_concerto_set_clk_delay(p_lld);

    spi_id = p_priv->spi_id;

#endif
#if 0
    hal_put_u32(R_SPIN_MEM_BOOT(p_priv->spi_id),0);
    hal_put_u32(R_SPIN_BAUD(p_priv->spi_id),0x40000008);
    hal_put_u32(R_SPIN_MODE_CFG(p_priv->spi_id),0x67050703);
    hal_put_u32(R_SPIN_INT_CFG(p_priv->spi_id),0x100807);
    hal_put_u32(R_SPIN_PIN_MODE(p_priv->spi_id),0xff);
    hal_put_u32(R_SPIN_DEBUG(p_priv->spi_id),0);
    hal_put_u32(R_SPIN_MOSI_CTRL(p_priv->spi_id),0xa0);
#endif

    OS_PRINTF("R_SPIN_MEM_BOOT(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_MEM_BOOT(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_BAUD(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_BAUD(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_MODE_CFG(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_INT_CFG(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_INT_CFG(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_PIN_MODE(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_PIN_MODE(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_DEBUG(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_DEBUG(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_MOSI_CTRL(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_MOSI_CTRL(p_priv->spi_id)));
    OS_PRINTF("R_SPIN_PIN_CFG_IO(%d):%x\n",p_priv->spi_id,hal_get_u32((volatile unsigned long *)R_SPIN_MOSI_CTRL(p_priv->spi_id)));

    OS_PRINTF("spi_concerto_init finish\n");
}

static BOOL spi_concerto_cfg_check(spi_cfg_t *p_cfg)
{
  BOOL ret = TRUE;
  if(p_cfg == NULL)
  {
      SPI_DBG_PRINTF("spi configure is null. %s %s %s", __FUNCTION__, __LINE__, __FILE__);
      ret = FALSE;
  }

  if(p_cfg->bus_clk_delay > 15)
  {
      SPI_DBG_PRINTF("bus_clk_delay out of range. %s %s %s", __FUNCTION__, __LINE__, __FILE__);
      ret = FALSE;
  }

  if(p_cfg->spi_id > 2)
  {
      SPI_DBG_PRINTF("spi_id out of range. %s %s %s", __FUNCTION__, __LINE__, __FILE__);
      ret = FALSE;
  }

  if(p_cfg->io_num > 4)
  {
      SPI_DBG_PRINTF("io_num out of range. %s %s %s", __FUNCTION__, __LINE__, __FILE__);
      ret = FALSE;
  }

  if(p_cfg->op_mode > 3)
  {
      SPI_DBG_PRINTF("op_mode out of range. %s %s %s", __FUNCTION__, __LINE__, __FILE__);
      ret = FALSE;
  }

  return ret;
}

static RET_CODE spi_concerto_open(lld_spi_t *p_lld, spi_cfg_t *p_cfg)
{
  spi_cfg_t *p_priv = NULL;
  drv_dev_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  OS_PRINTF("In i2c_wactrller_open\n");

  if(!spi_concerto_cfg_check(p_cfg))
  {
      return ERR_PARAM;
  }

  p_dev = DEV_FIND_BY_LLD(drv_dev_t, p_lld);
  p_base = p_dev->p_base;
  p_priv = p_lld->p_priv = mtos_malloc(sizeof(spi_cfg_t));
  if(! p_priv)
  {
      MT_ASSERT(0);
      return ERR_NO_MEM;
  }

  memcpy(p_priv, p_cfg, sizeof(spi_cfg_t));
  p_base->lock_mode = p_cfg->lock_mode;
#ifndef FPGA //by fliu to reset clock
  OS_PRINTF("SPI ID is %d\n",p_cfg->spi_id);
  if(p_cfg->spi_id != 2)
  {
    u32 temp = 0,ptmp = 0,dtmp = 0,reg_id = 0,reset_bit = 0;
    u32 clk = 0x03;//03 for 259,01 for 365
    temp = *(volatile unsigned int *)(R_CLK_SPI_CFG);
    temp = temp & ~0x3;
    * (volatile unsigned int *)(R_CLK_SPI_CFG) = temp | clk;

    reg_id = 2;
    if(p_cfg->spi_id == 0)
      reset_bit = 8;
    else if(p_cfg->spi_id == 1)
      reset_bit = 9;
    ptmp = R_RST_REQ(reg_id);
    dtmp = *((volatile u32 *)ptmp);
    dtmp |= 1 << reset_bit;
    *((volatile u32 *)ptmp) = dtmp;

    ptmp = R_RST_ALLOW(reg_id);
    do
    {
      dtmp = *((volatile u32 *)ptmp);
    }
    while (!((dtmp >> reset_bit) & 0x1));

    ptmp = R_RST_CTRL(reg_id);
    dtmp = *((volatile u32 *)ptmp);
    dtmp &= ~ (1 << reset_bit);
    //hal_put_u32((volatile u32 *)ptmp, dtmp);
    *((volatile u32 *)ptmp) = dtmp;

    dtmp |= 1 << reset_bit;
    //hal_put_u32((volatile u32 *)ptmp, dtmp);
    *((volatile u32 *)ptmp) = dtmp;

    ptmp = R_RST_REQ(reg_id);
    dtmp = *((volatile u32 *)ptmp);
    dtmp &= ~ (1 << reset_bit);
    //hal_put_u32((volatile u32 *)ptmp, dtmp);
    *((volatile u32 *)ptmp) = dtmp;

  }
#endif
  spi_concerto_reset(p_cfg->spi_id);
  spi_concerto_init(p_lld);
  return SUCCESS;
}


static RET_CODE spi_concerto_close(lld_spi_t *p_lld)
{
  if(NULL != p_lld->p_priv)
  {
    mtos_free(p_lld->p_priv);
    p_lld->p_priv = NULL;
  }
  return SUCCESS;
}


static void spi_concerto_detach(lld_spi_t *p_lld)
{
}

inline static void spi_concerto_ioctrl_set_unit_delay(u8 spi_id, u8 delay)
{
    u32 dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    dtmp &= ~ (0x7f << 1);
    dtmp |= (delay & 0x7f) << 1;
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id) , dtmp);
}

inline static void spi_concerto_ioctrl_enable_hbfirst(u8 spi_id, u8 hbfirst)
{
    u32 dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    if(hbfirst)
    {
        dtmp |= 1 << 9;
    }
    else
    {
        dtmp &= ~(1 << 9);
    }
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id) , dtmp);
}

inline static void spi_concerto_ioctrl_enable_lsb(u8 spi_id, u8 lsb)
{
    u32 dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    if(lsb)
    {
        dtmp |= 1 << 12;
    }
    else
    {
        dtmp &= ~(1 << 12);
    }
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id) , dtmp);
}

inline static void spi_concerto_ioctrl_set_cs2clk_time(u8 spi_id, u8 cs2clk_time)
{
    u32 dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id));
    dtmp &= ~ (0xf << 16);
    dtmp |= (cs2clk_time & 0xf) << 16;  
    dtmp |= (cs2clk_time & 0xf) << 18;
    hal_put_u32((volatile unsigned long *)R_SPIN_MODE_CFG(spi_id) , dtmp);
}

inline static void spi_concerto_ioctrl_set_ionum(u8 spi_id, u8 ionum)
{
  u32 dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_CTRL(spi_id));
  dtmp &= ~(0x3 << 14);
  dtmp |= ((ionum - 1) & 0x3) << 14;   
  hal_put_u32( (volatile unsigned long *)R_SPIN_CTRL(spi_id) , dtmp);
}

static void spi_concerto_ioctrl_set_pins_cfg(u8 spi_id, u8 io_num, spi_pins_cfg_t *p_pins_cfg)
{
  u32 dtmp = 0;
  //dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_PIN_CFG_IO(spi_id));
  dtmp = 0x00e400e4;

  if(io_num == 1)
  {
      dtmp &= ~0x00030003;
      //1��ģʽ�´�MISO1��ȡ
      dtmp |= (p_pins_cfg[0].miso0_src << 18) | (p_pins_cfg[0].spiio0_src);
  }
  else if(io_num == 2)
  {
      dtmp &= ~0x000f000f;
      dtmp |= (p_pins_cfg[1].miso0_src << 16) | (p_pins_cfg[1].spiio0_src) |
             (p_pins_cfg[1].miso1_src << 18) | (p_pins_cfg[1].spiio1_src << 2);
      dtmp = 0x00e400e4;
  }
  else if(io_num == 4)
  {
      dtmp &= ~0x00ff00ff;
      dtmp |= 0x00e400e4;
      //dtmp = (p_pins_cfg[2].miso0_src << 16) | (p_pins_cfg[2].spiio0_src)      |
      //       (p_pins_cfg[2].miso1_src << 18) | (p_pins_cfg[2].spiio1_src << 2) |
      //       (p_pins_cfg[2].miso2_src << 20) | (p_pins_cfg[2].spiio2_src << 4) |
      //       (p_pins_cfg[2].miso3_src << 22) | (p_pins_cfg[2].spiio3_src << 6) ;
  }

  hal_put_u32((volatile unsigned long *)R_SPIN_PIN_CFG_IO(spi_id), dtmp);

}

static void spi_concerto_ioctrl_set_pins_dir(u8 spi_id,  u8 io_num, spi_pins_dir_t *p_pins_dir)
{

  u32 dtmp = 0;

  dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_PIN_MODE(spi_id));
  dtmp =0x5f;
  if(io_num == 1)
  {
    dtmp &= ~0x3;
    dtmp |= (p_pins_dir[0].spiio0_dir);
  }
  else if(io_num == 2)
  {
    dtmp &= ~0xf;
    dtmp |= (p_pins_dir[1].spiio0_dir) | (p_pins_dir[1].spiio1_dir << 2);
    dtmp |=0xf;
  }
  else if(io_num == 4)
  {
    dtmp |=0xff; //Ä¬ÈÏÄ£Ê½
    //dtmp &= ~0xff;
    //dtmp |= (p_pins_dir[2].spiio0_dir)    | (p_pins_dir[2].spiio1_dir << 2) |
    //        (p_pins_dir[2].spiio2_dir<<4) | (p_pins_dir[2].spiio3_dir << 6);
  }
  OS_PRINTF("spi_concerto_ioctrl_set_pins_dir[%d]=0x%x\n",spi_id,dtmp);
  hal_put_u32((volatile unsigned long *) R_SPIN_PIN_MODE(spi_id), dtmp);
}

static RET_CODE spi_concerto_ioctrl(lld_spi_t *p_lld, u32 cmd, u32 param)
{
    spi_cfg_t *p_priv = (spi_cfg_t *)p_lld->p_priv;
    switch(cmd)
    {
        case SPI_IOCTRL_SET_CLCK:
            spi_concerto_ioctrl_set_baud_rate(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_SET_CLK_DELAY:
            if(param > 15)
            {
                SPI_DBG_PRINTF("SPI_IOCTRL_SET_CLK_DELAY out of range. %s %s %s",
                    __FUNCTION__, __LINE__, __FILE__);
                return ERR_PARAM;
            }
            spi_concerto_ioctrl_set_clk_delay(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_SET_CS2CLK_TIME:
            if(param > 4)
            {
                SPI_DBG_PRINTF("SPI_IOCTRL_SET_CS2CLK_TIME out of range. %s %s %s",
                    __FUNCTION__, __LINE__, __FILE__);
                return ERR_PARAM;
            }
            spi_concerto_ioctrl_set_cs2clk_time(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_ENABLE_LSB:
            spi_concerto_ioctrl_enable_lsb(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_ENABLE_HIGH_BYTE_FIRST:
            spi_concerto_ioctrl_enable_hbfirst(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_SET_TRANS_UNIT_DELAY:
            if(param > 0x7f)
            {
                SPI_DBG_PRINTF("SPI_IOCTRL_SET_TRANS_UNIT_DELAY out of range. %s %s %s",
                    __FUNCTION__, __LINE__, __FILE__);
                return ERR_PARAM;
            }
            spi_concerto_ioctrl_set_unit_delay(p_priv->spi_id, param);
            break;
        case SPI_IOCTRL_SET_TRANS_IONUM:
            if(param > 2 && param < 1)
            {
                SPI_DBG_PRINTF("SPI_IOCTRL_SET_TRANS_IONUM out of range. %s %s %s",
                    __FUNCTION__, __LINE__, __FILE__);
                return ERR_PARAM;
            }
            p_priv->io_num = param;
            spi_concerto_ioctrl_set_ionum(p_priv->spi_id, param);
            spi_concerto_ioctrl_set_pins_dir(p_priv->spi_id,param, p_priv->pins_dir);
            spi_concerto_ioctrl_set_pins_cfg(p_priv->spi_id, param,p_priv->pins_cfg);

            break;
        case SPI_IOCTRL_SET_DDR:
            is_ddr = param;
            break;
        case SPI_IOCTRL_SET_DMA:
            is_dma = param;
            break;
        case SPI_IOCTRL_SET_SECURE:
            is_secure = param;
            break;
        default:
            break;
    }

    return SUCCESS;
}

static RET_CODE reset(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;
    p_priv = (spi_cfg_t *)p_lld->p_priv;
    spi_concerto_reset(p_priv->spi_id);
    spi_concerto_init(p_lld);
    return SUCCESS;
}

static RET_CODE soft_reset(lld_spi_t *p_lld)
{
    spi_cfg_t *p_priv = NULL;
    p_priv = (spi_cfg_t *)p_lld->p_priv;
    spi_concerto_soft_reset(p_priv->spi_id);
    return SUCCESS;
}

RET_CODE spi_concerto_attach(char *p_name)
{
  spi_bus_t *p_dev = NULL;
  device_base_t *p_base = NULL;
  lld_spi_t *p_lld = NULL;

  if((p_dev = dev_allocate(p_name, SYS_BUS_TYPE_SPI,
                           sizeof(spi_bus_t),
                           sizeof(lld_spi_t))) == NULL)
  {
    return ERR_FAILURE;
  }

  p_base = (device_base_t *)(p_dev->p_base);
  p_base->open = (RET_CODE (*)(void *, void *))spi_concerto_open;
  p_base->close = (RET_CODE (*)(void *))spi_concerto_close;
  p_base->detach = (void (*)(void *))spi_concerto_detach;
  p_base->io_ctrl = (RET_CODE (*)(void *, u32, u32))spi_concerto_ioctrl;

  p_lld = (lld_spi_t *)p_dev->p_priv;
  p_lld->read = spi_concerto_read;
  p_lld->write = spi_concerto_write;
  p_lld->reset = reset;
  p_lld->soft_reset = soft_reset;


  OS_PRINTF("spi_concerto_attach over\n");
  return SUCCESS;
}
