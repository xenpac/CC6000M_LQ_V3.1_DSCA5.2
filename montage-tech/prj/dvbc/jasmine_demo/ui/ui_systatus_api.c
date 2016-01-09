/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

//#define SS_DEBUG

#ifdef SS_DEBUG
#define SS_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define SS_PRINTF(param ...)    do { } while (0)
#else
#define SS_PRINTF
#endif
#endif

static BOOL _get_curn_info(group_set_t *p_group_set, u16 pos, u8 mode,
                           u16 *p_curn_rid)
{
  curn_info_t *p_group_info;
  u16 pg_id = INVALID;

  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  p_group_info = &p_group_set->group_info[pos];
  pg_id =
    (mode == CURN_MODE_TV) ? p_group_info->curn_tv : p_group_info->curn_radio;

  if(pg_id == INVALIDID)
  {
    return FALSE;
  }

  *p_curn_rid = pg_id;

  return TRUE;
}


static BOOL _set_curn_info(group_set_t *p_group_set, u16 pos, u8 mode, u16 rid, u16 rpos)
{
  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  if(mode == CURN_MODE_TV)
  {
    p_group_set->group_info[pos].curn_tv = rid;
    p_group_set->group_info[pos].tv_pos = rpos;
  }
  else
  {
    p_group_set->group_info[pos].curn_radio = rid;
    p_group_set->group_info[pos].rd_pos = rpos;
  }

  return TRUE;
}


static BOOL _check_curn_group(group_set_t *p_group_set, u8 mode)
{
  u16 i, rid, curn = p_group_set->curn_group;

  //check curn-> total-1
  for (i = curn; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  //check curn-1 -> 0
  for (i = 0; i < curn; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  return FALSE;
}


static BOOL _check_curn_info(group_set_t *p_group_set)
{
  u8 mode = p_group_set->curn_mode;
  u32 org_context, new_context;
  u8 org_type, new_type;
  u16 pos;

  sys_status_get_group_info(p_group_set->curn_group, &org_type, &pos, &org_context);

  if (mode == CURN_MODE_NONE)
  {
    mode = CURN_MODE_TV;
  }

  // check curn mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // switch curn mode
  if (mode == CURN_MODE_TV)
  {
    mode = CURN_MODE_RADIO;
  }
  else
  {
    mode = CURN_MODE_TV;
  }

  // check other mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // no prog in all mode
  mode = CURN_MODE_NONE;
  p_group_set->curn_group = 0;

 _SET_CURN_MODE_:
  p_group_set->curn_mode = mode;

  sys_status_get_group_info(sys_status_get_curn_group(),
      &new_type, &pos, &new_context);

  return ((org_context == new_context) && (org_type == new_type));
}

static void _check_curn_pg(curn_info_t *p_info, u8 view_id,u8 mode)
{
  u16 rid, pos, cnt;

  // chk by cnt
  cnt = db_dvbs_get_count(view_id);
  if (cnt == 0)
  {
    if(mode == CURN_MODE_TV)
    {
      p_info->curn_tv = INVALIDID;
      p_info->tv_pos = INVALIDPOS;      
    }
    else
    {
      p_info->curn_radio = INVALIDID;
      p_info->rd_pos = INVALIDPOS;      
    }
    return;
  }
  // chk by id
  pos = db_dvbs_get_view_pos_by_id(view_id, p_info->curn_tv);
  if (pos != INVALIDPOS)
  {

    if(mode == CURN_MODE_TV)
    p_info->tv_pos = pos;
    else
    p_info->rd_pos = INVALIDPOS;
    return;
  }
  // chk by pos
  rid = db_dvbs_get_id_by_view_pos(view_id, p_info->tv_pos);
  if (rid != INVALIDID)
  {
    if(mode == CURN_MODE_TV)
    p_info->curn_tv = rid;
    else
    p_info->curn_radio = rid;
    return;
  }
  // try to keep the pos
  if(mode == CURN_MODE_TV)
  pos = p_info->tv_pos;
  else
  pos = p_info->rd_pos;
  if (pos == INVALIDPOS || pos >= cnt)
  {
    pos = cnt - 1; /* the last */
  }

  rid = db_dvbs_get_id_by_view_pos(view_id, pos);
  MT_ASSERT(rid != INVALIDID);

  if(mode == CURN_MODE_TV)
  {
    p_info->curn_tv = rid;
    p_info->tv_pos = pos;    
  }
  else
  {
    p_info->curn_radio = rid;
    p_info->rd_pos = pos;    
  }
}

static void _check_group_fav(group_set_t *p_group_set)
{
  curn_info_t *p_group;
  u16 i;
  u8 pg_vid;

  for (i = 0; i < MAX_PRESET_FAV_CNT; i++)
  {
    pg_vid = ui_dbase_create_view(DB_DVBS_FAV_ALL,i, NULL);
    if(db_dvbs_get_count(pg_vid) != 0)
    {
        p_group = &p_group_set->group_info[i + DB_DVBS_MAX_SAT +1];

        pg_vid = ui_dbase_create_view(DB_DVBS_FAV_TV, i , NULL);

        _check_curn_pg(p_group, pg_vid ,CURN_MODE_TV);

        pg_vid = ui_dbase_create_view(DB_DVBS_FAV_RADIO, i , NULL);
        _check_curn_pg(p_group, pg_vid,CURN_MODE_RADIO);

        p_group->context = (i);
    }
    else
    {
        p_group = &p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1 /*all group*/];
        p_group->curn_tv = INVALIDID, p_group->tv_pos= INVALIDPOS;
        p_group->curn_radio = INVALIDID, p_group->rd_pos= INVALIDPOS;
        p_group->context = 0;
    }
  }

  for (i = MAX_PRESET_FAV_CNT;
                             i < MAX_CATEGORY_CNT + MAX_PRESET_FAV_CNT; i++)
  {
    pg_vid = ui_dbase_create_view(DB_DVBS_FAV_ALL, i, NULL);
    if(db_dvbs_get_count(pg_vid) != 0)
    {
        p_group = &p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1];

        pg_vid = ui_dbase_create_view(DB_DVBS_FAV_TV, i , NULL);

        _check_curn_pg(p_group, pg_vid ,CURN_MODE_TV);

        pg_vid = ui_dbase_create_view(DB_DVBS_FAV_RADIO, i , NULL);
        _check_curn_pg(p_group, pg_vid,CURN_MODE_RADIO);

        p_group->context = (i);
    }
    else
    {
        p_group = &p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1];
        p_group->curn_tv = INVALIDID, p_group->tv_pos= INVALIDPOS;
        p_group->curn_radio = INVALIDID, p_group->rd_pos= INVALIDPOS;
        p_group->context = 0;
    }
  }

}

