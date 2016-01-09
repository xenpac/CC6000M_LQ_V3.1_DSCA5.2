/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"

#include "stdio.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_mutex.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "service.h"
#include "class_factory.h"
#include "mdl.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"

#include "subt_ttx_api.h"
#include "subt_ttx_ctrl_api.h"
#include "err_check_def.h"

/*!
 *  MACRO
 */

#define ERR_CHECK(ret)  if(SUCCESS != ret)\
                        {\
                            OS_PRINTF("[subt]:%s %d %d\n",__FUNCTION__, __LINE__,ret);\
                            break;\
                        }

#define ERR_POINT(p)    if(NULL == p)\
                        {\
                            OS_PRINTF("[subt]:%s %d\n",__FUNCTION__, __LINE__);\
                            break;\
                        }

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*! 
 * Use type is convenient, so change value to type
 */
typedef struct 
{
   u8 m_val;
   u8 m_type;
}subt_ttx_type_map_t;

static const subt_ttx_type_map_t subt_type_map[] = 
{
    {0x10, E_SUBT_TYPE_NORMAL},
    {0x11, E_SUBT_TYPE_AR4_3},
    {0x12, E_SUBT_TYPE_AR16_9},
    {0x13, E_SUBT_TYPE_AR2P21_1},
    {0x14, E_SUBT_TYPE_AR_HD}, 
    {0x20, E_SUBT_TYPE_NORMAL_HI},
    {0x21, E_SUBT_TYPE_AR4_3_HI},
    {0x22, E_SUBT_TYPE_AR16_9_HI},
    {0x23, E_SUBT_TYPE_AR2P21_1_HI}
};
static const subt_ttx_type_map_t ttx_type_map[] = 
{
    {0x01, E_TTX_INITIAL_PAGE},
    {0x02, E_SUBT_TYPE_TTX_NORMAL},
    {0x03, E_TTX_ADDITIONAL_INFO_PAGE},
    {0x04, E_TTX_PROG_SCHEDULE_PAGE},
    {0x05, E_SUBT_TYPE_TTX_HEARING_HI},
    {0x21, E_SUBT_TYPE_AR4_3_HI},
    {0x22, E_SUBT_TYPE_AR16_9_HI},
    {0x23, E_SUBT_TYPE_AR2P21_1_HI}
};
            
typedef struct
{
    /*!
     * Record the state for UI, UI may start subtitle frequently
     * If same, subtitle only need be started once
     */
    BOOL             m_ttx_start;
    /*!
     * Record the state for UI, UI may start subtitle frequently
     * If same, subtitle only need be started once
     */
    BOOL             m_subt_start;
    /*!
     * Record UI osd type, ttx osd is only used for ttx, other use normal osd
     */
    BOOL             m_ttx_osd;
    /*!
     * UI need show subtitle when program is played, but pmt may not be parsed
     * So set this value, when pmt has been parsed, show subtitle
     */
    BOOL            enable;

    /*!
     * Type of subtitle passed from UI
     */
    subt_ttx_type_t wait_type;

    u32 dmx_in;
    /*!
     * If subtitle with teletext need be shown automatic, it need call UI set ttx osd function
     */
    subt_ctrl_call_back      ttx_type_subt_cb;
    /*!
     * UI records language, because most users use same language, this language has high priority.
     * If subtitle in dr doesn't contain this language, use the first subtitle
     */
    u8               m_ui_lang[SUBT_TTX_CTRL_LANG_LEN];
    /*!
     * Count of subtitle dr received from pmt
     */
    u8               m_cnt;
    /*!
     * All subtitle dr information
     */
    subtitle_descr_t m_dr[DVB_MAX_SUBTILTE_DESC];

    subtitle_descr_t     cur_subt_dr;
    teletext_descr_t     cur_ttx_dr;
    char cur_lang[4];
    u8 cur_type;
}subt_ctrl_t;

typedef struct
{
    /*!
     * Count of teletext dr received from pmt
     */
    u8               m_cnt;
    /*!
     * Teletext start and stop must match
     */
    BOOL             m_start;
    u8               m_ui_lang[SUBT_TTX_CTRL_LANG_LEN];
    /*!
     * Teletext start and stop must match
     */
    u8  cur_lang[SUBT_TTX_CTRL_LANG_LEN];
    /*!
     * All teletext dr information
     */
    teletext_descr_t m_dr[DVB_MAX_TELETEXT_DESC];
}ttx_ctrl_t;

typedef struct
{
  subt_ctrl_t m_subt_ctrl;
  ttx_ctrl_t  m_ttx_ctrl;   
  BOOL enable_ttx_type_subt;
  u32 mutex_lock_mode;
  void *p_mutex;
  subt_ctrl_call_back subt_ready_cb;
  subt_ctrl_call_back ttx_ready_cb;
  u16 pg_id;
}subt_ttx_ctrl_priv_t;

/*================================================================================================
                SUBT and TTX internal founctions
=================================================================================================*/
static subt_ttx_ctrl_priv_t *get_priv(void)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;  

    p_this = (subt_ttx_ctrl_priv_t *)class_get_handle_by_id(SUBT_TTX_CTRL_CLASS_ID);

    return p_this;
}

