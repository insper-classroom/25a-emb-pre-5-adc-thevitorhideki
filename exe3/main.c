#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void mobile_average(int *vec, int n) {
    int average = 0;

    for (int i = 0; i < n; i++) {
        average += vec[i];
    }

    average /= n;

    printf("%d\n", average);
}

void process_task(void *p) {
    int data = 0;
    int vec[5] = {0, 0, 0, 0, 0};
    int count = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            vec[count] = data;

            if (count == 4) {
                count = 0;
            } else {
                count++;
            }

            mobile_average(vec, 5);
            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