BOOL sys_status_check_group(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;
  spec_check_group_t group = {{0}};
  u16 i = 0, j = 0;
  BOOL ret_boo = FALSE;
  //u32 ticks = mtos_ticks_get();

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  //to set the group info before check group
  //set all group
  group.all_group.orig_tv = p_group_set->group_info[0].curn_tv;
  group.all_group.orig_rd = p_group_set->group_info[0].curn_radio;
  group.all_group.orig_tv_pos = p_group_set->group_info[0].tv_pos;
  group.all_group.orig_rd_pos = p_group_set->group_info[0].rd_pos;

  //set sat group
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    group.sat_group[i].context = (u16)p_group_set->group_info[i + 1].context;
    group.sat_group[i].orig_tv = p_group_set->group_info[i + 1].curn_tv;
    group.sat_group[i].orig_rd = p_group_set->group_info[i + 1].curn_radio;
    group.sat_group[i].orig_tv_pos = p_group_set->group_info[i + 1].tv_pos;
    group.sat_group[i].orig_rd_pos = p_group_set->group_info[i + 1].rd_pos;
  }

  //check group
  special_check_group(&group, NULL);

  //to reset the group info after check.
  //all group
    p_group_set->group_info[0].curn_tv = group.all_group.tv_id;
    p_group_set->group_info[0].tv_pos = group.all_group.tv_pos;
    p_group_set->group_info[0].curn_radio = group.all_group.rd_id;
    p_group_set->group_info[0].rd_pos = group.all_group.rd_pos;

  //reset sat group info
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
      p_group_set->group_info[i + 1].curn_tv = group.sat_group[i].tv_id;
      p_group_set->group_info[i + 1].tv_pos = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].curn_radio = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].rd_pos = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].context = group.sat_group[i].context;

    if((p_group_set->group_info[i + 1].curn_tv    == INVALIDID)
    && (p_group_set->group_info[i + 1].curn_radio == INVALIDID))
    {
      p_group_set->group_info[i + 1].context = INVALIDID;
    }

    group.sat_group[i].context = p_group_set->group_info[i + 1].context;
    group.sat_group[i].orig_rd = p_group_set->group_info[i + 1].curn_radio;
    group.sat_group[i].orig_tv = p_group_set->group_info[i + 1].curn_tv;
    group.sat_group[i].orig_rd_pos = p_group_set->group_info[i + 1].rd_pos;
    group.sat_group[i].orig_tv_pos = p_group_set->group_info[i + 1].tv_pos;
  }

  j=0;
  for(i=0; i<DB_DVBS_MAX_SAT; i++)
  {
    p_group_set->group_info[i + 1].context    = INVALIDID;
    p_group_set->group_info[i + 1].curn_radio = INVALIDID;
    p_group_set->group_info[i + 1].curn_tv    = INVALIDID;
    p_group_set->group_info[i + 1].rd_pos = 0;
    p_group_set->group_info[i + 1].tv_pos = 0;

    if(group.sat_group[i].context != INVALIDID)
    {
      p_group_set->group_info[j + 1].context    = group.sat_group[i].context;
      p_group_set->group_info[j + 1].curn_radio = group.sat_group[i].orig_rd;
      p_group_set->group_info[j + 1].curn_tv    = group.sat_group[i].orig_tv;
      p_group_set->group_info[j + 1].rd_pos = group.sat_group[i].orig_rd_pos;
      p_group_set->group_info[j + 1].tv_pos = group.sat_group[i].orig_tv_pos;

      j++;
    }
  }
  _check_group_fav(p_group_set);

  //set curn group and curn mode
  ret_boo = _check_curn_info(p_group_set);

  return ret_boo;
}


