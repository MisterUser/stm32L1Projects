#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include "implementations/ff.h"
#include "implementations/sd_driver.h"
#include "implementations/dac_dma_tim.h"

 
void Delay(uint32_t nTime);
void gk_printChar(uint8_t pchar);
#define BUFFERSIZE 1000
uint8_t readBuf1[BUFFERSIZE];
uint8_t readBuf2[BUFFERSIZE];
uint8_t bufferToSend = 2;
uint8_t buffer_finished = 0;

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
  SD_Init(); //<--done by f_open

  //Configure SysTick Timer
  //Set System Clock to interrupt every ms.
  if(SysTick_Config(SystemCoreClock / 32000))
	while (1); //If fails, hang in while loop 

  //char userVal;

/*
  FATFS FatFs;   // Work area (file system object) for logical drive 

  // Register work area to the default drive 
  f_mount(&FatFs, "", 0);

  unsigned char read_buf[20];
  FIL fp;
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
  

  uint32_t fPTR = 0x0016002C;
  SD_Error readResp=SD_ReadBlock(readBuf1,fPTR,1000);
  if(readResp == SD_RESPONSE_NO_ERROR)
  {
	fPTR = fPTR+1000;
	readResp=SD_ReadBlock(readBuf2,fPTR,1000);
  }
  bufferToSend = 2;
  buffer_finished = 0;


  //DMA starts with buffer 1 
  DAC_DMA_TIM_init();
  //bufferToSend = 2 at this point, so
  //after buffer 1 is finished, buffer2 will be sent and buffer 1 will be loaded 

  while (1) {


    if(buffer_finished == 1)
    {
    
    	static int ledval = 0;
    	GPIO_WriteBit(GPIOB,GPIO_Pin_7,(ledval)? Bit_SET : Bit_RESET);
    	ledval = 1-ledval;
	
	//iterate fPtr
	fPTR = fPTR +1000;
	//make sure it's not > 5,644,844 (size of song)
	if(fPTR > 5644000)
	{
	   fPTR = 0x0016002C;
	}
		
	//choose buffer by reading previously sent buffer
	if(bufferToSend == 2) //2 was just sent
	{
	   readResp=SD_ReadBlock(readBuf1,fPTR,1000);
	   bufferToSend = 1;
	}
	else
	{
           readResp=SD_ReadBlock(readBuf2,fPTR,1000);
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
