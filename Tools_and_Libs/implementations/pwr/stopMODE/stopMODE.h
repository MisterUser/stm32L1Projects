#ifndef stop_MODE_H
#define stop_MODE_H

#include <stm32l1xx_syscfg.h>
#include <stm32l1xx_exti.h>
#include <misc.h>
#include <stm32l1xx_pwr.h>

extern __IO uint8_t sleepENABLED;

//Push Button for interrupt is PinA0
// IF NOT PIN 0!!! -> must change IRQHandler name in stopMODE.c

#define intPIN      	GPIO_Pin_0
#define intPIN_CLK 	RCC_AHBPeriph_GPIOA
#define intBank		GPIOA
#define intEXTI_SRC	EXTI_PortSourceGPIOA
#define intEXTI_PIN	EXTI_PinSource0
#define intEXTI_LINE    EXTI_Line0	
#define intIRQn		EXTI0_IRQn

//Pin for LED
#define ledPIN          GPIO_Pin_7
#define ledPIN_CLK      RCC_AHBPeriph_GPIOB
#define ledBank         GPIOB

/*
 * Initializes above-defined Pin (button) as EXTI
 * and other pin for LED
 * EXTI#_IRQHandler toggles extern sleepENABLED
 */

void stopMODE_init();
void stop_prepare_and_enter();
void stop_exit();

#endif
