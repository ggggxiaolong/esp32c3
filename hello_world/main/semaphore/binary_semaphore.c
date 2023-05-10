#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
SemaphoreHandle_t semaphore;
int count = 0;

void semaphore_task_1(void *pvParam)
{
    const char *name = pcTaskGetName(NULL);
    while (1)
    {
        if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdPASS)
        {
            for (size_t i = 0; i < 10; i++)
            {
                count++;
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                printf("%s: %d\n", name, count);
            }
            xSemaphoreGive(semaphore);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_semaphore_task_case_1(void)
{
    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore);
    xTaskCreate(semaphore_task_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(semaphore_task_1, "task_2", 10240, NULL, 1, NULL);
}