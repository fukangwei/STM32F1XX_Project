#ifndef __UDP_SERVER_H
#define __UDP_SERVER_H

#include "sys.h"

#define LWIP_CONNECTED  0X80
#define LWIP_NEW_DATA   0x40
#define LWIP_SEND_DATA  0x20

#define LWIP_SEND_BUF   200
#define LWIP_RECV_BUF   200

#define UDP_SERVER_PORT 1400

extern u8 lwip_flag;
extern u8 lwip_recv_buf[];
extern u8 lwip_send_buf[];

void Init_UDP_Server ( void );
void udp_server_send_data ( void );
#endif
