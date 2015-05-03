#include "dac_dma_tim.h"

DMA_InitTypeDef            DMA_InitStructure;
extern __IO uint8_t buffer_finished;
extern __IO uint8_t bufferToSend;

void DAC_Config(void)
{
  //RCC_MSIRangeConfig(RCC_MSIRange_5);

  /* Enable HSI Clock */
  RCC_HSICmd(DISABLE);
  /*!< Wait till HSI is ready */
  //while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  //disconnect HSE
  RCC_HSEConfig(RCC_HSE_OFF);
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  {
     while(1);
  }

  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//SYSCLK = 16MHz
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);//SYSCLK = default 2.097MHz
  RCC_MSIRangeConfig(RCC_MSIRange_5);//range 5 = 2.097MHz
 
  //AHB (HCLK, AHB bus, core, memory, and DMA, FCLK)
  //Cortex System timer = AHB/8
  RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB = 2.097 [DMA][GPIO]

  //APB clocks are based on AHB
  //APB1 -> PCLK1 to APB1 Peripherals (including TIM2-7)
  //if APB1 prescalar = 1, TIM2-7 clocks = APB1
  //if APB1 prescalar > 1, TIM2-7 clocks = APB1*2 
  RCC_PCLK1Config(RCC_HCLK_Div1);  //APB1 (low sleed) = 2MHz [DAC] -> [TIMERS 2-7]: 2MHz*1 = 2MHz

  //APB2 -> PCLK2 to APB2 Peripherals (including TIM9-11)
  //if APB2 prescalar = 1, TIM9-11 clocks = APB2
  //if APB2 prescalar > 1, TIM9-11 clocks = APB2*2
  RCC_PCLK2Config(RCC_HCLK_Div2);  //APB2 (high speed) = 2MHz [SPI]

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

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
  TIM_TimeBaseStructure.TIM_Period = 95; //2097kHz/95 = 22.07kHz
  TIM_TimeBaseStructure.TIM_Prescaler =1;//should give 2097KHz/1 = 2097KHz timer
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //DIV1 -> TIM2CLK = 2MHz
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
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&readBuf1;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
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
	/* Break connection between DMA for DAC Channel3 */
	DAC_DMACmd(DAC_Channel_2, DISABLE);

	/* Disable DMA1 Channel3 in order to load new buffer address */
   //     DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_DeInit(DMA1_Channel3);	

	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R2_Address;
   //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&readBuf1;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
 //  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;


	if(bufferToSend == 1)
	{
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&readBuf1;
//	  bufferToSend = 2;
	}
	else
	{
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&readBuf2;
//	  bufferToSend = 1;
	}

   DMA_Init(DMA1_Channel3, &DMA_InitStructure);
   DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);

/*
	//switch DMA_MemoryBaseAddr to recently filled buffer
	if(bufferToSend == 1)
	{
	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&readBuf1;
	}
	else
	{
	  ((DMA_Channel_TypeDef*)DMA1_Channel3)->CMAR = (uint32_t)&readBuf2;
	}
*/
	//Tell main loop to load new buffer
	buffer_finished = 1;

        DMA_ClearITPendingBit(DMA1_IT_GL3);

        //DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);
	/* Enable DMA1 Channel3 */
        DMA_Cmd(DMA1_Channel3, ENABLE);
	
	DAC_Cmd(DAC_Channel_2, ENABLE);
	/* Enable DMA for DAC Channel2 */
        DAC_DMACmd(DAC_Channel_2, ENABLE);
   }
}

