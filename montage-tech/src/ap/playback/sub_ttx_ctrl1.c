/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "lib_util.h"
#include "iso_639_2.h"

#include "stdio.h"
#include "string.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "hal_dma.h"

#include "drv_dev.h"
#include "common.h"
#include "lib_rect.h"
#include "display.h"
#include "aud_vsb.h"
#include "vdec.h"
#include "dmx.h"
//#include "audio.h"
//#include "video.h"
//#include "pti.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "avsync.h"
//#include "osd.h"
#include "drv_misc.h"
//#include "sub.h"

#include "service.h"
#include "class_factory.h"
#include "mdl.h"
#include "err_check_def.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "nit.h"
#include "pmt.h"
#include "sdt.h"
#include "avctrl1.h"
#include "subtitle_api.h"
#include "vbi_api.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "dvbc_util.h"

#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_i.h"

typedef struct
{
  u8 subt_dr_cnt;
  subtitle_descr_t subt_dr[MAX_SUBT_DESCRIPTION];

  u8 ttx_dr_cnt;
  teletext_descr_t ttx_dr[MAX_TTX_DESCRIPTION];

  subtitle_descr_t curr_subt;
  teletext_descr_t curr_ttx_subt;
  teletext_descr_t curr_ttx;

  subtitle_descr_t *p_new_subt;
  teletext_descr_t *p_new_ttx_subt;
  teletext_descr_t *p_new_ttx;

  BOOL is_ttx_play;
  BOOL is_subt_dec_on;
  BOOL is_subt_play;

  BOOL is_vbi_inserter_on;

  BOOL is_scart_vcr_detected;

  u8 pref_ttx_lang_b[3];
  u8 pref_ttx_lang_t[3];
  u8 pref_subt_lang_b[3];
  u8 pref_subt_lang_t[3];
  subt_type_t pref_subt_type;

  u8 curr_ttx_lang[3];
  u8 curr_subt_lang[3];
  subt_type_t curr_subt_type;
  
  u16 cur_pg_id;
} sub_ttx_priv_t;

handle_t init_sub_ttx_1(void)
{
  sub_ttx_priv_t *p_data = mtos_malloc(sizeof(sub_ttx_priv_t));
  memset(p_data, 0, sizeof(sub_ttx_priv_t));

  class_register(VBI_SUBT_CTRL_CLASS_ID, (class_handle_t)p_data);
  return p_data;
}

