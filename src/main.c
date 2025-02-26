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
#include "FreeRTOS_CLI.h"
#include "mxc_device.h"
#include "uart.h"
#include "lp.h"
#include "board.h"

/* FreeRTOS+CLI */
void vRegisterCLICommands(void);

/* Mutual exclusion (mutex) semaphores */
SemaphoreHandle_t xUARTmutex;

/* Task IDs */
TaskHandle_t cmd_task_id;

/* Explicitly disable tickless mode */
unsigned int disable_tickless = 1;

/* Stringification macros */
// Change string macros into string literals
#define STRING(x) STRING_(x)
#define STRING_(x) #x

/* Console ISR selection */
#if (CONSOLE_UART == 0)
#define UARTx_IRQHandler UART0_IRQHandler
#define UARTx_IRQn UART0_IRQn

#else
#error "Please update ISR macro for UART CONSOLE_UART"
#endif
mxc_uart_regs_t *ConsoleUART = MXC_UART_GET_UART(CONSOLE_UART);

/* Array sizes */
#define CMD_LINE_BUF_SIZE 80
#define OUTPUT_BUF_SIZE 512

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
        printf("System nominal: uptime of 0x%08x (%u seconds)", ticks, ticks / configTICK_RATE_HZ);
        vTaskDelayUntil(&xLastWakeTime, (configTICK_RATE_HZ * 10));
    }
}

/***** Functions *****/

/* =| UART0_IRQHandler |======================================
 *
 * This function is needed for asynchronous UART
 *  calls to work properly
 *
 * ===========================================================
 */
void UARTx_IRQHandler(void)
{
    MXC_UART_AsyncHandler(ConsoleUART);
}

/* =| vCmdLineTask_cb |======================================
 *
 * Callback on asynchronous reads to wake the waiting command
 *  processor task
 *
 * ===========================================================
 */
void vCmdLineTask_cb(mxc_uart_req_t *req, int error)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* Wake the task */
    xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(cmd_task_id, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}








/* =| main |==============================================
 * =======================================================
 */
int main(void)
{
    /* Delay to prevent bricks */
    volatile int i;
    for (i = 0; i < 0xFFFFFF; i++) {}

    /* Setup manual CTS/RTS to lockout console and wake from deep sleep */
    MXC_GPIO_Config(&uart_cts);
    MXC_GPIO_Config(&uart_rts);


    /* Print banner (RTOS scheduler not running) */
    printf("\n-=- G'day from Flinders University, Australia -=-\n");
    printf("\n-=-        %s Powered by FreeRTOS (%s)        -=-\n", STRING(TARGET), tskKERNEL_VERSION_NUMBER);
#   printf("SystemCoreClock = %d\n", SystemCoreClock);

    /* Create mutexes */
    xUARTmutex = xSemaphoreCreateMutex();
    if (xUARTmutex == NULL) {
        printf("xSemaphoreCreateMutex failed to create a mutex.\n");
    } else {
        /* Configure task */
        if ((xTaskCreate(vTask0, (const char *)"Task0", configMINIMAL_STACK_SIZE, NULL,
                         tskIDLE_PRIORITY + 1, NULL) != pdPASS) ||
            (xTaskCreate(vTask1, (const char *)"Task1", configMINIMAL_STACK_SIZE, NULL,
                         tskIDLE_PRIORITY + 1, NULL) != pdPASS) ||
            (xTaskCreate(vTickTockTask, (const char *)"TickTock", 2 * configMINIMAL_STACK_SIZE,
                         NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) ||
            (xTaskCreate(vCmdLineTask, (const char *)"CmdLineTask",
                         configMINIMAL_STACK_SIZE + CMD_LINE_BUF_SIZE + OUTPUT_BUF_SIZE, NULL,
                         tskIDLE_PRIORITY + 1, &cmd_task_id) != pdPASS)) {
            printf("xTaskCreate() failed to create a task.\n");
        } else {
            /* Start scheduler */
            printf("Starting scheduler...\n");
            vTaskStartScheduler();
        }
    }

    /* This code is only reached if the scheduler failed to start */
    printf("ERROR: FreeRTOS did not start due to above error!\n");
    while (1) {
        __NOP();
    }
    return -1;
}
