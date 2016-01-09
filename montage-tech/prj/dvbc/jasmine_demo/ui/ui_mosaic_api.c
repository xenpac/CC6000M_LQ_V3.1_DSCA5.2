/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_mosaic_api.h"
#include "ui_config.h"

#define MOSACI_RECT_OFFSET_X (-8)
#define MOSACI_RECT_OFFSET_Y (-2)

static play_param_t g_mosaic_pb_info;

static u8 g_mosaic_view = 0;
static u16 g_mosaic_pg_id = 0;

static dvbs_prog_node_t g_mosaic_prog = {0};
static dvbs_prog_node_t g_cur_cell_prog = {0};

static u16 g_cur_cell_prog_id = 0;
static BOOL is_find_cell_pg = FALSE;
static u8 g_logic_cell_id = 0;

static void print_cur_mosaic_info()
{

  UI_PRINTF("mosaic_prog name        =%s\n",g_mosaic_prog.name);
  UI_PRINTF("mosaic_prog s_id        =%d\n",g_mosaic_prog.s_id);
  UI_PRINTF("mosaic_prog video_pid   =%d\n",g_mosaic_prog.video_pid);
  UI_PRINTF("mosaic_prog service_type=%d\n",g_mosaic_prog.service_type);
  UI_PRINTF("mosaic_prog mosaic_flag =%d\n",g_mosaic_prog.mosaic_flag);
  UI_PRINTF("mosaic_prog h_e_cells   =%d\n",g_mosaic_prog.mosaic.h_e_cells);
  UI_PRINTF("mosaic_prog v_e_cells   =%d\n",g_mosaic_prog.mosaic.v_e_cells);
  UI_PRINTF("mosaic_prog lg_cell_cnt =%d\n",g_mosaic_prog.mosaic.lg_cell_cnt);
  UI_PRINTF("mosaic_prog pre_svc_id  =%d\n",g_mosaic_prog.mosaic.pre_svc_id);
  UI_PRINTF("mosaic_prog nxt_svc_id  =%d\n",g_mosaic_prog.mosaic.nxt_svc_id);

  UI_PRINTF("mosaic_prog g_logic_cell_id  =%d\n",g_logic_cell_id);
  UI_PRINTF("mosaic_prog g_cur_cell_prog_id  =%d\n",g_cur_cell_prog_id);
}

