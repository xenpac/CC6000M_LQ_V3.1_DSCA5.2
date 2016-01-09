/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_language.h"


enum control_id
{
  IDC_INVALID = 0,
  IDC_LANGUAGE,
  IDC_FIRST_AUDIO,
  IDC_SECOND_AUDIO,
#if ENABLE_TTX_SUBTITLE
  IDC_SUBTITLE,
  IDC_TELETEXT
#endif
};
/*
static comm_help_data_t language_help_data =
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
u16 language_cont_keymap(u16 key);
static u16 opt_language[LANGUAGE_MAX_CNT] = {0};

RET_CODE language_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


/*
	\param[in] lang_index: language index in  language resource (the array opt_data[])

	 \return focus : language index in language array selected by Tcom (the array opt_language , init this array in open_language)
*/
static u8 get_ui_focus(u8 lang_index)
{
	u8 focus;
	u16 opt_data[LANGUAGE_MAX_CNT] =
	  {
	    IDS_LANGUAGE_ENGLISH, IDS_LANGUAGE_SIMPLIFIED_CHINESE,
	  };

	for(focus = 0; focus < LANGUAGE_MAX_CNT; focus ++)
	{
		if(opt_data[lang_index] == opt_language[focus])
			return focus;
	}

	return focus;
}

/*
	\param[in] focus: language index in  language array selected by Tcom (the array opt_language , init this array in open_language)

	 \return index : language index in language resource (the array opt_data[])
*/
static u8 get_lang_index(u8 focus)
{
	u8 index;
	u16 opt_data[LANGUAGE_MAX_CNT] =
	  {
	    IDS_LANGUAGE_ENGLISH, IDS_LANGUAGE_SIMPLIFIED_CHINESE,
	  };

	for(index = 0 ; index < LANGUAGE_MAX_CNT; index ++)
	{
		if(opt_language[focus] == opt_data[index])
			return index;
	}

	return index;
}

RET_CODE open_language(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[LANGUAGE_ITEM_CNT];
  u8 i = 0, j = 0, opt_cnt, focus = 0;
  u16 y;
  language_set_t lang_set;
//  hw_cfg_t hw_cfg = dm_get_hw_cfg_info();

#if ENABLE_TTX_SUBTITLE
  u16 stxt[LANGUAGE_ITEM_CNT] =
  {
    IDS_LANGUAGE, IDS_FIRST_AUDIO, IDS_SECOND_AUDIO,
    IDS_SUBTITLE_LANGUAGE, IDS_TELETEXT_LANGUAGE
  };
#else
  u16 stxt[LANGUAGE_ITEM_CNT] =
  {
    IDS_LANGUAGE, IDS_FIRST_AUDIO, IDS_SECOND_AUDIO
  };
#endif

  /*
  Tcom Language Queen, Should Match it
  Chinese->English->French->Russian->Arabic->Farsi->Indonesian
  Portuguese->TurKish->German->Italian->Spanish->Polish
  */
  u16 opt_data[2] =
  {
    IDS_LANGUAGE_ENGLISH,IDS_LANGUAGE_SIMPLIFIED_CHINESE
  };

  sys_status_get_lang_set(&lang_set);

  opt_cnt = customer_config_language_num_get();
  
  if(opt_cnt > 2)
  {
    opt_cnt = 2;//to do
  }
  while(i < opt_cnt)
  {
    opt_language[i] = opt_data[i];
    i++;
  }

  p_cont = ui_comm_root_create(ROOT_ID_LANGUAGE,
                               0,
                               COMM_BG_X,
                               COMM_BG_Y,
                               COMM_BG_W,
                               COMM_BG_H,
                               IM_TITLEICON_TV,
                               IDS_LANGUAGE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, language_cont_keymap);

  y = LANGUAGE_ITEM_Y;
  for(i = 0; i < LANGUAGE_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_LANGUAGE + i),
                                      LANGUAGE_ITEM_X, y,
                                      LANGUAGE_ITEM_LW,
                                      LANGUAGE_ITEM_RW);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], language_select_proc);
    ui_comm_select_set_param(p_ctrl[i], TRUE,
                               CBOX_WORKMODE_STATIC, opt_cnt,
                               CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0; j < opt_cnt; j++)
    {
      ui_comm_select_set_content(p_ctrl[i], j, opt_language[j]);
    }

    ui_comm_select_create_droplist(p_ctrl[i], COMM_SELECT_DROPLIST_PAGE);
    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + LANGUAGE_ITEM_CNT) % LANGUAGE_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % LANGUAGE_ITEM_CNT + 1)); /* down */

    y += LANGUAGE_ITEM_H + LANGUAGE_ITEM_V_GAP;
  }

  /* set focus according to current info */
  focus = get_ui_focus(lang_set.osd_text);;
  ui_comm_select_set_focus(p_ctrl[0], focus);

  focus = get_ui_focus(lang_set.first_audio);
  ui_comm_select_set_focus(p_ctrl[1], focus);

  focus = get_ui_focus(lang_set.second_audio);
  ui_comm_select_set_focus(p_ctrl[2], focus);
