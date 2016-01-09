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
#include "drv_dev.h"
#include "pti.h"
#include "ads_ware.h"
#include "ads_adapter.h"
#include "ads_api_desai.h"
#include "ads_desai_include.h"

//#define ADS_DESAI_ADT_PRINTF

#ifdef ADS_DESAI_ADT_PRINTF
    #define ADS_ADT_PRINTF OS_PRINTF
#else
    #define ADS_ADT_PRINTF DUMMY_PRINTF
#endif

#define GET_AD_PID_MAX_TIMES    0x2FF

//extern ads_adapter_priv_t g_ads_priv;
static ads_adapter_priv_t *g_ads_priv = NULL;
ads_desai_priv_t *p_ads_ds_priv = NULL;

/*********************************DRV API*************************************/
extern BOOL ds_ad_nim_lock(u32 freq, u32 symbolrate, u16 qammode);
extern BOOL ds_ad_set_filter_nit_table(void);
extern void ds_ads_client_init(void);
extern u16 ds_ad_get_pid(void);
/*****************************************************************************/
extern RET_CODE ui_adv_desai_display(ads_info_t *p_ads_info);

static EN_DSAD_POS_TYPE desai_ad_type_get(u32 ads_type)
{
    EN_DSAD_POS_TYPE desai_ad_type = DSAD_STB_POS_UNKNOWN;

    switch(ads_type)
    {
        case ADS_AD_TYPE_LOGO:
            desai_ad_type = DSAD_STB_LOG;
            break;

        case ADS_AD_TYPE_CHLIST:
            desai_ad_type = DSAD_STB_EPG;
            break;

        case ADS_AD_TYPE_WELCOME:
            desai_ad_type = DSAD_STB_FULLSCREEN;
            break;

        case ADS_AD_TYPE_VOLBAR:
            desai_ad_type = DSAD_STB_VOLUME;
            break;

        case ADS_AD_TYPE_MAIN_MENU:
            desai_ad_type = DSAD_STB_MENU;
            break;
            
        case ADS_AD_TYPE_UNAUTH_PG:
            desai_ad_type = DSAD_STB_UNAUTHORIZED;
            break;
			
        default:
			break;
    }

    return desai_ad_type;
}

RET_CODE ads_adt_desai_init(void *param)
{
    RET_CODE ret = FALSE;
    ads_desai_priv_t *p_ads_desai_priv = g_ads_priv->adm_op[ADS_ID_ADT_DESAI].p_priv;

    //Init
    ret = DSAD_Init(p_ads_desai_priv->task_prio_start,
                              p_ads_desai_priv->flash_start_adr, 
                              (EN_DSAD_STB_DECODE_TYPE)p_ads_desai_priv->platform_type);
    if(ret == FALSE)
    {
        ADS_ADT_PRINTF("[adt] %s,%d ERROR! \n",__func__,__LINE__);
        return ERR_FAILURE;
    }
    return SUCCESS;
}

RET_CODE ads_adt_desai_deinit(void)
{
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
    return SUCCESS;
}

RET_CODE ads_adt_desai_open(void)
{
    BOOL ret = FALSE;
    u32 index = 0;
    u16 ad_pid = 0xffff;

    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

    //start monitor task
    ds_ads_client_init();
    
    //parser NIT table
    ret = ds_ad_nim_lock(p_ads_ds_priv->channel_frequency,
                                        p_ads_ds_priv->channel_symbolrate,
                                        p_ads_ds_priv->channel_qam);
    if(ret == FALSE)
    {
        ADS_ADT_PRINTF("[adt] %s,%d ERROR! \n",__func__,__LINE__);
        return ERR_FAILURE;
    }

    ds_ad_set_filter_nit_table();
    
    for(index = 0; index < GET_AD_PID_MAX_TIMES; index++)
    {
        ad_pid = ds_ad_get_pid();
        if(ad_pid <= 0x1fff)
        {
            //set PID
            DSAD_SetDataPid(ad_pid);
            return SUCCESS;
        }
        mtos_task_sleep(20);
    }

    return ERR_FAILURE;
}

RET_CODE ads_adt_desai_close(void)
{
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
    return SUCCESS;
}

