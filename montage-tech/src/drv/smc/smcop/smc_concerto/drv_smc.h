/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __DRV_SC_H__
#define __DRV_SC_H__

/*!
  commments
  */
#define  SMC_EN_DIS_CMD                     0x00000001                 
/*!
  commments
  */
#define  SMC_POWER_SET_CMD                  0x00000002 
/*!
  commments
  */
#define  SMC_SOFT_RESET_CMD                 0x00000003
/*!
  commments
  */
#define  SMC_RX_IRQ_EN_DIS_CMD              0x00000004
/*!
  commments
  */
#define  SMC_RM_IRQ_EN_DIS_CMD              0x00000005  
/*!
  commments
  */
#define  SMC_INS_IRQ_EN_DIS_CMD             0x00000006
/*!
  commments
  */
#define  SMC_PARITY_SET_CMD                 0x00000007
/*!
  commments
  */
#define  SMC_CHECK_SET_CMD                  0x00000008
/*!
  commments
  */
#define  SMC_NCSET_EN_DIS_CMD               0x00000009
/*!
  commments
  */
#define  SMC_STP_ONE_TWO_CMD                0x0000000a
/*!
  commments
  */
#define  SMC_FREQ_DIV_SET_CMD               0x0000000b
/*!
  commments
  */
#define  SMC_CARD_STAT_TYPE_SET_CMD         0x0000000c
/*!
  commments
  */
#define  SMC_ALL_FIRST_INIT_CMD             0x0000000d       
/*!
  commments
  */
#define  SMC_REG_DUMP_CMD                   0x0000000e

/*!
  commments
  */
#define  SMC_IF_DIS                         0x0
/*!
  commments
  */
#define  SMC_IF_EN                          0x1

/*!
  commments
  */
#define  SMC_POW_ON                         0x0
/*!
  commments
  */
#define  SMC_POW_OFF                        0x1

/*!
  commments
  */
#define  SMC_RX_IRQ_DIS                     0x0
/*!
  commments
  */
#define  SMC_RX_IRQ_EN                      0x1

/*!
  commments
  */
#define  SMC_RM_IRQ_DIS                     0x0
/*!
  commments
  */
#define  SMC_RM_IRQ_EN                      0x1

/*!
  commments
  */
#define  SMC_INS_IRQ_DIS                    0x0
/*!
  commments
  */
#define  SMC_INS_IRQ_EN                     0x1

/*!
  commments
  */
#define  SMC_PARITY_DIS                     0x0
/*!
  commments
  */
#define  SMC_PARITY_EN                      0x1

/*!
  commments
  */
#define  SMC_CHECK_DIS                      0x0
/*!
  commments
  */
#define  SMC_CHECK_EN                       0x1

/*!
  commments
  */
#define  SMC_NCSET_DIS                      0x0
/*!
  commments
  */
#define  SMC_NCSET_EN                       0x1

/*!
  commments
  */
#define  SMC_STP_TWO                        0x0
/*!
  commments
  */
#define  SMC_STP_ONE                        0x1

/*!
  commments
  */
#define  SMC_CARD_TYPE0                     0x0
/*!
  commments
  */
#define  SMC_CARD_TYPE1                     0x1

/*!
  commments
  */
#define  SMC_RX_STATUS                      0x0
/*!
  commments
  */
#define  SMC_CARD_STATUS                    0x1

/*!
  commments
  */
#define ERR_SMC_NO_ERR    0x00000000

/*!
  commments
  */
#define ERR_SMC_BASE    ERROR_PLATFORM_DRV_SMC_BASE

/*!
  commments
  */
#define ERR_SMC_USERMUTEX_CREATE_FAIL  (ERR_SMC_BASE + 1)
/*!
  commments
  */
#define ERR_SMC_PARAM_INVALID      (ERR_SMC_BASE + 2)
/*!
  commments
  */
#define ERR_SMC_PENDMUTEX_FAIL      (ERR_SMC_BASE + 3)
/*!
  commments
  */
#define ERR_SMC_HANDLE_INVALID      (ERR_SMC_BASE + 4)
/*!
  commments
  */
#define ERR_SMC_RX_TIMEOUT        (ERR_SMC_BASE + 5)
/*!
  commments
  */
#define ERR_SMC_GETATR_MIN_FAIL      (ERR_SMC_BASE + 6)
/*!
  commments
  */
#define ERR_SMC_ATRDATA_PARSE_ERR    (ERR_SMC_BASE + 7)
/*!
  commments
  */
#define ERR_SMC_ALREADY_INIT      (ERR_SMC_BASE + 8)
/*!
  commments
  */
