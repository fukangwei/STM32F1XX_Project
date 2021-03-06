#include "pwm.h"
#include "led.h"

/* PWM输出初始化，arr是自动重装值，psc是时钟预分频数 */
void TIM1_PWM_Init ( u16 arr, u16 psc ) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_TIM1, ENABLE );
    RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );
    /* 设置该引脚为复用输出功能，输出TIM1 CH1的PWM脉冲波形 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 复用推挽输出 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOA, &GPIO_InitStructure );
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit ( TIM1, &TIM_TimeBaseStructure );
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init ( TIM1, &TIM_OCInitStructure );
    TIM_CtrlPWMOutputs ( TIM1, ENABLE );
    TIM_OC1PreloadConfig ( TIM1, TIM_OCPreload_Enable );
    TIM_ARRPreloadConfig ( TIM1, ENABLE );
    TIM_Cmd ( TIM1, ENABLE );
}
