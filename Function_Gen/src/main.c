#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "implementations/usart_int_and_q.h"
#include "implementations/clock_setup.h"
#include "implementations/hd44780_4bit_lib.h"
#include "implementations/dac_signalGen.h"
#include "implementations/extern_DAC.h"

//-------------Locals-------------//
  //Function States
uint8_t F1_on;
uint8_t F2_on;
uint8_t F3_on;
uint8_t F4_on;
//uint8_t F5_on;
//uint8_t F6_on;

char F1_shape;
char F2_shape;
char F3_shape;
char F4_shape;

uint16_t F1_freq;
uint16_t F2_freq;
uint16_t F3_freq;
uint16_t F4_freq;

char F1_freq_str[3] = "001";
char F2_freq_str[3] = "001";
char F3_freq_str[3] = "001";
char F4_freq_str[3] = "001";

char F1_Hz_or_kHz;
char F2_Hz_or_kHz;
char F3_Hz_or_kHz;
char F4_Hz_or_kHz;

uint8_t Pulse_on;
uint16_t Pulse_freq;
char Pulse_freq_str[3]="001";
char Pulse_H_or_k;
char Pulse_dutyCycle;

static int ledval = 0;

  //USART
volatile char userVal;
volatile char functionNum;
volatile uint16_t freq;
volatile char Hz_or_kHz; 
volatile char functionShape;


//------------Declarations----------//
void Delay(uint32_t nTime);
void print_freq(uint8_t rowNum, char freq_str[], char hz_or_k);
void print_shape(uint8_t rowNum, char shape);
void print_duty(char duty);

int main(void){

  clock_setup(); //clocks now at 16MHz (SYS,HCLK(AHP),PCLK1(APB1),PCLK2(APB2))

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOA, ENABLE); 
  
  //Configure Pins
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

