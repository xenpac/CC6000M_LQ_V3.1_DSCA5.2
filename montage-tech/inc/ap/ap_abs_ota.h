/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __AP_ABS_OTA_H_
#define __AP_ABS_OTA_H_

/*!
  Delcare all command for OTA
  */
typedef enum
{
  /*!
    Start OTA
    */
  OTA_CMD_START = 0, 
  /*!
  Set user input parameter, \see nc_channel_info_t 
  */
  OTA_CMD_SET_USER_INPUT_PARAM,
  /*!
    Cancel OTA
    */
  OTA_CMD_CANCEL = SYNC_CMD,
} ota_cmd_t;

/*!
  Declare all events which will be notified to high level modules like
  UI
  */
typedef enum
{
  /*!
    Notify current scaning progress and current signal information
    */
  OTA_EVT_PROGRESS = (APP_OTA << 16),
  /*!
    Notify that OTA can't lock transponder and OTA entered IDLE status
    */
  OTA_EVT_NEED_USER_INPUT,
  /*!
    Can't find PMT
    */
  OTA_EVT_NO_PMT,
  /*!
    Can't find upgrade des in PMT
    */
  OTA_EVT_NO_DES_IN_PMT,
  /*!
    Can't find section
    */
  OTA_EVT_NO_SECTION,
  /*!
    Find bad section
    */
  OTA_EVT_BAD_SECTION,
  /*!
    Bad file size
    */
  OTA_EVT_BAD_FILE_SIZE,
  /*!
    Can't lock stream
    */
  OTA_EVT_LOCK_STREAM_FAILED,
  /*!
    Bad sigature
    */
  OTA_EVT_BAD_SIGNATURE,
  /*!
    Bad CRC
    */
  OTA_EVT_BAD_CRC,
  /*!
    Burning failed
    */
  OTA_EVT_BURNING_FAILED,
  /*!
    Initializing
    */
  OTA_EVT_INITIALIZING,
  /*!
    Processing section
    */
  OTA_EVT_PROCESSING_SECTION,
  /*!
    Waiting section
    */
  OTA_EVT_WAITING_SECTION,
  /*!
    Locking
    */
  OTA_EVT_LOCKING,
  /*!
    Waiting PMT
    */
  OTA_EVT_WAITING_PMT,
  /*!
    Downloading data
    */
  OTA_EVT_DOWNLOADING_DATA,
  /*!
    Download finished
    */
  OTA_EVT_DOWNLOAD_FINISHED,
  /*!
    Fatal error, abort
    */
  OTA_EVT_ABORT,
  /*!
    OTA finished
    */
  OTA_EVT_FINISHED,
} ota_evt_t;

/*!
  Singleton patern interface which can get an OTA application's
  instance

  \param[in] p_policy Policy handle, if it's NULL OTA will use default policy
  
  \return instance address
  */
app_t *construct_ap_abs_ota(void);

#endif // End for __AP_OTA_H_

