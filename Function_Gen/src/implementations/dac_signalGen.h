#ifndef DAC_SIGNALGEN_H
#define DAC_SIGNALGEN_H

#include <stm32l1xx_dac.h>
#include <stm32l1xx_dma.h>
#include <stm32l1xx_tim.h>

#define DAC_DHR12R2_Address      0x40007414
#define DAC_DHR8R1_Address       0x40007410
#define DAC_DHR8R2_Address       0x4000741C
#define DAC_DHR8RDual_1_Address  0x40007428
#define DAC_DHR8RDual_2_Address  0x40007429

/* Private function prototypes -----------------------------------------------*/
void DAC_Config(void);
void DAC_TIM_Config(void);
void DAC_signalGen_init(void);
void function_on(int channel);
void function_off(int channel);
/*
 *set_internal_DAC_freq
 *  freq must be between 4Hz and 250kHz
 */
void set_internal_DAC_freq(char channel,uint16_t freq,char hz_or_khz);

#endif
