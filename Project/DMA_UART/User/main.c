#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "dma.h"

const u8 TEXT_TO_SEND[] = {"ALIENTEK Mini STM32 DMA ����ʵ��"};

#define TEXT_LENTH  sizeof(TEXT_TO_SEND) - 1 /* TEXT_TO_SEND�ַ������� */

u8 SendBuff[ ( TEXT_LENTH + 2 ) * 100 ] = {0};

int main ( void ) {
    delay_init ( 72 );
    uart_init ( 9600 );
    LED_Init();
    LCD_Init();
    KEY_Init();
    /* DMA1ͨ��4������Ϊ����1���洢����ַΪSendBuff������Ϊ(TEXT_LENTH + 2) * 100 */
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
            float pro = 0; /* ���ݴ������ */
            LCD_ShowString ( 60, 150, 200, 16, 16, ( u8 * ) "Start Transimit...." );
            LCD_ShowString ( 60, 170, 200, 16, 16, ( u8 * ) "   %" ); /* ��ʾ�ٷֺ� */
            printf ( "\r\nDMA DATA: \r\n " );
            USART_DMACmd ( USART1, USART_DMAReq_Tx, ENABLE );
            MYDMA_Enable ( DMA1_Channel4 ); /* ��ʼһ��DMA���� */

            while ( 1 ) { /* ʵ��Ӧ���У��ڴ��������ڼ䣬����ִ����������� */
                if ( DMA_GetFlagStatus ( DMA1_FLAG_TC4 ) != RESET ) { /* �ȴ�ͨ��4������� */
                    DMA_ClearFlag ( DMA1_FLAG_TC4 ); /* ���ͨ��4������ɱ�־ */
                    break;
                }

                pro = DMA_GetCurrDataCounter ( DMA1_Channel4 ); /* �õ���ǰ��ʣ����ٸ����� */
                pro = 1 - pro / ( ( TEXT_LENTH + 2 ) * 100 ); /* �õ��ٷֱ� */
                pro *= 100; /* ����100�� */
                LCD_ShowNum ( 60, 170, pro, 3, 16 );
            }

            LCD_ShowNum ( 60, 170, 100, 3, 16 ); /* ��ʾ100% */
            LCD_ShowString ( 60, 150, 200, 16, 16, ( u8 * ) "Transimit Finished!" ); /* ��ʾ������� */
        }

        delay_ms ( 10 );

        if ( i++ == 20 ) {
            LED0 = !LED0; /* ��ʾϵͳ�������� */
            i = 0;
        }
    }
}