RET_CODE ads_adt_desai_io_ctrl(u32 cmd, void *param)
{
    RET_CODE ret = SUCCESS;
    EN_DSAD_POS_TYPE desai_ad_type = DSAD_STB_POS_UNKNOWN;
    ST_DSAD_PROGRAM_SHOW_INFO program_show;
    ST_DSAD_PROGRAM_PARA      program_para = {0};
    ST_DSAD_OSD_PROGRAM_SHOW_INFO osd_show;
    ads_info_t *p_ads_info = (ads_info_t *)param;
        
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

    memset(&program_show, 0, sizeof(ST_DSAD_PROGRAM_SHOW_INFO));
    memset(&osd_show, 0, sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));

    desai_ad_type = desai_ad_type_get(p_ads_info->ads_type);
    switch(cmd)
    {
        case ADS_IOCMD_AD_NO_PROGRAM:
            ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
            ret = DSAD_NoProgramGetElement(&program_show, desai_ad_type);
            if(ret != DSAD_FUN_OK)
                break;
            if(ADS_AD_TYPE_PIC == p_ads_info->ads_media_type)
            {
                memcpy((void *)p_ads_info->p_priv, (const void *)&program_show.stPictureShow, sizeof(ST_DSAD_PICTURE_SHOW_INFO));
            }
            else
            {
                memcpy((void *)p_ads_info->p_priv, (const void *)&program_show.stAvPlay, sizeof(ST_DSAD_AV_SHOW_INFO));
            }
            break;

        case ADS_IOCMD_AD_PROGRAM:
            ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
            program_para.usNetworkId = p_ads_info->network_id;
            program_para.usTsId = p_ads_info->ts_id;
            program_para.usServiceId = p_ads_info->service_id;
            ret = DSAD_ProgramGetElement(program_para, &program_show, desai_ad_type);
            if(ret != DSAD_FUN_OK)
                break;
            if(ADS_AD_TYPE_PIC == p_ads_info->ads_media_type)
            {
                memcpy((void *)p_ads_info->p_priv, (const void *)&program_show.stPictureShow, sizeof(ST_DSAD_PICTURE_SHOW_INFO));
            }
            else
            {
                memcpy((void *)p_ads_info->p_priv, (const void *)&program_show.stAvPlay, sizeof(ST_DSAD_AV_SHOW_INFO));
            }
            break;

        case ADS_IOCMD_AD_OSD:
            ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
            program_para.usNetworkId = p_ads_info->network_id;
            program_para.usTsId = p_ads_info->ts_id;
            program_para.usServiceId = p_ads_info->service_id;
            ret = DSAD_ProgramGetOSD(program_para, &osd_show);
            if(ret != DSAD_FUN_OK)
                break;
            memcpy((void *)p_ads_info->p_priv, (const void *)&osd_show, sizeof(ST_DSAD_OSD_PROGRAM_SHOW_INFO));
            break;
			
         case ADS_IOCMD_AD_VERSION_GET:
             ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
             ret =  DSAD_GetADVersion((UINT8 *)param);
			 break;
			 
		 default:
		 	ret = ERR_PARAM;
			break;
    }

    return ret;
}

RET_CODE ads_adt_desai_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);

    g_ads_priv = ads_get_adapter_priv();
 
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].attached = 1;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].inited = 0;

    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.init
        = ads_adt_desai_init;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.deinit
        = ads_adt_desai_deinit;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.open
        = ads_adt_desai_open;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.close
        = ads_adt_desai_close;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.display
        = ui_adv_desai_display;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.hide
        = NULL;
    g_ads_priv->adm_op[ADS_ID_ADT_DESAI].func.io_ctrl
        = ads_adt_desai_io_ctrl;

   p_ads_ds_priv = g_ads_priv->adm_op[ADS_ID_ADT_DESAI].p_priv
     = mtos_malloc(sizeof(ads_desai_priv_t));
   MT_ASSERT(NULL != p_ads_ds_priv);
   memset(p_ads_ds_priv, 0x00, sizeof(ads_desai_priv_t));

   p_ads_ds_priv->id = ADS_ID_ADT_DESAI;
   p_ads_ds_priv->platform_type = p_cfg->platform_type;
   p_ads_ds_priv->p_dmx_dev = p_cfg->p_dmx_dev;
   p_ads_ds_priv->flash_size = p_cfg->flash_size;
   p_ads_ds_priv->flash_start_adr = p_cfg->flash_start_adr;
   p_ads_ds_priv->nvram_read = p_cfg->nvram_read;
   p_ads_ds_priv->nvram_write = p_cfg->nvram_write;
   p_ads_ds_priv->nvram_erase2 = p_cfg->nvram_erase2;
   p_ads_ds_priv->read_gif = p_cfg->read_gif;
   p_ads_ds_priv->read_rec = p_cfg->read_rec;
   p_ads_ds_priv->write_rec = p_cfg->write_rec;
   p_ads_ds_priv->channel_frequency = p_cfg->channel_frequency;
   p_ads_ds_priv->channel_symbolrate = p_cfg->channel_symbolrate;
   p_ads_ds_priv->channel_qam = p_cfg->channel_qam;
   p_ads_ds_priv->task_prio_start = p_cfg->task_prio_start;
   p_ads_ds_priv->task_prio_end = p_cfg->task_prio_end;
   p_ads_ds_priv->notify_msg_to_ui = p_cfg->notify_msg_to_ui;

   *p_adm_id = ADS_ID_ADT_DESAI;

   return SUCCESS;
}


