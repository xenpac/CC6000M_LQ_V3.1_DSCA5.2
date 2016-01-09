/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
// lib
#include "ui_common.h"
#include "bat.h"
#include "nit.h"
#include "user_parse_table.h"
//#include "db_dvbs.h"

static bouquet_group_t g_bouquet_grp = {0};
static BOOL is_categories_loaded = FALSE;
static u8 flash_categories_count = 0;
static logic_info_t logic_info = {0};
static u16 un_india_ts_logic_num = 1001;

void categories_mem_reset(void)
{
  memset(&g_bouquet_grp, 0, sizeof(g_bouquet_grp));
  is_categories_loaded = FALSE;
  flash_categories_count = 0;
  un_india_ts_logic_num = 1001;
  memset(&logic_info, 0, sizeof(logic_info));
}

void categories_set_loaded_flag(BOOL b)
{
  is_categories_loaded = b;
  UI_PRINTF("categories_set_loaded_flag[%d] \n",b);
}

BOOL categories_get_loaded_flag(void)
{
  return is_categories_loaded;
}

BOOL add_logic_info(u16 sID, u16 logic_num)
{
  u16 i=0;
  
  if(logic_info.log_ch_num>0)
  {
    for(i=0; i<logic_info.log_ch_num; i++)
    {
      if(logic_info.log_ch_info[i].service_id == sID)
      {
        return FALSE;
      }
    }
  }

  logic_info.log_ch_info[logic_info.log_ch_num].service_id = sID;
  logic_info.log_ch_info[logic_info.log_ch_num].logical_channel_id = logic_num;

  logic_info.log_ch_num++;

  return TRUE;
}

void load_categories_info(void)
{
  u8 i;
  u16 fav_name_unistr[MAX_BOUQUET_NAMELEN] = {0};

  UI_PRINTF("load_categories_info[is_categories_loaded=%d] \n",is_categories_loaded);
  if(!is_categories_loaded)
  {
    flash_categories_count = sys_status_get_categories_count();
    un_india_ts_logic_num = sys_status_get_default_logic_num();
    
    UI_PRINTF("load_categories_info[un_india_ts_logic_num=%d] \n",un_india_ts_logic_num);
    if(flash_categories_count>FAV_LIST_COUNT)
    {
      for(i=0; i<flash_categories_count-FAV_LIST_COUNT; i++)
      {
        sys_status_get_fav_name(i, fav_name_unistr);
        memcpy(g_bouquet_grp.bouquet_name[i], fav_name_unistr, sizeof(fav_name_unistr));
        g_bouquet_grp.bouquet_id[i] = sys_status_get_bouquet_id(i);
      }
      g_bouquet_grp.bouquet_count = flash_categories_count-FAV_LIST_COUNT;
    }

    is_categories_loaded = TRUE;
  }
}

void sort_by_bouquet_id(void)
{
  u16 i,j,k;
  u16 fav_name_unistr[MAX_BOUQUET_NAMELEN] = {0};
  u16 temp;
  
  if(g_bouquet_grp.bouquet_count<=0)
  {
    return;
  }
  
  for(i=g_bouquet_grp.bouquet_count-1; i>0; i--)
  {
    for(j=0; j<i; j++)
    {
      if(g_bouquet_grp.bouquet_id[i]<g_bouquet_grp.bouquet_id[j])
      {
        //exchange bouquet id
        temp = g_bouquet_grp.bouquet_id[i];
        g_bouquet_grp.bouquet_id[i] = g_bouquet_grp.bouquet_id[j];
        g_bouquet_grp.bouquet_id[j] = temp;

        //exchange boq_SID_list_count
        temp = g_bouquet_grp.boq_SID_list_count[i];
        g_bouquet_grp.boq_SID_list_count[i] = g_bouquet_grp.boq_SID_list_count[j];
        g_bouquet_grp.boq_SID_list_count[j] = temp;

        //exchange boq_SID_list
        for(k=0; k<MAX_LOGIC_CHANNEL; k++)
        {
          temp = g_bouquet_grp.boq_SID_list[i][k];
          g_bouquet_grp.boq_SID_list[i][k] = g_bouquet_grp.boq_SID_list[j][k];
          g_bouquet_grp.boq_SID_list[j][k] = temp;
        }

        //exchange bouquet name
        memcpy(fav_name_unistr, g_bouquet_grp.bouquet_name[i], sizeof(fav_name_unistr));
        memcpy(g_bouquet_grp.bouquet_name[i], g_bouquet_grp.bouquet_name[j], sizeof(fav_name_unistr));
        memcpy(g_bouquet_grp.bouquet_name[j], fav_name_unistr, sizeof(fav_name_unistr));
      }
    }
  }
}

