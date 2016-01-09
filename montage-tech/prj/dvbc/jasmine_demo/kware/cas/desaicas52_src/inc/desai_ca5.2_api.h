/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __DS_SC_H__
#define __DS_SC_H__

/****************************************************************/
/****************************************************************/
/*提示信息*/
#define		MESSAGE_CANCEL_TYPE					0x00           
#define		MESSAGE_BADCARD_TYPE				0x01		/*无法识别卡，不能使用*/
#define		MESSAGE_EXPICARD_TYPE				0x02		/*智能卡已经过期，请更换新卡*/
#define		MESSAGE_CARD_INIT_TYPE				0x03		/*智能卡正在初始化*/
#define		MESSAGE_INSERTCARD_TYPE				0x04		/*加密节目，请插入智能卡*/
#define		MESSAGE_CARD_INIT_ERR_TYPE			0x05		/*智能卡初始化失败*/
#define		MESSAGE_STB_NO_CARD_TYPE			0x06		/*机顶盒中不存在智能卡*/
#define		MESSAGE_CARD_INIT_OK_TYPE			0x07		/*智能卡初始化完成*/
#define		MESSAGE_NOOPER_TYPE					0x10		/*卡中不存在节目运营商*/
#define		MESSAGE_CARDSTOP_TYPE				0x11		/*智能卡已停用*/
#define		MESSAGE_OUTWORKTIME_TYPE			0x12		/*不在工作时段内*/
#define		MESSAGE_WATCHLEVEL_TYPE				0x13		/*节目级别高于设定观看级别*/
#define		MESSAGE_PAIRING_TYPE					0x14		/*机卡没有对应*/
#define		MESSAGE_NET_ERROR_TYPE					0x17		/*网络不正确，请在正确的网络中使用*/
#define		MESSAGE_NOENTITLE_TYPE				0x20		/*您没有购买该节目*/
#define		MESSAGE_DECRYPTFAIL_TYPE				0x21		/*节目解密失败*/
#define		MESSAGE_NOMONEY_TYPE				0x22		/*卡内金额不足*/
#define		MESSAGE_ERRREGION_TYPE				0x23		/*区域不正确*/
#define		MESSAGE_FINGER_SHOW_TYPE			0x24		/*指纹显示*/
#define		MESSAGE_FINGER_DISAPPEAR_TYPE		0x30		/*指纹隐藏*/
#define		MESSAGE_CARDNOTACTIVE_TYPE			0x31		/*智能卡未激活*/
#define		MESSAGE_CARDLOGOUT_TYPE				0x32		/*智能卡已注销*/
#define		MESSAGE_CONDITION_CTL_TYPE			0x33		/*该产品被限播*/
#define		MESSAGE_DESCRA_NOENTITLE_TYPE		0x34		/*授权已到期，请联系运营商*/
#define		MESSAGE_ENTITLE_UPDATE_TYPE			0x35		/*授权更新中，请稍等！*/
#define		MESSAGE_CARD_UNWONTED_TYPE			0x36		/*智能卡内部异常*/
#define		MESSAGE_CHANGEPRO_TOO_FAST_TYPE			0x37		/*请不要频繁切台*/
#define		MESSAGE_STB_NEED_UPDATE_TYPE		0x38		/*机顶盒需要升级，请稍等*/

#define    	MESSAGE_MS_PAIR_OK_TYPE				0x40		/* 子母卡配对成功 */
#define    	MESSAGE_MS_PAIR_FAILED_TYPE			0x41		/* 子母卡配对失败,请重新插入正确的母卡 */
#define    	MESSAGE_MS_REPAIR_TYPE				0x42		/* 子卡需重新和母卡配对 */
#define		MESSAGE_CARD_MATCH_OK_TYPE			0x43		/*机卡配对成功*/
#define		MESSAGE_CARD_REMATCH_TYPE			0x44		/*重新机卡配对*/
#define		MESSAGE_CANCEL_MATCH_TYPE			0x45		/*取消机卡配对*/

