#include "usart_int_and_q.h"

void usart_int_and_q_init()
{

   //-----------------------Clocks-----------------------//
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SYSCFG, ENABLE);
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


   //-----------------------GPIOs------------------------//
   GPIO_InitTypeDef  GPIO_InitStructure;

   //Initialize TX of USART1 - PA9
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   //Initialize RX of USART1 - PA10
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   //Initialize CTS pin - PA11
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART1); //set pins to AF
   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   //Initialize RTS pin - PA12
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1); //set pins to AF

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);



   //-----------------------USART------------------------//
   USART_DeInit(USART1);

   //Initialize the USART
   USART_InitTypeDef USART_InitStructure ;
   USART_StructInit(&USART_InitStructure);

   USART_InitStructure.USART_BaudRate   = 57600;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
   USART_Init(USART1, &USART_InitStructure);

   USART_Cmd(USART1, ENABLE);

   //-----------------------Interrupts------------------------//
   //Interrupt init
   USART_ClearITPendingBit(USART1, USART_IT_RXNE);
   USART_ClearITPendingBit(USART1, USART_IT_TXE);

   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

   //Enable NVIC interrupt channel for USART
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   NVIC_InitTypeDef NVIC_InitStructure;

   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   //enable nRTS -> makes RX available
   GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
    
   //-----------------------Flags------------------------//
   TxPrimed = 0;
   RxOverflow = 0;

}//end usart_int_and_q_init()
 
int usart_w_interrupt_putchar (uint16_t c)
{
    while (! Enqueue (& UART1_TXq, c));
    if (! TxPrimed) {
        TxPrimed = 1;
        USART_ITConfig (USART1 , USART_IT_TXE , ENABLE );
    }

    return 0;
}//end putchar()

uint16_t usart_w_interrupt_getchar (void)
{
    uint8_t data;
    while (! Dequeue(& UART1_RXq , &data));

    // If the queue has fallen below high water mark , enable nRTS
    if ( QueueAvail (& UART1_RXq ) <= HIGH_WATER )
    GPIO_WriteBit(GPIOA, GPIO_Pin_12 , Bit_RESET);

    return data;
}//end getchar()

int gk_USART_RX_QueueEmpty()
{
    return QueueEmpty(&UART1_RXq);
}

//-----------------Queue Functions--------------------------//
static int QueueFull ( USART_Queue *q)
{
    return (((q->pWR + 1) % QUEUE_SIZE ) == q->pRD);
}
static int QueueEmpty ( USART_Queue *q)
{
    return (q->pWR == q->pRD);
}
static int Enqueue ( USART_Queue *q, uint8_t data)
{
    if ( QueueFull (q))
        return 0;
    else
    {
        q->q[q->pWR] = data;
        q->pWR = ((q->pWR + 1) == QUEUE_SIZE ) ? 0 : q->pWR + 1;
    }
    return 1;

}
static int Dequeue ( USART_Queue *q, uint8_t *data)
{
    if ( QueueEmpty (q))
        return 0;
    else {
        *data = q->q[q->pRD];
        q->pRD = ((q->pRD + 1) == QUEUE_SIZE ) ? 0 : q->pRD + 1;
    }
    return 1;
}
static int QueueAvail ( USART_Queue *q)
{
    if(q->pRD > q->pWR)
    {
        return q->pRD - (q->pWR + 1);
    }
    else
    {
        return q->pRD + (QUEUE_SIZE - (q->pWR + 1));
    }
}
//-------------END Queue Functions--------------------------//
void USART1_IRQHandler(void)
{
    if( USART_GetITStatus (USART1 , USART_IT_RXNE ) != RESET)
    {
        uint8_t data;

        // clear the interrupt
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

        // buffer the data (or toss it if there 's no room
        // Flow control will prevent this
        data = USART_ReceiveData(USART1) & 0xff;
        if (! Enqueue(& UART1_RXq , data))
            RxOverflow = 1;

        // If queue is above high water mark , disable nRTS
        if ( QueueAvail (& UART1_RXq ) > HIGH_WATER )
	GPIO_WriteBit(GPIOA, GPIO_Pin_12 , Bit_SET);
    }
    if( USART_GetITStatus (USART1 , USART_IT_TXE ) != RESET)
    {
        uint8_t data;
        // Write one byte to the transmit data register
        if ( Dequeue (& UART1_TXq , &data))
        {
            USART_SendData (USART1 , data);
        }
        else
        {
            // if we have nothing to send , disable the interrupt
            // and wait for a kick
            USART_ITConfig (USART1 , USART_IT_TXE , DISABLE );
            TxPrimed = 0;
        }
    }
    if(USART_GetITStatus (USART1 , USART_IT_PE ) != RESET)
    {
        //Parity Error
    }
    if(USART_GetITStatus (USART1 , USART_IT_TC ) != RESET)
    {
        // Transmission Complete
    }
    if(USART_GetITStatus (USART1 , USART_IT_IDLE ) != RESET)
    {
        // Idle Line Detected
    }
    if(USART_GetITStatus (USART1 , USART_IT_LBD ) != RESET)
    {
        // Break Flag
    }
    if(USART_GetITStatus (USART1 , USART_IT_CTS ) != RESET)
    {
        // CTS Flag
    }
    if(USART_GetITStatus (USART1 , USART_IT_ERR ) != RESET)
    {
        // Error
    }
    if(USART_GetITStatus (USART1 , USART_IT_ORE ) != RESET)
    {
        // Overrun Error
    }
    if(USART_GetITStatus (USART1 , USART_IT_NE ) != RESET)
    {
        // Noise Flag
    }
    if(USART_GetITStatus (USART1 , USART_IT_FE ) != RESET)
    {
        // Framing Error
    }

}
