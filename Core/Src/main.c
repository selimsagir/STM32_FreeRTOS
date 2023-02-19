/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
xTaskHandle handle_menu_task;
xTaskHandle handle_cmd_task;
xTaskHandle handle_print_task;
xTaskHandle handle_led_task;
xTaskHandle handle_rtc_task;

QueueHandle_t q_data;
QueueHandle_t q_print;

uint8_t user_data;
state_t curr_state = sMainMenu;
// timer handles for software timer - leds
xTimerHandle handle_led_timer[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void led_effect_callback(xTimerHandle xTimer);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  BaseType_t status;
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_PCD_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
/*
 * freertos user configuration
 * */
	status = xTaskCreate(menu_task, "menu_task", 128, NULL, 2, &handle_menu_task);

	configASSERT(status == pdPASS);

	status = xTaskCreate(cmd_handler_task, "cmd_task", 128, NULL, 2, &handle_cmd_task);

	configASSERT(status == pdPASS);

	status = xTaskCreate(print_task, "print_task", 128, NULL, 2, &handle_print_task);

	configASSERT(status == pdPASS);

	status = xTaskCreate(led_task, "led_task", 128, NULL, 2, &handle_led_task);

	configASSERT(status == pdPASS);

	//status = xTaskCreate(rtc_task, "rtc_task", 128, NULL, 2, &handle_rtc_task);

	configASSERT(status == pdPASS);

	q_data = xQueueCreate (5, sizeof(char));

	//	configASSERT(q_data != NULL);

	q_print = xQueueCreate (5, sizeof(size_t));

	//	configASSERT(q_print != NULL);

	// create for timer for 4 led attribute

  for ( int i = 0;  i < 4; i++) {
	  handle_led_timer[i] = xTimerCreate("led_timer", pdMS_TO_TICKS(500), pdTRUE, (void*)(i+1), led_effect_callback);
}

  HAL_UART_Receive_IT(&huart2, &user_data, 1);
  /* USER CODE END 2 */



  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void led_effect_callback(xTimerHandle xTimer)
{
	int id;
	id = (uint32_t) pvTimerGetTimerID( xTimer );
	switch (id) {
		case 1:
			LED_effect1();
			break;
		case 2:
			LED_effect2();
			break;
		case 3:
			LED_effect3();
			break;
		case 4:
			LED_effect4();
			break;
	}
}


/*
 * This function is called from the UART interrupt handler, hence it executes in interrupt context.
 * It handles received data bytes from the UART and enqueues them in a queue, as well as notifying
 * a command handling task when a new line character is received.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Local variable to temporarily hold a data byte when needed
    uint8_t dummy;

    // Delay to allow other interrupts to be handled
    for(uint32_t i = 0 ; i < 4000 ; i++);

    // Check if the queue is not full
    if(! xQueueIsQueueFullFromISR(q_data))
    {
        // Enqueue received data byte
        xQueueSendFromISR(q_data , (void*)&user_data , NULL);
    }
    else
    {
        // If the queue is full, check if the received data byte is a new line character
        if(user_data == '\n')
        {
            // Dequeue the last data byte from the queue
            xQueueReceiveFromISR(q_data,(void*)&dummy,NULL);

            // Enqueue the new line character
            xQueueSendFromISR(q_data ,(void*)&user_data , NULL);
        }
    }

    // Check if the received data byte is a new line character
    if( user_data == '\n' )
    {
        // Send notification to command handling task
        xTaskNotifyFromISR (handle_cmd_task,0,eNoAction,NULL);
    }

    // Enable UART data byte reception again in IT mode
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&user_data, 1);

}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
