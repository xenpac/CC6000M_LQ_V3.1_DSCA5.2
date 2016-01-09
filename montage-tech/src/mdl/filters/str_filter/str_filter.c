/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// std
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "common.h"
#include "drv_dev.h"

#include "lib_memp.h"
#include "pdec.h"
#include "malloc.h"

//util
#include "class_factory.h"
#include "simple_queue.h"
#include "lib_rect.h"

//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "imem_alloc.h"
#include "iasync_reader.h"
#include "ipin.h"
#include "input_pin.h"
#include "output_pin.h"
#include "transf_input_pin.h"
#include "transf_output_pin.h"
#include "str_input_pin_intra.h"
#include "str_output_pin_intra.h"

#include "iconv_ext.h"

#include "ifilter.h"
#include "transf_filter.h"
#include "common_filter.h"
#include "str_filter.h"
#include "str_filter_intra.h"
#include "gui_resource.h"
#include "char_map.h"
#include "gb2312.h"
#include "gui_resource.h"
#include "lib_unicode.h"
#include "iso_8859.h"
#include "lib_char.h"

#define STR_FILTER_MAX_LINES   2

static str_filter_private_t * get_priv(handle_t _this)
{
  MT_ASSERT(NULL != _this);

  return &((str_filter_t *)_this)->private_data;
}


static u32 utf8_to_unicode(u16 *p_dst, u32 outsize, u8 *p_src, u32 insize)
{
  u32 totalNum = 0;
  u8 *p_data = p_src;
  u32 resultsize = 0;
  u8 *p_tmp = (u8 *)p_dst;
  u32 i = 0;
  u8 t1 = 0, t2 = 0;
  u16 t3 = 0, t4 = 0, t5 = 0;
  u32 tmp_size = 0;

  if(p_dst == NULL || p_src == NULL || insize < 0)
  {
    return -1;
  }

  for(i = 0; i < insize; i++)
  {
    if (*p_data >= 0x00 && *p_data <= 0x7f)
    {
      p_data ++;
      totalNum ++;
    }
    else if ((*p_data & (0xe0))== 0xc0)
    {
      p_data += 2;
      totalNum ++;
    }
    else if ((*p_data & (0xf0))== 0xe0)
    {
      p_data += 3;
      totalNum ++;
    }
  }

  if(outsize < totalNum)
  {
    return -1;
  }

  p_data = p_src;
  while(*p_data)
  {
    if (*p_data >= 0x00 && *p_data <= 0x7f)
    {
      *p_tmp = *p_data;
      p_tmp ++;
      p_tmp ++;
      resultsize += 1;
    }
    else if ((*p_data & 0xe0)== 0xc0)
    {
      t1 = *p_data & (0x1f);
      p_data ++;
      t2 = *p_data & (0x3f);

      *p_tmp = t2 | ((t1 & (0x03)) << 6);
      p_tmp ++;
      *p_tmp = t1 >> 2;
      p_tmp ++;
      resultsize += 1;
    }
    else if ((*p_data & (0xf0))== 0xe0)
    {
      t3 = *p_data & (0x1f);
      p_data ++;
      t4 = *p_data & (0x3f);
      p_data ++;
      t5 = *p_data & (0x3f);

      *p_tmp = ((t4 & (0x03)) << 6) | t5;
      p_tmp ++;
      *p_tmp = (t3 << 4) | (t4 >> 2);
      p_tmp ++;
      resultsize += 1;
    }
    p_data ++;
    tmp_size ++;
    if (tmp_size >= insize)
    {
      break;
    }
  }

  return resultsize;

}

static RET_CODE str_on_command(handle_t _this, icmd_t *p_cmd)
{
  str_filter_private_t *p_priv = get_priv(_this);

  switch (p_cmd->cmd)
  {
    case STR_CFG_REND_RECT:
      p_priv->render_rect.left = ((rect_t *)p_cmd->p_para)->left;
      p_priv->render_rect.top = ((rect_t *)p_cmd->p_para)->top;
      p_priv->render_rect.right = ((rect_t *)p_cmd->p_para)->right;
      p_priv->render_rect.bottom = ((rect_t *)p_cmd->p_para)->bottom;
      break;

    case STR_CFG_SET:
      p_priv->pal_num = ((str_cfg_t *)p_cmd->p_para)->pal_num;

      MT_ASSERT(p_priv->p_pal != NULL);

      memcpy(p_priv->p_pal, ((str_cfg_t *)p_cmd->p_para)->p_pal, p_priv->pal_num * sizeof(u32));
      p_priv->fmt = ((str_cfg_t *)p_cmd->p_para)->fmt;

      p_priv->bpp = ((str_cfg_t *)p_cmd->p_para)->bpp;

      p_priv->use_font_bg = ((str_cfg_t *)p_cmd->p_para)->use_font_bg;

      p_priv->font_bg = ((str_cfg_t *)p_cmd->p_para)->font_bg;

      p_priv->win_bg = ((str_cfg_t *)p_cmd->p_para)->win_bg;

      MT_ASSERT(p_priv->bpp % 8 == 0);
      break;

    case STR_CFG_CHARSET:
       p_priv->str_charset = p_cmd->lpara;
      break;

    default:
      return ERR_FAILURE;
  }

  return SUCCESS;
}

static RET_CODE str_on_start(handle_t _this)
{
  str_filter_private_t *p_priv =  get_priv(_this);

  p_priv->pre_h = 0;
  p_priv->s_y = 0;

  return SUCCESS;
}

static RET_CODE str_on_stop(handle_t _this)
{
  return SUCCESS;
}

static RET_CODE str_on_close(handle_t _this)
{

  return SUCCESS;
}

