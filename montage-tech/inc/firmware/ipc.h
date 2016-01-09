/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __IPC_H__
#define __IPC_H__
/*!
  test case ipc for av
  */
#define AV_TESTCASE_IPC 0x80001001
/*!
  test case ipc for ap
  */
#define AP_TESTCASE_IPC 0x80001002
/*!
  test case msg id for video
  */
#define TEST_VIDEO_MSG_ID 0x000A
/*!
  test case msg id for audio
  */
#define TEST_AUDIO_MSG_ID 0x000B


/*!
   AP_CMD_ID
  */
#define AP_CMD_ID 0xA000A000
/*!
  AV_CMD_ID
  */
#define AV_CMD_ID 0xB000B000


/*!
  ap ipc regs 
  */
#define AP_CPU_INT (*(volatile unsigned int *)0xbfe7001c)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_CMD0 (*(volatile unsigned int *)0xbfe70000)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_CMD1 (*(volatile unsigned int *)0xbfe70004)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_CMD2 (*(volatile unsigned int *)0xbfe70008)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_CMD3 (*(volatile unsigned int *)0xbfe7000C)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_DATA (*(volatile unsigned int *)0xbfe70010)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_CMD_READY (*(volatile unsigned int *)0xbfe70014)
/*!
  ap ipc regs 
  */
#define AV_TO_AP_ACT_DONE (*(volatile unsigned int *)0xbfe70018)
/*!
  av ipc regs 
  */
#define AP_TO_AV_CMD0 (*(volatile unsigned int *)0xbfd00000)
/*!
  av ipc regs 
  */
#define AV_CPU_INT (*(volatile unsigned int *)0xbfd0001c)
/*!
  av ipc regs 
  */
#define AP_TO_AV_CMD1 (*(volatile unsigned int *)0xbfd00004)
/*!
  av ipc regs 
  */
#define AP_TO_AV_CMD2 (*(volatile unsigned int *)0xbfd00008)
/*!
  av ipc regs 
  */
#define AP_TO_AV_CMD3 (*(volatile unsigned int *)0xbfd0000C)
/*!
  av ipc regs 
  */
#define AP_TO_AV_DATA (*(volatile unsigned int *)0xbfd00010)
/*!
  av ipc regs 
  */
#define AP_TO_AV_CMD_READY (*(volatile unsigned int *)0xbfd00014)
/*!
  av ipc regs 
  */
#define AP_TO_AV_ACT_DONE (*(volatile unsigned int *)0xbfd00018)

/*!
  ap cpu magic word 
  */
#define AP_CPU_READY_MAGIC (0x12345678)
/*!
  av cpu magic word 
  */
#define AV_CPU_READY_MAGIC (0x87654321)
/*!
  av_ack_flag 
  */
#define AV_ACK_FLAG (0xacacacac)
/*!
  ap_ack_flag 
  */
#define AP_ACK_FLAG (0xcacacaca)
/*!
  Message transfered between device drivers even if thay are run on different CPU 
  */
typedef struct
{
  /*!
      messege id, see dev_pipe_msg_id_t
    */ 
  u32 msg_id;
  /*!
      messege 1st parameter
    */   
  u32 param1;
  /*!
      messege 2nd parameter
    */    
  u32 param2;
  /*!
     sem
  */   
  u32 sem;
  /*!
      time out (ms)
  */  
  u32 time_out_ms;
}ipc_msg_t;
/*!
    pipe max num
  */
#define MAX_PIPE_NUM 32
/*!
    struct for ipc_pipe_t
  */
typedef struct
{
  /*!
      messege id mask
    */ 
  u32 msg_id_mask;
  /*!
      the source processor id
    */   
  u32 p_sid;
  /*!
      the destin processor id
    */   
  u32 p_did;
}ipc_pipe_t;

/*!
    struct for pipe
  */
typedef struct pipe_info
{
  /*!
      pipe  message
    */   
  ipc_msg_t *Datapipe;
  /*!
      read pointer
    */     
  int ReadPt;
  /*!
      write pointer
    */   
  int WritePt;
   /*!
      spinlock
    */   
  u32 sp_lock;
  /*!
      this pipe infor
    */   
  ipc_pipe_t pipe_info;
  /*!
      this pipe depth
    */
  int pipe_depth;
  /*!
      this pipe used or not
    */  
  u32 used;
}pipe_info_t;

/*!
    struct for Mailbox Message
  */
typedef struct
{
  /*!
      pipe  message
    */   
  ipc_msg_t p_msg;
  /*!
      processor id
    */     
  u32 local_id;
}mb_msg_t;

/*!
  check ap cpu status
  */
typedef RET_CODE (* check_ap_ready_t)(void);
/*!
  check av cpu status
  */
typedef RET_CODE (* check_av_ready_t)(void);
/*!
  ipc ap fifo init
  */
typedef void (* ap_ipc_init_t)(int max_pipe_num);
/*!
  ipc av fifo init
  */
typedef void (* av_ipc_init_t)(int max_pipe_num);
/*!
  ipc av fifo create
  */
typedef RET_CODE (* av_ipc_pipe_create_t)(ipc_pipe_t *p_pipe_t,int pipe_depth);
/*!
  ipc ap fifo create
  */