#define ERR_SMC_STATUSTASK_CREATE_FAIL  (ERR_SMC_BASE + 9)
/*!
  commments
  */
#define ERR_SMC_OUT            (ERR_SMC_BASE + 10)
/*!
  commments
  */
#define ERR_SMC_UNSUPPORT_PTOTOCOL    (ERR_SMC_BASE + 11)
/*!
  commments
  */
#define ERR_SMC_NOT_INIT        (ERR_SMC_BASE + 12)
/*!
  commments
  */
#define ERR_SMC_NO_ANSWER        (ERR_SMC_BASE + 13)
/*!
  commments
  */
#define ERR_SMC_INVALID_STATUS_BYTE    (ERR_SMC_BASE + 14)
/*!
  commments
  */
#define ERR_SMC_CARD_VPP        (ERR_SMC_BASE + 15)
/*!
  commments
  */
#define ERR_SMC_CARD_ABORTED      (ERR_SMC_BASE + 16)
/*!
  commments
  */
#define ERR_SMC_BLOCK_RETRIES      (ERR_SMC_BASE + 17)
/*!
  commments
  */
#define ERR_SMC_POSTMUTEX_FAIL      (ERR_SMC_BASE + 18)

/*!
  t0 protocol error code definition
  */
#define ERR_SMC_T0_ERR_BASE        (ERR_SMC_BASE + 100)
/*!
  commments
  */
#define ERR_SMC_T0CMD_INS        (ERR_SMC_T0_ERR_BASE + 1)
/*!
  commments
  */
#define ERR_SMC_T0CMD_CLA        (ERR_SMC_T0_ERR_BASE + 2)


/*!
  t1 protocol error code definition
  */
#define ERR_SMC_T1_ERR_BASE        (ERR_SMC_BASE + 200)
/*!
  commments
  */
#define ERR_SMC_T1_READ_NAD_FAIL    (ERR_SMC_T1_ERR_BASE + 1)
/*!
  commments
  */
#define ERR_SMC_T1_READ_PCB_FAIL    (ERR_SMC_T1_ERR_BASE + 2)
/*!
  commments
  */
#define ERR_SMC_T1_READ_LEN_FAIL    (ERR_SMC_T1_ERR_BASE + 3)
/*!
  commments
  */
#define ERR_SMC_T1_READ_BUF_FAIL    (ERR_SMC_T1_ERR_BASE + 4)
/*!
  commments
  */
#define ERR_SMC_T1_READ_LRC_FAIL    (ERR_SMC_T1_ERR_BASE + 5)
/*!
  commments
  */
#define ERR_SMC_T1_LRC_CEHCK_FAIL    (ERR_SMC_T1_ERR_BASE + 6)

/*!
  *  drv_smartcardhandle_t 
  *  说明: 
  *    DRV_SmartCardInit返回的句柄，每个smart卡对应一个句柄.
  */
typedef int drv_smartcardhandle_t;


/*!
  *  drv_smartcardprotocol_t
  *  说明: 
  *    Smart卡协议类型 
*/
typedef enum
{
  /*!
    commments
  */
  UNSUPPORTED_PROTOCOL = -1,
  /*!
    commments
  */
  T0_PROTOCOL = 0,
  /*!
    commments
  */
  T1_PROTOCOL = 1,
  /*!
    commments
  */
  T14_PROTOCOL = 14
} drv_smartcardprotocol_t;

/*!
  *  drv_smartcardinitparam_t
  *  说明: 
  *  Smart卡初始化参数
  *  注解:
  *  Id:如果有多个smart卡，用来区分
  *  SmartInsert:插卡事件回调函数
  *  SmartRemove:拔卡事件回调函数
*/
typedef struct 
{
  /*!
    comment
  */
  u8 smc_id;
  /*!
    comment
  */
  void (*SmartInsert)(void);
  /*!
    comment
  */
  void (*SmartRemove)(void);
  /*!
    comment
  */
  u8 convt_en;
 /*!
    comment
  */
  u8 convt_value;
  /*!
    comment
  */
  u8 detect_pin_pol;
  /*!
    comment
  */
  u8 vcc_enable_pol;
  /*!
    comment
  */
  u8 phyclk;
  /*!
    comment
  */
  u8 iopin_mode;
  /*!
    comment
  */
  u8 rstpin_mode;
  /*!
    comment
  */
  u8 clkpin_mode;
} drv_smartcardinitparam_t;

