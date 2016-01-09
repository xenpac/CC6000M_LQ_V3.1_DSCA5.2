/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*!
 *Include files
 */
#include "ui_common.h"
#include "lib_char.h"
#include "ui_video.h"
#include "pnp_service.h"

/*!
 * Const value
 */
static char *p_g_name_filter="|mkv|mp4|MKV|MP4|avi|AVI|flv|FLV|f4v|F4V \
                              |ts|TS|m2ts|M2TS|mov|MOV|3gp|3GP|mpg|MPG";

static char *p_g_subt_filter="|ssa|SSA|srt|SRT|nodir";

static u16 p_url[512] = {0};
static u32 g_is_usb = 1;
static u32 g_ip_cur = 0;
static u16 ip_name[32]={0};
static u16 ipaddress_temp[32] ={0};

#define V_SUB_LANG_LEN     20
#define V_SUB_CNT_MAX      20

/*!
 * video subtitle
 */
typedef struct
{
    /*!
     * index to find file path
     */
    u16 idx;

    u16 resv;
    /*!
     * Save subtitle language
     */
    u16 subt_lang[V_SUB_LANG_LEN];
}ui_video_m_subt_t;
/*!
 * video modle struct
 */
typedef struct
{
    /*!
     * U disk partition count
     */
    u16         partition_cnt;
    /*!
     * Current using partition index
     */
    u16         cur_partition;
    /*!
     * Load video in some folder
     */
    file_list_t file_list;
    /*!
     * Load video in some folder
     */
    flist_dir_t flist_dir;
    /*!
     * Load subtitle in current folder
     */
    file_list_t subt_list;
    /*!
     * Load subtitle in current folder
     */
    flist_dir_t subt_dir;

    /*!
     * Save plug-in subtitle language
     */
    ui_video_m_subt_t subt[V_SUB_CNT_MAX];
    /*!
     * cnt with the same name of video
     */
    u16 subt_cnt;
    /*!
     * Current playing file without path
     */
    u16 cur_file[MAX_FILE_PATH];
    /*!
     * cur playing file is unsupport or not
     */
    BOOL cur_support;
    /*!
     * Current playing file with path
     */
    u16 cur_file_path[MAX_FILE_PATH];

    /*!
     * Current playing file idx
     */
    u16 cur_idx;
    /*!
     * Current partition
     */
    partition_t *p_partition;
    /*!
     * Current sub lang focus
     */
    u16 subt_focus;
    /*!
     *  Total time of current playfile
     */
    video_play_time_t time;
    /*!
     *  Resolution of current playfile
     */
    rect_size_t rect;
}ui_video_m_t;
/*!
 * video modle globle variable
 */
static ui_video_m_t g_video_m = {0};

/*=================================================================================================
                        Video modle internel function
=================================================================================================*/
/*!
 * Free dir and list point
 */
static void _ui_video_m_free_dir_and_list(void)
{
    if(NULL != g_video_m.flist_dir)
    {
        file_list_leave_dir(g_video_m.flist_dir);
        g_video_m.flist_dir = NULL;
        memset(&g_video_m.file_list, 0, sizeof(g_video_m.file_list));
    }
}
/*!
 * Load video subtitle file
 */
static void _ui_video_m_free_subt_dir_and_list(void)
{
    if(NULL != g_video_m.subt_dir)
    {
        file_list_leave_dir(g_video_m.subt_dir);

        memset(g_video_m.subt, 0, sizeof(g_video_m.subt));
        memset(&g_video_m.subt_list, 0, sizeof(g_video_m.subt_list));

        g_video_m.subt_dir = NULL;
        g_video_m.subt_cnt = 0;
    }
}
/*!
 * Load file list in some folder
 */
