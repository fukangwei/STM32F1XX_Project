#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern volatile unsigned long long FreeRTOSRunTimeTicks;

void ConfigureTimeForRunTimeStats ( void );
void TIM3_Int_Init ( u16 arr, u16 psc );
#endif
