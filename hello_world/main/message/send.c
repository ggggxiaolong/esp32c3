#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include <string.h>

MessageBufferHandle_t message_buffer;
void task_message_send_1(void *pvParam)
{
    char buf[50];
    size_t msg_len, send_len;
    for (size_t i = 0; i < 3; i++)
    {
        msg_len = sprintf(buf, "Hello this is MrTan, No.%d", i);
        send_len = xMessageBufferSend(message_buffer, buf, msg_len, portMAX_DELAY);
        printf("msg length: %d,send length: %d\n", msg_len, send_len);
    }
    vTaskDelete(NULL);
}

void task_message_receive_1(void *pvParam)
{
    size_t msg_len = 0;
    char buf[150];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        msg_len = xMessageBufferReceive(message_buffer, (void *)buf, sizeof(buf), portMAX_DELAY);
        printf("receive %d msg: %s\n", msg_len, buf);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_message_receive_2(void *pvParam)
{
    size_t msg_len = 0;
    char buf[10];
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        msg_len = xMessageBufferReceive(message_buffer, (void *)buf, sizeof(buf), portMAX_DELAY);
        printf("receive %d msg: %s\n", msg_len, buf);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void test_message_case_1(void)
{
    message_buffer = xMessageBufferCreate(200);
    xTaskCreate(task_message_send_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(task_message_receive_1, "task_2", 10240, NULL, 1, NULL);
}

/// @brief 当接收buffer的数据长度小于数据长度则接收不到数据
/// @param  
void test_message_case_2(void)
{
    message_buffer = xMessageBufferCreate(200);
    xTaskCreate(task_message_send_1, "task_1", 10240, NULL, 1, NULL);
    xTaskCreate(task_message_receive_2, "task_2", 10240, NULL, 1, NULL);
}