static RET_CODE _ui_video_m_get_file_list(u16 *p_path)
{
    u16 *p_name_filter = NULL;

    /*!
     * Free last time dir and list first
     */
    _ui_video_m_free_dir_and_list();

    p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_name_filter) + 1));

    str_asc2uni((u8 *)p_g_name_filter, p_name_filter);

    g_video_m.flist_dir = file_list_enter_dir(p_name_filter, FILE_LIST_MAX_SUPPORT_CNT, p_path);

    mtos_free(p_name_filter);

    if(NULL == g_video_m.flist_dir)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## file list null\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    file_list_get(g_video_m.flist_dir, FLIST_UNIT_FIRST, &g_video_m.file_list);

    /*!
     * Set default sort as a-z mode
     */
    file_list_sort(&g_video_m.file_list, (int)g_video_m.file_list.file_count, A_Z_MODE);

    return SUCCESS;
}
/*!
 * Get name without ext, WARNING: p_name is input param, it is also output param
 */
static void _ui_video_m_get_name_without_ext(u16 *p_name)
{
    int i = 0;

    for(i = (int)(uni_strlen(p_name) - 1); i >= 0; i--)
    {
        if('.' == p_name[i])
        {
            p_name[i] = 0;
            break;
        }
    }
}
/*!
 * Get ext of file, WARNING: p_name is input param, it is also output param
 */
static void _ui_video_m_get_ext_without_name(u16 *p_name)
{
    u8   i     = 0;
    u16 *p_ext = NULL;

    for(i = 0; i < (uni_strlen(p_name) - 1); i++)
    {
        if('.' == p_name[i])
        {
            p_ext = &p_name[i] + 1;
            uni_strcpy(p_name, p_ext);
            break;
        }
    }
}
static u16 * _ui_video_m_get_name_without_path(u16 *p_path)
{
    u16 *p_name = NULL;
    int  i      = 0;

    for(i = (int)(uni_strlen(p_path) - 1); i >= 0; i--)
    {
        if(0x5c == p_path[i])  //"\"
        {
            p_name = p_path + i + 1;
            break;
        }
    }

    return p_name;
}

/*!
 * Save plug-in subtitle with the same name of current playing file
 */
static void _ui_video_m_save_subt_lang(u16 *p_path)
{
    u8   i          = 0;
    u8   j          = 0;
    u8   cnt        = 0;
    u16 *p_temp     = NULL;
    u16 *p_name     = NULL;
    u16 *p_subt_tmp = NULL;

    if(NULL == p_path)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## file name null\n", __FUNCTION__);
        return;
    }

    cnt = (u8)MIN(g_video_m.subt_list.file_count, V_SUB_CNT_MAX);

    p_name = _ui_video_m_get_name_without_path(p_path);

    p_temp = mtos_malloc(sizeof(u16) * (uni_strlen(p_name) + 1));

    uni_strcpy(p_temp, p_name);

    _ui_video_m_get_name_without_ext(p_temp);

    p_subt_tmp = mtos_malloc(sizeof(u16) * MAX_FILE_PATH);

    for(i = 0; i < cnt; i++)
    {
        uni_strncpy(p_subt_tmp, g_video_m.subt_list.p_file[i].p_name, MAX_FILE_PATH - 1);

        if(0 == uni_strncmp(p_subt_tmp, p_temp, uni_strlen(p_temp)))
        {
            /*!
             * Ignore ext, Eg "test.chg.ssa->test.chg" or "test123.ssa" -> "test123"
             */
            _ui_video_m_get_name_without_ext(p_subt_tmp);

            /*!
             * If string is "test.chg", only save "chg"
             */
            _ui_video_m_get_ext_without_name(p_subt_tmp);

            uni_strncpy(g_video_m.subt[j].subt_lang, p_subt_tmp, V_SUB_LANG_LEN - 1);

            /*!
             * Save the index for finding name
             */
            g_video_m.subt[j++].idx = i;
        }
    }

    mtos_free(p_temp);
    mtos_free(p_subt_tmp);

    g_video_m.subt_cnt = j;

    UI_VIDEO_PRINF("[%s]: total cnt:%d name:%s sub cnt:%d\n", __FUNCTION__, cnt, p_name, j);
}
/*=================================================================================================
                        Video modle interface function
=================================================================================================*/
void ui_video_m_reset(void)
{
    _ui_video_m_free_dir_and_list();
    _ui_video_m_free_subt_dir_and_list();

    memset(&g_video_m, 0, sizeof(g_video_m));
}
/*!
 * Load video file by different partition
 */
