#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void task_mail_write(void *pvParam)
{
    QueueHandle_t mail = (QueueHandle_t)pvParam;
    int data = 1;
    while (1)
    {
        if (xQueueOverwrite(mail, &data) == pdPASS)
        {
            printf("write mail successful\n");
            data++;
        }
        else
        {
            printf("write mail fail\n");
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void task_mail_read(void *pvParam)
{
    QueueHandle_t mail = (QueueHandle_t)pvParam;
    int data = 0;
    char *name = pcTaskGetName(NULL);
    while (1)
    {
        if (xQueuePeek(mail, &data, portMAX_DELAY) == pdPASS)
        {
            printf("%s read mail %d\n", name, data);
        }
        else
        {
            printf("read mail fail\n");
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_mail_case_1(void)
{
    QueueHandle_t mail = xQueueCreate(1, sizeof(int));
    if (mail != NULL)
    {
        printf("mail create success\n");
        TaskHandle_t h_task_1, h_task_2, h_task_3;
        xTaskCreate(task_mail_write, "task_mail_write", 1024 * 10, (void *)mail, 1, &h_task_1);
        xTaskCreate(task_mail_read, "task_read_1", 1024 * 10, (void *)mail, 2, &h_task_2);
        xTaskCreate(task_mail_read, "task_read_2", 1024 * 10, (void *)mail, 2, &h_task_3);

        vTaskDelay(10000 / portTICK_PERIOD_MS);
        printf("finished\n");
        vTaskDelete(h_task_1);
        vTaskDelete(h_task_2);
        vTaskDelete(h_task_3);
    }
}