#include "dac_dma_tim.h"

DMA_InitTypeDef            DMA_InitStructure;
extern __IO uint8_t buffer_finished;
extern __IO uint8_t bufferToSend;
/*
const uint8_t Sine8Bit[32] = {
	0,1,7,18,34,54,77,101,127,153,177,200,220,236,247,254,
	255,251,242,228,210,189,165,140,114,87,65,44,26,12,3,1
};

const uint8_t Tri8Bit[32] = {
	0,20,40,60,80,100,120,140,160,180,200,210,220,230,240,250,
	255,240,230,220,210,200,180,160,140,120,100,80,60,40,20,10
};
*/



void DAC_Config(void)
{
  //RCC_MSIRangeConfig(RCC_MSIRange_5);

  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);
  /*!< Wait till HSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//SYSCLK = 16MHz
  
  RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB = 16MHz [DMA][GPIO]
  RCC_PCLK1Config(RCC_HCLK_Div2);  //APB1 (low sleed) = 8MHz [DAC] -> [TIMERS 2-7]: 8MHz*2 = 16MHz
  RCC_PCLK2Config(RCC_HCLK_Div2);  //APB2 (high speed) = 8MHz [SPI]
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//disconnect HSE

  /* DMA1 clock enable (to be used with DAC) */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* GPIOA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog */

  GPIO_InitTypeDef GPIO_InitStructure;
  //GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //<--these two aren't set
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;//<--for analog mode
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_TIM_Config(void)
{
   /* TIM2 Configuration ------------------------------------------------------*/
  /* TIM2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* Time base configuration 
   * we want 22KHz = 22,000 or 1/22000s per update
   * MSI is at 2.097 MHz = 2,097,000
   * 2,097,000 / 10 = 209,700 -> Prescalar 10
   * 209,700 / 9.5   = 22,073Hz
  */
  /*
   * With AHB = 16MHz
   * APB1 Prescalar = 2 -> 8MHz
   * TIM2 clock is APB1 x 2 = 16MHz
  */
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 10; //222kHz/10 = 22.2kHz
  TIM_TimeBaseStructure.TIM_Prescaler = 18;//should give 4000KHz/18 = 222KHz timer
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; //0x0; <--DIV4 should make the clock at 4MHz
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM2 TRGO selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

void DAC_DMA_TIM_init(void)
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
   DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;
   /* DAC Channel2 Init */
   DAC_Init(DAC_Channel_2, &DAC_InitStructure);


   DMA_DeInit(DMA1_Channel3);

   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R2_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&readBuf1;//&Sine8Bit;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;//32;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
   //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   //DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel3, &DMA_InitStructure);
   DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);

   /* Enable DMA1 Channel3 */
   DMA_Cmd(DMA1_Channel3, ENABLE);

   /* Enable DAC Channel2 */
   DAC_Cmd(DAC_Channel_2, ENABLE);

   /* Enable DMA for DAC Channel2 */
   DAC_DMACmd(DAC_Channel_2, ENABLE);
}

void DMA1_Channel3_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC3) != RESET) //DMA1_IT_TC3: DMA1 Channel3 transfer complete interrupt.DMA1_IT_TC3: DMA1 Channel3 transfer complete interrupt.
   {
	DAC_Cmd(DAC_Channel_2, DISABLE);
	/* Enable DMA for DAC Channel2 */
	DAC_DMACmd(DAC_Channel_2, DISABLE);

	/* Disable DMA1 Channel3 in order to load new buffer address */
        DMA_Cmd(DMA1_Channel3, DISABLE);

	//switch DMA_MemoryBaseAddr to recently filled buffer
	if(bufferToSend == 1)
	{
	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&readBuf1;
//	  bufferToSend = 2;
//	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&Sine8Bit;
	}
	else
	{
	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&readBuf2;
//	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&Tri8Bit;
//	  bufferToSend=1;
	}

	//Tell main loop to load new buffer
	buffer_finished = 1;
/*
	if(DMA_GetFlagStatus(DMA1_FLAG_TC3) == SET)
	{
	   DMA_ClearFlag(DMA1_FLAG_TC3);
	}
*/
        DMA_ClearITPendingBit(DMA1_IT_GL3);

        DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	/* Enable DMA1 Channel3 */
        DMA_Cmd(DMA1_Channel3, ENABLE);
	
	DAC_Cmd(DAC_Channel_2, ENABLE);
	/* Enable DMA for DAC Channel2 */
        DAC_DMACmd(DAC_Channel_2, ENABLE);
   }
}

