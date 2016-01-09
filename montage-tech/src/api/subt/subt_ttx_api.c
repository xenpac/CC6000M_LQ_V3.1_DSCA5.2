/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

/*!
 *  HEAD FILE
 */
 
#include "stdio.h"
#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_mem.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_task.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"

#include "media_format_define.h"
#include "subt_ttx_api.h"

#include "class_factory.h"
#include "interface.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "dmx.h"
#include "demux_filter.h"
#include "subt_filter.h"
#include "ttx_filter.h"
#include "eva_filter_factory.h"
#include "pic_render_filter.h"

#include "service.h"
#include "mdl.h"
#include "mosaic.h"
#include "dvb_svc.h"
#include "pmt.h"
#include "drv_dev.h"
#include "common.h"
#include "region.h"
#include "display.h"
#include "hal_misc.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "flinger.h"
#include "gdi.h"

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

/*! 
 * Filter name length
 */
#define M_SUBT_TTX_NAME_LEN         40                        
      
/*!
 *  Subtitle and ttx display rect
 */
#define M_SUBT_DISPALY_W_MAX    720
#define M_SUBT_DISPALY_H_MAX    576

/*!
 * When input digit time is 3, set page_no
 */
#define M_TTX_SET_PAGE_CNT_MAX      3

/*!
 * TTX mag no max and min
 */
#define M_TTX_MAG_NO_MAX            8
#define M_TTX_MAG_NO_MIN            1

/*!
 * TTX page_no no max and min
 */
#define M_TTX_PAGE_NO_MAX           899
#define M_TTX_PAGE_NO_MIN           100

/*!
 * Hex to Dec use
 */
#define M_TTX_DEC_MULTI             10

/*!
 * Bit for move
 */
#define M_SUBT_TTX_MOVE_16_BIT      16
#define M_SUBT_TTX_MOVE_8_BIT       8


/*!
 *  Inner struct
 */

/*!
 * Inner state of chain, chain can't be open twice, so use state to control.
 */
typedef enum
{
    E_SUBT_TTX_STATE_START,
    E_SUBT_TTX_STATE_STOP,
    E_SUBT_TTX_STATE_MAX
}subt_ttx_state_t;

/*!
 * Base struct for subtitle and ttx
 */
typedef struct
{
    chain_t         *p_chain;
    ifilter_t       *p_dmx_filter;
    ifilter_t       *p_pic_filter;
    ifilter_t       *p_trans_filter;
    ipin_t          *p_dmx_out;
    controller_t     controller;
    subt_ttx_state_t state;
}subt_ttx_base_t;

/*!
 * Subttitle private data
 */
typedef struct
{
    subt_ttx_base_t       base;
    mul_subt_init_param_t input_param;
    u32 dmx_in;
}subt_priv_t;

/*!
 * TTX private data
 */
typedef struct
{
    subt_ttx_base_t      base; 
    mul_ttx_attr_t       attr;
    mul_ttx_init_param_t input_param;
    BOOL                 enable_insert;
}ttx_priv_t;

/*=============================================================================
               Below: Common function for subtitle and teletext
=============================================================================*/


/*!
 * Create controller
 */
static RET_CODE subt_ttx_create_controller(subt_ttx_base_t *p_base)
{
    ctrl_para_t   ctrl_para    = {0};
    controller_t *p_controller = NULL;

    do
    {
        ctrl_para.user_handle = p_base;
    
        p_controller = controller_create(&p_base->controller, &ctrl_para);

        ERR_POINT(p_controller);

        return SUCCESS;
    }while(0);
    
    return ERR_FAILURE;
}
/*!
 * Start chain
 */
