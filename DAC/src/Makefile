TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o 
OBJS+= stm32l1xx_tim.o stm32l1xx_dma.o 
OBJS+= stm32l1xx_dac.o dac_signalGen.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


