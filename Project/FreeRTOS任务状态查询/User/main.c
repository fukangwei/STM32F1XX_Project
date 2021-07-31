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
    printf ( "/******** ��һ��������uxTaskGetSystemState��ʹ�� **********/\r\n" );
    ArraySize = uxTaskGetNumberOfTasks(); /* ��ȡϵͳ�������� */
    StatusArray = pvPortMalloc ( ArraySize * sizeof ( TaskStatus_t ) ); /* �����ڴ� */

    if ( StatusArray != NULL ) { /* �ڴ�����ɹ� */
        ArraySize = uxTaskGetSystemState ( ( TaskStatus_t*   ) StatusArray, /* ������Ϣ�洢���� */
                                           ( UBaseType_t     ) ArraySize,   /* ������Ϣ�洢�����С */
                                           ( uint32_t*       ) &TotalRunTime ); /* ����ϵͳ�ܵ�����ʱ�� */
        printf ( "TaskName\t\tPriority\t\tTaskNumber\t\t\r\n" );

        /* ��ӡ��ȡ����ϵͳ�����й���Ϣ�������������ơ��������ȼ��������� */
        for ( x = 0; x < ArraySize; x++ ) {
            printf ( "%s\t\t%d\t\t\t%d\t\t\t\r\n",
                     StatusArray[x].pcTaskName,
                     ( int ) StatusArray[x].uxCurrentPriority,
                     ( int ) StatusArray[x].xTaskNumber );
        }
    }

    vPortFree ( StatusArray ); /* �ͷ��ڴ� */
    printf ( "/************************** ���� ***************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;
    printf ( "/************ �ڶ���������vTaskGetInfo��ʹ�� **************/\r\n" );
    TaskHandle = xTaskGetHandle ( "led0_task" ); /* ������������ȡ������ */
    /* ��ȡled0_task��������Ϣ */
    vTaskGetInfo ( ( TaskHandle_t  ) TaskHandle,  /* ������ */
                   ( TaskStatus_t* ) &TaskStatus, /* ������Ϣ�ṹ�� */
                   ( BaseType_t    ) pdTRUE,      /* ����ͳ�������ջ��ʷ��Сʣ���С */
                   ( eTaskState    ) eInvalid );  /* �����Լ���ȡ��������״̬ */
    /* ��ӡָ��������й���Ϣ */
    printf ( "��������                %s\r\n", TaskStatus.pcTaskName );
    printf ( "�����ţ�              %d\r\n", ( int ) TaskStatus.xTaskNumber );
    printf ( "����״̬��              %d\r\n", TaskStatus.eCurrentState );
    printf ( "����ǰ���ȼ���        %d\r\n", ( int ) TaskStatus.uxCurrentPriority );
    printf ( "��������ȼ���          %d\r\n", ( int ) TaskStatus.uxBasePriority );
    printf ( "�����ջ����ַ��        %#x\r\n", ( int ) TaskStatus.pxStackBase );
    printf ( "�����ջ��ʷʣ����Сֵ��%d\r\n", TaskStatus.usStackHighWaterMark );
    printf ( "/************************** ���� ***************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    eTaskState TaskState;
    char TaskInfo[10];
    printf ( "/*********** ������������eTaskGetState��ʹ�� *************/\r\n" );
    TaskHandle = xTaskGetHandle ( "query_task" ); /* ������������ȡ������ */
    TaskState = eTaskGetState ( TaskHandle ); /* ��ȡquery_task������״̬ */
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

    printf ( "����״ֵ̬��%d����Ӧ��״̬Ϊ��%s\r\n", TaskState, TaskInfo );
    printf ( "/************************** ���� **************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    printf ( "/************* ������������vTaskList��ʹ�� *************/\r\n" );
    vTaskList ( InfoBuffer ); /* ��ȡ�����������Ϣ */
    printf ( "%s\r\n", InfoBuffer ); /* ͨ�����ڴ�ӡ�����������Ϣ */
    printf ( "/************************** ���� **************************/\r\n" );

    while ( 1 ) {
        LED1 = ~LED1;
        vTaskDelay ( 1000 );
    }
}
