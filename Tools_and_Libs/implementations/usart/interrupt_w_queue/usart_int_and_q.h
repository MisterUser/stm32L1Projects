#ifndef USART_INT_W_Q_H
#define USART_INT_W_Q_H

#include <stm32l1xx_usart.h>

//-------Defines----------//
#define USART1_INDEX 1
#define USART3_INDEX 3
#define QUEUE_SIZE   20
#define HIGH_WATER (QUEUE_SIZE - 6)

//-------Structs/Enums--------//
typedef struct {
    uint16_t pRD , pWR;
    uint8_t q[QUEUE_SIZE];
}USART_Queue;

//------File Scope----------//
static int TxPrimed;
static int RxOverflow;
static USART_Queue UART1_RXq;
static USART_Queue UART1_TXq;


//--------Functions-----------//
//public
void usart_int_and_q_init(); 
int putchar (uint16_t c);
uint16_t getchar (void);
int gk_USART_RX_QueueEmpty();

//private
static int QueueEmpty ( USART_Queue *q);
static int QueueFull ( USART_Queue *q);
static int QueueAvail ( USART_Queue *q);
static int Enqueue ( USART_Queue *q, uint8_t data);
static int Dequeue ( USART_Queue *q, uint8_t *data);

//void USART1_Handler(void);


#endif