static void select_subt_service_1(sub_ttx_priv_t *p_data)
{
  s32 i = 0;
  subtitle_descr_t *p_subt_dr = NULL;
  teletext_descr_t *p_ttx_dr = NULL;
  BOOL is_ttx_subtitle = FALSE;
  BOOL is_find = FALSE;


  /*
    *	STEP 1
    */
    while(!is_find && p_data->is_subt_dec_on == TRUE)
    {
        for(i = 0, p_subt_dr = p_data->subt_dr
            ; i < p_data->subt_dr_cnt
            ; i++, p_subt_dr++)
        {
            if(((p_data->pref_subt_lang_b[0] == p_subt_dr->language_code[0]
                && p_data->pref_subt_lang_b[1] == p_subt_dr->language_code[1]
                && p_data->pref_subt_lang_b[2] == p_subt_dr->language_code[2])
                || (p_data->pref_subt_lang_t[0] == p_subt_dr->language_code[0]
                && p_data->pref_subt_lang_t[1] == p_subt_dr->language_code[1]
                && p_data->pref_subt_lang_t[2] == p_subt_dr->language_code[2]))
                && (p_data->pref_subt_type == p_subt_dr->type))
            {
                break;
            }
        }

        if(i >= p_data->subt_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = FALSE;
    }


    while(!is_find)
    {
        for(i = 0, p_ttx_dr = p_data->ttx_dr
            ; i < p_data->ttx_dr_cnt
            ; i++, p_ttx_dr++)
        {
            if(((p_data->pref_subt_lang_b[0] == p_ttx_dr->language_code[0]
                && p_data->pref_subt_lang_b[1] == p_ttx_dr->language_code[1]
                && p_data->pref_subt_lang_b[2] == p_ttx_dr->language_code[2])
                || (p_data->pref_subt_lang_t[0] == p_ttx_dr->language_code[0]
                && p_data->pref_subt_lang_t[1] == p_ttx_dr->language_code[1]
                && p_data->pref_subt_lang_t[2] == p_ttx_dr->language_code[2]))
                && (p_data->pref_subt_type == p_ttx_dr->type))
            {
                break;
            }
        }

        if(i >= p_data->ttx_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = TRUE;
    }

    /*
    *	STEP 2
    */
    while(!is_find && p_data->is_subt_dec_on == TRUE)
    {
        for(i = 0, p_subt_dr = p_data->subt_dr
            ; i < p_data->subt_dr_cnt
            ; i++, p_subt_dr++)
        {
            if((p_data->pref_subt_lang_b[0] == p_subt_dr->language_code[0]
                && p_data->pref_subt_lang_b[1] == p_subt_dr->language_code[1]
                && p_data->pref_subt_lang_b[2] == p_subt_dr->language_code[2])
                || (p_data->pref_subt_lang_t[0] == p_subt_dr->language_code[0]
                && p_data->pref_subt_lang_t[1] == p_subt_dr->language_code[1]
                && p_data->pref_subt_lang_t[2] == p_subt_dr->language_code[2]))
            {
                break;
            }
        }

        if(i >= p_data->subt_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = FALSE;
    }

    while(!is_find)
    {
        for(i = 0, p_ttx_dr = p_data->ttx_dr
            ; i < p_data->ttx_dr_cnt
            ; i++, p_ttx_dr++)
        {
            /*
            *	type = 2:	Teletext subtitle page
            *	type = 5:	Teletext subtitle page for hearing
            *impaired people
            *	other		non-teletext subtitle
            */
            if(((p_data->pref_subt_lang_b[0] == p_ttx_dr->language_code[0]
                && p_data->pref_subt_lang_b[1] == p_ttx_dr->language_code[1]
                && p_data->pref_subt_lang_b[2] == p_ttx_dr->language_code[2])
                || (p_data->pref_subt_lang_t[0] == p_ttx_dr->language_code[0]
                && p_data->pref_subt_lang_t[1] == p_ttx_dr->language_code[1]
                && p_data->pref_subt_lang_t[2] == p_ttx_dr->language_code[2]))
                && (p_ttx_dr->type == SUBT_TYPE_TTX_NORMAL
                    || p_ttx_dr->type == SUBT_TYPE_TTX_HEARING_HI))
            {
                break;
            }
        }

        if(i >= p_data->ttx_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = TRUE;
    }

    /*
    *	STEP 3
    */
    while(!is_find && p_data->is_subt_dec_on == TRUE)
    {
        for(i = 0, p_subt_dr = p_data->subt_dr
            ; i < p_data->subt_dr_cnt
            ; i++, p_subt_dr++)
        {
            if(p_data->pref_subt_type == p_subt_dr->type)
            {
                break;
            }
        }

        if(i >= p_data->subt_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = FALSE;
    }


    while(!is_find)
    {
        for(i = 0, p_ttx_dr = p_data->ttx_dr
            ; i < p_data->ttx_dr_cnt
            ; i++, p_ttx_dr++)
        {
            if(p_data->pref_subt_type == p_ttx_dr->type)
            {
                break;
            }
        }

        if(i >= p_data->ttx_dr_cnt)
        {
            break;
        }

        is_find         = TRUE;
        is_ttx_subtitle = TRUE;
    }

    /*
    *	STEP 4
    */
    while(!is_find && p_data->is_subt_dec_on == TRUE)
    {
        if(p_data->subt_dr_cnt <= 0)
        {
            break;
        }

        p_subt_dr       = p_data->subt_dr;
        is_find         = TRUE;
        is_ttx_subtitle = FALSE;
    }

    /*
    *	STEP 5
    */
    while(!is_find)
    {
        for(i = 0, p_ttx_dr = p_data->ttx_dr
            ; i < p_data->ttx_dr_cnt
            ; i++, p_ttx_dr++)
        {
            if(p_ttx_dr->type == SUBT_TYPE_TTX_NORMAL
                || p_ttx_dr->type == SUBT_TYPE_TTX_HEARING_HI)
            {
                break;
            }
        }

        if(i >= p_data->ttx_dr_cnt)
        {
            break;
        }

        p_ttx_dr        = p_ttx_dr;
        is_find         = TRUE;
        is_ttx_subtitle = TRUE;
    }


    if(is_find == FALSE)
    {
        p_data->p_new_subt     = NULL;
        p_data->p_new_ttx_subt = NULL;
    }
    else
    {
        if(is_ttx_subtitle == FALSE)
        {
            p_data->p_new_subt     = p_subt_dr;
            p_data->p_new_ttx_subt = NULL;
        }
        else
        {
            p_data->p_new_subt     = NULL;
            p_data->p_new_ttx_subt = p_ttx_dr;
        }
    }
}

static void play_subt_1(sub_ttx_priv_t *p_data)
{
    if(p_data->p_new_subt == NULL
        && p_data->is_subt_dec_on != FALSE
        && (p_data->curr_subt_type > SUBT_TYPE_UNDEF
            && p_data->curr_subt_type <= SUBT_TYPE_AR2P21_1_HI))
    {
        subt_set_display_vsb(FALSE);
        subt_set_pause_vsb(TRUE);
        memset(&p_data->curr_subt, 0, sizeof(subtitle_descr_t));
    }

    if(p_data->p_new_ttx_subt == NULL
        && (p_data->curr_subt_type > SUBT_TYPE_AR2P21_1_HI
            && p_data->curr_subt_type <= SUBT_TYPE_TTX_HEARING_HI))
    {
        vbi_ttx_hide_vsb(TRUE);
        memset(&p_data->curr_ttx_subt, 0,sizeof(teletext_descr_t));


        if(p_data->p_new_ttx == NULL)
        {
            vbi_ttx_pause_vsb();
            memset(&p_data->curr_ttx, 0, sizeof(teletext_descr_t));
        }
        else
        {
            if(p_data->curr_ttx.pid != p_data->p_new_ttx->pid)
                vbi_set_pid_vsb(p_data->p_new_ttx->pid);

            vbi_ttx_resume_vsb();

            memcpy(&p_data->curr_ttx
                , p_data->p_new_ttx
                , sizeof(teletext_descr_t));
        }
    }

    if(p_data->p_new_subt != NULL
        && p_data->is_subt_dec_on != FALSE)
    {
        if(p_data->curr_subt.pid != p_data->p_new_subt->pid
            || p_data->curr_subt.cmps_page_id
                != p_data->p_new_subt->cmps_page_id
            || p_data->curr_subt.ancl_page_id
                != p_data->p_new_subt->ancl_page_id)
        {
            subt_set_page_vsb(p_data->p_new_subt->pid
                , p_data->p_new_subt->cmps_page_id
                , p_data->p_new_subt->ancl_page_id);


            subt_set_pause_vsb(FALSE);
            subt_set_display_vsb(TRUE);

            memcpy(&p_data->curr_subt
                , p_data->p_new_subt
                , sizeof(subtitle_descr_t));
            p_data->curr_subt_lang[0] = p_data->curr_subt.language_code[0];
            p_data->curr_subt_lang[1] = p_data->curr_subt.language_code[1];
            p_data->curr_subt_lang[2] = p_data->curr_subt.language_code[2];

            p_data->curr_subt_type = p_data->p_new_subt->type;
        }
    }
    else if(p_data->p_new_ttx_subt != NULL)
    {
        u16 ttx_page_no = 0;

        if(p_data->curr_ttx_subt.pid != p_data->p_new_ttx_subt->pid)
        {
            if(p_data->p_new_ttx_subt->pid != p_data->curr_ttx.pid)
            {
                vbi_set_pid_vsb(p_data->p_new_ttx_subt->pid);
            }

            ttx_page_no = ((p_data->p_new_ttx_subt->magazien & 0xf) << 8)
                | (p_data->p_new_ttx_subt->page & 0xff);


            vbi_ttx_resume_vsb();
            //valid ttx_subt,fix bug6319
            //ttx_page_no scope 0x0~0x99
            if(((ttx_page_no & 0xf) <= 9) && 
                (((ttx_page_no & 0xf0) >> 4) <= 9))
            {
                vbi_ttx_show_vsb(TRUE, ttx_page_no);
            }


            memcpy(&p_data->curr_ttx_subt
                , p_data->p_new_ttx_subt
                , sizeof(teletext_descr_t));

            p_data->curr_subt_lang[0] = p_data->curr_ttx_subt.language_code[0];
            p_data->curr_subt_lang[1] = p_data->curr_ttx_subt.language_code[1];
            p_data->curr_subt_lang[2] = p_data->curr_ttx_subt.language_code[2];

            p_data->curr_subt_type = p_data->p_new_ttx_subt->type;
        }
    }
    else
    {
        p_data->curr_subt_type = SUBT_TYPE_UNDEF;
    }
}
void start_subt_dec_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  CHECK_FAIL_RET_VOID(p_data != NULL);
  if(p_data->is_subt_dec_on)
  {
    return;
  }
  CHECK_FAIL_RET_VOID(!p_data->is_subt_dec_on);

  subt_start_vsb();
  p_data->is_subt_dec_on = TRUE;

  if(p_data->is_ttx_play == FALSE
      && p_data->is_subt_play != FALSE)
  {
    select_subt_service_1(p_data);
    play_subt_1(p_data);
  }
}

void stop_subt_dec_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;

  subt_stop_vsb();
  p_data->is_subt_dec_on = FALSE;

  if(p_data->is_ttx_play == FALSE
    && p_data->is_subt_play != FALSE)
  {
    select_subt_service_1(p_data);
    play_subt_1(p_data);
    memset(&p_data->curr_subt, 0, sizeof(subtitle_descr_t));
  }
}

/*!
  *	set subtitle service
  */
void set_subt_service_1(handle_t handle, u32 index, u32 type)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  u16 b_index = index & 0xff;
  u16 t_index = (index >> 16) & 0xff;
  const char *p_lang_b =  iso_639_2_idx_to_desc(b_index);
  const char *p_lang_t =  iso_639_2_idx_to_desc(t_index);
  
  p_data->pref_subt_lang_b[0] = *p_lang_b++;
  p_data->pref_subt_lang_b[1] = *p_lang_b++;
  p_data->pref_subt_lang_b[2] = *p_lang_b;

  p_data->pref_subt_lang_t[0] = *p_lang_t++;
  p_data->pref_subt_lang_t[1] = *p_lang_t++;
  p_data->pref_subt_lang_t[2] = *p_lang_t;

  p_data->pref_subt_type = (u8)type;

  if(p_data->is_ttx_play == FALSE
    || p_data->is_subt_play != FALSE)
  {
    select_subt_service_1(p_data);
    play_subt_1(p_data);
  }
}


/*!
  *	set subtitle service
  */
void set_subt_service_lang_code(handle_t handle, u32 code, u32 type)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  
  p_data->pref_subt_lang_b[0] = (code & 0xff);
  p_data->pref_subt_lang_b[1] = ((code >> 8) & 0xff);
  p_data->pref_subt_lang_b[2] = ((code >> 16) & 0xff);

  p_data->pref_subt_lang_t[0] = (code & 0xff);
  p_data->pref_subt_lang_t[1] = ((code >> 8) & 0xff);
  p_data->pref_subt_lang_t[2] = ((code >> 16) & 0xff);

  p_data->pref_subt_type = (u8)type;

  if(p_data->is_ttx_play == FALSE
    || p_data->is_subt_play != FALSE)
  {
    select_subt_service_1(p_data);
    play_subt_1(p_data);
  }
}


/*!
   show subtile
  */
void show_subtitle_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;

  if(!p_data->is_subt_play)
  {
    p_data->is_subt_play = TRUE;

    if(p_data->is_ttx_play == FALSE)
    {
      select_subt_service_1(p_data);
      play_subt_1(p_data);
    }
  }
}

