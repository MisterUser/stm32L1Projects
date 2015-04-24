#ifndef stop_MODE_H
#define stop_MODE_H

#include <stm32l1xx_syscfg.h>
#include <stm32l1xx_exti.h>
#include <misc.h>
#include <stm32l1xx_pwr.h>

extern uint8_t sleepENABLED;

void stopMODE_init();
void EXTI_config();


#endif