static RET_CODE subt_ttx_start_chain(subt_ttx_base_t *p_base)
{
    RET_CODE ret = ERR_FAILURE;
    interface_id_t *p_ins = NULL;

    p_ins = (interface_id_t *)p_base->p_trans_filter;
    do
    {
        if(E_SUBT_TTX_STATE_START == p_base->state)
        {
            /*!
             * chain can't be opend twice, needn't start
             */
            OS_PRINTF("[%s]:Chain can't be start twice\n", p_ins->name);
            return SUCCESS;
        }
        
        ret = p_base->p_chain->open(p_base->p_chain);
        ERR_CHECK(ret);
        
        ret = p_base->p_chain->start(p_base->p_chain);
        ERR_CHECK(ret);

        p_base->state = E_SUBT_TTX_STATE_START;
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}

/*!
 * Stop chain
 */
static RET_CODE subt_ttx_stop_chain(subt_ttx_base_t *p_base)
{
    RET_CODE ret = ERR_FAILURE;
    interface_id_t *p_ins = NULL;

    p_ins = (interface_id_t *)p_base->p_trans_filter;

    do
    {
        if(E_SUBT_TTX_STATE_STOP == p_base->state)
        {
            /*!
             * chain can't be stop twice
             */
            OS_PRINTF("[%s]:Chain can't be stop twice\n", p_ins->name);
            return SUCCESS;
        }
        
        ret = p_base->p_chain->stop(p_base->p_chain);
        OS_PRINTF("chain stop ret %d\n", ret);
        ERR_CHECK(ret);
        
        ret = p_base->p_chain->close(p_base->p_chain);
        OS_PRINTF("chain close ret %d\n", ret);
        ERR_CHECK(ret);

        p_base->state = E_SUBT_TTX_STATE_STOP;
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}

/*!
 * Create chain
 */
static RET_CODE subt_ttx_create_chain(subt_ttx_base_t *p_base, 
                                      char            *p_name,
                                      u32              stk_size)
{
    chain_para_t para = {0};

    do
    {
        para.p_owner  = (interface_t *)&p_base->controller;
        para.p_name   = p_name;
        para.stk_size = stk_size;
         
        p_base->p_chain = chain_create(&para);

        ERR_POINT(p_base->p_chain); 

        /*!
         * Init state of chain
         */
        p_base->state = E_SUBT_TTX_STATE_STOP;

        return SUCCESS;
        
    }while(0);
    
    return ERR_FAILURE;
}

/*!
 * Create filter
 */
static RET_CODE subt_ttx_create_filter(subt_ttx_base_t *p_base, filter_id_t trans_id)
{
    RET_CODE ret = ERR_FAILURE;

    do
    {
        ret = eva_add_filter_by_id(DEMUX_FILTER, &p_base->p_dmx_filter, NULL);
        ERR_CHECK(ret);

        ret = eva_add_filter_by_id(trans_id, &p_base->p_trans_filter, NULL);
        ERR_CHECK(ret);

        ret = eva_add_filter_by_id(PIC_RENDER_FILTER, &p_base->p_pic_filter, NULL);
        ERR_CHECK(ret);

        return SUCCESS;
        
    }while(0);

    p_base->p_chain->_interface._destroy(p_base->p_chain);
    return ret;
}

/*!
 * Add filter to chain
 */
static RET_CODE subt_ttx_add_filter_to_chain(subt_ttx_base_t *p_base, char *prefix)
{
    RET_CODE  ret                       = ERR_FAILURE;
    chain_t  *p_chain                   = p_base->p_chain;
    char      name[M_SUBT_TTX_NAME_LEN] = {0};    
    
    do
    {
        sprintf(name, "%s_dmx_filter", prefix);
        ret = p_chain->add_filter(p_chain, p_base->p_dmx_filter, name);
        ERR_CHECK(ret);

        sprintf(name, "%s_trans_filter", prefix);
        ret = p_chain->add_filter(p_chain, p_base->p_trans_filter, name);
        ERR_CHECK(ret);

        sprintf(name, "%s_pic_filter", prefix);
        ret = p_chain->add_filter(p_chain, p_base->p_pic_filter, name);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);

    p_chain->_interface._destroy(p_chain);
    return ret;
}
/*!
 * Set dmx pin type (subt use pes type)
 */
static void subt_ttx_set_dmx_pin_type(ifilter_t *p_filter, ipin_t *p_pin)
{
    icmd_t icmd = {0};
    
    icmd.cmd    = DMX_SET_PIN_TYPE;
    icmd.p_para = p_pin;
    icmd.lpara  = DMX_PIN_PES_TYPE;
    
    p_filter->do_command(p_filter, &icmd);
}
/*!
 * Connect pin
 */
static RET_CODE subt_ttx_connect_pin(subt_ttx_base_t *p_base)
{
    RET_CODE        ret         = ERR_FAILURE;
    ifilter_t      *p_filter    = NULL;
    ipin_t         *p_trans_in  = NULL;
    ipin_t         *p_trans_out = NULL;
    ipin_t         *p_pic_in    = NULL;
    media_format_t  format      = {0};

    do
    {
        /*!
         *  Get unconnect pin of dmx
         */
        p_filter = p_base->p_dmx_filter;
        ret = p_filter->get_unconnect_pin(p_filter, OUTPUT_PIN, &p_base->p_dmx_out);
        ERR_POINT(p_base->p_dmx_out);

        subt_ttx_set_dmx_pin_type(p_filter, p_base->p_dmx_out);
        
        /*!
         * Get unconnect input and out pin of subt
         */
        p_filter = p_base->p_trans_filter;
        ret = p_filter->get_unconnect_pin(p_filter, INPUT_PIN, &p_trans_in);
        ERR_POINT(p_trans_in);
        
        ret = p_filter->get_unconnect_pin(p_filter, OUTPUT_PIN, &p_trans_out);
        ERR_POINT(p_trans_out);

        /*!
         * Get unconnect input pin of pic
         */
        p_filter = p_base->p_pic_filter;
        ret = p_filter->get_unconnect_pin(p_filter, INPUT_PIN, &p_pic_in);
        ERR_POINT(p_pic_in);

        /*!
         * Connect output pin and input pin
         */
        format.stream_type = MT_PES;
        ret = p_base->p_chain->connect_pin(p_base->p_chain, 
                                             p_base->p_dmx_out, 
                                             p_trans_in, 
                                             &format);
        ERR_CHECK(ret);

        format.stream_type = MT_IMAGE;
        ret = p_base->p_chain->connect_pin(p_base->p_chain, 
                                             p_trans_out, 
                                             p_pic_in, 
                                             &format);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);

    p_base->p_chain->_interface._destroy(p_base->p_chain);
    return ret;    
}

/*!
 * Set pid for subfilter or ttx
 */
static void subt_ttx_set_pid(ifilter_t *p_filter, u32 cmd, u16 pid)
{
    icmd_t icmd = {0};

    icmd.cmd   = cmd;
    icmd.lpara = pid;
    
    p_filter->do_command(p_filter, &icmd);
}

static void ttx_filter_enable_show(ifilter_t *p_filter, BOOL show)
{
    icmd_t icmd = {0};

    icmd.cmd   = TTX_FILTER_CMD_ENABLE_SHOW;
    icmd.lpara = show;
    
    p_filter->do_command(p_filter, &icmd);

}

/*=============================================================================
                    Below: Implement for teletext
=============================================================================*/
/*!
 * Set teletext rect for pic filter
 */
static void teletext_set_pic_cfg(ifilter_t *p_pic_filter, ifilter_t *p_trans_filter, void *p_rgn)
{
    u16             rect_h  = 0;
    u16             rect_w  = 0;  
    rect_size_t     rect_s  = {0};
    icmd_t          icmd    = {0};
    pic_rend_cfg_t  pic_cfg;

    memset(&pic_cfg, 0, sizeof(pic_cfg));

    rect_h = ttx_filter_calc_osd_height(p_trans_filter);
    rect_w = ttx_filter_calc_osd_width(p_trans_filter);
    
    region_get_rectsize(p_rgn, &rect_s);

    pic_cfg.p_rgn           = p_rgn;
    pic_cfg.win_rect.top    = (rect_s.h - rect_h) / 2;
    pic_cfg.win_rect.left   = (rect_s.w - rect_w) / 2;
    pic_cfg.win_rect.right  = rect_w + pic_cfg.win_rect.left;
    pic_cfg.win_rect.bottom = rect_h + pic_cfg.win_rect.top;

    icmd.cmd    = PIC_RENDER_CONFIG;
    icmd.p_para = (void *)&pic_cfg;
    
    p_pic_filter->do_command(p_pic_filter, &icmd);
}

/*!
 * Post color key to ttx filter
 */
static void teletext_post_color_key(ifilter_t *p_filter, u32 color)
{
    icmd_t icmd = {0};

    icmd.cmd   = TTX_FILTER_CMD_COLOR_KEY;
    icmd.lpara = color;
    
    p_filter->do_command(p_filter, &icmd);
}
/*!
 * Set ttx page_no number
 */
static void teletext_set_page_no(ifilter_t *p_filter, int page_no)
{
  icmd_t icmd = {0};

  icmd.cmd   = TTX_FILTER_CMD_SET_PAGE;
  icmd.lpara = page_no << M_SUBT_TTX_MOVE_16_BIT; 
  
  p_filter->do_command(p_filter, &icmd);
}
/*!
 * Set ttx type 1 is subtitle, 0 is teletext
 */
static void teletext_set_type(ifilter_t *p_filter, int type)
{
  icmd_t icmd = {0};

  icmd.cmd   = TTX_FILTER_CMD_SET_TTX_TYPE;
  icmd.lpara = type; 
  
  p_filter->do_command(p_filter, &icmd);
}

/*!
 * Set cmd
 */
static void teletext_send_cmd_to_filter(ifilter_t *p_filter, u16 cmd)
{
    icmd_t icmd = {0};

    icmd.cmd = cmd;
    
    p_filter->do_command(p_filter, &icmd);
}
/*!
 * Get current page_no which used to set next/prev page_no
 */
static u16 teletext_get_cur_page(ifilter_t *p_filter)
{
    u16    page_no = 0;
    icmd_t icmd    = {0};

    icmd.cmd = TTX_FILTER_CMD_GET_CUR_PAGE;
    
    p_filter->do_command(p_filter, &icmd);
    
    page_no = icmd.lpara >> M_SUBT_TTX_MOVE_16_BIT;

    return page_no;
}
/*!
 * Combine page number and magazine number
 */
static u32 teletext_combine_page_and_maga_no(u32 page_no, u8 maga_no)
{
    u16 maga_des = 0;
    
    /*!
     * Get the units of maga_no
     */
    maga_des = maga_no % M_TTX_DEC_MULTI;
    maga_des = maga_des * M_TTX_DEC_MULTI * M_TTX_DEC_MULTI;

    page_no = page_no % (M_TTX_DEC_MULTI * M_TTX_DEC_MULTI);

    /*!
     * If page_no = 19, maga_no = 8, return 819
     */
    page_no = maga_des + page_no;

    return page_no;
}

/*!
 * Split page number and magazine number
 */
static void teletext_split_page_and_maga_no(u32 page_src, 
                                            u32 *p_page_no, 
                                            u8 *p_maga_no)
{
    u8 maga_no = 0;
    /*!
     * If page_src = 819, *p_maga_no = 8, *p_page_no = 19
     */
     
    *p_page_no = page_src % (M_TTX_DEC_MULTI * M_TTX_DEC_MULTI);
    
    maga_no = page_src / (M_TTX_DEC_MULTI * M_TTX_DEC_MULTI);

    /*!
     * Get the units of maga_no
     */
    *p_maga_no = maga_no % M_TTX_DEC_MULTI;
}
/*!
 * up key process, increase page_no number
 */
static void teletext_proc_up_key(ifilter_t *p_filter)
{
    u32 page_no = 0;

    page_no = teletext_get_cur_page(p_filter);

    if(M_TTX_PAGE_NO_MAX <= page_no)
    {
        page_no = M_TTX_PAGE_NO_MIN;
    }
    else
    {
        page_no++;    
    }

    teletext_set_page_no(p_filter, page_no);
}
/*!
 * Down key process, decrease page_no number
 */
static void teletext_proc_down_key(ifilter_t *p_filter)
{
    u32 page_no = 0;
    
    page_no = teletext_get_cur_page(p_filter);
    
    if(M_TTX_PAGE_NO_MIN >= page_no)
    {
        page_no = M_TTX_PAGE_NO_MAX;
    }
    else
    {
        page_no--;    
    }

    teletext_set_page_no(p_filter, page_no);
}
/*!
 * Process down key, increase magazine number
 */
static void teletext_proc_page_up_key(ifilter_t *p_filter)
{
    u8  mag_no   = 0;
    u32 page_no  = 0;
    u32 page_src = 0;
    
    page_src = teletext_get_cur_page(p_filter);
    teletext_split_page_and_maga_no(page_src, &page_no, &mag_no);

    /*!
     * mag_no is in 1~8
     */
    if(M_TTX_MAG_NO_MAX <= mag_no)
    {
        mag_no = M_TTX_MAG_NO_MIN;
    }
    else
    {
        mag_no++;
    }

    page_src = teletext_combine_page_and_maga_no(page_no, mag_no);
    teletext_set_page_no(p_filter, page_src);
}
/*!
 * Process page_no down key, decrease magazine number
 */
static void teletext_proc_page_down_key(ifilter_t *p_filter)
{
    u8  mag_no   = 0;
    u32 page_no  = 0;
    u32 page_src = 0;
    
    page_src = teletext_get_cur_page(p_filter);
    teletext_split_page_and_maga_no(page_src, &page_no, &mag_no);

    /*!
     * mag_no is in 1~8
     */
    if(M_TTX_MAG_NO_MIN >= mag_no)
    {
        mag_no = M_TTX_MAG_NO_MAX;
    }
    else
    {
        mag_no--;
    }     
    
    page_src = teletext_combine_page_and_maga_no(page_no, mag_no);
    teletext_set_page_no(p_filter, page_src);
}
/*!
 * Show the page_no number when user presses digit key to set page_no number
 */
static void teletext_show_page_no(ifilter_t *p_filter, u16 page_no)
{
  icmd_t icmd = {0};

  icmd.cmd   = TTX_FILTER_CMD_SHOW_PAGE_NO;
  icmd.lpara = page_no << M_SUBT_TTX_MOVE_16_BIT;
  
  p_filter->do_command(p_filter, &icmd);
}
/*!
 * Process digit key, use presses digit key to set page_no number
 */
static void teletext_proc_digit_key(ifilter_t *p_filter, mul_ttx_key_t key)
{
    static u8  cnt     = 0;
    static u16 page_no = 0;

    /*!
     * Page no is in 100~899, the first no can't be 1 or 9
     */
    if(((MUL_TTX_KEY_0 == key) || (MUL_TTX_KEY_9 == key)) && (0 == cnt))
    {
        return;
    }

    /*!
     * Defect:If user inputs digit two times and stops ttx, next time the count
     * and page number will be remembered when teletext is opened
     */
    page_no = (page_no * M_TTX_DEC_MULTI) + (key - MUL_TTX_KEY_0);
    teletext_show_page_no(p_filter, page_no);
    
    cnt++;

    /*!
     * If time of input digit is 3, set page_no no
     */
    if(M_TTX_SET_PAGE_CNT_MAX <= cnt)
    {
        teletext_set_page_no(p_filter, page_no);
        cnt = 0;
        page_no = 0;
    }
}
/*!
 * Release ttx_priv_t data, and unregister TTX_API_CLASS_ID
 */
static void teletext_release_priv(ttx_priv_t *p_ttx_priv)
{
    /*!
     * Register NULL means unregister
     */
    class_register(TTX_API_CLASS_ID, NULL);
    mtos_free(p_ttx_priv);
}
/*!
 * teletext init
 */
static RET_CODE teletext_init(ttx_priv_t *p_ttx_priv)
{
    RET_CODE ret = ERR_FAILURE;
    
    do
    {
        /*!
         * Create controller
         */
        ret = subt_ttx_create_controller(&p_ttx_priv->base);
        ERR_CHECK(ret);

        /*!
         * Create chain
         */
        ret = subt_ttx_create_chain(&p_ttx_priv->base, 
                                    "ttx_chain", 
                                    p_ttx_priv->input_param.ttx_stk_size);
        ERR_CHECK(ret);
        
        /*!
         * Create filter
         */
        ret = subt_ttx_create_filter(&p_ttx_priv->base, TTX_FILTER);
        ERR_CHECK(ret);

        /*!
         * Add filter to chain
         */
        ret = subt_ttx_add_filter_to_chain(&p_ttx_priv->base, "ttx");
        ERR_CHECK(ret);

        /*!
         * Get unconnect pins of source/trans/render filter, and connect them
         */
        ret = subt_ttx_connect_pin(&p_ttx_priv->base);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);
    
    return ret;
}
/*!
 *Allocate memory for "ttx_priv_t" structure and register with ID
 */
static ttx_priv_t * teletext_alloc_priv(void)
{
    ttx_priv_t *p_ttx_priv = NULL;

    do
    {
        p_ttx_priv = (ttx_priv_t *)mtos_malloc(sizeof(ttx_priv_t));
        ERR_POINT(p_ttx_priv);

        memset(p_ttx_priv, 0x00, sizeof(ttx_priv_t));
        class_register(TTX_API_CLASS_ID, p_ttx_priv);
    
        return p_ttx_priv;
    }while(0);

    return NULL;      
}
/*=============================================================================
                    Below: Implement for Subtitle
=============================================================================*/
/*!
 * Set subtitle rect for pic filter
 */
static void subtitle_set_pic_cfg(ifilter_t *p_pic_filter)
{
    icmd_t          icmd    = {0};
    pic_rend_cfg_t  pic_cfg;

    memset(&pic_cfg, 0, sizeof(pic_cfg));
    
    pic_cfg.win_rect.top    = 0;
    pic_cfg.win_rect.left   = 0;
    pic_cfg.win_rect.right  = M_SUBT_DISPALY_W_MAX;
    pic_cfg.win_rect.bottom = M_SUBT_DISPALY_H_MAX;

    icmd.cmd    = PIC_RENDER_CONFIG;
    icmd.p_para = (void *)&pic_cfg;
    
    p_pic_filter->do_command(p_pic_filter, &icmd);
}
/*!
 * Set layer id for subfilter
 */
static void subtitle_set_layer_id(ifilter_t *p_subt_filter, u16 id)
{
    icmd_t                   icmd = {0};
    subt_filter_params_cfg_t cfg  = {0};

    cfg.layer_id = id;
    icmd.cmd     = SUBT_FILTER_CMD_CFG_PARAMS;
    icmd.p_para  = (void *)&cfg;
    
    p_subt_filter->do_command(p_subt_filter, &icmd);
}



static void subtitle_set_dmx_config(subt_ttx_base_t *p_base, u16 dmx_in)
{
    icmd_t                   icmd = {0};

    icmd.cmd     = DMX_CFG_PARA;
    icmd.p_para  = (void *)p_base->p_dmx_out;
    icmd.lpara = (u32)dmx_in;

    p_base->p_dmx_filter->do_command(p_base->p_dmx_filter, &icmd);
}

/*!
 * Set page_no for subfilter
 */
static void subtitle_set_page(ifilter_t *p_subt_filter, u16 c_page, u8 a_page)
{
  icmd_t icmd = {0};

  icmd.cmd   = SUBT_FILTER_CMD_SET_PAGE;
  icmd.lpara = (c_page << M_SUBT_TTX_MOVE_16_BIT) | a_page;
  
  p_subt_filter->do_command(p_subt_filter, &icmd);
}

/*!
 * Release subt_priv_t data, and unregister SUBT_API_CLASS_ID
 */
static void subtitle_release_priv(subt_priv_t *p_subt_priv)
{
    /*!
     * Register NULL means unregister
     */
    class_register(SUBT_API_CLASS_ID, NULL);
    mtos_free(p_subt_priv);
}
/*!
 * Subt init
 */
static RET_CODE subtitle_init(subt_priv_t *p_subt_priv)
{
    RET_CODE ret = ERR_FAILURE;
    
    do
    {
        /*!
         * Create controller
         */
        ret = subt_ttx_create_controller(&p_subt_priv->base);
        ERR_CHECK(ret);

        /*!
         * Create chain
         */
        ret = subt_ttx_create_chain(&p_subt_priv->base, 
                                    "subt_chain", 
                                    p_subt_priv->input_param.subt_stk_size);
        ERR_CHECK(ret);
        
        /*!
         * Create filter
         */
        ret = subt_ttx_create_filter(&p_subt_priv->base, SUBT_FILTER);
        ERR_CHECK(ret);

        /*!
         * Add filter to chain
         */
        ret = subt_ttx_add_filter_to_chain(&p_subt_priv->base, "subt");
        ERR_CHECK(ret);

        /*!
         * Get unconnect pins of source/trans/render filter, and connect them
         */
        ret = subt_ttx_connect_pin(&p_subt_priv->base);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);
    
    return ret;
}
/*!
 *Allocate a memory for "subt_ttx_priv_t" structure and register with ID
 */
static subt_priv_t * subtitle_alloc_priv(void)
{
    subt_priv_t *p_subt_priv = NULL;

    do
    {
        p_subt_priv = (subt_priv_t *)mtos_malloc(sizeof(subt_priv_t));
        ERR_POINT(p_subt_priv);

        memset(p_subt_priv, 0x00, sizeof(subt_priv_t));
        class_register(SUBT_API_CLASS_ID, p_subt_priv);
    
        return p_subt_priv;
    }while(0);

    return NULL;      
}
/*=============================================================================
                    Below: Teletext API for UI
=============================================================================*/
/*!
  mul_teletext_set_insert
  \param[in] enable: enable insert or not
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_set_insert(BOOL enable)
{
    u16         cmd        = 0;
    ttx_priv_t *p_ttx_priv = NULL;
    
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);
        /*!
         * Check insert is enable or not
         */
        cmd = enable ? TTX_FILTER_CMD_START_INSERT : TTX_FILTER_CMD_STOP_INSERT;
      
        teletext_send_cmd_to_filter(p_ttx_priv->base.p_trans_filter, cmd);

        p_ttx_priv->enable_insert = enable;
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}

/*!
  mul_subtitle_start_decode
  \param[in] txt_pid: teletelxt pid from pmt
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_start_decode(u16 txt_pid)
{
    RET_CODE    ret            = ERR_FAILURE;
    ttx_priv_t *p_ttx_priv     = NULL;
   // ifilter_t  *p_pic_filter   = NULL;
    ifilter_t  *p_trans_filter = NULL;
    
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        /*!
         * Start chain
         */
        ret = subt_ttx_start_chain(&p_ttx_priv->base);
        ERR_CHECK(ret);

        /*!
         * Pass the regin and other configration to pic filter
         */
     //   p_pic_filter = p_ttx_priv->base.p_pic_filter; 

        p_trans_filter = p_ttx_priv->base.p_trans_filter;  
     //   teletext_set_pic_cfg(p_pic_filter, p_trans_filter, (void *)p_ttx_priv->attr.ttx_region);

        /*!
         * Set start decode state for ttx filter
         */
        teletext_send_cmd_to_filter(p_trans_filter, TTX_FILTER_CMD_START_DECODE);

        /*!
         * Set pid for decode
         */
         if(txt_pid != 0)
         {
          subt_ttx_set_pid(p_trans_filter, TTX_FILTER_CMD_SET_PID, txt_pid);
         }
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
/*!
  mul_teletext_reset_pid
  \param[in] txt_pid: teletelxt pid from pmt
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_reset_pid(u16 txt_pid)
{
    ttx_priv_t *p_ttx_priv = NULL;
    ifilter_t  *p_filter   = NULL;
    
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        /*!
         * Set pid for decode
         */
        p_filter = p_ttx_priv->base.p_trans_filter;  
        subt_ttx_set_pid(p_filter, TTX_FILTER_CMD_SET_PID, txt_pid);

        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
/*!
  mul_teletext_stop_decode
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_stop_decode(void)
{
    RET_CODE    ret        = ERR_FAILURE;
    ttx_priv_t *p_ttx_priv = NULL;
    ifilter_t  *p_filter   = NULL;
    
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);
        /*!
         * Set stop decode state for ttx filter
         */
        p_filter = p_ttx_priv->base.p_trans_filter;
        teletext_send_cmd_to_filter(p_filter, TTX_FILTER_CMD_STOP_DECODE);
        OS_PRINTF("stop decoder done\n");
        /*!
         * Stop chain
         */
        ret = subt_ttx_stop_chain(&p_ttx_priv->base);
        OS_PRINTF("stop CHAIN done %d\n", ret);
        ERR_CHECK(ret);
        
        return SUCCESS;
    }while(0);
    
    return ERR_FAILURE;
}
/*!
  mul_teletext_send_key
  \param[in] key: see mul_ttx_key_t
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_send_key(mul_ttx_key_t key)
{   
    ifilter_t  *p_filter   = NULL;
    ttx_priv_t *p_ttx_priv = NULL;
    
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        p_filter = p_ttx_priv->base.p_trans_filter;

        switch(key)
        {
            case MUL_TTX_KEY_RED:
            {
                teletext_post_color_key(p_filter, TTX_FILTER_KEY_RED);
                break;
            }
            case MUL_TTX_KEY_GREEN:
            {
                teletext_post_color_key(p_filter, TTX_FILTER_KEY_GREEN);
                break;
            }
            
            case MUL_TTX_KEY_YELLOW:
            {
                teletext_post_color_key(p_filter, TTX_FILTER_KEY_YELLOW);
                break;
            }
            case MUL_TTX_KEY_CYAN:
            {
                teletext_post_color_key(p_filter, TTX_FILTER_KEY_CYAN);
                break;
            }
            case MUL_TTX_KEY_LEFT:
            {
                teletext_send_cmd_to_filter(p_filter, TTX_FILTER_CMD_PREV_SUB);
                break;
            }
            case MUL_TTX_KEY_RIGHT:
            {
                teletext_send_cmd_to_filter(p_filter, TTX_FILTER_CMD_NEXT_SUB);
                break;
            }
            case MUL_TTX_KEY_UP:
            {
                teletext_proc_up_key(p_filter);
                break;
            }
            case MUL_TTX_KEY_DOWN:
            {
                teletext_proc_down_key(p_filter);
                break;
            }
            case MUL_TTX_KEY_PAGE_UP:
            {
                teletext_proc_page_up_key(p_filter);
                break;
            }
            case MUL_TTX_KEY_PAGE_DOWN:
            {
                teletext_proc_page_down_key(p_filter);
                break;
            }  
            case MUL_TTX_KEY_0:
            case MUL_TTX_KEY_1:
            case MUL_TTX_KEY_2:
            case MUL_TTX_KEY_3:
            case MUL_TTX_KEY_4:
            case MUL_TTX_KEY_5:
            case MUL_TTX_KEY_6:
            case MUL_TTX_KEY_7:
            case MUL_TTX_KEY_8:
            case MUL_TTX_KEY_9:
            {
                teletext_proc_digit_key(p_filter, key);
                break;
            }
            case MUL_TTX_KEY_CANCEL:
              OS_PRINTF("get cancel key\n");
              break;
            case MUL_TTX_KEY_TRANSPARENT:
              OS_PRINTF("get trans key\n");
              teletext_send_cmd_to_filter(p_filter, TTX_FILTER_CMD_SET_TRANSPARENT);
            default:
            {
                break;
            }
        }
        
        return SUCCESS;
    }while(0);
    
    return ERR_FAILURE;
}

/*!
  mul_teletext_display_teletext
  \param[in] txt_pid: teletelxt pid from pmt
  \param[in] p_ttx_lang: language code string
  \param[in] type: from pmt
  \param[in] magazine_num: see 300706 chapter3.1
  \param[in] page_num: pt. pu see 300706 chapter3.1
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_display_teletext(unsigned char *p_ttx_lang,
                                       int type, 
                                       int magazine_num, 
                                       int page_num)
{
    u32         page_no    = 0;
    ttx_priv_t *p_ttx_priv = NULL; 
    ifilter_t  *p_filter   = NULL;
    ifilter_t  *p_pic_filter   = NULL;
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        /*!
         * Some ttx_subtitle doesn't have magzine number, but in fact it is in mag 8 
         */
        if(0 == magazine_num)
        {
            magazine_num = 8;
        }

        page_no = bcd_number_to_dec((magazine_num << 8) + page_num);

        p_filter = p_ttx_priv->base.p_trans_filter;
        /*!
         * Pass the regin and other configration to pic filter
         */
        p_pic_filter = p_ttx_priv->base.p_pic_filter; 

        p_filter = p_ttx_priv->base.p_trans_filter;  
        teletext_set_pic_cfg(p_pic_filter, p_filter, (void *)p_ttx_priv->attr.ttx_region);

        ttx_filter_enable_show(p_filter, TRUE);
        teletext_set_type(p_filter, type);
        teletext_set_page_no(p_filter, page_no);
          
        return SUCCESS;
    }while(0);
    
    return ERR_FAILURE;
}        