#define		MESSAGE_NEWEMAIL_TYPE				0x50		/* 显示新邮件图标 */  
#define		MESSAGE_OVEREMAIL_TYPE				0x51		/* 显示邮件存满 */
#define		MESSAGE_HIDE_NEWEMAIL_TYPE			0x52		/* 隐藏新邮件图标*/
#define		MESSAGE_EXIGENT_PROG_TYPE			0x53		/*紧急广播*/
#define		MESSAGE_EXIGENT_PROG_STOP_TYPE		0x54		/*停止紧急广播*/
#define		MESSAGE_CARD_UNLOCK_OK_TYPE		0x55		/*智能卡密码解锁成功*/
#define		MESSAGE_PURSER_UPDATE_TYPE			0x56		/*电子钱包更新*/
#define		MESSAGE_NET_ERR_LOCK_TYPE			0x57		/*网络异常，卡被锁定*/
#define		MESSAGE_SYSTEM_ERR_LOCK_TYPE		0x58		/*系统异常，卡被锁定*/
#define		MESSAGE_NET_ERR_UNLOCK_TYPE			0x59		/*网络异常解锁*/
#define		MESSAGE_SYSTEM_ERR_UNLOCK_TYPE		0x60		/*系统异常解锁*/

/*OSD消息*/
#define		MESSAGE_SHOWOSD_TYPE				0x61		/*显示OSD消息*/
#define		MESSAGE_HIDEOSD_TYPE					0x62		/*隐藏OSD消息*/

/*智能卡更新*/
#define		MESSAGE_CARD_UPDATE_BEGIN_TYPE		0x65		/*智能卡开始更新，显示进度条*/
#define		MESSAGE_CARD_UPDATE_PROGRESS_TYPE	0x66		/*更新进度*/
#define		MESSAGE_CARD_UPDATE_ERR_TYPE		0x67		/*更新出错，错误代码*/
#define		MESSAGE_CARD_UPDATE_END_TYPE		0x68		/*更新成功，进度条消失*/
#define		MESSAGE_CARD_NEED_UPDATE_TYPE		0x69		/*智能卡需要更新，请稍后*/

/*提示对话框*/
#define		MESSAGE_CARDISDUE_TYPE					0x70		/*智能卡处于欠费催缴*/
#define		MESSAGE_IPPVBUY_TYPE					0x71		/*IPPV购买提示框 */
#define		MESSAGE_IPPV_OUTTIME_TYPE				0x72		/*IPPV节目过期*/
#define		MESSAGE_HIDE_IPPVBUY_TYPE				0x73		/*隐藏 IPPV购买提示框 */
#define		MESSAGE_CONDITION_SEARCH_TYPE			0x74		/*条件NIT表搜索节目*/

