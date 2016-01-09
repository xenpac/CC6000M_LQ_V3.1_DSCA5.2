/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_timer.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_printk.h"
//#include "sys_cfg.h"
#include "mem_manager.h"
#include "stdlib.h"
#include "string.h"
//#include "scheduler.h"
#include "ipc.h"
#include "hal.h"
#include "hal_misc.h"

#define SPI_FLASH_S25FL016A        0x010214
#define SPI_FLASH_S25FL256S        0x010219
#define SPI_FLASH_MX25L25735E      0xc22019
#define SPI_FLASH_F25L32QA         0x8c4116
#define SPI_FLASH_EN25Q32B         0x1c3016
#define SPI_FLASH_EN25QH128        0x1c7018
#define SPI_FLASH_GD25Q64B         0xc84017
#define SPI_FLASH_SST25VF016B      0xbf2541
#define SPI_FLASH_W25Q64           0xef4017

#define SPI_TIME_OUT   20000

#define u8 unsigned char
#define u32 unsigned int

#define RET_CODE int

#define ERR_PARAM -1

#define SPIN_BASE_ADDR                             0xBF010000

#define R_SPIN_MODE_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x0c))
#define R_SPIN_TC(i)                               ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x14))
#define R_SPIN_CTRL(i)                             ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x18))
#define R_SPIN_TXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
#define R_SPIN_RXD(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x100))
#define R_SPIN_STA(i)                              ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x1c))
#define R_CLK_SPI_CFG 0xBF50004C
#define R_SPIN_MOSI_CTRL(i)                       ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x34))


#define R_RST_REQ(n)                              (0xBF510000 + (n) * (0x4))
#define R_RST_REQ_AO                            0xBF150040
#define R_RST_CTRL(n)                             (0xBF510010 + (n) * (0x4))
#define R_RST_CTRL_AO                            0xBF150044
#define R_RST_ALLOW(n)                          (0xBF510020 + (n) * (0x4))
#define R_RST_ALLOW_AO                        0xBF150048

#define R_SPIN_MEM_BOOT(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x24))
#define R_SPIN_BAUD(i)                            ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x08))
#define R_SPIN_INT_CFG(i)                         ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x10))
#define R_SPIN_PIN_CFG_IO(i)                     ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x20))
#define R_SPIN_PIN_MODE(i)                        ((SPIN_BASE_ADDR) + (i) * (0x10000) + (0x2c))

#define BOOL int
#define TRUE 1
#define FALSE 0

#define  SCE_SIZE   (64*1024)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct spi_cmd_cfg
{
    /*!
        todo
      */
  u8 cmd0_ionum;
    /*!
        todo
      */
  u8 cmd0_len;
    /*!
        todo
      */
  u8 cmd1_ionum;
    /*!
        todo
      */
  u8 cmd1_len;
  /*!
    comments
    */
  u8 dummy;
} spi_cmd_cfg_t;


static u32 spiflash_ID = 0xffffff;

static inline void hal_put_u32(volatile unsigned long *p_addr,
                                                 unsigned long data)
{
  *p_addr = data;
}

