#include "adc_single_EOC_interrupt.h"

void adc_single_EOC_interrupt_init()
{

   //-----------------------Clocks------------------------//
   /* Enable HSI Clock */
   RCC_HSICmd(ENABLE);
  
   /*!< Wait till HSI is ready */
   while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
 
   //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
   //RCC_MSIRangeConfig(RCC_MSIRange_6);

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

   //-----------------------TIMER-------------------------//
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
   
   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

   //Internal High Speed Oscillator = 16,000,000 = 16MHz
   TIM_TimeBaseStructure.TIM_Prescaler = HSI_VALUE/1000; // 16,000,000/1,000 = 16,000 Prescalar
   TIM_TimeBaseStructure.TIM_Period = 1000; // 0..999
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
   
   TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);
   
   TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
   TIM_Cmd(TIM3, ENABLE);

   //-----------------------INTERRUPTS-------------------------//
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   //------------------------ADC--------------------------// 
   /* Wait until ADC + Temp sensor start */
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
   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
   ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; 
   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
   //ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfConversion = 1;
    
   ADC_Init(ADC1, &ADC_InitStructure);

   //ADC | ADC Channel | Rank | ADC Sample Time//
   ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_384Cycles);
   
   ADC_EOCOnEachRegularChannelCmd(ADC1,ENABLE);
   //ADC_BankSelection(ADC1,ADC_Bank_A);
   
   ADC_Cmd(ADC1, ENABLE);
   

   
}

void TIM3_IRQHandler(void)
{
    static int tim_LED_val=1;
    GPIO_WriteBit(GPIOB,GPIO_Pin_6,(tim_LED_val)? Bit_SET : Bit_RESET);
    tim_LED_val = 1-tim_LED_val;
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