/*!
   hide subtitle
  */
BOOL hide_subtitle_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  BOOL ret = FALSE;
  
    if(p_data->is_subt_play)
    {
        p_data->is_subt_play = FALSE;


        if(p_data->is_ttx_play == FALSE)
        {
            if(p_data->p_new_ttx_subt != NULL)
            {
                vbi_ttx_hide_vsb(TRUE);
                ret  = TRUE;
            }
            //else if(p_data->p_new_subt != NULL)  //fix bug 25822
            if(p_data->p_new_subt != NULL)
            {
                subt_set_display_vsb(FALSE);
                subt_set_pause_vsb(TRUE);
            }
        }

        memset(&p_data->curr_subt, 0, sizeof(subtitle_descr_t));
        memset(&p_data->curr_ttx_subt, 0, sizeof(teletext_descr_t));
    }
    return ret;
}

/*!
   start teletext decoder
  */
void start_ttx_dec_1(handle_t handle)
{
    return;
}

/*!
   stop teletext decoder
  */
BOOL stop_ttx_dec_1(handle_t handle)
{
    return FALSE;
}

static void select_ttx_service_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
    teletext_descr_t    *p_ttx_dr = NULL;
    s32                 i = 0;


    for(i = 0, p_ttx_dr = p_data->ttx_dr
        ; i < p_data->ttx_dr_cnt
        ; i++, p_ttx_dr++)
    {
        if(((p_data->pref_ttx_lang_b[0] == p_ttx_dr->language_code[0]
            && p_data->pref_ttx_lang_b[1] == p_ttx_dr->language_code[1]
            && p_data->pref_ttx_lang_b[2] == p_ttx_dr->language_code[2])
            ||(p_data->pref_ttx_lang_t[0] == p_ttx_dr->language_code[0]
            && p_data->pref_ttx_lang_t[1] == p_ttx_dr->language_code[1]
            && p_data->pref_ttx_lang_t[2] == p_ttx_dr->language_code[2]))
            && (p_ttx_dr->type == TTX_INITIAL_PAGE))
        {
            break;
        }
    }

    if(i >= p_data->ttx_dr_cnt)
    {
        for(i = 0, p_ttx_dr = p_data->ttx_dr
            ; i < p_data->ttx_dr_cnt
            ; i++, p_ttx_dr++)
        {
            if(p_ttx_dr->type == TTX_INITIAL_PAGE)
            {
                break;
            }
        }
    }

    if(i >= p_data->ttx_dr_cnt)
    {
        p_data->p_new_ttx = NULL;
    }
    else
    {
        p_data->p_new_ttx = p_ttx_dr;
    }
}

