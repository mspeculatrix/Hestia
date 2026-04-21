/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

#include "stm32h5xx_nucleo.h"
#include <stdio.h>

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

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DAT5_Pin GPIO_PIN_13
#define DAT5_GPIO_Port GPIOC
#define DAT5_EXTI_IRQn EXTI13_IRQn
#define DAT6_Pin GPIO_PIN_14
#define DAT6_GPIO_Port GPIOC
#define DAT7_Pin GPIO_PIN_15
#define DAT7_GPIO_Port GPIOC
#define SB_CLK_Pin GPIO_PIN_0
#define SB_CLK_GPIO_Port GPIOC
#define SB_ACT_Pin GPIO_PIN_1
#define SB_ACT_GPIO_Port GPIOC
#define ARD_D1_TX_Pin GPIO_PIN_14
#define ARD_D1_TX_GPIO_Port GPIOB
#define ARD_D0_RX_Pin GPIO_PIN_15
#define ARD_D0_RX_GPIO_Port GPIOB
#define DAT0_Pin GPIO_PIN_8
#define DAT0_GPIO_Port GPIOC
#define DAT1_Pin GPIO_PIN_9
#define DAT1_GPIO_Port GPIOC
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define T_JTDI_Pin GPIO_PIN_15
#define T_JTDI_GPIO_Port GPIOA
#define DAT2_Pin GPIO_PIN_10
#define DAT2_GPIO_Port GPIOC
#define DAT3_Pin GPIO_PIN_11
#define DAT3_GPIO_Port GPIOC
#define DAT4_Pin GPIO_PIN_12
#define DAT4_GPIO_Port GPIOC
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
