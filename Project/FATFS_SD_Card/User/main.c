#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "MMC_SD.h"
#include "ff.h"
#include "string.h"

FATFS fs;
FIL fsrc;
UINT bw;

int main ( void ) {
    char buffer[] = "hello, world";
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_2 );
    delay_init ( 72 );
    uart_init ( 9600 );
    LED_Init();
    delay_ms ( 1000 );

    while ( SD_Initialize() ) {
        printf ( "SD Card Error!\r\n" );
        delay_ms ( 500 );
        printf ( "Please Check!\r\n" );
        delay_ms ( 500 );
        LED0 = !LED0;
    }

    printf ( "SD Card Checked OK\r\n" );
    f_mount ( &fs, "0:", 1 ); /* π“‘ÿSDø® */
    f_open ( &fsrc, "test.txt", FA_OPEN_ALWAYS | FA_WRITE );
    f_write ( &fsrc, &buffer, strlen ( buffer ), &bw );
    f_close ( &fsrc );
    f_mount ( NULL, "0:", 1 ); /* –∂‘ÿSDø® */
    printf ( "Write file OK\r\n" );
}