static inline unsigned long hal_get_u32(volatile unsigned long *p_addr)
{
  return *p_addr;
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


static u32 inl(u32 port)
{
    return *((volatile u32*)(port));
}

static u32 spi_inl(u32 reg)
{
	u32 value;
	value = inl(reg);
	return value;
}

static void wait_spi_bus_complete(u8 spiID)
{
    u32 value;

    do
    {
        value = (spi_inl(R_SPIN_STA(spiID))) & 0x10000;
    }while(value == 0x10000);
}

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

static RET_CODE spi_concerto_read(u8 *p_cmd_buf,u32 cmd_len,
              spi_cmd_cfg_t *p_cmd_cfg,u8 *p_data_buf, u32 data_len)
{
  //spi_cfg_t *p_priv = NULL;
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

  data_ionum = 1 -1;

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

        cmd_dtmp = (cmd1_ionum & 0x3) << 30 | (cmd1_len & 0x3f) << 24
                        | (cmd0_ionum & 0x3) << 22 | (cmd0_len & 0x3f) << 16;

    cmd_dtmp |= data_ionum << 14;
    cmd_dtmp |= dummy << 3;


  if(cmd_len > 0)
  {
    i = cmd_len / 4;
    r = cmd_len % 4;

    p_buf = p_cmd_buf;
    hal_put_u32((volatile unsigned long *)R_SPIN_TC(0), data_len);
    hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(0), cmd_dtmp | 0x205);

    while(i)
    {
      if(spi_concerto_is_txd_fifo_full(0))
      {
        continue;
      }

      dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
      hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
      i --;
      p_buf += 4;
    }

    while(spi_concerto_is_txd_fifo_full(0));
    if(r > 0)
    {
      dtmp = 0;
      for(j = 1; j <= r; j++)
      {
        dtmp |= (p_buf[0] << (8 * (4 - j)));
        p_buf ++;
      }
      hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
    }
  }


{
  if(data_len > 0)
  {
    i = data_len / 4;
    r = data_len % 4;

    p_buf = p_data_buf;
    if(cmd_len <= 0)
    {
      hal_put_u32((volatile unsigned long *)R_SPIN_TC(0), data_len);
      hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(0), cmd_dtmp | 0x205);
    }


    while(i)
    {

      if(spi_concerto_is_rxd_fifo_empty(0))
      {
        continue;
      }

      dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_RXD(0));

      p_buf[0] = (dtmp >> 24) & 0xff ;
      p_buf[1] = (dtmp >> 16) & 0xff;
      p_buf[2] = (dtmp >> 8) & 0xff;
      p_buf[3] = dtmp&0xff;
      i --;
      p_buf += 4;
    }

    if(r > 0)
    {
      while(spi_concerto_is_rxd_fifo_empty(0));
      dtmp = hal_get_u32((volatile unsigned long *)R_SPIN_RXD(0));
      for(j = 1; j <= r; j++)
      {
        p_buf[0] = (dtmp >> (8 * (4 - j))) & 0xff;
        p_buf ++;
      }
    }
  }
  timeout = SPI_TIME_OUT;
  while(!spi_concerto_is_trans_complete(0) && timeout)
  {
    timeout --;
  }
  if(timeout == 0)
  {
    //OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
  }
}
  return SUCCESS;
}


static RET_CODE spi_concerto_write(u8 *p_cmd_buf, u32 cmd_len,
                  spi_cmd_cfg_t *p_cmd_cfg, u8 *p_data_buf, u32 data_len)
{

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
      //OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
      return ERR_PARAM;
  }

  if((p_data_buf == NULL) && (data_len != 0))
  {
      //OS_PRINTF("%s %d  %s\n",__FUNCTION__,__LINE__, __FILE__);
      return ERR_PARAM;
  }



 // spi_concerto_soft_reset(p_priv->spi_id);

  data_ionum = 1 -1;

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


        cmd_dtmp = (cmd1_ionum & 0x3) << 30 |
                    (cmd1_len & 0x3f) << 24 |
                    (cmd0_ionum & 0x3) << 22 |
                    (cmd0_len & 0x3f) << 16;


    cmd_dtmp |= data_ionum << 14;

  if(cmd_len > 0)
  {
      i = cmd_len / 4;
      r = cmd_len % 4;

      p_buf = p_cmd_buf;
      hal_put_u32((volatile unsigned long *)R_SPIN_TC(0), data_len);
      hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(0), cmd_dtmp | 0x203);

      while(i)
      {
          if(spi_concerto_is_txd_fifo_full(0))
          {
              continue;
          }
          dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
          i --;
          p_buf += 4;
      }

      while(spi_concerto_is_txd_fifo_full(0));
      if(r > 0)
      {
          dtmp = 0;
          for(j = 1; j <= r; j++)
          {
              dtmp |= (p_buf[0] << (8 * (4 - j)));
              p_buf ++;
          }

          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
      }
  }

