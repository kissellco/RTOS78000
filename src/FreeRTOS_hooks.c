#include "FreeRTOS.h"
#include "task.h"

/* Required for heap_4.c */
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

/* Called if a call to pvPortMalloc() fails */
void vApplicationMallocFailedHook(void)
{
    /* Force an assert */
    configASSERT((volatile void*)NULL);
}

/* Called if stack overflow is detected during context switch */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;
    
    /* Force an assert */
    configASSERT((volatile void*)NULL);
}

/* Called when a daemon task is created */
void vApplicationDaemonTaskStartupHook(void)
{
    /* This function will get called when a daemon task completes startup */
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