void parse_bat_india(handle_t handle, dvb_section_t *p_sec)
{
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_svc = (service_t *)handle;
  os_msg_t msg = {0};
  u8 *p_buf = p_sec->p_buffer;
  u16 bouquet_id = 0;
  u16 offset = 0;
  s16 bouquet_len = 0;
  u8 bouquet_name[64] = {0};
  u16 bouquet_name_unistr[64] = {0};
  BOOL hasBouquetName = FALSE;
  BOOL BouquetID_repeat = FALSE;
  u16 repeat_bouq_index = 0;
  s16 section_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  u16 dl_loop_len = 0;
  u16 i = 0, j = 0;
  bat_t bat = {0};
  u8 tmp_len = 0;
  u16 cur_bouquet_service_list_id[256];
  linkage_update_desc_t update_info = {0};
  u16 cur_bouquet_service_count = 0;

  load_categories_info();
  
  MT_ASSERT(p_dvb_handle != NULL);

  if(p_buf[0] != p_sec->table_id)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }

  if ((p_buf[5] & 0x01) == 0)
  {
    p_dvb_handle->filter_data_error(p_sec->p_svc, p_sec);
    return;
  }

  bouquet_id = MAKE_WORD2(p_buf[3], p_buf[4]);
  
  /* Section length */
  section_len = MAKE_WORD2((p_buf[1] & 0x0f), p_buf[2]);
  section_len += 3;

  memset(&bat, 0, sizeof(bat_t));
  
  bat.bouquet_id = bouquet_id;
  bat.version_num = (p_buf[5] & 0x3E) >> 1;
  bat.sec_number = p_buf[6];
  bat.last_sec_number = p_buf[7];

  bouquet_len = MAKE_WORD2(p_buf[8] & 0x0F, p_buf[9]);
  offset = 10;
  
  UI_PRINTF("[sunfengfeng] ...sec %d, last sec %d, bouquet id 0x%x\n",bat.sec_number, bat.last_sec_number, bouquet_id);
  UI_PRINTF("[parsebat]==========start===========\n");
  
  UI_PRINTF("[parsebat]bat bouquet_id: %d 0x%x\n", bouquet_id,bouquet_id);
  while(bouquet_len > 0)
  {
    tmp_len = p_buf[offset + 1];
    switch(p_buf[offset])
    {
      case DVB_DESC_LINKAGE:
        {
          bat.ts_id = update_info.ts_id = MAKE_WORD2(p_buf[offset + 2], 
                                                                p_buf[offset + 3]);
          bat.network_id = update_info.network_id = MAKE_WORD2(p_buf[offset + 4], 
                                                                                p_buf[offset + 5]);
          bat.service_id = update_info.service_id = MAKE_WORD2(p_buf[offset + 6], 
                                                                                p_buf[offset + 7]);
          if(p_buf[offset + 8] == 0xA0)  // linkage type 0xA0
          {
            update_info.operator_num = p_buf[offset + 9];
            dl_loop_len = p_buf[offset + 10];
            for(i = 0; i < dl_loop_len;)
            {
              update_info.manufacture_id = p_buf[offset + 11 + i];
              update_info.hardware_id = MAKE_WORD2(p_buf[offset + 12 + i], p_buf[offset + 13 + i]);
              update_info.model_id = p_buf[offset + 14 + i];
              update_info.software_ver = MAKE_WORD2(p_buf[offset + 15], p_buf[offset + 16 + i]);

              update_info.force_flag = p_buf[offset + 18 + i] & 0x03;
              update_info.start_STB_id = make32(&p_buf[offset + 19 + i]);
              update_info.end_STB_id = make32(&p_buf[offset + 23 + i]);
              i += 16;
            }
          }
          else if(p_buf[offset + 8] == 0x80)
          {
            bat.fdt_table_id = p_buf[offset + 11];
            if(DVB_TABLE_ID_FDT == bat.fdt_table_id)
            {
              bat.fdt_pid = MAKE_WORD2(p_buf[offset + 9]&0x1F, p_buf[offset + 10 ]);
              bat.fdt_ver_num = p_buf[offset + 12]&0x1F;
            }
          }
        }
        break;
      
      case DVB_DESC_BOUQUET_NAME://bouquet_name_descriptor
        {
          UI_PRINTF("[parsebat]bat descriptor name: %s \n", &p_buf[offset + 2]);
          memcpy(bouquet_name, &p_buf[offset + 2], p_buf[offset + 1]);
          hasBouquetName = TRUE;
        }
        break;

      default:
        break;
    }
    
    offset += (tmp_len + 2);  
    bouquet_len -= (tmp_len + 2);  
  }

  // TODO ts loop len while
  ts_loop_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
  offset += 2;
  while(ts_loop_len > 0)
  {
    offset += 4;
    desc_len = MAKE_WORD2(p_buf[offset] & 0xF, p_buf[offset + 1]);
    offset += 2;

    ts_loop_len -= (desc_len + 6);  
    while(desc_len > 0)
    {
      tmp_len = p_buf[offset + 1];

      switch(p_buf[offset])
      {
        case DVB_DESC_SERVICE_LIST:
          {
            u16 service_id = 0;
            u8 service_type = 0;
            u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
            u8 des_len = tmp_len;
            
            while(des_len)
            {
              service_id = MAKE_WORD2(p_log_des[0], p_log_des[1]);
              service_type = p_log_des[2];

              UI_PRINTF("[parsebat]service_id: %d 0x%x\n", service_id,service_id);

              cur_bouquet_service_list_id[cur_bouquet_service_count] = service_id;
              cur_bouquet_service_count++;
              
              p_log_des += 3;
              if(des_len < 3) //for check error
              {
                des_len = 0;
              }
              else
              {
                des_len -= 3;
              }
            }
          }
          break;
          
        case 0x92:
          {
          }
          break;

        case 0x83:
          {
            logical_channel_t *p_log_ch = bat.log_ch_info + bat.log_ch_num;
            u8 *p_log_des = p_buf + offset + 2;  //skip tag and len
            u8 des_len = tmp_len;
            
            while(des_len)
            {
              if(bat.log_ch_num >= DVB_MAX_SDT_SVC_NUM) //check input
              {
                break;
              }
              p_log_ch->service_id = MAKE_WORD2(p_log_des[0], p_log_des[1]);
              p_log_ch->logical_channel_id = (((p_log_des[2]&0x03)<<8) | p_log_des[3]);
              
              add_logic_info(p_log_ch->service_id, p_log_ch->logical_channel_id);

              UI_PRINTF("[parsebat]s_id = %d, logic =%d\n", p_log_ch->service_id, p_log_ch->logical_channel_id);

              p_log_ch++;
              bat.log_ch_num++;
              p_log_des += 4;
              if(des_len < 4) //for check error
              {
                des_len = 0;
              }
              else
              {
                des_len -= 4;
              }
              
            }
          }
          break;
      }
      offset += (tmp_len + 2);
      desc_len -= (tmp_len + 2);
    }
  }

  if(g_bouquet_grp.bouquet_count != 0)
  {
    for(i=0; i<g_bouquet_grp.bouquet_count; i++)
    {
      if(bouquet_id == g_bouquet_grp.bouquet_id[i])
      {
        BouquetID_repeat = TRUE;
        repeat_bouq_index = i;
        break;
      }
    }
  }
  
  if(BouquetID_repeat)
  {
    if(hasBouquetName)
    {
      str_asc2uni(bouquet_name, bouquet_name_unistr);
      memcpy(g_bouquet_grp.bouquet_name[repeat_bouq_index], bouquet_name_unistr, MAX_BOUQUET_NAMELEN);
    }

    for(i=0; i<cur_bouquet_service_count; i++)
    {
      if(g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]!=0)
      {
        for(j=0; j<g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]; j++)
        {
          if(cur_bouquet_service_list_id[i]==g_bouquet_grp.boq_SID_list[repeat_bouq_index][j])
          {
            break;
          }
        }

        if(j==g_bouquet_grp.boq_SID_list_count[repeat_bouq_index])
        {
          g_bouquet_grp.boq_SID_list[repeat_bouq_index][g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]] = cur_bouquet_service_list_id[i];
          g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]++;
        }
      }
      else
      {
        g_bouquet_grp.boq_SID_list[repeat_bouq_index][0] = cur_bouquet_service_list_id[i];
        g_bouquet_grp.boq_SID_list_count[repeat_bouq_index]++;
      }
    }
  }
  else if((MAX_BOUQUET_GROUP - 5) <= g_bouquet_grp.bouquet_count)
  {
    UI_PRINTF("[error]==========MAX_BOUQUET_GROUP - 5 <= g_bouquet_grp.bouquet_coun ===========\n");
  }
  else
  {
    if(hasBouquetName)
    {
      str_asc2uni(bouquet_name, bouquet_name_unistr);
    }
    else
    {
      str_asc2uni((u8*)"unknow group", bouquet_name_unistr);
    }
    memcpy(g_bouquet_grp.bouquet_name[g_bouquet_grp.bouquet_count], bouquet_name_unistr, MAX_BOUQUET_NAMELEN);
    g_bouquet_grp.bouquet_id[g_bouquet_grp.bouquet_count] = bouquet_id;

    for(i=0; i<cur_bouquet_service_count; i++)
    {
      if(g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]!=0)
      {
        for(j=0; j<g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]; j++)
        {
          if(cur_bouquet_service_list_id[i]==g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][j])
          {
            break;
          }
        }

        if(j==g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count])
        {
          g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]] = cur_bouquet_service_list_id[i];
          g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]++;
        }
      }
      else
      {
        g_bouquet_grp.boq_SID_list[g_bouquet_grp.bouquet_count][0] = cur_bouquet_service_list_id[i];
        g_bouquet_grp.boq_SID_list_count[g_bouquet_grp.bouquet_count]++;
      }
    }
    g_bouquet_grp.bouquet_count++;
  }

  UI_PRINTF("[parsebat]==========end===========\n");

  msg.content = DVB_BAT_FOUND;
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.para1 = (u32)(&bat);
  msg.para2 = sizeof(bat_t);
  p_svc->notify(p_svc, &msg);  
    
  return;
}

