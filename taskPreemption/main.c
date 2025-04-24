#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

const int togglePeriod = 3000;
const int taskDelay = 1000;
const int taskSize = 192;
SemaphoreHandle_t mutex;

void vSafePrint(char *out) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    puts(out);
    xSemaphoreGive(mutex);
}

void vTaskLowPriority(void *pvParameters) {
    char out[70];
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    UBaseType_t mask = vTaskCoreAffinityGet(handle);
    char *name = pcTaskGetName(handle);
    UBaseType_t priority = uxTaskPriorityGet(NULL);    

    TickType_t tickCount;

    while (1) {
        vTaskPreemptionDisable(NULL);
        tickCount = xTaskGetTickCount();
        sprintf(out, "Task %s priority %u core %d mask %d tick %d - no preempt",
                name, priority, get_core_num(), mask, tickCount);
        vSafePrint(out);
        while (xTaskGetTickCount() - tickCount <= togglePeriod) {
            // First 3 seconds, no preemption
        }

        tickCount = xTaskGetTickCount();
        sprintf(out,
            "Task %s priority %u core %d mask %d tick %d - preemptable",
             name, priority, get_core_num(), mask, tickCount);
        vSafePrint(out);

        vTaskPreemptionEnable(NULL);
        while (xTaskGetTickCount() - tickCount <= togglePeriod) {
            // Second 3 seconds, can be preempted
        }
    }
}

void vTaskHighPriority(void *pvParameters) {
	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	UBaseType_t mask = vTaskCoreAffinityGet(handle);
	char *name = pcTaskGetName(handle);
    UBaseType_t priority = uxTaskPriorityGet(NULL);
	char out[70];

	while (1) {
        // Every 1 second, run once
		sprintf(out, "Task %s priority %u core %d mask %d tick %d", name,
                priority, get_core_num(), mask, xTaskGetTickCount());
		vSafePrint(out);
		vTaskDelay(taskDelay);
	}
}

void main() {
	stdio_init_all();
	mutex = xSemaphoreCreateMutex();

	TaskHandle_t handleA;

    // Lower priority task A runs constantly on core 0, two higher priority
    // tasks run periodically.
	xTaskCreate(vTaskLowPriority, "A", taskSize, NULL, 1, &handleA);
	xTaskCreate(vTaskHighPriority, "B", taskSize, NULL, 2, NULL);
    xTaskCreate(vTaskHighPriority, "C", taskSize, NULL, 2, NULL);
    // Pin A to core 0.
	vTaskCoreAffinitySet(handleA, (1 << 0));
	vTaskStartScheduler();
}
