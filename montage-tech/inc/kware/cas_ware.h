/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __CAS_WARE_H__
#define __CAS_WARE_H__

/*!
  The macro function for parameter parse on hotglug
  */
#define CAS_HOTPLUG_PARAM_SLOT(param) (param >> 16)

/*!
  The macro function for parameter parse on hotglug
  */
#define CAS_HOTPLUG_PARAM_CAMID(param) (param & 0xFFFF)

/*!
  The cas max slot number
  */
#define CAS_MAX_SLOT_NUM 4


/*!
  The cas invalid slot number
  */
#define CAS_INVALID_SLOT 0xFF

/*!
  The cas mail subject text max length, in unit of char
  */
#define CAS_MAIL_SUBJECT_MAX_LEN (80)

/*!
  The cas mail from text max length, in unit of char
  */
#define CAS_MAIL_FROM_MAX_LEN (21)

/*!
  The cas mail sender text max length, in unit of char
  */
#define CAS_MAIL_SENDER_MAX_LEN (11 * 3)

/*!
  The cas mail date text max length, in unit of byte
  */
#define CAS_MAIL_DATE_MAX_LEN (8)

/*!
  The cas mail body text max length, in unit of char
  */
#define CAS_MAIL_BODY_MAX_LEN (2000)

/*!
  The cas eigen value max length, in unit of byte
  */
#define CAS_EIGEN_CODE_MAX_LEN (32)

/*!
  The cas card serial number max length, in unit of byte
  */
#define CAS_CARD_SN_MAX_LEN (32)

/*!
  The cas manufacturer name max length
  */
#define CAS_MAXLEN_MANUFACTURERNAME (32)

/*!
  The cas card ID max length, in unit of byte
  */
#define CAS_CARD_ID_MAX_LEN (10)

/*!
  The cas STB ID max length, in unit of byte
  */
#define CAS_STB_SN_MAX_LEN (15)
/*!
the cas operator name max len
  */
#define CAS_OPERATOR_NAME_MAX_LEN (40)
/*!
the cas operator phone max len
  */
#define CAS_OPERATOR_PHONE_MAX_LEN (40)
/*!
the cas operator address max len
  */
#define CAS_OPERATOR_ADDR_MAX_LEN (80)
/*!
the cas operator address max len
  */
#define CAS_OPERATOR_ACLIST_MAX_LEN (20)
/*!
the cas product name max len
  */
#define CAS_PRODUCT_NAME_MAX_LEN (30)
/*!
the cas price max num
  */
#define CAS_MAX_NUM_PRICE (2)

/*!
the cas max pin len
  */
#define CAS_MAX_PIN_LEN (8)
/*!
the cas max email len
  */
#define CAS_MAX_EMAIL_NUM (100)
/*!
the cas max opertor len
  */
#define CAS_MAX_OPERTOR_NUM (5)
/*!
the cas max entitle len
  */
#define CAS_MAX_PRODUCT_ENTITLE_NUM (1100)
/*!
the cas max ipp len
  */
#define CAS_MAX_IPP_NUM (300)
/*!
the cas max ipp consume record len
  */
#define CAS_MAX_IPP_RECORD_NUM (100)
/*!
the cas max ipp consume content len
  */
#define CAS_MAX_IPP_CONTENT_NUM (40)

/*!
the cas max burse len
  */
#define CAS_MAX_BURSE_NUM (20)
/*!
the cas max burse len
  */
#define CAS_MAX_BURSE_VALUE_NUM (24)
/*!
  the max message title length
  */
#define CAS_MAX_MSG_TITLE_LEN (100)
/*!
  the max message data length
  */
#define CAS_MAX_MSG_DATA_LEN  (2048)
/*!
  The max smart card version length
  */
#define CAS_CARD_VER_MAX_LEN (32)
/*!
  The max cas  describe
  */
#define CAS_CARD_DESC_MAX_LEN (32)
/*!
  The max cas software version length
  */
#define CAS_VERSION_MAX_LEN (32)
/*!
  The max cas software version length
  */
#define CAS_NOTE_TEXT_MAX_LEN 1024

/*!
  The max store mail num in flash for topreal
  */
#define TRCAS_STORE_MAX_MAIL_NUM (20)
/*!
  The max store announce num in flash for topreal
  */
#define TRCAS_STORE_MAX_ANNOUNCE_NUM (20)
/*!
  The max cas ecm emm info num
  */
#define CAS_MAX_ECMEMM_NUM 8

/*!
  The max area code length
  */
#define AREA_CODE_MAX_LEN (32)

/*!
  The time max length
  */
#define CAS_DATE_MAX_LEN (8)

/*!
  The IPPV/IPPT program name max length
  */
#define CAS_MAX_IPP_MAX_NAME_LEN  (35)

/*!
  The Service name max length
  */
#define CAS_MAX_SERVICE_NAME_LEN (21)
/*!
  ECM filter continuous mode disabled
  */
#define ECM_FILTER_CONTINUOUS_DISABLED 0x01
/*!
  EMM filter continuous mode disabled
  */
#define EMM_FILTER_CONTINUOUS_DISABLED 0x02
/*!
  SN length
  */
#define CAS_SN_LENGTH (16)
/*!
  OP keyset count
  */
#define CAS_MAX_OP_KEYSET_COUNT (2)
/*!
  pop msg condition count
  */
#define CAS_MAX_CONDITION_COUNT (50)
  /*!
  the force channel expired time
  */
#define CAS_FORCE_CHANNEL_EXPIRED_TIME (5)
  /*!
  the force channel component count
  */
#define CAS_MAX_FORCE_CHANNEL_COMPONENT (5)
/*!
the cas operator address max len
  */
#define CAS_OPERATOR_ACLIST_MAX_LEN (20)
/*!
  the max len of the STB list
  */
#define CAS_MAX_STB_LIST_LEN (30)
/*!
  the force channel component count
  */
#define CAS_MAX_FORCE_CHANNEL_COMPONENT (5)

/*!
  the max num of the finger show position
  */
#define CAS_MAXNUM_FINGER_POSITION  4

/*!
  the max length of the smc provider
  */
#define MAX_SMCPROVIDER_NAME_LEN 10

/*!
  The type of cas Lib
  */
typedef enum
{
    CAS_LIB_TEST,
    CAS_LIB_FORMAL,
    CAS_LIB_BY20,
    CAS_LIB_BY30
}CAS_LIB_TYPE;

/*!
  The type of ca language
  */
typedef enum 
{
  CAS_LANGUAGE_CHINESE,
  CAS_LANGUAGE_ENGLISH,
  CAS_LANGUAGE_TOTAL,
}CA_LANG_TYPE;

/*!
  CA system module id
  */
typedef enum
{
  /*!
    ADT_MG(from SunZhuXunChi)
    */
  CAS_ID_ADT_MG,
  /*!
    Topreal
    */
  CAS_ID_TR,
      /*!
    KingVon
    */
  CAS_ID_KV,
    /*!
    TongFang
    */
  CAS_ID_TF,
    /*!
    Sumavision
    */
  CAS_ID_SV,
  /*!
    DVN
    */
  CAS_ID_DVN,
     /*!
    Suantong
    */
  CAS_ID_ST,
  /*!
    boyuan
    */
  CAS_ID_BY,
      /*!
    Conax for Konka
    */
  CAS_ID_CONAXK,
  /*!
    GS
    */
  CAS_ID_GS,
   /*!
    DESAI
   */
  CAS_ID_DS,
  /*!
    GuoTong
    */
  CAS_ID_GT,
  /*!
    wanfa
    */
  CAS_ID_WF,
  /*!
    SHIDA
    */
  CAS_ID_SHIDA,
  /*!
    P4V
    */
  CAS_ID_P4V,
   /*!
    YINGJI
    */
  CAS_ID_YJ,
  /*!
   IRDETO
   */
   CAS_ID_IRDETO,
     /*!
    ABV
    */
  CAS_ID_ABV,
    /*!
   JIE XIANG
   */
   CAS_ID_ZHH,
       /*!
    LIXIN
    */
  CAS_ID_LIXIN,
  /*!
    XSM
  */
  CAS_ID_XSM,
  /*!
    ONLY1
    */
  CAS_ID_ONLY1,
    /*!
    CRYPG
    */
  CAS_ID_CRYPG,
    /*!
    novel supertv
    */
  CAS_ID_NOVELSUPERTV,
    /*!
    sumavision
    */
  CAS_ID_SUMAVISION,
    /*!
    kingvon40
    */
  CAS_ID_KV40,
    /*!
    reserved for custom cas1
    */
  CAS_ID_CUSTOM1,
    /*!
    reserved for custom cas2
    */
  CAS_ID_CUSTOM2,
    /*!
    reserved for custom cas3
    */
  CAS_ID_CUSTOM3,
    /*!
    reserved for custom cas4
    */
  CAS_ID_CUSTOM4,
    /*!
    reserved for custom cas5
    */
  CAS_ID_CUSTOM5,
    /*!
    Unkown module
    */
  CAS_UNKNOWN,
  /*!
    The maximal number  of CA system modules
    */
  CAS_ID_MAX_NUM,
}cas_module_id_t;

/*!
  The event callback structure for CA system module
  */
typedef enum
{
/*!
    The card and STB has been paternered
    */
  CAS_PATERNER = 1,
/*!
    Neither card nor STB has been paternered
    */
  CAS_NOT_PATERNER,
}cas_paterner_sta_t;

/*!
  The rating level of film
  */
typedef enum
{
/*!
    The G level
    */
  CAS_FILM_RATING_G = 1,
/*!
    The PG level
    */
  CAS_FILM_RATING_PG,
/*!
    The PG_13 level
    */
  CAS_FILM_RATING_PG_13,
/*!
    The R level
    */
  CAS_FILM_RATING_R,
/*!
    The NC level
    */
  CAS_FILM_RATING_NC
}cas_film_rating_t;

/*!
  cas data overlay policy
  */
typedef enum
{
  /*!
    set id policy
    */
  POLICY_BY_ID,
  /*!
    set send time policy
    */
  POLICY_BY_SEND_TIME,
  /*!
    set save time policy
    */
  POLICY_BY_SAVE_TIME,
}cas_data_overlay_policy_t;

/*!
  The event callback structure for CA system module:CAS_E_ :the errors of cas;CAS_S_ :the status of cas;CAS_c_ :the cmd of cas
  */