void create_categories(void)
{
  u8 view_id;
  u16 pg_count;
  u16 i,j;
#ifdef BOUQUET_SUPPORT
  u16 pg_id, k;
  BOOL edit_flag = FALSE;
  dvbs_prog_node_t prog = {0};
#endif
  u8 fav_name[MAX_BOUQUET_NAMELEN] = {0};
  u16 fav_name_unistr[MAX_BOUQUET_NAMELEN] = {0};
  u16 content[MAX_BOUQUET_NAMELEN];
  u8 bouquet_name[64] = {0};
  u16 bouquet_name_unistr[64] = {0};

  //customer_cfg_t customer_cfg = {0};
  
  //get_customer_config(&customer_cfg);

  UI_PRINTF("[create_categories]==========IN===========\n");
  
#ifndef BOUQUET_SUPPORT
  memset(&g_bouquet_grp, 0, sizeof(g_bouquet_grp));
#endif

  load_categories_info();
  sort_by_bouquet_id();

  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG, 0, NULL);
  pg_count = db_dvbs_get_count(view_id);
  for(i=0; i<g_bouquet_grp.bouquet_count; i++)
  {
    memset(bouquet_name, 0 ,sizeof(bouquet_name)/sizeof(u8));
    memset(bouquet_name_unistr, 0 ,sizeof(bouquet_name_unistr)/sizeof(u16));
    str_uni2asc(bouquet_name, g_bouquet_grp.bouquet_name[i]);
    gb2312_to_unicode((u8*)bouquet_name, MAX_BOUQUET_NAMELEN, bouquet_name_unistr, MAX_BOUQUET_NAMELEN+1);
    sys_status_set_fav_name((u8)i, bouquet_name_unistr);
    sys_status_set_bouquet_id(i, g_bouquet_grp.bouquet_id[i]);
  }
  sys_status_set_categories_count(g_bouquet_grp.bouquet_count+5);

  for(i=0; i<g_bouquet_grp.bouquet_count; i++)
  {
    UI_PRINTF("[create_categories] bouquet id = %d\n",g_bouquet_grp.bouquet_id[i]);
    UI_PRINTF("[create_categories] bouquet name = %s\n",g_bouquet_grp.bouquet_name[i]);
    for(j=0; j<g_bouquet_grp.boq_SID_list_count[i]; j++)
    {
      //UI_PRINTF("[create_categories] sID = %d\n",g_bouquet_grp.boq_SID_list[i][j]);
    }
  }
  
  for(i=0; i<5; i++)
  {
    memset(content, 0, sizeof(content));
    gui_get_string(IDS_FAV_LIST, content, MAX_BOUQUET_NAMELEN);
    sprintf((char *)fav_name, "%d", i+1);
    str_asc2uni(fav_name, fav_name_unistr);
    uni_strcat(content, fav_name_unistr, 256);
    sys_status_set_fav_name(g_bouquet_grp.bouquet_count+i, content);
  }
  #if 0
    sprintf((char *)fav_name, "All channel");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(5, fav_name_unistr);
    sprintf((char *)fav_name, "PPV");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(6, fav_name_unistr);
    sprintf((char *)fav_name, "MOSAIC ENT");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(7, fav_name_unistr);
    sprintf((char *)fav_name, "Entertainment");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(8, fav_name_unistr);
    sprintf((char *)fav_name, "Movies");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(9, fav_name_unistr);
    sprintf((char *)fav_name, "Kids");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(10, fav_name_unistr);
    sprintf((char *)fav_name, "Music");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(11, fav_name_unistr);
    sprintf((char *)fav_name, "News");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(12, fav_name_unistr);
    sprintf((char *)fav_name, "Sports");
    str_asc2uni(fav_name, fav_name_unistr);
    sys_status_set_fav_name(13, fav_name_unistr);
  #endif