RET_CODE ui_video_m_load(u32 partition)
{
    RET_CODE ret = SUCCESS;
    
    /*!
     * g_video_m.p_partition is array, needn't be free
     */
    g_video_m.partition_cnt = (u16)file_list_get_partition(&g_video_m.p_partition);

    UI_VIDEO_PRINF("[%s]: partition id = %d cnt = %d\n",
                                         __FUNCTION__,
                                         partition,
                                         g_video_m.partition_cnt);

    g_video_m.cur_partition = (u16)partition;

    ret = _ui_video_m_get_file_list(g_video_m.p_partition[partition].letter);
	
    memcpy((void *)ipaddress_temp, (void *)g_video_m.p_partition[partition].name, 26);
    return ret;
}

RET_CODE ui_video_m_net_load(u16*ip_address_path)
{
    RET_CODE ret = SUCCESS;
    
     ret = _ui_video_m_get_file_list(ip_address_path);
    return ret;
}

RET_CODE ui_video_m_get_ipname(u16*ip_allname)
{
   memcpy(ip_allname,ip_name,sizeof(ip_name));
  return SUCCESS;
}

RET_CODE ui_video_m_get_ipaddr(u16*ip_address_path)
{
   memcpy(ip_address_path,ipaddress_temp,sizeof(ipaddress_temp));
  return SUCCESS;
}

u32 ui_video_m_get_is_usb(void)
{
  return  g_is_usb;
}

void ui_video_m_set_is_usb(u32 is_usb)
{
   g_is_usb = is_usb;
}

u32 ui_video_m_get_ip_cur(void)
{
  return g_ip_cur;
}

void ui_video_m_set_ip_cur(u32 ip_cur)
{
   g_ip_cur = ip_cur;
}

BOOL ui_get_ip_service_with_ippath(char *ipaddress,char *service)
{

  u16 ip_address_with_path_temp[32] ={0};
  u16 count = 0;
  u16 service_temp[32] ={0};

  memcpy(ip_address_with_path_temp,ip_name,64);
  count = 2;
  while(count < 32)
  {
    if((ip_address_with_path_temp[count] != '\\') )
    {
      count ++;
    }
    else
    {
      ip_address_with_path_temp[count] ='\0';
      break;
    }
  }
  if(count !=32)
  {
    memcpy(service_temp,&ip_address_with_path_temp[count+1],(32 - count - 1)*2);
    memcpy(ipaddress_temp,&ip_address_with_path_temp[2],(count-1) *2);
    str_uni2asc( (u8 *)ipaddress, (u16 *)ipaddress_temp);
    str_uni2asc((u8 *)service, (u16 *)service_temp);
    return TRUE;
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 0);
    return FALSE;
  }
}

RET_CODE ui_video_m_net_open_mount(void)
{
  /*
  u32 ip_count = 0;
 
  ip_count = sys_status_get_ip_path_cnt();
  memset(ip_name,0,64);
  g_ip_cur = 0;
  if(ip_count > 0)
    {
    g_is_usb = 0;
    ui_video_m_net_mount();
    g_ip_cur ++;
  
  }
  else
  { 
      g_is_usb = 1;
      g_ip_cur = 0;
        return ERR_FAILURE;
    }
    */
   return SUCCESS;  
}

RET_CODE ui_video_m_net_partition_mount(void)
{
  /*
  u32 ip_count = 0;
 
  ip_count = sys_status_get_ip_path_cnt();
  memset(ip_name,0,64);
  if(g_ip_cur < ip_count ) 
    {
    ui_video_m_net_mount();
    g_ip_cur ++;
    }
*/
   return SUCCESS;  
}

