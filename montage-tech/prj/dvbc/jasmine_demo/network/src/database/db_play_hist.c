/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#include "db_play_hist.h"
/*!
 * Const value
 */




/*!
 * Type define
 */
typedef struct
{
    u32 timestamp;
    u16 phy_pos;
} db_play_hist_time_t;


/*!
 * Function define
 */


/*!
 * Priv data
 */
static db_play_hist_time_t g_PHTimeTable[MAX_PLAY_HIST_CNT];
static db_play_hist_item g_play_hist_item;
static u16 g_play_hist_cnt;

/*================================================================================================
                           play history internel function
 ================================================================================================*/

void db_plht_sort_time_table(void)
{
    db_play_hist_item *p_plht_table;
    db_play_hist_time_t temp_time;
    u16 phy_pos, i, j, max;

    p_plht_table = sys_status_get_play_hist_table();
    memset(g_PHTimeTable, 0, sizeof(g_PHTimeTable));
    g_play_hist_cnt = 0;

    for (phy_pos = 0; phy_pos < MAX_PLAY_HIST_CNT; phy_pos++)
    {
        if (p_plht_table[phy_pos].is_used)
        {
            g_PHTimeTable[g_play_hist_cnt].timestamp = p_plht_table[phy_pos].timestamp;
            g_PHTimeTable[g_play_hist_cnt].phy_pos = phy_pos;
            g_play_hist_cnt++;
        }
    }

    for (i = 0; i < g_play_hist_cnt; i++)
    {
        max = i;
        for (j = i + 1; j < g_play_hist_cnt; j++)
        {
            if (g_PHTimeTable[j].timestamp > g_PHTimeTable[max].timestamp)
            {
                max = j;
            }
        }

        if (max != i)
        {
            temp_time = g_PHTimeTable[max];
            g_PHTimeTable[max] = g_PHTimeTable[i];
            g_PHTimeTable[i] = temp_time;
        }
    }
}

u16 db_plht_get_count(void)
{
    return g_play_hist_cnt;
}

BOOL db_plht_get_item_by_idx(u16 index, play_hist_info *p_info)
{
    MT_ASSERT(p_info != NULL);

    OS_PRINTF("@@@db_plht_get_item_by_idx index=%d, g_play_hist_cnt=%d\n", index, g_play_hist_cnt);
    if (index < g_play_hist_cnt)
    {
        sys_status_read_play_hist_item(g_PHTimeTable[index].phy_pos, &g_play_hist_item);

        p_info->vdo_id = g_play_hist_item.vdo_id;
        p_info->res_id = g_play_hist_item.res_id;
        p_info->b_single_page = g_play_hist_item.b_single_page;
        uni_strncpy(p_info->pg_name, g_play_hist_item.pg_name, MAX_PG_NAME_LEN);

        return TRUE;
    }

    return FALSE;
}

void db_plht_add_item(play_hist_info *p_info)
{
    s32 phy_pos;

    MT_ASSERT(p_info != NULL);

    if (g_play_hist_cnt >= MAX_PLAY_HIST_CNT)
    {
        db_plht_del_item_by_idx(g_play_hist_cnt-1);
    }

    phy_pos = sys_status_find_free_play_hist_item();
    OS_PRINTF("@@@db_plht_add_item phy_pos=%d\n", phy_pos);
    if (phy_pos >= 0)
    {
        g_play_hist_item.is_used = TRUE;
        if (g_play_hist_cnt > 0)
        {
            g_play_hist_item.timestamp = g_PHTimeTable[0].timestamp + 1;
        }
        else
        {
            g_play_hist_item.timestamp = 0;
        }
        g_play_hist_item.vdo_id = p_info->vdo_id;
        g_play_hist_item.res_id = p_info->res_id;
        g_play_hist_item.b_single_page = p_info->b_single_page;
        memcpy(g_play_hist_item.pg_name, p_info->pg_name, MAX_PG_NAME_LEN*sizeof(u16));

        sys_status_write_play_hist_item((u16)phy_pos, &g_play_hist_item);
        sys_status_save();

        if (g_play_hist_cnt > 0)
        {
            memmove(g_PHTimeTable+1, g_PHTimeTable, g_play_hist_cnt*sizeof(db_play_hist_time_t));
        }
        g_PHTimeTable[0].timestamp = g_play_hist_item.timestamp;
        g_PHTimeTable[0].phy_pos = (u16)phy_pos;
        g_play_hist_cnt++;
    }
}

void db_plht_add_unique_item(play_hist_info *p_info)
{
    play_hist_info temp_info;
    u16 i;

    for (i = 0; i < g_play_hist_cnt; i++)
    {
        db_plht_get_item_by_idx(i, &temp_info);
        if (temp_info.res_id == p_info->res_id && temp_info.vdo_id == p_info->vdo_id)
        {
            db_plht_del_item_by_idx(i);
            break;
        }
    }

    db_plht_add_item(p_info);
}

void db_plht_up_item_by_idx(u16 index, play_hist_info *p_info)
{
    u16 phy_pos;

    MT_ASSERT(p_info != NULL);

    OS_PRINTF("@@@db_plht_up_item_by_idx index=%d, g_play_hist_cnt=%d\n", index, g_play_hist_cnt);
    if (index < g_play_hist_cnt)
    {
        phy_pos = g_PHTimeTable[index].phy_pos;
        sys_status_read_play_hist_item(phy_pos, &g_play_hist_item);

        g_play_hist_item.vdo_id = p_info->vdo_id;
        g_play_hist_item.res_id = p_info->res_id;
        g_play_hist_item.b_single_page = p_info->b_single_page;
        memcpy(g_play_hist_item.pg_name, p_info->pg_name, MAX_PG_NAME_LEN*sizeof(u16));
        
        sys_status_write_play_hist_item(phy_pos, &g_play_hist_item);
        sys_status_save();
    }
}

void db_plht_del_item_by_idx(u16 index)
{
    OS_PRINTF("@@@db_plht_del_item_by_idx index=%d, g_play_hist_cnt=%d\n", index, g_play_hist_cnt);
    if (index < g_play_hist_cnt)
    {
        memset(&g_play_hist_item, 0, sizeof(db_play_hist_item));
        sys_status_write_play_hist_item(g_PHTimeTable[index].phy_pos, &g_play_hist_item);
        sys_status_save();

        if (index < g_play_hist_cnt - 1)
        {
            memmove(g_PHTimeTable+index, g_PHTimeTable+index+1, (g_play_hist_cnt-1-index)*sizeof(db_play_hist_time_t));
        }
        memset(g_PHTimeTable+g_play_hist_cnt-1, 0, sizeof(db_play_hist_time_t));
        g_play_hist_cnt--;
    }
}

void db_plht_del_all(void)
{
    u16 i;

    memset(&g_play_hist_item, 0, sizeof(db_play_hist_item));
    for (i = 0; i < g_play_hist_cnt; i++)
    {
        sys_status_write_play_hist_item(g_PHTimeTable[i].phy_pos, &g_play_hist_item);
    }
    sys_status_save();

    memset(g_PHTimeTable, 0, sizeof(g_PHTimeTable));
    g_play_hist_cnt = 0;
}



