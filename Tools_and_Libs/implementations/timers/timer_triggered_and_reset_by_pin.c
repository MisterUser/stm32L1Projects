/*
 * Timer is triggered by pin -> resets timer and causes input capture
 * 	reset of timer creates interrupt
 * 	TIM2_IRQHandler() is then called, which reads capture register
 */
void inititalization()
{
	//1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//2. Enable pin
		// ---- For pin driven interrupts
		ttc_gpio_init(PIN_PA0, tgm_alternate_function_push_pull, tgs_Max);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource0, GPIO_AF_TIM2);

	//3. Enable interrupts on timer
		NVIC_InitTypeDef NVIC_InitStructure ;
		NVIC_InitStructure . NVIC_IRQChannel = TIM2_IRQn ;
		NVIC_InitStructure . NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure . NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure . NVIC_IRQChannelCmd = ENABLE ;
		NVIC_Init (& NVIC_InitStructure );

	//4. Fill the TIM_TimeBaseInitStruct with the desired parameters
		 //---define and initialize struct---//
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

		//Internal High Speed Oscillator = 16,000,000 = 16MHz
		// 16,000,000/1,000 = 16,000 Prescalar=> 1KHz timer-> each tick is 1ms
		TIM_TimeBaseStructure.TIM_Prescaler = HSI_VALUE/1000;         
		TIM_TimeBaseStructure.TIM_Period = 60000; // 0..999
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;

		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	//5. Fill the TIM_ICInitStruct with the desired parameters

		TIM_ICInitTypeDef TIM_ICInitStructure;
		TIM_ICStructInit(& TIM_ICInitStructure);

		//Channel 1
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = 0;
		TIM_ICInitStructure.TIM_ICFilter = 0;


		TIM_ICInit(TIM2, &TIM_ICInitStructure);

	//6. Make timer 2 slave of input channel 1 so it resets on input (starts the counter)
		TIM_SelectInputTrigger (TIM2 , TIM_TS_TI1FP1);
		TIM_SelectSlaveMode (TIM2 , TIM_SlaveMode_Reset );
		TIM_SelectMasterSlaveMode (TIM2 , TIM_MasterSlaveMode_Enable);

	//7. Enable timer
		TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(){
    period_in_tenth_of_ms = TIM_GetCapture1(TIM2);

    tens_sec        = (char)(((int)'0')+((period_in_tenth_of_ms%100000) /10000));
    one_sec         = (char)(((int)'0')+((period_in_tenth_of_ms%10000)  /1000));
    hundo_ms        = (char)(((int)'0')+((period_in_tenth_of_ms%1000)   /100));
    tens_ms         = (char)(((int)'0')+((period_in_tenth_of_ms%100)    /10));
    one_ms          = (char)(((int)'0')+ (period_in_tenth_of_ms%10));

    putchar(tens_sec);
    putchar(one_sec);
    putchar(',');
    putchar(hundo_ms);
    putchar(tens_ms);
    putchar(one_ms);
    putchar('m');
    putchar('s');
    putchar('\n');

    TIM_ClearITPendingBit (TIM2 , TIM_IT_Trigger );
}
