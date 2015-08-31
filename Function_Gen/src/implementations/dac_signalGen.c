#include "dac_signalGen.h"

const uint8_t Sine8Bit_32[32] = {
	0,1,7,18,34,54,77,101,127,153,177,200,220,236,247,254,
	255,251,242,228,210,189,165,140,114,87,65,44,26,12,3,1
};

//const uint8_t Sine8Bit_16[16]={0  ,  7, 34, 77,127,177,220,247,255,242,210,165,114, 65, 26,  3};

const uint8_t Escalator8bit[32] = {
	0x00,0x08,0x10,0x18,
	0x20,0x28,0x30,0x38,
        0x40,0x48,0x50,0x58,
	0x60,0x68,0x70,0x78,
	0x80,0x88,0x90,0x98,
	0xA0,0xA8,0xB0,0xB8,
	0xC0,0xC8,0xD0,0xD8,
	0xE0,0xE8,0xF0,0xF8
};

const uint8_t Rvrs_Escalator8bit[32] = {
        0xF8,0xF0,0xE8,0xE0,0xD8,0xD0,0xC8,0xC0,
        0xB8,0xB0,0xA8,0xA0,0x98,0x90,0x88,0x80,
        0x78,0x70,0x68,0x60,0x58,0x50,0x48,0x40,
        0x38,0x30,0x28,0x20,0x18,0x10,0x08,0x00
};

const uint8_t Triangle_8bit[32] = {
	0x00,0x10,0x20,0x30,
        0x40,0x50,0x60,0x70,
	0x80,0x90,0xA0,0xB0,
	0xC0,0xD0,0xE0,0xF0,
	0xFF,0xF0,0xE0,0xD0,
	0xC0,0xB0,0xA0,0x90,
	0x80,0x70,0x60,0x50,
	0x40,0x30,0x20,0x10
};

uint8_t Square_8bit[32]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


void DAC_Config(void)
{
  // DMA1 clock enable (to be used with DAC) 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  // DAC Periph clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  // GPIOA clock enable 
  // Configure PA.04 (DAC_OUT1), PA.05 (DAC_OUT2) as analog 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  //--------- TIM6 Configuration for Channel 1----------------//
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 250;//->8MHz/250 with 32-long functino=1kHz 
  TIM_TimeBaseStructure.TIM_Prescaler = 1;//+1=2 -> 8MHz
  //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4;//TIM6 has no CLKDIV
  //TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM6 can only count up
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

   //Moved enable commands to main.c
  /* TIM6 enable counter */
  //TIM_Cmd(TIM6, ENABLE);

  /* TIM7 Configuration ------------------------------------------------------*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = 250;// Function;//with 16MHz and 16-bit signal -> 1MHz signal. 
  TIM_TimeBaseStructure.TIM_Prescaler = 1;//PSC+1=15+1=divide by 16 for 1M-1.1k, divide by 4000 for 1Hz-1k
  //TIM7has no CLKDIV
  //TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //This isn't what it seems. Just keep at 1.
  //TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;TIM7 can only count up
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  /* TIM7 TRGO selection */
  TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);

   //Moved enable commands to main.c
  /* TIM7 enable counter */
  //TIM_Cmd(TIM7, ENABLE);

}

