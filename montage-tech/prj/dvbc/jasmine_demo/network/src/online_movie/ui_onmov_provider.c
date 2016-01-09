/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"

#if ENABLE_NETWORK
#include "ui_onmov_provider.h"

static onmov_website_item *g_pWlist = NULL;
static u16 g_wlist_cnt = 0;
static s16 g_wlist_index = -1;
static s16 g_catelist_index = -1;
static s16 g_movlist_index = -1;


static void *freadline(hfile_t file)
{
  u16 wc;
  u32 read_size = 0;
  u16 line_len = 0, stop_mark_count = 0;
  u16 *p_line = NULL;
  u16 str_buf[1024] = {0};

  while (TRUE)
  {
    read_size = vfs_read((void *)&wc, 1, sizeof(u16), file);

    if (read_size != sizeof(u16))
    {
      break;
    }

    if (wc != '\r' && wc != '\n')
    {
      str_buf[line_len] = wc;
      line_len++;
    }
    else if (wc == '\r')
    {
      str_buf[line_len] = wc;
      line_len++;
      stop_mark_count++;
    }
    else if (wc == '\n')
    {
      str_buf[line_len] = wc;
      line_len++;
      stop_mark_count++;
      break;
    }
  }

  if (line_len > 0)
  {
    u16 len = line_len - stop_mark_count;
    UI_PRINTF("@@@freadline() line_len=%d, stop_mark_count=%d\n", line_len, stop_mark_count);
    p_line = (u16 *) mtos_malloc((len + 1) * sizeof(u16));
    memset(p_line, 0, (len + 1) * sizeof(u16));

    uni_strncpy(p_line, str_buf, len);
    //vfs_seek(file, -(line_len*sizeof(u16)), VFS_SEEK_CUR);
    //vfs_read((void *)p_line, 1, line_len*sizeof(u16), file);

    //p_line[line_len - stop_mark_count] = '\0';
  }

  return (void *) p_line;
}

static void parse(u16 *p_line)
{
    u16 *p, *q;
    u16 mode = 0;
    u8 num[6] = {0};
    char *pstr;
    u16 type = 0, count = 0;
    onmov_cate_item *p_cate = NULL;
    onmov_movie_item *p_movie = NULL;

    p = q = p_line;

    while (TRUE)
    {
        while (*p != '\t' && *p != '\0')
        {
            p++;
        }

        switch (mode)
        {
            case 0://type
                memset(num, 0, 6);
                str_nuni2asc(num, q, p-q);
                type = (u16) strtol(num, &pstr, 10);

                switch (type)
                {
                    case ONMOV_TAB_TYPE_WEBSITE:
                        if (g_wlist_index + 1 >= g_wlist_cnt)
                        {
                            return;
                        }
                        g_wlist_index++;
                        break;
                    case ONMOV_TAB_TYPE_CATEGORY:
                        if (g_catelist_index + 1 >= g_pWlist[g_wlist_index].cate_count)
                        {
                            return;
                        }
                        g_catelist_index++;
                        p_cate = g_pWlist[g_wlist_index].p_cate;
                        break;
                    case ONMOV_TAB_TYPE_MOVIE:
                        p_cate = g_pWlist[g_wlist_index].p_cate;
                        if (g_movlist_index + 1 >= p_cate[g_catelist_index].movie_count)
                        {
                            return;
                        }
                        g_movlist_index++;
                        p_movie = p_cate[g_catelist_index].p_movie;
                        break;
                }
                mode = 1;
                break;
            case 1://name
                switch (type)
                {
                    case ONMOV_TAB_TYPE_WEBSITE:
                        uni_strncpy(g_pWlist[g_wlist_index].name, q, p-q);
                        break;
                    case ONMOV_TAB_TYPE_CATEGORY:
                        uni_strncpy(p_cate[g_catelist_index].name, q, p-q);
                        break;
                    case ONMOV_TAB_TYPE_MOVIE:
                        uni_strncpy(p_movie[g_movlist_index].name, q, p-q);
                        break;
                }
                mode = 2;
                break;
            case 2://picture
                switch (type)
                {
                    case ONMOV_TAB_TYPE_WEBSITE:
                        str_nuni2asc(g_pWlist[g_wlist_index].picture, q, p-q);
                        //uni_strncpy(g_pWlist[g_wlist_index].picture, q, p-q);
                        break;
                    case ONMOV_TAB_TYPE_MOVIE:
                        str_nuni2asc(p_movie[g_movlist_index].picture, q, p-q);
                        //uni_strncpy(p_movie[g_movlist_index].picture, q, p-q);
                        break;
                }
                mode = 3;
                break;
            case 3://director
                switch (type)
                {
                    case ONMOV_TAB_TYPE_MOVIE:
                        uni_strncpy(p_movie[g_movlist_index].director, q, p-q);
                        break;
                }
                mode = 4;
                break;
            case 4://actor
                switch (type)
                {
                    case ONMOV_TAB_TYPE_MOVIE:
                        uni_strncpy(p_movie[g_movlist_index].actor, q, p-q);
                        break;
                }
                mode = 5;
                break;
            case 5://description
                switch (type)
                {
                    case ONMOV_TAB_TYPE_MOVIE:
                        uni_strncpy(p_movie[g_movlist_index].description, q, p-q);
                        break;
                }
                mode = 6;
                break;
            case 6://url
                switch (type)
                {
                    case ONMOV_TAB_TYPE_MOVIE:
                        uni_strncpy(p_movie[g_movlist_index].url, q, p-q);
                        break;
                }
                mode = 7;
                break;
            case 7://count
                memset(num, 0, 6);
                str_nuni2asc(num, q, p-q);
                count = (u16) strtol(num, &pstr, 10);

                switch (type)
                {
                    case ONMOV_TAB_TYPE_TAB:
                        if (g_pWlist)
                        {
                            return;
                        }
                        g_pWlist = mtos_malloc(count * sizeof(onmov_website_item));
                        memset(g_pWlist, 0, count * sizeof(onmov_website_item));
                        g_wlist_cnt = count;
                        g_wlist_index = -1;
                        break;
                    case ONMOV_TAB_TYPE_WEBSITE:
                        g_pWlist[g_wlist_index].p_cate = mtos_malloc(count * sizeof(onmov_cate_item));
                        memset(g_pWlist[g_wlist_index].p_cate, 0, count * sizeof(onmov_cate_item));
                        g_pWlist[g_wlist_index].cate_count = count;
                        g_catelist_index = -1;
						break;
                    case ONMOV_TAB_TYPE_CATEGORY:
                        p_cate[g_catelist_index].p_movie = mtos_malloc(count * sizeof(onmov_movie_item));
                        memset(p_cate[g_catelist_index].p_movie, 0, count * sizeof(onmov_movie_item));
                        p_cate[g_catelist_index].movie_count = count;
                        g_movlist_index = -1;
                        break;
                    case ONMOV_TAB_TYPE_MOVIE:
                        p_movie[g_movlist_index].count = count;
                        break;
                }
                return;
        }
        q = ++p;
    }
}