u16 sys_status_get_sat_group_num(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  u8 mode;
  u16 i, num = 0;
  u16 rid;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    if (_get_curn_info(p_group_set, i + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_sat_group_pos(u16 group)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  u8 mode;
  u16 i, num = 0;
  u16 rid;

  MT_ASSERT(group > 0);
  MT_ASSERT(group <= DB_DVBS_MAX_SAT);

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < group; i++)
  {
    if (_get_curn_info(p_group_set, i + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_fav_group_num(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;

  for (i = 0; i < MAX_FAV_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i + DB_DVBS_MAX_SAT + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_fav_group_pos(u16 group)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;

  MT_ASSERT(group > DB_DVBS_MAX_SAT);
  MT_ASSERT(group <= (DB_DVBS_MAX_SAT + MAX_FAV_CNT));

  for (i = 0; i < (group - DB_DVBS_MAX_SAT); i++)
  {
    if (_get_curn_info(p_group_set, i + DB_DVBS_MAX_SAT + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_all_group_num(void)
{
  u16 num = 0;

  num += sys_status_get_sat_group_num();
  num += sys_status_get_fav_group_num();

  if(num > 0)
  {
    num++;
  }

  return num;
}

BOOL sys_status_get_group_all_info(u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_get_curn_info(p_group_set, 0, mode, &rid))
  {
    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}

BOOL sys_status_get_sat_group_info(u16 sat_pos, u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;


  if (sat_pos >= DB_DVBS_MAX_SAT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_get_curn_info(p_group_set, sat_pos + 1, mode, &rid))
  {
    *p_curn_rid = INVALIDID;

    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}


BOOL sys_status_get_fav_group_info(u16 fav_pos, u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_get_curn_info(p_group_set, fav_pos + DB_DVBS_MAX_SAT + 1, mode, &rid))
  {
    *p_curn_rid = INVALIDID;
    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}

BOOL sys_status_set_group_all_info(u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_set_curn_info(p_group_set, 0, mode, curn_rid, pos))
  {
    return FALSE;
  }

  sys_bg_data_save();

  return TRUE;
}

static BOOL sys_status_set_sat_group_info(u16 sat_pos, u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  if (sat_pos >= DB_DVBS_MAX_SAT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_set_curn_info(p_group_set, sat_pos + 1, mode, curn_rid, pos))
  {
    return FALSE;
  }

  sys_bg_data_save();

  return TRUE;
}


static BOOL sys_status_set_fav_group_info(u16 fav_pos, u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  if (!_set_curn_info(p_group_set, fav_pos + DB_DVBS_MAX_SAT + 1, mode, curn_rid, pos))
  {
    return FALSE;
  }

  sys_bg_data_save();

  return TRUE;
}

BOOL sys_status_get_group_info(u16 group, u8 *p_group_type, u16 *p_pos_in_set, u32 *p_context)
{
  sys_bg_data_t *p_bg_data;
  group_set_t *p_group_set;

  u16 pos;

  if (group >= MAX_GROUP_CNT)
  {
    return FALSE;
  }

  if (group == 0)
  {
    pos = 0;
    *p_group_type = GROUP_T_ALL;
  }
  else if (group < DB_DVBS_MAX_SAT + 1)
  {
    pos = group - 1;
    *p_group_type = GROUP_T_SAT;
  }
  else
  {
    pos = group - DB_DVBS_MAX_SAT - 1;
    *p_group_type = GROUP_T_FAV;
  }

  *p_pos_in_set = pos;


  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  *p_context =  p_group_set->group_info[group].context;

  return TRUE;
}


u16 sys_status_get_curn_group(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  return p_group_set->curn_group;
}


BOOL sys_status_set_curn_group(u16 group)
{
  u8 type;
  u16 pos;
  u32 context;

  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  // check
  if (!sys_status_get_group_info(group, &type, &pos, &context))
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  p_group_set->curn_group = group;

  sys_bg_data_save();

  return TRUE;
}

/* To solve bug57070,add this function */
BOOL sys_status_set_curn_group_not_write_flash(u16 group)
{
  u8 type;
  u16 pos;
  u32 context;

  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  // check
  if (!sys_status_get_group_info(group, &type, &pos, &context))
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  p_group_set->curn_group = group;

  return TRUE;
}

static s16 GroupProcess(s16 CurG, BOOL IsDown)
{
  s16 FinalG = 0;
  {
    if (IsDown)
    {
      CurG--;
      if (CurG < 0)
      {
        CurG = MAX_GROUP_CNT-1;
      }
    }
    else
    {
      CurG++;
      if (CurG == MAX_GROUP_CNT)
      {
        CurG = 0;
      }
    }

    FinalG = CurG;
  }
  
  return FinalG;
}
static u16 goto_next_group(group_set_t *p_group_set, BOOL is_reverse)
{
  s16 curn_group, next_group;
  u16 rid = INVALIDID;

  curn_group = (s16)p_group_set->curn_group;
  next_group = curn_group;

  do
  {
    next_group = GroupProcess(next_group, is_reverse);
    if (next_group == curn_group)
    {
      // no group has pgs, except curn group
      break;
    }
  }while(!_get_curn_info(p_group_set, (u16)next_group, p_group_set->curn_mode, &rid));

  return (u16)next_group;
}

u16 sys_status_shift_curn_group(s16 offset)
{
  BOOL is_reverse;
  u16 i, count, curn_group;

  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  count = (u16)(ABS(offset));
  is_reverse = (BOOL)(offset < 0);

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  curn_group = p_group_set->curn_group;

  for (i = 0; i < count; i++)
  {
    curn_group = goto_next_group(p_group_set, is_reverse);
  }

//  curn_group = ((curn_group + offset) % all_num + all_num) % all_num;
  p_group_set->curn_group = curn_group;

  return curn_group;
}


BOOL sys_status_get_curn_prog_in_group(u16 group, u8 mode, u16 *p_rid, u32 *p_context)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    *p_rid = INVALIDID;
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  *p_context = context;

  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_get_fav_group_info(group_pos, mode, p_rid);
      break;

    case GROUP_T_SAT:
      sys_status_get_sat_group_info(group_pos, mode, p_rid);
      break;

    default:
      p_bg_data = sys_bg_data_get();
      p_group_set = &p_bg_data->group_set;
      _get_curn_info(p_group_set, 0, mode, p_rid);
  }

  return TRUE;
}


static BOOL sys_status_set_curn_prog_in_group(u16 group, u8 mode, u16 rid, u16 pos)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_set_fav_group_info(group_pos, mode, rid, pos);
      break;

    case GROUP_T_SAT:
      sys_status_set_sat_group_info(group_pos, mode, rid, pos);
      break;

    default: /* all group */
        sys_status_set_group_all_info(mode, rid, pos);
        break;
  }

  return TRUE;
}


u8 sys_status_get_curn_prog_mode(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  return p_group_set->curn_mode;
}


BOOL sys_status_set_curn_prog_mode(u8 mode)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  p_group_set->curn_mode = mode;

  sys_bg_data_save();

  return TRUE;
}

u8 sys_status_get_group_curn_type(void)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;

  return p_group_set->curn_type;
}

BOOL sys_status_set_group_curn_type(u8 type)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  p_group_set->curn_type = type;

  sys_bg_data_save();

  return TRUE;
}


u16 sys_status_get_curn_group_curn_prog_id(void)
{
  u16 curn_group;
  u8 curn_mode;
  u16 pg_id;
  u32 context;

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  if(sys_status_get_curn_prog_in_group(curn_group, curn_mode, &pg_id, &context))
  {
    return pg_id;
  }
  else
  {
    return INVALIDID;
  }
}

BOOL sys_status_set_curn_group_info(u16 rid, u16 pos)
{
  u16 curn_group;
  u8 curn_mode;


  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  return sys_status_set_curn_prog_in_group(curn_group, curn_mode, rid, pos);

}

void sys_status_get_view_info(u16 group, u8 mode, u16 *p_view_type, u32 *p_context)
{
  u8 group_type, view_type;
  u16 pos_in_set;
  u32 context;

  if (mode == CURN_MODE_NONE)
  {
    *p_view_type = DB_DVBS_INVALID_VIEW;
    *p_context = 0;
    return;
  }



  sys_status_get_group_info(group, &group_type, &pos_in_set, &context);

  switch(group_type)
  {
    case GROUP_T_ALL:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO;
      break;
    case GROUP_T_FAV:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_FAV_TV:DB_DVBS_FAV_RADIO;
      break;
    default:
      MT_ASSERT(0);
      return;
  }

  *p_view_type = view_type;
  *p_context = context;
}


void sys_status_get_curn_view_info(u16 *p_view_type, u32 *p_context)
{
  u8 curn_mode;
  u16 curn_group;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  sys_status_get_view_info(curn_group, curn_mode, p_view_type, p_context);
}


u16 sys_status_get_pos_by_group(u16 group)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;
  u8 mode;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;

  for(i = 0; i <= group; i++)
  {
    if(_get_curn_info(p_group_set, i, mode, &rid))
    {
      num++;
    }
  }

  return (num - 1);
}

u16 sys_status_get_group_by_pos(u16 pos)
{
  group_set_t *p_group_set;
  sys_bg_data_t *p_bg_data;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_bg_data = sys_bg_data_get();
  p_group_set = &p_bg_data->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      if(num == pos)
      {
        return i;
      }
      num++;
    }
  }

  return INVALIDPOS;
}

