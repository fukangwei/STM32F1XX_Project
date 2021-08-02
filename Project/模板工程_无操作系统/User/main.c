#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"

int main ( void ) {
    SystemInit();
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();

    while ( 1 ) {
        LED0 = !LED0;
        LED1 = !LED1;
        delay_ms ( 500 );
    }
}
