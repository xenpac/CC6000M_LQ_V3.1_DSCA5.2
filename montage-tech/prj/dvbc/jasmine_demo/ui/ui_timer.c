/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#include "ui_timer.h"
#include "data_base16v2.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TIMER_TITLE,
  IDC_TIMER_CURN_TIME,
  IDC_TIMER_REPEAT,
  IDC_TIMER_CHANNEL,
  IDC_TIMER_DATE,
  IDC_TIMER_TIME,
  IDC_TIMER_END,
  IDC_TIMER_RECORD,
  IDC_TIMER_SAVE,
  IDC_TIMER_CANCLE,
};

static list_xstyle_t timer_dlist_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_HL,
  RSI_ITEM_1_SH,
};
/*
static comm_help_data_t timer_help_data = 
{
2,2,
  {IDS_MENU,IDS_EXIT},
  {IM_MENU,IM_EXIT}
};
*/
#define NOT(x) ((x) == 0)

static u32 menu_type;
static u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ' ', ' '};
static u8 timer_set_focus = MAX_BOOK_PG;
static book_pg_t tmr_node;
static u8 *p_ext_buf = NULL;
static u8 timer_vid = 0xFF;
u16 *p_unistr[6] = {NULL};
char *time_str[6] = {"-", "-", " ", ":","(+1D)"," "};

u16 timer_cont_keymap(u16 key);
RET_CODE timer_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 timer_btn_keymap(u16 key);
RET_CODE timer_btn_proc(control_t *p_btn, u16 msg, u32 para1, u32 para2);

RET_CODE timer_comm_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE close_timer(u32 para1, u32 para2);

static RET_CODE timer_set_curn_time(control_t *p_ctrl, BOOL is_paint)
{
  utc_time_t utc_time;
  u8 time_str_temp[32];
  //time_set_t p_set={{0}};

  time_get(&utc_time, FALSE);

  sprintf((char *)time_str_temp, "%.2d:%.2d %.4d-%.2d-%.2d",
          utc_time.hour, utc_time.minute, utc_time.year, utc_time.month, utc_time.day);

  ui_comm_static_set_content_by_ascstr(p_ctrl, time_str_temp);

  if(is_paint)
  {
    ui_comm_ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  return SUCCESS;
}


static RET_CODE fill_channel(control_t *p_ctrl, u16 focus, u16 *p_str,
                             u16 max_length)
{
  dvbs_prog_node_t pg;
  u16 pgid;

  pgid = db_dvbs_get_id_by_view_pos(timer_vid, focus);
  db_dvbs_get_pg_by_id(pgid, &pg);

  ui_dbase_get_full_prog_name(&pg, p_str, 18);  // pg name max_length
  return SUCCESS;
}


static void timer_active_menu(control_t *p_cont, BOOL is_active, BOOL is_paint)
{
  control_t *p_ctrl;
  u8 i;

  for(i = 0; i < 5; i++)
  {
    p_ctrl = ctrl_get_child_by_id(p_cont, (u8)(IDC_TIMER_CHANNEL + i));

    ctrl_set_attr(p_ctrl, is_active ? OBJ_ATTR_ACTIVE : OBJ_ATTR_INACTIVE);
    if(is_paint)
    {
      ctrl_paint_ctrl(p_ctrl, TRUE);
    }
  }
}


static RET_CODE on_timer_comm_select_change_focus(control_t *p_ctrl,
                                                  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 focus;
  control_t *p_parent;
  BOOL is_onfocus = ctrl_is_onfocus(p_ctrl);

  // before switch

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);

  // after switch
  p_parent = ctrl_get_parent(p_ctrl);
  switch(ctrl_get_ctrl_id(p_parent))
  {
    case IDC_TIMER_REPEAT:
      if(menu_type != FROM_EPG_MENU)
      {
        focus = cbox_static_get_focus(p_ctrl);
        if(focus == 0)
        {
          timer_active_menu(ctrl_get_parent(p_parent), FALSE, is_onfocus);
        }
        else
        {
          timer_active_menu(ctrl_get_parent(p_parent), TRUE, is_onfocus);
        }
      }
      break;
    case IDC_TIMER_CHANNEL:
      break;
    default:
      /* do nothing */;
  }

  return ret;
}


static RET_CODE on_timer_btn_change_focus(control_t *p_btn,
                                          u16 msg, u32 para1, u32 para2)
{
  return ctrl_process_msg(ctrl_get_parent(p_btn), msg, 0, 0);
}


