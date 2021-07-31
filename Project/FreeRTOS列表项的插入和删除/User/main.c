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
    /* 初始化列表和列表项 */
    vListInitialise ( &TestList );
    vListInitialiseItem ( &ListItem1 );
    vListInitialiseItem ( &ListItem2 );
    vListInitialiseItem ( &ListItem3 );
    ListItem1.xItemValue = 40; /* ListItem1列表项值为40 */
    ListItem2.xItemValue = 60; /* ListItem2列表项值为60 */
    ListItem3.xItemValue = 50; /* ListItem3列表项值为50 */
    /* 打印列表和其他列表项的地址 */
    printf ( "/*******************列表和列表项地址*******************/\r\n" );
    printf ( "项目                              地址                \r\n" );
    printf ( "TestList                          %#x                 \r\n", ( int ) &TestList );
    printf ( "TestList->pxIndex                 %#x                 \r\n", ( int ) TestList.pxIndex );
    printf ( "TestList->xListEnd                %#x                 \r\n", ( int ) ( &TestList.xListEnd ) );
    printf ( "ListItem1                         %#x                 \r\n", ( int ) &ListItem1 );
    printf ( "ListItem2                         %#x                 \r\n", ( int ) &ListItem2 );
    printf ( "ListItem3                         %#x                 \r\n", ( int ) &ListItem3 );
    printf ( "/************************结束**************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    /* 向列表TestList添加列表项 */
    vListInsert ( &TestList, &ListItem1 ); /* 插入列表项ListItem1 */
    printf ( "/******************添加列表项ListItem1*****************/\r\n" );
    printf ( "项目                              地址                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "/*******************前后向连接分割线********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "/************************结束**************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    vListInsert ( &TestList, &ListItem2 ); /* 插入列表项ListItem2 */
    printf ( "/******************添加列表项ListItem2*****************/\r\n" );
    printf ( "项目                              地址				    \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x					\r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x					\r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x					\r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "/*******************前后向连接分割线********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x					\r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x					\r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x					\r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "/************************结束**************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    vListInsert ( &TestList, &ListItem3 ); /* 插入列表项ListItem3 */
    printf ( "/******************添加列表项ListItem3*****************/\r\n" );
    printf ( "项目                              地址                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x                 \r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "/*******************前后向连接分割线********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x                 \r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "/************************结束**************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    /* 删除列表项 */
    uxListRemove ( &ListItem2 ); /* 删除ListItem2 */
    printf ( "/******************删除列表项ListItem2*****************/\r\n" );
    printf ( "项目                              地址                \r\n" );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "/*******************前后向连接分割线********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "/************************结束**************************/\r\n" );
    printf ( "按下KEY_UP键继续!\r\n\r\n\r\n" );

    while ( KEY_Scan ( 0 ) != WKUP_PRES ) { /* 等待KEY_UP键按下 */
        delay_ms ( 10 );
    }

    /* 在末尾添加列表项 */
    TestList.pxIndex = TestList.pxIndex->pxNext; /* pxIndex向后移一项，这样pxIndex就会指向ListItem1 */
    vListInsertEnd ( &TestList, &ListItem2 ); /* 列表末尾添加列表项ListItem2 */
    printf ( "/***************在末尾添加列表项ListItem2***************/\r\n" );
    printf ( "项目                              地址                \r\n" );
    printf ( "TestList->pxIndex                 %#x                 \r\n", ( int ) TestList.pxIndex );
    printf ( "TestList->xListEnd->pxNext        %#x                 \r\n", ( int ) ( TestList.xListEnd.pxNext ) );
    printf ( "ListItem2->pxNext                 %#x                 \r\n", ( int ) ( ListItem2.pxNext ) );
    printf ( "ListItem1->pxNext                 %#x                 \r\n", ( int ) ( ListItem1.pxNext ) );
    printf ( "ListItem3->pxNext                 %#x                 \r\n", ( int ) ( ListItem3.pxNext ) );
    printf ( "/*******************前后向连接分割线********************/\r\n" );
    printf ( "TestList->xListEnd->pxPrevious    %#x                 \r\n", ( int ) ( TestList.xListEnd.pxPrevious ) );
    printf ( "ListItem2->pxPrevious             %#x                 \r\n", ( int ) ( ListItem2.pxPrevious ) );
    printf ( "ListItem1->pxPrevious             %#x                 \r\n", ( int ) ( ListItem1.pxPrevious ) );
    printf ( "ListItem3->pxPrevious             %#x                 \r\n", ( int ) ( ListItem3.pxPrevious ) );
    printf ( "/************************结束**************************/\r\n\r\n\r\n" );

    while ( 1 ) {
        LED1 = !LED1;
        vTaskDelay ( 1000 );
    }
}
