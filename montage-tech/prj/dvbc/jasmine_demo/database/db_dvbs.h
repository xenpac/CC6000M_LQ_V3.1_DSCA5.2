/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/******************************************************************************/
/******************************************************************************/


#ifndef __DB_DVBS_H_
#define __DB_DVBS_H_

#ifdef FAST_LOGO    //in fastlog , only need DB_DVBS_MAX_SAT
#define DB_DVBS_MAX_SAT              64
#else
/*!
 \file db_dvbs.h

   This file defined data structures for Satellite, TP and Program management.
   And interfaces for high level modules and some middle-ware modules. Module
   DB is depend on OS, flash driver.

   Development policy:
   Modules who will call DB's interfaces should try it best to do data
   modifcation in memory first and reduce calling function \e DB_DVBS_WriteData
   times. This policy can protect flash and software will get more efficiency.
 */
//lint -e601 -e10 -e148 -e38 -e129 -e49  -e101 -e132 -e578  -e745 -e939 -e752 
#define INVALIDID                    DB_UNKNOWN_ID
#define INVALIDPOS                   DB_UNKNOWN_ID

/*!
   Max number of programs in flash and view depth of available tp node
 */
#define DB_DVBS_MAX_PRO              1024

/*!
   view depth of available tp node and Max number of TP node in Flash.
 */
#define DB_DVBS_MAX_TP               3000

/*!
   view depth of available SAT node and Max number of SAT node in Flash.
 */
#define DB_DVBS_MAX_SAT              64    //for fastlogo , use 64 directly in sys_status.h , attention

/*!
   name length for Satellite and program
 */
 #if 1//#ifdef CONFIG_CUSTOMER
#define DB_DVBS_MAX_NAME_LENGTH      24
#else
#define DB_DVBS_MAX_NAME_LENGTH      16
#endif

#define DB_DVBS_MAX_AUDIO_CHANNEL   (16)

#define MAX_FAV_GRP                 (32)

#define DB_DVBS_PARAM_ACTIVE_FLAG   (0x8000)

#define MAX_VIEW_BUFFER_SIZE        (sizeof(item_type_t) * DB_DVBS_MAX_PRO)//(200*KBYTES)//
/*!
  Length of "no name"
  */
#define LEN_OF_NO_NAME              (10)

#define SVC_TYPE_TRESERVED		       (0x00)
#define SVC_TYPE_TV                    (0x01)
#define SVC_TYPE_RADIO                 (0x02)
#define SVC_TYPE_TELETEXT              (0x03)
#define SVC_TYPE_NVOD_REFRENCE         (0x04)
#define SVC_TYPE_NVOD_TIMESHIFT_EVENT  (0x05)
#define SVC_TYPE_MOSAIC                (0x06)
#define SVC_TYPE_SIGNALPAL             (0x07)
#define SVC_TYPE_SIGNALSECAM           (0x08)
#define SVC_TYPE_DD2MAC                (0x09)
#define SVC_TYPE_FMRADIO               (0x0A)
#define SVC_TYPE_SIGNALNTSC            (0x0B)
#define SVC_TYPE_DATA                  (0x0C)
#define SVC_TYPE_MHP                   (0x10)
#define SVC_TYPE_RESERVED1		       (0x80)
#define SVC_TYPE_RESERVED2		       (0x81)
#define SVC_TYPE_NANJINGGAME		   (0x90)

/*!
   Module return types
 */
typedef enum
{
  /*!
     DB error code. Means operation successed.
   */
  DB_DVBS_OK = 0,
  /*!
     DB error code. Means operation failed.
   */
  DB_DVBS_FAILED,
  /*!
     DB error code. Means database was full or already create 3 views.
   */
  DB_DVBS_FULL,
  /*!
     DB error code. Means the given data was duplicated with the data already
     existing in database.
   */
  DB_DVBS_DUPLICATED,
  /*!
     DB error code. Means database can't get a valid node from the given index.
   */
  DB_DVBS_NOT_FOUND
} db_dvbs_ret_t;


typedef enum
{
 /*! mark bit declares locked.*/
 DB_DVBS_MARK_SEL = 0,
 DB_DVBS_MARK_DEL = 1
}dvbs_view_mark_digit;



/*!
 * satellite struct
 */
