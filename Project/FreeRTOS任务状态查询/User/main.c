#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO     1
#define START_STK_SIZE      128
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define LED0_TASK_PRIO      2
#define LED0_STK_SIZE       128
TaskHandle_t Led0Task_Handler;
void led0_task ( void *pvParameters );

#define QUERY_TASK_PRIO     3
#define QUERY_STK_SIZE      256
TaskHandle_t QueryTask_Handler;
void query_task ( void *pvParameters );

char InfoBuffer[1000];

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
    xTaskCreate ( ( TaskFunction_t ) led0_task,
                  ( const char*    ) "led0_task",
                  ( uint16_t       ) LED0_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) LED0_TASK_PRIO,
                  ( TaskHandle_t*  ) &Led0Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) query_task,
                  ( const char*    ) "query_task",
                  ( uint16_t       ) QUERY_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) QUERY_TASK_PRIO,
                  ( TaskHandle_t*  ) &QueryTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void led0_task ( void *pvParameters ) {
    while ( 1 ) {
        LED0 = ~LED0;
        vTaskDelay ( 500 );
    }
}

void query_task ( void *pvParameters ) {
    u32 TotalRunTime;
    UBaseType_t ArraySize, x;
    TaskStatus_t *StatusArray;
    delay_ms ( 5000 );
    printf ( "/******** 第一步：函数uxTaskGetSystemState的使用 **********/\r\n" );
    ArraySize = uxTaskGetNumberOfTasks(); /* 获取系统任务数量 */
    StatusArray = pvPortMalloc ( ArraySize * sizeof ( TaskStatus_t ) ); /* 申请内存 */

    if ( StatusArray != NULL ) { /* 内存申请成功 */
        ArraySize = uxTaskGetSystemState ( ( TaskStatus_t*   ) StatusArray, /* 任务信息存储数组 */
                                           ( UBaseType_t     ) ArraySize,   /* 任务信息存储数组大小 */
                                           ( uint32_t*       ) &TotalRunTime ); /* 保存系统总的运行时间 */
        printf ( "TaskName\t\tPriority\t\tTaskNumber\t\t\r\n" );

        /* 打印获取到的系统任务有关信息，例如任务名称、任务优先级和任务编号 */
        for ( x = 0; x < ArraySize; x++ ) {
            printf ( "%s\t\t%d\t\t\t%d\t\t\t\r\n",
                     StatusArray[x].pcTaskName,
                     ( int ) StatusArray[x].uxCurrentPriority,
                     ( int ) StatusArray[x].xTaskNumber );
        }
    }

    vPortFree ( StatusArray ); /* 释放内存 */
    printf ( "/************************** 结束 ***************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;
    printf ( "/************ 第二步：函数vTaskGetInfo的使用 **************/\r\n" );
    TaskHandle = xTaskGetHandle ( "led0_task" ); /* 根据任务名获取任务句柄 */
    /* 获取led0_task的任务信息 */
    vTaskGetInfo ( ( TaskHandle_t  ) TaskHandle,  /* 任务句柄 */
                   ( TaskStatus_t* ) &TaskStatus, /* 任务信息结构体 */
                   ( BaseType_t    ) pdTRUE,      /* 允许统计任务堆栈历史最小剩余大小 */
                   ( eTaskState    ) eInvalid );  /* 函数自己获取任务运行状态 */
    /* 打印指定任务的有关信息 */
    printf ( "任务名：                %s\r\n", TaskStatus.pcTaskName );
    printf ( "任务编号：              %d\r\n", ( int ) TaskStatus.xTaskNumber );
    printf ( "任务状态：              %d\r\n", TaskStatus.eCurrentState );
    printf ( "任务当前优先级：        %d\r\n", ( int ) TaskStatus.uxCurrentPriority );
    printf ( "任务基优先级：          %d\r\n", ( int ) TaskStatus.uxBasePriority );
    printf ( "任务堆栈基地址：        %#x\r\n", ( int ) TaskStatus.pxStackBase );
    printf ( "任务堆栈历史剩余最小值：%d\r\n", TaskStatus.usStackHighWaterMark );
    printf ( "/************************** 结束 ***************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    eTaskState TaskState;
    char TaskInfo[10];
    printf ( "/*********** 第三步：函数eTaskGetState的使用 *************/\r\n" );
    TaskHandle = xTaskGetHandle ( "query_task" ); /* 根据任务名获取任务句柄 */
    TaskState = eTaskGetState ( TaskHandle ); /* 获取query_task的任务状态 */
    memset ( TaskInfo, 0, 10 );

    switch ( ( int ) TaskState ) {
        case 0:
            sprintf ( TaskInfo, "Running" );
            break;

        case 1:
            sprintf ( TaskInfo, "Ready" );
            break;

        case 2:
            sprintf ( TaskInfo, "Suspend" );
            break;

        case 3:
            sprintf ( TaskInfo, "Delete" );
            break;

        case 4:
            sprintf ( TaskInfo, "Invalid" );
            break;
    }

    printf ( "任务状态值：%d，对应的状态为：%s\r\n", TaskState, TaskInfo );
    printf ( "/************************** 结束 **************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    printf ( "/************* 第三步：函数vTaskList的使用 *************/\r\n" );
    vTaskList ( InfoBuffer ); /* 获取所有任务的信息 */
    printf ( "%s\r\n", InfoBuffer ); /* 通过串口打印所有任务的信息 */
    printf ( "/************************** 结束 **************************/\r\n" );

    while ( 1 ) {
        LED1 = ~LED1;
        vTaskDelay ( 1000 );
    }
}
