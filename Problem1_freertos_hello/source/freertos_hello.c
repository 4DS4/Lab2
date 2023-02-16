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

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

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

char str[10] = "";
TaskHandle_t xHandle;

void get_value(void *pvParameters)
{

		PRINTF("Enter Value: ");
		scanf("%s", (char*)pvParameters);
		vTaskDelete( NULL );
}

void print_value(void *pvParameters)
{
	while(1)
	{
		if(strlen(pvParameters) > 0){
			PRINTF("%s.\r\n", (char*) pvParameters);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
int main(void)
{
	BaseType_t status;
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	status = xTaskCreate(get_value, "Get_value_task", 200, (void*)str, 2, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	status = xTaskCreate(print_value, "Print_value_task2", 200, (void*)str, 3, NULL);
	if (status != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	vTaskStartScheduler();
	for (;;);
}