{
  if(data_len > 0)
  {
      i = data_len / 4;
      r = data_len % 4;

      p_buf = p_data_buf;
      if(cmd_len <= 0)
      {
        hal_put_u32((volatile unsigned long *)R_SPIN_TC(0), data_len);
        hal_put_u32((volatile unsigned long *)R_SPIN_CTRL(0), cmd_dtmp | 0x203);
      }

      while(i)
      {
          if(spi_concerto_is_txd_fifo_full(0))
          {
              continue;
          }

          dtmp = (p_buf[0] << 24) | (p_buf[1] << 16) | (p_buf[2] << 8) | p_buf[3];
          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
          i --;
          p_buf += 4;
      }

      while(spi_concerto_is_txd_fifo_full(0));
      if(r > 0)
      {
          dtmp = 0;
          for(j = 1; j <= r; j++)
          {
              dtmp |= (p_buf[0] << (8 * (4 - j)));
              p_buf ++;
          }

          hal_put_u32((volatile unsigned long *)R_SPIN_TXD(0), dtmp);
      }
    }
    timeout = SPI_TIME_OUT;
    while(!spi_concerto_is_trans_complete(0) && timeout)
    {
        timeout --;
    }
    if(timeout == 0)
    {
        //OS_PRINTF("spi timeout  %d  %s\n",__LINE__, __FILE__);
    }
}
    return SUCCESS;
}


static void wait_write_complete()
{
    u8 value = 1;
    u8 cmd[2];

    while(value)
    {
        cmd[0] = 0x05;
        spi_concerto_read(cmd, 1, NULL, &value, 1);  //get w25_status_0
        value = value & 0x01;
    }
}

static u32 spiflash_get_block_cnt(u32 addr, u32 size, u32 block_size)
{
    u32 block_cnt;

    u32 block_start_inx, block_end_inx;

    block_start_inx = addr / block_size;

    block_end_inx = (addr + size - 1) / block_size;

    block_cnt = block_end_inx - block_start_inx + 1;

    return block_cnt;
}

static void spiflash_SST25_unblock()
{
    u8 cmd[5];
    u8 value;

    cmd[0] = 0x05;
    spi_concerto_read(cmd, 1, NULL, &value, 1);

    //OS_PRINTF("org SST25_status_0[0x%02x]\n", value);

    //write enable
    cmd[0] = 0x06;
    spi_concerto_read(cmd, 1, NULL, NULL, 0);  //write enable

    value = 0;

    cmd[0] = 0x01;
    spi_concerto_write(cmd, 1, NULL, &value, 1);

    value = 0;

    cmd[0] = 0x05;
    spi_concerto_read(cmd, 1, NULL, &value, 1);

    //OS_PRINTF("new SST25_status_0[0x%02x]\n", value);
}


static void spiflash_erase(u32 addr, u32 sec_cnt)
{
    u32 block_num = 0;
    u8 cmd[5];

    block_num = addr / SCE_SIZE;
    addr = block_num * SCE_SIZE;

    while(sec_cnt)
    {
        cmd[0] = 0x06;
        spi_concerto_read(cmd, 1, NULL, NULL, 0);

        cmd[0] = 0xd8;
        cmd[1] = addr >> 16;
        cmd[2] = addr >> 8;
        cmd[3] = addr >> 0;

        //OS_PRINTF("Erase: addr: 0x%08x, size: 0x%08x\n", addr, SCE_SIZE);

        spi_concerto_write(cmd, 4, NULL, NULL, 0);

        wait_write_complete();

        sec_cnt --;
        addr += SCE_SIZE;
    }
}

static void spiflash_MX25_erase(u32 addr, u32 sec_cnt)
{
    u32 block_num = 0;
    u8 cmd[5];

    block_num = addr / SCE_SIZE;
    addr = block_num * SCE_SIZE;

    while(sec_cnt)
    {
        cmd[0] = 0x06;
        spi_concerto_read(cmd, 1, NULL, NULL, 0);

        cmd[0] = 0xd8;
        cmd[1] = addr >> 24;
        cmd[2] = addr >> 16;
        cmd[3] = addr >> 8;
        cmd[4] = addr >> 0;

        spi_concerto_write(cmd, 5, NULL, NULL, 0);

        wait_write_complete();

        sec_cnt --;
        addr += SCE_SIZE;
    }
}


static void spiflash_single_normal_readAny(u8 spiID, u32 addr, u8* buf, u32 size)
{
	u8 cmd[4];
	struct spi_cmd_cfg	spiCmdCfg;

	cmd[0] = 0x03;
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;

	spiCmdCfg.cmd0_ionum = 1;	//cmd1 first send
	spiCmdCfg.cmd0_len	 = 3;
	spiCmdCfg.cmd1_ionum = 1;
	spiCmdCfg.cmd1_len	 = 1;
	spiCmdCfg.dummy 	 = 0;

    spi_concerto_read(cmd, 4, &spiCmdCfg, buf, size);
}