static void _subt_ttx_ctrl_lock(subt_ttx_ctrl_priv_t *p_priv)
{
  if (OS_MUTEX_LOCK ==p_priv->mutex_lock_mode) {
     mtos_mutex_take(p_priv->p_mutex);
  }else{
     mtos_task_lock();
  }
}

static void _subt_ttx_ctrl_unlock(subt_ttx_ctrl_priv_t *p_priv)
{
  if (OS_MUTEX_LOCK ==p_priv->mutex_lock_mode) {
     mtos_mutex_give(p_priv->p_mutex);
  }else{
     mtos_task_unlock();
  }
}


static BOOL _subt_ttx_ctrl_check_lang(u8 *p_lang_s, u8 *p_lang_d)
{
    u8 i = 0;

    if(NULL == p_lang_s)
    {
        return FALSE;
    }
    
    for(i = 0; i < SUBT_TTX_CTRL_LANG_LEN - 1; i++)
    {
        if(p_lang_s[i] != p_lang_d[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}
/*================================================================================================
                TTX internal founctions
=================================================================================================*/
static void _ttx_ctrl_reset(ttx_ctrl_t *p_ttx)
{
    _subt_ttx_ctrl_lock(get_priv());
    
    memset(p_ttx->m_ui_lang, 0, SUBT_TTX_CTRL_LANG_LEN * sizeof(u8));
    memset(p_ttx->cur_lang, 0, SUBT_TTX_CTRL_LANG_LEN * sizeof(u8));
    memset(p_ttx->m_dr, 0, SUBT_TTX_CTRL_LANG_LEN * sizeof(teletext_descr_t));
    p_ttx->m_cnt = 0;
    _subt_ttx_ctrl_unlock(get_priv());
}

static void _ttx_ctrl_convert_type(ttx_ctrl_t *p_ttx)
{
    u8 i = 0;
    u8 j = 0;

    for(i = 0; i < p_ttx->m_cnt; i++)
    {
        for(j = 0; j < ARRAY_SIZE(ttx_type_map); j++)
        {
            /*!
             * Change value to type
             */
            if(p_ttx->m_dr[i].type == ttx_type_map[j].m_val)
            {
               p_ttx->m_dr[i].type =  ttx_type_map[j].m_type;
               break;
            }
        }

        /*!
         * If not found, set type as E_SUBT_TYPE_UNDEF,  
         */
        if(j == ARRAY_SIZE(ttx_type_map))
        {
            p_ttx->m_dr[i].type =  E_SUBT_TYPE_UNDEF;
        }
    }
}
/*!
 * If language code is invalid, ignore
 */
static BOOL _ttx_ctrl_check_lang_valid(u8 *p_lang)
{
    u8 i = 0;

    for(i = 0; i < SUBT_TTX_CTRL_LANG_LEN - 1; i++)
    {
        if((p_lang[i] > 'z') || (p_lang[i] < 'a'))
        {
            return FALSE;
        }
    }

    return TRUE;
}                                  
/*!
 * If language and type is same, save one of them
 */
static BOOL _ttx_ctrl_filter_same(teletext_descr_t *p_dr_arr, 
                                  u8 cnt, 
                                  teletext_descr_t *p_pmt_dr)
{
    u8   i    = 0;
    BOOL same = FALSE;
    
    for(i = 0; i < cnt; i++)
    {
        same = _subt_ttx_ctrl_check_lang(p_dr_arr[i].language_code, p_pmt_dr->language_code);
        
        if(same && (p_dr_arr[i].type == p_pmt_dr->type))
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*!
 * Ignore same teletext
 */
static void _ttx_ctrl_save_description(ttx_ctrl_t *p_ttx, pmt_t *p_pmt)
{
    u8   i       = 0;
    u8   cnt     = 0;
    BOOL ret_val = FALSE;

    cnt = MIN(p_pmt->ttx_dr_cnt , DVB_MAX_TELETEXT_DESC);

    p_ttx->m_cnt = 0;

    for(i = 0; i < cnt; i++)
    {
        ret_val = _ttx_ctrl_check_lang_valid(p_pmt->ttx_descr[i].language_code);
        if(FALSE == ret_val)
        {
            continue;
        }
        
        ret_val = _ttx_ctrl_filter_same(p_ttx->m_dr, p_ttx->m_cnt, &p_pmt->ttx_descr[i]);
        if(ret_val)
        {
            continue;
        }
        
        p_ttx->m_dr[p_ttx->m_cnt] = p_pmt->ttx_descr[i];
        p_ttx->m_cnt++;
    }
}
static RET_CODE _ttx_ctrl_process(pmt_t *p_pmt)
{
    u8                    cnt    = 0;
    ttx_ctrl_t           *p_ttx  = NULL;
    subt_ttx_ctrl_priv_t *p_this = NULL;

    do
    {   
        p_this = get_priv();
        ERR_POINT(p_this);

        p_ttx = &p_this->m_ttx_ctrl;

        /*!
         * Reset p_ttx, maybe information in p_ttx is not useful
         */
        if((NULL == p_pmt) || (0 == p_pmt->ttx_dr_cnt))
        {
            _ttx_ctrl_reset(p_ttx);
            return SUCCESS;
        }
        
        cnt = MIN(p_pmt->ttx_dr_cnt, DVB_MAX_TELETEXT_DESC);

        /*!
         * Pmt info is under another task, so lock it
         */
        _subt_ttx_ctrl_lock(p_this);

        /*!
         * Copy ttx dr from pmt
         */
        _ttx_ctrl_save_description(p_ttx, p_pmt);
        
        /*!
         * After ttx_descr has been saved in p_ttx then convert type
         */
        _ttx_ctrl_convert_type(p_ttx);
        _subt_ttx_ctrl_unlock(p_this);
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;    
}



static teletext_descr_t *_ttx_ctrl_match_dr(ttx_ctrl_t *p_ttx, u8 *p_lang)
{
    u8                i     = 0;
    BOOL              equal = FALSE;
    teletext_descr_t *p_dr  = NULL;

    for(i = 0; i < p_ttx->m_cnt; i++)
    {
        /*!
         * Save first dr with special type, if no good else, use it
         */
        if((NULL == p_dr) && (E_TTX_INITIAL_PAGE == p_ttx->m_dr[i].type))
        {
            p_dr = &p_ttx->m_dr[i];
        }

        equal = _subt_ttx_ctrl_check_lang(p_lang, p_ttx->m_dr[i].language_code);
        if(equal && (E_TTX_INITIAL_PAGE == p_ttx->m_dr[i].type))
        {
            p_dr = &p_ttx->m_dr[i];
        }
    }

    return p_dr;
}

static RET_CODE _ttx_ctrl_start(ttx_ctrl_t *p_ttx, teletext_descr_t *p_ttx_dr)
{
    RET_CODE ret = ERR_FAILURE;

    do
    {
        /*!
         * First time use start decode interface, otherwise use reset pid interface
         */
        if(FALSE == p_ttx->m_start)
        {
            if(p_ttx_dr != NULL)
            {
              OS_PRINTF("lubin: _ttx_ctrl_start  PID %d\n", p_ttx_dr->pid);
             ret = mul_teletext_start_decode(p_ttx_dr->pid);
            }
            else
            {
              ret = mul_teletext_start_decode(0);
              OS_PRINTF("lubin: _ttx_ctrl_start  0 ret %d\n", ret);
            }
            ERR_CHECK(ret);
            
            p_ttx->m_start = TRUE;
        }
        
        return SUCCESS;
        
    }while(0);

    return ret;
}

static BOOL _ttx_ctrl_check_page_valid(ttx_ctrl_t *p_ttx)
{
    u8 i = 0;
    
    for(i = 0; i < p_ttx->m_cnt; i++)
    {
        if((E_TTX_INITIAL_PAGE == p_ttx->m_dr[i].type)
          && (0x1 == p_ttx->m_dr[i].magazien))
        {
            return TRUE;
        }
    }

    return FALSE;
}

static void _ttx_ctrl_auto(void)
{
  teletext_descr_t *p_ttx_dr = NULL;
  ttx_ctrl_t *p_ttx = NULL;
  subt_ttx_ctrl_priv_t *p_this = get_priv();

  p_ttx = &p_this->m_ttx_ctrl;
  if(p_ttx->m_start == TRUE)
  {
    p_ttx_dr = _ttx_ctrl_match_dr(p_ttx, p_ttx->m_ui_lang);
    if(p_ttx_dr != NULL 
      && memcmp(p_ttx->cur_lang, p_ttx_dr->language_code, 3))
    {
      memcpy(p_ttx->cur_lang, p_ttx_dr->language_code, 3);
      mul_teletext_reset_pid(p_ttx_dr->pid);
    }
    else if(p_ttx->m_cnt > 0)
    {
      p_ttx_dr = &p_ttx->m_dr[0];
      if(memcmp(p_ttx->cur_lang, p_ttx_dr->language_code, 3))
      {
        memcpy(p_ttx->cur_lang, p_ttx_dr->language_code, 3);
        mul_teletext_reset_pid(p_ttx_dr->pid);
      }
    }
  }
}

/*================================================================================================
                SUBT internal founctions
=================================================================================================*/
static void _subt_ctrl_reset(subt_ctrl_t *p_subt)
{
    _subt_ttx_ctrl_lock(get_priv());
    
    memset(p_subt->m_ui_lang, 0, SUBT_TTX_CTRL_LANG_LEN * sizeof(u8));
    memset(p_subt->m_dr, 0, SUBT_TTX_CTRL_LANG_LEN * sizeof(subtitle_descr_t));
    p_subt->m_cnt = 0;
    
    _subt_ttx_ctrl_unlock(get_priv());
}

static void _subt_ctrl_convert_type(subt_ctrl_t *p_subt)
{
    u8 i = 0;
    u8 j = 0;

    for(i = 0; i < p_subt->m_cnt; i++)
    {
        for(j = 0; j < ARRAY_SIZE(subt_type_map); j++)
        {
            /*!
             * Change value to type
             */
            if(p_subt->m_dr[i].type == subt_type_map[j].m_val)
            {
               p_subt->m_dr[i].type =  subt_type_map[j].m_type;
               break;
            }
        }

        /*!
         * If not found, set type as SUBT_TYPE_UNDEF,  
         */
        if(j == ARRAY_SIZE(subt_type_map))
        {
            p_subt->m_dr[i].type =  E_SUBT_TYPE_UNDEF;
        }
    }
}
/*!
 * If language and type is same, save one of them
 */
static BOOL _subt_ctrl_filter_same(subtitle_descr_t *p_dr_arr, 
                                   u8 cnt, 
                                   subtitle_descr_t *p_pmt_dr)
{
    u8   i    = 0;
    BOOL same = FALSE;
    
    for(i = 0; i < cnt; i++)
    {
        same = _subt_ttx_ctrl_check_lang(p_dr_arr[i].language_code, p_pmt_dr->language_code);
        
        if(same && (p_dr_arr[i].type == p_pmt_dr->type))
        {
            return TRUE;
        }
    }

    return FALSE;
}
/*!
 * Ignore same subtitle
 */
static void _subt_ctrl_save_description(subt_ctrl_t *p_subt, pmt_t *p_pmt)
{
    u8   i     = 0;
    u8   cnt   = 0;
    BOOL equal = FALSE;
    
    cnt = MIN(p_pmt->subt_dr_cnt , DVB_MAX_SUBTILTE_DESC);

    p_subt->m_cnt = 0;

    for(i = 0; i < cnt; i++)
    {
      equal = _subt_ctrl_filter_same(p_subt->m_dr, p_subt->m_cnt, &p_pmt->subt_descr[i]);
      if(equal)
      {
          continue;
      }
      
      p_subt->m_dr[p_subt->m_cnt] = p_pmt->subt_descr[i];
      p_subt->m_cnt++;
    }

}
/*!
 * Save subtitle dr from pmt
 */
static RET_CODE _subt_ctrl_process(pmt_t *p_pmt)
{
    subt_ctrl_t          *p_subt = NULL;
    subt_ttx_ctrl_priv_t *p_this = NULL;

    do
    { 
        p_this = get_priv();
        ERR_POINT(p_this);

        p_subt = &p_this->m_subt_ctrl;

        /*!
         * Reset p_subt, maybe information in p_subt is not useful
         */
        if((NULL == p_pmt) || (0 == p_pmt->subt_dr_cnt))
        {
            _subt_ctrl_reset(p_subt);
            return SUCCESS;
        }
        
        /*!
         * Pmt info is under another task, so lock it
         */
        _subt_ttx_ctrl_lock(p_this);
        /*!
         * Copy subt dr from pmt
         */
        _subt_ctrl_save_description(p_subt, p_pmt);
        
        /*!
         * After subt_descr has been saved in p_subt then convert type
         */
        _subt_ctrl_convert_type(p_subt);
        _subt_ttx_ctrl_unlock(p_this);
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}


static subtitle_descr_t *_subt_ctrl_match_full_subt_dr(subt_ctrl_t *p_subt, 
                                                       u8 *p_lang, 
                                                       subt_ttx_type_t type)
{
    u8   i     = 0;
    BOOL equal = 0;

    for(i = 0; i < p_subt->m_cnt; i++)
    {
        equal = _subt_ttx_ctrl_check_lang(p_lang, p_subt->m_dr[i].language_code);

        if((type == p_subt->m_dr[i].type) && equal)
        {
            return &p_subt->m_dr[i];
        }
    }

    return NULL;
}

static teletext_descr_t *_subt_ctrl_match_full_ttx_dr(ttx_ctrl_t *p_ttx, 
                                                      u8 *p_lang, 
                                                      subt_ttx_type_t type)
{
    u8   i     = 0;
    BOOL equal = 0;

    for(i = 0; i < p_ttx->m_cnt; i++)
    {
        equal = _subt_ttx_ctrl_check_lang(p_lang, p_ttx->m_dr[i].language_code);

        if((type == p_ttx->m_dr[i].type) && equal)
        {
            return &p_ttx->m_dr[i];
        }
    }

    return NULL;
}

static void _subt_ctrl_get_ttx_dr(ttx_ctrl_t *p_ttx, subt_service_i_t *p_subt_service)
{
    u8 i = 0;
    
    for(i = 0; i < p_ttx->m_cnt; i++)
    {
        if(SUBT_CTRL_DR_MAX == p_subt_service->m_cnt)
        {
            break;
        }

        /*!
         * This two type of teletext are considered subtitle
         */
        if((E_SUBT_TYPE_TTX_NORMAL == p_ttx->m_dr[i].type)
          || (E_SUBT_TYPE_TTX_HEARING_HI == p_ttx->m_dr[i].type))
        {
            memcpy(p_subt_service->m_dr[p_subt_service->m_cnt].m_lang, 
                   p_ttx->m_dr[i].language_code, 
                   sizeof(p_ttx->m_dr[0].language_code));
               
            p_subt_service->m_dr[p_subt_service->m_cnt].m_type = p_ttx->m_dr[i].type;
            p_subt_service->m_dr[p_subt_service->m_cnt].pid = p_ttx->m_dr[i].pid;
            p_subt_service->m_cnt++;    
        }
    }
}

static void _subt_ctrl_get_subt_dr(subt_ctrl_t *p_subt, subt_service_i_t *p_subt_service)
{
    u8 i = 0;
    
    for(i = 0; i < p_subt->m_cnt; i++)
    {
        if(SUBT_CTRL_DR_MAX == p_subt_service->m_cnt)
        {
            break;
        }
        
        memcpy(p_subt_service->m_dr[i].m_lang, 
               p_subt->m_dr[i].language_code, 
               sizeof(p_subt->m_dr[0].language_code));
               
        p_subt_service->m_dr[i].m_type = p_subt->m_dr[i].type;
        p_subt_service->m_dr[i].pid = p_subt->m_dr[i].pid;
        
        p_subt_service->m_cnt++;
    }
}

static RET_CODE _subt_ctrl_get_type_and_lang(subt_ctrl_t *p_subt)
{
    u8               i            = 0;
    u8               idx          = 0xFF;
    BOOL             equal        = FALSE;
    subt_service_i_t subt_service = {0};
    
    subt_ctrl_get_description(&subt_service);

    if(0 == subt_service.m_cnt)
    {
        return ERR_FAILURE;
    }
    
    for(i = 0; i < subt_service.m_cnt; i++)
    {
        equal = _subt_ttx_ctrl_check_lang(p_subt->m_ui_lang, subt_service.m_dr[i].m_lang);

        if(FALSE == equal)
        {
            continue;
        }

        if(0xFF == idx)
        {
            idx = i;
        }
        
        if(p_subt->wait_type == subt_service.m_dr[i].m_type)
        {
            idx = i;
            break;
        }
    }

    /*!
     * NOTE:If not found type with same language, now use the first
     * language and type value has been changed here
     */

    idx = (0xFF == idx) ? 0 : idx; 
    
    p_subt->wait_type = subt_service.m_dr[idx].m_type;
    strcpy(p_subt->m_ui_lang, subt_service.m_dr[idx].m_lang);

    return SUCCESS;
}


static RET_CODE _subt_ctrl_show_subtitle(subt_ctrl_t *p_subt, subtitle_descr_t *p_subt_dr)
{
    RET_CODE ret = ERR_FAILURE;
    
    do
    {
        /*!
         * Hide subtitle, maybe teletext subtitle show last time, subtitle show this time 
         */
        subt_ctrl_hide();
              
        ret = mul_subtitle_start_decode(p_subt_dr->pid, 
                                        p_subt_dr->cmps_page_id, 
                                        p_subt_dr->ancl_page_id); 
        ERR_CHECK(ret);

        p_subt->m_subt_start = TRUE;

        return SUCCESS;
    }while(0);
    
    return ret;
}

static RET_CODE _subt_ctrl_show_teletext(subt_ttx_ctrl_priv_t *p_this, teletext_descr_t *p_ttx_dr)

{
    RET_CODE ret = ERR_FAILURE;
    
    do
    {
        subt_ctrl_hide();
        OS_PRINTF("lubin: start ttx from subt\n");
        ret = _ttx_ctrl_start(&p_this->m_ttx_ctrl, p_ttx_dr);
        ERR_CHECK(ret);
        ret = mul_teletext_display_teletext(NULL, TRUE, p_ttx_dr->magazien, p_ttx_dr->page);
        ERR_CHECK(ret);

        p_this->m_subt_ctrl.m_ttx_start = TRUE;
        
        return SUCCESS;
    }while(0);

    return ret;
}

RET_CODE _subt_ctrl_show(u8 *p_lang, subt_ttx_type_t type)
{
  RET_CODE              ret       = ERR_FAILURE;
  subt_ttx_ctrl_priv_t *p_this    = NULL;
  subtitle_descr_t     *p_subt_dr = NULL;
  teletext_descr_t     *p_ttx_dr  = NULL;
  
  do
  {
    ERR_POINT(p_lang);
    
    p_this = get_priv();
    ERR_POINT(p_this);

    strcpy(p_this->m_subt_ctrl.cur_lang, p_lang);
    p_this->m_subt_ctrl.cur_type = type;
    /*!
     * First show subtitle, if not available, show teletext subtitle
     */
    p_subt_dr = _subt_ctrl_match_full_subt_dr(&p_this->m_subt_ctrl, p_lang, type);
    if(NULL != p_subt_dr)
    {
      if(0 != memcmp(&p_this->m_subt_ctrl.cur_subt_dr, p_subt_dr, sizeof(subtitle_descr_t)))
      {
        memcpy(&p_this->m_subt_ctrl.cur_subt_dr, p_subt_dr, sizeof(subtitle_descr_t));
        return _subt_ctrl_show_subtitle(&p_this->m_subt_ctrl, p_subt_dr);
      }
    }

    if(p_this->enable_ttx_type_subt)
    {
      p_ttx_dr = _subt_ctrl_match_full_ttx_dr(&p_this->m_ttx_ctrl, p_lang, type);
      if(NULL != p_ttx_dr)
      {
        if(0 != memcmp(&p_this->m_subt_ctrl.cur_ttx_dr, p_ttx_dr, sizeof(teletext_descr_t)))
        {
          memcpy(&p_this->m_subt_ctrl.cur_ttx_dr, p_ttx_dr, sizeof(teletext_descr_t));
          return _subt_ctrl_show_teletext(p_this, p_ttx_dr);
        }
      }
    }
    return ret;        
  }while(0);

  return ret;
}

/*!
 * If subtitle set on, it will be shown when UI plays program, but now pmt may not be parsed
 * So set p_subt->m_ui_enable as true, when pmt has been parsed, show subtitle
 */
static void _subt_ctrl_auto(void)
{   
    subt_ctrl_t          *p_subt = NULL;
    subt_ttx_ctrl_priv_t *p_this = NULL;
     
    p_this = get_priv();
    if(NULL == p_this)
    {
        return;
    }

    p_subt = &p_this->m_subt_ctrl;

    if(FALSE == p_subt->enable)
    {
        return;
    }

    
    if(p_subt->m_subt_start || p_subt->m_ttx_start)
    {
        return;
    }

    _subt_ctrl_get_type_and_lang(p_subt);

    if(((E_SUBT_TYPE_TTX_NORMAL == p_subt->wait_type) 
      || (E_SUBT_TYPE_TTX_HEARING_HI == p_subt->wait_type)) 
      && (NULL != p_subt->ttx_type_subt_cb))
    {
        p_subt->ttx_type_subt_cb(0);
    }
    
    _subt_ctrl_show(p_subt->m_ui_lang, p_subt->wait_type);
}

static void _auto_notify(void)
{
  subt_ttx_ctrl_priv_t *p_this = get_priv();
  if(p_this->m_subt_ctrl.m_cnt > 0
    && p_this->subt_ready_cb != NULL)
  {
    p_this->subt_ready_cb(p_this->pg_id);
  }

  if(p_this->m_ttx_ctrl.m_cnt > 0)
  {
    p_this->ttx_ready_cb(p_this->pg_id);
  }
}

/*================================================================================================
                TTX interface for UI
=================================================================================================*/

RET_CODE ttx_ctrl_set_insert(BOOL enable)
{
  subt_ttx_ctrl_priv_t *p_this   = NULL;
  do
  {
    p_this = get_priv();
    ERR_POINT(p_this);

    if(!p_this->m_ttx_ctrl.m_start)
    {
      return ERR_FAILURE;
    }

    return mul_teletext_set_insert(enable);
  }while(0);
  return ERR_FAILURE;  
}

RET_CODE ttx_ctrl_send_key(mul_ttx_key_t key)
{
  subt_ttx_ctrl_priv_t *p_this   = NULL;
  do
  {
    p_this = get_priv();
    ERR_POINT(p_this);

    if(!p_this->m_ttx_ctrl.m_start)
    {
      return ERR_FAILURE;
    }

    return mul_teletext_send_key(key);
  }while(0);
  return ERR_FAILURE;  
}


RET_CODE ttx_ctrl_set_attr(mul_ttx_attr_t *p_attr)
{
  return mul_teletext_set_attr(p_attr);
}


RET_CODE ttx_ctrl_get_attr(mul_ttx_attr_t *p_attr)
{
  return mul_teletext_get_attr(p_attr);
}

/*!
  ttx_ctrl_show
  \param[in] lang_idx: language index of teletext
  \return SUCCESS if no error
  */  
RET_CODE ttx_ctrl_start(u8 *p_lang)
{
    RET_CODE              ret      = ERR_FAILURE;
    teletext_descr_t     *p_dr     = NULL;
    subt_ttx_ctrl_priv_t *p_this   = NULL;
    
    do
    {
        ERR_POINT(p_lang);
        
        p_this = get_priv();
        ERR_POINT(p_this);

        memcpy(p_this->m_ttx_ctrl.m_ui_lang, p_lang, SUBT_TTX_CTRL_LANG_LEN);
        p_dr = _ttx_ctrl_match_dr(&p_this->m_ttx_ctrl, p_lang);
//        ERR_POINT(p_dr);
        OS_PRINTF("lubin: ttx_ctrl_start 863\n");
        ret = _ttx_ctrl_start(&p_this->m_ttx_ctrl, p_dr);
        ERR_CHECK(ret);
        
        return ret;        
    }while(0);

    return ERR_FAILURE;
}

RET_CODE ttx_ctrl_show(void)
{
    RET_CODE              ret      = ERR_FAILURE;
    subt_ttx_ctrl_priv_t *p_this   = NULL;
    
    do
    {
        p_this = get_priv();
        ERR_POINT(p_this);

        if(p_this->m_ttx_ctrl.m_start != TRUE)
        {
          return ERR_FAILURE;
        }
        ret = mul_teletext_display_teletext(NULL, FALSE, 1, 0);
        ERR_CHECK(ret);
        return ret;        
    }while(0);

    return ERR_FAILURE;
}


/*!
  ttx_ctrl_hide
  \return SUCCESS if no error
  */  
RET_CODE ttx_ctrl_hide(void)
{
    //TODO:Need use hide interface
    return mul_teletext_hide_osd();
}
/*!
  ttx_ctrl_stop
  \return SUCCESS if no error
  */  
RET_CODE ttx_ctrl_stop(void)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;
    
    do
    {
        OS_PRINTF("lubin: stop ttx ---x\n");
        p_this = get_priv();
        ERR_POINT(p_this);
        OS_PRINTF("lubin: stop ttx ---2\n");
        if(!p_this->m_ttx_ctrl.m_start)
        {
          
          return ERR_FAILURE;
        }
        OS_PRINTF("lubin: stop ttx ---3\n");
        p_this->m_ttx_ctrl.m_start = FALSE;

        return mul_teletext_stop_decode();
    }while(0);

    return ERR_FAILURE;
}
/*!
  ttx_ctrl_get_dr
  \param[in] p_ttx_service: struct for same ttx language and type
  \return SUCCESS if no error
  */  
RET_CODE ttx_ctrl_get_description(ttx_service_i_t *p_ttx_service)
{
    u8                    i      = 0;
    BOOL                  valid  = FALSE;
    ttx_ctrl_t           *p_ttx  = NULL;
    subt_ttx_ctrl_priv_t *p_this = NULL;

    do
    {   
        ERR_POINT(p_ttx_service);

        p_this = get_priv();
        ERR_POINT(p_this);

        p_ttx = &p_this->m_ttx_ctrl;

        valid = _ttx_ctrl_check_page_valid(p_ttx);
        if(FALSE == valid)
        {
            return ERR_FAILURE;
        }

        for(i = 0; i < p_ttx->m_cnt; i++)
        {
            if(TTX_CTRL_DR_MAX == p_ttx_service->m_cnt)
            {
                break;
            }
            
            if(E_TTX_INITIAL_PAGE == p_ttx->m_dr[i].type)
            {
                memcpy(p_ttx_service->m_dr[p_ttx_service->m_cnt].m_lang, 
                       p_ttx->m_dr[i].language_code, 
                       sizeof(p_ttx->m_dr[0].language_code));
                       
                p_ttx_service->m_dr[p_ttx_service->m_cnt].m_type = p_ttx->m_dr[i].type;
                p_ttx_service->m_dr[p_ttx_service->m_cnt].pid = p_ttx->m_dr[i].pid;
                p_ttx_service->m_cnt++;
            }
        }
            
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;        
}
/*================================================================================================
                SUBT interface for UI
=================================================================================================*/
/*!
  subt_ctrl_hide
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_hide(void)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;
    
    do
    {
        p_this = get_priv();
        ERR_POINT(p_this);
        p_this->m_subt_ctrl.enable = FALSE;
        
        if(p_this->m_subt_ctrl.m_subt_start)
        {
            p_this->m_subt_ctrl.m_subt_start = FALSE;
            memset(&p_this->m_subt_ctrl.cur_subt_dr, 0, sizeof(subtitle_descr_t));
            return mul_subtitle_stop_decode();
        }    

        if(p_this->m_subt_ctrl.m_ttx_start)
        {   
            p_this->m_subt_ctrl.m_ttx_start = FALSE;
            memset(&p_this->m_subt_ctrl.cur_ttx_dr, 0, sizeof(teletext_descr_t));
            return ttx_ctrl_hide();
        }
        
    }while(0);

    return ERR_FAILURE;
}

/*!
  subt_ctrl_show
  \param[in] lang_id: language id of subtitle
  \param[in] type: subt type  
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_show(u8 *p_lang, subt_ttx_type_t type)
{
    subt_ttx_ctrl_priv_t *p_this    = get_priv();

    if(p_this == NULL)
    {
      return ERR_FAILURE;
    }

    p_this->m_subt_ctrl.enable = TRUE;
    
    _subt_ctrl_show(p_lang, type);

    return SUCCESS;
}
/*!
  subt_ctrl_get_attr
  \param[in] p_subt_attr: attr of subtitle  
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_get_attr(subt_attr_i_t *p_subt_attr)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;

    do
    {   
        ERR_POINT(p_subt_attr);
        
        p_this = get_priv();
        ERR_POINT(p_this);

        /*!
         * Pass the status to UI
         */
        p_subt_attr->m_start = p_this->m_subt_ctrl.m_ttx_start | p_this->m_subt_ctrl.m_subt_start;
        p_subt_attr->m_ttx_osd = p_this->m_subt_ctrl.m_ttx_osd;
        p_subt_attr->dmx_in = p_this->m_subt_ctrl.dmx_in;

        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
/*!
  subt_ctrl_set_attr
  \param[in] p_subt_attr: attr of subtitle  
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_set_attr(subt_attr_i_t *p_subt_attr)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;
    mul_subt_attr_t attr = {0};  
    do
    {   
        ERR_POINT(p_subt_attr);
        
        p_this = get_priv();
        ERR_POINT(p_this);

        p_this->m_subt_ctrl.m_ttx_osd = p_subt_attr->m_ttx_osd;
        p_this->m_subt_ctrl.dmx_in = p_subt_attr->dmx_in;
        
        attr.dmx_in = p_subt_attr->dmx_in;
        mul_subtitle_set_attr(&attr);
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
/*!
  subt_ctrl_get_dr
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_get_description(subt_service_i_t *p_subt_service)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;

    do
    {   
        ERR_POINT(p_subt_service);
        
        p_this = get_priv();
        ERR_POINT(p_this);

        _subt_ctrl_get_subt_dr(&p_this->m_subt_ctrl, p_subt_service);
        /*!
         * Get subtitle from ttx dr, if it is subtitle ttx
         */
        if(p_this->enable_ttx_type_subt)
        {
          _subt_ctrl_get_ttx_dr(&p_this->m_ttx_ctrl, p_subt_service);
        }
        return SUCCESS;
     }while(0);

     return ERR_FAILURE;
}


RET_CODE subt_ctrl_get_cur_descripition(char *p_lang, u8 *p_type)
{
  subt_ttx_ctrl_priv_t *p_this = NULL;

  p_this = get_priv();
  //ERR_POINT(p_this);
  if(p_this == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_lang != NULL)
  {
    strcpy(p_lang, p_this->m_subt_ctrl.cur_lang);
    *p_type = p_this->m_subt_ctrl.cur_type;
    return SUCCESS;
  }
  return ERR_FAILURE;
}


/*!
  subt_ctrl_ui_enable
  \param[in] p_lang: language for subtitle to show
  \return SUCCESS if no error
  */  
RET_CODE subt_ctrl_prepare_for_subt(u8 *p_lang, subt_ttx_type_t type, subt_ctrl_call_back pre_open)
{
    subt_ctrl_t          *p_subt  = NULL;
    subt_ttx_ctrl_priv_t *p_this  = NULL;

    do
    {   
        ERR_POINT(p_lang);
        
        p_this = get_priv();
        ERR_POINT(p_this);

        p_subt = &p_this->m_subt_ctrl;

        memcpy(p_subt->m_ui_lang, p_lang, SUBT_TTX_CTRL_LANG_LEN - 1);

        p_subt->wait_type = type;
        p_subt->enable = TRUE;
        p_subt->ttx_type_subt_cb = pre_open;

        return SUCCESS;
     }while(0);

     return ERR_FAILURE;
}

/*================================================================================================
                SUBT and TTX interface for UI
=================================================================================================*/
/*!
  subt_ttx_ctrl_set_dr
  \param[in] p_pmt: pmt data received from pmt.c
  \return SUCCESS if no error
  */  
RET_CODE subt_ttx_ctrl_set_pmt(void *p_pmt)
{
    RET_CODE ret     = ERR_FAILURE;       
    pmt_t    *p_data = (pmt_t *)p_pmt;

    do
    {
        ERR_POINT(p_data);
        
        ret = _subt_ctrl_process(p_data);
        ERR_CHECK(ret);
        
        ret = _ttx_ctrl_process(p_data);
        ERR_CHECK(ret);

        if(p_data != NULL)
        {
          _subt_ctrl_auto();
          _ttx_ctrl_auto();
          _auto_notify();
        }
        return SUCCESS;
     }while(0);

     return ret;
}


void subt_ttx_ctrl_set_pg_id(u16 pg_id)
{
  subt_ttx_ctrl_priv_t *p_this = NULL;

  p_this = get_priv();
  p_this->pg_id = pg_id;
}

void subt_ttx_ctrl_reset(void)
{
  subt_ttx_ctrl_priv_t *p_this = NULL;
  p_this = get_priv();
  
  _subt_ctrl_reset(&p_this->m_subt_ctrl);
  _ttx_ctrl_reset(&p_this->m_ttx_ctrl);
}

void init_subt_ttx_ctrl(subt_ttx_ctrl_init_t *p_param)
{
    subt_ttx_ctrl_priv_t *p_this = NULL;
    RET_CODE ret = SUCCESS;
    
    p_this = mtos_malloc(sizeof(subt_ttx_ctrl_priv_t));
    
    memset(p_this, 0, sizeof(subt_ttx_ctrl_priv_t));
    p_this->mutex_lock_mode = p_param->lock_mode;
    p_this->enable_ttx_type_subt = p_param->enable_ttx_type_subt;
    p_this->subt_ready_cb = p_param->subt_ready_cb;
    p_this->ttx_ready_cb = p_param->ttx_ready_cb;
    class_register(SUBT_TTX_CTRL_CLASS_ID, (class_handle_t)p_this);


    ret = mul_teletext_init(&p_param->ttx_para); 
    CHECK_FAIL_RET_VOID(ret == SUCCESS);

    ret = mul_subtitle_init(&p_param->subt_para); 
    CHECK_FAIL_RET_VOID(ret == SUCCESS);    
}
