/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file        main.c
 * @brief       FreeRTOS Example Application.
 */

// C inclusions
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// FreeRTOS inclusions
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "task.h"
#include "semphr.h"

// MAX78000 inclusions
#include <max78000.h>
#include "board.h"
#include "mxc_device.h"
#include "uart.h"
#include "lp.h"
#include "led.h"
#include "trng.h"
#include "tmr.h"

// Team Flinders custom inclusions
#include "security_test.h"
#include "failsafe.h"

/* =================================================================
Configuration items 
================================================================= */
/* Explicitly disable tickless mode */
unsigned int disable_tickless = 1;

/* Stringification macros */
#define STRING(x) STRING_(x)
#define STRING_(x) #x

/* =================================================================
Task Setup 
================================================================= */
/* Task IDs */
TaskHandle_t crypto_manager_task_id;
TaskHandle_t subscription_manager_task_id;
TaskHandle_t serial_interface_manager_task_id;
TaskHandle_t channel_manager_task_id;
TaskHandle_t frame_manager_task_id;



/* =| vTickTockTask |============================================
 *
 * This task is only used to monitor signs of life on device.
 *
 * =======================================================
 */
void vTickTockTask(void *pvParameters)
{
    TickType_t ticks = 0;
    TickType_t xLastWakeTime;

    /* Get task start time */
    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        ticks = xTaskGetTickCount();
        printf("System live: uptime is %u\n", 
               (unsigned int)(ticks / configTICK_RATE_HZ)
            );
        vTaskDelayUntil(&xLastWakeTime, (configTICK_RATE_HZ * 5));
    }
}

/* =| main |==============================================
 * =======================================================
 */
int main(void)
{
    Board_Init();
    LED_Init();

    printf("Creating tasks...\n");

    /* 
     * IMPORTANT: These tasks are for testing purposes only!
     */
    
    /* Stack Overflow Test Task - uncomment to test */
    /*    
    stackOverflowTask_Init();
    if (xTaskCreate(stackOverflowTask_vMainTask, "StackTest", STACK_OVERFLOW_TASK_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("xTaskCreate() failed to create Stack Test task.\n");
        while(1) { __NOP(); }
    }
    */
    
    /* Malloc Fail Test Task - uncomment to test */
/*     
    mallocFailTask_Init();
    if (xTaskCreate(mallocFailTask_vMainTask, "MallocTest", MALLOC_FAIL_TASK_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("xTaskCreate() failed to create Malloc Test task.\n");
        while(1) { __NOP(); }
    }
    */
     

    /* Create the TickTock task */
    if (xTaskCreate(vTickTockTask, "TickTock", 2 * configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        printf("xTaskCreate() failed to create TickTock task.\n");
        while(1) { __NOP(); }
    }

    /* Start scheduler */
    printf("Starting scheduler...\n\n");
    vTaskStartScheduler();

    /* This code is only reached if the scheduler failed to start */
    printf("FreeRTOS scheduler failed to start! Activating failsafe...\n");
    failsafe();
    
    return -1;
}
