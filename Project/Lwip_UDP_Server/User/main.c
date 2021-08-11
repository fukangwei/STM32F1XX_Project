#include "delay.h"
#include "led.h"
#include "usart.h"
#include "enc28j60.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "netif/ethernetif.h"
#include "arch/sys_arch.h"
#include "udp_server.h"

#define CLOCKTICKS_PER_MS 10 /* ����ʱ�ӽ��� */

static ip_addr_t ipaddr, netmask, gw; /* ����IP��ַ */
struct netif enc28j60_netif; /* ��������ӿ� */
u32_t last_arp_time;
u32_t last_tcp_time;
u32_t last_ipreass_time;
u32_t input_time;

void LWIP_Polling ( void ) { /* LWIP��ѯ */
    if ( timer_expired ( &input_time, 5 ) ) {
        ethernetif_input ( &enc28j60_netif );
    }

    if ( timer_expired ( &last_tcp_time, TCP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* TCP����ʱ�� */
        tcp_tmr();
    }

    if ( timer_expired ( &last_arp_time, ARP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* ARP����ʱ�� */
        etharp_tmr();
    }

    if ( timer_expired ( &last_ipreass_time, IP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* IP������װ��ʱ�� */
        ip_reass_tmr();
    }

#if LWIP_DHCP > 0

    if ( timer_expired ( &last_dhcp_fine_time, DHCP_FINE_TIMER_MSECS / CLOCKTICKS_PER_MS ) ) {
        dhcp_fine_tmr();
    }

    if ( timer_expired ( &last_dhcp_coarse_time, DHCP_COARSE_TIMER_MSECS / CLOCKTICKS_PER_MS ) ) {
        dhcp_coarse_tmr();
    }

#endif
}

int main ( void ) {
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();
    IP4_ADDR ( &ipaddr, 192, 168, 2, 30 );
    IP4_ADDR ( &gw, 192, 168, 2, 1 );
    IP4_ADDR ( &netmask, 255, 255, 255, 0 );
    init_lwip_timer(); /* ��ʼ��LWIP��ʱ�� */

    while ( ENC28J60_Init() ) {
        printf ( "ENC28J60 init fail\r\n" );
    }

    lwip_init(); /* ��ʼ��LWIPЭ��ջ��ִ�м���û����п����õ�ֵ����ʼ�����е�ģ�� */

    /* �������ӿ� */
    while ( ( netif_add ( &enc28j60_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input ) == NULL ) ) {
        delay_ms ( 200 );
    }

    netif_set_default ( &enc28j60_netif ); /* ע��Ĭ�ϵ�����ӿ� */
    netif_set_up ( &enc28j60_netif ); /* ��������ӿ����ڴ���ͨ�� */
    Init_UDP_Server();

    while ( 1 ) {
        static int i = 0;
        LWIP_Polling();
        delay_ms ( 10 );

        if ( i++ > 20 ) {
            i = 0;
            LED0 = !LED0;
            sprintf ( ( char * ) lwip_send_buf, "This is a Udp Server\r\n" );
            lwip_flag |= LWIP_SEND_DATA;
            udp_server_send_data(); /* �������� */
        }

        if ( lwip_flag & LWIP_NEW_DATA ) { /* �������� */
            printf ( "Receive: %s\r\n", lwip_recv_buf );
            lwip_flag &= ~LWIP_NEW_DATA;
        }
    }
}
