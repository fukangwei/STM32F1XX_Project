#include "timerx.h"
#include "led.h"
#include "led.h"
#include "usart.h"

void TIM3_Int_Init ( u16 arr, u16 psc ) {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_TIM3, ENABLE );
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit ( TIM3, &TIM_TimeBaseStructure );
    TIM_ITConfig ( TIM3, TIM_IT_Update, ENABLE );
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
    TIM_Cmd ( TIM3, ENABLE );
}

void TIM3_IRQHandler ( void ) {
    if ( TIM_GetITStatus ( TIM3, TIM_IT_Update ) == SET ) {
        LED1 = !LED1;
    }

    TIM_ClearITPendingBit ( TIM3, TIM_IT_Update );
}
