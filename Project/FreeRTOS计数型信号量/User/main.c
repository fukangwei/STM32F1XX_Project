#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define START_TASK_PRIO         1
#define START_STK_SIZE          256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define SEMAPGIVE_TASK_PRIO     2
#define SEMAPGIVE_STK_SIZE      256
TaskHandle_t SemapGiveTask_Handler;
void SemapGive_task ( void *pvParameters );

#define SEMAPTAKE_TASK_PRIO     3
#define SEMAPTAKE_STK_SIZE      256
TaskHandle_t SemapTakeTask_Handler;
void SemapTake_task ( void *pvParameters );

SemaphoreHandle_t CountSemaphore; /* 计数型信号量句柄 */

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
    CountSemaphore = xSemaphoreCreateCounting ( 255, 0 ); /* 创建计数型信号量 */
    xTaskCreate ( ( TaskFunction_t ) SemapGive_task,
                  ( const char*    ) "semapgive_task",
                  ( uint16_t       ) SEMAPGIVE_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) SEMAPGIVE_TASK_PRIO,
                  ( TaskHandle_t*  ) &SemapGiveTask_Handler );
    xTaskCreate ( ( TaskFunction_t ) SemapTake_task,
                  ( const char*    ) "semaptake_task",
                  ( uint16_t       ) SEMAPTAKE_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) SEMAPTAKE_TASK_PRIO,
                  ( TaskHandle_t*  ) &SemapTakeTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void SemapGive_task ( void *pvParameters ) {
    u8 key, i = 0;
    u8 semavalue;
    BaseType_t err;

    while ( 1 ) {
        key = KEY_Scan ( 0 );

        if ( CountSemaphore != NULL ) { /* 计数型信号量创建成功 */
            switch ( key ) {
                case WKUP_PRES:
                    err = xSemaphoreGive ( CountSemaphore ); /* 释放计数型信号量 */

                    if ( err == pdFALSE ) {
                        printf ( "信号量释放失败！\r\n" );
                    }

                    semavalue = uxSemaphoreGetCount ( CountSemaphore ); /* 获取计数型信号量值 */
                    printf ( "%s, %d\r\n", __func__, semavalue );
                    break;
            }
        }

        i++;

        if ( i == 50 ) {
            i = 0;
            LED0 = !LED0;
        }

        vTaskDelay ( 10 );
    }
}

void SemapTake_task ( void *pvParameters ) {
    u8 semavalue;

    while ( 1 ) {
        xSemaphoreTake ( CountSemaphore, portMAX_DELAY ); /* 等待计数型信号量值 */
        semavalue = uxSemaphoreGetCount ( CountSemaphore ); /* 获取计数型信号量值 */
        printf ( "%s, %d\r\n", __func__, semavalue );
        LED1 = !LED1;
        vTaskDelay ( 1000 );
    }
}
