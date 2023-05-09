#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void task_sender_1(void *pvParam)
{
    QueueHandle_t queue;
    queue = (QueueHandle_t)pvParam;
    int data = 111;
    while (1)
    {
        if (xQueueSend(queue, &data, 0) == pdPASS)
        {
            printf("task1 send successful\n");
        }
        else
        {
            printf("task1 send failed\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_sender_2(void *pvParam)
{
    QueueHandle_t queue;
    queue = (QueueHandle_t)pvParam;
    int data = 222;
    while (1)
    {
        if (xQueueSend(queue, &data, 0) == pdPASS)
        {
            printf("task2 send successful\n");
        }
        else
        {
            printf("task2 send failed\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_receive_1(void *pvParam)
{
    QueueSetHandle_t queue_set;
    QueueSetMemberHandle_t queue;
    queue_set = (QueueSetHandle_t)pvParam;
    int data = 0;
    while (1)
    {
        queue = xQueueSelectFromSet(queue_set, portMAX_DELAY);
        if (xQueueReceive(queue, &data, 0) == pdPASS)
        {
            printf("task receive %d\n", data);
        }
        else
        {
            printf("task receive failed\n");
        }
    }
}

void test_queue_set_case_1()
{
    QueueHandle_t queue_1 = xQueueCreate(5, sizeof(int));
    QueueHandle_t queue_2 = xQueueCreate(5, sizeof(int));
    QueueSetHandle_t queue_set = xQueueCreateSet( 5 + 5);
    if (queue_1 != NULL && queue_2 != NULL && queue_set != NULL)
    {
        xQueueAddToSet(queue_1, queue_set);
        xQueueAddToSet(queue_2, queue_set);
        TaskHandle_t h_task_1, h_task_2, h_task_3;
        xTaskCreate(task_sender_1, "task_sender_1", 1024 * 5, (void *)queue_1, 1, &h_task_1);
        xTaskCreate(task_sender_2, "task_sender_2", 1024 * 5, (void *)queue_2, 1, &h_task_2);
        xTaskCreate(task_receive_1, "task_receive_1", 1024 * 5, (void *)queue_set, 1, &h_task_3);

        vTaskDelay(10000 / portTICK_PERIOD_MS);
        printf("finished\n");
        vTaskDelete(h_task_1);
        vTaskDelete(h_task_2);
        vTaskDelete(h_task_3);
    }
}