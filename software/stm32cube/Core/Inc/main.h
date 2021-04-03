/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SD_HandleTypeDef hsd1;
extern DMA_HandleTypeDef hdma_sdmmc1_rx;
extern DMA_HandleTypeDef hdma_sdmmc1_tx;

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;
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
void CallBootloader();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SD_CD_Pin GPIO_PIN_13
#define SD_CD_GPIO_Port GPIOC
#define ADDRESS_11_Pin GPIO_PIN_14
#define ADDRESS_11_GPIO_Port GPIOC
#define ADDRESS_10_Pin GPIO_PIN_15
#define ADDRESS_10_GPIO_Port GPIOC
#define ADDRESS_9_Pin GPIO_PIN_0
#define ADDRESS_9_GPIO_Port GPIOC
#define ADDRESS_8_Pin GPIO_PIN_0
#define ADDRESS_8_GPIO_Port GPIOA
#define ADDRESS_7_Pin GPIO_PIN_1
#define ADDRESS_7_GPIO_Port GPIOA
#define ADDRESS_6_Pin GPIO_PIN_2
#define ADDRESS_6_GPIO_Port GPIOA
#define ADDRESS_5_Pin GPIO_PIN_3
#define ADDRESS_5_GPIO_Port GPIOA
#define ADDRESS_4_Pin GPIO_PIN_4
#define ADDRESS_4_GPIO_Port GPIOA
#define ADDRESS_3_Pin GPIO_PIN_4
#define ADDRESS_3_GPIO_Port GPIOC
#define ADDRESS_2_Pin GPIO_PIN_5
#define ADDRESS_2_GPIO_Port GPIOC
#define ADDRESS_1_Pin GPIO_PIN_0
#define ADDRESS_1_GPIO_Port GPIOB
#define RESET_SW_Pin GPIO_PIN_1
#define RESET_SW_GPIO_Port GPIOB
#define RESET_SW_EXTI_IRQn EXTI1_IRQn
#define DEPOSIT_SW_Pin GPIO_PIN_2
#define DEPOSIT_SW_GPIO_Port GPIOB
#define DEPOSIT_SW_EXTI_IRQn EXTI2_IRQn
#define DEPOSIT_NEXT_SW_Pin GPIO_PIN_12
#define DEPOSIT_NEXT_SW_GPIO_Port GPIOB
#define DEPOSIT_NEXT_SW_EXTI_IRQn EXTI15_10_IRQn
#define EXAMINE_NEXT_SW_Pin GPIO_PIN_13
#define EXAMINE_NEXT_SW_GPIO_Port GPIOB
#define EXAMINE_NEXT_SW_EXTI_IRQn EXTI15_10_IRQn
#define EXAMINE_SW_Pin GPIO_PIN_14
#define EXAMINE_SW_GPIO_Port GPIOB
#define EXAMINE_SW_EXTI_IRQn EXTI15_10_IRQn
#define RUN_SW_Pin GPIO_PIN_15
#define RUN_SW_GPIO_Port GPIOB
#define RUN_SW_EXTI_IRQn EXTI15_10_IRQn
#define STOP_SW_Pin GPIO_PIN_6
#define STOP_SW_GPIO_Port GPIOC
#define STOP_SW_EXTI_IRQn EXTI9_5_IRQn
#define STEP_SW_Pin GPIO_PIN_7
#define STEP_SW_GPIO_Port GPIOC
#define STEP_SW_EXTI_IRQn EXTI9_5_IRQn
#define LED_LATCH_Pin GPIO_PIN_8
#define LED_LATCH_GPIO_Port GPIOA
#define ADDRESS_16_Pin GPIO_PIN_5
#define ADDRESS_16_GPIO_Port GPIOB
#define ADDRESS_15_Pin GPIO_PIN_6
#define ADDRESS_15_GPIO_Port GPIOB
#define ADDRESS_14_Pin GPIO_PIN_7
#define ADDRESS_14_GPIO_Port GPIOB
#define ADDRESS_13_Pin GPIO_PIN_8
#define ADDRESS_13_GPIO_Port GPIOB
#define ADDRESS_12_Pin GPIO_PIN_9
#define ADDRESS_12_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
