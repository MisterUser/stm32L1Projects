#include "extern_DAC.h"

//------------Functions-----------------//
const uint8_t fourBitSine_16[16]={8,11,13,14,15,14,12,10,7,4,2,1,0,1,3,5};
const uint8_t fourBitTri_15[15]={1,3,5,7,9,11,13,15,13,11,9,7,5,3,1};
const uint8_t fourBitEscalator_16[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


//------Local variables------//
const uint8_t* dac1_array_ptr;
uint8_t lastOfarray1;
uint8_t dac1_array_current;

const uint8_t* dac2_array_ptr;
uint8_t lastOfarray2;
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

   /* TIM IT disable */
   TIM_ITConfig(TIM9, TIM_IT_Update, DISABLE);

   /* -----------NVIC Configuration ------------*/
   /* Enable the TIM9 global Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /* Time base configuration */
   //with TimerCLK = 16MHz / Prsclr=16 and 16-bit function -> 62.5kHz >= function
   //Period: 1=5.87kHz, 65000=1.02Hz (max 65,535)
   //	     65=1k , 650 = 103Hz, 
   //	     166=400
   // 4MHz/16-long -> 250k max to match the internal DAC settings
   //247 and not 250 because increase freq (1k) by 1% (1010k) -> w/ interrupt delay->1kHz
   TIM_TimeBaseStructure.TIM_Period = 247; //4MHz/250=16kHz ->16-long ->1kHz
   TIM_TimeBaseStructure.TIM_Prescaler = 3;//+1 -> 4MHz
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

   dac1_array_ptr=&fourBitSine_16[0];
   dac1_array_current=0;
   lastOfarray1 = 15; 

   /* TIM enable counter */
//   TIM_Cmd(TIM9, ENABLE);
   
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

   //Timer
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

   /* TIM IT disable */
   TIM_ITConfig(TIM10, TIM_IT_Update, DISABLE);

   /* -----------NVIC Configuration ------------*/
   /* Enable the TIM10 global Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   /* Time base configuration */
   TIM_TimeBaseStructure.TIM_Period = 247;
   TIM_TimeBaseStructure.TIM_Prescaler = 3;//+1 -> 4MHz
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

   TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

   dac2_array_ptr=&fourBitTri_15[0];
   dac2_array_current=0;
   lastOfarray2 = 14;

   /* TIM enable counter */
 //  TIM_Cmd(TIM10, ENABLE);

   /* TIM IT enable */
   TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);
}

void set_external_DAC_freq(char channel,uint16_t freq,char hz_or_khz)
{
  uint32_t freqInHz = freq;
  uint8_t sizeOfFunction;

  TIM_TypeDef * timer;
  if(channel=='3')
  {
     timer = TIM9;
     sizeOfFunction = lastOfarray1+1;
  }
  else if(channel=='4') 
  {
     timer = TIM10;
     sizeOfFunction = lastOfarray2+1;
  }
  else {return;}

  TIM_Cmd(timer, DISABLE);
  if(hz_or_khz == 'k' || hz_or_khz == 'K')
  {
     freqInHz=freqInHz*1000;
  }
  
  //add offset for interrupt (~10 @1k, 1@100, and ignore the 10s)
  if(freqInHz>=100)
  {
    freqInHz=freqInHz+(freqInHz/100);//add 
  }


      //250k P=1,125k P=2,83.3kP=3,62.5 P=4,50k  P=5,41.67P=6,35.7 P=7,31.25  8,27.7  9,25 10
        //22.7,20.8,19.2,17.85,16.67,15.6,14.7,13.9,13.15,12.5
        //P=25 -> 10k,P=50 -> 5k,P=65,000 -> 3.8Hz
     //with 8MHz and a 32-length function, can only do 250kHz, but
     //timer->PSC=1;//16000000/2=8MHz
     //timer->ARR=(1+((80000-1)/freq))/32;//ceiling function with /32 for 32-length function
  
  if(freqInHz>=4 && freqInHz<=250000)
  {
    timer->ARR=(4000000/sizeOfFunction)/freqInHz;//ARR = {1,62500}
    TIM_Cmd(timer,ENABLE);
  }

}

void set_external_DAC_shape(char channel,char shape)
{
  if(channel=='3')
  {
     switch(shape)
     {
	case 'S':
         dac1_array_ptr=&fourBitSine_16[0];
         lastOfarray1=15;
	break;
	case 'E':
         dac1_array_ptr=&fourBitEscalator_16[0];
         lastOfarray1=15;
	break;
	case 'T':
         dac1_array_ptr=&fourBitTri_15[0];
         lastOfarray1=14;
	break;
	default:
	break;
     }
     dac1_array_current=0;
  }
  else if(channel=='4')
  {
     switch(shape)
     {
	case 'S':
         dac2_array_ptr=&fourBitSine_16[0];
         lastOfarray2=15;
	break;
	case 'E':
         dac2_array_ptr=&fourBitEscalator_16[0];
         lastOfarray2=15;
	break;
	case 'T':
         dac2_array_ptr=&fourBitTri_15[0];
         lastOfarray2=14;
	break;
	default:
	break;
     }
     dac2_array_current=0;
  }
}

void TIM9_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    //GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET);

    //~9.4us process to update the bits -> external DACs are limited to 100kHz
    //@1k, this brings the frequency to 990Hz -> 1% 
    //@100Hz, 99.9Hz ->.1%, or  (10us)/ (1/100Hz) = .00001/.01 = .001
    //2500 ->2475
    //	.001*100 = .1 -> make frequency 100.1 and this gets increased by .00001 -> 99.9999Hz
    //@10Hz, 9.999Hz
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    //Set pins according to array

    //--Using STL -> slow
    //GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin4_MSB,!!((dac1_array_ptr[dac1_array_current]) & 8));
    //GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin3,!!((dac1_array_ptr[dac1_array_current]) & 4));
    //GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin2,!!((dac1_array_ptr[dac1_array_current]) & 2));
    //GPIO_WriteBit(ExtDAC1_PORT,ExtDAC1_Pin1_LSB,!!((dac1_array_ptr[dac1_array_current++]) & 1));

    //--bit write operations, MUCH FASTER than using GPIO_WriteBit()!!!
    //array access + bitwise & + shift + bitset ~ 4 clock cycles (5 w/ NOT op in reset)
    ExtDAC_resetPin   = ((dac1_array_ptr[dac1_array_current])&0x0F) << ExtDAC1_PortOffset;
    ExtDAC_setPin = ((~(dac1_array_ptr[dac1_array_current++]))&0x0F) << ExtDAC1_PortOffset;
    if(dac1_array_current>=lastOfarray1){dac1_array_current = 0;}

    //GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_RESET);
  } 
}

void TIM10_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);

    //Set pins according to array
    ExtDAC_resetPin   = ((  dac2_array_ptr[dac2_array_current])&0x0F) << ExtDAC2_PortOffset;
    ExtDAC_setPin = ((~(dac2_array_ptr[dac2_array_current++]))&0x0F) << ExtDAC2_PortOffset;
    if(dac2_array_current>=lastOfarray2){dac2_array_current = 0;}
  }
}


