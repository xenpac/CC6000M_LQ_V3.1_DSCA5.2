/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/

#include "ui_common.h"
#if 0
#include "string.h"

#include "sys_define.h"

#include "mtos_sem.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "drv_dev.h"
#include "charsto.h"
#include "gzip.h"
#include "lib_memp.h"
#include "data_manager.h"
#include "class_factory.h"
#endif




/*!
 * Const value
 */

#define CONN_PLAY_HASH_TABLE_LEN (MAX_CONN_PLAY_CNT + (MAX_CONN_PLAY_CNT >> 2))
#define HASH_OFFSET (0)
#define HASH_A (1)
#define HASH_B (2)


/*!
 * Type define
 */
typedef struct
{
    u32 timestamp;
    u16 phy_pos;
} db_conn_play_time_t;

typedef struct 
{
    u8  bExists;
    u16 offset_pos;

    u32 nHashA;
    u32 nHashB;

    u16 phy_pos;
} db_conn_play_hash_node_t; 

/*!
 * Function define
 */


/*!
 * Priv data
 */
static db_conn_play_time_t g_CPTimeTable[MAX_CONN_PLAY_CNT];
static db_conn_play_hash_node_t g_CPHashTable[CONN_PLAY_HASH_TABLE_LEN];
static db_conn_play_item g_conn_play_item;
static u16 g_conn_play_cnt;

/*================================================================================================
                           conn play internel function
 ================================================================================================*/

static u32 HashString(u16 *key, u32 dwHashType)
{
    register u32 hash = 5381 + dwHashType;
    u32 ch;

    while ((ch = (u32)*key++))
    {
        hash += (hash << 5) + ch;
    }

    return hash;
}

static s32 Hashed(u16 *key)
{
    //不同的字符串三次hash还会碰撞的几率无限接近于不可能
    u32 nHash = HashString(key, HASH_OFFSET);
    u32 nHashA = HashString(key, HASH_A);
    u32 nHashB = HashString(key, HASH_B);
    u32 nHashStart = nHash % CONN_PLAY_HASH_TABLE_LEN;
    u32 nHashPos = nHashStart;
    u16 offset_pos = g_CPHashTable[nHashStart].offset_pos;

#if 0
    while (g_CPHashTable[nHashPos].bExists)
    {
        if (g_CPHashTable[nHashPos].nHashA == nHashA && g_CPHashTable[nHashPos].nHashB == nHashB)
        {
            return nHashPos;
        }
        else
        {
            nHashPos = (nHashPos + 1) % CONN_PLAY_HASH_TABLE_LEN;
        }
        if (nHashPos == nHashStart)
        {
            break;
        }
    }
#endif

    if (nHashStart <= offset_pos)
    {
        for (; nHashPos <= offset_pos; nHashPos++)
        {
            if (g_CPHashTable[nHashPos].nHashA == nHashA && g_CPHashTable[nHashPos].nHashB == nHashB
                && g_CPHashTable[nHashPos].bExists)
            {
                return (s32)nHashPos;
            }
        }
    }
    else
    {
        for (; nHashPos < CONN_PLAY_HASH_TABLE_LEN; nHashPos++)
        {
            if (g_CPHashTable[nHashPos].nHashA == nHashA && g_CPHashTable[nHashPos].nHashB == nHashB
                && g_CPHashTable[nHashPos].bExists)
            {
                return (s32)nHashPos;
            }
        }

        for (nHashPos = 0; nHashPos <= offset_pos; nHashPos++)
        {
            if (g_CPHashTable[nHashPos].nHashA == nHashA && g_CPHashTable[nHashPos].nHashB == nHashB
                && g_CPHashTable[nHashPos].bExists)
            {
                return (s32)nHashPos;
            }
        }
    }

    return -1; //没有找到
}

