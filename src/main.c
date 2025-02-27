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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "task.h"
#include "semphr.h"
#include "mxc_device.h"
#include "uart.h"
#include "lp.h"
#include "led.h"
#include "board.h"

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

    // Security delay on boot to prevent bruteforce attacks
    printf("Initialising boot processes\n\n");
    for (volatile int i = 0; i < SystemCoreClock/10; i++) {
        // This busy-wait loop should take approximately 1 second
        if (i % (SystemCoreClock/50) == 0) {
            printf(".");  // Print every ~20ms
        }
    }

    printf("\n\n");
    printf("*************************************************\n");
    printf("*            Boot Sequence Completed            *\n");
    printf("*            G'day from Team Flinders           *\n");
    printf("*      MAX78000 powered by FreeRTOS (V%s)       *\n", tskKERNEL_VERSION_NUMBER);
    printf("*************************************************\n");
    printf("SystemCoreClock = %d\n", SystemCoreClock);
    
    // Add 1 second delay to prevent brute force attacks

    
    // Continue with the rest of your code
    printf("Starting scheduler.\n");
    
    /* Print banner (RTOS scheduler not running) */
    printf("\n-=- MAX78000 FreeRTOS (V%s) Demo -=-\n", tskKERNEL_VERSION_NUMBER);
    printf("SystemCoreClock = %d\n", SystemCoreClock);

    /* Create the TickTock task */
    if (xTaskCreate(vTickTockTask, "TickTock", 2 * configMINIMAL_STACK_SIZE,
                    NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        printf("xTaskCreate() failed to create TickTock task.\n");
        while(1) { __NOP(); }
    }

    /* Start scheduler */
    printf("Starting scheduler.\n");
    vTaskStartScheduler();

    /* This code is only reached if the scheduler failed to start */
    printf("ERROR: FreeRTOS scheduler failed to start!\n");
    while (1) {
        __NOP();
    }
    
    return -1;
}
