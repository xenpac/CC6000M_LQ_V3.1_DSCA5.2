/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_misc.h"
#include "drv_dev.h"
#include "../../../drv/drvbase/drv_svc.h"
#include "dmx.h"
#include "nim.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "../cas_adapter.h"

#include "CDCASS.h"
#include "CDCAS_include.h"

#define YXSB_ADT_DEBUG_EN

#ifdef YXSB_ADT_DEBUG_EN
#define YXSB_ADT_DEBUG OS_PRINTF
#else
#define YXSB_ADT_DEBUG(...) do{}while(0)
#endif


scard_device_t *p_yxsb_scard_dev = NULL;

static cas_adapter_priv_t *p_g_cas_priv = NULL;
extern ipp_buy_info_t IppvBuyInfo;
extern msg_info_t OsdMsg;
extern finger_msg_t FingerInfo;
extern cas_force_channel_t ForceChannelInfo;

extern os_sem_t FingerInfo_lock;
extern os_sem_t OsdMsg_lock;
extern os_sem_t IppvBuyInfo_lock;
extern os_sem_t ForceChannelInfo_lock;

extern RET_CODE YXSB_STB_Drv_AdtInit(void);
extern u32 CAS_YXSB_ConvertDate(u16 days, cas_yxsb_time_t* yxsb_time);
extern void YXSB_STB_Drv_CaReqLock(os_sem_t * pSemaphore);
extern void YXSB_STB_Drv_CaReqUnLock(os_sem_t * pSemaphore);
extern void YXSB_STB_ReleaseOncePrivateDataFilter( void );

static u32 YXSB_ADT_ParseEvent(u32 event)
{
    u32 event_tmp = 0;
    
    switch(event)
    {
        case CDCA_RC_OK:
            YXSB_ADT_DEBUG("CDCA_RC_OK!!\n");
            event_tmp = CAS_EVT_NONE;
            break;
        case CDCA_RC_UNKNOWN:
            YXSB_ADT_DEBUG("CDCA_RC_UNKNOWN!!\n");
            event_tmp = CAS_E_UNKNOW_ERR;
            break;
        case CDCA_RC_POINTER_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_POINTER_INVALID!!\n");
            event_tmp = CAS_E_INVALID_POINTER;
            break;
        case CDCA_RC_CARD_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_INVALID!!\n");
            event_tmp = CAS_E_ILLEGAL_CARD;
            break;
        case CDCA_RC_PIN_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_PIN_INVALID!!\n");
            event_tmp = CAS_E_PIN_INVALID;
            break;
        case CDCA_RC_DATASPACE_SMALL:
            YXSB_ADT_DEBUG("CDCA_RC_DATASPACE_SMALL!!\n");
            event_tmp = CAS_E_DATASPACE_SMALL;
            break;
        case CDCA_RC_CARD_PAIROTHER:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_PAIROTHER!!\n");
            event_tmp = CAS_E_CARD_PAIROTHER;
            break;
        case CDCA_RC_DATA_NOT_FIND:
            YXSB_ADT_DEBUG("CDCA_RC_DATA_NOT_FIND!!\n");
            event_tmp = CAS_E_DATA_NOT_FIND;
            break;
        case CDCA_RC_PROG_STATUS_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_PROG_STATUS_INVALID!!\n");
            event_tmp = CAS_E_PROG_STATUS_INVALID;
            break;
        case CDCA_RC_CARD_NO_ROOM:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_NO_ROOM!!\n");
            event_tmp = CAS_E_CARD_NO_ROOM;
            break;
        case CDCA_RC_WORKTIME_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_WORKTIME_INVALID!!\n");
            event_tmp = CAS_E_CARD_OUT_WORK_TIME;
            break;
        case CDCA_RC_IPPV_CANNTDEL:
            YXSB_ADT_DEBUG("CDCA_RC_IPPV_CANNTDEL!!\n");
            event_tmp = CAS_E_IPPV_CANNTDEL;
            break;
        case CDCA_RC_CARD_NOPAIR:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_NOPAIR!!\n");
            event_tmp = CAS_E_CARD_DIS_PARTNER;
            break;
        case CDCA_RC_WATCHRATING_INVALID:
            YXSB_ADT_DEBUG("CDCA_RC_WATCHRATING_INVALID!!\n");
            event_tmp = CAS_E_WATCHRATING_INVALID;
            break;
        case CDCA_RC_CARD_NOTSUPPORT:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_NOTSUPPORT!!\n");
            event_tmp = CAS_ERROR_CARD_NOTSUPPORT;
            break;
        case CDCA_RC_DATA_ERROR:
            YXSB_ADT_DEBUG("CDCA_RC_DATA_ERROR!!\n");
            event_tmp = CAS_E_UNKNOW_ERR;
            break;
        case CDCA_RC_FEEDTIME_NOT_ARRIVE:
            YXSB_ADT_DEBUG("CDCA_RC_FEEDTIME_NOT_ARRIVE!!\n");
            event_tmp = CAS_E_FEEDTIME_NOT_ARRIVE;
            break;
        case CDCA_RC_CARD_TYPEERROR:
            YXSB_ADT_DEBUG("CDCA_RC_CARD_TYPEERROR!!\n");
            event_tmp = CAS_E_ILLEGAL_CARD;
            break;
        case CDCA_RC_CAS_FAILED:
            YXSB_ADT_DEBUG("CDCA_RC_CAS_FAILED!!\n");
            event_tmp = CAS_E_UNKNOW_ERR;
            break;
        case CDCA_RC_OPER_FAILED:
            YXSB_ADT_DEBUG("CDCA_RC_OPER_FAILED!!\n");
            event_tmp = CAS_E_UNKNOW_ERR;
            break;
        default:
            YXSB_ADT_DEBUG("Unkown Return Valure, event = %d!!\n", event);
            event_tmp = CAS_E_UNKNOW_ERR;
            break;
    }
    return event_tmp;
}

static u32 _YXSB_GetTimeByZone(u8 *phour, u8 *pminute, s8 *pday)
{
    u8 u8TimeZone = 8;

    if(NULL == phour || NULL == pminute || NULL == pday)
        return ERR_FAILURE;

    if(*phour > 23 || *pminute > 59)
        return ERR_FAILURE;

    *phour += u8TimeZone;

    if(*phour >= 24)
    {
        *pday = 1;
        *phour -= 24;
    }

  return SUCCESS;
}

static u32 _YXSB_GetDate(u16 udays, cas_yxsb_time_t* pSysTime)
{
    u32 nyear = 0;
    u32 nMon = 0;
    u32 nday = 0;
    u32 nTemp = 0;

    if (udays > (100 * 366))
    {
        OS_PRINTF( "Wrong YXSB Date %d\n", udays);
        return 1;
    }
    /*year.*/
    nyear = 1970;
    do
    {
        if((nyear % 4 == 0 && nyear % 100 != 0) || nyear % 400 == 0)
            nTemp = 366;
        else
            nTemp = 365;

        if(nTemp > udays)
            break;

        udays -= nTemp;
        nyear++;
    }while(1);

    /*month.*/
    nMon = 1;
    do
    {  
        switch (nMon)
        {
            case 2 :
                if ((nyear % 4 == 0 && nyear % 100 != 0) || nyear % 400 == 0)
                    nTemp = 29;
                else
                    nTemp = 28;
                break;

            case 4:
            case 6:
            case 9:
            case 11:
                nTemp = 30;
                break;

            default:
                nTemp = 31;
                break;
        }
        if(nTemp > udays)
            break;
        
        udays -= nTemp;
        nMon++;
    }while(1);

    if(1 > nMon || 12 < nMon)
    {
        return ERR_FAILURE;
    }

    /*day.*/
    nday = 1;
    nday += udays; 
    if(1 > nday || 31 < nday)
    {
        return ERR_FAILURE;
    }

    pSysTime->day = (u8)nday;
    pSysTime->month = (u8)nMon;
    pSysTime->year = nyear;

    return SUCCESS;
}

static u32 CAS_YXSB_ConvertU32Time(u32 u32Time, cas_yxsb_time_t* pSysTime)
{
    u32 days = 0;
    u32 hour = 0;
    u32 minute = 0;
    u32 second = 0;
    s8 sdayFlag = 0;

    days = u32Time / (24 * 3600);
    second = u32Time % (24 * 3600);

    hour = second / 3600;
    second %= 3600;
    minute = second / 60;
    second %= 60;

    pSysTime->hour = (u8)hour;
    pSysTime->minute = (u8)minute;
    pSysTime->second = (u8)second;
    _YXSB_GetTimeByZone(&(pSysTime->hour), &(pSysTime->minute), &sdayFlag);

    days = days + sdayFlag;

    _YXSB_GetDate(days, pSysTime);  

    return SUCCESS;
}

