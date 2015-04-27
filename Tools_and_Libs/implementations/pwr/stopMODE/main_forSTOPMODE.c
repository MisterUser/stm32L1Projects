#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_flash.h>

#include "implementations/stopMODE.h"
 
void Delay(uint32_t nTime);

uint8_t sleepENABLED = 0;


int main(void){

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  
  //Configure Pins
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(& GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Configure SysTick Timer
  //Set System Clock to interrupt every ms.
  if(SysTick_Config(SystemCoreClock / 1000))
	while (1); //If fails, hang in while loop 

  stopMODE_init();

  sleepENABLED = 0;
  GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_SET);

  while (1) {
    static int ledval = 0;

    if(sleepENABLED)
    {
	GPIO_WriteBit(GPIOB, GPIO_Pin_6,Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_RESET);

	 /* Enable Ultra low power mode */
        PWR_UltraLowPowerCmd(ENABLE);

	/* Enter Stop Mode */
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_SET);
    }
    //toggle led
    GPIO_WriteBit(GPIOB,GPIO_Pin_6,(ledval)? Bit_SET : Bit_RESET);
    ledval = 1-ledval;
    
    Delay(250); //wait 250ms
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
