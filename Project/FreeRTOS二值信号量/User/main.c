#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      256
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO     2
#define TASK1_STK_SIZE      256
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define DATAPROCESS_TASK_PRIO 3
#define DATAPROCESS_STK_SIZE  256
TaskHandle_t DataProcess_Handler;
void DataProcess_task ( void *pvParameters );

SemaphoreHandle_t BinarySemaphore; /* 二值信号量句柄 */

/* 用于命令解析的命令值 */
#define LED1ON      1
#define LED1OFF     2
#define COMMANDERR  0XFF

/* 将字符串中的小写字母转换为大写 */
void LowerToCap ( u8 *str, u8 len ) {
    u8 i;

    for ( i = 0; i < len; i++ ) {
        if ( ( 96 < str[i] ) && ( str[i] < 123 ) ) {
            str[i] = str[i] - 32;
        }
    }
}

/* 将字符串命令转换成命令值 */
u8 CommandProcess ( u8 *str ) {
    u8 CommandValue = COMMANDERR;

    if ( strcmp ( ( char* ) str, "LED1ON" ) == 0 ) {
        CommandValue = LED1ON;
    } else if ( strcmp ( ( char* ) str, "LED1OFF" ) == 0 ) {
        CommandValue = LED1OFF;
    }

    return CommandValue;
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
    BinarySemaphore = xSemaphoreCreateBinary(); /* 创建二值信号量 */
    xTaskCreate ( ( TaskFunction_t ) task1_task,
                  ( const char*    ) "task1_task",
                  ( uint16_t       ) TASK1_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK1_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task1Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) DataProcess_task,
                  ( const char*    ) "keyprocess_task",
                  ( uint16_t       ) DATAPROCESS_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) DATAPROCESS_TASK_PRIO,
                  ( TaskHandle_t*  ) &DataProcess_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    while ( 1 ) {
        LED0 = !LED0;
        vTaskDelay ( 500 );
    }
}

void DataProcess_task ( void *pvParameters ) {
    u8 len = 0;
    u8 CommandValue = COMMANDERR;
    BaseType_t err = pdFALSE;
    u8 CommandStr[30] = {0};

    while ( 1 ) {
        if ( BinarySemaphore != NULL ) {
            err = xSemaphoreTake ( BinarySemaphore, portMAX_DELAY ); /* 获取信号量 */

            if ( err == pdTRUE ) { /* 获取信号量成功 */
                len = USART_RX_STA & 0x3fff; /* 得到此次接收到的数据长度 */
                sprintf ( ( char* ) CommandStr, "%s", USART_RX_BUF );
                CommandStr[len] = '\0';
                LowerToCap ( CommandStr, len );
                CommandValue = CommandProcess ( CommandStr ); /* 命令解析 */

                if ( CommandValue != COMMANDERR ) {
                    printf ( "命令为：%s\r\n", CommandStr );

                    switch ( CommandValue ) { /* 处理命令 */
                        case LED1ON:
                            LED1 = 0;
                            break;

                        case LED1OFF:
                            LED1 = 1;
                            break;
                    }
                } else {
                    printf ( "无效的命令，请重新输入!!\r\n" );
                }

                USART_RX_STA = 0;
                memset ( USART_RX_BUF, 0, USART_REC_LEN );
                memset ( CommandStr, 0, 30 );
            }
        } else if ( err == pdFALSE ) {
            vTaskDelay ( 10 );
        }
    }
}