BOOL ui_load_onmov_file(void)
{
  partition_t *p_partition = NULL;
  u32 partition_cnt = 0;
  u16 file_name[MAX_FILE_PATH] = {0};
  hfile_t file = NULL;
  u8 identify[3] = {0};
  u16 *p_line = NULL;

  partition_cnt = file_list_get_partition(&p_partition);
  if (partition_cnt > 0)
  {
    file_name[0] = p_partition[0].letter[0];
    str_asc2uni(":\\onmov.txt", file_name + 1);
  }
#ifdef WIN32
	str_asc2uni("C:\\onmov.txt", file_name);
#endif

  file = vfs_open(file_name, VFS_READ);
  if(file == NULL)
  {
    return FALSE;
  }

  vfs_read((void *)identify, 1, 2, file);


  if (strcmp(identify, "\xFF\xFE") && strcmp(identify, "\xFE\xFF"))
  {
    vfs_close(file);
    return FALSE;
  }

  p_line = freadline(file);
  if (p_line != NULL)
  {
    mtos_free(p_line);
    p_line = NULL;
  }

  while (TRUE)
  {
    p_line = freadline(file);
    if (p_line == NULL)
    {
      break;
    }

    parse(p_line);

    mtos_free(p_line);
    p_line = NULL;
  }

  vfs_close(file);
  return TRUE;
}

BOOL ui_release_onmov_data(void)
{
    u16 i, j;

    
    for (i = 0; i < g_wlist_cnt; i++)
    {
        for (j = 0; j < g_pWlist[i].cate_count; j++)
        {
            if (g_pWlist[i].p_cate[j].p_movie)
            {
                mtos_free(g_pWlist[i].p_cate[j].p_movie);
                g_pWlist[i].p_cate[j].p_movie = NULL;
            }
        }
        
        if (g_pWlist[i].p_cate)
        {
            mtos_free(g_pWlist[i].p_cate);
            g_pWlist[i].p_cate = NULL;
        }
    }

    if (g_pWlist)
    {
        mtos_free(g_pWlist);
        g_pWlist = NULL;
    }

    return TRUE;
}

onmov_website_item *omp_get_websites(void)
{
  return (onmov_website_item *)g_pWlist;
}

u16 omp_get_websites_cnt(void)
{
  return g_wlist_cnt;
}



#if 0

u16 onmov_get_catelist_sel_idx(void)
{
  return (u16) g_catelist_index;
}

u16 onmov_get_movlist_sel_idx(void)
{
  return (u16) g_movlist_index;
}

BOOL onmov_get_cur_movie(onmov_movie_item *item)
{
  if (item != NULL)
  {
    *item =  p_catelist[g_catelist_index].p_movie[g_movlist_index];
    return TRUE;
  }

  return FALSE;
}
#endif

#endif
