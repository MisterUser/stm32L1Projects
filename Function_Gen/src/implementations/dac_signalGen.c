#include "dac_signalGen.h"

const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

const uint8_t Sine8Bit[32] = {
	0,1,7,18,34,54,77,101,127,153,177,200,220,236,247,254,
	255,251,242,228,210,189,165,140,114,87,65,44,26,12,3,1
};

const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};


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
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  /* TIM6 Configuration ------------------------------------------------------*/
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 0xFF;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

  /* TIM6 enable counter */
  TIM_Cmd(TIM6, ENABLE);

  /* TIM7 Configuration ------------------------------------------------------*/
  /* TIM7 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 0xFF;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  /* TIM7 TRGO selection */
  TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);

  /* TIM7 enable counter */
  TIM_Cmd(TIM7, ENABLE);

}

void DAC_signalGen_init(void)
{
   DAC_Config();
   DAC_TIM_Config();

   DAC_DeInit();
   
   
    
   //---------------DAC channel1 Configuration------------------//
   DAC_InitTypeDef            DAC_InitStructure;
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;//TSEL1&TEN1
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //0x0
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

   DAC_Init(DAC_Channel_1, &DAC_InitStructure);

   //DMA-Channel 1
   DMA_DeInit(DMA1_Channel1);

   DMA_InitTypeDef            DMA_InitStructure;
   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8RDual_1_Address;
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
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);

   DMA_Cmd(DMA1_Channel1, ENABLE);


   /* Enable DAC Channel1 */
   DAC_Cmd(DAC_Channel_1, ENABLE);

   /* Enable DMA for DAC Channel1 */
   DAC_DMACmd(DAC_Channel_1, ENABLE);



   //---------------DAC channel2 Configuration------------------//
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

   DAC_Init(DAC_Channel_2, &DAC_InitStructure);

   //DMA-Channel 2
   DMA_DeInit(DMA1_Channel2);

   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8RDual_2_Address;
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
   DMA_Init(DMA1_Channel2, &DMA_InitStructure);

   DMA_Cmd(DMA1_Channel2, ENABLE);


   /* Enable DAC Channel2 */
   DAC_Cmd(DAC_Channel_2, ENABLE);

   /* Enable DMA for DAC Channel2 */
   DAC_DMACmd(DAC_Channel_2, ENABLE);
}
