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

#define TCP_CLIENT_RX_BUFSIZE  1500 // TCP Client���������ݳ���
#define TCP_CLIENT_TX_BUFSIZE  200  // TCP Client��������ݳ���
#define TCP_SERVER_PORT        8087 // TCP ClientҪ���ӵķ������˿�

extern u8 tcp_client_flag;
extern u8 tcp_client_recvbuf[];
extern u8 tcp_client_sendbuf[];

enum tcp_client_states {    // TCP����������״̬
    ES_TCPCLIENT_NONE = 0,  // û������
    ES_TCPCLIENT_CONNECTED, // ���ӵ���������
    ES_TCPCLIENT_CLOSING,   // �ر�����
};

struct tcp_client_struct { // LwIP�ص�����ʹ�õĽṹ��
    u8 state;              // ��ǰ����״̬
    struct tcp_pcb *pcb;   // ָ��ǰ��pcb
    struct pbuf *p;        // ָ����ջ��͵�pbuf
};

err_t tcp_client_connected ( void *arg, struct tcp_pcb *tpcb, err_t err );
err_t tcp_client_recv ( void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err );
void tcp_client_error ( void *arg, err_t err );
err_t tcp_client_poll ( void *arg, struct tcp_pcb *tpcb );
err_t tcp_client_sent ( void *arg, struct tcp_pcb *tpcb, u16_t len );
void tcp_client_senddata ( struct tcp_pcb *tpcb, struct tcp_client_struct * es );
void tcp_client_connection_close ( struct tcp_pcb *tpcb, struct tcp_client_struct * es );

#endif
