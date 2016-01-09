/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
/****************************************************************************

 ****************************************************************************/
#ifndef __UI_USB_API_H__
#define __UI_USB_API_H__

#define FLOUNDER_FLASH_SIZE  (8*KBYTES*KBYTES)
#define SKIP_SIZE (64 * KBYTES)

typedef enum
{
    UPG_ALL_CODE,
    UPG_APP_ALL,
    UPG_MAIN_CODE,
    UPG_USER_DATA,
    UPG_CA_KEY,
}upg_data_type;

typedef struct upg_file_ver
{
    u8 changeset[20];
    u8 boot_version[12]; 
    u8 ota_version[12];  
    u32 sw_version;
}upg_file_ver_t;

BOOL ui_upg_file_list_get(flist_dir_t dir, flist_option_t option, 
                                                file_list_t *p_list, u8 data_mode);

RET_CODE ui_get_upg_file_version(u16 *p_path, upg_file_ver_t *p_upg_file_ver);

BOOL ui_is_file_exist_in_usb(u16 *p_path);

BOOL ui_find_usb_upg_file(void);

void ui_release_usb_upgrade(u8 root_id);

#endif


