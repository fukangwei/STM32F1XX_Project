#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "wdg.h"
#include "led.h"

#define FEED_DOG 1

int main ( void ) {
    SystemInit();
    delay_init ( 72 );
    NVIC_Configuration();
    LED_Init();
    uart_init ( 9600 );
    IWDG_Init ( 4, 625 ); /* 初始化看门狗，溢出时间为1s */
    delay_ms ( 500 );
    LED0 = 0;

    while ( 1 ) {
        printf ( "hello world\r\n" );
#if FEED_DOG
        IWDG_Feed(); /* 喂狗操作 */
#endif
        delay_ms ( 500 );
    }
}