static BOOL play_ttx_1(handle_t handle, BOOL is_display)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
    CHECK_FAIL_RET_FALSE(p_data != NULL);

    if(p_data->p_new_ttx == NULL)
    {
        if(p_data->curr_ttx.pid != 0)
        {
            vbi_ttx_pause_vsb();

            if(p_data->is_vbi_inserter_on)
            {
                vbi_inserter_stop_vsb();
            }

            memset(&p_data->curr_ttx, 0
                , sizeof(teletext_descr_t));
        }

        return FALSE;
    }


    if(p_data->curr_ttx.pid != p_data->p_new_ttx->pid)
    {
        vbi_set_pid_vsb(p_data->p_new_ttx->pid);


        vbi_ttx_resume_vsb();


        if(p_data->is_vbi_inserter_on)
        {
            vbi_inserter_start_vsb();
        }

        if(p_data->is_ttx_play)
        {
            vbi_ttx_show_vsb(FALSE, INVALID_TTX_PAGE);
        }

        memcpy(&p_data->curr_ttx
            , p_data->p_new_ttx
            , sizeof(teletext_descr_t));
    }
    else if(is_display == TRUE)
    {
        vbi_set_pid_vsb(p_data->p_new_ttx->pid);  // for bug 11965
        vbi_ttx_show_vsb(FALSE, INVALID_TTX_PAGE);
        p_data->is_ttx_play = TRUE;
    }


    return TRUE;
}