typedef enum
{
/*!
    None event, clear event last time received
    */
  CAS_EVT_NONE = 0, //0x00000000,
/*!
    the card requested insert to the stb
    */
  CAS_E_CARD_REGU_INSERT,
/*!
    SCRAMBLE PRO,PROJECT_KONKA
    */
  CAS_E_PROG_SCARMBLED,
/*!
    can't identify the card
    */
  CAS_E_ILLEGAL_CARD,
/*!
    Card init fail
    */
  CAS_E_CARD_INIT_FAIL,
/*!
    Card module fail
    */
  CAS_E_CARD_MODULE_FAIL,
/*!
    Card overdue
    */
  CAS_E_CARD_OVERDUE,
/*!
    Card reset fail
    */
  CAS_E_CARD_RST_FAIL,
/*!
    error card sleep
    */
  CAS_E_CARD_SLEEP,
/*!
    error card freeze
    */
  CAS_E_CARD_FREEZE,
/*!
    error card pause
    */
  CAS_E_CARD_PAUSE,
/*!
    YXSB: CDCA_MESSAGE_CURTAIN_TYPE
    */
  CAS_E_CARD_CURTAIN,
/*!
    YXSB: CDCA_CURTAIN_CANCLE
    */
  CAS_E_CARD_CURTAIN_CANCLE,
/*!
    YXSB: CDCA_CURTAIN_OK
    */
  CAS_E_CARD_CURTAIN_OK,
/*!
    YXSB: CDCA_MESSAGE_CARDTESTSTART_TYPE
    */
  CAS_E_CARD_TESTSTART,
/*!
    YXSB: CDCA_MESSAGE_CARDTESTFAILD_TYPE
    */
  CAS_E_CARD_TESTFAILD,
/*!
    YXSB: CDCA_MESSAGE_CARDTESTSUCC_TYPE
    */
  CAS_E_CARD_TESTSUCC,
/*!
    YXSB: CDCA_MESSAGE_NOCALIBOPER_TYPE
    */
  CAS_E_CARD_NOCALIBOPER,
/*!
    YXSB: CDCA_MESSAGE_STBLOCKED_TYPE
    */
  CAS_E_CARD_STBLOCKED,
/*!
    YXSB: CDCA_MESSAGE_STBFREEZE_TYPE
    */
  CAS_E_CARD_STBFREEZE,
/*!
    Set stb-card dis-partnered
    */
  CAS_E_CARD_DIS_PARTNER,
/*!
    STB and Card Partner failed
    */
  CAS_E_CARD_PARTNER_FAILED,
/*!
    STB and Card Partner failed,need to get error pin code
    */
  CAS_E_CARD_PARTNER_ERROR_CODE,
/*!
    Card upgrade failed
    */
  CAS_E_CARD_UPG_FAILED,
/*!
    unlock the parent locking
    */
  CAS_E_CARD_CHILD_UNLOCK,
/*!
    card not't in work time
    */
  CAS_E_CARD_OUT_WORK_TIME,
/*!
    No information of Operaters
    */
  CAS_E_NO_OPTRATER_INFO,
/*!
    The PPV account no money rest
    */
  CAS_E_CADR_NO_MONEY,
/*!
    For Ipp feature: program book success, but money lack, please charge
    */
  CAS_E_CARD_MONEY_LACK,
/*!
    For Ipp feature: burse not exist
    */
  CAS_E_CARD_SLOT_NOT_FOUNT,
/*!
    For Ipp feature: need PIN verify
    */
  CAS_E_NEED_PIN_PASS,
/*!
    For Ipp feature: invalid price
    */
  CAS_E_INVALID_PRICE,
/*!
    For Ipp feature: unknown error
    */
  CAS_E_UNKNOWN,
/*!
    For Ipp feature: product experied
    */
  CAS_E_PRODUCT_EXPERID,
/*!
    For Ipp feature: product has exist, can not operate
    */
  CAS_E_PRODUCT_EXIST,
/*!
    For Ipp feature: has no space
    */
  CAS_E_NO_SPACE,
/*!
    For Ipp feature: product has not found
    */
  CAS_E_PROD_NOT_FOUND,
/*!
    For Ipp feature: IC communicate param error
    */
  CAS_E_IC_COMMUNICATE,
/*!
    For Ipp feature: Operator ID invalid
    */
  CAS_E_TVS_NOT_FOUND,
/*!
    the program not authorized
    */
  CAS_E_PROG_UNAUTH,
/*!
        the program blackout
    */
  CAS_E_BLACKOUT,
/*!
    The card has been locked
    */
  CAS_E_CARD_LOCKED,
/*!
    The service has been locked
    */
  CAS_E_SERVICE_LOCKED,
/*!
    The son card has not been activated
    */
  CAS_E_CADR_NOT_ACT,
/*!
    The program not buy, you are previewing
    */
  CAS_E_PROGRAM_PREVIEW,
/*!
    The program not buy, Free preview is over
    */
  CAS_E_FREEPREVIEW_OVER,
/*!
    The program not buy, Free preview is over
    */
  CAS_E_FREEPREVIEW_ALLOVER,  
/*!
    The IC card not service
    */
  CAS_E_IC_NOT_SERVICE,
/*!
    The zone code error
    */
  CAS_E_ZONE_CODE_ERR,
/*!
    Not received operator ID
    */
  CAS_E_NOT_RECEIVED_SPID,
/*!
    use IPPV CANNTDEL
    */
  CAS_E_IPPV_CANNTDEL,
/*!
    error cw
    */
  CAS_E_CW_INVALID,
/*!
    error view locked
    */
  CAS_E_VIEWLOCK,
/*!
  pin error
  */
  CAS_E_PIN_INVALID,
/*!
  pin is locked
  */
  CAS_E_PIN_LOCKED,
/*!
  pin need recover
  */
  CAS_E_RECOVER_PIN,
/*!
card communication error
  */
  CAS_E_IC_CMD_FAIL,
/*!
invalid system id
  */
  CAS_E_SYSTEM_ID_INVALID,
/*!
memory alloc fail
  */
  CAS_E_MEM_ALLOC_FAIL,
 /*!
can't find the date of requirement
  */
  CAS_E_DATA_NOT_FIND,
/*!
Lack of space given
  */
  CAS_E_DATASPACE_SMALL,
/*!
  Data length error
  */
  CAS_E_DATA_LEN_ERROR,
/*!
card had been paired with other stb
  */
CAS_E_CARD_PAIROTHER,
/*!
card had no room to save data
  */
  CAS_E_CARD_NO_ROOM,
/*!
exceed the max count
 */
  CAS_E_EXCEED_MAX_COUNT,
/*!
card had not support  The feature
  */
  CAS_ERROR_CARD_NOTSUPPORT,
/*!
parameter error
  */
  CAS_E_PARAM_ERR,
/*!
parameter error
  */
  CAS_E_UNKNOW_ERR,
/*!
parameter error
  */
  CAS_E_INVALID_POINTER,
/*!
program status error
  */
  CAS_E_PROG_STATUS_INVALID,
/*!
parent control
  */
 CAS_E_WATCHRATING_INVALID,
/*!
not arrive feeding time
  */
 CAS_E_FEEDTIME_NOT_ARRIVE,
/*!
  IPP info(product/record) invalid
  */
 CAS_E_IPP_INFO_ERROR,
 /*!
  nvram error from smart card
  */
 CAS_E_NVRAM_ERROR,
/*!
   feed data invalid
  */
  CAS_E_FEED_DATA_ERROR,
/*!
   access smart card error
   */
  CAS_E_ACCESS_CARD_ERROR,
/*!
   Query control message  failed
  */
  CAS_E_QUERY_CTRL_MSG_ERROR,
/*!
   Control message  not available
  */
  CAS_E_CTRL_MSG_NOT_AVAILABLE,
/*!
   command not OK returned
   */
  CAS_E_IOCMD_NOT_OK,
/*!
    card module eeprom failure
    */
  CAS_E_CARD_MODULE_EEPROM_FAILED,
/*!
    card eeprom failure
    */
  CAS_E_CARD_EEPROM_FAILED,
/*!
   PMT format wrong
  */
  CAS_E_PMT_FORMAT_WRONG,
/*!
   no service available
   */
   CAS_E_SEVICE_NOT_AVAILABLE,
/*!
   no service available
   */
   CAS_E_SEVICE_SCRAMBLED,
/*!
   no service available
   */
   CAS_E_SEVICE_SCRAMBLED2,
/*!
   no service available
   */
   CAS_E_SEVICE_UNKNOWN,
/*!
   service not running
   */
   CAS_E_SEVICE_NOT_RUNNING,
/*!
   service name exceed the max length
   */
   CAS_E_SEVICE_EXCEED_MAX_LEN,
/*!
   not allowed in this broadcaster
   */
   CAS_E_NOT_ALLOWED_BROADCASTER,
/*!
   active emergency broadcasting
   */
   CAS_E_ACTIVE_EMERGENCY_BROADCASTING,
/*!
   run emergency broadcasting
   */
   CAS_E_RUN_EMERGENCY_BROADCASTING,
/*!
   ERROR E18
   */
   CAS_E_ERROR_E18,
/*!
   ERROR E21
   */
   CAS_E_ERROR_E19,
/*!
   ERROR E21
   */
   CAS_E_ERROR_E21,
/*!
   ERROR E22
   */
   CAS_E_ERROR_E22,
/*!
   ERROR E29
   */
   CAS_E_ERROR_E29,
/*!
   ERROR E30
   */
   CAS_E_ERROR_E30,
/*!
   not allowed in this region
   */
   CAS_E_NOT_ALLOWED_REGION,
/*!
   not allowed in this country
   */
   CAS_E_NOT_ALLOWED_COUNTRY,
/*!
   user define attribute blocking
   */
   CAS_E_ATTRIBUTE_BLOCKING,
/*!
   parent control locked
   */
   CAS_E_PARENTAL_CTRL_LOCKED,
/*!
   no authorization data
   */
   CAS_E_NO_AUTH_DATA,
/*!
   no authorization STB
   */
   CAS_E_NO_AUTH_STB,
/*!
   no signal
   */
   CAS_E_NO_SIGNAL,
/*!
   mail not belong to this user, can not read
   */
   CAS_E_MAIL_CAN_NOT_READ,
/*!
   mail not belong to this user, can not delete
   */
   CAS_E_MAIL_CAN_NOT_DELETE,
/*!
   Get Card info failed
   */
   CAS_E_CARD_INFO_GET,
/*!
   Get Card data failed
   */
   CAS_E_CARD_DATA_GET,
/*!
   Smart Card not found
   */
   CAS_E_CARD_NOT_FOUND,
/*!
   the end user have not buy the program
  */
   CAS_E_USER_NOT_BUY_PPV,
/*!
   The smart card not registered
  */
   CAS_E_NOT_REG_CARD,
/*!
   The smart card calculate GKAK failed
  */
   CAS_E_CALCU_GKAK,
/*!
   The smart card update failed
  */
   CAS_E_CARD_FAILED,
/*!
   The smart card external authentication failed
  */
   CAS_E_EXTER_AUTHEN,
/*!
   The smart card  internal authentication failed
  */
   CAS_E_INTER_AUTHEN,
/*!
   The authentication will expire
  */
   CAS_E_AUTHEN_RECENT_EXPIRE,
/*!
   The authentication expired
  */
   CAS_E_AUTHEN_EXPIRED,
/*!
   The smart card register failed
  */
   CAS_E_CARD_REG_FAILED,
/*!
   The smart card authentication failed
  */
   CAS_E_CARD_AUTH_FAILED,
/*!
   The smart card update status,please reset the stb
  */
   CAS_E_CARD_RENEW_RESET_STB,
/*!
   The child card expired
  */
   CAS_E_CARD_EXPIRED,
/*!
   The  card authen overdue
  */
   CAS_E_CARD_AUTHEN_OVERDUE,
/*!
   The  card operate faile
  */
   CAS_E_CARD_OPERATE_FAILE,
/*!
   The EMM invalid
  */
   CAS_E_EMM_INVALID,
/*!
   The EMM invalid
  */
   CAS_E_EMM_ERROR,
/*!
   The ECM invalid
  */
   CAS_E_ECM_ERROR,
/*!
   The ECM invalid
  */
   CAS_E_NO_ECM,
/*!
   The ECM return error
  */
   CAS_E_ECM_RETURN_ERROR,
/*!
   The ECM or EMM return error
  */
   CAS_E_ECMEMM_RETURN_ERROR,
/*!
   The ECM or EMM return p1p2 error
  */
   CAS_E_ECMEMM_RETURN_P1P2_ERROR,
/*!
   The ECM or EMM return p3 error
  */
   CAS_E_ECMEMM_RETURN_P3_ERROR,
/*!
   The unknown error
  */
   CAS_E_UNKNOWN_ERROR,
/*!
    Adapter layer card init fail
    */
  CAS_E_ADPT_CARD_INIT_FAIL,
/*!
    Adapter layer card reset fail
    */
  CAS_E_ADPT_CARD_RST_FAIL,
/*!
    Adapter layer card reset fail
    */
  CAS_E_ADPT_ILLEGAL_CARD,
/*!
    GS CAS ERR:SW1SW2_MEMORY_ERROR
    */
  CAS_E_SW1SW2_MEMORY_ERROR,
/*!
    GS CAS ERR:SW1SW2_WRONG_LC_LENGTH
    */
  CAS_E_SW1SW2_WRONG_LC_LENGTH,
/*!
    GS CAS ERR:SW1SW2_INCORRECT_DATA
    */
  CAS_E_SW1SW2_INCORRECT_DATA,
/*!
    GS CAS ERR:SW1SW2_INCORRECT_PARA
    */
  CAS_E_SW1SW2_INCORRECT_PARA,
/*!
    GS CAS ERR:SW1SW2_WRONG_LE_LENGTH
    */
  CAS_E_SW1SW2_WRONG_LE_LENGTH,
/*!
    GS CAS ERR:SW1SW2_INS_NOT_SUPPORTED
    */
  CAS_E_SW1SW2_INS_NOT_SUPPORTED,
/*!
    GS CAS ERR:SW1SW2_CLA_NOT_SUPPORTED
    */
  CAS_E_SW1SW2_CLA_NOT_SUPPORTED,
/*!
    GS CAS ERR:SW1SW2_INCORRECT_SUPER_CAS_ID
    */
  CAS_E_SW1SW2_INCORRECT_SUPER_CAS_ID,
/*!
    GS CAS ERR:SW1SW2_INCORRECT_SMARTCARD_ID
    */
  CAS_E_SW1SW2_INCORRECT_SMARTCARD_ID,
/*!
    GS CAS ERR:SW1SW2_NOT_CHILD_MODE
    */
  CAS_E_SW1SW2_NOT_CHILD_MODE,
/*!
    GS CAS ERR:SW1SW2_CHILD_AUTH_TIMEOUT
    */
  CAS_E_SW1SW2_CHILD_AUTH_TIMEOUT,
/*!
    Card init success
    */
      CAS_S_CARD_INIT_SUCCESS,
/*!
    Card reset success
    */
  CAS_S_CARD_RST_SUCCESS,
/*!
    STB and Card Partner successed
    */
   CAS_S_CARD_PARTNER_SUCCESS,
/*!
    Card upgrade started
    */
  CAS_S_CARD_UPG_START,
/*!
    Card receiving data
    */
  CAS_S_CARD_RECEIVE_DATA,
/*!
    Card upgrading
    */
  CAS_S_CARD_UPGRADING,
/*!
    Card upgrade successed
    */
  CAS_S_CARD_UPG_SUCCESS,
/*!
    Card insert
    */
  CAS_S_CARD_INSERT,
/*!
    Card removed
    */
  CAS_S_CARD_REMOVE,
/*!
    Card module removed
    */
  CAS_S_CARD_MODULE_REMOVE,
/*!
    Card need upg
    */
  CAS_S_CARD_NEED_UPG,
/*!
    There are programs authorized
    */
  CAS_S_PROG_AUTH,
/*!
    Set stb-card partnered
    */
  CAS_S_CARD_STB_PARTNER,
/*!
    The zonecode has been updated
    */
  CAS_S_MODIFY_ZONECODE,
/*!
    The card balance updated, you can get the balance
    */
  CAS_S_CARD_UPDATE_BALANCE,
/*!
   The card authen updating
  */
   CAS_S_CARD_AUTHEN_UPDATING,
/*!
    the mother card requested insert to the stb
    */
  CAS_S_MOTHER_CARD_REGU_INSERT,
/*!
    the son card requested insert to the stb
    */
  CAS_S_SON_CARD_REGU_INSERT,
/*!
    programs authorized  changes
    */
  CAS_S_PROG_AUTH_CHANGE,
/*!
    clear cur display
    */
  CAS_S_CLEAR_DISPLAY,
/*!
    clear cur display finger msg
    */
  CAS_S_CLEAR_DISPLAY_FINGER_MSG,
/*!
    clear cur display important email
    */
  CAS_S_CLEAR_DISPLAY_IMPORTANT_MSG,
/*!
    clear cur display
    */
  CAS_S_GET_KEY,
/*!
   service not scrambled
   */
   CAS_S_SEVICE_NOT_SCRAMBLED,
/*!
   checking smart card
  */
   CAS_S_CARD_CHECKING,
/*!
   service discrambling
  */
   CAS_S_SERVICE_DISCRAMBLING,
/*!
    Adapter layer card insert notify
    */
  CAS_S_ADPT_CARD_INSERT,
/*!
    Adapter layer card removed
    */
  CAS_S_ADPT_CARD_REMOVE,
/*!
    Adapter layer card reset success
    */
  CAS_S_ADPT_CARD_RST_SUCCESS,
/*!
    zone check end
    */
  CAS_S_ZONE_CHECK_END,
/*!
    nim lock fail
    */
  CAS_S_NIM_LOCK_FAIL,
/*!
    email Space Exhaust
    */
  CAS_C_SHOW_SPACE_EXHAUST,
/*!
    announce Space Exhaust
    */
  CAS_ANNOUNCE_SHOW_SPACE_EXHAUST,
/*!
    There is a new email received, need to get mail header and body
    */
  CAS_C_SHOW_NEW_EMAIL,
/*!
    There is a new email received, need to get mail header and body
    */
  CAS_C_HIDE_NEW_EMAIL,
/*!
    hide the osd message
    */
  CAS_C_HIDE_OSD_TEXT,
/*!
    The finger message received, WE should  show the dard id
    */
  CAS_C_SHOW_NEW_FINGER,
/*!
    The finger message received, WE should  show the dard id
    */
  CAS_C_HIDE_NEW_FINGER,
/*!
    The notify received, We should  show the dard id
    */
  CAS_C_STB_NEW_NOTIFY,
/*!
    The Announcement received, WE should  show the dard id
    */
  CAS_C_STB_NEW_ANNOUNCE,
/*!
    received urgent service, should get service id
    */
  CAS_C_STB_URGENT_SERVICE,
/*!
    received urgent Broadcast, should get service id
    */
  CAS_C_STB_URGENT_BROADCAST,
/*!
    cancle urgent Broadcast, should cancle
    */
  CAS_C_STB_CANCLE_URGENT_BROADCAST,
/*!
    The IPPV message received,
    */
  CAS_C_IPPV_BUY_OR_NOT,
/*!
   hide ippv dig
    */
  CAS_C_HIDE_IPPV,
/*!
    The IPPT message received,
    */
  CAS_C_IPPT_BUY_OR_NOT,
/*!
   hide ippt dig
    */
  CAS_C_HIDE_IPPT,
/*!
    The IPP message received,
    */
  CAS_C_IPP_BUY_OR_NOT,
/*!
    The IPP prg notify,
    */
  CAS_C_IPP_PROG_NOTIFY,
/*!
    The IPP prg over date,
    */
  CAS_C_IPP_OVERDUE,
/*!
   hide ipp dig
    */
  CAS_C_HIDE_IPP,
/*!
  The message received
  */
  CAS_C_SHOW_MSG,
/*!
  Received OSD message
  */
  CAS_C_SHOW_OSD,
/*!
    show osd on fullscreen top,
    */
  CAS_C_SHOW_OSD_UP,
/*!
    show osd on fullscreen bottom,
    */
  CAS_C_SHOW_OSD_DOWN,
/*!
    show osd on fullscreen,
    */
  CAS_C_SHOW_OSD_FULLSCREEN,
/*!
    show osd on halfscreen,
    */
  CAS_C_SHOW_OSD_HALFSCREEN,
    /*!
  Hide FORCEOSD message
  */
  CAS_C_HIDE_FORCEOSD,
/*!
    hide osd on fullscreen top,
    */
  CAS_C_HIDE_OSD_UP,
/*!
    hide osd on fullscreen bottom,
    */
  CAS_C_HIDE_OSD_DOWN,
/*!
    hide osd on fullscreen,
    */
  CAS_C_HIDE_OSD_FULLSCREEN,
/*!
    hide osd on halfscreen,
    */
  CAS_C_HIDE_OSD_HALFSCREEN,
/*!
    son card need feeding,
    */
  CAS_C_SON_CARD_NEED_FEED,
/*!
    rrequ nit section,
    */
  CAS_C_REQU_NIT_SECITON,
/*!
    Detitle Received,
    */
  CAS_C_DETITLE_RECEIVED,
/*!
   Detitle Space Small
    */
  CAS_C_DETITLE_SPACE_SMALL,
/*!
   Detitle All Readed
    */
  CAS_C_DETITLE_ALL_READED,
/*!
  No entitle info
  */
  CAS_C_NO_ENTITLE,
/*!
  Valid entitle, but limited
  */
  CAS_C_ENTITLE_LIMIT,
/*!
   Entitle closed
  */
  CAS_C_ENTITLE_CLOSED,
/*!
  Entitle paused
  */
  CAS_C_ENTITLE_PAUSED,
/*!
  BYCAS: UserCdn_Limit_Ctrl_Enable
  */
  CAS_C_USERCDN_LIMIT_CTRL_ENABLE,
/*!
  BYCAS: UserCdn_Limit_Ctrl_Disable
  */
  CAS_C_USERCDN_LIMIT_CTRL_DISABLE,
/*!
  BYCAS: ZoneCdn_Limit_Ctrl_Enable
  */
  CAS_C_ZONECDN_LIMIT_CTRL_ENABLE,
/*!
  BYCAS: ZoneCdn_Limit_Ctrl_Disable
  */
  CAS_C_ZONECDN_LIMIT_CTRL_DISABLE,
/*!
  BYCAS: PAIRED_OtherCARD
  */
  CAS_E_PAIRED_OTHERCARD_ERROR,
/*!
  BYCAS: PAIRED_OtherSTB
  */
  CAS_E_PAIRED_OTHERSTB_ERROR,
/*!
  BYCAS: masterslave paired ok
  */
  CAS_C_MASTERSLAVE_PAIRED_OK,
/*!
  BYCAS: masterslave paired TIMEUPDATE
  */
  CAS_C_MASTERSLAVE_PAIRED_TIMEUPDATE,
/*!
  BYCAS: masterslave needpaired
  */
  CAS_C_MASTERSLAVE_NEEDPAIRED,
/*!
  BYCAS:  cancel paired ok
  */
  CAS_C_CANCEL_PAIRED_OK,
/*!
  BYCAS: CA_CARD_ZONE_INVALID
  */
  CAS_C_CARD_ZONE_INVALID,
/*!
  BYCAS: CARD_ZONE_CHECK_ENTER
  */
  CAS_C_CARD_ZONE_CHECK_ENTER,
/*!
  BYCAS: CARD_ZONE_CHECK_OK
  */
  CAS_C_CARD_ZONE_CHECK_OK,
/*!
  TRCAS: Decoder info get
  */
  CAS_C_DECODER_INFO,
/*!
  TRCAS: service info
  */
  CAS_C_SERVICE_INFO,
/*!
  TRCAS: SC access
  */
  CAS_C_SC_ACCESS,
/*!
  TRCAS: Pin code change notify
  */
  CAS_C_PIN_CODE_CHANGE,
/*!
  TRCAS: Pin code check notify
  */
  CAS_C_PIN_CODE_CHECK,
/*!
  TRCAS: ECM message notify
  */
  CAS_C_MONITOR_ECM,
/*!
  TRCAS: EMM message notify
  */
  CAS_C_MONITOR_EMM,
/*!
  TRCAS: ECM or EMM message notify
  */
  CAS_C_IPP_NOTIFY,
/*!
  TRCAS: IPP info update notify
  */
  CAS_C_IPP_INFO_UPDATE,
/*!
  TRCAS: email message notify
  */
  //CAS_C_MAIL,  //CAS_C_SHOW_NEW_EMAIL
/*!
  TRCAS: Announce message notify
  */
  //CAS_C_ANNOUNCE, //CAS_C_STB_NEW_ANNOUNCE
/*!
  TRCAS: ECM or EMM message notify
  */
  CAS_C_FORCE_CHANNEL,
/*!
  disable force channel
  */
  CAS_C_UNFORCE_CHANNEL,
/*!
  enable stb-smc partner
  */
  CAS_C_EN_PARTNER,
/*!
  disable stb-smc partner
  */
  CAS_C_DIS_PARTNER,
/*!
  set mother/son card
  */
  CAS_C_SET_SON_CARD,
/*!
  TRCAS: finger print message notify
  */
//  CAS_C_FINGER_PRINT, // CAS_C_HIDE_NEW_FINGER
/*!
  TRCAS: EMM message download
  */
  CAS_C_EMM_DOWNLOAD,
/*!
  TRCAS: EMM changepin
  */
  CAS_C_EMM_CHANGEPIN,
/*!
  TRCAS: Notify expiry
  */
  CAS_C_NOTIFY_EXPIRY_STATE,
/*!
  TRCAS: Notify current state
  */
  CAS_C_NOTIFY_CURRENT_STATE,
/*!
  TRCAS: short message notify
  */
  CAS_C_NOTIFY_SHORT_MESSAGE,
/*!
  ADT_MG: sevice locked notify
  */
  CAS_C_EMM_APP_STOP,
/*!
  ADT_MG: service unlocked notify
  */
  CAS_C_EMM_APP_RESUME,
/*!
  ADT_MG: program un-authentication notify
  */
  CAS_C_EMM_PROG_UNAUTH,
/*!
  ADT_MG: program authentication
  */
  CAS_C_EMM_PROG_AUTH,
/*!
  ADT_MG: update group number
  */
  CAS_C_EMM_UPDATE_GNO,
/*!
  ADT_MG: child unlock
  */
  CAS_C_EMM_CHILD_UNLOCK,
/*!
  ADT_MG: smart card upgrade
  */
  CAS_C_EMM_UPDATE_GK,
/*!
  ADT_MG: received remote charge command
  */
  CAS_C_EMM_CHARGE,
/*!
  ADT_MG: received finger identify instruction
  */
  CAS_C_EMM_FIGEIDENT,
/*!
  ADT_MG: received master-son card setting instruction
  */
  CAS_C_EMM_SONCARD,
/*!
  ADT_MG: received urgent service identify instruction
  */
  CAS_C_EMM_URGENT_SERVICE,
/*!
  ADT_MG: received modify smart character command
  */
  CAS_C_EMM_UPDATE_COUT,
/*!
  ADT_MG: update athentication
  */
  CAS_C_NOTIFY_UPDATE_CRT,
/*!
  ADT_MG: NOP
  */
  CAS_C_EMM_NOP,
/*!
  ADT_MG: update cotrol code
  */
  CAS_C_NOTIFY_UPDATE_CTRLCODE,
/*!
  ADT_MG: EMM refresh
  */
  CAS_C_NOTIFY_EMM_REFRESH,
/*!
  ADT_MG: EMM refresh
  */
  CAS_C_NOTIFY_EMM_STRATEGY,
/*!
  ADT_MG: not entire mail
  */
  CAS_C_NOTIFY_NOT_ENTIRE_MAIL,
/*!
  ADT_MG: display specified message(Refer porting guide page 31)
  */
  CAS_C_UPDATE_BALANCE,
/*!
  ADT_MG: display PPV logo on screen corner(Refer porting guide page 41,42)
  */
  CAS_C_PPV_LOGO_DISP,
/*!
  ADT_MG: display authentication logo on screen corner(Refer porting guide page 41,42)
  */
  CAS_C_AUTH_LOGO_DISP,
  /*!
    The program is IPPV, please confirm buy on "IPPV proram confirm/cancle" menu
  */
  CAS_E_IPPV_NO_CONFIRM,
  /*!
    The IPPV program not book and cofirm buy, can not watch
  */
  CAS_E_IPPV_NO_BOOK,
  /*!
    The program is IPPT, please confirm buy on "IPPV proram confirm/cancle" menu
  */
  CAS_E_IPPT_NO_CONFIRM,
  /*!
    The IPPT program not book and cofirm buy, can not watch
  */
  CAS_E_IPPT_NO_BOOK,
  /*!
    Sumavision: import mail should pop display
  */
  CAS_C_IMPORTTANT_MAIL_DISPLAY,
  /*!
    P4V: display user number
  */
  CAS_E_DISPLAY_USER_NUMBER,
  /*!
    P4V: STB not active
  */
  CAS_E_INACTIVE,
  /*!
    P4V: active error
  */
  CAS_E_ACTIVE_ERROR,
  /*!
    P4V: unsupport frontend
  */
  CAS_E_UNSUPPORTED_FRONTEND,
  /*!
    P4V: gca blocked
  */
  CAS_E_GCA_BLOCKED,
  /*!
    P4V: blacklist
  */
  CAS_E_BLACKLISTED,
  /*!
    P4V: user frozen
  */
  CAS_E_USER_FROZEN,
  /*!
    P4V: not in working period
  */
  CAS_E_NOT_IN_WORKING_PERIOD,
  /*!
    P4V: no entitlement
  */
  CAS_E_NO_ENTITLEMENT,
  /*!
    P4V: pop message
  */
  CAS_E_POP_MESSAGE,
  /*!
    DESAI: contion display
  */
  CAS_E_CONDITION_DISPLAY,
   /*!
    DESAI: contion diaappear
  */
  CAS_E_CONDITION_DISAPPEAR,
    /*!
   SHIDA: mother and son card not paired
  */
   CAS_C_MOTHERSON_HANDFAILED,
  /*!
      stb and card match ok
  */
  CAS_E_CARD_MATCH_OK,
   /*!
      rematch card and stb
  */
  CAS_E_CARD_REMATCH,
  /*!
    P4V: film rating
  */
  CAS_E_FILM_RATING,
  /*!
      CANCEL match card and stb
  */
  CAS_E_CARD_REMATCH_CANCEL,
  /*!
      email is full
  */
  CAS_C_EMAIL_FULL,
  /*!
      exgent broadcast
   */
   CAS_C_IEXIGENT_PROG,
   /*!
     exgent broadcast stop
   */
   CAS_C_IEXIGENT_PROG_STOP,
 /*!
     card unlock ok
   */
    CAS_C_CARD_UNLOCK_OK,
  /*!
    puser update
  */
    CAS_C_PURSER_UPDATE,
  /*!
    net error, card lock
  */
   CAS_C_PNET_ERR_LOCK,
  /*!
    system error, card lock
  */
   CAS_C_SYSTEM_ERR_LOCK,
  /*!
    net OK, card UNlock
  */
   CAS_C_PNET_ERR_UNLOCK,
  /*!
    system ok, card unlock
  */
   CAS_C_SYSTEM_ERR_UNLOCK,
 /*!
    The authentication update
   */
    CAS_E_AUTHEN_UPDATE,
 /*!
    card update
   */
   CAS_C_CARD_UPDATE_BEGIN,
 /*!
    diasplay card update progress
   */
    CAS_C_CARD_UPDATE_PROGRESS,
 /*!
    card update err
   */
    CAS_C_CARD_UPDATE_ERR,
 /*!
    card update end
   */
    CAS_C_CARD_UPDATE_END,
 /*!
   card need update
  */
   CAS_C_CARD_NEED_UPDATE,
 /*!
    change too fast
   */
   CAS_C_CARD_CHANGEPRO_TOO_FAST,
/*!
    address control open
*/
   CAS_C_CARD_ADDR_CTL_OPEN,
/*!
    address control close
*/
   CAS_C_CARD_ADDR_CTL_CLOSE,
/*!
    OVERDUE for yingji_cas,Dynamic string
*/
   CAS_E_YJ_OVERDUE,
/*!
   ABVCA: ota info get
*/
   CAS_C_OTA_INFO,
   /*!
   ABVCA: ECM finger
*/
   CAS_C_SHOW_ECM_FINGER,
  /*!
      WATCH_LIMIT for yxsb_cas,Dynamic string
  */
     CAS_E_CONTINUE_WATCH_LIMIT,
  /*!
    stb notification
  */
     CAS_C_STB_NOTIFICATION,
/*!
   The channel property is forbid to pvr
*/
  CAS_C_FORBID_PVR,
   /*!
  show cover finger
*/
  CAS_C_SHOW_COVER_FINGER,
/*!
    end,
    */
  CAS_EVT_ALL
}cas_event_id_t;


