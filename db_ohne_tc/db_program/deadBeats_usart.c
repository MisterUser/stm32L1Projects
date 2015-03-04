#include "deadBeats_usart.h"

void deadbeats_usart_init()
{

    //basic config using TTC
    ttc_usart_prepare();

    ttc_usart_config_t* UsartConfig = ttc_usart_get_configuration(USART1_INDEX);

    UsartConfig->LogicalIndex = 1;
    UsartConfig->BaudRate = 57600;
    UsartConfig->WordLength = 8;
    UsartConfig->HalfStopBits = 2;
    UsartConfig->Flags.Bits.TransmitParity = 0;
    //UsartConfig->Flags.Bits.Cts = 1;
    //UsartConfig->Flags.Bits.Rts = 0;
    UsartConfig->Flags.Bits.RtsCts = 1;
    UsartConfig->Flags.Bits.DelayedTransmits = 0;

    ttc_usart_init(USART1_INDEX);

    //Initialize CTS pin
    ttc_gpio_init(PIN_PA11,tgm_input_floating, tgs_40MHz);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART1); //set pins to AF

    //Initialize RTS pin
    ttc_gpio_init(PIN_PA12,tgm_output_push_pull, tgs_40MHz);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1); //set pins to AF

    //Interrupt init
    ttc_interrupt_handle_t usart_interrupt_handle = ttc_interrupt_init(tit_USART_TransmitDataEmpty,0,USART1_Handler,0,0,0);
    ttc_interrupt_enable_usart(usart_interrupt_handle);
    ttc_interrupt_handle_t usart_interrupt_handle2 = ttc_interrupt_init(tit_USART_RxNE,0,USART1_Handler,0,0,0);
    ttc_interrupt_enable_usart(usart_interrupt_handle2);

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    USART_ClearITPendingBit(USART1, USART_IT_TXE);

    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    //enable nRTS
    ttc_gpio_clr(PIN_PA12);

    TxPrimed = 0;
    RxOverflow = 0;

/*
    putchar('\n');
    putchar('\n');
    putchar(' ');
    putchar(' ');
    putchar(' ');
    putchar(' ');
    putchar(' ');
    putchar(' ');
    putchar(' ');
    putchar('|');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');

    putchar('D');
    putchar('e');
    putchar('a');
    putchar('d');
    putchar('b');
    putchar('e');
    putchar('a');
    putchar('t');
    putchar(' ');
    putchar('D');
    putchar('r');
    putchar('u');
    putchar('m');
    putchar('s');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('-');
    putchar('|');
    putchar('\n');
    putchar('\n');
*/


}//end deadbeats_usart_init()


int putchar (uint16_t c)
{
    while (! Enqueue (& UART1_TXq, c));
    if (! TxPrimed) {
        TxPrimed = 1;
        USART_ITConfig (USART1 , USART_IT_TXE , ENABLE );
    }

    return 0;
}//end putchar()

uint16_t getchar (void)
{
    uint8_t data;
    while (! Dequeue(& UART1_RXq , &data));

    // If the queue has fallen below high water mark , enable nRTS
    if ( QueueAvail (& UART1_RXq ) <= HIGH_WATER )
    ttc_gpio_clr(PIN_PA12); /** @todo */

    return data;
}//end getchar()

int db_USART_RX_QueueEmpty()
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
void USART1_Handler(void)
{
    if( USART_GetITStatus (USART1 , USART_IT_RXNE ) != RESET)
    {
        uint8_t data;

        // clear the interrupt
        USART_ClearITPendingBit (USART1 , USART_IT_RXNE);

        // buffer the data (or toss it if there 's no room
        // Flow control will prevent this
        data = USART_ReceiveData(USART1) & 0xff;
        if (! Enqueue(& UART1_RXq , data))
            RxOverflow = 1;

        // If queue is above high water mark , disable nRTS
        if ( QueueAvail (& UART1_RXq ) > HIGH_WATER )
        ttc_gpio_set(PIN_PA12); /** @todo */
        //usart_ISR();
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
