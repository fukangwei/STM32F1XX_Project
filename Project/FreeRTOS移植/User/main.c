#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      128
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define LED0_TASK_PRIO      2
#define LED0_STK_SIZE       50
TaskHandle_t LED0Task_Handler;
void led0_task ( void *pvParameters );

#define PRINT_TASK_PRIO   3
#define PRINT_STK_SIZE    50
TaskHandle_t Print_Task_Handler;
void print_task ( void *pvParameters );

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
    xTaskCreate ( ( TaskFunction_t ) led0_task,
                  ( const char*    ) "led0_task",
                  ( uint16_t       ) LED0_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) LED0_TASK_PRIO,
                  ( TaskHandle_t*  ) &LED0Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) print_task,
                  ( const char*    ) "print_task",
                  ( uint16_t       ) PRINT_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) PRINT_TASK_PRIO,
                  ( TaskHandle_t*  ) &Print_Task_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void led0_task ( void *pvParameters ) {
    while ( 1 ) {
        LED0 = ~LED0;
        vTaskDelay ( 500 );
    }
}

void print_task ( void *pvParameters ) {
    while ( 1 ) {
        printf ( "hello, FreeRTOS" );
        vTaskDelay ( 1000 );
    }
}