/*!
    ippv price type
    */
typedef enum
{
/*!
  can't send in  and can  record
  */
  CA_IPPVPRICETYPE_TPPTAP = 0,
/*!
  can't send in  and can't  record
  */
  CA_IPPVPRICETYPE_TPPNOTAP,
/*!
  can send in  and can record
  */
  CA_IPPVPRICETYPE_CPPTAP,
/*!
  can send in  and can't record
  */
  CA_IPPVPRICETYPE_CPPNOTAP
}ippv_price_e;

/*!
  Define a macro function to culculate array length in u32
  */
#define EVT_U32_LEN (((CAS_EVT_ALL - 1) >> 5) + 1)

/*!
  Define the CA system event call back function's prototype
  */
typedef RET_CODE (*cas_event_cb_t)(u32 slot, u32 event, u32 param, u32 cam_id, u32 context);

/*! 
  UTC format time definition
  */
typedef struct
{
  /*!
    timezonebias
    */
  s32 timezonebias;  
  /*!
    Year
    */
  u16 year;
  /*!
    Month
    */
  u8 month;
  /*!
    Day
    */
  u8 day;
  /*!
    Hour
    */
  u8 hour;
  /*!
    Minute
    */
  u8 minute;
  /*!
    Sec
    */
  u8 second;  
  /*!
    researved.
    */
  u8 reserved;
}cas_time_t;