static  u16 str_get_right_string(str_filter_private_t *p_priv, u16 *p_unicode,
  u16 *p_end, u16 *p_width, BOOL is_arabic_blank, u32 style)
{
  glyph_input_t input = {0}, input_rep = {0};
  glyph_output_t output = {0};
  u16 len = 0, i = 0;

  *p_width = 0;

  while((0 != p_unicode[i]) && (p_unicode + i < p_end))
  {
    if(0x000a == p_unicode[i])
    {
      break;
    }

    if((0x000d == p_unicode[i]) && (0x000a == p_unicode[i]))
    {
      break;
    }


    if(is_arabic_uni_code(p_unicode[i]) || (p_unicode[i]== 0x0020))
    {
      len ++;

      input.char_code = p_unicode[i];
      input.fmt = p_priv->fmt;
      input.str_style = style;

      input_rep.char_code = 0x003f;
      input_rep.fmt = p_priv->fmt;
      input_rep.str_style = style;

      if((SUCCESS ==  p_priv->rsc_get_glyph(&input, &output)) ||
        (SUCCESS ==  p_priv->rsc_get_glyph(&input_rep, &output)))
      {
        if(output.p_alpha != NULL)
        {
          //vector font, use step_width & step_height.
          *p_width = *p_width + output.step_width;
        }
        else
        {
          //array font, use width & height directly.
          *p_width = *p_width + output.width;
        }
      }
    }
    else
    {
      break;
    }
    i++;
  }

  return len;
}



static  u16 str_get_left_string(str_filter_private_t *p_priv, u16 *p_unicode,
  u16 *p_end, u16 *p_width, BOOL is_arabic_blank, u32 style)
{
  glyph_input_t input = {0}, input_rep = {0};
  glyph_output_t output = {0};
  u16 len = 0, i = 0;

  *p_width = 0;

  while((0 != p_unicode[i]) && (p_unicode + i < p_end))
  {
    if(0x000a == p_unicode[i])
    {
      break;
    }

    if((0x000d == p_unicode[i]) && (0x000a == p_unicode[i]))
    {
      break;
    }

    if(!is_arabic_uni_code(p_unicode[i]))
    {

      len++;

      input.char_code = p_unicode[i];
      input.fmt = p_priv->fmt;
      input.str_style = style;

      input_rep.char_code = 0x003f;
      input_rep.fmt = p_priv->fmt;
      input_rep.str_style = style;

      if((SUCCESS ==  p_priv->rsc_get_glyph(&input, &output)) ||
        (SUCCESS ==  p_priv->rsc_get_glyph(&input_rep, &output)))
      {
        if(output.p_alpha != NULL)
        {
          //vector font, use step_width & step_height.
          *p_width = *p_width + output.step_width;
        }
        else
        {
          //array font, use width & height directly.
          *p_width = *p_width + output.width;
        }
      }    }
    else
    {
      break;
    }

    i++;
  }

  return len;
}