/* Not needed in implementation
  //Control pins for DAC Ch1&Ch2 switches
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/

  usart_int_and_q_init();
  
  Delay(150); //LCD has to wait 150ms after power on

  hd44780_init(N_2LINE,FONT_8);
  
  external_DAC_setup(); 

  DAC_signalGen_init();

  //all functions start off
  F1_on=0;
  F2_on=0;
  F3_on=0;
  F4_on=0;
  //F5_on=0;
  //F6_on=0;
  
  F1_shape='S';
  F2_shape='S';
  F3_shape='S';
  F4_shape='S';
  
  F1_freq=1;
  F2_freq=1;
  F3_freq=1;
  F4_freq=1;
  
  F1_Hz_or_kHz='k';
  F2_Hz_or_kHz='k';
  F3_Hz_or_kHz='k';
  F4_Hz_or_kHz='k';

  Pulse_on=0;
  Pulse_freq=100;
  Pulse_H_or_k='k';
  Pulse_dutyCycle='5';


  //initialize all DAC GPIOs to LOW
  ExtDAC_resetPin = 0x0F << ExtDAC1_PortOffset;
  ExtDAC_resetPin = 0x0F << ExtDAC2_PortOffset;
  Pulse_resetPin = Pulse_mask;

/*
  //switches HIGH -> OFF
  GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);
  GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
*/

  while (1) {


    if(!gk_USART_RX_QueueEmpty())
    {
       userVal = usart_w_interrupt_getchar();

        if(userVal == '1')//Ch1
        {
	 if(F1_on)//already on -> disable it
	 {
	  function_off(1);
	  hd44780_setCursorPosition(0,0);
          hd44780_write_string("-F1:OFF             ");
	 }
	 else
	 {
	  function_on(1);
	  hd44780_setCursorPosition(0,0);
	  hd44780_write_string("+F1: ");
	  print_shape(1,F1_shape);
	  hd44780_write_string(" | ");
	  print_freq(1,F1_freq_str,F1_Hz_or_kHz);	
	 }
         F1_on=1-F1_on;
        }
        else if(userVal == '2')//Ch2
        {
         if(F2_on)//already on -> disable it
         {
	  function_off(2);
	  hd44780_setCursorPosition(1,0);
   	  hd44780_write_string("-F2:OFF             ");
         }
         else
         {
	  function_on(2);
	  hd44780_setCursorPosition(1,0);
          hd44780_write_string("+F2: ");
          print_shape(2,F2_shape);
          hd44780_write_string(" | ");
          print_freq(2,F2_freq_str,F2_Hz_or_kHz);
         }
         F2_on=1-F2_on;
        }

       	else if(userVal == '3')//Ch3
       	{
	 if(F3_on)
	 {
	    TIM_Cmd(TIM9,DISABLE);
   	    ExtDAC_resetPin = 0x0F << ExtDAC1_PortOffset;
	    hd44780_setCursorPosition(2,3);
   	    hd44780_write_string("OFF     ");
	    GPIO_WriteBit(GPIOC,GPIO_Pin_0,Bit_RESET);
	    GPIO_WriteBit(GPIOC,GPIO_Pin_1,Bit_RESET);
	    GPIO_WriteBit(GPIOC,GPIO_Pin_2,Bit_RESET);
	    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET);
	 }
	 else
	 {
	   TIM_Cmd(TIM9,ENABLE);
           print_shape(3,F3_shape);
           print_freq(3,F3_freq_str,F3_Hz_or_kHz);
	 }
	 F3_on=1-F3_on;
	}
	else if(userVal == '4')//Ch4
	{
	 if(F4_on)
	 {
	   TIM_Cmd(TIM10,DISABLE);
   	   ExtDAC_resetPin = 0x0F << ExtDAC2_PortOffset;
	   hd44780_setCursorPosition(2,14);
           hd44780_write_string("OFF   ");
	   GPIO_WriteBit(GPIOC,GPIO_Pin_6,Bit_RESET);
	   GPIO_WriteBit(GPIOC,GPIO_Pin_7,Bit_RESET);
	   GPIO_WriteBit(GPIOC,GPIO_Pin_8,Bit_RESET);
	   GPIO_WriteBit(GPIOC,GPIO_Pin_9,Bit_RESET);
	 }
	 else
	 {
	   TIM_Cmd(TIM10,ENABLE);
           print_shape(4,F4_shape);
           print_freq(4,F4_freq_str,F4_Hz_or_kHz);
	 }
	 F4_on=1-F4_on;
	}
/*
	else if(userVal == '5')//switch for Ch1 
	{
	 if(F5_on)//already ON, turn it off
	 {
	   GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);//high to Schmitt trig -> inverted to LOW
	   hd44780_setCursorPosition(0,0);
           hd44780_write_string("-");
	 }
	 else
	 {
	   GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_RESET);//LOW to Schmitt trig -> inverted to HIGH ->NMOS ON
           hd44780_setCursorPosition(0,0);
           hd44780_write_string("+");
	 }
	 F5_on=1-F5_on;
	}
	else if(userVal == '6')//switch for Ch2
        {
	 if(F6_on)
	 {
	   GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);//HIGH->Schmitt->LOW->NMOS OFF
	   hd44780_setCursorPosition(1,0);
           hd44780_write_char('-');
	 }
	 else 
	 {
	   GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);//LOW->Schmitt->HIGH->NMOS OFF
	   hd44780_setCursorPosition(1,0);
           hd44780_write_char('+');
	 }
         F6_on=1-F6_on;
        }
*/
	else if(userVal == 'p'||userVal == 'P')//Pulse
        {
         if(Pulse_on)
         {
           TIM_Cmd(TIM11,DISABLE);
           hd44780_setCursorPosition(3,6);
           hd44780_write_string("OFF           ");
	   Pulse_resetPin = Pulse_mask;
         }
         else
         {
           TIM_Cmd(TIM11,ENABLE);
	   print_shape(7,'P');
	   hd44780_setCursorPosition(3,8);
	   hd44780_write_char(' ');
           print_freq(7,Pulse_freq_str,Pulse_H_or_k);
	   print_duty(Pulse_dutyCycle);
         }
         Pulse_on=1-Pulse_on;
        }
	else if(userVal == '0')
	{
	 //Turn off external DACs
	 TIM_Cmd(TIM9, DISABLE);
	 TIM_Cmd(TIM10,DISABLE);
	 //write all external DAC pins low
   	 ExtDAC_resetPin = 0x0F << ExtDAC1_PortOffset;
   	 ExtDAC_resetPin = 0x0F << ExtDAC2_PortOffset;

	 //Turn off Pulse
	 TIM_Cmd(TIM11,DISABLE);
	 Pulse_resetPin = Pulse_mask;

/*
	 //Close switches - active low because of Schmidt triggers
	 GPIO_WriteBit(GPIOA,GPIO_Pin_2,Bit_SET);
	 GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
*/

	 //Turn off internal DAC
	 function_off(1);
	 function_off(2); 

	 lcd_reset_screen();

	 F1_on=0;
	 F2_on=0;
	 F3_on=0;
	 F4_on=0;
	 //F5_on=0;
	 //F6_on=0;
	 Pulse_on=0;
	}
	else if(userVal=='f'||userVal=='F')
	{
	   freq=0;
	   char temp_freq_bin[3];
	   //wait for next char to arrive
	   while(gk_USART_RX_QueueEmpty());
	   functionNum = usart_w_interrupt_getchar();

           while(gk_USART_RX_QueueEmpty());
	   temp_freq_bin[0]=usart_w_interrupt_getchar();
           freq += (((uint16_t)temp_freq_bin[0])-48)*100;

           while(gk_USART_RX_QueueEmpty());
	   temp_freq_bin[1]=usart_w_interrupt_getchar();
           freq += (((uint16_t)temp_freq_bin[1])-48)*10;

           while(gk_USART_RX_QueueEmpty());
	   temp_freq_bin[2]=usart_w_interrupt_getchar();
           freq += ((uint16_t)temp_freq_bin[2])-48;
	
           while(gk_USART_RX_QueueEmpty());
	   Hz_or_kHz= usart_w_interrupt_getchar();
	 
	   int temp_freq_bin_iter = 0; 

	   switch(functionNum)
	   {
 		case '1': 
		  F1_freq=freq;
		  F1_Hz_or_kHz=Hz_or_kHz;
		  for(;temp_freq_bin_iter<3;temp_freq_bin_iter++)
		  { 
		    F1_freq_str[temp_freq_bin_iter]=temp_freq_bin[temp_freq_bin_iter];
		  }	          
		  set_internal_DAC_freq(functionNum,freq,Hz_or_kHz);
		break;
		case '2':
		  F2_freq=freq;
		  F2_Hz_or_kHz=Hz_or_kHz;
		  for(;temp_freq_bin_iter<3;temp_freq_bin_iter++)
		  { 
		    F2_freq_str[temp_freq_bin_iter]=temp_freq_bin[temp_freq_bin_iter];
		  }	          
		  set_internal_DAC_freq(functionNum,freq,Hz_or_kHz);
		break;
	   	case '3':
		  F3_freq=freq;
		  F3_Hz_or_kHz=Hz_or_kHz;
		  for(;temp_freq_bin_iter<3;temp_freq_bin_iter++)
		  { 
		    F3_freq_str[temp_freq_bin_iter]=temp_freq_bin[temp_freq_bin_iter];
		  }	          
		  set_external_DAC_freq(functionNum,freq,Hz_or_kHz);
		break;
		case '4':
		  F4_freq=freq;
		  F4_Hz_or_kHz=Hz_or_kHz;
		  for(;temp_freq_bin_iter<3;temp_freq_bin_iter++)
		  { 
		    F4_freq_str[temp_freq_bin_iter]=temp_freq_bin[temp_freq_bin_iter];
		  }	          
		  set_external_DAC_freq(functionNum,freq,Hz_or_kHz);
		break;
		case 'p':
	  	case 'P':
		  Pulse_freq=freq;
		  Pulse_H_or_k=Hz_or_kHz;
		  for(;temp_freq_bin_iter<3;temp_freq_bin_iter++)
		  { 
		    Pulse_freq_str[temp_freq_bin_iter]=temp_freq_bin[temp_freq_bin_iter];
		  }	          
		  set_pulse_freq(freq,Hz_or_kHz);
		break;
		default:
		break;
	   }
	      print_freq(((uint8_t)functionNum)-48, temp_freq_bin, Hz_or_kHz);//-48 to convert to function num and -1 for row number
	
	}
	else if(userVal=='s' || userVal=='S')
	{
           while(gk_USART_RX_QueueEmpty());
           functionNum = usart_w_interrupt_getchar();

	   while(gk_USART_RX_QueueEmpty());
	   functionShape=usart_w_interrupt_getchar();   

           switch(functionNum)
	   {
		case '1':
	   	   F1_shape=functionShape;	
		   set_internal_DAC_shape(functionNum,functionShape);
		break;
		case '2':
	   	   F2_shape=functionShape;	
		   set_internal_DAC_shape(functionNum,functionShape);
		break;
	/*	case '3':
	   	   F3_shape=functionShape;	
		   set_external_DAC_shape(functionNum,functionShape);
		break;
		case '4':
	   	   F4_shape=functionShape;	
		   set_external_DAC_shape(functionNum,functionShape);
		break;
	*/
		default:
		return;
	   }
	   print_shape(((uint8_t)functionNum)-48, functionShape);
	}
	else if(userVal=='d' || userVal=='D')
        {
           while(gk_USART_RX_QueueEmpty());
           Pulse_dutyCycle=usart_w_interrupt_getchar();
	   set_pulse_duty_cycle(Pulse_dutyCycle);   
	   if(Pulse_on){print_duty(Pulse_dutyCycle);}
        }
	else if(userVal=='l' || userVal=='L')//use this option if screen powers down
	{
	   lcd_setup(N_2LINE,FONT_8);
	   lcd_reset_screen();
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

void print_freq(uint8_t funcNum, char  freq_str[], char hz_or_k)
{
    switch(funcNum)
    {
        case 1:
        case 2:
          hd44780_setCursorPosition(funcNum-1,10);
        break;
        case 3:
          hd44780_setCursorPosition(2,5);
        break;
        case 4:
          hd44780_setCursorPosition(2,16);
        break;
        case 7:
          hd44780_setCursorPosition(3,9);
        break;
        default:
        break;
    }

    int fsiter=0;
    for(;fsiter<3;fsiter++)
    {
       hd44780_write_char(freq_str[fsiter]);
    }

    if(funcNum == 3 || funcNum == 4){

       if(hz_or_k=='k'){hd44780_write_char(hz_or_k);}
       else{hd44780_write_char('H');}
       return;
    }

    if(hz_or_k=='k'){hd44780_write_char(hz_or_k);}
    else{hd44780_write_char(' ');}
    hd44780_write_string("Hz ");
}

void print_shape(uint8_t funcNum, char shape)
{
   switch(funcNum)
   {
	case 1:
	case 2:
	  hd44780_setCursorPosition(funcNum-1,5);
	break;
	case 3:
	  hd44780_setCursorPosition(2,3);
	break;
	case 4:
	  hd44780_setCursorPosition(2,14);
	break;
	case 7:
	  hd44780_setCursorPosition(3,6);
        break;  
	default:
	break;
   }
   

   if(shape == 'S'|| shape=='s')
   {
      hd44780_write_char(0x00);
      hd44780_write_char(0x01);
   }
   else if(shape == 'T' || shape=='t')
   {
      hd44780_write_char(0x02);
      hd44780_write_char(0x03);
   }
   else if(shape == 'E' || shape=='e')
   {
      hd44780_write_char(0x02);
      hd44780_write_char(0x04);
   }
   else if(shape == 'R' || shape=='r')
   {
      hd44780_write_char(0x05);
      hd44780_write_char(0x03);
   }
   else if(shape == 'P' || shape == 'p')
   {
      hd44780_write_char(0x06);
      hd44780_write_char(0x07);
   }
}

void print_duty(char duty)
{
   hd44780_setCursorPosition(3,16);
   hd44780_write_char(duty);
   hd44780_write_char('0');
   hd44780_write_char('%');  
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
