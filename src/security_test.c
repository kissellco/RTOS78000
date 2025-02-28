/**
 * @file security_test.c
 * @brief Test tasks to demonstrate FreeRTOS failure hooks
 */

#include <stdio.h>
#include <string.h>

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// MAX78000 includes
#include "board.h"
#include "led.h"
#include "pb.h"

// Project includes
#include "security_test.h"

/**
 * @brief Initialize the stack overflow test task
 */
void stackOverflowTask_Init(void) {
    printf("[Stack Test] Task initialising...\n");
}

/**
 * @brief Test task that causes a stack overflow
 * @details This task intentionally causes a stack overflow by creating
 * a large local array and using infinite recursion
 */
void stackOverflowTask_vMainTask(void *pvParameters) {
    printf("[Stack Test] Task started. This will cause a stack overflow...\n");

    // Function to cause a stack overflow through recursion
    void recursiveFunction(int depth) {
        // Large local array to consume stack
        char largeArray[64];
        
        // Fill the array to prevent optimization
        memset(largeArray, 0xAA, sizeof(largeArray));
        
        // Print depth occasionally to show progress
        if (depth % 5 == 0) {
            printf("[Stack Test] Recursion depth: %d\n", depth);
        }
        
        // Recurse infinitely, consuming more stack each time
        recursiveFunction(depth + 1);
    }
    
    // Start the recursive function
    printf("[Stack Test] Starting infinite recursion...\n");
    recursiveFunction(1);
    
    // This point should never be reached
    printf("[Stack Test] Task ending (this should never be printed)\n");
}

/**
 * @brief Initialise the malloc failure test task
 */
void mallocFailTask_vMainTask(void *pvParameters) {
    printf("[Malloc Test] Task started. This will exhaust the heap...\n");
    
    // Use static array instead of stack array to avoid stack overflow
    static void *ptrArray[250]; // Reduced size, static allocation
    int allocCount = 0;   // Number of successful allocations
    
    printf("[Malloc Test] Starting memory allocations...\n");
    printf("[Malloc Test] Initial free heap: %d bytes\n", xPortGetFreeHeapSize());
    
    // Allocate memory in chunks until failure
    size_t chunkSize = 1024; // 1KB chunks
    while (1) {
        // Try to allocate a chunk of memory
        void *ptr = pvPortMalloc(chunkSize);
        
        if (ptr == NULL) {
            // Allocation failed, heap is exhausted
            printf("[Malloc Test] Allocation failed after %d allocations\n", allocCount);
            printf("[Malloc Test] Final free heap: %d bytes\n", xPortGetFreeHeapSize());
            
            // The next allocation will trigger the MallocFailedHook
            printf("[Malloc Test] Triggering MallocFailedHook...\n");
            void *finalPtr = pvPortMalloc(64); // Small allocation to trigger the hook
            
            // This point should never be reached
            printf("[Malloc Test] Still running (this should never be printed)\n");
            break;
        }
        
        // Fill the memory to ensure it's not optimized away
        memset(ptr, 0xBB, chunkSize);
        
        // Save the pointer
        if (allocCount < sizeof(ptrArray)/sizeof(ptrArray[0])) {
            ptrArray[allocCount] = ptr;
        }
        
        // Increment allocation count
        allocCount++;
        
        // Print progress periodically
        if (allocCount % 5 == 0) {
            printf("[Malloc Test] Allocated %d chunks (%d bytes), free heap: %d bytes\n", 
                    allocCount, allocCount * chunkSize, xPortGetFreeHeapSize());
            
            // Instead of vTaskDelay, use a short busy-wait
            for (volatile int i = 0; i < 10000; i++) {
                __NOP();
            }
        }
    }
    
    // This point should never be reached
    printf("[Malloc Test] Task ending (this should never be printed)\n");
    
    // Instead of vTaskDelete, just loop forever
    while (1) {
        __NOP();
    }
}
