#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_usart.h>
#include <stm32l1xx_tim.h>
#include <stm32l1xx_syscfg.h>
#include <stm32l1xx_exti.h>
#include <misc.h>


void Delay(uint32_t nTime);
int putchar (uint16_t c);
uint16_t getchar (void);






int main(void){

   
   
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOA, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SYSCFG, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
   

   //leds
   GPIO_InitTypeDef  GPIO_InitStructure;
   GPIO_StructInit(& GPIO_InitStructure );
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB , & GPIO_InitStructure );
   
   GPIO_StructInit (& GPIO_InitStructure );
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
   GPIO_Init(GPIOB , & GPIO_InitStructure );
   
   //Timer4 for LEDs
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
   TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
   
   TIM_TimeBaseStructInit(& TIM_TimeBaseStructure );
   TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock /1000000 - 1; 	// 0..239
   TIM_TimeBaseStructure.TIM_Period = 1000 - 1; 			// 0..999
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
   TIM_TimeBaseInit(TIM4 , & TIM_TimeBaseStructure );
   
   TIM_OCInitTypeDef TIM_OCInitStructure ;
   
   TIM_OCStructInit(& TIM_OCInitStructure );
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1 ;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable ;
   TIM_OC1Init(TIM4 , & TIM_OCInitStructure );


   TIM_Cmd(TIM4 , ENABLE );
   
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
   TIM_SetCompare1(TIM4, 10); //capture/compare reg -> 1% duty cycle

   
   

   //pushbutton
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init (GPIOA , & GPIO_InitStructure );
   
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
   
   EXTI_InitTypeDef EXTI_InitStructure;
   EXTI_InitStructure.EXTI_Line = EXTI_Line0;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising;
   EXTI_InitStructure.EXTI_LineCmd=ENABLE;
   
   EXTI_Init(&EXTI_InitStructure);
   
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_InitStructure . NVIC_IRQChannel = EXTI0_IRQn ;
   NVIC_InitStructure . NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure . NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure . NVIC_IRQChannelCmd = ENABLE ;
   NVIC_Init (& NVIC_InitStructure );

   
   
   //GPIO info for TX of USART1
   
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
   
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init (GPIOA , & GPIO_InitStructure );
   
   
   
   //info for RX of USART1
   
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init (GPIOA , & GPIO_InitStructure );
   
   USART_DeInit(USART1);
   //Initialize the USART
   USART_InitTypeDef USART_InitStructure ;
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.USART_BaudRate   = 9600;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity	= USART_Parity_No;
   USART_InitStructure.USART_Mode 	= USART_Mode_Rx | USART_Mode_Tx;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_Init(USART1, &USART_InitStructure);
   USART_Cmd(USART1 , ENABLE );

   
   
 
   //Set System Clock to interrupt every ms.
   if ( SysTick_Config ( SystemCoreClock / 1000) )
	while (1); //If fails, hang in while loop

   
   //static int ledCnt 0 ;
   static uint32_t compareVal = 10;
   static uint16_t userVal = 'U';
   putchar('H');
   putchar('e');
   putchar('l');
   putchar('l');
   putchar('o');
   putchar('\n');
  
   
   while (1) {
	
	
	//---------------GPIOs----------------//
	//Toggle blue LED - not used when timer is being used
	//GPIO_WriteBit(GPIOB , GPIO_Pin_6 , ( ledval ) ? Bit_SET : Bit_RESET );
	//Toggle variable
        //ledval = 1 - ledval ;
        
        //Timer LED
        if(compareVal < 900)
        	compareVal += 100;
        else
        	compareVal = 0;
        
        TIM_SetCompare1(TIM4, compareVal); //capture/compare reg -> 50% duty cycle

        //check if pushbutton is pressed
	/*if(GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_0 )){
		//if so, turn green LED on*/
		GPIO_WriteBit(GPIOB , GPIO_Pin_7, Bit_SET);
	/*	
	}
	else
	{
		//if not, turn green LED off
		GPIO_WriteBit(GPIOB , GPIO_Pin_7, Bit_RESET);
	}
	*/
	//-----------------------------------//
	
	
	//---------test USART----------------//
	userVal = getchar();
	switch(userVal){
		case 'U': 
		putchar('U');
		putchar('n');
		putchar('i');
		putchar('v');
		putchar('e');
		putchar('r');
		putchar('s');
		putchar('a');
		putchar('l');
		putchar('\n');
		break;
		case 'S': 
		putchar('S');
		putchar('y');
		putchar('n');
		putchar('c');
		putchar('h');
		putchar('r');
		putchar('o');
		putchar('n');
		putchar('o');
		putchar('u');
		putchar('s');
		putchar('\n');
		break;
		case 'A': 
		putchar('A');
		putchar('s');
		putchar('y');
		putchar('n');
		putchar('c');
		putchar('h');
		putchar('r');
		putchar('o');
		putchar('n');
		putchar('o');
		putchar('u');
		putchar('s');
		putchar('\n');
		break;
		case 'R': 
		putchar('R');
		putchar('e');
		putchar('c');
		putchar('e');
		putchar('i');
		putchar('v');
		putchar('e');
		putchar('r');
		putchar('\n');
		break;
		
		case 'T': 
		putchar('T');
		putchar('r');
		putchar('a');
		putchar('n');
		putchar('s');
		putchar('m');
		putchar('i');
		putchar('t');
		putchar('t');
		putchar('e');
		putchar('r');
		putchar('\n');
		break;
		
		default: 
		putchar('F');
		putchar('o');
		putchar('o');
		putchar('b');
		putchar('a');
		putchar('r');
		putchar('\n');
		break;
	}
	//-----------------------------------//
	
	Delay(150);	
   }




}

static __IO uint32_t TimingDelay;
static int ledval = 0;

void Delay(uint32_t delayMS){
   TimingDelay = delayMS;
   while ( TimingDelay != 0);
}

void SysTick_Handler (void){
if ( TimingDelay != 0x00)
TimingDelay --;
}

int putchar (uint16_t c){
while ( USART_GetFlagStatus(USART1 , USART_FLAG_TXE ) == RESET);

//USART1->DR = (c & 0xff);
USART_SendData(USART1, c);



return 0;
}

uint16_t getchar (void){
while ( USART_GetFlagStatus (USART1 , USART_FLAG_RXNE ) == RESET);

//return USART1->DR & 0xff;
return USART_ReceiveData(USART1);
}


void EXTI0_IRQHandler (void)
{ 
	if ( EXTI_GetITStatus ( EXTI_Line0 ) !=RESET)
	{ 
		GPIO_WriteBit(GPIOB , GPIO_Pin_7 , ( ledval ) ? Bit_SET : Bit_RESET );
		//Toggle variable
		ledval = 1 - ledval ;
		TIM_SetCompare1(TIM4, 0);
		EXTI_ClearITPendingBit ( EXTI_Line0 ); 
	} 
}


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}

#endif
