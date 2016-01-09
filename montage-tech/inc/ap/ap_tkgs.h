/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_TKGS_H_
#define __AP_TKGS_H_


/*!
  \file ap_tkgs.h

  Declare tkgs application defines some command, event type, and policy sturcture etc.
  */

/*****************************************************************************************/
/*!                             The common macro and structure.                          */ 
/*****************************************************************************************/

/*!
  default satellite number
  */
#define TKGS_DEFAULT_SAT_NUM                            (1)

/*!
  default tp number  
  */
#define TKGS_DEFAULT_TP_NUM                              (100)

/*!
  default program number.
  */
#define TKGS_DEFAULT_PG_NUM                              (1000)

/*!
  default service list number.
  */
#define TKGS_DEFAULT_SVC_LIST_NUM                    (5)

/*!
  default every service list's name number.
  */
#define TKGS_DEFAULT_SVC_NAME_NUM                  (2)

/*!
  default category number.
  */
#define TKGS_DEFAULT_CATEGORY_NUM                  (8)   

/*!
  default every category's name number.
  */
#define TKGS_DEFAULT_CATE_NAME_NUM               (2)

/*!
  Declare max channel blocking number.
  */
#define DEFAUTL_CHANNEL_BLK_NUM                     (50)  

/*!
  max user message length
  */
#define MAX_USER_MSG_LEN                                (64)

/*!
  default name length.
  */
#define MAX_TKGS_NAME_LENGTH                           (16)

/*!
  Max audio pid supported when parse PMT information
  and the actual audio pid number can be more or less than this value
  */
#define MAX_AUDIO_PID_NUM                               (8)

/*!
 category type
 */
typedef   u16     cate_id_t;

/*!
 tkgs buffer type
 */
typedef enum tag_tkgs_buf
{
/*!
 tkgs input buffer.
 */
 TKGS_INPUT_BUFFER,
/*!
 tkgs output buffer.
 */ 
 TKGS_OUTPUT_BUFFER,
/*!
 TKGS running buffer.
 */ 
 TKGS_RUN_BUFFER,
/*!
 TKGS impl run buffer.
  */ 
 TKGS_IMPL_BUFFER, 
}tkgs_buf_t;

/*!
  Delcare version number state
  */
typedef enum tag_tkgs_ver_method
{
 /*!
  In the during stanby or background, if found the same version number, stop update.
  */
  SAME_VER_STOP_UP,
 /*!
  In the factory reset, ignore check version number
  */
  SAME_VER_IGNORE,
 /*!
  in the manual activity, send event to ui when found the same version number.
  */ 
  SAME_VER_SEND_EVT,
 /*!
  same version end.
  */
  SAME_VER_END,
}tkgs_ver_method_t;

/*!
 tkgs montior function in charge of check table type.
 */
typedef enum tag_tkgs_check_table
{
/*!
  tkgs monitor request table unknown.
  */
  TKGS_CHECK_UNKNOWN,
/*!
  tkgs monitor request pmt table.
  */
  TKGS_CHECK_PMT_TABLE,
/*!
  tkgs monitor rquest tkgs table.
  */
  TKGS_CHECK_TKGS_TABLE,
}tkgs_check_table_t;

/*****************************************************************************************/
/*!                    The structures use for TKGS interaction with UI                   */ 
/*****************************************************************************************/
/*!
 tkgs policy config 
 */
typedef struct tag_tkgs_policy_cfg
{
 /*!
  tkgs pid 
  */
  u32 tkgs_pid          : 13;
 /*!
  using attach block.
  */ 
  u32  is_attach_block  : 1;
 /*!
  attach block id.
  */
  u32 block_id          : 18; 
 /*!
  handle version n
  */
  tkgs_ver_method_t  ver_method;
 /*!
  tkgs monitor request table 
  */
  tkgs_check_table_t check_table;
}tkgs_policy_cfg_t;



/*****************************************************************************************/
/*!                  TKGS input data all related maroc and structure.                    */ 
/*!                  Provide operation and parsed tkgs data parameters.                  */
/*****************************************************************************************/
/*!
  Delcare all operate mode for tkgs update
  */
typedef enum tag_tkgs_operate_mode
{
/*!
  Automatic profile : TKGS is active. The channel database is fully under control of TKGS. 
  The user shall not be allowed operations that result in a change to the channel database.
  */
  TKGS_AUTOMATIC_PROFILE  = 0x00 ,
  
/*!
  Customizable profile : TKGS is active. TKGS is still in control of the channel list, but it
  respects the ordering of channels. Additionally some restrictions of Profile 1 are liftedd so
  that the user can modify the channel list.
  */
  TKGS_CUSTOMIZE_PROFILE  = 0x01,
  
/*!
  TKGS OFF : In this mode TKGS doesn't manage the channel list in the background, managing the 
  channel list is completely left to the user. 
  */
  TKGS_OFF_PROFILE        = 0x02,
}tkgs_operate_mode_t;