static u16 str_calc_lines(str_filter_private_t *p_priv,
  u16 *p_unistr, u32 style, BOOL is_break_word)
{
  glyph_input_t input = {0}, input_rep = {0};
  glyph_output_t output = {0};
  u16 area_width = 0, area_height = 0, remn_width = 0, remn_height = 0;
  u16 i = 0, str_len = 0, tmp_w = 0, tmp_h = 0;
  u16 word_width = 0, word_height = 0;
  u16 char_code = 0, char_code1 = 0, char_code2 = 0, lines = 0;
  u16 chars = 0;
  BOOL is_start_with_word = FALSE, is_out = FALSE;

  str_len = uni_strlen(p_unistr);

  area_width = RECTW(p_priv->render_rect);
  area_height = RECTH(p_priv->render_rect);
  remn_height = area_height - p_priv->s_y;
  remn_width = area_width;
  lines = 0;

  p_priv->p_lineh[lines] = 0;
  p_priv->p_linew[lines] = 0;
  p_priv->pp_linep[lines] = p_unistr;
  p_priv->p_lines[lines] = 0;

  while(i < str_len)
  {
    //caculate the line.
    char_code = *(p_unistr + i);

    if(char_code == 0x000a)
    {
      //skip it.
      i++;

      //new line.
      remn_width = area_width;
      remn_height -= p_priv->p_lineh[lines];
      lines++;
      if(lines >= STR_FILTER_MAX_LINES)
      {
        //max lines, stop caculate.
        break;
      }

      p_priv->pp_linep[lines] = p_priv->pp_linep[lines - 1] + p_priv->p_lines[lines - 1] + 1;
      p_priv->p_lineh[lines] = 0;
      p_priv->p_linew[lines] = 0;
      p_priv->p_lines[lines] = 0;
    }
    else if((char_code == 0x000d) && (*(p_unistr + i + 1) == 0x000a))
    {
      //skip it.
      i += 2;

      //new line.
      remn_width = area_width;
      remn_height -= p_priv->p_lineh[lines];
      lines++;
      if(lines >= STR_FILTER_MAX_LINES)
      {
        //max lines, stop caculate.
        break;
      }

      p_priv->pp_linep[lines] = p_priv->pp_linep[lines - 1] + p_priv->p_lines[lines - 1] + 2;
      p_priv->p_lineh[lines] = 0;
      p_priv->p_linew[lines] = 0;
      p_priv->p_lines[lines] = 0;
    }
    else
    {
      input.char_code = p_unistr[i];
      input.fmt = p_priv->fmt;
      input.str_style = style;

      input_rep.char_code = 0x003f;
      input_rep.fmt = p_priv->fmt;
      input_rep.str_style = style;

      if((SUCCESS ==  p_priv->rsc_get_glyph(&input, &output)) ||
        (SUCCESS ==  p_priv->rsc_get_glyph(&input_rep, &output)))
      {

        if(output.p_alpha != NULL)
        {
          //vector font, use step_width & step_height.
          tmp_w = MAX(output.xoffset + output.width, output.step_width);
          tmp_h = MAX(p_priv->p_lineh[lines], output.step_height);
        }
        else
        {
          //array font, use width & height directly.
          tmp_w = p_priv->p_linew[lines] + output.width;
          tmp_h = MAX(p_priv->p_lineh[lines], output.height);
        }

        if(remn_height < MAX(p_priv->p_lineh[lines], tmp_h))
        {
          //draw end, not enough area in vertical direction.
          is_out = TRUE;
          break;
        }

        if(char_code == 0x0020)//is blank
        {
          is_start_with_word = FALSE;

          if(remn_width >= tmp_w)
          {
            p_priv->p_lines[lines]++;
            i++;
            remn_width -= tmp_w;
            p_priv->p_linew[lines] += tmp_w;
            p_priv->p_lineh[lines] = MAX(p_priv->p_lineh[lines], tmp_h);
          }
          else
          {
            //new line, reset width & height.
            remn_width = area_width;
            remn_height -= p_priv->p_lineh[lines];
            lines++;
            if(lines >= STR_FILTER_MAX_LINES)
            {
              //max lines, stop caculate.
              break;
            }

            p_priv->pp_linep[lines] = p_priv->pp_linep[lines - 1] + p_priv->p_lines[lines - 1];
            p_priv->p_lines[lines] = 0;
            p_priv->p_lineh[lines] = 0;
            p_priv->p_linew[lines] = 0;
          }
        }
        else
        {
          if(is_start_with_word || is_break_word)
          {
            if(remn_width >= tmp_w)
            {
              p_priv->p_lines[lines]++;
              i++;
              remn_width -= tmp_w;
              p_priv->p_linew[lines] += tmp_w;
              p_priv->p_lineh[lines] = MAX(p_priv->p_lineh[lines], tmp_h);
            }
            else
            {
              //new line, reset width & height.
              remn_width = area_width;
              remn_height -= p_priv->p_lineh[lines];
              lines++;
              if(lines >= STR_FILTER_MAX_LINES)
              {
                //max lines, stop caculate.
                break;
              }

              p_priv->pp_linep[lines] = p_priv->pp_linep[lines - 1] + p_priv->p_lines[lines - 1];
              p_priv->p_lineh[lines] = 0;
              p_priv->p_linew[lines] = 0;
              p_priv->p_lines[lines] = 0;
            }
          }
          else
          {
            //get word
            word_width = 0;
            word_height = 0;
            chars = 0;

            while(1)
            {
              char_code1 = *(p_priv->pp_linep[lines] + p_priv->p_lines[lines] + chars);
              char_code2 = *(p_priv->pp_linep[lines] + p_priv->p_lines[lines] + chars + 1);

              if(char_code1 == 0x0020)//is blank, means the end of a word.
              {
                break;
              }
              else if(char_code1 == 0)//end of the string.
              {
                break;
              }
              else if(char_code1 == 0x000a)//end of the ling.
              {
                break;
              }
              else if((char_code1 == 0x000d) && (char_code2 == 0x000a))
              {
                break;
              }

              tmp_w = 0;
              tmp_h = 0;

              input.char_code = char_code1;
              input.fmt = p_priv->fmt;
              input.str_style = style;

              input_rep.char_code = 0x003f;
              input_rep.fmt = p_priv->fmt;
              input_rep.str_style = style;

              if((SUCCESS ==  p_priv->rsc_get_glyph(&input, &output)) ||
                (SUCCESS ==  p_priv->rsc_get_glyph(&input_rep, &output)))
              {
                if(output.p_alpha != NULL)
                {
                  //vector font, use step_width & step_height.
                  tmp_w = MAX(output.xoffset + output.width, output.step_width);
                  tmp_h = MAX(p_priv->p_lineh[lines], output.step_height);
                }
                else
                {
                  //array font, use width & height directly.
                  tmp_w = p_priv->p_linew[lines] + output.width;
                  tmp_h = MAX(p_priv->p_lineh[lines], output.height);
                }
              }

              word_width += tmp_w;
              word_height = MAX(word_height, tmp_h);

              chars++;
            }

            if(remn_height < MAX(p_priv->p_lineh[lines], word_height))
            {
              //draw end, not enough area in vertical direction.
              is_out = TRUE;
              break;
            }

            if(remn_width >= word_width)
            {
              remn_width -= word_width;
              p_priv->p_linew[lines] += word_width;
              p_priv->p_lines[lines] += chars;
              i += chars;
              p_priv->p_lineh[lines] = MAX(p_priv->p_lineh[lines], word_height);
            }
            else
            {
              //new line, reset remain width & remain height.
              remn_width = area_width;
              remn_height -= p_priv->p_lineh[lines];
              is_start_with_word = TRUE;
              lines++;
              if(lines >= STR_FILTER_MAX_LINES)
              {
                //max lines, stop caculate.
                break;
              }
              p_priv->pp_linep[lines] = p_priv->pp_linep[lines - 1] + p_priv->p_lines[lines - 1];
              p_priv->p_lines[lines] = 0;
              p_priv->p_lineh[lines] = 0;
              p_priv->p_linew[lines] = 0;

            }
          }
        }

        if(lines >= STR_FILTER_MAX_LINES)
        {
          //max lines.
          break;
        }
      }
    }
  }

  if(lines < STR_FILTER_MAX_LINES && !is_out)
  {
    lines++;
  }

  return lines;
}

static void str_clear_line(str_filter_private_t *p_priv, u16 line_h)
{
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  rend_param_t *p_render = NULL;
  u32 *p_color = 0;

  //malloc buffer for push sample.
  p_render = (rend_param_t *)mtos_align_malloc(sizeof(rend_param_t) + 4, 8);
  MT_ASSERT(p_render != NULL);

  memset(p_render, 0, 4 + sizeof(rend_param_t));

  //clear the current line.
  p_render->rend_rect.left = p_priv->render_rect.left;
  p_render->rend_rect.top = p_priv->render_rect.top + p_priv->s_y;
  p_render->rend_rect.right = p_priv->render_rect.right;
  p_render->rend_rect.bottom = p_priv->render_rect.top + p_priv->s_y + line_h;

  p_render->use_bg_color = FALSE;
  p_render->bg_color = 0x0;
  p_render->pal_entry = p_priv->p_pal;
  p_render->pal_num = p_priv->pal_num;
  p_render->not_update = TRUE;

  p_color = (u32 *)((u8 *)p_render + sizeof(rend_param_t));

  *p_color = p_priv->win_bg;

  if(!is_invalid_rect(&p_render->rend_rect))
  {
    p_out_pin->query_sample(p_out_pin, &p_out_sample);

    if(p_out_sample == NULL)
    {
      mtos_align_free(p_render);

      return;
    }

    p_out_sample->p_data = (u8 *)p_render;
    p_out_sample->format.stream_type = MT_IMAGE;
    p_out_sample->format.media_type = MF_COLOR;

    p_out_pin->push_down(p_out_pin, p_out_sample);
  }

  mtos_align_free(p_render);
}