typedef struct
{
  u32 id                  : 16;
  /*! 22K option on/off, 0: off, 1: on */
  u32 k22                 : 1;
  /*! 0v12v option on/off, 0: off, 1: on */
  u32 v12                 : 1;
  /*! DiSEqC type, 0: off 1: 1/4, 2: 1/8, 3: 1/16 */
  u32 diseqc_type         : 2;
  /*! DiSEqC port */
  u32 diseqc_port         : 4;
  /*! Motor position */
  u32 position            : 8;

  /*! DiSEqC1.1 type, 0: off 1: mode 1, 2: mode 2, 3: mode 3 */
  u32 diseqc11_type       : 2;
  /*! DiSEqC1.1 port */
  u32 diseqc11_port       : 4;
  /* 0 no positioner 1 DiSEqC1.2 positioner support 2 USALS positioner support*/
  u32 positioner_type     : 2;
  /*! LNB power option, 0: 13/18v 1: 13v 2: 18v */
  u32 lnb_power           : 2;
  /*! LNB types, 0: Standard, 1: user, 2: universal */
  u32 lnb_type            : 2;
  /*! polarity types, 0:auto 1: vertical, 2: horizontal */
  u32 is_tkgs_sat         : 1;//TKGS_SUPPORT
  //u32 polarity            : 2;
  /*! toneburst 0: off, 1: A, 2: B */
  u32 toneburst           : 2;
  /*! longitude of sat */
  u32 longitude           : 16;
  /*! reserved for alignment */
  u32 reserved            : 1;

  /*! */
  u32 lnb_high            : 16;
  /*! */
  u32 lnb_low             : 16;

  /*
  ---------------------------------------------------------
  above into data cache
   */
   /*! IF_channel of unicable */
  u32 user_band           : 4;
  /*! Center freq of unicable */
  u32 band_freq           : 12;

   /*!  or unicable. 0: not unicable ; 1: unicable */
  u32 p_is_unicable       : 1;

  /*! unicable type. 0: unicable A; 1: unicable B */
  u32 unicable_type       : 1;

  /* motor type  0: None, 1: DisEqC1.2, 2: DisEqC1.3*/
  u32 motor_type          : 2;

    /*Motor location change */
  /* 0: position not location change , 1: on */
  u32 position_change     : 1;

   u32 tuner_index           :1;
  /*! reserved  */
  u32 band_reserved       : 11;
 

  /*! Satellite name by unicode*/
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1];
} sat_node_t;

/*!
 * tp structure
 */
typedef struct
{
  /*! frequency of TP */
  u32 freq;
  /*! symbol rate of TP */
  u32 sym;

  /*! The above will be saved firstly into data cache*/

  /*! NIT pid */
  u32 nit_pid     : 14;
  /*! polarity types, 0: horizontal, 1: vertical */
  u32 polarity    : 2;

  /*! TP node index in range of 0 - 299 */
  u32 id          : 16;
    /*! fec_inner */
  u32 fec_inner          : 4;
  /*! PAT version */
  u32 pat_version : 8;
  /*! modulate */
  u32 nim_modulate    : 8;
  /*! Scrambled or not, 0: not scrambled, 1: scrambled */
  u32 is_scramble : 1;
  /*! flag on 22k or not at cur tp, 0: 22k off, 1: 22k on */
  u32 is_on22k    : 1;
  /*! nim type*/
  u32 nim_type  : 4;
  /*! reserved field for 32 bit alignment */
  u32 reserved    : 2;
  /*! Satellite node index */
  u32 sat_id      : 16;

  /*
  ---------------------------------------------------------
  above into data cache
   */
  u32 emm_num :6;
  
    u32 sdt_version              : 16;
#if 1 //ecm_num support
  ca_desc_t emm_info[MAX_EMM_DESC_NUM];
#endif
} dvbs_tp_node_t;

/*!
 * program struct
 */