static RET_CODE on_timer_time_update(control_t *p_cont,
                                     u16 msg, u32 para1, u32 para2)
{
  control_t *p_time,*p_start_time,*p_end_time;
  utc_time_t start = {0};
  utc_time_t end = {0};
  u8 j = 0; 
  static utc_time_t record_time = {0};
  p_time = ctrl_get_child_by_id(p_cont, IDC_TIMER_CURN_TIME);
  timer_set_curn_time(p_time, TRUE);

  p_start_time = ctrl_get_child_by_id(p_cont, IDC_TIMER_TIME);
  ui_comm_timedit_get_time(p_start_time, &start);

  p_end_time = ctrl_get_child_by_id(p_cont, IDC_TIMER_END);
  ui_comm_timedit_get_time(p_end_time, &end);

 if(time_cmp(&end, &start, FALSE) < 0)
  {
    for(j = 0; j < 6; j++)
    {
      if(j == 4)
      {
        ui_comm_timedit_set_param(p_end_time, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
                                                      TBOX_SEPARATOR_TYPE_EXTSTR, 18);  
      
        str_asc2uni((u8 *)time_str[j], p_unistr[j]);
        ui_comm_timedit_set_separator_by_extstr(p_end_time, j, p_unistr[j]);
      }
      else
      {
        ui_comm_timedit_set_param(p_end_time, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
        TBOX_SEPARATOR_TYPE_EXTSTR, 18);   
          str_asc2uni((u8 *)time_str[j], p_unistr[j]);
        ui_comm_timedit_set_separator_by_extstr(p_end_time , j,p_unistr[j]);
      }
    }
    if(time_cmp(&record_time,&end,TRUE) == 0)
      ctrl_paint_ctrl(p_end_time, FALSE);
    else
    {
      memcpy(&record_time,&end,sizeof(utc_time_t));
      ctrl_paint_ctrl(p_end_time, TRUE);
    }
  }
  else
  {
  ui_comm_timedit_set_param(p_end_time, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
  TBOX_SEPARATOR_TYPE_UNICODE, 18);     
  for(j = 0; j < 6; j++)
  {
  ui_comm_timedit_set_separator_by_ascchar(p_end_time, j, separator[j]); 
  }
  if(time_cmp(&record_time,&end,TRUE) == 0)
  ctrl_paint_ctrl(p_end_time, FALSE);
  else
  {
  memcpy(&record_time,&end,sizeof(utc_time_t));
  ctrl_paint_ctrl(p_end_time, TRUE);
  }
  }
  return SUCCESS;
}


static RET_CODE on_timer_exit(control_t *p_cont,
                              u16 msg, u32 para1, u32 para2)
{
  mtos_free(p_ext_buf);

  return ERR_NOFEATURE;
}


