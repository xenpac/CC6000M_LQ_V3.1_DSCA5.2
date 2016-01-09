/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_task.h"
#include "mtos_msg.h"
#include "mtos_sem.h"

#include "lib_rect.h"
#include "mtos_misc.h"

#include "drv_dev.h"
#include "driver.h"

#include "sys_regs_concerto.h"
#include "hal_base.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_dma.h"
#include "hal_gpio.h"

#include "charsto.h"
#include "uio.h"

#include "lib_util.h"
#include "class_factory.h"
#include "fcrc.h"
#include "mdl.h"

#include "ap_framework.h"
#include "ap_uio.h"
#include "i2c.h"
#include "sys_cfg.h"

#define CRC_MODE      CRC32_ARITHMETIC
#define CRC_INIT      0xFFFFFFFF
#define NO_CRC        0x4352434E // "NCRC"

#define DM_INDICATOR1      0x23 //'#'
#define DM_INDICATOR2      0x2A //'*'

#define FP_POWER_LED_INDEX 1
#define FP_LOCK_LED_INDEX 0


typedef enum
{
  DM_ST_IDLE = 0,
  DM_ST_GET_KEY,
  DM_ST_SET_DATA,
  DM_ST_GET_DATA,
  DM_ST_TEST,
  DM_ST_SET_HW,
  DM_ST_GET_LED_MAP,
  DM_ST_WAIT_USER_ACK,
  DM_ST_LED_ACK_CHECK,
  DM_ST_LIGHT_ALL,
  DM_ST_TEST_LED,
}diamond_status_t;

static u8 dm_status = DM_ST_IDLE;

typedef enum
{
  DM_CMD_NONE = 0,
  DM_CMD_TEST = 0x50,
  DM_CMD_SET_DATA,
  DM_CMD_GET_DATA,
  DM_CMD_SET_HW,    // = 0x56,
  DM_CMD_GET_LED_MAP,
  DM_CMD_LED_MAP_ACK,
  DM_CMD_LED_ON,
  DM_CMD_LED_OFF,
  DM_CMD_TEST_LED,
  DM_CMD_GET_KEY,
  DM_CMD_EXIT_KEY,
  DM_CMD_EXIT_LED,
  DM_CMD_MOVE_LEFT,
  DM_CMD_MOVE_RIGHT,
  DM_CMD_CONFIRM,
  DM_CMD_LIGHT_ALL,
  DM_CMD_END
}diamond_cmd_t;

/*************************************
   respond id
   board respond to pc, and pc respond to board
  **************************************/
typedef enum
{
  DM_RES_OK = 0,
  DM_RES_ERR,
  DM_RES_ACK,
  DM_RES_UNKNOWN,
}diamond_response_t;

typedef enum
{
  DM_ERR_NONE = 0,
  DM_ERR_DATA_SIZE,
  DM_ERR_INDICATOR,
  DM_ERR_TIMEOUT,
  DM_ERR_CRC,
  DM_ERR_TYPE,
  DM_ERR_CMD,
}diamond_error_code_t;

typedef enum
{
  DM_LEDMAP_ST_ON = 0,
  DM_LEDMAP_ST_OFF,
  DM_LEDMAP_ST_SEG0,
  DM_LEDMAP_ST_SEG1,
  DM_LEDMAP_ST_SEG2,
  DM_LEDMAP_ST_SEG3,
  DM_LEDMAP_ST_SEG4,
  DM_LEDMAP_ST_SEG5,
  DM_LEDMAP_ST_SEG6,
  DM_LEDMAP_ST_SEG7,
  DM_LEDMAP_ST_MAX
}led_map_status_t;

#define DM_CMD_MAX_LEN           100 //normal case, UPG client check if it <120

static u8 cmd_buff[DM_CMD_MAX_LEN + 1] = {0};

typedef enum
{
  VDAC_CVBS_RGB = 0,
  VDAC_DUAL_CVBS,
  VDAC_CVBS_YPBPR,
  VDAC_CVBS_SVIDEO,
  VDAC_SIGN_CVBS,
}vdac_info_t;


typedef enum
{
  SPDIF_ON = 0,
  SPDIF_OFF,
}spdif_info_t;

typedef enum
{
  LOCK_LOW_ENABLE = 0,
  LOCK_HIGH_ENABLE,
}lamp_lock_t;

typedef enum
{
  LNB_LOAD_ON = 0,
  LNB_LOAD_OFF,
}lnb_load_t;

#define MAX_IR1_KEY_NUM 128
#define MAX_IR2_KEY_NUM 128
#define MAX_FP_KEY_NUM 16
#define MAX_LED_MAP_NUM 128
#define MAX_KEY_DEALY_TICKS 500
#define MAX_LED_DEALY_TICKS 30
#define MAX_LANG_NUM  13

typedef enum
{
  FP_GPIO = 0,
  FP_MCU
}fp_type_t;

typedef struct
{
  BOOL scart_enable;
  u8 pin_tvav;
  u8 pin_tvmode;
  u8 pin_tvratio;
  BOOL sog_enable;
}scart_info_t;

typedef enum
{
  DISP_TIME,
  DISP_OFF,
  DISP_NONE
}fp_disp_t;

typedef enum
{
  //ratio 480p
  RATIO_480I_576I,
  //ratio 576p
  RATIO_480P_576_P,
  //ratio 720p
  RATIO_720_P,
  //ratio 1080i
  RATIO_1080_i,
  //ratio 1080p
  RATIO_1080_P
}resolution_ratio;

typedef enum
{
  //pal
  TV_SYSTEM_PAL,
  //ntsc
  TV_SYSTEM_NTSC,
  //auto
  TV_SYSTEM_AUTO,
}tv_system;

typedef struct
{
  unsigned long addr;  
  unsigned long data; 
}reg_data_t;

typedef struct
{
  u8 num;
  reg_data_t array_data[10];  
}reg_cfg_t;

