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


/* Explicitly disable tickless mode */
unsigned int disable_tickless = 1;

/* Stringification macros */
#define STRING(x) STRING_(x)
#define STRING_(x) #x

/* =| vTickTockTask |============================================
 *
 * This task writes the current RTOS tick time to the console
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
        printf("Uptime is 0x%08x (%u seconds), tickless-idle is %s\n", 
               (unsigned int)ticks, 
               (unsigned int)(ticks / configTICK_RATE_HZ),
               disable_tickless ? "disabled" : "enabled");
        
        vTaskDelayUntil(&xLastWakeTime, (configTICK_RATE_HZ * 5));
    }
}

/* =| main |==============================================
 * =======================================================
 */
int main(void)
{
    Board_Init();

    printf("Creating tasks...\n");

    /* 
     * IMPORTANT: These tasks are for testing purposes only!
     * Comment in the one you want to test.
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
    printf("ERROR: FreeRTOS scheduler failed to start!\n");
    while (1) {
        __NOP();
    }
    
    return -1;
}
