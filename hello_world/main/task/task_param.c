#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_param.h"

/// @brief 传递整数
/// @param pvParam 参数指针
void task_param_int(void *pvParam)
{
    int *num;
    num = (int *)pvParam;

    printf("task_param param is %d\n", *num);

    vTaskDelete(NULL);
}

/// @brief 传递整数数组
/// @param pvParam 参数指针
void task_param_arr(void *pvParam)
{
    int *num;
    num = (int *)pvParam;

    printf("task_param_arr param array 0 is %d\n", *num);
    printf("task_param_arr param array 1 is %d\n", *(num + 1));
    printf("task_param_arr param array 2 is %d\n", *(num + 2));

    vTaskDelete(NULL);
}

/// @brief 传递结构体
/// @param pvParam 参数指针
void task_param_struct(void *pvParam)
{
    TransParam *p_param;
    p_param = (TransParam *)pvParam;

    printf("task_param_struct param1 is %d\n", p_param->param1);
    printf("task_param_struct param2 is %d\n", p_param->param2);

    vTaskDelete(NULL);
}

/// @brief 传递结构体
/// @param pvParam 参数指针
void task_param_string(void *pvParam)
{
    char *p_msg;
    p_msg = (char *)pvParam;

    printf("message is  %s\n", p_msg);

    vTaskDelete(NULL);
}

void test_task_param_case_1()
{
    printf("test_task_param_case_1\n");
    int param1 = 5;
    xTaskCreate(task_param_int, "task_param_int", 10240, (void *)&param1, 1, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void test_task_param_case_2()
{
    printf("test_task_param_case_3\n");
    int param1[] = {3,4,5};
    xTaskCreate(task_param_arr, "task_param_arr", 10240, (void *)&param1, 1, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void test_task_param_case_3()
{
    printf("test_task_param_case_3\n");
    TransParam param1 = {6,1};
    xTaskCreate(task_param_struct, "task_param_arr", 10240, (void *)&param1, 1, NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


void test_task_param_case_4()
{
    printf("test_task_param_case_4\n");
    const char *param1 = "hello this is MrTan";
    xTaskCreate(task_param_string, "task_param_arr", 10240, (void *)param1, 1, NULL);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    esp_restart();
}