typedef RET_CODE (* ap_ipc_pipe_create_t)(ipc_pipe_t *p_pipe_t,int pipe_depth);
/*!
  ap send to av
  */
typedef RET_CODE (* ap_send_to_av_t)(u32 local_id, ipc_msg_t *p_msg, u8 ack_flag);
/*!
  av send to ap
  */
typedef RET_CODE (* av_send_to_ap_t)(u32 local_id, ipc_msg_t *p_msg, u8 ack_flag);
/*!
  ap received from av
  */
typedef RET_CODE (* ap_recv_from_av_t)(u32 local_id, ipc_msg_t *p_msg);
/*!
  av received from ap
  */
typedef RET_CODE (* av_recv_from_ap_t)(u32 local_id, ipc_msg_t *p_msg);
/*!
  ap send to ap
  */
typedef RET_CODE (* ap_send_to_ap_t)(u32 local_id, ipc_msg_t *p_msg);
/*!
  av send to av
  */
typedef RET_CODE (* av_send_to_av_t)(u32 local_id, ipc_msg_t *p_msg);
/*!
  ap received from ap
  */
typedef RET_CODE (* ap_recv_from_ap_t)(u32 local_id, ipc_msg_t *p_msg);
/*!
  av received from av
  */
typedef RET_CODE (* av_recv_from_av_t)(u32 local_id, ipc_msg_t *p_msg);


/*!
  ap received from ap
  */
typedef void (* ap_recv_down_ack_t)(void);
/*!
  av received from av
  */
typedef void (* av_recv_down_ack_t)(ipc_msg_t *p_msg);

/*!
  av received from av
  */
typedef struct
{
/*!
  av ap ipc function
  */
  CACHE_ALIGN check_ap_ready_t     check_ap_ready_set;
  /*!
  av ap ipc function
  */                    
  CACHE_ALIGN check_av_ready_t     check_av_ready_set;
 /*!
  av ap ipc function
  */                     
  CACHE_ALIGN ap_ipc_init_t        ap_ipc_init_set;
  /*!
  av ap ipc function
  */                    
  CACHE_ALIGN av_ipc_init_t        av_ipc_init_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN av_ipc_pipe_create_t av_ipc_pipe_create_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_ipc_pipe_create_t ap_ipc_pipe_create_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_send_to_av_t      ap_send_to_av_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN av_send_to_ap_t      av_send_to_ap_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_recv_from_av_t    ap_recv_from_av_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN av_recv_from_ap_t    av_recv_from_ap_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_send_to_ap_t      ap_send_to_ap_set;
    /*!
  av ap ipc function
  */                  
  CACHE_ALIGN av_send_to_av_t      av_send_to_av_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_recv_from_ap_t    ap_recv_from_ap_set;
  /*!
  av ap ipc function
  */                    
  CACHE_ALIGN av_recv_from_av_t    av_recv_from_av_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN ap_recv_down_ack_t   ap_recv_down_ack_set;
   /*!
  av ap ipc function
  */                   
  CACHE_ALIGN av_recv_down_ack_t   av_recv_down_ack_set;
                      
}ipc_fw_fun_set_t;


/*!
  check ap cpu status
  */
RET_CODE check_ap_ready(void);
/*!
  check av cpu status
  */
RET_CODE check_av_ready(void);
/*!
  ipc ap fifo init
  */
void ap_ipc_init(int max_pipe_num);
/*!
  ipc av fifo init
  */
void av_ipc_init(int max_pipe_num);
/*!
  ipc av fifo create
  */
RET_CODE av_ipc_pipe_create(ipc_pipe_t *p_pipe_t,int pipe_depth);
/*!
  ipc ap fifo create
  */
RET_CODE ap_ipc_pipe_create(ipc_pipe_t *p_pipe_t,int pipe_depth);
/*!
  ap send to av
  */
RET_CODE ap_send_to_av(u32 local_id, ipc_msg_t *p_msg, u8 ack_flag);
/*!
  av send to ap
  */
RET_CODE av_send_to_ap(u32 local_id, ipc_msg_t *p_msg, u8 ack_flag);
/*!
  ap received from av
  */
RET_CODE ap_recv_from_av(u32 local_id, ipc_msg_t *p_msg);
/*!
  av received from ap
  */
RET_CODE av_recv_from_ap(u32 local_id, ipc_msg_t *p_msg);
/*!
  ap send to ap
  */
RET_CODE ap_send_to_ap(u32 local_id, ipc_msg_t *p_msg);
/*!
  av send to av
  */
RET_CODE av_send_to_av(u32 local_id, ipc_msg_t *p_msg);
/*!
  ap received from ap
  */
RET_CODE ap_recv_from_ap(u32 local_id, ipc_msg_t *p_msg);
/*!
  av received from av
  */
RET_CODE av_recv_from_av(u32 local_id, ipc_msg_t *p_msg);


/*!
  ap received from ap
  */
void ap_recv_down_ack(void);
/*!
  av received from av
  */
void av_recv_down_ack(ipc_msg_t *p_msg);
/*!
  av received from av
  */
//void ipc_fw_attach(void);
#endif //__IPC_H__
