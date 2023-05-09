#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

/// @brief 一直占用cpu 触发 IDL 5s 超时
/// @param pvParam 
void task_watch_dog_1(void *pvParam)
{
    while (1)
    {
        ;
    }
}

/// @brief 通过Delay 让出cpu, 避免触发 IDL 超时
/// @param pvParam 
void task_watch_dog_2(void *pvParam)
{
    while (1)
    {
        printf("task_watch_dog_2\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 将自己加入看门狗，但是不喂狗
/// @param pvParam 
void task_watch_dog_3(void *pvParam)
{
    esp_task_wdt_add(NULL);
    while (1)
    {
        printf("task_watch_dog_3\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/// @brief 将自己加入看门狗，定时喂狗
/// @param pvParam 
void task_watch_dog_4(void *pvParam)
{
    esp_task_wdt_add(NULL);
    while (1)
    {
        printf("task_watch_dog_4\n");
        esp_task_wdt_reset();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void test_watch_dog_case_1(){
    xTaskCreate(task_watch_dog_1, "task_watch_dog_1", 1024, NULL, 1, NULL);
}

void test_watch_dog_case_2(){
    xTaskCreate(task_watch_dog_2, "task_watch_dog_2", 1024, NULL, 1, NULL);
}

void test_watch_dog_case_3(){
    xTaskCreate(task_watch_dog_3, "task_watch_dog_3", 1024, NULL, 1, NULL);
}

void test_watch_dog_case_4(){
    xTaskCreate(task_watch_dog_4, "task_watch_dog_4", 1024, NULL, 1, NULL);
}