static void mosaic_do_play_pg(u16 pg_id)
{
  cmd_t cmd;

  load_play_paramter_by_pgid(&g_mosaic_pb_info, pg_id);

  g_mosaic_pb_info.pg_info.a_pid = g_mosaic_prog.audio[g_logic_cell_id].p_id;
  g_mosaic_pb_info.pg_info.audio_type = g_mosaic_prog.audio[g_logic_cell_id].type;
  g_mosaic_pb_info.pg_info.pcr_pid = (u16)g_mosaic_prog.pcr_pid;
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_mosaic_pb_info;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

static void mosaic_switch_pg(u16 pg_id)
{
  cmd_t cmd;

 // load_play_paramter_by_pgid(&g_mosaic_pb_info, pg_id);

//  g_mosaic_pb_info.pg_info.a_pid = g_mosaic_prog.audio[g_logic_cell_id].p_id;

  cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;

  UI_PRINTF("switch masaic audio pid %d  type %d\n",
                 g_mosaic_prog.audio[g_logic_cell_id].p_id,
                 g_mosaic_prog.audio[g_logic_cell_id].type);
  cmd.data1 = g_mosaic_prog.audio[g_logic_cell_id].p_id;
  cmd.data2 = g_mosaic_prog.audio[g_logic_cell_id].type;


  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

static void mosaic_stop_pg(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_STOP_SYNC;
  cmd.data1 = STOP_PLAY_FREEZE;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}

u8 ui_mosaic_get_logic_cell_id()
{
  return g_logic_cell_id;
}

u8 ui_mosaic_get_mosaic_view()
{
  return g_mosaic_view;
}

BOOL ui_init_mosaic()
{
  BOOL bRet = FALSE;
  u16 mosaic_prog_count = 0;
  u16 i = 0, temp_pg_id = 0;
  dvbs_prog_node_t temp_prog = {0};
  
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  
  memset(&g_mosaic_prog, 0, sizeof(dvbs_prog_node_t));
  g_logic_cell_id = 0;
  g_mosaic_pg_id = 0;

  g_mosaic_view = ui_dbase_create_view(DB_DVBS_MOSAIC, 0, NULL);
  mosaic_prog_count = db_dvbs_get_count(g_mosaic_view);
 
  if(mosaic_prog_count > 0)
  {
    for(i=0; i<mosaic_prog_count; i++)
    {
      temp_pg_id = db_dvbs_get_id_by_view_pos(g_mosaic_view, i);
      db_dvbs_get_pg_by_id(temp_pg_id, &temp_prog);
   
      if(temp_prog.service_type == SVC_TYPE_MOSAIC)
      {
        g_mosaic_pg_id = temp_pg_id;
        memcpy(&g_mosaic_prog, &temp_prog, sizeof(dvbs_prog_node_t));

        if(1 != temp_prog.mosaic.mosaic_entry_point && temp_prog.mosaic.mosaic_entry_point<mosaic_prog_count)
        {
          temp_pg_id = db_dvbs_get_id_by_view_pos(g_mosaic_view, temp_prog.mosaic.mosaic_entry_point);
          db_dvbs_get_pg_by_id(temp_pg_id, &temp_prog);

          if(temp_prog.service_type == SVC_TYPE_MOSAIC)
          {
            g_mosaic_pg_id = temp_pg_id;
            memcpy(&g_mosaic_prog, &temp_prog, sizeof(dvbs_prog_node_t));
          }
        }
      }
    }

    if(i == mosaic_prog_count)
    {
      if(g_mosaic_pg_id == 0)
      {
        return FALSE;
      }
    }

    ui_dbase_set_pg_view_id(g_mosaic_view);
    g_mosaic_prog.mosaic.screen_left = g_mosaic_prog.mosaic.screen_left *1280/720;
    g_mosaic_prog.mosaic.screen_top = g_mosaic_prog.mosaic.screen_top*720/576;
    g_mosaic_prog.mosaic.screen_right = g_mosaic_prog.mosaic.screen_right*1280/720;
    g_mosaic_prog.mosaic.screen_bottom = g_mosaic_prog.mosaic.screen_bottom*720/576;	

    g_mosaic_prog.mosaic.screen_left += MOSACI_RECT_OFFSET_X;
    g_mosaic_prog.mosaic.screen_top += MOSACI_RECT_OFFSET_Y;
    g_mosaic_prog.mosaic.screen_right += MOSACI_RECT_OFFSET_X;
    g_mosaic_prog.mosaic.screen_bottom += MOSACI_RECT_OFFSET_Y;

    avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
    mosaic_stop_pg();
    mosaic_do_play_pg(g_mosaic_pg_id);

    print_cur_mosaic_info();

    bRet = TRUE;
  }

  return bRet;
}

BOOL ui_mosaic_get_cur_mosaic_prog(dvbs_prog_node_t* cur_mosaic_pg)
{
  memcpy(cur_mosaic_pg, &g_mosaic_prog, sizeof(dvbs_prog_node_t));
  return TRUE;
}

BOOL ui_mosaic_get_cur_logic_cell_prog(dvbs_prog_node_t* cur_logic_cell_pg)
{
  memcpy(cur_logic_cell_pg, &g_cur_cell_prog, sizeof(dvbs_prog_node_t));
  return TRUE;
}

BOOL ui_mosaic_get_logic_cell_prog(u8 logic_cell_id)
{
  u16 i = 0;
  u8 all_pg_view = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  u16 all_pg_count = db_dvbs_get_count(all_pg_view);
  
  ui_dbase_set_pg_view_id(all_pg_view);
  is_find_cell_pg = FALSE;
  
  if(g_mosaic_pg_id>0)
  {
    for(i=0; i<all_pg_count; i++)
    {
      g_cur_cell_prog_id = db_dvbs_get_id_by_view_pos(all_pg_view,i);
      db_dvbs_get_pg_by_id(g_cur_cell_prog_id, &g_cur_cell_prog);

      if( (g_cur_cell_prog.orig_net_id == g_mosaic_prog.mosaic.lg_cell[logic_cell_id].lnk.svc.orig_network_id) 
        && (g_cur_cell_prog.ts_id == g_mosaic_prog.mosaic.lg_cell[logic_cell_id].lnk.svc.stream_id)
        && (g_cur_cell_prog.s_id == g_mosaic_prog.mosaic.lg_cell[logic_cell_id].lnk.svc.svc_id)
        )
      {
        is_find_cell_pg = TRUE;
        break;
      }
    }
  }

  g_mosaic_view = ui_dbase_create_view(DB_DVBS_MOSAIC, 0, NULL);
  ui_dbase_set_pg_view_id(g_mosaic_view);

  return is_find_cell_pg;
}

u16 ui_mosaic_get_cur_cell_prog_id(void)
{
  return g_cur_cell_prog_id;
}

BOOL ui_mosaic_change_logic_cell_id(u16 msg)
{
  u8 i = 0;
  u8 next_e_cell_id = 0;
  u8 h_e_cells = g_mosaic_prog.mosaic.h_e_cells;
  u8 v_e_cells = g_mosaic_prog.mosaic.v_e_cells;
  u8 e_cells_count = 0;
  u8 cur_top_left_e_cell_id = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].top_left_e_cell_id;
  u8 cur_bottom_right_e_cell_id = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].bottom_right_e_cell_id;
  u8 cur_logic_cell_lines = 0;

  if(h_e_cells==0||v_e_cells==0||g_mosaic_prog.service_type!=SVC_TYPE_MOSAIC)
  {
    UI_PRINTF("ui_mosaic_change_logic_cell_id ERROR");
    return FALSE;
  }
  
  e_cells_count = h_e_cells*v_e_cells;
  cur_logic_cell_lines = (cur_bottom_right_e_cell_id-cur_top_left_e_cell_id)/h_e_cells+1;
  
  switch(msg)
  {
    case MSG_FOCUS_LEFT:
      g_logic_cell_id = (g_logic_cell_id-1+g_mosaic_prog.mosaic.lg_cell_cnt)%g_mosaic_prog.mosaic.lg_cell_cnt;
      break;
      
    case MSG_FOCUS_RIGHT:
      g_logic_cell_id = (g_logic_cell_id+1)%g_mosaic_prog.mosaic.lg_cell_cnt;
      break;
      
    case MSG_FOCUS_UP:
      next_e_cell_id = (cur_top_left_e_cell_id-h_e_cells+e_cells_count)%e_cells_count;
      for(i=0; i<g_mosaic_prog.mosaic.lg_cell_cnt; i++)
      {
        if(next_e_cell_id%h_e_cells>=g_mosaic_prog.mosaic.lg_cell[i].top_left_e_cell_id%h_e_cells
          && next_e_cell_id%h_e_cells<=g_mosaic_prog.mosaic.lg_cell[i].bottom_right_e_cell_id%h_e_cells
          && next_e_cell_id/h_e_cells==g_mosaic_prog.mosaic.lg_cell[i].bottom_right_e_cell_id/h_e_cells
          )
        {
          g_logic_cell_id = i;
          break;
        }
      }
      break;
      
    case MSG_FOCUS_DOWN:
      next_e_cell_id = (cur_top_left_e_cell_id+h_e_cells*cur_logic_cell_lines)%e_cells_count;
      for(i=0; i<g_mosaic_prog.mosaic.lg_cell_cnt; i++)
      {
        if(next_e_cell_id%h_e_cells>=g_mosaic_prog.mosaic.lg_cell[i].top_left_e_cell_id%h_e_cells
          && next_e_cell_id%h_e_cells<=g_mosaic_prog.mosaic.lg_cell[i].bottom_right_e_cell_id%h_e_cells
          && next_e_cell_id/h_e_cells==g_mosaic_prog.mosaic.lg_cell[i].bottom_right_e_cell_id/h_e_cells
          )
        {
          g_logic_cell_id = i;
          break;
        }
      }
      break;
      break;
      
    default:
      break;
  }

  switch(msg)
  {
    case MSG_FOCUS_LEFT:
    case MSG_FOCUS_RIGHT:
    case MSG_FOCUS_UP:
    case MSG_FOCUS_DOWN:
      mosaic_switch_pg(g_mosaic_pg_id);
      UI_PRINTF("MSG_FOCUS_UPDOWNLEFTRIGHT\n");
      break;

    default:
      break;
  }

  print_cur_mosaic_info();
  
  return TRUE;
}


