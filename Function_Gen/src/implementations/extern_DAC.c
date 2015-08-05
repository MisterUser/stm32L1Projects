#include "extern_DAC.h"

extern const uint8_t fourBitSine_16[16];
const uint8_t fourBitTri_10[10]={0,3,6,9,12,15,12,9,6,3};
const uint8_t fourBitRamp_6[6]={0,3,6,9,12,15};
const uint8_t* dac1_array_ptr;
uint8_t sizeOfarray1;
uint8_t dac1_array_current;
const uint8_t* dac2_array_ptr;
uint8_t sizeOfarray2;
uint8_t dac2_array_current;

GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void external_DAC_setup(void)
{
   //Enable Peripheral Clocks
   RCC_AHBPeriphClockCmd(ExtDAC1_CLK, ENABLE);

   //Configure Pins
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Pin = ExtDAC1_Pin1_LSB | ExtDAC1_Pin2 | ExtDAC1_Pin3 | ExtDAC1_Pin4_MSB;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz ;
   GPIO_Init(ExtDAC1_PORT, &GPIO_InitStructure);

   //Timer
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

   /* TIM IT enable */
   TIM_ITConfig(TIM9, TIM_IT_Update, DISABLE);

   /* -----------NVIC Configuration ------------*/
   /* Enable the TIM9 global Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /* Time base configuration */
   TIM_TimeBaseStructure.TIM_Period = 10;
   TIM_TimeBaseStructure.TIM_Prescaler = 25;
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

   dac1_array_ptr=&fourBitSine_16[0];
   dac1_array_current=0;
   sizeOfarray1 = 15; 

   /* TIM enable counter */
   TIM_Cmd(TIM9, ENABLE);
   
   /* TIM IT enable */
   TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);
	  
   //--------------------------------------DAC2---------------------------// 
   //Enable Peripheral Clocks
   RCC_AHBPeriphClockCmd(ExtDAC2_CLK, ENABLE);

   //Configure Pins
   GPIO_StructInit(&GPIO_InitStructure);
   GPIO_InitStructure.GPIO_Pin = ExtDAC2_Pin1_LSB | ExtDAC2_Pin2 | ExtDAC2_Pin3 | ExtDAC2_Pin4_MSB;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz ;
   GPIO_Init(ExtDAC2_PORT, &GPIO_InitStructure);
}

void TIM9_IRQHandler(void)
{
  static int pinVal=0;
  if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    //Set pins according to array
    //x/t ((GPIO_TypeDef *)0x40020800)->BSRRL
    //GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin1_LSB,(pinVal)? Bit_SET : Bit_RESET);
    //pinVal=1-pinVal;
    GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin4_MSB,!!((dac1_array_ptr[dac1_array_current]) & 8));
    GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin3,!!((dac1_array_ptr[dac1_array_current]) & 4));
    GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin2,!!((dac1_array_ptr[dac1_array_current]) & 2));
    GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin1_LSB,!!((dac1_array_ptr[dac1_array_current++]) & 1));
    //ExtDAC_setPin = ((dac1_array_ptr[dac1_array_current++])&0x0F) << ExtDAC1_PortOffset;
    if(dac1_array_current>=sizeOfarray1){dac1_array_current = 0;}
  } 
}