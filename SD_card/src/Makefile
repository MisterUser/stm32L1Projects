TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o misc.o
OBJS+= stm32l1xx_usart.o usart_int_w_q_no_flowCNTL.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


