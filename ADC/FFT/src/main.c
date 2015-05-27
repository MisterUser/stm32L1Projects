#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_tim.h>
#include "implementations/usart_int_and_q.h"
#include "implementations/adc_dma_cont.h"
 
void Delay(uint32_t nTime);
extern uint16_t sampleVals1[4096];
extern uint16_t sampleVals2[4096];
extern uint8_t convDoneFlag;
extern uint8_t destBuffer;

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

  //Configure SysTick Timer
  //Set System Clock to interrupt every ms.
  if(SysTick_Config(SystemCoreClock / 1000))
	while (1); //If fails, hang in while loop 

  char userVal;
  //uint16_t sampleVal=0x0f0f; -> w/ interrupt, move sampleVal to adc file
  int sampleIter;
  int bitOfResult;
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
       GPIO_WriteBit(GPIOB,GPIO_Pin_7,(ledval)? Bit_SET : Bit_RESET);
       //toggle led
       ledval = 1-ledval;
//       GPIO_WriteBit(GPIOB,GPIO_Pin_7,Bit_SET);

	destBuffer = 1-destBuffer;

	convDoneFlag=0; //set by ADC1_IRQ_Handler()
	//The following two lines are if the ADC doesn't have EOC interrupt enables
	//  W/o the interrupt, it must poll the EOC flag before it gets the conversion value
	//while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	//sampleVal = ADC_GetConversionValue(ADC1); <--now done in ADC1_IRQ_Handler()

	sampleIter = 0;
	for(;sampleIter<20;sampleIter++)
	{
    	   bitOfResult = 11;
	   while(bitOfResult >= 0)
	   {
	      if((uint16_t)(sampleVals1[sampleIter] >> bitOfResult)&(uint16_t)1)
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
	//usart_w_interrupt_putchar('\n');	
	usart_w_interrupt_putchar('\r');	

        sampleIter = 0;
        for(;sampleIter<20;sampleIter++)
        {
           bitOfResult = 11;
           while(bitOfResult >= 0)
           {
              if((uint16_t)(sampleVals2[sampleIter] >> bitOfResult)&(uint16_t)1)
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
        //usart_w_interrupt_putchar('\n');      
        usart_w_interrupt_putchar('\r');

    }
    Delay(10); //wait 10ms
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