/*内部测试卡显示信息*/
#define		MESSAGE_URGE_OPEN_TYPE					0x80
#define		MESSAGE_URGE_CLOSE_TYPE					0x81
#define		MESSAGE_URGE_DAYS_TYPE					0x82
#define		MESSAGE_URGE_OSD_SHOWTIME_TYPE		0x83
#define		MESSAGE_ALLCARD_ADDR_CTL_OPEN_TYPE	0x84
#define		MESSAGE_ALLCARD_ADDR_CTL_CLOSE_TYPE	0x85
#define		MESSAGE_CARD_PAIR_DAYS_TYPE				0x86
#define		MESSAGE_WT_OPEN_TYPE					0x87
#define		MESSAGE_WT_CLOSE_TYPE					0x88
#define		MESSAGE_WL_OPEN_TYPE					0x89
#define		MESSAGE_WL_CLOSE_TYPE					0x8A
#define		MESSAGE_ALLCARD_FINGER_OPEN_TYPE		0x8B
#define		MESSAGE_ALLCARD_FINGER_CLOSE_TYPE		0x8C
#define		MESSAGE_FINGER_SHOW_TIME_TYPE			0x8D
#define		MESSAGE_FINGER_STOP_TIME_TYPE			0x8E
#define		MESSAGE_DUE_OPEN_TYPE					0x8F
#define		MESSAGE_DUE_CLOSE_TYPE					0x90
#define		MESSAGE_DUE_DAYS_TYPE					0x91
#define		MESSAGE_CARD_OVERDUE_TIME_TYPE			0x92
#define		MESSAGE_URGE_OSD_UPDATE_TYPE			0x93
#define		MESSAGE_EXIGENT_OSD_UPDATE_TYPE		0x94
#define		MESSAGE_CARD_TYPE_TYPE					0x95
#define		MESSAGE_MOTHER_CARD_UPDATE_TYPE		0x96
#define		MESSAGE_CARD_FINGER_OPEN_TYPE			0x97
#define		MESSAGE_CARD_FINGER_CLOSE_TYPE			0x98
#define		MESSAGE_CARD_ADDR_CTL_OPEN_TYPE		0x99
#define		MESSAGE_CARD_ADDR_CTL_CLOSE_TYPE		0x9A
#define		MESSAGE_CARD_ADDR_UPDATE_TYPE			0x9B
#define		MESSAGE_ENTITLE_TYPE_UPDATE_TYPE		0x9C
#define		MESSAGE_ALL_CARD_ENTITLE_OPEN_TYPE		0x9D
#define		MESSAGE_ALL_CARD_ENTITLE_CLOSE_TYPE	0x9E
#define		MESSAGE_ALL_CARD_ENTITLE_UPDATE_TYPE	0x9F
#define		MESSAGE_EXIGENT_PROG_OPEN_TYPE			0xA0
#define		MESSAGE_EXIGENT_PROG_CLOSE_TYPE		0xA1
#define		MESSAGE_CARD_GROUP_ID_UPDATE_TYPE		0xA2
#define		MESSAGE_CARD_LCO_UPDATE_TYPE			0xA3
#define		MESSAGE_SYS_MONITOR_OPEN_TYPE			0xA4
#define		MESSAGE_SYS_MONITOR_CLOSE_TYPE			0xA5
#define		MESSAGE_NET_MONITOR_OPEN_TYPE			0xA6
#define		MESSAGE_NET_MONITOR_CLOSE_TYPE			0xA7
#define		MESSAGE_NET_ERR_TIME_UPDATE_TYPE		0xA8
#define		MESSAGE_CARD_OUTTIME_OPEN_TYPE			0xA9
#define		MESSAGE_CARD_OUTTIME_CLOSE_TYPE		0xAA
#define		MESSAGE_DESRCAM_PRO_OPEN_TYPE			0xAB
#define		MESSAGE_DESRCAM_PRO_CLOSE_TYPE			0xAC
#define		MESSAGE_MULTI_PRO_OPEN_TYPE				0xAD
#define		MESSAGE_MULTI_PRO_CLOSE_TYPE			0xAE

#define		MESSAGE_CLEAN_SCREEN_TYPE			0xF0
/****************************************************************/
/****************************************************************/

#define		DSCA_OPERATOR_NAME_LEN				(40)
#define		DSCA_OPERATOR_PHONE_LEN				(40)
#define		DSCA_OPERATOR_ADDRESS_LEN			(80)

#define    IN                         /*函数参数(传入数据)*/
#define	 OUT                      /*函数参数(回传数据)*/
#define    DSCA_CARD_REGION_DESC   0XA5/*NIT表中智能卡区域ID描述符*/

#ifndef		NULL
#define		NULL		0
#endif

#ifndef TRUE
#define TRUE				(1)
#endif

#ifndef FALSE
#define FALSE			(0)
#endif

#ifndef BOOL
#define BOOL		unsigned char
#endif

typedef char				INT8;
typedef unsigned char		UINT8;
typedef short				INT16;
typedef unsigned short		UINT16;
typedef int				INT32;
typedef unsigned int		UINT32;
typedef UINT32			DSCA_Semaphore;
typedef UINT32			DSCA_MsgQueue; 