static RET_CODE on_timer_btn_select(control_t *p_btn,
                                    u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_mode,*p_repeat;
  control_t *p_channel;
  control_t *p_date, *p_time, *p_end;
  u16 mode, channel,record;
  utc_time_t start = {0};
  utc_time_t end = {0};
  utc_time_t dur = {0};
  book_pg_t node;

  memset((void *)&node, 0, sizeof(book_pg_t));

  if(IDC_TIMER_SAVE == ctrl_get_ctrl_id(p_btn))   //save
  {
    p_repeat = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_REPEAT);
    p_channel = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_CHANNEL);
    p_date = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_DATE);
    p_time = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_TIME);
    p_end = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_END);
    p_mode = ctrl_get_child_by_id(ctrl_get_parent(p_btn), IDC_TIMER_RECORD);
    mode = ui_comm_select_get_focus(p_repeat);
    record = ui_comm_select_get_focus(p_mode);    
    channel = ui_comm_numsel_get_focus(p_channel);
    ui_comm_timedit_get_time(p_date, &start);
    ui_comm_timedit_get_time(p_time, &start);
    ui_comm_timedit_get_time(p_date, &end);
    ui_comm_timedit_get_time(p_end, &end);
    if(time_cmp(&end,&start, FALSE) < 0)
    {
      end.day =  end.day + 1; //end time is tomorrw   
    }
    time_up(&dur,time_dec(&end,&start));
    node.pgid = db_dvbs_get_id_by_view_pos(timer_vid, channel);
    node.book_mode = (u8)mode;
    node.start_time = start;
    node.drt_time = dur;
    node.record_enable = record;

    if(mode == BOOK_TMR_OFF) //off
    {
      if(menu_type != FROM_EPG_MENU)
      {
        //close the timer, del it from system status.(if from timer set menu.)
        book_delete_node(timer_set_focus);

        fw_notify_parent(ROOT_ID_TIMER, NOTIFY_T_MSG, FALSE,
                         MSG_TIMER_UPDATE, timer_set_focus, 0);

        ret = close_timer(0, 0);
      }
      else
      {
        if(timer_set_focus != MAX_BOOK_PG)
        {
          book_delete_node(timer_set_focus);
        }

        ret = close_timer(0, 0);
      }
    }
    else
    {
      //to check if the event is playing.
      if(is_node_playing(&node))
      {
        //popup a dialog for overdue.
        ui_comm_cfmdlg_open(NULL, IDS_MSG_EVENT_IS_PLAYING, NULL, 0);
        return ret;
      }

      if(is_time_booked(&node, (BOOL)menu_type ? timer_set_focus : MAX_BOOK_PG))
      {
        //popup a dialog for duplicate time
        ui_comm_cfmdlg_open(NULL, IDS_MSG_TIMER_CONFLICT, NULL, 0);
        return ret;
      }

      if(menu_type != FROM_EPG_MENU) //open timer menu from timer set list or schedule
      {
        book_edit_node(timer_set_focus, &node);

        fw_notify_parent(ROOT_ID_TIMER, NOTIFY_T_MSG, FALSE,
                         MSG_TIMER_UPDATE, timer_set_focus, 0);

        ret = close_timer(0, 0);
      }
      else //open timer menu from epg menu.
      {
        if(timer_set_focus != MAX_BOOK_PG)
        {
          book_edit_node(timer_set_focus, &node);
          ret = close_timer(0, 0);
        }
        else
        {
          //to check if the book space is full, if yes, return.
          if(MAX_BOOK_PG == book_get_book_num())
          {
            //popup a dialog about book full.
            ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, NULL, 0);
            return ret;
          }

          book_add_node(&node);
          ret = close_timer(0, 0);
        }
      }
    }
  }
  else
  {
    ret = close_timer(0, 0);
  }

  return ret;
}

