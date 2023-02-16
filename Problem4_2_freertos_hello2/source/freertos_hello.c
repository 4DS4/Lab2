/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include <ctype.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LEFT_BIT (1 << 0)
#define RIGHT_BIT (1 << 1)
#define UP_BIT (1 << 2)
#define DOWN_BIT (1 << 3)
/* Task priorities. */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */

void producer_event(void* pvParameters)
{
//	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t left_semaphore = semaphores[0];
	SemaphoreHandle_t down_semaphore = semaphores[1];
	SemaphoreHandle_t right_semaphore = semaphores[2];
	SemaphoreHandle_t up_semaphore = semaphores[3];

	BaseType_t status;
	char c;
	while(1)
	{
		scanf("%c", &c);
		switch(c)
		{
		case 'a':
//		xEventGroupSetBits(event_group, LEFT_BIT);
		xSemaphoreGive(left_semaphore);
		break;
		case 's':
//		xEventGroupSetBits(event_group, DOWN_BIT);
		xSemaphoreGive(down_semaphore);
		break;
		case 'd':
//		xEventGroupSetBits(event_group, RIGHT_BIT);
		xSemaphoreGive(right_semaphore);
		break;
		case 'w':
//		xEventGroupSetBits(event_group, UP_BIT);
		xSemaphoreGive(up_semaphore);
		break;
		}
	}
}

//void consumer_event(void* pvParameters)
//{
//	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
//	EventBits_t bits;
//	while(1)
//	{
//		bits = xEventGroupWaitBits(event_group,
//				LEFT_BIT | RIGHT_BIT | UP_BIT | DOWN_BIT,
//				pdTRUE,
//				pdFALSE,
//				portMAX_DELAY);
//		status = xSemaphoreTake(print_caps_semaphore, portMAX_DELAY);
//		if((bits & LEFT_BIT) == LEFT_BIT)
//		{
//			PRINTF("Left\r\n");
//		}
//		if((bits & RIGHT_BIT) == RIGHT_BIT)
//		{
//			PRINTF("Right\r\n");
//		}
//		if((bits & UP_BIT) == UP_BIT)
//		{
//			PRINTF("Up\r\n");
//		}
//		if((bits & DOWN_BIT) == DOWN_BIT)
//		{
//			PRINTF("Down\r\n");
//		}
//	}
//}
void consumer_left_event(void* pvParameters) {
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t left_semaphore = semaphores[0];
	BaseType_t status;

	while(1)
	{
		status = xSemaphoreTake(left_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer1_semaphore\r\n");
			while (1);
		}
		PRINTF("Left\r\n");
	}
}

void consumer_down_event(void* pvParameters) {
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t down_semaphore = semaphores[1];
	BaseType_t status;

	while(1)
	{
		status = xSemaphoreTake(down_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer1_semaphore\r\n");
			while (1);
		}
		PRINTF("Down\r\n");
	}
}

void consumer_right_event(void* pvParameters) {
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t right_semaphore = semaphores[2];
	BaseType_t status;

	while(1)
	{
		status = xSemaphoreTake(right_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer1_semaphore\r\n");
			while (1);
		}
		PRINTF("Right\r\n");
	}
}

void consumer_up_event(void* pvParameters) {
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*)pvParameters;
	SemaphoreHandle_t up_semaphore = semaphores[3];
	BaseType_t status;

	while(1)
	{
		status = xSemaphoreTake(up_semaphore, portMAX_DELAY);
		if (status != pdPASS)
		{
			PRINTF("Failed to acquire producer1_semaphore\r\n");
			while (1);
		}
		PRINTF("Up\r\n");
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
//	EventGroupHandle_t event_group = xEventGroupCreate();
	SemaphoreHandle_t* semaphores = (SemaphoreHandle_t*) malloc(4 * sizeof(
	SemaphoreHandle_t));
	semaphores[0] = xSemaphoreCreateBinary(); //Producer semaphore
	semaphores[1] = xSemaphoreCreateBinary(); //Consumer semaphore
	semaphores[2] = xSemaphoreCreateBinary(); //Consumer semaphore
	semaphores[3] = xSemaphoreCreateBinary(); //Consumer semaphore

	status = xTaskCreate(producer_event, "producer", 200, (void*)semaphores, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer_left_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer_down_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer_up_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer_right_event, "consumer", 200, (void*)semaphores, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	vTaskStartScheduler();
	while (1)
	{}
}


