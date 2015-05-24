#include "adc_dma_cont.h"

uint16_t sampleVal;
uint8_t newSampleFlag;

void adc_dma_cont_init()
{

   //-----------------------Clocks------------------------//
   /* Enable HSI Clock */
   RCC_HSICmd(ENABLE);
  
   /*!< Wait till HSI is ready */
   while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
 
   RCC_HSEConfig(RCC_HSE_OFF);  
   if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
   {
     while(1);
   }
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1| RCC_APB2Periph_SYSCFG, ENABLE);
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
    //-----------------------GPIOs------------------------//
   GPIO_InitTypeDef  GPIO_InitStructure;

   //Initialize PA5 as analog input
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /*------------------------ DMA1 configuration ------------------------------*/
   /* Enable DMA1 clock */
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
   /* DMA1 channel1 configuration */
   DMA_DeInit(DMA1_Channel1);
   DMA_InitTypeDef DMA_InitStructure;
   	//I/O
   DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&sampleVal;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //Xfer direction (from peripheral as SRC)
   	//Data Info
   DMA_InitStructure.DMA_BufferSize = 1;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//Modes
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //increment peripheral address
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; //increment mem address after write
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //Prepare DMA for new Xfer after previous complete
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;//priority of Xfer
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //mem to mem
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);

   /* Enable Transfer Complete Interrupt*/
   DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);

   //-----------------------INTERRUPTS-------------------------//
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitTypeDef NVIC_InitStructure;
   
   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   //------------------------ADC--------------------------// 
   /* Wait until ADC starts */
   uint16_t T_StartupTimeDelay = 1024;
   while (T_StartupTimeDelay--);
   
   ADC_DeInit(ADC1);

   ADC_CommonInitTypeDef ADC_CommonInitStruct;
   ADC_CommonStructInit(&ADC_CommonInitStruct);
   ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;
   ADC_CommonInit(&ADC_CommonInitStruct);

   ADC_InitTypeDef ADC_InitStructure;
   ADC_StructInit(&ADC_InitStructure);

   ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion = 1;
    
   ADC_Init(ADC1, &ADC_InitStructure);

   //ADC | ADC Channel | Rank | ADC Sample Time//
   ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_384Cycles);
   
   //ADC_BankSelection(ADC1,ADC_Bank_A);

   /* Enable the request after last transfer for DMA Circular mode */
   ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

   /* Enable ADC1 DMA */
   ADC_DMACmd(ADC1, ENABLE);

   /* Enable ADC1 */
   ADC_Cmd(ADC1, ENABLE);
   
   /* Wait until the ADC1 is ready */
   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
   {
   }
   /* Start ADC1 Software Conversion */
   ADC_SoftwareStartConv(ADC1);

   sampleVal=0x0f0f;
   newSampleFlag=0; 
}

void DMA1_Channel1_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
   {
	newSampleFlag=1;
	DMA_ClearITPendingBit(DMA1_IT_TC1);
   }
}