/*!
  The CAS adapter layer configuation parameters
  */
typedef struct
{
  /*!
      cas type
      */
  u8 cas_type;  // 0 for cas with smartcard; 1 for cas without smartcard
  /*!
      task priority
      */
  u8 task_prio;
  /*!
      data task priority
      */
  u8 data_task_prio;
  /*!
      card slot number
      */
  u8 slot_num;
  /*!
      task stack pointer
      */
  u32 *p_task_stack;
  /*!
      data task stack pointer
      */
  u32 *p_data_task_stack;
  /*!
      task stack size
      */
  u32 stack_size;
  /*!
      data task stack size
      */
  u32 data_stack_size;
  /*!
      the smart card drivers corresponding to the specific slot
      */
  void *p_smc_drv[CAS_MAX_SLOT_NUM];
/*!
    Read nvram data
    */
  u32 (*nvram_read)(cas_module_id_t module_id, u16 node_id, u16 offset, u32 length, u8 *p_buffer);
  /*!
    Write nvram data
    */
  RET_CODE (*nvram_write)(cas_module_id_t module_id, u16 *p_node_id, u8 *p_buffer, u16 len);
  /*!
    Del nvram data
    */
  RET_CODE (*nvram_del)(cas_module_id_t module_id, u16 node_id);
  /*!
    Get nvram data node list
    */
  RET_CODE (*nvram_node_list)(cas_module_id_t module_id, u16 *p_buffer, u16 *p_max);
}cas_adapter_cfg_t;

/*!
  The CAS module configuation parameters
  */
typedef struct
{
/*!
    task priority, if is 0, means no task needed
    */
  u8 task_prio;
  /*!
  The end task prio for topreal
  */
  u8 end_task_prio;
  /*!
  The queue message manager task priority
  */
  u8 queue_task_prio;
  /*!
  The abv uart task priority
  */
  u8 cas_uart_task_prio;
  /*!
    max support mail num
    */
  u8 max_mail_num;
  /*!
    mail policy see cas_data_overlay_policy_t
    */
  u8 mail_policy;
  /*!
    the flag to specify if ca module will be in charge of card resetting
    */
  u8 card_reset_by_cam;
/*!
  The level of initializing MCELL. Normally is 0. The special card marriage
  solution will be used if iLevel is set to 1.
  */
  s16 level;
/*!
    task stack pointer
    */
  u32 *p_task_stack;
/*!
    task stack size
    */
  u32 stack_size;
/*!
   flash_start_adr
    */
  u32 flash_start_adr;
/*!
   flash_size
    */
  u32 flash_size;
/*!
   user_id
    */
  u8 *p_user_id;
/*!
   user_id_length
    */
  u8 user_id_len;
/*!
   hid
    */
  u8 *p_hid;
/*!
   hid_length
    */
  u8 hid_len;
/*!
    The handle of demux device
    */
  void *p_dmx_dev;
/*!
  set uart ca flag
  */
  u8 uart_flag;
/*!
  NIT PDSD data
  */
u32 private_data_specifier;
/*!
  filter mode
  0x01  ECM continuous mode disabled
  0x02  EMM continuous mode disabled
  */
  u8 filter_mode;
/*!
  write and read flash task interval (ms)
  */
  u32 wrt_read_inv;
/*!
    The callback of nvram reading function
    */
  RET_CODE (*nvram_read)(u32 offset,
              u8 *p_buf, u32 *size);
/*!
    The callback of nvram writing function
    */
  RET_CODE (*nvram_write)(u32 offset,
              u8 *p_buf, u32 size);
/*!    
    The callback of nvram erase function
    */
  RET_CODE (*nvram_erase)(u32 offset, u32 size);

/*!
    The callback of Virtual file open function
    */
  u32 (*file_open)(u8 *p_filename, u8 *p_mode, u8 level);
/*!
    The callback of Virtual file nvram reading function
    */
  RET_CODE (*file_read)(u32 node_id, u8 *p_buf, u32 size);
/*!
    The callback of Virtual file nvram writing function
    */
  RET_CODE (*file_write)(u32 node_id, u8 *p_buf, u32 size);
/*!
    The callback of machine serial number getting
    */
  RET_CODE (*machine_serial_get)(u8 *p_buf, u32 size);
/*!
  The callback of query program check
  */
  RET_CODE (*query_check)(u16 net_id, u16 ts_id, u16 sev_id);
/*!
    The type of cas lib
    */
  CAS_LIB_TYPE cas_lib_type;
/*!
    the area limit
    */
  BOOL area_limit_free;
/*!
    The callback of software version of STB
    */
  RET_CODE (*software_version_get)(u8 *p_buf, u32 size);
/*!
    The callback of hardware version of STB
    */
  RET_CODE (*hardware_version_get)(u8 *p_buf , u32 size);
/*!
    The callback of hardware version of STB
    */
  RET_CODE (*maucode_get)(u8 *p_buf , u32 size);
/*!
    The callback of get local time of STB
    */
  RET_CODE (*local_time_get)(cas_time_t *p_time, BOOL b_gmt_time);
}cas_module_cfg_t;

/*!
  The event callback structure for CA system module
  */
typedef enum
{
/*!
    The card and STB has already paternered
    */
  CAS_CARD_STB_PATERNERED = 1,
/*!
    Only the card has been paternered
    */
  CAS_CARD_PATERNERED,
/*!
    Only the STB has been paternered
    */
  CAS_STB_PATERNERED,
/*!
    Neither the card nor the STB has been paternered
    */
  CAS_CARD_NONE_PATERNERED,
/*!
    Undefined error code
    */
  CAS_PIN_ERR_UNDEF,
}cas_pin_err_code_t;

/*!
  The pin error information
  */
typedef struct
{
/*!
    old card password
    */
  u8 old_pin[CAS_MAX_PIN_LEN + 1];
/*!
    new card password
    */
  u8 new_pin[CAS_MAX_PIN_LEN + 1];
/*!
    ABVCA: match card password
    */
  u8 match_pin[CAS_MAX_PIN_LEN];
  /*!
  specify the pin length
  */
   u8 pin_len;
 /*!
  pin spare num
  */
   u8 pin_spare_num;
}cas_pin_modify_t;

/*!
  The pin status (enable or disable)
  */
typedef struct
{
/*!
    card password
    */
  u8 pin[CAS_MAX_PIN_LEN];
  /*!
  enable or disable
  */
   u8 status;
}cas_pin_status_t;

/*!
  The pin error information
  */
typedef struct
{
/*!
    pin error code, see cas_pin_err_code_t
    */
  u8 err_code;
/*!
    pin error cont
    */
  u8 err_cnt;
}cas_pin_err_t;

/*!
  The burse charge information
  */
typedef struct
{
/*!
    burse id
    */
  u8 burse_id;
/*!
    charge value
    */
  u32 charge_value;
}cas_burse_charge_t;

/*!
  The event callback structure for CA system module
  */
typedef enum
{
/*!
    The scrollbar, located at top
    */
  CAS_MSG_SCROLLBAR_TOP = 0x21,
/*!
    The scrollbar, located at bottom
    */
  CAS_MSG_SCROLLBAR_BOTTOM = 0x22,
}cas_msg_display_mode_t;


/*!
  The mail header format
  */
typedef struct
{
/*!
   uniquely identifies a Email
   */
    u32  m_id;
/*!
   0 :read mail, 1: new mail
   */
   u8  new_email;
/*!
  Important level of mail
  */
   u8  priority;
/*!
    The subject of this mail, in format of uni-code or GB2312.
    */
  u8 subject[CAS_MAIL_SUBJECT_MAX_LEN];
/*!
    The sender of this mail, in format of uni-code or GB2312
    */
  u8 sender[CAS_MAIL_SENDER_MAX_LEN];
/*!
    the date when this mail send, format: BCD code, 0xYYYYMMDDHHMMSS
    */
  u8 send_date[CAS_MAIL_DATE_MAX_LEN];
/*!
    the date when this mail creat, format: BCD code, 0xYYYYMMDDHHMMSS
    */
  u8 creat_date[CAS_MAIL_DATE_MAX_LEN];
/*!
    the length of mail boby
    */
  u16 body_len;
/*!
  display times of this mail
  */
  u32 display_times;
/*!
  creat mail order index
  */
  u32 index;
/*!
   uniquely identifies a Email
   */
  u32  special_id;
}cas_mail_header_t;

/*!
The operators info struct
*/
typedef struct
{
/*!
  email header
  */
  cas_mail_header_t  p_mail_head[CAS_MAX_EMAIL_NUM];
/*!
  max num of email
  */
  u16 max_num;
/*!
  new email num(not readed)
  */
  u16 new_mail_num;
/*!
  total email num, for sumavision it value may greater than 50
  */
  u16 total_email_num;
/*!
  max display email num, for sumavision it fixed value 50
  */
  u16 max_display_email_num;

}cas_mail_headers_t,cas_announce_headers_t;

/*!
  The mail body format
  */
typedef struct
{
/*!
    the data of mail boby, in format of uni-code or GB2312
    */
  u8 data[CAS_MAIL_BODY_MAX_LEN + 1];
/*!
   specify the data length
   */
  u16 data_len;
/*!
    mail id
    */
  u32 mail_id;
/*!
    for important mail: mail version
    */
  u32 mail_version;
/*!
    for important mail: sender name
    */
  u8 sender_name[CAS_MAIL_SENDER_MAX_LEN];
/*!
    for sv ca to check force email 
    */
  u8 reserved;
}cas_mail_body_t,cas_announce_body_t;

/*!
  The eigen value format
  */
typedef struct
{
/*!
    the data of eigen value, no format
    */
  u8 data[CAS_EIGEN_CODE_MAX_LEN];
/*!
   specify the data length
   */
  u8 data_len;
}cas_eigen_value_t;

/*!
  The work time status
  */
typedef enum
{
/*!
    Disable use
    */
  DISABLE_USE_WORK_TIME,
/*!
    Enable use
    */
  ENABLE_USE_WORK_TIME,
/*!
    Free work time
    */
  FREE_WORK_TIME,
}cas_work_time_status_t;

/*!
  The overdue inform
  */
typedef struct
{
/*!
    Overdue_inform enable 1, 0 disable
    */
    u8 state;
/*!
    Overdue_inform ahead days
    */
    u8 ahead_days;
}cas_overdue_inform_t;

/*!
    woke time
*/
typedef struct
{
/*!
    hour
*/
  u8 start_hour;
/*!
    minute
*/
  u8 start_minute;
/*!
    second
*/
  u8 start_second;
/*!
    hour
*/
  u8 end_hour;
/*!
    minute
*/
  u8 end_minute;
/*!
    second
*/
  u8 end_second;
/*!
    pin len
*/
  u8 pin[CAS_MAX_PIN_LEN + 1];
/*!
    pin len
*/
  u8 pin_len;
/*!
    pin spare num
*/
  u8 pin_spare_num;
/*!
    state 1 enable use, 0 disable use, 2 free work time
*/
  cas_work_time_status_t state;
/*!
    ABVCA: pin reset num
*/
  u8 pin_reset_num[5+1];
}cas_card_work_time_t;

/*!
  conax card info
  */
typedef struct
{
  /*!
    data flag
    */
  u8 ca_data_flg;
  /*!
    system version
    */
  u8 ca_sys_ver;
  /*!
    mat level
    */
  u8 ca_mat_level;
  /*!
    session id
    */
  u8 ca_session_id;
  /*!
    ca system id
    */
  u16 ca_sys_id;
  /*!
    country id
    */
  u16 ca_country_id;
  /*!
    card ua
    */
  u32 ca_card_ua;
  /*!
    cas version.
    */
  u8 ver[CAS_CARD_VER_MAX_LEN + 1];
}conax_card_info_t;