/*!
  tkgs update satellite information
  */
typedef struct
{
  /*!
    diseqc info
    */
  nc_diseqc_info_t diseqc_info;
  /*!
    sat rcv para
    */
  sat_rcv_para_t sat_rcv;
}tkgs_sat_info_i_t;

/*!
  tp structure (input data)
  */
typedef struct
{
 /*!
  tkgs  pid.
  */
  u16           tkgs_pid;
 /*!
  reserved
  */
  u16           reserved;
  /*!
  TP info
  */
  tp_rcv_para_t tp_info;
}tkgs_tp_info_i_t;

/*!
  current system supports language type
  */
typedef struct tag_language_code
{
 /*!
  bibliographic use
  */ 
  u8 lang_b[ISO_639_2_DESC_LENGTH];
 /*!
  terminological use
  */
  u8 lang_t[ISO_639_2_DESC_LENGTH];
}lang_code_t;


/*!
  application tkgs input data.
  */
typedef struct tag_tkgs_input_data
{
 /*!
  tkgs operate mode
  */
  tkgs_operate_mode_t opera_mode; 
 /*!
  handle tkge version method. 
  */
  tkgs_ver_method_t   ver_method;
/*!
  total service list.
  */  
  u8                  total_svc_list;
 /*!
  per-service-list's name number..
  */
  u8                   total_svc_name;
 /*!
  total channel block number.
  */
  u8                   total_block;
 /*!
  tkgs version number
  */
  u8                  ver_num;
 /*!
  total location
  */
  u8                  tp_list_depth;
 /*!
  total category info.
  */
  u8                  total_category;
 /*!
  total per-category name 
  */
  u8                  total_cate_name;
/*!
  tkgs satellite information
  */
  tkgs_sat_info_i_t   sat_info;
 /*!
  tkgs tp list
  */
  tkgs_tp_info_i_t    *p_tp_list;
 /*!
  language list
  */
  lang_code_t          lang_code;
  /*!
  total sat in this system;
  */
  u16                  total_sat_sys;
 /*!
  total tp in this system 
  */
  u16                  total_tp_sys;
 /*!
  total pg in this system.
  */
  u16                  total_pg_sys;
 /*!
  preferred service list name length.
  */
  u16 pre_svc_list_len;
 /*!
  preferred serice list name() 
  */
  char pre_svc_list_name[MAX_TKGS_NAME_LENGTH];
}tkgs_input_data_t;



/*****************************************************************************************/
/*!                  TKGS output data all related maroc and structure.                   */ 
/*!                  User for parsed tkgs data info and save to flash.                   */
/*****************************************************************************************/

/*!
  Declare tkgs satellite info.
  */
typedef struct tag_tkgs_sat_info
{
/*!
  node id
  */
  u16 node_id;
/*!
  orbital position
  */
  u16 orbital_position;
/*!
  west east flag. 
  A value "0" indicates the western position and a value "1" indicates the eastern position.
  */
  u8 west_east_flag;
/*!
  name length.
  */
  u8 name_len;
/*!
  reseved
  */
  u16 reseved;
/*!
  satellite name
  */  
  u8 name[MAX_TKGS_NAME_LENGTH];
}tkgs_sat_info_t;


/*!
 satellite delivery system descriptor.
 */
typedef struct tag_sat_delivery_sys_desc
{
 /*!
  TP frequency(Khz)
  */   
  u32 frequency;
 /*!
  Symbol rate
  */   
  u32 symbol_rate;
 /*!
  Satellite orbital position
  */   
  u32 orbital_position        : 16;
 /*!
  West or east orbit area
  */   
  u32 west_east_flag          : 1;
 /*!
  Polarization of transmitting signal
  */   
  u32 polarization            : 2;
 /*!
  roll off
  */   
  u32 roll_off                : 2;
 /*!
  modulation system
  */
  u32 modulation_sys          : 1;
 /*!
  modulation type
  */
  u32 modulation_type         : 2;
 /*!
  FEC inner
  */
  u32 FEC_inner               : 4; 
 /*!
  reserved
  */
  u32 reserved               : 4; 
}sat_delivery_desc_t;


