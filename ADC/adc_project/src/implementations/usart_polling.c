#include "usart_polling.h"


void usart_polling_init()
{

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SYSCFG, ENABLE);    
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

   GPIO_InitTypeDef  GPIO_InitStructure;

   //GPIO info for TX of USART1
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //info for RX of USART1
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
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
}//end deadbeats_usart_init()


int usart_polling_putchar(int c)
{
   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE ) == RESET);

   USART1->DR = (c & 0xff);
   //USART_SendData(USART1, c);

   return 0;
}//end putchar()

int usart_polling_getchar(void)
{
   while ( USART_GetFlagStatus (USART1 , USART_FLAG_RXNE ) == RESET);

   return USART1->DR & 0xff;
   //return USART_ReceiveData(USART1);
}//end getchar()