static u32 YXSB_ADT_GetOperatorInfo(u8 *max_num, cas_operator_info_t *pOperatorInfo)
{
    u16 wTVSIDs[CDCA_MAXNUM_OPERATOR] = {0};;
    SCDCAOperatorInfo pOptInfo = {{0,},};
    u32 ret = 0;
    u8 uIndex = 0;
    u8 Index = 0;

    if(NULL == pOperatorInfo)
        return ERR_FAILURE;

    ret = CDCASTB_GetOperatorIds(wTVSIDs);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < CDCA_MAXNUM_OPERATOR;uIndex++)
    {
        if(wTVSIDs[uIndex] == 0)
            break;

        ret = CDCASTB_GetOperatorInfo(wTVSIDs[uIndex], &pOptInfo);
        if(ret != CDCA_RC_OK)
        {
            YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
            continue;
            //return ERR_FAILURE;
        }

        pOperatorInfo[uIndex].operator_id = (u32)wTVSIDs[uIndex];
        memcpy(pOperatorInfo[uIndex].operator_name, pOptInfo.m_szTVSPriInfo, CDCA_MAXLEN_TVSPRIINFO);

        ret = CDCASTB_GetACList(wTVSIDs[uIndex], pOperatorInfo[uIndex].p_ACArray);
        if(ret != CDCA_RC_OK)
            YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);

        YXSB_ADT_DEBUG("operator_id = %d, private info = %s, %s\n", 
                                            pOperatorInfo[uIndex].operator_id,
                                            pOperatorInfo[uIndex].operator_name,
                                            pOptInfo.m_szTVSPriInfo);

        YXSB_ADT_DEBUG("private info:\n");
        for(Index = 0;Index < CDCA_MAXLEN_TVSPRIINFO; Index++)
            YXSB_ADT_DEBUG("%d", pOptInfo.m_szTVSPriInfo[Index]);
        YXSB_ADT_DEBUG("\n");

        YXSB_ADT_DEBUG("ACList:\n");
        for(Index = 0;Index < CDCA_MAXNUM_ACLIST; Index++)
            YXSB_ADT_DEBUG("%d", pOperatorInfo[uIndex].p_ACArray[Index]);
        YXSB_ADT_DEBUG("\n");
        
    }

    *max_num = uIndex;
    YXSB_ADT_DEBUG("max_num = %d\n", *max_num);
    return ret;
}

static u32 YXSB_ADT_GetBurseInfo(u16 OperatorID, u16 *max_num, burse_info_t *pSlotInfo)
{
    u32 ret = 0;
    u8 uIndex = 0;
    SCDCATVSSlotInfo CDSlotInfo = {0};
    u8 slot_id[CDCA_MAXNUM_SLOT] = {0, };
    if(NULL == pSlotInfo)
        return ERR_FAILURE;

    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);

    ret = CDCASTB_GetSlotIDs(OperatorID, slot_id);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < CDCA_MAXNUM_SLOT; uIndex++)
    {
        if(slot_id[uIndex] == 0)
            break;
        
        ret = CDCASTB_GetSlotInfo(OperatorID, slot_id[uIndex], &CDSlotInfo);
        if(ret != CDCA_RC_OK)
        {
            YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
            continue;
            //return ERR_FAILURE;
        }

        pSlotInfo[uIndex].balance = CDSlotInfo.m_wBalance;
        pSlotInfo[uIndex].credit_limit = CDSlotInfo.m_wCreditLimit;
        pSlotInfo[uIndex].record_index = slot_id[uIndex];
        
        YXSB_ADT_DEBUG("slot id = %d, balance = %d, credit limit = %d\n",
                                            pSlotInfo[uIndex].record_index,
                                            pSlotInfo[uIndex].balance,
                                            pSlotInfo[uIndex].credit_limit);
    }

    *max_num = uIndex;
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, *max_num);

    return ret;    
}

static u32 YXSB_ADT_GetIppvProgram(u16 OperatorID, u16 * max_num,  ipp_info_t * pIppvInfo)
{
    u32 ret = 0;
    SCDCAIppvInfo pCDIppv[CDCA_MAXNUM_IPPVP] = {{0, }, };
    u16 NumberToRead = CDCA_MAXNUM_IPPVP;
    u16 uIndex = 0;
    cas_yxsb_time_t yxsb_time = {0};
    if(pIppvInfo == NULL)
        return ERR_FAILURE;

    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);
    ret = CDCASTB_GetIPPVProgram(OperatorID, pCDIppv, &NumberToRead);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < NumberToRead; uIndex++)
    {
        pIppvInfo[uIndex].is_can_tape = pCDIppv[uIndex].m_bCanTape;
        switch(pCDIppv[uIndex].m_byBookEdFlag)
        {
            case CDCA_IPPVSTATUS_BOOKING:
                pIppvInfo[uIndex].book_state_flag = 0;
                break;
            case CDCA_IPPVSTATUS_VIEWED:
                pIppvInfo[uIndex].book_state_flag = 1;
                break;
            default:
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!\n", __FUNCTION__, __LINE__);
                pIppvInfo[uIndex].book_state_flag = 0xFF;
                break;
        }

        pIppvInfo[uIndex].product_id = pCDIppv[uIndex].m_dwProductID;
        pIppvInfo[uIndex].price = pCDIppv[uIndex].m_wPrice;

        CAS_YXSB_ConvertDate(pCDIppv[uIndex].m_wExpiredDate, &yxsb_time);
        pIppvInfo[uIndex].expired_time[0] = yxsb_time.year / 100;
        pIppvInfo[uIndex].expired_time[1] = yxsb_time.year % 100;
        pIppvInfo[uIndex].expired_time[2] = yxsb_time.month;
        pIppvInfo[uIndex].expired_time[3] = yxsb_time.day;
        pIppvInfo[uIndex].expired_time[4] = yxsb_time.hour;
        pIppvInfo[uIndex].expired_time[5] = yxsb_time.minute;
        pIppvInfo[uIndex].expired_time[6] = yxsb_time.second;

        pIppvInfo[uIndex].burse_id = pCDIppv[uIndex].m_bySlotID;

        YXSB_ADT_DEBUG("m_wExpiredDate = %d, is_can_tape = %d, book_state_flag = %d, product_id = %d, price = %d, expired_time:%d-%d-%d %d-%d-%d, burst id = %d\n",
                                            pCDIppv[uIndex].m_wExpiredDate,
                                            pIppvInfo[uIndex].is_can_tape,
                                            pIppvInfo[uIndex].book_state_flag,
                                            pIppvInfo[uIndex].product_id,
                                            pIppvInfo[uIndex].price,
                                            pIppvInfo[uIndex].expired_time[0] * 100 + pIppvInfo[uIndex].expired_time[1],
                                            pIppvInfo[uIndex].expired_time[2],
                                            pIppvInfo[uIndex].expired_time[3],
                                            pIppvInfo[uIndex].expired_time[4],
                                            pIppvInfo[uIndex].expired_time[5],
                                            pIppvInfo[uIndex].expired_time[6],
                                            pIppvInfo[uIndex].burse_id);
    }
    *max_num = NumberToRead;
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, *max_num);

    return ret;
}

static u32 YXSB_ADT_GetOperatorChildStatus(u16 OperatorID, chlid_card_status_info *ptChildStatus)
{
    u32 ret = 0;
    u16 days = 0;
    CDCA_U8 pbyIsChild;
    CDCA_U16 pwDelayTime;
    CDCA_TIME pLastFeedTime;
    char pParentCardSN[CDCA_MAXLEN_SN + 1];
    CDCA_BOOL pbIsCanFeed;
    cas_yxsb_time_t yxsb_time = {0};
    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);
    
    ret = CDCASTB_GetOperatorChildStatus(OperatorID, 
                                                                        &pbyIsChild, 
                                                                        &pwDelayTime, 
                                                                        &pLastFeedTime, 
                                                                        pParentCardSN, 
                                                                        &pbIsCanFeed);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    ptChildStatus->is_child = pbyIsChild;
    
    if(ptChildStatus->is_child == 0)
    {
        //mother card
        YXSB_ADT_DEBUG("--->Mother Card!!!\n");
        return SUCCESS;
    }
    ptChildStatus->delay_time = pwDelayTime;
    memcpy(ptChildStatus->parent_card_sn, pParentCardSN, CDCA_MAXLEN_SN + 1);
    ptChildStatus->parent_card_sn[CDCA_MAXLEN_SN + 1] = 0;
    YXSB_ADT_DEBUG("parent_card_sn: %s\n", ptChildStatus->parent_card_sn);
    ptChildStatus->is_can_feed = pbIsCanFeed;

    days = pLastFeedTime >> 16;
    YXSB_ADT_DEBUG("pLastFeedTime = 0x%x, %d, days = %d\n", pLastFeedTime, pLastFeedTime, days);
    CAS_YXSB_ConvertDate(days, &yxsb_time);
    ptChildStatus->last_feed_time[0] = yxsb_time.year / 100;
    ptChildStatus->last_feed_time[1] = yxsb_time.year % 100;
    ptChildStatus->last_feed_time[2] = yxsb_time.month;
    ptChildStatus->last_feed_time[3] = yxsb_time.day;
    ptChildStatus->last_feed_time[4] = (pLastFeedTime >> 11) & 0x1F;
    ptChildStatus->last_feed_time[5] = (pLastFeedTime >> 5) & 0x3F;
    ptChildStatus->last_feed_time[6] = pLastFeedTime & 0x1F;

    YXSB_ADT_DEBUG("is_child = %d, delay_time = %d, is_can_feed = %d, last_feed_time:%d-%d-%d %d-%d-%d\n",
                                        ptChildStatus->is_child,
                                        ptChildStatus->delay_time,
                                        ptChildStatus->is_can_feed,
                                        ptChildStatus->last_feed_time[0] * 100 + ptChildStatus->last_feed_time[1],
                                        ptChildStatus->last_feed_time[2],
                                        ptChildStatus->last_feed_time[3],
                                        ptChildStatus->last_feed_time[4],
                                        ptChildStatus->last_feed_time[5],
                                        ptChildStatus->last_feed_time[6]);

    return SUCCESS;
}

