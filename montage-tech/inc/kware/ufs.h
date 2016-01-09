/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UFS_H_
#define __UFS_H_

#ifdef __cplusplus
extern "C" 
{
  #endif    

/*!
   Physical drive# on the Windows mapped as drive 0
  */
  #define PHY_DRV_START 1
/*!
    Last physical drives to be mapped
  */
  #define PHY_DRV_END 5
/*!
   device limit
  */
  #define DEVICE_MAX  (PHY_DRV_END - PHY_DRV_START)
/*!
   logic parttion limit
  */
  #define LOGIC_PARTTION  ((DEVICE_MAX) * 4)
  #if 0

  /*!
   ufs use unicode
    */
  #define UFS_UNICODE 1

  /*!
    to do
    */
  #if UFS_UNICODE
  /*!
    to do
    */
  typedef u16 tchar_t;
  /*!
    to do
    */
  #define _T(x) L ## x
  /*!
    to do
    */
  #define _TEXT(x) L ## x
  /*!
    to do
    */
  #define _INC_TCHAR
  #else
  /*!
    to do
    */
  typedef u8 tchar_t;
  /*!
    to do
    */
  #define _T(x)   ## x
  /*!
    to do
    */
  #define _TEXT(x)  ## x
  /*!
    to do
    */
  #define _INC_TCHAR
  #endif

#endif

    /*!
    error id
    */
#define  ERROR_ID 0xFF
    /*!
    max partition table for ufs
    */
#define PATITION_TABLE_MAX    8
    /*!
     ufs monitor max
    */
#define UFS_MONITOR_MAX 50

    /*!
    FS format time definition
    */
  typedef struct
  {
    /*!
      Year
      */
    u16 year;
    /*!
      Month
      */
    u8 month;
    /*!
      Day
      */
    u8 day;
    /*!
      Hour
      */
    u8 hour;
    /*!
      Minute
      */
    u8 minute;
    /*!
      Sec
      */
    u8 second;
    /*!
      researved.
      */
    u8 reserved;
  } fs_time_t;


  /*!
  fs return code
    */
  typedef enum 
  {
    /*!
    different fs,different FILE struct
    */
    UFS_OK = 0,   
    /*!
     (1) A hard error occured in the low level disk I/O laye
    */
    UFS_DISK_ERR,   
    /*!
    (2) Assertion failed
    */
    UFS_INT_ERR,   
    /*!
    (3) The physical drive cannot work 
    */
    UFS_NOT_READY,  
    /*!
    (4) Could not find the file
    */
    UFS_NO_FILE,    
    /*!
    (5) Could not find the path
    */
    UFS_NO_PATH,    
    /*!
    (6) The path name format is invalid
    */
    UFS_INVALID_NAME,  
    /*!
    (7) Acces denied due to prohibited access or directory full
    */
    UFS_DENIED,   
    /*!
    (8) Acces denied due to prohibited access
    */
    UFS_EXIST,     
    /*!
    (9) The file/directory object is invalid
    */
    UFS_INVALID_OBJECT,   
    /*!
     (10) The physical drive is write protected
    */
    UFS_WRITE_PROTECTED, 
    /*!
    (11) The logical drive number is invalid
    */
    UFS_INVALID_DRIVE,   
    /*!
     (12) The volume has no work area
    */
    UFS_NOT_ENABLED,     
    /*!
    (13) There is no valid FAT volume on the physical drive
    */
    UFS_NO_FILESYSTEM, 
    /*!
     (14) The f_mkfs() aborted due to any parameter error 
    */
    UFS_MKFS_ABORTED,    
    /*!
    (15) Could not get a grant to access the volume within defined period 
    */
    UFS_TIMEOUT,       
    /*!
    (16) The operation is rejected according to the file shareing policy
    */
    UFS_LOCKED,      
    /*!
    (17) LFN working buffer could not be allocated
    */
    UFS_NOT_ENOUGH_CORE, 
    /*!
    (18) Number of open files > _FS_SHARE 
    */
    UFS_TOO_MANY_OPEN_FILES  
  } UFS_FRESULT;


  /*!
    different fs,different FILE struct
    */
  typedef struct _ufs_file
  {
    /*!
    virtual file system
    */
    void *vfs;
    /*!
    public attribute
    */
    u64  file_size;
    /*!
    0 is not used  1-FATMAX is fat file, FATMAX+1 is other
    */
    u32 file_id;
    /*!
     logic parttion: 0->LOGIC_PARTTION
    */
    u8 use_logic;
  }ufs_file_t;

  /*!
    UFILE denifition
    */
  typedef ufs_file_t UFILE;

  /*!
    dir of file system
    */
  typedef struct
  {
    /*!
    for dir
    */
    void *dir;
    /*!
    dir name, for ntfs compatibility
    */
    tchar_t name[256];
    /*!
     logic parttion
    */
    u8 use_logic;
    /*!
     file num need to read in read_dir interface
    */
    u32 file_num;
  }ufs_dir_t;
  /*!
   ufs_infor
   */
  typedef struct ufs_infor
  {
    /*!
    File size
    */
    u32 fsize;
    /*!
    Last modified date
    */
    u16  fdate;
    /*!
    Last modified time
    */
    u16  ftime;
    /*!
    Last modified time
    */
    u16  year;
    /*!
    Last modified time
    */
    u8  month;
    /*!
    Last modified time
    */
    u8  day;
    /*!
    Last modified time
    */
    u8  hour;
    /*!
    Last modified time
    */
    u8  minute;
    /*!
    Last modified time
    */
    u8  second;
    /*!
    Attribute
    */
    u8  fattrib;
    /*!
    Short file name (8.3 format)
    */
    tchar_t fname[13];
    /*!
    Pointer to the LFN buffer
    */
    tchar_t *lfname;
    /*!
    Size of LFN buffer in tchar_t
    */
    u32  lfsize;
  } ufs_infor_t;

  /*!
    file information
    */
  typedef struct
  {
    /*!
    ufs information
    */
    ufs_infor_t fileinfo;


  }ufs_fileinfo_t;

  /*!
    file system functions
    */
  typedef struct file_operations
  {
  /*!
   seek position
  */
    u32 (*fs_lseek) (ufs_file_t *p_fp, s64 ofs, u32 frm);
  /*!
   tell position
  */
    u32 (*fs_tell) (ufs_file_t *p_fp, u64 *p_ofs);
  /*!
  read file
  */
    u32 (*fs_read) (ufs_file_t *p_fp, void *p_buff, u32 btr, u32 *p_br);
  /*!
   write file
  */
    u32 (*fs_write) (ufs_file_t *p_fp, void *p_buff, u32 btr, u32 *p_br);
  /*!
   open dir
  */
    u32 (*fs_opendir) (ufs_dir_t *p_dj, tchar_t *p_path);
  /*!
   read dir
  */
    u32 (*fs_readdir) (ufs_dir_t *p_dj, void *p_fno);
  /*!
    open file
  */
    u32 (*fs_open) (ufs_file_t *p_fp, const tchar_t *p_path, u8 mode);
  /*!
   close file
  */
    u32 (*fs_close) (ufs_file_t *p_fp);
  /*!
   delete file
  */
    u32 (*fs_delete) (const tchar_t *p_path, u32 file_num);
  /*!
  rename file
  */
    u32 (*fs_rename) (const tchar_t *p_old, const tchar_t *p_new);
  /*!
  save file
  */
    u32 (*fs_flush) (ufs_file_t *p_fp);
  /*!
   other operation
  */
    u32 (*fs_ioctl) (void *p_fp, u8 cmd,  void *p_para0, void *p_para1, void *p_para2);
  /*!
   close dir
  */
    void (*fs_closedir) (ufs_dir_t *p_dj);
  }file_operations_t;

  /*!
   file operator mode
    */
  typedef enum  
  {
  /*!
    open exit
    */
    UFS_OPEN = 0x00,
  /*!
    read
     */
    UFS_READ = 0x01,
   /*!
    write
    */
    UFS_WRITE = 0x02,
   /*!
    create new
    */
    UFS_CREATE_NEW = 0x04,
   /*!
    create new, if exit the file, cover it.
    */
    UFS_CREATE_NEW_COVER = 0x08,

   /*!
    add for large file which beyond 512 k , same as that in ramfs.h
    */
    UFS_CREATE_LARGE_FILE = 0x10,  
  }op_mode_t;

  /*!
    ufs seek opt
  */
  typedef enum 
  {
  /*!
     from head
   */
    UFS_SEEK_HEAD = 0,
  /*!
     from current pos
   */
    UFS_SEEK_CUR = 0xFFFFFFFE,
     
  /*!
      from end
    */
    UFS_SEEK_END = 0xFFFFFFFF,
  }ufs_seek_opt;

  /*!
    mount map to device
    */
  typedef enum  
  {
    /*!
    unuse
    */
    UNMOUNT_DEV,
    /*!
    map to device
    */
    MOUNTED_LINKED,
    /*!
    reserved
    */
    MOUNT_RESERVED
  }mount_flag_t;
  /*!
    file system flag
    */
  typedef enum  
  {
  /*!
     fat 32
   */
    FL_DEFAULT_32 = 0,
  /*!
     fat 32
   */
    FL_FAT_12 = 0x01,
  /*!
    fat 16
   */
    FL_FAT_16_32M = 0x04,
  /*!
   fat 16
   */
    FL_FAT_16 = 0x06,
  /*!
    NTFS
   */
    FL_NTFS = 0x07,
  /*!
    fat 32
   */
    FL_WIN95_FAT_32_0 = 0x0B,
  /*!
    fat 32
   */
    FL_WIN95_FAT_32_1  = 0x0C,
     /*!
    fat 32
   */
    FL_WIN95_FAT_16_2  = 0x0E,
  /*!
   fat 32
   */
    FL_HINDDEN_FAT_32_0 = 0x1B,
  /*!
   fat 32
   */
    FL_HINDDEN_FAT_32_1 = 0x1C,
  /*!
    fat 32:linux or fat32???
   */
    FL_LINUX_FAT_32_0 = 0x83,

  /*!
   virtual device 
   */
    FL_VIRTUAL = 0xFE,
  }fl_sys_type_t;


  /*!
     device event
    */
  typedef enum  
  {
    /*!
    insert
    */
    INSERT_DEV = 1,
    /*!
    device  plug
    */
    PLUG_DEV,
    /*!
    reserved
    */
    RESERVED_DEV
  }dev_event_t;
  /*!
     partion table
    */
  typedef struct _parttion_table
  {
    /*!
    partion id 1bytes: C,D...
    */
    u8   partion_id;
    /*!
    start address  3bytes
    */
    u8   start_chs[3];
    /*!
    file system type 1bytes
    */
    u8   system_id;
    /*!
    end address  3bytes
    */
    u8   end_chs[3];
    /*!
    start logic sector  4bytes
    */
    u32  relative_sectors;
    /*!
    total sector  4bytes
    */
    u32  total_sectors;
    /*!
    free bytes  4bytes; KB
    */
    u32  free_size;
    /*!
    total bytes  4bytes; KB
    */
    u32  total_size;
    /*!
    fs_type; refert to fl_sys_type_t for detail
    */
    u32  fs_type;
    /*!
    parttion table number
    */
    u8 table_num;
    /*!
    mbr or ebr position in sect 
    */
    u32  partpos;   

  }parttion_table_t;
  /*!
     device partion
    */
  typedef struct _dev_part_map
  {
    /*!
    device number
    */
    u8 dev_id;
    /*!
    device handle
    */
    drv_dev_t *p_dev;
    /*!
    parttions for every device : not more than 8 (include extende partition)
    */
    tchar_t parttion[PATITION_TABLE_MAX];  //4
    /*!
    parttion infor table
    */
    parttion_table_t parttion_table[PATITION_TABLE_MAX];
  }dev_part_map_t;

  /*!
  device hanlde map to id
    */
  typedef struct _device_id_map
  {
    /*!
    id
    */
    u8 device_id;
    /*!
    device handle
    */
    drv_dev_t *p_dev;
    /*!
    flag
    */
    BOOL id_use;
  }device_id_map_t;

  /*!
     logic parttion
    */
  typedef struct _ufs_logic
  {
    /*!
    device number
    */
    u8 dev_id;

    /*!
    file system type
    */
    u8 fstype;

    /*!
    logic parttion
    */
    tchar_t parttion;

    /*!
    device handle
    */
    drv_dev_t *p_dev;


    /*!
    file system functions struct
    */
    file_operations_t fsoperate_t;
    /*!
    valid parttion or not
    */
    mount_flag_t mount_flag;

    /*!
     partition sect lenth
    */
    u32 total_sectors;

    /*!
    mbr or ebr position in sect 
    */
    u32  partpos;   

    /*!
    curent fs start position  in sect 
    */
    u32  fspos;

    /*!
    curent fs partition table entry in mbr or ebr 
    */
    u32  pte;
  }ufs_logic_t;


  /*!
     device event detail infor
    */
  typedef struct _notify_devic_infor
  {
    /*!
    device name
    */
    void *p_dev_name;
    /*!
    device event
    */
    dev_event_t event;

  }notify_devic_infor_t;
  /*!
     usb device event infor
    */
  typedef struct _ufs_devic_event
  {
   /*!
     usb device event infor
    */
    notify_devic_infor_t device_event[DEVICE_MAX];
    /*!
     usb device event count
    */
    u8 event_dev_count;
  }ufs_devic_event_t;
  /*!
     device fs status monitor
    */
  typedef struct _ufs_devic_monitor
  {
    /*!
    device event
    */
    u32 ufs_address;
    /*!
    parttion
    */
    u8 parttion;
    /*!
    device id
    */
    u8 id;
  }ufs_devic_monitor_t;

  /*!
     callback for config func to get current time
    */
  typedef struct ufs_gettime_notify
  {
    /*!
     callback for config func to get current time
     p_time is fs_time_t, and b_time is 0
    */
    BOOL (*get_time)(u32 p_time, BOOL b_time);
  }ufs_gettime_notify_t;
/*!
  set sys time func to file system
  filesys_time_get:  u32 p_time, BOOL b_time
  */
  extern void set_filesys_timefunc(u32 filesys_time_get);

  /*!
    seek file
    fp: Operate file pointer
    ofs: File pointer from top of file
    frm:from where
    */
  extern u8 ufs_lseek(ufs_file_t *p_fp, s64 ofs, u32 frm);

  /*!
    tell file: get current file r/w position
    fp: Operate file pointer
    ofs: File pointer from top of file
    */
  extern u8 ufs_tell(ufs_file_t *p_fp, u64 *p_ofs);



  /*!
    fp: Pointer to the file object
    buff: Pointer to data buffer
    btr: Number of bytes to read
    br: Pointer to number of bytes read
    */
  extern u8 ufs_read(ufs_file_t *p_fp, void *p_buff, u32 btr, u32 *p_br);
  /*!
    Write File
    fp: Pointer to the file object
    buff: Pointer to the data to be written
    btw: Number of bytes to write
    bw: Pointer to number of bytes written
    */
  extern u8 ufs_write(ufs_file_t *p_fp, void *p_buff, u32 btr, u32 *p_br);
  /*!
    Open/Create a Directroy Object
    dj: Pointer to directory object to create
    path: Pointer to the directory path
    */
  extern u8 ufs_opendir (ufs_dir_t *p_dj, tchar_t *p_path);
  /*!
    close a Directroy Object
    dj: Pointer to directory object to create
    */
  extern void ufs_closedir (ufs_dir_t *p_dj);
  /*!
    Read Directory Entry in Sequense
    dj: Pointer to the open directory object
    fno: Pointer to file information to return
    */
  extern u8 ufs_readdir (ufs_dir_t *p_dj, ufs_fileinfo_t *p_fno);
  /*!
    Open or Create a File.

    parttion_l : device partition
    fs_type : fs type
    sfd : sfd
    au : au
    */
  extern u8 ufs_format(tchar_t parttion_l, u8 fs_type, u8 sfd, u32 au);
  /*!
    Open or Create a File
    fp: Pointer to the blank file object
    path: Pointer to the file name
    mode: Access mode and file open mode flags
    */
  extern u8 ufs_open(ufs_file_t *p_fp, const tchar_t *p_path, op_mode_t mode);
  /*!
    Close File
    Pointer to the file object to be closed
    */
  extern u8 ufs_close (ufs_file_t *p_fp);
  /*!
    save File
    Pointer to the file object to be closed
    */
  extern u8 ufs_flush (ufs_file_t *p_fp);
  /*!
    Close File
    Pointer to the old  file
    Pointer to the new  file
    */
  extern u8 ufs_rename (const tchar_t *p_old, const tchar_t *p_new);
  /*!
  Delete a File or Directory
  path: Pointer to the file or directory path
  file_num: if directory, indecrate the max file list number,
            if give 0, use default max number 100
    */
  extern u8 ufs_delete (const tchar_t *p_path, u32 file_num);
  /*!
    Control file system.
    fp: Pointer to the blank file object
    cmd: control command. please refer to fsioctl.h: command mount fs
    para1: point to the first parameter.
    para2: point to the second parameter.
    */
  extern u8 ufs_ioctl (void *p_fp, u32 cmd, void *p_para0, \
                  void *p_para1, void *p_para2);
  /*!
    format logic parttition
    parttion_l: parttition
    fs_type: file type
    sfd:  Partitioning rule 0:FDISK, 1:SFD
    au: Allocation unit size [bytes] 512
    */
  extern u8 ufs_format(tchar_t parttion_l, u8 fs_type, u8 sfd, u32 au);
  /*!
    chang parttion
    letter: parttions. (base parttion is 'C': 'C', 'D', 'E'....)
    */
  extern u8 ufs_chdrive(tchar_t letter);
  /*!
    show current path
    p_path: the path
    */
  extern u8 ufs_get_path(tchar_t *p_path);
  /*!
    get current volume fs type
    */
  extern u8 ufs_get_fstype(void);
  /*!
    regist device (only add device, do not allocate parttions)
    dev: device handle
    */
  extern u8 regist_device(struct drv_dev *p_dev);
  /*!
    according dev handle  to  get id (0,1,2....)
    dev: device handle
    */
  extern u8 get_device_id(struct drv_dev *p_dev);
  /*!
    delete device, and give up parttions
    dev: device handle
    art_table: get parttion letter list
    num: pointer to parttion number
    */
  extern u8 unregist_device(struct drv_dev *p_dev, tchar_t *part_table, u8 *num);
  /*!
    add new device, and allocate partions.
    part_table: get parttion letter list
    num: pointer to parttion number
    */
  extern u8 regist_new_device(struct drv_dev *p_dev, tchar_t *part_table, u8 *num);
  /*!
    allocate parttions for all device, and please add device first.
    art_table: get parttion letter list
    num: pointer to parttion number
    */
  extern u8 regist_logic_parttion(tchar_t *part_table, u8 *num);

  /*!
    add new device, and allocate partions. max device and max parttion is for virtual device
    part_table: get parttion letter list
    num: pointer to parttion number
    ops: file system operations
    return: 0 success, other failed
    */
  extern u8 regist_virtual_device(struct drv_dev *p_dev, u8 *part_table, u8 *num, 
                                                          file_operations_t *ops);

  /*!
    delete device, and give up parttions, max device and max parttion is for virtual device
    dev: device handle
    art_table: get parttion letter list
    num: pointer to parttion number
    return: 0 success, other failed
    */
  extern u8 unregist_virtual_device(struct drv_dev *p_dev, u8 *part_table, u8 *num);

  /*!
  return event struct
   */
  extern BOOL ufs_get_event(notify_devic_infor_t *p_event, u8 *event_dev_count);
  /*!
  init ufs:
   */
  extern u8 ufs_dev_init();
  /*!
  get parttion infor
    */
  extern u8 ufs_get_part_infor(tchar_t parttion, parttion_table_t *infor);


  /*!
     ustrchr 
    */
  extern tchar_t *ustrchr(tchar_t *str,   tchar_t ch);

  /*!
     ustrsep 
    */
  extern tchar_t *ustrsep(tchar_t **stringp,  const tchar_t *delim);

  /*!
     ustrlen 
    */
  extern u32 ustrlen(const tchar_t *src);

  /*!
     ustrcat 
    */
  extern tchar_t *ustrcat(tchar_t *dst, const tchar_t *src);

  /*!
     ustrcpy 
    */
  extern tchar_t *ustrcpy(tchar_t *dst, const tchar_t *src);


  /*!
     ufs_asc2uni 
     [in] p_ascstr : simple ascstr
     [out]p_unistr : the tchar string, the buffer need allocated first
    */
  u32 ufs_asc2uni(u8 *p_ascstr,  tchar_t *p_unistr);


  /*!
     ufs_uni2asc:  convert tchar_t to ascstr
     [out] p_ascstr : simple ascstr the buffer need allocated first
     [in]p_unistr : the tchar string, the buffer need allocated first
    */
  u32 ufs_uni2asc(u8 *p_ascstr, const tchar_t *p_unistr);

  /*!
     ufs_utf16_to_utf8 - convert a little endian UTF16LE string to an UTF-8 string
     [in]p_ins:	input utf16 string buffer
     [in]ins_len:	length of input string in utf16 characters 0 ==4096
     [in]p_outs:	on return contains the (allocated) output multibyte string
     [out]p_outs_len:	length of output buffer in bytes
     Return -1 with  if string has invalid byte sequence or too long.
    */
  int ufs_utf16_to_utf8(u16 *p_ins,   int ins_len,
                           u8  *p_outs, u32 *p_outs_len);

  /*!
     ntfs_utf8_to_utf16 - convert a UTF-8 string to a UTF-16LE string
     [in]p_ins:	input multibyte string buffer
     [in]p_outs:	on return contains the (allocated) output utf16 string
     [in/out]p_out_len:	length of output buffer in utf16 characters
     Return  -1 for fail, > 0  cnt.
    */
  int  ufs_utf8_to_utf16(u8 *p_ins, u16 *p_outs, u32 *p_out_len);

  #ifdef __cplusplus
}
#endif

#endif