static void str_draw_font_bg(str_filter_private_t *p_priv,
  u16 left, u16 top, u16 line_w, u16 line_h)
{
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  rend_param_t *p_render = NULL;
  u32 *p_color = 0;

  if(p_priv->use_font_bg)
  {
    p_render = (rend_param_t *)mtos_align_malloc(sizeof(rend_param_t) + 4, 8);
    MT_ASSERT(p_render != NULL);

    memset(p_render, 0, 4 + sizeof(rend_param_t));

    p_render->fmt = p_priv->fmt;
    p_render->rend_rect.left = p_priv->render_rect.left + left;
    p_render->rend_rect.right = p_priv->render_rect.left + left + line_w;
    p_render->rend_rect.top = p_priv->render_rect.top + top;
    p_render->rend_rect.bottom = p_priv->render_rect.top + top + line_h;

    if(p_priv->pal_num != 0)
    {
      p_render->pal_entry = p_priv->p_pal;
      p_render->pal_num = p_priv->pal_num;
    }
    p_render->not_update = TRUE;

    p_color = (u32 *)((u8 *)p_render + sizeof(rend_param_t));

    *p_color = p_priv->font_bg;

    if(!is_invalid_rect(&p_render->rend_rect))
    {
      p_out_pin->query_sample(p_out_pin, &p_out_sample);

      if(p_out_sample == NULL)
      {
        mtos_align_free(p_render);

        return;
      }

      p_out_sample->p_data = (u8 *)p_render;
      p_out_sample->format.stream_type = MT_IMAGE;
      p_out_sample->format.media_type = MF_COLOR;

      p_out_pin->push_down(p_out_pin, p_out_sample);
    }

    mtos_align_free(p_render);  
    
  }

}


static u16 str_draw_char(str_filter_private_t *p_priv,
  u16 char_code, u32 style, u16 left, u16 top, BOOL is_l2r)
{
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  rend_param_t *p_render = NULL;
  glyph_input_t input = {0}, input_rep = {0};
  glyph_output_t output = {0};

  p_render = (rend_param_t *)mtos_align_malloc(sizeof(rend_param_t) + 4, 8);
  MT_ASSERT(p_render != NULL);

  memset(p_render, 0, 4 + sizeof(rend_param_t));

  input.char_code = char_code;
  input.fmt = p_priv->fmt;
  input.str_style = style;

  input_rep.char_code = 0x003f;
  input_rep.fmt = p_priv->fmt;
  input_rep.str_style = style;

  if((SUCCESS ==  p_priv->rsc_get_glyph(&input, &output)) ||
    (SUCCESS ==  p_priv->rsc_get_glyph(&input_rep, &output)))
  {
    if(output.p_alpha != NULL)
    {
      //alpha blending.
      memset(p_render, 0, sizeof(rend_param_t));
      p_render->fmt = p_priv->fmt;

      if(is_l2r)
      {
        p_render->rend_rect.left =
          p_priv->render_rect.left + left + output.xoffset;
        p_render->rend_rect.right =
          p_priv->render_rect.left + left + output.xoffset + output.width;
      }
      else
      {
        p_render->rend_rect.left =
          p_priv->render_rect.left + left + output.xoffset - 
          MAX(output.step_width, output.xoffset + output.width);;
        p_render->rend_rect.right =
          p_priv->render_rect.left + left + output.xoffset + output.width - 
          MAX(output.step_width, output.xoffset + output.width);;
      }

      p_render->rend_rect.top =
        p_priv->render_rect.top + top + output.yoffset;
      p_render->rend_rect.bottom =
        p_priv->render_rect.top + top + output.yoffset + output.height;

      //fix bug ##41856, temp solution.
      if(p_render->rend_rect.top < 0)
      {
        p_render->rend_rect.top = 0;
      }

      p_render->fill_rect.left = 0;
      p_render->fill_rect.top = 0;
      p_render->fill_rect.right = output.width;
      p_render->fill_rect.bottom = output.height;

      p_render->image_width = output.width;
      p_render->image_height = output.height;

      p_render->p_data = output.p_char;
      p_render->p_alpha = output.p_alpha;
      p_render->alpha_pitch = output.alpha_pitch;

      p_render->enable_ckey = output.enable_ckey;
      p_render->color_key = output.ckey;
      p_render->not_update = TRUE;

      p_render->pitch = output.pitch;

      if(!is_invalid_rect(&p_render->rend_rect) && !is_invalid_rect(&p_render->fill_rect))
      {
        p_out_pin->query_sample(p_out_pin, &p_out_sample);

        if(p_out_sample == NULL)
        {
          mtos_align_free(p_render);

          return 0;
        }

        p_out_sample->p_data = (u8 *)p_render;
        p_out_sample->format.stream_type = MT_IMAGE;
        p_out_sample->format.media_type = MF_IMAGE;

        p_out_pin->push_down(p_out_pin, p_out_sample);
      }

      if(output.p_strok_alpha != NULL)
      {
        //alpha blending.
        memset(p_render, 0, sizeof(rend_param_t));
        p_render->fmt = p_priv->fmt;

        if(is_l2r)
        {
          p_render->rend_rect.left =
            p_priv->render_rect.left + left + output.xoffset;
          p_render->rend_rect.right =
            p_priv->render_rect.left + left + output.xoffset + output.width;
        }
        else
        {
          p_render->rend_rect.left =
            p_priv->render_rect.left + left + output.xoffset - 
            MAX(output.step_width, output.xoffset + output.width);;
          p_render->rend_rect.right =
            p_priv->render_rect.left + left + output.xoffset + output.width - 
            MAX(output.step_width, output.xoffset + output.width);;
        }

        p_render->rend_rect.top =
          p_priv->render_rect.top + top + output.yoffset;
        p_render->rend_rect.bottom =
          p_priv->render_rect.top + top + output.yoffset + output.height;

        //fix bug ##41856, temp solution.
        if(p_render->rend_rect.top < 0)
        {
          p_render->rend_rect.top = 0;
        }

        p_render->fill_rect.left = 0;
        p_render->fill_rect.top = 0;
        p_render->fill_rect.right = output.width;
        p_render->fill_rect.bottom = output.height;

        p_render->image_width = output.width;
        p_render->image_height = output.height;

        p_render->p_data = output.p_strok_char;
        p_render->p_alpha = output.p_strok_alpha;
        p_render->alpha_pitch = output.alpha_pitch;

        p_render->enable_ckey = output.enable_ckey;
        p_render->color_key = output.ckey;
        p_render->not_update = TRUE;

        p_render->pitch = output.pitch;

        if(!is_invalid_rect(&p_render->rend_rect) && !is_invalid_rect(&p_render->fill_rect))
        {
          p_out_pin->query_sample(p_out_pin, &p_out_sample);

          if(p_out_sample == NULL)
          {
            mtos_align_free(p_render);

            return 0;
          }

          p_out_sample->p_data = (u8 *)p_render;
          p_out_sample->format.stream_type = MT_IMAGE;
          p_out_sample->format.media_type = MF_IMAGE;

          p_out_pin->push_down(p_out_pin, p_out_sample);
        }
      }
    }
    else
    {
      //push char.
      memset(p_render, 0, sizeof(rend_param_t));
      p_render->p_data = output.p_char;

      p_render->fmt = p_priv->fmt;

      if(is_l2r)
      {
        p_render->rend_rect.left = p_priv->render_rect.left + left;
        p_render->rend_rect.right =
          p_priv->render_rect.left + left + output.width;
      }
      else
      {
        p_render->rend_rect.left = p_priv->render_rect.left + left - output.width;
        p_render->rend_rect.right =
          p_priv->render_rect.left + left + output.width - output.width;
      }

      p_render->rend_rect.top = p_priv->render_rect.top + + top;
      p_render->rend_rect.bottom =
        p_priv->render_rect.top + output.height + top;

      p_render->fill_rect.left = 0;
      p_render->fill_rect.top = 0;
      p_render->fill_rect.right = output.width;
      p_render->fill_rect.bottom = output.height;

      p_render->image_width = output.width;
      p_render->image_height = output.height;
      if(p_priv->pal_num != 0)
      {
        p_render->pal_entry = p_priv->p_pal;
        p_render->pal_num = p_priv->pal_num;
      }

      p_render->use_bg_color = FALSE;
      p_render->bg_color = 0;
      p_render->enable_ckey = output.enable_ckey;
      p_render->color_key = output.ckey;

      p_render->pitch = output.pitch;
      p_render->not_update = TRUE;

      if(!is_invalid_rect(&p_render->rend_rect) && !is_invalid_rect(&p_render->fill_rect))
      {
        p_out_pin->query_sample(p_out_pin, &p_out_sample);

        if(p_out_sample == NULL)
        {
          mtos_align_free(p_render);

          return 0;
        }

        p_out_sample->p_data = (u8 *)p_render;
        p_out_sample->format.stream_type = MT_IMAGE;
        p_out_sample->format.media_type = MF_IMAGE;

        p_out_pin->push_down(p_out_pin, p_out_sample);
      }
    }
  }

  if(p_render != NULL)
  {
    mtos_align_free(p_render);
    p_render = NULL;
  }

  if(output.p_alpha != NULL)
  {
    //vector font, use step_width & step_height.
    return MAX(output.xoffset + output.width, output.step_width);
  }
  else
  {
    //array font, use width & height directly.
    return output.width;
  }
}

