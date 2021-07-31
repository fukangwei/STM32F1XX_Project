#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO     2
#define TASK1_STK_SIZE      256
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define SHOW_RECEIVE_TASK_PRIO  3
#define SHOW_RECEIVE_STK_SIZE   256
TaskHandle_t show_receive_Handler;
void show_receive_task ( void *pvParameters );

#define MESSAGE_Q_NUM      4 /* 消息队列的元素数 */
QueueHandle_t Message_Queue; /* 消息队列句柄 */

/* 查询Message_Queue中的总队列数量和剩余队列数量 */
void check_msg_queue ( void ) {
    u8 msgq_remain_size; /* 消息队列剩余大小 */
    u8 msgq_total_size;  /* 消息队列总大小 */
    taskENTER_CRITICAL();
    msgq_remain_size = uxQueueSpacesAvailable ( Message_Queue );
    /* 得到消息队列总大小，总大小 = 使用 + 剩余的 */
    msgq_total_size = uxQueueMessagesWaiting ( Message_Queue ) + uxQueueSpacesAvailable ( Message_Queue );
    printf ( "Total Size: %d\r\n", msgq_total_size );
    printf ( "Remain Size: %d\r\n", msgq_remain_size );
    taskEXIT_CRITICAL();
}

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
    Message_Queue = xQueueCreate ( MESSAGE_Q_NUM, USART_REC_LEN );
    xTaskCreate ( ( TaskFunction_t ) task1_task,
                  ( const char*    ) "task1_task",
                  ( uint16_t       ) TASK1_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK1_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task1Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) show_receive_task,
                  ( const char*    ) "show_receive_task",
                  ( uint16_t       ) SHOW_RECEIVE_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) SHOW_RECEIVE_TASK_PRIO,
                  ( TaskHandle_t*  ) &show_receive_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    u8 i = 0;

    while ( 1 ) {
        i++;

        if ( i == 100 ) {
            i = 0;
            check_msg_queue();
            LED0 = !LED0;
        }

        vTaskDelay ( 10 );
    }
}

void show_receive_task ( void *pvParameters ) {
    u8 buffer[USART_REC_LEN] = {0};

    while ( 1 ) {
        if ( Message_Queue != NULL ) {
            if ( xQueueReceive ( Message_Queue, &buffer, portMAX_DELAY ) ) {
                printf ( "I receive %s\r\n", buffer );
                memset ( buffer, 0, USART_REC_LEN );
            }
        }

        vTaskDelay ( 1000 );
    }
}