static s32 Hash(u16 *key)
{
    u32 nHash = HashString(key, HASH_OFFSET);
    u32 nHashA = HashString(key, HASH_A);
    u32 nHashB = HashString(key, HASH_B);
    u32 nHashStart = nHash % CONN_PLAY_HASH_TABLE_LEN;
    u32 nHashPos = nHashStart;
    u16 curOffsetPos;
    u16 preOffsetPos;
#if 0
    while (g_CPHashTable[nHashPos].bExists)
    {
        nHashPos = (nHashPos + 1) % CONN_PLAY_HASH_TABLE_LEN;
        offset++;

        if (nHashPos == nHashStart) //一个轮回
        {
            //hash表中没有空余的位置了,无法完成hash
            return -1;
        }
    }
#endif
    OS_PRINTF("@@@Hash nHashPos=%d\n", nHashPos);
    do {
        for (; nHashPos < CONN_PLAY_HASH_TABLE_LEN; nHashPos++)
        {
            OS_PRINTF("@@@Hash bExists=%d\n", g_CPHashTable[nHashPos].bExists);
            if (g_CPHashTable[nHashPos].bExists == FALSE)
            {
                break;
            }
        }
        if (nHashPos < CONN_PLAY_HASH_TABLE_LEN)
        {
            break;
        }

        for (nHashPos = 0; nHashPos < nHashStart; nHashPos++)
        {
            if (g_CPHashTable[nHashPos].bExists == FALSE)
            {
                break;
            }
        }
        if (nHashPos < nHashStart)
        {
            break;
        }

        return -1;
    } while (FALSE);

    g_CPHashTable[nHashPos].bExists = TRUE;
    g_CPHashTable[nHashPos].nHashA = nHashA;
    g_CPHashTable[nHashPos].nHashB = nHashB;

    curOffsetPos = (u16)nHashPos;
    preOffsetPos = g_CPHashTable[nHashStart].offset_pos;
    if (curOffsetPos < nHashStart)
    {
        curOffsetPos += CONN_PLAY_HASH_TABLE_LEN;
    }
    if (preOffsetPos < nHashStart)
    {
        preOffsetPos += CONN_PLAY_HASH_TABLE_LEN;
    }
    if (curOffsetPos > preOffsetPos)
    {
        g_CPHashTable[nHashStart].offset_pos = (u16)nHashPos;
    }

    return (s32)nHashPos;
}

///////////////////////////////////////////////////////////////////
static void db_cnpl_sort_time_table(void)
{
    db_conn_play_item *p_cnpl_table;
    db_conn_play_time_t temp_time;
    u16 phy_pos, i, j, max;

    p_cnpl_table = sys_status_get_conn_play_table();
    memset(g_CPTimeTable, 0, sizeof(g_CPTimeTable));
    g_conn_play_cnt = 0;

    for (phy_pos = 0; phy_pos < MAX_CONN_PLAY_CNT; phy_pos++)
    {
        if (p_cnpl_table[phy_pos].is_used)
        {
            g_CPTimeTable[g_conn_play_cnt].timestamp = p_cnpl_table[phy_pos].timestamp;
            g_CPTimeTable[g_conn_play_cnt].phy_pos = phy_pos;
            g_conn_play_cnt++;
        }
    }

    for (i = 0; i < g_conn_play_cnt; i++)
    {
        max = i;
        for (j = i + 1; j < g_conn_play_cnt; j++)
        {
            if (g_CPTimeTable[j].timestamp > g_CPTimeTable[max].timestamp)
            {
                max = j;
            }
        }

        if (max != i)
        {
            temp_time = g_CPTimeTable[max];
            g_CPTimeTable[max] = g_CPTimeTable[i];
            g_CPTimeTable[i] = temp_time;
        }
    }
}

static void db_cnpl_initalize_hash_table(void)
{
    db_conn_play_item *p_cnpl_table;
    s32 hash_pos;// Hash pos
    u16 key[MAX_PG_NAME_LEN + 1];
    u16 i;
    u16 phy_pos;// Physical pos

    p_cnpl_table = sys_status_get_conn_play_table();
    memset(g_CPHashTable, 0, sizeof(g_CPHashTable));

    for (i = 0; i < g_conn_play_cnt; i++)
    {
        phy_pos = g_CPTimeTable[i].phy_pos;
        uni_strncpy(key, p_cnpl_table[phy_pos].pg_name, MAX_PG_NAME_LEN);

        hash_pos = Hash(key);
        if (hash_pos >= 0)
        {
            g_CPHashTable[hash_pos].phy_pos = phy_pos;
        }
        else
        {
            MT_ASSERT(0);
        }
    }
}

void db_cnpl_initialize(void)
{
    db_cnpl_sort_time_table();

    db_cnpl_initalize_hash_table();
}

s32 db_cnpl_get_item_by_key(u16 *p_pgName, conn_play_info *p_info)
{
    s32 hash_pos;
    u16 key[MAX_PG_NAME_LEN+1];

    MT_ASSERT(p_pgName != NULL);
    MT_ASSERT(p_info != NULL);

    uni_strncpy(key, p_pgName, MAX_PG_NAME_LEN);
    hash_pos = Hashed(key);
    OS_PRINTF("@@@db_cnpl_get_item_by_key hash_pos=%d, g_conn_play_cnt=%d\n", hash_pos, g_conn_play_cnt);
    if (hash_pos >= 0)
    {
        sys_status_read_conn_play_item(g_CPHashTable[hash_pos].phy_pos, &g_conn_play_item);

        p_info->episode_num = g_conn_play_item.episode_num;
        p_info->play_time = g_conn_play_item.play_time;

        return hash_pos;
    }

    return -1;
}

