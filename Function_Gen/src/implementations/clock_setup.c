#include "clock_setup.h"

void clock_setup()
{
  //-----------------------System Clock Sources---------------------------//
  // Enable HSI Clock 
  RCC_HSICmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET); //! Wait till HSI is ready

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);//SYSCLK now at 16MHz



  //PLL
  //RCC_PLLCmd(DISABLE);
  //while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET ){}; //! Wait until off

  //Turn the PLL on for MCO!!! 
  RCC_PLLConfig(RCC_PLLSource_HSI,RCC_PLLMul_3,RCC_PLLDiv_4);//16*3/4 = 12MHz

  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET ){};

  //RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);



  //Turn HSE off
  RCC_HSEConfig(RCC_HSE_OFF);
  while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET ){}; //! Wait until off

  //Turn LSE off
  RCC_LSEConfig(RCC_LSE_OFF);
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != RESET ){}; //! Wait until off

  //Turn LSI off
  RCC_LSICmd(DISABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != RESET ){}; //! Wait until off

  //Turn MSI off
  RCC_MSICmd(DISABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_MSIRDY) != RESET ){}; //! Wait until off




  //-----------------------Peripheral Clock Sources----------------------//
 

  //AHB (HCLK, AHB bus(GPIOs,CRC,RCC,Flash), core, memory, and DMA, FCLK)
  //Cortex System timer = AHB/8
  RCC_HCLKConfig(RCC_SYSCLK_Div1); //AHB = 16MHz -> [DMA][GPIO]

  //APB clocks are based on AHB

  //APB1 -> PCLK1 to APB1 Peripherals (including TIM2-7)
  //Peripherals: TIM2-7,LCD,RTC,WWDG,IWDG,SPI2,USART2,USART3,I2C1,I2C2,PWR,DAC,COMP,RI
  //if APB1 prescalar = 1, TIM2-7 clocks = APB1
  //if APB1 prescalar > 1, TIM2-7 clocks = APB1*2
  RCC_PCLK1Config(RCC_HCLK_Div1);  //APB1 (low sleed) = 16MHz [DAC]
                                  //      [TIMERS 2-7]: 16MHz*1 = 16MHz
                                  // above may not be true, because TIM2 = 2MHz

  //APB2 -> PCLK2 to APB2 Peripherals (including TIM9-11)
  //Peripherals: SysCFG, EXTI,TIM9-11,ADC1,SPI1,USART1
  //if APB2 prescalar = 1, TIM9-11 clocks = APB2
  //if APB2 prescalar > 1, TIM9-11 clocks = APB2*2
  RCC_PCLK2Config(RCC_HCLK_Div1);  //APB2 (high speed) = 16MHz [SPI]

  //RCC_ClocksTypeDef RCC_ClockFreq;
  //RCC_GetClocksFreq(&RCC_ClockFreq);







  //----------------------------SYSTICK-------------------------//
  //Configure SysTick Timer (in core_cm4.h)
  //Set System Clock to interrupt every ms. HCLK/8 = 2MHz. Each tick is 500ns
  //SysTick_Config takes number of ticks before an interrupt
  //1ms=1000us, each tick = .5us, so need 2000 ticks
  if(SysTick_Config(2000)) //This is used to wait for UART (human) input, so c
        while (1); //If fails, hang in while loop







  //------------------------MCO--------------------------------//
  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  //MCO for UART converter 12Mhz?
  // It works!
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  GPIO_WriteBit(GPIOA,GPIO_Pin_8,Bit_RESET);

  RCC_MCOConfig(RCC_MCOSource_PLLCLK,RCC_MCODiv_1); //12MHz
}
