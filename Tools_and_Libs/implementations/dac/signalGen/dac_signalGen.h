#ifndef DAC_SIGNALGEN_H
#define DAC_SIGNALGEN_H

#include <stm32l1xx_dac.h>
#include <stm32l1xx_dma.h>
#include <stm32l1xx_tim.h>

#define DAC_DHR12R2_Address      0x40007414
#define DAC_DHR8R1_Address       0x40007410
#define DAC_DHR8R2_Address       0x4000741C

/* Private function prototypes -----------------------------------------------*/
void DAC_Config(void);
void DAC_TIM_Config(void);
void DAC_signalGen_init(void);

#endif
