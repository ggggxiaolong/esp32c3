#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "deliver_data.h"

// int
// struct
// point

void task_deliver_send_data_1(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    int data = 0;
    while (1)
    {
        if (xQueueSend(p_queue, &data, 0) == pdPASS)
        {
            printf("send successful!\n");
            data++;
            if (data == 10)
            {
                data = 0;
            }
        }
        else
        {
            printf("send fail!\n");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void task_deliver_send_data_2(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    UData data = {1, 0};
    while (1)
    {
        if (xQueueSend(p_queue, &data, 0) == pdPASS)
        {
            printf("send successful!\n");
            data.data += 1;
            if (data.data == 10)
            {
                data.data = 0;
            }
        }
        else
        {
            printf("send fail!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_deliver_send_data_3(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    char *data;
    int i = 0;
    while (1)
    {
        data = (char *)malloc(50);
        snprintf(data, 50, "Today is 2023-05-09 %d ", i);
        if (xQueueSend(p_queue, &data, 0) == pdPASS)
        {
            printf("send successful!\n");
            i += 1;
        }
        else
        {
            printf("send fail!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_deliver_receive_data_1(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    int data = 0;
    while (1)
    {
        if (uxQueueMessagesWaiting(p_queue) != 0)
        {
            if (xQueueReceive(p_queue, &data, 0) == pdPASS)
            {
                printf("receive %d!\n", data);
            }
            else
            {
                printf("send fail!\n");
            }
        }
        else
        {
            printf("no data!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_deliver_receive_data_2(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    UData data = {0, 0};
    while (1)
    {
        if (uxQueueMessagesWaiting(p_queue) != 0)
        {
            if (xQueueReceive(p_queue, &data, 0) == pdPASS)
            {
                printf("receive id: %d, data: %d!\n", data.id, data.data);
            }
            else
            {
                printf("send fail!\n");
            }
        }
        else
        {
            printf("no data!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task_deliver_receive_data_3(void *pvParam)
{
    QueueHandle_t p_queue;
    p_queue = (QueueHandle_t)pvParam;

    char *data;
    while (1)
    {
        if (uxQueueMessagesWaiting(p_queue) != 0)
        {
            if (xQueueReceive(p_queue, &data, 0) == pdPASS)
            {
                printf("receive %s!\n", data);
                free(data);
            }
            else
            {
                printf("send fail!\n");
            }
        }
        else
        {
            printf("no data!\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_deliver_data_case_1(void)
{
    QueueHandle_t queue = xQueueCreate(5, sizeof(int));
    if (queue != NULL)
    {
        printf("queue create successful!\n");

        TaskHandle_t h_task_1, h_task_2;
        xTaskCreate(task_deliver_send_data_1, "send_task", 10240, (void *)queue, 1, &h_task_1);
        xTaskCreate(task_deliver_receive_data_1, "receive_task", 10240, (void *)queue, 1, &h_task_2);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        vTaskDelete(h_task_1);
        vTaskDelete(h_task_2);
    }
    else
    {
        printf("queue create failed!\n");
    }
}

void test_deliver_data_case_2(void)
{
    QueueHandle_t queue = xQueueCreate(5, sizeof(UData));
    if (queue != NULL)
    {
        printf("queue create successful!\n");

        TaskHandle_t h_task_1, h_task_2;
        xTaskCreate(task_deliver_send_data_2, "send_task", 10240, (void *)queue, 1, &h_task_1);
        xTaskCreate(task_deliver_receive_data_2, "receive_task", 10240, (void *)queue, 1, &h_task_2);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        vTaskDelete(h_task_1);
        vTaskDelete(h_task_2);
    }
    else
    {
        printf("queue create failed!\n");
    }
}

void test_deliver_data_case_3(void)
{
    QueueHandle_t queue = xQueueCreate(5, sizeof(char *));
    if (queue != NULL)
    {
        printf("queue create successful!\n");

        TaskHandle_t h_task_1, h_task_2;
        xTaskCreate(task_deliver_send_data_3, "send_task", 10240, (void *)queue, 1, &h_task_1);
        xTaskCreate(task_deliver_receive_data_3, "receive_task", 10240, (void *)queue, 1, &h_task_2);
        UBaseType_t left;
        for (size_t i = 0; i < 10; i++)
        {
            left = xPortGetFreeHeapSize();
            printf("free heap size %d\n", left);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        vTaskDelete(h_task_1);
        vTaskDelete(h_task_2);
    }
    else
    {
        printf("queue create failed!\n");
    }
}