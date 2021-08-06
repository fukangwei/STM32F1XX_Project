#include "led.h"
#include "delay.h"
#include "sys.h"
#include "pwm.h"

int main ( void ) {
    u16 led0_pwm_val = 0;
    u8 dir = 1;
    delay_init ( 72 );
    LED_Init();
    TIM1_PWM_Init ( 899, 0 ); /* ²»·ÖÆµ£¬PWMÆµÂÊ = 72000 / (899 + 1) = 80Khz */

    while ( 1 ) {
        delay_ms ( 10 );

        if ( dir ) {
            led0_pwm_val++;
        } else {
            led0_pwm_val--;
        }

        if ( led0_pwm_val > 300 ) {
            dir = 0;
        }

        if ( led0_pwm_val == 0 ) {
            dir = 1;
        }

        TIM_SetCompare1 ( TIM1, led0_pwm_val );
    }
}
