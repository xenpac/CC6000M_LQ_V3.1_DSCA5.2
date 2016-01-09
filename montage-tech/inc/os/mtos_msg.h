/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MTOS_MSG_H__ 
#define __MTOS_MSG_H__ 

/*!
  Invaild message queue id
  */
#define INVALID_MSGQ_ID (-1)

/*!
  The max message queue number
  */
#define MTOS_MAX_MSGQ           32
/*!
  The max buffer number attached to a single message queue
  */
#define MTOS_MSGQ_MAX_BUF       32


#define MSG_WAIT_FOREVER  0

#define MSG_NO_WAIT  (-2)


/*!
  message structure, size = 12 Bytes
  */
typedef struct 
{
  /*!
    broadcast message or not
    */
  u32 is_brdcst :1;
  /*!
    if is_ext=0, that means you only want to send 12 byte using this struct
    if is_ext=1, that means you want to send more data than 12 byte.  
    */
  u32 is_ext :1;
  /*!
    if is_sync=1, this is a sync message, you need to wait the ack from the receiver
    */
  u32 is_sync :1;
  /*!
    if reserve=1,the message is inserted to the head of the queue. otherwise, to the tail
    */
  u32 reserve :1;
  /*!
    if is_transition = 1, it means the para1 is a buffer address and para2 is the buffer size.
    you need to copy the data out and call os_message_decrease_ref to tell msgq manager 
    you have gotten them and will not use them.
    */
  u32 is_transition : 1;
  /*!
    msg id
    */
  u32 content :27;
  /*!
    if is_ext=1, the para1 means the address of source buffer
    */
  u32 para1;
  /*!
    if is_ext=1, the para2 means the size of the buffer to copy
    */
  u32 para2;
  /*!
    extand data for unknown usage. Each module can define its own usage.
    */
  u32 extand_data;
  /*!
    context. Each module can define its own usage.
    */
  u32 context;
}os_msg_t;

//#ifdef DBG_MTOS_MSG   /*hrule*/
/*!
   Define the type of parsing function for a message
  */
typedef void (*msg_parser_t)(os_msg_t *p_msg);

/*!
  Set a message parser for a message queue
  
  \param[in] msgq_id the message queue index
  \param[in] p_parser pointer to the message parser, NULL means no parser.

  \return TRUE if success, else fail
  */
BOOL mtos_message_set_parser(u32 msgq_id, msg_parser_t p_parser);

//#endif


/*!
  Initiate message queue
  
  \param[in] None

  \return TRUE if success, else fail
  */
BOOL mtos_message_init(void);

/*!
  Create a message queue
  
  \param[in] depth the max message count
  \param[in] p_name the user name to create the message queue

  \return INVALID if fail, else return the message queue index
  */
u32 mtos_messageq_create(u32 depth, u8 *p_name);

/*!
  Send a message to a message queue
  
  \param[in] msgq_id the message queue index
  \param[in] p_msg pointer to the message to be sent

  \return TRUE if success, else fail
  */
BOOL mtos_messageq_send(u32 msgq_id, os_msg_t *p_msg);

/*!
  Receive a message from a message queue
  
  \param[in] msgq_id the message queue index
  \param[out] p_msg pointer to message where to store
  \param[out] ms time out value in ms

  \return TRUE if success, else fail
  */
BOOL mtos_messageq_receive(u32 msgq_id, os_msg_t *p_msg, u32 ms);

/*!
  Receive a message from a message queue without wait
  
  \param[in] msgq_id the message queue index
  \param[out] p_msg pointer to message where to store
 
  \return TRUE if success, else fail
  */
BOOL mtos_messageq_tryget(u32 msgq_id, os_msg_t *p_msg);


/*!
  Ack a message from a message queue, for synchronization mode only
  
  \param[in] msgq_id the message queue index
  */
void mtos_messageq_ack(u32 msgq_id);

/*!
  Clear all messages in a message queue
  
  \param[in] msgq_id the message queue index

  \return TRUE if success, else fail
  */
BOOL mtos_messageq_clr(u32 msgq_id);

/*!
  Relaese a message queue
  
  \param[in] msgq_id the message queue index
  */
void mtos_messageq_release(u32 msgq_id);

/*!
  Attach buffer for a message queue. 
  Extra data transfer is supported by message queue, but external buffer must be attached first.
  when sending message with extra data(is_transition=1), the data will be copied to the buffer attached here.
  Then when receiving the message, the data must be copied out and mtos_message_decrease_ref must be called 
  to notify that the message has been removed.
  The atom size is used to locate the buffer address if there are several type of extra data transfer
  in the same queue.Make SURE NOT ATTACH same atom size buffer to the same queue!
  
  \param[in] msgq_id the message queue index
  \param[in] p_buf pointer to external buffer address to be attached
  \param[in] atom_size the extra data unit size of transfered in message, MUST be different for the same queue
  \param[in] depth the max count of extra data unit supported by the buffer attached

  \return TRUE if success, else fail
  */
BOOL mtos_messageq_attach(u32 msgq_id, void *p_buf, u32 atom_size, u32 depth);

/*!
  Detach the buffer attached
  
  \param[in] msgq_id the message queue index
  \param[in] atom_size the extra data unit size, is used to find the buffer attached
  */
void mtos_messageq_detach(u32 msgq_id, u32 atom_size);

/*!
  Increase the reference count, which means one more user will process this message
  the reference count means that how many user will handle this message
  
  \param[in] msgq_id the message queue index
  \param[in] p_addr the attached buffer address
  \param[in] atom_size the atom size for attached buffer
  */
void mtos_message_increase_ref(u32 msgq_id, void *p_addr, u32 atom_size);

/*!
  Decrease the reference count, which means a user has processed this message
  the reference count means that how many user will handle this message
  
  \param[in] msgq_id the message queue index
  \param[in] p_addr the attached buffer address
  \param[in] atom_size the atom size for attached buffer
  */
void mtos_message_decrease_ref(u32 msgq_id, void *p_addr, u32 atom_size);

/*!
  Query the message queue how many messages left in current queue
  
  \param[in] msgq_id the message queue index
  \param[out] p_left how many message left in current queue
  */
void mtos_messageq_query(u32 msgq_id, u32 *p_left);

/*!
  dump the message queue content
  
  \param[in] msgq_id the message queue index
  */
void mtos_message_dump(u32 msgq_id);

#endif