/*!
  tkgs transport[TP] system decriptor
  */
typedef struct tag_tkgs_tp_info
{
 /*!
  satellite index.
  */ 
  u8 sat_index;
 /*!
  is hidden location. if the TKGS component descriptor which is located in the descriptor loop 
  of the elementary stream loop of the actual PMT table.
  */
  u8 hidden_loct;
 /*!
  own node id
  */
  u16 node_id;
 /*!
  satellite delivery system descriptor.
  */ 
  sat_delivery_desc_t delivery_desc;
}tkgs_tp_info_t;

/*!
  Declare tkgs program information
  */
typedef struct tag_tkgs_pg_info
{
 /*!
  satellite index
  */
  u32 sat_index               : 8;
 /*!
  tp index
  */  
  u32 tp_index                : 16;
 /*!
  EIT schedule flag
  */
  u32 eit_sch_flag             : 1;
 /*!
  EIT present following flag
  */
  u32 eit_pf_flag              : 1;
 /*!
  running status.
  */
  u32 run_sts                  : 3;
 /*!
  free_CA_mode : Service scrambled flag, 1: scramble 0: free
  */
  u32 is_scrambled             : 1;
 /*!
  reserved
  */
  u32 reserved_0               : 2;

 /*!
  network id
  */
  u16 network_id;
 /*!
  transport stream id
  */
  u16 ts_id;

 /*!
  service number in transport stream, 
  This is the same as the program_number in the corresponding program_map_section.
  */  
  u16 service_id;
 /*!
   service type (Service descriptor [DVB-SI Page 65])
   */
  u8 service_type;
 /*!
  Age limit. giving the recommended minimum age in years of the end user.
    0x0        : no age limit.
    0x1 to 0xf : Minimum age = age_limit + 3 years.
  */
  u8 age_limit;
 
 /*!
  how many the program belongs to categroy.
  */
  u32 cate_num                : 8;
 /*!
  video_type
  */
  u32 video_type              : 8;
 /*!
  tkgs service locator
  */
  u32 tkgs_svc_locator        : 13;
/*!
  IS belong with the current service list : 1 is belong.
  */
  u32 belong_svc_list         : 1;
/*!
 reserved
 */
  u32 reserved_1              : 2;

/*!
  pcr pid
  */
  u32 pcr_pid                   : 13;
/*!
  elementary pid
  */
  u32 video_pid                 : 13;
/*!
  audio count
  */
  u32 audio_count               : 6;

/*!
  tksg logical_order(LCN)
  */
  u16 logical_order;
/*!
 reserved
 */
  u16 reserved_2;
/*!
  Service name information Service descriptor in the DVB-SI Page 64
  */
  u8 name[MAX_SVC_NAME_LEN];  
/*!
  audio 
  */
  audio_t audio[MAX_AUDIO_PID_NUM];

/*!
 category id list.
 */
 cate_id_t *p_cate_id_list;
/*!
  control world
  */
  //u64 control_world;
}tkgs_pg_info_t;


/*!
  service list name.
  */
typedef struct tag_svc_list_name
{
/*!
  iso_639 language code
  */
  u8 lang_code[ISO_639_2_DESC_LENGTH];
/*!
  name length
  */
  u8 name_len;
/*!
  service list name.
  */
  u8 name[MAX_TKGS_NAME_LENGTH];
}svc_list_name_t;

/*!
 service list info.
  */
typedef struct tkgs_svc_list
{
/*!
  how name?
  */
  u32 count_name : 8;
  /*!
    reserved
    */
  u32 reserved : 24;
/*!
  service_list name 
  */  
  svc_list_name_t *p_name_list;
}tkgs_svc_list_t;


/*!
  category name.
  */
typedef struct tag_category_name
{
/*!
  iso_639 language code
  */
  u8 lang_code[ISO_639_2_DESC_LENGTH];
/*!
  name length
  */
  u8 name_len;
/*!
  service list name.
  */
  u8 name[MAX_TKGS_NAME_LENGTH];
}category_name_t;

/*!
  category information
  */
typedef struct tkgs_category
{
 /*!
  category id.
  */ 
  u16 category_id;
 /*!
  how name?
  */ 
  u16 count_name;
 /*!
  category name.
  */
  category_name_t *p_name_list;
}tkgs_category_t;


/*!
  declare tkgs channel blocking structure.
  */
