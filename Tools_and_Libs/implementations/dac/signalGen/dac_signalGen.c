#include "dac_signalGen.h"

const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

const uint8_t Sine8Bit[32] = {
	0,1,7,18,34,54,77,101,127,153,177,200,220,236,247,254,
	255,251,242,228,210,189,165,140,114,87,65,44,26,12,3,1
};

void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DMA1 clock enable (to be used with DAC) */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_TIM_Config(void)
{
   /* TIM2 Configuration ------------------------------------------------------*/
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 0xFF;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

void DAC_signalGen_init(void)
{
   DAC_Config();
   DAC_TIM_Config();

   
   /* The sine wave and the escalator wave has been selected */
   /* Sine Wave generator ---------------------------------------------*/
   DAC_DeInit();
   
   /* DAC channel1 and channel2 Configuration */
   DAC_InitTypeDef            DAC_InitStructure;
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

   DMA_DeInit(DMA1_Channel3);

   DMA_InitTypeDef            DMA_InitStructure;
   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R2_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine8Bit;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = 32;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel3, &DMA_InitStructure);

   /* Enable DMA1 Channel3 */
   DMA_Cmd(DMA1_Channel3, ENABLE);

     /* DAC Channel2 Init */
   DAC_Init(DAC_Channel_2, &DAC_InitStructure);

   /* Enable DAC Channel2 */
   DAC_Cmd(DAC_Channel_2, ENABLE);

   /* Enable DMA for DAC Channel2 */
   DAC_DMACmd(DAC_Channel_2, ENABLE);
}
