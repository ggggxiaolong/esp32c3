#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "create_task.h"

/// @brief Task
/// @param pvParam 传参
void creat_task_1(void *pvParam)
{
    while (1)
    {
        printf("Hello world! In Task1\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 自己删除自己的Task
/// @param pvParam 传参
void creat_task_2(void *pvParam)
{
    printf("Hello world! First\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Hello world! Second\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Hello world! Third\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // 自己删除自己传 NULL
    vTaskDelete(NULL);
}

void test_create_task_case_1()
{
    printf("test_create_task_case_1 \n");
    TaskHandle_t task_handle = NULL;
    xTaskCreate(creat_task_1, "creat_task_1", 1024, NULL, 1, &task_handle);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    if (task_handle != NULL)
    {
        vTaskDelete(task_handle);
    }
}


void test_create_task_case_2()
{
    printf("test_create_task_case_2 \n");
    xTaskCreate(creat_task_2, "creat_task_2", 1024, NULL, 1, NULL);
}