typedef struct
{
  fp_type_t fp_type;
  pan_hw_info_t pan_info;
  led_bitmap_t led_bitmap[MAX_LED_MAP_NUM];
  u16 map_size;  
  vdac_info_t vdac_info;
  spdif_info_t spdif_info;
  lnb_load_t lnb_load;
  scart_info_t scart_info;
  fp_disp_t fp_disp;
  u8 standby_mode;
  u8 loop_through;
  u8 tuner_clock; //0-demo 1-other
  u8 convention;  
  u8 detect_pin_pol;
  u8 vcc_enable_pol;
  BOOL enable_mute_pin;
  u8 mute_pin;
  BOOL mute_gpio_level;
  reg_cfg_t reg;
  u8 led_num;
  u32 reserve1; 
  u32 reserve2; 
  u32 reserve3; 
}hw_cfg_t;

typedef struct
{
  hw_cfg_t hw_cfg;
  key_map_t ir1_keymap[MAX_IR1_KEY_NUM];
  key_map_t ir2_keymap[MAX_IR2_KEY_NUM];
  key_map_t fp_keymap[MAX_FP_KEY_NUM];
  u16 ir1_num;
  u16 ir2_num;
  u16 fp_num;
  u16 usr_code[2];
}diamond_cfg_t;

typedef enum
{
  UIO_NONE = 0,
  UIO_GPIO,
  UIO_MCU,
}uio_state_t;

typedef enum
{
  LOCK_TEST_OK = 0,
  LOCK_TEST_L,
  LOCK_TEST_R,
  LOCK_TEST_SWITCH,
}lock_test_t;