/*!
  mul_teletext_hide_osd
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_hide_osd(void)
{
    ttx_priv_t *p_ttx_priv = NULL; 
    ifilter_t  *p_filter   = NULL;
    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        //TODO:Use pic filter cmd to hide osd
        p_filter = p_ttx_priv->base.p_trans_filter;
        ttx_filter_enable_show(p_filter, FALSE);
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}


/*!
  mul_teletext_get_attr
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_get_attr(mul_ttx_attr_t *p_attr)
{
    ttx_priv_t *p_ttx_priv = NULL;
    
    do
    {
        ERR_POINT(p_attr);
        
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        memcpy(p_attr, &p_ttx_priv->attr, sizeof(mul_ttx_attr_t));
        
        return SUCCESS;
    }while(0);
  
    return ERR_FAILURE;
}
/*!
  mul_teletext_set_attr
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_set_attr(mul_ttx_attr_t *p_attr)
{
    icmd_t                  icmd        = {0};
    ttx_priv_t              *p_ttx_priv = NULL;
    ttx_filter_params_cfg_t ttx_cfg     = {0};
    
    do
    {
        ERR_POINT(p_attr);
        
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        /*!
         * Save p_attr, use it in mul_teletext_get_attr function
         */
        memcpy(&p_ttx_priv->attr, p_attr, sizeof(mul_ttx_attr_t));

        ttx_cfg.level              = p_attr->ttx_level;
        ttx_cfg.font.font_h        = p_attr->font.font_h;
        ttx_cfg.font.font_w        = p_attr->font.font_w;
        ttx_cfg.font.p_ext_font    = p_attr->font.p_ext_font;
        ttx_cfg.font.p_std_font    = p_attr->font.p_std_font;
        ttx_cfg.max_page_cache     = p_attr->max_page_cache;
        ttx_cfg.max_sub_page_cache = p_attr->max_sub_page_cache;
        ttx_cfg.out_pin_task_prio = p_attr->out_pin_task_prio;
        ttx_cfg.out_pin_task_stack_size = p_attr->out_pin_task_stack_size;
         
        icmd.cmd    = TTX_FILTER_CMD_CFG_PARAMS;
        icmd.p_para = (void *)&ttx_cfg;
        
        p_ttx_priv->base.p_trans_filter->do_command(p_ttx_priv->base.p_trans_filter, &icmd);
        
        return SUCCESS;
    }while(0);
  
    return ERR_FAILURE;
}
/*!
  mul_subtitle_init
  \param[in] p_params: see mul_subt_init_param_t.
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_init(mul_ttx_init_param_t *p_para)
{
    RET_CODE   ret         = ERR_FAILURE;
    ttx_priv_t *p_ttx_priv = NULL;
    
    do
    {
        ERR_POINT(p_para);

        /*!
         * Alloc memory for subt private struct
         */
        p_ttx_priv = teletext_alloc_priv();
        ERR_POINT(p_ttx_priv);

        /*!
         * Save the param passed from UI
         */
        memcpy(&p_ttx_priv->input_param, p_para, sizeof(mul_ttx_init_param_t));

        /*!
         * Init subtitle
         */
        ret = teletext_init(p_ttx_priv);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);

    return ret;
}
/*!
  mul_subtitle_deinit
  \return SUCCESS if no error
  */
