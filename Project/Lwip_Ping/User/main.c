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

#define CLOCKTICKS_PER_MS 10 /* 定义时钟节拍 */

static ip_addr_t ipaddr, netmask, gw; /* 定义IP地址 */
struct netif enc28j60_netif; /* 定义网络接口 */
u32_t last_arp_time;
u32_t last_tcp_time;
u32_t last_ipreass_time;
u32_t input_time;

void LWIP_Polling ( void ) { /* LWIP轮询 */
    if ( timer_expired ( &input_time, 5 ) ) {
        ethernetif_input ( &enc28j60_netif );
    }

    if ( timer_expired ( &last_tcp_time, TCP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* TCP处理定时器 */
        tcp_tmr();
    }

    if ( timer_expired ( &last_arp_time, ARP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* ARP处理定时器 */
        etharp_tmr();
    }

    if ( timer_expired ( &last_ipreass_time, IP_TMR_INTERVAL / CLOCKTICKS_PER_MS ) ) { /* IP重新组装定时器 */
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
    init_lwip_timer(); /* 初始化LWIP定时器 */

    while ( ENC28J60_Init() ) {
        printf ( "ENC28J60 init fail\r\n" );
    }

    lwip_init(); /* 初始化LWIP协议栈，执行检查用户所有可配置的值，初始化所有的模块 */

    /* 添加网络接口 */
    while ( ( netif_add ( &enc28j60_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input ) == NULL ) ) {
        delay_ms ( 200 );
    }

    netif_set_default ( &enc28j60_netif ); /* 注册默认的网络接口 */
    netif_set_up ( &enc28j60_netif ); /* 建立网络接口用于处理通信 */

    while ( 1 ) {
        static int i = 0;
        LWIP_Polling();
        delay_ms ( 10 );

        if ( i++ > 20 ) {
            i = 0;
            LED0 = !LED0;
        }
    }
}
