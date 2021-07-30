#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;

static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static StaticTask_t TimerTaskTCB;

#define START_TASK_PRIO  1
#define START_STK_SIZE   128
StackType_t StartTaskStack[START_STK_SIZE];
StaticTask_t StartTaskTCB;
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO  2
#define TASK1_STK_SIZE   128
StackType_t Task1TaskStack[TASK1_STK_SIZE];
StaticTask_t Task1TaskTCB;
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define TASK2_TASK_PRIO  3
#define TASK2_STK_SIZE   128
StackType_t Task2TaskStack[TASK2_STK_SIZE];
StaticTask_t Task2TaskTCB;
TaskHandle_t Task2Task_Handler;
void task2_task ( void *pvParameters );

void vApplicationGetIdleTaskMemory ( StaticTask_t **ppxIdleTaskTCBBuffer,
                                     StackType_t **ppxIdleTaskStackBuffer,
                                     uint32_t *pulIdleTaskStackSize ) {
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory ( StaticTask_t **ppxTimerTaskTCBBuffer,
                                      StackType_t **ppxTimerTaskStackBuffer,
                                      uint32_t *pulTimerTaskStackSize ) {
    *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

int main ( void ) {
    NVIC_PriorityGroupConfig ( NVIC_PriorityGroup_4 );
    delay_init();
    uart_init ( 9600 );
    LED_Init();
    StartTask_Handler = xTaskCreateStatic ( ( TaskFunction_t ) start_task,
                                            ( const char*    ) "start_task",
                                            ( uint32_t       ) START_STK_SIZE,
                                            ( void*          ) NULL,
                                            ( UBaseType_t    ) START_TASK_PRIO,
                                            ( StackType_t*   ) StartTaskStack,
                                            ( StaticTask_t*  ) &StartTaskTCB );
    vTaskStartScheduler();
}


void start_task ( void *pvParameters ) {
    taskENTER_CRITICAL();
    Task1Task_Handler = xTaskCreateStatic ( ( TaskFunction_t ) task1_task,
                                            ( const char*    ) "task1_task",
                                            ( uint32_t       ) TASK1_STK_SIZE,
                                            ( void*          ) NULL,
                                            ( UBaseType_t    ) TASK1_TASK_PRIO,
                                            ( StackType_t*   ) Task1TaskStack,
                                            ( StaticTask_t*  ) &Task1TaskTCB );
    Task2Task_Handler = xTaskCreateStatic ( ( TaskFunction_t ) task2_task,
                                            ( const char*    ) "task2_task",
                                            ( uint32_t       ) TASK2_STK_SIZE,
                                            ( void*          ) NULL,
                                            ( UBaseType_t    ) TASK2_TASK_PRIO,
                                            ( StackType_t*   ) Task2TaskStack,
                                            ( StaticTask_t*  ) &Task2TaskTCB );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    u16 task1_num = 0;

    while ( 1 ) {
        task1_num++; /* 任务1执行次数加1 */
        LED0 = !LED0;
        printf ( "任务1已经执行：%d次\r\n", task1_num );

        if ( task1_num == 5 ) {
            vTaskDelete ( Task2Task_Handler ); /* 任务1执行5次后，删除任务2 */
            printf ( "任务1删除了任务2!\r\n" );
        }

        vTaskDelay ( 1000 );
    }
}

void task2_task ( void *pvParameters ) {
    u16 task2_num = 0;

    while ( 1 ) {
        task2_num++; /* 任务2执行次数加1 */
        LED1 = !LED1;
        printf ( "任务2已经执行：%d次\r\n", task2_num );
        vTaskDelay ( 1000 );
    }
}

