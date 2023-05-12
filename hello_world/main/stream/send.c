#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include <string.h>

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
        vTaskDelay(pdMS_TO_TICKS(3000));
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

void test_send_case_1(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(500, 10);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_1, "task_receive_1", 10240, NULL, 1, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}

/// @brief 修改触发唤醒task的接收长度, 第一次不到50长度也不会阻塞接收task
/// @param  
void test_send_case_2(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(500, 50);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_1, "task_receive_1", 10240, NULL, 1, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}

/// @brief 修改任务的执行顺序
/// @param 
void test_send_case_3(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(500, 50);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_1, "task_receive_1", 10240, NULL, 2, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}

/// @brief 当接收buffer的空间小于数据长度时会分包接收
/// @param 
void test_send_case_4(void)
{
    // buffer 总大小， 触发读操作的大小
    stream_buffer = xStreamBufferCreate(500, 50);
    if (stream_buffer != NULL)
    {
        printf("create stream buffer success\n");
        xTaskCreate(task_send_1, "task_send_1", 10240, NULL, 1, NULL);
        xTaskCreate(task_receive_2, "task_receive_1", 10240, NULL, 1, NULL);
    }
    else
    {
        printf("create stream buffer failed\n");
    }
}