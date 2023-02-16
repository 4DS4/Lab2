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
#define CONSUMER_1 (1 << 0)
#define CONSUMER_2 (1 << 1)
#define PRODUCER_1 (1 << 2)
#define PRODUCER_2 (1 << 3)
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
int counter = 0;

void producer_sem(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	EventBits_t bits;

	while(1)
	{
		bits = xEventGroupWaitBits(event_group,
				CONSUMER_1 | CONSUMER_2,
			pdTRUE,
			pdTRUE,
			portMAX_DELAY);
		counter++;
		xEventGroupSetBits(event_group, PRODUCER_1 | PRODUCER_2);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void consumer1_sem(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	EventBits_t bits;
	while(1)
	{
		xEventGroupSetBits(event_group, CONSUMER_1);
		bits = xEventGroupWaitBits(event_group,
				PRODUCER_1,
				pdTRUE,
				pdTRUE,
				portMAX_DELAY);
		PRINTF("Received Value = %d\r\n", counter);
	}
}
void consumer2_sem(void* pvParameters)
{
	EventGroupHandle_t event_group = (EventGroupHandle_t)pvParameters;
	BaseType_t status;
	EventBits_t bits;
	while(1)
	{
		xEventGroupSetBits(event_group, CONSUMER_2);
		bits = xEventGroupWaitBits(event_group,
				PRODUCER_2,
				pdTRUE,
				pdTRUE,
				portMAX_DELAY);
		PRINTF("Received Value = %d\r\n", counter);
	}
}

int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
	EventGroupHandle_t event_group = xEventGroupCreate();
	status = xTaskCreate(producer_sem, "producer", 200, (void*)event_group, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer1_sem, "consumer", 200, (void*)event_group, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(consumer2_sem, "consumer", 200, (void*)event_group, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	vTaskStartScheduler();
	while (1)
	{}
}

