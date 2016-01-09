/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
/*! 
 *******************************************************************************
 * @defgroup  IP_AdvancedCA Advanced Conditional Access
 *  @{
 *  \note project:    sonata
 *  \note chip:     
 *  \file hal_secure.h
 *  \author     Bob Yang
 *  \date       2013/12/12
 *  \version    1.0
 *  \brief      Advanced Conditional Access Module Driver
 *******************************************************************************
 */ 


#ifndef __HAL_SECURE_H__
#define __HAL_SECURE_H__


/* ========================================================================== */
/* header files                     */
/* ========================================================================== */
#include  "sys_types.h"


/* ========================================================================== */
/* Constant Definitions                                                       */
/* ========================================================================== */
/**
 * @brief   advanced ca module constant
 
 *
 *  A data structure used to describe all constants.
 */
enum    _EN_ADVANCEDCA_CONSTANT
{
    ADVANCEDCA_MAXKEYSIZE       = 32,     ///< max key size
    ADVANCEDCA_MAXIVSIZE        = 16,     ///< max IV size
    ADVANCEDCA_HDMIKEYSIZE      = 304,      ///< HDMI key size
    ADVANCEDCA_CHANNELSIZE      = 3,      ///< channel size
    ADVANCEDCA_ARBITRATIONLIMIT = 0x10001,  ///< channel size
    ADVANCEDCA_KEYBLOCK         = 1,
    ADVANCEDCA_LOADBLOCK        = 3,
    ADVANCEDCA_STOREBLOCK       = 4,
};

/**
 * @brief   advanced ca module application
 *
 *  A data structure used to describe all applications.
 */
enum  _EN_ADVANCEDCA_APPLICATION
{
    ADVANCEDCA_APPLICATION_CWDecryption = 0,    ///< CW decryption
    ADVANCEDCA_APPLICATION_PVRKEY       = 1,    ///< PVR key decryption
    ADVANCEDCA_APPLICATION_CADecryption = 2,    ///< CA decryption
    ADVANCEDCA_APPLICATION_PVRPlayback  = 3,    ///< PVR playback
    ADVANCEDCA_APPLICATION_HDMIKey      = 4,    ///< HDMI key decryption
    ADVANCEDCA_APPLICATION_GPDecryption = 5,    ///< general purpose decryption
    ADVANCEDCA_APPLICATION_MemoryDecryption = 6,  ///< Memory decryption
    ADVANCEDCA_APPLICATION_PVR              = 7,  ///< PVR
    ADVANCEDCA_APPLICATION_GPEncryption     = 8,  ///< General-purpose encryption
    ADVANCEDCA_APPLICATION_MemoryEncryption = 9,  ///< Memory encryption
    ADVANCEDCA_APPLICATION_MULTILAYER       = 10, ///< Multi-layer CAS
    ADVANCEDCA_APPLICATION_EncryptCWDec     = 11, ///< Multi-layer CAS
    ADVANCEDCA_APPLICATION_Keyladder            , ///< keyladder 
};

/**
 * @brief   advanced ca module cipher engine
 *
 *  A data structure used to describe all cipher engines.
 */
