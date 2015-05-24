#ifndef ADC_DMA_CONT_H
#define ADC_DMA_CONT_H

#include <stm32l1xx_adc.h>
#include <stm32l1xx_dma.h>

#define ADC1_DR_ADDRESS    ((uint32_t)0x40012458)
void adc_dma_cont_init();

#endif