/*!
   set teletext preferred language
  */
void set_ttx_lang_1(handle_t handle, u32 index)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;   
  u16 b_index = index & 0xff;
  u16 t_index = (index >> 16) & 0xff;
  const char *p_lang_b =  iso_639_2_idx_to_desc(b_index);
  const char *p_lang_t = iso_639_2_idx_to_desc(t_index);

  p_data->pref_ttx_lang_b[0] = *p_lang_b++;
  p_data->pref_ttx_lang_b[1] = *p_lang_b++;
  p_data->pref_ttx_lang_b[2] = *p_lang_b;
  
  p_data->pref_ttx_lang_t[0] = *p_lang_t++;
  p_data->pref_ttx_lang_t[1] = *p_lang_t++;
  p_data->pref_ttx_lang_t[2] = *p_lang_t;

  select_ttx_service_1(p_data);
  play_ttx_1(p_data, FALSE);
}

/*!
   send teletext key to VBI
  */
void post_ttx_key_1(handle_t handle, u32 key)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;


  if(p_data->is_ttx_play != FALSE)
  {
    vbi_post_ttx_key_vsb(key);
  }
}

void show_ttx_1(handle_t handle, u32 region_handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
    CHECK_FAIL_RET_VOID(p_data != NULL);
    CHECK_FAIL_RET_VOID(!p_data->is_ttx_play);


    p_data->is_ttx_play = TRUE;

    vbi_set_region_handle_vsb((void *)region_handle);
    /*
     *	playing dvb subtitle
     */
    if(p_data->is_subt_play)
    {
        if(p_data->p_new_subt != NULL)
        {
            subt_set_pause_vsb(TRUE);
            subt_set_display_vsb(FALSE);

            memset(&p_data->curr_subt, 0, sizeof(subtitle_descr_t));
        }

        memset(&p_data->curr_ttx_subt, 0, sizeof(teletext_descr_t));
    }

    play_ttx_1(p_data, TRUE);
}

