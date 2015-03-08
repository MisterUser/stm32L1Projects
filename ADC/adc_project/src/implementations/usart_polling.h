#ifndef USART_POLLING_H
#define USART_POLLING_H

#include <stm32l1xx_usart.h>

//--------Functions-----------//
//public
void usart_polling_init();
int usart_polling_putchar(int c);
int usart_polling_getchar(void);

#endif



