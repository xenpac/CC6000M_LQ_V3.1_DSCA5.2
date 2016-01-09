/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __FLASH_PARSER_H__
#define __FLASH_PARSER_H__
#include <string>
// system
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_msg.h"
#include "class_factory.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "unzip.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "video.h"
#include "audio.h"
#include "osd.h"
#include "sub.h"
#include "gpe.h"
#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager16.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "ts_packet.h"
#include "eit.h"
//#include "epg_data.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "fcrc.h"
#include "ss_ctrl.h"

#include "db_dvbs.h"
#include "mem_cfg.h"

#include "ap_framework.h"
#include "ap_uio.h"
//#include "ap_flounder_ota.h"
#include "sys_status.h"
#include "mt_time.h"
#define DM_TAG_SIZE 16
#define DM_BASE_INFO_HEAD_LEN 12

class FlashParser
{
public:
  // constructor and desconstructor
  FlashParser();
  ~FlashParser();

  // clear current bin file
  void Clear();

  // load binary file
  BOOL LoadBinFile(const char *szBinFile);
  BOOL LoadBinFile(const wchar_t *wszBinFile);

  // save binary file with specified name
  BOOL SaveBinFile(const char *szNewBinFile);
  BOOL SaveBinFile(const wchar_t *wszNewBinFile);

  // get ss_data block data
  BOOL GetSSData(char *szData, u32 size);

  // get ir_data block data
  BOOL GetIRData(char *szData, u32 size);

  // get misc_options block data
  BOOL GetMiscOptions(char *szData, u32 size);

  // save ss_data block
  BOOL SaveSSData(char *szData, u32 size);

  // save ir_data block
  BOOL SaveIRData(char *szData, u32 size);

  // save misc_options block data
  BOOL SaveMiscOptions(char *szData, u32 size);
  
  // save ss_data block
  BOOL SingleSaveSSData(const wchar_t *wszNewBinFile);

  // save ir_data block
  BOOL SingleSaveIRData(const wchar_t *wszNewBinFile);

  // save misc_options block data
  BOOL SingleSaveMiscOptions(const wchar_t *wszNewBinFile);

private:
  //------------------------------------------------------------------------------------
  u32 find_dm_head_start(int block_id);
  int __get_dm_block_head_info(int block_id, dmh_block_info_t *dest, u32 dm_head_start);
  int get_dm_block_head_info(int block_id, dmh_block_info_t *dest);
  int __update_dm_block_head_info(int block_id, u32 dm_head_start);
  int update_dm_block_head_info(int block_id);
  void crc_setup_fast_lut(DWORD *crc_lut);
  DWORD crc_fast_calculate(DWORD *crc_lut, DWORD crc, unsigned char *buf, int len);
  //------------------------------------------------------------------------------------

private:
  std::string m_sBinFile;
  char *m_pFlashData;
  int m_nFlashSize;
  u32 m_crc_table[256];
};

#endif
