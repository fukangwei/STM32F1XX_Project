#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"

int main ( void ) {
    u8 lcd_id[20] = {0};
    delay_init ( 72 );
    uart_init ( 9600 );
    LED_Init();
    LCD_Init();
    POINT_COLOR = RED;
    sprintf ( ( char* ) lcd_id, "LCD ID: %04X", lcddev.id );

    while ( 1 ) {
        static u8 x = 0;

        switch ( x ) {
            case 0:
                LCD_Clear ( WHITE );
                break;

            case 1:
                LCD_Clear ( BLACK );
                break;

            case 2:
                LCD_Clear ( BLUE );
                break;

            case 3:
                LCD_Clear ( RED );
                break;

            case 4:
                LCD_Clear ( MAGENTA );
                break;

            case 5:
                LCD_Clear ( GREEN );
                break;

            case 6:
                LCD_Clear ( CYAN );
                break;

            case 7:
                LCD_Clear ( YELLOW );
                break;

            case 8:
                LCD_Clear ( BRRED );
                break;

            case 9:
                LCD_Clear ( GRAY );
                break;

            case 10:
                LCD_Clear ( LGRAY );
                break;

            case 11:
                LCD_Clear ( BROWN );
                break;
        }

        POINT_COLOR = RED;
        LCD_ShowString ( 30, 40, 200, 24, 24, ( u8 * ) "Mini STM32 ^_^" );
        LCD_ShowString ( 30, 70, 200, 16, 16, ( u8 * ) "TFTLCD TEST" );
        LCD_ShowString ( 30, 90, 200, 16, 16, ( u8 * ) "ATOM@ALIENTEK" );
        LCD_ShowString ( 30, 110, 200, 16, 16, lcd_id );
        LCD_ShowString ( 30, 130, 200, 12, 12, ( u8 * ) "2014/3/7" );

        if ( x++ == 12 ) {
            x = 0;
        }

        LED0 = !LED0;
        delay_ms ( 1000 );
    }
}