/*!
  conax card info
  */
typedef struct
{
/*!
    the card id
    */
  u32 card_id;
/*!
    cas version
    */
  u32 cas_ver;
/*!
    TRCAS: cas rate
    */
  u32 sc_cos_ver;
/*!
  PIN code lock state
  */
  u8 pin_lock_state;
/*!
    cas rating
    */
  u8 cas_rating;
/*!
    work start time
    */
  cas_card_work_time_t work_time;
/*!
    cas manufacturer name
    */
  u8 cas_manu_name[CAS_MAXLEN_MANUFACTURERNAME + 1];

}sumavision_card_info_t;


/*!
  crypg card info
  */
typedef struct
{
/*!
    the menu level
    */
  u8 level;
/*!
    the menu name
    */
  u8 menu_name[1024];

}crypg_card_info_t;

/*!
  The card information
  */
typedef struct
{
  /*!
    the card SN, in format of BCD code, it is a string in topreal cas
    TRCAS: The string buffer size should be larger than 24 bytes
    */
  u8 sn[CAS_CARD_SN_MAX_LEN + 1];
  /*!
    specify the sn length
    */
  u8 sn_len;
  /*!
    the STB SN
    */
  u8 stb_sn[CAS_STB_SN_MAX_LEN];
  /*!
    specify the stb sn length
    */
  u8 stb_sn_len;
  /*!
    the card version
    */
  u8 card_version[CAS_CARD_VER_MAX_LEN + 1];
  /*!
    specify the card version length
    */
  u8 card_ver_len;
  /*!
    the card issue date, in format of BCD code, 0xYYYYMMDD
    */
  u8 issue_date[4];
  /*!
    the card expire date, in format of BCD code, 0xYYYYMMDD
    */
  u8 expire_date[4];
  /*!
    the card key version
    */
  u32 key_version;
  /*!
    card balance, for shida
    */
  u32 banlance;
  /*!
    the card id, no format, just a string
    */
  u8 card_id[CAS_CARD_ID_MAX_LEN];
  /*!
    specify the card id length
    */
  u8 card_id_len;
  /*!
    the cas id
    */
   u8 cas_id[6];
  /*!
    cas version
    TRCAS: the string buffer shouldn't be less than 26 bytes
    */
  u8 cas_ver[CAS_VERSION_MAX_LEN + 1];
  /*!
    TRCAS: cas rate
    */
  u8 sc_cos_ver[CAS_CARD_VER_MAX_LEN + 1];
  /*!
    cas manufacturer name
    */
  u8 cas_manu_name[CAS_MAXLEN_MANUFACTURERNAME + 1];
  /*!
    specify the menu name length
    */
   u8 menu_name_len;
  /*!
    PIN code lock state
    */
  u8 pin_lock_state;
  /*!
    cas rating
    */
  u8 cas_rating;
  /*!
    card zone code
    */
  u8 card_zone_code;
  /*!
    card control code
    */
  u16 card_ctrl_code;
/*!
    partner stat
    */
  cas_paterner_sta_t paterner_stat;
  /*!
    work start time
    */
  cas_card_work_time_t work_time;
  /*!
      work end time
      */
  cas_card_work_time_t end_time;
  /*!
    get smart card eigen value
    */
  u8 card_eigen_valu[CAS_CARD_SN_MAX_LEN + 1];
  /*!
    specify the eigen value length
    */
  u16 eigen_value_len;
  /*!
    area code
    */
  u8 area_code[AREA_CODE_MAX_LEN + 1];
  /*!
    specify the area code length
    */
  u8 area_len;
  /*!
    area lock state
    */
  u8 area_start_flag;
  /*!
    received area stream time
    */
  u8 set_stream_time[CAS_DATE_MAX_LEN];
  /*!
    card type:0-mother card/1-child card
    */
  u8 card_type;
  /*!
    mother card ID
    */
  u8 mother_card_id[CAS_CARD_VER_MAX_LEN + 1];
  /*!
    specify the card id length
    */
  u8 mather_card_id_len;
  /*!
    received area stream time:u32
    */
  u32 set_stream_time_u32;
  /*!
    sumavision card info
    */
  sumavision_card_info_t sumavison;
  /*!
    conax card info
    */
  conax_card_info_t conax;
  /*!
    card state
    */
  u8 card_state;
  /*!
    card work level
    */
  u8 card_work_level;
  /*!
    describe
    */
  u8 desc[CAS_CARD_DESC_MAX_LEN + 1];
  /*!
    cas system time
    */
  u8 cas_sys_time[CAS_MAIL_DATE_MAX_LEN];
  /*!
    cas local id
    */
  u8 cas_loc[8];
  /*!
    cas group id 
    */
  u8 cas_group[16];
  /*!
    cas certify time,for yingji
    */
  u8 cas_certify_time[4];
  /*!
    cas certify interval,for yingji
    */
  u8 cas_certify_interval;
  /*!
    ABVCA:card update info
    */
  u8 update_info[5+1];
    /*!
    cryptoguard card info
    */
  crypg_card_info_t crypg_info;
    /*!
    kingvon card info
    */
  u32 creditCnt;
    /*!
    kingvon card info
    */
  u32 consumeCnt;
    /*!
    kingvon card info
    */
  u32 zoneID;
    /*!
    kingvon card info
    */
  u8 pairStatus;
    /*!
    kingvon card info
    */
  u8 smcProvider[MAX_SMCPROVIDER_NAME_LEN + 1];
}cas_card_info_t;

/*!
The user info of no-card ca
*/
typedef struct
{
  /*!
    ca version
    */
    u8 version;
   /*!
    SN
    */
    u8 serial_number[CAS_SN_LENGTH];
    /*!
    algorithm Family
    */
    u8 algorithm_family;
    /*!
    ca sid
    */
    u32 super_ca_sid;
    /*!
    pin availability flag
    */
    u8 pin_availability_flag;
    /*!
    default pin flag
    */
    u8 default_pin_flag;
    /*!
    certificate
    */
    u8 system_approval_certificate;
    /*!
    user state
    */
    u8 user_state;
    /*!
    gca name length
    */
    u8 gca_name_length;
    /*!
    gca name
    */
    u8 gca_name[CAS_SN_LENGTH + 4];
    /*!
    provider name length
    */
    u8 cas_provider_name_length;
    /*!
    provider name
    */
    u8 cas_provider_name[CAS_SN_LENGTH + 4];
}cas_user_privacy_info_t;

/*!
The operator info
*/
typedef struct
{
/*!
max num of operator
  */
  u32  operator_count;
/*!
  uniquely identifies a operator
  */
  u16  operator_id;
/*!
  operator  name
  */
  u8  operator_name[CAS_OPERATOR_NAME_MAX_LEN];
/*!
  specify the operator length
  */
    u8 operator_len;
/*!
  operator  status
  */
  u8  operator_status;
/*!
  operator  id, ASII
  */
  u8  operator_id_ascii[6];
/*!
  operator  phone
  */
u8 operator_phone[CAS_OPERATOR_PHONE_MAX_LEN];
/*!
  operator  phone
  */
u8 operator_address[CAS_OPERATOR_ADDR_MAX_LEN];
/*!
  ABVCA:one operator's balance
  */
s32 balance;
/*!
  operator aclist
  */
u32 p_ACArray[CAS_OPERATOR_ACLIST_MAX_LEN];

}cas_operator_info_t;

/*!
The operators info struct
*/
typedef struct
{
/*!
max num of operator
*/
  u8 max_num;
/*!
operator info
*/
  cas_operator_info_t  p_operator_info[CAS_MAX_OPERTOR_NUM];
}cas_operators_info_t;

/*!
op info keyset of no-card ca
*/
typedef struct
{
/*!
  count
  */
    u8 count;
/*!
  ids
  */
    u16 ids[CAS_MAX_OP_KEYSET_COUNT];
}cas_info_keyset_t;

/*!
op info of no-card ca
*/
typedef struct
{
/*!
  sn
  */
    u8 serial_number[CAS_SN_LENGTH];
/*!
  sub ca sid
  */
    u32 sub_ca_sid;
/*!
  operator name length
  */
    u8 operator_name_length;
/*!
  operator name
  */
    u8 operator_name[CAS_SN_LENGTH + 4];
/*!
  keyset
  */
    cas_info_keyset_t keyset;
}cas_info_op_t;

/*!
op info list of no-card ca
*/
typedef struct
{
/*!
  count
  */
    u8 count;
/*!
  operator info
  */
    cas_info_op_t ops[CAS_MAX_OP_KEYSET_COUNT + 2];
}cas_info_op_list_t;

/*!
product entitle info
*/
typedef struct
{
/*!
  product id
  */
  u32   product_id;
/*!
  remain time
  */
  u32   remain_time;
/*!
  0 can't record 1 can record
  */
  u8  can_tape;
/*!
  0: Valid entitle; 1: Input parameters error; 2: No entitle info;
  3: Valid entitle, but limited; 4: Entitle closed; 5: Entitle paused;
  AVBCA: 0 Already paid ; 1 or 2 Paid by time;3 Paid by prog;4 Have not paid
  */
  s16  states;
/*!
  reserved
  */
  u8   reserved;
/*!
      the etitle when this mail send, format: BCD code, 0xYYYYMMDDHHMMSS
      */
  u8  etitle_time[CAS_MAIL_DATE_MAX_LEN];
/*!
      the start when this mail send, format: BCD code, 0xYYYYMMDDHHMMSS
      TRCAS: start time of one product
      */
  u8  start_time[CAS_MAIL_DATE_MAX_LEN];
/*!
      the expired when this mail send, format: BCD code, 0xYYYYMMDDHHMMSS
      TRCAS: end time of one product
      */
  u8  expired_time[CAS_MAIL_DATE_MAX_LEN];
/*!
      product name
      */
  u8  product_name[CAS_PRODUCT_NAME_MAX_LEN];
/*!
      the etitle when this mail send, format: u32
      */
  u32  etitle_time_u32;
/*!
      the start when this mail send, format: u32
      TRCAS: start time of one product
      */
  u32  start_time_u32;
/*!
      the expired when this mail send, format: u32
      TRCAS: end time of one product
      */
  u32  expired_time_u32;
/*!
  specify the product name length
  */
    u8 pro_name_len;
}product_entitle_info_t;

/*!
product entitles info
*/
typedef struct
{
/*!
  operator id
  */
  u16   operator_id;
/*!
  max num of entitle
  */
u16 max_num;
/*!
  max num of entitle
  */
u16 start_pos;
/*!
  max num of entitle
  */
u16 end_pos;
/*!
product entitle info
*/
 u16 read_num;
/*!
read product entitle: 0 for all entitles, 1 for simple entitles
*/
 u8 read_type;
/*!
 product entitle info
 */
product_entitle_info_t  p_entitle_info[CAS_MAX_PRODUCT_ENTITLE_NUM];
/*!
ABVCA:get only one row info from all the entile info
*/
 u16 entitle_no;
/*!
the free entitle num
*/
 u16 free_entitle_num;
/*!
the ppc entitle num
*/
 u16 ppc_entitle_num;
/*!
the ppv entitle num
*/
 u16 ppv_entitle_num;
}product_entitles_info_t;

/*!
product detitles info
*/
typedef struct
{
/*!
  operator id
  */
  u16   operator_id;
/*!
  0: not read; 1: read
  */
  u8   read_flag;
/*!
    read product entitle: 0 for all entitles, 1 for simple entitles
  */
  u8 read_type;
/*!
  max num of entitle
  */
u16 max_num;
/*!
  the detitle info to be deleted, 0 for deleting all
  */
u16 dwDetitleChkNum;
/*!
 product entitle info
 */
  u32  p_detitle_info[CAS_MAX_PRODUCT_ENTITLE_NUM];
}product_detitles_info_t;

/*!
  Card and STB paired list
  */
typedef struct
{
/*!
  paired status
  */
  u16   paired_status;
/*!
  the num of STB paired with card
  */
  u8   paired_num;
/*!
  paired STB list
  */
  u8   STB_list[CAS_MAX_STB_LIST_LEN];
}card_stb_paired_list_t;

/*!
  ipp price info
  */
typedef struct
{
/*!
  ipp price type
  */
  u8   price_type;
/*!
  ipp price
  */
  u16   price;
}ipp_price_info_t;

/*!
  ABVCA: buy ipp from epg
  */
typedef  struct
{
    /*!
        Orignal_Network_ID
    */
    u16 Orignal_Network_ID;
    /*!
        TS_ID
    */
    u16 TS_ID;
    /*!
        Service_ID
    */
    u16 Service_ID;
    /*!
        Event_ID
    */
    u16 Event_ID;
    /*!
        Pay_Type
    */
    u8 Pay_Type;
} ipp_buy_from_epg_t;

/*!
  ipp buy info from stb
  */
typedef struct
{
/*!
     specify the index of record
  */
  u16 index;
/*!
      Purchase or cancle flag, 0:cancle; 1:purchase.
   */
  u8  state_flag;
/*!
   Specify channel id.
   TRCAS: the channel index
   ABVCAS:0:buy current product  1:buy next product
  */
  u16 channel_id;
/*!
   for product purchase
  */
  u16 purchase_num;
/*!
   charge value of one unit time
  */
  u32 ipp_charge;
/*!
    for product purchase
  */
  u32 ipp_unit_time;
/*!
   for product purchase, for exch_runing_num too
  */
  u32 ipp_running_num;
/*!
  ipp buy message type
  TRCAS: IPP type, 0 IPPV, 1 IPPT
  */
  u8   message_type;
/*!
  operator id
  */
  u16  tvs_id;
/*!
  burse id
  */
  u8  burse_id;
/*!
  product id, act as topreal IPP Type, 0:ippv, 1:ippt
*/
  u32 product_id;
/*!
  The program ecm pid
  */
  u16 ecm_pid;
/*!
  The ipp_price_info_t num
  TRCAS: purchase num
  */
  u8  price_num;
/*!
  The ipp price info
  TRCAS: charge value of one unit time(price), just use ipp_price[0].price
  */
  ipp_price_info_t ipp_price[CAS_MAX_NUM_PRICE];
/*!
  The ipp start time. for ipp notify
  */
  u8 start_time[CAS_MAIL_DATE_MAX_LEN];
/*!
  The ipp end time
  */
  u8  expired_time[CAS_MAIL_DATE_MAX_LEN];

/*!
   pin info when purchase product, for product purchase
  */
  u8  pin[CAS_MAX_PIN_LEN + 1];
/*!
  specify the pin length
  */
    u8 pin_len;
/*!
  note form cas
  */
    u8  note[CAS_NOTE_TEXT_MAX_LEN];
/*!
  specify the note length
  */
    u8 note_len;
/*!
  ABVCA:specify the pin reset num
 */
    u8 pin_spare_num;
/*!
  ABVCA:buy ippv from epg
  */
  ipp_buy_from_epg_t ipp_from_epg;
}ipp_buy_info_t;

