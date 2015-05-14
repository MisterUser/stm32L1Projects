#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "implementations/sd_driver.h"
#include "implementations/dac_dma_tim.h"
//#include "implementations/stopMODE.h"

//Variables 
#define BUFFERSIZE 512 //for 4MHz
//#define BUFFERSIZE 512  //for 16MHz
//#define BUFFERSIZE 16 
uint8_t readBuf1[BUFFERSIZE];
uint8_t readBuf2[BUFFERSIZE];
__IO uint8_t bufferToSend = 2;
__IO uint8_t buffer_finished = 0;
//__IO uint8_t sleepENABLED = 0;
volatile uint32_t fPTR;

//Declarations

int main(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  
  //Configure Pins
  GPIO_StructInit(& GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_WriteBit(GPIOB, GPIO_Pin_6,Bit_SET);
  //--------------STOP MODE SETUP----------//
 // stopMODE_init();

  //sleepENABLED = 0;
  //GPIO_WriteBit(GPIOB, GPIO_Pin_7,Bit_SET);
  //---------------------------------------//

  //ONLY if not using FatFs
  SD_Init(); 

  fPTR = 0x0210062C; //MGRoL
  //fPTR = 0x02662E50; //Chopin
  //fPTR = 0x02662C00; //8-bit SineWave
  SD_Error readResp=SD_ReadBlock(readBuf1,fPTR,BUFFERSIZE);
  while(readResp != SD_RESPONSE_NO_ERROR)
  {
      readResp=SD_ReadBlock(readBuf1,fPTR,BUFFERSIZE);
  }

  fPTR = fPTR+BUFFERSIZE;
  readResp=SD_ReadBlock(readBuf2,fPTR,BUFFERSIZE);
  while(readResp != SD_RESPONSE_NO_ERROR)
  {
	readResp=SD_ReadBlock(readBuf2,fPTR,BUFFERSIZE);
  }

  bufferToSend = 2;
  buffer_finished = 0;


  //DMA starts with buffer 1 
  DAC_DMA_TIM_init();
  //bufferToSend = 2 at this point, so
  //after buffer 1 is finished, buffer2 will be sent and buffer 1 will be loaded 

  while (1) {
/*
    if(sleepENABLED)
    {
        stop_prepare_and_enter();
        stop_exit();
    } 
*/
    if(buffer_finished != 0)
    {
    
//    	static int ledval = 0;
//    	GPIO_WriteBit(GPIOB,GPIO_Pin_6,(ledval)? Bit_SET : Bit_RESET);
//    	ledval = 1-ledval;

	//iterate fPtr
        fPTR = fPTR +BUFFERSIZE;

	//make sure it's not > 5,644,844 (size of song)
	if(fPTR > 0x0266282F)
	{
	   fPTR = 0x0210062C;
	}
	//choose buffer by reading previously sent buffer
	if(bufferToSend == 2) //2 was just sent
	{
	   //fPTR = 0x02662c00;
	   readResp=SD_ReadBlock(readBuf1,fPTR,BUFFERSIZE);
	   while(readResp != SD_RESPONSE_NO_ERROR)
	   {
        	readResp=SD_ReadBlock(readBuf1,fPTR,BUFFERSIZE);
  	   }

	   bufferToSend = 1;
	}
	else
	{
	   //fPTR = fPTR+BUFFERSIZE;
           readResp=SD_ReadBlock(readBuf2,fPTR,BUFFERSIZE);
           while(readResp != SD_RESPONSE_NO_ERROR)
  	   {
         	readResp=SD_ReadBlock(readBuf2,fPTR,BUFFERSIZE);
	   }

	   bufferToSend = 2;
        }
		
	buffer_finished = 0;

    }
  }
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
#endif