RET_CODE ui_video_m_net_mount(void)
{
  /*
  BOOL is_ip = FALSE;
  char ipaddress[128] = {0};
  char service[64] = {0,};
  char mount_path[128]={0};
  char username[256];
  char password[256];
  u16 usr_account[128] = {0} ,usr_passwd[64] = {0};

  sys_status_get_ip_path_set(g_ip_cur, ip_name);
  sys_status_get_ip_username_pwd_set(g_ip_cur,usr_account,usr_passwd);
  str_uni2asc(username, usr_account);
  str_uni2asc(password, usr_passwd);
  is_ip =  ui_get_ip_service_with_ippath(ipaddress,service);
  if(is_ip == TRUE)  
  {
   #ifndef WIN32
    sprintf(mount_path, "smb://%s:%s@%s/%s", username, password, ipaddress, service);
    str_asc2uni(mount_path, p_url);

    pnp_service_vfs_mount(p_url);
   #endif
  }
  */
  return SUCCESS;  
}


RET_CODE ui_video_m_net_unmount(void)
{
    #ifndef WIN32
    pnp_service_vfs_unmount(p_url);
    #endif
    return SUCCESS;   
}
/*!
 * Load video subtitle file
 */
RET_CODE ui_video_m_load_subt(u16 *p_name)
{
    u16 *p_cur_path = NULL;
    u16 *p_filter   = NULL;

    if(NULL == p_name)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## cur name null\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    /*!
     * Subtitle files in current playing video path will be loaded
     */
    p_cur_path = ui_video_m_get_cur_path();

    if(NULL == p_cur_path)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## cur path null\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    UI_VIDEO_PRINF("[%s]: p_cur_path = %s\n", __FUNCTION__, p_cur_path);

    /*!
     * Free dir and list first
     */
    _ui_video_m_free_subt_dir_and_list();

    p_filter = mtos_malloc(sizeof(u16) * (strlen(p_g_subt_filter) + 1));

    str_asc2uni((u8 *)p_g_subt_filter, p_filter);

    g_video_m.subt_dir = file_list_enter_dir(p_filter, FILE_LIST_MAX_SUPPORT_CNT, p_cur_path);

    mtos_free(p_filter);

    if(NULL == g_video_m.subt_dir)
    {
        return ERR_FAILURE;
    }

    file_list_get(g_video_m.subt_dir, FLIST_UNIT_FIRST, &g_video_m.subt_list);

    file_list_sort(&g_video_m.subt_list, (int)g_video_m.subt_list.file_count, A_Z_MODE);

    /*!
     * Save subtitle by playing file name
     */
    _ui_video_m_save_subt_lang(p_name);

    return SUCCESS;
}

/*!
 * Get subtitle in current dir
 */
u16 ui_video_m_get_subt_cnt(void)
{
    u16 cnt = 0;

    cnt = MIN(V_SUB_CNT_MAX, g_video_m.subt_cnt);

    return cnt;
}
/*!
 * Get subtitle path
 */
u16 * ui_video_m_get_cur_subt_path(u16 idx)
{
    u16 file_idx = 0;

    if(idx >= ui_video_m_get_subt_cnt())
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than sub count\n", __FUNCTION__);
        return NULL;
    }

    file_idx = g_video_m.subt[idx].idx;

    if(file_idx >= g_video_m.subt_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: BAD PARAM\n", __FUNCTION__);
        return NULL;
    }

    return g_video_m.subt_list.p_file[file_idx].name;
}
/*!
 * Get subtitle by index
 */
void ui_video_m_get_subt_by_idx(u16 idx, u16 *p_file_name, u16 str_len)
{
    if(idx >= ui_video_m_get_subt_cnt())
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than sub count\n", __FUNCTION__);
        return;
    }

    UI_VIDEO_PRINF("[%s]: subtitle: %s\n", __FUNCTION__, g_video_m.subt[idx].subt_lang);

    uni_strncpy(p_file_name, g_video_m.subt[idx].subt_lang, str_len - 1);
}
/*!
 * Load video file path
 */
