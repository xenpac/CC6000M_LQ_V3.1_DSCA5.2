/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SMC_BLOCK_H__
#define __SMC_BLOCK_H__

/*!
  *  协议部分相关
  */

/*!
  mask of TAx byte
  */
#define ATR_TAX_MASK           0x10
/*!
  mask of TBx byte
  */
#define ATR_TBX_MASK           0x20
/*!
  mask of TCx byte
  */
#define ATR_TCX_MASK           0x40
/*!
  mask of TDx byte
  */
#define ATR_TDX_MASK           0x80

/*!
  Extra guardtime
  */
#define SMART_N_DEFAULT         2
/*!
  Encodes clockrate conversion
  */
#define SMART_F_DEFAULT           1
/*!
  Encodes bitrate conversion
  */
#define SMART_D_DEFAULT           1


/*!
  maxmium bytes in ATR
  */
#define SMC_MAX_ATR            30
/*!
  minimium bytes in ATR(ATR_TS,ATR_T0)
  */
#define SMC_MIN_ATR             2
/*!
  maximium number of historical bytes
  */
#define SMC_MAX_HIST           15


/*!
  comments
  */
typedef enum
{
  /*!
    commments
  */
  ATR_INVERSE_CONVENTION = 0x3F,
  /*!
    commments
  */
  ATR_DIRECT_CONVENTION  = 0x3B
} smc_bitconvention_t;

/*!
  T1 Protocol
*/
typedef enum
{
  /*!
    commments
  */
  T1_R_BLOCK = 0,
  /*!
    commments
  */
  T1_S_REQUEST,
  /*!
    commments
  */
  T1_S_RESPONSE,
  /*!
    commments
  */
  T1_I_BLOCK,
  /*!
    commments
  */
  T1_CORRUPT_BLOCK
} tblocktype;

/*!
  comments
  */
typedef struct
{
  /*!
    commments
  */
  u8 NAD;
  /*!
    commments
  */
  u8 PCB;
  /*!
    commments
  */
  u8 LEN;

} tt1header;

/*!
  comments
  */
typedef struct
{
  /*!
    commments
  */
  tt1header   Header;
  /*!
    commments
  */
  u8     *pBuffer;
  /*!
    commments
  */
  union
  {
    /*!
      commments
    */
    u8 LRC;
    /*!
      commments
    */
    u16 CRC;
  } EDC;

} tt1block;

/*!
  comments
  */
typedef struct
{
  /*!
    commments
  */
  u8     TxCount;
  /*!
    commments
  */
  BOOL     Aborting;
  /*!
    32
  */
  u8     TxMaxInfoSize;
  /*!
    commments
  */
  u32      State;
  /*!
    commments
  */
  u8      OurSequence;
  /*!
    commments
  */
  u8     TheirSequence;
  /*!
    commments
  */
  BOOL      FirstBlock;
  /*!
    commments
  */
  ERRORCODE_T ErrorCode;

}tt1details;


/*!
  commments
  */
#define SMC_MAX_IOFIFO          260
/*!
  commments
  */
typedef struct
{
  /*!
    commments
  */
  u32 num;
  /*!
    commments
  */
  u8 rx[SMC_MAX_IOFIFO];
  /*!
    commments
  */
  u32 start;
  /*!
    commments
  */
  u32 end;
} tsmcrxio;


/*!
  comments
  */
enum
{
  /*!
    commments
  */
  STATUS_INSERT,
  /*!
    commments
  */
  STATUS_REMOVE,
  /*!
    commments
  */
  STATUS_UNKNOWN
}tsmcstatus;

/*!
  comments
  */
