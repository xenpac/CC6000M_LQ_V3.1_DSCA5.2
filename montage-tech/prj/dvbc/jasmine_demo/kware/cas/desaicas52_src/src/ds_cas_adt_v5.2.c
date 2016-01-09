/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_event.h"

#include "lib_bitops.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"
#include "nim.h"

#include "desai_ca5.2_api.h"
#include "ds_cas_include_v5.2.h"

#include "cas_porting_drv_demux.h"
#include "cas_porting_drv_smc.h"
#include "cas_porting_drv_os.h"
#include "cas_porting_drv_base.h"

//#define CAS_DS_PRINTF cas_porting_adt_printf
//#define CAS_DS_ERR OS_PRINTF
#define CAS_DS_DEBUG
#define CAS_DS_ERROR

#ifdef CAS_DS_DEBUG
#define CAS_DS_PRINTF OS_PRINTF
#else
#define CAS_DS_PRINTF DUMMY_PRINTF
#endif

#ifdef CAS_DS_ERROR
#define CAS_DS_ERR OS_PRINTF
#else
#define CAS_DS_ERR DUMMY_PRINTF
#endif



UINT16 ds_ecm_pid = 0;
UINT16 ds_service_id = 0;
e_ds_smartcard_status_t s_ds_uScStatus = SC_STATUS_END;
scard_device_t *p_ds_scard_dev = NULL;


extern os_sem_t ca_osd_lock;
extern os_sem_t ca_finger_lock;
//extern cas_adapter_priv_t g_cas_priv;
extern finger_msg_t ds_msgFinger;
extern msg_info_t ds_OsdMsg;
extern u8 ds_watch_level;
extern u8 ds_msg_show_type;
extern ST_IPPV_INFO ds_ippv;
extern ST_CA_EXIGENT_PROGRAM ds_Exigent;

static cas_adapter_priv_t *g_cas_priv;


extern RET_CODE DS_STB_Drv_AdtInit(void);
extern RET_CODE DS_STB_Drv_FreeAllEcm(void);
extern void DS_STB_Drv_CaReqLock(os_sem_t * pSemaphore);
extern void DS_STB_Drv_CaReqUnLock(os_sem_t * pSemaphore);

static void ds_get_ipps_info(ipps_info_t *p_ippv_info)
{
    u8 i = 0;
    memcpy(p_ippv_info->p_ipp_info[0].product_name, ds_ippv.aucIppvName, sizeof(ds_ippv.aucIppvName));
    memcpy(p_ippv_info->p_ipp_info[0].event_id, ds_ippv.aucEventID, 4);
    p_ippv_info->p_ipp_info[0].operator_id = ds_ippv.ucProviderID;
    p_ippv_info->p_ipp_info[0].price = ds_ippv.usPrice;

    p_ippv_info->p_ipp_info[0].start_time[0] = ds_ippv.stBeginTime.usYear >> 8;
    p_ippv_info->p_ipp_info[0].start_time[1] = ds_ippv.stBeginTime.usYear & 0x00ff;
    p_ippv_info->p_ipp_info[0].start_time[2] = ds_ippv.stBeginTime.ucMonth;
    p_ippv_info->p_ipp_info[0].start_time[3] = ds_ippv.stBeginTime.ucDay;
    p_ippv_info->p_ipp_info[0].start_time[4] = ds_ippv.stBeginTime.ucHour;
    p_ippv_info->p_ipp_info[0].start_time[5] = ds_ippv.stBeginTime.ucMinute;
    p_ippv_info->p_ipp_info[0].start_time[6] = ds_ippv.stBeginTime.ucSecond;

    p_ippv_info->p_ipp_info[0].expired_time[0] = ds_ippv.stEndTime.usYear >> 8;
    p_ippv_info->p_ipp_info[0].expired_time[1] = ds_ippv.stEndTime.usYear & 0x00ff;
    p_ippv_info->p_ipp_info[0].expired_time[2] = ds_ippv.stEndTime.ucMonth;
    p_ippv_info->p_ipp_info[0].expired_time[3] = ds_ippv.stEndTime.ucDay;
    p_ippv_info->p_ipp_info[0].expired_time[4] = ds_ippv.stEndTime.ucHour;
    p_ippv_info->p_ipp_info[0].expired_time[5] = ds_ippv.stEndTime.ucMinute;
    p_ippv_info->p_ipp_info[0].expired_time[6] = ds_ippv.stEndTime.ucSecond;
    CAS_DS_PRINTF("[DS]product nuame : %s! \n", p_ippv_info->p_ipp_info[0].product_name);
    CAS_DS_PRINTF("[DS]event id : 0x%x-%x-%x-%x! \n", p_ippv_info->p_ipp_info[0].event_id[0], p_ippv_info->p_ipp_info[0].event_id[1], p_ippv_info->p_ipp_info[0].event_id[2], p_ippv_info->p_ipp_info[0].event_id[3]);
    CAS_DS_PRINTF("[DS]provide id : %d! \n", p_ippv_info->p_ipp_info[0].operator_id);
    CAS_DS_PRINTF("[DS]ippv price : %d! \n", p_ippv_info->p_ipp_info[0].price);
    CAS_DS_PRINTF("[DS]ippv start time :");
    for(i = 0; i < 7; i ++)
    {
        CAS_DS_PRINTF("%d-", p_ippv_info->p_ipp_info[0].start_time[i]);
    }
    CAS_DS_PRINTF("\n");
    CAS_DS_PRINTF("[DS]ippv end time :");
    for(i = 0; i < 7; i ++)
    {
        CAS_DS_PRINTF("%d-", p_ippv_info->p_ipp_info[0].expired_time[i]);
    }
    CAS_DS_PRINTF("\n");
}