#if ENABLE_TTX_SUBTITLE
  focus = get_ui_focus(lang_set.sub_title);
  ui_comm_select_set_focus(p_ctrl[3], focus);

  focus = get_ui_focus(lang_set.tel_text);
  ui_comm_select_set_focus(p_ctrl[4], focus);
#endif
  //ui_comm_help_create(&language_help_data, p_cont);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

static RET_CODE on_select_change_focus(control_t *p_ctrl,
                                       u16 msg,
                                       u32 para1,
                                       u32 para2)
{
  control_t *p_root, *p_subctrl;
  language_set_t lang_set;
  u8 focus = (u8)para2;
  u8 lang_index;
  u8 fav_name[32] = {0};
  u16 fav_name_unistr[32] = {0};
  u16 content[32];
  u16 i;

  lang_index = get_lang_index(focus);

  sys_status_get_lang_set(&lang_set);
  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_LANGUAGE:
      lang_set.osd_text = lang_index;

      if(ctrl_is_onfocus(p_ctrl))
      {
        // reset all language setting
        lang_set.first_audio = lang_index;
        lang_set.second_audio = lang_index;
        lang_set.sub_title = lang_index;
        lang_set.tel_text = lang_index;

        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_LANGUAGE, IDC_FIRST_AUDIO);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_LANGUAGE, IDC_SECOND_AUDIO);
        ui_comm_select_set_focus(p_subctrl, focus);
#if ENABLE_TTX_SUBTITLE
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_LANGUAGE, IDC_SUBTITLE);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_LANGUAGE, IDC_TELETEXT);
        ui_comm_select_set_focus(p_subctrl, focus);
#endif
        // set language & redraw
        rsc_set_curn_language(gui_get_rsc_handle(), lang_index + 1);

        for(i=0; i<5; i++)
        {
          memset(content, 0, sizeof(content));
          gui_get_string(IDS_FAV_LIST, content, 64);
          sprintf((char *)fav_name, "%d", i+1);
          str_asc2uni(fav_name, fav_name_unistr);
          uni_strcat(content, fav_name_unistr, 256);
          sys_status_set_fav_name((u8)(sys_status_get_categories_count() - 5 + i), content);
        }

#ifdef ENABLE_CA
        if((CAS_ID == CONFIG_CAS_ID_DS)|| (CAS_ID == CONFIG_CAS_ID_SC))
        {
          ui_ca_set_language(lang_set);
        }
#endif
        if((p_root = fw_get_focus()) != NULL)
        {
          if(p_root != NULL)
          {
            ctrl_paint_ctrl(p_root, TRUE);
          }
        }
      }
      break;
    case IDC_FIRST_AUDIO:
      lang_set.first_audio = lang_index;
      break;
    case IDC_SECOND_AUDIO:
      lang_set.second_audio = lang_index;
      break;
#if ENABLE_TTX_SUBTITLE
    case IDC_SUBTITLE:
      lang_set.sub_title = lang_index;
      break;
    case IDC_TELETEXT:
      lang_set.tel_text = lang_index;
      break;
#endif
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }
  sys_status_set_lang_set(&lang_set);
  sys_status_save();

  return SUCCESS;
}

BEGIN_MSGPROC(language_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_select_change_focus)
END_MSGPROC(language_select_proc, cbox_class_proc)

BEGIN_KEYMAP(language_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(language_cont_keymap, ui_comm_root_keymap)
