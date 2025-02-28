/**
 * @file failsafe.h
 * @brief Failsafe functions for critical system errors
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "failsafe.h"

/**
 * @brief Enter a failsafe state when a critical error occurs
 * 
 * This function turns on the red LED as a visual indicator,
 * disables all interrupts, and enters an infinite loop,
 * effectively halting the system. The system will require
 * a manual reset to recover. Last resort to prevent tampering.
 */

void failsafe(void)
{
    LED_On(LED_RED);
    /* Disable all interrupts at the processor level and stop scheduler */
    __disable_irq();
    taskDISABLE_INTERRUPTS();
    printf("System failsafe engaged. Reboot the device.
    /* Do nothing, uninterrupted, until heat death of universe */
    while (1) {
        __NOP();
    }
}