typedef struct tag_tkgs_channel_block
{
 /*!
  If set to '1', some elements of the following delivery descriptor will be used 
  as blocking criteria.
  */ 
  u32 delivery_filter_flag    :1;
 /*!
  if set to '1', svc id match is necessary for blocking to take effect.
  */
  u32 svc_id_filter_flag      :1;
 /*!
  if set to '1', network id match is necessary for blocking to take effect.
  */
  u32 network_id_filter_flag  :1;
 /*!
  if set to '1', transport stream id match is necessary for blocking to take effect.
  */
  u32 transtr_id_filter_flag :1;
 /*!
  if set to '1', video pid match is necessary for blocking to take effect.
  */
  u32 video_pid_filter_flag  :1;
 /*!
  if set to '1', audio pid match is necessary for blocking to take effect.
  */
  u32 audio_pid_filter_flag  :1;
 /*!
  if set to '1', name match is necessary for blocking to take effect.
  */
  u32 name_filter_flag       :1;
 /*!
  if set to '1', frequency match is necessary for blocking to take effect.
  frequecy will be defined by the following satellite/cable/terrestrial delivery descriptor.
  */
  u32 freq_filter_flag        :1;
 /*!
  if set to '1', symbol rate match is necessary for blocking to take effect.
  symbol rate will be defined by the following satellite/cable/terrestrial delivery descriptor.
  */
  u32 sym_filter_flag         :1;
 /*!
  if set to '1', polarization match is necessary for blocking to take effect.
  polarization will be defined by the following satellite delivery descriptor.
  */
  u32 polar_filter_flag         :1;
 /*!
  if set to '1', orbital position match is necessary for blocking to take effect.
  orbital position will be defined by the following satellite delivery descriptor.
  */
  u32 orbital_filter_flag       :1;
 /*!
  if set to '1', modulation type match is necessary for blocking to take effect.
  modulation type will be defined by the following satellite delivery descriptor.
  */
  u32 modul_type_filter_flag    :1;
 /*!
  if set to '1', modulation system match is necessary for blocking to take effect.
  Modulation system will be defined by the following satellite delivery descriptor.?
  */
  u32 modul_sys_filter_flag     :1;
 /*!
  if set to '1', modulation match is necessary for blocking to take effect.
  Modulation will be defined by the following cable delivery descriptor.?
  */
  u32 modul_filter_flag         :1;
 /*!
  if set to '1', FEC match is necessary for blocking to take effect. FEC will be
  defined by the following satellite/cable delivery descriptor.
  */
  u32 fec_inner_filter_flag     :1;
 /*!
   reserved
   */
 u32 reserved     :1;
 /*!
  Frequency tolerance : If the absolute value of the compared freq is below or equal to this 
  value, this will be considered as a match.
  */ 
  u32 frequency_toler : 8; 
 /*!
  symbolrate tolerance : If the absolute value of the compared freq is below or equal to this 
  value, this will be considered as a match.
  */ 
  u32 symb_toler : 8; 
 /*!
  satellite delivery descriptor info.
  */
  sat_delivery_desc_t delivery_desc;
 /*!
  service id
  */
  u16 service_id;
 /*!
  network id
  */
  u16 network_id;
 /*!
  transport stream id
  */
  u16 transport_stream_id;
 /*!
  video pid
  */
  u16 video_pid;
 /*!
  audio pid
  */
  u32 audio_pid : 16;
 /*!
  match type
  */
  u32 match_type : 2;
 /*!
  case sensitivity
  */
  u32 case_sens  : 1;
 /*!
  service name length.
  */
  u32 svc_name_len : 8;
 /*!
  reserved
  */
  u32 reserved1  : 5;
 /*!
  service name
  */ 
  u16 svc_name[MAX_SVC_NAME_LEN];
}tkgs_channel_block_t;


/*!
  Declare tkgs output data structure.
  */
typedef struct tag_tkgs_output_data
{
/*!
  Each STB firmware is designed and compiled for a specific TKGS interface.
  If the interface_version specified below doesn't match the STB's TKGS interface version,
  the STB should stop processing TKGS table.
  */
  u8  inter_ver;
/*!
  does have user message.
  */
  u8                  has_user_msg;
/*!
  tkgs version number.
  */
  u8 ver_num;

/*!
  count sat;
  */
  u8              count_sat;
/*!
  user message descriptor.(After the update is complete, the STB shall present the TKGS message 
  to the user.)
  */
  u8                    user_msg[MAX_USER_MSG_LEN];
/*!
  tkgs satellite 
  */
  tkgs_sat_info_t *p_sat_list;

/*!
  count tp
  */
  u16              count_tp;
/*!
  program count
  */
  u16              count_pg;
/*!
  tp list
  */  
  tkgs_tp_info_t  *p_tp_list;
/*!
  pg list
  */
  tkgs_pg_info_t  *p_pg_list;

/*!
  count channel_list list.
  */
  u16              count_svc_list;
/*!
  preferred index.
  */
  u16              prefer_index;
/*!
  tkgs service list list.
  */
  tkgs_svc_list_t *p_svc_list_list;

/*!
  count category info
  */
  u16              count_category;
/*!
  channel block number
  */
  u8                  count_block;
/*!
  tkgs up success tp index (input)
  */
  u8 tkgs_up_success_tp_index;
/*!
  category list 
  */
  tkgs_category_t *p_category_list;
/*!
  channel block list 
  */  
  tkgs_channel_block_t *p_block_list;
}tkgs_output_data_t;



