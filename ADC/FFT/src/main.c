#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_tim.h>
#include "implementations/usart_int_and_q.h"
#include "implementations/adc_dma_cont.h"
#include "implementations/dac_signalGen.h"
 
void Delay(uint32_t nTime);
extern volatile uint16_t sampleVals1[4096];
extern volatile uint16_t sampleVals2[4096];
extern volatile uint8_t convDoneFlag;
extern volatile uint8_t destBuffer;
char userVal;
  //uint16_t sampleVal=0x0f0f; -> w/ interrupt, move sampleVal to adc file
int sampleIter;
volatile uint16_t * fftBuffer;

void usart_print_bits(uint16_t value,uint8_t bitsToPrint);
void usart_print_decimal(uint16_t value,uint8_t numDigits);

int main(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  
  //Configure Pins - PB7 for USART RX LED
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Configure Pins - PB6 for Timer LED
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  usart_int_and_q_init();
  adc_dma_cont_init();
  DAC_signalGen_init(); //use this for testing

  //Configure SysTick Timer
  //Set System Clock to interrupt every ms.
  if(SysTick_Config(SystemCoreClock / 1000))
	while (1); //If fails, hang in while loop 

  destBuffer=1;

  while (1) {
    static int ledval = 0;
    

    if(!gk_USART_RX_QueueEmpty())
    {
       userVal = usart_w_interrupt_getchar();

       usart_w_interrupt_putchar('\n');
       usart_w_interrupt_putchar('\r');
       usart_w_interrupt_putchar('R');
       usart_w_interrupt_putchar('c');      
       usart_w_interrupt_putchar('v');
       usart_w_interrupt_putchar('d');
       usart_w_interrupt_putchar(':');
       usart_w_interrupt_putchar(userVal);
       usart_w_interrupt_putchar('\n');
       usart_w_interrupt_putchar('\r');
    }
    if(convDoneFlag)
    {
        //clear flag 
	convDoneFlag=0; //set by ADC1_IRQ_Handler()
        
	//toggle led
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,(ledval)? Bit_SET : Bit_RESET);
        ledval = 1-ledval;
/*
	//set fft pointer to buffer that was newly loaded by ADC	
        if(destBuffer)//1=buffer 2 is currently being loaded by ADC
        {
	   //so buffer 1 will be processed (just finished being loaded by ADC)
	   fftBuffer = sampleVals1;
	}
	else {fftBuffer = sampleVals2;}
*/

	fftBuffer = sampleVals1;
	//print values to USART (for testing only)
	usart_w_interrupt_putchar('{');
	sampleIter = 0;
	for(;sampleIter<32;sampleIter++)
	{
	   //usart_print_bits(fftBuffer[sampleIter],12);
	   usart_print_decimal(fftBuffer[sampleIter],4);
	   usart_w_interrupt_putchar(',');
	}
	usart_w_interrupt_putchar('}');
	usart_w_interrupt_putchar('\n');	
	//usart_w_interrupt_putchar('\r');	

	//after FFT is finished, designate current fftBuffer as next destBuffer for ADC
	destBuffer = 1-destBuffer;
    }
    Delay(10); //wait 10ms
  }
}

void usart_print_bits(uint16_t value,uint8_t bitsToPrint)
{
   int bitOfResult = bitsToPrint-1;
   while(bitOfResult >= 0)
   {
      if((uint16_t)(value >> bitOfResult)&(uint16_t)1)
      {
         usart_w_interrupt_putchar((char)'1');
      }
      else
      {
         usart_w_interrupt_putchar((char)'0');
      }
      bitOfResult--;
   }
}

void usart_print_decimal(uint16_t value,uint8_t numDigits)
{
  int powOfTen;
  if(value>=10^(numDigits-1))
  {
     powOfTen = numDigits -1;
  }
  else if(value>=10^(numDigits-2))
  {
     powOfTen = numDigits -2;
  }
  else if(value>=10^(numDigits-3))
  {
     powOfTen = numDigits -3;
  }
  else powOfTen = 0;

  int denom;
  while(powOfTen >=0)
  {
        denom=10^powOfTen;
	usart_w_interrupt_putchar((char)((value/denom)+'0'));
	value = value%denom;
	powOfTen--;
  } 
   
}

//Timer code
static __IO uint32_t TimingDelay ;

void Delay(uint32_t nTime)
{
  TimingDelay = nTime ;
  while(TimingDelay != 0);
}

void SysTick_Handler(void)
{
  if(TimingDelay != 0x00)
  TimingDelay--;
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
#endif