BOOL change_mosaic_prog(s8 offset)
{
  u16 mosaic_prog_count = db_dvbs_get_count(g_mosaic_view);
  u16 mosaic_view_pos = 0;
  u16 i = 0, temp_pg_id = 0;
  dvbs_prog_node_t temp_prog = {0};

  if(mosaic_prog_count>0)
  {
    mosaic_view_pos = db_dvbs_get_view_pos_by_id(g_mosaic_view, g_mosaic_pg_id);

    mosaic_view_pos = (mosaic_view_pos+offset+mosaic_prog_count)%mosaic_prog_count;
   
    temp_pg_id = db_dvbs_get_id_by_view_pos(g_mosaic_view, mosaic_view_pos);
    //ui_dbase_set_pg_view_id(g_mosaic_view);
    db_dvbs_get_pg_by_id(temp_pg_id, &temp_prog);

    if(temp_prog.service_type == SVC_TYPE_MOSAIC)
    {
      g_mosaic_pg_id = temp_pg_id;
      memcpy(&g_mosaic_prog, &temp_prog, sizeof(dvbs_prog_node_t));
    }
    else
    {
      for(i=0; i<mosaic_prog_count; i++)
      {
        temp_pg_id = db_dvbs_get_id_by_view_pos(g_mosaic_view, i);
        db_dvbs_get_pg_by_id(temp_pg_id, &temp_prog);

        if(temp_prog.service_type == SVC_TYPE_MOSAIC)
        {
          g_mosaic_pg_id = temp_pg_id;
          memcpy(&g_mosaic_prog, &temp_prog, sizeof(dvbs_prog_node_t));

          if(1 != temp_prog.mosaic.mosaic_entry_point && temp_prog.mosaic.mosaic_entry_point<mosaic_prog_count)
          {
            temp_pg_id = db_dvbs_get_id_by_view_pos(g_mosaic_view, temp_prog.mosaic.mosaic_entry_point);
            db_dvbs_get_pg_by_id(temp_pg_id, &temp_prog);

            if(temp_prog.service_type == SVC_TYPE_MOSAIC)
            {
              g_mosaic_pg_id = temp_pg_id;
              memcpy(&g_mosaic_prog, &temp_prog, sizeof(dvbs_prog_node_t));
            }
          }
        }
      }

      if(i == mosaic_prog_count)
      {
        if(g_mosaic_pg_id == 0)
        {
          return FALSE;
        }
      }
    }

    g_logic_cell_id = 0;
    mosaic_stop_pg();
    
    mosaic_do_play_pg(g_mosaic_pg_id);

    print_cur_mosaic_info();
  }

  return TRUE;
}

