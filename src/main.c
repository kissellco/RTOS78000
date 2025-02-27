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
    MXC_TRNG_Init();
    MXC_TRNG_Enable();

    // Security delay on boot from 800ms to 1000ms
    uint32_t random_value;
    MXC_TRNG_Random(&random_value, sizeof(random_value));
    int base_delay = SystemCoreClock/10; // Calculate base delay (1000ms worth of cycles)
    int random_factor = (random_value % 200) * -1; // Use random value to generate number between 0 and 200
    int delay_cycles = base_delay + (base_delay * random_factor / 1000); // Calculate final delay (between 80% and 100% of base_delay)
    float expected_delay_ms = (float)delay_cycles / (SystemCoreClock / 1000); // Estimate actual time taken to boot

    // Measure boot delay
    mxc_tmr_cfg_t tmr_cfg;
    tmr_cfg.pres = TMR_PRES_1;
    tmr_cfg.mode = TMR_MODE_CONTINUOUS;
    tmr_cfg.cmp_cnt = 0xFFFFFFFF;  // Set to max value
    tmr_cfg.pol = 0;

    // Initialise timer
    MXC_TMR_Init(MXC_TMR0, &tmr_cfg, false);
    MXC_TMR_SetCount(MXC_TMR0, 0);
    MXC_TMR_Start(MXC_TMR0);

    // Security delay on boot to prevent bruteforce attacks
    printf("Initialising secure boot process\n\n");
    int base_delay = SystemCoreClock/10;
    int random_factor = (rand() % 200) - 100;  // -100 to +100
    int delay_cycles = base_delay + (base_delay * random_factor / 1000);  // +/- 10%
    
    for (volatile int i = 0; i < delay_cycles; i++) {
        if (i % (delay_cycles/50) == 0) {
            printf(".");
        }
    }

    // Stop the timer and get the count
    MXC_TMR_Stop(MXC_TMR0);
    uint32_t timer_count = MXC_TMR_GetCount(MXC_TMR0);
    
    // Calculate actual delay in milliseconds
    float actual_delay_ms = (float)timer_count / (SystemCoreClock / 1000);

    
    printf("\n\n");
    printf("*************************************************\n");
    printf("*            Boot Sequence Completed            *\n");
    printf("*            G'day from Team Flinders           *\n");
    printf("*      MAX78000 powered by FreeRTOS (V%s)       *\n", tskKERNEL_VERSION_NUMBER);
    printf("*************************************************\n");
    printf("SystemCoreClock = %d\n\n", SystemCoreClock);
    
    printf("Security delay: %.2f ms (target: %.2f ms)\n\n", actual_delay_ms, expected_delay_ms);
    printf("SystemCoreClock = %d\n", SystemCoreClock);

    // Continue with the rest of your code
    printf("Creating tasks...\n");
    

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
