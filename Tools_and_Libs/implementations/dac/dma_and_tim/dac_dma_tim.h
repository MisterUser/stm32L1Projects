#ifndef DAC_DMA_TIM_H
#define DAC_DMA_TIM_H

#include <stm32l1xx_dac.h>
#include <stm32l1xx_dma.h>
#include <stm32l1xx_tim.h>

#define DAC_DHR12R2_Address      0x40007414
#define DAC_DHR8R1_Address       0x40007410
#define DAC_DHR8R2_Address       0x4000741C
#define BUFFERSIZE 1000
extern uint8_t readBuf1[BUFFERSIZE];
extern uint8_t readBuf2[BUFFERSIZE];

/* Private function prototypes -----------------------------------------------*/
void DAC_Config(void);
void DAC_TIM_Config(void);
void DAC_DMA_TIM_init(void);

#endif
