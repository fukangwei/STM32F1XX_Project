#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

void EXTIX_Init ( void ) {
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );
    GPIO_EXTILineConfig ( GPIO_PortSourceGPIOC, GPIO_PinSource5 );
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init ( &EXTI_InitStructure );
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
}

extern TaskHandle_t Task2Task_Handler;

void EXTI9_5_IRQHandler ( void ) {
    BaseType_t YieldRequired;
    delay_xms ( 20 );

    if ( KEY0 == 0 ) {
        YieldRequired = xTaskResumeFromISR ( Task2Task_Handler );
        printf ( "恢复任务2的运行!\r\n" );

        if ( YieldRequired == pdTRUE ) {
            /*如果xTaskResumeFromISR返回值为pdTRUE，那么说明要恢复的这个
              任务的任务优先级等于或者高于正在运行的任务，所以在
              退出中断的时候一定要进行上下文切换！*/
            portYIELD_FROM_ISR ( YieldRequired );
        }
    }

    EXTI_ClearITPendingBit ( EXTI_Line5 );
}