/****global  ******/
static uio_state_t uio_state = UIO_NONE;
static uio_device_t *gp_uio_dev = NULL;
static u32 g_pre_lbd_pos = 0;
static u32 g_pre_led_pos = 0;
static u32 g_last_cmd_id = DM_CMD_END;
static u32 g_ledmap_status = DM_LEDMAP_ST_MAX;
static  uio_device_t *p_dev_i2cfp = NULL;
static BOOL bFirst = FALSE;
static const pan_hw_info_t pan_info =
{
  PAN_SCAN_SHADOW,  /*type_scan, */
  0,              /*type_lbd,  0: Stand-alone LBD, 1: LBD in shifter */
  1,              /*type_com, 0: Stand-alone COM, 1: COM in shifter*/
  0,              /*num_scan, Number of scan PIN, 0: no scan; <= 2 */
  4,              /*num_com,  Number of COM PIN, 0: no com; <= 8 */
  8,              /*pos_colon, Position of colon flag, 0 to 7, 8 no colon */
  0,              /*special control byte: bit 0: revert the data sequence or not
                     bit 1: HT1628 special cmd, bit 2: GPIO control lbd or not*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*flatch*/
  {GPIO_LEVEL_HIGH, GPIO_DIR_OUTPUT, 63},  /*fclock*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*fdata*/
  {{GPIO_LEVEL_HIGH, GPIO_DIR_INPUT, 63},    /*scan[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}},  /*scan[1]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[3]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[4]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[5]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[6]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*com[7]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*lbd[3]*/
  20,       /* com_scan_intv, scan com digital interval in ms, */
  300,     /* repeat_intv, Repeat interval in ms, for rpt key only */
};

static diamond_cfg_t diamond_cfg;


#define FP_START_WORK 0
#define FP_STOP_WORK  1

static led_bitmap_t fp_bitmap_test[] =
{
  {'0', 0x01}, {'1', 0x02}, {'2', 0x04}, {'3', 0x08},
  {'4', 0x10}, {'5', 0x20}, {'6', 0x40}, {'7', 0x80},
  {'8', 0xFF}, {' ', 0x00}
};
#define FP_TABLE_SIZE_TEST sizeof(fp_bitmap_test) / sizeof(led_bitmap_t)

static led_bitmap_t fp_bitmap_test1[] =
{
  {'0', 0xFE}, {'1', 0xFD}, {'2', 0xFB}, {'3', 0xF7},
  {'4', 0xEF}, {'5', 0xDF}, {'6', 0xBF}, {'7', 0x7F},
  {'8', 0x00}, {' ', 0xFF}
};
#define FP_TABLE_SIZE_TEST1 sizeof(fp_bitmap_test1) / sizeof(led_bitmap_t)

static led_bitmap_t fp_bitmap_benchmark[] =
{
  {'.', 0x7F},
  {'0', 0xC0}, {'1', 0xF9}, {'2', 0xA4}, {'3', 0xB0},
  {'4', 0x99}, {'5', 0x92}, {'6', 0x82}, {'7', 0xF8},
  {'8', 0x80}, {'9', 0x90}, {'a', 0x88}, {'A', 0x88},
  {'b', 0x83}, {'B', 0x83}, {'c', 0xC6}, {'C', 0xC6},
  {'d', 0xA1}, {'D', 0xA1}, {'e', 0x86}, {'E', 0x86},
  {'f', 0x8E}, {'F', 0x8E}, {'g', 0x90}, {'G', 0xC2},
  {'h', 0x89}, {'H', 0x89}, {'i', 0xFB}, {'I', 0xCF},
  {'j', 0xF1}, {'J', 0xF1}, {'l', 0xC7}, {'L', 0xC7},
  {'n', 0xAB}, {'N', 0xC8}, {'o', 0xA3}, {'O', 0xC0},
  {'p', 0x8C}, {'P', 0x8C}, {'q', 0x98}, {'Q', 0x98},
  {'r', 0xAF}, {'R', 0x88}, {'s', 0x92}, {'S', 0x92},
  {'t', 0x87}, {'T', 0xCE}, {'u', 0xC1}, {'U', 0xC1},
  {'y', 0x91}, {'Y', 0x91}, {'z', 0xA4}, {'Z', 0xA4},
  {':', 0x7F}, {'-', 0xBF}, {'_', 0xF7}, {' ', 0xFF},
};


static led_bitmap_t fp_bitmap[] =
{
  {'.', 0x7F},
  {'0', 0xC0}, {'1', 0xF9}, {'2', 0xA4}, {'3', 0xB0},
  {'4', 0x99}, {'5', 0x92}, {'6', 0x82}, {'7', 0xF8},
  {'8', 0x80}, {'9', 0x90}, {'a', 0x88}, {'A', 0x88},
  {'b', 0x83}, {'B', 0x83}, {'c', 0xC6}, {'C', 0xC6},
  {'d', 0xA1}, {'D', 0xA1}, {'e', 0x86}, {'E', 0x86},
  {'f', 0x8E}, {'F', 0x8E}, {'g', 0x90}, {'G', 0xC2},
  {'h', 0x89}, {'H', 0x89}, {'i', 0xFB}, {'I', 0xCF},
  {'j', 0xF1}, {'J', 0xF1}, {'l', 0xC7}, {'L', 0xC7},
  {'n', 0xAB}, {'N', 0xC8}, {'o', 0xA3}, {'O', 0xC0},
  {'p', 0x8C}, {'P', 0x8C}, {'q', 0x98}, {'Q', 0x98},
  {'r', 0xAF}, {'R', 0x88}, {'s', 0x92}, {'S', 0x92},
  {'t', 0x87}, {'T', 0xCE}, {'u', 0xC1}, {'U', 0xC1},
  {'y', 0x91}, {'Y', 0x91}, {'z', 0xA4}, {'Z', 0xA4},
  {':', 0x7F}, {'-', 0xBF}, {'_', 0xF7}, {' ', 0xFF},
};
#define FP_TABLE_SIZE sizeof(fp_bitmap) / sizeof(led_bitmap_t)

static BOOL hight_light = FALSE;
static u32 g_bitmap[8] = {0};

#define FP_START_WORK 0
#define FP_STOP_WORK  1

static const pan_hw_info_t t_pan_info =
{
  PAN_SCAN_SLOT,  /*type_scan, */
  0,              /*type_lbd,  0: Stand-alone LBD, 1: LBD in shifter */
  0,              /*type_com, 0: Stand-alone COM, 1: COM in shifter*/
  0,              /*num_scan, Number of scan PIN, 0: no scan; <= 2 */
  4,              /*num_com,  Number of COM PIN, 0: no com; <= 8 */
  8,              /*pos_colon, Position of colon flag, 0 to 7, 8 no colon */
  8,              /*special control byte: bit 0: revert the data sequence or
                     not
                     bit 1: HT1628 special cmd, bit 2: GPIO control lbd or not
                    */
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*flatch*/
  {GPIO_LEVEL_HIGH, GPIO_DIR_OUTPUT, 25},  /*fclock*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 24},   /*fdata*/
  {{GPIO_LEVEL_HIGH, GPIO_DIR_INPUT, 63},    /*scan[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}},  /*scan[1]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[3]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[4]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[5]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[6]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*com[7]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 0x27},  /*lbd[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*lbd[3]*/
  5,       /* com_scan_intv, scan com digital interval in ms, */
  300,     /* repeat_intv, Repeat interval in ms, for rpt key only */
};

static const led_bitmap_t t_fp_bitmap[] =
{
  {'.', 0x01},
  {'0', 0xFC}, {'1', 0x60}, {'2', 0xDA}, {'3', 0xF2},
  {'4', 0x66}, {'5', 0xB6}, {'6', 0xBE}, {'7', 0xE0},
  {'8', 0xFE}, {'9', 0xF6}, {'a', 0xEE}, {'A', 0xEE},
  {'b', 0x3E}, {'B', 0x3E}, {'c', 0x9C}, {'C', 0x9C},
  {'d', 0x7A}, {'D', 0x7A}, {'e', 0x9E}, {'E', 0x9E},
  {'f', 0x8E}, {'F', 0x8E}, {'g', 0xF6}, {'G', 0xBC},
  {'h', 0x6E}, {'H', 0x6E}, {'i', 0x20}, {'I', 0x0C},
  {'j', 0x70}, {'J', 0x70}, {'l', 0x1C}, {'L', 0x1C},
  {'n', 0x2A}, {'N', 0xEC}, {'o', 0x3A}, {'O', 0xFC},
  {'p', 0xCE}, {'P', 0xCE}, {'q', 0xE6}, {'Q', 0xE6},
  {'r', 0x0A}, {'R', 0xEE}, {'s', 0xB6}, {'S', 0xB6},
  {'t', 0x1E}, {'T', 0x8C}, {'u', 0x7C}, {'U', 0x7C},
  {'y', 0x76}, {'Y', 0x76}, {'z', 0xDA}, {'Z', 0xDA},
  {':', 0x01}, {'-', 0x02}, {'_', 0x10}, {' ', 0x00},
};

#define T_FP_TABLE_SIZE sizeof(t_fp_bitmap) / sizeof(led_bitmap_t)

void init_diamond_cfg()
{
  uio_state = UIO_NONE;

  memset((u8 *)&diamond_cfg, 0, sizeof(diamond_cfg_t));
}


static void diamond_fp_enable(BOOL is_enable)
{
return;
#if 0
  uio_device_t *p_uio = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  if(p_uio != NULL)
  {
    if(is_enable)
    {
      uio_tmp_ctl(p_uio, FP_START_WORK);
    }
    else
    {
      uio_tmp_ctl(p_uio, FP_STOP_WORK);
    }
  }
  #endif
}

static void send_data(u8 *buff, u32 len)
{
  u32 i;
  u32 sr = 0;

  MT_ASSERT(NULL != buff);

  mtos_critical_enter(&sr);
  for(i = 0; i < len; i++)
  {
    uart_write_byte(0, buff[i]);
  }
  mtos_critical_exit(sr);
}

static s32 read_data(u8 *buff, u32 len, u32 timeout)
{
  u32 i;
  MT_ASSERT(NULL != buff);

  for(i = 0; i < len; i++)
  {
    if(SUCCESS != uart_read_byte_polling(0, &buff[i], timeout))
    {
      return ERR_FAILURE;
    }
  }  
  return SUCCESS;
}

static void flush_data()
{
  uart_flush(0);
}


/******************************************
   FUNCTION NAME:cmd_response
   COMMENT:  send response code to pc
 ******************************************/
static void cmd_response(diamond_response_t type,
                         diamond_status_t sts,
                         diamond_error_code_t code)
{
  u8 len = 0;
  u8 tmp[4] = {0, };

  tmp[0] = type;
  tmp[1] = sts;
  tmp[2] = code;
  len = 3;

  send_data(tmp, len);
}

static u32 get_dword(u8 *buff)
{
  return buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24);
}


static u8 diamond_get_next_pos(u8 curn_pos)
{
  if(curn_pos == 0x7)
  {
    return 0x10;
  }
  else if(curn_pos == 0x17)
  {
    return 0x20;
  }
  else if(curn_pos == 0x27)
  {
    return 0x30;
  }
  else if(curn_pos == 0x37)
  {
    return 0x0;
  }

  return ++curn_pos;
}


static u8 diamond_get_prev_pos(u8 curn_pos)
{
  if(curn_pos == 0x0)
  {
    return 0x37;
  }
  else if(curn_pos == 0x10)
  {
    return 0x7;
  }
  else if(curn_pos == 0x20)
  {
    return 0x17;
  }
  else if(curn_pos == 0x30)
  {
    return 0x27;
  }

  return --curn_pos;
}


static lock_test_t wait_for_user_check(void)
{
  lock_test_t e_ret = LOCK_TEST_SWITCH;

  switch(g_last_cmd_id)
  {
    case DM_CMD_CONFIRM:
      e_ret = LOCK_TEST_OK;
      break;
    case DM_CMD_MOVE_LEFT:
      e_ret = LOCK_TEST_L;
      break;
    case DM_CMD_MOVE_RIGHT:
      e_ret = LOCK_TEST_R;
      break;
    default:
      break;
  }
  g_last_cmd_id = DM_CMD_END;

  return e_ret;
}

static s32 set_pan_info(diamond_cfg_t *p_diamond)
{
  RET_CODE ret;  
  u32 reg= 0 , val = 0;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};  

  if(gp_uio_dev != NULL)
  {
      ret = dev_close(gp_uio_dev);
      if(ret != SUCCESS)
      {
          cmd_response(DM_RES_ERR, dm_status, 0);
          return ERR_FAILURE;
      }
  }
  
  fpcfg.p_info = &(p_diamond->hw_cfg.pan_info);
  //fpcfg.map_size =p_diamond->hw_cfg.led_num;//hw_cfg.led_num;
  fpcfg.fp_type = p_diamond->hw_cfg.fp_type;//hw_cfg.fp_type;
  // PINMUX
  switch (fpcfg.fp_type)
  {
   case HAL_CT1642:
   case HAL_LM8168:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0x7 << 27);
     val |= 0x5 << 27;
     *((volatile u32 *)reg) = val;
  
     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFFF << 16);
     val |= 0x444 << 16;
     *((volatile u32 *)reg) = val;
     break;
     
   case HAL_FD650:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val |= 0x3 << 27;
     *((volatile u32 *)reg) = val;
  
     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFF << 16);
     val |= 0x33 << 16;
     *((volatile u32 *)reg) = val;
     
     p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
     MT_ASSERT(NULL != p_dev_i2cfp);
     break;
     
    case HAL_GPIO:
   {
     u32 temp =0;
     temp = *((volatile u32 *)0xbf156000);       
     temp &= ~(0x3 << 0);
     temp &= ~(0x3 << 4);
     temp &= ~(0x3 << 8);
     temp &= ~(0x3 << 12);
     temp &= ~(0x7 << 16);
     temp &= ~(0x7 << 20);
     temp &= ~(0x7 << 24);
     temp |= (0x2 << 0);
     temp |= (0x2 << 4);
     temp |= (0x2 << 8);
     temp |= (0x2 << 12);
     temp |= (0x2 << 16);
     temp |= (0x2 << 20);
     temp |= (0x2 << 24);  
     *((volatile u32 *)0xbf156000)=temp; 
     temp = *((volatile u32 *)0xbf156000);     
   
   
     temp = *((volatile u32 *)0xbf15630c);      
     temp |= (0x1 << 23);
     temp |= (0x1 << 24);
     temp |= (0x1 << 25);
     temp |= (0x1 << 26);
     temp |= (0x1 << 27);
     temp |= (0x1 << 28);
     temp |= (0x1 << 29);
     *((volatile u32 *)0xbf15630c)=temp;
     temp = *((volatile u32 *)0xbf15630c);     
       
     temp = *((volatile u32 *)0xbf0a002c);
     temp |= (0xff << 0);
     temp &= ~(0x1 << 4);
     *((volatile u32 *)0xbf0a002c)=temp; 
     temp = *((volatile u32 *)0xbf0a002c);
     
     temp = *((volatile u32 *)0xbf0a0024);
     temp |= (0xff << 0);
     temp &= ~(0x1 << 4);
     *((volatile u32 *)0xbf0a0024)=temp;   
     temp = *((volatile u32 *)0xbf0a0024);
   
   
     //set AO_GPIO MODE to int
     temp = *((volatile u32 *)0xBF0A0120);
     *((volatile u32 *)0xBF0A0120)=0;  
     }
       break;

  default:
     break;
  }
  gp_uio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != gp_uio_dev);
  
  if (NULL != p_dev_i2cfp)
  {
    fpcfg.p_bus_dev = p_dev_i2cfp;
  }  
  memset(&irdacfg, 0, sizeof(irdacfg));
  irdacfg.protocol = IRDA_NEC;
  irdacfg.code_mode = OUR_DEF_MODE;
  irdacfg.irda_repeat_time = 300;
  
  irdacfg.irda_wfilt_channel = 1;
  irdacfg.irda_wfilt_channel_cfg[0].protocol = IRDA_NEC;
  irdacfg.irda_wfilt_channel_cfg[0].addr_len = 32;    
  irdacfg.irda_wfilt_channel_cfg[0].wfilt_code = 0x77480AF5;
  
  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  ret = dev_open(gp_uio_dev, &uiocfg);
  if(ret != SUCCESS)
  {
      cmd_response(DM_RES_ERR, dm_status, 0);
      return ERR_FAILURE;
  }  
  cmd_response(DM_RES_OK, 0, 0);

  if(fpcfg.fp_type != HAL_GPIO )
  {
      uio_display(gp_uio_dev, " OFF", 3);
  }
  else
  {
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, 1);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, 1);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_COLON, 1);
  }
  return SUCCESS;
}

