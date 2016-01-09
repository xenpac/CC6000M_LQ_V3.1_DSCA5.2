/******************************************************************************/
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif 


#define LOCAL_IP_ADDR_LEN  32
#define CITY_ID_LEN 16
#define WEATHER_STATUS_LEN  16
#define WEATHER_DIRECTION_LEN 32
#define WEATHER_STATUS_LEN 16
#define WEATHER_POWER_LEN 8
#define WEATHER_TEMP_LEN 8
#define WEATHER_DATE_LEN 32
#define REAL_TIME_DATE_LEN 8
/*
1 for day 
2 for night
status-天气状态(晴、多云等)
direction-风向
*/
typedef struct{
    char city[CITY_ID_LEN];
    char status1[WEATHER_STATUS_LEN];
    char status2[WEATHER_STATUS_LEN];
    char direction1[WEATHER_DIRECTION_LEN];
    char direction2[WEATHER_DIRECTION_LEN];
    char power1[WEATHER_POWER_LEN];
    char power2[WEATHER_POWER_LEN];
    char temperature1[WEATHER_TEMP_LEN];
    char temperature2[WEATHER_TEMP_LEN];
    char savedate[REAL_TIME_DATE_LEN];

}weather_report_item;

typedef struct{
    char year[REAL_TIME_DATE_LEN];
    char month[REAL_TIME_DATE_LEN];
    char day[REAL_TIME_DATE_LEN];
    char hour[REAL_TIME_DATE_LEN];
    char minite[REAL_TIME_DATE_LEN];
    char second[REAL_TIME_DATE_LEN];
}weather_report_real_time;

typedef  struct{
	char local_ip_addr[LOCAL_IP_ADDR_LEN];
    char city_id[CITY_ID_LEN];
	weather_report_item * p_weather_item_list;
    weather_report_real_time * p_real_time_list;
} weather_report_data_t;
typedef struct{

	void  * 	p_bg_task_hdl;
    weather_report_data_t  * p_weather;
}WEATHER_REPORT_HDL_T;


WEATHER_REPORT_HDL_T * init_weather_report_feed();
int deinit_weather_report_feed(void *hdl);
int weather_report_get_city_info(void *hdl);
int weather_report_get_weather_info(void *hdl,char *city_id);
int weather_get_sys_real_time(void *hdl);








/*add for test net speed 
	0) find the ip address: not the local net ip 
	1) find the net provider: chinamobile , chinaunicom , chinatelecom
	2) get the net speed:by second / 10 seconds 
	3) use thread to get the speed 
*/
 /*!
  define the type of event delived to app
  */
typedef enum
{
/*!
   	the thread was ip info success
*/
  	ISP_PROVIDER_IP_SUCCESS,
/*!
	the thread was ip info fail
*/
	ISP_PROVIDER_IP_FAIL,
/*!
   	the thread was city info success
*/
  	ISP_PROVIDER_CITY_SUCCESS,
/*!
	the thread was city info fail
*/
	ISP_PROVIDER_CITY_FAIL,	
/*!
   	the thread was ips info success
*/
  	ISP_PROVIDER_IPS_SUCCESS,
/*!
	the thread was ips info fail
*/
	ISP_PROVIDER_IPS_FAIL,	
/*!
   	the thread was net SPEED success
*/
  	ISP_PROVIDER_SPEED_SUCCESS,
/*!
	the thread was net SPEED fail
*/
	ISP_PROVIDER_SPEED_FAIL,
/*!
   the thread was running right now
*/
	ISP_PROVIDER_PARSERING,
/*!
   the thread was finish
*/
	ISP_PROVIDER_FINISH,
/*!
  invalid type
  */   
   ISP_PROVIDER_INSP_INFO_INVALID,
}ISP_PROVIDER_EVENT_E;
typedef RET_CODE  (*ips_info_event_cb_fun_t)(ISP_PROVIDER_EVENT_E, u32);
typedef enum
{
	CHINA_MOBILE ,
	CHINA_UNICOM,
	CHINA_TELECOM,
	OTHER_ISP,
}ISP_PROVIDER_TYPE ;

#define IP_DEAIOLS_STR_MAX  32
typedef struct 
{
	char ipaddress[32] ;
	char  country[IP_DEAIOLS_STR_MAX] ;
	char  areainfo[IP_DEAIOLS_STR_MAX] ;
	char  regioninfo[IP_DEAIOLS_STR_MAX] ;
	char  cityinfo[IP_DEAIOLS_STR_MAX] ;
	char  ispinfo[IP_DEAIOLS_STR_MAX] ;
}net_ip_details ;
typedef struct 
{
	net_ip_details ip_details ;

	u32 speed ;

	///for thread 
	ips_info_event_cb_fun_t  parser_event_cb ;	
	u32 prio_id ;
	u32* p_stack ;
	
	BOOL is_init;/*whether this type of data provider has been initilized successfully*/
}net_isp_info;

///add two function for get the ip , and get the city & isp 
BOOL get_ip_address(char *ip_addr);
BOOL get_IP_details_by_IP(net_ip_details *ipdetails)   ;  


BOOL get_net_isp_info(int  prio,void* p_cb)	;
char* get_net_info_ip();
char* get_net_info_city();
char* get_net_info_ips();
u32 get_net_speed();

///yiyuan add this 
int unicode_to_utf8(u16 *in, int insize, u8 *out) ;

///Tongxiang add this 
typedef  int (*write_data_fun_t)(u8 * p_data/*IN*/ , int data_len/*IN */);
typedef  int (*read_data_fun_t)(u8 ** pp_data/*OUT*/,int * p_data_len/*OUT*/);

typedef enum
{
	NO_ACK,
	READ_OK,
	WRITE_OK,
	READ_FAIL,
	WRITE_FAIL,
	INVALID_ACK,
}RW_STATUS;
/*********************************************************************
*
*  register callback function for reading and writing data block from flash
*
*  return value:
*           fail:  NULL
*           ok:   handle point to read/write flash instance  
*
*
********************************************************************/
void *  register_rw_flash_func(void * read_func/*IN*/,void * write_func/*IN*/);

/********************************************************************
*
*   notify dataprovider that data has been processed
*
*********************************************************************/
void  notify_dataprovider(void * p_handle,RW_STATUS *status);

#ifdef __cplusplus
}
#endif 


int read_from_flash(u8 **pp_data, int *p_data_len);

int write_to_flash(u8 * p_data, int data_len);