//时间格式
typedef struct _ST_TIME_INFO
{
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_TIME_INFO;

//日期时间格式
typedef struct _ST_DATE_TIME_INFO
{
	UINT16	usYear;
	UINT8	ucMonth;
	UINT8	ucDay;
	UINT8 	ucHour;
	UINT8 	ucMinute;
	UINT8	ucSecond;
}ST_DATE_TIME_INFO;

//邮件信息
typedef struct _ST_MAIL_INFO
{
	UINT8				ucMailID;				// Mail ID 
	UINT8				ucNewFlag;				// 0 不是新邮件 1是新邮件 
	UINT8				aucMailTitle[32 + 1];		// 邮件标题，最长为30
	UINT8				aucMailSender[32 + 1];	//邮件发送人
	ST_DATE_TIME_INFO	stSendTime;				//邮件发送时间(接收时间)
}ST_MAIL_INFO;

//产品授权信息
typedef struct  _ST_PRODUCT_ENTITLE
{
	UINT8			ucEnableType;		/*授权模式*/
	UINT8          		ucRecoadFlag;		/* 暂没有用 ，0：不可以录像 1：可以录像*/
	UINT16        		usProductID;		/* 产品ID */
	ST_DATE_TIME_INFO 	stBeginTime;		/* 产品起始时间*/
	ST_DATE_TIME_INFO 	stEndTime;		/* 产品结束时间*/
}ST_PRODUCT_ENTITLE;

//邮件列表信息
typedef struct _ST_MAILLIST_INFO
{
	ST_MAIL_INFO	stMailInfo[50];
	UINT8 		ucMailNum;
	
}ST_MAILLIST_INFO;

//运营商信息
typedef struct _ST_OPERATOR_INFO
{
	UINT8 aucOperatorName[DSCA_OPERATOR_NAME_LEN];
	UINT8 aucOperatorPhone[DSCA_OPERATOR_PHONE_LEN];
	UINT8 aucOperatorAddress[DSCA_OPERATOR_ADDRESS_LEN];
	
}ST_OPERATOR_INFO;

//IPPV信息
typedef struct _ST_IPPV_INFO
{
	UINT8			aucIppvName[33];
	UINT8			aucEventID[4];
	UINT8			ucProviderID;     
	UINT16			usPrice;  
	ST_DATE_TIME_INFO	stBeginTime;
	ST_DATE_TIME_INFO	stEndTime;

}ST_IPPV_INFO;

//指纹显示位置
typedef enum _EN_FINGER_SHOW_POS
{
	SHOW_LEFT_TOP = 0,
	SHOW_RIGHT_TOP,
	SHOW_LEFT_BOTTOM,
	SHOW_RIGHT_BOTTOM,
	SHOW_BY_COORDINATE,
	
}EN_FINGER_SHOW_POS;

//指纹显示信息
typedef struct _ST_FINGER_SHOW_INFO
{
	EN_FINGER_SHOW_POS enShowPostion;		/*显示位置*/
	UINT8 aucFontShowColor[4];				/*字体显示颜色*/
	UINT8 aucBackShowColor[4];				/*背景显示颜色*/
	UINT16 usShowXPos;						/*显示X坐标*/
	UINT16 usShowYPos;						/*显示Y坐标*/
	UINT8 ucShowFont;						/*显示字体*/
	UINT8 ucShowFontSize;					/*显示字体大小*/
	UINT8 pucShowContent[20];				/*显示内容*/
	
}ST_FINGER_SHOW_INFO;

//OSD显示位置
typedef enum _EN_OSD_SHOW_POS
{
	SHOW_TOP = 0,					//在顶部显示
	SHOW_BOTTOM,					//在底部显示
	SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
	SHOW_LEFT,						//在左边显示
	SHOW_RIGHT,					//在右边显示	
	SHOW_LEFT_RIGHT,				//在左边和右边同时显示
}EN_OSD_SHOW_POS;

//OSD显示信息
typedef struct _ST_OSD_SHOW_INFO
{
	EN_OSD_SHOW_POS	ucDisplayPos;
	UINT8	ucFontType;
	UINT8	ucFontSize;
	UINT8	aucBackgroundColor[4];
	UINT8	aucDisplayColor[4];
	UINT16	usContentSize;
	UINT16	usGetBufferSize;
	UINT8* 	pucDisplayBuffer;
	
}ST_OSD_SHOW_INFO;

//工作时段信息
typedef struct _ST_WORKTIME_INFO
{
	ST_TIME_INFO stBeginTime;
	ST_TIME_INFO stEndTime;
}ST_WORKTIME_INFO;

typedef enum _EN_CA_LANGUAGE
{
	CA_LANGUAGE_CHINESE,
	CA_LANGUAGE_ENGLISH,
	CA_LANGUAGE_TOTAL,
}EN_CA_LANGUAGE;

typedef struct _ST_CA_EXIGENT_PROGRAM
{
	UINT16 usNetWorkID;
	UINT16 usTransportStreamID;
	UINT16 usServiceID;
}ST_CA_EXIGENT_PROGRAM;

typedef struct _ST_CA_MSG_QUEUE
{
	UINT32 uiFirstPara;
	UINT32 uiSecondPara;
	UINT32 uiThirdPara;
	UINT32 uiFourthPara;
}ST_CA_MSG_QUEUE;

typedef enum _EN_CAS_SHOW_MSG_TYPE
{
	CAS_MSG_TYPE_PICTURE,//显示图片
	CAS_MSG_TYPE_FINGER_SHOW,//显示指纹
	CAS_MSG_TYPE_OSD,//显示osd
	CAS_MSG_TYPE_NOTE_LONG_TIME,//长期显示的消息
	CAS_MSG_TYPE_NOTE_SHORT_TIME,//显示时间很短的消息
	CAS_MSG_TYPE_DIALOG,//对话框
	CAS_MSG_TYPE_PROGRESS,//进度条
	CAS_MSG_TYPE_NOT_SHOW,//只是进行某种操作，不涉及到显示
	CAS_MSG_TYPE_TOTAL,
}EN_CAS_SHOW_MSG_TYPE;

//函数返回值类型
typedef enum _EN_ERR_CAS
{
	/*错误类型定义*/
	CA_ERROR_OK = 0x00,
	CA_CARD_FAILURE,                           /*卡通讯失败*/
	CA_CARD_SUCCESS,                             /*卡通讯成功*/
	CA_CARD_NOMONEY,                            /*卡内金额不足*/
	CA_CARD_WRITEPURSER_ERROR,          /*写钱包失败*/
	CA_CARD_ERROR_REGION,		     /*区域不正确*/
	CA_CARD_CHECK_PIN_OK,                   /*验证PIN码成功*/
	CA_CARD_CHECK_PIN_ERROR,		     /*验证PIN码失败*/
	CA_CARD_CHANGE_PIN_FAILED,        /*修改PIN失败*/
	CA_CARD_CHECK_PIN_LOCK,               /*验证PIN码次数已达最大，文件被锁*/
	CA_CARD_UNLOCK_PIN_FAILED,	     /*解锁PIN失败*/
	CA_CARD_INITING,				    /*智能卡正在初始化*/
	CA_ERROR_IC_INVALID ,                     /* 机顶盒内无卡或无效卡         */ 
	CA_ERROR_CARD_PAIROTHER,            /* 智能卡已经对应别的机顶盒     */
	CA_ERROR_CARD_STOP,        			/* 卡已停用    */
	CA_ERROR_CARD_CANCELED,        		/* 卡已注销  */
	CA_ERROR_CARD_OUTTIME,			/* 卡已过期 */
	CA_ERROR_CARD_PENDING,        			/* 卡未激活    */
	CA_ERROR_CARD_NOPAIR,                  /* 不要求机卡配对*/
	CA_ERROR_PARAMETER,                      /*参数错误*/   
	CA_ERROR_FLASHADDR,                      /*FLASH地址错误（为NULL）*/
	CA_ERROR_UNKOWNDATA,                  /*未知数据，不是CA所需要的数据*/
	CA_ERROR_REPEATINGDATA,             /*重复的数据*/
	CA_ERROR_PAIRED_OK,                      /*字母卡配对成功*/
	CA_ERROR_WORKTIME,                         /*不在工作时段*/
	CA_ERROR_WATCH_LEVLE,                 /*当前节目观看级别大于设置级别*/
	CA_ERROR_PAIRED_FAILED,              /*字母卡配对失败*/
	CA_ERROR_PAIRED_REPAIRED,          /*字母卡需要重新配对*/
	CA_ERROR_NOENTITLE,                      /*没有授权*/
	CA_ERROR_IPPV_NOBUY,                    /*IPPV未购买*/
	CA_ERROR_NETTEST,                         /*入网测试*/
	CA_ERROR_CRC,                                 /*CRC错误*/
	CA_ERROR_WORKTIME_FORMAT,		/*工作时段格式错误*/
	CA_ERROR_MALLOC_ERR,			/*内存分配失败*/
	CA_ERROR_CONDITION_CTL,			/*该产品限播*/
	CA_ERROR_NET_ERR,			/*网络异常，STB被锁定*/
	CA_ERROR_SYS_ERR,			/*系统异常，STB被锁定*/
	CA_ERROR_STATE_ERR,			/*状态不满足*/
	CA_ERROR_CARD_UNWONTED,			/*卡内数据异常*/
	CA_ERROR_OPT_TOO_FAST,		/*操作太频繁*/
	CA_ERROR_CARD_NEED_UPDATE,		/*智能卡需要升级*/
	CA_ERROR_STB_NEED_UPDATE,		/*机顶盒需要升级*/
	CA_ERROR_UNKNOWN,                       /* 未知错误*/
}EN_ERR_CAS;

typedef enum _EN_CA_MSG_QUEUE_MODE
{
	CA_MSG_QUEUE_WAIT,
	CA_MSG_QUEUE_NOWAIT,
	CA_MSG_QUEUE_UNKNOW,
}EN_CA_MSG_QUEUE_MODE;

typedef enum _EN_CA_DATA_ID
{
	ECM_DATA_ID = 1,
	EMM_DATA_ID = 2,
	OSD_DATA_ID = 3,
	MAIL_DATA_ID = 4,
	DELE_OSD_DATA_ID = 5,
	CONFIG_DATA_ID = 6,
	CADRESS_DATA_ID = 7,
	DELE_CADRESS_DATA_ID = 8,
	SK_DATA_ID = 9,
	TOTAL_DATA_TYPE = 10,
}EN_CA_DATA_ID;

/***********************************机顶盒提供给CA的函数**************************************/

extern void DSCA_printf(IN const INT8 *string,...);
extern void DSCA_GetBuffer(OUT UINT32 * puiStartAddr,  OUT UINT32 * puiSize);
extern void DSCA_FlashRead(UINT32 uiStartAddr,  OUT UINT8 * pucData,  IN UINT32 * puiLen);
extern void DSCA_FlashWrite(UINT32 uiStartAddr, IN UINT8* pucData, IN UINT32 uiLen);
extern void DSCA_FlashErase(void);
extern void* DSCA_Malloc(UINT16 usBufferSize);
extern void DSCA_Memset(IN void* pucBuffer, UINT8 ucValue, UINT32 uiSize);
extern void DSCA_Memcpy(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Memcmp(IN void* pucDestBuffer, IN void* pucSourceBuffer, UINT32 uiSize);
extern INT32 DSCA_Sprintf(IN UINT8* pucDestBuffer, IN const UINT8* pucFormatBuffer, ...);
extern INT32 DSCA_StrLen(IN const UINT8* pucFormatBuffer);
extern void DSCA_Free(IN void* pucBuffer);
extern void DSCA_Sleep(UINT16 usMilliSeconds);				       
extern void DSCA_SemaphoreInit(IN DSCA_Semaphore* puiSemaphore , UINT8 ucInitVal);
extern void DSCA_ReleaseSemaphore(IN DSCA_Semaphore* puiSemaphore);
extern void DSCA_WaitSemaphore(IN DSCA_Semaphore* puiSemaphore);
extern void DSCA_ShowMessage(UINT8 ucMessageType, EN_CAS_SHOW_MSG_TYPE  enShowType, IN UINT8 * pucMessage);
extern void DSCA_SetCW(UINT16 usEcmPID, IN UINT8* pucOddKey, IN UINT8* pucEvenKey, IN UINT8* pucKeyLen);
extern void DSCA_StopCurrentProgram(void);
extern void DSCA_PlayCurrentProgram(void);
extern void DSCA_SendDataToSmartCard(IN UINT8* pucSend, IN UINT8 *pucSendLen, OUT UINT8* pucReceive,
											OUT UINT8 *pucReceiveLen, UINT32 uiTimeout, OUT UINT8* pucStatusWord );
extern BOOL DSCA_RegisterTask (IN INT8* pucName, UINT8 ucPriority, IN void* pTaskFun );
extern BOOL DSCA_SCReset(UINT8 *pucAtr, UINT8 *pucLen);
extern BOOL DSCA_SetFilter(EN_CA_DATA_ID enDataID, IN UINT8* pucFilter, IN UINT8* pucMask,
							UINT8 ucFilterLen, UINT16 usPID, UINT8 ucWaitSeconds);
extern BOOL DSCA_StopFilter(EN_CA_DATA_ID enDataID, UINT16 usPID);
extern BOOL DSCA_QueryProgram(UINT16 usNetWorkID, UINT16 usTransportStreamID, UINT16 usServiceID);
extern BOOL DSCA_MsgQueueInit(IN INT8* pucName, UINT32* uiMsgQueueHandle, UINT32 uiMaxMsgLen, EN_CA_MSG_QUEUE_MODE enMode);
extern BOOL DSCA_MsgQueueGetMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg, EN_CA_MSG_QUEUE_MODE enMode, UINT32 uiWaitMilliSecond);
extern BOOL DSCA_MsgQueueSendMsg(UINT32 uiMsgHandle, ST_CA_MSG_QUEUE* pstMsg);
/*****************************************************************************************************/

/*******************************CA 提供给机顶盒调用的函数********************************/
void DSCA_SetEcmPid(UINT16 usPid);
void DSCA_SetEmmPid(UINT16 usPid);
void DSCA_PowerOff(void);
void DSCA_PowerOn(void);
void DSCA_SetCardState(UINT8  ucInOrOut);
void DSCA_SetSysLanguage(EN_CA_LANGUAGE enLanguage);
void DSCA_SetCardAddressCode(IN UINT8* pucCardCode);
void DSCA_OsdOneTimeOk(void);
void DSCA_GetPrivateData(UINT16 usPid, EN_CA_DATA_ID enDataID, IN UINT8* pucReceiveData, UINT16 usDataLen);
BOOL DSCA_IsDSCa(UINT16 usCaSystemID);
BOOL DSCA_QueryBroadcastEntitle(void);
EN_ERR_CAS DSCA_SC_UnlockPin(void);
EN_ERR_CAS DSCA_DelEmail(UINT8 ucMailID);
EN_ERR_CAS DSCA_DelAllEmail(void);
EN_ERR_CAS DSCA_GetCardRegion(OUT UINT8 * pucCardRegion);
EN_ERR_CAS DSCA_GetOperatorID(OUT UINT8 * pucOperator);
EN_ERR_CAS DSCA_GetOperatorInfo(ST_OPERATOR_INFO* pstOperatorInfo );
EN_ERR_CAS DSCA_SetWatchLevelState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_SetWorkTimeState(UINT8 ucOpenOrClose);
EN_ERR_CAS DSCA_IppvBuy(UINT8 ucBuy,UINT8 ucProvideID) ;
EN_ERR_CAS DSCA_Init(UINT8 ucPriority, EN_CA_LANGUAGE enLanguage);
EN_ERR_CAS DSCA_GetProductNumber(UINT32 uiOperatorID, OUT UINT32 *puiNumber);
EN_ERR_CAS DSCA_GetProductInfo (UINT32 uiOperatorID,  UINT32 uiStartNo,  UINT32 uiReadNum, OUT ST_PRODUCT_ENTITLE *pstProductEntitles);
EN_ERR_CAS DSCA_GetCardNo(OUT UINT8 * pucCardNO, OUT UINT8* pucCardType);
EN_ERR_CAS DSCA_GetMasterCard(UINT8 * pucMasterCardNO);
EN_ERR_CAS DSCA_GetCAVer(OUT UINT8 * pucCAVersion);
EN_ERR_CAS DSCA_GetLCO(OUT UINT8 aucLco[8]);
EN_ERR_CAS DSCA_GetGroupId(OUT UINT8 aucGroupId[16]);
EN_ERR_CAS DSCA_GetCardVersion(OUT UINT8 aucCardVer[32]);
EN_ERR_CAS DSCA_GetPurserInfo(UINT8 ucNo, OUT UINT8 * pucPurser);
EN_ERR_CAS DSCA_GetEmailNumber(OUT UINT8 *pstEmailNo);
EN_ERR_CAS DSCA_GetEmailInfoList(OUT ST_MAILLIST_INFO *pstEmailInfoList);
EN_ERR_CAS DSCA_GetEmailContent(UINT32 uiEmailID, OUT UINT8* pucContentBuf, OUT UINT16 *pusContentLen);
EN_ERR_CAS DSCA_GetCardState(void);
EN_ERR_CAS DSCA_ChangePin(IN UINT8* pucOldPin, IN UINT8* pucNewPin, OUT UINT8* ucOddTimes);
EN_ERR_CAS DSCA_PurseCheckPin(UINT8* pucPin, UINT8* pucOddTimes);
EN_ERR_CAS DSCA_SetWorkTime(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_SetWatchLevel(IN UINT8* pucPin, OUT UINT8* pucOddTimes, IN UINT8* pucWorkLevel);
EN_ERR_CAS DSCA_GetWorkTime(OUT ST_WORKTIME_INFO* pstWorkTime);
EN_ERR_CAS DSCA_GetWorkLevel(OUT UINT8* pucWorkLevel);
/****************************************************************************************************/
#endif