BOOL hide_ttx_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
    if(p_data->is_ttx_play)
    {
        p_data->is_ttx_play = FALSE;
        vbi_ttx_hide_vsb(TRUE);


        if(p_data->is_subt_play != FALSE)
        {
            select_subt_service_1(p_data);
            play_subt_1(p_data);
        }
     return TRUE;
    }
  return FALSE;
}

void start_vbi_inserter_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;


  if(!p_data->is_vbi_inserter_on)
  {
    p_data->is_vbi_inserter_on = TRUE;
    vbi_inserter_start_vsb();
  }
}

/*!
stop VBI inserter
*/
void stop_vbi_inserter_1(handle_t handle)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;


  if(p_data->is_vbi_inserter_on)
  {
    p_data->is_vbi_inserter_on = FALSE;
    vbi_inserter_stop_vsb();
  }
}

/*!
   get subtitle service description
  */
BOOL get_subt_description_1(class_handle_t handle,
        pb_subt_info_t *p_info, u16 pg_id)
{
  s32 i = 0;
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;

  if((p_data->cur_pg_id != pg_id))// || (p_data->subt_dr_cnt == 0))
  {
    return FALSE;
  }

  p_info->service_cnt = p_data->subt_dr_cnt;

  if(p_info->service_cnt > MAX_SUBT_SERVICE)
  {
    p_info->service_cnt = MAX_SUBT_SERVICE;
  }

  for(i = 0; i < p_info->service_cnt; i++)
  {
    p_info->service[i].lang[0] =
      p_data->subt_dr[i].language_code[0];
    p_info->service[i].lang[1] =
      p_data->subt_dr[i].language_code[1];
    p_info->service[i].lang[2] =
      p_data->subt_dr[i].language_code[2];
    p_info->service[i].type = p_data->subt_dr[i].type;
  }

  for(i = 0;
      i < p_data->ttx_dr_cnt && p_info->service_cnt <
      MAX_SUBT_SERVICE;
      i++)
  {
    if(p_data->ttx_dr[i].type == SUBT_TYPE_TTX_NORMAL
      || p_data->ttx_dr[i].type == SUBT_TYPE_TTX_HEARING_HI)
    {
      u8 idx = p_info->service_cnt++;

      p_info->service[idx].lang[0] =
        p_data->ttx_dr[i].language_code[0];
      p_info->service[idx].lang[1] =
        p_data->ttx_dr[i].language_code[1];
      p_info->service[idx].lang[2] =
        p_data->ttx_dr[i].language_code[2];
      p_info->service[idx].type = p_data->ttx_dr[i].type;
    }
  }
  if (!p_info->service_cnt)
  {
    return FALSE;
  }

  p_info->cur_service.lang[0] = p_data->curr_subt_lang[0];
  p_info->cur_service.lang[1] = p_data->curr_subt_lang[1];
  p_info->cur_service.lang[2] = p_data->curr_subt_lang[2];
  p_info->cur_service.type = p_data->curr_subt_type;
  return TRUE;
}


/*!
   get teletext service description
  */
BOOL get_ttx_description_1(class_handle_t handle,
        pb_ttx_info_t *p_info, u16 pg_id)
{
  s32 i = 0;
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  OS_PRINTF("  debug =======get_ttx_description_1\n");
  if((p_data->cur_pg_id != pg_id) || (p_data->ttx_dr_cnt == 0))
  {
    return FALSE;
  }

  for(i = 0; i < p_data->ttx_dr_cnt; i++)
  {
    if(TTX_INITIAL_PAGE == p_data->ttx_dr[i].type) 
      //(0x1 == p_data->ttx_dr[i].magazien))
    {
      break;
    }
  }
  if(i == p_data->ttx_dr_cnt)//perhaps only contains tele-sub
  {
    return FALSE;
  }
  
  p_info->service_cnt = p_data->ttx_dr_cnt;

  if(p_info->service_cnt > MAX_TTX_SERVICE)
  {
    p_info->service_cnt = MAX_TTX_SERVICE;
  }

  for(i = 0; i < p_info->service_cnt; i++)
  {
    p_info->service[i].lang[0] =
      p_data->ttx_dr[i].language_code[0];
    p_info->service[i].lang[1] =
      p_data->ttx_dr[i].language_code[1];
    p_info->service[i].lang[2] =
      p_data->ttx_dr[i].language_code[2];
    p_info->service[i].type = p_data->ttx_dr[i].type;
  }

  p_info->cur_service.lang[0] =
    p_data->curr_ttx.language_code[0];
  p_info->cur_service.lang[1] =
    p_data->curr_ttx.language_code[1];
  p_info->cur_service.lang[2] =
    p_data->curr_ttx.language_code[2];
  p_info->cur_service.type = p_data->curr_ttx.type;

  return TRUE;
}