static unsigned char revert(unsigned char input)
{
  unsigned char out = 0;
  int i;

  for(i = 0; i < 8; i++)
  {
    if(g_bitmap[i] >= i)
    {
      out |= (input & (1 << i)) << (g_bitmap[i] - i);
    }
    else
    {
      out |= (input & (1 << i)) >> (i - g_bitmap[i]);
    }
  }
  if(hight_light)
  {
    return ~out;
  }
  else
  {
    return out;
  }
}


/***********************************************
   FUNCTION NAME: uio_get_key
   COMMENT: 1.get ir key code and usr code
  ************************************************/
static BOOL uio_get_key(u16 *p_key, u16 *p_usr_code)
{
  u16 code = 0;
  u16 usr_code = 0;
  u8 tmp = 0;
  gp_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  if(NULL == gp_uio_dev)
  {
    return FALSE;
  }

  while(1)
  {
     tmp = 0;
     read_data(&tmp, 1, 50);  //0xFFFFFFFF
      if(DM_INDICATOR1 == tmp)
      {
          dm_status = DM_ST_IDLE;
          return FALSE;
      }
    if(uio_get_code(gp_uio_dev, &code) == SUCCESS)
    {
      *p_key = code;
      *p_key &= 0xff;

      if((code & 0x0100) != 0)  /*front panel key*/
      {
        usr_code = 0;
      }
      else  /*ir key*/
      {
        dev_io_ctrl(gp_uio_dev, UIO_IR_GET_USERCODE, (u32) & usr_code);
      }
      if((usr_code == 0) && (*p_key == 0xfe) )
      {
          continue;
      }

      *p_usr_code = usr_code;
     return TRUE;
    }
  }

  return FALSE;
}


