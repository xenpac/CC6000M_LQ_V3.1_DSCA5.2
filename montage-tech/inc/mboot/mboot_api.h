/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MBOOT_API_H__
#define __MBOOT_API_H__

/*!
  Get version command
  
  \param[in] None

  \return pointer to version command
  */
mbcmd_t *mboot_version_init(void);

/*!
  Get crc command
  
  \param[in] None

  \return pointer to crc command
  */
mbcmd_t *mboot_crc_init(void);

/*!
  Get decompress command
  
  \param[in] None

  \return pointer to decompress command
  */
mbcmd_t *mboot_decompress_init(void);

/*!
  Get download command
  
  \param[in] None

  \return pointer to download command
  */
mbcmd_t *mboot_download_init(void);

/*!
  Get burn command
  
  \param[in] None

  \return pointer to burn command
  */
mbcmd_t *mboot_burn_init(void);

/*!
  Get jump command
  
  \param[in] None

  \return pointer to jump command
  */
mbcmd_t *mboot_jump_init(void);

/*!
  Get ota command
  
  \param[in] None

  \return pointer to ota command
  */
mbcmd_t *mboot_ota_init(void);

/*!
  Get help command, this command is added by Mboot automatically, do not register this function in mboot_cmd_setup()
  
  \param[in] pp_cmd pointer to the command list
  \param[in] count the number of registered commands

  \return pointer to jump command
  */
mbcmd_t *mboot_help_init(mbcmd_t **pp_cmd, u32 count);



#endif //end of __MBOOT_API_H