/*
    For VBI Teletext:
    ----------------
    0x10    DVB subtitles (normal) with no monitor aspect ratio
            criticality
    0x11    DVB subtitles (normal) for display on 4:3 aspect
            ratio monitor
    0x12    DVB subtitles (normal) for display on 16:9 aspect
            ratio monitor
    0x13    DVB subtitles (normal) for display on 2.21:1 aspect
            ratio monitor
    0x14
      |
    0x1F    reserved for future use
    0x20    DVB subtitles (for the hard of hearing) with no
            monitor aspect ratio criticality
    0x21    DVB subtitles (for the hard of hearing) for display
            on 4:3 aspect ratio monitor
    0x22    DVB subtitles (for the hard of hearing) for display
            on 16:9 aspect ratio monitor
    0x23    DVB subtitles (for the hard of hearing) for display
            on 2.21:1 aspect ratio monitor



    For DVB Subtitle:
    ----------------
    0x00    reserved for future use
    0x01    initial Teletext page
    0x02    Teletext subtitle page
    0x03    additional information page
    0x04    programme schedule page
    0x05    Teletext subtitle page for hearing impaired people
    0x06    to 0x1F reserved for future use
*/
void pmt_update_1(handle_t handle, void *p_pmt_data, u8 ts_in)
{
    u32             i          = 0;
    event_t         evt        = {0};
    sub_ttx_priv_t  *p_data    = (sub_ttx_priv_t *)handle;
    pmt_t           *p_pmt     = (pmt_t *)p_pmt_data;
    BOOL            ttx_ready  = FALSE;
    BOOL            subt_ready = FALSE;



    if(p_pmt->subt_dr_cnt != 0)
    {
        u8               cnt = 0;
        subtitle_descr_t subt_dr[MAX_SUBT_DESCRIPTION];
        subtitle_descr_t *p_dr = NULL;

        cnt = (p_pmt->subt_dr_cnt > MAX_SUBT_DESCRIPTION)
                ? MAX_SUBT_DESCRIPTION : p_pmt->subt_dr_cnt;

        memcpy(subt_dr, p_pmt->subt_descr, sizeof(subtitle_descr_t) * cnt);


        for(i = 0, p_dr = subt_dr; i < cnt; i++, p_dr ++)
        {
            switch(p_dr->type)
            {
                case 0x10:
                    p_dr->type = SUBT_TYPE_NORMAL;
                    subt_ready = TRUE;
                    break;
                case 0x11:
                    p_dr->type = SUBT_TYPE_AR4_3;
                    subt_ready = TRUE;
                    break;
                case 0x12:
                    p_dr->type = SUBT_TYPE_AR16_9;
                    subt_ready = TRUE;
                    break;
                case 0x13:
                    p_dr->type = SUBT_TYPE_AR2P21_1;
                    subt_ready = TRUE;
                    break;
                case 0x14:
                    p_dr->type = SUBT_TYPE_AR_HD;
                    subt_ready = TRUE;   
                    break;                    
                case 0x20:
                    p_dr->type = SUBT_TYPE_NORMAL_HI;
                    subt_ready = TRUE;
                    break;
                case 0x21:
                    p_dr->type = SUBT_TYPE_AR4_3_HI;
                    subt_ready = TRUE;
                    break;
                case 0x22:
                    p_dr->type = SUBT_TYPE_AR16_9_HI;
                    subt_ready = TRUE;
                    break;
                case 0x23:
                    p_dr->type = SUBT_TYPE_AR2P21_1_HI;
                    subt_ready = TRUE;
                    break;
                default:
                    p_dr->type = SUBT_TYPE_UNDEF;
                    break;
            }
        }

        mtos_task_lock();
        memcpy(p_data->subt_dr, subt_dr , sizeof(subtitle_descr_t) * cnt);
        p_data->subt_dr_cnt = cnt;
        mtos_task_unlock();
    }
    else
    {
        mtos_task_lock();
        memset(p_data->subt_dr, 0, sizeof(subtitle_descr_t) * MAX_SUBT_DESCRIPTION);
        p_data->subt_dr_cnt = 0;
        mtos_task_unlock();
    }


    if(p_pmt->ttx_dr_cnt != 0)
    {
        u8               cnt = 0;
        teletext_descr_t ttx_dr[MAX_TTX_DESCRIPTION];
        teletext_descr_t *p_dr = NULL;


        cnt = (p_pmt->ttx_dr_cnt > MAX_TTX_DESCRIPTION)
            ? MAX_TTX_DESCRIPTION : p_pmt->ttx_dr_cnt;

        memcpy(ttx_dr, p_pmt->ttx_descr, sizeof(teletext_descr_t) * cnt);


        for(i = 0, p_dr = ttx_dr; i < cnt; i ++, p_dr ++)
        {
            switch(p_dr->type)
            {
                case 0x01:
                    p_dr->type = TTX_INITIAL_PAGE;
                    ttx_ready  = TRUE;
                    break;
                case 0x02:
  
                    if (((p_dr->page & 0xf) <= 9) && 
                    (((p_dr->page & 0xf0) >> 4) <= 9))
                   {
                      p_dr->type = SUBT_TYPE_TTX_NORMAL;
                      subt_ready  = TRUE;
                    }
                    else
                    {
                      p_dr->type = SUBT_TYPE_UNDEF;
                    }

                    break;
                case 0x03:
                    p_dr->type = TTX_ADDITIONAL_INFO_PAGE;
                    ttx_ready  = TRUE;
                    break;
                case 0x04:
                    p_dr->type = TTX_PROG_SCHEDULE_PAGE;
                    ttx_ready  = TRUE;
                    break;
                case 0x05:
                    if (((p_dr->page & 0xf) <= 9) && 
                    (((p_dr->page & 0xf0) >> 4) <= 9))
                   {
                      p_dr->type = SUBT_TYPE_TTX_HEARING_HI;
                      subt_ready = TRUE;
                    }
                    else
                    {
                      p_dr->type = SUBT_TYPE_UNDEF;
                    }
                    break;
                default:
                    p_dr->type = SUBT_TYPE_UNDEF;
                    break;
            }
        }

        mtos_task_lock();
        memcpy(p_data->ttx_dr, ttx_dr , sizeof(teletext_descr_t) * cnt);
        p_data->ttx_dr_cnt = cnt;
        mtos_task_unlock();
    }
    else
    {
        mtos_task_lock();
        memset(p_data->ttx_dr, 0, sizeof(teletext_descr_t) * MAX_TTX_DESCRIPTION);
        p_data->ttx_dr_cnt = 0;
        mtos_task_unlock();
    }





    if(p_data->is_subt_play)
    {
        select_subt_service_1(p_data);

        if(!p_data->is_ttx_play)
        {
          play_subt_1(p_data);
        }
    }


    select_ttx_service_1(p_data);
   // if(p_data->is_subt_play != TRUE)   // for  ttx-sub bug 11965
    play_ttx_1(p_data, FALSE);

    if(subt_ready)
    {
        evt.id    = PB_EVT_SUB_READY;
        evt.data1 = p_data->cur_pg_id;
        ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
    }

    if(ttx_ready)
    {
        evt.id    = PB_EVT_TTX_READY;
        evt.data1 = p_data->cur_pg_id;
        ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
    }
}

u32 get_subt_pids(u16 *pids)
{
  u32 i = 0;
  u32 pid_count = 0;
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
  
  for(i=0;i<p_data->subt_dr_cnt;i++)
  {
    pids[pid_count++] = p_data->subt_dr[i].pid;
  }
  
  for(i=0;i<p_data->ttx_dr_cnt;i++)
  {
    pids[pid_count++] = p_data->ttx_dr[i].pid;
  }
  
  return pid_count;
}

void subt_txt_info_reset_1(handle_t handle)
{
    sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;


    mtos_task_lock();
    memset(p_data->subt_dr, 0, sizeof(subtitle_descr_t) * MAX_SUBT_DESCRIPTION);
    p_data->subt_dr_cnt = 0;

    memset(p_data->ttx_dr, 0, sizeof(teletext_descr_t) * MAX_TTX_DESCRIPTION);
    p_data->ttx_dr_cnt = 0;
    mtos_task_unlock();
}


void stc_set_pg_id_1(handle_t handle, u16 pg_id)
{
  sub_ttx_priv_t *p_data = (sub_ttx_priv_t *)handle;
  p_data->cur_pg_id = pg_id;
}