static u32 YXSB_ADT_GetProductInfo(u16 OperatorID,  u16 *max_num, product_entitle_info_t *ptProductEntitles)
{
    u32 ret = 0;
    SCDCAEntitles pCDEntitles = {0};
    u16 uIndex = 0;
    cas_yxsb_time_t yxsb_time = {0};

    if(ptProductEntitles == NULL)
        return ERR_FAILURE;
    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);

    ret = CDCASTB_GetServiceEntitles((CDCA_U16)OperatorID, &pCDEntitles);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < pCDEntitles.m_wProductCount; uIndex++)
    {
        ptProductEntitles[uIndex].product_id = pCDEntitles.m_Entitles[uIndex].m_dwProductID;
        ptProductEntitles[uIndex].can_tape = pCDEntitles.m_Entitles[uIndex].m_bCanTape;

        CAS_YXSB_ConvertDate(pCDEntitles.m_Entitles[uIndex].m_tBeginDate, &yxsb_time);
        ptProductEntitles[uIndex].start_time[0] = yxsb_time.year / 100;
        ptProductEntitles[uIndex].start_time[1] = yxsb_time.year % 100;
        ptProductEntitles[uIndex].start_time[2] = yxsb_time.month;
        ptProductEntitles[uIndex].start_time[3] = yxsb_time.day;
        ptProductEntitles[uIndex].start_time[4] = yxsb_time.hour;
        ptProductEntitles[uIndex].start_time[5] =yxsb_time.minute;
        ptProductEntitles[uIndex].start_time[6] =yxsb_time.second;

        CAS_YXSB_ConvertDate(pCDEntitles.m_Entitles[uIndex].m_tExpireDate, &yxsb_time);
        ptProductEntitles[uIndex].expired_time[0] = yxsb_time.year / 100;
        ptProductEntitles[uIndex].expired_time[1] = yxsb_time.year % 100;
        ptProductEntitles[uIndex].expired_time[2] = yxsb_time.month;
        ptProductEntitles[uIndex].expired_time[3] = yxsb_time.day;
        ptProductEntitles[uIndex].expired_time[4] = yxsb_time.hour;
        ptProductEntitles[uIndex].expired_time[5] = yxsb_time.minute;
        ptProductEntitles[uIndex].expired_time[6] = yxsb_time.second;

        YXSB_ADT_DEBUG("m_tExpireDate = %d, product_id = %d, can_tape = %d, start time:%d-%d-%d %d-%d-%d, expired time:%d-%d-%d %d-%d-%d\n",
                                            pCDEntitles.m_Entitles[uIndex].m_tExpireDate,
                                            ptProductEntitles[uIndex].product_id,
                                            ptProductEntitles[uIndex].can_tape,
                                            ptProductEntitles[uIndex].start_time[0] * 100 + ptProductEntitles[uIndex].start_time[1],
                                            ptProductEntitles[uIndex].start_time[2],
                                            ptProductEntitles[uIndex].start_time[3],
                                            ptProductEntitles[uIndex].start_time[4],
                                            ptProductEntitles[uIndex].start_time[5],
                                            ptProductEntitles[uIndex].start_time[6],
                                            ptProductEntitles[uIndex].expired_time[0] * 100 + ptProductEntitles[uIndex].expired_time[1],
                                            ptProductEntitles[uIndex].expired_time[2],
                                            ptProductEntitles[uIndex].expired_time[3],
                                            ptProductEntitles[uIndex].expired_time[4],
                                            ptProductEntitles[uIndex].expired_time[5],
                                            ptProductEntitles[uIndex].expired_time[6]);
    }

    *max_num = pCDEntitles.m_wProductCount;
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, *max_num);

    return ret;
}

static u32 YXSB_ADT_GetEntitleIDs(u16 OperatorID,  u16 *max_num, product_entitle_info_t *ptProductEntitles)
{
    u32 ret = 0;
    u32 EntitleIDs[CDCA_MAXNUM_ENTITLE] = {0};
    u16 uIndex = 0;

    if(ptProductEntitles == NULL)
        return ERR_FAILURE;
    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);

    ret = CDCASTB_GetEntitleIDs(OperatorID, EntitleIDs);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < CDCA_MAXNUM_ENTITLE; uIndex++)
    {
        if(EntitleIDs[uIndex] == 0)
            break;
        
        ptProductEntitles[uIndex].product_id = EntitleIDs[uIndex];
        YXSB_ADT_DEBUG("product = %d\n", ptProductEntitles[uIndex].product_id);
    }

    *max_num = uIndex;
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, *max_num);
    
    return ret;
}

static u32 YXSB_ADT_GetDetitleChkNums(product_detitles_info_t *ptProductDetitles)
{
    u32 ret = 0;
    u32 DetitleIDs[CDCA_MAXNUM_ENTITLE] = {0};
    u16 uIndex = 0;

    if(ptProductDetitles == NULL)
        return ERR_FAILURE;

    YXSB_ADT_DEBUG("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, ptProductDetitles->operator_id);

    ret = CDCASTB_GetDetitleChkNums(ptProductDetitles->operator_id, 
                                                                 &(ptProductDetitles->read_flag), 
                                                                 DetitleIDs);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < CDCA_MAXNUM_ENTITLE; uIndex++)
    {
        if(DetitleIDs[uIndex] == 0)
            break;
        
        ptProductDetitles->p_detitle_info[uIndex] = DetitleIDs[uIndex];
        YXSB_ADT_DEBUG("detitle info = %d\n", ptProductDetitles->p_detitle_info[uIndex]);
    }

    ptProductDetitles->max_num = uIndex;
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, ptProductDetitles->max_num);
    
    return ret;
}

static u32 YXSB_ADT_ChangeSmartCardPIN(cas_pin_modify_t * p_pin_modify)
{
    u8 uIndex = 0;
    u8 ch_old[CDCA_MAXLEN_PINCODE] = {0};
    u8 ch_new[CDCA_MAXLEN_PINCODE] = {0};
    u32 ret = 0;

    if(p_pin_modify == NULL)
        return ERR_FAILURE;

    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
    {
        ch_old[uIndex] = p_pin_modify->old_pin[uIndex];
        ch_new[uIndex] = p_pin_modify->new_pin[uIndex];
    }

    YXSB_ADT_DEBUG("Old PIN:\n");
    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
        YXSB_ADT_DEBUG("%d", ch_old[uIndex]);
    YXSB_ADT_DEBUG("\n");
    
    YXSB_ADT_DEBUG("New PIN:\n");
    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
        YXSB_ADT_DEBUG("%d", ch_new[uIndex]);
    YXSB_ADT_DEBUG("\n");

    ret = CDCASTB_ChangePin(ch_old, ch_new);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return ret;
}

static u32 YXSB_ADT_SetRating(u8 *pbyPIN, u8 byRating)
{
    u32 ret = 0;
    u8 uIndex = 0;
    u8 aPin[CDCA_MAXLEN_PINCODE] = {0};

    ///TODO: any limitions:byRating < 4 OR| byRating > 18?
    if(NULL == pbyPIN)
        return ERR_FAILURE;

    YXSB_ADT_DEBUG("PIN:\n");
    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
    {
        aPin[uIndex] = pbyPIN[uIndex];
        YXSB_ADT_DEBUG("%d", aPin[uIndex]);
    }
    YXSB_ADT_DEBUG("\n");

    YXSB_ADT_DEBUG("byRating = %d\n", byRating);

    ret = CDCASTB_SetRating(aPin, byRating);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return ret;
}

static u32 YXSB_ADT_SetWorkTime(cas_card_work_time_t *p_time)
{
    u32 ret = 0;
    u8 uIndex = 0;
    u8 ch_pin[CDCA_MAXLEN_PINCODE] = {0};

    if(p_time == NULL)
        return ERR_FAILURE;

    YXSB_ADT_DEBUG("PIN:\n");
    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
    {
        ch_pin[uIndex] = p_time->pin[uIndex];
        YXSB_ADT_DEBUG("%d", ch_pin[uIndex]);
    }
    YXSB_ADT_DEBUG("\n");
    YXSB_ADT_DEBUG("start time: %d-%d-%d, end time: %d-%d-%d\n", 
                                        p_time->start_hour, 
                                        p_time->start_minute, 
                                        p_time->start_second, 
                                        p_time->end_hour, 
                                        p_time->end_minute, 
                                        p_time->end_second);

    ret = CDCASTB_SetWorkTime(ch_pin, 
                                                       p_time->start_hour, 
                                                       p_time->start_minute, 
                                                       p_time->start_second, 
                                                       p_time->end_hour, 
                                                       p_time->end_minute, 
                                                       p_time->end_second);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return ret;
}

