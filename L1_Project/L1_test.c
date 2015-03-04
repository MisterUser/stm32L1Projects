#include "L1_test.h"

void blinkLED(void)
{
   //ttc_gpio_init(GPIO_BANK_B,7,tgm_output_push_pull);
   //ttc_gpio_init(GPIO_BANK_B,6,tgm_output_push_pull);

   ttc_gpio_init(TTC_SWITCH1,tgm_input_floating);
   

   /*static int ledval = 0;

    if(ledval){

        ttc_gpio_clr(PIN_PB6);

    }

    else

    {

        ttc_gpio_set(PIN_PB6);

    }



    ledval = 1 - ledval ;
    */


    //Test GPIO - pushbutton and LED

    if(ttc_gpio_get(TTC_SWITCH1)){

        ttc_gpio_set(PIN_PB7);

    }

    else

    {

        ttc_gpio_clr(PIN_PB7);

    }  



    //-------------Test GPTimers, interrupts, and other LED------------------//
    //TIM4 - OC and PWM on LED2 on PB7
    if(0){
    //1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);


    //Test Hardware Timer 2 (TIM2) - using standard peripheral lib
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_StructInit(& GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
    GPIO_Init(GPIOB , & GPIO_InitStructure );
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7, GPIO_AF_TIM4);


    //2. Fill the TIM_TimeBaseInitStruct with the desired parameters
         //---define and initialize struct---//
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

         //---fill the struct---//
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock /100000 - 1; // 0..239
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; // 0..999
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;

    //3. Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
    //with the corresponding configuration
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    //4. For PWM, initialize
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);

    //5. Enable Timer
    TIM_Cmd(TIM4, ENABLE);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    //6. Set duty cycle
    TIM_SetCompare2(TIM4, 10); //capture/compare reg -> 1% duty cycle
    //ttc_hardware_timer_set_isr(TTC_TIMER2, ChangeHighLED1, (void *) &LED1, 100, 1);
    int compareVal = 999;
    while (0){
        if(compareVal > 0){
                compareVal = 0;
                mSleep(1000);
        }
        else{
                compareVal = 999;
                mSleep(100);
        }

        TIM_SetCompare2(TIM4, compareVal); //capture/compare reg -> 50% duty cycle
    }

    //}//End Test TIM4 LED PWM

    //TIM2 and TIM3 - Proximity using OC and PWM as well as IC
    //if(1){
        //1. Enable TIM clock using RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

        //2. Set up TIM2 Ch 2 for 10us trigger pulse at 10Hz
            //A---TIM2 CH2 is on PA1
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

            //GPIO_InitTypeDef  GPIO_InitStructure;
            GPIO_StructInit(& GPIO_InitStructure );
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

            GPIO_Init(GPIOA , & GPIO_InitStructure );
            GPIO_PinAFConfig(GPIOA,GPIO_PinSource1, GPIO_AF_TIM2);


            //B--- Fill the TIM_TimeBaseInitStruct with the desired parameters
                 //---define and initialize struct---//
            //TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
            TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

                 //---fill the struct---//
            TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock /200000 - 1; // 0..999
            TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 0..9999
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;

            //C---- Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
            //with the corresponding configuration
            TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


            //D--- For PWM, initialize
            //TIM_OCInitTypeDef TIM_OCInitStructure;
            TIM_OCStructInit(&TIM_OCInitStructure);

            TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
            TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

            TIM_OC2Init(TIM2, &TIM_OCInitStructure);

            //E---- Enable Timer
            TIM_Cmd(TIM2, ENABLE);
            TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
            //F--- Set duty cycle
            TIM_SetCompare2(TIM2, 1); //capture/compare reg -> .01% duty cycle (.0001 * 100000us = 10us)

        //3. Set up TIM3 CH1 and CH2 for input capture of echo signal
            //A---TIM3 CH1 is connected to PA6 and CH2 to PA7

            GPIO_StructInit(& GPIO_InitStructure );
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

            GPIO_Init(GPIOB , & GPIO_InitStructure );
            GPIO_PinAFConfig(GPIOB,GPIO_PinSource4, GPIO_AF_TIM3);

            GPIO_StructInit(& GPIO_InitStructure );
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

            GPIO_Init(GPIOB , & GPIO_InitStructure );
            GPIO_PinAFConfig(GPIOB,GPIO_PinSource5, GPIO_AF_TIM3);

            //B--- Fill the TIM_TimeBaseInitStruct with the desired parameters
                 //---define and initialize struct---//

            TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

                 //---fill the struct---//
            TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock /200000 - 1; // 0..239
            TIM_TimeBaseStructure.TIM_Period = 10000 - 1; // 0..9999
            TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

            //C---- Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure the Time Base unit
            //with the corresponding configuration
            TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

            //D---Fill the TIM_ICInitStruct with the desired parameters
            TIM_ICInitTypeDef TIM_ICInitStructure;
            TIM_ICStructInit(& TIM_ICInitStructure);
                //Channel 1
            TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
            TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
            TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
            TIM_ICInitStructure.TIM_ICPrescaler = 0;
            TIM_ICInitStructure.TIM_ICFilter = 0;

            TIM_ICInit(TIM3, & TIM_ICInitStructure);

                //Channel 2
            TIM_ICStructInit(& TIM_ICInitStructure);

            TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
            TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
            TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
            TIM_ICInitStructure.TIM_ICPrescaler = 0;
            TIM_ICInitStructure.TIM_ICFilter = 0;

            TIM_ICInit(TIM3, & TIM_ICInitStructure);

               //put channel 2 as slave of channel 1
            //TIM_SelectInputTrigger (TIM3 , TIM_TS_TI1FP1 );
            //TIM_SelectSlaveMode (TIM3 , TIM_SlaveMode_Reset );
            //TIM_SelectMasterSlaveMode (TIM3 , TIM_MasterSlaveMode_Enable );

            //E---- Enable Timer
            TIM_Cmd(TIM3, ENABLE);

            uint32_t risingEdge_of_Echo;
            uint32_t fallingEdge_of_Echo;
            uint32_t echo_PW;
            while(1){
                risingEdge_of_Echo = TIM_GetCapture1(TIM3);
                fallingEdge_of_Echo = TIM_GetCapture2(TIM3);

                echo_PW = risingEdge_of_Echo - fallingEdge_of_Echo;

                if(echo_PW > 200)
                    TIM_SetCompare2(TIM4, 10); //capture/compare reg -> 1% duty cycle

                else if(echo_PW >150 && echo_PW <=200)
                    TIM_SetCompare2(TIM4, 100); //capture/compare reg -> 1% duty cycle

                else if(echo_PW >100 && echo_PW <=150)
                    TIM_SetCompare2(TIM4, 100); //capture/compare reg -> 1% duty cycle

                else if(echo_PW >50 && echo_PW <=100)
                    TIM_SetCompare2(TIM4, 500); //capture/compare reg -> 1% duty cycle

                else if(echo_PW >15 && echo_PW <=50)
                    TIM_SetCompare2(TIM4, 900); //capture/compare reg -> 1% duty cycle

            }

    }//END Proximity Sensor Test



  //---------------------------Test DAC------------------------------------------//
    if(0){
    //-----------test DAC---------------//
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    //Channel 1 uses PA4, so have to initialize this pin for analog in
    ttc_gpio_init(GPIO_BANK_A,4,tgm_analog_in);
    /*RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_StructInit(& GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);*/


    //Initialize DAC Channel 1
    DAC_InitTypeDef DAC_InitStructure;
    DAC_StructInit(&DAC_InitStructure);

    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_2047;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // Enable DAC
    DAC_WaveGenerationCmd(DAC_Channel_1, DAC_Wave_Triangle, ENABLE);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    //Write to DAC
    //DAC_SetChannel1Data(align, value); //align = {DAC_Align_12b_R, DAC_Align_12b_L, DAC_Align_8b_R}
    //This should output a triangular wave
    /*while(1){
        for (int i = 0; i <50; i++)
        {
            DAC_SetChannel1Data(DAC_Align_12b_R, (i * 10) + 512);
        }
        for (int i = 49; i >= 0; i--)
        {
            DAC_SetChannel1Data(DAC_Align_12b_R, (i * 10) + 512);
        }
    }*/
    }//END Test DAC

    //------------------Test USART-------------//
    if(1){
        ttc_usart_prepare();

        ttc_usart_generic_t* UsartConfig = ttc_usart_get_configuration(USART1_INDEX);

        UsartConfig->LogicalIndex = 1;
        UsartConfig->BaudRate = 9600;
        UsartConfig->WordLength = 8;
        UsartConfig->HalfStopBits = 2;
        UsartConfig->Flags.Bits.TransmitParity = 0;
        UsartConfig->Flags.Bits.Cts = 0;
        UsartConfig->Flags.Bits.Rts = 0;

        ttc_usart_init(USART1_INDEX);

        static uint16_t userVal = 'U';
           putchar('H');
           putchar('e');
           putchar('l');
           putchar('l');
           putchar('o');
           putchar('\n');

        while (1) {



             //test USART
         if(1){
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
         }


        }

    }//END Test USART
}




//------private functions--------//


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








//------------ISRs-------------------------------------//
void ChangeHighLED1(void *TaskArgument){
    (void) TaskArgument;   //To Avoid Warnings

   //Drive LED High
    ttc_gpio_set(PIN_PB6);

   //Test Hardware Timer 3 (TIM3)
   //ttc_hardware_timer_set_isr(TTC_TIMER3, ChangeLowLED1, (void *) &LED1, 50, 1);
}

void ChangeLowLED1(void *TaskArgument){
    (void) TaskArgument;    //To Avoid Warnings

    ttc_gpio_clr(PIN_PB6);
    //ttc_hardware_timer_set_isr(TTC_TIMER2, ChangeHighLED1, (void *) &LED1, 100, 1);
}