BOOL ui_mosaic_calc_cell_rect(rect_t *rc)
{
  u16 elementary_cell_w = 0;
  u16 elementary_cell_h = 0;
  u8 x_index,y_index,w_index,h_index;
  
  if(g_mosaic_prog.mosaic.h_e_cells==0||g_mosaic_prog.mosaic.v_e_cells==0)
  {
    MT_ASSERT(0);
  }
  elementary_cell_w = (g_mosaic_prog.mosaic.screen_right - g_mosaic_prog.mosaic.screen_left)/g_mosaic_prog.mosaic.h_e_cells;
  elementary_cell_h = (g_mosaic_prog.mosaic.screen_bottom - g_mosaic_prog.mosaic.screen_top)/g_mosaic_prog.mosaic.v_e_cells;

  x_index = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].top_left_e_cell_id%g_mosaic_prog.mosaic.h_e_cells;
  y_index = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].top_left_e_cell_id/g_mosaic_prog.mosaic.h_e_cells;
  w_index = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].bottom_right_e_cell_id%g_mosaic_prog.mosaic.h_e_cells + 1;
  h_index = g_mosaic_prog.mosaic.lg_cell[g_logic_cell_id].bottom_right_e_cell_id/g_mosaic_prog.mosaic.h_e_cells + 1;
  
  rc->left = x_index*elementary_cell_w;
  rc->right = w_index*elementary_cell_w;
  rc->top = y_index*elementary_cell_h;
  rc->bottom = h_index*elementary_cell_h;
  
  return TRUE;
}