static u32 YXSB_ADT_GetMailContent(u32 MailID,cas_mail_body_t *p_mail_body)
{
    u32 ret = 0;
    SCDCAEmailContent CDEmailContent = {{0,},};

    if(NULL == p_mail_body)
        return ERR_FAILURE;

    ret = CDCASTB_GetEmailContent(MailID, &CDEmailContent);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    memcpy(p_mail_body->data, CDEmailContent.m_szEmail, CDCA_MAXLEN_EMAIL_CONTENT + 1);
    p_mail_body->data[CDCA_MAXLEN_EMAIL_CONTENT + 1] = 0;

    YXSB_ADT_DEBUG("Mail Body:%s\n", p_mail_body->data, CDEmailContent.m_szEmail);

    return ret;
}

static u32 YXSB_ADT_GetMailHeads(u16 *max_num, cas_mail_header_t *pMailHead)
{
    u32 ret = 0;
    u8 uIndex = 0;
    SCDCAEmailHead CDHead[CDCA_MAXNUM_EMAIL] = {{0,},};
    u8 uCount = 0;
    u8 uFromIndex = 0;
    cas_yxsb_time_t yxsb_time = {0};

    if(NULL == pMailHead)
        return ERR_FAILURE;

    uCount = CDCA_MAXNUM_EMAIL;
    uFromIndex = 0;

    ret = CDCASTB_GetEmailHeads(CDHead, &uCount, &uFromIndex);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    for(uIndex = 0; uIndex < uCount; uIndex++)
    {
        pMailHead[uIndex].m_id = CDHead[uIndex].m_dwActionID;
        pMailHead[uIndex].new_email = CDHead[uIndex].m_bNewEmail;
        memcpy(pMailHead[uIndex].subject, CDHead[uIndex].m_szEmailHead, CDCA_MAXLEN_EMAIL_TITLE + 1);
        pMailHead[uIndex].subject[CDCA_MAXLEN_EMAIL_TITLE + 1] = 0;
        pMailHead[uIndex].priority = CDHead[uIndex].m_wImportance;

        //YXSB_ADT_DEBUG("[YXSB]mail_id = %d, create time = %d\n", CDHead[uIndex].m_dwActionID, CDHead[uIndex].m_tCreateTime);
        CAS_YXSB_ConvertU32Time(CDHead[uIndex].m_tCreateTime, &yxsb_time);

        /*YXSB_ADT_DEBUG("[YXSB]convert time:\n year = %d, month = %d, day = %d, hour = %d, minute = %d\n", 
                                            yxsb_time.year, 
                                            yxsb_time.month, 
                                            yxsb_time.day,
                                            yxsb_time.hour,
                                            yxsb_time.minute);*/

        pMailHead[uIndex].creat_date[0] = yxsb_time.year / 100;
        pMailHead[uIndex].creat_date[1] = yxsb_time.year % 100;
        pMailHead[uIndex].creat_date[2] = yxsb_time.month;
        pMailHead[uIndex].creat_date[3] = yxsb_time.day;
        pMailHead[uIndex].creat_date[4] = yxsb_time.hour;
        pMailHead[uIndex].creat_date[5] = yxsb_time.minute;
        pMailHead[uIndex].creat_date[6] = yxsb_time.second;

        YXSB_ADT_DEBUG("m_id = %d, new_email = %d, subject = %s, prio = %d, date:%d-%d-%d %d:%d:%d\n",
                                            pMailHead[uIndex].m_id,
                                            pMailHead[uIndex].new_email,
                                            pMailHead[uIndex].subject,
                                            pMailHead[uIndex].priority,
                                            pMailHead[uIndex].creat_date[0] * 100 + pMailHead[uIndex].creat_date[1],
                                            pMailHead[uIndex].creat_date[2],
                                            pMailHead[uIndex].creat_date[3],
                                            pMailHead[uIndex].creat_date[4],
                                            pMailHead[uIndex].creat_date[5],
                                            pMailHead[uIndex].creat_date[6]);
    }

    *max_num = uCount;

    return ret;
}

static u32 YXSB_ADT_IppBuySet( ipp_buy_info_t *p_purch_info)
{
    u32 ret = 0;
    u8 uIndex = 0;
    SCDCAIPPVPrice  m_Price;
    u8 ch_pin[CDCA_MAXLEN_PINCODE] = {0};

    if(NULL == p_purch_info)
        return ERR_FAILURE;
    if(p_purch_info->ipp_price[0].price_type == CA_IPPVPRICETYPE_TPPNOTAP)
        m_Price.m_byPriceCode = CDCA_IPPVPRICETYPE_TPPVVIEW;
    else
        m_Price.m_byPriceCode = CDCA_IPPVPRICETYPE_TPPVVIEWTAPING;
    
    m_Price.m_wPrice = p_purch_info->ipp_price[0].price;
    YXSB_ADT_DEBUG("price_type = %d, price = %d\n", m_Price.m_byPriceCode, m_Price.m_wPrice);

    YXSB_ADT_DEBUG("PIN:\n");
    for(uIndex = 0; uIndex < CDCA_MAXLEN_PINCODE; uIndex++)
    {
        ch_pin[uIndex] = p_purch_info->pin[uIndex];
        YXSB_ADT_DEBUG("%d", ch_pin[uIndex]);
    }
    YXSB_ADT_DEBUG("\n");

    ret = CDCASTB_StopIPPVBuyDlg(p_purch_info->state_flag, 
                                                            p_purch_info->ecm_pid, 
                                                            ch_pin, 
                                                            &m_Price);
    if(ret != CDCA_RC_OK)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return ret;

}

static void YXSB_ADT_ParseCaDesc(u8 *pBuf, s32 nDescLen, yxsb_cas_desc_t* pDesc)
{
    u8 *p_data = pBuf;
    u16 ca_system_id = 0;
    //memset(pDesc, 0x00, sizeof(yxsb_cas_desc_t));

    YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    if((NULL != pBuf) && (NULL != pDesc))
    {
        ca_system_id = (p_data[0] << 8) | p_data[1];
        if(CDCASTB_IsCDCa(ca_system_id) == CDCA_TRUE)
        {
            pDesc->ca_system_id = (p_data[0] << 8) | p_data[1];
            pDesc->ca_pid = ((p_data[2] & 0x1F) << 8) | p_data[3];
        }
        YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d, [ca] pDesc->ca_system_id = 0x%x ca_pid=%d, ca_system_id = %d\n", __FUNCTION__, __LINE__,
            pDesc->ca_system_id, pDesc->ca_pid, ca_system_id);
    }
}

static void YXSB_ADT_ParsePmtDesc(u8 *buf, u16 length, yxsb_cas_desc_t *yxsb_cas_desc)
{
    u8 *p_data = buf;
    u8 descriptor_tag = 0;
    u8 descriptor_length = 0;

    memset(yxsb_cas_desc, 0x00, sizeof(yxsb_cas_desc_t));

    while(length > 0)
    {
        descriptor_tag = p_data[0];
        descriptor_length = p_data[1];
        YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d,descriptor_tag=0x%x\n", __FUNCTION__, __LINE__, descriptor_tag);

        if(0 == descriptor_length)
            break;

        length -= 2;
        p_data += 2;

        switch (descriptor_tag)
        {
            case 0x09:
                YXSB_ADT_ParseCaDesc(p_data, descriptor_length, yxsb_cas_desc);
                break;
            default:
                break;
        }

        //handle with SimulCrypt
        if(yxsb_cas_desc->ca_pid && yxsb_cas_desc->ca_system_id)
            break;
        
        length -= descriptor_length;
        p_data = p_data + descriptor_length;
    }
}