/***********************************************
   FUNCTION NAME: diamond_send_back
   COMMENT: 1.send date back to pc
  ************************************************/
static void diamond_send_back(void *p_buff, u32 size)
{
  u32 crc = CRC_INIT;
  u8 cmd[10] = {0};
  u8 ack = 0;
  u8 send_counter = 0;

  while(1)
  {
    flush_data();

    mtos_task_delay_ms(20);

    //send indicator
    cmd[0] = DM_INDICATOR1;           //data packet start indicator1: '#'
    cmd[1] = DM_INDICATOR2;           //data packet start indicator2: '*'
    send_data(cmd, 2);

    //send data
    send_data(p_buff, size);

    //send crc
    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, p_buff, size);
    send_data((u8 *)&crc, 4);  //send crc

    //get response
    if(SUCCESS == read_data(&ack, 1, 3000))
    {
      if(ack == DM_RES_ACK)
      {
        break;
      }
    }
    send_counter++;
    if(send_counter > 5)
    {
      cmd_response(DM_RES_ERR, dm_status, DM_ERR_TIMEOUT);
      break;
    }
  }
}


//format: ID+len+flag('c'=check crc, else ignore crc)
static BOOL diamond_set(void)
{
  if(SUCCESS == set_pan_info(&diamond_cfg))
  {
    return TRUE;
  }

  return FALSE;
}


static s32 diamond_test(void)
{
  u32 len;

  len = cmd_buff[1];
  send_data(&cmd_buff[2], len);

  return SUCCESS;
}


static void diamond_get_data(void)
{
  diamond_send_back(&diamond_cfg, sizeof(diamond_cfg_t));
}


//format:id + len + block_id + zipflag + size + crc (for all data)
//    zipflag = 'Z': zipped data(only available for upg all, block_id = 0xFF),
//    zipfleg = other char, normal data
static void diamond_set_data(void)
{
  u32 data_size = get_dword(&cmd_buff[2]);
  u8 start[2] = {0};
  u32 crc = 0;
  u32 cal_crc = 0;
  diamond_cfg_t diamond;

  if(data_size == sizeof(diamond_cfg_t))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_DATA, DM_ERR_DATA_SIZE);

    return;
  }

  //get packet start indicator
  diamond_fp_enable(FALSE);

  start[0] = 0;      //start indecator 1
  start[1] = 0;      //start indecator 2
  flush_data();
  if((SUCCESS != read_data(start, 2, 4000))
    || (DM_INDICATOR1 != start[0])
    || (DM_INDICATOR2 != start[1]))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_DATA, DM_ERR_INDICATOR);
    diamond_fp_enable(TRUE);
    return;
  }

  //get data
  if(SUCCESS != read_data((u8 *)&diamond, sizeof(diamond_cfg_t), 4000))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_DATA, DM_ERR_TIMEOUT);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }

  //get crc
  if(SUCCESS != read_data((u8 *)&crc, 4, 4000))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_DATA, DM_ERR_TIMEOUT);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }

  //check crc
  cal_crc = crc_fast_calculate(CRC_MODE, CRC_INIT, (u8 *)&diamond, sizeof(diamond_cfg_t));
  if(crc != cal_crc)
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_DATA, DM_ERR_CRC);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }


  diamond_fp_enable(TRUE);

  memcpy(&diamond_cfg, &diamond, sizeof(diamond_cfg_t));

  //response ok.
  cmd_response(DM_RES_OK, DM_ST_SET_DATA, DM_ERR_NONE);

  return;
}


//format:id + len + block_id + zipflag + size + crc (for all data)
//    zipflag = 'Z': zipped data(only available for upg all, block_id = 0xFF),
//    zipfleg = other char, normal data
static void diamond_set_hw()
{
  u32 data_size = get_dword(&cmd_buff[2]);
  u8 start[2] = {0};
  u32 crc = 0;
  u32 cal_crc = 0;
  hw_cfg_t hw_cfg = {0};
  if(data_size == sizeof(hw_cfg_t))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_DATA_SIZE);
    return;
  }
   diamond_fp_enable(FALSE);

  //get packet start indicator
  start[0] = 0;        //start indecator 1
  start[1] = 0;        //start indecator 2
  flush_data();

  if((SUCCESS != read_data(start, 2, 4000))
    || (DM_INDICATOR1 != start[0])
    || (DM_INDICATOR2 != start[1]))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_INDICATOR);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }
  //get data
  if(SUCCESS != read_data((u8 *)&hw_cfg, sizeof(hw_cfg_t), 4000))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_TIMEOUT);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }

  //get crc
  if(SUCCESS != read_data((u8 *)&crc, 4, 4000))
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_TIMEOUT);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }

  //check crc
  cal_crc = crc_fast_calculate(CRC_MODE, CRC_INIT, (u8 *)&hw_cfg, sizeof(hw_cfg_t));
  if(crc != cal_crc)
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_CRC);
    flush_data();
    diamond_fp_enable(TRUE);
    return;
  }

  diamond_fp_enable(TRUE);

  memcpy(&diamond_cfg.hw_cfg, &hw_cfg, sizeof(hw_cfg_t));

  //try to config project.
  if(diamond_set())
  {
    cmd_response(DM_RES_OK, DM_ST_SET_HW, DM_ERR_NONE);
  }
  else
  {
    cmd_response(DM_RES_ERR, DM_ST_SET_HW, DM_ERR_NONE);
  }
  return;
}


