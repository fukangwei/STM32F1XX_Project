#include "delay.h"
#include "led.h"
#include "usart.h"
#include "key.h"
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
#include "tcp_client.h"

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
    struct tcp_pcb *tcppcb;     //����һ��TCP���������ƿ�
    struct ip_addr rmtipaddr;   //Զ��ip��ַ
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();
    KEY_Init();
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
    tcppcb = tcp_new(); /* ����һ���µ�pcb */
    IP4_ADDR ( &rmtipaddr, 192, 168, 2, 3 );
    /* ���ӵ�Ŀ�ĵ�ַ��ָ���˿��ϣ������ӳɹ���ִ�лص�����tcp_client_connected */
    tcp_connect ( tcppcb, &rmtipaddr, TCP_SERVER_PORT, tcp_client_connected );

    while ( 1 ) {
        u8 key = KEY_Scan ( 0 );
        static int i = 0;
        LWIP_Polling();

        if ( tcp_client_flag & LWIP_NEW_DATA ) { /* �Ƿ��յ����� */
            printf ( "Receive: %s\r\n", tcp_client_recvbuf ); /* ��ʾ���յ������� */
            tcp_client_flag &= ~ LWIP_NEW_DATA; /* ��������Ѿ��������� */
        }

        if ( key == KEY0_PRES ) { /* KEY0�����ˣ�Ҫ�������� */
            sprintf ( ( char * ) tcp_client_sendbuf, "This is a TCP Client\r\n" );
            tcp_client_flag |= LWIP_SEND_DATA; /* ���Ҫ�������� */
        }

        if ( key == KEY1_PRES ) { /* KEY1�����ˣ�Ҫ�Ͽ����� */
            printf ( "Close the connection\r\n" );
            tcp_client_connection_close ( tcppcb, 0 ); /* �Ͽ����� */
        }

        if ( i++ > 200 ) {
            i = 0;
            LED0 = !LED0;

            if ( tcp_client_flag & LWIP_CONNECTED ) { /* �Ƿ�������״̬ */
                printf ( "STATUS: Connected\r\n" );
            } else { /* ���δ��������״̬���������� */
                printf ( "STATUS: Disconnected\r\n" );
                tcp_client_connection_close ( tcppcb, 0 ); /* �ر����� */
                tcppcb = tcp_new();
                tcp_connect ( tcppcb, &rmtipaddr, TCP_SERVER_PORT, tcp_client_connected );
            }
        }

        delay_ms ( 10 );
    }
}
