#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO  1
#define START_STK_SIZE   128
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define KEY_TASK_PRIO  2
#define KEY_STK_SIZE   128
TaskHandle_t KeyTask_Handler;
void key_task ( void *pvParameters );

#define TASK1_TASK_PRIO  3
#define TASK1_STK_SIZE   128
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define TASK2_TASK_PRIO  4
#define TASK2_STK_SIZE   128
TaskHandle_t Task2Task_Handler;
void task2_task ( void *pvParameters );

int main ( void ) {
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );
    delay_init();
    uart_init ( 9600 );
    LED_Init();
    KEY_Init();
    EXTIX_Init();
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
    xTaskCreate ( ( TaskFunction_t ) key_task,
                  ( const char*    ) "key_task",
                  ( uint16_t       ) KEY_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) KEY_TASK_PRIO,
                  ( TaskHandle_t*  ) &KeyTask_Handler );
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
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void key_task ( void *pvParameters ) {
    u8 key, statflag = 0;

    while ( 1 ) {
        key = KEY_Scan ( 0 );

        switch ( key ) {
            case WKUP_PRES:
                statflag = !statflag;

                if ( statflag == 1 ) {
                    vTaskSuspend ( Task1Task_Handler ); /* 挂起任务 */
                    printf ( "挂起任务1的运行!\r\n" );
                } else if ( statflag == 0 ) {
                    vTaskResume ( Task1Task_Handler ); /* 回复任务 */
                    printf ( "恢复任务1的运行!\r\n" );
                }

                break;

            case KEY1_PRES:
                vTaskSuspend ( Task2Task_Handler );
                printf ( "挂起任务2的运行!\r\n" );
                break;
        }

        vTaskDelay ( 10 );
    }
}

void task1_task ( void *pvParameters ) {
    u16 task1_num = 0;

    while ( 1 ) {
        task1_num++;
        LED0 = !LED0;
        printf ( "任务1已经执行：%d次\r\n", task1_num );
        vTaskDelay ( 1000 );
    }
}

void task2_task ( void *pvParameters ) {
    u16 task2_num = 0;

    while ( 1 ) {
        task2_num++;
        LED1 = !LED1;
        printf ( "任务2已经执行：%d次\r\n", task2_num );
        vTaskDelay ( 1000 );
    }
}
