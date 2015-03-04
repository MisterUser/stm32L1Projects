//Functions to test STM32L1 discovery board
#include "ttc-lib/ttc_basic.h"
#include "ttc-lib/ttc_timer_old.h"
#include "ttc-lib/ttc_gpio.h"
#include "ttc-lib/ttc_usart.h"
#include <stm32l1xx.h>
#include <stm32l1xx_tim.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_dac.h>


void blinkLED(); 
void ChangeHighLED1(void *TaskArgument);
void ChangeLowLED1(void *TaskArgument);
int putchar (uint16_t c);
uint16_t getchar (void);


#define USART1_INDEX 1
