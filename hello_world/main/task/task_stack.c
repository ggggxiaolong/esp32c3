#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_stack_1(void *pvParam)
{
    while (1)
    {
        printf("task_stack_1\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_stack_2(void *pvParam)
{
    while (1)
    {
        // printf("task_stack_1");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_stack_3(void *pvParam)
{
    int *p;
    p = (int *)pvParam;
    while (1)
    {
        printf("task_stack_3 %d\n", *p);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_task_stack_case_1()
{
    int param = 3;
    TaskHandle_t task_handle_1, task_handle_2, task_handle_3;
    xTaskCreate(task_stack_1, "task_stack_1", 520, NULL, 1, &task_handle_1);
    xTaskCreate(task_stack_2, "task_stack_2", 256, NULL, 1, &task_handle_2);
    xTaskCreate(task_stack_3, "task_stack_3", 1024 * 2, (void *)&param, 1, &task_handle_3);

    UBaseType_t task_1_left, task_2_left, task_3_left;
    while (1)
    {
        task_1_left = uxTaskGetStackHighWaterMark(task_handle_1);
        task_2_left = uxTaskGetStackHighWaterMark(task_handle_2);
        task_3_left = uxTaskGetStackHighWaterMark(task_handle_3);
        printf("task1: %d, task2: %d, task3: %d\n", task_1_left, task_2_left, task_3_left);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}