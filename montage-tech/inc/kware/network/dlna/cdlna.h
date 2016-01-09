#ifndef _C_DLNA_H_
#define _C_DLNA_H_


#ifdef  __cplusplus
extern "C" {
#endif

#define DLNA_EXT_DMS_NUM_MAX           (32)
#define DLNA_DMS_DIR_RES_NUM_MAX       (128)
#define DLNA_DMS_NAME_LEN_MAX          (64)
#define DLNA_DMS_UDN_LEN_MAX           (64)
#define DLNA_RESC_TITLE_LEN_MAX        (128)
#define DLNA_RESC_RESID_LEN_MAX        (256)
#define DLNA_SATIP_PROGRAM_NUM_MAX     (128)



typedef struct _dlna_dms_info
{
    char name[DLNA_DMS_NAME_LEN_MAX];
    char udn[DLNA_DMS_UDN_LEN_MAX];
} dlna_dms_info;

typedef enum{
    DLNA_RESC_DIR  = 0,
    DLNA_RESC_FILE = 1,
} dlna_resc_type;

typedef enum
{
    DLNA_FUNC_STATUS_STOPED = 0,
    DLNA_FUNC_STATUS_RUNNING,
} DLNA_FUNC_STATUS;


typedef struct _dlna_dms_resouce_info // dir and files
{
    dlna_resc_type type;
    char title[DLNA_RESC_TITLE_LEN_MAX];  // file or dir's titile
    char resId[DLNA_RESC_RESID_LEN_MAX];  // dir: objectId; files: res url.
} dlna_dms_resouce_info;


/** \brief Errors returned from the vendor defined callbacks.
*/
typedef enum
{
    /** \brief There is no error. */
    DMR_ERROR_OK = 0,
    /** \brief The This pointer for the pseudo class is not valid. */
    DMR_ERROR_BADTHIS = -1,
    /** \brief The internal pointer for the pseudo class's private variables is not valid. */
    DMR_ERROR_BADINTERNALSTATE = -2,
    /** \brief The DMR is not currently running. */
    DMR_ERROR_NOTRUNNING = -3,
    /** \brief The play list is bad and cannot be played. */
    DMR_ERROR_BADPLAYLIST = -4,
    /** \brief The play list is too large and cannot be played. */
    DMR_ERROR_PLAYLISTTOOLARGE = -5,
    /** \brief The specified track index is out of range. */
    DMR_ERROR_TRACKINDEXOUTOFRANGE = -6,
    /** \brief The specified new play state of transition is not supported from this state. */
    DMR_ERROR_TRANSITIONNOTSUPPORTED = -7,
    /** \brief The specified new volume is out of range. */
    DMR_ERROR_VOLUMEOUTOFRANGE = -8,
    /** \brief The specified new contrast is out of range. */
    DMR_ERROR_CONTRASTOUTOFRANGE = -9,
    /** \brief The specified new brightness is out of range. */
    DMR_ERROR_BRIGHTNESSOUTOFRANGE = -10,
    /** \brief The specified transport URI was rejected by the application. */
    DMR_ERROR_REJECTURI = -11,
    /** \brief The specified transport URI metadata was rejected by the application. */
    DMR_ERROR_REJECTMETADATA = -12,
    /** \brief The specified transport URI is not a valid URI. */
    DMR_ERROR_BADURI = -13,
    /** \brief The specified transport URI metadata is not a valid. */
    DMR_ERROR_BADMETADATA = -14,
    /** \brief The specified play state is not a valid play state. */
    DMR_ERROR_BADPLAYSTATE = -15,
    /** \brief The specified record state is not a valid record state. */
    DMR_ERROR_BADRECORDSTATE = -16,
    /** \brief The specified media play mode command is not a valid. */
    DMR_ERROR_BADMEDIAPLAYMODE = -17,
    /** \brief The specified transport status is not valid. */
    DMR_ERROR_BADTRANSPORTSTATUS = -18,
    /** \brief The specified play command is not a valid play command. */
    DMR_ERROR_BADPLAYCOMMAND = -19,
    /** \brief The specified record command is not a valid record command. */
    DMR_ERROR_BADRECORDCOMMAND = -20,
    /** \brief No thread pool was supplied by the application for use in the DMR. */
    DMR_ERROR_NOTHREADPOOL = -21,
    /** \brief A passed argument was invalid. */
    DMR_ERROR_INVALIDARGUMENT = -22,
    /** \brief The DMR failed to allocate memory on the heap. */
    DMR_ERROR_OUTOFMEMORY = -23,
    /** \brief The playspeed was invalid with the given play command */
    DMR_ERROR_BADPLAYSPEED = -24
} DMR_Error;


typedef enum
{
    DMS_STATSU_ADD = 0,
    DMS_STATSU_DEL,

}DMP_DMS_NOTIFY_STATUS;


typedef enum
{
    DLNA_OPERATE_OK = 0,
    DLNA_OPERATE_FAIL = 1,

}DLNA_OPERATE_CONFIRM_STATUS;



/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetAVTransportURI. */
typedef int (*DMRCallback_SetAVTransportURI)(char *uri);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetPlayMode. */
typedef int (*DMRCallback_SetPlayMode)(char *playMode);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Stop. */
typedef int (*DMRCallback_Stop)(void);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Play. */
typedef int (*DMRCallback_Play)(char *playSpeed, char *cur_media_duration,
  char *cur_track_duration, int *track_num, int *cur_track);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Pause. */
typedef int (*DMRCallback_Pause)(void);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Resume. */
typedef int (*DMRCallback_Resume)(void);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SeekTrack. */
typedef int (*DMRCallback_SeekTrack)(unsigned int trackIndex);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SeekTrackPosition. */
typedef int (*DMRCallback_SeekTrackPosition)(long position);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SeekMediaPosition. */
typedef int (*DMRCallback_SeekMediaPosition)(long position);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Next. */
typedef int (*DMRCallback_Next)(void);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_Previous. */
typedef int (*DMRCallback_Previous)(void);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SelectPreset. */
typedef int (*DMRCallback_SelectPreset)(char* presetName);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetVolume. */
typedef int (*DMRCallback_SetVolume)(unsigned char volume);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetMute. */
typedef int (*DMRCallback_SetMute)(BOOL mute);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetContrast. */
typedef int (*DMRCallback_SetContrast)(unsigned char contrast);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetBrightness. */
typedef int (*DMRCallback_SetBrightness)(unsigned char brightness);
/** \brief This is a function pointer definition for the event \ref _DMP::Event_DmsStatusNotify. */
typedef int (*DMPCallback_DmsStatusNotify)(DMP_DMS_NOTIFY_STATUS notifyStatus);
/** \brief This is a function pointer definition for the event \ref _DLNA::Event_DlnaOperateConfirm. */
typedef int (*DlnaCallback_OperateConfirm)(DLNA_OPERATE_CONFIRM_STATUS OpStatus);
/** \brief This is a function pointer definition for the event \ref _DMR::Event_SetAVTransportMETADATA. */
typedef int (*DMRCallback_SetAVTransportMETADATA)(char *uri);
/** \brief This is a function pointer definition for the event \ref _DLNA::Event_GetPostionInfo. */
typedef int (*DlnaCallback_GetPostionInfo)(unsigned int *vpts, unsigned int *total);


/*!
  cg_dlna_event_callback_t
  */
typedef struct {
    /** \brief \b EVENT: This is in response to a UPnP action call for action
        SetAVTransportURI.
        \param uri The URI of the media item or playlist to be rendered by
        the vendor's application.
        \returns An instance of the enum \ref DMR_Error.  A value of
        \c DMR_ERROR_OK means that the function succeeded; any other
        \ref DMR_Error value is an error.
    */
    DMRCallback_SetAVTransportURI Event_SetAVTransportURI;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        SetPlayMode.
        \param playMode One of \ref DMR_MediaPlayMode.  This defines how the
        application should play the media set (repeat all, shuffle, etc...).
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called
        \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SetPlayMode Event_SetPlayMode;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Stop.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Stop Event_Stop;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Play.
        \param playSpeed This is a string containing the playSpeed that the renderer
        is intended to play the media.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Play Event_Play;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Pause.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Pause Event_Pause;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Pause.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Resume Event_Resume;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Seek with a seekMode of TRACK_NR.
        \param trackIndex The track number to seek to.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SeekTrack Event_SeekTrack;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Seek with a seekMode of REL_TIME.
        \param position The track time index to seek to.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SeekTrackPosition Event_SeekTrackPosition;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Seek with a seekMode of ABS_TIME.
        \param position The media time index to seek to.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SeekMediaPosition Event_SeekMediaPosition;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Next.
        \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Next Event_Next;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        Previous.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_Previous Event_Previous;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        SelectPreset.
        \param presetName This is the preset name to select in the application.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SelectPreset Event_SelectPreset;

    /** \brief \b EVENT: This is to set the volume in the vendor's application.
        \param volume The volume level that the application should attempt to
        set. This value cannot exceed \ref VOLUME_MAX".
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SetVolume Event_SetVolume;

    /** \brief \b EVENT: This is to set the mute state in the vendor's application.
        \param mute The mute state that the application should attempt to set.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SetMute Event_SetMute;

    /** \brief \b EVENT: This is to set the display contrast in the vendor's
        application.
        \param contrast The contrast level that the application should attempt
        to set. This value cannot exceed \ref CONTRAST_MAX.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SetContrast Event_SetContrast;

    /** \brief \b EVENT: This is to set the display brightness in the vendor's
        application.
        \param brightness The brightness level that the application should
        attempt to set. This value cannot exceed \ref BRIGHTNESS_MAX.
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DMRCallback_SetBrightness Event_SetBrightness;

    /** \brief \b EVENT: This is to notify dms status change
        \param notifyStatus The new status of the changed DMS, defined in DMP_DMS_NOTIFY_STATUS
        \returns Zero (0) for normal completetion
    */
    DMPCallback_DmsStatusNotify Event_DmsStatusNotify;

    /** \brief \b EVENT: This is to comfirm the status for user's operation
        \param OpStatus The status for the user's operation, defined in DLNA_OPERATE_CONFIRM_STATUS
        \returns Zero (0) for normal completetion
    */
    DlnaCallback_OperateConfirm Event_DlnaOperateConfirm;
    DlnaCallback_OperateConfirm Event_SatIpOperateConfirm;

    /** \brief \b EVENT: This is in response to a UPnP action call for action
        SetAVTransportURI.
        \param metadata The metadata of the media item or playlist to be rendered by
        the vendor's application.
        \returns Zero (0) for normal completetion
    */
    DMRCallback_SetAVTransportMETADATA Event_SetAVTransportMETADATA;
	/** \brief \b EVENT: This is to get the postion info
        \param abstime(ms)
        \returns Zero (0) for normal completetion and UPnP response or negative one (-1)
        if the Event handler code called \ref DMR_Method_ErrorEventResponse on its own  Otherwise the return value is a UPnP
        error code and the microstack will respond for you with the appropriate error.
    */
    DlnaCallback_GetPostionInfo Event_DlnaGetPostionInfo;


}cg_dlna_event_callback_t;

/*!
  cg_dlna_config_t
  */
typedef struct {
  /*!
  stack size
  */
  int stack_size;
  /*!
  task prio
  */
  int task_prio;
  /*!
  dlna thread pool start
  */
  int cg_dlna_thread_pool_start;
  /*!
  dlna thread pool end
  */
  int cg_dlna_thread_pool_end;
  /*!
  dlna device name
  */
  char name[64];
  /*!
  dlna device manufacturer name
  */
  char manufacturer_name[64];
  /*!
  dlna device manufacturerURL
  */
  char manufacturer_url[64];
  /*!
  dlna event callback
  */
  cg_dlna_event_callback_t callback;
} cg_dlna_config_t;

int cg_dlna_init(cg_dlna_config_t *config);
int cg_dlna_rename(char *device_name, char *device_manufacturer_name, char *device_manufacturer_url);
int cg_dlna_start();
int cg_dlna_stop();

int cg_dlna_render_start();
int cg_dlna_render_stop();

int cg_dlna_server_start();
int cg_dlna_server_stop();

int cg_dlna_set_current_time(unsigned int pts, unsigned int duration);
int cg_dlna_user_pause();
int cg_dlna_user_resume();
int cg_dlna_user_stop();

int cg_dlna_dmp_get_play_resc();
int cg_dlna_dmp_get_play_resc_full();
int cg_dlna_dump_info();
int cg_dlna_dmp_status_notify(DMP_DMS_NOTIFY_STATUS notifyStatus);


/*********************************************************************************
  * Function:      cg_dlna_dmp_get_dms_list
  * Description:  get the dms list within network
  *
  * Input:
  *     pp_dms_list: null
  *
  * Output:
  *     pp_dms_list: point to the point of dms info list
  *
  * Return:
  *     return the dms sum if success, else return -1
  *
**********************************************************************************/
int cg_dlna_dmp_get_dms_list(dlna_dms_info **pp_dms_list);

/*********************************************************************************
  * Function:      cg_dlna_dmp_refresh_dms_list
  * Description:  refresh get the dms list within network
  *
  * Input:
  *     pp_dms_list: null
  *
  * Output:
  *     pp_dms_list: point to the point of dms info list
  *
  * Return:
  *     return the dms sum if success, else return -1
  *
**********************************************************************************/
int cg_dlna_dmp_refresh_dms_list(dlna_dms_info **pp_dms_list);

/*********************************************************************************
  * Function:      cg_dlna_dmp_get_dms_dir_resouce
  * Description:  get the files and directorys infomation under one dms dir
  *
  * Input:
  *     p_dms_info: point to the dms info
  *     dir_resId: the resId in dir's dlna_dms_resouce_info, use "0" if get the root dir resource.
  *     pp_res_list: null
  *
  * Output:
  *     pp_res_list: point to the point of resource info list
  *
  * Return:
  *     return the resouce sum including files and directorys if success, else return -1
  *
**********************************************************************************/
int cg_dlna_dmp_get_dms_dir_resouce(dlna_dms_info *p_dms_info,
    char *dir_resId, dlna_dms_resouce_info **pp_res_list);

/*********************************************************************************
  * Function:      cg_dlna_set_log_level
  * Description:  set dlna module log level
  *
  * Input:
  *     level: log level value, 1~7
  *
  * Output:
  *     NULL
  *
  * Return:
  *     return 0 if success, else return -1
  *
**********************************************************************************/
int cg_dlna_set_log_level(int level);


void* cg_dlna_malloc(u32 size);

typedef enum
{
    TV_TYPE = 0,
    DISK_TYPE,
}satip_type;


typedef struct _satip_pg_info_t
{
    unsigned char  pg_name[34];     // Program name
    unsigned long  freq;            // Input freq in MHz(950~2150)
    unsigned long  symb_rate;       // Input symb_rate in KSs
    unsigned char  onoff_22k;       // onoff_22k : Input 22k on/off(1/0)
    unsigned char  polar;           // polarization : Input polarization, 0:H; 1:V; 2:L; 3:R
    unsigned short v_pid;           //  video pid
    unsigned short a_pid;           // audio pid
    unsigned short pcr_pid;         // pcr pid
    unsigned short pmt_pid;
    unsigned short pg_id;
    satip_type     type;
    unsigned short is_des;          // [7:4] dmx record mode
} satip_pg_info_t;

int put_satip_pg_list(satip_pg_info_t *pg_list, u32 pg_cnt);


#ifdef  __cplusplus
}
#endif

#endif
