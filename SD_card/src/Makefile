TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o misc.o
OBJS+= stm32l1xx_usart.o usart_int_w_q_no_flowCNTL.o
OBJS+= stm32l1xx_spi.o ff.o diskio.o sd_driver.o sd_low_level.o
OBJS+= stm32l1xx_dac.o dac_dma_tim.o
OBJS+= stm32l1xx_tim.o stm32l1xx_dma.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