RET_CODE mul_teletext_deinit(void)
{
    ttx_priv_t *p_ttx_priv = NULL;
    chain_t    *p_chain    = NULL;

    do
    {
        p_ttx_priv = (ttx_priv_t *)class_get_handle_by_id(TTX_API_CLASS_ID);
        ERR_POINT(p_ttx_priv);

        p_chain = p_ttx_priv->base.p_chain;

        /*!
         * Release private struct and then destroy chain struct, 
         * in case p_chain is NULL, p_ttx_priv will not be free
         */
        teletext_release_priv(p_ttx_priv);
        p_ttx_priv = NULL;

        ERR_POINT(p_chain);
        p_chain->_interface._destroy(p_chain);   
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}

/*=============================================================================
                    Below: Subtitle API for UI
=============================================================================*/
/*!
  mul_subtitle_stop_decode
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_stop_decode(void)
{
    RET_CODE    ret          = ERR_FAILURE;
    subt_priv_t *p_subt_priv = NULL;
    icmd_t icmd = {0};
    
    do
    {
        p_subt_priv = (subt_priv_t *)class_get_handle_by_id(SUBT_API_CLASS_ID);
        ERR_POINT(p_subt_priv);

        /*!
         * Stop chain
         */


        icmd.cmd = SUBT_FILTER_CMD_STOP;
        
        p_subt_priv->base.p_trans_filter->do_command(p_subt_priv->base.p_trans_filter, &icmd);

        ret = subt_ttx_stop_chain(&p_subt_priv->base);
        ERR_CHECK(ret);
        
        return SUCCESS;
    }while(0);
    
    return ERR_FAILURE;
}