typedef struct
{
  /*!Each bit of fav_grp_flag represents a single favorite group*/
  u32 fav_grp_flag;

  u32 tv_flag                 : 1;
  u32 lck_flag                : 1;
  u32 skp_flag                : 1;
  u32 hide_flag               : 1;
  /*! audio volume */
  u32 volume                  : 5;
  /*! audio channel option, 0: channel 1 \sa audio_pid1, 1: channel 2
   \sa audio_pid2 */
  u32 audio_channel           : 5;
  /*! scrambled flag 0: not scrambled, 1: scrambled */

  u32 audio_ch_num            : 5;
  /*! video pid */
  u32 video_pid               : 13;

  /*! pcr pid */
  u32 pcr_pid                 : 13;
  /*! audio track option, 0: stereo, 1: left, 2: right */
  u32 audio_track             : 2;
  u32 is_scrambled            : 1;
  u32 ca_system_id            : 16;

  u32 is_audio_channel_modify     :1;

  /*
  ---------------------------------------------------------
  above into data cache
   */
  u32 tp_id                      : 16;
  /*!Default order of pg */
  u32 default_order    :16;

  /*! Program node index in range of 0 - 65535 */
  u32 id                      : 16;
  u32 sat_id                  : 16;

  /*! service number in stream */
  u32 s_id                    : 16;

  u32 orig_net_id             : 16;
  u32 ts_id                   : 16;
  u32 service_type   :8;
  /*!
    mosaic found
    */
  u32 mosaic_flag : 8;
  /*!
    nvod reference des cnt
    */
  u32 nvod_reference_svc_cnt : 16;


  /*! TKGS_SUPPORT START */
  u32 age_limit               :5;
  u32 logical_num           : 14;
  /*!
    used by aiset bouquet protocol
    */
  u32 operator_flag               :1;
  u32 reserve1                :11;

  /*! TKGS_SUPPORT END */
  u32 pmt_pid              :16;
  u32 video_type         :10;
  u32 ecm_num       :6;
#if 1 //ecm_num support
  cas_desc_t cas_ecm[MAX_ECM_DESC_NUM];
#endif
  audio_t audio[DB_DVBS_MAX_AUDIO_CHANNEL];
  /*! program name */
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1];
  u32 bouquet_id             : 16;

  /*!
    mosaic descriptor(service_type = 0x06)
    */
  mosaic_t mosaic;

  u8 run_status;
  
} dvbs_prog_node_t;

#define MAX_TABLE_CACHE_SIZE (10) // must more than sizeof(pg_data_t)
#define MAX_VIEW_CACHE_SIZE  (sizeof(item_type_t))

typedef enum
{
  /*! A view contains all favorite radio programs */
  DB_DVBS_FAV_RADIO = 0,
  /*! A view contains all favorite TV programs */
  DB_DVBS_FAV_TV,
  /*! A view contains all favorite TV programs */
  DB_DVBS_FAV_ALL_TV,
  /*! A view contains all favorite TV programs */
  DB_DVBS_FAV_ALL,
  /*! This view contains all radio programs */
  DB_DVBS_ALL_RADIO,
  DB_DVBS_ALL_RADIO_IGNORE_SKIP_FLAG,
  /*! This view contains all TV programs */
  DB_DVBS_ALL_TV,
  DB_DVBS_ALL_TV_IGNORE_SKIP_FLAG,
  DB_DVBS_TV_RADIO,
  DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG,
  DB_DVBS_ALL_PG,
  DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG,
  DB_DVBS_ALL_TP,
  DB_DVBS_TP_PG,
  DB_DVBS_TP_PG_IGNORE_SKIP_FLAG,
  DB_DVBS_MOSAIC,
  DB_DVBS_NVOD_TIME_SHIFT_EVENT_PROG,
  DB_DVBS_TP_PG_SHOW,
  DB_DVBS_SAT_PG,
  DB_DVBS_ALL_TV_FTA,
  DB_DVBS_ALL_TV_CAS,
  DB_DVBS_ALL_TV_HD,
  DB_DVBS_FAV_TV_CAS,
  DB_DVBS_FAV_TV_FTA,
  DB_DVBS_FAV_TV_HD,
  DB_DVBS_ALL_RADIO_FTA,
  DB_DVBS_ALL_RADIO_CAS,
  DB_DVBS_ALL_RADIO_HD,
  DB_DVBS_FAV_RADIO_CAS,
  DB_DVBS_FAV_RADIO_FTA,
  DB_DVBS_FAV_RADIO_HD,
  DB_DVBS_SELECTED_NOTTKGS_SAT,
  /*! Invaild view*/
  DB_DVBS_INVALID_VIEW
} dvbs_view_t; /*! Enumator of view names */

typedef enum
{
  DB_DVBS_PG_NODE = 0,
  DB_DVBS_SAT_NODE,
  DB_DVBS_TP_NODE
}dvbs_sort_node_type_t;

