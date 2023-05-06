#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "create_task.h"

/// @brief Task
/// @param pvParam 传参
void my_task(void *pvParam)
{
    while (1)
    {
        printf("Hello world! In Task1");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 自己删除自己的Task
/// @param pvParam 传参
void my_task_2(void *pvParam)
{
    printf("Hello world! First");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Hello world! Second");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Hello world! Third");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // 自己删除自己传 NULL
    vTaskDelete(NULL);
}