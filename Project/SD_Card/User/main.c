#include "led.h"
#include "delay.h"
#include "sys.h"
#include "stdlib.h"
#include "usart.h"
#include "MMC_SD.h"

/* 读取SD卡的指定扇区的内容，sec是扇区物理地址编号 */
void SD_Read_Sectorx ( u32 sec ) {
    u8 *buf = NULL;
    u16 i = 0;
    buf = malloc ( 512 );

    if ( SD_ReadDisk ( buf, sec, 1 ) == 0 ) {
        printf ( "SECTOR 0 DATA: \r\n" );

        for ( i = 0; i < 512; i++ ) {
            printf ( "%x ", buf[i] );
        }

        printf ( "\r\nREAD END\r\n" );
    }

    free ( buf );
}

int main ( void ) {
    u32 sd_size;
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

    sd_size = SD_GetSectorCount();
    printf ( "sd_size is %dMB\r\n", sd_size >> 11 );

    while ( 1 ) {
        SD_Read_Sectorx ( 0 );
        delay_ms ( 1000 );
        LED0 = !LED0;
    }
}
