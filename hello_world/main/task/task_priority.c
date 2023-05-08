#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_priority.h"

void task_priority_1(void *pvParam)
{
    while (1)
    {
        printf("task_1\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_priority_2(void *pvParam)
{
    while (1)
    {
        printf("task_2\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 相同优先级交替执行
void test_task_priority_case_1(){
    xTaskCreate(task_priority_1, "task_1", 1024 * 10, NULL, 1, NULL);
    xTaskCreate(task_priority_2, "task_2", 1024 * 10, NULL, 1, NULL);
}


/// @brief 不相同优先级 高优先级先执行
void test_task_priority_case_2(){
    xTaskCreate(task_priority_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(task_priority_2, "task_2", 10240, NULL, 2, NULL);
}



/// @brief 修改任务优先级
void test_task_priority_case_3(){
    TaskHandle_t task_1_handle = NULL;
    UBaseType_t task_1_priority = 0;
    xTaskCreate(task_priority_1, "task_1", 10240, NULL, 1, &task_1_handle);
    xTaskCreate(task_priority_2, "task_2", 10240, NULL, 2, NULL);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskPrioritySet(task_1_handle, 3);

    task_1_priority = uxTaskPriorityGet(task_1_handle);
    printf("task_1 priority is %d\n", task_1_priority);
}