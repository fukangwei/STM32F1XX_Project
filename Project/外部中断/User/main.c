#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"

int main ( void ) {
    SystemInit();
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();
    KEY_Init();
    EXTIX_Init();
    LED0 = 0;

    while ( 1 ) {
        printf ( "OK\n" );
        delay_ms ( 1000 );
    }
}
