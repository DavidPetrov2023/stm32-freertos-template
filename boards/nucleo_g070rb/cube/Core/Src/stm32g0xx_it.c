/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_it.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Avoid including project driver headers here to prevent macro collisions. */
/* Forward declarations only (keep ISR minimal & decoupled). */
typedef struct uart_stm32_ctrl uart_stm32_ctrl_t;
extern uart_stm32_ctrl_t g_uart2_ctrl;
void uart_stm32_irq_rx_feed(uart_stm32_ctrl_t *ctrl, uint8_t byte);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Compatibility: some HAL variants use RXNE_RXFNE flag. */
#ifndef UART_FLAG_RXNE_RXFNE
#define UART_FLAG_RXNE_RXFNE UART_FLAG_RXNE
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim6;

/* USER CODE BEGIN EV */
/* Export control block of UART2 instance (defined in instances/uart_instances.c). */
extern uart_stm32_ctrl_t g_uart2_ctrl;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
    /* Intentionally left as infinite loop; add logging if required. */
    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1) {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */
    /* USER CODE END HardFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
    /* USER CODE BEGIN TIM6_IRQn 0 */
    /* Keep ISR minimal; defer any heavy work to tasks. */
    /* USER CODE END TIM6_IRQn 0 */
    HAL_TIM_IRQHandler(&htim6);
    /* USER CODE BEGIN TIM6_IRQn 1 */
    /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
  */
void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART2_IRQn 0 */
    /* Minimal ISR: fetch one byte and push into driver's RX ring buffer. */
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE_RXFNE) != RESET) {
        uint8_t byte = (uint8_t) (huart2.Instance->RDR & 0xFFu); /* Read clears RXNE */
        uart_stm32_irq_rx_feed(&g_uart2_ctrl, byte);
    }
    /* USER CODE END USART2_IRQn 0 */
    HAL_UART_IRQHandler(&huart2);
    /* USER CODE BEGIN USART2_IRQn 1 */
    /* Extension point: do not block, do not call RTOS primitives here. */
    /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
