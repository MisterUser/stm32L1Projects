TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o clock_setup.o
#UART 
OBJS+= stm32l1xx_usart.o usart_int_and_q.o misc.o
#LCD
OBJS+= hd44780_4bit_lib.o
#DAC
OBJS+= dac_signalGen.o stm32l1xx_dac.o stm32l1xx_tim.o stm32l1xx_dma.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