u16 * ui_video_m_get_cur_path(void)
{
    u16 *p_cur_path = NULL;

    if(NULL == g_video_m.flist_dir)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## dir null\n", __FUNCTION__);
        return NULL;
    }

    p_cur_path = file_list_get_cur_path(g_video_m.flist_dir);

    UI_VIDEO_PRINF("[%s]: cur_path:%s\n", __FUNCTION__, p_cur_path);

    return p_cur_path;
}
/*!
 * Back to parent directory
 */
RET_CODE ui_video_m_back_to_parent_dir(void)
{
    file_type_t type = UNKNOW_FILE;

    if(0 == g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## file cnt 0\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    /*!
     * Check first file is ".." or not, only ".." can back.
     */
    type = ui_video_m_get_type_by_idx(0);

    if(DIRECTORY != type)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## not directory\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    _ui_video_m_get_file_list(g_video_m.file_list.p_file[0].name);

    return SUCCESS;
}
/*!
 * Go to child directory
 */
RET_CODE ui_video_m_go_to_dir(u16 idx)
{
    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    _ui_video_m_get_file_list(g_video_m.file_list.p_file[idx].name);

    return SUCCESS;
}
/*!
 * is enter directory
 */
BOOL ui_video_file_is_enter_dir(void)
{
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};
  u16 cur_path[255] = {0};

  if(g_video_m.flist_dir != NULL && g_video_m.file_list.file_count != 0)
  {
    uni_strcpy(cur_path, g_video_m.file_list.p_file[0].name);

    p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);

    if (p_temp != NULL)
    {
      //parent dir
      str_asc2uni((u8 *)"..", (u16 *)parent);
      str_asc2uni((u8 *)".", (u16 *)curn);
      if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
      {
        p_temp[0] = 0/*'\0'*/;
        p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);
        if (p_temp != NULL)
        {
          p_temp[0] = 0/*'\0'*/;
        }
      }
      //cur dir
      else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
      {
        p_temp[0] = 0/*'\0'*/;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}
/*!
 * Get total video file and dictionary
 */
u32 ui_video_m_get_total(void)
{
    return g_video_m.file_list.file_count;
}
/*!
 * Get U disk partition count
 */
u16 ui_video_m_get_partition_cnt(void)
{
    return g_video_m.partition_cnt;
}
/*!
 * Set U disk partition count
 */
void ui_video_m_set_partition_cnt(u16 cnt)
{
    g_video_m.partition_cnt = cnt;
}
/*!
 * Get U disk cur partition name
 */
u16 * ui_video_m_get_partition_name(u16 focus)
{
    return g_video_m.p_partition[focus].name;
}

/*!
 * Get video name by index
 */
void ui_video_m_get_name_by_idx(u32 idx, u16 *p_file_name, u16 str_len)
{
    u16 * p_name = NULL;

    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return;
    }

    p_name = g_video_m.file_list.p_file[idx].p_name;

    if(NULL == p_name)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## name null\n", __FUNCTION__);
        return;
    }

    uni_strncpy(p_file_name, p_name, str_len - 1);
}
/*!
 * Get video name by index
 */
u16 * ui_video_m_get_name_ptr_by_idx(u32 idx)
{
    u16 * p_name = NULL;

    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return NULL;
    }

    p_name = g_video_m.file_list.p_file[idx].p_name;

    return p_name;
}
/*!
 * Get video file size by index
 */
void ui_video_m_get_size_by_idx(u32 idx, u32 *p_size)
{
    u64 size = 0;

    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return;
    }

    size = file_list_get_file_size(g_video_m.file_list.p_file[idx].name);

    *p_size = (u32)(size / 1024);

    /*!
     * Use k size
     */
    UI_VIDEO_PRINF("[%s]: size = %ld,movie size = %d\n", __FUNCTION__, size, *p_size);
}

