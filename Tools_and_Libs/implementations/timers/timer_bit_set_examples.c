#include "deadBeats_timer.h"

uint8_t  db_user_button_pressed;
uint16_t period_in_ms;

void deadbeats_timer_init()
{
    //1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

        //Enable pin input pins

        // ttc_gpio_init(PIN_PC0,tgm_input_pull_down, tgs_Max);
        // ttc_interrupt_init(tit_GPIO_Rising,PIN_PC0,R_hit_ISR,0,0,0);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
   	GPIO_StructInit(& GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz ;
   	GPIO_Init(GPIOC , & GPIO_InitStructure );

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource0);

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



        //ttc_gpio_init(PIN_PC1,tgm_input_pull_down, tgs_Max);
        //ttc_interrupt_init(tit_GPIO_Rising,PIN_PC1,L_hit_ISR,0,0,0);

	GPIO_InitTypeDef  GPIO_InitStructure;
   	GPIO_StructInit(& GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz ;
   	GPIO_Init(GPIOC , & GPIO_InitStructure );

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource1);

	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;

	EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure . NVIC_IRQChannel = EXTI1_IRQn ;
	NVIC_InitStructure . NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure . NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure . NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Init (& NVIC_InitStructure );

        //Enable PB7 to turn on LED when timer is on
        //ttc_gpio_init(PIN_PB7,tgm_output_push_pull, tgs_Max);


    //2. Fill the TIM_TimeBaseInitStruct with the desired parameters
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

        //Internal High Speed Oscillator = 16,000,000 = 16MHz
        TIM_TimeBaseStructure.TIM_Prescaler = HSI_VALUE/1000; // 16,000,000/1,000 = 16,000 Prescalar=> 1KHz timer-> each tick is 1ms
        TIM_TimeBaseStructure.TIM_Period = 60000; // 0..999
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

        //The CC1S bit in the CCMR1 register must be set for software interrupts
        //((register_stm32l1xx_tim2_to_tim4_input_capture_mode_t*) TIM2)->CCMR1.CC1S = 0b11;
	TIM2->CCMR1 |= ((uint16_t)0x0003);//0b11;

        //start with timer disabled
        TIM_Cmd(TIM2, DISABLE);
        db_user_button_pressed = 0;
}





void start_Timer()
{
    //Enable Timer
    TIM_Cmd(TIM2, ENABLE);

    //turn on LED to show user timer is running
    //ttc_gpio_set(PIN_PB7);

    //Enable interrupts
    //((register_stm32l1xx_exti_t*) EXTI)->IMR.MR0 = 1;
    //((register_stm32l1xx_exti_t*) EXTI)->IMR.MR1 = 1;
    EXTI->IMR |= ((uint16_t)0x0003); //MR0 and MR1
}

void stop_Timer()
{
    //Disable Timer
    TIM_Cmd(TIM2, DISABLE);

    //Led OFF
    //ttc_gpio_clr(PIN_PB7);

    //Disable interrupts
    //((register_stm32l1xx_exti_t*) EXTI)->IMR.MR0 = 0;
    //((register_stm32l1xx_exti_t*) EXTI)->IMR.MR1 = 0;
    EXTI->IMR |= ((uint16_t)0x0003); //MR0 and MR1
}

/** @todo change R_hit_ISR and L_hit_ISR -> EXTI interrupt handlers */
void R_hit_ISR(void)
{
    if(!db_user_button_pressed)
    {
        //capture value into capture register
        ((register_stm32l1xx_tim2_to_tim4_input_capture_mode_t*) TIM2)->EGR.CC1G=1;
        //reset timer
        ((register_stm32l1xx_tim2_to_tim4_input_capture_mode_t*) TIM2)->EGR.UG  =1;
        //read capture register into global
        period_in_ms = TIM_GetCapture1(TIM2);
        //set flag to tell main loop to process captured data
        //db_user_button_pressed = 1;
        //userCurrentHit = 'R';
        putchar('H');
        putchar('I');
        putchar('T');
        putchar(':');
        putchar('R');
        putchar('h');
        putchar(':');
        putchar((char)(((int)'0')+((period_in_ms%100000) /10000)));
        putchar((char)(((int)'0')+((period_in_ms%10000)  /1000)));
        //putchar('.');
        putchar((char)(((int)'0')+((period_in_ms%1000)   /100)));
        putchar((char)(((int)'0')+((period_in_ms%100)    /10)));
        putchar((char)(((int)'0')+ (period_in_ms%10)));
        putchar('#');
        putchar('\r');
    }

}
void L_hit_ISR(void)
{
    if(!db_user_button_pressed)
    {
        //capture value into capture register
        ((register_stm32l1xx_tim2_to_tim4_input_capture_mode_t*) TIM2)->EGR.CC1G=1;
        //reset timer
        ((register_stm32l1xx_tim2_to_tim4_input_capture_mode_t*) TIM2)->EGR.UG  =1;
        //read capture register into global
        period_in_ms = TIM_GetCapture1(TIM2);
        //set flag to tell main loop to process captured data
        //db_user_button_pressed = 1;
        //userCurrentHit = 'L';
        putchar('H');
        putchar('I');
        putchar('T');
        putchar(':');
        putchar('L');
        putchar('S');
        putchar(':');
        putchar((char)(((int)'0')+((period_in_ms%100000) /10000)));
        putchar((char)(((int)'0')+((period_in_ms%10000)  /1000)));
        //putchar('.');
        putchar((char)(((int)'0')+((period_in_ms%1000)   /100)));
        putchar((char)(((int)'0')+((period_in_ms%100)    /10)));
        putchar((char)(((int)'0')+ (period_in_ms%10)));
        putchar('#');
        putchar('\r');

    }

}