/*!
  ipp consume record info
*/
typedef struct
{
  /*!
    The ipp echange time.
    */
  u8 start_time[CAS_MAIL_DATE_MAX_LEN];
  /*!
    Charging or consume flag, 0:consume; 1:charge.
    */
  u8  state_flag;
  /*!
    length
    */
  u8  content_len;
  /*!
    index
    */
  u8  index;
  /*!
    reserved
    */
  u8 reserved;
  /*!
    content
    */
  u8  content[CAS_MAX_IPP_CONTENT_NUM];
  /*!
    ipp running num
    */
  u32 runningNum;
  /*!
    ipp echange value
    */
  u32 value;
  /*!
  entitle tokens
  */
  s32 tokens;
} ipp_record_info_t;

/*!
  ipp total consume record info
*/
typedef struct
{
  /*!
    total num.
    */
  u32 max_num;
  /*!
    ABVCA: which program_provider
    */
  u8 ppid_no;
  /*!
    see ipp_record_info_t.
    */
  ipp_record_info_t record_info[CAS_MAX_IPP_RECORD_NUM];
} ipp_total_record_info_t;


/*!
ipp info  form ca
*/
typedef struct
{
/*!
     specify the index of Product
  */
  u16 index;
/*!
  operator id
  */
  u16   operator_id;
/*!
  product id
  */
  u32   product_id;
/*!
  product status: 0:book 1:viewed
  */
  u8  book_state_flag;
/*!
   0:program can't be recorded    1:program can be recorded
  */
  u8  is_can_tape;
/*!
  ipp price
  */
  u16   price;
/*!
  ipp start time
  */
  u8  start_time[CAS_MAIL_DATE_MAX_LEN];
/*!
  ipp expire time
  TRCAS: the end time of one product
  */
  u8  expired_time[CAS_MAIL_DATE_MAX_LEN];
/*!
  card burse id
  */
  u8  burse_id;
/*!
  ipp type // 0:ippv; 1:ippt
  ABVCA: 2:both ippv & ippt
  */
  u8  type;
  /*!
    pg is authed
    */
  u8 authed;
  /*!
    reserved
    */
  u8 reserved;
/*!
     the remain working day of one product.  Gavin need confirm it with Topreal.
  */
  u32 remain_time;
/*!
      for IPP product
      TRCAS:
  */
   u32 running_num;
/*!
    for Sumavision CA: has booked price
  */
   u32 booked_price;
/*!
    for Sumavision CA: booked price type 0:TppTap;1:TppNoTap;2:CppTap;3:CppNoTap;
  */
   u8 booked_price_type;
/*!
    for Sumavision CA: has booked program charge interval
  */
   u8 booked_by_interval;
/*!
    for Sumavision CA: program charge unit
    0-minute, 1-hour, 2-day, 3-month, 4-year
  */
   u8 booked_by_unit;
/*!
    for Sumavision CA: current program charge interval
  */
   u8 reserved1;
/*!
    for Sumavision CA: product name
  */
   u8 product_name[CAS_MAX_IPP_MAX_NAME_LEN];
/*!
    for Sumavision CA
  */
   u8 m_szOtherInfo[44];
/*!
    fro Sumavision CA
  */
   u8 service_name[CAS_MAX_IPP_MAX_NAME_LEN];
/*!
    for Sumavision CA
  */
   u16 service_id;
/*!
    for Sumavision CA: current price
  */
   u16 current_price;
/*!
    for Sumavision CA: current charge interval
  */
   u8 current_by_interval;
/*!
  for Sumavision CA: IPPT program num
*/
  u16 ippt_period;
  /*!
    for desai CA: event id
  */
   u8 event_id[4];
  /*!
    ABVCA: price of pay per minute
  */
  u32 time_price;
  /*!
    ABVCA: price of pay per view
  */
  u32 view_price;
}ipp_info_t;

/*!
ipp info  form ca
*/
typedef struct
{
/*!
max num of ippv  */
  u16   max_num;
/*!
  operator id
  */
  u16   operator_id;
/*!
  product id
  */
   ipp_info_t p_ipp_info[CAS_MAX_IPP_NUM];
}ipps_info_t;

/*!
  The event callback structure for CA system module
  */
typedef struct
{
/*!
  0:means show on top of the stb 1:means show on bottom of the stb
  */
   u8 location;
/*!
  roll mode  0:roll bases time 1:bases times
  */
  u32  roll_mode;
/*!
  roll_mode when bases on time: value 0 means osd show always when roll_mode bases on times:value 0 means osd show one time
  */
  u8    roll_value;

}osd_display_mode_t;

/*!
pop msg channel limit info
*/
typedef struct
{
/*!
  display flag: 0:show; 1:no show  ;
  */
    u8 flag;
/*!
  the condition count if 0 no show!
  */
    u16 count;
/*!
  the condition list (channel service_id)
  */
    u16 condition_list[CAS_MAX_CONDITION_COUNT];
}msg_limit_list_t;

/*!
crypg message info
*/
typedef struct
{
  /*!
  year/month/day  ;
  */
  u16 MJD;
  /*!
  service id  ;
  */
  u16 service_id;
  /*!
  ver  ;
  */
  u16 version;
  /*!
  force and normal  ;
  */
  u8  type;
  /*!
  hour  ;
  */
  u8  hour;
  /*!
  min  ;
  */
  u8  min;
  /*!
  second  ;
  */
  u8  second;
}crypg_message_desc_t;

/*!
msg info
*/
typedef struct
{
/*!
  msg type: 0:osd;   1:notify  ;
  */
  u8  type;
/*!
  osd show position: 0:on the top of stb,1:on the bottom of stb
  */
  osd_display_mode_t  osd_display;
/*!
  text content in stb
  */
  u8   data[CAS_MAX_MSG_DATA_LEN];
/*!
  specify the return data length
  */
  u16 data_len;
/*!
  titile for notify
  */
  u8   title[CAS_MAX_MSG_TITLE_LEN];
/*!
  specify the return title length
  */
  u8 title_len;
/*!
  message index
  */
  u16 index;
/*!
  priority
  */
  u8 priority;
/*!
  message display type, defined by CAS_MSG_CLASS
  */
  u8 class;
/*!
  notification display interval
  */
  u16 period;
/*!
  message create time; no use for osd msg
  */
  u8 create_time[CAS_MAIL_DATE_MAX_LEN];
/*!
  osd display postion
  ABVCA:  000: full screen; 001: scroll   
*/
    u8 display_pos;
/*!
      osd display front
      ABVCA:  0:do not display background;1:display background
     */
    u8 display_front;
/*!
      osd display front size
     */
    u8 display_front_size;
/*!
      osd display background colour
     */
    u8 display_back_color[4];
/*!
      osd display  colour
     */
    u8 display_color[4];
/*!
      font color: RGB
     */
    u32 font_color;
/*!
      background color: RGB
     */
    u32 background_color;
/*!
      back ground area
     */
    u8 background_area;
/*!
      osd display  content
     */
    u16 content_len;
/*!
  msg channel limit info
*/
  msg_limit_list_t condition;
/*!
  display time
*/
  u8 display_time;
/*!
  ABVCA: transparency of osd layer
*/
  u8 transparency;
/*!
  desai5.6: colour type
*/
  u8 color_type;
/*!
  desai5.6 Screen Ratio
*/
  u8 screen_ratio;
/*!
  CRYPG: message desc
*/
  crypg_message_desc_t msg_detail;
/*!
      topreal enhanced msg postion x
*/
  u8 postion_x;
/*!
      topreal enhanced msg postion y
*/
  u8 postion_y;
/*!
      topreal enhanced msg postion w
*/
  u8 postion_w;
/*!
      topreal enhanced msg postion h
*/
  u8 postion_h;
/*!
      show  status
*/
  u8 display_status;
}msg_info_t;

/*!
  finger display property
  */
typedef struct{
/*!
  percent of the start point to the screen top, value 0-100. -1 means display random
  */
  s8 m_byLocationFromTop;
/*!
  percent of the start point to the screen left, value 0-100. -1 means display random
  */
  s8 m_byLocationFromLeft;
/*!
  percent of the display range to screen range, value 0-100
  */
  u8 m_byBannerWidth;
/*!
  font type
  */
  u8 m_byFontType;
/*!
  background transparency and RGB
  */
  u32 m_dwBackgroundARGB;
/*!
  font transparency and RGB
  */
  u32 m_dwFontARGB;
/*!
  m_bySize
  */
  u8 m_bySize;
/*!
  ABVCA:transparency of OSD layer
  */
  u32 transparency;
}finger_property_t;

/*!
finger display postion
*/
typedef struct
{
/*!
  ONLY1 CA:X postion
*/
  u32 m_dwX; 
/*!
  ONLY1 CA:Y postion
*/
  u32 m_dwY;  
}finger_coodinate_t;

/*!
  finger msg info
  */
typedef struct
{
/*!
  finger content:card sn
  */
  u8   data[CAS_NOTE_TEXT_MAX_LEN];
/*!
  specify the finger version
  */
  u32 finger_version;
/*!
  specify the return data length
  ABVCA:display the card_sn as default if date_len =0 or =1
  */
  u16 data_len;
/*!
  finger data showing time
  */
  u16 during;
/*!
  reserved
  */
  u8 reserved;
/*!
  finger msg property
  */
  finger_property_t finger_property;
/*!
  finger data showing postion
  */
  u8    show_postion;
/*!
  finger data showing postion
  */
  u16    show_postion_x;
/*!
  finger data showing postion
  */
  u16    show_postion_y;
/*!
  finger data showing color
  */
  u8    show_color[4];
/*!
  finger data showing color
  */
  u8    show_color_back[4];
/*!
  finger data showing front
  */
  u8    show_front;
/*!
  finger data showing front size
  */
  u8    show_front_size;
/*!
  YongXxinShiBo:show type, visible or not visible
  */
  u8    bIsVisible;
/*!
  YongXxinShiBo:finger type, Card No. OR STB SN
  */
  u8    byType;
  /*!
  ABVCA: 1:common emm fp; 2:channel emm fp should be cleard when switch channel
  */
  u8 fp_type;
/*!
  ABVCA:display times
  */
  u16 repetition;
/*!
  ABVCA:display interval
  */
  u16 period;
/*!
  ABVCA:sparkling; 0:no sparking;1:
  */
  u8 font_sparkling;
/*!
  ABVCA:emm send date
  */
  u8 current_time[5];
/*!
  ABVCA:emm msg_id
  */
  u32 msg_id;
/*!
  ABVCA:fp start date and time
  */
  u8 start_time[7];
/*!
  ABVCA:width
  */
  u8 width;
/*!
  ABVCA:height
  */
  u8 height;
/*!
  ABVCA: 1:display backgroud; 0: not display background
  */
  u8 disp_bg;
/*!
  ABVCA: 1:display UA; 0:not display UA
  */
  u8 disp_ua;
/*!
  ABVCA: 1:display stb number; 0:not display stb number
  */
  u8 disp_stbnum;
/*!
  ABVCA: 1:display message; 0:not display message
  */
  u8 disp_msg;
/*!
  ABVCA: transparency
  */
  u8 transparency;
/*!
  ABVCA:font size
  */
  u8 font_size;
/*!
ONLY1CA:position num
*/
  u8 position_num;
/*!
ONLY1CA:position
*/
  finger_coodinate_t position[CAS_MAXNUM_FINGER_POSITION];
/*!
CRYPTOGUARD:Card num flag:
*/
 u8 card_num_flag; 
/*!
CRYPTOGUARD:Card number:
*/
 u32 card_num;  
/*!
CRYPTOGUARD:IRD num flag:
*/
 u8 ird_num_flag;
/*!
CRYPTOGUARD:IRD number:
*/
 u32 ird_num;
/*!
CRYPTOGUARD:Service ID:
*/
 u16 service_id;
/*!
CRYPTOGUARD:radom:
*/
 u8 radom;
/*!
CRYPTOGUARD:repeat_num:
*/
 u16 repeat_num;
/*!
CDCAS:for india 
*/
 u8 matrix_inner_pos[16];
/*!
The program ecm pid
*/
 u16 ecm_pid;
 /*!
  topreal: on time
  */
  u32 on_time;
/*!
  topreal: off time
  */
  u32 off_time;
}finger_msg_t;


/*!
son-monther card info
*/
typedef struct
{
/*!
  operator id
  */
  u16   operator_id;
/*!
  0:monther card  1:son card 2:normal card
  */
  u8  is_child;
/*!
  0:son card can't be feed 1:son card can be feed
  */
  u8  is_can_feed;
/*!
  time between two feed times
  */
  u16  delay_time;
/*!
  parent card sn
  */
  u8  parent_card_sn[CAS_CARD_SN_MAX_LEN + 4];
/*!
  specify the parent card sn length
  */
    u8 card_sn_len;
/*!
  parent card stb sn
  */
  u8  parent_card_stb_sn[16];
/*!
  specify the parent card stb length
  */
    u8 stb_sn_len;
/*!
  child card totally authed time
  */
  u8    child_card_authed_time[4];
/*!
  child card remain time
  */
  u8    child_card_remain_time[4];
/*!
  last feed time
  */
  u8    last_feed_time[CAS_MAIL_DATE_MAX_LEN];
/*!
  before feed timing notify days
  */
  u16  notify_time;
/*!
   The feed data buffer, it size must greater than 128 bytes. for feed data read and feed.
  */
  u8 feed_data[256];
/*!
   The actual feed data length in feed data buffer. for feed data read and feed.
  */
  u8 length;
}chlid_card_status_info;