void ui_video_m_set_cur_playing_file_by_idx(u32 idx)
{
    u16 len = 0;
    u32 str_len = 0;
    
    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return;
    }

    memset(g_video_m.cur_file, 0, sizeof(g_video_m.cur_file));
    memset(g_video_m.cur_file_path, 0, sizeof(g_video_m.cur_file_path));

    /*!
     * Copy current file without path
     */
    str_len = uni_strlen(g_video_m.file_list.p_file[idx].p_name);
    len = (u16)MIN(MAX_FILE_PATH, str_len);

    uni_strncpy(g_video_m.cur_file, g_video_m.file_list.p_file[idx].p_name, len);

    /*!
     * Copy current file with path
     */
    str_len = uni_strlen(g_video_m.file_list.p_file[idx].name);
    len = (u16)MIN(MAX_FILE_PATH, str_len);

    uni_strncpy(g_video_m.cur_file_path, g_video_m.file_list.p_file[idx].name, len);

    g_video_m.cur_idx = (u16)idx;

    UI_VIDEO_PRINF("[%s]: current play file idx:%d\n", __FUNCTION__, idx);
}

void ui_video_m_set_support_flag_by_idx(u32 idx, BOOL support)
{
    g_video_m.cur_support = support;
}

u16 *ui_video_m_get_cur_playing_file(void)
{
    return g_video_m.cur_file;
}
BOOL ui_video_m_check_cur_playing_file_support(void)
{
    return g_video_m.cur_support;
}
u16 *ui_video_m_get_cur_playing_file_path(void)
{
    return g_video_m.cur_file_path;
}


u16 ui_video_m_get_cur_playing_idx(void)
{
    return g_video_m.cur_idx;
}

RET_CODE ui_video_m_get_next_file_idx(u16 *p_idx)
{
    u32 i = 0,j = 0;

    /*!
     * From current file to end
     */
    for(i = g_video_m.cur_idx; i < g_video_m.file_list.file_count && j <= 2; )
    {
        if(g_video_m.file_list.file_count == (i + 1))
        {
            i = 0;
            j ++;
        }
        else
        {
            i= i + 1;
        }
        if(NOT_DIR_FILE == ui_video_m_get_type_by_idx(i))
        {
            *p_idx = (u16)i;
            return SUCCESS;
        }
    }

    UI_VIDEO_PRINF("[%s]: no next file\n", __FUNCTION__);

    return ERR_FAILURE;
}

RET_CODE ui_video_m_get_prev_file_idx(u16 *p_idx)
{
    s32 i = 0;
      
    /*!
     * i maybe < 0
     */
    i = (s32)g_video_m.cur_idx;

    /*!
     * From current file to begin
     */
    for(i = (s32)g_video_m.cur_idx; i >= 0; )
    {
        if(0 == i)
        {
            i = (s32)(g_video_m.file_list.file_count - 1);
        }
        else
        {
            i= i - 1;
        }
        if(NOT_DIR_FILE == ui_video_m_get_type_by_idx((u32)i))
        {
            *p_idx = (u16)i;
            return SUCCESS;
        }
    }

    UI_VIDEO_PRINF("[%s]: no previous file\n", __FUNCTION__);

    return ERR_FAILURE;
}
/*!
 * Get video path by index
 */
u16 * ui_video_m_get_path_by_idx(u32 idx)
{
    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return NULL;
    }

    return g_video_m.file_list.p_file[idx].name;
}
/*!
 * Get video type by index
 */
file_type_t ui_video_m_get_type_by_idx(u32 idx)
{
    if(idx >= g_video_m.file_list.file_count)
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## more than file count\n", __FUNCTION__);
        return UNKNOW_FILE;
    }

    return g_video_m.file_list.p_file[idx].type;
}

/*!
 * Get video type by index
 */
u16 ui_video_m_get_cur_partition(void)
{
    return g_video_m.cur_partition;
}
/*!
 * Get subtitle focus
 */
u16 ui_video_m_get_subt_focus(void)
{
    return g_video_m.subt_focus;
}

/*!
 * Set subtitle focus
 */
void ui_video_m_set_subt_focus(u16 focus)
{
   g_video_m.subt_focus = focus;
}

