#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_notify_1(void *pvParam)
{
    while (1)
    {
        printf("task1 is waiting \n");
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("task1 release task2 \n");
        xTaskNotifyGive(task_2);
    }
}

void task_notify_2(void *pvParam)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("task2 release task1 \n");
        xTaskNotifyGive(task_1);
        printf("task2 is waiting \n");
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

void test_notify_case_1(void)
{
    xTaskCreate(task_notify_1, "task1", 10240, NULL, 1, &task_1);
    xTaskCreate(task_notify_2, "task2", 10240, NULL, 1, &task_2);
}