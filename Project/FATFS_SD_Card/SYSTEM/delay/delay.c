#include "delay.h"
#include "sys.h"

static u8  fac_us = 0;
static u16 fac_ms = 0;

#ifdef OS_CRITICAL_METHOD /* ���OS_CRITICAL_METHOD�����ˣ�˵��ʹ��ucosII */

void SysTick_Handler ( void ) { /* systick�жϷ�������ucos��Ҫ�ú��� */
    OSIntEnter(); /* �����ж� */
    OSTimeTick(); /* ����ucos��ʱ�ӷ������ */
    OSIntExit(); /* ���������л����ж� */
}

#endif

void delay_init ( u8 SYSCLK ) {
#ifdef OS_CRITICAL_METHOD
    u32 reload;
#endif
    SysTick->CTRL &= ~ ( 1 << 2 );
    fac_us = SYSCLK / 8;
#ifdef OS_TICKS_PER_SEC
    reload = SYSCLK / 8; /* ÿ���ӵļ�����������λΪK */
    reload *= 1000000 / OS_TICKS_PER_SEC; /* ����OS_TICKS_PER_SEC�趨���ʱ�� */
    /* reloadΪ24λ�Ĵ��������ֵΪ16777216����72M��Լ��1.86s���� */
    fac_ms = 1000 / OS_TICKS_PER_SEC; /* ����ucos������ʱ�����ٵ�λ */
    SysTick->CTRL |= 1 << 1; /* ����SYSTICK�ж� */
    SysTick->LOAD = reload; /* ÿ1/OS_TICKS_PER_SEC���ж�һ�� */
    SysTick->CTRL |= 1 << 0; /* ����SYSTICK */
#else
    fac_ms = ( u16 ) fac_us * 1000; /* ��ucos�£�����ÿ��ms��Ҫ��systickʱ���� */
#endif
}

#ifdef OS_TICKS_PER_SEC

void delay_us ( u32 nus ) {
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;
    ticks = nus * fac_us;
    tcnt = 0;
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

void delay_ms ( u16 nms ) {
    if ( OSRunning == TRUE ) { /* ���os�Ѿ������� */
        if ( nms >= fac_ms ) { /* ��ʱ��ʱ�����ucos������ʱ������ */
            OSTimeDly ( nms / fac_ms ); /* ucos��ʱ */
        }

        nms %= fac_ms; /* ucos�Ѿ��޷��ṩ��ôС����ʱ�ˣ�������ͨ��ʽ��ʱ */
    }

    delay_us ( ( u32 ) ( nms * 1000 ) ); /* ��ͨ��ʽ��ʱ����ʱucos�޷��������� */
}
#else

void delay_us ( u32 nus ) {
    u32 temp;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;

    do {
        temp = SysTick->CTRL;
    } while ( temp & 0x01 && ! ( temp & ( 1 << 16 ) ) );

    SysTick->CTRL = 0x00;
    SysTick->VAL = 0X00;
}

void delay_ms ( u16 nms ) {
    u32 temp;
    SysTick->LOAD = ( u32 ) nms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;

    do {
        temp = SysTick->CTRL;
    } while ( temp & 0x01 && ! ( temp & ( 1 << 16 ) ) );

    SysTick->CTRL = 0x00;
    SysTick->VAL = 0X00;
}

#endif

void JTAG_Set ( u8 mode ) {
    u32 temp;
    temp = mode;
    temp <<= 25;
    RCC->APB2ENR |= 1 << 0; /* ��������ʱ�� */
    AFIO->MAPR &= 0XF8FFFFFF; /* ���MAPR��[26:24] */
    AFIO->MAPR |= temp; /* ����jtagģʽ */
}