u16 ui_video_m_get_video_icon(u16 *p_filename)
{
    u16  id    = IM_M_ICON_AVI;
    u16 *p_tip = NULL;
    u8  *p_ext = NULL;

    if(NULL == p_filename)
    {
        return id;
    }

    p_tip = uni_strrchr(p_filename, char_asc2uni('.'));

    if(NULL == p_tip)
    {
        return id;
    }

    p_ext = (u8 *)mtos_malloc(uni_strlen(p_tip) + 1);

    str_uni2asc(p_ext, p_tip);

    if(STRCMPI((void *)p_ext, ".AVI") == 0)
    {
        id = IM_M_ICON_AVI;
    }
    else if(STRCMPI((void *)p_ext, ".MKV") == 0)
    {
        id = IM_M_ICON_MKV;
    }
    else if(STRCMPI((void *)p_ext, ".MP4") == 0)
    {
        id = IM_M_ICON_MP4;
    }
    else if(STRCMPI((void *)p_ext, ".M2TS") == 0)
    {
        id = IM_M_ICON_M2TS;
    }
    else if(STRCMPI((void *)p_ext, ".F4V") == 0)
    {
        id = IM_M_ICON_F4V;
    }  
    else if(STRCMPI((void *)p_ext, ".3GP") == 0)
    {
        id = IM_M_ICON_3GP;
    }  
    else if(STRCMPI((void *)p_ext, ".MOV") == 0)
    {
        id = IM_M_ICON_MOV;
    }  
    else if(STRCMPI((void *)p_ext, ".FLV") == 0)
    {
        id = IM_M_ICON_FLV;
    }  
    else if(STRCMPI((void *)p_ext, ".TS") == 0)
    {
        id = IM_M_ICON_TS;
    }  
    mtos_free(p_ext);

    return id;
}
/*!
 * When enter directory or change partition, it need check current playing file is in directory
 * or partition, if in, it should highlight it.
 */
RET_CODE ui_video_m_find_idx_by_cur_play_file(u32 *p_idx)
{
    u32 i          = 0;
    u32 cnt        = 0;
    u16 *p_path     = NULL;
    u16 *p_cur_file = NULL;

    p_cur_file = ui_video_m_get_cur_playing_file_path();

    if((NULL == p_cur_file) || (NULL == p_idx))
    {
        UI_VIDEO_PRINF("[%s]: ##ERR## current file null\n", __FUNCTION__);
        return ERR_FAILURE;
    }

    UI_VIDEO_PRINF("[%s]: current playing file path = %s\n", __FUNCTION__, p_cur_file);

    cnt = ui_video_m_get_total();

    for(i = 0; i < cnt; i++)
    {
        p_path = ui_video_m_get_path_by_idx(i);

        if(NULL != p_path)
        {
            UI_VIDEO_PRINF("[%s]: path[%d] = %s\n", __FUNCTION__, i, p_path);
        }

        if((NULL != p_path) && (0 == uni_strncmp(p_cur_file,p_path, uni_strlen(p_cur_file))))
        {
            *p_idx = i;
            return SUCCESS;
        }
    }

    return ERR_FAILURE;
}

/*!
 * Save total time of video
 */
void ui_video_m_save_total_time(video_play_time_t *play_time)
{
   g_video_m.time.hour = play_time->hour;
   g_video_m.time.min  = play_time->min;
   g_video_m.time.sec  = play_time->sec;
}

/*!
 * Get saved total time of video
 */
void ui_video_m_get_saved_total_time(video_play_time_t *play_time)
{
   play_time->hour = g_video_m.time.hour;
   play_time->min  = g_video_m.time.min;
   play_time->sec  = g_video_m.time.sec;
}

/*!
 * Save resolution of video
 */
void ui_video_m_save_resolution(rect_size_t *rect)
{
    g_video_m.rect.w = rect->w;
    g_video_m.rect.h = rect->h;
}


/*!
 * Get saved resolution of video
 */
void ui_video_m_get_saved_resolution(rect_size_t *rect)
{
    rect->w = g_video_m.rect.w;
    rect->h = g_video_m.rect.h;
}

