#ifndef __DOWNLOAD_MANAGER_H__
#define __DOWNLOAD_MANAGER_H__

#include <fcntl.h>
#include <stddef.h>     /* for NULL */
#include <string.h>
#include <stdlib.h>     /* for exit() */
#include <unistd.h>
#ifdef __LINUX__
#include <dirent.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <pthread.h>
#include "eventSink.h"
#include "http_download_mini.h"

//typedef struct _Task_Info {
//    unsigned char abort_flag; /// 0-not abort    1-abort
 //   int          task_pro;
//} TaskInfo;

typedef struct _Download_Manager {
    //TaskInfo *task_info;
    http_download_mini * instance;
    BOOL abort_flag; /// 0-not abort    1-abort
    int      task_pro;
} DownloadManager;

void   http_download_mamager_init();
void   add_task_to_manager(int task_prio, BOOL abort_flag, http_download_mini * instance);
void   remove_task_from_manager(int task_prio);
BOOL check_task_download_running(int task_prio);
void   get_running_tasks_from_manager(int * task_list);
void   abort_download_task(int task_prio, BOOL abort_flag);


#endif