void sys_status_get_group_name(u16 group, u16 *p_name, u32 len)
{
  u8 type = 0;
  u16 pos = 0;
  u32 context = 0;

  sys_status_get_group_info(group, &type, &pos, &context);
  switch (type)
  {
    case GROUP_T_ALL:
      gui_get_string(IDS_ALL, p_name, (u16)len);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_name((u8)context, p_name);
      break;
    default:
      MT_ASSERT(0);
  }
}

scart_v_format_t sys_status_get_scart_out_cfg(u8 focus)
{
  scart_v_format_t mode = SCART_VID_CVBS;

  switch(focus)
  {
    case 0: // RGB: DONT change the order.
      mode = SCART_VID_RGB;
      break;
    case 1: //YUV
      mode = SCART_VID_YUV;
      break;
    case 2: // CVBS
      mode = SCART_VID_CVBS;
      break;
    default:
      MT_ASSERT(0);
  }

  return mode;
}

avc_video_mode_1_t sys_status_get_sd_mode(u8 focus)
{
  avc_video_mode_1_t sd_mode = AVC_VIDEO_MODE_AUTO_1;
  switch(focus)
  {
    case 0:
      sd_mode = AVC_VIDEO_MODE_PAL_1;
      break;

    case 1:
      sd_mode = AVC_VIDEO_MODE_NTSC_1;
      break;

    case 2:
      sd_mode = AVC_VIDEO_MODE_AUTO_1;
      break;

    default:
      MT_ASSERT(0);
      break;
  }
  return sd_mode;
}

