/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define SENSOR_PWR_ON_Pin GPIO_PIN_13
#define SENSOR_PWR_ON_GPIO_Port GPIOC
#define BTN_L_Pin GPIO_PIN_1
#define BTN_L_GPIO_Port GPIOA
#define ECHO_B_Pin GPIO_PIN_4
#define ECHO_B_GPIO_Port GPIOA
#define ECHO_B_EXTI_IRQn EXTI4_IRQn
#define TRIG_B_Pin GPIO_PIN_5
#define TRIG_B_GPIO_Port GPIOA
#define ESP32_BOOT_Pin GPIO_PIN_4
#define ESP32_BOOT_GPIO_Port GPIOC
#define ESP32_EN_Pin GPIO_PIN_5
#define ESP32_EN_GPIO_Port GPIOC
#define ESP32_PWR_ON_Pin GPIO_PIN_2
#define ESP32_PWR_ON_GPIO_Port GPIOB
#define BTN_R_Pin GPIO_PIN_12
#define BTN_R_GPIO_Port GPIOB
#define LED_BL_Pin GPIO_PIN_13
#define LED_BL_GPIO_Port GPIOB
#define LED_BR_Pin GPIO_PIN_14
#define LED_BR_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOA
#define LED_FR_Pin GPIO_PIN_2
#define LED_FR_GPIO_Port GPIOD
#define LED_FL_Pin GPIO_PIN_3
#define LED_FL_GPIO_Port GPIOB
#define TRIG_F_Pin GPIO_PIN_4
#define TRIG_F_GPIO_Port GPIOB
#define ECHO_F_Pin GPIO_PIN_5
#define ECHO_F_GPIO_Port GPIOB
#define ECHO_F_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
