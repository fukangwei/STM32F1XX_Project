#include "udp_client.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/udp.h"
#include "usart.h"
#include "string.h"

struct udp_pcb *udp_client_pcb;
struct pbuf *ubuf_client;
u8 lwip_flag = 0;

u8 lwip_recv_buf[LWIP_RECV_BUF] = {0};
u8 lwip_send_buf[LWIP_SEND_BUF] = {0};

void udp_client_rev ( void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port ) {
    if ( p != NULL ) {
        if ( ( p->tot_len ) >= LWIP_RECV_BUF ) {
            ( ( char * ) p->payload ) [LWIP_RECV_BUF - 1] = 0;
            memcpy ( lwip_recv_buf, p->payload, LWIP_RECV_BUF );
        } else {
            memcpy ( lwip_recv_buf, p->payload, p->tot_len );
            lwip_recv_buf[p->tot_len] = 0;
        }

        lwip_flag |= LWIP_NEW_DATA;
        pbuf_free ( p );
    }
}

void udp_client_send_data ( void ) {
    err_t err;

    if ( ( lwip_flag & LWIP_SEND_DATA ) == LWIP_SEND_DATA ) {
        ubuf_client = pbuf_alloc ( PBUF_TRANSPORT, strlen ( ( char * ) lwip_send_buf ), PBUF_RAM );
        ubuf_client->payload = lwip_send_buf;
        err = udp_send ( udp_client_pcb, ubuf_client );

        if ( err != ERR_OK ) {
            printf ( "UDP Client Send Error£¡" );
        }

        lwip_flag &= ~LWIP_SEND_DATA;
        pbuf_free ( ubuf_client );
    }
}

void Init_UDP_Client ( ip_addr_t server_ipaddr ) {
    ip_addr_t ipaddr = server_ipaddr;
    udp_client_pcb = udp_new();

    if ( udp_client_pcb != NULL ) {
        udp_bind ( udp_client_pcb, IP_ADDR_ANY, UDP_CLIENT_PORT );
        udp_connect ( udp_client_pcb, &ipaddr, UDP_SERVER_PORT );
        udp_recv ( udp_client_pcb, udp_client_rev, NULL );
    }
}
