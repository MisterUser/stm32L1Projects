#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "implementations/usart_int_and_q.h"
#include "implementations/clock_setup.h"
#include "implementations/hd44780_4bit_lib.h"
#include "implementations/dac_signalGen.h"

void Delay(uint32_t nTime);


int main(void){

  clock_setup();

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  
  //Configure Pins
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  usart_int_and_q_init();

  hd44780_init(N_2LINE,FONT_8);

  hd44780_setCursorPosition(0,0);
  hd44780_write_string("-F1:OFF");
  hd44780_setCursorPosition(1,0);
  hd44780_write_string("-F2:OFF");
  hd44780_setCursorPosition(2,0);
  hd44780_write_string("-F3:OFF");
  hd44780_setCursorPosition(3,0);
  hd44780_write_string("-F4:OFF");
  
  DAC_signalGen_init();

  //Configure SysTick Timer (in core_cm4.h)
  //Set System Clock to interrupt every ms. HCLK/8 = 2MHz. Each tick is 500ns
  if(SysTick_Config(SystemCoreClock/8000))
	while (1); //If fails, hang in while loop 

  char userVal;

  while (1) {
    static int ledval = 0;
    static int F1_on=0;
    static int F2_on=0;

    if(!gk_USART_RX_QueueEmpty())
    {
       userVal = usart_w_interrupt_getchar();

       	if(userVal == '1')
       	{
	 GPIO_WriteBit(GPIOA,GPIO_Pin_2,(F1_on)?Bit_SET:Bit_RESET);
	 F1_on=1-F1_on;
	}
	else if(userVal == '2')
	{
	 GPIO_WriteBit(GPIOA,GPIO_Pin_3,(F2_on)?Bit_SET:Bit_RESET);
	 F2_on=1-F2_on;
	}
	else
	{	
   	   usart_w_interrupt_putchar('\n');
   	   usart_w_interrupt_putchar('R');
   	   usart_w_interrupt_putchar('c');
   	   usart_w_interrupt_putchar('v');
   	   usart_w_interrupt_putchar('d');
   	   usart_w_interrupt_putchar(':');
   	   usart_w_interrupt_putchar(userVal);
   	   usart_w_interrupt_putchar('\n');
   
   	   hd44780_write_char(userVal);
	}
    }
    else
    {
      
       //toggle led
       GPIO_WriteBit(GPIOB,GPIO_Pin_6,(ledval)? Bit_SET : Bit_RESET);
       ledval = 1-ledval;
    }

    Delay(5); //wait 250ms
  }
}

//Timer code
static __IO uint32_t TimingDelay ;

void Delay( uint32_t nTime ){
  TimingDelay = nTime ;
  while ( TimingDelay != 0);
}

void SysTick_Handler (void){
  if ( TimingDelay != 0x00)
  TimingDelay --;
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
#endif