avc_video_mode_1_t sys_status_get_hd_mode(u8 focus)
{
  avc_video_mode_1_t hd_mode = AVC_VIDEO_MODE_AUTO_1;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  disp_sys_t video_std = VID_SYS_AUTO;

  avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);
  video_std = avc_get_video_mode_1(avc_handle);

  switch (video_std)
  {
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_480I;
          break;

        case 1:
          hd_mode = AVC_VIDEO_MODE_480P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_60HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_60HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_60HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_576I;
          break;

        case 1:
          hd_mode = AVC_VIDEO_MODE_576P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_50HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_50HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_50HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    default:
//      MT_ASSERT(0);
      break;
  }

  return hd_mode;
}

avc_video_mode_1_t sys_status_get_hd_mode_1(u8 focus,void *p_disp)
{
  avc_video_mode_1_t hd_mode = AVC_VIDEO_MODE_AUTO_1;
  disp_sys_t video_std = VID_SYS_AUTO;
  disp_get_tv_sys(p_disp, DISP_CHANNEL_SD, &video_std);
  switch (video_std)
  {
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_480I;
          break;

        case 1:
          hd_mode = AVC_VIDEO_MODE_480P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_60HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_60HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_60HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_576I;
          break;

        case 1:
          hd_mode = AVC_VIDEO_MODE_576P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_50HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_50HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_50HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    default:
//      MT_ASSERT(0);
      break;
  }

  return hd_mode;
}

