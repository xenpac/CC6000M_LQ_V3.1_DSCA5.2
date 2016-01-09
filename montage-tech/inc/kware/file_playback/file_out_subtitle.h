/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_OUT_SUBTITLE_H__
#define __FILE_OUT_SUBTITLE_H__

/*!
  xxxxxxxx
  */
typedef enum{
     /*!
      xxxxxxxx
      */
     STOPPED = 0,
     /*!
      xxxxxxxx
      */
     RUNNING = 1,
     /*!
      xxxxxxxx
      */
     SUB_SEEK= 2,
     /*!
      xxxxxxxx
      */
     EXITING = 3,
     /*!
      xxxxxxxx
      */
     EXIT_END = 4,
} TASK_STATE;


/*!
  subt_sys_type_t
  */
typedef enum
{
  /*!
    SUBT_SYS_SSA
    */
  SUBT_SYS_SSA,
  /*!
    SUBT_SYS_SRT
    */
  SUBT_SYS_SRT,
  /*!
    SUBT_SYS_SMI
    */
  SUBT_SYS_SMI,
  /*!
    SUBT_SYS_SUB
    */
  SUBT_SYS_SUB,
  /*!
    SUBT_SYS_UNKOWN
    */
  SUBT_SYS_UNKOWN
}subt_sys_type_t;

/*!
      xxxxxxxx
  */
typedef struct
{
    /*!
      xxxxxxxx
    */
    void  *p_subt_task_hdl;
    /*!
      xxxxxxxx
    */
    int  thread_priority;
    //unsigned char * p_mem_start;
    /*!
      xxxxxxxx
    */
    void *p_mem_start;
    /*!
      xxxxxxxx
    */
    int   stack_size;

    /*!
         for subtitile in usb disk
    */
    void *p_out_sub_fifo_handle;

	/*!
       mutex for sub fio
      */
    unsigned int out_sub_fifo_mutex;

    /*!
      xxxxxxxx
    */
    int inter_subt_cnt;
    /*!
      xxxxxxxx
    */
    int extra_subt_cnt;
    /*!
      xxxxxxxx
    */
    int cur_extra_subt_id;//start from 0.

}File_Out_Subtile_T;

/*!
      xxxxxxxx
  */
typedef struct
{
    /*!
      xxxxxxxx
    */
    void *p_fifo_handle; //from p_out_sub_fifo_handle
    /*!
      xxxxxxxx
    */
    unsigned int fifo_mutex;//from out_sub_fifo_mutex

    /*!
      xxx
      */
    TASK_STATE  task_status;
    /*!
      xxxxxxxx
    */
    subt_sys_type_t type;
    /*!
      xxxxxxxx
    */
    char *file_name;
    /*!
      xxxxxxxx
    */
    int task_prio;
    /*!
      xxxxxxxx
    */
    int  file_size;
    /*!
      xxxxxxxx
    */
    char * file_buffer;
    /*!
      xxxxxxxx
    */
    int  seek_second;

} PROC_SUBT_HDL_T;

/*!
*   xxxx
*/
int file_out_subtile_start(int subtitleId, File_Out_Subtile_T *p_subtitle);
/*!
*   xxxx
*/
int file_out_subtile_seek(File_Out_Subtile_T *p_subtitle, int seconds);
/*!
*   xxxx
*/
int file_out_subtile_stop(File_Out_Subtile_T *p_subtitle);
/*!
*   xxxx
*/
int file_get_extra_subt(File_Out_Subtile_T *p_subt, unsigned char * subt_buf, int * pts);
/*!
*   xxxx
*/
int get_extra_subt_num(File_Out_Subtile_T *p_subt, unsigned char *file_path, int inter_cnt);
/*!
*   xxxx
*/
char *get_extra_subt_name(File_Out_Subtile_T *p_subt, int extra_id);

/*!
*   xxxx
*/
int get_extra_subtitle_num(char *path, char *film_name);

/*!
  tag_line_type_spec
  */
typedef enum tag_line_type_spec
{
  /*!
      xxxxxxxx
  */
  LINE_SCRIPT_INFO = 0,
  /*!
      xxxxxxxx
    */
  LINE_TITLE,
  /*!
      xxxxxxxx
    */
  LINE_ORIGINAL_SCRIPT,
  /*!
      xxxxxxxx
    */
  LINE_ORIGINAL_TRANS,
  /*!
      xxxxxxxx
    */
  LINE_ORIGINAL_TIMING,
  /*!
      xxxxxxxx
    */
  LINE_ORIGINAL_POINT,
  /*!
      xxxxxxxx
    */
  LINE_SCRIPT_TYPE,
  /*!
      xxxxxxxx
    */
  LINE_COLLISIONS,
  /*!
      xxxxxxxx
    */
  LINE_RESX,
  /*!
      xxxxxxxx
    */
  LINE_RESY,
  /*!
      xxxxxxxx
    */
  LINE_TIMER,
  /*!
      xxxxxxxx
    */
  LINE_FORMAT,
  /*!
      xxxxxxxx
    */
  LINE_STYLE,
  /*!
      xxxxxxxx
    */
  LINE_EVENT,
  /*!
      xxxxxxxx
    */
  LINE_DIALOGUE,
  /*!
      xxxxxxxx
    */
  LINE_TOTAL_SPEC
}line_type_spec_t;

/*!
  tag_base_cmd_spec
  */
typedef enum tag_base_cmd_spec
{
  /*!
      xxxxxxxx
    */
  LINE_RETURN = 0,
  /*!
      xxxxxxxx
    */
  LINE_FORCE_RETURN,
  /*!
      xxxxxxxx
    */
  LINE_FORCE_SPACE,
  /*!
      xxxxxxxx
    */
  LINE_TOTAL_CMD
}line_cmd_t;

/*!
      xxxxxxxx
  */
typedef struct {
    /*!
      xxxxxxxx
    */
    const char *p_name;
    /*!
      xxxxxxxx
    */
    int type;
} ssa_line_t;

/*!
      xxxxxxxx
    */
static const ssa_line_t ssa_line_list[] =
{
  { "[Script Info]",              LINE_SCRIPT_INFO},
  { "Title:",                        LINE_TITLE         },
  {"Original Script:",            LINE_ORIGINAL_SCRIPT},
  { "Original Translation:",     LINE_ORIGINAL_TRANS},
  { "Original Timing:",          LINE_ORIGINAL_TIMING},
  {"Synch Point:",              LINE_ORIGINAL_POINT},
  {"ScriptType:",               LINE_SCRIPT_TYPE},
  {"Collisions:",                 LINE_COLLISIONS},
  {"PlayResX:",                 LINE_RESX},
  {"PlayResY:",                 LINE_RESY},
  {"Timer:",                      LINE_TIMER},
  {"Format:",                     LINE_FORMAT},
  {"Style:",                      LINE_STYLE},
  {"[Events]",                  LINE_EVENT},
  {"Dialogue:",                 LINE_DIALOGUE},
};

/*!
      xxxxxxxx
    */
static const ssa_line_t ssa_cmd_list[] =
{
 { "n",              LINE_RETURN},
 { "N",              LINE_FORCE_RETURN},
 { "h",              LINE_FORCE_SPACE},
};


#endif