typedef enum
{
  /*!Sorting pg name from A to Z*/
  DB_DVBS_A_Z_MODE = 0,
  /*!Sorting pg name from Z to A*/
  DB_DVBS_Z_A_MODE,
  /*!FTA first mode*/
  DB_DVBS_FTA_FIRST_MODE,
  /*!Lock last*/
  DB_DVBS_LOCK_FIRST_MODE,
  /*!Default order*/
  DB_DVBS_DEFAULT_ORDER_MODE,
  /*!Default order*/
  DB_DVBS_LOCAL_NUM,
  /*!Default order*/
  DB_DVBS_0_9_MODE,
    /*!Default order*/
  DB_DVBS_9_0_MODE,
}dvbs_sort_type_t;

typedef enum
{
  DB_DVBS_MARK_TV  = 0x0,
  DB_DVBS_MARK_LCK ,
  DB_DVBS_MARK_SKP ,
  DB_DVBS_FAV_GRP	 ,
  DB_DVBS_DEL_FLAG ,
  DB_DVBS_SEL_FLAG ,
  DB_DVBS_HIDE_FLAG
}dvbs_element_mark_digit;

/*!
 * program sort struct
 */
typedef struct
{
  u16  node_id;
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1];
  u32  is_scrambled:1;
  u32  lck_flag:1;
  u32  default_order;
}pro_sort_t;

/*!
 * program sort identify struct
 */
typedef struct
{
  u16  id_count;
  u16  id_offset;
  u16  id_start;
  u16  id_init;
}pro_sort_id_t;

/*!
 * program view identify struct
 */
typedef struct
{
  dvbs_sort_type_t  sort_type;
  u16  view_init;
  u16  view_count;
  u16 *p_mem;
}pro_view_id_t;

/*!
  defines check group info
  */
typedef struct tag_check_group_info
{
 /*!
  [IN]sat id.
  */
  u32 context;
 /*!
  [IN]filled the group, original tv pg id.
  */
  u16 orig_tv;
 /*!
  [IN]filled the group, original radio pg id.
  */
  u16 orig_rd;
 /*!
  [IN]filled the group, original tv pg pos.
  */
  u16 orig_tv_pos;
 /*!
  [IN]filled the group, original radio pg pos.
  */
  u16 orig_rd_pos;
 /*!
  [OUT]fill the group, original tv pg id.
  */
  BOOL tv_found;
 /*!
  [OUT]fill the group, original radio pg id.
  */
  BOOL rd_found;
  /*!
  [OUT]fill the group, original tv pg id.
  */
  u16 tv_id;
 /*!
  [OUT]fill the group, original radio pg id.
  */
  u16 rd_id;
  /*!
  [OUT]fill the group, original tv pg id.
  */
  u16 tv_pos;
 /*!
  [OUT]fill the group, original radio pg id.
  */
  u16 rd_pos;
  /*!
  temp var
  */
  u16 tv_cur;
 /*!
  temp var
  */
  u16 rd_cur;
}check_group_info_t;

typedef struct tag_spec_check_group
{
 /*!
  all group.
  */
  check_group_info_t all_group;
 /*!
  all satellite group
  */
  check_group_info_t sat_group[DB_DVBS_MAX_SAT];
 /*!
  all favorite group
  */
  check_group_info_t fav_group[MAX_FAV_GRP];
}spec_check_group_t;

/*!
  Init DVBS database module.
  \return DB_DVBS_OK if succeed,
    DB_DVBS_FAILED for failed.
 */
db_dvbs_ret_t db_dvbs_init(void);

/*!
  Create view in db dvbs
  \param[in] name:  iew name
  \param[in] context: context information
  \param[in] buf_ex:  external buffer for creating view
  \return Return current created view id
 */
u8 db_dvbs_create_view(dvbs_view_t name, u32 context, u8 *buf_ex);

/*!
  Create view for allin db dvbs
  \param[in] name:  iew name
  \param[in] p_context: context information
  \param[in] fav_count: fav information
  \param[in] buf_ex:  external buffer for creating view
  \return Return current created view id
 */
u8 db_dvbs_create_view_all(dvbs_view_t name, u16* p_context,u16 fav_count, u8 *buf_ex);

/*!
  Return current view type
  \param[in] name:  iew vv_id
  \return Return current view type
 */dvbs_view_t db_dvbs_get_view_type(u8 vv_id);


/*!
  Get view del flag
  \param[in] view_id: view id
 */
BOOL db_dvbs_get_view_del_flag(u8 view_id);

/*!
  Destroy view
  \param[in] view_id: view id to be destoryed
 */
void db_dvbs_destroy_view(u8 view_id);

/*!
  Get item count in a view.
  \param[in] view_id : id for view
  \return item count in db dvbs
 */
