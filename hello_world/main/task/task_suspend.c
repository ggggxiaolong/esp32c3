#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void task_suspend_1(void *pvParam)
{
    while (1)
    {
        printf("task_suspend_1 \n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_suspend_2(void *pvParam)
{
    while (1)
    {
        printf("task_suspend_2 \n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 挂起自己
/// @param pvParam
void task_suspend_3(void *pvParam)
{
    printf("task_suspend_3 \n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("task_suspend_3 \n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("task_suspend_3 \n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    vTaskSuspend(NULL);
}

void task_suspend_4(void *pvParam)
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("task_suspend_4 start\n");
    vTaskSuspendAll();
    for (size_t i = 0; i < 9999; i++)
    {
        for (size_t j = 0; j < 9999; j++)
        {
            ;
        }
    }
    xTaskResumeAll();
    printf("task_suspend_4 end\n");
    vTaskDelete(NULL);
}

void test_task_suspend_case_1()
{
    printf("test_task_suspend_case_1 \n");
    TaskHandle_t task_handle;
    xTaskCreate(task_suspend_1, "task_suspend_1", 1024, NULL, 1, &task_handle);
    xTaskCreate(task_suspend_2, "task_suspend_2", 1024, NULL, 1, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    printf("suspend task1 \n");
    vTaskSuspend(task_handle);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("resume task1 \n");
    vTaskResume(task_handle);
}

void test_task_suspend_case_2()
{
    printf("test_task_suspend_case_2 \n");
    TaskHandle_t task_handle;
    xTaskCreate(task_suspend_3, "task_suspend_3", 1024, NULL, 1, &task_handle);
    xTaskCreate(task_suspend_2, "task_suspend_2", 1024, NULL, 1, NULL);
}

void test_task_suspend_case_3()
{
    printf("test_task_suspend_case_3 \n");
    xTaskCreate(task_suspend_1, "task_suspend_1", 1024, NULL, 1, NULL);
    xTaskCreate(task_suspend_4, "task_suspend_4", 1024, NULL, 1, NULL);
}