void db_cnpl_add_item_by_key(u16 *p_pgName, conn_play_info *p_info)
{
    s32 phy_pos;
    s32 hash_pos;
    u16 key[MAX_PG_NAME_LEN+1];

    MT_ASSERT(p_pgName != NULL);
    MT_ASSERT(p_info != NULL);

    if (g_conn_play_cnt >= MAX_CONN_PLAY_CNT)
    {
        db_cnpl_del_item_by_idx(g_conn_play_cnt-1);
    }

    phy_pos = sys_status_find_free_conn_play_item();
    OS_PRINTF("@@@db_cnpl_add_item_by_key phy_pos=%d\n", phy_pos);
    if (phy_pos >= 0)
    {
        uni_strncpy(key, p_pgName, MAX_PG_NAME_LEN);

        g_conn_play_item.is_used = TRUE;
        if (g_conn_play_cnt > 0)
        {
            g_conn_play_item.timestamp = g_CPTimeTable[0].timestamp + 1;
        }
        else
        {
            g_conn_play_item.timestamp = 0;
        }
        g_conn_play_item.episode_num = p_info->episode_num;
        g_conn_play_item.play_time = p_info->play_time;
        memcpy(g_conn_play_item.pg_name, key, MAX_PG_NAME_LEN*sizeof(u16));

        sys_status_write_conn_play_item((u16)phy_pos, &g_conn_play_item);
        sys_status_save();

        if (g_conn_play_cnt > 0)
        {
            memmove(g_CPTimeTable+1, g_CPTimeTable, g_conn_play_cnt*sizeof(db_conn_play_time_t));
        }
        g_CPTimeTable[0].timestamp = g_conn_play_item.timestamp;
        g_CPTimeTable[0].phy_pos = (u16)phy_pos;
        g_conn_play_cnt++;

        hash_pos = Hash(key);
        OS_PRINTF("@@@db_cnpl_add_item_by_key hash_pos=%d\n", phy_pos);
        if (hash_pos >= 0)
        {
            g_CPHashTable[hash_pos].phy_pos = (u16)phy_pos;
        }
        else
        {
            MT_ASSERT(0);
        }
    }
}

void db_cnpl_up_item_by_hash_pos(u16 hash_pos, conn_play_info *p_info)
{
    u16 phy_pos;

    MT_ASSERT(p_info != NULL);

    OS_PRINTF("@@@db_cnpl_up_item_by_hash_pos hash_pos=%d\n", hash_pos);
    if (hash_pos < CONN_PLAY_HASH_TABLE_LEN)
    {
        phy_pos = g_CPHashTable[hash_pos].phy_pos;

        sys_status_read_conn_play_item(phy_pos, &g_conn_play_item);
        g_conn_play_item.episode_num = p_info->episode_num;
        g_conn_play_item.play_time = p_info->play_time;

        sys_status_write_conn_play_item(phy_pos, &g_conn_play_item);
        sys_status_save();
    }
}

void db_cnpl_del_item_by_idx(u16 index)
{
    s32 hash_pos;
    u16 key[MAX_PG_NAME_LEN + 1];
    u16 phy_pos;
    u16 offset_pos;

    OS_PRINTF("@@@db_cnpl_del_item_by_idx index=%d\n", index);
    if (index < g_conn_play_cnt)
    {
        phy_pos = g_CPTimeTable[index].phy_pos;
        OS_PRINTF("@@@db_cnpl_del_item_by_idx phy_pos=%d\n", phy_pos);
        sys_status_read_conn_play_item(phy_pos, &g_conn_play_item);
        uni_strncpy(key, g_conn_play_item.pg_name, MAX_PG_NAME_LEN);
        
        memset(&g_conn_play_item, 0, sizeof(db_conn_play_item));
        sys_status_write_conn_play_item(phy_pos, &g_conn_play_item);
        sys_status_save();

        if (index < g_conn_play_cnt - 1)
        {
            memmove(g_CPTimeTable+index, g_CPTimeTable+index+1, (g_conn_play_cnt-1-index)*sizeof(db_conn_play_time_t));
        }
        memset(g_CPTimeTable+g_conn_play_cnt-1, 0, sizeof(db_conn_play_time_t));
        g_conn_play_cnt--;

        hash_pos = Hashed(key);
        OS_PRINTF("@@@db_cnpl_del_item_by_idx hash_pos=%d\n", hash_pos);
        if (hash_pos >= 0)
        {
            offset_pos = g_CPHashTable[hash_pos].offset_pos;
            memset(g_CPHashTable+hash_pos, 0, sizeof(db_conn_play_hash_node_t));
            g_CPHashTable[hash_pos].offset_pos = offset_pos;
        }
        else
        {
            MT_ASSERT(0);
        }
    }
}

void db_cnpl_del_all(void)
{
    u16 i;

    memset(&g_conn_play_item, 0, sizeof(db_conn_play_item));
    for (i = 0; i < g_conn_play_cnt; i++)
    {
        sys_status_write_conn_play_item(g_CPTimeTable[i].phy_pos, &g_conn_play_item);
    }
    sys_status_save();

    memset(g_CPTimeTable, 0, sizeof(g_CPTimeTable));
    g_conn_play_cnt = 0;
}



