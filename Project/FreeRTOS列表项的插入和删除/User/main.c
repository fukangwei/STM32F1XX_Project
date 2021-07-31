#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO  1
#define START_STK_SIZE   128
TaskHandle_t StartTask_Handler;
void start_task ( void *pvParameters );

#define TASK1_TASK_PRIO  2
#define TASK1_STK_SIZE   128
TaskHandle_t Task1Task_Handler;
void task1_task ( void *pvParameters );

#define LIST_TASK_PRIO  3
#define LIST_STK_SIZE   128
TaskHandle_t ListTask_Handler;
void list_task ( void *pvParameters );

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
    xTaskCreate ( ( TaskFunction_t ) task1_task,
                  ( const char*    ) "task1_task",
                  ( uint16_t       ) TASK1_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) TASK1_TASK_PRIO,
                  ( TaskHandle_t*  ) &Task1Task_Handler );
    xTaskCreate ( ( TaskFunction_t ) list_task,
                  ( const char*    ) "list_task",
                  ( uint16_t       ) LIST_STK_SIZE,
                  ( void*          ) NULL,
                  ( UBaseType_t    ) LIST_TASK_PRIO,
                  ( TaskHandle_t*  ) &ListTask_Handler );
    vTaskDelete ( StartTask_Handler );
    taskEXIT_CRITICAL();
}

void task1_task ( void *pvParameters ) {
    while ( 1 ) {
        LED0 = !LED0;
        vTaskDelay ( 500 );
    }
}

List_t TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;

void list_task ( void *pvParameters ) {
    delay_ms ( 5000 );
    /* ��ʼ���б���б��� */
    vListInitialise ( &TestList );
    vListInitialiseItem ( &ListItem1 );
    vListInitialiseItem ( &ListItem2 );
    vListInitialiseItem ( &ListItem3 );
    ListItem1.xItemValue = 40; /* ListItem1�б���ֵΪ40 */
    ListItem2.xItemValue = 60; /* ListItem2�б���ֵΪ60 */
    ListItem3.xItemValue = 50; /* ListItem3�б���ֵΪ50 */
    /* ��ӡ�б�������б���ĵ�ַ */
    printf ( "/*******************�б���б����ַ*******************/\r\n" );
    printf ( "��Ŀ                              ��ַ                \r\n" );
    printf ( "TestList                          %#x                 \r\n", ( int ) &TestList );
    printf ( "TestList->pxIndex                 %#x                 \r\n", ( int ) TestList.pxIndex );
    printf ( "TestList->xListEnd                %#x                 \r\n", ( int ) ( &TestList.xListEnd ) );
    printf ( "ListItem1                         %#x                 \r\n", ( int ) &ListItem1 );
    printf ( "ListItem2                         %#x                 \r\n", ( int ) &ListItem2 );
    printf ( "ListItem3                         %#x                 \r\n", ( int ) &ListItem3 );
    printf ( "/************************����**************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    /* ���б�TestList����б��� */
    vListInsert ( &TestList, &ListItem1 ); /* �����б���ListItem1 */
    printf ( "/******************����б���ListItem1*****************/\r\n" );
    printf ( "��Ŀ                              ��ַ                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "/*******************ǰ�������ӷָ���********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "/************************����**************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    vListInsert ( &TestList, &ListItem2 ); /* �����б���ListItem2 */
    printf ( "/******************����б���ListItem2*****************/\r\n" );
    printf ( "��Ŀ                              ��ַ				    \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x					\r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x					\r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x					\r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "/*******************ǰ�������ӷָ���********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x					\r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x					\r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x					\r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "/************************����**************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    vListInsert ( &TestList, &ListItem3 ); /* �����б���ListItem3 */
    printf ( "/******************����б���ListItem3*****************/\r\n" );
    printf ( "��Ŀ                              ��ַ                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x                 \r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "/*******************ǰ�������ӷָ���********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x                 \r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "/************************����**************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    /* ɾ���б��� */
    uxListRemove ( &ListItem2 ); /* ɾ��ListItem2 */
    printf ( "/******************ɾ���б���ListItem2*****************/\r\n" );
    printf ( "��Ŀ                              ��ַ                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "/*******************ǰ�������ӷָ���********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "/************************����**************************/\r\n" );
    printf ( "����KEY_UP������!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* �ȴ�KEY_UP������ */
        delay_ms ( 10 );
    }

    /* ��ĩβ����б��� */
    TestList.pxIndex = TestList.pxIndex->pxNext; /* pxIndex�����һ�����pxIndex�ͻ�ָ��ListItem1 */
    vListInsertEnd ( &TestList, &ListItem2 ); /* �б�ĩβ����б���ListItem2 */
    printf ( "/***************��ĩβ����б���ListItem2***************/\r\n" );
    printf ( "��Ŀ                              ��ַ                \r\n" );
    printf ( "TestList->pxIndex                 %#x                 \r\n", ( int ) TestList.pxIndex );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x                 \r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "/*******************ǰ�������ӷָ���********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x                 \r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "/************************����**************************/\r\n\r\n\r\n" );

    while ( 1 ) {
        LED1 = !LED1;
        vTaskDelay ( 1000 );
    }
}
