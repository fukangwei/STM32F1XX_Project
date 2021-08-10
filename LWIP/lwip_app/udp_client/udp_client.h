#ifndef __UDP_CLIENT_H
#define __UDP_CLIENT_H

#include "sys.h"
#include "lwip/ip_addr.h"

#define LWIP_CONNECTED  0X80
#define LWIP_NEW_DATA   0x40
#define LWIP_SEND_DATA  0x20

#define LWIP_SEND_BUF   200
#define LWIP_RECV_BUF   200

#define UDP_CLIENT_PORT 1500
#define UDP_SERVER_PORT 2000

extern u8 lwip_flag;
extern u8 lwip_recv_buf[];
extern u8 lwip_send_buf[];

void Init_UDP_Client ( ip_addr_t server_ipaddr );
void udp_client_send_data ( void );

#endif
