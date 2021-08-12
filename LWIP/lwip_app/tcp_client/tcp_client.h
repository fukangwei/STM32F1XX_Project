#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"

#define LWIP_CONNECTED  0x20
#define LWIP_NEW_DATA   0x40
#define LWIP_SEND_DATA  0x80

#define TCP_CLIENT_RX_BUFSIZE  1500 // TCP Client最大接收数据长度
#define TCP_CLIENT_TX_BUFSIZE  200  // TCP Client最大发送数据长度
#define TCP_SERVER_PORT        8087 // TCP Client要连接的服务器端口

extern u8 tcp_client_flag;
extern u8 tcp_client_recvbuf[];
extern u8 tcp_client_sendbuf[];

enum tcp_client_states {    // TCP服务器连接状态
    ES_TCPCLIENT_NONE = 0,  // 没有连接
    ES_TCPCLIENT_CONNECTED, // 连接到服务器了
    ES_TCPCLIENT_CLOSING,   // 关闭连接
};

struct tcp_client_struct { // LwIP回调函数使用的结构体
    u8 state;              // 当前连接状态
    struct tcp_pcb *pcb;   // 指向当前的pcb
    struct pbuf *p;        // 指向接收或发送的pbuf
};

err_t tcp_client_connected ( void *arg, struct tcp_pcb *tpcb, err_t err );
err_t tcp_client_recv ( void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err );
void tcp_client_error ( void *arg, err_t err );
err_t tcp_client_poll ( void *arg, struct tcp_pcb *tpcb );
err_t tcp_client_sent ( void *arg, struct tcp_pcb *tpcb, u16_t len );
void tcp_client_senddata ( struct tcp_pcb *tpcb, struct tcp_client_struct * es );
void tcp_client_connection_close ( struct tcp_pcb *tpcb, struct tcp_client_struct * es );

#endif
