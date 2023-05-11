#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

TaskHandle_t task_1, task_2;

void task_notify_1(void *pvParam)
{
    uint32_t notify_value;
    while (1)
    {
        // printf("task1 is waiting \n");
        // 进入时清空的位， 退出时清空的位， 当前的通知值, 等待时间
        xTaskNotifyWait(0x00, ULONG_MAX, &notify_value, portMAX_DELAY);
        xTaskNotifyGive(task_2);
        if ((notify_value & BIT_0) != 0)
        {
            printf("task1 processing bit0 \n");
        }
        if ((notify_value & BIT_1) != 0)
        {
            printf("task1 processing bit1 \n");
        }
        if ((notify_value & BIT_2) != 0)
        {
            printf("task1 processing bit2 \n");
        }
    }
}

void task_notify_2(void *pvParam)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("task2 set bit0 \n");
        xTaskNotify(task_1, BIT_0, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("task2 set bit1 \n");
        xTaskNotify(task_1, BIT_1, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("task2 set bit2 \n");
        xTaskNotify(task_1, BIT_2, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void test_notify_value_case_1(void)
{
    xTaskCreate(task_notify_1, "task1", 10240, NULL, 1, &task_1);
    xTaskCreate(task_notify_2, "task2", 10240, NULL, 1, &task_2);
}