#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      128
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO     2
#define TASK1_STK_SIZE      128
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define TASK2_TASK_PRIO     3
#define TASK2_STK_SIZE      128
TaskHandle_t Task2Task_Handler;
void task2_task ( void *pvParameters );

#define RUNTIMESTATS_TASK_PRIO  4
#define RUNTIMESTATS_STK_SIZE   128
TaskHandle_t RunTimeStats_Handler;
void RunTimeStats_task ( void *pvParameters );

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
    xTaskCreate ( ( TaskFunction_t ) task1_task,
                  ( const char*    ) "task1_task",
                  ( uint16_t       ) TASK1_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK1_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task1Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) task2_task,
                  ( const char*    ) "task2_task",
                  ( uint16_t       ) TASK2_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK2_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task2Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) RunTimeStats_task,
                  ( const char*    ) "RunTimeStats_task",
                  ( uint16_t       ) RUNTIMESTATS_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) RUNTIMESTATS_TASK_PRIO,
                  ( TaskHandle_t*  ) &RunTimeStats_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    while ( 1 ) {
        LED0 = !LED0;
        vTaskDelay ( 1000 );
    }
}

void task2_task ( void *pvParameters ) {
    while ( 1 ) {
        LED1 = !LED1;
        vTaskDelay ( 1000 );
    }
}

char RunTimeInfo[400];

void RunTimeStats_task ( void *pvParameters ) {
    u8 key = 0;

    while ( 1 ) {
        key = KEY_Scan ( 0 );

        if ( key == WKUP_PRES ) {
            memset ( RunTimeInfo, 0, 400 );
            vTaskGetRunTimeStats ( RunTimeInfo ); /* 获取任务运行时间信息 */
            printf ( "任务名\t\t\t运行时间\t运行所占百分比\r\n" );
            printf ( "%s\r\n", RunTimeInfo );
        }

        vTaskDelay ( 10 );
    }
}
