/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __VFS_H__
#define __VFS_H__
#ifdef __cplusplus
extern "C" {
#endif
/*!
  max file name length
  */
#define MAX_FILE_NAME 16

/*!
  hfile handle
  */
typedef void * hfile_t;

/*!
  hdir handle
  */
typedef void * hdir_t;

/*!
  structure for file information.
  */
typedef struct
{
  /*!
    file last modified date.
    */
  utc_time_t file_date;
  /*!
    file attribe.
    */
  u32 file_attr;
  /*!
    file size
    */
  u32 file_size;
  /*!
    file name
    */
  u16 file_name[MAX_FILE_NAME];
}file_info_t;   


/*!
  open exist.
  */
#define VFS_OPEN    0x0
/*!
  open for read.
  */
#define VFS_READ    0x01
/*!
  open for write.
  */
#define VFS_WRITE   0x02
/*!
  open new file.
  */
#define VFS_NEW     0x04
/*!
  shared open file
  */
#define VFS_SHARE   0x08

/*!
  enum for file system type
  */
typedef enum
{
  /*!
    fat 16
    */
  VFS_FAT16,
  /*!
    fat 32
    */
  VFS_FAT32,    
  /*!
    ntfs
    */
  VFS_NTFS,
  VFS_FAT12
}vfs_fs_t;

/*!
  vfs seek mode
  */
typedef enum
{
  /*!
    seek from head.
    */
  VFS_SEEK_SET,
  /*!
    seek from tail.
    */
  VFS_SEEK_CUR,
  /*!
    seek from curn.
    */
  VFS_SEEK_END,
}vfs_seek_mode_t;

/*!
  short file name length
  */
#define FILE_NAME_LEN 256

/*!
  vfs file attr
  */
typedef enum
{
  /*!
    Read only
    */
  VFS_RDO = 0x01,
  /*!
    hide
    */
  VFS_HID = 0x02,
  /*!
    system
    */
  VFS_SYS = 0x04,
  /*!
    volume label
    */
  VFS_VOL = 0x08,
  /*!
    lfn entry
    */
  VFS_LFN = 0x0F,
  /*!
    directory
    */
  VFS_DIR = 0x10,
  /*!
    archive
    */
  VFS_ARC = 0x20,
}vfs_fattr_t;

/*!
  vfs file handle
  */
typedef struct
{
  /*!
    file size
    */
  u32 fsize;
  /*!
    last modified date.
    */
  u16 fdate;
  /*!
    last modified time.
    */
  u16 ftime;
  /*!
    attribute
    */
  u8 fattrib;
  /*!
    short file name.
    */
  u16 fname[FILE_NAME_LEN];
}vfs_dir_info_t;

/*!
  vfs file info
  */
typedef struct
{
  /*!
    ufs file info
    */
  u64 file_size;
}vfs_file_info_t;

/*!
  vfs command
  */
typedef enum
{
  /*!
    mount
    */
  VFS_CMD_MOUNT = 0,
  /*!
    format
    */
  VFS_CMD_FORMAT,
  /*!
    change driver
    */
  VFS_CMD_CHDRIVE,
  /*!
    make directory
    */
  VFS_CMD_MKDIR,
  /*!
    change directory
    */
  VFS_CMD_CHDIR,
  /*!
    show path
    */
  VFS_CMD_SHOW_PATH,
  /*!
    seek linkmap
    */
  VFS_CMD_SEEK_LINKMAP,
}vfs_cmd_t;

/*!
   vfs device event
  */
typedef enum  {
/*!
  plug in
  */
    VFS_PLUG_IN_EVENT = 1,
/*!
  plug out
  */
    VFS_PLUG_OUT_EVENT,
/*!
   reserved  
  */
    VFS_RESERVED
}dev_evt_t;

/*!
   struct of vfs device
  */
typedef struct
{
/*!
   dev name  
  */
  u16 *p_name;
/*!
   evt id  
  */
  dev_evt_t evt;  
}vfs_dev_event_t;

/*!
   vfs_fast_state_t 
  */
typedef enum
{
/*!
   dev name  
  */
  VFS_FAST_UNKOWN = 0,
/*!
   narmal  
  */
  VFS_FAST_WRITE_NARMAL = 1<<0,
/*!
   write pos return to zero  
  */
  VFS_FAST_WRITE_BACK = 1<<1,
/*!
   write pos return to zero  
  */
  VFS_FAST_WRITE_REACH_READ = 1<<2,
}vfs_fast_state_t;

/*!
   struct of vfs device info
  */
typedef struct
{
  /*!
     total size KB.
    */
  u32 total_size;
  /*!
     free size KB.
    */
  u32 free_size;
  /*!
    partition used size KB
    */
  u32 used_size;
  /*!
    partition name
    */
  u16 parttion_name[13];
  /*!
    direct block size for fast open
    */
  u32 direct_block_size;
  /*!
     fs type.
    */
  vfs_fs_t fs_type;
}vfs_dev_info_t;

typedef void (*vfs_evt_callback)(handle_t handle, vfs_dev_event_t *p_event);

/*!
  init virtual file system

  \return NULL
  */
void vfs_mt_init(void);

/*!
  init virtual file system

  \return NULL
  */
void vfs_uri_init(void);

/*!
  vfs mount.

  \param[in] volume : volume label.

  \return NULL
  */
RET_CODE vfs_mount(u16 volume);

/*!
  vfs unmount.

  \param[in] volume : volume label.

  \return NULL
  */
RET_CODE vfs_unmount(u16 volume);

/*!
  vfs mount.

  \param[in] p_path : mount path.

  \return RET_CODE
  */
RET_CODE vfs_uri_mount(u16 *p_path);

/*!
  vfs unmount.

  \param[in] p_path : mount path.

  \return RET_CODE
  */
RET_CODE vfs_uri_unmount(u16 *p_path);

/*!
  vfs get partitions.

  \param[in] p_letters : letters.
  \param[in] length : buffer length.

  \return partition count.
  */
u8 vfs_get_partitions(u16 *p_letters, u8 length);

/*!
  vfs read.

  \param[in] p_buf : buffer.
  \param[in] count : count.
  \param[in] size : size.
  \param[in] file : file handle.

  \return read length.
  */
u32 vfs_read(void * p_buf, u32 count, u32 size, hfile_t file);

/*!
  vfs open.

  \param[in] p_path : file path.
  \param[in] mode : mode.

  \return file handle.
  */
hfile_t vfs_open(u16 *p_path, u32 mode);

/*!
  vfs close.

  \param[in] file : file.

  \return NULL.
  */
void vfs_close(hfile_t file);

/*!
  vfs delete.

  \param[in] p_path : path.

  \return SUCCESS.
  */
RET_CODE vfs_del(u16 *p_path);

/*!
  vfs rename.

  \param[in] p_old : path.
  \param[in] p_new : p_new.

  \return SUCCESS.
  */
RET_CODE vfs_rename(u16 *p_old, u16 *p_new);

/*!
  vfs seek.

  \param[in] file : file.
  \param[in] offset : offset.
  \param[in] mode : mode.

  \return NULL.
  */
void vfs_seek(hfile_t file, s64 offset, vfs_seek_mode_t mode);

/*!
  vfs tell.

  \param[in] file : file.

  \return file length.
  */
s64 vfs_tell(hfile_t file);

/*!
  vfs tell.

  \param[in] file : file.

  \return file length.
  */
RET_CODE vfs_flush(hfile_t file);

/*!
  vfs write.

  \param[in] p_buf : buffer.
  \param[in] count : count.
  \param[in] size : size.
  \param[in] file : file handle.

  \return write length.
  */
u32 vfs_write(void *p_buf, u32 size, u32 count, hfile_t file);

/*!
  vfs fast open.

  \param[in] p_path : path.
  \param[in] mode : mode.
  \param[in] len : len.

  \return file handle.
  */
hfile_t vfs_fast_open(u16 *p_path, u32 mode, u32 len);

/*!
  vfs fast read.

  \param[in] file : file handle.
  \param[in] p_buf : buffer.
  \param[in] size : size.
  \param[out] p_ret_pos : return read pos.
  \param[out] p_state : return read state see vfs_fast_state_t.

  \return read length.
  */
u32 vfs_fast_read(hfile_t file, u8 *p_buf, u32 size, u32 *p_ret_pos, vfs_fast_state_t *p_state);

/*!
  vfs fast write.

  \param[in] file : file handle.
  \param[in] p_buf : buffer.
  \param[in] size : size.
  \param[out] p_ret_pos : return write pos.
  \param[out] p_state : return wirte state see vfs_fast_state_t.

  \return write length.
  */
u32 vfs_fast_write(hfile_t file, u8 *p_buf, u32 size, u32 *p_ret_pos, vfs_fast_state_t *p_state);

/*!
  vfs fast seek.

  \param[in] file : file handle.
  \param[in] offset : offset.

  \return SUCCESS.
  */
RET_CODE vfs_fast_seek(hfile_t file, s64 offset, vfs_seek_mode_t mode);

/*!
  vfs fast close.

  \param[in] file : file handle.

  \return.
  */
void vfs_fast_close(hfile_t file);

/*!
  vfs block size available.

  \param[in] file : path.
  \param[in] offset : len.

  \return TRUE/FALSE.
  */
BOOL vfs_is_block_size_available(u16 *p_path, u32 len);

/*!
  vfs open directory.

  \param[in] p_path : path.

  \return dir handle.
  */
hdir_t vfs_opendir(u16 *p_path, u32 max_cnt);

/*!
  vfs mkdir directory.

  \param[in] p_path : path.

  \return RET_CODE.
  */
RET_CODE vfs_mkdir(u16 *p_path);

/*!
  vfs close directory.

  \param[in] dir : dir handle.

  \return.
  */
void vfs_closedir(hdir_t dir);

/*!
  vfs read directory.

  \param[in] dir : dir handle.
  \param[in] p_info : dir info.

  \return SUCCESS.
  */
RET_CODE vfs_readdir(hdir_t dir, vfs_dir_info_t *p_info);

/*!
  vfs get file info.

  \param[in] file : file handle.
  \param[in] p_info : file info.

  \return SUCCESS.
  */
RET_CODE vfs_get_file_info(hfile_t file, vfs_file_info_t *p_info);

/*!
  vfs msg process.

  \param[in] p_event : event.

  \return TRUE or FLASE.
  */
BOOL vfs_process(handle_t handle, vfs_evt_callback evt_callback);

/*!
  vfs ioctrl.

  \param[in] file : file handle.
  \param[in] vfs_cmd : vfs command.
  \param[in] para0 : para0.
  \param[in] para1 : para1.
  \param[in] para2 : para2.

  \return SUCCESS.
  */
RET_CODE vfs_ioctrl(hfile_t file, vfs_cmd_t vfs_cmd, u32 para0, u32 para1, u32 para2);

/*!
  vfs get dev info.

  \param[in] volume : volume.
  \param[in] p_info : dev info.

  \return SUCCESS.
  */
RET_CODE vfs_get_dev_info(u16 volume, vfs_dev_info_t *p_info);

/*!
  vfs format.

  \param[in] partition : partition.
  \param[in] fs_type : fs type.

  \return SUCCESS.  
  */
RET_CODE vfs_format(u16 partition, u8 fs_type);
#ifdef __cplusplus
}
#endif
#endif
