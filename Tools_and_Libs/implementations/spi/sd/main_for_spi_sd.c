#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
//#include "implementations/ff.h"
#include "implementations/sd_driver.h"
#include "implementations/dac_dma_tim.h"

//Variables 
#define BUFFERSIZE 250 
uint8_t readBuf1[BUFFERSIZE];
uint8_t readBuf2[BUFFERSIZE];
__IO uint8_t bufferToSend = 2;
__IO uint8_t buffer_finished = 0;

//Declarations
void Delay(uint32_t nTime);
//void gk_printChar(uint8_t pchar);

int main(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  //Enable Peripheral Clocks
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
  
  //Configure Pins
  GPIO_StructInit(& GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
  GPIO_Init(GPIOB , & GPIO_InitStructure );

  //usart_init();

  //Configure SysTick Timer
  //Set System Clock to interrupt every ms.
  //if(SysTick_Config(SystemCoreClock / 32000))
//	while (1); //If fails, hang in while loop 

  //char userVal;

/*
  FATFS FatFs;   // Work area (file system object) for logical drive 
  FIL fp;
  
  // Register work area to the default drive 
  if(f_mount(&FatFs, "", 0) == FR_OK)
  {

     unsigned char read_buf[20];
     FRESULT openResult;
     openResult = f_open(&fp,"test",FA_READ);
     if(!openResult)
     {
        FRESULT readResult;
	UINT* num_bytes_read;
	readResult = f_read(&fp,read_buf,10,num_bytes_read);
	if(!readResult)
	{
	   int i = 0;
	   for(;i<*num_bytes_read;i++)
	   {
		usart_w_interrupt_getchar(read_buf[i]);
	   }
	}
	// Close the file //
    	f_close(&fp);
     }

     f_mount(0,"",0); //unmount
  }
*/

/*
  SD_CardInfo* cardinfo;
  SD_Error poo = SD_GetCardInfo(cardinfo); 
  if(poo == SD_RESPONSE_NO_ERROR)
  {
  uint32_t sd_size = cardinfo->SD_csd.DeviceSize;
  uint8_t  sd_format = cardinfo->SD_csd.FileFormat;
  uint8_t  sd_manuf = cardinfo->SD_cid.ManufacturerID;
  uint16_t sd_oem = cardinfo->SD_cid.OEM_AppliID; 
  uint32_t sd_name = cardinfo->SD_cid.ProdName1; 
  }
*/
  
  //ONLY if not using FatFs
  SD_Init(); 

  uint32_t fPTR = 0x0210062C;
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


    if(buffer_finished != 0)
    {
    
    	static int ledval = 0;
    	GPIO_WriteBit(GPIOB,GPIO_Pin_7,(ledval)? Bit_SET : Bit_RESET);
    	ledval = 1-ledval;

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
    /*
    if(!gk_USART_RX_QueueEmpty())
    {
       userVal = usart_w_interrupt_getchar();

       //toggle led
       GPIO_WriteBit(GPIOB,GPIO_Pin_7,(ledval)? Bit_SET : Bit_RESET);
       ledval = 1-ledval;
       usart_w_interrupt_putchar('\n');
       usart_w_interrupt_putchar('R');
       usart_w_interrupt_putchar('c');      
       usart_w_interrupt_putchar('v');
       usart_w_interrupt_putchar('d');
       usart_w_interrupt_putchar(':');
       usart_w_interrupt_putchar(userVal);
       usart_w_interrupt_putchar('\n');
    }
    else
    {
       //usart_w_interrupt_putchar('x');
    }
    */
    
    //Delay(250); //wait 250ms
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

/*void gk_printChar(uint8_t pchar)
{
	int bitOfResult = 7;
        while(bitOfResult >= 0)
        {
           if((uint8_t)(pchar >> bitOfResult)&(uint8_t)1)
           {
                usart_w_interrupt_putchar((char)'1');
           }
           else
           {
                usart_w_interrupt_putchar((char)'0');
           }
           bitOfResult--;
        }
        usart_w_interrupt_putchar('\n');
}*/


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	while(1);
}
#endif
