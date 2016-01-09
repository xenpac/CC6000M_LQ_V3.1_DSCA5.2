/************************************************************
*
 
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cmediarenderer.h
*
*	Revision:
*		2009/06/11
*        - first release.
*
************************************************************/

#ifndef _CG_CLINKCAV_MEDIARENDERER_H_
#define _CG_CLINKCAV_MEDIARENDERER_H_

#include <cybergarage/upnp/cupnp.h>
#include <cybergarage/upnp/std/av/cupnpav.h>

#ifdef  __cplusplus
extern "C" {
#endif

#undef CG_CLINKCAV_USE_UPNPSTD_XML 

/****************************************
* Struct
****************************************/

typedef struct _CgUpnpAvRenderer {
CgMutex *mutex;
CgUpnpDevice *dev;
CG_UPNPAV_HTTP_LISTENER httplistener;
CG_UPNPAV_ACTION_LISTNER actionListner;
CG_UPNPAV_STATEVARIABLE_LISTNER queryListner;
CgUpnpAvProtocolInfoList *protocolInfoList;
void *userData;
} CgUpnpAvRenderer;

/****************************************
* Constants (Media Server)
****************************************/

#define CG_UPNPAV_DMR_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaRenderer:1"
#define CG_UPNPAV_DMR_DEFAULT_HTTP_PORT 38520

/****************************************
* Constants (Rendering Control)
****************************************/

#define CG_UPNPAV_DMR_RENDERINGCONTROL_SERVICE_TYPE "urn:schemas-upnp-org:service:RenderingControl:1"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_INSTANCE_ID "InstanceID"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_GETVOLUME "GetVolume"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_GETMUTE "GetMute"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_SETVOLUME "SetVolume"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_SETMUTE "SetMute"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_CHANNEL "Channel"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_CURRENTMUTE "CurrentMute"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_DESIREDMUTE "DesiredMute"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_CURRENTVOLUME "CurrentVolume"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_DESIREDVOLUME "DesiredVolume"

/* statevariable */
#define CG_UPNPAV_DMR_RENDERINGCONTROL_CONTRAST "Contrast"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_BRIGHTNESS "Brightness"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_VOLUME "Volume"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_MUTE "Mute"
#define CG_UPNPAV_DMR_RENDERINGCONTROL_LASTCHANGE "LastChange"

#define CG_UPNPAV_DMR_RENDERINGCONTROL_MASTER "Master"


/****************************************
* Constants (Connection Manager)
****************************************/

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SERVICE_TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_HTTP_GET "http-get"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_INFO "GetCurrentConnectionInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_PROTOCOL_INFO "GetProtocolInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_GET_CURRENT_CONNECTION_IDS "GetCurrentConnectionIDs"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SOURCE "Source"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SINK "Sink"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_CURRENTCONNECTIONIDS "CurrentConnectionIDs"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SOURCEPROTOCOLINFO "SourceProtocolInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_SINKPROTOCOLINFO "SinkProtocolInfo"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_CONNECTION_ID "ConnectionID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_RCS_ID "RcsID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_AVTRANSPORT_ID "AVTransportID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_PROTOCOL_INFO "ProtocolInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_PEER_CONNECTIONMANAGER "PeerConnectionManager"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_PEER_CONNECTION_ID "PeerConnectionID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_CONNECTION_IDS "ConnectionIDs"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_DIRECTION "Direction"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_STATUS "Status"

#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_CONNECTION_ID "A_ARG_TYPE_ConnectionID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_RCS_ID "A_ARG_TYPE_RcsID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_AVTRANSPORT_ID "A_ARG_TYPE_AVTransportID"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_PROTOCOL_INFO "A_ARG_TYPE_ ProtocolInfo"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_CONNECTION_MANAGER "A_ARG_TYPE_ConnectionManager"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_DIRECTION "A_ARG_TYPE_Direction"
#define CG_UPNPAV_DMR_CONNECTIONMANAGER_A_ARG_TYPE_CONNECTION_STATUS "A_ARG_TYPE_ConnectionStatus"
/****************************************
* Constants (AVTransport)
****************************************/

#define CG_UPNPAV_DMR_AVTRANSPORT_SERVICE_TYPE "urn:schemas-upnp-org:service:AVTransport:1"

/* action */
#define CG_UPNPAV_DMR_AVTRANSPORT_GETTRANSPORTINFO "GetTransportInfo"
#define CG_UPNPAV_DMR_AVTRANSPORT_GET_DEVICE_CAPABILITIES "GetDeviceCapabilities"
#define CG_UPNPAV_DMR_AVTRANSPORT_GET_MEDIA_INFO "GetMediaInfo"
#define CG_UPNPAV_DMR_AVTRANSPORT_GET_POSITION_INFO "GetPositionInfo"
#define CG_UPNPAV_DMR_AVTRANSPORT_GET_TRANSPORT_SETTINGS "GetTransportSettings"
#define CG_UPNPAV_DMR_AVTRANSPORT_SET_AVTRANSPORT_URI "SetAVTransportURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_PLAY "Play"
#define CG_UPNPAV_DMR_AVTRANSPORT_STOP "Stop"
#define CG_UPNPAV_DMR_AVTRANSPORT_PAUSE "Pause"
#define CG_UPNPAV_DMR_AVTRANSPORT_SEEK "Seek"

/* argument */
#define CG_UPNPAV_DMR_AVTRANSPORT_INSTANCE_ID "InstanceID"
#define CG_UPNPAV_DMR_AVTRANSPORT_PLAY_MEDIA "PlayMedia"
#define CG_UPNPAV_DMR_AVTRANSPORT_REC_MEDIA "RecMedia"
#define CG_UPNPAV_DMR_AVTRANSPORT_REC_QUALITY_MODES "RecQualityModes"

#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE "CurrentTransportState"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATUS "CurrentTransportStatus"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTSPEED "CurrentSpeed"

#define CG_UPNPAV_DMR_AVTRANSPORT_NR_TRACKS "NrTracks"
#define CG_UPNPAV_DMR_AVTRANSPORT_MEDIA_DURATION "MediaDuration"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_URI "CurrentURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_URI_METADATA "CurrentURIMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_NEXT_URI "NextURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_NEXT_URI_METADATA "NextURIMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_PLAY_MEDIUM "PlayMedium"
#define CG_UPNPAV_DMR_AVTRANSPORT_RECORD_MEDIUM "RecordMedium"
#define CG_UPNPAV_DMR_AVTRANSPORT_WRITE_STATUS "WriteStatus"

#define CG_UPNPAV_DMR_AVTRANSPORT_TRACK "Track"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRACK_DURATION "TrackDuration"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRACK_METADATA "TrackMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRACK_URI "TrackURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_REL_TIME "RelTime"
#define CG_UPNPAV_DMR_AVTRANSPORT_ABS_TIME "AbsTime"
#define CG_UPNPAV_DMR_AVTRANSPORT_REL_COUNT "RelCount"
#define CG_UPNPAV_DMR_AVTRANSPORT_ABS_COUNT "AbsCount"

#define CG_UPNPAV_DMR_AVTRANSPORT_PLAY_MODE "PlayMode"
#define CG_UPNPAV_DMR_AVTRANSPORT_REC_QUALITY_MODE "RecQualityMode"
#define CG_UPNPAV_DMR_AVTRANSPORT_SPEED "Speed"
#define CG_UPNPAV_DMR_AVTRANSPORT_UNIT "Unit"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRAGET "Target"

/* statevariable */
#define CG_UPNPAV_DMR_AVTRANSPORT_NUMBER_OF_TRACKS "NumberOfTracks"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_MEDIA_DURATION "CurrentMediaDuration"
#define CG_UPNPAV_DMR_AVTRANSPORT_AVTRANSPORT_URI "AVTransportURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_AVTRANSPORT_URI_METADATA "AVTransportURIMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_NEXT_AVTRANSPORT_URI "NextAVTransportURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_NEXT_AVTRANSPORT_URI_METADATA "NextAVTransportURIMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_PLAYBACK_STORAGE_MEDIUM "PlaybackStorageMedium"
#define CG_UPNPAV_DMR_AVTRANSPORT_RECORD_STORAGE_MEDIUM "RecordStorageMedium"
#define CG_UPNPAV_DMR_AVTRANSPORT_RECORD_MEDIUM_WRITE_STATUS "RecordMediumWriteStatus"

#define CG_UPNPAV_DMR_AVTRANSPORT_POSSIBLE_PLAYBACK_STORAGE_MEDIA "PossiblePlaybackStorageMedia"
#define CG_UPNPAV_DMR_AVTRANSPORT_POSSIBLE_RECORD_STORAGE_MEDIA "PossibleRecordStorageMedia"
#define CG_UPNPAV_DMR_AVTRANSPORT_POSSIBLE_RECORD_QUALITY_MODES "PossibleRecordQualityModes"

#define CG_UPNPAV_DMR_AVTRANSPORT_TRANSPORT_STATE "TransportState"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRANSPORT_STATUS "TransportStatus"

#define CG_UPNPAV_DMR_AVTRANSPORT_CURRTENT_TRACK "CurrentTrack"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRTENT_TRACK_DURATION "CurrentTrackDuration"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRTENT_TRACK_METADATA "CurrentTrackMetaData"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRTENT_TRACK_URI "CurrentTrackURI"
#define CG_UPNPAV_DMR_AVTRANSPORT_RELATIVE_TIME_POSTION "RelativeTimePosition"
#define CG_UPNPAV_DMR_AVTRANSPORT_ABSOLUTE_TIME_POSTION "AbsoluteTimePosition"
#define CG_UPNPAV_DMR_AVTRANSPORT_RELATIVE_COUNTER_POSITION "RelativeCounterPosition"
#define CG_UPNPAV_DMR_AVTRANSPORT_ABSOLUTE_COUNTER_POSITION "AbsoluteCounterPosition"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_CURRENT_TRANSPORTACTIONS "CurrentTransportActions"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_PLAY_MODE "CurrentPlayMode"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENT_RECORD_QUALITY_MODE "CurrentRecordQualityMode"
#define CG_UPNPAV_DMR_AVTRANSPORT_TRANSPORT_PLAY_SPEED "TransportPlaySpeed"
#define CG_UPNPAV_DMR_AVTRANSPORT_LASTCHANGE "LastChange"
#define CG_UPNPAV_DMR_AVTRANSPORT_A_ARG_TYPE_SEEKMODE "A_ARG_TYPE_SeekMode"
#define CG_UPNPAV_DMR_AVTRANSPORT_A_ARG_TYPE_SEEKTARGET "A_ARG_TYPE_SeekTarget"

#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE_NOMEDIAPRESENT "NO_MEDIA_PRESENT"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE_STOPPED "STOPPED"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE_PLAYING "PLAYING"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATE_PAUSED_PLAYBACK "PAUSED_PLAYBACK"
#define CG_UPNPAV_DMR_AVTRANSPORT_CURRENTTRANSPORTSTATUS_OK "OK"
#define CG_UPNPAV_DMR_AVTRANSPORT_NOT_IMPLEMENTED "NOT_IMPLEMENTED"
#define CG_UPNPAV_DMR_AVTRANSPORT_NONE "NONE"
#define CG_UPNPAV_DMR_AVTRANSPORT_NORMAL "NORMAL"
#define CG_UPNPAV_DMR_AVTRANSPORT_NETWORK "NETWORK"
#define CG_UPNPAV_DMR_AVTRANSPORT_RELATIVE_TIME "REL_TIME"

/****************************************
* Public Functions
****************************************/

CgUpnpAvRenderer *cg_upnpav_dmr_new();
void cg_upnpav_dmr_delete(CgUpnpAvRenderer *dmr);

#define cg_upnpav_dmr_getdevice(dmr) (dmr->dev)

#define cg_upnpav_dmr_start(dmr) cg_upnp_device_start(dmr->dev)
#define cg_upnpav_dmr_stop(dmr) cg_upnp_device_stop(dmr->dev)

#define cg_upnpav_dmr_lock(dmr) cg_mutex_lock(dmr->mutex)
#define cg_upnpav_dmr_unlock(dmr) cg_mutex_unlock(dmr->mutex)

#define cg_upnpav_dmr_setfriendlyname(dmr, value) cg_upnp_device_setfriendlyname(dmr->dev, value)
#define cg_upnpav_dmr_getfriendlyname(dmr) cg_upnp_device_getfriendlyname(dmr->dev)

#define cg_upnpav_dmr_setudn(dmr, value) cg_upnp_device_setudn(dmr->dev, value)
#define cg_upnpav_dmr_getudn(dmr) cg_upnp_device_getudn(dmr->dev)

#define cg_upnpav_dmr_sethttplistener(dmr,func) (dmr->httplistener = func)
#define cg_upnpav_dmr_gethttplistener(dmr) (dmr->httplistener)

#define cg_upnpav_dmr_setactionlistener(dmr,func) (dmr->actionListner = func)
#define cg_upnpav_dmr_getactionlistener(dmr) (dmr->actionListner)

#define cg_upnpav_dmr_setquerylistener(dmr,func) (dmr->queryListner = func)
#define cg_upnpav_dmr_getquerylistener(dmr) (dmr->queryListner)

#define cg_upnpav_dmr_setuserdata(dmr,data) (dmr->userData = data)
#define cg_upnpav_dmr_getuserdata(dmr) (dmr->userData)

void cg_upnpav_dmr_addprotocolinfo(CgUpnpAvRenderer *dmr, CgUpnpAvProtocolInfo *info);
#define cg_upnpav_dmr_getprotocolinfos(dmr) cg_upnpav_protocolinfolist_gets(dmr->protocolInfoList)

void cg_upnpav_dmr_setsinkprotocolinfo(CgUpnpAvRenderer *dmr, char *value);
char *cg_upnpav_dmr_getsinkprotocolinfo(CgUpnpAvRenderer *dmr);

void cg_upnpav_dmr_setsourceprotocolinfo(CgUpnpAvRenderer *dmr, char *value);
char *cg_upnpav_dmr_getsourceprotocolinfo(CgUpnpAvRenderer *dmr);

void cg_upnpav_dmr_setcurrentconnectionids(CgUpnpAvRenderer *dmr, char *value);
char *cg_upnpav_dmr_getcurrentconnectionids(CgUpnpAvRenderer *dmr);

void cg_upnpav_dmr_setavtransportlastchange(CgUpnpAvRenderer *dmr, char *value);
char *cg_upnpav_dmr_getavtransportlastchange(CgUpnpAvRenderer *dmr);

void cg_upnpav_dmr_setrenderingcontrollastchange(CgUpnpAvRenderer *dmr, char *value);
char *cg_upnpav_dmr_getrenderingcontrollastchange(CgUpnpAvRenderer *dmr);

#ifdef  __cplusplus
}
#endif

#endif