static void spiflash_MX25_single_saveAny(u8 spiID, u32 addr, u8* buf, u32 size)
{
	u8 cmd[5];

    u32 cnt;
    u32 page_addr = 0, byte_addr = 0, page_size = 0;
    u32 chunk_len = 0, actual = 0;

    page_size = 256;
    page_addr = addr / page_size;
    byte_addr = addr % page_size;

    cnt = spiflash_get_block_cnt(addr, size, SCE_SIZE);

    spiflash_MX25_erase(addr, cnt);
    mtos_task_sleep(200);

    for (actual = 0; actual < size; actual += chunk_len)
    {
        chunk_len = MIN(size - actual, page_size - byte_addr);

        cmd[0] = 0x06;
        spi_concerto_read(cmd, 1, NULL, NULL, 0);

        cmd[0] = 0x02;
        cmd[1] = page_addr >> 16;
        cmd[2] = page_addr >> 8;
        cmd[3] = page_addr;
        cmd[4] = byte_addr;

        spi_concerto_write(cmd, 5, NULL, buf + actual, chunk_len);

        wait_spi_bus_complete(spiID);

        wait_write_complete();

        page_addr++;
        byte_addr = 0;
    }
}

static void spiflash_SST25_single_saveAny(u8 spiID, u32 addr, u8* buf, u32 size)
{
	u8 cmd[4];
    u32 i;

    u32 cnt;

    spiflash_SST25_unblock();

    cnt = spiflash_get_block_cnt(addr, size, SCE_SIZE);

    spiflash_erase(addr, cnt);
    mtos_task_sleep(200);

    for(i = 0; i < size; i ++)
    {
        cmd[0] = 0x06;
        spi_concerto_read(cmd, 1, NULL, NULL, 0);

        cmd[0] = 0x02;
        cmd[1] = addr >> 16;
        cmd[2] = addr >> 8;
        cmd[3] = addr;

        spi_concerto_write(cmd, 4, NULL, buf + i, 1);

        wait_spi_bus_complete(spiID);

        wait_write_complete();
        addr += 1;
    }
}

static void spiflash_single_saveAny(u8 spiID, u32 addr, u8* buf, u32 size)
{
	u8 cmd[4];

    u32 cnt;
    u32 page_addr = 0, byte_addr = 0, page_size = 0;
    u32 chunk_len = 0, actual = 0;


    page_size = 256;
    page_addr = addr / page_size;
    byte_addr = addr % page_size;

    cnt = spiflash_get_block_cnt(addr, size, SCE_SIZE);
    spiflash_erase(addr, cnt);
    mtos_task_sleep(200);

    for (actual = 0; actual < size; actual += chunk_len)
    {
        chunk_len = MIN(size - actual, page_size - byte_addr);

        cmd[0] = 0x06;
        spi_concerto_read(cmd, 1, NULL, NULL, 0);

        cmd[0] = 0x02;
        cmd[1] = page_addr >> 8;
        cmd[2] = page_addr;
        cmd[3] = byte_addr;

        //OS_PRINTF("Write: addr: 0x%08x, size: 0x%08x\n", (page_addr << 8 | byte_addr), chunk_len);

        spi_concerto_write(cmd, 4, NULL, buf + actual, chunk_len);


        wait_spi_bus_complete(spiID);
        wait_write_complete();
        page_addr++;
        byte_addr = 0;
    }
}

static u32 spi_getJedecID(u8 spiID)
{
    u8 cmd[4];
    u8 codeID[5];
    u32 id = 0xffffff;

    cmd[0] = 0x9f;
	//OS_PRINTF("%d\n",__LINE__);

    spi_concerto_read(cmd, 1, NULL,codeID, 5);
	//OS_PRINTF("%d\n",__LINE__);

    id = (codeID[0] << 16) | (codeID[1] << 8) | (codeID[2] << 0);

    if(id == SPI_FLASH_MX25L25735E)
    {
        cmd[0] = 0xb7;      //enter 4 byte mode
        spi_concerto_read(cmd, 1, NULL, NULL, 0);
    }
    return id;

}

