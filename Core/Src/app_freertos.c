/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "at_define.h"
#include "at_statemachine.h"
#include "stdio.h"
#include "utils.h"
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
/* USER CODE BEGIN Variables */

uint8_t at_buffer1[AT_BUFFER_SIZE];
uint8_t at_buffer2[AT_BUFFER_SIZE];
uint32_t receive_buffer_index = 1;
uint32_t dispose_buffer_index = 1;
/* USER CODE END Variables */
/* Definitions for at_downloadfile */
osThreadId_t at_downloadfileHandle;
const osThreadAttr_t at_downloadfile_attributes = {
  .name = "at_downloadfile",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void at_download_file_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of at_downloadfile */
  at_downloadfileHandle = osThreadNew(at_download_file_task, NULL, &at_downloadfile_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_at_download_file_task */
/**
  * @brief  Function implementing the at_download_fil thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_at_download_file_task */
void at_download_file_task(void *argument)
{
  /* USER CODE BEGIN at_download_file_task */
  /* Infinite loop */
	nbiot_fsm_state_index_t* current_index_pointer;
	osStatus_t wait_result = osOK;
	int reset_flag = 0;
	int wait_again = 0;
	int wait_time = 0;
	
	//check the update flag
	if (!should_update()) {
		jump_to_app();
	}
	
	//init at state machine
	init_statemachine();
	
	//download the file
	while(1) {
		
		if (reset_flag) {
			jump_to_app();
			reset_flag = 0;
		}
		
		// ok and command arrived at different time
		if (!wait_again) {
			current_index_pointer = get_current_state_index();
			current_index_pointer->fsm_state->action1();
			//HAL_Delay(50);
			wait_time = current_index_pointer->fsm_state->wait_time;
		}
		printf("d:%d,r:%d\n", dispose_buffer_index, receive_buffer_index);
		wait_result = osThreadFlagsWait(AT_WAIT_FLAG, osFlagsWaitAny, wait_time);
		/*
		if (dispose_buffer_index != receive_buffer_index) {
			wait_result = osThreadFlagsWait(AT_WAIT_FLAG, osFlagsWaitAny, wait_time);
		} else {
			(dispose_buffer_index==1) ? (dispose_buffer_index = 2) : (dispose_buffer_index = 1);
			wait_result = 55;
		}
		*/
		//if time out , retry tyrcnt times.
		printf("wait result = %d\n", wait_result);
		if (wait_result == osErrorTimeout) {
			if (current_index_pointer->trycnt < current_index_pointer->fsm_state->try_cnt) {
				current_index_pointer->trycnt++;
				continue;
			} else {
				reset_flag = 1;
				continue;
			}
		} else if (wait_result == AT_RESET_FLAG) {
			reset_flag = 0;
			continue;
		}
		
		action_result action2_result;
		if (dispose_buffer_index == 1) {
			printf("buffer:%s\n", at_buffer1);
			action2_result = current_index_pointer->fsm_state->action2((char*)at_buffer1);
			dispose_buffer_index = 2;
		} else {
			printf("buffer:%s\n", at_buffer2);
			action2_result = current_index_pointer->fsm_state->action2((char*)at_buffer2);
			dispose_buffer_index = 1;
		}
		
		if (action2_result == ACTION_REPEAT) {
			wait_again = 0;
			continue;
		} else if (action2_result == ACTION_SUCCESS) {
			wait_again = 0;
			HAL_Delay(current_index_pointer->fsm_state->delay_time);
			jump_to_next_at_statemachine(); 
		} else if (action2_result == ACTION_WAIT_AGAIN) {
			wait_again = 1;
		} else if (action2_result == ACTION_COMPLETED) {
			jump_to_app();
		} else {
			current_index_pointer->trycnt++;
		}
		printf("number2=%ld, %ld\n", uxTaskGetNumberOfTasks(), uxTaskGetStackHighWaterMark(at_downloadfileHandle));
	}
  /* USER CODE END at_download_file_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