static void YXSB_ADT_ParsePMT(u8 *p_buf, s_cas_yxsb_priv_t* p_priv)
{
    u8 table_id = 0;
    u16 section_length = 0;
    u16 program_number = 0;
    u8 version_number = 0;
    u16 pcr_pid = 0;
    u16 program_info_length = 0;
    u8 stream_type = 0;
    u16 elementary_pid = 0;
    u8 es_info_length = 0;
    u8 *p_data = NULL;

    YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    MT_ASSERT(p_buf != NULL);

    p_data = p_buf;

    table_id = p_data[0];
    section_length = (((u16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
    if(section_length > 0)
    {
          program_number = (u16)p_data[3] << 8 | p_data[4];
          p_priv->program_id = program_number;
          version_number = (p_data[5] & 0x3E) >> 1;
          pcr_pid = ((u16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

          program_info_length = ((u16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
          p_data += 12;
          
          YXSB_ADT_ParsePmtDesc(p_data, program_info_length, &p_priv->cas_desc);
          OS_PRINTF("ECM_PID = 0x%x\n", p_priv->cas_desc.ca_pid);
          p_data += program_info_length;
          section_length -= (13 + program_info_length);//head and crc

          while(section_length > 0)
          {
                stream_type = p_data[0];
                elementary_pid = ((u16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
                OS_PRINTF(" stream_type=0x%x, elementary_PID=0x%x\n", stream_type, elementary_pid);
                es_info_length = ((u16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
                p_data += 5;
                switch(stream_type)
                {
                    case DVB_STREAM_MG2_VIDEO:
                    case DVB_STREAM_MG1_VIDEO:
                    case DVB_STREAM_264_VIDEO:
                    case DVB_STREAM_AVS_VIDEO:
                        YXSB_ADT_ParsePmtDesc(p_data, es_info_length, &p_priv->vedio_desc.v_cas_desc);
                        break;
                    case DVB_STREAM_MG1_AUDIO:
                    case DVB_STREAM_MG2_AUDIO:
                    case DVB_STREAM_AAC_AUDIO:
                        YXSB_ADT_ParsePmtDesc(p_data, es_info_length, &p_priv->audio_desc.a_cas_desc);
                        break;
                }
                p_data += es_info_length;
                section_length -= (es_info_length + 5);
          }
}

}

static RET_CODE cas_yxsb_init()
{
    static u8 init_flag = 0;
    RET_CODE ret = 0;
    u8 task_prio = 0;
    s_cas_yxsb_priv_t *p_priv = NULL;

    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]\n", __FUNCTION__, __LINE__);

    p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    if(p_priv->task_prio)
        task_prio = p_priv->task_prio;
    else
        task_prio = YXSB_TASK_PRIORITY_0;

    if(!init_flag)
    {
        ret = YXSB_STB_Drv_AdtInit();
        if(ret != SUCCESS)
        {
            YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!\n", __FUNCTION__, __LINE__);
            return ERR_FAILURE;
        }
        
        ret = CDCASTB_Init(task_prio);
        if(ret == CDCA_FALSE)
        {
            YXSB_ADT_DEBUG("[YXSB]Fatal Error on [Func:%s, Line:%d]\n", __FUNCTION__, __LINE__);
            return ERR_FAILURE;
        }
        init_flag = 1;
    }

    YXSB_ADT_DEBUG("[YXSB]cas_yxsb_init SUCC!\n");
    return SUCCESS;
}

static RET_CODE cas_yxsb_deinit()
{
    YXSB_ADT_DEBUG("[YXSB][Func:%s, Line:%d]\n", __FUNCTION__, __LINE__);
    //CDCASTB_Close();
    return SUCCESS;
}

static RET_CODE cas_yxsb_identify(u16 ca_sys_id)
{
     RET_CODE ret = 0;

     ret = CDCASTB_IsCDCa(ca_sys_id);
     if(ret != CDCA_TRUE)
     {
        YXSB_ADT_DEBUG("[YXSB]Not YongXinShiBo CA Card!\n");
        return ERR_FAILURE;
     }

    return SUCCESS;
}

static RET_CODE cas_yxsb_card_reset(u32 slot, card_reset_info_t *p_info)
{
    CDCA_BOOL nRet = 0;
    s_cas_yxsb_priv_t *p_priv = NULL;
    YXSB_ADT_DEBUG("[YXSB]Card Insert!\n");

    p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;

    p_yxsb_scard_dev = (scard_device_t *)p_info->p_smc;
    scard_pro_register_op(p_info->p_smc);
    p_priv->slot = slot;

    ///TODO: anything to do??
    p_priv->card_status = SMC_CARD_INSERT;
    nRet = CDCASTB_SCInsert();
    if(nRet != CDCA_TRUE)
    {
        YXSB_ADT_DEBUG("[YXSB]Fatal Error on [Func:%s, Line:%d]Card Insert Error!!!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }

    return SUCCESS;    
}

static RET_CODE cas_yxsb_card_remove()
{
    s_cas_yxsb_priv_t *p_priv = NULL;
    YXSB_ADT_DEBUG("[YXSB]Card Remove!\n");

    p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;
    p_priv->card_status = SMC_CARD_REMOVE;

    ///TODO: anything to do??
    CDCASTB_SCRemove();
    cas_send_event(p_priv->slot, CAS_ID_TF, CAS_S_CARD_REMOVE, 0);

    return SUCCESS;
}

static RET_CODE cas_yxsb_io_ctrl(u32 cmd, void *param)
{
    u32 lib_ver = 0;
    u32 ret = SUCCESS;
    u16 uIndex = 0;
    u8 pbyEmailNum = 0;
    u8 pbyEmptyNum = 0;
    SCDCASServiceInfo pServiceInfo = {0,};
    s_cas_yxsb_priv_t *p_priv = p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;
    
    cas_card_info_t *p_card_info = NULL;
    cas_table_info_t *p_cas_table_info = NULL;
    cas_card_work_time_t *p_work_time = NULL;
    cas_rating_set_t *p_rating_set = NULL;
    card_stb_paired_list_t *p_stb_paired_list = NULL;
    product_entitles_info_t *p_entitles_info = NULL;
    product_detitles_info_t *p_detitles_info = NULL;
    cas_pin_modify_t *p_pin_modify = NULL;
    burses_info_t *p_burses_info = NULL;
    cas_operators_info_t *p_operators_info = NULL;
    cas_mail_headers_t *p_mail_headers = NULL;
    cas_mail_body_t *p_mail_body = NULL;
    msg_info_t *p_osd_msg = NULL;
    finger_msg_t *p_finger_msg = NULL;
    cas_force_channel_t *p_force_channel = NULL;
    ipps_info_t *p_ippv_info = NULL;
    //nim_channel_info_t *p_nim_info = NULL;
    //nim_device_t *p_nim_dev = NULL;
    chlid_card_status_info *p_child_satus =  NULL;
    ipp_buy_info_t *p_purch_info = NULL;
    u32 *p_mail_id = NULL;
    //u8 *p_nit_zone = NULL;
    u8 *p_rating = 0;
    u16 p_wTVSID = 0;
    u32 *p_ACArray = NULL;
    u16 *p_platID = NULL;

    switch(cmd)
    {
        case CAS_IOCMD_VIDEO_PID_SET:
            memset(&p_priv->vedio_desc.v_cas_desc, 0x00, sizeof(yxsb_cas_desc_t));
            p_priv->vedio_desc.v_pid = *((u16 *)param);
            YXSB_ADT_DEBUG("[YXSB]v_pid=0x%x \n",p_priv->vedio_desc.v_pid);
            break;
            
        case CAS_IOCMD_AUDIO_PID_SET:
            memset(&p_priv->audio_desc.a_cas_desc, 0x00, sizeof(yxsb_cas_desc_t));
            p_priv->audio_desc.a_pid = *((u16 *)param);
            YXSB_ADT_DEBUG("[YXSB]a_pid=0x%x \n",p_priv->audio_desc.a_pid);
            break;
            
        case CAS_IOCMD_SET_ECMEMM:
            YXSB_ADT_DEBUG("[YXSB]CAS_IOCMD_SET_ECMEMM \n");
            
            p_cas_table_info = (cas_table_info_t *)param;
            YXSB_STB_ReleaseOncePrivateDataFilter();

            memset(&p_priv->cas_desc, 0x00, sizeof(yxsb_cas_desc_t));
            YXSB_ADT_DEBUG("[YXSB]v_pid=0x%x \n",p_priv->vedio_desc.v_pid);
            YXSB_ADT_DEBUG("[YXSB]a_pid=0x%x \n",p_priv->audio_desc.a_pid);
            
            #if 1
            p_priv->program_id = p_cas_table_info->service_id;
            //CDCASTB_SetEcmPid(CDCA_LIST_FIRST, NULL);
            
            for(uIndex= 0; uIndex < CAS_MAX_ECMEMM_NUM; uIndex++)
            {
                if(CDCA_TRUE== CDCASTB_IsCDCa(p_cas_table_info->ecm_info[uIndex].ca_system_id))
                {
                    if(p_cas_table_info->ecm_info[uIndex].es_pid == 0)
                    {
                        p_priv->cas_desc.ca_pid = p_cas_table_info->ecm_info[uIndex].ecm_id;
                        p_priv->cas_desc.ca_system_id = p_cas_table_info->ecm_info[uIndex].ca_system_id;
                    }
                    else if(p_cas_table_info->ecm_info[uIndex].es_pid == p_priv->vedio_desc.v_pid)
                    {
                        p_priv->vedio_desc.v_cas_desc.ca_pid = p_cas_table_info->ecm_info[uIndex].ecm_id;
                        p_priv->vedio_desc.v_cas_desc.ca_system_id = p_cas_table_info->ecm_info[uIndex].ca_system_id;
                    }
                    else if(p_cas_table_info->ecm_info[uIndex].es_pid == p_priv->audio_desc.a_pid)
                    {
                        p_priv->audio_desc.a_cas_desc.ca_pid = p_cas_table_info->ecm_info[uIndex].ecm_id;
                        p_priv->audio_desc.a_cas_desc.ca_system_id = p_cas_table_info->ecm_info[uIndex].ca_system_id;
                    }
                    
                    //CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
                }
            }
            //CDCASTB_SetEcmPid(CDCA_LIST_OK, NULL);
            OS_PRINTF("ecm_pid = 0x%x, audio_ecm_pid = 0x%x, video_ecm_pid = 0x%x, pro_id = %d\n",
                                    p_priv->cas_desc.ca_pid,
                                    p_priv->audio_desc.a_cas_desc.ca_pid,
                                    p_priv->vedio_desc.v_cas_desc.ca_pid,
                                    p_priv->program_id);

            CDCASTB_SetEcmPid(CDCA_LIST_FIRST, NULL);

            if(p_priv->cas_desc.ca_pid)
            {
                memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                
                pServiceInfo.m_wEcmPid = p_priv->cas_desc.ca_pid;
                pServiceInfo.m_byServiceNum = 1;
                pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
            }
            else
            {
                if(p_priv->audio_desc.a_cas_desc.ca_pid)
                {
                    memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                    
                    pServiceInfo.m_wEcmPid = p_priv->audio_desc.a_cas_desc.ca_pid;
                    pServiceInfo.m_byServiceNum = 1;
                    pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                    CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
                }
                
                if(p_priv->vedio_desc.v_cas_desc.ca_pid)
                {
                    memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                    
                    pServiceInfo.m_wEcmPid = p_priv->vedio_desc.v_cas_desc.ca_pid;
                    pServiceInfo.m_byServiceNum = 1;
                    pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                    CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
                }
            }

            CDCASTB_SetEcmPid(CDCA_LIST_OK, NULL);
            #endif
            ///TODO: finish the func EMM
            for(uIndex= 0; uIndex < CAS_MAX_ECMEMM_NUM; uIndex++)
            {
                if(CDCA_TRUE== CDCASTB_IsCDCa(p_cas_table_info->emm_info[uIndex].ca_system_id))
                {
                    CDCASTB_SetEmmPid(p_cas_table_info->emm_info[uIndex].emm_id);
                }
            }
            break;
            
        case CAS_IOCMD_CARD_INFO_GET:
            YXSB_ADT_DEBUG("[YXSB]CAS_IOCMD_CARD_INFO_GET \n");
            p_card_info = (cas_card_info_t *)param;

             if (p_priv->card_status != SMC_CARD_INSERT)
            {
                YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d, CAS_IOCMD_CARD_INFO_GET!\n", __FUNCTION__, __LINE__);
                return CAS_E_ILLEGAL_CARD;
            }

            memset(p_card_info, 0, sizeof(cas_card_info_t));

             /*get the card sn*/
            ret = CDCASTB_GetCardSN((char *)p_card_info->sn);
            p_card_info->sn[CDCA_MAXLEN_SN + 1] = 0;
            YXSB_ADT_DEBUG("[YXSB]Card SN: %s\n", p_card_info->sn);

            /*get lib ver*/
            lib_ver = CDCASTB_GetVer();
            YXSB_ADT_DEBUG("[YXSB]Lib Ver: %d\n", lib_ver);
            memcpy(p_card_info->cas_ver, &lib_ver, 4);
            p_card_info->cas_ver[4] = 0;

            #if 0
            /*get work time*/
            ret = CDCASTB_GetWorkTime(&p_card_info->work_time.start_hour,
                                                               &p_card_info->work_time.start_minute,
                                                               &p_card_info->work_time.start_second,
                                                               &p_card_info->work_time.end_hour,
                                                               &p_card_info->work_time.end_minute,
                                                               &p_card_info->work_time.end_second);
            
            YXSB_ADT_DEBUG("[YXSB]Work Time: %d:%d:%d-%d:%d:%d\n", 
                                              p_card_info->work_time.start_hour,
                                              p_card_info->work_time.start_minute,
                                              p_card_info->work_time.start_second,
                                              p_card_info->work_time.end_hour,
                                              p_card_info->work_time.end_minute,
                                              p_card_info->work_time.end_second);

            /*get watching level*/
            ret = CDCASTB_GetRating(&p_card_info->cas_rating);
            YXSB_ADT_DEBUG("[YXSB]Watching Level: %d\n", p_card_info->cas_rating);
            #endif
            break;

        case CAS_IOCMD_STB_SERIAL_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_STB_SERIAL_GET \n");
            p_platID = (u16 *)param;
            *p_platID = CDCASTB_GetPlatformID();
            YXSB_ADT_DEBUG("plat ID = 0x%x\n", p_platID);
            break;

        case CAS_IOCMD_FINGER_MSG_GET:
            p_finger_msg = (finger_msg_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_FINGER_MSG_GET \n");
            YXSB_STB_Drv_CaReqLock(&FingerInfo_lock);
            memcpy(p_finger_msg, &FingerInfo, sizeof(finger_msg_t));
            YXSB_ADT_DEBUG("[YXSB] dwFontColor: %d!\n", p_finger_msg->finger_property.m_dwFontARGB);
            YXSB_ADT_DEBUG("[YXSB] dwBackgroundColor: %d!\n", p_finger_msg->finger_property.m_dwBackgroundARGB);
            YXSB_ADT_DEBUG("[YXSB] show_postion_X: %d!\n", p_finger_msg->show_postion_x);
            YXSB_ADT_DEBUG("[YXSB] show_postion_Y: %d!\n", p_finger_msg->show_postion_y);
            YXSB_ADT_DEBUG("[YXSB] bIsVisible: %d!\n", p_finger_msg->bIsVisible);
            YXSB_ADT_DEBUG("[YXSB] byType: %d!\n", p_finger_msg->byType);
            YXSB_ADT_DEBUG("[YXSB] show_front_size: %d!\n", p_finger_msg->show_front_size);
            YXSB_ADT_DEBUG("[YXSB] data: %s!\n", p_finger_msg->data);
            YXSB_STB_Drv_CaReqUnLock(&FingerInfo_lock);
            break;
            
        case CAS_IOCMD_OPERATOR_INFO_GET:
            p_operators_info = (cas_operators_info_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_OPERATOR_INFO_GET \n");
            ret = YXSB_ADT_GetOperatorInfo(&p_operators_info->max_num, p_operators_info->p_operator_info);
            break;
            
        case CAS_IOCMD_PATERNER_STA_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_PATERNER_STA_GET \n");
            p_stb_paired_list = (card_stb_paired_list_t *)param;
            p_stb_paired_list->paired_num = 5;
            p_stb_paired_list->paired_status = CDCASTB_IsPaired(&p_stb_paired_list->paired_num, 
                                                                                                        p_stb_paired_list->STB_list);
            
            if(p_stb_paired_list->paired_status != CDCA_RC_OK)
            {
                YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, p_stb_paired_list->paired_status);
                p_stb_paired_list->paired_status = YXSB_ADT_ParseEvent(p_stb_paired_list->paired_status);
            }
            YXSB_ADT_DEBUG("STB_Num = %d, pbySTBID_List:\n", p_stb_paired_list->paired_num);
            for(uIndex = 0; uIndex < 30; uIndex++)
            {
                if(uIndex % 6 == 0)
                    YXSB_ADT_DEBUG("\n"); 
                
                YXSB_ADT_DEBUG("%2x", p_stb_paired_list->STB_list[uIndex]);
            }
            YXSB_ADT_DEBUG("\n");
            break;
            
        case CAS_IOCMD_OSD_MSG_GET:
            p_osd_msg = (msg_info_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_OSD_MSG_GET \n");
            YXSB_STB_Drv_CaReqLock(&OsdMsg_lock);
            memcpy(p_osd_msg, &OsdMsg, sizeof(msg_info_t));
            YXSB_ADT_DEBUG("szMsg = %s \n", p_osd_msg->data);
            YXSB_STB_Drv_CaReqUnLock(&OsdMsg_lock);
            break;
            
        case CAS_IOCMD_OSD_GET:
            p_osd_msg = (msg_info_t *)param;
            YXSB_ADT_DEBUG("[YXSB] super OSD get \n");
            YXSB_STB_Drv_CaReqLock(&OsdMsg_lock);
            memcpy(p_osd_msg, &OsdMsg, sizeof(msg_info_t));
            
            YXSB_ADT_DEBUG("OSD font size = %d\n", p_osd_msg->display_front_size);
            YXSB_ADT_DEBUG("OSD font color = %d\n", p_osd_msg->font_color);
            YXSB_ADT_DEBUG("OSD bg color = %d\n", p_osd_msg->background_color);
            YXSB_ADT_DEBUG("OSD bg area = %d\n", p_osd_msg->background_area);
            YXSB_ADT_DEBUG("OSD data len = %d\n", p_osd_msg->data_len);
            YXSB_ADT_DEBUG("OSD data = %s\n", p_osd_msg->data);
            
            YXSB_STB_Drv_CaReqUnLock(&OsdMsg_lock);
            break;
            
        case CAS_IOCMD_MAIL_HEADER_GET:
            p_mail_headers = (cas_mail_headers_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MAIL_HEADER_GET \n");
            ret = YXSB_ADT_GetMailHeads(&p_mail_headers->max_num, p_mail_headers->p_mail_head);
            YXSB_ADT_DEBUG("[YXSB] mail num = %d \n", p_mail_headers->max_num);
            break;
            
        case CAS_IOCMD_MAIL_BODY_GET:
            p_mail_body = (cas_mail_body_t *)param;
            YXSB_ADT_DEBUG("[YXSB] mail id = %d \n", p_mail_body->mail_id);
            ret = YXSB_ADT_GetMailContent(p_mail_body->mail_id, p_mail_body);
            break;

        case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
            p_mail_id = (u32*)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MAIL_DELETE_BY_INDEX \n");
            CDCASTB_DelEmail(*p_mail_id);
            YXSB_ADT_DEBUG("[YXSB] Del the email: %d \n", *p_mail_id);
            break;

        case CAS_IOCMD_MAIL_DELETE_ALL:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MAIL_DELETE_ALL \n");
            CDCASTB_DelEmail(0);
            break;
            
        case CAS_IOCMD_WORK_TIME_SET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_WORK_TIME_SET \n");
            p_work_time = (cas_card_work_time_t *)param;
            ret = YXSB_ADT_SetWorkTime(p_work_time);
            break;

        case CAS_IOCMD_WORK_TIME_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_WORK_TIME_GET \n");
            p_work_time = (cas_card_work_time_t *)param;
            
            /*get work time*/
            ret = CDCASTB_GetWorkTime(&p_work_time->start_hour,
                                                               &p_work_time->start_minute,
                                                               &p_work_time->start_second,
                                                               &p_work_time->end_hour,
                                                               &p_work_time->end_minute,
                                                               &p_work_time->end_second);
            
            YXSB_ADT_DEBUG("[YXSB]Work Time: %d:%d:%d-%d:%d:%d, ret = %d\n", 
                                                p_work_time->start_hour,
                                                p_work_time->start_minute,
                                                p_work_time->start_second,
                                                p_work_time->end_hour,
                                                p_work_time->end_minute,
                                                p_work_time->end_second,
                                                ret);
            break;
            
        case CAS_IOCMD_RATING_SET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_RATING_SET \n");
            p_rating_set = (cas_rating_set_t *)param;
            
            ret = YXSB_ADT_SetRating(p_rating_set->pin, p_rating_set->rate);
            break;
            
        case CAS_IOCMD_RATING_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_RATING_GET \n");
            p_rating = (u8 *)param;
            ret = CDCASTB_GetRating(p_rating);
            YXSB_ADT_DEBUG("p_rating = %d\n", *p_rating);
            if(ret != CDCA_RC_OK)
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
            break;
            
        case CAS_IOCMD_PIN_SET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_PIN_SET \n");
            p_pin_modify = (cas_pin_modify_t *)param;
            ret = YXSB_ADT_ChangeSmartCardPIN(p_pin_modify);
            break;
            
        case CAS_IOCMD_ENTITLE_INFO_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_ENTITLE_INFO_GET \n");
            p_entitles_info = (product_entitles_info_t *)param;
            if(p_entitles_info->read_type == 0)
            {
                ret  = YXSB_ADT_GetProductInfo(p_entitles_info->operator_id, 
                                                                       &p_entitles_info->max_num, 
                                                                       p_entitles_info->p_entitle_info);
            }
            else
            {
                ret = YXSB_ADT_GetEntitleIDs(p_entitles_info->operator_id, 
                                                                   &p_entitles_info->max_num, 
                                                                   p_entitles_info->p_entitle_info);
            }
            break;
            
        case CAS_IOCMD_READ_FEED_DATA:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_READ_FEED_DATA \n");
            p_child_satus = (chlid_card_status_info *)param;
            ret = CDCASTB_ReadFeedDataFromParent(p_child_satus->operator_id,
                                                                                    p_child_satus->feed_data,
                                                                                    &p_child_satus->length);
            if(CDCA_RC_OK!= ret)
            {
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
                return YXSB_ADT_ParseEvent(ret);
            }

            YXSB_ADT_DEBUG("operator_id = %d, data_len = %d\n", p_child_satus->operator_id, p_child_satus->length);
            YXSB_ADT_DEBUG("feed data: \n");
            for(uIndex = 0; uIndex < 256; uIndex++)
                YXSB_ADT_DEBUG("%d", p_child_satus->feed_data[uIndex]);
            YXSB_ADT_DEBUG("data end\n");

            break;

        case CAS_IOCMD_MON_CHILD_STATUS_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MON_CHILD_STATUS_GET \n");
            /*get feed data from month card*/
            p_child_satus = (chlid_card_status_info *)param;
            ret = YXSB_ADT_GetOperatorChildStatus(p_child_satus->operator_id,  p_child_satus);
            break;

        case CAS_IOCMD_MON_CHILD_STATUS_SET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MON_CHILD_STATUS_SET \n");
            /*feed child card*/
            p_child_satus = (chlid_card_status_info *)param;
            
            YXSB_ADT_DEBUG("OperatorID = %d, data_len = %d \n", p_child_satus->operator_id, p_child_satus->length);
            YXSB_ADT_DEBUG("feed data: \n");
            for(uIndex = 0; uIndex < 256; uIndex++)
                YXSB_ADT_DEBUG("%d", p_child_satus->feed_data[uIndex]);
            YXSB_ADT_DEBUG("data end\n");
            ret = CDCASTB_WriteFeedDataToChild(p_child_satus->operator_id,
                                                                                   p_child_satus->feed_data, 
                                                                                   p_child_satus->length);
            
            if(CDCA_RC_OK != ret)
            {
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
                return YXSB_ADT_ParseEvent(ret);
            }
            YXSB_ADT_DEBUG("operator_id = %d, feed_data = %s\n", p_child_satus->operator_id, p_child_satus->feed_data);
            
            break;
            
        case CAS_IOCMD_IPPV_INFO_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_IPPV_INFO_GET \n");
            p_ippv_info = (ipps_info_t *)param;
            ret = YXSB_ADT_GetIppvProgram(p_ippv_info->operator_id, &p_ippv_info->max_num, p_ippv_info->p_ipp_info);
            break;
            
        case CAS_IOCMD_BURSE_INFO_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_BURSE_INFO_GET \n");
            p_burses_info = (burses_info_t *)param;
            ret = YXSB_ADT_GetBurseInfo(p_burses_info->operator_id, &p_burses_info->max_num, p_burses_info->p_burse_info);
            break;
        case CAS_IOCMD_IPP_BUY_INFO_GET:
            p_purch_info = (ipp_buy_info_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_IPP_BUY_INFO_GET \n");
            YXSB_STB_Drv_CaReqLock(&IppvBuyInfo_lock);
            memcpy(p_purch_info, &IppvBuyInfo, sizeof(ipp_buy_info_t));
            YXSB_STB_Drv_CaReqUnLock(&IppvBuyInfo_lock);
            break;
            
        case CAS_IOCMD_DETITLE_INFO_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_DETITLE_INFO_GET \n");
            p_detitles_info = (product_detitles_info_t *)param;
            if(p_detitles_info->read_type == 0)
            {
                p_detitles_info->read_flag = CDCASTB_GetDetitleReaded(p_detitles_info->operator_id);
                YXSB_ADT_DEBUG("[YXSB] read_flag = %d \n", p_detitles_info->read_flag);
                ret = YXSB_ADT_GetDetitleChkNums(p_detitles_info);
            }
            else
            {
                p_detitles_info->read_flag = CDCASTB_GetDetitleReaded(p_detitles_info->operator_id);
            }
            break;

        case CAS_IOCMD_REQUEST_MASK_BUFFER:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_REQUEST_MASK_BUFFER \n");
            CDCASTB_RequestMaskBuffer();
            break;
            
        case CAS_IOCMD_REQUEST_UPDATE_BUFFER:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_REQUEST_UPDATE_BUFFER \n");
            CDCASTB_RequestUpdateBuffer();
            break;

        case CAS_IOCMD_ACLIST_GET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_ACLIST_GET \n");
            p_ACArray = (u32 *)param;
            p_wTVSID = (u16)p_ACArray[0];
            YXSB_ADT_DEBUG("p_wTVSID = %d\n", p_wTVSID);
            ret = CDCASTB_GetACList(p_wTVSID, p_ACArray);
            if(ret != CDCA_RC_OK)
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
                
            YXSB_ADT_DEBUG("AC_List:\n");
            
            for(uIndex = 0; uIndex < 18; uIndex++)
            {
                YXSB_ADT_DEBUG("%d ", p_ACArray[uIndex]);
            }
            YXSB_ADT_DEBUG("\n");
            break;
            
        case CAS_IOCMD_DETITLE_INFO_DEL:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_DETITLE_INFO_DEL \n");
            p_detitles_info = (product_detitles_info_t *)param;
            ret = CDCASTB_DelDetitleChkNum(p_detitles_info->operator_id, p_detitles_info->dwDetitleChkNum);
            if(ret == CDCA_FALSE)
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
            break;
            
        case CAS_IOCMD_PURCHASE_SET:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_PURCHASE_SET \n");
            p_purch_info = (ipp_buy_info_t *)param;
            ret  = YXSB_ADT_IppBuySet(p_purch_info);
            break;
            
        case CAS_IOCMD_MAIL_SPACE_STATUS:
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_MAIL_SPACE_STATUS \n");
            p_mail_headers = (cas_mail_headers_t *)param;
            ret = CDCASTB_GetEmailSpaceInfo(&pbyEmailNum, &pbyEmptyNum);
            if(CDCA_RC_OK != ret)
            {
                YXSB_ADT_DEBUG("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
                return YXSB_ADT_ParseEvent(ret);
            }
            p_mail_headers->max_num = pbyEmailNum;
            p_mail_headers->new_mail_num = pbyEmptyNum;
            YXSB_ADT_DEBUG("pbyEmailNum = %d, pbyEmptyNum = %d\n",
                                                p_mail_headers->max_num,
                                                p_mail_headers->new_mail_num);
            break;
            
        case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
            p_force_channel= (cas_force_channel_t *)param;
            YXSB_ADT_DEBUG("[YXSB] CAS_IOCMD_FORCE_CHANNEL_INFO_GET \n");
            YXSB_STB_Drv_CaReqLock(&ForceChannelInfo_lock);
            memcpy(p_force_channel, &ForceChannelInfo, sizeof(cas_force_channel_t));
            YXSB_STB_Drv_CaReqUnLock(&ForceChannelInfo_lock);
            break;
            
        default:
            break;
    }
    return YXSB_ADT_ParseEvent(ret);
}

static RET_CODE cas_yxsb_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{
    //cas_pmt_t pmt = {0};
    cas_cat_t cat = {0};
    u8 idx;
    SCDCASServiceInfo pServiceInfo = {0,};
    s_cas_yxsb_priv_t *p_priv = NULL;

    YXSB_ADT_DEBUG("[YXSB]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    p_priv = (s_cas_yxsb_priv_t *)p_g_cas_priv->cam_op[CAS_ID_TF].p_priv;


    if (CAS_TID_PMT == t_id)
    {
        OS_PRINTF("[YXSB]pmt process\n");

        YXSB_ADT_ParsePMT(p_buf, p_priv);
        OS_PRINTF("ecm_pid = 0x%x, audio_ecm_pid = 0x%x, video_ecm_pid = 0x%x, pro_id = %d\n",
                                p_priv->cas_desc.ca_pid,
                                p_priv->audio_desc.a_cas_desc.ca_pid,
                                p_priv->vedio_desc.v_cas_desc.ca_pid,
                                p_priv->program_id);


        CDCASTB_SetEcmPid(CDCA_LIST_FIRST, NULL);

        if(p_priv->cas_desc.ca_pid)
        {
            //if(CDCASTB_IsCDCa(p_priv->cas_desc.ca_system_id) == CDCA_TRUE)
            //{
                memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                
                pServiceInfo.m_wEcmPid = p_priv->cas_desc.ca_pid;
                pServiceInfo.m_byServiceNum = 1;
                pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
            //}
        }
        else
        {
            if(p_priv->audio_desc.a_cas_desc.ca_pid)
            {
                //if(CDCASTB_IsCDCa(p_priv->audio_desc.a_cas_desc.ca_system_id) == CDCA_TRUE)
                //{
                    memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                    
                    pServiceInfo.m_wEcmPid = p_priv->audio_desc.a_cas_desc.ca_pid;
                    pServiceInfo.m_byServiceNum = 1;
                    pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                    CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
                //}
            }
            
            if(p_priv->vedio_desc.v_cas_desc.ca_pid)
            {
               // if(CDCASTB_IsCDCa(p_priv->vedio_desc.v_cas_desc.ca_system_id) == CDCA_TRUE)
                //{
                    memset(&pServiceInfo, 0x00, sizeof(SCDCASServiceInfo));
                    
                    pServiceInfo.m_wEcmPid = p_priv->vedio_desc.v_cas_desc.ca_pid;
                    pServiceInfo.m_byServiceNum = 1;
                    pServiceInfo.m_wServiceID[0] = p_priv->program_id;
                    CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
                //}
            }
        }
        

        CDCASTB_SetEcmPid(CDCA_LIST_OK, NULL);
#if 0        
        cas_parse_pmt(p_buf, &pmt);

        CDCASTB_SetEcmPid(CDCA_LIST_FIRST, NULL);

        for(idx = 0; idx < pmt.ecm_cnt; idx++)
        {
            OS_PRINTF("## ca_sys_id :0x%x, PMT\n", pmt.cas_descr[idx].ca_sys_id);
            if(CDCASTB_IsCDCa(pmt.cas_descr[idx].ca_sys_id) != CDCA_TRUE)
                continue;
            pServiceInfo.m_wEcmPid = pmt.cas_descr[idx].ecm_pid;
            pServiceInfo.m_byServiceNum = 1;
            pServiceInfo.m_wServiceID[0] = pmt.prog_num;
            CDCASTB_SetEcmPid(CDCA_LIST_ADD, &pServiceInfo);
            break;
        }
        
        CDCASTB_SetEcmPid(CDCA_LIST_OK, NULL);
#endif
    }
    else if (CAS_TID_CAT == t_id)
    {
        OS_PRINTF("[YXSB]cat process\n");
        cas_parse_cat(p_buf, &cat);

        for(idx = 0; idx < cat.emm_cnt; idx++)
        {
            OS_PRINTF("## ca_sys_id :0x%x, CAT\n", cat.ca_desc[idx].ca_sys_id);
            if(CDCASTB_IsCDCa(cat.ca_desc[idx].ca_sys_id) != CDCA_TRUE)
                continue;

            CDCASTB_SetEmmPid(cat.ca_desc[idx].emm_pid);
            break;
        }
    }
    else if (CAS_TID_NIT == t_id)
    {
        OS_PRINTF("[YXSB]nit process\n");
        p_priv->network_id = (u32)((((u16)(p_buf[3])) << 8) |(p_buf[4]));
    }

    return SUCCESS;

}

RET_CODE cas_yxsb_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
    s_cas_yxsb_priv_t *p_priv = NULL;

    p_g_cas_priv = cas_get_private_data();
    
    p_g_cas_priv->cam_op[CAS_ID_TF].attached = 1;
    p_g_cas_priv->cam_op[CAS_ID_TF].card_reset_by_cam = 1;
    p_g_cas_priv->cam_op[CAS_ID_TF].inited = 0;

    p_g_cas_priv->cam_op[CAS_ID_TF].func.init = cas_yxsb_init;
    p_g_cas_priv->cam_op[CAS_ID_TF].func.deinit = cas_yxsb_deinit;
    p_g_cas_priv->cam_op[CAS_ID_TF].func.identify = cas_yxsb_identify;
    p_g_cas_priv->cam_op[CAS_ID_TF].func.io_ctrl = cas_yxsb_io_ctrl;
    p_g_cas_priv->cam_op[CAS_ID_TF].func.table_process = cas_yxsb_table_process;

    p_g_cas_priv->cam_op[CAS_ID_TF].func.card_remove = cas_yxsb_card_remove;
    p_g_cas_priv->cam_op[CAS_ID_TF].func.card_reset = cas_yxsb_card_reset;

    p_priv = mtos_malloc(sizeof(s_cas_yxsb_priv_t));
    p_g_cas_priv->cam_op[CAS_ID_TF].p_priv = (s_cas_yxsb_priv_t *)p_priv;
    memset(p_priv, 0x00, sizeof(s_cas_yxsb_priv_t));
 
    //p_priv->flash_size = p_cfg->flash_size;
    //p_priv->flash_start_add = p_cfg->flash_start_adr;
    //YXSB_ADT_DEBUG("[YXSB]flash  %x %x\n", p_priv->flash_size, p_priv->flash_start_add);
    // p_priv->p_dmx_dev = p_cfg->p_dmx_dev;
    #if 1
    if(p_cfg->nvram_read)
        p_priv->nvram_read = p_cfg->nvram_read;
    else
        p_priv->nvram_read = NULL;

    if(p_cfg->nvram_write)
        p_priv->nvram_write = p_cfg->nvram_write;
    else
        p_priv->nvram_write = NULL;
    #else
    p_priv->nvram_read = NULL;
    p_priv->nvram_write = NULL;
    #endif

    if(p_cfg->machine_serial_get != NULL)
    {
        YXSB_ADT_DEBUG("[YXSB]machine_serial_get ok\n");
        p_priv->machine_serial_get = p_cfg->machine_serial_get;
    }
    
    p_priv->task_prio = p_cfg->task_prio;

    *p_cam_id = (u32)(&p_g_cas_priv->cam_op[CAS_ID_TF]);

    YXSB_ADT_DEBUG("[YXSB]cas_yxsb_attach ok\n");
    return SUCCESS;
}


