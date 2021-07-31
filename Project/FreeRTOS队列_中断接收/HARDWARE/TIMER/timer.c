#include "timer.h"
#include "usart.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void TIM2_Int_Init ( u16 arr, u16 psc ) {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd ( RCC_APB1Periph_TIM2, ENABLE );
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit ( TIM2, &TIM_TimeBaseStructure );
    TIM_ITConfig ( TIM2, TIM_IT_Update, ENABLE );
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init ( &NVIC_InitStructure );
    TIM_Cmd ( TIM2, ENABLE );
}

extern QueueHandle_t Message_Queue;

void TIM2_IRQHandler ( void ) {
    u8 buffer[USART_REC_LEN] = {0};
    BaseType_t xTaskWokenByReceive = pdFALSE;
    BaseType_t err;

    if ( TIM_GetITStatus ( TIM2, TIM_IT_Update ) == SET ) {
        if ( Message_Queue != NULL ) {
            err = xQueueReceiveFromISR ( Message_Queue, buffer, &xTaskWokenByReceive );

            if ( err == pdTRUE ) {
                printf ( "I receive %s\r\n", buffer );
            }
        }

        portYIELD_FROM_ISR ( xTaskWokenByReceive );
    }

    TIM_ClearITPendingBit ( TIM2, TIM_IT_Update );
}
