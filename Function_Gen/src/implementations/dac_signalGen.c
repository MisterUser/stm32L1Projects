#include "dac_signalGen.h"

/*const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
*/

const uint8_t Sine8Bit[32] = {
	0,1,7,18,34,54,77,101,127,153,177,200,220,236,247,254,
	255,251,242,228,210,189,165,140,114,87,65,44,26,12,3,1
};

const uint8_t Escalator8bit[8] = {
	0x00,//0x08,0x10,0x18,
	0x20,//0x28,0x30,0x38,
        0x40,//0x48,0x50,0x58,
	0x60,//0x68,0x70,0x78,
	0x80,//0x88,0x90,0x98,
	0xA0,//0xA8,0xB0,0xB8,
	0xC0,//0xC8,0xD0,0xD8,
	0xE0//,0xE8,0xF0,0xF8
};

const uint8_t Rvrs_Escalator8bit[16] = {
        0xF8,//0xF0,
	0xE8,//0xE0,
	0xD8,//0xD0,
	0xC8,//0xC0,
        0xB8,//0xB0,
	0xA8,//0xA0,
	0x98,//0x90,
	0x88,//0x80,
        0x78,//0x70,
	0x68,//0x60,
	0x58,//0x50,
	0x48,//0x40,
        0x38,//0x30,
	0x28,//0x20,
        0x18,//0x10,
	0x08//0x00
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
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  /* TIM6 Configuration ------------------------------------------------------*/
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  /* Time base configuration */
  //call TIM_Cmd(0x40001000,0)
  //set ((TIM_TypeDef*)0x40001000)->ARR = 0xFF

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  //0x0380 is decent sine @16MHz (HSI) ->561Hz
  //0x0080 is decent @2MHz (MSI)
  TIM_TimeBaseStructure.TIM_Period = 0x0380;//80 is fastest for sine without sign. distortion ->@16MHz, 500Hz Sine
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
  TIM_TimeBaseStructure.TIM_Period = 0x0FFF;//0xFF; //0xFF = 8.2k (without Presclr & CLKDiv)
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
   DMA_DeInit(DMA1_Channel1);  
   DMA_DeInit(DMA1_Channel2);  
   DMA_DeInit(DMA1_Channel3);  
   DMA_DeInit(DMA1_Channel4);  
   DMA_Cmd(DMA1_Channel1, DISABLE); 
   DMA_Cmd(DMA1_Channel2, DISABLE); 
   DMA_Cmd(DMA1_Channel3, DISABLE); 
   DMA_Cmd(DMA1_Channel4, DISABLE); 
    
   //---------------DAC channel1 Configuration------------------//
   DAC_InitTypeDef            DAC_InitStructure;
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;//TSEL1&TEN1
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //0x0
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

   DAC_Init(DAC_Channel_1, &DAC_InitStructure);

   //DMA-Channel 2
   DMA_DeInit(DMA1_Channel2);

   DMA_InitTypeDef            DMA_InitStructure;
   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
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


   /* Enable DAC Channel1 */
   DAC_Cmd(DAC_Channel_1, ENABLE);

   /* Enable DMA for DAC Channel1 */
   DAC_DMACmd(DAC_Channel_1, ENABLE);

   //---------------DAC channel2 Configuration------------------//
   DAC_InitTypeDef            DAC_InitStructure_CH2;
   DAC_InitStructure_CH2.DAC_Trigger = DAC_Trigger_T7_TRGO;
   DAC_InitStructure_CH2.DAC_WaveGeneration = DAC_WaveGeneration_None;
   DAC_InitStructure_CH2.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0; //0x0
   DAC_InitStructure_CH2.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

   DAC_Init(DAC_Channel_2, &DAC_InitStructure_CH2);

   //DMA-Channel 3
   DMA_DeInit(DMA1_Channel3);

   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R2_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Rvrs_Escalator8bit;
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

   DMA_Cmd(DMA1_Channel3, ENABLE);


   // Enable DAC Channel2 //
   DAC_Cmd(DAC_Channel_2, ENABLE);

   // Enable DMA for DAC Channel2 //
   DAC_DMACmd(DAC_Channel_2, ENABLE);


   /* 
   //------------------TRI------------------------//
   //very slow, probably not that useful
   DAC_InitStructure.DAC_Trigger = DAC_Trigger_T7_TRGO;
   DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
   DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
   DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
   DAC_Init(DAC_Channel_2, &DAC_InitStructure);  

   // Enable DAC Channel2: Once the DAC channel2 is enabled, PA.05 is
   // automatically connected to the DAC converter.
   DAC_Cmd(DAC_Channel_2, ENABLE);
   
   //DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);

   */
}
