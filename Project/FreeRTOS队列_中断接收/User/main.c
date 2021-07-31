#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timer.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO     2
#define TASK1_STK_SIZE      256
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define MESSAGE_Q_NUM      4 /* ��Ϣ���е�Ԫ���� */
QueueHandle_t Message_Queue; /* ��Ϣ���о�� */

/* ��ѯMessage_Queue�е��ܶ���������ʣ��������� */
void check_msg_queue ( void ) {
    u8 msgq_remain_size; /* ��Ϣ����ʣ���С */
    u8 msgq_total_size;  /* ��Ϣ�����ܴ�С */
    taskENTER_CRITICAL();
    msgq_remain_size = uxQueueSpacesAvailable ( Message_Queue );
    /* �õ���Ϣ�����ܴ�С���ܴ�С = ʹ�� + ʣ��� */
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
    TIM2_Int_Init ( 5000, 7200 - 1 ); /* ��ʼ����ʱ��2������Ϊ500ms */
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