enum  _EN_ADVANCEDCA_CIPHERENGINE
{
    ADVANCEDCA_CIPHERENGINE_NONE  = 0,    ///< algorithm DES
    ADVANCEDCA_CIPHERENGINE_DES   = 1,    ///< algorithm DES
    ADVANCEDCA_CIPHERENGINE_TDEA  = 2,    ///< algorithm TDEA (Triple-DES)
    ADVANCEDCA_CIPHERENGINE_AES   = 3,    ///< algorithm AES
    ADVANCEDCA_CIPHERENGINE_SHA   = 4,    ///< algorithm SHA
    ADVANCEDCA_CIPHERENGINE_RSA   = 5,    ///< algorithm RSA
    ADVANCEDCA_CIPHERENGINE_XOR   = 6,    ///< algorithm XOR
    ADVANCEDCA_CIPHERENGINE_CSA         = 7,        ///< algorithm CSA
    ADVANCEDCA_CIPHERENGINE_SHA224    ,
    ADVANCEDCA_CIPHERENGINE_SHA256    ,
    ADVANCEDCA_CIPHERENGINE_SHA384    ,
    ADVANCEDCA_CIPHERENGINE_SHA512    ,
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_KEYPATH
{
    ADVANCEDCA_KEYPATH_BLOCK      = 0,    ///< key path from block
    ADVANCEDCA_KEYPATH_OTP        = 1,    ///< key path from OTP
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_ENCRYPT_KEYPATH
{
    ADVANCEDCA_ENCRYPT_KEYPATH_OTP_SK0         = 0,        ///< key path from OTP
    ADVANCEDCA_ENCRYPT_KEYPATH_OTP_SK1         = 1,        ///< key path from OTP
    ADVANCEDCA_ENCRYPT_KEYPATH_OTP_SK2         = 2,        ///< key path from OTP
    ADVANCEDCA_ENCRYPT_KEYPATH_OTP_PVR         = 3,        ///< key path from OTP
 //   ADVANCEDCA_ENCRYPT_KEYPATH_DEFAULT        = 2,        ///< key path from OTP
 //   ADVANCEDCA_ENCRYPT_KEYPATH_CPU            = 0,        ///< key path from block
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_PVRFLAG
{
    ADVANCEDCA_PVRFLAG_CASE1    = 0,    ///< 
    ADVANCEDCA_PVRFLAG_CASE2    = 1,    ///< key path from OTP
    ADVANCEDCA_PVRFLAG_CASE3    = 2,    ///< key path from Block
    ADVANCEDCA_PVRFLAG_CASE4    = 3,    ///< 
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_KEYSIZE
{
    ADVANCEDCA_KEYSIZE_64     = 8,    ///< key size bits 64
    ADVANCEDCA_KEYSIZE_128    = 16,    ///< key size bits 128 
    ADVANCEDCA_KEYSIZE_192    = 24,    ///< key size bits 192 
    ADVANCEDCA_KEYSIZE_256    = 32,    ///< key size bits 256 
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_KEYOPTION
{
    ADVANCEDCA_KEYOPTION_DSK0           = 0 ,        ///< key option DSK0
    ADVANCEDCA_KEYOPTION_DSK1           = 1 ,        ///< key option DSK1
    ADVANCEDCA_KEYOPTION_DSK2           = 2 ,        ///< key option DSK2
    ADVANCEDCA_KEYOPTION_PVR            = 3 ,        ///< key option PVR
    ADVANCEDCA_KEYOPTION_CPU            = 4 ,        ///< key option in CPU mode 
    ADVANCEDCA_KEYOPTION_SV1                ,        // key option for sv1
    ADVANCEDCA_KEYOPTION_SV3                ,        // key option for sv3
    ADVANCEDCA_KEYOPTION_ESMK               ,        // key option for esmk
    ADVANCEDCA_KEYOPTION_SMEM_FKEY          ,        //flash key 
    ADVANCEDCA_KEYOPTION_SMEM_SVC_KLD       ,        //SVC in key ladder mode
    ADVANCEDCA_KEYOPTION_SMEM_SV3_KLD       ,        //SV3 in key ladder mode
    ADVANCEDCA_KEYOPTION_SMEM_ESMK_KLD      ,        //emsk in key ladder mode
    ADVANCEDCA_KEYOPTION_DEFAULT            ,        ///< key option PVR
};

/**
 * @brief   advanced ca module key size used by cipher engine.
 *
 *  A data structure used to describe key size for all cipher engines.
 */
enum  _EN_ADVANCEDCA_BLOCKSIZE
{
    ADVANCEDCA_BLOCKSIZE_64   = 8,    ///< key size bits 64
    ADVANCEDCA_BLOCKSIZE_128  = 16,    ///< key size bits 128 
};

/**
 * @brief   advanced ca module block mode used by cipher engine.
 *
 *  A data structure used to describe block mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_BLOCKMODE
{
    ADVANCEDCA_BLOCKMODE_ECB    = 0,    ///< block cipher mode ECB
    ADVANCEDCA_BLOCKMODE_CBC    = 1,    ///< block cipher mode CBC
    ADVANCEDCA_BLOCKMODE_PCBC   = 2,    ///< block cipher mode PCBC
    ADVANCEDCA_BLOCKMODE_CFB    = 3,    ///< block cipher mode CFB
    ADVANCEDCA_BLOCKMODE_OFB    = 4,    ///< block cipher mode OFB
    ADVANCEDCA_BLOCKMODE_CTR    = 5,    ///< block cipher mode CTR
};

/**
 * @brief   advanced ca module routing mode used by cipher engine.
 *
 *  A data structure used to describe routing mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_ROUTINGMODE
{
    ADVANCEDCA_ROUTINGMODE_BYPASS       = 0,  ///< routing mode bypass
    ADVANCEDCA_ROUTINGMODE_PASSTHROUGH  = 1,    ///< routing mode pass through
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_WAITINGMODE
{
  ADVANCEDCA_WAITINGMODE_POLLING    = 0,    ///< waiting mode polling
  ADVANCEDCA_WAITINGMODE_INTERRUPT  = 1,    ///< waiting mode interrupt
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_CWTYPE
{
  ADVANCEDCA_CWTYPE_ODD     = 0,    ///< waiting mode polling
  ADVANCEDCA_CWTYPE_EVEN    = 1,    ///< waiting mode interrupt
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_CWLOADING
{
  ADVANCEDCA_CWLOADING_OFF    = 0,    ///< waiting mode polling
  ADVANCEDCA_CWLOADING_ON               = 1,        ///< The clr CW loading for pti descramble 
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_CHANNELID
{
  ADVANCEDCA_CHANNELID_CW     = 0,    ///< waiting mode polling
  ADVANCEDCA_CHANNELID_PTI    = 1,    ///< waiting mode interrupt
  ADVANCEDCA_CHANNELID_DMA    = 2,    ///< waiting mode interrupt
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_CHANNELSTATUS
{
  ADVANCEDCA_CHANNELSTATUS_IDLE   = 0,    ///< waiting mode polling
  ADVANCEDCA_CHANNELSTATUS_BUSY   = 1,    ///< waiting mode polling
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_BLOCKINDEX
{
  ADVANCEDCA_BLOCK_0                    = 0,        ///< cant't be written,only use pvr key
  ADVANCEDCA_BLOCK_1                    = 1,        ///< waiting mode polling
  ADVANCEDCA_BLOCK_2                    = 2,        ///< waiting mode polling
  ADVANCEDCA_BLOCK_3                    = 3,        ///< waiting mode polling
  ADVANCEDCA_BLOCK_4                    = 4,        ///< For load block,temp store the mid result
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_BLOCK64BIT
{
  ADVANCEDCA_BLOCK64BIT_LOW   = 0,    ///< waiting mode polling
  ADVANCEDCA_BLOCK64BIT_HIGH  = 1,    ///< waiting mode polling
};

/**
 * @brief   advanced ca module waiting mode used by cipher engine.
 *
 *  A data structure used to describe waiting mode for all cipher engines.
 */
enum  _EN_ADVANCEDCA_LAYER
{
  ADVANCEDCA_LAYER_1      = 1,    ///< waiting mode polling
  ADVANCEDCA_LAYER_2      = 2,    ///< waiting mode polling
  ADVANCEDCA_LAYER_3      = 3,    ///< waiting mode polling
};

/* ========================================================================== */
/* macro definitions                    */
/* ========================================================================== */
#define HAL_SECURE_DEBUG

#ifdef  HAL_SECURE_DEBUG
#define COLORPRINT(COLOR, ...)          \
    OS_PRINTF("%c[%d;%d;%dm", 0x1B, 7, COLOR, 40);  \
    OS_PRINTF(__VA_ARGS__);         \
    OS_PRINTF("%c[%dm", 0x1B, 0);
#else
#define COLORPRINT(COLOR, ...)
#endif


/* ========================================================================== */
/* structure definitions                  */
/* ========================================================================== */

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe rsa key parameters
 */
struct  _ST_RSA_KEYCONFIGINFO
{
  enum _EN_ADVANCEDCA_KEYPATH     enKeyPath;
  u32                             enKeySize;
  u32                             *p_addr_m;
  u32                             *p_addr_d;
  u32                             *p_addr_e;
};
/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_KEYCONFIGINFO
{
  enum _EN_ADVANCEDCA_KEYPATH     enKeyPath;
  enum _EN_ADVANCEDCA_KEYSIZE     enKeySize;
  u8                              u8Key[ADVANCEDCA_MAXKEYSIZE];
  enum _EN_ADVANCEDCA_KEYOPTION         enKeyOption;
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_BLOCKCONFIGINFO
{
  enum _EN_ADVANCEDCA_BLOCKINDEX        enKeyBlock;		///store the key
  enum _EN_ADVANCEDCA_BLOCKINDEX        enLoadBlock;	///Engine input block
  enum _EN_ADVANCEDCA_BLOCKINDEX        enStoreBlock; ///Engine output block
  enum _EN_ADVANCEDCA_BLOCK64BIT        enBlock64Bit; 
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_CWCONFIGINFO
{
  u8                              u8SlotIndex;
  enum _EN_ADVANCEDCA_CWLOADING   enCWLoading;
  enum _EN_ADVANCEDCA_CWTYPE      enCWType;
	enum _EN_ADVANCEDCA_CIPHERENGINE			enCipherEngine;
  u8                              u8CWIndex;
  u8                              pti_id;       /*tag the pti id for security*/
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_LAYERINFO
{
  enum _EN_ADVANCEDCA_LAYER       enTotalLayer;
  enum _EN_ADVANCEDCA_LAYER       enCurrentLayer;
  u8                              u8Length;
  u8                              const * pu8Data;
  enum _EN_ADVANCEDCA_KEYOPTION         enKeyOption;
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_CIPHERENGINEINFO
{
  enum _EN_ADVANCEDCA_CIPHERENGINE      enCipherEngine;
  enum _EN_ADVANCEDCA_ROUTINGMODE       enRoutingMode;
  struct _ST_ADVANCEDCA_KEYCONFIGINFO   stKeyConfigInfo;
  struct _ST_ADVANCEDCA_BLOCKCONFIGINFO stBlockConfigInfo;
  enum _EN_ADVANCEDCA_BLOCKMODE         enBlockMode;
  u8                                    u8IV[ADVANCEDCA_MAXIVSIZE];
  struct _ST_ADVANCEDCA_CWCONFIGINFO    stCWConfigInfo;
  struct _ST_ADVANCEDCA_LAYERINFO       stLayerInfo;
  u32                                   u32Length;
  u8                                    const * pu8Input;
  u8                                    * pu8Output;
  enum _EN_ADVANCEDCA_WAITINGMODE       enWaitingMode;
	enum  _EN_ADVANCEDCA_ENCRYPT_KEYPATH  enEncryptKeyPath;
  struct _ST_RSA_KEYCONFIGINFO          info_RSAkey;
  // endian
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_CHANNELINFO
{
  enum _EN_ADVANCEDCA_CHANNELID         enChannelID;
  enum _EN_ADVANCEDCA_CHANNELSTATUS     enChannelStatus;
  enum _EN_ADVANCEDCA_APPLICATION       enApplication;
};

/**
 * @brief   advanced ca module cipher engine information structure
 *
 *  A data structure used to describe cipher engine's information which contains 
 *  cipher engine, key size, key, block mode, and routing mode.
 */
struct  _ST_ADVANCEDCA_OPTIONINFO
{
  RET_CODE                              (* ModuleInit)(void);
  RET_CODE                              (* ModuleExit)(void);
  RET_CODE                              (* ModuleReset)(void);
  RET_CODE                              (* EngineStart)
    (
     enum _EN_ADVANCEDCA_APPLICATION, 
     struct _ST_ADVANCEDCA_CIPHERENGINEINFO const * const
    );
  RET_CODE                              (* EngineStop)
    (
     enum _EN_ADVANCEDCA_APPLICATION
    );
};


/* ========================================================================== */
/* type definitions                   */
/* ========================================================================== */
typedef enum    _EN_ADVANCEDCA_APPLICATION      EN_ADVANCEDCA_APPLICATION;
typedef enum    _EN_ADVANCEDCA_ROUTINGMODE      EN_ADVANCEDCA_ROUTINGMODE;
typedef enum    _EN_ADVANCEDCA_WAITINGMODE      EN_ADVANCEDCA_WAITINGMODE;
typedef enum    _EN_ADVANCEDCA_BLOCKMODE        EN_ADVANCEDCA_BLOCKMODE;
typedef enum    _EN_ADVANCEDCA_LAYER            EN_ADVANCEDCA_LAYER;
typedef enum    _EN_ADVANCEDCA_CIPHERENGINE     EN_ADVANCEDCA_CIPHERENGINE;
typedef enum    _EN_ADVANCEDCA_KEYSIZE          EN_ADVANCEDCA_KEYSIZE;
typedef enum    _EN_ADVANCEDCA_KEYOPTION        EN_ADVANCEDCA_KEYOPTION;
typedef enum    _EN_ADVANCEDCA_CWLOADING        EN_ADVANCEDCA_CWLOADING;
typedef enum    _EN_ADVANCEDCA_CWTYPE           EN_ADVANCEDCA_CWTYPE;
typedef struct  _ST_ADVANCEDCA_KEYCONFIGINFO    ST_ADVANCEDCA_KEYCONFIGINFO;
typedef struct  _ST_ADVANCEDCA_BLOCKCONFIGINFO  ST_ADVANCEDCA_BLOCKCONFIGINFO;
typedef struct  _ST_ADVANCEDCA_CWCONFIGINFO     ST_ADVANCEDCA_CWCONFIGINFO;
typedef struct  _ST_ADVANCEDCA_LAYERINFO        ST_ADVANCEDCA_LAYERINFO;
typedef struct  _ST_ADVANCEDCA_CIPHERENGINEINFO ST_ADVANCEDCA_CIPHERENGINEINFO;
typedef struct  _ST_ADVANCEDCA_CHANNELINFO      ST_ADVANCEDCA_CHANNELINFO;
typedef struct  _ST_ADVANCEDCA_OPTIONINFO       ST_ADVANCEDCA_OPTIONINFO;
typedef enum    _EN_ADVANCEDCA_ENCRYPT_KEYPATH  EN_ADVANCEDCA_ENCRYPT_KEYPATH;

/* ========================================================================== */
/* variable definitions                   */
/* ========================================================================== */


/* ========================================================================== */
/* function definitions                   */
/* ========================================================================== */
/**
 * @name Public Functions
 * @{
 */

/**
  \brief    This function initializes the advance ca module.

  \return   0         Success
  \return   Others        Exception
 ******************************************************************************/
RET_CODE    hal_secure_ModuleInit (void); 

/**
  \brief    This function exits the advance ca module.

  \return   0         Success
  \return   Others        Exception
 ******************************************************************************/
RET_CODE    hal_secure_ModuleExit (void); 

/**
  \brief    This function resets the advance ca module.

  \return   0         Success
  \return   Others        Exception
 ******************************************************************************/
RET_CODE    hal_secure_ModuleReset (void); 

/**
  \brief    This function starts a cipher engine for the given application.

  Different applications use different cipher engine, which needs different
  parameters and configurations. So you must provide corresponding content for 
  the cipher information structure by following below rules:  

    1. CW decryption
    \n  routing mode
    \n  cipher engine
    \n  key size
    \n  key
    \n  block mode
    \n  IV
    \n  input
    \n  output

    2. PVR key decryption
    \n  cipher engine
    \n  key size
    \n  key
    \n  block mode
    \n  IV
    \n  input

    3. CA decryption
    \n  routing mode

    4. PVR playback
    \n  cipher engine
    \n  key path
    \n  key size
    \n  key
    \n  block mode
    \n  IV

    5. HDMI key decryption
    \n  cipher engine
    \n  key size
    \n  key
    \n  block mode
    \n  IV

    6. General purpose decryption 
    \n  routing mode
    \n  cipher engine
    \n  key size
    \n  key
    \n  block mode
    \n  IV

    7. PVR
    \n  cipher engine
    \n  key path
    \n  key size
    \n  key
    \n  block mode
    \n  IV

    8. General purpose encryption 
    \n  cipher engine
    \n  key size
    \n  key
    \n  block mode
    \n  IV

  \param[in]  enApplication   application to start
  \param[in]  stCipherEngineInfo  cipher engine information structure

  \return     0       Success
  \return     Others      Exception
 ******************************************************************************/
RET_CODE    hal_secure_EngineStart (
  EN_ADVANCEDCA_APPLICATION enApplication,
  ST_ADVANCEDCA_CIPHERENGINEINFO  const * const stCipherEngineInfo
  );

/**
  \brief    This function stops a cipher engine for the given application, 
  which started before.

  \param[in]  enApplication   application to stop

  \return     0       Success
  \return     Others      Exception
 ******************************************************************************/
RET_CODE    hal_secure_EngineStop (
    EN_ADVANCEDCA_APPLICATION enApplication
  );

/**
  \brief    This function attaches driver for Advanced Conditional Access Module.

  \return   0       Success
  \return   Others      Exception
 ******************************************************************************/
RET_CODE    hal_secure_DriverAttach (void);

/**
 * @}
 */


#endif /* __HAL_SECURE_H__ END */


/**
 * @}
 */

