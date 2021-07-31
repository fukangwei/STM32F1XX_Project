#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#include "timerx.h"

int main ( void ) {
    SystemInit();
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();
    TIM3_Int_Init( 4999, 7199 ); /* 定时时间为500ms */

    while ( 1 ) {
        delay_ms ( 1000 );
    }
}
