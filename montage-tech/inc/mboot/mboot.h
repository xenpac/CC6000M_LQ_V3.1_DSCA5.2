/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MBOOT_H__
#define __MBOOT_H__

/*!
  define the Mboot version (format: x.yy mult by 100)
  */
#define MBOOT_VERSION  100

/*!
  define the max length for each commands
  */
#define CMD_MAX_LEN            256
/*!
  define the max argument count for each commands
  */
#define CMD_MAX_ARGS           10
/*!
  define the max number of supported commands
  */
#define MBOOT_MAX_CMD          20


/*!
  define the mboot command execute function
  */
typedef s32 (*mfunc)(u32 argc, u8 **argv);
/*!
  define the mboot commmad structure
  */
typedef struct
{
  /*!
    cmd name
    */
  u8 *p_name;
  /*!
    cmd usage
    */  
  u8 *p_usage;
  /*!
    cmd help info
    */  
  u8 *p_help;
  /*!
    cmd execute function
    */  
  mfunc func;
}mbcmd_t;


/*!
 Block information of data manager header
 */
typedef struct tag_dmh_block_info
{
  /*!
    Block id
    */
  u8 id;
  /*!
    Block type refer to enum tag_block_type
    */
  u8 type;
  /*!
    Node number in current block
    */
  u16 node_num;
  /*!
    Base address of current block
    */
  u32 base_addr;
  /*!
    Block size
    */
  u32 size;
  /*!
    CRC byte
    */
  u32 crc;
  /*!
    Version info of this block
    */
  u8 version[8];
  /*!
    Name of this block
    */
  u8 name[8];
  /*!
    Time of the last version of this block
    */
  u8 time[12];
  /*!
    Reserve
    */
  u16 ota_ver;
  /*!
    Reserve
    */
  u16 reserve;
}dmh_block_info_t;

/*!
  warning msg routine
  */
#define DM_BLOCK_SIZE (sizeof(dmh_block_info_t))
/*!
  warning msg routine
  */
#define DM_GET_U8(x) (*((volatile u8 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_GET_U16(x) (*((volatile u16 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_GET_U32(x) (*((volatile u32 *)(x + 0x8000000)))
/*!
  warning msg routine
  */
#define DM_HEADER_SHIFT (16 + 12)

/*!
  DM_GET_BLOCK_ID
  */
#define DM_GET_BLOCK_ID(block_id, h_addr, i)                                          \
do{                                                                           \
    (block_id) =  (*((volatile u32 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 0x8000000)));  \
}while(0)

/*!
  DM_GET_ADDR
  */
#define DM_GET_ADDR(addr, h_addr, i)                                          \
do{                                                                           \
    (addr) =  (*((volatile u32 *) \
    ((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 4 + 0x8000000)));  \
    (addr) += (h_addr + 0x8000000);                                                       \
}while(0)

/*!
  DM_GET_SIZE
  */
#define DM_GET_SIZE(size, h_addr, i)                                          \
do{                                                                           \
    (size) = (*((volatile u32 *)((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 8 + 0x8000000)));\
}while(0)

/*!
  DM_GET_CRC
  */
#define DM_GET_CRC(crc, h_addr, i)                                            \
do{                                                                           \
    (crc) = (*((volatile u32 *)((h_addr) + DM_HEADER_SHIFT + i * DM_BLOCK_SIZE + 12 + 0x8000000)));\
}while(0)

/*!
  DM_GET_BLOCK_NUM
  */
#define DM_GET_BLOCK_NUM(num, h_addr)                     \
do{                                                       \
  (num) = (*((volatile u16 *)((h_addr) + 16 + 8 + 0x8000000)));                  \
}while(0)

/*!
  DM_GET_FIRST_BLOCK_HEAD_ADDR
  */
#define DM_GET_FIRST_BLOCK_HEAD_ADDR(num, h_addr)                     \
do{                                                       \
  (num) = ((h_addr) + 16 + 8 + 4 + 0x8000000);                  \
}while(0)


///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///      These functions should be implemented by project
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*!
  Board relative initialation
  
  \param[in] param1 not defined yet
  \param[in] param2 not defined yet

  \return SUCCESS if OK, else fail
  */
BOOL sys_board_init(u32 param1, u32 param2);

/*!
  Board relative initialation
  
  \param[in] param1 not defined yet
  \param[in] param2 not defined yet

  \return SUCCESS if OK, else fail
  */
BOOL sys_env_init(u32 param1, u32 param2);

/*!
  Add the commands supported to command list
  
  \param[in] None
  */
void mboot_cmd_setup(void);

/*!
  Run the command in sequence
  
  \param[in] None
  */
void mboot_auto_run(void);



///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///       APIs provided by mboot                      
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*!
  Register the command to command list
  
  \param[in] p_cmd pointer to the command

  \return SUCCESS if OK, else fail
  */
s32 mboot_register_cmd(mbcmd_t *p_cmd);

/*!
  Execute specific command
  
  \param[in] p_cmd the command string to execute

  \return SUCCESS if OK, else fail
  */
s32 mboot_run_cmd(u8 *p_cmd);
#endif //end of __MBOOT_H__