/*!
  *  drv_smartcardinsertstatus_t
  *  说明: 
  *    Smart卡初始化参数
  *  注解:
  *    DRV_SMART_STATUS_INSERT:smart卡处在插入状态
  *    DRV_SMART_STATUS_REMOVE:smart卡处在拔出状态
*/
typedef enum
{
  /*!
    commments
  */
  DRV_SMART_STATUS_INSERT = 0,
  /*!
    commments
  */
  DRV_SMART_STATUS_REMOVE
}drv_smartcardinsertstatus_t;

/*!
  *  drv_smartcardstatus_t
  *  说明: 
  *    Smart卡初始化参数
  *  注解:
  *    insert_status:smart是插入还是拔除
  *    protocol:当前smart卡使用的协议
*/
typedef struct
{
  /*!
    commments
  */
  drv_smartcardinsertstatus_t   insert_status;
  /*!
    commments
  */
  drv_smartcardprotocol_t  protocol;
} drv_smartcardstatus_t;


/*!
  *  drv_smartcardinit
  *  说明: 
  *    smart卡初始化函数
  *  参数:
  *    init_params:初始化参数，见DRV_SmartCardInitParam_t定义.
  *    handle:初始化后返回句柄
  *  返回:
  *    NO_ERROR : 正确
  *    ERROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T drv_smartcardinit(const drv_smartcardinitparam_t *pinit_params,
                                drv_smartcardhandle_t *phandle);

/*!
  * DRV_SmartCardReset
  *  说明: 
  *    smart卡复位
  *  参数:
  *    handle:句柄
  *    answer:复位后smart卡返回的数据
  *    answer_len:返回长度
  *  返回:
  *    NO_ERROR : 正确
  *    ERROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T drv_smartcard_reset(drv_smartcardhandle_t handle,u8 answer[33],u32 *panswer_len);
    
/*!
  *drv_smartcardtransfer
  *  说明: 
  *    smart卡复位
  *  参数:
  *    handle:句柄
  *    command :保存命令的buff
  *    command_len:命令字节长度
  *    num_write_ok:命令发送成功的字节数
  *    response:从smart卡返回数据的buff
  *    num_to_read:希望返回数据的长度
  *    num_read_ok:实际返回数据的长度
  *  返回:
  *    NO_ERROR : 正确
  *    ROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T  drv_smartcardtransfer(drv_smartcardhandle_t handle, 
                  const u8 *pcommand, u32 command_len, u32  *pnum_write_ok,
                  u8 *presponse, u32   num_to_read, u32  *pnum_read_ok);

/*!
  *
  *  drv_smartcardgetstatus
  *  说明: 
  *    smart卡复位
  *  参数:
  *    smc_id: smart card id
  *    handle:句柄
  *    status:返回状态
  *  返回:
  *    NO_ERROR : 正确
  *    ERROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T  drv_smartcardgetstatus(drv_smartcardhandle_t handle,drv_smartcardstatus_t  *pstatus);



/*!
  *
  *  drv_smartcardwrite
  *  说明: 
  *    发送命令函数
  *  参数:
  *    handle:句柄
  *    pBuf : 发送数据
  *    NumberToWrite : 需要发送数据长度
  *    NumberWritten_p: 实际发送的长度
  *    Timeout : 发送的超时等待
  *  返回:
  *    NO_ERROR : 正确
  *    ERROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T  drv_smartcardwrite(drv_smartcardhandle_t handle, 
                IN const U8 *pBuf, 
                U32 NumberToWrite, 
                U32  *pNumberWritten_p, 
                U32 Timeout);

/*!
  *
  *  drv_smartcardgetstatus
  *  说明: 
  *    接收返回函数
  *  参数:
  *    handle:句柄
  *    pBuf : 接收数据
  *    NumberToWrite : 接收数据长度
  *    NumberWritten_p: 实际接收数据的长度
  *    Timeout : 接收的超时等待
  *  返回:
  *    NO_ERROR : 正确
  *    ERROR_PARM : 参数错误,指针为空等。
  */
ERRORCODE_T  drv_smartcardread(drv_smartcardhandle_t handle,
            OUT U8 *pBuf, 
            U32 NumberToRead, 
            U32  *pNumberRead_p, 
            U32 Timeout);

/*!
  *
  *  drv_smartcardsetetu
  *  说明: 
  *    设置ETU，提供给PPS协议用
  *  参数:
  *    WorkETU:通过Fi/Di值计算出WorkETU
  */
  void drv_smartcardsetetu(drv_smartcardhandle_t handle, u32 WorkETU);

/*!
  just for debug
  */
void util_print_buffer(u8 * pAddr, u32 len, s8 * pname);
#endif //__DRV_SC_H__
