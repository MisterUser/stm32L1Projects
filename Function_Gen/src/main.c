#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "implementations/usart_int_and_q.h"
#include "implementations/clock_setup.h"
#include "implementations/hd44780_4bit_lib.h"
#include "implementations/dac_signalGen.h"
#include "implementations/extern_DAC.h"

void Delay(uint32_t nTime);


int main(void){

  clock_setup(); //clocks now at 16MHz (SYS,HCLK(AHP),PCLK1(APB1),PCLK2(APB2))

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

  //Control pins for DAC Ch1&Ch2
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  usart_int_and_q_init();

  
  Delay(150); //LCD has to wait 150ms after power on
  hd44780_init(N_2LINE,FONT_8);

  hd44780_setCursorPosition(0,0);
  hd44780_write_string("-F1:OFF");
  hd44780_setCursorPosition(1,0);
  hd44780_write_string("-F2:OFF");
  hd44780_setCursorPosition(2,0);
  hd44780_write_string("-F3:OFF");
  hd44780_setCursorPosition(3,0);
  hd44780_write_string("-F4:OFF");
  
  external_DAC_setup(); 
  DAC_signalGen_init();


  char userVal;
  volatile char functionNum;
  volatile uint16_t freq;
  volatile char Hz_or_kHz; 
  char functionShape;

  while (1) {
    static int ledval = 0;
    //State variables for functions: all functions start off
    static int F1_on=0;
    static int F2_on=0;
    static int F3_on=0;
    static int F4_on=0;
    static int F5_on=0;
    static int F6_on=0;


    if(!gk_USART_RX_QueueEmpty())
    {
       userVal = usart_w_interrupt_getchar();

        if(userVal == '1')//Ch1
        {
	 if(F1_on)//already on -> disable it
	 {
	  function_off(1);
	 }
	 else
	 {
	  function_on(1);
	 }
         F1_on=1-F1_on;
        }
        else if(userVal == '2')//Ch2
        {
         if(F2_on)//already on -> disable it
         {
	  function_off(2);
         }
         else
         {
	  function_on(2);
         }
         F2_on=1-F2_on;
        }

       	else if(userVal == '3')
       	{
	 TIM_Cmd(TIM9,(F3_on)?DISABLE:ENABLE);//Ch3
	 F3_on=1-F3_on;
	}
	else if(userVal == '4')
	{
	 TIM_Cmd(TIM10,(F4_on)?DISABLE:ENABLE);//Ch4
	 F4_on=1-F4_on;
	}
	else if(userVal == '5')
	{
	 GPIO_WriteBit(GPIOA,GPIO_Pin_2,(F5_on)?Bit_RESET:Bit_SET);//switch for Ch1
	 F5_on=1-F5_on;
	}
	else if(userVal == '6')
        {
	 GPIO_WriteBit(GPIOA,GPIO_Pin_3,(F6_on)?Bit_RESET:Bit_SET);//switch for Ch2
         F6_on=1-F6_on;
        }

	else if(userVal == '0')
	{
	 //Turn off external DACs
	 TIM_Cmd(TIM9, DISABLE);
	 TIM_Cmd(TIM10,DISABLE);

	 //Close switches - active low because of Schmidt triggers
	 GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);
	 GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);

	 //Turn off internal DAC
	 function_off(1);
	 function_off(2); 
	
	}
	else if(userVal=='f'||userVal=='F')
	{
	   freq=0;
	   //wait for next char to arrive
	   while(gk_USART_RX_QueueEmpty());
	   functionNum = usart_w_interrupt_getchar();

           while(gk_USART_RX_QueueEmpty());
           freq += (((uint16_t)usart_w_interrupt_getchar())-48)*100;
           while(gk_USART_RX_QueueEmpty());
           freq += (((uint16_t)usart_w_interrupt_getchar())-48)*10;
           while(gk_USART_RX_QueueEmpty());
           freq += ((uint16_t)usart_w_interrupt_getchar())-48;
	
           while(gk_USART_RX_QueueEmpty());
	   Hz_or_kHz= usart_w_interrupt_getchar();
	  
	   if(functionNum == '1' || functionNum == '2')
	   {
		set_internal_DAC_freq(functionNum,freq,Hz_or_kHz);
	   } 
	
	}
	else if(userVal=='s' || userVal=='S')
	{
	   //wait for next char to arrive
           while(gk_USART_RX_QueueEmpty());

           functionNum = usart_w_interrupt_getchar();
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
       //delay only happens when no USART data coming in
       Delay(1); //wait 1ms
    }

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
