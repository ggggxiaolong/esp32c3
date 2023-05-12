#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include <string.h>
#include "esp_log.h"

// send receive
// change triggerBytes size
// change task order

StreamBufferHandle_t stream_buffer;

void task_send_1(void *pvParam)
{
    char buf[50];
    size_t write_len = 0;
    size_t send_len = 0;
    int i = 0;
    while (1)
    {
        i++;
        write_len = sprintf(buf, "Hello this is MrTan, count %d", i);
        // stream buffer指针， 写入buffer指针， 写入长度， 等待时间
        send_len = xStreamBufferSend(stream_buffer, buf, write_len, portMAX_DELAY);
        printf("write len: %d, send len: %d\n", write_len, send_len);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void task_receive_1(void *pvParam)
{
    char buf[50];
    size_t read_len = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        // stream buffer指针， 读取buffer指针， buffer长度， 等待时间
        read_len = xStreamBufferReceive(stream_buffer, (void *)buf, sizeof(buf), portMAX_DELAY);
        printf("read len: %d, data: %s\n", read_len, buf);
    }
}

void task_receive_2(void *pvParam)
{
    char buf[10];
    size_t read_len = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        // stream buffer指针， 读取buffer指针， buffer长度， 等待时间
        read_len = xStreamBufferReceive(stream_buffer, (void *)buf, sizeof(buf), portMAX_DELAY);
        printf("read len: %d, data: %s\n", read_len, buf);
    }
}

void task_watch_1(void *pvParam){
    size_t min_size = 200;
    size_t left_space = 0;
    while (1)
    {
        left_space = xStreamBufferSpacesAvailable(stream_buffer);
        if(left_space < min_size){
            min_size = left_space;
        }
        ESP_LOGI("monitor","left_space = %d, min_size = %d",left_space,min_size);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    
}

void test_send_case_1(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(150, 100);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_1, "task_receive_1", 10240, NULL, 2, NULL);
        xTaskCreate(task_watch_1, "task_watch_1", 10240, NULL, 2, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}

void test_send_case_2(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(200, 100);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_2, "task_receive_1", 10240, NULL, 2, NULL);
        xTaskCreate(task_watch_1, "task_watch_1", 10240, NULL, 2, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}