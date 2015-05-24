TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o stm32l1xx_usart.o stm32l1xx_tim.o misc.o
OBJS+= usart_int_and_q.o
OBJS+= stm32l1xx_adc.o adc_dma_cont.o
OBJS+= stm32l1xx_dma.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


