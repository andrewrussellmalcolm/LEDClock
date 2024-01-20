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
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern uint8_t usb_char_waiting(void);
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define CLS "\x1B[2J"
#define CLL "\x1B[2K"
#define kbhit usb_char_waiting
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SEG_A_Pin GPIO_PIN_0
#define SEG_A_GPIO_Port GPIOA
#define SEG_B_Pin GPIO_PIN_1
#define SEG_B_GPIO_Port GPIOA
#define SEG_C_Pin GPIO_PIN_2
#define SEG_C_GPIO_Port GPIOA
#define SEG_D_Pin GPIO_PIN_3
#define SEG_D_GPIO_Port GPIOA
#define SEG_E_Pin GPIO_PIN_4
#define SEG_E_GPIO_Port GPIOA
#define SEG_F_Pin GPIO_PIN_5
#define SEG_F_GPIO_Port GPIOA
#define SEG_G_Pin GPIO_PIN_6
#define SEG_G_GPIO_Port GPIOA
#define DIG_1_Pin GPIO_PIN_7
#define DIG_1_GPIO_Port GPIOA
#define SW1_INT_Pin GPIO_PIN_0
#define SW1_INT_GPIO_Port GPIOB
#define SW1_INT_EXTI_IRQn EXTI0_1_IRQn
#define SW2_INT_Pin GPIO_PIN_1
#define SW2_INT_GPIO_Port GPIOB
#define SW2_INT_EXTI_IRQn EXTI0_1_IRQn
#define SW3_INT_Pin GPIO_PIN_2
#define SW3_INT_GPIO_Port GPIOB
#define SW3_INT_EXTI_IRQn EXTI2_3_IRQn
#define DBG_TX_Pin GPIO_PIN_10
#define DBG_TX_GPIO_Port GPIOB
#define DBG_RX_Pin GPIO_PIN_11
#define DBG_RX_GPIO_Port GPIOB
#define SW5_INT_Pin GPIO_PIN_12
#define SW5_INT_GPIO_Port GPIOB
#define SW5_INT_EXTI_IRQn EXTI4_15_IRQn
#define DP_Pin GPIO_PIN_13
#define DP_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_14
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOB
#define DIG_2_Pin GPIO_PIN_8
#define DIG_2_GPIO_Port GPIOA
#define DIG_3_Pin GPIO_PIN_9
#define DIG_3_GPIO_Port GPIOA
#define DIG_4_Pin GPIO_PIN_10
#define DIG_4_GPIO_Port GPIOA
#define COLON_Pin GPIO_PIN_15
#define COLON_GPIO_Port GPIOA
#define SW4_INT_Pin GPIO_PIN_3
#define SW4_INT_GPIO_Port GPIOB
#define SW4_INT_EXTI_IRQn EXTI2_3_IRQn
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
