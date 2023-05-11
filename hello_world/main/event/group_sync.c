#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
EventGroupHandle_t group;
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define WAIT_BIT (BIT_0 | BIT_1 | BIT_2)

void task_group_sync_1(void *pvParam)
{
    while (1)
    {
        printf("task 1 start\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("task 1 set bit \n");
        xEventGroupSync(
            group,
            BIT_0,
            WAIT_BIT,
            portMAX_DELAY);
        printf("task 1 is run \n");
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void task_group_sync_2(void *pvParam)
{
    while (1)
    {
        printf("task 2 start\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("task 2 set bit \n");
        xEventGroupSync(
            group,
            BIT_1,
            WAIT_BIT,
            portMAX_DELAY);
        printf("task 2 is run \n");
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void task_group_sync_3(void *pvParam)
{
    while (1)
    {
        printf("task 3 start\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("task 3 set bit \n");
        xEventGroupSync(
            group,
            BIT_2,
            WAIT_BIT,
            portMAX_DELAY);
        printf("task 3 is run \n");
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void test_group_sync_case_1(void)
{
    group = xEventGroupCreate();
    xTaskCreate(task_group_sync_1, "task1", 10240, NULL, 1, NULL);
    xTaskCreate(task_group_sync_2, "task2", 10240, NULL, 2, NULL);
    xTaskCreate(task_group_sync_3, "task3", 10240, NULL, 3, NULL);
}