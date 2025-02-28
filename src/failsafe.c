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

/**
 * @brief Reset the device when a critical error occurs
 * 
 * This function turns on the red LED as a visual indicator,
 * logs an error message, and then resets the device.
 * If reset fails, enter failsafe mode.
 * 
 * @param message The error message to print before rebooting
 */
void system_reset(const char *message)
{
    /* Visual indication - set red LED on */
    LED_On(LED_RED);
    
    /* Log the error message */
    if (message != NULL) {
        printf("[FATAL] %s\n", message);
        printf("System attempting reboot...\n Flushing buffer...\n");
    }
    
    /* Ensure all buffered data is sent */
    fflush(stdout);

    /* Try to reset using watchdog */
    mxc_wdt_cfg_t cfg;
    cfg.mode = MXC_WDT_MODE_RESET;  /* Reset mode */
    cfg.timeout = 1;                /* Minimal timeout */
    MXC_WDT_Init(MXC_WDT0, &cfg);
    MXC_WDT_Enable(MXC_WDT0);
    
    /* Try to reset using system reset function */
    MXC_SYS_Reset_Periph(MXC_SYS_RESET_SYSTEM);
    
    /* If we get here, both reset methods failed, enter failsafe mode */
    enter_failsafe();
}