static RET_CODE unicode_trans(handle_t _this,
  u16 *p_unicode, u32 style, str_align_t align, BOOL is_break_word)
{
  str_filter_private_t *p_priv =  get_priv(_this);
  u16 *p_str_convert = NULL, *p_str_base = NULL, *p_tmp = NULL;
  u16 area_width = 0, area_height = 0;
  u16 char_code = 0, word_width = 0, chars = 0;
  u16 left = 0, top = 0, char_w = 0;
  u16 str_len = 0, tot_h = 0, i = 0, j = 0, l = 0, lines = 0;
  u8 BPP = 0;
  BOOL is_arabic = FALSE, is_arabic_lang = FALSE;

  BPP = p_priv->bpp / 8;

  if(p_priv->rsc_get_glyph == NULL)
  {
    return ERR_FAILURE;
  }

  if(p_unicode == NULL)
  {
    return ERR_FAILURE;
  }
  
  if(*p_unicode == 0xFEFF)
  {
  	//skip unicode identifier.
  	p_unicode++;	
  }

  str_len = uni_strlen(p_unicode);

  p_str_convert = (u16 *)mtos_malloc(sizeof(u16) * (str_len + 1));
  MT_ASSERT(p_str_convert != NULL);
  memset(p_str_convert, 0, sizeof(u16) * (str_len + 1));

  p_str_base = (u16 *)mtos_malloc(sizeof(u16) * (str_len + 1));
  MT_ASSERT(p_str_base != NULL);
  memset(p_str_base, 0, sizeof(u16) * (str_len + 1));

  if((is_arabic_uni_code(p_unicode[0]) == TRUE) && (align & STR_LEFT))
  {
    align = align | STR_RIGHT;
  }

  //convert ara string.
  for(i = 0; i < str_len; i++)
  {
    arabic_input_char_convert(p_str_base, p_str_convert, p_unicode[i]);
  }

  area_width = RECTW(p_priv->render_rect);
  area_height = RECTH(p_priv->render_rect);

  //caculate lines.
  lines = str_calc_lines(p_priv, p_str_convert, style, is_break_word);

  for(i = 0; i < lines; i++)
  {
    tot_h += p_priv->p_lineh[i];
  }

  //clear current line.
  str_clear_line(p_priv, tot_h);

  top = p_priv->s_y;
  //draw line.
  for(i = 0; i < lines; i++)
  {
    char_code = *(p_priv->pp_linep[i]);
    is_arabic = is_arabic_uni_code(char_code);

    if(align == STR_RIGHT)
    {
      if(is_arabic)
      {
        left = area_width;
        is_arabic_lang = TRUE;
      }
      else
      {
        left = area_width - p_priv->p_linew[i];
        is_arabic_lang = FALSE;
      }
    }
    else if(align == STR_LEFT)
    {
      if(is_arabic)
      {
        left = p_priv->p_linew[i];
        is_arabic_lang = TRUE;
      }
      else
      {
        left = 0;
        is_arabic_lang = FALSE;
      }
    }
    else
    {
      if(is_arabic)
      {
        left = ((area_width + p_priv->p_linew[i]) >> 1);
        is_arabic_lang = TRUE;
      }
      else
      {
        left = ((area_width - p_priv->p_linew[i]) >> 1);
        is_arabic_lang = FALSE;
      }
    }

    if(is_arabic)
    {
      str_draw_font_bg(p_priv,
        (left - p_priv->p_linew[i]), top, p_priv->p_linew[i], p_priv->p_lineh[i]);
    }
    else
    {
      str_draw_font_bg(p_priv,
        left, top, p_priv->p_linew[i], p_priv->p_lineh[i]);
    }

    for(j = 0; j < p_priv->p_lines[i];)
    {
      p_tmp = p_priv->pp_linep[i] + j;

      char_code = *p_tmp;

      is_arabic = is_arabic_uni_code(char_code);

      if(is_arabic || ((char_code == 0x20) && is_arabic_lang))
      {
        chars = str_get_right_string(p_priv, p_tmp,
          p_priv->pp_linep[i] + p_priv->p_lines[i], &word_width, is_arabic_lang, style);

        if(!is_arabic_lang)
        {
          left = left + word_width;
        }

        for(l = 0; l < chars; l++)
        {
          char_code = *(p_tmp + l);

          char_w = str_draw_char(p_priv, char_code, style, left, top, FALSE);

          left = left - char_w;

          j++;
        }

        if(!is_arabic_lang)
        {
          left = left + word_width;
        }
      }
      else
      {
        chars = str_get_left_string(p_priv, p_tmp,
          p_priv->pp_linep[i] + p_priv->p_lines[i], &word_width, is_arabic_lang, style);

        if(is_arabic_lang)
        {
          left = left - word_width;
        }

        for(l = 0; l < chars; l++)
        {
          char_code = *(p_tmp + l);

          char_w = str_draw_char(p_priv, char_code, style, left, top, TRUE);

          left = left + char_w;

          j++;
        }

        if(is_arabic_lang)
        {
          left = left - word_width;
        }
      }

    }

    top += p_priv->p_lineh[i];
  }

  p_priv->s_y += tot_h;

  //free buf
  if(p_str_convert != NULL)
  {
    mtos_free(p_str_convert);
    p_str_convert = NULL;
  }

  if(p_str_base != NULL)
  {
    mtos_free(p_str_base);
    p_str_base = NULL;    
  }

  return SUCCESS;
}