void DAC_signalGen_init(void)
{
   DAC_DeInit();
   DAC_Config();
   DAC_TIM_Config();
/*
   DMA_DeInit(DMA1_Channel1);  
   DMA_DeInit(DMA1_Channel2);  
   DMA_DeInit(DMA1_Channel3);  
   DMA_DeInit(DMA1_Channel4);  
   DMA_Cmd(DMA1_Channel1, DISABLE); 
   DMA_Cmd(DMA1_Channel2, DISABLE); 
   DMA_Cmd(DMA1_Channel3, DISABLE); 
   DMA_Cmd(DMA1_Channel4, DISABLE); 
*/

   //---------------DAC channel1 Configuration------------------//
   //set ((DAC_TypeDef*)0x40007400)->CR &= 0X6
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
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine8Bit_32;
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


   //Moved enable commands to main.c
   /* Enable DAC Channel1 */
   //DAC_Cmd(DAC_Channel_1, ENABLE);

   /* Enable DMA for DAC Channel1 */
   //DAC_DMACmd(DAC_Channel_1, ENABLE);

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
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Sine8Bit_32;
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


   //Moved enable commands to main.c
   // Enable DAC Channel2 //
   //DAC_Cmd(DAC_Channel_2, ENABLE);

   // Enable DMA for DAC Channel2 //
   //DAC_DMACmd(DAC_Channel_2, ENABLE);


   /* 
   //------------------TRI------------------------//
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


void function_on(int channel)
{
  if(channel==1)
  {
     TIM_Cmd(TIM6, ENABLE);
     DAC_Cmd(DAC_Channel_1,ENABLE);
     DAC_DMACmd(DAC_Channel_1,ENABLE);
  }
  else if(channel==2)
  {
     TIM_Cmd(TIM7, ENABLE);
     DAC_Cmd(DAC_Channel_2,ENABLE);
     DAC_DMACmd(DAC_Channel_2,ENABLE);
  }
}
void function_off(int channel)
{
  if(channel==1)
  {
     DAC_DMACmd(DAC_Channel_1,DISABLE);
     DAC_Cmd(DAC_Channel_1,DISABLE);
     TIM_Cmd(TIM6, DISABLE);
  }
  else if(channel==2)
  {
     DAC_DMACmd(DAC_Channel_2,DISABLE);
     DAC_Cmd(DAC_Channel_2,DISABLE);
     TIM_Cmd(TIM7, DISABLE);
  }
}

void set_internal_DAC_freq(char channel,uint16_t freq,char hz_or_khz)
{
  uint32_t freqInHz = freq;

  TIM_TypeDef * timer;
  if(channel=='1')
  {
     timer = TIM6;
  }
  else {timer = TIM7;}

  TIM_Cmd(timer, DISABLE);
  if(hz_or_khz == 'k' || hz_or_khz == 'K')
  {
     freqInHz=freqInHz*1000; 
  }
     	//250k P=1,125k P=2,83.3kP=3,62.5 P=4,50k  P=5,41.67P=6,35.7 P=7,31.25	8,27.7	9,25 10
	//22.7,20.8,19.2,17.85,16.67,15.6,14.7,13.9,13.15,12.5
	//P=25 -> 10k,P=50 -> 5k,P=65,000 -> 3.8Hz
     //with 8MHz and a 32-length function, can only do 250kHz, but 
     //timer->PSC=1;//16000000/2=8MHz
     //timer->ARR=(1+((80000-1)/freq))/32;//ceiling function with /32 for 32-length function
  if(freqInHz>=4 && freqInHz<=250000)
  {
    timer->ARR=250000/freqInHz;//ARR = {1,62500}
    TIM_Cmd(timer,ENABLE);
  }
}

void set_internal_DAC_shape(char channel, char functionShape)
{
    TIM_TypeDef* timer;
    DMA_Channel_TypeDef*  DMA_ch;
    uint32_t DAC_ch;

    if(channel=='1')
    {
	timer=TIM6;	
	DAC_ch=DAC_Channel_1;	
	DMA_ch=DMA1_Channel2;
    }
    else if(channel=='2')
    {
	timer=TIM7;
	DAC_ch=DAC_Channel_2;	
        DMA_ch=DMA1_Channel3;
    }
    else {return;}

     //first disable DMA, DAC, and TIM
     DAC_DMACmd(DAC_ch,DISABLE);
     DAC_Cmd(DAC_ch,DISABLE);
     DMA_Cmd(DMA_ch, DISABLE);
     TIM_Cmd(timer, DISABLE);

     switch(functionShape)
     {
     	case 'S':
	  DMA_ch->CMAR=(uint32_t)&Sine8Bit_32;
	break;
	case 'E':
	  DMA_ch->CMAR=(uint32_t)&Escalator8bit;
	break;
	case 'R':
	  DMA_ch->CMAR=(uint32_t)&Rvrs_Escalator8bit;
	break;
	case 'T':
	  DMA_ch->CMAR=(uint32_t)&Triangle_8bit;
	break;
	case 'P':
	  DMA_ch->CMAR=(uint32_t)&Square_8bit;
	break;
	default:
	  return;
     }
     //lastly enable TIM, DAC, DMA
     TIM_Cmd(timer, ENABLE);
     DMA_Cmd(DMA_ch, ENABLE);
     DAC_Cmd(DAC_ch,ENABLE);
     DAC_DMACmd(DAC_ch,ENABLE);
}
