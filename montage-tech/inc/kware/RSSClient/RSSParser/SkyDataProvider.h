#ifndef  __FUNNYSKY_DATA_PROVIDER__
#define  __FUNNYSKY_DATA_PROVIDER__

#ifdef __cplusplus
extern "C" {
#endif
/*
*
*
*
*
*
*
*
*/
BOOL Sky_LiveDataProvider_Init(void * p_hdl);
/*
*
*
*
*
*
*
*
*/
void Sky_LiveDataProvider_Deinit(void * p_hdl);




void * Sky_Get_ChannelEpgInfo(void * hdl, int year,int month,int day, int frequency, int serviceid, char *tv_name);


#ifdef __cplusplus
}
#endif




#endif