#ifdef BOUQUET_SUPPORT
  UI_PRINTF("[create_categories]pg_count=%d,bouquet_count=%d\n",pg_count,g_bouquet_grp.bouquet_count);
  
  for(i=0; i<pg_count; i++)
  {
    edit_flag = FALSE;
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);
    
    for(j=0; j<g_bouquet_grp.bouquet_count; j++)
    {
      for(k=0; k<g_bouquet_grp.boq_SID_list_count[j]; k++)
      {
        if(prog.s_id == g_bouquet_grp.boq_SID_list[j][k])
        {
          edit_flag = TRUE;
          prog.bouquet_id = g_bouquet_grp.bouquet_id[j];
          prog.fav_grp_flag |= (1<<j);

#if 0
          UI_PRINTF("[create_categories]add group===pgname=%s; groupname=%s; groupIndex=%d; pg_sid=%d; pg_boquID=%d====\n",
                      prog.name,
                      g_bouquet_grp.bouquet_name[j],
                      j,
                      prog.s_id,
                      prog.bouquet_id);
#endif
          
          break;
        }
      }
    }

    if(edit_flag)
    {
      db_dvbs_edit_program(&prog);
    }
  }

  for(i=0; i<pg_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);

    if(prog.fav_grp_flag == 0 && prog.logical_num == 0)
    {
      prog.logical_num = un_india_ts_logic_num;
      db_dvbs_edit_program(&prog);
      un_india_ts_logic_num++;
    }
    UI_PRINTF("[-111---%d----]==========%d===========\n",i+1,prog.logical_num);
  }

  sys_status_set_default_logic_num(un_india_ts_logic_num);

  if(pg_count>0)
  {
    db_dvbs_pg_sort_init(view_id);
    db_dvbs_pg_sort(view_id,DB_DVBS_LOCAL_NUM);
    db_dvbs_pg_sort_deinit();
    db_dvbs_save(view_id);
  }

  for(i=0; i<pg_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &prog);
    UI_PRINTF("[----%d----]==========%d===========\n",i+1,prog.logical_num);
  }