u16 db_dvbs_get_count(u8 view_id);

/*!
  Delete all user datas and restore all preset Satellite/TP/Program information
  into memory.
  \param[in]blk_id: block id
  \param[in]ext_mem_blk_id: block id of external memory block, zero for none
  \return DB_DVBS_OK for succeed otherwise return DB_DVBS_FAILED.
 */
db_dvbs_ret_t db_dvbs_restore_to_factory(u8 blk_id, u8 ext_mem_blk_id);

/*!
  Add a new TP information into database

  \param[in,out] p_node An user TP information, can't be NULL.
  \param[in] view id
  \return Function will return DB_DVBS_FULL if datebase was full and return
  DB_DVBS_DUPLICATED if TP frequency was duplicated within the same
  satellite. Otherwise it'll return DB_DVBS_OK.
  Function will fill new TP id in p_node->id.
 */
db_dvbs_ret_t db_dvbs_add_tp(u8 view_id, dvbs_tp_node_t *p_node);

/*!
   Edit a TP information by TP index

 \param[in] p_node An user TP information, can't be NULL.

 \return Function will return DB_DVBS_DUPLICATED if new node information is the
          same as old value in datebase. Return zero if successed. And function
          will assertion failed by bad TP index.
 */
db_dvbs_ret_t db_dvbs_edit_tp(dvbs_tp_node_t *p_node);

/*!
 Save edit to tp
 \param[in] p_tp_node: tp node information
 */
void db_dvbs_save_tp_edit(dvbs_tp_node_t *p_tp_node);

/*!
 Undo edit to tp
 \param[in] p_tp_node: tp node information
 */
void db_dvbs_undo_tp_edit(dvbs_tp_node_t *p_tp_node);

/*!
   Delete a TP in database. This operation will cause all programs which belong
   to this TP will be deleted automatically.

 \param[in] id TP index in dvbs_tp_node_t

 \return Return DB_DVBS_NOT_FOUND if can't find the TP by id.  Return
          DB_DVBS_OK if succeed.
 */
void db_dvbs_del_tp(u8 view_id, u16 pos);

/*!
   Add a new Program information into database

 \param[in,out] p_node An user program information, can't be NULL.

 \return Function will return DB_DVBS_FULL if datebase was full and return
          DB_DVBS_DUPLICATED if video_pid or program name was duplicated within
          the same TP.  Otherwise it'll return DB_DVBS_OK.
          Function will fill new program id in p_node->id.
 */
db_dvbs_ret_t db_dvbs_add_program(u8 view_id, dvbs_prog_node_t *p_node);

/*!
  Edit a program information by program index

  \param[in] p_node An user program information, can't be NULL.

  \return Function will return DB_DVBS_DUPLICATED if new node information
  is the same as old value in datebase. Return DB_DVBS_OK if successed.
  And function will assertion failed by bad TP index.
  */
db_dvbs_ret_t db_dvbs_edit_program(dvbs_prog_node_t *p_node);
/*!
  Save one pg node
  \param[in] p_pg_node: pg node to be saved
  */
void db_dvbs_save_pg_edit(dvbs_prog_node_t *p_pg_node);

/*!
  Save one pg node
  \param[in] p_pg_node: pg node
  */
void db_dvbs_undo_pg_edit(dvbs_prog_node_t *p_pg_node);

/*!
  Get program info by id

  \param[in] id Program id
  \param[in,out] p_pg Program info

  \return Return DB_DVBS_OK for succeed otherwise return DB_DVBS_FAILED.
  */
db_dvbs_ret_t db_dvbs_get_pg_by_id(u16 id, dvbs_prog_node_t *p_pg);

/*!
  Get TP info by id

  \param[in] id TP id
  \param[in,out] p_tp TP info

  \return Return DB_DVBS_OK for succeed otherwise return DB_DVBS_FAILED.
  */
db_dvbs_ret_t db_dvbs_get_tp_by_id(u16 id, dvbs_tp_node_t *p_tp);

/*!
   This func will search in view and check the id of the node which include in
   view.  if the id is same with given id ,the node's position will be returned.
   if no corresponding view was created, of depth of the view is 0, or input pos
   exceeding the depth of the view, the func will return INVALIDPOS
   \param[in]view id: view id to search
   \param[in]pos:     item position in view
 */
u16 db_dvbs_get_id_by_view_pos(u8 view_id, u16 pos);

