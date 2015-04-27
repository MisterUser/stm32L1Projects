TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o #stm32l1xx_usart.o usart_int_and_q.o misc.o
OBJS+= stm32l1xx_syscfg.o stm32l1xx_exti.o misc.o
OBJS+= stm32l1xx_pwr.o stopMODE.o
OBJS+= stm32l1xx_flash.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