/*!
  mul_subtitle_start_decode
  \param[in] sub_pid: subtitle pid from pmt
  \param[in] composition_page_id: see EN300473 chapter 3.1
  \param[in] ancillary_page_id: see EN300473 chapter 3.1
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_start_decode(u16 sub_pid, 
                                   u16 composition_page_id, 
                                   u16 ancillary_page_id)
{
    RET_CODE        ret          = ERR_FAILURE;
    subt_priv_t     *p_subt_priv = NULL;
    subt_ttx_base_t *p_base      = NULL;
    
    do
    {
        p_subt_priv = (subt_priv_t *)class_get_handle_by_id(SUBT_API_CLASS_ID);
        ERR_POINT(p_subt_priv);

        p_base = &p_subt_priv->base;
        
        /*!
         * Start chain
         */
        ret = subt_ttx_start_chain(p_base);
        ERR_CHECK(ret);

        /*!
         * Set subtitle param
         */
        subtitle_set_dmx_config(p_base, p_subt_priv->dmx_in);
        subtitle_set_layer_id(p_base->p_trans_filter, p_subt_priv->input_param.subt_layer_id); 
        subt_ttx_set_pid(p_base->p_trans_filter, SUBT_FILTER_CMD_SET_PID, sub_pid); 
        subtitle_set_page(p_base->p_trans_filter, composition_page_id, ancillary_page_id);
        subtitle_set_pic_cfg(p_base->p_pic_filter);
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
/*!
  mul_subtitle_reset_pid
  \param[in] sub_pid: subtitle pid from pmt
  \param[in] composition_page_id: see EN300473 chapter 3.1
  \param[in] ancillary_page_id: see EN300473 chapter 3.1
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_reset_pid(u16 sub_pid, 
                                u16 composition_page_id, 
                                u16 ancillary_page_id)
{
    ifilter_t   *p_filter    = NULL;
    subt_priv_t *p_subt_priv = NULL;
    
    do
    {
        p_subt_priv = (subt_priv_t *)class_get_handle_by_id(SUBT_API_CLASS_ID);
        ERR_POINT(p_subt_priv);

        p_filter = p_subt_priv->base.p_trans_filter;

        /*!
         * Set subtitle param
         */
        subt_ttx_set_pid(p_filter, SUBT_FILTER_CMD_SET_PID, sub_pid); 
        subtitle_set_page(p_filter, composition_page_id, ancillary_page_id);
        
        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}


