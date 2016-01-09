#ifndef _C_DLNA_PLAYER_H_
#define _C_DLNA_PLAYER_H_


#ifdef  __cplusplus
extern "C" {
#endif
int cg_dlna_player_get_postion_info(unsigned int *vpts, unsigned int *total);
int cg_dlna_player_set_play_uri(char *uri);
int cg_dlna_player_set_play_metadata(char *metadata);
int cg_dlna_player_start(char *speed);
int cg_dlna_player_stop(void);
int cg_dlna_player_pause(void);
int cg_dlna_player_resume(void);
int cg_dlna_player_seek(char *sec);
int cg_dlna_player_setmute(char *mute);
int cg_dlna_player_setvolume(char *volume);
#ifdef  __cplusplus
}
#endif

#endif
