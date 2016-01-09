/* mini_httpd.h - struct defines for mini_httpd */

#ifndef _MINI_HTTPD_H_
#define _MINI_HTTPD_H_

#define CONNECTION_NUM 2            //support two terminal connection
#define IP_STRING_NUM 128

#define HTTP_SERVER_PORT1 81    //if you want change port num, you should check
#define HTTP_SERVER_PORT2 8181  //weather conflict to port defined in http.c

typedef struct customer_info_s
{
    char customer_ip[IP_STRING_NUM];
    int conn_fd;
}customer_info_t;

typedef struct conn_socket_s
{
    int conn_fd;
    u8 is_used;
}conn_socket_t;

typedef struct mini_httpd_s
{
    unsigned int task_prio_satip[CONNECTION_NUM];
    unsigned int task_prio_server; //mini_httpd server task
    unsigned int task_prio_handle; //user request handle task
    unsigned int stack_size_satip[CONNECTION_NUM];
    unsigned int stack_size_server;
    unsigned int stack_size_handle;
}mini_httpd_t;

#if 0
typedef enum
{
    PERMIT_CUSTOMER_CHANGE_PROGRAM_CMD,
    //NIM_LOCK_STAUTS_CMD,
}mini_httpd_ioctl_cmd_e;
#endif

unsigned short get_mini_httpd_port();
RET_CODE get_mini_httpd_start_status();
//int start_mini_httpd(satip_changeprogram_t p_changeprogram, u16 port_num);
void mini_httpd_config(mini_httpd_t *p_server_config);
int stop_mini_httpd();
void stop_play_callback();
//RET_CODE mini_httpd_io_ctrl(u32 cmd, void *param);

#endif /* _MINI_HTTPD_H_ */