static BOOL diamond_led_map_check(led_map_status_t e_usr_res)
{
  BOOL is_light = FALSE;
  u32 i = 0;
  u8 pos = 0;
  u8 led_seg = 0;

  switch(g_ledmap_status)
  {
    case DM_LEDMAP_ST_ON:
      hight_light = (e_usr_res == DM_LEDMAP_ST_ON) ?  TRUE : FALSE;
      g_ledmap_status = DM_LEDMAP_ST_OFF;
      break;
    case DM_LEDMAP_ST_OFF:
      is_light = (e_usr_res == DM_LEDMAP_ST_OFF) ?  FALSE : TRUE;
      if(!((!is_light
           && hight_light)
          || (is_light
             && !hight_light)))
      {
        cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
        return FALSE;
      }
      if(!hight_light)
      {
        diamond_fp_enable(FALSE);
        uio_set_bitmap(gp_uio_dev, fp_bitmap_test1, FP_TABLE_SIZE_TEST1);
        diamond_fp_enable(TRUE);
      }
      g_ledmap_status = DM_LEDMAP_ST_SEG0;
      break;
    case DM_LEDMAP_ST_SEG0:
    case DM_LEDMAP_ST_SEG1:
    case DM_LEDMAP_ST_SEG2:
    case DM_LEDMAP_ST_SEG3:
    case DM_LEDMAP_ST_SEG4:
    case DM_LEDMAP_ST_SEG5:
    case DM_LEDMAP_ST_SEG6:
    case DM_LEDMAP_ST_SEG7:
      pos = e_usr_res - DM_LEDMAP_ST_SEG0;
      led_seg = g_ledmap_status - DM_LEDMAP_ST_SEG0;
      if((pos > 7)
        || (led_seg > 7))
      {
        cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
        return FALSE;
      }
      g_bitmap[pos] = led_seg;
      g_ledmap_status++;

      if(DM_LEDMAP_ST_MAX == g_ledmap_status)
      {
        for(i = 0; i < sizeof(fp_bitmap_benchmark) / sizeof(led_bitmap_t); i++)
        {
          fp_bitmap[i].bitmap = revert(fp_bitmap_benchmark[i].bitmap);
        }

        diamond_fp_enable(FALSE);
        uio_set_bitmap(gp_uio_dev, fp_bitmap, FP_TABLE_SIZE);
        diamond_fp_enable(TRUE);
        memcpy(&diamond_cfg.hw_cfg.led_bitmap, &fp_bitmap, FP_TABLE_SIZE * sizeof(led_bitmap_t));
        diamond_cfg.hw_cfg.led_num = FP_TABLE_SIZE;
        cmd_response(DM_RES_OK, dm_status, DM_ERR_NONE);

        //send led_bitmap back.
        diamond_send_back(&diamond_cfg.hw_cfg.led_bitmap,
                          MAX_LED_MAP_NUM * sizeof(led_bitmap_t));
        return FALSE;
      }
      break;
    default:
      cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
      return FALSE;
  }
  dm_status = DM_ST_GET_LED_MAP;
  cmd_response(DM_RES_OK, dm_status, DM_ERR_NONE);
  return TRUE;
}


static BOOL diamond_light_all(u8 light_cmd)
{
  u8 let_content_on[4] = {'8', '8', '8', '8'};
  u8 let_content_off[4] = {' ', ' ', ' ', ' '};

  BOOL b_onoff = TRUE;

  if(NULL == gp_uio_dev)
  {
    cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
    return FALSE;
  }

  if(light_cmd == DM_LEDMAP_ST_ON)
  {
      uio_display(gp_uio_dev, let_content_on, diamond_cfg.hw_cfg.led_num);
      b_onoff = TRUE;
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, b_onoff);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, b_onoff);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_COLON, b_onoff);
       mtos_task_delay_ms(1);
  }
  else
  {
      uio_display(gp_uio_dev, let_content_off, diamond_cfg.hw_cfg.led_num);
      b_onoff = FALSE;
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, b_onoff);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, b_onoff);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_COLON, b_onoff);
      mtos_task_delay_ms(1);
  }

  return TRUE;
}


static BOOL diamond_get_led_map(void)
{
  u8 let_content_on[4] = {'8', '8', '8', '8'};
  u8 let_content_off[4] = {' ', ' ', ' ', ' '};
  u8 tmp[4];

  if(NULL == gp_uio_dev)
  {
    cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
    return FALSE;
  }

  switch(g_ledmap_status)
  {
    case DM_LEDMAP_ST_ON:
      diamond_fp_enable(FALSE);
      uio_set_bitmap(gp_uio_dev, fp_bitmap_test, FP_TABLE_SIZE_TEST);
      diamond_fp_enable(TRUE);
      uio_display(gp_uio_dev, let_content_on, diamond_cfg.hw_cfg.led_num);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, TRUE);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, TRUE);
      mtos_task_delay_ms(1);
      break;
    case DM_LEDMAP_ST_OFF:
      uio_display(gp_uio_dev, let_content_off, diamond_cfg.hw_cfg.led_num);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, FALSE);
      mtos_task_delay_ms(1);

      break;
    case DM_LEDMAP_ST_SEG0:
    case DM_LEDMAP_ST_SEG1:
    case DM_LEDMAP_ST_SEG2:
    case DM_LEDMAP_ST_SEG3:
    case DM_LEDMAP_ST_SEG4:
    case DM_LEDMAP_ST_SEG5:
    case DM_LEDMAP_ST_SEG6:
    case DM_LEDMAP_ST_SEG7:
      tmp[0] = '0' + g_ledmap_status - DM_LEDMAP_ST_SEG0;
      tmp[1] = tmp[0];
      tmp[2] = tmp[0];
      tmp[3] = tmp[0];
      uio_display(gp_uio_dev, tmp, diamond_cfg.hw_cfg.led_num);
      mtos_task_delay_ms(1);

      break;
    default:
      cmd_response(DM_RES_ERR, dm_status, DM_ERR_CMD);
      return FALSE;
  }
  dm_status = DM_ST_WAIT_USER_ACK;

  return TRUE;
}