/*!
  burse info
  */
typedef struct
{
/*!
  credit of use
  TRCAS: credit value
  */
  u32   credit_limit;
/*!
      cash of use
      TRCAS: cash value
   */
    s32 cash_value;
/*!
     TRCAS: cash value
   */
    u32 cash_total;
/*!
  burse used
  */
  u32   balance;
/*!
  record index of purse
   TRCAS: Last charge record index
  */
    u16 record_index;
/*!
  burse value, ASCII,
  */
  u8 burse_value[CAS_MAX_BURSE_VALUE_NUM];

}burse_info_t;

/*!
  burses info
  */
typedef struct
{
/*!
  max num
  */
  u16   max_num;
/*!
  operator id
  */
  u16   operator_id;
/*!
  * The purse index.
  */
   u8 index;
/*!
  burse value, ASCII,
  */
  u8 burse_value[CAS_MAX_BURSE_VALUE_NUM];
/*!
  burse info
  */
  burse_info_t p_burse_info[CAS_MAX_BURSE_NUM];
}burses_info_t;

/*!
  The pin error information
  */
typedef struct
{
/*!
    TRCAS: the rating level of smart card. from 0 to 10
    ABVCAS: level 6 to 21 means the adult level can be set;level <6 or >21 is invalid;
    */
  u8 rate;
/*!
    TRCAS: Pin code for maturity rating set, length is 6
    */
  u8 pin[CAS_MAX_PIN_LEN + 1];
/*!
  specify the pin length
  */
    u8 pin_len;
/*!
  specify spare num
  */
    u8 pin_spare_num;

}cas_rating_set_t;
/*!
   The channel change structure for CA system module
  */
typedef struct
{
   /*!
      The id of the channel to be changed to.
     */
   u16 channel_id;
   /*!
      org_network_id.
     */
   u16 org_network_id;
   /*!
      ts_id.
     */
   u16 ts_id;   
   /*!
      The relationship state between current service and the service to be changed to.
      The state is 0 if they are in different transponders, and it is 1 if they are in same
      transponder.
     */
   u8 state;
  /*!
  PMT pid
    */
  u16 pmt_pid;
  /*!
  video pid
    */
  u16 v_pid;
  /*!
  audio pid
    */
  u16 a_pid;
  /*!
    Service id
    */
  u16 service_id;
}cas_channel_state_set;

/*!
  structure for force channel component info
  */
typedef struct
{
/*!
  component PID
  */
    u16  comp_PID;
/*!
  corresponding ECM PID
  */
    u16  ECM_PID;
/*!
  component type
  */
    u8  comp_type;
/*!
  reserved
  */
    u8  reserved[3];
}force_channel_comp_t;

/*!
  structure for force channel infomation
  */
typedef struct
{
/*!
  lock flag
  */
    u8  lock_flag;
/*!
   network id
  */
    u16 netwrok_id;
/*!
   transport id
  */
    u16 ts_id;
/*!
   service id
  */
    u16 serv_id;
/*!
  note form cas
  */
    u8  note[CAS_NOTE_TEXT_MAX_LEN];
/*!
  specify the note length
  */
    u8 note_len;
/*!
  specify the urgency broadcast duration
  */
    u32 duration_time;
/*!
  the expired time
  */
    u8 expired_time[CAS_FORCE_CHANNEL_EXPIRED_TIME];
/*!
  video id
  */
    u16 video_id;
/*!
  audio id
  */
    u16 audio_id;
/*!
  frequency
  */
    u16 frequency;
/*!
  symbol rate
  */
    u32 symbol_rate;
/*!
  pcr pid
  */
    u16 pcr_pid;
/*!
  Modulation
  */
    u8 modulation;
/*!
  Component Num
  */
    u8 component_num;
/*!
  Specific Component Info
  */
    force_channel_comp_t CompArr[CAS_MAX_FORCE_CHANNEL_COMPONENT];
/*!
  outer FEC
  */
    u8 m_fec_outer;
/*!
  inner FEC
  */
    u8 m_fec_inner;
} cas_force_channel_t;

/*!
  The card lock status
  */
typedef enum
{
/*!
    The card service has been locked
    */
  CAS_CARD_SERVICE_LOCKED = 1,
/*!
    The card service has been unlocked
    */
  CAS_CARD_SERVICE_UNLOCKED,
}cas_card_lock_sta_t;

/*!
  The card status
  */
typedef enum
{
/*!
    The card has ready
    */
  CAS_CARD_IS_READY = 0,
/*!
    The card has not ready
    */
  CAS_CARD_NOT_READY = 2
}cas_card_sta_t;

/*!
  The program information
  */
typedef struct
{
/*!
    The program number
    */
  u16 prog_num;
/*!
    The program type, if is a PPV
    */
  u8 type;
/*!
    If the program has been authorized
    */
  u8 authed;
/*!
    The program price
    */
  u32 price;
}cas_prog_info_t;

/*!
  The event callback structure for CA system module
  */
typedef enum
{
/*!
    The PPV has been confirmed to teleview by user
    */
  CAS_PPV_TAG_TELEVIEW_CONFIRMED = 1,
/*!
    The PPV has not been confirmed to teleview by user
    */
  CAS_PPV_TAG_TELEVIEW_UNCONFIRMED,
}cas_ppv_tag_t;

/*!
  The ppv tag information
  */
typedef struct
{
/*!
    The program number
    */
  u16 prog_num;
/*!
    The ppv tag value, see cas_ppv_tag_t
    */
  u8 tag;
}cas_ppv_tag_info_t;

/*!
  The ppv tag information
  */
typedef struct
{
/*!
    The cas module id, see cas_module_id_t
    */
  u16 cam_id[CAS_MAX_SLOT_NUM];
/*!
    The index of card slot on which the hotplug happened, 1: actived, 0:de-actived
    */
  u8 slot_actived[CAS_MAX_SLOT_NUM];
}cas_hotplug_info_t;

/*!
  The cas program certificate info
  */
typedef struct
{
/*!
    Service id
    */
    u16    service_id;
/*!
    Program_certificate
    */
    u8     program_certificate;
/*!
   System_approval_certificate
    */
    u8     system_approval_certificate;
}cas_program_certificate_info_t;

/*!
  ABVCA: The OTA info ,please keep same with update_t
  */
typedef struct 
{
    /*!
        oui
    */
    u32 oui;
    /*!
        symbol
    */
    u16 symbol;
    /*!
        freq
    */
    u32 freq;
    /*!
        qam_mode
    */
    u8  qam_mode;
    /*!
        data_pid
    */
    u16  data_pid;
    /*!
        ota_type
    */
   u8  ota_type;
    /*!
        polarization
    */
   u8  polarization;
    /*!
        hwVersion
    */
    u32 hwVersion;
    /*!
        swVersion
    */
    u32 swVersion;
    /*!
        Serial_number_start
    */
    u8  Serial_number_start[16];
    /*!
        Serial_number_end
    */
    u8  Serial_number_end[16];
}cas_ota_info_t;

/*!
  ABVCA: The ECM FingerPrint Paraments
  */
typedef struct 
{
   /*!
    Ecm FingerPrint X-coordinate
  */
    u16 ECM_X_Axis;
   /*!
    Ecm FingerPrint Y-coordinate
  */
    u16 ECM_Y_Axis;
   /*!
    0 ore 1
  */
    u8 type; 
   /*!
    service id
  */ 
   u32 service_id;
   /*!
     the card SN, in format of BCD code, it is a string in topreal cas
     TRCAS: The string buffer size should be larger than 24 bytes
     */
   u8 sn[CAS_CARD_SN_MAX_LEN + 1];
   /*!
     specify the sn length
     */
   u8 sn_len;
   /*!
     the STB SN
     */
   u8 stb_sn[CAS_STB_SN_MAX_LEN];
   /*!
     specify the stb sn length
     */
   u8 stb_sn_len;
}cas_ecm_position_t;

/*!
  ABVCA: The msg from other STB
  this struct will also be used for STB extention info
  */
typedef struct
{
    /*!
        data
  */
    u8 data[1024];
    /*!
        data_len
  */
    u16 data_len;
}cas_other_stb_msg_t;

/*!
  cas_continues_watch_limit_t
  */
typedef struct
{
/*!
limit type
*/
  u8   type;
/*!
work time 
*/
  u16  work_time;
/*!
stop time 
*/
  u16  stop_time;
}cas_continues_watch_limit_t;


/*!
  The event callback structure for CA system module
  */
