#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "dma.h"

const u8 TEXT_TO_SEND[] = {"ALIENTEK Mini STM32 DMA 串口实验"};

#define TEXT_LENTH  sizeof(TEXT_TO_SEND) - 1 /* TEXT_TO_SEND字符串长度 */

u8 SendBuff[ ( TEXT_LENTH + 2 ) * 100 ] = {0};

int main ( void ) {
    delay_init ( 72 );
    uart_init ( 9600 );
    LED_Init();
    LCD_Init();
    KEY_Init();
    /* DMA1通道4，外设为串口1，存储器地址为SendBuff，长度为(TEXT_LENTH + 2) * 100 */
    MYDMA_Config ( DMA1_Channel4, ( u32 ) &USART1->DR, ( u32 ) SendBuff, ( TEXT_LENTH + 2 ) * 100 );
    POINT_COLOR = RED;
    LCD_ShowString ( 60, 130, 200, 16, 16, ( u8 * ) "KEY0: Start" );

    for ( int i = 0; i < ( TEXT_LENTH + 2 ) * 100; i++ ) {
        static u8 t = 0;

        if ( t >= TEXT_LENTH ) {
            SendBuff[i++] = 0x0d;
            SendBuff[i] = 0x0a;
            t = 0;
        } else {
            SendBuff[i] = TEXT_TO_SEND[t++];
        }
    }

    POINT_COLOR = BLUE;

    while ( 1 ) {
        static u16 i = 0;
        u8 key_index = KEY_Scan ( 0 );

        if ( key_index == KEY0_PRES ) {
            float pro = 0; /* 数据传输进度 */
            LCD_ShowString ( 60, 150, 200, 16, 16, ( u8 * ) "Start Transimit...." );
            LCD_ShowString ( 60, 170, 200, 16, 16, ( u8 * ) "   %" ); /* 显示百分号 */
            printf ( "\r\nDMA DATA: \r\n " );
            USART_DMACmd ( USART1, USART_DMAReq_Tx, ENABLE );
            MYDMA_Enable ( DMA1_Channel4 ); /* 开始一次DMA传输 */

            while ( 1 ) { /* 实际应用中，在传输数据期间，可以执行另外的任务 */
                if ( DMA_GetFlagStatus ( DMA1_FLAG_TC4 ) != RESET ) { /* 等待通道4传输完成 */
                    DMA_ClearFlag ( DMA1_FLAG_TC4 ); /* 清除通道4传输完成标志 */
                    break;
                }

                pro = DMA_GetCurrDataCounter ( DMA1_Channel4 ); /* 得到当前还剩余多少个数据 */
                pro = 1 - pro / ( ( TEXT_LENTH + 2 ) * 100 ); /* 得到百分比 */
                pro *= 100; /* 扩大100倍 */
                LCD_ShowNum ( 60, 170, pro, 3, 16 );
            }

            LCD_ShowNum ( 60, 170, 100, 3, 16 ); /* 显示100% */
            LCD_ShowString ( 60, 150, 200, 16, 16, ( u8 * ) "Transimit Finished!" ); /* 提示传送完成 */
        }

        delay_ms ( 10 );

        if ( i++ == 20 ) {
            LED0 = !LED0; /* 提示系统正在运行 */
            i = 0;
        }
    }
}
