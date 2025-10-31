/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
CAN_TxHeaderTypeDef pTxHeader; // Entête trame CAN à transmettre
CAN_RxHeaderTypeDef pRxHeader; // Entête trame CAN reçue
CAN_FilterTypeDef Filtercan1;  // Configuration filtre CAN 

// Enumération pour choisir la sortie des données reçues
typedef enum {
    DEST_NONE = 0,
    DEST_CAN,
    DEST_UART,
    DEST_SPI
} Destination;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Buffers pour SPI / UART / CAN 
uint8_t Tx_Can[5];
uint8_t Rx_Can[5];

uint8_t Tx_Uart[5];
uint8_t Rx_Uart[5];


uint8_t Tx_Spi[5];
uint8_t Rx_Spi[5];

// Flags indiquant réception via interruption 
char spi_data_ready = 0;
char can_data_ready = 0;
char uart_data_ready = 0;

// Destination sélectionnée pour rediriger les données reçues 
Destination data_output = DEST_SPI;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

// Prototypes fonctions 
void send_data_from_spi(void);
void send_data_from_can(void);
void send_data_from_uart(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  
  // Démarrage et configuration CAN 
  if (HAL_CAN_Start(&hcan1)!= HAL_OK)
  {
     Error_Handler();
  }
  Filtercan1.FilterIdHigh = 0x00;
  Filtercan1.FilterIdLow = 0x00;
  Filtercan1.FilterMaskIdHigh = 0x00;
  Filtercan1.FilterMaskIdLow = 0x00;
  Filtercan1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  Filtercan1.FilterBank = 0x00;
  Filtercan1.FilterMode = CAN_FILTERMODE_IDMASK;
  Filtercan1.FilterScale = CAN_FILTERSCALE_32BIT;
  Filtercan1.FilterActivation = CAN_FILTER_ENABLE;
  Filtercan1.SlaveStartFilterBank = 14;
  if (HAL_CAN_ConfigFilter(&hcan1, &Filtercan1)!= HAL_OK)
  {
  	 Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING)!= HAL_OK)
  {
  	 Error_Handler();
  }

  // Activation réception UART et SPI en mode IT 
  if (HAL_UART_Receive_IT(&huart2, Rx_Uart, 3)!= HAL_OK)
  {
	 Error_Handler();
  }
  
  // Activation réception SPI en mode IT
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
  	 Error_Handler();
  }
  if( HAL_SPI_Receive_IT(&hspi2, Rx_Spi, 4)!= HAL_OK)
  {
  	 Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Si données reçues via SPI 
      if (spi_data_ready == 1)
      {
	spi_data_ready = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	send_data_from_spi();
	if( HAL_SPI_Receive_IT(&hspi2, Rx_Spi, 3)!= HAL_OK)
	{
	  Error_Handler();
	}
      }
      HAL_Delay(500);

    // Si données reçues via CAN 
      if (can_data_ready == 1)
      {
    	 can_data_ready = 0;
    	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    	 send_data_from_can();
      }
      HAL_Delay(500);

    // Si données reçues via UART
      if (uart_data_ready == 1)
      {
         uart_data_ready = 0;
         HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
         send_data_from_uart();
      }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// Traitement des données reçues depuis SPI 
void send_data_from_spi(void) 
{
    switch (data_output) {
    case DEST_CAN:
    memcpy(Tx_Can, Rx_Spi, 5);
    pTxHeader.DLC=3;
    pTxHeader.ExtId=0;
    pTxHeader.IDE=CAN_ID_STD;
    pTxHeader.StdId=1;
    pTxHeader.RTR=CAN_RTR_DATA;
    pTxHeader.TransmitGlobalTime = DISABLE;
    if (HAL_CAN_AddTxMessage(&hcan1,&pTxHeader,Tx_Can,(uint32_t *)CAN_TX_MAILBOX0)!= HAL_OK)
    {
       Error_Handler();
    }
    break;

    case DEST_UART:
    memcpy(Tx_Uart, Rx_Spi, 5);
    if (HAL_UART_Transmit_IT(&huart2, Tx_Uart, 10)!= HAL_OK)
    {
       Error_Handler();
    }
    break;

    default:
    break;
   }
}

// Traitement des données reçues depuis CAN
void send_data_from_can(void)
{
    switch (data_output) {
    case DEST_SPI:
    memcpy(Tx_Spi, Rx_Can, 5);
    if (HAL_SPI_DeInit(&hspi2) != HAL_OK)
    {
       Error_Handler();
    }
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
       Error_Handler();
    }
    if(HAL_SPI_Transmit_IT(&hspi2, Tx_Spi, sizeof(Tx_Spi))!= HAL_OK)
    {
       Error_Handler();
    }
    break;

    case DEST_UART:
    memcpy(Tx_Uart, Rx_Can, 5);
    if (HAL_UART_Transmit_IT(&huart2, Tx_Uart, 10)!= HAL_OK)
    {
       Error_Handler();
    }
    break;

    default:
    break;
   }
}

// Traitement des données reçues depuis UART
void send_data_from_uart(void)
{
    switch (data_output) {
    case DEST_SPI:
    memcpy(Tx_Spi, Rx_Uart, 5);
    if (HAL_SPI_DeInit(&hspi2) != HAL_OK)
    {
       Error_Handler();
    }
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
       Error_Handler();
    }
    if(HAL_SPI_Transmit_IT(&hspi2, Tx_Spi, sizeof(Tx_Spi))!= HAL_OK)
    {
        Error_Handler();
    }
    break;

    case DEST_CAN:
    memcpy(Tx_Can, Rx_Uart, 5);
    pTxHeader.DLC=3;
    pTxHeader.ExtId=0;
    pTxHeader.IDE=CAN_ID_STD;
    pTxHeader.StdId=1;
    pTxHeader.RTR=CAN_RTR_DATA;
    pTxHeader.TransmitGlobalTime = DISABLE;
    if (HAL_CAN_AddTxMessage(&hcan1,&pTxHeader,Tx_Can,(uint32_t *)CAN_TX_MAILBOX0)!= HAL_OK)
    {
      Error_Handler();
    }
    break;

    default:
    break;
   }
}

// Callback fin transmission SPI 
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	 if(hspi->Instance == SPI2)
	 {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	 }
}

// Callback fin réception CAN 
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	 can_data_ready = 1;
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	 if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0,&pRxHeader,Rx_Can)!= HAL_OK)
	 {
	    Error_Handler();
	 }
}


// Callback fin réception UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
        {
	  uart_data_ready = 1;
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	  if (HAL_UART_Receive_IT(&huart2, Rx_Uart, 3)!= HAL_OK)
	  {
	      Error_Handler();
	  }
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