void spi_flash_init()
{
#if 0
	{//reset to set 259MHz
		u32 temp,ptmp,dtmp,reg_id,reset_bit;
		temp = *(volatile unsigned int*)(R_CLK_SPI_CFG);
		*(volatile unsigned int*)(R_CLK_SPI_CFG) = temp | 0x03;

		reg_id = 2;
		reset_bit = 8;
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
		*((volatile u32 *)ptmp) = dtmp;

		dtmp |= 1 << reset_bit;
		*((volatile u32 *)ptmp) = dtmp;

		ptmp = R_RST_REQ(reg_id);
		dtmp = *((volatile u32 *)ptmp);
		dtmp &= ~ (1 << reset_bit);
		*((volatile u32 *)ptmp) = dtmp;
	}
#endif
#if 0
	//following is init spi controller
	hal_put_u32((volatile unsigned long *) R_SPIN_MEM_BOOT(0), 0x00000000);
	{

		u32 dtmp = 0;
		s16 baud_rate = 12;
		u16 phy_clk = 0,clk=50;
		u16 int_delay = 0;

		phy_clk = 259;//81;

		dtmp = hal_get_u32((volatile unsigned long *) R_SPIN_BAUD(0));

		//OS_PRINTF("phy_clk = %d, clk=%d\n",phy_clk, 50);
		baud_rate = phy_clk / clk;
		if((phy_clk%clk) > 0)
			baud_rate ++;

		baud_rate = baud_rate > 0 ? baud_rate : 1;

		int_delay = baud_rate / 2;
		dtmp = baud_rate | (int_delay << 28);

		OS_PRINTF("R_SPIN_BAUD: %x\n",dtmp);
		hal_put_u32((volatile unsigned long *) R_SPIN_BAUD(0), 0x60000006);
	}
	{
		u32 dtmp ;
		dtmp |= 0x1 << 30;
		dtmp |= 0x1 << 29;
		dtmp |= 0x7 << 24;
		dtmp |= 0 << 22;
		dtmp |= 0 << 20;
		dtmp |= 1 << 18;
		dtmp |= 1 << 16;
		dtmp |= 0 << 13;
		dtmp |= 1 << 10;
		dtmp |= 1 << 9;
		dtmp |= 1 << 8;
		dtmp |= 1 << 1;
		dtmp |= 1 << 0;

		hal_put_u32((volatile unsigned long *) R_SPIN_MODE_CFG(0), 0x67050703);

	}
	{
		hal_put_u32((volatile unsigned long *) R_SPIN_INT_CFG(0), 0x100807);
	}
	{
	  u32 dtmp = 0;
	  dtmp = 0x00e400e4;
	  hal_put_u32((volatile unsigned long *)R_SPIN_PIN_CFG_IO(0), dtmp);
	}
	{
	  u32 dtmp = 0;
	  dtmp =0x5f;
	  hal_put_u32((volatile unsigned long *) R_SPIN_PIN_MODE(0), dtmp);
	}

	hal_put_u32((volatile unsigned long *) R_SPIN_MOSI_CTRL(0), 0xa0);
#endif
	//OS_PRINTF("ID 1\n");
	spiflash_ID = spi_getJedecID(0);
	//OS_PRINTF("ID IS %x\n",spiflash_ID);
}

s32 spi_flash_read(u32 addr, u8* buf, u32 size)
{
	u32 spiID = 0;
	spiflash_single_normal_readAny(0,addr,buf,size);
}

s32 spi_flash_write(u32 addr, u8* buf, u32 size)
{
	u32 spiID = 0;
	if((spiflash_ID == SPI_FLASH_W25Q64) ||
		   (spiflash_ID == SPI_FLASH_S25FL256S) ||
		   (spiflash_ID == SPI_FLASH_S25FL016A) ||
		   (spiflash_ID == SPI_FLASH_GD25Q64B) ||
		   (spiflash_ID == SPI_FLASH_EN25QH128) ||
		   (spiflash_ID == SPI_FLASH_EN25Q32B) ||
		   (spiflash_ID == SPI_FLASH_F25L32QA))
	{
		spiflash_single_saveAny(spiID, addr, buf, size);
		mtos_task_sleep(50);
	}
	else if(spiflash_ID == SPI_FLASH_MX25L25735E)
	{
		spiflash_MX25_single_saveAny(spiID, addr, buf, size);
		mtos_task_sleep(50);
	}
	else if(spiflash_ID == SPI_FLASH_SST25VF016B)
	{
		spiflash_SST25_single_saveAny(spiID, addr, buf, size);
		mtos_task_sleep(50);
	}

}


