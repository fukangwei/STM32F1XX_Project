#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define START_TASK_PRIO         1
#define START_STK_SIZE          256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TIMERCONTROL_TASK_PRIO  2
#define TIMERCONTROL_STK_SIZE   256
TaskHandle_t TimerControlTask_Handler;
void timercontrol_task ( void *pvParameters );

TimerHandle_t AutoReloadTimer_Handle; /* ���ڶ�ʱ����� */
TimerHandle_t OneShotTimer_Handle;    /* ���ζ�ʱ����� */

void AutoReloadCallback ( TimerHandle_t xTimer ); /* ���ڶ�ʱ���ص����� */
void OneShotCallback ( TimerHandle_t xTimer );    /* ���ζ�ʱ���ص����� */

int main ( void ) {
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );
    delay_init();
    uart_init ( 9600 );
    LED_Init();
    KEY_Init();
    xTaskCreate ( ( TaskFunction_t ) start_task,
                  ( const char*    ) "start_task",
                  ( uint16_t       ) START_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) START_TASK_PRIO,
                  ( TaskHandle_t*  ) &StartTask_Handler );
    vTaskStartScheduler();
}

void start_task ( void *pvParameters ) {
    taskENTER_CRITICAL();
    /* ���ڶ�ʱ��������Ϊ1s(1000��ʱ�ӽ���)������ģʽ */
    AutoReloadTimer_Handle = xTimerCreate ( ( const char*        ) "AutoReloadTimer",
                                            ( TickType_t         ) 1000,
                                            ( UBaseType_t        ) pdTRUE,
                                            ( void*              ) 1,
                                            ( TimerCallbackFunction_t ) AutoReloadCallback );
    /* ���ζ�ʱ��������2s(2000��ʱ�ӽ���)������ģʽ */
    OneShotTimer_Handle = xTimerCreate ( ( const char*           ) "OneShotTimer",
                                         ( TickType_t            ) 2000,
                                         ( UBaseType_t           ) pdFALSE,
                                         ( void*                 ) 2,
                                         ( TimerCallbackFunction_t ) OneShotCallback );
    xTaskCreate ( ( TaskFunction_t ) timercontrol_task,
                  ( const char*    ) "timercontrol_task",
                  ( uint16_t       ) TIMERCONTROL_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TIMERCONTROL_TASK_PRIO,
                  ( TaskHandle_t*  ) &TimerControlTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void timercontrol_task ( void *pvParameters ) {
    u8 key, num;

    while ( 1 ) {
        if ( ( AutoReloadTimer_Handle != NULL ) && ( OneShotTimer_Handle != NULL ) ) {
            key = KEY_Scan ( 0 );

            switch ( key ) {
                case WKUP_PRES:
                    xTimerStart ( AutoReloadTimer_Handle, 0 ); /* �������ڶ�ʱ�� */
                    printf ( "������ʱ��1\r\n" );
                    break;

                case KEY0_PRES:
                    xTimerStart ( OneShotTimer_Handle, 0 ); /* �������ζ�ʱ�� */
                    printf ( "������ʱ��2\r\n" );
                    break;

                case KEY1_PRES:
                    xTimerStop ( AutoReloadTimer_Handle, 0 ); /* �ر����ڶ�ʱ�� */
                    xTimerStop ( OneShotTimer_Handle, 0 );  /* �رյ��ζ�ʱ�� */
                    printf ( "�رն�ʱ��1��2\r\n" );
                    break;
            }
        }

        num++;

        if ( num == 50 ) {
            num = 0;
            LED0 = !LED0;
        }

        vTaskDelay ( 10 );
    }
}

void AutoReloadCallback ( TimerHandle_t xTimer ) {
    static u16 tmr1_num = 0;
    tmr1_num++;
    printf ( "%s, %d\r\n", __func__, tmr1_num );
}

void OneShotCallback ( TimerHandle_t xTimer ) {
    static u16 tmr2_num = 0;
    tmr2_num++;
    printf ( "%s, %d\r\n", __func__, tmr2_num );
    LED1 = !LED1;
    printf ( "��ʱ��2���н���\r\n" );
}
