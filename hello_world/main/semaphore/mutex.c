#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
SemaphoreHandle_t semaphore;

void task_mutex_1(void *pvParam)
{
    printf("task1 start\n");
    while (1)
    {
        if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdPASS)
        {
            printf("task1 get mutex\n");
            for (size_t i = 0; i < 10; i++)
            {
                printf("task1 tick %d\n", i);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            printf("task1 leave\n");
            xSemaphoreGive(semaphore);
        }
        else
        {
            printf("task1 didn't get mutex\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_mutex_2(void *pvParam)
{
    printf("task2 start\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (1)
    {
        ;
    }
}

void task_mutex_3(void *pvParam)
{
    printf("task3 start\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (1)
    {
        // 注意这里不能设置等待时间为0
        if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdPASS)
        {
            printf("task3 get mutex\n");
            for (size_t i = 0; i < 3; i++)
            {
                printf("task3 tick %d\n", i);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            printf("task3 leave\n");
            xSemaphoreGive(semaphore);
        }
        else
        {
            printf("task3 didn't get mutex\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_mutex_case_1(void)
{
    semaphore = xSemaphoreCreateMutex();
    TaskHandle_t h_task_1, h_task_2, h_task_3;
    xTaskCreate(task_mutex_1, "task_mutex_1", 10240, NULL, 1, &h_task_1);
    xTaskCreate(task_mutex_2, "task_mutex_2", 10240, NULL, 2, &h_task_2);
    xTaskCreate(task_mutex_3, "task_mutex_3", 10240, NULL, 3, &h_task_3);
    // main 函数丧失优先级，无法获取时间片
}