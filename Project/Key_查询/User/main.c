#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "led.h"
#include "key.h"

int main ( void ) {
    SystemInit();
    delay_init ( 72 );
    NVIC_Configuration();
    uart_init ( 9600 );
    LED_Init();
    KEY_Init();
    LED0 = 0;

    while ( 1 ) {
        u8 t = KEY_Scan ( 0 );

        switch ( t ) {
            case KEY0_PRES:
                LED0 = !LED0;
                break;

            case KEY1_PRES:
                LED1 = !LED1;
                break;

            case WKUP_PRES:
                LED0 = !LED0;
                LED1 = !LED1;
                break;

            default:
                delay_ms ( 10 );
        }
    }
}