BOOL ui_mosaic_calc_cell_rect_by_logic_cell_id(rect_t *rc, u8 lg_cell_id)
{
  u16 elementary_cell_w = 0;
  u16 elementary_cell_h = 0;
  u8 x_index,y_index,w_index,h_index;
  
  if(g_mosaic_prog.mosaic.h_e_cells==0||g_mosaic_prog.mosaic.v_e_cells==0)
  {
    MT_ASSERT(0);
  }
  elementary_cell_w = (g_mosaic_prog.mosaic.screen_right - g_mosaic_prog.mosaic.screen_left)/g_mosaic_prog.mosaic.h_e_cells;
  elementary_cell_h = (g_mosaic_prog.mosaic.screen_bottom - g_mosaic_prog.mosaic.screen_top)/g_mosaic_prog.mosaic.v_e_cells;

  x_index = g_mosaic_prog.mosaic.lg_cell[lg_cell_id].top_left_e_cell_id%g_mosaic_prog.mosaic.h_e_cells;
  y_index = g_mosaic_prog.mosaic.lg_cell[lg_cell_id].top_left_e_cell_id/g_mosaic_prog.mosaic.h_e_cells;
  w_index = g_mosaic_prog.mosaic.lg_cell[lg_cell_id].bottom_right_e_cell_id%g_mosaic_prog.mosaic.h_e_cells + 1;
  h_index = g_mosaic_prog.mosaic.lg_cell[lg_cell_id].bottom_right_e_cell_id/g_mosaic_prog.mosaic.h_e_cells + 1;
  
  rc->left = x_index*elementary_cell_w;
  rc->right = w_index*elementary_cell_w;
  rc->top = y_index*elementary_cell_h;
  rc->bottom = h_index*elementary_cell_h;
  
  return TRUE;
}


BOOL ui_mosaic_exit_cell_prog(void)
{
  g_mosaic_view = ui_dbase_create_view(DB_DVBS_MOSAIC, 0, NULL);
  ui_dbase_set_pg_view_id(g_mosaic_view);
  
  mosaic_stop_pg();
  mosaic_do_play_pg(g_mosaic_pg_id);
  return TRUE;
}

BOOL ui_mosaic_exit(void)
{
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;
  //ui_resume_play();
  mosaic_stop_pg();
  ui_restore_view();
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if (curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if (curn_mode == CURN_MODE_NONE)
    {
      //ui_stop_play(STOP_PLAY_BLACK, TRUE);
      ui_set_front_panel_by_str("----");
      return FALSE;
    }
  }
//  ui_play_prog(ui_sys_get_curn_prog(view), FALSE);
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id,
                                    &context);

  ui_play_prog(prog_id, TRUE);
  //ui_dvbc_change_view(DB_DVBS_ALL_TV, TRUE);
  //ui_india_change_group(DB_DVBS_TV_RADIO, 0/*All group*/, TRUE);

  return TRUE;
}

