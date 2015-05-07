/**
  ******************************************************************************
  * @file    sd_low_level.h
  * @author  MCD Application Team
  * MODIFIED BY GK
  * @brief   Low level functions for SD 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_LOW_LEVEL_H
#define __SD_LOW_LEVEL_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "sd_low_level.h"
#include "stm32l1xx_spi.h"

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;


/**
  * @brief  SD Card SPI Interface
  */  
#define SD_SPI                           SPI1
#define SD_SPI_CLK                       RCC_APB2Periph_SPI1 

#define SD_SPI_SCK_PIN                   GPIO_Pin_3                  /* PB.3 */
#define SD_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOB */
#define SD_SPI_SCK_GPIO_CLK              RCC_AHBPeriph_GPIOB
#define SD_SPI_SCK_SOURCE                GPIO_PinSource3
#define SD_SPI_SCK_AF                    GPIO_AF_SPI1

#define SD_SPI_MISO_PIN                  GPIO_Pin_4                  /* PB.4 */
#define SD_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MISO_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SD_SPI_MISO_SOURCE               GPIO_PinSource4
#define SD_SPI_MISO_AF                   GPIO_AF_SPI1

#define SD_SPI_MOSI_PIN                  GPIO_Pin_5                  /* PB.5 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOB */
#define SD_SPI_MOSI_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define SD_SPI_MOSI_SOURCE               GPIO_PinSource5
#define SD_SPI_MOSI_AF                   GPIO_AF_SPI1

#define SD_CS_PIN                        GPIO_Pin_8                  /* PD.2 */
#define SD_CS_GPIO_PORT                  GPIOB                       /* GPIOD */
#define SD_CS_GPIO_CLK                   RCC_AHBPeriph_GPIOB
/*
#define SD_DETECT_PIN                    GPIO_Pin_6                  
#define SD_DETECT_EXTI_LINE              EXTI_Line6
#define SD_DETECT_EXTI_PIN_SOURCE        EXTI_PinSource6

#define SD_DETECT_GPIO_PORT              GPIOE                       
#define SD_DETECT_GPIO_CLK               RCC_AHBPeriph_GPIOE
#define SD_DETECT_EXTI_PORT_SOURCE       EXTI_PortSourceGPIOE
#define SD_DETECT_EXTI_IRQn              EXTI9_5_IRQn 
*/  

/** @defgroup STM32L152_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
  

#endif /* __SD_LOW_LEVEL_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