static RET_CODE string_trans(handle_t _this, str_str_t *p_char)
{
  str_filter_private_t *p_priv =  get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  media_sample_t *p_out_sample = NULL;
  rend_param_t *p_render = NULL;
  str_data_t *p_data = NULL;
  str_fmt_t str_charset = p_priv->str_charset;
  u32 *p_color = NULL;
  u16 *p_dst = NULL;
  u8 i = 0;

  if((p_char == NULL) || (p_char->p_data == NULL) || (p_char->lines == 0))
  {
    return ERR_FAILURE;
  }

  p_data = p_char->p_data;

  for(i = 0; i < p_char->lines; i++)
  {
    p_dst = (u16 *)mtos_malloc((p_data->str_len + 1) * sizeof(u16));
    MT_ASSERT(p_dst != NULL);

    memset(p_dst, 0, (p_data->str_len + 1) * sizeof(u16));
    //p_priv->str_charset = STR_UTF8;

    switch(str_charset)
    {
      case STR_ASCII:
        str_nasc2uni(p_data->p_str, p_dst, p_data->str_len);

        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
        break;

      case STR_GB2312:
        //gb2312_to_unicode(p_data->p_str, p_data->str_len, p_dst, p_data->str_len);
        {
          char *p_inbuf = p_data->p_str;
          char *p_outbuf = (char *)p_dst;
          int src_len =  strlen(p_inbuf) + 1;
          int dest_len = (p_data->str_len + 1) * sizeof(u16);
          iconv(p_priv->gb2312_to_utf16le, &p_inbuf, &src_len, &p_outbuf, &dest_len);
        }
        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
        break;

      case STR_UNICODE:
        memcpy((u8 *)p_dst, p_data->p_str, p_data->str_len);
        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
        break;

      case STR_UTF8:
        utf8_to_unicode(p_dst, p_data->str_len, p_data->p_str, p_data->str_len);

        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
        break;

     case STR_CP1256:
        //iso8859_to_unicode(6, p_data->p_str, p_data->str_len, p_dst, p_data->str_len);
        cp1256_to_unicode(p_data->p_str, p_data->str_len, p_dst, p_data->str_len);

        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
       break;

     case STR_8859_9:
        iso8859_to_unicode(9, p_data->p_str, p_data->str_len, p_dst, p_data->str_len);

        unicode_trans(_this, p_dst, p_data->str_style, p_data->align, FALSE);
       break;

      default:
        MT_ASSERT(0);
        break;
    }

    mtos_free(p_dst);

    p_data = (str_data_t *)((u8 *)p_data + sizeof(str_data_t) + p_data->str_len);

  }

  //clear the additional area.
  p_render = (rend_param_t *)mtos_align_malloc(4 + sizeof(rend_param_t), 8);
  MT_ASSERT(p_render != NULL);

  memset((u8 *)p_render, 0, sizeof(rend_param_t) + 4);

  if(p_priv->s_y < p_priv->pre_h)
  {
    p_render->rend_rect.top = p_priv->render_rect.top + p_priv->s_y;
    p_render->rend_rect.left = p_priv->render_rect.left;
    p_render->rend_rect.right = p_priv->render_rect.right;
    p_render->rend_rect.bottom = p_priv->render_rect.bottom;

    p_render->use_bg_color = FALSE;
    p_render->bg_color = 0x0;
    p_render->pal_entry = p_priv->p_pal;
    p_render->pal_num = p_priv->pal_num;
    p_render->not_update = FALSE;

    p_color = (u32 *)((u8 *)p_render + sizeof(rend_param_t));

    *p_color = p_priv->win_bg;

    p_out_pin->query_sample(p_out_pin, &p_out_sample);

    if(p_out_sample == NULL)
    {
      mtos_align_free(p_render);

      return ERR_FAILURE;
    }

    p_out_sample->p_data = (u8 *)p_render;
    p_out_sample->format.stream_type = MT_IMAGE;
    p_out_sample->format.media_type = MF_COLOR;

    p_out_pin->push_down(p_out_pin, p_out_sample);

  }
  else
  {
    p_out_pin->query_sample(p_out_pin, &p_out_sample);

    if(p_out_sample == NULL)
    {
      mtos_align_free(p_render);

      return ERR_FAILURE;
    }

    p_out_sample->p_data = (u8 *)p_render;
    p_out_sample->format.stream_type = MT_IMAGE;
    p_out_sample->format.media_type = MF_CMD;

    p_out_pin->push_down(p_out_pin, p_out_sample);

  }

  p_priv->pre_h = p_priv->s_y;

  mtos_align_free(p_render);

  return SUCCESS;
}

