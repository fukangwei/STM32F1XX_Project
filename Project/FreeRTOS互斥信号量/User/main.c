#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define START_TASK_PRIO         1
#define START_STK_SIZE          256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define LOW_TASK_PRIO           2
#define LOW_STK_SIZE            256
TaskHandle_t LowTask_Handler;
void low_task ( void *pvParameters );

#define MIDDLE_TASK_PRIO        3
#define MIDDLE_STK_SIZE         256
TaskHandle_t MiddleTask_Handler;
void middle_task ( void *pvParameters );

#define HIGH_TASK_PRIO          4
#define HIGH_STK_SIZE           256
TaskHandle_t HighTask_Handler;
void high_task ( void *pvParameters );

SemaphoreHandle_t MutexSemaphore; /* 互斥信号量句柄 */

int main ( void ) {
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );
    delay_init();
    uart_init ( 9600 );
    LED_Init();
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
    MutexSemaphore = xSemaphoreCreateMutex(); /* 创建互斥信号量 */
    xTaskCreate ( ( TaskFunction_t ) high_task,
                  ( const char*    ) "high_task",
                  ( uint16_t       ) HIGH_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) HIGH_TASK_PRIO,
                  ( TaskHandle_t*  ) &HighTask_Handler );
    xTaskCreate ( ( TaskFunction_t ) middle_task,
                  ( const char*    ) "middle_task",
                  ( uint16_t       ) MIDDLE_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) MIDDLE_TASK_PRIO,
                  ( TaskHandle_t*  ) &MiddleTask_Handler );
    xTaskCreate ( ( TaskFunction_t ) low_task,
                  ( const char*    ) "low_task",
                  ( uint16_t       ) LOW_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) LOW_TASK_PRIO,
                  ( TaskHandle_t*  ) &LowTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void high_task ( void *pvParameters ) {
    while ( 1 ) {
        vTaskDelay ( 500 );
        printf ( "high task Pend Sem\r\n" );
        xSemaphoreTake ( MutexSemaphore, portMAX_DELAY ); /* 获取互斥信号量 */
        printf ( "high task Running!\r\n" );
        LED1 = !LED1;
        xSemaphoreGive ( MutexSemaphore ); /* 释放互斥信号量 */
        vTaskDelay ( 500 );
    }
}

void middle_task ( void *pvParameters ) {
    while ( 1 ) {
        printf ( "middle task Running!\r\n" );
        LED0 = !LED0;
        vTaskDelay ( 1000 );
    }
}

void low_task ( void *pvParameters ) {
    static u32 times;

    while ( 1 ) {
        xSemaphoreTake ( MutexSemaphore, portMAX_DELAY ); /* 获取互斥信号量 */
        printf ( "low task Running!\r\n" );

        for ( times = 0; times < 1000000; times++ ) {
            taskYIELD(); /* 发起任务调度 */
        }

        xSemaphoreGive ( MutexSemaphore ); /* 释放互斥信号量 */
        vTaskDelay ( 1000 );
    }
}