/*****************************************************************************************/
/*!              the structure use for  app_tkgs interaction with app_tkgs_impl          */ 
/*****************************************************************************************/


/*!
  Implementation policy of application tkgs
  */
typedef struct tag_tkgs_policy
{
/*!
 initialize policy
 */ 
 void (*init_policy)(handle_t _this, tkgs_policy_cfg_t *p_cfg);
/*!
  destory policy
  */
  void (*destory_policy)(handle_t _this);
/*!
  get input data.  
  */
  BOOL (*get_input_data)(handle_t _this, tkgs_input_data_t *p_data, 
                         BOOL is_monitor);
/*!
  save tkgs version number.
  */
  void (*save_ver_num)(handle_t _this, u8 ver_num);
/*!
  save output data.
  */
  RET_CODE (*save_output_data)(handle_t _this, tkgs_output_data_t *p_data);
/*!
  alloc running buffer.
  */
  void *(*alloc_buff)(handle_t _this, u32 size, tkgs_buf_t buf_type);
/*!
  Private data for application tkgs
  */
  void *p_priv_data;
}tkgs_policy_t;


/*****************************************************************************************/
/*!                      application tkgs command and event                              */ 
/*****************************************************************************************/
/*!
  Delcare all command for tkgs update
  */
typedef enum tag_tkgs_ret_up
{
/*!
  tkgs update failure.
  */
  TKGS_RET_UP_FAILURE = 0x00,
/*!
  tkgs update success.
  */
  TKGS_RET_UP_SUCCESS = 0x01,
}tkgs_ret_up_t;

/*!
  Declare all events which will be notified to high level modules like UI
  */
typedef enum tag_tkgs_evt
{
/*!
  Notify current tkgs progress and current signal information.
  */
  TKGS_EVT_PROGRESS = (APP_TKGS << 16),
/*!
  Notify ui not found hiddlen and visible location.
  */  
  TKGS_EVT_NOT_LOCATION,
/*!
  Notify UI found the same version number.
  */  
  TKGS_EVT_SAME_VER,
/*!
  Notify UI save tkgs data.
  */  
  TKGS_EVT_SAVE_DATA,
/*!
  start monitor(use for test case)
  */
  TKGS_EVT_START_MONITOR,
  /*!
  Notify UI space full.
  */  
  TKGS_EVT_SPACE_FULL,
/*!
  tkgs finished.(tkgs state is IDLE when finished. need ui start cmd monitor or 
  update).
  */
  TKGS_EVT_FINISHED = ((APP_TKGS << 16) | SYNC_EVT),
 /*!
  tkgs stop monitor.
  */
  TKGS_EVT_STOP_MONITOR,
}tkgs_evt_t;


/*!
  Delcare all command for tkgs update
  */
typedef enum tag_tkgs_cmd
{
/*!
  Start update, with two parameter.
  para1 : tkgs_policy_cfg_t
  */
  TKGS_CMD_START_UPDATE = 0, 
/*!
  start monitor. 
  */  
  TKGS_CMD_START_MONITOR,
/*!
  continue update (Used for forced to upgrade.) 
  */
  TKGS_CMD_CONTINUE_UPDATE,
/*!
  stop update
  */
  TKGS_CMD_STOP_UPDATE    = SYNC_CMD,
/*!
  stop monitor
  */  
  TKGS_CMD_STOP_MONITOR,
}tkgs_cmd_t;
/*!
 Construct tkgs policy
 */
tkgs_policy_t *construct_tkgs_policy(void);

/*!
  Call this method, load tkgs instance by the policy.
  
  \param[in] p_policy The policy of application tkgs update
  \return    Return app_t instance address
  */
app_t *construct_ap_tkgs(tkgs_policy_t *p_policy);



#endif // End for __AP_TKGS_H_

