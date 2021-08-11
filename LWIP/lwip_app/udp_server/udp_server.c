#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/udp.h"
#include "usart.h"
#include "string.h"
#include "udp_server.h"

struct udp_pcb *udp_server_pcb;
struct pbuf *ubuf;

u8 lwip_flag = 0;
u8 lwip_recv_buf[LWIP_RECV_BUF] = {0};
u8 lwip_send_buf[LWIP_SEND_BUF] = {0};

void udp_server_rev ( void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port ) {
    if ( p != NULL ) {
        if ( ( p->tot_len ) >= LWIP_RECV_BUF ) {
            ( ( char * ) p->payload ) [LWIP_RECV_BUF - 1] = 0;
            memcpy ( lwip_recv_buf, p->payload, LWIP_RECV_BUF );
        } else {
            memcpy ( lwip_recv_buf, p->payload, p->tot_len );
            lwip_recv_buf[p->tot_len] = 0;
        }

        lwip_flag |= LWIP_NEW_DATA;
        udp_server_pcb->remote_ip = *addr;
        udp_server_pcb->remote_port = port;
        pbuf_free ( p );
    }
}

void udp_server_send_data ( void ) {
    err_t err;

    if ( ( lwip_flag & LWIP_SEND_DATA ) == LWIP_SEND_DATA ) {
        ubuf = pbuf_alloc ( PBUF_TRANSPORT, strlen ( ( char * ) lwip_send_buf ), PBUF_RAM );
        ubuf->payload = lwip_send_buf;
        err = udp_send ( udp_server_pcb, ubuf );

        if ( err != ERR_OK ) {
            printf ( "UDP SERVER Send Error!\r\n" );
        }

        lwip_flag &= ~LWIP_SEND_DATA;
        pbuf_free ( ubuf );
    }
}

void Init_UDP_Server ( void ) {
    udp_server_pcb = udp_new();

    if ( udp_server_pcb != NULL ) {
        udp_bind ( udp_server_pcb, IP_ADDR_ANY, UDP_SERVER_PORT );
        udp_recv ( udp_server_pcb, udp_server_rev, NULL );
    }
}
