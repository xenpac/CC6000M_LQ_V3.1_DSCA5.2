#ifndef __SAMBA_FS_H_
#define __SAMBA_FS_H_

/* SMBlib return codes */
/* We want something that indicates whether or not the return code was a   */
/* remote error, a local error in SMBlib or returned from lower layer ...  */
/* Wonder if this will work ...                                            */
/* SAMBAFSE_Remote = 1 indicates remote error                               */
/* SAMBAFSE_ values < 0 indicate local error with more info available       */
/* SAMBAFSE_ values >1 indicate local from SMBlib code errors?              */

#define SAMBAFSE_Success 0
#define SAMBAFSE_Remote  1    /* Remote error, get more info from con        */
#define SAMBAFSE_BAD     -1
#define SAMBAFSE_LowerLayer 2 /* Lower layer error                           */
#define SAMBAFSE_NotImpl 3    /* Function not yet implemented                */
#define SAMBAFSE_ProtLow 4    /* Protocol negotiated does not support req    */
#define SAMBAFSE_NoSpace 5    /* No space to allocate a structure            */
#define SAMBAFSE_BadParam 6   /* Bad parameters                              */
#define SAMBAFSE_NegNoProt 7  /* None of our protocols was liked             */
#define SAMBAFSE_SendFailed 8 /* Sending an SMB failed                       */
#define SAMBAFSE_RecvFailed 9 /* Receiving an SMB failed                     */
#define SAMBAFSE_GuestOnly 10 /* Logged in as guest                          */
#define SAMBAFSE_CallFailed 11 /* Call remote end failed                     */
#define SAMBAFSE_ProtUnknown 12 /* Protocol unknown                          */
#define SAMBAFSE_NoSuchMsg  13 /* Keep this up to date                       */

int mount_samba_fs(char *server, char *service, char *username, char *password);
int umount_samba_fs(char *server, char *service);

#endif