static RET_CODE ds_get_product_info(product_entitles_info_t *p_entitles_info)
{
    EN_ERR_CAS ret = CA_ERROR_OK;
    u16 index = 0;
    u8 i = 0;
    ST_PRODUCT_ENTITLE  *p_pstProductEntitles = NULL;

    p_pstProductEntitles = (ST_PRODUCT_ENTITLE  *)cas_porting_drv_base_malloc(p_entitles_info->max_num * sizeof(ST_PRODUCT_ENTITLE));

    p_entitles_info->read_num = p_entitles_info->max_num;

    ret = DSCA_GetProductInfo(p_entitles_info->operator_id, p_entitles_info->start_pos,
                                               p_entitles_info->read_num, p_pstProductEntitles);
    if(ret != CA_ERROR_OK)
    {
        CAS_DS_ERR("[DS]get product info error : %d! \n", ret);
        cas_porting_drv_base_free(p_pstProductEntitles);
        return ERR_FAILURE;
    }
    CAS_DS_PRINTF("[DS]operator_id : %d! \n", p_entitles_info->operator_id);
    CAS_DS_PRINTF("[DS]product start_pos : %d! \n", p_entitles_info->start_pos);
    CAS_DS_PRINTF("[DS]product read_num : %d! \n", p_entitles_info->read_num);

    for(index = 0; index < p_entitles_info->read_num; index++)
    {
        p_entitles_info->p_entitle_info[index].states = p_pstProductEntitles[index].ucEnableType;
        p_entitles_info->p_entitle_info[index].reserved = p_pstProductEntitles[index].ucRecoadFlag;
        p_entitles_info->p_entitle_info[index].product_id = p_pstProductEntitles[index].usProductID;

        if(p_entitles_info->p_entitle_info[index].states == 2)
        {
            p_entitles_info->p_entitle_info[index].start_time[0]= p_pstProductEntitles[index].stBeginTime.usYear >> 8;
            p_entitles_info->p_entitle_info[index].start_time[1]= p_pstProductEntitles[index].stBeginTime.usYear & 0x00ff;
            p_entitles_info->p_entitle_info[index].start_time[2]= p_pstProductEntitles[index].stBeginTime.ucMonth;
            p_entitles_info->p_entitle_info[index].start_time[3]= p_pstProductEntitles[index].stBeginTime.ucDay;
            p_entitles_info->p_entitle_info[index].start_time[4]= p_pstProductEntitles[index].stBeginTime.ucHour;
            p_entitles_info->p_entitle_info[index].start_time[5]= p_pstProductEntitles[index].stBeginTime.ucMinute;
            p_entitles_info->p_entitle_info[index].start_time[6]= p_pstProductEntitles[index].stBeginTime.ucSecond;

            p_entitles_info->p_entitle_info[index].expired_time[0]= p_pstProductEntitles[index].stEndTime.usYear >> 8;
            p_entitles_info->p_entitle_info[index].expired_time[1]= p_pstProductEntitles[index].stEndTime.usYear & 0x00ff;
            p_entitles_info->p_entitle_info[index].expired_time[2]= p_pstProductEntitles[index].stEndTime.ucMonth;
            p_entitles_info->p_entitle_info[index].expired_time[3]= p_pstProductEntitles[index].stEndTime.ucDay;
            p_entitles_info->p_entitle_info[index].expired_time[4]= p_pstProductEntitles[index].stEndTime.ucHour;
            p_entitles_info->p_entitle_info[index].expired_time[5]= p_pstProductEntitles[index].stEndTime.ucMinute;
            p_entitles_info->p_entitle_info[index].expired_time[6]= p_pstProductEntitles[index].stEndTime.ucSecond;
        }
        CAS_DS_PRINTF("[DS]product num : %d! \n", index);
        CAS_DS_PRINTF("[DS]product id : %d! \n", p_entitles_info->p_entitle_info[index].product_id);
        CAS_DS_PRINTF("[DS]product states : %d! \n", p_entitles_info->p_entitle_info[index].states);
        CAS_DS_PRINTF("[DS]product start time :");
        for(i = 0; i < 7; i ++)
        {
            CAS_DS_PRINTF("%d-", p_entitles_info->p_entitle_info[index].start_time[i]);
        }
        CAS_DS_PRINTF("\n");
        CAS_DS_PRINTF("[DS]product end time :");
        for(i = 0; i < 7; i ++)
        {
            CAS_DS_PRINTF("%d-", p_entitles_info->p_entitle_info[index].expired_time[i]);
        }
        CAS_DS_PRINTF("\n");

    }
    cas_porting_drv_base_free(p_pstProductEntitles);
    return SUCCESS;
}

static RET_CODE ds_get_operator_info(cas_operators_info_t *p_operators_info)
{
    EN_ERR_CAS ret = CA_ERROR_OK;
    ST_OPERATOR_INFO pstOperatorInfo = {{0,},};
    //get operator id
    ret = DSCA_GetOperatorID((UINT8 *) p_operators_info->p_operator_info[0].operator_id_ascii);
    if(ret != CA_ERROR_OK)
    {
        CAS_DS_ERR("[DS]get opreator id error : %d! \n", ret);
        return ERR_FAILURE;
    }
    CAS_DS_PRINTF("[DS]get opreator id : %s \n", p_operators_info->p_operator_info[0].operator_id_ascii);
    //get operator info
    ret = DSCA_GetOperatorInfo(&pstOperatorInfo);
    if(ret != CA_ERROR_OK)
    {
        CAS_DS_ERR("[DS]get opreator info error : %d! \n", ret);
        return ERR_FAILURE;
     }
    memcpy(p_operators_info->p_operator_info[0].operator_name,  pstOperatorInfo.aucOperatorName, 40);
    memcpy(p_operators_info->p_operator_info[0].operator_phone,  pstOperatorInfo.aucOperatorPhone, 40);
    memcpy(p_operators_info->p_operator_info[0].operator_address, pstOperatorInfo.aucOperatorAddress, 80);
    CAS_DS_PRINTF("[DS]get opreator name: %s!",p_operators_info->p_operator_info[0].operator_name);
    CAS_DS_PRINTF("[DS]get opreator phone: %s!",p_operators_info->p_operator_info[0].operator_phone);
    CAS_DS_PRINTF("[DS]get opreator addr: %s!",p_operators_info->p_operator_info[0].operator_address);

    return SUCCESS;
}


static s32 ds_ca_desc_parse(u8 *pBuf, s32 nDescLen, ds_ca_desc_t *pDesc)
{
    u8 *p_data = pBuf;

    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    if((NULL != pBuf) && (NULL != pDesc))
    {
        pDesc->ca_system_id = (p_data[0] << 8) | p_data[1];
        pDesc->ca_pid = ((p_data[2]&0x1F) << 8) | p_data[3];
        CAS_DS_PRINTF("[DS]%s:LINE:%d, [ca] ca_system_id = 0x%x ca_pid=%d\n", __FUNCTION__, __LINE__,
                    pDesc->ca_system_id, pDesc->ca_pid);

        return SUCCESS;
    }
    return ERR_FAILURE;
}