/*!
   This func search in view after any view-creating func were called by given id.
   if no corresponding view was created,or depth of view is 0,the func will
   return INVALIDID
 */
u16 db_dvbs_get_view_pos_by_id(u8 view_id, u16 id);

/*!
  move item to destination position

  \param[in] name View name
  \param[in] src Source position in view
  \param[in] des Destition position in view

  \return DB_DVBS_OK means success otherwise means failed
  */
db_dvbs_ret_t db_dvbs_move_item_in_view(u8 view_id, u16 pos_src, u16 pos_des);

/*!
   exchange item to destination position

 \param[in] name View name
 \param[in] src Source position in view
 \param[in] des Destition position in view

 \return DB_DVBS_OK means success otherwise means failed
 */
db_dvbs_ret_t db_dvbs_exchange_item_in_view(u8 view_id, u16 pos_src,u16 pos_des);

/*!
   exchange item to destination position

 \param[in] name View name
 \param[in] p_mem
 \param[in] len

 \return DB_DVBS_OK means success otherwise means failed
 */
db_dvbs_ret_t db_dvbs_dump_item_in_view(u8 view_id, void *p_mem, u16 len);

/*!
 Save certain view information into flash
 \param[in] view id
 */
void db_dvbs_save(u8 view_id);

void db_dvbs_save_use_hide(u8 view_id);

/*!
 Discarding the temporary modification in memory
 */
void db_dvbs_undo(u8 view_id);


/*!
 Check whether pg number in current pg view is more than the maximum item number in view
 return TRUE: is current pg view is full
  FALSE: current pg view is not full
 */
BOOL is_pg_full(void);
/*!
 Check whether tp number in current tp view is more than the maximum item number in view
 return TRUE: current tp view is full
  FALSE:current tp view is not full
 */
BOOL is_tp_full(void);
/*!
 Get the specified pg according to input ts id and service id
 \param[in] ts_id: the ts id of the pg to be got
 \param[in] s_id : the service id of the pg to be got
 \param[in] p_pg : the pg node for saving pg got from data base
 */
BOOL db_dvbs_get_special_pg(u16 ts_id, u16 s_id,dvbs_prog_node_t* p_pg);

/*!
  Change status
  \param[in]view_id:  view contains certain item
  \param[in]item_pos:   item position in view
  \param[in]dig:        digital position of item status
  \param[in]param:      The highest bit is set or reset
  the position to operate
 */
void db_dvbs_change_mark_status(u8 view_id, u16 item_pos, dvbs_element_mark_digit dig, u16 param);

/*!
  get view active status
  \param[in]view_id: view contains certain item
  \param[in]item_pos:   item position in view
  \param[in]dig:        digital position of item status
  \param[in]param:      The highest bit is set or reset
  the position to fetch status
  */
BOOL db_dvbs_get_mark_status(u8 view_id, u16 item_pos,
dvbs_element_mark_digit dig, u16 param);

/*!
  \param[in]parent_view_id: id of the view to be searched for the input string
  \param[in]p_string_in:   string to be searched for in given view
  \param[in/out]p_sub_view_id the sub-view id
  return    TRUE:  input string existing
  FALSE: input string not found
  */
BOOL db_dvbs_find(u8 parent_view_id, const u16 *p_string_in, u8 *p_sub_view_id);
/*!
  \param[in]parent_view_id: view to be sorted
  \param[in]sort_type:  sort type to be selected
  \param[in]p_sub_view_id the sub-view created based on sort result
  \param[in]view: view to be sorted
   */

#if 0
void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type);
#endif

void db_dvbs_pg_sort_init(u8 view_id);

void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type);

void db_dvbs_pg_sort_deinit(void);

/*!
  \param[in]parent_view_id: view to be sorted
  \param[in]sort_type:  sort type to be selected
  \param[in]p_sub_view_id the sub-view created based on sort result
  \param[in]view: view to be sorted
  */
void db_dvbs_tp_sort(u8 view_id, dvbs_sort_type_t sort_type);
/*!
  \param[in]p_pg: pg info
  */
u32 get_audio_channel(dvbs_prog_node_t *p_pg);

/*!
  Special check group.

  \param[in] p_group group info.
  \param[in] buf_ex buffer for create view.

  \return NULL.
  */
void special_check_group(spec_check_group_t *p_group, u8 *buf_ex);

//lint +e601 +e10 +e148 +e38 +e129 +e49 +e101 +e132 +e578 +e745 +e939 +e752
#endif
#endif // End for __DB_DVBS_H

