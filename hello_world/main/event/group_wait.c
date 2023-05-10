#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
EventGroupHandle_t group;
#define BIT_0 ( 1 << 0 )
#define BIT_4 ( 1 << 4 )

void task_group_wait_1(void *pvParam)
{
    while (1)
    {
        printf("task 1 start\n");
        xEventGroupWaitBits(
            group,
            BIT_0 | BIT_4, //标记位
            pdTRUE, // 是否清楚位
            pdTRUE, // 是否全置为1
            portMAX_DELAY // 最大等待时间
        );
        printf("task 1 BIT_0 AND BIT_4 was set\n");
    }
    
}

void task_group_wait_2(void *pvParam)
{
    while (1)
    {
        printf("task 2 start\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("task 2 set BIT_0\n");
        xEventGroupSetBits(group, BIT_0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("task 2 set BIT_4\n");
        xEventGroupSetBits(group, BIT_4);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_group_wait_case(void)
{
    group = xEventGroupCreate();
    xTaskCreate(task_group_wait_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(task_group_wait_2, "task_", 10240, NULL, 1, NULL);
}