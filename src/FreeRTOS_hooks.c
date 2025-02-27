#include <stdio.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "mxc_delay.h"

/* Required for heap_4.c */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

/* Global variables to store boot timing information */
float g_actual_delay_ms = 0.0f;
float g_expected_delay_ms = 0.0f;

/* Called if a call to pvPortMalloc() fails */
void vApplicationMallocFailedHook(void)
{
    /* Log the failure */
    printf("[FATAL] Memory allocation failed! System halted. Please reboot.\n");
    
    /* Visual indication - rapidly flash red LED */
    while (1) {
        LED_On(LED_RED);
        MXC_Delay(MXC_DELAY_MSEC(100));
        LED_Off(LED_RED);
        MXC_Delay(MXC_DELAY_MSEC(100));
    }
}

/* Called if stack overflow is detected during context switch */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    /* Log the stack overflow */
    printf("[FATAL] Stack overflow in task %s! System halted. Please reboot.\n", pcTaskName);
    
    /* Visual indication - solid red LED */
    LED_On(LED_RED);
    
    /* Prevent further execution */
    while (1) {
        __NOP();
    }
}

/* Called when a daemon task is created */
void vApplicationDaemonTaskStartupHook(void)
{
    /* Initialize LEDs for system status indication */
    LED_Init();
    
    /* Display boot information and welcome message */
    printf("\n\n");
    printf("*************************************************\n");
    printf("*            Boot Sequence Completed            *\n");
    printf("*            G'day from Team Flinders           *\n");
    printf("*          MAX78000 powered by FreeRTOS         *\n");
    printf("*                     (%s)                      *\n", tskKERNEL_VERSION_NUMBER);
    printf("*************************************************\n");
    printf("SystemCoreClock = %d Hz\n\n", SystemCoreClock);
    
    /* Display boot timing information */
    printf("Security delay: %.2f ms (target: %.2f ms)\n\n", g_actual_delay_ms, g_expected_delay_ms);
    
    /* Display system information */
    printf("FreeRTOS Information:\n");
    printf("  Total Heap Size: %d bytes\n", configTOTAL_HEAP_SIZE);
    printf("  Free Heap: %d bytes\n", xPortGetFreeHeapSize());
    printf("  Minimum Ever Free Heap: %d bytes\n", xPortGetMinimumEverFreeHeapSize());
    printf("  Number of Tasks: %d\n", uxTaskGetNumberOfTasks());
    
    /* Indicate successful boot with a green LED */
    LED_Off(LED_RED);
    LED_On(LED_GREEN);
    
    printf("System initialisation complete. Running tasks...\n\n");
}

/* Static memory for idle task */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

/* Provide memory for the idle task - required for static allocation */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* Static memory for timer task */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

/* Provide memory for the timer task - required for static allocation */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/* Note: Idle and Tick hooks are disabled in FreeRTOSConfig.h 
   (configUSE_IDLE_HOOK=0, configUSE_TICK_HOOK=0) */
