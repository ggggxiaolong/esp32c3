#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
SemaphoreHandle_t semaphore;

void task_recursive_1(void *pvParam)
{
    while (1)
    {
        ESP_LOGI("task1","start!------------");
        if (xSemaphoreTakeRecursive(semaphore, portMAX_DELAY) == pdPASS)
        {
            printf("task1 get A!\n");
            for (size_t i = 0; i < 4; i++)
            {
                printf("task1 A %d\n", i);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            if (xSemaphoreTakeRecursive(semaphore, portMAX_DELAY) == pdPASS)
            {
                printf("task1 get B!\n");
                for (size_t i = 0; i < 3; i++)
                {
                    printf("task1 B %d\n", i);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
                vTaskDelay(pdMS_TO_TICKS(2000));
                printf("task1 leave B!\n");
                xSemaphoreGiveRecursive(semaphore);
            }
            else
            {
                printf("task1 didn't get B!\n");
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            printf("task1 leave A!\n");
            xSemaphoreGiveRecursive(semaphore);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else
        {
            printf("task1 didn't get A!\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void task_recursive_2(void *pvParam)
{
    // 让Task1先执行
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (1)
    {
        ESP_LOGI("task2","start!------------");
        if (xSemaphoreTakeRecursive(semaphore, portMAX_DELAY) == pdPASS)
        {
            printf("task2 get A!\n");
            for (size_t i = 0; i < 4; i++)
            {
                printf("task2 A %d\n", i);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
            printf("task2 leave A!\n");
            xSemaphoreGiveRecursive(semaphore);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else
        {
            printf("task2 didn't get A!\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void test_recursive_case_1(void)
{
    // 创建递归互斥锁
    semaphore = xSemaphoreCreateRecursiveMutex();
    xTaskCreate(task_recursive_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(task_recursive_2, "task_2", 10240, NULL, 1, NULL);
}