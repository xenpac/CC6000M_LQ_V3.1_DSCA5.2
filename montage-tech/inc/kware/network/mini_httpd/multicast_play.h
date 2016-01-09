#ifndef _MULTICAST_PLAY_H_
#define _MULTICAST_PLAY_H_

typedef struct
{
    u8 server_ip_addr[32];
    u16 port;
    u8 protol_type; // 1: rtp 0: udp
    u32 rate;
}mcast_server_info_t;

enum protol_type
{
   UDP_TYPE = 1,
   RTP_TYPE,
};

typedef struct
{
    u8 *pBuffer;
    u16 bufferSize;
    u8 payloadType;
    u32 timeStamp;
    s32 ssrc;
    u8 marker;
    u8 protol_type;
}mcast_packet_info_t;

RET_CODE multicast_socket_create(mcast_server_info_t *p_server_info);
void multicast_socket_close(void);
void multicast_start_send_data(u8 *p_data, u32 length, u8 index, u8 protol_type);
#endif
