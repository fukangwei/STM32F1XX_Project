#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "sys.h"
#include "usart.h"

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

#define vAssertCalled(char, int) printf( "Error: %s, %d\r\n", char, int )
#define configASSERT(x)          if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

/***************************************************************************************************************/
/*                                        FreeRTOS基础配置选项                                                  */
/***************************************************************************************************************/
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configUSE_TICKLESS_IDLE                 0
#define configUSE_QUEUE_SETS                    1
#define configCPU_CLOCK_HZ                      ( SystemCoreClock )
#define configTICK_RATE_HZ                      ( 1000 )
#define configMAX_PRIORITIES                    ( 32 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 130 )
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configCHECK_FOR_STACK_OVERFLOW          0
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           1

/**************************************************************************************************************/
/*                                FreeRTOS与内存申请有关配置选项                                                */
/**************************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 20 * 1024 ) )

/*************************************************************************************************************/
/*                                FreeRTOS与钩子函数有关的配置选项                                              */
/*************************************************************************************************************/
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0

/*************************************************************************************************************/
/*                                FreeRTOS与运行时间和任务状态收集有关的配置选项                                 */
/*************************************************************************************************************/
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1

/*************************************************************************************************************/
/*                                FreeRTOS与软件定时器有关的配置选项                                            */
/*************************************************************************************************************/
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )

/**************************************************************************************************************/
/*                                FreeRTOS可选函数配置选项                                                     */
/**************************************************************************************************************/
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define	INCLUDE_xTaskGetHandle                  1

/**************************************************************************************************************/
/*                                FreeRTOS与中断有关的配置选项                                                  */
/**************************************************************************************************************/
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS  __NVIC_PRIO_BITS
#else
#define configPRIO_BITS  4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5
#define configKERNEL_INTERRUPT_PRIORITY               ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY          ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/*************************************************************************************************************/
/*                                FreeRTOS与中断服务函数有关的配置选项                                          */
/*************************************************************************************************************/
#define xPortPendSVHandler  PendSV_Handler
#define vPortSVCHandler     SVC_Handler

#endif /* FREERTOS_CONFIG_H */
