#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO  1
#define START_STK_SIZE   256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define INTERRUPT_TASK_PRIO  2
#define INTERRUPT_STK_SIZE   256
TaskHandle_t INTERRUPTTask_Handler;
void interrupt_task ( void *p_arg );

int main ( void ) {
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );
    delay_init();
    uart_init ( 9600 );
    LED_Init();
    TIM3_Int_Init ( 10000 - 1, 7200 - 1 ); /* 初始化定时器3，定时器周期1s */
    TIM5_Int_Init ( 10000 - 1, 7200 - 1 ); /* 初始化定时器5，定时器周期1s */
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
    xTaskCreate ( ( TaskFunction_t ) interrupt_task,
                  ( const char*    ) "interrupt_task",
                  ( uint16_t       ) INTERRUPT_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) INTERRUPT_TASK_PRIO,
                  ( TaskHandle_t*  ) &INTERRUPTTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void interrupt_task ( void *pvParameters ) {
    static u32 total_num = 0;

    while ( 1 ) {
        total_num += 1;

        if ( total_num == 20 ) {
            printf ( "关闭中断.............\r\n" );
            portDISABLE_INTERRUPTS();
            delay_xms ( 5000 );
            printf ( "打开中断.............\r\n" );
            portENABLE_INTERRUPTS();
        }

        LED0 = ~LED0;
        vTaskDelay ( 1000 );
    }
}
