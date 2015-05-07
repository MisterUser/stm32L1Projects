TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O1 -g
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o
OBJS+= stm32l1xx_gpio.o stm32l1xx_rcc.o misc.o

#SPI, SD, and FATFS
OBJS+= stm32l1xx_spi.o sd_driver.o sd_low_level.o

#DAC with DMA and TIMER
OBJS+= stm32l1xx_dac.o dac_dma_tim.o
OBJS+= stm32l1xx_tim.o stm32l1xx_dma.o

#sleep mode and interrupts
#OBJS+= stm32l1xx_syscfg.o stm32l1xx_exti.o #misc.o
#OBJS+= stm32l1xx_pwr.o stopMODE.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


