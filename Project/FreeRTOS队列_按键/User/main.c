#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
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

#define KEYPROCESS_TASK_PRIO  3
#define KEYPROCESS_STK_SIZE   256
TaskHandle_t Keyprocess_Handler;
void Keyprocess_task ( void *pvParameters );

#define KEYMSG_Q_NUM     1 /* 按键消息队列的元素数 */
QueueHandle_t Key_Queue;   /* 按键值消息队列句柄 */

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
    Key_Queue = xQueueCreate ( KEYMSG_Q_NUM, sizeof ( u8 ) ); /* 创建消息队列Key_Queue */
    xTaskCreate ( ( TaskFunction_t ) task1_task,
                  ( const char*    ) "task1_task",
                  ( uint16_t       ) TASK1_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK1_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task1Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) Keyprocess_task,
                  ( const char*    ) "keyprocess_task",
                  ( uint16_t       ) KEYPROCESS_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) KEYPROCESS_TASK_PRIO,
                  ( TaskHandle_t*  ) &Keyprocess_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    u8 key, i = 0;
    BaseType_t err;

    while ( 1 ) {
        key = KEY_Scan ( 0 );

        if ( ( Key_Queue != NULL ) && ( key ) ) { /* 消息队列Key_Queue创建成功，并且按键被按下 */
            err = xQueueSend ( Key_Queue, &key, 10 ); /* 向消息队列发送数据 */

            if ( err == errQUEUE_FULL ) {
                printf ( "队列Key_Queue已满，数据发送失败!\r\n" );
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

void Keyprocess_task ( void *pvParameters ) {
    u8 key = 0;

    while ( 1 ) {
        if ( Key_Queue != NULL ) {
            if ( xQueueReceive ( Key_Queue, &key, portMAX_DELAY ) ) {
                switch ( key ) {
                    case WKUP_PRES:
                        printf ( "I receive WKUP_PRES\r\n" );
                        break;

                    case KEY0_PRES:
                        printf ( "I receive KEY0_PRES\r\n" );
                        break;
                }
            }
        }

        vTaskDelay ( 10 );
    }
}
