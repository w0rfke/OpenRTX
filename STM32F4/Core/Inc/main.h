/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_adc.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_rcc.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_bus.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_system.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_exti.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_cortex.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_utils.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_pwr.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_dma.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_spi.h"
#include "../../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
//void display_init(void);
//void sendByte(uint8_t data);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RFV3R_EN_Pin LL_GPIO_PIN_13
#define RFV3R_EN_GPIO_Port GPIOC
#define RFU3R_EN_Pin LL_GPIO_PIN_14
#define RFU3R_EN_GPIO_Port GPIOC
#define RFV3T_EN_Pin LL_GPIO_PIN_15
#define RFV3T_EN_GPIO_Port GPIOC
#define FLASH_CS_Pin LL_GPIO_PIN_1
#define FLASH_CS_GPIO_Port GPIOC
#define FLASH_SDI_Pin LL_GPIO_PIN_2
#define FLASH_SDI_GPIO_Port GPIOC
#define FLASH_SDO_Pin LL_GPIO_PIN_3
#define FLASH_SDO_GPIO_Port GPIOC
#define AIN_VBAT_Pin LL_GPIO_PIN_1
#define AIN_VBAT_GPIO_Port GPIOA
#define LCD_DC_Pin LL_GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOA
#define LCD_CLK_Pin LL_GPIO_PIN_5
#define LCD_CLK_GPIO_Port GPIOA
#define LCD_DAT_Pin LL_GPIO_PIN_7
#define LCD_DAT_GPIO_Port GPIOA
#define PWR_SW_Pin LL_GPIO_PIN_0
#define PWR_SW_GPIO_Port GPIOB
#define FLASH_CLK_Pin LL_GPIO_PIN_10
#define FLASH_CLK_GPIO_Port GPIOB
#define LCD_RST_Pin LL_GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOB
#define LCD_DCB13_Pin LL_GPIO_PIN_13
#define LCD_DCB13_GPIO_Port GPIOB
#define LCD_CS_Pin LL_GPIO_PIN_14
#define LCD_CS_GPIO_Port GPIOB
#define KBD_K0_Pin LL_GPIO_PIN_8
#define KBD_K0_GPIO_Port GPIOD
#define KBD_K1_Pin LL_GPIO_PIN_9
#define KBD_K1_GPIO_Port GPIOD
#define KBD_K2_Pin LL_GPIO_PIN_10
#define KBD_K2_GPIO_Port GPIOD
#define KBD_K3_Pin LL_GPIO_PIN_11
#define KBD_K3_GPIO_Port GPIOD
#define KBD_DB0_Pin LL_GPIO_PIN_12
#define KBD_DB0_GPIO_Port GPIOD
#define KBD_DB1_Pin LL_GPIO_PIN_13
#define KBD_DB1_GPIO_Port GPIOD
#define KBD_DB3_Pin LL_GPIO_PIN_14
#define KBD_DB3_GPIO_Port GPIOD
#define KBD_DB4_Pin LL_GPIO_PIN_15
#define KBD_DB4_GPIO_Port GPIOD
#define BK1080_CLK_Pin LL_GPIO_PIN_8
#define BK1080_CLK_GPIO_Port GPIOC
#define BK1080_EN_Pin LL_GPIO_PIN_9
#define BK1080_EN_GPIO_Port GPIOC
#define RF_AM_AGC_Pin LL_GPIO_PIN_8
#define RF_AM_AGC_GPIO_Port GPIOA
#define MIC_SPK_EN_Pin LL_GPIO_PIN_11
#define MIC_SPK_EN_GPIO_Port GPIOA
#define PTT_SW_Pin LL_GPIO_PIN_12
#define PTT_SW_GPIO_Port GPIOA
#define RED_LED_Pin LL_GPIO_PIN_13
#define RED_LED_GPIO_Port GPIOA
#define GREEN_LED_Pin LL_GPIO_PIN_14
#define GREEN_LED_GPIO_Port GPIOA
#define BK4819_CS_Pin LL_GPIO_PIN_11
#define BK4819_CS_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