RET_CODE mul_subtitle_set_attr(mul_subt_attr_t *p_attr)
{
  subt_priv_t *p_subt_priv = NULL;

  p_subt_priv = (subt_priv_t *)class_get_handle_by_id(SUBT_API_CLASS_ID);
  p_subt_priv->dmx_in = p_attr->dmx_in;
  return SUCCESS;
}

/*!
  mul_subtitle_init
  \param[in] p_params: see mul_subt_init_param_t.
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_init(mul_subt_init_param_t *p_para)
{
    RET_CODE    ret          = ERR_FAILURE;
    subt_priv_t *p_subt_priv = NULL;
    
    do
    {
        ERR_POINT(p_para);

        /*!
         * Alloc memory for subt private struct
         */
        p_subt_priv = subtitle_alloc_priv();
        ERR_POINT(p_subt_priv);

        /*!
         * Save the param passed from UI
         */
        memcpy(&p_subt_priv->input_param, p_para, sizeof(mul_subt_init_param_t));

        /*!
         * Init subtitle
         */
        ret = subtitle_init(p_subt_priv);
        ERR_CHECK(ret);

        return SUCCESS;
    }while(0);

    return ret;
}
/*!
  mul_subtitle_deinit
  \return SUCCESS if no error
  */
RET_CODE mul_subtitle_deinit(void)
{
    subt_priv_t *p_subt_priv = NULL;
    chain_t     *p_chain     = NULL;
    
    do
    {
        p_subt_priv = (subt_priv_t *)class_get_handle_by_id(SUBT_API_CLASS_ID);
        ERR_POINT(p_subt_priv);
        
        p_chain = p_subt_priv->base.p_chain;

        /*!
         * Release private struct and then destroy chain struct, 
         * in case p_chain is NULL, p_subt_priv will not be free
         */
        subtitle_release_priv(p_subt_priv);
        p_subt_priv = NULL;

        ERR_POINT(p_chain);
        p_chain->_interface._destroy(p_chain);      

        return SUCCESS;
    }while(0);

    return ERR_FAILURE;
}
