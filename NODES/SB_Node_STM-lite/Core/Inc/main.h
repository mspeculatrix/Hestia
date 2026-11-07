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
#define SB_CLK_Pin GPIO_PIN_13
#define SB_CLK_GPIO_Port GPIOC
#define SB_ACT_Pin GPIO_PIN_14
#define SB_ACT_GPIO_Port GPIOC
#define CD_L_Pin GPIO_PIN_0
#define CD_L_GPIO_Port GPIOH
#define CD_R_Pin GPIO_PIN_1
#define CD_R_GPIO_Port GPIOH
#define SB_DAT2_Pin GPIO_PIN_2
#define SB_DAT2_GPIO_Port GPIOB
#define SB_DAT3_Pin GPIO_PIN_4
#define SB_DAT3_GPIO_Port GPIOB
#define SB_DAT4_Pin GPIO_PIN_5
#define SB_DAT4_GPIO_Port GPIOB
#define SB_DAT5_Pin GPIO_PIN_6
#define SB_DAT5_GPIO_Port GPIOB
#define SB_DAT6_Pin GPIO_PIN_7
#define SB_DAT6_GPIO_Port GPIOB
#define SB_DAT7_Pin GPIO_PIN_8
#define SB_DAT7_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