typedef struct
{
  /*!
    commments
  */
  u8    *Name;
  /*!
    commments
  */
  BOOL    Init;
  /*!
    commments
  */
  u16     Irq;
  /*!
    commments
  */
  u8    Status;
  /*!
    Communications buffers
  */
  u32    HistoricalSize;
  /*!
    commments
  */
  u8    HistoricalBytes[SMC_MAX_HIST];
  /*!
    Size of current ATR message
  */
  u32    ATRMsgSize;
  /*!
    commments
  */
  u8    ATRBytes[SMC_MAX_ATR];

  /*!
    Transmit retries
  */
  u8     Retries;

  /*!
    commments
  */
  u16    SupportedProtocolTypes;

  /*!
    ATR's TS value NOTE: see ISO7816-3
  */
  u8    TS;
  /*!
    "F" variables integer represenation
  */
  u8    FInt;
  /*!
    "D" variables integer represenation
  */
  u8    DInt;
  /*!
    Current working protocol type
  */
  u8     WorkingType;
  /*!
    Specific mode of card indicated?
  */
  BOOL     SpecificMode;
  /*!
    Specific protocol type
  */
  u8     SpecificType;
  /*!
    Type changeable?
  */
  BOOL     SpecificTypeChangable;
  /*!
    Card's maximum block size for (T=1)
  */
  u8     IFSC;
  /*!
    "II" variables integer represenation
  */
  u8     IInt;
  /*!
    "PI1" variables integer represenation
  */
  u8     PInt1;
  /*!
    "PI2" variables integer represenation
  */
  u8     PInt2;
  /*!
    "CWT" variables integer represenation
  */
  u8     CWInt;
  /*!
    "BWT" variables integer represenation
  */
  u8     BWInt;
  /*!
    "N" setting
  */
  u16      N;
  /*!
    "WI" variables integer represenation
  */
  u8     WInt;
  /*!
    Redundancy Check used CRC=1 LRC=0 (T=1)
  */
  u8     RC;
  /*!
    Elementary time unit
  */
  u32      WorkETU;
  /*!
    Work waiting time
  */
  u32      WWT;

  /*!
    Initial clock frequency to smartcard
  */
  u32      Fi;
  /*!
    Subsequent clock frequency
  */
  u32      Fs;
  /*!
    Maximum clock frequency
  */
  u32      Fmax;

  /*!
    commments
  */
  tt1details  T1Details;
  /*!
    commments
  */
  os_sem_t UserMutex;
  /*!
    commments
  */
  u8   SmcId;
  /*!
    commments
  */
  u8   vcc_enable_pol;
  /*!
    commments
  */
  void (*SmartInsert)(void);
  /*!
    commments
  */
  void (*SmartRemove)(void);

  /*!
    commments
  */
  tsmcrxio rxfifo;
} tscdevice;

/*!
  comments
  */
typedef enum
{
  /*!
    commments
  */
  SC_T0_PROTOCOL     = 0,
  /*!
    commments
  */
  SC_T1_PROTOCOL     = 1,
  /*!
    commments
  */
  SC_T14_PROTOCOL   = 14,
  /*!
    commments
  */
  SC_UNSUPPORT_PTOTOCOL = 31

}tscprotocol;

/*!
  SC CTL Funs Exported
*/
ERRORCODE_T sc_answertoreset(smc_id_t smc_id, u8 *panswer, 
u32 *panswer_len, BOOL bT14);

/*!
  comments
  */
BOOL sc_getcardstatus(smc_id_t smc_id);

/*!
  comments
  */
ERRORCODE_T sc_rx(smc_id_t smc_id, u8 *pBuf, u32 nNumToRead, u32 * pNumRead);

/*!
  comments
  */
ERRORCODE_T sc_rx_intimeout(smc_id_t smc_id, u8 *pBuf, u32 nNumToRead, 
u32 *pNumRead, u32 Timeout);

/*!
  comments
  */
ERRORCODE_T sc_tx(smc_id_t smc_id, u8 *pBuf, U16 nNumToWrite);

/*!
  comments
  */
ERRORCODE_T sc_intialize(smc_id_t smc_id, u8 phyclk, u8 convt_en, u8 convt_value,
  u8 detect_pin_pol, u8 vcc_enable_pol, u8 iopin_mode, u8 rstpin_mode, u8 clkpin_mode);

/*!
  comments
  */
void sc_flush(smc_id_t smc_id);

/*!
  comments
  */
void sc_getworkingprotocol(smc_id_t smc_id, u8 * pProtocol);

/*!
  comments
  */
BOOL sc_checkhandle(tscdevice * pSCDev);

/*!
  comments
  */
tscdevice * sc_getscdevice(smc_id_t smc_id);

#endif //__SMC_BLOCK_H__