avc_video_aspect_1_t sys_status_get_video_aspect(u8 focus)
{
  switch(focus)
  {
  case 0:
    return AVC_VIDEO_ASPECT_AUTO_1;

  case 1:
    return AVC_VIDEO_ASPECT_43_LETTERBOX_1;

  case 2:
    return AVC_VIDEO_ASPECT_43_PANSCAN_1;

  case 3:
    return AVC_VIDEO_ASPECT_169_1;

  default:
    return AVC_VIDEO_ASPECT_AUTO_1;
  }
}

scart_v_aspect_t sys_status_get_scart_aspect(u8 focus)
{
  MT_ASSERT(focus < 5);

  return (sys_status_get_video_aspect(focus) >= AVC_VIDEO_ASPECT_169_1) ?
         SCART_ASPECT_16_9 : SCART_ASPECT_4_3;
}


rf_sys_t sys_status_get_rf_system(u8 focus)
{
  rf_sys_t sys = RF_SYS_NTSC;

  return (rf_sys_t)(sys + focus);
}

u16 sys_status_get_rf_channel(BOOL is_pal, u8 focus)
{
  if(is_pal)  // PAL
  {
    focus += RF_PAL_MIN;
  }
  else // NTSC
  {
    focus += RF_NTSC_MIN;
  }

  return (u16)focus;
}

u8 sys_status_get_global_media_volume(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->global_media_volume;
}

void sys_status_set_global_media_volume(u8 volume)
{
  sys_status_t *p_status = sys_status_get();
  p_status->global_media_volume = volume;
}

u8 sys_status_get_brightness(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->brightness;
}

void sys_status_set_brightness(u8 brightness)
{
  sys_status_t *p_status = sys_status_get();
  p_status->brightness = brightness;
}

u8 sys_status_get_contrast(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->contrast;
}

void sys_status_set_contrast(u8 contrast)
{
  sys_status_t *p_status = sys_status_get();
  p_status->contrast = contrast;
}

u8 sys_status_get_saturation(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->saturation;
}

