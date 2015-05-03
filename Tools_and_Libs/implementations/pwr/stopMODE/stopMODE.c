#include "stopMODE.h"

void stopMODE_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure Button pin as input */
  RCC_AHBPeriphClockCmd(intPIN_CLK, ENABLE);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = intPIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(intBank, &GPIO_InitStructure);

  /* Configure LED to be toggled with button push */
  RCC_AHBPeriphClockCmd(ledPIN_CLK, ENABLE);
 
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin  = ledPIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(ledBank, &GPIO_InitStructure);
  
  /* Enable SYSCONFIG clock: routes EXTI to button line (line 1 in this case)*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(intEXTI_SRC, intEXTI_PIN);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = intEXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  NVIC_InitStructure.NVIC_IRQChannel = intIRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void stop_prepare_and_enter(void)
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_6,Bit_RESET);
   GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_RESET);

   /* Enable Ultra low power mode */
   PWR_UltraLowPowerCmd(ENABLE);

   /* Enter Stop Mode */
   PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

void stop_exit(void)
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_SET);
}

void EXTI0_IRQHandler (void)
{
   if(EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
      //Toggle variable
      sleepENABLED = 1 - sleepENABLED;
      EXTI_ClearITPendingBit(EXTI_Line0);
   }
}