#endif
  sys_status_save();
  UI_PRINTF("[create_categories]==========OUT===========\n");
}

void parse_nit_india(handle_t handle, dvb_section_t *p_sec)
{
  service_t *p_svc = handle;
  nit_t _nit = {0};
  nit_t *p_nit = &_nit;
  u8 *p_buf = p_sec->p_buffer;
  s16 network_desc_len = 0;
  s16 ts_loop_len = 0;
  s16 desc_len = 0;
  //u16 net_name_des_len = 0;
  os_msg_t msg = {0};
  u16 ts_id = 0;
  u16 offset = 0;
  u8  tp_cnt = 0 ;
  u8  svc_cnt = 0;
  s16 svc_list_des_length = 0;

  u16 nxt_ts_start = 0;

  u8  link_desc_len = 0;
  u8  linkage_type = 0;
  u8  OUI_data_length = 0;
  u8  OUI_data_offset = 0;
  u8  selector_length = 0;
  u8  selector_offset = 0;
  u32 OUI = 0;
  update_t up_info = {0};
  u8 *p_buf_temp = NULL;
  u32 temp = 0;

  if(p_buf[0] != DVB_TABLE_ID_NIT_ACTUAL &&
    p_buf[0] != DVB_TABLE_ID_NIT_OTHER)
  {
    UI_PRINTF("MDL: not nit\n");
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_NIT_ACTUAL;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    return;
  }

  if((p_buf[5] & 0x01)== 0)
  {
    UI_PRINTF("MDL: this nit is not usable\n");
    msg.content = DVB_TABLE_TIMED_OUT;
    msg.is_ext = FALSE;
    msg.para1 = DVB_TABLE_ID_NIT_ACTUAL;
    msg.context = p_sec->r_context;
    p_svc->notify(p_svc, &msg);
    return;
  }
  p_nit->p_origion_data = p_sec->p_buffer;
  p_nit->network_id = MAKE_WORD(p_buf[4], p_buf[3]);
  p_nit->version_num =(p_buf[5] & 0x3E) >> 1;
  p_nit->sec_number = p_buf[6];

  p_nit->last_sec_number = p_buf[7];
  
  network_desc_len = MAKE_WORD(p_buf[9], (p_buf[8] & 0x0F));
  offset = 10;
  //UI_PRINTF("network_desc_len[%d]\n", network_desc_len);
  
  while(network_desc_len > 0)
  {
    //net_name_des_len = p_buf[offset + 1];
    if(p_buf[offset] == DVB_DESC_NETWORK_NAME)
    {
      //Skip tag
      offset ++;
      network_desc_len --;
      
      //Fetch desc length
      desc_len = p_buf[offset];

      //Skip desc length
      offset ++;
      network_desc_len --;
  
      //get network name & skip it.
      if(desc_len != 0)
      {
        memcpy(p_nit->network_name, p_buf + offset, desc_len);
      }
      else
      {
        memset(p_nit->network_name, 0, MAX_NETWORK_NAME_LEN);
      }

      //UI_PRINTF("nit network name: %s, length %d\n", &p_nit->network_name, desc_len);

      offset += desc_len;
      network_desc_len -= desc_len;
    }
    else if(p_buf[offset] == DVB_DESC_SERVICE_UPDATE)
    {
      //Skip tag
      offset += 1;
      network_desc_len -=1;

      //skip length
      offset += 1;
      network_desc_len -=1;
      
      //
      p_nit->svc_update.force_flag = (p_buf[offset]&0x80)>>7;
      p_nit->svc_update.ver_num  = p_buf[offset]&0x7f;     

      //Skip flag
      offset += 1;
      network_desc_len -= 1;

      // length is fixed as 1
      //UI_PRINTF("service update len: %d\n", p_buf[offset + 2]);
    }
    else if(p_buf[offset] == DVB_DESC_LOGIC_CHANNEL)
    {
      desc_len = p_buf[offset + 1];
      network_desc_len -= 2 + desc_len;
      offset += 2 + desc_len;
      // parse logical channel list here
    }
    else if(p_buf[offset] == DVB_DESC_LINKAGE)
    {
      desc_len = p_buf[offset + 1];
      network_desc_len -= 2 + desc_len;
      offset += 2 + desc_len;
    }
    else
    {
     offset ++;
     network_desc_len --;
    }
  }

  ts_loop_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
  //UI_PRINTF("ts_loop_len = %d\n", ts_loop_len);
  
  offset += 2;

  while(ts_loop_len > 0)
  {
    //UI_PRINTF("ts_loop_len[%d]\n", ts_loop_len);

    /*! ts id*/
    ts_id = MAKE_WORD(p_buf[offset + 1], p_buf[offset]);
    p_nit->tp_svc_list[tp_cnt].ts_id = ts_id;

    offset += 4;

    desc_len = MAKE_WORD(p_buf[offset + 1], (p_buf[offset] & 0x0F));
    /*! Skip offset length*/
    offset += 2;
    
    ts_loop_len -= (desc_len + 6);
    /*! Saving next start position for next ts id*/
    nxt_ts_start = offset + desc_len; 
    svc_cnt   = 0 ;
    while(desc_len > 0)
    {
      //UI_PRINTF("desc_len[%d]\n", desc_len);
      switch(p_buf[offset])
      {           
        case DVB_DESC_SERVICE_LIST:
        /*!Jump to descriptor length*/
        offset += 1; 
        desc_len --;
        
        svc_list_des_length = p_buf[offset];
        desc_len -= svc_list_des_length;

        /*!Jump to service id */
        offset += 1;
        desc_len --;
        while(svc_list_des_length > 0)
        {
          //UI_PRINTF("svc_list_des_length[%d]\n", svc_list_des_length);
          //p_nit->tp_svc_list[tp_cnt].svc_id[svc_cnt] 
                  //= MAKE_WORD(p_buf[offset+1], p_buf[offset]);
          /*!Skip service id and service type */
          offset += 3;
          svc_cnt ++;
          svc_list_des_length -=3;
          if(svc_cnt > MAX_PROG_NUM)
          {
            break;
          }
        }
        p_nit->tp_svc_list[tp_cnt].total_svc_num = svc_cnt;
          break;
        case DVB_DESC_SATELLITE_DELIVERY:
        {
          sat_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbs_tp_info;

          p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBS_TP; 
          /*! Skip tag*/
          offset += 1;
          desc_len -=1;
          
          /*! Fetch desc length*/
          desc_len -=  p_buf[offset];

          /*! Skip length*/
          offset += 1;
          desc_len -=1;

          p_tp_info->frequency  = make32(&p_buf[offset]);

          offset += 4;
          p_tp_info->orbital_position 
                                    = MAKE_WORD(p_buf[offset+1], p_buf[offset]);

          offset += 2;
          p_tp_info->west_east_flag = (p_buf[offset]&0x80)>>7;
          p_tp_info->polarization = (p_buf[offset] & 0x60)>> 5;
          //circule left 
          if(p_tp_info->polarization >= 2)
          {
            p_tp_info->polarization -= 2;
          }
          
          p_tp_info->modulation_system = (p_buf[offset] & 0x04);
          p_tp_info->modulation_type = (p_buf[offset] & 0x03);
          //p_tp_info->modulation = (p_buf[offset] & 0x1F);

          offset += 1;
          p_tp_info->symbol_rate = make32(&p_buf[offset])>>4;  
          offset += 4;
        }
          break;
        case DVB_DESC_CABLE_DELIVERY:
          {
            cable_tp_info_t *p_tp_info = &p_nit->tp_svc_list[tp_cnt].dvbc_tp_info;
            
            p_nit->tp_svc_list[tp_cnt].tp_type = NIT_DVBC_TP;
            offset += 1;
            desc_len -=1;
            desc_len -=  p_buf[offset];
            offset += 1;
            desc_len -=1;
            p_tp_info->frequency = make32(&p_buf[offset]);
            offset += 4;
            offset += 1;
            p_tp_info->fec_outer = p_buf[offset] & 0xf;
            offset += 1;
            p_tp_info->modulation = p_buf[offset];
            offset += 1;
            p_tp_info->symbol_rate = MT_MAKE_DWORD(
                      MAKE_WORD((p_buf[offset + 3] & 0xf0), p_buf[offset + 2]),
                      MAKE_WORD(p_buf[offset + 1], p_buf[offset]));
            p_tp_info->symbol_rate >>= 4;
            
            offset += 3;
            p_tp_info->fec_inner = p_buf[offset] & 0xf;
            offset += 1;
          }
          break;
          case DVB_DESC_STUFFING:
          case DVB_DESC_TERRESTRIAL_DELIVERY:
          case DVB_DESC_PRIVATE_DATA:
          case DVB_DESC_FREQUENCY_LIST:
          case DVB_DESC_MULTILIGUAL_NETWORK_NAME:
          case DVB_DESC_CELL_LIST:
          case DVB_DESC_CELL_FREQUE:
          case DVB_DESC_DEFAULT_AUTHORITY:
          case DVB_DESC_TIME_SLICE:
            //Skip tag
            offset ++;
            desc_len -- ;
            //if(desc_len > tmp_desc_length)
            {
              desc_len -= (s16)p_buf[offset];
              offset   += p_buf[offset];   
            }

            offset ++;
            desc_len -- ;
            break;
              
          case DVB_DESC_LINKAGE:
            //Skip tag
            offset ++;
            desc_len -- ;

            link_desc_len = (u8)p_buf[offset];
            if(link_desc_len>=7)
            {
              linkage_type = p_buf[offset+7];
              switch(linkage_type)
              {
                case 0x09:
                  p_buf_temp = &p_buf[offset+8];
                  OUI_data_length = p_buf_temp[0];
                  p_buf_temp++;
                  while(OUI_data_length>0)
                  {                                        
                    OUI = (p_buf_temp[0]<<16 | p_buf_temp[1]<<8 | p_buf_temp[2]);

                    OUI_data_length -= 3;
                    OUI_data_offset += 3;

                    p_buf_temp += 3;

                    selector_length = p_buf_temp[0];
                    OUI_data_length--;
                    OUI_data_offset++;

                    p_buf_temp++;

                    OUI_data_length -= selector_length;
                    OUI_data_offset += selector_length;

                    while(selector_length>0)
                    {
                      //Cable_delivery_desc
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;
                      
                      //DSysDes_Length
                      temp = p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Freq_desc
                      up_info.freq= make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Fec_outer
                      selector_offset++;
                      p_buf_temp++;

                      //QAM_Mode
                      up_info.qam_mode = (u8)p_buf_temp[0];
                      selector_offset++;
                      p_buf_temp++;

                      //Symbolrate_desc
                      up_info.symbol= make16(&p_buf_temp[0]);
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Fec_inner
                      selector_offset++;
                      p_buf_temp++;

                      //Download_Pid
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Update_type
                      selector_offset++;
                      p_buf_temp++;

                      //Private_data_length
                      selector_offset++;
                      p_buf_temp++;

                      //Hardware_version
                      up_info.hwVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Software_type
                      selector_offset += 2;
                      p_buf_temp += 2;

                      //Software_version
                      up_info.swVersion = make32(&p_buf_temp[0]);
                      selector_offset += 4;
                      p_buf_temp += 4;

                      //Serial_number_start
                      memcpy(up_info.Serial_number_start, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Serial_number_end
                      memcpy(up_info.Serial_number_end, p_buf_temp, 16);
                      selector_offset += 16;
                      p_buf_temp += 16;

                      //Private_data
                      selector_offset += 4;
                      p_buf_temp += 4;

                      selector_length -= selector_offset;
                      #if 1
                      UI_PRINTF("up_info.freq=%d\n", up_info.freq);
                      UI_PRINTF("up_info.symbol=%d\n", up_info.symbol);
                      UI_PRINTF("up_info.qam_mode=%d\n", up_info.qam_mode);
                      UI_PRINTF("up_info.Serial_number_start=%s\n", up_info.Serial_number_start);
                      UI_PRINTF("up_info.Serial_number_end=%s\n", up_info.Serial_number_end);
                      UI_PRINTF("up_info.swVersion=%d\n", up_info.swVersion);
                      UI_PRINTF("up_info.hwVersion=%d\n", up_info.hwVersion);
                      #endif
                    }
                  }
                  break;

                default:
                  break;
              }
            }

            desc_len -= link_desc_len;
            offset   += link_desc_len;   

            offset ++;
            desc_len -- ;
            break;
          
          default:
          /*! 
            Jump to unknown desc length
           */
          offset ++;
          desc_len --;

          //UI_PRINTF("MDL: parse nit tmp_desc_length %d\n",p_buf[offset]);
          //UI_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          //Skip content of unkown descriptor
          //if(desc_len > tmp_desc_length)
          {
            desc_len -=  (s16)p_buf[offset];
            offset   +=  p_buf[offset];   
          }

          //UI_PRINTF("MDL: parse nit offset %d\n",  offset);
          //UI_PRINTF("MDL: parse nit desc_len %d\n",desc_len);

          offset ++;
          desc_len -- ;
          break;
        }

    }
    
    offset = nxt_ts_start ;
    tp_cnt ++;
    if(tp_cnt >= MAX_TP_NUM)
    {
      break;
    }
  }

  /*! Saving total tp number*/
  p_nit->total_tp_num = tp_cnt;
  p_nit->table_id = p_sec->table_id;
  
  //All the tasks receiving this message must have higher task priority 
  //than DVB task
  msg.content = DVB_NIT_FOUND;
  msg.is_ext = 0;
  msg.para1 =(u32)(p_nit);
  msg.para2 = sizeof(nit_t);  
  msg.context = p_sec->r_context;
  p_svc->notify(p_svc, &msg);

  //UI_PRINTF(("MDL: End parse nit\n"));

  return;  
}

void request_bat_section_india(dvb_section_t *p_sec, u32 table_id, u32 para)
{
  u16 bouquet_id = para & 0xFFFF;
  u8 sec_number = (para & 0xFF0000) >> 16;
  dvb_t *p_dvb_handle = (dvb_t *)class_get_handle_by_id(DVB_CLASS_ID);
  
  MT_ASSERT(p_dvb_handle != NULL);

  
  p_sec->pid      = DVB_BAT_PID;
  p_sec->table_id = table_id;
  p_sec->timeout = (u32)4000;
  p_sec->direct_data = FALSE;
  p_sec->crc_enable = 1;
  memset(p_sec->filter_code, 0, sizeof(u8) * MAX_FILTER_MASK_BYTES);
  memset(p_sec->filter_mask, 0,sizeof(u8) * MAX_FILTER_MASK_BYTES);
  
  p_sec->filter_mode = FILTER_TYPE_SECTION;  
  p_sec->filter_code[0] = p_sec->table_id;
  p_sec->filter_mask[0] = 0xFF;
#ifndef WIN32
  p_sec->filter_code[1] = bouquet_id >> 8;
  p_sec->filter_mask[1] = 0xFF;
  p_sec->filter_code[2] = bouquet_id & 0xFF;
  p_sec->filter_mask[2] = 0xFF;
  p_sec->filter_code[4] = (u8)sec_number;
  p_sec->filter_mask[4] = 0xFF;  
  p_sec->filter_mask_size = 5;
#else
  p_sec->filter_code[3] = bouquet_id >> 8;
  p_sec->filter_mask[3] = 0xFF;
  p_sec->filter_code[4] = bouquet_id & 0xFF;
  p_sec->filter_mask[4] = 0xFF;
  p_sec->filter_code[6] = (u8)sec_number;
  p_sec->filter_mask[6] = 0xFF;  
  p_sec->filter_mask_size = 7;
#endif
  p_dvb_handle->alloc_section(p_dvb_handle, p_sec);
}

