#ifndef EXTERN_DAC_H 
#define EXTERN_DAC_H

#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_tim.h>
#include <misc.h>
#include "dac_signalGen.h"

#define ExtDAC_setPin    GPIOC->BSRRL
#define ExtDAC_resetPin  GPIOC->BSRRH

#define ExtDAC1_PORT 	 GPIOC
#define ExtDAC1_CLK  	 RCC_AHBPeriph_GPIOC
#define ExtDAC1_Pin1_LSB GPIO_Pin_0
#define ExtDAC1_Pin2	 GPIO_Pin_1
#define ExtDAC1_Pin3	 GPIO_Pin_2
#define ExtDAC1_Pin4_MSB GPIO_Pin_3
#define ExtDAC1_PortOffset 0

#define ExtDAC2_PORT     GPIOC
#define ExtDAC2_CLK      RCC_AHBPeriph_GPIOC
#define ExtDAC2_Pin1_LSB GPIO_Pin_6
#define ExtDAC2_Pin2     GPIO_Pin_7
#define ExtDAC2_Pin3     GPIO_Pin_8
#define ExtDAC2_Pin4_MSB GPIO_Pin_9
#define ExtDAC2_PortOffset 6

void external_DAC_setup(void);

#endif