void sys_status_set_saturation(u8 saturation)
{
  sys_status_t *p_status = sys_status_get();
  p_status->saturation = saturation;
}
void sys_status_reload_environment_setting(BOOL restory)
{
  BOOL is_enable, is_first_state;
  av_set_t av_set;
  osd_set_t osd_set;
  language_set_t lang_set;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
#ifdef DISPLAY_AND_VOLUME_AISAT
  u8 bright,sature,contrast;
  void *p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  RET_CODE ret = SUCCESS;
#endif
  nim_device_t *p_dev = dev_find_identifier(NULL,
                                            DEV_IDT_TYPE,
                                            SYS_DEV_TYPE_NIM);

  //set lnb power
  sys_status_get_status(BS_LNB_POWER, &is_enable);
  sys_status_get_lang_set(&lang_set);
  sys_status_get_status(BS_FIRST_ON, &is_first_state);
  sys_status_get_av_set(&av_set);

  if((!is_first_state) || restory)
  {
    if(!restory)
      sys_status_set_status(BS_FIRST_ON, 1);
  }

  sys_status_set_av_set(&av_set);
  sys_status_set_lang_set(&lang_set);
  sys_status_set_status(BS_LNB_POWER,is_enable);
  rsc_set_curn_language(gui_get_rsc_handle(), lang_set.osd_text + 1);
  sys_status_save();
  dev_io_ctrl(p_dev, NIM_IOCTRL_SET_LNB_ONOFF, (u32)is_enable);
  //set video
  avc_switch_video_mode_1(avc_handle, sys_status_get_sd_mode(av_set.tv_mode));
  avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
  avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));


  #ifndef WIN32
   {
    void *p_disp = dev_find_identifier(NULL,
                                    DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      MT_ASSERT(p_disp != NULL);

   disp_set_postprocess_mode(p_disp, (disp_pp_mode_t)(av_set.video_effects));
   }
 #endif

  //set osd
  sys_status_get_osd_set(&osd_set);
  gdi_set_global_alpha((u8)((100 - osd_set.transparent) * 255 / 100));
#if ENABLE_TTX_SUBTITLE
  ui_enable_vbi_insert((BOOL)osd_set.enable_vbinserter);
#endif
#ifdef DISPLAY_AND_VOLUME_AISAT
// display
  bright = sys_status_get_brightness();
  sature = sys_status_get_saturation();
  contrast = sys_status_get_contrast();
  //transparency = sys_status_get_transparency();
  
  ret = disp_set_bright(p_disp_dev, DISP_CHANNEL_HD, bright);
  MT_ASSERT(SUCCESS == ret);
  ret = disp_set_sature(p_disp_dev, DISP_CHANNEL_HD, sature);
  MT_ASSERT(SUCCESS == ret);
  ret = disp_set_contrast(p_disp_dev, DISP_CHANNEL_HD, contrast);
  MT_ASSERT(SUCCESS == ret);
#endif

}

u16 sys_status_get_fav1_index(void)
{
  u16 index = 0;

  if(sys_status_get_categories_count()>FAV_LIST_COUNT)
  {
    index = sys_status_get_categories_count()-FAV_LIST_COUNT;
  }

  return index;
}

BOOL get_dm_block_real_file_info(u8 block_id,dmh_block_info_t *block_info)
{
  void *p_dev = NULL;
  u32 block_addr = 0;
  u32 block_size = 0;
  u32 wr_addr = 0;
  dmh_block_info_t real_dm_info = {0};
  RET_CODE ret = ERR_NOFEATURE;
  dmh_block_info_t block_dm_info = {0};
  if(dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) == DM_FAIL)
  {
    return -1;
  }
  if(block_dm_info.type != BLOCK_TYPE_RO)
  {
    memcpy(block_info,&block_dm_info,sizeof(dmh_block_info_t));
    return 0;
  }
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);
  block_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
  if(block_addr == 0)
    {
       return -1;
    }
  block_addr = block_addr - get_flash_addr();
  block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),block_id);
  wr_addr = block_addr + block_size - sizeof(dmh_block_info_t);
  ret = charsto_read(p_dev,wr_addr,(u8 *)&real_dm_info,sizeof(dmh_block_info_t));
  if((ret == SUCCESS) && (block_id == real_dm_info.id))
    {
        memcpy(block_info,&real_dm_info,sizeof(dmh_block_info_t));
        return 0;
    }
  return -1;
}