//lint -e732 -e830
RET_CODE open_timer(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[TIMER_ITEM_CNT];
  control_t *p_title, *p_save, *p_cancle;
  utc_time_t tmp_time = {0};
  utc_time_t end_time = {0};
  u16 stxt[TIMER_ITEM_CNT] =
  {
    RSC_INVALID_ID, IDS_REPEAT, IDS_CHANNEL,
    IDS_WAKEUP_DATE, IDS_START_TIME, IDS_END_TIME,IDS_MODE
  };
  u16 strid[4] = {IDS_OFF, IDS_ONCE, IDS_DAILY, IDS_WEEKLY};
  u16 strid_record[2] = {IDS_VIEW, IDS_RECORD};
  u16 total, y = 0;
  u8 i, j, mode, parent_id = ROOT_ID_EPG;
  
  for(i = 0;i < 6;i++)
 {
  p_unistr[i] = (u16 *)mtos_malloc(10);
 }
  menu_type = para2;

  if(menu_type == FROM_EPG_MENU)                            //from epg menu
  {
    memcpy(&tmr_node, (book_pg_t *)para1, sizeof(book_pg_t));
    timer_set_focus = book_get_match_node(&tmr_node);
  }
  else //from timer set menu or schedule menu
  {
    timer_set_focus = (u8)para1;
    sys_status_get_book_node(timer_set_focus, &tmr_node);

    if(tmr_node.book_mode == BOOK_TMR_OFF)
    {
      time_get(&tmr_node.start_time, FALSE);
      memset(&tmp_time, 0, sizeof(utc_time_t));
      tmp_time.minute = 10;
    
      time_add(&tmr_node.start_time, &tmp_time);
    }
  }

  switch(menu_type)
  {
    case FROM_EPG_MENU:
      parent_id = ROOT_ID_EPG;
      break;

    case FROM_TIMER_SETMENU:
      parent_id = ROOT_ID_BOOK_LIST;
      break;

    case FROM_SCHEDULE_MENU:
      parent_id = ROOT_ID_SCHEDULE;
      break;

    default:
      break;
  }

  p_cont = fw_create_mainwin(ROOT_ID_TIMER,
                             TIMER_CONT_X, TIMER_CONT_Y, TIMER_CONT_W, TIMER_CONT_H,
                             parent_id, 0, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_TIMER_FRM, RSI_TIMER_FRM, RSI_TIMER_FRM);
  ctrl_set_keymap(p_cont, timer_cont_keymap);
  ctrl_set_proc(p_cont, timer_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMER_TITLE,
                             TIMER_TITLE_X, TIMER_TITLE_Y, TIMER_TITLE_W, TIMER_TITLE_H,
                             p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_TIMER_SET);
  
  memset(p_ctrl, 0 , 4 * TIMER_ITEM_CNT);
  y = TIMER_ITEM_Y;
  for(i = 0; i < TIMER_ITEM_CNT; i++)
  {
    switch(i)
    {
      //current time.
      case 0:
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_TIMER_CURN_TIME + i),
                                          TIMER_ITEM_X, y, TIMER_ITEM_LW, TIMER_ITEM_RW);
        //ui_comm_ctrl_set_cont_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
        //ui_comm_ctrl_set_ctrl_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);
        ui_comm_static_set_align_type(p_ctrl[i], 0, 0, 20, STL_RIGHT | STL_VCENTER);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
        break;
      // common select control
      case 1:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_TIMER_CURN_TIME + i),
                                          TIMER_ITEM_X, y, TIMER_ITEM_LW, TIMER_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], timer_comm_select_proc);
        {
          if(menu_type != FROM_EPG_MENU)
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, 4,
                                     CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0; j < 4; j++)
            {
              ui_comm_select_set_content(p_ctrl[i], j, strid[j]);
            }
          }
          else
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, 2,
                                     CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0; j < 2; j++)
            {
              ui_comm_select_set_content(p_ctrl[i], j, strid[j]);
            }
          }
        }
        ui_comm_select_create_droplist(p_ctrl[i], 5);
        ui_comm_select_droplist_set_rstyle(p_ctrl[i],
                                           RSI_TIMER_DLIST, RSI_TIMER_DLIST, RSI_TIMER_DLIST);
        ui_comm_select_droplist_set_item_rstyle(p_ctrl[i],
                                                &timer_dlist_item_rstyle);
        ui_comm_select_droplist_set_sbar_rstyle(p_ctrl[i],
                                                RSI_TIMER_DLIST_SBAR,
                                                RSI_TIMER_DLIST_SBAR,
                                                RSI_TIMER_DLIST_SBAR);
        break;
      case 2:   // number select control
        p_ctrl[i] = ui_comm_numsel_create(p_cont, (u8)(IDC_TIMER_CURN_TIME + i),
                                          TIMER_ITEM_X, y, TIMER_ITEM_LW, TIMER_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);

        p_ext_buf = mtos_malloc(MAX_VIEW_BUFFER_SIZE);
        MT_ASSERT(p_ext_buf != NULL);
        memset(p_ext_buf, 0, MAX_VIEW_BUFFER_SIZE);
        mode = sys_status_get_curn_prog_mode();
        MT_ASSERT(mode != CURN_MODE_NONE);

        if(CURN_MODE_TV == mode)
            timer_vid = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, p_ext_buf);
        else if(CURN_MODE_RADIO == mode)
            timer_vid = ui_dbase_create_view(DB_DVBS_ALL_RADIO, 0, p_ext_buf);

        total = db_dvbs_get_count(timer_vid);
        ui_comm_numsel_set_param(p_ctrl[i], TRUE,
                                 CBOX_WORKMODE_DYNAMIC, total, 4, 0, fill_channel);

        ui_comm_numsel_create_droplist(p_ctrl[i], 5);
        ui_comm_numsel_droplist_set_rstyle(p_ctrl[i],
                                           RSI_TIMER_DLIST, RSI_TIMER_DLIST, RSI_TIMER_DLIST);
        ui_comm_numsel_droplist_set_item_rstyle(p_ctrl[i],
                                                &timer_dlist_item_rstyle);
        ui_comm_numsel_droplist_set_sbar_rstyle(p_ctrl[i],
                                                RSI_TIMER_DLIST_SBAR,
                                                RSI_TIMER_DLIST_SBAR,
                                                RSI_TIMER_DLIST_SBAR);
        break;

      // common timedit control
      case 3:
      case 4:
      case 5:
        p_ctrl[i] = ui_comm_timedit_create(p_cont, (u8)(IDC_TIMER_CURN_TIME + i),
                                           TIMER_ITEM_X, y, TIMER_ITEM_LW, TIMER_ITEM_RW);
        ui_comm_timedit_set_static_txt(p_ctrl[i], stxt[i]);
        if(i == 5)
        {
          memcpy(&end_time,&(tmr_node.start_time),sizeof(utc_time_t));
	       time_add(&end_time, &(tmr_node.drt_time));
          ui_comm_timedit_set_time(p_ctrl[i], &end_time);
        }
        else
        {
          ui_comm_timedit_set_time(p_ctrl[i], &(tmr_node.start_time));
        }

        if((tmr_node.start_time.hour - end_time.hour > 0 
           ||  ((tmr_node.start_time.hour == end_time.hour)  
           &&  (((tmr_node.start_time.minute - end_time.minute) > 0)? 1 : 0)))
           && i == 5)
        {
        for(j = 0; j < 6; j++)
        {
          if(j == 4)
          {
            ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
                  TBOX_SEPARATOR_TYPE_EXTSTR, 18);    
            str_asc2uni((u8 *)time_str[j], p_unistr[j]);
            ui_comm_timedit_set_separator_by_extstr(p_ctrl[i], j, p_unistr[j]);
          }
          else
          {
               ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
                TBOX_SEPARATOR_TYPE_EXTSTR, 18);   
                  str_asc2uni((u8 *)time_str[j], p_unistr[j]);
           ui_comm_timedit_set_separator_by_extstr(p_ctrl[i] , j,p_unistr[j]);
          }
        }
        }
        else
        {
         if(i == 3)
        {
          ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_YEAR, TBOX_YEAR | TBOX_MONTH | TBOX_DAY,
                                    TBOX_SEPARATOR_TYPE_UNICODE, 18);
        }
        else
        {
          ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
            TBOX_SEPARATOR_TYPE_UNICODE, 18);          
        }

        for(j = 0; j < TBOX_MAX_ITEM_NUM; j++)
        {
          ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], j, separator[j]);
        }
        }
        break;
        case 6:
	p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_TIMER_RECORD),
	TIMER_ITEM_X, y, TIMER_ITEM_LW, TIMER_ITEM_RW);
	ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
	ui_comm_ctrl_set_proc(p_ctrl[i], timer_comm_select_proc);
	ui_comm_select_set_param(p_ctrl[i], TRUE,
	CBOX_WORKMODE_STATIC, 2,
	CBOX_ITEM_STRTYPE_STRID, NULL);
	for(j = 0; j < 2; j++)
	{
	ui_comm_select_set_content(p_ctrl[i], j, strid_record[j]);
	}
    ui_comm_select_set_focus(p_ctrl[i], tmr_node.record_enable);
	ui_comm_select_create_droplist(p_ctrl[i], 2);
	ui_comm_select_droplist_set_rstyle(p_ctrl[i],
	RSI_TIMER_DLIST, RSI_TIMER_DLIST, RSI_TIMER_DLIST);
	ui_comm_select_droplist_set_item_rstyle(p_ctrl[i],
	&timer_dlist_item_rstyle);
	ui_comm_select_droplist_set_sbar_rstyle(p_ctrl[i],
	RSI_TIMER_DLIST_SBAR,
	RSI_TIMER_DLIST_SBAR,
	RSI_TIMER_DLIST_SBAR);
	break;
      default:
        MT_ASSERT(0);
        break;
    }
    y += TIMER_ITEM_H + TIMER_ITEM_V_GAP;
  }

  p_save = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMER_SAVE,
                            TIMER_BTN_X, y, TIMER_BTN_W, TIMER_BTN_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_save, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_save, timer_btn_keymap);
  ctrl_set_proc(p_save, timer_btn_proc);
  text_set_font_style(p_save, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_save, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_save, IDS_SAVE);

  p_cancle = ctrl_create_ctrl(CTRL_TEXT, IDC_TIMER_CANCLE,
                              (TIMER_BTN_X + TIMER_BTN_W + TIMER_BTN_HGAP),
                              y, TIMER_BTN_W, TIMER_BTN_H, p_cont, 0);
  ctrl_set_rstyle(p_cancle, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
  ctrl_set_keymap(p_cancle, timer_btn_keymap);
  ctrl_set_proc(p_cancle, timer_btn_proc);
  text_set_font_style(p_cancle, FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
  text_set_content_type(p_cancle, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_cancle, IDS_CANCEL);

	ctrl_set_related_id(p_ctrl[1], 0, IDC_TIMER_SAVE, 0, IDC_TIMER_CHANNEL); //MODE
	ctrl_set_related_id(p_ctrl[2], 0, IDC_TIMER_REPEAT, 0, IDC_TIMER_DATE); //CHANNEL
	ctrl_set_related_id(p_ctrl[3], 0, IDC_TIMER_CHANNEL, 0, IDC_TIMER_TIME); //DATE
	ctrl_set_related_id(p_ctrl[4], 0, IDC_TIMER_DATE, 0, IDC_TIMER_END); //TIME
	ctrl_set_related_id(p_ctrl[5], 0, IDC_TIMER_TIME, 0, IDC_TIMER_RECORD); //DURA
	ctrl_set_related_id(p_ctrl[6], 0, IDC_TIMER_END, 0, IDC_TIMER_SAVE); //DURA
	ctrl_set_related_id(p_save, IDC_TIMER_CANCLE, IDC_TIMER_RECORD,
	IDC_TIMER_CANCLE, IDC_TIMER_REPEAT);
	ctrl_set_related_id(p_cancle, IDC_TIMER_SAVE, IDC_TIMER_RECORD,
	IDC_TIMER_SAVE, IDC_TIMER_REPEAT);

  timer_set_curn_time(p_ctrl[0], FALSE);

  if(menu_type != FROM_EPG_MENU) //from timer set menu or schedule
  {
    ui_comm_select_set_focus(p_ctrl[1], tmr_node.book_mode);
    if(tmr_node.pgid == 0)
    {
      ui_comm_numsel_set_focus(p_ctrl[2],
                               db_dvbs_get_view_pos_by_id(timer_vid,
                                                          sys_status_get_curn_group_curn_prog_id()));
    }
    else
    {
      ui_comm_numsel_set_focus(
        p_ctrl[2], db_dvbs_get_view_pos_by_id(timer_vid, tmr_node.pgid));
    }

    if(tmr_node.book_mode == 0)
    {
      ctrl_set_attr(p_ctrl[2], OBJ_ATTR_INACTIVE);
      ctrl_set_attr(p_ctrl[3], OBJ_ATTR_INACTIVE);
      ctrl_set_attr(p_ctrl[4], OBJ_ATTR_INACTIVE);
      ctrl_set_attr(p_ctrl[5], OBJ_ATTR_INACTIVE);
      ctrl_set_attr(p_ctrl[6], OBJ_ATTR_INACTIVE);
    }
  }
  else //from epg menu
  {
    ui_comm_select_set_focus(p_ctrl[1], tmr_node.book_mode);
    ui_comm_numsel_set_focus(
      p_ctrl[2], db_dvbs_get_view_pos_by_id(timer_vid, tmr_node.pgid));

    ctrl_set_attr(p_ctrl[2], OBJ_ATTR_INACTIVE);
    ctrl_set_attr(p_ctrl[3], OBJ_ATTR_INACTIVE);
    ctrl_set_attr(p_ctrl[4], OBJ_ATTR_INACTIVE);
    ctrl_set_attr(p_ctrl[5], OBJ_ATTR_INACTIVE);
    ctrl_set_attr(p_ctrl[6], OBJ_ATTR_INACTIVE);
  }
  //ui_comm_help_create(&timer_help_data, p_cont);

  ctrl_default_proc(p_ctrl[1], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}
//lint +e732 +e830


static RET_CODE close_timer(u32 para1, u32 para2)
{
  u8 i = 0;
  mtos_free(p_ext_buf);
  for(i = 0;i < 6;i++)
  mtos_free(p_unistr[i]);
  return manage_close_menu(ROOT_ID_TIMER, 0, 0);
}


BEGIN_KEYMAP(timer_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_EPG, MSG_EXIT)
END_KEYMAP(timer_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(timer_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_EXIT, on_timer_exit)
ON_COMMAND(MSG_TIME_UPDATE, on_timer_time_update)
END_MSGPROC(timer_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(timer_btn_keymap, NULL)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(timer_btn_keymap, NULL)

BEGIN_MSGPROC(timer_btn_proc, text_class_proc)
ON_COMMAND(MSG_SELECT, on_timer_btn_select)
ON_COMMAND(MSG_FOCUS_LEFT, on_timer_btn_change_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, on_timer_btn_change_focus)
END_MSGPROC(timer_btn_proc, text_class_proc)

BEGIN_MSGPROC(timer_comm_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_timer_comm_select_change_focus)
END_MSGPROC(timer_comm_select_proc, cbox_class_proc)