static void ds_pmt_parse_descriptor(u8 *buf, s16 length,u16 program_number)
{
    u8 *p_data = buf;
    u8 descriptor_tag = 0;
    u8 descriptor_length = 0;

    ds_ca_desc_t ca_desc = {0};

    while (length > 0)
    {
        descriptor_tag = p_data[0];
        descriptor_length = p_data[1];
        CAS_DS_PRINTF("[DS]%s:LINE:%d,descriptor_tag=0x%x\n", __FUNCTION__, __LINE__, descriptor_tag);

        if(0 == descriptor_length)
        {
            break;
        }

        length -= 2;
        p_data += 2;
        switch (descriptor_tag)
        {
            case 0x09:  /* DVB_DESC_CA */
            {
                ds_ca_desc_parse(p_data, descriptor_length, &ca_desc);

                CAS_DS_PRINTF("[DS]%s:LINE:%d,[PMT] ecm_pid = 0x%x, system_id=0x%x\n", __FUNCTION__, __LINE__,
                       ca_desc.ca_pid, ca_desc.ca_system_id);
                if(TRUE == DSCA_IsDSCa((UINT16)(ca_desc.ca_system_id)))
                {
                    if(ds_ecm_pid == ca_desc.ca_pid)
                    {
                        return;
                    }
                    ds_ecm_pid = (UINT16)(int)(ca_desc.ca_pid);

                    CAS_DS_PRINTF("[DS]DSCA_SetEcmPid: %d!\n", ds_ecm_pid);
                    DSCA_SetEcmPid(ds_ecm_pid);
                }
                else
                {
                    //   DSCA_SetEcmPid(0x1FFF);
                    CAS_DS_PRINTF("[DS]%s:LINE:%d, not DESC CA!\n", __FUNCTION__, __LINE__);
                }
            }
                break;
            default:
              //DSCA_SetEcmPid(0x1FFF);
                break;
        }
        length -= descriptor_length;
        p_data = p_data + descriptor_length;
    }
}