static BOOL diamond_test_led(void)
{
  static u8 count_tmp = 0;
  static BOOL b_onoff = 0;
  lock_test_t lock_ret = LOCK_TEST_SWITCH;

  if(NULL == gp_uio_dev)
  {
    return FALSE;
  }
  count_tmp++;
  if((count_tmp % 4) == 0)
  {
    b_onoff = ~b_onoff;
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, b_onoff);
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_POWER_LBD, FALSE);
    mtos_task_delay_ms(1);
  }

  lock_ret = wait_for_user_check();

  if(lock_ret == LOCK_TEST_OK)
  {
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
    mtos_task_delay_ms(1);
    cmd_response(DM_RES_OK,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos);
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, TRUE);
    diamond_cfg.hw_cfg.pan_info.lbd[0].pos = g_pre_lbd_pos;
    dm_status = DM_ST_IDLE;
    mtos_task_delay_ms(1);
  }
  else if(lock_ret == LOCK_TEST_L)
  {
     if(bFirst)
     {
         g_pre_led_pos =1;
         bFirst =FALSE;
     }
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
    mtos_task_delay_ms(1);
    g_pre_led_pos--;
    if(g_pre_led_pos == 0)
    {
        g_pre_led_pos = diamond_cfg.hw_cfg.led_num;
        uio_display(gp_uio_dev, "8    ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 1)
    {
        uio_display(gp_uio_dev, " 8  ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 2)
    {
        uio_display(gp_uio_dev, "  8 ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 3)
    {
        if(diamond_cfg.hw_cfg.led_num == 3)
        uio_display(gp_uio_dev, "  8", diamond_cfg.hw_cfg.led_num);
        else
        uio_display(gp_uio_dev, "   8", diamond_cfg.hw_cfg.led_num);
    }
    diamond_cfg.hw_cfg.pan_info.lbd[0].pos = diamond_get_prev_pos(
    diamond_cfg.hw_cfg.pan_info.lbd[0].pos);
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, TRUE);
    cmd_response(DM_RES_OK,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos);
    mtos_task_delay_ms(1);
  }
  else if(lock_ret == LOCK_TEST_R)
  {
     if(bFirst)
     {
         g_pre_led_pos =diamond_cfg.hw_cfg.led_num-1;
         bFirst =FALSE;
     }
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
    g_pre_led_pos++;
    if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 0)
    {
        uio_display(gp_uio_dev, "8   ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 1)
    {
        uio_display(gp_uio_dev, " 8  ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 2)
    {
        uio_display(gp_uio_dev, "  8 ", diamond_cfg.hw_cfg.led_num);
    }
    else if(g_pre_led_pos%(diamond_cfg.hw_cfg.led_num) == 3)
    {
         g_pre_led_pos =diamond_cfg.hw_cfg.led_num-1;
        if(diamond_cfg.hw_cfg.led_num == 3)
        uio_display(gp_uio_dev, "  8", diamond_cfg.hw_cfg.led_num);
        else
        uio_display(gp_uio_dev, "   8", diamond_cfg.hw_cfg.led_num);
    }

    diamond_cfg.hw_cfg.pan_info.lbd[0].pos = diamond_get_next_pos(
      diamond_cfg.hw_cfg.pan_info.lbd[0].pos);
    dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, TRUE);

    cmd_response(DM_RES_OK,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos,
                 diamond_cfg.hw_cfg.pan_info.lbd[0].pos);
    mtos_task_delay_ms(1);
  }
  return TRUE;
}


/***********************************************
   FUNCTION NAME: diamond_get_key
   COMMENT: 1.get the ir code and front pannel code
                2.sent code to uart
                the front pannel key code is "0"
  ************************************************/
static void diamond_get_key(void)
{
  u16 code = 0, usr_code = 0;
  u32 data = 0;

  //get key
  if(uio_get_key(&code, &usr_code))
  {
    //send command to pc.
    data = (code << 16 | usr_code);
    send_data((u8 *)&data, 4);
  }
}


/***********************************************
   FUNCTION NAME: get_cmd
   COMMENT: 1.get cmd form uart
            2.cmd format: #(1) + ID(1) + LEN(1) + data(LEN) + crc(4)
  ************************************************/
void get_cmd(void)
{
  u8 tmp = 0, cmd_id = 0, cmd_len = 0;
  u8 t[4] = {0, };
  u32 i = 0, crc = 0, rcrc;

  while(1)
  {
    memset(cmd_buff, 0, sizeof(cmd_buff));

    //wait for new command
    while(1)
    {
      tmp = 0;
      read_data(&tmp, 1, 100);  //0xFFFFFFFF
      if(DM_INDICATOR1 == tmp)
      {
        break;
      }
      else if(dm_status != DM_ST_IDLE) /*same cmd is processing*/
      {
         return;
      }
    }
    
    i = 0;
    if((SUCCESS != read_data(&cmd_id, 1, 1000))
      || ((cmd_id < DM_CMD_TEST)
      || (cmd_id >= DM_CMD_END)))                        //get cmd id
    {
      OS_PRINTK("\r\n*%s:get cmd_id error[0x%x]", __FUNCTION__, cmd_id);
      continue;
    }
    cmd_buff[i++] = cmd_id;

    if((SUCCESS != read_data(&cmd_len, 1, 1000))
      || (cmd_len >= DM_CMD_MAX_LEN))                    //get cmd length
    {
      OS_PRINTK("\r\n*%s:get cmd_len error[0x%x]", __FUNCTION__, cmd_len);
      continue;
    }
    cmd_buff[i++] = cmd_len;

    if(SUCCESS != read_data(&cmd_buff[i], cmd_len, 1000))  //get data
    {
      OS_PRINTK("\r\n*%s:get data error", __FUNCTION__);
      continue;
    }

    if(SUCCESS != read_data(t, 4, 1000))  //get crc
    {
      OS_PRINTK("\r\n*%s:get crc error", __FUNCTION__);
      continue;
    }

    rcrc = get_dword(t);

    crc = crc_fast_calculate(CRC_MODE, CRC_INIT, cmd_buff, cmd_len + 2);
    if(crc != rcrc)
    {
      OS_PRINTK("\r\n*%s:crc is wrong crc[0x%x],rcrc[0x%x]", __FUNCTION__, crc, rcrc);
      continue;
    }
    else
    {
      cmd_response(DM_RES_OK, 0, 0);
    }
    break;
  }
}


void dispatch_cmd()
{
  u8 led_content[5] = {' ', ' ', ' ', ' ', };

  if(DM_CMD_NONE == cmd_buff[0])
  {
    /*no cmd, no need dispatch*/
    return;
  }
  g_last_cmd_id = cmd_buff[0];

  switch(cmd_buff[0])
  {
    case DM_CMD_TEST:
      dm_status = DM_ST_TEST;
      break;
    case DM_CMD_SET_DATA:
      dm_status = DM_ST_SET_DATA;
      break;
    case DM_CMD_GET_DATA:
      dm_status = DM_ST_GET_DATA;
      break;
    case DM_CMD_GET_KEY:
      dm_status = DM_ST_GET_KEY;

      gp_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
      if(gp_uio_dev == NULL)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        dm_status = DM_ST_IDLE;
        return;
      }
      break;
    case DM_CMD_EXIT_KEY:
      dm_status = DM_ST_IDLE;
      break;
    case DM_CMD_SET_HW:
      dm_status = DM_ST_SET_HW;
      break;
    case DM_CMD_GET_LED_MAP:
      dm_status = DM_ST_GET_LED_MAP;
      gp_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
      if(gp_uio_dev == NULL)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        dm_status = DM_ST_IDLE;
        return;
      }
      g_ledmap_status = DM_LEDMAP_ST_ON;
      break;
    case DM_CMD_LED_MAP_ACK:
      if(DM_ST_WAIT_USER_ACK != dm_status)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        dm_status = DM_ST_IDLE;
        return;
      }
      dm_status = DM_ST_LED_ACK_CHECK;
      break;
    case DM_CMD_TEST_LED:
      dm_status = DM_ST_TEST_LED;
      bFirst = TRUE;
      gp_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
      if(gp_uio_dev == NULL)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        dm_status = DM_ST_IDLE;
        return;
      }
      uio_display(gp_uio_dev, led_content, diamond_cfg.hw_cfg.led_num);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
      mtos_task_delay_ms(1);
      g_pre_lbd_pos = diamond_cfg.hw_cfg.pan_info.lbd[0].pos;
      diamond_cfg.hw_cfg.pan_info.lbd[0].pos = 0;
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, TRUE);
      mtos_task_delay_ms(1);
      break;
    case DM_CMD_EXIT_LED:
      diamond_cfg.hw_cfg.pan_info.lbd[0].pos = g_pre_lbd_pos;
      uio_display(gp_uio_dev, led_content, 4);
      dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
      gp_uio_dev = NULL;
      dm_status = DM_ST_IDLE;
      mtos_task_delay_ms(1);
      break;
    case DM_CMD_CONFIRM:
    case DM_CMD_MOVE_LEFT:
    case DM_CMD_MOVE_RIGHT:
      if(DM_ST_TEST_LED != dm_status)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        return;
      }
      break;
    case DM_CMD_LIGHT_ALL:
      gp_uio_dev = (uio_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
      if(gp_uio_dev == NULL)
      {
        cmd_response(DM_RES_ERR, dm_status, 0);
        dm_status = DM_ST_IDLE;
        return;
      }
      dm_status = DM_ST_LIGHT_ALL;
      break;
    default:
      cmd_response(DM_RES_UNKNOWN, 0, 0);
      return;
  }

  /**get cmd ok**/
  cmd_response(DM_RES_OK, dm_status, DM_ERR_NONE);
  return;
}


void app_proc()
{
  OS_PRINTK("enter app proc\n");

  crc_setup_fast_lut(CRC_MODE);

  init_diamond_cfg();

  uart_ioctl(UART_CMD_EN_INTERRUPT, FALSE, 0);

  OS_PRINTK("enter app proc end\n");

  mtos_close_printk();
  while(1)
  {
    get_cmd(); 
    dispatch_cmd();

    switch(dm_status)
    {
      case DM_ST_IDLE:
        break;

      case DM_ST_TEST:
        diamond_test();
        break;

      case DM_ST_GET_KEY:
        diamond_get_key();
        continue;  //break;

      case DM_ST_SET_HW:
        diamond_set_hw();
        break;

      case DM_ST_GET_LED_MAP:
        if(diamond_get_led_map())
        {
          continue;
        }
        break;

      case DM_ST_WAIT_USER_ACK:
        continue;

      case DM_ST_LED_ACK_CHECK:
        if(diamond_led_map_check((led_map_status_t)cmd_buff[2]))
        {
          continue;
        }
        if(gp_uio_dev!=NULL)
        {
          uio_display(gp_uio_dev, "", 4);
          dev_io_ctrl(gp_uio_dev, UIO_FP_SET_LOCK_LBD, FALSE);
        }
        mtos_task_delay_ms(1);
        break;

      case DM_ST_SET_DATA:
        diamond_set_data();
        break;

      case DM_ST_GET_DATA:
        diamond_get_data();
        break;

      case DM_ST_TEST_LED:
        if(diamond_test_led())
        {
          continue;
        }
        break;

      case DM_ST_LIGHT_ALL:
        diamond_light_all(cmd_buff[2]);
        break;

      default:
        break;
    }
    dm_status = DM_ST_IDLE;
  }
  return;
}


void drv_init()
{
  RET_CODE ret;  
  ret = uio_attach_concerto("concerto");
  MT_ASSERT(ret == SUCCESS);  
  i2c_cfg_t i2c_cfg = {0};
  ret = i2c_concerto_attach("i2c_FP");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
  MT_ASSERT(NULL != p_dev_i2cfp);
  i2c_cfg.i2c_id = 2;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2cfp, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  return ;
}

void ap_init(void)
{
  u32 *p_stack = NULL;
  RET_CODE ret = SUCCESS;

  drv_init();

  p_stack = (u32 *)mtos_malloc(SYS_APP_TASK_STK_SIZE);
  MT_ASSERT(p_stack != NULL);


  ret = mtos_task_create((u8 *)"APP",
                         app_proc,
                         (void *)0,
                         SYS_APP_TASK_PRIORITY,
                         p_stack,
                         SYS_APP_TASK_STK_SIZE);
}