typedef enum
{
/*!
    Get card hotplug information, parametern is cas_hotplug_info_t
    */
  CAS_IOCMD_HOTPLUG_INFO_GET = 1,
/*!
    Get mail max
    */
  CAS_IOCMD_GET_MAIL_MAX,
/*!
    Get mail overlay policy
    */
  CAS_IOCMD_GET_MAIL_POLICY,
/*!
    Get current mail
    */
  CAS_IOCMD_GET_CURRENT_MAIL,
/*!
    Get serial data
    */
  CAS_IOCMD_GET_SERIAL_DATA,
/*!
    Get CA_SYSTEM_ID
    */
  CAS_IOCMD_CA_SYS_ID_GET,
  /*!
    Get Operator Info
    */
  CAS_IOCMD_OPERATOR_INFO_GET,
  /*!
    CAS_IOCMD_ZONE_CHECK
    */
  CAS_IOCMD_ZONE_CHECK,
  /*!
    set zone check code
    */
  CAS_IOCMD_SET_ZONE_CODE,
  /*!
      Get card balance, paramerter is (u32 *)
      */
  CAS_IOCMD_CARD_BALANCE_GET,
  /*!
      Get finger msg
      */
  CAS_IOCMD_FINGER_MSG_GET,
  /*!
      ABVCA: Get ECM finger msg
      */
  CAS_IOCMD_ECM_FINGER_MSG_GET,
  /*!
      CDCAS CA: watch limit msg
  */
  CAS_IOCMD_CONTINUE_WATCH_LIMIT_GET,
  /*!
      Get ipp buy info
      */
  CAS_IOCMD_IPP_BUY_INFO_GET,
  /*!
      stop cas ecm process
      */
  CAS_IOCMD_STOP,
  /*!
      stop all ca process(ecm and emm)
      */
  CAS_IOCMD_STOP_CAS,
  /*!
      set ecm emm
      */
  CAS_IOCMD_SET_ECMEMM,
  /*!
      vedio pid
      */
  CAS_IOCMD_VIDEO_PID_SET,
  /*!
      audio pid
      */
  CAS_IOCMD_AUDIO_PID_SET,
/*!
    Get card over draft, paramerter is (u32 *)
    */
  CAS_IOCMD_CARD_OVERDRAFT_GET,
  /*!
    Get osd message
    */
  CAS_IOCMD_OSD_MSG_GET,
  /*!
    OSD message show end
    */
  CAS_IOCMD_OSD_MSG_SHOW_END,
      /*!
    rate get
    */
  CAS_IOCMD_RATING_GET,
      /*!
    rate set
    */
  CAS_IOCMD_RATING_SET,
    /*!
    set work time
    */
  CAS_IOCMD_WORK_TIME_SET,
    /*!
    get work time
    */
  CAS_IOCMD_WORK_TIME_GET,
/*!
    set pin
    */
   CAS_IOCMD_PIN_SET,
 /*!
    Get pin state
    */
   CAS_IOCMD_PIN_STATE_GET,
 /*!
    enable pin
    */
   CAS_IOCMD_PIN_ENABLE,
  /*!
    verify pin
    */
   CAS_IOCMD_PIN_VERIFY,
     /*!
   child status
    */
   CAS_IOCMD_MON_CHILD_STATUS_GET,
     /*!
   child feed
    */
   CAS_IOCMD_MON_CHILD_STATUS_SET,
    /*!
     To identify card is Mother/child card
     */
   CAS_IOCMD_MON_CHILD_IDENTIFY,
   /*!
     Read feed data from mother card
     */
   CAS_IOCMD_READ_FEED_DATA,
   /*!
   get entitle info
    */
   CAS_IOCMD_ENTITLE_INFO_GET,
   /*!
   ABVCA: get only one piece entitle info
    */
   CAS_IOCMD_ONE_ENTITLE_INFO_GET,
      /*!
    get ippv info
    */
   CAS_IOCMD_IPPV_INFO_GET,
   /*!
    get ippv info
    TRCAS: IPP exchange record get
   */
   CAS_IOCMD_IPP_EXCH_RECORD_GET,
   /*!
    ABVCA: get one row record
   */
   CAS_IOCMD_ONE_EXCH_RECORD_GET,
         /*!
    get burse info
    */
   CAS_IOCMD_BURSE_INFO_GET,
           /*!
    get entitle info
    */
   CAS_IOCMD_DETITLE_INFO_GET,
/*!
    del entitle info
    */
   CAS_IOCMD_DETITLE_INFO_DEL,
/*!
    Get paterner status, parameter please see cas_paterner_sta_t
    */
  CAS_IOCMD_PATERNER_STA_GET,
/*!
    Get pin error count, the parameter for this command please see cas_pin_err_t
    */
  CAS_IOCMD_PIN_ERR_CNT_GET,
/*!
    Get message
    */
  CAS_IOCMD_MSG_GET,
/*!
    Get OSD message
    */
  CAS_IOCMD_OSD_GET,
/*!
    Get mail header, the parameter please see cas_mail_header_t
    */
  CAS_IOCMD_MAIL_HEADER_GET,
/*!
    Get mail body, the parameter please see cas_mail_body_t
    */
  CAS_IOCMD_MAIL_BODY_GET,
 /*!
    Mail has readed, change mail status to readed
    */
  CAS_IOCMD_MAIL_CHANGE_STATUS,
 /*!
    Get Mail Space Info
    */
  CAS_IOCMD_MAIL_SPACE_STATUS,
  /*!
    Get announce header, the parameter please see cas_mail_header_t
  */
  CAS_IOCMD_ANNOUNCE_HEADER_GET,
  /*!
    Get announce body, the parameter please see cas_mail_body_t
  */
  CAS_IOCMD_ANNOUNCE_BODY_GET,
  /*!
    Delete mail msg by index
  */
  CAS_IOCMD_MAIL_DELETE_BY_INDEX,
  /*!
    Delete all mail
  */
  CAS_IOCMD_MAIL_DELETE_ALL,
  /*!
    FACTORY SET
  */
  CAS_IOCMD_FACTORY_SET,
  /*!
    Delete ANNOUNCE msg by index
  */
  CAS_IOCMD_ANNOUNCE_DELETE_BY_INDEX,
  /*!
    Delete all ANNOUNCE
  */
  CAS_IOCMD_ANNOUNCE_DELETE_ALL,
/*!
    Get PPV charge value in 0.01 Fen/Cent, the parameter is (u32 *)
    */
  CAS_IOCMD_PPV_CHARGE_GET,
/*!
    Get STB serial number, the parameter please see cas_stb_serial_t
    */
  CAS_IOCMD_STB_SERIAL_GET,
/*!
    Get urgent service id, the parameter should be (u16 *)
    */
  CAS_IOCMD_SERVICE_ID_GET,
/*!
    Get card information, parameter please see cas_card_info_t,
    no-card ca see cas_user_privacy_info_t
    */
  CAS_IOCMD_CARD_INFO_GET,
/*!
    Get card lock status, the parameter please see cas_card_sta_t
    */
  CAS_IOCMD_CARD_LOCK_STA_GET,
/*!
    Get group id in 4 bytes
    */
  CAS_IOCMD_GROUP_ID_GET,
/*!
    Get current program number
    */
  CAS_IOCMD_CUR_PRG_NUM_GET,
/*!
    Get current program type, PPV or not, the parameter please see cas_prog_info_t
    */
  CAS_IOCMD_CUR_PROG_INFO_GET,
/*!
    Get PPV teleview tag, parameter please see cas_ppv_tag_t
    */
  CAS_IOCMD_PPV_TAG_GET,
/*!
    Get loader information
    */
  CAS_IOCMD_LOADER_INFO_GET,
/*!
    Get CAS module software version number
    */
  CAS_IOCMD_CAS_SOFT_VER_GET,
/*!
    Get zone code
    */
  CAS_IOCMD_ZONE_CODE_GET,
/*!
    Set zone code
    */
  CAS_IOCMD_ZONE_CODE_SET,
/*!
    Get eigen value, the parameter please see cas_eigen_value_t
    */
  CAS_IOCMD_EIGEN_VALUE_GET,
/*!
    Set PPV teleview tag, called by up-layer if user wanna watch the current PPV,
    parameter is 1 means wanna watch, and vice versa
    */
  CAS_IOCMD_PPV_TAG_SET,
/*!
    Clean last program CA information, called by up-layer if user wanna switch change
    */
  CAS_IOCMD_LAST_PRG_INFO_CLR,
/*!
    Notify CAS that PSI table changed
  */
  CAS_IOCMD_PSI_TABLE_SET,
/*!
    Notify CAS that channel is changed
  */
  CAS_IOCMD_CHANNEL_STATE_SET,
/*!
  Get EMM or ECM control information
  */
  CAS_IOCMD_FORCE_CHANNEL_INFO_GET,
/*!
  Get authen remain days
  */
  CAS_IOCMD_AUTHEN_DAY_GET,
/*!
  Get IPP notify information
  */
  CAS_IOCMD_IPP_NOTIFY_INFO_GET,
/*!
  Get short message infomation
  */
  CAS_IOCMD_SHORT_MSG_GET,
/*!
  Purchase IPPT/IPPV product
  */
  CAS_IOCMD_PURCHASE_SET,
/*!
  parent control unlock
  */
  CAS_IOCMD_PARENT_UNLOCK_SET,
/*!
    Get purchased product type, IPPV/IPPT
    */
  CAS_IOCMD_PRODUCT_TYPE_GET,
/*!
    Get important mail to pop display
    */
  CAS_IOCMD_GET_IMPORTANT_MAIL,
  /*!
     get smart card status
    */
  CAS_IOCMD_SMC_STATUS_GET,
  /*!
     open CA debug msg for Sumavision
    */
  CAS_IOCMD_SMC_OPEN_DEBUG,
/*!
    Get control code
    */
  CAS_IOCMD_CTRL_CODE_GET,
/*!
    Request Certificate Confirm Service
    */
  CAS_IOCMD_GET_CERTIFICATE_SERVICE,
/*!
    Set Overdue Info Service
    */
  CAS_IOCMD_SET_OVERDUE_INFO_SERVICE,
/*!
    Get Overdue Info Service
    */
  CAS_IOCMD_GET_OVERDUE_INFO_SERVICE,
  /*!
    set language
    */
  CAS_IOCMD_LANGUAGE_SET,
   /*!
    get msg show type
    */
  CAS_IOCMD_SHOW_TYPE_GET,
      /*!
      set ipp buy
      */
  CAS_IOCMD_IPP_BUY_SET,
  /*!
    save file data before entering standby
    */
  CAS_IOCMD_SAVE_FILE_DATA,
  /*!
    Get Film Rating Info
    */
  CAS_IOCMD_GET_FILM_RATING,
  /*!
    Mask operation to Flash
    */
  CAS_IOCMD_REQUEST_MASK_BUFFER,
  /*!
    Recover operation to Flash
    */
  CAS_IOCMD_REQUEST_UPDATE_BUFFER,
  /*!
    Get operator's or user's private information
    */
  CAS_IOCMD_ACLIST_GET,
  /*!
    ABVCA:Watch Mr program set
    */
  CAS_IOCMD_WATCH_MR_PROGRAM,
  /*!
    ABVCA:Watch Mr program set
    */
  CAS_IOCMD_NO_WATCH_PROGRAM,
  /*!
    ABVCA:OTA info get
    */
  CAS_IOCMD_OTA_INFO_GET,
  /*!
    ABVCA:system time get
    */
  CAS_IOCMD_SYSTEM_TIME_GET,
  /*!
    ABVCA:recieve message from other STB
    */
  CAS_IOCMD_REC_MSG_FORM_OTHER_STB,
  /*!
    ABVCA:recieve message from other STB timeout
    */
  CAS_IOCMD_REC_MSG_FORM_OTHER_STB_TIMEOUT,
  /*!
    ABVCA:mobile mail get ua
    */
  CAS_IOCOM_MOBILE_UA_GET,
  /*!
    ABVCA:STB extended info get
    */
  CAS_IOCOM_STB_EXTENDED_INFO_GET,
  /*!
    ABVCA:get ecmemm pid
    */
  CAS_IOCOM_GET_ECMEMM_PID,
  /*!
    ABVCA:get UA
    */
  CAS_IOCOM_UA_GET,
  /*!
    ABVCA:AS stbid get
    */
  CAS_IOCOM_AS_STBID_GET,
  /*!
    ABVCA:buy ippv from epg
    */
  CAS_IOCOM_IPPV_BUY_FROM_EPG,
  /*!
    ABVCA:new mail check
    */
  CAS_IOCOM_NEW_MAIL_CHECK,
  /*!
      set overdue data
      */
  CAS_IOCMD_CA_OVERDUE,
 /*!
      get card state
      */
  CAS_IOCMD_CARD_STATUS_GET,
  /*!
      set dmx slot
      */
  CAS_IOCMD_DMX_SLOT_SET,
}cas_ioctrl_cmd_t;
/*!
  The table should be processed by CAS
  */
typedef enum
{
/*!
    The PMT table
    */
  CAS_TID_PMT = 1,
/*!
    The CAT table
    */
  CAS_TID_CAT,
/*!
    The NIT table
    */
  CAS_TID_NIT,
}cas_tid_t;
/*!
  The event callback structure for CA system module
  */
typedef struct
{
/*!
    The callback function pointer.
    */
  cas_event_cb_t evt_cb;
/*!
    The context need to transfered by this callback function.
    */
  u32 context;
}cas_event_notify_t;

/*!
  The event structure for one slot
  */
typedef struct
{
  /*!
      To indicate which cam the events happened on
    */
  cas_module_id_t cam_id;
  /*!
      The events happened
    */
  u32 events[EVT_U32_LEN];
  /*!
    The current event parameter
    */
  u32 event_param;
}cas_slot_event_t;

/*!
  The event callback structure for CA system module
  */
typedef struct
{
  /*!
    The slot event info
    */
  cas_slot_event_t slot_evt[CAS_MAX_SLOT_NUM];
  /*!
      The flag to indicate on which slot the events happened
    */
  u8 happened[CAS_MAX_SLOT_NUM];
}cas_event_info_t;

/*!
  The structure used for io control command
  */
typedef struct
{
  /*!
      The specific cas module id, see cas_module_id_t
    */
  cas_module_id_t cam_id;
  /*!
      The parameter corresponding to the io control command
    */
  void *p_param;
}cas_io_param_t;

/*!
The cas ecm info
*/
typedef struct
{
  /*!
  ca system id
  */
  u16  ca_system_id;
  /*!
  es pid
    */
  u16  es_pid;
  /*!
  ecm pid
    */
  u16  ecm_id;
}cas_ecm_info_t;

/*!
The cas emm info
*/
typedef struct
{
  /*!
  ca system id
  */
  u16  ca_system_id;
  /*!
  emm pid
    */
  u16  emm_id;
}cas_emm_info_t;

/*!
The cas table info
*/
typedef struct
{
  /*!
  ca system id
    */
  u16  service_id;
  /*!
  ca ecm info
    */
  cas_ecm_info_t  ecm_info[CAS_MAX_ECMEMM_NUM];
  /*!
  ca emm info
    */
  cas_emm_info_t  emm_info[CAS_MAX_ECMEMM_NUM];
  /*!
  tp
  */
  u32 cur_tp_freq;
}cas_table_info_t;


/*!
  Attach the CAS module: ABV
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_abv_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: ADT_MG(by SanZhouXunChi)
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_adt_mg_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  For 4.2.x
  Attach the CAS module: ADT_MG(by SanZhouXunChi)
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_adt_mg_attach_v42x(cas_module_cfg_t *p_cfg, u32 *p_cam_id);


/*!
  Attach the CAS module: boyuan
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_by_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: ADT_MG(by SanZhouXunChi)
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_conaxk_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: DESAI
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_ds_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: DESAI
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_ds_attach_v5(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: gs
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_gs_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: guotong
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_gt_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: irdeto
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_irdeto_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: lixin
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_lx_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: Pay4View
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_p4v_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);
/*!
  Attach the CAS module: SHIDA
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_shida_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: ST
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_st_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: SUMAVISION
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_sv_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: SUMAVISION
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE adcas_sv_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);


/*!
  Attach the CAS module: tf
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_tf_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: Topreal
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_tr_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: wf
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_wf_attach(cas_module_cfg_t * p_cfg, u32 * p_cam_id);
/*!
  Attach the CAS module: Sumavision
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_sv_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);


/*!
  Attach the CAS module: Sumavision
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_yj_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: TF YongXinShiBo
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_yxsb_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);

/*!
  Attach the CAS module: zhh
  \param[in] p_cfg The configuation of this CAS
  \param[out] p_cam_id The attached CAS handle

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_zhh_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id);








/*!
  Detach the CAS module
  \param[in] cam_id CAS module id

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_detach(cas_module_id_t cam_id);

/*!
  Initialize CAS adapter
  \param[in] p_cfg the configuration of CAS adapter

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_init(cas_adapter_cfg_t *p_cfg);

/*!
  De-Initialize CAS adapter

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_deinit();

/*!
  Initialize a specific CAS module
  \param[in] cam_id CAS module id

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_module_init(cas_module_id_t cam_id);

/*!
  De-Initialize a specific CAS module
  \param[in] cam_id CAS module id

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_module_deinit(cas_module_id_t cam_id);

/*!
  Register event notify function for CAS module
  \param[in] p_notify The callback function information

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_event_notify_register(cas_event_notify_t *p_notify);

/*!
  Identify the cas module's information by ca system id
  \param[in] ca_sys_id The CAS System ID of this CAS module
  \param[out] p_cam_id The CAS module ID should be identified, see cas_module_id_t
  \param[out] p_is_actived The flag to indicate if this cas module is actived
  \return Return the cas module id
  */
extern RET_CODE cas_identify(u16 ca_sys_id, u32 *p_cam_id, BOOL *p_is_actived);

/*!
  Process the other tables
  \param[in] t_id The table id, see cas_tid_t
  \param[in] p_buf The table buffer
  \param[out] p_result The result of processing

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_table_process(u32 t_id, u8 *p_buf, u32 *p_result);

/*!
  Polling the current event of this CAS module
  \param[out] events All the events happened in type of bit mask

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_event_polling(cas_event_info_t *p_event_info);

/*!
  Parse the event from bit mask to the value defined in cas_event_t
  when events[2] filled in multi event bits,
  the function can be called multi-times until return 0 means no event can be parsed.
  \param[in] events[2] The event bit mask, maybe filled in multi event bits.

  \return Return the value defined in cas_event_t, if return 0, means no event can be parsed
  */
extern cas_event_id_t cas_event_parse(u32 events[EVT_U32_LEN]);

/*!
  IO control function for this CAS module
  \param[in] slot The index of the slot in which the card plug,
                   when set it as CAS_INVALID_SLOT, should transfer the parameter using cas_io_param_t
  \param[in] cmd The IO command, see cas_io_param_t
  \param[in] p_param The parameter of this command

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_io_ctrl(u32 slot, u32 cmd, cas_io_param_t *p_param);

/*!
  Descrambler control function
  \param[in] ca_sys_id The ca system id
  \param[in] cmd The command, see cas_ioctrl_cmd_t
  \param[in] p_param The parameter of this command

  \return Return 0 for success and others for failure.
  */
extern RET_CODE cas_descrambler_ctrl(u16 ca_sys_id, u32 cmd, void *p_param);


#endif //__CAS_WARE_H__