static void ds_cas_parse_pmt(UINT8 *p_buf)
{
    //u8 table_id = 0;
    s16 section_length = 0;
    u16 program_number = 0;
    //u8 version_number = 0;
    //u16 pcr_pid = 0;
    u16 program_info_length = 0;
    u8 stream_type = 0;
    u16 elementary_pid = 0;
    u8 es_info_length = 0;
    u8 *p_data = NULL;

    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    MT_ASSERT(p_buf != NULL);

    p_data = p_buf;

    if(NULL != p_data)
    {
        //table_id = p_data[0];
        section_length = (((U16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
        if(section_length > 0)
        {
            program_number = (U16)p_data[3] << 8 | p_data[4];
            //version_number = (p_data[5] & 0x3E) >> 1;
            //pcr_pid = ((U16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

            program_info_length = ((U16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
            p_data += 12;
            //  GUOTONG_SetEcmPID(CA_INFO_CLEAR, NULL);
            ds_pmt_parse_descriptor(p_data, (s16)program_info_length,program_number);
            p_data += program_info_length;
            section_length -= (s16)(13 + program_info_length);//head and crc
            CAS_DS_PRINTF("[DS]stream_type=0x%x \n",p_data[0]);

            while(section_length > 0)
            {
                stream_type = p_data[0];
                elementary_pid = ((U16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
                OS_PRINTF(" stream_type=0x%x, elementary_PID=0x%x\n", stream_type, elementary_pid);
                es_info_length = (u8)(((U16)p_data[3] << 8 | p_data[4]) & 0x0FFF);
                p_data += 5;
                ds_pmt_parse_descriptor(p_data, es_info_length,program_number);
                p_data += es_info_length;
                section_length -= (es_info_length + 5);
            }
        }
    }
}


static void ds_cat_parse_descriptor(u8 *buf, s16 length)
{
    u8 *p_data = buf;
    u8  descriptor_tag = 0;
    u8  descriptor_length = 0;
    u16 ca_system_id = 0;
    u16 emm_pid = 0;

    CAS_DS_PRINTF("%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    while (length > 0)
    {
        descriptor_tag = p_data[0];
        descriptor_length = p_data[1];
        p_data += 2;
        length -= 2;
        if (descriptor_length == 0)
        {
            continue;
        }
        switch (descriptor_tag)
        {
            case 0x09:  /* DVB_DESC_CA */
            {
                ca_system_id = MAKE_WORD(p_data[1], p_data[0]);
                emm_pid = MAKE_WORD(p_data[3],p_data[2]&0x1F);
                if (1 == DSCA_IsDSCa((UINT16)ca_system_id))
                {
                    CAS_DS_PRINTF("[DS EMM] emm pid =0x%x,Desc.ca_system_id =0x%x\n", emm_pid, ca_system_id);
                    DSCA_SetEmmPid((UINT16)emm_pid);
                }
                else
                {
                    CAS_DS_ERR("[DS] NOT DESAI CA_ID!!!! \n");
                }
            }
                break;
            default:
                break;
        }
        length -= descriptor_length;
        p_data = p_data + descriptor_length;
    }
}


static void ds_cas_parse_cat(u8 *p_buf)
{
    u8 *p_data = p_buf;
    s16 length = 0;

#ifdef CAS_DS_DEBUG
    u8 version_number = 0;
    version_number = (u8)((p_data[5] & 0x3E) >> 1);
    CAS_DS_PRINTF("[DS]%s:LINE:%d, [CAT] version_number=%d\n",
            __FUNCTION__, __LINE__,  version_number);
#endif
    length = (s16)(((p_data[1] & 0x0f) << 8) | p_data[2]);
    CAS_DS_PRINTF("[DS]%s:LINE:%d, [CAT] length=%d\n",
            __FUNCTION__, __LINE__, length );

    p_data += 8;
    ds_cat_parse_descriptor(p_data, length - 9);
    
}

static void ds_cas_parse_nit(u8 *p_buf, u32 uiLen)
{
    u8 p_data[7] = {0};
    u8 descriptor_tag = 0;
    u8 descriptor_length = 0;
    u32 uiLoop =  uiLen;
    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    while (uiLoop > 0)
    {
        descriptor_tag = p_buf[0];
        descriptor_length = p_buf[1];
        p_buf += 1;
        uiLoop -= 1;
        if (descriptor_tag == 0xa5 && descriptor_length == 7)
        {
            memcpy(p_data,  & p_buf [1],  descriptor_length);
            CAS_DS_PRINTF("[DS]ÇøÓòÉèÖÃ:\n");
            DSCA_SetCardAddressCode(p_data);
            CAS_DS_PRINTF("[DS]\r\n %s:LINE:%d, parsing nit over.\n", __FUNCTION__, __LINE__);
            break;
        }
    }
}

#ifdef LCN_SWITCH_DS_JHC
void jhc_ds_cas_parse_nit(u8 *p_buf)
{
  u32 len = MAKE_WORD(p_buf[2], p_buf[1] & 0xF) + 3;
  ds_cas_parse_nit(p_buf, len);
}
#endif

static RET_CODE cas_ds_init()
{
    static u32 init_flag = 0;
    RET_CODE ret = 0;
    EN_ERR_CAS cas_ret = CA_ERROR_OK;
    u8 task_prio = 0;
    s_cas_ds_priv_t *p_cas_ds_priv = NULL;

    CAS_DS_PRINTF("[DS]cas_ds_init \n");

    p_cas_ds_priv = g_cas_priv->cam_op[CAS_ID_DS].p_priv;

    if (0 != p_cas_ds_priv->task_prio)
    {
        task_prio = (u8)(p_cas_ds_priv->task_prio);
    }
    else
    {
        task_prio = DSCA_TASK_PRIO;
    }
    if(!init_flag)
    {
        ret = DS_STB_Drv_AdtInit();
        if (SUCCESS != ret)
        {
            CAS_DS_PRINTF("[DS]DS_STB_Drv_AdtInit return error! ret=%d\n",ret);
            return ERR_FAILURE;
        }

        cas_ret = DSCA_Init(task_prio, CA_LANGUAGE_CHINESE);
        if (cas_ret != CA_ERROR_OK)
        {
            CAS_DS_PRINTF("[DS]DESAI CA STB_Init return error! ret=%d\n",cas_ret);
            return ERR_FAILURE;
        }
	
        CAS_DS_PRINTF("[DS]cas_ds_init success ! \n");
        cas_porting_drv_task_sleep(300);
        init_flag = 1;
    }
    return SUCCESS;
}

static RET_CODE cas_ds_deinit()
{
    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    return SUCCESS;
}

static RET_CODE cas_ds_identify(UINT16 ca_sys_id)
{
    RET_CODE ret = 0;

    ret = DSCA_IsDSCa(ca_sys_id);
    if(ret != TRUE)
    {
            CAS_DS_ERR("[DS]Not DeSai CA card ! \n");
            return ERR_FAILURE;
    }
    return SUCCESS;
}

static RET_CODE cas_ds_io_ctrl(u32 cmd, void *param)
{
    s_cas_ds_priv_t *p_priv = (s_cas_ds_priv_t *)g_cas_priv->cam_op[CAS_ID_DS].p_priv;
    ST_WORKTIME_INFO pstWorkTime = {{0,},};
    ST_MAILLIST_INFO pstEmailInfoList = {{{0,},},0};
 //   UINT8 nit_match[13] = {NIT_TABLE_ID,0,0,0,0,0,0,0,0,0,0,0,0};
//    UINT8 nit_mask[13] =  {0xff,0,0,0,0,0,0,0,0,0,0,0,0};

    cas_card_info_t *p_card_info = NULL;
    cas_table_info_t *p_cas_table_info = NULL;
    cas_card_work_time_t *p_work_time = NULL;
    cas_rating_set_t *p_rating_set = NULL;
    product_entitles_info_t *p_entitles_info = NULL;
    cas_pin_modify_t *p_pin_modify = NULL;
    burses_info_t *p_burses_info = NULL;
    cas_operators_info_t *p_operators_info = NULL;
    cas_mail_headers_t *p_mail_headers = NULL;
    cas_mail_body_t *p_mail_body = NULL;
    msg_info_t *p_osd_msg = NULL;
    finger_msg_t *p_finger_msg = NULL;
    cas_force_channel_t *p_exigent = NULL;
    ipps_info_t *p_ippv_info = NULL;
//    nim_channel_info_t *p_nim_info = NULL;
//    nim_device_t *p_nim_dev = NULL;
    u32 *p_mail_id = NULL;
    u32 *p_sys_language = NULL;
    u8 *p_msg_show_type = NULL;
    u8 *p_nit_zone = NULL;
    u32 max_num_p = 0;
	
//lint -e64, -e545
    EN_ERR_CAS ret = 0;
    u8 index = 0;

    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in, cmd =%d!\n", __FUNCTION__, __LINE__, cmd);

    switch(cmd)
    {
        case CAS_IOCMD_STOP:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_STOP \n");
            DS_STB_Drv_FreeAllEcm();
            ret = SUCCESS;
            break;

        case CAS_IOCMD_VIDEO_PID_SET:
            CAS_DS_PRINTF("[DS]v_pid=0x%x \n",p_priv->v_pid);
            p_priv->v_pid = *((UINT16 *)param);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_AUDIO_PID_SET:
            CAS_DS_PRINTF("[DS]a_pid=0x%x \n",p_priv->a_pid);
            p_priv->a_pid = *((UINT16 *)param);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_SET_ECMEMM:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_SET_ECMEMM \n");
            p_cas_table_info = (cas_table_info_t *)param;

            for(index = 0; index < CAS_MAX_ECMEMM_NUM; index++)
            {
                if(1 == DSCA_IsDSCa(p_cas_table_info->ecm_info[index].ca_system_id))
                {
                    ds_ecm_pid = p_cas_table_info->ecm_info[index].ecm_id;
                    ds_service_id = p_cas_table_info->service_id;
                    DSCA_SetEcmPid(ds_ecm_pid);
                } 
                else
                {
                    if(p_cas_table_info->ecm_info[index].ecm_id == 0x1FFF)
                    {
                        ds_ecm_pid=0x1FFF;
                        ds_service_id = p_cas_table_info->service_id;
                        DSCA_SetEcmPid(0x1FFF);
                    }
                }
            }
            ret = SUCCESS;
            break;

        case CAS_IOCMD_CARD_INFO_GET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_CARD_INFO_GET \n");
            p_card_info = (cas_card_info_t *)param;

            if (p_priv->card_status != SMC_CARD_INSERT)
            {
                CAS_DS_PRINTF("[DS]%s:LINE:%d, CAS_IOCMD_CARD_INFO_GET!\n", __FUNCTION__, __LINE__);
                return ERR_FAILURE;
            }

            memset(p_card_info, 0, sizeof(cas_card_info_t));

            //get operator id
            ret = DSCA_GetOperatorID((UINT8 *) p_card_info->cas_id);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get opreator id error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get opreator id  : %s! \n", p_card_info->cas_id);


            //get card id
            ret = DSCA_GetCardNo((UINT8 *)p_card_info->card_id, (UINT8 *)&p_card_info->card_type);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get card id error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get card id  : %s! \n", p_card_info->card_id);
            CAS_DS_PRINTF("[DS]Card Type: %d \n",p_card_info->card_type);

            //get card version
            ret = DSCA_GetCAVer((UINT8 *)p_card_info->cas_ver);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get card version error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get card version : %s! \n", p_card_info->cas_ver);


            //get card region
            ret = DSCA_GetCardRegion((UINT8 *)p_card_info->area_code);
            if(ret != CA_ERROR_OK)
            {
                // p_card_info->area_len = 32;
                CAS_DS_ERR("[DS]get card region error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get card region: %s! \n", p_card_info->area_code);

            //get card state
            ret = DSCA_GetCardState();
            p_card_info->card_state = ret;
            CAS_DS_PRINTF("[DS]get card state: %d! \n", ret);

            //get work time
            ret = DSCA_GetWorkTime(&pstWorkTime);
            if(ret != CA_CARD_SUCCESS)
            {
                CAS_DS_ERR("[DS]get card WORKTIME error : %d! \n", ret);
                return ERR_FAILURE;
            }
            p_card_info->work_time.start_hour = pstWorkTime.stBeginTime.ucHour;
            p_card_info->work_time.start_minute = pstWorkTime.stBeginTime.ucMinute;
            p_card_info->work_time.start_second = pstWorkTime.stBeginTime.ucSecond;

            p_card_info->work_time.end_hour = pstWorkTime.stEndTime.ucHour;
            p_card_info->work_time.end_minute = pstWorkTime.stEndTime.ucMinute;
            p_card_info->work_time.end_second = pstWorkTime.stEndTime.ucSecond;

            CAS_DS_PRINTF("[DS]get card WORKTIME START : %d : %d : %d \n",p_card_info->work_time.start_hour, p_card_info->work_time.start_minute, p_card_info->work_time.start_second);
            CAS_DS_PRINTF("[DS]get card WORKTIME END     : %d : %d : %d \n",p_card_info->work_time.end_hour, p_card_info->work_time.end_minute, p_card_info->work_time.end_second);

            // get work level
            ret = DSCA_GetWorkLevel((UINT8 *)&p_card_info->card_work_level);
            if(ret != CA_CARD_SUCCESS)
            {
                CAS_DS_PRINTF("[DS]get card WORK LEVLE error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get card WORK level : %d! \n", p_card_info->card_work_level);

            //get master id
            ret = DSCA_GetMasterCard((UINT8 *)&p_card_info->mother_card_id);
            CAS_DS_PRINTF("[DS]get mother card id  : %s! \n", p_card_info->mother_card_id);
            // get local id
            ret = DSCA_GetLCO((UINT8 *)&p_card_info->cas_loc);
            CAS_DS_PRINTF("[DS]get localcard id  : %s! \n", p_card_info->cas_loc);
            // get group id
            ret = DSCA_GetGroupId((UINT8 *)&p_card_info->cas_group);
            CAS_DS_PRINTF("[DS]get localcard id  : %s! \n", p_card_info->cas_group);
            // get card version
            ret = DSCA_GetCardVersion((UINT8 *)&p_card_info->card_version);
            CAS_DS_PRINTF("[DS]get localcard id  : %s! \n", p_card_info->card_version);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_WORK_TIME_SET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_WORK_TIME_SET \n");
            p_work_time = (cas_card_work_time_t *)param;

            pstWorkTime.stBeginTime.ucHour = p_work_time->start_hour;
            pstWorkTime.stBeginTime.ucMinute = p_work_time->start_minute;
            pstWorkTime.stBeginTime.ucSecond = p_work_time->start_second;

            pstWorkTime.stEndTime.ucHour = p_work_time->end_hour;
            pstWorkTime.stEndTime.ucMinute = p_work_time->end_minute;
            pstWorkTime.stEndTime.ucSecond = p_work_time->end_second;

            ret = DSCA_SetWorkTime((UINT8 *)p_work_time->pin, (UINT8 *)&p_work_time->pin_spare_num, &pstWorkTime);
            if(ret != CA_CARD_SUCCESS)
            {
                CAS_DS_PRINTF("[DS]CAS_IOCMD_WORK_TIME_SET WRONG: %d \n", ret);
            }
            CAS_DS_PRINTF("[DS]pin : %x-%x-%x ! \n", p_work_time->pin[0], p_work_time->pin[1], p_work_time->pin[2]);
            CAS_DS_PRINTF("[DS]sapare num %d \n", p_work_time->pin_spare_num);
            CAS_DS_PRINTF("[DS]set card WORKTIME START : %d : %d : %d \n", pstWorkTime.stBeginTime.ucHour, pstWorkTime.stBeginTime.ucMinute, pstWorkTime.stBeginTime.ucSecond);
            CAS_DS_PRINTF("[DS]set card WORKTIME END     : %d : %d : %d \n", pstWorkTime.stEndTime.ucHour, pstWorkTime.stEndTime.ucMinute, pstWorkTime.stEndTime.ucSecond);
              //only for test
            #if 0
            if(p_work_time->pin_spare_num == 0)
            {
                ret = DSCA_SC_UnlockPin();
                CAS_DS_PRINTF("Unlock pin result = %d", ret);
            }
            #endif
            break;

        case CAS_IOCMD_RATING_GET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_RATING_SET \n");
            param = &ds_watch_level;
            ret = SUCCESS;
            break;

        case CAS_IOCMD_RATING_SET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_RATING_SET \n");
            p_rating_set = (cas_rating_set_t *)param;

            ret = DSCA_SetWatchLevel((UINT8 *)p_rating_set->pin, (UINT8 *)&p_rating_set->pin_spare_num, (UINT8 *)&p_rating_set->rate);
            {
                if(ret != CA_CARD_SUCCESS)
                    CAS_DS_ERR("[DS]CAS_IOCMD_RATING_SET WRONG: %d \n", ret);
            }
            CAS_DS_PRINTF("[DS]pin : %x-%x-%x ! \n", p_rating_set->pin[0], p_rating_set->pin[1], p_rating_set->pin[2]);
            CAS_DS_PRINTF("[DS]sapare num %d \n", p_rating_set->pin_spare_num);
            CAS_DS_PRINTF("[DS]pating set %d \n", p_rating_set->rate);
            break;

        case CAS_IOCMD_CUR_PRG_NUM_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_CUR_PRG_NUM_GET \n");
            p_entitles_info = (product_entitles_info_t *)param;
            ret = DSCA_GetProductNumber(p_entitles_info->operator_id, (UINT32 *)&max_num_p);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get product number error : %d! \n", ret);
                return ERR_FAILURE;
            }
            p_entitles_info->max_num = (u16)max_num_p;
            CAS_DS_PRINTF("[DS]operator id : %d! \n", p_entitles_info->operator_id);
            CAS_DS_PRINTF("[DS]get product number : %d! \n", p_entitles_info->max_num);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_ENTITLE_INFO_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_ENTITLE_INFO_GET \n");
            p_entitles_info = (product_entitles_info_t *)param;
            p_entitles_info->operator_id = 1;  // for test
            // get num
            ret = DSCA_GetProductNumber(p_entitles_info->operator_id, (UINT32 *)&max_num_p);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_PRINTF("[DS]get product number error : %d! \n", ret);
                return ERR_FAILURE;
            }
            p_entitles_info->max_num = (u16)max_num_p;
            CAS_DS_PRINTF("[DS]operator id : %d! \n", p_entitles_info->operator_id);
            CAS_DS_PRINTF("[DS]get product number : %d! \n", p_entitles_info->max_num);
            // get info
            if(ds_get_product_info(p_entitles_info) != SUCCESS)
            {
                CAS_DS_ERR("[DS]get product info error : %d! \n", ret);
                return ERR_FAILURE;
            }
            ret = SUCCESS;
            break;

        case CAS_IOCMD_PIN_SET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_PIN_SET \n");
            p_pin_modify = (cas_pin_modify_t *)param;

            ret = DSCA_ChangePin((UINT8 *)p_pin_modify->old_pin, (UINT8 *)p_pin_modify->new_pin, (UINT8 *)&p_pin_modify->pin_spare_num);
            CAS_DS_PRINTF("[DS]CAS_IOCMD_PIN_SET result : %d! \n", ret);
            CAS_DS_PRINTF("[DS]old_pin : %x-%x-%x ! \n", p_pin_modify->old_pin[0], p_pin_modify->old_pin[1], p_pin_modify->old_pin[2]);
            CAS_DS_PRINTF("[DS]new_pin : %x-%x-%x ! \n", p_pin_modify->new_pin[0], p_pin_modify->new_pin[1], p_pin_modify->new_pin[2]);
            CAS_DS_PRINTF("[DS]sapare num", p_pin_modify->pin_spare_num);
            break;

        case CAS_IOCMD_PIN_VERIFY:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_PIN_VERIFY \n");
            p_pin_modify = (cas_pin_modify_t *)param;
            
            ret = DSCA_PurseCheckPin(p_pin_modify->old_pin, (UINT8 *)&p_pin_modify->pin_spare_num);
            {
                if(ret != CA_CARD_SUCCESS)
                    CAS_DS_ERR("[DS]CAS_IOCMD_PIN_VERIFY FAIL = %d", ret);
            }
            CAS_DS_PRINTF("[DS]old_pin : %x-%x-%x ! \n", p_pin_modify->old_pin[0], p_pin_modify->old_pin[1], p_pin_modify->old_pin[2]);
            CAS_DS_PRINTF("[DS]sapare num %d", p_pin_modify->pin_spare_num);
            break;

        case CAS_IOCMD_BURSE_INFO_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_BURSE_INFO_GET \n");
            p_burses_info = (burses_info_t *)param;
            for(index = 1; index < 5; index ++)
            {
                ret = DSCA_GetPurserInfo(index, (UINT8 *) p_burses_info->p_burse_info[index].burse_value);
                if(ret != CA_ERROR_OK)
                {
                    CAS_DS_ERR("[DS]burse get wrong\n");
                    return ERR_FAILURE;
                }
                CAS_DS_PRINTF("[DS] burse %d value is: %s \n", index, p_burses_info->p_burse_info[index].burse_value);
            }
            ret = SUCCESS;
            break;

        case CAS_IOCMD_OPERATOR_INFO_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_OPERATOR_INFO_GET \n");
            p_operators_info = (cas_operators_info_t *)param;
            ret = ds_get_operator_info(p_operators_info);
            break;

        case CAS_IOCMD_MAIL_HEADER_GET:
            p_mail_headers = (cas_mail_headers_t *)param;
            ret = DSCA_GetEmailNumber(&index);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get email number error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get email num: %d! \n", index);
            p_mail_headers->total_email_num = index;
            ret = DSCA_GetEmailInfoList(&pstEmailInfoList);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get email list error : %d! \n", ret);
                return ERR_FAILURE;
            }
            p_mail_headers->max_num = pstEmailInfoList.ucMailNum;
            CAS_DS_PRINTF("[DS]get email max num: %d! \n", p_mail_headers->max_num);
            for(index = 0; index < p_mail_headers->max_num; index ++)
            {
                p_mail_headers->p_mail_head[index].m_id = pstEmailInfoList.stMailInfo[index].ucMailID;
                p_mail_headers->p_mail_head[index].new_email = pstEmailInfoList.stMailInfo[index].ucNewFlag;
                memcpy(p_mail_headers->p_mail_head[index].subject,
                                                   pstEmailInfoList.stMailInfo[index].aucMailTitle, 32);
                memcpy(p_mail_headers->p_mail_head[index].sender,
                                                   pstEmailInfoList.stMailInfo[index].aucMailSender, 32);
                p_mail_headers->p_mail_head[index].creat_date[0] = pstEmailInfoList.stMailInfo[index].stSendTime.usYear >> 8;
                p_mail_headers->p_mail_head[index].creat_date[1] = pstEmailInfoList.stMailInfo[index].stSendTime.usYear & 0x00ff;
                p_mail_headers->p_mail_head[index].creat_date[2] = pstEmailInfoList.stMailInfo[index].stSendTime.ucMonth;
                p_mail_headers->p_mail_head[index].creat_date[3] = pstEmailInfoList.stMailInfo[index].stSendTime.ucDay;
                p_mail_headers->p_mail_head[index].creat_date[4] = pstEmailInfoList.stMailInfo[index].stSendTime.ucHour;
                p_mail_headers->p_mail_head[index].creat_date[5] = pstEmailInfoList.stMailInfo[index].stSendTime.ucMinute;
                p_mail_headers->p_mail_head[index].creat_date[6] = pstEmailInfoList.stMailInfo[index].stSendTime.ucSecond;

                CAS_DS_PRINTF("[DS] email: %d! \n", index);
                CAS_DS_PRINTF("[DS]get email id: %d! \n", p_mail_headers->p_mail_head[index].m_id);
                CAS_DS_PRINTF("[DS]get email flag: %d! \n",p_mail_headers->p_mail_head[index].new_email);
                CAS_DS_PRINTF("[DS]get email subject: %s! \n",p_mail_headers->p_mail_head[index].subject);
                CAS_DS_PRINTF("[DS]get email sender: %s! \n",p_mail_headers->p_mail_head[index].sender);
                CAS_DS_PRINTF("[DS]get email send data: %d-%d-%d-%d-%d-%d! \n",p_mail_headers->p_mail_head[index].creat_date[0]*256 + p_mail_headers->p_mail_head[index].creat_date[1],
                                                                                                                                p_mail_headers->p_mail_head[index].creat_date[2], p_mail_headers->p_mail_head[index].creat_date[3],
                                                                                                                                p_mail_headers->p_mail_head[index].creat_date[4], p_mail_headers->p_mail_head[index].creat_date[5], p_mail_headers->p_mail_head[index].creat_date[6]);

            }
            ret = SUCCESS;
            break;

        case CAS_IOCMD_MAIL_BODY_GET:
            p_mail_body = (cas_mail_body_t *)param;
            ret = DSCA_GetEmailContent((UINT32)p_mail_body->mail_id, (UINT8 *)p_mail_body->data, (UINT16 *)&p_mail_body->data_len);
            if(ret != CA_ERROR_OK)
            {
                CAS_DS_ERR("[DS]get email content error : %d! \n", ret);
                return ERR_FAILURE;
            }
            CAS_DS_PRINTF("[DS]get email id: %d! \n", p_mail_body->mail_id);
            CAS_DS_PRINTF("[DS]get email len: %d! \n", p_mail_body->data_len);
            CAS_DS_PRINTF("[DS]get email data: %s! \n", p_mail_body->data);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_MAIL_DELETE_ALL:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_MAIL_DELETE_ALL \n");
            ret = DSCA_DelAllEmail();               //ret = ??
            break;

        case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_MAIL_DELETE_BY_INDEX \n");
            p_mail_id = (u32 *)param;
            ret = DSCA_DelEmail((UINT8)*p_mail_id);  //ret = ??
            CAS_DS_PRINTF("[DS]DEL the email: %d \n",*p_mail_id);
            break;

        case CAS_IOCMD_LANGUAGE_SET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_LANGUAGE_SET \n");
            p_sys_language = (u32 *)param;
            if(*p_sys_language == CA_LANGUAGE_CHINESE || *p_sys_language == CA_LANGUAGE_ENGLISH)
            {
                DSCA_SetSysLanguage(*p_sys_language);
                CAS_DS_PRINTF("[DS]the language id: %d \n",*p_sys_language);
                ret = SUCCESS;
            }
            else
            {
                CAS_DS_ERR("[DS]not support the language id: %d \n",*p_sys_language);
                ret = ERR_FAILURE;
            }
            break;

        case CAS_IOCMD_FINGER_MSG_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_FINGER_MSG_GET \n");
            p_finger_msg = (finger_msg_t *)param;
            DS_STB_Drv_CaReqLock(&ca_finger_lock);
            memcpy(p_finger_msg, &ds_msgFinger, sizeof(finger_msg_t));
            DS_STB_Drv_CaReqUnLock(&ca_finger_lock);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_OSD_GET:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_OSD_MSG_GET \n");
            p_osd_msg = (msg_info_t *)param;
            DS_STB_Drv_CaReqLock(&ca_osd_lock);
            memcpy(p_osd_msg, &ds_OsdMsg, sizeof(msg_info_t));
            CAS_DS_PRINTF("get p_osd_msg-MSG %s \n ", p_osd_msg->data);
            DS_STB_Drv_CaReqUnLock(&ca_osd_lock);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_OSD_MSG_SHOW_END:
            CAS_DS_PRINTF("[DS] CAS_IOCMD_OSD_GET \n");
            DSCA_OsdOneTimeOk();
            ret = SUCCESS;
            break;

        case CAS_IOCMD_SHOW_TYPE_GET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_SHOW_TYPE_GET \n");
            p_msg_show_type = (u8 *)param;
            *p_msg_show_type = ds_msg_show_type;
            ret = SUCCESS;
            break;

        case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_FORCE_CHANNEL_INFO_GET \n");
            p_exigent = (cas_force_channel_t *)param;
            p_exigent->netwrok_id = ds_Exigent.usNetWorkID;
            p_exigent->serv_id = ds_Exigent.usServiceID;
            p_exigent->ts_id = ds_Exigent.usTransportStreamID;
            ret = SUCCESS;
            break;

        case CAS_IOCMD_IPP_BUY_INFO_GET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_IPPV_INFO_GET \n");
            p_ippv_info = (ipps_info_t *)param;
            ds_get_ipps_info(p_ippv_info);
            ret = SUCCESS;
            break;

        case CAS_IOCMD_PURCHASE_SET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_PURCHASE_SET \n");
            p_ippv_info = (ipps_info_t *)param;
            ret = DSCA_IppvBuy(1, (UINT8)(p_ippv_info->p_ipp_info[0].operator_id));
            switch(ret)
            {
                case CA_CARD_WRITEPURSER_ERROR:
                    cas_send_event(p_priv->slot, CAS_ID_DS, CAS_E_INVALID_PRICE, 0);
                    break;
                case CA_CARD_NOMONEY:
                    cas_send_event(p_priv->slot, CAS_ID_DS, CAS_E_CARD_MONEY_LACK, 0);
                    break;
                default:
                    break;
            }
            //  ret = DSCA_IppvBuy(1, 2);
            CAS_DS_PRINTF("[DS]ippv buy result: %d \n",ret);
            CAS_DS_PRINTF("[DS]ippv buy id: %d \n",p_ippv_info->p_ipp_info[0].operator_id);
            break;

        case CAS_IOCMD_ZONE_CODE_SET:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_ZONE_CODE_SET \n");
            p_nit_zone = (u8 *)param;
            DSCA_SetCardAddressCode((UINT8 *)p_nit_zone);
            CAS_DS_PRINTF("[DS]nit set: %s \n",p_nit_zone);
            break;

        case CAS_IOCMD_ZONE_CHECK:
            CAS_DS_PRINTF("[DS]CAS_IOCMD_ZONE_CHECK \n");
            u8 p_data[7] = {0x01,0x00,0x01,0x00,0x02,0x00,0x01};
            DSCA_SetCardAddressCode(p_data);
             cas_send_event(p_priv->slot, CAS_ID_DS, CAS_S_ZONE_CHECK_END, 0);
            #if 0
            CAS_DS_PRINTF("[DS] CAS_IOCMD_ZONE_CHECK \n");
            p_nim_info = (nim_channel_info_t *)param;
            p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                                DEV_IDT_TYPE,
                                                                SYS_DEV_TYPE_NIM);
            //lock main freq
            CAS_DS_PRINTF("[DS] set tuner_lock_delivery %d %d %d\n",p_nim_info->frequency,
		     p_nim_info->param.dvbc.symbol_rate,p_nim_info->param.dvbc.modulation);
            nim_channel_connect(p_nim_dev, p_nim_info, 0);
            if(p_nim_info->lock != 1)
            {
                CAS_DS_ERR("[DS], nim unlock ! \n");
                cas_send_event(p_priv->slot, CAS_ID_DS, CAS_S_ZONE_CHECK_END, 0);
            }
            else
            {
                 CAS_DS_PRINTF("[DS], nim lock ! \n");
                 DSCA_SetFilter(NIT_TABLE_ID,nit_match, nit_mask,1,NIT_PID,2);
            }
            #endif
            break;

           default:
              break;
//lint -esym(438,param)

        }
        return ret;
//lint +e64, +e545

  }


static RET_CODE cas_ds_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{
    s_cas_ds_priv_t *p_priv = NULL;
    u16 section_len = 0;

    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!tid=%d\n", __FUNCTION__, __LINE__, t_id);
    section_len = MAKE_WORD(p_buf[2], p_buf[1] & 0xF) + 3;

    p_priv = (s_cas_ds_priv_t *)g_cas_priv->cam_op[CAS_ID_DS].p_priv;

    if (CAS_TID_PMT == t_id)
    {
        CAS_DS_PRINTF("[DS]desai: pmt process\n");
        memcpy(g_cas_priv->pmt_data, p_buf, section_len);
        g_cas_priv->pmt_length = section_len;
        ds_cas_parse_pmt(g_cas_priv->pmt_data);
    }
    else if (CAS_TID_CAT == t_id)
    {
        CAS_DS_PRINTF("[DS]desai: cat process\n");
        memcpy(g_cas_priv->cat_data, p_buf, section_len);
        g_cas_priv->cat_length = section_len;
        ds_cas_parse_cat(g_cas_priv->cat_data);
    }
    else if (CAS_TID_NIT == t_id)
    {
        CAS_DS_PRINTF("[DS]desai: nit process\n");
        memcpy(g_cas_priv->nit_data, p_buf, section_len);
        g_cas_priv->nit_length = section_len;
        p_priv->private_data_specifier = (u32)((((u16)(g_cas_priv->nit_data[3])) << 8)
                            | (g_cas_priv->nit_data[4]));
        ds_cas_parse_nit(g_cas_priv->nit_data, section_len);
    }

    return SUCCESS;
}

static void ds_set_sc_insert_status(u8 uStatus)
{
    s_ds_uScStatus = (e_ds_smartcard_status_t)uStatus;
}


static RET_CODE cas_ds_card_remove(void)
{
    s_cas_ds_priv_t *p_priv  = NULL;
    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

    p_priv = (s_cas_ds_priv_t *)g_cas_priv->cam_op[CAS_ID_DS].p_priv;
    p_priv->card_status = SMC_CARD_REMOVE;

    cas_send_event(p_priv->slot, CAS_ID_DS, CAS_S_CARD_REMOVE, 0);
    ds_set_sc_insert_status(SC_REMOVE_START);
    DSCA_SetCardState(0);
    ds_set_sc_insert_status(SC_REMOVE_OVER);

    return SUCCESS;
}

static RET_CODE cas_ds_card_reset(u32 slot, card_reset_info_t *p_info)
{
    s_cas_ds_priv_t *p_priv = NULL;

    CAS_DS_PRINTF("[DS]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    if (NULL == p_info)
    {
        CAS_DS_ERR("[DS]%s:LINE:%d, p_info is NULL!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }
    p_priv = g_cas_priv->cam_op[CAS_ID_DS].p_priv;

    p_ds_scard_dev = (scard_device_t *)p_info->p_smc;
    scard_pro_register_op((scard_device_t *)p_info->p_smc);
    p_priv->slot = (u8)slot;

    p_priv->card_status = SMC_CARD_INSERT;
    CAS_DS_PRINTF("[DS]%s:LINE:%d, smartCard insert!\n", __FUNCTION__, __LINE__);
    ds_set_sc_insert_status(SC_INSERT_START);
    DSCA_SetCardState(1);
    ds_set_sc_insert_status(SC_INSERT_OVER);

    return SUCCESS;
}




RET_CODE cas_ds_attach_v5(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
    s_cas_ds_priv_t *p_priv = NULL;

    g_cas_priv = cas_get_private_data();

    g_cas_priv->cam_op[CAS_ID_DS].attached = 1;
    g_cas_priv->cam_op[CAS_ID_DS].inited = 0;

    g_cas_priv->cam_op[CAS_ID_DS].func.init
        = cas_ds_init;
    g_cas_priv->cam_op[CAS_ID_DS].func.deinit
        = cas_ds_deinit;
    g_cas_priv->cam_op[CAS_ID_DS].func.identify
        = cas_ds_identify;
    g_cas_priv->cam_op[CAS_ID_DS].func.io_ctrl
        = cas_ds_io_ctrl;
    g_cas_priv->cam_op[CAS_ID_DS].func.table_process
        = cas_ds_table_process;
    g_cas_priv->cam_op[CAS_ID_DS].func.card_remove = cas_ds_card_remove;
    g_cas_priv->cam_op[CAS_ID_DS].func.card_reset = cas_ds_card_reset;

    p_priv = cas_porting_drv_base_malloc(sizeof(s_cas_ds_priv_t));
    g_cas_priv->cam_op[CAS_ID_DS].p_priv = (s_cas_ds_priv_t *)p_priv;
    memset(p_priv, 0x00, sizeof(s_cas_ds_priv_t));

    p_priv->flash_size = p_cfg->flash_size;
    p_priv->flash_start_add = p_cfg->flash_start_adr;
    CAS_DS_PRINTF("[DS]dscas:flash  %x %x\n", p_priv->flash_size, p_priv->flash_start_add);
    p_priv->nvram_read = p_cfg->nvram_read;
    p_priv->nvram_write = p_cfg->nvram_write;
    p_priv->query_check = p_cfg->query_check;
    p_priv->task_prio = p_cfg->task_prio;
    p_priv->task_prio_monitor = p_cfg->end_task_prio;


    *p_cam_id = (u32)(&g_cas_priv->cam_op[CAS_ID_DS]);

    CAS_DS_PRINTF("[DS]cas_ds_attach ok\n");

    return SUCCESS;
}

