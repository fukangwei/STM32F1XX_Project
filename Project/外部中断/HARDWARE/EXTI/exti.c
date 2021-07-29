#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"

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
    GPIO_EXTILineConfig ( GPIO_PortSourceGPIOA, GPIO_PinSource15 );
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init ( &EXTI_InitStructure );
    GPIO_EXTILineConfig ( GPIO_PortSourceGPIOA, GPIO_PinSource0 );
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init ( &EXTI_InitStructure );
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
}


void EXTI0_IRQHandler ( void ) {
    delay_ms ( 10 );

    if ( WK_UP == 1 ) {
        LED0 = !LED0;
        LED1 = !LED1;
    }

    EXTI_ClearITPendingBit ( EXTI_Line0 );
}

void EXTI9_5_IRQHandler ( void ) {
    delay_ms ( 10 );

    if ( KEY0 == 0 ) {
        LED0 = !LED0;
    }

    EXTI_ClearITPendingBit ( EXTI_Line5 );
}

void EXTI15_10_IRQHandler ( void ) {
    delay_ms ( 10 );

    if ( KEY1 == 0 )   {
        LED1 = !LED1;
    }

    EXTI_ClearITPendingBit ( EXTI_Line15 );
}
