#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
SemaphoreHandle_t semaphore;

void task_car_in(void *pvParam)
{
    UBaseType_t count;
    while (1)
    {
        count = uxSemaphoreGetCount(semaphore);
        printf("car space: %d\n", count);
        if (count > 0 && xSemaphoreTake(semaphore, 0) == pdPASS)
        {
            printf("car in ++++\n");
        }
        else
        {
            printf("car is wait ==== \n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_car_out(void *pvParam)
{
    while (1)
    {
        vTaskDelay(6000 / portTICK_PERIOD_MS);
        printf("car out ----\n");
        xSemaphoreGive(semaphore);
    }
}

void test_count_semaphore_case_1()
{
    semaphore = xSemaphoreCreateCounting(5, 5);
    TaskHandle_t h_task_1, h_task_2;
    xTaskCreate(task_car_in, "task_car_in", 10240, NULL, 1, &h_task_1);
    xTaskCreate(task_car_out, "task_car_out", 10240, NULL, 1, &h_task_2);
}