u32 get_dm_block_real_file_size(u8 block_id)
{
    dmh_block_info_t real_dm_info = {0};
    BOOL ret = 0;
    ret = get_dm_block_real_file_info(block_id,&real_dm_info);
    if(ret == 0)
    {   
        return real_dm_info.size;
    }
    return 0; 
}

u16 get_dm_block_real_file_ota_version(u8 block_id)
{
    dmh_block_info_t real_dm_info = {0};
    BOOL ret = 0;
    ret = get_dm_block_real_file_info(block_id,&real_dm_info);
    if(ret == 0)
    {   
        return real_dm_info.ota_ver;
    }
    return 0; 
}

u32 get_dm_block_real_file_crc(u8 block_id)
{
    dmh_block_info_t real_dm_info = {0};
    BOOL ret = 0;
    ret = get_dm_block_real_file_info(block_id,&real_dm_info);
    if(ret == 0)
    {   
        return real_dm_info.crc;
    }
    return 0; 
}

u32 get_dm_block_real_file_addr(u8 block_id)
{
    dmh_block_info_t real_dm_info = {0};
    u32 block_addr = 0;
    u32 dm_addr = 0;
    BOOL ret = 0;
    dmh_block_info_t block_dm_info = {0};
     if(dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) == DM_FAIL)
      {
        return 0;
      }

     block_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
     block_addr = block_addr - get_flash_addr();
     dm_addr = block_addr - block_dm_info.base_addr;
      if(block_dm_info.type != BLOCK_TYPE_RO)
      {
        return block_addr;
      }
    
    ret = get_dm_block_real_file_info(block_id,&real_dm_info);
    if((ret == 0) && (block_addr == (real_dm_info.base_addr + dm_addr)))
    {   
        return block_addr;
    }
    return 0; 
}


BOOL set_dm_block_real_file_ota_version(u8 block_id,u16 version) 
{
    u8 *p_block_buffer = NULL;
    dmh_block_info_t real_dm_info = {0};
    u32 block_addr = 0;
    RET_CODE ret = 0;
    u32 block_size = 0;
    void *p_dev = NULL;
    u32 check_value = 0;
    u32 wr_addr = 0;
    dmh_block_info_t block_dm_info = {0};
     if(dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) == DM_FAIL)
      {
        return -1;
      }
     if(block_dm_info.type != BLOCK_TYPE_RO)
      {
        return -1;
      }

    block_addr = get_dm_block_real_file_addr(block_id);
    if(block_addr == 0)
    {   
      return -1;
    }
    block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),block_id);
    wr_addr = block_addr + block_size -CHARSTO_SECTOR_SIZE;
    check_value = wr_addr % CHARSTO_SECTOR_SIZE;
    if(check_value != 0)
    {
      return -1;
    }
    p_block_buffer = mtos_malloc(CHARSTO_SECTOR_SIZE);
    if(p_block_buffer == NULL)
    {
       return -1; 
    }
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
   MT_ASSERT(NULL != p_dev);
     /**read one block 64kbyte one time****/
    
    ret = charsto_read(p_dev,wr_addr, p_block_buffer,CHARSTO_SECTOR_SIZE);
    if (ret != SUCCESS)
    {
      mtos_free(p_block_buffer);
      return -1;
    }

    memcpy((u8 *)&real_dm_info,p_block_buffer + CHARSTO_SECTOR_SIZE - sizeof(dmh_block_info_t),sizeof(dmh_block_info_t));

    if( (block_id != real_dm_info.id))
    {
      mtos_free(p_block_buffer);
      return -1;
    }
    real_dm_info.base_addr = block_addr;
    real_dm_info.ota_ver = version;
   memcpy(p_block_buffer + CHARSTO_SECTOR_SIZE - sizeof(dmh_block_info_t),(u8 *)&real_dm_info,sizeof(dmh_block_info_t));
    ret = charsto_erase(p_dev,wr_addr, 1);
     if (ret != SUCCESS)
    {
      mtos_free(p_block_buffer);
      return -1;
    }

    ret = charsto_writeonly(p_dev,wr_addr,p_block_buffer,CHARSTO_SECTOR_SIZE);
     if (ret != SUCCESS)
    {
      mtos_free(p_block_buffer);
      return -1;
    }
    return 0; 
}


