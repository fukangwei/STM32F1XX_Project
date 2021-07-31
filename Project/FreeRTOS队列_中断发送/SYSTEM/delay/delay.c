#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

static u8  fac_us = 0;                          //us延时倍乘数
static u16 fac_ms = 0;                          //ms延时倍乘数,在ucos下,代表每个节拍的ms数

extern void xPortSysTickHandler ( void );

void SysTick_Handler ( void ) {
    if ( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) { //系统已经运行
        xPortSysTickHandler();
    }
}

void delay_init() {
    u32 reload;
    SysTick_CLKSourceConfig ( SysTick_CLKSource_HCLK );
    fac_us = SystemCoreClock / 1000000;
    reload = SystemCoreClock / 1000000;
    reload *= 1000000 / configTICK_RATE_HZ;
    fac_ms = 1000 / configTICK_RATE_HZ;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD = reload;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay_us ( u32 nus ) {
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;
    ticks = nus * fac_us;
    told = SysTick->VAL;

    while ( 1 ) {
        tnow = SysTick->VAL;

        if ( tnow != told ) {
            if ( tnow < told ) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if ( tcnt >= ticks ) {
                break;
            }
        }
    };
}

void delay_ms ( u32 nms ) {
    if ( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED ) {
        if ( nms >= fac_ms ) {
            vTaskDelay ( nms / fac_ms );
        }

        nms %= fac_ms;
    }

    delay_us ( ( u32 ) ( nms * 1000 ) );
}

void delay_xms ( u32 nms ) {
    u32 i;

    for ( i = 0; i < nms; i++ ) {
        delay_us ( 1000 );
    }
}







































