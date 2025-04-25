#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

const int taskDelay = 100;
const int taskSize = 192;
SemaphoreHandle_t mutex;

void vSafePrint(char *out) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    puts(out);
    xSemaphoreGive(mutex);
}

void vTaskSMP(void *pvParameters) {
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    UBaseType_t mask = vTaskCoreAffinityGet(handle);
    char *name = pcTaskGetName(handle);
    char out[40];

    for (;;) {
        sprintf(out,"Task %s core %u tick %u mask %d", name, get_core_num(),
                xTaskGetTickCount(), mask);
        vSafePrint(out);
        vTaskDelay(taskDelay);
    }
}

void main() {
    stdio_init_all();
    mutex = xSemaphoreCreateMutex();

    TaskHandle_t handleA;
    // Pin B to core 1
    UBaseType_t affinityMaskB = 1 << 1;

    xTaskCreate(vTaskSMP, "A", taskSize, NULL, 1, &handleA);
    xTaskCreateAffinitySet(vTaskSMP, "B", taskSize, NULL, 1, affinityMaskB, NULL);
    xTaskCreate(vTaskSMP, "C", taskSize, NULL, 1, NULL);
    xTaskCreate(vTaskSMP, "D", taskSize, NULL, 1, NULL);
    // Pin A to core 0
    vTaskCoreAffinitySet(handleA, (1 << 0));
    vTaskStartScheduler();
}