static RET_CODE image_trans(handle_t _this, str_img_t *p_img_data)
{
  rend_param_t *p_render = NULL;
  str_filter_private_t *p_priv =  get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  u16 img_width = 0, img_height = 0;
  u16 rend_w = 0, rend_h = 0;
  media_sample_t *p_out_sample = NULL;

  if((p_priv == NULL) || (p_img_data == NULL))
  {
    return ERR_FAILURE;
  }

  p_out_pin->query_sample(p_out_pin, &p_out_sample);

  if(p_out_sample == NULL)
  {
    return ERR_FAILURE;
  }

  img_width = p_img_data->image_width;
  img_height = p_img_data->image_height;

  rend_w = RECTW(p_priv->render_rect);
  rend_h = RECTH(p_priv->render_rect);

  p_render = (rend_param_t *)mtos_align_malloc(sizeof(rend_param_t), 8);
  MT_ASSERT(p_render != NULL);
  memset(p_render, 0, sizeof(rend_param_t));

  switch(p_img_data->image_align)
  {
    case STR_CENTER:
      if(img_width <= rend_w)
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = ((rend_w - img_width) >> 1);
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = ((rend_w - img_width) >> 1);
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      else
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = rend_w;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = ((img_width - rend_w) >> 1);
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = ((rend_w - img_width) >> 1);
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = ((img_width - rend_w) >> 1);
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      break;

    case STR_LEFT:
      if(img_width <= rend_w)
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      else
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = rend_w;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      break;

    case STR_RIGHT:
      if(img_width <= rend_w)
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = (rend_w - img_width);
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = (rend_w - img_width);
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = 0;
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = img_width;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      else
      {
        if(img_height <= rend_h)
        {
          p_render->rend_rect.left = 0;
          p_render->rend_rect.top = ((rend_h - img_height) >> 1);
          p_render->rend_rect.right = rend_w;
          p_render->rend_rect.bottom = p_render->rend_rect.top + img_height;

          p_render->fill_rect.left = (img_width - rend_w);
          p_render->fill_rect.top = 0;
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = img_height;
        }
        else
        {
          p_render->rend_rect.left = (rend_w - img_width);
          p_render->rend_rect.top = 0;
          p_render->rend_rect.right = p_render->rend_rect.left + img_width;
          p_render->rend_rect.bottom = rend_h;

          p_render->fill_rect.left = (img_width - rend_w);
          p_render->fill_rect.top = ((img_height - rend_h) >> 1);
          p_render->fill_rect.right = p_render->fill_rect.left + rend_w;
          p_render->fill_rect.bottom = p_render->fill_rect.top + rend_h;
        }
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  p_render->fmt = p_priv->fmt;
  p_render->use_bg_color = FALSE;
  p_render->bg_color = 0x0;
  p_render->pal_entry = p_priv->p_pal;
  p_render->pal_num = p_priv->pal_num;

  p_render->image_height = img_height;
  p_render->image_width = img_width;
  p_render->pitch = p_img_data->image_pitch;

  p_render->p_data = p_img_data->p_image;

  MT_ASSERT(((u32)p_render->p_data) % 8 == 0);

  p_out_sample->p_data = (u8 *)p_render;
  p_out_sample->format.stream_type = MT_IMAGE;
  p_out_sample->format.media_type = MF_IMAGE;

  p_out_pin->push_down(p_out_pin, p_out_sample);

  mtos_align_free(p_render);
  return SUCCESS;
}

static RET_CODE color_trans(handle_t _this, str_clr_t *p_clr_data)
{
  rend_param_t *p_render = NULL;
  str_filter_private_t *p_priv =  get_priv(_this);
  transf_output_pin_t *p_out_pin = (transf_output_pin_t *)&p_priv->m_out_pin;
  u32 *p_data = NULL;
  media_sample_t *p_out_sample = NULL;

  if((p_priv == NULL) || (p_clr_data == NULL))
  {
    return ERR_FAILURE;
  }

  p_out_pin->query_sample(p_out_pin, &p_out_sample);

  if(p_out_sample == NULL)
  {
    return ERR_FAILURE;
  }
  p_render = (rend_param_t *)mtos_align_malloc(4 + sizeof(rend_param_t), 8);
  MT_ASSERT(p_render != NULL);

  memset((u8 *)p_render, 0, sizeof(rend_param_t) + 4);

  p_render->rend_rect.top = 0;
  p_render->rend_rect.left = 0;
  p_render->rend_rect.right = RECTW(p_priv->render_rect);
  p_render->rend_rect.bottom = RECTH(p_priv->render_rect);

  p_render->fill_rect.top = 0;
  p_render->fill_rect.left = 0;
  p_render->fill_rect.right = RECTW(p_priv->render_rect);
  p_render->fill_rect.bottom = RECTH(p_priv->render_rect);

  p_render->use_bg_color = FALSE;
  p_render->bg_color = 0x0;
  p_render->pal_entry = p_priv->p_pal;
  p_render->pal_num = p_priv->pal_num;

  p_data = (u32 *)((u8 *)p_render + sizeof(rend_param_t));

  *p_data = p_clr_data->color;

  p_out_sample->p_data = (u8 *)p_render;
  p_out_sample->format.stream_type = MT_IMAGE;
  p_out_sample->format.media_type = MF_COLOR;

  p_out_pin->push_down(p_out_pin, p_out_sample);

  mtos_align_free(p_render);
  return SUCCESS;
}

static RET_CODE str_transform(handle_t _this, media_sample_t *p_in)
{
  str_filter_private_t *p_priv = get_priv(_this);
  str_param_t *p_str_param = NULL;

  if(p_in == NULL)
  {
    return ERR_FAILURE;
  }

  p_priv->s_y = 0;

  p_str_param = (str_param_t *)p_in->p_data;

  if(p_str_param == NULL)
  {
    return ERR_FAILURE;
  }

  switch(p_str_param->fmt)
  {
    case STR_ASCII:
    case STR_UNICODE:
    case STR_GB2312:
    case STR_UTF8:
      string_trans(_this, &p_str_param->str_union.str_data);//, p_str_param->fmt);
      break;

    case STR_IMAGE:
      image_trans(_this, &p_str_param->str_union.img_data);
      break;

    case STR_COLOR:
      color_trans(_this, &p_str_param->str_union.clr_data);
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return SUCCESS;
}

static void str_on_destory(handle_t _this)
{
  str_filter_private_t *p_priv =  get_priv(_this);

  p_priv->pre_h = 0;
  p_priv->s_y = 0;

  if(p_priv->p_pal != NULL)
  {
    mtos_align_free(p_priv->p_pal);
  }

  if(p_priv->utf8_to_utf16le != (iconv_t)-1)
  {
    iconv_close(p_priv->utf8_to_utf16le);
  }
  if(p_priv->gb2312_to_utf16le != (iconv_t)-1)
  {
    iconv_close(p_priv->gb2312_to_utf16le);
  }

  if (p_priv->p_linew)
  {
    mtos_free(p_priv->p_linew);
    p_priv->p_linew = NULL;
  }
  if (p_priv->p_lineh)
  {
    mtos_free(p_priv->p_lineh);
    p_priv->p_lineh = NULL;
  }
  if (p_priv->p_lines)
  {
    mtos_free(p_priv->p_lines);
    p_priv->p_lines = NULL;
  }
  if (p_priv->pp_linep)
  {
    mtos_free(p_priv->pp_linep);
    p_priv->pp_linep = NULL;
  }
  if(_this != NULL)
  {
    mtos_free((void *)_this);
  }
}

ifilter_t * str_filter_create(void *p_para)
{
  str_filter_private_t *p_priv = NULL;
  str_filter_t *p_ins = NULL;
  transf_filter_t *p_transf_filter = NULL;
  ifilter_t *p_ifilter = NULL;
  transf_filter_para_t transf_filter_para;
  interface_t *p_interface = NULL;
  str_para_t *p_str_para = NULL;

  transf_filter_para.dummy = 0;
  p_ins = mtos_malloc(sizeof(str_filter_t));
  MT_ASSERT(p_ins != NULL);
  memset(p_ins, 0, sizeof(str_filter_t));
  //create base class
  transf_filter_create(&p_ins->m_filter, &transf_filter_para);

  //init private date
  p_priv = &p_ins->private_data;
  memset(p_priv, 0, sizeof(str_filter_private_t));

  p_priv->p_this = p_ins; //this point
  p_priv->p_output_sample = NULL;

  p_priv->p_pal = mtos_align_malloc(1024, 8);
  MT_ASSERT(p_priv->p_pal != NULL);

  //init member function

  //over loading the virtual function
  p_transf_filter = (transf_filter_t *)p_ins;
  p_transf_filter->transform = str_transform;

  p_ifilter = (ifilter_t *)p_ins;
  p_ifilter->on_command = str_on_command;
  p_ifilter->on_start = str_on_start;
  p_ifilter->on_stop = str_on_stop;
  p_ifilter->on_close = str_on_close;

  p_interface = (interface_t *)p_ins;
  p_interface->on_destroy = str_on_destory;

  //init resource handle.
  p_str_para = (str_para_t *)p_para;

  if(p_str_para != NULL)
  {
    p_priv->rsc_get_glyph = p_str_para->rsc_get_glyph;
  }

  p_priv->str_charset = 0;

  p_priv->utf8_to_utf16le = iconv_open("ucs2le", "utf8");
  p_priv->gb2312_to_utf16le = iconv_open("ucs2le", "euccn");

  p_priv->p_linew = (u16 *)mtos_malloc(STR_FILTER_MAX_LINES * sizeof(u16));
  MT_ASSERT(p_priv->p_linew != NULL);
  p_priv->p_lineh = (u16 *)mtos_malloc(STR_FILTER_MAX_LINES * sizeof(u16));
  MT_ASSERT(p_priv->p_lineh != NULL);
  p_priv->p_lines = (u16 *)mtos_malloc(STR_FILTER_MAX_LINES * sizeof(u16));
  MT_ASSERT(p_priv->p_lines != NULL);
  p_priv->pp_linep = (u16 **)mtos_malloc(STR_FILTER_MAX_LINES * sizeof(u16 *));
  MT_ASSERT(p_priv->pp_linep != NULL);

  //create it's pin
  str_in_pin_create(&p_priv->m_in_pin, (interface_t *)p_ins);
  str_out_pin_create(&p_priv->m_out_pin, (interface_t *)p_ins);

  return (ifilter_t *)p_ins;
}


