#ifndef ADC_W_DMA_H
#define ADC_W_DMA_H

#include <stm32l1xx_adc.h>
#include <stm32l1xx_dma.h>

#define ADC1_DR_ADDRESS    ((uint32_t)0x40012458)
void adc_w_dma_init();

#endif
